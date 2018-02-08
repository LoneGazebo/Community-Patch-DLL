/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvDealAI.h"
#include "CvDealClasses.h"
#include "CvDiplomacyAI.h"
#include "CvMinorCivAI.h"
#include "CvDllInterfaces.h"
#include "CvGrandStrategyAI.h"
#if defined(MOD_BALANCE_CORE_MILITARY)
#include "CvMilitaryAI.h"
#include "CvEconomicAI.h"
#include "CvVotingClasses.h"
#endif

// must be included after all other headers
#include "LintFree.h"

//======================================================================================================
//					CvDealAI
//======================================================================================================
CvDealAI::CvDealAI()
{
}

//------------------------------------------------------------------------------
CvDealAI::~CvDealAI(void)
{
	Uninit();
}

/// Initialize
void CvDealAI::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	Reset();
}

/// Deallocate memory created in initialize
void CvDealAI::Uninit()
{
}

/// Reset
void CvDealAI::Reset()
{
	m_iCachedValueOfPeaceWithHuman = 0;
	m_vResearchRates = std::vector<std::pair<int,int>>(MAX_PLAYERS, std::make_pair(0,0));
}

/// Serialization read
void CvDealAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);
}

/// Serialization write
void CvDealAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

/// Returns the Player object this DealAI is associated with
CvPlayer* CvDealAI::GetPlayer()
{
	return m_pPlayer;
}

// Helper function which returns this player's TeamType
TeamTypes CvDealAI::GetTeam()
{
	return m_pPlayer->getTeam();
}

/// How much are we willing to back off on what our perceived value of a deal is with an AI player to make something work?
#if defined(MOD_BALANCE_CORE_DEALS)
int CvDealAI::GetDealPercentLeewayWithAI(PlayerTypes eOtherPlayer) const
#else
int CvDealAI::GetDealPercentLeewayWithAI() const
#endif
{
#if defined(MOD_BALANCE_CORE_DEALS)
	switch (m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			return 15;
			
		case MAJOR_CIV_OPINION_FRIEND:
			return 12;
			
		case MAJOR_CIV_OPINION_FAVORABLE:
			return 10;
			
		case MAJOR_CIV_OPINION_NEUTRAL:
			return 10;
			
		case MAJOR_CIV_OPINION_COMPETITOR:
			return 5;
			
		case MAJOR_CIV_OPINION_ENEMY:
			return 3;
			
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			return 2;		
		default:
			return 10;
		}
#else
	return 25;
#endif
}

/// How much are we willing to back off on what our perceived value of a deal is with a human player to make something work?
int CvDealAI::GetDealPercentLeewayWithHuman() const
{
	return 1;
}
/// Offer up a deal to this AI, and see if he accepts
DealOfferResponseTypes CvDealAI::DoHumanOfferDealToThisAI(CvDeal* pDeal)
{
	DealOfferResponseTypes eResponse = NO_DEAL_RESPONSE_TYPE;
	DiploUIStateTypes eUIState = NO_DIPLO_UI_STATE;

	const char* szText = "";
	LeaderheadAnimationTypes eAnimation = NO_LEADERHEAD_ANIM;

	PlayerTypes eFromPlayer = GC.getGame().getActivePlayer();

	bool bFromIsActivePlayer = eFromPlayer == GC.getGame().getActivePlayer();

	int iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer;

	bool bCantMatchOffer;
#if defined(MOD_BALANCE_CORE)
	bool bDealAcceptable = IsDealWithHumanAcceptable(pDeal, eFromPlayer, /*Passed by reference*/ iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, &bCantMatchOffer, false);
#else
	bool bDealAcceptable = IsDealWithHumanAcceptable(pDeal, eFromPlayer, /*Passed by reference*/ iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, bCantMatchOffer);
#endif
	// If they're actually giving us more than we're asking for (e.g. a gift) then accept the deal
	if(!bDealAcceptable)
	{
		if(!pDeal->IsPeaceTreatyTrade(eFromPlayer) && iValueTheyreOffering > iValueImOffering)
		{
			bDealAcceptable = true;
		}
	}

#if defined(MOD_BALANCE_CORE)
	//Final failsafe
	if(!pDeal->IsPeaceTreatyTrade(eFromPlayer))
	{
		if(pDeal->GetRequestingPlayer() == GetPlayer()->GetID())
		{
			bDealAcceptable = true;
			pDeal->SetRequestingPlayer(NO_PLAYER);
		}
		else if (GetPlayer()->GetDiplomacyAI()->IsOfferingGift(eFromPlayer))
		{
			bDealAcceptable = true;
			GetPlayer()->GetDiplomacyAI()->SetOfferingGift(eFromPlayer, false);
		}
		//Getting 'error' values in this deal? It is bad, abort!
		else if((iValueTheyreOffering == INT_MAX) || (iValueImOffering == INT_MAX) || (iDealValueToMe == INT_MAX))
		{
			bDealAcceptable = false;
			iDealValueToMe = -500;
		}
	}
#endif

	if(bDealAcceptable)
	{
		CvDeal kDeal = *pDeal;

		// If it's from a human, send it through the network
		if(GET_PLAYER(eFromPlayer).isHuman())
		{
			GC.GetEngineUserInterface()->SetDealInTransit(true);
			auto_ptr<ICvDeal1> pDllDeal = GC.WrapDealPointer(&kDeal);
			gDLL->sendNetDealAccepted(eFromPlayer, GetPlayer()->GetID(), pDllDeal.get(), iDealValueToMe, iValueImOffering, iValueTheyreOffering);
		}
		// Deal between AI players, we can process it immediately
		else
		{
			DoAcceptedDeal(eFromPlayer, kDeal, iDealValueToMe, iValueImOffering, iValueTheyreOffering);
		}
#if defined(MOD_BALANCE_CORE)
		if(pDeal)
		{
			pDeal->SetRequestingPlayer(NO_PLAYER);
		}
#endif
	}
	// We want more from this Deal
	else if(iDealValueToMe > -75 &&
	        iValueImOffering < (iValueTheyreOffering * 5))	// The total value of the deal might not be that bad, but if he's asking for WAY more than he's offering (e.g. something for nothing) then it's not unacceptable, but insulting
	{
		eResponse = DEAL_RESPONSE_UNACCEPTABLE;
		eUIState = DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER;

		if(bFromIsActivePlayer)
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_REJECT_UNACCEPTABLE);
			eAnimation = LEADERHEAD_ANIM_NO;
		}
	}
	// Pretty bad deal for us
	else
	{
		eResponse = DEAL_RESPONSE_INSULTING;
		eUIState = DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER;

		if(bFromIsActivePlayer)
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_REJECT_INSULTING);
			eAnimation = LEADERHEAD_ANIM_NO;
		}
	}

	if(bFromIsActivePlayer)
	{
		// Modify response if the player's offered a deal lot
		if(eResponse >= DEAL_RESPONSE_UNACCEPTABLE)
		{
			int iTimesDealOffered = GC.GetEngineUserInterface()->GetOfferTradeRepeatCount();
			if(iTimesDealOffered > 4)
			{
				szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_TRADE_TOO_MUCH);
			}
			else if(iTimesDealOffered > 1)
			{
				szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_TRADE);
			}

			GC.GetEngineUserInterface()->ChangeOfferTradeRepeatCount(1);
			gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), eUIState, szText, eAnimation);
		}
	}

	return eResponse;
}

/// Deal has been accepted
void CvDealAI::DoAcceptedDeal(PlayerTypes eFromPlayer, const CvDeal& kDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering)
{
	int iDealType = -1;
	if(m_pPlayer->GetDiplomacyAI()->GetDealToRenew(&iDealType, eFromPlayer))
	{
		if (iDealType != 0) // if it's not a historic deal
		{
			// make the deal not remove resources when processed
			CvGameDeals::PrepareRenewDeal(m_pPlayer->GetDiplomacyAI()->GetDealToRenew(NULL, eFromPlayer), &kDeal);
		}
		m_pPlayer->GetDiplomacyAI()->ClearDealToRenew();
	}
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		GC.getGame().GetGameDeals().FinalizeMPDeal(kDeal, true);
	}
	else
	{
		GC.getGame().GetGameDeals().AddProposedDeal(kDeal);
		GC.getGame().GetGameDeals().FinalizeDeal(eFromPlayer, GetPlayer()->GetID(), true);
	}
#else
	GC.getGame().GetGameDeals().AddProposedDeal(kDeal);
	GC.getGame().GetGameDeals().FinalizeDeal(eFromPlayer, GetPlayer()->GetID(), true);
#endif	
	if(GET_PLAYER(eFromPlayer).isHuman())
	{
		iDealValueToMe -= GetCachedValueOfPeaceWithHuman();

		// Reset cached values
		SetCachedValueOfPeaceWithHuman(0);
#if defined(MOD_BALANCE_CORE)
		m_pPlayer->SetCachedValueOfPeaceWithHuman(0);
#endif
#if defined(MOD_BALANCE_CORE)
		LeaderheadAnimationTypes eAnimation = LEADERHEAD_ANIM_POSITIVE;
#else
		LeaderheadAnimationTypes eAnimation;
#endif
		// This signals Lua to do some interface cleanup, we only want to do this on the local machine.
		if(GC.getGame().getActivePlayer() == eFromPlayer)
			gDLL->DoClearDiplomacyTradeTable();

		DiploUIStateTypes eUIState = NO_DIPLO_UI_STATE;
#if defined(MOD_BALANCE_CORE)
		const char* szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE);
#else
		const char* szText = 0;
#endif

		// We made a demand and they gave in
		if(kDeal.GetDemandingPlayer() == GetPlayer()->GetID())
		{
			if(GC.getGame().getActivePlayer() == eFromPlayer)
			{
				szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_AI_DEMAND);
				gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_POSITIVE);
			}

			return;
		}

		// We made a request and they agreed
		if(kDeal.GetRequestingPlayer() == GetPlayer()->GetID())
		{
			if(GC.getGame().getActivePlayer() == eFromPlayer)
			{
				szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_THANKFUL);
				gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
			}
			GetPlayer()->GetDiplomacyAI()->ChangeRecentAssistValue(eFromPlayer, -iDealValueToMe);
			return;
		}

		eUIState = DIPLO_UI_STATE_BLANK_DISCUSSION;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		// We're offering help to a player
		if (MOD_DIPLOMACY_CIV4_FEATURES && (GetPlayer()->GetDiplomacyAI()->IsOfferingGift(eFromPlayer) || GetPlayer()->GetDiplomacyAI()->IsOfferedGift(eFromPlayer)))
		{
			//End the gift exchange after this.
			GetPlayer()->GetDiplomacyAI()->SetOfferingGift(eFromPlayer, false);
			GetPlayer()->GetDiplomacyAI()->SetOfferedGift(eFromPlayer, true);
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
			eAnimation = LEADERHEAD_ANIM_POSITIVE;
		}
		else
#else
		// Good deal for us
#endif
#if defined(MOD_BALANCE_CORE)
		if (kDeal.GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
		{
#endif
		if(iDealValueToMe >= 100 ||
		        iValueTheyreOffering > (iValueImOffering * 5))	// A deal can be generous if we're getting a lot overall, OR a lot more than we're giving up
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_GENEROUS);
			eAnimation = LEADERHEAD_ANIM_YES;
			if (kDeal.GetDemandingPlayer() != eFromPlayer)
				GetPlayer()->GetDiplomacyAI()->ChangeRecentTradeValue(eFromPlayer, (iDealValueToMe / 5));
		}
		// Acceptable deal for us
		else
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE);
			eAnimation = LEADERHEAD_ANIM_YES;
			if (kDeal.GetDemandingPlayer() != eFromPlayer)
				GetPlayer()->GetDiplomacyAI()->ChangeRecentTradeValue(eFromPlayer, (iDealValueToMe / 10));
		}
#if defined(MOD_BALANCE_CORE)
		}
#endif

		if(GC.getGame().getActivePlayer() == eFromPlayer)
			GC.GetEngineUserInterface()->SetOfferTradeRepeatCount(0);

		// If this was a peace deal then use that animation instead
		if(kDeal.GetPeaceTreatyType() != NO_PEACE_TREATY_TYPE)
		{
			eAnimation = LEADERHEAD_ANIM_PEACEFUL;
#if defined(MOD_BALANCE_CORE)
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE);
#endif
		}

		// Send message back to diplo UI
		if(GC.getGame().getActivePlayer() == eFromPlayer)
			gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), eUIState, szText, eAnimation);
	}
	if(GC.getGame().getActivePlayer() == eFromPlayer || GC.getGame().getActivePlayer() == GetPlayer()->GetID())
	{
		GC.GetEngineUserInterface()->makeInterfaceDirty();
	}
}

/// Human making a demand of the AI
DemandResponseTypes CvDealAI::DoHumanDemand(CvDeal* pDeal)
{
	DemandResponseTypes eResponse = NO_DEMAND_RESPONSE_TYPE;

	PlayerTypes eFromPlayer = GC.getGame().getActivePlayer();
	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iValueWillingToGiveUp = 0;

	CvDiplomacyAI* pDiploAI = GET_PLAYER(eMyPlayer).GetDiplomacyAI();
	
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	MajorCivApproachTypes eApproach = pDiploAI->GetMajorCivApproach(eFromPlayer, /*bHideTrueFeelings*/ true);
	StrengthTypes eMilitaryStrength = pDiploAI->GetPlayerMilitaryStrengthComparedToUs(eFromPlayer);
	StrengthTypes eEconomicStrength = pDiploAI->GetPlayerEconomicStrengthComparedToUs(eFromPlayer);
	AggressivePostureTypes eMilitaryPosture = pDiploAI->GetMilitaryAggressivePosture(eFromPlayer);
	PlayerProximityTypes eProximity = GET_PLAYER(eMyPlayer).GetProximityToPlayer(eFromPlayer);

	// If we're friends with the demanding player, it's actually a Request for Help. Let's evaluate in a separate function and come back here
	if(MOD_DIPLOMACY_CIV4_FEATURES && pDiploAI->IsDoFAccepted(eFromPlayer))
	{
		eResponse = GetRequestForHelpResponse(pDeal);
	}
	else
	{
#endif
		// Too soon for another demand?
		if(pDiploAI->IsDemandTooSoon(eFromPlayer))
			eResponse = DEMAND_RESPONSE_REFUSE_TOO_SOON;
		// Not too soon for a demand
		else
		{
			bool bWeak = false;
			bool bHostile = false;
			// Initial odds of giving in to ANY demand are based on the player's boldness (which is also tied to the player's likelihood of going for world conquest)
			int iOddsOfGivingIn = (10 - pDiploAI->GetBoldness()) * 10;

			// Unforgivable: AI will never give in
			if (pDiploAI->GetMajorCivOpinion(eFromPlayer) == MAJOR_CIV_OPINION_UNFORGIVABLE)
			{
				bHostile = true;
				iOddsOfGivingIn -= 25;
			}
			// Hostile: AI will never give in
			if (eApproach == MAJOR_CIV_APPROACH_HOSTILE)
			{
				bHostile = true;
				iOddsOfGivingIn -= 25;
			}
			// They are very far away and have no units near us (from what we can tell): AI will never give in
			if (eProximity <= PLAYER_PROXIMITY_FAR && eMilitaryPosture == AGGRESSIVE_POSTURE_NONE)
			{
				bWeak = true;
				iOddsOfGivingIn -= 25;
			}
			// Our military is stronger: AI will never give in
			if (eMilitaryStrength < STRENGTH_AVERAGE && eEconomicStrength < STRENGTH_AVERAGE)
			{
				bWeak = true;
				iOddsOfGivingIn -= 25;
			}

			iValueWillingToGiveUp = 0;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			//Vassals give in to demands more often, and give more away.
			if(MOD_DIPLOMACY_CIV4_FEATURES)
			{
				TeamTypes eMasterTeam = GET_TEAM(GET_PLAYER(eMyPlayer).getTeam()).GetMaster();
				if(eMasterTeam == GET_PLAYER(eFromPlayer).getTeam())
				{
					iOddsOfGivingIn += 100;
					iValueWillingToGiveUp += 500;
				}
			}
			switch(pDiploAI->GetWarmongerThreat(eFromPlayer))
			{
				case THREAT_NONE:
				{
					iOddsOfGivingIn -= 10;
					break;
				}
				case THREAT_MINOR:
				{
					iOddsOfGivingIn -= 5;
					break;
				}
				case THREAT_MAJOR:
				{
					iOddsOfGivingIn += 10;
					break;
				}
				case THREAT_CRITICAL:
				{
					iOddsOfGivingIn += 25;
					break;
				}
			}

			switch (pDiploAI->GetMilitaryAggressivePosture(eFromPlayer))
			{
				case AGGRESSIVE_POSTURE_NONE:
				{
					iOddsOfGivingIn -= 10;
					break;
				}
				case AGGRESSIVE_POSTURE_LOW:
				{
					iOddsOfGivingIn -= 5;
					break;
				}
				case AGGRESSIVE_POSTURE_MEDIUM:
				{
					iOddsOfGivingIn += 10;
					break;
				}
				case AGGRESSIVE_POSTURE_HIGH:
				{
					iOddsOfGivingIn += 25;
					break;
				}
				case AGGRESSIVE_POSTURE_INCREDIBLE:
				{
					iOddsOfGivingIn += 50;
					break;
				}
			}
#endif
			// If we're afraid we're more likely to give in
			if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
			{
				iOddsOfGivingIn += 50;
				iValueWillingToGiveUp += 200;
			}
			// Not afraid
			else
			{
				// How strong are they compared to us?
				switch(eMilitaryStrength)
				{
				case STRENGTH_PATHETIC:
					iOddsOfGivingIn += -50;
					iValueWillingToGiveUp += 5;
					break;
				case STRENGTH_WEAK:
					iOddsOfGivingIn += -50;
					iValueWillingToGiveUp += 10;
					break;
				case STRENGTH_POOR:
					iOddsOfGivingIn += -25;
					iValueWillingToGiveUp += 20;
					break;
				case STRENGTH_AVERAGE:
					iOddsOfGivingIn += 0;
					iValueWillingToGiveUp += 40;
					break;
				case STRENGTH_STRONG:
					iOddsOfGivingIn += 15;
					iValueWillingToGiveUp += 60;
					break;
				case STRENGTH_POWERFUL:
					iOddsOfGivingIn += 25;
					iValueWillingToGiveUp += 80;
					break;
				case STRENGTH_IMMENSE:
					iOddsOfGivingIn += 50;
					iValueWillingToGiveUp += 100;
					break;
				default:
					break;
				}

				switch (eEconomicStrength)
				{
				case STRENGTH_PATHETIC:
					iOddsOfGivingIn += -50;
					iValueWillingToGiveUp += 5;
					break;
				case STRENGTH_WEAK:
					iOddsOfGivingIn += -50;
					iValueWillingToGiveUp += 10;
					break;
				case STRENGTH_POOR:
					iOddsOfGivingIn += -25;
					iValueWillingToGiveUp += 25;
					break;
				case STRENGTH_AVERAGE:
					iOddsOfGivingIn += 0;
					iValueWillingToGiveUp += 40;
					break;
				case STRENGTH_STRONG:
					iOddsOfGivingIn += 15;
					iValueWillingToGiveUp += 60;
					break;
				case STRENGTH_POWERFUL:
					iOddsOfGivingIn += 25;
					iValueWillingToGiveUp += 80;
					break;
				case STRENGTH_IMMENSE:
					iOddsOfGivingIn += 50;
					iValueWillingToGiveUp += 100;
					break;
				default:
					break;
				}
			}

			// IMPORTANT NOTE: This APPEARS to be very bad for multiplayer, but the only changes made to the game state are the fact that the human
			// made a demand, and if the deal went through. These are both sent over the network later in this function.

			int iRand = GC.getGame().getSmallFakeRandNum(100, iValueWillingToGiveUp);

			// Are they going to say no matter what?
			if (iRand > iOddsOfGivingIn)
			{
				if (bHostile)
					eResponse = DEMAND_RESPONSE_REFUSE_HOSTILE;
				else if (bWeak)
					eResponse = DEMAND_RESPONSE_REFUSE_WEAK;
				else
					eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
			}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		}
#endif
	}

	// Possibility exists that the AI will accept
	if(eResponse == NO_DEAL_RESPONSE_TYPE)
	{
		int iValueDemanded = 0;

		int iGPT = GetPlayer()->GetTreasury()->CalculateBaseNetGold();
		int iTempGold;
		int iModdedGoldValue;

		// Loop through items in this deal
		TradedItemList::iterator it;
		for(it = pDeal->m_TradedItems.begin(); it != pDeal->m_TradedItems.end(); ++it)
		{
			// Item from this AI
			if(it->m_eFromPlayer == eMyPlayer)
			{
				switch(it->m_eItemType)
				{
					// Gold
				case TRADE_ITEM_GOLD:
				{
					iTempGold = it->m_iData1;
					if (iGPT > 0)
						iModdedGoldValue = iTempGold * 5 / iGPT;
					else
						iModdedGoldValue = 0;

					iValueDemanded += max(iTempGold, iModdedGoldValue);
					break;
				}

				// GPT
				case TRADE_ITEM_GOLD_PER_TURN:
				{
					iValueDemanded += (it->m_iData1 * it->m_iDuration * 50 / 100);
					break;
				}

				// Resources
				case TRADE_ITEM_RESOURCES:
				{
					ResourceTypes eResource = (ResourceTypes) it->m_iData1;
					ResourceUsageTypes eUsage = GC.getResourceInfo(eResource)->getResourceUsage();

					if(eUsage == RESOURCEUSAGE_LUXURY)
						iValueDemanded += 100;
					else if(eUsage == RESOURCEUSAGE_STRATEGIC)
						iValueDemanded += (25 * it->m_iData2);

					break;
				}

				// Open Borders
				case TRADE_ITEM_OPEN_BORDERS:
				{
					iValueDemanded += 25;
					break;
				}

				case TRADE_ITEM_CITIES:
				case TRADE_ITEM_DEFENSIVE_PACT:
				case TRADE_ITEM_RESEARCH_AGREEMENT:
				{
					eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
					break;
				}
				case TRADE_ITEM_PERMANENT_ALLIANCE:
				case TRADE_ITEM_THIRD_PARTY_PEACE:
				case TRADE_ITEM_THIRD_PARTY_WAR:
				case TRADE_ITEM_THIRD_PARTY_EMBARGO:
				default:
					iValueDemanded += 75;
					break;
				}
			}
		}

		// No illegal items in the demand
		if(eResponse == NO_DEAL_RESPONSE_TYPE)
		{
			if(iValueDemanded <= iValueWillingToGiveUp)
				eResponse = DEMAND_RESPONSE_ACCEPT;
			else
				eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
		}
	}

	// Have to sent AI response through the network  - it affects AI behavior
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES && pDiploAI->IsDoFAccepted(eFromPlayer))
	{
		GC.getGame().DoFromUIDiploEvent(FROM_UI_DIPLO_EVENT_HUMAN_REQUEST, eMyPlayer, /*iData1*/ eResponse, -1);
	}
	else
	{
#endif
		GC.getGame().DoFromUIDiploEvent(FROM_UI_DIPLO_EVENT_HUMAN_DEMAND, eMyPlayer, /*iData1*/ eResponse, -1);
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	}
#endif

	// Demand agreed to
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(eResponse == DEMAND_RESPONSE_ACCEPT || (MOD_DIPLOMACY_CIV4_FEATURES && eResponse == DEMAND_RESPONSE_GIFT_ACCEPT))
#else
	if(eResponse == DEMAND_RESPONSE_ACCEPT)
#endif
	{
		CvDeal kDeal = *pDeal;
		//gDLL->sendNetDealAccepted(eFromPlayer, GetPlayer()->GetID(), kDeal, -1, -1, -1);
		GC.GetEngineUserInterface()->SetDealInTransit(true);

		auto_ptr<ICvDeal1> pDllDeal = GC.WrapDealPointer(&kDeal);
		gDLL->sendNetDemandAccepted(eFromPlayer, GetPlayer()->GetID(), pDllDeal.get());
	}

	return eResponse;
}

/// Demand has been agreed to
void CvDealAI::DoAcceptedDemand(PlayerTypes eFromPlayer, const CvDeal& kDeal)
{
	CvGame& kGame = GC.getGame();
	CvGameDeals& kGameDeals = kGame.GetGameDeals();
	const PlayerTypes eActivePlayer = kGame.getActivePlayer();
	const PlayerTypes ePlayer = GetPlayer()->GetID();
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		kGameDeals.FinalizeMPDeal(kDeal, true);
	}
	else
	{
		kGameDeals.AddProposedDeal(kDeal);
		kGameDeals.FinalizeDeal(eFromPlayer, ePlayer, true);
	}
#else
	kGameDeals.AddProposedDeal(kDeal);
	kGameDeals.FinalizeDeal(eFromPlayer, ePlayer, true);
#endif
	if(eActivePlayer == eFromPlayer || eActivePlayer == ePlayer)
	{
		GC.GetEngineUserInterface()->makeInterfaceDirty();
	}
}
/// Will this AI accept pDeal? Handles deal from both human and AI players
#if defined(MOD_BALANCE_CORE)
bool CvDealAI::IsDealWithHumanAcceptable(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValueToMe, int& iValueImOffering, int& iValueTheyreOffering, int& iAmountOverWeWillRequest, int& iAmountUnderWeWillOffer, bool* bCantMatchOffer, bool bFirstPass)
#else
bool CvDealAI::IsDealWithHumanAcceptable(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValueToMe, int& iValueImOffering, int& iValueTheyreOffering, int& iAmountOverWeWillRequest, int& iAmountUnderWeWillOffer, bool& bCantMatchOffer)
#endif
{
	CvAssertMsg(GET_PLAYER(eOtherPlayer).isHuman(), "DEAL_AI: Trying to see if AI will accept a deal with human player... but it's not human.  Please show Jon.");

	int iPercentOverWeWillRequest;
	int iPercentUnderWeWillOffer;
#if defined(MOD_BALANCE_CORE)
	*bCantMatchOffer = false;
#else
	bCantMatchOffer = false;
#endif
#if defined(MOD_BALANCE_CORE)
	if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		if(pDeal->GetRequestingPlayer() == GetPlayer()->GetID() && (iValueImOffering <= 0))
		{
			return true;
		}
	}
#endif
	// Deal leeway with human
	iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
	iPercentUnderWeWillOffer = GetDealPercentLeewayWithHuman();

	// Now do the valuation
	iTotalValueToMe = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// We're offering help to a player
	if (MOD_DIPLOMACY_CIV4_FEATURES && GetPlayer()->GetDiplomacyAI()->IsOfferingGift(eOtherPlayer))
	{
		return true;
	}
#endif

	// Important: check invalid return value!
	if (iTotalValueToMe==INT_MAX || iTotalValueToMe==(INT_MAX * -1))
		return false;

#if defined(MOD_BALANCE_CORE_DEALS)
	if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		//Getting 'error' values in this deal? It is bad, abort!
		if(iValueTheyreOffering <= -1e6 || iValueTheyreOffering >= 1e6)
			return false;

		//Getting 'error' values in this deal? It is bad, abort!
		if(iValueImOffering <= -1e6 || iValueImOffering >= 1e6)
			return false;
#endif

	// If no Gold in deal and within value of 1 GPT, then it's close enough
	if (pDeal->GetGoldTrade(eOtherPlayer) == 0 && pDeal->GetGoldTrade(m_pPlayer->GetID()) == 0)
	{
		int iOneGPT = 25;
		int iDiff = abs(iValueTheyreOffering - iValueImOffering);
		if (iDiff < iOneGPT)
		{
			return true;
		}
	}
#if defined(MOD_BALANCE_CORE)
	}
#endif

	int iDealSumValue = iValueImOffering + iValueTheyreOffering;

	iAmountOverWeWillRequest = iDealSumValue;
	iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
	iAmountOverWeWillRequest /= 100;

	iAmountUnderWeWillOffer = (iDealSumValue * -1);
	iAmountUnderWeWillOffer *= iPercentUnderWeWillOffer;
	iAmountUnderWeWillOffer /= 100;

	// We're surrendering
	if(pDeal->GetSurrenderingPlayer() == GetPlayer()->GetID())
	{
#if defined(MOD_BALANCE_CORE)
		int iMaxPeace = (GetCachedValueOfPeaceWithHuman() * -1);
		if(iTotalValueToMe < 0)
		{
			iTotalValueToMe *= -1;
		}
		if (iTotalValueToMe <= iMaxPeace)
#else
		if (iTotalValueToMe >= GetCachedValueOfPeaceWithHuman())
#endif
		{
			return true;
		}
	}

	// Peace deal where we're not surrendering, value must equal cached value
	else if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
#if defined(MOD_BALANCE_CORE)
		int iMaxPeace = GetCachedValueOfPeaceWithHuman();
		if (iTotalValueToMe >= iMaxPeace)
#else
		if (iTotalValueToMe >= GetCachedValueOfPeaceWithHuman())
#endif
		{
			return true;
		}
	}

	// If we've gotten the deal to a point where we're happy, offer it up
#if defined(MOD_BALANCE_CORE)
	if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer) && !bFirstPass)
	{
		if (iValueImOffering > 0)
		{
			//Is this a good deal for both parties?
			if (iTotalValueToMe >= iAmountUnderWeWillOffer && iTotalValueToMe <= iAmountOverWeWillRequest)
			{
				//Are they offer much more than we can afford? Let's lowball, but let them know.
				if (iValueTheyreOffering > (iValueImOffering * 2))
				{
					*bCantMatchOffer = true;
				}
				return true;
			}
		}
	}
	//This is to get the AI to actually make an offer the first time through.
	else if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer) && bFirstPass)
	{
		return false;
	}
#else
	else if(iTotalValueToMe <= iAmountOverWeWillRequest && iTotalValueToMe >= iAmountUnderWeWillOffer)
	{
		return true;
	}
	else if (iTotalValueToMe > iAmountOverWeWillRequest)
	{
		bCantMatchOffer = true;
	}
#endif
	return false;
}
/// Try to even out the value on both sides.  If bFavorMe is true we'll bias things in our favor if necessary
bool CvDealAI::DoEqualizeDealWithHuman(CvDeal* pDeal, PlayerTypes eOtherPlayer, bool bDontChangeMyExistingItems, bool bDontChangeTheirExistingItems, bool& bDealGoodToBeginWith, bool& bCantMatchOffer)
{
	bool bMakeOffer;
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	DEBUG_VARIABLE(eMyPlayer);

	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	CvAssertMsg(eMyPlayer != eOtherPlayer, "DEAL_AI: Trying to equalize human deal, but both players are the same.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iDealDuration = GC.getGame().GetDealDuration();
#if defined(MOD_BALANCE_CORE)
	if (pDeal->GetNumItems() <= 0)
	{
		return false;
	}

	GetPlayer()->GetDiplomacyAI()->SetCantMatchDeal(eOtherPlayer, false);
#else
	bCantMatchOffer = false;
#endif

	// Is this a peace deal?
	if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		pDeal->ClearItems();
		bMakeOffer = IsOfferPeace(eOtherPlayer, pDeal, true /*bEqualizingDeals*/);
	}
	else
	{
		int iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer;
#if defined(MOD_BALANCE_CORE)
		bMakeOffer = IsDealWithHumanAcceptable(pDeal, GC.getGame().getActivePlayer(), /*Passed by reference*/ iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, &bCantMatchOffer, true);
#else
		bMakeOffer = IsDealWithHumanAcceptable(pDeal, GC.getGame().getActivePlayer(), /*Passed by reference*/ iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, bCantMatchOffer);
#endif

		if (iTotalValueToMe < 0 && bDontChangeTheirExistingItems)
		{
			return false;
		}

		if(bMakeOffer)
		{
			bDealGoodToBeginWith = true;
		}
		else
		{
			bDealGoodToBeginWith = false;
		}

		if(!bMakeOffer)
		{
			/////////////////////////////
			// See if there are items we can add or remove from either side to balance out the deal if it's not already even
			/////////////////////////////

			bool bUseEvenValue = false;

			// Maybe reorder these based on the AI's priorities (e.g. if it really doesn't want to give up Strategic Resources try adding those from us last)
			
			// Hint: "Us" seems to mean our side of the table, so we would trade an item away!

#if defined(MOD_BALANCE_CORE)
			DoAddCitiesToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddThirdPartyWarToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddThirdPartyPeaceToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
#endif

			DoAddVoteCommitmentToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddVoteCommitmentToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

			DoAddEmbassyToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddEmbassyToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

			DoAddResourceToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
			DoAddResourceToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

			DoAddOpenBordersToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
			DoAddOpenBordersToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			if (MOD_DIPLOMACY_CIV4_FEATURES) {
				// AI would rather offer human resources/open borders than techs/maps
				DoAddTechToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
				DoAddTechToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

				DoAddMapsToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
				DoAddMapsToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);
			}
#endif

			DoAddGPTToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);
			DoAddGPTToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);

			DoAddGoldToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
			DoAddGoldToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, bUseEvenValue);

#if defined(MOD_BALANCE_CORE)
			DoAddCitiesToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddThirdPartyWarToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddThirdPartyPeaceToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
#endif

			//Do it again.
			DoAddGPTToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);
			DoAddGPTToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);


			if (!bDontChangeTheirExistingItems)
			{
				DoRemoveGPTFromThem(pDeal, eOtherPlayer, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);
			}
			if (!bDontChangeMyExistingItems)
			{
				DoRemoveGPTFromUs(pDeal, eOtherPlayer, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);
			}
			DoRemoveGoldFromUs(pDeal, eOtherPlayer, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
			DoRemoveGoldFromThem(pDeal, eOtherPlayer, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, bUseEvenValue);

			// Make sure we haven't removed everything from the deal!
			if(pDeal->m_TradedItems.size() > 0)
			{
#if defined(MOD_BALANCE_CORE)
				bMakeOffer = IsDealWithHumanAcceptable(pDeal, GC.getGame().getActivePlayer(), /*Passed by reference*/ iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, /*passed by reference*/&bCantMatchOffer, false);
				if (bCantMatchOffer)
				{
					GetPlayer()->GetDiplomacyAI()->SetCantMatchDeal(eOtherPlayer, true);
				}
				if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer))
				{
					//Getting 'error' values in this deal? It is bad, abort!
					if((iValueTheyreOffering == INT_MAX) || (iValueImOffering == INT_MAX) || (iTotalValueToMe == INT_MAX))
					{
						return false;
					}
				}
				//Let's not lowball, it leads to spam.
				if(iValueImOffering > 0 && (iTotalValueToMe > 0) && !bCantMatchOffer)
				{
					return true;
				}
#else
				bMakeOffer = IsDealWithHumanAcceptable(pDeal, GC.getGame().getActivePlayer(), /*Passed by reference*/ iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, /*passed by reference*/bCantMatchOffer);
				return true;
				}
#endif
			}
		}
	}

	return bMakeOffer;
}


/// Try to even out the value on both sides.  If bFavorMe is true we'll bias things in our favor if necessary
bool CvDealAI::DoEqualizeDealWithAI(CvDeal* pDeal, PlayerTypes eOtherPlayer)
{
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	DEBUG_VARIABLE(eMyPlayer);

	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	CvAssertMsg(eMyPlayer != eOtherPlayer, "DEAL_AI: Trying to equalize AI deal, but both players are the same.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iEvenValueImOffering;
	int iEvenValueTheyreOffering;
	int iTotalValue = GetDealValue(pDeal, iEvenValueImOffering, iEvenValueTheyreOffering, /*bUseEvenValue*/ true);

	int iDealDuration = GC.getGame().GetDealDuration();

	bool bMakeOffer = false;

	/////////////////////////////
	// Outline the boundaries for an acceptable deal
	/////////////////////////////
#if defined(MOD_BALANCE_CORE_DEALS)
	if (pDeal->GetNumItems() <= 0)
	{
		return false;
	}

	int iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eOtherPlayer);
	int iPercentUnderWeWillOffer = -GetDealPercentLeewayWithAI(eOtherPlayer);
#else
	int iPercentOverWeWillRequest = GetDealPercentLeewayWithAI();
	int iPercentUnderWeWillOffer = -GetDealPercentLeewayWithAI();
#endif
#if defined(MOD_BALANCE_CORE)
	if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);
	}
