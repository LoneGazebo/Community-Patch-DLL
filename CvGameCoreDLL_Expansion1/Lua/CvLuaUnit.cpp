/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvLuaSupport.h"
#include "CvLuaArea.h"
#include "CvLuaCity.h"
#include "CvLuaPlot.h"
#include "CvLuaUnit.h"
#include "../CvMinorCivAI.h"
#include "../CvUnitCombat.h"

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
	Method(Kill);

	Method(IsActionRecommended);
	Method(IsBetterDefenderThan);

	Method(CanDoCommand);
	Method(DoCommand);

	Method(GetPathEndTurnPlot);
	Method(GeneratePath);

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
	Method(Embark);

	Method(IsRangeAttackIgnoreLOS);

	Method(CanAirlift);
	Method(CanAirliftAt);

	Method(IsNukeVictim);
	Method(CanNuke);
	Method(CanNukeAt);

	Method(CanRangeStrike);
	Method(CanRangeStrikeAt);

	Method(CanParadrop);
	Method(CanParadropAt);

	Method(CanPillage);

	Method(IsSelected);

	Method(CanFound);
	Method(CanJoin);
	Method(CanConstruct);

	Method(CanDiscover);
	Method(GetTradeGold);
	Method(CanTrade);
	Method(CanRepairFleet);
	Method(CanBuildSpaceship);

	Method(CanGoldenAge);
	Method(GetGoldenAgeTurns);
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
	Method(GetNumResourceNeededToUpgrade);

	Method(GetHandicapType);
	Method(GetCivilizationType);
	Method(GetSpecialUnitType);
	Method(GetCaptureUnitType);
	Method(GetUnitCombatType);
	Method(GetUnitAIType);
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
	Method(Range);
	Method(NukeDamageLevel);

	Method(CanBuildRoute);
	Method(GetBuildType);
	Method(WorkRate);

	Method(IsNoBadGoodies);
	Method(IsOnlyDefensive);

	Method(IsNoCapture);
	Method(IsRivalTerritory);
	Method(IsFound);
	Method(IsWork);
	Method(IsGoldenAge);
	Method(CanCoexistWithEnemyUnit);

	Method(IsGreatPerson);

	Method(IsFighting);
	Method(IsAttacking);
	Method(IsDefending);
	Method(IsInCombat);

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

	Method(GetBaseRangedCombatStrength);
	Method(GetMaxRangedCombatStrength);
	Method(GetCombatLimit);
	Method(GetRangedCombatLimit);
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
	Method(AttackXPValue);
	Method(DefenseXPValue);
	Method(MaxXPValue);
	Method(FirstStrikes);
	Method(ChanceFirstStrikes);
	Method(MaxFirstStrikes);
	Method(IsRanged);
	Method(IsMustSetUpToRangedAttack);
	Method(CanSetUpForRangedAttack);
	Method(IsSetUpForRangedAttack);
	Method(ImmuneToFirstStrikes);
	Method(NoDefensiveBonus);
	Method(IgnoreBuildingDefense);
	Method(CanMoveImpassable);
	Method(CanMoveAllTerrain);
	Method(FlatMovementCost);
	Method(IgnoreTerrainCost);
	Method(IsNeverInvisible);
	Method(IsInvisible);
	Method(IsNukeImmune);
	Method(IsRangeAttackOnlyInDomain);
	Method(IsCityAttackOnly);

	Method(MaxInterceptionProbability);
	Method(CurrInterceptionProbability);
	Method(EvasionProbability);
	Method(WithdrawalProbability);

	Method(GetAdjacentModifier);
	Method(GetAttackModifier);
	Method(GetDefenseModifier);
	Method(GetRangedAttackModifier);
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
	Method(FlankAttackModifier);
	Method(RoughDefenseModifier);
	Method(TerrainAttackModifier);
	Method(TerrainDefenseModifier);
	Method(FeatureAttackModifier);
	Method(FeatureDefenseModifier);
	Method(UnitClassAttackModifier);
	Method(UnitClassDefenseModifier);
	Method(UnitCombatModifier);
	Method(DomainModifier);
	Method(GetStrategicResourceCombatPenalty);
	Method(AirSweepCombatMod);
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
	Method(GetExtraMoves);
	Method(GetExtraMoveDiscount);
	Method(GetExtraRange);
	Method(GetExtraIntercept);
	Method(GetExtraEvasion);
	Method(GetExtraFirstStrikes);
	Method(GetExtraChanceFirstStrikes);
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
	Method(GetKamikazePercent);


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
	Method(IsStackedGreatGeneral);
	Method(IsIgnoreGreatGeneralBenefit);
	Method(GetReverseGreatGeneralModifier);
	Method(GetGreatGeneralCombatModifier);
	Method(GetNearbyImprovementModifier);
	Method(IsFriendlyUnitAdjacent);
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
	Method(IsTerrainDoubleMove);
	Method(IsFeatureDoubleMove);

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

	Method(GetActivityType);
	Method(IsReadyToMove);
	Method(IsBusy);

	Method(GetReligion);
	Method(GetConversionStrength);
	Method(GetSpreadsLeft);
	Method(GetNumFollowersAfterSpread);
	Method(GetMajorityReligionAfterSpread);

	// Helper Functions
	Method(RangeStrike);

	Method(PushMission);
	Method(PopMission);
	Method(LastMissionPlot);
	Method(CanStartMission);

	Method(ExecuteSpecialExploreMove);

	Method(SetDeployFromOperationTurn);
	Method(IsHigherTechThan);
	Method(IsLargerCivThan);
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
//void convert(CyUnit* pUnit);
int CvLuaUnit::lConvert(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkUnitToConvert = GetInstance(L, 2);

	pkUnit->convert(pkUnitToConvert);
	return 0;
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
//CyPlot* getPathEndTurnPlot();
int CvLuaUnit::lGetPathEndTurnPlot(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	CvPlot* pkPlot = pkUnit->GetPathEndTurnPlot();
	CvLuaPlot::Push(L, pkPlot);

	return 1;
}
//------------------------------------------------------------------------------
//bool generatePath(CyPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL);
int CvLuaUnit::lGeneratePath(lua_State* L)
{
	luaL_error(L, "NYI");
	/*CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const int iFlags = 0;
	const bool bReuse = luaL_optint(L, 4, 0);	//defeaults to false
	const bool bResult = pkUnit->generatePath();

	lua_pushboolean(L, bResult);
	return 1;*/
	return 0;
}
//------------------------------------------------------------------------------
//bool canEnterTerritory(int /*TeamTypes*/ eTeam, bool bIgnoreRightOfPassage = false, bool bIsCity = false);
int CvLuaUnit::lCanEnterTerritory(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const TeamTypes eTeam				= (TeamTypes)lua_tointeger(L, 2);
	const bool bIgnoreRightOfPassage	= luaL_optint(L, 3, 0);
	const bool bIsCity					= luaL_optint(L, 4, 0);

	const bool bResult = pkUnit->canEnterTerritory(eTeam, bIgnoreRightOfPassage, bIsCity);

	lua_pushboolean(L, bResult);
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
	const bool bDeclareWar = luaL_optint(L, 3, 0);
	const bool bDestination = luaL_optint(L, 4, 0);

	byte bMoveFlags = 0;
	if(bDeclareWar)
	{
		bMoveFlags |= CvUnit::MOVEFLAG_DECLARE_WAR;
	}
	if(bDestination)
	{
		bMoveFlags |= CvUnit::MOVEFLAG_DESTINATION;
	}

	bool bResult = false;
	if(pkPlot)
	{
		pkUnit->canMoveOrAttackInto(*pkPlot, bMoveFlags);
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
		bResult = pkUnit->canMoveThrough(*pkPlot);
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
	const int iCurrentDamage = lua_tointeger(L, 4);
	const bool bIncludeRand = lua_toboolean(L, 5);
	const bool bAttackerIsCity = lua_toboolean(L, 6);
	const bool bDefenderIsCity = lua_toboolean(L, 7);

	const int iResult = pkUnit->getCombatDamage(iStrength, iOpponentStrength, iCurrentDamage, bIncludeRand, bAttackerIsCity, bDefenderIsCity);
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
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const int iX = lua_tointeger(L, 3);
	const int iY = lua_tointeger(L, 4);
	const bool bResult = pkUnit->canRebaseAt(pkPlot,iX,iY);

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
	const bool bResult = pkUnit->canEmbark(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canDisembark(CvPlot* pPlot)
int CvLuaUnit::lCanDisembark(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canDisembark(pkPlot);

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
	bool bResult = false;
	if(pkOriginPlot && pkTargetPlot)
	{
		bResult = pkUnit->canEmbarkOnto(*pkOriginPlot, *pkTargetPlot);
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

	bool bResult = false;
	if(pkTargetPlot)
	{
		bResult = pkUnit->canDisembarkOnto(*pkTargetPlot);
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
	const bool bResult = pkUnit->canNuke(pkUnit->plot());

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
	const bool bResult = pkUnit->canFound(pkPlot, bTestVisible);

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
	const bool bResult = pkUnit->canJoin(pkPlot, eSpecialist);

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
	const bool bResult = pkUnit->IsCityAttackOnly();
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
int CvLuaUnit::lCanDiscover(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);
	const bool bResult = pkUnit->canDiscover(pkPlot);

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getTradeGold(CyPlot* pPlot);
int CvLuaUnit::lGetTradeGold(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2);

	const int iResult = pkUnit->getTradeGold(pkPlot);
	lua_pushinteger(L, iResult);
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
	const int iResult = pkUnit->GetGoldenAgeTurns();

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
int CvLuaUnit::lGetNumResourceNeededToUpgrade(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const ResourceTypes eResource = (ResourceTypes) lua_tointeger(L, 2);

	const UnitTypes eUpgradeUnitType = pkUnit->GetUpgradeUnitType();

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

	const UnitTypes eResult = pkUnit->getCaptureUnitType(eCivilization);
	lua_pushinteger(L, eResult);
	return 1;
}
//------------------------------------------------------------------------------
//int /*UnitCombatTypes*/ getUnitCombatType();
int CvLuaUnit::lGetUnitCombatType(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const UnitCombatTypes eResult = pkUnit->getUnitCombatType();
	lua_pushinteger(L, eResult);
	return 1;
}
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
	CvPlot* pkPlot = CvLuaPlot::GetInstance(L, 2, false);
	const bool bResult = pkUnit->IsCanDefend(pkPlot);

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
	bool bFromRangedAttack = luaL_optbool(L, 4, false);

	const int iResult = pkUnit->GetMaxDefenseStrength(pInPlot, pkAttacker, bFromRangedAttack);
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

	lua_pushboolean(L, pkUnit->IsEnemyInMovementRange(bOnlyFortified, bOnlyCities));
	return 1;
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
//int airMaxCombatStr(CyUnit* pOther, bool bAttacking);
int CvLuaUnit::lGetMaxRangedCombatStrength(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	CvUnit* pkOther = GetInstance(L, 2, false);
	CvCity* pkCity = CvLuaCity::GetInstance(L, 3, false);
	const bool bAttacking = lua_toboolean(L, 4);
	const bool bForRangedAttack = lua_toboolean(L, 5);

	const int iResult = pkUnit->GetMaxRangedCombatStrength(pkOther, pkCity, bAttacking, bForRangedAttack);
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int combatLimit();
int CvLuaUnit::lGetCombatLimit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetCombatLimit();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int GetRangedCombatLimit();
int CvLuaUnit::lGetRangedCombatLimit(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetRangedCombatLimit();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool canAirAttack();
int CvLuaUnit::lCanAirAttack(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->canAirAttack();

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

	const int iResult = pkUnit->GetRangeCombatDamage(pkDefender, pkCity, bIncludeRand);
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
	{
		pkBestUnit = pkUnit->GetBestInterceptor(*pkPlot, pkDefender, bLandInterceptorsOnly, bVisibleInterceptorsOnly);
	}

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
	{
		iCount = pkUnit->GetInterceptorCount(*pkPlot, pkDefender, bLandInterceptorsOnly, bVisibleInterceptorsOnly);
	}

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
	{
		pkBestUnit = pkUnit->GetBestInterceptor(*pkPlot);
	}

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
	const bool bResult = pkUnit->isFortifyable();

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
//int attackXPValue();
int CvLuaUnit::lAttackXPValue(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->attackXPValue();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int defenseXPValue();
int CvLuaUnit::lDefenseXPValue(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->defenseXPValue();
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
//int firstStrikes();
int CvLuaUnit::lFirstStrikes(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->firstStrikes();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int chanceFirstStrikes();
int CvLuaUnit::lChanceFirstStrikes(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->chanceFirstStrikes();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int maxFirstStrikes();
int CvLuaUnit::lMaxFirstStrikes(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->maxFirstStrikes();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool isRanged();
int CvLuaUnit::lIsRanged(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->isRanged();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaUnit::lIsMustSetUpToRangedAttack(lua_State* L)
{
	return BasicLuaMethod(L, &CvUnit::isMustSetUpToRangedAttack);
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
//bool immuneToFirstStrikes();
int CvLuaUnit::lImmuneToFirstStrikes(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bResult = pkUnit->immuneToFirstStrikes();

	lua_pushboolean(L, bResult);
	return 1;
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
int CvLuaUnit::lMaxInterceptionProbability(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->maxInterceptionProbability();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int currInterceptionProbability();
int CvLuaUnit::lCurrInterceptionProbability(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->currInterceptionProbability();
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
//int GetRangedAttackModifier();
int CvLuaUnit::lGetRangedAttackModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetRangedAttackModifier();
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
//------------------------------------------------------------------------------
//int FlankAttackModifier();
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
int CvLuaUnit::lGetStrategicResourceCombatPenalty(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->GetStrategicResourceCombatPenalty();
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

	CvLuaArea::Push(L, pkUnit->area());
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

	const int iResult = pkUnit->getExperience();
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

	pkUnit->setExperience(iNewValue, iMax);
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

	pkUnit->changeExperience(iChange, iMax, bFromCombat, bInBorders, bUpdateGlobal);
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

	const int iResult = pkUnit->getFortifyTurns();
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
	const bool bResult = pkUnit->isAmphib();

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

	const int iResult = pkUnit->getExtraIntercept();
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
//int getExtraFirstStrikes();
int CvLuaUnit::lGetExtraFirstStrikes(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraFirstStrikes();
	lua_pushinteger(L, iResult);
	return 1;
}
//------------------------------------------------------------------------------
//int getExtraChanceFirstStrikes();
int CvLuaUnit::lGetExtraChanceFirstStrikes(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int iResult = pkUnit->getExtraChanceFirstStrikes();
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
//int getKamikazePercent() const;
int CvLuaUnit::lGetKamikazePercent(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const int iResult = pkUnit->getKamikazePercent();

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
	const bool bResult = pkUnit->isOutOfInterceptions();

	lua_pushboolean(L, bResult);
	return 1;
}
//------------------------------------------------------------------------------
//void setMadeInterception(bool bNewValue);
int CvLuaUnit::lSetMadeInterception(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const bool bNewValue = lua_toboolean(L, 2);

	pkUnit->setMadeInterception(bNewValue);
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
//bool GetNearbyImprovementModifier();
int CvLuaUnit::lGetNearbyImprovementModifier(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);

	const int bResult = pkUnit->GetNearbyImprovementModifier();
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
	UnitHandle pkTransportUnit = pkUnit->getTransportUnit();

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
//------------------------------------------------------------------------------
//string getScriptData() const;
int CvLuaUnit::lGetScriptData(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	const char* szScriptData = pkUnit->getScriptData().c_str();

	lua_pushstring(L, szScriptData);
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
	int iReligiousStrength = pkUnit->GetConversionStrength();

	lua_pushinteger(L, iReligiousStrength);

	return 1;
}
//------------------------------------------------------------------------------
//int GetReligionSpreads();
int CvLuaUnit::lGetSpreadsLeft(lua_State* L)
{
	CvUnit* pkUnit = GetInstance(L);
	int iReligiousStrength = pkUnit->GetReligionData()->GetSpreadsLeft();
	lua_pushinteger(L, iReligiousStrength);

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
	const bool bUseCache = luaL_optint(L, 7, 0);

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