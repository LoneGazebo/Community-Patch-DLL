/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaTeam.cpp
//!  \brief     Private implementation to CvLuaTeam.
//!
//!		This file includes the implementation for a Lua Team instance.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "../CustomMods.h"
#include "CvLuaSupport.h"
#include "CvLuaTeam.h"
#include "CvLuaTeamTech.h"

//Utility macro for registering methods
#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);

//------------------------------------------------------------------------------
void CvLuaTeam::Register(lua_State* L)
{
	FLua::Details::CCallWithErrorHandling(L, pRegister);
}
//------------------------------------------------------------------------------
void CvLuaTeam::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}
//------------------------------------------------------------------------------
void CvLuaTeam::PushMethods(lua_State* L, int t)
{
	Method(IsNone);

	Method(AddTeam);

	Method(CanChangeWarPeace);
	Method(CanDeclareWar);
	Method(DeclareWar);
	Method(MakePeace);
	Method(GetNumTurnsLockedIntoWar);
	Method(Meet);

	Method(GetScore);
	Method(GetPower);
	Method(GetDefensivePower);
	Method(GetNumNukeUnits);

	Method(GetTeamVotingForInDiplo);
	Method(GetProjectedVotesFromMinorAllies);
	Method(GetProjectedVotesFromLiberatedMinors);
	Method(GetProjectedVotesFromCivs);
	Method(GetTotalProjectedVotes);
	Method(GetTotalSecuredVotes); // DEPRECATED, use GetTotalProjectedVotes instead

	Method(GetAtWarCount);
	Method(GetHasMetCivCount);
	Method(HasMetHuman);
	Method(GetDefensivePactCount);

	Method(GetUnitClassMaking);
	Method(GetUnitClassCountPlusMaking);
	Method(GetBuildingClassMaking);
	Method(GetBuildingClassCountPlusMaking);

	Method(CountNumUnitsByArea);
	Method(CountNumCitiesByArea);
	Method(CountTotalPopulationByArea);
	Method(CountEnemyDangerByArea);

	Method(IsHuman);
	Method(IsBarbarian);

#if defined(MOD_API_LUA_EXTENSIONS)
	Method(IsMajorCiv);
#endif
	Method(IsMinorCiv);
	Method(IsMinorCivWarmonger);

	Method(GetLeaderID);
	Method(GetSecretaryID);
	Method(GetHandicapType);
	Method(GetName);
	Method(GetNameKey);

	Method(GetNumMembers);
	Method(IsAlive);
	Method(IsEverAlive);
	Method(GetNumCities);
	Method(GetTotalPopulation);
	Method(GetTotalLand);
	Method(GetNukeInterception);
	Method(ChangeNukeInterception);

	Method(GetForceTeamVoteEligibilityCount);
	Method(IsForceTeamVoteEligible);
	Method(ChangeForceTeamVoteEligibilityCount);
	Method(GetExtraWaterSeeFromCount);
	Method(IsExtraWaterSeeFrom);
	Method(ChangeExtraWaterSeeFromCount);
	Method(GetMapTradingCount);
	Method(IsMapTrading);
	Method(ChangeMapTradingCount);
	Method(GetTechTradingCount);
	Method(IsTechTrading);
	Method(ChangeTechTradingCount);
	Method(GetGoldTradingCount);
	Method(IsGoldTrading);
	Method(ChangeGoldTradingCount);
	Method(GetAllowEmbassyTradingAllowedCount);
	Method(IsAllowEmbassyTradingAllowed);
	Method(ChangeAllowEmbassyTradingAllowedCount);
	Method(GetOpenBordersTradingAllowedCount);
	Method(IsOpenBordersTradingAllowed);
	Method(IsOpenBordersTradingAllowedWithTeam);
	Method(ChangeOpenBordersTradingAllowedCount);
	Method(GetDefensivePactTradingAllowedCount);
	Method(IsDefensivePactTradingAllowed);
	Method(IsDefensivePactTradingAllowedWithTeam);
	Method(ChangeDefensivePactTradingAllowedCount);
	Method(IsResearchAgreementTradingAllowed);
	Method(IsResearchAgreementTradingAllowedWithTeam);
#if defined(MOD_BALANCE_CORE)
	Method(SetOpenBorders);
	Method(SetForcePeace);
	Method(GetNumTurnsAtWar);
	Method(GetNumNaturalWondersDiscovered);
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_TECHS_CITY_WORKING)
	Method(GetCityWorkingChange);
	Method(IsCityWorkingChange);
	Method(ChangeCityWorkingChange);
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
	Method(GetCityAutomatonWorkersChange);
	Method(IsCityAutomatonWorkersChange);
	Method(ChangeCityAutomatonWorkersChange);
#endif
	Method(GetBridgeBuildingCount);
	Method(IsBridgeBuilding);
	Method(ChangeBridgeBuildingCount);
	Method(GetWaterWorkCount);
	Method(IsWaterWork);
	Method(ChangeWaterWorkCount);

	Method(GetBorderObstacleCount);
	Method(IsBorderObstacle);
	Method(ChangeBorderObstacleCount);

	Method(IsMapCentering);
	Method(SetMapCentering);

	Method(GetID);

	Method(GetTechShareCount);
	Method(IsTechShare);
	Method(ChangeTechShareCount);

	Method(GetExtraMoves);
	Method(ChangeExtraMoves);

	Method(CanEmbark);
	Method(CanDefensiveEmbark);

	Method(IsHasMet);
	Method(IsAtWar);
	Method(IsPermanentWarPeace);
	Method(SetPermanentWarPeace);

	Method(GetLiberatedByTeam);
	Method(GetKilledByTeam);

	Method(HasEmbassyAtTeam);
