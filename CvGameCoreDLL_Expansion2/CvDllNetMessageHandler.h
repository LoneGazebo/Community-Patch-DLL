/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "CvDllInterfaces.h"

class CvDllNetMessageHandler : public ICvNetMessageHandler3
{
public:
	CvDllNetMessageHandler();
	virtual ~CvDllNetMessageHandler();

	void* DLLCALL QueryInterface(GUID guidInterface);

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	void DLLCALL ResponseAdvancedStartAction(PlayerTypes ePlayer, AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd);
	void DLLCALL ResponseAutoMission(PlayerTypes ePlayer, int iUnitID);
	void DLLCALL ResponseBarbarianRansom(PlayerTypes ePlayer, int iOptionChosen, int iUnitID);
	void DLLCALL ResponseChangeWar(PlayerTypes ePlayer, TeamTypes eRivalTeam, bool bWar);
	void DLLCALL ResponseIgnoreWarning(PlayerTypes ePlayer, TeamTypes eRivalTeam);
	void DLLCALL ResponseCityBuyPlot(PlayerTypes ePlayer, int iCityID, int iX, int iY);
	void DLLCALL ResponseCityDoTask(PlayerTypes ePlayer, int iCityID, TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl);
	void DLLCALL ResponseCityPopOrder(PlayerTypes ePlayer, int iCityID, int iNum);
	void DLLCALL ResponseCityPurchase(PlayerTypes ePlayer, int iCityID, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType);
	void DLLCALL ResponseCityPurchase(PlayerTypes ePlayer, int iCityID, UnitTypes eUnitType, BuildingTypes eBuildingType, ProjectTypes eProjectType, int ePurchaseYield);
	void DLLCALL ResponseCityPushOrder(PlayerTypes ePlayer, int iCityID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl);
	void DLLCALL ResponseCitySwapOrder(PlayerTypes ePlayer, int iCityID, int iNum);
	void DLLCALL ResponseChooseElection(PlayerTypes ePlayer, int iSelection, int iVoteId);
	void DLLCALL ResponseDestroyUnit(PlayerTypes ePlayer, int iUnitID);
	void DLLCALL ResponseDiplomacyFromUI(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2);
	void DLLCALL ResponseDiploVote(PlayerTypes ePlayer, PlayerTypes eVotePlayer);
	void DLLCALL ResponseDoCommand(PlayerTypes ePlayer, int iUnitID, CommandTypes eCommand, int iData1, int iData2, bool bAlt);
	void DLLCALL ResponseExtendedGame(PlayerTypes ePlayer);
	void DLLCALL ResponseFoundPantheon(PlayerTypes ePlayer, BeliefTypes eBelief);
	void DLLCALL ResponseFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, BeliefTypes eBelief3, BeliefTypes eBelief4, int iCityX, int iCityY);
	void DLLCALL ResponseEnhanceReligion(PlayerTypes ePlayer, ReligionTypes eReligion, const char* szCustomName, BeliefTypes eBelief1, BeliefTypes eBelief2, int iCityX, int iCityY);
	void DLLCALL ResponseMoveSpy(PlayerTypes ePlayer, int iSpyIndex, int iTargetPlayer, int iTargetCity, bool bAsDiplomat);
	void DLLCALL ResponseStageCoup(PlayerTypes eSpyPlayer, int iSpyIndex);
	void DLLCALL ResponseFaithPurchase(PlayerTypes ePlayer, FaithPurchaseTypes eFaithPurchaseType, int iFaithPurchaseIndex);
	void DLLCALL ResponseLeagueVoteEnact(LeagueTypes eLeague, int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice);
	void DLLCALL ResponseLeagueVoteRepeal(LeagueTypes eLeague, int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice);
	void DLLCALL ResponseLeagueVoteAbstain(LeagueTypes eLeague, PlayerTypes eVoter, int iNumVotes);
	void DLLCALL ResponseLeagueProposeEnact(LeagueTypes eLeague, ResolutionTypes eResolution, PlayerTypes eProposer, int iChoice);
	void DLLCALL ResponseLeagueProposeRepeal(LeagueTypes eLeague, int iResolutionID, PlayerTypes eProposer);
	void DLLCALL ResponseLeagueEditName(LeagueTypes eLeague, PlayerTypes ePlayer, const char* szCustomName);
	void DLLCALL ResponseSetSwappableGreatWork(PlayerTypes ePlayer, int iWorkClass, int iWorkIndex);
	void DLLCALL ResponseSwapGreatWorks(PlayerTypes ePlayer1, int iWorkIndex1, PlayerTypes ePlayer2, int iWorkIndex2);
	void DLLCALL ResponseMoveGreatWorks(PlayerTypes ePlayer, int iCity1, int iBuildingClass1, int iWorkIndex1, int iCity2, int iBuildingClass2, int iWorkIndex2);
	void DLLCALL ResponseChangeIdeology(PlayerTypes ePlayer);
	void DLLCALL ResponseGiftUnit(PlayerTypes ePlayer, PlayerTypes eMinor, int iUnitID);
	void DLLCALL ResponseGreatPersonChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit);
	void DLLCALL ResponseMayaBonusChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit);
	void DLLCALL ResponseFaithGreatPersonChoice(PlayerTypes ePlayer, UnitTypes eGreatPersonUnit);
	void DLLCALL ResponseGoodyChoice(PlayerTypes ePlayer, int iPlotX, int iPlotY, GoodyTypes eGoody, int iUnitID);
	void DLLCALL ResponseArchaeologyChoice(PlayerTypes ePlayer, ArchaeologyChoiceType eChoice);
	void DLLCALL ResponseIdeologyChoice(PlayerTypes ePlayer, PolicyBranchTypes eChoice);
	void DLLCALL ResponseLaunchSpaceship(PlayerTypes ePlayer, VictoryTypes eVictory);
	void DLLCALL ResponseLiberatePlayer(PlayerTypes ePlayer, PlayerTypes eLiberatedPlayer, int iCityID);
	void DLLCALL ResponseMinorCivBullyGold(PlayerTypes ePlayer, PlayerTypes eMinor, int iGold);
	void DLLCALL ResponseMinorCivBullyUnit(PlayerTypes ePlayer, PlayerTypes eMinor, UnitTypes eUnitType);
	void DLLCALL ResponseMinorCivGiftGold(PlayerTypes ePlayer, PlayerTypes eMinor, int iGold);
	void DLLCALL ResponseMinorCivGiftTileImprovement(PlayerTypes eMajor, PlayerTypes eMinor, int iPlotX, int iPlotY);
	void DLLCALL ResponseMinorCivBuyout(PlayerTypes eMajor, PlayerTypes eMinor);
	void DLLCALL ResponseMinorNoUnitSpawning(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue);
	void DLLCALL ResponsePlayerDealFinalized(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, PlayerTypes eActBy, bool bAccepted);
	void DLLCALL ResponsePlayerOption(PlayerTypes ePlayer, PlayerOptionTypes eOption, bool bValue);
	void DLLCALL ResponsePledgeMinorProtection(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue, bool bPledgeNowBroken);
	void DLLCALL ResponsePushMission(PlayerTypes ePlayer, int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift);
	void DLLCALL ResponseRenameCity(PlayerTypes ePlayer, int iCityID, _In_z_ const char* szName);
	void DLLCALL ResponseRenameUnit(PlayerTypes ePlayer, int iUnitID, _In_z_ const char* szName);
	void DLLCALL ResponseResearch(PlayerTypes ePlayer, TechTypes eTech, int iDiscover, bool bShift);
	void DLLCALL ResponseResearch(PlayerTypes ePlayer, TechTypes eTech, int iDiscover, PlayerTypes ePlayerToStealFrom, bool bShift);
	void DLLCALL ResponseReturnCivilian(PlayerTypes ePlayer, PlayerTypes eToPlayer, int iUnitID, bool bReturn);
	void DLLCALL ResponseSellBuilding(PlayerTypes ePlayer, int iCityID, BuildingTypes eBuilding);
	void DLLCALL ResponseSetCityAIFocus(PlayerTypes ePlayer, int iCityID, CityAIFocusTypes eFocus);
	void DLLCALL ResponseSetCityAvoidGrowth(PlayerTypes ePlayer, int iCityID, bool bAvoidGrowth);
	void DLLCALL ResponseSwapUnits(PlayerTypes ePlayer, int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift);
	void DLLCALL ResponseUpdateCityCitizens(PlayerTypes ePlayer, int iCityID);
	void DLLCALL ResponseUpdatePolicies(PlayerTypes ePlayer, bool bNOTPolicyBranch, int iPolicyID, bool bValue);

private:
	void DLLCALL Destroy();
};