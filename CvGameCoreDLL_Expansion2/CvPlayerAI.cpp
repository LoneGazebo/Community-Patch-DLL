﻿/*	-------------------------------------------------------------------------------------------------------
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
#include "CvEnumMap.h"

#if defined(MOD_BALANCE_CORE)
#include "CvDistanceMap.h"
#endif

// Include this after all other headers.
#include "LintFree.h"

#if defined(MOD_BALANCE_CORE_MILITARY)
#include <queue>
#endif

// statics

static CvEnumMap<PlayerTypes, CvPlayerAI> s_players;

CvPlayerAI& CvPlayerAI::getPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer != NO_PLAYER, "Player is not assigned a valid value");
	CvAssertMsg(ePlayer < MAX_PLAYERS, "Player is not assigned a valid value");

	if (ePlayer <= NO_PLAYER || ePlayer >= MAX_PLAYERS)
		ePlayer = BARBARIAN_PLAYER;

	return s_players[ePlayer];
}

void CvPlayerAI::initStatics()
{
	s_players.init();
	for(std::size_t i = 0; i < s_players.size(); ++i)
	{
		s_players[i].m_eID = PlayerTypes(i);
	}
}

void CvPlayerAI::freeStatics()
{
	s_players.uninit();
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

	if(isHuman())
	{
		return;
	}

#if defined(MOD_BALANCE_CORE)
	//make sure we iterate our units in a sensible order
	struct CompareUnitPowerAscending
	{
		bool operator()(const CvUnit* a, const CvUnit* b)
		{
			if (a->GetPower() != b->GetPower())
				return a->GetPower() > b->GetPower();
			else //tiebreak
				return a->GetID() < b->GetID();
		}
	};

	//this orders units by combat strength
	m_units.OrderByContent( CompareUnitPowerAscending() );
#endif

	AI_doResearch();
	AI_considerAnnex();
}


void CvPlayerAI::AI_doTurnPost()
{
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
	int iLoop = 0;

	//order is important. when a unit was killed, an army might become invalid, which might invalidate an operation

	//unit cleanup (two step approach because deleting a unit invalidates the iterator)
	std::vector<CvUnit*> unitsToDelete;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		if (pLoopUnit->isDelayedDeath())
			unitsToDelete.push_back(pLoopUnit);

	for (size_t i = 0; i < unitsToDelete.size(); i++)
		unitsToDelete[i]->doDelayedDeath();

#if defined(MOD_CORE_DELAYED_VISIBILITY)
	//force explicit visibility update for killed units
	if (!unitsToDelete.empty())
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			GC.getMap().plotByIndexUnchecked(iI)->flipVisibility(getTeam());
#endif

	//army cleanup (two step approach because deleting a unit invalidates the iterator)
	std::vector<int> itemsToDelete;
	for(CvArmyAI* pLoopArmyAI = firstArmyAI(&iLoop); pLoopArmyAI != NULL; pLoopArmyAI = nextArmyAI(&iLoop))
		if (pLoopArmyAI->IsDelayedDeath())
			itemsToDelete.push_back(pLoopArmyAI->GetID());

	for (size_t i=0; i<itemsToDelete.size(); i++)
		getArmyAI(itemsToDelete[i])->Kill();

	//operation cleanup
	itemsToDelete.clear();
	for (size_t i=0; i<getNumAIOperations(); i++)
	{
		CvAIOperation* pOp = getAIOperationByIndex(i);
		if (pOp->ShouldAbort())
			itemsToDelete.push_back(pOp->GetID());
	}

	for (size_t i=0; i<itemsToDelete.size(); i++)
		getAIOperation(itemsToDelete[i])->Kill();
}

void CvPlayerAI::AI_doTurnUnitsPost()
{
	CvUnit* pLoopUnit = NULL;
	int iLoop = 0;

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

		bool bResult = false;
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
		GetHomelandAI()->Update();
		CvUnit::dispatchingNetMessage(false);
	}
	else
	{
		// Now let the tactical AI run.  Putting it after the operations update allows units who have
		// just been handed off to the tactical AI to get a move in the same turn they switch between
		GetTacticalAI()->Update();
		GetHomelandAI()->Update();
	}
}

void CvPlayerAI::AI_conquerCity(CvCity* pCity, PlayerTypes ePlayerToLiberate, bool bGift, bool bAllowSphereRemoval)
{
	if (isHuman())
		return;

	// What are our options for this city?
	bool bCanLiberate = ePlayerToLiberate != NO_PLAYER && !bGift; //shouldn't liberate cities you bought (exploitable)
	bool bCanRaze = canRaze(pCity) && !bGift; //shouldn't raze cities you bought
	bool bCanAnnex = !GetPlayerTraits()->IsNoAnnexing();

	// City-States
	if (isMinorCiv())
	{
		PlayerTypes eAlly = GetMinorCivAI()->GetAlly();

		// They will liberate their ally's team and no one else
		if (bCanLiberate && eAlly != NO_PLAYER && GET_PLAYER(eAlly).getTeam() == GET_PLAYER(ePlayerToLiberate).getTeam())
		{
			DoLiberatePlayer(ePlayerToLiberate, pCity->GetID(), false, false);
			return;
		}

		pCity->DoAnnex();
		return;
	}

	// If we have been actively trying to liberate this city, liberate it! (note - some more liberation checks are later in this method)
	if (bCanLiberate && GetDiplomacyAI()->IsTryingToLiberate(pCity, ePlayerToLiberate))
	{
		DoLiberatePlayer(ePlayerToLiberate, pCity->GetID(), false, false);
		return;
	}

	// Burn them all to the ground! (Timurids modmod)
	if (MOD_BALANCE_CORE_SETTLER_ADVANCED && GetPlayerTraits()->GetRazeSpeedModifier() > 0 && canRaze(pCity))
	{
		pCity->doTask(TASK_RAZE);
		return;
	}

	// What is our happiness situation?
	bool bUnhappy = false;

	if (MOD_BALANCE_CORE_HAPPINESS)
		bUnhappy = GetExcessHappiness() < /*40*/ GD_INT_GET(AI_MOSTLY_HAPPY_THRESHOLD); // as a buffer; if a little unhappy it's fine to take a city
	else
		bUnhappy = IsEmpireUnhappy();


	// City value is rated on a percentage scale, where 0 = worthless, 100 = equal value with median city, 200 = twice the value of the median city.
	int iMedianEconomicPower = GC.getGame().getMedianEconomicValue();
	int iLocalEconomicPower = pCity->getEconomicValue(GetID());
	int iCityValue = (iLocalEconomicPower * 100) / max(1, iMedianEconomicPower);

	// Modders can change this value to apply a multiplier to the worth of all cities
	iCityValue *= /*100*/ GD_INT_GET(AI_CITY_VALUE_MULTIPLIER);
	iCityValue /= 100;

	// Original major capitals are worth more.
	if (pCity->IsOriginalMajorCapital())
	{
		iCityValue *= /*150*/ GD_INT_GET(AI_CAPITAL_VALUE_MULTIPLIER);
		iCityValue /= 100;
	}

	bool bHighValue = iCityValue >= /*80*/ GD_INT_GET(AI_CITY_HIGH_VALUE_THRESHOLD);


	// Now that the preliminary checks are out of the way, a choice:
	// Should we keep the city (annex/puppet) or do we not want it too much (liberate if we want to, raze if low value/unhappy, otherwise puppet)?
	bool bKeepCity = bHighValue && !IsEmpireVeryUnhappy();

	//if it has wonders or is a holy city, keep it
	if (pCity->HasAnyWonder() || pCity->GetCityReligions()->IsHolyCityAnyReligion())
	{
		bKeepCity = true;
	}

	//only city in the area? may be strategically important
	CvArea* pArea = pCity->plot()->area();
	if (pArea != NULL && pArea->getCitiesPerPlayer(GetID()) < 1)
	{
		bKeepCity = true;
	}

	// If we're going for world conquest, have to keep any capitals we get
	if (!bKeepCity && pCity->IsOriginalMajorCapital() && (GetDiplomacyAI()->IsGoingForWorldConquest() || GetDiplomacyAI()->IsCloseToWorldConquest()))
	{
		bKeepCity = true;
	}

	// Want to keep city - will puppet/annex
	if (bKeepCity || (!bCanLiberate && !bCanRaze))
	{
		// Can't annex? Have to puppet.
		if (!bCanAnnex)
		{
			pCity->DoCreatePuppet();
			return;
		}

		// Too unhappy to annex? Let's puppet.
		if (bUnhappy)
		{
			pCity->DoCreatePuppet();
			return;
		}

		if (bCanAnnex)
		{
			// If we have a unique courthouse, use it!
			BuildingClassTypes iCourthouse = (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_COURTHOUSE");
			if (iCourthouse != -1 && getCivilizationInfo().isCivilizationBuildingOverridden(iCourthouse))
			{
				pCity->DoAnnex();
				return;
			}

			if (bHighValue)
			{
				pCity->DoAnnex();
				return;
			}

			if (!IsEmpireUnhappy())
			{
				pCity->DoAnnex();
				return;
			}
		}

		pCity->DoCreatePuppet();
		return;
	}
	// Doesn't want the city - might liberate, puppet or raze
	else
	{
		if (bCanLiberate)
		{
			if (GET_PLAYER(ePlayerToLiberate).isMinorCiv())
			{
				DoLiberatePlayer(ePlayerToLiberate, pCity->GetID(), false, false);
				return;
			}

			if (GetDiplomacyAI()->DoPossibleMajorLiberation(pCity, ePlayerToLiberate))
				return;
		}

		// If we have bonuses for liberating cities, remove Sphere of Influence if possible
		if (bAllowSphereRemoval && GetPlayerPolicies()->GetNumericModifier(POLICYMOD_LIBERATION_BONUS) > 0)
		{
			DoLiberatePlayer(ePlayerToLiberate, pCity->GetID(), false, true);
			return;
		}

		// If the city is of ok-ish value and we're not too unhappy, let's puppet
		if (!IsEmpireVeryUnhappy())
		{
			bool bPuppetBonuses = GetPlayerTraits()->IsNoAnnexing() || GetPlayerPolicies()->GetNumericModifier(POLICYMOD_PUPPET_BONUS) > 0;

			if (iCityValue >= /*40*/ GD_INT_GET(AI_CITY_SOME_VALUE_THRESHOLD) && (bPuppetBonuses || !bUnhappy))
			{
				pCity->DoCreatePuppet();
				return;
			}
			else if (iCityValue >= /*25*/ GD_INT_GET(AI_CITY_PUPPET_BONUS_THRESHOLD) && bPuppetBonuses && !bUnhappy)
			{
				pCity->DoCreatePuppet();
				return;
			}
		}

		// Remove sphere of influence?
		if (bAllowSphereRemoval)
		{
			DoLiberatePlayer(pCity->getPreviousOwner(), pCity->GetID(), false, true);
			return;
		}

		// Insufficient value or too unhappy? Burn, baby, burn!
		if (bCanRaze)
		{
			pCity->doTask(TASK_RAZE);
			return;
		}
	}

	pCity->DoCreatePuppet();
}

