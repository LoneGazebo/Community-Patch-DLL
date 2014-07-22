/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
int CvDealAI::GetDealPercentLeewayWithAI() const
{
	return 25;
}

/// How much are we willing to back off on what our perceived value of a deal is with a human player to make something work?
int CvDealAI::GetDealPercentLeewayWithHuman() const
{
	return 10;
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
	bool bDealAcceptable = IsDealWithHumanAcceptable(pDeal, eFromPlayer, /*Passed by reference*/ iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, bCantMatchOffer);

	// If they're actually giving us more than we're asking for (e.g. a gift) then accept the deal
	if(!bDealAcceptable)
	{
		if(!pDeal->IsPeaceTreatyTrade(eFromPlayer) && iValueTheyreOffering > iValueImOffering)
		{
#if defined(MOD_BALANCE_CORE_DEALS)
			if (MOD_BALANCE_CORE_DEALS) 
			{
				//If there is a city in this deal...let's refuse. It is probably a trap.
				if(!pDeal->ContainsItemType(TRADE_ITEM_CITIES, eFromPlayer))
				{
#endif
			bDealAcceptable = true;
#if defined(MOD_BALANCE_CORE_DEALS)
				}
			}
#endif
		}
	}

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
	if(m_pPlayer->GetDiplomacyAI()->GetDealToRenew(&iDealType))
	{
		if (iDealType != 0) // if it's not a historic deal
		{
			// make the deal not remove resources when processed
			CvGameDeals::PrepareRenewDeal(m_pPlayer->GetDiplomacyAI()->GetDealToRenew(), &kDeal);
		}
		m_pPlayer->GetDiplomacyAI()->ClearDealToRenew();
	}

	GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
	GC.getGame().GetGameDeals()->FinalizeDeal(eFromPlayer, GetPlayer()->GetID(), true);

	if(GET_PLAYER(eFromPlayer).isHuman())
	{
		iDealValueToMe -= GetCachedValueOfPeaceWithHuman();

		// Reset cached values
		SetCachedValueOfPeaceWithHuman(0);

		LeaderheadAnimationTypes eAnimation;
		// This signals Lua to do some interface cleanup, we only want to do this on the local machine.
		if(GC.getGame().getActivePlayer() == eFromPlayer)
			gDLL->DoClearDiplomacyTradeTable();

		DiploUIStateTypes eUIState = NO_DIPLO_UI_STATE;

		const char* szText = 0;

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

		// Good deal for us
		if(iDealValueToMe >= 100 ||
		        iValueTheyreOffering > (iValueImOffering * 5))	// A deal can be generous if we're getting a lot overall, OR a lot more than we're giving up
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_GENEROUS);
			eAnimation = LEADERHEAD_ANIM_YES;
			GetPlayer()->GetDiplomacyAI()->ChangeRecentTradeValue(eFromPlayer, iDealValueToMe);
		}
		// Acceptable deal for us
		else
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE);
			eAnimation = LEADERHEAD_ANIM_YES;
			GetPlayer()->GetDiplomacyAI()->ChangeRecentTradeValue(eFromPlayer, (iDealValueToMe / 2));
		}

		if(GC.getGame().getActivePlayer() == eFromPlayer)
			GC.GetEngineUserInterface()->SetOfferTradeRepeatCount(0);

		// If this was a peace deal then use that animation instead
		if(kDeal.GetPeaceTreatyType() != NO_PEACE_TREATY_TYPE)
		{
			eAnimation = LEADERHEAD_ANIM_PEACEFUL;
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
			MajorCivApproachTypes eApproach = pDiploAI->GetMajorCivApproach(eFromPlayer, /*bHideTrueFeelings*/ true);
			StrengthTypes eMilitaryStrength = pDiploAI->GetPlayerMilitaryStrengthComparedToUs(eFromPlayer);
			AggressivePostureTypes eMilitaryPosture = pDiploAI->GetMilitaryAggressivePosture(eFromPlayer);
			PlayerProximityTypes eProximity = GET_PLAYER(eMyPlayer).GetProximityToPlayer(eFromPlayer);

			// Unforgivable: AI will never give in
			if(pDiploAI->GetMajorCivOpinion(eFromPlayer) == MAJOR_CIV_OPINION_UNFORGIVABLE)
				eResponse = DEMAND_RESPONSE_REFUSE_HOSTILE;

			// Hostile: AI will never give in
			else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
				eResponse = DEMAND_RESPONSE_REFUSE_HOSTILE;

			// Our military is stronger: AI will never give in
			else if(eMilitaryStrength < STRENGTH_AVERAGE)
				eResponse = DEMAND_RESPONSE_REFUSE_WEAK;

			// They are very far away and have no units near us (from what we can tell): AI will never give in
			else if(eProximity <= PLAYER_PROXIMITY_FAR && eMilitaryPosture == AGGRESSIVE_POSTURE_NONE)
				eResponse = DEMAND_RESPONSE_REFUSE_WEAK;

			// Willing to give in to demand
			else
			{
				// Initial odds of giving in to ANY demand are based on the player's boldness (which is also tied to the player's likelihood of going for world conquest)
				int iOddsOfGivingIn = (10 - pDiploAI->GetBoldness()) * 10;

				iValueWillingToGiveUp = 0;

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
						iOddsOfGivingIn += -100;
						iValueWillingToGiveUp += 10;
						break;
					case STRENGTH_WEAK:
						iOddsOfGivingIn += -100;
						iValueWillingToGiveUp += 10;
						break;
					case STRENGTH_POOR:
						iOddsOfGivingIn += -100;
						iValueWillingToGiveUp += 20;
						break;
					case STRENGTH_AVERAGE:
						iOddsOfGivingIn += -10;
						iValueWillingToGiveUp += 50;
						break;
					case STRENGTH_STRONG:
						iOddsOfGivingIn += 10;
						iValueWillingToGiveUp += 120;
						break;
					case STRENGTH_POWERFUL:
						iOddsOfGivingIn += 20;
						iValueWillingToGiveUp += 200;
						break;
					case STRENGTH_IMMENSE:
						iOddsOfGivingIn += 35;
						iValueWillingToGiveUp += 200;
						break;
					default:
						break;
					}
				}

				// IMPORTANT NOTE: This APPEARS to be very bad for multiplayer, but the only changes made to the game state are the fact that the human
				// made a demand, and if the deal went through. These are both sent over the network later in this function.

				int iAsyncRand = GC.getGame().getAsyncRandNum(100, "Deal AI: ASYNC RAND call to determine if AI will give into a human demand.");

				// Are they going to say no matter what?
				if(iAsyncRand > iOddsOfGivingIn)
					eResponse = DEMAND_RESPONSE_REFUSE_HOSTILE;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
			}
#endif
		}
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
						iModdedGoldValue = iTempGold * 10 / iGPT;
					else
						iModdedGoldValue = 0;

					iValueDemanded += max(iTempGold, iModdedGoldValue);
					break;
				}

				// GPT
				case TRADE_ITEM_GOLD_PER_TURN:
				{
					iValueDemanded += (it->m_iData1 * it->m_iDuration * 80 / 100);
					break;
				}

				// Resources
				case TRADE_ITEM_RESOURCES:
				{
					ResourceTypes eResource = (ResourceTypes) it->m_iData1;
					ResourceUsageTypes eUsage = GC.getResourceInfo(eResource)->getResourceUsage();

					if(eUsage == RESOURCEUSAGE_LUXURY)
						iValueDemanded += 200;
					else if(eUsage == RESOURCEUSAGE_STRATEGIC)
						iValueDemanded += (40 * it->m_iData2);

					break;
				}

				// Open Borders
				case TRADE_ITEM_OPEN_BORDERS:
				{
					iValueDemanded += 50;
					break;
				}

				case TRADE_ITEM_CITIES:
				case TRADE_ITEM_DEFENSIVE_PACT:
				case TRADE_ITEM_RESEARCH_AGREEMENT:
				case TRADE_ITEM_PERMANENT_ALLIANCE:
				case TRADE_ITEM_THIRD_PARTY_PEACE:
				case TRADE_ITEM_THIRD_PARTY_WAR:
				case TRADE_ITEM_THIRD_PARTY_EMBARGO:
				default:
					eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
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
	CvGameDeals* pGameDeals = kGame.GetGameDeals();
	const PlayerTypes eActivePlayer = kGame.getActivePlayer();
	const PlayerTypes ePlayer = GetPlayer()->GetID();

	pGameDeals->AddProposedDeal(kDeal);
	pGameDeals->FinalizeDeal(eFromPlayer, ePlayer, true);
	if(eActivePlayer == eFromPlayer || eActivePlayer == ePlayer)
	{
		GC.GetEngineUserInterface()->makeInterfaceDirty();
	}
}

