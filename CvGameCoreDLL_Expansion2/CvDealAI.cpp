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

/// Helper function which returns this player's TeamType
TeamTypes CvDealAI::GetTeam()
{
	return m_pPlayer->getTeam();
}

/// How much are we willing to back off on what our perceived value of a deal is with an AI player to make something work?
int CvDealAI::GetDealPercentLeeway(PlayerTypes eOtherPlayer) const
{
	int iPercent = 0;
	switch (m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer))
	{
	case MAJOR_CIV_OPINION_ALLY:
		iPercent = 125;
		break;
	case MAJOR_CIV_OPINION_FRIEND:
		iPercent = 100;
		break;
	case MAJOR_CIV_OPINION_FAVORABLE:
		iPercent = 75;
		break;
	case MAJOR_CIV_OPINION_NEUTRAL:
		iPercent = 50;
		break;
	case MAJOR_CIV_OPINION_COMPETITOR:
	case MAJOR_CIV_OPINION_ENEMY:
	case MAJOR_CIV_OPINION_UNFORGIVABLE:
		iPercent = 25;
		break;
	default:
		iPercent = 25;
		break;
	}

	return iPercent;
}

bool CvDealAI::WithinAcceptableRange(PlayerTypes ePlayer, int iValue)
{
	//if zero, definitely good!
	if (iValue == 0)
		return true;

	int iLeewayFromZero = GetDealPercentLeeway(ePlayer);

	if ((iValue >= (iLeewayFromZero * -1)) && (iValue <= iLeewayFromZero))
		return true;

	return false;
}

bool CvDealAI::BothSidesIncluded(CvDeal* pDeal)
{
	if (pDeal->GetDemandingPlayer() != NO_PLAYER)
		return true;

	if (pDeal->m_bIsGift)
		return true;

	//humans sometimes give nice things!
	if (GET_PLAYER(pDeal->m_eFromPlayer).isHuman() && GetDealValue(pDeal) > 0)
		return true;

	return (pDeal->GetFromPlayerValue() > 0 && pDeal->GetToPlayerValue() > 0);
}

bool CvDealAI::TooMuchAdded(PlayerTypes ePlayer, int iTotalValue, int iItemValue, bool bFromUs)
{
	//already good? Don't add anything!
	if (WithinAcceptableRange(ePlayer, iTotalValue))
		return true;

	//good now? then let's do it! (need to respect the negative if we're adding to a negative deal)
	int iNewValue = bFromUs ? (iTotalValue - iItemValue) : iTotalValue + iItemValue;
	if (WithinAcceptableRange(ePlayer, iNewValue))
		return false;
	
	//Is the new value taking us too high? Let's bring it back down.
	if (bFromUs && iNewValue < -200)
		return true;

	//Is the new value taking us too low? Let's bring it back up.
	if (!bFromUs && iNewValue > 200)
		return true;

	//still not good enough, but trending in the right direction? Carry on.
	return bFromUs ? (iTotalValue <= 0) : (iTotalValue >= 0);
}

/// Offer up a deal to this AI, and see if he accepts
DealOfferResponseTypes CvDealAI::DoHumanOfferDealToThisAI(CvDeal* pDeal)
{
	DealOfferResponseTypes eResponse = NO_DEAL_RESPONSE_TYPE;
	DiploUIStateTypes eUIState = NO_DIPLO_UI_STATE;

	const char* szText = "";
	LeaderheadAnimationTypes eAnimation = NO_LEADERHEAD_ANIM;

	PlayerTypes eFromPlayer = pDeal->GetOtherPlayer(GetPlayer()->GetID()); // Playing it safe, should be OK to use pDeal->GetFromPlayer() but code was using GetActivePlayer so maybe the From field wasn't always the human (although in my testing it was fine!)

	bool bFromIsActivePlayer = eFromPlayer == GC.getGame().getActivePlayer();

	int iDealValue = GetDealValue(pDeal);

	bool bCantMatchOffer;

	//if a human sent this, don't modify their offer, that's annoying.
	if (iDealValue > 0)
		pDeal->SetDoNotModifyFrom(true);
	else if (iDealValue < 0)
		pDeal->SetDoNotModifyTo(true);

	bool bDealAcceptable = IsDealWithHumanAcceptable(pDeal, eFromPlayer, iDealValue, &bCantMatchOffer, false);


	// If they're actually giving us more than we're asking for (e.g. a gift) then accept the deal
	if(!pDeal->IsPeaceTreatyTrade(eFromPlayer) && iDealValue > 0)
	{
		bDealAcceptable = true;
	}


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
		else if (iDealValue == INT_MAX)
		{
			bDealAcceptable = false;
			iDealValue = -500;
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
			bool bFromMe = (kDeal.GetFromPlayer() == GetPlayer()->GetID());
			int iValueImOffering = bFromMe ? kDeal.GetFromPlayerValue() : kDeal.GetToPlayerValue();
			int iValueTheyreOffering = bFromMe ? kDeal.GetToPlayerValue() : kDeal.GetFromPlayerValue();
			gDLL->sendNetDealAccepted(eFromPlayer, GetPlayer()->GetID(), pDllDeal.get(), iDealValue, iValueImOffering, iValueTheyreOffering);
		}
		// Deal between AI players, we can process it immediately
		else
		{
			bool bFromMe = (kDeal.GetFromPlayer() == GetPlayer()->GetID());
			int iValueImOffering = bFromMe ? kDeal.GetFromPlayerValue() : kDeal.GetToPlayerValue();
			int iValueTheyreOffering = bFromMe ? kDeal.GetToPlayerValue() : kDeal.GetFromPlayerValue();
			DoAcceptedDeal(eFromPlayer, kDeal, iDealValue, iValueImOffering, iValueTheyreOffering);
		}

		if(pDeal)
		{
			pDeal->SetRequestingPlayer(NO_PLAYER);
		}

	}
	// We want more from this Deal
	else if (GetPlayer()->GetDiplomacyAI()->IsActHostileTowardsHuman(eFromPlayer))
	{
		eResponse = DEAL_RESPONSE_INSULTING;
		eUIState = DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER;

		if (bFromIsActivePlayer)
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_REJECT_INSULTING);
			eAnimation = LEADERHEAD_ANIM_NO;
		}
	}
	// Pretty bad deal for us
	else
	{
		eResponse = DEAL_RESPONSE_UNACCEPTABLE;
		eUIState = DIPLO_UI_STATE_TRADE_AI_REJECTS_OFFER;

		if (bFromIsActivePlayer)
		{
			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_REJECT_UNACCEPTABLE);
			eAnimation = LEADERHEAD_ANIM_NO;
		}
	}

	if(bFromIsActivePlayer)
	{
		// Modify response if the player's offered this deal a lot
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
void CvDealAI::DoAcceptedDeal(PlayerTypes eFromPlayer, CvDeal kDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering)
{
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

		m_pPlayer->SetCachedValueOfPeaceWithHuman(0);
		LeaderheadAnimationTypes eAnimation = LEADERHEAD_ANIM_POSITIVE;


		// This signals Lua to do some interface cleanup, we only want to do this on the local machine.
		if(GC.getGame().getActivePlayer() == eFromPlayer)
			gDLL->DoClearDiplomacyTradeTable();

		DiploUIStateTypes eUIState = NO_DIPLO_UI_STATE;

		const char* szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE);


		// We made a demand and they gave in
		if(kDeal.GetDemandingPlayer() == GetPlayer()->GetID())
		{
			if(GC.getGame().getActivePlayer() == eFromPlayer)
			{
				szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_AI_DEMAND);
				gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
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

		if (kDeal.GetPeaceTreatyType() == NO_PEACE_TREATY_TYPE)
		{

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
				if (kDeal.GetDemandingPlayer() != eFromPlayer && iDealValueToMe > 0)
					GetPlayer()->GetDiplomacyAI()->ChangeRecentTradeValue(eFromPlayer, (iDealValueToMe / 10));
			}

		}


		if(GC.getGame().getActivePlayer() == eFromPlayer)
			GC.GetEngineUserInterface()->SetOfferTradeRepeatCount(0);

		// If this was a peace deal then use that animation instead
		if(kDeal.GetPeaceTreatyType() != NO_PEACE_TREATY_TYPE)
		{
			eAnimation = LEADERHEAD_ANIM_PEACEFUL;

			szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE);

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

	PlayerTypes eFromPlayer = pDeal->GetOtherPlayer(GetPlayer()->GetID()); // Playing it safe, should be OK to use pDeal->GetFromPlayer() but code was using GetActivePlayer so maybe the From field wasn't always the human (although in my testing it was fine!)
	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iValueWillingToGiveUp = 0;

	CvDiplomacyAI* pDiploAI = GET_PLAYER(eMyPlayer).GetDiplomacyAI();
	
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	MajorCivApproachTypes eApproach = pDiploAI->GetMajorCivApproach(eFromPlayer, /*bHideTrueFeelings*/ false);
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

			//much harder if vassalage on the table.
			if (pDeal->IsVassalageTrade(eFromPlayer) || pDeal->IsVassalageTrade(eMyPlayer))
			{
				if (GetPlayer()->IsAITeammateOfHuman())
				{
					eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
				}
				else
				{
					iOddsOfGivingIn -= 100;
				}
			}

			//no third party war, that's weird.
			if (pDeal->ContainsItemType(TRADE_ITEM_THIRD_PARTY_WAR, eFromPlayer) || pDeal->ContainsItemType(TRADE_ITEM_THIRD_PARTY_WAR, eMyPlayer))
				iOddsOfGivingIn -= 1000;

			// Unforgivable: AI will never give in
			if (pDiploAI->GetMajorCivOpinion(eFromPlayer) == MAJOR_CIV_OPINION_UNFORGIVABLE)
			{
				bHostile = true;
				iOddsOfGivingIn -= 50;
			}
			// Hostile: AI will never give in
			if (eApproach == MAJOR_CIV_APPROACH_HOSTILE)
			{
				bHostile = true;
				iOddsOfGivingIn -= 50;
			}
			// They are very far away and have no units near us (from what we can tell): AI will never give in
			if (eProximity <= PLAYER_PROXIMITY_FAR && eMilitaryPosture == AGGRESSIVE_POSTURE_NONE)
			{
				bWeak = true;
				iOddsOfGivingIn -= 50;
			}
			// Our military is stronger: AI will never give in
			if (eMilitaryStrength < STRENGTH_AVERAGE && eEconomicStrength < STRENGTH_AVERAGE)
			{
				bWeak = true;
				iOddsOfGivingIn -= 50;
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
			if (iRand > iOddsOfGivingIn || iOddsOfGivingIn <= 0)
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

				// Embassy
				case TRADE_ITEM_ALLOW_EMBASSY:
				{
					if (GetPlayer()->IsAITeammateOfHuman())
					{
						eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
					}
					else
					{
						iValueDemanded += 10;
					}
					break;
				}

				// Open Borders
				case TRADE_ITEM_OPEN_BORDERS:
				{
					if (GetPlayer()->IsAITeammateOfHuman())
					{
						eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
					}
					else
					{
						iValueDemanded += 25;
					}
					break;
				}

				// Other stuff
				case TRADE_ITEM_TECHS:
				case TRADE_ITEM_MAPS:
				case TRADE_ITEM_THIRD_PARTY_PEACE:
				case TRADE_ITEM_VASSALAGE_REVOKE:
				{
					if (GetPlayer()->IsAITeammateOfHuman())
					{
						eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
					}
					else
					{
						iValueDemanded += 75;
					}
					break;
				}
				
				// AI will never agree to give these up
				case TRADE_ITEM_CITIES:
				case TRADE_ITEM_DEFENSIVE_PACT:
				case TRADE_ITEM_RESEARCH_AGREEMENT:
				case TRADE_ITEM_THIRD_PARTY_WAR:
				default:
					eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
					break;
				}
			}
		}

		if (iValueDemanded == 0)
			eResponse = DEMAND_RESPONSE_REFUSE_TOO_SOON;
		// No illegal items in the demand
		else if (eResponse == NO_DEAL_RESPONSE_TYPE)
		{
			if(iValueDemanded <= iValueWillingToGiveUp)
				eResponse = DEMAND_RESPONSE_ACCEPT;
			else
				eResponse = DEMAND_RESPONSE_REFUSE_TOO_MUCH;
		}
	}

	// Have to send AI response through the network  - it affects AI behavior
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

bool CvDealAI::IsDealWithHumanAcceptable(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValueToMe, bool* bCantMatchOffer, bool bFirstPass)
{
	CvAssertMsg(GET_PLAYER(eOtherPlayer).isHuman(), "DEAL_AI: Trying to see if AI will accept a deal with human player... but it's not human.  Please show Jon.");

	*bCantMatchOffer = false;

	if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		if (pDeal->GetRequestingPlayer() == GetPlayer()->GetID() && (iTotalValueToMe > 0))
		{
			return true;
		}
	}


#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// We're offering help to a player
	if (MOD_DIPLOMACY_CIV4_FEATURES && GetPlayer()->GetDiplomacyAI()->IsOfferingGift(eOtherPlayer))
	{
		return true;
	}
#endif
	//special case for peacetime single return cities...
	if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer) && pDeal->ContainsItemType(TRADE_ITEM_CITIES, eOtherPlayer) && pDeal->GetNumItems() == 1)
	{
		int iCityLoop;
		for (CvCity* pLoopCity = GET_PLAYER(eOtherPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eOtherPlayer).nextCity(&iCityLoop))
		{
			if (pDeal->IsCityInDeal(eOtherPlayer, pLoopCity->GetID()))
			{
				if (pLoopCity->getOriginalOwner() == pDeal->GetToPlayer() && !pLoopCity->IsRazing() && !pLoopCity->IsResistance())
				{
					iTotalValueToMe = GetDealValue(pDeal);
					return true;
				}
			}
		}
	}

	// We're surrendering
	if (pDeal->GetSurrenderingPlayer() == GetPlayer()->GetID())
	{
		int iMaxPeace = (GetCachedValueOfPeaceWithHuman() * -1);
		if (iTotalValueToMe < 0)
		{
			iTotalValueToMe *= -1;
		}
		if (iTotalValueToMe <= iMaxPeace)
		{
			return true;
		}
	}

	// Peace deal where we're not surrendering, value must equal cached value
	else if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		int iMaxPeace = GetCachedValueOfPeaceWithHuman();
		if (iTotalValueToMe >= iMaxPeace)
		{
			return true;
		}
	}

	// Now do the valuation
	iTotalValueToMe = GetDealValue(pDeal);

	// Important: check invalid return value!
	if (iTotalValueToMe==INT_MAX || iTotalValueToMe==(INT_MAX * -1))
		return false;

	// If we've gotten the deal to a point where we're happy, offer it up
	if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer) && !bFirstPass)
	{
		//Is this a good deal for both parties?
		if (WithinAcceptableRange(eOtherPlayer, iTotalValueToMe) && BothSidesIncluded(pDeal))
		{
			return true;
		}
		//Are they offer much more than we can afford? Let's lowball, but let them know.
		else if (iTotalValueToMe > 0)
		{
			*bCantMatchOffer = true;
			return true;
		}
	}
	//This is to get the AI to actually make an offer the first time through.
	else if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer) && bFirstPass)
	{
		return false;
	}
	
	return false;
}


/// Try to even out the value on both sides.  If bFavorMe is true we'll bias things in our favor if necessary
bool CvDealAI::DoEqualizeDealWithHuman(CvDeal* pDeal, PlayerTypes eOtherPlayer, bool& bDealGoodToBeginWith, bool& bCantMatchOffer)
{
	bool bMakeOffer;
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	DEBUG_VARIABLE(eMyPlayer);

	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	CvAssertMsg(eMyPlayer != eOtherPlayer, "DEAL_AI: Trying to equalize human deal, but both players are the same.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	//reset these
	pDeal->SetDoNotModifyFrom(false);
	pDeal->SetDoNotModifyTo(false);


	if (pDeal->GetNumItems() <= 0)
	{
		return false;
	}

	GetPlayer()->GetDiplomacyAI()->SetCantMatchDeal(eOtherPlayer, false);


	// Is this a peace deal?
	if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		pDeal->ClearItems();
		bMakeOffer = IsOfferPeace(eOtherPlayer, pDeal, true /*bEqualizingDeals*/);
	}
	else
	{
		int iTotalValue = 0;

		bMakeOffer = IsDealWithHumanAcceptable(pDeal, eOtherPlayer, /*Passed by reference*/ iTotalValue, &bCantMatchOffer, true);
		bDealGoodToBeginWith = bMakeOffer;

		// If we set this pointer again it clears the data out!
		if (pDeal != GC.getGame().GetGameDeals().GetTempDeal())
		{
			GC.getGame().GetGameDeals().SetTempDeal(pDeal);
		}

		CvDeal* pCounterDeal = GC.getGame().GetGameDeals().GetTempDeal();

		if(!bMakeOffer)
		{
			//if a human sent this, don't modify their offer, that's annoying.
			if (iTotalValue > 0)
				pDeal->SetDoNotModifyFrom(true);
			else if (iTotalValue < 0)
				pDeal->SetDoNotModifyTo(true);

			/////////////////////////////
			// See if there are items we can add or remove from either side to balance out the deal if it's not already even
			/////////////////////////////
			int iLoops = 0;
			while (iLoops < 5 && !WithinAcceptableRange(eOtherPlayer, iTotalValue))
			{
				if (iTotalValue > 0)
				{
					DoAddItemsToUs(pCounterDeal, eOtherPlayer, iTotalValue);
				}
				else if (iTotalValue < 0)
				{
					DoAddItemsToThem(pCounterDeal, eOtherPlayer, iTotalValue);
				}

				iTotalValue = GetDealValue(pDeal);
				iLoops++;
			}

			// Make sure we haven't removed everything from the deal!
			if (BothSidesIncluded(pCounterDeal))
			{
				bMakeOffer = WithinAcceptableRange(eOtherPlayer, iTotalValue);
			}

			// Make sure we haven't removed everything from the deal!
			if (bMakeOffer)
			{
				bMakeOffer = IsDealWithHumanAcceptable(pDeal, eOtherPlayer, /*Passed by reference*/ iTotalValue, /*passed by reference*/&bCantMatchOffer, false);
				if (bCantMatchOffer)
				{
					GetPlayer()->GetDiplomacyAI()->SetCantMatchDeal(eOtherPlayer, true);
				}
				if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer))
				{
					//Getting 'error' values in this deal? It is bad, abort!
					if (iTotalValue == INT_MAX)
					{
						return false;
					}
				}
				//Let's not lowball, it leads to spam.
				if (iTotalValue > 0 && !bCantMatchOffer)
				{
					return true;
				}

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

	int iTotalValue = GetDealValue(pDeal);
	if (iTotalValue == MAX_INT)
		return false;

	bool bMakeOffer = false;

	//reset these
	pDeal->SetDoNotModifyFrom(false);
	pDeal->SetDoNotModifyTo(false);

	if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);
	}


	/////////////////////////////
	// Outline the boundaries for an acceptable deal
	/////////////////////////////
	if (pDeal->GetNumItems() <= 0)
	{
		return false;
	}

	GetPlayer()->GetDiplomacyAI()->SetCantMatchDeal(eOtherPlayer, false);


	// Is this a peace deal?
	if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		pDeal->ClearItems();
		bMakeOffer = IsOfferPeace(eOtherPlayer, pDeal, true /*bEqualizingDeals*/);
	}
	else
	{
		// Deal is already even enough for us
		if (WithinAcceptableRange(eOtherPlayer, iTotalValue) && BothSidesIncluded(pDeal))
		{
			bMakeOffer = true;
		}
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
		int iLoops = 0;
		while (iLoops < 3 && !WithinAcceptableRange(eOtherPlayer, iTotalValue))
		{
			if (iTotalValue > 0)
			{
				DoAddItemsToUs(pCounterDeal, eOtherPlayer, iTotalValue);
			}
			else if (iTotalValue < 0)
			{
				DoAddItemsToThem(pCounterDeal, eOtherPlayer, iTotalValue);
			}

			iTotalValue = GetDealValue(pDeal);
			iLoops++;
		}

		// Make sure we haven't removed everything from the deal!
		if(pCounterDeal->m_TradedItems.size() > 0)
		{
			bMakeOffer = WithinAcceptableRange(eOtherPlayer, iTotalValue) && BothSidesIncluded(pCounterDeal);
		}
	}

	return bMakeOffer;
}

