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
#include "CvEconomicAI.h"

// Include this after all other headers.
#include "LintFree.h"

#define DANGER_RANGE				(6)

// statics

CvPlayerAI* CvPlayerAI::m_aPlayers = NULL;

void CvPlayerAI::initStatics()
{
	m_aPlayers = FNEW(CvPlayerAI[MAX_PLAYERS], c_eCiv5GameplayDLL, 0);
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
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
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayerAI::AI_doTurnPre, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
	CvAssertMsg(getPersonalityType() != NO_LEADER, "getPersonalityType() is not expected to be equal with NO_LEADER");
	CvAssertMsg(getLeaderType() != NO_LEADER, "getLeaderType() is not expected to be equal with NO_LEADER");
	CvAssertMsg(getCivilizationType() != NO_CIVILIZATION, "getCivilizationType() is not expected to be equal with NO_CIVILIZATION");

	if(isHuman())
	{
		return;
	}

	AI_updateFoundValues();

	AI_doResearch();
	AI_considerAnnex();
}


void CvPlayerAI::AI_doTurnPost()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayerAI::AI_doTurnPost, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), getCivilizationShortDescription()) );
	if(isHuman())
	{
		return;
	}

	if(isBarbarian())
	{
		return;
	}

	if(isMinorCiv())
	{
		return;
	}

	for(int i = 0; i < GC.getNumVictoryInfos(); ++i)
	{
		AI_launch((VictoryTypes)i);
	}

	ProcessGreatPeople();
	GetEspionageAI()->DoTurn();
	GetTradeAI()->DoTurn();
}


void CvPlayerAI::AI_doTurnUnitsPre()
{
	GetTacticalAI()->InitializeQueuedAttacks();

	if(isHuman())
	{
		return;
	}

	if(isBarbarian())
	{
		return;
	}
}


void CvPlayerAI::AI_doTurnUnitsPost()
{
	CvUnit* pLoopUnit;
	int iLoop;

	if(!isHuman())
	{
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			pLoopUnit->AI_promote();
		}
	}
}

void CvPlayerAI::AI_updateFoundValues(bool bStartingLoc)
{
	int iGoodEnoughToBeWorthOurTime = GC.getAI_STRATEGY_MINIMUM_SETTLE_FERTILITY();
	int iLoop;
	const int iNumPlots = GC.getMap().numPlots();
	for(CvArea* pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
	{
		pLoopArea->setTotalFoundValue(0);
	}

	const PlayerTypes eID = GetID();
	if(bStartingLoc)
	{
		for(int iI = 0; iI < iNumPlots; iI++)
		{
			GC.getMap().plotByIndexUnchecked(iI)->setFoundValue(eID, -1);
		}
	}
	else
	{
		const TeamTypes eTeam = getTeam();
		GC.getGame().GetSettlerSiteEvaluator()->ComputeFlavorMultipliers(this);
		for (int iI = 0; iI < iNumPlots; iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);

			if (pLoopPlot->isRevealed(eTeam))
			{
				const int iValue = GC.getGame().GetSettlerSiteEvaluator()->PlotFoundValue(pLoopPlot, this, NO_YIELD, false);
				pLoopPlot->setFoundValue(eID, iValue);
				if (iValue >= iGoodEnoughToBeWorthOurTime)
				{
					CvArea* pLoopArea = GC.getMap().getArea(pLoopPlot->getArea());
					if(pLoopArea && !pLoopArea->isWater())
					{
						pLoopArea->setTotalFoundValue(pLoopArea->getTotalFoundValue() + iValue);
					}
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
	if(hasBusyUnitOrCity())
	{
		return;
	}

	if(isHuman())
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
		if(m_eID != BARBARIAN_PLAYER)
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
	PlayerTypes eOriginalOwner = pCity->getOriginalOwner();
	TeamTypes eOldOwnerTeam = GET_PLAYER(eOldOwner).getTeam();

	// Liberate a city?
	if(eOriginalOwner != eOldOwner && eOriginalOwner != GetID() && CanLiberatePlayerCity(eOriginalOwner))
	{
		// minor civ
		if(GET_PLAYER(eOriginalOwner).isMinorCiv())
		{
			if(GetDiplomacyAI()->DoPossibleMinorLiberation(eOriginalOwner, pCity->GetID()))
				return;
		}
		else // major civ
		{
			bool bLiberate = false;
			if (GET_PLAYER(eOriginalOwner).isAlive())
			{
				// If the original owner and this player have a defensive pact
				// and both the original owner and the player are at war with the old owner of this city
				// give the city back to the original owner
				TeamTypes eOriginalOwnerTeam = GET_PLAYER(eOriginalOwner).getTeam();
				if (GET_TEAM(getTeam()).IsHasDefensivePact(eOriginalOwnerTeam) && GET_TEAM(getTeam()).isAtWar(eOldOwnerTeam) && GET_TEAM(eOriginalOwnerTeam).isAtWar(eOldOwnerTeam))
				{
					bLiberate = true;
				}
				// if the player is a friend and we're going for diplo victory, then liberate to score some friend points
				else if (GetDiplomacyAI()->IsDoFAccepted(eOriginalOwner) && GetDiplomacyAI()->IsGoingForDiploVictory())
				{
					bLiberate = true;
				}
			}
			// if the player isn't human and we're going for diplo victory, resurrect players to get super diplo bonuses
			else if (!GET_PLAYER(eOriginalOwner).isHuman() && GetDiplomacyAI()->IsGoingForDiploVictory())
			{
				bLiberate = true;
			}

			if (bLiberate)
			{
				DoLiberatePlayer(eOriginalOwner, pCity->GetID());
				return;
			}
		}
	}

	// Do we want to burn this city down?
	if(canRaze(pCity))
	{
		// Burn the city if the empire is unhappy - keeping the city will only make things worse or if map hint dictates
		// Huns will burn down everything possible once they have a core of a few cities (was 3, but this put Attila out of the running long term as a conqueror)
#if defined(MOD_GLOBAL_CS_RAZE_RARELY)
		CUSTOMLOG("AI_conquerCity: City=%s, Player=%d, ExcessHappiness=%d", pCity->getName().GetCString(), GetID(), GetExcessHappiness());
		bool bUnhappy = isMinorCiv() ? IsEmpireVeryUnhappy() : IsEmpireUnhappy();
		if (bUnhappy || (GC.getMap().GetAIMapHint() & 2) || (GetPlayerTraits()->GetRazeSpeedModifier() > 0 && getNumCities() >= 3 + (GC.getGame().getGameTurn() / 100)) )
#else
		if (IsEmpireUnhappy() || (GC.getMap().GetAIMapHint() & 2) || (GetPlayerTraits()->GetRazeSpeedModifier() > 0 && getNumCities() >= 3 + (GC.getGame().getGameTurn() / 100)) )
#endif
		{
			pCity->doTask(TASK_RAZE);
			return;
		}
	}

	// Puppet the city
	if(pCity->getOriginalOwner() != GetID() || GET_PLAYER(m_eID).GetPlayerTraits()->IsNoAnnexing())
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

	// we own it
	if (pPlot->getTeam() == getTeam())
		return true;

	// no man's land - may as well
	if (pPlot->getTeam() == NO_TEAM)
		return true;

	// friendly, sure (okay, this is pretty much just means open borders)
	if (pPlot->IsFriendlyTerritory(GetID()))
		return true;

	// not friendly, but "near" us
	pNearestCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), NO_PLAYER, getTeam());
	if (pNearestCity != NULL)
	{
		if (plotDistance(pPlot->getX(), pPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) <= 7)
			return true;
	}

	// very near someone we aren't friends with (and far from our nearest city)
	pNearestCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY());
	if (pNearestCity != NULL)
	{
		if (plotDistance(pPlot->getX(), pPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) <= 4)
			return false;
	}

	// I'd rather we grab it and run than destroy it
	return true;
}

int CvPlayerAI::AI_foundValue(int iX, int iY, int, bool bStartingLoc)
{

	CvPlot* pPlot;
	int rtnValue = 0;

	pPlot = GC.getMap().plot(iX, iY);

	if(bStartingLoc)
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
	while(GetNumFreeGreatPeople() > 0)
	{
		UnitTypes eDesiredGreatPerson = NO_UNIT;

		// Highly wonder competitive and still early in game?
		if(GetDiplomacyAI()->GetWonderCompetitiveness() >= 8 && GC.getGame().getGameTurn() <= (GC.getGame().getEstimateEndTurn() / 2))
		{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
			eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_ENGINEER");
#else
			eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ENGINEER");
#endif
		}
		else
		{
			// Pick the person based on our victory method
			AIGrandStrategyTypes eVictoryStrategy = GetGrandStrategyAI()->GetActiveGrandStrategy();
			if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
			{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
				eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_GREAT_GENERAL");
#else
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");
#endif
			}
			else if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
			{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
				eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_ARTIST");
#else
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST");
#endif
			}
			else if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
			{
#if defined(MOD_DIPLOMACY_CITYSTATES)
				if (MOD_DIPLOMACY_CITYSTATES)
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
					eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_GREAT_DIPLOMAT");
#else
					eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_DIPLOMAT");
#endif
				else
#endif
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
					eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_MERCHANT");
#else
					eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_MERCHANT");
#endif
			}
			else if(eVictoryStrategy == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
			{
#if defined(MOD_BUGFIX_UNITCLASS_NOT_UNIT)
				eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_SCIENTIST");
#else
				eDesiredGreatPerson = (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST");
#endif
			}
		}

		if(eDesiredGreatPerson != NO_UNIT)
		{
			CvCity* pCapital = getCapitalCity();
			if(pCapital)
			{
				pCapital->GetCityCitizens()->DoSpawnGreatPerson(eDesiredGreatPerson, true, false);
			}
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
	
#if defined(MOD_BUGFIX_MINOR)
	if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		return;
#endif

	clearResearchQueue();

#if defined(MOD_EVENTS_AI_OVERRIDE_TECH)
	if (MOD_EVENTS_AI_OVERRIDE_TECH && eBestTech == NO_TECH) {
		int iValue = 0;
		if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_AiOverrideChooseNextTech, GetID(), true) == GAMEEVENTRETURN_VALUE) {
			// Defend against modder stupidity!
			if (iValue >= 0 && iValue < GC.getNumTechInfos() && !GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) iValue)) {
				eBestTech = (TechTypes)iValue;
			}
		}
	}
#else
	// TODO: script override
#endif

	if(eBestTech == NO_TECH)
	{
		eBestTech = GetPlayerTechs()->GetTechAI()->ChooseNextTech(/*bFreeTech*/ true);
	}

	if(eBestTech != NO_TECH)
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

	if(GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{
		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if(GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if((iI != GetID()) && (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam()))
				{
					if(GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->GetCurrentResearch() != NO_TECH)
					{
						if(GetPlayerTechs()->CanResearch(GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->GetCurrentResearch()))
						{
							pushResearch(GET_PLAYER((PlayerTypes)iI).GetPlayerTechs()->GetCurrentResearch());
						}
					}
				}
			}
		}
	}

	if(GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
	{
#if defined(MOD_EVENTS_AI_OVERRIDE_TECH)
		if (MOD_EVENTS_AI_OVERRIDE_TECH && eBestTech == NO_TECH) {
			int iValue = 0;
			if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_AiOverrideChooseNextTech, GetID(), false) == GAMEEVENTRETURN_VALUE) {
				// Defend against modder stupidity!
				if (iValue >= 0 && iValue < GC.getNumTechInfos() && !GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) iValue)) {
					eBestTech = (TechTypes)iValue;
				}
			}
		}