/// Will this AI accept pDeal? Handles deal from both human and AI players
bool CvDealAI::IsDealWithHumanAcceptable(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValueToMe, int& iValueImOffering, int& iValueTheyreOffering, int& iAmountOverWeWillRequest, int& iAmountUnderWeWillOffer, bool& bCantMatchOffer)
{
	CvAssertMsg(GET_PLAYER(eOtherPlayer).isHuman(), "DEAL_AI: Trying to see if AI will accept a deal with human player... but it's not human.  Please show Jon.");

	int iPercentOverWeWillRequest;
	int iPercentUnderWeWillOffer;

	bCantMatchOffer = false;

	// Deal leeway with human
	iPercentOverWeWillRequest = GetDealPercentLeewayWithHuman();
	iPercentUnderWeWillOffer = 0;

	// Now do the valuation
	iTotalValueToMe = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);

	// If no Gold in deal and within value of 1 GPT, then it's close enough
	if (pDeal->GetGoldTrade(eOtherPlayer) == 0 && pDeal->GetGoldTrade(m_pPlayer->GetID()) == 0)
	{
		int iOneGPT = 25;
		int iDiff = abs(iValueTheyreOffering - iValueImOffering);
		if (iDiff < iOneGPT)
		{
#if defined(MOD_BALANCE_CORE_DEALS)
			if(!pDeal->ContainsItemType(TRADE_ITEM_CITIES))
			{
#endif
			return true;
#if defined(MOD_BALANCE_CORE_DEALS)
			}
#endif
		}
	}

	int iDealSumValue = iValueImOffering + iValueTheyreOffering;

	iAmountOverWeWillRequest = iDealSumValue;
	iAmountOverWeWillRequest *= iPercentOverWeWillRequest;
	iAmountOverWeWillRequest /= 100;

	iAmountUnderWeWillOffer = iDealSumValue;
	iAmountUnderWeWillOffer *= iPercentUnderWeWillOffer;
	iAmountUnderWeWillOffer /= 100;

	// We're surrendering
	if(pDeal->GetSurrenderingPlayer() == GetPlayer()->GetID())
	{
		if (iTotalValueToMe >= GetCachedValueOfPeaceWithHuman())
		{
			return true;
		}
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// We're offering help to a player
	else if (MOD_DIPLOMACY_CIV4_FEATURES && pDeal->GetOfferingPlayer() == GetPlayer()->GetID())
	{
		return true;
	}
#endif

	// Peace deal where we're not surrendering, value must equal cached value
	else if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		if (iTotalValueToMe >= GetCachedValueOfPeaceWithHuman())
		{
			return true;
		}
	}
#if defined(MOD_BALANCE_CORE_DEALS)
	//Does the offer contain a city and we're unhappy? Abort.
	else if (pDeal->ContainsItemType(TRADE_ITEM_CITIES))
	{
		if(!pDeal->IsPeaceTreatyTrade(eOtherPlayer))
		{
			if(GetPlayer()->IsEmpireUnhappy() || GetPlayer()->GetExcessHappiness() < 10)
			{
				return false;
			}
		}
	}
#endif
	// If we've gotten the deal to a point where we're happy, offer it up
	else if(iTotalValueToMe <= iAmountOverWeWillRequest && iTotalValueToMe >= iAmountUnderWeWillOffer)
	{
		return true;
	}
	else if (iTotalValueToMe > iAmountOverWeWillRequest)
	{
		bCantMatchOffer = true;
	}

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
	bCantMatchOffer = false;

	// Is this a peace deal?
	if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		pDeal->ClearItems();
		bMakeOffer = IsOfferPeace(eOtherPlayer, pDeal, true /*bEqualizingDeals*/);
	}
	else
	{
		int iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer;
		bMakeOffer = IsDealWithHumanAcceptable(pDeal, GC.getGame().getActivePlayer(), /*Passed by reference*/ iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, bCantMatchOffer);

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

			//DoAddCitiesToThem(pDeal, eOtherPlayer, bDontChangeTheirExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);

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

			DoAddCitiesToUs(pDeal, eOtherPlayer, bDontChangeMyExistingItems, iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

			// Make sure we haven't removed everything from the deal!
			if(pDeal->m_TradedItems.size() > 0)
			{
				bMakeOffer = IsDealWithHumanAcceptable(pDeal, GC.getGame().getActivePlayer(), /*Passed by reference*/ iTotalValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, /*passed by reference*/bCantMatchOffer);
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

	int iPercentOverWeWillRequest = GetDealPercentLeewayWithAI();
	int iPercentUnderWeWillOffer = -GetDealPercentLeewayWithAI();

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
	if(pDeal != GC.getGame().GetGameDeals()->GetTempDeal())
	{
		GC.getGame().GetGameDeals()->SetTempDeal(pDeal);
	}

	CvDeal* pCounterDeal = GC.getGame().GetGameDeals()->GetTempDeal();

	if(!bMakeOffer)
	{
		/////////////////////////////
		// See if there are items we can add or remove from either side to balance out the deal if it's not already even
		/////////////////////////////

		bool bUseEvenValue = true;

		DoAddVoteCommitmentToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
		DoAddVoteCommitmentToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

		DoAddResourceToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
		DoAddResourceToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

		DoAddOpenBordersToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ true, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, iDealDuration, bUseEvenValue);
		DoAddOpenBordersToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ true, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, iDealDuration, bUseEvenValue);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		if (MOD_DIPLOMACY_CIV4_FEATURES) {
			DoAddTechToThem(pDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddTechToUs(pDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);

			DoAddMapsToThem(pDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountOverWeWillRequest, bUseEvenValue);
			DoAddMapsToUs(pDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iAmountUnderWeWillOffer, bUseEvenValue);
		}
#endif

		DoAddGPTToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);
		DoAddGPTToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);

		DoAddGoldToThem(pCounterDeal, eOtherPlayer, /*bDontChangeTheirExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);
		DoAddGoldToUs(pCounterDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ false, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);

		DoRemoveGPTFromThem(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);
		DoRemoveGPTFromUs(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, iDealDuration, bUseEvenValue);

		DoRemoveGoldFromUs(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);
		DoRemoveGoldFromThem(pCounterDeal, eOtherPlayer, iTotalValue, iEvenValueImOffering, iEvenValueTheyreOffering, bUseEvenValue);

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
			GET_PLAYER(eOtherPlayer).GetDealAI()->GetDealValue(pDeal, iValueTheyThinkTheyreOffering, iValueTheyThinkTheyreGetting, /*bUseEvenValue*/ false);

			// They don't think they're getting enough for what's on their side of the table
			int iLowEndOfWhatTheyWillAccept = iValueTheyThinkTheyreOffering - (iValueTheyThinkTheyreOffering * GET_PLAYER(eOtherPlayer).GetDealAI()->GetDealPercentLeewayWithAI() / 100);
			if(iValueTheyThinkTheyreGetting < iLowEndOfWhatTheyWillAccept)
			{
				return false;
			}

			bMakeOffer = true;
		}
	}

	return bMakeOffer;
}

///// What is the value of pDeal?  Use either the "even function" or the "for us function" based on bUseEvenValue
//int CvDealAI::GetDealValue(CvDeal* pDeal, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
//{
//	return GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
//}