#if defined(MOD_API_LUA_EXTENSIONS)
	Method(HasSpyAtTeam);
#endif
	Method(IsAllowsOpenBordersToTeam);
	Method(IsForcePeace);
	Method(IsDefensivePact);
	Method(GetRouteChange);
	Method(ChangeRouteChange);
	Method(GetProjectCount);
	Method(GetProjectDefaultArtType);
	Method(SetProjectDefaultArtType);
	Method(GetProjectArtType);
	Method(SetProjectArtType);
	Method(IsProjectMaxedOut);
	Method(IsProjectAndArtMaxedOut);
	Method(ChangeProjectCount);
	Method(GetProjectMaking);
	Method(GetUnitClassCount);
	Method(IsUnitClassMaxedOut);
	Method(GetBuildingClassCount);
	Method(IsBuildingClassMaxedOut);
	Method(GetObsoleteBuildingCount);
	Method(IsObsoleteBuilding);

	Method(IsHasResearchAgreement);

	Method(IsHasTech);
	Method(SetHasTech);

	Method(GetTeamTechs);

	Method(GetImprovementYieldChange);
	Method(ChangeImprovementYieldChange);

	Method(IsHomeOfUnitedNations);

	Method(GetVictoryCountdown);
	Method(GetVictoryDelay);
	Method(CanLaunch);

	Method(GetVictoryPoints);
	Method(ChangeVictoryPoints);

	Method(GetCurrentEra);
	Method(SetCurrentEra);

	Method(UpdateEmbarkGraphics);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CIV4_FEATURES)
	Method(IsVassal);
	Method(CanBecomeVassal);
	Method(CanMakeVassal);
	Method(CanEndVassal);
	Method(CanEndAllVassal);
	Method(IsVassalageTradingAllowed);
	Method(GetNumTurnsIsVassal);
	Method(GetNumTurnsSinceVassalEnded);
	Method(IsTooSoonForVassal);
	Method(IsVassalOfSomeone);
	Method(IsVassalLockedIntoWar);
	Method(GetMaster);
	Method(IsVoluntaryVassal);
	Method(DoBecomeVassal);
	Method(DoEndVassal);
	Method(GetNumCitiesWhenVassalMade);
	Method(GetTotalPopulationWhenVassalMade);
	Method(CanLiberateVassal);
	Method(DoLiberateVassal);
	Method(CanSetVassalTax);
	Method(GetVassalTax);
	Method(GetNumTurnsSinceVassalTaxSet);
	Method(DoApplyVassalTax);
	Method(GetNumVassals);
#endif

#if defined(MOD_BALANCE_CORE)
	Method(IsCorporationsEnabled);
#endif
}
//------------------------------------------------------------------------------
const char* CvLuaTeam::GetTypeName()
{
	return "Team";
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Lua Methods
//------------------------------------------------------------------------------
int CvLuaTeam::pRegister(lua_State* L)
{
	lua_getglobal(L, "Teams");
	if(lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "Teams");
	}

	for(int i = 0; i < MAX_TEAMS; ++i)
	{
		CvTeam* pkTeam = &(GET_TEAM((TeamTypes)i));
		CvLuaTeam::Push(L, pkTeam);
		lua_rawseti(L, -2, i);
	}

	return 0;
}


//------------------------------------------------------------------------------
//bool isNone();
int CvLuaTeam::lIsNone(lua_State* L)
{
	const bool bDoesNotExist = (GetInstance(L, false) == NULL);
	lua_pushboolean(L, bDoesNotExist);

	return 1;
}


//------------------------------------------------------------------------------
//void addTeam(TeamTypes eTeam);
int CvLuaTeam::lAddTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::addTeam);
}


//------------------------------------------------------------------------------
//bool canChangeWarPeace(TeamTypes eTeam);
int CvLuaTeam::lCanChangeWarPeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canChangeWarPeace);
}

//------------------------------------------------------------------------------
//bool canDeclareWar(TeamTypes eTeam);
int CvLuaTeam::lCanDeclareWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canDeclareWar);
}

//------------------------------------------------------------------------------
//void declareWar(TeamTypes eTeam);
int CvLuaTeam::lDeclareWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::declareWar);
}

//------------------------------------------------------------------------------
//void makePeace(TeamTypes eTeam);
int CvLuaTeam::lMakePeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::makePeace);
}

//------------------------------------------------------------------------------
//bool GetNumTurnsLockedIntoWar(TeamTypes eTeam);
int CvLuaTeam::lGetNumTurnsLockedIntoWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetNumTurnsLockedIntoWar);
}

//------------------------------------------------------------------------------
//void meet(TeamTypes eTeam, bool bSuppressMessages);
int CvLuaTeam::lMeet(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	TeamTypes eOtherTeam = (TeamTypes)lua_tointeger(L, 2);
	const bool bSuppressMessages = lua_toboolean(L, 3);

	pkTeam->meet(eOtherTeam, bSuppressMessages);
	return 0;
}
//------------------------------------------------------------------------------
//int getScore();
int CvLuaTeam::lGetScore(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetScore);
}

