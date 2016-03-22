/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "../CustomMods.h"
#include "CvLuaSupport.h"
#include "CvLuaDeal.h"

//Utility macro for registering methods
#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);


using namespace CvLuaArgs;
TradedItemList::iterator CvLuaDeal::m_iterator;

//------------------------------------------------------------------------------
void CvLuaDeal::PushMethods(lua_State* L, int t)
{
	Method(ClearItems);
	Method(GetNumItems);

	Method(GetStartTurn);
	Method(GetEndTurn);
	Method(GetDuration);

	Method(GetOtherPlayer);
	Method(GetFromPlayer);
	Method(GetToPlayer);
	Method(SetFromPlayer);
	Method(SetToPlayer);

	Method(GetSurrenderingPlayer);
	Method(SetSurrenderingPlayer);
	Method(GetDemandingPlayer);
	Method(SetDemandingPlayer);
	Method(GetRequestingPlayer);
	Method(SetRequestingPlayer);

	Method(ResetIterator);
	Method(GetNextItem);

	Method(GetGoldAvailable);

	Method(IsPossibleToTradeItem);
	Method(GetNumResource);

	Method(AddGoldTrade);
	Method(AddGoldPerTurnTrade);
	Method(AddMapTrade);
	Method(AddResourceTrade);
	Method(AddCityTrade);
	Method(AddUnitTrade);
	Method(AddAllowEmbassy);
	Method(AddOpenBorders);
	Method(AddDefensivePact);
	Method(AddResearchAgreement);
	Method(AddTradeAgreement);
	Method(AddPermamentAlliance);
	Method(AddSurrender);
	Method(AddTruce);
	Method(AddPeaceTreaty);
	Method(AddThirdPartyPeace);
	Method(AddThirdPartyWar);
	Method(AddThirdPartyEmbargo);
	Method(AddDeclarationOfFriendship);
	Method(AddVoteCommitment);

	Method(RemoveByType);
	Method(RemoveResourceTrade);
	Method(RemoveCityTrade);
	Method(RemoveUnitTrade);
	Method(RemoveThirdPartyPeace);
	Method(RemoveThirdPartyWar);
	Method(RemoveThirdPartyEmbargo);
	Method(RemoveVoteCommitment);

	Method(ChangeGoldTrade);
	Method(ChangeGoldPerTurnTrade);
	Method(ChangeResourceTrade);
	Method(ChangeThirdPartyWarDuration);
	Method(ChangeThirdPartyPeaceDuration);
	Method(ChangeThirdPartyEmbargoDuration);

#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_DIPLOMACY_CIV4_FEATURES)
	Method(AddTechTrade);
	Method(AddVassalageTrade);
	Method(AddRevokeVassalageTrade);
	Method(RemoveTechTrade);
#endif
}

//------------------------------------------------------------------------------
int CvLuaDeal::lRemoveByType(lua_State* L)
{
	CvDeal* pkDeal = GetInstance(L);
	int args = lua_gettop(L);

	if(args == 1)
		pkDeal->RemoveByType((TradeableItems) lua_tointeger(L, 2), NO_PLAYER);
	else
		pkDeal->RemoveByType((TradeableItems) lua_tointeger(L, 2), (PlayerTypes) lua_tointeger(L, 3));

	return 0;
}


//------------------------------------------------------------------------------
void CvLuaDeal::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}
//------------------------------------------------------------------------------
const char* CvLuaDeal::GetTypeName()
{
	return "Deal";
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Lua Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int CvLuaDeal::lIsPossibleToTradeItem(lua_State* L)
{
	CvDeal* pkDeal = GetInstance(L);
	const PlayerTypes eFromPlayer = (PlayerTypes) lua_tointeger(L, 2);
	const PlayerTypes eToPlayer = (PlayerTypes) lua_tointeger(L, 3);
	const TradeableItems eItem = (TradeableItems) lua_tointeger(L, 4);
	const int iData1 = lua_tointeger(L, 5);
	const int iData2 = lua_tointeger(L, 6);
	const int iData3 = lua_tointeger(L, 7);
	const bool bFlag1 = lua_toboolean(L, 8);

	const bool bResult = pkDeal->IsPossibleToTradeItem(eFromPlayer, eToPlayer, eItem, iData1, iData2, iData3, bFlag1);
	lua_pushboolean(L, bResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaDeal::lGetNumResource(lua_State* L)
{
	CvDeal* pkDeal = GetInstance(L);
	const PlayerTypes ePlayer = (PlayerTypes) lua_tointeger(L, 2);
	const ResourceTypes eResource = (ResourceTypes) lua_tointeger(L, 3);
	int iResult = pkDeal->GetNumResource(ePlayer, eResource);
	lua_pushinteger(L, iResult);
	return 1;
}

//------------------------------------------------------------------------------
int CvLuaDeal::lResetIterator(lua_State* L)
{
	CvDeal* pkDeal = GetInstance(L);
	m_iterator = pkDeal->m_TradedItems.begin();
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaDeal::lGetNextItem(lua_State* L)
{
	CvDeal* pkDeal = GetInstance(L);

	if(m_iterator == pkDeal->m_TradedItems.end())
		return 0;

	const CvTradedItem& item = (*m_iterator);
	lua_pushinteger(L, item.m_eItemType);
	lua_pushinteger(L, item.m_iDuration);
	lua_pushinteger(L, item.m_iFinalTurn);
	lua_pushinteger(L, item.m_iData1);
	lua_pushinteger(L, item.m_iData2);
	lua_pushinteger(L, item.m_iData3);
	lua_pushboolean(L, item.m_bFlag1);
	lua_pushinteger(L, item.m_eFromPlayer);

	m_iterator++;

	return 8;
}

//------------------------------------------------------------------------------
int CvLuaDeal::lAddVoteCommitment(lua_State* L)
{
	CvDeal* pkDeal = GetInstance(L);
	const PlayerTypes eFromPlayer = (PlayerTypes) lua_tointeger(L, 2);
	const int iResolutionID = lua_tointeger(L, 3);
	const int iVoteChoice = lua_tointeger(L, 4);
	const int iNumVotes = lua_tointeger(L, 5);
	const bool bRepeal = lua_toboolean(L, 6);

	pkDeal->AddVoteCommitment(eFromPlayer, iResolutionID, iVoteChoice, iNumVotes, bRepeal);
	return 0;
}

//------------------------------------------------------------------------------
int CvLuaDeal::lRemoveVoteCommitment(lua_State* L)
{
	CvDeal* pkDeal = GetInstance(L);
	const PlayerTypes eFromPlayer = (PlayerTypes) lua_tointeger(L, 2);
	const int iResolutionID = lua_tointeger(L, 3);
	const int iVoteChoice = lua_tointeger(L, 4);
	const int iNumVotes = lua_tointeger(L, 5);
	const bool bRepeal = lua_toboolean(L, 6);

	pkDeal->RemoveVoteCommitment(eFromPlayer, iResolutionID, iVoteChoice, iNumVotes, bRepeal);
	return 0;
}