/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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

#if defined(MOD_BALANCE_CORE)
#include "CvDistanceMap.h"
#endif


// Include this after all other headers.
#include "LintFree.h"

#if defined(MOD_BALANCE_CORE_MILITARY)
#include <queue>
#endif

// statics

CvPlayerAI* CvPlayerAI::m_aPlayers = NULL;

// inlined for performance reasons
CvPlayerAI& CvPlayerAI::getPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer != NO_PLAYER, "Player is not assigned a valid value");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "Player is not assigned a valid value");

	return m_aPlayers[ePlayer];
}

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

#if defined(MOD_BALANCE_CORE)
	//make sure we iterate our units in a sensible order
	struct CompareUnitPowerAscending
	{
		const TContainer<CvUnit>& container;

		CompareUnitPowerAscending(TContainer<CvUnit>& c) : container(c) {}
		bool operator()(int iID1, int iID2)
		{
			return ( container.Get(iID1)->GetPower() > container.Get(iID2)->GetPower() );
		}

	private:
		//need an assignment operator apparently
		CompareUnitPowerAscending& operator=( const CompareUnitPowerAscending& ) { return *this; }
	};

	//this orders units by combat strength
	m_units.OrderByContent( CompareUnitPowerAscending(m_units) );
#endif

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
	int iLoop;

	//order is important. when a unit was killed, an army might become invalid, which might invalidate an operation

	//unit cleanup - this should probably also be done in a two-pass scheme like below
	//but since it's too involved to change that now, we do the ugly loop to make sure we didn't skip a unit
	bool bKilledAtLeastOne = false;
	bool bKilledOneThisPass = false;
	do
	{
		bKilledOneThisPass = false;
		for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
			bKilledOneThisPass |= pLoopUnit->doDelayedDeath();
		bKilledAtLeastOne |= bKilledOneThisPass;
	} while (bKilledOneThisPass);

#if defined(MOD_CORE_DELAYED_VISIBILITY)
	//force explicit visibility update for killed units
	if (bKilledAtLeastOne)
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			GC.getMap().plotByIndexUnchecked(iI)->flipVisibility(getTeam());
#endif

	//army cleanup
	std::vector<int> itemsToDelete;
	for(CvArmyAI* pLoopArmyAI = firstArmyAI(&iLoop); pLoopArmyAI != NULL; pLoopArmyAI = nextArmyAI(&iLoop))
		if (pLoopArmyAI->IsDelayedDeath())
			itemsToDelete.push_back(pLoopArmyAI->GetID());

	for (size_t i=0; i<itemsToDelete.size(); i++)
		getArmyAI(itemsToDelete[i])->Kill();

	//operation cleanup
	itemsToDelete.clear();
	CvAIOperation* nextOp = getFirstAIOperation();
	while(nextOp)
	{
		if (nextOp->ShouldAbort())
			itemsToDelete.push_back(nextOp->GetID());

		nextOp = getNextAIOperation();
	}

	for (size_t i=0; i<itemsToDelete.size(); i++)
		getAIOperation(itemsToDelete[i])->Kill();
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

	//check garrison sanity
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->IsGarrisoned())
		{
			CvCity* pCity = pLoopUnit->GetGarrisonedCity();
			if (pCity)
			{
				CvUnit* pGarrison = pCity->GetGarrisonedUnit();
				if (pLoopUnit != pGarrison || pLoopUnit->plot() != pCity->plot())
					OutputDebugString("garrison error!");
			}
			else
				OutputDebugString("garrison error!");
		}
	}

	for(CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->HasGarrison())
		{
			CvUnit* pUnit = pLoopCity->GetGarrisonedUnit();
			if (pUnit)
			{
				CvCity *pGarrisonCity = pUnit->GetGarrisonedCity();
				if (pLoopCity != pGarrisonCity || pLoopCity->plot() != pGarrisonCity->plot())
					OutputDebugString("garrison error!");
			}
			else
				OutputDebugString("garrison error!");
		}
	}

}

//	---------------------------------------------------------------------------
void CvPlayerAI::AI_unitUpdate()
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(GetID());

		bool bResult;
		LuaSupport::CallHook(pkScriptSystem, "PlayerPreAIUnitUpdate", args.get(), bResult);
	}

	// this was a !hasBusyUnit around the entire rest of the function, so I tried to make it a bit flatter.
	if(hasBusyUnitOrCity())
	{
		return;
	}

	//do this only after updating the danger plots (happens in CvPlayer::doTurnPostDiplomacy)
	//despite the name, the tactical map is used by homeland AI as well.
	GetTacticalAI()->GetTacticalAnalysisMap()->Refresh();

	//so that workers know where to build roads
	GetBuilderTaskingAI()->Update();

	if(isHuman())
	{
		CvUnit::dispatchingNetMessage(true);
		//no tactical AI for human
		GetHomelandAI()->Update();
		AI_PERF_FORMAT("AI-perf.csv", ("AI_unitUpdate, Turn %03d, finished Human HomelandAI update", GC.getGame().getElapsedGameTurns()));
		CvUnit::dispatchingNetMessage(false);
	}
	else
	{
		// Now let the tactical AI run.  Putting it after the operations update allows units who have
		// just been handed off to the tactical AI to get a move in the same turn they switch between
		// AI subsystems. Tactical map has already been refreshed above.
		GetTacticalAI()->Update();
		GetHomelandAI()->Update();
		AI_PERF_FORMAT("AI-perf.csv", ("AI_unitUpdate, Turn %03d, finished AI HomelandAI update", GC.getGame().getElapsedGameTurns()));
	}
}

#if defined(MOD_BALANCE_CORE)
void CvPlayerAI::AI_conquerCity(CvCity* pCity, PlayerTypes eOldOwner, bool bGift, bool bAllowRaze)
#else
void CvPlayerAI::AI_conquerCity(CvCity* pCity, PlayerTypes eOldOwner)
#endif
{
	PlayerTypes eOriginalOwner = pCity->getOriginalOwner();
	TeamTypes eOldOwnerTeam = GET_PLAYER(eOldOwner).getTeam();
#if defined(MOD_BALANCE_CORE)
	if(isHuman())
	{
		return;
	}
	//Don't burn down gifts, that makes you look ungrateful.
	if (bGift && eOriginalOwner != GetID())
	{
		pCity->DoCreatePuppet();
		return;
	}
#endif

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
#if defined(MOD_BALANCE_CORE)
			if(isMinorCiv() && GetMinorCivAI()->GetAlly() == eOriginalOwner)
			{
				bLiberate = true;
			}
			if(IsEmpireUnhappy() && !GET_TEAM(getTeam()).isAtWar(eOldOwnerTeam) && !GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eOriginalOwner).getTeam()))
			{
				if(GET_PLAYER(eOriginalOwner).isMajorCiv() && GetDiplomacyAI()->GetMajorCivOpinion(eOriginalOwner) > MAJOR_CIV_OPINION_FAVORABLE)
				{
					bLiberate = true;
				}
				if(GET_PLAYER(eOriginalOwner).isMajorCiv() && GET_PLAYER(eOriginalOwner).GetDiplomacyAI()->GetMajorCivOpinion(eOriginalOwner) >= MAJOR_CIV_OPINION_NEUTRAL && GetDiplomacyAI()->GetMajorCivOpinion(eOldOwner) < MAJOR_CIV_OPINION_NEUTRAL)
				{
					bLiberate = true;
				}
			}
#endif

			if (bLiberate)
			{
				DoLiberatePlayer(eOriginalOwner, pCity->GetID());
				return;
			}
		}
	}

	// Do we want to burn this city down?
	if (canRaze(pCity) && bAllowRaze)
	{
		// Huns will burn down everything possible once they have a core of a few cities (was 3, but this put Attila out of the running long term as a conqueror)
		CUSTOMLOG("AI_conquerCity: City=%s, Player=%d, ExcessHappiness=%d", pCity->getName().GetCString(), GetID(), GetExcessHappiness());
		if ((GC.getMap().GetAIMapHint() & ciMapHint_Raze) || (GetPlayerTraits()->GetRazeSpeedModifier() > 0 && getNumCities() >= (GetDiplomacyAI()->GetBoldness() + GetDiplomacyAI()->GetMeanness() + (GC.getGame().getGameTurn() / 100))) )
		{
			pCity->doTask(TASK_RAZE);
			return;
		}

		//don't burn cities with wonders, they tend to be good
		if(IsEmpireVeryUnhappy() && !pCity->HasAnyWonder())
		{
			//Only raze if this isn't a beachhead city.
			CvArea* pArea = GC.getMap().getArea(pCity->getArea());
			if (pArea != NULL && pArea->getCitiesPerPlayer(GetID()) > 1)
			{
				MajorCivOpinionTypes eOpinion = GetDiplomacyAI()->GetMajorCivOpinion(eOldOwner);
				if (eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
				{
					pCity->doTask(TASK_RAZE);
					return;
				}
				else if (eOpinion == MAJOR_CIV_OPINION_ENEMY)
				{
					if (GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eOldOwner).getTeam()))
					{
						if (GetDiplomacyAI()->GetWarGoal(eOldOwner) == WAR_GOAL_DAMAGE)
						{
							pCity->doTask(TASK_RAZE);
							return;
						}
					}
				}
			}
		}
	}

	// Puppet the city
	if(pCity->getOriginalOwner() != GetID() || GET_PLAYER(m_eID).GetPlayerTraits()->IsNoAnnexing())
	{
		pCity->DoCreatePuppet();
		return;
	}
