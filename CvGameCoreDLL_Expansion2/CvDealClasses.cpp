/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvDiplomacyAI.h"
#include "CvMinorCivAI.h"
#if defined(MOD_BALANCE_CORE)
#include "CvDealAI.h"
#include "CvMilitaryAI.h"
#include "CvDiplomacyRequests.h"
#include "CvCitySpecializationAI.h"
#endif

// must be included after all other headers
#include "LintFree.h"

//=====================================
// TradeableItems
//====================================

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, TradeableItems& writeTo)
{
	int v;
	loadFrom >> v;
	writeTo = static_cast<TradeableItems>(v);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const TradeableItems& readFrom)
{
	saveTo << static_cast<int>(readFrom);
	return saveTo;
}

//====================================
// CvTradedItem
//====================================

/// Constructor
CvTradedItem::CvTradedItem()
{
	m_eItemType = TRADE_ITEM_NONE;
	m_iDuration = 0;
	m_iFinalTurn = 0;
	m_iData1 = 0;
	m_iData2 = 0;
	m_iData3 = 0;
	m_bFlag1 = false;
	m_eFromPlayer = NO_PLAYER;
	m_bFromRenewed = false;
	m_bToRenewed = false;
	m_iValue = INT_MAX;
	m_bValueIsEven = false;
}

/// Equals operator
bool CvTradedItem::operator==(const CvTradedItem& rhs) const
{
	return (m_eItemType == rhs.m_eItemType &&
	        m_iData1 == rhs.m_iData1 &&
	        m_iData2 == rhs.m_iData2 &&
			m_iData3 == rhs.m_iData3 &&
			m_bFlag1 == rhs.m_bFlag1 &&
	        m_eFromPlayer == rhs.m_eFromPlayer &&
	        m_iDuration == rhs.m_iDuration &&
	        m_iFinalTurn == rhs.m_iFinalTurn);
}

FDataStream& OldLoad(FDataStream& loadFrom, CvTradedItem& writeTo)
{
	loadFrom >> writeTo.m_eItemType;
	loadFrom >> writeTo.m_iDuration;
	loadFrom >> writeTo.m_iFinalTurn;
	loadFrom >> writeTo.m_iData1;
	loadFrom >> writeTo.m_iData2;
	loadFrom >> writeTo.m_eFromPlayer;
	writeTo.m_bFromRenewed = false;
	writeTo.m_bToRenewed = false;
	return loadFrom;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvTradedItem& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);
	loadFrom >> writeTo.m_eItemType;
	loadFrom >> writeTo.m_iDuration;
	loadFrom >> writeTo.m_iFinalTurn;
	loadFrom >> writeTo.m_iData1;
	loadFrom >> writeTo.m_iData2;
	if (uiVersion >= 2)
	{
		loadFrom >> writeTo.m_iData3;
		loadFrom >> writeTo.m_bFlag1;
	}
	else
	{
		writeTo.m_iData3 = 0;
		writeTo.m_bFlag1 = false;
	}
	loadFrom >> writeTo.m_eFromPlayer;
	loadFrom >> writeTo.m_bFromRenewed;
	loadFrom >> writeTo.m_bToRenewed;
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvTradedItem& readFrom)
{
	uint uiVersion = 2;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);
	saveTo << readFrom.m_eItemType;
	saveTo << readFrom.m_iDuration;
	saveTo << readFrom.m_iFinalTurn;
	saveTo << readFrom.m_iData1;
	saveTo << readFrom.m_iData2;
	saveTo << readFrom.m_iData3;
	saveTo << readFrom.m_bFlag1;
	saveTo << readFrom.m_eFromPlayer;
	saveTo << readFrom.m_bFromRenewed;
	saveTo << readFrom.m_bToRenewed;
	return saveTo;
}

//=====================================
// CvDeal
//=====================================

/// Constructor
CvDeal::CvDeal()
{
	m_eFromPlayer = NO_PLAYER;
	m_eToPlayer = NO_PLAYER;
	m_ePeaceTreatyType = NO_PEACE_TREATY_TYPE;
	m_eSurrenderingPlayer = NO_PLAYER;
	m_eDemandingPlayer = NO_PLAYER;
	m_eRequestingPlayer = NO_PLAYER;
	m_TradedItems.clear();
	m_iStartTurn = 0;
	m_bConsideringForRenewal = false;
	m_bCheckedForRenewal = false;
	m_bDealCancelled = false;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsGift = false;
#endif
}

/// Constructor with typical parameters
CvDeal::CvDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer)
{
	m_eFromPlayer = eFromPlayer;
	m_eToPlayer = eToPlayer;
	m_TradedItems.clear();
	m_bConsideringForRenewal = false;
	m_bCheckedForRenewal = false;
	m_bDealCancelled = false;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsGift = false;
#endif
}

/// Copy Constructor with typical parameters
CvDeal::CvDeal(const CvDeal& source)
{
	m_eFromPlayer = source.m_eFromPlayer;
	m_eToPlayer = source.m_eToPlayer;
	m_iDuration = source.m_iDuration;
	m_iFinalTurn = source.m_iFinalTurn;
	m_iStartTurn = source.m_iStartTurn;
	m_ePeaceTreatyType = source.m_ePeaceTreatyType;
	m_eSurrenderingPlayer = source.m_eSurrenderingPlayer;
	m_eDemandingPlayer = source.m_eDemandingPlayer;
	m_eRequestingPlayer = source.m_eRequestingPlayer;
	m_bConsideringForRenewal = source.m_bConsideringForRenewal;
	m_bCheckedForRenewal = source.m_bCheckedForRenewal;
	m_bDealCancelled = source.m_bDealCancelled;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsGift = source.m_bIsGift;
#endif
	m_TradedItems.clear();
	TradedItemList::const_iterator it;
	for(it = source.m_TradedItems.begin(); it != source.m_TradedItems.end(); ++it)
	{
		m_TradedItems.push_back(*it);
	}
}

/// Destructor
CvDeal::~CvDeal()
{
}

/// Overloaded assignment operator
CvDeal& CvDeal::operator=(const CvDeal& source)
{
	m_eFromPlayer = source.m_eFromPlayer;
	m_eToPlayer = source.m_eToPlayer;
	m_iDuration = source.m_iDuration;
	m_iFinalTurn = source.m_iFinalTurn;
	m_iStartTurn = source.m_iStartTurn;
	m_ePeaceTreatyType = source.m_ePeaceTreatyType;
	m_eSurrenderingPlayer = source.m_eSurrenderingPlayer;
	m_eDemandingPlayer = source.m_eDemandingPlayer;
	m_eRequestingPlayer = source.m_eRequestingPlayer;
	m_bConsideringForRenewal = source.m_bConsideringForRenewal;
	m_bCheckedForRenewal = source.m_bCheckedForRenewal;
	m_bDealCancelled = source.m_bDealCancelled;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsGift = source.m_bIsGift;
#endif
	m_TradedItems.clear();
	TradedItemList::const_iterator it;
	for(it = source.m_TradedItems.begin(); it != source.m_TradedItems.end(); ++it)
	{
		m_TradedItems.push_back(*it);
	}

	return (*this);
}

/// Burn it... burn it all...
void CvDeal::ClearItems()
{
	m_TradedItems.clear();

	m_iFinalTurn = -1;
	m_iDuration = -1;
	m_iStartTurn = -1;
	m_bConsideringForRenewal = false;
	m_bCheckedForRenewal = false;
	m_bDealCancelled = false;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsGift = false;
#endif

	SetPeaceTreatyType(NO_PEACE_TREATY_TYPE);
	SetSurrenderingPlayer(NO_PLAYER);
	SetDemandingPlayer(NO_PLAYER);
	SetRequestingPlayer(NO_PLAYER);
}

/// How many trade items are in this deal?
int CvDeal::GetNumItems()
{
	return m_TradedItems.size();
}

/// Who is player 1 in this deal (called from Lua because the interface has a static Deal that is initialized with both players as -1)
void CvDeal::SetFromPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_eFromPlayer = ePlayer;
}

/// Who is player 2 in this deal (called from Lua because the interface has a static Deal that is initialized with both players as -1)
void CvDeal::SetToPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_eToPlayer = ePlayer;
}

/// Helper function to figure out who the TO player is for a TradeableItem
PlayerTypes CvDeal::GetOtherPlayer(PlayerTypes eFromPlayer) const
{
	CvAssertMsg(eFromPlayer >= 0, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFromPlayer < MAX_CIV_PLAYERS, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(m_eFromPlayer != eFromPlayer)
	{
		return m_eFromPlayer;
	}
	else
	{
		return m_eToPlayer;
	}
}

/// How much Gold does ePlayer have available to be used in this Deal?
int CvDeal::GetGoldAvailable(PlayerTypes ePlayer, TradeableItems eItemToBeChanged)
{
	int iGoldAvailable = GET_PLAYER(ePlayer).GetTreasury()->GetGold();

	// Remove Gold we're sending to the other player in this deal (unless we're changing it)
	if(eItemToBeChanged != TRADE_ITEM_GOLD)
	{
		iGoldAvailable -= GetGoldTrade(ePlayer);
	}

	int iGoldCost;

	// Loop through all trade items to see if they have a cost
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		// Don't count something against itself when trying to add it
		if(it->m_eItemType != eItemToBeChanged)
		{
			if(it->m_eFromPlayer == ePlayer)
			{
				iGoldCost = GC.getGame().GetGameDeals().GetTradeItemGoldCost(it->m_eItemType, m_eFromPlayer, m_eToPlayer);

				if(iGoldCost != 0)		// Negative cost valid?  Maybe ;-O
				{
					iGoldAvailable -= iGoldCost;
				}
			}
		}
	}

	return iGoldAvailable;
}

/// Is it actually possible for a player to offer up this trade item?
bool CvDeal::IsPossibleToTradeItem(PlayerTypes ePlayer, PlayerTypes eToPlayer, TradeableItems eItem, int iData1, int iData2, int iData3, bool bFlag1, bool bCheckOtherPlayerValidity, bool bFinalizing)
{
	//failsafe
	if (ePlayer == NO_PLAYER || eToPlayer == NO_PLAYER)
		return false;

	// The Data parameters can be -1, which means we don't care about whatever data is stored there (e.g. -1 for Gold means can we trade ANY amount of Gold?)
	CvPlayer* pFromPlayer = &GET_PLAYER(ePlayer);
	CvPlayer* pToPlayer = &GET_PLAYER(eToPlayer);

	TeamTypes eFromTeam = pFromPlayer->getTeam();
	TeamTypes eToTeam = pToPlayer->getTeam();

	CvTeam* pFromTeam = &GET_TEAM(eFromTeam);
	CvTeam* pToTeam = &GET_TEAM(eToTeam);

#if !defined(MOD_BALANCE_CORE) 
	CvDeal* pRenewDeal = pFromPlayer->GetDiplomacyAI()->GetDealToRenew(NULL, eToPlayer);
	if (!pRenewDeal)
	{
		pRenewDeal = pToPlayer->GetDiplomacyAI()->GetDealToRenew(NULL, ePlayer);
	}
#endif
#if defined(MOD_BALANCE_CORE)
	bool bHumanToHuman = false;
	if(pFromPlayer->isHuman() && pToPlayer->isHuman())
	{
		bHumanToHuman = true;
	}
#endif

	if (this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE && eItem != TRADE_ITEM_PEACE_TREATY && !ContainsItemType(TRADE_ITEM_PEACE_TREATY))
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague != NULL && pLeague->IsTradeEmbargoed(ePlayer, eToPlayer))
		{
			return false;
		}
	}


	int iGoldAvailable = GetGoldAvailable(ePlayer, eItem);

	// Some items require gold be spent (e.g. Research and Trade Agreements)
	int iCost = GC.getGame().GetGameDeals().GetTradeItemGoldCost(eItem, ePlayer, eToPlayer);
	if(iCost > 0 && iGoldAvailable < iCost)
		return false;

	iGoldAvailable -= iCost;

	////////////////////////////////////////////////////

	// Gold
	if(eItem == TRADE_ITEM_GOLD)
	{
		// DoF has not been made with this player
#if defined(MOD_BALANCE_CORE)
		if (!this->IsPeaceTreatyTrade(eToPlayer) && !this->IsPeaceTreatyTrade(ePlayer) && this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
#else
		if (!this->IsPeaceTreatyTrade(eToPlayer) && !this->IsPeaceTreatyTrade(ePlayer))
#endif
		{
			if (pFromPlayer->getTeam() != pToPlayer->getTeam() && (!pFromPlayer->GetDiplomacyAI()->IsDoFAccepted(eToPlayer) || !pToPlayer->GetDiplomacyAI()->IsDoFAccepted(ePlayer)))
				return false;
		}

		// Can't trade more Gold than you have
		int iGold = iData1;
		if(iGold != -1 && iGoldAvailable < iGold)
			return false;

#if defined(MOD_BALANCE_CORE)
		if (!this->IsPeaceTreatyTrade(eToPlayer) && !this->IsPeaceTreatyTrade(ePlayer) && this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
		{
			// Can't exchange GPT for lump Gold - we aren't a bank.
			if(GetGoldPerTurnTrade(eToPlayer) > 0)
				return false;
		}
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
	// Gold per Turn
	else if(eItem == TRADE_ITEM_GOLD_PER_TURN)
	{
		// Can't trade more GPT than you're making
		int iGoldPerTurn = iData1;
		if(iGoldPerTurn != -1 && pFromPlayer->calculateGoldRate() < iGoldPerTurn)
			return false;

#if defined(MOD_BALANCE_CORE)
		if (!this->IsPeaceTreatyTrade(eToPlayer) && !this->IsPeaceTreatyTrade(ePlayer) && this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
		{
			// Can't exchange GPT for lump Gold - we aren't a bank.
			if(GetGoldTrade(eToPlayer) > 0)
				return false;
		}
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif

		//int iDuration = iData2;
		//if (iDuration != GC.getGame().GetDealDuration())
		//	return false;
	}
	// Map
	else if(eItem == TRADE_ITEM_MAPS)
	{
		if(!MOD_DIPLOMACY_CIV4_FEATURES)
			return false;

		// Both need tech for Map trading
		if (!pToTeam->isMapTrading() || !pFromTeam->isMapTrading())
			return false;

		CvPlot* pPlot;
		// Look at every tile on map
		bool unrevealed = false;
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			pPlot = GC.getMap().plotByIndexUnchecked(iI);
			if (pPlot && !pPlot->isRevealed(GET_PLAYER(eToPlayer).getTeam()))
			{
				unrevealed = true; 
				break;
			}
		}
		if (!unrevealed)
			return false;
	}
	// Resource
	else if(eItem == TRADE_ITEM_RESOURCES)
	{
		ResourceTypes eResource = (ResourceTypes) iData1;
		if(eResource != NO_RESOURCE)
		{
			int iResourceQuantity = iData2;

			// Can't trade a negative amount of something!
			if(iResourceQuantity < 0)
				return false;

			if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(ePlayer, eResource) || GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(eToPlayer, eResource))
			{
				return false;
			}

			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if (pkResourceInfo)
			{
				TechTypes eTech = (TechTypes)pkResourceInfo->getTechObsolete();
				if (eTech != NO_TECH && (GET_PLAYER(ePlayer).HasTech(eTech) || GET_PLAYER(eToPlayer).HasTech(eTech)))
					return false;
			}

			//int iNumAvailable = GetNumResource(ePlayer, eResource, true);

			int iNumAvailable = pFromPlayer->getNumResourceAvailable(eResource, false);
#if !defined(MOD_BALANCE_CORE) 
			int iNumInExistingDeal = 0;
			int iNumInRenewDeal = 0;
			if (pRenewDeal)
			{
				// count any that are in the renew deal
				TradedItemList::iterator it;
				for(it = pRenewDeal->m_TradedItems.begin(); it != pRenewDeal->m_TradedItems.end(); ++it)
				{
					if(it->m_eItemType == TRADE_ITEM_RESOURCES && it->m_eFromPlayer == ePlayer && (ResourceTypes)it->m_iData1 == eResource)
					{
						// credit the amount
						iNumInRenewDeal += it->m_iData2;
					}
				}

				// remove any that are in this deal
				for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
				{
					if(it->m_eItemType == TRADE_ITEM_RESOURCES && it->m_eFromPlayer == ePlayer && (ResourceTypes)it->m_iData1 == eResource)
					{
						iNumInExistingDeal += it->m_iData2;
					}
				}
				if(iNumInRenewDeal > 0)
				{
					// Offering up more of a Resource than we have available
					if(iNumAvailable + iNumInRenewDeal - iNumInExistingDeal < 0)
						return false;
				}
				else
				{
					// Offering up more of a Resource than we have available
					if(iNumAvailable + iNumInRenewDeal - iNumInExistingDeal < iResourceQuantity)
						return false;
				}
			}
			else
#endif
			{
				// Offering up more of a Resource than we have available
				if(iNumAvailable < iResourceQuantity)
					return false;
			}
			
			// Must be a Luxury or a Strategic Resource
			ResourceUsageTypes eUsage = GC.getResourceInfo(eResource)->getResourceUsage();
			if(eUsage != RESOURCEUSAGE_LUXURY && eUsage != RESOURCEUSAGE_STRATEGIC)
				return false;

			if(eUsage == RESOURCEUSAGE_LUXURY)
			{
#if !defined(MOD_BALANCE_CORE) 
				if (pRenewDeal)
				{
					// Can't trade Luxury if the other player already has one
					if((pToPlayer->getNumResourceAvailable(eResource) - iNumInRenewDeal) > 0)
					{
						return false;
					}
				}
				else
#endif
				{
					// Can't trade Luxury if the other player already has one
					if(pToPlayer->getNumResourceAvailable(eResource) > 0 && !pToPlayer->GetPlayerTraits()->IsImportsCountTowardsMonopolies())
					{
						return false;
					}
				}
			}

			// Can't trade them something they're already giving us in the deal
			if(!bFinalizing && IsResourceTrade(eToPlayer, eResource))
				return false;

			// AI can't trade an obsolete resource
			if (!pFromTeam->isHuman() && pFromTeam->IsResourceObsolete(eResource))
			{
				return false;
			}
#if defined(MOD_BALANCE_CORE)
			if(!bHumanToHuman)
			{
				if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
				{
					if(this->GetSurrenderingPlayer() != ePlayer)
					{
						return false;
					}
				}
			}
#endif
			// Can't trade resource if the seller does not have the city trade tech
			TechTypes eCityTradeTech = (TechTypes)GC.getResourceInfo(eResource)->getTechCityTrade();
			if (eCityTradeTech != NO_TECH)
			{
				if (!pFromPlayer->HasTech(eCityTradeTech))
				{
					return false;
				}
			}
		}
	}
	// City
	else if(eItem == TRADE_ITEM_CITIES)
	{
		CvCity* pCity = NULL;
		CvPlot* pPlot = GC.getMap().plot(iData1, iData2);
		if(pPlot != NULL)
			pCity = pPlot->getPlotCity();

		if(pCity != NULL)
		{
			// Can't trade someone else's city
			if(pCity->getOwner() != ePlayer)
				return false;

			// Can't trade one's capital
			if(pCity->isCapital())
				return false;

			if (pCity->getDamage() > 0 && !pFromTeam->isAtWar(pToTeam->GetID()))
				return false;

			//do not trade away our closest city in the same deal!
			CvCity* pClosestCity = GET_PLAYER(ePlayer).GetClosestCityByPlots(pCity->plot());
			if (pClosestCity != NULL && IsCityInDeal(pClosestCity->getOwner(), pClosestCity->GetID()))
				return false;

			// Can't trade a city to a human in an OCC game
			if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && GET_PLAYER(eToPlayer).isHuman())
				return false;

#if defined(MOD_BALANCE_CORE)
			if (!this->IsPeaceTreatyTrade(eToPlayer) && !this->IsPeaceTreatyTrade(ePlayer) && this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
			{
				if (pFromTeam != pToTeam && !pToTeam->HasEmbassyAtTeam(eFromTeam))
					return false;
			}
#endif
		}
		// Can't trade a null city
		else
			return false;

		// Can't already have this city in the deal
		if(!bFinalizing && IsCityTrade(ePlayer, iData1, iData2))
			return false;

#if defined(MOD_BALANCE_CORE)
		//Can't in a peace deal if not surrendering.
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
	// Unit
	else if(eItem == TRADE_ITEM_UNITS)
	{
		return false;
	}
	// Embassy
	else if(eItem == TRADE_ITEM_ALLOW_EMBASSY)
	{
		// too few cities
		if (pToPlayer->getNumCities() < 1)
			return false;
		// Does not have tech for Embassy trading
		if(!pToTeam->isAllowEmbassyTradingAllowed())
			return false;
		// Already has embassy
		if(pToTeam->HasEmbassyAtTeam(eFromTeam))
			return false;
		// Same team
		if(eFromTeam == eToTeam)
			return false;

#if defined(MOD_BALANCE_CORE)
		//Can't if at war.
		if(pFromTeam->isAtWar(eToTeam))
			return false;

		//Can't in a peace deal.
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
	// Open Borders
	else if(eItem == TRADE_ITEM_OPEN_BORDERS)
	{
		// Neither of us yet has the Tech for OP
		if(!pFromTeam->isOpenBordersTradingAllowed() && !pToTeam->isOpenBordersTradingAllowed())
			return false;
		// Embassy has not been established
		if(!pFromTeam->HasEmbassyAtTeam(eToTeam))
			return false;
		
		bool bIgnoreExistingOP = true;
#if !defined(MOD_BALANCE_CORE)
		if (pRenewDeal)
		{
			// count any that are in the renew deal
			int iEndingTurn = -1;
			TradedItemList::iterator it;
			for(it = pRenewDeal->m_TradedItems.begin(); it != pRenewDeal->m_TradedItems.end(); ++it)
			{
				if(it->m_eItemType == TRADE_ITEM_OPEN_BORDERS && (it->m_eFromPlayer == ePlayer || it->m_eFromPlayer == eToPlayer == ePlayer))
				{
					iEndingTurn = it->m_iFinalTurn;
				}
			}

			if (iEndingTurn == GC.getGame().getGameTurn())
			{
				bIgnoreExistingOP = false;
			}
		}
#endif

		// Already has OB
		if(pFromTeam->IsAllowsOpenBordersToTeam(eToTeam) && bIgnoreExistingOP)
			return false;

		// Same Team
		if(eFromTeam == eToTeam)
			return false;

#if defined(MOD_BALANCE_CORE)
		//Can't if at war.
		if(pFromTeam->isAtWar(eToTeam))
			return false;

		//Can't be part of a peace deal.
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
	// Defensive Pact
	else if(eItem == TRADE_ITEM_DEFENSIVE_PACT)
	{
		// Neither of us yet has the Tech for DP
		if(!pFromTeam->isDefensivePactTradingAllowed() && !pToTeam->isDefensivePactTradingAllowed())
			return false;
		// Embassy has not been established
		if(!pFromTeam->HasEmbassyAtTeam(eToTeam) || !pToTeam->HasEmbassyAtTeam(eFromTeam))
			return false;
		// Already has DP
		if(pFromTeam->IsHasDefensivePact(eToTeam))
			return false;
		// Same Team
		if(eFromTeam == eToTeam)
			return false;
#if defined(MOD_BALANCE_CORE)
		//Can't if at war.
		if(pFromTeam->isAtWar(eToTeam))
			return false;
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		// If we're a vassal, can't
		if(pFromTeam->IsVassalOfSomeone())
			return false;
#endif

		// Check to see if the other player can trade this item to us as well.  If we can't, we can't trade it either
		if(bCheckOtherPlayerValidity)
		{
			if(!IsPossibleToTradeItem(eToPlayer, ePlayer, eItem, iData1, iData2, iData3, bFlag1, /*bCheckOtherPlayerValidity*/ false))
				return false;
		}
	}
	// Research Agreement
	else if(eItem == TRADE_ITEM_RESEARCH_AGREEMENT)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
			return false;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		// Research agreements aren't enabled
		if(MOD_DIPLOMACY_CIV4_FEATURES && !GC.getGame().isOption(GAMEOPTION_RESEARCH_AGREEMENTS))
			return false;
#endif

		// Neither of us yet has the Tech for RA
		if(!pFromTeam->IsResearchAgreementTradingAllowed() && !pToTeam->IsResearchAgreementTradingAllowed())
			return false;
		// Embassy has not been established with this team
		if(!pFromTeam->HasEmbassyAtTeam(eToTeam) || !pToTeam->HasEmbassyAtTeam(eFromTeam))
			return false;
		// DoF has not been made with this player
		if(!pFromPlayer->GetDiplomacyAI()->IsDoFAccepted(eToPlayer) || !pToPlayer->GetDiplomacyAI()->IsDoFAccepted(ePlayer))
			return false;
		// Already has RA
		if(pFromTeam->IsHasResearchAgreement(eToTeam))
			return false;
		// Same Team
		if(eFromTeam == eToTeam)
			return false;
		// Someone already has all techs
		if(pFromTeam->GetTeamTechs()->HasResearchedAllTechs() || pToTeam->GetTeamTechs()->HasResearchedAllTechs())
			return false;

		// Check to see if the other player can trade this item to us as well.  If we can't, we can't trade it either
		if(bCheckOtherPlayerValidity)
		{
			if(!IsPossibleToTradeItem(eToPlayer, ePlayer, eItem, iData1, iData2, iData3, bFlag1, /*bCheckOtherPlayerValidity*/ false))
				return false;
		}
#if defined(MOD_BALANCE_CORE)
		//Not allowed in peace deals.
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
	// Trade Agreement
	else if(eItem == TRADE_ITEM_TRADE_AGREEMENT)
	{
		// Neither of us yet has the Tech for TA
		if(!pFromTeam->IsTradeAgreementTradingAllowed() && !pToTeam->IsTradeAgreementTradingAllowed())
			return false;
		// Already has TA
		if(pFromTeam->IsHasTradeAgreement(eToTeam))
			return false;
		// Same Team
		if(eFromTeam == eToTeam)
			return false;

		// Check to see if the other player can trade this item to us as well.  If we can't, we can't trade it either
		if(bCheckOtherPlayerValidity)
		{
			if(!IsPossibleToTradeItem(eToPlayer, ePlayer, eItem, iData1, iData2, iData3, bFlag1, /*bCheckOtherPlayerValidity*/ false))
				return false;
		}
	}
	// Permanent Alliance
	else if(eItem == TRADE_ITEM_PERMANENT_ALLIANCE)
		return false;
	// Surrender
	else if(eItem == TRADE_ITEM_SURRENDER)
		return false;
	// Truce
	else if(eItem == TRADE_ITEM_TRUCE)
		return false;
	// Peace Treaty
	else if(eItem == TRADE_ITEM_PEACE_TREATY)
	{
		if(!pFromTeam->isAtWar(eToTeam))
			return false;

		if(!pToTeam->isAtWar(eFromTeam))
			return false;
		
		if (!GET_PLAYER(eToPlayer).isHuman() && !GET_PLAYER(eToPlayer).GetDiplomacyAI()->IsWantsPeaceWithPlayer(ePlayer))
			return false;
		
		if (!GET_PLAYER(ePlayer).isHuman() && !GET_PLAYER(ePlayer).GetDiplomacyAI()->IsWantsPeaceWithPlayer(eToPlayer))
			return false;
			
#if defined(MOD_EVENTS_WAR_AND_PEACE)
		if (MOD_EVENTS_WAR_AND_PEACE) {
			if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_IsAbleToMakePeace, ePlayer, eToTeam) == GAMEEVENTRETURN_FALSE) {
				return false;
			}

			if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanMakePeace, ePlayer, eToTeam) == GAMEEVENTRETURN_FALSE) {
				return false;
			}
		} else {
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem)
		{
			// Construct and push in some event arguments.
			CvLuaArgsHandle args;
			args->Push(ePlayer);
			args->Push(eToTeam);

			// Attempt to execute the game events.
			// Will return false if there are no registered listeners.
			bool bResult = false;
			if (LuaSupport::CallTestAll(pkScriptSystem, "IsAbleToMakePeace", args.get(), bResult)) 
			{
				// Check the result.
				if (bResult == false)
				{
					return false;
				}
			}
		}
#if defined(MOD_EVENTS_WAR_AND_PEACE)
		}
#endif
	}
	// Third Party Peace
	else if(eItem == TRADE_ITEM_THIRD_PARTY_PEACE)
	{
		TeamTypes eThirdTeam = (TeamTypes) iData1;
#if defined(MOD_BALANCE_CORE)
		if(eThirdTeam == NO_TEAM)
			return false;

		//If not at war, need embassy.
		if (!this->IsPeaceTreatyTrade(eToPlayer) && !this->IsPeaceTreatyTrade(ePlayer) && this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
		{
			if (!GET_TEAM(eToTeam).HasEmbassyAtTeam(eFromTeam))
				return false;

			//vassals get out!
			if (GET_TEAM(eToTeam).IsVassalOfSomeone() || GET_TEAM(eThirdTeam).IsVassalOfSomeone() || GET_TEAM(eFromTeam).IsVassalOfSomeone())
				return false;
		
			//Can't already be offering this.
			if (!bFinalizing && IsThirdPartyPeaceTrade( ePlayer, eThirdTeam))
				return false;
		}

#endif
		// Can't be the same team
		if(eFromTeam == eThirdTeam)
			return false;

		// Can't ask teammates
		if(eToTeam == eFromTeam)
			return false;

		//Can't ask for yourself
		if(eToTeam == eThirdTeam)
			return false;

		// Must be alive
		if(!GET_TEAM(eThirdTeam).isAlive())
			return false;

		// Player that wants Peace hasn't yet met the 3rd Team
		if(!pToTeam->isHasMet(eThirdTeam))
			return false;

		// Player that would go to Peace hasn't yet met the 3rd Team
		if(!pFromTeam->isHasMet(eThirdTeam))
			return false;

		// Player that would go to peace is already at peace with the 3rd Team
		if(!pFromTeam->isAtWar(eThirdTeam))
			return false;

		//Either side can't make peace yet?
		if(!pFromTeam->canChangeWarPeace(eThirdTeam))
			return false;

		//Either side can't make peace yet?
		if(!GET_TEAM(eThirdTeam).canChangeWarPeace(eFromTeam))
			return false;

#if defined(MOD_BALANCE_CORE)
		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eThirdTeam)
			{
				CvPlayer* pOtherPlayer = &GET_PLAYER(eLoopPlayer);

				if(!pOtherPlayer)
					continue;

				if(!pOtherPlayer->isAlive())
					return false;

				// Minor civ
				if(pOtherPlayer->isMinorCiv())
				{
					// Minor at permanent war with this player
					if(pOtherPlayer->GetMinorCivAI()->IsPermanentWar(eFromTeam))
						return false;

					// Only matters if this isn't a peace treaty.
					if (!this->IsPeaceTreatyTrade(eToPlayer) && !this->IsPeaceTreatyTrade(ePlayer) && this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
					{
						// Minor's ally at war with this player?
						if(pOtherPlayer->GetMinorCivAI()->IsPeaceBlocked(eFromTeam))
						{
							// If the ally is us, don't block peace here
							if(pOtherPlayer->GetMinorCivAI()->GetAlly() != eToPlayer)
								return false;
						}
					}
					//It is a peace treaty? We only want allied CSs in here.
					else if (this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer) || this->GetPeaceTreatyType() != NO_PEACE_TREATY_TYPE)
					{
						if(pOtherPlayer->GetMinorCivAI()->GetAlly() != eToPlayer)
							return false;
					}
				}

				// Major civ
				else
				{				
					//Only matters if not a peace deal (i.e. we're not making negotiations)
					if (!this->IsPeaceTreatyTrade(eToPlayer) && !this->IsPeaceTreatyTrade(ePlayer) && this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
					{
						if(pFromPlayer->GetDiplomacyAI()->GetPlayerNumTurnsAtWar(eLoopPlayer) < GD_INT_GET(WAR_MAJOR_MINIMUM_TURNS))
						{
							return false;
						}
						if(pOtherPlayer->GetDiplomacyAI()->GetPlayerNumTurnsAtWar(ePlayer) < GD_INT_GET(WAR_MAJOR_MINIMUM_TURNS))
						{
							return false;
						}

						//Either side can't make peace yet because of city capture?
						if(pOtherPlayer->GetDiplomacyAI()->GetPlayerNumTurnsSinceCityCapture(ePlayer) <= 1)
							return false;

						if(pFromPlayer->GetDiplomacyAI()->GetPlayerNumTurnsSinceCityCapture(eLoopPlayer) <= 1)
							return false;

						//Can't force third party peace with a loser. Has to be a sizeable difference
						int iFromWarScore = pFromPlayer->GetDiplomacyAI()->GetWarScore(pOtherPlayer->GetID());

						if(iFromWarScore < 75)
							return false;
					}
					else
					{
						//Can't force third party peace with a loser. Has to be a sizeable difference
						int iFromWarScore = pFromPlayer->GetDiplomacyAI()->GetWarScore(pOtherPlayer->GetID());

						if(iFromWarScore < 75)
							return false;
					}
				}
			}
		}
#else
		CvPlayer* pOtherPlayer = &GET_PLAYER(GET_TEAM(eThirdTeam).getLeaderID());

		// Minor civ
		if(pOtherPlayer->isMinorCiv())
		{
			// Minor at permanent war with this player
			if(pOtherPlayer->GetMinorCivAI()->IsPermanentWar(eFromTeam))
				return false;

			// Minor's ally at war with this player?
			else if(pOtherPlayer->GetMinorCivAI()->IsPeaceBlocked(eFromTeam))
			{
				// If the ally is us, don't block peace here
				if(pOtherPlayer->GetMinorCivAI()->GetAlly() != eToPlayer)
					return false;
			}
		}
		// Major civ
		else
		{
			// Can't ask them to make peace with a human, because we have no way of knowing if the human wants peace
			if(pOtherPlayer->isHuman())
				return false;

			// Player does not want peace with eOtherPlayer
			if(pFromPlayer->isHuman() || pFromPlayer->GetDiplomacyAI()->GetWarGoal(pOtherPlayer->GetID()) < WAR_GOAL_DAMAGE)
				return false;

			// Other player does not want peace with eToPlayer
			if(!pOtherPlayer->GetDiplomacyAI()->IsWantsPeaceWithPlayer(ePlayer))
				return false;
		}
#endif
	}
	// Third Party War
	else if(eItem == TRADE_ITEM_THIRD_PARTY_WAR)
	{
		TeamTypes eThirdTeam = (TeamTypes) iData1;
#if defined(MOD_BALANCE_CORE)
		if(eThirdTeam == NO_TEAM)
			return false;

		// Can't be the same team
		if(eFromTeam == eThirdTeam)
			return false;

		//Not allowed in peace deals.
		if (this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer) || this->GetPeaceTreatyType() != NO_PEACE_TREATY_TYPE)
			return false;

		//vassals get out!
		if (GET_TEAM(eToTeam).IsVassalOfSomeone() || GET_TEAM(eThirdTeam).IsVassalOfSomeone() || GET_TEAM(eFromTeam).IsVassalOfSomeone())
			return false;

		//If not at war, need embassy.
		if (!this->IsPeaceTreatyTrade(eToPlayer) && !this->IsPeaceTreatyTrade(ePlayer) && this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
		{
			if (!GET_TEAM(eToTeam).HasEmbassyAtTeam(eFromTeam))
				return false;

			if (!GET_TEAM(eFromTeam).canDeclareWar(eThirdTeam))
				return false;

			//Can't already be offering this.
			if (!bFinalizing && IsThirdPartyWarTrade(ePlayer, eThirdTeam))
				return false;
		}

		// Can't ask teammates
		if(eToTeam == eFromTeam)
			return false;

		//Can't ask for yourself
		if(eToTeam == eThirdTeam)
			return false;

		// Must be alive
		if(!GET_TEAM(eThirdTeam).isAlive())
			return false;

		// Player that would go to war hasn't yet met the 3rd Team
		if(!pToTeam->isHasMet(eThirdTeam))
			return false;

		// Player that wants war not met this team
		if(!pFromTeam->isHasMet(eThirdTeam))
			return false;

		// Player that would go to war is already at war with the 3rd Team
		if(pFromTeam->isAtWar(eThirdTeam))
			return false;

		// Can't ask DPs to DOW on each other
		if(GET_TEAM(eThirdTeam).IsHasDefensivePact(eFromTeam))
			return false;

		if(GET_TEAM(eThirdTeam).IsHasDefensivePact(eToTeam))
			return false;

		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}

		// Can this player actually declare war?
