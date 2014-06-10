/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once
#ifndef CVLUALEAGUE_H
#define CVLUALEAGUE_H

#include "CvLuaScopedInstance.h"


class CvLuaLeague : public CvLuaScopedInstance<CvLuaLeague, CvLeague>
{
public:
	//! Push CvLeague methods into table t
	static void PushMethods(lua_State* L, int t);

	//! Error handle for missing instances
	static void HandleMissingInstance(lua_State* L);

	//! Gets the type name
	static const char* GetTypeName();

protected:

	static int lIsNone(lua_State* L);
	
	static int lGetID(lua_State* L);
	static int lGetName(lua_State* L);
	static int lCanChangeCustomName(lua_State* L);
	
	static int lIsInSession(lua_State* L);
	static int lIsInSpecialSession(lua_State* L);
	static int lGetTurnsUntilSession(lua_State* L);
	static int lGetTurnsUntilVictorySession(lua_State* L);

	static int lCanProposeEnactAnyChoice(lua_State* L);
	static int lCanProposeEnact(lua_State* L);
	static int lCanProposeRepeal(lua_State* L);
	static int lIsProposed(lua_State* L);
	static int lGetChoicesForDecision(lua_State* L);
	static int lGetTextForChoice(lua_State* L);
	static int lGetInactiveResolutions(lua_State* L);
	static int lGetEnactProposals(lua_State* L);
	static int lGetEnactProposalsOnHold(lua_State* L);
	static int lGetRepealProposals(lua_State* L);
	static int lGetRepealProposalsOnHold(lua_State* L);
	static int lGetActiveResolutions(lua_State* L);

	static int lIsMember(lua_State* L);
	static int lCanVote(lua_State* L);
	static int lGetRemainingVotesForMember(lua_State* L);
	static int lGetSpentVotesForMember(lua_State* L);
	static int lGetCoreVotesForMember(lua_State* L);
	static int lCalculateStartingVotesForMember(lua_State* L);
	static int lCanPropose(lua_State* L);
	static int lGetRemainingProposalsForMember(lua_State* L);

	static int lIsHostMember(lua_State* L);
	static int lGetHostMember(lua_State* L);
	static int lIsUnitedNations(lua_State* L);

	static int lIsProjectActive(lua_State* L);
	static int lIsProjectComplete(lua_State* L);
	static int lGetProjectCostPerPlayer(lua_State* L);
	static int lGetProjectBuildingCostPerPlayer(lua_State* L);
	static int lGetProjectCost(lua_State* L);
	static int lGetMemberContribution(lua_State* L);
	static int lGetMemberContributionTier(lua_State* L);
	static int lGetContributionTierThreshold(lua_State* L);

	static int lGetArtsyGreatPersonRateModifier(lua_State* L);
	static int lGetScienceyGreatPersonRateModifier(lua_State* L);

	static int lGetResolutionName(lua_State* L);
	static int lGetResolutionDetails(lua_State* L);
	static int lGetMemberDetails(lua_State* L);
	static int lGetProjectDetails(lua_State* L);
	static int lGetProjectRewardTierDetails(lua_State* L);
	static int lGetCurrentEffectsSummary(lua_State* L);
	static int lGetLeagueSplashTitle(lua_State* L);
	static int lGetLeagueSplashDescription(lua_State* L);
	static int lGetLeagueSplashThisEraDetails(lua_State* L);
	static int lGetLeagueSplashNextEraDetails(lua_State* L);
	static int lGetGreatPersonRateModifierDetails(lua_State* L);

	// Helper functions
	static int lResolutionTableHelper(lua_State* L, const int iTop, CvResolution &resolution);
	static int lProposalTableHelper(lua_State* L, const int iTop, CvProposal &proposal);
};


#endif //CVLUALEAGUE_H