//------------------------------------------------------------------------------
//int getPower();
int CvLuaTeam::lGetPower(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getPower);
}

//------------------------------------------------------------------------------
//int getDefensivePower();
int CvLuaTeam::lGetDefensivePower(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getDefensivePower);
}

//------------------------------------------------------------------------------
//int getNumNukeUnits();
int CvLuaTeam::lGetNumNukeUnits(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getNumNukeUnits);
}

//------------------------------------------------------------------------------
//int GetTeamVotingForInDiplo();
int CvLuaTeam::lGetTeamVotingForInDiplo(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetTeamVotingForInDiplo);
}

//------------------------------------------------------------------------------
//int GetProjectedVotesFromMinorAllies();
int CvLuaTeam::lGetProjectedVotesFromMinorAllies(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetProjectedVotesFromMinorAllies);
}

//------------------------------------------------------------------------------
//int GetProjectedVotesFromLiberatedMinors();
int CvLuaTeam::lGetProjectedVotesFromLiberatedMinors(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetProjectedVotesFromLiberatedMinors);
}

//------------------------------------------------------------------------------
//int GetProjectedVotesFromCivs();
int CvLuaTeam::lGetProjectedVotesFromCivs(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetProjectedVotesFromCivs);
}

//------------------------------------------------------------------------------
//int GetTotalProjectedVotes();
int CvLuaTeam::lGetTotalProjectedVotes(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetTotalProjectedVotes);
}

//------------------------------------------------------------------------------
//int GetTotalSecuredVotes();
int CvLuaTeam::lGetTotalSecuredVotes(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetTotalSecuredVotes);
}

//------------------------------------------------------------------------------
//int getAtWarCount(bool bIgnoreMinors);
int CvLuaTeam::lGetAtWarCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getAtWarCount);
}

//------------------------------------------------------------------------------
//int getHasMetCivCount(bool bIgnoreMinors);
int CvLuaTeam::lGetHasMetCivCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getHasMetCivCount);
}

//------------------------------------------------------------------------------
//bool hasMetHuman();
int CvLuaTeam::lHasMetHuman(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::hasMetHuman);
}

//------------------------------------------------------------------------------
//int getDefensivePactCount();
int CvLuaTeam::lGetDefensivePactCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getDefensivePactCount);
}

//------------------------------------------------------------------------------
//int getUnitClassMaking(UnitClassTypes eUnitClass);
int CvLuaTeam::lGetUnitClassMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getUnitClassMaking);
}

//------------------------------------------------------------------------------
//int getUnitClassCountPlusMaking(UnitClassTypes eUnitClass);
int CvLuaTeam::lGetUnitClassCountPlusMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getUnitClassCountPlusMaking);
}

//------------------------------------------------------------------------------
//int getBuildingClassMaking(BuildingClassTypes eBuildingClass);
int CvLuaTeam::lGetBuildingClassMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBuildingClassMaking);
}

//------------------------------------------------------------------------------
//int getBuildingClassCountPlusMaking(BuildingClassTypes eUnitClass);
int CvLuaTeam::lGetBuildingClassCountPlusMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBuildingClassCountPlusMaking);
}


//------------------------------------------------------------------------------
//int countNumUnitsByArea(CvArea* pArea);
int CvLuaTeam::lCountNumUnitsByArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::countNumUnitsByArea);
}

//------------------------------------------------------------------------------
//int countNumCitiesByArea(CvArea* pArea);
int CvLuaTeam::lCountNumCitiesByArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::countNumCitiesByArea);
}

//------------------------------------------------------------------------------
//int countTotalPopulationByArea(CvArea* pArea);
int CvLuaTeam::lCountTotalPopulationByArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::countTotalPopulationByArea);
}

//------------------------------------------------------------------------------
//int countEnemyDangerByArea(CvArea* pArea);
int CvLuaTeam::lCountEnemyDangerByArea(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::countEnemyDangerByArea);
}

//------------------------------------------------------------------------------
//bool isHuman();
int CvLuaTeam::lIsHuman(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isHuman);
}

//------------------------------------------------------------------------------
//bool isBarbarian();
int CvLuaTeam::lIsBarbarian(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isBarbarian);
}

#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
//bool isMajorCiv();
int CvLuaTeam::lIsMajorCiv(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isMajorCiv);
}
#endif

//------------------------------------------------------------------------------
//bool isMinorCiv();
int CvLuaTeam::lIsMinorCiv(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isMinorCiv);
}

//------------------------------------------------------------------------------
//bool IsMinorCivWarmonger();
int CvLuaTeam::lIsMinorCivWarmonger(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsMinorCivWarmonger);
}

//------------------------------------------------------------------------------
//PlayerTypes getLeaderID();
int CvLuaTeam::lGetLeaderID(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getLeaderID);
}

//------------------------------------------------------------------------------
//PlayerTypes getSecretaryID();
int CvLuaTeam::lGetSecretaryID(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getSecretaryID);
}

//------------------------------------------------------------------------------
//HandicapTypes getHandicapType();
int CvLuaTeam::lGetHandicapType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getHandicapType);
}

