/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "../CustomMods.h"
#include "CvLuaSupport.h"
#include "CvLuaArea.h"
#include "CvLuaCity.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"
#include "../CvMinorCivAI.h"
#include "../CvUnitCombat.h"

#pragma warning(disable:4800 ) //forcing value to bool 'true' or 'false'

//Utility macro for registering methods
#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);

//------------------------------------------------------------------------------
void CvLuaUnit::HandleMissingInstance(lua_State* L)
{
	luaL_error(L, "Instance no longer exists.");
}
//------------------------------------------------------------------------------
void CvLuaUnit::PushMethods(lua_State* L, int t)
{
	Method(IsNone);
	Method(Convert);
	Method(Upgrade);
	Method(UpgradeTo);
	Method(Kill);

	Method(IsActionRecommended);
	Method(IsBetterDefenderThan);

	Method(CanDoCommand);
	Method(DoCommand);

	Method(GetPathEndTurnPlot);
	Method(GeneratePath);
	Method(GetActivePath);
	Method(GetWaypointPath);
	Method(GeneratePathToNextWaypoint);
	Method(GetMeleeAttackFromPlot);
	Method(GetPotentialRangeAttackTargetPlots);
	Method(GetPotentialRangeAttackOriginPlots);

	Method(CanEnterTerritory);
	Method(GetDeclareWarRangeStrike);
	Method(CanMoveOrAttackInto);
	Method(CanMoveThrough);
	Method(JumpToNearestValidPlot);

	Method(GetCombatDamage);
	Method(GetFireSupportUnit);

	Method(CanAutomate);
	Method(CanScrap);
	Method(GetScrapGold);
	Method(CanGift);
	Method(CanDistanceGift);
	Method(IsUnitValidGiftForCityStateQuest);
	Method(CanLoadUnit);
	Method(CanLoad);
	Method(CanUnload);
	Method(CanUnloadAll);
	Method(CanHold);
	Method(CanSleep);
	Method(CanFortify);
	Method(CanAirPatrol);
	Method(IsEmbarked);
	Method(SetEmbarked);
	Method(CanHeal);
	Method(CanSentry);
	Method(CanEmbark);
	Method(CanDisembark);
	Method(CanEmbarkOnto);
	Method(CanDisembarkOnto);
	Method(CanRebaseAt);
	Method(RebaseAt);
	Method(Embark);

	Method(IsRangeAttackIgnoreLOS);

	Method(CanAirlift);
	Method(CanAirliftAt);

	Method(IsNukeVictim);
	Method(CanNuke);
	Method(CanNukeAt);

	Method(CanRangeStrike);
	Method(CanRangeStrikeAt);
	Method(CanEverRangeStrikeAt);

	Method(CanParadrop);
	Method(CanParadropAt);

	Method(CanMakeTradeRoute);
	Method(CanMakeTradeRouteAt);

	Method(CanPlunderTradeRoute);
	Method(PlunderTradeRoute);

	Method(CanCreateGreatWork);
	Method(CreateGreatWork);
	Method(greatperson);
	Method(GetCombatVersusOtherReligionOwnLands);
	Method(GetCombatVersusOtherReligionTheirLands);

	Method(GetExoticGoodsGoldAmount);
	Method(GetExoticGoodsXPAmount);

	Method(CanPillage);

	Method(IsSelected);

	Method(CanFound);
	Method(CanJoin);
	Method(CanConstruct);

	Method(CreateFreeLuxuryCheckCopy);
	Method(CreateFreeLuxuryCheck);
	Method(CanDiscover);
	Method(GetDiscoverAmount);
	Method(GetHurryProduction);
	Method(GetTradeGold);
	Method(GetTradeInfluence);
	Method(GetRestingPointChange);
	Method(CanTrade);
	Method(CanBuyCityState);
	Method(CanRepairFleet);
#if defined(MOD_GLOBAL_SEPARATE_GREAT_ADMIRAL)
	Method(CanChangePort);
#endif
	Method(CanBuildSpaceship);

	Method(CanGoldenAge);
	Method(GetGoldenAgeTurns);
	Method(GetGAPAmount);
	Method(GetGivePoliciesCulture);
	Method(GetBlastTourism);
	Method(CanBuild);
	Method(CanLead);
	Method(Lead);
	Method(CanGiveExperience);
	Method(GiveExperience);

	Method(CanPromote);
	Method(Promote);

	Method(GetUpgradeUnitType);
	Method(UpgradePrice);
	Method(CanUpgradeRightNow);
	Method(CanUpgradeTo);
#if defined(MOD_GLOBAL_CS_UPGRADES)
	Method(CanUpgradeInTerritory);
#endif
	Method(GetNumResourceNeededToUpgrade);
	Method(GetNumResourceTotalNeededToUpgrade);

	Method(GetHandicapType);
	Method(GetCivilizationType);
	Method(GetSpecialUnitType);
	Method(GetCaptureUnitType);
	Method(GetUnitCombatType);
#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
	Method(GetUnitPromotionType);
#endif
	Method(GetUnitAIType);
	Method(SetUnitAIType);
	Method(GetDomainType);
	Method(GetInvisibleType);
	Method(GetSeeInvisibleType);
	Method(GetDropRange);

	Method(FlavorValue);
	Method(IsBarbarian);

	Method(IsHuman);
	Method(VisibilityRange);

	Method(MaxMoves);
	Method(MovesLeft);

	Method(CanMove);
	Method(HasMoved);

	Method(IsLinked);
	Method(IsLinkedLeader);
	Method(IsGrouped);
	Method(CanLinkUnits);
	Method(LinkUnits);
	Method(UnlinkUnits);
	Method(MoveLinkedLeader);
	Method(DoGroupMovement);

	Method(GetSquadNumber);
	Method(AssignToSquad);
	Method(RemoveFromSquad);
	Method(DoSquadMovement);
	Method(SetSquadEndMovementType);

	Method(Range);
	Method(NukeDamageLevel);

	Method(CanBuildRoute);
	Method(GetBuildType);
	Method(WorkRate);

	Method(GetImprovementBuildType);
	Method(GetRouteBuildType);

#if defined(MOD_CIV6_WORKER)
	Method(GetBuilderStrength);
#endif

	Method(IsNoBadGoodies);
	Method(IsOnlyDefensive);
	Method(IsNoAttackInOcean);

	Method(IsNoCapture);
	Method(IsRivalTerritory);
	Method(IsFound);
	Method(IsFoundAbroad);
	Method(IsWork);
	Method(IsGoldenAge);
	Method(CanCoexistWithEnemyUnit);
#if defined(MOD_BALANCE_CORE)
	Method(IsContractUnit);
	Method(IsSpecificContractUnit);
	Method(GetContractUnit);
#endif
	Method(IsGreatPerson);

	Method(IsFighting);
	Method(IsAttacking);
	Method(IsDefending);
	Method(IsInCombat);

	Method(GetMaxHitPointsBase);
	Method(SetMaxHitPointsBase);
	Method(ChangeMaxHitPointsBase);

	Method(GetMaxHitPoints);
	Method(GetCurrHitPoints);
	Method(IsHurt);
	Method(IsDead);
	Method(IsDelayedDeath);
	Method(SetBaseCombatStrength);
	Method(GetBaseCombatStrength);

	Method(GetMaxAttackStrength);
	Method(GetMaxDefenseStrength);
	Method(GetEmbarkedUnitDefense);

	Method(IsCombatUnit);
	Method(IsCanDefend);
	Method(CanSiege);
	Method(IsCanAttackWithMove);
	Method(IsCanAttackRanged);
	Method(IsCanAttack);
	Method(IsCanAttackWithMoveNow);
	Method(IsEnemyInMovementRange);

	Method(IsTrade);
	Method(GetTradeRouteIndex);
	Method(IsRecalledTrader);
	Method(RecallTrader);
	Method(EndTrader);

	Method(GetBaseRangedCombatStrength);
	Method(SetBaseRangedCombatStrength);
	Method(GetDamageCombatModifier);
	Method(GetMaxRangedCombatStrength);
	Method(CanAirAttack);
	Method(CanAirDefend);
	Method(GetAirCombatDamage);
	Method(GetRangeCombatDamage);
	Method(GetAirStrikeDefenseDamage);
	Method(GetBestInterceptor);
	Method(GetInterceptorCount);
	Method(GetBestSeaPillageInterceptor);
	Method(GetCaptureChance);

	Method(IsAutomated);
	Method(IsWaiting);
	Method(IsFortifyable);
	Method(IsEverFortifyable);
	Method(FortifyModifier);
	Method(ExperienceNeeded);
	Method(MaxXPValue);
	Method(IsRanged);
	Method(IsMustSetUpToRangedAttack);
	Method(CanSetUpForRangedAttack);
	Method(IsSetUpForRangedAttack);
	Method(NoDefensiveBonus);
	Method(IgnoreBuildingDefense);
	Method(CanMoveImpassable);
	Method(CanMoveAllTerrain);
	Method(IsHoveringUnit);
	Method(FlatMovementCost);
	Method(IgnoreTerrainCost);
	Method(IgnoreTerrainDamage);
	Method(IgnoreFeatureDamage);
	Method(ExtraTerrainDamage);
	Method(ExtraFeatureDamage);
	Method(GetMovementRules);
	Method(GetZOCStatus);
	Method(GetWithdrawChance);
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
	Method(GetNearbyImprovementCombatBonus);
	Method(GetNearbyImprovementBonusRange);
	Method(GetCombatBonusImprovement);
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
	Method(CanCrossMountains);
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
	Method(CanCrossOceans);
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
	Method(CanCrossIce);
#endif
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
	Method(IsGGFromBarbarians);
#endif
	Method(IsNeverInvisible);
	Method(IsInvisible);
	Method(IsNukeImmune);
	Method(IsRangeAttackOnlyInDomain);
	Method(IsCityAttackOnly);

	Method(GetAirInterceptRange);
	Method(MaxInterceptionProbability);
	Method(CurrInterceptionProbability);
	Method(EvasionProbability);
	Method(WithdrawalProbability);

	Method(GetAdjacentModifier);
	Method(GetNoAdjacentUnitModifier);
	Method(GetAttackModifier);
	Method(GetDefenseModifier);
	Method(GetRangedDefenseModifier);
	Method(GetRangedAttackModifier);
	Method(GetGarrisonRangedAttackModifier);
	Method(CityAttackModifier);
	Method(CityDefenseModifier);
	Method(HillsAttackModifier);
	Method(HillsDefenseModifier);
	Method(RoughAttackModifier);
	Method(OpenAttackModifier);
	Method(OpenRangedAttackModifier);
	Method(OpenDefenseModifier);
	Method(RoughRangedAttackModifier);
	Method(AttackFortifiedModifier);
	Method(AttackWoundedModifier);
	Method(AttackFullyHealedModifier);
	Method(AttackAbove50Modifier);
	Method(AttackBelow50Modifier);
	Method(IsRangedFlankAttack);
	Method(FlankAttackModifier);
	Method(RoughDefenseModifier);
	Method(RoughFromModifier);
	Method(OpenFromModifier);
	Method(TerrainAttackModifier);
	Method(TerrainDefenseModifier);
	Method(FeatureAttackModifier);
	Method(FeatureDefenseModifier);
	Method(UnitClassAttackModifier);
	Method(UnitClassDefenseModifier);
	Method(UnitCombatModifier);
#if defined(MOD_BALANCE_CORE)
	Method(PerAdjacentUnitCombatModifier);
	Method(PerAdjacentUnitCombatAttackMod);
	Method(PerAdjacentUnitCombatDefenseMod);
	Method(IsMounted);
	Method(IsStrongerDamaged);
	Method(GetMultiAttackBonus);
	Method(GetMultiAttackBonusCity);
	Method(BarbarianCombatBonus);
#endif
	Method(DomainModifier);
	Method(DomainAttackPercent);
	Method(DomainDefensePercent);
	Method(GetStrategicResourceCombatPenalty);
	Method(GetUnhappinessCombatPenalty);
	Method(AirSweepCombatMod);
	Method(GetEmbarkDefensiveModifier);
	Method(CapitalDefenseModifier);
	Method(CapitalDefenseFalloff);

	Method(SpecialCargo);
	Method(DomainCargo);
	Method(CargoSpace);
	Method(ChangeCargoSpace);
	Method(IsFull);
	Method(CargoSpaceAvailable);
	Method(HasCargo);
	Method(CanCargoAllMove);
	Method(GetUnitAICargo);
	Method(GetID);

	Method(GetHotKeyNumber);
	Method(SetHotKeyNumber);

	Method(GetX);
	Method(GetY);
	Method(SetXY);
	Method(At);
	Method(AtPlot);
	Method(GetPlot);
	Method(GetArea);
	Method(GetReconPlot);
	Method(SetReconPlot);

	Method(GetGameTurnCreated);
	Method(GetLastMoveTurn);

	Method(GetDamage);
	Method(SetDamage);
	Method(ChangeDamage);
	Method(GetMoves);
	Method(SetMoves);
	Method(ChangeMoves);
	Method(FinishMoves);
	Method(IsImmobile);

	Method(GetExperience);
	Method(SetExperience);
	Method(ChangeExperience);
	Method(GetExperienceTimes100);
	Method(SetExperienceTimes100);
	Method(ChangeExperienceTimes100);
	Method(GetLevel);
	Method(SetLevel);
	Method(ChangeLevel);
	Method(GetFacingDirection);
	Method(RotateFacingDirectionClockwise);
	Method(RotateFacingDirectionCounterClockwise);
	Method(GetCargo);
	Method(GetFortifyTurns);
	Method(GetBlitzCount);
	Method(IsBlitz);
	Method(GetAmphibCount);
	Method(IsAmphib);
	Method(GetRiverCrossingNoPenaltyCount);
	Method(IsRiverCrossingNoPenalty);
	Method(IsEnemyRoute);
	Method(IsAlwaysHeal);
	Method(IsHealOutsideFriendly);
	Method(IsHillsDoubleMove);
	Method(IsGarrisoned);
	Method(GetGarrisonedCity);

	Method(GetExtraVisibilityRange);
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
	Method(GetExtraReconRange);
#endif
	Method(GetExtraMoves);
	Method(GetExtraMoveDiscount);
	Method(GetExtraRange);
	Method(GetExtraIntercept);
	Method(GetExtraEvasion);
	Method(GetExtraWithdrawal);
	Method(GetExtraEnemyHeal);
	Method(GetExtraNeutralHeal);
	Method(GetExtraFriendlyHeal);

	Method(GetSameTileHeal);
	Method(GetAdjacentTileHeal);

	Method(GetExtraCombatPercent);
	Method(GetFriendlyLandsModifier);
	Method(GetFriendlyLandsAttackModifier);
	Method(GetOutsideFriendlyLandsModifier);
	Method(GetExtraCityAttackPercent);
	Method(GetExtraCityDefensePercent);
	Method(GetExtraHillsAttackPercent);
	Method(GetExtraHillsDefensePercent);

	Method(GetExtraOpenAttackPercent);
	Method(GetExtraOpenRangedAttackMod);
	Method(GetExtraRoughAttackPercent);
	Method(GetExtraRoughRangedAttackMod);
	Method(GetExtraAttackFortifiedMod);
	Method(GetExtraAttackWoundedMod);
	Method(GetExtraOpenDefensePercent);

	Method(GetPillageChange);
	Method(GetUpgradeDiscount);
	Method(GetExperiencePercent);

	Method(IsOutOfAttacks);
	Method(SetMadeAttack);
	Method(isOutOfInterceptions);
	Method(SetMadeInterception);

	Method(IsPromotionReady);
	Method(SetPromotionReady);
	Method(GetOwner);
	Method(GetVisualOwner);
	Method(GetCombatOwner);
	Method(GetOriginalOwner);
	Method(SetOriginalOwner);
	Method(GetTeam);
	Method(GetUnitFlagIconOffset);
	Method(GetUnitPortraitOffset);

	Method(GetUnitType);
	Method(GetUnitClassType);
	Method(GetLeaderUnitType);
	Method(SetLeaderUnitType);
	Method(IsNearGreatGeneral);
	Method(GetGreatGeneralAuraBonus);
	Method(IsStackedGreatGeneral);
	Method(IsIgnoreGreatGeneralBenefit);
	Method(GetReverseGreatGeneralModifier);
	Method(GetGreatGeneralCombatModifier);
	Method(GetAuraRange);
	Method(GetAuraEffect);
	Method(IsNearSapper);
#if defined(MOD_BALANCE_CORE)
	Method(IsHalfNearSapper);
	Method(GetNearbyUnitClassModifierFromUnitClass);
	Method(GetSapperAreaEffectBonus);
	Method(GetGiveCombatModToUnit);
	Method(GetNearbyCityBonusCombatMod);
#endif
	Method(GetNearbyImprovementModifier);
	Method(IsFriendlyUnitAdjacent);
	Method(IsNoFriendlyUnitAdjacent);
	Method(GetNumEnemyUnitsAdjacent);
	Method(IsEnemyCityAdjacent);

	Method(GetTransportUnit);
	Method(IsCargo);

	Method(GetExtraDomainModifier);

	Method(GetName);
	Method(GetNameNoDesc);
	Method(HasName);
	Method(GetNameKey);
	Method(SetName);
	Method(GetCityName);
	Method(IsTerrainDoubleMove);
	Method(IsFeatureDoubleMove);
#if defined(MOD_PROMOTIONS_HALF_MOVE)
	Method(IsTerrainHalfMove);
	Method(IsFeatureHalfMove);
#endif

	Method(GetScriptData);
	Method(SetScriptData);
	Method(GetScenarioData);
	Method(SetScenarioData);

	Method(GetExtraTerrainAttackPercent);
	Method(GetExtraTerrainDefensePercent);
	Method(GetExtraFeatureAttackPercent);
	Method(GetExtraFeatureDefensePercent);
	Method(GetExtraUnitCombatModifier);
	Method(GetUnitClassModifier);

	Method(CanAcquirePromotion);
	Method(CanAcquirePromotionAny);
	Method(IsPromotionValid);
	Method(IsHasPromotion);
	Method(SetHasPromotion);
	Method(GetPromotionDuration);
	Method(GetTurnPromotionGained);

	Method(SetActivityType);
	Method(GetActivityType);
	Method(IsReadyToMove);
	Method(IsBusy);

	Method(GetReligion);
	Method(GetConversionStrength);
	Method(GetSpreadsLeft);
	Method(GetChargesLeft);
	Method(GetNumFollowersAfterInquisitor);
	Method(GetMajorityReligionAfterInquisitor);
	Method(GetNumFollowersAfterSpread);
	Method(GetMajorityReligionAfterSpread);
	Method(SetReligion);
	Method(SetConversionStrength);
	Method(SetSpreadsLeft);

	Method(GetTourismBlastStrength);
	Method(GetTourismBlastLength);

	Method(GetGreatWorkSlotType);

	// Helper Functions
	Method(RangeStrike);

	Method(PushMission);
	Method(PopMission);
	Method(LastMissionPlot);
	Method(CanStartMission);

	Method(ExecuteSpecialExploreMove);

	Method(SetDeployFromOperationTurn);
#if defined(MOD_BALANCE_CORE)
	Method(IsHigherPopThan);
	Method(GetResistancePower);
	Method(GetAllianceCSStrength);
#endif
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	Method(GetMonopolyAttackBonus);
	Method(GetMonopolyDefenseBonus);
#endif
	Method(IsHigherTechThan);
	Method(IsLargerCivThan);

	Method(IsRangedSupportFire);

#if defined(MOD_BALANCE_CORE_MILITARY)
	Method(GetAIOperationInfo);
	Method(GetMissionInfo);
	Method(GetDanger);
#endif

	Method(AddMessage);
	Method(IsCivilization);
	Method(HasPromotion);
	Method(IsUnit);
	Method(IsUnitClass);
	Method(IsOnFeature);
	Method(IsAdjacentToFeature);
	Method(IsWithinDistanceOfFeature);
	Method(IsWithinDistanceOfUnit);
	Method(IsWithinDistanceOfUnitClass);
	Method(IsWithinDistanceOfUnitCombatType);
	Method(IsWithinDistanceOfUnitPromotion);
	Method(IsAdjacentToUnit);
	Method(IsAdjacentToUnitClass);
	Method(IsAdjacentToUnitCombatType);
	Method(IsAdjacentToUnitPromotion);
	Method(IsWithinDistanceOfCity);
	Method(IsOnImprovement);
	Method(IsAdjacentToImprovement);
	Method(IsWithinDistanceOfImprovement);
	Method(IsOnPlotType);
	Method(IsAdjacentToPlotType);
	Method(IsWithinDistanceOfPlotType);
	Method(IsOnResource);
	Method(IsAdjacentToResource);
	Method(IsWithinDistanceOfResource);
	Method(IsOnTerrain);
	Method(IsAdjacentToTerrain);
	Method(IsWithinDistanceOfTerrain);
}
//------------------------------------------------------------------------------
const char* CvLuaUnit::GetTypeName()
{
	return "Unit";
}