#else
		//todo: script override
#endif

		if(eBestTech == NO_TECH)
		{
			eBestTech = GetPlayerTechs()->GetTechAI()->ChooseNextTech();
		}

		if(eBestTech != NO_TECH)
		{
			pushResearch(eBestTech);
		}
	}
}

// sort player numbers
struct CityAndProduction
{
	CvCity* pCity;
	int iProduction;
};

struct CityAndProductionEval
{
	bool operator()(CityAndProduction const& a, CityAndProduction const& b) const
	{
		return (a.iProduction > b.iProduction);
	}
};

void CvPlayerAI::AI_considerAnnex()
{
	AI_PERF("AI-perf.csv", "AI_ considerAnnex");

	// if the empire is unhappy, don't consider annexing
	if (IsEmpireUnhappy())
	{
		return;
	}

	// if we're going for a culture victory, don't consider annexing
	if (GetDiplomacyAI()->IsGoingForCultureVictory())
	{
		return;
	}

	// for Venice
	if (GetPlayerTraits()->IsNoAnnexing())
	{
		return;
	}

	// if their capital city is puppeted, annex it
	CvCity* pCity = getCapitalCity();
	if (pCity && pCity->IsPuppet())
	{
		// we should only annex one city a turn, and sense this is one, we're done!
		pCity->DoAnnex();
		return;
	}

	std::vector<CityAndProduction> aCityAndProductions;
	int iLoop = 0;
	pCity = NULL;

	// Find first coastal city in same area as settler
	for(pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		CityAndProduction kEval;
		kEval.pCity = pCity;
		kEval.iProduction = pCity->getYieldRateTimes100(YIELD_PRODUCTION, false);
		aCityAndProductions.push_back(kEval);
	}
	
	std::stable_sort(aCityAndProductions.begin(), aCityAndProductions.end(), CityAndProductionEval());
	
	CvCity* pTargetCity = NULL;
	float fCutoffValue = GC.getNORMAL_ANNEX();
	BuildingClassTypes eCourthouseType = NO_BUILDINGCLASS;
	// find courthouse
	for(int eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(eBuildingType);
		CvBuildingEntry* buildingInfo = GC.getBuildingInfo(eBuilding);

		if(buildingInfo)
		{
			if (buildingInfo->IsNoOccupiedUnhappiness())
			{
				eCourthouseType = (BuildingClassTypes)buildingInfo->GetBuildingClassType();
				break;
			}
		}
	}

	bool bCourthouseImprovement = false;
	if (eCourthouseType != NO_BUILDINGCLASS)
	{
		if (GetPlayerPolicies()->GetBuildingClassProductionModifier(eCourthouseType) > 0)
		{
			bCourthouseImprovement = true;
		}
	}

	if (bCourthouseImprovement)
	{
		fCutoffValue = GC.getAGGRESIVE_ANNEX();
	}

	uint uiCutOff = (uint)(aCityAndProductions.size() * fCutoffValue);
	for (uint ui = 0; ui < uiCutOff; ui++)
	{
		if (aCityAndProductions[ui].pCity->IsPuppet())
		{
			pTargetCity = aCityAndProductions[ui].pCity;
			break;
		}
	}

	if (pTargetCity)
	{
		if (!pTargetCity->IsResistance())
		{
			pTargetCity->DoAnnex();
		}
	}
}