//------------------------------------------------------------------------------
//string getName();
int CvLuaTeam::lGetName(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);

	lua_pushstring(L, pkTeam->getName());
	return 1;
}

//------------------------------------------------------------------------------
// string getNameKey();
int CvLuaTeam::lGetNameKey(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);

	lua_pushstring(L, pkTeam->getNameKey());
	return 1;
}
//------------------------------------------------------------------------------
//int getNumMembers();
int CvLuaTeam::lGetNumMembers(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getNumMembers);
}

//------------------------------------------------------------------------------
//bool isAlive();
int CvLuaTeam::lIsAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isAlive);
}

//------------------------------------------------------------------------------
//bool isEverAlive();
int CvLuaTeam::lIsEverAlive(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isEverAlive);
}

//------------------------------------------------------------------------------
//int getNumCities();
int CvLuaTeam::lGetNumCities(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getNumCities);
}

//------------------------------------------------------------------------------
//int getTotalPopulation();
int CvLuaTeam::lGetTotalPopulation(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getTotalPopulation);
}

//------------------------------------------------------------------------------
//int getTotalLand();
int CvLuaTeam::lGetTotalLand(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getTotalLand);
}

//------------------------------------------------------------------------------
//int getNukeInterception();
int CvLuaTeam::lGetNukeInterception(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getNukeInterception);
}

//------------------------------------------------------------------------------
//void changeNukeInterception(int iChange);
int CvLuaTeam::lChangeNukeInterception(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeNukeInterception);
}


//------------------------------------------------------------------------------
//int getForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource);
int CvLuaTeam::lGetForceTeamVoteEligibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getForceTeamVoteEligibilityCount);
}

//------------------------------------------------------------------------------
//bool isForceTeamVoteEligible(VoteSourceTypes eVoteSource);
int CvLuaTeam::lIsForceTeamVoteEligible(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isForceTeamVoteEligible);
}

//------------------------------------------------------------------------------
//void changeForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource, int iChange);
int CvLuaTeam::lChangeForceTeamVoteEligibilityCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeForceTeamVoteEligibilityCount);
}

//------------------------------------------------------------------------------
//int getExtraWaterSeeFromCount();
int CvLuaTeam::lGetExtraWaterSeeFromCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getExtraWaterSeeFromCount);
}

//------------------------------------------------------------------------------
//bool isExtraWaterSeeFrom();
int CvLuaTeam::lIsExtraWaterSeeFrom(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isExtraWaterSeeFrom);
}

//------------------------------------------------------------------------------
//void changeExtraWaterSeeFromCount(int iChange);
int CvLuaTeam::lChangeExtraWaterSeeFromCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeExtraWaterSeeFromCount);
}

//------------------------------------------------------------------------------
//int getMapTradingCount();
int CvLuaTeam::lGetMapTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getMapTradingCount);
}

//------------------------------------------------------------------------------
//bool isMapTrading();
int CvLuaTeam::lIsMapTrading(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isMapTrading);
}

//------------------------------------------------------------------------------
//void changeMapTradingCount(int iChange);
int CvLuaTeam::lChangeMapTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeMapTradingCount);
}

//------------------------------------------------------------------------------
//int getTechTradingCount();
int CvLuaTeam::lGetTechTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getTechTradingCount);
}

//------------------------------------------------------------------------------
//bool isTechTrading();
int CvLuaTeam::lIsTechTrading(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isTechTrading);
}

//------------------------------------------------------------------------------
//void changeTechTradingCount(int iChange);
int CvLuaTeam::lChangeTechTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeTechTradingCount);
}

//------------------------------------------------------------------------------
//int getGoldTradingCount();
int CvLuaTeam::lGetGoldTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getGoldTradingCount);
}

//------------------------------------------------------------------------------
//bool isGoldTrading();
int CvLuaTeam::lIsGoldTrading(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isGoldTrading);
}

//------------------------------------------------------------------------------
//void changeGoldTradingCount(int iChange);
int CvLuaTeam::lChangeGoldTradingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeGoldTradingCount);
}

//------------------------------------------------------------------------------
int CvLuaTeam::lGetAllowEmbassyTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getAllowEmbassyTradingAllowedCount);
}

//------------------------------------------------------------------------------
int CvLuaTeam::lIsAllowEmbassyTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isAllowEmbassyTradingAllowed);
}

//------------------------------------------------------------------------------
int CvLuaTeam::lChangeAllowEmbassyTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeAllowEmbassyTradingAllowedCount);
}

//------------------------------------------------------------------------------
//int getOpenBordersTradingAllowedCount();
int CvLuaTeam::lGetOpenBordersTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getOpenBordersTradingAllowedCount);
}

//------------------------------------------------------------------------------
//bool isOpenBordersTradingAllowed();
int CvLuaTeam::lIsOpenBordersTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isOpenBordersTradingAllowed);
}

//------------------------------------------------------------------------------
//bool isOpenBordersTradingAllowedWithTeam(TeamTypes eTeam)
int CvLuaTeam::lIsOpenBordersTradingAllowedWithTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isOpenBordersTradingAllowedWithTeam);
}

//------------------------------------------------------------------------------
//void changeOpenBordersTradingAllowedCount(int iChange);
int CvLuaTeam::lChangeOpenBordersTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeOpenBordersTradingAllowedCount);
}

