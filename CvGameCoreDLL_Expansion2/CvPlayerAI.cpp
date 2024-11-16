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
#include "CvEnumMap.h"

#if defined(MOD_BALANCE_CORE)
#include "CvDistanceMap.h"
#endif

// Include this after all other headers.
#include "LintFree.h"

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
	m_iCurrentCitadelTargetsTurn = -1;
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
	AI_considerRaze();
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

	if (MOD_CORE_DELAYED_VISIBILITY)
	{
		// Force explicit visibility update for killed units
		if (!unitsToDelete.empty())
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
				GC.getMap().plotByIndexUnchecked(iI)->flipVisibility(getTeam());
	}

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
void CvPlayerAI::AI_unitUpdate(bool bUpdateHomelandAI)
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
		GetTacticalAI()->UpdateVisibility();
		GetHomelandAI()->Update(bUpdateHomelandAI);
		GetTacticalAI()->CleanUp();
		CvUnit::dispatchingNetMessage(false);
	}
	else
	{
		// Now let the tactical AI run.  Putting it after the operations update allows units who have
		// just been handed off to the tactical AI to get a move in the same turn they switch between
		GetTacticalAI()->Update();
		GetHomelandAI()->Update(true);
		GetTacticalAI()->CleanUp();
	}
}

void CvPlayerAI::AI_conquerCity(CvCity* pCity, bool bGift, bool bAllowSphereRemoval)
{
	if (isHuman())
		return;

	// What are our options for this city?
	PlayerTypes ePlayerToLiberate = GetPlayerToLiberate(pCity);
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
	if (bCanLiberate && GetDiplomacyAI()->IsTryingToLiberate(pCity))
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

	if (MOD_BALANCE_VP)
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

	//only city on a landmass? may be strategically important
	bool bOnlyCityOnLandmass = false;
	CvLandmass* pLandmass = GC.getMap().getLandmassById(pCity->plot()->getLandmass());
	if (pLandmass != NULL && pLandmass->getCitiesPerPlayer(GetID()) <= 1)
	{
		bOnlyCityOnLandmass = true;
		if (!IsEmpireVeryUnhappy())
			bKeepCity = true;
	}

	// If we're going for world conquest, have to keep any capitals we get
	if (!bKeepCity && pCity->IsOriginalMajorCapital() && (GetDiplomacyAI()->IsGoingForWorldConquest() || GetDiplomacyAI()->IsCloseToWorldConquest()))
	{
		bKeepCity = true;
	}

	// Want to keep city - will puppet/annex
	if (bKeepCity || (!bCanLiberate && !bCanRaze && !bAllowSphereRemoval))
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

			if (GetDiplomacyAI()->DoPossibleMajorLiberation(pCity))
				return;
		}

		// If we have bonuses for liberating cities, remove Sphere of Influence if possible
		if (bAllowSphereRemoval && GetPlayerPolicies()->GetNumericModifier(POLICYMOD_LIBERATION_BONUS) > 0)
		{
			DoLiberatePlayer(ePlayerToLiberate, pCity->GetID(), false, true);
			return;
		}

		bool bCanRevolt = IsEmpireSuperUnhappy() || (IsEmpireVeryUnhappy() && (MOD_BALANCE_VP || GetCulture()->GetPublicOpinionUnhappiness() > 0));
		if (bOnlyCityOnLandmass && !bCanRevolt)
		{
			pCity->DoCreatePuppet();
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
				pCapital->GetCityCitizens()->DoSpawnGreatPerson(eDesiredGreatPerson, true, false, MOD_GLOBAL_TRULY_FREE_GP);
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

	if (MOD_EVENTS_AI_OVERRIDE_TECH && eBestTech == NO_TECH)
	{
		int iValue = 0;
		if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_AiOverrideChooseNextTech, GetID(), true) == GAMEEVENTRETURN_VALUE)
		{
			// Defend against modder stupidity!
			if (iValue >= 0 && iValue < GC.getNumTechInfos() && !GET_TEAM(getTeam()).GetTeamTechs()->HasTech((TechTypes) iValue))
			{
				eBestTech = (TechTypes)iValue;
			}
		}
	}

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

	// Unraze and unpuppet all cities if we aren't a major civ
	if (!isMajorCiv())
	{
		int iLoop = 0;
		for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
		{
			if (pCity->IsRazing())
				unraze(pCity);

			if (pCity->IsPuppet())
				pCity->DoAnnex();
		}
		return;
	}

	// if our capital city is puppeted or being razed, annex and unraze it
	// can happen if we lose our original capital
	CvCity* pCapital = getCapitalCity();
	if (pCapital)
	{
		if (pCapital->IsRazing())
			unraze(pCapital);

		if (pCapital->IsPuppet())
		{
			pCapital->DoAnnex();
			return;
		}
	}

	// for Venice
	if (GetPlayerTraits()->IsNoAnnexing())
		return;

	// no annexing if the empire is unhappy
	if (IsEmpireUnhappy())
		return;

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
				eCourthouseType = buildingInfo->GetBuildingClassType();
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

