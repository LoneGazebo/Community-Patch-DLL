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

	bool WithinAcceptableRange(PlayerTypes ePlayer, int iMaxValue, int iNetValue) const;
	bool BothSidesIncluded(CvDeal* pDeal);
	bool TooMuchAdded(PlayerTypes ePlayer, int iMaxValue, int iNetValue, int iItemValue, bool bFromUs);

	// Offer deal to this AI player and see what his response is

	DealOfferResponseTypes DoHumanOfferDealToThisAI(CvDeal* pDeal);
	void DoAcceptedDeal(PlayerTypes eFromPlayer, const CvDeal& kDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering);

	DemandResponseTypes DoHumanDemand(CvDeal* pDeal);
	DemandResponseTypes GetDemandResponse(CvDeal* pDeal);
	void DoAcceptedDemand(PlayerTypes eFromPlayer, const CvDeal& kDeal);

	bool IsDealWithHumanAcceptable(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValueToMe, bool* bCantMatchOffer, bool bFirstPass = false);

	// The following functions are used to valuate items and construct a deal this AI thinks is fair

	bool DoEqualizeDeal(CvDeal* pDeal, PlayerTypes eOtherPlayer, bool& bDealGoodToBeginWith, bool& bCantMatchOffer, bool bHumanRequestedEqualization = false);

	int GetOneGPTValue(bool bPeaceDeal) const;
	int GetDealValue(CvDeal* pDeal);
	int GetTradeItemValue(TradeableItems eItem, bool bFromMe, PlayerTypes eOtherPlayer, int iData1, int iData2, int iData3, bool bFlag1, int iDuration, bool bIsAIOffer, bool bEqualize = true);

	int GetGoldForForValueExchange(int iGoldOrValue, bool bNumGoldFromValue);
	int GetGPTForForValueExchange(int iGPTorValue, bool bNumGPTFromValue, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer);
	int GetResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer);
	int GetLuxuryResourceValue(ResourceTypes eResource, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer);
	vector<int> GetStrategicResourceItemList(ResourceTypes eResource, int iNumTurns, bool bFromMe, int iNumAvailable, bool bPeaceDeal);
	int GetStrategicResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer);
	int GetEmbassyValue(bool bFromMe, PlayerTypes eOtherPlayer);
	int GetOpenBordersValue(bool bFromMe, PlayerTypes eOtherPlayer);
	int GetDefensivePactValue(bool bFromMe, PlayerTypes eOtherPlayer);
	int GetResearchAgreementValue(bool bFromMe, PlayerTypes eOtherPlayer);
	int GetPeaceTreatyValue(PlayerTypes eOtherPlayer);
	int GetThirdPartyPeaceValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam, bool bLogging = false);
	int GetThirdPartyWarValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam, bool bLogging = false);
	int GetVoteCommitmentValue(bool bFromMe, PlayerTypes eOtherPlayer, int iProposalID, int iVoteChoice, int iNumVotes, bool bRepeal);
	int GetCityValueForDeal(CvCity* pCity, PlayerTypes eAssumedOwner);

	void DoAddVoteCommitmentToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddVoteCommitmentToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddThirdPartyWarToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddThirdPartyWarToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddThirdPartyPeaceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddThirdPartyPeaceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddLuxuryResourceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddLuxuryResourceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddStrategicResourceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddStrategicResourceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddEmbassyToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddEmbassyToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddOpenBordersToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddOpenBordersToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddCitiesToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddCitiesToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddGoldToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iDemandValue = 0);
	void DoAddGoldToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoAddGPTToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iDemandValue = 0);
	void DoAddGPTToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue);

	void DoRemoveGPTFromThem(CvDeal* pDeal, PlayerTypes eThem, int iToRemove);
	void DoRemoveGPTFromUs(CvDeal* pDeal, int iToRemove);

	void DoRemoveGoldFromThem(CvDeal* pDeal, PlayerTypes eThem, int& iGold);
	void DoRemoveGoldFromUs(CvDeal* pDeal, int& iGold);

	void DoAddItemsToThem(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValue, int iThresholdValue, bool bGoldOnly, bool bHumanRequestedEqualization);
	void DoAddItemsToUs(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValue, int iThresholdValue, bool bGoldOnly, bool bHumanRequestedEqualization);

	// Possible deals the AI can offer
	bool IsOfferPeace(PlayerTypes eOtherPlayer, CvDeal* pDeal, bool bEqualizingDeals);
	void DoAddItemsToDealForPeaceTreaty(PlayerTypes eOtherPlayer, CvDeal* pDeal, PeaceTreatyTypes eTreaty, bool bMeSurrendering);
	int GetCachedValueOfPeaceWithHuman() const;
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
	bool IsMakeOfferForMaps(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForTech(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForVassalage(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferToBecomeVassal(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForRevokeVassalage(PlayerTypes eOtherPlayer, CvDeal* pDeal);

	// Called when the human opens or closes the Trade Screen

	void DoTradeScreenOpened();
	void DoTradeScreenClosed(bool bAIWasMakingOffer);

	DemandResponseTypes GetRequestForHelpResponse(CvDeal* pDeal);

	// How much is item worth to AI?
	int GetMapValue(bool bFromMe, PlayerTypes eOtherPlayer);
	int GetTechValue(TechTypes eTech, bool bFromMe, PlayerTypes eOtherPlayer);
	int GetVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer);
	int GetRevokeVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bWar = false);

	// Will adding item to deal even it out?
	void DoAddTechToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddTechToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddMapsToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddMapsToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddVassalageToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddVassalageToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

	void DoAddRevokeVassalageToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);
	void DoAddRevokeVassalageToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue);