#if defined(MOD_BALANCE_CORE)
	//Let's make sure we annex.
	else if(pCity->getOriginalOwner() == GetID() && !GET_PLAYER(m_eID).GetPlayerTraits()->IsNoAnnexing())
	{
		pCity->DoAnnex();
		pCity->ChangeNoOccupiedUnhappinessCount(1);
		return;
	}
#endif
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
				eDesiredGreatPerson = GetMilitaryAI()->GetWarType() == WARTYPE_SEA ? GetSpecificUnitType("UNITCLASS_GREAT_ADMIRAL") : GetSpecificUnitType("UNITCLASS_GREAT_GENERAL");
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
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				pCapital->GetCityCitizens()->DoSpawnGreatPerson(eDesiredGreatPerson, true, false, MOD_GLOBAL_TRULY_FREE_GP);
#else
				pCapital->GetCityCitizens()->DoSpawnGreatPerson(eDesiredGreatPerson, true, false);
#endif
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
		eBestTech = GetPlayerTechs()->GetTechAI()->ChooseNextTech(this, /*bFreeTech*/ true);
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
		if (MOD_EVENTS_AI_OVERRIDE_TECH && eBestTech == NO_TECH) 
		{
			int iValue = 0;
			if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_AiOverrideChooseNextTech, GetID(), false) == GAMEEVENTRETURN_VALUE) 
			{
				// Defend against modder stupidity!
				if (iValue >= 0 && iValue < GC.getNumTechInfos() && !GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) iValue)) 
				{
					eBestTech = (TechTypes)iValue;
				}
			}
		}
#else
		//todo: script override
#endif

		if(eBestTech == NO_TECH)
		{
			eBestTech = GetPlayerTechs()->GetTechAI()->ChooseNextTech(this);
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

	// if the empire is very unhappy, don't consider annexing
	if (IsEmpireVeryUnhappy())
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

	BuildingClassTypes eCourthouseType = NO_BUILDINGCLASS;
	// find courthouse
	for (int eBuildingType = 0; eBuildingType < GC.getNumBuildingInfos(); eBuildingType++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(eBuildingType);
		CvBuildingEntry* buildingInfo = GC.getBuildingInfo(eBuilding);

		if (buildingInfo)
		{
			if (buildingInfo->IsNoOccupiedUnhappiness() && canConstruct(eBuilding))
			{
				eCourthouseType = (BuildingClassTypes)buildingInfo->GetBuildingClassType();
				break;
			}
		}
	}

	//Can't build a courthouse? Abort!
	if (eCourthouseType == NO_BUILDINGCLASS)
		return;

	std::vector<CityAndProduction> aCityAndProductions;
	int iLoop = 0;
	pCity = NULL;
	for(pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		//simple check to stop razing "good" cities
		if (pCity->IsRazing() && pCity->HasAnyWonder() && !IsEmpireVeryUnhappy())
			unraze(pCity);

		//Original City and puppeted? Stop!
		if(pCity->getOriginalOwner() == GetID() && pCity->IsPuppet())
		{
			pCity->DoAnnex();
			return;
		}

		if (pCity->IsResistance())
			continue;

		CityAndProduction kEval;
		kEval.pCity = pCity;
		kEval.iProduction = pCity->getYieldRateTimes100(YIELD_PRODUCTION, false);
		aCityAndProductions.push_back(kEval);
	}
	
	std::stable_sort(aCityAndProductions.begin(), aCityAndProductions.end(), CityAndProductionEval());
	
	CvCity* pTargetCity = NULL;
	float fCutoffValue = GC.getNORMAL_ANNEX();

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
#if defined(MOD_BALANCE_CORE)
		if(aCityAndProductions[ui].pCity->IsRazing())
		{
			pTargetCity = aCityAndProductions[ui].pCity;
			break;
		}
#endif
	}

	if (pTargetCity)
	{
		if (pTargetCity->IsRazing())
			unraze(pTargetCity);

		if (!pTargetCity->IsResistance())
			pTargetCity->DoAnnex();
	}
}
#if defined(MOD_BALANCE_CORE_EVENTS)
void CvPlayerAI::AI_DoEventChoice(EventTypes eChosenEvent)
{
	if(eChosenEvent != NO_EVENT)
	{
		CvModEventInfo* pkEventInfo = GC.getEventInfo(eChosenEvent);
		if(pkEventInfo != NULL)
		{
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString playerName;
				FILogFile* pLog;
				CvString strBaseString;
				CvString strOutBuf;
				CvString strFileName = "EventLogging.csv";
				playerName = getCivilizationShortDescription();
				pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
				strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
				strBaseString += playerName + ", ";
				strOutBuf.Format("AI considering Event choices: %s", pkEventInfo->GetDescription());
				strBaseString += strOutBuf;
				pLog->Msg(strBaseString);
			}
			//Lua Hook
			if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_OverrideAIEventChoice, GetID(), eChosenEvent) == GAMEEVENTRETURN_TRUE) 
			{
				return;
			}

			// Now let's get the event flavors.
			CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS * 2, true> flavorChoices;
			for(int iLoop = 0; iLoop < GC.getNumEventChoiceInfos(); iLoop++)
			{
				EventChoiceTypes eEventChoice = (EventChoiceTypes)iLoop;
				if(eEventChoice != NO_EVENT_CHOICE)
				{
					CvModEventChoiceInfo* pkEventChoiceInfo = GC.getEventChoiceInfo(eEventChoice);
					if(pkEventChoiceInfo != NULL)
					{
						if(IsEventChoiceValid(eEventChoice, eChosenEvent))
						{
							for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
							{
								if(pkEventChoiceInfo->getFlavorValue(iFlavor) > 0)
								{
									int iOurFlavor = GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavor);
									iOurFlavor += pkEventChoiceInfo->getFlavorValue(iFlavor);
									flavorChoices.push_back(eEventChoice, iOurFlavor);
								}
							}
						}
					}
				}
			}
			if(flavorChoices.size() > 0)
			{
				//sort em!
				flavorChoices.SortItems();
				
				//And grab the top selection.
				EventChoiceTypes eBestEventChoice = (EventChoiceTypes)flavorChoices.GetElement(0);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvModEventChoiceInfo* pkEventChoiceInfo = GC.getEventChoiceInfo(eBestEventChoice);
					if(pkEventChoiceInfo != NULL)
					{
						CvString playerName;
						FILogFile* pLog;
						CvString strBaseString;
						CvString strOutBuf;
						CvString strFileName = "EventLogging.csv";
						playerName = getCivilizationShortDescription();
						pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
						strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
						strBaseString += playerName + ", ";
						strOutBuf.Format("AI made a flavor event choice: %s", pkEventChoiceInfo->GetDescription());
						strBaseString += strOutBuf;
						pLog->Msg(strBaseString);
					}
				}

				//If didn't find something (probably because a modder forgot to set flavors...), do a random selection.
				if(eBestEventChoice != NO_EVENT_CHOICE)
				{
					DoEventChoice(eBestEventChoice);
					return;
				}
			}
			//If we got here, it is because we haven't made a choice yet. Do so now.
			CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> randomChoices;
			for(int iLoop = 0; iLoop < GC.getNumEventChoiceInfos(); iLoop++)
			{
				EventChoiceTypes eEventChoice = (EventChoiceTypes)iLoop;
				if(eEventChoice != NO_EVENT_CHOICE)
				{
					CvModEventChoiceInfo* pkEventChoiceInfo = GC.getEventChoiceInfo(eEventChoice);
					if(pkEventChoiceInfo != NULL)
					{
						if(IsEventChoiceValid(eEventChoice, eChosenEvent))
						{
							int iRandom = GC.getGame().getJonRandNum(pkEventInfo->getNumChoices(), "Random Event Choice");
							if(iRandom <= 0)
							{
								iRandom = 1;
							}
							randomChoices.push_back(eEventChoice, iRandom);
						}
					}
				}
			}
			randomChoices.SortItems();
				
			//And grab the top selection.
			EventChoiceTypes eBestEventChoice = (EventChoiceTypes)randomChoices.GetElement(0);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvModEventChoiceInfo* pkEventChoiceInfo = GC.getEventChoiceInfo(eBestEventChoice);
				if(pkEventChoiceInfo != NULL)
				{
					CvString playerName;
					FILogFile* pLog;
					CvString strBaseString;
					CvString strOutBuf;
					CvString strFileName = "EventLogging.csv";
					playerName = getCivilizationShortDescription();
					pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
					strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
					strBaseString += playerName + ", ";
					strOutBuf.Format("AI made a random event choice: %s", pkEventChoiceInfo->GetDescription());
					strBaseString += strOutBuf;
					pLog->Msg(strBaseString);
				}
			}

			//If didn't find something (probably because a modder forgot to set flavors...), do a random selection.
			if(eBestEventChoice != NO_EVENT_CHOICE)
			{
				DoEventChoice(eBestEventChoice);
				return;
			}
		}
	}
}
#endif
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
OperationSlot CvPlayerAI::PeekAtNextUnitToBuildForOperationSlot(CvCity* pCity, bool& bCitySameAsMuster)
{
	OperationSlot thisSlot;
	OperationSlot bestSlot;
	// search through our operations till we find one that needs a unit
	std::map<int, CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
#if defined(MOD_BALANCE_CORE)
			CvPlot *pMusterPlot = pThisOperation->GetMusterPlot();

			if (!pMusterPlot)
				continue;

			if (pThisOperation->IsNavalOperation() && !pCity->isMatchingArea(pMusterPlot))
			{
					continue;
			}				
#endif
			thisSlot = pThisOperation->PeekAtNextUnitToBuild();
			
			if (thisSlot.m_iOperationID == -1)
				continue;

			if (thisSlot.IsValid() && OperationalAIHelpers::IsSlotRequired(GetID(), thisSlot))
			{
				bestSlot = thisSlot;
			}

			if (pCity == pMusterPlot->getWorkingCity() && bestSlot == thisSlot)
			{
				bCitySameAsMuster = true;
				break;
			}
		}
	}

	return bestSlot;
}


