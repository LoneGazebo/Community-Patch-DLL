/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvPlayerAI.h"
#include "CvRandom.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvMap.h"
#include "CvArea.h"
#include "CvPlot.h"
#include "CvTeam.h"
#include "CvGameCoreUtils.h"
#include "ICvDLLUserInterface.h"
#include "CvInfos.h"
#include "CvAStar.h"
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"
#include "CvTechAI.h"
#include "CvDangerPlots.h"
#include "CvImprovementClasses.h"
#include "CvTacticalAnalysisMap.h"
#include "CvMilitaryAI.h"
#include "CvWonderProductionAI.h"
#include "CvCitySpecializationAI.h"
#include "cvStopWatch.h"


// Include this after all other headers.
#include "LintFree.h"

#define DANGER_RANGE				(6)

// statics

CvPlayerAI* CvPlayerAI::m_aPlayers = NULL;

void CvPlayerAI::initStatics()
{
	m_aPlayers = FNEW(CvPlayerAI[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aPlayers[iI].m_eID = ((PlayerTypes)iI);
	}
}

void CvPlayerAI::freeStatics()
{
	SAFE_DELETE_ARRAY(m_aPlayers);
}

// Public Functions...
CvPlayerAI::CvPlayerAI()
{
	AI_reset();
}


CvPlayerAI::~CvPlayerAI()
{
	AI_uninit();
}


void CvPlayerAI::AI_init()
{
	AI_reset();
}


void CvPlayerAI::AI_uninit()
{
}


void CvPlayerAI::AI_reset()
{
	AI_uninit();
}

void CvPlayerAI::AI_doTurnPre()
{
	CvAssertMsg(getPersonalityType() != NO_LEADER, "getPersonalityType() is not expected to be equal with NO_LEADER");
	CvAssertMsg(getLeaderType() != NO_LEADER, "getLeaderType() is not expected to be equal with NO_LEADER");
	CvAssertMsg(getCivilizationType() != NO_CIVILIZATION, "getCivilizationType() is not expected to be equal with NO_CIVILIZATION");

	if (isHuman())
	{
		return;
	}

	AI_updateFoundValues();

	AI_doResearch();

	GetPlayerPolicies()->DoPolicyAI();

	if (isBarbarian())
	{
		return;
	}

	if (isMinorCiv())
	{
		return;
	}
}


void CvPlayerAI::AI_doTurnPost()
{
	if (isHuman())
	{
		return;
	}

	if (isBarbarian())
	{
		return;
	}

	if (isMinorCiv())
	{
		return;
	}

	for (int i = 0; i < GC.getNumVictoryInfos(); ++i)
	{
		AI_launch((VictoryTypes)i);
	}

	ProcessGreatPeople();
}


void CvPlayerAI::AI_doTurnUnitsPre()
{
	GetTacticalAI()->InitializeQueuedAttacks();

	if (isHuman())
	{
		return;
	}

	if (isBarbarian())
	{
		return;
	}
}


void CvPlayerAI::AI_doTurnUnitsPost()
{
	CvUnit* pLoopUnit;
	int iLoop;

	if (!isHuman())
	{
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			pLoopUnit->AI_promote();
		}
	}
}

void CvPlayerAI::AI_updateFoundValues(bool bStartingLoc)
{
	int iLoop;
	const int iNumPlots = GC.getMap().numPlots();
	for(CvArea *pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		pLoopArea->setTotalFoundValue(0);
	}

	const PlayerTypes eID = GetID();
	if (bStartingLoc)
	{
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			GC.getMap().plotByIndexUnchecked(iI)->setFoundValue(eID, -1);
		}
	}
	else
	{
		const TeamTypes eTeam = getTeam();
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

			if (pLoopPlot->isRevealed(eTeam))
			{
				const int iValue = AI_foundValue(pLoopPlot->getX(), pLoopPlot->getY());
				pLoopPlot->setFoundValue(eID, iValue);
				CvArea *pLoopArea = GC.getMap().getArea(pLoopPlot->getArea());
				if (pLoopArea && !pLoopArea->isWater())
				{
					pLoopArea->setTotalFoundValue(pLoopArea->getTotalFoundValue() + iValue);
				}
			}
			else
			{
				pLoopPlot->setFoundValue(eID, -1);
			}
		}
	}
}