void CvPlayerAI::AI_considerRaze()
{
	if (isHuman() || !isMajorCiv())
		return;

	AI_PERF("AI-perf.csv", "AI_ considerRaze");

	// Only raze when at risk of revolting
	int iNumCities = getNumCities();
	if (iNumCities <= 1 || GetPlayerTraits()->IsNoAnnexing())
		return;

	bool bCanRevoltIdeology = GetCulture()->GetPublicOpinionUnhappiness() > 0;
	bool bCanRevolt = IsEmpireSuperUnhappy() || (IsEmpireVeryUnhappy() && (MOD_BALANCE_VP || bCanRevoltIdeology));
	if (!bCanRevolt)
		return;

	int iCurrentHappiness = 0;
	int iCurrentHappy = 0;
	int iCurrentUnhappy = 0;
	int iThreshold = (MOD_BALANCE_VP || bCanRevoltIdeology) ? /*-10 in CP, 35 in VP*/ GD_INT_GET(VERY_UNHAPPY_THRESHOLD) : /*-20*/ GD_INT_GET(SUPER_UNHAPPY_THRESHOLD);

	// Look at our Unhappiness situation to see how much Unhappiness we need to remove
	if (MOD_BALANCE_VP)
	{
		iCurrentHappy = GetHappinessFromCitizenNeeds();
		iCurrentUnhappy = GetUnhappinessFromCitizenNeeds();
		// Protect against modder stupidity
		if (iThreshold <= 0 || iCurrentHappy <= 0)
			return;

		iCurrentHappiness = min(200, (iCurrentHappy * 100) / max(1, iCurrentUnhappy)) / 2;
	}
	else
	{
		iCurrentHappy = GetHappiness();
		iCurrentUnhappy = GetUnhappiness();
		iCurrentHappiness = iCurrentHappy - iCurrentUnhappy;
	}

	vector<CvCity*> EndangeredCities;
	vector<CvCity*> ValidCities;
	vector<PlayerTypes> vPlayersAtWarWith = GetPlayersAtWarWith();
	int iLoop = 0;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		// Already razing a city?
		if (pLoopCity->IsRazing())
		{
			iNumCities--;
			iCurrentHappy -= pLoopCity->GetLocalHappiness();
			iCurrentUnhappy -= pLoopCity->GetUnhappinessAggregated();
			continue;
		}

		// Must be occupied (no puppets)
		if (!pLoopCity->IsOccupied())
			continue;

		// Must be able to raze
		if (!canRaze(pLoopCity))
			continue;

		// City is in danger of being captured? Raze with higher priority to deny our opponents the ability to capture it.
		if (pLoopCity->IsInDangerFromPlayers(vPlayersAtWarWith))
			EndangeredCities.push_back(pLoopCity);

		ValidCities.push_back(pLoopCity);
	}

	if (iNumCities <= 1)
		return;

	// Recalculate happiness to see if we still have a problem ... possible we already have enough cities burning
	if (MOD_BALANCE_VP)
	{
		iCurrentHappiness = min(200, (iCurrentHappy * 100) / max(1, iCurrentUnhappy)) / 2;
		if (iCurrentHappiness >= iThreshold)
			return;
	}
	else
	{
		iCurrentHappiness = iCurrentHappy - iCurrentUnhappy;
		if (iCurrentHappiness > iThreshold)
			return;
	}

	// Go through our list of cities and see which ones we might raze
	CvWeightedVector<CvCity*> RazeCandidates;
	for (uint ui = 0; ui < ValidCities.size(); ui++)
	{
		CvCity* pLoopCity = ValidCities[ui];

		// Would we gain Happiness from razing this city?
		int iHappinessFromRazing = AI_computeHappinessFromRazing(pLoopCity, iCurrentHappy, iCurrentUnhappy);
		if (iHappinessFromRazing <= 0)
			continue;

		RazeCandidates.push_back(pLoopCity, iHappinessFromRazing);
	}

	RazeCandidates.StableSortItems();
	bool bDone = false;

	// First priority: burn down endangered cities that we haven't built a courthouse in
	for (int i = 0; i < RazeCandidates.size(); i++)
	{
		CvCity* pLoopCity = RazeCandidates.GetElement(i);
		if (!pLoopCity->IsNoOccupiedUnhappiness() && std::find(EndangeredCities.begin(), EndangeredCities.end(), pLoopCity) != EndangeredCities.end())
		{
			iNumCities--;
			iCurrentHappy -= pLoopCity->GetLocalHappiness();
			iCurrentUnhappy -= pLoopCity->GetUnhappinessAggregated();

			// Recalculate Happiness and test if we've met our goal
			if (MOD_BALANCE_VP)
			{
				iCurrentHappiness = min(200, (iCurrentHappy * 100) / max(1, iCurrentUnhappy)) / 2;
				if (iCurrentHappiness >= iThreshold)
					bDone = true;
			}
			else
			{
				iCurrentHappiness = iCurrentHappy - iCurrentUnhappy;
				if (iCurrentHappiness > iThreshold)
					bDone = true;
			}

			pLoopCity->doTask(TASK_RAZE);

			if (bDone || iNumCities == 1)
				return;
		}
	}
	// Second priority: burn down non-endangered cities that we haven't built a courthouse in
	for (int i = 0; i < RazeCandidates.size(); i++)
	{
		CvCity* pLoopCity = RazeCandidates.GetElement(i);
		if (pLoopCity->IsRazing())
			continue;

		if (!pLoopCity->IsNoOccupiedUnhappiness())
		{
			iNumCities--;
			iCurrentHappy -= pLoopCity->GetLocalHappiness();
			iCurrentUnhappy -= pLoopCity->GetUnhappinessAggregated();

			// Recalculate Happiness and test if we've met our goal
			if (MOD_BALANCE_VP)
			{
				iCurrentHappiness = min(200, (iCurrentHappy * 100) / max(1, iCurrentUnhappy)) / 2;
				if (iCurrentHappiness >= iThreshold)
					bDone = true;
			}
			else
			{
				iCurrentHappiness = iCurrentHappy - iCurrentUnhappy;
				if (iCurrentHappiness > iThreshold)
					bDone = true;
			}

			pLoopCity->doTask(TASK_RAZE);

			if (bDone || iNumCities == 1)
				return;
		}
	}
	// Third priority: burn down endangered cities that we HAVE built a courthouse in
	for (int i = 0; i < RazeCandidates.size(); i++)
	{
		CvCity* pLoopCity = RazeCandidates.GetElement(i);
		if (pLoopCity->IsRazing())
			continue;

		if (std::find(EndangeredCities.begin(), EndangeredCities.end(), pLoopCity) != EndangeredCities.end())
		{
			iNumCities--;
			iCurrentHappy -= pLoopCity->GetLocalHappiness();
			iCurrentUnhappy -= pLoopCity->GetUnhappinessAggregated();

			// Recalculate Happiness and test if we've met our goal
			if (MOD_BALANCE_VP)
			{
				iCurrentHappiness = min(200, (iCurrentHappy * 100) / max(1, iCurrentUnhappy)) / 2;
				if (iCurrentHappiness >= iThreshold)
					bDone = true;
			}
			else
			{
				iCurrentHappiness = iCurrentHappy - iCurrentUnhappy;
				if (iCurrentHappiness > iThreshold)
					bDone = true;
			}

			pLoopCity->doTask(TASK_RAZE);

			if (bDone || iNumCities == 1)
				return;
		}
	}
	// Fourth priority: burn down other cities
	for (int i = 0; i < RazeCandidates.size(); i++)
	{
		CvCity* pLoopCity = RazeCandidates.GetElement(i);
		if (pLoopCity->IsRazing())
			continue;

		iNumCities--;
		iCurrentHappy -= pLoopCity->GetLocalHappiness();
		iCurrentUnhappy -= pLoopCity->GetUnhappinessAggregated();

		// Recalculate Happiness and test if we've met our goal
		if (MOD_BALANCE_VP)
		{
			iCurrentHappiness = min(200, (iCurrentHappy * 100) / max(1, iCurrentUnhappy)) / 2;
			if (iCurrentHappiness >= iThreshold)
				bDone = true;
		}
		else
		{
			iCurrentHappiness = iCurrentHappy - iCurrentUnhappy;
			if (iCurrentHappiness > iThreshold)
				bDone = true;
		}

		pLoopCity->doTask(TASK_RAZE);

		if (bDone || iNumCities == 1)
			return;
	}
}