/// What do we think of a Deal?
int CvDealAI::GetDealValue(CvDeal* pDeal, int& iValueImOffering, int& iValueTheyreOffering, bool bUseEvenValue)
{
	int iDealValue = 0;
	iValueImOffering = 0;
	iValueTheyreOffering = 0;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iItemValue;

	bool bFromMe;
	PlayerTypes eOtherPlayer;

	eOtherPlayer = pDeal->m_eFromPlayer == eMyPlayer ? pDeal->m_eToPlayer : pDeal->m_eFromPlayer;

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

		iItemValue = GetTradeItemValue(it->m_eItemType, bFromMe, eOtherPlayer, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, it->m_iDuration, bUseEvenValue);

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
int CvDealAI::GetTradeItemValue(TradeableItems eItem, bool bFromMe, PlayerTypes eOtherPlayer, int iData1, int iData2, int iData3, bool bFlag1, int iDuration, bool bUseEvenValue)
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
	else if(eItem == TRADE_ITEM_RESOURCES)
		iItemValue = GetResourceValue(/*ResourceType*/ (ResourceTypes) iData1, /*Quantity*/ iData2, iDuration, bFromMe, eOtherPlayer);
	else if(eItem == TRADE_ITEM_CITIES)
		iItemValue = GetCityValue(/*iX*/ iData1, /*iY*/ iData2, bFromMe, eOtherPlayer, bUseEvenValue);
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
		iItemValue = GetVassalageValue(bFromMe, eOtherPlayer, bUseEvenValue);
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

	int iModifier;

	// While we have a big number shall we apply some modifiers to it?
	if(bFromMe)
	{
		// Approach is important
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iModifier = 150;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iModifier = 110;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iModifier = 100;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iModifier = 100;
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

		// Opinion also matters
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iModifier = 115;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iModifier = 140;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iModifier = 200;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Gold valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iReturnValue *= 100;
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

	int iPreCalculationDurationMultiplier;
	int iMultiplier;
	int iDivisor;
	int iPostCalculationDurationDivider;

	// We passed in Value, we want to know how much GPT we get for it
	if(bNumGPTFromValue)
	{
		iPreCalculationDurationMultiplier = 1;
		iMultiplier = 100;
		iDivisor = /*80*/ GC.getEACH_GOLD_PER_TURN_VALUE_PERCENT();
		iPostCalculationDurationDivider = iNumTurns;	// Divide value by number of turns to get GPT

		// Example: want amount of GPT for 100 value.
		// 100v * 1 = 100
		// 100 * 100 / 80 = 125
		// 125 / 20 turns = 6.25GPT
	}
	// We passed in an amount of GPT, we want to know how much it's worth
	else
	{
		iPreCalculationDurationMultiplier = iNumTurns;	// Multiply GPT by number of turns to get value
		iMultiplier = /*80*/ GC.getEACH_GOLD_PER_TURN_VALUE_PERCENT();
		iDivisor = 100;
		iPostCalculationDurationDivider = 1;

		// Example: want value for 6 GPT
		// 6GPT * 20 turns = 120
		// 120 * 80 / 100 = 96
		// 96 / 1 = 96v
	}

	// Convert based on the rules above
	int iReturnValue = iGPTorValue * iPreCalculationDurationMultiplier;
	iReturnValue *= iMultiplier;

	// While we have a big number shall we apply some modifiers to it?
	if(bFromMe)
	{
		// AI values it's GPT more highly because it's easy to exploit this
		// See whether we should multiply or divide
		if(!bNumGPTFromValue)
		{
			iReturnValue *= 130;
			iReturnValue /= 100;
		}
		else
		{
			iReturnValue *= 100;
			iReturnValue /= 130;
		}

		int iModifier;

		// Approach is important
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iModifier = 150;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iModifier = 110;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iModifier = 100;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iModifier = 100;
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
		if(!bNumGPTFromValue)
		{
			iReturnValue *= iModifier;
			iReturnValue /= 100;
		}
		else
		{
			iReturnValue *= 100;
			iReturnValue /= iModifier;
		}

		// Opinion also matters
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iModifier = 115;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iModifier = 140;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iModifier = 200;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Gold valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iModifier = 100;
			break;
		}

		// See whether we should multiply or divide
		if(!bNumGPTFromValue)
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

	// Sometimes we want to round up.  Let's say a the AI offers a deal to the human.  We have to ensure that the human can also offer that deal back and the AI will accept (and vice versa)
	if(bRoundUp)
	{
		iReturnValue += 99;
	}

	iReturnValue /= iDivisor;

	iReturnValue /= iPostCalculationDurationDivider;

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		iReturnValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetGPTforForValueExchange(iGPTorValue, bNumGPTFromValue, iNumTurns, !bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false, bRoundUp);

		iReturnValue /= 2;
	}

	return iReturnValue;
}

/// How much is a Resource worth?
int CvDealAI::GetResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Resource with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	CvAssert(pkResourceInfo != NULL);
	if (pkResourceInfo == NULL)
		return 0;

	ResourceUsageTypes eUsage = pkResourceInfo->getResourceUsage();

	// Luxury Resource
	if(eUsage == RESOURCEUSAGE_LUXURY)
	{
		if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(GetPlayer()->GetID(), eResource))
		{
			iItemValue = 0;
		}
		else
		{
			int iHappinessFromResource = pkResourceInfo->getHappiness();
			iItemValue += (iResourceQuantity * iHappinessFromResource * iNumTurns * 2);	// Ex: 1 Silk for 4 Happiness * 30 turns * 2 = 240

			// If we only have 1 of a Luxury then we value it much more
			if(bFromMe)
			{
				if(GetPlayer()->getNumResourceAvailable(eResource) == 1)
				{
					iItemValue *= 3;
					if(GetPlayer()->GetPlayerTraits()->GetLuxuryHappinessRetention() > 0)
					{
#if defined(MOD_BALANCE_CORE_DEALS)
						if (MOD_BALANCE_CORE_DEALS) 
						{
							iItemValue /= 4;
						}
#else
						iItemValue /= 2;
#endif
					}
				}
			}
		}
#if defined(MOD_BALANCE_CORE_DEALS)
	if (MOD_BALANCE_CORE_DEALS) 
	{
		CvCity* pLoopCity;
		int iCityLoop;
		for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
		{
			if(pLoopCity != NULL)
			{
				ResourceTypes eResourceDemanded = pLoopCity->GetResourceDemanded();
				if(eResourceDemanded != NO_RESOURCE)
				{
					//Will we get a WLTKD from this? We want it a bit more, please.
					if(eResourceDemanded == eResource)
					{
						iItemValue *= 3;
						iItemValue /= 2;
					}
				}
			}
		}
		//Let's consider how many resources each player has - if he has more than us, ours is worth more (and vice-versa).
		int iOtherHappiness = GET_PLAYER(eOtherPlayer).GetExtraHappinessPerLuxury();
		int iOurHappiness = GetPlayer()->GetExtraHappinessPerLuxury();
		//He's happier than us? Let's not be too liberal with our stuff.
		if(iOtherHappiness > iOurHappiness)
		{
			if(!bFromMe)
			{
				//How much is their stuff worth?
				switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
				{
					case MAJOR_CIV_OPINION_ALLY:
						iItemValue *= 115;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_FRIEND:
						iItemValue *= 110;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_FAVORABLE:
						iItemValue *= 105;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_NEUTRAL:
						iItemValue *= 100;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_COMPETITOR:
						iItemValue *= 85;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_ENEMY:
						iItemValue *= 65;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_UNFORGIVABLE:
						iItemValue *= 45;
						iItemValue /= 100;
						break;
				}
			}
			else
			{
				//How much is OUR stuff worth?
				switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
				{
					case MAJOR_CIV_OPINION_ALLY:
						iItemValue *= 85;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_FRIEND:
						iItemValue *= 90;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_FAVORABLE:
						iItemValue *= 95;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_NEUTRAL:
						iItemValue *= 100;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_COMPETITOR:
						iItemValue *= 135;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_ENEMY:
						iItemValue *= 160;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_UNFORGIVABLE:
						iItemValue *= 200;
						iItemValue /= 100;
						break;
				}
			}
		}
		//He is less happy than we are? We can give away, but only at a slight discount.
		else
		{
			if(!bFromMe)
			{
				//How much is their stuff worth?
				switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
				{
					case MAJOR_CIV_OPINION_ALLY:
						iItemValue *= 120;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_FRIEND:
						iItemValue *= 115;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_FAVORABLE:
						iItemValue *= 110;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_NEUTRAL:
						iItemValue *= 105;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_COMPETITOR:
						iItemValue *= 90;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_ENEMY:
						iItemValue *= 75;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_UNFORGIVABLE:
						iItemValue *= 65;
						iItemValue /= 100;
						break;
				}
			}
			else
			{
				//How much is OUR stuff worth?
				switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
				{
					case MAJOR_CIV_OPINION_ALLY:
						iItemValue *= 80;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_FRIEND:
						iItemValue *= 85;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_FAVORABLE:
						iItemValue *= 90;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_NEUTRAL:
						iItemValue *= 95;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_COMPETITOR:
						iItemValue *= 120;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_ENEMY:
						iItemValue *= 150;
						iItemValue /= 100;
						break;
					case MAJOR_CIV_OPINION_UNFORGIVABLE:
						iItemValue *= 180;
						iItemValue /= 100;
						break;
				}
			}
		}
	}
#endif
	}
	// Strategic Resource
	else if(eUsage == RESOURCEUSAGE_STRATEGIC)
	{
		//tricksy humans trying to sploit us
		if(!bFromMe)
		{
			// if we already have a big surplus of this resource
			if(GetPlayer()->getNumResourceAvailable(eResource) > GetPlayer()->getNumCities())
			{
				iResourceQuantity = 0;
			}
			else
			{
				iResourceQuantity = min(max(5,GetPlayer()->getNumCities()), iResourceQuantity);
			}
		}
		if(!GET_TEAM(GetPlayer()->getTeam()).IsResourceObsolete(eResource))
		{
			iItemValue += (iResourceQuantity * iNumTurns * 150 / 100);	// Ex: 5 Iron for 30 turns * 2 = value of 300
#if defined(MOD_DIPLOMACY_CITYSTATES)
			if (MOD_DIPLOMACY_CITYSTATES) {
				//Paper is super important - don't give it away too easily!
				ResourceTypes ePaperResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_PAPER", true);
				if(eResource == ePaperResource)
				{
					iItemValue *= iResourceQuantity;
				}
			}
#endif
		}
#if defined(MOD_BALANCE_CORE_DEALS)
		if (MOD_BALANCE_CORE_DEALS) 
		{
			//If they're stronger than us, do not give away strategic resources easily.
			if(GetPlayer()->GetMilitaryMight() < GET_PLAYER(eOtherPlayer).GetMilitaryMight())
			{
				iItemValue *= 15;
				iItemValue /= 10;
			}
			//Are they close, or far away? We should always be a bit more reluctant to give war resources to neighbors.
			if(GetPlayer()->GetProximityToPlayer(eOtherPlayer) >= PLAYER_PROXIMITY_CLOSE)
			{
				iItemValue *= 15;
				iItemValue /= 10;
			}
		}
#endif
		else
		{
			iItemValue = 0;
		}
	}

	// Increase value if it's from us and we don't like the guy
	if(bFromMe)
	{
		int iModifier = 0;
#if defined(MOD_BALANCE_CORE_DEALS)
		if (MOD_BALANCE_CORE_DEALS) 
		{
			//Lets use our DoF willingness to determine these values - introduce some variability.
			iModifier = GetPlayer()->GetDiplomacyAI()->GetDoFWillingness();
			iModifier *= -3;
			
			// Opinion also matters
			switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
			{
			case MAJOR_CIV_OPINION_ALLY:
				iModifier += 90;
				break;
			case MAJOR_CIV_OPINION_FRIEND:
				iModifier += 95;
				break;
			case MAJOR_CIV_OPINION_FAVORABLE:
				iModifier += 98;
				break;
			case MAJOR_CIV_OPINION_NEUTRAL:
				iModifier += 100;
				break;
			case MAJOR_CIV_OPINION_COMPETITOR:
				iModifier = 130;
				break;
			case MAJOR_CIV_OPINION_ENEMY:
				iModifier = 170;
				break;
			case MAJOR_CIV_OPINION_UNFORGIVABLE:
				iModifier = 300;
				break;
			default:
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Resource valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iModifier = 100;
				break;
			}
		}
#else
		// Opinion also matters
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
			iModifier = 100;
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iModifier = 175;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iModifier = 400;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iModifier = 1000;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Resource valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iModifier = 100;
			break;
		}