/// What do we think of a Deal?
int CvDealAI::GetDealValue(CvDeal* pDeal, bool bLogging)
{
	int iDealValue = 0;
	if (pDeal->GetDuration() == -1)
		pDeal->SetDuration(GC.getGame().getGameSpeedInfo().GetDealDuration());

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
	pDeal->SetFromPlayerValue(0);
	pDeal->SetToPlayerValue(0);
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
			iItemValue = GetTradeItemValue(it->m_eItemType, bFromMe, eOtherPlayer, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, it->m_iDuration, bLogging);
			it->m_iValue = iItemValue;
		}


		//if the item is invalid, return invalid value for the whole deal
		if (iItemValue==INT_MAX)
		{
			pDeal->SetFromPlayerValue(INT_MAX);
			pDeal->SetToPlayerValue(INT_MAX);
			return iItemValue;
		}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		if (MOD_DIPLOMACY_CIV4_FEATURES) {
			// Item is worth 20% less if its owner is a vassal
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

		iItemValue *= iValueMultiplier;
		iDealValue += iItemValue;

		if (bFromMe)
			pDeal->ChangeFromPlayerValue(iItemValue);
		else
			pDeal->ChangeToPlayerValue(iItemValue);
#endif
	}

	return iDealValue;
}

/// What is a particular item worth?
int CvDealAI::GetTradeItemValue(TradeableItems eItem, bool bFromMe, PlayerTypes eOtherPlayer, int iData1, int iData2, int iData3, bool bFlag1, int iDuration, bool bLogging)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to get deal item value for trading to oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eItem != TRADE_ITEM_NONE, "DEAL_AI: Trying to get value of TRADE_ITEM_NONE.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	//clear cache if outdated
	if (GC.getGame().getTurnSlice() != m_iDealItemValuesTurnSlice)
	{
		m_iDealItemValuesTurnSlice = GC.getGame().getTurnSlice();
		m_dealItemValues.clear();
	}

	//cache lookup
	SDealItemValueParams currentCall(eItem,bFromMe,eOtherPlayer,iData1,iData2,iData3,bFlag1,iDuration);
	if (m_dealItemValues.find(currentCall) != m_dealItemValues.end())
		return m_dealItemValues[currentCall];

	//actual computation
	int iItemValue = 0;

	if(eItem == TRADE_ITEM_GOLD)
		iItemValue = GetGoldForForValueExchange(/*Gold Amount*/ iData1, /*bNumGoldFromValue*/ false, bFromMe, eOtherPlayer, /*bRoundUp*/ false);
	else if(eItem == TRADE_ITEM_GOLD_PER_TURN)
		iItemValue = GetGPTforForValueExchange(/*Gold Per Turn Amount*/ iData1, /*bNumGPTFromValue*/ false, iDuration, bFromMe, eOtherPlayer, /*bRoundUp*/ false, !GET_PLAYER(eOtherPlayer).isHuman(), bLogging);
	else if (eItem == TRADE_ITEM_RESOURCES)
	{
		// precalculate, it's expensive
		int iCurrentNetGoldOfReceivingPlayer = bFromMe ? GET_PLAYER(eOtherPlayer).GetTreasury()->CalculateBaseNetGold() : m_pPlayer->GetTreasury()->CalculateBaseNetGold();
		iItemValue = GetResourceValue(/*ResourceType*/ (ResourceTypes)iData1, /*Quantity*/ iData2, iDuration, bFromMe, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);
	}
	else if (eItem == TRADE_ITEM_CITIES)
	{
		//Don't even out city values ...
		CvCity* pCity = GC.getMap().plot(/*iX*/ iData1, /*iY*/ iData2)->getPlotCity();
		PlayerTypes eBuyer = bFromMe ? eOtherPlayer : m_pPlayer->GetID();
		PlayerTypes eSeller = bFromMe ? m_pPlayer->GetID() : eOtherPlayer;
		iItemValue = GetCityValueForBuyer(pCity, eSeller, eBuyer);
	}
	else if(eItem == TRADE_ITEM_ALLOW_EMBASSY)
		iItemValue = GetEmbassyValue(bFromMe, eOtherPlayer);
	else if(eItem == TRADE_ITEM_OPEN_BORDERS)
		iItemValue = GetOpenBordersValue(bFromMe, eOtherPlayer);
	else if(eItem == TRADE_ITEM_DEFENSIVE_PACT)
		iItemValue = GetDefensivePactValue(bFromMe, eOtherPlayer);
	else if(eItem == TRADE_ITEM_RESEARCH_AGREEMENT)
		iItemValue = GetResearchAgreementValue(bFromMe, eOtherPlayer);
	else if(eItem == TRADE_ITEM_PEACE_TREATY)
		iItemValue = GetPeaceTreatyValue(eOtherPlayer);
	else if(eItem == TRADE_ITEM_THIRD_PARTY_PEACE)
		iItemValue = GetThirdPartyPeaceValue(bFromMe, eOtherPlayer, /*eWithTeam*/ (TeamTypes)iData1, bLogging);
	else if(eItem == TRADE_ITEM_THIRD_PARTY_WAR)
		iItemValue = GetThirdPartyWarValue(bFromMe, eOtherPlayer, /*eWithTeam*/ (TeamTypes)iData1, bLogging);
	else if(eItem == TRADE_ITEM_VOTE_COMMITMENT)
		iItemValue = GetVoteCommitmentValue(bFromMe, eOtherPlayer, iData1, iData2, iData3, bFlag1);
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_MAPS)
		iItemValue = GetMapValue(bFromMe, eOtherPlayer);
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_TECHS)
		iItemValue = GetTechValue(/*TechType*/ (TechTypes) iData1, bFromMe, eOtherPlayer);
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_VASSALAGE)
		iItemValue = GetVassalageValue(bFromMe, eOtherPlayer, GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()));
	else if(MOD_DIPLOMACY_CIV4_FEATURES && eItem == TRADE_ITEM_VASSALAGE_REVOKE)
		iItemValue = GetRevokeVassalageValue(bFromMe, eOtherPlayer, GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()));
#endif

	CvAssertMsg(iItemValue >= 0, "DEAL_AI: Trade Item value is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	//cache update
	m_dealItemValues[currentCall] = iItemValue;

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

	// Sometimes we want to round up. Let's say the AI offers a deal to the human. We have to ensure that the human can also offer that deal back and the AI will accept (and vice versa)
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
int CvDealAI::GetGPTforForValueExchange(int iGPTorValue, bool bNumGPTFromValue, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue, bool bRoundUp, bool bLogging)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of GPT with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (iGPTorValue <= 0)
		return -1;

	int iValueTimes100 = 0;

	// We passed in Value, we want to know how much GPT we get for it
	if(bNumGPTFromValue)
	{
		// Sometimes we want to round up. Let's say the AI offers a deal to the human. We have to ensure that the human can also offer that deal back and the AI will accept (and vice versa)
		if (bRoundUp)
		{
			int iRound = 0;
			while (iGPTorValue % iNumTurns != 0 && iRound < 100)
			{
				iGPTorValue++;
				iRound++;
			}
		}

		iValueTimes100 = iGPTorValue;
		iValueTimes100 /= iNumTurns;
	}
	else
	{
		if (!bLogging && bFromMe && iGPTorValue > (GetPlayer()->calculateGoldRate() - 2))
			return MAX_INT;

		iValueTimes100 = (iGPTorValue * iNumTurns);

		//let's assume an interest rate of 0.5% per turn, no compounding
		int iInterestPercent = min(50, 100 * (iNumTurns * /*5*/ GC.getEACH_GOLD_PER_TURN_VALUE_PERCENT()) / 1000);

		//subtract interest. 100 gold now is better than 100 gold in the future
		iValueTimes100 -= (iValueTimes100*iInterestPercent) / 100;

		// Sometimes we want to round up. Let's say the AI offers a deal to the human. We have to ensure that the human can also offer that deal back and the AI will accept (and vice versa)
		if (bRoundUp)
		{
			int iRound = 0;
			while (iValueTimes100 % iNumTurns != 0 && iRound < 100)
			{
				iGPTorValue++;
				iRound++;
			}
		}
	}

	int iReturnValue = iValueTimes100;

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if(bUseEvenValue)
	{
		iReturnValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetGPTforForValueExchange(iGPTorValue, bNumGPTFromValue, iNumTurns, !bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false, !GET_PLAYER(eOtherPlayer).isHuman(), bRoundUp);

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

	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	CvAssert(pkResourceInfo != NULL);
	if (pkResourceInfo == NULL)
		return 0;

	//Integer zero check...
	if (iNumTurns <= 0)
		iNumTurns = 1;

	CvDeal* pRenewDeal = GetPlayer()->GetDiplomacyAI()->GetDealToRenew(eOtherPlayer);

	//how much happiness from one additional luxury?
	int iBaseHappiness = 0;
	if (bFromMe)
		iBaseHappiness += GetPlayer()->GetHappinessFromLuxury(eResource);
	else
		iBaseHappiness += GET_PLAYER(eOtherPlayer).GetHappinessFromLuxury(eResource);

	int iItemValue = (iBaseHappiness+GC.getGame().getCurrentEra())*iNumTurns/2;

	//Let's look at flavors for resources
	int iFlavorResult = 0;
	int iFlavors = 0;
	for (int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		int iResourceFlavor = pkResourceInfo->getFlavorValue((FlavorTypes)i);
		if (iResourceFlavor > 0)
		{
			int iPersonalityFlavorValue = GetPlayer()->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)i);
			//Has to be above average to affect price. Will usually result in a x2-x3 modifier
			iFlavorResult += ((iResourceFlavor + iPersonalityFlavorValue) / 6);
			iFlavors++;
		}
	}
	if ((iFlavorResult > 0) && (iFlavors > 0))
	{
		//Get the average multiplier from the number of Flavors being considered.
		iItemValue *= (iFlavorResult / iFlavors);
	}
			
	if (bFromMe)
	{
		//Every x gold in net GPT will increase resource value by 1, up to the value of the item itself (so never more than double).
		int iGPT = int(0.25+sqrt(iCurrentNetGoldOfReceivingPlayer/3.));
		if (iGPT > 0)
			iItemValue += min(iGPT,iItemValue);
	}
	else
	{
		//Every x gold in net GPT will increase resource value by 1, up to the value of the item itself (so never more than double).
		int iGPT = int(0.25+sqrt(iCurrentNetGoldOfReceivingPlayer/4.));
		if (iGPT > 0)
			iItemValue += min(iGPT,iItemValue);
	}

	if (bFromMe)
	{
		int iNumAvailable = GetPlayer()->getNumResourceTotal(eResource);

		if (pRenewDeal)
			iNumAvailable += pRenewDeal->GetNumResourcesInDeal(GetPlayer()->GetID(), eResource);

		if (GetPlayer()->IsEmpireUnhappy() && iNumAvailable == 1)
		{
			return INT_MAX;
		}
		if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(GetPlayer()->GetID(), eResource))
		{
			return INT_MAX;
		}
		if (iNumAvailable == 1)
		{
			int iFactor = GetPlayer()->GetPlayerTraits()->GetLuxuryHappinessRetention() ? 2 : 3;
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
		int iOtherHappiness = GET_PLAYER(eOtherPlayer).GetHappinessFromResources() + GET_PLAYER(eOtherPlayer).GetHappinessFromResourceVariety();
		int iOurHappiness = GetPlayer()->GetHappinessFromResources() + GetPlayer()->GetHappinessFromResourceVariety();
		//He's happier than us?
		if (iOtherHappiness >= iOurHappiness)
		{
			iItemValue *= 10;
			iItemValue /= 11;
		}
		//He is less happy than we are?
		else
		{
			iItemValue *= 11;
			iItemValue /= 10;
		}

		//How much is OUR stuff worth?
		switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
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
			iItemValue *= 100;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 200;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_WAR:
			iItemValue *= 250;
			iItemValue /= 100;
			break;
		}

		return iItemValue;
	}
	else
	{
		int iNumAvailable = GetPlayer()->getNumResourceTotal(eResource);
		if (pRenewDeal)
			iNumAvailable -= pRenewDeal->GetNumResourcesInDeal(eOtherPlayer, eResource);

		if (GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(GetPlayer()->GetID(), eResource))
			return 0;
		if (iNumAvailable > 0 && !GetPlayer()->GetPlayerTraits()->IsImportsCountTowardsMonopolies())
			return 0;

		if (GetPlayer()->IsEmpireUnhappy())
		{
			iItemValue += GetPlayer()->GetUnhappiness() * 2;
		}

		if (GetPlayer()->GetPlayerTraits()->IsImportsCountTowardsMonopolies() && GetPlayer()->GetMonopolyPercent(eResource) < GC.getGame().GetGreatestPlayerResourceMonopolyValue(eResource))
		{
			//we don't want resources that won't get us a bonus.
			int iNumResourceOwned = GetPlayer()->getNumResourceTotal(eResource, false);
			int iNumResourceImported = iNumAvailable;
			if (iNumResourceOwned == 0 && iNumResourceImported >= 0)
			{
				bool bBad = false;
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					if (pkResourceInfo->getYieldChangeFromMonopoly((YieldTypes)iJ) > 0)
					{
						bBad = true;
						break;
					}
				}
				if (!bBad)
				{
					iItemValue *= (100 + GetPlayer()->GetMonopolyPercent(eResource));
					iItemValue /= 100;
				}
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
		int iOtherHappiness = GET_PLAYER(eOtherPlayer).GetHappinessFromResources() + GET_PLAYER(eOtherPlayer).GetHappinessFromResourceVariety();
		int iOurHappiness = GetPlayer()->GetHappinessFromResources() + GetPlayer()->GetHappinessFromResourceVariety();
		//He's happier than us?
		if (iOtherHappiness >= iOurHappiness)
		{
			iItemValue *= 11;
			iItemValue /= 10;
		}
		//He is less happy than we are?
		else
		{
			iItemValue *= 10;
			iItemValue /= 11;
		}

		//How much is THEIR stuff worth?
		switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
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
			iItemValue *= 100;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 75;
			iItemValue /= 100;
			break;
		case MAJOR_CIV_APPROACH_WAR:
			iItemValue *= 50;
			iItemValue /= 100;
			break;
		}

		return iItemValue;
	}
}

//Ratio between 50 and 200.
int CvDealAI::GetResourceRatio(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, ResourceTypes eResource, int iNumInTrade)
{
	bool bImSelling = ePlayer == GetPlayer()->GetID();
	int iPlayer1 = GET_PLAYER(ePlayer).getNumResourceTotal(eResource, true);
	int iPlayer2 = GET_PLAYER(eOtherPlayer).getNumResourceTotal(eResource, true);

	CvDeal* pRenewDeal = m_pPlayer->GetDiplomacyAI()->GetDealToRenew(eOtherPlayer);
	if (pRenewDeal)
	{
		int iRenewAmount = pRenewDeal->GetNumResourcesInDeal(bImSelling ? ePlayer : eOtherPlayer, eResource);

		if (bImSelling)
		{
			iPlayer2 -= iRenewAmount;
			iPlayer1 += iRenewAmount;
		}
		else
		{
			iPlayer1 -= iRenewAmount;
			iPlayer2 += iRenewAmount;
		}
	}
	
	if (bImSelling)
		iPlayer1 -= iNumInTrade;
	else
		iPlayer2 -= iNumInTrade;

	int iRatio = 100;
	if (bImSelling)
		iRatio = (iPlayer2 * 100) / max(1, iPlayer1);
	else
		iRatio = (iPlayer1 * 100) / max(1, iPlayer2);

	//min half, max double
	return range(iRatio, 50, 200);
}

/// How much is a Resource worth?
int CvDealAI::GetStrategicResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Resource with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	//this is to reduce rounding errors
	int iValueScale = 10;

	//more or less arbitrary base value
	int iItemValue = (GC.getGame().getCurrentEra()+3)*iValueScale;

	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	CvAssert(pkResourceInfo != NULL);
	if (pkResourceInfo == NULL)
		return 0;

	if (iResourceQuantity == 0)
		return 0;

	//Integer zero check...
	if (iNumTurns <= 0)
		iNumTurns = 1;

	CvDeal* pRenewDeal = m_pPlayer->GetDiplomacyAI()->GetDealToRenew(eOtherPlayer);

	int iFlavorResult = 0;
	int iFlavors = 0;

	//Let's look at flavors for resources
	for (int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		int iResourceFlavor = pkResourceInfo->getFlavorValue((FlavorTypes)i);
		if (iResourceFlavor > 0)
		{
			int iPersonalityFlavorValue = GetPlayer()->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)i);
			//Has to be above average to affect price. Will usually result in a x2-x3 modifier
			iFlavorResult += ((iResourceFlavor + iPersonalityFlavorValue) / 5);
			iFlavors++;
		}
	}
	//Get the average multiplier from the number of Flavors being considered.
	if ((iFlavorResult > 0) && (iFlavors > 0))
		iItemValue += (iFlavorResult / iFlavors)*(iValueScale/2);

	if (bFromMe)
	{
		//Every x gold in net GPT will increase resource value by 1, up to the value of the item itself (so never more than double).
		int iGPT = int(0.5+sqrt(iCurrentNetGoldOfReceivingPlayer/4.));
		if (iGPT > 0)
			iItemValue += min(iGPT,iItemValue);
	}
	else
	{
		//Every x gold in net GPT will increase resource value by 1, up to the value of the item itself (so never more than double).
		int iGPT = int(0.5+sqrt(iCurrentNetGoldOfReceivingPlayer/5.));
		if (iGPT > 0)
			iItemValue += min(iGPT,iItemValue);
	}
	
	if (bFromMe)
	{
		if (!GET_TEAM(GetPlayer()->getTeam()).IsResourceObsolete(eResource))
		{
			//Never trade away everything.
			int iNumRemaining = (GetPlayer()->getNumResourceAvailable(eResource, true) - ((pRenewDeal != NULL) ? 0 : iResourceQuantity));
			if (iNumRemaining < 0)
				return INT_MAX;
			else if (iNumRemaining == 0)
				iItemValue *= 2;


			//If they're stronger than us, strategic resources are valuable.
			if (GetPlayer()->GetMilitaryMight() < GET_PLAYER(eOtherPlayer).GetMilitaryMight())
			{
				iItemValue *= 10;
				iItemValue /= 7;
			}

			//Good target? Don't sell to them!
			if (GetPlayer()->GetDiplomacyAI()->IsWantsSneakAttack(eOtherPlayer))
			{
				iItemValue *= 2;
			}
			//Are they close, or far away? We should always be a bit less eager to sell war resources from neighbors.
			if (GetPlayer()->GetProximityToPlayer(eOtherPlayer) >= PLAYER_PROXIMITY_CLOSE)
			{
				iItemValue *= 10;
				iItemValue /= 9;
			}
			//Are we going for science win? Don't sell aluminum!
			ProjectTypes eApolloProgram = (ProjectTypes)GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
			if (eApolloProgram != NO_PROJECT)
			{
				if (GetPlayer()->GetDiplomacyAI()->IsGoingForSpaceshipVictory() || GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).getProjectCount(eApolloProgram) > 0)
				{
					ResourceTypes eAluminumResource = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
					if (eResource == eAluminumResource)
					{
						return INT_MAX;
					}
				}
			}

			//Increase value based on number remaining (up to 10).
			iItemValue += ((10 - min(10, iNumRemaining)) * (10 - min(10, iNumRemaining)));

			//How much do we have compared to them?
			int iResourceRatio = GetResourceRatio(GetPlayer()->GetID(), eOtherPlayer, eResource, iResourceQuantity);

			//More we have compared to them, the less what we have is worth,and vice-versa!
			iItemValue *= max(50, iResourceRatio);
			iItemValue /= 100;

			// Approach is important
			switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
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
				iItemValue *= 125;
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

			return iItemValue/iValueScale;
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
				iItemValue *= 8;
				iItemValue /= 10;
			}

			//Good target? Don't buy from them!
			if (GetPlayer()->GetDiplomacyAI()->IsWantsSneakAttack(eOtherPlayer))
			{
				iItemValue /= 2;
			}
			//Are they close, or far away? We should always be a bit less eager to buy war resources from neighbors.
			if (GetPlayer()->GetProximityToPlayer(eOtherPlayer) >= PLAYER_PROXIMITY_CLOSE)
			{
				iItemValue *= 7;
				iItemValue /= 10;
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
						iItemValue *= 10;
						iItemValue /= 8;
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
						iItemValue *= 10;
						iItemValue /= 8;
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
				//How much do they have compared to us?
				int iResourceRatio = GetResourceRatio(eOtherPlayer, GetPlayer()->GetID(), eResource, iResourceQuantity);
				//More we have compared to them, the less what they're offering is worth!
				iItemValue *= 100;
				iItemValue /= max(80, iResourceRatio); //pay a premium if we have less but max 20%
			}

			// Approach is important
			switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
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

			//greatly increase this if we're at a deficit of resources.
			iItemValue *= max(1, GetPlayer()->getResourceOverValue(eResource));

			//And now speed/quantity.
			iItemValue *= (iResourceQuantity*iNumTurns);

			//how many have we already bought? Diminishing returns, bby.
			int iNumberWeGet = 0;
			int iDeals = GC.getGame().GetGameDeals().GetNumCurrentDealsWithPlayer(GetPlayer()->GetID(), eOtherPlayer);
			for (int i = 0; i < iDeals; i++)
			{
				CvDeal* pCurrentDeal = GC.getGame().GetGameDeals().GetCurrentDealWithPlayer(GetPlayer()->GetID(), eOtherPlayer, i);
				if (!pCurrentDeal)
					continue;

				iNumberWeGet += pCurrentDeal->GetNumResourceInDeal(eOtherPlayer, eResource);
			}

			int iPrevDealScaler = 100 - min(90, iNumberWeGet*10);
			iItemValue *= iPrevDealScaler;
			iItemValue /= 100;

			return iItemValue/iValueScale;
		}
		else
		{
			return 0;
		}
	}
}