int CvPlayerAI::AI_computeHappinessFromRazing(CvCity* pCity, int iCurrentHappy, int iCurrentUnhappy)
{
	int iCurrentHappiness = 0;
	if (MOD_BALANCE_VP)
	{
		iCurrentHappiness = min(200, (iCurrentHappy * 100) / max(1, iCurrentUnhappy)) / 2;
	}
	else
	{
		iCurrentHappiness = iCurrentHappy - iCurrentUnhappy;
	}

	iCurrentHappy -= pCity->GetLocalHappiness();
	iCurrentUnhappy -= pCity->GetUnhappinessAggregated();

	int iFutureHappiness = 0;
	if (MOD_BALANCE_VP)
	{
		iFutureHappiness = min(200, (iCurrentHappy * 100) / max(1, iCurrentUnhappy)) / 2;
	}
	else
	{
		iFutureHappiness = iCurrentHappy - iCurrentUnhappy;
	}

	return iFutureHappiness - iCurrentHappiness;
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
	return pMilitaryAI->IsUsingStrategy(eWarMobilizationStrategy);
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
	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE)
	{
		int iNumImprovement = getImprovementCount(eManufactory);
		if(iNumImprovement <= iFlavor)
		{
			eDirective = GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
		}
	}

	if (eDirective == NO_GREAT_PEOPLE_DIRECTIVE_TYPE && (GC.getGame().getGameTurn() - pGreatEngineer->getGameTurnCreated()) >= (/*5*/ GD_INT_GET(AI_HOMELAND_GREAT_PERSON_TURNS_TO_WAIT) - GetCurrentEra()))
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
	bool bHasGeneralNegation = false;
	if (MOD_ERA_RESTRICTED_GENERALS)
	{
		string GGNegationPromotions[4] = {
			"PROMOTION_NEGATE_GENERAL",
			"PROMOTION_NEGATE_GENERAL_S",
			"PROMOTION_NEGATE_GENERAL_P",
			"PROMOTION_NEGATE_GENERAL_SP",
		};

		for (int i = 0; i < 4; i++)
		{
			PromotionTypes eNegationPromotion = static_cast<PromotionTypes>(GC.getInfoTypeForString(GGNegationPromotions[i].c_str(), true));
			if (pGreatGeneral->isHasPromotion(eNegationPromotion))
				bHasGeneralNegation = true;
		}

		if (bHasGeneralNegation)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("%s is obsolete", pGreatGeneral->getName().GetCString());
				GetHomelandAI()->LogHomelandMessage(strLogString);
			}
		}
	}

	// Units in armies are always field commanders
	if (!bHasGeneralNegation && pGreatGeneral->getArmyID() != -1)
		return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;

	// Always stick to culture bomb if it has already decided to do so (it can be reset in CvHomelandAI)
	if (pGreatGeneral->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
		return GREAT_PEOPLE_DIRECTIVE_USE_POWER;

	// We might have multiple generals... First get an overview
	int iCommanders = 0;
	int iCitadels = 0;
	int iLoop = 0;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit != pGreatGeneral && pLoopUnit->IsGreatGeneral())
		{
			switch (pLoopUnit->GetGreatPeopleDirective())
			{
			case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
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

	if (!bHasGeneralNegation)
	{
		// During war we want field commanders
		int iWars = static_cast<int>(GetPlayersAtWarWith().size());
		// Just a rough estimation
		int iPotentialArmies = max(1, GetMilitaryAI()->GetNumLandUnits() - getNumCities() * 3) / 13;

		int iDesiredNumCommanders = max(1, (iWars + iPotentialArmies) / 2);
		if (iCommanders <= iDesiredNumCommanders || pGreatGeneral->getArmyID() != -1 || pGreatGeneral->IsRecentlyDeployedFromOperation())
			return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
	}

	// Build one citadel at a time
	if (iCitadels == 0)
	{
		CvPlot* pTargetPlot = FindBestCultureBombPlot(pGreatGeneral, vector<CvPlot*>());
		if (pTargetPlot)
			return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	// Certain generals can build non-culture bomb improvements
	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		BuildTypes eBuild = static_cast<BuildTypes>(iI);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if (!pkBuildInfo)
			continue;

		ImprovementTypes eImprovement = static_cast<ImprovementTypes>(pkBuildInfo->getImprovement());
		if (eImprovement == NO_IMPROVEMENT)
			continue;

		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (!pkImprovementInfo)
			continue;

		if (pkImprovementInfo->GetCultureBombRadius() <= 0 && pGreatGeneral->canBuild(NULL, eBuild))
			return GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}

	// Default
	return bHasGeneralNegation ? NO_GREAT_PEOPLE_DIRECTIVE_TYPE : GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
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

		//Let's use our prophets for improvements instead of wasting them on conversion.
		int iNumImprovement = getImprovementCount(eHolySite);
		if (iNumImprovement <= iFlavor || GetReligionAI()->ChooseProphetConversionCity(pUnit) == NULL || GetPlayerTraits()->IsProphetFervor())
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
	bool bHasAdmiralNegation = false;
	if (MOD_ERA_RESTRICTED_GENERALS)
	{
		string GANegationPromotions[4] = {
			"PROMOTION_NEGATE_ADMIRAL",
			"PROMOTION_NEGATE_ADMIRAL_S",
			"PROMOTION_NEGATE_ADMIRAL_P",
			"PROMOTION_NEGATE_ADMIRAL_SP",
		};

		for (int i = 0; i < 4; i++)
		{
			PromotionTypes eNegationPromotion = static_cast<PromotionTypes>(GC.getInfoTypeForString(GANegationPromotions[i].c_str(), true));
			if (pGreatAdmiral->isHasPromotion(eNegationPromotion))
				bHasAdmiralNegation = true;
		}

		if (bHasAdmiralNegation)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvString strLogString;
				strLogString.Format("%s is obsolete", pGreatAdmiral->getName().GetCString());
				GetHomelandAI()->LogHomelandMessage(strLogString);
			}
		}
	}

	// Units in armies are always field commanders
	if (!bHasAdmiralNegation && pGreatAdmiral->getArmyID() != -1)
		return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;

	int iCommanders = 0;
	int iLoop = 0;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit != pGreatAdmiral && pLoopUnit->IsGreatAdmiral())
		{
			switch (pLoopUnit->GetGreatPeopleDirective())
			{
			case NO_GREAT_PEOPLE_DIRECTIVE_TYPE:
			case GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT:
			case GREAT_PEOPLE_DIRECTIVE_USE_POWER:
				break;
			case GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND:
				iCommanders++;
				break;
			default:
				UNREACHABLE(); // Great admirals shouldn't have directives other than these.
			}
		}
	}

	if (!bHasAdmiralNegation)
	{
		// During war we want field commanders
		int iWars = IsAtWar() ? 2 : 0;
		// Just a rough estimation
		int iPotentialArmies = max(1, GetMilitaryAI()->GetNumNavalUnits() - getNumCities()) / 5;

		int iDesiredNumCommanders = max(1, (iWars + iPotentialArmies) / 2);
		if (iCommanders <= iDesiredNumCommanders || pGreatAdmiral->getArmyID() != -1 || pGreatAdmiral->IsRecentlyDeployedFromOperation())
			return GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND;
	}

	// Can this admiral build improvements?
	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		BuildTypes eBuild = static_cast<BuildTypes>(iI);
		if (pGreatAdmiral->canBuild(NULL, eBuild))
			return GREAT_PEOPLE_DIRECTIVE_CONSTRUCT_IMPROVEMENT;
	}

	// We're unhappy or have too many admirals; expend some!
	if (pGreatAdmiral->canGetFreeLuxury())
	{
		int iThreshold = IsEmpireUnhappy() ? 0 : GetNumEffectiveCoastalCities() / 2 + 1;
		if (iCommanders > iThreshold)
			return GREAT_PEOPLE_DIRECTIVE_USE_POWER;
	}

	return bHasAdmiralNegation ? GREAT_PEOPLE_DIRECTIVE_FIELD_COMMAND : NO_GREAT_PEOPLE_DIRECTIVE_TYPE;
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
		//what would be a sane limit?
		//FIXME: look at the politics of potential targets ...
		return static_cast<bool>(GetNumPuppetCities() > max(2,GC.getGame().GetNumMinorCivsEver()-3));
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
	for (int iI = MAX_MAJOR_CIVS; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kPlayer.isMinorCiv() || !kPlayer.isAlive())
			continue;

		if (kPlayer.IsAtWarWith(GetID()))
			continue;

		//FIXME: if somebody is protecting them, look at our relations to that particular player
		int iPoliticalReduction = 0;
		if (kPlayer.GetMinorCivAI()->IsProtectedByAnyMajor())
			iPoliticalReduction = 40;

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

				//political fudge factor
				iScore -= (iScore*iPoliticalReduction)/100;

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
	for (int iI = MAX_MAJOR_CIVS; iI < MAX_PLAYERS; iI++)
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

