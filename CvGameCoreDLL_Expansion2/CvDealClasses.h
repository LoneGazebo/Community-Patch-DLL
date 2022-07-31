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
#include "CvWeightedVector.h"

enum CLOSED_ENUM TradeableItems
{
    TRADE_ITEM_NONE = -1,

    TRADE_ITEM_GOLD,
    TRADE_ITEM_GOLD_PER_TURN,
    TRADE_ITEM_MAPS,
    TRADE_ITEM_RESOURCES,
    TRADE_ITEM_CITIES,
    TRADE_ITEM_OPEN_BORDERS,
    TRADE_ITEM_DEFENSIVE_PACT,
    TRADE_ITEM_RESEARCH_AGREEMENT,
    TRADE_ITEM_PEACE_TREATY,
    TRADE_ITEM_THIRD_PARTY_PEACE,
    TRADE_ITEM_THIRD_PARTY_WAR,
    TRADE_ITEM_ALLOW_EMBASSY,
	TRADE_ITEM_DECLARATION_OF_FRIENDSHIP, // Only "traded" between human players
	TRADE_ITEM_VOTE_COMMITMENT,
	TRADE_ITEM_TECHS,
	TRADE_ITEM_VASSALAGE,
	TRADE_ITEM_VASSALAGE_REVOKE,

    NUM_TRADEABLE_ITEMS MAYBE_UNUSED_ENUM,
};
FDataStream& operator>>(FDataStream&, TradeableItems&);
FDataStream& operator<<(FDataStream&, const TradeableItems&);

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

	template<typename TradedItem, typename Visitor>
	static void Serialize(TradedItem& tradedItem, Visitor& visitor);
	bool IsTwoSided() const;

	TradeableItems m_eItemType;		// What type of item is this
	int m_iDuration;
	int m_iFinalTurn;
	int m_iData1;					// Any additional data?
	int m_iData2;
	int m_iData3;
	bool m_bFlag1;
	PlayerTypes m_eFromPlayer;      // Which player is giving up this item?
	int m_iValue;					// not serialized, only temporary
	bool m_bValueIsEven;			// not serialized, only temporary
};
FDataStream& operator>>(FDataStream&, CvTradedItem&);
FDataStream& operator<<(FDataStream&, const CvTradedItem&);
typedef vector<CvTradedItem> TradedItemList;

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
	enum CLOSED_ENUM DealRenewStatus
	{
	    DEAL_RENEWABLE,
	    DEAL_NONRENEWABLE,
	    DEAL_SUPPLEMENTAL
	};

	CvDeal(PlayerTypes eFromPlayer=NO_PLAYER, PlayerTypes eToPlayer=NO_PLAYER);
	CvDeal(const CvDeal& source);
	virtual ~CvDeal();
	CvDeal& operator=(const CvDeal& source);
	bool operator==(const CvDeal& other) const;

	template<typename Deal, typename Visitor>
	static void Serialize(Deal& deal, Visitor& visitor);

	// Public data
	PlayerTypes m_eFromPlayer;
	PlayerTypes m_eToPlayer;
	int m_iStartTurn;
	int m_iFinalTurn;
	int m_iDuration;

	int m_iFromPlayerValue;
	int m_iToPlayerValue;

	PeaceTreatyTypes m_ePeaceTreatyType;
	PlayerTypes m_eSurrenderingPlayer;
	PlayerTypes m_eDemandingPlayer;
	PlayerTypes m_eRequestingPlayer;

	bool m_bConsideringForRenewal; // is currently considering renewing this deal
	bool m_bCheckedForRenewal; // this deal has been discussed with the player for renewal
	bool m_bIsGift;
	bool m_bDoNotModifyFrom;
	bool m_bDoNotModifyTo;

	TradedItemList m_TradedItems;

	void ClearItems();
	int GetNumItems();

	void SetFromPlayer(PlayerTypes ePlayer);
	void SetToPlayer(PlayerTypes ePlayer);

	int GetMaxValue() const;
	void SetFromPlayerValue(int iValue);
	void ChangeFromPlayerValue(int iValue);

	void SetToPlayerValue(int iValue);
	void ChangeToPlayerValue(int iValue);

	void SetDuration(int iValue);

	void SetDoNotModifyFrom(bool bValue);
	void SetDoNotModifyTo(bool bValue);

	PlayerTypes GetOtherPlayer(PlayerTypes eFromPlayer) const;
	PlayerTypes GetToPlayer()   const
	{
		return m_eToPlayer;
	};
	PlayerTypes GetFromPlayer() const
	{
		return m_eFromPlayer;
	};
	uint GetStartTurn() const
	{
		return m_iStartTurn;
	};
	uint GetDuration()  const
	{
		return m_iDuration;
	};
	uint GetEndTurn()   const
	{
		return m_iFinalTurn;
	};
	uint GetFromPlayerValue() const
	{
		return m_iFromPlayerValue;
	};
	uint GetToPlayerValue() const
	{
		return m_iToPlayerValue;
	};

	bool DoNotModifyFrom() const
	{
		return m_bDoNotModifyFrom;
	};

	bool DoNotModifyTo() const
	{
		return m_bDoNotModifyTo;
	};

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