#endif
		// Approach is important
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iModifier += 300;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iModifier += 150;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
#if defined(MOD_BALANCE_CORE_DEALS)
		if (MOD_BALANCE_CORE_DEALS) 
		{
			iModifier += 90;	// Not forced value
		}
#else
			iModifier = 200;	// Forced value
#endif
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
#if defined(MOD_BALANCE_CORE_DEALS)
		if (MOD_BALANCE_CORE_DEALS) 
		{
			iModifier += 95;	// Not forced value
		}
#else
			iModifier = 200;	// Forced value
#endif
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iModifier += 100;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Resource valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iModifier += 100;
			break;
		}

#if defined(MOD_DIPLOMACY_CITYSTATES)
		if (MOD_DIPLOMACY_CITYSTATES) {
			//Paper is super important - don't give it away too easily!
			ResourceTypes ePaperResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_PAPER", true);
			if(eResource == ePaperResource)
			{
				iModifier *= iResourceQuantity;
			}
		}
#endif

		iItemValue *= iModifier;
		iItemValue /= 200;	// 200 because we've added two mods together
	}

	return iItemValue;
}

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
#if defined(MOD_GLOBAL_CITY_WORKING)
		for(int iI = 0; iI < pCity->GetNumWorkablePlots(); iI++)
#else
		for(int iI = 0; iI < NUM_CITY_PLOTS; iI++)
#endif
		{
			CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);
			if(NULL != pLoopPlot && pCity->GetID() == pLoopPlot->GetCityPurchaseID())
			{
				if(iI > 6)
				{
					iGoldValueOfPlots += goldPerPlot; // this is a bargain, but at least it's in the ballpark
				}
				if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					iGoldValueOfImprovedPlots += goldPerPlot * 25;
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

#if defined(MOD_BALANCE_CORE_DEALS)
	if (MOD_BALANCE_CORE_DEALS) 
	{
		if(!bFromMe)
		{
			int iHappiness = GetPlayer()->GetExcessHappiness();
			//Would this city cause us to become unhappy? It is worthless to us.
			if(iHappiness < pCity->getPopulation())
			{
				iItemValue = -1;
			}
			if(GetPlayer()->IsEmpireUnhappy())
			{
				iItemValue = -1;
			}
		}
		if(bFromMe)
		{
			//Are we trading away one of our core 3 cities? Bad idea.
			int iNumCities = GetPlayer()->getNumCities();
			if(iNumCities <= 3)
			{
				iItemValue = -1;
			}
		}
	}
#endif
	return iItemValue;
}

// How much is an embassy worth?
int CvDealAI::GetEmbassyValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Embassy with oneself.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 35;

	// Scale up or down by deal duration at this game speed
	CvGameSpeedInfo *pkStdSpeedInfo = GC.getGameSpeedInfo((GameSpeedTypes)GC.getSTANDARD_GAMESPEED());
	if (pkStdSpeedInfo)
	{
		iItemValue *= GC.getGame().getGameSpeedInfo().GetDealDuration();
		iItemValue /= pkStdSpeedInfo->GetDealDuration();
	}

	if(bFromMe)  // giving the other player an embassy in my capital
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
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetTradeAgreementValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		iItemValue /= 2;
	}

	return iItemValue;
}

/// How much in V-POINTS (aka value) is Open Borders worth?  You gotta admit that V-POINTS sound pretty cool though
int CvDealAI::GetOpenBordersValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of Open Borders with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true);

	// If we're friends, then OB is always equally valuable to both parties
	if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		return 50;

	int iItemValue = 0;

	// Me giving Open Borders to the other guy
	if(bFromMe)
	{
		// Approach is important
		switch(eApproach)
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue = 1000;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue = 100;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iItemValue = 20;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iItemValue = 50;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iItemValue = 75;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Open Borders valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue = 100;
			break;
		}

		// Opinion also matters
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
		{
		case MAJOR_CIV_OPINION_ALLY:
			iItemValue = 0;
			break;
		case MAJOR_CIV_OPINION_FRIEND:
			iItemValue *= 35;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_OPINION_FAVORABLE:
			iItemValue *= 70;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_OPINION_NEUTRAL:
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iItemValue *= 150;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iItemValue *= 400;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iItemValue = 10000;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Open Borders valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			break;
		}

		// If they're at war with our enemies then we're more likely to give them OB
		int iNumEnemiesAtWarWith = GetPlayer()->GetDiplomacyAI()->GetNumOurEnemiesPlayerAtWarWith(eOtherPlayer);
		if(iNumEnemiesAtWarWith >= 2)
		{
			iItemValue *= 10;
			iItemValue /= 100;
		}
		else if(iNumEnemiesAtWarWith == 1)
		{
			iItemValue *= 25;
			iItemValue /= 100;
		}

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
	}
	// Other guy giving me Open Borders
	else
	{
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

		// Reduce value by half if the other guy only has a single City
		if(GET_PLAYER(eOtherPlayer).getNumCities() == 1)
		{
			iItemValue *= 50;
			iItemValue /= 100;
		}

		// Boost value greatly if we are going for a culture win
		// If going for culture win always want open borders against civs we need influence on
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
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetOpenBordersValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		iItemValue /= 2;
	}

	return iItemValue;
}

/// How much is a Defensive Pact worth?
int CvDealAI::GetDefensivePactValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Defensive Pact with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue;

	// What is a Defensive Pact with eOtherPlayer worth to US?
	if(!bFromMe)
	{
		iItemValue = 100;
		//	// How strong are they compared to us?
		//	switch (GetPlayer()->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer))
		//	{
		//	case STRENGTH_PATHETIC:
		//		iItemValue = 10;
		//		break;
		//	case STRENGTH_WEAK:
		//		iItemValue = 40;
		//		break;
		//	case STRENGTH_POOR:
		//		iItemValue = 70;
		//		break;
		//	case STRENGTH_AVERAGE:
		//		iItemValue = 100;
		//		break;
		//	case STRENGTH_STRONG:
		//		iItemValue = 130;
		//		break;
		//	case STRENGTH_POWERFUL:
		//		iItemValue = 150;
		//		break;
		//	case STRENGTH_IMMENSE:
		//		iItemValue = 200;
		//		break;
		//	default:
		//		CvAssertMsg(false, "DEAL_AI: AI player has no valid MilitaryStrengthComparedToUs for Defensive Pact valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		//		iItemValue = 100;
		//		break;
		//	}
	}
	// How much do we value giving away a Defensive Pact?
	else
	{
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
			iItemValue = 100000;
			break;
		case MAJOR_CIV_OPINION_COMPETITOR:
			iItemValue = 100000;
			break;
		case MAJOR_CIV_OPINION_ENEMY:
			iItemValue = 100000;
			break;
		case MAJOR_CIV_OPINION_UNFORGIVABLE:
			iItemValue = 100000;
			break;
		default:
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Opinion for Defensive Pact valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue = 100000;
			break;
		}

		// Approach is important
		//switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
		//{
		//case MAJOR_CIV_APPROACH_HOSTILE:
		//	iItemValue *= 200;	// Value should already be increased above by Opinion as well
		//	break;
		//case MAJOR_CIV_APPROACH_GUARDED:
		//	iItemValue *= 100;	// If we're guarded against someone, getting a Defensive Pact is kinda nice
		//	break;
		//case MAJOR_CIV_APPROACH_AFRAID:
		//	iItemValue *= 80;		// If we're afraid of eOtherPlayer, we couldn't be happier to sign a Defensive Pact with them!
		//	break;
		//case MAJOR_CIV_APPROACH_FRIENDLY:
		//	iItemValue *= 100;
		//	break;
		//case MAJOR_CIV_APPROACH_NEUTRAL:
		//	iItemValue *= 100;
		//	break;
		//default:
		//	CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Defensive Pact valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		//	iItemValue *= 100;
		//	break;
		//}
		//iItemValue /= 100;
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetDefensivePactValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		iItemValue /= 2;
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
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetResearchAgreementValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		iItemValue /= 2;
	}

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
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetTradeAgreementValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		iItemValue /= 2;
	}

	return iItemValue;
}

