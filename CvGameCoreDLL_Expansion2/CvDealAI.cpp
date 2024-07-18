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

///
template<typename DealAI, typename Visitor>
void CvDealAI::Serialize(DealAI& /*dealAI*/, Visitor& /*visitor*/)
{
}

/// Serialization read
void CvDealAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	CvDealAI::Serialize(*this, serialVisitor);
}

/// Serialization write
void CvDealAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	CvDealAI::Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvDealAI& dealAI)
{
	dealAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvDealAI& dealAI)
{
	dealAI.Write(stream);
	return stream;
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

/// How much are we willing to back off on what our perceived value of a deal is to make something work?
int CvDealAI::GetDealPercentLeeway(PlayerTypes eOtherPlayer, bool bInTheBlack) const
{
	int iPercent = 0;

	//for human players the opinion score is unreliable
	//so we use a fixed amount, but make it asymmetric
	if (GET_PLAYER(eOtherPlayer).isHuman())
		iPercent = bInTheBlack ? 13 : 7;
	else
	{
		switch (m_pPlayer->GetDiplomacyAI()->GetCivOpinion(eOtherPlayer))
		{
		case CIV_OPINION_ALLY:
			iPercent = 35;
			break;
		case CIV_OPINION_FRIEND:
			iPercent = 30;
			break;
		case CIV_OPINION_FAVORABLE:
			iPercent = 25;
			break;
		case CIV_OPINION_NEUTRAL:
			iPercent = 20;
			break;
		case CIV_OPINION_COMPETITOR:
		case CIV_OPINION_ENEMY:
		case CIV_OPINION_UNFORGIVABLE:
			iPercent = 15;
			break;
		default:
			iPercent = 15;
			break;
		}
	}

	//want better deals if we're having economic problems
	if (!bInTheBlack && m_pPlayer->GetEconomicAI()->IsUsingStrategy((EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY")))
		iPercent /= 2;

	return iPercent;
}

bool CvDealAI::WithinAcceptableRange(PlayerTypes ePlayer, int iMaxValue, int iNetValue) const
{
	int iLeewayPercent = GetDealPercentLeeway(ePlayer,iNetValue>0);
	// Make trades at low max value easier
	int iMaxDeviation = iMaxValue * iLeewayPercent + min(100, iMaxValue) * 15;
	iMaxDeviation /= 100;

	// a deal value of less than or equal to half the value of 1 GPT should always be acceptable, to avoid deals that can't be equalized with GPT
	int iGPTValue = GetOneGPTValue(m_pPlayer->IsAtWarWith(ePlayer));

	//put some sanity checks
	return abs(iNetValue) <= min(max(iMaxDeviation,iGPTValue/2),500);
}

bool CvDealAI::BothSidesIncluded(CvDeal* pDeal)
{
	if (pDeal->GetDemandingPlayer() != NO_PLAYER || pDeal->GetRequestingPlayer() != NO_PLAYER)
		return true;

	if (pDeal->m_bIsGift)
		return true;

	//humans sometimes give nice things!
	if (GET_PLAYER(pDeal->m_eFromPlayer).isHuman() && pDeal->GetNumItems() > 0 && GetDealValue(pDeal) >= 0)
		return true;

	return (pDeal->GetFromPlayerValue() > 0 && pDeal->GetToPlayerValue() > 0);
}

bool CvDealAI::TooMuchAdded(PlayerTypes ePlayer, int iMaxValue, int iNetValue, int iItemValue, bool bFromUs)
{
	//good now? then let's do it! (need to respect the negative if we're adding to a negative deal)
	int iNewValue = bFromUs ? (iNetValue - iItemValue) : iNetValue + iItemValue;
	if (WithinAcceptableRange(ePlayer, iMaxValue, iNewValue))
		return false;
	
	//still not good enough, but trending in the right direction? Carry on.
	return bFromUs ? (iNewValue <= 0) : (iNewValue >= 0);
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

	bool bCantMatchOffer = false;

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
			CvInterfacePtr<ICvDeal1> pDllDeal = GC.WrapDealPointer(&kDeal);
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
void CvDealAI::DoAcceptedDeal(PlayerTypes eFromPlayer, const CvDeal& kDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering)
{
	if (GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		GC.getGame().GetGameDeals().FinalizeMPDeal(kDeal, true);
	}
	else
	{
		GC.getGame().GetGameDeals().AddProposedDeal(kDeal);
		GC.getGame().GetGameDeals().FinalizeDeal(eFromPlayer, GetPlayer()->GetID(), true);
	}

	if (GET_PLAYER(eFromPlayer).isHuman())
	{
		// If this was a peace deal, apply a recent assist bonus for demanding less than the maximum value.
		int iCachedPeaceValue = GetCachedValueOfPeaceWithHuman();
		bool bGenerousPeaceTreaty = false;
		if (iCachedPeaceValue != 0)
		{
			iCachedPeaceValue *= -1;
			if (iDealValueToMe < 0)
				iDealValueToMe = 0;

			if (iCachedPeaceValue > 0 && iDealValueToMe < iCachedPeaceValue)
			{
				int iDifference = iCachedPeaceValue - iDealValueToMe;
				int iPercentage = (iDifference * 100) / iCachedPeaceValue;
				int iRecentAssistBonus = (m_pPlayer->GetDiplomacyAI()->GetMaxRecentAssistValue() * 2 * iPercentage) / 100;
				m_pPlayer->GetDiplomacyAI()->ChangeRecentAssistValue(eFromPlayer, -iRecentAssistBonus);

				// If half or less of max value, mark it as generous for the text selection below
				if (iPercentage >= 50)
				{
					bGenerousPeaceTreaty = true;
				}
			}

			// Reset cached values
			SetCachedValueOfPeaceWithHuman(0);
			m_pPlayer->SetCachedValueOfPeaceWithHuman(0);
			iDealValueToMe = 0;
			iValueImOffering = 0;
			iValueTheyreOffering = 0;
		}

		if (GC.getGame().getActivePlayer() == eFromPlayer)
		{
			// This signals Lua to do some interface cleanup, we only want to do this on the local machine.
			gDLL->DoClearDiplomacyTradeTable();
			GC.GetEngineUserInterface()->SetOfferTradeRepeatCount(0);

			// We made a demand and they gave in
			if (kDeal.GetDemandingPlayer() == GetPlayer()->GetID())
			{
				const char* szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_AI_DEMAND);
				gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
				return;
			}
			// We made a request and they agreed
			if (kDeal.GetRequestingPlayer() == GetPlayer()->GetID())
			{
				const char* szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_THANKFUL);
				gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
				if (iDealValueToMe > 0)
					GetPlayer()->GetDiplomacyAI()->ChangeRecentAssistValue(eFromPlayer, iDealValueToMe / -2);

				return;
			}
			// We're offering help to a player
			if (GetPlayer()->GetDiplomacyAI()->IsOfferingGift(eFromPlayer) || GetPlayer()->GetDiplomacyAI()->IsOfferedGift(eFromPlayer))
			{
				//End the gift exchange after this.
				GetPlayer()->GetDiplomacyAI()->SetOfferingGift(eFromPlayer, false);
				GetPlayer()->GetDiplomacyAI()->SetOfferedGift(eFromPlayer, true);
				const char* szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
			}
			else
			{
				// If this was a peace deal then use that animation instead
				LeaderheadAnimationTypes eAnimation = kDeal.GetPeaceTreatyType() != NO_PEACE_TREATY_TYPE ? LEADERHEAD_ANIM_PEACEFUL : LEADERHEAD_ANIM_YES;

				// A deal is generous if we're getting a lot overall, OR a lot more than we're giving up
				if (bGenerousPeaceTreaty || iDealValueToMe >= 100 || iValueTheyreOffering >= (iValueImOffering * 2))
				{
					const char* szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_GENEROUS);
					gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, eAnimation);
					GetPlayer()->GetDiplomacyAI()->ChangeRecentTradeValue(eFromPlayer, iDealValueToMe / 5);
				}
				// Acceptable deal for us
				else
				{
					const char* szText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE);
					gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, eAnimation);
					if (iDealValueToMe > 0)
						GetPlayer()->GetDiplomacyAI()->ChangeRecentTradeValue(eFromPlayer, iDealValueToMe / 10);
				}
			}
		}
		if (GC.getGame().getActivePlayer() == eFromPlayer || GC.getGame().getActivePlayer() == GetPlayer()->GetID())
		{
			GC.GetEngineUserInterface()->makeInterfaceDirty();
		}
	}
}

/// Human making a demand of the AI
DemandResponseTypes CvDealAI::DoHumanDemand(CvDeal* pDeal)
{
	PlayerTypes eFromPlayer = pDeal->GetOtherPlayer(GetPlayer()->GetID()); // Playing it safe, should be OK to use pDeal->GetFromPlayer() but code was using GetActivePlayer so maybe the From field wasn't always the human (although in my testing it was fine!)
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	DemandResponseTypes eResponse = NO_DEMAND_RESPONSE_TYPE;

	// If we're friends with the demanding player, it's actually a Request for Help. Let's evaluate in a separate function and come back here
	// Have to send AI response through the network  - it affects AI behavior
	if (GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eFromPlayer))
	{
		eResponse = GetRequestForHelpResponse(pDeal);
		GC.getGame().DoFromUIDiploEvent(FROM_UI_DIPLO_EVENT_HUMAN_REQUEST, eMyPlayer, /*iData1*/ eResponse, -1);
	}
	else
	{
		eResponse = GetDemandResponse(pDeal);
		GC.getGame().DoFromUIDiploEvent(FROM_UI_DIPLO_EVENT_HUMAN_DEMAND, eMyPlayer, /*iData1*/ eResponse, -1);
	}

	// Demand agreed to
	if (eResponse == DEMAND_RESPONSE_ACCEPT || eResponse == DEMAND_RESPONSE_GIFT_ACCEPT)
	{
		CvDeal kDeal = *pDeal;
		//gDLL->sendNetDealAccepted(eFromPlayer, GetPlayer()->GetID(), kDeal, -1, -1, -1);
		GC.GetEngineUserInterface()->SetDealInTransit(true);

		CvInterfacePtr<ICvDeal1> pDllDeal = GC.WrapDealPointer(&kDeal);
		gDLL->sendNetDemandAccepted(eFromPlayer, eMyPlayer, pDllDeal.get());
	}

	return eResponse;
}

/// What is the AI's response to a demand?
DemandResponseTypes CvDealAI::GetDemandResponse(CvDeal* pDeal)
{
	PlayerTypes eFromPlayer = pDeal->GetOtherPlayer(GetPlayer()->GetID());
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	TeamTypes eFromTeam = GET_PLAYER(eFromPlayer).getTeam();
	TeamTypes eMyTeam = GetPlayer()->getTeam();

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();
	CivApproachTypes eApproach = pDiploAI->GetCivApproach(eFromPlayer);
	StrengthTypes eMilitaryStrength = pDiploAI->GetMilitaryStrengthComparedToUs(eFromPlayer);
	AggressivePostureTypes eMilitaryPosture = pDiploAI->GetMilitaryAggressivePosture(eFromPlayer);
	PlayerProximityTypes eProximity = GET_PLAYER(eFromPlayer).GetProximityToPlayer(eMyPlayer);
	TeamTypes eOurMaster = GET_TEAM(eMyTeam).GetMaster();
	// if they're close, there's always some danger, even if we don't see any units
	if (eProximity > PLAYER_PROXIMITY_FAR && eMilitaryPosture == AGGRESSIVE_POSTURE_NONE)
		eMilitaryPosture = AGGRESSIVE_POSTURE_LOW;

	// Too soon for another demand? Never give in.
	if (pDiploAI->IsDemandTooSoon(eFromPlayer))
		return DEMAND_RESPONSE_REFUSE_TOO_SOON;

	// Untrustworthy? Never give in.
	if (pDiploAI->IsUntrustworthy(eFromPlayer))
		return DEMAND_RESPONSE_REFUSE_HOSTILE;

	// If we're a vassal, check if we can benefit from our master's protection.
	if (eOurMaster != NO_TEAM && eOurMaster != eFromTeam)
	{
		vector<PlayerTypes> vMasterTeam = GET_TEAM(eOurMaster).getPlayers();
		for (size_t i=0; i < vMasterTeam.size(); i++)
		{
			if (!GET_PLAYER(vMasterTeam[i]).isAlive() || !GET_PLAYER(vMasterTeam[i]).isMajorCiv() || GET_PLAYER(vMasterTeam[i]).getNumCities() == 0)
				continue;

			// This master is not strong enough to protect us.
			if (GET_PLAYER(vMasterTeam[i]).GetDiplomacyAI()->GetRawMilitaryStrengthComparedToUs(eFromPlayer) > STRENGTH_AVERAGE)
				continue;

			// Is our master failing to protect us? Then their protection isn't worth much.
			if (pDiploAI->GetVassalFailedProtectScore(vMasterTeam[i]) > 0 && pDiploAI->GetVassalFailedProtectScore(vMasterTeam[i]) > (pDiploAI->GetVassalProtectScore(vMasterTeam[i]) * -1))
				continue;

			// Is our master neighbors with them?
			if (GET_PLAYER(vMasterTeam[i]).GetProximityToPlayer(eFromPlayer) == PLAYER_PROXIMITY_NEIGHBORS)
				return DEMAND_RESPONSE_REFUSE_PROTECTED_BY_MASTER;

			// Master is at least as close to us as they are? Refuse.
			if (GET_PLAYER(vMasterTeam[i]).GetProximityToPlayer(eMyPlayer) >= eProximity)
				return DEMAND_RESPONSE_REFUSE_PROTECTED_BY_MASTER;
		}
	}

	// Are they not able to declare war on us? Never give in...but our response will vary based on how the latest war went.
	if (!GET_TEAM(eFromTeam).canDeclareWar(eMyTeam, eFromPlayer))
	{
		if (GET_TEAM(eFromTeam).IsVassalOfSomeone())
			return DEMAND_RESPONSE_REFUSE_WEAK;

		if (GET_TEAM(eFromTeam).isForcePeace(eMyTeam) && GET_TEAM(eFromTeam).IsWonLatestWar(eMyTeam))
			return DEMAND_RESPONSE_REFUSE_TOO_SOON_SINCE_PEACE;

		return DEMAND_RESPONSE_REFUSE_WEAK;
	}
	// They are weak? Never give in.
	else if (eMilitaryStrength <= STRENGTH_WEAK)
		return DEMAND_RESPONSE_REFUSE_WEAK;
	// They are very far away and have no units near us (from what we can tell)? Never give in.
	else if (eProximity <= PLAYER_PROXIMITY_FAR && eMilitaryPosture == AGGRESSIVE_POSTURE_NONE)
		return DEMAND_RESPONSE_REFUSE_WEAK;
	// Are we planning to war / demand from this guy ourselves? Never give in...but whether we give the weak or hostile response depends on their strength.
	else if (eApproach == CIV_APPROACH_WAR || pDiploAI->GetDemandTargetPlayer() == eFromPlayer)
	{
		if (eMilitaryStrength <= STRENGTH_AVERAGE)
			return DEMAND_RESPONSE_REFUSE_WEAK;

		return DEMAND_RESPONSE_REFUSE_HOSTILE;
	}
	// 10 Boldness? Never give in.
	// Hostility check is run after the weakness check, because the AI doesn't benefit from prematurely disclosing that they wouldn't be willing to accept regardless.
	else if (pDiploAI->GetBoldness() == 10)
		return DEMAND_RESPONSE_REFUSE_HOSTILE;
	// Deal valued as impossible? Never give in. This also catches any untradeable items.
	else if (GetDealValue(pDeal) == INT_MAX)
		return DEMAND_RESPONSE_REFUSE_TOO_MUCH;
	// Are there items in the deal that can't be demanded?
	else
	{
		TradedItemList::iterator it;
		for (it = pDeal->m_TradedItems.begin(); it != pDeal->m_TradedItems.end(); ++it)
		{
			// Illegal demand item
			if (it->IsTwoSided())
				return DEMAND_RESPONSE_REFUSE_TOO_MUCH;

			// Item from this AI
			if (it->m_eFromPlayer == eMyPlayer)
			{
				// Always refuse any of the following
				switch (it->m_eItemType)
				{
				case TRADE_ITEM_GOLD: // Don't give away Gold and similar items because nothing stops the aggressor from declaring war immediately after.
				case TRADE_ITEM_MAPS:
				case TRADE_ITEM_CITIES:
				case TRADE_ITEM_TECHS:
				case TRADE_ITEM_VASSALAGE_REVOKE: // Vassalage is permitted but only if the AI was okay with becoming a voluntary vassal anyway. AIs strong enough to have vassals shouldn't be willing to give them away.
				case TRADE_ITEM_THIRD_PARTY_WAR: // If third party war is being demanded, we're being tricked.
					return DEMAND_RESPONSE_REFUSE_TOO_MUCH;
				default:
					break;
				}
			}
		}
	}

	// calculate the value we're willing to give up
	int iValueWillingToGiveUp = /*200*/ GD_INT_GET(DEMAND_LIMIT_MAX_VALUE);
	// scaling with game progress
	int iGameProgressFactor = (GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown() * 100) / GC.getNumTechInfos();
	iValueWillingToGiveUp *= 100 + iGameProgressFactor * /*20*/ GD_INT_GET(DEMAND_LIMIT_GAMEPROGRESS_SCALING);
	iValueWillingToGiveUp /= 100;

	// a fraction of the maximum value is given based on military strength, aggressive posture and boldness
	iValueWillingToGiveUp *= (int)eMilitaryStrength * (int)eMilitaryPosture * (10 - pDiploAI->GetBoldness() + 1);
	iValueWillingToGiveUp /= ((int)NUM_STRENGTH_VALUES - 1) * ((int)NUM_AGGRESSIVE_POSTURE_TYPES - 1) * 10;

	// If we're their vassal or afraid of them, we give away more
	TeamTypes eMasterTeam = GET_TEAM(GET_PLAYER(eMyPlayer).getTeam()).GetMaster();
	if (eMasterTeam == GET_PLAYER(eFromPlayer).getTeam() || eApproach == CIV_APPROACH_AFRAID)
	{
		iValueWillingToGiveUp *= 150;
		iValueWillingToGiveUp /= 100;
	}

	// compare the demanded value to the value we're willing to give up
	return ((-pDeal->m_iFromPlayerValue) <= iValueWillingToGiveUp) ? DEMAND_RESPONSE_ACCEPT : DEMAND_RESPONSE_REFUSE_TOO_MUCH;
}

/// Demand has been agreed to
void CvDealAI::DoAcceptedDemand(PlayerTypes eFromPlayer, const CvDeal& kDeal)
{
	CvGame& kGame = GC.getGame();
	CvGameDeals& kGameDeals = kGame.GetGameDeals();
	const PlayerTypes eActivePlayer = kGame.getActivePlayer();
	const PlayerTypes ePlayer = GetPlayer()->GetID();

	if (GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		kGameDeals.FinalizeMPDeal(kDeal, true);
	}
	else
	{
		kGameDeals.AddProposedDeal(kDeal);
		kGameDeals.FinalizeDeal(eFromPlayer, ePlayer, true);
	}

	if(eActivePlayer == eFromPlayer || eActivePlayer == ePlayer)
	{
		GC.GetEngineUserInterface()->makeInterfaceDirty();
	}
}
/// Will this AI accept pDeal? Handles deal from both human and AI players

bool CvDealAI::IsDealWithHumanAcceptable(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValueToMe, bool* bCantMatchOffer, bool bFirstPass)
{
	// Important: check invalid return value!
	if (iTotalValueToMe==INT_MAX || iTotalValueToMe==(INT_MAX * -1))
		return false;

	*bCantMatchOffer = false;

	if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		if (pDeal->GetRequestingPlayer() == GetPlayer()->GetID() && (iTotalValueToMe > 0))
		{
			return true;
		}
	}

	// We're offering help to a player
	if (GetPlayer()->GetDiplomacyAI()->IsOfferingGift(eOtherPlayer))
	{
		return true;
	}

	//special case for peacetime return of single cities...
	if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer) && pDeal->ContainsItemType(TRADE_ITEM_CITIES, eOtherPlayer))
	{
		int iCount = 0;
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(pDeal->GetFromPlayer()).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(pDeal->GetFromPlayer()).nextCity(&iCityLoop))
		{
			if (pDeal->IsCityTrade(eOtherPlayer, pLoopCity->getX(), pLoopCity->getY()))
			{
				if (pLoopCity->getOriginalOwner() == pDeal->GetToPlayer())
				{
					iTotalValueToMe = GetDealValue(pDeal);
					iCount++;
				}
			}
		}

		//only do this if there is exactly one city
		return iCount == 1;
	}

	// We're surrendering
	if (pDeal->GetSurrenderingPlayer() == GetPlayer()->GetID())
	{
		int iMaxPeace = (GetCachedValueOfPeaceWithHuman() * -1);
		if (iTotalValueToMe < 0)
			iTotalValueToMe *= -1;

		if (iTotalValueToMe <= iMaxPeace)
			return true;
	}

	// Peace deal where we're not surrendering, value must equal cached value
	else if (pDeal->IsPeaceTreatyTrade(eOtherPlayer))
	{
		int iMaxPeace = GetCachedValueOfPeaceWithHuman();
		if (iTotalValueToMe >= iMaxPeace)
			return true;
	}

	// Now do the valuation
	iTotalValueToMe = GetDealValue(pDeal);

	// If we've gotten the deal to a point where we're happy, offer it up
	if (!pDeal->IsPeaceTreatyTrade(eOtherPlayer) && !bFirstPass)
	{
		//Is this a good deal for both parties?
		if (WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValueToMe) && BothSidesIncluded(pDeal))
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

/// Try to even out the value on both sides.
bool CvDealAI::DoEqualizeDeal(CvDeal* pDeal, PlayerTypes eOtherPlayer, bool& bDealGoodToBeginWith, bool& bCantMatchOffer, bool bHumanRequestedEqualization)
{
	bool bMakeOffer = false;
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	DEBUG_VARIABLE(eMyPlayer);

	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);
	CvAssertMsg(eMyPlayer != eOtherPlayer, "DEAL_AI: Trying to equalize human deal, but both players are the same.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

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
		int iTotalValue = GetDealValue(pDeal);

		// impossible items in the deal?
		if (iTotalValue == INT_MAX)
			return false;

		if (GET_PLAYER(eOtherPlayer).isHuman())
		{
			bMakeOffer = IsDealWithHumanAcceptable(pDeal, eOtherPlayer, /*Passed by reference*/ iTotalValue, &bCantMatchOffer, true);
			bDealGoodToBeginWith = bMakeOffer;
		}
		else
		{
			bMakeOffer = BothSidesIncluded(pDeal) && WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValue);
		}

		if(!bMakeOffer)
		{
			/////////////////////////////
			// Try to modify only the side of the deal on which something still needs to be added
			/////////////////////////////

			if (iTotalValue > 0)
			{
				// first try to equalize the deal by adding gold
				if (!pDeal->IsGoldOnlyTrade())
				{
					DoAddItemsToUs(pDeal, eOtherPlayer, iTotalValue, 0, /*bGoldOnly*/ true, bHumanRequestedEqualization);
					bMakeOffer = WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValue);
					// adding gold didn't work? reset the deal to starting terms and try to add items
					if (!bMakeOffer)
					{
						pDeal->RemoveAllPossibleItems();
						iTotalValue = GetDealValue(pDeal);
						DoAddItemsToUs(pDeal, eOtherPlayer, iTotalValue, 0, false, bHumanRequestedEqualization);
						bMakeOffer = WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValue);
					}
				}
				else
				{
					// try to add items
					DoAddItemsToUs(pDeal, eOtherPlayer, iTotalValue, 0, false, bHumanRequestedEqualization);
					bMakeOffer = WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValue);
				}
			}
			else if (iTotalValue < 0)
			{
				// first try to equalize the deal by adding gold
				if (!pDeal->IsGoldOnlyTrade())
				{
					DoAddItemsToThem(pDeal, eOtherPlayer, iTotalValue, 0, /*bGoldOnly*/ true, bHumanRequestedEqualization);
					bMakeOffer = WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValue);
					// adding gold didn't work? reset the deal to starting terms and try to add items
					if (!bMakeOffer)
					{
						pDeal->RemoveAllPossibleItems();
						iTotalValue = GetDealValue(pDeal);
						DoAddItemsToThem(pDeal, eOtherPlayer, iTotalValue, 0, false, bHumanRequestedEqualization);
						bMakeOffer = WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValue);
					}
				}
				else
				{
					// try to add items
					DoAddItemsToThem(pDeal, eOtherPlayer, iTotalValue, 0, false, bHumanRequestedEqualization);
					bMakeOffer = WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValue);
				}
			}
			// neither adding gold nor adding items worked? reset the deal. in the next step, we'll try to add items to both sides
			if (!bMakeOffer)
			{
				pDeal->RemoveAllPossibleItems();
				iTotalValue = GetDealValue(pDeal);
			}

			/////////////////////////////
			// See if there are items we can add or remove from either side to balance out the deal if it's not already even
			/////////////////////////////
			int iLoops = 0;
			while (iLoops < 5 && !WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValue))
			{
				int iPrevValue = GetDealValue(pDeal);

				if (iTotalValue > 0)
				{
					DoAddItemsToUs(pDeal, eOtherPlayer, iTotalValue, -iTotalValue/2, false, bHumanRequestedEqualization);
				}
				else if (iTotalValue < 0)
				{
					DoAddItemsToThem(pDeal, eOtherPlayer, iTotalValue, -iTotalValue / 2, false, bHumanRequestedEqualization);
				}

				//bail if we're stuck
				if (iTotalValue == iPrevValue)
					break;

				iLoops++;
			}

			if (GET_PLAYER(eOtherPlayer).isHuman())
			{
				bMakeOffer = IsDealWithHumanAcceptable(pDeal, eOtherPlayer, /*Passed by reference*/ iTotalValue, /*passed by reference*/&bCantMatchOffer, false);
				if (bCantMatchOffer)
				{
					GetPlayer()->GetDiplomacyAI()->SetCantMatchDeal(eOtherPlayer, true);
				}
			}
			else
			{
				bMakeOffer = BothSidesIncluded(pDeal) && WithinAcceptableRange(eOtherPlayer, pDeal->GetMaxValue(), iTotalValue);
			}
		}
	}

	return bMakeOffer;
}