//------------------------------------------------------------------------------
//int getDefensivePactTradingAllowedCount();
int CvLuaTeam::lGetDefensivePactTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getDefensivePactTradingAllowedCount);
}

//------------------------------------------------------------------------------
//bool isDefensivePactTradingAllowed();
int CvLuaTeam::lIsDefensivePactTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isDefensivePactTradingAllowed);
}

//------------------------------------------------------------------------------
//bool isDefensivePactTradingAllowedWithTeam(TeamTypes eTeam)
int CvLuaTeam::lIsDefensivePactTradingAllowedWithTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isDefensivePactTradingAllowedWithTeam);
}

//------------------------------------------------------------------------------
//void changeDefensivePactTradingAllowedCount(int iChange);
int CvLuaTeam::lChangeDefensivePactTradingAllowedCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeDefensivePactTradingAllowedCount);
}

//------------------------------------------------------------------------------
//bool IsResearchAgreementTradingAllowed();
int CvLuaTeam::lIsResearchAgreementTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsResearchAgreementTradingAllowed);
}

//------------------------------------------------------------------------------
//bool IsResearchAgreementTradingAllowedWithTeam(TeamTypes eTeam)
int CvLuaTeam::lIsResearchAgreementTradingAllowedWithTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsResearchAgreementTradingAllowedWithTeam);
}

#if defined(MOD_BALANCE_CORE)
//void SetAllowsOpenBordersToTeam(eOtherTeam, bool bValue)
int CvLuaTeam::lSetOpenBorders(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes) lua_tointeger(L, 2);
	const bool bValue = luaL_optbool(L, 3, false);

	pkTeam->SetAllowsOpenBordersToTeam(eTeam, bValue);
	return 0;
}
//void CvTeam::setForcePeace(TeamTypes eIndex, bool bNewValue)
int CvLuaTeam::lSetForcePeace(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes) lua_tointeger(L, 2);
	const bool bValue = luaL_optbool(L, 3, false);

	pkTeam->setForcePeace(eTeam, bValue);
	return 0;
}

int CvLuaTeam::lGetNumTurnsAtWar(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes) lua_tointeger(L, 2);
	int iValue = 0;
	if(eTeam != NO_TEAM)
	{
		iValue =  pkTeam->GetNumTurnsAtWar(eTeam);
		
	}
	lua_pushnumber(L, iValue);
	return 1;
}
int CvLuaTeam::lGetNumNaturalWondersDiscovered(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);

	const int iValue = pkTeam->GetNumNaturalWondersDiscovered();

	lua_pushnumber(L, iValue);
	return 1;
}
#endif
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_TECHS_CITY_WORKING)
//------------------------------------------------------------------------------
//int getCityWorkingChange();
int CvLuaTeam::lGetCityWorkingChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetCityWorkingChange);
}

//------------------------------------------------------------------------------
//bool isCityWorkingChange();
int CvLuaTeam::lIsCityWorkingChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isCityWorkingChange);
}

//------------------------------------------------------------------------------
//void changeCityWorkingChange(int iChange);
int CvLuaTeam::lChangeCityWorkingChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeCityWorkingChange);
}
#endif

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_TECHS_CITY_AUTOMATON_WORKERS)
//------------------------------------------------------------------------------
//int getCityAutomatonWorkersChange();
int CvLuaTeam::lGetCityAutomatonWorkersChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetCityAutomatonWorkersChange);
}

//------------------------------------------------------------------------------
//bool isCityAutomatonWorkersChange();
int CvLuaTeam::lIsCityAutomatonWorkersChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isCityAutomatonWorkersChange);
}

//------------------------------------------------------------------------------
//void changeCityAutomatonWorkersChange(int iChange);
int CvLuaTeam::lChangeCityAutomatonWorkersChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeCityAutomatonWorkersChange);
}
#endif

//------------------------------------------------------------------------------
//int getBridgeBuildingCount();
int CvLuaTeam::lGetBridgeBuildingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBridgeBuildingCount);
}

//------------------------------------------------------------------------------
//bool isBridgeBuilding();
int CvLuaTeam::lIsBridgeBuilding(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isBridgeBuilding);
}

//------------------------------------------------------------------------------
//void changeBridgeBuildingCount(int iChange);
int CvLuaTeam::lChangeBridgeBuildingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeBridgeBuildingCount);
}

//------------------------------------------------------------------------------
//int getWaterWorkCount();
int CvLuaTeam::lGetWaterWorkCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getWaterWorkCount);
}

//------------------------------------------------------------------------------
//bool isWaterWork();
int CvLuaTeam::lIsWaterWork(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isWaterWork);
}

//------------------------------------------------------------------------------
//void changeWaterWorkCount(int iChange);
int CvLuaTeam::lChangeWaterWorkCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeWaterWorkCount);
}

//------------------------------------------------------------------------------
//int getBorderObstacleCount();
int CvLuaTeam::lGetBorderObstacleCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBorderObstacleCount);
}

//------------------------------------------------------------------------------
//bool isBorderObstacle();
int CvLuaTeam::lIsBorderObstacle(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isBorderObstacle);
}