//	---------------------------------------------------------------------------
void CvPlayerAI::AI_unitUpdate()
{
	GC.getPathFinder().ForceReset();
	GC.getIgnoreUnitsPathFinder().ForceReset();
	GC.getRouteFinder().ForceReset();
	GC.GetWaterRouteFinder().ForceReset();

	// Set individual pathers as MP cache safe.  A global for all pathers might be simpler,
	// but this will allow selective control in case one type of pather is causing out-of-syncs.
	bool bCommonPathFinderMPCaching = GC.getPathFinder().SetMPCacheSafe(true);
	bool bIgnoreUnitsPathFinderMPCaching = GC.getIgnoreUnitsPathFinder().SetMPCacheSafe(true);
	bool bTacticalPathFinderMPCaching = GC.GetTacticalAnalysisMapFinder().SetMPCacheSafe(true);
	bool bInfluencePathFinderMPCaching = GC.getInfluenceFinder().SetMPCacheSafe(true);
	bool bRoutePathFinderMPCaching = GC.getRouteFinder().SetMPCacheSafe(true);
	bool bWaterRoutePathFinderMPCaching = GC.GetWaterRouteFinder().SetMPCacheSafe(true);

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "PlayerPreAIUnitUpdate", args.get(), bResult);
	}

	//GC.getGame().GetTacticalAnalysisMap()->RefreshDataForNextPlayer(this);

	// this was a !hasBusyUnit around the entire rest of the function, so I tried to make it a bit flatter.
	if (hasBusyUnitOrCity())
	{
		return;
	}

	if (isHuman())
	{
		CvUnit::dispatchingNetMessage(true);
		// The homeland AI goes first.
		GetHomelandAI()->FindAutomatedUnits();
		GetHomelandAI()->Update();
		CvUnit::dispatchingNetMessage(false);
	}
	else
	{
		// Update tactical AI
		GetTacticalAI()->CommandeerUnits();

		// Now let the tactical AI run.  Putting it after the operations update allows units who have
		// just been handed off to the tactical AI to get a move in the same turn they switch between
		// AI subsystems
		GetTacticalAI()->Update();

		// Skip homeland AI processing if a barbarian
		if (m_eID != BARBARIAN_PLAYER)
		{
			// Now its the homeland AI's turn.
			GetHomelandAI()->RecruitUnits();
			GetHomelandAI()->Update();
		}
	}

	GC.getPathFinder().SetMPCacheSafe(bCommonPathFinderMPCaching);
	GC.getIgnoreUnitsPathFinder().SetMPCacheSafe(bIgnoreUnitsPathFinderMPCaching);
	GC.GetTacticalAnalysisMapFinder().SetMPCacheSafe(bTacticalPathFinderMPCaching);
	GC.getInfluenceFinder().SetMPCacheSafe(bInfluencePathFinderMPCaching);
	GC.getRouteFinder().SetMPCacheSafe(bRoutePathFinderMPCaching);
	GC.GetWaterRouteFinder().SetMPCacheSafe(bWaterRoutePathFinderMPCaching);
}


void CvPlayerAI::AI_conquerCity(CvCity* pCity, PlayerTypes eOldOwner)
{
	// Liberate a Minor's City?
	if (pCity->getOriginalOwner() != eOldOwner && pCity->getOriginalOwner() != GetID() && CanLiberatePlayer(eOldOwner))
	{
		if (GET_PLAYER(pCity->getOriginalOwner()).isMinorCiv())
		{
			if (GetDiplomacyAI()->DoPossibleMinorLiberation(pCity->getOriginalOwner(), pCity->GetID()))
				return;
		}
	}

	// Do we want to burn this city down?
	if (canRaze(pCity))
	{
		// Burn the city if the empire is unhappy - keeping the city will only make things worse or if map hint dictates
		if (IsEmpireUnhappy() || (GC.getMap().GetAIMapHint() & 2))
		{
			pCity->doTask(TASK_RAZE);
			return;
		}
	}

	// Puppet the city
	if (pCity->getOriginalOwner() != GetID())
	{
		pCity->DoCreatePuppet();
	}
}

bool CvPlayerAI::AI_captureUnit(UnitTypes, CvPlot* pPlot)
{
	CvCity* pNearestCity;

	CvAssert(!isHuman());

	// Barbs always capture
	if (isBarbarian())
		return true;

	if (pPlot->getTeam() == getTeam())
		return true;

	pNearestCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), NO_PLAYER, getTeam());

	if (pNearestCity != NULL)
	{
		if (plotDistance(pPlot->getX(), pPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) <= 4)
			return true;
	}

	return false;
}

