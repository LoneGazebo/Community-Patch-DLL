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

bool CvTradedItem::IsTwoSided() const
{
	switch (m_eItemType)
	{
	//these only make sense if both sides include them in the deal
	case TRADE_ITEM_DEFENSIVE_PACT:
	case TRADE_ITEM_DECLARATION_OF_FRIENDSHIP:
	case TRADE_ITEM_PEACE_TREATY:
	case TRADE_ITEM_RESEARCH_AGREEMENT:
		return true;
	default:
		return false;
	}
}

template<typename TradedItem, typename Visitor>
void CvTradedItem::Serialize(TradedItem& tradedItem, Visitor& visitor)
{
	visitor(tradedItem.m_eItemType);
	visitor(tradedItem.m_iDuration);
	visitor(tradedItem.m_iFinalTurn);
	visitor(tradedItem.m_iData1);
	visitor(tradedItem.m_iData2);
	visitor(tradedItem.m_iData3);
	visitor(tradedItem.m_bFlag1);
	visitor(tradedItem.m_eFromPlayer);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvTradedItem& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvTradedItem::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvTradedItem& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvTradedItem::Serialize(readFrom, serialVisitor);
	return saveTo;
}

//=====================================
// CvDeal
//=====================================

/// Constructor with typical parameters
CvDeal::CvDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer)
{
	m_eFromPlayer = eFromPlayer;
	m_eToPlayer = eToPlayer;

	m_ePeaceTreatyType = NO_PEACE_TREATY_TYPE;
	m_eSurrenderingPlayer = NO_PLAYER;
	m_eDemandingPlayer = NO_PLAYER;
	m_eRequestingPlayer = NO_PLAYER;
	m_iStartTurn = 0;
	m_iFinalTurn = 0;
	m_iDuration = 0;

	m_bConsideringForRenewal = false;
	m_bCheckedForRenewal = false;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsGift = false;
	m_bDoNotModifyFrom = false;
	m_bDoNotModifyTo = false;
	m_iFromPlayerValue = -1;
	m_iToPlayerValue = -1;
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
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsGift = source.m_bIsGift;
	m_bDoNotModifyFrom = source.m_bDoNotModifyFrom;
	m_bDoNotModifyTo = source.m_bDoNotModifyTo;
	m_iFromPlayerValue = source.m_iFromPlayerValue;
	m_iToPlayerValue = source.m_iToPlayerValue;
#endif
	m_TradedItems = source.m_TradedItems;
}

/// Destructor
CvDeal::~CvDeal()
{
}