#if defined(MOD_EVENTS_WAR_AND_PEACE)
		if(!pFromTeam->canDeclareWar(eThirdTeam, ePlayer))
#else
		if(!pFromTeam->canDeclareWar(eThirdTeam))
#endif
			return false;

		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eThirdTeam)
			{
				CvPlayer* pOtherPlayer = &GET_PLAYER(eLoopPlayer);
				
				if(!pOtherPlayer)
					return false;

				if(!pOtherPlayer->isAlive())
					return false;

				// Major civ
				if(pOtherPlayer->isMajorCiv())
				{
					// Can't ask friends to backstab each other
					if(pOtherPlayer->GetDiplomacyAI()->IsDoFAccepted(ePlayer))
						return false;

					// Can't ask friends to backstab each other
					if(pOtherPlayer->GetDiplomacyAI()->IsDoFAccepted(eToPlayer))
						return false;			
				}
				else
				{
					// Can't ask a player to declare war on their ally
					if(pOtherPlayer->GetMinorCivAI()->GetAlly() == ePlayer)
						return false;
					//Can't offer an attack like that either
					if(pOtherPlayer->GetMinorCivAI()->GetAlly() == eToPlayer)
						return false;
				}
			}
		}	
#else
		// Can't be the same team
		if(eFromTeam == eThirdTeam)
			return false;

		// Can't ask teammates
		if(eToTeam == eFromTeam)
			return false;

		// Must be alive
		if(!GET_TEAM(eThirdTeam).isAlive())
			return false;

		// Player that would go to war hasn't yet met the 3rd Team
		if(!pToTeam->isHasMet(eThirdTeam))
			return false;
		// Player that wants war not met this team
		if(!pFromTeam->isHasMet(eThirdTeam))
			return false;

		// Player that would go to war is already at war with the 3rd Team
		if(pFromTeam->isAtWar(eThirdTeam))
			return false;

		// Can this player actually declare war?
		if(!pFromTeam->canDeclareWar(eThirdTeam))
			return false;

		// Can't already have this in the deal
		//if (IsThirdPartyWarTrade( ePlayer, GET_TEAM(eThirdTeam).getLeaderID() ))
		//	return false;

		// Can't ask a player to declare war on their ally
		if(GET_TEAM(eThirdTeam).isMinorCiv())
		{
			if(GET_PLAYER(GET_TEAM(eThirdTeam).getLeaderID()).GetMinorCivAI()->GetAlly() == ePlayer)
				return false;
		}
#endif
	}
	// Third Party Embargo
	else if(eItem == TRADE_ITEM_THIRD_PARTY_EMBARGO)
	{
		return false;
	}
	// Declaration of friendship
	else if(eItem == TRADE_ITEM_DECLARATION_OF_FRIENDSHIP)
	{
		// If we are at war, then we can't until we make peace
		if(pFromTeam->isAtWar(eToTeam))
			return false;

		// Already have a DoF?
		if (pFromPlayer->GetDiplomacyAI()->IsDoFAccepted(eToPlayer) && pToPlayer->GetDiplomacyAI()->IsDoFAccepted(ePlayer))
			return false;
	}
	// Promise to Vote in upcoming league session
	else if (eItem == TRADE_ITEM_VOTE_COMMITMENT)
	{
		// If we are at war, then we can't until we make peace
		if(pFromTeam->isAtWar(eToTeam))
			return false;

		int iID = iData1;
		//antonjs: todo: verify iChoice is valid as well:
		//int iChoice = iData2;
		int iNumVotes = iData3;
		bool bRepeal = bFlag1;

		DEBUG_VARIABLE(iNumVotes);

		if(GC.getGame().GetGameLeagues()->GetNumActiveLeagues() == 0)
			return false;

		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if(pLeague == NULL)
			return false;

		CvAssert(pLeague->IsProposed(iID, bRepeal));
#if defined(MOD_BALANCE_CORE_DEALS)
		CvAssert(iNumVotes <= pLeague->GetPotentialVotesForMember(ePlayer, eToPlayer));
#else
		CvAssert(iNumVotes <= pLeague->GetCoreVotesForMember(ePlayer));
#endif
		
		// Can't already have a vote commitment in the deal
		if(!bFinalizing && IsVoteCommitmentTrade(ePlayer))
			return false;

		// Must be a valid proposal
		if(!pLeague->IsProposed(iID, bRepeal))
			return false;

		// This player must be allowed to
		if(!bFinalizing && !pFromPlayer->GetLeagueAI()->CanCommitVote(eToPlayer))
			return false;
#if defined(MOD_BALANCE_CORE)
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// Maps
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_MAPS)
	{
		// We don't have the tech for Map Trading yet
		if(!pFromTeam->isMapTrading())
			return false;
		// We don't have an embassy established
		if(!pFromTeam->HasEmbassyAtTeam(eToTeam))
			return false;
		// Same team
		if(eFromTeam == eToTeam)
			return false;

		//Can't already be offering this
		if (!bFinalizing && IsMapTrade( ePlayer))
			return false;
#if defined(MOD_BALANCE_CORE)
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
	// Techs
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_TECHS)
	{
		// Do we have no science enabled?
		if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
			return false;

		// Can't trade techs if they're disabled
		if(GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING))
			return false;

		// Same team
		if(eFromTeam == eToTeam)
			return false;

		// We don't have the tech for Technology Trading yet
		if(!pFromTeam->isTechTrading())
			return false;

		// We don't have an embassy established
		if (!GET_TEAM(eToTeam).HasEmbassyAtTeam(eFromTeam) || !GET_TEAM(eFromTeam).HasEmbassyAtTeam(eToTeam))
			return false;

		// We don't own this tech
		if(!pFromTeam->GetTeamTechs()->HasTech((TechTypes) iData1))
			return false;

		// We are researching this tech
		if(GET_PLAYER(ePlayer).GetPlayerTechs()->GetCurrentResearch() == ((TechTypes) iData1))
			return false;

		// They can't research this tech yet
		if(!GET_PLAYER(eToPlayer).GetPlayerTechs()->CanResearch((TechTypes) iData1, false))
			return false;

		// This tech is repeatable, we can't sell it.
		CvTechEntry* pkTechInfo = GC.getTechInfo((TechTypes) iData1);
		if(pkTechInfo->IsRepeat())
			return false;

		// Tech Brokering is enabled, and we didn't research that tech
		if(GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING) && !pFromTeam->IsTradeTech((TechTypes) iData1))
			return false;
		
		//Can't already be offering this
		if (!bFinalizing && IsTechTrade( ePlayer, (TechTypes) iData1))
			return false;
#if defined(MOD_BALANCE_CORE)
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_VASSALAGE)
	{
		// Vassalage is disabled...
		if(GC.getGame().isOption(GAMEOPTION_NO_VASSALAGE))
			return false;

		// Same Team
		if(eFromTeam == eToTeam)
			return false;

		// This prevents AI teammates selling capitulation in peace deals
		if(!pFromPlayer->isHuman() && pFromPlayer->IsAITeammateOfHuman())
			return false;

		// Can we become the vassal of eToTeam?
		if(!pFromTeam->canBecomeVassal(eToTeam))
			return false;

		//Does the offering team have a vassal?
		if(pFromTeam->GetNumVassals() > 0)
			return false;

		//If the other player is our master, or vice versa...
		if(GET_TEAM(pToPlayer->getTeam()).IsVassal(pFromPlayer->getTeam()))
		{
			return false;
		}

		if(GET_TEAM(pFromPlayer->getTeam()).IsVassal(pToPlayer->getTeam()))
		{
			return false;
		}

		//Can't already be offering this
		if (!bFinalizing && IsVassalageTrade( ePlayer))
			return false;

		//Can't already be offering this
		if (!bFinalizing && IsRevokeVassalageTrade( ePlayer))
			return false;
#if defined(MOD_BALANCE_CORE)
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_VASSALAGE_REVOKE)
	{
		// Vassalage is disabled...
		if(GC.getGame().isOption(GAMEOPTION_NO_VASSALAGE))
			return false;

		// Same Team
		if(eFromTeam == eToTeam)
			return false;

		// This prevents AI teammates selling capitulation in peace deals
		if(!pFromPlayer->isHuman() && pFromPlayer->IsAITeammateOfHuman())
			return false;

		//If the other player has no vassals...
		if(GET_TEAM(pFromPlayer->getTeam()).GetNumVassals() <= 0)
		{
			return false;
		}

		//Cannot ask him to do so.
		if (pFromPlayer->IsVassalsNoRebel())
			return false;

		// If either team is a vassal of the other, we cannot end vassals
		if(pToTeam->IsVassal(eFromTeam) || pFromTeam->IsVassal(eToTeam))
		{
			return false;
		}

		// Must be able to end all vassals
		if (!pFromTeam->canEndAllVassal())
			return false;

		//Can't already be offering this
		if (!bFinalizing && IsVassalageTrade( ePlayer))
			return false;

		//Can't already be offering this
		if (!bFinalizing && IsRevokeVassalageTrade( ePlayer))
			return false;
#if defined(MOD_BALANCE_CORE)
		if(!bHumanToHuman)
		{
			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
#endif
	}
	
#endif

	return true;
}

/// Get the number of resources available according to the deal being renewed and what's not on the table
int CvDeal::GetNumResource(PlayerTypes ePlayer, ResourceTypes eResource)
{
	int iNumAvailable = GET_PLAYER(ePlayer).getNumResourceAvailable(eResource, false);
	int iNumInExistingDeal = 0;

	TradedItemList::iterator it;
	// remove any that are in this deal
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_RESOURCES && it->m_eFromPlayer == ePlayer && (ResourceTypes)it->m_iData1 == eResource)
		{
			iNumInExistingDeal += it->m_iData2;
		}
	}

	return iNumAvailable - iNumInExistingDeal;
}

#if defined(MOD_BALANCE_CORE)
int CvDeal::GetNumCities(PlayerTypes ePlayer)
{
	if(ePlayer == NO_PLAYER)
		return 0;

	int iNumCities = 0;
	for(TradedItemList::iterator it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_CITIES && it->m_eFromPlayer == ePlayer)
		{
			iNumCities ++;
		}
	}
	return iNumCities;
}
bool CvDeal::IsCityInDeal(PlayerTypes ePlayer, int iCityID)
{
	if (ePlayer == NO_PLAYER)
		return 0;

	for (TradedItemList::iterator it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if (it->m_eItemType == TRADE_ITEM_CITIES && it->m_eFromPlayer == ePlayer)
		{
			CvCity* pCity = GET_PLAYER(ePlayer).getCity(iCityID);
			if (pCity != NULL)
			{
				if (it->m_iData1 == pCity->getX() &&
					it->m_iData2 == pCity->getY())
				{
					return true;
				}
			}
		}
	}
	return false;
}
#endif
/// What kind of Peace Treaty (if any) is this Deal?
PeaceTreatyTypes CvDeal::GetPeaceTreatyType() const
{
	return m_ePeaceTreatyType;
}

/// Sets what kind of Peace Treaty (if any) is this Deal
void CvDeal::SetPeaceTreatyType(PeaceTreatyTypes eTreaty)
{
	CvAssertMsg(eTreaty >= NO_PEACE_TREATY_TYPE, "DEAL: Invalid PeaceTreatyType index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");	// NO_PEACE_TREATY_TYPE is valid because we could be clearing the deal out for other uses
	CvAssertMsg(eTreaty < NUM_PEACE_TREATY_TYPES, "DEAL: Invalid PeaceTreatyType index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_ePeaceTreatyType = eTreaty;
}

/// Who (if anyone) is surrendering in this Deal?
PlayerTypes CvDeal::GetSurrenderingPlayer() const
{
	return m_eSurrenderingPlayer;
}

/// Sets Who (if anyone) is surrendering in this Deal
void CvDeal::SetSurrenderingPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= NO_PLAYER, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");	// NO_PLAYER is valid because we could be clearing the deal out for other uses
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_eSurrenderingPlayer = ePlayer;
}

