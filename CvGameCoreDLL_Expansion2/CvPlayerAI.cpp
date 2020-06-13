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
#include "CvBarbarians.h"

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
inline CvPlayerAI& CvPlayerAI::getPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer != NO_PLAYER, "Player is not assigned a valid value");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "Player is not assigned a valid value");

	if (ePlayer == NO_PLAYER || ePlayer >= MAX_PLAYERS)
		ePlayer = BARBARIAN_PLAYER;

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

	if(isHuman())
	{
		CvUnit::dispatchingNetMessage(true);
		//no tactical AI for human, only make sure we have current postures in case we want the AI to take over (debugging)
		GetTacticalAI()->UpdatePostures();
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
	if (isHuman())
		return;

	PlayerTypes eOriginalOwner = pCity->getOriginalOwner();
	TeamTypes eOldOwnerTeam = GET_PLAYER(eOldOwner).getTeam();
#if defined(MOD_BALANCE_CORE)
	//Don't burn down gifts, that makes you look ungrateful.
	if (bGift && eOriginalOwner != GetID())
	{
		pCity->DoCreatePuppet();
		return;
	}
#endif

	// Liberate a city?
	if(eOriginalOwner != eOldOwner && eOriginalOwner != GetID() && CanLiberatePlayerCity(eOriginalOwner) && getNumCities() > 1)
	{
		// minor civ
		if(GET_PLAYER(eOriginalOwner).isMinorCiv())
		{
			// If we're a major civ, decision is made by diplo AI (minors don't liberate other minors)
			if (isMajorCiv())
			{
				if (GetDiplomacyAI()->DoPossibleMinorLiberation(eOriginalOwner, pCity->GetID()))
					return;
			}
		}
		// Original owner is a major civ
		else
		{
			// If we're a minor civ, only liberate if they're our ally
			if (isMinorCiv())
			{
				if (GetMinorCivAI()->GetAlly() == eOriginalOwner)
				{
					DoLiberatePlayer(eOriginalOwner, pCity->GetID());
					return;
				}
			}
			// If we're a major civ, decision is made by diplo AI
			else if (isMajorCiv())
			{
				if (GetDiplomacyAI()->DoPossibleMajorLiberation(eOriginalOwner, eOldOwner, pCity))
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
					if (GET_TEAM(getTeam()).isAtWar(eOldOwnerTeam))
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

	//annex holy city for faith use
	if (GetReligions()->GetCurrentReligion(false) != NO_RELIGION && pCity->GetCityReligions()->IsHolyCityForReligion(GetReligions()->GetCurrentReligion(false)))
	{
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

	int iLoop = 0;
	for (pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		//if we're already converting one, stop!
		if (pCity->IsOccupied() && !pCity->IsNoOccupiedUnhappiness())
			return;
	}

	std::vector<CityAndProduction> aCityAndProductions;
	pCity = NULL;
	for(pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		//simple check to stop razing "good" cities
		if (pCity->IsRazing() && pCity->HasAnyWonder() && !IsEmpireVeryUnhappy())
			unraze(pCity);

		if (pCity->IsResistance())
			continue;

		if (!pCity->IsPuppet())
			continue;

		//Original City and puppeted? Stop!
		if(pCity->getOriginalOwner() == GetID())
		{
			pCity->DoAnnex();
			return;
		}

		if (pCity->IsOriginalMajorCapital())
		{
			pCity->DoAnnex();
			return;
		}

		if (pCity->isBorderCity() || pCity->isCoastal())
		{
			pCity->DoAnnex();
			return;
		}

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
	OperationSlot bestSlot;
	// search through our operations till we find one that needs a unit
	std::map<int, CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if(pThisOperation)
		{
#if defined(MOD_BALANCE_CORE)
			bCitySameAsMuster = false;

			CvPlot *pMusterPlot = pThisOperation->GetMusterPlot();

			if (!pMusterPlot)
				continue;

			if (pCity == pMusterPlot->getOwningCity())
				bCitySameAsMuster = true;
			else if (!pThisOperation->IsNavalOperation() && pCity != NULL && pMusterPlot->getOwningCity() != NULL && pCity->getArea() == pMusterPlot->getOwningCity()->getArea())
			{
				int iDistance = plotDistance(pMusterPlot->getOwningCity()->getX(), pMusterPlot->getOwningCity()->getY(), pCity->getX(), pCity->getY());
				if (iDistance <= 5)
					bCitySameAsMuster = true;
			}

			if (pThisOperation->IsNavalOperation() && !pCity->isMatchingArea(pMusterPlot))
			{
					continue;
			}				
#endif
			OperationSlot thisSlot = pThisOperation->PeekAtNextUnitToBuild();

			if (!thisSlot.IsValid())
				continue;

			CvArmyAI* pThisArmy = GET_PLAYER(pCity->getOwner()).getArmyAI(thisSlot.m_iArmyID);

			if (!pThisArmy || !pThisArmy->GetFormationSlot(thisSlot.m_iSlotID)->IsFree())
				continue;

			if (OperationalAIHelpers::IsSlotRequired(GetID(), thisSlot))
			{
				bestSlot = thisSlot;
				break;
			}
		}
	}

	return bestSlot;
}


OperationSlot CvPlayerAI::CityCommitToBuildUnitForOperationSlot(CvCity* pCity)
{
	OperationSlot thisSlot;
	if (!pCity)
		return thisSlot;

	CvAIOperation* pBestOperation = NULL;
	int iBestScore = -1;

	// search through our operations till we find one that needs a unit
	std::map<int, CvAIOperation*>::iterator iter;
	for(iter = m_AIOperations.begin(); iter != m_AIOperations.end(); ++iter)
	{
		CvAIOperation* pThisOperation = iter->second;
		if (!pThisOperation || pThisOperation->GetMusterPlot() == NULL)
			continue;

		int iScore = 0;
		int iMusterArea = pThisOperation->GetMusterPlot()->getArea();
		int iDistance = plotDistance(*pThisOperation->GetMusterPlot(),*pCity->plot());

		//check if we're on the correct continent/ocean
		if(!pCity->isAdjacentToArea(iMusterArea))
			continue;

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

	if(pBestOperation != NULL)
	{
		thisSlot = pBestOperation->CommitToBuildNextUnit();
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
		// Pseudo Great People (units with missions from GP, but are not SPECIALUNIT_PEOPLE)
		else if (pLoopUnit->getSpecialUnitType() != eSpecialUnitGreatPerson && pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_ARTIST) && (pLoopUnit->getUnitInfo().GetGoldenAgeTurns() > 0 || pLoopUnit->getUnitInfo().GetBaseTurnsForGAPToCount() > 0) && pLoopUnit->getUnitInfo().IsGreatWorkUnit())
		{
			pLoopUnit->SetGreatPeopleDirective(GetDirectiveArtist(pLoopUnit));
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
	if (GetDiplomacyAI()->IsGoingForCultureVictory() || (GetCulture()->GetNumCivsInfluentialOn() > (GC.getGame().GetGameCulture()->GetNumCivsInfluentialForWin() / 2)))
	{
		if(pTarget)
		{
#if defined(MOD_BALANCE_CORE)
			if(pTarget->getOwner() != NO_PLAYER && GET_PLAYER(pTarget->getOwner()).isMajorCiv())
			{
				if (GetCulture()->GetInfluenceLevel(pTarget->getOwner()) <= INFLUENCE_LEVEL_POPULAR && GetCulture()->GetTurnsToInfluential(pTarget->getOwner()) <= 100)
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
	bool bAlmostThere = false;
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		BuildingTypes eNextWonderDesired = GetCitySpecializationAI()->GetNextWonderDesired();
		
		int iLoop;
		CvCity* pLoopCity;
		int iOurPower = pGreatEngineer->GetHurryStrength();
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			BuildingTypes eCurrentBuilding = pLoopCity->getProductionBuilding();
			//alright, we found a city with the wonder we want.
			if (eNextWonderDesired != NO_BUILDING && eCurrentBuilding != NO_BUILDING && eCurrentBuilding == eNextWonderDesired)
			{
				int iProductionNeeded = pLoopCity->getProductionNeeded() - pLoopCity->GetCityBuildings()->GetBuildingProduction(eCurrentBuilding);
				if (iProductionNeeded <= 1)
					continue;

				int iDelta = iProductionNeeded - iOurPower;
				
				//Surplus? Just how much?
				if (iDelta < 0)
				{
					iDelta *= -1;
					int iFraction = (iDelta * 100) / iProductionNeeded;

					//less than double waste? Go for it!
					if (iFraction <= 200)
					{
						eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
						break;
					}
				}
				//Deficit? How close are we?
				else
				{
					int iFraction = (iDelta * 100) / iProductionNeeded;

					//more than 80% completed with this? Go for it!
					if (iFraction <= 20)
					{
						eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
						break;
					}
					//50% of the way there? Let's hold on a bit.
					else if (iFraction <= 50)
						bAlmostThere = true;
				}
			}
		}
	}
	
	if (bAlmostThere)
		return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) < GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT())
		return eDirective;

#if defined(MOD_BALANCE_CORE)
	ImprovementTypes eManufactory = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_MANUFACTORY");
	int iFlavor =  GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION"));
	iFlavor += GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
	iFlavor += (GetPlayerTraits()->GetWLTKDGPImprovementModifier() / 5);
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (eYield == NULL)
			continue;
		iFlavor += GetPlayerTraits()->GetYieldChangePerImprovementBuilt(eManufactory, eYield);
	}
	iFlavor -= (GetCurrentEra() + GetCurrentEra() + getGreatEngineersCreated(true));
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
	// if the merchant is in an army, he's already marching to a destination, so don't evaluate him
	if(pGreatMerchant->getArmyID() != -1)
		return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	ImprovementTypes eCustomHouse = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CUSTOMS_HOUSE");
	// buff up customs houses up to your flavor
	if (GreatMerchantWantsCash() && eCustomHouse != NO_IMPROVEMENT)
	{
		int iFlavor = GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
		iFlavor += GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
		iFlavor += (GetPlayerTraits()->GetWLTKDGPImprovementModifier() / 5);
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			YieldTypes eYield = (YieldTypes)iYield;
			if (eYield == NULL)
				continue;
			iFlavor += GetPlayerTraits()->GetYieldChangePerImprovementBuilt(eCustomHouse, eYield);
		}
		iFlavor -= (GetCurrentEra() + GetCurrentEra() + getGreatMerchantsCreated(true));

		//don't count colonias here (IMPROVEMENT_CUSTOMS_HOUSE_VENICE), so venice will build any number of them once they run out of city states to buy
		int iNumImprovement = getImprovementCount(eCustomHouse);
		if (iNumImprovement <= iFlavor)
			return GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}

	// Attempt to find a target for our special mission
	CvPlot* pTarget = GreatMerchantWantsCash() ? FindBestMerchantTargetPlotForCash(pGreatMerchant) : FindBestMerchantTargetPlotForPuppet(pGreatMerchant);
	if (pTarget)
	{
		if (!pGreatMerchant->CanFoundColony())
			return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		else
		{
			bool bIsSafe;
			CvPlot* pSettlePlot = GetBestSettlePlot(pGreatMerchant, -1, bIsSafe);
			if (pSettlePlot == NULL)
				return GREAT_PEOPLE_DIRECTIVE_USE_POWER;

			//prefer to claim empty space if we can
			if ( GetCityDistanceInEstimatedTurns(pSettlePlot) < GetCityDistanceInEstimatedTurns(pTarget)+2 )
				return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
			else
				return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}

	}
	else if (pGreatMerchant->CanFoundColony())
	{
		bool bIsSafe;
		CvPlot* pPlot = GetBestSettlePlot(pGreatMerchant, -1, bIsSafe);
		if (pPlot != NULL)
			return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
	}

	//failsafe (wait until embarkation for barbarian-besieged venice)
	if(GC.getGame().getGameTurn() - pGreatMerchant->getGameTurnCreated() > GC.getAI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT() && GET_TEAM(getTeam()).canEmbark())
		return GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;

	return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveScientist(CvUnit* /*pGreatScientist*/)
{
	if (!IsAtWarAnyMajor() || GetDiplomacyAI()->GetStateAllWars() != STATE_ALL_WARS_LOSING)
	{
		ImprovementTypes eAcademy = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_ACADEMY");
		int iFlavor = GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
		iFlavor += (GetPlayerTraits()->GetWLTKDGPImprovementModifier() / 5);
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			YieldTypes eYield = (YieldTypes)iYield;
			if (eYield == NULL)
				continue;
			iFlavor += GetPlayerTraits()->GetYieldChangePerImprovementBuilt(eAcademy, eYield);
		}
		iFlavor += GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
		//This is to prevent a buildup of scientists if the AI is having a hard time planting them.
		iFlavor -= (GetCurrentEra() + GetCurrentEra() + getGreatScientistsCreated(true));
		// Even if not going spaceship right now, build academies up to your flavor.
		int iNumImprovement = getImprovementCount(eAcademy);
		if (iNumImprovement <= iFlavor)
			return GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}

	// a tech boost is never bad
	return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
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

	std::vector<CvPlot*> vDummy;
	BuildTypes eCitadel = (BuildTypes)GC.getInfoTypeForString("BUILD_CITADEL");
	//keep at least one general around
	if(iGreatGeneralCount > 1 && pGreatGeneral->getArmyID() == -1)
	{
		CvPlot* pTargetPlot = FindBestCultureBombPlot(pGreatGeneral, eCitadel, vDummy, false);
		//build a citadel
		if (pTargetPlot)
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
		if (GetReligionAI()->ChooseProphetConversionCity())
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
	int iDesiredEmb = max(1, ((iFlavorDiplo * 2) - 3));
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
	//Venice wants to buy city states, unless it already has enough cities, then it doesn't want city states.
	bool bIsVenice = GetPlayerTraits()->IsNoAnnexing();
	if (bIsVenice)
	{
		if (GetNumPuppetCities() > max(2,GC.getGame().GetNumMinorCivsEver()-3)) //what would be a sane limit?
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

CvPlot* CvPlayerAI::FindBestMerchantTargetPlotForPuppet(CvUnit* pMerchant)
{
	if (!pMerchant)
		return NULL;

	int iBestScore = 0;
	CvPlot* pBestTargetPlot = NULL;

	// Loop through each city state
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kPlayer.isMinorCiv() || !kPlayer.isAlive())
			continue;

		if (kPlayer.IsAtWarWith(GetID()))
			continue;

		int iLoop = 0;
		for (CvCity* pCity=kPlayer.firstCity(&iLoop); pCity; pCity=kPlayer.nextCity(&iLoop))
		{
			if (!pCity->plot()->isAdjacentRevealed(getTeam()))
				continue;

			//should not be too far out
			if (GetCityDistanceInEstimatedTurns(pCity->plot()) > 12)
				continue;

			//merchant may not be in the closest owned city, so cut him some slack
			int iPathTurns = 0;
			pMerchant->GeneratePath(pCity->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1, 23, &iPathTurns, true);
			if (iPathTurns < INT_MAX)
			{
				int iScore =  (pCity->getEconomicValue(GetID())*(100+10*kPlayer.getNumMilitaryUnits())) / (1+iPathTurns*iPathTurns);
				if (iScore > iBestScore)
				{
					iBestScore = iScore;
					//we can't use the city plot itself as target ... so with the approximate path flag this is our target
					pBestTargetPlot = pMerchant->GetPathLastPlot();
				}
			}
		}
	}

	return pBestTargetPlot;
}

CvPlot* CvPlayerAI::FindBestMerchantTargetPlotForCash(CvUnit* pMerchant)
{
	if(!pMerchant)
		return NULL;

	// distance and plot id
	vector< pair<int,int> > vCandidates;

	// Loop through each city state
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kPlayer.isMinorCiv() || !kPlayer.isAlive())
			continue;

		CvCity* pCity = kPlayer.getCapitalCity();
		if (!pCity || !pCity->plot()->isRevealed(getTeam()))
			continue;

		// Is this a minor we are friendly with?
		bool bMinorCivApproachIsCorrect = (GetDiplomacyAI()->GetMinorCivApproach(kPlayer.GetID()) != MINOR_CIV_APPROACH_CONQUEST);
		bool bNotPlanningAWar = GetDiplomacyAI()->GetWarGoal(kPlayer.GetID()) == NO_WAR_GOAL_TYPE;

		if (bMinorCivApproachIsCorrect && !kPlayer.IsAtWarWith(GetID()) && bNotPlanningAWar)
		{
			int iDistance = plotDistance(*pCity->plot(), *pMerchant->plot());
			vCandidates.push_back(make_pair(iDistance, pCity->plot()->GetPlotIndex()));
		}
	}

	sort(vCandidates.begin(), vCandidates.end());

	int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;
	for (size_t i = 0; i < vCandidates.size(); i++)
	{
		CvPlot* pTarget = GC.getMap().plotByIndexUnchecked(vCandidates[i].second);
		if (pMerchant->GeneratePath(pTarget, iFlags, 23, NULL, true))
			return pMerchant->GetPathLastPlot();
	}

	return NULL;
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
				if (pUnit->GeneratePath(pCity->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1))
					return pCity;
			}
		}
	}

	return NULL;
}