OperationSlot CvPlayerAI::CityCommitToBuildUnitForOperationSlot(int iAreaID, int iTurns, CvCity* pCity)
{
	OperationSlot thisSlot;
#if defined(MOD_BALANCE_CORE)
	CvAIOperation* pBestOperation = NULL;
	int iBestScore = -1;
#endif
	// search through our operations till we find one that needs a unit
	std::map<int, CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
			if(iAreaID == -1)
			{
				iAreaID = pCity->getArea();
			}

			int iScore = 0;
			int iArea = -1;
			int iDistance = 0;
			if(pThisOperation->GetMusterPlot() != NULL)
			{
				iArea = pThisOperation->GetMusterPlot()->getArea();
				iDistance = plotDistance(*pThisOperation->GetMusterPlot(),*pCity->plot());
			}
			else
				continue;

			//for naval ops, check if we're on the correct water body
			if(pThisOperation->IsNavalOperation()) 
			{
				if(!pCity->isAdjacentToArea(iArea))
				{
					continue;
				}
			}

			iScore += pThisOperation->GetNumUnitsNeededToBeBuilt();
			iScore += range( 10-iDistance, 0, 10 );

			if(pThisOperation->GetOperationType() == AI_OPERATION_CITY_SNEAK_ATTACK || pThisOperation->GetOperationType() == AI_OPERATION_NAVAL_INVASION_SNEAKY)
			{
				iScore *= 2;
			}

			if(iScore > iBestScore)
			{
				pBestOperation = pThisOperation;
				iBestScore = iScore;
			}
		}
	}
	if(pBestOperation != NULL)
	{
		thisSlot = pBestOperation->CommitToBuildNextUnit(iAreaID, iTurns, pCity);
		if(thisSlot.IsValid())
		{
			return thisSlot;
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
#if defined(MOD_BALANCE_CORE)
		if(pLoopUnit->IsCityAttackSupport())
		{
			pLoopUnit->SetGreatPeopleDirective(GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND);
			continue;
		}
		else if (pLoopUnit->IsCombatUnit() && pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_ENGINEER) && !IsAtWar())
		{
			pLoopUnit->SetGreatPeopleDirective(GREAT_PEOPLE_DIRECTIVE_USE_POWER);
			continue;
		}
		else
#endif
		if(pLoopUnit->getSpecialUnitType() != eSpecialUnitGreatPerson || pLoopUnit->getArmyID()!=-1)
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
#if defined(MOD_BALANCE_CORE)
	// Create Great Work if there is a slot
	GreatWorkType eGreatWork = pGreatWriter->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatWriter->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
#endif
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
#if !defined(MOD_BALANCE_CORE)
	// Create Great Work if there is a slot
	GreatWorkType eGreatWork = pGreatWriter->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatWriter->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
#endif

#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	else if (!MOD_AI_SMART_GREAT_PEOPLE || (GC.getGame().getGameTurn() - pGreatWriter->getGameTurnCreated()) >= (GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() / 2))
#else
	else
#endif
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST;
	}
	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveArtist(CvUnit* pGreatArtist)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
#if defined(MOD_BALANCE_CORE)
	// Create Great Work if there is a slot
	GreatWorkType eGreatWork = pGreatArtist->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatArtist->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
	// If finishing up spaceship parts, Golden Age will help build those quickly
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetDiplomacyAI()->IsGoingForSpaceshipVictory() && EconomicAIHelpers::IsTestStrategy_GS_SpaceshipHomestretch(this))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetPlayerTraits()->GetGoldenAgeTourismModifier() > 0 && GetCulture()->GetNumCivsInfluentialOn() > 0)
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}
	// If Persia and I'm at war, get a Golden Age going
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetPlayerTraits()->GetGoldenAgeMoveChange() > 0 && GetMilitaryAI()->GetNumberCivsAtWarWith() > 0 && !isGoldenAge())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}
	if ((GC.getGame().getGameTurn() - pGreatArtist->getGameTurnCreated()) >= (GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() / 2))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}
#else
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
#endif
	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveMusician(CvUnit* pGreatMusician)
{
	// If headed on a concert tour, keep going
	if (pGreatMusician->getArmyID() != -1)
	{
		return GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;
	}

	CvPlot* pTarget = FindBestMusicianTargetPlot(pGreatMusician);

	// If closing in on a Culture win, go for the Concert Tour
	if (GetDiplomacyAI()->IsGoingForCultureVictory() && GetCulture()->GetNumCivsInfluentialOn() > (GC.getGame().GetGameCulture()->GetNumCivsInfluentialForWin() / 2))
	{
		if(pTarget)
		{
#if defined(MOD_BALANCE_CORE)
			if(pTarget->getOwner() != NO_PLAYER && GET_PLAYER(pTarget->getOwner()).isMajorCiv())
			{
				if(GetCulture()->GetTurnsToInfluential(pTarget->getOwner()) <= 100)
				{
#endif
			return GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;
#if defined(MOD_BALANCE_CORE)
				}
			}
#endif
		}
	}

	// Create Great Work if there is a slot
	GreatWorkType eGreatWork = pGreatMusician->GetGreatWork();
	if (GetEconomicAI()->GetBestGreatWorkCity(pGreatMusician->plot(), eGreatWork))
	{
		return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	if ((GC.getGame().getGameTurn() - pGreatMusician->getGameTurnCreated()) >= (GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() / 2))
	{
		if(pTarget)
		{
			return GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;
		}
	}

	return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveEngineer(CvUnit* pGreatEngineer)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
	
	// look for a wonder to rush
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		int iNextWonderWeight;
		BuildingTypes eNextWonderDesired = GetWonderProductionAI()->ChooseWonder(false /*bAdjustForOtherPlayers*/, iNextWonderWeight);
		if(eNextWonderDesired != NO_BUILDING)
		{
#if defined(MOD_BALANCE_CORE)
			//Let's make sure the wonder is worth it. Need scales with era. This should, hopefully, discount the value of wonders by the mid-game.
			CvBuildingEntry* thisBuildingEntry = GC.getBuildingInfo(eNextWonderDesired);
			if(thisBuildingEntry)
			{
				const CvBuildingClassInfo& kBuildingClassInfo = thisBuildingEntry->GetBuildingClassInfo();
				//Is this actually a world wonder?
				if(kBuildingClassInfo.getMaxGlobalInstances() != -1)
				{
					if(GetWonderProductionAI()->GetWeight(eNextWonderDesired) > (10000 * GetCurrentEra() + 1))
					{
#endif
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
#if defined(MOD_BALANCE_CORE)
					}
				}
			}
#endif
		}
	}
#if defined(MOD_BALANCE_CORE)
	ImprovementTypes eManufactory = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_MANUFACTORY");
	int iFlavor =  GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION"));
	iFlavor += GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
	iFlavor += (GetPlayerTraits()->GetWLTKDGPImprovementModifier() / 5);
	iFlavor -= (GetCurrentEra() + GetNumUnitsWithUnitAI(UNITAI_ENGINEER));
	// Build manufactories up to your flavor.
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		int iNumImprovement = getImprovementCount(eManufactory);
		if(iNumImprovement <= iFlavor)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) >= (GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() - GetCurrentEra()))