/// How much is a City worth - that is: how much would the buyer pay?
int CvDealAI::GetCityValueForBuyer(CvCity* pCity, PlayerTypes eSeller, PlayerTypes eBuyer, bool bPeaceTreatyTrade /* = false */)
{
	if (!pCity)
		return INT_MAX;

	if (pCity->isCapital())
		return INT_MAX;

	//note that it can also happen that a player pretends to buy a city they already own, just to see the appropriate price
	CvPlayer& seller = GET_PLAYER(eSeller);
	CvPlayer& buyer = GET_PLAYER(eBuyer);

	//don't sell if less than 4 cities!
	if (seller.getNumCities() <= 4)
		return INT_MAX;

	//can't sell during unrest
	if (pCity->IsResistance())
		return INT_MAX;

	//traded this city before? Don't trade again.
	if (pCity->IsTraded(eBuyer) && !bPeaceTreatyTrade)
		return INT_MAX;

	//don't sell to warmongers
	if (!bPeaceTreatyTrade)
	{
		switch (seller.GetDiplomacyAI()->GetWarmongerThreat(eBuyer))
		{
		case THREAT_MAJOR:
		case THREAT_SEVERE:
		case THREAT_CRITICAL:
			return INT_MAX;
			break;
		}
	}

	//never give away holy cities
	ReligionTypes eSellerReligion = seller.GetReligions()->GetCurrentReligion(false);

	if (eSellerReligion != NO_RELIGION && pCity->GetCityReligions()->IsHolyCityForReligion(eSellerReligion))
		return INT_MAX;

	if (!bPeaceTreatyTrade && pCity->GetCityReligions()->IsHolyCityAnyReligion())
		return INT_MAX;

	//initial value
	int iItemValue = 25000;

	//economic value is important
	int iEconomicValue = pCity->getEconomicValue(eBuyer);
	int iEconomicValuePerPop = (iEconomicValue / (max(1, pCity->getPopulation())));
	iItemValue += (max(1,iEconomicValue-1000)/3); //tricky to define the correct factor

	//prevent cheesy exploit: founding cities just to sell them
	if ((GC.getGame().getGameTurn() - pCity->getGameTurnFounded()) < (42 + GC.getGame().getSmallFakeRandNum(5, iEconomicValue)))
		return INT_MAX;

	//obviously the seller doesn't really want it
	if (pCity->IsRazing())
		iItemValue /= 2;

	//If not as good as any of our cities, we don't want it.
	int iBetterThanCount = 0;
	int iCityLoop;
	for(CvCity* pLoopCity = buyer.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = buyer.nextCity(&iCityLoop))
	{
		int iScore = (pLoopCity->getEconomicValue(eBuyer) / (max(1, pLoopCity->getPopulation())));
		if (iScore < iEconomicValuePerPop)
			iBetterThanCount++;
	}
	//better than half of the buyer's cities?
	if (iBetterThanCount > buyer.getNumCities() / 2)
		iItemValue *= 2;

	//first some amount for the territory (outside of the first ring)
	int iInternalBorderCount = 0;
	int iCityTiles = 0;
	for(int iI = RING1_PLOTS; iI < MAX_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);
		if (!pLoopPlot)
			continue;

		//if it belongs to the city
		if (pCity->GetID() == pLoopPlot->GetCityPurchaseID())
			iCityTiles++;
		else if (pLoopPlot->getOwner() == eBuyer)
			//belongs to another one of the buyer's cities
			iInternalBorderCount++;
	}

	//this is how much ANY plot is worth to the buyer right now
	int goldPerPlot = buyer.GetBuyPlotCost();
	iItemValue += goldPerPlot * iCityTiles;

	//important. it's valuable to have as much internal border as possible
	iItemValue += goldPerPlot * 8 * iInternalBorderCount;

	//re-use the gold value as a general unit and penalize unhappy citizens
	iItemValue -= pCity->getUnhappyCitizenCount() * goldPerPlot * 3;

	//the less the buyer likes the seller, the less he offers
	switch (buyer.GetDiplomacyAI()->GetMajorCivApproach(eSeller, /*bHideTrueFeelings*/ false))
	{
	case MAJOR_CIV_APPROACH_FRIENDLY:
	case MAJOR_CIV_APPROACH_AFRAID:
	case MAJOR_CIV_APPROACH_NEUTRAL:
		iItemValue *= 100;
		break;
	case MAJOR_CIV_APPROACH_DECEPTIVE:
	case MAJOR_CIV_APPROACH_GUARDED:
		iItemValue *= 75;
		break;
	case MAJOR_CIV_APPROACH_HOSTILE:
		iItemValue *= 50;
		break;
	case MAJOR_CIV_APPROACH_WAR:
	default:
		iItemValue *= 25;
		break;
	}
	iItemValue /= 100;

	//buyer likes it close to home. todo: what about the seller? careful: simple distance lookup will always be zero
	CvCity* pNeighborCity = buyer.GetClosestCityToCity(pCity);
	int iBuyerDist = pNeighborCity ? plotDistance(*pNeighborCity->plot(),*pCity->plot()) : 0;
	int iRefDist = GC.getAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT();
	if (iBuyerDist > iRefDist)
		iItemValue = (iItemValue*iRefDist) / iBuyerDist;

	// premium if buyer founded it
	if (pCity->getOriginalOwner() == eBuyer)
	{
		iItemValue *= 120;
		iItemValue /= 100;
	}

	// premium if buyer currently owns it
	if ( pCity->getOwner() == eBuyer )
	{
		iItemValue *= 120;
		iItemValue /= 100;
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(seller.getTeam()).IsVassal(buyer.getTeam()))
	{
		iItemValue *= 70;
		iItemValue /= 100;
	}
#endif

	if (pCity->IsPuppet())
	{
		iItemValue *= 70;
		iItemValue /= 100;
	}

	// don't want to lose wonders
	int iWonders = pCity->getNumWorldWonders();
	int iPerWonderBonus = 10;
	if (pCity->getOwner() == eBuyer)
	{
		iPerWonderBonus = 20;
		iWonders += pCity->getNumNationalWonders();
	}

	if (iWonders > 0)
	{
		// 20% per wonder
		iItemValue *= (100 + iWonders*iPerWonderBonus);
		iItemValue /= 100;
	}

	// Original capitals are very valuable
	if (pCity->IsOriginalMajorCapital())
	{
		//don't sell capital if we're collecting them
		if ((!bPeaceTreatyTrade && seller.GetDiplomacyAI()->IsGoingForWorldConquest()) || seller.GetDiplomacyAI()->IsCloseToDominationVictory())
			return MAX_INT;
		else
			iItemValue *= 3;
	}

	// Good price for you my friend
	if (!seller.GetDiplomacyAI()->IsDoFAccepted(eBuyer))
	{
		iItemValue *= 150;
		iItemValue /= 100;
	}

	// so here's the tricky part - convert to gold
	iItemValue /= 3;

	//OutputDebugString(CvString::format("City value for %s from %s to %s is %d\n", pCity->getName().c_str(), sellingPlayer.getName(), buyingPlayer.getName(), iItemValue).c_str());
	return iItemValue;
}

// How much is an embassy worth?
int CvDealAI::GetEmbassyValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Embassy with oneself.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 50;

	if (GetPlayer()->IsAITeammateOfHuman())
		return INT_MAX;

	// Scale up or down by deal duration at this game speed
	CvGameSpeedInfo *pkStdSpeedInfo = GC.getGameSpeedInfo((GameSpeedTypes)GC.getSTANDARD_GAMESPEED());
	if (pkStdSpeedInfo)
	{
		iItemValue *= GC.getGame().getGameSpeedInfo().GetDealDuration();
		iItemValue /= pkStdSpeedInfo->GetDealDuration();
	}
	
	// Denouncement in either direction?
	if (GetPlayer()->GetDiplomacyAI()->IsDenouncedPlayer(eOtherPlayer) || GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsDenouncedPlayer(GetPlayer()->GetID()))
	{
		return INT_MAX;
	}

	if (bFromMe)  // giving the other player an embassy in my capital
	{
		// Approach is important
		switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
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
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Embassy valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}

	if (!bFromMe)  // they want to give us an embassy in their capital
	{
		// Approach is important
		switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ true))
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
			CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Embassy valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}

	if (iItemValue <= 50)
		iItemValue = 50;

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if (bUseEvenValue)
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

/// How much in V-POINTS (aka value) is Open Borders worth? You gotta admit that V-POINTS sound pretty cool though
int CvDealAI::GetOpenBordersValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bUseEvenValue)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of Open Borders with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();
	MajorCivApproachTypes eApproach = pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);

	int iItemValue = 10;

	if (GetPlayer()->IsAITeammateOfHuman())
		return INT_MAX;
	
	// Me giving Open Borders to the other guy
	if (bFromMe)
	{
		if (!pDiploAI->IsWillingToGiveOpenBordersToPlayer(eOtherPlayer))
		{
			return INT_MAX;
		}
		// Approach is important
		switch (eApproach)
		{
		case MAJOR_CIV_APPROACH_HOSTILE:
		case MAJOR_CIV_APPROACH_GUARDED:
		case MAJOR_CIV_APPROACH_WAR:
			iItemValue += 600;
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
		int iNumEnemiesAtWarWith = pDiploAI->GetNumOurEnemiesPlayerAtWarWith(eOtherPlayer);

		if (iNumEnemiesAtWarWith > 0)
		{
			iItemValue *= max(5, (100 - (iNumEnemiesAtWarWith * 5)));
			iItemValue /= 100;
		}

#if defined(MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS)
		if (GetPlayer()->GetCulture()->GetTourism() / 100 > 0)
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
#else
		// Do we think he's going for culture victory? If we're contesting this, don't take his open borders!
		if (pDiploAI->IsCompetingForVictory() && pDiploAI->GetMajorCivOpinion(eOtherPlayer) != MAJOR_CIV_OPINION_ALLY && pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) != MAJOR_CIV_APPROACH_FRIENDLY)
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eOtherPlayer);
			if (kOtherPlayer.GetCulture()->GetTourism() / 100 > 0 && (kOtherPlayer.GetCulture()->GetInfluenceOn(GetPlayer()->GetID()) < INFLUENCE_LEVEL_INFLUENTIAL))
			{
				// Fiercely competitive over victory
				if (pDiploAI->GetVictoryBlockLevel(eOtherPlayer) >= BLOCK_LEVEL_STRONG || pDiploAI->GetVictoryDisputeLevel(eOtherPlayer) >= DISPUTE_LEVEL_STRONG)
				{
					return INT_MAX;
				}

				// If he has influence over half the civs, want to block OB with the other half
				if (kOtherPlayer.GetCulture()->GetNumCivsToBeInfluentialOn() < kOtherPlayer.GetCulture()->GetNumCivsInfluentialOn())
				{
					return INT_MAX;
				}
			}
		}
#endif

		// Add 200 Gold for each of our artifacts they've stolen
		if (pDiploAI->GetNumArtifactsEverDugUp(eOtherPlayer) > 0)
		{
			iItemValue += (pDiploAI->GetNumArtifactsEverDugUp(eOtherPlayer) * 200);
		}
#if defined(MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON)
		// Does open borders make their production siphon trait stronger?
		if (MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON && GET_PLAYER(eOtherPlayer).GetPlayerTraits()->IsTradeRouteProductionSiphon())
		{
			iItemValue += GET_PLAYER(eOtherPlayer).GetPlayerTraits()->GetTradeRouteProductionSiphon(true).m_iPercentIncreaseWithOpenBorders * 20;
			iItemValue += GET_PLAYER(eOtherPlayer).GetPlayerTraits()->GetTradeRouteProductionSiphon(false).m_iPercentIncreaseWithOpenBorders * 20;
		}
#endif
	}
	// Other guy giving me Open Borders
	else
	{
		if (!pDiploAI->IsWantsOpenBordersWithPlayer(eOtherPlayer))
		{
			return INT_MAX;
		}

		// Approach is important
		switch (eApproach)
		{
		case MAJOR_CIV_APPROACH_WAR:
		case MAJOR_CIV_APPROACH_HOSTILE:
		case MAJOR_CIV_APPROACH_DECEPTIVE:
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue += 0;
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
		switch (GetPlayer()->GetProximityToPlayer(eOtherPlayer))
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
		// Reduce value by half if the other guy only has a single City
		if (GET_PLAYER(eOtherPlayer).getNumCities() == 1)
		{
			iItemValue *= 50;
			iItemValue /= 100;
		}
		if (GetPlayer()->IsCramped() && GET_PLAYER(eOtherPlayer).getNumCities() > GetPlayer()->getNumCities())
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		if (pDiploAI->GetNeighborOpinion(eOtherPlayer) == MAJOR_CIV_OPINION_ENEMY)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		if (pDiploAI->MusteringForNeighborAttack(eOtherPlayer))
		{
			iItemValue *= 150;
			iItemValue /= 100;
		}
		//We need to explore?
		EconomicAIStrategyTypes eNeedRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON");
		EconomicAIStrategyTypes eNavalRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON_SEA");
		if (eNeedRecon != NO_ECONOMICAISTRATEGY && GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNeedRecon))
		{
			iItemValue *= 115;
			iItemValue /= 100;
		}
		if (eNavalRecon != NO_ECONOMICAISTRATEGY && GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNavalRecon))
		{
			iItemValue *= 115;
			iItemValue /= 100;
		}
#if defined(MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON)
		// Does open borders make our production siphon trait stronger?
		if (MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON && GetPlayer()->GetPlayerTraits()->IsTradeRouteProductionSiphon())
		{
			iItemValue += GetPlayer()->GetPlayerTraits()->GetTradeRouteProductionSiphon(true).m_iPercentIncreaseWithOpenBorders * 20;
			iItemValue += GetPlayer()->GetPlayerTraits()->GetTradeRouteProductionSiphon(false).m_iPercentIncreaseWithOpenBorders * 20;
		}
#endif

#if defined(MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS)
		// Do we think he's going for culture victory? If we're contesting this, don't take his open borders!
		if (pDiploAI->IsCompetingForVictory() && pDiploAI->GetMajorCivOpinion(eOtherPlayer) != MAJOR_CIV_OPINION_ALLY && pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) != MAJOR_CIV_APPROACH_FRIENDLY)
		{
			CvPlayer &kOtherPlayer = GET_PLAYER(eOtherPlayer);
			if (kOtherPlayer.GetCulture()->GetTourism() / 100 > 0 && (kOtherPlayer.GetCulture()->GetInfluenceOn(GetPlayer()->GetID()) < INFLUENCE_LEVEL_INFLUENTIAL))
			{
				// Fiercely competitive over victory
				if (pDiploAI->GetVictoryBlockLevel(eOtherPlayer) >= BLOCK_LEVEL_STRONG || pDiploAI->GetVictoryDisputeLevel(eOtherPlayer) >= DISPUTE_LEVEL_STRONG)
				{
					return INT_MAX;
				}

				// If he has influence over half the civs, want to block OB with the other half
				if (kOtherPlayer.GetCulture()->GetNumCivsToBeInfluentialOn() < kOtherPlayer.GetCulture()->GetNumCivsInfluentialOn())
				{
					return INT_MAX;
				}
			}
		}
#else
		if (GetPlayer()->GetCulture()->GetTourism() / 100 > 0)
		{
			// The civ we need influence on the most should ALWAYS be included
			if (GetPlayer()->GetCulture()->GetCivLowestInfluence(false) == eOtherPlayer)
			{
				iItemValue *= 200;
				iItemValue /= 100;
			}

			// If have influence over half the civs, want OB with the other half
			if (GetPlayer()->GetCulture()->GetNumCivsToBeInfluentialOn() <= GetPlayer()->GetCulture()->GetNumCivsInfluentialOn())
			{
				if (GetPlayer()->GetCulture()->GetInfluenceLevel(eOtherPlayer) < INFLUENCE_LEVEL_INFLUENTIAL)
				{
					iItemValue *= 200;
					iItemValue /= 100;
				}
			}
		}