/// Who (if anyone) is making a demand in this Deal?
PlayerTypes CvDeal::GetDemandingPlayer() const
{
	return m_eDemandingPlayer;
}

/// Sets Who (if anyone) is making a demand in this Deal
void CvDeal::SetDemandingPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= NO_PLAYER, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");	// NO_PLAYER is valid because we could be clearing the deal out for other uses
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_eDemandingPlayer = ePlayer;
}

/// Who (if anyone) is making a request in this Deal?
PlayerTypes CvDeal::GetRequestingPlayer() const
{
	return m_eRequestingPlayer;
}

/// Sets Who (if anyone) is making a request in this Deal
void CvDeal::SetRequestingPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= NO_PLAYER, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");	// NO_PLAYER is valid because we could be clearing the deal out for other uses
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DEAL: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_eRequestingPlayer = ePlayer;
}


// METHODS TO ADD A CvTradedItem TO A DEAL

/// Insert an immediate gold trade
void CvDeal::AddGoldTrade(PlayerTypes eFrom, int iAmount)
{
	CvAssertMsg(iAmount >= 0, "DEAL: Trying to add a negative amount of Gold to a deal.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_GOLD, iAmount))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_GOLD;
		item.m_iDuration = 0;
		item.m_iFinalTurn = -1;
		item.m_iData1 = iAmount;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
}

/// Insert a gold per turn trade
void CvDeal::AddGoldPerTurnTrade(PlayerTypes eFrom, int iAmount, int iDuration)
{
	CvAssertMsg(iAmount >= 0, "DEAL: Trying to add a negative amount of GPT to a deal.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_GOLD_PER_TURN, iAmount, iDuration))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_GOLD_PER_TURN;
		item.m_iDuration = iDuration;
		//item.m_iFinalTurn = iDuration + GC.getGame().getGameTurn();
		item.m_iFinalTurn = -1;
		item.m_iData1 = iAmount;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid GPT amount to a deal");
	}
}

/// Insert a map trade
void CvDeal::AddMapTrade(PlayerTypes eFrom)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_MAPS))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_MAPS;
		item.m_iDuration = 0;
		item.m_iFinalTurn = -1;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Map item to a deal");
	}
}

/// Insert a resource trade
void CvDeal::AddResourceTrade(PlayerTypes eFrom, ResourceTypes eResource, int iAmount, int iDuration)
{
	CvAssertMsg(iAmount >= 0, "DEAL: Trying to add a negative amount of a Resource to a deal.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_RESOURCES, eResource, iAmount))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_RESOURCES;
		item.m_iDuration = iDuration;
		//item.m_iFinalTurn = iDuration + GC.getGame().getGameTurn();
		item.m_iFinalTurn = -1;
		item.m_iData1 = (int)eResource;
		item.m_iData2 = iAmount;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Resource to a deal");
	}
}

/// Insert a city trade
void CvDeal::AddCityTrade(PlayerTypes eFrom, int iCityID)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvCity* pCity = GET_PLAYER(eFrom).getCity(iCityID);
	if (!pCity)
	{
		OutputDebugString("invalid city ID!\n");
		return;
	}

	int x = pCity->getX();
	int y = pCity->getY();

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_CITIES, x, y))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_CITIES;
		item.m_iDuration = 0;
		item.m_iFinalTurn = -1;

		item.m_iData1 = x;
		item.m_iData2 = y;

		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid City to a deal");
	}
}

/// Insert a unit trade
void CvDeal::AddUnitTrade(PlayerTypes eFrom, int iUnitID)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_UNITS, iUnitID))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_UNITS;
		item.m_iDuration = 0;
		item.m_iFinalTurn = -1;
		item.m_iData1 = iUnitID;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Unit to a deal");
	}
}

/// Insert adding an embassy to the deal
void CvDeal::AddAllowEmbassy(PlayerTypes eFrom)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_ALLOW_EMBASSY))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_ALLOW_EMBASSY;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Allow Embassy item to a deal");
	}
}

/// Insert an open borders pact
void CvDeal::AddOpenBorders(PlayerTypes eFrom, int iDuration)
{
	CvAssertMsg(iDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_OPEN_BORDERS, iDuration))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_OPEN_BORDERS;
		item.m_iDuration = iDuration;
		//item.m_iFinalTurn = iDuration + GC.getGame().getGameTurn();
		item.m_iFinalTurn = -1;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Open Borders item to a deal");
	}
}

/// Insert a defensive pact
void CvDeal::AddDefensivePact(PlayerTypes eFrom, int iDuration)
{
	CvAssertMsg(iDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_DEFENSIVE_PACT, iDuration))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_DEFENSIVE_PACT;
		item.m_iDuration = iDuration;
		//item.m_iFinalTurn = iDuration + GC.getGame().getGameTurn();
		item.m_iFinalTurn = -1;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Defensive Pact item to a deal");
	}
}

/// Insert a Research Agreement
void CvDeal::AddResearchAgreement(PlayerTypes eFrom, int iDuration)
{
	CvAssertMsg(iDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_RESEARCH_AGREEMENT, iDuration))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_RESEARCH_AGREEMENT;
		item.m_iDuration = iDuration;
		//item.m_iFinalTurn = iDuration + GC.getGame().getGameTurn();
		item.m_iFinalTurn = -1;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Research Agreement item to a deal");
	}
}

/// Insert a Trade Agreement
void CvDeal::AddTradeAgreement(PlayerTypes eFrom, int iDuration)
{
	CvAssertMsg(iDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_TRADE_AGREEMENT, iDuration))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_TRADE_AGREEMENT;
		item.m_iDuration = iDuration;
		//item.m_iFinalTurn = iDuration + GC.getGame().getGameTurn();
		item.m_iFinalTurn = -1;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Trade Agreement item to a deal");
	}
}

/// Insert a permanent alliance
void CvDeal::AddPermamentAlliance()
{
//	if (IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_PERMANENT_ALLIANCE, iDuration))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_PERMANENT_ALLIANCE;
		item.m_iDuration = 0;
		item.m_iFinalTurn = -1;
		m_TradedItems.push_back(item);
	}
//	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Permanent Alliance item to a deal");
	}
}

/// Insert one side surrendering in a war
void CvDeal::AddSurrender(PlayerTypes eFrom)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_SURRENDER))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_SURRENDER;
		item.m_iDuration = 0;
		item.m_iFinalTurn = -1;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Surrender item to a deal");
	}
}

/// Insert a short-term truce
void CvDeal::AddTruce()
{
//	if (IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_TRUCE))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_TRUCE;
		item.m_iDuration = 0;
		item.m_iFinalTurn = -1;
		m_TradedItems.push_back(item);
	}
//	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Truce item to a deal");
	}
}

/// Insert ending a war
void CvDeal::AddPeaceTreaty(PlayerTypes eFrom, int iDuration)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_PEACE_TREATY))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_PEACE_TREATY;
		item.m_iDuration = iDuration;
		item.m_iFinalTurn = -1;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Peace Treaty item to a deal");
	}
}

/// Insert going to peace with a third party
void CvDeal::AddThirdPartyPeace(PlayerTypes eFrom, TeamTypes eThirdPartyTeam, int iDuration)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_THIRD_PARTY_PEACE, eThirdPartyTeam))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_THIRD_PARTY_PEACE;
		item.m_iDuration = iDuration;
		item.m_iFinalTurn = -1;
		item.m_iData1 = eThirdPartyTeam;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Third Party Peace item to a deal");
	}
}

/// Insert going to war with a third party
void CvDeal::AddThirdPartyWar(PlayerTypes eFrom, TeamTypes eThirdPartyTeam)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_THIRD_PARTY_WAR, eThirdPartyTeam))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_THIRD_PARTY_WAR;
		item.m_iDuration = 0;
		item.m_iFinalTurn = -1;
		item.m_iData1 = eThirdPartyTeam;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Third Party War item to a deal");
	}
}

/// Insert starting an embargo on a third party
void CvDeal::AddThirdPartyEmbargo(PlayerTypes eFrom, PlayerTypes eThirdParty, int iDuration)
{
	CvAssertMsg(iDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_THIRD_PARTY_EMBARGO, eThirdParty, iDuration))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_THIRD_PARTY_EMBARGO;
		item.m_iDuration = iDuration;
		//item.m_iFinalTurn = iDuration + GC.getGame().getGameTurn();
		item.m_iFinalTurn = -1;
		item.m_iData1 = eThirdParty;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Third Party Embargo item to a deal");
	}
}

/// Insert adding a declaration of peace to the deal
void CvDeal::AddDeclarationOfFriendship(PlayerTypes eFrom)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_DECLARATION_OF_FRIENDSHIP))
	{
		if (!ContainsItemType(TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, eFrom))
		{
			CvTradedItem item;
			item.m_eItemType = TRADE_ITEM_DECLARATION_OF_FRIENDSHIP;
			item.m_eFromPlayer = eFrom;
			m_TradedItems.push_back(item);
		}
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Declaration of Friendship item to a deal");
	}
}

/// Insert a vote commitment to the deal
void CvDeal::AddVoteCommitment(PlayerTypes eFrom, int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_VOTE_COMMITMENT, iResolutionID, iVoteChoice, iNumVotes, bRepeal))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_VOTE_COMMITMENT;
		item.m_eFromPlayer = eFrom;
		item.m_iData1 = iResolutionID;
		item.m_iData2 = iVoteChoice;
		item.m_iData3 = iNumVotes;
		item.m_bFlag1 = bRepeal;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Vote Commitment item to a deal");
	}
}

int CvDeal::GetGoldTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_GOLD && it->m_eFromPlayer == eFrom)
		{
			return it->m_iData1;
		}
	}
	return 0;
}

bool CvDeal::ChangeGoldTrade(PlayerTypes eFrom, int iNewAmount)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Changing deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iOldValue;

	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_GOLD && it->m_eFromPlayer == eFrom)
		{
			// Reduce Gold value to 0 first, because otherwise IsPossibleToTradeItem will think we're trying to spend more than we have
			iOldValue = it->m_iData1;
			it->m_iData1 = 0;

			if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_GOLD, iNewAmount))
			{
				it->m_iData1 = iNewAmount;
				return true;
			}
			// If we can't do this then restore the previous Gold quantity
			else
			{
				it->m_iData1 = iOldValue;
			}
		}
	}
	return false;
}

int CvDeal::GetGoldPerTurnTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_GOLD_PER_TURN && it->m_eFromPlayer == eFrom)
		{
			return it->m_iData1;
		}
	}
	return 0;
}

bool CvDeal::ChangeGoldPerTurnTrade(PlayerTypes eFrom, int iNewAmount, int iDuration)
{
	CvAssertMsg(iDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Changing deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_GOLD_PER_TURN && it->m_eFromPlayer == eFrom)
		{
			if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_GOLD_PER_TURN, iNewAmount, iDuration))
			{
				it->m_iData1 = iNewAmount;
				it->m_iDuration = iDuration;
				it->m_iFinalTurn = iDuration + GC.getGame().getGameTurn();
				return true;
			}
		}
	}
	return false;
}

bool CvDeal::IsResourceTrade(PlayerTypes eFrom, ResourceTypes eResource)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_RESOURCES &&
		        it->m_eFromPlayer == eFrom &&
		        (ResourceTypes)it->m_iData1 == eResource)
		{
			return true;
		}
	}
	return false;
}

int CvDeal::GetNumResourcesInDeal(PlayerTypes eFrom, ResourceTypes eResource)
{
	int iNum = 0;
	TradedItemList::iterator it;
	for (it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if (it->m_eItemType == TRADE_ITEM_RESOURCES &&
			it->m_eFromPlayer == eFrom &&
			(ResourceTypes)it->m_iData1 == eResource)
		{
			iNum += it->m_iData2;
		}
	}
	return iNum;
}

bool CvDeal::ChangeResourceTrade(PlayerTypes eFrom, ResourceTypes eResource, int iAmount, int iDuration)
{
	CvAssertMsg(iDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Changing deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_RESOURCES &&
		        it->m_eFromPlayer == eFrom &&
		        (ResourceTypes)it->m_iData1 == eResource)
		{
			if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_RESOURCES, eResource, iAmount))
			{
				it->m_iData2 = iAmount;
				it->m_iDuration = iDuration;
				it->m_iFinalTurn = iDuration + GC.getGame().getGameTurn();
				return true;
			}
		}
	}
	return false;
}

bool CvDeal::IsCityTrade(PlayerTypes eFrom, int cityX, int cityY)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType   == TRADE_ITEM_CITIES &&
		        it->m_eFromPlayer == eFrom &&
		        it->m_iData1 == cityX &&
		        it->m_iData2 == cityY)
		{
			return true;
		}
	}
	return false;
}

bool CvDeal::IsAllowEmbassyTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_ALLOW_EMBASSY && it->m_eFromPlayer == eFrom)
		{
			return true;
		}
	}
	return 0;
}

bool CvDeal::IsOpenBordersTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_OPEN_BORDERS && it->m_eFromPlayer == eFrom)
		{
			return true;
		}
	}
	return 0;
}

bool CvDeal::IsDefensivePactTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_DEFENSIVE_PACT && it->m_eFromPlayer == eFrom)
		{
			return true;
		}
	}
	return 0;
}

bool CvDeal::IsResearchAgreementTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_RESEARCH_AGREEMENT && it->m_eFromPlayer == eFrom)
		{
			return true;
		}
	}
	return 0;
}

bool CvDeal::IsTradeAgreementTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_TRADE_AGREEMENT && it->m_eFromPlayer == eFrom)
		{
			return true;
		}
	}
	return 0;
}

bool CvDeal::IsPeaceTreatyTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_PEACE_TREATY && it->m_eFromPlayer == eFrom)
		{
			return true;
		}
	}
	return 0;
}

bool CvDeal::IsThirdPartyPeaceTrade(PlayerTypes eFrom, TeamTypes eThirdPartyTeam)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_PEACE && it->m_eFromPlayer == eFrom && it->m_iData1 == eThirdPartyTeam)
		{
			return true;
		}
	}
	return 0;
}

bool CvDeal::IsThirdPartyWarTrade(PlayerTypes eFrom, TeamTypes eThirdPartyTeam)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_WAR && it->m_eFromPlayer == eFrom && it->m_iData1 == eThirdPartyTeam)
		{
			return true;
		}
	}
	return 0;
}

bool CvDeal::IsVoteCommitmentTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_VOTE_COMMITMENT && it->m_eFromPlayer == eFrom)
		{
			return true;
		}
	}
	return false;
}

CvDeal::DealRenewStatus CvDeal::GetItemTradeableState(TradeableItems eTradeItem)
{
	switch(eTradeItem)
	{
		// not renewable
	case TRADE_ITEM_ALLOW_EMBASSY:
	case TRADE_ITEM_CITIES:
	case TRADE_ITEM_UNITS:
	case TRADE_ITEM_SURRENDER:
	case TRADE_ITEM_TRUCE:
	case TRADE_ITEM_PEACE_TREATY:
	case TRADE_ITEM_PERMANENT_ALLIANCE:
	case TRADE_ITEM_THIRD_PARTY_PEACE:
	case TRADE_ITEM_THIRD_PARTY_WAR:
	case TRADE_ITEM_VOTE_COMMITMENT:
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	case TRADE_ITEM_VASSALAGE:
	case TRADE_ITEM_MAPS:
#endif
		return DEAL_NONRENEWABLE;
		break;

		// renewable
	case TRADE_ITEM_GOLD_PER_TURN:
	case TRADE_ITEM_RESOURCES:
	case TRADE_ITEM_OPEN_BORDERS:
	case TRADE_ITEM_DEFENSIVE_PACT:
	case TRADE_ITEM_THIRD_PARTY_EMBARGO: // dead!
		return DEAL_RENEWABLE;
		break;

		// doesn't matter
	case TRADE_ITEM_TRADE_AGREEMENT:
	case TRADE_ITEM_GOLD:
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	case TRADE_ITEM_TECHS:
#else
	case TRADE_ITEM_MAPS:
#endif
	case TRADE_ITEM_RESEARCH_AGREEMENT:
		return DEAL_SUPPLEMENTAL;
		break;
	}

	CvAssertMsg(false, "unknown eTradeItem passed in");
	return DEAL_NONRENEWABLE;
}

bool CvDeal::IsPotentiallyRenewable()
{
	TradedItemList::iterator it;
	bool bHasValidTradeItem = false;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		switch(GetItemTradeableState(it->m_eItemType))
		{
		case DEAL_NONRENEWABLE:
			return false;
		case DEAL_RENEWABLE:
			bHasValidTradeItem = true;
			break;
		case DEAL_SUPPLEMENTAL:
			break;
		}
	}
	return bHasValidTradeItem;
}

/// Delete a trade item that can be identified by type alone
void CvDeal::RemoveByType(TradeableItems eItemType, PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == eItemType &&
		        (eFrom == NO_PLAYER || eFrom == it->m_eFromPlayer))
		{
			m_TradedItems.erase(it);
			break;
		}
	}
}

/// Delete a resource trade
void CvDeal::RemoveResourceTrade(ResourceTypes eResource)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_RESOURCES &&
		        (ResourceTypes)it->m_iData1 == eResource)
		{
			m_TradedItems.erase(it);
			break;
		}
	}
}

/// Delete a city trade
void CvDeal::RemoveCityTrade(PlayerTypes eFrom, int iCityID)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_CITIES &&
		        it->m_eFromPlayer == eFrom)
		{
			CvCity* pCity = GET_PLAYER(eFrom).getCity(iCityID);
			if(it->m_iData1 == pCity->getX() &&
			        it->m_iData2 == pCity->getY())
			{
				m_TradedItems.erase(it);
				return;
			}
		}
	}
}

/// Delete a unit trade
void CvDeal::RemoveUnitTrade(int iUnitID)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_UNITS &&
		        it->m_iData1 == iUnitID)
		{
			m_TradedItems.erase(it);
			break;
		}
	}
}

/// Delete a peace deal with a third party
void CvDeal::RemoveThirdPartyPeace(PlayerTypes eFrom, TeamTypes eThirdPartyTeam)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_PEACE &&
		        (TeamTypes)it->m_iData1 == eThirdPartyTeam &&
		        (PlayerTypes)it->m_eFromPlayer == eFrom)
		{
			m_TradedItems.erase(it);
			break;
		}
	}
}

/// Delete a war deal with a third party
void CvDeal::RemoveThirdPartyWar(PlayerTypes eFrom, TeamTypes eThirdPartyTeam)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_WAR &&
		        (PlayerTypes)it->m_iData1 == eThirdPartyTeam &&
		        (PlayerTypes)it->m_eFromPlayer == eFrom)
		{
			m_TradedItems.erase(it);
			break;
		}
	}
}

/// Delete an embargo deal with a third party
void CvDeal::RemoveThirdPartyEmbargo(PlayerTypes eFrom, PlayerTypes eThirdParty)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_EMBARGO &&
		        (PlayerTypes)it->m_iData1 == eThirdParty &&
		        (PlayerTypes)it->m_eFromPlayer == eFrom)
		{
			m_TradedItems.erase(it);
			break;
		}
	}
}

/// Delete a vote commitment
void CvDeal::RemoveVoteCommitment(PlayerTypes eFrom, int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_VOTE_COMMITMENT &&
			it->m_eFromPlayer == eFrom &&
			it->m_iData1 == iResolutionID &&
			it->m_iData2 == iVoteChoice &&
			it->m_iData3 == iNumVotes &&
			it->m_bFlag1 == bRepeal)
		{
			m_TradedItems.erase(it);
			break;
		}
	}
}

void CvDeal::ChangeThirdPartyWarDuration(PlayerTypes eFrom, TeamTypes eThirdPartyTeam, int iNewDuration)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_WAR &&
		        (PlayerTypes)it->m_iData1 == eThirdPartyTeam &&
		        (PlayerTypes)it->m_eFromPlayer == eFrom)
		{
			it->m_iDuration = iNewDuration;
			break;
		}
	}
}


void CvDeal::ChangeThirdPartyPeaceDuration(PlayerTypes eFrom, TeamTypes eThirdPartyTeam, int iNewDuration)
{
	CvAssertMsg(iNewDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iNewDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_PEACE &&
		        (TeamTypes)it->m_iData1 == eThirdPartyTeam &&
		        (PlayerTypes)it->m_eFromPlayer == eFrom)
		{
			it->m_iDuration = iNewDuration;
			break;
		}
	}
}

void CvDeal::ChangeThirdPartyEmbargoDuration(PlayerTypes eFrom, PlayerTypes eThirdParty, int iNewDuration)
{
	CvAssertMsg(iNewDuration >= 0, "DEAL: Trying to add a negative duration to a TradeItem.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iNewDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_EMBARGO &&
		        (PlayerTypes)it->m_iData1 == eThirdParty &&
		        (PlayerTypes)it->m_eFromPlayer == eFrom)
		{
			it->m_iDuration = iNewDuration;
			break;
		}
	}
}

bool CvDeal::ContainsItemType(TradeableItems eItemType, PlayerTypes eFrom /* = NO_PLAYER */)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == eItemType && (eFrom == NO_PLAYER || it->m_eFromPlayer == eFrom))
		{
			return true;
		}
	}
	return false;
}

// PRIVATE METHODS

FDataStream& OldLoad(FDataStream& loadFrom, CvDeal& writeTo)
{
	int iEntriesToRead;
	CvTradedItem tempItem;

	loadFrom >> writeTo.m_eFromPlayer;
	loadFrom >> writeTo.m_eToPlayer;
	loadFrom >> writeTo.m_iFinalTurn;
	loadFrom >> writeTo.m_iDuration;
	loadFrom >> writeTo.m_iStartTurn;
	loadFrom >> writeTo.m_bConsideringForRenewal;
	loadFrom >> writeTo.m_bDealCancelled;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	loadFrom >> writeTo.m_bIsGift;
#endif
	loadFrom >> writeTo.m_ePeaceTreatyType;
	loadFrom >> writeTo.m_eSurrenderingPlayer;
	loadFrom >> writeTo.m_eDemandingPlayer;
	loadFrom >> writeTo.m_eRequestingPlayer;
	loadFrom >> iEntriesToRead;

	writeTo.m_TradedItems.clear();
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom = OldLoad(loadFrom, tempItem);
		writeTo.m_TradedItems.push_back(tempItem);
	}
	return loadFrom;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvDeal& writeTo)
{
	uint uiVersion;
	int iEntriesToRead;
	CvTradedItem tempItem;

	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);
	loadFrom >> writeTo.m_eFromPlayer;
	loadFrom >> writeTo.m_eToPlayer;
	loadFrom >> writeTo.m_iFinalTurn;
	loadFrom >> writeTo.m_iDuration;
	loadFrom >> writeTo.m_iStartTurn;
	loadFrom >> writeTo.m_bConsideringForRenewal;
	if (uiVersion >= 3)
	{
		loadFrom >> writeTo.m_bCheckedForRenewal;
	}
	else
	{
		writeTo.m_bCheckedForRenewal = false;
	}
	loadFrom >> writeTo.m_bDealCancelled;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	loadFrom >> writeTo.m_bIsGift;
#endif
	loadFrom >> writeTo.m_ePeaceTreatyType;
	loadFrom >> writeTo.m_eSurrenderingPlayer;
	loadFrom >> writeTo.m_eDemandingPlayer;
	loadFrom >> writeTo.m_eRequestingPlayer;
	loadFrom >> iEntriesToRead;

	writeTo.m_TradedItems.clear();
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		if(uiVersion >= 2)
		{
			loadFrom >> tempItem;
		}
		else
		{
			loadFrom = OldLoad(loadFrom, tempItem);
		}
		writeTo.m_TradedItems.push_back(tempItem);
	}
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvDeal& readFrom)
{
	// Current version number
	uint uiVersion = 3;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);
	saveTo << readFrom.m_eFromPlayer;
	saveTo << readFrom.m_eToPlayer;
	saveTo << readFrom.m_iFinalTurn;
	saveTo << readFrom.m_iDuration;
	saveTo << readFrom.m_iStartTurn;
	saveTo << readFrom.m_bConsideringForRenewal;
	saveTo << readFrom.m_bCheckedForRenewal;
	saveTo << readFrom.m_bDealCancelled;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	saveTo << readFrom.m_bIsGift;