/// How much is a Peace Treaty worth?
int CvDealAI::GetPeaceTreatyValue(PlayerTypes eOtherPlayer)
{
	DEBUG_VARIABLE(eOtherPlayer);

	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Peace Treaty with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return 0;

	// DEPRECATED

	//int iItemValue = 500;

	//// What I think me giving up peace is worth to them (if we're winning "our peace" is more valuable)
	//if (bFromMe)
	//{
	//	if (GetPlayer()->GetDiplomacyAI()->IsWantsPeaceWithPlayer(eOtherPlayer))
	//	{
	//		iItemValue = 200;
	//	}
	//}
	//// What I think them agreeing to peace with me is worth (if they're winning "their peace" is more valuable)
	//else
	//{
	//	if (GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsWantsPeaceWithPlayer(GetPlayer()->GetID()))
	//	{
	//		iItemValue = 200;
	//	}
	//}

	//// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	//if (bUseEvenValue)
	//{
	//	iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetPeaceTreatyValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

	//	iItemValue /= 2;
	//}

	//return iItemValue;
}

/// What is the value of peace with eWithTeam? NOTE: This deal item should be disabled if eWithTeam doesn't want to go to peace
int CvDealAI::GetThirdPartyPeaceValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Third Party Peace with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();

	PlayerTypes eWithPlayer = NO_PLAYER;

	// find the first player associated with the team
	for (uint ui = 0; ui < MAX_CIV_PLAYERS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).getTeam() == eWithTeam) 
		{
			eWithPlayer = ePlayer;
			break;
		}
	}

	CvAssertMsg(eWithPlayer != NO_PLAYER, "eWithPlayer could not be found");
	if (eWithPlayer == NO_PLAYER)
	{
		return 0;
	}

	WarProjectionTypes eWarProjection = pDiploAI->GetWarProjection(eWithPlayer);

	EraTypes eOurEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();

	MajorCivOpinionTypes eOpinionTowardsAskingPlayer = pDiploAI->GetMajorCivOpinion(eOtherPlayer);
	MajorCivOpinionTypes eOpinionTowardsWarPlayer = NO_MAJOR_CIV_OPINION_TYPE;

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
	// Major
	else
		eOpinionTowardsWarPlayer = pDiploAI->GetMajorCivOpinion(eWithPlayer);

	// From me
	if(bFromMe)
	{
		if(eWarProjection == WAR_PROJECTION_VERY_GOOD)
			iItemValue = 600;
		else if(eWarProjection == WAR_PROJECTION_GOOD)
			iItemValue = 400;
		else if(eWarProjection == WAR_PROJECTION_UNKNOWN)
			iItemValue = 250;
		else
			iItemValue = 200;

		// Add 50 gold per era
		int iExtraCost = eOurEra * 50;
		iItemValue += iExtraCost;

		// Minors
		if(bMinor)
		{
		}
		// Majors
		else
		{
			// Modify for our feelings towards the player we're at war with
			if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_UNFORGIVABLE)
			{
				iItemValue *= 300;
				iItemValue /= 100;
			}
			else if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_ENEMY)
			{
				iItemValue *= 200;
				iItemValue /= 100;
			}
		}

		// Modify for our feelings towards the asking player
		if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_ALLY)
		{
			iItemValue *= 30;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_FRIEND)
		{
			iItemValue *= 50;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_FAVORABLE)
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
	}
	// From them
	else
	{
		iItemValue = -10000;
	}

	return iItemValue;
}

/// What is the value of war with eWithPlayer?
int CvDealAI::GetThirdPartyWarValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Third Party War with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();

	// How much does this AI like to go to war? If it's a 3 or less, never accept
	int iWarApproachWeight = pDiploAI->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_WAR);
	if(bFromMe && iWarApproachWeight < 4)
		return 100000;


	PlayerTypes eWithPlayer = NO_PLAYER;
	// find the first player associated with the team
	for (uint ui = 0; ui < MAX_CIV_PLAYERS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if (GET_PLAYER(ePlayer).getTeam() == eWithTeam) 
		{
			eWithPlayer = ePlayer;
			break;
		}
	}
	WarProjectionTypes eWarProjection = pDiploAI->GetWarProjection(eWithPlayer);

	EraTypes eOurEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();

	MajorCivOpinionTypes eOpinionTowardsAskingPlayer = pDiploAI->GetMajorCivOpinion(eOtherPlayer);
	MajorCivOpinionTypes eOpinionTowardsWarPlayer = NO_MAJOR_CIV_OPINION_TYPE;
	MinorCivApproachTypes eMinorApproachTowardsWarPlayer = NO_MINOR_CIV_APPROACH;

	bool bMinor = false;

	// Minor
	if(GET_PLAYER(eWithPlayer).isMinorCiv())
	{
		bMinor = true;
		eMinorApproachTowardsWarPlayer = pDiploAI->GetMinorCivApproach(eWithPlayer);
	}
	// Major
	else
		eOpinionTowardsWarPlayer = pDiploAI->GetMajorCivOpinion(eWithPlayer);

	// From me
	if(bFromMe)
	{
		if(eWarProjection >= WAR_PROJECTION_GOOD)
			iItemValue = 400;
		else if(eWarProjection == WAR_PROJECTION_UNKNOWN)
			iItemValue = 600;
		else if(eWarProjection == WAR_PROJECTION_STALEMATE)
			iItemValue = 1000;
		else
			iItemValue = 50000;

		// Add 50 gold per era
		int iExtraCost = eOurEra * 50;
		iItemValue += iExtraCost;

		// Modify based on our War Approach
		int iWarBias = /*5*/ GC.getDEFAULT_FLAVOR_VALUE() - iWarApproachWeight;
		int iWarMod = iWarBias * 10;	// EX: 5 - War Approach of 9 = -4 * 10 = -40% cost
		iWarMod *= iItemValue;
		iWarMod /= 100;

		iItemValue += iWarMod;

		// Minor
		if(bMinor)
		{
			if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_FRIENDLY)
				iItemValue = 100000;
			else if(eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_PROTECTIVE)
				iItemValue = 100000;
		}
		// Major
		else
		{
			// Modify for our feelings towards the player we're would go to war with
			if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_UNFORGIVABLE)
			{
				iItemValue *= 25;
				iItemValue /= 100;
			}
			else if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_ENEMY)
			{
				iItemValue *= 50;
				iItemValue /= 100;
			}
			else if(eOpinionTowardsWarPlayer == MAJOR_CIV_OPINION_COMPETITOR)
			{
				iItemValue *= 75;
				iItemValue /= 100;
			}
		}

		// Modify for our feelings towards the asking player
		if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_ALLY)
		{
			iItemValue *= 50;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_FRIEND)
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_FAVORABLE)
		{
			iItemValue *= 85;
			iItemValue /= 100;
		}
		else if(eOpinionTowardsAskingPlayer == MAJOR_CIV_OPINION_COMPETITOR)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
	}

	// From them
	else
	{
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
	}

	return iItemValue;
}