#endif
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if (bUseEvenValue)
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

	if (!GetPlayer()->GetDiplomacyAI()->IsWantsDefensivePactWithPlayer(eOtherPlayer))
	{
		return INT_MAX;
	}

	if (bFromMe)
	{
		int iStrengthMod = (int)GetPlayer()->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer) - 3;
		iItemValue += (200 * iStrengthMod);

		if (GetPlayer()->GetDiplomacyAI()->GetMostValuableAlly() == eOtherPlayer)
		{
			iItemValue += 500;
		}
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if (bUseEvenValue)
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

	if (GetPlayer()->IsAITeammateOfHuman())
		return INT_MAX;

	if (bFromMe)
	{
		// if they are ahead of me in tech by one or more eras ratchet up the value since they are more likely to get a good tech than I am
		EraTypes eMyEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();
		EraTypes eTheirEra = GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetCurrentEra();

		int iAdditionalValue = iItemValue * max(0,(int)(eTheirEra-eMyEra));
		iItemValue += iAdditionalValue;

		// Approach is important
		switch (GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
		{
		case MAJOR_CIV_APPROACH_WAR:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 1000;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
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

	if (GetPlayer()->GetRAToVotes() > 0)
	{
		iItemValue *= 5;
	}

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if (bUseEvenValue)
	{
		int iReverseValue = GET_PLAYER(eOtherPlayer).GetDealAI()->GetResearchAgreementValue(!bFromMe, GetPlayer()->GetID(), /*bUseEvenValue*/ false);

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

	return 20;
}

/// What is the value of peace with eWithTeam? NOTE: This deal item should be disabled if eWithTeam doesn't want to go to peace
int CvDealAI::GetThirdPartyPeaceValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam, bool bLogging)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Third Party Peace with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 100; //just some base value
	
	if (GetPlayer()->IsAITeammateOfHuman())
		return INT_MAX;

	if (bLogging)
		return iItemValue;

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
			// Captured our capital or Holy City? Close to winning the game? Don't accept peace bribes.
			if (pDiploAI->IsCapitalCapturedBy(eWithPlayer) || pDiploAI->IsHolyCityCapturedBy(eWithPlayer) || pDiploAI->IsCloseToDominationVictory() || pDiploAI->IsEndgameAggressiveTo(eWithPlayer))
			{
				return INT_MAX;
			}

			MajorCivApproachTypes eWarMajorCivApproach = pDiploAI->GetMajorCivApproach(eWithPlayer, /*bHideTrueFeelings*/ false);
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
			
			// If we want them conquered, let's be much more reluctant to accept peace bribes.
			bool bWantsConquest = (pDiploAI->IsGoingForWorldConquest() || pDiploAI->GetMajorCivOpinion(eWithPlayer) == MAJOR_CIV_OPINION_UNFORGIVABLE);
			if (bWantsConquest && eWarMajorCivApproach == MAJOR_CIV_APPROACH_WAR)
			{
				iItemValue *= 3;
			}

			// Major competitor?
			if (pDiploAI->GetBiggestCompetitor() == eWithPlayer && eWarMajorCivApproach <= MAJOR_CIV_APPROACH_HOSTILE)
			{
				iItemValue *= 2;
			}
			else if (pDiploAI->IsMajorCompetitor(eWithPlayer) && eWarMajorCivApproach == MAJOR_CIV_APPROACH_WAR)
			{
				iItemValue *= 150;
				iItemValue /= 100;
			}
		}

		MajorCivApproachTypes eAskingMajorCivApproach = pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);

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
		else if(eAskingMajorCivApproach <= MAJOR_CIV_APPROACH_HOSTILE)
		{
			iItemValue *= 600;
			iItemValue /= 100;
		}
		
		// Not an easy target? Halve the value.
		if (!pDiploAI->IsEasyTarget(eWithPlayer))
		{
			iItemValue /= 2;
		}
	}
	// From them
	else
	{

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

		if (!GET_PLAYER(eWithPlayer).isMinorCiv())
		{
			MajorCivApproachTypes eWarMajorCivApproach = pDiploAI->GetMajorCivApproach(eWithPlayer, /*bHideTrueFeelings*/ false);

			// Modify for our feelings towards the player they're at war with
			if (eWarMajorCivApproach == MAJOR_CIV_APPROACH_FRIENDLY)
			{
				iItemValue *= 200;
				iItemValue /= 100;
			}
			else if (eWarMajorCivApproach == MAJOR_CIV_APPROACH_NEUTRAL)
			{
				iItemValue *= 50;
				iItemValue /= 100;
			}
			else if (eWarMajorCivApproach == MAJOR_CIV_APPROACH_AFRAID)
			{
				iItemValue *= 75;
				iItemValue /= 100;
			}
			else
			{
				return INT_MAX;
			}
		}
		else if (GET_PLAYER(eWithPlayer).isMinorCiv())
		{
			//Way less value.
			iItemValue /= 50;

			MinorCivApproachTypes eMinorApproachTowardsWarPlayer = pDiploAI->GetMinorCivApproach(eWithPlayer);
			if (eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_FRIENDLY)
			{
				iItemValue *= 150;
				iItemValue /= 100;
			}
			else if (eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_PROTECTIVE)
			{
				iItemValue *= 300;
				iItemValue /= 100;
			}
			else
			{
				return INT_MAX;
			}
		}
		
		MajorCivApproachTypes eAskingMajorCivApproach = pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);

		// Modify for our feelings towards the asking player
		if (eAskingMajorCivApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		{
			iItemValue *= 200;
			iItemValue /= 100;
		}
		else if (eAskingMajorCivApproach == MAJOR_CIV_APPROACH_NEUTRAL)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		else if (eAskingMajorCivApproach == MAJOR_CIV_APPROACH_AFRAID)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		else if (eAskingMajorCivApproach == MAJOR_CIV_APPROACH_GUARDED)
		{
			iItemValue *= 125;
			iItemValue /= 100;
		}
		else if (eAskingMajorCivApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		{
			iItemValue *= 150;
			iItemValue /= 100;
		}
		else if (eAskingMajorCivApproach <= MAJOR_CIV_APPROACH_HOSTILE)
		{
			return INT_MAX;
		}
		
		// Not an easy target? Halve the value.
		if (!GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsEasyTarget(eWithPlayer))
		{
			iItemValue /= 2;
		}
	}


	return iItemValue;
}

/// What is the value of war with eWithPlayer?
int CvDealAI::GetThirdPartyWarValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam, bool bLogging)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Third Party War with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 300; //just some base value
	
	if (GetPlayer()->IsAITeammateOfHuman())
		return INT_MAX;

	if (bLogging)
		return iItemValue;

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();

	bool bWithHuman = GET_PLAYER(eOtherPlayer).isHuman();

	// How much does this AI like to go to war?
	int iWarApproachWeight = pDiploAI->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_WAR);
	if (bFromMe)
		iItemValue *= max(1, (5 - iWarApproachWeight/2));
	else
		iItemValue *= max(1, iWarApproachWeight/2);

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
	if(pDiploAI->IsDoFAccepted(eWithPlayer) || GET_PLAYER(eWithPlayer).GetDiplomacyAI()->IsDoFAccepted(GetPlayer()->GetID()))
	{
		return INT_MAX;
	}
	//Are the asked in a DOF with the player? Don't do it!
	if(GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsDoFAccepted(eWithPlayer) || GET_PLAYER(eWithPlayer).GetDiplomacyAI()->IsDoFAccepted(eOtherPlayer))
	{
		return INT_MAX;
	}
	//Can't see any of their cities?
	if (bFromMe)
	{
		int iCityLoop;
		bool bSeen = false;
		for (CvCity* pLoopCity = GET_PLAYER(eWithPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eWithPlayer).nextCity(&iCityLoop))
		{
			if (pLoopCity != NULL && pLoopCity->isRevealed(m_pPlayer->getTeam(), false))
			{
				bSeen = true;
				break;
			}
		}
		if (!bSeen)
		{
			return INT_MAX;
		}
	}
	//Can't see any of their cities?
	if (!bFromMe)
	{
		int iCityLoop;
		bool bSeen = false;
		for (CvCity* pLoopCity = GET_PLAYER(eWithPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eWithPlayer).nextCity(&iCityLoop))
		{
			if (pLoopCity != NULL && pLoopCity->isRevealed(GET_PLAYER(eOtherPlayer).getTeam(), false))
			{
				bSeen = true;
				break;
			}
		}
		if (!bSeen)
		{
			return INT_MAX;
		}
	}
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	//NO FUCKING VASSALS GODDAMMIT STOP IT
	if(GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsVassalOfSomeone())
	{
		return INT_MAX;
	}
	if (GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsVassalOfSomeone())
	{
		return INT_MAX;
	}
	if (GET_TEAM(GetPlayer()->getTeam()).IsVassalOfSomeone())
	{
		return INT_MAX;
	}
#endif

	// Don't offer or accept a war bribe against someone we agreed to go on a coop war with.
	if (pDiploAI->GetGlobalCoopWarWithState(eWithPlayer) >= COOP_WAR_STATE_PREPARING)
	{
		return INT_MAX;
	}
	// Ditto if they resurrected us or liberated our capital
	if (pDiploAI->WasResurrectedBy(eWithPlayer) || pDiploAI->IsPlayerLiberatedCapital(eWithPlayer))
	{
		return INT_MAX;
	}

	//Already moving on asker?
	if(bFromMe && pDiploAI->IsArmyInPlaceForAttack(eOtherPlayer))
	{
		return INT_MAX;
	}
	if(!bFromMe && pDiploAI->IsArmyInPlaceForAttack(eOtherPlayer))
	{
		iItemValue += 500;
	}
	if(bFromMe && GetPlayer()->HasAnyOffensiveOperationsAgainstPlayer(eOtherPlayer))
	{
		return INT_MAX;
	}
	if(!bFromMe && GetPlayer()->HasAnyOffensiveOperationsAgainstPlayer(eOtherPlayer))
	{
		iItemValue += 500;
	}

	//Already moving on target?
	if(bFromMe && pDiploAI->IsArmyInPlaceForAttack(eWithPlayer))
	{
		iItemValue += 250;
	}
	if(!bFromMe && pDiploAI->IsArmyInPlaceForAttack(eWithPlayer))
	{
		iItemValue += 500;
	}
	if(bFromMe && GetPlayer()->HasAnyOffensiveOperationsAgainstPlayer(eWithPlayer))
	{
		iItemValue += 250;
	}
	if(!bFromMe && GetPlayer()->HasAnyOffensiveOperationsAgainstPlayer(eWithPlayer))
	{
		iItemValue += 500;
	}

	WarProjectionTypes eWarProjection = pDiploAI->GetWarProjection(eWithPlayer);

	EraTypes eOurEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();

	MajorCivApproachTypes eApproachTowardsAskingPlayer = pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
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
	else
	{
		eMajorApproachTowardsWarPlayer = pDiploAI->GetMajorCivApproach(eWithPlayer, /*bHideTrueFeelings*/ false);
	}
	// From me
	if (bFromMe)
	{
		//Are we already at war with the team? Don't do it!
		if (GET_TEAM(GetPlayer()->getTeam()).isAtWar(eWithTeam))
		{
			return INT_MAX;
		}
		//Can we declare war?
		if (!GET_TEAM(GetPlayer()->getTeam()).canDeclareWar(GET_PLAYER(eWithPlayer).getTeam()))
		{
			return INT_MAX;
		}
		// Don't accept if we don't like the asker, unless the target's our biggest competitor
		if (eApproachTowardsAskingPlayer < MAJOR_CIV_APPROACH_AFRAID && pDiploAI->GetBiggestCompetitor() != eWithPlayer)
		{
			return INT_MAX;
		}
		// Player must be a potential war target
		if (!pDiploAI->IsPotentialWarTarget(eWithPlayer))
		{
			return INT_MAX;
		}
		// Would this war cause us or our teammates to backstab a friend/ally? Don't do it!
		if (!pDiploAI->IsWarSane(eWithPlayer))
		{
			return INT_MAX;
		}
		// If we're in bad shape to start a war, no wars
		if (GetPlayer()->IsNoNewWars())
		{
			return INT_MAX;
		}
		//don't bite if we can't easily attack.
		if (pDiploAI->GetPlayerTargetValue(eWithPlayer) <= TARGET_VALUE_AVERAGE)
		{
			return INT_MAX;
		}

		//anti-exploit rule - AI to AI bribes should only be for competitors.
		if (!bWithHuman)
		{
			if (!GetPlayer()->GetDiplomacyAI()->IsMajorCompetitor(eWithPlayer) && !GetPlayer()->GetDiplomacyAI()->IsEarlyGameCompetitor(eWithPlayer))
				return INT_MAX;
		}
		
		// Don't accept war bribes if we recently made peace.
		if (pDiploAI->GetNumWarsFought(eWithPlayer) > 0)
		{
			int iPeaceTreatyTurn = GET_TEAM(GetTeam()).GetTurnMadePeaceTreatyWithTeam(GET_PLAYER(eWithPlayer).getTeam());
			if (iPeaceTreatyTurn > -1)
			{
				int iTurnsSincePeace = GC.getGame().getGameTurn() - iPeaceTreatyTurn;
				int iPeaceDampenerTurns = 30;

				if (iTurnsSincePeace < iPeaceDampenerTurns)
				{
					return INT_MAX;
				}
			}
		}

		// Sanity check - avoid going bankrupt
		int iMinIncome = 2 + (GetPlayer()->GetCurrentEra() * 2);
		if (pDiploAI->IsWarWouldBankruptUs(eWithPlayer, false, iMinIncome))
		{
			return INT_MAX;
		}

		// Sanity check - who else would we go to war with?
		vector<PlayerTypes> vLinkedWarPlayers = pDiploAI->GetLinkedWarPlayers(eWithPlayer, false, true, false);

		for (std::vector<PlayerTypes>::iterator it = vLinkedWarPlayers.begin(); it != vLinkedWarPlayers.end(); it++)
		{
			// Would we be declaring war on a powerful neighbor?
			if (GetPlayer()->GetProximityToPlayer(*it) >= PLAYER_PROXIMITY_CLOSE)
			{
				if (GET_PLAYER(*it).isMajorCiv())
				{
					if (pDiploAI->GetMajorCivApproach(*it, false) == MAJOR_CIV_APPROACH_AFRAID)
					{
						return INT_MAX;
					}
					// If we're already planning a war/demand against them, then we don't care.
					else if (pDiploAI->GetMajorCivApproach(*it, false) != MAJOR_CIV_APPROACH_WAR && pDiploAI->GetWarGoal(*it) != WAR_GOAL_DEMAND)
					{
						// Bold AIs will take more risks.
						if (pDiploAI->GetBoldness() <= 5 || pDiploAI->GetPlayerMilitaryStrengthComparedToUs(*it) > STRENGTH_STRONG)
						{
							return INT_MAX;
						}
						else if (pDiploAI->GetPlayerMilitaryStrengthComparedToUs(*it) > STRENGTH_AVERAGE)
						{
							return INT_MAX;
						}
					}
				}
				else
				{
					if (pDiploAI->GetMinorCivApproach(*it) != MINOR_CIV_APPROACH_CONQUEST && pDiploAI->GetMinorCivApproach(*it) != MINOR_CIV_APPROACH_BULLY)
					{
						if (pDiploAI->GetPlayerMilitaryStrengthComparedToUs(*it) > STRENGTH_AVERAGE)
						{
							return INT_MAX;
						}
					}
				}
			}
		}

		if (eApproachTowardsAskingPlayer != MAJOR_CIV_APPROACH_FRIENDLY)
		{
			if(eWarProjection == WAR_PROJECTION_VERY_GOOD)
				iItemValue += 100;
			else if(eWarProjection == WAR_PROJECTION_GOOD)
				iItemValue += 500;
			else
				return INT_MAX;
		}
		else
		{
			if(eWarProjection == WAR_PROJECTION_VERY_GOOD)
				iItemValue += 100;
			else if(eWarProjection == WAR_PROJECTION_GOOD)
				iItemValue += 200;
			else if(eWarProjection >= WAR_PROJECTION_STALEMATE)
				iItemValue += 500;
			else
				return INT_MAX;
		}

		if (pDiploAI->GetBiggestCompetitor() == eWithPlayer)
		{
			iItemValue /= 2;
		}
		else if (pDiploAI->IsMajorCompetitor(eWithPlayer))
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
		
		// Add 50 gold per era
		int iExtraCost = eOurEra * 50;
		iItemValue += iExtraCost;

		// Minor
		if(bMinor)
		{
			if (eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_FRIENDLY || 
				eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_PROTECTIVE)
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
				iItemValue *= 125;
				iItemValue /= 100;
			}
			else
			{
				return INT_MAX;
			}
		}

		//Already planning war? Less valuable.
		if(pDiploAI->GetWarGoal(eWithPlayer) == WAR_GOAL_CONQUEST)
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}

		if(pDiploAI->IsArmyInPlaceForAttack(eWithPlayer))
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
		if(GetPlayer()->HasAnyOffensiveOperationsAgainstPlayer(eWithPlayer))
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

		if(!bMinor)
		{
			if (GetPlayer()->IsAtWar() && eWithPlayer != NO_PLAYER)
			{
				// find any other wars we have going
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					PlayerTypes eWarPlayer = (PlayerTypes)iPlayerLoop;
					if (eWarPlayer != NO_PLAYER && eWarPlayer != eOtherPlayer && eWarPlayer != eWithPlayer && eWarPlayer != GetPlayer()->GetID() && !GET_PLAYER(eWarPlayer).isMinorCiv())
					{
						if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eWarPlayer).getTeam()))
						{
							WarStateTypes eWarState = pDiploAI->GetWarState(eWarPlayer);
							if (eWarState != NO_WAR_STATE_TYPE && eWarState <= WAR_STATE_STALEMATE)
							{
								return INT_MAX;
							}
							else if (eWarState == WAR_STATE_CALM && pDiploAI->GetWarProjection(eWarPlayer) <= WAR_PROJECTION_STALEMATE)
							{
								return INT_MAX;
							}
							else
							{
								iItemValue += 150;
							}
						}
					}
				}
			}

			// Only accept a bribe if we have a good attack target - reduces spam
			if (!GetPlayer()->GetMilitaryAI()->HavePreferredAttackTarget(eWithPlayer))
			{
				return INT_MAX;
			}

			switch(GetPlayer()->GetProximityToPlayer(eWithPlayer))
			{
				//let's only do this with neighbors
				case PLAYER_PROXIMITY_DISTANT:
				case PLAYER_PROXIMITY_FAR:
					return INT_MAX;
					break;
				case PLAYER_PROXIMITY_CLOSE:
					iItemValue += 1000;
					break;
				case PLAYER_PROXIMITY_NEIGHBORS:
					iItemValue += 500;
					break;
				default:
					CvAssertMsg(false, "DEAL_AI: Player has no valid proximity for 3rd party deal.");
					iItemValue += 250;
					break;
			}
			
			// Target is close to winning the game? Halve the value.
			if (GET_PLAYER(eWithPlayer).isMajorCiv() && pDiploAI->IsEndgameAggressiveTo(eWithPlayer))
			{
				iItemValue /= 2;
			}
			
			// Easy target? Halve the value.
			if (pDiploAI->IsEasyTarget(eWithPlayer))
			{
				iItemValue /= 2;
			}
		}
	}

	// From them
	else
	{
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
				//biggest competitor? let's dogpile.
				if (pDiploAI->GetBiggestCompetitor() == eWithPlayer)
					iItemValue *= 5;
				else
				{
					if (bWithHuman)
					{
						//if our heart isn't in it, we don't want to dogpile.
						if (eMajorApproachTowardsWarPlayer >= MAJOR_CIV_APPROACH_FRIENDLY)
							iItemValue *= 10;
						//if we don't trust the other player, don't ask.
						if (eApproachTowardsAskingPlayer <= MAJOR_CIV_APPROACH_DECEPTIVE)
							iItemValue *= 10;
					}
					else
					{
						//if our heart isn't in it, we don't want to dogpile.
						if (eMajorApproachTowardsWarPlayer >= MAJOR_CIV_APPROACH_FRIENDLY)
							return INT_MAX;
						//if we don't trust the other player, don't ask.
						if (eApproachTowardsAskingPlayer <= MAJOR_CIV_APPROACH_DECEPTIVE)
							return INT_MAX;
					}
				}
			}
		}

		//don't ask if they can't easily attack.
		if (bWithHuman)
		{
			if (GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetPlayerTargetValue(eWithPlayer) <= TARGET_VALUE_BAD)
				iItemValue *= 2;
		}
		else
		{
			if (GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetPlayerTargetValue(eWithPlayer) <= TARGET_VALUE_BAD)
				return INT_MAX;
		}

		WarProjectionTypes eWarProjection2 = GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetWarProjection(eWithPlayer);

		if(eWarProjection2 >= WAR_PROJECTION_VERY_GOOD)
			iItemValue += 50;
		else if(eWarProjection2 >= WAR_PROJECTION_GOOD)
			iItemValue += 100;
		else if(eWarProjection2 == WAR_PROJECTION_UNKNOWN)
			iItemValue += 200;
		else if(eWarProjection2 <= WAR_PROJECTION_STALEMATE)
			iItemValue += 1000;

		// Add 50 gold per era
		int iExtraCost = eOurEra * 50;
		iItemValue += iExtraCost;

		// Minor
		if (bMinor)
		{
			//Only pay if we want to see it conquered - otherwise we don't care at all (and also, 1/2 the value, ok?)
			if (eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_CONQUEST || eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_BULLY)
			{
				iItemValue *= 50;
				iItemValue /= 100;
			}
			else
			{
				if (bWithHuman)
				{
					if (eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_PROTECTIVE || eMinorApproachTowardsWarPlayer == MINOR_CIV_APPROACH_FRIENDLY)
					{
						return INT_MAX;
					}
					else
					{
						iItemValue *= 5;
						iItemValue /= 100;
					}
				}
				else
				{
					return INT_MAX;
				}
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
				else if (eMajorApproachTowardsWarPlayer >= MAJOR_CIV_APPROACH_AFRAID)
				{
					if (bWithHuman)
					{
						iItemValue *= 50;
						iItemValue /= 100;
					}
					else
					{
						return INT_MAX;
					}
				}
			}
			else
			{
				iItemValue *= 250;
				iItemValue /= 100;
			}

			if (eWithPlayer == m_pPlayer->GetDiplomacyAI()->GetBiggestCompetitor())
			{
				iItemValue *= 2;
			}
			else if (m_pPlayer->GetDiplomacyAI()->IsMajorCompetitor(eWithPlayer))
			{
				iItemValue *= 175;
				iItemValue /= 100;
			}
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
			if (GET_TEAM(GetPlayer()->getTeam()).isAtWar(eWithTeam))
			{
				if (pDiploAI->GetWarState(eWithPlayer) < WAR_STATE_STALEMATE)
				{
					iItemValue *= 2;
				}
				else
				{
					iItemValue *= 175;
					iItemValue /= 100;
				}
			}
			//Not a human? Let's see if he has a valid target...if not, don't accept!
			if (!GET_PLAYER(eOtherPlayer).isHuman())
			{
				// No good target? Abort!
				if (!GET_PLAYER(eOtherPlayer).GetMilitaryAI()->HavePreferredAttackTarget(eWithPlayer))
				{
					return INT_MAX;
				}

				switch (GET_PLAYER(eOtherPlayer).GetProximityToPlayer(eWithPlayer))
				{
					case PLAYER_PROXIMITY_DISTANT:
					case PLAYER_PROXIMITY_FAR:
						return INT_MAX;
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
			//Humans?
			else
			{
				switch (GET_PLAYER(eOtherPlayer).GetProximityToPlayer(eWithPlayer))
				{
					case PLAYER_PROXIMITY_DISTANT:
					case PLAYER_PROXIMITY_FAR:
						return INT_MAX;
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

		// Target is close to winning the game? Double the value.
		if (GET_PLAYER(eWithPlayer).isMajorCiv() && GetPlayer()->GetDiplomacyAI()->IsEndgameAggressiveTo(eWithPlayer))
		{
			iItemValue *= 2;
		}

		// Easy target? Halve the value.
		if (GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsEasyTarget(eWithPlayer))
		{
			iItemValue /= 2;
		}
	}

	return iItemValue;
}

/// What is the value of trading a vote commitment?
int CvDealAI::GetVoteCommitmentValue(bool bFromMe, PlayerTypes eOtherPlayer, int iProposalID, int iVoteChoice, int iNumVotes, bool bRepeal, bool bUseEvenValue)
{
	int iValue = 100;

	if (iNumVotes == 0)
		return INT_MAX;

	// Giving our votes to them - Higher value for voting on things we dislike
	if (bFromMe)
	{
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
					if(eTargetPlayer != NO_PLAYER && (eTargetPlayer == GetPlayer()->GetID() || GET_PLAYER(eTargetPlayer).getTeam() == GetPlayer()->getTeam()))
					{
						return INT_MAX;
					}
				}
				//Let's look real quick to see if this is the world leader vote. If so, don't give ANYTHING away if we can win.
				if (pProposal->GetEffects()->bDiplomaticVictory)
				{
					// Vassals do not sell their votes!
					if (GetPlayer()->IsVassalOfSomeone())
						return INT_MAX;

					// Never support the opposition if we're on a human's team.
					if (GetPlayer()->IsAITeammateOfHuman())
						return INT_MAX;

					int iOurVotes = pLeague->CalculateStartingVotesForMember(GetPlayer()->GetID());
					int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
					if (iOurVotes >= iVotesNeededToWin)
					{
						return INT_MAX;
					}
					//We can't win? Well, our votes should be super-duper valuable, then!
					else
					{
						iValue *= 10;
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
					iValue += 2500 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_DISLIKE:
					iValue += 500 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_WEAK_DISLIKE:
					iValue += 300 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_NEUTRAL:
					iValue += 200 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_WEAK_LIKE:
				case CvLeagueAI::DESIRE_LIKE:
					iValue += 100 * iNumVotes;
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
					iValue += 500;
					break;
				case CvLeagueAI::ALIGNMENT_HATRED:
				case CvLeagueAI::ALIGNMENT_ENEMY:
				case CvLeagueAI::ALIGNMENT_WAR:
					iValue += 5000;
				default:
					break;
				}
				MajorCivApproachTypes eOtherPlayerApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
				if (eOtherPlayerApproach == MAJOR_CIV_APPROACH_HOSTILE || eOtherPlayerApproach == MAJOR_CIV_APPROACH_WAR)
				{
					iValue += 500;
				}
			}
			//If the total is more than half of the total votes on the board...that probably means it'll pass.
			if((iNumVotes + iTheirVotes) > (iVotesNeeded / 2))
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
			else
			{
				// They have a hoard of votes
				iValue *= 2;
			}
		}
	}
	// Giving their votes to something we want - Higher value for voting on things we like
	else
	{
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
					if(eTargetPlayer != NO_PLAYER)
					{
						//we don't ask them about things that involve themselves!
						if (eTargetPlayer == eOtherPlayer)
						{
							return INT_MAX;
						}
						//Let's look real quick to see if this is the world leader vote. If so, BUY EVERYTHING WE CAN if we can win with their votes in tow.
						if (pProposal->GetEffects()->bDiplomaticVictory)
						{
							//if we aren't the target, we don't care!
							if (eTargetPlayer == GetPlayer()->GetID())
							{
								int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
								if ((iOurVotes + iTheirVotes) >= iVotesNeededToWin)
								{
									iValue *= 10;
								}
								else
									iValue *= 2;
							}
							else
								return INT_MAX;
						}
						//don't ask them to embargo, decolonize, or end our vassalage!
						if (eTargetPlayer == GetPlayer()->GetID())
						{
							if (pProposal->GetEffects()->bEmbargoPlayer || pProposal->GetEffects()->bDecolonization || pProposal->GetEffects()->bEndAllCurrentVassals) 
								return INT_MAX;
						}
					}
				}
				else if (eProposerDecision == RESOLUTION_DECISION_RELIGION)
				{
					//Let's not ask them about religion if it's their faith.
					ReligionTypes eReligion = (ReligionTypes)pProposal->GetProposerDecision()->GetDecision();
					if (eReligion != NO_RELIGION)
						if (eReligion == GET_PLAYER(eOtherPlayer).GetReligions()->GetCurrentReligion(false))
							return INT_MAX;
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
					iValue += 1 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_DISLIKE:
					iValue += 5 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_WEAK_DISLIKE:	
					iValue += 15 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_NEUTRAL:
					iValue += 25 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_WEAK_LIKE:
					iValue += 50 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_LIKE:
					iValue += 100 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_STRONG_LIKE:
					iValue += 200 * iNumVotes;
					break;
				case CvLeagueAI::DESIRE_ALWAYS:
					iValue += 500 * iNumVotes;
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
					iValue += 50;
					break;
				case CvLeagueAI::ALIGNMENT_HATRED:
					iValue += 25;
					break;
				case CvLeagueAI::ALIGNMENT_ENEMY:
				case CvLeagueAI::ALIGNMENT_WAR:
					iValue += 1;
					break;
				default:
					break;
				}
				MajorCivApproachTypes eOtherPlayerApproach = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false);
				if (eOtherPlayerApproach == MAJOR_CIV_APPROACH_HOSTILE || eOtherPlayerApproach == MAJOR_CIV_APPROACH_WAR)
				{
					iValue /= 10;
				}
			}
			//If the total is more than half of the total votes on the board...that probably means it'll pass.
			if((iNumVotes + iOurVotes) > (iVotesNeeded / 2))
			{
				iValue /= 2;
			}
			// Adjust based on their vote total - Having lots of votes left means they could counter these ones and exploit us
			float fVotesRatio = (float)iNumVotes / (float)pLeague->CalculateStartingVotesForMember(eOtherPlayer);
			if (fVotesRatio > 0.5f)
			{
				// More than half their votes...they probably aren't going to screw us
				iValue *= 2;
			}
			else
			{
				// They have a hoard of votes
				iValue /= 2;
			}
		}
	}

	iValue = MAX(iValue, 0);

	// Are we trying to find the middle point between what we think this item is worth and what another player thinks it's worth?
	if (bUseEvenValue)
	{
		iValue += GET_PLAYER(eOtherPlayer).GetDealAI()->GetVoteCommitmentValue(!bFromMe, GetPlayer()->GetID(), iProposalID, iVoteChoice, iNumVotes, bRepeal, /*bUseEvenValue*/ false);

		iValue /= 2;
	}

	return iValue;
}

