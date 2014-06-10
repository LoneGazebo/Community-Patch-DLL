/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvLuaSupport.h"
#include "CvLuaLeague.h"

//Utility macro for registering methods
#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);


// ================================================================================
//			CvLuaLeague
// ================================================================================
//------------------------------------------------------------------------------
void CvLuaLeague::HandleMissingInstance(lua_State* L)
{
	luaL_error(L, "Instance no longer exists.");
}
//------------------------------------------------------------------------------
void CvLuaLeague::PushMethods(lua_State* L, int t)
{
	Method(IsNone);
	
	Method(GetID);
	Method(GetName);
	Method(CanChangeCustomName);

	Method(IsInSession);
	Method(IsInSpecialSession);
	Method(GetTurnsUntilSession);
	Method(GetTurnsUntilVictorySession);

	Method(CanProposeEnactAnyChoice);
	Method(CanProposeEnact);
	Method(CanProposeRepeal);
	Method(IsProposed);
	Method(GetChoicesForDecision);
	Method(GetTextForChoice);
	Method(GetInactiveResolutions);
	Method(GetEnactProposals);
	Method(GetEnactProposalsOnHold);
	Method(GetRepealProposals);
	Method(GetRepealProposalsOnHold);
	Method(GetActiveResolutions);

	Method(IsMember);
	Method(CanVote);
	Method(GetRemainingVotesForMember);
	Method(GetSpentVotesForMember);
	Method(GetCoreVotesForMember);
	Method(CalculateStartingVotesForMember);
	Method(CanPropose);
	Method(GetRemainingProposalsForMember);

	Method(IsHostMember);
	Method(GetHostMember);
	Method(IsUnitedNations);

	Method(IsProjectActive);
	Method(IsProjectComplete);
	Method(GetProjectCostPerPlayer);
	Method(GetProjectBuildingCostPerPlayer);
	Method(GetProjectCost);
	Method(GetMemberContribution);
	Method(GetMemberContributionTier);
	Method(GetContributionTierThreshold);

	Method(GetArtsyGreatPersonRateModifier);
	Method(GetScienceyGreatPersonRateModifier);
	
	Method(GetResolutionName);
	Method(GetResolutionDetails);
	Method(GetMemberDetails);
	Method(GetProjectDetails);
	Method(GetProjectRewardTierDetails);
	Method(GetCurrentEffectsSummary);
	Method(GetLeagueSplashTitle);
	Method(GetLeagueSplashDescription);
	Method(GetLeagueSplashThisEraDetails);
	Method(GetLeagueSplashNextEraDetails);
	Method(GetGreatPersonRateModifierDetails);
}
//------------------------------------------------------------------------------
const char* CvLuaLeague::GetTypeName()
{
	return "League";
}
//------------------------------------------------------------------------------
// Lua member methods
//------------------------------------------------------------------------------
//bool IsNone();
int CvLuaLeague::lIsNone(lua_State* L)
{
	const bool bDoesNotExist = (GetInstance(L, false) == NULL);
	lua_pushboolean(L, bDoesNotExist);
	return 1;
}
//------------------------------------------------------------------------------
//int GetID();
int CvLuaLeague::lGetID(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	const int iValue = (int) pLeague->GetID();
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//string GetName();
int CvLuaLeague::lGetName(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	Localization::String strName = pLeague->GetName();
	const CvString sResult = strName.toUTF8();
	lua_pushstring(L, sResult);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanChangeCustomName();
int CvLuaLeague::lCanChangeCustomName(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pLeague->CanChangeCustomName(ePlayer);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsInSession();
int CvLuaLeague::lIsInSession(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	const bool bValue = pLeague->IsInSession();
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsInSpecialSession();
int CvLuaLeague::lIsInSpecialSession(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	const bool bValue = pLeague->IsInSpecialSession();
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetTurnsUntilSession();
int CvLuaLeague::lGetTurnsUntilSession(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	const int iValue = pLeague->GetTurnsUntilSession();
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetTurnsUntilVictorySession();
int CvLuaLeague::lGetTurnsUntilVictorySession(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	const int iValue = pLeague->GetTurnsUntilVictorySession();
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanProposeEnactAnyChoice(ResolutionTypes eResolution, PlayerTypes eProposer);
int CvLuaLeague::lCanProposeEnactAnyChoice(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const ResolutionTypes eResolution = (ResolutionTypes) lua_tointeger(L, 2);
	const PlayerTypes eProposer = (PlayerTypes) lua_tointeger(L, 3);

	const bool bValue = pLeague->CanProposeEnactAnyChoice(eResolution, eProposer);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanProposeEnact(ResolutionTypes eResolution, PlayerTypes eProposer, int iChoice);
int CvLuaLeague::lCanProposeEnact(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const ResolutionTypes eResolution = (ResolutionTypes) lua_tointeger(L, 2);
	const PlayerTypes eProposer = (PlayerTypes) lua_tointeger(L, 3);
	const int iChoice = lua_tointeger(L, 4);

	const bool bValue = pLeague->CanProposeEnact(eResolution, eProposer, iChoice);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanProposeRepeal(int iResolutionID, PlayerTypes eProposer);
int CvLuaLeague::lCanProposeRepeal(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const int iResolutionID = lua_tointeger(L, 2);
	const PlayerTypes eProposer = (PlayerTypes) lua_tointeger(L, 3);

	const bool bValue = pLeague->CanProposeRepeal(iResolutionID, eProposer);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsProposed(int iResolutionID, bool bRepeal);
int CvLuaLeague::lIsProposed(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const int iResolutionID = lua_tointeger(L, 2);
	const bool bRepeal = lua_toboolean(L, 3);

	const bool bValue = pLeague->IsProposed(iResolutionID, bRepeal);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//table GetChoicesForDecision(ResolutionDecisionTypes eDecision, PlayerTypes eDecider);
int CvLuaLeague::lGetChoicesForDecision(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const ResolutionDecisionTypes eDecision = (ResolutionDecisionTypes) lua_tointeger(L, 2);
	const PlayerTypes eDecider = (PlayerTypes) lua_tointeger(L, 3);

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	int iIndex = 1;

	std::vector<int> v = pLeague->GetChoicesForDecision(eDecision, eDecider);
	for (std::vector<int>::iterator it = v.begin(); it != v.end(); it++)
	{
		const int iChoice = (*it);
		lua_pushinteger(L, iChoice);
		lua_rawseti(L, t, iIndex++);
	}
	return 1;
}
//------------------------------------------------------------------------------
//string GetTextForChoice(ResolutionDecisionTypes eDecision, int iChoice);
int CvLuaLeague::lGetTextForChoice(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const ResolutionDecisionTypes eDecision = (ResolutionDecisionTypes) lua_tointeger(L, 2);
	const int iChoice = lua_tointeger(L, 3);

	const CvString sResult = pLeague->GetTextForChoice(eDecision, iChoice);
	lua_pushstring(L, sResult);
	return 1;
}
//------------------------------------------------------------------------------
//table GetInactiveResolutions();
int CvLuaLeague::lGetInactiveResolutions(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	lua_createtable(L, 0, 0);
	int iIndex = 1;

	std::vector<ResolutionTypes> v = pLeague->GetInactiveResolutions();
	for(std::vector<ResolutionTypes>::iterator it = v.begin(); it != v.end(); ++it)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		const ResolutionTypes eResolution = (*it);
		lua_pushinteger(L, eResolution);
		lua_setfield(L, t, "Type");

		lua_rawseti(L, -2, iIndex++);
	}
	return 1;
}
//------------------------------------------------------------------------------
//table GetEnactProposals();
int CvLuaLeague::lGetEnactProposals(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	lua_createtable(L, 0, 0);
	int iIndex = 1;

	EnactProposalList v = pLeague->GetEnactProposals();
	for(EnactProposalList::iterator it = v.begin(); it != v.end(); ++it)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lProposalTableHelper(L, t, (*it));

		lua_rawseti(L, -2, iIndex++);
	}
	return 1;
}
//------------------------------------------------------------------------------
//table GetEnactProposalsOnHold();
int CvLuaLeague::lGetEnactProposalsOnHold(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	lua_createtable(L, 0, 0);
	int iIndex = 1;

	EnactProposalList v = pLeague->GetEnactProposalsOnHold();
	for(EnactProposalList::iterator it = v.begin(); it != v.end(); ++it)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lProposalTableHelper(L, t, (*it));

		lua_rawseti(L, -2, iIndex++);
	}
	return 1;
}
//------------------------------------------------------------------------------
//table GetRepealProposals();
int CvLuaLeague::lGetRepealProposals(lua_State* L) 
{
	CvLeague* pLeague = GetInstance(L);

	lua_createtable(L, 0, 0);
	int iIndex = 1;

	RepealProposalList v = pLeague->GetRepealProposals();
	for(RepealProposalList::iterator it = v.begin(); it != v.end(); ++it)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lProposalTableHelper(L, t, (*it));

		lua_pushinteger(L, (*it).GetTargetResolutionID());
		lua_setfield(L, t, "TargetResolutionID");

		lua_pushinteger(L, (*it).GetRepealDecision()->GetDecision());
		lua_setfield(L, t, "RepealDecision");

		lua_rawseti(L, -2, iIndex++);
	}
	return 1;
}
//------------------------------------------------------------------------------
//table GetRepealProposalsOnHold();
int CvLuaLeague::lGetRepealProposalsOnHold(lua_State* L) 
{
	CvLeague* pLeague = GetInstance(L);

	lua_createtable(L, 0, 0);
	int iIndex = 1;

	RepealProposalList v = pLeague->GetRepealProposalsOnHold();
	for(RepealProposalList::iterator it = v.begin(); it != v.end(); ++it)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lProposalTableHelper(L, t, (*it));

		lua_pushinteger(L, (*it).GetTargetResolutionID());
		lua_setfield(L, t, "TargetResolutionID");

		lua_pushinteger(L, (*it).GetRepealDecision()->GetDecision());
		lua_setfield(L, t, "RepealDecision");

		lua_rawseti(L, -2, iIndex++);
	}
	return 1;
}
//------------------------------------------------------------------------------
//table GetActiveResolutions();
int CvLuaLeague::lGetActiveResolutions(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	lua_createtable(L, 0, 0);
	int iIndex = 1;

	ActiveResolutionList v = pLeague->GetActiveResolutions();
	for(ActiveResolutionList::iterator it = v.begin(); it != v.end(); ++it)
	{
		lua_createtable(L, 0, 0);
		const int t = lua_gettop(L);

		lResolutionTableHelper(L, t, (*it));

		lua_pushinteger(L, (*it).GetTurnEnacted());
		lua_setfield(L, t, "TurnEnacted");

		lua_rawseti(L, -2, iIndex++);
	}
	return 1;
}
//------------------------------------------------------------------------------
//bool IsMember(PlayerTypes ePlayer);
int CvLuaLeague::lIsMember(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pLeague->IsMember(ePlayer);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanVote(PlayerTypes ePlayer);
int CvLuaLeague::lCanVote(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pLeague->CanVote(ePlayer);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetRemainingVotesForMember(PlayerTypes ePlayer);
int CvLuaLeague::lGetRemainingVotesForMember(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pLeague->GetRemainingVotesForMember(ePlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetSpentVotesForMember(PlayerTypes ePlayer);
int CvLuaLeague::lGetSpentVotesForMember(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pLeague->GetSpentVotesForMember(ePlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetCoreVotesForMember(PlayerTypes ePlayer);
int CvLuaLeague::lGetCoreVotesForMember(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pLeague->GetCoreVotesForMember(ePlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int CalculateStartingVotesForMember(PlayerTypes ePlayer);
int CvLuaLeague::lCalculateStartingVotesForMember(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pLeague->CalculateStartingVotesForMember(ePlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool CanPropose(PlayerTypes ePlayer);
int CvLuaLeague::lCanPropose(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pLeague->CanPropose(ePlayer);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetRemainingProposalsForMember(PlayerTypes ePlayer);
int CvLuaLeague::lGetRemainingProposalsForMember(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const int iValue = pLeague->GetRemainingProposalsForMember(ePlayer);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsHostMember(PlayerTypes ePlayer);
int CvLuaLeague::lIsHostMember(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);

	const bool bValue = pLeague->IsHostMember(ePlayer);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//PlayerTypes GetHostMember();
int CvLuaLeague::lGetHostMember(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	
	const PlayerTypes eHost = pLeague->GetHostMember();
	lua_pushinteger(L, (int)eHost);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsUnitedNations();
int CvLuaLeague::lIsUnitedNations(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	const bool bValue = pLeague->IsUnitedNations();
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsProjectActive(LeagueProjectTypes eLeagueProject);
int CvLuaLeague::lIsProjectActive(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) lua_tointeger(L, 2);

	const bool bValue = pLeague->IsProjectActive(eLeagueProject);
	lua_pushboolean(L, bValue);
	return 1;
}
//------------------------------------------------------------------------------
//bool IsProjectComplete(LeagueProjectTypes eLeagueProject);
int CvLuaLeague::lIsProjectComplete(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) lua_tointeger(L, 2);

	const bool bValue = pLeague->IsProjectComplete(eLeagueProject);
	lua_pushboolean(L, bValue);
	return 1;
}

//------------------------------------------------------------------------------
//int GetProjectCostPerPlayer(LeagueProjectTypes eLeagueProject);
int CvLuaLeague::lGetProjectCostPerPlayer(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) lua_tointeger(L, 2);

	const int iValue = pLeague->GetProjectCostPerPlayer(eLeagueProject);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetProjectBuildingCostPerPlayer(BuildingTypes eRewardBuilding);
int CvLuaLeague::lGetProjectBuildingCostPerPlayer(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const BuildingTypes eRewardBuilding = (BuildingTypes) lua_tointeger(L, 2);

	const int iValue = pLeague->GetProjectBuildingCostPerPlayer(eRewardBuilding);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetProjectCost(LeagueProjectTypes eLeagueProject);
int CvLuaLeague::lGetProjectCost(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) lua_tointeger(L, 2);

	const int iValue = pLeague->GetProjectCost(eLeagueProject);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetMemberContribution(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject);
int CvLuaLeague::lGetMemberContribution(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) lua_tointeger(L, 3);

	const int iValue = pLeague->GetMemberContribution(ePlayer, eLeagueProject);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetMemberContributionTier(PlayerTypes ePlayer, LeagueProjectTypes eLeagueProject);
int CvLuaLeague::lGetMemberContributionTier(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) lua_tointeger(L, 3);

	const int iValue = pLeague->GetMemberContributionTier(ePlayer, eLeagueProject);
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetContributionTierThreshold(int iTier, LeagueProjectTypes eLeagueProject);
int CvLuaLeague::lGetContributionTierThreshold(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const CvLeague::ContributionTier eTier = (CvLeague::ContributionTier) lua_tointeger(L, 2);
	const LeagueProjectTypes eLeagueProject = (LeagueProjectTypes) lua_tointeger(L, 3);

	float fValue = pLeague->GetContributionTierThreshold(eTier, eLeagueProject);
	const int iValue = (int) fValue;
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetArtsyGreatPersonRateModifier();
int CvLuaLeague::lGetArtsyGreatPersonRateModifier(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	int iValue = pLeague->GetArtsyGreatPersonRateModifier();
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//int GetScienceyGreatPersonRateModifier();
int CvLuaLeague::lGetScienceyGreatPersonRateModifier(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);

	int iValue = pLeague->GetScienceyGreatPersonRateModifier();
	lua_pushinteger(L, iValue);
	return 1;
}
//------------------------------------------------------------------------------
//string GetResolutionName(ResolutionTypes eResolution, int iResolutionID, int iProposerChoice, bool bIncludePrefix);
int CvLuaLeague::lGetResolutionName(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const ResolutionTypes eResolution = (ResolutionTypes) lua_tointeger(L, 2);
	const int iResolutionID = lua_tointeger(L, 3);
	const int iProposerChoice = lua_tointeger(L, 4);
	const bool bIncludePrefix = lua_toboolean(L, 5);

	CvString sValue = pLeague->GetResolutionName(eResolution, iResolutionID, iProposerChoice, bIncludePrefix);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//string GetResolutionDetails(ResolutionTypes eResolution, PlayerTypes eObserver, int iResolutionID, int iProposerChoice);
int CvLuaLeague::lGetResolutionDetails(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const ResolutionTypes eResolution = (ResolutionTypes) lua_tointeger(L, 2);
	const PlayerTypes eObserver = (PlayerTypes) lua_tointeger(L, 3);
	const int iResolutionID = lua_tointeger(L, 4);
	const int iProposerChoice = lua_tointeger(L, 5);

	CvString sValue = pLeague->GetResolutionDetails(eResolution, eObserver, iResolutionID, iProposerChoice);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//string GetMemberDetails(PlayerTypes eMember, PlayerTypes eObserver);
int CvLuaLeague::lGetMemberDetails(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes eMember = (PlayerTypes) lua_tointeger(L, 2);
	const PlayerTypes eObserver = (PlayerTypes) lua_tointeger(L, 3);

	CvString sValue = pLeague->GetMemberDetails(eMember, eObserver);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//string GetProjectDetails(LeagueProjectTypes eProject, PlayerTypes eObserver);
int CvLuaLeague::lGetProjectDetails(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const LeagueProjectTypes eProject = (LeagueProjectTypes) lua_tointeger(L, 2);
	const PlayerTypes eObserver = (PlayerTypes) luaL_optint(L, 3, NO_PLAYER);

	CvString sValue = pLeague->GetProjectDetails(eProject, eObserver);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//string GetProjectRewardTierDetails(int iTier, LeagueProjectTypes eProject, PlayerTypes eObserver);
int CvLuaLeague::lGetProjectRewardTierDetails(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const int iTier = lua_tointeger(L, 2);
	const LeagueProjectTypes eProject = (LeagueProjectTypes) lua_tointeger(L, 3);
	const PlayerTypes eObserver = (PlayerTypes) luaL_optint(L, 4, NO_PLAYER);

	CvString sValue = pLeague->GetProjectRewardTierDetails(iTier, eProject, eObserver);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//table GetCurrentEffectsSummary(PlayerTypes eObserver);
int CvLuaLeague::lGetCurrentEffectsSummary(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const PlayerTypes eObserver = (PlayerTypes) luaL_optint(L, 2, NO_PLAYER);

	lua_createtable(L, 0, 0);
	const int t = lua_gettop(L);
	std::vector<CvString> vsEffects = pLeague->GetCurrentEffectsSummary(eObserver);
	for(uint i = 0; i < vsEffects.size(); i++)
	{
		const CvString s = vsEffects[i];
		lua_pushstring(L, s);
		lua_rawseti(L, t, i + 1);
	}

	return 1;
}
//------------------------------------------------------------------------------
//string GetLeagueSplashTitle(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded);
int CvLuaLeague::lGetLeagueSplashTitle(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const LeagueSpecialSessionTypes eGoverningSpecialSession = (LeagueSpecialSessionTypes) lua_tointeger(L, 2);
	const bool bJustFounded = lua_toboolean(L, 3);

	CvString sValue = pLeague->GetLeagueSplashTitle(eGoverningSpecialSession, bJustFounded);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//string GetLeagueSplashDescription(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded);
int CvLuaLeague::lGetLeagueSplashDescription(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const LeagueSpecialSessionTypes eGoverningSpecialSession = (LeagueSpecialSessionTypes) lua_tointeger(L, 2);
	const bool bJustFounded = lua_toboolean(L, 3);

	CvString sValue = pLeague->GetLeagueSplashDescription(eGoverningSpecialSession, bJustFounded);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//string GetLeagueSplashThisEraDetails(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded);
int CvLuaLeague::lGetLeagueSplashThisEraDetails(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const LeagueSpecialSessionTypes eGoverningSpecialSession = (LeagueSpecialSessionTypes) lua_tointeger(L, 2);
	const bool bJustFounded = lua_toboolean(L, 3);

	CvString sValue = pLeague->GetLeagueSplashThisEraDetails(eGoverningSpecialSession, bJustFounded);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//string GetLeagueSplashNextEraDetails(LeagueSpecialSessionTypes eGoverningSpecialSession, bool bJustFounded);
int CvLuaLeague::lGetLeagueSplashNextEraDetails(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const LeagueSpecialSessionTypes eGoverningSpecialSession = (LeagueSpecialSessionTypes) lua_tointeger(L, 2);
	const bool bJustFounded = lua_toboolean(L, 3);

	CvString sValue = pLeague->GetLeagueSplashNextEraDetails(eGoverningSpecialSession, bJustFounded);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
//string GetGreatPersonRateModifierDetails(UnitClassTypes eGreatPersonClass);
int CvLuaLeague::lGetGreatPersonRateModifierDetails(lua_State* L)
{
	CvLeague* pLeague = GetInstance(L);
	const UnitClassTypes eGreatPersonClass = (UnitClassTypes) lua_tointeger(L, 2);

	CvString sValue = pLeague->GetGreatPersonRateModifierDetails(eGreatPersonClass);
	lua_pushstring(L, sValue.c_str());
	return 1;
}
//------------------------------------------------------------------------------
// Helper functions
//------------------------------------------------------------------------------
int CvLuaLeague::lResolutionTableHelper(lua_State* L, const int iTop, CvResolution &resolution)
{
	lua_pushinteger(L, resolution.GetID());
	lua_setfield(L, iTop, "ID");
	
	lua_pushinteger(L, resolution.GetType());
	lua_setfield(L, iTop, "Type");

	lua_pushinteger(L, resolution.GetVoterDecision()->GetDecision());
	lua_setfield(L, iTop, "VoterDecision");

	lua_pushinteger(L, resolution.GetProposerDecision()->GetDecision());
	lua_setfield(L, iTop, "ProposerDecision");

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaLeague::lProposalTableHelper(lua_State* L, const int iTop, CvProposal &proposal)
{
	lResolutionTableHelper(L, iTop, proposal);

	lua_pushinteger(L, proposal.GetProposalPlayer());
	lua_setfield(L, iTop, "ProposalPlayer");
	
	return 0;
}