/// What do we think of a Deal?
int CvDealAI::GetDealValue(CvDeal* pDeal)
{
	int iDealValue = 0;
	if (pDeal->GetDuration() == -1)
		pDeal->SetDuration(GC.getGame().getGameSpeedInfo().GetDealDuration());

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	int iItemValue = -1;

	bool bFromMe = false; // will be set for individual trade items later

	PlayerTypes eOtherPlayer;
	eOtherPlayer = pDeal->m_eFromPlayer == eMyPlayer ? pDeal->m_eToPlayer : pDeal->m_eFromPlayer;

	// We're offering help to a player
	if (eOtherPlayer != NO_PLAYER && GetPlayer()->GetDiplomacyAI()->IsOfferingGift(eOtherPlayer))
	{
		return 1;
	}
	pDeal->SetFromPlayerValue(0);
	pDeal->SetToPlayerValue(0);

	TradedItemList::iterator it;
	for (it = pDeal->m_TradedItems.begin(); it != pDeal->m_TradedItems.end(); ++it)
	{
		// peace deals are always evaluated from the winner's perspective
		bFromMe = (it->m_eFromPlayer == eMyPlayer);
		// Multiplier is -1 if we're giving something away, 1 if we're receiving something
		int iValueMultiplier = bFromMe ? -1 : 1;

		if (bFromMe)
		{
			if (!pDeal->IsPossibleToTradeItem(eMyPlayer, eOtherPlayer, it->m_eItemType, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, true))
			{
				iItemValue = INT_MAX;
			}
		}
		else
		{
			if (!pDeal->IsPossibleToTradeItem(eOtherPlayer, eMyPlayer, it->m_eItemType, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, true))
			{
				iItemValue = INT_MAX;
			}
		}

		if (iItemValue != INT_MAX)
		{
			iItemValue = GetTradeItemValue(it->m_eItemType, bFromMe, eOtherPlayer, it->m_iData1, it->m_iData2, it->m_iData3, it->m_bFlag1, it->m_iDuration, false);
			it->m_iValue = iItemValue;
		}


		//if the item is invalid, return invalid value for the whole deal
		if (iItemValue == INT_MAX)
		{
			pDeal->SetFromPlayerValue(INT_MAX);
			pDeal->SetToPlayerValue(INT_MAX);
			return iItemValue;
		}

		iItemValue *= iValueMultiplier;
		iDealValue += iItemValue;

		if (bFromMe)
			pDeal->ChangeFromPlayerValue(iItemValue);
		else
			pDeal->ChangeToPlayerValue(iItemValue);
	}

	return iDealValue;
}


/// What is the value of 1 GPT? This value is used as the minimum value for certain deal items
int CvDealAI::GetOneGPTValue(bool bPeaceDeal) const
{
	int iGPTValue = bPeaceDeal ? GC.getGame().getGameSpeedInfo().getPeaceDealDuration() : GC.getGame().getGameSpeedInfo().GetDealDuration();
	//let's assume an interest rate of 0.5% per turn, no compounding
	int iInterestPercent = 5 * /*5*/ GD_INT_GET(EACH_GOLD_PER_TURN_VALUE_PERCENT);
	//subtract interest. 100 gold now is better than 100 gold in the future
	iGPTValue *= 100;
	iGPTValue /= max(1, 100 + iInterestPercent);
	return(max(1, iGPTValue));
}

/// What is a particular item worth?
int CvDealAI::GetTradeItemValue(TradeableItems eItem, bool bFromMe, PlayerTypes eOtherPlayer, int iData1, int iData2, int iData3, bool bFlag1, int iDuration, bool bIsAIOffer, bool bEqualize)
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
	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	SDealItemValueParams currentCall(eItem, bFromMe, eOtherPlayer, iData1, iData2, iData3, bFlag1, iDuration, bEqualize);
	if (m_dealItemValues.find(currentCall) != m_dealItemValues.end())
		return m_dealItemValues[currentCall];


	int iItemValue = 0;
	// if bEqualize is false, our valuation of the deal item is calculated (only used internally and to help the AI decide which items to make offers for)
	// The values that are calculated with bEqualize = true are the ones that are shown in the trade UI and that are used to determine deal values
	// if bEqualize is true and the deal is not a peace deal, the average of our valuation and the other player's valuation (perceived valuation if human) is taken
	// if bEqualize is true and the deal is a peace deal (no white peace), the valuation of the winning player is taken
	
	// bEqualize isn't applied to gold or gold per turn, as they have the same value for all players. it's also not applied to peace treaties and vassalage
	if (!bEqualize || eItem == TRADE_ITEM_GOLD || eItem == TRADE_ITEM_GOLD_PER_TURN || eItem == TRADE_ITEM_PEACE_TREATY ||eItem == TRADE_ITEM_VASSALAGE ||eItem == TRADE_ITEM_ALLOW_EMBASSY)
	{
		// not for sale modmod
		if (MOD_NOT_FOR_SALE && GetPlayer()->isHuman() && bFromMe)
		{
			if ((eItem == TRADE_ITEM_RESOURCES && GetPlayer()->IsResourceNotForSale((ResourceTypes)iData1)) ||
				(eItem == TRADE_ITEM_ALLOW_EMBASSY && GetPlayer()->IsRefuseEmbassyTrade()) ||
				(eItem == TRADE_ITEM_OPEN_BORDERS && GetPlayer()->IsRefuseOpenBordersTrade()) ||
				(eItem == TRADE_ITEM_RESEARCH_AGREEMENT && GetPlayer()->IsRefuseResearchAgreementTrade()) ||
				(eItem == TRADE_ITEM_DEFENSIVE_PACT && GetPlayer()->IsRefuseDefensivePactTrade()) ||
				(eItem == TRADE_ITEM_THIRD_PARTY_PEACE && GetPlayer()->IsRefuseBrokeredPeaceTrade()) ||
				(eItem == TRADE_ITEM_THIRD_PARTY_WAR && GetPlayer()->IsRefuseBrokeredWarTrade()))
			{
				iItemValue = INT_MAX;
			}
		}
		if (iItemValue != INT_MAX)
		{
			//actual computation

			if (eItem == TRADE_ITEM_GOLD)
				iItemValue = GetGoldForForValueExchange(/*Gold Amount*/ iData1, /*bNumGoldFromValue*/ false);
			else if (eItem == TRADE_ITEM_GOLD_PER_TURN)
				iItemValue = GetGPTForForValueExchange(/*Gold Per Turn Amount*/ iData1, /*bNumGPTFromValue*/ false, iDuration, bFromMe, eOtherPlayer);
			else if (eItem == TRADE_ITEM_RESOURCES)
			{
				// precalculate, it's expensive
				int iCurrentNetGoldOfReceivingPlayer = bFromMe ? GET_PLAYER(eOtherPlayer).GetTreasury()->CalculateBaseNetGold() : m_pPlayer->GetTreasury()->CalculateBaseNetGold();
				iItemValue = GetResourceValue(/*ResourceType*/ (ResourceTypes)iData1, /*Quantity*/ iData2, iDuration, bFromMe, eOtherPlayer, iCurrentNetGoldOfReceivingPlayer);
			}
			else if (eItem == TRADE_ITEM_CITIES)
			{
				CvCity* pCity = GC.getMap().plot(/*iX*/ iData1, /*iY*/ iData2)->getPlotCity();
				PlayerTypes eBuyer = bFromMe ? eOtherPlayer : m_pPlayer->GetID();
				iItemValue = GetCityValueForDeal(pCity, eBuyer);
			}
			else if (eItem == TRADE_ITEM_ALLOW_EMBASSY)
				iItemValue = GetEmbassyValue(bFromMe, eOtherPlayer);
			else if (eItem == TRADE_ITEM_OPEN_BORDERS)
				iItemValue = GetOpenBordersValue(bFromMe, eOtherPlayer);
			else if (eItem == TRADE_ITEM_DEFENSIVE_PACT)
				iItemValue = GetDefensivePactValue(bFromMe, eOtherPlayer);
			else if (eItem == TRADE_ITEM_RESEARCH_AGREEMENT)
				iItemValue = GetResearchAgreementValue(bFromMe, eOtherPlayer);
			else if (eItem == TRADE_ITEM_PEACE_TREATY)
				iItemValue = GetPeaceTreatyValue(eOtherPlayer);
			else if (eItem == TRADE_ITEM_THIRD_PARTY_PEACE)
				iItemValue = GetThirdPartyPeaceValue(bFromMe, eOtherPlayer, /*eWithTeam*/ (TeamTypes)iData1);
			else if (eItem == TRADE_ITEM_THIRD_PARTY_WAR)
				iItemValue = GetThirdPartyWarValue(bFromMe, eOtherPlayer, /*eWithTeam*/ (TeamTypes)iData1);
			else if (eItem == TRADE_ITEM_VOTE_COMMITMENT)
				iItemValue = GetVoteCommitmentValue(bFromMe, eOtherPlayer, iData1, iData2, iData3, bFlag1);
			else if (eItem == TRADE_ITEM_MAPS)
				iItemValue = GetMapValue(bFromMe, eOtherPlayer);
			else if (eItem == TRADE_ITEM_TECHS)
				iItemValue = GetTechValue(/*TechType*/ (TechTypes)iData1, bFromMe, eOtherPlayer);
			else if (eItem == TRADE_ITEM_VASSALAGE)
				iItemValue = GetVassalageValue(bFromMe, eOtherPlayer);
			else if (eItem == TRADE_ITEM_VASSALAGE_REVOKE)
				iItemValue = GetRevokeVassalageValue(bFromMe, eOtherPlayer, GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()));

			CvAssertMsg(iItemValue >= 0, "DEAL_AI: Trade Item value is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		}
	}
	else
	{
		// peace deals: deal items are evaluated from the winner's perspective
		PlayerTypes eWinner = NO_PLAYER;
		if (GetPlayer()->IsAtWarWith(eOtherPlayer))
		{
			if (GET_PLAYER(eOtherPlayer).isHuman())
			{
				PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eOtherPlayer);
				PeaceTreatyTypes ePeaceTreatyImWillingToAccept = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToAccept(eOtherPlayer);
				// We are surrendering
				if (ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE)
				{
					eWinner = eOtherPlayer;
				}
				// We are asking the human player to surrender
				else if (ePeaceTreatyImWillingToAccept > PEACE_TREATY_WHITE_PEACE)
				{
					eWinner = eMyPlayer;
				}
			}
			else
			{
				// AI-AI peace
				PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eOtherPlayer);
				PeaceTreatyTypes ePeaceTreatyTheyreWillingToOffer = GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetTreatyWillingToAccept(eMyPlayer);
				if (ePeaceTreatyImWillingToOffer > PEACE_TREATY_WHITE_PEACE && ePeaceTreatyTheyreWillingToOffer > PEACE_TREATY_WHITE_PEACE)
				{
					if (ePeaceTreatyImWillingToOffer > ePeaceTreatyTheyreWillingToOffer)
						eWinner = eOtherPlayer;
					else if (ePeaceTreatyImWillingToOffer < ePeaceTreatyTheyreWillingToOffer)
						eWinner = eMyPlayer;
				}
			}
		}
		if (eWinner != NO_PLAYER)
		{
			iItemValue = GET_PLAYER(eWinner).GetDealAI()->GetTradeItemValue(eItem, !bFromMe, eMyPlayer, iData1, iData2, iData3, bFlag1, iDuration, bIsAIOffer, false);
			// resources which the winner evaluates as 0 must be impossible to trade, otherwise an unlimited amount of them could be added to the deal
			if (iItemValue == 0 && eItem == TRADE_ITEM_RESOURCES)
			{
				iItemValue = INT_MAX;
			}
		}
		// non-peace deals or white peace: the average of the two players' evaluation is taken
		else
		{
			bool bHumanInvolved = GetPlayer()->isHuman() || GET_PLAYER(eOtherPlayer).isHuman();

			// modify buy and sell price by the players' opinions towards each other
			int iApproachModifier = (GetPlayer()->isHuman()) ? 100 : GetPlayer()->GetDiplomacyAI()->GetSurfaceApproachDealModifier(eOtherPlayer, bFromMe);
			int iOtherPlayerApproachModifier = (GET_PLAYER(eOtherPlayer).isHuman()) ? 100 : GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetSurfaceApproachDealModifier(eMyPlayer, !bFromMe);

			int iBuyPrice = 0;
			int iBuyModifier = 0;
			int iSellPrice = 0;
			int iSellModifier = 0;

			int iMaxAcceptableBuyPrice = 0;
			int iMinAcceptableSellPrice = 0;

			bool bTwoSidedItem = (
				eItem == TRADE_ITEM_DECLARATION_OF_FRIENDSHIP ||
				eItem == TRADE_ITEM_DEFENSIVE_PACT ||
				eItem == TRADE_ITEM_RESEARCH_AGREEMENT
				);

			if (bFromMe)
			{
				// we are selling
				iSellPrice = GetTradeItemValue(eItem, bFromMe, eOtherPlayer, iData1, iData2, iData3, bFlag1, iDuration, bIsAIOffer, false);
				if (iSellPrice == INT_MAX)
				{
					iItemValue = INT_MAX;
				}
				else
				{
					// modify acceptable selling price based on opinion
					iSellModifier = iApproachModifier;
					iMinAcceptableSellPrice = iSellPrice * iSellModifier / 100;
					iBuyPrice = GET_PLAYER(eOtherPlayer).GetDealAI()->GetTradeItemValue(eItem, !bFromMe, eMyPlayer, iData1, iData2, iData3, bFlag1, iDuration, bIsAIOffer, false);
					if (iBuyPrice == INT_MAX)
					{
						// The other player doesn't want to buy this item (perceived buy value if human).
						// Don't offer the item in AI-AI deals or in AI offers to humans.
						// If a human has asked for this item, return the sell price we'd be willing to accept.
						iItemValue = (!bHumanInvolved || bIsAIOffer) ? INT_MAX : iMinAcceptableSellPrice;
					}
					else
					{
						// modify acceptable buying price based on opinion
						iBuyModifier = iOtherPlayerApproachModifier;
						iMaxAcceptableBuyPrice = iBuyPrice * iBuyModifier / 100;

						// take the average of buy and sell price, modified by opinion
						iItemValue = (iBuyPrice + iSellPrice) / 2;
						iItemValue *= iSellModifier;
						iItemValue /= 100;
						iItemValue *= iBuyModifier;
						iItemValue /= 100;

						// for items that are not two-sided: check if item value is acceptable for both players
						if (!bTwoSidedItem && (iItemValue > iMaxAcceptableBuyPrice || iItemValue < iMinAcceptableSellPrice))
						{
							// The deal is unacceptable for us or for the buyer (perceived buy value if human).
							// Don't offer the item in AI-AI deals or in AI offers to humans.
							// If a human has asked for this item, return the sell price we'd be willing to accept.
							iItemValue = (!bHumanInvolved || bIsAIOffer) ? INT_MAX : iMinAcceptableSellPrice;
						}
					}
				}
			}
			else
			{
				// we are buying
				iBuyPrice = GetTradeItemValue(eItem, bFromMe, eOtherPlayer, iData1, iData2, iData3, bFlag1, iDuration, bIsAIOffer, false);
				if (iBuyPrice == INT_MAX)
				{
					iItemValue = INT_MAX;
				}
				else
				{
					// modify acceptable buying price based on opinion
					iBuyModifier = iApproachModifier;
					iMaxAcceptableBuyPrice = iBuyPrice * iBuyModifier / 100;
					iSellPrice = GET_PLAYER(eOtherPlayer).GetDealAI()->GetTradeItemValue(eItem, !bFromMe, GetPlayer()->GetID(), iData1, iData2, iData3, bFlag1, iDuration, bIsAIOffer, false);
					if (iSellPrice == INT_MAX)
					{
						// The other player doesn't want to sell this item (perceived sell value if human). 
						// Don't offer the item in AI-AI deals or in AI offers to humans.
						// If a human is offering this item, return the buy price we'd be willing to accept.
						iItemValue = (!bHumanInvolved || bIsAIOffer) ? INT_MAX : iMaxAcceptableBuyPrice;
					}
					else
					{
						// modify acceptable selling price based on opinion
						iSellModifier = iOtherPlayerApproachModifier;
						iMinAcceptableSellPrice = iSellPrice * iSellModifier / 100;

						// take the average of buy and sell price, modified by opinion
						iItemValue = (iBuyPrice + iSellPrice) / 2;
						iItemValue *= iSellModifier;
						iItemValue /= 100;
						iItemValue *= iBuyModifier;
						iItemValue /= 100;

						// for items that are not two-sided: check if item value is acceptable for both players
						if (!bTwoSidedItem && (iItemValue > iMaxAcceptableBuyPrice || iItemValue < iMinAcceptableSellPrice))
						{
							// The deal is unacceptable for us or for the buyer (perceived buy value if human).
							// Don't offer the item in AI-AI deals or in AI offers to humans.
							// If a human is offering this item, return the buy price we'd be willing to accept.
							iItemValue = (!bHumanInvolved || bIsAIOffer) ? INT_MAX : iMaxAcceptableBuyPrice;
						}
					}
				}
			}

			if (iItemValue != INT_MAX && iItemValue > 0)
			{
				// Item is worth 20% less if its owner is a vassal
				if (bFromMe)
				{
					// If it's my item and I'm the vassal of the other player, reduce it.
					if (GET_TEAM(GetPlayer()->getTeam()).GetMaster() == GET_PLAYER(eOtherPlayer).getTeam())
					{
						iItemValue *= 80;
						iItemValue /= 100;
					}
				}
				else
				{
					// If it's their item and they're my vassal, reduce it.
					if (GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetMaster() == GetPlayer()->getTeam())
					{
						iItemValue *= 80;
						iItemValue /= 100;
					}
				}

				// Difficulty option to modify the AI's selling prices towards humans.
				if (bFromMe && GET_PLAYER(eOtherPlayer).isHuman() && eItem != TRADE_ITEM_PEACE_TREATY && (eItem != TRADE_ITEM_THIRD_PARTY_PEACE || !GetPlayer()->IsAtWarWith(eOtherPlayer)))
				{
					iItemValue *= 100 + GET_PLAYER(eOtherPlayer).getHandicapInfo().getHumanTradeModifier();
					iItemValue /= 100;
				}

				if (iItemValue <= 0)
					iItemValue = 1;
			}
		}
	}
	
	//cache update
	m_dealItemValues[currentCall] = iItemValue;
	return iItemValue;
}

/// How much Gold should be provided if we're trying to make it worth iValue?
int CvDealAI::GetGoldForForValueExchange(int iGoldOrValue, bool bNumGoldFromValue)
{
	int iMultiplier = 0;
	int iDivisor = 0;

	// We passed in Value, we want to know how much Gold we get for it
	if (bNumGoldFromValue)
	{
		iMultiplier = 100;
		iDivisor = /*100*/ max(GD_INT_GET(EACH_GOLD_VALUE_PERCENT), 1);
	}
	// We passed in an amount of Gold, we want to know how much it's worth
	else
	{
		iMultiplier = /*100*/ GD_INT_GET(EACH_GOLD_VALUE_PERCENT);
		iDivisor = 100;
	}

	// Convert based on the rules above
	return iGoldOrValue * iMultiplier / iDivisor;
}