void CvPlayerAI::AI_chooseFreeGreatPerson()
{
	while(GetNumFreeGreatPeople() > 0)
	{
		UnitTypes eDesiredGreatPerson = NO_UNIT;

		// Highly wonder competitive and still early in game?
		if(GetDiplomacyAI()->GetWonderCompetitiveness() >= 8 && GC.getGame().getGameTurn() <= (GC.getGame().getEstimateEndTurn() / 2))
		{
			eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_ENGINEER");
		}
		else
		{
			// Pick the person based on our victory method
			if (GetDiplomacyAI()->IsGoingForWorldConquest())
			{
				eDesiredGreatPerson = GetMilitaryAI()->GetWarType() == WARTYPE_SEA ? GetSpecificUnitType("UNITCLASS_GREAT_ADMIRAL") : GetSpecificUnitType("UNITCLASS_GREAT_GENERAL");
			}
			else if (GetDiplomacyAI()->IsGoingForCultureVictory())
			{
				eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_ARTIST");
			}
			else if (GetDiplomacyAI()->IsGoingForDiploVictory())
			{
				if (MOD_BALANCE_VP)
					eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_GREAT_DIPLOMAT");
				else
					eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_MERCHANT");
			}
			else if (GetDiplomacyAI()->IsGoingForSpaceshipVictory())
			{
				eDesiredGreatPerson = GetSpecificUnitType("UNITCLASS_SCIENTIST");
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

	if(GC.getGame().isOption(GAMEOPTION_NO_SCIENCE))
		return;

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
	int iI = 0;

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

void CvPlayerAI::AI_considerAnnex()
{
	if (isHuman())
		return;

	AI_PERF("AI-perf.csv", "AI_ considerAnnex");

	// for Venice
	if (GetPlayerTraits()->IsNoAnnexing())
	{
		return;
	}

	// if our capital city is puppeted, annex it
	// can happen if we lose our real capital
	CvCity* pCapital = getCapitalCity();
	if (pCapital && pCapital->IsPuppet())
	{
		pCapital->DoAnnex();
		return;
	}

	// no annexing if the empire is unhappy
	if (IsEmpireUnhappy())
	{
		return;
	}

	//Find the anti-occupation building
	BuildingClassTypes eCourthouseType = NO_BUILDINGCLASS;
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
	for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		//if we're already converting one, stop!
		if (pCity->IsOccupied() && !pCity->IsNoOccupiedUnhappiness())
			return;
	}

	//Need this later
	ReligionTypes eOurReligion = GetReligions()->GetStateReligion(false);

	vector<OptionWithScore<CvCity*>> options;
	vector<PlayerTypes> vUnfriendlyMajors = GetUnfriendlyMajors();
	for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		//simple check to stop razing "good" cities
		if (pCity->IsRazing() && pCity->HasAnyWonder())
			unraze(pCity);

		if (pCity->IsResistance())
			continue;

		if (!pCity->IsPuppet() && !pCity->IsRazing())
			continue;

		int iWeight = 1;

		// Original City and puppeted? Stop!
		if (pCity->getOriginalOwner() == GetID())
			iWeight += 4;

		// if a capital city is puppeted, annex it
		if (pCity->IsOriginalMajorCapital())
			iWeight += 5;
	
		// annex the holy city for our religion
		if (pCity->GetCityReligions()->IsHolyCityForReligion(eOurReligion))
			iWeight += 4;

		// if we don't have a religion, but we have a Holy City, let's grab it
		if (eOurReligion == NO_RELIGION && pCity->GetCityReligions()->IsHolyCityAnyReligion())
		{
			int iBonus = 4 - (int)GetCurrentEra();
			iWeight += (iBonus > 0) ? iBonus : 0;
		}

		if (pCity->isCoastal() || (!vUnfriendlyMajors.empty() && pCity->isBorderCity(vUnfriendlyMajors)))
			iWeight += 1;

		// Add weight for each World Wonder in the city - cities with Wonders should be annexed quickly so we can benefit from their bonuses
		iWeight += pCity->getNumWorldWonders();

		// if we're going for a culture victory, don't consider annexing ordinary cities
		if (GetDiplomacyAI()->IsGoingForCultureVictory())
			if (iWeight < 4)
				continue;

		// higher bar if close to winning
		if (GetDiplomacyAI()->IsCloseToCultureVictory())
			if (iWeight < 5)
				continue;

		// if we're willing to consider annexing, annex cities that are in danger more quickly, so we can produce defenses
		// ... but don't bother if the city's about to fall
		if (!pCity->isInDangerOfFalling() && pCity->isUnderSiege())
			iWeight += 3;

		int iScore = iWeight * pCity->getYieldRateTimes100(YIELD_PRODUCTION, false);
		options.push_back( OptionWithScore<CvCity*>(pCity,iScore) );
	}

	if (!options.empty())
	{
		//descending by default
		std::stable_sort(options.begin(), options.end());

		CvCity* pTargetCity = options.front().option;
		if (pTargetCity)
		{
			if (pTargetCity->IsRazing())
				unraze(pTargetCity);
			else
				pTargetCity->DoAnnex();
		}
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
				FILogFile* pLog = NULL;
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
			CvWeightedVector<int> flavorChoices;
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
				flavorChoices.StableSortItems();
				
				//And grab the top selection.
				EventChoiceTypes eBestEventChoice = (EventChoiceTypes)flavorChoices.GetElement(0);

				if(GC.getLogging() && GC.getAILogging())
				{
					CvModEventChoiceInfo* pkEventChoiceInfo = GC.getEventChoiceInfo(eBestEventChoice);
					if(pkEventChoiceInfo != NULL)
					{
						CvString playerName;
						FILogFile* pLog = NULL;
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
					DoEventChoice(eBestEventChoice, NO_EVENT, false);
					return;
				}
			}
			//If we got here, it is because we haven't made a choice yet. Do so now.
			CvWeightedVector<int> randomChoices;
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
			randomChoices.StableSortItems();
				
			//And grab the top selection.
			EventChoiceTypes eBestEventChoice = (EventChoiceTypes)randomChoices.GetElement(0);

			if(GC.getLogging() && GC.getAILogging())
			{
				CvModEventChoiceInfo* pkEventChoiceInfo = GC.getEventChoiceInfo(eBestEventChoice);
				if(pkEventChoiceInfo != NULL)
				{
					CvString playerName;
					FILogFile* pLog = NULL;
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
				DoEventChoice(eBestEventChoice, NO_EVENT, false);
				return;
			}
		}
	}
}
bool CvPlayerAI::AI_DoEspionageEventChoice(CityEventTypes eEvent, int uiSpyIndex, CvCity* pCity)
{
	if (GetEspionage() == NULL)
		return false;

	if (eEvent != NO_EVENT_CITY)
	{
		CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
		if (pkEventInfo != NULL)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString playerName;
				FILogFile* pLog = NULL;
				CvString strBaseString;
				CvString strOutBuf;
				CvString strFileName = "EspionageLog.csv";
				playerName = getCivilizationShortDescription();
				pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
				strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
				strBaseString += playerName + ", ";
				strOutBuf.Format("AI considering AA Event choices: %s", pkEventInfo->GetDescription());
				strBaseString += strOutBuf;
				pLog->Msg(strBaseString);
			}
			//Lua Hook
			if (GAMEEVENTINVOKE_TESTANY(GAMEEVENT_OverrideAIEventChoice, GetID(), eEvent) == GAMEEVENTRETURN_TRUE)
			{
				return true;
			}

			// Now let's get the event flavors.
			CvWeightedVector<int> flavorChoices;
			for (int iLoop = 0; iLoop < GC.getNumCityEventChoiceInfos(); iLoop++)
			{
				CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)iLoop;
				if (eEventChoice != NO_EVENT_CHOICE_CITY)
				{
					CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
					if (pkEventChoiceInfo != NULL)
					{
						if (pCity->IsCityEventChoiceValidEspionage(eEventChoice, eEvent, uiSpyIndex, GetID()))
						{
							int iRisk = pkEventChoiceInfo->GetIdentificationModifier() / 2;
							int iDeath = pkEventChoiceInfo->GetDeathModifier() / 2;
							for (int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
							{
								if (pkEventChoiceInfo->getFlavorValue(iFlavor) > 0)
								{
									int iOurFlavor = GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavor);
									iOurFlavor += pkEventChoiceInfo->getFlavorValue(iFlavor) * 10;

									if (pCity->getOwner() == GetID())
									{
										iOurFlavor *= (100 + iRisk);
										iOurFlavor /= 100;
										iOurFlavor *= (100 + iDeath);
										iOurFlavor /= 100;
									}
									else
									{
										iOurFlavor *= (100 - iRisk);
										iOurFlavor /= 100;
										iOurFlavor *= (100 - iDeath);
										iOurFlavor /= 100;
									}

									if (pkEventChoiceInfo->getDamageCity() != 0 || pkEventChoiceInfo->getDamageGarrison() != 0)
									{
										CvCity* pCity = GetEspionage()->GetCityWithSpy(uiSpyIndex);
										if (pCity && pCity->isUnderSiege())
											iOurFlavor *= 10;
									}

									if (pkEventChoiceInfo->isSurveillance())
									{
										CvCity* pCity = GetEspionage()->GetCityWithSpy(uiSpyIndex);
										if (pCity && pCity->IsResistance())
											iOurFlavor *= 10;
									}

									//counterspy filter selection
									if (pCity->getOwner() == GetID())
									{
										if (pCity->getHappinessDelta() < 0 && pkEventChoiceInfo->getCityHappiness() > 0)
											iOurFlavor *= 2;

										if (pkEventChoiceInfo->getCityDefenseModifier() > 0 && IsAtWarAnyMajor() && pCity->isBorderCity())
											iOurFlavor *= 2;
									}
									flavorChoices.push_back(eEventChoice, iOurFlavor);
								}
							}
						}
					}
				}
			}
			if (flavorChoices.size() > 0)
			{
				//sort em!
				flavorChoices.StableSortItems();

				//And grab the top selection.
				CityEventChoiceTypes eBestEventChoice = (CityEventChoiceTypes)flavorChoices.GetElement(0);

				if (GC.getLogging() && GC.getAILogging())
				{
					CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eBestEventChoice);
					if (pkEventChoiceInfo != NULL)
					{
						CvString playerName;
						FILogFile* pLog = NULL;
						CvString strBaseString;
						CvString strOutBuf;
						CvString strFileName = "EspionageLog.csv";
						playerName = getCivilizationShortDescription();
						pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
						strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
						strBaseString += playerName + ", ";
						strOutBuf.Format("AI made an AA flavor event choice: %s", pkEventChoiceInfo->GetDescription());
						strBaseString += strOutBuf;
						pLog->Msg(strBaseString);
					}
				}

				if (eBestEventChoice != NO_EVENT_CHOICE_CITY)
				{
					pCity->DoEventChoice(eBestEventChoice, NO_EVENT_CITY, false, uiSpyIndex, GetID());
					return true;
				}
			}
			//If we got here, it is because we haven't made a choice yet. Do so now.
			CvWeightedVector<int> randomChoices;
			for (int iLoop = 0; iLoop < GC.getNumCityEventChoiceInfos(); iLoop++)
			{
				CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)iLoop;
				if (eEventChoice != NO_EVENT_CHOICE_CITY)
				{
					CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
					if (pkEventChoiceInfo != NULL)
					{
						if (pCity->IsCityEventChoiceValidEspionage(eEventChoice, eEvent, uiSpyIndex, GetID()))
						{
							int iRandom = GC.getGame().getJonRandNum(pkEventInfo->getNumChoices(), "Random Event Choice");
							if (iRandom <= 0)
							{
								iRandom = 1;
							}
							randomChoices.push_back(eEventChoice, iRandom);
						}
					}
				}
			}
			randomChoices.StableSortItems();
			if (randomChoices.size() <= 0)
				return false;

			//And grab the top selection.
			CityEventChoiceTypes eBestEventChoice = (CityEventChoiceTypes)randomChoices.GetElement(0);

			if (GC.getLogging() && GC.getAILogging())
			{
				CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eBestEventChoice);
				if (pkEventChoiceInfo != NULL)
				{
					CvString playerName;
					FILogFile* pLog = NULL;
					CvString strBaseString;
					CvString strOutBuf;
					CvString strFileName = "EspionageLog.csv";
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
			if (eBestEventChoice != NO_EVENT_CHOICE_CITY)
			{
				pCity->DoEventChoice(eBestEventChoice, NO_EVENT_CITY, false, uiSpyIndex, GetID());
				return true;
			}
		}
	}

	return false;
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