#else
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 3) / 4))
	{
		if (GetDiplomacyAI()->IsGoingForWorldConquest())
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
#endif
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveMerchant(CvUnit* pGreatMerchant)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	bool bTheVeniceException = false;
	if (GetPlayerTraits()->IsNoAnnexing() && !GreatMerchantWantsCash())
	{
		bTheVeniceException = true;
	}

	// if the merchant is in an army, he's already marching to a destination, so don't evaluate him
	if(pGreatMerchant->getArmyID() != -1)
	{
		return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
	}
#if defined(MOD_BALANCE_CORE)
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && ((GC.getGame().getGameTurn() - pGreatMerchant->getGameTurnCreated()) >= (GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() - GetCurrentEra())))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}
#endif
#if defined(MOD_BALANCE_CORE)
	ImprovementTypes eCustomHouse = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CUSTOMS_HOUSE");
	int iFlavor =  GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
	iFlavor += GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
	iFlavor += (GetPlayerTraits()->GetWLTKDGPImprovementModifier() / 5);
	iFlavor -= (GetCurrentEra() + GetNumUnitsWithUnitAI(UNITAI_MERCHANT));
	// build custom houses up to your flavor.
	bool bConstructImprovement = !bTheVeniceException;
	if(bConstructImprovement && eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		int iNumImprovement = getImprovementCount(eCustomHouse);
		if(iNumImprovement <= iFlavor)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}
#else
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
#endif
	// Attempt a run to a minor civ
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && IsSafe(this))
	{
		CvPlot* pTarget = FindBestMerchantTargetPlot(pGreatMerchant);
		if(pTarget)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
#if defined(MOD_BALANCE_CORE)
		else if(bTheVeniceException)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
#endif
	}

#if defined(MOD_BALANCE_CORE)
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && ((GC.getGame().getGameTurn() - pGreatMerchant->getGameTurnCreated()) >= (GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() - GetCurrentEra())) && !bTheVeniceException)
#else
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatMerchant->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() && !bTheVeniceException)
#endif
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
#if defined(MOD_BALANCE_CORE)
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		ImprovementTypes eAcademy = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_ACADEMY");
		int iFlavor =  GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
		iFlavor += (GetPlayerTraits()->GetWLTKDGPImprovementModifier() / 5);
		iFlavor += GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
		//This is to prevent a buildup of scientists if the AI is having a hard time planting them.
		iFlavor -= (GetCurrentEra() + GetNumUnitsWithUnitAI(UNITAI_SCIENTIST));
		// Even if not going spaceship right now, build academies up to your flavor.
		int iNumImprovement = getImprovementCount(eAcademy);
		if(iNumImprovement <= iFlavor)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}
#else

#if defined(MOD_AI_SMART_GREAT_PEOPLE)
	//First half. Simple.
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 1) / 2))
#else
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 1) / 4))
#endif
	{
		if(GetDiplomacyAI()->IsGoingForSpaceshipVictory())
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}
#endif
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		// a tech boost is never bad
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveGeneral(CvUnit* pGreatGeneral)
{
	//if he has a directive, don't change it
	if (pGreatGeneral->GetGreatPeopleDirective() != NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
		return pGreatGeneral->GetGreatPeopleDirective();

	bool bWar = (GetMilitaryAI()->GetNumberCivsAtWarWith(false) > 0);

	//in army or recently out of an army?
	if (pGreatGeneral->getArmyID() != -1 || pGreatGeneral->IsRecentlyDeployedFromOperation() || pGreatGeneral->GetDanger(pGreatGeneral->plot()) > 0)
	{
		return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
	}

	int iFriendlies = 0;
	if (bWar && (pGreatGeneral->plot()->getNumDefenders(GetID()) > 0))
	{
		iFriendlies++;
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot *pLoopPlot = plotDirection(pGreatGeneral->plot()->getX(), pGreatGeneral->plot()->getY(), ((DirectionTypes)iI));
			if (pLoopPlot != NULL && pLoopPlot->getNumUnits() > 0)
			{
				CvUnit* pUnit = pLoopPlot->getUnitByIndex(0);
				if (pUnit != NULL && pUnit->getOwner() == pGreatGeneral->getOwner() && pUnit->IsCombatUnit() && pUnit->getDomainType() == DOMAIN_LAND)
				{
					iFriendlies++;
				}
			}
		}
	}
	if (iFriendlies > 2)
	{
		return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
	}

	int iGreatGeneralCount = 0;
	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
		if(pLoopUnit->IsGreatGeneral())
			iGreatGeneralCount++;

	int iDummy = 0;
	std::vector<CvPlot*> vDummy;
	CvPlot* pTargetPlot = FindBestGreatGeneralTargetPlot(pGreatGeneral, vDummy, iDummy);
	//keep at least one general around
	if(iGreatGeneralCount > 1 && pTargetPlot && (pGreatGeneral->getArmyID() == -1))
	{
		//build a citadel
		return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
	
	return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveProphet(CvUnit* pUnit)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	ReligionTypes eReligion = GetReligions()->GetReligionCreatedByPlayer();
	const CvReligion* pMyReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, GetID());

	// CASE 1: I have an enhanced religion. 
	if (pMyReligion && pMyReligion->m_bEnhanced)
	{
#if defined(MOD_BALANCE_CORE)
		ImprovementTypes eHolySite = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_HOLY_SITE");
		int iFlavor =  GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
		iFlavor -= GetNumUnitsWithUnitAI(UNITAI_PROPHET);
		//Let's use our prophets for improvments instead of wasting them on conversion.
		int iNumImprovement = getImprovementCount(eHolySite);
		if(iNumImprovement <= iFlavor)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
		//Only convert once we've hit our peak.
		if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;
		}
#else
		// Spread religion if there is any city that needs it
		if (GetReligionAI()->ChooseProphetConversionCity(false/*bOnlyBetterThanEnhancingReligion*/))
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;
		}
		else
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
#endif
	}


	// CASE 2: I have a religion that hasn't yet been enhanced
	else if (pMyReligion)
	{
		//always enhance
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		// Spread religion if there is a city that needs it CRITICALLY
		if (GetReligionAI()->ChooseProphetConversionCity(true/*bOnlyBetterThanEnhancingReligion*/,pUnit))
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
#if defined(MOD_BALANCE_CORE)
		if (GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() <= 0 && !GetPlayerTraits()->IsAlwaysReligion())
#else
		if (GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() <= 0)
#endif
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}

		// Not locked out
		else
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if ((GC.getGame().getGameTurn() - pUnit->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;
	}

	return eDirective;
}