//------------------------------------------------------------------------------
// Lua member methods
//------------------------------------------------------------------------------
//bool isNone();
int CvLuaUnit::lIsNone(lua_State* L)
{
	const bool bDoesNotExist = (GetInstance(L, false) == NULL);
	lua_pushboolean(L, bDoesNotExist);

	return 1;
}
//------------------------------------------------------------------------------
//void convert(CvUnit* pUnit, bool bIsUpgrade, bool bSupply = true);
int CvLuaUnit::lConvert(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkUnitToConvert = GetInstance(L, 2);
	bool bIsUpgrade = lua_toboolean(L, 3);
	pkUnit->convert(pkUnitToConvert, bIsUpgrade);

	// Unlike every other call to CvUnit::convert() do NOT call CvUnit::setupGraphical() here as it creates ghost units on the map
	return 0;
}
//------------------------------------------------------------------------------
//void DoUpgrade;
int CvLuaUnit::lUpgrade(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	bool bIsFree = luaL_optbool(L, 2, false);
	
	CvUnit* pkNewUnit = pkUnit->DoUpgrade(bIsFree);

	CvLuaUnit::Push(L, pkNewUnit);
	return 1;
}
//------------------------------------------------------------------------------
//void DoUpgradeTo;
int CvLuaUnit::lUpgradeTo(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitTypes eUpgradeUnitType = (UnitTypes) lua_tointeger(L, 2);
	const bool bIsFree = luaL_optbool(L, 3, false);
	
	CvUnit* pkNewUnit = pkUnit->DoUpgradeTo(eUpgradeUnitType, bIsFree);

	CvLuaUnit::Push(L, pkNewUnit);
	return 1;
}
//------------------------------------------------------------------------------
//void kill(bool bDelay, PlayerTypes ePlayer = NO_PLAYER);
int CvLuaUnit::lKill(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bDelay = lua_toboolean(L, 2);
	const PlayerTypes ePlayer
	    = (lua_isnil(L, 3))? NO_PLAYER : (PlayerTypes)lua_tointeger(L, 3);

	pkUnit->kill(bDelay, ePlayer);
	return 0;
}
//------------------------------------------------------------------------------
//bool isActionRecommended(int i);
int CvLuaUnit::lIsActionRecommended(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::isActionRecommended);
}
//------------------------------------------------------------------------------
//bool isBetterDefenderThan(CyUnit* pDefender, CyUnit* pAttacker);
int CvLuaUnit::lIsBetterDefenderThan(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkDefender = GetInstance(L, 2, false);
	CvUnit* pkAttacker = GetInstance(L, 3, false);
	const bool bResult = pkUnit->isBetterDefenderThan(pkDefender, pkAttacker);

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bTestBusy = true);
int CvLuaUnit::lCanDoCommand(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const CommandTypes eCommand = (CommandTypes)lua_tointeger(L, 2);
	const int iData1		= lua_tointeger(L, 3);
	const int iData2		= lua_tointeger(L, 4);
	const bool bTestVisible = luaL_optint(L, 5, 0);		//defaults to false
	const bool bTestBusy	= luaL_optint(L, 6, 1);		//defaults to true

	const bool bResult = pkUnit->canDoCommand(eCommand, iData1, iData2, bTestVisible, bTestBusy);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void doCommand(CommandTypes eCommand, int iData1, int iData2);
int CvLuaUnit::lDoCommand(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::doCommand);
}
//------------------------------------------------------------------------------
//CyPlot* GetPathEndTurnPlot();
int CvLuaUnit::lGetPathEndTurnPlot(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	CvPlot* pkPlot = pkUnit->GetPathEndFirstTurnPlot();
	CvLuaPlot::Push(L, pkPlot);

	return 1;
}
//------------------------------------------------------------------------------
//bool generatePath(CvPlot* pToPlot, int iMaxTurns);
int CvLuaUnit::lGeneratePath(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const int iMaxTurns = lua_tointeger(L, 3);

	lua_createtable(L, 0, 0);
	int iCount = 1;

	//no caching!
	SPathFinderUserData data(pkUnit, CvUnit::MOVEFLAG_IGNORE_STACKING_SELF, iMaxTurns);
	SPath newPath = GC.GetPathFinder().GetPath(pkUnit->getX(), pkUnit->getY(), pkPlot->getX(), pkPlot->getY(), data, TC_UI);

	for (int i = 0; i < newPath.length(); i++)
	{
		const SPathNode& kNode = newPath.vPlots[i];

		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);
		lua_pushinteger(L, kNode.x);
		lua_setfield(L, t, "X");
		lua_pushinteger(L, kNode.y);
		lua_setfield(L, t, "Y");
		lua_pushinteger(L, kNode.moves);
		lua_setfield(L, t, "RemainingMovement");
		lua_pushinteger(L, kNode.turns);
		lua_setfield(L, t, "Turn");
		lua_pushinteger(L, 0);
		lua_setfield(L, t, "Flags");
		lua_pushboolean(L, newPath.get(i)->isVisible(pkUnit->getTeam())==false ); //don't have that info, make it up
		lua_setfield(L, t, "Invisible");
		lua_pushboolean(L, newPath.get(i)->isAdjacentNonvisible(pkUnit->getTeam()) ); //don't have that info, make it up
		lua_setfield(L, t, "AdjInvisible");
		lua_rawseti(L, -2, iCount++);
	}

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetActivePath(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	lua_createtable(L, 0, 0);
	int iCount = 1;

	CvPlot* pDestPlot = pkUnit->LastMissionPlot();
	if (!pDestPlot)
		return 0;

	//no caching!
	SPathFinderUserData data(pkUnit, 0, INT_MAX);
	SPath newPath = GC.GetPathFinder().GetPath(pkUnit->getX(), pkUnit->getY(), pDestPlot->getX(), pDestPlot->getY(), data, TC_UI);

	for (int i = 0; i < newPath.length(); i++)
	{
		const SPathNode& kNode = newPath.vPlots[i];

		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);
		lua_pushinteger(L, kNode.x);
		lua_setfield(L, t, "X");
		lua_pushinteger(L, kNode.y);
		lua_setfield(L, t, "Y");
		lua_pushinteger(L, kNode.moves);
		lua_setfield(L, t, "RemainingMovement");
		lua_pushinteger(L, kNode.turns);
		lua_setfield(L, t, "Turn");
		lua_pushinteger(L, 0);
		lua_setfield(L, t, "Flags");
		lua_pushboolean(L, newPath.get(i)->isVisible(pkUnit->getTeam()) == false); //don't have that info, make it up
		lua_setfield(L, t, "Invisible");
		lua_pushboolean(L, newPath.get(i)->isAdjacentNonvisible(pkUnit->getTeam())); //don't have that info, make it up
		lua_setfield(L, t, "AdjInvisible");
		lua_rawseti(L, -2, iCount++);
	}

	return 1;
}
//------------------------------------------------------------------------------
//-----Returns the proper path for queued move orders
int CvLuaUnit::lGetWaypointPath(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	lua_createtable(L, 0, 0);
	int iCount = 1;

	int fromX = pkUnit->getX();
	int fromY = pkUnit->getY();

	for (int k = 0; k < pkUnit->GetLengthMissionQueue(); k++)
	{

		const MissionData* pMissionNode = pkUnit->GetMissionData(k);

		int toX = pMissionNode->iData1;
		int toY = pMissionNode->iData2;

		//no caching!
		SPathFinderUserData data(pkUnit, 0, INT_MAX);
		SPath newPath = GC.GetPathFinder().GetPath(fromX, fromY, toX, toY, data, TC_UI);

		for (int i = 0; i < newPath.length(); i++)
		{
			const SPathNode& kNode = newPath.vPlots[i];

			lua_createtable(L, 0, 0);
			const int t = lua_gettop(L);
			lua_pushinteger(L, kNode.x);
			lua_setfield(L, t, "X");
			lua_pushinteger(L, kNode.y);
			lua_setfield(L, t, "Y");
			lua_pushinteger(L, kNode.moves);
			lua_setfield(L, t, "RemainingMovement");
			lua_pushinteger(L, kNode.turns);
			lua_setfield(L, t, "Turn");
			lua_pushinteger(L, 0);
			lua_setfield(L, t, "Flags");
			lua_pushboolean(L, newPath.get(i)->isVisible(pkUnit->getTeam()) == false); //don't have that info, make it up
			lua_setfield(L, t, "Invisible");
			lua_pushboolean(L, newPath.get(i)->isAdjacentNonvisible(pkUnit->getTeam())); //don't have that info, make it up
			lua_setfield(L, t, "AdjInvisible"); 
			lua_rawseti(L, -2, iCount++);
		}
		
		fromX = toX;
		fromY = toY;
	}
	return 1;
}
//-----Returns the proper path for queued move orders
int CvLuaUnit::lGeneratePathToNextWaypoint(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = pkUnit->LastMissionPlot();
	CvPlot* pDestPlot = CvLuaPlot::GetInstance(L, 2);
	if (!pDestPlot)
		return 0;

	lua_createtable(L, 0, 0);
	int iCount = 1;

	//no caching!
	SPathFinderUserData data(pkUnit, 0, INT_MAX);
	SPath newPath = GC.GetPathFinder().GetPath(pkPlot->getX(), pkPlot->getY(), pDestPlot->getX(), pDestPlot->getY(), data, TC_UI);

	for (int i = 0; i < newPath.length(); i++)
	{
		const SPathNode& kNode = newPath.vPlots[i];

		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);
		lua_pushinteger(L, kNode.x);
		lua_setfield(L, t, "X");
		lua_pushinteger(L, kNode.y);
		lua_setfield(L, t, "Y");
		lua_pushinteger(L, kNode.moves);
		lua_setfield(L, t, "RemainingMovement");
		lua_pushinteger(L, kNode.turns);
		lua_setfield(L, t, "Turn");
		lua_pushinteger(L, 0);
		lua_setfield(L, t, "Flags");
		lua_pushboolean(L, newPath.get(i)->isVisible(pkUnit->getTeam()) == false); //don't have that info, make it up
		lua_setfield(L, t, "Invisible");
		lua_pushboolean(L, newPath.get(i)->isAdjacentNonvisible(pkUnit->getTeam())); //don't have that info, make it up
		lua_setfield(L, t, "AdjInvisible");
		lua_rawseti(L, -2, iCount++);
	}

	return 1;
}

//-------------- returns the plots the unit can bombard from a given plot. does not check for presence of enemies!
// GetPotentialRangeAttackTargetPlots(plot)
int CvLuaUnit::lGetPotentialRangeAttackTargetPlots(lua_State* L)
{
	CvUnit* pUnit = GetInstance(L);
	CvPlot* pOrigin = CvLuaPlot::GetInstance(L, 2);
	if (!pOrigin)
		return 0;

	lua_createtable(L, 0, 0);
	int iCount = 1;

	// Aircraft and special promotions make us ignore LOS
	bool bIgnoreLOS = pUnit->IsRangeAttackIgnoreLOS() || pUnit->getDomainType() == DOMAIN_AIR;
	// Can only bombard in domain? (used for Subs' torpedo attack)
	bool bOnlyInDomain = pUnit->getUnitInfo().IsRangeAttackOnlyInDomain();

	for (int iRange=1; iRange<=pUnit->GetRange(); iRange++)
	{
		const vector<CvPlot*>& vCandidates = GC.getMap().GetPlotsAtRangeX(pOrigin, iRange, true, !bIgnoreLOS);

		for (size_t i = 0; i < vCandidates.size(); i++)
		{
			//skip sentinels
			if (vCandidates[i] == NULL)
				continue;

			if (!vCandidates[i]->isRevealed(pUnit->getTeam()))
				continue;

			if (!pUnit->isNativeDomain(vCandidates[i]))
				continue;

			if (bOnlyInDomain)
			{
				//subs can only attack within their (water) area or adjacent cities
				if (pOrigin->getArea() != vCandidates[i]->getArea())
				{
					CvCity* pCity = vCandidates[i]->getPlotCity();
					if (!pCity || !pCity->HasAccessToArea(pOrigin->getArea()))
						continue;
				}
			}

			lua_createtable(L, 0, 0);
			const int t = lua_gettop(L);
			lua_pushinteger(L, vCandidates[i]->getX());
			lua_setfield(L, t, "X");
			lua_pushinteger(L, vCandidates[i]->getY());
			lua_setfield(L, t, "Y");
			lua_rawseti(L, -2, iCount++);
		}
	}

	return 1;
}

//-------------- returns the plots the unit would need to be in to bombard a given plot. does not check for stacking.
// GetPotentialRangeAttackOriginPlots(plot)
int CvLuaUnit::lGetPotentialRangeAttackOriginPlots(lua_State* L)
{
	CvUnit* pUnit = GetInstance(L);
	CvPlot* pTarget = CvLuaPlot::GetInstance(L, 2);
	if (!pTarget)
		return 0;

	lua_createtable(L, 0, 0);
	int iCount = 1;

	// Aircraft and special promotions make us ignore LOS
	bool bIgnoreLOS = pUnit->IsRangeAttackIgnoreLOS() || pUnit->getDomainType() == DOMAIN_AIR;
	// Can only bombard in domain? (used for Subs' torpedo attack)
	bool bOnlyInDomain = pUnit->getUnitInfo().IsRangeAttackOnlyInDomain();

	for (int iRange = 1; iRange <= pUnit->GetRange(); iRange++)
	{
		const vector<CvPlot*>& vCandidates = GC.getMap().GetPlotsAtRangeX(pTarget, iRange, false, !bIgnoreLOS);

		for (size_t i = 0; i < vCandidates.size(); i++)
		{
			//skip sentinels
			if (vCandidates[i] == NULL)
				continue;

			if (!vCandidates[i]->isRevealed(pUnit->getTeam()))
				continue;

			if (!pUnit->isNativeDomain(vCandidates[i]))
				continue;

			if (!pUnit->canEndTurnAtPlot(vCandidates[i]))
				continue;

			if (bOnlyInDomain)
			{
				//subs can only attack within their (water) area or adjacent cities
				if (pTarget->getArea() != vCandidates[i]->getArea())
				{
					CvCity* pCity = vCandidates[i]->getPlotCity();
					if (!pCity || !pCity->HasAccessToArea(pTarget->getArea()))
						continue;
				}
			}

			lua_createtable(L, 0, 0);
			const int t = lua_gettop(L);
			lua_pushinteger(L, vCandidates[i]->getX());
			lua_setfield(L, t, "X");
			lua_pushinteger(L, vCandidates[i]->getY());
			lua_setfield(L, t, "Y");
			lua_rawseti(L, -2, iCount++);
		}
	}

	return 1;
}

//-------------------------------------------------------------------------------------------
// Returns the estimated "from" plot when this unit melee attacks a unit on the given plot
int CvLuaUnit::lGetMeleeAttackFromPlot(lua_State* L)
{
	CvUnit* pUnit = GetInstance(L);
	CvPlot* pToPlot = CvLuaPlot::GetInstance(L, 2);
	CvPlot* pFromPlot = pUnit->plot();

	if (pUnit->GeneratePath(pToPlot))
	{
		CvPathNodeArray path = pUnit->GetLastPath();
		if (path.size() >= 2)
		{
			CvPathNode secondLastNode = path.at(path.size() - 2);
			CvPlot* pEnd = GC.getMap().plot(secondLastNode.m_iX, secondLastNode.m_iY);
			if (pEnd)
				pFromPlot = pEnd;
		}
	}

	CvLuaPlot::Push(L, pFromPlot);
	return 1;
}