//	--------------------------------------------------------------------------------
/// planner for AI spaceship production: returns the cities in which the currently available spaceship parts should be built in order to achieve SV as early as possible
const vector<CvCity*> CvPlayerAI::GetBestCitiesForSpaceshipParts()
{
	const vector<CvCity*> vpEmpty;

	CvCity* pCapital = getCapitalCity();
	if (!pCapital)
		return vpEmpty;

	int iNumSpaceshipPartsStillNeeded = 6 - GET_TEAM(getTeam()).GetSSProjectCount(/*bIncludeApollo*/ false);
	int iNumSpaceshipPartsBuildableNow = GetNumSpaceshipPartsBuildableNow(/*bIncludeCurrentlyInProduction*/ true);

	if (iNumSpaceshipPartsStillNeeded == 0 || iNumSpaceshipPartsBuildableNow == 0)
		return vpEmpty;

	// get unit type for one of the spaceship parts
	UnitTypes eSpaceshipUnit = NO_UNIT;
	for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(static_cast<UnitTypes>(iUnitLoop));
		if (pkUnitInfo && pkUnitInfo->GetSpaceshipProject() != NO_PROJECT)
		{
			eSpaceshipUnit = static_cast<UnitTypes>(iUnitLoop);
			break;
		}
	}

	CvString logMsg;
	// pre-selection: get cities we consider for spaceship part production
	vector<CvCity*> vCitiesToConsider;
	VictoryTypes eCultureVictory = (VictoryTypes)GC.getInfoTypeForString("VICTORY_CULTURAL", true);
	int iLoop = 0;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->IsPuppet())
			continue;

		if (pLoopCity->isUnderSiege())
			continue;

		// city too far away?
		if (!(pLoopCity->CanAirlift() && pCapital->CanAirlift()) && plotDistance(pLoopCity->getX(), pLoopCity->getY(), pCapital->getX(), pCapital->getY()) > 40)
			continue;

		// building a world wonder?
		if (pLoopCity->IsBuildingWorldWonder())
			continue;

		// building utopia project?
		if (pLoopCity->getProductionProject() != NO_PROJECT)
		{
			CvProjectEntry* pkProjectInfo = GC.getProjectInfo(pLoopCity->getProductionProject());
			if (pkProjectInfo && pkProjectInfo->GetVictoryPrereq() == eCultureVictory && eCultureVictory != NO_VICTORY)
			{
				continue;
			}
		}

		vCitiesToConsider.push_back(pLoopCity);
	}

	int iNumCitiesToConsider = vCitiesToConsider.size();
	if (iNumCitiesToConsider == 0)
		return vpEmpty;

	// we want to assign spaceship part to cities in such a way that the total number of turns to bring all spaceship parts to the capital is minimized
	// to do this, we loop through the spaceship parts we still need and assign each of it to the best city. one city can be considered for multiple parts.
	// the total number of production turns needed for each city will be stored in vTotalTurns

	// first, we get some production data for all the cities we're considering
	vector<int>vProductionTurnsForNewSpaceshipPart(iNumCitiesToConsider, 0); // turns for building a spaceship part that we haven't started yet
	vector<int>vTransportTurnsToCapital(iNumCitiesToConsider, 0); // estimated number of turns we need to get a spaceship part to the capital
	vector<int>vExistingProgress(iNumCitiesToConsider, 0); // how many turns of progress towards a spaceship part do we have?
	vector<int>vIsProducingSpaceshipPart(iNumCitiesToConsider, false); // are we currently producing a spaceship part?
	vector<int>vTotalTurns(iNumCitiesToConsider, 0);

	for (int i = 0; i < iNumCitiesToConsider; i++)
	{
		CvCity* pLoopCity = vCitiesToConsider[i];

		// temporarily change city focus to production before calculating unit production turns
		CityAIFocusTypes eCurrentFocus = pLoopCity->GetCityCitizens()->GetFocusType();
		pLoopCity->GetCityCitizens()->SetFocusType(CITY_AI_FOCUS_TYPE_PRODUCTION, true);
		vProductionTurnsForNewSpaceshipPart[i] = pLoopCity->getUnitTotalProductionTurns(eSpaceshipUnit);
		// change city focus back to what it was before
		pLoopCity->GetCityCitizens()->SetFocusType(eCurrentFocus, true);

		int iTransportTurns = 0;
		if (!pLoopCity->isCapital())
		{
			// rough approximation
			iTransportTurns = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pCapital->getX(), pCapital->getY()) / 15;
			if (!pLoopCity->HasAccessToLandmass(pCapital->plot()->getLandmass()))
			{
				iTransportTurns += 2;
			}
			if (pLoopCity->CanAirlift() && pCapital->CanAirlift())
			{
				iTransportTurns = min(iTransportTurns, 1);
			}
		}
		vTransportTurnsToCapital[i] = iTransportTurns;

		int iTurnsExistingProgress = 0;
		for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
		{
			UnitTypes eUnitLoop = (UnitTypes)iUnitLoop;
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitLoop);
			if (pkUnitInfo && pkUnitInfo->GetSpaceshipProject() != NO_PROJECT)
			{
				if (pLoopCity->getUnitProduction(eUnitLoop) > 0)
				{
					iTurnsExistingProgress = max(iTurnsExistingProgress, vProductionTurnsForNewSpaceshipPart[i] - pLoopCity->getProductionTurnsLeft(eUnitLoop, 0));
					if (pLoopCity->getProductionUnit() == eUnitLoop)
					{
						vIsProducingSpaceshipPart[i] = true;
					}
				}
			}
		}
		vExistingProgress[i] = iTurnsExistingProgress;
		// if we have existing progress towards a spaceship part in the city, we set vTotalTurns initially to minus that value
		// this will ensure that the existing progress is counted towards the first spaceship part built in the city, but not towards any future ones
		vTotalTurns[i] = -iTurnsExistingProgress;
	}

	if (GC.getLogging())
	{
		logMsg.Format("Spaceship planning. Parts needed: %d, Parts we can build currently: %d", iNumSpaceshipPartsStillNeeded, iNumSpaceshipPartsBuildableNow);
		LogSpaceshipPlanMessage(logMsg);
		logMsg.Format("Considering the following cities:");
		LogSpaceshipPlanMessage(logMsg);
		for (int i = 0; i < iNumCitiesToConsider; i++)
		{
			if (vIsProducingSpaceshipPart[i])
			{
				logMsg.Format(" *%s: Turns per SS part: %d, Existing progress: %d, Transport turns: %d", vCitiesToConsider[i]->getName().GetCString(), vProductionTurnsForNewSpaceshipPart[i], -vTotalTurns[i], vTransportTurnsToCapital[i]);
			}
			else
			{
				logMsg.Format("  %s: Turns per SS part: %d, Existing progress: %d, Transport turns: %d", vCitiesToConsider[i]->getName().GetCString(), vProductionTurnsForNewSpaceshipPart[i], -vTotalTurns[i], vTransportTurnsToCapital[i]);
			}
			LogSpaceshipPlanMessage(logMsg);
		}
	}

	for (int j = 0; j < iNumSpaceshipPartsStillNeeded; j++)
	{
		// at this point, vTurnsTotal contains for all cities the number of turns needed for the other spaceship parts
		// we want to find the city such that, if we added the spaceship part we're considering right now to the city queue, the number of turns we need until all parts are produced would be as low as possible
		int iBestCity = -1;
		int iNumberOfTurnsIfBestCityUsed = INT_MAX;
		for (int i = 0; i < iNumCitiesToConsider; i++)
		{
			// how long would it take if we used this city for the spaceship part?
			// add up turns for other spaceship parts we want to produce in this city and turns of the newly added spaceship part
			int iNumberOfTurnsIfThisCityUsed = vTotalTurns[i] + vProductionTurnsForNewSpaceshipPart[i];
			// add transport costs for the last item
			iNumberOfTurnsIfThisCityUsed += vTransportTurnsToCapital[i];
			// is this city better than the ones considered previously?
			if (iNumberOfTurnsIfThisCityUsed < iNumberOfTurnsIfBestCityUsed)
			{
				iBestCity = i;
				iNumberOfTurnsIfBestCityUsed = iNumberOfTurnsIfThisCityUsed;
			}
		}
		// add production turns to vTotalTurns for the city we've chosen
		vTotalTurns[iBestCity] += vProductionTurnsForNewSpaceshipPart[iBestCity];

		if (GC.getLogging())
		{
			logMsg.Format("Choosing city for part #%d: %s. Total turns until part finished: %d", j + 1, vCitiesToConsider[iBestCity]->getName().GetCString(), vTotalTurns[iBestCity]);
			LogSpaceshipPlanMessage(logMsg);
		}
	}

	// after having assigned all spaceship parts to cities, the cities we want to use for spaceship parts are the ones for which vTotalTurns is positive. spaceship parts will only be built in those
	// for the parts we can build right now, we choose only the cities with the *highest* number of turns. this will make sure the better cities won't be blocked when the other parts will become available later
	// existing progress is added to the weights and cities that are already producing a spaceship part get an additional bonus, this prevents the AI from switching back and forth between cities too often
	CvWeightedVector<CvCity*> vCitiesSortedByTurns;
	for (int i = 0; i < iNumCitiesToConsider; i++)
	{
		if (vTotalTurns[i] > 0)
		{
			vCitiesSortedByTurns.push_back(vCitiesToConsider[i], vTotalTurns[i] + vExistingProgress[i] + (vIsProducingSpaceshipPart[i] ? 2 : 0));
		}
	}
	vCitiesSortedByTurns.StableSortItems();

	// for the parts we can produce right now, choose the worst cities.
	int iNumCitiesForSpaceship = min(vCitiesSortedByTurns.size(), iNumSpaceshipPartsBuildableNow);
	vector<CvCity*> vpCitiesForSpaceshipParts(iNumCitiesForSpaceship);
	for (int i = 0; i < iNumCitiesForSpaceship; i++)
	{
		vpCitiesForSpaceshipParts[i] = vCitiesSortedByTurns.GetElement(i);
	}

	if (GC.getLogging())
	{
		logMsg.Format("All spaceship parts assigned to cities. Cities selected:  ( '***': for currently available parts)");
		LogSpaceshipPlanMessage(logMsg);
		for (int k = 0; k < vCitiesSortedByTurns.size(); k++)
		{
			if (k < iNumSpaceshipPartsBuildableNow)
			{
				logMsg.Format("  ***  %s, Weight: %d", vCitiesSortedByTurns.GetElement(k)->getName().GetCString(), vCitiesSortedByTurns.GetWeight(k));
			}
			else
			{
				logMsg.Format("  %s, Weight: %d", vCitiesSortedByTurns.GetElement(k)->getName().GetCString(), vCitiesSortedByTurns.GetWeight(k));
			}
			LogSpaceshipPlanMessage(logMsg);
		}
	}

	return vpCitiesForSpaceshipParts;
}