int CvPlayerAI::AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, int iRange)
{
	int iCount = 0;

	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		CvPlot* pMissionPlot = pLoopUnit->GetMissionAIPlot();
		if(!pMissionPlot)
		{
			continue;
		}

		MissionAITypes eGroupMissionAI = pLoopUnit->GetMissionAIType();
		if(eGroupMissionAI != eMissionAI)
		{
			continue;
		}

		int iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pMissionPlot->getX(), pMissionPlot->getY());
		if(iDistance == iRange)
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

	if(GetPlayerTechs()->GetCurrentResearch() == NO_TECH)
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
	MOD_SERIALIZE_INIT_READ(kStream);
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
	MOD_SERIALIZE_INIT_WRITE(kStream);
}

void CvPlayerAI::AI_launch(VictoryTypes eVictory)
{
	if(GET_TEAM(getTeam()).isHuman())
	{
		return;
	}

	if(!GET_TEAM(getTeam()).canLaunch(eVictory))
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
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
			thisSlot = pThisOperation->PeekAtNextUnitToBuild(iAreaID);
			if(thisSlot.IsValid())
			{
				break;
			}
		}
	}

	return thisSlot;
}


OperationSlot CvPlayerAI::CityCommitToBuildUnitForOperationSlot(int iAreaID, int iTurns, CvCity* pCity)
{
	OperationSlot thisSlot;

	// search through our operations till we find one that needs a unit
	std::map<int, CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
			thisSlot = pThisOperation->CommitToBuildNextUnit(iAreaID, iTurns, pCity);
			if(thisSlot.IsValid())
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
	if(pThisOperation)
	{
		pThisOperation->UncommitToBuild(thisSlot);
	}
}

void CvPlayerAI::CityFinishedBuildingUnitForOperationSlot(OperationSlot thisSlot, CvUnit* pThisUnit)
{
	// find this operation
	CvAIOperation* pThisOperation = getAIOperation(thisSlot.m_iOperationID);
	CvArmyAI* pThisArmy = getArmyAI(thisSlot.m_iArmyID);
	if(pThisOperation && pThisArmy && pThisUnit)
	{
		pThisArmy->AddUnit(pThisUnit->GetID(), thisSlot.m_iSlotID);
		pThisOperation->FinishedBuilding(thisSlot);
	}
}

int CvPlayerAI::GetNumUnitsNeededToBeBuilt()
{
	int iRtnValue = 0;

	std::map<int, CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
			iRtnValue += pThisOperation->GetNumUnitsNeededToBeBuilt();
		}
	}

	return iRtnValue;
}

void CvPlayerAI::ProcessGreatPeople(void)
{
	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	CvAssert(isAlive());

	if(!isAlive())
		return;

	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->getSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}

		GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
		switch(pLoopUnit->AI_getUnitAIType())
		{
		case UNITAI_WRITER:
			eDirective = GetDirectiveWriter(pLoopUnit);
			break;
		case UNITAI_ARTIST:
			eDirective = GetDirectiveArtist(pLoopUnit);
			break;
		case UNITAI_MUSICIAN:
			eDirective = GetDirectiveMusician(pLoopUnit);
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
		case UNITAI_PROPHET:
			eDirective = GetDirectiveProphet(pLoopUnit);
			break;
		case UNITAI_ADMIRAL:
			eDirective = GetDirectiveAdmiral(pLoopUnit);
			break;
#if defined(MOD_DIPLOMACY_CITYSTATES)
		case UNITAI_DIPLOMAT:
			eDirective = GetDirectiveDiplomat(pLoopUnit);
			break;
#endif
		}

		pLoopUnit->SetGreatPeopleDirective(eDirective);
	}
}

bool PreparingForWar(CvPlayerAI* pPlayer)
{
	CvAssertMsg(pPlayer, "Need a player");
	if(!pPlayer)
	{
		return false;
	}
	CvMilitaryAI* pMilitaryAI = pPlayer->GetMilitaryAI();
	CvAssertMsg(pMilitaryAI, "No military AI");
	if(!pMilitaryAI)
	{
		return false;
	}

	MilitaryAIStrategyTypes eWarMobilizationStrategy = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
	if(pMilitaryAI->IsUsingStrategy(eWarMobilizationStrategy))
	{
		return true;
	}

	return false;
}

bool IsSafe(CvPlayerAI* pPlayer)
{
	CvAssertMsg(pPlayer, "Need a player");
	if(!pPlayer)
	{
		return false;
	}

	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		return true;
	}
	else
	{
		CvMilitaryAI* pMilitaryAI = pPlayer->GetMilitaryAI();
		CvAssertMsg(pMilitaryAI, "No military AI");
		if(!pMilitaryAI)
		{
			return false;
		}

		MilitaryAIStrategyTypes eAtWarStrategy = (MilitaryAIStrategyTypes)GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
		if(!pMilitaryAI->IsUsingStrategy(eAtWarStrategy))
		{
			return true;
		}

		return false;
	}
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveWriter(CvUnit* pGreatWriter)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	// Defend against ideology pressure if not going for culture win
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !GetDiplomacyAI()->IsGoingForCultureVictory() && GetCulture()->GetPublicOpinionUnhappiness() > 10)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST;
	}

	// If not going for culture win and a Level 2 or 3 Tenet is available, try to snag it
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !GetDiplomacyAI()->IsGoingForCultureVictory() && GetPlayerPolicies()->CanGetAdvancedTenet())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST;
	}

	// Create Great Work if there is a slot
	GreatWorkType eGreatWork = pGreatWriter->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatWriter->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	else if (!MOD_AI_SMART_GREAT_PEOPLE || (GC.getGame().getGameTurn() - pGreatWriter->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
#else
	else
#endif
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST;
	}

#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	// If still no directive, defaults at building great work.
	if (MOD_AI_SMART_GREAT_PEOPLE && eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
#endif

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveArtist(CvUnit* pGreatArtist)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	// Defend against ideology pressure if not going for culture win
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !GetDiplomacyAI()->IsGoingForCultureVictory() && GetCulture()->GetPublicOpinionUnhappiness() > 10)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	// If prepping for war, Golden Age will build units quickly
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !GetDiplomacyAI()->IsGoingForCultureVictory() && PreparingForWar(this))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	// If finishing up spaceship parts, Golden Age will help build those quickly
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetDiplomacyAI()->IsGoingForSpaceshipVictory() && EconomicAIHelpers::IsTestStrategy_GS_SpaceshipHomestretch(this))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	// If Persia and I'm at war, get a Golden Age going
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetPlayerTraits()->GetGoldenAgeMoveChange() > 0 && GetMilitaryAI()->GetNumberCivsAtWarWith() > 1 && !isGoldenAge())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	// If Brazil and we're closing in on Culture Victory
#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	int iInfluentialOn = MOD_AI_SMART_GREAT_PEOPLE ? (GC.getGame().GetGameCulture()->GetNumCivsInfluentialForWin() / 4) : 0;
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetPlayerTraits()->GetGoldenAgeTourismModifier() > 0 && GetCulture()->GetNumCivsInfluentialOn() > iInfluentialOn)
#else
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetPlayerTraits()->GetGoldenAgeTourismModifier() > 0 && GetCulture()->GetNumCivsInfluentialOn() > 0)
#endif
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

	// Create Great Work if there is a slot
	GreatWorkType eGreatWork = pGreatArtist->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatArtist->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !isGoldenAge() && (!MOD_AI_SMART_GREAT_PEOPLE || (GC.getGame().getGameTurn() - pGreatArtist->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT()))