/// What is the value of trading a vote commitment?
int CvDealAI::GetVoteCommitmentValue(bool bFromMe, PlayerTypes eOtherPlayer, int iProposalID, int iVoteChoice, int iNumVotes, bool bRepeal, bool bUseEvenValue)
{
	int iValue = 0;

	// Giving our votes to them - Higher value for voting on things we dislike
	if (bFromMe)
	{
		// Adjust based on LeagueAI
		CvLeagueAI::DesireLevels eDesire = GetPlayer()->GetLeagueAI()->EvaluateVoteForTrade(iProposalID, iVoteChoice, iNumVotes, bRepeal);
		switch(eDesire)
		{
		case CvLeagueAI::DESIRE_NEVER:
		case CvLeagueAI::DESIRE_STRONG_DISLIKE:
			iValue += 100000;
			break;
		case CvLeagueAI::DESIRE_DISLIKE:
			iValue += 300;
			break;
		case CvLeagueAI::DESIRE_WEAK_DISLIKE:
			iValue += 200;
			break;
		case CvLeagueAI::DESIRE_NEUTRAL:
		case CvLeagueAI::DESIRE_WEAK_LIKE:
		case CvLeagueAI::DESIRE_LIKE:
			iValue += 150;
			break;
		case CvLeagueAI::DESIRE_STRONG_LIKE:
		case CvLeagueAI::DESIRE_ALWAYS:
			iValue += 50;
			break;
		default:
			iValue += 100000;
			break;
		}

		// Adjust based on relationship
		CvAssert(eOtherPlayer != NO_PLAYER);
		if (eOtherPlayer != NO_PLAYER)
		{
			CvLeagueAI::AlignmentLevels eAlignment = GetPlayer()->GetLeagueAI()->EvaluateAlignment(eOtherPlayer);
			switch (eAlignment)
			{
			case CvLeagueAI::ALIGNMENT_LIBERATOR:
			case CvLeagueAI::ALIGNMENT_LEADER:
				iValue += -50;
				break;
			case CvLeagueAI::ALIGNMENT_SELF:
				CvAssertMsg(false, "ALIGNMENT_SELF found when evaluating a trade deal for delegates. Please send Anton your save file and version.");
				break;
			case CvLeagueAI::ALIGNMENT_ALLY:
				iValue += -35;
				break;
			case CvLeagueAI::ALIGNMENT_CONFIDANT:
			case CvLeagueAI::ALIGNMENT_FRIEND:
				iValue += -25;
				break;
			case CvLeagueAI::ALIGNMENT_NEUTRAL:
				break;
			case CvLeagueAI::ALIGNMENT_RIVAL:
				iValue += 25;
				break;
			case CvLeagueAI::ALIGNMENT_HATRED:
				iValue += 50;
				break;
			case CvLeagueAI::ALIGNMENT_ENEMY:
			case CvLeagueAI::ALIGNMENT_WAR:
				iValue += 100000;
				break;
			default:
				break;
			}

			MajorCivApproachTypes eOtherPlayerApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
			if (eOtherPlayerApproach == MAJOR_CIV_APPROACH_HOSTILE || eOtherPlayerApproach == MAJOR_CIV_APPROACH_WAR)
			{
				iValue += 100000;
			}
		}
	}
	// Giving their votes to us - Higher value for voting on things we like
	else
	{
		// Adjust based on LeagueAI
		CvLeagueAI::DesireLevels eDesire = GetPlayer()->GetLeagueAI()->EvaluateVoteForTrade(iProposalID, iVoteChoice, iNumVotes, bRepeal);
		switch(eDesire)
		{
		case CvLeagueAI::DESIRE_NEVER:
		case CvLeagueAI::DESIRE_STRONG_DISLIKE:
		case CvLeagueAI::DESIRE_WEAK_DISLIKE:
		case CvLeagueAI::DESIRE_NEUTRAL:
			iValue += -100000;
			break;
		case CvLeagueAI::DESIRE_WEAK_LIKE:
			iValue += 50;
			break;
		case CvLeagueAI::DESIRE_LIKE:
			iValue += 100;
			break;
		case CvLeagueAI::DESIRE_STRONG_LIKE:
			iValue += 150;
			break;
		case CvLeagueAI::DESIRE_ALWAYS:
			iValue += 200;
			break;
		default:
			iValue += -100000;
			break;
		}

		// Adjust based on their vote total - Having lots of votes left means they could counter these ones and exploit us
		if (GC.getGame().GetGameLeagues()->GetNumActiveLeagues() > 0)
		{
			CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
			if (pLeague)
			{
				float fVotesRatio = (float)iNumVotes / (float)pLeague->CalculateStartingVotesForMember(eOtherPlayer);
				if (fVotesRatio > 0.5f)
				{
					// More than half their votes...they probably aren't going to screw us
				}
				else if (fVotesRatio > 0.25f)
				{
					// They have a lot of remaining votes
					iValue += -20;
				}
				else
				{
					// They have a hoard of votes
					iValue += -40;
				}
			}
		}
	}

	iValue = MAX(iValue, 0);

	// Adjust based on how many votes
	iValue *= iNumVotes;

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

						// If adding this to the deal doesn't take it over the limit, do it
#if defined(MOD_AI_SMART_DEALS)
						if((!MOD_AI_SMART_DEALS || iItemValue > 0) && (iItemValue + iTotalValue <= iAmountOverWeWillRequest))
#else
						if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
#endif
						{
							pDeal->AddVoteCommitment(eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
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

	if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// Can't already be a Vote Commitment in the Deal
			if(!pDeal->IsVoteCommitmentTrade(eThem) && !pDeal->IsVoteCommitmentTrade(eMyPlayer))
			{
				CvLeagueAI::VoteCommitmentList vDesiredCommitments = GET_PLAYER(eThem).GetLeagueAI()->GetDesiredVoteCommitments(eMyPlayer);
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
#if defined(MOD_AI_SMART_DEALS)
						if((!MOD_AI_SMART_DEALS || iItemValue > 0) && (-iItemValue + iTotalValue >= iAmountUnderWeWillOffer))
#else
						if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
#endif
						{
							pDeal->AddVoteCommitment(eMyPlayer, iProposalID, iVoteChoice, iNumVotes, bRepeal);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
						}
					}
				}
			}
		}
	}
}

/// See if adding a Resource to their side of the deal helps even out pDeal
void CvDealAI::DoAddResourceToThem(CvDeal* pDeal, PlayerTypes eThem, bool bDontChangeTheirExistingItems, int& iTotalValue, int& iValueImOffering, int& iValueTheyreOffering, int iAmountOverWeWillRequest, int iDealDuration, bool bUseEvenValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Them, but them is us.  Please show Jon");

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

				// Quantity is always 1 if it's a Luxury, 5 if Strategic
				iResourceQuantity = 1;

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration, bUseEvenValue);

					// If adding this to the deal doesn't take it over the limit, do it
#if defined(MOD_AI_SMART_DEALS)
					if((!MOD_AI_SMART_DEALS || iItemValue > 0) && (iItemValue + iTotalValue <= iAmountOverWeWillRequest))
#else
					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
#endif
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

				// Quantity is always 1 if it's a Luxury, 5 if Strategic
				iResourceQuantity = min(5, iResourceQuantity);	// 5 or what they have, whichever is less

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration, bUseEvenValue);

					// If adding this to the deal doesn't take it over the limit, do it
#if defined(MOD_AI_SMART_DEALS)
					if((!MOD_AI_SMART_DEALS || iItemValue > 0) && (iItemValue + iTotalValue <= iAmountOverWeWillRequest))
#else
					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
#endif
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
					iResourceQuantity = min(5, iResourceQuantity);	// 5 or what we have, whichever is less
				}

				// See if we can actually trade it to them
				if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ true, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration, bUseEvenValue);

					// If adding this to the deal doesn't take it under the min limit, do it
#if defined(MOD_AI_SMART_DEALS)
					if((!MOD_AI_SMART_DEALS || iItemValue > 0) && (-iItemValue + iTotalValue >= iAmountUnderWeWillOffer))
#else
					if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