#endif

	int iDealSumValue = iEvenValueImOffering + iEvenValueTheyreOffering;

	int iAmountOverWeWillRequest = iDealSumValue;
	iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
	iAmountOverWeWillRequest /= 100;

	int iAmountUnderWeWillOffer = iDealSumValue;
	iAmountUnderWeWillOffer *= iPercentUnderWeWillOffer;
	iAmountUnderWeWillOffer /= 100;

	// Deal is already even enough for us
	if(iTotalValue <= iAmountOverWeWillRequest && iTotalValue >= iAmountUnderWeWillOffer)
	{
		bMakeOffer = true;
	}

	// If we set this pointer again it clears the data out!
	if(pDeal != GC.getGame().GetGameDeals().GetTempDeal())
	{
		GC.getGame().GetGameDeals().SetTempDeal(pDeal);
	}

	CvDeal* pCounterDeal = GC.getGame().GetGameDeals().GetTempDeal();

	if(!bMakeOffer)
	{
		/////////////////////////////
		// See if there are items we can add or remove from either side to balance out the deal if it's not already even
		/////////////////////////////

		bool bUseEvenValue = false;

#if defined(MOD_BALANCE_CORE)
		DoAddCitiesToThem(pCounterDeal, eOtherPlayer, false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
		DoAddThirdPartyWarToThem(pCounterDeal, eOtherPlayer, false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);
		DoAddThirdPartyPeaceToThem(pCounterDeal, eOtherPlayer, false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);
#endif

		DoAddVoteCommitmentToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
		DoAddVoteCommitmentToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

		DoAddResourceToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
		DoAddResourceToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

		DoAddOpenBordersToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ true, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
		DoAddOpenBordersToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ true, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		if (MOD_DIPLOMACY_CIV4_FEATURES) {
			DoAddTechToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddTechToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

			DoAddMapsToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddMapsToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);
		}
#endif

		DoAddGPTToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);
		DoAddGPTToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);

		DoAddGoldToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);
		DoAddGoldToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);

#if defined(MOD_BALANCE_CORE)
		DoAddCitiesToUs(pCounterDeal, eOtherPlayer, false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);
		DoAddThirdPartyWarToUs(pCounterDeal, eOtherPlayer, false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);
		DoAddThirdPartyPeaceToUs(pCounterDeal, eOtherPlayer, false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);
#endif

		DoRemoveGPTFromThem(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);
		DoRemoveGPTFromUs(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);

		DoRemoveGoldFromUs(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);
		DoRemoveGoldFromThem(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);

		DoAddGPTToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);
		DoAddGPTToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);

		// Make sure we haven't removed everything from the deal!
		if(pCounterDeal->m_TradedItems.size() > 0)
		{
			int iValueIThinkImOffering, iValueIThinkImGetting;
			GetDealValue(pDeal, iValueIThinkImOffering, iValueIThinkImGetting, /*bUseEvenValue*/ false);

			// We don't think we're getting enough for what's on our side of the table
			int iLowEndOfWhatIWillAccept = iValueIThinkImOffering - (iValueIThinkImOffering * -iPercentUnderWeWillOffer / 100);
			if(iValueIThinkImGetting < iLowEndOfWhatIWillAccept)
			{
				return false;
			}

			int iValueTheyThinkTheyreOffering, iValueTheyThinkTheyreGetting;
			GET_PLAYER(eOtherPlayer).GetDealAI()->GetDealValue(pCounterDeal, iValueTheyThinkTheyreOffering, iValueTheyThinkTheyreGetting, /*bUseEvenValue*/ false);

			// They don't think they're getting enough for what's on their side of the table
#if defined(MOD_BALANCE_CORE_DEALS)
			int iLowEndOfWhatTheyWillAccept = iValueTheyThinkTheyreOffering - (iValueTheyThinkTheyreOffering * GET_PLAYER(eOtherPlayer).GetDealAI()->GetDealPercentLeewayWithAI(m_pPlayer->GetID()) / 100);
#else
			int iLowEndOfWhatTheyWillAccept = iValueTheyThinkTheyreOffering - (iValueTheyThinkTheyreOffering * GET_PLAYER(eOtherPlayer).GetDealAI()->GetDealPercentLeewayWithAI() / 100);
#endif
			if(iValueTheyThinkTheyreGetting < iLowEndOfWhatTheyWillAccept)
			{
				return false;
			}

			bMakeOffer = true;
		}
	}

	return bMakeOffer;
}

/// What do we think of a Deal?
int CvDealAI::GetDealValue(CvDeal* pDeal, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue, bool bLogging)
{
	int iDealValue = 0;
	iValueImOffering = 0;
	iValueTheyreOffering = 0;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iItemValue = -1;

	bool bFromMe;
	PlayerTypes eOtherPlayer;

	eOtherPlayer = pDeal->m_eFromPlayer == eMyPlayer ? pDeal->m_eToPlayer : pDeal->m_eFromPlayer;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// We're offering help to a player
	if (MOD_DIPLOMACY_CIV4_FEATURES && eOtherPlayer != NO_PLAYER && GetPlayer()->GetDiplomacyAI()->IsOfferingGift(eOtherPlayer))
	{
		return 1;
	}
#endif
	TradedItemList::iterator it;
	for(it = pDeal->m_TradedItems.begin(); it != pDeal->m_TradedItems.end(); ++it)
	{
		if(eMyPlayer == it->m_eFromPlayer)
		{
			bFromMe = true;
			eOtherPlayer = pDeal->GetOtherPlayer(eMyPlayer);
		}
		else
		{
			bFromMe = false;
			eOtherPlayer = it->m_eFromPlayer;
		}

		// Multiplier is -1 if we're giving something away, 1 if we're receiving something
		int iValueMultiplier = bFromMe ? -1 : 1;

#if defined(MOD_BALANCE_CORE)
		if (!bLogging)
		{
			if (bFromMe)
			{
				if (!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eOtherPlayer, it->m_eItemType, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, true, true))
				{
					iItemValue = INT_MAX;
				}
			}
			else
			{
				if (!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), it->m_eItemType, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, true, true))
				{
					iItemValue = INT_MAX;
				}
			}
		}
		if(iItemValue != INT_MAX)
		{
			iItemValue = GetTradeItemValue(it->m_eItemType, bFromMe, eOtherPlayer, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, it->m_iDuration, bUseEvenValue, pDeal);
			it->m_iValue = iItemValue;
			it->m_bValueIsEven = bUseEvenValue;
		}
#else
		iItemValue = GetTradeItemValue(it->m_eItemType, bFromMe, eOtherPlayer, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, it->m_iDuration, bUseEvenValue);
#endif

		//if the item is invalid, return invalid value for the whole deal
		if (iItemValue==INT_MAX)
		{
			if(bFromMe)
			{
				iValueImOffering = INT_MAX;
			}
			else
			{
				iValueTheyreOffering = INT_MAX;
			}

			return iItemValue*iValueMultiplier;
		}

		iItemValue *= iValueMultiplier;
		iDealValue += iItemValue;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		if (MOD_DIPLOMACY_CIV4_FEATURES) {
			// Item is worth 20% less if it's owner is a vassal
			if(bFromMe)
			{
				// If it's my item and I'm the vassal of the other player, reduce it.
				if(GET_TEAM(GET_PLAYER(eMyPlayer).getTeam()).GetMaster() == GET_PLAYER(it->m_eFromPlayer).getTeam())
				{
					iItemValue *= 80;
					iItemValue /= 100;
				}
			}
			else
			{
				// If it's their item and they're my vassal, reduce it.
				if(GET_TEAM(GET_PLAYER(it->m_eFromPlayer).getTeam()).GetMaster() == GET_PLAYER(eMyPlayer).getTeam())
				{
					iItemValue *= 80;
					iItemValue /= 100;
				}
			}
		}
#endif

		// Figure out who's offering what, and keep track of the overall value on both sides of the deal
		if(iItemValue < 0)
		{
			iValueImOffering -= iItemValue;
		}
		else
		{
			iValueTheyreOffering += iItemValue;
		}
	}

	return iDealValue;
}

/// What is a particular item worth?
int CvDealAI::GetTradeItemValue(TradeableItems eItem, bool bFromMe, PlayerTypes eOtherPlayer, int iData1, int iData2, int iData3, bool bFlag1, int iDuration, bool bUseEvenValue, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to get deal item value for trading to oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eItem != TRADE_ITEM_NONE, "DEAL_AI: Trying to get value of TRADE_ITEM_NONE.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	if(eItem == TRADE_ITEM_GOLD)
		iItemValue = GetGoldForForValueExchange(/*Gold Amount*/ iData1, /*bNumGoldFromValue*/ false, bFromMe, eOtherPlayer, bUseEvenValue, /*bRoundUp*/ false);
	else if(eItem == TRADE_ITEM_GOLD_PER_TURN)
		iItemValue = GetGPTforForValueExchange(/*Gold Per Turn Amount*/ iData1, /*bNumGPTFromValue*/ false, iDuration, bFromMe, eOtherPlayer, bUseEvenValue, /*bRoundUp*/ false);
	else if (eItem == TRADE_ITEM_RESOURCES)
	{
		// precalculate, it's expensive
		int iCurrentNetGoldOfReceivingPlayer = bFromMe ? GET_PLAYER(eOtherPlayer).GetTreasury()->CalculateBaseNetGold() : m_pPlayer->GetTreasury()->CalculateBaseNetGold();
		iItemValue = GetResourceValue(/*ResourceType*/ (ResourceTypes)iData1, /*Quantity*/ iData2, iDuration, bFromMe, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);
	}
	else if(eItem == TRADE_ITEM_CITIES)
		//Don't even out city values ...
		iItemValue = GetCityValue(/*iX*/ iData1, /*iY*/ iData2, bFromMe, eOtherPlayer, false, pDeal);
	else if(eItem == TRADE_ITEM_ALLOW_EMBASSY)
		iItemValue = GetEmbassyValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_OPEN_BORDERS)
		iItemValue = GetOpenBordersValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_DEFENSIVE_PACT)
		iItemValue = GetDefensivePactValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_RESEARCH_AGREEMENT)
		iItemValue = GetResearchAgreementValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_TRADE_AGREEMENT)
		iItemValue = GetTradeAgreementValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(eItem == TRADE_ITEM_PEACE_TREATY)
		iItemValue = GetPeaceTreatyValue(eOtherPlayer);
	else if(eItem == TRADE_ITEM_THIRD_PARTY_PEACE)
		iItemValue = GetThirdPartyPeaceValue(bFromMe, eOtherPlayer, /*eWithTeam*/ (TeamTypes) iData1);
	else if(eItem == TRADE_ITEM_THIRD_PARTY_WAR)
		iItemValue = GetThirdPartyWarValue(bFromMe, eOtherPlayer, /*eWithTeam*/ (TeamTypes) iData1);
	else if(eItem == TRADE_ITEM_VOTE_COMMITMENT)
		iItemValue = GetVoteCommitmentValue(bFromMe, eOtherPlayer, iData1, iData2, iData3, bFlag1, bUseEvenValue);
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_MAPS)
		iItemValue = GetMapValue(bFromMe, eOtherPlayer, bUseEvenValue);
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_TECHS)
		iItemValue = GetTechValue(/*TechType*/ (TechTypes) iData1, bFromMe, eOtherPlayer);
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_VASSALAGE)
		iItemValue = GetVassalageValue(bFromMe, eOtherPlayer, bUseEvenValue, GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()));
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_VASSALAGE_REVOKE)
		iItemValue = GetRevokeVassalageValue(bFromMe, eOtherPlayer, bUseEvenValue, GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()));
#endif

	CvAssertMsg(iItemValue >= 0, "DEAL_AI: Trade Item value is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return iItemValue;
}

/// How much Gold should be provided if we're trying to make it worth iValue?
int CvDealAI::GetGoldForForValueExchange(int iGoldOrValue, bool bNumGoldFromValue, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, bool bRoundUp)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of Gold with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iMultiplier;
	int iDivisor;

	// We passed in Value, we want to know how much Gold we get for it
	if(bNumGoldFromValue)
	{
		iMultiplier = 100;
		iDivisor = /*100*/ GC.getEACH_GOLD_VALUE_PERCENT();
		// Protect against a modder setting this to 0
		if(iDivisor == 0)
			iDivisor = 1;
	}
	// We passed in an amount of Gold, we want to know how much it's worth
	else
	{
		iMultiplier = /*100*/ GC.getEACH_GOLD_VALUE_PERCENT();
		iDivisor = 100;
	}

	// Convert based on the rules above
	int iReturnValue = iGoldOrValue * iMultiplier;

#if !defined(MOD_BALANCE_CORE)
	int iModifier;
	// While we have a big number shall we apply some modifiers to it?
	if(bFromMe)
	{
		// Approach is important
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_WAR:
			iModifier = 200;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iModifier = 150;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			iModifier = 125;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iModifier = 110;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iModifier = 100;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iModifier = 110;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iModifier = 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Gold valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iModifier = 100;
			break;
		}
		// See whether we should multiply or divide
		if(!bNumGoldFromValue)
		{
			iReturnValue *= iModifier;
			iReturnValue /= 100;
		}
		else
		{
			iReturnValue *= 100;
			iReturnValue /= iModifier;
		}
	}
#endif

	// Sometimes we want to round up.  Let's say a the AI offers a deal to the human.  We have to ensure that the human can also offer that deal back and the AI will accept (and vice versa)
	if(bRoundUp)
	{
		iReturnValue += 99;
	}

	iReturnValue /= iDivisor;

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		iReturnValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetGoldForForValueExchange(iGoldOrValue, bNumGoldFromValue, !bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false, bRoundUp);

		iReturnValue /= 2;
	}

	return iReturnValue;
}

/// How much GPT should be provided if we're trying to make it worth iValue?
int CvDealAI::GetGPTforForValueExchange(int iGPTorValue, bool bNumGPTFromValue, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, bool bRoundUp)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of GPT with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iValueTimes100 = 0;

	// We passed in Value, we want to know how much GPT we get for it
	if(bNumGPTFromValue)
	{
		iValueTimes100 = iGPTorValue;
		iValueTimes100 /= iNumTurns;

		if (iGPTorValue > (GetPlayer()->calculateGoldRate() - 2))
		{
			iGPTorValue = (GetPlayer()->calculateGoldRate() - 2);
		}
	}
	else
	{
		if (bFromMe && iGPTorValue > (GetPlayer()->calculateGoldRate() - 2))
			return MAX_INT;

		iValueTimes100 = (iGPTorValue * iNumTurns);
	}

	// Sometimes we want to round up.  Let's say a the AI offers a deal to the human.  We have to ensure that the human can also offer that deal back and the AI will accept (and vice versa)
	if(bRoundUp)
	{
		iValueTimes100 += 99;
	}

	int iReturnValue = iValueTimes100;

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		iReturnValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetGPTforForValueExchange(iGPTorValue, bNumGPTFromValue, iNumTurns, !bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false, bRoundUp);

		iReturnValue /= 2;
	}

	return iReturnValue;
}

int CvDealAI::GetResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer)
{
	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	CvAssert(pkResourceInfo != NULL);
	if (pkResourceInfo == NULL)
		return 0;

	ResourceUsageTypes eUsage = pkResourceInfo->getResourceUsage();
	if (eUsage == RESOURCEUSAGE_LUXURY)
		return GetLuxuryResourceValue(eResource, iNumTurns, bFromMe, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);
	else
		return GetStrategicResourceValue(eResource, iResourceQuantity, iNumTurns, bFromMe, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);
}

/// How much is a Resource worth?
int CvDealAI::GetLuxuryResourceValue(ResourceTypes eResource, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Resource with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 10;

	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	CvAssert(pkResourceInfo != NULL);
	if (pkResourceInfo == NULL)
		return 0;

	//Integer zero check...
	if (iNumTurns <= 0)
		iNumTurns = 1;

	int iResult = 0;
	int iFlavors = 0;

	int iHappinessFromResource = pkResourceInfo->getHappiness();
	//Let's modify this value to make it more clearly pertain to current happiness for luxuries.
	iHappinessFromResource = GetPlayer()->GetBaseLuxuryHappiness();
	if (iHappinessFromResource < 1)
	{
		iHappinessFromResource = 1;
	}
	iItemValue += (iHappinessFromResource * iNumTurns);	// Ex: 1 Silk for 2 Happiness * 30 turns * 2 = 120

	//Let's look at flavors for resources
	for (int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		int iResourceFlavor = pkResourceInfo->getFlavorValue((FlavorTypes)i);
		if (iResourceFlavor > 0)
		{
			int iPersonalityFlavorValue = GetPlayer()->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)i);
			//Has to be above average to affect price. Will usually result in a x2-x3 modifier
			iResult += ((iResourceFlavor + iPersonalityFlavorValue) / 6);
			iFlavors++;
		}
	}
	if ((iResult > 0) && (iFlavors > 0))
	{
		//Get the average mulitplier from the number of Flavors being considered.
		iResult = (iResult / iFlavors);
		if (iResult > 0)
		{
			iItemValue *= iResult;
		}
	}
			
	if (bFromMe)
	{
		int iGPT = iCurrentNetGoldOfReceivingPlayer;
		//Every 10 gold in net GPT will increase resource value by 1, up to the value of the item itself (so never more than double).
		iGPT /= 10;
		if ((iGPT > 0) && (iGPT > iItemValue))
		{
			iGPT = iItemValue;
		}
		if (iGPT > 0)
		{
			iItemValue += iGPT;
		}
	}
	else
	{
		int iGPT = iCurrentNetGoldOfReceivingPlayer;
		//Every 15 gold in net GPT will increase resource value by 1, up to the value of the item itself (so never more than double).
		iGPT /= 15;
		if ((iGPT > 0) && (iGPT > iItemValue))
		{
			iGPT = iItemValue;
		}
		if (iGPT > 0)
		{
			iItemValue += iGPT;
		}
	}
	if (bFromMe)
	{
		if (GetPlayer()->IsEmpireUnhappy() && GetPlayer()->getNumResourceAvailable(eResource) == 1)
		{
			return INT_MAX;
		}
		if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(GetPlayer()->GetID(), eResource))
		{
			return INT_MAX;
		}
		if (GetPlayer()->getNumResourceAvailable(eResource) == 1)
		{
			int iFactor = 3;
			if (GetPlayer()->GetPlayerTraits()->GetLuxuryHappinessRetention())
				iFactor -= 1;
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(GetPlayer()->GetReligions()->GetCurrentReligion(), GetPlayer()->GetID());
			if (pReligion)
			{
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					if (pReligion->m_Beliefs.GetYieldPerLux((YieldTypes)iJ, GetPlayer()->GetID(), GetPlayer()->getCapitalCity()) > 0)
					{
						iFactor += 1;
					}
				}
			}

			iItemValue *= iFactor; //last one is x as valuable
		}

		//Let's consider how many resources each player has - if he has more than us, ours is worth more (and vice-versa).
		int iOtherHappiness = GET_PLAYER(eOtherPlayer).GetHappinessFromResources();
		int iOurHappiness = GetPlayer()->GetHappinessFromResources();
		//He's happier than us?
		if (iOtherHappiness >= iOurHappiness)
		{
			iItemValue *= 10;
			iItemValue /= 12;
		}
		//He is less happy than we are?
		else
		{
			iItemValue *= 12;
			iItemValue /= 10;
		}

		//How much is OUR stuff worth?
		switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false))
		{
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 90;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 90;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 150;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			iItemValue *= 200;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 300;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_WAR:
			iItemValue *= 500;
			iItemValue /= 100;
			break;
		}

		return iItemValue;
	}
	else
	{
		if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(GetPlayer()->GetID(), eResource))
			return 0;
		if (GetPlayer()->getNumResourceAvailable(eResource) > 0 && !GetPlayer()->GetPlayerTraits()->IsImportsCountTowardsMonopolies())
			return 0;

		if (GetPlayer()->IsEmpireUnhappy())
		{
			iItemValue += GetPlayer()->GetUnhappiness() * 2;
		}

		if (GetPlayer()->GetPlayerTraits()->IsImportsCountTowardsMonopolies() && GetPlayer()->GetMonopolyPercent(eResource) < GC.getGame().GetGreatestPlayerResourceMonopolyValue(eResource))
		{
			int iNumResourceOwned = GetPlayer()->getNumResourceTotal(eResource, false);
			int iNumResourceImported = GetPlayer()->getNumResourceTotal(eResource, true, true);
			//we don't want resources that won't get us a bonus.
			bool bBad = false;
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				if (pkResourceInfo->getYieldChangeFromMonopoly((YieldTypes)iJ) > 0 && iNumResourceOwned <= 0 && iNumResourceImported > 0)
				{
					bBad = true;
					return 0;
				}
			}
			if (!bBad)
			{
				iItemValue *= (100 + GetPlayer()->GetMonopolyPercent(eResource));
				iItemValue /= 100;
			}
		}
		
		int iFactor = 1;
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(GetPlayer()->GetReligions()->GetCurrentReligion(), GetPlayer()->GetID());
		if (pReligion)
		{
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				if (pReligion->m_Beliefs.GetYieldPerLux((YieldTypes)iJ, GetPlayer()->GetID(), GetPlayer()->getCapitalCity()) > 0)
				{
					iFactor += 1;
				}
			}
			if (iFactor > 1)
			{
				iItemValue *= iFactor; //last one is x as valuable
				iItemValue /= 2;
			}
		}

		//Let's consider how many resources each player has - if he has more than us, ours is worth more (and vice-versa).
		int iOtherHappiness = GET_PLAYER(eOtherPlayer).GetHappinessFromResources();
		int iOurHappiness = GetPlayer()->GetHappinessFromResources();
		//He's happier than us?
		if (iOtherHappiness >= iOurHappiness)
		{
			iItemValue *= 12;
			iItemValue /= 10;
		}
		//He is less happy than we are?
		else
		{
			iItemValue *= 10;
			iItemValue /= 12;
		}

		//How much is THEIR stuff worth?
		switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false))
		{
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 110;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 110;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 75;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			iItemValue *= 75;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 50;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_WAR:
			iItemValue *= 25;
			iItemValue /= 100;
			break;
		}

		return iItemValue;
	}
}

int CvDealAI::GetResourceRatio(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, ResourceTypes eResource)
{
	int iBase = (ePlayer == GetPlayer()->GetID()) ? 4 : 1;
	//Ratio between 0 and 100.
	int iValue = min(100, ((GET_PLAYER(ePlayer).getNumResourceAvailable(eResource) * 100) / (max(1, GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eResource)))));

	//I'm selling? Lower ratio means I have fewer (and mine are worth way more!)
	if (ePlayer == GetPlayer()->GetID())
		iBase *= (100 - iValue);
	//I'm buying? Lower ratio means I have more (and theirs are worth way less!)
	else
		iBase *= iValue;
	return max(0, iBase);
}

/// How much is a Resource worth?
int CvDealAI::GetStrategicResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Resource with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 10;

	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	CvAssert(pkResourceInfo != NULL);
	if (pkResourceInfo == NULL)
		return 0;

	if (iResourceQuantity == 0)
		return 0;

	//Integer zero check...
	if (iNumTurns <= 0)
		iNumTurns = 1;

	int iResult = 0;
	int iFlavors = 0;

	//Let's look at flavors for resources
	for (int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		int iResourceFlavor = pkResourceInfo->getFlavorValue((FlavorTypes)i);
		if (iResourceFlavor > 0)
		{
			int iPersonalityFlavorValue = GetPlayer()->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)i);
			//Has to be above average to affect price. Will usually result in a x2-x3 modifier
			iResult += ((iResourceFlavor + iPersonalityFlavorValue) / 5);
			iFlavors++;
		}
	}
	if ((iResult > 0) && (iFlavors > 0))
	{
		//Get the average mulitplier from the number of Flavors being considered.
		iResult = (iResult / iFlavors);
		if (iResult > 0)
		{
			iItemValue *= iResult;
		}
	}

	if (bFromMe)
	{
		int iGPT = iCurrentNetGoldOfReceivingPlayer;
		//Every 20 gold in net GPT will increase resource value by 1, up to the value of the item itself (so never more than double).
		iGPT /= 20;
		if ((iGPT > 0) && (iGPT > iItemValue))
		{
			iGPT = iItemValue;
		}
		if (iGPT > 0)
		{
			iItemValue += iGPT;
		}
	}
	else
	{
		int iGPT = iCurrentNetGoldOfReceivingPlayer;
		//Every 30 gold in net GPT will increase resource value by 1, up to the value of the item itself (so never more than double).
		iGPT /= 30;
		if ((iGPT > 0) && (iGPT > iItemValue))
		{
			iGPT = iItemValue;
		}
		if (iGPT > 0)
		{
			iItemValue += iGPT;
		}
	}
		
	if (bFromMe)
	{
		if (!GET_TEAM(GetPlayer()->getTeam()).IsResourceObsolete(eResource))
		{
			//Never trade away everything.
			int iNumRemaining = (GetPlayer()->getNumResourceAvailable(eResource) - iResourceQuantity);
			if (iNumRemaining <= 0)
				return INT_MAX;

			//If they're stronger than us, strategic resources are valuable.
			if (GetPlayer()->GetMilitaryMight() < GET_PLAYER(eOtherPlayer).GetMilitaryMight())
			{
				iItemValue *= 5;
				iItemValue /= 3;
			}
			else
			{
				iItemValue *= 9;
				iItemValue /= 10;
			}
			//Good target? Don't sell to them!
			bool bGood = false;
			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && eLoopPlayer != eOtherPlayer && eLoopPlayer != GetPlayer()->GetID())
				{
					if (GetPlayer()->GetDiplomacyAI()->IsWantsSneakAttack(eLoopPlayer))
					{
						bGood = true;
					}
				}
			}
			if (bGood)
			{
				iItemValue *= 3;
			}
			//Are they close, or far away? We should always be a bit less eager to sell war resources from neighbors.
			if (GetPlayer()->GetProximityToPlayer(eOtherPlayer) >= PLAYER_PROXIMITY_CLOSE)
			{
				iItemValue *= 3;
				iItemValue /= 2;
			}
			//Are we going for science win? Don't sell aluminum!
			ProjectTypes eApolloProgram = (ProjectTypes)GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
			if (!bFromMe && eApolloProgram != NO_PROJECT)
			{
				if (GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).getProjectCount(eApolloProgram) > 0)
				{
					ResourceTypes eAluminumResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
					if (eResource == eAluminumResource)
					{
						return INT_MAX;
					}
				}
			}

			if (MOD_DIPLOMACY_CITYSTATES)
			{
				ResourceTypes ePaper = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_PAPER", true);
				if (eResource == ePaper)
				{
					if (GetPlayer()->GetDiplomacyAI()->IsGoingForDiploVictory())
					{
						return INT_MAX;
					}
				}
			}

			//Increase value based on number remaining (up to 10).
			iItemValue += ((10 - min(10, iNumRemaining)) * 10);

			//How much do we have compared to them?
			int iResourceRatio = GetResourceRatio(GetPlayer()->GetID(), eOtherPlayer, eResource);

			//More we have compared to them, the less what we have is worth,and vice-versa!
			iItemValue *= (100 + iResourceRatio);
			iItemValue /= 100;

			// Approach is important
			switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false))
			{
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 90;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				iItemValue *= 90;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				iItemValue *= 100;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 115;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_DECEPTIVE:
				iItemValue *= 150;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 200;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_WAR:
				iItemValue *= 300;
				iItemValue /= 100;
				break;
			}

			//And now speed/quantity.
			iItemValue *= (iResourceQuantity * iNumTurns);
			iItemValue /= 25;

			return iItemValue;
		}
		else
		{
			return INT_MAX;
		}
	}
	else
	{
		if (!GET_TEAM(GetPlayer()->getTeam()).IsResourceObsolete(eResource))
		{
			//If they're stronger than us, strategic resources are less valuable, as we might war soon.
			if (GetPlayer()->GetMilitaryMight() < GET_PLAYER(eOtherPlayer).GetMilitaryMight())
			{
				iItemValue *= 3;
				iItemValue /= 5;
			}
			else
			{
				iItemValue *= 10;
				iItemValue /= 9;
			}
			//Good target? Don't buy from them!
			bool bGood = false;
			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && eLoopPlayer != eOtherPlayer && eLoopPlayer != GetPlayer()->GetID())
				{
					if (GetPlayer()->GetDiplomacyAI()->IsWantsSneakAttack(eLoopPlayer))
					{
						bGood = true;
					}
				}
			}
			if (bGood)
			{
				iItemValue /= 2;
			}
			//Are they close, or far away? We should always be a bit less eager to buy war resources from neighbors.
			if (GetPlayer()->GetProximityToPlayer(eOtherPlayer) >= PLAYER_PROXIMITY_CLOSE)
			{
				iItemValue *= 2;
				iItemValue /= 3;
			}
			//Are we going for science win? Buy aluminum!
			ProjectTypes eApolloProgram = (ProjectTypes)GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
			if (!bFromMe && eApolloProgram != NO_PROJECT)
			{
				if (GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).getProjectCount(eApolloProgram) > 0)
				{
					ResourceTypes eAluminumResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
					if (eResource == eAluminumResource)
					{
						iItemValue *= 3;
						iItemValue /= 2;
					}
				}
			}

			if (MOD_DIPLOMACY_CITYSTATES)
			{
				ResourceTypes ePaper = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_PAPER", true);
				if (eResource == ePaper)
				{
					if (GetPlayer()->GetDiplomacyAI()->IsGoingForDiploVictory())
					{
						iItemValue *= 3;
						iItemValue /= 2;
					}
				}
			}
			if (GetPlayer()->GetPlayerTraits()->IsImportsCountTowardsMonopolies() && GetPlayer()->GetMonopolyPercent(eResource) < GC.getGame().GetGreatestPlayerResourceMonopolyValue(eResource))
			{
				int iNumResourceOwned = GetPlayer()->getNumResourceTotal(eResource, false);
				//we don't want resources that won't get us a bonus.
				bool bBad = false;
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					if (pkResourceInfo->getYieldChangeFromMonopoly((YieldTypes)iJ) > 0 && iNumResourceOwned <= 0)
					{
						return 0;
					}
				}
				if (!bBad)
				{
					//More we have compared to them, the less what they have is worth, and vice-versa!
					iItemValue *= (100 + (GetPlayer()->GetMonopolyPercent(eResource) * 2));
					iItemValue /= 100;
				}
			}
			else
			{
				//Decrease value based on number we have.
				int iNumRemaining = (GetPlayer()->getNumResourceAvailable(eResource));
				//20% minimum.
				iItemValue -= min((iItemValue / 5), (iNumRemaining * 20));

				if (iItemValue <= 0)
					iItemValue = 1;

				//How much do they have compared to us?
				int iResourceRatio = GetResourceRatio(eOtherPlayer, GetPlayer()->GetID(), eResource);
				//More we have compared to them, the less what they have is worth, and vice-versa!
				iItemValue *= 100;
				iItemValue /= (100 + iResourceRatio);
			}

			// Approach is important
			switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false))
			{
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 110;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				iItemValue *= 110;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				iItemValue *= 100;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 80;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_DECEPTIVE:
				iItemValue *= 100;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 50;
				iItemValue /= 100;
				break;
			case MAJOR_CIV_APPROACH_WAR:
				iItemValue *= 50;
				iItemValue /= 100;
				break;
			}

			//And now speed/quantity.
			iItemValue *= (iResourceQuantity * iNumTurns);
			iItemValue /= 10;

			return iItemValue;
		}
		else
		{
			return 0;
		}
	}
}

#if defined(MOD_BALANCE_CORE_DEALS)
/// How much is a City worth - that is: how much would the buyer pay?
int CvDealAI::GetCityValue(int iX, int iY, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, CvDeal* pDeal)
{
	CvCity* pCity = GC.getMap().plot(iX, iY)->getPlotCity();
	if (!pCity)
		return INT_MAX;

	//note that it can also happen that a player pretends to buy a city they already own, just to see the appropriate price
	CvPlayer& sellingPlayer = GET_PLAYER(bFromMe ? GetPlayer()->GetID() : eOtherPlayer);
	CvPlayer& buyingPlayer = GET_PLAYER(bFromMe ? eOtherPlayer : GetPlayer()->GetID());

	//initial value - if we founded the city, we like it more
	bool bOurs = pCity->getOriginalOwner() == buyingPlayer.GetID();
	int iItemValue = (bOurs) ? 25000 : 20000;

	//If at war, halve the value (that way it'll fit in a peace deal's valuation model).
	if (sellingPlayer.IsAtWarWith(buyingPlayer.GetID()))
		iItemValue /= 2;

	//obviously the seller doesn't really want it
	if (pCity->IsRazing())
		iItemValue /= 2;

	//economic value is important
	int iEconomicValue = pCity->getEconomicValue(buyingPlayer.GetID());

	//If not as good as any of our cities, we don't want it.
	bool bGood = false;
	//We're buying a city.
	if(!bFromMe)
	{
		int iBetterThanTotal = 0;
		CvCity* pLoopCity;
		int iCityLoop;
		int iTargetEconomicDelta = (iEconomicValue / (max(1, pCity->getPopulation())));

		for(pLoopCity = buyingPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = buyingPlayer.nextCity(&iCityLoop))
		{
			if(pLoopCity != NULL)
			{
				//If city we're looking at is better than or equal to one of our cities, go for it.

				int iEconomicDelta = (pLoopCity->getEconomicValue(buyingPlayer.GetID()) / (max(1, pLoopCity->getPopulation())));
			
				//Better per capita? Not good!
				if (iTargetEconomicDelta > iEconomicDelta)
				{
					iBetterThanTotal++;
				}
			}
		}
		if (iBetterThanTotal > (buyingPlayer.getNumCities() / 2))
		{
			bGood = true;
		}
	}
	//We're selling a city.
	else if (bFromMe)
	{
		CvCity* pLoopCity;
		int iCityLoop;
		int iBetterThanTotal = 0;
		int iTargetEconomicDelta = (iEconomicValue / (max(1, pCity->getPopulation())));
		for (pLoopCity = sellingPlayer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = sellingPlayer.nextCity(&iCityLoop))
		{
			if (pLoopCity != NULL)
			{
				//If city they're looking at is better than or equal to one of our cities, do not trade it.

				int iEconomicDelta = (pLoopCity->getEconomicValue(sellingPlayer.GetID()) / (max(1, pLoopCity->getPopulation())));
				
				//Better per capita? Not good!
				if (iTargetEconomicDelta > iEconomicDelta)
				{
					iBetterThanTotal++;
				}
			}
		}
		//Top half of our cities? Do not sell!
		if (iBetterThanTotal > (sellingPlayer.getNumCities() / 2))
		{
			bGood = true;
		}
	}
		
	
	iItemValue += (iEconomicValue / 2);

	//first some amount for the territory
	int iInternalBorderCount = 0;
	int iCityTiles = 0;
#if defined(MOD_BALANCE_CORE)
	if (sellingPlayer.IsAtPeaceWith(buyingPlayer.GetID()))
	{
		if (!bOurs && (pCity->IsRazing() || pCity->IsResistance()))
		{
			return INT_MAX;
		}
		//I traded for this city once before? Don't trade again.
		if(bFromMe && pCity->IsTraded(GetPlayer()->GetID()))
		{
			return INT_MAX;
		}
	}
	if(bFromMe && pCity->GetCityReligions()->IsHolyCityAnyReligion())
	{
		return INT_MAX;
	}
#endif


	for(int iI = 1; iI < MAX_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);
		if (!pLoopPlot)
			continue;

		//if it belongs to the city
		if (pCity->GetID() == pLoopPlot->GetCityPurchaseID())
			iCityTiles++;
		else if (pLoopPlot->getOwner() == buyingPlayer.GetID())
			//belongs to another one of the buyer's cities
			iInternalBorderCount++;
	}

	//this is how much ANY plot is worth to the buyer right now
	int goldPerPlot = buyingPlayer.GetBuyPlotCost();
	iItemValue += goldPerPlot * iCityTiles;

	//important. it's valuable to have as much internal border as possible
	iItemValue += goldPerPlot * 8 * iInternalBorderCount;

	//re-use the gold value as a general unit and penalize unhappy citizens
	iItemValue -= pCity->getUnhappyCitizenCount() * goldPerPlot * 3;

	if (sellingPlayer.IsAtPeaceWith(buyingPlayer.GetID()))
	{
		//Not good? Offer much less.
		if (!bFromMe && !bGood && !bOurs)
		{
			iItemValue /= 4;
		}
		if (bFromMe && bGood)
		{
			return MAX_INT;
		}
	}

	// Opinion also matters - but not if we're doing this for a peace settlement
	if (!bFromMe && sellingPlayer.IsAtPeaceWith(buyingPlayer.GetID()))
	{
		//brainfuck. we do this from the buyer's perspective and assume he knows the seller's opinion of him
		//so the less the seller likes the buyer, the more we offer for the city
		switch (sellingPlayer.GetDiplomacyAI()->GetMajorCivApproach(buyingPlayer.GetID(), false))
		{
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			//won't give up the city at any price if we're not friends
			if(bOurs)
			{
				iItemValue *= 200;
			}
			else
			{
				iItemValue *= 100;
			}
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			//won't give up the city at any price if we're not friends
			if (bOurs)
			{
				iItemValue *= 200;
			}
			else
			{
				iItemValue *= 100;
			}
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			//won't give up the city at any price if we're not friends
			if (bOurs)
			{
				iItemValue *= 150;
			}
			else
			{
				iItemValue *= 75;
			}
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			//won't give up the city at any price if we're not friends
			if (bOurs)
			{
				iItemValue *= 200;
			}
			else
			{
				iItemValue *= 125;
			}
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			//won't give up the city at any price if we're not friends
			if (bOurs)
			{
				iItemValue *= 75;
			}
			else
			{
				iItemValue *= 50;
			}
			break;
		case MAJOR_CIV_APPROACH_WAR:
		default:
			//won't give up the city at any price if we're not friends
			if (bOurs)
			{
				iItemValue *= 50;
			}
			else
			{
				return INT_MAX;
			}
			break;
		}
		iItemValue /= 100;
	}

	// Opinion also matters - but not if we're doing this for a peace settlement
	if (bFromMe && sellingPlayer.IsAtPeaceWith(buyingPlayer.GetID()))
	{
		//brainfuck. we do this from the buyer's perspective and assume he knows the seller's opinion of him
		//so the less the seller likes the buyer, the more we offer for the city
		switch (sellingPlayer.GetDiplomacyAI()->GetMajorCivApproach(buyingPlayer.GetID(), false))
		{
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 125;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			iItemValue *= 150;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
		case MAJOR_CIV_APPROACH_HOSTILE:
		case MAJOR_CIV_APPROACH_WAR:
		default:
			return MAX_INT;
		}
		iItemValue /= 100;
	}

	//note: cannot use GetCityDistanceInEstimatedTurns() here, as the city itself is included in the map result would be zero
	if (!bOurs)
	{
		CvCity* pClosestCity = NULL;
		int iBuyerDistance = INT_MAX;
		int iLoop = 0;
		for (CvCity* pRefCity = buyingPlayer.firstCity(&iLoop); pRefCity != NULL; pRefCity = buyingPlayer.nextCity(&iLoop))
		{
			if (pRefCity == pCity)
				continue;

			int iTemp = plotDistance(pRefCity->getX(), pRefCity->getY(), pCity->getX(), pCity->getY());
			if (iTemp < iBuyerDistance)
			{
				pClosestCity = pRefCity;
				iBuyerDistance = iTemp;
			}
		}
		if (iBuyerDistance <= 2)
		{
			iBuyerDistance = 2;
		}

		if (pClosestCity != NULL && pDeal != NULL && pDeal->IsCityInDeal(pClosestCity->getOwner(), pClosestCity->GetID()))
		{
			return MAX_INT;
		}

		//buyer likes it close to home (up to 50% bonus) - this is in addition to the tile overlap above
		int iRefDist = GC.getAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT();
		if (iBuyerDistance > (iRefDist + 2))
		{
			return MAX_INT;
		}

		iItemValue -= (iBuyerDistance * 1000);

		if (iItemValue <= 0)
			return MAX_INT;
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES)
	{
		if (GET_TEAM(sellingPlayer.getTeam()).IsVassal(buyingPlayer.getTeam()))
		{
			iItemValue *= 70;
			iItemValue /= 100;
		}
	}