#endif
	saveTo << readFrom.m_ePeaceTreatyType;
	saveTo << readFrom.m_eSurrenderingPlayer;
	saveTo << readFrom.m_eDemandingPlayer;
	saveTo << readFrom.m_eRequestingPlayer;
	saveTo << readFrom.m_TradedItems.size();
	TradedItemList::const_iterator it;
	for(it = readFrom.m_TradedItems.begin(); it != readFrom.m_TradedItems.end(); ++it)
	{
		saveTo << *it;
	}

	return saveTo;
}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
/// Insert a tech trade
void CvDeal::AddTechTrade(PlayerTypes eFrom, TechTypes eTech)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_TECHS, eTech))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_TECHS;
		item.m_iDuration = 0;
		item.m_iFinalTurn = -1;
		item.m_iData1 = (int)eTech;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Tech item to a deal");
	}
}

/// Insert Vassalage Trade
void CvDeal::AddVassalageTrade(PlayerTypes eFrom)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_VASSALAGE))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_VASSALAGE;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Vassalage item to a deal");
	}
}

/// Insert Vassalage Trade
void CvDeal::AddRevokeVassalageTrade(PlayerTypes eFrom)
{
	CvAssertMsg(eFrom == m_eFromPlayer || eFrom == m_eToPlayer, "DEAL: Adding deal item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPossibleToTradeItem(eFrom, GetOtherPlayer(eFrom), TRADE_ITEM_VASSALAGE_REVOKE))
	{
		CvTradedItem item;
		item.m_eItemType = TRADE_ITEM_VASSALAGE_REVOKE;
		item.m_eFromPlayer = eFrom;
		m_TradedItems.push_back(item);
	}
	else
	{
		CvAssertMsg(false, "DEAL: Trying to add an invalid Vassalage item to a deal");
	}
}

bool CvDeal::IsMapTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType	== TRADE_ITEM_MAPS &&
			it->m_eFromPlayer == eFrom)
		{
			return true;
		}

	}
	return 0;
}

bool CvDeal::IsTechTrade(PlayerTypes eFrom, TechTypes eTech)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType	== TRADE_ITEM_TECHS &&
			it->m_eFromPlayer == eFrom &&
			(TechTypes)it->m_iData1 == eTech)
		{
			return true;
		}

	}
	return 0;
}

bool CvDeal::IsVassalageTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_VASSALAGE && it->m_eFromPlayer == eFrom)
		{
			return true;
		}
	}
	return 0;
}

bool CvDeal::IsRevokeVassalageTrade(PlayerTypes eFrom)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_VASSALAGE_REVOKE && it->m_eFromPlayer == eFrom)
		{
			return true;
		}
	}
	return 0;
}

/// Delete a tech trade
void CvDeal::RemoveTechTrade(TechTypes eTech)
{
	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_TECHS &&
		        (TechTypes)it->m_iData1 == eTech)
		{
			m_TradedItems.erase(it);
			break;
		}
	}
}
#endif

//=====================================
// CvGameDeals
//=====================================

/// Constructor
CvGameDeals::CvGameDeals()
	: m_uiDealCounter(0)
{
	Init();
}

/// Destructor
CvGameDeals::~CvGameDeals()
{
	for(std::vector<std::pair<uint, CvDeal*> >::iterator it = m_Deals.begin();
	        it != m_Deals.end(); ++it)
	{
		delete(*it).second;
		it->second = NULL;
	}

	m_Deals.clear();
}

/// Initialize
void CvGameDeals::Init()
{
	m_ProposedDeals.clear();
	m_CurrentDeals.clear();
	m_HistoricalDeals.clear();
}

/// Save off a new deal that has been agreed to
void CvGameDeals::AddProposedDeal(CvDeal kDeal)
{
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		PlayerTypes eFrom = kDeal.GetFromPlayer();
		PlayerTypes eTo = kDeal.GetToPlayer();
		if (CvPreGame::isHuman(eFrom) && CvPreGame::isHuman(eTo))
		{
			// only one deal from Human to Human
			CvDeal kRemovedDeal;
			while (RemoveProposedDeal(eFrom, eTo, &kRemovedDeal, true))
			{//deal from eCancelPlayer
				FinalizeMPDeal(kRemovedDeal, false);
			}
			while (RemoveProposedDeal(eTo, eFrom, &kRemovedDeal, true))
			{//deal to eCancelPlayer
				FinalizeMPDeal(kRemovedDeal, false);
			}
		}
	}
#endif
	// Store Deal away
	m_ProposedDeals.push_back(kDeal);

	// Update UI if we were involved in the deal
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
	if(kDeal.m_eFromPlayer == eActivePlayer || kDeal.m_eToPlayer == eActivePlayer)
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}

#if defined(MOD_ACTIVE_DIPLOMACY)
/// Removes a deal from the proposed deals list (returns FALSE if deal not found)
bool CvGameDeals::RemoveProposedDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal* pDealOut, bool latest)
{
	CvDeal* pDeal = GetProposedMPDeal(eFromPlayer, eToPlayer, latest);

	if (!pDeal)
		return false;

	if (pDealOut)
	{
		*pDealOut = *pDeal;
	}

	m_ProposedDeals.erase(pDeal);

	return true;
}

bool CvDeal::AreAllTradeItemsValid()
{
	TradedItemList::iterator iter;
	for (iter = m_TradedItems.begin(); iter != m_TradedItems.end(); ++iter)
	{
		if (iter->m_bToRenewed)  // slewis - added exception in case of something that was renewed
		{
			continue;
		}

		if (!IsPossibleToTradeItem(iter->m_eFromPlayer, GetOtherPlayer(iter->m_eFromPlayer), iter->m_eItemType, iter->m_iData1, iter->m_iData2, iter->m_iData3, iter->m_bFlag1, false, true))
			return false;
	}
	return true;
}

/// Moves a deal from the proposed list to the active one (returns FALSE if deal is not valid)
bool CvGameDeals::FinalizeMPDealLatest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bAccepted, bool latest)
{
	CvDeal kDeal;
	if (!RemoveProposedDeal(eFromPlayer, eToPlayer, &kDeal, latest))
	{
		LogDealFailed(NULL, false, !bAccepted, false);
		return false;
	}
	return FinalizeMPDeal(kDeal, bAccepted);
}

/// Moves a deal from the proposed list to the active one (returns FALSE if deal not valid)
bool CvGameDeals::FinalizeMPDeal(CvDeal kDeal, bool bAccepted)
{
	PlayerTypes eFromPlayer = kDeal.m_eFromPlayer;
	PlayerTypes eToPlayer = kDeal.m_eToPlayer;
	bool bFoundIt = true;
	bool bValid = kDeal.AreAllTradeItemsValid();
	CvWeightedVector<TeamTypes, MAX_CIV_TEAMS, true> veNowAtPeacePairs; // hacked CvWeighedVector to keep track of third party minors that this deal makes at peace
	{
		if(!bValid || !bAccepted)
		{
			LogDealFailed(&kDeal, false, !bAccepted, true);
		}

		if(bValid && bAccepted)
		{
			FinalizeDealValidAndAccepted(eFromPlayer, eToPlayer, kDeal, bAccepted, veNowAtPeacePairs);
			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (eLoopPlayer != NO_PLAYER)
				{
					GET_PLAYER(eLoopPlayer).GetDiplomacyRequests()->CheckRemainingNotifications();
				}
			}
		}
	}
	
	FinalizeDealNotify(eFromPlayer, eToPlayer, veNowAtPeacePairs);

	return bFoundIt && bValid;
}

void CvGameDeals::FinalizeDealValidAndAccepted(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal& kDeal, bool bAccepted, CvWeightedVector<TeamTypes, MAX_CIV_TEAMS, true>& veNowAtPeacePairs)
{
	// Determine total duration of the Deal
	int iLatestItemLastTurn = 0;
	int iLongestDuration = 0;
	//int iTemp;

	TradedItemList::iterator it;
	for(it = kDeal.m_TradedItems.begin(); it != kDeal.m_TradedItems.end(); ++it)
	{
		CvAssertMsg(it->m_eFromPlayer == kDeal.m_eFromPlayer || it->m_eFromPlayer == kDeal.m_eToPlayer, "DEAL: Adding deal that has an item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		// Calculate duration
		if(it->m_iDuration > 0)
		{
			it->m_iFinalTurn = it->m_iDuration + GC.getGame().getGameTurn();
			if(it->m_iDuration > iLongestDuration)
			{
				iLongestDuration = it->m_iDuration;
				iLatestItemLastTurn = it->m_iFinalTurn;
			}
		}
	}

	kDeal.m_iDuration = iLongestDuration;
	kDeal.m_iFinalTurn = iLatestItemLastTurn;
	kDeal.m_iStartTurn = GC.getGame().getGameTurn();

	// Add to current deals
	CvAssertMsg(kDeal.m_TradedItems.size() > 0, "New deal has no tradeable items!");
	m_CurrentDeals.push_back(kDeal);
	kDeal.m_iStartTurn = GC.getGame().getGameTurn();

	bool bSentResearchAgreementNotification = false;

	int iCost;

	// What effects does this Deal have right now?
	PlayerTypes eAcceptedFromPlayer;
	PlayerTypes eAcceptedToPlayer;
	TeamTypes eFromTeam;
	TeamTypes eToTeam;
#if defined(MOD_BALANCE_CORE)
	bool bDone = false;
	//Set surrendering player in human v. human war. Based on who puts what where.
	if(GET_PLAYER(eFromPlayer).isHuman() && GET_PLAYER(eToPlayer).isHuman())
	{
		if(kDeal.IsPeaceTreatyTrade(eFromPlayer) || kDeal.IsPeaceTreatyTrade(eToPlayer))
		{
			for(it = kDeal.m_TradedItems.begin(); it != kDeal.m_TradedItems.end(); ++it)
			{
				// if the deal is renewed do not start it up
				if(it->m_bToRenewed)
				{
					continue;
				}

				eAcceptedFromPlayer = it->m_eFromPlayer;
				eAcceptedToPlayer = kDeal.GetOtherPlayer(eAcceptedFromPlayer);
				eFromTeam = GET_PLAYER(eAcceptedFromPlayer).getTeam();
				eToTeam = GET_PLAYER(eAcceptedToPlayer).getTeam();

				CvAssertMsg(eAcceptedFromPlayer == kDeal.m_eFromPlayer || eAcceptedFromPlayer == kDeal.m_eToPlayer, "DEAL: Adding deal that has an item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					
				// Gold
				if(it->m_eItemType == TRADE_ITEM_GOLD)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
				// Gold Per Turn
				else if(it->m_eItemType == TRADE_ITEM_GOLD_PER_TURN)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
				// Resource
				else if(it->m_eItemType == TRADE_ITEM_RESOURCES)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
				// City
				else if(it->m_eItemType == TRADE_ITEM_CITIES)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
				else if(it->m_eItemType == TRADE_ITEM_ALLOW_EMBASSY)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;	
				}
				// Vote Commitment
				else if(it->m_eItemType == TRADE_ITEM_VOTE_COMMITMENT)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
				// Open Borders
				else if(it->m_eItemType == TRADE_ITEM_OPEN_BORDERS)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
				// Research Agreement
				else if(it->m_eItemType == TRADE_ITEM_RESEARCH_AGREEMENT)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
				// Maps
				else if (MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_MAPS)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
				// Techs
				else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_TECHS)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
				// Vassalage
				else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_VASSALAGE)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}
				// Revoke Vassalage
				else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_VASSALAGE_REVOKE)
				{
					kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
					break;
				}						
			}
		}
	}
#endif
#endif

	for(it = kDeal.m_TradedItems.begin(); it != kDeal.m_TradedItems.end(); ++it)
	{
		// if the deal is renewed do not start it up
		if(it->m_bToRenewed)
		{
			LogDealFailed(&kDeal, true, !bAccepted, false);	
			continue;
		}

		eAcceptedFromPlayer = it->m_eFromPlayer;
		eAcceptedToPlayer = kDeal.GetOtherPlayer(eAcceptedFromPlayer);
		eFromTeam = GET_PLAYER(eAcceptedFromPlayer).getTeam();
		eToTeam = GET_PLAYER(eAcceptedToPlayer).getTeam();

		CvAssertMsg(eAcceptedFromPlayer == kDeal.m_eFromPlayer || eAcceptedFromPlayer == kDeal.m_eToPlayer, "DEAL: Adding deal that has an item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		// Deduct Gold cost (if applicable)
		iCost = GetTradeItemGoldCost(it->m_eItemType, eAcceptedFromPlayer, eAcceptedToPlayer);
		GET_PLAYER(eAcceptedFromPlayer).GetTreasury()->ChangeGold(-iCost);

		// Gold
		if(it->m_eItemType == TRADE_ITEM_GOLD)
		{
			int iGoldAmount = it->m_iData1;
			GET_PLAYER(eAcceptedFromPlayer).GetTreasury()->ChangeGold(-iGoldAmount);
			GET_PLAYER(eAcceptedToPlayer).GetTreasury()->ChangeGold(iGoldAmount);
		}
		// Gold Per Turn
		else if(it->m_eItemType == TRADE_ITEM_GOLD_PER_TURN)
		{
			int iGoldPerTurn = it->m_iData1;
			GET_PLAYER(eAcceptedFromPlayer).GetTreasury()->ChangeGoldPerTurnFromDiplomacy(-iGoldPerTurn);
			GET_PLAYER(eAcceptedToPlayer).GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iGoldPerTurn);
		}
		// Resource
		else if(it->m_eItemType == TRADE_ITEM_RESOURCES)
		{
			ResourceTypes eResource = (ResourceTypes) it->m_iData1;
			int iResourceQuantity = it->m_iData2;
			GET_PLAYER(eAcceptedFromPlayer).changeResourceExport(eResource, iResourceQuantity);
			GET_PLAYER(eAcceptedToPlayer).changeResourceImportFromMajor(eResource, iResourceQuantity);

#if !defined(NO_ACHIEVEMENTS)
			//Resource Trading Achievements
			if(!GC.getGame().isGameMultiPlayer())
			{
				if(GET_PLAYER(eAcceptedFromPlayer).isHuman())
				{
					gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_RESOURCESTRADED, 100, ACHIEVEMENT_100RESOURCES);

					if(GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
					{
						//OutputDebugString("\nStep0");
						if(((CvString)GET_PLAYER(eAcceptedFromPlayer).getLeaderTypeKey() == "LEADER_HARUN_AL_RASHID"))
						{
							//OutputDebugString("\nStep1");
							int iJ;
							for(iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
							{
								CvBuildingEntry* pkBuildingEntry = GC.getBuildingInfo((BuildingTypes)iJ);
								if(pkBuildingEntry)
								{
									if((CvString)pkBuildingEntry->GetType() == "BUILDING_BAZAAR")
									{
										//OutputDebugString("\nStep2");
										if(GET_PLAYER(eAcceptedFromPlayer).getBuildingClassCount((BuildingClassTypes)pkBuildingEntry->GetBuildingClassType()) >= 1)
										{
											gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_TRADER);
										}

									}
								}
							}
						}
					}
				}
			}
#endif
		}
		// City
		else if(it->m_eItemType == TRADE_ITEM_CITIES)
		{
			CvCity* pCity = GC.getMap().plot(it->m_iData1, it->m_iData2)->getPlotCity();
			if(pCity != NULL)
#if defined(MOD_BALANCE_CORE)
			{
				//I've traded for? I don't want to give away again.
				pCity->SetTraded(eAcceptedToPlayer, true);
				bool bWar = kDeal.IsPeaceTreatyTrade(eAcceptedFromPlayer);
#endif
				GET_PLAYER(eAcceptedToPlayer).acquireCity(pCity, bWar, !bWar);
#if defined(MOD_BALANCE_CORE)
			}
#endif
		}
		else if(it->m_eItemType == TRADE_ITEM_ALLOW_EMBASSY)
		{
			GET_TEAM(eToTeam).SetHasEmbassyAtTeam(eFromTeam, true);
		}
		else if(it->m_eItemType == TRADE_ITEM_DECLARATION_OF_FRIENDSHIP)
		{
			// Declaration of friendship always goes both ways.  We will most likely have two entries in the deal for this
			// but just in case, set both anyway.
			GET_PLAYER(eAcceptedFromPlayer).GetDiplomacyAI()->SetDoFAccepted(eAcceptedToPlayer, true);
			GET_PLAYER(eAcceptedFromPlayer).GetDiplomacyAI()->SetDoFCounter(eAcceptedToPlayer, 0);
			GET_PLAYER(eAcceptedToPlayer).GetDiplomacyAI()->SetDoFAccepted(eAcceptedFromPlayer, true);
			GET_PLAYER(eAcceptedToPlayer).GetDiplomacyAI()->SetDoFCounter(eAcceptedFromPlayer, 0);
		}
		// Vote Commitment
		else if(it->m_eItemType == TRADE_ITEM_VOTE_COMMITMENT)
		{
			GET_PLAYER(eAcceptedFromPlayer).GetLeagueAI()->AddVoteCommitment(eAcceptedToPlayer, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1);
		}
		// Open Borders
		else if(it->m_eItemType == TRADE_ITEM_OPEN_BORDERS)
		{
			GET_TEAM(eFromTeam).SetAllowsOpenBordersToTeam(eToTeam, true);
		}
		// Defensive Pact
		else if(it->m_eItemType == TRADE_ITEM_DEFENSIVE_PACT)
		{
			GET_TEAM(eFromTeam).SetHasDefensivePact(eToTeam, true);
		}
		// Research Agreement
		else if(it->m_eItemType == TRADE_ITEM_RESEARCH_AGREEMENT)
		{
			GET_TEAM(eFromTeam).SetHasResearchAgreement(eToTeam, true);
			GET_PLAYER(eAcceptedFromPlayer).GetTreasury()->LogExpenditure(GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescription(), iCost, 9);

			if(!bSentResearchAgreementNotification)
			{
				bSentResearchAgreementNotification = true;
				GC.getGame().DoResearchAgreementNotification(eFromTeam, eToTeam);
			}
		}
		// Trade Agreement
		else if(it->m_eItemType == TRADE_ITEM_TRADE_AGREEMENT)
		{
			GET_TEAM(eFromTeam).SetHasTradeAgreement(eToTeam, true);
		}
		// Third Party Peace
		else if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_PEACE)
		{
			TeamTypes eTargetTeam = (TeamTypes) it->m_iData1;
			bool bTargetTeamIsMinor = GET_TEAM(eTargetTeam).isMinorCiv();
#if defined(MOD_EVENTS_WAR_AND_PEACE)
			GET_TEAM(eFromTeam).makePeace(eTargetTeam, /*bBumpUnits*/ true, /*bSuppressNotification*/ bTargetTeamIsMinor, eFromPlayer);
#else
			GET_TEAM(eFromTeam).makePeace(eTargetTeam, /*bBumpUnits*/ true, /*bSuppressNotification*/ bTargetTeamIsMinor);
#endif
			GET_TEAM(eFromTeam).setForcePeace(eTargetTeam, true);
			GET_TEAM(eTargetTeam).setForcePeace(eFromTeam, true);
			
			// Update diplo stuff.
			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				
				if (!GET_PLAYER(eLoopPlayer).isHuman() && GET_PLAYER(eLoopPlayer).isAlive())
				{
					if (GET_PLAYER(eLoopPlayer).getTeam() == eFromTeam || GET_PLAYER(eLoopPlayer).getTeam() == eTargetTeam)
					{
						vector<PlayerTypes> v;
						GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateOpinions();
						GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateMajorCivApproaches(v);
					}
				}
			}

			if(bTargetTeamIsMinor)
			{
				veNowAtPeacePairs.push_back(eTargetTeam, eFromTeam); //eFromTeam is second so we can take advantage of CvWeightedVector's sort by weights
			}
#if defined(MOD_BALANCE_CORE)
			PlayerTypes eThirdParty;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eTargetTeam)
				{
					//If human was target, let human know.
					if(GET_PLAYER(eLoopPlayer).isHuman())
					{
						CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
						if(pNotifications)
						{
							Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_PEACE_NAME");
							strText << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
							strText << GET_PLAYER(eAcceptedFromPlayer).getCivilizationShortDescriptionKey();
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_PEACE_NAME_S");
							strSummary << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
							pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
						}
					}
					//If AI, improve opinion of broker a bit.
					else if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
					{
						GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eAcceptedToPlayer, -300);
					}
					// Other players' reactions
					for (int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
					{
						eThirdParty = (PlayerTypes) iThirdPartyLoop;
						
						if (GET_PLAYER(eThirdParty).isMajorCiv() && GET_PLAYER(eThirdParty).isAlive() && !GET_PLAYER(eThirdParty).isHuman() && GET_PLAYER(eThirdParty).GetDiplomacyAI()->IsPlayerValid(eAcceptedToPlayer))
						{
							if (GET_PLAYER(eThirdParty).GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer) || GET_TEAM(GET_PLAYER(eThirdParty).getTeam()).IsHasDefensivePact(eTargetTeam))
							{
								GET_PLAYER(eThirdParty).GetDiplomacyAI()->ChangeRecentAssistValue(eAcceptedToPlayer, -300);
							}
						}
					}
				}
			}
