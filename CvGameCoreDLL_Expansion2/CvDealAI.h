/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_DEALAI_H
#define CIV5_DEALAI_H

#include "CvDealClasses.h"
#include "CvDiplomacyAIEnums.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvDealAI
//!  \brief		Determines the value of a diplomatic Deal for each side (from the perspective of this AI player).  Also handles some logic for sending deals, although much of this is in CvDiplomacyAI
//
//!  Author:	Jon Shafer
//
//!  Key Attributes:
//!  - Object created by CvPlayer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDealAI
{
public:
	CvDealAI(void);
	~CvDealAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	template<typename DealAI, typename Visitor>
	static void Serialize(DealAI& dealAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	CvPlayer* GetPlayer();

	TeamTypes GetTeam();

	int GetDealPercentLeeway(PlayerTypes eOtherPlayer, bool bInTheBlack) const;

	bool WithinAcceptableRange(PlayerTypes ePlayer, int iMaxValue, int iNetValue);
	bool BothSidesIncluded(CvDeal* pDeal);
	bool TooMuchAdded(PlayerTypes ePlayer, int iMaxValue, int iNetValue, int iItemValue, bool bFromUs);

	// Offer deal to this AI player and see what his response is

	DealOfferResponseTypes DoHumanOfferDealToThisAI(CvDeal* pDeal);
	void DoAcceptedDeal(PlayerTypes eFromPlayer, const CvDeal& kDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering);

	DemandResponseTypes DoHumanDemand(CvDeal* pDeal);
	void DoAcceptedDemand(PlayerTypes eFromPlayer, const CvDeal& kDeal);

	bool IsDealWithHumanAcceptable(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValueToMe, bool* bCantMatchOffer, bool bFirstPass = false);

	// The following functions are used to valuate items and construct a deal this AI thinks is fair

	bool DoEqualizeDealWithHuman(CvDeal* pDeal, PlayerTypes eOtherPlayer, bool& bDealGoodToBeginWith, bool& bCantMatchOffer);
	bool DoEqualizeDealWithAI(CvDeal* pDeal, PlayerTypes eOtherPlayer);

	// What is something worth? - bUseEvenValue will see what the mean is between two AI players (us and eOtherPlayer) - will NOT work with a human involved

	int GetDealValue(CvDeal* pDeal, bool bLogging = false);
	int GetTradeItemValue(TradeableItems eItem, bool bFromMe, PlayerTypes eOtherPlayer, int iData1, int iData2, int iData3, bool bFlag1, int iDuration, bool bLogging = false);

	int GetResourceRatio(PlayerTypes eSeller, PlayerTypes eBuyer, ResourceTypes eResource, int iNumInTrade);
	// Value of individual trade items - bUseEvenValue will see what the mean is between two AI players (us and eOtherPlayer) - will NOT work with a human involved

	int GetGoldForForValueExchange(int iGoldOrValue, bool bNumGoldFromValue, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue = false);
	int GetGPTforForValueExchange(int iGPTorValue, bool bNumGPTFromValue, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue = false);
	int GetResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer);
	int GetLuxuryResourceValue(ResourceTypes eResource, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer);
	int GetStrategicResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer);
	int GetEmbassyValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue = false);
	int GetOpenBordersValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue = false);
	int GetDefensivePactValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue = false);
	int GetResearchAgreementValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue = false);
	int GetPeaceTreatyValue(PlayerTypes eOtherPlayer);
	int GetThirdPartyPeaceValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam, bool bLogging = false);
	int GetThirdPartyWarValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam, bool bLogging = false);
	int GetVoteCommitmentValue(bool bFromMe, PlayerTypes eOtherPlayer, int iProposalID, int iVoteChoice, int iNumVotes, bool bRepeal, bool bUseEvenValue = false);
	int GetCityValueForDeal(CvCity* pCity, PlayerTypes eAssumedOwner, bool bPeaceTreatyTrade = false);

	// Potential items an AI can try to add to a deal to even it out - bUseEvenValue will see what the mean is between two AI players (us and eOtherPlayer) - will NOT work with a human involved

	void DoAddVoteCommitmentToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddVoteCommitmentToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddThirdPartyWarToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddThirdPartyWarToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddThirdPartyPeaceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddThirdPartyPeaceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddLuxuryResourceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddLuxuryResourceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddStrategicResourceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddStrategicResourceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddEmbassyToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddEmbassyToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddOpenBordersToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddOpenBordersToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddCitiesToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddCitiesToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddGoldToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddGoldToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddGPTToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddGPTToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoRemoveGPTFromThem(CvDeal* pDeal, PlayerTypes eThem, int iNumToRemove);
	void DoRemoveGPTFromUs(CvDeal* pDeal, int iNumToRemove);

	void DoRemoveGoldFromThem(CvDeal* pDeal, PlayerTypes eThem, int& iNumGoldAlreadyInTrade);
	void DoRemoveGoldFromUs(CvDeal* pDeal, int& iNumGoldAlreadyInTrade);

	void DoAddItemsToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddItemsToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	// Possible deals the AI can offer
	bool IsOfferPeace(PlayerTypes eOtherPlayer, CvDeal* pDeal, bool bEqualizingDeals);
	void DoAddItemsToDealForPeaceTreaty(PlayerTypes eOtherPlayer, CvDeal* pDeal, PeaceTreatyTypes eTreaty, bool bMeSurrendering);
	int GetCachedValueOfPeaceWithHuman();
	void SetCachedValueOfPeaceWithHuman(int iValue);

	void DoAddPlayersAlliesToTreaty(PlayerTypes eToPlayer, CvDeal* pDeal);

	int GetPotentialDemandValue(PlayerTypes eOtherPlayer, CvDeal* pDeal, int iIdealValue);
	bool IsMakeDemand(PlayerTypes eOtherPlayer, CvDeal* pDeal);

	bool IsMakeOfferForLuxuryResource(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForEmbassy(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForOpenBorders(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForResearchAgreement(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForStrategicResource(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForDefensivePact(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForCityExchange(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForThirdPartyWar(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForThirdPartyPeace(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForVote(PlayerTypes eOtherPlayer, CvDeal* pDeal);

	// Called when the human opens or closes the Trade Screen

	void DoTradeScreenOpened();
	void DoTradeScreenClosed(bool bAIWasMakingOffer);

	DemandResponseTypes GetRequestForHelpResponse(CvDeal* pDeal);

	// How much is item worth to AI?
	int GetMapValue(bool bFromMe, PlayerTypes eOtherPlayer);
	int GetTechValue(TechTypes eTech, bool bFromMe, PlayerTypes eOtherPlayer);
	int GetVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer);
	int GetRevokeVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bWar = false);

	// Does AI want to make offer for X?
	bool IsMakeOfferForMaps(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForTech(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForVassalage(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferToBecomeVassal(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForRevokeVassalage(PlayerTypes eOtherPlayer, CvDeal* pDeal);

	// Will adding item to deal even it out?
	void DoAddTechToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddTechToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddMapsToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddMapsToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddVassalageToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);
	void DoAddVassalageToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddRevokeVassalageToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

protected:
	void UpdateResearchRateCache(PlayerTypes eOther);

	CvPlayer* m_pPlayer;

	int m_iCachedValueOfPeaceWithHuman;		// NOT SERIALIZED

	//player to (turn,value)
	std::vector<std::pair<int,int> > m_vResearchRates;

	//some magic to enable memoization of calls to GetTradeItemValue
	struct SDealItemValueParams
	{
		TradeableItems eItem;
		bool bFromMe;
		PlayerTypes eOtherPlayer;
		int iData1;
		int iData2;
		int iData3;
		bool bFlag1;
		int iDuration;
		//bool bUseEvenValue;

		SDealItemValueParams(TradeableItems eItem_, bool bFromMe_, PlayerTypes eOtherPlayer_, int iData1_, int iData2_, int iData3_, bool bFlag1_, int iDuration_) :
			eItem(eItem_), bFromMe(bFromMe_), eOtherPlayer(eOtherPlayer_), iData1(iData1_), iData2(iData2_), iData3(iData3_), bFlag1(bFlag1_), iDuration(iDuration_) { }

		bool operator==(const SDealItemValueParams& rhs) const
		{
			return eItem == rhs.eItem && bFromMe == rhs.bFromMe && eOtherPlayer == rhs.eOtherPlayer && iData1 == rhs.iData1 && iData2 == rhs.iData2 && iData3 == rhs.iData3 && bFlag1 == rhs.bFlag1 && iDuration == rhs.iDuration;
		}
	};

	// specialized hash function for unordered_map keys
	struct SDealItemValueParamsHash
	{
		//obviously not portable ...
		size_t rotl32 (size_t value, size_t count) const {
			return value << count | value >> (32 - count);
		}

		std::size_t operator() (const SDealItemValueParams& key) const
		{
			std::size_t h1 = tr1::hash<int>()(key.eItem);
			std::size_t h2 = tr1::hash<bool>()(key.bFromMe);
			std::size_t h3 = tr1::hash<int>()(key.eOtherPlayer);
			std::size_t h4 = tr1::hash<int>()(key.iData1);
			std::size_t h5 = tr1::hash<int>()(key.iData2);
			std::size_t h6 = tr1::hash<int>()(key.iData3);
			std::size_t h7 = tr1::hash<bool>()(key.bFlag1);
			std::size_t h8 = tr1::hash<int>()(key.iDuration);
 
			//rotate the bits so that XORing isn't quite as likely to collapse
			return rotl32(h1,1) ^ rotl32(h2,2) ^ rotl32(h3,3) ^ rotl32(h4,4) ^ rotl32(h5,5) ^ rotl32(h6,6) ^ rotl32(h7,7) ^ rotl32(h8,8);
		}
	};

	//finally the cache itself
	tr1::unordered_map<SDealItemValueParams, int, SDealItemValueParamsHash> m_dealItemValues;
	int m_iDealItemValuesTurnSlice;
};

FDataStream& operator>>(FDataStream&, CvDealAI&);
FDataStream& operator<<(FDataStream&, const CvDealAI&);

#endif //CIV5_DEALAI_H