/// See if adding a Vote Commitment to their side of the deal helps even out pDeal
void CvDealAI::DoAddVoteCommitmentToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Them, but them is us. Please send Anton your save file and version.");

	CvWeightedVector<int> viTradeValues;
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if(pLeague == NULL || !pLeague)
	{
		return;
	}

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
					int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ false, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1);

					// If adding this to the deal doesn't take it over the limit, do it (pick best option below)

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
							if(iWeight == GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ false, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1))
							{
								if (!TooMuchAdded(eThem, iTotalValue, iWeight))
								{
									pDeal->AddVoteCommitment(eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal);
									iTotalValue = GetDealValue(pDeal);
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

/// See if adding a Vote Commitment to our side of the deal helps even out pDeal
void CvDealAI::DoAddVoteCommitmentToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if(pLeague == NULL || !pLeague)
	{
		return;
	}

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
					int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ true, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1);

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
				//reverse!
				for (int iRanking = viTradeValues.size() - 1; iRanking >= 0; iRanking--)
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
								if (iWeight == GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ true, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1))
								{
									if (!TooMuchAdded(eThem, iTotalValue, iWeight, true))
									{
										pDeal->AddVoteCommitment(eMyPlayer, iProposalID, iVoteChoice, iNumVotes, bRepeal);
										iTotalValue = GetDealValue(pDeal);
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


/// See if adding 3rd Party War to their side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyWarToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Them, but them is us. Please send Anton your save file and version.");
	

	CvWeightedVector<int> viTradeValues;

	
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

				if (GET_TEAM(GET_PLAYER(GetPlayer()->GetID()).getTeam()).IsVassalOfSomeone() || GET_TEAM(GET_PLAYER(eThem).getTeam()).IsVassalOfSomeone() || GET_TEAM(eOtherTeam).IsVassalOfSomeone())
					continue;

				// Can't already be a War Commitment in the Deal
				if(!pDeal->IsThirdPartyWarTrade(eThem, eOtherTeam) && !pDeal->IsThirdPartyWarTrade(eMyPlayer, eOtherTeam))
				{
					if (pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_THIRD_PARTY_WAR, eOtherTeam))
					{
						int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_WAR, /*bFromMe*/ false, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1);

						// If adding this to the deal doesn't take it over the limit, do it (pick best option below)
						if (iItemValue == INT_MAX)
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
					PlayerTypes eLoopPlayer = (PlayerTypes)viTradeValues.GetElement(iRanking);

					if(eLoopPlayer == NO_PLAYER)
						continue;
						
					TeamTypes eOtherTeam = GET_PLAYER(eLoopPlayer).getTeam();

					if (!TooMuchAdded(eThem, iTotalValue, iWeight))
					{
						pDeal->AddThirdPartyWar(eThem, eOtherTeam);
						iTotalValue = GetDealValue(pDeal);
						return;
					}
									
				}
			}
		}
	}
}

/// See if adding 3rd Party War to our side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyWarToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");
	
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

				if (GET_TEAM(GET_PLAYER(GetPlayer()->GetID()).getTeam()).IsVassalOfSomeone() || GET_TEAM(GET_PLAYER(eThem).getTeam()).IsVassalOfSomeone() || GET_TEAM(eOtherTeam).IsVassalOfSomeone())
					continue;

				// Can't already be a Trade Commitment in the Deal
				if(!pDeal->IsThirdPartyWarTrade(eThem, eOtherTeam) && !pDeal->IsThirdPartyWarTrade(eMyPlayer, eOtherTeam))
				{
					if (pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_THIRD_PARTY_WAR, eOtherTeam))
					{
						int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_WAR, /*bFromMe*/ true, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1);

						// If adding this to the deal doesn't take it over the limit, do it
						if(iItemValue==INT_MAX)
						{
							continue;
						}
						if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
						{
							pDeal->AddThirdPartyWar(eMyPlayer, eOtherTeam);
							iTotalValue = GetDealValue(pDeal);
							return;
						}
					}
				}
			}
		}
	}
}
/// See if adding 3rd Party Peace to their side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyPeaceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Them, but them is us. Please send Anton your save file and version.");

	CvWeightedVector<int> viTradeValues;


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
						int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_PEACE, /*bFromMe*/ false, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1);

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
					PlayerTypes eLoopPlayer = (PlayerTypes)viTradeValues.GetElement(iRanking);

					if (eLoopPlayer == NO_PLAYER)
						continue;
						
					TeamTypes eOtherTeam = GET_PLAYER(eLoopPlayer).getTeam();

					if (!TooMuchAdded(eThem, iTotalValue, iWeight))
					{
						pDeal->AddThirdPartyWar(eThem, eOtherTeam);
						iTotalValue = GetDealValue(pDeal);
						return;
					}								
				}
			}
		}
	}
}

/// See if adding a 3rd Party Peace deal to our side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyPeaceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");

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
						int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_PEACE, /*bFromMe*/ true, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1);

						// If adding this to the deal doesn't take it over the limit, do it
						if(iItemValue==INT_MAX)
						{
							continue;
						}
						if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
						{
							pDeal->AddThirdPartyWar(eMyPlayer, eOtherTeam);
							iTotalValue = GetDealValue(pDeal);
						}
					}
				}
			}
		}
	}
}

/// See if adding a Resource to their side of the deal helps even out pDeal
void CvDealAI::DoAddLuxuryResourceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Them, but them is us.  Please show Jon");

	CvWeightedVector<int> viTradeValues;

	int iDealDuration = pDeal->GetDuration();
	if(iTotalValue < 0 || pDeal->GetDemandingPlayer() != NO_PLAYER)
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		// Look to trade Luxuries first
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;

			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
				continue;

			//already in deal?
			if (pDeal->IsResourceTrade(eThem, eResource))
				continue;

			// Don't bother looking at this Resource if the other player doesn't even have any of it
			int iResourceQuantity = GET_PLAYER(eThem).getNumResourceAvailable(eResource, false);
			if(iResourceQuantity < 1)
				continue;

			// Don't bother if we already have it
			if (GetPlayer()->getNumResourceAvailable(eResource, true) > 0)
				continue;

			// Don't bother if we wouldn't get Happiness from it due to World Congress
			if(GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(eMyPlayer, eResource))
				continue;

			// Quantity is always 1 if it's a Luxury
			iResourceQuantity = 1;

			// See if they can actually trade it to us
			if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration);

				if (iItemValue > 0 && GET_PLAYER(eMyPlayer).GetPlayerTraits()->IsImportsCountTowardsMonopolies())
				{
					if(GetPlayer()->GetMonopolyPercent(eResource) < GC.getGame().GetGreatestPlayerResourceMonopolyValue(eResource))
					{
						//we don't want resources that won't get us a bonus.
						int iNumResourceOwned = GetPlayer()->getNumResourceTotal(eResource, false);
						int iNumResourceImported = GetPlayer()->getNumResourceTotal(eResource, true);
						//FIXME: does this make sense?
						if (iNumResourceOwned == 0 && iNumResourceImported > 0)
						{
							bool bBad = false;
							for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
							{
								if (pkResourceInfo->getYieldChangeFromMonopoly((YieldTypes)iJ) > 0)
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
				}

				// If adding this to the deal doesn't take it over the limit, do it (pick best option below)
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
		if (viTradeValues.size() > 0)
		{
			for (int iRanking = 0; iRanking < viTradeValues.size(); iRanking++)
			{
				int iWeight = viTradeValues.GetWeight(iRanking);
				if (iWeight != 0)
				{
					ResourceTypes eResource = (ResourceTypes)viTradeValues.GetElement(iRanking);
					if (eResource == NO_RESOURCE)
						continue;

					const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
					if (pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
						continue;

					int iResourceQuantity = GET_PLAYER(eThem).getNumResourceAvailable(eResource, false);

					// Don't bother looking at this Resource if the other player doesn't even have any of it
					if (iResourceQuantity <= 0)
						continue;

					iResourceQuantity = 1;

					if (pDeal->GetDemandingPlayer() != NO_PLAYER || !TooMuchAdded(eThem, iTotalValue, iWeight))
					{
						// Try to change the current item if it already exists, otherwise add it
						if (!pDeal->ChangeResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration);
							iTotalValue = GetDealValue(pDeal);
						}
					}
				}
			}
		}
	}
}

/// See if adding a Resource to our side of the deal helps even out pDeal
void CvDealAI::DoAddLuxuryResourceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Us, but them is us.  Please show Jon");

	CvWeightedVector<int> viTradeValues;


	int iDealDuration = pDeal->GetDuration();
	if(iTotalValue > 0)
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		int iItemValue;

		ResourceTypes eResource;
		int iResourceQuantity;

		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			eResource = (ResourceTypes) iResourceLoop;

			const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if (pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
				continue;

			//already in deal?
			if (pDeal->IsResourceTrade(eThem, eResource))
				continue;

			iResourceQuantity = GET_PLAYER(eMyPlayer).getNumResourceAvailable(eResource, false);

			// Don't bother looking at this Resource if we'd give away our last copy! Unless we're special.
			int iMin = GET_PLAYER(eMyPlayer).GetPlayerTraits()->GetLuxuryHappinessRetention() ? 1 : 2;
			if (iResourceQuantity < iMin)
			{
				continue;
			}

			//reset to 1 because we only want to sell one!
			iResourceQuantity = 1;

			// Don't bother if they wouldn't get Happiness from it due to World Congress
			if(GC.getGame().GetGameLeagues()->IsLuxuryHappinessBanned(eThem, eResource))
				continue;

			// See if we can actually trade it to them
			if (pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
			{
				iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ true, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration);

				// If adding this to the deal doesn't take it under the min limit, do it
				if (iItemValue == INT_MAX)
				{
					continue;
				}

				viTradeValues.push_back(eResource, iItemValue);
			}
		}

		viTradeValues.SortItems();
		if(viTradeValues.size() > 0)
		{
			//reverse!
			for (int iRanking = viTradeValues.size()-1; iRanking >= 0; iRanking--)
			{
				int iWeight = viTradeValues.GetWeight(iRanking);
				if (iWeight != 0)
				{
					eResource = (ResourceTypes)viTradeValues.GetElement(iRanking);
					if (eResource == NO_RESOURCE)
						continue;

					const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
					if (pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
						continue;
							
					iResourceQuantity = 1;

					if (!TooMuchAdded(eThem, iTotalValue, iWeight, true))
					{
						// Try to change the current item if it already exists, otherwise add it
						if (!pDeal->ChangeResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration);
							iTotalValue = GetDealValue(pDeal);
						}
					}
				}
			}
		}
	}
}