#if defined(MOD_BALANCE_CORE_MILITARY)
GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveAdmiral(CvUnit* pGreatAdmiral)
{
	//if he has a directive, don't change it
	if (pGreatAdmiral->GetGreatPeopleDirective()!=NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
		return pGreatAdmiral->GetGreatPeopleDirective();

	bool bWar = (GetMilitaryAI()->GetNumberCivsAtWarWith(false)>0);

	if(pGreatAdmiral->getArmyID() != -1 || pGreatAdmiral->IsRecentlyDeployedFromOperation() || pGreatAdmiral->GetDanger(pGreatAdmiral->plot()) > 0)
		return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;

	int iFriendlies = 0;
	if(bWar && (pGreatAdmiral->plot()->getNumDefenders(GetID()) > 0))
	{
		iFriendlies++;
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
		{
			CvPlot *pLoopPlot = plotDirection(pGreatAdmiral->plot()->getX(), pGreatAdmiral->plot()->getY(), ((DirectionTypes)iI));
			if (pLoopPlot != NULL && pLoopPlot->getNumUnits() > 0)
			{
				CvUnit* pUnit = pLoopPlot->getUnitByIndex(0);
				if(pUnit != NULL && pUnit->getOwner() == pGreatAdmiral->getOwner() && pUnit->IsCombatUnit() && pUnit->getDomainType() == DOMAIN_SEA)
				{
					iFriendlies++;
				}
			}
		}
	}
	if(iFriendlies > 2)
	{
		return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
	}
	//No war? Let's settle down.
	int iGreatAdmiralCount = 0;

	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if(pLoopUnit->IsGreatAdmiral())
		{
			iGreatAdmiralCount++;
		}
	}
	if(iGreatAdmiralCount > 1 && pGreatAdmiral->canGetFreeLuxury())
	{
		return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
	else if (iGreatAdmiralCount > 0 && IsEmpireUnhappy() && pGreatAdmiral->canGetFreeLuxury())
	{
		return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
}

#else

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveAdmiral(CvUnit* /*pGreatAdmiral*/)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	return eDirective;
}
#endif

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
	
	int iFlavorDiplo =  GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
	int iDesiredEmb = (iFlavorDiplo - 1);
	int iNumMinors = GC.getGame().GetNumMinorCivsAlive();
	if(iDesiredEmb > iNumMinors)
	{
		iDesiredEmb = iNumMinors;
	}
	int iEmbassies = GetImprovementLeagueVotes();

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

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		if(pCity == NULL || (iEmbassies >= iDesiredEmb) || bTheAustriaException || bTheVeniceException)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if ((GC.getGame().getGameTurn() - pGreatDiplomat->getGameTurnCreated()) >= GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
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

CvPlot* CvPlayerAI::FindBestMerchantTargetPlot(CvUnit* pMerchant)
{
	if(!pMerchant)
		return NULL;

	int iBestTurnsToReach = MAX_INT;
	CvPlot* pBestTargetPlot = NULL;
	int iPathTurns;
	CvTeam& myTeam = GET_TEAM(getTeam());

	bool bIsVenice = GetPlayerTraits()->IsNoAnnexing();
	bool bWantsCash = GreatMerchantWantsCash();

	// Loop through each city state
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kPlayer.isMinorCiv() || !kPlayer.isAlive())
			continue;

		/*
		// if I'm Venice, I don't want to send a Merchant of Venice to a buy a city that I have trade routes 
		// with because it's probably more valuable as a trade partner than as an owned entity
		if (!bWantsCash)
		{
			if (bIsVenice)
			{
				if (GetTrade()->IsConnectedToPlayer(kPlayer.GetID()))
				{
					continue;
				}
			}
		}
		*/

		CvCity* pCity = kPlayer.getCapitalCity();
		if (!pCity || !pCity->plot()->isRevealed(getTeam()))
			continue;

		// Is this a minor we are friendly with?
		bool bMinorCivApproachIsCorrect = (GetDiplomacyAI()->GetMinorCivApproach(kPlayer.GetID()) != MINOR_CIV_APPROACH_CONQUEST);
		bool bAtPeace = !myTeam.isAtWar(kPlayer.getTeam());
		bool bNotPlanningAWar = GetDiplomacyAI()->GetWarGoal(kPlayer.GetID()) == NO_WAR_GOAL_TYPE;
#if defined(MOD_BALANCE_CORE)
		int iNumUnits = 0;
		if(bIsVenice && !bWantsCash)
		{
			iNumUnits = (kPlayer.getNumMilitaryUnits() / 2);
		}
#endif
		if(bMinorCivApproachIsCorrect && bAtPeace && bNotPlanningAWar)
		{
			// Search all the plots adjacent to this city (since can't enter the minor city plot itself)
			for(int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
			{
				CvPlot* pAdjacentPlot = plotDirection(pCity->plot()->getX(), pCity->plot()->getY(), ((DirectionTypes)jJ));
				if(pAdjacentPlot != NULL)
				{
					if(!pAdjacentPlot->isValidMovePlot(GetID()))
						continue;
					if(!pMerchant->canTrade(pAdjacentPlot))
						continue;

					// Make sure this is still owned by the city state and is revealed to us and isn't a water tile
					bool bRightOwner = (pAdjacentPlot->getOwner() == (PlayerTypes)iI);
					bool bIsRevealed = pAdjacentPlot->isRevealed(getTeam());
					if(bRightOwner && bIsRevealed)
					{
						iPathTurns = pMerchant->TurnsToReachTarget(pAdjacentPlot, false, false, iBestTurnsToReach);
#if defined(MOD_BALANCE_CORE)
						if(bIsVenice)
						{
							iPathTurns -= iNumUnits;
						}
#endif
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
CvCity* CvPlayerAI::FindBestDiplomatTargetCity(CvUnit* pUnit)
{
	CvWeightedVector<CvCity *, SAFE_ESTIMATE_NUM_CITIES, true> vTargets;

	// Loop through each city state
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && kPlayer.isMinorCiv())
		{
		//Loop through each city
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if(pLoopCity)
				{
					int iScore = ScoreCityForDiplomat(pLoopCity, pUnit);
					if(iScore > 0)
					{
						vTargets.push_back( pLoopCity,iScore);
					}
				}
			}
		}
	}

	//highest score first ..
	if(vTargets.size() > 0)
	{
		vTargets.SortItems();

		//check if we can actually go there only if the city is promising
		for (int i = 0; i < vTargets.size(); ++i )
		{
			CvCity* pCity = vTargets.GetElement(i);
			if(pCity != NULL)
			{
				if (pUnit->GeneratePath(pCity->plot(), CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1))
					return pCity;
			}
		}
	}

	return NULL;
}

CvCity* CvPlayerAI::FindBestMessengerTargetCity(CvUnit* pUnit)
{
	CvWeightedVector<CvCity *, SAFE_ESTIMATE_NUM_CITIES, true> vTargets;

	// Loop through each city state
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && kPlayer.isMinorCiv() && !GET_TEAM(kPlayer.getTeam()).isAtWar(getTeam()))
		{
			//Loop through each city
			int iLoop;
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				//want to have at least on revealed plot as target for pathfinding
				if(pLoopCity && pLoopCity->plot()->isAdjacentRevealed(pUnit->getTeam()))
				{
					int iScore = ScoreCityForMessenger(pLoopCity, pUnit);
					if(iScore > 0)
					{
						vTargets.push_back( pLoopCity,iScore);
					}
				}
			}
		}
	}

	//highest score first ..
	if(vTargets.size() > 0)
	{
		vTargets.SortItems();

		//check if we can actually go there only if the city is promising
		for (int i = 0; i < vTargets.size(); ++i )
		{
			CvCity* pCity = vTargets.GetElement(i);
			if(pCity != NULL)
			{
				if (pUnit->GeneratePath(pCity->plot(), CvUnit::MOVEFLAG_TERRITORY_NO_ENEMY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1))
					return pCity;
			}
		}
	}

	return NULL;
}	

int CvPlayerAI::ScoreCityForDiplomat(CvCity* pCity, CvUnit* pUnit)
{
	int iScore = 0;
	int iI;
	CvPlayer& kCityPlayer = GET_PLAYER(pCity->getOwner());
	CvPlayer& eMinor = GET_PLAYER(pCity->getOwner());

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

	if(eMinor.GetMinorCivAI()->IsActiveQuestForPlayer(GetID(), MINOR_CIV_QUEST_HORDE) || eMinor.GetMinorCivAI()->IsActiveQuestForPlayer(GetID(), MINOR_CIV_QUEST_REBELLION))
	{
		return 0;
	}

	//Return score if we can't embark and they aren't on our landmass.
	if(pCity->getArea() != pUnit->plot()->getArea())
	{
		if(!GET_TEAM(getTeam()).canEmbark())
		{
			return iScore;
		}
	}

	// Do we already have an embassy here?
	// To iterate all plots owned by a CS, wrap this is a loop that iterates all cities owned by the CS
	// Iterate all plots owned by a city

	for(iI = 0; iI < pCity->GetNumWorkablePlots(); iI++)
	{
		CvPlot* pCityPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);

		if(pCityPlot != NULL && pCityPlot->getOwner() == pCity->getOwner())
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

	iScore = 500;

	// Subtract distance (XML value important here!)
	int iDistance = (plotDistance(pUnit->getX(), pUnit->getY(), pCity->getX(), pCity->getY()) * GC.getINFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE());

	//Are there barbarians near the city-state? If so, careful!
	if(eMinor.GetMinorCivAI()->IsThreateningBarbariansEventActiveForPlayer(GetID()))
	{
		iDistance *= 2;
	}

	//Let's downplay minors we can't walk to if we don't have embarkation.
	if((pCity->getArea() != pUnit->getArea()) && !GET_TEAM(GET_PLAYER(GetID()).getTeam()).canEmbark())
	{
		iDistance *= 2;
	}

	//Let's downplay far/distant minors without full embarkation.
	else if((pCity->getArea() != pUnit->getArea()) && !GET_PLAYER(GetID()).CanCrossOcean())
	{
		iDistance *= 4;
	}

	//If this is way too far away, let's not penalize it too much.
	iScore -= iDistance;

	//All CSs should theoretically be valuable if we've gotten this far.
	if(iScore <= 0)
	{
		iScore = 1;
	}

	return iScore;
}
	
int CvPlayerAI::ScoreCityForMessenger(CvCity* pCity, CvUnit* pUnit)
{
	//First, the exclusions!

	//Initializations...
	int iScore = 0;
	CvPlot* pPlot = pCity->plot();
	int iFriendship = pUnit->getTradeInfluence(pPlot);

	CvMinorCivAI* pMinorCivAI;

	CvPlayer& eMinor = GET_PLAYER(pCity->getOwner());
	PlayerTypes pMinor = eMinor.GetID();
	pMinorCivAI = eMinor.GetMinorCivAI();

	// Skip if not revealed.
	if (!pCity->plot()->isRevealed(getTeam()))
	{
		return 0;
	}

	if (pMinorCivAI->GetPermanentAlly() == GetID())
	{
		return 0;
	}
	if (pMinorCivAI->GetPermanentAlly() != GetID() && pMinorCivAI->GetPermanentAlly() != NO_PLAYER)
	{
		return 0;
	}
	if (pMinorCivAI->IsNoAlly())
		return 0;

	if (pMinorCivAI->IsAllies(GetID()) && !pMinorCivAI->IsCloseToNotBeingAllies(GetID()))
		return 0;

	//If we are at war with target minor, let's not send diplomatic lambs to slaughter.
	if (eMinor.GetMinorCivAI()->IsAtWarWithPlayersTeam(GetID()))
	{
		return 0;
	}

	if (eMinor.GetMinorCivAI()->IsActiveQuestForPlayer(GetID(), MINOR_CIV_QUEST_HORDE) || eMinor.GetMinorCivAI()->IsActiveQuestForPlayer(GetID(), MINOR_CIV_QUEST_REBELLION))
	{
		return 0;
	}

	// Did we bully you recently?  If so, being friendly now would be very odd.
	if (pMinorCivAI->IsRecentlyBulliedByMajor(GetID()))
	{
		return 0;
	}

	//Return score if we can't embark and they aren't on our landmass.
	if (pCity->getArea() != pUnit->plot()->getArea())
	{
		if (!GET_TEAM(getTeam()).canEmbark())
		{
			return 0;
		}
	}

	//are we at war with a player close to this CS? Don't go near here!
	for (int iNeighborMajorLoop = 0; iNeighborMajorLoop < MAX_MAJOR_CIVS; iNeighborMajorLoop++)
	{
		PlayerTypes eOtherMajor = (PlayerTypes)iNeighborMajorLoop;

		if (GET_PLAYER(eOtherMajor).isAlive() && GET_TEAM(GET_PLAYER(eOtherMajor).getTeam()).isAtWar(getTeam()) && GET_PLAYER(eOtherMajor).GetProximityToPlayer(pMinor) >= PLAYER_PROXIMITY_CLOSE)
		{
			return 0;
		}
	}

	int iOtherMajorLoop;
	PlayerTypes eOtherMajor;
	int iFriendshipWithMinor;
	int iOtherPlayerFriendshipWithMinor;

	EconomicAIStrategyTypes eNeedHappiness = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS");
	EconomicAIStrategyTypes eNeedHappinessCritical = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL");
	bool bNeedHappiness = (eNeedHappiness != NO_ECONOMICAISTRATEGY) ? GetEconomicAI()->IsUsingStrategy(eNeedHappiness) : false;
	bool bNeedHappinessCritical = (eNeedHappinessCritical != NO_ECONOMICAISTRATEGY) ? GetEconomicAI()->IsUsingStrategy(eNeedHappinessCritical) : false;

	MinorCivApproachTypes eApproach;

	// **************************
	// Approaches
	// **************************

	iScore = 100;
	eApproach = GetDiplomacyAI()->GetMinorCivApproach(pMinor);

	if (eApproach == MINOR_CIV_APPROACH_IGNORE)
	{
		iScore /= 10;
	}

	// **************************
	// Benefits to Us!
	// **************************

	//DIPLOMACY - We want all of them the same!
	if (GetDiplomacyAI()->IsGoingForDiploVictory())
	{
		iScore *= 2;
	}

	//MILITARY - We want units and happiness!!
	else if (GetDiplomacyAI()->IsGoingForWorldConquest())
	{
		if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
		{
			iScore *= 2;
		}
	}

	//SCIENCE - We want happiness and growth!!
	else if (GetDiplomacyAI()->IsGoingForSpaceshipVictory())
	{
		if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME)
		{
			iScore *= 2;
		}
		if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
		{
			iScore *= 2;
		}
	}

	//CULTURE - We want culture and religion!!
	else if (GetDiplomacyAI()->IsGoingForCultureVictory())
	{
		if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_CULTURED)
		{
			iScore *= 2;
		}
		if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS)
		{
			iScore *= 2;
		}
	}

	// Is Our Influence worth more here? Definitely take advantage of this.
	if (pMinorCivAI->IsActiveQuestForPlayer(GetID(), MINOR_CIV_QUEST_INFLUENCE))
	{
		iScore *= 5;
	}

	// Do they have a resource we lack?
	int iResourcesWeLack = pMinorCivAI->GetNumResourcesMajorLacks(GetID());
	if (iResourcesWeLack > 0)
	{
		if (bNeedHappiness)
		{
			iScore *= 3;
		}
		else if (bNeedHappinessCritical)
		{
			iScore *= 4;
		}
		else
		{
			iScore *= 2;
		}
	}

	//Will they give us a WLTKD for their resource?
	CvCity* pLoopCity;
	int iCityLoop;
	for (pLoopCity = GET_PLAYER(GetID()).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(GetID()).nextCity(&iCityLoop))
	{
		if (pLoopCity != NULL)
		{
			ResourceTypes eResourceDemanded = pLoopCity->GetResourceDemanded();
			if (eResourceDemanded != NO_RESOURCE)
			{
				//Will we get a WLTKD from this? We want it a bit more, please.
				if (eMinor.getResourceInOwnedPlots(eResourceDemanded) > 0)
				{
					iScore *= 3;
					iScore /= 2;
				}
			}
		}
	}

	//Nobody likes hostile city-states.
	if (pMinorCivAI->GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
	{
		iScore /= 5;
	}
	//If our friendship is under 0, we've probably done something bad to this City-State. Let's not look at them!
	if (eMinor.GetMinorCivAI()->GetBaseFriendshipWithMajor(GetID()) < 0)
	{
		iScore /= 5;
	}

	// **************************
	// Other Relationships!
	// **************************

	PlayerTypes eAlliedPlayer = NO_PLAYER;
	iFriendshipWithMinor = pMinorCivAI->GetBaseFriendshipWithMajor(GetID());
	eAlliedPlayer = pMinorCivAI->GetAlly();

	int iHighestInfluence = 0;
	// Loop through other players to see if we can pass them in influence
	for (iOtherMajorLoop = 0; iOtherMajorLoop < MAX_MAJOR_CIVS; iOtherMajorLoop++)
	{
		eOtherMajor = (PlayerTypes)iOtherMajorLoop;

		if (eOtherMajor == NO_PLAYER || eOtherMajor == GetID())
			continue;

		iOtherPlayerFriendshipWithMinor = pMinorCivAI->GetBaseFriendshipWithMajor(eOtherMajor);
		if (iOtherPlayerFriendshipWithMinor > iHighestInfluence)
		{
			iHighestInfluence = iOtherPlayerFriendshipWithMinor;
		}
	}
	//What would be the difference her after we do a mission?
	int iDifference = iHighestInfluence - (iFriendshipWithMinor + iFriendship);

	// Are we allied? Yay! But let's be careful.
	if (eAlliedPlayer == GetID())
	{
		// Are WE allies by a wide margin (over 100)? If so, let's find someone new to love.
		if (iDifference < iFriendship * -2)
		{
			return 1;
		}
		// Are we close to losing our status? If so, obsess away!
		else if (iDifference < iFriendship * -1 || pMinorCivAI->IsCloseToNotBeingAllies(GetID()))
		{
			iScore *= 2;
		}
	}
	else if (eAlliedPlayer != NO_PLAYER)
	{
		//If their influence is way higher than ours, let's tune this down...
		if (iDifference > iFriendship * 2)
		{
			return 1;
		}
		else if (iDifference > iFriendship)
		{
			iScore /= 5;
		}
		//If we can pass them, ramp it up!
		else if (iDifference <= (iFriendship * -1))
		{
			iScore *= 4;
		}
		else if (iDifference <= 0)
		{
			iScore *= 2;
		}

		if (GET_TEAM(GET_PLAYER(GetID()).getTeam()).isHasMet(GET_PLAYER(eAlliedPlayer).getTeam()))
		{
			MajorCivApproachTypes eApproachType = GetDiplomacyAI()->GetMajorCivApproach(eAlliedPlayer, false);
			MajorCivOpinionTypes eOpinion = GetDiplomacyAI()->GetMajorCivOpinion(eAlliedPlayer);

			// If a friendly player is allied, let's discourage going there.
			if (eApproachType == MAJOR_CIV_APPROACH_FRIENDLY)
			{
				iScore /= 10;
			}
			// If an enemy is allied, let's take their stuff!
			else if (eApproachType == MAJOR_CIV_APPROACH_HOSTILE)
			{
				iScore *= 2;
			}
			// If an ally is allied, let's leave it alone!
			if (eOpinion == MAJOR_CIV_OPINION_ALLY)
			{
				iScore /= 10;
			}
			// If an competitor is allied, let's fight for it!
			else if (eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
			{
				iScore *= 2;
			}
		}
	}
	else
	{
		// Are we close to becoming an normal (60) ally and no one else ? If so, obsess away!
#if defined(MOD_CITY_STATE_SCALE)
		if((iFriendshipWithMinor + iFriendship) >= pMinorCivAI->GetAlliesThreshold(GetID()))
#else
		if((iFriendshipWithMinor + iFriendship) >= pMinorCivAI->GetAlliesThreshold())
#endif
		{
			iScore *= 10;
		}

		// Are we already Friends? If so, let's stay the course.
		else if(pMinorCivAI->IsFriends(GetID()))
		{
			iScore *= 2;
		}
		//No friends, and no points? Let's not completely ignore this.
		else if (iDifference != 0)
		{
			iScore /= 5;
		}
		//Not friends, and no one else is even interested? We should be!
		else
		{
			iScore *= 2;
		}
	}

	// **************************
	// Distance
	// **************************

	// Subtract distance (XML value important here!)
	int iDistance = (plotDistance(pUnit->getX(), pUnit->getY(), pCity->getX(), pCity->getY()) * GC.getINFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE());

	//Are there barbarians near the city-state? If so, careful!
	if(eMinor.GetMinorCivAI()->IsThreateningBarbariansEventActiveForPlayer(GetID()))
	{
		iDistance *= 3;
	}

	//Let's downplay minors we can't walk to if we don't have embarkation.
	if((pCity->getArea() != pUnit->getArea()) && !GET_TEAM(GET_PLAYER(GetID()).getTeam()).canEmbark())
	{
		iDistance *= 3;
	}

	//Let's downplay far/distant minors without full embarkation.
	else if((pCity->getArea() != pUnit->getArea()) && !GET_PLAYER(GetID()).CanCrossOcean())
	{
		iDistance *= 3;
	}

	//If this is way too far away, let's not penalize it too much.
	iScore -= iDistance;

	//All CSs should theoretically be valuable if we've gotten this far.
	if(iScore <= 0)
	{
		iScore = 1;
	}

	return iScore;
}

CvPlot* CvPlayerAI::ChooseDiplomatTargetPlot(CvUnit* pUnit)
{
	if(pUnit->AI_getUnitAIType() != UNITAI_DIPLOMAT)
	{
		return NULL;
	}

	CvCity* pCity = FindBestDiplomatTargetCity(pUnit);

	if(pCity == NULL)
		return NULL;

	int iBestDistance = MAX_INT;
	CvPlot* pBestTarget = NULL;

	// Find suitable adjacent plot
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iI));

		if(pLoopPlot != NULL)
		{
			CvUnit* pFirstUnit = pLoopPlot->getUnitByIndex(0);
			if(pFirstUnit && pFirstUnit->getOwner() != GetID())
			{
				continue;
			}
			if(pLoopPlot->isWater() || !pLoopPlot->isValidMovePlot(GetID(), false) || pLoopPlot->isMountain() || pLoopPlot->isIce())
			{
				continue;
			}
			if(pLoopPlot->getResourceType() != NO_RESOURCE)
			{
				continue;
			}
			// Make sure this is still owned by target and is revealed to us
			bool bRightOwner = (pLoopPlot->getOwner() == pCity->getOwner());
			bool bIsRevealed = pLoopPlot->isRevealed(getTeam());
			if(!bRightOwner || !bIsRevealed)
			{
				continue;
			}
			// Don't be captured
			if(pUnit->GetDanger(pLoopPlot)>0)
				continue;

			int	iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pLoopPlot->getX(), pLoopPlot->getY());

			if(iDistance < iBestDistance)
			{
				iBestDistance = iDistance;
				pBestTarget = pLoopPlot;
			}
		}
	}

	return pBestTarget;
}