int CvPlayerAI::AI_foundValue(int iX, int iY, int, bool bStartingLoc)
{

	CvPlot *pPlot;
	int rtnValue = 0;

	pPlot = GC.getMap().plot(iX, iY);

	if (bStartingLoc)
	{
		rtnValue =  GC.getGame().GetStartSiteEvaluator()->PlotFoundValue(pPlot, this);
	}
	else
	{
		GC.getGame().GetSettlerSiteEvaluator()->ComputeFlavorMultipliers(this);
		rtnValue =  GC.getGame().GetSettlerSiteEvaluator()->PlotFoundValue(pPlot, this, NO_YIELD, false);
	}

	return rtnValue;
}

void CvPlayerAI::AI_chooseFreeGreatPerson()
{
	while (GetNumFreeGreatPeople() > 0)
	{
		UnitTypes eDesiredGreatPerson = NO_UNIT;

		// Highly wonder competitive and still early in game?
		if (GetDiplomacyAI()->GetWonderCompetitiveness() >= 8 && GC.getGame().getGameTurn() <= (GC.getGame().getEstimateEndTurn() / 2))
		{
			eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ENGINEER");
		}
		else
		{
			// Pick the person based on our victory method
			AIGrandStrategyTypes eVictoryStrategy = GetGrandStrategyAI()->GetActiveGrandStrategy();
			if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
			{
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");
			}
			else if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
			{
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST");
			}
			else if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
			{
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_MERCHANT");
			}
			else if (eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
			{
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST");
			}
		}

		if (eDesiredGreatPerson != NO_UNIT)
		{
			addFreeUnit(eDesiredGreatPerson);
			ChangeNumFreeGreatPeople(-1);
		}
		else
		{
			break;
		}
	}
}

void CvPlayerAI::AI_chooseFreeTech()
{
	TechTypes eBestTech = NO_TECH;

	clearResearchQueue();

	// TODO: script override

	if (eBestTech == NO_TECH)
	{
		eBestTech = GetPlayerTechs()->GetTechAI()->ChooseNextTech();
	}

	if (eBestTech != NO_TECH)
	{
		GET_TEAM(getTeam()).setHasTech(eBestTech, true, GetID(), true, true);
	}
}


void CvPlayerAI::AI_chooseResearch()
{
	AI_PERF("AI-perf.csv", "AI_chooseResearch");
	TechTypes eBestTech = NO_TECH;
	int iI;

	clearResearchQueue();

	if (GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if ((iI != GetID()) && (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam()))
				{
					if (GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->GetCurrentResearch() != NO_TECH)
					{
						if (GetPlayerTechs()->CanResearch(GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->GetCurrentResearch()))
						{
							pushResearch(GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->GetCurrentResearch());
						}
					}
				}
			}
		}
	}

	if (GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{
		//todo: script override

		if (eBestTech == NO_TECH)
		{
			eBestTech = GetPlayerTechs()->GetTechAI()->ChooseNextTech();
		}

		if (eBestTech != NO_TECH)
		{
			pushResearch(eBestTech);
		}
	}
}

int CvPlayerAI::AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, int iRange)
{
	int iCount = 0;

	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		CvPlot* pMissionPlot = pLoopUnit->GetMissionAIPlot();
		if (!pMissionPlot)
		{
			continue;
		}

		MissionAITypes eGroupMissionAI = pLoopUnit->GetMissionAIType();
		if (eGroupMissionAI != eMissionAI)
		{
			continue;
		}

		int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pMissionPlot->getX(), pMissionPlot->getY());
		if (iDistance == iRange)
		{
			iCount++;
		}
	}

	return iCount;
}

// Protected Functions...