/// See if adding a Resource to their side of the deal helps even out pDeal
void CvDealAI::DoAddStrategicResourceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Them, but them is us.  Please show Jon");

	typedef pair<ResourceTypes, int> TradeItem;
	vector<OptionWithScore<TradeItem>> vOptions;

	if (iTotalValue < 1)
		return;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	// Now look at Strategic Resources
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes)iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_STRATEGIC)
			continue;

		//ignore if already in deal on the other side
		if (pDeal->IsResourceTrade(eMyPlayer, eResource))
			continue;

		//AI should not sell if they are importing in parallel
		if (!GET_PLAYER(eThem).isHuman() && GET_PLAYER(eThem).getResourceImportFromMajor(eResource)>0)
			continue;

		// Don't bother looking at this resource if they don't even have any of it
		int iMaxResourceQuantity = GET_PLAYER(eThem).getNumResourceAvailable(eResource, false);
		if (iMaxResourceQuantity < 1)
			continue;

		// Don't try to buy all of it
		int iResourceQuantity = 0;
		
		int iAlreadyInDeal = pDeal->GetNumResourcesInDeal(eThem, eResource);
		if (iAlreadyInDeal > 0)
		{
			iResourceQuantity = min(iAlreadyInDeal + 1, iMaxResourceQuantity);
		}
		else
		{
			iResourceQuantity = max((iMaxResourceQuantity / 3), 1);
		}

		// See if they can actually trade it to us
		if (pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
		{
			int iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, pDeal->GetDuration());

			// If adding this to the deal doesn't take it over the limit, do it (pick best option below)
			if (iItemValue == INT_MAX)
				continue;

			vOptions.push_back(OptionWithScore<TradeItem>(TradeItem(eResource, iResourceQuantity), iItemValue));
		}
	}

	sort(vOptions.begin(), vOptions.end());
	for (size_t i=0; i<vOptions.size(); i++)
	{
		ResourceTypes eResource = vOptions[i].option.first;
		int iQuantity = vOptions[i].option.second;
		int iScore = vOptions[i].score;

		if (!TooMuchAdded(eThem, iTotalValue, iScore))
		{
			// Try to change the current item if it already exists, otherwise add it
			if (!pDeal->ChangeResourceTrade(eThem, eResource, iQuantity, pDeal->GetDuration()))
			{
				pDeal->AddResourceTrade(eThem, eResource, iQuantity, pDeal->GetDuration());
				iTotalValue = GetDealValue(pDeal);
			}
		}
	}
}

/// See if adding a Resource to our side of the deal helps even out pDeal
void CvDealAI::DoAddStrategicResourceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Us, but them is us.  Please show Jon");

	typedef pair<ResourceTypes, int> TradeItem;
	vector<OptionWithScore<TradeItem>> vOptions;

	if (iTotalValue < 1)
		return;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes)iResourceLoop;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_STRATEGIC)
			continue;

		//ignore if already in the deal on the other side
		if (pDeal->IsResourceTrade(eThem, eResource))
			continue;

		//don't sell if we're importing at the same time
		//todo: or is this a chance for arbitrage?
		if (GET_PLAYER(eMyPlayer).getResourceImportFromMajor(eResource) > 0)
			continue;

		//always keep some for ourselves
		int iMaxResourceQuantity = GET_PLAYER(eMyPlayer).getNumResourceAvailable(eResource, false)/2;
		if (iMaxResourceQuantity < 2)
			continue;

		//how do we judge this? A good rule of thumb: never give away more than we're getting
		//but don't try to buy more than twice as much as were getting either
		int iResourceQuantity = 1;
		int iTheirResourceCount = pDeal->GetNumStrategicsOnTheirSide(eThem);
		if (iMaxResourceQuantity < iTheirResourceCount / 2)
			continue;
		else if (iMaxResourceQuantity > iTheirResourceCount)
			//offer max 2 copies more
			iResourceQuantity = min(iMaxResourceQuantity,iTheirResourceCount+2);

		//already in deal? add one more to the mix and reset quantity
		if (pDeal->IsResourceTrade(eThem, eResource))
		{
			iResourceQuantity = pDeal->GetNumResourcesInDeal(eMyPlayer, eResource);
			pDeal->RemoveResourceTrade(eResource);
			//and bump the quantity.
			iResourceQuantity++;
			iTotalValue = GetDealValue(pDeal);
		}

		//don't exceed total.
		iResourceQuantity = min(iResourceQuantity, iMaxResourceQuantity);

		// See if we can actually trade it to them
		if (pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_RESOURCES, eResource, iResourceQuantity))
		{
			int iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ true, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, pDeal->GetDuration());
			if (iItemValue == INT_MAX)
				continue;

			vOptions.push_back(OptionWithScore<TradeItem>(TradeItem(eResource, iResourceQuantity), iItemValue));
		}
	}

	sort(vOptions.begin(), vOptions.end());
	for (size_t i=0; i<vOptions.size(); i++)
	{
		ResourceTypes eResource = vOptions[i].option.first;
		int iQuantity = vOptions[i].option.second;
		int iScore = vOptions[i].score;

		// If adding this to the deal doesn't take it under the min limit, do it
		if (!TooMuchAdded(eThem, iTotalValue, iScore, true))
		{
			// Try to change the current item if it already exists, otherwise add it
			if (!pDeal->ChangeResourceTrade(eMyPlayer, eResource, iQuantity, pDeal->GetDuration()))
			{
				pDeal->AddResourceTrade(eMyPlayer, eResource, iQuantity, pDeal->GetDuration());
				iTotalValue = GetDealValue(pDeal);
			}
		}
	}
}


/// See if adding Embassy to their side of the deal helps even out pDeal
void CvDealAI::DoAddEmbassyToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Embassy to Them, but them is us.  Please show Jon");

	if(iTotalValue < 0)
	{
		if(!pDeal->IsAllowEmbassyTrade(eThem))
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if they can actually trade it to us
			if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_ALLOW_EMBASSY))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_ALLOW_EMBASSY, /*bFromMe*/ false, eThem, -1, -1, -1, /*bFlag1*/false, -1);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, iTotalValue, iItemValue))
				{
					pDeal->AddAllowEmbassy(eThem);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

/// See if adding Embassy to our side of the deal helps even out pDeal
void CvDealAI::DoAddEmbassyToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Embassy to Us, but them is us.  Please show Jon");

	if(iTotalValue > 0)
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		if(!pDeal->IsAllowEmbassyTrade(eMyPlayer))
		{
			// See if we can actually trade it to them
			if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_ALLOW_EMBASSY))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_ALLOW_EMBASSY, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, -1);

				// If adding this to the deal doesn't take it under the min limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
				{
					pDeal->AddAllowEmbassy(eMyPlayer);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

/// See if adding Open Borders to their side of the deal helps even out pDeal
void CvDealAI::DoAddOpenBordersToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Them, but them is us.  Please show Jon");

	int iDealDuration = pDeal->GetDuration();
	
	if(iTotalValue < 0)
	{
		if(!pDeal->IsOpenBordersTrade(eThem))
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if they can actually trade it to us
			if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_OPEN_BORDERS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_OPEN_BORDERS, /*bFromMe*/ false, eThem, -1, -1, -1, /*bFlag1*/false, iDealDuration);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, iTotalValue, iItemValue))
				{
					pDeal->AddOpenBorders(eThem, iDealDuration);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

/// See if adding Open Borders to our side of the deal helps even out pDeal
void CvDealAI::DoAddOpenBordersToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iDealDuration = pDeal->GetDuration();

	if (iTotalValue > 0)
	{
		if (!pDeal->IsOpenBordersTrade(eMyPlayer))
		{
			// See if we can actually trade it to them
			if (pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_OPEN_BORDERS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_OPEN_BORDERS, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, iDealDuration);

				// If adding this to the deal doesn't take it under the min limit, do it
				if (iItemValue == INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
				{
					pDeal->AddOpenBorders(eMyPlayer, iDealDuration);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

/// See if adding Cities to our side of the deal helps even out pDeal
void CvDealAI::DoAddCitiesToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	// If we're not the one surrendering here, don't bother
	if(pDeal->IsPeaceTreatyTrade(eThem) && pDeal->GetSurrenderingPlayer() != m_pPlayer->GetID())
		return;

	// AIs will only do city exchanges. Versus humans we only offer a city if they offer one too
	if (!pDeal->ContainsItemType(TRADE_ITEM_CITIES, eThem))
		return;

	// We don't owe them anything
	if(iTotalValue <= 0)
		return;

	// Create vector of the losing players' Cities so we can see which are the closest to the winner
	CvWeightedVector<int> viCityPriceRatio;

	// Loop through all of our cities
	int iCityLoop;
	CvPlayer* pSellingPlayer = GetPlayer();
	for(CvCity* pLoopCity = pSellingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pSellingPlayer->nextCity(&iCityLoop))
	{
		//make sure we're not just flipping two border towns
		CvCity* pTheirClosest = GET_PLAYER(eThem).GetClosestCityByPathLength(pLoopCity->plot());
		if (pTheirClosest && pDeal->IsCityTrade(eThem, pTheirClosest->getX(), pTheirClosest->getY()))
			continue;

		int iWhatTheyWouldPay = GetCityValueForBuyer(pLoopCity, m_pPlayer->GetID(), eThem, pDeal->IsPeaceTreatyTrade(eThem));
		int iWhatIWouldPay = GetCityValueForBuyer(pLoopCity, eThem, m_pPlayer->GetID(), pDeal->IsPeaceTreatyTrade(eThem));

		if (iWhatTheyWouldPay == INT_MAX || iWhatIWouldPay == INT_MAX)
		{
			continue;
		}

		int iRatio = (iWhatTheyWouldPay * 100) / max(1,iWhatIWouldPay);
		//if the deal is good
		if(iRatio>111)
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
		CvCity* pLoopCity = pSellingPlayer->getCity(iSortedCityID);

		// See if we can actually trade it to them
		if(pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eThem, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
		{
			int iItemValue = GetCityValueForBuyer(pLoopCity, m_pPlayer->GetID(), eThem, pDeal->IsPeaceTreatyTrade(eThem));
			if (iItemValue == INT_MAX)
				continue;

			// If adding this to the deal doesn't take it under the min limit, do it
			if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
			{
				OutputDebugString( CvString::format("Adding %s to deal. Seller %s, buyer %s, price ratio %d\n", 
					pLoopCity->getName().c_str(), pSellingPlayer->getName(), GET_PLAYER(eThem).getName(), viCityPriceRatio.GetWeight(iSortedCityIndex) ).c_str() );
				pDeal->AddCityTrade(m_pPlayer->GetID(), iSortedCityID);
				iTotalValue = GetDealValue(pDeal);
				return;
			}
		}

	}
}


/// See if adding Cities to their side of the deal helps even out pDeal
void CvDealAI::DoAddCitiesToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	// If they're not the one surrendering here, don't bother
	if(pDeal->IsPeaceTreatyTrade(eThem) && pDeal->GetSurrenderingPlayer() != eThem)
		return;

	// AIs will only do city exchanges. Versus humans we only buy a city if they want one too
	if (!pDeal->ContainsItemType(TRADE_ITEM_CITIES, GetPlayer()->GetID()))
		return;

	// They don't owe us anything
	if(iTotalValue > 0 && pDeal->GetDemandingPlayer() == NO_PLAYER)
		return;

	// Create vector of the losing players' Cities so we can see which are the closest to the winner
	CvWeightedVector<int> viCityPriceRatio;

	// Loop through all of the their Cities
	int iCityLoop;
	CvPlayer* pSellingPlayer  = &GET_PLAYER(eThem);
	for(CvCity* pLoopCity = pSellingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pSellingPlayer->nextCity(&iCityLoop))
	{
		//make sure we're not just flipping two border towns
		CvCity* pOurClosest = GetPlayer()->GetClosestCityByPathLength(pLoopCity->plot());
		if (pOurClosest && pDeal->IsCityTrade(GetPlayer()->GetID(), pOurClosest->getX(), pOurClosest->getY()))
			continue;

		int iWhatTheyWouldPay = GetCityValueForBuyer(pLoopCity, m_pPlayer->GetID(), eThem, pDeal->IsPeaceTreatyTrade(eThem));
		int iWhatIWouldPay = GetCityValueForBuyer(pLoopCity, eThem, m_pPlayer->GetID(), pDeal->IsPeaceTreatyTrade(eThem));

		if (iWhatTheyWouldPay == INT_MAX || iWhatIWouldPay == INT_MAX)
		{
			continue;
		}

		int iRatio = (iWhatTheyWouldPay * 100) / max(1,iWhatIWouldPay);
		//if the deal is good
		if (iRatio<88) 
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
		CvCity* pLoopCity = pSellingPlayer->getCity(iSortedCityID);

		// See if we can actually trade it to them
		if (pDeal->IsPossibleToTradeItem(eThem, GetPlayer()->GetID(), TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
		{
			int iItemValue = GetCityValueForBuyer(pLoopCity, eThem, GetPlayer()->GetID(), pDeal->IsPeaceTreatyTrade(eThem));
			if (iItemValue == INT_MAX)
				continue;

			// If adding this to the deal doesn't take it under the min limit, do it
			if (!TooMuchAdded(eThem, iTotalValue, iItemValue))
			{
				OutputDebugString( CvString::format("Adding %s to deal. Seller %s, buyer %s, price ratio %d\n", 
					pLoopCity->getName().c_str(), pSellingPlayer->getName(), GetPlayer()->getName(), viCityPriceRatio.GetWeight(iSortedCityIndex) ).c_str() );
				pDeal->AddCityTrade(eThem, iSortedCityID);
				iTotalValue = GetDealValue(pDeal);
				return;
			}
		}
	}
}


/// See if adding Gold to their side of the deal helps even out pDeal
void CvDealAI::DoAddGoldToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Gold to Them, but them is us.  Please show Jon");

	int iNumGoldAlreadyInTrade = pDeal->GetGoldTrade(eThem);
	//if raw gold in deal, remove so we can refresh.
	if (iNumGoldAlreadyInTrade > 0)
	{
		DoRemoveGoldFromThem(pDeal, eThem, iTotalValue);
		iNumGoldAlreadyInTrade = 0;
		iTotalValue = GetDealValue(pDeal);
	}
	if (iTotalValue < 0 || pDeal->GetDemandingPlayer() != NO_PLAYER)
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		// Can't already be Gold from the other player in the Deal
		if(pDeal->GetGoldTrade(eMyPlayer) == 0)
		{
			int iNumGold = GetGoldForForValueExchange(-iTotalValue, /*bNumGoldFromValue*/ true, /*bFromMe*/ false, eThem, /*bRoundUp*/ false);

			if(iNumGold <= 0)
			{
				return;
			}

			iNumGold += iNumGoldAlreadyInTrade;
			iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eThem, TRADE_ITEM_GOLD));

			//iNumGold = min(iNumGold, GET_PLAYER(eThem).GetTreasury()->GetGold());
			if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eThem, iNumGold))
			{
				pDeal->AddGoldTrade(eThem, iNumGold);
			}

			iTotalValue = GetDealValue(pDeal);

		}
	}
}

/// See if adding Gold to our side of the deal helps even out pDeal
void CvDealAI::DoAddGoldToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Gold to Us, but them is us.  Please show Jon");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	int iNumGoldAlreadyInTrade = pDeal->GetGoldTrade(eMyPlayer);

	//if raw gold in deal, remove so we can refresh.
	if (iNumGoldAlreadyInTrade > 0)
	{
		DoRemoveGoldFromUs(pDeal, iTotalValue);
		iNumGoldAlreadyInTrade = 0;
		iTotalValue = GetDealValue(pDeal);
	}
	if(iTotalValue > 0)
	{
		// Can't already be Gold from the other player in the Deal
		if(pDeal->GetGoldTrade(eThem) == 0)
		{
			int iNumGold = GetGoldForForValueExchange(iTotalValue, /*bNumGoldFromValue*/ true, /*bFromMe*/ true, eThem, /*bRoundUp*/ false);

			if(iNumGold < 0)
			{
				return;
			}
				
			iNumGold += iNumGoldAlreadyInTrade;
			iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eMyPlayer, TRADE_ITEM_GOLD));

			//iNumGold = min(iNumGold, GET_PLAYER(eMyPlayer).GetTreasury()->GetGold());
			if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eMyPlayer, iNumGold))
			{
				pDeal->AddGoldTrade(eMyPlayer, iNumGold);
			}

			iTotalValue = GetDealValue(pDeal);

		}
	}
}

/// See if adding Gold Per Turn to their side of the deal helps even out pDeal
void CvDealAI::DoAddGPTToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add GPT to Them, but them is us.  Please show Jon");

	int iDealDuration = pDeal->GetDuration();
	int iNumGPTAlreadyInTrade = pDeal->GetGoldPerTurnTrade(eThem);
	//if they already have GPT in this trade, remove it so we can refresh the value.
	if (iNumGPTAlreadyInTrade > 0)
	{
		DoRemoveGPTFromThem(pDeal, eThem, iTotalValue);
		iNumGPTAlreadyInTrade = 0;
		iTotalValue = GetDealValue(pDeal);
	}
	if (iTotalValue < 0 || pDeal->GetDemandingPlayer() != NO_PLAYER)
	{
		if(GET_PLAYER(eThem).calculateGoldRate() > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// Can't already be GPT from the other player in the Deal
			if(pDeal->GetGoldPerTurnTrade(eMyPlayer) == 0)
			{
				int iNumGPT = GetGPTforForValueExchange(-iTotalValue, /*bNumGPTFromValue*/ true, iDealDuration, /*bFromMe*/ false, eThem, /*bRoundUp*/ false, !GET_PLAYER(eThem).isHuman());

				if(iNumGPT < 0)
				{
					return;
				}

				iNumGPT += iNumGPTAlreadyInTrade;
				iNumGPT = min(iNumGPT, GET_PLAYER(eThem).calculateGoldRate()-2);

				if (iNumGPT <= 0)
					return;

				if(iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eThem, iNumGPT, iDealDuration))
				{
					pDeal->AddGoldPerTurnTrade(eThem, iNumGPT, iDealDuration);
				}

				iTotalValue = GetDealValue(pDeal);
			}
		}
	}
}