#endif
		}
		// Third Party War
		else if (it->m_eItemType == TRADE_ITEM_THIRD_PARTY_WAR)
		{
			TeamTypes eTargetTeam = (TeamTypes)it->m_iData1;
			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				if (eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eFromTeam)
				{
					if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DeclareWar(eTargetTeam))
					{
						int iLockedTurns = /*15*/ GC.getCOOP_WAR_LOCKED_LENGTH();
						GET_TEAM(eFromTeam).ChangeNumTurnsLockedIntoWar(eTargetTeam, iLockedTurns);
#if defined(MOD_BALANCE_CORE)
						// Spies will detect third party war brokering
						bool bTargetTeamIsMinor = GET_TEAM(eTargetTeam).isMinorCiv();
						bool bAnySurveillanceEstablished = false;
						PlayerTypes eSpyingPlayer = NO_PLAYER;
						
						if (!bTargetTeamIsMinor)
						{
							for (int iSpyPlayerLoop = 0; iSpyPlayerLoop < MAX_MAJOR_CIVS; iSpyPlayerLoop++)
							{
								eSpyingPlayer = (PlayerTypes) iSpyPlayerLoop;
								
								if (!GET_PLAYER(eSpyingPlayer).GetEspionage())
									continue;
								
								if (GET_PLAYER(eSpyingPlayer).getTeam() == eTargetTeam || GET_TEAM(GET_PLAYER(eSpyingPlayer).getTeam()).IsHasDefensivePact(eTargetTeam))
								{
									if (GET_PLAYER(eSpyingPlayer).GetEspionage()->IsAnySurveillanceEstablished(eAcceptedToPlayer) || GET_PLAYER(eSpyingPlayer).GetEspionage()->IsAnySurveillanceEstablished(eAcceptedFromPlayer))
									{
										bAnySurveillanceEstablished = true;
										break;
									}
								}
							}
						}
						
						PlayerTypes eLoopPlayer;
						PlayerTypes eLoopPlayer2;
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							eLoopPlayer = (PlayerTypes)iPlayerLoop;

							if (eLoopPlayer == eAcceptedFromPlayer || eLoopPlayer == eAcceptedToPlayer)
								continue;

							if (eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eTargetTeam)
							{
								//AI go to war now.
								if (!GET_PLAYER(eAcceptedFromPlayer).isHuman())
								{
									GET_PLAYER(eAcceptedFromPlayer).GetMilitaryAI()->RequestBasicAttack(eLoopPlayer, 2);
									GET_PLAYER(eAcceptedFromPlayer).GetMilitaryAI()->RequestPureNavalAttack(eLoopPlayer, 2);
								}

								//If human attacked, send notification with info.
								if (GET_PLAYER(eLoopPlayer).isHuman() && bAnySurveillanceEstablished)
								{
									CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
									if (pNotifications)
									{
										Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_NAME");
										strText << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
										strText << GET_PLAYER(eAcceptedFromPlayer).getCivilizationShortDescriptionKey();
										Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_NAME_S");
										strSummary << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
										pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
									}
								}
								//If AI, bump down their opinion of the broker and the warrior a bit.
								else if (!bTargetTeamIsMinor && bAnySurveillanceEstablished)
								{
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eAcceptedToPlayer, 2);
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eAcceptedFromPlayer, 2);
								}
							}
							else if (!bTargetTeamIsMinor && GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsHasDefensivePact(eTargetTeam))
							{
								//If human attacked, send notification with info.
								if (GET_PLAYER(eLoopPlayer).isHuman() && bAnySurveillanceEstablished)
								{
									CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
									if (pNotifications)
									{
										Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_NAME");
										strText << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
										strText << GET_PLAYER(eAcceptedFromPlayer).getCivilizationShortDescriptionKey();
										Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_NAME_S");
										strSummary << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
										pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
									}
								}
								//If AI, bump down their opinion of the broker and the warrior a bit.
								else if (bAnySurveillanceEstablished)
								{
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eAcceptedToPlayer, 2);
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eAcceptedFromPlayer, 2);
								}
							}
							// If the brokering was detected, run a second loop for diplo purposes
							if (!bTargetTeamIsMinor && bAnySurveillanceEstablished && eSpyingPlayer != NO_PLAYER)
							{
								for (int iPlayerLoop2 = 0; iPlayerLoop2 < MAX_MAJOR_CIVS; iPlayerLoop2++)
								{
									eLoopPlayer2 = (PlayerTypes) iPlayerLoop2;
								
									if (!GET_PLAYER(eLoopPlayer2).isAlive() || !GET_PLAYER(eLoopPlayer2).isMajorCiv())
										continue;
									
									// All players who shared intel get a diplo bonus for it
									if (GET_PLAYER(eLoopPlayer2).getTeam() == GET_PLAYER(eLoopPlayer).getTeam() || GET_TEAM(GET_PLAYER(eLoopPlayer2).getTeam()).IsHasDefensivePact(GET_PLAYER(eLoopPlayer).getTeam()))
									{
										if (eLoopPlayer2 == eSpyingPlayer)
										{
											GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesIntrigueSharedBy(eLoopPlayer2, 1);
										}
										else if (GET_PLAYER(eLoopPlayer2).GetEspionage())
										{
											if (GET_PLAYER(eLoopPlayer2).GetEspionage()->IsAnySurveillanceEstablished(eAcceptedToPlayer) || GET_PLAYER(eLoopPlayer2).GetEspionage()->IsAnySurveillanceEstablished(eAcceptedFromPlayer))
											{
												GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesIntrigueSharedBy(eLoopPlayer2, 1);
											}
										}
									}
									// Penalty to recent assistance for friends of the target(s)
									else if (!GET_PLAYER(eLoopPlayer2).isHuman() && GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer))
									{
										if (GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->IsPlayerValid(eAcceptedToPlayer))
										{
											GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->ChangeRecentAssistValue(eAcceptedToPlayer, 300);
										}
										
										if (GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->IsPlayerValid(eAcceptedFromPlayer))
										{
											GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->ChangeRecentAssistValue(eAcceptedFromPlayer, 300);
										}
									}
								}
							}
						}
					}
				}
			}
#endif
		}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		// Maps
		else if (MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_MAPS)
		{
			GET_TEAM(eToTeam).AcquireMap(eFromTeam);
		}
		// Techs
		else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_TECHS)
		{
			TechTypes eTech = (TechTypes) it->m_iData1;

			GET_TEAM(eToTeam).setHasTech(eTech, true, NO_PLAYER, true, false);
					
			// If No Tech Brokering is enabled then don't let the player trade this tech
			if(GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING))
			{
				GET_TEAM(eToTeam).SetTradeTech(eTech, false);
			}
		}
		// Vassalage
		else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_VASSALAGE)
		{
			bool bCapitulation = kDeal.IsPeaceTreatyTrade(eFromPlayer) || kDeal.IsPeaceTreatyTrade(eToPlayer);
			GET_TEAM(eFromTeam).DoBecomeVassal(eToTeam, !bCapitulation);
		}
		// Revoke Vassalage
		else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_VASSALAGE_REVOKE)
		{
			PlayerTypes eLoopPlayer;
			TeamTypes eLoopTeam;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive())
				{
					eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();
					if(eLoopTeam != NO_TEAM)
					{
						GET_TEAM(eFromTeam).DoEndVassal(eLoopTeam, true, true);
					}
				}
			}
		}
#endif
		// **** Peace Treaty **** this should always be the last item processed!!!
		else if(it->m_eItemType == TRADE_ITEM_PEACE_TREATY)
		{
#if defined(MOD_BALANCE_CORE)
			if(MOD_BALANCE_CORE)
			{
				if (GET_PLAYER(eAcceptedToPlayer).GetDiplomacyAI()->GetWarScore(eAcceptedFromPlayer) >= 25 && !bDone)
				{
					GET_PLAYER(eAcceptedToPlayer).DoWarVictoryBonuses();
					bDone = true;
				}
				else if (GET_PLAYER(eAcceptedFromPlayer).GetDiplomacyAI()->GetWarScore(eAcceptedToPlayer) >= 25 && !bDone)
				{
					GET_PLAYER(eAcceptedFromPlayer).DoWarVictoryBonuses();
					bDone = true;
				}
			}

#if defined(MOD_EVENTS_WAR_AND_PEACE)
			GET_TEAM(eFromTeam).makePeace(eToTeam, true, false, eFromPlayer);
#else
			GET_TEAM(eFromTeam).makePeace(eToTeam);
#endif
			GET_TEAM(eFromTeam).setForcePeace(eToTeam, true);
#endif
		}
		//////////////////////////////////////////////////////////////////////
		// **** DO NOT PUT ANYTHING AFTER THIS LINE ****
		//////////////////////////////////////////////////////////////////////
	}

	LogDealComplete(&kDeal);
}

void CvGameDeals::FinalizeDealNotify(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvWeightedVector<TeamTypes, MAX_CIV_TEAMS, true>& veNowAtPeacePairs)
{
	// Update UI if we were involved in the deal
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
	if(eFromPlayer == eActivePlayer || eToPlayer == eActivePlayer)
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	// Send out a condensed notification if peace was made with third party minor civs in this deal
	if(veNowAtPeacePairs.size() > 0)
	{
		// Loop through all teams
		for(int iFromTeamIndex = 0; iFromTeamIndex < MAX_CIV_TEAMS; iFromTeamIndex++)
		{
			TeamTypes eFromTeam = (TeamTypes) iFromTeamIndex;
			TeamTypes eToTeam = NO_TEAM;
			bool bFromTeamMadePeace = false;

			Localization::String strTemp = Localization::Lookup("TXT_KEY_MISC_MADE_PEACE_WITH_MINOR_ALLIES");
			Localization::String strSummary = Localization::Lookup("TXT_KEY_MISC_MADE_PEACE_WITH_MINOR_ALLIES_SUMMARY");
			strTemp << GET_TEAM(eFromTeam).getName().GetCString();
			strSummary << GET_TEAM(eFromTeam).getName().GetCString();
			CvString strMessage = strTemp.toUTF8();

			// Did this team make peace with someone in this deal?
			for(int iPairIndex = 0; iPairIndex < veNowAtPeacePairs.size(); iPairIndex++)
			{
				if(veNowAtPeacePairs.GetWeight(iPairIndex) == (int) eFromTeam)
				{
					eToTeam = veNowAtPeacePairs.GetElement(iPairIndex);
					strTemp = Localization::Lookup(GET_TEAM(eToTeam).getName().GetCString());
					strMessage = strMessage + "[NEWLINE]" + strTemp.toUTF8();
					bFromTeamMadePeace = true;
				}
			}

			// Send out notifications if there was a change
			if(bFromTeamMadePeace)
			{
				// Send out the notifications to other players
				for(int iNotifPlayerLoop = 0; iNotifPlayerLoop < MAX_MAJOR_CIVS; iNotifPlayerLoop++)
				{
					PlayerTypes eNotifPlayer = (PlayerTypes) iNotifPlayerLoop;

					if(!GET_PLAYER(eNotifPlayer).isAlive())
						continue;

					if(GET_PLAYER(eNotifPlayer).getTeam() == eFromTeam)
						continue;

					if(GET_TEAM(GET_PLAYER(eNotifPlayer).getTeam()).isHasMet(eFromTeam))  //antonjs: consider: what if eNotifPlayer hasn't met one or more of the minors that eFromTeam made peace with?
					{
						if(GET_PLAYER(eNotifPlayer).GetNotifications())
						{
							GET_PLAYER(eNotifPlayer).GetNotifications()->Add(NOTIFICATION_PEACE, strMessage, strSummary.toUTF8(), -1, -1, GET_TEAM(eFromTeam).getLeaderID(), eToTeam);
						}
					}
				}
			}
		}
	}
}
#endif

/// Moves a deal from the proposed list to the active one (returns FALSE if deal not found)
bool CvGameDeals::FinalizeDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bAccepted)
{
	DealList::iterator dealIt;
	CvDeal kDeal;
	bool bFoundIt = false;
	bool bValid   = true;
	CvWeightedVector<TeamTypes, MAX_CIV_TEAMS, true> veNowAtPeacePairs; // hacked CvWeighedVector to keep track of third party minors that this deal makes at peace

	// Find the deal in the list of proposed deals
	for(dealIt = m_ProposedDeals.begin(); dealIt != m_ProposedDeals.end(); ++dealIt)
	{
		if(dealIt->m_eFromPlayer == eFromPlayer && dealIt->m_eToPlayer == eToPlayer)
		{
			kDeal = *dealIt;

// EFB: once we can use list containers in AutoVariables, go back to this way of deleting
//			m_ProposedDeals.erase(dealIt);
			bFoundIt = true;
		}
	}

	if(bFoundIt)
	{

		TradedItemList::iterator iter;
		for(iter = kDeal.m_TradedItems.begin(); iter != kDeal.m_TradedItems.end(); ++iter)
		{
			if(iter->m_bToRenewed)  // slewis - added exception in case of something that was renewed
			{
				continue;
			}

			if(!kDeal.IsPossibleToTradeItem(iter->m_eFromPlayer, kDeal.GetOtherPlayer(iter->m_eFromPlayer), iter->m_eItemType, iter->m_iData1, iter->m_iData2, iter->m_iData3, iter->m_bFlag1, false, true))
			{
				// mark that the deal is no longer valid. We will still delete the deal but not commit its actions
				bValid = false;
				break;
			}
		}


		// **** START HACK ****
		// EFB: temporary delete method; recopy vector without this element
		//
		// Copy the deals into a temporary container
		DealList tempDeals(m_ProposedDeals);

		// Copy back in minus this element
		m_ProposedDeals.clear();
		for(dealIt = tempDeals.begin(); dealIt != tempDeals.end(); ++dealIt)
		{
			if(dealIt->m_eFromPlayer != eFromPlayer || dealIt->m_eToPlayer != eToPlayer)
			{
				m_ProposedDeals.push_back(*dealIt);
			}
		}
		// **** END HACK ****

		if(bValid && bAccepted)
		{
			// Determine total duration of the Deal
			int iLatestItemLastTurn = 0;
			int iLongestDuration = 0;
			//int iTemp;

			TradedItemList::iterator it;
			for(it = kDeal.m_TradedItems.begin(); it != kDeal.m_TradedItems.end(); ++it)
			{
				CvAssertMsg(it->m_eFromPlayer == kDeal.m_eFromPlayer || it->m_eFromPlayer == kDeal.m_eToPlayer, "DEAL: Adding deal that has an item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

				// Calculate duration
				if(it->m_iDuration > 0)
				{
					it->m_iFinalTurn = it->m_iDuration + GC.getGame().getGameTurn();
					if(it->m_iDuration > iLongestDuration)
					{
						iLongestDuration = it->m_iDuration;
						iLatestItemLastTurn = it->m_iFinalTurn;
					}
				}
			}

			kDeal.m_iDuration = iLongestDuration;
			kDeal.m_iFinalTurn = iLatestItemLastTurn;
			kDeal.m_iStartTurn = GC.getGame().getGameTurn();

			// Add to current deals
			CvAssertMsg(kDeal.m_TradedItems.size() > 0, "New deal has no tradeable items!");
			m_CurrentDeals.push_back(kDeal);
			kDeal.m_iStartTurn = GC.getGame().getGameTurn();

			bool bSentResearchAgreementNotification = false;

			int iCost;

			// What effects does this Deal have right now?
			PlayerTypes eAcceptedFromPlayer;
			PlayerTypes eAcceptedToPlayer;
			TeamTypes eFromTeam;
			TeamTypes eToTeam;
#if defined(MOD_BALANCE_CORE)
			bool bDone = false;
			//Set surrendering player in human v. human war. Based on who puts what where.
			if(GET_PLAYER(eFromPlayer).isHuman() && GET_PLAYER(eToPlayer).isHuman())
			{
				if(kDeal.IsPeaceTreatyTrade(eFromPlayer) || kDeal.IsPeaceTreatyTrade(eToPlayer))
				{
					for(it = kDeal.m_TradedItems.begin(); it != kDeal.m_TradedItems.end(); ++it)
					{
						// if the deal is renewed do not start it up
						if(it->m_bToRenewed)
						{
							continue;
						}

						eAcceptedFromPlayer = it->m_eFromPlayer;
						eAcceptedToPlayer = kDeal.GetOtherPlayer(eAcceptedFromPlayer);
						eFromTeam = GET_PLAYER(eAcceptedFromPlayer).getTeam();
						eToTeam = GET_PLAYER(eAcceptedToPlayer).getTeam();

						CvAssertMsg(eAcceptedFromPlayer == kDeal.m_eFromPlayer || eAcceptedFromPlayer == kDeal.m_eToPlayer, "DEAL: Adding deal that has an item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					
						// Gold
						if(it->m_eItemType == TRADE_ITEM_GOLD)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
						// Gold Per Turn
						else if(it->m_eItemType == TRADE_ITEM_GOLD_PER_TURN)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
						// Resource
						else if(it->m_eItemType == TRADE_ITEM_RESOURCES)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
						// City
						else if(it->m_eItemType == TRADE_ITEM_CITIES)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
						else if(it->m_eItemType == TRADE_ITEM_ALLOW_EMBASSY)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;	
						}
						// Vote Commitment
						else if(it->m_eItemType == TRADE_ITEM_VOTE_COMMITMENT)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
						// Open Borders
						else if(it->m_eItemType == TRADE_ITEM_OPEN_BORDERS)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
						// Research Agreement
						else if(it->m_eItemType == TRADE_ITEM_RESEARCH_AGREEMENT)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
						// Maps
						else if (MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_MAPS)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
						// Techs
						else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_TECHS)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
						// Vassalage
						else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_VASSALAGE)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}
						// Revoke Vassalage
						else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_VASSALAGE_REVOKE)
						{
							kDeal.SetSurrenderingPlayer(it->m_eFromPlayer);
							break;
						}						
					}
				}
			}
#endif
#endif
			for(it = kDeal.m_TradedItems.begin(); it != kDeal.m_TradedItems.end(); ++it)
			{
				// if the deal is renewed do not start it up
				if(it->m_bToRenewed)
				{
					continue;
				}

				eAcceptedFromPlayer = it->m_eFromPlayer;
				eAcceptedToPlayer = kDeal.GetOtherPlayer(eAcceptedFromPlayer);
				eFromTeam = GET_PLAYER(eAcceptedFromPlayer).getTeam();
				eToTeam = GET_PLAYER(eAcceptedToPlayer).getTeam();

				CvAssertMsg(eAcceptedFromPlayer == kDeal.m_eFromPlayer || eAcceptedFromPlayer == kDeal.m_eToPlayer, "DEAL: Adding deal that has an item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

				// Deduct Gold cost (if applicable)
				iCost = GetTradeItemGoldCost(it->m_eItemType, eAcceptedFromPlayer, eAcceptedToPlayer);
				GET_PLAYER(eAcceptedFromPlayer).GetTreasury()->ChangeGold(-iCost);

				// Gold
				if(it->m_eItemType == TRADE_ITEM_GOLD)
				{
					int iGoldAmount = it->m_iData1;
					GET_PLAYER(eAcceptedFromPlayer).GetTreasury()->ChangeGold(-iGoldAmount);
					GET_PLAYER(eAcceptedToPlayer).GetTreasury()->ChangeGold(iGoldAmount);
				}
				// Gold Per Turn
				else if(it->m_eItemType == TRADE_ITEM_GOLD_PER_TURN)
				{
					int iGoldPerTurn = it->m_iData1;
					GET_PLAYER(eAcceptedFromPlayer).GetTreasury()->ChangeGoldPerTurnFromDiplomacy(-iGoldPerTurn);
					GET_PLAYER(eAcceptedToPlayer).GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iGoldPerTurn);
				}
				// Resource
				else if(it->m_eItemType == TRADE_ITEM_RESOURCES)
				{
					ResourceTypes eResource = (ResourceTypes) it->m_iData1;
					int iResourceQuantity = it->m_iData2;
					GET_PLAYER(eAcceptedFromPlayer).changeResourceExport(eResource, iResourceQuantity);
					GET_PLAYER(eAcceptedToPlayer).changeResourceImportFromMajor(eResource, iResourceQuantity);

#if !defined(NO_ACHIEVEMENTS)
					//Resource Trading Achievements
					if(!GC.getGame().isGameMultiPlayer())
					{
						if(GET_PLAYER(eAcceptedFromPlayer).isHuman())
						{
							gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_RESOURCESTRADED, 100, ACHIEVEMENT_100RESOURCES);

							if(GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY)
							{
								//OutputDebugString("\nStep0");
								if(((CvString)GET_PLAYER(eAcceptedFromPlayer).getLeaderTypeKey() == "LEADER_HARUN_AL_RASHID"))
								{
									//OutputDebugString("\nStep1");
									int iJ;
									for(iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
									{
										CvBuildingEntry* pkBuildingEntry = GC.getBuildingInfo((BuildingTypes)iJ);
										if(pkBuildingEntry)
										{
											if((CvString)pkBuildingEntry->GetType() == "BUILDING_BAZAAR")
											{
												//OutputDebugString("\nStep2");
												if(GET_PLAYER(eAcceptedFromPlayer).getBuildingClassCount((BuildingClassTypes)pkBuildingEntry->GetBuildingClassType()) >= 1)
												{
													gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_TRADER);
												}

											}
										}
									}
								}
							}
						}
					}
#endif
				}
				// City
				else if(it->m_eItemType == TRADE_ITEM_CITIES)
				{
					CvCity* pCity = GC.getMap().plot(it->m_iData1, it->m_iData2)->getPlotCity();
					if(pCity != NULL)
#if defined(MOD_BALANCE_CORE)
					{
						//I've traded for? I don't want to give away again.
						pCity->SetTraded(eAcceptedToPlayer, true);
#endif
						GET_PLAYER(eAcceptedToPlayer).acquireCity(pCity, false, true);
#if defined(MOD_BALANCE_CORE)
					}
#endif
				}
				else if(it->m_eItemType == TRADE_ITEM_ALLOW_EMBASSY)
				{
					GET_TEAM(eToTeam).SetHasEmbassyAtTeam(eFromTeam, true);
				}
				else if(it->m_eItemType == TRADE_ITEM_DECLARATION_OF_FRIENDSHIP)
				{
					// Declaration of friendship always goes both ways.  We will most likely have two entries in the deal for this
					// but just in case, set both anyway.
					GET_PLAYER(eAcceptedFromPlayer).GetDiplomacyAI()->SetDoFAccepted(eAcceptedToPlayer, true);
					GET_PLAYER(eAcceptedFromPlayer).GetDiplomacyAI()->SetDoFCounter(eAcceptedToPlayer, 0);
					GET_PLAYER(eAcceptedToPlayer).GetDiplomacyAI()->SetDoFAccepted(eAcceptedFromPlayer, true);
					GET_PLAYER(eAcceptedToPlayer).GetDiplomacyAI()->SetDoFCounter(eAcceptedFromPlayer, 0);
				}
				// Vote Commitment
				else if(it->m_eItemType == TRADE_ITEM_VOTE_COMMITMENT)
				{
					GET_PLAYER(eAcceptedFromPlayer).GetLeagueAI()->AddVoteCommitment(eAcceptedToPlayer, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1);
				}
				// Open Borders
				else if(it->m_eItemType == TRADE_ITEM_OPEN_BORDERS)
				{
					GET_TEAM(eFromTeam).SetAllowsOpenBordersToTeam(eToTeam, true);
				}
				// Defensive Pact
				else if(it->m_eItemType == TRADE_ITEM_DEFENSIVE_PACT)
				{
					GET_TEAM(eFromTeam).SetHasDefensivePact(eToTeam, true);
				}
				// Research Agreement
				else if(it->m_eItemType == TRADE_ITEM_RESEARCH_AGREEMENT)
				{
					GET_TEAM(eFromTeam).SetHasResearchAgreement(eToTeam, true);
					GET_PLAYER(eAcceptedFromPlayer).GetTreasury()->LogExpenditure(GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescription(), iCost, 9);

					if(!bSentResearchAgreementNotification)
					{
						bSentResearchAgreementNotification = true;
						GC.getGame().DoResearchAgreementNotification(eFromTeam, eToTeam);
					}
				}
				// Trade Agreement
				else if(it->m_eItemType == TRADE_ITEM_TRADE_AGREEMENT)
				{
					GET_TEAM(eFromTeam).SetHasTradeAgreement(eToTeam, true);
				}
				// Third Party Peace
				else if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_PEACE)
				{
					TeamTypes eTargetTeam = (TeamTypes) it->m_iData1;
					bool bTargetTeamIsMinor = GET_TEAM(eTargetTeam).isMinorCiv();
#if defined(MOD_EVENTS_WAR_AND_PEACE)
					GET_TEAM(eFromTeam).makePeace(eTargetTeam, /*bBumpUnits*/ true, /*bSuppressNotification*/ bTargetTeamIsMinor, eFromPlayer);
#else
					GET_TEAM(eFromTeam).makePeace(eTargetTeam, /*bBumpUnits*/ true, /*bSuppressNotification*/ bTargetTeamIsMinor);
#endif
					GET_TEAM(eFromTeam).setForcePeace(eTargetTeam, true);
					GET_TEAM(eTargetTeam).setForcePeace(eFromTeam, true);
					
					// Update diplo stuff.
					PlayerTypes eLoopPlayer;
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;
						
						if (!GET_PLAYER(eLoopPlayer).isHuman() && GET_PLAYER(eLoopPlayer).isAlive())
						{
							if (GET_PLAYER(eLoopPlayer).getTeam() == eFromTeam || GET_PLAYER(eLoopPlayer).getTeam() == eTargetTeam)
							{
								vector<PlayerTypes> v;
								GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateOpinions();
								GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateMajorCivApproaches(v);
							}
						}
					}

					if(bTargetTeamIsMinor)
					{
						veNowAtPeacePairs.push_back(eTargetTeam, eFromTeam); //eFromTeam is second so we can take advantage of CvWeightedVector's sort by weights
					}