/// How much GPT should be provided if we're trying to make it worth iValue?
int CvDealAI::GetGPTForForValueExchange(int iGPTorValue, bool bNumGPTFromValue, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of GPT with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (iGPTorValue <= 0)
		return -1;

	int iValueTimes100 = 0;

	// We passed in Value, we want to know how much GPT we get for it
	if (bNumGPTFromValue)
	{
		//let's assume an interest rate of 0.5% per turn, no compounding
		int iInterestPercent = (5 * /*5*/ GD_INT_GET(EACH_GOLD_PER_TURN_VALUE_PERCENT) * iNumTurns) / max(1, GC.getGame().getGameSpeedInfo().GetDealDuration());

		//add interest. 100 gold now is better than 100 gold in the future
		iGPTorValue += (iGPTorValue*iInterestPercent) / 100;

		// Sometimes we want to round up. Let's say the AI offers a deal to the human. We have to ensure that the human can also offer that deal back and the AI will accept (and vice versa)
		int iRound = 0;
		while (iGPTorValue % iNumTurns != 0 && iRound < iNumTurns/2)
		{
			iGPTorValue++;
			iRound++;
		}

		iValueTimes100 = iGPTorValue;
		iValueTimes100 /= iNumTurns;
	}
	else
	{
		if (!GetPlayer()->isHuman() && bFromMe)
		{
			int iNetGold = GetPlayer()->calculateGoldRate();
			std::vector<CvDeal*> pRenewDeals = GetPlayer()->GetDiplomacyAI()->GetDealsToRenew(eOtherPlayer);
			if (pRenewDeals.size() > 0)
			{
				for (uint i = 0; i < pRenewDeals.size(); i++)
				{
					// include the gold of the renew deals if the AI is currently evaluating if it should propose the deal again (deals that are not yet checked for renewal)
					// if the renewed deal is being offered to the human, don't include it as AI values have already been reset
					if (!pRenewDeals[i]->IsCheckedForRenewal())
					{
						iNetGold += pRenewDeals[i]->GetGoldPerTurnTrade(GetPlayer()->GetID());
						iNetGold -= pRenewDeals[i]->GetGoldPerTurnTrade(eOtherPlayer);
					}
				}
			}
			if (iGPTorValue > (iNetGold - 2))
				return INT_MAX;
		}
		iValueTimes100 = (iGPTorValue * iNumTurns);

		//let's assume an interest rate of 0.5% per turn, no compounding
		int iInterestPercent = (5 * /*5*/ GD_INT_GET(EACH_GOLD_PER_TURN_VALUE_PERCENT) * iNumTurns) / max(1,GC.getGame().getGameSpeedInfo().GetDealDuration());

		//subtract interest. 100 gold now is better than 100 gold in the future
		iValueTimes100 *= 100;
		iValueTimes100 /= max(1,100 + iInterestPercent);
	}

	int iReturnValue = iValueTimes100;

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
	bool bPeaceDeal = GetPlayer()->IsAtWarWith(eOtherPlayer);

	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	if (!pkResourceInfo)
		return 0;

	//Integer zero check...
	if (iNumTurns <= 0)
		iNumTurns = 1;

	//how much happiness from one additional luxury?
	int iBaseHappiness = 0;
	if (bFromMe)
		iBaseHappiness += GetPlayer()->GetHappinessFromLuxury(eResource) + GetPlayer()->GetExtraHappinessPerLuxury();
	else
		iBaseHappiness += GET_PLAYER(eOtherPlayer).GetHappinessFromLuxury(eResource) + GET_PLAYER(eOtherPlayer).GetExtraHappinessPerLuxury();

	// How much of this luxury resource do we already have?
	int iNumAvailableToUs = GetPlayer()->getNumResourceTotal(eResource, true);

	std::vector<CvDeal*> pRenewDeals = GetPlayer()->GetDiplomacyAI()->GetDealsToRenew(eOtherPlayer);
	if (pRenewDeals.size() > 0)
	{
		for (uint i = 0; i < pRenewDeals.size(); i++)
		{
			// include the gold of the renew deals if the AI is currently evaluating if it should propose the deal again (deals that are not yet checked for renewal)
			// if the renewed deal is being offered to the human, don't include it as AI values have already been reset
			if (!pRenewDeals[i]->IsCheckedForRenewal())
			{
				iNumAvailableToUs += pRenewDeals[i]->GetNumResourcesInDeal(GetPlayer()->GetID(), eResource);
				iNumAvailableToUs -= pRenewDeals[i]->GetNumResourcesInDeal(eOtherPlayer, eResource);
				iCurrentNetGoldOfReceivingPlayer -= pRenewDeals[i]->GetGoldPerTurnTrade(GetPlayer()->GetID());
				iCurrentNetGoldOfReceivingPlayer += pRenewDeals[i]->GetGoldPerTurnTrade(eOtherPlayer);
			}
		}
	}

	// VP calculation
	if (MOD_BALANCE_VP)
	{
		int OneGPT = GetOneGPTValue(bPeaceDeal);
		int OneGPTScaled = OneGPT * GC.getNumEraInfos() * GET_TEAM(GetTeam()).GetTechProgressPercent() / 100;

		// Base value
		int iItemValue = OneGPTScaled + OneGPT;

		// We are selling!
		if (bFromMe)
		{
			if (iNumAvailableToUs == 1)
			{
				// Don't sell our last copy if already unhappy
				if (GetPlayer()->IsEmpireUnhappy())
					return INT_MAX;

				// Don't sell our last copy if it would make us unhappy
				int iUnhappyCitizens = GetPlayer()->GetUnhappinessFromCitizenNeeds();
				if (iUnhappyCitizens > 0)
				{
					int iHappyCitizens = GetPlayer()->GetHappinessFromCitizenNeeds() - iBaseHappiness;
					int iPercent = min(200, (iHappyCitizens * 100) / max(1, iUnhappyCitizens));
					iPercent /= 2;

					if (iPercent < /*50*/ GD_INT_GET(UNHAPPY_THRESHOLD))
						return INT_MAX;
				}

				// Extra value for the last copy
				iItemValue += OneGPTScaled * (iBaseHappiness + 2);
			}

			// Netherlands sells resources more cheaply
			if (GetPlayer()->getResourceExport(eResource) <= 0)
			{
				int iYieldBonusFromExport = 0;
				for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
				{
					YieldTypes eYield = (YieldTypes)iYieldLoop;

					//Simplification - errata yields not worth considering.
					if (eYield > YIELD_GOLDEN_AGE_POINTS)
						continue;

					int iBonus = GetPlayer()->GetPlayerTraits()->GetYieldFromExport(eYield);
					if (iBonus > 0)
					{
						if (eYield == YIELD_SCIENCE || eYield == YIELD_CULTURE)
							iBonus *= 2;

						iYieldBonusFromExport += iBonus;
					}
				}

				iItemValue -= (iYieldBonusFromExport * OneGPTScaled) / 3;
				if (iItemValue <= OneGPT)
					return OneGPT;
			}
		}
		// We are buying!
		else
		{
			// Goddess of Festivals bonus
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(GetPlayer()->GetReligions()->GetStateReligion(), GetPlayer()->GetID());
			if (pReligion)
			{
				if ((iNumAvailableToUs + GetPlayer()->getResourceExport(eResource)) <= 0)
				{
					int iYieldBonuses = 0;
					for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
					{
						YieldTypes eYield = (YieldTypes)iYieldLoop;

						//Simplification - errata yields not worth considering.
						if (eYield > YIELD_GOLDEN_AGE_POINTS)
							continue;

						if (eYield == YIELD_SCIENCE || eYield == YIELD_CULTURE || eYield == YIELD_FAITH)
							iYieldBonuses += pReligion->m_Beliefs.GetYieldPerLux(eYield, GetPlayer()->GetID(), GetPlayer()->getCapitalCity()) * 2;
						else
							iYieldBonuses += pReligion->m_Beliefs.GetYieldPerLux(eYield, GetPlayer()->GetID(), GetPlayer()->getCapitalCity());
					}
					iItemValue += (iYieldBonuses * OneGPT) / 3;
				}
			}

			// We're unhappy! Buy luxuries!
			if (GetPlayer()->IsEmpireUnhappy())
			{
				if (GetPlayer()->IsEmpireSuperUnhappy())
					iItemValue += OneGPTScaled * 12;
				else if (GetPlayer()->IsEmpireVeryUnhappy())
					iItemValue += OneGPTScaled * 6;
				else
					iItemValue += OneGPTScaled * 3;

				// At war or planning war: this is more serious; we may lose the war due to this.
				if (GetPlayer()->IsAtWarAnyMajor())
					iItemValue += OneGPTScaled * 3;
				else
				{
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
					{
						PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
						if (GetPlayer()->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && GetPlayer()->GetDiplomacyAI()->GetCivApproach(eLoopPlayer) == CIV_APPROACH_WAR)
						{
							iItemValue += OneGPTScaled * 3;
							break;
						}
					}
				}
			}

			// Are any of our cities demanding this resource?
			int iLoop = 0;
			int iYieldBonuses = 0;
			for (CvCity* pLoopCity = GetPlayer()->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoop))
			{
				if (pLoopCity->IsResistance() || pLoopCity->IsRazing() || pLoopCity->GetResourceDemanded() != eResource || pLoopCity->GetWeLoveTheKingDayCounter() > 0)
					continue;

				// Bonus value for each WLTKD if we're not unhappy, plus additional bonus value for any yield bonuses we get (below)
				if (!GetPlayer()->IsEmpireUnhappy())
					iItemValue += OneGPT * 2;

				// Tally up the total modifiers this city gets.
				for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
				{
					YieldTypes eYield = (YieldTypes)iYieldLoop;

					//Simplification - errata yields not worth considering.
					if (eYield > YIELD_GOLDEN_AGE_POINTS)
						continue;

					// Do we have any WLTKD modifiers at the player level?
					int iPlayerModifier = GetPlayer()->GetYieldFromWLTKD(eYield);
					if (eYield == YIELD_CULTURE)
						iPlayerModifier += GetPlayer()->GetPlayerTraits()->GetWLTKDCulture();
					else if (eYield == YIELD_FOOD)
						iPlayerModifier += GetPlayer()->GetPlayerTraits()->GetGrowthBoon();

					// Do we have any WLTKD modifiers at the city level?
					int iCityModifier = iPlayerModifier; // Add the player modifier for each city
					iCityModifier += pLoopCity->GetYieldFromWLTKD(eYield);
					if (pReligion)
						iCityModifier += pReligion->m_Beliefs.GetYieldFromWLTKD(eYield, GetPlayer()->GetID(), pLoopCity);

					int iYieldValue = 1;
					if (eYield == YIELD_SCIENCE || eYield == YIELD_CULTURE || eYield == YIELD_FAITH)
						iYieldValue = 2;

					iYieldBonuses += iYieldValue * pLoopCity->getBaseYieldRate(eYield) * (iPlayerModifier + iCityModifier) / 100;
				}
			}
			int iWLTKDLength = (GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 2);
			iWLTKDLength *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iWLTKDLength /= 100;

			iYieldBonuses *= iWLTKDLength;
			iYieldBonuses /= iNumTurns;
			iItemValue += (iYieldBonuses * OneGPT) / 3;

			// Netherlands buys resources for more if they aren't already importing it
			if (GetPlayer()->getResourceImportFromMajor(eResource) <= 0 &&
				GetPlayer()->getResourceFromMinors(eResource) <= 0 &&
				GetPlayer()->getResourceFromCSAlliances(eResource) <= 0 &&
				GetPlayer()->getResourceSiphoned(eResource) <= 0)
			{
				int iYieldBonusFromImport = 0;
				for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
				{
					YieldTypes eYield = (YieldTypes)iYieldLoop;

					//Simplification - errata yields not worth considering.
					if (eYield > YIELD_GOLDEN_AGE_POINTS)
						continue;

					int iBonus = GetPlayer()->GetPlayerTraits()->GetYieldFromImport(eYield);
					if (iBonus > 0)
					{
						if (eYield == YIELD_SCIENCE || eYield == YIELD_CULTURE)
							iBonus *= 2;

						iYieldBonusFromImport += iBonus;
					}
				}

				iItemValue += (iYieldBonusFromImport * OneGPTScaled) / 3;
			}

			// Netherlands might also want to buy resources that get them a monopoly
			if (GetPlayer()->GetPlayerTraits()->IsImportsCountTowardsMonopolies() && GetPlayer()->WouldGainMonopoly(eResource, 1))
			{
				// What does this monopoly give us?
				bool bPercentageBonus = false;
				int iNumYieldChangeBonuses = 0;
				for (int iYieldLoop = 0; iYieldLoop < NUM_YIELD_TYPES; iYieldLoop++)
				{
					YieldTypes eYield = (YieldTypes)iYieldLoop;

					//Simplification - errata yields not worth considering.
					if (eYield > YIELD_GOLDEN_AGE_POINTS)
						continue;

					if (pkResourceInfo->getCityYieldModFromMonopoly(eYield) > 0)
						bPercentageBonus = true;

					if (pkResourceInfo->getYieldChangeFromMonopoly(eYield) > 0)
						iNumYieldChangeBonuses++;
				}

				// Percentage bonuses? This is definitely worth it!
				if (bPercentageBonus || pkResourceInfo->getMonopolyGALength() > 0)
					iItemValue += OneGPTScaled * 8;

				// Yield change bonuses? Value depends on how much of this resource we have in our borders.
				if (iNumYieldChangeBonuses > 0)
				{
					for (CvCity* pLoopCity = GetPlayer()->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoop))
					{
						if (pLoopCity->IsRazing())
							continue;

						int iX = pLoopCity->getX();
						int iY = pLoopCity->getY();
						int iNumWorkablePlots = pLoopCity->GetNumWorkablePlots();
						for (int iPlotLoop = 0; iPlotLoop < iNumWorkablePlots; iPlotLoop++)
						{
							CvPlot* pLoopPlot = iterateRingPlots(iX, iY, iPlotLoop);
							if (pLoopPlot && pLoopPlot->getResourceType(GetTeam()) == eResource)
							{
								iItemValue += OneGPTScaled * iNumYieldChangeBonuses; // Okay to double count; monopoly resources placed between two cities are extra valuable
							}
						}
					}
				}
			}

			// Is there a City-State quest the AI wants to fulfill by obtaining this resource?
			for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
			{
				PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
				if (GetPlayer()->GetDiplomacyAI()->IsPlayerValid(eMinor) && GET_PLAYER(eMinor).isMinorCiv() && !GetPlayer()->GetDiplomacyAI()->IsAtWar(eMinor) && GetPlayer()->GetDiplomacyAI()->GetCivApproach(eMinor) > CIV_APPROACH_HOSTILE)
				{
					CvMinorCivAI* pMinorAI = GET_PLAYER(eMinor).GetMinorCivAI();
					if (!pMinorAI->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_CONNECT_RESOURCE))
						continue;

					for (QuestListForPlayer::iterator itr_quest = pMinorAI->m_QuestsGiven[GetPlayer()->GetID()].begin(); itr_quest != pMinorAI->m_QuestsGiven[GetPlayer()->GetID()].end(); ++itr_quest)
					{
						if (itr_quest->GetType() == MINOR_CIV_QUEST_CONNECT_RESOURCE && (ResourceTypes)itr_quest->GetPrimaryData() == eResource)
						{
							iItemValue += itr_quest->GetGold();
							iItemValue += (GetPlayer()->GetPlayerTraits()->IsDiplomat() || GetPlayer()->GetDiplomacyAI()->IsGoingForDiploVictory()) ? itr_quest->GetInfluence() * 4 : itr_quest->GetInfluence() * 2;
						}
					}
				}
			}

			// Resource value is increased based on net GPT, capped at 20% of the value of the resource itself
			if (iCurrentNetGoldOfReceivingPlayer > 0)
			{
				int iGPTSurcharge = int(0.5 * sqrt(iCurrentNetGoldOfReceivingPlayer / 1.));
				iItemValue += min(iGPTSurcharge, iItemValue / 5);
			}
		}

		// Cap price at 10 GPT, scaling with era
		iItemValue = min(iItemValue, OneGPTScaled * 10);
		return iItemValue;
	}

	// Base Community Patch calculation
	int iItemValue = (iBaseHappiness + GC.getGame().getCurrentEra()) * iNumTurns / 2;

	if (bFromMe)
	{
		// Resource value is increased based on net GPT, up to the value of the item itself (so never more than double).
		if (iCurrentNetGoldOfReceivingPlayer > 0) {
			int iGPT = int(0.25 + sqrt(iCurrentNetGoldOfReceivingPlayer / 3.));
			iItemValue += min(iGPT, iItemValue);
		}
	}
	else
	{
		// Resource value is increased based on net GPT, up to the value of the item itself (so never more than double).
		if (iCurrentNetGoldOfReceivingPlayer > 0) {
			int iGPT = int(0.25 + sqrt(iCurrentNetGoldOfReceivingPlayer / 4.));
			iItemValue += min(iGPT, iItemValue);
		}
	}

	if (bFromMe)
	{
		if (GetPlayer()->IsEmpireUnhappy() && iNumAvailableToUs == 1)
		{
			return INT_MAX;
		}
		if (iNumAvailableToUs == 1)
		{
			int iFactor = GetPlayer()->GetPlayerTraits()->GetLuxuryHappinessRetention() ? 2 : 3;
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(GetPlayer()->GetReligions()->GetStateReligion(), GetPlayer()->GetID());
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
		return iItemValue;
	}
	else
	{
		if (GetPlayer()->IsEmpireUnhappy())
		{
			iItemValue *= GetPlayer()->IsEmpireVeryUnhappy() ? 3 : 2;
		}

		if (GetPlayer()->GetPlayerTraits()->IsImportsCountTowardsMonopolies() && GetPlayer()->GetMonopolyPercent(eResource) < GC.getGame().GetGreatestPlayerResourceMonopolyValue(eResource))
		{
			//we don't want monopolies that won't get us a bonus.
			int iNumResourceOwned = GetPlayer()->getNumResourceTotal(eResource, false);
			if (iNumResourceOwned == 0)
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
		const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(GetPlayer()->GetReligions()->GetStateReligion(), GetPlayer()->GetID());
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
				iItemValue *= iFactor;
				iItemValue /= 2;
			}
		}
		return iItemValue;
	}
}

/// Which buildings/units do we want to build with our Strategic Resources and how much are they worth?
vector<int> CvDealAI::GetStrategicResourceItemList(ResourceTypes eResource, int iNumTurns, bool bFromMe, int iNumAvailable, bool bPeaceDeal)
{

	vector<int>vTotalWeightList;
	vector<int>vAirUnitsList;
	vector<int>vNonAirUnitsList;

	CvPlayer* pPlayer = GetPlayer();
	int iOneGPTValue = GetOneGPTValue(bPeaceDeal);

	//all techs within 2 of what we currently know
	vector<TechTypes> vFrontierTechs = GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->GetTechFrontier();

	// are there any buildings we can potentially construct using this resource?
	vector<BuildingTypes> vBuildingsWithResourceRequirement;
	vector<int> vMutuallyExclusiveGroupsAlreadyConsidered;
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		const BuildingClassTypes eBuildingClass = static_cast<BuildingClassTypes>(iI);
		CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
		if (pkBuildingClassInfo == NULL)
			continue;

		const CvCivilizationInfo& playerCivilizationInfo = GetPlayer()->getCivilizationInfo();
		const BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings(eBuildingClass);

		if (eBuilding == NO_BUILDING)
			continue;

		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if (pkBuildingInfo == NULL)
			continue;

		if (pkBuildingInfo->GetResourceQuantityRequirement(eResource) == 0)
			continue;

		// don't have prereq tech?
		// if we're selling, consider also buildings we'll soon be able to build
		TechTypes ePrereqTech = (TechTypes)pkBuildingInfo->GetPrereqAndTech();
		if (ePrereqTech != NO_TECH && !GetPlayer()->HasTech(ePrereqTech) && (!bFromMe || std::find(vFrontierTechs.begin(), vFrontierTechs.end(), ePrereqTech)==vFrontierTechs.end()))
			continue;

		// is the building obsolete?
		if (pkBuildingInfo->GetObsoleteTech() != NO_TECH && GetPlayer()->HasTech((TechTypes)pkBuildingInfo->GetObsoleteTech()))
			continue;

		vBuildingsWithResourceRequirement.push_back(eBuilding);
	}
	if (vBuildingsWithResourceRequirement.size() > 0)
	{
		// loop through our cities and calculate the valuation for each building we can potentially build
		int iCityLoop = 0;
		for (CvCity* pLoopCity = pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
		{
			// Exclude puppets
			if (pLoopCity->IsPuppet())
				continue;

			CvWeightedVector<BuildingTypes> aCityBuildingWeights;
			for (size_t i = 0; i < vBuildingsWithResourceRequirement.size(); i++)
			{
				BuildingTypes eBuilding = vBuildingsWithResourceRequirement[i];
				if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0 || pLoopCity->isBuildingInQueue(eBuilding))
				{
					continue;
				}

				if (!pLoopCity->isValidBuildingLocation(eBuilding) || !pLoopCity->hasBuildingPrerequisites(eBuilding))
					continue;

				// we can potentially construct this building in this city, calculate its weight
				int iWeight = pLoopCity->GetCityStrategyAI()->GetBuildingProductionAI()->GetWeight(eBuilding) / 100;
				iWeight *= pLoopCity->getEconomicValue(GetPlayer()->GetID());
				iWeight *= iOneGPTValue;
				iWeight /= 100000;

				// scaling with deal length
				iWeight *= iNumTurns;
				iWeight /= 50;

				if (iWeight > 0)
				{
					int iNumResourceNeeded = GC.getBuildingInfo(eBuilding)->GetResourceQuantityRequirement(eResource);
					// add a reduced weight multiple times if the building needs more than one resource
					for (int j = 0; j < iNumResourceNeeded; j++)
					{
						aCityBuildingWeights.push_back(eBuilding, iWeight / iNumResourceNeeded);
					}
				}
			}
			aCityBuildingWeights.StableSortItems();

			// if any of the buildings is in a mutually exclusive group, exclude it if another building of that group is already built
			// if we have several buildings in the same group, exclude all but the one with the highest weighting
			vector<int>vMutuallyExclusiveGroupsChecked;
			for (int i = 0; i < aCityBuildingWeights.size(); i++)
			{
				int iBuildingExclusiveGroup = GC.getBuildingInfo(aCityBuildingWeights.GetElement(i))->GetMutuallyExclusiveGroup();

				if (iBuildingExclusiveGroup != -1)
				{
					if (std::find(vMutuallyExclusiveGroupsChecked.begin(), vMutuallyExclusiveGroupsChecked.end(), iBuildingExclusiveGroup) == vMutuallyExclusiveGroupsChecked.end())
					{
						// we have a building in a mutually exclusive group that has not yet been checked
						// check if there's an existing building of the same group
						bool bIsExistingBuilding = false;
						int iNumBuildingInfos = GC.getNumBuildingInfos();
						for (int iI = 0; iI < iNumBuildingInfos; iI++)
						{
							const BuildingTypes eBuildingLoop = static_cast<BuildingTypes>(iI);
							if (eBuildingLoop == aCityBuildingWeights.GetElement(i))
								continue;

							CvBuildingEntry* pkLoopBuilding = GC.getBuildingInfo(eBuildingLoop);
							if (pkLoopBuilding)
							{
								// Buildings are in a Mutually Exclusive Group, so only one is allowed
								if (pkLoopBuilding->GetMutuallyExclusiveGroup() == iBuildingExclusiveGroup)
								{
									if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuildingLoop) > 0 || pLoopCity->isBuildingInQueue(eBuildingLoop))
									{
										bIsExistingBuilding = true;
									}
								}
							}
						}

						if (!bIsExistingBuilding)
						{
							// the building can be constructed. add it to the list and mark its mutually exclusive group as checked to prevent other buildings of the same group being added
							vMutuallyExclusiveGroupsChecked.push_back(iBuildingExclusiveGroup);
							vTotalWeightList.push_back(aCityBuildingWeights.GetWeight(i));
						}
					}
				}
				else
				{
					vTotalWeightList.push_back(aCityBuildingWeights.GetWeight(i));
				}
				// if we're selling and we know at this point already that we need all the resource items we have, we can stop here
				if (bFromMe && (int)vTotalWeightList.size() >= iNumAvailable)
				{
					return vTotalWeightList;
				}
			}
		}
	}


	// are there any units we can potentially train using this resource?
	int iNumSupplyLeft = pPlayer->GetNumUnitsSupplied() - pPlayer->GetNumUnitsToSupply();

	//at war or planing to go to war?
	bool bWar = false;
	if (GetPlayer()->IsAtWarAnyMajor())
	{
		bWar = true;
	}
	else if (!GetPlayer()->isHuman())
	{
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
			if (pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && pPlayer->GetDiplomacyAI()->GetCivApproach(eLoopPlayer) == CIV_APPROACH_WAR)
			{
				bWar = true;
				break;
			}
		}
	}
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		const UnitClassTypes eUnitClass = static_cast<UnitClassTypes>(iI);
		const UnitTypes eUnit = pPlayer->GetSpecificUnitType(eUnitClass);
		if (eUnit == NO_UNIT)
			continue;

		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if (pkUnitInfo == NULL)
			continue;

		// does the unit require the resource?
		if (pkUnitInfo->GetResourceQuantityRequirement(eResource) == 0)
			continue;

		if (pPlayer->GetPlayerTraits()->NoTrain(eUnitClass))
			continue;

		if (MOD_BALANCE_CORE_MINOR_CIV_GIFT && pkUnitInfo->IsMinorCivGift())
			continue;

		// if we're over supply limit, only check air units
		if (iNumSupplyLeft < 0 && pkUnitInfo->GetAirUnitCap() == 0)
			continue;

		// nuclear non-proliferation?
		if (pkUnitInfo->GetNukeDamageLevel() > 0 && (GC.getGame().isNoNukes() || GC.getGame().GetGameLeagues()->IsNoTrainingNuclearWeapons(pPlayer->GetID())))
			continue;

		// don't have prereq tech?
		// if we're selling, consider also units we'll soon be able to build
		TechTypes ePrereqTech = (TechTypes)pkUnitInfo->GetPrereqAndTech();
		if (ePrereqTech != NO_TECH && !pPlayer->HasTech(ePrereqTech) && (!bFromMe || std::find(vFrontierTechs.begin(), vFrontierTechs.end(), ePrereqTech) == vFrontierTechs.end()))
			continue;

		// is the unit obsolete?
		if (pkUnitInfo->GetObsoleteTech() != NO_TECH && pPlayer->HasTech((TechTypes)pkUnitInfo->GetObsoleteTech()))
			continue;

		// don't count spaceship parts here, they'll be checked later
		if (pkUnitInfo->GetSpaceshipProject() != NO_PROJECT)
			continue;

		// we can train this unit, calculate it's value
		int iUnitValue = pkUnitInfo->GetPower();
		if (bWar)
		{
			iUnitValue *= 2;
		}
		// scaling with deal length
		iUnitValue *= iNumTurns;
		iUnitValue /= 50;

		// add the value of the unit to the list 3 times for each unit, with slightly decreasing value each time
		// we'll check later how many units we actually need
		for (int i = 0; i < 3; i++)
		{
			if (pkUnitInfo->GetAirUnitCap() == 0)
				vNonAirUnitsList.push_back(iUnitValue * (10 - i) / 10);
			else
				vAirUnitsList.push_back(iUnitValue * (10 - i) / 10);
		}
	}

	// now check how many units we want to build
	if (vAirUnitsList.size() > 0)
	{
		int iNumFreeAirSlots = 0; // how many air units can we station in cities?
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GetPlayer()->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
		{
			iNumFreeAirSlots += pLoopCity->GetMaxAirUnits() - pLoopCity->plot()->countNumAirUnits(pPlayer->getTeam());
		}

		// select the units with the highest value for which we have space
		if (iNumFreeAirSlots > 0)
		{
			iNumFreeAirSlots = min(iNumFreeAirSlots, (int)vAirUnitsList.size());
			std::stable_sort(vAirUnitsList.rbegin(), vAirUnitsList.rend());
			vTotalWeightList.insert(vTotalWeightList.end(), vAirUnitsList.begin(), vAirUnitsList.begin() + iNumFreeAirSlots);
		}
	}
	if (vNonAirUnitsList.size() > 0 && iNumSupplyLeft > 0)
	{
		int iNonAirUnitsToBuild = iNumSupplyLeft;
		if (!bWar)
		{
			iNonAirUnitsToBuild *= 2;
			iNonAirUnitsToBuild /= 3;
		}
		iNonAirUnitsToBuild = min(max(iNonAirUnitsToBuild, 1), (int)vNonAirUnitsList.size());
		std::stable_sort(vNonAirUnitsList.rbegin(), vNonAirUnitsList.rend());
		vTotalWeightList.insert(vTotalWeightList.end(), vNonAirUnitsList.begin(), vNonAirUnitsList.begin() + iNonAirUnitsToBuild);
	}

	// spaceship parts and spaceship factories
	ResourceTypes eAluminum = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
	if (eResource == eAluminum)
	{
		ProjectTypes eApolloProgram = (ProjectTypes)GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
		if (eApolloProgram != NO_PROJECT && GET_TEAM(m_pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			// how many do we still need?
			int iNumAluminumStillNeeded = GetPlayer()->GetNumAluminumStillNeededForSpaceship() + GetPlayer()->GetNumAluminumStillNeededForCoreCities();
			for (int i = 0; i < iNumAluminumStillNeeded; i++)
			{
				vTotalWeightList.push_back(10000);
			}
		}
	}

	//don't need anything?
	if(vTotalWeightList.size() == 0)
	{
		return vTotalWeightList;
	}

	// sort the resource list (not necessary if we're selling)
	if (!bFromMe)
		std::stable_sort(vTotalWeightList.rbegin(), vTotalWeightList.rend());

	return vTotalWeightList;
}

/// How much is a Resource worth?
int CvDealAI::GetStrategicResourceValue(ResourceTypes eResource, int iResourceQuantity, int iNumTurns, bool bFromMe, PlayerTypes eOtherPlayer, int iCurrentNetGoldOfReceivingPlayer)
{
	bool bPeaceDeal = GetPlayer()->IsAtWarWith(eOtherPlayer);

	if (eResource == NO_RESOURCE)
		return INT_MAX;

	if (!GetPlayer()->IsResourceRevealed(eResource))
		return INT_MAX;

	const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
	if (!pkResourceInfo)
		return INT_MAX;

	if (iResourceQuantity == 0)
		return 0;

	//Integer zero check...
	if (iNumTurns <= 0)
		iNumTurns = 1;

	// how many resources do we have available? when selling, don't count imported resources
	int iNumberAvailableToUs = GetPlayer()->getNumResourceAvailable(eResource, !bFromMe);
	// when buying, we treat unimproved resources in our empire as improved
	if (!bFromMe)
		iNumberAvailableToUs += GetPlayer()->getNumResourceUnimproved(eResource);

	// include resources from renew deals
	std::vector<CvDeal*> pRenewDeals = m_pPlayer->GetDiplomacyAI()->GetDealsToRenew(eOtherPlayer);

	if (pRenewDeals.size() > 0)
	{
		for (uint i = 0; i < pRenewDeals.size(); i++)
		{
			// include the gold of the renew deals if the AI is currently evaluating if it should propose the deal again (deals that are not yet checked for renewal)
			// if the renewed deal is being offered to the human, don't include it as AI values have already been reset
			if (!pRenewDeals[i]->IsCheckedForRenewal())
			{
				iNumberAvailableToUs += pRenewDeals[i]->GetNumResourcesInDeal(GetPlayer()->GetID(), eResource);
				iNumberAvailableToUs -= pRenewDeals[i]->GetNumResourcesInDeal(eOtherPlayer, eResource);
				iCurrentNetGoldOfReceivingPlayer -= pRenewDeals[i]->GetGoldPerTurnTrade(GetPlayer()->GetID());
				iCurrentNetGoldOfReceivingPlayer += pRenewDeals[i]->GetGoldPerTurnTrade(eOtherPlayer);
			}
		}
	}
	
	//Never trade away everything.
	if (bFromMe && (iNumberAvailableToUs - iResourceQuantity <= 0))
		return INT_MAX;

	// Don't sell Uranium to Nuclear Gandhi!
	ResourceTypes eUranium = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_URANIUM", true);
	if (eResource == eUranium)
	{
		if (bFromMe && GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsNuclearGandhi(true))
			return INT_MAX;
	}

	// Don't buy if we already have enough
	if (!bFromMe && iNumberAvailableToUs >= 5)
		return 0;

	// for every item of the resource that we could use for something, list its valuation
	vector<int>vTotalWeightList = GetStrategicResourceItemList(eResource, iNumTurns, bFromMe, iNumberAvailableToUs, bPeaceDeal);

	// each item of a strategic resource has as base value the value of 1 GPT, scaling with Era
	int iGPTValue = GetOneGPTValue(bPeaceDeal);
	int iBaseValue = iGPTValue * GC.getNumEraInfos() * GET_TEAM(GetTeam()).GetTechProgressPercent() / 100;

	int iValue = 0;
	if (bFromMe)
	{
		// we're selling
		// how many items are we willing to give away?
		int iNumberWeCanSell = iNumberAvailableToUs - (int)vTotalWeightList.size();
		if (iNumberWeCanSell <= 0)
			return INT_MAX;

		int iValueToAdd = iBaseValue;

		// Uranium is extra valuable
		if (eResource == eUranium)
			iValueToAdd *= 2;

		// If this trade leaves us with 1 or 2 of the resource, we'll charge way more for those.
		for (int iLoop = 1; iLoop <= iResourceQuantity; iLoop++)
		{
			int iAmountAfterThisResource = iNumberWeCanSell - iLoop;
			if (iAmountAfterThisResource <= 0)
				return INT_MAX;
			else if (iAmountAfterThisResource <= 2)
				iValue += iValueToAdd * 4 * (3 - iAmountAfterThisResource);
			else
				iValue += iValueToAdd;
		}
	}
	else
	{
		// we're buying
		if (vTotalWeightList.size() == 0)
		{
			// don't need anything
			return 0;
		}
		else
		{
			// vTotalWeightsList is sorted in descending order. loop through the vector (excluding the first iNumberAvailableToUs elements) to add the value for the items we could build with the new resources we would get
			// don't buy more than 5 at a time
			for (int iLoop = iNumberAvailableToUs; iLoop < iNumberAvailableToUs + min(5, iResourceQuantity); iLoop++)
			{
				if (iLoop < (int)vTotalWeightList.size())
				{
					// if we're lacking resources (iLoop < 0), pay for them as much as the highest value in the list
					int iValueToAdd = vTotalWeightList[max(0,iLoop)];
					// Resource value is increased based on net GPT, capped at 20% of the value of the resource itself
					if (iCurrentNetGoldOfReceivingPlayer > 0)
					{
						int iGPTSurcharge = int(0.5 * sqrt(iCurrentNetGoldOfReceivingPlayer / 1.));
						iValueToAdd += min(iGPTSurcharge, iValueToAdd / 5);
					}
					iValue += min(max(iBaseValue, iValueToAdd), 10 * iBaseValue);
				}
			}
		}
	}
	return iValue;
}