/// See if adding Gold Per Turn to our side of the deal helps even out pDeal
void CvDealAI::DoAddGPTToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add GPT to Us, but them is us.  Please show Jon");

	int iDealDuration = pDeal->GetDuration();

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	int iNumGPTAlreadyInTrade = pDeal->GetGoldPerTurnTrade(eMyPlayer);

	//if we already have GPT in this trade, remove it so we can refresh the value.
	if (iNumGPTAlreadyInTrade > 0)
	{
		DoRemoveGPTFromUs(pDeal, iNumGPTAlreadyInTrade);
		iNumGPTAlreadyInTrade = 0;
		iTotalValue = GetDealValue(pDeal);
	}		
	if (iTotalValue > 0)
	{	
		if(GET_PLAYER(eMyPlayer).calculateGoldRate() > 0)
		{
			// Can't already be GPT from the other player in the Deal
			if(pDeal->GetGoldPerTurnTrade(eThem) == 0)
			{
				int iNumGPT = GetGPTforForValueExchange(iTotalValue, /*bNumGPTFromValue*/ true, iDealDuration, /*bFromMe*/ true, eThem, false, !GET_PLAYER(eThem).isHuman());

				if(iNumGPT < 0)
				{
					return;
				}
					
				iNumGPT += iNumGPTAlreadyInTrade;
				iNumGPT = min(iNumGPT, GET_PLAYER(eMyPlayer).calculateGoldRate()-2);

				int iItemValue = GetGPTforForValueExchange(iNumGPT, false, iDealDuration, /*bFromMe*/ true, eThem, false, /*bRoundUp*/ !GET_PLAYER(eThem).isHuman());
				if (iItemValue <= 0)
					return;

				if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
				{
					if(iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration))
					{
						pDeal->AddGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration);
					}

					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

void CvDealAI::DoAddItemsToThem(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValue)
{
	if (pDeal->DoNotModifyFrom() && pDeal->GetFromPlayer() == eOtherPlayer)
	{
		DoAddGPTToThem(pDeal, eOtherPlayer, iTotalValue);
		DoAddGoldToThem(pDeal, eOtherPlayer, iTotalValue);
		return;
	}

	DoAddVassalageToThem(pDeal, eOtherPlayer, iTotalValue);
	DoAddCitiesToThem(pDeal, eOtherPlayer, iTotalValue);
	DoAddTechToThem(pDeal, eOtherPlayer, iTotalValue);
	DoAddRevokeVassalageToThem(pDeal, eOtherPlayer, iTotalValue);
	DoAddMapsToThem(pDeal, eOtherPlayer, iTotalValue);
	DoAddThirdPartyWarToThem(pDeal, eOtherPlayer, iTotalValue);
	DoAddThirdPartyPeaceToThem(pDeal, eOtherPlayer, iTotalValue);
	DoAddVoteCommitmentToThem(pDeal, eOtherPlayer, iTotalValue);
	//if a strategics trade, try strategics first.
	if (pDeal->IsStrategicsTrade())
	{
		DoAddStrategicResourceToThem(pDeal, eOtherPlayer, iTotalValue);
		DoAddLuxuryResourceToThem(pDeal, eOtherPlayer, iTotalValue);
	}
	else
	{
		DoAddLuxuryResourceToThem(pDeal, eOtherPlayer, iTotalValue);
		DoAddStrategicResourceToThem(pDeal, eOtherPlayer, iTotalValue);
	}
	DoAddOpenBordersToThem(pDeal, eOtherPlayer, iTotalValue);
	DoAddGoldToThem(pDeal, eOtherPlayer, iTotalValue);
	DoAddGPTToThem(pDeal, eOtherPlayer, iTotalValue);
}

void CvDealAI::DoAddItemsToUs(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValue)
{
	if (pDeal->DoNotModifyTo() && pDeal->GetToPlayer() == eOtherPlayer)
	{
		DoAddGPTToUs(pDeal, eOtherPlayer, iTotalValue);
		DoAddGoldToUs(pDeal, eOtherPlayer, iTotalValue);
		return;
	}

	DoAddVassalageToUs(pDeal, eOtherPlayer, iTotalValue);
	DoAddCitiesToUs(pDeal, eOtherPlayer, iTotalValue);
	DoAddTechToUs(pDeal, eOtherPlayer, iTotalValue);
	DoAddMapsToUs(pDeal, eOtherPlayer, iTotalValue);
	DoAddThirdPartyWarToUs(pDeal, eOtherPlayer, iTotalValue);
	DoAddThirdPartyPeaceToUs(pDeal, eOtherPlayer, iTotalValue);
	DoAddVoteCommitmentToUs(pDeal, eOtherPlayer, iTotalValue);

	//if a strategics trade, try strategics first.
	if (pDeal->IsStrategicsTrade())
	{
		DoAddStrategicResourceToUs(pDeal, eOtherPlayer, iTotalValue);
		DoAddLuxuryResourceToUs(pDeal, eOtherPlayer, iTotalValue);
	}
	else
	{
		DoAddLuxuryResourceToUs(pDeal, eOtherPlayer, iTotalValue);
		DoAddStrategicResourceToUs(pDeal, eOtherPlayer, iTotalValue);
	}
	DoAddOpenBordersToUs(pDeal, eOtherPlayer, iTotalValue);
	DoAddGPTToUs(pDeal, eOtherPlayer, iTotalValue);

	//do not offer lump sums of gold for resources
	if (!pDeal->IsResourceTrade(eOtherPlayer,NO_RESOURCE))
		DoAddGoldToUs(pDeal, eOtherPlayer, iTotalValue);
}

/// See if removing Gold Per Turn from their side of the deal helps even out pDeal
void CvDealAI::DoRemoveGPTFromThem(CvDeal* pDeal, PlayerTypes eThem, int& iNumGPT)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove GPT from Them, but them is us.  Please show Jon");

	int iDealDuration = pDeal->GetDuration();
	int iNumGoldPerTurnInThisDeal = pDeal->GetGoldPerTurnTrade(eThem);
	if (iNumGoldPerTurnInThisDeal > 0)
	{
		// Found some GoldPerTurn to remove
		int iNumGoldPerTurnToRemove = min(iNumGPT, iNumGoldPerTurnInThisDeal);
		iNumGoldPerTurnInThisDeal -= iNumGoldPerTurnToRemove;

		// Removing ALL GoldPerTurn, so just erase the item from the deal
		if (iNumGoldPerTurnInThisDeal == 0)
		{
			pDeal->RemoveByType(TRADE_ITEM_GOLD_PER_TURN);
		}
		// Remove some of the GoldPerTurn from the deal
		else
		{
			if (!pDeal->ChangeGoldPerTurnTrade(eThem, iNumGoldPerTurnInThisDeal, iDealDuration))
			{
				CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove GoldPerTurn from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
			}
		}
	}
}

/// See if removing Gold Per Turn from our side of the deal helps even out pDeal
void CvDealAI::DoRemoveGPTFromUs(CvDeal* pDeal, int& iNumGPT)
{
	int iDealDuration = pDeal->GetDuration();
	
	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iNumGoldPerTurnInThisDeal = pDeal->GetGoldPerTurnTrade(eMyPlayer);
	if(iNumGoldPerTurnInThisDeal > 0)
	{
		// Found some GoldPerTurn to remove
		int iNumGoldPerTurnToRemove = min(iNumGPT, iNumGoldPerTurnInThisDeal);
		iNumGoldPerTurnInThisDeal -= iNumGoldPerTurnToRemove;

		// Removing ALL GoldPerTurn, so just erase the item from the deal
		if(iNumGoldPerTurnInThisDeal == 0)
		{
			pDeal->RemoveByType(TRADE_ITEM_GOLD_PER_TURN);
		}
		// Remove some of the GoldPerTurn from the deal
		else
		{
			if(!pDeal->ChangeGoldPerTurnTrade(eMyPlayer, iNumGoldPerTurnInThisDeal, iDealDuration))
			{
				CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove GoldPerTurn from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
			}
		}
	}
}

/// See if removing Gold from their side of the deal helps even out pDeal
void CvDealAI::DoRemoveGoldFromThem(CvDeal* pDeal, PlayerTypes eThem, int& iGold)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove Gold from Them, but them is us.  Please show Jon");

	int iNumGoldInThisDeal = pDeal->GetGoldTrade(eThem);
	if(iNumGoldInThisDeal > 0)
	{
		// Found some Gold to remove
		int iNumGoldToRemove = min(iNumGoldInThisDeal, iGold);
		iNumGoldInThisDeal -= iNumGoldToRemove;

		// Removing ALL Gold, so just erase the item from the deal
		if(iNumGoldInThisDeal == 0)
		{
			pDeal->RemoveByType(TRADE_ITEM_GOLD);
		}
		// Remove some of the Gold from the deal
		else
		{
			if(!pDeal->ChangeGoldTrade(eThem, iNumGoldInThisDeal))
			{
				CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove Gold from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
			}
		}
	}
}

/// See if removing Gold from our side of the deal helps even out pDeal
void CvDealAI::DoRemoveGoldFromUs(CvDeal* pDeal, int& iGold)
{
	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iNumGoldInThisDeal = pDeal->GetGoldTrade(eMyPlayer);
	if (iNumGoldInThisDeal > 0)
	{
		
		// Found some Gold to remove
		int iNumGoldToRemove = min(iNumGoldInThisDeal, iGold);
		iNumGoldInThisDeal -= iNumGoldToRemove;

		// Removing ALL Gold, so just erase the item from the deal
		if (iNumGoldInThisDeal == 0)
		{
			pDeal->RemoveByType(TRADE_ITEM_GOLD);
		}
		// Remove some of the Gold from the deal
		else
		{
			if (!pDeal->ChangeGoldTrade(eMyPlayer, iNumGoldInThisDeal))
			{
				CvAssertMsg(false, "DEAL_AI: DealAI is trying to remove Gold from a deal but couldn't find the item for some reason.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
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


			int iValue = GetDealValue(pDeal);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(iValue);
				m_pPlayer->SetCachedValueOfPeaceWithHuman(iValue);
			}

		}
		// AI is asking human to surrender
		else if(ePeaceTreatyImWillingToAccept > PEACE_TREATY_WHITE_PEACE)
		{
			pDeal->SetSurrenderingPlayer(eOtherPlayer);
			pDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToAccept);

			DoAddItemsToDealForPeaceTreaty(eOtherPlayer, pDeal, ePeaceTreatyImWillingToAccept, /*bMeSurrendering*/ false);


			int iValue = GetDealValue(pDeal);
			if (!bEqualizingDeals)
			{
				SetCachedValueOfPeaceWithHuman(iValue);
				m_pPlayer->SetCachedValueOfPeaceWithHuman(iValue);
			}

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
	if(eTreaty < PEACE_TREATY_ARMISTICE)
	{
		return;
	}
	int iDuration = GC.getGame().GetDealDuration();

	PlayerTypes eLosingPlayer = bMeSurrendering ? GetPlayer()->GetID() : eOtherPlayer;
	CvPlayer* pLosingPlayer = &GET_PLAYER(eLosingPlayer);
	PlayerTypes eWinningPlayer = bMeSurrendering ? eOtherPlayer : GetPlayer()->GetID();
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

		// Create vector of the losing players' Cities so we can see which are the closest to the winner
		CvWeightedVector<int> viCityValue;

		// Loop through all of the loser's Cities, looking only at valid ones.
		for(pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
		{
			//do this from the winner's perspective!
			int iCurrentCityValue = GetCityValueForBuyer(pLoopCity, eLosingPlayer, eWinningPlayer, true);
			if (iCurrentCityValue == INT_MAX)
				continue;

			//add up total city value of the loser (before danger and damage adjustment)
			iTotalCityValue += iCurrentCityValue;

			//Remember for later
			viCityValue.push_back(pLoopCity->GetID(), iCurrentCityValue);
		}

		// Sort the vector based on distance from winner's capital
		viCityValue.SortItems();
		int iSortedCityID;

		// Determine the value of Cities to be given up
		int iCityValueToSurrender = iTotalCityValue * iPercentCitiesGiveUp / 100;
		// Loop through sorted Cities and add them to the deal if they're under the amount to give up
		// Start from the back of the list, because that's where the cheapest cities are
		for(int iSortedCityIndex = viCityValue.size() - 1; iSortedCityIndex > -1 ; iSortedCityIndex--)
		{
			iSortedCityID = viCityValue.GetElement(iSortedCityIndex);
			pLoopCity = pLosingPlayer->getCity(iSortedCityID);

			int iCurrentCityValue = GetCityValueForBuyer(pLoopCity, eLosingPlayer, eWinningPlayer, true);
			if (iCurrentCityValue == INT_MAX)
				continue;

			// City is worth less than what is left to be added to the deal, so add it
			if (iCurrentCityValue < iCityValueToSurrender && iCurrentCityValue > 0)
			{
				pDeal->AddCityTrade(eLosingPlayer, iSortedCityID);
				iCityValueToSurrender -= iCurrentCityValue;

				//Frontline cities count more than they're worth. Ideally they should satisfy the winner?
				iCityValueToSurrender -= (pLoopCity->getDamage() * 10);
				if (pLosingPlayer->GetPlotDanger(pLoopCity) > GC.getCITY_HIT_POINTS_HEALED_PER_TURN())
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

		if(eMinor == NO_PLAYER)
		{
			continue;
		}

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
			if (pMinor->GetMinorCivAI()->IsPermanentWar(GET_PLAYER(eToPlayer).getTeam()))
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

int CvDealAI::GetPotentialDemandValue(PlayerTypes eOtherPlayer, CvDeal* pDeal, int iIdealValue)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Set that this CvDeal is a demand
	pDeal->SetDemandingPlayer(GetPlayer()->GetID());
	DoAddItemsToThem(pDeal, eOtherPlayer, iIdealValue);
	int iTotalValue = GetDealValue(pDeal);

	if (pDeal->m_TradedItems.size() <= 0)
	{
		return 0;
	}

	if (iTotalValue <= 0 || iTotalValue >= INT_MAX)
	{
		return 0;
	}

	return iTotalValue;
}

/// AI making a demand of eOtherPlayer
bool CvDealAI::IsMakeDemand(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Set that this CvDeal is a demand
	pDeal->SetDemandingPlayer(GetPlayer()->GetID());

	int iIdealValue = 25 * (GetPlayer()->GetDiplomacyAI()->GetMeanness() + GetPlayer()->GetCurrentEra());
	int Value = NUM_STRENGTH_VALUES - (int)GetPlayer()->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer);
	if (Value > 0)
	{
		iIdealValue *= Value;
	}
	int iTotalValue = iIdealValue;
	DoAddItemsToThem(pDeal, eOtherPlayer, iTotalValue);

	if (pDeal->m_TradedItems.size() > 0 && iTotalValue > 0)
	{
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


	int iBestValue = 20;


	// precalculate, it's expensive
	int iCurrentNetGoldOfReceivingPlayer = m_pPlayer->GetTreasury()->CalculateBaseNetGold();

	// See if the other player has a Resource to trade
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes) iResourceLoop;

		// Only look at Luxuries
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
		{
			continue;
		}

		//one copy is enough
		if (m_pPlayer->getNumResourceAvailable(eResource, true) > 0)
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

			//Let's try to get their best resource :)
			int iItemValue = GetResourceValue(eResource, 1, GC.getGame().GetDealDuration(), false, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);
			if(iItemValue!=INT_MAX && iItemValue > iBestValue)
			{
				eLuxuryFromThem = eResource;
				iBestValue = iItemValue;
			}
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
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;
	}
	return false;
}


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

	int iBestValue = 15;

	// precalculate, it's expensive
	int iCurrentNetGoldOfReceivingPlayer = m_pPlayer->GetTreasury()->CalculateBaseNetGold();
	// See if the other player has a Resource to trade
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes) iResourceLoop;

		// Only look at Strats
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_STRATEGIC)
			continue;

		if (GetPlayer()->getNumResourceAvailable(eResource, false) > 2)
			continue;

		//don't buy more if we're already exporting it
		//todo: or is this a chance for arbitrage?
		if (GetPlayer()->getResourceExport(eResource)>0)
			continue;

		int iNum = GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eResource, false);
		if (iNum <= 0)
			continue;

		//Let's try to get their best resource :)
		int iItemValue = GetResourceValue(eResource, 2, GC.getGame().GetDealDuration(), false, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);
		if(iItemValue!=INT_MAX && iItemValue > iBestValue)
		{
			eStratFromThem = eResource;
			iBestValue = iItemValue;
		}
	}

	// Extra Strat found!
	if(eStratFromThem != NO_RESOURCE)
	{
		int iAvailable = GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eStratFromThem, false);
		int iDesired = min(4,max(1,iAvailable/2));
		iDesired += GC.getGame().getSmallFakeRandNum(3, iCurrentNetGoldOfReceivingPlayer + eStratFromThem + eOtherPlayer);
		iDesired += GetPlayer()->getResourceOverValue(eStratFromThem);
		iDesired = min(iAvailable, iDesired);

		// Can we actually complete this deal?
		if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eStratFromThem, iDesired))
		{
			return false;
		}

		// Seed the deal with the item we want
		pDeal->AddResourceTrade(eOtherPlayer, eStratFromThem, iDesired, GC.getGame().GetDealDuration());

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
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;
	}
	return false;
}


/// A good time to make an offer to get an embassy?
bool CvDealAI::MakeOfferForEmbassy(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for an embassy if we're hostile or planning war
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

	// Do we actually want an embassy with eOtherPlayer?
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
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;
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

	// Already allowing Open Borders?
	if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsAllowsOpenBordersToTeam(GetPlayer()->getTeam()))
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
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;
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
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;
}

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

		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;
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
			int iMyPrice = GetCityValueForBuyer(pTheirCity, eOtherPlayer, GetPlayer()->GetID());
			int iTheirPrice = GetCityValueForBuyer(pTheirCity, GetPlayer()->GetID(), eOtherPlayer);

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
			int iMyPrice = GetCityValueForBuyer(pMyCity, eOtherPlayer, m_pPlayer->GetID());
			int iTheirPrice = GetCityValueForBuyer(pMyCity, m_pPlayer->GetID(), eOtherPlayer);

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
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;
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
	
	// Don't ask humans' AI teammates
	if (GET_PLAYER(eOtherPlayer).IsAITeammateOfHuman())
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
	if (GetPlayer()->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer) < STRENGTH_AVERAGE && GetPlayer()->GetDiplomacyAI()->GetPlayerEconomicStrengthComparedToUs(eOtherPlayer) < STRENGTH_AVERAGE)
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
		if(GET_PLAYER(eAgainstPlayer).isMajorCiv() && GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eAgainstPlayer, /*bHideTrueFeelings*/ false) > MAJOR_CIV_APPROACH_GUARDED)
		{
			continue;
		}
		// Can we actually complete this deal?
		if (!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_THIRD_PARTY_WAR, GET_PLAYER(eAgainstPlayer).getTeam()))
			continue;

		iWarValue = GetThirdPartyWarValue(false, eOtherPlayer, GET_PLAYER(eAgainstPlayer).getTeam());
		if(iWarValue != INT_MAX && iWarValue > iBestValue)
		{
			eBestTeam = GET_PLAYER(eAgainstPlayer).getTeam();
			iBestValue = iWarValue;
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
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;
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
	if(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_FRIENDLY)
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
		if(!GET_PLAYER(eAgainstPlayer).isMinorCiv() && GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eAgainstPlayer, /*bHideTrueFeelings*/ false) < MAJOR_CIV_APPROACH_DECEPTIVE)
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
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;
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
	if(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) < MAJOR_CIV_APPROACH_AFRAID)
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
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;
}