//	--------------------------------------------------------------------------------
/// if going for spaceship victory, the results from GetBestCitiesForSpaceshipParts are used to overwrite normal AI city production selection
void CvPlayerAI::AI_doSpaceshipProduction()
{
	if (isHuman() || isMinorCiv() || !GetDiplomacyAI()->IsGoingForSpaceshipVictory())
		return;

	if (GetNumSpaceshipPartsBuildableNow(true) == 0)
		return;

	// calculate cities to build spaceship parts in
	const vector<CvCity*> vBestCitiesForSpaceshipParts = GetBestCitiesForSpaceshipParts();

	// cancel spaceship part production in cities that are not considered the best cities (have to do this first to make the parts available)
	CvString strOutBuf;
	int iLoop = 0;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isProductionSpaceshipPart())
		{
			if (find(vBestCitiesForSpaceshipParts.begin(), vBestCitiesForSpaceshipParts.end(), pLoopCity) == vBestCitiesForSpaceshipParts.end())
			{
				// cancel current production
				pLoopCity->clearOrderQueue();
				if (GC.getLogging() && GC.getAILogging())
				{
					strOutBuf.Format("%s, NOT BEST CITY, CANCELING SPACESHIP PRODUCTION", pLoopCity->getName().GetCString());
					LogSpaceshipPlanMessage(strOutBuf);
				}
			}
		}
	}

	// which of the best cities are not building spaceship parts?
	vector<CvCity*> vCitiesForAvailableSpaceshipParts;
	int iLoop2 = 0;
	for (CvCity* pLoopCity = firstCity(&iLoop2); pLoopCity != NULL; pLoopCity = nextCity(&iLoop2))
	{
		if (find(vBestCitiesForSpaceshipParts.begin(), vBestCitiesForSpaceshipParts.end(), pLoopCity) != vBestCitiesForSpaceshipParts.end())
		{
			// is the city producing a spaceship unit? continue doing so
			// otherwise, cancel whatever it is the city is producing
			if (!pLoopCity->isProductionSpaceshipPart())
			{
				pLoopCity->clearOrderQueue();
				vCitiesForAvailableSpaceshipParts.push_back(pLoopCity);
			}
		}
	}

	if (vCitiesForAvailableSpaceshipParts.size() == 0)
		return;

	// if a city has existing progress towards a spaceship part, it should build that part
	for (size_t iLoopAvailable = 0; iLoopAvailable < vCitiesForAvailableSpaceshipParts.size(); iLoopAvailable++)
	{
		CvCity* pLoopCity = vCitiesForAvailableSpaceshipParts[iLoopAvailable];
		UnitTypes ePreferredUnit = NO_UNIT;
		UnitAITypes ePreferredUnitAI = NO_UNITAI;
		for (int iLoopUnit = 0; iLoopUnit < GC.GetGameUnits()->GetNumUnits(); iLoopUnit++)
		{
			UnitTypes eLoopUnit = (UnitTypes)iLoopUnit;
			CvUnitEntry* pkLoopUnitInfo = GC.getUnitInfo(eLoopUnit);
			if (pkLoopUnitInfo && pkLoopUnitInfo->GetSpaceshipProject() != NO_PROJECT && pLoopCity->canTrain(eLoopUnit))
			{
				if (pLoopCity->getUnitProduction(eLoopUnit) > 0)
				{
					ePreferredUnit = eLoopUnit;
					ePreferredUnitAI = pkLoopUnitInfo->GetDefaultUnitAIType();
					break;
				}
			}
		}
		if (ePreferredUnit != NO_UNIT)
		{
			pLoopCity->pushOrder(ORDER_TRAIN, ePreferredUnit, ePreferredUnitAI, false, true, false, false);

			if (GC.getLogging() && GC.getAILogging())
			{
				CvCityBuildable buildable;
				buildable.m_eBuildableType = CITY_BUILDABLE_UNIT;
				buildable.m_iIndex = ePreferredUnit;
				buildable.m_iTurnsToConstruct = pLoopCity->getProductionTurnsLeft(ePreferredUnit, 0);
				pLoopCity->GetCityStrategyAI()->LogCityProduction(buildable, false);
				
				strOutBuf.Format("%s, CONTINUING INTERRUPTED SPACESHIP PART CONSTRUCTION - Started %s, Turns: %d", pLoopCity->getName().GetCString(), GC.getUnitInfo(ePreferredUnit)->GetDescription(), buildable.m_iTurnsToConstruct);
				LogSpaceshipPlanMessage(strOutBuf);
			}
		}
	}

	// assign any remaining spaceship parts to the other cities
	for (size_t iLoopAvailable = 0; iLoopAvailable < vCitiesForAvailableSpaceshipParts.size(); iLoopAvailable++)
	{
		CvCity* pLoopCity = vCitiesForAvailableSpaceshipParts[iLoopAvailable];
		if (pLoopCity->getProductionUnit() != NO_UNIT)
			continue;

		for (int iLoopUnit = 0; iLoopUnit < GC.GetGameUnits()->GetNumUnits(); iLoopUnit++)
		{
			UnitTypes eLoopUnit = (UnitTypes)iLoopUnit;
			CvUnitEntry* pkLoopUnitInfo = GC.getUnitInfo(eLoopUnit);
			if (pkLoopUnitInfo && pkLoopUnitInfo->GetSpaceshipProject() != NO_PROJECT && pLoopCity->canTrain(eLoopUnit))
			{
				pLoopCity->pushOrder(ORDER_TRAIN, eLoopUnit, pkLoopUnitInfo->GetDefaultUnitAIType(), false, true, false, false);

				if (GC.getLogging() && GC.getAILogging())
				{
					CvCityBuildable buildable;
					buildable.m_eBuildableType = CITY_BUILDABLE_UNIT;
					buildable.m_iIndex = eLoopUnit;
					buildable.m_iTurnsToConstruct = pLoopCity->getProductionTurnsLeft(eLoopUnit, 0);

					pLoopCity->GetCityStrategyAI()->LogCityProduction(buildable, false);

					strOutBuf.Format("%s, STARTING SPACESHIP PART CONSTRUCTION - Started %s, Turns: %d", pLoopCity->getName().GetCString(), pkLoopUnitInfo->GetDescription(), buildable.m_iTurnsToConstruct);
					LogSpaceshipPlanMessage(strOutBuf);
				}

				break;
			}
		}
	}
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

	// Don't compete with our teammates for Influence.
	PlayerTypes eAlliedPlayer = pMinorCivAI->GetAlly();
	if (eAlliedPlayer != NO_PLAYER && eAlliedPlayer != GetID() && GET_PLAYER(eAlliedPlayer).getTeam() == getTeam() && GET_PLAYER(eAlliedPlayer).getNumCities() > 0)
		return 0;

	// They captured one of our cities? Do not raise influence; we want to recapture.
	if (GetNumOurCitiesOwnedBy(kMinor.GetID()) > 0)
		return 0;

	//Return if we can't embark and they aren't on our landmass.
	if (!pCity->HasAccessToLandmass(pUnit->plot()->getLandmass()) && !CanEmbark())
		return 0;

	//Is there a proposal (not resolution) involving a Sphere of Influence or Open Door?
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	if (pLeague != NULL) 
	{
		for (EnactProposalList::iterator it = pLeague->m_vEnactProposals.begin(); it != pLeague->m_vEnactProposals.end(); ++it)
		{
			if ((it->GetEffects()->bSphereOfInfluence || it->GetEffects()->bOpenDoor) && it->GetProposerDecision()->GetDecision() == pCity->getOwner())
			{
				return 0;
			}
		}
	}

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