/// How much is a City worth - that is: how much would the buyer pay?
int CvDealAI::GetCityValueForDeal(CvCity* pCity, PlayerTypes eAssumedOwner)
{
	//can't trade capitals ever
	if (!pCity || pCity->isCapital())
		return INT_MAX;

	//note that it can also happen that a player pretends to buy a city they already own, just to see the appropriate price
	CvPlayer& assumedOwner = GET_PLAYER(eAssumedOwner);
	bool bPeaceTreatyTrade = assumedOwner.IsAtWarWith(pCity->getOwner());

	// Don't buy any cities that aren't ours if we're unhappy
	if (!bPeaceTreatyTrade && pCity->getOwner() != eAssumedOwner && pCity->getOriginalOwner() != eAssumedOwner && GET_PLAYER(eAssumedOwner).IsEmpireUnhappy())
		return INT_MAX;

	//if we already own it and trade voluntarily ...
	if (!bPeaceTreatyTrade && pCity->getOwner() == eAssumedOwner)
	{
		//traded this city before? Don't give it away again.
		if (pCity->IsTraded(eAssumedOwner))
			return INT_MAX;

		//don't sell if less than 5 cities!
		if (assumedOwner.getNumCities() < 5)
			return INT_MAX;

		//do not trade special cities
		if (pCity->IsOriginalCapital() || pCity->GetCityReligions()->IsHolyCityAnyReligion())
			return INT_MAX;
	}

	//initial value
	int iItemValue = 20000;

	//economic value is important
	int iEconomicValue = pCity->getEconomicValue(eAssumedOwner);
	int iEconomicValuePerPop = (iEconomicValue / (max(1, pCity->getPopulation())));
	iItemValue += (max(1,iEconomicValue-1000)/3); //tricky to define the correct factor

	//prevent cheesy exploit: founding cities just to sell them
	if (!bPeaceTreatyTrade && (GC.getGame().getGameTurn() - pCity->getGameTurnFounded()) < (42 + GC.getGame().randRangeExclusive(0, 5, CvSeeder(iEconomicValue))))
		return INT_MAX;

	//If not as good as any of our cities, we don't want it.
	int iBetterThanCount = 0;
	int iCityLoop = 0;
	for(CvCity* pLoopCity = assumedOwner.firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = assumedOwner.nextCity(&iCityLoop))
	{
		int iScore = (pLoopCity->getEconomicValue(eAssumedOwner) / (max(1, pLoopCity->getPopulation())));
		if (iEconomicValuePerPop > iScore)
			iBetterThanCount++;
	}
	//better than half of the buyer's cities?
	if (iBetterThanCount > assumedOwner.getNumCities() / 2)
		iItemValue *= 2;

	//first some amount for the territory (outside of the first ring)
	int iNewInternalBorderCount = 0;
	int iOldInternalBorderCount = 0;
	int iCityTiles = 0;
	for(int iI = RING1_PLOTS; iI < RING5_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);
		if (!pLoopPlot)
			continue;

		//if it belongs to the city
		if (pCity->GetID() == pLoopPlot->getOwningCityID())
			iCityTiles++;
		else if (pLoopPlot->getOwner() == eAssumedOwner)
			//belongs to another one of the buyer's cities
			iNewInternalBorderCount++;
		else if (pLoopPlot->getOwner() == pCity->getOwner())
			//belongs to another one of the current owner's cities
			iOldInternalBorderCount++;
	}

	//this is how much ANY plot is worth to the buyer right now
	int goldPerPlot = assumedOwner.GetBuyPlotCost();
	iItemValue += goldPerPlot * 50 * iCityTiles;

	//important. it's valuable to have as much internal border as possible
	iItemValue += goldPerPlot * 80 * iNewInternalBorderCount;

	//unhappy cities are worth less
	iItemValue -= pCity->getUnhappyCitizenCount() * goldPerPlot * 30;

	// premium if buyer founded it
	if (pCity->getOriginalOwner() == eAssumedOwner)
	{
		iItemValue *= 120;
		iItemValue /= 100;
	}

	// premium if buyer currently owns it
	if ( pCity->getOwner() == eAssumedOwner )
	{
		iItemValue *= 120;
		iItemValue /= 100;
	}
	else if (iNewInternalBorderCount == 0 && iOldInternalBorderCount == 0)
	{
		//don't buy a city which doesn't overlap with existing cities, unless it's isolated from the current owner as well
		return INT_MAX;
	}

	if (pCity->IsPuppet())
	{
		iItemValue *= 70;
		iItemValue /= 100;
	}

	//don't want the trouble
	if (pCity->IsResistance())
	{
		iItemValue *= (100-10*min(5,pCity->GetResistanceTurns()));
		iItemValue /= 100;
	}

	//obviously the owner doesn't really want it
	if (pCity->IsRazing())
	{
		iItemValue *= 50;
		iItemValue /= 100;
	}

	// don't want to lose wonders
	int iWonders = pCity->getNumWorldWonders();
	if (pCity->getOwner() == eAssumedOwner)
		iWonders += pCity->getNumNationalWonders();

	if (iWonders > 0)
	{
		iItemValue *= (100 + iWonders*11);
		iItemValue /= 100;
	}

	// add some friction
	if (pCity->getOwner() != eAssumedOwner)
	{
		//don't sell to warmongers
		switch (GET_PLAYER(pCity->getOwner()).GetDiplomacyAI()->GetWarmongerThreat(eAssumedOwner))
		{
		case THREAT_NONE:
		case THREAT_MINOR:
			break; // No change in value.
		case THREAT_MAJOR:
			iItemValue *= 2;
			break;
		case THREAT_SEVERE:
		case THREAT_CRITICAL:
			if (bPeaceTreatyTrade)
				iItemValue *= 3;
			else
				return INT_MAX;
			break;
		}

		// reduce city value if it's about to be captured by the assumed owner
		if (bPeaceTreatyTrade && pCity->plot()->GetNumEnemyUnitsAdjacent(pCity->getTeam(), NO_DOMAIN, NULL, false, GET_PLAYER(eAssumedOwner).getTeam(),true) >= 2)
		{
			int iHPPercent = 100 * (pCity->GetMaxHitPoints() - pCity->getDamage()) / pCity->GetMaxHitPoints();
			iItemValue *= max(10, min(2*iHPPercent, 100));
			iItemValue /= 100;
		}

		if (!bPeaceTreatyTrade && !assumedOwner.isHuman())
		{
			// don't buy a city we're trying to liberate (exploitable)
			// this is not an ideal solution - ideally AI would check whether the city is at risk of getting recaptured if liberated ... but will do for now
			if (assumedOwner.GetDiplomacyAI()->IsTryingToLiberate(pCity))
				return INT_MAX;
		}
	}

	//OutputDebugString(CvString::format("City value for %s from %s to %s is %d\n", pCity->getName().c_str(), sellingPlayer.getName(), buyingPlayer.getName(), iItemValue).c_str());
	return max(iItemValue, pCity->getPopulation()*729);
}

// How much is an embassy worth?
int CvDealAI::GetEmbassyValue(bool bFromMe, PlayerTypes eOtherPlayer)
{
	// Backstabber?
	if (bFromMe && GetPlayer()->GetDiplomacyAI()->IsUntrustworthy(eOtherPlayer))
		return INT_MAX;

	int iItemValue = 50;

	// Scale up or down by deal duration at this game speed
	CvGameSpeedInfo *pkStdSpeedInfo = GC.getGameSpeedInfo((GameSpeedTypes)GD_INT_GET(STANDARD_GAMESPEED));
	if (pkStdSpeedInfo)
	{
		iItemValue *= GC.getGame().getGameSpeedInfo().GetDealDuration();
		iItemValue /= pkStdSpeedInfo->GetDealDuration();
	}

	return iItemValue;
}

/// How much is Open Borders worth?
int CvDealAI::GetOpenBordersValue(bool bFromMe, PlayerTypes eOtherPlayer)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of Open Borders with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();
	CivApproachTypes eApproach = pDiploAI->GetSurfaceApproach(eOtherPlayer);

	int iItemValue = 10;
	
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
		case CIV_APPROACH_HOSTILE:
		case CIV_APPROACH_GUARDED:
		case CIV_APPROACH_WAR:
			iItemValue += 600;
			break;
		case CIV_APPROACH_DECEPTIVE:
			iItemValue += 400;
			break;
		case CIV_APPROACH_AFRAID:
			iItemValue += 300;
			break;
		case CIV_APPROACH_FRIENDLY:
			iItemValue += 50;
			break;
		case CIV_APPROACH_NEUTRAL:
			iItemValue += 200;
			break;
		default:
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

		if (MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && (GetPlayer()->GetCulture()->GetTourism() / 100) > 0)
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

		// Add 200 Gold for each of our artifacts they've stolen
		if (pDiploAI->GetNumArtifactsEverDugUp(eOtherPlayer) > 0)
		{
			iItemValue += pDiploAI->GetNumArtifactsEverDugUp(eOtherPlayer) * 200;
		}

		// Does open borders make their production siphon trait stronger?
		if (MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON && GET_PLAYER(eOtherPlayer).GetPlayerTraits()->IsTradeRouteProductionSiphon())
		{
			iItemValue += GET_PLAYER(eOtherPlayer).GetPlayerTraits()->GetTradeRouteProductionSiphon(true).m_iPercentIncreaseWithOpenBorders * 20;
			iItemValue += GET_PLAYER(eOtherPlayer).GetPlayerTraits()->GetTradeRouteProductionSiphon(false).m_iPercentIncreaseWithOpenBorders * 20;
		}
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
		case CIV_APPROACH_WAR:
		case CIV_APPROACH_HOSTILE:
		case CIV_APPROACH_DECEPTIVE:
		case CIV_APPROACH_GUARDED:
			iItemValue += 0;
			break;
		case CIV_APPROACH_AFRAID:
			iItemValue += 200;
			break;
		case CIV_APPROACH_FRIENDLY:
			iItemValue += 100;
			break;
		case CIV_APPROACH_NEUTRAL:
			iItemValue += 50;
			break;
		default:
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
		if (pDiploAI->GetNeighborOpinion(eOtherPlayer) == CIV_OPINION_ENEMY)
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

		// Does open borders make our production siphon trait stronger?
		if (MOD_TRAITS_TRADE_ROUTE_PRODUCTION_SIPHON && GetPlayer()->GetPlayerTraits()->IsTradeRouteProductionSiphon())
		{
			iItemValue += GetPlayer()->GetPlayerTraits()->GetTradeRouteProductionSiphon(true).m_iPercentIncreaseWithOpenBorders * 20;
			iItemValue += GetPlayer()->GetPlayerTraits()->GetTradeRouteProductionSiphon(false).m_iPercentIncreaseWithOpenBorders * 20;
		}

		if (!MOD_BALANCE_FLIPPED_TOURISM_MODIFIER_OPEN_BORDERS && (GetPlayer()->GetCulture()->GetTourism() / 100) > 0)
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
	}

	return max(iItemValue, GetOneGPTValue(false));
}

/// How much is a Defensive Pact worth?
int CvDealAI::GetDefensivePactValue(bool bFromMe, PlayerTypes eOtherPlayer)
{
	if (!GetPlayer()->GetDiplomacyAI()->IsWantsDefensivePactWithPlayer(eOtherPlayer))
		return INT_MAX;

	int iDefensivePactValue = GetPlayer()->GetDiplomacyAI()->ScoreDefensivePactChoice(eOtherPlayer, GetPlayer()->GetNumEffectiveCoastalCities() > 1);

	if (MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
		iDefensivePactValue *= max(1, (int)GetPlayer()->GetCurrentEra());

	int iItemValue = 0;
	bool bMostValuableAlly = GetPlayer()->GetDiplomacyAI()->GetMostValuableAlly() == eOtherPlayer;

	if (iDefensivePactValue > 0 && !bFromMe)
	{
		iItemValue = iDefensivePactValue;
		if (bMostValuableAlly)
			iItemValue *= 2;
	}
	else if (iDefensivePactValue < 0 && bFromMe)
	{
		iItemValue = -iDefensivePactValue;
		if (!bMostValuableAlly)
			iItemValue *= 2;
	}

	return iItemValue;
}

/// How much is a Research Agreement worth?
int CvDealAI::GetResearchAgreementValue(bool bFromMe, PlayerTypes eOtherPlayer)
{
	CvAssertMsg(GetPlayer()->GetID() != eOtherPlayer, "DEAL_AI: Trying to check value of a Research Agreement with oneself.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iItemValue = 100;

	if (!GetPlayer()->GetDiplomacyAI()->IsWantsResearchAgreementWithPlayer(eOtherPlayer))
		return INT_MAX;

	if (bFromMe)
	{
		// if they are ahead of me in tech by one or more eras ratchet up the value since they are more likely to get a good tech than I am
		EraTypes eMyEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();
		EraTypes eTheirEra = GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetCurrentEra();

		int iAdditionalValue = iItemValue * max(0,(int)(eTheirEra-eMyEra));
		iItemValue += iAdditionalValue;

		// Approach is important
		switch (GetPlayer()->GetDiplomacyAI()->GetSurfaceApproach(eOtherPlayer))
		{
		case CIV_APPROACH_WAR:
			iItemValue *= 100;
			break;
		case CIV_APPROACH_HOSTILE:
			iItemValue *= 1000;
			break;
		case CIV_APPROACH_GUARDED:
			iItemValue *= 100;
			break;
		case CIV_APPROACH_DECEPTIVE:
			iItemValue *= 100;
			break;
		case CIV_APPROACH_AFRAID:
			iItemValue *= 80;
			break;
		case CIV_APPROACH_FRIENDLY:
			iItemValue *= 100;
			break;
		case CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			break;
		default:
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}

	if (!bFromMe && GetPlayer()->GetRAToVotes() > 0)
	{
		iItemValue *= 5;
	}

	return iItemValue;
}

/// How much is a Peace Treaty worth?
int CvDealAI::GetPeaceTreatyValue(PlayerTypes eOtherPlayer)
{
	DEBUG_VARIABLE(eOtherPlayer);
	return 20;
}

/// What is the value of peace with eWithTeam? NOTE: This deal item should be disabled if eWithTeam doesn't want to go to peace
int CvDealAI::GetThirdPartyPeaceValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam, bool bLogging)
{
	int iItemValue = 100; //just some base value

	if (bLogging)
		return iItemValue;

	CvDiplomacyAI* pDiploAI = GetPlayer()->GetDiplomacyAI();
	PlayerTypes eWithPlayer = GET_TEAM(eWithTeam).getLeaderID();
	bool bMinor = GET_PLAYER(eWithPlayer).isMinorCiv();

	// Minor
	if (bMinor)
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
	}

	// if we're at war with the opponent, then this must be a peace deal. In this case we should evaluate vassal civ peace deals as zero
	if (GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsVassal(GET_PLAYER(eOtherPlayer).getTeam()) || GET_TEAM(GET_PLAYER(eWithPlayer).getTeam()).IsVassal(GetPlayer()->getTeam()))
	{
		if(GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam()))
		{
			return 0;
		}
	}

	// Denouncement in either direction?
	if (pDiploAI->IsDenouncedPlayer(eOtherPlayer) || pDiploAI->IsDenouncedByPlayer(eOtherPlayer))
		return INT_MAX;

	// No peace deals with backstabbers - it's a trap!
	if (pDiploAI->IsUntrustworthy(eOtherPlayer))
		return INT_MAX;

	EraTypes eOurEra = GET_TEAM(GetPlayer()->getTeam()).GetCurrentEra();

	// From me
	if(bFromMe)
	{
		//Our war score with the player
		int iWarScore = pDiploAI->GetWarScore(eWithPlayer);

		iItemValue += (iWarScore * 15);

		// Add 50 gold per era
		int iExtraCost = eOurEra * 50;
		iItemValue += iExtraCost;

		// Minors
		if (bMinor)
		{
			CivApproachTypes eMinorApproachTowardsWarPlayer = pDiploAI->GetCivApproach(eWithPlayer);
			if (eMinorApproachTowardsWarPlayer == CIV_APPROACH_WAR)
			{
				return INT_MAX;
			}
			else if (eMinorApproachTowardsWarPlayer == CIV_APPROACH_HOSTILE)
			{
				iItemValue += 200;
			}
		}
		// Majors
		else
		{
			// Captured our capital or Holy City? Close to winning the game? Don't accept peace bribes.
			if (pDiploAI->IsCapitalCapturedBy(eWithPlayer) || pDiploAI->IsHolyCityCapturedBy(eWithPlayer) || pDiploAI->IsCloseToWorldConquest() || pDiploAI->IsEndgameAggressiveTo(eWithPlayer))
			{
				return INT_MAX;
			}

			CivApproachTypes eWarMajorCivApproach = pDiploAI->GetCivApproach(eWithPlayer);
			// Modify for our feelings towards the player we're at war with
			if (eWarMajorCivApproach <= CIV_APPROACH_HOSTILE)
			{
				iItemValue *= 500;
				iItemValue /= 100;
			}
			else if (eWarMajorCivApproach == CIV_APPROACH_DECEPTIVE)
			{
				iItemValue *= 250;
				iItemValue /= 100;
			}
			else if (eWarMajorCivApproach == CIV_APPROACH_GUARDED)
			{
				iItemValue *= 150;
				iItemValue /= 100;
			}
			
			// If we want them conquered, let's be much more reluctant to accept peace bribes.
			bool bWantsConquest = (pDiploAI->IsGoingForWorldConquest() || pDiploAI->GetCivOpinion(eWithPlayer) == CIV_OPINION_UNFORGIVABLE);
			if (bWantsConquest && eWarMajorCivApproach == CIV_APPROACH_WAR)
			{
				iItemValue *= 3;
			}

			// Major competitor?
			if (pDiploAI->GetBiggestCompetitor() == eWithPlayer && eWarMajorCivApproach <= CIV_APPROACH_HOSTILE)
			{
				iItemValue *= 2;
			}
			else if (pDiploAI->IsMajorCompetitor(eWithPlayer) && eWarMajorCivApproach == CIV_APPROACH_WAR)
			{
				iItemValue *= 150;
				iItemValue /= 100;
			}
		}

		// Modify for our feelings towards the asking player
		switch (pDiploAI->GetSurfaceApproach(eOtherPlayer))
		{
		case CIV_APPROACH_WAR:
		case CIV_APPROACH_HOSTILE:
			iItemValue *= 600;
			break;
		case CIV_APPROACH_DECEPTIVE:
			iItemValue *= 150;
			break;
		case CIV_APPROACH_GUARDED:
		case CIV_APPROACH_NEUTRAL:
			iItemValue *= 125;
			break;
		case CIV_APPROACH_AFRAID:
		case CIV_APPROACH_FRIENDLY:
			iItemValue *= 75;
			break;
		}

		iItemValue /= 100;
		
		// Not an easy target? Halve the value.
		if (!pDiploAI->IsEasyTarget(eWithPlayer))
		{
			iItemValue /= 2;
		}
	}
	// From them
	else
	{
		//Their war score with the player
		int iTheirWarScore = GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->GetWarScore(eWithPlayer);

		iItemValue += (iTheirWarScore * 5);

		// Add 25 gold per era
		int iExtraCost = eOurEra * 25;
		iItemValue += iExtraCost;

		if (!GET_PLAYER(eWithPlayer).isMinorCiv())
		{
			CivApproachTypes eWarMajorCivApproach = pDiploAI->GetCivApproach(eWithPlayer);

			// Modify for our feelings towards the player they're at war with
			if (eWarMajorCivApproach == CIV_APPROACH_FRIENDLY)
			{
				iItemValue *= 200;
				iItemValue /= 100;
			}
			else if (eWarMajorCivApproach == CIV_APPROACH_NEUTRAL)
			{
				iItemValue *= 50;
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

			if (GET_PLAYER(eWithPlayer).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()) || GET_PLAYER(eWithPlayer).GetMinorCivAI()->IsAllies(GetPlayer()->GetID()))
			{
				iItemValue *= 300;
				iItemValue /= 100;
			}
			else if (pDiploAI->GetCivApproach(eWithPlayer) == CIV_APPROACH_FRIENDLY)
			{
				iItemValue *= 150;
				iItemValue /= 100;
			}
			else
			{
				return INT_MAX;
			}
		}
		
		CivApproachTypes eAskingMajorCivApproach = pDiploAI->GetSurfaceApproach(eOtherPlayer);

		// Modify for our feelings towards the asking player
		switch (eAskingMajorCivApproach)
		{
		case CIV_APPROACH_WAR:
		case CIV_APPROACH_HOSTILE:
			return INT_MAX;
			break;
		case CIV_APPROACH_DECEPTIVE:
			iItemValue *= 150;
			break;
		case CIV_APPROACH_GUARDED:
		case CIV_APPROACH_AFRAID:
		case CIV_APPROACH_NEUTRAL:
			iItemValue *= 125;
			break;
		case CIV_APPROACH_FRIENDLY:
			iItemValue *= 200;
			break;
		}

		iItemValue /= 100;
		
		// Not an easy target? Halve the value.
		if (!GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsEasyTarget(eWithPlayer))
		{
			iItemValue /= 2;
		}
	}


	return iItemValue;
}