#else
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !isGoldenAge())
#endif
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	// If still no directive, defaults at building great work.
	if (MOD_AI_SMART_GREAT_PEOPLE && eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
#endif

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveMusician(CvUnit* pGreatMusician)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	// If headed on a concert tour, keep going
	if (pGreatMusician->getArmyID() != FFreeList::INVALID_INDEX)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;
	}

	// If closing in on a Culture win, go for the Concert Tour
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetDiplomacyAI()->IsGoingForCultureVictory() && GetCulture()->GetNumCivsInfluentialOn() > (GC.getGame().GetGameCulture()->GetNumCivsInfluentialForWin() / 2))
	{		
		CvPlot* pTarget = FindBestMusicianTargetPlot(pGreatMusician, true);
		if(pTarget)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;
		}
	}

	// Create Great Work if there is a slot
	GreatWorkType eGreatWork = pGreatMusician->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatMusician->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	else if (!MOD_AI_SMART_GREAT_PEOPLE || (GC.getGame().getGameTurn() - pGreatMusician->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
#else
	else
#endif
	{
		CvPlot* pTarget = FindBestMusicianTargetPlot(pGreatMusician, true);
		if(pTarget)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;
		}
	}

#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	// If still no directive, defaults at building great work.
	if (MOD_AI_SMART_GREAT_PEOPLE && eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
#endif

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveEngineer(CvUnit* pGreatEngineer)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	// look for a wonder to rush
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		int iNextWonderWeight;
		BuildingTypes eNextWonderDesired = GetWonderProductionAI()->ChooseWonder(false /*bUseAsyncRandom*/, false /*bAdjustForOtherPlayers*/, iNextWonderWeight);
		if(eNextWonderDesired != NO_BUILDING)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 3) / 4))
	{
		if (GetDiplomacyAI()->IsGoingForWorldConquest())
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}

	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveMerchant(CvUnit* pGreatMerchant)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	bool bTheVeniceException = false;
	if (GetPlayerTraits()->IsNoAnnexing())
	{
		bTheVeniceException = true;
	}

	// if the merchant is in an army, he's already marching to a destination, so don't evaluate him
	if(pGreatMerchant->getArmyID() != FFreeList::INVALID_INDEX)
	{
		return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 2) / 4))
	{
#if defined(MOD_DIPLOMACY_CITYSTATES)
		bool bConstructImprovement = !bTheVeniceException;
		if (!MOD_DIPLOMACY_CITYSTATES) bConstructImprovement = bConstructImprovement && GetDiplomacyAI()->IsGoingForDiploVictory();

		if (bConstructImprovement)
#else
		if (GetDiplomacyAI()->IsGoingForDiploVictory() && !bTheVeniceException)
#endif
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}

#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	if (MOD_AI_SMART_GREAT_PEOPLE && eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 1) / 4))
	{
		if (!bTheVeniceException)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}	
	}
#endif

	// Attempt a run to a minor civ
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && IsSafe(this))
	{
		CvPlot* pTarget = FindBestMerchantTargetPlot(pGreatMerchant, true);
		if(pTarget)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatMerchant->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() && !bTheVeniceException)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveScientist(CvUnit* /*pGreatScientist*/)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	// If I'm in danger, use great person to get a tech boost
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && !IsSafe(this))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	int iDivisor = MOD_AI_SMART_GREAT_PEOPLE ? 3 : 4;
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 1) / iDivisor))
#else
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 1) / 4))
#endif
	{
		if(GetDiplomacyAI()->IsGoingForSpaceshipVictory())
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
#if defined(MOD_AI_SMART_GREAT_PEOPLE)
		else if (MOD_AI_SMART_GREAT_PEOPLE)
		{
			// Even if not going spaceship right now, build academy 66% of times.
			if ((GC.getGame().getGameTurn()) % 3 != 0)
			{
				eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
			}
		}
#endif
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		// a tech boost is never bad
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveGeneral(CvUnit* pGreatGeneral)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");

	int iGreatGeneralCount = 0;

	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->getSpecialUnitType() != eSpecialUnitGreatPerson)
		{
			continue;
		}

		if(pLoopUnit->AI_getUnitAIType() == UNITAI_GENERAL && pLoopUnit->GetGreatPeopleDirective() != GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE)
		{
			iGreatGeneralCount++;
		}
	}

	if(iGreatGeneralCount > 2 && pGreatGeneral->plot()->getOwner() == pGreatGeneral->getOwner())
	{
		// we're using a power at this point because constructing the improvement goes through different code
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveProphet(CvUnit*)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	ReligionTypes eReligion = GetReligions()->GetReligionCreatedByPlayer();
	const CvReligion* pMyReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, GetID());

	// CASE 1: I have an enhanced religion
	if (pMyReligion && pMyReligion->m_bEnhanced)
	{
#if defined(MOD_AI_SMART_GREAT_PEOPLE)
		bool bSpreadReligion = GetReligionAI()->ChooseProphetConversionCity(false/*bOnlyBetterThanEnhancingReligion*/);
		
		// If gets a phophet very early in the game progress, holy site is a great option.
		if(MOD_AI_SMART_GREAT_PEOPLE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 1) / 5))
		{
			if ((GC.getGame().getGameTurn()) % 3 != 0)
			{
				bSpreadReligion = false;
			}
		}
#endif

		// Spread religion if there is any city that needs it
#if defined(MOD_AI_SMART_GREAT_PEOPLE)
		if (bSpreadReligion)
#else
		if (GetReligionAI()->ChooseProphetConversionCity(false/*bOnlyBetterThanEnhancingReligion*/))