void CvPlayerAI::AI_doResearch()
{
	CvAssertMsg(!isHuman(), "isHuman did not return false as expected");

	if (GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{
		AI_chooseResearch();
		//AI_forceUpdateStrategies(); //to account for current research.
	}
}


//
// read object from a stream
// used during load
//
void CvPlayerAI::Read(FDataStream& kStream)
{
	CvPlayer::Read(kStream);	// read base class data first

	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
}


//
// save object to a stream
// used during save
//
void CvPlayerAI::Write(FDataStream& kStream) const
{
	CvPlayer::Write(kStream);	// write base class data first

	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
}

void CvPlayerAI::AI_launch(VictoryTypes eVictory)
{
	if (GET_TEAM(getTeam()).isHuman())
	{
		return;
	}

	if (!GET_TEAM(getTeam()).canLaunch(eVictory))
	{
		return;
	}

	launch(eVictory);
}

OperationSlot CvPlayerAI::PeekAtNextUnitToBuildForOperationSlot(int iAreaID)
{
	OperationSlot thisSlot;

	// search through our operations till we find one that needs a unit
	std::map<int, CvAIOperation*>::iterator iter;
	for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if (pThisOperation)
		{
			thisSlot = pThisOperation->PeekAtNextUnitToBuild(iAreaID);
			if (thisSlot.IsValid())
			{
				break;
			}
		}
	}

	return thisSlot;
}


OperationSlot CvPlayerAI::CityCommitToBuildUnitForOperationSlot(int iAreaID, int iTurns, CvCity *pCity)
{
	OperationSlot thisSlot;

	// search through our operations till we find one that needs a unit
	std::map<int, CvAIOperation*>::iterator iter;
	for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if (pThisOperation)
		{
			thisSlot = pThisOperation->CommitToBuildNextUnit(iAreaID, iTurns, pCity);
			if (thisSlot.IsValid())
			{
				break;
			}
		}
	}

	return thisSlot;
}

void CvPlayerAI::CityUncommitToBuildUnitForOperationSlot(OperationSlot thisSlot)
{
	// find this operation
	CvAIOperation* pThisOperation = getAIOperation(thisSlot.m_iOperationID);
	if (pThisOperation)
	{
		pThisOperation->UncommitToBuild(thisSlot);
	}
}

void CvPlayerAI::CityFinishedBuildingUnitForOperationSlot(OperationSlot thisSlot, CvUnit* pThisUnit)
{
	// find this operation
	CvAIOperation* pThisOperation = getAIOperation(thisSlot.m_iOperationID);
	CvArmyAI* pThisArmy = getArmyAI(thisSlot.m_iArmyID);
	if (pThisOperation && pThisArmy && pThisUnit)
	{
		pThisArmy->AddUnit(pThisUnit->GetID(), thisSlot.m_iSlotID);
		pThisOperation->FinishedBuilding(thisSlot);
	}
}

int CvPlayerAI::GetNumUnitsNeededToBeBuilt()
{
	int iRtnValue = 0;

	std::map<int, CvAIOperation*>::iterator iter;
	for (iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if (pThisOperation)
		{
			iRtnValue += pThisOperation->GetNumUnitsNeededToBeBuilt();
		}
	}

	return iRtnValue;
}

void CvPlayerAI::ProcessGreatPeople (void)
{
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	CvAssert(isAlive());

	if (!isAlive())
		return;

	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->getSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
		switch (pLoopUnit->AI_getUnitAIType())
		{
		case UNITAI_ARTIST:
			eDirective = GetDirectiveArtist(pLoopUnit);
			break;
		case UNITAI_ENGINEER:
			eDirective = GetDirectiveEngineer(pLoopUnit);
			break;
		case UNITAI_MERCHANT:
			eDirective = GetDirectiveMerchant(pLoopUnit);
			break;
		case UNITAI_SCIENTIST:
			eDirective = GetDirectiveScientist(pLoopUnit);
			break;
		case UNITAI_GENERAL:
			eDirective = GetDirectiveGeneral(pLoopUnit);
			break;
		}

		pLoopUnit->SetGreatPeopleDirective(eDirective);
	}
}

bool PreparingForWar (CvPlayerAI* pPlayer)
{
	CvAssertMsg(pPlayer, "Need a player");
	if (!pPlayer)
	{
		return false;
	}
	CvMilitaryAI* pMilitaryAI = pPlayer->GetMilitaryAI();
	CvAssertMsg(pMilitaryAI, "No military AI");
	if (!pMilitaryAI)
	{
		return false;
	}

	MilitaryAIStrategyTypes eWarMobilizationStrategy = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
	if (pMilitaryAI->IsUsingStrategy(eWarMobilizationStrategy))
	{
		return true;
	}

	return false;
}

GreatPeopleDirectiveTypes GetUnresolvedAction (CvPlayerAI*, CvUnit*)
{
	int iRand = GC.getGame().getJonRandNum(2, "Randomly picking a directive for a great person to do");
	if (iRand == 0)
	{
		return GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}
	else
	{
		return GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}
}

