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
	static int lResetIterator( lua_State *L );
	static int lGetNextItem( lua_State *L );

	static int lClearItems( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::ClearItems ); };
	static int lGetNumItems( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetNumItems ); };

    static int lGetStartTurn( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetStartTurn ); };
	static int lGetEndTurn( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetEndTurn ); };
	static int lGetDuration( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetDuration ); };

	static int lSetFromPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::SetFromPlayer ); };
	static int lSetToPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::SetToPlayer ); };
	static int lGetOtherPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetOtherPlayer ); };
	static int lGetToPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetToPlayer ); };
	static int lGetFromPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetFromPlayer ); };

	static int lGetSurrenderingPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetSurrenderingPlayer ); };
	static int lSetSurrenderingPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::SetSurrenderingPlayer ); };
	static int lGetDemandingPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetDemandingPlayer ); };
	static int lSetDemandingPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::SetDemandingPlayer ); };
	static int lGetRequestingPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetRequestingPlayer ); };
	static int lSetRequestingPlayer( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::SetRequestingPlayer ); };

	static int lGetGoldAvailable( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::GetGoldAvailable ); };

	static int lIsPossibleToTradeItem( lua_State *L );

	static int lAddGoldTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddGoldTrade ); };
	static int lAddGoldPerTurnTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddGoldPerTurnTrade ); };
	static int lAddMapTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddMapTrade ); };
	static int lAddResourceTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddResourceTrade ); };
	static int lAddCityTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddCityTrade ); };
	static int lAddUnitTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddUnitTrade ); };
	static int lAddOpenBorders( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddOpenBorders ); };
	static int lAddDefensivePact( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddDefensivePact ); };
	static int lAddResearchAgreement( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddResearchAgreement ); };
	static int lAddTradeAgreement( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddTradeAgreement ); };
	static int lAddPermamentAlliance( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddPermamentAlliance ); };
	static int lAddSurrender( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddSurrender ); };
	static int lAddTruce( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddTruce ); };
	static int lAddPeaceTreaty( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddPeaceTreaty ); };
	static int lAddThirdPartyPeace( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddThirdPartyPeace ); };
	static int lAddThirdPartyWar( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddThirdPartyWar ); };
	static int lAddThirdPartyEmbargo( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::AddThirdPartyEmbargo ); };

    static int lChangeGoldTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::ChangeGoldTrade ); };
    static int lChangeGoldPerTurnTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::ChangeGoldPerTurnTrade ); };
    static int lChangeResourceTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::ChangeResourceTrade ); };
    static int lChangeThirdPartyWarDuration( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::ChangeThirdPartyWarDuration ); };
    static int lChangeThirdPartyPeaceDuration( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::ChangeThirdPartyPeaceDuration ); };
    static int lChangeThirdPartyEmbargoDuration( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::ChangeThirdPartyEmbargoDuration ); };

	static int lRemoveByType( lua_State *L );
	static int lRemoveResourceTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::RemoveResourceTrade ); };
	static int lRemoveCityTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::RemoveCityTrade ); };
	static int lRemoveUnitTrade( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::RemoveUnitTrade ); };
	static int lRemoveThirdPartyPeace( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::RemoveThirdPartyPeace ); };
	static int lRemoveThirdPartyWar( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::RemoveThirdPartyWar ); };
	static int lRemoveThirdPartyEmbargo( lua_State *L ) { return BasicLuaMethod( L, &CvDeal::RemoveThirdPartyEmbargo ); };

    static TradedItemList::iterator m_iterator;
};

#endif //CVLUADEAL_H