#endif
					{
						// Try to change the current item if it already exists, otherwise add it
						if(!pDeal->ChangeResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration);
							iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
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
#if defined(MOD_AI_SMART_DEALS)
					if((!MOD_AI_SMART_DEALS || iItemValue > 0) && (iItemValue + iTotalValue <= iAmountOverWeWillRequest))
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
#if defined(MOD_AI_SMART_DEALS)
					if((!MOD_AI_SMART_DEALS || iItemValue > 0) && (-iItemValue + iTotalValue >= iAmountUnderWeWillOffer))
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
#if defined(MOD_AI_SMART_DEALS)
					if((!MOD_AI_SMART_DEALS || iItemValue > 0) && (iItemValue + iTotalValue <= iAmountOverWeWillRequest))
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
#if defined(MOD_AI_SMART_DEALS)
					if((!MOD_AI_SMART_DEALS || iItemValue > 0) && (-iItemValue + iTotalValue >= iAmountUnderWeWillOffer))
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

	// If we're not the one surrendering here, don't bother
	if(pDeal->GetSurrenderingPlayer() != eMyPlayer)
		return;

	// Don't change things
	if(bDontChangeMyExistingItems)
		return;

	// We don't owe them anything
	if(iTotalValue <= 0)
		return;

	CvPlayer* pLosingPlayer = GetPlayer();
	CvPlayer* pWinningPlayer = &GET_PLAYER(eThem);

	// If the player only has 1 City then we can't get any more from him
	if(pLosingPlayer->getNumCities() == 1)
		return;

	//int iCityValue = 0;

	int iCityDistanceFromWinnersCapital = 0;
	int iWinnerCapitalX = -1, iWinnerCapitalY = -1;

	// If winner has no capital then we can't use proximity - it will stay at 0
	CvCity* pWinnerCapital = pWinningPlayer->getCapitalCity();
	if(pWinnerCapital != NULL)
	{
		iWinnerCapitalX = pWinningPlayer->getCapitalCity()->getX();
		iWinnerCapitalY = pWinningPlayer->getCapitalCity()->getY();
	}

	// Create vector of the losing players' Cities so we can see which are the closest to the winner
	CvWeightedVector<int> viCityProximities;

	// Loop through all of the loser's Cities
	CvCity* pLoopCity;
	int iCityLoop;
	for(pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
	{
		// Get total city value of the loser
		//iCityValue += GetCityValue(pLoopCity->getX(), pLoopCity->getY(), /*bFromMe*/ true, eThem, bUseEvenValue);
		//iCityValue += GetCityValue(pLoopCity->getX(), pLoopCity->getY(), /*bFromMe*/ true, eThem, /*bUseEvenValue*/ true);

		// If winner has no capital, Distance defaults to 0
		if(pWinnerCapital != NULL)
		{
			iCityDistanceFromWinnersCapital = plotDistance(iWinnerCapitalX, iWinnerCapitalY, pLoopCity->getX(), pLoopCity->getY());
		}

		// Don't include the capital in the list of Cities the winner can receive
		if(!pLoopCity->isCapital())
		{
			viCityProximities.push_back(pLoopCity->GetID(), iCityDistanceFromWinnersCapital);
		}
	}

	// Sort the vector based on distance from winner's capital
	viCityProximities.SortItems();

	// Loop through sorted Cities and add them to the deal if they're under the amount to give up - start from the back of the list, because that's where the CLOSEST cities are
	int iSortedCityID;
	//			for (int iSortedCityIndex = 0; iSortedCityIndex < viCityProximities.size(); iSortedCityIndex++)
	for(int iSortedCityIndex = viCityProximities.size() - 1; iSortedCityIndex > -1 ; iSortedCityIndex--)
	{
		iSortedCityID = viCityProximities.GetElement(iSortedCityIndex);
		pLoopCity = pLosingPlayer->getCity(iSortedCityID);

		//iCityValue = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), bMeSurrendering, eOtherPlayer, /*bUseEvenValue*/ true);

		// See if we can actually trade it to them
		if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
			//if (pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_OPEN_BORDERS))
		{
			int iItemValue = GetCityValue(pLoopCity->getX(), pLoopCity->getY(), /*bFromMe*/ true, eThem, bUseEvenValue);
			//int iItemValue = GetTradeItemValue(TRADE_ITEM_CITIES, /*bFromMe*/ true, eThem, pLoopCity->getX(), pLoopCity->getY(), iDealDuration, bUseEvenValue);
			//int iItemValue = GetTradeItemValue(TRADE_ITEM_OPEN_BORDERS, /*bFromMe*/ true, eThem, -1, -1, iDealDuration, bUseEvenValue);

			// If adding this to the deal doesn't take it under the min limit, do it
			if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
			{
				//pDeal->AddOpenBorders(eMyPlayer, iDealDuration);
				pDeal->AddCityTrade(eMyPlayer, iSortedCityID);
				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
			}
		}

		// City is worth less than what is left to be added to the deal, so add it
		//if (iCityValue < iCityValueToSurrender)
		//{
		//	if (pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
		//	{
		//		pDeal->AddCityTrade(eLosingPlayer, iSortedCityID);

		//		// Remove GPT from this City so we don't give more than we can support
		//		iGPTToGive -= pLoopCity->getYieldRate(YIELD_GOLD);

		//		iCityValueToSurrender -= iCityValue;
		//	}
		//}
	}
}

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
				int iNumGoldAlreadyInTrade = pDeal->GetGoldTrade(eThem);
				iNumGold += iNumGoldAlreadyInTrade;
				iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eThem, TRADE_ITEM_GOLD));
				//iNumGold = min(iNumGold, GET_PLAYER(eThem).GetTreasury()->GetGold());

				if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eThem, iNumGold))
				{
					pDeal->AddGoldTrade(eThem, iNumGold);
				}

				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
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
				int iNumGoldAlreadyInTrade = pDeal->GetGoldTrade(eMyPlayer);
				iNumGold += iNumGoldAlreadyInTrade;
				iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eMyPlayer, TRADE_ITEM_GOLD));
				//iNumGold = min(iNumGold, GET_PLAYER(eMyPlayer).GetTreasury()->GetGold());

				if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eMyPlayer, iNumGold))
				{
					pDeal->AddGoldTrade(eMyPlayer, iNumGold);
				}

				iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
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
					int iNumGPTAlreadyInTrade = pDeal->GetGoldPerTurnTrade(eThem);
					iNumGPT += iNumGPTAlreadyInTrade;
					iNumGPT = min(iNumGPT, GET_PLAYER(eThem).calculateGoldRate());

					if(iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eThem, iNumGPT, iDealDuration))
					{
						pDeal->AddGoldPerTurnTrade(eThem, iNumGPT, iDealDuration);
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
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
					int iNumGPTAlreadyInTrade = pDeal->GetGoldPerTurnTrade(eMyPlayer);
					iNumGPT += iNumGPTAlreadyInTrade;
					iNumGPT = min(iNumGPT, GET_PLAYER(eMyPlayer).calculateGoldRate());

					if(iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration))
					{
						pDeal->AddGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration);
					}

					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
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
			int iValueToRemove = -iTotalValue * 150;
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

		// Add the peace items to the deal so that we actually stop the war
		int iPeaceTreatyLength = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
		pDeal->AddPeaceTreaty(eMyPlayer, iPeaceTreatyLength);
		pDeal->AddPeaceTreaty(eOtherPlayer, iPeaceTreatyLength);

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
			GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(-iValueImOffering);
			}
		}
		// AI is asking human to surrender
		else if(ePeaceTreatyImWillingToAccept > PEACE_TREATY_WHITE_PEACE)
		{
			pDeal->SetSurrenderingPlayer(eOtherPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToAccept);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyImWillingToAccept, /*bMeSurrendering*/ false);

			// Store the value of the deal with the human so that we have a number to use for renegotiation (if necessary)
			int iValueImOffering, iValueTheyreOffering;
			GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(iValueTheyreOffering);
			}
		}
		else
		{
			// if the case is that we both want white peace, don't forget to add the city-states into the peace deal.
			DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);
		}

		int iPeaceTreatyLength = GC.getGame().getGameSpeedInfo().getPeaceDealDuration();
		pDeal->AddPeaceTreaty(eMyPlayer, iPeaceTreatyLength);
		pDeal->AddPeaceTreaty(eOtherPlayer, iPeaceTreatyLength);

		result = true;
	}

	return result;
}

/// Add appropriate items to pDeal based on what type of PeaceTreaty eTreaty is
void CvDealAI::DoAddItemsToDealForPeaceTreaty(PlayerTypes eOtherPlayer, CvDeal* pDeal, PeaceTreatyTypes eTreaty, bool bMeSurrendering)
{
	int iPercentGoldToGive = 0;
	int iPercentGPTToGive = 0;
	bool bGiveOpenBorders = false;
	bool bGiveOnlyOneCity = false;
	int iPercentCitiesGiveUp = 0; /* 100 = all but capital */
	bool bGiveUpStratResources = false;
	bool bGiveUpLuxuryResources = false;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool bBecomeMyVassal = false;
#endif

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
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		bBecomeMyVassal = true;
#endif
		break;

	case PEACE_TREATY_CAPITULATION:
		iPercentCitiesGiveUp = 33;
		iPercentGoldToGive = 100;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		bBecomeMyVassal = true;
#endif
		break;

	case PEACE_TREATY_UNCONDITIONAL_SURRENDER:
		iPercentCitiesGiveUp = 100;
		iPercentGoldToGive = 100;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		bBecomeMyVassal = true;
#endif
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
#if defined(MOD_BALANCE_CORE_DEALS)
					if (MOD_BALANCE_CORE_DEALS && iCityValue != -1)
					{
#endif
					if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
					{
						pDeal->AddCityTrade(eLosingPlayer, iSortedCityID);
						iCityValueToSurrender -= iCityValue;
					}
#if defined(MOD_BALANCE_CORE_DEALS)
					}
#endif

				}
			}
		}
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if(MOD_DIPLOMACY_CIV4_FEATURES && bBecomeMyVassal)
	{
		bool bLostCapital = pLosingPlayer->IsHasLostCapital();
		bool bGoingForConquest = pWinningPlayer->GetDiplomacyAI()->IsGoingForWorldConquest();

		// AIs only want vassalage if winner is not going for conquest victory or the loser lost his capital
		if(pWinningPlayer->isHuman() || !bGoingForConquest || bLostCapital)
		{
			if(pDeal->IsPossibleToTradeItem(eLosingPlayer, eWinningPlayer, TRADE_ITEM_VASSALAGE))
			{
				pDeal->AddVassalageTrade(eLosingPlayer);
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
			if (pMinor->GetMinorCivAI()->IsPermanentWar(GetTeam()))
			{
				continue;
			}

			// Add peace with this minor to the deal
			// slewis - if there is not a peace deal with them already on the table and we can trade it
			if(!pDeal->IsThirdPartyPeaceTrade(GetPlayer()->GetID(), pMinor->getTeam()) && pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eToPlayer, TRADE_ITEM_THIRD_PARTY_PEACE, pMinor->getTeam()))
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
			if (pMinor->GetMinorCivAI()->IsPermanentWar(GET_PLAYER(eToPlayer).getTeam()))
			{
				continue;
			}

			// Add peace with this minor to the deal
			// slewis - if there is not a peace deal with them already on the table and we can trade it
			if(!pDeal->IsThirdPartyPeaceTrade(eToPlayer, pMinor->getTeam()) && pDeal->IsPossibleToTradeItem(eToPlayer, GetPlayer()->GetID(), TRADE_ITEM_THIRD_PARTY_PEACE, pMinor->getTeam()))
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

	int iGold = pDeal->GetGoldAvailable(eOtherPlayer, TRADE_ITEM_GOLD);

	// Don't ask for too much
	int iMaxGold = 200 + (GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetCurrentEra() * 150);
	iGold = min(iMaxGold, iGold);

	if(pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_GOLD, iGold))
	{
		pDeal->AddGoldTrade(eOtherPlayer, iGold);

		return true;
	}

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
			eLuxuryFromThem = eResource;
			break;
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

		return bDealAcceptable;
	}

	return false;
}

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

		return bDealAcceptable;
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

		return bDealAcceptable;
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
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, true, false, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable;
}