#endif
	//Alright, so we need to check the deal to see how many cities are in it. If we're already offering one city, greatly increase the cost of a second+ one on offer.
	if(bFromMe)
	{
		if(pDeal != NULL)
		{
			int iNumCities = pDeal->GetNumCities(m_pPlayer->GetID());
			if(iNumCities > 0)
			{
				iItemValue += (5000 * iNumCities);
			}
		}
	}

	//if we currently own the city, the price is higher.
	if (bFromMe && pCity->getOwner() == buyingPlayer.GetID())
	{
		if (pCity->IsPuppet())
		{
			iItemValue *= 130;
			iItemValue /= 100;
		}
		else
		{
			iItemValue *= 150;
			iItemValue /= 100;
		}

		int iWonders = pCity->getNumWorldWonders() + pCity->getNumNationalWonders();
		if (iWonders>0)
		{
			iItemValue *= (100 * iWonders);
			iItemValue /= 100;
		}
	}

	//Resistance? Reduces the value
	iItemValue *= 100 - MapToPercent(pCity->GetResistanceTurns(), 3, 9)/5;
	iItemValue /= 100;

	// Original capitals are very valuable
	if (bFromMe && pCity->IsOriginalMajorCapital())
		return MAX_INT;

	if (!bFromMe && pCity->IsOriginalMajorCapital())
		iItemValue *= 4;

	if (!GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsDoFAccepted(GetPlayer()->GetID()))
	{
		iItemValue *= 4;
	}

	// so here's the tricky part - convert to gold
	iItemValue /= 3;

	//OutputDebugString(CvString::format("City value for %s from %s to %s is %d\n", pCity->getName().c_str(), sellingPlayer.getName(), buyingPlayer.getName(), iItemValue).c_str());

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetCityValue(iX, iY, !bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false, pDeal);

		if (iReverseValue == INT_MAX)
			//no deal, can't agree on a value
			iItemValue = INT_MAX;
		else
			iItemValue = (iItemValue + iReverseValue)/2;
	}

	return iItemValue;
}

#else

/// How much is a City worth?
int CvDealAI::GetCityValue(int iX, int iY, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of City with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	CvCity* pCity = GC.getMap().plot(iX, iY)->getPlotCity();

	if(pCity != NULL)
	{
		iItemValue = 440 + (pCity->getPopulation() * 200);

		// add in the value of every plot this city owns (plus improvements and resources)
		// okay, I'm only going to count in the 3-rings plots since we can't actually use any others (I realize there may be a resource way out there)

		int goldPerPlot = GetPlayer()->GetBuyPlotCost(); // this is how much ANY plot is worth to me right now

		int iGoldValueOfPlots = 0;
		int iGoldValueOfImprovedPlots = 0;
		int iGoldValueOfResourcePlots = 0;

		for(int iI = 0; iI < pCity->GetNumWorkablePlots(); iI++)
		{
			CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);
			if(NULL != pLoopPlot && pCity->GetID() == pLoopPlot->GetCityPurchaseID())
			{
				if(iI > 6)
				{
					iGoldValueOfPlots += goldPerPlot; // this is a bargain, but at least it's in the ballpark
				}
				ResourceTypes eResource = pLoopPlot->getNonObsoleteResourceType(GetPlayer()->getTeam());
				if(eResource != NO_RESOURCE)
				{
					const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
					if (pkResourceInfo)
					{
						ResourceUsageTypes eUsage = pkResourceInfo->getResourceUsage();
						int iResourceQuantity = pLoopPlot->getNumResource();
						// Luxury Resource
						if(eUsage == RESOURCEUSAGE_LUXURY)
						{
							int iNumTurns = min(1,GC.getGame().getMaxTurns() - GC.getGame().getGameTurn());
							iNumTurns = max(120,iNumTurns); // let's not go hog wild here
							int iHappinessFromResource = pkResourceInfo->getHappiness();
							iGoldValueOfResourcePlots += (iResourceQuantity * iHappinessFromResource * iNumTurns * 2);	// Ex: 1 Silk for 4 Happiness * 30 turns * 2 = 240
							// If we only have 1 of a Luxury then we value it much more
							if(bFromMe)
							{
								if(GetPlayer()->getNumResourceAvailable(eResource) == 1)
								{
									iGoldValueOfResourcePlots += (iResourceQuantity * iHappinessFromResource * iNumTurns * 4);
								}
							}
						}
						// Strategic Resource
						else if(eUsage == RESOURCEUSAGE_STRATEGIC)
						{
							int iNumTurns = 60; // okay, this is a reasonable estimate
							iGoldValueOfResourcePlots += (iResourceQuantity * iNumTurns * 150 / 100);
						}
					}
				}
			}
		}
		iGoldValueOfImprovedPlots /= 100;

		iItemValue = iItemValue + iGoldValueOfPlots + iGoldValueOfImprovedPlots + iGoldValueOfResourcePlots;

		// add in the (gold) value of the buildings (Or should we?  Will they transfer?)

		// From this player - add extra weight (don't want the human giving the AI a bit of gold for good cities)
		if(bFromMe)
		{
			// Wonders are nice
			if(pCity->hasActiveWorldWonder())
				iItemValue *= 2;

			// Adjust for how well a war against this player would go (or is going)
			switch(GetPlayer()->GetDiplomacyAI()->GetWarProjection(eOtherPlayer))
			{
			case WAR_PROJECTION_DESTRUCTION:
				iItemValue *= 100;
				break;
			case WAR_PROJECTION_DEFEAT:
				iItemValue *= 180;
				break;
			case WAR_PROJECTION_STALEMATE:
				iItemValue *= 220;
				break;
			case WAR_PROJECTION_UNKNOWN:
				iItemValue *= 250;
				break;
			case WAR_PROJECTION_GOOD:
				iItemValue *= 400;
				break;
			case WAR_PROJECTION_VERY_GOOD:
				iItemValue *= 400;
				break;
			default:
				CvAssertMsg(false, "DEAL_AI: AI player has no valid War Projection for City valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 300;
				break;
			}
			iItemValue /= 100;

			// AI players should be less willing to trade cities when not at war
			if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()))
			{
				iItemValue *= 2;
			}

		}	// END bFromMe
		else
		{
			CvPlayerAI& theOtherPlayer = GET_PLAYER(eOtherPlayer);
			if(!GET_TEAM(GetTeam()).isAtWar(theOtherPlayer.getTeam()))
			{
				if(theOtherPlayer.isHuman())  // he is obviously trying to trick us
				{
					CvCity* pLoopCity;
					int iCityLoop;
					int iBestDistance = 99;
					for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
					{
						int iDistFromThisCity = plotDistance(iX, iY, pLoopCity->getX(), pLoopCity->getY());
						if(iDistFromThisCity < iBestDistance)
						{
							iBestDistance = iDistFromThisCity;
						}
					}
					iBestDistance = (iBestDistance > 4) ? iBestDistance : 5;
					iItemValue /= iBestDistance - 4;
					iItemValue = (iItemValue >= 100) ? iItemValue : 100;
				}
			}
		}

		// slewis - Due to rule changes, value of major capitals should go up quite a bit because someone can win the game by owning them
		if (pCity->IsOriginalMajorCapital())
		{
			iItemValue *= 2;
		}
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetCityValue(iX, iY, !bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		iItemValue /= 2;
	}

	return iItemValue;
}

#endif

// How much is an embassy worth?
int CvDealAI::GetEmbassyValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Embassy with oneself.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
#if defined(MOD_BALANCE_CORE)
	int iItemValue = 50;
#else
	int iItemValue = 35;
#endif

	// Scale up or down by deal duration at this game speed
	CvGameSpeedInfo *pkStdSpeedInfo = GC.getGameSpeedInfo((GameSpeedTypes)GC.getSTANDARD_GAMESPEED());
	if (pkStdSpeedInfo)
	{
		iItemValue *= GC.getGame().getGameSpeedInfo().GetDealDuration();
		iItemValue /= pkStdSpeedInfo->GetDealDuration();
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES) {
		// Item is worth 20% less if it's owner is a vassal
		if (bFromMe)
		{
			// If it's my item and I'm the vassal of the other player, accept it.
			if (GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetMaster() == GET_PLAYER(GetPlayer()->GetID()).getTeam())
			{
				return iItemValue;
			}
		}
		else
		{
			// If it's their item and they're my vassal, accept it.
			if (GET_TEAM(GetPlayer()->getTeam()).GetMaster() == GET_PLAYER(eOtherPlayer).getTeam())
			{
				return iItemValue;
			}
		}
	}
#endif

	if(bFromMe)  // giving the other player an embassy in my capital
	{
#if defined(MOD_BALANCE_CORE)
		if(GetPlayer()->GetDiplomacyAI()->IsDenouncedPlayer(eOtherPlayer) || GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsDenouncedPlayer(GetPlayer()->GetID()))
			return INT_MAX;
#endif
		// Approach is important
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 250;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 130;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 80;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Research Agreement valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}
#if defined(MOD_BALANCE_CORE)
	if(!bFromMe)  // they want to build an embassy with us.
	{
		if(GetPlayer()->GetDiplomacyAI()->IsDenouncedPlayer(eOtherPlayer) || GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsDenouncedPlayer(GetPlayer()->GetID()))
			return INT_MAX;

		// Approach is important
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 30;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 60;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 120;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 150;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Research Agreement valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}
#endif

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetEmbassyValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);
		if (iReverseValue == INT_MAX)
			//no deal, can't agree on a value
			iItemValue = INT_MAX;
		else
			iItemValue = (iItemValue + iReverseValue)/2;
	}

	return iItemValue;
}

/// How much in V-POINTS (aka value) is Open Borders worth?  You gotta admit that V-POINTS sound pretty cool though
int CvDealAI::GetOpenBordersValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of Open Borders with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true);

	// If we're friends, then OB is always equally valuable to both parties
#if !defined(MOD_BALANCE_CORE)
	if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		return 50;
#else
	int iItemValue = 100;
#endif

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES) {
		// Item is worth 20% less if it's owner is a vassal
		if (bFromMe)
		{
			// If it's my item and I'm the vassal of the other player, accept it.
			if (GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetMaster() == GET_PLAYER(GetPlayer()->GetID()).getTeam())
			{
				return iItemValue;
			}
		}
		else
		{
			// If it's their item and they're my vassal, accept it.
			if (GET_TEAM(GetPlayer()->getTeam()).GetMaster() == GET_PLAYER(eOtherPlayer).getTeam())
			{
				return iItemValue;
			}
		}
	}
#endif

	// Me giving Open Borders to the other guy
	if(bFromMe)
	{
		if (!GetPlayer()->GetDiplomacyAI()->IsWillingToGiveOpenBordersToPlayer(eOtherPlayer))
		{
			return INT_MAX;
		}
		// Approach is important
		switch(eApproach)
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
		case MAJOR_CIV_APPROACH_GUARDED:
		case MAJOR_CIV_APPROACH_WAR:
			return INT_MAX;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			iItemValue += 400;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue += 300;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue += 50;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue += 200;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Open Borders valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue += 100;
			break;
		}

		// If they're at war with our enemies then we're more likely to give them OB
		int iNumEnemiesAtWarWith = GetPlayer()->GetDiplomacyAI()->GetNumOurEnemiesPlayerAtWarWith(eOtherPlayer);
#if defined(MOD_BALANCE_CORE_DEALS)
		if(iNumEnemiesAtWarWith > 0)
#else
		if(iNumEnemiesAtWarWith >= 2)
#endif
		{
#if defined(MOD_BALANCE_CORE_DEALS)
			iItemValue *= (100 - (iNumEnemiesAtWarWith * 5));
#else
			iItemValue *= 10;
#endif
			iItemValue /= 100;
		}
#if !defined(MOD_BALANCE_CORE_DEALS)
		else if(iNumEnemiesAtWarWith == 1)
		{
			iItemValue *= 50;
			iItemValue /= 100;
		}
#endif
#if defined(MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS)
		if (GetPlayer()->GetCulture()->GetTourism() > 0 )
		{
			// The civ we need influence on the most should ALWAYS be included
			if (GetPlayer()->GetCulture()->GetCivLowestInfluence(false /*bCheckOpenBorders*/) == eOtherPlayer)
			{
				iItemValue *= 50;
				iItemValue /= 100;
			}

			// If have influence over half the civs, want OB with the other half
			if (GetPlayer()->GetCulture()->GetNumCivsToBeInfluentialOn() <= GetPlayer()->GetCulture()->GetNumCivsInfluentialOn())
			{
				if (GetPlayer()->GetCulture()->GetInfluenceLevel(eOtherPlayer) < INFLUENCE_LEVEL_INFLUENTIAL)
				{
					iItemValue *= 50;
					iItemValue /= 100;
				}
			}
		}
#endif
#if !defined(MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS)
		// Do we think he's going for culture victory?
		AIGrandStrategyTypes eCultureStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
		if (eCultureStrategy != NO_AIGRANDSTRATEGY && GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eOtherPlayer) == eCultureStrategy)
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eOtherPlayer);

			// If he has tourism and he's not influential on us yet, resist!
			if (kOtherPlayer.GetCulture()->GetTourism() > 0 && kOtherPlayer.GetCulture()->GetInfluenceOn(GetPlayer()->GetID()) < INFLUENCE_LEVEL_INFLUENTIAL)
			{
				iItemValue *= 500;
				iItemValue /= 100;
			}
		}
#if defined(MOD_BALANCE_CORE_DEALS)
		if(MOD_BALANCE_CORE_DEALS)
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eOtherPlayer);
			if (kOtherPlayer.GetCulture()->GetTourism() > 0 && (kOtherPlayer.GetCulture()->GetInfluenceOn(GetPlayer()->GetID()) > INFLUENCE_LEVEL_FAMILIAR) && (kOtherPlayer.GetCulture()->GetInfluenceOn(GetPlayer()->GetID()) < INFLUENCE_LEVEL_INFLUENTIAL))
			{
				if(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer) <= MAJOR_CIV_OPINION_NEUTRAL)
				{
					if(GetPlayer()->GetDiplomacyAI()->GetVictoryBlockLevel(eOtherPlayer) >= BLOCK_LEVEL_STRONG || GetPlayer()->GetDiplomacyAI()->GetVictoryDisputeLevel(eOtherPlayer) >= DISPUTE_LEVEL_STRONG)
					{
						iItemValue = 100000;
					}
				}
			}
		}
#endif
#endif
	}
	// Other guy giving me Open Borders
	else
	{
#if defined(MOD_BALANCE_CORE)
		if (!GetPlayer()->GetDiplomacyAI()->IsWantsOpenBordersWithPlayer(eOtherPlayer))
		{
			return INT_MAX;
		}

		// Approach is important
		switch(eApproach)
		{
		//If we don't like him, don't take his borders. It's a trap!
		case MAJOR_CIV_APPROACH_HOSTILE:
		case MAJOR_CIV_APPROACH_DECEPTIVE:
		case MAJOR_CIV_APPROACH_GUARDED:
			return INT_MAX;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue += 200;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue += 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue += 50;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Open Borders valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue += 100;
			break;
		}
		// Proximity is very important
		switch(GetPlayer()->GetProximityToPlayer(eOtherPlayer))
		{
		case PLAYER_PROXIMITY_DISTANT:
			iItemValue *= 70;
			iItemValue /= 100;
			break;
		case PLAYER_PROXIMITY_FAR:
			iItemValue *= 80;
			iItemValue /= 100;
			break;
		case PLAYER_PROXIMITY_CLOSE:
			iItemValue *= 90;
			iItemValue /= 100;
			break;
		case PLAYER_PROXIMITY_NEIGHBORS:
			iItemValue *= 110;
			iItemValue /= 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Proximity for Open Borders valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue = 0;
			break;
		}
#else
		// Proximity is very important
		switch(GetPlayer()->GetProximityToPlayer(eOtherPlayer))
		{
		case PLAYER_PROXIMITY_DISTANT:
			iItemValue = 5;
			break;
		case PLAYER_PROXIMITY_FAR:
			iItemValue = 10;
			break;
		case PLAYER_PROXIMITY_CLOSE:
			iItemValue = 15;
			break;
		case PLAYER_PROXIMITY_NEIGHBORS:
			iItemValue = 30;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Proximity for Open Borders valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue = 0;
			break;
		}
#endif
		// Reduce value by half if the other guy only has a single City
		if(GET_PLAYER(eOtherPlayer).getNumCities() == 1)
		{
			iItemValue *= 50;
			iItemValue /= 100;
		}
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
		if(GetPlayer()->IsCramped() && GET_PLAYER(eOtherPlayer).getNumCities() > GetPlayer()->getNumCities())
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		if(GetPlayer()->GetDiplomacyAI()->GetNeighborOpinion(eOtherPlayer) == MAJOR_CIV_OPINION_ENEMY)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		if(GetPlayer()->GetDiplomacyAI()->MusteringForNeighborAttack(eOtherPlayer))
		{
			iItemValue *= 150;
			iItemValue /= 100;
		}
		//We need to explore?
		EconomicAIStrategyTypes eNeedRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON");
		EconomicAIStrategyTypes eNavalRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON_SEA");
		if(eNeedRecon != NO_ECONOMICAISTRATEGY && GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNeedRecon))
		{
			iItemValue *= 115;
			iItemValue /= 100;
		}
		if(eNavalRecon != NO_ECONOMICAISTRATEGY && GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNavalRecon))
		{
			iItemValue *= 115;
			iItemValue /= 100;
		}
#endif
		// Boost value greatly if we are going for a culture win
		// If going for culture win always want open borders against civs we need influence on
#if !defined(MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS)
		AIGrandStrategyTypes eCultureStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
		if (eCultureStrategy != NO_AIGRANDSTRATEGY && GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eCultureStrategy && GetPlayer()->GetCulture()->GetTourism() > 0 )
		{
			// The civ we need influence on the most should ALWAYS be included
			if (GetPlayer()->GetCulture()->GetCivLowestInfluence(false /*bCheckOpenBorders*/) == eOtherPlayer)
			{
				iItemValue *= 1000;
				iItemValue /= 100;
			}

			// If have influence over half the civs, want OB with the other half
			else if (GetPlayer()->GetCulture()->GetNumCivsToBeInfluentialOn() <= GetPlayer()->GetCulture()->GetNumCivsInfluentialOn())
			{
				if (GetPlayer()->GetCulture()->GetInfluenceLevel(eOtherPlayer) < INFLUENCE_LEVEL_INFLUENTIAL)
				{
					iItemValue *= 500;
					iItemValue /= 100;
				}
			}

			else if (GetPlayer()->GetProximityToPlayer(eOtherPlayer) == PLAYER_PROXIMITY_NEIGHBORS)
			{
				// If we're cramped then we want OB more with our neighbors
				if(GetPlayer()->IsCramped())
				{
					iItemValue *= 300;
					iItemValue /= 100;
				}
			}
		}
#endif
#if defined(MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS)
		// Do we think he's going for culture victory? If we're contesting this, don't take his open borders!
		CvPlayer &kOtherPlayer = GET_PLAYER(eOtherPlayer);
		// Opinion also matters
		if(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false) < MAJOR_CIV_APPROACH_FRIENDLY)
		{
			if (kOtherPlayer.GetCulture()->GetTourism() > 0 && (kOtherPlayer.GetCulture()->GetInfluenceOn(GetPlayer()->GetID()) < INFLUENCE_LEVEL_INFLUENTIAL))
			{
				if(GetPlayer()->GetDiplomacyAI()->GetVictoryBlockLevel(eOtherPlayer) >= BLOCK_LEVEL_STRONG || GetPlayer()->GetDiplomacyAI()->GetVictoryDisputeLevel(eOtherPlayer) >= DISPUTE_LEVEL_STRONG)
				{
					return INT_MAX;
				}

				// If he has influence over half the civs, want to block OB with the other half
				if (kOtherPlayer.GetCulture()->GetNumCivsToBeInfluentialOn() <= kOtherPlayer.GetCulture()->GetNumCivsInfluentialOn())
				{
					return INT_MAX;
				}
			}
		}
#endif
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetOpenBordersValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		if (iReverseValue == INT_MAX)
			//no deal, can't agree on a value
			iItemValue = INT_MAX;
		else
			iItemValue = (iItemValue + iReverseValue)/2;
	}

	return iItemValue;
}

/// How much is a Defensive Pact worth?
int CvDealAI::GetDefensivePactValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Defensive Pact with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 500;

#if defined(MOD_BALANCE_CORE_DEALS)
	if(GetPlayer()->GetDiplomacyAI()->IsMusteringForAttack(eOtherPlayer))
	{
		return INT_MAX;
	}
	if(!GET_PLAYER(eOtherPlayer).isHuman() && !GetPlayer()->GetDiplomacyAI()->IsWantsDefensivePactWithPlayer(eOtherPlayer))
	{
		return INT_MAX;
	}
	if(GET_PLAYER(eOtherPlayer).isHuman())
	{
		if (!GetPlayer()->GetDiplomacyAI()->IsGoodChoiceForDefensivePact(eOtherPlayer))
			return INT_MAX;
	}
#endif

	// What is a Defensive Pact with eOtherPlayer worth to US?
	if(!bFromMe)
	{
#if defined(MOD_BALANCE_CORE_DEALS)
		if(MOD_BALANCE_CORE_DEALS)
		{
			iItemValue *= 35;
			iItemValue /= max(1, GetPlayer()->GetDiplomacyAI()->GetDefensivePactValue(eOtherPlayer));
		}
#endif
	}
	// How much do we value giving away a Defensive Pact?
	else
	{
#if defined(MOD_BALANCE_CORE_DEALS)
		if(MOD_BALANCE_CORE_DEALS)
		{
			iItemValue *= GetPlayer()->GetDiplomacyAI()->GetDefensivePactValue(eOtherPlayer);
			iItemValue /= 40;
		}
#else
		// Opinion also matters
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iItemValue = 100;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iItemValue = 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iItemValue = 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
		case MAJOR_CIV_OPINION_COMPETITOR:
		case MAJOR_CIV_OPINION_ENEMY:
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Defensive Pact valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue = 100;
			break;
		}
#endif
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetDefensivePactValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		if (iReverseValue == INT_MAX)
			//no deal, can't agree on a value
			iItemValue = INT_MAX;
		else
			iItemValue = (iItemValue + iReverseValue)/2;
	}

	return iItemValue;
}

/// How much is a Research Agreement worth?
int CvDealAI::GetResearchAgreementValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Research Agreement with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 100;

	if(bFromMe)
	{
		// if they are ahead of me in tech by one or more eras ratchet up the value since they are more likely to get a good tech than I am
		EraTypes eMyEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();
		EraTypes eTheirEra = GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetCurrentEra();

		int iAdditionalValue = iItemValue * max(0,(int)(eTheirEra-eMyEra));
		iItemValue += iAdditionalValue;

		// Approach is important
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 1000;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 80;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Research Agreement valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;

	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetResearchAgreementValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		if (iReverseValue == INT_MAX)
			//no deal, can't agree on a value
			iItemValue = INT_MAX;
		else
			iItemValue = (iItemValue + iReverseValue)/2;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES)
	if(MOD_DIPLOMACY_CITYSTATES)
	{
		if(GetPlayer()->GetRAToVotes() > 0)
		{
			iItemValue *= 5;
		}
	}
#endif

	return iItemValue;
}

/// How much is a Trade Agreement worth?
int CvDealAI::GetTradeAgreementValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Trade Agreement with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 100;

	if(bFromMe)
	{
		// Approach is important
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 250;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 130;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 80;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 110;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Research Agreement valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetTradeAgreementValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		if (iReverseValue == INT_MAX)
			//no deal, can't agree on a value
			iItemValue = INT_MAX;
		else
			iItemValue = (iItemValue + iReverseValue)/2;
	}

	return iItemValue;
}

/// How much is a Peace Treaty worth?
int CvDealAI::GetPeaceTreatyValue(PlayerTypes eOtherPlayer)
{
	DEBUG_VARIABLE(eOtherPlayer);

	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Peace Treaty with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
#if defined(MOD_BALANCE_CORE)
	return 20;
#else
	return 0;
#endif
}

/// What is the value of peace with eWithTeam? NOTE: This deal item should be disabled if eWithTeam doesn't want to go to peace
int CvDealAI::GetThirdPartyPeaceValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Third Party Peace with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

#if defined(MOD_BALANCE_CORE)
	int iItemValue = 100; //just some base value
#else
	int iItemValue = 0;
#endif

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();

	if(eWithTeam == NO_TEAM)
	{
		return INT_MAX;
	}

	PlayerTypes eWithPlayer = GET_TEAM(eWithTeam).getLeaderID();
	if(eWithPlayer == NO_PLAYER)
	{
		return INT_MAX;
	}

	bool bMinor = false;

	// Minor
	if(GET_PLAYER(eWithPlayer).isMinorCiv())
	{
		// if we're at war with the opponent, then this must be a peace deal. In this case we should evaluate minor civ peace deals as zero
		if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()))
		{
			PlayerTypes eMinorAlly = GET_PLAYER(eWithPlayer).GetMinorCivAI()->GetAlly();
			// if they are allied with the city state or we are allied with the city state
			if (eMinorAlly == eOtherPlayer || eMinorAlly == m_pPlayer->GetID())
			{
				return 0;
			}
		}

		bMinor = true;
	}

#if defined(MOD_BALANCE_CORE)
	// if we're at war with the opponent, then this must be a peace deal. In this case we should evaluate vassal civ peace deals as zero
	if (MOD_DIPLOMACY_CIV4_FEATURES && (GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsVassal(GET_PLAYER(eOtherPlayer).getTeam()) || GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsVassal(GetPlayer()->getTeam())))
	{
		if(GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()))
		{
			return 0;
		}
	}

	// Don't evaluate barbarians
	if(GET_PLAYER(eWithPlayer).isBarbarian())
	{
		return INT_MAX;
	}
	//not alive?
	if(!GET_PLAYER(eOtherPlayer).isAlive())
	{
		return INT_MAX;
	}
	//same team as asked?
	if(GET_PLAYER(eWithPlayer).getTeam() == GET_PLAYER(eOtherPlayer).getTeam())
	{
		return INT_MAX;
	}
	// same team as asker?
	if(GET_PLAYER(eWithPlayer).getTeam() == m_pPlayer->getTeam())
	{
		return INT_MAX;
	}
	// asker has met?
	if(!GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).isHasMet(m_pPlayer->getTeam()))
	{
		return INT_MAX;
	}
	// asked has met?
	if(!GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).isHasMet(GET_PLAYER(eOtherPlayer).getTeam()))
	{
		return INT_MAX;
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	//Asking about a vassal? Abort!
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsVassalOfSomeone())
	{
		return INT_MAX;
	}
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsVassalOfSomeone())
	{
		return INT_MAX;
	}
#endif
	//Things blocking their peace?
	if(!bFromMe)
	{
		//Minor civ? Permawar?
		if(GET_PLAYER(eWithPlayer).isMinorCiv() && (GET_PLAYER(eWithPlayer).GetMinorCivAI()->IsPeaceBlocked(GET_PLAYER(eOtherPlayer).getTeam()) || GET_PLAYER(eWithPlayer).GetMinorCivAI()->IsPermanentWar(GET_PLAYER(eOtherPlayer).getTeam())))
		{
			return INT_MAX;
		}
		//Can they make peace?
		if (!GET_PLAYER(eWithPlayer).isMinorCiv() && !GET_TEAM(eWithTeam).canChangeWarPeace(GET_PLAYER(eOtherPlayer).getTeam()))
		{
			return INT_MAX;
		}
	}
	if(bFromMe)
	{
		//Minor civ? Permawar?
		if(GET_PLAYER(eWithPlayer).isMinorCiv() && (GET_PLAYER(eWithPlayer).GetMinorCivAI()->IsPeaceBlocked(GetPlayer()->getTeam()) || GET_PLAYER(eWithPlayer).GetMinorCivAI()->IsPermanentWar(GetPlayer()->getTeam())))
		{
			return INT_MAX;
		}
		//Can they make peace?
		if (!GET_PLAYER(eWithPlayer).isMinorCiv() && !GET_TEAM(eWithTeam).canChangeWarPeace(GetPlayer()->getTeam()))
		{
			return INT_MAX;
		}
	}
#endif

	EraTypes eOurEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();

	// From me
	if(bFromMe)
	{
		//Not at war?
		if (!GET_TEAM(m_pPlayer->getTeam()).isAtWar(eWithTeam))
		{
			return INT_MAX;
		}

		//Our war score with the player
		int iWarScore = pDiploAI->GetWarScore(eWithPlayer);

		iItemValue += (iWarScore * 15);

		// Add 50 gold per era
		int iExtraCost = eOurEra * 50;
		iItemValue += iExtraCost;

		// Minors
		if(bMinor)
		{
			MinorCivApproachTypes eMinorApproachTowardsWarPlayer = pDiploAI->GetMinorCivApproach(eWithPlayer);
			if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_CONQUEST)
			{
				return INT_MAX;
			}
			else if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_BULLY)
			{
				iItemValue += 200;
			}
		}
		// Majors
		else
		{
			MajorCivApproachTypes eWarMajorCivApproach = pDiploAI->GetMajorCivApproach(eWithPlayer, true);
			// Modify for our feelings towards the player we're at war with
			if(eWarMajorCivApproach <= MAJOR_CIV_APPROACH_HOSTILE)
			{
				iItemValue *= 500;
				iItemValue /= 100;
			}
			else if(eWarMajorCivApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
			{
				iItemValue *= 250;
				iItemValue /= 100;
			}
			else if(eWarMajorCivApproach == MAJOR_CIV_APPROACH_GUARDED)
			{
				iItemValue *= 150;
				iItemValue /= 100;
			}
		}

		MajorCivApproachTypes eAskingMajorCivApproach = pDiploAI->GetMajorCivApproach(eOtherPlayer, true);

		// Modify for our feelings towards the asking player
		if(eAskingMajorCivApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
		else if(eAskingMajorCivApproach == MAJOR_CIV_APPROACH_NEUTRAL)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		else if(eAskingMajorCivApproach == MAJOR_CIV_APPROACH_AFRAID)
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
		else if(eAskingMajorCivApproach == MAJOR_CIV_APPROACH_GUARDED)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		else if(eAskingMajorCivApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		{
			iItemValue *= 150;
			iItemValue /= 100;
		}
		else if(eAskingMajorCivApproach >= MAJOR_CIV_APPROACH_HOSTILE)
		{
			iItemValue *= 600;
			iItemValue /= 100;
		}
	}
	// From them
	else
	{
#if defined(MOD_BALANCE_CORE)
		//Not at war?
		if (!GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).isAtWar(eWithTeam))
		{
			return INT_MAX;
		}

		//Their war score with the player
		int iTheirWarScore = GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetWarScore(eWithPlayer);

		iItemValue += (iTheirWarScore * 5);

		// Add 25 gold per era
		int iExtraCost = eOurEra * 25;
		iItemValue += iExtraCost;

		if(!GET_PLAYER(eWithPlayer).isMinorCiv())
		{
			MajorCivApproachTypes eWarMajorCivApproach = pDiploAI->GetMajorCivApproach(eWithPlayer, true);

			// Modify for our feelings towards the player we're at war with
			if(eWarMajorCivApproach == MAJOR_CIV_APPROACH_FRIENDLY)
			{
				iItemValue *= 200;
				iItemValue /= 100;
			}
			else if(eWarMajorCivApproach == MAJOR_CIV_APPROACH_NEUTRAL)
			{
				iItemValue *= 50;
				iItemValue /= 100;
			}
			else if(eWarMajorCivApproach == MAJOR_CIV_APPROACH_AFRAID)
			{
				iItemValue *= 75;
				iItemValue /= 100;
			}
			else
			{
				return INT_MAX;
			}
		}
		else if(GET_PLAYER(eWithPlayer).isMinorCiv())
		{
			//Way less value.
			iItemValue /= 50;

			MinorCivApproachTypes eMinorApproachTowardsWarPlayer = pDiploAI->GetMinorCivApproach(eWithPlayer);
			if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_FRIENDLY)
			{
				iItemValue *= 150;
				iItemValue /= 100;
			}
			else if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_PROTECTIVE)
			{
				iItemValue *= 300;
				iItemValue /= 100;
			}
			else
			{
				return INT_MAX;
			}
		}
		
		MajorCivApproachTypes eAskingMajorCivApproach = pDiploAI->GetMajorCivApproach(eOtherPlayer, true);

		// Modify for our feelings towards the asking player
		if(eAskingMajorCivApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		{
			iItemValue *= 200;
			iItemValue /= 100;
		}
		else if(eAskingMajorCivApproach == MAJOR_CIV_APPROACH_NEUTRAL)
		{
			return INT_MAX;
		}
		else if(eAskingMajorCivApproach == MAJOR_CIV_APPROACH_AFRAID)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		else if(eAskingMajorCivApproach == MAJOR_CIV_APPROACH_GUARDED)
		{
			return INT_MAX;
		}
		else if (eAskingMajorCivApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		else if (eAskingMajorCivApproach <= MAJOR_CIV_APPROACH_HOSTILE)
		{
			return INT_MAX;
		}
	}
#else
		iItemValue = -10000;
#endif

	return iItemValue;
}