#endif
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;
		}
		else
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}


	// CASE 2: I have a religion that hasn't yet been enhanced
	else if (pMyReligion)
	{
		// Spread religion if there is a city that needs it CRITICALLY
		if (GetReligionAI()->ChooseProphetConversionCity(true/*bOnlyBetterThanEnhancingReligion*/))
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;
		}
		else
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	// CASE 3: No religion for me yet
	else
	{
		// Locked out?
		if (GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() <= 0)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}

		// Not locked out
		else
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveAdmiral(CvUnit* /*pGreatAdmiral*/)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	return eDirective;
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveDiplomat(CvUnit* pGreatDiplomat)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	CvCity* pCity = FindBestDiplomatTargetCity(pGreatDiplomat);

	bool bTheVeniceException = false;
	if (GetPlayerTraits()->IsNoAnnexing())
	{
		bTheVeniceException = true;
	}

	bool bTheAustriaException = false;
	if (GetPlayerTraits()->IsAbleToAnnexCityStates())
	{
		bTheAustriaException = true;
	}

	PlayerTypes eID = GetDiplomacyAI()->GetPlayer()->GetID();
	
	int iFlavorDiplo =  GET_PLAYER(eID).GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
	int iDesiredEmb = (iFlavorDiplo - 3);
	int iEmbassies = GET_PLAYER(eID).GetImprovementLeagueVotes();

	//Embassy numbers should be based on Diplomacy Flavor. More flavor, more embassies!
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && pCity != NULL)
	{
		if (!bTheAustriaException && !bTheVeniceException)
		{
			if((iEmbassies < iDesiredEmb) || GetDiplomacyAI()->IsGoingForDiploVictory())
			{
				eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
			}
		}
	}

	if(GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT && (GC.getGame().getGameTurn() - pGreatDiplomat->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		if(pCity == NULL || (iEmbassies >= iDesiredEmb) || bTheAustriaException || bTheVeniceException)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatDiplomat->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return eDirective;
}
#endif

bool CvPlayerAI::GreatMerchantWantsCash()
{
	// slewis - everybody wants cash . . .
	// slewis - . . . except Venice. Venice wants to buy city states, unless it already has enough cities, then it doesn't want city states.
	bool bIsVenice = GetPlayerTraits()->IsNoAnnexing();
	if (bIsVenice)
	{
		if (getNumCities() >= 4)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

CvPlot* CvPlayerAI::FindBestMerchantTargetPlot(CvUnit* pGreatMerchant, bool bOnlySafePaths)
{
	CvAssertMsg(pGreatMerchant, "pGreatMerchant is null");
	if(!pGreatMerchant)
	{
		return NULL;
	}

	int iBestTurnsToReach = MAX_INT;
	CvPlot* pBestTargetPlot = NULL;
	int iPathTurns;
	UnitHandle pMerchant = UnitHandle(pGreatMerchant);
	CvTeam& kTeam = GET_TEAM(getTeam());

	//bool bIsVenice = GetPlayerTraits()->IsNoAnnexing();
	//bool bWantsCash = GreatMerchantWantsCash();

	// Loop through each city state
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kPlayer.isMinorCiv())
		{
			continue;
		}

		// if I'm Venice, I don't want to send a Merchant of Venice to a buy a city that I have trade routes 
		// with because it's probably more valuable as a trade partner than as an owned entity
		//if (!bWantsCash)
		//{
		//	if (bIsVenice)
		//	{
		//		if (GetTrade()->IsConnectedToPlayer(kPlayer.GetID()))
		//		{
		//			continue;
		//		}
		//	}
		//}

		CvPlot* pCSPlot = kPlayer.getStartingPlot();
		if (!pCSPlot)
		{
			continue;
		}

		if (!pCSPlot->isRevealed(getTeam()))
		{
			continue;
		}

		// Is this a minor we are friendly with?
		bool bMinorCivApproachIsCorrect = (GetDiplomacyAI()->GetMinorCivApproach(kPlayer.GetID()) != MINOR_CIV_APPROACH_CONQUEST);
		bool bNotAtWar = !kTeam.isAtWar(kPlayer.getTeam());
		bool bNotPlanningAWar = GetDiplomacyAI()->GetWarGoal(kPlayer.GetID()) == NO_WAR_GOAL_TYPE;

		if(bMinorCivApproachIsCorrect && bNotAtWar && bNotPlanningAWar)
		{
			// Search all the plots adjacent to this city (since can't enter the minor city plot itself)
			for(int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
			{
				CvPlot* pAdjacentPlot = plotDirection(pCSPlot->getX(), pCSPlot->getY(), ((DirectionTypes)jJ));
				if(pAdjacentPlot != NULL)
				{
					// Make sure this is still owned by the city state and is revealed to us and isn't a water tile
					//if(pAdjacentPlot->getOwner() == (PlayerTypes)iI && pAdjacentPlot->isRevealed(getTeam()) && !pAdjacentPlot->isWater())
					bool bRightOwner = (pAdjacentPlot->getOwner() == (PlayerTypes)iI);
					bool bIsRevealed = pAdjacentPlot->isRevealed(getTeam());
					if(bRightOwner && bIsRevealed)
					{
						iPathTurns = TurnsToReachTarget(pMerchant, pAdjacentPlot, true /*bReusePaths*/, !bOnlySafePaths/*bIgnoreUnits*/);
						if(iPathTurns < iBestTurnsToReach)
						{
							iBestTurnsToReach = iPathTurns;
							pBestTargetPlot = pAdjacentPlot;
						}
					}
				}
			}
		}
	}

	return pBestTargetPlot;
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
CvCity* CvPlayerAI::FindBestDiplomatTargetCity(UnitHandle pUnit)
{
	CvCity* pBestCity = NULL;
	int iBestScore = 0;

	// Loop through each city state
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
		//Loop through each city
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if(pLoopCity && (GET_PLAYER(pLoopCity->getOwner()).isMinorCiv()))
				{
					int iScore = ScoreCityForDiplomat(pLoopCity, pUnit);
					if(iScore > iBestScore)
					{
						iBestScore = iScore;
						pBestCity = pLoopCity;
					}
				}
			}
		}
	}
	return pBestCity;
}

CvCity* CvPlayerAI::FindBestMessengerTargetCity(UnitHandle pUnit)
{
	CvCity* pBestCity = NULL;
	int iBestScore = 0;

	// Loop through each city state
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive())
		{
		//Loop through each city
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if(pLoopCity && (GET_PLAYER(pLoopCity->getOwner()).isMinorCiv()))
				{
					int iScore = ScoreCityForMessenger(pLoopCity, pUnit);
					if(iScore > iBestScore)
					{
						iBestScore = iScore;
						pBestCity = pLoopCity;
					}
				}
			}
		}
	}
	return pBestCity;
}	

int CvPlayerAI::ScoreCityForDiplomat(CvCity* pCity, UnitHandle pUnit)
{
	int iScore = 0;
	int iI;
	CvPlayer& kCityPlayer = GET_PLAYER(pCity->getOwner());

	// Skip if not revealed
	if(!pCity->plot()->isRevealed(getTeam()))
	{
		return iScore;
	}

	// Return score if at war
	if(atWar(getTeam(), kCityPlayer.getTeam()))
	{
		return iScore;
	}

	//Return score if we can't embark and they aren't on our landmass.
	if(pCity->getArea() != pUnit->plot()->getArea())
	{
		if(!GET_TEAM(getTeam()).canEmbarkAllWaterPassage())
		{
			return iScore;
		}
	}

	// Do we already have an embassy here?
	// To iterate all plots owned by a CS, wrap this is a loop that iterates all cities owned by the CS
	// Iterate all plots owned by a city
#if defined(MOD_GLOBAL_CITY_WORKING)
	for(iI = 0; iI < pCity->GetNumWorkablePlots(); iI++)
#else
	for(iI = 0; iI < NUM_CITY_PLOTS; iI++)
#endif
	{
		CvPlot* pCityPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);

		if(pCityPlot != NULL)
		{
			ImprovementTypes eEmbassy = (ImprovementTypes)GC.getEMBASSY_IMPROVEMENT();
			ImprovementTypes CSImprovement = pCityPlot->getImprovementType();
			if(CSImprovement == eEmbassy)
			{
				return iScore;
			}
		}
	}

	//Are we planning on conquering them?
	if(GetDiplomacyAI()->GetMinorCivApproach(kCityPlayer.GetID()) == MINOR_CIV_APPROACH_CONQUEST)
	{
		return iScore;
	}

	iScore = 100;

	// Then subtract distance
	iScore -= (plotDistance(pUnit->getX(), pUnit->getY(), pCity->getX(), pCity->getY()) * GC.getINFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE());

	return iScore;
}
	