priority_queue<SPlotWithScore> CvPlayerAI::GetBestCultureBombPlots(const UnitTypes eUnit, const vector<CvPlot*>& vPlotsToAvoid, bool bCheckDanger)
{
	priority_queue<SPlotWithScore> goodPlots;
	if (eUnit == NO_UNIT)
		return goodPlots;

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);

	// Find the improvement (we assume there's only one) that can culture bomb
	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		if (!pkUnitInfo->GetBuilds(iI))
			continue;

		BuildTypes eBuild = static_cast<BuildTypes>(iI);
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if (!pkBuildInfo)
			continue;

		ImprovementTypes eImprovement = static_cast<ImprovementTypes>(pkBuildInfo->getImprovement());
		CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
		if (!pkImprovementInfo)
			continue;

		if (pkImprovementInfo->GetCultureBombRadius() <= 0)
			continue;

		int iRange = range(pkImprovementInfo->GetCultureBombRadius() + GetCultureBombBoost(), 1, 5);

		// Build a list of valid candidate plots, and score them
		for (int iJ = 0; iJ < GC.getMap().numPlots(); iJ++)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iJ);
			if (!pPlot)
				continue;

			// Don't consider plots we already targeted
			bool bTooClose = false;
			for (vector<CvPlot*>::const_iterator it = vPlotsToAvoid.begin(); it != vPlotsToAvoid.end(); ++it)
			{
				if (plotDistance(**it, *pPlot) < iRange)
					bTooClose = true;
			}
			if (bTooClose)
				continue;

			// Don't replace other Great Person improvements
			ImprovementTypes eExistingImprovement = pPlot->getImprovementType();
			if (eExistingImprovement != NO_IMPROVEMENT)
			{
				CvImprovementEntry* pkImprovementInfo2 = GC.getImprovementInfo(eExistingImprovement);
				if (pkImprovementInfo2->IsCreatedByGreatPerson())
					continue;
			}

			// Can we build it at all?
			if (!pPlot->canBuild(eBuild, GetID()))
				continue;

			// Not if we're about to give up the city
			CvCity* pCity = pPlot->getOwningCity();
			if (pCity && pCity->IsRazing())
				continue;

			// This plot should be more defensive with the improvement
			// The function returns a value in the thousands; we only want a tenth of it
			int iPlotScore = pPlot->GetDefenseBuildValue(GetID(), eBuild, eImprovement) / 10;
			if (iPlotScore <= 0)
				continue;

			// Can the plot be worked?
			CvCity* pClosestCity = GetClosestCityByPlots(pPlot);
			if (pClosestCity && pClosestCity->getWorkPlotDistance() >= GetCityDistanceInPlots(pPlot))
				iPlotScore += 100;

			int iStealScore = 0;
			bool bIncludeEnemyPlot = false;
			for (int iK = 0; iK < RING_PLOTS[iRange]; iK++)
			{
				CvPlot* pAdjacentPlot = iterateRingPlots(pPlot, iK);
				if (!pAdjacentPlot)
					continue;

				// We can't steal city plots
				if (pAdjacentPlot->isCity())
					continue;

				PlayerTypes eOwner = pAdjacentPlot->getOwner();

				// We already own this plot
				if (eOwner == GetID())
					continue;

				// We shouldn't steal from them
				if (GetDiplomacyAI()->IsBadTheftTarget(eOwner, THEFT_TYPE_CULTURE_BOMB))
				{
					iStealScore = 0;
					break;
				}

				// It's dangerous to go there - only check radius 1
				if (bCheckDanger && iK < RING1_PLOTS && GetPossibleAttackers(*pAdjacentPlot, getTeam()).size() > 0)
				{
					iStealScore = 0;
					break;
				}

				int iMultiplier = 1;
				int iTempScore = 10;

				// We're stealing a choke point - more score if it's from our enemy!
				if (pAdjacentPlot->IsChokePoint())
				{
					iTempScore += 50;
					if (eOwner != NO_PLAYER)
						iTempScore += 50;
				}

				// There's a resource on the plot
				ResourceTypes eResource = pAdjacentPlot->getResourceType(getTeam());
				if (eResource != NO_RESOURCE)
				{
					CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
					ResourceUsageTypes eUsage = pkResourceInfo->getResourceUsage();
					if (eUsage == RESOURCEUSAGE_STRATEGIC || eUsage == RESOURCEUSAGE_LUXURY)
					{
						iTempScore += 50;
						if (eOwner != NO_PLAYER)
							iTempScore += 50;
					}
				}

				// These only apply when we're stealing from other players
				if (eOwner != NO_PLAYER)
				{
					bIncludeEnemyPlot = true;
					if (GET_PLAYER(eOwner).isMinorCiv())
					{
						iMultiplier = 3;

						// We're stealing someone else's embassy
						if (pAdjacentPlot->IsImprovementEmbassy())
						{
							// Not our own!
							if (GET_PLAYER(pAdjacentPlot->GetPlayerThatBuiltImprovement()).getTeam() == getTeam())
							{
								iStealScore = 0;
								break;
							}

							iTempScore += 300;
						}
					}
					// For major civs (and barbarians), it depends on how much we hate them
					else
					{
						CivOpinionTypes eOpinion = GetDiplomacyAI()->GetCivOpinion(eOwner);
						if (eOpinion < CIV_OPINION_NEUTRAL)
							iMultiplier = 8;
						else if (eOpinion == CIV_OPINION_NEUTRAL)
							iMultiplier = 4;
					}

					// We're stealing a defensive plot or Great Person improvement
					ImprovementTypes eAdjacentImprovement = pAdjacentPlot->getImprovementType();
					if (eAdjacentImprovement != NO_IMPROVEMENT)
					{
						CvImprovementEntry* pkAdjacentImprovementInfo = GC.getImprovementInfo(eAdjacentImprovement);
						if (pkAdjacentImprovementInfo->IsNoFollowUp())
							iTempScore += 20;

						if (pkAdjacentImprovementInfo->IsCreatedByGreatPerson())
							iTempScore += 50;

						iTempScore += pkAdjacentImprovementInfo->GetDefenseModifier();
					}

					// Finally we tie break with yields (only a rough count)
					for (int iYield = 0; iYield < YIELD_JFD_HEALTH; iYield++)
					{
						YieldTypes eYield = static_cast<YieldTypes>(iYield);
						iTempScore += pAdjacentPlot->getYield(eYield);
					}
				}

				iStealScore += iTempScore * iMultiplier;
			}

			// It needs to steal at least one enemy plot
			if (bIncludeEnemyPlot)
				goodPlots.push(SPlotWithScore(pPlot, iPlotScore + iStealScore));
		}

		// We have one valid improvement; time to get out of here
		break;
	}

	return goodPlots;
}