/// What is the value of war with eWithPlayer?
int CvDealAI::GetThirdPartyWarValue(bool bFromMe, PlayerTypes eOtherPlayer, TeamTypes eWithTeam, bool /*bLogging*/)
{
	PlayerTypes ePlayerDeclaringWar = bFromMe ? GetPlayer()->GetID() : eOtherPlayer;
	CvPlayer& kPlayerDeclaringWar = GET_PLAYER(ePlayerDeclaringWar);
	TeamTypes eTeamDeclaringWar = kPlayerDeclaringWar.getTeam();
	PlayerTypes eWithPlayer = GET_TEAM(eWithTeam).getLeaderID();
	CvCity* pCapital = kPlayerDeclaringWar.getCapitalCity();
	if (!pCapital)
		return INT_MAX;

	// No deals if there is a denouncement in either direction
	CvDiplomacyAI* pOurDiploAI = GetPlayer()->GetDiplomacyAI();
	if (pOurDiploAI->IsDenouncedPlayer(eOtherPlayer) || pOurDiploAI->IsDenouncedByPlayer(eOtherPlayer))
		return INT_MAX;

	// No war deals with backstabbers - it's a trap!
	if (pOurDiploAI->IsUntrustworthy(eOtherPlayer))
		return INT_MAX;

	// Friendly towards Minor Civ?
	CvDiplomacyAI* pDiploAI = kPlayerDeclaringWar.GetDiplomacyAI();
	CivApproachTypes eMajorApproachTowardsWarPlayer = pDiploAI->GetCivApproach(eWithPlayer);
	if (GET_PLAYER(eWithPlayer).isMinorCiv())
	{
		if (eMajorApproachTowardsWarPlayer == CIV_APPROACH_FRIENDLY)
			return INT_MAX;
	}
	// Afraid of Major Civ?
	else if (GET_PLAYER(eWithPlayer).isMajorCiv())
	{
		if (eMajorApproachTowardsWarPlayer == CIV_APPROACH_AFRAID)
			return INT_MAX;
	}

	// If we're in bad shape to start a war, no wars
	if (kPlayerDeclaringWar.IsNoNewWars())
		return INT_MAX;

	// Don't get distracted by bribed wars when we're close to winning
	if (pDiploAI->IsCloseToAnyVictoryCondition())
		return INT_MAX;

	//do we even have a target to attack?
	if (!kPlayerDeclaringWar.GetMilitaryAI()->HavePreferredAttackTarget(eWithPlayer))
		return INT_MAX;

	//don't get into additional wars if we cannot afford it
	if (kPlayerDeclaringWar.IsAtWarAnyMajor() && pDiploAI->GetStateAllWars() != STATE_ALL_WARS_WINNING)
		return INT_MAX;

	// Player must be a potential war target
	if (GET_PLAYER(eWithPlayer).isMajorCiv() && !pDiploAI->IsPotentialWarTarget(eWithPlayer))
		return INT_MAX;

	// Would this war cause us or our teammates to backstab a friend/ally? Don't do it!
	if (!pDiploAI->IsWarSane(eWithPlayer))
		return INT_MAX;

	// Can't be too far away
	if (kPlayerDeclaringWar.GetProximityToPlayer(eWithPlayer) < PLAYER_PROXIMITY_CLOSE)
		return INT_MAX;

	// Don't accept war bribes if we recently made peace.
	if (pDiploAI->GetNumWarsFought(eWithPlayer) > 0)
	{
		int iPeaceTreatyTurn = GET_TEAM(eTeamDeclaringWar).GetTurnMadePeaceTreatyWithTeam(eWithTeam);
		if (iPeaceTreatyTurn > -1)
		{
			int iTurnsSincePeace = GC.getGame().getGameTurn() - iPeaceTreatyTurn;
			int iPeaceDampenerTurns = 23 + GC.getGame().randRangeExclusive(0, 15, kPlayerDeclaringWar.GetPseudoRandomSeed());
			if (iTurnsSincePeace < iPeaceDampenerTurns)
				return INT_MAX;
		}
	}

	// AI sanity check - who else would we go to war with?
	if (!kPlayerDeclaringWar.isHuman())
	{
		CivsList vDefensiveWarAllies = pDiploAI->GetDefensiveWarAllies(eWithPlayer, /*bIncludeMinors*/ true, /*bReverseMode*/ true, /*bNewWarsOnly*/ true);
		for (CivsList::iterator it = vDefensiveWarAllies.begin(); it != vDefensiveWarAllies.end(); ++it)
		{
			// Would we be declaring war on a powerful neighbor?
			CvPlayer& kDefensiveWarAlly = GET_PLAYER(*it);
			if (kDefensiveWarAlly.GetProximityToPlayer(ePlayerDeclaringWar) >= PLAYER_PROXIMITY_CLOSE)
			{
				if (kDefensiveWarAlly.isMajorCiv())
				{
					if (pDiploAI->GetCivApproach(*it) == CIV_APPROACH_AFRAID)
					{
						return INT_MAX;
					}
					// If we're already planning a war/demand against them, then we don't care.
					else if (pDiploAI->GetCivApproach(*it) != CIV_APPROACH_WAR && pDiploAI->GetDemandTargetPlayer() != *it)
					{
						if (pDiploAI->GetMilitaryStrengthComparedToUs(*it) > STRENGTH_AVERAGE)
						{
							return INT_MAX;
						}
					}

					// Anti-stupidity: Is this guy not sane to attack (e.g. friends, coop war, DP etc)? Then don't accept a bribe...indirect backstabbing should never be because of a bribe.
					if (!pDiploAI->IsWarSane(*it))
						return INT_MAX;
				}
				else if (pDiploAI->GetCivApproach(*it) > CIV_APPROACH_HOSTILE)
				{
					if (pDiploAI->GetMilitaryStrengthComparedToUs(*it) > STRENGTH_AVERAGE)
					{
						return INT_MAX;
					}
				}
			}
		}
	}

	// Already planning a coop war against the target? then we aren't interested! (reduce the chance of a premature declaration...)
	vector<PlayerTypes> vMyTeam = GET_TEAM(kPlayerDeclaringWar.getTeam()).getPlayers();
	for (size_t i=0; i<vMyTeam.size(); i++)
	{
		if (!GET_PLAYER(vMyTeam[i]).isAlive() || !GET_PLAYER(vMyTeam[i]).isMajorCiv())
			continue;

		if (GET_PLAYER(vMyTeam[i]).GetDiplomacyAI()->GetGlobalCoopWarAgainstState(eWithPlayer) == COOP_WAR_STATE_PREPARING)
			return INT_MAX;
	}

	// What does a basic unit cost these days, use that as a base
	UnitTypes eUnit = kPlayerDeclaringWar.GetCompetitiveSpawnUnitType(true, true, true, true);
	int iItemValue = eUnit != NO_UNIT ? pCapital->GetPurchaseCost(eUnit) : 100;

	// Scale with WarmongerHate flavor
	// Rationale: If the AI hates warmongers, they'll require more in order to go to war, and they'll also pay more to have others wage their battles
	iItemValue *= bFromMe ? GetPlayer()->GetDiplomacyAI()->GetWarmongerHate() : GetPlayer()->GetDiplomacyAI()->GetWarmongerHate() / 2;

	if (!kPlayerDeclaringWar.isHuman())
	{
		if (pDiploAI->GetBiggestCompetitor() == eWithPlayer)
		{
			iItemValue /= 2;
		}
		else if (pDiploAI->IsMajorCompetitor(eWithPlayer))
		{
			iItemValue *= 75;
			iItemValue /= 100;
		}
		// If not against a major competitor, don't accept if we don't like the other guy
		else if (pDiploAI->GetCivApproach(eOtherPlayer) < CIV_APPROACH_AFRAID)
		{
			return INT_MAX;
		}
	}

	// Modify for our feelings towards the player we're would go to war with
	if (eMajorApproachTowardsWarPlayer == CIV_APPROACH_WAR)
	{
		iItemValue *= 75;
		iItemValue /= 100;
	}
	else if (eMajorApproachTowardsWarPlayer <= CIV_APPROACH_HOSTILE)
	{
		iItemValue *= 90;
		iItemValue /= 100;
	}
	else
	{
		iItemValue *= 125;
		iItemValue /= 100;
	}

	return iItemValue;
}

/// What is the value of trading a vote commitment?
int CvDealAI::GetVoteCommitmentValue(bool bFromMe, PlayerTypes eOtherPlayer, int iProposalID, int iVoteChoice, int iNumVotes, bool bRepeal)
{
	int iValue = 0;

	if (iNumVotes == 0)
		return INT_MAX;

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (!pLeague)
		return INT_MAX;

	int iDealDuration = GC.getGame().getGameSpeedInfo().GetDealDuration();

	// Giving our votes to them - Higher value for voting on things we dislike, lower value for voting on things we like
	if (bFromMe)
	{
		iValue += 2 * iDealDuration;

		// Adjust based on how favorable this proposal is for us
		CvLeagueAI::DesireLevels eDesire = GetPlayer()->GetLeagueAI()->EvaluateVoteForTrade(iProposalID, iVoteChoice, iNumVotes, bRepeal);
		switch (eDesire)
		{
		case CvLeagueAI::DESIRE_NEVER:
			return INT_MAX;
			break;
		case CvLeagueAI::DESIRE_STRONG_DISLIKE:
			iValue += 100 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_DISLIKE:
			iValue += 60 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_WEAK_DISLIKE:
			iValue += 40 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_NEUTRAL:
			iValue += 30 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_WEAK_LIKE:
			iValue += 20 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_LIKE:
			iValue += 10 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_STRONG_LIKE:
			iValue += 5 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_ALWAYS:
			iValue += 2 * iNumVotes * iDealDuration;
			break;
		default:
			iValue += 30 * iNumVotes * iDealDuration;
			break;
		}
		// Adjust based on how much this player's World Congress interests align with ours
		CvLeagueAI::AlignmentLevels eAlignment = GetPlayer()->GetLeagueAI()->EvaluateAlignment(eOtherPlayer);
		switch (eAlignment)
		{
		case CvLeagueAI::ALIGNMENT_LIBERATOR:
		case CvLeagueAI::ALIGNMENT_LEADER:
		case CvLeagueAI::ALIGNMENT_TEAMMATE:
			iValue *= 1;
			break;
		case CvLeagueAI::ALIGNMENT_ALLY:
			iValue *= 110;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_CONFIDANT:
			iValue *= 120;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_FRIEND:
			iValue *= 130;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_NEUTRAL:
			iValue *= 150;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_RIVAL:
			iValue *= 200;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_HATRED:
			iValue *= 250;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_ENEMY:
			iValue *= 300;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_WAR:
			iValue *= 500;
			iValue /= 100;
			break;
		default:
			iValue *= 150;
			iValue /= 100;
			break;
		}
		if (bRepeal)
		{
			CvRepealProposal* pProposal = pLeague->GetRepealProposal(iProposalID);
			if (pProposal != NULL)
			{
				PlayerTypes eTargetPlayer = NO_PLAYER;
				ResolutionDecisionTypes eProposerDecision = pProposal->GetProposerDecision()->GetType();
				if (eProposerDecision == RESOLUTION_DECISION_ANY_MEMBER ||
					eProposerDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
					eProposerDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
				{
					eTargetPlayer = (PlayerTypes)pProposal->GetProposerDecision()->GetDecision();
					// They shouldn't ask us to vote on things that have to do with us personally.
					if (eTargetPlayer != NO_PLAYER && GET_PLAYER(eTargetPlayer).getTeam() == GetPlayer()->getTeam())
					{
						return INT_MAX;
					}
				}
			}
		}
		else
		{
			CvEnactProposal* pProposal = pLeague->GetEnactProposal(iProposalID);
			if (pProposal != NULL)
			{
				// Is this the World Leader vote?
				if (pProposal->GetEffects()->bDiplomaticVictory)
				{
					int iOurVotes = pLeague->CalculateStartingVotesForMember(GetPlayer()->GetID());
					int iNeededVotes = GC.getGame().GetVotesNeededForDiploVictory();
					int iOurPercent = (iOurVotes * 100) / max(1, iNeededVotes);
					if (iOurPercent >= 50)
					{
						return INT_MAX;
					}
					else
					{
						iValue *= 20;
					}
				}
				else
				{
					PlayerTypes eTargetPlayer = NO_PLAYER;
					ResolutionDecisionTypes eProposerDecision = pProposal->GetProposerDecision()->GetType();
					if (eProposerDecision == RESOLUTION_DECISION_ANY_MEMBER ||
						eProposerDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
						eProposerDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
					{
						eTargetPlayer = (PlayerTypes)pProposal->GetProposerDecision()->GetDecision();
						// They shouldn't ask us to vote on things that have to do with us personally.
						if (eTargetPlayer != NO_PLAYER && GET_PLAYER(eTargetPlayer).getTeam() == GetPlayer()->getTeam())
						{
							return INT_MAX;
						}
					}
				}
			}
		}
	}
	// Giving their votes to something we want - Higher value for voting on things we like
	else
	{
		// Adjust based on how favorable this proposal is for us
		CvLeagueAI::DesireLevels eDesire = GetPlayer()->GetLeagueAI()->EvaluateVoteForTrade(iProposalID, iVoteChoice, iNumVotes, bRepeal);
		switch (eDesire)
		{
		case CvLeagueAI::DESIRE_NEVER:
		case CvLeagueAI::DESIRE_STRONG_DISLIKE:
		case CvLeagueAI::DESIRE_DISLIKE:
		case CvLeagueAI::DESIRE_WEAK_DISLIKE:	
		case CvLeagueAI::DESIRE_NEUTRAL:
			// Things we don't want to see happen are worth 0.
			break;
		case CvLeagueAI::DESIRE_WEAK_LIKE:
			iValue += 2 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_LIKE:
			iValue += 4 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_STRONG_LIKE:
			iValue += 10 * iNumVotes * iDealDuration;
			break;
		case CvLeagueAI::DESIRE_ALWAYS:
			iValue += 20 * iNumVotes * iDealDuration;
			break;
		default:
			break;
		}
		// Adjust based on how much this player's World Congress interests align with ours
		CvLeagueAI::AlignmentLevels eAlignment = GetPlayer()->GetLeagueAI()->EvaluateAlignment(eOtherPlayer);
		switch (eAlignment)
		{
		case CvLeagueAI::ALIGNMENT_LIBERATOR:
		case CvLeagueAI::ALIGNMENT_LEADER:
		case CvLeagueAI::ALIGNMENT_TEAMMATE:
			iValue *= 120;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_ALLY:
			iValue *= 110;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_CONFIDANT:
		case CvLeagueAI::ALIGNMENT_FRIEND:
		case CvLeagueAI::ALIGNMENT_NEUTRAL:
			break;
		case CvLeagueAI::ALIGNMENT_RIVAL:
			iValue *= 90;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_HATRED:
			iValue *= 80;
			iValue /= 100;
			break;
		case CvLeagueAI::ALIGNMENT_ENEMY:
		case CvLeagueAI::ALIGNMENT_WAR:
			iValue *= 70;
			iValue /= 100;
			break;
		default:
			break;
		}

		if (bRepeal)
		{
			CvRepealProposal* pProposal = pLeague->GetRepealProposal(iProposalID);
			if (pProposal != NULL)
			{
				PlayerTypes eTargetPlayer = NO_PLAYER;
				ResolutionDecisionTypes eProposerDecision = pProposal->GetProposerDecision()->GetType();
				if (eProposerDecision == RESOLUTION_DECISION_ANY_MEMBER ||
					eProposerDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
					eProposerDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
				{
					eTargetPlayer = (PlayerTypes)pProposal->GetProposerDecision()->GetDecision();
					//we don't ask them about things that involve themselves!
					if (eTargetPlayer != NO_PLAYER && GET_PLAYER(eTargetPlayer).getTeam() == GET_PLAYER(eOtherPlayer).getTeam())
					{
						return INT_MAX;
					}
				}
			}
		}
		else
		{
			CvEnactProposal* pProposal = pLeague->GetEnactProposal(iProposalID);
			if (pProposal != NULL)
			{
				if (pProposal->GetEffects()->bDiplomaticVictory)
				{
					PlayerTypes eLeader = GET_TEAM(GetPlayer()->getTeam()).getLeaderID();
					if (eLeader == NO_PLAYER)
						return INT_MAX;

					int iOurVotes = pLeague->CalculateStartingVotesForMember(eLeader);
					int iNeededVotes = GC.getGame().GetVotesNeededForDiploVictory();
					PlayerTypes eChoicePlayer = (PlayerTypes)iVoteChoice;

					if (iOurVotes + iNumVotes < iNeededVotes || eChoicePlayer != eLeader)
					{
						return INT_MAX;
					}
					else
					{
						iValue *= 10;
					}
				}
				else
				{
					PlayerTypes eTargetPlayer = NO_PLAYER;
					ResolutionDecisionTypes eProposerDecision = pProposal->GetProposerDecision()->GetType();
					if (eProposerDecision == RESOLUTION_DECISION_ANY_MEMBER ||
						eProposerDecision == RESOLUTION_DECISION_MAJOR_CIV_MEMBER ||
						eProposerDecision == RESOLUTION_DECISION_OTHER_MAJOR_CIV_MEMBER)
					{
						eTargetPlayer = (PlayerTypes)pProposal->GetProposerDecision()->GetDecision();
						//we don't ask them about things that involve themselves!
						if (eTargetPlayer != NO_PLAYER && GET_PLAYER(eTargetPlayer).getTeam() == GET_PLAYER(eOtherPlayer).getTeam())
						{
							return INT_MAX;
						}
					}
				}
			}
		}
	}

	iValue = MAX(iValue, 0);

	return iValue;
}

/// See if adding a Vote Commitment to their side of the deal helps even out pDeal
void CvDealAI::DoAddVoteCommitmentToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
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

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
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
					int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ false, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, true);

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
			viTradeValues.StableSortItems();
			if(viTradeValues.size() > 0)
			{
				for(int iRanking = 0; iRanking < viTradeValues.size(); iRanking++)
				{
					int iWeight = viTradeValues.GetWeight(iRanking);
					int ID = viTradeValues.GetElement(iRanking);
					if(iWeight != 0)
					{
						CvLeagueAI::VoteCommitmentList vDesiredCommitments = GetPlayer()->GetLeagueAI()->GetDesiredVoteCommitments(eThem);
						for (CvLeagueAI::VoteCommitmentList::iterator it = vDesiredCommitments.begin(); it != vDesiredCommitments.end(); ++it)
						{
							int iProposalID = it->iResolutionID;
							int iVoteChoice = it->iVoteChoice;
							int iNumVotes = it->iNumVotes;
							bool bRepeal = !it->bEnact;
							if(iProposalID == ID)
							{
								if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iWeight, false))
								{
									pDeal->AddVoteCommitment(eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, false);
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
void CvDealAI::DoAddVoteCommitmentToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if(pLeague == NULL || !pLeague)
	{
		return;
	}

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		// Can't already be a Vote Commitment in the Deal
		if(!pDeal->IsVoteCommitmentTrade(eThem) && !pDeal->IsVoteCommitmentTrade(eMyPlayer))
		{
			if (!GetPlayer()->GetLeagueAI()->CanCommitVote(eThem))
			{
				return;
			}
			CvLeagueAI::VoteCommitmentList vDesiredCommitments;
			vDesiredCommitments = GET_PLAYER(eThem).GetLeagueAI()->GetDesiredVoteCommitments(eMyPlayer);
			
			CvWeightedVector<int> viTradeValues;
			for (CvLeagueAI::VoteCommitmentList::iterator it = vDesiredCommitments.begin(); it != vDesiredCommitments.end(); ++it)
			{
				int iProposalID = it->iResolutionID;
				int iVoteChoice = it->iVoteChoice;
				int iNumVotes = it->iNumVotes;
				bool bRepeal = !it->bEnact;

				if (iProposalID != -1 && pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_VOTE_COMMITMENT, iProposalID, iVoteChoice, iNumVotes, bRepeal))
				{
					int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ true, eThem, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, true);

					// If adding this to the deal doesn't take it under the min limit, do it
					if (iItemValue == INT_MAX)
					{
						continue;
					}

					viTradeValues.push_back(iProposalID, iItemValue);
				}
			}
			// Sort the vector based on value to get the best possible item to trade.
			viTradeValues.StableSortItems();
			if (viTradeValues.size() > 0)
			{
				//reverse!
				for (int iRanking = viTradeValues.size() - 1; iRanking >= 0; iRanking--)
				{
					int iWeight = viTradeValues.GetWeight(iRanking);
					int ID = viTradeValues.GetElement(iRanking);
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
								if (iProposalID == ID)
								{
									if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iWeight, true))
									{
										pDeal->AddVoteCommitment(eMyPlayer, iProposalID, iVoteChoice, iNumVotes, bRepeal, false);
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
void CvDealAI::DoAddThirdPartyWarToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Them, but them is us. Please send Anton your save file and version.");

	// Not allowed in demands.
	if (pDeal->GetDemandingPlayer() != NO_PLAYER)
		return;
	

	CvWeightedVector<int> viTradeValues;

	
	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
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
						int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_WAR, /*bFromMe*/ false, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, true);

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
		viTradeValues.StableSortItems();
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

					if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iWeight, false))
					{
						pDeal->AddThirdPartyWar(eThem, eOtherTeam, false);
						iTotalValue = GetDealValue(pDeal);
						return;
					}
									
				}
			}
		}
	}
}

/// See if adding 3rd Party War to our side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyWarToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");
	
	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
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
						int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_WAR, /*bFromMe*/ true, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, true);

						// If adding this to the deal doesn't take it over the limit, do it
						if(iItemValue==INT_MAX)
						{
							continue;
						}
						if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
						{
							pDeal->AddThirdPartyWar(eMyPlayer, eOtherTeam, false);
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
void CvDealAI::DoAddThirdPartyPeaceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Them, but them is us. Please send Anton your save file and version.");

	CvWeightedVector<int> viTradeValues;


	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
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
						int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_PEACE, /*bFromMe*/ false, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, true);

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
		viTradeValues.StableSortItems();
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

					if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iWeight, false))
					{
						pDeal->AddThirdPartyWar(eThem, eOtherTeam, false);
						iTotalValue = GetDealValue(pDeal);
						return;
					}								
				}
			}
		}
	}
}

/// See if adding a 3rd Party Peace deal to our side of the deal helps even out pDeal
void CvDealAI::DoAddThirdPartyPeaceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS)
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vote Commitment to Us, but them is us. Please send Anton your save file and version.");

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
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
						int iItemValue = GetTradeItemValue(TRADE_ITEM_THIRD_PARTY_PEACE, /*bFromMe*/ true, eThem, eOtherTeam, -1, -1, /*bFlag1*/false, -1, true);

						// If adding this to the deal doesn't take it over the limit, do it
						if(iItemValue==INT_MAX)
						{
							continue;
						}
						if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
						{
							pDeal->AddThirdPartyWar(eMyPlayer, eOtherTeam, false);
							iTotalValue = GetDealValue(pDeal);
						}
					}
				}
			}
		}
	}
}

/// See if adding a Resource to their side of the deal helps even out pDeal
void CvDealAI::DoAddLuxuryResourceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Them, but them is us.  Please show Jon");

	CvWeightedVector<int> viTradeValues;

	int iDealDuration = pDeal->GetDuration();
	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

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
				int iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration, true);

				if (iItemValue == INT_MAX)
				{
					continue;
				}
				else if (iItemValue > 0 && GET_PLAYER(eMyPlayer).GetPlayerTraits()->IsImportsCountTowardsMonopolies())
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
					viTradeValues.push_back(eResource, iItemValue);
				}
			}
		}
		// Sort the vector based on value to get the best possible item to trade.
		viTradeValues.StableSortItems();
		if (viTradeValues.size() > 0)
		{
			for (int iRanking = 0; iRanking < viTradeValues.size(); iRanking++)
			{
				int iWeight = viTradeValues.GetWeight(iRanking);
				if (iWeight != 0)
				{
					ResourceTypes eResource = (ResourceTypes)viTradeValues.GetElement(iRanking);
					int iResourceQuantity = 1;

					if (pDeal->GetDemandingPlayer() != NO_PLAYER || !TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iWeight, false))
					{
						// Try to change the current item if it already exists, otherwise add it
						if (!pDeal->ChangeResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eThem, eResource, iResourceQuantity, iDealDuration, false);
							iTotalValue = GetDealValue(pDeal);
						}
					}
				}
			}
		}
	}
}

/// See if adding a Resource to our side of the deal helps even out pDeal
void CvDealAI::DoAddLuxuryResourceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Us, but them is us.  Please show Jon");

	CvWeightedVector<int> viTradeValues;


	int iDealDuration = pDeal->GetDuration();
	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		int iItemValue = 0;

		ResourceTypes eResource;
		int iResourceQuantity = 0;

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
				iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ true, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, iDealDuration, true);

				// If adding this to the deal doesn't take it under the min limit, do it
				if (iItemValue == INT_MAX)
				{
					continue;
				}

				viTradeValues.push_back(eResource, iItemValue);
			}
		}

		viTradeValues.StableSortItems();
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

					if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iWeight, true))
					{
						// Try to change the current item if it already exists, otherwise add it
						if (!pDeal->ChangeResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration))
						{
							pDeal->AddResourceTrade(eMyPlayer, eResource, iResourceQuantity, iDealDuration, false);
							iTotalValue = GetDealValue(pDeal);
						}
					}
				}
			}
		}
	}
}

/// See if adding a Resource to their side of the deal helps even out pDeal
void CvDealAI::DoAddStrategicResourceToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Them, but them is us.  Please show Jon");

	typedef pair<ResourceTypes, int> TradeItem;
	vector<OptionWithScore<TradeItem>> vOptions;

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
	{
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
			if (!GET_PLAYER(eThem).isHuman() && GET_PLAYER(eThem).getResourceImportFromMajor(eResource) > 0)
				continue;

			// Don't bother looking at this resource if they don't even have any of it
			int iMaxResourceQuantity = GET_PLAYER(eThem).getNumResourceAvailable(eResource, false);
			if (iMaxResourceQuantity < 1)
				continue;

			// Don't try to buy all of it
			int iAlreadyInDeal = pDeal->GetNumResourcesInDeal(eThem, eResource);
			if (iAlreadyInDeal > 0)
			{
				iMaxResourceQuantity = min(iAlreadyInDeal + 1, iMaxResourceQuantity);
			}
			else
			{
				iMaxResourceQuantity = max((iMaxResourceQuantity / 3), 1);
			}

			int iResourceQuantity = iAlreadyInDeal;
			// Don't buy more than we're willing to pay for
			int iPrevQuantityValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iAlreadyInDeal, -1, /*bFlag1*/false, pDeal->GetDuration(), true);
			int iNextQuantityValue = 0;
			for (int iResourceQuantityLoop = iAlreadyInDeal + 1; iResourceQuantityLoop <= iMaxResourceQuantity; iResourceQuantityLoop++)
			{
				if (!pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_RESOURCES, eResource, iResourceQuantityLoop))
				{
					// can't trade that many of this item
					iResourceQuantity = iResourceQuantityLoop - 1;
					break;
				}
				iNextQuantityValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iResourceQuantityLoop, -1, /*bFlag1*/false, pDeal->GetDuration(), true);
				if (iNextQuantityValue == iPrevQuantityValue || iNextQuantityValue == INT_MAX)
				{
					// we're not willing to pay more if we add one resource item. no need to check further
					iResourceQuantity = iResourceQuantityLoop - 1;
					break;
				}
				else
				{
					iPrevQuantityValue = iNextQuantityValue;
					iResourceQuantity = iResourceQuantityLoop;
				}
			}
			if(iResourceQuantity > iAlreadyInDeal)
			{
				vOptions.push_back(OptionWithScore<TradeItem>(TradeItem(eResource, iResourceQuantity), GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ false, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, pDeal->GetDuration(), true)));
			}
		}

		std::stable_sort(vOptions.begin(), vOptions.end());
		for (size_t i = 0; i < vOptions.size(); i++)
		{
			ResourceTypes eResource = vOptions[i].option.first;
			int iQuantity = vOptions[i].option.second;
			int iScore = vOptions[i].score;

			if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iScore, false))
			{
				// Try to change the current item if it already exists, otherwise add it
				if (!pDeal->ChangeResourceTrade(eThem, eResource, iQuantity, pDeal->GetDuration()))
				{
					pDeal->AddResourceTrade(eThem, eResource, iQuantity, pDeal->GetDuration(), false);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

/// See if adding a Resource to our side of the deal helps even out pDeal
void CvDealAI::DoAddStrategicResourceToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Resource to Us, but them is us.  Please show Jon");

	typedef pair<ResourceTypes, int> TradeItem;
	vector<OptionWithScore<TradeItem>> vOptions;

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
	{

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
			int iMaxResourceQuantity = GET_PLAYER(eMyPlayer).getNumResourceAvailable(eResource, false) / 2;
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
				iResourceQuantity = min(iMaxResourceQuantity, iTheirResourceCount + 2);

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
				int iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, /*bFromMe*/ true, eThem, eResource, iResourceQuantity, -1, /*bFlag1*/false, pDeal->GetDuration(), true);
				if (iItemValue == INT_MAX)
					continue;

				vOptions.push_back(OptionWithScore<TradeItem>(TradeItem(eResource, iResourceQuantity), iItemValue));
			}
		}

		std::stable_sort(vOptions.begin(), vOptions.end());
		for (size_t i = 0; i < vOptions.size(); i++)
		{
			ResourceTypes eResource = vOptions[i].option.first;
			int iQuantity = vOptions[i].option.second;
			int iScore = vOptions[i].score;

			// If adding this to the deal doesn't take it under the min limit, do it
			if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iScore, true))
			{
				// Try to change the current item if it already exists, otherwise add it
				if (!pDeal->ChangeResourceTrade(eMyPlayer, eResource, iQuantity, pDeal->GetDuration()))
				{
					pDeal->AddResourceTrade(eMyPlayer, eResource, iQuantity, pDeal->GetDuration(), false);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}


/// See if adding Embassy to their side of the deal helps even out pDeal
void CvDealAI::DoAddEmbassyToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Embassy to Them, but them is us.  Please show Jon");

	// don't make a demand for an embassy
	if (pDeal->GetDemandingPlayer() != NO_PLAYER)
		return;

	if((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
	{
		if(!pDeal->IsAllowEmbassyTrade(eThem))
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if they can actually trade it to us
			if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_ALLOW_EMBASSY))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_ALLOW_EMBASSY, /*bFromMe*/ false, eThem, -1, -1, -1, /*bFlag1*/false, -1, true);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, false))
				{
					pDeal->AddAllowEmbassy(eThem, false);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

/// See if adding Embassy to our side of the deal helps even out pDeal
void CvDealAI::DoAddEmbassyToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Embassy to Us, but them is us.  Please show Jon");

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		if(!pDeal->IsAllowEmbassyTrade(eMyPlayer))
		{
			// See if we can actually trade it to them
			if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_ALLOW_EMBASSY))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_ALLOW_EMBASSY, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, -1, true);

				// If adding this to the deal doesn't take it under the min limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
				{
					pDeal->AddAllowEmbassy(eMyPlayer, false);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

/// See if adding Open Borders to their side of the deal helps even out pDeal
void CvDealAI::DoAddOpenBordersToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Them, but them is us.  Please show Jon");

	int iDealDuration = pDeal->GetDuration();
	
	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
	{
		if(!pDeal->IsOpenBordersTrade(eThem))
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if they can actually trade it to us
			if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_OPEN_BORDERS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_OPEN_BORDERS, /*bFromMe*/ false, eThem, -1, -1, -1, /*bFlag1*/false, iDealDuration, true);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, false))
				{
					pDeal->AddOpenBorders(eThem, iDealDuration, false);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

/// See if adding Open Borders to our side of the deal helps even out pDeal
void CvDealAI::DoAddOpenBordersToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iDealDuration = pDeal->GetDuration();

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
	{
		if (!pDeal->IsOpenBordersTrade(eMyPlayer))
		{
			// See if we can actually trade it to them
			if (pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_OPEN_BORDERS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_OPEN_BORDERS, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, iDealDuration, true);

				// If adding this to the deal doesn't take it under the min limit, do it
				if (iItemValue == INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
				{
					pDeal->AddOpenBorders(eMyPlayer, iDealDuration, false);
					iTotalValue = GetDealValue(pDeal);
				}
			}
		}
	}
}