/// What is the value of war with eWithPlayer?
int CvDealAI::GetThirdPartyWarValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Third Party War with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

#if defined(MOD_BALANCE_CORE)
	int iItemValue = 300; //just some base value
#else
	int iItemValue = 0;
#endif

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();

	// How much does this AI like to go to war? If it's a 6 or less, never accept
	int iWarApproachWeight = pDiploAI->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_WAR);
	if(bFromMe && iWarApproachWeight < 6)
		return INT_MAX;

	if(eWithTeam == NO_TEAM)
	{
		return INT_MAX;
	}

	PlayerTypes eWithPlayer = GET_TEAM(eWithTeam).getLeaderID();
	if(eWithPlayer == NO_PLAYER)
	{
		return INT_MAX;
	}

	// Don't evaluate barbarians
	if(GET_PLAYER(eWithPlayer).isBarbarian())
	{
		return INT_MAX;
	}
	//Minor Civ? Only if not allies. Applies to both parties.
	if(GET_PLAYER(eWithPlayer).isMinorCiv() && (GET_PLAYER(eWithPlayer).GetMinorCivAI()->IsAllies(GetPlayer()->GetID()) || GET_PLAYER(eWithPlayer).GetMinorCivAI()->IsAllies(eOtherPlayer)))
	{
		return INT_MAX;
	}
	//same team as asked?
	if(GET_PLAYER(eWithPlayer).getTeam() == GET_PLAYER(eOtherPlayer).getTeam())
	{
		return INT_MAX;
	}
	// same team as asker?
	if(GET_PLAYER(eWithPlayer).getTeam() == m_pPlayer->getTeam())
	{
		return INT_MAX;
	}
	// asker has met?
	if(!GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).isHasMet(m_pPlayer->getTeam()))
	{
		return INT_MAX;
	}
	// asked has met?
	if(!GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).isHasMet(GET_PLAYER(eOtherPlayer).getTeam()))
	{
		return INT_MAX;
	}
	//defensive pact with us?
	if(GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsHasDefensivePact(m_pPlayer->getTeam()))
	{
		return INT_MAX;
	}
	//defensive pact with asked?
	if(GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsHasDefensivePact(GET_PLAYER(eOtherPlayer).getTeam()))
	{
		return INT_MAX;
	}
	//Are we in a DOF with the player? Don't do it!
	if(GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eWithPlayer) || GET_PLAYER(eWithPlayer).GetDiplomacyAI()->IsDoFAccepted(GetPlayer()->GetID()))
	{
		return INT_MAX;
	}
	//Are the asked in a DOF with the player? Don't do it!
	if(GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsDoFAccepted(eWithPlayer) || GET_PLAYER(eWithPlayer).GetDiplomacyAI()->IsDoFAccepted(eOtherPlayer))
	{
		return INT_MAX;
	}
	//Can't see any of their cities?
	if(bFromMe)
	{
		CvCity* pLoopCity;
		int iCityLoop;
		bool bSeen = false;
		for(pLoopCity = GET_PLAYER(eWithPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eWithPlayer).nextCity(&iCityLoop))
		{
			if(pLoopCity != NULL && pLoopCity->isRevealed(m_pPlayer->getTeam(), false))
			{
				bSeen = true;
				break;
			}
		}
		if(!bSeen)
		{
			return INT_MAX;
		}
	}
	//Can't see any of their cities?
	if(!bFromMe)
	{
		CvCity* pLoopCity;
		int iCityLoop;
		bool bSeen = false;
		for(pLoopCity = GET_PLAYER(eWithPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eWithPlayer).nextCity(&iCityLoop))
		{
			if(pLoopCity != NULL && pLoopCity->isRevealed(GET_PLAYER(eOtherPlayer).getTeam(), false))
			{
				bSeen = true;
				break;
			}
		}
		if(!bSeen)
		{
			return INT_MAX;
		}
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	//Asking about a vassal? Abort!
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsVassalOfSomeone())
	{
		return INT_MAX;
	}
	//No vassal/master war requests, please.
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsVassal(GET_PLAYER(eOtherPlayer).getTeam()))
	{
		return INT_MAX;
	}
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsVassal(GET_PLAYER(GetPlayer()->GetID()).getTeam()))
	{
		return INT_MAX;
	}
#endif

	// don't accept third party wars if we just signed a defensive pact.
	if (pDiploAI->GetNumDefensePacts() > 0)
	{
		CvGameDeals& kGameDeals = GC.getGame().GetGameDeals();
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eDPPlayer = (PlayerTypes)iPlayerLoop;
			TeamTypes eTeam = GET_PLAYER(eDPPlayer).getTeam();
			if (eDPPlayer != NO_PLAYER && GET_TEAM(GET_PLAYER(eDPPlayer).getTeam()).getDefensivePactCount(GetPlayer()->getTeam()) > 0)
			{
				int iNumDeals = kGameDeals.GetNumCurrentDealsWithPlayer(GetPlayer()->GetID(), eDPPlayer);
				for (int iDeal = 0; iDeal < iNumDeals; iDeal++)
				{
					CvDeal* pCurrentDeal = kGameDeals.GetCurrentDealWithPlayer(m_pPlayer->GetID(), eDPPlayer, iDeal);
					int iStart = pCurrentDeal->GetStartTurn();
					if (pCurrentDeal != NULL && ((iStart + 10) > GC.getGame().getGameTurn()))
					{
						return INT_MAX;
					}
				}
			}
			//we have a defensive pact with a player?
			if (eTeam != NO_TEAM && GET_TEAM(eTeam).IsHasDefensivePact(m_pPlayer->getTeam()))
			{
				//and our target has a defensive pact with this same player? Abort!
				if (GET_TEAM(eTeam).IsHasDefensivePact(eWithTeam))
				{
					return INT_MAX;
				}
			}
		}
	}

#if defined(MOD_BALANCE_CORE_MILITARY)
	//Already moving on asker?
	if(bFromMe && pDiploAI->IsMusteringForAttack(eOtherPlayer))
	{
		return INT_MAX;
	}
	if(!bFromMe && pDiploAI->IsMusteringForAttack(eOtherPlayer))
	{
		iItemValue += 500;
	}
	if(bFromMe && GetPlayer()->GetMilitaryAI()->GetSneakAttackOperation(eOtherPlayer))
	{
		return INT_MAX;
	}
	if(!bFromMe && GetPlayer()->GetMilitaryAI()->GetSneakAttackOperation(eOtherPlayer))
	{
		iItemValue += 500;
	}

	//Already moving on target?
	if(bFromMe && pDiploAI->IsMusteringForAttack(eWithPlayer))
	{
		iItemValue += 250;
	}
	if(!bFromMe && pDiploAI->IsMusteringForAttack(eWithPlayer))
	{
		iItemValue += 300;
	}
	if(bFromMe && GetPlayer()->GetMilitaryAI()->GetSneakAttackOperation(eWithPlayer))
	{
		iItemValue += 250;
	}
	if(!bFromMe && GetPlayer()->GetMilitaryAI()->GetSneakAttackOperation(eWithPlayer))
	{
		iItemValue += 300;
	}
#endif
	WarProjectionTypes eWarProjection = pDiploAI->GetWarProjection(eWithPlayer);

	EraTypes eOurEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();

	MajorCivApproachTypes eApproachTowardsAskingPlayer = pDiploAI->GetMajorCivApproach(eOtherPlayer, false);
	MinorCivApproachTypes eMinorApproachTowardsWarPlayer = NO_MINOR_CIV_APPROACH;
	MajorCivApproachTypes eMajorApproachTowardsWarPlayer = NO_MAJOR_CIV_APPROACH;

	bool bMinor = false;

	// Minor
	if(GET_PLAYER(eWithPlayer).isMinorCiv())
	{
		bMinor = true;
		eMinorApproachTowardsWarPlayer = pDiploAI->GetMinorCivApproach(eWithPlayer);
	}
	// Major
#if defined(MOD_BALANCE_CORE_MILITARY)
	else
	{
		eMajorApproachTowardsWarPlayer = pDiploAI->GetMajorCivApproach(eWithPlayer, false);
	}
#else
	else
		eOpinionTowardsWarPlayer = pDiploAI->GetMajorCivOpinion(eWithPlayer);
#endif
	// From me
	if(bFromMe)
	{
		//Are we already at war with the team? Don't do it!
		if(GET_TEAM(GetPlayer()->getTeam()).isAtWar(eWithTeam))
		{
			return INT_MAX;
		}
		//Can we declare war?
		if(!GET_TEAM(GetPlayer()->getTeam()).canDeclareWar(GET_PLAYER(eWithPlayer).getTeam()))
		{
			return INT_MAX;
		}
		if (eApproachTowardsAskingPlayer < MAJOR_CIV_APPROACH_AFRAID && pDiploAI->GetBiggestCompetitor() != eWithPlayer)
		{
			return INT_MAX;
		}
		if(eApproachTowardsAskingPlayer != MAJOR_CIV_APPROACH_FRIENDLY)
		{
			if(eWarProjection == WAR_PROJECTION_VERY_GOOD)
				iItemValue += 100;
			else if(eWarProjection == WAR_PROJECTION_GOOD)
				iItemValue += 200;
			else if (eWarProjection <= WAR_PROJECTION_UNKNOWN && pDiploAI->GetBiggestCompetitor() != eWithPlayer)
				return INT_MAX;			
		}
		else
		{
			if(eWarProjection == WAR_PROJECTION_VERY_GOOD)
				iItemValue += 100;
			else if(eWarProjection == WAR_PROJECTION_GOOD)
				iItemValue += 200;
			else if(eWarProjection >= WAR_PROJECTION_STALEMATE)
				iItemValue += 300;
			else if (eWarProjection < WAR_PROJECTION_STALEMATE && pDiploAI->GetBiggestCompetitor() != eWithPlayer)
				return INT_MAX;
		}
		
		// Add 25 gold per era
		int iExtraCost = eOurEra * 25;
		iItemValue += iExtraCost;

		// Minor
		if(bMinor)
		{
			if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_FRIENDLY)
				iItemValue = INT_MAX;
			else if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_PROTECTIVE)
				iItemValue = INT_MAX;
		}
		// Major
		else
		{
			// Modify for our feelings towards the player we're would go to war with
			if (eMajorApproachTowardsWarPlayer == MAJOR_CIV_APPROACH_WAR)
			{
				iItemValue *= 75;
				iItemValue /= 100;
			}
			else if (eMajorApproachTowardsWarPlayer <= MAJOR_CIV_APPROACH_HOSTILE && eApproachTowardsAskingPlayer >= MAJOR_CIV_APPROACH_AFRAID)
			{
				iItemValue *= 90;
				iItemValue /= 100;
			}
			else if (eMajorApproachTowardsWarPlayer <= MAJOR_CIV_APPROACH_GUARDED && eApproachTowardsAskingPlayer == MAJOR_CIV_APPROACH_FRIENDLY)
			{
				iItemValue *= 150;
				iItemValue /= 100;
			}
			else
			{
				return INT_MAX;
			}

			if (eWithPlayer == m_pPlayer->GetDiplomacyAI()->GetBiggestCompetitor())
				iItemValue /= 2;
		}
#if defined(MOD_BALANCE_CORE)
		//Already planning war? Less valuable.
		if(pDiploAI->GetWarGoal(eWithPlayer) == WAR_GOAL_CONQUEST)
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
#endif

		if(pDiploAI->IsMusteringForAttack(eWithPlayer))
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
		if(GetPlayer()->GetMilitaryAI()->GetSneakAttackOperation(eWithPlayer))
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}

		// Modify for our feelings towards the asking player
		if(eApproachTowardsAskingPlayer == MAJOR_CIV_APPROACH_FRIENDLY)
		{
			iItemValue *= 90;
			iItemValue /= 100;
		}
		else if(eApproachTowardsAskingPlayer == MAJOR_CIV_APPROACH_NEUTRAL && eMajorApproachTowardsWarPlayer <= MAJOR_CIV_APPROACH_HOSTILE)
		{
			iItemValue *= 150;
			iItemValue /= 100;
		}
		else if(eApproachTowardsAskingPlayer == MAJOR_CIV_APPROACH_AFRAID)
		{
			iItemValue *= 90;
			iItemValue /= 100;
		}
		else if (pDiploAI->GetBiggestCompetitor() != eWithPlayer)
		{
			return INT_MAX;
		}

		if(!bMinor)
		{
			if(GetPlayer()->IsAtWar() && eWithPlayer != NO_PLAYER)
			{
				// find any other wars we have going
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					PlayerTypes eWarPlayer = (PlayerTypes)iPlayerLoop;
					if(eWarPlayer != NO_PLAYER && eWarPlayer != eOtherPlayer && eWarPlayer != eWithPlayer && eWarPlayer != GetPlayer()->GetID() && !GET_PLAYER(eWarPlayer).isMinorCiv())
					{
						if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eWarPlayer).getTeam()))
						{
							WarStateTypes eWarState = pDiploAI->GetWarState(eWarPlayer);
							if (eWarState <= WAR_STATE_DEFENSIVE)
							{
								return INT_MAX;
							}
							else
							{
								iItemValue *= 3;
							}
						}
					}
				}
			}

			bool bTargetLand = GetPlayer()->GetMilitaryAI()->HaveCachedAttackTarget(eWithPlayer, AI_OPERATION_CITY_SNEAK_ATTACK);
			bool bTargetSeaPure = GetPlayer()->GetMilitaryAI()->HaveCachedAttackTarget(eWithPlayer, AI_OPERATION_NAVAL_ONLY_CITY_ATTACK);
			bool bTargetSea = GetPlayer()->GetMilitaryAI()->HaveCachedAttackTarget(eWithPlayer, AI_OPERATION_NAVAL_INVASION_SNEAKY);
			if(!bTargetLand && !bTargetSeaPure && !bTargetSea)
			{
				CvMilitaryTarget target = GetPlayer()->GetMilitaryAI()->FindBestAttackTargetCached(AI_OPERATION_CITY_SNEAK_ATTACK, eWithPlayer);
				if(target.m_pTargetCity != NULL && target.m_pMusterCity != NULL)
				{
					if(!target.m_bAttackBySea)
					{
						bTargetLand = true;
					}
					else
					{
						bTargetSea = true;
					}
				}
			}
			//No target? Abort!
			if(!bTargetLand && !bTargetSeaPure && !bTargetSea)
			{
				return INT_MAX;
			}
			else if(!GetPlayer()->CanCrossOcean())
			{
				switch(GetPlayer()->GetProximityToPlayer(eWithPlayer))
				{
					case PLAYER_PROXIMITY_DISTANT:
						iItemValue *= 10;
					case PLAYER_PROXIMITY_FAR:
						iItemValue *= 25;
						break;
					case PLAYER_PROXIMITY_CLOSE:
						iItemValue *= 150;
						break;
					case PLAYER_PROXIMITY_NEIGHBORS:
						iItemValue *= 300;
						break;
					default:
						CvAssertMsg(false, "DEAL_AI: Player has no valid proximity for 3rd party deal.");
						iItemValue *= 100;
						break;
				}
				iItemValue /= 100;
			}
			else
			{
				switch(GetPlayer()->GetProximityToPlayer(eWithPlayer))
				{
					case PLAYER_PROXIMITY_DISTANT:
						iItemValue *= 10;
					case PLAYER_PROXIMITY_FAR:
						iItemValue *= 25;
						break;
					case PLAYER_PROXIMITY_CLOSE:
						iItemValue *= 200;
						break;
					case PLAYER_PROXIMITY_NEIGHBORS:
						iItemValue *= 300;
						break;
					default:
						CvAssertMsg(false, "DEAL_AI: Player has no valid proximity for 3rd party deal.");
						iItemValue *= 100;
						break;
				}
				iItemValue /= 100;
			}
		}
	}

	// From them
	else
	{
#if defined(MOD_BALANCE_CORE)
		//Are they already at war with the team? Don't do it!
		if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).isAtWar(eWithTeam))
		{
			return INT_MAX;
		}
		//Can they declare war?
		if(!GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).canDeclareWar(GET_PLAYER(eWithPlayer).getTeam()))
		{
			return INT_MAX;
		}
		//Don't care if we're not upset with the third party.
		if(GET_PLAYER(eWithPlayer).isMajorCiv())
		{
			//Are we already at war with the player? Let's lower our standards a bit.
			if(GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eWithPlayer).getTeam()))
			{
				if (eApproachTowardsAskingPlayer <= MAJOR_CIV_APPROACH_HOSTILE &&  pDiploAI->GetBiggestCompetitor() != eWithPlayer)
				{
					return INT_MAX;
				}
			}
			else if(eMajorApproachTowardsWarPlayer < MAJOR_CIV_APPROACH_DECEPTIVE)
			{
				if (eApproachTowardsAskingPlayer <= MAJOR_CIV_APPROACH_HOSTILE && pDiploAI->GetBiggestCompetitor() != eWithPlayer)
				{
					return INT_MAX;
				}
			}
			else
			{
				if (eApproachTowardsAskingPlayer < MAJOR_CIV_APPROACH_GUARDED && pDiploAI->GetBiggestCompetitor() != eWithPlayer)
				{
					return INT_MAX;
				}
			}
		}

		WarProjectionTypes eWarProjection2 = GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetWarProjection(eWithPlayer);

		if(eWarProjection2 >= WAR_PROJECTION_VERY_GOOD)
			iItemValue += 50;
		else if(eWarProjection2 >= WAR_PROJECTION_GOOD)
			iItemValue += 100;
		else if(eWarProjection2 == WAR_PROJECTION_UNKNOWN)
			iItemValue += 200;
		else if(eWarProjection2 == WAR_PROJECTION_STALEMATE)
			iItemValue += 300;
		else if(eWarProjection2 < WAR_PROJECTION_STALEMATE)
			return INT_MAX;

		// Add 25 gold per era
		int iExtraCost = eOurEra * 25;
		iItemValue += iExtraCost;

		// Minor
		if(bMinor)
		{
			//Only pay if we want to see it conquered - otherwise we don't care at all (and also, 1/4 the value, ok?)
			if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_CONQUEST)
			{
				iItemValue *= 25;
				iItemValue /= 100;
			}
			else
			{
				return INT_MAX;
			}
		}
		// Major
		else
		{
			if(!GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eWithPlayer).getTeam()))
			{
				// Modify for our feelings towards the player they would go to war with
				if(eMajorApproachTowardsWarPlayer == MAJOR_CIV_APPROACH_WAR)
				{
					iItemValue *= 200;
					iItemValue /= 100;
				}
				else if(eMajorApproachTowardsWarPlayer == MAJOR_CIV_APPROACH_HOSTILE)
				{
					iItemValue *= 150;
					iItemValue /= 100;
				}
				else if(eMajorApproachTowardsWarPlayer == MAJOR_CIV_APPROACH_DECEPTIVE)
				{
					iItemValue *= 175;
					iItemValue /= 100;
				}
				else if (eMajorApproachTowardsWarPlayer >= MAJOR_CIV_APPROACH_FRIENDLY && pDiploAI->GetBiggestCompetitor() != eWithPlayer)
				{
					return INT_MAX;
				}
			}
			else
			{
				iItemValue *= 250;
				iItemValue /= 100;
			}

			if (eWithPlayer == m_pPlayer->GetDiplomacyAI()->GetBiggestCompetitor())
				iItemValue *= 4;
		}
		if(eApproachTowardsAskingPlayer == MAJOR_CIV_APPROACH_AFRAID)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		else if(eApproachTowardsAskingPlayer == MAJOR_CIV_APPROACH_NEUTRAL)
		{
			iItemValue *= 90;
			iItemValue /= 100;
		}
		else if(eApproachTowardsAskingPlayer == MAJOR_CIV_APPROACH_FRIENDLY)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}

		if(!bMinor)
		{
			//We warring against this player already? Let's dogpile 'em.
			if(GET_TEAM(GetPlayer()->getTeam()).isAtWar(eWithTeam))
			{
				WarStateTypes eWarState = pDiploAI->GetWarState(eWithPlayer);
				if(eWarState < WAR_STATE_STALEMATE)
				{
					iItemValue *= 3;
				}
				else
				{
					iItemValue *= 2;
				}
			}
			//Not a human? Let's see if he has a valid target...if not, don't accept!
			if(!GET_PLAYER(eOtherPlayer).isHuman())
			{
				bool bTargetLand = GET_PLAYER(eOtherPlayer).GetMilitaryAI()->HaveCachedAttackTarget(eWithPlayer, AI_OPERATION_CITY_SNEAK_ATTACK);
				bool bTargetSeaPure = GET_PLAYER(eOtherPlayer).GetMilitaryAI()->HaveCachedAttackTarget(eWithPlayer, AI_OPERATION_NAVAL_ONLY_CITY_ATTACK);
				bool bTargetSea = GET_PLAYER(eOtherPlayer).GetMilitaryAI()->HaveCachedAttackTarget(eWithPlayer, AI_OPERATION_NAVAL_INVASION_SNEAKY);
				if(!bTargetLand && !bTargetSeaPure && !bTargetSea)
				{
					CvMilitaryTarget target = GET_PLAYER(eOtherPlayer).GetMilitaryAI()->FindBestAttackTargetCached(AI_OPERATION_CITY_SNEAK_ATTACK, eWithPlayer);
					if(target.m_pTargetCity != NULL && target.m_pMusterCity != NULL)
					{
						if(!target.m_bAttackBySea)
						{
							bTargetLand = true;
						}
						else
						{
							bTargetSea = true;
						}
					}
				}
				//No target? Abort!
				if(!bTargetLand && !bTargetSeaPure && !bTargetSea)
				{
					return INT_MAX;
				}
				else if(!GET_PLAYER(eOtherPlayer).CanCrossOcean())
				{
					switch(GET_PLAYER(eOtherPlayer).GetProximityToPlayer(eWithPlayer))
					{
						case PLAYER_PROXIMITY_DISTANT:
						case PLAYER_PROXIMITY_FAR:
							iItemValue *= 5;
						case PLAYER_PROXIMITY_CLOSE:
							iItemValue *= 125;
							break;
						case PLAYER_PROXIMITY_NEIGHBORS:
							iItemValue *= 200;
							break;
						default:
							CvAssertMsg(false, "DEAL_AI: Player has no valid proximity for 3rd party deal.");
							iItemValue *= 100;
							break;
					}
					iItemValue /= 100;
				}
				else
				{
					switch(GET_PLAYER(eOtherPlayer).GetProximityToPlayer(eWithPlayer))
					{
						case PLAYER_PROXIMITY_DISTANT:
							iItemValue *= 5;
						case PLAYER_PROXIMITY_FAR:
							iItemValue *= 10;
							break;
						case PLAYER_PROXIMITY_CLOSE:
							iItemValue *= 125;
							break;
						case PLAYER_PROXIMITY_NEIGHBORS:
							iItemValue *= 225;
							break;
						default:
							CvAssertMsg(false, "DEAL_AI: Player has no valid proximity for 3rd party deal.");
							iItemValue *= 100;
							break;
					}
					iItemValue /= 100;
				}
			}
			//Humans?
			else
			{
				switch(GET_PLAYER(eOtherPlayer).GetProximityToPlayer(eWithPlayer))
				{
					case PLAYER_PROXIMITY_DISTANT:
						iItemValue *= 10;
						break;
					case PLAYER_PROXIMITY_FAR:
						iItemValue *= 15;
						break;
					case PLAYER_PROXIMITY_CLOSE:
						iItemValue *= 110;
						break;
					case PLAYER_PROXIMITY_NEIGHBORS:
						iItemValue *= 130;
						break;
					default:
						CvAssertMsg(false, "DEAL_AI: Player has no valid proximity for 3rd party deal.");
						iItemValue *= 100;
						break;
				}
				iItemValue /= 100;
			}
		}
#else
		// Minor
		if(bMinor)
			iItemValue = -100000;

		// Major
		else
		{
			// Modify for our feelings towards the player they would go to war with
			if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_UNFORGIVABLE)
				iItemValue = 200;
			else if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_ENEMY)
				iItemValue = 100;
			else
				iItemValue = -100000;
		}
#endif
	}

	return iItemValue;
}

/// What is the value of trading a vote commitment?
int CvDealAI::GetVoteCommitmentValue(bool bFromMe, PlayerTypes eOtherPlayer, int iProposalID, int iVoteChoice, int iNumVotes, bool bRepeal, bool bUseEvenValue)
{
	int iValue = 150;

	if(iNumVotes == 0)
		return INT_MAX;

	// Giving our votes to them - Higher value for voting on things we dislike
	if (bFromMe)
	{
#if defined(MOD_BALANCE_CORE_DEALS)
		//Alright, so vote logic. We're giving them votes, so let's only vote on things we like.
		//Also, make sure that whatever we vote on can't be screwed over. Ask for more if they have lots of votes.
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if(pLeague)
		{
			//They shouldn't ask us to vote on things that have to do with us personally.
			CvEnactProposal* pProposal = pLeague->GetEnactProposal(iProposalID);
			if (pProposal != NULL)
			{
				PlayerTypes eTargetPlayer = NO_PLAYER;
				ResolutionDecisionTypes eProposerDecision = pProposal->GetProposerDecision()->GetType();
				if (eProposerDecision == RESOLUTION_DECISION_ANY_MEMBER ||
					eProposerDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
					eProposerDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
				{
					eTargetPlayer = (PlayerTypes) pProposal->GetProposerDecision()->GetDecision();
					if(eTargetPlayer != NO_PLAYER && eTargetPlayer == GetPlayer()->GetID())
					{
						return INT_MAX;
					}
				}
				//Let's look real quick to see if this is the world leader vote. If so, don't give ANYTHING away if we can win.
				if(pProposal->GetEffects()->bDiplomaticVictory)
				{
					int iOurVotes = pLeague->CalculateStartingVotesForMember(GetPlayer()->GetID());
					int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
					if(iOurVotes >= iVotesNeededToWin)
					{
						return INT_MAX;
					}
					//We can't win? Well, our votes should be super-duper valuable, then!
					else
					{
						iValue *= 100;
					}
				}
			}
			int iVotesNeeded = 0;
			int iTheirVotes = pLeague->CalculateStartingVotesForMember(eOtherPlayer);

			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				if (GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && pLeague->IsMember(eLoopPlayer))
				{			
					iVotesNeeded += pLeague->CalculateStartingVotesForMember(eLoopPlayer);
				}
			}
			// Adjust based on LeagueAI
			CvLeagueAI::DesireLevels eDesire = GetPlayer()->GetLeagueAI()->EvaluateVoteForTrade(iProposalID, iVoteChoice, iNumVotes, bRepeal);
			switch(eDesire)
			{
				case CvLeagueAI::DESIRE_NEVER:
				case CvLeagueAI::DESIRE_STRONG_DISLIKE:
				case CvLeagueAI::DESIRE_DISLIKE:
					return INT_MAX;
				case CvLeagueAI::DESIRE_WEAK_DISLIKE:
					iValue += 400 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_NEUTRAL:
					iValue += 250 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_WEAK_LIKE:
				case CvLeagueAI::DESIRE_LIKE:
					iValue += 125 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_STRONG_LIKE:
					iValue += 50 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_ALWAYS:
					iValue += 25 * iNumVotes;
					break;
				default:
					iValue += 150 * iNumVotes;
					break;
			}
			CvAssert(eOtherPlayer != NO_PLAYER);
			if (eOtherPlayer != NO_PLAYER)
			{
				CvLeagueAI::AlignmentLevels eAlignment = GetPlayer()->GetLeagueAI()->EvaluateAlignment(eOtherPlayer);
				switch (eAlignment)
				{
				case CvLeagueAI::ALIGNMENT_LIBERATOR:
				case CvLeagueAI::ALIGNMENT_LEADER:
					iValue += -200;
					break;
				case CvLeagueAI::ALIGNMENT_SELF:
					CvAssertMsg(false, "ALIGNMENT_SELF found when evaluating a trade deal for delegates. Please send Anton your save file and version.");
					break;
				case CvLeagueAI::ALIGNMENT_ALLY:
					iValue += -100;
					break;
				case CvLeagueAI::ALIGNMENT_CONFIDANT:
				case CvLeagueAI::ALIGNMENT_FRIEND:
					iValue += -50;
					break;
				case CvLeagueAI::ALIGNMENT_NEUTRAL:
					iValue += 200;
					break;
				case CvLeagueAI::ALIGNMENT_RIVAL:
				case CvLeagueAI::ALIGNMENT_HATRED:
				case CvLeagueAI::ALIGNMENT_ENEMY:
				case CvLeagueAI::ALIGNMENT_WAR:
					return INT_MAX;
				default:
					break;
				}
				MajorCivApproachTypes eOtherPlayerApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
				if (eOtherPlayerApproach == MAJOR_CIV_APPROACH_HOSTILE || eOtherPlayerApproach == MAJOR_CIV_APPROACH_WAR)
				{
					iValue += 500;
				}
			}
			//If the total is more than a third of the total votes on the board...that probably means it'll pass.
			if((iNumVotes + iTheirVotes) > (iVotesNeeded / 3))
			{
				iValue /= 2;
			}
			// Adjust based on their vote total - Having lots of votes left means they could counter these ones and exploit us
			float fVotesRatio = (float)iNumVotes / (float)pLeague->CalculateStartingVotesForMember(eOtherPlayer);
			if (fVotesRatio > 0.5f)
			{
				// More than half their votes...they probably aren't going to screw us
				iValue /= 2;
			}
			else if (fVotesRatio > 0.25f)
			{
				// They have a lot of remaining votes
				iValue *= 2;
			}
			else
			{
				// They have a hoard of votes
				iValue *= 4;
			}
		}
#endif
	}
	// Giving their votes to us - Higher value for voting on things we like
	else
	{
#if defined(MOD_BALANCE_CORE_DEALS)
		//They're offering us a vote...but on what? If we like it, it's worth more. If we don't, it's worth less.
		//Also, if they have tons of extra votes, it's worth less (cuz they'll screw us, probably).
		CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
		if(pLeague)
		{
			int iOurVotes = pLeague->CalculateStartingVotesForMember(GetPlayer()->GetID());
			int iTheirVotes = pLeague->CalculateStartingVotesForMember(eOtherPlayer);
			//We shouldn't ask them to vote on things that have to do with them personally.
			CvEnactProposal* pProposal = pLeague->GetEnactProposal(iProposalID);
			if (pProposal != NULL)
			{
				PlayerTypes eTargetPlayer = NO_PLAYER;
				ResolutionDecisionTypes eProposerDecision = pProposal->GetProposerDecision()->GetType();
				if (eProposerDecision == RESOLUTION_DECISION_ANY_MEMBER ||
					eProposerDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
					eProposerDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
				{
					eTargetPlayer = (PlayerTypes) pProposal->GetProposerDecision()->GetDecision();
					if(eTargetPlayer != NO_PLAYER && eTargetPlayer == eOtherPlayer)
					{
						return INT_MAX;
					}
				}
				//Let's look real quick to see if this is the world leader vote. If so, BUY EVERYTHING WE CAN if we can win with their votes in tow.
				if(pProposal->GetEffects()->bDiplomaticVictory)
				{			
					int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
					if((iOurVotes + iTheirVotes) >= iVotesNeededToWin)
					{
						iValue *= 100;
					}
				}
			}
			int iVotesNeeded = 0;
			
			PlayerTypes eLoopPlayer;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;
				if (GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isMinorCiv() && pLeague->IsMember(eLoopPlayer))
				{			
					iVotesNeeded += pLeague->CalculateStartingVotesForMember(eLoopPlayer);
				}
			}
			// Adjust based on LeagueAI
			CvLeagueAI::DesireLevels eDesire = GetPlayer()->GetLeagueAI()->EvaluateVoteForTrade(iProposalID, iVoteChoice, iNumVotes, bRepeal);
			switch(eDesire)
			{
				case CvLeagueAI::DESIRE_NEVER:
				case CvLeagueAI::DESIRE_STRONG_DISLIKE:
				case CvLeagueAI::DESIRE_DISLIKE:
				case CvLeagueAI::DESIRE_WEAK_DISLIKE:	
				case CvLeagueAI::DESIRE_NEUTRAL:
					return INT_MAX;
				case CvLeagueAI::DESIRE_WEAK_LIKE:
					iValue += 25 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_LIKE:
					iValue += 75 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_STRONG_LIKE:
					iValue += 150 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_ALWAYS:
					iValue += 250 * iNumVotes;
					break;
				default:
					iValue += 100 * iNumVotes;
					break;
			}
			CvAssert(eOtherPlayer != NO_PLAYER);
			if (eOtherPlayer != NO_PLAYER)
			{
				CvLeagueAI::AlignmentLevels eAlignment = GetPlayer()->GetLeagueAI()->EvaluateAlignment(eOtherPlayer);
				switch (eAlignment)
				{
				case CvLeagueAI::ALIGNMENT_LIBERATOR:
				case CvLeagueAI::ALIGNMENT_LEADER:
					iValue += 400;
					break;
				case CvLeagueAI::ALIGNMENT_SELF:
					CvAssertMsg(false, "ALIGNMENT_SELF found when evaluating a trade deal for delegates. Please send Anton your save file and version.");
					break;
				case CvLeagueAI::ALIGNMENT_ALLY:
					iValue += 300;
					break;
				case CvLeagueAI::ALIGNMENT_CONFIDANT:
				case CvLeagueAI::ALIGNMENT_FRIEND:
					iValue += 150;
					break;
				case CvLeagueAI::ALIGNMENT_NEUTRAL:
					break;
				case CvLeagueAI::ALIGNMENT_RIVAL:
					iValue += -150;
					break;
				case CvLeagueAI::ALIGNMENT_HATRED:
					iValue += -300;
					break;
				case CvLeagueAI::ALIGNMENT_ENEMY:
				case CvLeagueAI::ALIGNMENT_WAR:
					iValue += -500;
					break;
				default:
					break;
				}
				MajorCivApproachTypes eOtherPlayerApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
				if (eOtherPlayerApproach == MAJOR_CIV_APPROACH_HOSTILE || eOtherPlayerApproach == MAJOR_CIV_APPROACH_WAR)
				{
					iValue += -500;
				}
			}
			//If the total is more than a third of the total votes on the board...that probably means it'll pass.
			if((iNumVotes + iOurVotes) > (iVotesNeeded / 3))
			{
				iValue *= 2;
			}
			// Adjust based on their vote total - Having lots of votes left means they could counter these ones and exploit us
			float fVotesRatio = (float)iNumVotes / (float)pLeague->CalculateStartingVotesForMember(eOtherPlayer);
			if (fVotesRatio > 0.5f)
			{
				// More than half their votes...they probably aren't going to screw us
				iValue *= 2;
			}
			else if (fVotesRatio > 0.25f)
			{
				// They have a lot of remaining votes
				iValue /= 2;
			}
			else
			{
				// They have a hoard of votes
				iValue /= 4;
			}
		}
#endif
	}

	iValue = MAX(iValue, 0);

#if !defined(MOD_BALANCE_CORE)
	// Adjust based on how many votes
	iValue *= iNumVotes;
#endif

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if (bUseEvenValue)
	{
		iValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetVoteCommitmentValue(!bFromMe, GetPlayer()->GetID(), iProposalID, iVoteChoice, iNumVotes, bRepeal, /*bUseEvenValue*/ false);

		iValue /= 2;
	}

	return iValue;
}