int CvPlayerAI::ScoreCityForMessenger(CvCity* pCity, UnitHandle pUnit)
{
	//Initializations...
	
	EconomicAIStrategyTypes eDevelopingReligion = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_DEVELOPING_RELIGION");
	EconomicAIStrategyTypes eNeedHappinessCritical = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL");
	EconomicAIStrategyTypes eNeedHappiness = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS");
	EconomicAIStrategyTypes eFoundCity = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_FOUND_CITY");
	EconomicAIStrategyTypes eExpandLikeCrazy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY");
	EconomicAIStrategyTypes eExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	bool bDevelopingReligion = (eFoundCity != NO_ECONOMICAISTRATEGY) ? GetDiplomacyAI()->GetPlayer()->GetEconomicAI()->IsUsingStrategy(eDevelopingReligion) : false;
	bool bNeedHappinessCritical = (eFoundCity != NO_ECONOMICAISTRATEGY) ? GetDiplomacyAI()->GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNeedHappinessCritical) : false;
	bool bNeedHappiness = (eFoundCity != NO_ECONOMICAISTRATEGY) ? GetDiplomacyAI()->GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNeedHappiness) : false;
	bool bFoundCity = (eFoundCity != NO_ECONOMICAISTRATEGY) ? GetDiplomacyAI()->GetPlayer()->GetEconomicAI()->IsUsingStrategy(eFoundCity) : false;
	bool bExpandLikeCrazy = (eExpandLikeCrazy != NO_ECONOMICAISTRATEGY) ? GetDiplomacyAI()->GetPlayer()->GetEconomicAI()->IsUsingStrategy(eExpandLikeCrazy) : false;
	bool bExpandToOtherContinents = (eExpandToOtherContinents != NO_ECONOMICAISTRATEGY) ? GetDiplomacyAI()->GetPlayer()->GetEconomicAI()->IsUsingStrategy(eExpandToOtherContinents) : false;
	int iScore = 0;
	PlayerTypes eID = GetDiplomacyAI()->GetPlayer()->GetID();
	CvPlot* pPlot = pCity->plot();
	int iFriendship = pUnit->getTradeInfluence(pPlot);

	CvMinorCivAI* pMinorCivAI;

	
	CvPlayer& eMinor= GET_PLAYER(pCity->getOwner());
	PlayerTypes pMinor = eMinor.GetID();
	pMinorCivAI = eMinor.GetMinorCivAI();

	int iOtherMajorLoop;
	PlayerTypes eOtherMajor;
	int iFriendshipWithMinor;
	int iOtherPlayerFriendshipWithMinor;


	MinorCivApproachTypes eApproach;

	//And now for the exclusions!

	// Skip if not revealed.
	if(!pCity->plot()->isRevealed(getTeam()))
	{
		return iScore;
	}

	//If we are at war with target minor, let's not send diplomatic lambs to slaughter.
	if(eMinor.GetMinorCivAI()->IsAtWarWithPlayersTeam(eID))
	{
		return iScore;
	}

	bool bWantsToBuyout = false;
	if(GetDiplomacyAI()->GetPlayer()->IsAbleToAnnexCityStates())
	{
		if(bFoundCity || bExpandLikeCrazy || bExpandToOtherContinents ||
		        GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING ||
		        GetDiplomacyAI()->IsGoingForWorldConquest())
		{
			bWantsToBuyout = true;
			return iScore;
		}
	}
	
	// **************************
	// Approaches
	// **************************

	iScore = 100;
	eApproach = GetDiplomacyAI()->GetMinorCivApproach(pMinor);

	// Do we want to change our protection of this minor?
	GetDiplomacyAI()->DoUpdateMinorCivProtection(pMinor, eApproach);

	if (eApproach == MINOR_CIV_APPROACH_IGNORE)
	{
		iScore *= 2;
	}

	else if (eApproach == MINOR_CIV_APPROACH_FRIENDLY)
	{
		iScore *= 3;
	}

	else if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE)
	{
		iScore  *= 4;
	}

	// **************************
	// Benefits to Us!
	// **************************

	//DIPLOMACY - We want all of them the same!
	if(GetDiplomacyAI()->IsGoingForDiploVictory())
	{
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_CULTURED)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iScore *= 2;
		}
	}

	//MILITARY - We want units and happiness!!
	else if(GetDiplomacyAI()->IsGoingForWorldConquest())
	{
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_CULTURED)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iScore *= 4;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iScore *= 3;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iScore *= 2;
		}
	}

	//SCIENCE - We want happiness and growth!!
	else if(GetDiplomacyAI()->IsGoingForSpaceshipVictory())
	{
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_CULTURED)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME)
		{
			iScore *= 3;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iScore *= 4;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iScore *= 2;
		}
	}

	//CULTURE - We want culture and religion!!
	else if(GetDiplomacyAI()->IsGoingForCultureVictory())
	{
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_CULTURED)
		{
			iScore *= 4;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iScore *= 2;
		}
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iScore *= 3;
		}
	}

	if(bDevelopingReligion)
	{
		if(pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iScore *= 3;
		}
	}



	// Is Our Influence worth more here? Definitely take advantage of this.
	if(pMinorCivAI->IsActiveQuestForPlayer(eID, MINOR_CIV_QUEST_INFLUENCE))
	{
		iScore *= 4;
	}
	
	// Do they have a resource we lack?
	int iResourcesWeLack = pMinorCivAI->GetNumResourcesMajorLacks(eID);
	if(iResourcesWeLack > 0)
	{
		if(bNeedHappiness)
		{
			iScore *= 3;
		}
		else if(bNeedHappinessCritical)
		{
			iScore *= 5;
		}
		else
		{
			iScore *= 2;
		}
	}

	//Nobody likes hostile city-states.
	if(pMinorCivAI->GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
	{
		iScore /= 2;
	}

	// **************************
	// Other Relationships!
	// **************************

	PlayerTypes eAlliedPlayer = NO_PLAYER;
	iFriendshipWithMinor = pMinorCivAI->GetEffectiveFriendshipWithMajor(eID);
	eAlliedPlayer = pMinorCivAI->GetAlly();
	
	if(eAlliedPlayer != NO_PLAYER)
	{
		// Loop through other players to see if we can pass them in influence
		for(iOtherMajorLoop = 0; iOtherMajorLoop < MAX_MAJOR_CIVS; iOtherMajorLoop++)
		{
			eOtherMajor = (PlayerTypes) iOtherMajorLoop;

			iOtherPlayerFriendshipWithMinor = pMinorCivAI->GetEffectiveFriendshipWithMajor(eOtherMajor);

			// If another player is allied, let's evaluate that.

			//Are they not on our team?
			if(GET_PLAYER(eOtherMajor).getTeam() != getTeam())
			{
				// Only care if they are allies
				if(pMinorCivAI->IsAllies(eOtherMajor))
				{
					//If their influence is way higher than ours, let's tune this down...
					if(iOtherPlayerFriendshipWithMinor >= (60 + iFriendship + iFriendshipWithMinor))
					{
						iScore /= 3;
					}
					//If we can pass them, ramp it up!
					else if(iOtherPlayerFriendshipWithMinor < (iFriendship + iFriendshipWithMinor)) 
					{
						iScore *= 3;
					}
				}
			}
			// If a teammate is allied, let's discourage going there.
			else if(GET_PLAYER(eOtherMajor).getTeam() == getTeam())
			{
				// Only care if they are allies
				if(pMinorCivAI->IsAllies(eOtherMajor))
				{
					iScore /= 3;
				}
			}
			// Are we allied? Yay! But let's be careful.
			if(pMinorCivAI->IsAllies(eID))
			{
				// Are WE allies by a wide margin (over 100)? If so, let's find someone new to love.
				if(iFriendshipWithMinor >= (60 + iOtherPlayerFriendshipWithMinor)) 
				{
					iScore /= 4;
				}
				// Is another player really close to us? If so, let's keep an eye on things.
				if(iOtherPlayerFriendshipWithMinor >= (iFriendshipWithMinor - 30))
				{
					iScore *= 2;
				}
					// Are we close to losing our status? If so, obsess away!
				if(pMinorCivAI->IsCloseToNotBeingAllies(eID))
				{
					iScore *= 5;
				}
			}
			MajorCivApproachTypes eApproachType = GetDiplomacyAI()->GetMajorCivApproach(eOtherMajor, false);
			MajorCivOpinionTypes eOpinion = GetDiplomacyAI()->GetMajorCivOpinion(eOtherMajor);
			// If a friendly player is allied, let's discourage going there.
			if(eApproachType == MAJOR_CIV_APPROACH_FRIENDLY)
			{
				// Only care if they are allies
				if(pMinorCivAI->IsAllies(eOtherMajor))
				{
					iScore /= 2;
				}
			}
			// If an enemy is allied, let's take their stuff!
			else if(eApproachType == MAJOR_CIV_APPROACH_HOSTILE)
			{
				// Only care if they are allies
				if(pMinorCivAI->IsAllies(eOtherMajor))
				{
					iScore *= 2;
				}
			}
			// If an ally is allied, let's leave it alone!
			if(eOpinion == MAJOR_CIV_OPINION_ALLY)
			{
				// Only care if they are allies
				if(pMinorCivAI->IsAllies(eOtherMajor))
				{
					iScore /= 2;
				}
			}
			// If an competitor is allied, let's fight for it!
			else if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
			{
				// Only care if they are allies
				if(pMinorCivAI->IsAllies(eOtherMajor))
				{
					iScore *= 2;
				}
			}
		}
	}
	// Are we close to becoming an normal (60) ally and no one else ? If so, obsess away!
	if((iFriendshipWithMinor + iFriendship) >= pMinorCivAI->GetAlliesThreshold())
	{
			iScore *= 6;
	}

	// Are we already Friends? If so, let's stay the course.
	if(pMinorCivAI->IsFriends(eID))
	{
		iScore *= 2;
	}

	// Did we bully you recently?  If so, being friendly now would be very odd.
	if(pMinorCivAI->IsRecentlyBulliedByMajor(eID))
	{
		iScore /= 6;
	}

	// **************************
	// Distance
	// **************************

	
	CvMap& theMap = GC.getMap();
	CvArea* pArea = theMap.getArea(pCity->getArea());

	// Subtract distance (XML value important here!)
	iScore -= ((plotDistance(pUnit->getX(), pUnit->getY(), pCity->getX(), pCity->getY())) * GC.getINFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE());

	// Multiplier by how safe it is
	if(!atWar(getTeam(), eMinor.getTeam()))
	{
		iScore *= 2;
	}

	//If our friendship is under 0, we've probably done something bad to this City-State. Let's not look at them!
	if(eMinor.GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eID) < 0)
	{
		iScore /= 2;
	}

	//Are there barbarians near the city-state? If so, abort!
	if(eMinor.GetMinorCivAI()->IsThreateningBarbariansEventActiveForPlayer(eID))
	{
		iScore /= 4;
	}

	//Let's downplay minors we can't walk to if we don't have embarkation.
	if((pCity->getArea() != pUnit->getArea()) && !GET_TEAM(GET_PLAYER(eID).getTeam()).canEmbark())
	{
		iScore /= 3;
	}

	//Let's downplay far/distant minors without full embarkation.
	else if((pCity->getArea() != pUnit->getArea()) && !GET_TEAM(GET_PLAYER(eID).getTeam()).canEmbarkAllWaterPassage())
	{
		iScore /= 2;
	}

	//If we can travel across oceans, let's emphasize island CSs to get them alliances.
	else if((pArea->getNumTiles() <= 15) && GET_TEAM(GET_PLAYER(eID).getTeam()).canEmbarkAllWaterPassage())
	{
		iScore *= 2;
	}

	return iScore;
}