bool IsSafe (CvPlayerAI* pPlayer)
{
	CvAssertMsg(pPlayer, "Need a player");
	if (!pPlayer)
	{
		return false;
	}

	if (pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		return true;
	}
	else
	{
		CvMilitaryAI* pMilitaryAI = pPlayer->GetMilitaryAI();
		CvAssertMsg(pMilitaryAI, "No military AI");
		if (!pMilitaryAI)
		{
			return false;
		}

		MilitaryAIStrategyTypes eAtWarStrategy = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
		if (!pMilitaryAI->IsUsingStrategy(eAtWarStrategy))
		{
			return true;
		}

		return false;
	}
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveArtist (CvUnit* pGreatArtist)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && PreparingForWar(this) && !isGoldenAge())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 2) / 4))
	{
		if (GetDiplomacyAI()->IsGoingForCultureVictory())
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}

	// gank territory from neighbor we don't like
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && IsSafe(this))
	{
		int iScore = 0;
		CvPlot* pTargetPlot = FindBestArtistTargetPlot(pGreatArtist, iScore);
		if (pTargetPlot)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatArtist->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
	{
		eDirective = GetUnresolvedAction(this, pGreatArtist);
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveEngineer (CvUnit* pGreatEngineer)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !isGoldenAge() && PreparingForWar(this))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	//if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 3) / 4))
	//{
	//	if (GetDiplomacyAI()->IsGoingForWorldConquest())
	//	{
	//		eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	//	}
	//}

	// look for a wonder to rush
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		int iNextWonderWeight;
		BuildingTypes eNextWonderDesired = GetWonderProductionAI()->ChooseWonder(false /*bUseAsyncRandom*/, false /*bAdjustForOtherPlayers*/, iNextWonderWeight);
		if (eNextWonderDesired != NO_BUILDING)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
	{
		eDirective = GetUnresolvedAction(this, pGreatEngineer);
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveMerchant (CvUnit* pGreatMerchant)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	// if the merchant is in an army, he's already marching to a destination, so don't evaluate him
	if (pGreatMerchant->getArmyID() != FFreeList::INVALID_INDEX)
	{
		return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && PreparingForWar(this) && !isGoldenAge())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	// Attempt a run to a minor civ
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && IsSafe(this))
	{
		CvPlot* pTarget = FindBestMerchantTargetPlot (pGreatMerchant, true);
		if (pTarget)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatMerchant->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
	{
		eDirective = GetUnresolvedAction(this, pGreatMerchant);
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveScientist (CvUnit* pGreatScientist)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && PreparingForWar(this) && !isGoldenAge())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	// If I'm in danger, use great person to get a tech
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !IsSafe(this))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 1) / 4))
	{
		if (GetDiplomacyAI()->IsGoingForSpaceshipVictory())
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatScientist->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
	{
		// a free tech is not bad
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveGeneral (CvUnit*)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	int iGreatGeneralCount = 0;

	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->getSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}

		if (pLoopUnit->AI_getUnitAIType() == UNITAI_GENERAL && pLoopUnit->GetGreatPeopleDirective() != GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			iGreatGeneralCount++;
		}
	}

	if (iGreatGeneralCount > 2)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	return eDirective;
}