/// See if adding Cities to our side of the deal helps even out pDeal
void CvDealAI::DoAddCitiesToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	if (!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eThem, TRADE_ITEM_CITIES))
		return;

	// If we're not the one surrendering here, don't bother
	if(pDeal->IsPeaceTreatyTrade(eThem) && pDeal->GetSurrenderingPlayer() != m_pPlayer->GetID())
		return;

	// AIs will only do city exchanges. Versus humans we only offer a city if they offer one too
	if (!pDeal->ContainsItemType(TRADE_ITEM_CITIES, eThem))
		return;

	// We don't owe them anything
	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
	{
		// Create vector of the losing players' Cities so we can see which are the closest to the winner
		CvWeightedVector<int> viCityPriceRatio;

		// Loop through all of our cities
		int iCityLoop = 0;
		CvPlayer* pSellingPlayer = GetPlayer();
		for(CvCity* pLoopCity = pSellingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pSellingPlayer->nextCity(&iCityLoop))
		{
			//make sure we're not just flipping two border towns
			CvCity* pTheirClosest = GET_PLAYER(eThem).GetClosestCityByPathLength(pLoopCity->plot());
			if (pTheirClosest && pDeal->IsCityTrade(eThem, pTheirClosest->getX(), pTheirClosest->getY()))
				continue;

			int iWhatTheyWouldPay = GetCityValueForDeal(pLoopCity, eThem);
			int iWhatIWouldPay = GetCityValueForDeal(pLoopCity, m_pPlayer->GetID());

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
		viCityPriceRatio.StableSortItems();

		// Loop through sorted Cities
		int iSortedCityID = 0;
		for (int iSortedCityIndex = 0; iSortedCityIndex < viCityPriceRatio.size(); iSortedCityIndex++)
		{
			iSortedCityID = viCityPriceRatio.GetElement(iSortedCityIndex);
			CvCity* pLoopCity = pSellingPlayer->getCity(iSortedCityID);

			// See if we can actually trade it to them
			if (pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eThem, TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
			{
				int iItemValue = GetCityValueForDeal(pLoopCity, eThem);
				if (iItemValue == INT_MAX)
					continue;

				// If adding this to the deal doesn't take it under the min limit, do it
				if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
				{
					OutputDebugString(CvString::format("Adding %s to deal. Seller %s, buyer %s, price ratio %d\n",
						pLoopCity->getName().c_str(), pSellingPlayer->getName(), GET_PLAYER(eThem).getName(), viCityPriceRatio.GetWeight(iSortedCityIndex)).c_str());
					pDeal->AddCityTrade(m_pPlayer->GetID(), iSortedCityID, false);
					iTotalValue = GetDealValue(pDeal);
					return;
				}
			}
		}
	}
}


/// See if adding Cities to their side of the deal helps even out pDeal
void CvDealAI::DoAddCitiesToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Open Borders to Us, but them is us.  Please show Jon");

	// Not allowed in demands.
	if (pDeal->GetDemandingPlayer() != NO_PLAYER)
		return;

	if(!pDeal->IsPossibleToTradeItem(eThem, GetPlayer()->GetID(), TRADE_ITEM_CITIES))
		return;

	// If they're not the one surrendering here, don't bother
	if(pDeal->IsPeaceTreatyTrade(eThem) && pDeal->GetSurrenderingPlayer() != eThem)
		return;

	// AIs will only do city exchanges. Versus humans we only buy a city if they want one too
	if (!pDeal->ContainsItemType(TRADE_ITEM_CITIES, GetPlayer()->GetID()))
		return;

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
	{
		// Create vector of the losing players' Cities so we can see which are the closest to the winner
		CvWeightedVector<int> viCityPriceRatio;

		// Loop through all of the their Cities
		int iCityLoop = 0;
		CvPlayer* pSellingPlayer = &GET_PLAYER(eThem);
		for (CvCity* pLoopCity = pSellingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pSellingPlayer->nextCity(&iCityLoop))
		{
			//make sure we're not just flipping two border towns
			CvCity* pOurClosest = GetPlayer()->GetClosestCityByPathLength(pLoopCity->plot());
			if (pOurClosest && pDeal->IsCityTrade(GetPlayer()->GetID(), pOurClosest->getX(), pOurClosest->getY()))
				continue;

			int iWhatTheyWouldPay = GetCityValueForDeal(pLoopCity, eThem);
			int iWhatIWouldPay = GetCityValueForDeal(pLoopCity, m_pPlayer->GetID());

			if (iWhatTheyWouldPay == INT_MAX || iWhatIWouldPay == INT_MAX)
			{
				continue;
			}

			int iRatio = (iWhatTheyWouldPay * 100) / max(1, iWhatIWouldPay);
			//if the deal is good
			if (iRatio < 88)
			{
				viCityPriceRatio.push_back(pLoopCity->GetID(), iRatio);
			}
		}

		// Sort the vector based on price ratio we can get
		viCityPriceRatio.StableSortItems();

		// Loop through sorted Cities.
		int iSortedCityID = 0;
		for (int iSortedCityIndex = 0; iSortedCityIndex < viCityPriceRatio.size(); iSortedCityIndex++)
		{
			iSortedCityID = viCityPriceRatio.GetElement(iSortedCityIndex);
			CvCity* pLoopCity = pSellingPlayer->getCity(iSortedCityID);

			// See if we can actually trade it to them
			if (pDeal->IsPossibleToTradeItem(eThem, GetPlayer()->GetID(), TRADE_ITEM_CITIES, pLoopCity->getX(), pLoopCity->getY()))
			{
				int iItemValue = GetCityValueForDeal(pLoopCity, GetPlayer()->GetID());
				if (iItemValue == INT_MAX)
					continue;

				// If adding this to the deal doesn't take it under the min limit, do it
				if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, false))
				{
					OutputDebugString(CvString::format("Adding %s to deal. Seller %s, buyer %s, price ratio %d\n",
						pLoopCity->getName().c_str(), pSellingPlayer->getName(), GetPlayer()->getName(), viCityPriceRatio.GetWeight(iSortedCityIndex)).c_str());
					pDeal->AddCityTrade(eThem, iSortedCityID, false);
					iTotalValue = GetDealValue(pDeal);
					return;
				}
			}
		}
	}
}


/// See if adding Gold to their side of the deal helps even out pDeal
void CvDealAI::DoAddGoldToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iDemandValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Gold to Them, but them is us.  Please show Jon");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	if (!pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_GOLD))
		return;

	int iNumGoldAlreadyInTrade = pDeal->GetGoldTrade(eThem);
	//if raw gold in deal, remove so we can refresh.
	if (iNumGoldAlreadyInTrade > 0)
	{
		DoRemoveGoldFromThem(pDeal, eThem, iTotalValue);
		iNumGoldAlreadyInTrade = 0;
		iTotalValue = GetDealValue(pDeal);
	}
	int iValueNeeded = -iTotalValue + iDemandValue;
	if (iValueNeeded > 0)
	{
		// Can't already be Gold from the other player in the Deal
		if(pDeal->GetGoldTrade(eMyPlayer) == 0)
		{
			int iNumGold = GetGoldForForValueExchange(iValueNeeded, /*bNumGoldFromValue*/ true);

			if(iNumGold <= 0)
			{
				return;
			}

			iNumGold += iNumGoldAlreadyInTrade;
			iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eThem, TRADE_ITEM_GOLD));

			//iNumGold = min(iNumGold, GET_PLAYER(eThem).GetTreasury()->GetGold());
			if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eThem, iNumGold))
			{
				pDeal->AddGoldTrade(eThem, iNumGold, false);
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
	if (!pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_GOLD))
		return;

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
			int iNumGold = GetGoldForForValueExchange(iTotalValue, /*bNumGoldFromValue*/ true);

			if(iNumGold < 0)
			{
				return;
			}
				
			iNumGold += iNumGoldAlreadyInTrade;
			iNumGold = min(iNumGold, pDeal->GetGoldAvailable(eMyPlayer, TRADE_ITEM_GOLD));

			//iNumGold = min(iNumGold, GET_PLAYER(eMyPlayer).GetTreasury()->GetGold());
			if(iNumGold != iNumGoldAlreadyInTrade && !pDeal->ChangeGoldTrade(eMyPlayer, iNumGold))
			{
				pDeal->AddGoldTrade(eMyPlayer, iNumGold, false);
			}

			iTotalValue = GetDealValue(pDeal);

		}
	}
}

/// See if adding Gold Per Turn to their side of the deal helps even out pDeal
void CvDealAI::DoAddGPTToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iDemandValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add GPT to Them, but them is us.  Please show Jon");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	if (!pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_GOLD_PER_TURN))
		return;

	int iDealDuration = pDeal->GetDuration();
	int iNumGPTAlreadyInTrade = pDeal->GetGoldPerTurnTrade(eThem);
	//if they already have GPT in this trade, remove it so we can refresh the value.
	if (iNumGPTAlreadyInTrade > 0)
	{
		DoRemoveGPTFromThem(pDeal, eThem, iNumGPTAlreadyInTrade);
		iNumGPTAlreadyInTrade = 0;
		iTotalValue = GetDealValue(pDeal);
	}
	int iValueNeeded = -iTotalValue + iDemandValue;
	{
		int iGoldRate = GET_PLAYER(eThem).calculateGoldRate();
		if (!GET_PLAYER(eThem).isHuman())
		{
			iGoldRate -= 2;
		}
		if (iGoldRate > 0)
		{

			// Can't already be GPT from the other player in the Deal
			if(pDeal->GetGoldPerTurnTrade(eMyPlayer) == 0)
			{
				int iNumGPT = GetGPTForForValueExchange(iValueNeeded, /*bNumGPTFromValue*/ true, iDealDuration, /*bFromMe*/ false, eThem);

				if (iNumGPT < 0)
					return;

				iNumGPT += iNumGPTAlreadyInTrade;
				iNumGPT = min(iNumGPT, iGoldRate);

				if (iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eThem, iNumGPT, iDealDuration))
				{
					pDeal->AddGoldPerTurnTrade(eThem, iNumGPT, iDealDuration, false);
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

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	if (!pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_GOLD_PER_TURN))
		return;

	int iDealDuration = pDeal->GetDuration();
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
		int iGoldRate = GET_PLAYER(eMyPlayer).calculateGoldRate();
		if (!GET_PLAYER(eMyPlayer).isHuman())
		{
			iGoldRate -= 2;
		}
		if (iGoldRate > 0)
		{
			// Can't already be GPT from the other player in the Deal
			if(pDeal->GetGoldPerTurnTrade(eThem) == 0)
			{
				int iNumGPT = GetGPTForForValueExchange(iTotalValue, /*bNumGPTFromValue*/ true, iDealDuration, /*bFromMe*/ true, eThem);

				if (iNumGPT < 0)
					return;
					
				iNumGPT += iNumGPTAlreadyInTrade;
				iNumGPT = min(iNumGPT, iGoldRate);

				if(iNumGPT != iNumGPTAlreadyInTrade && !pDeal->ChangeGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration))
				{
					pDeal->AddGoldPerTurnTrade(eMyPlayer, iNumGPT, iDealDuration, false);
				}

				iTotalValue = GetDealValue(pDeal);
			}
		}
	}
}

void CvDealAI::DoAddItemsToThem(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValue, int iThresholdValue, bool bGoldOnly, bool bHumanRequestedEqualization)
{
	// Don't make "our temporary for their permanent" offers to human players even if the setting is enabled, that's annoying
	// Such offers are permitted if the human requests that the AI equalize the offer, but they should still be avoided if a temporary alternative is possible
	bool bBlockPermanentItems = false;
	bool bAvoidPermanentItems = false;
	bool bWeAreOfferingPermanentItems = pDeal->ContainsPermanentItems(GetPlayer()->GetID());
	bool bWeAreOfferingTemporaryItems = pDeal->ContainsTemporaryItems(GetPlayer()->GetID(), eOtherPlayer);
	if (bWeAreOfferingTemporaryItems)
	{
		if (GET_PLAYER(eOtherPlayer).isHuman())
		{
			if (!GC.getGame().IsHumanPermanentForAITemporaryTradingAllowed())
				bBlockPermanentItems = true;
			else if (!bWeAreOfferingPermanentItems)
			{
				if (bHumanRequestedEqualization)
					bAvoidPermanentItems = true;
				else
					bBlockPermanentItems = true;
			}
		}
		else
			bBlockPermanentItems = !GC.getGame().IsPermanentForTemporaryTradingAllowed();
	}

	// If this is a demand, can't ask for permanent items, so don't try
	if (pDeal->GetDemandingPlayer() != NO_PLAYER)
		bBlockPermanentItems = true;

	// Add items to the deal while the deal value is below the threshold value. Each item added increases the deal value. iThresholdValue should be 0 if we want to equalize the deal.
	// We use a positive value for iThresholdValue if a previous attempt to equalize the deal using iThresholdValue=0 has failed and we're now trying to add items on both sides. 
	if (!bGoldOnly)
	{
		// Try to get the best items first :)
		if (!bBlockPermanentItems)
		{
			DoAddVassalageToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

			if (!bAvoidPermanentItems)
			{
				DoAddRevokeVassalageToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
				DoAddCitiesToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
				DoAddTechToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
				if (MOD_BALANCE_PERMANENT_VOTE_COMMITMENTS)
					DoAddVoteCommitmentToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

				DoAddMapsToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			}
		}

		if (!MOD_BALANCE_PERMANENT_VOTE_COMMITMENTS)
			DoAddVoteCommitmentToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

		DoAddThirdPartyWarToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		DoAddThirdPartyPeaceToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

		// if a strategics trade, try strategics first.
		if (pDeal->IsStrategicsTrade())
		{
			DoAddStrategicResourceToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddLuxuryResourceToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		}
		else
		{
			DoAddLuxuryResourceToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddStrategicResourceToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		}

		DoAddOpenBordersToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		DoAddEmbassyToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

		// If we couldn't succeed with temporary items, let's try permanent items
		if (bAvoidPermanentItems)
		{
			DoAddMapsToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddTechToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddCitiesToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddRevokeVassalageToThem(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		}
	}

	// in the last step, when adding gold, we always want to bring the deal closer to 0, so we don't use iThresholdValue here. Exception: Demands
	int iDemandValue = (pDeal->GetDemandingPlayer() != NO_PLAYER) ? iThresholdValue : 0;

	if (!bBlockPermanentItems && !bAvoidPermanentItems)
		DoAddGoldToThem(pDeal, eOtherPlayer, iTotalValue, iDemandValue);

	DoAddGPTToThem(pDeal, eOtherPlayer, iTotalValue, iDemandValue);

	if (bAvoidPermanentItems)
		DoAddGoldToThem(pDeal, eOtherPlayer, iTotalValue, iDemandValue);
}

void CvDealAI::DoAddItemsToUs(CvDeal* pDeal, PlayerTypes eOtherPlayer, int& iTotalValue, int iThresholdValue, bool bGoldOnly, bool bHumanRequestedEqualization)
{
	// Even if the trading restrictions on our permanent items for their temporary items are removed, let's not make offers for it unless we're teammates or war can't be declared
	bool bBlockPermanentItems = false;
	bool bPrioritizePermanentItems = false;
	bool bTheyAreOfferingPermanentItems = pDeal->ContainsPermanentItems(eOtherPlayer);
	bool bTheyAreOfferingTemporaryItems = pDeal->ContainsTemporaryItems(eOtherPlayer, GetPlayer()->GetID());
	if (GetPlayer()->getTeam() != GET_PLAYER(eOtherPlayer).getTeam() && !GC.getGame().isOption(GAMEOPTION_ALWAYS_PEACE) && !GC.getGame().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE))
	{
		if (bTheyAreOfferingTemporaryItems)
			bBlockPermanentItems = !bHumanRequestedEqualization || !GC.getGame().IsPermanentForTemporaryTradingAllowed();
	}
	// As a convenience to humans and teammates, let's prioritize giving permanent items (when requesting them) in a no-betrayal scenario
	if (bTheyAreOfferingPermanentItems && !bTheyAreOfferingTemporaryItems)
	{
		if (GetPlayer()->getTeam() == GET_PLAYER(eOtherPlayer).getTeam() || GET_PLAYER(eOtherPlayer).isHuman())
			bPrioritizePermanentItems = true;
	}

	// Add items to the deal while the deal value is above the threshold value. Each item added decreases the deal value. iThresholdValue should be 0 if we want to equalize the deal.
	// We use a negative value for iThresholdValue if a previous attempt to equalize the deal using iThresholdValue=0 has failed and we're now trying to add items on both sides. 
	if (!bGoldOnly)
	{
		// In most circumstances, we give up the least valuable item first
		// Exception for vassalage: if we want to voluntarily capitulate for protection, let's offer it first
		if (!bBlockPermanentItems)
			DoAddVassalageToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

		if (bPrioritizePermanentItems)
		{
			DoAddMapsToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			if (MOD_BALANCE_PERMANENT_VOTE_COMMITMENTS)
				DoAddVoteCommitmentToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

			DoAddTechToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddCitiesToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddRevokeVassalageToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		}

		DoAddEmbassyToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		DoAddOpenBordersToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

		// if a strategics trade, try strategics first.
		if (pDeal->IsStrategicsTrade())
		{
			DoAddStrategicResourceToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddLuxuryResourceToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		}
		else
		{
			DoAddLuxuryResourceToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddStrategicResourceToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		}

		DoAddThirdPartyPeaceToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		DoAddThirdPartyWarToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		if (!MOD_BALANCE_PERMANENT_VOTE_COMMITMENTS)
			DoAddVoteCommitmentToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

		if (!bBlockPermanentItems && !bPrioritizePermanentItems)
		{
			DoAddMapsToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			if (MOD_BALANCE_PERMANENT_VOTE_COMMITMENTS)
				DoAddVoteCommitmentToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);

			DoAddTechToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddCitiesToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
			DoAddRevokeVassalageToUs(pDeal, eOtherPlayer, iTotalValue, iThresholdValue);
		}
	}

	// in the last step, when adding gold, we always want to bring the deal closer to 0, so we don't use iThresholdValue here
	if (bPrioritizePermanentItems)
		DoAddGoldToUs(pDeal, eOtherPlayer, iTotalValue);

	DoAddGPTToUs(pDeal, eOtherPlayer, iTotalValue);

	if (!bBlockPermanentItems && !bPrioritizePermanentItems)
		DoAddGoldToUs(pDeal, eOtherPlayer, iTotalValue);
}