CvPlot* CvPlayerAI::ChooseDiplomatTargetPlot(UnitHandle pUnit, int* piTurns)
{
	CvCity* pCity = FindBestDiplomatTargetCity(pUnit);
	int iBestNumTurns = MAX_INT;
	int iTurns;
	int iBestDistance = MAX_INT;
	int iDistance;
	CvPlot* pBestTarget = NULL;

	if(pCity == NULL)
	{
		return NULL;
	}
	// Find adjacent plot with no units (that aren't our own)
	CvPlot* pLoopPlot;
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{
			CvUnit* pFirstUnit = pLoopPlot->getUnitByIndex(0);
			if(pFirstUnit && pFirstUnit->getOwner() != GetID())
			{
				continue;
			}
			iTurns = TurnsToReachTarget(pUnit, pLoopPlot, false /* bReusePaths */);
			if(iTurns < MAX_INT)
			{
				iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pLoopPlot->getX(), pLoopPlot->getY());

				if(pLoopPlot->isWater())
				{
					continue;
				}

				if(iTurns < iBestNumTurns || (iTurns == iBestNumTurns && iDistance < iBestDistance))
				{
					iBestNumTurns = iTurns;
					iBestDistance = iDistance;
					pBestTarget = pLoopPlot;
				}
			}
		}
	}

	if(piTurns)
		*piTurns = iBestNumTurns;
	return pBestTarget;
}

CvPlot* CvPlayerAI::ChooseMessengerTargetPlot(UnitHandle pUnit, int* piTurns)
{
	CvCity* pCity = FindBestMessengerTargetCity(pUnit);
	int iBestNumTurns = MAX_INT;
	int iTurns;
	int iBestDistance = MAX_INT;
	int iDistance;
	CvPlot* pBestTarget = NULL;

	if(pCity == NULL)
	{
		return NULL;
	}
	// Find adjacent plot with no units (that aren't our own)
	CvPlot* pLoopPlot;
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{
			CvUnit* pFirstUnit = pLoopPlot->getUnitByIndex(0);
			if(pFirstUnit && pFirstUnit->getOwner() != GetID())
			{
				continue;
			}
			iTurns = TurnsToReachTarget(pUnit, pLoopPlot, false /* bReusePaths */);
			if(iTurns < MAX_INT)
			{
				iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pLoopPlot->getX(), pLoopPlot->getY());

				// Consider it to be twice as far if a water plot (those are dangerous!)
				if(pLoopPlot->isWater())
				{
					iDistance *= 2;
				}

				if(iTurns < iBestNumTurns || (iTurns == iBestNumTurns && iDistance < iBestDistance))
				{
					iBestNumTurns = iTurns;
					iBestDistance = iDistance;
					pBestTarget = pLoopPlot;
				}
			}
		}
	}

	if(piTurns)
		*piTurns = iBestNumTurns;
	return pBestTarget;
}
#endif