CvPlot* CvPlayerAI::ChooseMessengerTargetPlot(CvUnit* pUnit)
{
	//this function is used for diplomat influence spread as well (embassies go through ChooseDiplomatTargetPlot)
	if(pUnit->AI_getUnitAIType() != UNITAI_MESSENGER && pUnit->AI_getUnitAIType() != UNITAI_DIPLOMAT)
	{
		return NULL;
	}
	CvCity* pCity = FindBestMessengerTargetCity(pUnit);
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
		if(pLoopPlot == NULL)
		{
			continue;
		}
		if(!pLoopPlot->isValidMovePlot(GetID(), !pUnit->isRivalTerritory()))
		{
			continue;
		}

#if defined(MOD_BALANCE_CORE)
		if(pUnit->GetDanger(pLoopPlot)>0)
			continue;
#endif

		// Make sure this is still owned by target and is revealed to us
		bool bRightOwner = (pLoopPlot->getOwner() == pCity->getOwner());
		bool bIsRevealed = pLoopPlot->isRevealed(getTeam());
		if(bRightOwner && bIsRevealed)
		{
			if(pLoopPlot->getNumUnits() <= 0)
			{
				pBestTarget = pLoopPlot;
				break;
			}
#if defined(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
			else if(MOD_GLOBAL_BREAK_CIVILIAN_1UPT)
			{
				pBestTarget = pLoopPlot;
				break;
			}
		}
#endif
	}

	return pBestTarget;
}
#endif