CvCity* CvPlayerAI::FindBestMessengerTargetCity(CvUnit* pUnit, const vector<int>& vIgnoreCities)
{
	CvWeightedVector<CvCity *, SAFE_ESTIMATE_NUM_CITIES, true> vTargets;

	// Loop through each city state
	for(int iI = MAX_MAJOR_CIVS; iI < MAX_CIV_PLAYERS; iI++)
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
					//we often have multiple messengers active at the same time, don't all go to the same target
					if (std::find(vIgnoreCities.begin(), vIgnoreCities.end(), pLoopCity->GetID()) != vIgnoreCities.end())
						continue;

					int iScore = ScoreCityForMessenger(pLoopCity, pUnit);
					if(iScore > 0)
						vTargets.push_back( pLoopCity,iScore);
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
				if (pUnit->GeneratePath(pCity->plot(), CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1))
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
	CvPlot* pPlot = pCity->plot();
	int iFriendshipFromUnit = pUnit->getTradeInfluence(pPlot);

	CvPlayer& kMinor = GET_PLAYER(pCity->getOwner());
	CvMinorCivAI* pMinorCivAI = kMinor.GetMinorCivAI();

	// Skip if not revealed
	if (!pPlot->isRevealed(getTeam()))
		return 0;

	// Sphere of Influence resolution
	if (pMinorCivAI->GetPermanentAlly() != NO_PLAYER)
		return 0;

	// Open Door resolution
	if (pMinorCivAI->IsNoAlly())
		return 0;

	// If we are at war with target minor, let's not send diplomatic lambs to slaughter.
	if (kMinor.GetMinorCivAI()->IsAtWarWithPlayersTeam(GetID()))
		return 0;

	// Barbarians expected?
	if (kMinor.GetMinorCivAI()->IsActiveQuestForPlayer(GetID(), MINOR_CIV_QUEST_HORDE) || kMinor.GetMinorCivAI()->IsActiveQuestForPlayer(GetID(), MINOR_CIV_QUEST_REBELLION))
		return 0;

	// Did we bully you recently?  If so, being friendly now would be very odd.
	if (pMinorCivAI->IsRecentlyBulliedByMajor(GetID()))
		return 0;

	//Return if we can't embark and they aren't on our landmass.
	if (pCity->getArea() != pUnit->plot()->getArea() && !GET_TEAM(getTeam()).canEmbark())
		return 0;

	//are we at war with a player close to this CS? Don't go near here!
	vector<PlayerTypes> currentWars = GetPlayersAtWarWith();
	for (size_t i = 0; i < currentWars.size(); i++)
	{
		CvPlayer& kEnemy = GET_PLAYER(currentWars[i]);
		if (kEnemy.isMajorCiv() && kEnemy.GetProximityToPlayer(kMinor.GetID()) > PLAYER_PROXIMITY_CLOSE)
			return 0;
	}

	EconomicAIStrategyTypes eNeedHappiness = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS");
	EconomicAIStrategyTypes eNeedHappinessCritical = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL");
	bool bNeedHappiness = (eNeedHappiness != NO_ECONOMICAISTRATEGY) ? GetEconomicAI()->IsUsingStrategy(eNeedHappiness) : false;
	bool bNeedHappinessCritical = (eNeedHappinessCritical != NO_ECONOMICAISTRATEGY) ? GetEconomicAI()->IsUsingStrategy(eNeedHappinessCritical) : false;

	// **************************
	// Approaches
	// **************************

	int iScore = 100;
	MinorCivApproachTypes eApproach = GetDiplomacyAI()->GetMinorCivApproach(kMinor.GetID());

	if (eApproach == MINOR_CIV_APPROACH_IGNORE)
		iScore /= 5;

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
				if (kMinor.getResourceInOwnedPlots(eResourceDemanded) > 0)
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
		iScore /= 2;
	}

	//If our friendship is under 0, we've probably done something bad to this City-State. Let's not look at them!
	if (kMinor.GetMinorCivAI()->GetBaseFriendshipWithMajor(GetID()) < -20)
	{
		iScore /= 5;
	}

	// **************************
	// Other Relationships!
	// **************************

	int iFriendshipWithMinor = pMinorCivAI->GetBaseFriendshipWithMajor(GetID());
	PlayerTypes eAlliedPlayer = pMinorCivAI->GetAlly();

	int iHighestInfluence = 0;
	// Loop through other players to see if we can pass them in influence
	for (int iOtherMajorLoop = 0; iOtherMajorLoop < MAX_MAJOR_CIVS; iOtherMajorLoop++)
	{
		PlayerTypes eOtherMajor = (PlayerTypes)iOtherMajorLoop;

		if (eOtherMajor == NO_PLAYER || eOtherMajor == GetID())
			continue;

		int iOtherPlayerFriendshipWithMinor = pMinorCivAI->GetBaseFriendshipWithMajor(eOtherMajor);
		if (iOtherPlayerFriendshipWithMinor > iHighestInfluence)
		{
			iHighestInfluence = iOtherPlayerFriendshipWithMinor;
		}
	}

	//What would be the difference her after we do a mission?
	int iDifference = iHighestInfluence - (iFriendshipWithMinor + iFriendshipFromUnit);

	// Are we allied? Yay! But let's be careful.
	if (eAlliedPlayer == GetID())
	{
		// Are WE allies by a wide margin (over 100)? If so, let's find someone new to love.
		if (iDifference < iFriendshipFromUnit * -2)
		{
			iScore /= 4;
		}
		// Are we close to losing our status? If so, obsess away!
		else if (iDifference < iFriendshipFromUnit * -1 || pMinorCivAI->IsCloseToNotBeingAllies(GetID()))
		{
			iScore *= 2;
		}
	}
	else if (eAlliedPlayer != NO_PLAYER)
	{
		//If their influence is way higher than ours, let's tune this down...
		if (iDifference > iFriendshipFromUnit * 2)
		{
			iScore /= 4;
		}
		else if (iDifference > iFriendshipFromUnit)
		{
			iScore /= 2;
		}
		//If we can pass them, ramp it up!
		else if (iDifference <= (iFriendshipFromUnit * -1))
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
		if((iFriendshipWithMinor + iFriendshipFromUnit) >= pMinorCivAI->GetAlliesThreshold(GetID()))
#else
		if((iFriendshipWithMinor + iFriendshipFromUnit) >= pMinorCivAI->GetAlliesThreshold())
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
	int iDistance = GetCityDistanceInEstimatedTurns(pPlot) * GC.getINFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE();

	//Are there barbarians near the city-state? If so, careful!
	if(kMinor.GetMinorCivAI()->IsThreateningBarbariansEventActiveForPlayer(GetID()))
		iDistance *= 3;

	if (kMinor.getCapitalCity() != NULL && kMinor.getCapitalCity()->isUnderSiege())
		iDistance *= 3;

	//Let's downplay far/distant minors without full embarkation.
	if((pCity->getArea() != pUnit->getArea()) && !GET_PLAYER(GetID()).CanCrossOcean())
		iDistance *= 3;

	//If this is way too far away, let's not penalize it too much.
	iScore -= iDistance;

	//All CSs should theoretically be valuable if we've gotten this far.
	return max(1,iScore);
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

CvPlot* CvPlayerAI::ChooseMessengerTargetPlot(CvUnit* pUnit, vector<int>* pvIgnoreCities)
{
	//this function is used for diplomat influence spread as well (embassies go through ChooseDiplomatTargetPlot)
	if(pUnit->AI_getUnitAIType() != UNITAI_MESSENGER && pUnit->AI_getUnitAIType() != UNITAI_DIPLOMAT)
	{
		return NULL;
	}

	CvCity* pCity = FindBestMessengerTargetCity(pUnit, pvIgnoreCities ? *pvIgnoreCities : vector<int>());
	CvPlot* pBestTarget = NULL;
	if(pCity == NULL)
	{
		return NULL;
	}

	// Find adjacent plot with no units (that aren't our own)
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pLoopPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iI));
		if(pLoopPlot == NULL)
			continue;

		if(!pLoopPlot->isValidMovePlot(GetID(), !pUnit->isRivalTerritory()))
			continue;

		if(pUnit->GetDanger(pLoopPlot)>20) //allow some fog danger
			continue;

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

	//remember this city
	if (pBestTarget && pvIgnoreCities)
		pvIgnoreCities->push_back(pCity->GetID());

	return pBestTarget;
}
#endif