/// See if adding Vote Commitment to their side of the deal helps even out pDeal
void CvDealAI::DoAddVoteCommitmentToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Them, but them is us. Please send Anton your save file and version.");
#if defined(MOD_BALANCE_CORE)
	CvWeightedVector<int> viTradeValues;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if(pLeague == NULL || !pLeague)
	{
		return;
	}
#endif
	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// Can't already be a Vote Commitment in the Deal
			if(!pDeal->IsVoteCommitmentTrade(eThem) && !pDeal->IsVoteCommitmentTrade(eMyPlayer))
			{
				CvLeagueAI::VoteCommitmentList vDesiredCommitments = GetPlayer()->GetLeagueAI()->GetDesiredVoteCommitments(eThem);
				for (CvLeagueAI::VoteCommitmentList::iterator it = vDesiredCommitments.begin(); it != vDesiredCommitments.end(); ++it)
				{
					int iProposalID = it->iResolutionID;
					int iVoteChoice = it->iVoteChoice;
					int iNumVotes = it->iNumVotes;
					bool bRepeal = !it->bEnact;

					if (iProposalID != -1 && pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_VOTE_COMMITMENT, iProposalID, iVoteChoice, iNumVotes, bRepeal))
					{
						int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ false, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, bUseEvenValue);

						// If adding this to the deal doesn't take it over the limit, do it (pick best option below)
#if defined(MOD_BALANCE_CORE)
						if(iItemValue==INT_MAX)
						{
							continue;
						}
						else
						{
							viTradeValues.push_back(iProposalID, iItemValue);
						}
					}
				}
				// Sort the vector based on value to get the best possible item to trade.
				viTradeValues.SortItems();
				if(viTradeValues.size() > 0)
				{
					for(int iRanking = 0; iRanking < viTradeValues.size(); iRanking++)
					{
						int iWeight = viTradeValues.GetWeight(iRanking);
						if(iWeight != 0)
						{
							CvLeagueAI::VoteCommitmentList vDesiredCommitments = GetPlayer()->GetLeagueAI()->GetDesiredVoteCommitments(eThem);
							for (CvLeagueAI::VoteCommitmentList::iterator it = vDesiredCommitments.begin(); it != vDesiredCommitments.end(); ++it)
							{
								int iProposalID = it->iResolutionID;
								int iVoteChoice = it->iVoteChoice;
								int iNumVotes = it->iNumVotes;
								bool bRepeal = !it->bEnact;
								if(iWeight == GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ false, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, bUseEvenValue))
								{
									if((iWeight + iValueTheyreOffering) <= (iValueImOffering + iAmountOverWeWillRequest))
									{
										pDeal->AddVoteCommitment(eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal);
										iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
									
										int iAmountOverWeWillRequest = iTotalValue;
										int iPercentOverWeWillRequest = 0;
										if(GET_PLAYER(eThem).isHuman())
										{
											iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
										}
										else
										{
											iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eThem);
										}
										iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
										iAmountOverWeWillRequest /= 100;
										if(iTotalValue >= iAmountOverWeWillRequest)
										{
											return;
										}
									}
								}
							}					
#endif
						}
					}
				}
			}
		}
	}
}

/// See if adding a Vote Commitment to our side of the deal helps even out pDeal
void CvDealAI::DoAddVoteCommitmentToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");
#if defined(MOD_BALANCE_CORE)
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if(pLeague == NULL || !pLeague)
	{
		return;
	}
#endif
	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// Can't already be a Vote Commitment in the Deal
			if(!pDeal->IsVoteCommitmentTrade(eThem) && !pDeal->IsVoteCommitmentTrade(eMyPlayer))
			{
				CvLeagueAI::VoteCommitmentList vDesiredCommitments;
				if (GET_PLAYER(eThem).isHuman())
				{
					vDesiredCommitments = GetPlayer()->GetLeagueAI()->GetDesiredVoteCommitments(eThem, true);
				}
				else
				{
					vDesiredCommitments = GET_PLAYER(eThem).GetLeagueAI()->GetDesiredVoteCommitments(eMyPlayer);
				}
				CvWeightedVector<int> viTradeValues;
				for (CvLeagueAI::VoteCommitmentList::iterator it = vDesiredCommitments.begin(); it != vDesiredCommitments.end(); ++it)
				{
					int iProposalID = it->iResolutionID;
					int iVoteChoice = it->iVoteChoice;
					int iNumVotes = it->iNumVotes;
					bool bRepeal = !it->bEnact;

					if (iProposalID != -1 && pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_VOTE_COMMITMENT, iProposalID, iVoteChoice, iNumVotes, bRepeal))
					{
						int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ true, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, bUseEvenValue);

						// If adding this to the deal doesn't take it under the min limit, do it
						if (iItemValue == INT_MAX)
						{
							continue;
						}

						viTradeValues.push_back(iProposalID, iItemValue);
					}
				}
				// Sort the vector based on value to get the best possible item to trade.
				viTradeValues.SortItems();
				if (viTradeValues.size() > 0)
				{
					for (int iRanking = 0; iRanking < viTradeValues.size(); iRanking++)
					{
						int iWeight = viTradeValues.GetWeight(iRanking);
						if (iWeight != 0)
						{
							for (CvLeagueAI::VoteCommitmentList::iterator it = vDesiredCommitments.begin(); it != vDesiredCommitments.end(); ++it)
							{
								int iProposalID = it->iResolutionID;
								int iVoteChoice = it->iVoteChoice;
								int iNumVotes = it->iNumVotes;
								bool bRepeal = !it->bEnact;

								if (iProposalID != -1 && pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_VOTE_COMMITMENT, iProposalID, iVoteChoice, iNumVotes, bRepeal))
								{
									if (iWeight == GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ true, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, bUseEvenValue))
									{
										if ((iWeight + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
										{
											pDeal->AddVoteCommitment(eMyPlayer, iProposalID, iVoteChoice, iNumVotes, bRepeal);
											iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
											int iAmountUnderWeWillOffer = iTotalValue;
											int iPercentOverWeWillOffer = 0;
											if (GET_PLAYER(eThem).isHuman())
											{
												iPercentOverWeWillOffer = GetDealPercentLeewayWithHuman();
											}
											else
											{
												iPercentOverWeWillOffer = GetDealPercentLeewayWithAI(eThem);
											}
											iAmountUnderWeWillOffer *= iPercentOverWeWillOffer;
											iAmountUnderWeWillOffer /= 100;
											if (iTotalValue <= iAmountUnderWeWillOffer)
											{
												return;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
#if defined(MOD_BALANCE_CORE)
/// See if adding 3rd Party War to their side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyWarToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Them, but them is us. Please send Anton your save file and version.");
#if defined(MOD_BALANCE_CORE)
	CvWeightedVector<int> viTradeValues;
#endif
	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();
			PlayerTypes eLoopPlayer;

			for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(eLoopPlayer != NO_PLAYER && GetPlayer()->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && !GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					TeamTypes eOtherTeam = GET_PLAYER(eLoopPlayer).getTeam();
					// Can't already be a War Commitment in the Deal
					if(!pDeal->IsThirdPartyWarTrade(eThem, eOtherTeam) && !pDeal->IsThirdPartyWarTrade(eMyPlayer, eOtherTeam))
					{
						if (pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_THIRD_PARTY_WAR, eOtherTeam))
						{
							int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_WAR, /*bFromMe*/ false, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

							// If adding this to the deal doesn't take it over the limit, do it (pick best option below)
							if(iItemValue==INT_MAX)
							{
								continue;
							}
							else
							{
								viTradeValues.push_back(eLoopPlayer, iItemValue);
							}
						}
					}
				}
			}
			// Sort the vector based on value to get the best possible item to trade.
			viTradeValues.SortItems();
			if(viTradeValues.size() > 0)
			{
				for(int iRanking = 0; iRanking < viTradeValues.size(); iRanking++)
				{
					int iWeight = viTradeValues.GetWeight(iRanking);
					if(iWeight != 0)
					{
						PlayerTypes eLoopPlayer;

						for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
						{
							eLoopPlayer = (PlayerTypes) iPlayerLoop;

							if(eLoopPlayer != NO_PLAYER && GetPlayer()->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && !GET_PLAYER(eLoopPlayer).isMinorCiv())
							{
								TeamTypes eOtherTeam = GET_PLAYER(eLoopPlayer).getTeam();

								if(iWeight == GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_WAR, /*bFromMe*/ false, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, bUseEvenValue))
								{
									if((iWeight + iValueTheyreOffering) <= (iValueImOffering + iAmountOverWeWillRequest))
									{
										pDeal->AddThirdPartyWar(eThem, eOtherTeam);
										iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
										int iAmountOverWeWillRequest = iTotalValue;
										int iPercentOverWeWillRequest = 0;
										if(GET_PLAYER(eThem).isHuman())
										{
											iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
										}
										else
										{
											iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eThem);
										}
										iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
										iAmountOverWeWillRequest /= 100;
										if(iTotalValue >= iAmountOverWeWillRequest)
										{
											return;
										}
									}
								}
							}
						}			
					}
				}
			}
		}
	}
}

/// See if adding a Vote Commitment to our side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyWarToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();
			PlayerTypes eLoopPlayer;

			for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(eLoopPlayer != NO_PLAYER && eLoopPlayer != eThem && eLoopPlayer != GetPlayer()->GetID() && GetPlayer()->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && !GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					TeamTypes eOtherTeam = GET_PLAYER(eLoopPlayer).getTeam();				
					// Can't already be a Trade Commitment in the Deal
					if(!pDeal->IsThirdPartyWarTrade(eThem, eOtherTeam) && !pDeal->IsThirdPartyWarTrade(eMyPlayer, eOtherTeam))
					{
						if (pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_THIRD_PARTY_WAR, eOtherTeam))
						{
							int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_WAR, /*bFromMe*/ true, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

							// If adding this to the deal doesn't take it over the limit, do it
							if(iItemValue==INT_MAX)
							{
								continue;
							}
							if((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
							{
								pDeal->AddThirdPartyWar(eMyPlayer, eOtherTeam);
								iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
								int iAmountUnderWeWillOffer = iTotalValue;
								int iPercentOverWeWillOffer = 0;
								if(GET_PLAYER(eThem).isHuman())
								{
									iPercentOverWeWillOffer = GetDealPercentLeewayWithHuman();
								}
								else
								{
									iPercentOverWeWillOffer = GetDealPercentLeewayWithAI(eThem);
								}
								iAmountUnderWeWillOffer *= iPercentOverWeWillOffer;
								iAmountUnderWeWillOffer /= 100;
								if(iTotalValue <= iAmountUnderWeWillOffer)
								{
									return;
								}
							}
						}
					}
				}
			}
		}
	}
}
/// See if adding 3rd Party Peace to their side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyPeaceToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Them, but them is us. Please send Anton your save file and version.");
#if defined(MOD_BALANCE_CORE)
	CvWeightedVector<int> viTradeValues;
#endif
	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();
			PlayerTypes eLoopPlayer;

			for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(eLoopPlayer != NO_PLAYER && GetPlayer()->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && !GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					TeamTypes eOtherTeam = GET_PLAYER(eLoopPlayer).getTeam();
					// Can't already be a Vote Commitment in the Deal
					if(!pDeal->IsThirdPartyPeaceTrade(eThem, eOtherTeam) && !pDeal->IsThirdPartyPeaceTrade(eMyPlayer, eOtherTeam))
					{
						if (pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_THIRD_PARTY_PEACE, eOtherTeam))
						{
							int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_PEACE, /*bFromMe*/ false, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

							// If adding this to the deal doesn't take it over the limit, do it (pick best option below)
							if(iItemValue==INT_MAX)
							{
								continue;
							}
							else
							{
								viTradeValues.push_back(eLoopPlayer, iItemValue);
							}
						}
					}
				}
			}
			// Sort the vector based on value to get the best possible item to trade.
			viTradeValues.SortItems();
			if(viTradeValues.size() > 0)
			{
				for(int iRanking = 0; iRanking < viTradeValues.size(); iRanking++)
				{
					int iWeight = viTradeValues.GetWeight(iRanking);
					if(iWeight != 0)
					{
						PlayerTypes eLoopPlayer;

						for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
						{
							eLoopPlayer = (PlayerTypes) iPlayerLoop;

							if(eLoopPlayer != NO_PLAYER && GetPlayer()->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && !GET_PLAYER(eLoopPlayer).isMinorCiv())
							{
								TeamTypes eOtherTeam = GET_PLAYER(eLoopPlayer).getTeam();

								if(iWeight == GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_PEACE, /*bFromMe*/ false, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, bUseEvenValue))
								{
									if((iWeight + iValueTheyreOffering) <= (iValueImOffering + iAmountOverWeWillRequest))
									{
										pDeal->AddThirdPartyWar(eThem, eOtherTeam);
										iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
										int iAmountOverWeWillRequest = iTotalValue;
										int iPercentOverWeWillRequest = 0;
										if(GET_PLAYER(eThem).isHuman())
										{
											iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
										}
										else
										{
											iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eThem);
										}
										iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
										iAmountOverWeWillRequest /= 100;
										if(iTotalValue >= iAmountOverWeWillRequest)
										{
											return;
										}
									}
								}
							}
						}			
					}
				}
			}
		}
	}
}

/// See if adding a 3rd Party Peace deal to our side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyPeaceToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();
			PlayerTypes eLoopPlayer;

			for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(eLoopPlayer != NO_PLAYER && GetPlayer()->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && !GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					TeamTypes eOtherTeam = GET_PLAYER(eLoopPlayer).getTeam();
					// Can't already be a Vote Commitment in the Deal
					if(!pDeal->IsThirdPartyPeaceTrade(eThem, eOtherTeam) && !pDeal->IsThirdPartyPeaceTrade(eMyPlayer, eOtherTeam))
					{
						if (pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_THIRD_PARTY_PEACE, eOtherTeam))
						{
							int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_PEACE, /*bFromMe*/ true, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

							// If adding this to the deal doesn't take it over the limit, do it
							if(iItemValue==INT_MAX)
							{
								continue;
							}
							if((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
							{
								pDeal->AddThirdPartyWar(eMyPlayer, eOtherTeam);
								iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
								int iAmountUnderWeWillOffer = iTotalValue;
								int iPercentOverWeWillOffer = 0;
								if(GET_PLAYER(eThem).isHuman())
								{
									iPercentOverWeWillOffer = GetDealPercentLeewayWithHuman();
								}
								else
								{
									iPercentOverWeWillOffer = GetDealPercentLeewayWithAI(eThem);
								}
								iAmountUnderWeWillOffer *= iPercentOverWeWillOffer;
								iAmountUnderWeWillOffer /= 100;
								if(iTotalValue <= iAmountUnderWeWillOffer)
								{
									return;
								}
							}
						}
					}
				}
			}
		}
	}
}
#endif
/// See if adding a Resource to their side of the deal helps even out pDeal
void CvDealAI::DoAddResourceToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Them, but them is us.  Please show Jon");
#if defined(MOD_BALANCE_CORE)
	CvWeightedVector<int> viTradeValues;
#endif
	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iItemValue;

			int iResourceLoop;
			ResourceTypes eResource;
			int iResourceQuantity;

			// Look to trade Luxuries first
			for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				eResource = (ResourceTypes) iResourceLoop;

				const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
					continue;

				iResourceQuantity = GET_PLAYER(eThem).getNumResourceAvailable(eResource, false);

				// Don't bother looking at this Resource if the other player doesn't even have any of it
				if(iResourceQuantity <= 0)
					continue;

				// Don't bother if we wouldn't get Happiness from it due to World Congress
				if(GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(eMyPlayer, eResource))
					continue;

				// Quantity is always 1 if it's a Luxury
				iResourceQuantity = 1;

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration, bUseEvenValue);

					if (iItemValue > 0 && GET_PLAYER(eMyPlayer).GetPlayerTraits()->IsImportsCountTowardsMonopolies())
					{
						if(GetPlayer()->GetMonopolyPercent(eResource) < GC.getGame().GetGreatestPlayerResourceMonopolyValue(eResource))
						{
							int iNumResourceOwned = GetPlayer()->getNumResourceTotal(eResource, false);
							int iNumResourceImported = GetPlayer()->getNumResourceTotal(eResource, true, true);
							//we don't want resources that won't get us a bonus.
							bool bBad = false;
							for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
							{
								if (pkResourceInfo->getYieldChangeFromMonopoly((YieldTypes)iJ) > 0 && iNumResourceOwned <= 0 && iNumResourceImported > 0)
								{
									bBad = true;
									break;
								}
							}
							if (!bBad)
							{
								iItemValue *= (125 + GetPlayer()->GetMonopolyPercent(eResource));
								iItemValue /= 100;
							}
						}
					}

					// If adding this to the deal doesn't take it over the limit, do it (pick best option below)
#if defined(MOD_BALANCE_CORE)
					if(iItemValue==INT_MAX)
					{
						continue;
					}
					else
					{
						viTradeValues.push_back(eResource, iItemValue);
					}
				}
			}
#else
					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)

					{
						// Try to change the current item if it already exists, otherwise add it
						if(!pDeal->ChangeResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
						}
					}
				}
			}
#endif
			// Now look at Strategic Resources
			for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				eResource = (ResourceTypes) iResourceLoop;

				const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
					continue;

				iResourceQuantity = GET_PLAYER(eThem).getNumResourceAvailable(eResource, false);

				// Don't bother looking at this Resource if the other player doesn't even have any of it
				if(iResourceQuantity <= 0)
					continue;

				// Quantity is always 1 if it's a Luxury, 3 if Strategic
				iResourceQuantity = min(3, iResourceQuantity);	// 3 or what they have, whichever is less

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration, bUseEvenValue);

					// If adding this to the deal doesn't take it over the limit, do it (pick best option below)
#if defined(MOD_BALANCE_CORE)
					if(iItemValue==INT_MAX)
					{
						continue;
					}
					else
					{
						viTradeValues.push_back(eResource, iItemValue);
					}
				}
			}
			// Sort the vector based on value to get the best possible item to trade.
			viTradeValues.SortItems();
			if(viTradeValues.size() > 0)
			{
				bool bFirst = true;
				for(int iRanking = 0; iRanking < viTradeValues.size(); iRanking++)
				{
					int iWeight = viTradeValues.GetWeight(iRanking);
					if(iWeight != 0)
					{
						eResource = (ResourceTypes) viTradeValues.GetElement(iRanking);
						if(eResource == NO_RESOURCE)
							continue;

						const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
						if(!pkResourceInfo)
							continue;

						iResourceQuantity = GET_PLAYER(eThem).getNumResourceAvailable(eResource, false);

						// Don't bother looking at this Resource if the other player doesn't even have any of it
						if(iResourceQuantity <= 0)
							continue;

						if(pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_STRATEGIC)
						{
							iResourceQuantity = min(3, iResourceQuantity);
						}
						else if(pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
						{
							iResourceQuantity = 1;
						}
						if(bFirst || ((iWeight + iValueTheyreOffering) <= (iValueImOffering + iAmountOverWeWillRequest)))
						{
							bFirst = false;
							// Try to change the current item if it already exists, otherwise add it
							if(!pDeal->ChangeResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration))
							{
								pDeal->AddResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration);
								iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
								int iAmountOverWeWillRequest = iTotalValue;
								int iPercentOverWeWillRequest = 0;
								if(GET_PLAYER(eThem).isHuman())
								{
									iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
								}
								else
								{
									iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eThem);
								}
								iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
								iAmountOverWeWillRequest /= 100;
								if(iTotalValue >= iAmountOverWeWillRequest)
								{
									return;						
#endif
								}
							}
						}
					}
#if defined(MOD_BALANCE_CORE)
#else
					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)

					{
						// Try to change the current item if it already exists, otherwise add it
						if(!pDeal->ChangeResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
						}
					}
#endif
				}
			}
		}
	}
}

/// See if adding a Resource to our side of the deal helps even out pDeal
void CvDealAI::DoAddResourceToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iItemValue;

			ResourceTypes eResource;
			int iResourceQuantity;
			for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				eResource = (ResourceTypes) iResourceLoop;
				iResourceQuantity = GET_PLAYER(eMyPlayer).getNumResourceAvailable(eResource, false);

				// Don't bother looking at this Resource if we don't even have any of it
				if(iResourceQuantity == 0)
				{
					continue;
				}

				const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if (pkResourceInfo == NULL)
					continue;

				// Quantity is always 1 if it's a Luxury, 5 if Strategic
				if(pkResourceInfo->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					iResourceQuantity = 1;

					// Don't bother if they wouldn't get Happiness from it due to World Congress
					if(GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(eThem, eResource))
						continue;
				}
				else
				{
					iResourceQuantity = min(2, iResourceQuantity);	// 2 or what we have, whichever is less
				}

				// See if we can actually trade it to them
				if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ true, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration, bUseEvenValue);

					// If adding this to the deal doesn't take it under the min limit, do it
#if defined(MOD_BALANCE_CORE)
					if(iItemValue==INT_MAX)
					{
						continue;
					}
					if((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
#else
					if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
#endif
					{
						// Try to change the current item if it already exists, otherwise add it
						if(!pDeal->ChangeResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
#if defined(MOD_BALANCE_CORE)
							int iAmountUnderWeWillOffer = iTotalValue;
							int iPercentOverWeWillOffer = 0;
							if(GET_PLAYER(eThem).isHuman())
							{
								iPercentOverWeWillOffer = GetDealPercentLeewayWithHuman();
							}
							else
							{
								iPercentOverWeWillOffer = GetDealPercentLeewayWithAI(eThem);
							}
							iAmountUnderWeWillOffer *= iPercentOverWeWillOffer;
							iAmountUnderWeWillOffer /= 100;
							if(iTotalValue <= iAmountUnderWeWillOffer)
							{
								return;
							}
#endif
						}
					}
				}
			}
		}
	}
}


/// See if adding Embassy to their side of the deal helps even out pDeal
void CvDealAI::DoAddEmbassyToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Embassy to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			if(!pDeal->IsAllowEmbassyTrade(eThem))
			{
				PlayerTypes eMyPlayer = GetPlayer()->GetID();

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_ALLOW_EMBASSY))
				{
					int iItemValue = GetTradeItemValue(TRADE_ITEM_ALLOW_EMBASSY, /*bFromMe*/ false, eThem, -1, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

					// If adding this to the deal doesn't take it over the limit, do it
#if defined(MOD_BALANCE_CORE)
					if(iItemValue==INT_MAX)
					{
						return;
					}
					if((iItemValue + iValueTheyreOffering) >= (iValueImOffering + iAmountOverWeWillRequest))
#else
					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
#endif
					{
						pDeal->AddAllowEmbassy(eThem);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					}
				}
			}
		}
	}
}

/// See if adding Embassy to our side of the deal helps even out pDeal
void CvDealAI::DoAddEmbassyToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Embassy to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			if(!pDeal->IsAllowEmbassyTrade(eMyPlayer))
			{
				// See if we can actually trade it to them
				if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_ALLOW_EMBASSY))
				{
					int iItemValue = GetTradeItemValue(TRADE_ITEM_ALLOW_EMBASSY, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

					// If adding this to the deal doesn't take it under the min limit, do it
#if defined(MOD_BALANCE_CORE)
					if(iItemValue==INT_MAX)
					{
						return;
					}
					if((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
#else
					if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
#endif
					{
						pDeal->AddAllowEmbassy(eMyPlayer);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					}
				}
			}
		}
	}
}

/// See if adding Open Borders to their side of the deal helps even out pDeal
void CvDealAI::DoAddOpenBordersToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			if(!pDeal->IsOpenBordersTrade(eThem))
			{
				PlayerTypes eMyPlayer = GetPlayer()->GetID();

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_OPEN_BORDERS))
				{
					int iItemValue = GetTradeItemValue(TRADE_ITEM_OPEN_BORDERS, /*bFromMe*/ false, eThem, -1, -1, -1, /*bFlag1*/false, iDealDuration, bUseEvenValue);

					// If adding this to the deal doesn't take it over the limit, do it
#if defined(MOD_BALANCE_CORE)
					if(iItemValue==INT_MAX)
					{
						return;
					}
					if((iItemValue + iValueTheyreOffering) >= (iValueImOffering + iAmountOverWeWillRequest))
#else
					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
#endif
					{
						pDeal->AddOpenBorders(eThem, iDealDuration);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					}
				}
			}
		}
	}
}

/// See if adding Open Borders to our side of the deal helps even out pDeal
void CvDealAI::DoAddOpenBordersToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			if(!pDeal->IsOpenBordersTrade(eMyPlayer))
			{
				// See if we can actually trade it to them
				if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_OPEN_BORDERS))
				{
					int iItemValue = GetTradeItemValue(TRADE_ITEM_OPEN_BORDERS, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, iDealDuration, bUseEvenValue);

					// If adding this to the deal doesn't take it under the min limit, do it
#if defined(MOD_BALANCE_CORE)
					if(iItemValue==INT_MAX)
					{
						return;
					}
					if((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
#else
					if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
#endif
					{
						pDeal->AddOpenBorders(eMyPlayer, iDealDuration);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					}
				}
			}
		}
	}
}

/// See if adding Cities to our side of the deal helps even out pDeal
void CvDealAI::DoAddCitiesToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
#if defined(MOD_BALANCE_CORE)
	// If we're not the one surrendering here, don't bother
	if((pDeal->IsPeaceTreatyTrade(eThem) && pDeal->GetSurrenderingPlayer() != eMyPlayer) || !pDeal->ContainsItemType(TRADE_ITEM_CITIES, eThem))
		return;
#else
	// If we're not the one surrendering here, don't bother
	if(pDeal->GetSurrenderingPlayer() != eMyPlayer)
		return;
#endif
	// Don't change things
	if(bDontChangeMyExistingItems)
		return;

	// We don't owe them anything
	if(iTotalValue <= 0)
		return;

	CvPlayer* pSellingPlayer = GetPlayer();

	// If the player only has 1 City then we can't get any more from him
	if(pSellingPlayer->getNumCities() == 1)
		return;

	// Create vector of the losing players' Cities so we can see which are the closest to the winner
	CvWeightedVector<int> viCityPriceRatio;

	// Loop through all of the loser's Cities
	CvCity* pLoopCity;
	int iCityLoop;
	for(pLoopCity = pSellingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pSellingPlayer->nextCity(&iCityLoop))
	{
		int iWhatTheyWouldPay = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), /*bFromMe*/ true, eThem, /*bUseEvenValue*/ false, pDeal);
		int iWhatIWouldPay = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), /*bFromMe*/ false, eThem, /*bUseEvenValue*/ false, pDeal);

		if (iWhatTheyWouldPay == INT_MAX || iWhatIWouldPay == INT_MAX)
		{
			continue;
		}

		int iRatio = (iWhatTheyWouldPay * 100) / max(1,iWhatIWouldPay);

		// Don't include the capital in the list of Cities the buyer can receive
		if(!pLoopCity->isCapital() && iRatio>130)
		{
			viCityPriceRatio.push_back(pLoopCity->GetID(), iRatio);
		}
	}

	// Sort the vector based on the price ratio we can achieve
	viCityPriceRatio.SortItems();

	// Loop through sorted Cities
	int iSortedCityID;
	for(int iSortedCityIndex = 0; iSortedCityIndex < viCityPriceRatio.size(); iSortedCityIndex++)
	{
		iSortedCityID = viCityPriceRatio.GetElement(iSortedCityIndex);
		pLoopCity = pSellingPlayer->getCity(iSortedCityID);

		// See if we can actually trade it to them
		if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
		{
			int iItemValue = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), /*bFromMe*/ true, eThem, false, pDeal);
			if (iItemValue == INT_MAX)
				continue;

			// If adding this to the deal doesn't take it under the min limit, do it
#if defined(MOD_BALANCE_CORE)
			if((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
#else
			if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
#endif
			{
				OutputDebugString( CvString::format("Adding %s to deal. Seller %s, buyer %s, price ratio %d\n", 
					pLoopCity->getName().c_str(), pSellingPlayer->getName(), GET_PLAYER(eThem).getName(), viCityPriceRatio.GetWeight(iSortedCityIndex) ).c_str() );
				pDeal->AddCityTrade(eMyPlayer, iSortedCityID);
				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
#if defined(MOD_BALANCE_CORE)
				int iAmountUnderWeWillOffer = iTotalValue;
				int iPercentOverWeWillOffer = 0;
				if(GET_PLAYER(eThem).isHuman())
				{
					iPercentOverWeWillOffer = GetDealPercentLeewayWithHuman();
				}
				else
				{
					iPercentOverWeWillOffer = GetDealPercentLeewayWithAI(eThem);
				}
				iAmountUnderWeWillOffer *= iPercentOverWeWillOffer;
				iAmountUnderWeWillOffer /= 100;
				if(iTotalValue <= iAmountUnderWeWillOffer)
				{
					return;
				}
#endif
			}
		}

	}
}
#if defined(MOD_BALANCE_CORE)
/// See if adding Cities to our side of the deal helps even out pDeal
void CvDealAI::DoAddCitiesToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	// If we're not the one surrendering here, don't bother
	if((pDeal->IsPeaceTreatyTrade(eThem) && pDeal->GetSurrenderingPlayer() != eThem) || !pDeal->ContainsItemType(TRADE_ITEM_CITIES, eMyPlayer))
		return;

	// Don't change things
	if(bDontChangeMyExistingItems)
		return;

	// We don't owe them anything
	if(iTotalValue > 0)
		return;

	CvPlayer* pSellingPlayer  = &GET_PLAYER(eThem);

	// If the player only has 1 City then we can't get any more from him
	if(pSellingPlayer->getNumCities() == 1)
		return;

	// Create vector of the losing players' Cities so we can see which are the closest to the winner
	CvWeightedVector<int> viCityPriceRatio;

	// Loop through all of the loser's Cities
	CvCity* pLoopCity;
	int iCityLoop;
	for(pLoopCity = pSellingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pSellingPlayer->nextCity(&iCityLoop))
	{
		int iWhatTheyWouldPay = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), /*bFromMe*/ false, eThem, /*bUseEvenValue*/ false, pDeal);
		int iWhatIWouldPay = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), /*bFromMe*/ true, eThem, /*bUseEvenValue*/ false, pDeal);

		if (iWhatTheyWouldPay == INT_MAX || iWhatIWouldPay == INT_MAX)
		{
			continue;
		}

		int iRatio = (iWhatTheyWouldPay * 100) / max(1,iWhatIWouldPay);

		// Don't include the capital in the list of Cities the buyer can receive
		if (!pLoopCity->isCapital() && iRatio<80) 
		{
			viCityPriceRatio.push_back(pLoopCity->GetID(), iRatio);
		}
	}

	// Sort the vector based on price ratio we can get
	viCityPriceRatio.SortItems();

	// Loop through sorted Cities.
	int iSortedCityID;
	for (int iSortedCityIndex = 0; iSortedCityIndex < viCityPriceRatio.size(); iSortedCityIndex++)
	{
		iSortedCityID = viCityPriceRatio.GetElement(iSortedCityIndex);
		pLoopCity = pSellingPlayer->getCity(iSortedCityID);

		// See if we can actually trade it to them
		if (pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
		{
			int iItemValue = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), /*bFromMe*/ false, eThem, false, pDeal);
			if (iItemValue == INT_MAX)
				continue;

			// If adding this to the deal doesn't take it under the min limit, do it
#if defined(MOD_BALANCE_CORE)
			if((iItemValue + iValueTheyreOffering) >= (iValueImOffering + iAmountOverWeWillRequest))
#else
			if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
#endif
			{
				OutputDebugString( CvString::format("Adding %s to deal. Seller %s, buyer %s, price ratio %d\n", 
					pLoopCity->getName().c_str(), pSellingPlayer->getName(), GetPlayer()->getName(), viCityPriceRatio.GetWeight(iSortedCityIndex) ).c_str() );
				pDeal->AddCityTrade(eThem, iSortedCityID);
				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
#if defined(MOD_BALANCE_CORE)
				int iAmountOverWeWillRequest = iTotalValue;
				int iPercentOverWeWillRequest = 0;
				if(GET_PLAYER(eThem).isHuman())
				{
					iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
				}
				else
				{
					iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eThem);
				}
				iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
				iAmountOverWeWillRequest /= 100;
				if(iTotalValue >= iAmountOverWeWillRequest)
				{
					return;
				}			
#endif
			}
		}
	}
}
#endif
/// See if adding Gold to their side of the deal helps even out pDeal
void CvDealAI::DoAddGoldToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Gold to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// Can't already be Gold from the other player in the Deal
			if(pDeal->GetGoldTrade(eMyPlayer) == 0)
			{
				int iNumGold = GetGoldForForValueExchange(-iTotalValue, /*bNumGoldFromValue*/ true, /*bFromMe*/ false, eThem, bUseEvenValue, /*bRoundUp*/ false);
#if defined(MOD_BALANCE_CORE)
				if(iNumGold <= 0)
				{
					return;
				}
#endif
				int iNumGoldAlreadyInTrade = pDeal->GetGoldTrade(eThem);
				iNumGold += iNumGoldAlreadyInTrade;
				iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eThem, TRADE_ITEM_GOLD));
#if defined(MOD_BALANCE_CORE)
				int iItemValue = GetGoldForForValueExchange(iNumGold, false, /*bFromMe*/ false, eThem, bUseEvenValue, /*bRoundUp*/ false);
				int iAmountOverWeWillRequest = iTotalValue;
				int iPercentOverWeWillRequest = 0;
				if(GET_PLAYER(eThem).isHuman())
				{
					iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
				}
				else
				{
					iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eThem);
				}
				iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
				iAmountOverWeWillRequest /= 100;
				if((iItemValue + iValueTheyreOffering) >= (iValueImOffering + iAmountOverWeWillRequest))
				{
#endif
				//iNumGold = min(iNumGold, GET_PLAYER(eThem).GetTreasury()->GetGold());
				if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eThem, iNumGold))
				{
					pDeal->AddGoldTrade(eThem, iNumGold);
				}

				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
#if defined(MOD_BALANCE_CORE)
				}
#endif
			}
		}
	}
}

/// See if adding Gold to our side of the deal helps even out pDeal
void CvDealAI::DoAddGoldToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Gold to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			// Can't already be Gold from the other player in the Deal
			if(pDeal->GetGoldTrade(eThem) == 0)
			{
				PlayerTypes eMyPlayer = GetPlayer()->GetID();

				int iNumGold = GetGoldForForValueExchange(iTotalValue, /*bNumGoldFromValue*/ true, /*bFromMe*/ true, eThem, bUseEvenValue, /*bRoundUp*/ false);
#if defined(MOD_BALANCE_CORE)
				if(iNumGold < 0)
				{
					return;
				}
#endif
				int iNumGoldAlreadyInTrade = pDeal->GetGoldTrade(eMyPlayer);
				iNumGold += iNumGoldAlreadyInTrade;
				iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eMyPlayer, TRADE_ITEM_GOLD));
#if defined(MOD_BALANCE_CORE)
				int iItemValue = GetGoldForForValueExchange(iNumGold, false, /*bFromMe*/ true, eThem, bUseEvenValue, /*bRoundUp*/ false);
				int iAmountUnderWeWillOffer = iTotalValue;
				int iPercentOverWeWillOffer = 0;
				if(GET_PLAYER(eThem).isHuman())
				{
					iPercentOverWeWillOffer = GetDealPercentLeewayWithHuman();
				}
				else
				{
					iPercentOverWeWillOffer = GetDealPercentLeewayWithAI(eThem);
				}
				iAmountUnderWeWillOffer *= iPercentOverWeWillOffer;
				iAmountUnderWeWillOffer /= 100;
				if((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
				{
#endif
				//iNumGold = min(iNumGold, GET_PLAYER(eMyPlayer).GetTreasury()->GetGold());
				if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eMyPlayer, iNumGold))
				{
					pDeal->AddGoldTrade(eMyPlayer, iNumGold);
				}

				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
#if defined(MOD_BALANCE_CORE)
				}
#endif
			}
		}
	}
}