CvPlot* CvPlayerAI::FindBestMusicianTargetPlot(CvUnit* pMusician)
{
	if(!pMusician)
		return NULL;

	int iBestTurnsToReach = MAX_INT;
	CvCity* pBestTargetCity = NULL;

	// Find target civ
#if defined(MOD_BALANCE_CORE)
	PlayerTypes eTargetPlayer = NO_PLAYER;
	if(pMusician->isRivalTerritory())
	{
		eTargetPlayer = GetCulture()->GetCivLowestInfluence(false /*bCheckOpenBorders*/);
	}
	else
	{
#endif
		eTargetPlayer = GetCulture()->GetCivLowestInfluence(true /*bCheckOpenBorders*/);
#if defined(MOD_BALANCE_CORE)
	}
#endif
	if (eTargetPlayer == NO_PLAYER)
		return NULL;

	CvPlayer &kTargetPlayer = GET_PLAYER(eTargetPlayer);

	int iMoveFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;

	// Loop through each of that player's cities
	int iLoop;
	for(CvCity *pLoopCity = kTargetPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kTargetPlayer.nextCity(&iLoop))
	{
		int iPathTurns;
		if (pMusician->GeneratePath(pLoopCity->plot(),iMoveFlags,iBestTurnsToReach,&iPathTurns))
		{
			if(iPathTurns < iBestTurnsToReach)
			{
				iBestTurnsToReach = iPathTurns;
				pBestTargetCity = pLoopCity;
			}
		}
	}

	// Found a city now look at ALL the plots owned by that player near that city
	if (pBestTargetCity)
	{
		int iBestDistance = INT_MAX;
		CvPlot* pBestPlot = NULL;


		for(int iJ = 0; iJ < pBestTargetCity->GetNumWorkablePlots(); iJ++)
		{
			CvPlot *pLoopPlot = iterateRingPlots(pBestTargetCity->getX(), pBestTargetCity->getY(), iJ);
			if(pLoopPlot != NULL)
			{
				// Make sure this is still owned by target and is revealed to us
				bool bRightOwner = (pLoopPlot->getOwner() == eTargetPlayer);
				bool bIsRevealed = pLoopPlot->isRevealed(getTeam());
				if(bRightOwner && bIsRevealed)
				{
					int iDistance = ::plotDistance(pLoopPlot->getX(),pLoopPlot->getY(),pMusician->getX(),pMusician->getY());
					if(iDistance < iBestDistance)
					{
						iBestDistance = iDistance;
						pBestPlot = pLoopPlot;
					}
				}
			}	
		}

		return pBestPlot;
	}

	return NULL;
}