#if defined(MOD_BALANCE_CORE)
					PlayerTypes eThirdParty;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;
						if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eTargetTeam)
						{
							//If human was target, let human know.
							if(GET_PLAYER(eLoopPlayer).isHuman())
							{
								CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
								if(pNotifications)
								{
									Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_PEACE_NAME");
									strText << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
									strText << GET_PLAYER(eAcceptedFromPlayer).getCivilizationShortDescriptionKey();
									Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_PEACE_NAME_S");
									strSummary << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
									pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
								}
							}
							//If AI, improve opinion of broker a bit.
							else if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
							{
								GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eAcceptedToPlayer, -300);
							}
							// Other players' reactions
							for (int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
							{
								eThirdParty = (PlayerTypes) iThirdPartyLoop;
								
								if (GET_PLAYER(eThirdParty).isMajorCiv() && GET_PLAYER(eThirdParty).isAlive() && !GET_PLAYER(eThirdParty).isHuman() && GET_PLAYER(eThirdParty).GetDiplomacyAI()->IsPlayerValid(eAcceptedToPlayer))
								{
									if (GET_PLAYER(eThirdParty).GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer) || GET_TEAM(GET_PLAYER(eThirdParty).getTeam()).IsHasDefensivePact(eTargetTeam))
									{
										GET_PLAYER(eThirdParty).GetDiplomacyAI()->ChangeRecentAssistValue(eAcceptedToPlayer, -300);
									}
								}
							}
						}
					}
#endif
				}
				// Third Party War
				else if(it->m_eItemType == TRADE_ITEM_THIRD_PARTY_WAR)
				{
					TeamTypes eTargetTeam = (TeamTypes) it->m_iData1;
#if defined(MOD_EVENTS_WAR_AND_PEACE)
					GET_TEAM(eFromTeam).declareWar(eTargetTeam, false, eFromPlayer);
#else
					GET_TEAM(eFromTeam).declareWar(eTargetTeam);
#endif
					int iLockedTurns = /*15*/ GC.getCOOP_WAR_LOCKED_LENGTH();
					GET_TEAM(eFromTeam).ChangeNumTurnsLockedIntoWar(eTargetTeam, iLockedTurns);
#if defined(MOD_BALANCE_CORE)
					GET_PLAYER(eAcceptedFromPlayer).GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NOW_AT_WAR);
					
					if(!CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActiveTeam() == eTargetTeam)
					{
						const char* strText = GET_PLAYER(eAcceptedFromPlayer).GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_DOW_ROOT, GET_TEAM(eTargetTeam).getLeaderID());
						gDLL->GameplayDiplomacyAILeaderMessage(eAcceptedFromPlayer, DIPLO_UI_STATE_AI_DECLARED_WAR, strText, LEADERHEAD_ANIM_DECLARE_WAR);
					}
					
					// Spies will detect third party war brokering
					bool bTargetTeamIsMinor = GET_TEAM(eTargetTeam).isMinorCiv();
					bool bAnySurveillanceEstablished = false;
					PlayerTypes eSpyingPlayer = NO_PLAYER;
					
					if (!bTargetTeamIsMinor)
					{
						for (int iSpyPlayerLoop = 0; iSpyPlayerLoop < MAX_MAJOR_CIVS; iSpyPlayerLoop++)
						{
							eSpyingPlayer = (PlayerTypes) iSpyPlayerLoop;
							
							if (!GET_PLAYER(eSpyingPlayer).GetEspionage())
								continue;
							
							if (GET_PLAYER(eSpyingPlayer).getTeam() == eTargetTeam || GET_TEAM(GET_PLAYER(eSpyingPlayer).getTeam()).IsHasDefensivePact(eTargetTeam))
							{
								if (GET_PLAYER(eSpyingPlayer).GetEspionage()->IsAnySurveillanceEstablished(eAcceptedToPlayer) || GET_PLAYER(eSpyingPlayer).GetEspionage()->IsAnySurveillanceEstablished(eAcceptedFromPlayer))
								{
									bAnySurveillanceEstablished = true;
									break;
								}
							}
						}
					}

					PlayerTypes eLoopPlayer;
					PlayerTypes eLoopPlayer2;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;

						if (eLoopPlayer == eAcceptedFromPlayer || eLoopPlayer == eAcceptedToPlayer)
							continue;

						if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eTargetTeam)
						{
							if (eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eTargetTeam)
							{
								//AI go to war now.
								if (!GET_PLAYER(eAcceptedFromPlayer).isHuman())
								{
									GET_PLAYER(eAcceptedFromPlayer).GetMilitaryAI()->RequestBasicAttack(eLoopPlayer, 2);
									GET_PLAYER(eAcceptedFromPlayer).GetMilitaryAI()->RequestPureNavalAttack(eLoopPlayer, 2);
								}

								//If human attacked, send notification with info.
								if (GET_PLAYER(eLoopPlayer).isHuman() && bAnySurveillanceEstablished)
								{
									CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
									if (pNotifications)
									{
										Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_NAME");
										strText << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
										strText << GET_PLAYER(eAcceptedFromPlayer).getCivilizationShortDescriptionKey();
										Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_NAME_S");
										strSummary << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
										pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
									}
								}
								//If AI, bump down their opinion of the broker and the warrior a bit.
								else if (!bTargetTeamIsMinor && bAnySurveillanceEstablished)
								{
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eAcceptedToPlayer, 2);
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eAcceptedFromPlayer, 2);
								}
							}
							else if (!bTargetTeamIsMinor && GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsHasDefensivePact(eTargetTeam))
							{
								//If human attacked, send notification with info.
								if (GET_PLAYER(eLoopPlayer).isHuman() && bAnySurveillanceEstablished)
								{
									CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
									if (pNotifications)
									{
										Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_NAME");
										strText << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
										strText << GET_PLAYER(eAcceptedFromPlayer).getCivilizationShortDescriptionKey();
										Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_NAME_S");
										strSummary << GET_PLAYER(eAcceptedToPlayer).getCivilizationShortDescriptionKey();
										pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
									}
								}
								//If AI, bump down their opinion of the broker and the warrior a bit.
								else if (bAnySurveillanceEstablished)
								{
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eAcceptedToPlayer, 2);
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eAcceptedFromPlayer, 2);
								}
							}
							// If the brokering was detected, run a second loop for diplo purposes
							if (!bTargetTeamIsMinor && bAnySurveillanceEstablished && eSpyingPlayer != NO_PLAYER)
							{
								for (int iPlayerLoop2 = 0; iPlayerLoop2 < MAX_MAJOR_CIVS; iPlayerLoop2++)
								{
									eLoopPlayer2 = (PlayerTypes) iPlayerLoop2;
								
									if (!GET_PLAYER(eLoopPlayer2).isAlive() || !GET_PLAYER(eLoopPlayer2).isMajorCiv())
										continue;
									
									// Make sure all players who shared intel get the intrigue bonus
									if (GET_PLAYER(eLoopPlayer2).getTeam() == GET_PLAYER(eLoopPlayer).getTeam() || GET_TEAM(GET_PLAYER(eLoopPlayer2).getTeam()).IsHasDefensivePact(GET_PLAYER(eLoopPlayer).getTeam()))
									{
										if (eLoopPlayer2 == eSpyingPlayer)
										{
											GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesIntrigueSharedBy(eLoopPlayer2, 1);
										}
										else if (GET_PLAYER(eLoopPlayer2).GetEspionage())
										{
											if (GET_PLAYER(eLoopPlayer2).GetEspionage()->IsAnySurveillanceEstablished(eAcceptedToPlayer) || GET_PLAYER(eLoopPlayer2).GetEspionage()->IsAnySurveillanceEstablished(eAcceptedFromPlayer))
											{
												GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesIntrigueSharedBy(eLoopPlayer2, 1);
											}
										}
									}
									// Penalty to recent assistance for friends of the target(s)
									else if (!GET_PLAYER(eLoopPlayer2).isHuman() && GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer))
									{
										if (GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->IsPlayerValid(eAcceptedToPlayer))
										{
											GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->ChangeRecentAssistValue(eAcceptedToPlayer, 300);
										}
										
										if (GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->IsPlayerValid(eAcceptedFromPlayer))
										{
											GET_PLAYER(eLoopPlayer2).GetDiplomacyAI()->ChangeRecentAssistValue(eAcceptedFromPlayer, 300);
										}
									}
								}
							}
						}
					}
#endif
				}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
				// Maps
				else if (MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_MAPS)
				{
					GET_TEAM(eToTeam).AcquireMap(eFromTeam);
				}
				// Techs
				else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_TECHS)
				{
					TechTypes eTech = (TechTypes) it->m_iData1;

					GET_TEAM(eToTeam).setHasTech(eTech, true, NO_PLAYER, true, false);
					
					// If No Tech Brokering is enabled then don't let the player trade this tech
					if(GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING))
					{
						GET_TEAM(eToTeam).SetTradeTech(eTech, false);
					}
				}
				// Vassalage
				else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_VASSALAGE)
				{
					bool bCapitulation = kDeal.IsPeaceTreatyTrade(eFromPlayer) || kDeal.IsPeaceTreatyTrade(eToPlayer);
					GET_TEAM(eFromTeam).DoBecomeVassal(eToTeam, !bCapitulation);
				}
				// Revoke Vassalage
				else if(MOD_DIPLOMACY_CIV4_FEATURES && it->m_eItemType == TRADE_ITEM_VASSALAGE_REVOKE)
				{
					PlayerTypes eLoopPlayer;
					TeamTypes eLoopTeam;
					for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;

						if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive())
						{
							eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();
							if(eLoopTeam != NO_TEAM)
							{
								GET_TEAM(eFromTeam).DoEndVassal(eLoopTeam, true, true);
							}
						}
					}
				}
#endif
				// **** Peace Treaty **** this should always be the last item processed!!!
				else if(it->m_eItemType == TRADE_ITEM_PEACE_TREATY)
				{
#if defined(MOD_BALANCE_CORE)
					if(MOD_BALANCE_CORE)
					{
						if (GET_PLAYER(eAcceptedToPlayer).GetDiplomacyAI()->GetWarScore(eAcceptedFromPlayer) >= 25 && !bDone)
						{
							GET_PLAYER(eAcceptedToPlayer).DoWarVictoryBonuses();
							bDone = true;
						}
						else if (GET_PLAYER(eAcceptedFromPlayer).GetDiplomacyAI()->GetWarScore(eAcceptedToPlayer) >= 25 && !bDone)
						{
							GET_PLAYER(eAcceptedFromPlayer).DoWarVictoryBonuses();
							bDone = true;
						}
					}
#endif
#if defined(MOD_EVENTS_WAR_AND_PEACE)
					GET_TEAM(eFromTeam).makePeace(eToTeam, true, false, eFromPlayer);
#else
					GET_TEAM(eFromTeam).makePeace(eToTeam);
#endif
					GET_TEAM(eFromTeam).setForcePeace(eToTeam, true);
					
					// Update diplo stuff.
					PlayerTypes eLoopPlayer;
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						eLoopPlayer = (PlayerTypes) iPlayerLoop;
						
						if (!GET_PLAYER(eLoopPlayer).isHuman() && GET_PLAYER(eLoopPlayer).isAlive())
						{
							if (GET_PLAYER(eLoopPlayer).getTeam() == eFromTeam || GET_PLAYER(eLoopPlayer).getTeam() == eToTeam)
							{
								vector<PlayerTypes> v;
								GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateOpinions();
								GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoUpdateMajorCivApproaches(v);
							}
						}
					}
				}
				//////////////////////////////////////////////////////////////////////
				// **** DO NOT PUT ANYTHING AFTER THIS LINE ****
				//////////////////////////////////////////////////////////////////////
			}

			LogDealComplete(&kDeal);
		}
	}

	// Update UI if we were involved in the deal
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
	if(eFromPlayer == eActivePlayer || eToPlayer == eActivePlayer)
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	//update happiness.
	GET_PLAYER(eFromPlayer).CalculateNetHappiness();
	GET_PLAYER(eToPlayer).CalculateNetHappiness();

	// Send out a condensed notification if peace was made with third party minor civs in this deal
	if(veNowAtPeacePairs.size() > 0)
	{
		// Loop through all teams
		for(int iFromTeamIndex = 0; iFromTeamIndex < MAX_CIV_TEAMS; iFromTeamIndex++)
		{
			TeamTypes eFromTeam = (TeamTypes) iFromTeamIndex;
			TeamTypes eToTeam = NO_TEAM;
			bool bFromTeamMadePeace = false;

			Localization::String strTemp = Localization::Lookup("TXT_KEY_MISC_MADE_PEACE_WITH_MINOR_ALLIES");
			Localization::String strSummary = Localization::Lookup("TXT_KEY_MISC_MADE_PEACE_WITH_MINOR_ALLIES_SUMMARY");
			strTemp << GET_TEAM(eFromTeam).getName().GetCString();
			strSummary << GET_TEAM(eFromTeam).getName().GetCString();
			CvString strMessage = strTemp.toUTF8();

			// Did this team make peace with someone in this deal?
			for(int iPairIndex = 0; iPairIndex < veNowAtPeacePairs.size(); iPairIndex++)
			{
				if(veNowAtPeacePairs.GetWeight(iPairIndex) == (int) eFromTeam)
				{
					eToTeam = veNowAtPeacePairs.GetElement(iPairIndex);
					strTemp = Localization::Lookup(GET_TEAM(eToTeam).getName().GetCString());
					strMessage = strMessage + "[NEWLINE]" + strTemp.toUTF8();
					bFromTeamMadePeace = true;
				}
			}

			// Send out notifications if there was a change
			if(bFromTeamMadePeace)
			{
				// Send out the notifications to other players
				for(int iNotifPlayerLoop = 0; iNotifPlayerLoop < MAX_MAJOR_CIVS; iNotifPlayerLoop++)
				{
					PlayerTypes eNotifPlayer = (PlayerTypes) iNotifPlayerLoop;

					if(!GET_PLAYER(eNotifPlayer).isAlive())
						continue;

					if(GET_PLAYER(eNotifPlayer).getTeam() == eFromTeam)
						continue;

					if(GET_TEAM(GET_PLAYER(eNotifPlayer).getTeam()).isHasMet(eFromTeam))  //antonjs: consider: what if eNotifPlayer hasn't met one or more of the minors that eFromTeam made peace with?
					{
						if(GET_PLAYER(eNotifPlayer).GetNotifications())
						{
							GET_PLAYER(eNotifPlayer).GetNotifications()->Add(NOTIFICATION_PEACE, strMessage, strSummary.toUTF8(), -1, -1, GET_TEAM(eFromTeam).getLeaderID(), eToTeam);
						}
					}
				}
			}
		}
	}

	return bFoundIt && bValid;
}

CvDeal* CvGameDeals::GetTempDeal()
{
	return &m_TempDeal;
}

void CvGameDeals::SetTempDeal(CvDeal* pDeal)
{
	m_TempDeal = *pDeal;
}

/// Update deals for the start of a new turn
void CvGameDeals::DoTurn()
{
	DealList::iterator it;

	if(m_CurrentDeals.size() > 0)
	{
		PlayerTypes eFromPlayer = NO_PLAYER;
		PlayerTypes eToPlayer = NO_PLAYER;

		bool bSomethingChanged = false;
		//int iTemp;
		//TradeableItems eTempItem;

		int iGameTurn = GC.getGame().getGameTurn();

		// Check to see if any of our TradeItems in any of our Deals expire this turn
		for(it = m_CurrentDeals.begin(); it != m_CurrentDeals.end(); ++it)
		{
			TradedItemList::iterator itemIter;
			for(itemIter = it->m_TradedItems.begin(); itemIter != it->m_TradedItems.end(); ++itemIter)
			{
				int iFinalTurn = itemIter->m_iFinalTurn;
				CvAssertMsg(iFinalTurn >= -1, "DEAL: Trade item has a negative final turn.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
				CvAssertMsg(iFinalTurn < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a final turn way beyond the end of the game.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
				CvAssertMsg(itemIter->m_iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
				CvAssertMsg(itemIter->m_eFromPlayer == it->m_eFromPlayer || itemIter->m_eFromPlayer == it->m_eToPlayer, "DEAL: Processing turn for a deal that has an item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

				if(iFinalTurn > -1 && iFinalTurn == iGameTurn)
				{
					//eTempItem = itemIter->m_eItemType;
					//iTemp = iFinalTurn;

					bSomethingChanged = true;

					eFromPlayer = itemIter->m_eFromPlayer;
					eToPlayer = it->GetOtherPlayer(eFromPlayer);

					DoEndTradedItem(&*itemIter, eToPlayer, false);
				}
			}
		}

		// check to see if one of our deals in no longer valid
		for(it = m_CurrentDeals.begin(); it != m_CurrentDeals.end(); ++it)
		{
			TradedItemList::iterator itemIter;
			bool bInvalidDeal = false;
			bool bUnbreakable = false;
			for(itemIter = it->m_TradedItems.begin(); itemIter != it->m_TradedItems.end(); ++itemIter)
			{
				int iFinalTurn = itemIter->m_iFinalTurn;
				if(iFinalTurn > -1 && iFinalTurn != iGameTurn)  // if this was the last turn the deal was ending anyways
				{
					eFromPlayer = itemIter->m_eFromPlayer;
					// check to see if we are negative on resource or gold
					bool bHaveEnoughGold = true;
					bool bHaveEnoughResource = true;
					//if (itemIter->m_eItemType == TRADE_ITEM_GOLD_PER_TURN)
					//{
					//	int iGoldPerTurn = itemIter->m_iData1;
					//	bHaveEnoughGold = GET_PLAYER(eFromPlayer).GetTreasury()->GetGold() > iGoldPerTurn;
					//}
					// Resource
					/*else */
					if(itemIter->m_eItemType == TRADE_ITEM_RESOURCES)
					{
						ResourceTypes eResource = (ResourceTypes) itemIter->m_iData1;
						//int iResourceQuantity = itemIter->m_iData2;
						bHaveEnoughResource = GET_PLAYER(eFromPlayer).getNumResourceTotal(eResource) >= 0;
					}
					else if(itemIter->m_eItemType == TRADE_ITEM_PEACE_TREATY)
					{
						bUnbreakable = true;
						break;
					}

					if(!bHaveEnoughGold || !bHaveEnoughResource)
					{
						bInvalidDeal = true;
					}
				}
			}

			if(!bUnbreakable && bInvalidDeal)
			{
				bSomethingChanged = true;
				it->m_iFinalTurn = iGameTurn;
				it->m_bDealCancelled = true;

				for(itemIter = it->m_TradedItems.begin(); itemIter != it->m_TradedItems.end(); ++itemIter)
				{
					// Cancel individual items
					itemIter->m_iFinalTurn = GC.getGame().getGameTurn();

					eFromPlayer = itemIter->m_eFromPlayer;
					eToPlayer = it->GetOtherPlayer(eFromPlayer);

					DoEndTradedItem(&*itemIter, eToPlayer, true);
				}
			}
		}

		if(bSomethingChanged)
		{
			// Update UI if we were involved in the deal
			PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
			if(eFromPlayer == eActivePlayer || eToPlayer == eActivePlayer)
			{
				GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
			}
		}

		DoUpdateCurrentDealsList();
	}
}

PlayerTypes CvGameDeals::HasMadeProposal(PlayerTypes ePlayer)
{
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		for (DealList::const_iterator it = m_ProposedDeals.begin(); it != m_ProposedDeals.end(); ++it)
		{
		if (it->GetFromPlayer() == ePlayer)
			return it->GetToPlayer();		
		}
	}
	else
	{
		if(m_ProposedDeals.size() > 0)
		{
			DealList::iterator iter;
			for(iter = m_ProposedDeals.begin(); iter != m_ProposedDeals.end(); ++iter)
				if(iter->m_eFromPlayer == ePlayer)
					return iter->m_eToPlayer;
		}
	}
#else
	if(m_ProposedDeals.size() > 0)
	{
		DealList::iterator iter;
		for(iter = m_ProposedDeals.begin(); iter != m_ProposedDeals.end(); ++iter)
			if(iter->m_eFromPlayer == ePlayer)
				return iter->m_eToPlayer;
	}
#endif
	return NO_PLAYER;
}

bool CvGameDeals::ProposedDealExists(PlayerTypes eFromPlayer, PlayerTypes eToPlayer)
{
	if(m_ProposedDeals.size() > 0)
	{
		DealList::iterator iter;
		for(iter = m_ProposedDeals.begin(); iter != m_ProposedDeals.end(); ++iter)
		{
			if(iter->m_eFromPlayer == eFromPlayer && iter->m_eToPlayer == eToPlayer)
				return true;
		}
	}

	return false;
}

#if defined(MOD_ACTIVE_DIPLOMACY)
CvDeal* CvGameDeals::GetProposedMPDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool latest)
{
	int start = latest ? m_ProposedDeals.size() - 1 : 0;
	int end = latest ? -1 : m_ProposedDeals.size();
	int inc = latest ? -1 : 1;
	for (int i = start; i != end; i += inc)
	{
		CvDeal* pDeal = &m_ProposedDeals[i];
		if (pDeal->GetFromPlayer() == eFromPlayer && pDeal->GetToPlayer() == eToPlayer)
			return pDeal;
		else if (pDeal->GetFromPlayer() == eToPlayer && pDeal->GetToPlayer() == eFromPlayer)
			return pDeal;
	}
	return NULL;
}
#endif
CvDeal* CvGameDeals::GetProposedDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer)
{
	if(m_ProposedDeals.size() > 0)
	{
		DealList::iterator iter;
		for(iter = m_ProposedDeals.begin(); iter != m_ProposedDeals.end(); ++iter)
			if(iter->m_eFromPlayer == eFromPlayer && iter->m_eToPlayer == eToPlayer)
				return &(*iter);
	}
	return NULL;
}

/// If a deal has actually ended, move it from the current list to the historic list
void CvGameDeals::DoUpdateCurrentDealsList()
{
	DealList::iterator it;

	DealList tempDeals;

	// Copy the deals into a temporary container
	for(it = m_CurrentDeals.begin(); it != m_CurrentDeals.end(); ++it)
	{
		tempDeals.push_back(*it);
	}

	// Copy them to either current or historical deals based on whether or not they
	// are still active
	m_CurrentDeals.clear();
	for(it = tempDeals.begin(); it != tempDeals.end(); ++it)
	{
#if defined(MOD_BALANCE_CORE)
		if(it->m_iFinalTurn < 0)
			continue;
#endif
		if(it->m_iFinalTurn <= GC.getGame().getGameTurn())
		{
			m_HistoricalDeals.push_back(*it);
		}
		else
		{
			m_CurrentDeals.push_back(*it);
		}
	}
}

/// Deals between these two teams were interrupted (war or something)
void CvGameDeals::DoCancelDealsBetweenTeams(TeamTypes eTeam1, TeamTypes eTeam2)
{
	if(m_CurrentDeals.size() > 0)
	{
		PlayerTypes eFromPlayer, eToPlayer;
		int iPlayerLoop1, iPlayerLoop2;

		// Loop through first set of players
		for(iPlayerLoop1 = 0; iPlayerLoop1 < MAX_MAJOR_CIVS; iPlayerLoop1++)
		{
			eFromPlayer = (PlayerTypes) iPlayerLoop1;

			if(!GET_PLAYER(eFromPlayer).isEverAlive())
			{
				continue;
			}
			if(GET_PLAYER(eFromPlayer).getTeam() != eTeam1)
			{
				continue;
			}

			// Loop through second set of players
			for(iPlayerLoop2 = 0; iPlayerLoop2 < MAX_MAJOR_CIVS; iPlayerLoop2++)
			{
				eToPlayer = (PlayerTypes) iPlayerLoop2;

				if(!GET_PLAYER(eToPlayer).isEverAlive())
				{
					continue;
				}
				if(GET_PLAYER(eToPlayer).getTeam() != eTeam2)
				{
					continue;
				}

				DoCancelDealsBetweenPlayers(eFromPlayer, eToPlayer);
			}
		}
	}
}

/// Deals between these two Players were interrupted (death)
void CvGameDeals::DoCancelDealsBetweenPlayers(PlayerTypes eFromPlayer, PlayerTypes eToPlayer)
{
	DealList::iterator it;
	DealList tempDeals;

	if(m_CurrentDeals.size() > 0)
	{
		bool bSomethingChanged = false;

		// Copy the deals into a temporary container
		for(it = m_CurrentDeals.begin(); it != m_CurrentDeals.end(); ++it)
		{
			tempDeals.push_back(*it);
		}

		m_CurrentDeals.clear();
		for(it = tempDeals.begin(); it != tempDeals.end(); ++it)
		{
			// Players on this deal match?
			if(it->m_eFromPlayer == eFromPlayer && it->m_eToPlayer == eToPlayer ||
			        it->m_eFromPlayer == eToPlayer && it->m_eToPlayer == eFromPlayer)
			{
				// Change final turn
				it->m_iFinalTurn = GC.getGame().getGameTurn();

				// Cancel individual items
				TradedItemList::iterator itemIter;
				for(itemIter = it->m_TradedItems.begin(); itemIter != it->m_TradedItems.end(); ++itemIter)
				{
					bSomethingChanged = true;

					itemIter->m_iFinalTurn = GC.getGame().getGameTurn();

					eFromPlayer = itemIter->m_eFromPlayer;
					eToPlayer = it->GetOtherPlayer(eFromPlayer);

					DoEndTradedItem(&*itemIter, eToPlayer, true);
				}
				m_HistoricalDeals.push_back(*it);
			}
			else
			{
				m_CurrentDeals.push_back(*it);
			}
		}

		if(bSomethingChanged)
		{
			// Update UI if we were involved in the deal
			PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
			if(eFromPlayer == eActivePlayer || eToPlayer == eActivePlayer)
			{
				GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
			}
		}
	}
}

/// End EVERYONE's deals with eCancelPlayer (typically upon death)
void CvGameDeals::DoCancelAllDealsWithPlayer(PlayerTypes eCancelPlayer)
{
	CvTeam* pCancelTeam = &GET_TEAM(GET_PLAYER(eCancelPlayer).getTeam());

	// Loop through first set of players
	TeamTypes eTeam;
	PlayerTypes ePlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(!GET_PLAYER(ePlayer).isEverAlive())
		{
			continue;
		}

		if(ePlayer == eCancelPlayer)
		{
			continue;
		}

		eTeam = GET_PLAYER(ePlayer).getTeam();

		if(pCancelTeam->isHasMet(eTeam))
		{
			DoCancelDealsBetweenPlayers(eCancelPlayer, ePlayer);
		}
	}
}