CvPlot* CvPlayerAI::FindBestMusicianTargetPlot(CvUnit* pGreatMusician, bool bOnlySafePaths)
{
	CvAssertMsg(pGreatMusician, "pGreatMusician is null");
	if(!pGreatMusician)
	{
		return NULL;
	}

	int iBestTurnsToReach = MAX_INT;
	CvPlot* pBestTargetPlot = NULL;
	CvCity* pBestTargetCity = NULL;
	int iPathTurns;
	UnitHandle pMusician = UnitHandle(pGreatMusician);

	// Find target civ
	PlayerTypes eTargetPlayer = GetCulture()->GetCivLowestInfluence(true /*bCheckOpenBorders*/);
	if (eTargetPlayer == NO_PLAYER)
	{
		return NULL;
	}

	CvPlayer &kTargetPlayer = GET_PLAYER(eTargetPlayer);

	// Loop through each of that player's cities
	int iLoop;
	CvCity *pLoopCity;
	for(pLoopCity = kTargetPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kTargetPlayer.nextCity(&iLoop))
	{
		// Search all the plots adjacent to this city
		for(int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
		{
			CvPlot* pAdjacentPlot = plotDirection(pLoopCity->getX(), pLoopCity->getY(), ((DirectionTypes)jJ));
			if(pAdjacentPlot != NULL)
			{
				// Make sure this is still owned by target and is revealed to us
				bool bRightOwner = (pAdjacentPlot->getOwner() == eTargetPlayer);
				bool bIsRevealed = pAdjacentPlot->isRevealed(getTeam());
				if(bRightOwner && bIsRevealed)
				{
					iPathTurns = TurnsToReachTarget(pMusician, pAdjacentPlot, true /*bReusePaths*/, !bOnlySafePaths/*bIgnoreUnits*/);
					if(iPathTurns < iBestTurnsToReach)
					{
						iBestTurnsToReach = iPathTurns;
						pBestTargetCity = pLoopCity;
					}
				}
			}
		}
	}

	// Found a city now look at ALL the plots owned by that player near that city
	if (pBestTargetCity)
	{
		iBestTurnsToReach = MAX_INT;
		CvPlot *pLoopPlot;
#if defined(MOD_GLOBAL_CITY_WORKING)
		for(int iJ = 0; iJ < pBestTargetCity->GetNumWorkablePlots(); iJ++)
#else
		for(int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
#endif
		{
			pLoopPlot = plotCity(pBestTargetCity->getX(), pBestTargetCity->getY(), iJ);
			if(pLoopPlot != NULL)
			{
				// Make sure this is still owned by target and is revealed to us
				bool bRightOwner = (pLoopPlot->getOwner() == eTargetPlayer);
				bool bIsRevealed = pLoopPlot->isRevealed(getTeam());
				if(bRightOwner && bIsRevealed)
				{
					iPathTurns = TurnsToReachTarget(pMusician, pLoopPlot, true /*bReusePaths*/, !bOnlySafePaths/*bIgnoreUnits*/);
					if(iPathTurns < iBestTurnsToReach)
					{
						iBestTurnsToReach = iPathTurns;
						pBestTargetPlot = pLoopPlot;
					}
				}
			}	
		}
	}

	return pBestTargetPlot;
}

CvPlot* CvPlayerAI::FindBestArtistTargetPlot(CvUnit* pGreatArtist, int& iResultScore)
{
	CvAssertMsg(pGreatArtist, "pGreatArtist is null");
	if(!pGreatArtist)
	{
		return NULL;
	}

	iResultScore = 0;

	CvPlotsVector& m_aiPlots = GetPlots();

	CvPlot* pBestPlot = NULL;
	int iBestScore = 0;

	// loop through plots and wipe out ones that are invalid
	const uint nPlots = m_aiPlots.size();
	for(uint ui = 0; ui < nPlots; ui++)
	{
		if(m_aiPlots[ui] == -1)
		{
			continue;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(m_aiPlots[ui]);

		if(pPlot->isWater())
		{
			continue;
		}

		if(!pPlot->IsAdjacentOwnedByOtherTeam(getTeam()))
		{
			continue;
		}

		// don't build over luxury resources
		ResourceTypes eResource = pPlot->getResourceType();
		if(eResource != NO_RESOURCE)
		{
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if(pkResource != NULL)
			{
				if (pkResource->getResourceUsage() == RESOURCEUSAGE_LUXURY)
				{
					continue;
				}
			}
		}

		// if no improvement can be built on this plot, then don't consider it
		FeatureTypes eFeature = pPlot->getFeatureType();
		if (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->isNoImprovement())
		{
			continue;
		}

		// Improvement already here?
		ImprovementTypes eImprovement = (ImprovementTypes)pPlot->getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
			if(pkImprovementInfo)
			{
				if (pkImprovementInfo->GetCultureBombRadius() > 0)
				{
					continue;
				}
			}
		}

		int iScore = 0;

		for(int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			// if there's no plot, bail
			if(pAdjacentPlot == NULL)
			{
				continue;
			}

			// if the plot is ours or no one's, bail
			if(pAdjacentPlot->getTeam() == NO_TEAM || pAdjacentPlot->getTeam() == getTeam())
			{
				continue;
			}

			// don't evaluate city plots since we don't get ownership of them with the bomb
			if(pAdjacentPlot->getPlotCity())
			{
				continue;
			}

			const PlayerTypes eOtherPlayer = pAdjacentPlot->getOwner();
			if(GET_PLAYER(eOtherPlayer).isMinorCiv())
			{
				MinorCivApproachTypes eMinorApproach = GetDiplomacyAI()->GetMinorCivApproach(eOtherPlayer);
				// if we're friendly or protective, don't be a jerk. Bail out.
				if(eMinorApproach != MINOR_CIV_APPROACH_CONQUEST && eMinorApproach != MINOR_CIV_APPROACH_IGNORE)
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
				if(!bTicked && !bTickedAboutLand)
				{
					iScore = 0;
					break;
				}
			}

			eResource = pAdjacentPlot->getResourceType();
			if(eResource != NO_RESOURCE)
			{
				iScore += GetBuilderTaskingAI()->GetResourceWeight(eResource, NO_IMPROVEMENT, pAdjacentPlot->getNumResource()) * 10;
			}
			else
			{
				for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
				{
					iScore += pAdjacentPlot->getYield((YieldTypes)iYield);
				}
			}
		}

		if(iScore > iBestScore)
		{
			iBestScore = iScore;
			pBestPlot = pPlot;
		}
	}

	iResultScore = iBestScore;
	return pBestPlot;
}

