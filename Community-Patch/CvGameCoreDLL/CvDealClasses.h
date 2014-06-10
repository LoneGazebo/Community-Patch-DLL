/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CV_DEAL_CLASSES_H
#define CV_DEAL_CLASSES_H

#include "CvDiplomacyAIEnums.h"

enum TradeableItems
{
	TRADE_ITEM_NONE = -1,
	TRADE_ITEM_GOLD,
	TRADE_ITEM_GOLD_PER_TURN,
	TRADE_ITEM_MAPS,
	TRADE_ITEM_RESOURCES,
	TRADE_ITEM_CITIES,
	TRADE_ITEM_UNITS,
	TRADE_ITEM_OPEN_BORDERS,
	TRADE_ITEM_DEFENSIVE_PACT,
	TRADE_ITEM_RESEARCH_AGREEMENT,
	TRADE_ITEM_TRADE_AGREEMENT,
	TRADE_ITEM_PERMANENT_ALLIANCE,
	TRADE_ITEM_SURRENDER,
	TRADE_ITEM_TRUCE,
	TRADE_ITEM_PEACE_TREATY,
	TRADE_ITEM_THIRD_PARTY_PEACE,
	TRADE_ITEM_THIRD_PARTY_WAR,
	TRADE_ITEM_THIRD_PARTY_EMBARGO,
	NUM_TRADEABLE_ITEMS,
};
FDataStream & operator>>(FDataStream &, TradeableItems &);
FDataStream & operator<<(FDataStream &, const TradeableItems &);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT: CvTradedItem
//!  \brief Information about one item being exchanged between two players
//
//!  Key Attributes:
//!  - Multiple CvTradedItems can be packaged together into a single deal
//!  - Typically contained inside a CvDeal (where there is a vector of CvTradedItems)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct CvTradedItem
{
	CvTradedItem();
	bool operator==(const CvTradedItem& rhs) const;

	TradeableItems m_eItemType;		// What type of item is this
	int m_iDuration;
	int m_iFinalTurn;
	int m_iData1;					// Any additional data?
	int m_iData2;
	PlayerTypes m_eFromPlayer;      // Which player is giving up this item?
};
FDataStream & operator>>(FDataStream &, CvTradedItem &);
FDataStream & operator<<(FDataStream &, const CvTradedItem &);
typedef FFastList< CvTradedItem, c_eMPoolTypeGame, 0 > TradedItemList;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS: CvDeal
//!  \brief All the information about a single deal
//
//!  Key Attributes:
//!  - Core data in this class is a vector of CvTradedItems
//!  - Also stores the players involved and the turn the deal ends (if any)
//!  - Populated through calls to a group of methods that each create and add a CvTradedItem
//!  - These methods to create CvTradedItems have customized parameters so the external caller
//!    doesn’t need to know how the data is stored internally
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDeal
{
public:
	CvDeal();
	CvDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	CvDeal(const CvDeal& source);
	virtual ~CvDeal();
	CvDeal& operator=(const CvDeal& source);

	// Public data
	PlayerTypes m_eFromPlayer;
	PlayerTypes m_eToPlayer;
	int m_iStartTurn;
	int m_iFinalTurn;
	int m_iDuration;

	PeaceTreatyTypes m_ePeaceTreatyType;
	PlayerTypes m_eSurrenderingPlayer;
	PlayerTypes m_eDemandingPlayer;
	PlayerTypes m_eRequestingPlayer;

	TradedItemList m_TradedItems;

	void ClearItems();
	int GetNumItems();

	void SetFromPlayer(PlayerTypes ePlayer);
	void SetToPlayer(PlayerTypes ePlayer);

	PlayerTypes GetOtherPlayer(PlayerTypes eFromPlayer) const;
    PlayerTypes GetToPlayer()   const { return m_eToPlayer;   };
	PlayerTypes GetFromPlayer() const { return m_eFromPlayer; };
    uint GetStartTurn() const { return m_iStartTurn; };
	uint GetDuration()  const { return m_iDuration; };
    uint GetEndTurn()   const { return m_iFinalTurn; };

	// Peace Treaty stuff
	PeaceTreatyTypes GetPeaceTreatyType() const;
	void SetPeaceTreatyType(PeaceTreatyTypes eTreaty);

	PlayerTypes GetSurrenderingPlayer() const;
	void SetSurrenderingPlayer(PlayerTypes ePlayer);

	// Is this deal a demand from someone?
	PlayerTypes GetDemandingPlayer() const;
	void SetDemandingPlayer(PlayerTypes ePlayer);

	// Is this deal a request from someone?
	PlayerTypes GetRequestingPlayer() const;
	void SetRequestingPlayer(PlayerTypes ePlayer);

	// Misc important functions

	int GetGoldAvailable(PlayerTypes ePlayer, TradeableItems eItemToBeChanged);

	bool IsPossibleToTradeItem(PlayerTypes ePlayer, PlayerTypes eToPlayer, TradeableItems eItem, int iData1 = -1, int iData2 = -1, bool bCheckOtherPlayerValidity = true, bool bFinalizing = false);

	// Methods to add a CvTradedItem to a deal
	void AddGoldTrade(PlayerTypes eFrom, int iAmount);
	void AddGoldPerTurnTrade(PlayerTypes eFrom, int iAmount, int iDuration);
	void AddMapTrade(PlayerTypes eFrom);
	void AddResourceTrade(PlayerTypes eFrom, ResourceTypes eResource, int iAmount, int iDuration);
	void AddCityTrade(PlayerTypes eFrom, int iCityID);
	void AddUnitTrade(PlayerTypes eFrom, int iUnitID);
	void AddOpenBorders(PlayerTypes eFrom, int iDuration);
	void AddDefensivePact(PlayerTypes eFrom, int iDuration);
	void AddResearchAgreement(PlayerTypes eFrom, int iDuration);
	void AddTradeAgreement(PlayerTypes eFrom, int iDuration);
	void AddPermamentAlliance();
	void AddSurrender(PlayerTypes eFrom);
	void AddTruce();
	void AddPeaceTreaty(PlayerTypes eFrom, int iDuration);
	void AddThirdPartyPeace(PlayerTypes eFrom, PlayerTypes eThirdParty, int iDuration);
	void AddThirdPartyWar(PlayerTypes eFrom, PlayerTypes eThirdParty);
	void AddThirdPartyEmbargo(PlayerTypes eFrom, PlayerTypes eThirdParty, int iDuration);

    int GetGoldTrade( PlayerTypes eFrom );
	bool ChangeGoldTrade( PlayerTypes eFrom, int iNewAmount );

    int GetGoldPerTurnTrade( PlayerTypes eFrom );
	bool ChangeGoldPerTurnTrade( PlayerTypes eFrom, int iNewAmount, int iDuration );

	bool IsResourceTrade( PlayerTypes eFrom, ResourceTypes eResource );
    bool ChangeResourceTrade( PlayerTypes eFrom, ResourceTypes eResource, int iAmount, int iDuration );
	bool IsCityTrade( PlayerTypes eFrom, int x, int y );
    void ChangeThirdPartyWarDuration( PlayerTypes eFrom, PlayerTypes eThirdParty, int iNewDuration );
    void ChangeThirdPartyPeaceDuration( PlayerTypes eFrom, PlayerTypes eThirdParty, int iNewDuration );
    void ChangeThirdPartyEmbargoDuration( PlayerTypes eFrom, PlayerTypes eThirdParty, int iNewDuration );

	bool IsOpenBordersTrade( PlayerTypes eFrom );
	bool IsDefensivePactTrade( PlayerTypes eFrom );
	bool IsResearchAgreementTrade( PlayerTypes eFrom );
	bool IsTradeAgreementTrade( PlayerTypes eFrom );
	bool IsPeaceTreatyTrade( PlayerTypes eFrom );
	bool IsThirdPartyPeaceTrade( PlayerTypes eFrom, PlayerTypes eThirdParty );
	bool IsThirdPartyWarTrade( PlayerTypes eFrom, PlayerTypes eThirdParty );

	void RemoveByType(TradeableItems eType, PlayerTypes eFrom = NO_PLAYER);
	void RemoveResourceTrade(ResourceTypes eResource);
	void RemoveCityTrade(PlayerTypes eFrom, int iCityID);
	void RemoveUnitTrade(int iUnitID);
	void RemoveThirdPartyPeace( PlayerTypes eFrom, PlayerTypes eThirdParty );
	void RemoveThirdPartyWar( PlayerTypes eFrom, PlayerTypes eThirdParty );
	void RemoveThirdPartyEmbargo( PlayerTypes eFrom, PlayerTypes eThirdParty );
};
FDataStream & operator>>(FDataStream &, CvDeal &);
FDataStream & operator<<(FDataStream &, const CvDeal &);

