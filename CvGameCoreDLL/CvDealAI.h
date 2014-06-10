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
	void Init(CvPlayer *pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	CvPlayer* GetPlayer();

	TeamTypes GetTeam();

	int GetDealPercentLeewayWithAI() const;
	int GetDealPercentLeewayWithHuman() const;

	// Offer deal to this AI player and see what his response is

	DealOfferResponseTypes DoHumanOfferDealToThisAI(CvDeal* pDeal);
	void DoAcceptedDeal(PlayerTypes eFromPlayer, const CvDeal & kDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering);

	DemandResponseTypes DoHumanDemand(CvDeal* pDeal);
	void DoAcceptedDemand(PlayerTypes eFromPlayer, const CvDeal & kDeal);

	bool IsDealWithHumanAcceptable(CvDeal* pDeal, PlayerTypes eOtherPlayer, int &iTotalValueToMe, int &iValueImOffering, int &iValueTheyreOffering, int &iAmountOverWeWillRequest, int &iAmountUnderWeWillOffer);

	// The following functions are used to valuate items and construct a deal this AI thinks is fair

	bool DoEqualizeDealWithHuman(CvDeal* pDeal, PlayerTypes eOtherPlayer, bool bDontChangeMyExistingItems, bool bDontChangeTheirExistingItems, bool &bDealGoodToBeginWith, bool &bCantMatchDeal);
	bool DoEqualizeDealWithAI(CvDeal* pDeal, PlayerTypes eOtherPlayer);

	// What is something worth? - bUseEvenValue will see what the mean is between two AI players (us and eOtherPlayer) - will NOT work with a human involved

	int GetDealValue(CvDeal* pDeal, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue);
	int GetTradeItemValue(TradeableItems eItem, bool bFromMe, PlayerTypes eOtherPlayer, int iData1, int iData2, int iDuration, bool bUseEvenValue);

	// Value of individual trade items - bUseEvenValue will see what the mean is between two AI players (us and eOtherPlayer) - will NOT work with a human involved

	int GetGoldForForValueExchange(int iGoldOrValue, bool bNumGoldFromValue, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, bool bRoundUp);
	int GetGPTforForValueExchange(int iGPTorValue, bool bNumGPTFromValue, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, bool bRoundUp);
	int GetResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer);
	int GetCityValue(int iX, int iY, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue);
	int GetOpenBordersValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue);
	int GetDefensivePactValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue);
	int GetResearchAgreementValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue);
	int GetTradeAgreementValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue);
	int GetPeaceTreatyValue(PlayerTypes eOtherPlayer);
	int GetThirdPartyPeaceValue(bool bFromMe, PlayerTypes eOtherPlayer, PlayerTypes eWithPlayer);
	int GetThirdPartyWarValue(bool bFromMe, PlayerTypes eOtherPlayer, PlayerTypes eWithPlayer);

	// Potential items an AI can try to add to a deal to even it out - bUseEvenValue will see what the mean is between two AI players (us and eOtherPlayer) - will NOT work with a human involved

	void DoAddResourceToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, int iAmountOverWeWillRequest, int iDealDuration, bool bUseEvenValue);
	void DoAddResourceToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, int iAmountUnderWeWillOffer, int iDealDuration, bool bUseEvenValue);

	void DoAddOpenBordersToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, int iAmountOverWeWillRequest, int iDealDuration, bool bUseEvenValue);
	void DoAddOpenBordersToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, int iAmountUnderWeWillOffer, int iDealDuration, bool bUseEvenValue);

	void DoAddCitiesToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue);

	void DoAddGoldToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, bool bUseEvenValue);
	void DoAddGoldToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, bool bUseEvenValue);

	void DoAddGPTToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, int iDealDuration, bool bUseEvenValue);
	void DoAddGPTToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, int iDealDuration, bool bUseEvenValue);

	void DoRemoveGPTFromThem(CvDeal* pDeal, PlayerTypes eThem, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, int iDealDuration, bool bUseEvenValue);
	void DoRemoveGPTFromUs(CvDeal* pDeal, PlayerTypes eThem, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, int iDealDuration, bool bUseEvenValue);

	void DoRemoveGoldFromThem(CvDeal* pDeal, PlayerTypes eThem, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, bool bUseEvenValue);
	void DoRemoveGoldFromUs(CvDeal* pDeal, PlayerTypes eThem, int &iTotalValue, int &iValueImOffering, int &iValueTheyreOffering, bool bUseEvenValue);

	// Possible deals the AI can offer

	bool IsOfferPeace(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	void DoAddItemsToDealForPeaceTreaty(PlayerTypes eOtherPlayer, CvDeal* pDeal, PeaceTreatyTypes eTreaty, bool bMeSurrendering);
	int GetCachedValueOfPeaceWithHuman();
	void SetCachedValueOfPeaceWithHuman(int iValue);

	void DoAddPlayersAlliesToTreaty(PlayerTypes eToPlayer, CvDeal* pDeal);

	bool IsMakeDemand(PlayerTypes eOtherPlayer, CvDeal* pDeal);

	bool IsMakeOfferForLuxuryResource(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForOpenBorders(PlayerTypes eOtherPlayer, CvDeal* pDeal);
	bool IsMakeOfferForResearchAgreement(PlayerTypes eOtherPlayer, CvDeal* pDeal);

	// AI demands/human reparations

	void DoSeedReparationsDealWithHuman();

	int GetCachedValueOfConcessionsWithHuman();
	void SetCachedValueOfConcessionsWithHuman(int iValue);

	// Called when the human opens or closes the Trade Screen

	void DoTradeScreenOpened();
	void DoTradeScreenClosed(bool bAIWasMakingOffer);

private:
	CvPlayer *m_pPlayer;

	int m_iCachedValueOfConcessionsWithHuman;		// NOT SERIALIZED
	int m_iCachedValueOfPeaceWithHuman;		// NOT SERIALIZED

};

#endif //CIV5_DEALAI_H