/// See if removing Gold Per Turn from their side of the deal helps even out pDeal
void CvDealAI::DoRemoveGPTFromThem(CvDeal* pDeal, PlayerTypes eThem, int iToRemove)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to remove GPT from Them, but them is us.  Please show Jon");

	int iDealDuration = pDeal->GetDuration();
	int iNumGoldPerTurnInThisDeal = pDeal->GetGoldPerTurnTrade(eThem);
	if (iNumGoldPerTurnInThisDeal > 0)
	{
		// Found some GoldPerTurn to remove
		int iNumGoldPerTurnToRemove = min(iToRemove, iNumGoldPerTurnInThisDeal);
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
void CvDealAI::DoRemoveGPTFromUs(CvDeal* pDeal, int iToRemove)
{
	int iDealDuration = pDeal->GetDuration();
	
	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	int iNumGoldPerTurnInThisDeal = pDeal->GetGoldPerTurnTrade(eMyPlayer);
	if(iNumGoldPerTurnInThisDeal > 0)
	{
		// Found some GoldPerTurn to remove
		int iNumGoldPerTurnToRemove = min(iToRemove, iNumGoldPerTurnInThisDeal);
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

	bool bBecomeMyVassal = pLosingPlayer->GetDiplomacyAI()->IsVassalageAcceptable(eWinningPlayer, false);
	bool bRevokeMyVassals = false;
	// Reduce war score if losing player wants to become winning player's vassal
	if (bBecomeMyVassal)
	{
		iWarScore /= 2;
	}
	// Is losing player willing to revoke his vassals?
	if(iWarScore <= -85 && GET_TEAM(pLosingPlayer->getTeam()).GetNumVassals() > 0)
	{
		//If we're willing to do this, give less below.
		bRevokeMyVassals = true;
		iWarScore /= max(2, GET_TEAM(pLosingPlayer->getTeam()).GetNumVassals());
	}
	if(iWarScore < 0)
	{
		iWarScore *= -1;
	}

	//strategic warscore adjustment
	if (!pLosingPlayer->HasCityInDanger(false,0))
		iWarScore = max(0, iWarScore - 10);

	pDeal->AddPeaceTreaty(eWinningPlayer, GC.getGame().getGameSpeedInfo().getPeaceDealDuration());
	pDeal->AddPeaceTreaty(eLosingPlayer, GC.getGame().getGameSpeedInfo().getPeaceDealDuration());
	DoAddPlayersAlliesToTreaty(eOtherPlayer, pDeal);

	// do we offer a city that's about to fall?
	CvCity* pCityToOffer = NULL;
	int iMaxCityValue = 0;
	CvCity* pLoopCity = NULL;
	int iCityLoop = 0;
	if (iWarScore > 25 && pLosingPlayer->getNumCities() > 1)
	{
		// Loop through all of the loser's Cities, looking only at the ones with low health.
		for (pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
		{
			//do this from the winner's perspective!
			int iCurrentCityValue = GetCityValueForDeal(pLoopCity, eWinningPlayer);
			if (iCurrentCityValue == INT_MAX)
				continue;

			if (pLoopCity->plot()->GetNumEnemyUnitsAdjacent(pLoopCity->getTeam(), NO_DOMAIN, NULL, false, GET_PLAYER(eWinningPlayer).getTeam()) >= 2)
			{
				int iHPPercent = 100 * (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()) / pLoopCity->GetMaxHitPoints();
				if (iHPPercent < 10 + iWarScore / 10)
				{
					if (iCurrentCityValue > iMaxCityValue)
					{
						iMaxCityValue = iCurrentCityValue;
						pCityToOffer = pLoopCity;
					}
				}
			}
		}
	}
	if (pCityToOffer)
	{
		iWarScore -= 25;
		pDeal->AddCityTrade(eLosingPlayer, pCityToOffer->GetID());
	}

	int iPercentGoldToGive = iWarScore;
	int iPercentGPTToGive = (iWarScore / 2);
	bool bGiveUpCities = (iWarScore > 60);
	int iPercentCitiesGiveUp = (iWarScore / 5);
	bool bGiveUpStratResources = (iWarScore > 35);
	bool bGiveUpLuxuryResources = (iWarScore > 15);
	int iGiveUpLuxResources = iWarScore;
	int iGiveUpStratResources = (iWarScore / 4);


	// If the player only has one city then we can't get any more from him
	if (!pCityToOffer && bGiveUpCities && iPercentCitiesGiveUp > 0 && pLosingPlayer->getNumCities() > 1)
	{
		int iTotalCityValue = 0;

		CvWeightedVector<int> viCityValue;

		// Loop through all of the loser's Cities, looking only at valid ones.
		for(pLoopCity = pLosingPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pLosingPlayer->nextCity(&iCityLoop))
		{
			//do this from the winner's perspective!
			int iCurrentCityValue = GetCityValueForDeal(pLoopCity, eWinningPlayer);
			if (iCurrentCityValue == INT_MAX)
				continue;

			//add up total city value of the loser (before danger and damage adjustment)
			iTotalCityValue += iCurrentCityValue;

			//Remember for later
			viCityValue.push_back(pLoopCity->GetID(), iCurrentCityValue);
		}

		// Sort the vector based on distance from winner's capital
		viCityValue.StableSortItems();
		int iSortedCityID = 0;

		// Determine the value of Cities to be given up
		int iCityValueToSurrender = iTotalCityValue * iPercentCitiesGiveUp / 100;
		// Loop through sorted Cities and add them to the deal if they're under the amount to give up
		// Start from the back of the list, because that's where the cheapest cities are
		for(int iSortedCityIndex = viCityValue.size() - 1; iSortedCityIndex > -1 ; iSortedCityIndex--)
		{
			iSortedCityID = viCityValue.GetElement(iSortedCityIndex);
			pLoopCity = pLosingPlayer->getCity(iSortedCityID);

			int iCurrentCityValue = GetCityValueForDeal(pLoopCity, eWinningPlayer);
			if (iCurrentCityValue == INT_MAX)
				continue;

			// City is worth less than what is left to be added to the deal, so add it
			if (iCurrentCityValue < iCityValueToSurrender && iCurrentCityValue > 0)
			{
				pDeal->AddCityTrade(eLosingPlayer, iSortedCityID);
				iCityValueToSurrender -= iCurrentCityValue;

				//Frontline cities count more than they're worth. Ideally they should satisfy the winner?
				iCityValueToSurrender -= (pLoopCity->getDamage() * 10);
				if (pLosingPlayer->GetPlotDanger(pLoopCity) > /*20 in CP, 8 in VP*/ GD_INT_GET(CITY_HIT_POINTS_HEALED_PER_TURN))
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
			pDeal->AddGoldTrade(eLosingPlayer, iGold, false);
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
			pDeal->AddGoldPerTurnTrade(eLosingPlayer, iGPTToGive, iDuration, false);
		}
	}

	// Luxury Resources
	if(bGiveUpLuxuryResources && iGiveUpLuxResources > 0)
	{
		ResourceUsageTypes eUsage;
		ResourceTypes eResource;
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
				continue;

			// Don't bother looking at this Resource if the other player doesn't even have any of it
			if (pLosingPlayer->getNumResourceAvailable(eResource, false) == 0)
				continue;

			// resource value evaluation from the winner's perspective
			int iCurrentResourceValue = GET_PLAYER(eWinningPlayer).GetDealAI()->GetTradeItemValue(TRADE_ITEM_RESOURCES, false, eLosingPlayer, eResource, 1, -1, false, GC.getGame().GetDealDuration(), false, /*bEqualize*/ false);

			if(iCurrentResourceValue == INT_MAX)
				continue;

			if(iCurrentResourceValue > 0)
			{
				iTotalResourceValue += iCurrentResourceValue;
				viResourceValue.push_back(eResource, iCurrentResourceValue);
			}
		}

		int iResourceValueToSurrender = (iTotalResourceValue * iGiveUpLuxResources) / 100;

		// Sort the vector based on value
		viResourceValue.StableSortItems();
		if(viResourceValue.size() > 0)
		{
			for(int iSortedResourceIndex =  0; iSortedResourceIndex < viResourceValue.size(); iSortedResourceIndex++)
			{
				ResourceTypes eResourceList = (ResourceTypes)viResourceValue.GetElement(iSortedResourceIndex);

				int iCurrentResourceValue = viResourceValue.GetWeight(iSortedResourceIndex);

				if(iCurrentResourceValue <= iResourceValueToSurrender)
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

			int iCurrentResourceValue = GET_PLAYER(eWinningPlayer).GetDealAI()->GetTradeItemValue(TRADE_ITEM_RESOURCES, false, eLosingPlayer, eResource, iResourceQuantity, -1, false, GC.getGame().GetDealDuration(), false, /*bEqualize*/ false);

			if(iCurrentResourceValue == INT_MAX)
				continue;

			if(iCurrentResourceValue > 0)
			{
				iTotalResourceValue += iCurrentResourceValue;
				viResourceValue.push_back(eResource, iCurrentResourceValue);
			}
		}
		
		int iResourceValueToSurrender = (iTotalResourceValue * iGiveUpStratResources) / 100;

		viResourceValue.StableSortItems();
		if(viResourceValue.size() > 0)
		{
			for(int iSortedResourceIndex =  0; iSortedResourceIndex < viResourceValue.size(); iSortedResourceIndex++)
			{
				ResourceTypes eResourceList = (ResourceTypes)viResourceValue.GetElement(iSortedResourceIndex);				
				iResourceQuantity = pLosingPlayer->getNumResourceAvailable(eResourceList, false);
				if (iResourceQuantity > 3)
				{
					iResourceQuantity = 3;
				}
				int iCurrentResourceValue = viResourceValue.GetWeight(iSortedResourceIndex);

				if(iCurrentResourceValue < iResourceValueToSurrender && iCurrentResourceValue > 0)
				{
					pDeal->AddResourceTrade(eLosingPlayer, eResourceList, iResourceQuantity, GC.getGame().GetDealDuration());
					iResourceValueToSurrender -= iCurrentResourceValue;
				}
			}
		}
	}
	if(bBecomeMyVassal)
	{
		pDeal->AddVassalageTrade(eLosingPlayer);
	}
	if(bRevokeMyVassals)
	{
		pDeal->AddRevokeVassalageTrade(eLosingPlayer);
	}
}

/// What are we willing to give/receive for peace with the active human player?
int CvDealAI::GetCachedValueOfPeaceWithHuman() const
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
	CvPlayer* pMinor = NULL;
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
	int iTotalValue = GetDealValue(pDeal);
	DoAddItemsToThem(pDeal, eOtherPlayer, iTotalValue, iIdealValue, false, false);
	
	if (pDeal->m_TradedItems.empty())
	{
		return 0;
	}

	if (iTotalValue <= 0 || iTotalValue == INT_MAX)
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

	int iIdealValue = /*200*/ GD_INT_GET(DEMAND_LIMIT_MAX_VALUE);
	// scaling with game progress
	int iGameProgressFactor = (GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown() * 100) / GC.getNumTechInfos();
	iIdealValue *= (100 + iGameProgressFactor * /*20*/ GD_INT_GET(DEMAND_LIMIT_GAMEPROGRESS_SCALING));
	iIdealValue /= 100;

	// a fraction of the maximum value is demanded based on military strength and meanness
	iIdealValue *= (NUM_STRENGTH_VALUES - 1 - (int)GetPlayer()->GetDiplomacyAI()->GetMilitaryStrengthComparedToUs(eOtherPlayer)) * GetPlayer()->GetDiplomacyAI()->GetMeanness();
	iIdealValue /= (((int)NUM_STRENGTH_VALUES - 1)  * 10);

	int iTotalValue = 0;
	DoAddItemsToThem(pDeal, eOtherPlayer, iTotalValue, iIdealValue, false, false);

	return (!pDeal->m_TradedItems.empty() && iTotalValue > 0);
}

/// A good time to make an offer for someone's extra Luxury?
bool CvDealAI::IsMakeOfferForLuxuryResource(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	ResourceTypes eLuxuryFromThem = NO_RESOURCE;

	// Don't ask for a Luxury if we're hostile or planning a war
	CivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer);
	if (eApproach <= CIV_APPROACH_HOSTILE)
	{
		return false;
	}


	int iBestValue = 20;

	// See if the other player has a Resource to trade
	int iDuration = GC.getGame().GetDealDuration();
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes) iResourceLoop;

		// Only look at Luxuries
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);

		if (!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eResource))
			continue;

		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
		{
			continue;
		}

		//one copy is enough
		if (m_pPlayer->getNumResourceAvailable(eResource, true) > 0)
		{
			continue;
		}

		// Don't ask if they have only one copy
		if (GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eResource, false) <= 1)
			continue;

		// Is it possible to trade that item?
		if (!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eResource, 1))
			continue;

		// Can we strike a deal with the other AI?
		if (GetTradeItemValue(TRADE_ITEM_RESOURCES, false, eOtherPlayer, eResource, 1, -1, false, iDuration, /* bIsAIOffer*/ true, /*bEqualize*/ true) == INT_MAX)
			continue;

		// Let's try to get the resource that's most valuable to us (do not do bEqualize here)
		int iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, false, eOtherPlayer, eResource, 1, -1, false, iDuration, /* bIsAIOffer*/ true, /*bEqualize*/ false);
		if (iItemValue != INT_MAX && iItemValue > iBestValue)
		{
			eLuxuryFromThem = eResource;
			iBestValue = iItemValue;
		}
	}

	// Extra Luxury found!
	if (eLuxuryFromThem != NO_RESOURCE)
	{
		// Seed the deal with the item we want
		pDeal->AddResourceTrade(eOtherPlayer, eLuxuryFromThem, 1, iDuration);

		// AI evaluation
		bool bUselessReferenceVariable = false;
		bool bCantMatchOffer = false;
		bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, /*bDontChangeMyExistingItems*/ bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

		return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
	}
	return false;
}


/// A good time to make an offer for someone's extra strats?
bool CvDealAI::IsMakeOfferForStrategicResource(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	ResourceTypes eStratFromThem = NO_RESOURCE;

	// Don't ask for a resource if we're hostile or planning a war
	CivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer);
	if (eApproach <= CIV_APPROACH_HOSTILE)
		return false;

	int iBestValue = 15;

	// See if the other player has a Resource to trade
	for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		ResourceTypes eResource = (ResourceTypes) iResourceLoop;

		// Only look at strategic resources here
		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if(pkResourceInfo == NULL || pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_STRATEGIC)
			continue;

		if (!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eResource))
			continue;

		// If we have some to spare, don't get more
		if (GetPlayer()->getNumResourceAvailable(eResource, true) > 2)
			continue;

		int iNum = GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eResource, false);
		if (iNum <= 0)
			continue;

		// can't strike a deal with the other AI?
		if (GetTradeItemValue(TRADE_ITEM_RESOURCES, false, eOtherPlayer, eResource, 1, -1, false, GC.getGame().GetDealDuration(), /*bIsAIOffer*/ true, /*bEqualize*/ true) == INT_MAX)
			continue;

		// Let's try to get the resource that's most valuable to us (do not do bEqualize here)
		int iItemValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, false, eOtherPlayer, eResource, 1, -1, false, GC.getGame().GetDealDuration(), /*bIsAIOffer*/ true, /*bEqualize*/ false);
		if(iItemValue != INT_MAX && iItemValue > iBestValue)
		{
			eStratFromThem = eResource;
			iBestValue = iItemValue;
		}
	}

	// Extra Strat found!
	if(eStratFromThem != NO_RESOURCE)
	{
		int iAvailable = GET_PLAYER(eOtherPlayer).getNumResourceAvailable(eStratFromThem, false) - 1; // don't ask for all of their resources
		int iDesired = 0;

		int iPrevDealValue = -1;
		int iNextDealValue = 0;

		// we try to increase iDesired as long as it's possible and as long as we're willing to pay more for the additional item
		while(iPrevDealValue != iNextDealValue)
		{
			iDesired++;
			// max 5 items
			if (iDesired > 5 || iDesired > iAvailable)
				break;

			if (!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eStratFromThem, iDesired))
				break;
			
			// can we strike a deal with the other AI?
			if (GetTradeItemValue(TRADE_ITEM_RESOURCES, false, eOtherPlayer, eStratFromThem, iDesired, -1, false, GC.getGame().GetDealDuration(), /*bIsAIOffer*/ true, /*bEqualize*/ true) == INT_MAX)
				break;

			iPrevDealValue = iNextDealValue;
			// this is our valuation of the item, use bEqualize=false here 
			iNextDealValue = GetTradeItemValue(TRADE_ITEM_RESOURCES, false, eOtherPlayer, eStratFromThem, iDesired, -1, false, GC.getGame().GetDealDuration(), /*bIsAIOffer*/ true, /*bEqualize*/ false);
		}
		// the last item added was too much, don't include it in the deal
		iDesired--;

		// failsave
		if (iDesired == 0)
			return false;

		// Seed the deal with the item we want
		pDeal->AddResourceTrade(eOtherPlayer, eStratFromThem, iDesired, GC.getGame().GetDealDuration());

		// AI evaluation
		bool bUselessReferenceVariable = false;
		bool bCantMatchOffer = false;
		bool bDealAcceptable = false;
		
		while (!bDealAcceptable)
		{
			bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
			bDealAcceptable = bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
			if (!bDealAcceptable)
			{
				// try again with one resource item less
				iDesired--;
				if (iDesired == 0)
				{
					return false;
				}
				else
				{
					pDeal->ClearItems();
					pDeal->AddResourceTrade(eOtherPlayer, eStratFromThem, iDesired, GC.getGame().GetDealDuration());
				}
			}
		}

		return bDealAcceptable;
	}
	return false;
}


/// A good time to make an offer to get an embassy?
bool CvDealAI::IsMakeOfferForEmbassy(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for an embassy if we're hostile or planning war
	CivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer);
	if (eApproach <= CIV_APPROACH_HOSTILE || eApproach == CIV_APPROACH_GUARDED)
	{
		return false;
	}

	// Can we actually complete this deal?
	// Note the order of the player ids: This is because the trade item is "accept embassy" not "send embassy"
	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_ALLOW_EMBASSY))
	{
		return false;
	}

	// AI should not offer embassies for gold - wait until embassy for embassy is possible
	// Note that humans can still offer and get an embassy for gold trade, it's not illegal
	if(!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eOtherPlayer, TRADE_ITEM_ALLOW_EMBASSY) && !GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).HasEmbassyAtTeam(GetPlayer()->getTeam()))
	{
		return false;
	}

	// Do we actually want an embassy with eOtherPlayer?
	if(GetPlayer()->GetDiplomacyAI()->WantsEmbassyAtPlayer(eOtherPlayer))
	{
		// Seed the deal with the item we want
		pDeal->AddAllowEmbassy(eOtherPlayer);

		// AI evaluation
		bool bUselessReferenceVariable = false;
		bool bCantMatchOffer = false;
		bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

		return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
	}

	return false;
}

/// A good time to make an offer to get Open Borders?
bool CvDealAI::IsMakeOfferForOpenBorders(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for Open Borders if we're hostile or planning war
	CivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer);
	if (eApproach <= CIV_APPROACH_HOSTILE)
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

		// AI evaluation
		bool bUselessReferenceVariable = false;
		bool bCantMatchOffer = false;
		bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

		return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
	}

	return false;
}

/// A good time to make an offer for a Research Agreement?
bool CvDealAI::IsMakeOfferForResearchAgreement(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	if (GetPlayer()->IsAITeammateOfHuman())
		return false;

	if (!GetPlayer()->GetDiplomacyAI()->IsWantsResearchAgreementWithPlayer(eOtherPlayer))
		return false;

	if (!GET_PLAYER(eOtherPlayer).isHuman() && !GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsWantsResearchAgreementWithPlayer(GetPlayer()->GetID()))
		return false;

	// Logic for when THIS AI wants to make a RA is in the Diplo AI

	// Can we actually complete this deal?
	if(!pDeal->IsPossibleToTradeItem(eOtherPlayer, GetPlayer()->GetID(), TRADE_ITEM_RESEARCH_AGREEMENT))
	{
		return false;
	}

	// Seed the deal with the item we want
	pDeal->AddResearchAgreement(GetPlayer()->GetID(), GC.getGame().GetDealDuration());
	pDeal->AddResearchAgreement(eOtherPlayer, GC.getGame().GetDealDuration());

	// AI evaluation
	bool bUselessReferenceVariable = false;
	bool bCantMatchOffer = false;
	bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

	return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
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

	if (!GetPlayer()->GetDiplomacyAI()->IsWantsDefensivePactWithPlayer(eOtherPlayer))
	{
		return false;
	}
	if (!GET_PLAYER(eOtherPlayer).isHuman() && !GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsWantsDefensivePactWithPlayer(GetPlayer()->GetID()))
	{
		return false;
	}

	// Seed the deal with the item we want
	pDeal->AddDefensivePact(GetPlayer()->GetID(), GC.getGame().GetDealDuration());
	pDeal->AddDefensivePact(eOtherPlayer, GC.getGame().GetDealDuration());

	// AI evaluation
	bool bUselessReferenceVariable = false;
	bool bCantMatchOffer = false;
	bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

	return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
}

/// A good time to make an offer to buy or sell a city?
bool CvDealAI::IsMakeOfferForCityExchange(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for a city if we're hostile or planning a war
	CivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer);
	if (eApproach <= CIV_APPROACH_HOSTILE || eApproach == CIV_APPROACH_GUARDED)
	{
		return false;
	}

	int iCityLoop = 0;
	CvCity* pBestBuyCity = NULL;
	int iBestBuyCity = 120; //initial value, deal must be good to justify building a courthouse 
	CvCity* pBestSellCity = NULL;
	int iBestSellCity = 150; //initial value, they must overpay a lot

	//check their cities
	for (CvCity* pTheirCity = GET_PLAYER(eOtherPlayer).firstCity(&iCityLoop); pTheirCity != NULL; pTheirCity = GET_PLAYER(eOtherPlayer).nextCity(&iCityLoop))
	{
		if(!pTheirCity)
			continue;

		if(pDeal->IsPossibleToTradeItem(eOtherPlayer, m_pPlayer->GetID(), TRADE_ITEM_CITIES, pTheirCity->getX(), pTheirCity->getY()))
		{
			int iMyPrice = GetCityValueForDeal(pTheirCity, GetPlayer()->GetID());
			int iTheirPrice = GetCityValueForDeal(pTheirCity, eOtherPlayer);

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
			int iMyPrice = GetCityValueForDeal(pMyCity, m_pPlayer->GetID());
			int iTheirPrice = GetCityValueForDeal(pMyCity, eOtherPlayer);

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

	// AI evaluation
	bool bUselessReferenceVariable = false;
	bool bCantMatchOffer = false;
	bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

	return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
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
	CivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer);
	if (eApproach <= CIV_APPROACH_HOSTILE || eApproach == CIV_APPROACH_GUARDED)
	{
		return false;
	}

	// If we aren't at war, also buck on deceptive. Otherwise we want allies ASAP.
	if (!GetPlayer()->IsAtWarAnyMajor() && eApproach == CIV_APPROACH_DECEPTIVE)
		return false;

	// Don't ask for war if they are weaker than us
	if (GetPlayer()->GetDiplomacyAI()->GetMilitaryStrengthComparedToUs(eOtherPlayer) < STRENGTH_AVERAGE && GetPlayer()->GetDiplomacyAI()->GetEconomicStrengthComparedToUs(eOtherPlayer) < STRENGTH_AVERAGE)
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

	//Asking a vassal? Abort!
	if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsVassalOfSomeone())
	{
		return false;
	}
	//Vassals asking? Nope!
	if(GET_TEAM(GetPlayer()->getTeam()).IsVassalOfSomeone())
	{
		return false;
	}

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
		if(GET_PLAYER(eAgainstPlayer).isMajorCiv() && GetPlayer()->GetDiplomacyAI()->GetCivApproach(eAgainstPlayer) > CIV_APPROACH_GUARDED)
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

	// AI evaluation
	bool bUselessReferenceVariable = false;
	bool bCantMatchOffer = false;
	bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

	return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
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
	if(GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer) == CIV_APPROACH_FRIENDLY)
	{
		return false;
	}
	// Don't ask for peace if we are in a DoF
	if(GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eOtherPlayer))
	{
		return false;
	}

	//Asking a vassal? Abort!
	if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsVassalOfSomeone())
	{
		return false;
	}
	//Vassals asking? Nope!
	if(GET_TEAM(GetPlayer()->getTeam()).IsVassalOfSomeone())
	{
		return false;
	}

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
		if(GET_PLAYER(eAgainstPlayer).isMinorCiv() && (GetPlayer()->GetDiplomacyAI()->GetCivApproach(eAgainstPlayer) == CIV_APPROACH_WAR))
		{
			continue;
		}
		//Is our approach of the player we're asking about bad? Don't do it!
		if(!GET_PLAYER(eAgainstPlayer).isMinorCiv() && GetPlayer()->GetDiplomacyAI()->GetCivApproach(eAgainstPlayer) < CIV_APPROACH_DECEPTIVE)
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


	// AI evaluation
	bool bUselessReferenceVariable = false;
	bool bCantMatchOffer = false;
	bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

	return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
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
	if(GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer) < CIV_APPROACH_AFRAID)
	{
		return false;
	}
	if (!GET_PLAYER(eOtherPlayer).GetLeagueAI()->CanCommitVote(GetPlayer()->GetID()))
	{
		return false;
	}
	//Asking a vassal? Abort!
	if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).IsVassalOfSomeone())
	{
		return false;
	}
	//Vassals asking? Nope!
	if(GET_TEAM(GetPlayer()->getTeam()).IsVassalOfSomeone())
	{
		return false;
	}

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
			int iItemValue = GetTradeItemValue(TRADE_ITEM_VOTE_COMMITMENT, /*bFromMe*/ false, eOtherPlayer, iProposalID, iVoteChoice, iNumVotes, bRepeal, -1, true);
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

	// AI evaluation
	bool bUselessReferenceVariable = false;
	bool bCantMatchOffer = false;
	bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

	return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
}