CvPlot* CvPlayerAI::FindBestMusicianTargetPlot(CvUnit* pMusician)
{
	if(!pMusician)
		return NULL;

	// Find target civ
	PlayerTypes eTargetPlayer = GetCulture()->GetCivLowestInfluence(!pMusician->isRivalTerritory()/*bCheckOpenBorders*/);
	if (eTargetPlayer == NO_PLAYER)
		return NULL;

	//try the closest city first
	int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;
	CvCity* pTargetCity = GC.getGame().GetClosestCityByPlots(pMusician->plot(), eTargetPlayer);
	if (pTargetCity && pMusician->GeneratePath(pTargetCity->plot(), iFlags, 23, NULL, true))
		return pMusician->GetPathLastPlot();

	//fallback, try the capital
	pTargetCity = GET_PLAYER(eTargetPlayer).getCapitalCity();
	if (pTargetCity && pMusician->GeneratePath(pTargetCity->plot(), iFlags, 23, NULL, true))
		return pMusician->GetPathLastPlot();

	return NULL;
}

CvPlot* CvPlayerAI::FindBestCultureBombPlot(CvUnit* pUnit, BuildTypes eBuild, const std::vector<CvPlot*>& vPlotsToAvoid, bool bMustBeWorkable)
{
	if (!pUnit)
		return NULL;

	// we may build in one of our border tiles or in enemy tiles adjacent to them
	std::set<int> setCandidates;
	CvImprovementEntry* pkImprovementInfo = NULL;
	int iRange = 0;

	if (eBuild == NO_BUILD)
	{
		CvUnitEntry *pkUnitEntry = GC.getUnitInfo(pUnit->getUnitType());
		if (!pkUnitEntry || !pUnit->isCultureBomb())
			return NULL;

		iRange = range(pkUnitEntry->GetCultureBombRadius() + GetCultureBombBoost(), 1, 5);
	}
	else
	{
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if (!pkBuildInfo)
			return NULL;

		ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();
		pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (!pkImprovementInfo)
			return NULL;

		iRange = range(pkImprovementInfo->GetCultureBombRadius() + GetCultureBombBoost(), 1, 5);
	}

	// loop through plots and wipe out ones that are invalid
	for (PlotIndexContainer::iterator it = m_aiPlots.begin(); it != m_aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if(!pPlot)
			continue;

		//don't consider plots we already targeted
		bool bTooClose = false;
		for (size_t i=0; i<vPlotsToAvoid.size(); i++)
			if (plotDistance(*vPlotsToAvoid[i],*pPlot)<3)
				bTooClose = true;
		if (bTooClose)
			continue;
		
		bool bGoodCandidate = true;
		std::vector<int> vPossibleTiles;

		//watch this! plotDirection[NUM_DIRECTION_TYPES] is the plot itself
		//we need to include it as it may belong to us or the enemy
		for(int iI = 0; iI < NUM_DIRECTION_TYPES+1; ++iI)
		{
			CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));
			if(pAdjacentPlot == NULL)
				continue;
			if (!pUnit->canMoveInto(*pAdjacentPlot, CvUnit::MOVEFLAG_DESTINATION))
				continue;

			// don't waste a colonia ...
			if (bMustBeWorkable)
			{
				CvCity* pClosestCity = GetClosestCityByPlots(pAdjacentPlot);
				if (pClosestCity && pClosestCity->getWorkPlotDistance() < GetCityDistanceInPlots(pAdjacentPlot))
					continue;
			}

			// not if we're about to give up the city
			if (pAdjacentPlot->getOwningCity() && pAdjacentPlot->getOwningCity()->IsRazing())
				continue;

			// can't build on some plots
			if(pAdjacentPlot->isCity() || pAdjacentPlot->isWater() || !pAdjacentPlot->isValidMovePlot(GetID()) )
				continue;
			if(eBuild != NO_BUILD && !pAdjacentPlot->canBuild(eBuild, GetID()))
				continue;

			//citadel special
			if (pkImprovementInfo && pkImprovementInfo->GetDefenseModifier() > 0)
			{
				//we want to steal at least one plot
				if (!pAdjacentPlot->IsAdjacentOwnedByTeamOtherThan(getTeam()))
					continue;

				if (pAdjacentPlot->GetDefenseBuildValue(GetID()) <= 0)
					continue;
			}

			//don't remove existing great people improvements
			ImprovementTypes eExistingImprovement = (ImprovementTypes)pAdjacentPlot->getImprovementType();
			if (eExistingImprovement != NO_IMPROVEMENT)
			{
				CvImprovementEntry* pkImprovementInfo2 = GC.getImprovementInfo(eExistingImprovement);
				if(pkImprovementInfo2 && pkImprovementInfo2->IsCreatedByGreatPerson())
					continue;
			}

			// make sure we don't step on the wrong toes
			const PlayerTypes eOwner = pAdjacentPlot->getOwner();
			if (eOwner != NO_PLAYER && eOwner != BARBARIAN_PLAYER && eOwner != m_eID)
			{
				if (GetDiplomacyAI()->IsPlayerBadTheftTarget(eOwner, THEFT_TYPE_CULTURE_BOMB))
				{
					bGoodCandidate = false;
					break;
				}
			}

			vPossibleTiles.push_back(pAdjacentPlot->GetPlotIndex());
		}

		//a set guarantees uniqueness
		if (bGoodCandidate)
			setCandidates.insert(vPossibleTiles.begin(), vPossibleTiles.end() );
	}

	std::priority_queue<SPlotWithScore> goodPlots;

	//now that we have a number of possible plots, score each
	for (std::set<int>::iterator it = setCandidates.begin(); it != setCandidates.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(*it);
		int iScore = (pkImprovementInfo && pkImprovementInfo->GetDefenseModifier() > 0) ? pPlot->GetDefenseBuildValue(GetID()) : 0;

		for (int iI=0; iI<RING_PLOTS[iRange]; iI++)
		{
			CvPlot* pAdjacentPlot = iterateRingPlots(pPlot,iI);

			if(pAdjacentPlot == NULL)
				continue;

			// don't evaluate city plots since we don't get ownership of them with the bomb
			if(pAdjacentPlot->isCity())
				continue;

			//don't count the plot if we already own it
			if (pAdjacentPlot->getOwner() == GetID())
				continue;

			//danger is bad - check even adjacent plots!
			int iDanger = pUnit->GetDanger(pAdjacentPlot);
			if (iDanger > 0)
			{
				iScore = 0;
				break;
			}

			// don't build next to existing bombs
			if (iRange == 1)
			{
				ImprovementTypes eExistingImprovement = (ImprovementTypes)pAdjacentPlot->getImprovementType();
				if (eExistingImprovement != NO_IMPROVEMENT)
				{
					CvImprovementEntry* pkImprovementInfo2 = GC.getImprovementInfo(eExistingImprovement);
					if (pkImprovementInfo2 && pkImprovementInfo2->GetCultureBombRadius() > 1)
					{
						iScore = 0;
						break;
					}
				}
			}

			int iWeightFactor = 1;
			// choke points are good, even if only adjacent to the citadel
			if(pAdjacentPlot->IsChokePoint())
			{
				iWeightFactor += 3;
			}

			//Let's grab embassies if we can!
			if (pAdjacentPlot->IsImprovementEmbassy())
			{
				if (GET_PLAYER(pAdjacentPlot->GetPlayerThatBuiltImprovement()).getTeam() != GET_PLAYER(pUnit->getOwner()).getTeam())
					iWeightFactor += 5;
				else //don't steal our own embassy
					iWeightFactor = 1;
			}

			const PlayerTypes eOtherPlayer = pAdjacentPlot->getOwner();
			if (eOtherPlayer != NO_PLAYER && eOtherPlayer != BARBARIAN_PLAYER && eOtherPlayer != GetID())
			{
				if (GetDiplomacyAI()->IsPlayerBadTheftTarget(eOtherPlayer, THEFT_TYPE_CULTURE_BOMB))
				{
					iScore = 0;
					break;
				}
				else if (GET_PLAYER(eOtherPlayer).isMinorCiv())
				{
					// grabbing tiles away from minors is nice
					iWeightFactor += 3;
				}
				else
				{
					// grabbing tiles away from majors is really nice
					iWeightFactor += 4;
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
		}

		//require a certain minimum score ...
		if(iScore > 0)
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
					strLogString.Format("Possible culture bomb location: X: %d, Y: %d. SCORE: %d", pPlot->getX(), pPlot->getY(), iScore);
					GetHomelandAI()->LogHomelandMessage(strLogString);
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
		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			strLogString.Format("Found a single culture bomb location, Location: X: %d, Y: %d. SCORE: %d", goodPlots.top().pPlot->getX(), goodPlots.top().pPlot->getY(), goodPlots.top().score);
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
			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Found one good culture bomb location: X: %d, Y: %d. SCORE: %d", goodPlots.top().pPlot->getX(), goodPlots.top().pPlot->getY(), goodPlots.top().score);
				GetHomelandAI()->LogHomelandMessage(strLogString);
			}
			return nr1.pPlot;
		}
		else
		{
			SPlotWithScore chosen = nr1;
			int iTurns1 = pUnit->TurnsToReachTarget(nr1.pPlot, true);
			int iTurns2 = pUnit->TurnsToReachTarget(nr2.pPlot, true);
			if (iTurns2*nr1.score < iTurns1*nr2.score)
				chosen = nr2;

			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("Found at least two similar culture bomb locations, chosen location: X: %d, Y: %d. SCORE: %d", chosen.pPlot->getX(), chosen.pPlot->getY(), chosen.score);
				GetHomelandAI()->LogHomelandMessage(strLogString);
			}
			return chosen.pPlot;
		}
	}
}

