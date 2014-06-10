/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaTeam.h
//!  \brief     Public interface to CvLuaTeam.
//!
//!		This includes the public interface to CvLuaTeam.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CVLUATEAM_H
#define CVLUATEAM_H

#include "CvLuaScopedInstance.h"

class CvLuaTeam : public CvLuaScopedInstance<CvLuaTeam, CvTeam>
{
public:
	//! Push all player instances to Lua
	static void Register(lua_State* L);

	//! Push CvTeam methods into table t
	static void PushMethods(lua_State* L, int t);

	//! Required by CvLuaScopedInstance.
	static void HandleMissingInstance(lua_State* L);

	//! Required by CvLuaScopedInstance.
	static const char* GetTypeName();

protected:

	//! Protected Lua call to register all team instances.
	static int pRegister(lua_State* L);

	static int lIsNone(lua_State* L);

	static int lAddTeam(lua_State* L);

	static int lCanChangeWarPeace(lua_State* L);
	static int lCanDeclareWar(lua_State* L);
	static int lDeclareWar(lua_State* L);
	static int lMakePeace(lua_State* L);
	static int lGetNumTurnsLockedIntoWar(lua_State* L);
	static int lMeet(lua_State* L);

	static int lGetScore(lua_State* L);
	static int lGetPower(lua_State* L);
	static int lGetDefensivePower(lua_State* L);
	static int lGetNumNukeUnits(lua_State* L);

	static int lGetTeamVotingForInDiplo(lua_State* L);
	static int lGetProjectedVotesFromMinorAllies(lua_State* L);
	static int lGetProjectedVotesFromLiberatedMinors(lua_State* L);
	static int lGetProjectedVotesFromCivs(lua_State* L);
	static int lGetTotalProjectedVotes(lua_State* L);
	static int lGetTotalSecuredVotes(lua_State* L); // DEPRECATED, use lGetTotalProjectedVotes instead

	static int lGetAtWarCount(lua_State* L);
	static int lGetHasMetCivCount(lua_State* L);
	static int lHasMetHuman(lua_State* L);
	static int lGetDefensivePactCount(lua_State* L);

	static int lGetUnitClassMaking(lua_State* L);
	static int lGetUnitClassCountPlusMaking(lua_State* L);
	static int lGetBuildingClassMaking(lua_State* L);
	static int lGetBuildingClassCountPlusMaking(lua_State* L);

	static int lCountNumUnitsByArea(lua_State* L);
	static int lCountNumCitiesByArea(lua_State* L);
	static int lCountTotalPopulationByArea(lua_State* L);
	static int lCountEnemyDangerByArea(lua_State* L);

	static int lIsHuman(lua_State* L);
	static int lIsBarbarian(lua_State* L);

	static int lIsMinorCiv(lua_State* L);
	static int lIsMinorCivWarmonger(lua_State* L);

	static int lGetLeaderID(lua_State* L);
	static int lGetSecretaryID(lua_State* L);
	static int lGetHandicapType(lua_State* L);
	static int lGetName(lua_State* L);
	static int lGetNameKey(lua_State* L);

	static int lGetNumMembers(lua_State* L);
	static int lIsAlive(lua_State* L);
	static int lIsEverAlive(lua_State* L);
	static int lGetNumCities(lua_State* L);
	static int lGetTotalPopulation(lua_State* L);
	static int lGetTotalLand(lua_State* L);
	static int lGetNukeInterception(lua_State* L);
	static int lChangeNukeInterception(lua_State* L);

	static int lGetForceTeamVoteEligibilityCount(lua_State* L);
	static int lIsForceTeamVoteEligible(lua_State* L);
	static int lChangeForceTeamVoteEligibilityCount(lua_State* L);
	static int lGetExtraWaterSeeFromCount(lua_State* L);
	static int lIsExtraWaterSeeFrom(lua_State* L);
	static int lChangeExtraWaterSeeFromCount(lua_State* L);
	static int lGetMapTradingCount(lua_State* L);
	static int lIsMapTrading(lua_State* L);
	static int lChangeMapTradingCount(lua_State* L);
	static int lGetTechTradingCount(lua_State* L);
	static int lIsTechTrading(lua_State* L);
	static int lChangeTechTradingCount(lua_State* L);
	static int lGetGoldTradingCount(lua_State* L);
	static int lIsGoldTrading(lua_State* L);
	static int lChangeGoldTradingCount(lua_State* L);