/// See if adding Gold Per Turn to their side of the deal helps even out pDeal
void CvDealAI::DoAddGPTToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add GPT to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			if(GET_PLAYER(eThem).calculateGoldRate() > 0)
			{
				PlayerTypes eMyPlayer = GetPlayer()->GetID();

				// Can't already be GPT from the other player in the Deal
				if(pDeal->GetGoldPerTurnTrade(eMyPlayer) == 0)
				{
					int iNumGPT = GetGPTforForValueExchange(-iTotalValue, /*bNumGPTFromValue*/ true, iDealDuration, /*bFromMe*/ false, eThem, bUseEvenValue, /*bRoundUp*/ false);
#if defined(MOD_BALANCE_CORE)
					if(iNumGPT < 0)
					{
						return;
					}
#endif
					int iNumGPTAlreadyInTrade = pDeal->GetGoldPerTurnTrade(eThem);
					iNumGPT += iNumGPTAlreadyInTrade;
					iNumGPT = min(iNumGPT, GET_PLAYER(eThem).calculateGoldRate());
#if defined(MOD_BALANCE_CORE)
					int iItemValue = GetGPTforForValueExchange(iNumGPT, false, iDealDuration, /*bFromMe*/ false, eThem, bUseEvenValue, /*bRoundUp*/ false);
					if (iItemValue == MAX_INT)
					{
						return;
					}
					int iAmountOverWeWillRequest = iTotalValue;
					int iPercentOverWeWillRequest = 0;
					if (GET_PLAYER(eThem).isHuman())
					{
						iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
					}
					else
					{
						iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eThem);
					}
					iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
					iAmountOverWeWillRequest /= 100;
					if((iItemValue + iValueTheyreOffering) >= (iValueImOffering + iAmountOverWeWillRequest))
					{
#endif
					if(iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eThem, iNumGPT, iDealDuration))
					{
						pDeal->AddGoldPerTurnTrade(eThem, iNumGPT, iDealDuration);
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
#if defined(MOD_BALANCE_CORE)
					}
#endif
				}
			}
		}
	}
}

/// See if adding Gold Per Turn to our side of the deal helps even out pDeal
void CvDealAI::DoAddGPTToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add GPT to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			if(GET_PLAYER(eMyPlayer).calculateGoldRate() > 0)
			{
				// Can't already be GPT from the other player in the Deal
				if(pDeal->GetGoldPerTurnTrade(eThem) == 0)
				{
					int iNumGPT = GetGPTforForValueExchange(iTotalValue, /*bNumGPTFromValue*/ true, iDealDuration, /*bFromMe*/ true, eThem, bUseEvenValue, /*bRoundUp*/ false);
#if defined(MOD_BALANCE_CORE)
					if(iNumGPT < 0)
					{
						return;
					}
#endif
					int iNumGPTAlreadyInTrade = pDeal->GetGoldPerTurnTrade(eMyPlayer);
					iNumGPT += iNumGPTAlreadyInTrade;
					iNumGPT = min(iNumGPT, GET_PLAYER(eMyPlayer).calculateGoldRate());
#if defined(MOD_BALANCE_CORE)
					int iItemValue = GetGPTforForValueExchange(iNumGPT, false, iDealDuration, /*bFromMe*/ true, eThem, bUseEvenValue, /*bRoundUp*/ false);
					if (iItemValue == MAX_INT)
						return;

					int iAmountUnderWeWillOffer = iTotalValue;
					int iPercentOverWeWillOffer = 0;
					if (GET_PLAYER(eThem).isHuman())
					{
						iPercentOverWeWillOffer = GetDealPercentLeewayWithHuman();
					}
					else
					{
						iPercentOverWeWillOffer = GetDealPercentLeewayWithAI(eThem);
					}
					iAmountUnderWeWillOffer *= iPercentOverWeWillOffer;
					iAmountUnderWeWillOffer /= 100;
					if ((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
					{
#endif
					if(iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration))
					{
						pDeal->AddGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration);
					}
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
#if defined(MOD_BALANCE_CORE)
					}
#endif
				}
			}
		}
	}
}

/// See if removing Gold Per Turn from their side of the deal helps even out pDeal
void CvDealAI::DoRemoveGPTFromThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove GPT from Them, but them is us.  Please show Jon");

//	if (!bDontChangeTheirExistingItems)
	{
		if(iTotalValue > 0)
		{
			// Try to remove a bit more than the actual value discrepancy, as this should get us closer to even in the long-run
			int iValueToRemove = iTotalValue * 150;
			iValueToRemove /= 100;

			int iNumGoldPerTurnToRemove = GetGPTforForValueExchange(iValueToRemove, /*bNumGPTFromValue*/ true, iDealDuration, /*bFromMe*/ false, eThem, bUseEvenValue, /*bRoundUp*/ true);

			int iNumGoldPerTurnInThisDeal = pDeal->GetGoldPerTurnTrade(eThem);
			if(iNumGoldPerTurnInThisDeal > 0)
			{
				// Found some GoldPerTurn to remove
				iNumGoldPerTurnToRemove = min(iNumGoldPerTurnToRemove, iNumGoldPerTurnInThisDeal);
				iNumGoldPerTurnInThisDeal -= iNumGoldPerTurnToRemove;

				// Removing ALL GoldPerTurn, so just erase the item from the deal
				if(iNumGoldPerTurnInThisDeal == 0)
				{
					pDeal->RemoveByType(TRADE_ITEM_GOLD_PER_TURN);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
				// Remove some of the GoldPerTurn from the deal
				else
				{
					if(!pDeal->ChangeGoldPerTurnTrade(eThem, iNumGoldPerTurnInThisDeal, iDealDuration))
					{
						CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove GoldPerTurn from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}

/// See if removing Gold Per Turn from our side of the deal helps even out pDeal
void CvDealAI::DoRemoveGPTFromUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove GPT from Us, but them is us.  Please show Jon");

//	if (!bDontChangeMyExistingItems)
	{
		if(iTotalValue < 0)
		{
			// Try to remove a bit more than the actual value discrepancy, as this should get us closer to even in the long-run
#if defined(MOD_BALANCE_CORE)
			int iValueToRemove = -iTotalValue * 175;
#else
			int iValueToRemove = -iTotalValue * 150;
#endif

			iValueToRemove /= 100;

			int iNumGoldPerTurnToRemove = GetGPTforForValueExchange(iValueToRemove, /*bNumGPTFromValue*/ true, iDealDuration, /*bFromMe*/ true, eThem, bUseEvenValue, /*bRoundUp*/ true);

			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iNumGoldPerTurnInThisDeal = pDeal->GetGoldPerTurnTrade(eMyPlayer);
			if(iNumGoldPerTurnInThisDeal > 0)
			{
				// Found some GoldPerTurn to remove
				iNumGoldPerTurnToRemove = min(iNumGoldPerTurnToRemove, iNumGoldPerTurnInThisDeal);
				iNumGoldPerTurnInThisDeal -= iNumGoldPerTurnToRemove;

				// Removing ALL GoldPerTurn, so just erase the item from the deal
				if(iNumGoldPerTurnInThisDeal == 0)
				{
					pDeal->RemoveByType(TRADE_ITEM_GOLD_PER_TURN);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
				// Remove some of the GoldPerTurn from the deal
				else
				{
					if(!pDeal->ChangeGoldPerTurnTrade(eMyPlayer, iNumGoldPerTurnInThisDeal, iDealDuration))
					{
						CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove GoldPerTurn from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}

/// See if removing Gold from their side of the deal helps even out pDeal
void CvDealAI::DoRemoveGoldFromThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove Gold from Them, but them is us.  Please show Jon");

//	if (!bDontChangeTheirExistingItems)
	{
		if(iTotalValue > 0)
		{
			int iNumGoldInThisDeal = pDeal->GetGoldTrade(eThem);
			if(iNumGoldInThisDeal > 0)
			{
				// Found some Gold to remove
				int iNumGoldToRemove = min(iNumGoldInThisDeal, GetGoldForForValueExchange(iTotalValue, /*bNumGoldFromValue*/ true, /*bFromMe*/ false, eThem, bUseEvenValue, /*bRoundUp*/ true));
				iNumGoldInThisDeal -= iNumGoldToRemove;

				// Removing ALL Gold, so just erase the item from the deal
				if(iNumGoldInThisDeal == 0)
				{
					pDeal->RemoveByType(TRADE_ITEM_GOLD);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
				// Remove some of the Gold from the deal
				else
				{
					if(!pDeal->ChangeGoldTrade(eThem, iNumGoldInThisDeal))
					{
						CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove Gold from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}

/// See if removing Gold from our side of the deal helps even out pDeal
void CvDealAI::DoRemoveGoldFromUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove Gold from Us, but them is us.  Please show Jon");

//	if (!bDontChangeMyExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iNumGoldInThisDeal = pDeal->GetGoldTrade(eMyPlayer);
			if(iNumGoldInThisDeal > 0)
			{
				// Found some Gold to remove
				int iNumGoldToRemove = min(iNumGoldInThisDeal, GetGoldForForValueExchange(-iTotalValue, /*bNumGoldFromValue*/ true, /*bFromMe*/ true, eThem, bUseEvenValue, /*bRoundUp*/ true));
				iNumGoldInThisDeal -= iNumGoldToRemove;

				// Removing ALL Gold, so just erase the item from the deal
				if(iNumGoldInThisDeal == 0)
				{
					pDeal->RemoveByType(TRADE_ITEM_GOLD);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
				// Remove some of the Gold from the deal
				else
				{
					if(!pDeal->ChangeGoldTrade(eMyPlayer, iNumGoldInThisDeal))
					{
						CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove Gold from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
			}
		}
	}
}

/// Offer peace
bool CvDealAI::IsOfferPeace(PlayerTypes eOtherPlayer, CvDeal* pDeal, bool bEqualizingDeals)
{
	bool result = false;
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Can we actually complete this deal?

	if(!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eOtherPlayer, TRADE_ITEM_PEACE_TREATY))
	{
		return false;
	}
	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_PEACE_TREATY))
	{
		return false;
	}

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eOtherPlayer);
	PeaceTreatyTypes ePeaceTreatyImWillingToAccept = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToAccept(eOtherPlayer);

	// Peace between AI players
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		PeaceTreatyTypes ePeaceTreatyTheyreWillingToAccept = GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetTreatyWillingToAccept(eMyPlayer);
		PeaceTreatyTypes ePeaceTreatyTheyreWillingToOffer = GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetTreatyWillingToOffer(eMyPlayer);

		// Is what we're willing to offer acceptable to eOtherPlayer?
		if(ePeaceTreatyImWillingToOffer < ePeaceTreatyTheyreWillingToAccept)
		{
			return false;
		}
		// Is what eOtherPalyer is willing to offer acceptable to us?
		if(ePeaceTreatyTheyreWillingToOffer < ePeaceTreatyImWillingToAccept)
		{
			return false;
		}

		// If we're both willing to give something up (for whatever reason) reduce the surrender level of both parties until White Peace is on one side
		if(ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE && ePeaceTreatyTheyreWillingToOffer > PEACE_TREATY_WHITE_PEACE)
		{
			int iAmountToReduce = min(ePeaceTreatyImWillingToOffer, ePeaceTreatyTheyreWillingToOffer);

			ePeaceTreatyImWillingToOffer = PeaceTreatyTypes(ePeaceTreatyImWillingToOffer - iAmountToReduce);
			ePeaceTreatyTheyreWillingToOffer = PeaceTreatyTypes(ePeaceTreatyTheyreWillingToOffer - iAmountToReduce);
		}

		// Get the Peace in between if there's a gap
		if(ePeaceTreatyImWillingToOffer > ePeaceTreatyTheyreWillingToAccept)
		{
			ePeaceTreatyImWillingToOffer = PeaceTreatyTypes((ePeaceTreatyImWillingToOffer + ePeaceTreatyTheyreWillingToAccept) / 2);
		}
		if(ePeaceTreatyTheyreWillingToOffer > ePeaceTreatyImWillingToAccept)
		{
			ePeaceTreatyTheyreWillingToOffer = PeaceTreatyTypes((ePeaceTreatyTheyreWillingToOffer + ePeaceTreatyImWillingToAccept) / 2);
		}

		CvAssertMsg(ePeaceTreatyImWillingToOffer >= PEACE_TREATY_WHITE_PEACE, "DEAL_AI: I'm offering a peace treaty with negative ID.  Please show Jon");
		CvAssertMsg(ePeaceTreatyTheyreWillingToOffer >= PEACE_TREATY_WHITE_PEACE, "DEAL_AI: They're offering a peace treaty with negative ID.  Please show Jon");

		// I'm surrendering in this deal
		if(ePeaceTreatyImWillingToOffer > ePeaceTreatyTheyreWillingToOffer)
		{
			pDeal->SetSurrenderingPlayer(eMyPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToOffer);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyImWillingToOffer, /*bMeSurrendering*/ true);
		}
		// They're surrendering in this deal
		else if(ePeaceTreatyImWillingToOffer < ePeaceTreatyTheyreWillingToOffer)
		{
			pDeal->SetSurrenderingPlayer(eOtherPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyTheyreWillingToOffer);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyTheyreWillingToOffer, /*bMeSurrendering*/ false);
		}
		else
		{
			// Add the peace items to the deal so that we actually stop the war
			int iPeaceTreatyLength = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
			pDeal->AddPeaceTreaty(eMyPlayer, iPeaceTreatyLength);
			pDeal->AddPeaceTreaty(eOtherPlayer, iPeaceTreatyLength);
			DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);
		}

		result = true;
	}

	// Peace with a human
	else
	{
		// AI is surrendering
		if(ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE)
		{
			pDeal->SetSurrenderingPlayer(eMyPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToOffer);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyImWillingToOffer, /*bMeSurrendering*/ true);

			// Store the value of the deal with the human so that we have a number to use for renegotiation (if necessary)
			int iValueImOffering, iValueTheyreOffering;
#if defined(MOD_BALANCE_CORE)
			int iValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(iValue);
				m_pPlayer->SetCachedValueOfPeaceWithHuman(iValue);
			}
#else
			GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(-iValueImOffering);
			}
#endif
		}
		// AI is asking human to surrender
		else if(ePeaceTreatyImWillingToAccept > PEACE_TREATY_WHITE_PEACE)
		{
			pDeal->SetSurrenderingPlayer(eOtherPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToAccept);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyImWillingToAccept, /*bMeSurrendering*/ false);

			// Store the value of the deal with the human so that we have a number to use for renegotiation (if necessary)
			int iValueImOffering, iValueTheyreOffering;
#if defined(MOD_BALANCE_CORE)
			int iValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(iValue);
				m_pPlayer->SetCachedValueOfPeaceWithHuman(iValue);
			}
#else
			GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(iValueTheyreOffering);
			}
#endif
		}
		else
		{
			// if the case is that we both want white peace, don't forget to add the city-states into the peace deal.
			int iPeaceTreatyLength = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
			pDeal->AddPeaceTreaty(eMyPlayer, iPeaceTreatyLength);
			pDeal->AddPeaceTreaty(eOtherPlayer, iPeaceTreatyLength);
			DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);
		}
		result = true;
	}

	return result;
}

/// Add appropriate items to pDeal based on what type of PeaceTreaty eTreaty is
void CvDealAI::DoAddItemsToDealForPeaceTreaty(PlayerTypes eOtherPlayer, CvDeal* pDeal, PeaceTreatyTypes eTreaty, bool bMeSurrendering)
{
#if defined(MOD_BALANCE_CORE)
	if(eTreaty < PEACE_TREATY_ARMISTICE)
	{
		return;
	}
	int iDuration = GC.getGame().GetDealDuration();

	PlayerTypes eLosingPlayer = bMeSurrendering ? GetPlayer()->GetID() : eOtherPlayer;
	CvPlayer* pLosingPlayer = &GET_PLAYER(eLosingPlayer);
	PlayerTypes eWinningPlayer = bMeSurrendering ? eOtherPlayer : GetPlayer()->GetID();
	CvPlayer* pWinningPlayer = &GET_PLAYER(eWinningPlayer);
	pDeal->SetSurrenderingPlayer(eLosingPlayer);
	int iWarScore = pLosingPlayer->GetDiplomacyAI()->GetWarScore(eWinningPlayer);
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool bBecomeMyVassal = pLosingPlayer->GetDiplomacyAI()->IsVassalageAcceptable(eWinningPlayer, true);
	bool bRevokeMyVassals = false;
	// Reduce war score if losing player wants to become winning player's vassal
	if(MOD_DIPLOMACY_CIV4_FEATURES && bBecomeMyVassal)
	{
		iWarScore /= 2;
	}
	// Is losing player willing to revoke his vassals?
	if(MOD_DIPLOMACY_CIV4_FEATURES && iWarScore <= -85 && GET_TEAM(pLosingPlayer->getTeam()).GetNumVassals() > 0)
	{
		//If we're willing to do this, give less below.
		bRevokeMyVassals = true;
		iWarScore /= max(2, GET_TEAM(pLosingPlayer->getTeam()).GetNumVassals());
	}
	if(iWarScore < 0)
	{
		iWarScore *= -1;
	}
#endif

	//strategic warscore adjustment
	if (!pLosingPlayer->HasCityInDanger(false,0))
		iWarScore = max(0, iWarScore - 10);

	int iPercentGoldToGive = iWarScore;
	int iPercentGPTToGive = (iWarScore / 2);
	bool bGiveUpCities = (iWarScore > 60);
	int iPercentCitiesGiveUp = (iWarScore / 5);
	bool bGiveUpStratResources = (iWarScore > 35);
	bool bGiveUpLuxuryResources = (iWarScore > 15);
	int iGiveUpLuxResources = iWarScore;
	int iGiveUpStratResources = (iWarScore / 4);

	pDeal->AddPeaceTreaty(eWinningPlayer, GC.getGame().getGameSpeedInfo().getPeaceDealDuration());
	pDeal->AddPeaceTreaty(eLosingPlayer, GC.getGame().getGameSpeedInfo().getPeaceDealDuration());
	DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);

	CvCity* pLoopCity;
	int iCityLoop;
	// If the player only has one city then we can't get any more from him
	if (bGiveUpCities && iPercentCitiesGiveUp > 0 && pLosingPlayer->getNumCities() > 1)
	{
		int iTotalCityValue = 0;
		int iCityDistanceFromWinnersCapital = 0;

		// If winner has no capital then we can't use proximity - it will stay at 0
		CvCity* pWinnerCapital = pWinningPlayer->getCapitalCity();

		// Create vector of the losing players' Cities so we can see which are the closest to the winner
		CvWeightedVector<int> viCityProximities;

		// Loop through all of the loser's Cities, looking only at valid ones.
		for(pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
		{
			//skip the capital, it cannot be traded away
			if (pLoopCity->isCapital())
				continue;

			int iCurrentCityValue = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), bMeSurrendering, eOtherPlayer, /*bUseEvenValue*/ false, NULL);
			if (iCurrentCityValue == INT_MAX)
				continue;

			// If winner has no capital, Distance defaults to 0
			if(pWinnerCapital != NULL)
			{
				iCityDistanceFromWinnersCapital = plotDistance(pWinnerCapital->getX(), pWinnerCapital->getY(), pLoopCity->getX(), pLoopCity->getY());
			}

			// Divide the distance by three if the city was originally owned by the winning player to make these cities more likely
			if (pLoopCity->getOriginalOwner() == eWinningPlayer)
			{
				iCityDistanceFromWinnersCapital *= 5;
			}

			iCurrentCityValue += (iCityDistanceFromWinnersCapital * 10);

			//add up total city value of the loser (before danger and damage adjustment)
			iTotalCityValue += iCurrentCityValue;

			//Remember for later
			viCityProximities.push_back(pLoopCity->GetID(), iCurrentCityValue);
		}

		// Sort the vector based on distance from winner's capital
		viCityProximities.SortItems();
		int iSortedCityID;

		// Determine the value of Cities to be given up
		int iCityValueToSurrender = iTotalCityValue * iPercentCitiesGiveUp / 100;
		// Loop through sorted Cities and add them to the deal if they're under the amount to give up
		// Start from the back of the list, because that's where the cheapest cities are
		for(int iSortedCityIndex = viCityProximities.size() - 1; iSortedCityIndex > -1 ; iSortedCityIndex--)
		{
			iSortedCityID = viCityProximities.GetElement(iSortedCityIndex);
			pLoopCity = pLosingPlayer->getCity(iSortedCityID);

			int iCurrentCityValue = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), bMeSurrendering, eOtherPlayer, /*bUseEvenValue*/ false, NULL);
			if (iCurrentCityValue == INT_MAX)
				continue;

			// City is worth less than what is left to be added to the deal, so add it
			if (iCurrentCityValue < iCityValueToSurrender && iCurrentCityValue > 0)
			{
				pDeal->AddCityTrade(eLosingPlayer, iSortedCityID);
				iCityValueToSurrender -= iCurrentCityValue;

				//Frontline cities count more than they're worth. Ideally they should satisfy the winner?
				iCityValueToSurrender -= (pLoopCity->getDamage() * 10);
				if (pLosingPlayer->GetPlotDanger(*pLoopCity->plot(),pLoopCity) > 0)
					iCityValueToSurrender -= iCurrentCityValue / 10;
			}
		}
	}
	// Gold
	int iGold = 0;
	if (iPercentGoldToGive > 0)
	{
		iGold = pDeal->GetGoldAvailable(eLosingPlayer, TRADE_ITEM_GOLD);
		iGold = iGold * iPercentGoldToGive / 100;
		if(iGold > 0)
		{
			pDeal->AddGoldTrade(eLosingPlayer, iGold);
		}
	}

	// Gold per turn
	int iGPT = 0;
	if (iPercentGPTToGive > 0)
	{
		iGPT = pLosingPlayer->calculateGoldRate();
		int iGPTToGive = ((iGPT * iPercentGPTToGive) / 100);

		if (iGPTToGive >= iGPT-2)
		{
			iGPTToGive -= 2;
		}

		if (iGPTToGive > 0)
		{
			pDeal->AddGoldPerTurnTrade(eLosingPlayer, iGPTToGive, iDuration);
		}
	}

	// precalculate, it's expensive
	int iCurrentNetGoldOfReceivingPlayer = GET_PLAYER(eOtherPlayer).GetTreasury()->CalculateBaseNetGold();

	// Luxury Resources
	if(bGiveUpLuxuryResources && iGiveUpLuxResources > 0)
	{
		ResourceUsageTypes eUsage;
		ResourceTypes eResource;
		int iResourceQuantity = 1;
		int iTotalResourceValue = 0;
		CvWeightedVector<int> viResourceValue;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			eResource = (ResourceTypes) iResourceLoop;

			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if (pkResourceInfo == NULL)
				continue;

			eUsage = pkResourceInfo->getResourceUsage();

			// Can't trade bonus Resources
			if (eUsage != RESOURCEUSAGE_LUXURY)
			{
				continue;
			}

			iResourceQuantity = pLosingPlayer->getNumResourceAvailable(eResource, false);

			// Don't bother looking at this Resource if the other player doesn't even have any of it
			if (iResourceQuantity == 0)
			{
				continue;
			}
		
			// Can only get 1 copy of a Luxury
			if (eUsage == RESOURCEUSAGE_LUXURY)
			{
				iResourceQuantity = 1;
			}

			int iCurrentResourceValue = GetResourceValue(eResource, iResourceQuantity, GC.getGame().GetDealDuration(), true, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);

			if(iCurrentResourceValue == INT_MAX)
				continue;

			if(iCurrentResourceValue > 0)
			{
				// Get total city value of the loser
				iTotalResourceValue += iCurrentResourceValue;
				viResourceValue.push_back(eResource, iCurrentResourceValue);
			}
		}
		
		// Determine the value of Cities to be given up
		int iResourceValueToSurrender = (iTotalResourceValue * iGiveUpLuxResources) / 100;

		// Sort the vector based on value
		viResourceValue.SortItems();
		if(viResourceValue.size() > 0)
		{
			// Loop through sorted Cities and add them to the deal if they're under the amount to give up - start from the back of the list, because that's where the CLOSEST cities are
			for(int iSortedResourceIndex =  0; iSortedResourceIndex < viResourceValue.size(); iSortedResourceIndex++)
			{
				ResourceTypes eResourceList = (ResourceTypes)viResourceValue.GetElement(iSortedResourceIndex);

				int iCurrentResourceValue = GetResourceValue(eResourceList, 1,  GC.getGame().GetDealDuration(), true, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);

				if(iCurrentResourceValue == INT_MAX)
					continue;

				// City is worth less than what is left to be added to the deal, so add it
				if(iCurrentResourceValue <= iResourceValueToSurrender && iCurrentResourceValue > 0)
				{
					pDeal->AddResourceTrade(eLosingPlayer, eResourceList, 1, GC.getGame().GetDealDuration());
					iResourceValueToSurrender -= iCurrentResourceValue;
				}
			}
		}
	}
	// Strategic Resources
	if(bGiveUpStratResources && iGiveUpStratResources > 0)
	{
		ResourceUsageTypes eUsage;
		ResourceTypes eResource;
		int iResourceQuantity = 1;
		int iTotalResourceValue = 0;
		CvWeightedVector<int> viResourceValue;
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			eResource = (ResourceTypes) iResourceLoop;

			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if (pkResourceInfo == NULL)
				continue;

			eUsage = pkResourceInfo->getResourceUsage();

			// Can't trade bonus Resources
			if (eUsage != RESOURCEUSAGE_STRATEGIC)
			{
				continue;
			}

			iResourceQuantity = pLosingPlayer->getNumResourceAvailable(eResource, false);

			// Don't bother looking at this Resource if the other player doesn't even have any of it
			if (iResourceQuantity == 0)
			{
				continue;
			}
		
			if (iResourceQuantity > 3)
			{
				iResourceQuantity = 3;
			}

			int iCurrentResourceValue = GetResourceValue(eResource, iResourceQuantity, GC.getGame().GetDealDuration(), true, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);

			if(iCurrentResourceValue == INT_MAX)
				continue;

			if(iCurrentResourceValue > 0)
			{
				// Get total city value of the loser
				iTotalResourceValue += iCurrentResourceValue;
				viResourceValue.push_back(eResource, iCurrentResourceValue);
			}
		}
		
		// Determine the value of Cities to be given up
		int iResourceValueToSurrender = (iTotalResourceValue * iGiveUpStratResources) / 100;

		// Sort the vector based on distance from winner's capital
		viResourceValue.SortItems();
		if(viResourceValue.size() > 0)
		{
			// Loop through sorted Cities and add them to the deal if they're under the amount to give up - start from the back of the list, because that's where the CLOSEST cities are
			for(int iSortedResourceIndex =  0; iSortedResourceIndex < viResourceValue.size(); iSortedResourceIndex++)
			{
				ResourceTypes eResourceList = (ResourceTypes)viResourceValue.GetElement(iSortedResourceIndex);				
				iResourceQuantity = pLosingPlayer->getNumResourceAvailable(eResourceList, false);
				if (iResourceQuantity > 3)
				{
					iResourceQuantity = 3;
				}
				int iCurrentResourceValue = GetResourceValue(eResourceList, iResourceQuantity, GC.getGame().GetDealDuration(), true, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);

				if(iCurrentResourceValue == INT_MAX)
					continue;

				// City is worth less than what is left to be added to the deal, so add it
				if(iCurrentResourceValue < iResourceValueToSurrender && iCurrentResourceValue > 0)
				{
					pDeal->AddResourceTrade(eLosingPlayer, eResourceList, iResourceQuantity, GC.getGame().GetDealDuration());
					iResourceValueToSurrender -= iCurrentResourceValue;
				}
			}
		}
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES && bBecomeMyVassal)
	{
		pDeal->AddVassalageTrade(eLosingPlayer);
	}
	if(MOD_DIPLOMACY_CIV4_FEATURES && bRevokeMyVassals)
	{
		pDeal->AddRevokeVassalageTrade(eLosingPlayer);
	}
#endif
#else
	int iPercentGoldToGive = 0;
	int iPercentGPTToGive = 0;
	bool bGiveOpenBorders = false;
	bool bGiveOnlyOneCity = false;
	int iPercentCitiesGiveUp = 0; /* 100 = all but capital */
	bool bGiveUpStratResources = false;
	bool bGiveUpLuxuryResources = false;
	int iGiveUpLuxResources = 0;
	int iGiveUpStratResources = 0;

	// Setup what needs to be given up based on the level of the treaty
	switch (eTreaty)
	{
	case PEACE_TREATY_WHITE_PEACE:
		// White Peace: nothing changes hands
		break;

	case PEACE_TREATY_ARMISTICE:
		iPercentGoldToGive = 50;
		iPercentGPTToGive = 50;
		break;

	case PEACE_TREATY_SETTLEMENT:
		iPercentGoldToGive = 100;
		iPercentGPTToGive = 100;
		break;

	case PEACE_TREATY_BACKDOWN:
		iPercentGoldToGive = 100;
		iPercentGPTToGive = 100;
		bGiveOpenBorders = true;
		bGiveUpStratResources = true;
		break;

	case PEACE_TREATY_SUBMISSION:
		iPercentGoldToGive = 100;
		iPercentGPTToGive = 100;
		bGiveOpenBorders = true;
		bGiveUpStratResources = true;
		bGiveUpLuxuryResources = true;
		break;

	case PEACE_TREATY_SURRENDER:
		bGiveOnlyOneCity = true;
		break;

	case PEACE_TREATY_CESSION:
		iPercentCitiesGiveUp = 25;
		iPercentGoldToGive = 50;	
		break;

	case PEACE_TREATY_CAPITULATION:
		iPercentCitiesGiveUp = 33;
		iPercentGoldToGive = 100;
		break;

	case PEACE_TREATY_UNCONDITIONAL_SURRENDER:
		iPercentCitiesGiveUp = 100;
		iPercentGoldToGive = 100;
		break;
	}

	int iDuration = GC.getGame().GetDealDuration();

	PlayerTypes eLosingPlayer = bMeSurrendering ? GetPlayer()->GetID() : eOtherPlayer;
	CvPlayer* pLosingPlayer = &GET_PLAYER(eLosingPlayer);
	PlayerTypes eWinningPlayer = bMeSurrendering ? eOtherPlayer : GetPlayer()->GetID();
	CvPlayer* pWinningPlayer = &GET_PLAYER(eWinningPlayer);
	DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);


	CvCity* pLoopCity;
	int iCityLoop;
	// Gold
	int iGold = 0;
	if (iPercentGoldToGive > 0)
	{
		iGold = pDeal->GetGoldAvailable(eLosingPlayer, TRADE_ITEM_GOLD);
		if(iGold > 0)
		{
			iGold = iGold * iPercentGoldToGive / 100;

			if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_GOLD, iGold))
			{
				pDeal->AddGoldTrade(eLosingPlayer, iGold);
			}
		}
	}

	// Gold per turn
	int iGPT = 0;
	if (iPercentGPTToGive > 0)
	{
		iGPT = min(pLosingPlayer->calculateGoldRate(), pWinningPlayer->calculateGoldRate() / /*3*/ GC.getARMISTICE_GPT_DIVISOR());
		if (iGPT > 0)
		{
			iGPT = iGPT * iPercentGPTToGive / 100;

			if(iGPT > 0 && pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_GOLD_PER_TURN, iGPT, iDuration))
			{
				pDeal->AddGoldPerTurnTrade(eLosingPlayer, iGPT, iDuration);
			}
		}
	}

	// Open Borders
	if (bGiveOpenBorders)
	{
		if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_OPEN_BORDERS))
		{
			pDeal->AddOpenBorders(eLosingPlayer, iDuration);
		}
	}

	// Resources
	ResourceUsageTypes eUsage;
	ResourceTypes eResource;
	int iResourceQuantity;
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo == NULL)
			continue;

		eUsage = pkResourceInfo->getResourceUsage();

		// Can't trade bonus Resources
		if(eUsage == RESOURCEUSAGE_BONUS)
		{
			continue;
		}

		iResourceQuantity = pLosingPlayer->getNumResourceAvailable(eResource, false);

		// Don't bother looking at this Resource if the other player doesn't even have any of it
		if (iResourceQuantity == 0)
		{
			continue;
		}

		// Match with deal type
		if (eUsage == RESOURCEUSAGE_LUXURY && !bGiveUpLuxuryResources)
		{
			continue;
		}

		if (eUsage == RESOURCEUSAGE_STRATEGIC && !bGiveUpStratResources)
		{
			continue;
		}

		// Can only get 1 copy of a Luxury
		if (eUsage == RESOURCEUSAGE_LUXURY)
		{
			iResourceQuantity = 1;
		}

		if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
		{
			pDeal->AddResourceTrade(eLosingPlayer, eResource, iResourceQuantity, iDuration);
		}
	}

	//	Give up all but capital?
	if (iPercentCitiesGiveUp == 100)
	{
		// All Cities but the capital
		for(pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
		{
			if(pLoopCity->isCapital())
			{
				continue;
			}

			if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
			{
				pDeal->AddCityTrade(eLosingPlayer, pLoopCity->GetID());
			}
		}
	}
	// If the player only has 1 City then we can't get any more from him
	else if (iPercentCitiesGiveUp > 0 || bGiveOnlyOneCity && pLosingPlayer->getNumCities() > 1)
	{
		int iCityValue = 0;
		int iCityDistanceFromWinnersCapital = 0;
		int iWinnerCapitalX = -1, iWinnerCapitalY = -1;

		// If winner has no capital then we can't use proximity - it will stay at 0
		CvCity* pWinnerCapital = pWinningPlayer->getCapitalCity();
		if(pWinnerCapital != NULL)
		{
			iWinnerCapitalX = pWinnerCapital->getX();
			iWinnerCapitalY = pWinnerCapital->getY();
		}

		// Create vector of the losing players' Cities so we can see which are the closest to the winner
		CvWeightedVector<int> viCityProximities;

		// Loop through all of the loser's Cities
		for(pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
		{
			// Get total city value of the loser
			iCityValue += GetCityValue(pLoopCity->getX(), pLoopCity->getY(), bMeSurrendering, eOtherPlayer, /*bUseEvenValue*/ true);

			// If winner has no capital, Distance defaults to 0
			if(pWinnerCapital != NULL)
			{
				iCityDistanceFromWinnersCapital = plotDistance(iWinnerCapitalX, iWinnerCapitalY, pLoopCity->getX(), pLoopCity->getY());
			}

			// Divide the distance by three if the city was originally owned by the winning player to make these cities more likely
			if (pLoopCity->getOriginalOwner() == eWinningPlayer)
			{
				iCityDistanceFromWinnersCapital /= 3;
			}

			// Don't include the capital in the list of Cities the winner can receive
			if(!pLoopCity->isCapital())
			{
				viCityProximities.push_back(pLoopCity->GetID(), iCityDistanceFromWinnersCapital);
			}
		}

		// Sort the vector based on distance from winner's capital
		viCityProximities.SortItems();
		int iSortedCityID;

		// Just one city?
		if (bGiveOnlyOneCity)
		{
			iSortedCityID = viCityProximities.GetElement(viCityProximities.size() - 1);
			pDeal->AddCityTrade(eLosingPlayer, iSortedCityID);
		}

		else
		{
			// Determine the value of Cities to be given up
			int iCityValueToSurrender = iCityValue * iPercentCitiesGiveUp / 100;

			// Loop through sorted Cities and add them to the deal if they're under the amount to give up - start from the back of the list, because that's where the CLOSEST cities are
			for(int iSortedCityIndex = viCityProximities.size() - 1; iSortedCityIndex > -1 ; iSortedCityIndex--)
			{
				iSortedCityID = viCityProximities.GetElement(iSortedCityIndex);
				pLoopCity = pLosingPlayer->getCity(iSortedCityID);

				iCityValue = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), bMeSurrendering, eOtherPlayer, /*bUseEvenValue*/ true);

				// City is worth less than what is left to be added to the deal, so add it
				if(iCityValue < iCityValueToSurrender)
				{
					if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
					{
						pDeal->AddCityTrade(eLosingPlayer, iSortedCityID);
						iCityValueToSurrender -= iCityValue;
					}
				}
			}
		}
	}
#endif
}

/// What are we willing to give/receive for peace with the active human player?
int CvDealAI::GetCachedValueOfPeaceWithHuman()
{
	return m_iCachedValueOfPeaceWithHuman;		// NOT SERIALIZED
}

/// Sets what are we willing to give/receive for peace with the active human player
void CvDealAI::SetCachedValueOfPeaceWithHuman(int iValue)
{
	m_iCachedValueOfPeaceWithHuman = iValue;		// NOT SERIALIZED
}