/// This function called when human player enters diplomacy
void CvDealAI::DoTradeScreenOpened()
{
	TeamTypes eActiveTeam = GC.getGame().getActiveTeam();
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	if(GET_TEAM(GetTeam()).isAtWar(eActiveTeam))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eActivePlayer);
		PeaceTreatyTypes ePeaceTreatyImWillingToAccept = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToAccept(eActivePlayer);

		// Does the AI actually want peace?
		if(ePeaceTreatyImWillingToOffer >= PEACE_TREATY_WHITE_PEACE && ePeaceTreatyImWillingToAccept >= PEACE_TREATY_WHITE_PEACE)
		{
			// Clear out UI deal first, we're going to add a couple things to it
			auto_ptr<ICvDeal1> pUIDeal(GC.GetEngineUserInterface()->GetScratchDeal());
			CvDeal* pkUIDeal = GC.UnwrapDealPointer(pUIDeal.get());
			pkUIDeal->ClearItems();

			CvDeal* pDeal = GC.getGame().GetGameDeals()->GetTempDeal();
			pDeal->ClearItems();
			pDeal->SetFromPlayer(eActivePlayer);	// The order of these is very important!
			pDeal->SetToPlayer(eMyPlayer);	// The order of these is very important!

			// AI is surrendering
			if(ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE)
			{
				pkUIDeal->SetSurrenderingPlayer(eMyPlayer);
				pkUIDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToOffer);

				DoAddItemsToDealForPeaceTreaty(eActivePlayer, pDeal, ePeaceTreatyImWillingToOffer, /*bMeSurrendering*/ true);

				// Store the value of the deal with the human so that we have a number to use for renegotiation (if necessary)
				int iValueImOffering, iValueTheyreOffering;
				GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
				SetCachedValueOfPeaceWithHuman(-iValueImOffering);
			}
			// AI is asking human to surrender
			else if(ePeaceTreatyImWillingToAccept > PEACE_TREATY_WHITE_PEACE)
			{
				pkUIDeal->SetSurrenderingPlayer(eActivePlayer);
				pkUIDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToAccept);

				DoAddItemsToDealForPeaceTreaty(eActivePlayer, pDeal, ePeaceTreatyImWillingToAccept, /*bMeSurrendering*/ false);

				// Store the value of the deal with the human so that we have a number to use for renegotiation (if necessary)
				int iValueImOffering, iValueTheyreOffering;
				GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, /*bUseEvenValue*/ false);
				SetCachedValueOfPeaceWithHuman(iValueTheyreOffering);
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

	int iItemValue = 0;
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

		// Buyer can't have plot revealed
		if(pPlot->isRevealed(pBuyer->getTeam()))
		{
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
		if(pPlot->IsNaturalWonder())
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

		int iModifier = (GC.getGame().countCivTeamsAlive() - iNumRevealed) * 100 / GC.getGame().countCivTeamsAlive();

		if(iModifier < 50)
			iModifier = 50;

		iPlotValue *= iModifier;
		iPlotValue /= 100;

		iItemValue += iPlotValue;
	}
	iItemValue /= 100;

	if(bFromMe)
	{
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

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetMapValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		iItemValue /= 2;
	}

	return iItemValue;
}

// How much is a technology worth?
int CvDealAI::GetTechValue(TechTypes eTech, bool bFromMe, PlayerTypes eOtherPlayer)
{
	int iItemValue = 0;
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);

	int iTurnsLeft;
	int iTechEra = pkTechInfo->GetEra();

	if(bFromMe)
	{
		iTurnsLeft = GET_PLAYER(eOtherPlayer).GetPlayerTechs()->GetResearchTurnsLeft(eTech, true);
	}
	else
	{
		iTurnsLeft = GetPlayer()->GetPlayerTechs()->GetResearchTurnsLeft(eTech, true);
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
					iTechMod *= 3;
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
				iTechMod += 4;
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
				iTechMod += 3;
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
				iTechMod += 3;
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
				iTechMod += 3;
			}
		}
	}

	if(pkTechInfo->IsTechTrading())
	{
		if(!GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING) && !GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		{
			iTechMod += 3;
		}
	}

	if(pkTechInfo->IsResearchAgreementTradingAllowed())
	{
		if(GC.getGame().isOption(GAMEOPTION_RESEARCH_AGREEMENTS) && !GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		{
			iTechMod += 3;
		}
	}

	if(pkTechInfo->IsResearchAgreementTradingAllowed())
	{
		if(GC.getGame().isOption(GAMEOPTION_RESEARCH_AGREEMENTS) && !GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		{
			iTechMod += 3;
		}
	}

	// BASE COST = (TurnsLeft * 30 * (era ^ 0.7))	-- Ancient Era is 1, Classical Era is 2 because I incremented it
	iItemValue = iTurnsLeft * /*30*/ GC.getGame().getGameSpeedInfo().getTechCostPerTurnMultiplier();
	float fItemMultiplier = (float)(pow( (double) std::max(1, (iTechEra + 1)), (double) /*0.7*/ GC.getTECH_COST_ERA_EXPONENT() ) );
	iItemValue = (int)(iItemValue * fItemMultiplier);

	// Apply the Modifier
	iItemValue *= (100 + iTechMod);
	iItemValue /= 100;

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
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Technology valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
				break;
		}
		iItemValue /= 100;

	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	//if(bUseEvenValue)
	//{
	//	iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetTechValue(eTech, !bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

	//	iItemValue /= 2;
	//}
	
	return iItemValue;
}

/// How much is Vassalage worth?
int CvDealAI::GetVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Vassalage Agreement with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	CvDiplomacyAI* m_pDiploAI = GetPlayer()->GetDiplomacyAI();

	if(bFromMe)						
	{
		// Don't want to capitulate
		if(!m_pDiploAI->IsVassalageAcceptable(eOtherPlayer))
		{
			return 100000;
		}

		// Initial Vassalage deal value at 500 -- edited to 400
		iItemValue = 400;

		// Add deal value based on number of wars player is currently fighting (including with minors)
		iItemValue += iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).getAtWarCount(false));

		// Increase deal value based on number of vassals player already has
		iItemValue += iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetNumVassals());

		// ex: 2 wars and 2 vassals = 500 + 1000 + 1000 = a 2500 gold before modifiers!

		// The point of Vassalage is protection, if they're not militarily dominant - what's the point?
		switch(m_pDiploAI->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer))
		{
			case STRENGTH_IMMENSE:
				iItemValue *= 90;
				break;
			case STRENGTH_POWERFUL:
				iItemValue *= 100;
				break;
			case STRENGTH_STRONG:
				iItemValue *= 125;
				break;
			case STRENGTH_AVERAGE:
				iItemValue *= 200;
				break;
			case STRENGTH_POOR:
			case STRENGTH_WEAK:
			case STRENGTH_PATHETIC:
			default:
				iItemValue *= 1000;
				break;
		}
		iItemValue /= 100;

		switch(GetPlayer()->GetProximityToPlayer(eOtherPlayer))
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

		switch(m_pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
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
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Vassalage valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
				break;
		}
		iItemValue /= 100;
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		iItemValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetVassalageValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

		iItemValue /= 2;
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

		return bDealAcceptable;
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

	int iTechLoop;
	TechTypes eTech;
	TechTypes eTechWeWant = NO_TECH;

	// See if the other player has a technology to trade
	for(iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		eTech = (TechTypes) iTechLoop;

		// Let's not ask for the tech we're currently researching
		if(eTech == GetPlayer()->GetPlayerTechs()->GetCurrentResearch())
		{
			continue;
		}

		// Don't ask for a tech that costs 15 or more turns to research
		if(GetPlayer()->GetPlayerTechs()->GetResearchTurnsLeft(eTech, true) > 15)
		{
			continue;
		}

		// Can they sell that tech?
		if(pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_TECHS, eTech))
		{
			eTechWeWant = eTech;
			break;
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

		return bDealAcceptable;
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
		CvAssertMsg(false, "Don't ask a human to become a vassal!");
	}

	return bDealAcceptable;
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
				if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
				{
					pDeal->AddMapTrade(eThem);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
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

				// If adding this to the deal doesn't take it under the min limit, do it
				if(-iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
				{
					pDeal->AddMapTrade(eMyPlayer);
					iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
				}
				else
				{
					// Does adding in the territory map help even out pDeal?
					iItemValue = GetTradeItemValue(TRADE_ITEM_MAPS, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, -1, bUseEvenValue);

					// If adding this to the deal doesn't take it under the min limit, do it
					if(iItemValue + iTotalValue >= iAmountUnderWeWillOffer)
					{
						pDeal->AddMapTrade(eThem);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
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

					// If adding this to the deal doesn't take it over the limit, do it
					if(iItemValue + iTotalValue <= iAmountOverWeWillRequest)
					{
						pDeal->AddTechTrade(eThem, eTech);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
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

					// If adding this to the deal doesn't take it over the limit, do it
					if(iItemValue + iTotalValue <= iAmountUnderWeWillOffer)
					{
						pDeal->AddTechTrade(eMyPlayer, eTech);
						iTotalValue = GetDealValue(pDeal, iValueImOffering, iValueTheyreOffering, bUseEvenValue);
					}
				}
			}
		}
	}
}
#endif