#if defined(MOD_ACTIVE_DIPLOMACY)
// JdH => added eTargetPlayers parameter to be able to control proposed deal removal
void CvGameDeals::DoCancelAllProposedMPDealsWithPlayer(PlayerTypes eCancelPlayer, DiplomacyPlayerType eTargetPlayers)
{//Cancel all proposed deals involving eCancelPlayer.
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		CvPlayer& kLoopPlayer = GET_PLAYER(eLoopPlayer);
		if ((eTargetPlayers == DIPLO_AI_PLAYERS && !kLoopPlayer.isHuman()) || (eTargetPlayers == DIPLO_ALL_PLAYERS) || (eLoopPlayer == static_cast<PlayerTypes>(eTargetPlayers)))
		{
			CvDeal kDeal;
			while (RemoveProposedDeal(eCancelPlayer, eLoopPlayer, &kDeal, true))
			{//deal from eCancelPlayer
				FinalizeMPDeal(kDeal, false);
			}
			while (RemoveProposedDeal(eLoopPlayer, eCancelPlayer, &kDeal, true))
			{//deal to eCancelPlayer
				FinalizeMPDeal(kDeal, false);
			}
		}
	}
}
#endif

void CvGameDeals::DoCancelAllProposedDealsWithPlayer(PlayerTypes eCancelPlayer)
{//Cancel all proposed deals involving eCancelPlayer.
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GetProposedDeal(eCancelPlayer, eLoopPlayer))
		{//deal from eCancelPlayer
			FinalizeDeal(eCancelPlayer, eLoopPlayer, false);
		}
		if(GetProposedDeal(eLoopPlayer, eCancelPlayer))
		{//deal to eCancelPlayer
			FinalizeDeal(eLoopPlayer, eCancelPlayer, false);
		}
	}
}

/// End a TradedItem (if it's an ongoing item)
void CvGameDeals::DoEndTradedItem(CvTradedItem* pItem, PlayerTypes eToPlayer, bool bCancelled)
{
	CvString strBuffer;
	CvString strSummary;

	PlayerTypes eFromPlayer = pItem->m_eFromPlayer;

	CvPlayerAI& fromPlayer = GET_PLAYER(eFromPlayer);
	CvPlayerAI& toPlayer = GET_PLAYER(eToPlayer);

	TeamTypes eFromTeam = fromPlayer.getTeam();
	TeamTypes eToTeam = toPlayer.getTeam();

	CvNotifications* pNotifications = NULL;

	pItem->m_bToRenewed = false; // if this item is properly ended, then don't have it marked with "to renew"

	if(pItem->m_bFromRenewed)
	{
		return;
	}

	// Gold Per Turn
	if(pItem->m_eItemType == TRADE_ITEM_GOLD_PER_TURN)
	{
		int iGoldPerTurn = pItem->m_iData1;
		fromPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iGoldPerTurn);
		toPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(-iGoldPerTurn);

		pNotifications = GET_PLAYER(eFromPlayer).GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_GPT_FROM_US", toPlayer.getNameKey());
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_GPT_FROM_US", toPlayer.getNameKey());
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_GPT, strBuffer, strSummary, -1, -1, -1);
		}

		pNotifications = toPlayer.GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_GPT_TO_US", fromPlayer.getNameKey());
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_GPT_TO_US", fromPlayer.getNameKey());
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_GPT, strBuffer, strSummary, -1, -1, -1);
		}
	}
	// Resource
	else if(pItem->m_eItemType == TRADE_ITEM_RESOURCES)
	{
		ResourceTypes eResource = (ResourceTypes) pItem->m_iData1;
		int iResourceQuantity = pItem->m_iData2;

		fromPlayer.changeResourceExport(eResource, -iResourceQuantity);
		toPlayer.changeResourceImportFromMajor(eResource, -iResourceQuantity);

		CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		const char* szResourceDescription = (pkResourceInfo)? pkResourceInfo->GetDescriptionKey() : "";

		pNotifications = fromPlayer.GetNotifications();
		if(pNotifications)
		{

			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_RESOURCE_FROM_US", toPlayer.getNameKey(), szResourceDescription);
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_RESOURCE_FROM_US", toPlayer.getNameKey(), szResourceDescription);
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_RESOURCE, strBuffer, strSummary, -1, -1, -1);
		}

		pNotifications = toPlayer.GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_RESOURCE_TO_US", fromPlayer.getNameKey(), szResourceDescription);
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_RESOURCE_TO_US", fromPlayer.getNameKey(), szResourceDescription);
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_RESOURCE, strBuffer, strSummary, -1, -1, -1);
		}
	}
	// Open Borders
	else if(pItem->m_eItemType == TRADE_ITEM_OPEN_BORDERS)
	{
		GET_TEAM(eFromTeam).SetAllowsOpenBordersToTeam(eToTeam, false);

		pNotifications = fromPlayer.GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_OPEN_BORDERS_FROM_US", toPlayer.getNameKey());
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_OPEN_BORDERS_FROM_US", toPlayer.getNameKey());
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_OPEN_BORDERS, strBuffer, strSummary, -1, -1, -1);
		}

		pNotifications = toPlayer.GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_OPEN_BORDERS_TO_US", fromPlayer.getNameKey());
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_OPEN_BORDERS_TO_US", fromPlayer.getNameKey());
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_OPEN_BORDERS, strBuffer, strSummary, -1, -1, -1);
		}
	}
	// Defensive Pact
	else if(pItem->m_eItemType == TRADE_ITEM_DEFENSIVE_PACT)
	{
		GET_TEAM(eFromTeam).SetHasDefensivePact(eToTeam, false);

		pNotifications = fromPlayer.GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_DEFENSIVE_PACT_FROM_US", toPlayer.getNameKey());
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_DEFENSIVE_PACT_FROM_US", toPlayer.getNameKey());
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_DEFENSIVE_PACT, strBuffer, strSummary, -1, -1, -1);
		}

		pNotifications = toPlayer.GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_DEFENSIVE_PACT_TO_US", fromPlayer.getNameKey());
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_DEFENSIVE_PACT_TO_US", fromPlayer.getNameKey());
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_DEFENSIVE_PACT, strBuffer, strSummary, -1, -1, -1);
		}
	}
	// Research Agreement
	else if(pItem->m_eItemType == TRADE_ITEM_RESEARCH_AGREEMENT)
	{
		GET_TEAM(eFromTeam).SetHasResearchAgreement(eToTeam, false);

		if(!GET_TEAM(eFromTeam).isAtWar(eToTeam) && !bCancelled)
		{
			// Beaker boost = ((sum of both players' beakers over term of RA) / 2) / 3) * (median tech percentage rate)
			CvTeam& kTeam = GET_TEAM(toPlayer.getTeam());
			int iToPlayerBeakers = toPlayer.GetResearchAgreementCounter(eFromPlayer);
			int iFromPlayerBeakers = fromPlayer.GetResearchAgreementCounter(eToPlayer);
			int iBeakersBonus = min(iToPlayerBeakers, iFromPlayerBeakers) / GC.getRESEARCH_AGREEMENT_BOOST_DIVISOR(); //one (third) of minimum contribution
			iBeakersBonus = (iBeakersBonus * toPlayer.GetMedianTechPercentage()) / 100;

			TechTypes eCurrentTech = toPlayer.GetPlayerTechs()->GetCurrentResearch();
			if(eCurrentTech == NO_TECH)
			{
				toPlayer.changeOverflowResearch(iBeakersBonus);
			}
			else
			{
				kTeam.GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iBeakersBonus, eToPlayer);
			}

			pNotifications = toPlayer.GetNotifications();
			if(pNotifications)
			{
				strBuffer = GetLocalizedText("TXT_KEY_NTFN_RA_FREE_TECH", fromPlayer.getNameKey());
				strSummary = GetLocalizedText("TXT_KEY_NTFN_RA_FREE_TECH_S", fromPlayer.getNameKey());
				pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_RESEARCH_AGREEMENT, strBuffer, strSummary, -1, -1, -1);
			}
		}
		else
		{
			pNotifications = toPlayer.GetNotifications();
			if(pNotifications)
			{
				if(GET_TEAM(eFromTeam).isAtWar(eToTeam))
				{
					strBuffer = GetLocalizedText("TXT_KEY_NTFN_RA_FREE_TECH_WAR_CANCEL", fromPlayer.getNameKey());
					strSummary = GetLocalizedText("TXT_KEY_NTFN_RA_FREE_TECH_WAR_CANCEL_S");
				}
				else
				{
					strBuffer = GetLocalizedText("TXT_KEY_NTFN_RA_FREE_TECH_CANCEL", fromPlayer.getNameKey());
					strSummary = GetLocalizedText("TXT_KEY_NTFN_RA_FREE_TECH_CANCEL_S");
				}

				pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_RESEARCH_AGREEMENT, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}
	// Trade Agreement
	else if(pItem->m_eItemType == TRADE_ITEM_TRADE_AGREEMENT)
	{
		GET_TEAM(eFromTeam).SetHasTradeAgreement(eToTeam, false);

		pNotifications = fromPlayer.GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT_FROM_US", toPlayer.getNameKey());
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_TRADE_AGREEMENT_FROM_US", toPlayer.getNameKey());
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT, strBuffer, strSummary, -1, -1, -1);
		}

		pNotifications = toPlayer.GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT_TO_US", fromPlayer.getNameKey());
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_TRADE_AGREEMENT_TO_US", fromPlayer.getNameKey());
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT, strBuffer, strSummary, -1, -1, -1);
		}
	}
	// Peace Treaty
	else if(pItem->m_eItemType == TRADE_ITEM_PEACE_TREATY)
	{
		GET_TEAM(eFromTeam).setForcePeace(eToTeam, false);

		pNotifications = toPlayer.GetNotifications();
		if(pNotifications)
		{
			strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_PEACE", fromPlayer.getNameKey());
			strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_PEACE");
			pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT, strBuffer, strSummary, -1, -1, -1);
		}
	}
	// Third Party Peace Treaty
	else if(pItem->m_eItemType == TRADE_ITEM_THIRD_PARTY_PEACE)
	{
		TeamTypes eTargetTeam = (TeamTypes) pItem->m_iData1;
		GET_TEAM(eFromTeam).setForcePeace(eTargetTeam, false);
		GET_TEAM(eTargetTeam).setForcePeace(eFromTeam, false);

		CvPlayer* targetPlayer = &GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID());

		if (targetPlayer->isAlive())
		{
			// Notification for FROM player
			pNotifications = fromPlayer.GetNotifications();
			if(pNotifications)
			{
				strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_PEACE", targetPlayer->getNameKey());
				strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_PEACE");
				pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT, strBuffer, strSummary, -1, -1, -1);
			}

			// Notification for TARGET player
			pNotifications = targetPlayer->GetNotifications();
			if(pNotifications)
			{
				strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_DEAL_EXPIRED_PEACE", fromPlayer.getNameKey());
				strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_DEAL_EXPIRED_PEACE");
				pNotifications->Add(NOTIFICATION_DEAL_EXPIRED_TRADE_AGREEMENT, strBuffer, strSummary, -1, -1, -1);
			}
		}
	}
	// Vote Commitment
	else if(pItem->m_eItemType == TRADE_ITEM_VOTE_COMMITMENT)
	{
		fromPlayer.GetLeagueAI()->CancelVoteCommitmentsToPlayer(eToPlayer);
		toPlayer.GetLeagueAI()->CancelVoteCommitmentsToPlayer(eFromPlayer);
	}
#if defined(MOD_BALANCE_CORE)
	//Deal cancelled? That makes us sad.
	if(bCancelled)
	{
		if(!toPlayer.isHuman())
		{
			toPlayer.GetDiplomacyAI()->ChangeRecentTradeValue(fromPlayer.GetID(), -400);
		}
	}
#endif
}

/// Some trade items require Gold to be spent by both players
int CvGameDeals::GetTradeItemGoldCost(TradeableItems eItem, PlayerTypes ePlayer1, PlayerTypes ePlayer2) const
{
	int iGoldCost = 0;

	switch(eItem)
	{
	case TRADE_ITEM_RESEARCH_AGREEMENT:
	{
		iGoldCost = GC.getGame().GetResearchAgreementCost(ePlayer1, ePlayer2);
		break;
	}
	case TRADE_ITEM_TRADE_AGREEMENT:
		iGoldCost = 250;
		break;
	}

	return iGoldCost;
}

/// Mark elements in the deal as renewed depending on if they are in both deals
void CvGameDeals::PrepareRenewDeal(CvDeal* pOldDeal, const CvDeal* pNewDeal)
{
	// Cancel individual items
	// HACK HACK HACK
	// bad slewis! bad! bad!!
	CvDeal* pNonConstNewDeal = (CvDeal*)pNewDeal;
	// end HACK HACK HACK

	CvAssertMsg(pOldDeal->m_eFromPlayer == pNewDeal->m_eFromPlayer, "Deal is not to the same from players");
	CvAssertMsg(pOldDeal->m_eToPlayer == pNewDeal->m_eToPlayer, "Deal is not to the same to players");

	CvPlayerAI& fromPlayer = GET_PLAYER(pOldDeal->m_eFromPlayer);
	CvPlayerAI& toPlayer = GET_PLAYER(pOldDeal->m_eToPlayer);

	TradedItemList::iterator oldDealItemIter;
	for(oldDealItemIter = pOldDeal->m_TradedItems.begin(); oldDealItemIter != pOldDeal->m_TradedItems.end(); ++oldDealItemIter)
	{
		// if this is not a renewable item, ignore
		if(CvDeal::GetItemTradeableState(oldDealItemIter->m_eItemType) != CvDeal::DEAL_RENEWABLE)
		{
			continue;
		}

		TradedItemList::iterator newDealItemIter;
		for(newDealItemIter = pNonConstNewDeal->m_TradedItems.begin(); newDealItemIter != pNonConstNewDeal->m_TradedItems.end(); ++newDealItemIter)
		{
			// if this is not a renewable item, ignore
			if(CvDeal::GetItemTradeableState(newDealItemIter->m_eItemType) != CvDeal::DEAL_RENEWABLE)
			{
				continue;
			}

			// if the from player doesn't match, ignore
			if(oldDealItemIter->m_eFromPlayer != newDealItemIter->m_eFromPlayer)
			{
				continue;
			}

			// if the item types don't match, ignore
			if(oldDealItemIter->m_eItemType != newDealItemIter->m_eItemType)
			{
				continue;
			}

			TradeableItems eItemType = oldDealItemIter->m_eItemType;

			if(eItemType == TRADE_ITEM_RESOURCES)
			{
				// resource type
				if(oldDealItemIter->m_iData1 != newDealItemIter->m_iData1)
				{
					continue;
				}

				ResourceTypes eResource = (ResourceTypes)oldDealItemIter->m_iData1;

				// quantity
				if(oldDealItemIter->m_iData2 != newDealItemIter->m_iData2)
				{
					int iResourceDelta = newDealItemIter->m_iData2 - oldDealItemIter->m_iData2;
					if(oldDealItemIter->m_eFromPlayer == pOldDeal->m_eFromPlayer)
					{
						fromPlayer.changeResourceExport(eResource, iResourceDelta);
						toPlayer.changeResourceImportFromMajor(eResource, iResourceDelta);
					}
					else
					{
						toPlayer.changeResourceExport(eResource, iResourceDelta);
						fromPlayer.changeResourceImportFromMajor(eResource, iResourceDelta);
					}
				}
			}

			if(oldDealItemIter->m_eItemType == TRADE_ITEM_GOLD_PER_TURN)
			{
				int iOldGPTAmount = oldDealItemIter->m_iData1;
				int iNewGPTAmount = newDealItemIter->m_iData1;

				// has the amount of gold amount changed?
				if(iOldGPTAmount != iNewGPTAmount)
				{
					int iGoldPerTurnDelta = iNewGPTAmount - iOldGPTAmount;
					if(oldDealItemIter->m_eFromPlayer == pOldDeal->m_eFromPlayer)
					{
						fromPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(-iGoldPerTurnDelta);
						toPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iGoldPerTurnDelta);
					}
					else
					{
						toPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(-iGoldPerTurnDelta);
						fromPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iGoldPerTurnDelta);
					}
				}
			}

			// mark the deals as appropriately renewed
			newDealItemIter->m_bToRenewed = true;
			oldDealItemIter->m_bFromRenewed = true;

			// break because we found the match and can continue on
			break;
		}
	}
}


void CvGameDeals::LogDealComplete(CvDeal* pDeal)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
#if defined(MOD_ACTIVE_DIPLOMACY)
		if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
		{
			CvString playerName = GET_PLAYER(pDeal->GetFromPlayer()).getCivilizationShortDescription();
		}
		int iEvenValueImOffering;
		int iEvenValueTheyreOffering;
		int iTotalValueFrom = GET_PLAYER(pDeal->GetFromPlayer()).GetDealAI()->GetDealValue(pDeal, iEvenValueImOffering, iEvenValueTheyreOffering, /*bUseEvenValue*/ false, true);
		int iTotalValueTo = GET_PLAYER(pDeal->GetToPlayer()).GetDealAI()->GetDealValue(pDeal, iEvenValueImOffering, iEvenValueTheyreOffering, /*bUseEvenValue*/ false, true);