template<typename PlayerAI, typename Visitor>
void CvPlayerAI::Serialize(PlayerAI& /*playerAI*/, Visitor& /*visitor*/)
{
}

//
// read object from a stream
// used during load
//
void CvPlayerAI::Read(FDataStream& kStream)
{
	CvPlayer::Read(kStream);	// read base class data first

	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}


//
// save object to a stream
// used during save
//
void CvPlayerAI::Write(FDataStream& kStream) const
{
	CvPlayer::Write(kStream);	// write base class data first

	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
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
	for (size_t i = 0; i < m_AIOperations.size(); i++)
	{
		CvAIOperation* pThisOperation = m_AIOperations[i].second;
		if(pThisOperation)
		{
			bCitySameAsMuster = false;

			CvPlot *pMusterPlot = pThisOperation->GetMusterPlot();

			if (!pMusterPlot)
				continue;

			if (pCity == pMusterPlot->getOwningCity() && pCity->HasAccessToLandmass(pMusterPlot->getLandmass()))
				bCitySameAsMuster = true;

			if (pThisOperation->IsNavalOperation() && !pCity->HasAccessToLandmass(pMusterPlot->getLandmass()))
				continue;

			OperationSlot thisSlot = pThisOperation->PeekAtNextUnitToBuild();
			if (!thisSlot.IsValid())
				continue;

			CvArmyAI* pThisArmy = GET_PLAYER(pCity->getOwner()).getArmyAI(thisSlot.m_iArmyID);

			if (!pThisArmy || !pThisArmy->GetSlotStatus(thisSlot.m_iSlotID)->IsFree())
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


void CvPlayerAI::CityCommitToBuildUnitForOperationSlot(OperationSlot thisSlot)
{
	CvAIOperation* pThisOperation = getAIOperation(thisSlot.m_iOperationID);
	if (pThisOperation)
		pThisOperation->CommitToBuildNextUnit(thisSlot);
}

void CvPlayerAI::CityUncommitToBuildUnitForOperationSlot(OperationSlot thisSlot)
{
	CvAIOperation* pThisOperation = getAIOperation(thisSlot.m_iOperationID);
	if(pThisOperation)
		pThisOperation->UncommitToBuildUnit(thisSlot);
}

void CvPlayerAI::CityFinishedBuildingUnitForOperationSlot(OperationSlot thisSlot, CvUnit* pThisUnit)
{
	// find this operation
	CvAIOperation* pThisOperation = getAIOperation(thisSlot.m_iOperationID);
	CvArmyAI* pThisArmy = getArmyAI(thisSlot.m_iArmyID);
	if(pThisOperation && pThisArmy && pThisUnit)
	{
		pThisArmy->AddUnit(pThisUnit->GetID(), thisSlot.m_iSlotID,true);
		pThisOperation->FinishedBuildingUnit(thisSlot);
	}
}

int CvPlayerAI::GetNumUnitsNeededToBeBuilt()
{
	int iRtnValue = 0;

	for (size_t i = 0; i < m_AIOperations.size(); i++)
	{
		CvAIOperation* pThisOperation = m_AIOperations[i].second;
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

	int iLoop = 0;
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
		switch(pLoopUnit->getUnitInfo().GetDefaultUnitAIType()) //look at the default type!
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
		case UNITAI_DIPLOMAT:
			eDirective = GetDirectiveDiplomat(pLoopUnit);
			break;
		default:
			continue; // Not a great person.
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

	// Create Great Work if there is a slot
	GreatWorkType eGreatWork = pGreatWriter->GetGreatWork();
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GetEconomicAI()->GetBestGreatWorkCity(pGreatWriter->plot(), eGreatWork))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
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

	else if ((GC.getGame().getGameTurn() - pGreatWriter->getGameTurnCreated()) >= (/*2*/ GD_INT_GET(AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT) / 2))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_CULTURE_BLAST;
	}
	return eDirective;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveArtist(CvUnit* pGreatArtist)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

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
	if ((GC.getGame().getGameTurn() - pGreatArtist->getGameTurnCreated()) >= (/*2*/ GD_INT_GET(AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT) / 2))
	{
		eDirective = GREAT_PEOPLE_DIRECTIVE_GOLDEN_AGE;
	}

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
			if (pTarget->getOwner() != NO_PLAYER && GET_PLAYER(pTarget->getOwner()).isMajorCiv())
			{
				if (GetCulture()->GetInfluenceLevel(pTarget->getOwner()) <= INFLUENCE_LEVEL_POPULAR)
				{
					if (GetCulture()->GetTurnsToInfluential(pTarget->getOwner()) <= 100 || getGreatMusiciansCreated(true) > GC.getGame().getCurrentEra())
						return GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST;

				}
			}
		}
	}

	// Create Great Work if there is a slot
	GreatWorkType eGreatWork = pGreatMusician->GetGreatWork();
	if (GetEconomicAI()->GetBestGreatWorkCity(pGreatMusician->plot(), eGreatWork))
	{
		return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	if ((GC.getGame().getGameTurn() - pGreatMusician->getGameTurnCreated()) >= (/*2*/ GD_INT_GET(AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT) / 2))
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
		
		int iLoop = 0;
		CvCity* pLoopCity = NULL;
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

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) < /*5*/ GD_INT_GET(AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT))
		return eDirective;