bool CvDeal::operator==(const CvDeal& other) const
{
	return
		m_eFromPlayer == other.m_eFromPlayer &&
		m_eToPlayer == other.m_eToPlayer &&
		m_iDuration == other.m_iDuration &&
		m_iFinalTurn == other.m_iFinalTurn &&
		m_iStartTurn == other.m_iStartTurn &&
		m_ePeaceTreatyType == other.m_ePeaceTreatyType &&
		m_eSurrenderingPlayer == other.m_eSurrenderingPlayer &&
		m_eDemandingPlayer == other.m_eDemandingPlayer &&
		m_eRequestingPlayer == other.m_eRequestingPlayer &&
		m_bConsideringForRenewal == other.m_bConsideringForRenewal &&
		m_bCheckedForRenewal == other.m_bCheckedForRenewal &&
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		m_bIsGift == other.m_bIsGift &&
		m_bDoNotModifyFrom == other.m_bDoNotModifyFrom &&
		m_bDoNotModifyTo == other.m_bDoNotModifyTo &&
		m_iFromPlayerValue == other.m_iFromPlayerValue &&
		m_iToPlayerValue == other.m_iToPlayerValue &&
#endif
		m_TradedItems == other.m_TradedItems;
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
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsGift = source.m_bIsGift;
	m_bDoNotModifyFrom = source.m_bDoNotModifyFrom;
	m_bDoNotModifyTo = source.m_bDoNotModifyTo;
	m_iFromPlayerValue = source.m_iFromPlayerValue;
	m_iToPlayerValue = source.m_iToPlayerValue;
#endif
	m_TradedItems = source.m_TradedItems;
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
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	m_bIsGift = false;
	m_bDoNotModifyFrom = false;
	m_bDoNotModifyTo = false;
	m_iFromPlayerValue = -1;
	m_iToPlayerValue = -1;
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

int CvDeal::GetMaxValue() const
{
	//do not look at (m_iFromPlayerValue, m_iToPlayerValue) they are net values
	int iLeftSideValue = 0; //this is us
	int iRightSideValue = 0; //this is the other player

	TradedItemList::const_iterator it;
	for (it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if (it->m_eFromPlayer == m_eFromPlayer)
			iLeftSideValue += it->m_iValue;
		else if (it->m_eFromPlayer == m_eToPlayer)
			iRightSideValue += it->m_iValue;
	}

	return max(iLeftSideValue, iRightSideValue);
}

void CvDeal::SetFromPlayerValue(int iValue)
{
	m_iFromPlayerValue = iValue;
}
void CvDeal::ChangeFromPlayerValue(int iValue)
{
	m_iFromPlayerValue += iValue;
}

void CvDeal::SetToPlayerValue(int iValue)
{
	m_iToPlayerValue = iValue;
}
void CvDeal::ChangeToPlayerValue(int iValue)
{
	m_iToPlayerValue += iValue;
}

void CvDeal::SetDuration(int iValue)
{
	m_iDuration = iValue;
}

void CvDeal::SetDoNotModifyFrom(bool bValue)
{
	m_bDoNotModifyFrom = bValue;
}

void CvDeal::SetDoNotModifyTo(bool bValue)
{
	m_bDoNotModifyTo = bValue;
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

	// Observer can't trade anything
	if (!GET_PLAYER(ePlayer).isAlive() || GET_PLAYER(ePlayer).isObserver())
		return false;

	// The Data parameters can be -1, which means we don't care about whatever data is stored there (e.g. -1 for Gold means can we trade ANY amount of Gold?)
	CvPlayer* pFromPlayer = &GET_PLAYER(ePlayer);
	CvPlayer* pToPlayer = &GET_PLAYER(eToPlayer);

	TeamTypes eFromTeam = pFromPlayer->getTeam();
	TeamTypes eToTeam = pToPlayer->getTeam();

	CvTeam* pFromTeam = &GET_TEAM(eFromTeam);
	CvTeam* pToTeam = &GET_TEAM(eToTeam);

	bool bHumanToHuman = false;
	if (pFromPlayer->isHuman() && pToPlayer->isHuman())
	{
		bHumanToHuman = true;
	}

	std::vector<CvDeal*> pRenewDeals = pFromPlayer->GetDiplomacyAI()->GetDealsToRenew(eToPlayer);

	if (this->GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE && eItem != TRADE_ITEM_PEACE_TREATY && !ContainsItemType(TRADE_ITEM_PEACE_TREATY))
	{
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if (pLeague != NULL && pLeague->IsTradeEmbargoed(ePlayer, eToPlayer))
		{
			return false;
		}
	}

	bool bLumpGoldEnabled = GC.getGame().IsLumpGoldTradingEnabled();
	bLumpGoldEnabled |= GC.getGame().IsLumpGoldTradingHumanOnly() && (pFromPlayer->isHuman() || pToPlayer->isHuman());

	int iGoldAvailable = GetGoldAvailable(ePlayer, eItem);

	// Some items require gold be spent (e.g. Research and Trade Agreements)
	int iCost = GC.getGame().GetGameDeals().GetTradeItemGoldCost(eItem, ePlayer, eToPlayer);
	if(iCost > 0 && iGoldAvailable < iCost)
		return false;

	iGoldAvailable -= iCost;

	////////////////////////////////////////////////////

	// Gold
	if (eItem == TRADE_ITEM_GOLD)
	{
		// Can't trade more Gold than you have
		int iGold = iData1;
		if (iGold != -1 && iGoldAvailable < iGold)
			return false;

		// Can't demand lump Gold - too exploitable.
		if (this->GetDemandingPlayer() != NO_PLAYER && !bLumpGoldEnabled)
			return false;

		if (!bHumanToHuman)
		{
			if (this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if (this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}

			if (eFromTeam != eToTeam && !bLumpGoldEnabled)
			{
				// Can't exchange GPT for lump Gold - we aren't a bank.
				if (GetGoldPerTurnTrade(eToPlayer) > 0)
					return false;

				//cannot pay for resources with lump sums of gold
				if (IsResourceTrade(eToPlayer, NO_RESOURCE))
					return false;

				//cannot pay for vote commitments with lump sums of gold
				if (IsVoteCommitmentTrade(eToPlayer))
					return false;

				//cannot pay for embassies with lump sums of gold
				if (IsAllowEmbassyTrade(eToPlayer))
					return false;

				//cannot pay for open borders with lump sums of gold
				if (IsOpenBordersTrade(eToPlayer))
					return false;

				//cannot pay for defensive pacts with lump sums of gold
				if (IsDefensivePactTrade(eToPlayer))
					return false;

				//cannot pay for research agreements with lump sums of gold
				if (IsResearchAgreementTrade(eToPlayer))
					return false;
			}
		}
	}
	// Gold per Turn
	else if (eItem == TRADE_ITEM_GOLD_PER_TURN)
	{
		// Can't trade more GPT than you're making
		int iGoldPerTurn = iData1;
		//if a renewal deal, subtract the gold already included in the renewal.
		int iGoldRate = pFromPlayer->calculateGoldRate();
		for (uint i = 0; i < pRenewDeals.size(); i++)
		{
			CvDeal* pRenewDeal = pRenewDeals[i];
			if (pRenewDeal && pRenewDeal->GetGoldPerTurnTrade(ePlayer) > 0)
				iGoldRate += pRenewDeal->GetGoldPerTurnTrade(ePlayer);
		}

		if (iGoldPerTurn != -1 && iGoldRate < iGoldPerTurn)
		{
			return false;
		}

		if (!bHumanToHuman)
		{
			if (this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if (this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}

			// Can't exchange GPT for lump Gold - we aren't a bank.
			if (eFromTeam != eToTeam && !bLumpGoldEnabled && GetGoldTrade(eToPlayer) > 0)
				return false;

			// Cannot trade cities for gold per turn
			if (ContainsItemType(TRADE_ITEM_CITIES))
				return false;
		}

		//int iDuration = iData2;
		//if (iDuration != GC.getGame().GetDealDuration())
		//	return false;
	}
	// Resource
	else if(eItem == TRADE_ITEM_RESOURCES)
	{
		ResourceTypes eResource = (ResourceTypes) iData1;
		if (eResource != NO_RESOURCE)
		{
			int iResourceQuantity = iData2;

			// Can't trade nothing
			if (iResourceQuantity < 1)
				return false;

			if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(ePlayer, eResource) || GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(eToPlayer, eResource))
				return false;

			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if (pkResourceInfo)
			{
				TechTypes eTech = (TechTypes)pkResourceInfo->getTechObsolete();
				if (eTech != NO_TECH && (GET_PLAYER(ePlayer).HasTech(eTech) || GET_PLAYER(eToPlayer).HasTech(eTech)))
					return false;
			}

			int iNumAvailable = 0;
			if (pRenewDeals.size() > 0)
			{
				//if a renewal deal, add the resources already included in the renewal.
				for (uint i = 0; i < pRenewDeals.size(); i++)
				{
					CvDeal* pRenewDeal = pRenewDeals[i];

					iNumAvailable = pFromPlayer->getNumResourceAvailable(eResource, false);
					int iResourcesAlreadyInDeal = pRenewDeal->GetNumResourcesInDeal(ePlayer, eResource);
					if (iResourcesAlreadyInDeal > 0)
						iNumAvailable += iResourcesAlreadyInDeal;
				}
			}
			else
			{
				iNumAvailable = pFromPlayer->getNumResourceAvailable(eResource, false);
			}
			
			// Offering up more of a Resource than we have available
			if (iNumAvailable < iResourceQuantity)
			{
				return false;
			}
			
			
			// Must be a Luxury or a Strategic Resource
			ResourceUsageTypes eUsage = GC.getResourceInfo(eResource)->getResourceUsage();
			if(eUsage != RESOURCEUSAGE_LUXURY && eUsage != RESOURCEUSAGE_STRATEGIC)
				return false;

			if(eUsage == RESOURCEUSAGE_LUXURY)
			{
				int iNumAvailableOther = 0;
				//if a renewal deal, subtract the resources already included in the renewal.
				if (pRenewDeals.size() > 0)
				{
					//if a renewal deal, add the resources already included in the renewal.
					for (uint i = 0; i < pRenewDeals.size(); i++)
					{
						CvDeal* pRenewDeal = pRenewDeals[i];

						iNumAvailableOther = pToPlayer->getNumResourceAvailable(eResource);
						int iResourcesAlreadyInDeal = pRenewDeal->GetNumResourcesInDeal(ePlayer, eResource);
						if (iResourcesAlreadyInDeal > 0)
							iNumAvailableOther -= iResourcesAlreadyInDeal;
					}
				}
				else
					iNumAvailableOther = pToPlayer->getNumResourceAvailable(eResource);

				// Can't trade Luxury if the other player already has one
				if (iNumAvailableOther > 0 && !pToPlayer->GetPlayerTraits()->IsImportsCountTowardsMonopolies())
				{
					return false;
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

			if(!bHumanToHuman)
			{
				if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
				{
					if(this->GetSurrenderingPlayer() != ePlayer)
					{
						return false;
					}
				}

				//cannot pay for resources with lump sums of gold
				if (eFromTeam != eToTeam && !bLumpGoldEnabled && GetGoldTrade(eToPlayer) > 0)
					return false;
			}

			// Can't trade resource if the seller does not have the city trade tech
			if (!pFromPlayer->IsResourceCityTradeable(eResource))
			{
				return false;
			}
		}
	}
	// City
	else if(eItem == TRADE_ITEM_CITIES)
	{
		// Some game options restrict all city trades
		if (GC.getGame().IsAllCityTradingDisabled())
		{
			return false;
		}
		else if (GC.getGame().IsAICityTradingDisabled() && !bHumanToHuman)
		{
			return false;
		}
		else if (GC.getGame().IsAICityTradingHumanOnly() && !GET_PLAYER(ePlayer).isHuman() && !GET_PLAYER(eToPlayer).isHuman())
		{
			return false;
		}

		CvPlot* pPlot = GC.getMap().plot(iData1, iData2);
		CvCity* pCity = pPlot ? pPlot->getPlotCity() : NULL;
		if (pCity == NULL)
			return false;

		// Can't trade someone else's city
		if(pCity->getOwner() != ePlayer)
			return false;

		// Can't trade one's capital
		if(pCity->isCapital())
			return false;

		if (pCity->getDamage() > 0 && !pFromTeam->isAtWar(pToTeam->GetID()))
			return false;

		// Can't trade a city to a human in an OCC game
		if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && GET_PLAYER(eToPlayer).isHuman())
			return false;

		// Need an embassy in peacetime
		if (GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE && !pToTeam->HasEmbassyAtTeam(eFromTeam))
			return false;

		// Can't already have this city in the deal
		if(!bFinalizing && IsCityTrade(ePlayer, iData1, iData2))
			return false;

		//Can't give up cities in a peace deal if not surrendering.
		if(!bHumanToHuman)
		{
			// Cannot trade cities for gold per turn or third party war
			if (ContainsItemType(TRADE_ITEM_GOLD_PER_TURN) || ContainsItemType(TRADE_ITEM_THIRD_PARTY_WAR))
				return false;

			if(this->IsPeaceTreatyTrade(eToPlayer) || this->IsPeaceTreatyTrade(ePlayer))
			{
				if(this->GetSurrenderingPlayer() != ePlayer)
				{
					return false;
				}
			}
		}
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

		// Already have embassy
		if (pToTeam->HasEmbassyAtTeam(eFromTeam))
			return false;

		// Same team
		if(eFromTeam == eToTeam)
			return false;

		// If we are at war, then we can't until we make peace
		if(pFromTeam->isAtWar(eToTeam))
			return false;

		if(!bHumanToHuman && !bLumpGoldEnabled)
		{
			//cannot pay for embassies with lump sums of gold
			if (GetGoldTrade(eToPlayer) > 0)
				return false;
		}
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
		
		bool bIgnoreExistingOB = false;
		//if a renewal deal, add the resources already included in the renewal.
		for (uint i = 0; i < pRenewDeals.size(); i++)
		{
			CvDeal* pRenewDeal = pRenewDeals[i];
			if (pRenewDeal->IsOpenBordersTrade(ePlayer))
			{
				bIgnoreExistingOB = true;
				break;
			}
		}

		// Already has OB
		if (!bIgnoreExistingOB)
		{
			if (pFromTeam->IsAllowsOpenBordersToTeam(eToTeam))
				return false;
		}

		// Same Team
		if(eFromTeam == eToTeam)
			return false;

		// If we are at war, then we can't until we make peace
		if(pFromTeam->isAtWar(eToTeam))
			return false;

		if(!bHumanToHuman && !bLumpGoldEnabled)
		{
			//cannot pay for open borders with lump sums of gold
			if (GetGoldTrade(eToPlayer) > 0)
				return false;
		}
	}
	// Defensive Pact
	else if(eItem == TRADE_ITEM_DEFENSIVE_PACT)
	{
		bool bIgnoreExistingDP = false;
		//if a renewal deal, add the resources already included in the renewal.
		for (uint i = 0; i < pRenewDeals.size(); i++)
		{
			CvDeal* pRenewDeal = pRenewDeals[i];
			if (pRenewDeal->IsDefensivePactTrade(ePlayer))
			{
				bIgnoreExistingDP = true;
				break;
			}

		}

		// Not valid in a demand/request
		if (this->GetDemandingPlayer() != NO_PLAYER || this->GetRequestingPlayer() != NO_PLAYER)
			return false;

		// No DPs with vassals!
		if (pFromTeam->IsVassalOfSomeone())
			return false;

		// Neither of us yet has the Tech for DP
		if (!pFromTeam->isDefensivePactTradingAllowed() && !pToTeam->isDefensivePactTradingAllowed())
			return false;

		// Embassy has not been established
		if (!pFromTeam->HasEmbassyAtTeam(eToTeam) || !pToTeam->HasEmbassyAtTeam(eFromTeam))
			return false;

		// Can't have vassalage in the deal
		if (IsVassalageTrade(ePlayer) || IsVassalageTrade(eToPlayer))
			return false;

		// Already has DP
		if (!bIgnoreExistingDP)
		{
			if (pFromTeam->IsHasDefensivePact(eToTeam))
				return false;
		}

		// Same Team
		if (eFromTeam == eToTeam)
			return false;

		// If we are at war, then we can't until we make peace
		if(pFromTeam->isAtWar(eToTeam))
			return false;

		if(!bHumanToHuman && !bLumpGoldEnabled)
		{
			//cannot pay for defensive pacts with lump sums of gold
			if (!bLumpGoldEnabled && GetGoldTrade(eToPlayer) > 0)
				return false;
		}

		// Check to see if the other player can trade this item to us as well. If they can't, we can't trade it either
		if (bCheckOtherPlayerValidity)
		{
			if (!IsPossibleToTradeItem(eToPlayer, ePlayer, eItem, iData1, iData2, iData3, bFlag1, /*bCheckOtherPlayerValidity*/ false))
				return false;
		}
	}
	// Research Agreement
	else if(eItem == TRADE_ITEM_RESEARCH_AGREEMENT)
	{
		if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
			return false;

		// Not valid in a demand/request
		if (this->GetDemandingPlayer() != NO_PLAYER || this->GetRequestingPlayer() != NO_PLAYER)
			return false;

		// If we are at war, then we can't until we make peace
		if(pFromTeam->isAtWar(eToTeam))
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

		if(!bHumanToHuman && !bLumpGoldEnabled)
		{
			//cannot pay for research agreements with lump sums of gold
			if (GetGoldTrade(eToPlayer) > 0)
				return false;
		}

		// Check to see if the other player can trade this item to us as well. If they can't, we can't trade it either
		if(bCheckOtherPlayerValidity)
		{
			if(!IsPossibleToTradeItem(eToPlayer, ePlayer, eItem, iData1, iData2, iData3, bFlag1, /*bCheckOtherPlayerValidity*/ false))
				return false;
		}
	}
	// Peace Treaty
	else if(eItem == TRADE_ITEM_PEACE_TREATY)
	{
		if(!pFromTeam->isAtWar(eToTeam))
			return false;

		if(!pToTeam->isAtWar(eFromTeam))
			return false;

		// Failsafe check: make sure the AI doesn't make peace when they don't want to! (this is now a fairly cheap check)
		// This should not trigger any errors because peace treaty willingness is updated whenever the human opens the diplo screen with the AI, and no interactions within the screen should change willingness
		if (!pFromPlayer->isHuman() && !pFromPlayer->GetDiplomacyAI()->IsWantsPeaceWithPlayer(eToPlayer))
			return false;

		if (!pToPlayer->isHuman() && !pToPlayer->GetDiplomacyAI()->IsWantsPeaceWithPlayer(ePlayer))
			return false;
			
#if defined(MOD_EVENTS_WAR_AND_PEACE)
		if (MOD_EVENTS_WAR_AND_PEACE) 
		{
			if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_IsAbleToMakePeace, ePlayer, eToTeam) == GAMEEVENTRETURN_FALSE) 
			{
				return false;
			}

			if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_PlayerCanMakePeace, ePlayer, eToTeam) == GAMEEVENTRETURN_FALSE) 
			{
				return false;
			}
		}
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
	}
	// Third Party Peace
	else if(eItem == TRADE_ITEM_THIRD_PARTY_PEACE)
	{
		TeamTypes eThirdTeam = (TeamTypes) iData1;

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

		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if (eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eThirdTeam)
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
						if(pFromPlayer->GetPlayerNumTurnsAtWar(eLoopPlayer) < GD_INT_GET(WAR_MAJOR_MINIMUM_TURNS))
						{
							return false;
						}
						if(pOtherPlayer->GetPlayerNumTurnsAtWar(ePlayer) < GD_INT_GET(WAR_MAJOR_MINIMUM_TURNS))
						{
							return false;
						}

						//Either side can't make peace yet because of city capture?
						if(pOtherPlayer->GetPlayerNumTurnsSinceCityCapture(ePlayer) <= 1)
							return false;

						if(pFromPlayer->GetPlayerNumTurnsSinceCityCapture(eLoopPlayer) <= 1)
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
	}
	// Third Party War
	else if(eItem == TRADE_ITEM_THIRD_PARTY_WAR)
	{
		TeamTypes eThirdTeam = (TeamTypes) iData1;

		if(eThirdTeam == NO_TEAM)
			return false;

		// Can't be the same team
		if(eFromTeam == eThirdTeam)
			return false;

		// If we are at war, then we can't until we make peace
		if(pFromTeam->isAtWar(eToTeam))
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
		}

		//Can't already be offering this.
		if (!bFinalizing)
		{
			if (IsThirdPartyWarTrade(ePlayer, eThirdTeam))
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
			// Cannot trade cities for war
			if (ContainsItemType(TRADE_ITEM_CITIES))
				return false;

			if (GC.getGame().IsAllWarBribesDisabled())
			{
				return false;
			}
			if (GC.getGame().IsAIWarBribesDisabled() && !GET_PLAYER(eToPlayer).isHuman() && !GET_PLAYER(ePlayer).isHuman())
			{
				return false;
			}
		}

		// Can this player actually declare war?
		if (!pFromTeam->canDeclareWar(eThirdTeam, ePlayer))
			return false;

		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if (eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).getTeam() == eThirdTeam)
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
					// Can't ask a player to declare war if they pledged to protect them
					if(pOtherPlayer->GetMinorCivAI()->IsProtectedByMajor(ePlayer))
						return false;
					// Can't offer an attack like that either
					if(pOtherPlayer->GetMinorCivAI()->IsProtectedByMajor(eToPlayer))
						return false;
				}
			}
		}
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
		CvAssert(iNumVotes <= pLeague->GetPotentialVotesForMember(ePlayer, eToPlayer));
		
		// Can't already have a vote commitment in the deal
		if(!bFinalizing && IsVoteCommitmentTrade(ePlayer))
			return false;

		// Must be a valid proposal
		if(!pLeague->IsProposed(iID, bRepeal))
			return false;

		// This player must be allowed to
		if(!bFinalizing && !pFromPlayer->GetLeagueAI()->CanCommitVote(eToPlayer))
			return false;

		if (!bHumanToHuman)
		{
			//cannot pay for vote commitments with lump sums of gold
			if (eFromTeam != eToTeam && GetGoldTrade(eToPlayer) > 0)
				return false;
		}
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// Maps
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_MAPS)
	{
		// Both need tech for Map trading
		if (!pToTeam->isMapTrading() || !pFromTeam->isMapTrading())
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

		// Can't have a Defensive Pact in the deal
		if (IsDefensivePactTrade(eToPlayer))
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

		// Voluntary vassalage has been disallowed by game options
		if (!pFromTeam->isAtWar(eToTeam) && GC.getDIPLOAI_DISABLE_VOLUNTARY_VASSALAGE() > 0)
			return false;

		//Can't already be offering this
		if (!bFinalizing && IsVassalageTrade(ePlayer))
			return false;

		//Can't already be offering this
		if (!bFinalizing && IsRevokeVassalageTrade(ePlayer))
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
	}
	