CvPlot* CvPlayerAI::FindBestGreatGeneralTargetPlot(CvUnit* pGeneral, const std::vector<CvPlot*>& vPlotsToAvoid, int& iResultScore)
{
	iResultScore = 0;
	if(!pGeneral)
		return NULL;

	// we may build in one of our border tiles or in enemy tiles adjacent to them
	std::set<int> setCandidates;
	
	BuildTypes eCitadel = (BuildTypes)GC.getInfoTypeForString("BUILD_CITADEL");
	CvBuildInfo* pkBuildInfoCitadel = GC.getBuildInfo(eCitadel);
	if(!pkBuildInfoCitadel)
	{
		return NULL;
	}

	// loop through plots and wipe out ones that are invalid
	for (set<int>::iterator it = m_aiPlots.begin(); it != m_aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);

		if(!pPlot || !pPlot->IsAdjacentOwnedByOtherTeam(getTeam()))
			continue;

		if(!pGeneral->canMoveInto(*pPlot,CvUnit::MOVEFLAG_DESTINATION))
			continue;

		if(!pPlot->canBuild(eCitadel, GetID()))
			continue;

		if (pPlot->getWorkingCity() && pPlot->getWorkingCity()->IsRazing())
			continue;

		//defense yield
		if(pPlot->GetDefenseBuildValue(GetID()) <= 0)
			continue;

		//don't consider plots we already targeted
		bool bTooClose = false;
		for (size_t i=0; i<vPlotsToAvoid.size(); i++)
			if (plotDistance(*vPlotsToAvoid[i],*pPlot)<3)
				bTooClose = true;
		if (bTooClose)
			continue;
		
		bool bGoodCandidate = true;
		std::vector<int> vPossibleCitadelTiles;

		//watch this! plotDirection[NUM_DIRECTION_TYPES] is the plot itself
		//we need to include it as it may belong to us or the enemy
		for(int iI = 0; iI < NUM_DIRECTION_TYPES+1; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			if(pAdjacentPlot == NULL)
				continue;

			// can't build on some plots
			if(pAdjacentPlot->isCity() || pAdjacentPlot->isWater() || !pAdjacentPlot->isValidMovePlot(GetID()) )
				continue;
			if(!pAdjacentPlot->canBuild(eCitadel, GetID()))
				continue;

			if(!pAdjacentPlot->IsAdjacentOwnedByOtherTeam(getTeam()))
				continue;

			// don't build right next door to an existing citadel
			ImprovementTypes eImprovement = (ImprovementTypes)pAdjacentPlot->getImprovementType();
			if (eImprovement != NO_IMPROVEMENT)
			{
				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
				//don't remove existing great people improvements
				if(pkImprovementInfo && pkImprovementInfo->IsCreatedByGreatPerson())
					continue;
			}

			// if no improvement can be built on this plot, then don't consider it
			FeatureTypes eFeature = pPlot->getFeatureType();
			if (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->isNoImprovement())
				continue;

			// make sure we don't step on the wrong toes
			const PlayerTypes eOwner = pAdjacentPlot->getOwner();
			if (eOwner != NO_PLAYER && eOwner != m_eID )
			{
				if(GET_PLAYER(eOwner).isMinorCiv())
				{
					MinorCivApproachTypes eMinorApproach = GetDiplomacyAI()->GetMinorCivApproach(eOwner);
					// if we're friendly or protective, don't be a jerk
					if(eMinorApproach == MINOR_CIV_APPROACH_FRIENDLY || eMinorApproach == MINOR_CIV_APPROACH_PROTECTIVE)
					{
						bGoodCandidate = false;
						break;
					}
				}
				else
				{
					MajorCivApproachTypes eMajorApproach = GetDiplomacyAI()->GetMajorCivApproach(eOwner, false);
					DisputeLevelTypes eLandDisputeLevel = GetDiplomacyAI()->GetLandDisputeLevel(eOwner);

					bool bTicked = (eMajorApproach <= MAJOR_CIV_APPROACH_GUARDED);
					bool bTickedAboutLand = (eLandDisputeLevel >= DISPUTE_LEVEL_STRONG);

					// only bomb if we're hostile
					if(!bTicked && !bTickedAboutLand)
					{
						bGoodCandidate = false;
						break;
					}
				}
			}

			vPossibleCitadelTiles.push_back(pAdjacentPlot->GetPlotIndex());
		}

		if (bGoodCandidate)
			setCandidates.insert( vPossibleCitadelTiles.begin(), vPossibleCitadelTiles.end() );
	}

	std::priority_queue<SPlotWithScore> goodPlots;

	//now that we have a number of possible plots, score each
	for (std::set<int>::iterator it = setCandidates.begin(); it != setCandidates.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(*it);
		int iScore = 0;

		//watch this! plotDirection[NUM_DIRECTION_TYPES] is the plot itself
		//we need to include it as it may belong to us or the enemy
		for(int iI = 0; iI < NUM_DIRECTION_TYPES+1; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

			if(pAdjacentPlot == NULL)
				continue;

			// don't evaluate city plots since we don't get ownership of them with the bomb
			if(pAdjacentPlot->isCity())
				continue;

			int iDefenseScore = pAdjacentPlot->GetDefenseBuildValue(GetID());
			if(iDefenseScore <= 0)
				continue;

			if(!pAdjacentPlot->IsAdjacentOwnedByOtherTeam(getTeam()))
				continue;

			int iWeightFactor = 1;
			// choke points are good, even if only adjacent to the citadel
			if(pAdjacentPlot->IsChokePoint())
			{
				iWeightFactor += 3;
			}

			//Let's grab embassies if we can!
			if (pAdjacentPlot->IsImprovementEmbassy())
			{
				iWeightFactor += 5;
			}

			const PlayerTypes eOtherPlayer = pAdjacentPlot->getOwner();
			if (eOtherPlayer != NO_PLAYER && eOtherPlayer != pGeneral->getOwner())
			{
				if(GET_PLAYER(eOtherPlayer).isMinorCiv())
				{
					MinorCivApproachTypes eMinorApproach = GetDiplomacyAI()->GetMinorCivApproach(eOtherPlayer);
					// if we're friendly or protective, don't count the tile (but accept it as collateral damage)
					if(eMinorApproach == MINOR_CIV_APPROACH_FRIENDLY || eMinorApproach == MINOR_CIV_APPROACH_PROTECTIVE)
					{
						continue;
					}
					else
					{
						// grabbing tiles away from minors is nice
						iWeightFactor += 3;
					}
				}
				else
				{
					MajorCivApproachTypes eMajorApproach = GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false);
					DisputeLevelTypes eLandDisputeLevel = GetDiplomacyAI()->GetLandDisputeLevel(eOtherPlayer);

					bool bTicked = (eMajorApproach <= MAJOR_CIV_APPROACH_GUARDED);
					bool bTickedAboutLand = (eLandDisputeLevel >= DISPUTE_LEVEL_STRONG);

					// don't count the tile if we're not hostile (but accept it as collateral damage)
					if(!bTicked && !bTickedAboutLand)
					{
						continue;
					}
					else
					{
						// grabbing tiles away from majors is really nice
						iWeightFactor += 4;
					}
				}

				//Do we get a bonus for citadels? Do it!
				if(IsCitadelBoost())
				{
					iWeightFactor += 3;
				}
			}

			// score resource
			ResourceTypes eResource = pAdjacentPlot->getResourceType();
			if(eResource != NO_RESOURCE)
			{
				iScore += (GetBuilderTaskingAI()->GetResourceWeight(eResource, NO_IMPROVEMENT, pAdjacentPlot->getNumResource()) * iWeightFactor);
			}

			// score yield
			for(int iYield = 0; iYield <= YIELD_TOURISM; iYield++)
			{
				iScore += (pAdjacentPlot->getYield((YieldTypes)iYield) * iWeightFactor);
			}

			//Defense build yield.
			iScore += iDefenseScore * 2;

			//danger is bad
			int iDanger = pGeneral->GetDanger(pAdjacentPlot);
			if (iDanger == INT_MAX)
			{
				iScore = 0;
				break;
			}
			else
			{
				iScore -= iDanger;
			}
		}

		//require a certain minimum score ...
		if(iScore > 0)
		{
			//we don't need an escort as the target is right on our border, but check for enemies nevertheless
			bool bSafe = true;
			int iRange = 2;
			for(int iX = -iRange; iX <= iRange; iX++)
			{
				for(int iY = -iRange; iY <= iRange; iY++)
				{
					CvPlot* pEvalPlot = NULL;
					pEvalPlot = plotXYWithRangeCheck(pGeneral->getX(), pGeneral->getY(), iX, iY, iRange);
					if(pEvalPlot && pEvalPlot->isVisibleEnemyUnit( GetID() ))
					{
						bSafe = false;
					}
				}
			}

			if (bSafe)
			{
				if (goodPlots.empty())
				{
					goodPlots.push( SPlotWithScore(pPlot,iScore) );
				}
				else if (iScore > goodPlots.top().score * 0.8f )
				{
					//don't even keep it if it's much worse than the current best
					goodPlots.push( SPlotWithScore(pPlot,iScore) );

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Great General considering a Citadel, Location: X: %d, Y: %d. SCORE: %d", pPlot->getX(), pPlot->getY(), iScore);
						GetHomelandAI()->LogHomelandMessage(strLogString);
					}
				}
			}
		}
	}

	if ( goodPlots.size() == 0 )
	{
		return NULL;
	}
	else if ( goodPlots.size() == 1 )
	{
		iResultScore = goodPlots.top().score;
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Great General found a single Citadel location, Location: X: %d, Y: %d. SCORE: %d", goodPlots.top().pPlot->getX(), goodPlots.top().pPlot->getY(), goodPlots.top().score);
			GetHomelandAI()->LogHomelandMessage(strLogString);
		}
		return goodPlots.top().pPlot;
	}
	else
	{
		//look at the top two and take the closer one
		SPlotWithScore nr1 = goodPlots.top(); goodPlots.pop();
		SPlotWithScore nr2 = goodPlots.top(); goodPlots.pop();
		if (nr2.score < nr1.score * 0.8f)
		{
			iResultScore = nr1.score;
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Great General found the top two Citadel locations, and chose first. Location: X: %d, Y: %d. SCORE: %d", goodPlots.top().pPlot->getX(), goodPlots.top().pPlot->getY(), goodPlots.top().score);
				GetHomelandAI()->LogHomelandMessage(strLogString);
			}
			return nr1.pPlot;
		}
		else
		{
			int iTurns1 = pGeneral->TurnsToReachTarget(nr1.pPlot, true);
			int iTurns2 = pGeneral->TurnsToReachTarget(nr2.pPlot, true);
			if (iTurns2*nr1.score < iTurns1*nr2.score )
			{
				iResultScore = nr2.score;
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Great General found the top two Citadel locations, and chose second because distance. Location: X: %d, Y: %d. SCORE: %d", nr2.pPlot->getX(), nr2.pPlot->getY(), iResultScore);
					GetHomelandAI()->LogHomelandMessage(strLogString);
				}
				return nr2.pPlot;
			}
			else
			{
				iResultScore = nr1.score;
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Great General found the top two Citadel locations, and chose first because distance. Location: X: %d, Y: %d. SCORE: %d", nr1.pPlot->getX(), nr1.pPlot->getY(), iResultScore);
					GetHomelandAI()->LogHomelandMessage(strLogString);
				}
				return nr1.pPlot;
			}
		}
	}
}