//------------------------------------------------------------------------------
//void changeBorderObstacleCount(int iChange);
int CvLuaTeam::lChangeBorderObstacleCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeBorderObstacleCount);
}


//------------------------------------------------------------------------------
//bool isMapCentering();
int CvLuaTeam::lIsMapCentering(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isMapCentering);
}

//------------------------------------------------------------------------------
//void setMapCentering(bool bNewValue);
int CvLuaTeam::lSetMapCentering(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::setMapCentering);
}


//------------------------------------------------------------------------------
//int getID();
int CvLuaTeam::lGetID(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetID);
}

//------------------------------------------------------------------------------
//int getTechShareCount(int iIndex);
int CvLuaTeam::lGetTechShareCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getTechShareCount);
}

//------------------------------------------------------------------------------
//bool isTechShare(int iIndex);
int CvLuaTeam::lIsTechShare(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isTechShare);
}

//------------------------------------------------------------------------------
//void changeTechShareCount(int iIndex, int iChange);
int CvLuaTeam::lChangeTechShareCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeTechShareCount);
}


//------------------------------------------------------------------------------
//int getExtraMoves(DomainTypes eIndex);
int CvLuaTeam::lGetExtraMoves(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getExtraMoves);
}

//------------------------------------------------------------------------------
//void changeExtraMoves(DomainTypes eIndex, int iChange);
int CvLuaTeam::lChangeExtraMoves(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeExtraMoves);
}

//------------------------------------------------------------------------------
int CvLuaTeam::lCanEmbark(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canEmbark);
}

//------------------------------------------------------------------------------
int CvLuaTeam::lCanDefensiveEmbark(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canDefensiveEmbark);
}

//------------------------------------------------------------------------------
//bool isHasMet(TeamTypes eIndex);
int CvLuaTeam::lIsHasMet(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isHasMet);
}

//------------------------------------------------------------------------------
//bool isAtWar(TeamTypes eIndex);
int CvLuaTeam::lIsAtWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isAtWar);
}

//------------------------------------------------------------------------------
//bool isPermanentWarPeace(TeamTypes eIndex);
int CvLuaTeam::lIsPermanentWarPeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isPermanentWarPeace);
}

//------------------------------------------------------------------------------
//void setPermanentWarPeace(TeamTypes eIndex, bool bNewValue);
int CvLuaTeam::lSetPermanentWarPeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::setPermanentWarPeace);
}
//------------------------------------------------------------------------------
//TeamTypes GetLiberatedByTeam();
int CvLuaTeam::lGetLiberatedByTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetLiberatedByTeam);
}
//------------------------------------------------------------------------------
//TeamTypes GetKilledByTeam();
int CvLuaTeam::lGetKilledByTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetKilledByTeam);
}
//------------------------------------------------------------------------------
int CvLuaTeam::lHasEmbassyAtTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::HasEmbassyAtTeam);
}
#if defined(MOD_API_LUA_EXTENSIONS)
//------------------------------------------------------------------------------
int CvLuaTeam::lHasSpyAtTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::HasSpyAtTeam);
}
#endif
//------------------------------------------------------------------------------
//bool isAllowsOpenBordersToTeam(TeamTypes eIndex);
int CvLuaTeam::lIsAllowsOpenBordersToTeam(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsAllowsOpenBordersToTeam);
}

//------------------------------------------------------------------------------
//bool isForcePeace(TeamTypes eIndex);
int CvLuaTeam::lIsForcePeace(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isForcePeace);
}

//------------------------------------------------------------------------------
//bool isDefensivePact(TeamTypes eIndex);
int CvLuaTeam::lIsDefensivePact(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsHasDefensivePact);
}

//------------------------------------------------------------------------------
//int getRouteChange(RouteTypes eIndex);
int CvLuaTeam::lGetRouteChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getRouteChange);
}

//------------------------------------------------------------------------------
//void changeRouteChange(RouteTypes eIndex, int iChange);
int CvLuaTeam::lChangeRouteChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeRouteChange);
}

//------------------------------------------------------------------------------
//int getProjectCount(ProjectTypes eIndex);
int CvLuaTeam::lGetProjectCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getProjectCount);
}

//------------------------------------------------------------------------------
//int getProjectDefaultArtType(ProjectTypes eIndex);
int CvLuaTeam::lGetProjectDefaultArtType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getProjectDefaultArtType);
}

//------------------------------------------------------------------------------
//void setProjectDefaultArtType(ProjectTypes eIndex, int value);
int CvLuaTeam::lSetProjectDefaultArtType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::setProjectDefaultArtType);
}

//------------------------------------------------------------------------------
//int getProjectArtType(ProjectTypes eIndex, int number);
int CvLuaTeam::lGetProjectArtType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getProjectArtType);
}

//------------------------------------------------------------------------------
//void setProjectArtType(ProjectTypes eIndex, int number, int value);
int CvLuaTeam::lSetProjectArtType(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::setProjectArtType);
}

//------------------------------------------------------------------------------
//bool isProjectMaxedOut(ProjectTypes eIndex, int iExtra);
int CvLuaTeam::lIsProjectMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isProjectMaxedOut);
}

//------------------------------------------------------------------------------
//bool isProjectAndArtMaxedOut(ProjectTypes eIndex);
int CvLuaTeam::lIsProjectAndArtMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isProjectAndArtMaxedOut);
}