/// Add third party peace for allied city-states
void CvDealAI::DoAddPlayersAlliesToTreaty(PlayerTypes eToPlayer, CvDeal* pDeal)
{
	int iPeaceDuration = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
	PlayerTypes eMinor;
	CvPlayer* pMinor;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;
#if defined(MOD_BALANCE_CORE)
		if(eMinor == NO_PLAYER)
		{
			continue;
		}
#endif
		pMinor = &GET_PLAYER(eMinor);

		// Minor not alive?
		if(!pMinor->isAlive())
			continue;

		PlayerTypes eAlly = pMinor->GetMinorCivAI()->GetAlly();
		// ally of other player
		if (eAlly == eToPlayer)
		{
			// if they are not at war with us, continue
			if (!GET_TEAM(GetTeam()).isAtWar(pMinor->getTeam()))
			{
				continue;
			}

			// if they are always at war with us, continue
#if defined(MOD_BALANCE_CORE)
			if (pMinor->GetMinorCivAI()->IsPermanentWar(GetTeam()))
#else
			if (pMinor->GetMinorCivAI()->IsPermanentWar(GetTeam()) || pMinor->GetMinorCivAI()->IsPeaceBlocked(GetTeam()))
#endif
			{
				continue;
			}

			// Add peace with this minor to the deal
			// slewis - if there is not a peace deal with them already on the table and we can trade it
			if(!pDeal->IsThirdPartyPeaceTrade(GetPlayer()->GetID(), pMinor->getTeam()))
			{	
				pDeal->AddThirdPartyPeace(GetPlayer()->GetID(), pMinor->getTeam(), iPeaceDuration);
			}
		}
		// ally with us
		else if (eAlly == GetPlayer()->GetID())
		{
			// if they are not at war with the opponent, continue
			if (!GET_TEAM(GET_PLAYER(eToPlayer).getTeam()).isAtWar(pMinor->getTeam()))
			{
				continue;
			}

			// if they are always at war with them, continue
#if defined(MOD_BALANCE_CORE)
			if (pMinor->GetMinorCivAI()->IsPermanentWar(GET_PLAYER(eToPlayer).getTeam()))
#else
			if (pMinor->GetMinorCivAI()->IsPermanentWar(GET_PLAYER(eToPlayer).getTeam()) || pMinor->GetMinorCivAI()->IsPeaceBlocked(GET_PLAYER(eToPlayer).getTeam()))
#endif
			{
				continue;
			}

			// Add peace with this minor to the deal
			// slewis - if there is not a peace deal with them already on the table and we can trade it
			if(!pDeal->IsThirdPartyPeaceTrade(eToPlayer, pMinor->getTeam()))
			{
				pDeal->AddThirdPartyPeace(eToPlayer, pMinor->getTeam(), iPeaceDuration);
			}
		}
	}
}

/// AI making a demand of eOtherPlayer
bool CvDealAI::IsMakeDemand(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Set that this CvDeal is a demand
	pDeal->SetDemandingPlayer(GetPlayer()->GetID());
#if defined(MOD_BALANCE_CORE)
	int iIdealValue = 200 * (GetPlayer()->GetDiplomacyAI()->GetMeanness() + GetPlayer()->GetCurrentEra());
	int Value = NUM_STRENGTH_VALUES - (int)GetPlayer()->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer);
	if (Value > 0)
	{
		iIdealValue *= Value;
	}
	bool bDontChangeTheirExistingItems = false;
	int iDealDuration = GC.getGame().GetDealDuration();
	bool bUseEvenValue = false;
	int iTotalValueToMe, iValueImOffering, iValueTheyreOffering;
	int iAmountOverWeWillRequest = 0;
	iTotalValueToMe = (iIdealValue * -1);
	DoAddResourceToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
	if(iValueTheyreOffering >= iIdealValue && pDeal->m_TradedItems.size() > 0)
	{
		return true;
	}
	DoAddGPTToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iDealDuration, bUseEvenValue);
	if(iValueTheyreOffering >= iIdealValue && pDeal->m_TradedItems.size() > 0)
	{
		return true;
	}
	DoAddGoldToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
	if(iValueTheyreOffering >= iIdealValue && pDeal->m_TradedItems.size() > 0)
	{
		return true;
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES) {
		// AI would rather offer human resources/open borders than techs/maps
		DoAddTechToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
		if(iValueTheyreOffering >= iIdealValue && pDeal->m_TradedItems.size() > 0)
		{
			return true;
		}
		DoAddMapsToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
		if(iValueTheyreOffering >= iIdealValue && pDeal->m_TradedItems.size() > 0)
		{
			return true;
		}
	}
#endif
	DoAddCitiesToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
	if(iValueTheyreOffering >= iIdealValue && pDeal->m_TradedItems.size() > 0)
	{
		return true;
	}
	DoAddThirdPartyWarToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
	if(iValueTheyreOffering >= iIdealValue && pDeal->m_TradedItems.size() > 0)
	{
		return true;
	}
	DoAddThirdPartyPeaceToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
	if (iValueTheyreOffering >= iIdealValue && pDeal->m_TradedItems.size() > 0)
	{
		return true;
	}
#else
	int iGold = pDeal->GetGoldAvailable(eOtherPlayer, TRADE_ITEM_GOLD);

	// Don't ask for too much
	int iMaxGold = 200 + (GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetCurrentEra() * 150);
	iGold = min(iMaxGold, iGold);

	if(pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_GOLD, iGold))
	{
		pDeal->AddGoldTrade(eOtherPlayer, iGold);

		return true;
	}
#endif
	return false;
}

/// A good time to make an offer for someone's extra Luxury?
bool CvDealAI::IsMakeOfferForLuxuryResource(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	ResourceTypes eLuxuryFromThem = NO_RESOURCE;

	// Don't ask for a Luxury if we're hostile or planning a war
	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR)
	{
		return false;
	}

	int iResourceLoop;
	ResourceTypes eResource;
#if defined(MOD_BALANCE_CORE)
	int iBestValue = 20;
#endif

	// precalculate, it's expensive
	int iCurrentNetGoldOfReceivingPlayer = m_pPlayer->GetTreasury()->CalculateBaseNetGold();

	// See if the other player has a Resource to trade
	for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		// Only look at Luxuries
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
		{
			continue;
		}

		// Must not be banned by World Congress
		if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(GetPlayer()->GetID(), eResource))
		{
			continue;
		}

		// Any extras?
		if(GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eResource, false) > 1)
		{
#if defined(MOD_BALANCE_CORE)
			//Let's try to get their best resource :)
			int iItemValue = GetResourceValue(eResource, 1, GC.getGame().GetDealDuration(), false, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);
			if(iItemValue!=INT_MAX && iItemValue > iBestValue)
			{
				eLuxuryFromThem = eResource;
				iBestValue = iItemValue;
			}
#else
			eLuxuryFromThem = eResource;
			break;
#endif
		}
	}

	// Extra Luxury found!
	if(eLuxuryFromThem != NO_RESOURCE)
	{
		// Can we actually complete this deal?
		if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eLuxuryFromThem, 1))
		{
			return false;
		}

		// Seed the deal with the item we want
		pDeal->AddResourceTrade(eOtherPlayer, eLuxuryFromThem, 1, GC.getGame().GetDealDuration());

		bool bDealAcceptable = false;

		// AI evaluation
		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, /*bDontChangeTheirExistingItems*/ true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;
	}
	return false;
}
#if defined(MOD_BALANCE_CORE)
/// A good time to make an offer for someone's extra strats?
bool CvDealAI::IsMakeOfferForStrategicResource(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	ResourceTypes eStratFromThem = NO_RESOURCE;

	// Don't ask for a Luxury if we're hostile or planning a war
	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR)
	{
		return false;
	}

	int iResourceLoop;
	ResourceTypes eResource;
	int iBestValue = 15;

	// precalculate, it's expensive
	int iCurrentNetGoldOfReceivingPlayer = m_pPlayer->GetTreasury()->CalculateBaseNetGold();
	int iRand = 0;
	// See if the other player has a Resource to trade
	for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		eResource = (ResourceTypes) iResourceLoop;

		// Only look at Strats
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_STRATEGIC)
		{
			continue;
		}


		// Any extras?
		if(GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eResource, false) > 3 && GetPlayer()->getNumResourceAvailable(eResource, true) <= 0)
		{
			int iNum = GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eResource, false);
			iRand = GC.getGame().getSmallFakeRandNum(max(iNum, 10), eResource);
			iRand /= 2;
			if(iRand <= 0)
			{
				iRand = 1;
			}
			//Let's try to get their best resource :)
			int iItemValue = GetResourceValue(eResource, iRand, GC.getGame().GetDealDuration(), false, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);
			if(iItemValue!=INT_MAX && iItemValue > iBestValue)
			{
				eStratFromThem = eResource;
				iBestValue = iItemValue;
			}
		}
	}

	// Extra Strat found!
	if(eStratFromThem != NO_RESOURCE && iRand > 0)
	{
		// Can we actually complete this deal?
		if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eStratFromThem, iRand))
		{
			return false;
		}

		// Seed the deal with the item we want
		pDeal->AddResourceTrade(eOtherPlayer, eStratFromThem, iRand, GC.getGame().GetDealDuration());

		bool bDealAcceptable = false;

		// AI evaluation
		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, /*bDontChangeTheirExistingItems*/ true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;;
	}
	return false;
}
#endif
/// A good time to make an offer to get an embassy?
bool CvDealAI::MakeOfferForEmbassy(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for Open Borders if we're hostile or planning war
	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR		||
	        eApproach == MAJOR_CIV_APPROACH_GUARDED)
	{
		return false;
	}

	// Can we actually complete this deal?
	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_ALLOW_EMBASSY))
	{
		return false;
	}

	// Do we actually want OB with eOtherPlayer?
	if(GetPlayer()->GetDiplomacyAI()->WantsEmbassyAtPlayer(eOtherPlayer))
	{
		// Seed the deal with the item we want
		pDeal->AddAllowEmbassy(eOtherPlayer);
		bool bDealAcceptable = false;

		// AI evaluation
		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;;
	}

	return false;
}

/// A good time to make an offer to get Open Borders?
bool CvDealAI::IsMakeOfferForOpenBorders(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for Open Borders if we're hostile or planning war
	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR)
	{
		return false;
	}

	// Can we actually complete this deal?
	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_OPEN_BORDERS))
	{
		return false;
	}

#if defined(MOD_BALANCE_CORE)
	//Already allows?
	if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsAllowsOpenBordersToTeam(GetPlayer()->getTeam()))
	{
		return false;
	}
#endif

	// Do we actually want OB with eOtherPlayer?
	if(GetPlayer()->GetDiplomacyAI()->IsWantsOpenBordersWithPlayer(eOtherPlayer))
	{
		// Seed the deal with the item we want
		pDeal->AddOpenBorders(eOtherPlayer, GC.getGame().GetDealDuration());

		bool bDealAcceptable = false;

		// AI evaluation
		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;;
	}

	return false;
}

/// A good time to make an offer for a Research Agreement?
bool CvDealAI::IsMakeOfferForResearchAgreement(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Logic for when THIS AI wants to make a RA is in the Diplo AI

	// Can we actually complete this deal?
	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESEARCH_AGREEMENT))
	{
		return false;
	}

	// Seed the deal with the item we want
	pDeal->AddResearchAgreement(GetPlayer()->GetID(), GC.getGame().GetDealDuration());
	pDeal->AddResearchAgreement(eOtherPlayer, GC.getGame().GetDealDuration());

	bool bDealAcceptable = false;

	// AI evaluation
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
	}
	else
	{
		bool bUselessReferenceVariable;
		bool bCantMatchOffer;
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;;
}
#if defined(MOD_BALANCE_CORE_DEALS)
/// A good time to make an offer for a Defensive Pact?
bool CvDealAI::IsMakeOfferForDefensivePact(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Logic for when THIS AI wants to make a RA is in the Diplo AI

	// Can we actually complete this deal?
	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_DEFENSIVE_PACT))
	{
		return false;
	}

	if(!GetPlayer()->GetDiplomacyAI()->IsWantsDefensivePactWithPlayer(eOtherPlayer))
	{
		return false;
	}

	// Seed the deal with the item we want
	pDeal->AddDefensivePact(GetPlayer()->GetID(), GC.getGame().GetDealDuration());
	pDeal->AddDefensivePact(eOtherPlayer, GC.getGame().GetDealDuration());

	bool bDealAcceptable = false;

	// AI evaluation
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
	}
	else
	{
		bool bUselessReferenceVariable;
		bool bCantMatchOffer;

		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;;
}

/// A good time to make an offer to buy a city?
bool CvDealAI::IsMakeOfferForCity(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for a city if we're hostile or planning a war
	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE || eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_GUARDED)
	{
		return false;
	}

	int iCityLoop;
	CvCity* pBestBuyCity = NULL;
	int iBestBuyCity = 100; //initial value, must be a fair deal for us!

	//check their cities
	for(CvCity* pTheirCity = GET_PLAYER(eOtherPlayer).firstCity(&iCityLoop); pTheirCity != NULL; pTheirCity = GET_PLAYER(eOtherPlayer).nextCity(&iCityLoop))
	{
		if(!pTheirCity)
			continue;

		if(pDeal->IsPossibleToTradeItem(eOtherPlayer, m_pPlayer->GetID(), TRADE_ITEM_CITIES, pTheirCity->getX(), pTheirCity->getY()))
		{
			int iTheirPrice = GetCityValue(pTheirCity->getX(), pTheirCity->getY(), true, eOtherPlayer, false, pDeal);
			int iMyPrice = GetCityValue(pTheirCity->getX(), pTheirCity->getY(), false, eOtherPlayer, false, pDeal);

			int iBuyRatio = (iMyPrice*100)/max(1,iTheirPrice);
			if(iMyPrice!=INT_MAX && iTheirPrice!=INT_MAX && iBuyRatio > iBestBuyCity)
			{
				pBestBuyCity = pTheirCity;
				iBestBuyCity = iBuyRatio;
			}
		}
	}

	if(pBestBuyCity == NULL)
	{
		return false;
	}
	else
	{
		pDeal->AddCityTrade(eOtherPlayer, pBestBuyCity->GetID());
	}

	bool bDealAcceptable = false;

	// AI evaluation
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
	}
	else
	{
		bool bUselessReferenceVariable;
		bool bCantMatchOffer;
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;;
}

/// A good time to make an offer to buy or sell a city?
bool CvDealAI::IsMakeOfferForCityExchange(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for a city if we're hostile or planning a war
	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE || eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_GUARDED)
	{
		return false;
	}

	int iCityLoop;
	CvCity* pBestBuyCity = NULL;
	int iBestBuyCity = 120; //initial value, deal must be good to justify building a courthouse 
	CvCity* pBestSellCity = NULL;
	int iBestSellCity = 150; //initial value, they must overpay a lot

	//check their cities
	for(CvCity* pTheirCity = GET_PLAYER(eOtherPlayer).firstCity(&iCityLoop); pTheirCity != NULL; pTheirCity = GET_PLAYER(eOtherPlayer).nextCity(&iCityLoop))
	{
		if(!pTheirCity)
			continue;

		if(pDeal->IsPossibleToTradeItem(eOtherPlayer, m_pPlayer->GetID(), TRADE_ITEM_CITIES, pTheirCity->getX(), pTheirCity->getY()))
		{
			int iTheirPrice = GetCityValue(pTheirCity->getX(), pTheirCity->getY(), true, eOtherPlayer, false, pDeal);
			int iMyPrice = GetCityValue(pTheirCity->getX(), pTheirCity->getY(), false, eOtherPlayer, false, pDeal);

			int iBuyRatio = (iMyPrice*100)/max(1,iTheirPrice);
			if(iMyPrice!=INT_MAX && iTheirPrice!=INT_MAX && iBuyRatio>iBestBuyCity)
			{
				pBestBuyCity = pTheirCity;
				iBestBuyCity = iBuyRatio;
			}
		}
	}
	
	if (pBestBuyCity == NULL)
	{
		pDeal->ClearItems();
		return false;
	}
	else
	{
		pDeal->AddCityTrade(eOtherPlayer, pBestBuyCity->GetID());
	}

	//check my cities
	for(CvCity* pMyCity = GetPlayer()->firstCity(&iCityLoop); pMyCity != NULL; pMyCity = GetPlayer()->nextCity(&iCityLoop))
	{
		if(!pMyCity)
			continue;

		if(pDeal->IsPossibleToTradeItem(m_pPlayer->GetID(), eOtherPlayer, TRADE_ITEM_CITIES, pMyCity->getX(), pMyCity->getY()))
		{
			int iTheirPrice = GetCityValue(pMyCity->getX(), pMyCity->getY(), true, eOtherPlayer, false, pDeal);
			int iMyPrice = GetCityValue(pMyCity->getX(), pMyCity->getY(), false, eOtherPlayer, false, pDeal);

			int iSellRatio = (iTheirPrice*100)/max(1,iMyPrice);
			if(iMyPrice!=INT_MAX && iTheirPrice!=INT_MAX && iSellRatio>iBestSellCity)
			{
				pBestSellCity = pMyCity;
				iBestSellCity = iSellRatio;
			}
		}
	}

	if(pBestSellCity == NULL)
	{
		pDeal->ClearItems();
		return false;
	}
	else
	{
		//ok, everything is good
		pDeal->AddCityTrade(m_pPlayer->GetID(), pBestSellCity->GetID());
	}

	bool bDealAcceptable = false;

	// AI evaluation
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
	}
	else
	{
		bool bUselessReferenceVariable;
		bool bCantMatchOffer;
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;;
}
/// A good time to make an offer to start a war?
bool CvDealAI::IsMakeOfferForThirdPartyWar(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	if(eOtherPlayer == NO_PLAYER)
	{
		return false;
	}

	// Don't ask for a war if we're hostile or planning a war
	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE || eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_GUARDED)
	{
		return false;
	}

	//If we aren't at war, also buck on deceptive. Otherwise we want allies ASAP.
	if (!GetPlayer()->IsAtWarAnyMajor() && eApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		return false;

	// Don't ask for war if they are weaker than us
	if (GetPlayer()->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer) < STRENGTH_AVERAGE && GetPlayer()->GetDiplomacyAI()->GetPlayerEconomicStrengthComparedToUs(eOtherPlayer))
	{
		return false;
	}

	// Don't ask for war if we are in a DoF (we'll do the 'free' method instead)
	if(GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eOtherPlayer))
	{
		return false;
	}

	//Don't offer if we have a DP.
	if (GET_TEAM(GetPlayer()->getTeam()).IsHasDefensivePact(GET_PLAYER(eOtherPlayer).getTeam()))
	{
		return false;
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	//Asking a vassal? Abort!
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsVassalOfSomeone())
	{
		return false;
	}
	//Vassals asking? Nope!
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GetPlayer()->getTeam()).IsVassalOfSomeone())
	{
		return false;
	}
#endif

	int iWarValue = 0;
	int iBestValue = 0;
	TeamTypes eBestTeam = NO_TEAM;

	// find the first player associated with the team
	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		PlayerTypes eAgainstPlayer = (PlayerTypes)iI;
		if (eAgainstPlayer == NO_PLAYER)
		{
			continue;
		}
		//Is minor? Don't do it! (Too easy to wiggle out of)
		if(GET_PLAYER(eAgainstPlayer).isMinorCiv())
		{
			continue;
		}
		//Is our major approach friendly? Don't do it!
		if(GET_PLAYER(eAgainstPlayer).isMajorCiv() && GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eAgainstPlayer, false) > MAJOR_CIV_APPROACH_GUARDED)
		{
			continue;
		}
		// Can we actually complete this deal?
		if(pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_THIRD_PARTY_WAR, GET_PLAYER(eAgainstPlayer).getTeam()))
		{
			iWarValue = GetThirdPartyWarValue(false, eOtherPlayer, GET_PLAYER(eAgainstPlayer).getTeam());
			if(iWarValue != INT_MAX && iWarValue > iBestValue)
			{
				eBestTeam = GET_PLAYER(eAgainstPlayer).getTeam();
				iBestValue = iWarValue;
			}	
		}
	}

	if(eBestTeam == NO_TEAM)
	{
		return false;
	}
	else
	{
		pDeal->AddThirdPartyWar(eOtherPlayer, eBestTeam);
	}

	bool bDealAcceptable = false;

	// AI evaluation
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
	}
	else
	{
		bool bUselessReferenceVariable;
		bool bCantMatchOffer;
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;;
}
/// A good time to make an offer for a Peace Deal?
bool CvDealAI::IsMakeOfferForThirdPartyPeace(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	
	if(eOtherPlayer == NO_PLAYER)
	{
		return false;
	}
	//Friends? We overlook friendly wars.
	if(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false) == MAJOR_CIV_APPROACH_FRIENDLY)
	{
		return false;
	}
	// Don't ask for peace if we are in a DoF
	if(GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eOtherPlayer))
	{
		return false;
	}
	
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	//Asking a vassal? Abort!
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsVassalOfSomeone())
	{
		return false;
	}
	//Vassals asking? Nope!
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GetPlayer()->getTeam()).IsVassalOfSomeone())
	{
		return false;
	}
#endif

	int iWarValue = 0;
	int iBestValue = 0;
	TeamTypes eBestTeam = NO_TEAM;

	// find the first player associated with the team
	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		PlayerTypes eAgainstPlayer = (PlayerTypes)iI;
		if (eAgainstPlayer == NO_PLAYER)
		{
			continue;
		}
		//Minor Civ? Only if allies or protective
		if(GET_PLAYER(eAgainstPlayer).isMinorCiv() && (!GET_PLAYER(eAgainstPlayer).GetMinorCivAI()->IsAllies(GetPlayer()->GetID())))
		{
			continue;
		}
		//Minor civ? Only if we're not out for conquest.
		if(GET_PLAYER(eAgainstPlayer).isMinorCiv() && (GetPlayer()->GetDiplomacyAI()->GetMinorCivApproach(eAgainstPlayer) == MINOR_CIV_APPROACH_CONQUEST))
		{
			continue;
		}
		//Is our approach of the player we're asking about bad? Don't do it!
		if(!GET_PLAYER(eAgainstPlayer).isMinorCiv() && GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eAgainstPlayer, false) < MAJOR_CIV_APPROACH_DECEPTIVE)
		{
			continue;
		}
		// Can we actually complete this deal?
		if(pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_THIRD_PARTY_PEACE, GET_PLAYER(eAgainstPlayer).getTeam()))
		{
			iWarValue = GetThirdPartyPeaceValue(false, eOtherPlayer, GET_PLAYER(eAgainstPlayer).getTeam());
			if(iWarValue!=INT_MAX && iWarValue > iBestValue)
			{
				eBestTeam = GET_PLAYER(eAgainstPlayer).getTeam();
				iBestValue = iWarValue;
			}	
		}
	}
	int iPeaceDuration = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
	if(eBestTeam == NO_TEAM)
	{
		return false;
	}
	else
	{
		pDeal->AddThirdPartyPeace(eOtherPlayer, eBestTeam, iPeaceDuration);
	}

	bool bDealAcceptable = false;

	// AI evaluation
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
	}
	else
	{
		bool bUselessReferenceVariable;
		bool bCantMatchOffer;
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;;
}
/// A good time to make an offer for a Peace Deal?
bool CvDealAI::IsMakeOfferForVote(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if(!pLeague)
	{
		return false;
	}
	
	if(eOtherPlayer == NO_PLAYER)
	{
		return false;
	}
	//Friends?
	if(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false) < MAJOR_CIV_APPROACH_AFRAID)
	{
		return false;
	}
	
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	//Asking a vassal? Abort!
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsVassalOfSomeone())
	{
		return false;
	}
	//Vassals asking? Nope!
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GetPlayer()->getTeam()).IsVassalOfSomeone())
	{
		return false;
	}
#endif
	int iBestValue = 0;
	int iBestProposal = -1;
	int iProposalID = -1;
	int iVoteChoice = -1;
	int iNumVotes = -1;
	bool bRepeal = false;
	CvLeagueAI::VoteCommitmentList vDesiredCommitments = GetPlayer()->GetLeagueAI()->GetDesiredVoteCommitments(eOtherPlayer);
	for (CvLeagueAI::VoteCommitmentList::iterator it = vDesiredCommitments.begin(); it != vDesiredCommitments.end(); ++it)
	{
		iProposalID = it->iResolutionID;
		iVoteChoice = it->iVoteChoice;
		iNumVotes = it->iNumVotes;
		bRepeal = !it->bEnact;

		if (iProposalID != -1 && pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_VOTE_COMMITMENT, iProposalID, iVoteChoice, iNumVotes, bRepeal))
		{
			int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ false, eOtherPlayer, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, false);
			if(iItemValue!=INT_MAX && iItemValue > iBestValue)
			{
				iBestProposal = iProposalID;
				iBestValue = iItemValue;
			}
		}
	}

	if(iBestProposal == -1)
	{
		return false;
	}
	else
	{
		pDeal->AddVoteCommitment(eOtherPlayer, iProposalID, iVoteChoice, iNumVotes, bRepeal);
	}

	bool bDealAcceptable = false;

	// AI evaluation
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
	}
	else
	{
		bool bUselessReferenceVariable;
		bool bCantMatchOffer;
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;;
}

#endif
/// This function called when human player enters diplomacy
void CvDealAI::DoTradeScreenOpened()
{
	TeamTypes eActiveTeam = GC.getGame().getActiveTeam();
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	if(GET_TEAM(GetTeam()).isAtWar(eActiveTeam))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();
#if defined(MOD_BALANCE_CORE)
		GetPlayer()->GetDiplomacyAI()->DoUpdatePeaceTreatyWillingness();
		m_pPlayer->SetCachedValueOfPeaceWithHuman(0);
#endif
		PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eActivePlayer);
		PeaceTreatyTypes ePeaceTreatyImWillingToAccept = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToAccept(eActivePlayer);

		// Does the AI actually want peace?
		if(ePeaceTreatyImWillingToOffer >= PEACE_TREATY_WHITE_PEACE && ePeaceTreatyImWillingToAccept >= PEACE_TREATY_WHITE_PEACE)
		{
			// Clear out UI deal first, we're going to add a couple things to it
			auto_ptr<ICvDeal1> pUIDeal(GC.GetEngineUserInterface()->GetScratchDeal());
			CvDeal* pkUIDeal = GC.UnwrapDealPointer(pUIDeal.get());
			pkUIDeal->ClearItems();

			CvDeal* pDeal = GC.getGame().GetGameDeals().GetTempDeal();
			pDeal->ClearItems();
			pDeal->SetFromPlayer(eActivePlayer);	// The order of these is very important!
			pDeal->SetToPlayer(eMyPlayer);	// The order of these is very important!

			// AI is surrendering
			if(ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE)
			{
				pkUIDeal->SetSurrenderingPlayer(eMyPlayer);
				pkUIDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToOffer);

				DoAddItemsToDealForPeaceTreaty(eActivePlayer, pDeal, ePeaceTreatyImWillingToOffer, /*bMeSurrendering*/ true);
#if defined(MOD_BALANCE_CORE)
				DoAddPlayersAlliesToTreaty(eActivePlayer, pDeal);
#endif
				// Store the value of the deal with the human so that we have a number to use for renegotiation (if necessary)	
				int iValueImOffering, iValueTheyreOffering;
#if defined(MOD_BALANCE_CORE)
				int iValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
				if(iValue < 0)
				{
					iValue *= 1;
				}
				SetCachedValueOfPeaceWithHuman(iValue);
				m_pPlayer->SetCachedValueOfPeaceWithHuman(iValue);
#else			
				GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
				SetCachedValueOfPeaceWithHuman(-iValueImOffering);
#endif
			}
			// AI is asking human to surrender
			else if(ePeaceTreatyImWillingToAccept > PEACE_TREATY_WHITE_PEACE)
			{
				pkUIDeal->SetSurrenderingPlayer(eActivePlayer);
				pkUIDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToAccept);

				DoAddItemsToDealForPeaceTreaty(eActivePlayer, pDeal, ePeaceTreatyImWillingToAccept, /*bMeSurrendering*/ false);
#if defined(MOD_BALANCE_CORE)
				DoAddPlayersAlliesToTreaty(eActivePlayer, pDeal);
#endif
				// Store the value of the deal with the human so that we have a number to use for renegotiation (if necessary)
				int iValueImOffering, iValueTheyreOffering;
#if defined(MOD_BALANCE_CORE)
				int iValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
				SetCachedValueOfPeaceWithHuman(iValue);
				m_pPlayer->SetCachedValueOfPeaceWithHuman(iValue);
#else
				GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
				SetCachedValueOfPeaceWithHuman(iValueTheyreOffering);
#endif
			}
			pDeal->ClearItems();

			// Now add peace items to the UI deal so that it's ready for us to make an offer
			pkUIDeal->SetFromPlayer(eActivePlayer);	// The order of these is very important!
			pkUIDeal->SetToPlayer(eMyPlayer);	// The order of these is very important!
			pkUIDeal->AddPeaceTreaty(eMyPlayer, GC.getGame().getGameSpeedInfo().getPeaceDealDuration());
			pkUIDeal->AddPeaceTreaty(eActivePlayer, GC.getGame().getGameSpeedInfo().getPeaceDealDuration());
			
			// slewis - adding third party city-states into the deal automatically
			DoAddPlayersAlliesToTreaty(eActivePlayer, pkUIDeal);

			// Start off as a white peace
			pkUIDeal->SetPeaceTreatyType(PEACE_TREATY_WHITE_PEACE);
		}
	}
}

/// This function called when human player enters diplomacy
void CvDealAI::DoTradeScreenClosed(bool bAIWasMakingOffer)
{
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	// Reset cached values each time screen closed
	SetCachedValueOfPeaceWithHuman(0);
#if defined(MOD_BALANCE_CORE)
	m_pPlayer->SetCachedValueOfPeaceWithHuman(0);
#endif
	GC.GetEngineUserInterface()->SetAIRequestingConcessions(false);
	GC.GetEngineUserInterface()->SetHumanMakingDemand(false);

	GetPlayer()->GetDiplomacyAI()->ClearDealToRenew();

	if(bAIWasMakingOffer)
	{
		// If AI was planning on a mutual Research Agreement, cancel it because the human left :(
		// May want to do this slightly differently, as we can't be 100% sure this is what the AI was asking about (although if we make it through both of the following if statements there's an awful lot of circumstantial evidence)
		if(GetPlayer()->GetDiplomacyAI()->IsWantsResearchAgreementWithPlayer(eActivePlayer))
		{
			if(GetPlayer()->GetDiplomacyAI()->IsCanMakeResearchAgreementRightNow(eActivePlayer))
			{
				GetPlayer()->GetDiplomacyAI()->DoCancelWantsResearchAgreementWithPlayer(eActivePlayer);
			}
		}
	}
}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
// Is the human's request for help acceptable?
DemandResponseTypes CvDealAI::GetRequestForHelpResponse(CvDeal* pDeal)
{
	PlayerTypes eFromPlayer = GC.getGame().getActivePlayer();
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();

	DemandResponseTypes eResponse = NO_DEMAND_RESPONSE_TYPE;

	int iGoldWillingToGiveUp = 0;
	int iGPTWillingToGiveUp = 0;
	int iNumStrategicsGiveUp = 0;
	bool bGiveUpOneLuxury = false;
	bool bGiveUpOneTech = false;

	// Too soon for another help request?
	if(pDiploAI->IsHelpRequestTooSoon(eFromPlayer))
		return DEMAND_RESPONSE_GIFT_REFUSE_TOO_SOON;

	// Not too soon for a help request
	else
	{
		// Deceptive AI's won't ever accept a demand request
		if(pDiploAI->GetMajorCivApproach(eFromPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_DECEPTIVE)
		{
			return DEMAND_RESPONSE_GIFT_REFUSE_TOO_MUCH;
		}

		int iOddsOfGivingIn = pDiploAI->GetLoyalty() * 10;	// initial odds of giving in are loyalty * 10

		int iOurGold = GET_PLAYER(eMyPlayer).GetTreasury()->GetGold();
		int iOurGPT = GET_PLAYER(eMyPlayer).calculateGoldRate();
		int iOurTotalHappy = GET_PLAYER(eMyPlayer).GetExcessHappiness();
		int iOurBeakersPerTurn = GET_PLAYER(eMyPlayer).GetScience();
		int iOurTechs = GET_TEAM(GET_PLAYER(eMyPlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();
		int iTheirGold = GET_PLAYER(eFromPlayer).GetTreasury()->GetGold();
		int iTheirGPT = GET_PLAYER(eFromPlayer).calculateGoldRate();
		int iTheirTotalHappy = GET_PLAYER(eFromPlayer).GetExcessHappiness();
		int iTheirBeakersPerTurn = GET_PLAYER(eFromPlayer).GetScience();
		int iTheirTechs = GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();

		// AI will only give up enough gold to even them out between the player
		iGoldWillingToGiveUp = (iOurGold - iTheirGold) / 2;

		if(iGoldWillingToGiveUp < 0)
		{
			iGoldWillingToGiveUp = 0;
		}
		// Make sure the human isn't exploiting the AI before the Industrial Era
		else if((iGoldWillingToGiveUp > 1000) && GET_PLAYER(eMyPlayer).GetCurrentEra() < 4)
		{
			iGoldWillingToGiveUp = 1000;
		}

		// If we have gold to spare then add more in if the player is in wars (250g per war)
		if(iOurGold - iGoldWillingToGiveUp > 300 && iGoldWillingToGiveUp > 0)
		{
			// Add 250 more gold for every war the player is in, if it won't bankrupt us
			for(int i = 0; i < GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).getAtWarCount(true); i++)
			{
				if(iOurGold - iGoldWillingToGiveUp - 250 > 0)
				{
					iGoldWillingToGiveUp += 250;
				}
			}
		}
		
		// AI will only give enough GPT to even them out between the player
		iGPTWillingToGiveUp = (iOurGPT - iTheirGPT) / 2;

		if(iGPTWillingToGiveUp < 0)
		{
			iGPTWillingToGiveUp = 0;
		}
		// 5 more GPT willing to give up each era...(no GPT in the Ancient Era)
		else if(iGPTWillingToGiveUp > 5 * GET_PLAYER(eMyPlayer).GetCurrentEra())
		{
			iGPTWillingToGiveUp = 5 * GET_PLAYER(eMyPlayer).GetCurrentEra();
		}

		// Give a luxury to this player? (only one)
		if(iOurTotalHappy > 6 &&											// need to have some spare happiness
			iTheirTotalHappy < 2 &&											// they need happiness
			(pDiploAI->GetWarmongerThreat(eFromPlayer) < THREAT_MAJOR &&	// don't give out happiness to warmongers
			pDiploAI->GetWarmongerHate() > 5) &&							// only if we hate warmongers
			!pDiploAI->IsPlayerRecklessExpander(eFromPlayer))				// don't give out happiness to reckless expanders
		{
			bGiveUpOneLuxury = true;
		}

		// Give strategics (and how many?)
		iNumStrategicsGiveUp =  5 * GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).getAtWarCount(true);			// we're willing to give up strategics if he's at war

		// Give up tech?
		if((iOurTechs > iTheirTechs ||									// we have more techs than them -OR-
			iOurBeakersPerTurn > iTheirBeakersPerTurn * 120 / 100) &&	// we have 120% more science per turn than them
			!pDiploAI->IsGoingForSpaceshipVictory())					// don't give up tech if we want to launch spaceship
		{
			bGiveUpOneTech = true;
		}

		switch(pDiploAI->GetMajorCivOpinion(eFromPlayer))
		{
			case MAJOR_CIV_OPINION_ALLY:
			{
				iOddsOfGivingIn += 100;
				break;
			}
			case MAJOR_CIV_OPINION_FRIEND:
			{
				iOddsOfGivingIn += 50;
				break;
			}
			case MAJOR_CIV_OPINION_FAVORABLE:
			{
				iOddsOfGivingIn += 25;
				break;
			}
		}

		// IMPORTANT NOTE: This APPEARS to be very bad for multiplayer, but the only changes made to the game state are the fact that the human
		// made a demand, and if the deal went through. These are both sent over the network later in this function.

		int iAsyncRand = GC.getGame().getAsyncRandNum(100, "Deal AI: ASYNC RAND call to determine if AI will give into a human demand.");

		// Are they going to say no matter what?
		if(iAsyncRand > iOddsOfGivingIn)
			return DEMAND_RESPONSE_GIFT_REFUSE_TOO_MUCH;
	}

	// Possibility exists the AI will accept
	if(eResponse == NO_DEMAND_RESPONSE_TYPE)
	{
		int iGoldRequested = 0, iGPTRequested = 0, iLuxuriesRequested = 0, iStrategicsRequested = 0, iTechsRequested = 0;
		int iTempGold;

		TradedItemList::iterator it;
		for(it = pDeal->m_TradedItems.begin(); it != pDeal->m_TradedItems.end(); ++it)
		{
			// Item from this AI
			if(it->m_eFromPlayer == eMyPlayer)
			{
				switch(it->m_eItemType)
				{
					case TRADE_ITEM_GOLD:
					{
						iTempGold = it->m_iData1;
						iGoldRequested += iTempGold;
						break;
					}
					case TRADE_ITEM_GOLD_PER_TURN:
					{
						iGPTRequested = it->m_iData1;
						break;
					}
					case TRADE_ITEM_RESOURCES:
					{
						ResourceTypes eResource = (ResourceTypes) it->m_iData1;
						ResourceUsageTypes eUsage = GC.getResourceInfo(eResource)->getResourceUsage();

						if(eUsage == RESOURCEUSAGE_LUXURY)
							iLuxuriesRequested++;
						else if(eUsage == RESOURCEUSAGE_STRATEGIC)
							iStrategicsRequested += it->m_iData2;
						break;
					}
					case TRADE_ITEM_TECHS:
					{
						iTechsRequested++;
						break;
					}
					case TRADE_ITEM_OPEN_BORDERS:
					case TRADE_ITEM_MAPS:
					{
						break;				// skip over open borders and maps, don't care about them.
					}
					case TRADE_ITEM_CITIES:
					case TRADE_ITEM_DEFENSIVE_PACT:
					case TRADE_ITEM_RESEARCH_AGREEMENT:
					case TRADE_ITEM_PERMANENT_ALLIANCE:
					case TRADE_ITEM_THIRD_PARTY_PEACE:
					case TRADE_ITEM_THIRD_PARTY_WAR:
					case TRADE_ITEM_THIRD_PARTY_EMBARGO:
					case TRADE_ITEM_VOTE_COMMITMENT:
					case TRADE_ITEM_VASSALAGE:
					case TRADE_ITEM_VASSALAGE_REVOKE:
					default:
					{
						eResponse = DEMAND_RESPONSE_GIFT_REFUSE_TOO_MUCH;
						break;
					}
				}
			}
		}

		// No illegal items in the request
		if(eResponse == NO_DEMAND_RESPONSE_TYPE)
		{
			if(iGoldWillingToGiveUp >= iGoldRequested &&
				iGPTWillingToGiveUp >= iGPTRequested &&
				(bGiveUpOneLuxury ? (iLuxuriesRequested <= 1) : iLuxuriesRequested <= 0) &&
				iNumStrategicsGiveUp >= iStrategicsRequested &&
				(bGiveUpOneTech ? (iTechsRequested <= 1) : iTechsRequested <= 0))
			{
				eResponse = DEMAND_RESPONSE_GIFT_ACCEPT;
			}
			else
			{
				eResponse = DEMAND_RESPONSE_GIFT_REFUSE_TOO_MUCH;
			}
		}
	}

	return eResponse;
}

// How much is this world map worth?
int CvDealAI::GetMapValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Map with oneself.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 500;
	int iPlotValue = 0;

	CvPlayer* pSeller;		// Who is selling this map?
	CvPlayer* pBuyer;		// Who is buying this map?

	if(bFromMe)
	{
		pSeller = GetPlayer();
		pBuyer = &GET_PLAYER(eOtherPlayer);
	}
	else
	{
		pSeller = &GET_PLAYER(eOtherPlayer);
		pBuyer = GetPlayer();
	}
	int iUsRevealed = 0;
	int iThemRevealed = 0;
	int iNumTeams = GC.getGame().countCivTeamsAlive();

	CvPlot* pPlot;
	// Look at every tile on map
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iI);
		iPlotValue = 0;

		if(pPlot == NULL)
		{
			continue;
		}

		// Seller must have plot revealed
		if(!pPlot->isRevealed(pSeller->getTeam()))
		{
			continue;
		}
		else
		{
			if(bFromMe)
			{
				iUsRevealed++;
			}
			else
			{
				iThemRevealed++;
			}
		}

		// Buyer can't have plot revealed
		if(pPlot->isRevealed(pBuyer->getTeam()))
		{
			if(bFromMe)
			{
				iThemRevealed++;
			}
			else
			{
				iUsRevealed++;
			}
			continue;
		}

		// Pick only plots that have been revealed by us and not revealed by them

		// Handle terrain features
		switch(pPlot->getTerrainType())
		{
			case TERRAIN_GRASS:
				iPlotValue = 60;
				break;
			case TERRAIN_PLAINS:
				iPlotValue = 60;
				break;
			case TERRAIN_DESERT:
				iPlotValue = 60;
				break;
			case TERRAIN_HILL:
				iPlotValue = 60;
				break;
			case TERRAIN_COAST:
				iPlotValue = 45;
				break;
			case TERRAIN_TUNDRA:
				iPlotValue = 35;
				break;
			case TERRAIN_MOUNTAIN:
				iPlotValue = 35;
				break;
			case TERRAIN_SNOW:
				iPlotValue = 5;
				break;
			case TERRAIN_OCEAN:
				iPlotValue = 5;
				break;
			default:
				iPlotValue = 20;
				break;
		}

		// Modifier based on feature type
		switch(pPlot->getFeatureType())
		{
			case FEATURE_ICE:
				iPlotValue *= 0;
				break;
			default:
				iPlotValue *= 100;
				break;
		}
		iPlotValue /= 100;

		// Is there a Natural Wonder here? 500% of plot.