typedef FStaticVector<CvDeal, 20, false, c_eCiv5GameplayDLL > DealList;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS: CvGameDeals
//!  \brief All the information about deals made between players
//
//!  Key Attributes:
//!  - Core data in this class is a list of CvDeals
//!  - This object is created inside the CvGame object and accessed through CvGame
//!  - Provides convenience functions to the other game subsystems to quickly summarize
//!    information on the deals in place
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameDeals
{
public:
	CvGameDeals();
	virtual ~CvGameDeals();
	void Init();

	void AddProposedDeal(CvDeal kDeal);
	bool FinalizeDeal( PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bAccepted );
	void DoTurn();

	void DoUpdateCurrentDealsList();

	CvDeal* GetTempDeal();
	void SetTempDeal(CvDeal* pDeal);

	PlayerTypes HasMadeProposal( PlayerTypes eFromPlayer );
	bool ProposedDealExists( PlayerTypes eFromPlayer, PlayerTypes eToPlayer );
	CvDeal* GetProposedDeal( PlayerTypes eFromPlayer, PlayerTypes eToPlayer );

	CvDeal* GetCurrentDeal( PlayerTypes ePlayer, uint index );
	CvDeal* GetHistoricDeal( PlayerTypes ePlayer, uint indx );
	uint GetNumCurrentDeals( PlayerTypes ePlayer );
	uint GetNumHistoricDeals( PlayerTypes ePlayer );

	uint CreateDeal();
	CvDeal* GetDeal(uint index);
	void DestroyDeal(uint index);

	void DoCancelDealsBetweenTeams(TeamTypes eTeam1, TeamTypes eTeam2);
	void DoCancelDealsBetweenPlayers(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	void DoCancelAllDealsWithPlayer(PlayerTypes eCancelPlayer);
	void DoEndTradedItem(CvTradedItem *pItem, PlayerTypes eToPlayer, bool bCancelled);

	int GetTradeItemGoldCost(TradeableItems eItem, PlayerTypes ePlayer1, PlayerTypes ePlayer2) const;

	// Variables below should really be lists to support easy deletion
	DealList m_ProposedDeals;
	DealList m_CurrentDeals;
	DealList m_HistoricalDeals;

protected:
	void LogDealComplete(CvDeal* pDeal);

	CvDeal m_TempDeal;

	//Programmer Note:
	//Rather than use the common idiom of using the index of the deal in an array as the public
	//identifier to the deal, I chose to use a system where the index will always be unique even
	//after deals are destroyed.  This technique is prone to fewer bugs w/ heavily threaded apps.
	std::vector<std::pair<uint, CvDeal*> > m_Deals;
	unsigned int m_uiDealCounter;
};

FDataStream & operator>>(FDataStream &, CvGameDeals &);
FDataStream & operator<<(FDataStream &, const CvGameDeals &);

#endif // CV_DEAL_CLASSES_H