//------------------------------------------------------------------------------
//void changeProjectCount(ProjectTypes eIndex, int iChange);
int CvLuaTeam::lChangeProjectCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeProjectCount);
}

//------------------------------------------------------------------------------
//int getProjectMaking(ProjectTypes eIndex);
int CvLuaTeam::lGetProjectMaking(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getProjectMaking);
}

//------------------------------------------------------------------------------
//int getUnitClassCount(UnitClassTypes eIndex);
int CvLuaTeam::lGetUnitClassCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getUnitClassCount);
}

//------------------------------------------------------------------------------
//bool isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra);
int CvLuaTeam::lIsUnitClassMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isUnitClassMaxedOut);
}

//------------------------------------------------------------------------------
//int getBuildingClassCount(BuildingClassTypes eIndex);
int CvLuaTeam::lGetBuildingClassCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getBuildingClassCount);
}

//------------------------------------------------------------------------------
//bool isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra);
int CvLuaTeam::lIsBuildingClassMaxedOut(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isBuildingClassMaxedOut);
}

//------------------------------------------------------------------------------
//int getObsoleteBuildingCount(BuildingTypes eIndex);
int CvLuaTeam::lGetObsoleteBuildingCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getObsoleteBuildingCount);
}

//------------------------------------------------------------------------------
//bool isObsoleteBuilding(BuildingTypes eIndex);
int CvLuaTeam::lIsObsoleteBuilding(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::isObsoleteBuilding);
}

//------------------------------------------------------------------------------
//bool IsHasResearchAgreement(TeamTypes eTeam);
int CvLuaTeam::lIsHasResearchAgreement(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsHasResearchAgreement);
}

//------------------------------------------------------------------------------
//bool IsHasTech(TechTypes eIndex);
int CvLuaTeam::lIsHasTech(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	const TechTypes eTech = (TechTypes)lua_tointeger(L, 2);

	lua_pushboolean(L, pkTeam->GetTeamTechs()->HasTech(eTech));
	return 1;
}

//------------------------------------------------------------------------------
//void setHasTech(TechTypes eIndex, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce);
int CvLuaTeam::lSetHasTech(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	const TechTypes eIndex = (TechTypes)lua_tointeger(L, 2);
	const bool bNewValue = lua_toboolean(L, 3);
	const PlayerTypes ePlayer = (PlayerTypes)lua_tointeger(L, 4);

	bool bFirst = lua_toboolean(L, 4);
	bool bAnnounce = lua_toboolean(L, 5);

	if (lua_gettop(L) == 6)
	{
		bFirst = lua_toboolean(L, 5);
		bAnnounce = lua_toboolean(L, 6);
	}

	const bool bNoBonus = luaL_optbool(L, 7, false);
	pkTeam->setHasTech(eIndex, bNewValue, ePlayer, bFirst, bAnnounce, bNoBonus);

	return 0;
}

//------------------------------------------------------------------------------
//TeamTechs GetTeamTechs()
int CvLuaTeam::lGetTeamTechs(lua_State* L)
{
	CvTeam* pkTeam = GetInstance(L);
	CvTeamTechs* pkTeamTechs = pkTeam->GetTeamTechs();

	CvLuaTeamTech::Push(L, pkTeamTechs);
	return 1;
}

//------------------------------------------------------------------------------
//int getImprovementYieldChange(ImprovementTypes eIndex, YieldTypes eIndex2);
int CvLuaTeam::lGetImprovementYieldChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getImprovementYieldChange);
}

//------------------------------------------------------------------------------
//void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);
int CvLuaTeam::lChangeImprovementYieldChange(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeImprovementYieldChange);
}


//------------------------------------------------------------------------------
//int IsHomeOfUnitedNations();
int CvLuaTeam::lIsHomeOfUnitedNations(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsHomeOfUnitedNations);
}

//------------------------------------------------------------------------------
//int getVictoryCountdown(VictoryTypes eVictory);
int CvLuaTeam::lGetVictoryCountdown(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getVictoryCountdown);
}

//------------------------------------------------------------------------------
//int getVictoryDelay(VictoryTypes eVictory);
int CvLuaTeam::lGetVictoryDelay(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getVictoryDelay);
}

//------------------------------------------------------------------------------
//bool canLaunch(VictoryTypes eVictory);
int CvLuaTeam::lCanLaunch(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canLaunch);
}


//------------------------------------------------------------------------------
//int getVictoryPoints();
int CvLuaTeam::lGetVictoryPoints(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::getVictoryPoints);
}

//------------------------------------------------------------------------------
//void changeVictoryPoints(int iChange);
int CvLuaTeam::lChangeVictoryPoints(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::changeVictoryPoints);
}