#if defined(MOD_PSEUDO_NATURAL_WONDER)
		if(pPlot->IsNaturalWonder(true))
#else
		if(pPlot->IsNaturalWonder())
#endif
		{
			iPlotValue *= 500;
			iPlotValue /= 100;
		}

		int iNumRevealed = 0;
		TeamTypes eTeam;
		// Value decreased based on number of teams who've seen this plot
		for(int iTeamLoop = 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
		{
			eTeam = (TeamTypes) iTeamLoop;
			// Don't evaluate us or the buyer
			if(eTeam == pSeller->getTeam() || eTeam == pBuyer->getTeam())
			{
				continue;
			}

			// Don't evaluate minors/barbarians
			if(GET_TEAM(eTeam).isMinorCiv() || GET_TEAM(eTeam).isBarbarian())
			{
				continue;
			}

			if(pPlot->isRevealed(eTeam) && GET_TEAM(eTeam).isAlive())
			{
				iNumRevealed++;
			}
		}

		// Modifier based on uniqueness
		CvAssertMsg(GC.getGame().countCivTeamsAlive() != 0, "CvDealAI: Civ Team count equals zero...");

		int iModifier = (iNumTeams - iNumRevealed) * 100 / iNumTeams;

		if(iModifier < 50)
			iModifier = 50;

		iPlotValue *= iModifier;
		iPlotValue /= 100;

		iItemValue += iPlotValue;
	}
	iItemValue /= 100;

	if(bFromMe)
	{
		if((iThemRevealed * 3) > (iUsRevealed * 2))
		{
			return INT_MAX;
		}
		// Approach will modify the deal
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 250;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 130;
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				iItemValue *= 80;
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 100;
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				iItemValue *= 100;
				break;
			default:
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Map Trading valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}
	else
	{
		if((iUsRevealed * 3) > (iThemRevealed  * 2))
		{
			return INT_MAX;
		}
		// Approach will modify the deal
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 25;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 50;
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				iItemValue *= 75;
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 120;
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				iItemValue *= 100;
				break;
			default:
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Map Trading valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetMapValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		if (iReverseValue == INT_MAX)
			//no deal, can't agree on a value
			iItemValue = INT_MAX;
		else
			iItemValue = (iItemValue + iReverseValue)/2;
	}

	return iItemValue;
}

void CvDealAI::UpdateResearchRateCache(PlayerTypes eOther)
{
	TeamTypes ourTeam = m_pPlayer->getTeam();
	TeamTypes theirTeam = GET_PLAYER(eOther).getTeam();

	if ( m_vResearchRates[m_pPlayer->GetID()].first != GC.getGame().getGameTurn() )
	{
		m_vResearchRates[m_pPlayer->GetID()] = std::make_pair(GC.getGame().getGameTurn(),0);
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
			if(kPlayer.isAlive() && kPlayer.getTeam() == ourTeam)
				m_vResearchRates[m_pPlayer->GetID()].second += kPlayer.GetScienceTimes100();
		}
	}

	if ( m_vResearchRates[eOther].first != GC.getGame().getGameTurn() )
	{
		m_vResearchRates[eOther] = std::make_pair(GC.getGame().getGameTurn(),0);
		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
			if(kPlayer.isAlive() && kPlayer.getTeam() == theirTeam)
				m_vResearchRates[eOther].second += kPlayer.GetScienceTimes100();
		}
	}
}

// How much is a technology worth?
int CvDealAI::GetTechValue(TechTypes eTech, bool bFromMe, PlayerTypes eOtherPlayer)
{
	int iItemValue = 100;
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);

	if (pkTechInfo == NULL)
		return INT_MAX;

	//important, don't want to recalculate for every potential tech
	UpdateResearchRateCache(eOtherPlayer);

	int iTurnsLeft;
	int iTechEra = pkTechInfo->GetEra();

	if(bFromMe)
	{
		iTurnsLeft = GET_PLAYER(eOtherPlayer).GetPlayerTechs()->GetResearchTurnsLeft(eTech, true, m_vResearchRates[eOtherPlayer].second);
	}
	else
	{
		iTurnsLeft = GetPlayer()->GetPlayerTechs()->GetResearchTurnsLeft(eTech, true, m_vResearchRates[GetPlayer()->GetID()].second);
	}

	int iI, iTechMod = 0;

	for(iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		CvUnitEntry* pUnitEntry = GC.getUnitInfo((UnitTypes) iI);
		if(pUnitEntry)
		{
			if(pUnitEntry->GetPrereqAndTech() == eTech)
			{
				iTechMod += 1;

				if(pUnitEntry->GetNukeDamageLevel() > 0)
				{
					iTechMod *= 2;
				}
			}
		}
	}

	for(iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		CvBuildingEntry* pBuildingEntry = GC.getBuildingInfo((BuildingTypes) iI);
		if(pBuildingEntry)
		{
			if(pBuildingEntry->GetPrereqAndTech() == eTech)
			{
				iTechMod += 2;
			}
		}
	}

	for(iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		CvProjectEntry* pProjectEntry = GC.getProjectInfo((ProjectTypes) iI);
		if(pProjectEntry)
		{
			if(pProjectEntry->GetTechPrereq() == eTech)
			{
				iTechMod += 2;
			}
		}
	}

	for(iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		CvBuildInfo* pBuildInfo = GC.getBuildInfo((BuildTypes) iI);
		if(pBuildInfo)
		{
			if(pBuildInfo->getTechPrereq() == eTech)
			{
				iTechMod += 2;
			}
		}
	}

	for(iI = 0; iI < GC.getNumResourceInfos(); iI++)
	{
		CvResourceInfo* pResourceInfo = GC.getResourceInfo((ResourceTypes) iI);
		if(pResourceInfo)
		{
			if(pResourceInfo->getTechReveal() == eTech)
			{
				iTechMod += 2;
			}
		}
	}

	if(pkTechInfo->IsTechTrading())
	{
		if(!GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING) && !GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		{
			iTechMod += 2;
		}
	}

	if(pkTechInfo->IsResearchAgreementTradingAllowed())
	{
		if(GC.getGame().isOption(GAMEOPTION_RESEARCH_AGREEMENTS) && !GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		{
			iTechMod += 2;
		}
	}

	if(pkTechInfo->IsResearchAgreementTradingAllowed())
	{
		if(GC.getGame().isOption(GAMEOPTION_RESEARCH_AGREEMENTS) && !GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		{
			iTechMod += 2;
		}
	}

	// BASE COST = (TurnsLeft * 30 * (era ^ 0.7))	-- Ancient Era is 1, Classical Era is 2 because I incremented it
	iItemValue = max(15, iTurnsLeft) * /*30*/ max(100, GC.getGame().getGameSpeedInfo().getTechCostPerTurnMultiplier());
	float fItemMultiplier = (float)(pow( (double) std::max(1, (iTechEra + 1)), (double) /*0.7*/ GC.getTECH_COST_ERA_EXPONENT() ) );
	iItemValue = (int)(iItemValue * fItemMultiplier);

	// Apply the Modifier
	iItemValue *= (100 + iTechMod);
	iItemValue /= 125;

	PlayerTypes ePlayer;
	if(bFromMe)
		ePlayer = GET_PLAYER(eOtherPlayer).GetID();
	else
		ePlayer = GetPlayer()->GetID();

	//Policy modifier - To-do, something else?
	for(int iPoliciesLoop = 0; iPoliciesLoop < GC.getNumPolicyInfos(); iPoliciesLoop++)
	{
		// Do this player have this policy && is it not blocked (e.g. Piety/Rationalism)
		if(GET_PLAYER(ePlayer).GetPlayerPolicies()->HasPolicy((PolicyTypes)iPoliciesLoop) && !GET_PLAYER(ePlayer).GetPlayerPolicies()->IsPolicyBlocked((PolicyTypes)iPoliciesLoop))
		{
			int iTechValueChangePercent = GC.getPolicyInfo((PolicyTypes)iPoliciesLoop)->GetMedianTechPercentChange();
			// Does this policy modify median research agreement percent?
			if(iTechValueChangePercent != 0)
			{
				if(iTechValueChangePercent > 100)
				{
					iTechValueChangePercent = 100; // super mega policy that makes tech trading free! Don't be an idiot and do this!
				}

				iItemValue *= (100 - iTechValueChangePercent); //by default, should be 25. Thus multiply by 75.
				iItemValue /= 100;	// result will be -25%
			}
		}
	}

	if(bFromMe)
	{
		iItemValue *= 2;
		// Approach is important
#if defined(MOD_BALANCE_CORE)
		if(GetPlayer()->GetDiplomacyAI()->GetVictoryBlockLevel(eOtherPlayer) >= BLOCK_LEVEL_STRONG || GetPlayer()->GetDiplomacyAI()->GetVictoryDisputeLevel(eOtherPlayer) >= DISPUTE_LEVEL_STRONG)
		{
			return INT_MAX;
		}
		if(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer) < MAJOR_CIV_OPINION_FRIEND && !GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eOtherPlayer) )
		{
			return INT_MAX;
		}
#endif
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
			case MAJOR_CIV_APPROACH_HOSTILE:
				return INT_MAX;
			case MAJOR_CIV_APPROACH_GUARDED:
				return INT_MAX;
			case MAJOR_CIV_APPROACH_AFRAID:
				return INT_MAX;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 125;
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				return INT_MAX;
			default:
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Technology valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
				break;
		}
		iItemValue /= 100;

	}
#if defined(MOD_BALANCE_CORE)
	else
	{
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 50;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 75;
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				iItemValue *= 100;
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 100;
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				iItemValue *= 100;
				break;
			default:
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Technology valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
				break;
		}
		iItemValue /= 100;
		
		if(GetPlayer()->GetDiplomacyAI()->GetVictoryBlockLevel(eOtherPlayer) >= BLOCK_LEVEL_STRONG || GetPlayer()->GetDiplomacyAI()->GetVictoryDisputeLevel(eOtherPlayer) >= DISPUTE_LEVEL_STRONG)
		{
			iItemValue /= 3;
		}
	}

	if(iItemValue <= 250)
		return 250;

#endif

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	//if(bUseEvenValue)
	//{
	//	iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetTechValue(eTech, !bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

	//	iItemValue /= 2;
	//}
	
	return iItemValue;
}

/// How much is Vassalage worth?
int CvDealAI::GetVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, bool bWar)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Vassalage Agreement with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	CvDiplomacyAI* m_pDiploAI = GetPlayer()->GetDiplomacyAI();

	if (bFromMe)
	{
		if (!m_pDiploAI->IsVassalageAcceptable(eOtherPlayer, bWar))
		{
			return INT_MAX;
		}

		// Initial Vassalage deal value at 1000
		iItemValue = 1000;

		if (bWar)
		{
			return (iItemValue / 2);
		}

		// Add deal value based on number of wars player is currently fighting (including with minors)
		iItemValue += iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).getAtWarCount(true));

		// Increase deal value based on number of vassals player already has
		iItemValue += iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetNumVassals());

		// ex: 2 wars and 2 vassals = 500 + 1000 + 1000 = a 2500 gold before modifiers!

		// The point of Vassalage is protection, if they're not militarily dominant - what's the point?
		switch (m_pDiploAI->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer))
		{
		case STRENGTH_IMMENSE:
			iItemValue *= 80;
			break;
		case STRENGTH_POWERFUL:
			iItemValue *= 90;
			break;
		case STRENGTH_STRONG:
			iItemValue *= 120;
			break;
		case STRENGTH_AVERAGE:
			iItemValue *= 200;
			break;
		case STRENGTH_POOR:
		case STRENGTH_WEAK:
		case STRENGTH_PATHETIC:
		default:
			if (bWar)
			{
				iItemValue *= 100;
			}
			else
			{
				return INT_MAX;
			}
			break;
		}
		iItemValue /= 100;

		switch (GetPlayer()->GetProximityToPlayer(eOtherPlayer))
		{
		case PLAYER_PROXIMITY_DISTANT:
			iItemValue *= 300;
			break;
		case PLAYER_PROXIMITY_FAR:
			iItemValue *= 150;
			break;
		case PLAYER_PROXIMITY_CLOSE:
		case PLAYER_PROXIMITY_NEIGHBORS:
			iItemValue *= 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: Player has no valid proximity for Vassalage valuation.");
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;

		switch (m_pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 250;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 130;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 75;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 85;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Vassalage valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}
	//from them?
	else
	{
		//they don't want to do it?
		if (!GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsVassalageAcceptable(m_pPlayer->GetID(), bWar))
		{
			return INT_MAX;
		}

		// Initial Vassalage deal value at 1000
		iItemValue = 1000;

		if (bWar)
		{
			return (iItemValue / 2);
		}

		// Add deal value based on number of wars player is currently fighting (including with minors)
		iItemValue -= iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).getAtWarCount(true));

		// Increase deal value based on number of vassals player already has
		iItemValue -= iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetNumVassals());

		// ex: 2 wars and 2 vassals = 500 + 1000 + 1000 = a 2500 gold before modifiers!

		// The point of Vassalage is protection, if they're not militarily dominant - what's the point?
		switch (m_pDiploAI->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer))
		{
		case STRENGTH_IMMENSE:
			iItemValue *= 150;
			break;
		case STRENGTH_POWERFUL:
			iItemValue *= 120;
			break;
		case STRENGTH_STRONG:
			iItemValue *= 100;
			break;
		case STRENGTH_AVERAGE:
			iItemValue *= 75;
			break;
		case STRENGTH_POOR:
		case STRENGTH_WEAK:
		case STRENGTH_PATHETIC:
		default:
			if (bWar)
			{
				iItemValue *= 50;
			}
			else
			{
				return INT_MAX;
			}
			break;
		}
		iItemValue /= 100;

		switch (GetPlayer()->GetProximityToPlayer(eOtherPlayer))
		{
		case PLAYER_PROXIMITY_DISTANT:
			iItemValue *= 25;
			break;
		case PLAYER_PROXIMITY_FAR:
			iItemValue *= 50;
			break;
		case PLAYER_PROXIMITY_CLOSE:
		case PLAYER_PROXIMITY_NEIGHBORS:
			iItemValue *= 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: Player has no valid proximity for Vassalage valuation.");
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;

		switch (m_pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 50;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 60;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue *= 125;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue *= 150;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue *= 125;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Vassalage valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}


	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetVassalageValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false, bWar);

		if (iReverseValue == INT_MAX)
			//no deal, can't agree on a value
			iItemValue = INT_MAX;
		else
			iItemValue = (iItemValue + iReverseValue)/2;
	}

	return iItemValue;
}
//How much is ending our vassalage worth to us?
int CvDealAI::GetRevokeVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, bool bWar)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Vassalage Agreement with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	CvDiplomacyAI* m_pDiploAI = GetPlayer()->GetDiplomacyAI();
	//Can't ask if we're they're vassal, or vice-versa.
	if(GET_TEAM(GetPlayer()->getTeam()).IsVassal(GET_PLAYER(eOtherPlayer).getTeam()))
	{
		return INT_MAX;
	}
	else if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsVassal(GetPlayer()->getTeam()))
	{
		return INT_MAX;
	}
	//They're asking me to revoke my vassals? Quoi?
	if(bFromMe)						
	{
		// Initial End Vassalage deal value at 1000
		iItemValue = 1000;

		//War? Refuse if we're not losing badly.
		if(bWar)
		{
			if(m_pDiploAI->GetWarScore(eOtherPlayer) >= -90)
			{
				return INT_MAX;
			}
			else
			{
				return iItemValue;
			}
		}

		// Increase deal value based on number of vassals we have
		for(int iTeamLoop= 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
		{
			TeamTypes eLoopTeam = (TeamTypes) iTeamLoop;

			// Ignore minors.
			if(eLoopTeam != NO_TEAM && !GET_TEAM(eLoopTeam).isMinorCiv())
			{
				// Is eLoopTeam our vassal?
				if(GET_TEAM(eLoopTeam).IsVassal(GetPlayer()->getTeam()))
				{
					if(GET_TEAM(eLoopTeam).isAlive())
					{
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
						{
							PlayerTypes eVassalPlayer = (PlayerTypes)iPlayerLoop;

							if(eVassalPlayer != NO_PLAYER && GET_PLAYER(eVassalPlayer).getTeam() != eLoopTeam && eLoopTeam != GetPlayer()->getTeam())
								continue;

							iItemValue += (GET_PLAYER(eVassalPlayer).GetMilitaryMight() + GET_PLAYER(eVassalPlayer).GetEconomicMight());
						}
					}
				}
			}
		}

		//Diplo and Conquest victories
		if(GetPlayer()->GetDiplomacyAI()->IsGoingForWorldConquest())
		{
			iItemValue *= 2;
		}
		if(GetPlayer()->GetDiplomacyAI()->IsGoingForDiploVictory())
		{
			iItemValue *= 2;
		}

		// What's the power of the asking party? They need to be real strong to push us out of this.
		switch(m_pDiploAI->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer))
		{
			case STRENGTH_IMMENSE:
				iItemValue *= 80;
				break;
			case STRENGTH_POWERFUL:
				iItemValue *= 100;
				break;
			case STRENGTH_STRONG:
				iItemValue *= 200;
				break;
			case STRENGTH_AVERAGE:
				iItemValue *= 400;
				break;
			case STRENGTH_POOR:
				return INT_MAX;
				break;
			case STRENGTH_WEAK:
				return INT_MAX;
				break;
			case STRENGTH_PATHETIC:
				return INT_MAX;
				break;
			default:
				return INT_MAX;
				break;
		}
		iItemValue /= 100;

		switch(m_pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
			case MAJOR_CIV_APPROACH_HOSTILE:
				return INT_MAX;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				return INT_MAX;
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				iItemValue *= 60;
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 80;
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				return INT_MAX;
				break;
			default:
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Vassalage valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
				break;
		}
		iItemValue /= 100;

		switch(m_pDiploAI->GetMajorCivOpinion(eOtherPlayer))
		{
			case MAJOR_CIV_OPINION_ALLY:
				iItemValue *= 70;
				break;
			case MAJOR_CIV_OPINION_FRIEND:
				iItemValue *= 85;
				break;
			case MAJOR_CIV_OPINION_FAVORABLE:
				iItemValue *= 95;
				break;
			case MAJOR_CIV_OPINION_NEUTRAL:
			case MAJOR_CIV_OPINION_COMPETITOR:
			case MAJOR_CIV_OPINION_ENEMY:
			case MAJOR_CIV_OPINION_UNFORGIVABLE:
				return INT_MAX;
				break;
			default:
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Vassalage valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
				break;
		}
		iItemValue /= 100;
	}
	//I'm offering to revoke their vassals? This better be worth it.
	else
	{
		bool bWorthIt = false;
		iItemValue = 1000;
		// Increase deal value based on number of vassals we have
		for(int iTeamLoop= 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
		{
			TeamTypes eLoopTeam = (TeamTypes) iTeamLoop;

			// Ignore minors.
			if(!GET_TEAM(eLoopTeam).isMinorCiv())
			{
				// Is eLoopTeam the vassal of them?
				if(GET_TEAM(eLoopTeam).IsVassal(GET_PLAYER(eOtherPlayer).getTeam()))
				{
					if(GET_TEAM(eLoopTeam).isAlive())
					{
						for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
						{
							PlayerTypes eVassalPlayer = (PlayerTypes)iPlayerLoop;

							if(eVassalPlayer != NO_PLAYER && GET_PLAYER(eVassalPlayer).getTeam() != eLoopTeam)
								continue;

							iItemValue += (GET_PLAYER(eVassalPlayer).GetMilitaryMight() + GET_PLAYER(eVassalPlayer).GetEconomicMight());

							//Are we friends with a vassal? If so, we care about this. Otherwise, meh.
							switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eVassalPlayer))
							{
								case MAJOR_CIV_OPINION_ALLY:
									iItemValue *= 2;
									bWorthIt = true;
									break;
								case MAJOR_CIV_OPINION_FRIEND:
									bWorthIt = true;
									break;
							}
						}
					}
				}
			}
		}
		if(!bWorthIt)
		{
			return INT_MAX;
		}
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetRevokeVassalageValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false, bWar);

		if (iReverseValue == INT_MAX)
			//no deal, can't agree on a value
			iItemValue = INT_MAX;
		else
			iItemValue = (iItemValue + iReverseValue)/2;
	}

	return iItemValue;
}

/// A good time to offer for World Map?
bool CvDealAI::IsMakeOfferForMaps(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{

	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for a map if we're hostile
	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
	{
		return false;
	}

	// Can we actually complete this deal?
	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_MAPS))
	{
		return false;
	}

	// Do we actually want Maps from this other Player?
	if(GetPlayer()->GetDiplomacyAI()->WantsMapsFromPlayer(eOtherPlayer))
	{
		// Seed the deal with the item we want
		pDeal->AddMapTrade(eOtherPlayer);
		bool bDealAcceptable = false;

		// AI evaluation
		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, false, true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;;
	}
	return false;
}

/// A good time to make an offer for technology?
bool CvDealAI::IsMakeOfferForTech(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for Technology if we're hostile or planning war
	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR		||
	        eApproach == MAJOR_CIV_APPROACH_GUARDED)
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE_DEALS)
	//No current research? We may have just finished something.
	if(GetPlayer()->GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{
		return false;
	}
	//No current research? We may have just finished something.
	if(GET_PLAYER(eOtherPlayer).GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{
		return false;
	}
#endif
	int iTechLoop;
	TechTypes eTech;
	TechTypes eTechWeWant = NO_TECH;

	int iBestValue = 0;
	// See if the other player has a technology to trade
	for(iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		eTech = (TechTypes) iTechLoop;

		int iValue = 0;
		// Let's not ask for the tech we're currently researching
		if(eTech == GetPlayer()->GetPlayerTechs()->GetCurrentResearch())
		{
			continue;
		}

#if defined(MOD_BALANCE_CORE_DEALS)
		if(!GetPlayer()->GetPlayerTechs()->IsResearch())
		{
			continue;
		}
		if(!GET_PLAYER(eOtherPlayer).GetPlayerTechs()->IsResearch())
		{
			continue;
		}
		if(eTech == GET_PLAYER(eOtherPlayer).GetPlayerTechs()->GetCurrentResearch())
		{
			continue;
		}
		// don't do the expensive turn calculation for techs that can't be traded anyway
		if(!GetPlayer()->GetPlayerTechs()->CanResearch(eTech) )
			continue;
#else
		// Don't ask for a tech that costs 15 or more turns to research
		if(GetPlayer()->GetPlayerTechs()->GetResearchTurnsLeft(eTech, true) > 15)
		{
			continue;
		}
#endif

		// Can they sell that tech?
		if(pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_TECHS, eTech))
		{
			iValue = GetTechValue(eTech, false, eOtherPlayer);
			if(iValue > iBestValue)
			{
				eTechWeWant = eTech;
				iBestValue = iValue;
			}
		}
	}

	// TECH FOUND!
	if(eTechWeWant != NO_TECH)
	{
		// Seed the deal with the item we want
		pDeal->AddTechTrade(eOtherPlayer, eTechWeWant);

		bool bDealAcceptable = false;

		// AI evaluation
		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, /*bDontChangeTheirExistingItems*/ true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;;
	}

	return false;
}

/// A good time to make an offer for Vassalage?
bool CvDealAI::IsMakeOfferForVassalage(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Human teams don't get asked for vassalage
	if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).isHuman())
	{
		return false;
	}

	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_VASSALAGE))
	{
		return false;
	}

	if(!GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsVassalageAcceptable(GetPlayer()->GetID(), false))
	{
		return false;
	}
	
	// Seed the deal with the item we want
	pDeal->AddVassalageTrade(eOtherPlayer);

	bool bDealAcceptable = false;

	// AI evaluation
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
	}
	else
	{
		CvAssertMsg(false, "Don't ask humans for vassalage!");
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;;
}
bool CvDealAI::IsMakeOfferForRevokeVassalage(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	//If the other player has no vassals...
	if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetNumVassals() <= 0)
	{
		return false;
	}

	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_VASSALAGE_REVOKE))
	{
		return false;
	}

	bool bWorthIt = false;
	bool bValid = true;
	for(int iTeamLoop= 0; iTeamLoop < MAX_TEAMS; iTeamLoop++)
	{
		TeamTypes eLoopTeam = (TeamTypes) iTeamLoop;

		// Ignore minors.
		if(!GET_TEAM(eLoopTeam).isMinorCiv())
		{
			// Is eLoopTeam the vassal of them?
			if(GET_TEAM(eLoopTeam).IsVassal(GET_PLAYER(eOtherPlayer).getTeam()))
			{
				if(GET_TEAM(eLoopTeam).isAlive())
				{
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
					{
						PlayerTypes eVassalPlayer = (PlayerTypes)iPlayerLoop;

						if(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eVassalPlayer) >= MAJOR_CIV_OPINION_FRIEND)
						{
							bWorthIt = true;
						}
						if(!GET_PLAYER(eVassalPlayer).GetDiplomacyAI()->IsEndVassalageAcceptable(eOtherPlayer))
						{
							bValid = false;
							break;
						}
					}
				}
			}
		}
	}
	bool bDealAcceptable = false;
	if(bWorthIt && bValid)
	{
		// Seed the deal with the item we want
		pDeal->AddRevokeVassalageTrade(eOtherPlayer);
		// AI evaluation
		if(!GET_PLAYER(eOtherPlayer).isHuman())
		{
			bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
		}
		else
		{
			bool bUselessReferenceVariable;
			bool bCantMatchOffer;
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, /*bDontChangeTheirExistingItems*/ true, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;;
}


/// See if adding Maps to their side of the deal helps even out pDeal
void CvDealAI::DoAddMapsToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add World Map to Them, but them is us.  Please show Jon");

	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if they can actually trade it to us
			if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_MAPS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_MAPS, /*bFromMe*/ false, eThem, -1, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if((iItemValue + iValueTheyreOffering) >= (iValueImOffering + iAmountOverWeWillRequest))
				{
					pDeal->AddMapTrade(eThem);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					int iAmountOverWeWillRequest = iTotalValue;
					int iPercentOverWeWillRequest = 0;
					if(GET_PLAYER(eThem).isHuman())
					{
						iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
					}
					else
					{
						iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eThem);
					}
					iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
					iAmountOverWeWillRequest /= 100;
					if(iTotalValue >= iAmountOverWeWillRequest)
					{
						return;
					}
				}
			}
		}
	}
}

/// See if adding Maps to our side of the deal helps even out pDeal
void CvDealAI::DoAddMapsToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add World Map to Us, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if we can actually trade it to them
			if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_MAPS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_MAPS, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
				{
					pDeal->AddMapTrade(eMyPlayer);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					int iAmountUnderWeWillOffer = iTotalValue;
					int iPercentOverWeWillOffer = 0;
					if(GET_PLAYER(eThem).isHuman())
					{
						iPercentOverWeWillOffer = GetDealPercentLeewayWithHuman();
					}
					else
					{
						iPercentOverWeWillOffer = GetDealPercentLeewayWithAI(eThem);
					}
					iAmountUnderWeWillOffer *= iPercentOverWeWillOffer;
					iAmountUnderWeWillOffer /= 100;
					if(iTotalValue <= iAmountUnderWeWillOffer)
					{
						return;
					}
				}
			}
		}
	}
}

/// See if adding Technology to their side of the deal helps even out pDeal
void CvDealAI::DoAddTechToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	CvWeightedVector<int> viTradeValues;
	if(!bDontChangeTheirExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iItemValue;

			int iTechLoop;
			TechTypes eTech;

			// Loop through each Tech
			for(iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
			{
				eTech = (TechTypes) iTechLoop;

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_TECHS, eTech))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_TECHS, /*bFromMe*/ false, eThem, eTech, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

					// If adding this to the deal doesn't take it over the limit, do it (pick best option below)
					if(iItemValue==INT_MAX)
					{
						continue;
					}
					else
					{
						viTradeValues.push_back(eTech, iItemValue);
					}
				}
			}
			// Sort the vector based on value to get the best possible item to trade.
			viTradeValues.SortItems();
			if(viTradeValues.size() > 0)
			{
				for(int iRanking = 0; iRanking < viTradeValues.size(); iRanking++)
				{
					int iWeight = viTradeValues.GetWeight(iRanking);
					if(iWeight != 0)
					{
						// Loop through each Tech
						for(iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
						{
							eTech = (TechTypes) iTechLoop;

							if(iWeight == GetTradeItemValue(TRADE_ITEM_TECHS, /*bFromMe*/ false, eThem, eTech, -1, -1, /*bFlag1*/false, -1, bUseEvenValue))
							{
								if((iWeight + iValueTheyreOffering) <= (iValueImOffering + iAmountOverWeWillRequest))
								{
									pDeal->AddTechTrade(eThem, eTech);
									iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
									int iAmountOverWeWillRequest = iTotalValue;
									int iPercentOverWeWillRequest = 0;
									if(GET_PLAYER(eThem).isHuman())
									{
										iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
									}
									else
									{
										iPercentOverWeWillRequest = GetDealPercentLeewayWithAI(eThem);
									}
									iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
									iAmountOverWeWillRequest /= 100;
									if(iTotalValue >= iAmountOverWeWillRequest)
									{
										return;
									}
								}
							}
						}			
					}
				}
			}
		}
	}
}

/// See if adding Technology to our side of the deal helps even out pDeal
void CvDealAI::DoAddTechToUs(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeMyExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountUnderWeWillOffer, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue < 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iItemValue;

			int iTechLoop;
			TechTypes eTech;

			// Loop through each Tech
			for(iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
			{
				eTech = (TechTypes) iTechLoop;

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_TECHS, eTech))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_TECHS, /*bFromMe*/ true, eThem, eTech, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

					if(iItemValue==INT_MAX)
					{
						return;
					}
					if((iItemValue + iValueImOffering) <= (iValueTheyreOffering + iAmountUnderWeWillOffer))
					{
						pDeal->AddTechTrade(eMyPlayer, eTech);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
						int iAmountUnderWeWillOffer = iTotalValue;
						int iPercentOverWeWillOffer = 0;
						if(GET_PLAYER(eThem).isHuman())
						{
							iPercentOverWeWillOffer = GetDealPercentLeewayWithHuman();
						}
						else
						{
							iPercentOverWeWillOffer = GetDealPercentLeewayWithAI(eThem);
						}
						iAmountUnderWeWillOffer *= iPercentOverWeWillOffer;
						iAmountUnderWeWillOffer /= 100;
						if(iTotalValue <= iAmountUnderWeWillOffer)
						{
							return;
						}
					}
				}
			}
		}
	}
}
#endif