	static int lGetAllowEmbassyTradingAllowedCount(lua_State* L);
	static int lIsAllowEmbassyTradingAllowed(lua_State* L);
	static int lChangeAllowEmbassyTradingAllowedCount(lua_State* L);
	static int lGetOpenBordersTradingAllowedCount(lua_State* L);
	static int lIsOpenBordersTradingAllowed(lua_State* L);
	static int lIsOpenBordersTradingAllowedWithTeam(lua_State* L);
	static int lChangeOpenBordersTradingAllowedCount(lua_State* L);
	static int lGetDefensivePactTradingAllowedCount(lua_State* L);
	static int lIsDefensivePactTradingAllowed(lua_State* L);
	static int lIsDefensivePactTradingAllowedWithTeam(lua_State* L);
	static int lChangeDefensivePactTradingAllowedCount(lua_State* L);
	static int lIsResearchAgreementTradingAllowed(lua_State* L);
	static int lIsResearchAgreementTradingAllowedWithTeam(lua_State* L);
	static int lIsTradeAgreementTradingAllowed(lua_State* L);
	static int lGetPermanentAllianceTradingCount(lua_State* L);
	static int lIsPermanentAllianceTrading(lua_State* L);
	static int lChangePermanentAllianceTradingCount(lua_State* L);
	static int lGetBridgeBuildingCount(lua_State* L);
	static int lIsBridgeBuilding(lua_State* L);
	static int lChangeBridgeBuildingCount(lua_State* L);
	static int lGetWaterWorkCount(lua_State* L);
	static int lIsWaterWork(lua_State* L);
	static int lChangeWaterWorkCount(lua_State* L);

	static int lGetBorderObstacleCount(lua_State* L);
	static int lIsBorderObstacle(lua_State* L);
	static int lChangeBorderObstacleCount(lua_State* L);

	static int lIsMapCentering(lua_State* L);
	static int lSetMapCentering(lua_State* L);

	static int lGetID(lua_State* L);

	static int lGetTechShareCount(lua_State* L);
	static int lIsTechShare(lua_State* L);
	static int lChangeTechShareCount(lua_State* L);

	static int lGetExtraMoves(lua_State* L);
	static int lChangeExtraMoves(lua_State* L);

	static int lCanEmbark(lua_State* L);
	static int lCanDefensiveEmbark(lua_State* L);

	static int lIsHasMet(lua_State* L);
	static int lIsAtWar(lua_State* L);
	static int lIsPermanentWarPeace(lua_State* L);
	static int lSetPermanentWarPeace(lua_State* L);

	static int lGetLiberatedByTeam(lua_State* L);
	static int lGetKilledByTeam(lua_State* L);

	static int lHasEmbassyAtTeam(lua_State* L);
	static int lIsAllowsOpenBordersToTeam(lua_State* L);
	static int lIsForcePeace(lua_State* L);
	static int lIsDefensivePact(lua_State* L);
	static int lGetRouteChange(lua_State* L);
	static int lChangeRouteChange(lua_State* L);
	static int lGetProjectCount(lua_State* L);
	static int lGetProjectDefaultArtType(lua_State* L);
	static int lSetProjectDefaultArtType(lua_State* L);
	static int lGetProjectArtType(lua_State* L);
	static int lSetProjectArtType(lua_State* L);
	static int lIsProjectMaxedOut(lua_State* L);
	static int lIsProjectAndArtMaxedOut(lua_State* L);
	static int lChangeProjectCount(lua_State* L);
	static int lGetProjectMaking(lua_State* L);
	static int lGetUnitClassCount(lua_State* L);
	static int lIsUnitClassMaxedOut(lua_State* L);
	static int lGetBuildingClassCount(lua_State* L);
	static int lIsBuildingClassMaxedOut(lua_State* L);
	static int lGetObsoleteBuildingCount(lua_State* L);
	static int lIsObsoleteBuilding(lua_State* L);

	static int lIsHasResearchAgreement(lua_State* L);
	static int lIsHasTradeAgreement(lua_State* L);

	static int lIsHasTech(lua_State* L);
	static int lSetHasTech(lua_State* L);

	static int lGetTeamTechs(lua_State* L);

	static int lGetImprovementYieldChange(lua_State* L);
	static int lChangeImprovementYieldChange(lua_State* L);

	static int lIsHomeOfUnitedNations(lua_State* L);

	static int lGetVictoryCountdown(lua_State* L);
	static int lGetVictoryDelay(lua_State* L);
	static int lCanLaunch(lua_State* L);

	static int lGetVictoryPoints(lua_State* L);
	static int lChangeVictoryPoints(lua_State* L);

	static int lGetCurrentEra(lua_State* L);
	static int lSetCurrentEra(lua_State* L);

	static int lUpdateEmbarkGraphics(lua_State* L);
};

#endif //CVLUATEAM_H