const vector<CvPlot*>& CvPlayerAI::GetTopCitadelPlotsCached()
{
	const int MAX_CANDIDATES = 5;

	if (m_iCurrentCitadelTargetsTurn == GC.getGame().getGameTurn())
		return m_vCurrentCitadelTargets;

	static const UnitClassTypes eGeneralClass = static_cast<UnitClassTypes>(GC.getInfoTypeForString("UNITCLASS_GREAT_GENERAL"));
	const UnitTypes eGeneral = GetSpecificUnitType(eGeneralClass);

	m_vCurrentCitadelTargets.clear();
	priority_queue<SPlotWithScore> goodPlots = GetBestCultureBombPlots(eGeneral, vector<CvPlot*>(), false);
	for (int i = 0; i < MAX_CANDIDATES && !goodPlots.empty(); i++)
	{
		SPlotWithScore candidate = goodPlots.top();

		m_vCurrentCitadelTargets.push_back(candidate.pPlot);
		goodPlots.pop();

		//give up if the rest is much worse than what we just had
		if (goodPlots.top().score < candidate.score * 0.7f)
			break;
	}

	m_iCurrentCitadelTargetsTurn = GC.getGame().getGameTurn();
	return m_vCurrentCitadelTargets;
}

bool CvPlayerAI::IsNicePlotForCitadel(const CvPlot* pPlot)
{
	const vector<CvPlot*>& choices = GetTopCitadelPlotsCached();
	return std::find(choices.begin(), choices.end(), pPlot) != choices.end();
}

