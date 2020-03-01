/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllNetMessageHandler.h"
#include "CvDllContext.h"

#include "CvDiplomacyAI.h"
#include "CvTypes.h"
#include "CvGameCoreUtils.h"
#include "CvDllNetMessageExt.h"

bool PlayerInvalid(PlayerTypes ePlayer)
{
	return ePlayer<0 || ePlayer>=MAX_PLAYERS;
}

CvDllNetMessageHandler::CvDllNetMessageHandler()
{
}
//------------------------------------------------------------------------------
CvDllNetMessageHandler::~CvDllNetMessageHandler()
{
}
//------------------------------------------------------------------------------
void* CvDllNetMessageHandler::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvNetMessageHandler1::GetInterfaceId() ||
	        guidInterface == ICvNetMessageHandler2::GetInterfaceId() ||
			guidInterface == ICvNetMessageHandler3::GetInterfaceId())
	{
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::Destroy()
{
	// Do nothing.
	// This is a static class whose instance is managed externally.
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllNetMessageHandler::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseAdvancedStartAction(PlayerTypes ePlayer, AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	GET_PLAYER(ePlayer).doAdvancedStartAction(eAction, iX, iY, iData, bAdd);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseAutoMission(PlayerTypes ePlayer, int iUnitID)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pkUnit = kPlayer.getUnit(iUnitID);
	if(pkUnit)
	{
		pkUnit->AutoMission();
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseBarbarianRansom(PlayerTypes ePlayer, int iOptionChosen, int iUnitID)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	// Pay ransom
	if(iOptionChosen == 0)
	{
		CvTreasury* pkTreasury = kPlayer.GetTreasury();
		int iNumGold = /*100*/ GC.getBARBARIAN_UNIT_GOLD_RANSOM_exp();
		const int iTreasuryGold = pkTreasury->GetGold();
		if(iNumGold > iTreasuryGold)
		{
			iNumGold = iTreasuryGold;
		}

		pkTreasury->ChangeGold(-iNumGold);
	}
	// Abandon Unit
	else if(iOptionChosen == 1)
	{
		CvUnit* pkUnit = kPlayer.getUnit(iUnitID);
		if(pkUnit != NULL)
			pkUnit->kill(true, BARBARIAN_PLAYER);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseChangeWar(PlayerTypes ePlayer, TeamTypes eRivalTeam, bool bWar)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());
	const TeamTypes eTeam = kPlayer.getTeam();

	FAssert(eTeam != eRivalTeam);

	if(bWar)
	{
#if defined(MOD_EVENTS_WAR_AND_PEACE)
		kTeam.declareWar(eRivalTeam, false, ePlayer);
#else
		kTeam.declareWar(eRivalTeam);
#endif
	}
	else
	{
#if defined(MOD_EVENTS_WAR_AND_PEACE)
		kTeam.makePeace(eRivalTeam, true, false, ePlayer);
#else
		kTeam.makePeace(eRivalTeam);
#endif
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseIgnoreWarning(PlayerTypes ePlayer, TeamTypes eRivalTeam)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());
	const TeamTypes eTeam = kPlayer.getTeam();
	FAssert(eTeam != eRivalTeam);
	
	kTeam.PushIgnoreWarning(eRivalTeam);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCityBuyPlot(PlayerTypes ePlayer, int iCityID, int iX, int iY)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);
	if(pkCity != NULL)
	{
		CvPlot* pkPlot = NULL;

		// (-1,-1) means pick a random plot to buy
		if(iX == -1 && iY == -1)
		{
#if defined(MOD_BALANCE_CORE)
			pkPlot = pkCity->GetNextBuyablePlot(false);
#else
			pkPlot = pkCity->GetNextBuyablePlot();
#endif
		}
		else
		{
			pkPlot = GC.getMap().plot(iX, iY);
		}

		if(pkPlot != NULL)
		{
			if(pkCity->CanBuyPlot(pkPlot->getX(), pkPlot->getY()))
			{
				pkCity->BuyPlot(pkPlot->getX(), pkPlot->getY());
				if(ePlayer == GC.getGame().getActivePlayer() && GC.GetEngineUserInterface()->isCityScreenUp())
				{
					GC.GetEngineUserInterface()->setDirty(CityScreen_DIRTY_BIT, true);
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCityDoTask(PlayerTypes ePlayer, int iCityID, TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);

	if(pkCity != NULL)
	{
		pkCity->doTask(eTask, iData1, iData2, bOption, bAlt, bShift, bCtrl);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCityPopOrder(PlayerTypes ePlayer, int iCityID, int iNum)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);
	if(pkCity != NULL)
	{
		pkCity->popOrder(iNum);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCityPurchase(PlayerTypes ePlayer, int iCityID, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	ResponseCityPurchase(ePlayer, iCityID, eUnitType, eBuildingType, eProjectType, NO_YIELD);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCityPurchase(PlayerTypes ePlayer, int iCityID, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, int ePurchaseYield)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);
	if(pkCity && ePurchaseYield >= -1 && ePurchaseYield < NUM_YIELD_TYPES)
	{
		pkCity->Purchase(eUnitType, eBuildingType, eProjectType, static_cast<YieldTypes>(ePurchaseYield));
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCityPushOrder(PlayerTypes ePlayer, int iCityID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);
	if(pkCity != NULL)
	{
		pkCity->pushOrder(eOrder, iData, -1, bAlt, bShift, bCtrl);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCitySwapOrder(PlayerTypes ePlayer, int iCityID, int iNum)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);
	if(pkCity != NULL)
	{
		pkCity->swapOrder(iNum);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseChooseElection(PlayerTypes ePlayer, int iSelection, int iVoteId)
{
	// Unused
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseDestroyUnit(PlayerTypes ePlayer, int iUnitID)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pkUnit = kPlayer.getUnit(iUnitID);

	if(pkUnit)
	{
		pkUnit->kill(true, ePlayer);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseDiplomacyFromUI(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	// hijacks message for MP events since it has a few args and is sent to everyone
	if (NetMessageExt::Process::FromDiplomacyFromUI(ePlayer, eOtherPlayer, eEvent, iArg1, iArg2))
		return;
	GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->DoFromUIDiploEvent(ePlayer, eEvent, iArg1, iArg2);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseDiploVote(PlayerTypes ePlayer, PlayerTypes eVotePlayer)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	TeamTypes eVotingTeam = GET_PLAYER(ePlayer).getTeam();
	TeamTypes eVote = GET_PLAYER(eVotePlayer).getTeam();

	GC.getGame().SetVoteCast(eVotingTeam, eVote);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseDoCommand(PlayerTypes ePlayer, int iUnitID, CommandTypes eCommand, int iData1, int iData2, bool bAlt)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pkUnit = kPlayer.getUnit(iUnitID);

	if(pkUnit != NULL)
	{
		if(bAlt && GC.getCommandInfo(eCommand)->getAll())
		{
			const UnitTypes eUnitType = pkUnit->getUnitType();

			CvUnit* pkLoopUnit = NULL;
			int iLoop = 0;

			for(pkLoopUnit = kPlayer.firstUnit(&iLoop); pkLoopUnit != NULL; pkLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				if(pkLoopUnit->getUnitType() == eUnitType)
				{
					pkLoopUnit->doCommand(eCommand, iData1, iData2);
				}
			}
		}
		else
		{
			pkUnit->doCommand(eCommand, iData1, iData2);
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseExtendedGame(PlayerTypes ePlayer)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	GET_PLAYER(ePlayer).makeExtendedGame();
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseFoundPantheon(PlayerTypes ePlayer, BeliefTypes eBelief)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvGame& kGame(GC.getGame());
	CvGameReligions* pkGameReligions(kGame.GetGameReligions());
	CvBeliefXMLEntries* pkBeliefs = GC.GetGameBeliefs();
	CvBeliefEntry* pEntry = pkBeliefs->GetEntry((int)eBelief);

	// Pantheon belief, or adding one through Reformation?
	if (pEntry && ePlayer != NO_PLAYER)
	{
		if (pEntry->IsPantheonBelief())
		{
			CvGameReligions::FOUNDING_RESULT eResult = pkGameReligions->CanCreatePantheon(ePlayer, true);
			if(eResult == CvGameReligions::FOUNDING_OK)
			{
#if defined(MOD_TRAITS_ANY_BELIEF)
				if(pkGameReligions->IsPantheonBeliefAvailable(eBelief, ePlayer))
#else
				if(pkGameReligions->IsPantheonBeliefAvailable(eBelief))
#endif
				{
					pkGameReligions->FoundPantheon(ePlayer, eBelief);
				}
				else
				{
					CvGameReligions::NotifyPlayer(ePlayer, CvGameReligions::FOUNDING_BELIEF_IN_USE);
				}
			}
			else
			{
				CvGameReligions::NotifyPlayer(ePlayer, eResult);
			}
		}
		else if (pEntry->IsReformationBelief())
		{
			CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
			if (!pkGameReligions->HasAddedReformationBelief(ePlayer) && kPlayer.GetReligions()->HasCreatedReligion())
			{
				ReligionTypes eReligion = kPlayer.GetReligions()->GetReligionCreatedByPlayer();
				pkGameReligions->AddReformationBelief(ePlayer, eReligion, eBelief);
			}
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, int iCityX, int iCityY)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvGame& kGame(GC.getGame());
	CvGameReligions* pkGameReligions(kGame.GetGameReligions());

	CvCity* pkCity = GC.getMap().plot(iCityX, iCityY)->getPlotCity();
	if(pkCity && ePlayer != NO_PLAYER)
	{
		CvGameReligions::FOUNDING_RESULT eResult = pkGameReligions->CanFoundReligion(ePlayer, eReligion, szCustomName, eBelief1, eBelief2, eBelief3, eBelief4, pkCity);
		if(eResult == CvGameReligions::FOUNDING_OK)
			pkGameReligions->FoundReligion(ePlayer, eReligion, szCustomName, eBelief1, eBelief2, eBelief3, eBelief4, pkCity);
		else
		{
			CvGameReligions::NotifyPlayer(ePlayer, eResult);
			// We don't want them to lose the opportunity to found the religion, and the Great Prophet is already gone so just repost the notification
			// If someone beat them to the last religion, well... tough luck.
			CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
			if(kPlayer.isHuman() && eResult != CvGameReligions::FOUNDING_NO_RELIGIONS_AVAILABLE)
			{
				CvNotifications* pNotifications = kPlayer.GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FOUND_RELIGION");
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_FOUND_RELIGION");
#if defined(MOD_API_EXTENSIONS)
					pNotifications->Add(NOTIFICATION_FOUND_RELIGION, strBuffer, strSummary, iCityX, iCityY, eReligion, pkCity->GetID());
#else
					pNotifications->Add(NOTIFICATION_FOUND_RELIGION, strBuffer, strSummary, iCityX, iCityY, -1, pkCity->GetID());
#endif
				}
				kPlayer.GetReligions()->SetFoundingReligion(true);
			}
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseEnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, int iCityX, int iCityY)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvGame& kGame(GC.getGame());
	CvGameReligions* pkGameReligions(kGame.GetGameReligions());

	CvGameReligions::FOUNDING_RESULT eResult = pkGameReligions->CanEnhanceReligion(ePlayer, eReligion, eBelief1, eBelief2);
	if(eResult == CvGameReligions::FOUNDING_OK)
		pkGameReligions->EnhanceReligion(ePlayer, eReligion, eBelief1, eBelief2);
	else
	{
		CvGameReligions::NotifyPlayer(ePlayer, eResult);
		// We don't want them to lose the opportunity to enhance the religion, and the Great Prophet is already gone so just repost the notification
		CvCity* pkCity = GC.getMap().plot(iCityX, iCityY)->getPlotCity();
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		if(kPlayer.isHuman() && eResult != CvGameReligions::FOUNDING_NO_RELIGIONS_AVAILABLE && pkCity)
		{
			CvNotifications* pNotifications = kPlayer.GetNotifications();
			if(pNotifications)
			{
				CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENHANCE_RELIGION");
				CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENHANCE_RELIGION");
#if defined(MOD_API_EXTENSIONS)
				pNotifications->Add(NOTIFICATION_ENHANCE_RELIGION, strBuffer, strSummary, iCityX, iCityY, eReligion, pkCity->GetID());
#else
				pNotifications->Add(NOTIFICATION_ENHANCE_RELIGION, strBuffer, strSummary, iCityX, iCityY, -1, pkCity->GetID());
#endif
			}
			kPlayer.GetReligions()->SetFoundingReligion(true);
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMoveSpy(PlayerTypes ePlayer, int iSpyIndex, int iTargetPlayer, int iTargetCity, bool bAsDiplomat)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvPlayerEspionage* pPlayerEspionage = kPlayer.GetEspionage();

	if(pPlayerEspionage)
	{
		if(iTargetCity == -1)
		{
			pPlayerEspionage->ExtractSpyFromCity(iSpyIndex);
			GC.GetEngineUserInterface()->setDirty(EspionageScreen_DIRTY_BIT, true);
		}
		else
		{
			CvAssertMsg(iTargetPlayer != -1, "iTargetPlayer is -1");
			if(iTargetPlayer != -1)
			{
				PlayerTypes eTargetPlayer = (PlayerTypes)iTargetPlayer;
				CvCity* pCity = GET_PLAYER(eTargetPlayer).getCity(iTargetCity);
				CvAssertMsg(pCity, "pCity is null");
				if(pCity)
				{
					pPlayerEspionage->MoveSpyTo(pCity, iSpyIndex, bAsDiplomat);
					GC.GetEngineUserInterface()->setDirty(EspionageScreen_DIRTY_BIT, true);
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseStageCoup(PlayerTypes eSpyPlayer, int iSpyIndex)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(eSpyPlayer))
		return;

	CvAssertMsg(eSpyPlayer != NO_PLAYER, "eSpyPlayer invalid");
	CvAssertMsg(iSpyIndex >= 0, "iSpyIndex invalid");

	CvPlayerAI& kPlayer = GET_PLAYER(eSpyPlayer);
	CvPlayerEspionage* pPlayerEspionage = kPlayer.GetEspionage();

	CvAssertMsg(pPlayerEspionage, "pPlayerEspionage is null");
	if(pPlayerEspionage)
	{
		bool bCoupSuccess = pPlayerEspionage->AttemptCoup(iSpyIndex);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseFaithPurchase(PlayerTypes ePlayer, FaithPurchaseTypes eFaithPurchaseType, int iFaithPurchaseIndex)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvAssertMsg(ePlayer != NO_PLAYER, "ePlayer invalid");
	CvAssertMsg(eFaithPurchaseType > -1, "Faith Purchase Type invalid");
	CvAssertMsg(iFaithPurchaseIndex > -1, "Faith Purchase Index invalid");

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	kPlayer.SetFaithPurchaseType(eFaithPurchaseType);
	kPlayer.SetFaithPurchaseIndex(iFaithPurchaseIndex);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseLeagueVoteEnact(LeagueTypes eLeague, int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(eVoter) || eLeague==NO_LEAGUE)
		return;

	CvAssertMsg(eLeague != NO_LEAGUE, "eLeague invalid");
	CvAssertMsg(eVoter != NO_PLAYER, "eVoter invalid");

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetLeague(eLeague);
	CvAssertMsg(pLeague->CanVote(eVoter), "eVoter not allowed to vote. Please send Anton your save file and version.");
	pLeague->DoVoteEnact(iResolutionID, eVoter, iNumVotes, iChoice);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseLeagueVoteRepeal(LeagueTypes eLeague, int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(eVoter) || eLeague==NO_LEAGUE)
		return;

	CvAssertMsg(eLeague != NO_LEAGUE, "eLeague invalid");
	CvAssertMsg(eVoter != NO_PLAYER, "eVoter invalid");

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetLeague(eLeague);
	CvAssertMsg(pLeague->CanVote(eVoter), "eVoter not allowed to vote. Please send Anton your save file and version.");
	pLeague->DoVoteRepeal(iResolutionID, eVoter, iNumVotes, iChoice);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseLeagueVoteAbstain(LeagueTypes eLeague, PlayerTypes eVoter, int iNumVotes)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(eVoter) || eLeague==NO_LEAGUE)
		return;

	CvAssertMsg(eLeague != NO_LEAGUE, "eLeague invalid");
	CvAssertMsg(eVoter != NO_PLAYER, "eVoter invalid");

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetLeague(eLeague);
	CvAssertMsg(pLeague->CanVote(eVoter), "eVoter not allowed to vote. Please send Anton your save file and version.");
	pLeague->DoVoteAbstain(eVoter, iNumVotes);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseLeagueProposeEnact(LeagueTypes eLeague, ResolutionTypes eResolution, PlayerTypes eProposer, int iChoice)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(eProposer) || eLeague==NO_LEAGUE)
		return;

	CvAssertMsg(eLeague != NO_LEAGUE, "eLeague invalid");
	CvAssertMsg(eResolution != NO_RESOLUTION, "eResolution invalid");
	CvAssertMsg(eProposer != NO_PLAYER, "eProposer invalid");

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetLeague(eLeague);
	CvAssertMsg(pLeague->CanProposeEnact(eResolution, eProposer, iChoice), "eProposer not allowed to enact Resolution. Please send Anton your save file and version.");
	pLeague->DoProposeEnact(eResolution, eProposer, iChoice);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseLeagueProposeRepeal(LeagueTypes eLeague, int iResolutionID, PlayerTypes eProposer)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(eProposer) || eLeague==NO_LEAGUE)
		return;

	CvAssertMsg(eLeague != NO_LEAGUE, "eLeague invalid");
	CvAssertMsg(eProposer != NO_PLAYER, "eProposer invalid");

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetLeague(eLeague);
	CvAssertMsg(pLeague->CanProposeRepeal(iResolutionID, eProposer), "eProposer not allowed to repeal Resolution. Please send Anton your save file and version.");
	pLeague->DoProposeRepeal(iResolutionID, eProposer);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseLeagueEditName(LeagueTypes eLeague, PlayerTypes ePlayer, const char* szCustomName)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetLeague(eLeague);
	pLeague->DoChangeCustomName(ePlayer, szCustomName);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSetSwappableGreatWork(PlayerTypes ePlayer, int iWorkClass, int iWorkIndex)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;
	
	// is this player alive
	if (GET_PLAYER(ePlayer).isAlive())
	{
		// -1 indicates that they want to clear the slot
		if (iWorkIndex == -1)
		{
			if (iWorkClass == GC.getInfoTypeForString("GREAT_WORK_ARTIFACT"))
			{
				GET_PLAYER(ePlayer).GetCulture()->SetSwappableArtifactIndex(-1);
			}
			else if (iWorkClass == GC.getInfoTypeForString("GREAT_WORK_ART"))
			{
				GET_PLAYER(ePlayer).GetCulture()->SetSwappableArtIndex(-1);
			}
			else if (iWorkClass == GC.getInfoTypeForString("GREAT_WORK_LITERATURE"))
			{
				GET_PLAYER(ePlayer).GetCulture()->SetSwappableWritingIndex(-1);
			}			
			else if (iWorkClass == GC.getInfoTypeForString("GREAT_WORK_MUSIC"))
			{
				GET_PLAYER(ePlayer).GetCulture()->SetSwappableMusicIndex(-1);
			}
		}
		else
		{
			// does this player control this work
			if (GET_PLAYER(ePlayer).GetCulture()->ControlsGreatWork(iWorkIndex))
			{
				if (iWorkClass == GC.getInfoTypeForString("GREAT_WORK_ARTIFACT"))
				{
					GET_PLAYER(ePlayer).GetCulture()->SetSwappableArtifactIndex(iWorkIndex);
				}
				else if (iWorkClass == GC.getInfoTypeForString("GREAT_WORK_ART"))
				{
					GET_PLAYER(ePlayer).GetCulture()->SetSwappableArtIndex(iWorkIndex);
				}
				else if (iWorkClass == GC.getInfoTypeForString("GREAT_WORK_LITERATURE"))
				{
					GET_PLAYER(ePlayer).GetCulture()->SetSwappableWritingIndex(iWorkIndex);
				}			
				else if (iWorkClass == GC.getInfoTypeForString("GREAT_WORK_MUSIC"))
				{
					GET_PLAYER(ePlayer).GetCulture()->SetSwappableMusicIndex(iWorkIndex);
				}				
			}
		}
		GC.GetEngineUserInterface()->setDirty(GreatWorksScreen_DIRTY_BIT, true);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSwapGreatWorks(PlayerTypes ePlayer1, int iWorkIndex1, PlayerTypes ePlayer2, int iWorkIndex2)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer1) || PlayerInvalid(ePlayer2))
		return;
	GC.getGame().GetGameCulture()->SwapGreatWorks(ePlayer1, iWorkIndex1, ePlayer2, iWorkIndex2);
}

//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMoveGreatWorks(PlayerTypes ePlayer, int iCity1, int iBuildingClass1, int iWorkIndex1,
													int iCity2, int iBuildingClass2, int iWorkIndex2)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;
	GC.getGame().GetGameCulture()->MoveGreatWorks(ePlayer, iCity1, iBuildingClass1, iWorkIndex1, iCity2, iBuildingClass2, iWorkIndex2);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseChangeIdeology(PlayerTypes ePlayer)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvAssertMsg(ePlayer != NO_PLAYER, "ePlayer invalid");

	// is this player alive
	CvPlayer &kPlayer = GET_PLAYER(ePlayer);
	if (kPlayer.isAlive())
	{
		PolicyBranchTypes ePreferredIdeology = kPlayer.GetCulture()->GetPublicOpinionPreferredIdeology();
		kPlayer.SetAnarchyNumTurns(GC.getSWITCH_POLICY_BRANCHES_ANARCHY_TURNS());
		kPlayer.GetPlayerPolicies()->DoSwitchIdeologies(ePreferredIdeology);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseGiftUnit(PlayerTypes ePlayer, PlayerTypes eMinor, int iUnitID)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvUnit* pkUnit = GET_PLAYER(ePlayer).getUnit(iUnitID);
	GET_PLAYER(eMinor).DoDistanceGift(ePlayer, pkUnit);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseLaunchSpaceship(PlayerTypes ePlayer, VictoryTypes eVictory)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());

	if(kTeam.canLaunch(eVictory))
	{
		kPlayer.launch(eVictory);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseLiberatePlayer(PlayerTypes ePlayer, PlayerTypes eLiberatedPlayer, int iCityID)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	GET_PLAYER(ePlayer).DoLiberatePlayer(eLiberatedPlayer, iCityID);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMinorCivBullyGold(PlayerTypes ePlayer, PlayerTypes eMinor, int iGold)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	GET_PLAYER(eMinor).GetMinorCivAI()->DoMajorBullyGold(ePlayer, iGold);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMinorCivBullyUnit(PlayerTypes ePlayer, PlayerTypes eMinor, UnitTypes eUnitType)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	GET_PLAYER(eMinor).GetMinorCivAI()->DoMajorBullyUnit(ePlayer, eUnitType);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMinorCivGiftGold(PlayerTypes ePlayer, PlayerTypes eMinor, int iGold)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	// Enough Gold?
	if(GET_PLAYER(ePlayer).GetTreasury()->GetGold() >= iGold)
	{
		GET_PLAYER(eMinor).GetMinorCivAI()->DoGoldGiftFromMajor(ePlayer, iGold);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMinorCivGiftTileImprovement(PlayerTypes eMajor, PlayerTypes eMinor, int iPlotX, int iPlotY)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(eMajor) || PlayerInvalid(eMinor))
		return;

	GET_PLAYER(eMinor).GetMinorCivAI()->DoTileImprovementGiftFromMajor(eMajor, iPlotX, iPlotY);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMinorCivBuyout(PlayerTypes eMajor, PlayerTypes eMinor)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(eMajor) || PlayerInvalid(eMinor))
		return;

	GET_PLAYER(eMinor).GetMinorCivAI()->DoBuyout(eMajor);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMinorNoUnitSpawning(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer) || PlayerInvalid(eMinor))
		return;

	GET_PLAYER(eMinor).GetMinorCivAI()->SetUnitSpawningDisabled(ePlayer, bValue);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponsePlayerDealFinalized(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, PlayerTypes eActBy, bool bAccepted)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(eFromPlayer) || PlayerInvalid(eToPlayer))
		return;

	CvGame& game = GC.getGame();
	PlayerTypes eActivePlayer = game.getActivePlayer();

	// is the deal valid?
#if defined(MOD_ACTIVE_DIPLOMACY)
	if(GC.getGame().isReallyNetworkMultiPlayer() && MOD_ACTIVE_DIPLOMACY)
	{
		if(!game.GetGameDeals().FinalizeMPDealLatest(eFromPlayer, eToPlayer, bAccepted, true))
		{
			Localization::String strMessage;
			Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_EXPIRED");

			CvPlayerAI& kToPlayer = GET_PLAYER(eToPlayer);
			CvPlayerAI& kFromPlayer = GET_PLAYER(eFromPlayer);
			CvPlayerAI& kActivePlayer = GET_PLAYER(eActivePlayer);

			strMessage = Localization::Lookup("TXT_KEY_DEAL_EXPIRED_FROM_YOU");
			strMessage << kToPlayer.getNickName();
			kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

			strMessage = Localization::Lookup("TXT_KEY_DEAL_EXPIRED_FROM_THEM");
			strMessage << kFromPlayer.getNickName();
			kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
		}
		else
		{
			CvPlayerAI& kToPlayer = GET_PLAYER(eToPlayer);
			CvPlayerAI& kFromPlayer = GET_PLAYER(eFromPlayer);
			if(bAccepted)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED");
				Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED_BY_THEM");
				strMessage << kToPlayer.getNickName();
				kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

				strSummary = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED");
				strMessage = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED_BY_YOU");
				strMessage << kFromPlayer.getNickName();
				kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
			}
			else
			{
				if(eActBy == eFromPlayer)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN");
					Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN_BY_YOU");
					strMessage << kToPlayer.getNickName();
					kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

					strSummary = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN");
					strMessage = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN_BY_THEM");
					strMessage << kFromPlayer.getNickName();
					kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
				}
				else
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_REJECTED");
					Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_REJECTED_BY_THEM");
					strMessage << kToPlayer.getNickName();
					kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

					strSummary = Localization::Lookup("TXT_KEY_DEAL_REJECTED");
					strMessage = Localization::Lookup("TXT_KEY_DEAL_REJECTED_BY_YOU");
					strMessage << kFromPlayer.getNickName();
					kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
				}
			}
		}
	}
	else
	{

		if(!game.GetGameDeals().FinalizeDeal(eFromPlayer, eToPlayer, bAccepted))
		{
			Localization::String strMessage;
			Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_EXPIRED");

			CvPlayerAI& kToPlayer = GET_PLAYER(eToPlayer);
			CvPlayerAI& kFromPlayer = GET_PLAYER(eFromPlayer);
			CvPlayerAI& kActivePlayer = GET_PLAYER(eActivePlayer);

			strMessage = Localization::Lookup("TXT_KEY_DEAL_EXPIRED_FROM_YOU");
			strMessage << kToPlayer.getNickName();
			kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

			strMessage = Localization::Lookup("TXT_KEY_DEAL_EXPIRED_FROM_THEM");
			strMessage << kFromPlayer.getNickName();
			kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
		}
		else
		{
			CvPlayerAI& kToPlayer = GET_PLAYER(eToPlayer);
			CvPlayerAI& kFromPlayer = GET_PLAYER(eFromPlayer);
			if(bAccepted)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED");
				Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED_BY_THEM");
				strMessage << kToPlayer.getNickName();
				kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

				strSummary = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED");
				strMessage = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED_BY_YOU");
				strMessage << kFromPlayer.getNickName();
				kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
			}
			else
			{
				if(eActBy == eFromPlayer)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN");
					Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN_BY_YOU");
					strMessage << kToPlayer.getNickName();
					kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

					strSummary = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN");
					strMessage = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN_BY_THEM");
					strMessage << kFromPlayer.getNickName();
					kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
				}
				else
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_REJECTED");
					Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_REJECTED_BY_THEM");
					strMessage << kToPlayer.getNickName();
					kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

					strSummary = Localization::Lookup("TXT_KEY_DEAL_REJECTED");
					strMessage = Localization::Lookup("TXT_KEY_DEAL_REJECTED_BY_YOU");
					strMessage << kFromPlayer.getNickName();
					kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
				}
			}
		}
	}

#else
	if(!game.GetGameDeals().FinalizeDeal(eFromPlayer, eToPlayer, bAccepted))
	{
		Localization::String strMessage;
		Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_EXPIRED");

		CvPlayerAI& kToPlayer = GET_PLAYER(eToPlayer);
		CvPlayerAI& kFromPlayer = GET_PLAYER(eFromPlayer);
		CvPlayerAI& kActivePlayer = GET_PLAYER(eActivePlayer);

		strMessage = Localization::Lookup("TXT_KEY_DEAL_EXPIRED_FROM_YOU");
		strMessage << kToPlayer.getNickName();
		kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

		strMessage = Localization::Lookup("TXT_KEY_DEAL_EXPIRED_FROM_THEM");
		strMessage << kFromPlayer.getNickName();
		kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
	}
	else
	{
		CvPlayerAI& kToPlayer = GET_PLAYER(eToPlayer);
		CvPlayerAI& kFromPlayer = GET_PLAYER(eFromPlayer);
		if(bAccepted)
		{
			Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED");
			Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED_BY_THEM");
			strMessage << kToPlayer.getNickName();
			kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

			strSummary = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED");
			strMessage = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED_BY_YOU");
			strMessage << kFromPlayer.getNickName();
			kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
		}
		else
		{
			if(eActBy == eFromPlayer)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN");
				Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN_BY_YOU");
				strMessage << kToPlayer.getNickName();
				kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

				strSummary = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN");
				strMessage = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN_BY_THEM");
				strMessage << kFromPlayer.getNickName();
				kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
			}
			else
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_REJECTED");
				Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_REJECTED_BY_THEM");
				strMessage << kToPlayer.getNickName();
				kFromPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1);

				strSummary = Localization::Lookup("TXT_KEY_DEAL_REJECTED");
				strMessage = Localization::Lookup("TXT_KEY_DEAL_REJECTED_BY_YOU");
				strMessage << kFromPlayer.getNickName();
				kToPlayer.GetNotifications()->Add(NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1);
			}
		}
	}
#endif
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponsePlayerOption(PlayerTypes ePlayer, PlayerOptionTypes eOption, bool bValue)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	GET_PLAYER(ePlayer).setOption(eOption, bValue);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponsePledgeMinorProtection(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue, bool bPledgeNowBroken)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	GET_PLAYER(eMinor).GetMinorCivAI()->DoChangeProtectionFromMajor(ePlayer, bValue, bPledgeNowBroken);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponsePushMission(PlayerTypes ePlayer, int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvUnit::dispatchingNetMessage(true);

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pkUnit = kPlayer.getUnit(iUnitID);

	if(pkUnit != NULL)
	{
		pkUnit->PushMission(eMission, iData1, iData2, iFlags, bShift, true);
	}

	CvUnit::dispatchingNetMessage(false);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseGreatPersonChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pCity = kPlayer.GetGreatPersonSpawnCity(eGreatPersonUnit);
	if(pCity)
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		pCity->GetCityCitizens()->DoSpawnGreatPerson(eGreatPersonUnit, true, false, MOD_GLOBAL_TRULY_FREE_GP);
#else
		pCity->GetCityCitizens()->DoSpawnGreatPerson(eGreatPersonUnit, true, false);
#endif
	}
	kPlayer.ChangeNumFreeGreatPeople(-1);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMayaBonusChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pCity = kPlayer.GetGreatPersonSpawnCity(eGreatPersonUnit);
	if(pCity)
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		pCity->GetCityCitizens()->DoSpawnGreatPerson(eGreatPersonUnit, true, false, MOD_GLOBAL_TRULY_FREE_GP);
#else
		pCity->GetCityCitizens()->DoSpawnGreatPerson(eGreatPersonUnit, true, false);
#endif
	}
	kPlayer.ChangeNumMayaBoosts(-1);
	kPlayer.GetPlayerTraits()->SetUnitBaktun(eGreatPersonUnit);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseFaithGreatPersonChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pCity = kPlayer.GetGreatPersonSpawnCity(eGreatPersonUnit);
	if(pCity)
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		pCity->GetCityCitizens()->DoSpawnGreatPerson(eGreatPersonUnit, true, true, false);
#else
		pCity->GetCityCitizens()->DoSpawnGreatPerson(eGreatPersonUnit, true, true);
#endif
	}
	kPlayer.ChangeNumFaithGreatPeople(-1);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseGoodyChoice(PlayerTypes ePlayer, int iPlotX, int iPlotY, GoodyTypes eGoody, int iUnitID)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	CvUnit* pUnit = kPlayer.getUnit(iUnitID);
	kPlayer.receiveGoody(pPlot, eGoody, pUnit);
}

//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseArchaeologyChoice(PlayerTypes ePlayer, ArchaeologyChoiceType eChoice)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	kPlayer.GetCulture()->DoArchaeologyChoice(eChoice);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseIdeologyChoice(PlayerTypes ePlayer, PolicyBranchTypes eChoice)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	kPlayer.GetPlayerPolicies()->SetPolicyBranchUnlocked(eChoice, true, false);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseRenameCity(PlayerTypes ePlayer, int iCityID, const char* szName)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);
	if(pkCity)
	{
		CvString strName = szName;
		pkCity->setName(strName);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseRenameUnit(PlayerTypes ePlayer, int iUnitID, const char* szName)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pkUnit = kPlayer.getUnit(iUnitID);
	if(pkUnit)
	{
		CvString strName = szName;
		pkUnit->setName(strName);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseResearch(PlayerTypes ePlayer, TechTypes eTech, int iDiscover, bool bShift)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	ResponseResearch(ePlayer, eTech, iDiscover, NO_PLAYER, bShift);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseResearch(PlayerTypes ePlayer, TechTypes eTech, int iDiscover, PlayerTypes ePlayerToStealFrom, bool bShift)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());

	// Free tech
	if(iDiscover > 0)
	{
		// Make sure we can research this tech for free
		if(kPlayer.GetPlayerTechs()->CanResearchForFree(eTech))
		{
			kTeam.setHasTech(eTech, true, ePlayer, true, true);

			if(iDiscover > 1)
			{
				if(ePlayer == GC.getGame().getActivePlayer())
				{
					kPlayer.chooseTech(iDiscover - 1);
				}
			}
			kPlayer.SetNumFreeTechs(max(0, iDiscover - 1));
		}
	}
	// Stealing tech
	else if(ePlayerToStealFrom != NO_PLAYER)
	{
		// make sure we can still take a tech
		CvAssertMsg(kPlayer.GetEspionage()->m_aiNumTechsToStealList[ePlayerToStealFrom] > 0, "No techs to steal from player");
		CvAssertMsg(kPlayer.GetEspionage()->m_aaPlayerStealableTechList[ePlayerToStealFrom].size() > 0, "No techs to be stolen from this player");
		CvAssertMsg(kPlayer.GetPlayerTechs()->CanResearch(eTech), "Player can't research this technology");
		CvAssertMsg(GET_TEAM(GET_PLAYER(ePlayerToStealFrom).getTeam()).GetTeamTechs()->HasTech(eTech), "ePlayerToStealFrom does not have the requested tech");
		if (kPlayer.GetEspionage()->m_aiNumTechsToStealList[ePlayerToStealFrom] > 0)
		{
			kTeam.setHasTech(eTech, true, ePlayer, true, true);
			kPlayer.GetEspionage()->m_aiNumTechsToStealList[ePlayerToStealFrom]--;
		}
	}
	// Normal tech
	else
	{
		CvPlayerTechs* pPlayerTechs = kPlayer.GetPlayerTechs();
		CvTeamTechs* pTeamTechs = kTeam.GetTeamTechs();

		if(eTech == NO_TECH)
		{
			kPlayer.clearResearchQueue();
		}
		else if(pPlayerTechs->CanEverResearch(eTech))
		{
			if((pTeamTechs->HasTech(eTech) || pPlayerTechs->IsResearchingTech(eTech)) && !bShift)
			{
				kPlayer.clearResearchQueue();
			}

			kPlayer.pushResearch(eTech, !bShift);
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseReturnCivilian(PlayerTypes ePlayer, PlayerTypes eToPlayer, int iUnitID, bool bReturn)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	GET_PLAYER(ePlayer).DoCivilianReturnLogic(bReturn, eToPlayer, iUnitID);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSellBuilding(PlayerTypes ePlayer, int iCityID, BuildingTypes eBuilding)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvCity* pCity = GET_PLAYER(ePlayer).getCity(iCityID);
	if(pCity)
	{
		pCity->GetCityBuildings()->DoSellBuilding(eBuilding);

#if defined(MOD_EVENTS_CITY)
		if (MOD_EVENTS_CITY) {
			GAMEEVENTINVOKE_HOOK(GAMEEVENT_CitySoldBuilding, ePlayer, iCityID, eBuilding);
		} else {
#endif
		ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
		if (pkScriptSystem) 
		{
			CvLuaArgsHandle args;
			args->Push(ePlayer);
			args->Push(iCityID);
			args->Push(eBuilding);

			bool bResult;
			LuaSupport::CallHook(pkScriptSystem, "CitySoldBuilding", args.get(), bResult);
		}
#if defined(MOD_EVENTS_CITY)
		}
#endif
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSetCityAIFocus(PlayerTypes ePlayer, int iCityID, CityAIFocusTypes eFocus)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvCity* pCity = GET_PLAYER(ePlayer).getCity(iCityID);
	if(pCity != NULL)
	{
		CvCityCitizens* pkCitizens = pCity->GetCityCitizens();
		if(pkCitizens != NULL)
		{
			pkCitizens->SetFocusType(eFocus, true);
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSetCityAvoidGrowth(PlayerTypes ePlayer, int iCityID, bool bAvoidGrowth)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvCity* pCity = GET_PLAYER(ePlayer).getCity(iCityID);
	if(pCity != NULL)
	{
		CvCityCitizens* pkCitizens = pCity->GetCityCitizens();
		if(pkCitizens != NULL)
		{
			pkCitizens->SetForcedAvoidGrowth(bAvoidGrowth, true);
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSwapUnits(PlayerTypes ePlayer, int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvUnit::dispatchingNetMessage(true);

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pkUnit = kPlayer.getUnit(iUnitID);

	if(pkUnit != NULL)
	{
		// Get target plot
		CvMap& kMap = GC.getMap();
		CvPlot* pkTargetPlot = kMap.plot(iData1, iData2);

		if(pkTargetPlot != NULL)
		{
			CvPlot* pkOriginationPlot = pkUnit->plot();

			// Find unit to move out
			for(int iI = 0; iI < pkTargetPlot->getNumUnits(); iI++)
			{
				CvUnit* pkUnit2 = pkTargetPlot->getUnitByIndex(iI);

				if(pkUnit2 && pkUnit2->AreUnitsOfSameType(*pkUnit,pkTargetPlot->needsEmbarkation(pkUnit)))
				{
					// Start the swap
					pkUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), iData1, iData2, CvUnit::MOVEFLAG_IGNORE_STACKING, bShift, true);

					// Move the other unit back out, again splitting if necessary
					pkUnit2->PushMission(CvTypes::getMISSION_MOVE_TO(), pkOriginationPlot->getX(), pkOriginationPlot->getY());
				}
			}
		}
	}
	CvUnit::dispatchingNetMessage(false);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseUpdateCityCitizens(PlayerTypes ePlayer, int iCityID)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvCity* pCity = GET_PLAYER(ePlayer).getCity(iCityID);
	if(NULL != pCity && pCity->GetCityCitizens())
	{
		CvCityCitizens* pkCitizens = pCity->GetCityCitizens();
		if(pkCitizens != NULL)
		{
			pkCitizens->DoVerifyWorkingPlots();
			pkCitizens->DoReallocateCitizens();
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseUpdatePolicies(PlayerTypes ePlayer, bool bNOTPolicyBranch, int iPolicyID, bool bValue)
{
	//safeguard
	if (!GC.getGame().isFinalInitialized() || PlayerInvalid(ePlayer))
		return;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	// Policy Update
	if(bNOTPolicyBranch)
	{
		const PolicyTypes ePolicy = static_cast<PolicyTypes>(iPolicyID);
		if(bValue)
		{
			kPlayer.doAdoptPolicy(ePolicy);
		}
		else
		{
			kPlayer.setHasPolicy(ePolicy, bValue);

			kPlayer.CalculateNetHappiness();
		}
	}
	// Policy Branch Update
	else
	{
		const PolicyBranchTypes eBranch = static_cast<PolicyBranchTypes>(iPolicyID);
		CvPlayerPolicies* pPlayerPolicies = kPlayer.GetPlayerPolicies();

		// If Branch was blocked by another branch, then unblock this one - this may be the only thing this NetMessage does
		if(pPlayerPolicies->IsPolicyBranchBlocked(eBranch))
		{
			// Can't switch to a Branch that's still locked. DoUnlockPolicyBranch below will handle this for us
			if(pPlayerPolicies->IsPolicyBranchUnlocked(eBranch))
			{
				//pPlayerPolicies->ChangePolicyBranchBlockedCount(eBranch, -1);
				pPlayerPolicies->DoSwitchToPolicyBranch(eBranch);
			}
		}

		// Unlock the branch if it hasn't been already
		if(!pPlayerPolicies->IsPolicyBranchUnlocked(eBranch))
		{
			pPlayerPolicies->DoUnlockPolicyBranch(eBranch);
		}
	}
}
//------------------------------------------------------------------------------