#if defined(MOD_ACTIVE_DIPLOMACY)
	bool AreAllTradeItemsValid();
#endif

	bool IsPossibleToTradeItem(PlayerTypes ePlayer, PlayerTypes eToPlayer, TradeableItems eItem, int iData1 = -1, int iData2 = -1, int iData3 = -1, bool bFlag1 = false, bool bFinalizing = false);
	bool BlockTemporaryForPermanentTrade(TradeableItems eItemType, PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	CvString GetReasonsItemUntradeable(PlayerTypes ePlayer, PlayerTypes eToPlayer, TradeableItems eItem, int iData1, int iData2, int iData3, bool bFlag1);

	bool ContainsItemType(TradeableItems eItemType, PlayerTypes eFrom = NO_PLAYER, ResourceTypes eResource = NO_RESOURCE);
	bool ContainsItemTypes(vector<TradeableItems> vItemTypes, PlayerTypes eFrom = NO_PLAYER);

	int GetNumResourceInDeal(PlayerTypes ePlayer, ResourceTypes eResource);
	int GetNumCitiesInDeal(PlayerTypes ePlayer);

	// Methods to add a CvTradedItem to a deal
	void AddGoldTrade(PlayerTypes eFrom, int iAmount);
	void AddGoldPerTurnTrade(PlayerTypes eFrom, int iAmount, int iDuration);
	void AddMapTrade(PlayerTypes eFrom);
	void AddResourceTrade(PlayerTypes eFrom, ResourceTypes eResource, int iAmount, int iDuration);
	void AddCityTrade(PlayerTypes eFrom, int iCityID);
	void AddAllowEmbassy(PlayerTypes eFrom);
	void AddOpenBorders(PlayerTypes eFrom, int iDuration);
	void AddDefensivePact(PlayerTypes eFrom, int iDuration);
	void AddResearchAgreement(PlayerTypes eFrom, int iDuration);
	void AddPeaceTreaty(PlayerTypes eFrom, int iDuration);
	void AddThirdPartyPeace(PlayerTypes eFrom, TeamTypes eThirdPartyTeam, int iDuration);
	void AddThirdPartyWar(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	void AddDeclarationOfFriendship(PlayerTypes eFrom);
	void AddVoteCommitment(PlayerTypes eFrom, int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal);

	void AddTechTrade(PlayerTypes eFrom, TechTypes eTech);
	void AddVassalageTrade(PlayerTypes eFrom);
	void AddRevokeVassalageTrade(PlayerTypes eFrom);

	void RemoveTechTrade(TechTypes eTech);

	bool IsMapTrade(PlayerTypes eFrom);
	bool IsTechTrade(PlayerTypes eFrom, TechTypes eTech);
	bool IsVassalageTrade(PlayerTypes eFrom);
	bool IsRevokeVassalageTrade(PlayerTypes eFrom);

	int GetGoldTrade(PlayerTypes eFrom);
	bool ChangeGoldTrade(PlayerTypes eFrom, int iNewAmount);

	int GetGoldPerTurnTrade(PlayerTypes eFrom);
	bool ChangeGoldPerTurnTrade(PlayerTypes eFrom, int iNewAmount, int iDuration);

	bool IsStrategicsTrade();
	int GetNumStrategicsOnTheirSide(PlayerTypes eFrom);

	bool IsResourceTrade(PlayerTypes eFrom, ResourceTypes eResource);
	int GetNumResourcesInDeal(PlayerTypes eFrom, ResourceTypes eResource);
	bool ChangeResourceTrade(PlayerTypes eFrom, ResourceTypes eResource, int iAmount, int iDuration);
	bool IsCityTrade(PlayerTypes eFrom, int x, int y);
	void ChangeThirdPartyWarDuration(PlayerTypes eFrom, TeamTypes eThirdPartyTeam, int iNewDuration);
	void ChangeThirdPartyPeaceDuration(PlayerTypes eFrom, TeamTypes eThirdPartyTeam, int iNewDuration);

	bool IsAllowEmbassyTrade(PlayerTypes eFrom);
	bool IsOpenBordersTrade(PlayerTypes eFrom);
	bool IsDefensivePactTrade(PlayerTypes eFrom);
	bool IsResearchAgreementTrade(PlayerTypes eFrom);
	bool IsPeaceTreatyTrade(PlayerTypes eFrom);
	bool IsThirdPartyPeaceTrade(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	bool IsThirdPartyWarTrade(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	bool IsVoteCommitmentTrade(PlayerTypes eFrom);
	static DealRenewStatus GetItemTradeableState(TradeableItems eItem);
	bool IsPotentiallyRenewable();

	//return true if anything was removed
	bool RemoveAllByPlayer(PlayerTypes eFrom);
	void RemoveByType(TradeableItems eType, PlayerTypes eFrom = NO_PLAYER);
	void RemoveResourceTrade(ResourceTypes eResource);
	void RemoveCityTrade(PlayerTypes eFrom, int iCityID);
	void RemoveThirdPartyPeace(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	void RemoveThirdPartyWar(PlayerTypes eFrom, TeamTypes eThirdPartyTeam);
	void RemoveVoteCommitment(PlayerTypes eFrom, int iResolutionID, int iVoteChoice, int iNumVotes, bool bRepeal);
};

FDataStream& operator>>(FDataStream&, CvDeal&);
FDataStream& operator<<(FDataStream&, const CvDeal&);

typedef vector<CvDeal> DealList;
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

	template<typename GameDeals, typename Visitor>
	static void Serialize(GameDeals& gameDeals, Visitor& visitor);

	void AddProposedDeal(CvDeal kDeal);
#if defined(MOD_ACTIVE_DIPLOMACY)
	bool RemoveProposedDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal* pDealOut, bool latest);
	bool FinalizeMPDeal(CvDeal kDeal, bool bAccepted);
	bool FinalizeMPDealLatest(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bAccepted, bool latest);
	void FinalizeDealValidAndAccepted(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal& kDeal, bool bAccepted, CvWeightedVector<TeamTypes>& veNowAtPeacePairs);
	void FinalizeDealNotify(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvWeightedVector<TeamTypes>& veNowAtPeacePairs);
	CvDeal* GetProposedMPDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool latest = false);
	void DoCancelAllProposedMPDealsWithPlayer(PlayerTypes eCancelPlayer, DiplomacyMode eTargetPlayers);
#endif
	bool FinalizeDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bAccepted);
	void ActivateDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, CvDeal& kDeal, CvWeightedVector<TeamTypes>& veNowAtPeacePairs);
	void DoTurn();
	void DoTurnPost();

	void DoUpdateCurrentDealsList();

	CvDeal* GetTempDeal();
	void SetTempDeal(CvDeal* pDeal);

	PlayerTypes HasMadeProposal(PlayerTypes eFromPlayer);
	bool ProposedDealExists(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	CvDeal* GetProposedDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	CvDeal* GetCurrentDeal(PlayerTypes ePlayer, uint index);
	CvDeal* GetHistoricDeal(PlayerTypes ePlayer, uint indx);
	uint GetNumCurrentDeals(PlayerTypes ePlayer);
	uint GetNumHistoricDeals(PlayerTypes ePlayer, uint iMaxCount=UINT_MAX);

	CvDeal* GetCurrentDealWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, uint index);
	CvDeal* GetHistoricDealWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, uint indx);
	uint GetNumCurrentDealsWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);
	uint GetNumHistoricDealsWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, uint iMaxCount = UINT_MAX);
	std::vector<CvDeal*> GetRenewableDealsWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, uint iMaxCount = UINT_MAX);
	bool IsReceivingItemsFromPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, bool bMutual);
	int GetDealValueWithPlayer(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, bool bConsiderDuration = true);
	int GetDealGPTLostFromWar(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);

	uint CreateDeal();
	CvDeal* GetDeal(uint index);
	void DestroyDeal(uint index);

	void DoCancelDealsBetweenTeams(TeamTypes eTeam1, TeamTypes eTeam2);
	void DoCancelDealsBetweenPlayers(PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	void DoCancelAllDealsWithPlayer(PlayerTypes eCancelPlayer);
	void DoCancelAllProposedDealsWithPlayer(PlayerTypes eCancelPlayer);
	void DoEndTradedItem(CvTradedItem* pItem, PlayerTypes eToPlayer, bool bCancelled, bool bSkip = false);

	int GetTradeItemGoldCost(TradeableItems eItem, PlayerTypes ePlayer1, PlayerTypes ePlayer2) const;

	static void PrepareRenewDeal(CvDeal* pOldDeal);

	// Variables below should really be lists to support easy deletion
	DealList m_ProposedDeals;
	DealList m_CurrentDeals;
	DealList m_HistoricalDeals;

protected:
	void LogDealComplete(CvDeal* pDeal);
#if defined(MOD_BALANCE_CORE)
	void LogDealFailed(CvDeal* pDeal, bool bNoRenew, bool bNotAccepted, bool bNotValid);
#endif

	CvDeal m_TempDeal;

	//Programmer Note:
	//Rather than use the common idiom of using the index of the deal in an array as the public
	//identifier to the deal, I chose to use a system where the index will always be unique even
	//after deals are destroyed.  This technique is prone to fewer bugs w/ heavily threaded apps.
	std::vector<std::pair<uint, CvDeal*> > m_Deals;
	unsigned int m_uiDealCounter;
};

FDataStream& operator>>(FDataStream&, CvGameDeals&);
FDataStream& operator<<(FDataStream&, const CvGameDeals&);

#endif // CV_DEAL_CLASSES_H