//------------------------------------------------------------------------------
//ErasTypes  GetCurrentEra();
int CvLuaTeam::lGetCurrentEra(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetCurrentEra);
}
//------------------------------------------------------------------------------
//void SetCurrentEra(EraTypes  iNewValue);
int CvLuaTeam::lSetCurrentEra(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::SetCurrentEra);
}
//------------------------------------------------------------------------------
//void UpdateEmbarkGraphics();
int CvLuaTeam::lUpdateEmbarkGraphics(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::UpdateEmbarkGraphics);
}

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CIV4_FEATURES)
//------------------------------------------------------------------------------
//int GetNumTurnsIsVassal(TeamTypes eIndex) const;
int CvLuaTeam::lGetNumTurnsIsVassal(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetNumTurnsIsVassal);
}
//------------------------------------------------------------------------------
//int GetNumTurnsSinceVassalEnded(TeamTypes eIndex) const;
int CvLuaTeam::lGetNumTurnsSinceVassalEnded(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetNumTurnsSinceVassalEnded);
}
//------------------------------------------------------------------------------
//bool IsTooSoonForVassal(TeamTypes eIndex) const;
int CvLuaTeam::lIsTooSoonForVassal(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsTooSoonForVassal);
}
//------------------------------------------------------------------------------
//bool IsVassal(TeamTypes eIndex) const;
int CvLuaTeam::lIsVassal(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsVassal);
}
//------------------------------------------------------------------------------
// bool CanBecomeVassal(TeamTypes eIndex, bool bIgnoreAlreadyVassal) const;
int CvLuaTeam::lCanBecomeVassal(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canBecomeVassal);
}
//------------------------------------------------------------------------------
// bool CanMakeVassal(TeamTypes eIndex, bool bIgnoreAlreadyVassal) const;
int CvLuaTeam::lCanMakeVassal(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::CanMakeVassal);
}
//------------------------------------------------------------------------------
// bool canEndVassal(TeamTypes eIndex) const;
int CvLuaTeam::lCanEndVassal(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canEndVassal);
}
// bool canEndVassal(TeamTypes eIndex) const;
int CvLuaTeam::lCanEndAllVassal(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::canEndAllVassal);
}
//------------------------------------------------------------------------------
// bool IsVassalageTradingAllowed() const;
int CvLuaTeam::lIsVassalageTradingAllowed(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsVassalageTradingAllowed);
}
//------------------------------------------------------------------------------
// bool IsVassalOfSomeone() const;
int CvLuaTeam::lIsVassalOfSomeone(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsVassalOfSomeone);
}
//------------------------------------------------------------------------------
// bool IsVassalLockedIntoWar() const;
int CvLuaTeam::lIsVassalLockedIntoWar(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsVassalLockedIntoWar);
}
//------------------------------------------------------------------------------
// TeamTypes GetMaster() const;
int CvLuaTeam::lGetMaster(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::GetMaster);
}
//------------------------------------------------------------------------------
// bool IsVoluntaryVassal(TeamTypes eTeam) const;
int CvLuaTeam::lIsVoluntaryVassal(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeam::IsVoluntaryVassal);
}
// ---------------------------------------------------------------------
// void DoBecomeVassal(TeamTypes eTeam, bool bVoluntary)
int CvLuaTeam::lDoBecomeVassal(lua_State *L)
{
	CvTeam* pkTeam = GetInstance(L);
	const TeamTypes eTeam = (TeamTypes) lua_tointeger(L, 2);
	const bool bVoluntary = luaL_optbool(L, 3, false);

	pkTeam->DoBecomeVassal(eTeam, bVoluntary);
	return 0;
}
// ---------------------------------------------------------------------
// void DoEndVassal(TeamTypes eTeam, bool bPeaceful, bool bSuppressNotification)
int CvLuaTeam::lDoEndVassal(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::DoEndVassal);
}
// ---------------------------------------------------------------------
// int GetNumCitiesWhenVassalMade()
int CvLuaTeam::lGetNumCitiesWhenVassalMade(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::getNumCitiesWhenVassalMade);
}
// ---------------------------------------------------------------------
// int GetTotalPopulationWhenVassalMade()
int CvLuaTeam::lGetTotalPopulationWhenVassalMade(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::getTotalPopulationWhenVassalMade);
}
// ---------------------------------------------------------------------
// bool CanLiberateVassal()
int CvLuaTeam::lCanLiberateVassal(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::CanLiberateVassal);
}
// ---------------------------------------------------------------------
// void DoLiberateVassal()
int CvLuaTeam::lDoLiberateVassal(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::DoLiberateVassal);
}
// ---------------------------------------------------------------------
// bool CanSetVassalTax(PlayerTypes ePlayer)
int CvLuaTeam::lCanSetVassalTax(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::CanSetVassalTax);
}
// ---------------------------------------------------------------------
// int GetVassalTax(PlayerTypes ePlayer)
int CvLuaTeam::lGetVassalTax(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetVassalTax);
}
// ---------------------------------------------------------------------
// void GetNumTurnsSinceVassalTaxSet(PlayerTypes ePlayer)
int CvLuaTeam::lGetNumTurnsSinceVassalTaxSet(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetNumTurnsSinceVassalTaxSet);
}
// ---------------------------------------------------------------------
// void DoApplyVassalTax(PlayerTypes ePlayer, int iAmount)
int CvLuaTeam::lDoApplyVassalTax(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::DoApplyVassalTax);
}
// ---------------------------------------------------------------------
// int GetNumVassals()
int CvLuaTeam::lGetNumVassals(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::GetNumVassals);
}
#endif

#if defined(MOD_BALANCE_CORE)
int CvLuaTeam::lIsCorporationsEnabled(lua_State *L)
{
	return BasicLuaMethod(L, &CvTeam::IsCorporationsEnabled);
}
#endif