#if defined(MOD_BALANCE_CORE)
	ImprovementTypes eManufactory = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_MANUFACTORY");
	int iFlavor =  GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION"));
	iFlavor += GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
	iFlavor += (GetPlayerTraits()->GetWLTKDGPImprovementModifier() / 5);
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		if (eYield == NO_YIELD)
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
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) >= (/*5*/ GD_INT_GET(AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT) - GetCurrentEra()))
#else
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && GC.getGame().getGameTurn() <= ((GC.getGame().getEstimateEndTurn() * 3) / 4))
	{
		if (GetDiplomacyAI()->IsGoingForWorldConquest())
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}
	if(eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) >= /*5*/ GD_INT_GET(AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT))
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
			iFlavor += GetPlayerTraits()->GetYieldChangePerImprovementBuilt(eCustomHouse, (YieldTypes)iYield);

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
			CvPlot* pSettlePlot = GetBestSettlePlot(pGreatMerchant);
			if (pSettlePlot == NULL)
				return GREAT_PEOPLE_DIRECTIVE_USE_POWER;

			//prefer to claim empty space if we can
			if ( GetCityDistancePathLength(pSettlePlot) < GetCityDistancePathLength(pTarget)+2 )
				return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
			else
				return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}

	}
	else if (pGreatMerchant->CanFoundColony())
	{
		CvPlot* pPlot = GetBestSettlePlot(pGreatMerchant);
		if (pPlot != NULL)
			return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
	}

	//failsafe (wait until embarkation for barbarian-besieged venice)
	if(GC.getGame().getGameTurn() - pGreatMerchant->getGameTurnCreated() > /*5*/ GD_INT_GET(AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT) && CanEmbark())
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
			if (eYield == NO_YIELD)
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
	if (MOD_ERA_RESTRICTED_GENERALS)
	{
		bool bHasGeneralNegation = false;
		std::string GGNegationPromotions[4] = { "PROMOTION_NEGATE_GENERAL", "PROMOTION_NEGATE_GENERAL_S", "PROMOTION_NEGATE_GENERAL_P", "PROMOTION_NEGATE_GENERAL_SP" };

		for (int i = 0; i < 4; i++)
			if (pGreatGeneral->isHasPromotion((PromotionTypes)GC.getInfoTypeForString(GGNegationPromotions[i].c_str(), true)))
				bHasGeneralNegation = true;

		if (bHasGeneralNegation == true)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("%s is obsolete", pGreatGeneral->getName().GetCString());
				GetHomelandAI()->LogHomelandMessage(strLogString);
			}
			return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	//units in armies are always field commanders
	if (pGreatGeneral->getArmyID() != -1)
		return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;

	//this one is sticky
	if (pGreatGeneral->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
		return GREAT_PEOPLE_DIRECTIVE_USE_POWER;

	//we might have multiple generals ... first get an overview
	int iCommanders = 0;
	int iCitadels = 0;
	int iLoop = 0;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->IsGreatGeneral())
		{
			switch (pLoopUnit->GetGreatPeopleDirective())
			{
			case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
				break;
			case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
				iCitadels++;
				break;
			case GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND:
				iCommanders++;
				break;
			default:
				UNREACHABLE(); // Great generals shouldn't have directives other than these.
			}
		}
	}

	//during war we want field commanders
	int iWars = (int)GetPlayersAtWarWith().size();
	//just a rough estimation
	int iPotentialArmies = max(1,GetMilitaryAI()->GetNumLandUnits()-getNumCities()*3) / 13;

	int iDesiredNumCommanders = max(1, (iWars+iPotentialArmies)/2);
	if (iCommanders <= iDesiredNumCommanders || pGreatGeneral->getArmyID() != -1 || pGreatGeneral->IsRecentlyDeployedFromOperation())
		return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;

	//build one citadel at a time
	std::vector<CvPlot*> vDummy;
	BuildTypes eCitadel = (BuildTypes)GC.getInfoTypeForString("BUILD_CITADEL");
	if(iCitadels==0)
	{
		CvPlot* pTargetPlot = FindBestCultureBombPlot(pGreatGeneral, eCitadel, vDummy, false);
		if (pTargetPlot)
			return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}
	
	// default
	return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveProphet(CvUnit* pUnit)
{
	GreatPeopleDirectiveTypes eDirective = NO_GREAT_PEOPLE_DIRECTIVE_TYPE;

	ReligionTypes eReligion = GetReligions()->GetOwnedReligion();
	const CvReligion* pMyReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, GetID());

	// sometimes we have no choice
	if (pUnit && !pUnit->GetReligionData()->IsFullStrength())
		return GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;

	// CASE 1: I have an enhanced religion. 
	if (pMyReligion && pMyReligion->m_bEnhanced)
	{
		ImprovementTypes eHolySite = (ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_HOLY_SITE");
		int iFlavor =  GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
		iFlavor -= GetNumUnitsWithUnitAI(UNITAI_PROPHET,false);

		//Let's use our prophets for improvments instead of wasting them on conversion.
		int iNumImprovement = getImprovementCount(eHolySite);
		if(iNumImprovement <= iFlavor || GetReligionAI()->ChooseProphetConversionCity(pUnit)==NULL)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
		else
		{
			//Only convert once we've hit our peak.
			eDirective = GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;
		}
	}

	// CASE 2: I have a religion that hasn't yet been enhanced and we still have the holy city
	else if (pMyReligion && pMyReligion->GetHolyCity()->getOwner() == pUnit->getOwner())
	{
		// emergency re-conversions needed? hopefully not
		if (pMyReligion->GetHolyCity()->GetCityReligions()->GetReligiousMajority() == eReligion)
			eDirective = GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		else
			eDirective = GREAT_PEOPLE_DIRECTIVE_SPREAD_RELIGION;
	}

	// CASE 3: No religion for me yet
	else
	{
		// Locked out?
		if (GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() <= 0 && !GetPlayerTraits()->IsAlwaysReligion())
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

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveAdmiral(CvUnit* pGreatAdmiral)
{
	if (MOD_ERA_RESTRICTED_GENERALS)
	{
		bool bHasAdmiralNegation = false;
		std::string GANegationPromotions[4] = {"PROMOTION_NEGATE_ADMIRAL", "PROMOTION_NEGATE_ADMIRAL_S", "PROMOTION_NEGATE_ADMIRAL_P", "PROMOTION_NEGATE_ADMIRAL_SP"};

		for (int i = 0; i < 4; i++)
			if (pGreatAdmiral->isHasPromotion((PromotionTypes)GC.getInfoTypeForString(GANegationPromotions[i].c_str(), true)))
				bHasAdmiralNegation = true;

		if (bHasAdmiralNegation == true)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("%s is obsolete", pGreatAdmiral->getName().GetCString());
				GetHomelandAI()->LogHomelandMessage(strLogString);
			}
			return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
		}
	}

	if (IsAtWar() || pGreatAdmiral->getArmyID() != -1 || pGreatAdmiral->IsRecentlyDeployedFromOperation())
		return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;

	int iGreatAdmiralCount = 0;
	int iLoop = 0;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->IsGreatAdmiral() && pLoopUnit->GetGreatPeopleDirective() != GREAT_PEOPLE_DIRECTIVE_USE_POWER) // should not count the admirals set for use_power as it expends all at the same turn
		{
			iGreatAdmiralCount++;
		}
	}

	int iThreshold = IsEmpireUnhappy() ? 0 : ((GetNumEffectiveCoastalCities() / 2) + 1); // open to tweaks, but a human player would rarely expend if happy, so should AI
	if (iGreatAdmiralCount > iThreshold && pGreatAdmiral->canGetFreeLuxury())
	{
		return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
}