/// This function called when human player enters diplomacy
void CvDealAI::DoTradeScreenOpened()
{
	TeamTypes eActiveTeam = GC.getGame().getActiveTeam();
	PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();

	if(GET_TEAM(GetTeam()).isAtWar(eActiveTeam))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();
		m_pPlayer->SetCachedValueOfPeaceWithHuman(0);

		PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eActivePlayer);
		PeaceTreatyTypes ePeaceTreatyImWillingToAccept = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToAccept(eActivePlayer);

		// Does the AI actually want peace?
		if(ePeaceTreatyImWillingToOffer >= PEACE_TREATY_WHITE_PEACE && ePeaceTreatyImWillingToAccept >= PEACE_TREATY_WHITE_PEACE)
		{
			// Clear out UI deal first, we're going to add a couple things to it
			CvInterfacePtr<ICvDeal1> pUIDeal(GC.GetEngineUserInterface()->GetScratchDeal());
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
		m_pPlayer->GetDiplomacyAI()->CancelRenewDeal(eActivePlayer, REASON_HUMAN_REJECTION, false, NULL, true);
	}
}

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
		// Deal valued impossible?
		if (GET_PLAYER(eMyPlayer).GetDealAI()->GetDealValue(pDeal) == INT_MAX)
		{
			return DEMAND_RESPONSE_GIFT_REFUSE_TOO_MUCH;
		}
		// Deceptive AIs won't ever accept a help request
		if (pDiploAI->GetCivApproach(eFromPlayer) <= CIV_APPROACH_DECEPTIVE)
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
		if(iOurTotalHappy > 60 &&											// need to have some spare happiness
			iTheirTotalHappy < 50 &&										// they need happiness
			(pDiploAI->GetWarmongerThreat(eFromPlayer) < THREAT_MAJOR ||	// don't give out happiness to warmongers
			pDiploAI->GetWarmongerHate() <= 5) &&							// only if we hate warmongers
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

		switch(pDiploAI->GetCivOpinion(eFromPlayer))
		{
		case CIV_OPINION_ALLY:
			iOddsOfGivingIn += 100;
			break;
		case CIV_OPINION_FRIEND:
			iOddsOfGivingIn += 50;
			break;
		case CIV_OPINION_FAVORABLE:
			iOddsOfGivingIn += 25;
			break;
		case CIV_OPINION_NEUTRAL:
		case CIV_OPINION_COMPETITOR:
		case CIV_OPINION_ENEMY:
		case CIV_OPINION_UNFORGIVABLE:
			break; // No change.
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
		int iGoldRequested = 0;
		int iGPTRequested = 0;
		int iLuxuriesRequested = 0;
		int iStrategicsRequested = 0;
		int iTechsRequested = 0;
		int iTempGold = 0;

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
	int iItemValue = 0;
	
	CvPlayer* pSeller = bFromMe ? GetPlayer() : &GET_PLAYER(eOtherPlayer);	// Who is selling this map?
	CvPlayer* pBuyer = bFromMe ? &GET_PLAYER(eOtherPlayer) : GetPlayer();	// Who is buying this map?

	// Look at every tile on map
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);

		if(pPlot == NULL)
			continue;

		// Seller must have plot revealed
		if(!pPlot->isRevealed(pSeller->getTeam()))
			continue;

		// Buyer can't have plot revealed
		if(pPlot->isRevealed(pBuyer->getTeam()))
			continue;

		// Handle terrain features. A human will estimate based on map type ...
		int iPlotValue = 1;
		switch(pPlot->getTerrainType())
		{
			case TERRAIN_GRASS:
			case TERRAIN_PLAINS:
			case TERRAIN_HILL:
			case TERRAIN_COAST:
				iPlotValue = 5;
				break;
			case TERRAIN_DESERT:
			case TERRAIN_TUNDRA:
				iPlotValue = 2;
				break;
			case NO_TERRAIN:
			case TERRAIN_MOUNTAIN:
			case TERRAIN_SNOW:
			case TERRAIN_OCEAN:
				iPlotValue = 1;
				break;
		}

		iItemValue += iPlotValue;
	}

	iItemValue /= 10;

	//nothing to be gained
	if (iItemValue == 0)
		return INT_MAX;

	if(bFromMe)
	{
		//prevent AI spam
		if (iItemValue <= 400 && !GET_PLAYER(eOtherPlayer).isHuman())
			return INT_MAX;

		// Approach will modify the deal
		switch (GetPlayer()->GetDiplomacyAI()->GetSurfaceApproach(eOtherPlayer))
		{
			case CIV_APPROACH_WAR:
				iItemValue *= 500;
				break;
			case CIV_APPROACH_HOSTILE:
				iItemValue *= 250;
				break;
			case CIV_APPROACH_GUARDED:
				iItemValue *= 130;
				break;
			case CIV_APPROACH_DECEPTIVE:
				iItemValue *= 100;
				break;
			case CIV_APPROACH_AFRAID:
				iItemValue *= 80;
				break;
			case CIV_APPROACH_FRIENDLY:
				iItemValue *= 100;
				break;
			case CIV_APPROACH_NEUTRAL:
				iItemValue *= 100;
				break;
			default:
				iItemValue *= 100;
			break;
		}

		iItemValue /= 100;
	}
	else
	{
		if (iItemValue <= 400 && !GET_PLAYER(eOtherPlayer).isHuman())
			return INT_MAX;

		// Approach will modify the deal
		switch(GetPlayer()->GetDiplomacyAI()->GetSurfaceApproach(eOtherPlayer))
		{
			case CIV_APPROACH_WAR:
				iItemValue *= 100;
				break;
			case CIV_APPROACH_HOSTILE:
				iItemValue *= 25;
				break;
			case CIV_APPROACH_GUARDED:
				iItemValue *= 50;
				break;
			case CIV_APPROACH_DECEPTIVE:
				iItemValue *= 100;
				break;
			case CIV_APPROACH_AFRAID:
				iItemValue *= 75;
				break;
			case CIV_APPROACH_FRIENDLY:
				iItemValue *= 120;
				break;
			case CIV_APPROACH_NEUTRAL:
				iItemValue *= 100;
				break;
			default:
				iItemValue *= 100;
			break;
		}

		iItemValue /= 100;
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

	if (pkTechInfo == NULL)
		return INT_MAX;

	//important, don't want to recalculate for every potential tech
	UpdateResearchRateCache(eOtherPlayer);

	int iTurnsLeft = 0;
	int iTechEra = pkTechInfo->GetEra();

	if(bFromMe)
	{
		iTurnsLeft = GET_PLAYER(eOtherPlayer).GetPlayerTechs()->GetResearchTurnsLeft(eTech, true, m_vResearchRates[eOtherPlayer].second);
	}
	else
	{
		iTurnsLeft = GetPlayer()->GetPlayerTechs()->GetResearchTurnsLeft(eTech, true, m_vResearchRates[GetPlayer()->GetID()].second);
	}

	int iI = 0;
	int iTechMod = 0;

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
			TechTypes eRevealTech = (TechTypes)pResourceInfo->getTechReveal();
			if (GetPlayer()->GetPlayerTraits()->IsAlternateResourceTechs())
			{
				TechTypes eAltRevealTech = GetPlayer()->GetPlayerTraits()->GetAlternateResourceTechs((ResourceTypes)iI).m_eTechReveal;
				if (eAltRevealTech != NO_TECH)
				{
					eRevealTech = eAltRevealTech;
				}
			}
			if(eRevealTech == eTech)
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

	// BASE COST = (TurnsLeft * 30 * (era ^ 0.7))	-- Ancient Era is 1, Classical Era is 2 because I incremented it
	iItemValue = max(10, iTurnsLeft) * /*30*/ max(100, GC.getGame().getGameSpeedInfo().getTechCostPerTurnMultiplier());
	float fItemMultiplier = (float)(pow( (double) std::max(1, (iTechEra)), (double) /*0.7*/ GD_FLOAT_GET(TECH_COST_ERA_EXPONENT) ) );
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

	if (bFromMe)
	{
		if(GetPlayer()->GetDiplomacyAI()->GetVictoryBlockLevel(eOtherPlayer) >= BLOCK_LEVEL_STRONG || GetPlayer()->GetDiplomacyAI()->GetVictoryDisputeLevel(eOtherPlayer) >= DISPUTE_LEVEL_STRONG)
		{
			iItemValue *= 5;
		}
		if(GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eOtherPlayer) < CIV_OPINION_FRIEND && !GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eOtherPlayer))
		{
			iItemValue *= 5;
		}

		// Approach is important
		switch (GetPlayer()->GetDiplomacyAI()->GetSurfaceApproach(eOtherPlayer))
		{
			case CIV_APPROACH_WAR:
				iItemValue *= 250;
				break;
			case CIV_APPROACH_HOSTILE:
				iItemValue *= 150;
				break;
			case CIV_APPROACH_GUARDED:
				iItemValue *= 125;
				break;
			case CIV_APPROACH_DECEPTIVE:
				iItemValue *= 100;
				break;
			case CIV_APPROACH_AFRAID:
				iItemValue *= 80;
				break;
			case CIV_APPROACH_FRIENDLY:
				iItemValue *= 90;
				break;
			case CIV_APPROACH_NEUTRAL:
				iItemValue *= 100;
				break;
			default:
				iItemValue *= 100;
				break;
		}
		iItemValue /= 100;

	}

	else
	{
		switch(GetPlayer()->GetDiplomacyAI()->GetSurfaceApproach(eOtherPlayer))
		{
			case CIV_APPROACH_WAR:
				iItemValue *= 50;
				break;
			case CIV_APPROACH_HOSTILE:
				iItemValue *= 50;
				break;
			case CIV_APPROACH_GUARDED:
				iItemValue *= 75;
				break;
			case CIV_APPROACH_DECEPTIVE:
				iItemValue *= 110;
				break;
			case CIV_APPROACH_AFRAID:
				iItemValue *= 100;
				break;
			case CIV_APPROACH_FRIENDLY:
				iItemValue *= 110;
				break;
			case CIV_APPROACH_NEUTRAL:
				iItemValue *= 110;
				break;
			default:
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
int CvDealAI::GetVassalageValue(bool bFromMe, PlayerTypes eOtherPlayer)
{
	int iItemValue = 0;
	CvDiplomacyAI* m_pDiploAI = GetPlayer()->GetDiplomacyAI();

	if (bFromMe)
	{
		if (!m_pDiploAI->IsVassalageAcceptable(eOtherPlayer, false))
		{
			return INT_MAX;
		}

		// Initial Vassalage deal value at 2000
		iItemValue = 2000;

		if (m_pPlayer->IsAtWarWith(eOtherPlayer))
		{
			return iItemValue / 2;
		}

		// Add deal value based on number of wars player is currently fighting (including with minors)
		iItemValue += iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).getAtWarCount(true));

		// Increase deal value based on number of vassals player already has
		iItemValue += iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetNumVassals());

		// ex: 2 wars and 2 vassals = 500 + 1000 + 1000 = a 2500 gold before modifiers!

		// The point of Vassalage is protection, if they're not militarily dominant - what's the point?
		switch (m_pDiploAI->GetMilitaryStrengthComparedToUs(eOtherPlayer))
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
			if (m_pPlayer->IsAtWarWith(eOtherPlayer))
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
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;

		switch (m_pDiploAI->GetSurfaceApproach(eOtherPlayer))
		{
		case CIV_APPROACH_WAR:
			iItemValue *= 100;
			break;
		case CIV_APPROACH_HOSTILE:
			iItemValue *= 250;
			break;
		case CIV_APPROACH_GUARDED:
			iItemValue *= 130;
			break;
		case CIV_APPROACH_DECEPTIVE:
			iItemValue *= 100;
			break;
		case CIV_APPROACH_AFRAID:
			iItemValue *= 75;
			break;
		case CIV_APPROACH_FRIENDLY:
			iItemValue *= 85;
			break;
		case CIV_APPROACH_NEUTRAL:
			iItemValue *= 100;
			break;
		default:
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;
	}
	//from them?
	else
	{
		// we don't want to do it?
		if (!m_pDiploAI->IsVassalageAcceptable(m_pPlayer->GetID(), true))
		{
			return INT_MAX;
		}

		// Initial Vassalage deal value at 1000
		iItemValue = 1000;

		if (m_pPlayer->IsAtWarWith(eOtherPlayer))
		{
			return iItemValue / 2;
		}

		// Add deal value based on number of wars player is currently fighting (including with minors)
		iItemValue -= iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).getAtWarCount(true));

		// Increase deal value based on number of vassals player already has
		iItemValue -= iItemValue * min(1, GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).GetNumVassals());

		// ex: 2 wars and 2 vassals = 500 + 1000 + 1000 = a 2500 gold before modifiers!

		// The point of Vassalage is protection, if they're militarily dominant - what's the point?
		switch (m_pDiploAI->GetMilitaryStrengthComparedToUs(eOtherPlayer))
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
			iItemValue *= 100;
			break;
		}
		iItemValue /= 100;

		switch (m_pDiploAI->GetSurfaceApproach(eOtherPlayer))
		{
		case CIV_APPROACH_WAR:
			iItemValue *= 100;
			break;
		case CIV_APPROACH_HOSTILE:
			iItemValue *= 50;
			break;
		case CIV_APPROACH_GUARDED:
			iItemValue *= 60;
			break;
		case CIV_APPROACH_DECEPTIVE:
			iItemValue *= 100;
			break;
		case CIV_APPROACH_AFRAID:
			iItemValue *= 125;
			break;
		case CIV_APPROACH_FRIENDLY:
			iItemValue *= 150;
			break;
		case CIV_APPROACH_NEUTRAL:
			iItemValue *= 125;
			break;
		default:
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
	int iItemValue = 0;
	CvDiplomacyAI* m_pDiploAI = GetPlayer()->GetDiplomacyAI();

	//They're asking me to revoke my vassals? Quoi?
	if (bFromMe)
	{
		// Initial End Vassalage deal value at 1000
		iItemValue = 1000;

		//War? Refuse if we're not losing badly.
		if (bWar)
		{
			if (m_pDiploAI->GetWarScore(eOtherPlayer) >= -75)
			{
				return INT_MAX;
			}
			else
			{
				return iItemValue;
			}
		}

		if (GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer) == CIV_APPROACH_WAR)
			return INT_MAX;

		if (GetPlayer()->GetDiplomacyAI()->IsCloseToWorldConquest())
			return INT_MAX;

		if (GetPlayer()->GetDiplomacyAI()->IsCloseToDiploVictory())
			return INT_MAX;

		if (GetPlayer()->GetDiplomacyAI()->IsUntrustworthy(eOtherPlayer))
			return INT_MAX;

		// Increase deal value based on number of vassals we have
		int iCapitalsControlled = 0;
		int iVassalsControlled = 0;
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if (GET_PLAYER(eLoopPlayer).isAlive() && GetPlayer()->GetDiplomacyAI()->IsMaster(eLoopPlayer))
			{
				iItemValue += GET_PLAYER(eLoopPlayer).GetMilitaryMight() / 4 + GET_PLAYER(eLoopPlayer).GetEconomicMight();
				iVassalsControlled++;

				int iCityLoop = 0;
				for (CvCity* pCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pCity != NULL; pCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
				{
					if (pCity->IsOriginalMajorCapital())
						iCapitalsControlled++;
				}
			}
		}

		if (iCapitalsControlled > 0 && GetPlayer()->GetDiplomacyAI()->IsGoingForWorldConquest())
		{
			if (!GetPlayer()->IsInTerribleShapeForWar())
				return INT_MAX;

			iItemValue *= iCapitalsControlled + 1;
		}

		if (GetPlayer()->GetDiplomacyAI()->IsGoingForDiploVictory())
		{
			iItemValue *= iVassalsControlled + 1;
		}

		// What's the power of the asking party? They need to be real strong to push us out of this.
		switch(m_pDiploAI->GetMilitaryStrengthComparedToUs(eOtherPlayer))
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

		switch (m_pDiploAI->GetSurfaceApproach(eOtherPlayer))
		{
			case CIV_APPROACH_WAR:
				iItemValue *= 1000;
				break;
			case CIV_APPROACH_HOSTILE:
				iItemValue *= 500;
				break;
			case CIV_APPROACH_GUARDED:
				iItemValue *= 250;
				break;
			case CIV_APPROACH_DECEPTIVE:
				iItemValue *= 80;
				break;
			case CIV_APPROACH_AFRAID:
				iItemValue *= 60;
				break;
			case CIV_APPROACH_FRIENDLY:
				iItemValue *= 80;
				break;
			case CIV_APPROACH_NEUTRAL:
				iItemValue *= 125;
				break;
			default:
				iItemValue *= 100;
				break;
		}
		iItemValue /= 100;

		switch(m_pDiploAI->GetCivOpinion(eOtherPlayer))
		{
			case CIV_OPINION_ALLY:
				iItemValue *= 70;
				break;
			case CIV_OPINION_FRIEND:
				iItemValue *= 85;
				break;
			case CIV_OPINION_FAVORABLE:
				iItemValue *= 95;
				break;
			case CIV_OPINION_NEUTRAL:
				iItemValue *= 150;
				break;
			case CIV_OPINION_COMPETITOR:
			case CIV_OPINION_ENEMY:
			case CIV_OPINION_UNFORGIVABLE:
				iItemValue *= 500;
				break;
			default:
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
		// Increase deal value based on number of vassals they have
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

							iItemValue += GET_PLAYER(eVassalPlayer).GetMilitaryMight() / 4 + GET_PLAYER(eVassalPlayer).GetEconomicMight();

							// Did the vassal previously resurrect or liberate us?
							if (GetPlayer()->GetDiplomacyAI()->WasResurrectedBy(eVassalPlayer) || GetPlayer()->GetDiplomacyAI()->IsMasterLiberatedMeFromVassalage(eVassalPlayer))
							{
								iItemValue *= 2;
								bWorthIt = true;
								break;
							}
							
							//Are we friends with a vassal? If so, we care about this. Otherwise, meh.
							switch (GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eVassalPlayer))
							{
								case CIV_OPINION_ALLY:
									iItemValue *= 2;
									bWorthIt = true;
									break;
								case CIV_OPINION_FRIEND:
									bWorthIt = true;
									break;
								case CIV_OPINION_FAVORABLE:
									if (GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eVassalPlayer) && GetPlayer()->GetDiplomacyAI()->GetCivApproach(eVassalPlayer) == CIV_APPROACH_FRIENDLY)
									{
										bWorthIt = true;
									}
									break;
								case CIV_OPINION_NEUTRAL:
								case CIV_OPINION_COMPETITOR:
								case CIV_OPINION_ENEMY:
								case CIV_OPINION_UNFORGIVABLE:
									break; // meh.
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
	if (GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer) == CIV_APPROACH_HOSTILE)
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

		// AI evaluation
		bool bUselessReferenceVariable = false;
		bool bCantMatchOffer = false;
		bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

		return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
	}
	return false;
}

/// A good time to make an offer for technology?
bool CvDealAI::IsMakeOfferForTech(PlayerTypes eOtherPlayer, CvDeal* pDeal)
{
	CvAssert(eOtherPlayer >= 0);
	CvAssert(eOtherPlayer < MAX_MAJOR_CIVS);

	// Don't ask for Technology if we're hostile or planning war
	CivApproachTypes eApproach = GetPlayer()->GetDiplomacyAI()->GetCivApproach(eOtherPlayer);
	if (eApproach <= CIV_APPROACH_HOSTILE || eApproach == CIV_APPROACH_GUARDED)
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

	int iTechLoop = 0;
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

		// AI evaluation
		bool bUselessReferenceVariable = false;
		bool bCantMatchOffer = false;
		bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

		return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
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

	if (m_pPlayer->IsAtWarWith(eOtherPlayer))
	{
		return false;
	}

	// we don't want to do it?
	if (!GetPlayer()->GetDiplomacyAI()->IsVassalageAcceptable(eOtherPlayer, true))
	{
		return false;
	}

	// they don't want to do it?
	if (!GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsVassalageAcceptable(GetPlayer()->GetID(), false))
	{
		return false;
	}
	
	// Seed the deal with the item we want
	pDeal->AddVassalageTrade(eOtherPlayer);

	// AI evaluation
	bool bUselessReferenceVariable = false;
	bool bCantMatchOffer = false;
	bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

	return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
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

	if (m_pPlayer->IsAtWarWith(eOtherPlayer))
	{
		return false;
	}

	// we don't want to do it?
	if (!GetPlayer()->GetDiplomacyAI()->IsVassalageAcceptable(eOtherPlayer, false))
	{
		return false;
	}

	// they don't want to do it?
	if (!GET_PLAYER(eOtherPlayer).isHuman() && !GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->IsVassalageAcceptable(GetPlayer()->GetID(), true))
	{
		return false;
	}

	// Seed the deal with the item we want
	pDeal->AddVassalageTrade(GetPlayer()->GetID());

	// AI evaluation
	bool bUselessReferenceVariable = false;
	bool bCantMatchOffer = false;
	bool bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work

	return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
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
					const vector<PlayerTypes>& vVassalTeam = GET_TEAM(eLoopTeam).getPlayers();
					for (size_t iPlayerLoop = 0; iPlayerLoop < vVassalTeam.size(); ++iPlayerLoop)
					{
						PlayerTypes eVassalPlayer = vVassalTeam[iPlayerLoop];

						// Check if this player actually wants to be free of their vassalage.
						// If not, we never consider freeing the team worth it and exit immediately.
						// 
						// Note: Any one player not wanting to be free is enough for us to disregard the rest of the team.
						if (!GET_PLAYER(eVassalPlayer).GetDiplomacyAI()->IsEndVassalageWithPlayerAcceptable(eOtherPlayer))
						{
							bWorthIt = false;
							break;
						}
						// Now check if we actually care about this player.
						// 
						// Note: We only need to care for one player on the team to consider freeing the team.
						if (!bWorthIt)
						{
							bWorthIt = (GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eVassalPlayer) >= CIV_OPINION_FRIEND)
								|| (GetPlayer()->GetDiplomacyAI()->GetCivOpinion(eVassalPlayer) >= CIV_OPINION_FAVORABLE
									&& GetPlayer()->GetDiplomacyAI()->IsDoFAccepted(eVassalPlayer)
									&& GetPlayer()->GetDiplomacyAI()->GetCivApproach(eVassalPlayer) == CIV_APPROACH_FRIENDLY)
								|| (GetPlayer()->GetDiplomacyAI()->WasResurrectedBy(eVassalPlayer)
									|| GetPlayer()->GetDiplomacyAI()->IsMasterLiberatedMeFromVassalage(eVassalPlayer));
						}
					}
				}
			}
		}
	}
	bool bDealAcceptable = false;
	bool bCantMatchOffer = false;
	if(bWorthIt)
	{
		// Seed the deal with the item we want
		pDeal->AddRevokeVassalageTrade(eOtherPlayer);
		// AI evaluation
		bool bUselessReferenceVariable = false;
		bDealAcceptable = DoEqualizeDeal(pDeal, eOtherPlayer, bUselessReferenceVariable, bCantMatchOffer);	// Change the deal as necessary to make it work
	}

	return bDealAcceptable && !bCantMatchOffer && pDeal->GetNumItems() > 0;
}

/// See if adding Maps to their side of the deal helps even out pDeal
void CvDealAI::DoAddMapsToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add World Map to Them, but them is us.  Please show Jon");

	// Not allowed in demands.
	if (pDeal->GetDemandingPlayer() != NO_PLAYER)
		return;

//	if(!bDontChangeTheirExistingItems)
	{
		if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if they can actually trade it to us
			if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_MAPS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_MAPS, /*bFromMe*/ false, eThem, -1, -1, -1, /*bFlag1*/false, -1, true);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (pDeal->GetDemandingPlayer() != NO_PLAYER || !TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, false))
				{
					pDeal->AddMapTrade(eThem);
					return;
				}
			}
		}
	}
}

/// See if adding Maps to our side of the deal helps even out pDeal
void CvDealAI::DoAddMapsToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add World Map to Us, but them is us.  Please show Jon");

	//if(!bDontChangeMyExistingItems)
	{
		if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			// See if we can actually trade it to them
			if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_MAPS))
			{
				int iItemValue = GetTradeItemValue(TRADE_ITEM_MAPS, /*bFromMe*/ true, eThem, -1, -1, -1, /*bFlag1*/false, -1, true);

				// If adding this to the deal doesn't take it over the limit, do it
				if(iItemValue==INT_MAX)
				{
					return;
				}
				if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
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
void CvDealAI::DoAddTechToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	// Not allowed in demands.
	if (pDeal->GetDemandingPlayer() != NO_PLAYER)
		return;

	CvWeightedVector<int> viTradeValues;
	//if(!bDontChangeTheirExistingItems)
	{
		if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iItemValue = 0;

			int iTechLoop = 0;
			TechTypes eTech;

			// Loop through each Tech
			for(iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
			{
				eTech = (TechTypes) iTechLoop;

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eThem, eMyPlayer, TRADE_ITEM_TECHS, eTech))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_TECHS, /*bFromMe*/ false, eThem, eTech, -1, -1, /*bFlag1*/false, -1, true);

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
			viTradeValues.StableSortItems();
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
	
						if (pDeal->GetDemandingPlayer() != NO_PLAYER || !TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iWeight, false))
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
void CvDealAI::DoAddTechToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	//if(!bDontChangeMyExistingItems)
	{
		if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
		{
			PlayerTypes eMyPlayer = GetPlayer()->GetID();

			int iItemValue = 0;

			int iTechLoop = 0;
			TechTypes eTech;

			// Loop through each Tech
			for(iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
			{
				eTech = (TechTypes) iTechLoop;

				// See if they can actually trade it to us
				if(pDeal->IsPossibleToTradeItem(eMyPlayer, eThem, TRADE_ITEM_TECHS, eTech))
				{
					iItemValue = GetTradeItemValue(TRADE_ITEM_TECHS, /*bFromMe*/ true, eThem, eTech, -1, -1, /*bFlag1*/false, -1, true);

					if(iItemValue==INT_MAX)
					{
						return;
					}
					if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
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

void CvDealAI::DoAddVassalageToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	if (!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eThem, TRADE_ITEM_VASSALAGE))
		return;

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
	{
		PlayerTypes eMyPlayer = GetPlayer()->GetID();

		int iItemValue = 0;

		if (GetPlayer()->IsAtWarWith(eThem))
		{
			return;
		}

		// we don't want to do it?
		if (!GetPlayer()->GetDiplomacyAI()->IsVassalageAcceptable(eThem, false))
		{
			return;
		}

		// they don't want to do it?
		if (!GET_PLAYER(eThem).isHuman() && !GET_PLAYER(eThem).GetDiplomacyAI()->IsVassalageAcceptable(GetPlayer()->GetID(), true))
		{
			return;
		}

		iItemValue = GetTradeItemValue(TRADE_ITEM_VASSALAGE, /*bFromMe*/ true, eThem, -1, -1, -1, false, -1, true);

		if (iItemValue == INT_MAX)
		{
			return;
		}
		if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
		{
			pDeal->AddVassalageTrade(eMyPlayer);
			iTotalValue = GetDealValue(pDeal);
		}
	}
}

void CvDealAI::DoAddVassalageToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Vassalage to Them, but them is us.  Please show Jon");

	if (!pDeal->IsPossibleToTradeItem(eThem, GetPlayer()->GetID(), TRADE_ITEM_VASSALAGE))
		return;

	if (pDeal->GetDemandingPlayer() != NO_PLAYER)
	{
		if (GET_PLAYER(eThem).isHuman())
		{
			// Assume humans won't agree unless they're way weaker.
			if (GetPlayer()->GetDiplomacyAI()->GetRawTargetValue(eThem) < TARGET_VALUE_SOFT)
				return;
		}
	}
	else
	{
		if (!GetPlayer()->IsAtWarWith(eThem) && GET_PLAYER(eThem).isHuman())
		{
			return;
		}
	}

	// we don't want to do it?
	if (!GetPlayer()->GetDiplomacyAI()->IsVassalageAcceptable(eThem, true))
	{
		return;
	}

	// they don't want to do it?
	if (!GET_PLAYER(eThem).isHuman() && !GET_PLAYER(eThem).GetDiplomacyAI()->IsVassalageAcceptable(GetPlayer()->GetID(), false))
	{
		return;
	}

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue <= iThresholdValue))
	{
		int iItemValue = 0;

		iItemValue = GetTradeItemValue(TRADE_ITEM_VASSALAGE, /*bFromMe*/ false, eThem, -1, -1, -1, false, -1, true);

		if (iItemValue == INT_MAX)
		{
			return;
		}
		if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
		{
			pDeal->AddVassalageTrade(eThem);
			iTotalValue = GetDealValue(pDeal);
		}
	}
}

void CvDealAI::DoAddRevokeVassalageToUs(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	if (!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), eThem, TRADE_ITEM_VASSALAGE_REVOKE))
		return;

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
	{
		int iItemValue = 0;

		iItemValue = GetTradeItemValue(TRADE_ITEM_VASSALAGE_REVOKE, /*bFromMe*/ true, eThem, -1, -1, -1, false, -1, true);

		if (iItemValue == INT_MAX)
		{
			return;
		}
		if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
		{
			pDeal->AddRevokeVassalageTrade(eThem);
			iTotalValue = GetDealValue(pDeal);
		}
	}
}

void CvDealAI::DoAddRevokeVassalageToThem(CvDeal* pDeal, PlayerTypes eThem, int& iTotalValue, int iThresholdValue)
{
	CvAssert(eThem >= 0);
	CvAssert(eThem < MAX_MAJOR_CIVS);
	CvAssertMsg(eThem != GetPlayer()->GetID(), "DEAL_AI: Trying to add Technology to Them, but them is us.  Please show Jon");

	// Not allowed in demands.
	if (pDeal->GetDemandingPlayer() != NO_PLAYER)
		return;

	if (!pDeal->IsPossibleToTradeItem(eThem, GetPlayer()->GetID(), TRADE_ITEM_VASSALAGE_REVOKE))
		return;

	if ((iThresholdValue != 0 || !WithinAcceptableRange(eThem, pDeal->GetMaxValue(), iTotalValue)) && (iThresholdValue == 0 || iTotalValue >= iThresholdValue))
	{
		int iItemValue = 0;

		iItemValue = GetTradeItemValue(TRADE_ITEM_VASSALAGE_REVOKE, /*bFromMe*/ false, eThem, -1, -1, -1, false, -1, true);

		if (iItemValue == INT_MAX)
		{
			return;
		}
		if (!TooMuchAdded(eThem, pDeal->GetMaxValue(), iTotalValue-iThresholdValue, iItemValue, true))
		{
			pDeal->AddRevokeVassalageTrade(eThem);
			iTotalValue = GetDealValue(pDeal);
		}
	}
}