CvPlot* CvPlayerAI::FindBestMerchantTargetPlot (CvUnit* pGreatMerchant, bool bOnlySafePaths)
{
	CvAssertMsg(pGreatMerchant, "pGreatMerchant is null");
	if (!pGreatMerchant)
	{
		return NULL;
	}

	int iBestTurnsToReach = MAX_INT;
	CvPlot* pBestTargetPlot = NULL;
	int iPathTurns;
	UnitHandle pMerchant = UnitHandle(pGreatMerchant);
	CvTeam &kTeam = GET_TEAM(getTeam());

	// Loop through each city state
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer &kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isMinorCiv())
		{
			CvPlot *pCSPlot = kPlayer.getStartingPlot();
			if (pCSPlot)
			{
				if (pCSPlot->isRevealed(getTeam()))
				{
					// Is this a minor we are friendly with?
					if (GetDiplomacyAI()->GetMinorCivApproach(kPlayer.GetID()) != MINOR_CIV_APPROACH_CONQUEST &&
						!kTeam.isAtWar(kPlayer.getTeam()) && GetDiplomacyAI()->GetWarGoal(kPlayer.GetID()) == NO_WAR_GOAL_TYPE)
					{
						// Search all the plots adjacent to this city (since can't enter the minor city plot itself)
						for (int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
						{
							CvPlot *pAdjacentPlot = plotDirection(pCSPlot->getX(), pCSPlot->getY(), ((DirectionTypes)jJ));
							if (pAdjacentPlot != NULL)
							{
								// Make sure this is still owned by the city state and is revealed to us and isn't a water tile
								if (pAdjacentPlot->getOwner() == (PlayerTypes)iI && pAdjacentPlot->isRevealed(getTeam())
									&& !pAdjacentPlot->isWater())
								{
									iPathTurns = TurnsToReachTarget(pMerchant, pAdjacentPlot, true /*bReusePaths*/, !bOnlySafePaths/*bIgnoreUnits*/);
									if (iPathTurns < iBestTurnsToReach)
									{
										iBestTurnsToReach = iPathTurns;
										pBestTargetPlot = pAdjacentPlot;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestTargetPlot;
}

CvPlot* CvPlayerAI::FindBestArtistTargetPlot (CvUnit* pGreatArtist, int& iResultScore)
{
	CvAssertMsg(pGreatArtist, "pGreatArtist is null");
	if (!pGreatArtist)
	{
		return NULL;
	}

	iResultScore = 0;

	CvPlotsVector& m_aiPlots = GetPlots();

	CvPlot* pBestPlot = NULL;
	int iBestScore = 0;

	// loop through plots and wipe out ones that are invalid
	const uint nPlots = m_aiPlots.size();
	for (uint ui = 0; ui < nPlots; ui++)
	{
		if (m_aiPlots[ui] == -1)
		{
			continue;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(m_aiPlots[ui]);

		if (pPlot->isWater())
		{
			continue;
		}

		if (!pPlot->IsAdjacentOwnedByOtherTeam(getTeam()))
		{
			continue;
		}

		int iScore = 0;

		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			// if there's no plot, bail
			if (pAdjacentPlot == NULL)
			{
				continue;
			}

			// if the plot is ours or no one's, bail
			if (pAdjacentPlot->getTeam() == NO_TEAM || pAdjacentPlot->getTeam() == getTeam())
			{
				continue;
			}

			// don't evaluate city plots since we don't get ownership of them with the bomb
			if (pAdjacentPlot->getPlotCity())
			{
				continue;
			}

			const PlayerTypes eOtherPlayer = pAdjacentPlot->getOwner();
			if (GET_PLAYER(eOtherPlayer).isMinorCiv())
			{
				MinorCivApproachTypes eMinorApproach = GetDiplomacyAI()->GetMinorCivApproach(eOtherPlayer);
				// if we're friendly or protective, don't be a jerk. Bail out.
				if (eMinorApproach != MINOR_CIV_APPROACH_CONQUEST && eMinorApproach != MINOR_CIV_APPROACH_IGNORE)
				{
					iScore = 0;
					break;
				}
			}
			else
			{
				MajorCivApproachTypes eMajorApproach = GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, true);
				DisputeLevelTypes eLandDisputeLevel = GetDiplomacyAI()->GetLandDisputeLevel(eOtherPlayer);

				bool bTicked = eMajorApproach == MAJOR_CIV_APPROACH_HOSTILE;
				bool bTickedAboutLand = eMajorApproach == MAJOR_CIV_APPROACH_NEUTRAL && (eLandDisputeLevel == DISPUTE_LEVEL_STRONG || eLandDisputeLevel == DISPUTE_LEVEL_FIERCE);

				// only bomb if we're hostile
				if (!bTicked && !bTickedAboutLand)
				{
					iScore = 0;
					break;
				}
			}

			ResourceTypes eResource = pAdjacentPlot->getResourceType();
			if (eResource != NO_RESOURCE)
			{
				iScore += GetBuilderTaskingAI()->GetResourceWeight(eResource, NO_IMPROVEMENT, pAdjacentPlot->getNumResource()) * 10;
			}
			else
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
				{
					iScore += pAdjacentPlot->getYield((YieldTypes)iYield);
				}
			}
		}

		if (iScore > iBestScore)
		{
			iBestScore = iScore;
			pBestPlot = pPlot;
		}
	}

	iResultScore = iBestScore;
	return pBestPlot;
}