GreatPeopleDirectiveTypes CvPlayerAI::GetDirectiveDiplomat(CvUnit* pGreatDiplomat)
{
	bool bTheVeniceException = GetPlayerTraits()->IsNoAnnexing();
	bool bTheAustriaException = GetPlayerTraits()->IsAbleToAnnexCityStates();
	
	int iFlavorDiplo =  GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
	int iNumMinors = GC.getGame().GetNumMinorCivsAlive();
	int iEmbassies = GetImprovementLeagueVotes();
	int iDesiredEmb = range(iFlavorDiplo*2 - 3, 1, iNumMinors);

	//Embassy numbers should be based on Diplomacy Flavor. More flavor, more embassies!
	CvPlot* pPlot = FindBestDiplomatTargetPlot(pGreatDiplomat);
	if (pPlot && !bTheAustriaException && !bTheVeniceException)
	{
		if ((iEmbassies < iDesiredEmb) || GetDiplomacyAI()->IsGoingForDiploVictory())
		{
			return GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}

	return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
}

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
			if (GetCityDistancePathLength(pCity->plot()) > 23)
				continue;

			//merchant may not be in the closest owned city, so cut him some slack
			int iPathTurns = 0;
			pMerchant->GeneratePath(pCity->plot(), CvUnit::MOVEFLAG_APPROX_TARGET_RING1, 23, &iPathTurns);
			if (iPathTurns < INT_MAX)
			{
				int iScore =  (pCity->getEconomicValue(GetID())*(100+10*kPlayer.getNumMilitaryUnits())) / (1+iPathTurns*iPathTurns);
				if (iScore > iBestScore)
				{
					iBestScore = iScore;
					pBestTargetPlot = pCity->plot();
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
		if (!pCity || !pCity->isRevealed(getTeam(),false,true))
			continue;

		// Is this a minor we are friendly with?
		if (GetDiplomacyAI()->GetCivApproach(kPlayer.GetID()) > CIV_APPROACH_HOSTILE && !kPlayer.IsAtWarWith(GetID()))
		{
			int iDistance = plotDistance(*pCity->plot(), *pMerchant->plot());
			vCandidates.push_back(make_pair(iDistance, pCity->plot()->GetPlotIndex()));
		}
	}

	std::stable_sort(vCandidates.begin(), vCandidates.end());

	int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_APPROX_TARGET_NATIVE_DOMAIN;
	for (size_t i = 0; i < vCandidates.size(); i++)
	{
		CvPlot* pTarget = GC.getMap().plotByIndexUnchecked(vCandidates[i].second);
		if (pMerchant->GeneratePath(pTarget, iFlags, 23))
			return pMerchant->GetPathLastPlot();
	}

	return NULL;
}

CvPlot* CvPlayerAI::FindBestDiplomatTargetPlot(CvUnit* pUnit)
{
	if (!pUnit)
		return NULL;

	//don't go too far out, it's dangerous
	SPathFinderUserData data(pUnit, 0, 13);
	data.iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY;
	ReachablePlots plots = GC.GetPathFinder().GetPlotsInReach(pUnit->plot(), data);
	set<PlayerTypes> badTargets;

	for (ReachablePlots::iterator it = plots.begin(); it != plots.end(); ++it)
	{
		//don't check for cities directly because we cannot enter them ...
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
		if (GET_PLAYER(pPlot->getOwner()).isMinorCiv())
		{
			//small performance optimization
			if (badTargets.find(pPlot->getOwner()) != badTargets.end())
				continue;

			//we iterate by distance, so take the first one we find
			CvCity* pCity = GET_PLAYER(pPlot->getOwner()).getCapitalCity();

			CvPlot* pBuildPlot = HomelandAIHelpers::GetPlotForEmbassy(pUnit, pCity);
			if (pBuildPlot)
				return pBuildPlot;
			else
				badTargets.insert(pPlot->getOwner());
		}
	}

	return NULL;
}

CvCity* CvPlayerAI::FindBestMessengerTargetCity(CvUnit* pUnit, const vector<int>& vIgnoreCities)
{
	CvWeightedVector<CvCity *> vTargets;

	// Loop through each city state
	for(int iI = MAX_MAJOR_CIVS; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && kPlayer.isMinorCiv() && !GET_TEAM(kPlayer.getTeam()).isAtWar(getTeam()))
		{
			//Loop through each city
			int iLoop = 0;
			CvCity* pLoopCity = NULL;
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
		vTargets.StableSortItems();

		//check if we can actually go there only if the city is promising
		for (int i = 0; i < vTargets.size(); ++i )
		{
			CvCity* pCity = vTargets.GetElement(i);
			if(pCity != NULL)
			{
				int iFlags = CvUnit::MOVEFLAG_NO_ENEMY_TERRITORY | CvUnit::MOVEFLAG_APPROX_TARGET_RING1 | CvUnit::MOVEFLAG_ABORT_IF_NEW_ENEMY_REVEALED;
				if (pUnit->GeneratePath(pCity->plot(), iFlags))
					return pCity;
			}
		}
	}

	return NULL;
}	
	
int CvPlayerAI::ScoreCityForMessenger(CvCity* pCity, CvUnit* pUnit)
{
	//First, the exclusions!

	//Initializations...
	CvPlot* pPlot = pCity->plot();
	CvUnitEntry *pkUnitEntry = GC.getUnitInfo(pUnit->getUnitType());
	int iFriendshipFromUnit = pUnit->getTradeInfluence(pPlot);
	int iRestingPointChange = pkUnitEntry->GetRestingPointChange();

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

	// If player has manually disabled Influence gain from quests with this City-State, don't send diplomats here.
	if (pMinorCivAI->IsQuestInfluenceDisabled(GetID()))
		return 0;

	// They captured one of our cities? Do not raise influence; we want to recapture.
	if (GetNumOurCitiesOwnedBy(kMinor.GetID()) > 0)
		return 0;

	//Return if we can't embark and they aren't on our landmass.
	if (!pCity->HasAccessToLandmass(pUnit->plot()->getLandmass()) && !CanEmbark())
		return 0;

	//are we at war with a player close to this CS? Don't go near here!
	vector<PlayerTypes> currentWars = GetPlayersAtWarWith();
	for (size_t i = 0; i < currentWars.size(); i++)
	{
		CvPlayer& kEnemy = GET_PLAYER(currentWars[i]);
		if (kEnemy.isMajorCiv() && kEnemy.GetProximityToPlayer(kMinor.GetID()) > PLAYER_PROXIMITY_CLOSE)
			return 0;
	}

	int iScore = 100;
	if (!isHuman())
	{
		EconomicAIStrategyTypes eNeedHappiness = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS");
		EconomicAIStrategyTypes eNeedHappinessCritical = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL");
		bool bNeedHappiness = (eNeedHappiness != NO_ECONOMICAISTRATEGY) ? GetEconomicAI()->IsUsingStrategy(eNeedHappiness) : false;
		bool bNeedHappinessCritical = (eNeedHappinessCritical != NO_ECONOMICAISTRATEGY) ? GetEconomicAI()->IsUsingStrategy(eNeedHappinessCritical) : false;

		// **************************
		// Approaches
		// **************************

		CivApproachTypes eApproach = GetDiplomacyAI()->GetCivApproach(kMinor.GetID());

		if (eApproach <= CIV_APPROACH_HOSTILE)
			return 0;

		if (eApproach == CIV_APPROACH_NEUTRAL)
			iScore /= 5;

		// **************************
		// Benefits to Us!
		// **************************

		CvPlayerTraits* pTraits = GetPlayerTraits();

		//DIPLOMACY - We want all of them the same!
		if (GetDiplomacyAI()->IsGoingForDiploVictory() || pTraits->IsDiplomat())
		{
			iScore *= 2;
		}

		//MILITARY - We want units and happiness!!
		if (GetDiplomacyAI()->IsGoingForWorldConquest() || pTraits->IsWarmonger() || pTraits->IsExpansionist())
		{
			if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC || pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
			{
				iScore *= 2;
			}
		}

		//SCIENCE - We want growth and units!!
		if (GetDiplomacyAI()->IsGoingForSpaceshipVictory() || pTraits->IsNerd() || pTraits->IsSmaller())
		{
			if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME || pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
			{
				iScore *= 2;
			}
			// Happiness gets a smaller bonus too.
			else if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
			{
				iScore *= 3;
				iScore /= 2;
			}
		}

		//CULTURE - We want culture and growth!!
		if (GetDiplomacyAI()->IsGoingForCultureVictory() || pTraits->IsTourism() || pTraits->IsSmaller())
		{
			if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_CULTURED || pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME)
			{
				iScore *= 2;
			}
			// Happiness gets a smaller bonus too.
			else if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
			{
				iScore *= 3;
				iScore /= 2;
			}
		}

		//RELIGIOUS MINORS - Minors can be helpful in starting, enhancing or spreading a religion.
		if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_RELIGIOUS && !GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
		{
			ReligionTypes eReligion = GetReligions()->GetStateReligion(false);

			// Do we already have a religion?
			if (eReligion != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, NO_PLAYER);

				// But not yet enhanced?
				if (!pReligion->m_bEnhanced)
				{
					iScore *= 2;
				}
				// Enhanced? Be a bit more friendly (to benefit from faith quests etc), but it's no longer a huge issue.
				else
				{
					iScore *= 3;
					iScore /= 2;
				}				
			}
			// Can we found a religion?
			else if (GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() <= 0 || pTraits->IsAlwaysReligion())
			{
				// We created a pantheon? We're on track, so keep these City-States close...
				if (GetReligions()->HasCreatedPantheon())
				{
					iScore *= 2;
				}
				// No one has founded a religion yet, so let's be more friendly...
				else if (GC.getGame().GetGameReligions()->GetNumReligionsFounded() <= 0)
				{
					iScore *= 2;
				}
				// Less interesting.
				else if (!pTraits->IsReligious())
				{
					iScore /= 2;
				}
			}
			// Far less interesting.
			else
			{
				iScore /= 4;
			}

			// Religious civ - apply extra weight if ANY religion criteria is met
			if (pTraits->IsReligious())
			{
				if (eReligion != NO_RELIGION || GetReligions()->HasCreatedPantheon())
				{
					iScore *= 2;
				}
			}
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
			// Boost this big time if we have the monopoly resource bonus.
			if (IsCSResourcesCountMonopolies())
			{
				iScore *= iResourcesWeLack;
			}
		}

		//Will they give us a WLTKD for their resource?
		int iCityLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(GetID()).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(GetID()).nextCity(&iCityLoop))
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
	}

	// Is Our Influence worth more here? Definitely take advantage of this.
	if (pMinorCivAI->IsActiveQuestForPlayer(GetID(), MINOR_CIV_QUEST_INFLUENCE))
	{
		iScore *= 5;
	}

	// Resting point increase, and our resting point isn't enough for perma-friendship yet?
	if (iRestingPointChange > 0 && pMinorCivAI->GetFriendshipAnchorWithMajor(GetID()) < pMinorCivAI->GetFriendsThreshold(GetID()))
	{
		iScore += (100 * iRestingPointChange) / max(pMinorCivAI->GetFriendsThreshold(GetID()), 1);
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
			CivApproachTypes eApproachType = GetDiplomacyAI()->GetCivApproach(eAlliedPlayer);
			CivOpinionTypes eOpinion = GetDiplomacyAI()->GetCivOpinion(eAlliedPlayer);

			// If a friendly player is allied, let's discourage going there.
			if (eApproachType == CIV_APPROACH_FRIENDLY)
			{
				iScore /= 10;
			}
			// If an enemy is allied, let's take their stuff!
			else if (eApproachType == CIV_APPROACH_HOSTILE)
			{
				iScore *= 2;
			}
			// If an ally is allied, let's leave it alone!
			if (eOpinion == CIV_OPINION_ALLY)
			{
				iScore /= 10;
			}
			// If an competitor is allied, let's fight for it!
			else if (eOpinion == CIV_OPINION_COMPETITOR)
			{
				iScore *= 2;
			}
		}
	}
	else
	{
		// Are we close to becoming an normal (60) ally and no one else ? If so, obsess away!
		if ((iFriendshipWithMinor + iFriendshipFromUnit) >= pMinorCivAI->GetAlliesThreshold(GetID()))
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
	int iDistance = GetCityDistancePathLength(pPlot) * /*3*/ GD_INT_GET(INFLUENCE_TARGET_DISTANCE_WEIGHT_VALUE);

	//Are there barbarians near the city-state? If so, careful!
	if(kMinor.GetMinorCivAI()->IsThreateningBarbariansEventActiveForPlayer(GetID()))
		iDistance *= 3;

	if (kMinor.getCapitalCity() != NULL && kMinor.getCapitalCity()->isUnderSiege())
		iDistance *= 3;

	//Let's downplay far/distant minors without full embarkation.
	if(!pCity->HasAccessToLandmass(pUnit->plot()->getLandmass()) && !GET_PLAYER(GetID()).CanCrossOcean())
		iDistance *= 3;

	//If this is way too far away, let's not penalize it too much.
	iScore -= iDistance;

	//All CSs should theoretically be valuable if we've gotten this far.
	return max(1,iScore);
}