protected:
	void UpdateResearchRateCache(PlayerTypes eOther);

	CvPlayer* m_pPlayer;

	int m_iCachedValueOfPeaceWithHuman;		// NOT SERIALIZED

	//player to (turn,value)
	std::vector<std::pair<int,int>> m_vResearchRates;

	//some magic to enable memorization of calls to GetTradeItemValue
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
		bool bEqualize;

		SDealItemValueParams(TradeableItems eItem_, bool bFromMe_, PlayerTypes eOtherPlayer_, int iData1_, int iData2_, int iData3_, bool bFlag1_, int iDuration_, bool bEqualize_) :
			eItem(eItem_), bFromMe(bFromMe_), eOtherPlayer(eOtherPlayer_), iData1(iData1_), iData2(iData2_), iData3(iData3_), bFlag1(bFlag1_), iDuration(iDuration_), bEqualize(bEqualize_) { }

		bool operator==(const SDealItemValueParams& rhs) const
		{
			return eItem == rhs.eItem && bFromMe == rhs.bFromMe && eOtherPlayer == rhs.eOtherPlayer && iData1 == rhs.iData1 && iData2 == rhs.iData2 && iData3 == rhs.iData3 && bFlag1 == rhs.bFlag1 && iDuration == rhs.iDuration && bEqualize == rhs.bEqualize;
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
			std::size_t h9 = tr1::hash<bool>()(key.bEqualize);
 
			//rotate the bits so that XORing isn't quite as likely to collapse
			return rotl32(h1,1) ^ rotl32(h2,2) ^ rotl32(h3,3) ^ rotl32(h4,4) ^ rotl32(h5,5) ^ rotl32(h6,6) ^ rotl32(h7,7) ^ rotl32(h8,8) ^ rotl32(h9,9);
		}
	};

	//finally the cache itself
	tr1::unordered_map<SDealItemValueParams, int, SDealItemValueParamsHash> m_dealItemValues;
	int m_iDealItemValuesTurnSlice;
};

FDataStream& operator>>(FDataStream&, CvDealAI&);
FDataStream& operator<<(FDataStream&, const CvDealAI&);

#endif //CIV5_DEALAI_H