#endif
		CvString otherPlayerName;

		CvString strTemp;

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_TradeAgreements_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_TradeAgreements_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		PlayerTypes eFromPlayer;
		PlayerTypes eToPlayer;

		TradedItemList::iterator itemIter;
		for(itemIter = pDeal->m_TradedItems.begin(); itemIter != pDeal->m_TradedItems.end(); ++itemIter)
		{
			// Turn number
			strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

			eFromPlayer = itemIter->m_eFromPlayer;
			eToPlayer = eFromPlayer == pDeal->m_eFromPlayer ? pDeal->m_eToPlayer : pDeal->m_eFromPlayer;

			playerName = GET_PLAYER(eFromPlayer).getCivilizationShortDescription();

			// Our Name
			strBaseString += playerName;

			// Their Name
			otherPlayerName = GET_PLAYER(eToPlayer).getCivilizationShortDescription();
			strBaseString += ", " + otherPlayerName;

			strOutBuf = strBaseString + ", , ";

			// Peace Treaty deal?
			if(pDeal->GetPeaceTreatyType() != NO_PEACE_TREATY_TYPE)
			{
				switch(pDeal->GetPeaceTreatyType())
				{
				case PEACE_TREATY_WHITE_PEACE:
					strTemp.Format("***** WHITE PEACE *****");
					break;
				case PEACE_TREATY_ARMISTICE:
					strTemp.Format("***** ARMISTICE *****");
					break;
				case PEACE_TREATY_SETTLEMENT:
					strTemp.Format("***** SETTLEMENT *****");
					break;
				case PEACE_TREATY_BACKDOWN:
					strTemp.Format("***** BACKDOWN *****");
					break;
				case PEACE_TREATY_SUBMISSION:
					strTemp.Format("***** SUBMISSION *****");
					break;
				case PEACE_TREATY_SURRENDER:
					strTemp.Format("***** SURRENDER *****");
					break;
				case PEACE_TREATY_CESSION:
					strTemp.Format("***** CESSION *****");
					break;
				case PEACE_TREATY_CAPITULATION:
					strTemp.Format("***** CAPITULATION *****");
					break;
				case PEACE_TREATY_UNCONDITIONAL_SURRENDER:
					strTemp.Format("***** UNCONDITIONAL SURRENDER *****");
					break;
				default:
					strTemp.Format("XXX NO VALID PEACE TREATY!!!");
					break;
				}
				strOutBuf += ", " + strTemp;
			}

			// Is someone surrendering?
			if(pDeal->GetSurrenderingPlayer() != NO_PLAYER)
			{
				playerName = GET_PLAYER(pDeal->GetSurrenderingPlayer()).getCivilizationShortDescription();
				int iWarScore = GET_PLAYER(eFromPlayer).GetDiplomacyAI()->GetWarScore(eToPlayer);
				CvString strWarscore;
				strWarscore.Format("%d", iWarScore);
				strOutBuf += ", " + playerName + " is giving up" + " at a Warscore of: " + strWarscore;
			}
			// White Peace
			else if(pDeal->GetPeaceTreatyType() == PEACE_TREATY_WHITE_PEACE)
			{
				strOutBuf += ", White Peace!";
			}

			// Is this a demand?
			if(pDeal->GetDemandingPlayer() != NO_PLAYER)
			{
				playerName = GET_PLAYER(pDeal->GetDemandingPlayer()).getCivilizationShortDescription();
				strOutBuf += ", " + playerName + " has made a demand!";
			}

			// Is this a request?
			if(pDeal->GetRequestingPlayer() != NO_PLAYER)
			{
				playerName = GET_PLAYER(pDeal->GetRequestingPlayer()).getCivilizationShortDescription();
				strOutBuf += ", " + playerName + " has made a request!";
			}

			// What is this particular Trade item?
			switch(itemIter->m_eItemType)
			{
			case TRADE_ITEM_GOLD:
				strTemp.Format("***** Gold Trade: %d *****", itemIter->m_iData1);
				break;
			case TRADE_ITEM_GOLD_PER_TURN:
				strTemp.Format("***** GPT Trade: %d *****", itemIter->m_iData1);
				break;
			case TRADE_ITEM_RESOURCES:
				strTemp.Format("***** Resource Trade: ID %s *****", GC.getResourceInfo((ResourceTypes)itemIter->m_iData1)->GetDescriptionKey());
				break;
			case TRADE_ITEM_CITIES:
			{
				CvPlot* pPlot = GC.getMap().plot(itemIter->m_iData1, itemIter->m_iData2);
				CvCity* pCity = 0;
				if (pPlot)
					pCity = pPlot->getPlotCity();
				strTemp.Format("***** City Trade: ID %s *****", pCity ? pCity->getName().c_str() : "unknown" );
				break;
			}
			case TRADE_ITEM_OPEN_BORDERS:
				strTemp.Format("Open Borders Trade");
				break;
			case TRADE_ITEM_DEFENSIVE_PACT:
				strTemp.Format("***** Defensive Pact Trade *****");
				break;
			case TRADE_ITEM_RESEARCH_AGREEMENT:
				strTemp.Format("Research Agreement Trade");
				break;
			case TRADE_ITEM_PEACE_TREATY:
				strTemp.Format("***** Peace Treaty Trade *****");
				break;
#if defined(MOD_BALANCE_CORE)
			case TRADE_ITEM_THIRD_PARTY_PEACE:
				strTemp.Format("***** Third Party Peace Trade versus ID %s *****", GET_PLAYER((PlayerTypes)itemIter->m_iData1).getCivilizationDescriptionKey());
				break;
			case TRADE_ITEM_THIRD_PARTY_WAR:
				strTemp.Format("***** Third Party War Trade versus ID %s *****" , GET_PLAYER((PlayerTypes)itemIter->m_iData1).getCivilizationDescriptionKey());
				break;
#else
			case TRADE_ITEM_THIRD_PARTY_PEACE:
				strTemp.Format("***** Third Party Peace Trade *****");
				break;
			case TRADE_ITEM_THIRD_PARTY_WAR:
				strTemp.Format("***** Third Party War Trade *****");
				break;
#endif
			case TRADE_ITEM_VOTE_COMMITMENT:
				strTemp.Format("***** Vote Commitment: ID %d, Choice %d *****", itemIter->m_iData1, itemIter->m_iData2);
				break;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			case TRADE_ITEM_MAPS:
				strTemp.Format("***** Map Trade *****");
				break;
			case TRADE_ITEM_TECHS:
				strTemp.Format("***** Tech Trade ***** ID %s",  GC.getTechInfo((TechTypes)itemIter->m_iData1)->GetDescriptionKey());
				break;
			case TRADE_ITEM_VASSALAGE:
				strTemp.Format("***** Vassalage Trade *****");
				break;
			case TRADE_ITEM_VASSALAGE_REVOKE:
				strTemp.Format("***** Revoke Vassalage Trade *****");
				break;
#endif
#if defined(MOD_BALANCE_CORE)
			case TRADE_ITEM_ALLOW_EMBASSY:
				strTemp.Format("***** Embassy Trade *****");
				break;
#endif
			default:
				strTemp.Format("***** UNKNOWN TRADE!!! *****");
				break;
			}
			strOutBuf += ", " + strTemp;

#if defined(MOD_BALANCE_CORE)
			if (itemIter->m_iValue != INT_MAX)
			{
				strTemp.Format("(value %d - %s)", itemIter->m_iValue, itemIter->m_bValueIsEven ? "equalized" : "raw");
				strOutBuf += ", " + strTemp;
			}
#endif

			pLog->Msg(strOutBuf);
#if !defined(MOD_BALANCE_CORE)
			OutputDebugString("\n");
			OutputDebugString(strOutBuf);
			OutputDebugString("\n");
#endif
		}

		strTemp.Format("DEAL COMPLETE: Deal Value Sender: %d ; Deal Value Recipient: %d", iTotalValueFrom, iTotalValueTo);
		strOutBuf = strTemp;

		pLog->Msg(strOutBuf);
	}
}
#if defined(MOD_BALANCE_CORE)
void CvGameDeals::LogDealFailed(CvDeal* pDeal, bool bNoRenew, bool bNotAccepted, bool bNotValid)
{
	if(GC.getLogging() && GC.getAILogging() && pDeal)
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName = GET_PLAYER(pDeal->GetFromPlayer()).getCivilizationShortDescription();
		CvString otherPlayerName;

		CvString strTemp;

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_TradeAgreements_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_TradeAgreements_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		PlayerTypes eFromPlayer;
		PlayerTypes eToPlayer;

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		if(bNoRenew)
		{
			strOutBuf = strBaseString + "FAILED: NOT RENEWED, ";
		}
		if(bNotAccepted)
		{
			strOutBuf = strBaseString + "NOT ACCEPTED, ";
		}
		if(bNotValid)
		{
			strOutBuf = strBaseString + "FAILED: NOT A VALID DEAL, ";
		}
		if(pDeal == NULL)
		{
			strOutBuf = strBaseString + "FAILED: NO DEAL FOUND";
			pLog->Msg(strOutBuf);
			return;
		}
		else if (!(bNoRenew || bNotAccepted || bNotValid))
		{
			strOutBuf = strBaseString + "FAILED: UNKNOWN, ";
		}

		TradedItemList::iterator itemIter;
		for(itemIter = pDeal->m_TradedItems.begin(); itemIter != pDeal->m_TradedItems.end(); ++itemIter)
		{
			// Turn number
			strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

			eFromPlayer = itemIter->m_eFromPlayer;
			eToPlayer = eFromPlayer == pDeal->m_eFromPlayer ? pDeal->m_eToPlayer : pDeal->m_eFromPlayer;

			playerName = GET_PLAYER(eFromPlayer).getCivilizationShortDescription();

			// Our Name
			strBaseString += playerName;

			// Their Name
			otherPlayerName = GET_PLAYER(eToPlayer).getCivilizationShortDescription();
			strBaseString += ", " + otherPlayerName;

			strOutBuf = strBaseString + ", , ";

			// Peace Treaty deal?
			if(pDeal->GetPeaceTreatyType() != NO_PEACE_TREATY_TYPE)
			{
				switch(pDeal->GetPeaceTreatyType())
				{
				case PEACE_TREATY_WHITE_PEACE:
					strTemp.Format("***** WHITE PEACE *****");
					break;
				case PEACE_TREATY_ARMISTICE:
					strTemp.Format("***** ARMISTICE *****");
					break;
				case PEACE_TREATY_SETTLEMENT:
					strTemp.Format("***** SETTLEMENT *****");
					break;
				case PEACE_TREATY_BACKDOWN:
					strTemp.Format("***** BACKDOWN *****");
					break;
				case PEACE_TREATY_SUBMISSION:
					strTemp.Format("***** SUBMISSION *****");
					break;
				case PEACE_TREATY_SURRENDER:
					strTemp.Format("***** SURRENDER *****");
					break;
				case PEACE_TREATY_CESSION:
					strTemp.Format("***** CESSION *****");
					break;
				case PEACE_TREATY_CAPITULATION:
					strTemp.Format("***** CAPITULATION *****");
					break;
				case PEACE_TREATY_UNCONDITIONAL_SURRENDER:
					strTemp.Format("***** UNCONDITIONAL SURRENDER *****");
					break;
				default:
					strTemp.Format("XXX NO VALID PEACE TREATY!!!");
					break;
				}
				strOutBuf += ", " + strTemp;
			}

			// Is someone surrendering?
			if(pDeal->GetSurrenderingPlayer() != NO_PLAYER)
			{
				playerName = GET_PLAYER(pDeal->GetSurrenderingPlayer()).getCivilizationShortDescription();
				int iWarScore = GET_PLAYER(eFromPlayer).GetDiplomacyAI()->GetWarScore(eToPlayer);
				CvString strWarscore;
				strWarscore.Format("%d", iWarScore);
				strOutBuf += ", " + playerName + " is giving up" + " at a Warscore of: " + strWarscore;
			}
			// White Peace
			else if(pDeal->GetPeaceTreatyType() == PEACE_TREATY_WHITE_PEACE)
			{
				strOutBuf += ", White Peace!";
			}

			// Is this a demand?
			if(pDeal->GetDemandingPlayer() != NO_PLAYER)
			{
				playerName = GET_PLAYER(pDeal->GetDemandingPlayer()).getCivilizationShortDescription();
				strOutBuf += ", " + playerName + " has made a demand!";
			}

			// Is this a request?
			if(pDeal->GetRequestingPlayer() != NO_PLAYER)
			{
				playerName = GET_PLAYER(pDeal->GetRequestingPlayer()).getCivilizationShortDescription();
				strOutBuf += ", " + playerName + " has made a request!";
			}

			// What is this particular Trade item?
			switch(itemIter->m_eItemType)
			{
			case TRADE_ITEM_GOLD:
				strTemp.Format("***** Gold Trade: %d *****", itemIter->m_iData1);
				break;
			case TRADE_ITEM_GOLD_PER_TURN:
				strTemp.Format("***** GPT Trade: %d *****", itemIter->m_iData1);
				break;
			case TRADE_ITEM_RESOURCES:
				strTemp.Format("***** Resource Trade: ID %s *****", GC.getResourceInfo((ResourceTypes)itemIter->m_iData1)->GetDescriptionKey());
				break;
			case TRADE_ITEM_CITIES:
			{
				CvPlot* pPlot = GC.getMap().plot(itemIter->m_iData1, itemIter->m_iData2);
				CvCity* pCity = 0;
				if (pPlot)
					pCity = pPlot->getPlotCity();
				strTemp.Format("***** City Trade: ID %s *****", pCity ? pCity->getName().c_str() : "unknown" );
				break;
			}
			case TRADE_ITEM_OPEN_BORDERS:
				strTemp.Format("Open Borders Trade");
				break;
			case TRADE_ITEM_DEFENSIVE_PACT:
				strTemp.Format("***** Defensive Pact Trade *****");
				break;
			case TRADE_ITEM_RESEARCH_AGREEMENT:
				strTemp.Format("Research Agreement Trade");
				break;
			case TRADE_ITEM_PEACE_TREATY:
				strTemp.Format("***** Peace Treaty Trade *****");
				break;
#if defined(MOD_BALANCE_CORE)
			case TRADE_ITEM_THIRD_PARTY_PEACE:
				strTemp.Format("***** Third Party Peace Trade versus ID %s *****", GET_PLAYER((PlayerTypes)itemIter->m_iData1).getCivilizationDescriptionKey());
				break;
			case TRADE_ITEM_THIRD_PARTY_WAR:
				strTemp.Format("***** Third Party War Trade versus ID %s *****" , GET_PLAYER((PlayerTypes)itemIter->m_iData1).getCivilizationDescriptionKey());
				break;
#else
			case TRADE_ITEM_THIRD_PARTY_PEACE:
				strTemp.Format("***** Third Party Peace Trade *****");
				break;
			case TRADE_ITEM_THIRD_PARTY_WAR:
				strTemp.Format("***** Third Party War Trade *****");
				break;
#endif
			case TRADE_ITEM_VOTE_COMMITMENT:
				strTemp.Format("***** Vote Commitment: ID %d, Choice %d *****", itemIter->m_iData1, itemIter->m_iData2);
				break;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			case TRADE_ITEM_MAPS:
				strTemp.Format("***** Map Trade *****");
				break;
			case TRADE_ITEM_TECHS:
				strTemp.Format("***** Tech Trade ***** ID %s",  GC.getTechInfo((TechTypes)itemIter->m_iData1)->GetDescriptionKey());
				break;
			case TRADE_ITEM_VASSALAGE:
				strTemp.Format("***** Vassalage Trade *****");
				break;
			case TRADE_ITEM_VASSALAGE_REVOKE:
				strTemp.Format("***** Revoke Vassalage Trade *****");
				break;
#endif
#if defined(MOD_BALANCE_CORE)
			case TRADE_ITEM_ALLOW_EMBASSY:
				strTemp.Format("***** Embassy Trade *****");
				break;
#endif
			default:
				strTemp.Format("***** UNKNOWN TRADE!!! *****");
				break;
			}
			strOutBuf += ", " + strTemp;

#if defined(MOD_BALANCE_CORE)
			if (itemIter->m_iValue != INT_MAX)
			{
				strTemp.Format("(value %d - %s)", itemIter->m_iValue, itemIter->m_bValueIsEven ? "equalized" : "raw");
				strOutBuf += ", " + strTemp;
			}
#endif

			pLog->Msg(strOutBuf);
#if !defined(MOD_BALANCE_CORE)
			OutputDebugString("\n");
			OutputDebugString(strOutBuf);
			OutputDebugString("\n");
#endif
		}
	}
}
#endif

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
CvDeal* CvGameDeals::GetCurrentDeal(PlayerTypes ePlayer, uint index)
{
	DealList::iterator iter;
	DealList::iterator end = m_CurrentDeals.end();

	uint iCount = 0;
	for(iter = m_CurrentDeals.begin(); iter != end; ++iter)
	{
		if((iter->m_eToPlayer == ePlayer ||
		        iter->m_eFromPlayer == ePlayer) &&
		        (iCount++ == index))
		{
			return &(*iter);
		}
	}

	return NULL;
}


// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
CvDeal* CvGameDeals::GetHistoricDeal(PlayerTypes ePlayer, uint index)
{
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		//iterate backwards, usually the latest deals are most interesting
		uint iCount = 0;
		for (int i = m_HistoricalDeals.size() - 1; i >= 0; --i)
		{
			CvDeal& kDeal = m_HistoricalDeals[i];
			if((kDeal.m_eToPlayer == ePlayer || kDeal.m_eFromPlayer == ePlayer) && (iCount++ == index))
			{
				return &kDeal;
			}
		}
	}
	else
	{
		DealList::iterator iter;
		DealList::iterator end = m_HistoricalDeals.end();

		uint iCount = 0;
		for(iter = m_HistoricalDeals.begin(); iter != end; ++iter)
		{
			if((iter->m_eToPlayer == ePlayer ||
		        iter->m_eFromPlayer == ePlayer) &&
		        (iCount++ == index))
			{
				return &(*iter);
			}
		}
	}
#else
	DealList::iterator iter;
	DealList::iterator end = m_HistoricalDeals.end();

	uint iCount = 0;
	for(iter = m_HistoricalDeals.begin(); iter != end; ++iter)
	{
		if((iter->m_eToPlayer == ePlayer ||
		        iter->m_eFromPlayer == ePlayer) &&
		        (iCount++ == index))
		{
			return &(*iter);
		}
	}
#endif
	return NULL;
}


// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
uint CvGameDeals::GetNumCurrentDeals(PlayerTypes ePlayer)
{
	DealList::iterator iter;
	DealList::iterator end = m_CurrentDeals.end();

	uint iCount = 0;
	for(iter = m_CurrentDeals.begin(); iter != end; ++iter)
	{
		if(iter->m_eToPlayer == ePlayer ||
		        iter->m_eFromPlayer == ePlayer)
		{
			++iCount;
		}
	}

	return iCount;
}


// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
uint CvGameDeals::GetNumHistoricDeals(PlayerTypes ePlayer, uint iMaxCount)

{
	DealList::iterator iter;
	DealList::iterator end = m_HistoricalDeals.end();

	uint iCount = 0;
	for(iter = m_HistoricalDeals.begin(); iter != end; ++iter)
	{
		if(iter->m_eToPlayer == ePlayer ||
		        iter->m_eFromPlayer == ePlayer)
		{
			++iCount;
			if (iCount==iMaxCount)
				break;
		}
	}

	return iCount;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
CvDeal* CvGameDeals::GetCurrentDealWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, uint index)
{
	DealList::iterator iter;
	DealList::iterator end = m_CurrentDeals.end();

	uint iCount = 0;
	for (iter = m_CurrentDeals.begin(); iter != end; ++iter)
	{
		if ((iter->m_eToPlayer == ePlayer || iter->m_eFromPlayer == ePlayer) && 
			(iter->m_eToPlayer == eOtherPlayer || iter->m_eFromPlayer == eOtherPlayer) &&
			(iCount++ == index))
		{
			return &(*iter);
		}
	}

	return NULL;
}


// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
CvDeal* CvGameDeals::GetHistoricDealWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, uint index)
{
#if defined(MOD_ACTIVE_DIPLOMACY)
	if (GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		//iterate backwards, usually the latest deals are most interesting
		uint iCount = 0;
		for (int i = m_HistoricalDeals.size() - 1; i >= 0; --i)
		{
			CvDeal& kDeal = m_HistoricalDeals[i];
			if ((kDeal.m_eToPlayer == ePlayer || kDeal.m_eFromPlayer == ePlayer) &&
				(kDeal.m_eToPlayer == eOtherPlayer || kDeal.m_eFromPlayer == eOtherPlayer) &&
				(iCount++ == index))
			{
				return &kDeal;
			}
		}
	}
	else
	{
		DealList::iterator iter;
		DealList::iterator end = m_HistoricalDeals.end();

		uint iCount = 0;
		for (iter = m_HistoricalDeals.begin(); iter != end; ++iter)
		{
			if ((iter->m_eToPlayer == ePlayer || iter->m_eFromPlayer == ePlayer) &&
				(iter->m_eToPlayer == eOtherPlayer || iter->m_eFromPlayer == eOtherPlayer) &&
				(iCount++ == index))
			{
				return &(*iter);
			}
		}
	}
#else
	DealList::iterator iter;
	DealList::iterator end = m_HistoricalDeals.end();

	uint iCount = 0;
	for (iter = m_HistoricalDeals.begin(); iter != end; ++iter)
	{
		if ((iter->m_eToPlayer == ePlayer ||
			iter->m_eFromPlayer == ePlayer) &&
			(iCount++ == index))
		{
			return &(*iter);
		}
	}
#endif
	return NULL;
}


// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
uint CvGameDeals::GetNumCurrentDealsWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer)
{
	DealList::iterator iter;
	DealList::iterator end = m_CurrentDeals.end();

	uint iCount = 0;
	for (iter = m_CurrentDeals.begin(); iter != end; ++iter)
	{
		if ((iter->m_eToPlayer == ePlayer || iter->m_eFromPlayer == ePlayer) &&
			(iter->m_eToPlayer == eOtherPlayer || iter->m_eFromPlayer == eOtherPlayer))
		{
			++iCount;
		}
	}

	return iCount;
}

int CvGameDeals::GetDealValueWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer)
{
	DealList::iterator iter;
	DealList::iterator end = m_CurrentDeals.end();

	int iVal = 0;
	for (iter = m_CurrentDeals.begin(); iter != end; ++iter)
	{
		if ((iter->m_eToPlayer == ePlayer || iter->m_eFromPlayer == ePlayer) &&
			(iter->m_eToPlayer == eOtherPlayer || iter->m_eFromPlayer == eOtherPlayer))
		{
			//GPT - base it on number of turns remaining
			iVal += iter->GetGoldPerTurnTrade(eOtherPlayer) * (iter->GetEndTurn() - GC.getGame().getGameTurn());

			//Resources
			int iResourceLoop;
			ResourceTypes eResource;

			for (iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				eResource = (ResourceTypes)iResourceLoop;

				const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if (pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
					continue;

				iVal += iter->GetNumResourcesInDeal(eOtherPlayer, eResource) * (iter->GetEndTurn() - GC.getGame().getGameTurn());

			}

			iVal += iter->IsOpenBordersTrade(eOtherPlayer) ? 10 : 0;
			iVal += iter->IsOpenBordersTrade(ePlayer) ? 5 : 0;

			iVal += iter->IsDefensivePactTrade(eOtherPlayer) ? 10 : 0;
		}
	}

	return iVal;
}

int CvGameDeals::GetDealGPTLostFromWar(PlayerTypes ePlayer, PlayerTypes eOtherPlayer)
{
	DealList::iterator iter;
	DealList::iterator end = m_CurrentDeals.end();

	int iGPT = 0;
	for (iter = m_CurrentDeals.begin(); iter != end; ++iter)
	{
		if ((iter->m_eToPlayer == ePlayer || iter->m_eFromPlayer == ePlayer) &&
			(iter->m_eToPlayer == eOtherPlayer || iter->m_eFromPlayer == eOtherPlayer))
		{
			// Add incoming GPT, subtract outgoing GPT
			iGPT += iter->GetGoldPerTurnTrade(eOtherPlayer);
			iGPT -= iter->GetGoldPerTurnTrade(ePlayer);
		}
	}
	
	return iGPT;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
uint CvGameDeals::GetNumHistoricDealsWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, uint iMaxCount)

{
	DealList::iterator iter;
	DealList::iterator end = m_HistoricalDeals.end();

	uint iCount = 0;
	for (iter = m_HistoricalDeals.begin(); iter != end; ++iter)
	{
		if ((iter->m_eToPlayer == ePlayer || iter->m_eFromPlayer == ePlayer) &&
			(iter->m_eToPlayer == eOtherPlayer || iter->m_eFromPlayer == eOtherPlayer))
		{
			++iCount;
			if (iCount == iMaxCount)
				break;
		}
	}

	return iCount;
}

//------------------------------------------------------------------------------
uint CvGameDeals::CreateDeal()
{
	uint index = m_uiDealCounter++;
	std::pair<uint, CvDeal*> entry(index, FNEW(CvDeal, c_eCiv5GameplayDLL, 0));

	m_Deals.push_back(entry);
	return index;
}
//------------------------------------------------------------------------------
CvDeal* CvGameDeals::GetDeal(uint index)
{
	for(std::vector<std::pair<uint, CvDeal*> >::iterator it = m_Deals.begin();
	        it != m_Deals.end(); ++it)
	{
		if((*it).first == index)
			return (*it).second;
	}

	return NULL;
}
//------------------------------------------------------------------------------
void CvGameDeals::DestroyDeal(uint index)
{
	std::vector<std::pair<uint, CvDeal*> >::iterator it = m_Deals.end();
	for(it = m_Deals.begin();
	        it != m_Deals.end(); ++it)
	{
		if((*it).first == index)
			break;
	}

	if(it != m_Deals.end())
	{
		delete(*it).second;
		m_Deals.erase(it);
	}
}
//------------------------------------------------------------------------------
FDataStream& OldLoad(FDataStream& loadFrom, CvGameDeals& writeTo)
{
	int iEntriesToRead;
	CvDeal tempItem;

#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		// JdH => savegame compatible load
		loadFrom >> iEntriesToRead;
		for (int iI = 0; iI < iEntriesToRead; iI++)
		{
			loadFrom >> tempItem;
			if (CvPreGame::isHuman(tempItem.GetFromPlayer()) && CvPreGame::isHuman(tempItem.GetToPlayer())) 
			{
				// only load human to humand deals until other problems are fixed
				writeTo.m_ProposedDeals.push_back(tempItem);
			}
		}
	}
	else
	{
		writeTo.m_ProposedDeals.clear();
		loadFrom >> iEntriesToRead;
		for(int iI = 0; iI < iEntriesToRead; iI++)
		{
			loadFrom = OldLoad(loadFrom, tempItem);
			writeTo.m_ProposedDeals.push_back(tempItem);
		}
	}
#else
	writeTo.m_ProposedDeals.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom = OldLoad(loadFrom, tempItem);
		writeTo.m_ProposedDeals.push_back(tempItem);
	}

#endif

	writeTo.m_CurrentDeals.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom = OldLoad(loadFrom, tempItem);
		writeTo.m_CurrentDeals.push_back(tempItem);
	}

	writeTo.m_HistoricalDeals.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom = OldLoad(loadFrom, tempItem);
		writeTo.m_HistoricalDeals.push_back(tempItem);
	}

	return loadFrom;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGameDeals& writeTo)
{
	uint uiVersion;
	int iEntriesToRead;
	CvDeal tempItem;

	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	writeTo.m_ProposedDeals.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> tempItem;
		writeTo.m_ProposedDeals.push_back(tempItem);
	}
	writeTo.m_CurrentDeals.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> tempItem;
		writeTo.m_CurrentDeals.push_back(tempItem);
	}

	writeTo.m_HistoricalDeals.clear();
	loadFrom >> iEntriesToRead;
	for(int iI = 0; iI < iEntriesToRead; iI++)
	{
		loadFrom >> tempItem;
		writeTo.m_HistoricalDeals.push_back(tempItem);
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGameDeals& readFrom)
{
	uint uiVersion = 1;
	DealList::const_iterator it;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_ProposedDeals.size();
	for(it = readFrom.m_ProposedDeals.begin(); it != readFrom.m_ProposedDeals.end(); ++it)
	{
		saveTo << *it;
	}
	saveTo << readFrom.m_CurrentDeals.size();
	for(it = readFrom.m_CurrentDeals.begin(); it != readFrom.m_CurrentDeals.end(); ++it)
	{
		saveTo << *it;
	}
	saveTo << readFrom.m_HistoricalDeals.size();
	for(it = readFrom.m_HistoricalDeals.begin(); it != readFrom.m_HistoricalDeals.end(); ++it)
	{
		saveTo << *it;
	}

	return saveTo;
}