#endif

	return true;
}

/// Get the number of resources available according to the deal being renewed and what's on the table
int CvDeal::GetNumResourceInDeal(PlayerTypes ePlayer, ResourceTypes eResource)
{
	int iNumInExistingDeal = 0;

	TradedItemList::iterator it;
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_RESOURCES && it->m_eFromPlayer == ePlayer && (ResourceTypes)it->m_iData1 == eResource)
		{
			iNumInExistingDeal += it->m_iData2;
		}
	}

	return iNumInExistingDeal;
}

int CvDeal::GetNumCitiesInDeal(PlayerTypes ePlayer)
{
	if(ePlayer == NO_PLAYER)
		return 0;

	int iNumCities = 0;
	for(TradedItemList::iterator it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if(it->m_eItemType == TRADE_ITEM_CITIES && it->m_eFromPlayer == ePlayer)
		{
			iNumCities++;
		}
	}
	return iNumCities;
}

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
		if (it->m_eItemType == TRADE_ITEM_RESOURCES && it->m_eFromPlayer == eFrom)
		{
			if (eResource == NO_RESOURCE || (ResourceTypes)it->m_iData1 == eResource)
			{
				return true;
			}
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

bool CvDeal::IsStrategicsTrade()
{
	TradedItemList::iterator it;
	for (it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if (it->m_eItemType == TRADE_ITEM_RESOURCES)
		{
			if (GC.getResourceInfo((ResourceTypes)it->m_iData1) != NULL && GC.getResourceInfo((ResourceTypes)it->m_iData1)->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
				return true;
		}

	}
	return false;
}

int CvDeal::GetNumStrategicsOnTheirSide(PlayerTypes eFrom)
{
	int iNum = 0;
	TradedItemList::iterator it;
	for (it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if (it->m_eItemType == TRADE_ITEM_RESOURCES && it->m_eFromPlayer == eFrom)
		{
			if (GC.getResourceInfo((ResourceTypes)it->m_iData1) != NULL && GC.getResourceInfo((ResourceTypes)it->m_iData1)->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
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
	case TRADE_ITEM_PEACE_TREATY:
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
		return DEAL_RENEWABLE;
		break;

		// doesn't matter
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
	for(it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
	{
		if (GetItemTradeableState(it->m_eItemType) == DEAL_RENEWABLE)
			return true;
	}
	return false;
}

/// Delete all items from a given player
bool CvDeal::RemoveAllByPlayer(PlayerTypes eOffering)
{
	//have to do this in a nested fashion to avoid invalidating the iterator
	bool bFound = false;
	bool bChange = false;
	do
	{
		bFound = false;

		TradedItemList::iterator it;
		for (it = m_TradedItems.begin(); it != m_TradedItems.end(); ++it)
		{
			//do not remove items which need to be on both sides to make sense
			if (eOffering == it->m_eFromPlayer && !it->IsTwoSided())
			{
				m_TradedItems.erase(it);
				bFound = true;
				bChange = true;
				break;
			}
		}
	}
	while (bFound);

	return bChange;
}

/// Delete a SINGLE trade item that can be identified by type alone
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

template<typename Deal, typename Visitor>
void CvDeal::Serialize(Deal& deal, Visitor& visitor)
{
	visitor(deal.m_eFromPlayer);
	visitor(deal.m_eToPlayer);
	visitor(deal.m_iFinalTurn);
	visitor(deal.m_iDuration);
	visitor(deal.m_iStartTurn);
	visitor(deal.m_bConsideringForRenewal);
	visitor(deal.m_bCheckedForRenewal);
	visitor(deal.m_bIsGift);
	visitor(deal.m_ePeaceTreatyType);
	visitor(deal.m_eSurrenderingPlayer);
	visitor(deal.m_eDemandingPlayer);
	visitor(deal.m_eRequestingPlayer);
	visitor(deal.m_TradedItems);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvDeal& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvDeal::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvDeal& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvDeal::Serialize(readFrom, serialVisitor);
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
		*pDealOut = *pDeal;

	m_ProposedDeals.erase(std::remove(m_ProposedDeals.begin(), m_ProposedDeals.end(), *pDeal), m_ProposedDeals.end());
	return true;
}

bool CvDeal::AreAllTradeItemsValid()
{
	TradedItemList::iterator iter;
	for (iter = m_TradedItems.begin(); iter != m_TradedItems.end(); ++iter)
	{
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
	CvWeightedVector<TeamTypes> veNowAtPeacePairs; // hacked CvWeighedVector to keep track of third party minors that this deal makes at peace
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

void CvGameDeals::FinalizeDealValidAndAccepted(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal& kDeal, bool bAccepted, CvWeightedVector<TeamTypes>& veNowAtPeacePairs)
{
	if (!bAccepted)
		return;

	if (kDeal.m_TradedItems.size() <= 0)
		return;

	ActivateDeal(eFromPlayer, eToPlayer, kDeal, veNowAtPeacePairs);
}

void CvGameDeals::FinalizeDealNotify(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvWeightedVector<TeamTypes>& veNowAtPeacePairs)
{
	// Update UI if we were involved in the deal
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
	if (eFromPlayer == eActivePlayer || eToPlayer == eActivePlayer)
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	// Send out a condensed notification if peace was made with third party minor civs in this deal
	if (veNowAtPeacePairs.size() > 0)
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
	bool bFoundIt = false;
	bool bValid = true;
	CvDeal kDeal;
	CvWeightedVector<TeamTypes> veNowAtPeacePairs;  // CvWeighedVector to keep track of third party minors that this deal makes at peace

	// Find the deal in the list of proposed deals
	for (DealList::iterator dealIt = m_ProposedDeals.begin(); dealIt != m_ProposedDeals.end(); dealIt++)
	{
		if (dealIt->m_eFromPlayer == eFromPlayer && dealIt->m_eToPlayer == eToPlayer)
		{
			kDeal = *dealIt;
			bFoundIt = true;
			break;
		}
	}

	if (bFoundIt)
	{
		if (!kDeal.m_bCheckedForRenewal) //we've already done this.
		{
			for (TradedItemList::iterator iter = kDeal.m_TradedItems.begin(); iter != kDeal.m_TradedItems.end(); ++iter)
			{
				if (!kDeal.IsPossibleToTradeItem(iter->m_eFromPlayer, kDeal.GetOtherPlayer(iter->m_eFromPlayer), iter->m_eItemType, iter->m_iData1, iter->m_iData2, iter->m_iData3, iter->m_bFlag1, false, true))
				{
					// mark that the deal is no longer valid. We will still delete the deal but not commit its actions
					bValid = false;
					break;
				}
			}
		}

		// Recopy all deals in the vector except this one
		DealList tempDeals(m_ProposedDeals);
		m_ProposedDeals.clear();

		for (DealList::iterator dealIt = tempDeals.begin(); dealIt != tempDeals.end(); ++dealIt)
		{
			if (dealIt->m_eFromPlayer != eFromPlayer || dealIt->m_eToPlayer != eToPlayer)
			{
				m_ProposedDeals.push_back(*dealIt);
			}
		}

		if (bValid && bAccepted)
			ActivateDeal(eFromPlayer, eToPlayer, kDeal, veNowAtPeacePairs);
	}

	// Update UI if we were involved in the deal
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
	if (eFromPlayer == eActivePlayer || eToPlayer == eActivePlayer)
	{
		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}

	//update happiness.
	GET_PLAYER(eFromPlayer).CalculateNetHappiness();
	GET_PLAYER(eToPlayer).CalculateNetHappiness();

	// Send out a condensed notification if peace was made with third party minor civs in this deal
	if (veNowAtPeacePairs.size() > 0)
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
			for (int iPairIndex = 0; iPairIndex < veNowAtPeacePairs.size(); iPairIndex++)
			{
				if (veNowAtPeacePairs.GetWeight(iPairIndex) == (int) eFromTeam)
				{
					eToTeam = veNowAtPeacePairs.GetElement(iPairIndex);
					strTemp = Localization::Lookup(GET_TEAM(eToTeam).getName().GetCString());
					strMessage = strMessage + "[NEWLINE]" + strTemp.toUTF8();
					bFromTeamMadePeace = true;
				}
			}

			// Send out notifications if there was a change
			if (bFromTeamMadePeace)
			{
				// Send out the notifications to other players
				for (int iNotifPlayerLoop = 0; iNotifPlayerLoop < MAX_MAJOR_CIVS; iNotifPlayerLoop++)
				{
					PlayerTypes eNotifPlayer = (PlayerTypes) iNotifPlayerLoop;

					if (!GET_PLAYER(eNotifPlayer).isAlive())
						continue;

					if (GET_PLAYER(eNotifPlayer).getTeam() == eFromTeam)
						continue;

					if (GET_TEAM(GET_PLAYER(eNotifPlayer).getTeam()).isHasMet(eFromTeam))  //antonjs: consider: what if eNotifPlayer hasn't met one or more of the minors that eFromTeam made peace with?
					{
						if (GET_PLAYER(eNotifPlayer).GetNotifications())
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

void CvGameDeals::ActivateDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal& kDeal, CvWeightedVector<TeamTypes>& veNowAtPeacePairs)
{
	// Determine total duration of the Deal
	int iLatestItemLastTurn = 0;
	int iLongestDuration = 0;

	// Set the surrendering player for a human v. human war (based on who puts what where).
	bool bIsPeaceDeal = kDeal.IsPeaceTreatyTrade(eFromPlayer) || kDeal.IsPeaceTreatyTrade(eToPlayer);
	bool bHumanToHuman = GET_PLAYER(eFromPlayer).isHuman() && GET_PLAYER(eToPlayer).isHuman();
	bool bShouldSetHumanSurrender = bHumanToHuman && bIsPeaceDeal;
	bool bFromPlayerItem = false, bToPlayerItem = false;

	for (TradedItemList::iterator it = kDeal.m_TradedItems.begin(); it != kDeal.m_TradedItems.end(); it++)
	{
		if (it->m_iDuration > 0)
		{
			it->m_iFinalTurn = it->m_iDuration + GC.getGame().getGameTurn();
			if (it->m_iDuration > iLongestDuration)
			{
				iLongestDuration = it->m_iDuration;
				iLatestItemLastTurn = it->m_iFinalTurn;
			}
		}
		if (bShouldSetHumanSurrender && it->m_eItemType != TRADE_ITEM_NONE && it->m_eItemType != TRADE_ITEM_PEACE_TREATY && it->m_eItemType != TRADE_ITEM_DECLARATION_OF_FRIENDSHIP && it->m_eItemType != TRADE_ITEM_DEFENSIVE_PACT && it->m_eItemType != TRADE_ITEM_RESEARCH_AGREEMENT)
		{
			if (it->m_eItemType == TRADE_ITEM_THIRD_PARTY_PEACE)
			{
				// Ignore City-State allies.
				TeamTypes eTargetTeam = (TeamTypes)it->m_iData1;
				if (eTargetTeam == NO_TEAM)
					continue;

				PlayerTypes eTargetMinor = GET_TEAM(eTargetTeam).getLeaderID();
				if (eTargetMinor == NO_PLAYER)
					continue;

				if (GET_PLAYER(eTargetMinor).isMinorCiv())
				{
					PlayerTypes eAlly = GET_PLAYER(eTargetMinor).GetMinorCivAI()->GetAlly();
					if (eAlly != NO_PLAYER)
					{
						if (GET_PLAYER(eAlly).getTeam() == GET_PLAYER(eFromPlayer).getTeam() || GET_PLAYER(eAlly).getTeam() == GET_PLAYER(eToPlayer).getTeam())
							continue;
					}
				}
			}

			if (it->m_eFromPlayer == eFromPlayer)
				bFromPlayerItem = true;
			else
				bToPlayerItem = true;
		}
	}
	if (bFromPlayerItem && !bToPlayerItem)
	{
		kDeal.SetSurrenderingPlayer(eFromPlayer);
	}
	else if (!bFromPlayerItem && bToPlayerItem)
	{
		kDeal.SetSurrenderingPlayer(eToPlayer);
	}

	kDeal.m_iDuration = iLongestDuration;
	kDeal.m_iFinalTurn = iLatestItemLastTurn;
	kDeal.m_iStartTurn = GC.getGame().getGameTurn();

	// Add to current deals
	m_CurrentDeals.push_back(kDeal);

	// Set one-time values here
	bool bDoDefensivePactNotification = true, bDoResearchAgreementNotification = true, bDoWarVictoryBonuses = true;

	// Process each item in the deal!
	for (TradedItemList::iterator it = kDeal.m_TradedItems.begin(); it != kDeal.m_TradedItems.end(); it++)
	{
		if (it->m_eItemType == TRADE_ITEM_NONE)
			continue;

		PlayerTypes eGivingPlayer = it->m_eFromPlayer;
		PlayerTypes eReceivingPlayer = kDeal.GetOtherPlayer(eGivingPlayer);
		TeamTypes eGivingTeam = GET_PLAYER(eGivingPlayer).getTeam();
		TeamTypes eReceivingTeam = GET_PLAYER(eReceivingPlayer).getTeam();

		// If the item costs Gold to give, deduct it from the treasury.
		int iCost = GetTradeItemGoldCost(it->m_eItemType, eGivingPlayer, eReceivingPlayer);
		GET_PLAYER(eGivingPlayer).GetTreasury()->ChangeGold(-iCost);

		// What happens next depends on the item type...
		switch (it->m_eItemType)
		{
		case TRADE_ITEM_GOLD:
		{
			int iGoldAmount = it->m_iData1;
			GET_PLAYER(eGivingPlayer).GetTreasury()->ChangeGold(-iGoldAmount);
			GET_PLAYER(eReceivingPlayer).GetTreasury()->ChangeGold(iGoldAmount);
			break;
		}

		case TRADE_ITEM_GOLD_PER_TURN:
		{
			int iGoldPerTurn = it->m_iData1;
			GET_PLAYER(eGivingPlayer).GetTreasury()->ChangeGoldPerTurnFromDiplomacy(-iGoldPerTurn);
			GET_PLAYER(eReceivingPlayer).GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iGoldPerTurn);
			break;
		}

		case TRADE_ITEM_RESOURCES:
		{
			ResourceTypes eResource = (ResourceTypes)it->m_iData1;
			int iResourceQuantity = it->m_iData2;
			GET_PLAYER(eGivingPlayer).changeResourceExport(eResource, iResourceQuantity);
			GET_PLAYER(eReceivingPlayer).changeResourceImportFromMajor(eResource, iResourceQuantity);

#if defined(MOD_API_ACHIEVEMENTS)
			// Resource Trading Achievements
			if (!GC.getGame().isGameMultiPlayer() && GET_PLAYER(eGivingPlayer).isHuman())
			{
				gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_RESOURCESTRADED, 100, ACHIEVEMENT_100RESOURCES);

				if (GC.getResourceInfo(eResource)->getResourceUsage() == RESOURCEUSAGE_LUXURY && (CvString)GET_PLAYER(eGivingPlayer).getLeaderTypeKey() == "LEADER_HARUN_AL_RASHID")
				{
					for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
					{
						CvBuildingEntry* pkBuildingEntry = GC.getBuildingInfo((BuildingTypes)iJ);
						if (pkBuildingEntry && (CvString)pkBuildingEntry->GetType() == "BUILDING_BAZAAR")
						{
							if (GET_PLAYER(eGivingPlayer).getBuildingClassCount((BuildingClassTypes)pkBuildingEntry->GetBuildingClassType()) >= 1)
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_SPECIAL_TRADER);
							}
						}
					}
				}
			}
#endif
			break;
		}

		case TRADE_ITEM_ALLOW_EMBASSY:
		{
			GET_TEAM(eReceivingTeam).SetHasEmbassyAtTeam(eGivingTeam, true);
			break;
		}

		case TRADE_ITEM_OPEN_BORDERS:
		{
			GET_TEAM(eGivingTeam).SetAllowsOpenBordersToTeam(eReceivingTeam, true);
			break;
		}

		case TRADE_ITEM_DECLARATION_OF_FRIENDSHIP:
		{
			GET_PLAYER(eGivingPlayer).GetDiplomacyAI()->SetDoFAccepted(eReceivingPlayer, true);
			GET_PLAYER(eReceivingPlayer).GetDiplomacyAI()->SetDoFAccepted(eGivingPlayer, true);
			break;
		}

		case TRADE_ITEM_DEFENSIVE_PACT:
		{
			GET_TEAM(eGivingTeam).SetHasDefensivePact(eReceivingTeam, true);
			GET_TEAM(eReceivingTeam).SetHasDefensivePact(eGivingTeam, true);

			if (bDoDefensivePactNotification)
			{
				GC.getGame().DoDefensivePactNotification(eGivingPlayer, eReceivingPlayer);
				bDoDefensivePactNotification = false;
			}
			break;
		}

		case TRADE_ITEM_RESEARCH_AGREEMENT:
		{
			GET_TEAM(eGivingTeam).SetHasResearchAgreement(eReceivingTeam, true);
			GET_PLAYER(eGivingPlayer).GetTreasury()->LogExpenditure(GET_PLAYER(eReceivingPlayer).getCivilizationShortDescription(), iCost, 9);

			if (bDoResearchAgreementNotification)
			{
				GC.getGame().DoResearchAgreementNotification(eGivingPlayer, eReceivingPlayer);
				bDoResearchAgreementNotification = false;
			}
			break;
		}

		case TRADE_ITEM_VOTE_COMMITMENT:
		{
			GET_PLAYER(eGivingPlayer).GetLeagueAI()->AddVoteCommitment(eReceivingPlayer, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1);
			break;
		}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		case TRADE_ITEM_MAPS:
		{
			GET_TEAM(eReceivingTeam).AcquireMap(eGivingTeam);
			break;
		}

		case TRADE_ITEM_TECHS:
		{
			TechTypes eTech = (TechTypes)it->m_iData1;
			GET_TEAM(eReceivingTeam).setHasTech(eTech, true, NO_PLAYER, true, false);

			// If No Tech Brokering is enabled then don't let the player trade this tech
			if (GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING))
			{
				GET_TEAM(eReceivingTeam).SetTradeTech(eTech, false);
			}
			break;
		}

		case TRADE_ITEM_VASSALAGE:
		{
			GET_TEAM(eGivingTeam).DoBecomeVassal(eReceivingTeam, !bIsPeaceDeal, eReceivingPlayer);
			break;
		}

		case TRADE_ITEM_VASSALAGE_REVOKE:
		{
			for (int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
			{
				TeamTypes eLoopTeam = (TeamTypes) iTeamLoop;
				bool bWasVoluntary = false;

				if (GET_TEAM(eLoopTeam).IsVassal(eGivingTeam))
				{
					bWasVoluntary = GET_TEAM(eLoopTeam).IsVoluntaryVassal(eGivingTeam);
					GET_TEAM(eLoopTeam).DoEndVassal(eGivingTeam, true, true);

					if (!bWasVoluntary)
					{
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
							if (GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getTeam() == eLoopTeam)
							{
								GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DoLiberatedFromVassalage(eReceivingTeam, true);
							}
						}
					}
				}
			}
			break;
		}
#endif

		case TRADE_ITEM_CITIES:
		{
			CvCity* pCity = GC.getMap().plot(it->m_iData1, it->m_iData2)->getPlotCity();
			GET_PLAYER(eReceivingPlayer).acquireCity(pCity, bIsPeaceDeal, !bIsPeaceDeal);
			break;
		}

		case TRADE_ITEM_THIRD_PARTY_PEACE:
		{
			TeamTypes eTargetTeam = (TeamTypes)it->m_iData1;
			bool bCityState = GET_TEAM(eTargetTeam).isMinorCiv();
			vector<PlayerTypes> vTargetTeam = GET_TEAM(eTargetTeam).getPlayers();

			// Make peace!
			GET_TEAM(eGivingTeam).makePeace(eTargetTeam, /*bBumpUnits*/ true, /*bSuppressNotification*/ bCityState, eGivingPlayer);
			GET_TEAM(eGivingTeam).setForcePeace(eTargetTeam, true);
			GET_TEAM(eTargetTeam).setForcePeace(eGivingTeam, true);

			if (bCityState)
			{
				veNowAtPeacePairs.push_back(eTargetTeam, eGivingTeam); // eGivingTeam is second so we can take advantage of CvWeightedVector's sort by weights

				// Skip global reactions for CS allies in a peace treaty.
				if (bIsPeaceDeal)
				{
					PlayerTypes eTargetMinor = GET_TEAM(eTargetTeam).getLeaderID();
					if (eTargetMinor == NO_PLAYER)
						break;

					PlayerTypes eAlly = GET_PLAYER(eTargetMinor).GetMinorCivAI()->GetAlly();
					if (eAlly != NO_PLAYER)
					{
						if (GET_PLAYER(eAlly).getTeam() == eGivingTeam || GET_PLAYER(eAlly).getTeam() == eReceivingTeam)
							break;
					}
				}
			}

			// Process global reactions
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;
				if (ePlayer == eGivingPlayer || ePlayer == eReceivingPlayer)
					continue;

				bool bObserver = GET_PLAYER(ePlayer).isObserver() && !bHumanToHuman;
				TeamTypes eTeam = bObserver ? NO_TEAM : GET_PLAYER(ePlayer).getTeam();

				if (!bObserver)
				{
					if (!GET_PLAYER(ePlayer).isMajorCiv() || !GET_PLAYER(ePlayer).isAlive())
						continue;
				}

				if (eTeam == eTargetTeam)
				{
					// If human was target, notify them.
					if (GET_PLAYER(ePlayer).isHuman())
					{
						CvNotifications* pNotify = GET_PLAYER(ePlayer).GetNotifications();
						if (pNotify)
						{
							Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_PEACE");
							strText << GET_PLAYER(eReceivingPlayer).getNameKey();
							strText << GET_PLAYER(eGivingPlayer).getCivilizationShortDescriptionKey();
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_PEACE_S");
							strSummary << GET_PLAYER(eReceivingPlayer).getCivilizationShortDescriptionKey();
							pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
						}
					}
					// If AI was target, improve opinion of the peacemaker a bit and reevaluate them.
					else
					{
						GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, -300);
						vector<PlayerTypes> v;
						v.push_back(eReceivingPlayer);
						GET_PLAYER(ePlayer).GetDiplomacyAI()->DoReevaluatePlayers(v, false, false);
					}
				}
				// Notify all other civs
				else if (bObserver || eTeam == eGivingTeam || eTeam == eReceivingTeam || (GET_TEAM(eTeam).isHasMet(eTargetTeam) && GET_TEAM(eTeam).isHasMet(eGivingTeam)) || (GET_PLAYER(ePlayer).isHuman() && GC.getGame().IsDiploDebugModeEnabled() && !bHumanToHuman))
				{
					if (bObserver || GET_PLAYER(ePlayer).isHuman())
					{
						CvNotifications* pNotify = GET_PLAYER(ePlayer).GetNotifications();
						if (pNotify)
						{
							bool bMetBroker = bObserver || eTeam == eGivingTeam || eTeam == eReceivingTeam || GET_TEAM(eTeam).isHasMet(eReceivingTeam) || (GC.getGame().IsDiploDebugModeEnabled() && !bHumanToHuman);
							Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_PEACE_OTHER");
							if (bMetBroker)
							{
								strText << GET_PLAYER(eReceivingPlayer).getNameKey();
							}
							else
							{
								strText << Localization::Lookup("TXT_KEY_UNMET_PLAYER");
							}
							strText << GET_PLAYER(eGivingPlayer).getCivilizationShortDescriptionKey();
							strText << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_PEACE_OTHER_S");
							if (bMetBroker)
							{
								strSummary << GET_PLAYER(eReceivingPlayer).getCivilizationShortDescriptionKey();
							}
							else
							{
								strSummary << Localization::Lookup("TXT_KEY_UNMET_PLAYER");
							}
							strSummary << GET_PLAYER(eGivingPlayer).getCivilizationShortDescriptionKey();
							strSummary << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
							pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
						}
					}
					else if (eTeam != eGivingTeam && eTeam != eReceivingTeam && GET_TEAM(eTeam).isHasMet(eReceivingTeam))
					{
						if (!bCityState)
						{
							// Players at war with the target - penalty to recent assistance for the peacemaker and the warrior!
							if (GET_TEAM(eTeam).isAtWar(eTargetTeam))
							{
								GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
								GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
							}
							// DPs/vassals/friends of the target - bonus to recent assistance for the peacemaker!
							else if (GET_TEAM(eTeam).IsHasDefensivePact(eTargetTeam) || GET_TEAM(eTeam).IsVassal(eTargetTeam))
							{
								GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, -300);
							}
							else
							{
								for (size_t i=0; i<vTargetTeam.size(); i++)
								{
									if (!GET_PLAYER(vTargetTeam[i]).isAlive() || !GET_PLAYER(vTargetTeam[i]).isMajorCiv() || GET_PLAYER(vTargetTeam[i]).getNumCities() <= 0)
										continue;

									if (GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDoFAccepted(vTargetTeam[i]))
									{
										GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, -300);
										break;
									}
								}
							}
						}
						// Allies, PTPs, friends of the City-State - bonus to recent assistance for the peacemaker!
						else
						{
							PlayerTypes eTargetMinor = GET_TEAM(eTargetTeam).getLeaderID();
							if (eTargetMinor == NO_PLAYER)
								continue;

							if (!GET_PLAYER(eTargetMinor).isMinorCiv() || GET_PLAYER(eTargetMinor).getNumCities() <= 0)
								continue;

							if (GET_PLAYER(eTargetMinor).GetMinorCivAI()->IsAllies(ePlayer))
							{
								GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, -300);
							}
							else if (GET_PLAYER(eTargetMinor).GetMinorCivAI()->IsFriends(ePlayer) || GET_PLAYER(eTargetMinor).GetMinorCivAI()->IsProtectedByMajor(ePlayer))
							{
								if (GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDiplomat() || GET_PLAYER(ePlayer).GetPlayerTraits()->IsDiplomat()
									|| GET_PLAYER(ePlayer).GetDiplomacyAI()->IsGoingForDiploVictory() || GET_PLAYER(ePlayer).GetDiplomacyAI()->IsCloseToDiploVictory())
								{
									GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, -300);
								}
								else
								{
									GET_PLAYER(ePlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, -150);
								}
							}								
						}
					}
				}
			}
			break;
		}

		case TRADE_ITEM_THIRD_PARTY_WAR:
		{
			TeamTypes eTargetTeam = (TeamTypes)it->m_iData1;
			bool bCityState = GET_TEAM(eTargetTeam).isMinorCiv();
			vector<PlayerTypes> vTargetTeam = GET_TEAM(eTargetTeam).getPlayers();

			// First loop, check to see who established surveillance
			vector<PlayerTypes> vPlayersWithSurveillance;
			vector<PlayerTypes> vDebugModePlayers;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
			{
				PlayerTypes ePlayer = (PlayerTypes) iPlayerLoop;
				if (ePlayer == eGivingPlayer || ePlayer == eReceivingPlayer)
					continue;

				if (GET_PLAYER(ePlayer).isObserver() && !bHumanToHuman) // Notify the observer of what transpired.
				{
					CvNotifications* pNotify = GET_PLAYER(ePlayer).GetNotifications();
					if (pNotify)
					{
						Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_OTHER_NOT_INFORMED");
						strText << GET_PLAYER(eReceivingPlayer).getNameKey();
						strText << GET_PLAYER(eGivingPlayer).getCivilizationShortDescriptionKey();
						strText << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_OTHER_S");
						strSummary << GET_PLAYER(eReceivingPlayer).getCivilizationShortDescriptionKey();
						strSummary << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
						pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
					}
					continue;
				}

				if (!GET_PLAYER(ePlayer).isAlive() || !GET_PLAYER(ePlayer).isMajorCiv())
					continue;

				if (GET_PLAYER(ePlayer).getTeam() == eGivingTeam || GET_PLAYER(ePlayer).getTeam() == eReceivingTeam)
				{
					vPlayersWithSurveillance.push_back(ePlayer);
					continue;
				}

				if (GET_PLAYER(ePlayer).isHuman() && GC.getGame().IsDiploDebugModeEnabled() && !bHumanToHuman)
				{
					vDebugModePlayers.push_back(ePlayer);
				}

				// Must have met both teams
				if (!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eGivingTeam) || !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(eReceivingTeam))
					continue;

				if (!GET_PLAYER(ePlayer).GetEspionage())
					continue;

				if (GET_PLAYER(ePlayer).GetEspionage()->IsAnySurveillanceEstablished(eGivingPlayer) || GET_PLAYER(ePlayer).GetEspionage()->IsAnySurveillanceEstablished(eReceivingPlayer))
					vPlayersWithSurveillance.push_back(ePlayer);
			}

			// Now process global reactions (anyone who establishes surveillance informs their friends and allies of what they learned!)
			vector<PlayerTypes> vNotifiedPlayers;

			// First, players notify themselves
			for (std::vector<PlayerTypes>::iterator iter = vPlayersWithSurveillance.begin(); iter != vPlayersWithSurveillance.end(); iter++)
			{
				TeamTypes eTeam = GET_PLAYER(*iter).getTeam();

				// If human, notify the human.
				if (GET_PLAYER(*iter).isHuman())
				{
					CvNotifications* pNotify = GET_PLAYER(*iter).GetNotifications();
					if (pNotify)
					{
						if (eTeam == eTargetTeam)
						{
							Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR");
							strText << GET_PLAYER(eReceivingPlayer).getNameKey();
							strText << GET_PLAYER(eGivingPlayer).getCivilizationShortDescriptionKey();
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_S");
							strSummary << GET_PLAYER(eReceivingPlayer).getCivilizationShortDescriptionKey();
							pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
						}
						else
						{
							Localization::String strText = (eTeam == eGivingTeam || eTeam == eReceivingTeam) ? Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_OTHER_NOT_INFORMED") : Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_OTHER");
							strText << GET_PLAYER(eReceivingPlayer).getNameKey();
							strText << GET_PLAYER(eGivingPlayer).getCivilizationShortDescriptionKey();
							strText << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_OTHER_S");
							strSummary << GET_PLAYER(eReceivingPlayer).getCivilizationShortDescriptionKey();
							strSummary << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
							pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
						}
					}
				}
				// If AI, adjust opinion of the broker and the warrior, if appropriate.
				else
				{
					if (!bCityState)
					{
						// People at war with the target - bonus to opinion!
						if (GET_TEAM(eTeam).isAtWar(eTargetTeam))
						{
							GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, -300);
						}
						// The target and their allies & friends - penalty to opinion!
						else if (eTeam == eTargetTeam || GET_TEAM(eTeam).IsHasDefensivePact(eTargetTeam) || GET_TEAM(eTeam).IsVassal(eTargetTeam) || GET_TEAM(eTargetTeam).IsVassal(eTeam))
						{
							GET_PLAYER(*iter).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eGivingPlayer, 1);
							GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
							GET_PLAYER(*iter).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eReceivingPlayer, 1);
							GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
						}
						else
						{
							for (size_t i=0; i<vTargetTeam.size(); i++)
							{
								if (!GET_PLAYER(vTargetTeam[i]).isAlive() || !GET_PLAYER(vTargetTeam[i]).isMajorCiv())
									continue;

								if (GET_PLAYER(*iter).GetDiplomacyAI()->IsDoFAccepted(vTargetTeam[i]))
								{
									GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
									GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
									break;
								}
							}
						}
					}
					// Allies, PTPs and friends of the City-State - penalty to opinion!
					else
					{
						PlayerTypes eMinor = GET_TEAM(eTargetTeam).getLeaderID();

						if (GET_PLAYER(eMinor).isMinorCiv())
						{
							PlayerTypes eAlly = GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly();

							if (eAlly != NO_PLAYER && GET_PLAYER(eAlly).getTeam() == eTeam)
							{
								GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
								GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
							}
							else if (GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(*iter))
							{
								GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
								GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
							}
							else if (GET_PLAYER(eMinor).GetMinorCivAI()->IsFriends(*iter))
							{
								if (GET_PLAYER(*iter).GetDiplomacyAI()->IsDiplomat() || GET_PLAYER(*iter).GetPlayerTraits()->IsDiplomat()
									|| GET_PLAYER(*iter).GetDiplomacyAI()->IsGoingForDiploVictory() || GET_PLAYER(*iter).GetDiplomacyAI()->IsCloseToDiploVictory())
								{
									GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
									GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
								}
								else
								{
									GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 150);
									GET_PLAYER(*iter).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 150);
								}
							}
						}
					}
				}

				vNotifiedPlayers.push_back(*iter);
			}
			// Next, anyone with surveillance notifies their allies
			for (std::vector<PlayerTypes>::iterator iter = vPlayersWithSurveillance.begin(); iter != vPlayersWithSurveillance.end(); iter++)
			{
				TeamTypes eTeam = GET_PLAYER(*iter).getTeam();

				// If on the broker or warrior's team, we don't tell anyone else.
				if (eTeam == eGivingTeam || eTeam == eReceivingTeam)
					continue;

				for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
				{
					PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
					TeamTypes eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

					// Don't notify the broker or warrior's team.
					if (eLoopTeam == eGivingTeam || eLoopTeam == eReceivingTeam)
						continue;

					// Must have met both teams
					if (!GET_TEAM(eLoopTeam).isHasMet(eGivingTeam) || !GET_TEAM(eLoopTeam).isHasMet(eReceivingTeam))
						continue;

					if (eTeam == eLoopTeam || GET_TEAM(eTeam).IsHasDefensivePact(eLoopTeam) || GET_TEAM(eTeam).IsVassal(eLoopTeam) || GET_TEAM(eLoopTeam).IsVassal(eTeam) || GET_PLAYER(*iter).GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer))
					{
						// If human, notify the human.
						if (GET_PLAYER(eLoopPlayer).isHuman())
						{
							// But not more than once!
							if (std::find(vNotifiedPlayers.begin(), vNotifiedPlayers.end(), eLoopPlayer) != vNotifiedPlayers.end())
								continue;

							CvNotifications* pNotify = GET_PLAYER(eLoopPlayer).GetNotifications();
							vNotifiedPlayers.push_back(eLoopPlayer);

							// If they're on the target team, the message is different.
							if (eLoopTeam == eTargetTeam)
							{
								Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_INFORMED");
								strText << GET_PLAYER(*iter).getCivilizationShortDescriptionKey();
								strText << GET_PLAYER(eReceivingPlayer).getNameKey();
								strText << GET_PLAYER(eGivingPlayer).getCivilizationShortDescriptionKey();
								Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_S");
								strSummary << GET_PLAYER(eReceivingPlayer).getCivilizationShortDescriptionKey();
								pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
							}
							else
							{
								Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_OTHER_INFORMED");
								strText << GET_PLAYER(*iter).getCivilizationShortDescriptionKey();
								strText << GET_PLAYER(eReceivingPlayer).getNameKey();
								strText << GET_PLAYER(eGivingPlayer).getCivilizationShortDescriptionKey();
								strText << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
								Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_OTHER_S");
								strSummary << GET_PLAYER(eReceivingPlayer).getCivilizationShortDescriptionKey();
								strSummary << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
								pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
							}
						}
						// If AI, adjust opinion of the broker and the warrior, if appropriate.
						else
						{
							// Diplo bonus for sharing intrigue!
							GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesIntrigueSharedBy(*iter, 1);

							// Don't apply penalties more than once.
							if (std::find(vNotifiedPlayers.begin(), vNotifiedPlayers.end(), eLoopPlayer) != vNotifiedPlayers.end())
								continue;

							vNotifiedPlayers.push_back(eLoopPlayer);

							if (!bCityState)
							{
								// People at war with the target - bonus to opinion!
								if (GET_TEAM(eLoopTeam).isAtWar(eTargetTeam))
								{
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, -300);
								}
								// The target and their allies & friends - penalty to opinion!
								else if (eLoopTeam == eTargetTeam || GET_TEAM(eLoopTeam).IsHasDefensivePact(eTargetTeam) || GET_TEAM(eLoopTeam).IsVassal(eTargetTeam) || GET_TEAM(eTargetTeam).IsVassal(eLoopTeam))
								{
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eGivingPlayer, 1);
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(eReceivingPlayer, 1);
									GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
								}
								else
								{
									for (size_t i=0; i<vTargetTeam.size(); i++)
									{
										if (!GET_PLAYER(vTargetTeam[i]).isAlive() || !GET_PLAYER(vTargetTeam[i]).isMajorCiv())
											continue;

										if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsDoFAccepted(vTargetTeam[i]))
										{
											GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
											GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
											break;
										}
									}
								}
							}
							// Allies, PTPs and friends of the City-State - penalty to opinion!
							else
							{
								PlayerTypes eMinor = GET_TEAM(eTargetTeam).getLeaderID();

								if (GET_PLAYER(eMinor).isMinorCiv() && GET_PLAYER(eMinor).getNumCities() > 0)
								{
									PlayerTypes eAlly = GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly();

									if (eAlly != NO_PLAYER && GET_PLAYER(eAlly).getTeam() == eLoopTeam)
									{
										GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
										GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
									}
									else if (GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(eLoopPlayer))
									{
										GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
										GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
									}
									else if (GET_PLAYER(eMinor).GetMinorCivAI()->IsFriends(eLoopPlayer))
									{
										if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsDiplomat() || GET_PLAYER(eLoopPlayer).GetPlayerTraits()->IsDiplomat()
											|| GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsGoingForDiploVictory() || GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsCloseToDiploVictory())
										{
											GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 300);
											GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 300);
										}
										else
										{
											GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eGivingPlayer, 150);
											GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeRecentAssistValue(eReceivingPlayer, 150);
										}
									}
								}
							}
						}
					}
				}
			}
			// Notify any humans who have debug mode enabled
			for (std::vector<PlayerTypes>::iterator iter = vDebugModePlayers.begin(); iter != vDebugModePlayers.end(); iter++)
			{
				if (std::find(vNotifiedPlayers.begin(), vNotifiedPlayers.end(), *iter) != vNotifiedPlayers.end())
					continue;

				CvNotifications* pNotify = GET_PLAYER(*iter).GetNotifications();
				if (pNotify)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_OTHER_NOT_INFORMED");
					strText << GET_PLAYER(eReceivingPlayer).getNameKey();
					strText << GET_PLAYER(eGivingPlayer).getCivilizationShortDescriptionKey();
					strText << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_THIRD_PARTY_BROKER_WAR_OTHER_S");
					strSummary << GET_PLAYER(eReceivingPlayer).getCivilizationShortDescriptionKey();
					strSummary << GET_PLAYER(GET_TEAM(eTargetTeam).getLeaderID()).getCivilizationShortDescriptionKey();
					pNotify->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
				}
			}

			// Declare war!
			if (GET_PLAYER(eGivingPlayer).GetDiplomacyAI()->DeclareWar(eTargetTeam))
			{
				int iLockedTurns = /*15*/ GC.getCOOP_WAR_LOCKED_LENGTH();
				GET_TEAM(eGivingTeam).ChangeNumTurnsLockedIntoWar(eTargetTeam, iLockedTurns);
			}
			else
			{
				break;
			}

			// All AI players on the attacking team go to war now.
			vector<PlayerTypes> vAttackingTeam = GET_TEAM(eGivingTeam).getPlayers();
			for (size_t i=0; i<vAttackingTeam.size(); i++)
			{
				if (!GET_PLAYER(vAttackingTeam[i]).isAlive() || !GET_PLAYER(vAttackingTeam[i]).isMajorCiv() || GET_PLAYER(vAttackingTeam[i]).isHuman() || GET_PLAYER(vAttackingTeam[i]).getNumCities() <= 0)
					continue;

				for (size_t j=0; j<vTargetTeam.size(); j++)
				{
					if (!GET_PLAYER(vTargetTeam[j]).isAlive() || GET_PLAYER(vTargetTeam[j]).getNumCities() <= 0)
						continue;

					bool bCareful = GET_PLAYER(vAttackingTeam[i]).CountNumDangerousMajorsAtWarWith(true, false) > 0 && GET_PLAYER(vAttackingTeam[i]).GetDiplomacyAI()->GetGlobalCoopWarAgainstState(vTargetTeam[j]) < COOP_WAR_STATE_PREPARING;

					if (!GET_PLAYER(vAttackingTeam[i]).HasAnyOffensiveOperationsAgainstPlayer(vTargetTeam[j]))
					{
						GET_PLAYER(vAttackingTeam[i]).GetMilitaryAI()->RequestCityAttack(vTargetTeam[j], 2, bCareful);
					}
				}
			}

			// Notified players reevaluate the broker!
			for (std::vector<PlayerTypes>::iterator iter = vNotifiedPlayers.begin(); iter != vNotifiedPlayers.end(); iter++)
			{
				if (!GET_PLAYER(*iter).isHuman())
				{
					vector<PlayerTypes> v;
					v.push_back(eReceivingPlayer);
					GET_PLAYER(*iter).GetDiplomacyAI()->DoReevaluatePlayers(v);
				}
			}

			break;
		}

		// **** Peace Treaty **** this should always be the last item processed!!!
		case TRADE_ITEM_PEACE_TREATY:
		{
			if (bDoWarVictoryBonuses)
			{
				if (GET_PLAYER(eReceivingPlayer).GetDiplomacyAI()->GetWarScore(eGivingPlayer) >= 25)
				{
					GET_PLAYER(eReceivingPlayer).DoWarVictoryBonuses();
				}
				else if (GET_PLAYER(eGivingPlayer).GetDiplomacyAI()->GetWarScore(eReceivingPlayer) >= 25)
				{
					GET_PLAYER(eGivingPlayer).DoWarVictoryBonuses();
				}

				bDoWarVictoryBonuses = false;
			}

			GET_TEAM(eGivingTeam).makePeace(eReceivingTeam, true, false, eGivingPlayer);
			GET_TEAM(eGivingTeam).setForcePeace(eReceivingTeam, true);

			break;
		}
		}
	}

	LogDealComplete(&kDeal);
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

		// Check to see if any of our RENEWABLE TradeItems in any of our Deals expire this turn
		for (it = m_CurrentDeals.begin(); it != m_CurrentDeals.end(); ++it)
		{
			it->m_bConsideringForRenewal = false;

			//humans deal with their own renewals - no diplo AI prompting.
			bool bHumanToHuman = false;
			if (GET_PLAYER(it->GetFromPlayer()).isHuman() && GET_PLAYER(it->GetToPlayer()).isHuman())
				bHumanToHuman = true;

			//if we can renew this deal, we're going to send it to the Diplo AI first.
			if (!bHumanToHuman && !it->IsPotentiallyRenewable())
				continue;

			// if this deal is a gift or peace deal, move on.
			if (it->m_bIsGift || it->IsPeaceTreatyTrade(it->GetFromPlayer()) || it->IsPeaceTreatyTrade(it->GetToPlayer()))
			{
				continue;
			}

			TradedItemList::iterator itemIter;
			for (itemIter = it->m_TradedItems.begin(); itemIter != it->m_TradedItems.end(); ++itemIter)
			{
				int iFinalTurn = itemIter->m_iFinalTurn;
				CvAssertMsg(iFinalTurn >= -1, "DEAL: Trade item has a negative final turn.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
				CvAssertMsg(iFinalTurn < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a final turn way beyond the end of the game.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
				CvAssertMsg(itemIter->m_iDuration < GC.getGame().getEstimateEndTurn() * 2, "DEAL: Trade item has a crazy long duration (probably invalid).  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
				CvAssertMsg(itemIter->m_eFromPlayer == it->m_eFromPlayer || itemIter->m_eFromPlayer == it->m_eToPlayer, "DEAL: Processing turn for a deal that has an item for a player that's not actually in this deal!  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

				if (iFinalTurn > -1 && iFinalTurn == iGameTurn)
				{
					bSomethingChanged = true;
					it->m_bConsideringForRenewal = true;
					break;
				}
			}
		}

		//we only want one renewable deal per player. We clean that up here.
		//DoTurnPost();

		// Check to see if any of our NONRENEWABLE TradeItems in any of our Deals expire this turn
		for(it = m_CurrentDeals.begin(); it != m_CurrentDeals.end(); ++it)
		{
			//if we can renew this deal, we're going to send it to the Diplo AI first.
			if (it->m_bConsideringForRenewal)
				continue;

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
					bSomethingChanged = true;

					eFromPlayer = itemIter->m_eFromPlayer;
					eToPlayer = it->GetOtherPlayer(eFromPlayer);

					DoEndTradedItem(&*itemIter, eToPlayer, false);
				}
			}
		}

		// check to see if one of our deals in no longer valid
		for (it = m_CurrentDeals.begin(); it != m_CurrentDeals.end(); ++it)
		{
			TradedItemList::iterator itemIter;
			bool bInvalidDeal = false;
			bool bUnbreakable = false;
			for (itemIter = it->m_TradedItems.begin(); itemIter != it->m_TradedItems.end(); ++itemIter)
			{
				int iFinalTurn = itemIter->m_iFinalTurn;
				if (iFinalTurn > -1 && iFinalTurn != iGameTurn)  // if this was the last turn the deal was ending anyways
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
					if (itemIter->m_eItemType == TRADE_ITEM_RESOURCES)
					{
						ResourceTypes eResource = (ResourceTypes)itemIter->m_iData1;
						//int iResourceQuantity = itemIter->m_iData2;
						bHaveEnoughResource = GET_PLAYER(eFromPlayer).getNumResourceTotal(eResource) >= 0;
					}
					else if (itemIter->m_eItemType == TRADE_ITEM_PEACE_TREATY)
					{
						bUnbreakable = true;
						break;
					}

					if (!bHaveEnoughGold || !bHaveEnoughResource)
					{
						bInvalidDeal = true;
					}
				}
			}

			if (!bUnbreakable && bInvalidDeal)
			{
				bSomethingChanged = true;
				it->m_iFinalTurn = iGameTurn;

				for (itemIter = it->m_TradedItems.begin(); itemIter != it->m_TradedItems.end(); ++itemIter)
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

/// Update deals for after DiploAI
void CvGameDeals::DoTurnPost()
{
	DoUpdateCurrentDealsList();
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
		//empty deals should be cleared out.
		if (it->GetNumItems() <= 0)
			continue;

		if (it->m_iFinalTurn <= GC.getGame().getGameTurn() && (!it->m_bConsideringForRenewal || it->m_bCheckedForRenewal))
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
void CvGameDeals::DoCancelAllProposedMPDealsWithPlayer(PlayerTypes eCancelPlayer, DiplomacyMode eTargetPlayers)
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
void CvGameDeals::DoEndTradedItem(CvTradedItem* pItem, PlayerTypes eToPlayer, bool bCancelled, bool bSkip)
{
	CvString strBuffer;
	CvString strSummary;

	PlayerTypes eFromPlayer = pItem->m_eFromPlayer;

	CvPlayerAI& fromPlayer = GET_PLAYER(eFromPlayer);
	CvPlayerAI& toPlayer = GET_PLAYER(eToPlayer);

	TeamTypes eFromTeam = fromPlayer.getTeam();
	TeamTypes eToTeam = toPlayer.getTeam();

	CvNotifications* pNotifications = NULL;

	// Gold Per Turn
	if(pItem->m_eItemType == TRADE_ITEM_GOLD_PER_TURN)
	{
		if (!bSkip)
		{
			int iGoldPerTurn = pItem->m_iData1;
			fromPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iGoldPerTurn);
			toPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(-iGoldPerTurn);
		}

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

		if (!bSkip)
		{
			int iResourceQuantity = pItem->m_iData2;
			fromPlayer.changeResourceExport(eResource, -iResourceQuantity);
			toPlayer.changeResourceImportFromMajor(eResource, -iResourceQuantity);
		}

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
		GET_TEAM(eToTeam).SetHasDefensivePact(eFromTeam, false);

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
#if defined(MOD_BALANCE_CORE)
			CvCity* pCapital = toPlayer.getCapitalCity();
			if (pCapital)
			{
				toPlayer.doInstantYield(INSTANT_YIELD_TYPE_RESEARCH_AGREMEENT, false, NO_GREATPERSON, NO_BUILDING, iBeakersBonus, false, NO_PLAYER, NULL, false, pCapital, false, false, false, YIELD_SCIENCE);
			}
			else
#endif
			if(eCurrentTech == NO_TECH)
			{
				toPlayer.changeOverflowResearch(iBeakersBonus);
#if defined(MOD_BALANCE_CORE)
				toPlayer.changeInstantYieldValue(YIELD_SCIENCE, iBeakersBonus);
#endif
			}
			else
			{
				kTeam.GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iBeakersBonus, eToPlayer);
#if defined(MOD_BALANCE_CORE)
				toPlayer.changeInstantYieldValue(YIELD_SCIENCE, iBeakersBonus);
#endif
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

	if (eItem == TRADE_ITEM_RESEARCH_AGREEMENT)
	{
		iGoldCost = GC.getGame().GetResearchAgreementCost(ePlayer1, ePlayer2);
	}

	return iGoldCost;
}

/// Mark elements in the deal as renewed depending on if they are in both deals
void CvGameDeals::PrepareRenewDeal(CvDeal* pOldDeal)
{
	CvPlayerAI& fromPlayer = GET_PLAYER(pOldDeal->m_eFromPlayer);
	CvPlayerAI& toPlayer = GET_PLAYER(pOldDeal->m_eToPlayer);

	TradedItemList::iterator oldDealItemIter;
	for (oldDealItemIter = pOldDeal->m_TradedItems.begin(); oldDealItemIter != pOldDeal->m_TradedItems.end(); ++oldDealItemIter)
	{
		// if this is not a renewable item, ignore
		if (CvDeal::GetItemTradeableState(oldDealItemIter->m_eItemType) != CvDeal::DEAL_RENEWABLE)
		{
			continue;
		}

		TradeableItems eItemType = oldDealItemIter->m_eItemType;
		if (eItemType == TRADE_ITEM_RESOURCES)
		{
			ResourceTypes eResource = (ResourceTypes)oldDealItemIter->m_iData1;

			// quantity
			int iOldResourceAmount = oldDealItemIter->m_iData2;
			if (oldDealItemIter->m_eFromPlayer == pOldDeal->m_eFromPlayer)
			{
				fromPlayer.changeResourceExport(eResource, iOldResourceAmount*-1);
				toPlayer.changeResourceImportFromMajor(eResource, iOldResourceAmount*-1);
			}
			else
			{
				toPlayer.changeResourceExport(eResource, iOldResourceAmount*-1);
				fromPlayer.changeResourceImportFromMajor(eResource, iOldResourceAmount*-1);
			}
		}

		if (oldDealItemIter->m_eItemType == TRADE_ITEM_GOLD_PER_TURN)
		{
			int iOldGPTAmount = oldDealItemIter->m_iData1;

			if (oldDealItemIter->m_eFromPlayer == pOldDeal->m_eFromPlayer)
			{
				fromPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iOldGPTAmount);
				toPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iOldGPTAmount * -1);
			}
			else
			{
				toPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iOldGPTAmount);
				fromPlayer.GetTreasury()->ChangeGoldPerTurnFromDiplomacy(iOldGPTAmount * -1);
			}
		}
	}
	pOldDeal->m_bCheckedForRenewal = true;
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

		int iTotalValue = GET_PLAYER(pDeal->GetFromPlayer()).GetDealAI()->GetDealValue(pDeal, true);
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

		strTemp.Format("DEAL COMPLETE: Deal Net Value: %d, From Player Value: %d, To Player Value: %d", iTotalValue, pDeal->GetFromPlayerValue(), pDeal->GetToPlayerValue());
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

bool CvGameDeals::IsReceivingItemsFromPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, bool bMutual)
{
	DealList::iterator iter;
	DealList::iterator end = m_CurrentDeals.end();

	bool bWeGetSomething = false;
	bool bTheyGetSomething = false;

	for (iter = m_CurrentDeals.begin(); iter != end; ++iter)
	{
		if ((iter->m_eToPlayer == ePlayer || iter->m_eFromPlayer == ePlayer) &&
			(iter->m_eToPlayer == eOtherPlayer || iter->m_eFromPlayer == eOtherPlayer))
		{
			int iEndTurn = iter->GetEndTurn();
			if (iEndTurn <= GC.getGame().getGameTurn())
				continue;

			if (iter->GetGoldPerTurnTrade(eOtherPlayer) > 0)
				bWeGetSomething = true;

			if (iter->GetGoldPerTurnTrade(ePlayer) > 0)
				bTheyGetSomething = true;

			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResource = (ResourceTypes) iResourceLoop;

				const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if (pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
					continue;

				if (iter->GetNumResourcesInDeal(eOtherPlayer, eResource) > 0)
					bWeGetSomething = true;

				if (iter->GetNumResourcesInDeal(ePlayer, eResource) > 0)
					bTheyGetSomething = true;
			}

			if (bMutual && bWeGetSomething && bTheyGetSomething)
			{
				return true;
			}
			else if (!bMutual && bWeGetSomething)
			{
				return true;
			}
		}
	}

	return false;
}

int CvGameDeals::GetDealValueWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, bool bConsiderDuration)
{
	DealList::iterator iter;
	DealList::iterator end = m_CurrentDeals.end();

	int iVal = 0;
	int iAvgDealDuration = GC.getGame().GetDealDuration() / 2;
	for (iter = m_CurrentDeals.begin(); iter != end; ++iter)
	{
		if ((iter->m_eToPlayer == ePlayer || iter->m_eFromPlayer == ePlayer) &&
			(iter->m_eToPlayer == eOtherPlayer || iter->m_eFromPlayer == eOtherPlayer))
		{
			int iEndTurn = iter->GetEndTurn();
			if (iEndTurn <= GC.getGame().getGameTurn())
				continue;

			if (bConsiderDuration)
			{
				iVal += iter->GetGoldPerTurnTrade(eOtherPlayer) * (iter->GetEndTurn() - GC.getGame().getGameTurn());
			}
			else
			{
				iVal += iter->GetGoldPerTurnTrade(eOtherPlayer) * iAvgDealDuration;
			}

			//Resources
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				ResourceTypes eResource = (ResourceTypes) iResourceLoop;

				const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if (pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_BONUS)
					continue;

				if (bConsiderDuration)
				{
					iVal += iter->GetNumResourcesInDeal(eOtherPlayer, eResource) * 5 * (iter->GetEndTurn() - GC.getGame().getGameTurn());
				}
				else
				{
					iVal += iter->GetNumResourcesInDeal(eOtherPlayer, eResource) * 5 * iAvgDealDuration;
				}
			}

			iVal += iter->IsOpenBordersTrade(eOtherPlayer) ? 10 * iAvgDealDuration : 0;
			iVal += iter->IsOpenBordersTrade(ePlayer) ? 5 * iAvgDealDuration : 0;
			iVal += iter->IsDefensivePactTrade(eOtherPlayer) ? 50 * iAvgDealDuration : 0;
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

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
std::vector<CvDeal*> CvGameDeals::GetRenewableDealsWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, uint iMaxCount)
{

	std::vector<CvDeal*> renewDeals;
	for (uint i = 0; i < m_CurrentDeals.size(); i++)
	{
		CvDeal& kDeal = m_CurrentDeals[i];

		if (!kDeal.m_bConsideringForRenewal)
			continue;

		if ((kDeal.m_eToPlayer == ePlayer || kDeal.m_eFromPlayer == ePlayer) &&
			(kDeal.m_eToPlayer == eOtherPlayer || kDeal.m_eFromPlayer == eOtherPlayer))
		{
			
			renewDeals.push_back(&kDeal);
			if (renewDeals.size() >= iMaxCount)
				break;
		}
	}

	return renewDeals;
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

template<typename GameDeals, typename Visitor>
void CvGameDeals::Serialize(GameDeals& gameDeals, Visitor& visitor)
{
	visitor(gameDeals.m_ProposedDeals);
	visitor(gameDeals.m_CurrentDeals);
	visitor(gameDeals.m_HistoricalDeals);
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvGameDeals& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvGameDeals::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvGameDeals& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvGameDeals::Serialize(readFrom, serialVisitor);
	return saveTo;
}
