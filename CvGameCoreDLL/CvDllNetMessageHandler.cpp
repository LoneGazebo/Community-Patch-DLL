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

#include "CvTypes.h"
#include "CvDiplomacyAI.h"
#include "CvMinorCivAI.h"


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
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvNetMessageHandler1::GetInterfaceId())
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
	GET_PLAYER(ePlayer).doAdvancedStartAction(eAction, iX, iY, iData, bAdd);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseAutoMission(PlayerTypes ePlayer, int iUnitID)
{
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
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	// Pay ransom
	if (iOptionChosen == 0)
	{
		CvTreasury* pkTreasury = kPlayer.GetTreasury();
		int iNumGold = /*100*/ GC.getBARBARIAN_UNIT_GOLD_RANSOM_exp();
		const int iTreasuryGold = pkTreasury->GetGold();
		if (iNumGold > iTreasuryGold)
		{
			iNumGold = iTreasuryGold;
		}

		pkTreasury->ChangeGold(-iNumGold);
	}
	// Abandon Unit
	else if (iOptionChosen == 1)
	{
		CvUnit* pkUnit = kPlayer.getUnit(iUnitID);
		if(pkUnit != NULL)
			pkUnit->kill(true, BARBARIAN_PLAYER);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseChangeWar(PlayerTypes ePlayer, TeamTypes eRivalTeam, bool bWar)
{
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());
	const TeamTypes eTeam = kPlayer.getTeam();

	FAssert(eTeam != eRivalTeam);

	if (bWar)
	{
		kTeam.declareWar(eRivalTeam);
	}
	else
	{
		kTeam.makePeace(eRivalTeam);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseIgnoreWarning(PlayerTypes ePlayer, TeamTypes eRivalTeam)
{
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCityBuyPlot(PlayerTypes ePlayer, int iCityID, int iX, int iY)
{
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);
	if(pkCity != NULL)
	{
		CvPlot* pkPlot = NULL;

		// (-1,-1) means pick a random plot to buy
		if (iX == -1 && iY == -1)
		{
			pkPlot = pkCity->GetNextBuyablePlot();
		}
		else
		{
			pkPlot = GC.getMap().plot(iX, iY);
		}

		if(pkPlot != NULL)
		{
			if (pkCity->CanBuyPlot(pkPlot->getX(), pkPlot->getY()))
			{
				pkCity->BuyPlot(pkPlot->getX(), pkPlot->getY());
				if (ePlayer == GC.getGame().getActivePlayer() && GC.GetEngineUserInterface()->isCityScreenUp())
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
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);

	if (pkCity != NULL)
	{
		pkCity->doTask(eTask, iData1, iData2, bOption, bAlt, bShift, bCtrl);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCityPopOrder(PlayerTypes ePlayer, int iCityID, int iNum)
{
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
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity* pkCity = kPlayer.getCity(iCityID);
	if(pkCity)
	{
		pkCity->Purchase(eUnitType, eBuildingType, eProjectType);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseCityPushOrder(PlayerTypes ePlayer, int iCityID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
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
	GET_PLAYER(eOtherPlayer).GetDiplomacyAI()->DoFromUIDiploEvent(ePlayer, eEvent, iArg1, iArg2);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseDiploVote(PlayerTypes ePlayer, PlayerTypes eVotePlayer)
{
	TeamTypes eVotingTeam = GET_PLAYER(ePlayer).getTeam();
	TeamTypes eVote = GET_PLAYER(eVotePlayer).getTeam();

	GC.getGame().SetVoteCast(eVotingTeam, eVote);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseDoCommand(PlayerTypes ePlayer, int iUnitID, CommandTypes eCommand, int iData1, int iData2, bool bAlt)
{
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pkUnit = kPlayer.getUnit(iUnitID);

	if (pkUnit != NULL)
	{
		if (bAlt && GC.getCommandInfo(eCommand)->getAll())
		{
			const UnitTypes eUnitType = pkUnit->getUnitType();

			CvUnit* pkLoopUnit = NULL;
			int iLoop = 0;

			for(pkLoopUnit = kPlayer.firstUnit(&iLoop); pkLoopUnit != NULL; pkLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				if (pkLoopUnit->getUnitType() == eUnitType)
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
	GET_PLAYER(ePlayer).makeExtendedGame();
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseGiftUnit(PlayerTypes ePlayer, PlayerTypes eMinor, int iUnitID)
{
	CvUnit* pkUnit = GET_PLAYER(ePlayer).getUnit(iUnitID);
	GET_PLAYER(eMinor).DoDistanceGift(ePlayer, pkUnit);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseLaunchSpaceship(PlayerTypes ePlayer, VictoryTypes eVictory)
{
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
	GET_PLAYER(ePlayer).DoLiberatePlayer(eLiberatedPlayer, iCityID);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMinorCivGiftGold(PlayerTypes ePlayer, PlayerTypes eMinor, int iGold)
{
	// Enough Gold?
	if (GET_PLAYER(ePlayer).GetTreasury()->GetGold() >= iGold)
	{
		GET_PLAYER(eMinor).GetMinorCivAI()->DoGoldGiftFromMajor(ePlayer, iGold);
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseMinorNoUnitSpawning(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue)
{
	GET_PLAYER(eMinor).GetMinorCivAI()->SetUnitSpawningDisabled(ePlayer, bValue);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponsePlayerDealFinalized(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, PlayerTypes eActBy, bool bAccepted)
{
	CvGame &game = GC.getGame();
	PlayerTypes eActivePlayer = game.getActivePlayer();

	// is the deal valid?
	if( !game.GetGameDeals()->FinalizeDeal( eFromPlayer, eToPlayer, bAccepted ) )
	{
		Localization::String strMessage;
		Localization::String strSummary = Localization::Lookup( "TXT_KEY_DEAL_EXPIRED" );

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
			kFromPlayer.GetNotifications()->Add( NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1 );

			strSummary = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED");
			strMessage = Localization::Lookup("TXT_KEY_DEAL_ACCEPTED_BY_YOU");
			strMessage << kFromPlayer.getNickName();
			kToPlayer.GetNotifications()->Add( NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1 );
		}
		else
		{
			if(eActBy == eFromPlayer)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN");
				Localization::String strMessage = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN_BY_YOU");
				strMessage << kToPlayer.getNickName();
				kFromPlayer.GetNotifications()->Add( NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eToPlayer, -1, -1 );

				strSummary = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN");
				strMessage = Localization::Lookup("TXT_KEY_DEAL_WITHDRAWN_BY_THEM");
				strMessage << kFromPlayer.getNickName();
				kToPlayer.GetNotifications()->Add( NOTIFICATION_PLAYER_DEAL_RESOLVED, strMessage.toUTF8(), strSummary.toUTF8(), eFromPlayer, -1, -1 );
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
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponsePlayerOption(PlayerTypes ePlayer, PlayerOptionTypes eOption, bool bValue)
{
	GET_PLAYER(ePlayer).setOption(eOption, bValue);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponsePushMission(PlayerTypes ePlayer, int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift)
{
	CvUnit::dispatchingNetMessage(true);

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pkUnit = kPlayer.getUnit(iUnitID);

	if (pkUnit != NULL)
	{
		pkUnit->PushMission(eMission, iData1, iData2, iFlags, bShift, true);
	}

	CvUnit::dispatchingNetMessage(false);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseGreatPersonChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit)
{
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvCity *pCapital = kPlayer.getCapitalCity();
	if (pCapital)
	{
		pCapital->GetCityCitizens()->DoSpawnGreatPerson(eGreatPersonUnit);
	}
	kPlayer.ChangeNumFreeGreatPeople(-1);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseRenameCity(PlayerTypes ePlayer, int iCityID, const char* szName)
{
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
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());

	// Free tech
	if (iDiscover > 0)
	{
		kTeam.setHasTech(eTech, true, ePlayer, true, true);

		if (iDiscover > 1)
		{
			if (ePlayer == GC.getGame().getActivePlayer())
			{
				kPlayer.chooseTech(iDiscover - 1);
			}
		}
		kPlayer.SetNumFreeTechs(max(0, iDiscover - 1));
	}
	// Normal tech
	else
	{
		CvPlayerTechs* pPlayerTechs = kPlayer.GetPlayerTechs();
		CvTeamTechs* pTeamTechs = kTeam.GetTeamTechs();

		if (eTech == NO_TECH)
		{
			kPlayer.clearResearchQueue();
		}
		else if (pPlayerTechs->CanEverResearch(eTech))
		{
			if ((pTeamTechs->HasTech(eTech) || pPlayerTechs->IsResearchingTech(eTech)) && !bShift)
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
	GET_PLAYER(ePlayer).DoCivilianReturnLogic(bReturn, eToPlayer, iUnitID);
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSellBuilding(PlayerTypes ePlayer, int iCityID, BuildingTypes eBuilding)
{
	CvCity* pCity = GET_PLAYER(ePlayer).getCity(iCityID);
	if (pCity)
	{
		pCity->GetCityBuildings()->DoSellBuilding(eBuilding);

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
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSetCityAIFocus(PlayerTypes ePlayer, int iCityID, CityAIFocusTypes eFocus)
{
	CvCity* pCity = GET_PLAYER(ePlayer).getCity(iCityID);
	if (pCity != NULL)
	{
		CvCityCitizens* pkCitizens = pCity->GetCityCitizens();
		if(pkCitizens != NULL)
		{
			pkCitizens->SetFocusType(eFocus);
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSetCityAvoidGrowth(PlayerTypes ePlayer, int iCityID, bool bAvoidGrowth)
{
	CvCity* pCity = GET_PLAYER(ePlayer).getCity(iCityID);
	if (pCity != NULL)
	{
		CvCityCitizens* pkCitizens = pCity->GetCityCitizens();
		if(pkCitizens != NULL)
		{
			pkCitizens->SetForcedAvoidGrowth(bAvoidGrowth);
		}
	}
}
//------------------------------------------------------------------------------
void CvDllNetMessageHandler::ResponseSwapUnits(PlayerTypes ePlayer, int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift)
{
	CvUnit::dispatchingNetMessage(true);

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvUnit* pkUnit = kPlayer.getUnit(iUnitID);

	if (pkUnit != NULL)
	{
		// Get target plot
		CvMap& kMap = GC.getMap();
		CvPlot* pkTargetPlot = kMap.plot(iData1, iData2);

		if (pkTargetPlot != NULL)
		{
			CvPlot* pkOriginationPlot = pkUnit->plot();

			// Find unit to move out
			for (int iI = 0; iI < pkTargetPlot->getNumUnits(); iI++)
			{
				CvUnit* pkUnit2 = pkTargetPlot->getUnitByIndex(iI);

				if (pkUnit2 && pkUnit2->AreUnitsOfSameType(*pkUnit))
				{
					// Start the swap
					pkUnit->PushMission(CvTypes::getMISSION_MOVE_TO(), iData1, iData2, MOVE_IGNORE_STACKING, bShift, true);

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
	CvCity* pCity = GET_PLAYER(ePlayer).getCity(iCityID);
	if (NULL != pCity && pCity->GetCityCitizens())
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
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	// Policy Update
	if (bNOTPolicyBranch)
	{
		const PolicyTypes ePolicy = static_cast<PolicyTypes>(iPolicyID);
		if (bValue)
		{
			kPlayer.doAdoptPolicy(ePolicy);
		}
		else
		{
			kPlayer.setHasPolicy(ePolicy, bValue);
			kPlayer.DoUpdateHappiness();
		}
	}
	// Policy Branch Update
	else
	{
		const PolicyBranchTypes eBranch = static_cast<PolicyBranchTypes>(iPolicyID);
		CvPlayerPolicies* pPlayerPolicies = kPlayer.GetPlayerPolicies();

		// If Branch was blocked by another branch, then unblock this one - this may be the only thing this NetMessage does
		if (pPlayerPolicies->IsPolicyBranchBlocked(eBranch))
		{
			// Can't switch to a Branch that's still locked. DoUnlockPolicyBranch below will handle this for us
			if (pPlayerPolicies->IsPolicyBranchUnlocked(eBranch))
			{
				//pPlayerPolicies->ChangePolicyBranchBlockedCount(eBranch, -1);
				pPlayerPolicies->DoSwitchToPolicyBranch(eBranch);
			}
		}

		// Unlock the branch if it hasn't been already
		if (!pPlayerPolicies->IsPolicyBranchUnlocked(eBranch))
		{
			pPlayerPolicies->DoUnlockPolicyBranch(eBranch);
		}
	}
}
//------------------------------------------------------------------------------