CvPlot* CvPlayerAI::ChooseMessengerTargetPlot(CvUnit* pUnit, vector<int>* pvIgnoreCities)
{
	//this function is used for diplomat influence spread as well (embassies go through ChooseDiplomatTargetPlot)
	if(pUnit->AI_getUnitAIType() != UNITAI_MESSENGER && pUnit->AI_getUnitAIType() != UNITAI_DIPLOMAT)
		return NULL;

	CvCity* pCity = FindBestMessengerTargetCity(pUnit, pvIgnoreCities ? *pvIgnoreCities : vector<int>());
	if(pCity == NULL)
		return NULL;

	//remember this city
	if (pvIgnoreCities)
		pvIgnoreCities->push_back(pCity->GetID());

	return pCity->plot();
}

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
	if (pTargetCity && pMusician->GeneratePath(pTargetCity->plot(), iFlags, 23))
		return pMusician->GetPathLastPlot();

	//fallback, try the capital
	pTargetCity = GET_PLAYER(eTargetPlayer).getCapitalCity();
	if (pTargetCity && pMusician->GeneratePath(pTargetCity->plot(), iFlags, 23))
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
			if (iDanger > 20) //allow fog
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

			// score resource - this may be the dominant factor!
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
		if (iScore > 0)
		{
			goodPlots.push(SPlotWithScore(pPlot, iScore));
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
		if (nr1.score * 0.8f > nr2.score)
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
			int iTurns1 = pUnit->TurnsToReachTarget(nr1.pPlot, true);
			int iTurns2 = pUnit->TurnsToReachTarget(nr2.pPlot, true);
			SPlotWithScore chosen = (iTurns2<iTurns1) ? nr2 : nr1;

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