//------------------------------------------------------------------------------
//bool canEnterTerritory(int /*TeamTypes*/ eTeam, bool bIgnoreRightOfPassage = false, bool bIsCity = false);
int CvLuaUnit::lCanEnterTerritory(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TeamTypes eTeam				= (TeamTypes)lua_tointeger(L, 2);
	//this parameter is useless
	const bool bIgnoreRightOfPassage	= luaL_optint(L, 3, 0);

	//this parameter is ignored ...
	//const bool bIsCity				= luaL_optint(L, 4, 0);

	if (bIgnoreRightOfPassage)
	{
		lua_pushboolean(L, true);
	}
	else
	{
		const bool bResult = pkUnit->canEnterTerritory(eTeam);
		lua_pushboolean(L, bResult);
	}

	return 1;
}
//------------------------------------------------------------------------------
//TeamTypes GetDeclareWarRangeStrike(CvPlot* pPlot)
int CvLuaUnit::lGetDeclareWarRangeStrike(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	TeamTypes eResult = NO_TEAM;
	if(pkPlot)
	{
		eResult = pkUnit->GetDeclareWarRangeStrike(*pkPlot);
	}
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canMoveOrAttackInto(CyPlot* pPlot, bool bDeclareWar = false, bDestination = false);
int CvLuaUnit::lCanMoveOrAttackInto(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	//const bool bDeclareWar = luaL_optint(L, 3, 0);
	const bool bDestination = luaL_optint(L, 4, 0);

	int iMoveFlags = 0;
	if(bDestination)
	{
		iMoveFlags |= CvUnit::MOVEFLAG_DESTINATION;
	}

	bool bResult = false;
	if(pkPlot)
	{
		bResult = pkUnit->canMoveOrAttackInto(*pkPlot, iMoveFlags);
	}

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canMoveThrough(CyPlot* pPlot);
int CvLuaUnit::lCanMoveThrough(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	bool bResult = false;
	if(pkPlot)
	{
		bResult = pkUnit->canMoveInto(*pkPlot);
	}

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void jumpToNearestValidPlot();
int CvLuaUnit::lJumpToNearestValidPlot(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	bool bResult = pkUnit->jumpToNearestValidPlot();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
// int getCombatDamage(int iStrength, int iOpponentStrength, int iCurrentDamage, bool bIncludeRand = true, bool bAttackerIsCity = false, bool bDefenderIsCity = false);
int CvLuaUnit::lGetCombatDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iStrength = lua_tointeger(L, 2);
	const int iOpponentStrength = lua_tointeger(L, 3);
	//damage is factored into the strength values now
	//const int iCurrentDamage = lua_tointeger(L, 4); 
	const bool bIncludeRand = lua_toboolean(L, 5);
	const bool bAttackerIsCity = lua_toboolean(L, 6);
	const bool bDefenderIsCity = lua_toboolean(L, 7);

	int iResult = pkUnit->getCombatDamage(iStrength, iOpponentStrength, bIncludeRand, bAttackerIsCity, bDefenderIsCity);

#if defined(MOD_BALANCE_CORE)
	//for visual feedback, take care that we show the precise value
	CvCity* pkCity = CvLuaCity::GetInstance(L, 8, false);
	if (pkCity && pkCity->HasGarrison())
	{
		CvUnit* pGarrison = pkCity->GetGarrisonedUnit();
		if (pGarrison)
		{
			int iGarrisonShare = (iResult*2*pGarrison->GetMaxHitPoints()) / (pkCity->GetMaxHitPoints()+2*pGarrison->GetMaxHitPoints());
			iResult -= iGarrisonShare;
		}
	}
#endif

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//CvUnit* getFireSupportUnit(PlayerTypes eDefender, int iX, int iY);
int CvLuaUnit::lGetFireSupportUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PlayerTypes eDefender = (PlayerTypes) lua_tointeger(L, 2);
	const int iX = lua_tointeger(L, 3);
	const int iY = lua_tointeger(L, 4);

	CvUnit* pkUnitToReturn = CvUnitCombat::GetFireSupportUnit(eDefender, iX, iY, pkUnit->getX(), pkUnit->getY());
	CvLuaUnit::Push(L, pkUnitToReturn);

	return 1;
}
//------------------------------------------------------------------------------
//bool canAutomate(AutomateTypes eAutomate);
int CvLuaUnit::lCanAutomate(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const AutomateTypes eAutomate = (AutomateTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->CanAutomate(eAutomate);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canRebaseAt(plot,x,y);
int CvLuaUnit::lCanRebaseAt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	//CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const int iX = lua_tointeger(L, 3);
	const int iY = lua_tointeger(L, 4);
	const bool bResult = pkUnit->canRebaseAt(iX,iY);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool rebase(x,y);
int CvLuaUnit::lRebaseAt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iX = lua_tointeger(L, 2);
	const int iY = lua_tointeger(L, 3);
	const bool bResult = pkUnit->rebase(iX, iY);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canScrap();
int CvLuaUnit::lCanScrap(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canScrap();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool GetScrapGold();
int CvLuaUnit::lGetScrapGold(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->GetScrapGold();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canGift(bool bTestVisible = false, bool bTestTransport = false);
int CvLuaUnit::lCanGift(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bTestVisible = luaL_optint(L, 2, 0);
	const bool bTestTransport = luaL_optint(L, 3, 0);
	const bool bResult = pkUnit->canGift(bTestVisible, bTestTransport);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanDistanceGift();
int CvLuaUnit::lCanDistanceGift(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PlayerTypes eToPlayer = (PlayerTypes) lua_tointeger(L, 2);
	const bool bResult = pkUnit->CanDistanceGift(eToPlayer);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsUnitValidGiftForCityStateQuest(PlayerTypes ePlayer, CvUnit* pUnit);
int CvLuaUnit::lIsUnitValidGiftForCityStateQuest(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PlayerTypes eFromPlayer = (PlayerTypes)lua_tointeger(L, 2);
	const PlayerTypes eToPlayer = (PlayerTypes)lua_tointeger(L, 3);

	bool bResult = false;
	if (GET_PLAYER(eToPlayer).isMinorCiv())
		bResult = GET_PLAYER(eToPlayer).GetMinorCivAI()->IsUnitValidGiftForCityStateQuest(eFromPlayer, pkUnit);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canLoadUnit(CyUnit* pUnit, CyPlot* pPlot);
int CvLuaUnit::lCanLoadUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkLoadUnit = GetInstance(L, 2);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 3);
	bool bResult = false;
	if(pkPlot && pkUnit)
	{
		bResult = pkUnit->canLoadUnit(*pkLoadUnit, *pkPlot);
	}

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canLoad(CyPlot* pPlot);
int CvLuaUnit::lCanLoad(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	bool bResult = false;
	if(pkPlot)
	{
		bResult = pkUnit->canLoad(*pkPlot);
	}
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canUnload();
int CvLuaUnit::lCanUnload(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canUnload();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canUnloadAll();
int CvLuaUnit::lCanUnloadAll(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canUnloadAll();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canHold(CyPlot* pPlot);
int CvLuaUnit::lCanHold(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canHold(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canSleep(CyPlot* pPlot);
int CvLuaUnit::lCanSleep(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canSleep(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canFortify(CyPlot* pPlot);
int CvLuaUnit::lCanFortify(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canFortify(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canAirPatrol(CyPlot* pPlot);
int CvLuaUnit::lCanAirPatrol(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canAirPatrol(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isEmbarked();
int CvLuaUnit::lIsEmbarked(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isEmbarked();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setEmbarked(bool bNewValue);
int CvLuaUnit::lSetEmbarked(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bNewValue = lua_toboolean(L, 2);

	pkUnit->setEmbarked(bNewValue);
	return 0;
}
//------------------------------------------------------------------------------
//bool canHeal(CyPlot* pPlot);
int CvLuaUnit::lCanHeal(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canHeal(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canSentry(CyPlot* pPlot);
int CvLuaUnit::lCanSentry(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canSentry(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canEmbark(CvPlot* pPlot)
int CvLuaUnit::lCanEmbark(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canEmbarkAtPlot(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canDisembark(CvPlot* pPlot)
int CvLuaUnit::lCanDisembark(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canDisembarkAtPlot(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanEmbarkOnto(CvPlot* pPlot)
int CvLuaUnit::lCanEmbarkOnto(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkOriginPlot = CvLuaPlot::GetInstance(L, 2);
	CvPlot* pkTargetPlot = CvLuaPlot::GetInstance(L, 3);
	bool bIsDestination = luaL_optbool(L, 4, true);		// Assume Lua is querying for a destination of the embark.  This will cause the test to fail if there is are stacking issues.
	bool bResult = false;
	if(pkOriginPlot && pkTargetPlot)
	{
		bResult = pkUnit->canEmbarkOnto(*pkOriginPlot, *pkTargetPlot, false, bIsDestination ? CvUnit::MOVEFLAG_DESTINATION : 0);
	}

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanDisembarkOnto(CvPlot* pPlot)
int CvLuaUnit::lCanDisembarkOnto(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkTargetPlot = CvLuaPlot::GetInstance(L, 2);
	bool bIsDestination = luaL_optbool(L, 3, true);		// Assume Lua is querying for a destination of the dis-embark.  This will cause the test to fail if there is are stacking issues.

	bool bResult = false;
	if(pkTargetPlot)
	{
		bResult = pkUnit->canDisembarkOnto(*pkUnit->plot(), *pkTargetPlot, false, bIsDestination ? CvUnit::MOVEFLAG_DESTINATION : 0);	
	}

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void Embark(CvPlot* pPlot)
int CvLuaUnit::lEmbark(lua_State* L)
{
	bool bSuccess = false;

	CvUnit* pkUnit = GetInstance(L);
	if(pkUnit != NULL)
	{
		CvPlot* pkTargetPlot = NULL;
		if(lua_gettop(L) > 1)
			pkTargetPlot = CvLuaPlot::GetInstance(L, 2);
		else
			pkTargetPlot = GC.getMap().plot(pkUnit->getX(), pkUnit->getY());

		if(pkTargetPlot != NULL)
		{
			pkUnit->embark(pkTargetPlot);
			bSuccess = true;
		}
	}

	lua_pushboolean(L, bSuccess);

	return 1;
}
//------------------------------------------------------------------------------
//bool IsRangeAttackIgnoreLOS(CyPlot* pPlot);
int CvLuaUnit::lIsRangeAttackIgnoreLOS(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::IsRangeAttackIgnoreLOS);
}
//------------------------------------------------------------------------------
//bool canAirlift(CyPlot* pPlot);
int CvLuaUnit::lCanAirlift(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canAirlift(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canAirliftAt(CyPlot* pPlot, int iX, int iY);
int CvLuaUnit::lCanAirliftAt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const int x = lua_tointeger(L, 3);
	const int y = lua_tointeger(L, 4);
	const bool bResult = pkUnit->canAirliftAt(pkPlot, x, y);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isNukeVictim(CyPlot* pPlot, int /*TeamTypes*/ eTeam);
int CvLuaUnit::lIsNukeVictim(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 3);
	const bool bResult = pkUnit->isNukeVictim(pkPlot, eTeam);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canNuke(CyPlot* pPlot);
int CvLuaUnit::lCanNuke(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canNuke();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canNukeAt(CyPlot* pPlot, int iX, int iY);
int CvLuaUnit::lCanNukeAt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int x = lua_tointeger(L, 2);
	const int y = lua_tointeger(L, 3);
	const bool bResult = pkUnit->canNukeAt(pkUnit->plot(), x, y);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanRangeStrike()
int CvLuaUnit::lCanRangeStrike(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::canRangeStrike);
}
//------------------------------------------------------------------------------
//bool CanRangeStrikeAt(int iX, int iY)
int CvLuaUnit::lCanEverRangeStrikeAt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int x = lua_tointeger(L, 2);
	const int y = lua_tointeger(L, 3);

	const bool bResult = pkUnit->canEverRangeStrikeAt(x, y);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool CanRangeStrikeAt(int iX, int iY)
int CvLuaUnit::lCanRangeStrikeAt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int x = lua_tointeger(L, 2);
	const int y = lua_tointeger(L, 3);
	const bool bNeedsWar = luaL_optbool(L, 4, true);
	const bool bNoncombatAllowed = luaL_optbool(L, 5, true);

	const bool bResult = pkUnit->canRangeStrikeAt(x, y, bNeedsWar, bNoncombatAllowed);
	lua_pushboolean(L, bResult);
	return 1;

	//return BasicLuaMethod(L, &CvUnit::canRangeStrikeAt);
}
//------------------------------------------------------------------------------
//bool canParadrop(CyPlot* pPlot);
int CvLuaUnit::lCanParadrop(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	bool bTestVisibility = lua_toboolean(L, 3);
	const bool bResult = pkUnit->canParadrop(pkPlot, bTestVisibility);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canParadropAt(CyPlot* pPlot, int iX, int iY);
int CvLuaUnit::lCanParadropAt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const int x = lua_tointeger(L, 3);
	const int y = lua_tointeger(L, 4);
	const bool bResult = pkUnit->canParadropAt(pkPlot, x, y);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canMakeTradeRoute(CyPlot* pPlot)
int CvLuaUnit::lCanMakeTradeRoute(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canMakeTradeRoute(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canMakeTradeRouteAt(CyPlot* pPlot, int iX, int iY)
int CvLuaUnit::lCanMakeTradeRouteAt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const int x = lua_tointeger(L, 3);
	const int y = lua_tointeger(L, 4);
	TradeConnectionType eConnection = (TradeConnectionType)lua_tointeger(L, 5);
	const bool bResult = pkUnit->canMakeTradeRouteAt(pkPlot, x, y, eConnection);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canPlunderTradeRoute(CvPlot* pPlot)
int CvLuaUnit::lCanPlunderTradeRoute(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canPlunderTradeRoute(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool plunderTradeRoute()
int CvLuaUnit::lPlunderTradeRoute(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->plunderTradeRoute();

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool canCreateGreatWork(CvPlot* pPlot)
int CvLuaUnit::lCanCreateGreatWork(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canCreateGreatWork(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool createGreatWork()
int CvLuaUnit::lCreateGreatWork(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->createGreatWork();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool greatperson()
int CvLuaUnit::lgreatperson(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->greatperson();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool GetCombatVersusOtherReligionOwnLands();
int CvLuaUnit::lGetCombatVersusOtherReligionOwnLands(lua_State* L)
{
	int iRtnValue = 0;

	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOtherUnit = CvLuaUnit::GetInstance(L, 2);
	if (pkUnit && pkUnit->getDomainType() == DOMAIN_LAND && pkOtherUnit)
	{
		CvGameReligions* pReligions = GC.getGame().GetGameReligions();
		ReligionTypes eOwnedReligion = GET_PLAYER(pkUnit->getOwner()).GetReligions()->GetOwnedReligion();
		ReligionTypes eTheirReligion = GET_PLAYER(pkOtherUnit->getOwner()).GetReligions()->GetStateReligion();

		if (eOwnedReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = pReligions->GetReligion(eOwnedReligion, pkUnit->getOwner());
			if (pReligion)
			{
				CvCity* pHolyCity = pReligion->GetHolyCity();
				if (eTheirReligion != eOwnedReligion)
				{			
					int iOtherOwn = pReligion->m_Beliefs.GetCombatVersusOtherReligionOwnLands(pkUnit->getOwner(), pHolyCity);
					// Bonus in own land
					if((iOtherOwn > 0) && pkUnit->plot()->IsFriendlyTerritory(pkUnit->getOwner()))
					{
						iRtnValue = iOtherOwn;
					}
				}
				else
				{
					int iOtherOwn = pReligion->m_Beliefs.GetCombatVersusOtherReligionOwnLands(pkUnit->getOwner(), pHolyCity);
					// Bonus in own land
					if((iOtherOwn > 0) && pkUnit->plot()->IsFriendlyTerritory(pkUnit->getOwner()))
					{
						iRtnValue = (iOtherOwn / 2);
					}
				}
			}
		}
	}
	lua_pushinteger(L, iRtnValue);

	return 1;
}
//------------------------------------------------------------------------------
//bool GetCombatVersusOtherReligionTheirLands();
int CvLuaUnit::lGetCombatVersusOtherReligionTheirLands(lua_State* L)
{
	int iRtnValue = 0;

	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOtherUnit = CvLuaUnit::GetInstance(L, 2);
	if (pkUnit && pkUnit->getDomainType() == DOMAIN_LAND && pkOtherUnit)
	{
		ReligionTypes eOwnedReligion = GET_PLAYER(pkUnit->getOwner()).GetReligions()->GetOwnedReligion();
		ReligionTypes eTheirReligion = GET_PLAYER(pkOtherUnit->getOwner()).GetReligions()->GetStateReligion();

		if (eOwnedReligion != NO_RELIGION)
		{
			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			const CvReligion* pReligion = pReligions->GetReligion(eOwnedReligion, pkUnit->getOwner());
			if (pReligion)
			{
				CvCity* pHolyCity = pReligion->GetHolyCity();
				if (eTheirReligion != eOwnedReligion)
				{			
					int iOtherTheir = pReligion->m_Beliefs.GetCombatVersusOtherReligionTheirLands(pkUnit->getOwner(), pHolyCity);
					//Bonus in their land
					if ((iOtherTheir > 0) && pkOtherUnit->plot()->IsFriendlyTerritory(pkOtherUnit->getOwner()))
					{
						iRtnValue = iOtherTheir;
					}
				}
				else
				{
					int iOtherTheir = pReligion->m_Beliefs.GetCombatVersusOtherReligionTheirLands(pkUnit->getOwner(), pHolyCity);
					//Bonus in their land
					if ((iOtherTheir > 0) && pkOtherUnit->plot()->IsFriendlyTerritory(pkOtherUnit->getOwner()))
					{
						iRtnValue = (iOtherTheir / 2);
					}
				}
			}
		}
	}
	lua_pushinteger(L, iRtnValue);

	return 1;
}

//------------------------------------------------------------------------------
//int GetExoticGoodsGoldAmount()
int CvLuaUnit::lGetExoticGoodsGoldAmount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iValue = pkUnit->getExoticGoodsGoldAmount();
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetExoticGoodsXPAmount()
int CvLuaUnit::lGetExoticGoodsXPAmount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iValue = pkUnit->getExoticGoodsXPAmount();
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool canPillage(CyPlot* pPlot);
int CvLuaUnit::lCanPillage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canPillage(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsSelected( void );
int CvLuaUnit::lIsSelected(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsSelected();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canFound(CyPlot* pPlot, bool bTestVisible = false);
int CvLuaUnit::lCanFound(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bTestVisible = luaL_optint(L, 3, 0);
	const bool bResult = pkUnit->canFoundCity(pkPlot, bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canJoin(CyPlot* pPlot, int /*SpecialistTypes*/ eSpecialist);
int CvLuaUnit::lCanJoin(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const SpecialistTypes eSpecialist = (SpecialistTypes)lua_tointeger(L, 3);
	const bool bResult = pkUnit->canJoinCity(pkPlot, eSpecialist);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canConstruct(CyPlot* pPlot, int /*BuildingTypes*/ eBuilding);
int CvLuaUnit::lCanConstruct(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const BuildingTypes eBuilding = (BuildingTypes)lua_tointeger(L, 3);
	const bool bResult = pkUnit->canConstruct(pkPlot, eBuilding);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lIsRangeAttackOnlyInDomain(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pkUnit->getUnitType());

	if(pkUnitInfo == NULL)
	{
		luaL_error(L, "Could not find unit info (%d) for unit.", pkUnit->getUnitType());
		return 0;
	}

	const bool bResult = pkUnitInfo->IsRangeAttackOnlyInDomain();
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lIsCityAttackOnly(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	//for historical reasons, we keep the lua name, but in the dll we use a better one
	const bool bResult = pkUnit->IsCityAttackSupport();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//TechTypes getDiscoveryTech();
//int CvLuaUnit::lGetDiscoveryTech(lua_State* L)
//{
//	CvUnit* pkUnit = GetInstance(L);
//
//	const TechTypes eResult = pkUnit->getDiscoveryTech();
//	lua_pushinteger(L, eResult);
//	return 1;
//}
////------------------------------------------------------------------------------
////int getDiscoverResearch(int /*TechTypes*/ eTech);
//int CvLuaUnit::lGetDiscoverResearch(lua_State* L)
//{
//	CvUnit* pkUnit = GetInstance(L);
//	const TechTypes eTech = (TechTypes)lua_tointeger(L, 2);
//
//	const int iResult = pkUnit->getDiscoverResearch(eTech);
//	lua_pushinteger(L, iResult);
//	return 1;
//}
//------------------------------------------------------------------------------
//bool canDiscover(CyPlot* pPlot);

int CvLuaUnit::lCreateFreeLuxuryCheckCopy(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->CreateFreeLuxuryCheckCopy();
	lua_pushinteger(L, iResult);
	return 1;
}

int CvLuaUnit::lCreateFreeLuxuryCheck(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->CreateFreeLuxuryCheck();
	lua_pushinteger(L, iResult);
	return 1;
}

int CvLuaUnit::lCanDiscover(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canDiscover(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getDiscoverAmount();
int CvLuaUnit::lGetDiscoverAmount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->GetScienceBlastStrength();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetHurryProduction();
int CvLuaUnit::lGetHurryProduction(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetHurryStrength();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetTradeGold();
int CvLuaUnit::lGetTradeGold(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getTradeGold();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetTradeInfluence(CyPlot* pPlot);
int CvLuaUnit::lGetTradeInfluence(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	const int iResult = pkUnit->getTradeInfluence(pkPlot);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetRestingPointChange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnitEntry *pkUnitEntry = GC.getUnitInfo(pkUnit->getUnitType());
	if (pkUnitEntry == NULL)
	{
		lua_pushinteger(L, 0);
		return 1;
	}

	const int iResult = pkUnitEntry->GetRestingPointChange();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canBuyCityState(CyPlot* pPlot, bool bTestVisible);
int CvLuaUnit::lCanBuyCityState(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bTestVisible = luaL_optint(L, 3, 0);
	const bool bResult = pkUnit->canBuyCityState(pkPlot, bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canTrade(CyPlot* pPlot, bool bTestVisible);
int CvLuaUnit::lCanTrade(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bTestVisible = luaL_optint(L, 3, 0);
	const bool bResult = pkUnit->canTrade(pkPlot, bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canRepairFleet(CyPlot* pPlot, bool bTestVisible);
int CvLuaUnit::lCanRepairFleet(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bTestVisible = luaL_optint(L, 3, 0);
	const bool bResult = pkUnit->canRepairFleet(pkPlot, bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool canChangePort(CvPlot* pPlot);
int CvLuaUnit::lCanChangePort(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canChangeAdmiralPort(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool CanBuildSpaceship(CyPlot* pPlot, bool bVisible);
int CvLuaUnit::lCanBuildSpaceship(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bTestVisible = luaL_optint(L, 3, 0);
	const bool bResult = pkUnit->CanBuildSpaceship(pkPlot, bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canGoldenAge(CyPlot* pPlot, bool bTestVisible);
int CvLuaUnit::lCanGoldenAge(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canGoldenAge(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetGoldenAgeTurns();
int CvLuaUnit::lGetGoldenAgeTurns(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iResult = 0;
	if (pkUnit->GetGAPBlastStrength() > 0)
	{
		iResult = pkUnit->GetGAPBlastStrength();
	}
	else
		iResult = pkUnit->getGoldenAgeTurns();

	lua_pushinteger(L, iResult);
	return 1;
}

//int GetGoldenAgeTurns();
int CvLuaUnit::lGetGAPAmount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->GetGAPBlastStrength();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetGivePoliciesCulture()
int CvLuaUnit::lGetGivePoliciesCulture(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
#if defined(MOD_BALANCE_CORE)
	const int iResult = pkUnit->GetCultureBlastStrength();
#else
	const int iResult = pkUnit->getGivePoliciesCulture();
#endif
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetBlastTourism()
int CvLuaUnit::lGetBlastTourism(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iResult = 0;
	if (pkUnit)
	{
		iResult = pkUnit->getBlastTourism();

#if defined(MOD_BALANCE_CORE)
		if (MOD_BALANCE_CORE_NEW_GP_ATTRIBUTES && pkUnit && pkUnit->getBlastTourism() > 0)
		{
			CvPlot* pPlot = pkUnit->plot();
			if (pPlot && pkUnit->canBlastTourism(pPlot))
			{
				CvPlayer& kUnitOwner = GET_PLAYER(pkUnit->getOwner());
				PlayerTypes eOtherPlayer = pPlot->getOwner();
				

				// below logic based on CvPlayerCulture::ChangeInfluenceOn()
				if (eOtherPlayer != NO_PLAYER)
				{
					// gamespeed modifier
					iResult = iResult * GC.getGame().getGameSpeedInfo().getCulturePercent() / 100;

					// player to player modifier (eg religion, open borders, ideology)
					if (kUnitOwner.getCapitalCity())
					{
						int iModifier = kUnitOwner.getCapitalCity()->GetCityCulture()->GetTourismMultiplier(eOtherPlayer, false, false, false, false, false);
						if (iModifier != 0)
						{
							iResult = iResult * (100 + iModifier) / 100;
						}
					}

					// IsNoOpenTrade trait modifier (half tourism if trait owner does not send a trade route to the unit owner)
					CvPlayer& kOtherPlayer = GET_PLAYER(eOtherPlayer);
					if (eOtherPlayer != pkUnit->getOwner() && kOtherPlayer.isMajorCiv() && kOtherPlayer.GetPlayerTraits()->IsNoOpenTrade())
					{
						if (!GC.getGame().GetGameTrade()->IsPlayerConnectedToPlayer(eOtherPlayer, pkUnit->getOwner(), true))
							iResult /= 2;
					}
				}
			}
		}
#endif
	}
	
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canBuild(CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestVisible = false, bTestGold = true);
int CvLuaUnit::lCanBuild(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const BuildTypes eBuild = (BuildTypes)lua_tointeger(L, 3);
	const bool bTestVisible = luaL_optint(L, 4, 0);
	const bool bTestGold = luaL_optint(L, 5, 1);
	const bool bResult = pkUnit->canBuild(pkPlot, eBuild, bTestVisible, bTestGold);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int canLead(CyPlot* pPlot, int iUnitId) const;
int CvLuaUnit::lCanLead(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const int iUnitId = lua_tointeger(L, 3);

	const bool bResult = pkUnit->canLead(pkPlot, iUnitId);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool lead(int iUnitId);
int CvLuaUnit::lLead(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iUnitId = lua_tointeger(L, 2);
	const bool bResult = pkUnit->lead(iUnitId);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int canGiveExperience(CyPlot* pPlot) const;
int CvLuaUnit::lCanGiveExperience(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	const int iResult = pkUnit->canGiveExperience(pkPlot);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool giveExperience();
int CvLuaUnit::lGiveExperience(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->giveExperience();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canPromote(int /*PromotionTypes*/ ePromotion, int iLeaderUnitId);
int CvLuaUnit::lCanPromote(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PromotionTypes ePromotion = (PromotionTypes)lua_tointeger(L, 2);
	const int iLeaderUnitId = lua_tointeger(L, 3);
	const bool bResult = pkUnit->canPromote(ePromotion, iLeaderUnitId);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void promote(int /*PromotionTypes*/ ePromotion, int iLeaderUnitId);
int CvLuaUnit::lPromote(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PromotionTypes ePromotion = (PromotionTypes)lua_tointeger(L, 2);
	const int iLeaderUnitId = lua_tointeger(L, 3);

	pkUnit->promote(ePromotion, iLeaderUnitId);
	return 0;
}
//------------------------------------------------------------------------------
//int GetUpgradeUnitType();
int CvLuaUnit::lGetUpgradeUnitType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetUpgradeUnitType();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int upgradePrice(int /*UnitTypes*/ eUnit);
int CvLuaUnit::lUpgradePrice(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitTypes eUnit = (UnitTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->upgradePrice(eUnit);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lCanUpgradeRightNow(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bTestVisible = luaL_optint(L, 2, 0);
	const bool bResult = pkUnit->CanUpgradeRightNow(bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lCanUpgradeTo(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitTypes eUpgradeUnitType = (UnitTypes)lua_tointeger(L, 2);
	const bool bTestVisible = luaL_optint(L, 3, 0);
	const bool bResult = pkUnit->CanUpgradeTo(eUpgradeUnitType, bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}
#if defined(MOD_GLOBAL_CS_UPGRADES)
//------------------------------------------------------------------------------
int CvLuaUnit::lCanUpgradeInTerritory(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bTestVisible = luaL_optint(L, 2, 0);
	const bool bResult = pkUnit->CanUpgradeInTerritory(bTestVisible);

	lua_pushboolean(L, bResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
int CvLuaUnit::lGetNumResourceNeededToUpgrade(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes) lua_tointeger(L, 2);
	const UnitTypes eUpgradeUnitType = (UnitTypes) luaL_optint(L, 3, pkUnit->GetUpgradeUnitType());

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUpgradeUnitType);
	if(pkUnitInfo == NULL)
	{
		luaL_error(L, "Cannot find unit info (%d) for unit.", eUpgradeUnitType);
		return 0;
	}

	const int iResult = pkUnitInfo->GetResourceQuantityRequirement(eResource);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetNumResourceTotalNeededToUpgrade(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes)lua_tointeger(L, 2);
	const UnitTypes eUpgradeUnitType = (UnitTypes)luaL_optint(L, 3, pkUnit->GetUpgradeUnitType());

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUpgradeUnitType);
	if (pkUnitInfo == NULL)
	{
		luaL_error(L, "Cannot find unit info (%d) for unit.", eUpgradeUnitType);
		return 0;
	}

	const int iResult = pkUnitInfo->GetResourceQuantityTotal(eResource);

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*HandicapTypes*/ getHandicapType();
int CvLuaUnit::lGetHandicapType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const HandicapTypes eResult = pkUnit->getHandicapType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*CivilizationTypes*/ getCivilizationType();
int CvLuaUnit::lGetCivilizationType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const CivilizationTypes eResult = pkUnit->getCivilizationType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*SpecialUnitTypes*/ getSpecialUnitType();
int CvLuaUnit::lGetSpecialUnitType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const SpecialUnitTypes eResult = pkUnit->getSpecialUnitType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*UnitTypes*/ getCaptureUnitType(int /*CivilizationTypes*/ eCivilization);
int CvLuaUnit::lGetCaptureUnitType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const CivilizationTypes eCivilization = (CivilizationTypes)lua_tointeger(L, 2);
	
	// for modmod compatibility, the lua function takes an argument of type CivilizationTypes. The c++ function needs an argument of type PlayerTypes
	PlayerTypes ePlayer = NO_PLAYER;
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).getCivilizationType() == eCivilization)
		{
			ePlayer = (PlayerTypes)i;
			break;
		}
	}

	const UnitTypes eResult = pkUnit->getCaptureUnitType(ePlayer);
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*UnitCombatTypes*/ getUnitCombatType();
int CvLuaUnit::lGetUnitCombatType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const UnitCombatTypes eResult = (UnitCombatTypes)pkUnit->getUnitCombatType();
	lua_pushinteger(L, eResult);
	return 1;
}
#if defined(MOD_GLOBAL_PROMOTION_CLASSES)
//------------------------------------------------------------------------------
//int /*UnitCombatTypes*/ getUnitPromotionType();
int CvLuaUnit::lGetUnitPromotionType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const UnitCombatTypes eResult = pkUnit->getUnitPromotionType();
	lua_pushinteger(L, eResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//int /*UnitAITypes*/ getUnitAIType();
int CvLuaUnit::lGetUnitAIType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const UnitAITypes eResult = pkUnit->AI_getUnitAIType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setUnitAIType(UnitAITypes eNewValue);
int CvLuaUnit::lSetUnitAIType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitAITypes eUnitAI = (UnitAITypes)lua_tointeger(L, 2);
	pkUnit->AI_setUnitAIType(eUnitAI);
	return 1;
}
//------------------------------------------------------------------------------
//int /*DomainTypes*/ getDomainType();
int CvLuaUnit::lGetDomainType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const DomainTypes eResult = pkUnit->getDomainType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*InvisibleTypes*/ getInvisibleType();
int CvLuaUnit::lGetInvisibleType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const InvisibleTypes eResult = pkUnit->getInvisibleType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*InvisibleTypes*/ getSeeInvisibleType();
int CvLuaUnit::lGetSeeInvisibleType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const InvisibleTypes eResult = pkUnit->getSeeInvisibleType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetDropRange();
int CvLuaUnit::lGetDropRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int eResult = pkUnit->getDropRange();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int flavorValue(int /*FlavorTypes*/ eFlavor);
int CvLuaUnit::lFlavorValue(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const FlavorTypes eFlavor = (FlavorTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->flavorValue(eFlavor);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isBarbarian();
int CvLuaUnit::lIsBarbarian(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isBarbarian();

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool isHuman();
int CvLuaUnit::lIsHuman(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isHuman();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int visibilityRange();
int CvLuaUnit::lVisibilityRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->visibilityRange();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int maxMoves();
int CvLuaUnit::lMaxMoves(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->maxMoves();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int movesLeft();
int CvLuaUnit::lMovesLeft(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->movesLeft();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canMove();
int CvLuaUnit::lCanMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canMove();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool hasMoved();
int CvLuaUnit::lHasMoved(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->hasMoved();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsLinked();
int CvLuaUnit::lIsLinked(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsLinked();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsLinkedLeader();
int CvLuaUnit::lIsLinkedLeader(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsLinkedLeader();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsGrouped();
int CvLuaUnit::lIsGrouped(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsGrouped();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------ 
//bool CanLinkUnits();
int CvLuaUnit::lCanLinkUnits(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const bool bResult = pkUnit->CanLinkUnits();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------ 
//void LinkUnits();
int CvLuaUnit::lLinkUnits(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	pkUnit->LinkUnits();
	return 0;
}
//------------------------------------------------------------------------------
//void UnlinkUnits();
int CvLuaUnit::lUnlinkUnits(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	pkUnit->UnlinkUnits();

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lMoveLinkedLeader(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkDestPlot = CvLuaPlot::GetInstance(L, 2);

	pkUnit->MoveLinkedLeader(pkDestPlot);
	return 0;
}
//------------------------------------------------------------------------------
//void DoGroupMovement();
int CvLuaUnit::lDoGroupMovement(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkDestPlot = CvLuaPlot::GetInstance(L, 2);

	pkUnit->DoGroupMovement(pkDestPlot);
	return 0;
}
//------------------------------------------------------------------------------
// int  GetSquadNumber()
int CvLuaUnit::lGetSquadNumber(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetSquadNumber();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
// AssignToSquad(int iNewSquadNumber)
int CvLuaUnit::lAssignToSquad(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iNewValue = lua_tointeger(L, 2);

	pkUnit->AssignToSquad(iNewValue);
	return 0;
}
//------------------------------------------------------------------------------
// RemoveFromSquad(int iNewSquadNumber)
int CvLuaUnit::lRemoveFromSquad(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	pkUnit->RemoveFromSquad();
	return 0;
}
//------------------------------------------------------------------------------
// void DoSquadMovement(CvPlot* pDestPlot)
int CvLuaUnit::lDoSquadMovement(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkDestPlot = CvLuaPlot::GetInstance(L, 2);

	pkUnit->DoSquadMovement(pkDestPlot);
	return 0;
}
//------------------------------------------------------------------------------
// SetSquadEndMovementType(SquadsEndMovementType endMovementType)
int CvLuaUnit::lSetSquadEndMovementType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const SquadsEndMovementType iNewValue = (SquadsEndMovementType)lua_tointeger(L, 2);

	pkUnit->SetSquadEndMovementType(iNewValue);
	return 0;
}
//------------------------------------------------------------------------------
//int GetRange();
int CvLuaUnit::lRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetRange();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNukeDamageLevel();
int CvLuaUnit::lNukeDamageLevel(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetNukeDamageLevel();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canBuildRoute();
int CvLuaUnit::lCanBuildRoute(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canBuildRoute();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*BuildTypes*/ getBuildType();
int CvLuaUnit::lGetBuildType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const BuildTypes eResult = pkUnit->getBuildType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int workRate(bool bMax);
int CvLuaUnit::lWorkRate(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bMax = lua_toboolean(L, 2);

	const int iResult = pkUnit->workRate(bMax);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetImprovementBuildType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const BuildTypes eBuild = pkUnit->getBuildType();
	const CvBuildInfo* pBuild = GC.getBuildInfo(eBuild);
	int iImprovement = (int)NO_IMPROVEMENT;
	if (pBuild)
	{
		iImprovement = pBuild->getImprovement();
	}


	lua_pushinteger(L, iImprovement);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetRouteBuildType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const BuildTypes eBuild = pkUnit->getBuildType();
	const CvBuildInfo* pBuild = GC.getBuildInfo(eBuild);
	int iRoute = (int)NO_ROUTE;
	if (pBuild)
	{
		iRoute = pBuild->getRoute();
	}


	lua_pushinteger(L, iRoute);
	return 1;
}
#if defined(MOD_CIV6_WORKER)
//------------------------------------------------------------------------------
//int getBuilderStrength();
int CvLuaUnit::lGetBuilderStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getBuilderStrength();
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//bool isNoBadGoodies();
int CvLuaUnit::lIsNoBadGoodies(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isNoBadGoodies();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isOnlyDefensive();
int CvLuaUnit::lIsOnlyDefensive(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isOnlyDefensive();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isNoAttackInOcean();
int CvLuaUnit::lIsNoAttackInOcean(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isNoAttackInOcean();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isNoCapture();
int CvLuaUnit::lIsNoCapture(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isNoCapture();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isRivalTerritory();
int CvLuaUnit::lIsRivalTerritory(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isRivalTerritory();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isFound();
int CvLuaUnit::lIsFound(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isFound();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isFound();
int CvLuaUnit::lIsFoundAbroad(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsFoundAbroad();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
// bool IsWork()
int CvLuaUnit::lIsWork(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsWork();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isGoldenAge();
int CvLuaUnit::lIsGoldenAge(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isGoldenAge();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canCoexistWithEnemyUnit(int /*TeamTypes*/ eTeam);
int CvLuaUnit::lCanCoexistWithEnemyUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->canCoexistWithEnemyUnit(eTeam);

	lua_pushboolean(L, bResult);
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//bool IsContractUnit
int CvLuaUnit::lIsContractUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isContractUnit();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsSpecificContractUnit
int CvLuaUnit::lIsSpecificContractUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	ContractTypes eContract = (ContractTypes)lua_tointeger(L, 2);
	const bool bResult = (pkUnit->getContract() == eContract);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetContractUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const ContractTypes eResult = pkUnit->getContract();
	lua_pushinteger(L, eResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//bool IsGreatPerson();
int CvLuaUnit::lIsGreatPerson(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsGreatPerson();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isFighting();
int CvLuaUnit::lIsFighting(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isFighting();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isAttacking();
int CvLuaUnit::lIsAttacking(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isAttacking();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isDefending();
int CvLuaUnit::lIsDefending(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isDefending();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isInCombat();
int CvLuaUnit::lIsInCombat(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isInCombat();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetMaxHitPointsBase(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getMaxHitPointsBase();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lSetMaxHitPointsBase(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkUnit->setMaxHitPointsBase(iValue);
	return 0;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lChangeMaxHitPointsBase(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iValue = lua_tointeger(L, 2);

	pkUnit->changeMaxHitPointsBase(iValue);
	return 0;
}
//------------------------------------------------------------------------------
//int maxHitPoints();
int CvLuaUnit::lGetMaxHitPoints(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetMaxHitPoints();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int currHitPoints();
int CvLuaUnit::lGetCurrHitPoints(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetCurrHitPoints();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isHurt();
int CvLuaUnit::lIsHurt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsHurt();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isDead();
int CvLuaUnit::lIsDead(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsDead();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isDelayedDeath();
int CvLuaUnit::lIsDelayedDeath(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isDelayedDeath();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setBaseCombatStr(int iCombat);
int CvLuaUnit::lSetBaseCombatStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iCombat = lua_tointeger(L, 2);

	pkUnit->SetBaseCombatStrength(iCombat);
	return 0;
}
//------------------------------------------------------------------------------
//int baseCombatStr();
int CvLuaUnit::lGetBaseCombatStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetBaseCombatStrength();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool HasCombatStrength();
int CvLuaUnit::lIsCombatUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsCombatUnit();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanDefend(CyPlot* pPlot);
int CvLuaUnit::lIsCanDefend(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	//CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2, false);
	const bool bResult = pkUnit->IsCanDefend();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanAttackWithMove();
int CvLuaUnit::lIsCanAttackWithMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsCanAttackWithMove();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanAttackRanged()
int CvLuaUnit::lIsCanAttackRanged(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsCanAttackRanged();
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool CanAttack()
int CvLuaUnit::lIsCanAttack(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsCanAttack();
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool CanAttackWithMoveNow()
int CvLuaUnit::lIsCanAttackWithMoveNow(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsCanAttackWithMoveNow();
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetMaxAttackStrength(const CvPlot* pFromPlot, const CvPlot* pToPlot, const CvUnit* pDefender);
int CvLuaUnit::lGetMaxAttackStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkFromPlot = CvLuaPlot::GetInstance(L, 2, false);
	CvPlot* pToPlot = CvLuaPlot::GetInstance(L, 3, false);
	CvUnit* pkDefender = GetInstance(L, 4, false);

	const int iResult = pkUnit->GetMaxAttackStrength(pkFromPlot, pToPlot, pkDefender);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetMaxDefenseStrength(const CvPlot* pInPlot, const CvUnit* pAttacker);
int CvLuaUnit::lGetMaxDefenseStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pInPlot = CvLuaPlot::GetInstance(L, 2, false);
	CvUnit* pkAttacker = GetInstance(L, 3, false);
	CvPlot* pFromPlot = CvLuaPlot::GetInstance(L, 4, false);
	bool bFromRangedAttack = luaL_optbool(L, 5, false);
	const int iAssumeExtraDamage = luaL_optint(L, 6, 0);

	const int iResult = pkUnit->GetMaxDefenseStrength(pInPlot, pkAttacker, pFromPlot, bFromRangedAttack, false, iAssumeExtraDamage);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int GetEmbarkedUnitDefense();
int CvLuaUnit::lGetEmbarkedUnitDefense(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->GetEmbarkedUnitDefense();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool canSiege(int /*TeamTypes*/ eTeam);
int CvLuaUnit::lCanSiege(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->canSiege(eTeam);

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lIsEnemyInMovementRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bOnlyFortified = lua_toboolean(L, 2);
	const bool bOnlyCities = lua_toboolean(L, 3);

	lua_pushboolean(L, pkUnit->GetPlotsWithEnemyInMovementRange(bOnlyFortified, bOnlyCities).size()>0);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lIsTrade(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	lua_pushboolean(L, pkUnit->isTrade());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lGetTradeRouteIndex(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iRouteIndex = -1;

	if (pkUnit->isTrade()) {
		iRouteIndex = GC.getGame().GetGameTrade()->GetIndexFromUnitID(pkUnit->GetID(), pkUnit->getOwner());
	}

	lua_pushinteger(L, iRouteIndex);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lIsRecalledTrader(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	bool bRecalled = false;

	if (pkUnit->isTrade()) {
		CvGameTrade* pkTrades = GC.getGame().GetGameTrade();
		int iRouteIndex = pkTrades->GetIndexFromUnitID(pkUnit->GetID(), pkUnit->getOwner());
		if (iRouteIndex >= 0) {
			bRecalled = pkTrades->IsRecalledUnit(iRouteIndex);
		}
	}

	lua_pushboolean(L, bRecalled);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lRecallTrader(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bImmediate = lua_toboolean(L, 2);

	if (pkUnit->isTrade()) {
		CvGameTrade* pkTrades = GC.getGame().GetGameTrade();
		int iRouteIndex = pkTrades->GetIndexFromUnitID(pkUnit->GetID(), pkUnit->getOwner());
		if (iRouteIndex >= 0) {
			pkTrades->RecallUnit(iRouteIndex, bImmediate);
		}
	}

	return 0;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lEndTrader(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	if (pkUnit->isTrade()) {
		CvGameTrade* pkTrades = GC.getGame().GetGameTrade();
		int iRouteIndex = pkTrades->GetIndexFromUnitID(pkUnit->GetID(), pkUnit->getOwner());
		if (iRouteIndex >= 0) {
			pkTrades->EndTradeRoute(iRouteIndex);
		}
	}

	return 0;
}

//------------------------------------------------------------------------------
//int airBaseCombatStr();
int CvLuaUnit::lGetBaseRangedCombatStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->GetBaseRangedCombatStrength();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lSetBaseRangedCombatStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iStrength = lua_tointeger(L, 2);
	pkUnit->SetBaseRangedCombatStrength(iStrength);
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lGetDamageCombatModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bRanged = luaL_optbool(L, 2, false);
	const int iDamage = luaL_optint(L, 3, 0);
	const int iResult = pkUnit->GetDamageCombatModifier(bRanged, iDamage);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int airMaxCombatStr(CyUnit* pOther, bool bAttacking);
int CvLuaUnit::lGetMaxRangedCombatStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOther = GetInstance(L, 2, false);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3, false);
	const bool bAttacking = lua_toboolean(L, 4);

	const int iResult = pkUnit->GetMaxRangedCombatStrength(pkOther, pkCity, bAttacking, NULL, NULL, false, false);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canAirAttack();
int CvLuaUnit::lCanAirAttack(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsCanAttackRanged();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canAirDefend(CyPlot* pPlot);
int CvLuaUnit::lCanAirDefend(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2, false);
	const bool bResult = pkUnit->canAirDefend(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int airCombatDamage( CyUnit* pDefender);
int CvLuaUnit::lGetAirCombatDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkDefender = GetInstance(L, 2);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3);
	const bool bIncludeRand = lua_toboolean(L, 4);

	const int iResult = pkUnit->GetAirCombatDamage(pkDefender, pkCity, bIncludeRand);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
// int rangeCombatDamage(const CvUnit* pDefender, CvCity* pCity, bool bIncludeRand) const;
int CvLuaUnit::lGetRangeCombatDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkDefender = GetInstance(L, 2, false);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3, false);
	const bool bIncludeRand = lua_toboolean(L, 4);

	int iResult = pkUnit->GetRangeCombatDamage(pkDefender, pkCity, bIncludeRand);

	//for visual feedback, take care that we show the precise value
	if (pkCity && pkCity->HasGarrison())
	{
		CvUnit* pGarrison = pkCity->GetGarrisonedUnit();
		if (pGarrison)
		{
			int iGarrisonShare = (iResult*2*pGarrison->GetMaxHitPoints()) / (pkCity->GetMaxHitPoints()+2*pGarrison->GetMaxHitPoints());
			iResult -= iGarrisonShare;
		}
	}

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int airCombatDamage( CyUnit* pDefender);
int CvLuaUnit::lGetAirStrikeDefenseDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkAttacker = GetInstance(L, 2);
	const bool bIncludeRand = lua_toboolean(L, 3);

	const int iResult = pkUnit->GetAirStrikeDefenseDamage(pkAttacker, bIncludeRand);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//CvUnit* GetBestInterceptor( CvPlot* pPlot, CvUnit *pDefender, bool bLandInterceptorsOnly, bool bVisibleInterceptorsOnly);
int CvLuaUnit::lGetBestInterceptor(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	CvUnit* pkDefender = GetInstance(L, 3, false);
	const bool bLandInterceptorsOnly = lua_toboolean(L, 4);
	const bool bVisibleInterceptorsOnly = lua_toboolean(L, 5);

	CvUnit* pkBestUnit = 0;
	if(pkPlot)
		pkBestUnit = pkPlot->GetBestInterceptor(pkUnit->getOwner(), pkDefender, bLandInterceptorsOnly, bVisibleInterceptorsOnly);

	CvLuaUnit::Push(L, pkBestUnit);
	return 1;
}
//------------------------------------------------------------------------------
//CvUnit* GetInterceptor Count( CvPlot* pPlot, CvUnit *pDefender, bool bLandInterceptorsOnly, bool bVisibleInterceptorsOnly);
int CvLuaUnit::lGetInterceptorCount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	CvUnit* pkDefender = GetInstance(L, 3, false);
	const bool bLandInterceptorsOnly = lua_toboolean(L, 4);
	const bool bVisibleInterceptorsOnly = lua_toboolean(L, 5);

	int iCount  = 0;
	if(pkPlot)
		iCount = pkPlot->GetInterceptorCount(pkUnit->getOwner(), pkDefender, bLandInterceptorsOnly, bVisibleInterceptorsOnly);

	lua_pushinteger(L, iCount);
	return 1;
}
//------------------------------------------------------------------------------
//CyUnit* bestSeaPillageInterceptor( CyPlot* pPlot);
int CvLuaUnit::lGetBestSeaPillageInterceptor(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	CvUnit* pkBestUnit = 0;
	if(pkPlot)
		pkBestUnit = pkPlot->GetBestInterceptor(pkUnit->getOwner());

	CvLuaUnit::Push(L, pkBestUnit);
	return 1;
}
//------------------------------------------------------------------------------
//int GetCaptureChance (CvUnit *pEnemy);
int CvLuaUnit::lGetCaptureChance(lua_State* L)
{
	int iChance = 0;
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkEnemy = CvLuaUnit::GetInstance(L, 2);

	if(pkEnemy)
	{
		iChance = pkUnit->GetCaptureChance(pkEnemy);
	}

	lua_pushinteger(L, iChance);

	return 1;
}
//------------------------------------------------------------------------------
//bool isAutomated();
int CvLuaUnit::lIsAutomated(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsAutomated();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isWaiting();
int CvLuaUnit::lIsWaiting(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isWaiting();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isFortifyable();
int CvLuaUnit::lIsFortifyable(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canFortify(pkUnit->plot());

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lIsEverFortifyable(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsEverFortifyable();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int fortifyModifier();
int CvLuaUnit::lFortifyModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->fortifyModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int experienceNeeded();
int CvLuaUnit::lExperienceNeeded(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->experienceNeeded();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int maxXPValue();
int CvLuaUnit::lMaxXPValue(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->maxXPValue();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsCanAttackRanged();
int CvLuaUnit::lIsRanged(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsCanAttackRanged();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lIsMustSetUpToRangedAttack(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::isSlowInEnemyLand); //note that this promotion has been redefined
}
//------------------------------------------------------------------------------
int CvLuaUnit::lCanSetUpForRangedAttack(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::canSetUpForRangedAttack);
}
//------------------------------------------------------------------------------
int CvLuaUnit::lIsSetUpForRangedAttack(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::isSetUpForRangedAttack);
}
//------------------------------------------------------------------------------
//bool noDefensiveBonus();
int CvLuaUnit::lNoDefensiveBonus(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->noDefensiveBonus();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool ignoreBuildingDefense();
int CvLuaUnit::lIgnoreBuildingDefense(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->ignoreBuildingDefense();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canMoveImpassable();
int CvLuaUnit::lCanMoveImpassable(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canMoveImpassable();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canMoveAllTerrain();
int CvLuaUnit::lCanMoveAllTerrain(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canMoveAllTerrain();

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool isHoveringUnit();
int CvLuaUnit::lIsHoveringUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsHoveringUnit();

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool flatMovementCost();
int CvLuaUnit::lFlatMovementCost(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->flatMovementCost();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool ignoreTerrainCost();
int CvLuaUnit::lIgnoreTerrainCost(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->ignoreTerrainCost();

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool ignoreTerrainDamage();
int CvLuaUnit::lIgnoreTerrainDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->ignoreTerrainDamage();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool ignoreFeatureDamage();
int CvLuaUnit::lIgnoreFeatureDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->ignoreFeatureDamage();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool extraTerrainDamage();
int CvLuaUnit::lExtraTerrainDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->extraTerrainDamage();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool extraFeatureDamage();
int CvLuaUnit::lExtraFeatureDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->extraFeatureDamage();

	lua_pushboolean(L, bResult);
	return 1;
}

int CvLuaUnit::lGetMovementRules(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOtherUnit = CvLuaUnit::GetInstance(L, 2);
	CvString text = "";
	int iChance = -1;

	if (pkUnit != NULL && pkOtherUnit != NULL && pkOtherUnit->CanPlague(pkUnit))
	{
		vector<int> vInflictedPlagues = pkOtherUnit->GetInflictedPlagueIDs();
		if (vInflictedPlagues.size() > 0)
		{
			//FIXME: The UI currently supports only one plague being inflicted.
			//If anyone is inclined to do so, they can write the code to make the UI support this.
			//For now, just grab the first plague.
			int iPlagueChance = 0;
			PromotionTypes ePlague = pkOtherUnit->GetInflictedPlague(vInflictedPlagues[0], iPlagueChance);
			CvPromotionEntry* pkPlaguePromotionInfo = GC.getPromotionInfo(ePlague);
			int iPlagueID = pkPlaguePromotionInfo->GetPlagueID();
			if (pkPlaguePromotionInfo)
			{
				// Already have this plague, or a stronger version of this plague?
				if (pkUnit->HasPlague(iPlagueID, pkPlaguePromotionInfo->GetPlaguePriority()))
				{
					text = GetLocalizedText("TXT_KEY_UNIT_ALREADY_PLAGUED", pkPlaguePromotionInfo->GetText());
				}
				// Immune to this plague?
				else if (pkUnit->ImmuneToPlague(iPlagueID))
				{
					text = GetLocalizedText("TXT_KEY_UNIT_IMMUNE_PLAGUED", pkPlaguePromotionInfo->GetText());
				}
				else
				{
					text = GetLocalizedText("TXT_KEY_UNIT_PLAGUE_CHANCE", pkPlaguePromotionInfo->GetText());
					iChance = iPlagueChance;
				}
			}
		}
	}

	lua_pushstring(L, text);
	lua_pushinteger(L, iChance);
	return 2;
}

int CvLuaUnit::lGetZOCStatus(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	if (pkUnit == NULL)
	{
		lua_pushstring(L, "");
		return 1;
	}

	if (pkUnit->IsIgnoreZOC())
	{
		Localization::String strMessage = Localization::Lookup("TXT_KEY_UNIT_IGNORE_ZOC");
		
		lua_pushstring(L, strMessage.toUTF8());
		return 1;
	}

	lua_pushstring(L, "");
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetWithdrawChance(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkAttacker = CvLuaUnit::GetInstance(L, 2);
	int iResult;
	if (pkUnit->getExtraWithdrawal() > 0)
		iResult = pkUnit->GetWithdrawChance(*pkAttacker);
	else
		iResult = -1;
		
	lua_pushinteger(L, iResult);
	return 1;
}
#if defined(MOD_PROMOTIONS_IMPROVEMENT_BONUS)
//------------------------------------------------------------------------------
int CvLuaUnit::lGetNearbyImprovementCombatBonus(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->GetNearbyImprovementCombatBonus();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetNearbyImprovementBonusRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->GetNearbyImprovementBonusRange();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetCombatBonusImprovement(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = (int) pkUnit->GetCombatBonusImprovement();

	lua_pushinteger(L, iResult);
	return 1;
}
#endif
#if defined(MOD_PROMOTIONS_CROSS_MOUNTAINS)
//------------------------------------------------------------------------------
//bool canCrossMountains();
int CvLuaUnit::lCanCrossMountains(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canCrossMountains();

	lua_pushboolean(L, bResult);
	return 1;
}
#endif
#if defined(MOD_PROMOTIONS_CROSS_OCEANS)
//------------------------------------------------------------------------------
//bool canCrossOceans();
int CvLuaUnit::lCanCrossOceans(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canCrossOceans();

	lua_pushboolean(L, bResult);
	return 1;
}
#endif
#if defined(MOD_PROMOTIONS_CROSS_ICE)
//------------------------------------------------------------------------------
//bool canCrossIce();
int CvLuaUnit::lCanCrossIce(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canCrossIce();

	lua_pushboolean(L, bResult);
	return 1;
}
#endif
#if defined(MOD_PROMOTIONS_GG_FROM_BARBARIANS)
//------------------------------------------------------------------------------
//bool isGGFromBarbarians();
int CvLuaUnit::lIsGGFromBarbarians(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isGGFromBarbarians();

	lua_pushboolean(L, bResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//bool isNeverInvisible();
int CvLuaUnit::lIsNeverInvisible(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isNeverInvisible();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isInvisible(int /*TeamTypes*/ eTeam, bool bDebug, bCheckCargo = false);
int CvLuaUnit::lIsInvisible(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bDebug = lua_toboolean(L, 3);
	const int iCheckCargo = luaL_optint(L, 4, 0);
	const bool bResult = pkUnit->isInvisible(eTeam, bDebug, iCheckCargo != 0);

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lIsEnemyCityAdjacent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	bool bResult = pkUnit->IsEnemyCityAdjacent();
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//bool isNukeImmune();
int CvLuaUnit::lIsNukeImmune(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isNukeImmune();

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
//int maxInterceptionProbability();
int CvLuaUnit::lGetAirInterceptRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetAirInterceptRange();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int maxInterceptionProbability();
int CvLuaUnit::lMaxInterceptionProbability(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getInterceptChance();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int currInterceptionProbability();
int CvLuaUnit::lCurrInterceptionProbability(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->interceptionProbability();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int evasionProbability();
int CvLuaUnit::lEvasionProbability(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->evasionProbability();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int withdrawalProbability();
int CvLuaUnit::lWithdrawalProbability(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->withdrawalProbability();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetAdjacentModifier();
int CvLuaUnit::lGetAdjacentModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetAdjacentModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNoAdjacentUnitModifier();
int CvLuaUnit::lGetNoAdjacentUnitModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetNoAdjacentUnitModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getAttackModifier();
int CvLuaUnit::lGetAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getDefenseModifier();
int CvLuaUnit::lGetDefenseModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getDefenseModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraRangedDefenseModifier();
int CvLuaUnit::lGetRangedDefenseModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraRangedDefenseModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetRangedAttackModifier();
int CvLuaUnit::lGetRangedAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetRangedAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GarrisonRangedAttackModifier();
int CvLuaUnit::lGetGarrisonRangedAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	int iResult = 0;
	CvPlot* pPlot = pkUnit->plot();
	if (pPlot && pPlot->isCity())
	{
		CvCity* pCity = pPlot->getPlotCity();
		iResult = pCity->getGarrisonRangedAttackModifier();
	}
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int cityAttackModifier();
int CvLuaUnit::lCityAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->cityAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int cityDefenseModifier();
int CvLuaUnit::lCityDefenseModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->cityDefenseModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int hillsAttackModifier();
int CvLuaUnit::lHillsAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->hillsAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int hillsDefenseModifier();
int CvLuaUnit::lHillsDefenseModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->hillsDefenseModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int openAttackModifier();
int CvLuaUnit::lOpenAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->openAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int openRangedAttackModifier();
int CvLuaUnit::lOpenRangedAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->openRangedAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int RoughAttackModifier();
int CvLuaUnit::lRoughAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->roughAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int roughRangedAttackModifier();
int CvLuaUnit::lRoughRangedAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->roughRangedAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int AttackFortifiedModifier();
int CvLuaUnit::lAttackFortifiedModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->attackFortifiedModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int AttackWoundedModifier();
int CvLuaUnit::lAttackWoundedModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->attackWoundedModifier();
	lua_pushinteger(L, iResult);
	return 1;
}

int CvLuaUnit::lAttackFullyHealedModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->attackFullyHealedModifier();
	lua_pushinteger(L, iResult);
	return 1;
}

int CvLuaUnit::lAttackAbove50Modifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->attackAbove50HealthModifier();
	lua_pushinteger(L, iResult);
	return 1;
}

int CvLuaUnit::lAttackBelow50Modifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->attackBelow50HealthModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int IsRangedFlankAttack();
int CvLuaUnit::lIsRangedFlankAttack(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const bool bResult = pkUnit->IsRangedFlankAttack();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetFlankAttackModifier();
int CvLuaUnit::lFlankAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetFlankAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int openDefenseModifier();
int CvLuaUnit::lOpenDefenseModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->openDefenseModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int RoughDefenseModifier();
int CvLuaUnit::lRoughDefenseModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->roughDefenseModifier();
	lua_pushinteger(L, iResult);
	return 1;
}


//------------------------------------------------------------------------------
//int openDefenseModifier();
int CvLuaUnit::lOpenFromModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraOpenFromPercent(); 
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int RoughDefenseModifier();
int CvLuaUnit::lRoughFromModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraRoughFromPercent();
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//int terrainAttackModifier(int /*TerrainTypes*/ eTerrain);
int CvLuaUnit::lTerrainAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TerrainTypes eTerrain = (TerrainTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->terrainAttackModifier(eTerrain);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int terrainDefenseModifier(int /*TerrainTypes*/ eTerrain);
int CvLuaUnit::lTerrainDefenseModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TerrainTypes eTerrain = (TerrainTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->terrainDefenseModifier(eTerrain);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int featureAttackModifier(int /*FeatureTypes*/ eFeature);
int CvLuaUnit::lFeatureAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const FeatureTypes eFeature = (FeatureTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->featureAttackModifier(eFeature);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int featureDefenseModifier(int /*FeatureTypes*/ eFeature);
int CvLuaUnit::lFeatureDefenseModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const FeatureTypes eFeature = (FeatureTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->featureDefenseModifier(eFeature);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int unitClassAttackModifier(int /*UnitClassTypes*/ eUnitClass);
int CvLuaUnit::lUnitClassAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitClassTypes eUnitClass = (UnitClassTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->unitClassAttackModifier(eUnitClass);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int unitClassDefenseModifier(int /*UnitClassTypes*/ eUnitClass);
int CvLuaUnit::lUnitClassDefenseModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitClassTypes eUnitClass = (UnitClassTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->unitClassDefenseModifier(eUnitClass);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int unitCombatModifier(int /*UnitCombatTypes*/ eUnitCombat);
int CvLuaUnit::lUnitCombatModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitCombatTypes eUnitCombat = (UnitCombatTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->unitCombatModifier(eUnitCombat);
	
	lua_pushinteger(L, iResult);
	return 1;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//int getCombatModPerAdjacentUnitCombatModifier(int /*UnitCombatTypes*/ eUnitCombat);
int CvLuaUnit::lPerAdjacentUnitCombatModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	//const UnitCombatTypes eUnitCombat = (UnitCombatTypes)lua_tointeger(L, 2);

	CvPlot* pFromPlot = pkUnit->plot();

	int iResult = 0;
	if (pFromPlot != NULL)
	{
		for (int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
			CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
			int iModPerAdjacent = pkUnit->getCombatModPerAdjacentUnitCombatModifier(eUnitCombat);
			if (pkUnitCombatInfo && iModPerAdjacent != 0)
			{
				int iNumFriendliesAdjacent = pFromPlot->GetNumSpecificFriendlyUnitCombatsAdjacent(pkUnit->getTeam(), eUnitCombat, NULL);
				iResult += (iNumFriendliesAdjacent * iModPerAdjacent);
			}
		}
	}
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getCombatModPerAdjacentUnitCombatAttackMod(int /*UnitCombatTypes*/ eUnitCombat);
int CvLuaUnit::lPerAdjacentUnitCombatAttackMod(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	//const UnitCombatTypes eUnitCombat = (UnitCombatTypes)lua_tointeger(L, 2);

	CvPlot* pFromPlot = pkUnit->plot();
	
	int iResult = 0;
	if (pFromPlot != NULL)
	{
		for (int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
			CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
			int iModPerAdjacent = pkUnit->getCombatModPerAdjacentUnitCombatAttackMod(eUnitCombat);
			if (pkUnitCombatInfo && iModPerAdjacent != 0)
			{
				int iNumFriendliesAdjacent = pFromPlot->GetNumSpecificFriendlyUnitCombatsAdjacent(pkUnit->getTeam(), eUnitCombat, NULL);
				iResult += (iNumFriendliesAdjacent * iModPerAdjacent);
			}
		}
	}
	
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getCombatModPerAdjacentUnitCombatDefenseMod(int /*UnitCombatTypes*/ eUnitCombat);
int CvLuaUnit::lPerAdjacentUnitCombatDefenseMod(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	//const UnitCombatTypes eUnitCombat = (UnitCombatTypes)lua_tointeger(L, 2);

	CvPlot* pFromPlot = pkUnit->plot();
	
	int iResult = 0;
	if (pFromPlot != NULL)
	{
		for (int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
		{
			const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);
			CvBaseInfo* pkUnitCombatInfo = GC.getUnitCombatClassInfo(eUnitCombat);
			int iModPerAdjacent = pkUnit->getCombatModPerAdjacentUnitCombatDefenseMod(eUnitCombat);
			if (pkUnitCombatInfo && iModPerAdjacent != 0)
			{
				int iNumFriendliesAdjacent = pFromPlot->GetNumSpecificFriendlyUnitCombatsAdjacent(pkUnit->getTeam(), eUnitCombat, NULL);
				iResult += (iNumFriendliesAdjacent * iModPerAdjacent);
			}
		}
	}
	
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int unitCombatModifier(int /*UnitCombatTypes*/ eUnitCombat);
int CvLuaUnit::lBarbarianCombatBonus(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetBarbarianCombatBonus();
	
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lIsMounted(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnitEntry* pUnitInfo = GC.getUnitInfo(pkUnit->getUnitType());

	const bool bResult = pUnitInfo->IsMounted();
	
	lua_pushboolean(L, bResult);
	return 1;
}	
//------------------------------------------------------------------------------
int CvLuaUnit::lIsStrongerDamaged(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const bool bResult = pkUnit->IsStrongerDamaged();

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lGetMultiAttackBonus(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOtherUnit = CvLuaUnit::GetInstance(L, 2);

	if (pkUnit == NULL || pkOtherUnit == NULL)
		return 0;

	int iModifier = 0;
	//bonus for attacking same unit over and over in a turn?
	int iTempModifier = pkUnit->getMultiAttackBonus() + GET_PLAYER(pkUnit->getOwner()).GetPlayerTraits()->GetMultipleAttackBonus();
	if (iTempModifier != 0)
	{
		iTempModifier *= pkOtherUnit->GetNumTimesAttackedThisTurn(pkUnit->getOwner());
		iModifier += iTempModifier;
	}

	lua_pushinteger(L, iModifier);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetMultiAttackBonusCity(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2, false);

	if (pkUnit == NULL || pkCity == NULL)
		return 0;

	int iModifier = 0;
	//bonus for attacking same unit over and over in a turn?
	int iTempModifier = pkUnit->getMultiAttackBonus() + GET_PLAYER(pkUnit->getOwner()).GetPlayerTraits()->GetMultipleAttackBonus();
	if (iTempModifier != 0)
	{
		iTempModifier *= pkCity->GetNumTimesAttackedThisTurn(pkUnit->getOwner());
		iModifier += iTempModifier;
	}

	lua_pushinteger(L, iModifier);
	return 1;
}

#endif
//------------------------------------------------------------------------------
//int domainModifier(int /*DomainTypes*/ eDomain);
int CvLuaUnit::lDomainModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->domainModifier(eDomain);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lDomainAttackPercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getExtraDomainAttack(eDomain);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lDomainDefensePercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const DomainTypes eDomain = (DomainTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getExtraDomainDefense(eDomain);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetStrategicResourceCombatPenalty(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetStrategicResourceCombatPenalty();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetUnhappinessCombatPenalty(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetUnhappinessCombatPenalty();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int AirSweepCombatMod();
int CvLuaUnit::lAirSweepCombatMod(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::GetAirSweepCombatModifier);
}
//------------------------------------------------------------------------------
//int GetEmbarkDefensiveModifier();
int CvLuaUnit::lGetEmbarkDefensiveModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::GetEmbarkDefensiveModifier);
}
//------------------------------------------------------------------------------
//int CapitalDefenseModifier();
int CvLuaUnit::lCapitalDefenseModifier(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::GetCapitalDefenseModifier);
}
//------------------------------------------------------------------------------
//int CapitalDefenseFalloff();
int CvLuaUnit::lCapitalDefenseFalloff(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::GetCapitalDefenseFalloff);
}
//------------------------------------------------------------------------------
//int /*SpecialUnitTypes*/ specialCargo();
int CvLuaUnit::lSpecialCargo(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const SpecialUnitTypes eResult = pkUnit->specialCargo();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*DomainTypes*/ domainCargo();
int CvLuaUnit::lDomainCargo(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const DomainTypes eResult = pkUnit->domainCargo();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int cargoSpace();
int CvLuaUnit::lCargoSpace(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->cargoSpace();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void changeCargoSpace(int iChange);
int CvLuaUnit::lChangeCargoSpace(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iChange = lua_tointeger(L, 2);

	pkUnit->changeCargoSpace(iChange);
	return 0;
}
//------------------------------------------------------------------------------
//bool isFull();
int CvLuaUnit::lIsFull(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isFull();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int cargoSpaceAvailable(int /*SpecialUnitTypes*/ eSpecialCargo, int /*DomainTypes*/ eDomainCargo);
int CvLuaUnit::lCargoSpaceAvailable(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->cargoSpaceAvailable();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool hasCargo();
int CvLuaUnit::lHasCargo(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->hasCargo();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canCargoAllMove();
int CvLuaUnit::lCanCargoAllMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canCargoAllMove();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUnitAICargo(UnitAITypes eUnitAI);
int CvLuaUnit::lGetUnitAICargo(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitAITypes eUnitAI = (UnitAITypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getUnitAICargo(eUnitAI);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getID();
int CvLuaUnit::lGetID(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetID();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getHotKeyNumber();
int CvLuaUnit::lGetHotKeyNumber(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getHotKeyNumber();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setHotKeyNumber(int iNewValue);
int CvLuaUnit::lSetHotKeyNumber(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iNewValue = lua_tointeger(L, 2);

	pkUnit->setHotKeyNumber(iNewValue);
	return 0;
}
//------------------------------------------------------------------------------
//int getX();
int CvLuaUnit::lGetX(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getX();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getY();
int CvLuaUnit::lGetY(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getY();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setXY(int iX, int iY, bool bGroup = false, bool bUpdate = true, bool bShow = false, bCheckPlotVisible = false);
int CvLuaUnit::lSetXY(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int x = lua_tointeger(L, 2);
	const int y = lua_tointeger(L, 3);
	const bool bGroup = luaL_optint(L, 4, 0);
	const bool bUpdate = luaL_optint(L, 5, 1);
	const bool bShow = luaL_optint(L, 6, 0);
	const bool bCheckPlotVisible = luaL_optint(L, 7, 0);

	pkUnit->setXY(x, y, bGroup, bUpdate, bShow, bCheckPlotVisible);
	return 0;
}
//------------------------------------------------------------------------------
//bool at(int iX, int iY);
int CvLuaUnit::lAt(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int x = lua_tointeger(L, 2);
	const int y = lua_tointeger(L, 3);

	const bool bResult = pkUnit->at(x, y);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool atPlot(CyPlot* pPlot);
int CvLuaUnit::lAtPlot(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	bool bResult = false;
	if(pkPlot)
	{
		bResult = pkUnit->atPlot(*pkPlot);
	}

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//CyPlot* plot();
int CvLuaUnit::lGetPlot(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	if(!GC.getMap().isPlot(pkUnit->getX(), pkUnit->getY()))
	{
		CvAssertMsg(false, "pUnit plot is not valid. Pushing nil!")
		lua_pushnil(L);
		return 1;
	}

	CvLuaPlot::Push(L, pkUnit->plot());
	return 1;
}
//------------------------------------------------------------------------------
//CyArea* area();
int CvLuaUnit::lGetArea(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	CvLuaArea::Push(L, pkUnit->plot()->area());
	return 1;
}
//------------------------------------------------------------------------------
//CyPlot* getReconPlot();
int CvLuaUnit::lGetReconPlot(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	CvPlot* pkPlot = pkUnit->getReconPlot();
	CvLuaPlot::Push(L, pkPlot);

	return 1;
}
//------------------------------------------------------------------------------
//void setReconPlot(CyPlot* pNewValue);
int CvLuaUnit::lSetReconPlot(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	pkUnit->setReconPlot(pkPlot);
	return 0;
}
//------------------------------------------------------------------------------
//int getGameTurnCreated();
int CvLuaUnit::lGetGameTurnCreated(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getGameTurnCreated();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getLastMoveTurn()
int CvLuaUnit::lGetLastMoveTurn(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getLastMoveTurn();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getDamage();
int CvLuaUnit::lGetDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getDamage();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setDamage(int iNewValue, int /*PlayerTypes*/ ePlayer, bNotifyEntity = true);
int CvLuaUnit::lSetDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iNewValue = lua_tointeger(L, 2);
	const PlayerTypes ePlayer = (lua_isnil(L, 3))? NO_PLAYER : (PlayerTypes)lua_tointeger(L, 3);
	const bool bNotifyEntity = luaL_optint(L, 4, 1);

	pkUnit->setDamage(iNewValue, ePlayer, bNotifyEntity);
	return 0;
}
//------------------------------------------------------------------------------
//void changeDamage(int iChange, int /*PlayerTypes*/ ePlayer);
int CvLuaUnit::lChangeDamage(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iChange = lua_tointeger(L, 2);
	const PlayerTypes ePlayer = (lua_isnil(L, 3))? NO_PLAYER : (PlayerTypes)lua_tointeger(L, 3);

	pkUnit->changeDamage(iChange, ePlayer);
	return 0;
}
//------------------------------------------------------------------------------
//int getMoves();
int CvLuaUnit::lGetMoves(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getMoves();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setMoves(int iNewValue);
int CvLuaUnit::lSetMoves(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iNewValue = lua_tointeger(L, 2);

	pkUnit->setMoves(iNewValue);
	return 0;
}
//------------------------------------------------------------------------------
//void changeMoves(int iChange);
int CvLuaUnit::lChangeMoves(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iChange = lua_tointeger(L, 2);

	pkUnit->changeMoves(iChange);
	return 0;
}
//------------------------------------------------------------------------------
//void finishMoves();
int CvLuaUnit::lFinishMoves(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	pkUnit->finishMoves();
	return 0;
}
//------------------------------------------------------------------------------
//int IsImmobile();
int CvLuaUnit::lIsImmobile(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const bool bResult = pkUnit->IsImmobile();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExperience();
int CvLuaUnit::lGetExperience(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExperienceTimes100() / 100;
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setExperience(int iNewValue, int iMax = -1);
int CvLuaUnit::lSetExperience(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iNewValue = lua_tointeger(L, 2);
	const int iMax = luaL_optint(L, 3, -1);

	pkUnit->setExperienceTimes100(iNewValue * 100, iMax);
	return 0;
}
//------------------------------------------------------------------------------
//void changeExperience(int iChange, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false);
int CvLuaUnit::lChangeExperience(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iChange = lua_tointeger(L, 2);
	const int iMax = luaL_optint(L, 3, -1);
	const bool bFromCombat = luaL_optint(L, 4, 0);
	const bool bInBorders = luaL_optint(L, 5, 0);
	const bool bUpdateGlobal = luaL_optint(L, 6, 0);
	const bool bFromHuman = luaL_optint(L, 7, 0);

	pkUnit->changeExperienceTimes100(iChange * 100, iMax, bFromCombat, bInBorders, bUpdateGlobal, bFromHuman);
	return 0;
}
//------------------------------------------------------------------------------
//int getExperience();
int CvLuaUnit::lGetExperienceTimes100(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExperienceTimes100();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setExperience(int iNewValue, int iMax = -1);
int CvLuaUnit::lSetExperienceTimes100(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iNewValueTimes100 = lua_tointeger(L, 2);
	const int iMax = luaL_optint(L, 3, -1);

	pkUnit->setExperienceTimes100(iNewValueTimes100, iMax);
	return 0;
}
//------------------------------------------------------------------------------
//void changeExperience(int iChange, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false);
int CvLuaUnit::lChangeExperienceTimes100(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iChangeTimes100 = lua_tointeger(L, 2);
	const int iMax = luaL_optint(L, 3, -1);
	const bool bFromCombat = luaL_optint(L, 4, 0);
	const bool bInBorders = luaL_optint(L, 5, 0);
	const bool bUpdateGlobal = luaL_optint(L, 6, 0);
	const bool bFromHuman = luaL_optint(L, 7, 0);

	pkUnit->changeExperienceTimes100(iChangeTimes100, iMax, bFromCombat, bInBorders, bUpdateGlobal, bFromHuman);
	return 0;
}
//------------------------------------------------------------------------------
//int getLevel();
int CvLuaUnit::lGetLevel(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getLevel();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setLevel(int iNewLevel);
int CvLuaUnit::lSetLevel(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iNewLevel = lua_tointeger(L, 2);

	pkUnit->setLevel(iNewLevel);
	return 0;
}
//------------------------------------------------------------------------------
//void changeLevel(int iChange);
int CvLuaUnit::lChangeLevel(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iChange = lua_tointeger(L, 2);

	pkUnit->changeLevel(iChange);
	return 0;
}
//------------------------------------------------------------------------------
//int getFacingDirection();
int CvLuaUnit::lGetFacingDirection(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bCheckLineOfSightProperty = luaL_optint(L, 2, 0);

	const int iResult = pkUnit->getFacingDirection(bCheckLineOfSightProperty);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//void rotateFacingDirectionClockwise();
int CvLuaUnit::lRotateFacingDirectionClockwise(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	pkUnit->rotateFacingDirectionClockwise();
	return 0;
}
//------------------------------------------------------------------------------
//void rotateFacingDirectionCounterClockwise();
int CvLuaUnit::lRotateFacingDirectionCounterClockwise(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	pkUnit->rotateFacingDirectionCounterClockwise();
	return 0;
}
//------------------------------------------------------------------------------
//int getCargo();
int CvLuaUnit::lGetCargo(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getCargo();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getFortifyTurns();
int CvLuaUnit::lGetFortifyTurns(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->IsFortified() ? 1 : 0; //need to fake this
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getBlitzCount();
int CvLuaUnit::lGetBlitzCount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getBlitzCount();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isBlitz();
int CvLuaUnit::lIsBlitz(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isBlitz();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getAmphibCount();
int CvLuaUnit::lGetAmphibCount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getAmphibCount();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isAmphib();
int CvLuaUnit::lIsAmphib(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isAmphibious();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getRiverCrossingNoPenaltyCount();
int CvLuaUnit::lGetRiverCrossingNoPenaltyCount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getRiverCrossingNoPenaltyCount();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isRiverCrossingNoPenalty();
int CvLuaUnit::lIsRiverCrossingNoPenalty(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isRiverCrossingNoPenalty();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isEnemyRoute();
int CvLuaUnit::lIsEnemyRoute(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isEnemyRoute();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isAlwaysHeal();
int CvLuaUnit::lIsAlwaysHeal(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isAlwaysHeal();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isHealOutsideFriendly();
int CvLuaUnit::lIsHealOutsideFriendly(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isHealOutsideFriendly();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isHillsDoubleMove();
int CvLuaUnit::lIsHillsDoubleMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isHillsDoubleMove();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsGarrisoned()
int CvLuaUnit::lIsGarrisoned(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsGarrisoned();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//CvCity* lGetGarrisonedCity()
int CvLuaUnit::lGetGarrisonedCity(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvLuaCity::Push(L, pkUnit->GetGarrisonedCity());

	return 1;
}
//------------------------------------------------------------------------------
//int getExtraVisibilityRange();
int CvLuaUnit::lGetExtraVisibilityRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraVisibilityRange();
	lua_pushinteger(L, iResult);
	return 1;
}
#if defined(MOD_PROMOTIONS_VARIABLE_RECON)
//------------------------------------------------------------------------------
//int getExtraReconRange();
int CvLuaUnit::lGetExtraReconRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraReconRange();
	lua_pushinteger(L, iResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//int getExtraMoves();
int CvLuaUnit::lGetExtraMoves(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraMoves();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraMoveDiscount();
int CvLuaUnit::lGetExtraMoveDiscount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraMoveDiscount();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraRange();
int CvLuaUnit::lGetExtraRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraRange();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraIntercept();
int CvLuaUnit::lGetExtraIntercept(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getInterceptChance();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraEvasion();
int CvLuaUnit::lGetExtraEvasion(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraEvasion();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraWithdrawal();
int CvLuaUnit::lGetExtraWithdrawal(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraWithdrawal();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraEnemyHeal();
int CvLuaUnit::lGetExtraEnemyHeal(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraEnemyHeal();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraNeutralHeal();
int CvLuaUnit::lGetExtraNeutralHeal(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraNeutralHeal();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraFriendlyHeal();
int CvLuaUnit::lGetExtraFriendlyHeal(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraFriendlyHeal();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getSameTileHeal();
int CvLuaUnit::lGetSameTileHeal(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getSameTileHeal();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getAdjacentTileHeal();
int CvLuaUnit::lGetAdjacentTileHeal(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getAdjacentTileHeal();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraCombatPercent();
int CvLuaUnit::lGetExtraCombatPercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraCombatPercent();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetFriendlyLandsModifier();
int CvLuaUnit::lGetFriendlyLandsModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getFriendlyLandsModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetFriendlyLandsAttackModifier();
int CvLuaUnit::lGetFriendlyLandsAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getFriendlyLandsAttackModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetOutsideFriendlyLandsModifier();
int CvLuaUnit::lGetOutsideFriendlyLandsModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getOutsideFriendlyLandsModifier();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraCityAttackPercent();
int CvLuaUnit::lGetExtraCityAttackPercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraCityAttackPercent();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraCityDefensePercent();
int CvLuaUnit::lGetExtraCityDefensePercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraCityDefensePercent();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraHillsAttackPercent();
int CvLuaUnit::lGetExtraHillsAttackPercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraHillsAttackPercent();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraHillsDefensePercent();
int CvLuaUnit::lGetExtraHillsDefensePercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraHillsDefensePercent();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraOpenAttackPercent();
int CvLuaUnit::lGetExtraOpenAttackPercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraOpenAttackPercent();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraOpenRangedAttackMod();
int CvLuaUnit::lGetExtraOpenRangedAttackMod(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraOpenRangedAttackMod();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraRoughAttackPercent();
int CvLuaUnit::lGetExtraRoughAttackPercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraRoughAttackPercent();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraRoughRangedAttackMod();
int CvLuaUnit::lGetExtraRoughRangedAttackMod(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraRoughRangedAttackMod();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraAttackFortifiedMod();
int CvLuaUnit::lGetExtraAttackFortifiedMod(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraAttackFortifiedMod();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraAttackWoundedMod();
int CvLuaUnit::lGetExtraAttackWoundedMod(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraAttackWoundedMod();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraOpenDefensePercent();
int CvLuaUnit::lGetExtraOpenDefensePercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraOpenDefensePercent();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getPillageChange() const;
int CvLuaUnit::lGetPillageChange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->getPillageChange();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getUpgradeDiscount() const;
int CvLuaUnit::lGetUpgradeDiscount(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->getUpgradeDiscount();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExperiencePercent() const;
int CvLuaUnit::lGetExperiencePercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->getExperiencePercent();

	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isOutOfAttacks();
int CvLuaUnit::lIsOutOfAttacks(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isOutOfAttacks();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setMadeAttack(bool bNewValue);
int CvLuaUnit::lSetMadeAttack(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bNewValue = lua_toboolean(L, 2);

	pkUnit->setMadeAttack(bNewValue);
	return 0;
}
//------------------------------------------------------------------------------
//bool isOutOfInterceptions();
int CvLuaUnit::lisOutOfInterceptions(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = !pkUnit->canInterceptNow();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setMadeInterception(bool bNewValue);
int CvLuaUnit::lSetMadeInterception(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bNewValue = lua_toboolean(L, 2);

	if (bNewValue)
		pkUnit->increaseInterceptionCount();
	else
		pkUnit->resetInterceptionCount();

	return 0;
}
//------------------------------------------------------------------------------
//bool isPromotionReady();
int CvLuaUnit::lIsPromotionReady(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isPromotionReady();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setPromotionReady(bool bNewValue);
int CvLuaUnit::lSetPromotionReady(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bNewValue = lua_toboolean(L, 2);

	pkUnit->setPromotionReady(bNewValue);
	return 0;
}
//------------------------------------------------------------------------------
//int getOwner();
int CvLuaUnit::lGetOwner(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getOwner();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getVisualOwner();
int CvLuaUnit::lGetVisualOwner(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getVisualOwner();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getCombatOwner(int /* TeamTypes*/ eForTeam);
int CvLuaUnit::lGetCombatOwner(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TeamTypes eForTeam = (TeamTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getCombatOwner(eForTeam, *(pkUnit->plot()));
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//PlayerTypes getOriginalOwner();
int CvLuaUnit::lGetOriginalOwner(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::GetOriginalOwner);
}
//------------------------------------------------------------------------------
//void setOriginalOwner(int /*UnitTypes*/ leaderUnitType);
int CvLuaUnit::lSetOriginalOwner(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PlayerTypes ePlayerType = (PlayerTypes)lua_tointeger(L, 2);

	pkUnit->SetOriginalOwner(ePlayerType);
	return 0;
}
//------------------------------------------------------------------------------
//int getTeam();
int CvLuaUnit::lGetTeam(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getTeam();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*UnitTypes*/ getUnitType();
int CvLuaUnit::lGetUnitType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const UnitTypes eResult = pkUnit->getUnitType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*UnitClassTypes*/ getUnitClassType();
int CvLuaUnit::lGetUnitClassType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const UnitClassTypes eResult = pkUnit->getUnitClassType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*UnitTypes*/ getLeaderUnitType();
int CvLuaUnit::lGetLeaderUnitType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const UnitTypes eResult = pkUnit->getLeaderUnitType();
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setLeaderUnitType(int /*UnitTypes*/ leaderUnitType);
int CvLuaUnit::lSetLeaderUnitType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitTypes eLeaderUnitType = (UnitTypes)lua_tointeger(L, 2);

	pkUnit->setLeaderUnitType(eLeaderUnitType);
	return 0;
}
//------------------------------------------------------------------------------
//bool IsNearGreatGeneral();
int CvLuaUnit::lIsNearGreatGeneral(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->IsNearGreatGeneral();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetGreatGeneralAuraBonus(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	int iActualBonus = GET_PLAYER(pkUnit->getOwner()).GetAreaEffectModifier(AE_GREAT_GENERAL, pkUnit->getDomainType(), pkUnit->plot());
	int iDefaultBonus = GET_PLAYER(pkUnit->getOwner()).GetGreatGeneralCombatBonus() + GET_PLAYER(pkUnit->getOwner()).GetPlayerTraits()->GetGreatGeneralExtraBonus();
	lua_pushinteger(L, iActualBonus-iDefaultBonus);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsStackedGreatGeneral();
int CvLuaUnit::lIsStackedGreatGeneral(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const bool bResult = pkUnit->IsStackedGreatGeneral();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsIgnoresGreatGeneralBenefit();
int CvLuaUnit::lIsIgnoreGreatGeneralBenefit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const bool bResult = pkUnit->IsIgnoreGreatGeneralBenefit();
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool GetReverseGreatGeneralModifier();
int CvLuaUnit::lGetReverseGreatGeneralModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int bResult = pkUnit->GetReverseGreatGeneralModifier();
	lua_pushinteger(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool GetGreatGeneralCombatModifier();
int CvLuaUnit::lGetGreatGeneralCombatModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int bResult = pkUnit->GetGreatGeneralCombatModifier();
	lua_pushinteger(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetAuraRange();
int CvLuaUnit::lGetAuraRange(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetAuraRangeChange() + /*2*/ GD_INT_GET(GREAT_GENERAL_RANGE);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetAuraEffect();
int CvLuaUnit::lGetAuraEffect(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	CvPlayerAI& kPlayer = GET_PLAYER(pkUnit->getOwner());

	const int iResult = kPlayer.GetGreatGeneralCombatBonus() + kPlayer.GetPlayerTraits()->GetGreatGeneralExtraBonus() + pkUnit->GetAuraEffectChange();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsNearSapper(CvCity* pTargetCity);
int CvLuaUnit::lIsNearSapper(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2, false);
	if (!pkCity)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	int iBonus = GET_PLAYER(pkUnit->getOwner()).GetAreaEffectModifier(AE_SAPPER, NO_DOMAIN, pkCity->plot());
	lua_pushboolean(L, iBonus ==  /*50 in CP, 40 in VP*/ GD_INT_GET(SAPPED_CITY_ATTACK_MODIFIER));
	return 1;
}

#if defined(MOD_BALANCE_CORE)
int CvLuaUnit::lIsHalfNearSapper(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2, false);

	if (!pkCity)
	{
		lua_pushboolean(L, false);
		return 1;
	}

	int iBonus = GET_PLAYER(pkUnit->getOwner()).GetAreaEffectModifier(AE_SAPPER, NO_DOMAIN, pkCity->plot());
	lua_pushboolean(L, iBonus > 0 && iBonus <  /*50 in CP, 40 in VP*/ GD_INT_GET(SAPPED_CITY_ATTACK_MODIFIER));
	return 1;
}

int CvLuaUnit::lGetSapperAreaEffectBonus(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2, false);
	if (!pkCity)
	{
		lua_pushinteger(L, 0);
		return 1;
	}

	const int iResult = GET_PLAYER(pkUnit->getOwner()).GetAreaEffectModifier(AE_SAPPER, NO_DOMAIN, pkCity->plot());
	lua_pushinteger(L, iResult);
	return 1;
}

int CvLuaUnit::lGetGiveCombatModToUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	if (MOD_CORE_AREA_EFFECT_PROMOTIONS)
	{
		const int bResult = pkUnit->GetGiveCombatModToUnit();
		lua_pushinteger(L, bResult);
	}
	else
		lua_pushinteger(L, 0);

	return 1;
}
int CvLuaUnit::lGetNearbyCityBonusCombatMod(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int bResult = pkUnit->GetNearbyCityBonusCombatMod();
	lua_pushinteger(L, bResult);

	return 1;
}

//bool GetNearbyUnitClassModifierFromUnitClass();
int CvLuaUnit::lGetNearbyUnitClassModifierFromUnitClass(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int bResult = pkUnit->GetNearbyUnitClassModifierFromUnitClass(pkUnit->plot());
	lua_pushinteger(L, bResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//bool GetNearbyImprovementModifier();
int CvLuaUnit::lGetNearbyImprovementModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
#if defined(MOD_BALANCE_CORE_MILITARY)
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2, false);
	const int bResult = pkUnit->GetNearbyImprovementModifier(pkPlot);
#else
	const int bResult = pkUnit->GetNearbyImprovementModifier();
#endif
	lua_pushinteger(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsFriendlyUnitAdjacent(bool bCombatUnit);
int CvLuaUnit::lIsFriendlyUnitAdjacent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bCombatUnit = lua_toboolean(L, 2);

	const bool bResult = pkUnit->IsFriendlyUnitAdjacent(bCombatUnit);
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsNoFriendlyUnitAdjacent(bool bCombatUnit);
int CvLuaUnit::lIsNoFriendlyUnitAdjacent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bCombatUnit = lua_toboolean(L, 2);

	const bool bResult = !(pkUnit->IsFriendlyUnitAdjacent(bCombatUnit));
	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool GetNumEnemyUnitsAdjacent(CvUnit* pOtherUnit);
int CvLuaUnit::lGetNumEnemyUnitsAdjacent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOtherUnit = CvLuaUnit::GetInstance(L, 2);

	const int iResult = pkUnit->GetNumEnemyUnitsAdjacent(pkOtherUnit);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//CyUnit* getTransportUnit() const;
int CvLuaUnit::lGetTransportUnit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkTransportUnit = pkUnit->getTransportUnit();

	CvLuaUnit::Push(L, pkTransportUnit);
	return 1;
}
//------------------------------------------------------------------------------
//bool isCargo();
int CvLuaUnit::lIsCargo(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isCargo();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraDomainModifier(int /*DomainTypes*/ eIndex);
int CvLuaUnit::lGetExtraDomainModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const DomainTypes eIndex = (DomainTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getExtraDomainModifier(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//string GetName();
int CvLuaUnit::lGetName(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	lua_pushstring(L, pkUnit->getName());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lGetNameNoDesc(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvString name = pkUnit->getNameNoDesc();
	lua_pushstring(L, name.c_str());
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lHasName(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvString name = pkUnit->getNameNoDesc();
	lua_pushboolean(L, !name.IsEmpty());
	return 1;
}
//------------------------------------------------------------------------------
//string GetNameKey();
int CvLuaUnit::lGetNameKey(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	lua_pushstring(L, pkUnit->getNameKey());
	return 1;
}
//------------------------------------------------------------------------------
//void setName(std::string szNewValue);
int CvLuaUnit::lSetName(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvString strName = lua_tostring(L, 2);

	pkUnit->setName(strName);
	return 0;
}
//------------------------------------------------------------------------------
//string GetCityName();
int CvLuaUnit::lGetCityName(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvCity* pCity = pkUnit->getOriginCity();
	CvString strName = pCity == NULL ? "" : pCity->getName();
	lua_pushstring(L, strName);
	return 1;
}
//------------------------------------------------------------------------------
//bool isTerrainDoubleMove(int /*TerrainTypes*/ eIndex);
int CvLuaUnit::lIsTerrainDoubleMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TerrainTypes eIndex = (TerrainTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->isTerrainDoubleMove(eIndex);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isFeatureDoubleMove(int /*FeatureTypes*/ eIndex);
int CvLuaUnit::lIsFeatureDoubleMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const FeatureTypes eIndex = (FeatureTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->isFeatureDoubleMove(eIndex);

	lua_pushboolean(L, bResult);
	return 1;
}
#if defined(MOD_PROMOTIONS_HALF_MOVE)
//------------------------------------------------------------------------------
//bool isTerrainHalfMove(int /*TerrainTypes*/ eIndex);
int CvLuaUnit::lIsTerrainHalfMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TerrainTypes eIndex = (TerrainTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->isTerrainHalfMove(eIndex);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isFeatureHalfMove(int /*FeatureTypes*/ eIndex);
int CvLuaUnit::lIsFeatureHalfMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const FeatureTypes eIndex = (FeatureTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->isFeatureHalfMove(eIndex);

	lua_pushboolean(L, bResult);
	return 1;
}
#endif
//------------------------------------------------------------------------------
//string getScriptData() const;
int CvLuaUnit::lGetScriptData(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	std::string sScriptData = pkUnit->getScriptData();

	lua_pushstring(L, sScriptData.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//void setScriptData(string szNewValue);
int CvLuaUnit::lSetScriptData(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const char* strNewValue = lua_tostring(L, 2);

	pkUnit->setScriptData(strNewValue);
	return 0;
}
//------------------------------------------------------------------------------
//int getScenarioData();
int CvLuaUnit::lGetScenarioData(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iData = pkUnit->getScenarioData();

	lua_pushinteger(L, iData);
	return 1;
}
//------------------------------------------------------------------------------
//void setScenarioData(int iNewValue);
int CvLuaUnit::lSetScenarioData(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iNewData = lua_tointeger(L, 2);

	pkUnit->setScenarioData(iNewData);
	return 0;
}
//------------------------------------------------------------------------------
//int getExtraTerrainAttackPercent(int /*TerrainTypes*/ eIndex);
int CvLuaUnit::lGetExtraTerrainAttackPercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TerrainTypes eIndex = (TerrainTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getExtraTerrainAttackPercent(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraTerrainDefensePercent(int /*TerrainTypes*/ eIndex);
int CvLuaUnit::lGetExtraTerrainDefensePercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TerrainTypes eIndex = (TerrainTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getExtraTerrainDefensePercent(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraFeatureAttackPercent(int /*FeatureTypes*/ eIndex);
int CvLuaUnit::lGetExtraFeatureAttackPercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const FeatureTypes eIndex = (FeatureTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getExtraFeatureAttackPercent(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraFeatureDefensePercent(int /*FeatureTypes*/ eIndex);
int CvLuaUnit::lGetExtraFeatureDefensePercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const FeatureTypes eIndex = (FeatureTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getExtraFeatureDefensePercent(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraUnitCombatModifier(int /*UnitCombatTypes*/ eIndex);
int CvLuaUnit::lGetExtraUnitCombatModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitCombatTypes eIndex = (UnitCombatTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getExtraUnitCombatModifier(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetUnitClassModifier(int /*UnitClassTypes*/ eIndex);
int CvLuaUnit::lGetUnitClassModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const UnitClassTypes eIndex = (UnitClassTypes)lua_tointeger(L, 2);

	const int iResult = pkUnit->getUnitClassModifier(eIndex);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canAcquirePromotion(int /*PromotionTypes*/ ePromotion);
int CvLuaUnit::lCanAcquirePromotion(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PromotionTypes ePromotion = (PromotionTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->canAcquirePromotion(ePromotion);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canAcquirePromotionAny();
int CvLuaUnit::lCanAcquirePromotionAny(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canAcquirePromotionAny();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isPromotionValid(int /*PromotionTypes*/ ePromotion);
int CvLuaUnit::lIsPromotionValid(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PromotionTypes ePromotion = (PromotionTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->isPromotionValid(ePromotion);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isHasPromotion(int /*PromotionTypes*/ ePromotion);
int CvLuaUnit::lIsHasPromotion(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PromotionTypes ePromotion = (PromotionTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->isHasPromotion(ePromotion);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setHasPromotion(int /*PromotionTypes*/ eIndex, bool bNewValue);
int CvLuaUnit::lSetHasPromotion(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PromotionTypes eIndex = (PromotionTypes)lua_tointeger(L, 2);
	const bool bNewValue = lua_toboolean(L, 3);

	pkUnit->setHasPromotion(eIndex, bNewValue);
	return 0;
}
int CvLuaUnit::lGetPromotionDuration(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PromotionTypes ePromotion = (PromotionTypes)lua_tointeger(L, 2);
	const int iResult = pkUnit->getPromotionDuration(ePromotion);

	lua_pushinteger(L, iResult);
	return 1;
}
int CvLuaUnit::lGetTurnPromotionGained(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const PromotionTypes ePromotion = (PromotionTypes)lua_tointeger(L, 2);
	const int iResult = pkUnit->getTurnPromotionGained(ePromotion);

	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
//ReligionTypes GetReligion();
int CvLuaUnit::lGetReligion(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	ReligionTypes eReligion = pkUnit->GetReligionData()->GetReligion();
	lua_pushinteger(L, (int)eReligion);

	return 1;
}
//------------------------------------------------------------------------------
//int GetConversionStrength();
int CvLuaUnit::lGetConversionStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 2, false);
	int iReligiousStrength = pkUnit->GetConversionStrength(pkCity);

	lua_pushinteger(L, iReligiousStrength);

	return 1;
}
//------------------------------------------------------------------------------
//int GetReligionSpreads();
int CvLuaUnit::lGetSpreadsLeft(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iResult = pkUnit->GetReligionData()->GetSpreadsLeft(pkUnit);
	lua_pushinteger(L, iResult);

	return 1;
}
//int GetReligionSpreads();
int CvLuaUnit::lGetChargesLeft(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iCharge = pkUnit->GetNumRepairCharges();
	lua_pushinteger(L, iCharge);

	return 1;
}
//------------------------------------------------------------------------------
//int GetNumFollowersAfterInquisitor();
int CvLuaUnit::lGetNumFollowersAfterInquisitor(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iData = pkUnit->GetNumFollowersAfterInquisitor();

	lua_pushinteger(L, iData);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumFollowersAfterInquisitor();
int CvLuaUnit::lGetMajorityReligionAfterInquisitor(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iData = pkUnit->GetMajorityReligionAfterInquisitor();

	lua_pushinteger(L, iData);
	return 1;
}
//------------------------------------------------------------------------------
//int GetNumFollowersAfterSpread();
int CvLuaUnit::lGetNumFollowersAfterSpread(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iData = pkUnit->GetNumFollowersAfterSpread();

	lua_pushinteger(L, iData);
	return 1;
}
//------------------------------------------------------------------------------
//ReligionTypes GetMajorityReligionAfterSpread();
int CvLuaUnit::lGetMajorityReligionAfterSpread(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iData = pkUnit->GetMajorityReligionAfterSpread();

	lua_pushinteger(L, iData);
	return 1;
}

//------------------------------------------------------------------------------
//void SetReligion(ReligionTypes);
int CvLuaUnit::lSetReligion(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iReligion = lua_tointeger(L, 2);

	pkUnit->GetReligionDataMutable()->SetReligion((ReligionTypes) iReligion);
	return 0;
}
//------------------------------------------------------------------------------
//void SetConversionStrength(int);
int CvLuaUnit::lSetConversionStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iStrength = lua_tointeger(L, 2);

	pkUnit->GetReligionDataMutable()->SetReligiousStrength(iStrength);
	return 0;
}
//------------------------------------------------------------------------------
//void SetReligionSpreads(int);
int CvLuaUnit::lSetSpreadsLeft(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iSpreadsLeft = lua_tointeger(L, 2);

	//need to do some gymnastics here
	int iSpreadsUsed = pkUnit->GetReligionData()->GetMaxSpreads(pkUnit) - iSpreadsLeft;

	pkUnit->GetReligionDataMutable()->SetSpreadsUsed(iSpreadsUsed);
	return 0;
}

//------------------------------------------------------------------------------
//int GetTourismBlastStrength();
int CvLuaUnit::lGetTourismBlastStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	int iStrength = pkUnit->GetTourismBlastStrength();

	lua_pushinteger(L, iStrength);

	return 1;
}
//------------------------------------------------------------------------------
//int GetTourismBlastLtrength();
int CvLuaUnit::lGetTourismBlastLength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	int iStrength = pkUnit->GetTourismBlastLength();

	lua_pushinteger(L, iStrength);

	return 1;
}
//------------------------------------------------------------------------------
//GreatWorkSlotType GetGreatWorkSlotType();
int CvLuaUnit::lGetGreatWorkSlotType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iData = (int)CultureHelpers::GetGreatWorkSlot(pkUnit->GetGreatWork());

	lua_pushinteger(L, iData);
	return 1;
}
//------------------------------------------------------------------------------
//void rangeStrike(int iX, int iY);
int CvLuaUnit::lRangeStrike(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int x = lua_tointeger(L, 2);
	const int y = lua_tointeger(L, 3);

	if(pkUnit)
		CvUnitCombat::AttackRanged(*pkUnit, x, y, CvUnitCombat::ATTACK_OPTION_NONE);
	return 0;
}
//------------------------------------------------------------------------------
//int GetUnitFlagIconOffset
int CvLuaUnit::lGetUnitFlagIconOffset(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnitEntry& unitInfo = pkUnit->getUnitInfo();
	lua_pushinteger(L, unitInfo.GetUnitFlagIconOffset());

	return 1;
}
//------------------------------------------------------------------------------
//int GetUnitPortraitOffset
int CvLuaUnit::lGetUnitPortraitOffset(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnitEntry& unitInfo = pkUnit->getUnitInfo();
	lua_pushinteger(L, unitInfo.GetUnitPortraitOffset());

	return 1;
}

//------------------------------------------------------------------------------
//void SetActivityType;
int CvLuaUnit::lSetActivityType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const ActivityTypes eActivity = (ActivityTypes)lua_tointeger(L, 2);
	pkUnit->SetActivityType(eActivity);

	return 0;
}

//------------------------------------------------------------------------------
//
int CvLuaUnit::lGetActivityType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	int iActivityType = NO_ACTIVITY;
	if(pkUnit)
	{
		iActivityType = pkUnit->GetActivityType();
	}
	lua_pushinteger(L, iActivityType);

	return 1;
}

//------------------------------------------------------------------------------
// bool ReadyToMove() const;
int CvLuaUnit::lIsReadyToMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	bool bResult = pkUnit->ReadyToMove();
	lua_pushboolean(L, bResult);

	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lIsBusy(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	bool bResult = pkUnit->IsBusy();
	lua_pushboolean(L, bResult);

	return 1;
}

//------------------------------------------------------------------------------
//void PushMission(MissionTypes eMission, int iData1 = -1, int iData2 = -1, int iFlags = 0, bool bAppend = false, bool bManual = fa;se, MissionAITypes eMissionAI = NO_MISSIONAI, CyPlot* pMissionAIPlot = NULL, CyUnit* pMissionAIUnit = NULL)
int CvLuaUnit::lPushMission(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const MissionTypes eMission = (MissionTypes)lua_tointeger(L, 2);
	const int iData1 = luaL_optint(L, 3, -1);
	const int iData2 = luaL_optint(L, 4, -1);
	const int iFlags = luaL_optint(L, 5, 0);
	const bool bAppend = luaL_optint(L, 6, 0);
	const bool bManual = luaL_optint(L, 7, 0);
	const MissionAITypes eMissionAI = (MissionAITypes)luaL_optint(L, 8, NO_MISSIONAI);
	CvPlot* pkMissionAIPlot = CvLuaPlot::GetInstance(L, 9, false);
	CvUnit* pkMissionAIUnit = CvLuaUnit::GetInstance(L, 10, false);

	pkUnit->PushMission(eMission, iData1, iData2, iFlags, bAppend, bManual, eMissionAI, pkMissionAIPlot, pkMissionAIUnit);

	return 0;
};
//------------------------------------------------------------------------------
//void PopMission()
int CvLuaUnit::lPopMission(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	pkUnit->PopMission();

	return 0;
};
//------------------------------------------------------------------------------
//CyPlot* LastMissionPlot();
int CvLuaUnit::lLastMissionPlot(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = pkUnit->LastMissionPlot();
	CvLuaPlot::Push(L, pkPlot);

	return 1;
}
//------------------------------------------------------------------------------
//bool CanStartMission(int iMission, int iData1, int iData2, CyPlot* pPlot = NULL, bool bTestVisible = false)
int CvLuaUnit::lCanStartMission(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iMission = lua_tointeger(L, 2);
	const int iData1 = lua_tointeger(L, 3);
	const int iData2 = lua_tointeger(L, 4);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 5, false);
	const bool bTestVisible = luaL_optint(L, 6, 0);

	const bool bResult = pkUnit->CanStartMission(iMission, iData1, iData2, pkPlot, bTestVisible);
	lua_pushboolean(L, bResult);
	return 1;
};
//------------------------------------------------------------------------------
//bool ExecuteSpecialExploreMove (CvPlot *pPlot);
int CvLuaUnit::lExecuteSpecialExploreMove(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2, false);

	CvPlayerAI& kPlayer = GET_PLAYER(pkUnit->getOwner());

	const bool bResult = kPlayer.GetHomelandAI()->ExecuteSpecialExploreMove(pkUnit, pkPlot);
	lua_pushboolean(L, bResult);
	return 1;
};
//------------------------------------------------------------------------------
//void SetDeployFromOperationTurn(int iTurn);
int CvLuaUnit::lSetDeployFromOperationTurn(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iTurn = lua_tointeger(L, 2);

	pkUnit->SetDeployFromOperationTurn(iTurn);
	return 0;
}
#if defined(MOD_BALANCE_CORE)
//------------------------------------------------------------------------------
//bool IsHigherPopThan(CvUnit *pOtherUnit);
int CvLuaUnit::lIsHigherPopThan(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOtherUnit = CvLuaUnit::GetInstance(L, 2);
	const bool bResult = pkUnit->IsHigherPopThan(pkOtherUnit);

	lua_pushboolean(L, bResult);
	return 1;
}
//int GetResistancePower(CvUnit *pOtherUnit);
int CvLuaUnit::lGetResistancePower(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOtherUnit = CvLuaUnit::GetInstance(L, 2);
	int iResistancePower = pkUnit->GetResistancePower(pkOtherUnit);
	lua_pushinteger(L, iResistancePower);

	return 1;
}
//int GetAllianceCSStrength();
int CvLuaUnit::lGetAllianceCSStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iStrengthMod = 0;
	if(GET_PLAYER(pkUnit->getOwner()).isMinorCiv())
	{
		PlayerTypes eAlly = GET_PLAYER(pkUnit->getOwner()).GetMinorCivAI()->GetAlly();
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
							if(iNumAllies >= /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH))
							{
								break;
							}
						}
					}
				}
				if(iNumAllies > /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH))
				{
					iNumAllies = /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH);
				}
				iStrengthMod = (iCSBonus * iNumAllies);
			}
		}
	}
	else
	{
		int iCSBonus = GET_PLAYER(pkUnit->getOwner()).GetPlayerTraits()->GetAllianceCSStrength();
		if(iCSBonus > 0)
		{
			int iNumAllies = 0;
			// Loop through all minors and get the total number we've met.
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes eMinor = (PlayerTypes) iPlayerLoop;

				if (GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
				{
					if (GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(pkUnit->getOwner()))
					{
						iNumAllies++;
						if(iNumAllies >= /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH))
						{
							break;
						}
					}
				}
			}
			if(iNumAllies > /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH))
			{
				iNumAllies = /*5*/ GD_INT_GET(BALANCE_MAX_CS_ALLY_STRENGTH);
			}
			iStrengthMod = (iCSBonus * iNumAllies);
		}
	}
	lua_pushinteger(L, iStrengthMod);

	return 1;
}
#endif
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
//int GetMonopolyAttackBonus();
int CvLuaUnit::lGetMonopolyAttackBonus(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iAttackBonus = 0;
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
		if(eResourceLoop != NO_RESOURCE)
		{
			CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
			if (pInfo && pInfo->isMonopoly())
			{
				// Strategic monopolies
				if (GET_PLAYER(pkUnit->getOwner()).HasStrategicMonopoly(eResourceLoop) && (pInfo->getMonopolyAttackBonus() > 0 || pInfo->getMonopolyAttackBonus(MONOPOLY_STRATEGIC) > 0))
				{
					iAttackBonus +=  pInfo->getMonopolyAttackBonus();
					iAttackBonus += pInfo->getMonopolyAttackBonus(MONOPOLY_STRATEGIC);
				}
				// Global monopolies
				if (GET_PLAYER(pkUnit->getOwner()).HasGlobalMonopoly(eResourceLoop) && pInfo->getMonopolyAttackBonus(MONOPOLY_GLOBAL) > 0)
				{
					int iTempBonus = pInfo->getMonopolyAttackBonus(MONOPOLY_GLOBAL);
					if (iTempBonus != 0)
					{
						iTempBonus += GET_PLAYER(pkUnit->getOwner()).GetMonopolyModPercent(); // Global monopolies get the mod percent boost from policies.
					}

					iAttackBonus += iTempBonus;
				}
			}
		}
	}
	lua_pushinteger(L, iAttackBonus);

	return 1;
}
//int GetMonopolyDefenseBonus();
int CvLuaUnit::lGetMonopolyDefenseBonus(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iDefenseBonus = 0;
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResourceLoop = (ResourceTypes) iResourceLoop;
		if(eResourceLoop != NO_RESOURCE)
		{
			CvResourceInfo* pInfo = GC.getResourceInfo(eResourceLoop);
			if (pInfo && pInfo->isMonopoly())
			{
				// Strategic monopolies
				if (GET_PLAYER(pkUnit->getOwner()).HasStrategicMonopoly(eResourceLoop) && (pInfo->getMonopolyDefenseBonus() > 0 || pInfo->getMonopolyDefenseBonus(MONOPOLY_STRATEGIC) > 0))
				{
					iDefenseBonus +=  pInfo->getMonopolyDefenseBonus();
					iDefenseBonus += pInfo->getMonopolyAttackBonus(MONOPOLY_STRATEGIC);
				}
				// Global monopolies
				if (GET_PLAYER(pkUnit->getOwner()).HasGlobalMonopoly(eResourceLoop) && pInfo->getMonopolyDefenseBonus(MONOPOLY_GLOBAL) > 0)
				{
					int iTempBonus = pInfo->getMonopolyDefenseBonus(MONOPOLY_GLOBAL);
					if (iTempBonus != 0)
					{
						iTempBonus += GET_PLAYER(pkUnit->getOwner()).GetMonopolyModPercent(); // Global monopolies get the mod percent boost from policies.
					}

					iDefenseBonus += iTempBonus;
				}
			}
		}
	}
	lua_pushinteger(L, iDefenseBonus);

	return 1;
}
#endif
//------------------------------------------------------------------------------
//bool IsHigherTechThan(UnitTypes eOtherUnit );
int CvLuaUnit::lIsHigherTechThan(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	UnitTypes eOtherUnit = (UnitTypes)lua_tointeger(L, 2);
	const bool bResult = pkUnit->IsHigherTechThan(eOtherUnit);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsLargerCivThan(CvUnit *pOtherUnit);
int CvLuaUnit::lIsLargerCivThan(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOtherUnit = CvLuaUnit::GetInstance(L, 2);
	const bool bResult = pkUnit->IsLargerCivThan(pkOtherUnit);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsRangedSupportFire();
int CvLuaUnit::lIsRangedSupportFire(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isRangedSupportFire();

	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lAddMessage(lua_State* L)
{
	CvUnit* pUnit = GetInstance(L);
	const char* szMessage = lua_tostring(L, 2);
	const PlayerTypes ePlayer = (PlayerTypes) luaL_optinteger(L, 3, pUnit->getOwner());

	SHOW_UNIT_MESSAGE(pUnit, ePlayer, szMessage);
	return 0;
}

#if defined(MOD_BALANCE_CORE_MILITARY)
#include "../CvPlayerAI.h"

//------------------------------------------------------------------------------
int CvLuaUnit::lGetAIOperationInfo(lua_State* L)
{
	CvUnit* pUnit = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes)pUnit->getOwner();
	int iArmyID = pUnit->getArmyID();
	if (ePlayer!=NO_PLAYER && iArmyID!=-1)
	{
		CvArmyAI* pArmy = CvPlayerAI::getPlayer(ePlayer).getArmyAI(iArmyID);
		if (pArmy)
		{
			CvAIOperation* pAIOp = CvPlayerAI::getPlayer(ePlayer).getAIOperation(pArmy->GetOperationID());
			if (pAIOp)
			{
				lua_pushstring(L, pAIOp->GetInfoString());
				return 1;
			}
			else
			{
				lua_pushstring(L, "warning: army without operation!");
				return 1;
			}
		}
		else
		{
			lua_pushstring(L, "warning: invalid army ID!");
			return 1;
		}
	}

	lua_pushstring(L, "");
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lGetMissionInfo(lua_State* L)
{
	CvUnit* pUnit = GetInstance(L);
	lua_pushstring(L, pUnit->GetMissionInfo());
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaUnit::lGetDanger(lua_State* L)
{
	CvUnit* pUnit = GetInstance(L);
	int iDanger = pUnit->GetDanger();

	lua_pushinteger(L, iDanger);
	return 1;
}

#endif

LUAAPIIMPL(Unit, IsCivilization)
LUAAPIIMPL(Unit, HasPromotion)
LUAAPIIMPL(Unit, IsUnit)
LUAAPIIMPL(Unit, IsUnitClass)
LUAAPIIMPL(Unit, IsOnFeature)
LUAAPIIMPL(Unit, IsAdjacentToFeature)
LUAAPIIMPL(Unit, IsWithinDistanceOfFeature)
LUAAPIIMPL(Unit, IsWithinDistanceOfUnit)
LUAAPIIMPL(Unit, IsWithinDistanceOfUnitClass)
LUAAPIIMPL(Unit, IsWithinDistanceOfUnitCombatType)
LUAAPIIMPL(Unit, IsWithinDistanceOfUnitPromotion)
LUAAPIIMPL(Unit, IsAdjacentToUnit)
LUAAPIIMPL(Unit, IsAdjacentToUnitClass)
LUAAPIIMPL(Unit, IsAdjacentToUnitCombatType)
LUAAPIIMPL(Unit, IsAdjacentToUnitPromotion)
LUAAPIIMPL(Unit, IsWithinDistanceOfCity)
LUAAPIIMPL(Unit, IsOnImprovement)
LUAAPIIMPL(Unit, IsAdjacentToImprovement)
LUAAPIIMPL(Unit, IsWithinDistanceOfImprovement)
LUAAPIIMPL(Unit, IsOnPlotType)
LUAAPIIMPL(Unit, IsAdjacentToPlotType)
LUAAPIIMPL(Unit, IsWithinDistanceOfPlotType)
LUAAPIIMPL(Unit, IsOnResource)
LUAAPIIMPL(Unit, IsAdjacentToResource)
LUAAPIIMPL(Unit, IsWithinDistanceOfResource)
LUAAPIIMPL(Unit, IsOnTerrain)
LUAAPIIMPL(Unit, IsAdjacentToTerrain)
LUAAPIIMPL(Unit, IsWithinDistanceOfTerrain)
