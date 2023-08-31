/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once
#ifndef CVLUADEAL_H
#define CVLUADEAL_H

#include "CvLuaScopedInstance.h"

class CvLuaDeal : public CvLuaScopedInstance<CvLuaDeal, CvDeal>
{
public:
	//! Push CvCity methods into table t
	static void PushMethods(lua_State* L, int t);

	//! Required by CvLuaScopedInstance.
	static void HandleMissingInstance(lua_State* L);

	//! Required by CvLuaScopedInstance.
	static const char* GetTypeName();

protected:
	static int lResetIterator(lua_State* L);
	static int lGetNextItem(lua_State* L);

	static int lClearItems(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::ClearItems);
	};
	static int lGetNumItems(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetNumItems);
	};

	static int lGetStartTurn(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetStartTurn);
	};
	static int lGetEndTurn(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetEndTurn);
	};
	static int lGetDuration(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetDuration);
	};

	static int lSetFromPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::SetFromPlayer);
	};
	static int lSetToPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::SetToPlayer);
	};
	static int lGetOtherPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetOtherPlayer);
	};
	static int lGetToPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetToPlayer);
	};
	static int lGetFromPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetFromPlayer);
	};

	static int lGetSurrenderingPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetSurrenderingPlayer);
	};
	static int lSetSurrenderingPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::SetSurrenderingPlayer);
	};
	static int lGetDemandingPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetDemandingPlayer);
	};
	static int lSetDemandingPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::SetDemandingPlayer);
	};
	static int lGetRequestingPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetRequestingPlayer);
	};
	static int lSetRequestingPlayer(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::SetRequestingPlayer);
	};

	static int lGetGoldAvailable(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::GetGoldAvailable);
	};

	static int lIsPossibleToTradeItem(lua_State* L);
	static int lGetReasonsItemUntradeable(lua_State* L);
	static int lBlockTemporaryForPermanentTrade(lua_State* L);

	static int lAddGoldTrade(lua_State* L);
	static int lAddGoldPerTurnTrade(lua_State* L);
	static int lAddMapTrade(lua_State* L);
	static int lAddResourceTrade(lua_State* L);
	static int lAddCityTrade(lua_State* L);
	static int lAddAllowEmbassy(lua_State* L);
	static int lAddOpenBorders(lua_State* L);
	static int lAddDefensivePact(lua_State* L);
	static int lAddResearchAgreement(lua_State* L);
	static int lAddPeaceTreaty(lua_State* L);
	static int lAddThirdPartyPeace(lua_State* L);
	static int lAddThirdPartyWar(lua_State* L);
	static int lAddDeclarationOfFriendship(lua_State* L);
	static int lAddVoteCommitment(lua_State* L);

	static int lDoReevaluateDeal(lua_State* L);
	static int lGetRenewDealMessage(lua_State* L);
	static int lIsCheckedForRenewal(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::IsCheckedForRenewal);
	};
	static int lChangeGoldTrade(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::ChangeGoldTrade);
	};
	static int lChangeGoldPerTurnTrade(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::ChangeGoldPerTurnTrade);
	};
	static int lChangeResourceTrade(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::ChangeResourceTrade);
	};
	static int lChangeThirdPartyWarDuration(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::ChangeThirdPartyWarDuration);
	};
	static int lChangeThirdPartyPeaceDuration(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::ChangeThirdPartyPeaceDuration);
	};

	static int lRemoveByType(lua_State* L);
	static int lRemoveResourceTrade(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::RemoveResourceTrade);
	};
	static int lRemoveCityTrade(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::RemoveCityTrade);
	};
	static int lRemoveThirdPartyPeace(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::RemoveThirdPartyPeace);
	};
	static int lRemoveThirdPartyWar(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::RemoveThirdPartyWar);
	};
	static int lRemoveVoteCommitment(lua_State* L); // Too many args for template, defined in cpp

	static TradedItemList::iterator m_iterator;

	static int lAddTechTrade(lua_State* L);
	static int lAddVassalageTrade(lua_State* L);
	static int lAddRevokeVassalageTrade(lua_State* L);
	static int lRemoveTechTrade(lua_State* L)
	{
		return BasicLuaMethod(L, &CvDeal::RemoveTechTrade);
	};
};

namespace CvLuaArgs
{
	template<> inline const CvDeal* toValue(lua_State* L, int idx)
	{
		return CvLuaDeal::GetInstance(L, idx);
	}
	template<> inline CvDeal* toValue(lua_State* L, int idx)
	{
		return CvLuaDeal::GetInstance(L, idx);
	}
	template<> inline const CvDeal& toValue(lua_State* L, int idx)
	{
		return *CvLuaDeal::GetInstance(L, idx);
	}
	template<> inline CvDeal& toValue(lua_State* L, int idx)
	{
		return *CvLuaDeal::GetInstance(L, idx);
	}

	template<> inline void pushValue(lua_State* L, CvDeal* p)
	{
		CvLuaDeal::Push(L, p);
	}
	template<> inline void pushValue(lua_State* L, CvDeal& r)
	{
		CvLuaDeal::Push(L, &r);
	}
}

#endif //CVLUADEAL_H