CvPlot* CvPlayerAI::FindBestCultureBombPlot(CvUnit* pUnit, const vector<CvPlot*>& vPlotsToAvoid)
{
	if (!pUnit)
		return NULL;

	priority_queue<SPlotWithScore> goodPlots = GetBestCultureBombPlots(pUnit->getUnitType(), vPlotsToAvoid, true);

	if (goodPlots.empty())
		return NULL;

	SPlotWithScore nr1 = goodPlots.top();
	goodPlots.pop();
	SPlotWithScore nr2 = goodPlots.empty() ? SPlotWithScore(NULL, 0) : goodPlots.top();
	CvString strLogMessage;
	SPlotWithScore chosen;
	if (nr1.score * 0.8f > nr2.score)
	{
		strLogMessage = "Found one good culture bomb location: X: %d, Y: %d. SCORE: %d";
		chosen = nr1;
	}
	else
	{
		int iTurns1 = pUnit->TurnsToReachTarget(nr1.pPlot, true);
		int iTurns2 = pUnit->TurnsToReachTarget(nr2.pPlot, true);
		strLogMessage = "Found at least two similar culture bomb locations, chosen location: X: %d, Y: %d. SCORE: %d";
		chosen = iTurns1 < iTurns2 ? nr1 : nr2;
	}

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strLogString;
		strLogString.Format(strLogMessage, chosen.pPlot->getX(), chosen.pPlot->getY(), chosen.score);
		GetHomelandAI()->LogHomelandMessage(strLogString);
	}

	return chosen.pPlot;
}