/// This function called when human player enters diplomacy
void CvDealAI::DoTradeScreenOpened()
{
	TeamTypes eActiveTeam = GC.getGame().getActiveTeam();
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	if(GET_TEAM(GetTeam()).isAtWar(eActiveTeam))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		GetPlayer()->GetDiplomacyAI()->DoUpdatePeaceTreatyWillingness();
		m_pPlayer->SetCachedValueOfPeaceWithHuman(0);

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
			pDeal->SetDuration(GC.getGame().getGameSpeedInfo().GetDealDuration());

			// AI is surrendering
			if(ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE)
			{
				pkUIDeal->SetSurrenderingPlayer(eMyPlayer);
				pkUIDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToOffer);

				DoAddItemsToDealForPeaceTreaty(eActivePlayer, pDeal, ePeaceTreatyImWillingToOffer, /*bMeSurrendering*/ true);

				DoAddPlayersAlliesToTreaty(eActivePlayer, pDeal);


				int iValue = GetDealValue(pDeal);
				if(iValue < 0)
				{
					iValue *= 1;
				}
				SetCachedValueOfPeaceWithHuman(iValue);
				m_pPlayer->SetCachedValueOfPeaceWithHuman(iValue);

			}
			// AI is asking human to surrender
			else if(ePeaceTreatyImWillingToAccept > PEACE_TREATY_WHITE_PEACE)
			{
				pkUIDeal->SetSurrenderingPlayer(eActivePlayer);
				pkUIDeal->SetPeaceTreatyType(ePeaceTreatyImWillingToAccept);

				DoAddItemsToDealForPeaceTreaty(eActivePlayer, pDeal, ePeaceTreatyImWillingToAccept, /*bMeSurrendering*/ false);

				DoAddPlayersAlliesToTreaty(eActivePlayer, pDeal);


				int iValue = GetDealValue(pDeal);
				SetCachedValueOfPeaceWithHuman(iValue);
				m_pPlayer->SetCachedValueOfPeaceWithHuman(iValue);

			}
			pDeal->ClearItems();

			// Now add peace items to the UI deal so that it's ready for us to make an offer
			pkUIDeal->SetFromPlayer(eActivePlayer);	// The order of these is very important!
			pkUIDeal->SetToPlayer(eMyPlayer);	// The order of these is very important!
			pkUIDeal->SetDuration(GC.getGame().getGameSpeedInfo().GetDealDuration());
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

	m_pPlayer->SetCachedValueOfPeaceWithHuman(0);

	GC.GetEngineUserInterface()->SetAIRequestingConcessions(false);
	GC.GetEngineUserInterface()->SetHumanMakingDemand(false);

	if (bAIWasMakingOffer)
	{
		m_pPlayer->GetDiplomacyAI()->CancelRenewDeal(eActivePlayer, REASON_HUMAN_REJECTION);
	}
}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
// Is the human's request for help acceptable?
DemandResponseTypes CvDealAI::GetRequestForHelpResponse(CvDeal* pDeal)
{
	PlayerTypes eFromPlayer = pDeal->GetOtherPlayer(GetPlayer()->GetID()); // Playing it safe, should be OK to use pDeal->GetFromPlayer() but code was using GetActivePlayer so maybe the From field wasn't always the human (although in my testing it was fine!)
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
		// Deceptive or War AI's won't ever accept a demand request
		if(pDiploAI->GetMajorCivApproach(eFromPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_DECEPTIVE || pDiploAI->GetMajorCivApproach(eFromPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_WAR)
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
			!pDiploAI->IsGoingForSpaceshipVictory())                    // don't give up tech if we want to launch spaceship
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
						if (GetPlayer()->IsAITeammateOfHuman())
						{
							eResponse = DEMAND_RESPONSE_GIFT_REFUSE_TOO_MUCH;
						}
						else
						{
							iTechsRequested++;
						}
						break;
					}
					case TRADE_ITEM_ALLOW_EMBASSY:
					case TRADE_ITEM_OPEN_BORDERS:
					case TRADE_ITEM_MAPS:
					{
						if (GetPlayer()->IsAITeammateOfHuman())
						{
							eResponse = DEMAND_RESPONSE_GIFT_REFUSE_TOO_MUCH;
						}
						break;				// skip over embassy, open borders and maps, don't care about them.
					}
					case TRADE_ITEM_CITIES:
					case TRADE_ITEM_DEFENSIVE_PACT:
					case TRADE_ITEM_RESEARCH_AGREEMENT:
					case TRADE_ITEM_THIRD_PARTY_PEACE:
					case TRADE_ITEM_THIRD_PARTY_WAR:
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

		// Empty request
		if (iGoldRequested == 0 && iGPTRequested == 0 && iLuxuriesRequested == 0 && iStrategicsRequested == 0 && iTechsRequested == 0)
		{
			// Not a perfect fix but will prevent 0 Gold requests from triggering the help request timer for now
			eResponse = DEMAND_RESPONSE_GIFT_REFUSE_TOO_SOON;
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
int CvDealAI::GetMapValue(bool bFromMe, PlayerTypes eOtherPlayer)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Map with oneself.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 500;
	
	if (GetPlayer()->IsAITeammateOfHuman())
		return INT_MAX;
	
	CvPlayer* pSeller = bFromMe ? GetPlayer() : &GET_PLAYER(eOtherPlayer);	// Who is selling this map?
	CvPlayer* pBuyer = bFromMe ? &GET_PLAYER(eOtherPlayer) : GetPlayer();	// Who is buying this map?

	// Look at every tile on map
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);
		int iPlotValue = 0;

		if(pPlot == NULL)
			continue;

		// Seller must have plot revealed
		if(!pPlot->isRevealed(pSeller->getTeam()))
			continue;

		// Buyer can't have plot revealed
		if(pPlot->isRevealed(pBuyer->getTeam()))
			continue;

		// ignore ice plots
		if (pPlot->getFeatureType() == FEATURE_ICE)
			continue;

		// Handle terrain features
		switch(pPlot->getTerrainType())
		{
			case TERRAIN_GRASS:
			case TERRAIN_PLAINS:
			case TERRAIN_HILL:
			case TERRAIN_COAST:
				iPlotValue = 30;
				break;
			case TERRAIN_DESERT:
			case TERRAIN_TUNDRA:
				iPlotValue = 10;
				break;
			case TERRAIN_MOUNTAIN:
			case TERRAIN_SNOW:
			case TERRAIN_OCEAN:
				iPlotValue = 5;
				break;
			default:
				iPlotValue = 20;
				break;
		}

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

		int iNumTeams = GC.getGame().countMajorCivsAlive();
		int iModifier = (iNumTeams - iNumRevealed) * 100 / iNumTeams;

		iPlotValue *= max(50,iModifier);
		iPlotValue /= 100;

		iItemValue += iPlotValue;
	}

	if(bFromMe)
	{
		//prevent AI spam
		if (iItemValue <= 750 && !GET_PLAYER(eOtherPlayer).isHuman())
			return INT_MAX;

		// Approach will modify the deal
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
		{
			case MAJOR_CIV_APPROACH_WAR:
				iItemValue *= 500;
				break;
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 250;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 130;
				break;
			case MAJOR_CIV_APPROACH_DECEPTIVE:
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
				CvAssertMsg(false, "DEAL_AI: AI player has no valid Approach for Map Trading valuation.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
				iItemValue *= 100;
			break;
		}

		iItemValue /= 500;
	}
	else
	{
		if (iItemValue <= 750 && !GET_PLAYER(eOtherPlayer).isHuman())
			return INT_MAX;

		// Approach will modify the deal
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
		{
			case MAJOR_CIV_APPROACH_WAR:
				iItemValue *= 100;
				break;
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 25;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 50;
				break;
			case MAJOR_CIV_APPROACH_DECEPTIVE:
				iItemValue *= 100;
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

		iItemValue /= 500;
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
	int iItemValue = 5;
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);

	if (GetPlayer()->IsAITeammateOfHuman())
		return INT_MAX;

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
					iTechMod += 1;
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
	iItemValue = max(10, iTurnsLeft) * /*30*/ max(100, GC.getGame().getGameSpeedInfo().getTechCostPerTurnMultiplier());
	float fItemMultiplier = (float)(pow( (double) std::max(1, (iTechEra)), (double) /*0.7*/ GC.getTECH_COST_ERA_EXPONENT() ) );
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

		if(GetPlayer()->GetDiplomacyAI()->GetVictoryBlockLevel(eOtherPlayer) >= BLOCK_LEVEL_STRONG || GetPlayer()->GetDiplomacyAI()->GetVictoryDisputeLevel(eOtherPlayer) >= DISPUTE_LEVEL_STRONG)
		{
			iItemValue *= 5;
		}
		if(GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eOtherPlayer) < MAJOR_CIV_OPINION_FRIEND && !GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eOtherPlayer) )
		{
			iItemValue *= 5;
		}
#endif
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
		{
			case MAJOR_CIV_APPROACH_WAR:
				iItemValue *= 250;
				break;
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 150;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 125;
				break;
			case MAJOR_CIV_APPROACH_DECEPTIVE:
				iItemValue *= 100;
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				iItemValue *= 80;
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 90;
				break;
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

	else
	{
		switch(GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
		{
			case MAJOR_CIV_APPROACH_WAR:
				iItemValue *= 50;
				break;
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 50;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 75;
				break;
			case MAJOR_CIV_APPROACH_DECEPTIVE:
				iItemValue *= 110;
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				iItemValue *= 100;
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 110;
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				iItemValue *= 110;
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

	return max(1000,iItemValue);
}

/// How much is Vassalage worth?
int CvDealAI::GetVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bWar)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Vassalage Agreement with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	if (GetPlayer()->IsAITeammateOfHuman())
		return INT_MAX;

	CvDiplomacyAI* m_pDiploAI = GetPlayer()->GetDiplomacyAI();

	if (bFromMe)
	{
		if (!m_pDiploAI->IsVassalageAcceptable(eOtherPlayer, bWar))
		{
			return INT_MAX;
		}

		// Initial Vassalage deal value at 2000
		iItemValue = 2000;

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
				iItemValue *= 5000;
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

		switch (m_pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
		{
		case MAJOR_CIV_APPROACH_WAR:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 250;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 130;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			iItemValue *= 100;
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
		if (!GET_PLAYER(eOtherPlayer).isHuman() && !GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsVassalageAcceptable(m_pPlayer->GetID(), bWar))
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

		// The point of Vassalage is protection, if they're militarily dominant - what's the point?
		switch (m_pDiploAI->GetPlayerMilitaryStrengthComparedToUs(eOtherPlayer))
		{
		case STRENGTH_IMMENSE:
		case STRENGTH_POWERFUL:
		case STRENGTH_STRONG:
			iItemValue *= 50;
			break;
		case STRENGTH_AVERAGE:
			iItemValue *= 75;
			break;
		case STRENGTH_POOR:
			iItemValue *= 100;
			break;
		case STRENGTH_WEAK:
			iItemValue *= 125;
			break;
		case STRENGTH_PATHETIC:
			iItemValue *= 150;
			break;
		default:
			iItemValue *= 50;
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

		switch (m_pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
		{
		case MAJOR_CIV_APPROACH_WAR:
			iItemValue *= 100;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iItemValue *= 50;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iItemValue *= 60;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			iItemValue *= 100;
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

	return iItemValue;
}

//How much is ending our vassalage worth to us?
int CvDealAI::GetRevokeVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer, bool bWar)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Vassalage Agreement with oneself. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 0;

	if (GetPlayer()->IsAITeammateOfHuman())
		return INT_MAX;

	CvDiplomacyAI* m_pDiploAI = GetPlayer()->GetDiplomacyAI();

	//Can't ask if we're their vassal, or vice-versa.
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
			if(m_pDiploAI->GetWarScore(eOtherPlayer) >= -75)
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
		if(GetPlayer()->GetFractionOriginalCapitalsUnderControl() >= 50)
		{
			return INT_MAX;
		}
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
				iItemValue *= 600;
				break;
			case STRENGTH_WEAK:
				iItemValue *= 1000;
				break;
			case STRENGTH_PATHETIC:
				iItemValue *= 2000;
				break;
			default:
				iItemValue *= 2000;
				break;
		}
		iItemValue /= 100;

		switch(m_pDiploAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false))
		{
			case MAJOR_CIV_APPROACH_WAR:
				iItemValue *= 1000;
				break;
			case MAJOR_CIV_APPROACH_HOSTILE:
				iItemValue *= 500;
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				iItemValue *= 250;
				break;
			case MAJOR_CIV_APPROACH_DECEPTIVE:
				iItemValue *= 80;
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				iItemValue *= 60;
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				iItemValue *= 80;
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
				iItemValue *= 150;
				break;
			case MAJOR_CIV_OPINION_COMPETITOR:
			case MAJOR_CIV_OPINION_ENEMY:
			case MAJOR_CIV_OPINION_UNFORGIVABLE:
				iItemValue *= 500;
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

							// Did the vassal previously resurrect or liberate us?
							if (GetPlayer()->GetDiplomacyAI()->WasResurrectedBy(eVassalPlayer) || GetPlayer()->GetDiplomacyAI()->IsMasterLiberatedMeFromVassalage(eVassalPlayer))
							{
								iItemValue *= 2;
								bWorthIt = true;
								break;
							}
							
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
								case MAJOR_CIV_OPINION_FAVORABLE:
									if (GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eVassalPlayer) && GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eVassalPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_FRIENDLY)
									{
										bWorthIt = true;
									}
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
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;
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
		if(!GetPlayer()->GetPlayerTechs()->CanResearch(eTech))
			continue;

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
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}

		return bDealAcceptable && pDeal->GetNumItems() > 0;
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

	return bDealAcceptable && pDeal->GetNumItems() > 0;
}

/// A good time to make an offer for Vassalage?
bool CvDealAI::IsMakeOfferToBecomeVassal(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	if (!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eOtherPlayer, TRADE_ITEM_VASSALAGE))
	{
		return false;
	}

	if (!GetPlayer()->GetDiplomacyAI()->IsVassalageAcceptable(eOtherPlayer, false))
	{
		return false;
	}

	// Seed the deal with the item we want
	pDeal->AddVassalageTrade(GetPlayer()->GetID());

	bool bDealAcceptable = false;

	// AI evaluation
	if (!GET_PLAYER(eOtherPlayer).isHuman())
	{
		bDealAcceptable = DoEqualizeDealWithAI(pDeal, eOtherPlayer);	// Change the deal as necessary to make it work
	}
	else
	{
		bool bUselessReferenceVariable;
		bool bCantMatchOffer;
		bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;
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

						if (GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eVassalPlayer) >= MAJOR_CIV_OPINION_FRIEND)
						{
							bWorthIt = true;
						}
						else if (GetPlayer()->GetDiplomacyAI()->GetMajorCivOpinion(eVassalPlayer) >= MAJOR_CIV_OPINION_FAVORABLE)
						{
							if (GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eVassalPlayer) && GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eVassalPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_FRIENDLY)
							{
								bWorthIt = true;
							}
						}
						else if (GetPlayer()->GetDiplomacyAI()->WasResurrectedBy(eVassalPlayer) || GetPlayer()->GetDiplomacyAI()->IsMasterLiberatedMeFromVassalage(eVassalPlayer))
						{
							bWorthIt = true;
						}
						if (!GET_PLAYER(eVassalPlayer).GetDiplomacyAI()->IsEndVassalageAcceptable(eOtherPlayer))
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
			bDealAcceptable = DoEqualizeDealWithHuman(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
		}
	}

	return bDealAcceptable && pDeal->GetNumItems() > 0;
}

/// See if adding Maps to their side of the deal helps even out pDeal
void CvDealAI::DoAddMapsToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add World Map to Them, but them is us.  Please show Jon");

//	if(!bDontChangeTheirExistingItems)
	{
		if (iTotalValue < 0 || pDeal->GetDemandingPlayer() != NO_PLAYER)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if they can actually trade it to us
			if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_MAPS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_MAPS, /*bFromMe*/ false, eThem, -1, -1, -1, /*bFlag1*/false, -1);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (pDeal->GetDemandingPlayer() != NO_PLAYER || !TooMuchAdded(eThem, iTotalValue, iItemValue))
				{
					pDeal->AddMapTrade(eThem);
					return;
				}
			}
		}
	}
}

/// See if adding Maps to our side of the deal helps even out pDeal
void CvDealAI::DoAddMapsToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add World Map to Us, but them is us.  Please show Jon");

	//if(!bDontChangeMyExistingItems)
	{
		if(iTotalValue > 0)
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if we can actually trade it to them
			if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_MAPS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_MAPS, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, -1);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
				{
					pDeal->AddMapTrade(eThem);
					pDeal->AddMapTrade(eMyPlayer);
					return;
				}
			}
		}
	}
}

/// See if adding Technology to their side of the deal helps even out pDeal
void CvDealAI::DoAddTechToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	CvWeightedVector<int> viTradeValues;
	//if(!bDontChangeTheirExistingItems)
	{
		if (iTotalValue < 0 || pDeal->GetDemandingPlayer() != NO_PLAYER)
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
					iItemValue = GetTradeItemValue(TRADE_ITEM_TECHS, /*bFromMe*/ false, eThem, eTech, -1, -1, /*bFlag1*/false, -1);

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
						TechTypes eTech = (TechTypes)viTradeValues.GetElement(iRanking);
						if (eTech == NO_TECH)
							continue;
	
						if (pDeal->GetDemandingPlayer() != NO_PLAYER || !TooMuchAdded(eThem, iTotalValue, iWeight))
						{
							pDeal->AddTechTrade(eThem, eTech);
							iTotalValue = GetDealValue(pDeal);
							return;
						}	
					}
				}
			}
		}
	}
}

/// See if adding Technology to our side of the deal helps even out pDeal
void CvDealAI::DoAddTechToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	//if(!bDontChangeMyExistingItems)
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
					iItemValue = GetTradeItemValue(TRADE_ITEM_TECHS, /*bFromMe*/ true, eThem, eTech, -1, -1, /*bFlag1*/false, -1);

					if(iItemValue==INT_MAX)
					{
						return;
					}
					if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
					{
						pDeal->AddTechTrade(eMyPlayer, eTech);
						iTotalValue = GetDealValue(pDeal);
						return;
					}
				}
			}
		}
	}
}

void CvDealAI::DoAddVassalageToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	if (iTotalValue < 0)
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		int iItemValue;

		if (IsMakeOfferToBecomeVassal(eThem, pDeal))
		{
			iItemValue = GetTradeItemValue(TRADE_ITEM_VASSALAGE, /*bFromMe*/ true, eThem, -1, -1, -1, false, -1, false);

			if (iItemValue == INT_MAX)
			{
				return;
			}
			if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
			{
				pDeal->AddVassalageTrade(eMyPlayer);
				iTotalValue = GetDealValue(pDeal);
			}
		}
	}
}

void CvDealAI::DoAddVassalageToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	if (iTotalValue > 0)
	{
		int iItemValue;

		if (IsMakeOfferForVassalage(eThem, pDeal))
		{
			iItemValue = GetTradeItemValue(TRADE_ITEM_VASSALAGE, /*bFromMe*/ false, eThem, -1, -1, -1, false, -1, false);

			if (iItemValue == INT_MAX)
			{
				return;
			}
			if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
			{
				pDeal->AddVassalageTrade(eThem);
				iTotalValue = GetDealValue(pDeal);
			}
		}
	}
}

void CvDealAI::DoAddRevokeVassalageToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	if (iTotalValue > 0)
	{
		int iItemValue;

		if (IsMakeOfferForRevokeVassalage(eThem, pDeal))
		{
			iItemValue = GetTradeItemValue(TRADE_ITEM_VASSALAGE_REVOKE, /*bFromMe*/ false, eThem, -1, -1, -1, false, -1, false);

			if (iItemValue == INT_MAX)
			{
				return;
			}
			if (!TooMuchAdded(eThem, iTotalValue, iItemValue, true))
			{
				pDeal->AddRevokeVassalageTrade(eThem);
				iTotalValue = GetDealValue(pDeal);
			}
		}
	}
}
