/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvBarbarians.h"
#include "CvGameCoreUtils.h"
#include "CvTypes.h"
#include "CvSpanSerialization.h"

//static 
short* CvBarbarians::m_aiBarbSpawnerCounter = NULL;
short* CvBarbarians::m_aiBarbSpawnerNumUnitsSpawned = NULL;

//	---------------------------------------------------------------------------
void CvBarbarians::MapInit(int iWorldNumPlots)
{
	Uninit();

	if (iWorldNumPlots > 0)
	{
		if (m_aiBarbSpawnerCounter == NULL)
		{
			m_aiBarbSpawnerCounter = FNEW(short[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
		}
		if (m_aiBarbSpawnerNumUnitsSpawned == NULL)
		{
			m_aiBarbSpawnerNumUnitsSpawned = FNEW(short[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
		}

		// Default values
		for (int iI = 0; iI < iWorldNumPlots; ++iI)
		{
			m_aiBarbSpawnerCounter[iI] = -1;
			m_aiBarbSpawnerNumUnitsSpawned[iI] = -1;
		}
	}
}

//	---------------------------------------------------------------------------
/// Uninit
void CvBarbarians::Uninit()
{
	if (m_aiBarbSpawnerCounter != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiBarbSpawnerCounter);
	}
	if (m_aiBarbSpawnerNumUnitsSpawned != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiBarbSpawnerNumUnitsSpawned);
	}
}

//	---------------------------------------------------------------------------
/// Serialize
template<typename Visitor>
void CvBarbarians::Serialize(Visitor& visitor)
{
	const bool bLoading = visitor.isLoading();

	const int iWorldNumPlots = GC.getMap().numPlots();
	if (bLoading)
		MapInit(iWorldNumPlots);	// Map will have been initialized/unserialized by now so this is ok.
	visitor(MakeConstSpan(m_aiBarbSpawnerCounter, iWorldNumPlots));
	visitor(MakeConstSpan(m_aiBarbSpawnerNumUnitsSpawned, iWorldNumPlots));
}

/// Serialization Read
void CvBarbarians::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(serialVisitor);
}

/// Serialization Write
void CvBarbarians::Write(FDataStream& kStream)
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(serialVisitor);
}

//	---------------------------------------------------------------------------
int CvBarbarians::GetBarbSpawnerCounter(CvPlot* pPlot)
{
	if (pPlot)
		return m_aiBarbSpawnerCounter[pPlot->GetPlotIndex()];

	return -1;
}

void CvBarbarians::SetBarbSpawnerCounter(CvPlot* pPlot, int iNewValue)
{
	if (pPlot)
		m_aiBarbSpawnerCounter[pPlot->GetPlotIndex()] = iNewValue;
}

void CvBarbarians::ChangeBarbSpawnerCounter(CvPlot* pPlot, int iChange)
{
	SetBarbSpawnerCounter(pPlot, GetBarbSpawnerCounter(pPlot) + iChange);
}

void CvBarbarians::DoBarbSpawnerAttacked(CvPlot* pPlot)
{
	if (!pPlot)
		return;

	int iNewValue = GetBarbSpawnerCounter(pPlot);
	if (pPlot->isCity())
	{
		iNewValue *= /*50*/ GD_INT_GET(BARBARIAN_CITY_ATTACKED_DELAY_MULTIPLIER);
		iNewValue /= 100;
	}
	else
	{
		iNewValue *= /*50*/ GD_INT_GET(BARBARIAN_ENCAMPMENT_ATTACKED_DELAY_MULTIPLIER);
		iNewValue /= 100;
	}

	SetBarbSpawnerCounter(pPlot, std::max(iNewValue, 0));
}

bool CvBarbarians::ShouldSpawnBarbFromCamp(CvPlot* pPlot)
{
	if (!pPlot)
		return false;

	if (GC.getGame().getElapsedGameTurns() < /*10*/ GD_INT_GET(BARBARIAN_INITIAL_SPAWN_TURN_FROM_SPAWNER))
		return false;

	if (MOD_EVENTS_BARBARIANS && GAMEEVENTINVOKE_TESTALL(GAMEEVENT_BarbariansCampCanSpawnUnit, pPlot->getX(), pPlot->getY()) == GAMEEVENTRETURN_FALSE)
		return false;

	return GetBarbSpawnerCounter(pPlot) == 0;
}

bool CvBarbarians::ShouldSpawnBarbFromCity(CvPlot* pPlot)
{
	if (!pPlot)
		return false;

	if (GC.getGame().getElapsedGameTurns() < /*10*/ GD_INT_GET(BARBARIAN_INITIAL_SPAWN_TURN_FROM_SPAWNER))
		return false;

	return GetBarbSpawnerCounter(pPlot) == 0;
}

//	---------------------------------------------------------------------------
int CvBarbarians::GetBarbSpawnerNumUnitsSpawned(CvPlot* pPlot)
{
	if (pPlot)
		return m_aiBarbSpawnerNumUnitsSpawned[pPlot->GetPlotIndex()];

	return -1;
}

void CvBarbarians::SetBarbSpawnerNumUnitsSpawned(CvPlot* pPlot, int iNewValue)
{
	if (pPlot)
		m_aiBarbSpawnerNumUnitsSpawned[pPlot->GetPlotIndex()] = iNewValue;
}

void CvBarbarians::ChangeBarbSpawnerNumUnitsSpawned(CvPlot* pPlot, int iChange)
{
	SetBarbSpawnerNumUnitsSpawned(pPlot, GetBarbSpawnerNumUnitsSpawned(pPlot) + iChange);
}

//	---------------------------------------------------------------------------
/// Gameplay informing us when a Camp/City has either been created or spawned a Unit so we can reseed the spawn counter
void CvBarbarians::ActivateBarbSpawner(CvPlot* pPlot)
{
	if (!pPlot)
		return;

	// Probably means this is being called as CvWorldBuilderMapLoaded is adding camps, MapInit() will follow soon and set everything up correctly
	if (m_aiBarbSpawnerNumUnitsSpawned == NULL)
		return;

	CvGame& kGame = GC.getGame();
	bool bInitialSpawning = kGame.getElapsedGameTurns() == /*0 in CP, 2 in VP*/ std::max(GD_INT_GET(BARBARIAN_INITIAL_SPAWN_TURN), 0);
	int iNumTurnsToSpawn = bInitialSpawning ? /*0*/ GD_INT_GET(BARBARIAN_INITIAL_SPAWN_SPAWNER_DELAY) : 0;

	// Cities spawn units 2x as often as camps
	if (pPlot->isCity())
	{
		iNumTurnsToSpawn = /*6*/ GD_INT_GET(BARBARIAN_SPAWN_DELAY_FROM_CITY);
		iNumTurnsToSpawn += kGame.randRangeExclusive(0, /*5*/ GD_INT_GET(BARBARIAN_SPAWN_DELAY_FROM_CITY_RAND), pPlot->GetPseudoRandomSeed());
	}
	else
	{
		iNumTurnsToSpawn = /*12*/ GD_INT_GET(BARBARIAN_SPAWN_DELAY_FROM_ENCAMPMENT);
		iNumTurnsToSpawn += kGame.randRangeExclusive(0, /*10*/ GD_INT_GET(BARBARIAN_SPAWN_DELAY_FROM_ENCAMPMENT_RAND), pPlot->GetPseudoRandomSeed());
	}

	// Chill
	if (kGame.isOption(GAMEOPTION_CHILL_BARBARIANS))
	{
		if (pPlot->isCity())
		{
			iNumTurnsToSpawn *= /*150*/ GD_INT_GET(BARBARIAN_SPAWN_DELAY_FROM_CITY_CHILL_MULTIPLIER);
			iNumTurnsToSpawn /= 100;
		}
		else
		{
			iNumTurnsToSpawn *= /*200*/ GD_INT_GET(BARBARIAN_SPAWN_DELAY_FROM_ENCAMPMENT_CHILL_MULTIPLIER);
			iNumTurnsToSpawn /= 100;
		}
	}

	// Raging
	if (kGame.isOption(GAMEOPTION_RAGING_BARBARIANS))
	{
		if (pPlot->isCity())
		{
			iNumTurnsToSpawn *= /*50*/ GD_INT_GET(BARBARIAN_SPAWN_DELAY_FROM_CITY_RAGING_MULTIPLIER);
			iNumTurnsToSpawn /= 100;
		}
		else
		{
			iNumTurnsToSpawn *= /*50*/ GD_INT_GET(BARBARIAN_SPAWN_DELAY_FROM_ENCAMPMENT_RAGING_MULTIPLIER);
			iNumTurnsToSpawn /= 100;
		}
	}

	// Reduce turns between spawn if we've pumped out more guys (meaning we're further into the game)
	// -1 turns if we've spawned one Unit, -3 turns if we've spawned three
	if (pPlot->isCity())
	{
		iNumTurnsToSpawn += /*-1*/ GD_INT_GET(BARBARIAN_CITY_SPAWN_DELAY_REDUCTION_PER_SPAWN) * std::min(/*3*/ (GD_INT_GET(BARBARIAN_CITY_MAX_SPAWN_DELAY_REDUCTION)*-1), GetBarbSpawnerNumUnitsSpawned(pPlot));
	}
	else
	{
		iNumTurnsToSpawn += /*-1*/ GD_INT_GET(BARBARIAN_ENCAMPMENT_SPAWN_DELAY_REDUCTION_PER_SPAWN) * std::min(/*3*/ (GD_INT_GET(BARBARIAN_ENCAMPMENT_MAX_SPAWN_DELAY_REDUCTION)*-1), GetBarbSpawnerNumUnitsSpawned(pPlot));
	}

	// Increment # of barbs spawned
	// This starts at -1, so when a camp is first created it'll bump up to 0, which is correct
	ChangeBarbSpawnerNumUnitsSpawned(pPlot, 1);

	// Difficulty level can add time between spawns
	iNumTurnsToSpawn += kGame.getHandicapInfo().getBarbarianSpawnDelay();

	// Game Speed can increase or decrease amount of time between spawns (ranges from 67 on Quick to 400 on Marathon)
	CvGameSpeedInfo* pGameSpeedInfo = GC.getGameSpeedInfo(kGame.getGameSpeedType());
	if (pGameSpeedInfo)
	{
		iNumTurnsToSpawn *= pGameSpeedInfo->getBarbPercent();
		iNumTurnsToSpawn /= 100;
	}

	SetBarbSpawnerCounter(pPlot, std::max(iNumTurnsToSpawn, /*2*/ std::max(GD_INT_GET(BARBARIAN_MIN_SPAWN_DELAY), 1)));
}

//	--------------------------------------------------------------------------------
/// Camp cleared, so reset counter
void CvBarbarians::DoBarbCampCleared(CvPlot* pPlot, PlayerTypes ePlayer, CvUnit* pUnit)
{
	if (!pPlot)
		return;

	int iCooldownTimer = /*15*/ GD_INT_GET(BARBARIAN_CAMP_CLEARED_MIN_TURNS_TO_RESPAWN);
	SetBarbSpawnerCounter(pPlot, (iCooldownTimer * -1) - 1);
	SetBarbSpawnerNumUnitsSpawned(pPlot, -1);

	pPlot->AddArchaeologicalRecord(CvTypes::getARTIFACT_BARBARIAN_CAMP(), ePlayer, NO_PLAYER);

	if (ePlayer != NO_PLAYER)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		CvCity* pBestCity = kPlayer.GetClosestCityByPlots(pPlot);
		if (pBestCity)
		{
			int iNumGold = kPlayer.getHandicapInfo().getBarbarianCampGold();
			iNumGold += kPlayer.isHuman() ? 0 : GC.getGame().getHandicapInfo().getAIBarbarianCampGold();
			kPlayer.doInstantYield(INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED, false, NO_GREATPERSON, NO_BUILDING, iNumGold, MOD_BALANCE_VP, NO_PLAYER, NULL, false, pBestCity, false, true, false, YIELD_GOLD, pUnit);

			if (GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) > 0 || GET_PLAYER(ePlayer).GetBarbarianCombatBonus(true) > 0)
			{
				int iCulturePoints = (GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) / 5);
				if (iCulturePoints <= 0)
					iCulturePoints = GET_PLAYER(ePlayer).GetBarbarianCombatBonus(true);

				kPlayer.doInstantYield(INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED, false, NO_GREATPERSON, NO_BUILDING, iCulturePoints, true, NO_PLAYER, NULL, false, pBestCity, false, true, false, YIELD_CULTURE, pUnit);

				if (ePlayer == GC.getGame().getActivePlayer())
				{
					char text[256] = { 0 };
					sprintf_s(text, "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]", iCulturePoints);
					SHOW_PLOT_POPUP(pPlot, ePlayer, text);
				}
			}

			// call one for era scaling, and another for non-era scaling
			GET_PLAYER(ePlayer).doInstantYield(INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pBestCity);
			GET_PLAYER(ePlayer).doInstantYield(INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED, false, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, pBestCity);

			// If it's the active player then show the popup (but not in MP)
			if (ePlayer == GC.getGame().getActivePlayer() && !GC.getGame().isReallyNetworkMultiPlayer())
			{
				// Make sure we show the correct amount of Gold in the popup
				iNumGold *= 100 + GET_PLAYER(ePlayer).GetPlayerTraits()->GetPlunderModifier();
				iNumGold /= 100;
				if (MOD_BALANCE_VP)
				{
					int iEra = max<int>(1, GET_PLAYER(ePlayer).GetCurrentEra());
					iNumGold *= iEra;
				}

				CvPopupInfo kPopupInfo(BUTTONPOPUP_BARBARIAN_CAMP_REWARD, iNumGold);
				DLLUI->AddPopup(kPopupInfo);
				// We are adding a popup that the player must make a choice in, make sure they are not in the end-turn phase.
				CancelActivePlayerEndTurn();

				//Increment Stat
				if (MOD_API_ACHIEVEMENTS && kPlayer.isHuman() && !GC.getGame().isGameMultiPlayer())
				{
					gDLL->IncrementSteamStatAndUnlock(ESTEAMSTAT_BARBARIANCAMPS, 100, ACHIEVEMENT_100CAMPS);
				}
			}

			if (GET_PLAYER(ePlayer).isMajorCiv())
			{
				// Completes a quest for anyone?
				for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
				{
					PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
					CvPlayer& minorPlayer = GET_PLAYER(eMinor);

					if (!minorPlayer.isAlive())
						continue;

					CvMinorCivAI* pMinorCivAI = minorPlayer.GetMinorCivAI();
					pMinorCivAI->DoTestActiveQuestsForPlayer(ePlayer, /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_KILL_CAMP);
				}
			}
		}
	}

	if (MOD_EVENTS_BARBARIANS)
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansCampCleared, pPlot->getX(), pPlot->getY(), ePlayer);
}

/// Barbarian city captured, so reset counter
void CvBarbarians::DoBarbCityCleared(CvPlot* pPlot)
{
	int iCooldownTimer = /*15*/ GD_INT_GET(BARBARIAN_CAMP_CLEARED_MIN_TURNS_TO_RESPAWN);
	SetBarbSpawnerCounter(pPlot, (iCooldownTimer * -1) - 1);
	SetBarbSpawnerNumUnitsSpawned(pPlot, -1);
}

//	--------------------------------------------------------------------------------
//return false if stealing is impossible and the unit should do something else instead
bool CvBarbarians::DoStealFromCity(CvUnit* pUnit, CvCity* pCity)
{
	if (!MOD_BALANCE_CORE_BARBARIAN_THEFT)
		return false;

	if (!pUnit || !pUnit->IsCanAttack() || !pCity)
		return false;

	//don't steal from ourselves
	if (pCity->isBarbarian())
		return false;

	//can't steal if guarded
	if (pCity->HasGarrison())
		return false;

	// if city is very low on health, try to take it over outright
	// this also prevents attempts to steal from a 0 HP city
	if (pCity->getDamage() >= (pCity->GetMaxHitPoints() - 20))
		return false;

	//pretend the unit attacks this city
	//city pays them off so they don't do damage
	int iAttackerDamage = 0;
	int iDefenderDamage = TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(pCity, pUnit, pUnit->plot(), iAttackerDamage);

	//tried but failed
	if (iDefenderDamage <= 0 || iAttackerDamage >= pUnit->GetCurrHitPoints())
		return true;

	//they get x turns worth of yields
	int iTheftTurns = std::max(1, iDefenderDamage / 30 + GC.getGame().randRangeExclusive(0, 5, GET_PLAYER(pCity->getOwner()).GetPseudoRandomSeed()));

	//but they lose some health in exchange
	pUnit->changeDamage(GC.getGame().randRangeExclusive(0, std::min(pUnit->GetCurrHitPoints(),30), GET_PLAYER(pCity->getOwner()).GetPseudoRandomSeed().mix(iDefenderDamage)));

	//which yield is affected?
	int iYield = GC.getGame().randRangeExclusive(0, 10, GET_PLAYER(pCity->getOwner()).GetPseudoRandomSeed().mix(pUnit->plot()->GetPlotIndex()));
	if (iYield <= 2)
	{
		int iGold = std::min(pCity->getBaseYieldRate(YIELD_GOLD) * iTheftTurns, pUnit->GetCurrHitPoints());
		if (iGold > 0)
		{
			GET_PLAYER(pCity->getOwner()).GetTreasury()->ChangeGold(-iGold);

			Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT_CITY_DETAILED");
			strMessage << iGold;
			strMessage << pCity->getNameKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT_CITY");
			strSummary << pCity->getNameKey();

			CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
			if (pNotification)
			{
				pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
			}

			CvString strLog;
			strLog.Format("Barbarians stole %d gold from %s", iGold, pCity->getNameKey());
			GET_PLAYER(pCity->getOwner()).GetHomelandAI()->LogHomelandMessage(strLog);
		}
	}
	else if (iYield <= 4)
	{
		int iCulture = std::min(pCity->getJONSCulturePerTurn() * iTheftTurns, pUnit->GetCurrHitPoints());
		if (iCulture > 0)
		{
			GET_PLAYER(pCity->getOwner()).changeJONSCulture(-iCulture);

			Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_CULTURE_THEFT_CITY_DETAILED");
			strMessage << iCulture;
			strMessage << pCity->getNameKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_CULTURE_THEFT_CITY");
			strSummary << pCity->getNameKey();

			CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
			if (pNotification)
			{
				pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
			}
			CvString strLog;
			strLog.Format("Barbarians stole %d culture from %s", iCulture, pCity->getNameKey());
			GET_PLAYER(pCity->getOwner()).GetHomelandAI()->LogHomelandMessage(strLog);
		}
	}
	else if (iYield <= 6)
	{
		TechTypes eCurrentTech = GET_PLAYER(pCity->getOwner()).GetPlayerTechs()->GetCurrentResearch();
		int iScience = 0;
		if (eCurrentTech != NO_TECH)
		{
			iScience = std::min(pCity->getBaseYieldRate(YIELD_SCIENCE) * iTheftTurns, pUnit->GetCurrHitPoints());
			if (iScience > 0)
			{
				GET_TEAM(pCity->getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, -iScience, pCity->getOwner());

				Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_SCIENCE_THEFT_CITY_DETAILED");
				strMessage << iScience;
				strMessage << pCity->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_SCIENCE_THEFT_CITY");
				strSummary << pCity->getNameKey();

				CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
				if (pNotification)
				{
					pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
				}

				CvString strLog;
				strLog.Format("Barbarians stole %d science from %s", iScience, pCity->getNameKey());
				GET_PLAYER(pCity->getOwner()).GetHomelandAI()->LogHomelandMessage(strLog);
			}
		}
	}
	else if (iYield <= 8)
	{
		int iFood = std::min(pCity->getBaseYieldRate(YIELD_FOOD) * iTheftTurns, pUnit->GetCurrHitPoints());
		if (iFood > 0)
		{
			pCity->changeFood(-iFood);

			Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_FOOD_THEFT_CITY_DETAILED");
			strMessage << iFood;
			strMessage << pCity->getNameKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_FOOD_THEFT_CITY");
			strSummary << pCity->getNameKey();

			CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
			if (pNotification)
			{
				pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
			}

			CvString strLog;
			strLog.Format("Barbarians stole %d food from %s", iFood, pCity->getNameKey());
			GET_PLAYER(pCity->getOwner()).GetHomelandAI()->LogHomelandMessage(strLog);
		}
	}
	else
	{
		if ((pCity->getProduction() > 0) && (pCity->getProductionTurnsLeft() >= 2) && (pCity->getProductionTurnsLeft() != INT_MAX))
		{
			int iProduction = std::min(pCity->getBaseYieldRate(YIELD_PRODUCTION) * iTheftTurns, pUnit->GetCurrHitPoints());
			if (iProduction > 0)
			{
				pCity->changeProduction(-iProduction);

				Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_PRODUCTION_THEFT_CITY_DETAILED");
				strMessage << iProduction;
				strMessage << pCity->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_PRODUCTION_THEFT_CITY");
				strSummary << pCity->getNameKey();

				CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
				if (pNotification)
				{
					pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
				}

				CvString strLog;
				strLog.Format("Barbarians stole %d production from %s", iProduction, pCity->getNameKey());
				GET_PLAYER(pCity->getOwner()).GetHomelandAI()->LogHomelandMessage(strLog);
			}
		}
	}

	return true;
}

bool CvBarbarians::DoTakeOverCityState(CvCity* pCity)
{
	if (!pCity)
		return false;

	CvPlayer& owner = GET_PLAYER(pCity->getOwner());
	if (!owner.isMinorCiv() || !owner.GetMinorCivAI()->IsReadyForTakeOver())
		return false;

	CvPlot* pCityPlot = pCity->plot();
	if (!pCityPlot)
		return false;

	// create a barbarian unit in the city, taking it over
	SpawnBarbarianUnits(pCity->plot(), 1, BARB_SPAWN_CITY_STATE_CAPTURE);
	if (pCityPlot->isCity() && pCityPlot->getOwner() == BARBARIAN_PLAYER)
		return true;

	return false;
}

//	--------------------------------------------------------------------------------
void CvBarbarians::DoCamps()
{
	CvGame& kGame = GC.getGame();
	ImprovementTypes eCamp = (ImprovementTypes)GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT);
	CvImprovementEntry* pkCampInfo = GC.getImprovementInfo(eCamp);
	int iGameTurn = GC.getGame().getGameTurn();
	int iInitialSpawnTurn = /*0 in CP, 2 in VP*/ std::max(GD_INT_GET(BARBARIAN_INITIAL_SPAWN_TURN), 0);
	if (eCamp == NO_IMPROVEMENT || pkCampInfo == NULL)
		return;
	if (kGame.isOption(GAMEOPTION_NO_BARBARIANS) || kGame.getElapsedGameTurns() < iInitialSpawnTurn)
		return;

	int iNumCampsToAdd = 0;
	bool bInitialSpawning = kGame.getElapsedGameTurns() == iInitialSpawnTurn;

	// At the end of turn 2, spawn camps in 1% of eligible tiles, plus 0.5% per era for advanced starts
	// In Community Patch only at the end of turn 0, spawn 1/3 of the maximum number of camps
	if (bInitialSpawning)
	{
		iNumCampsToAdd = /*10 in VP, 33 in CP*/ GD_INT_GET(BARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD);

		if (MOD_BALANCE_VP)
			iNumCampsToAdd += GC.getGame().getStartEra() > 0 ? (GC.getGame().getStartEra() * /*5 in VP, 0 in CP*/ GD_INT_GET(BARBARIAN_CAMP_FIRST_TURN_PERCENT_PER_ERA)) : 0;

		if (iNumCampsToAdd <= 0)
			iNumCampsToAdd = 1;
	}
	else
	{
		// In VP, spawn a camp every 1-3 turns, depending on game options
		if (MOD_BALANCE_VP)
		{
			int iGameTurn = kGame.getElapsedGameTurns() - iInitialSpawnTurn;
			int iSpawnRate = /*2*/ GD_INT_GET(BARBARIAN_CAMP_SPAWN_RATE);
			iSpawnRate += kGame.isOption(GAMEOPTION_CHILL_BARBARIANS) ? /*1*/ GD_INT_GET(BARBARIAN_CAMP_SPAWN_RATE_CHILL) : 0;
			iSpawnRate += kGame.isOption(GAMEOPTION_RAGING_BARBARIANS) ? /*-1*/ GD_INT_GET(BARBARIAN_CAMP_SPAWN_RATE_RAGING) : 0;

			if (iSpawnRate <= 0)
				iSpawnRate = 1;

			if (iGameTurn % iSpawnRate == 0)
			{
				iNumCampsToAdd = /*1*/ GD_INT_GET(BARBARIAN_CAMP_NUM_AFTER_INITIAL);
				iNumCampsToAdd += kGame.isOption(GAMEOPTION_CHILL_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_CAMP_NUM_AFTER_INITIAL_CHILL) : 0;
				iNumCampsToAdd += kGame.isOption(GAMEOPTION_RAGING_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_CAMP_NUM_AFTER_INITIAL_RAGING) : 0;
			}
		}
		// In Community Patch only, 1/2 chance of spawning a camp each turn (after turn 0)
		else if (GD_INT_GET(BARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING) > 0)
		{
			if (kGame.randRangeExclusive(0, /*2*/ std::max(GD_INT_GET(BARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING), 1), CvSeeder(kGame.getGameTurn()).mix(kGame.getGameTurn())) == 0)
			{
				iNumCampsToAdd = /*1*/ GD_INT_GET(BARBARIAN_CAMP_NUM_AFTER_INITIAL);
				iNumCampsToAdd += kGame.isOption(GAMEOPTION_CHILL_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_CAMP_NUM_AFTER_INITIAL_CHILL) : 0;
				iNumCampsToAdd += kGame.isOption(GAMEOPTION_RAGING_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_CAMP_NUM_AFTER_INITIAL_RAGING) : 0;
			}
		}
	}

	CvMap& theMap = GC.getMap();
	int iNumCampsInExistence = 0, iNumCoastalCamps = 0, iNumWorldPlots = theMap.numPlots();
	int iMajorCapitalMinDistance = /*4*/ GD_INT_GET(BARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL);
	int iBarbCampMinDistance = /*4*/ GD_INT_GET(BARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP);
	int iRecentlyClearedCampMinDistance = /*2*/ GD_INT_GET(BARBARIAN_CAMP_MINIMUM_DISTANCE_RECENTLY_CLEARED_CAMP);
	int iEra = GC.getGame().getCurrentEra();
	std::vector<CvPlot*> vPotentialPlots,vPotentialCoastalPlots;
	std::vector<int> MajorCapitals,BarbCamps,RecentlyClearedBarbCamps;

	// Iterate through all plots
	for (int iI = 0; iI < iNumWorldPlots; iI++)
	{
		CvPlot* pLoopPlot = theMap.plotByIndexUnchecked(iI);
		ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

		// Existing Barbarian camp here
		if (eImprovement == eCamp)
		{
			if (GetBarbSpawnerCounter(pLoopPlot) > 0)
				ChangeBarbSpawnerCounter(pLoopPlot, -1);

			// If time is up, unleash the beast!
			if (ShouldSpawnBarbFromCamp(pLoopPlot))
			{
				int iNumBarbs = /*1*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PER_ENCAMPMENT_SPAWN);
				iNumBarbs += kGame.isOption(GAMEOPTION_CHILL_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PER_ENCAMPMENT_SPAWN_CHILL) : 0;
				iNumBarbs += kGame.isOption(GAMEOPTION_RAGING_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PER_ENCAMPMENT_SPAWN_RAGING) : 0;
				if (iEra > 0)
					iNumBarbs += iEra * /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PER_ENCAMPMENT_SPAWN_PER_ERA) / 100;

				SpawnBarbarianUnits(pLoopPlot, iNumBarbs, BARB_SPAWN_FROM_ENCAMPMENT);
			}

			if (iBarbCampMinDistance > 0)
				BarbCamps.push_back(pLoopPlot->GetPlotIndex());

			iNumCampsInExistence++;
			if (pLoopPlot->isCoastalLand())
				iNumCoastalCamps++;

			continue;
		}
		// City here
		else if (pLoopPlot->isCity())
		{
			// Barbarian city?
			if (pLoopPlot->getOwner() == BARBARIAN_PLAYER)
			{
				if (GetBarbSpawnerCounter(pLoopPlot) > 0)
					ChangeBarbSpawnerCounter(pLoopPlot, -1);

				// If time is up, unleash the beast!
				if (ShouldSpawnBarbFromCity(pLoopPlot))
				{
					int iNumBarbs = /*1*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PER_CITY_SPAWN);
					iNumBarbs += kGame.isOption(GAMEOPTION_CHILL_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PER_CITY_SPAWN_CHILL) : 0;
					iNumBarbs += kGame.isOption(GAMEOPTION_RAGING_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PER_CITY_SPAWN_RAGING) : 0;
					if (iEra > 0)
						iNumBarbs += iEra * /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_PER_CITY_SPAWN_PER_ERA) / 100;

					SpawnBarbarianUnits(pLoopPlot, iNumBarbs, BARB_SPAWN_FROM_CITY);
				}

				// Cities count as "camps" for the minimum distance requirement, but don't add to the encampment totals
				if (iBarbCampMinDistance > 0)
					BarbCamps.push_back(pLoopPlot->GetPlotIndex());
			}
			// Major capital?
			else if (iMajorCapitalMinDistance > 0 && GET_PLAYER(pLoopPlot->getOwner()).isMajorCiv() && pLoopPlot->getPlotCity()->isCapital())
			{
				MajorCapitals.push_back(pLoopPlot->GetPlotIndex());
			}

			continue;
		}
		// Barbarian camp recently cleared here, not allowed to respawn for a while
		else if (GetBarbSpawnerCounter(pLoopPlot) < -1)
		{
			ChangeBarbSpawnerCounter(pLoopPlot, 1);
			if (GetBarbSpawnerCounter(pLoopPlot) < -1 && iRecentlyClearedCampMinDistance > 0)
				RecentlyClearedBarbCamps.push_back(pLoopPlot->GetPlotIndex());

			continue;
		}
		// Initial spawning in Ancient or Classical Era? Avoid spawning too close to starting Settlers (treat them like capitals).
		else if (MOD_BALANCE_VP && bInitialSpawning && GC.getGame().getStartEraInfo().getStartingUnitMultiplier() <= 1 && pLoopPlot->getFirstUnitOfAITypeOtherTeam(BARBARIAN_TEAM, UNITAI_SETTLE) != NULL)
		{
			MajorCapitals.push_back(pLoopPlot->GetPlotIndex());
			continue;
		}

		// Not adding any camps this turn? No need to proceed!
		if (iNumCampsToAdd <= 0)
			continue;

		// No camps on Improvements in Community Patch only, and no camps on embassies ever
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (!MOD_BALANCE_VP || GC.getImprovementInfo(eImprovement)->IsPermanent())
				continue;
		}

		// No camps on Ancient Ruins
		if (pLoopPlot->isGoody())
			continue;

		// No camps on Natural Wonders
		if (pLoopPlot->getFeatureType() != NO_FEATURE && GC.getFeatureInfo(pLoopPlot->getFeatureType())->isNoImprovement())
			continue;

		// No camps on n-tile islands
		if (pLoopPlot->getLandmass() == -1 || theMap.getLandmassById(pLoopPlot->getLandmass())->getNumTiles() <= /*1*/ GD_INT_GET(BARBARIAN_CAMP_MINIMUM_ISLAND_SIZE))
			continue;

		// No camps on water, in owned territory, adjacent to owned territory, or in impassable tiles
		if (pLoopPlot->isWater() || pLoopPlot->isOwned() || pLoopPlot->IsAdjacentOwnedByTeamOtherThan(BARBARIAN_TEAM) || !pLoopPlot->isValidMovePlot(BARBARIAN_PLAYER))
			continue;

		// Custom rule preventing Barbarian camps from spawning here?
		if (MOD_EVENTS_BARBARIANS && GAMEEVENTINVOKE_TESTALL(GAMEEVENT_BarbariansCanFoundCamp, pLoopPlot->getX(), pLoopPlot->getY()) == GAMEEVENTRETURN_FALSE)
			continue;

		// No camps on plots in sight of a civ or City-State in Community Patch only
		if (!MOD_BALANCE_VP && pLoopPlot->isVisibleToAnyTeam(false))
			continue;

		// No camps on occupied plots or those adjacent to a non-Barbarian unit
		if (pLoopPlot->getNumUnits() > 0 || pLoopPlot->IsEnemyUnitAdjacent(BARBARIAN_TEAM))
			continue;

		// Are Barbarian camps allowed to spawn on a tile with this terrain and this feature?
		if (!pkCampInfo->GetTerrainMakesValid(pLoopPlot->getTerrainType()) || !pkCampInfo->GetFeatureMakesValid(pLoopPlot->getFeatureType()))
			continue;

		// This plot is POTENTIALLY eligible, add it to the list
		vPotentialPlots.push_back(pLoopPlot);

		if (vPotentialPlots.size() > 1)
		{
			//do one iteration of a fisher-yates shuffle
			uint uSwap = kGame.urandLimitExclusive(vPotentialPlots.size(), pLoopPlot->GetPseudoRandomSeed().mix(vPotentialPlots.size()).mix(iGameTurn));
			std::swap(vPotentialPlots[uSwap],vPotentialPlots.back());
		}

		if (pLoopPlot->isCoastalLand())
		{
			vPotentialCoastalPlots.push_back(pLoopPlot);

			if (vPotentialCoastalPlots.size() > 1)
			{
				//do one iteration of a fisher-yates shuffle
				uint uSwap = kGame.urandLimitExclusive(vPotentialCoastalPlots.size(), pLoopPlot->GetPseudoRandomSeed().mix(vPotentialCoastalPlots.size()).mix(iGameTurn));
				std::swap(vPotentialCoastalPlots[uSwap],vPotentialCoastalPlots.back());
			}
		}
	}

	// Community Patch only: Calculate maximum number of camps
	if (!MOD_BALANCE_VP)
	{
		int iMaximumCamps = vPotentialPlots.size() / std::max(theMap.getWorldInfo().getFogTilesPerBarbarianCamp(), 1);
		if (iMaximumCamps <= 0 || iNumCampsInExistence >= iMaximumCamps)
			return;

		// If this is the initial spawning, let's calculate how many camps to place on the map
		if (bInitialSpawning)
		{
			iNumCampsToAdd *= iMaximumCamps;
			iNumCampsToAdd /= 100;

			if (iNumCampsToAdd <= 0)
				iNumCampsToAdd = 1;
		}

		// Make sure not to exceed the limit
		if ((iNumCampsInExistence + iNumCampsToAdd) >= iMaximumCamps)
			iNumCampsToAdd = iMaximumCamps - iNumCampsInExistence;
	}

	// Not adding any camps this turn? We're done here!
	if (iNumCampsToAdd <= 0)
		return;

	// Now go through our list of potentially eligible plots and get rid of any that aren't valid (too close to something)
	std::vector<CvPlot*> vValidPlots,vValidCoastalPlots;
	for (unsigned int iI = 0; iI < vPotentialPlots.size(); iI++)
	{
		CvPlot* pLoopPlot = vPotentialPlots[iI];
		int iX = pLoopPlot->getX(), iY = pLoopPlot->getY();
		bool bTooClose = false;

		for (std::vector<int>::iterator it = MajorCapitals.begin(); it != MajorCapitals.end(); it++)
		{
			CvPlot* pInvalidAreaPlot = theMap.plotByIndex(*it);
			int iCapitalX = pInvalidAreaPlot->getX(), iCapitalY = pInvalidAreaPlot->getY();
			int iDistance = plotDistance(iX, iY, iCapitalX, iCapitalY);
			if (iDistance <= iMajorCapitalMinDistance)
			{
				bTooClose = true;
				break;
			}
		}
		if (!bTooClose)
		{
			for (std::vector<int>::iterator it = BarbCamps.begin(); it != BarbCamps.end(); it++)
			{
				CvPlot* pInvalidAreaPlot = theMap.plotByIndex(*it);
				int iCampX = pInvalidAreaPlot->getX(), iCampY = pInvalidAreaPlot->getY();
				int iDistance = plotDistance(iX, iY, iCampX, iCampY);
				if (iDistance <= iBarbCampMinDistance)
				{
					bTooClose = true;
					break;
				}
			}
		}
		if (!bTooClose)
		{
			for (std::vector<int>::iterator it = RecentlyClearedBarbCamps.begin(); it != RecentlyClearedBarbCamps.end(); it++)
			{
				CvPlot* pInvalidAreaPlot = theMap.plotByIndex(*it);
				int iFormerCampX = pInvalidAreaPlot->getX(), iFormerCampY = pInvalidAreaPlot->getY();
				int iDistance = plotDistance(iX, iY, iFormerCampX, iFormerCampY);
				if (iDistance <= iRecentlyClearedCampMinDistance)
				{
					bTooClose = true;
					break;
				}
			}
		}
		if (!bTooClose)
		{
			vValidPlots.push_back(pLoopPlot);

			if (std::find(vPotentialCoastalPlots.begin(), vPotentialCoastalPlots.end(), pLoopPlot) != vPotentialCoastalPlots.end())
				vValidCoastalPlots.push_back(pLoopPlot);
		}
	}

	vPotentialPlots.clear();
	vPotentialCoastalPlots.clear();

	// Which players can see new Barbarian camps in revealed territory?
	bool bUpdateMapFog = false;
	std::vector<PlayerTypes> vThoseWhoSee;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iPlayerLoop;
		if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).IsAlwaysSeeBarbCamps())
			vThoseWhoSee.push_back(ePlayer);
	}

	// What % of camps should be prioritized to be on the coast?
	int iCoastPercent = /*33*/ GD_INT_GET(BARBARIAN_CAMP_COASTAL_SPAWN_ROLL);

	// How many units should we spawn from new encampments?
	int iNumInitialUnits = 0;
	if (bInitialSpawning)
	{
		iNumInitialUnits = /*1 in CP, 2 in VP*/ GD_INT_GET(BARBARIAN_NUM_UNITS_INITIAL_ENCAMPMENT_CREATION_SPAWN);
		iNumInitialUnits += kGame.isOption(GAMEOPTION_CHILL_BARBARIANS) ? /*-1*/ GD_INT_GET(BARBARIAN_NUM_UNITS_INITIAL_ENCAMPMENT_CREATION_SPAWN_CHILL) : 0;
		iNumInitialUnits += kGame.isOption(GAMEOPTION_RAGING_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_INITIAL_ENCAMPMENT_CREATION_SPAWN_RAGING) : 0;
		if (iEra > 0)
			iNumInitialUnits += iEra * /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_INITIAL_ENCAMPMENT_CREATION_SPAWN_PER_ERA) / 100;
	}
	else
	{
		iNumInitialUnits = /*1 in CP, 2 in VP*/ GD_INT_GET(BARBARIAN_NUM_UNITS_ENCAMPMENT_CREATION_SPAWN);
		iNumInitialUnits += kGame.isOption(GAMEOPTION_CHILL_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_ENCAMPMENT_CREATION_SPAWN_CHILL) : 0;
		iNumInitialUnits += kGame.isOption(GAMEOPTION_RAGING_BARBARIANS) ? /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_ENCAMPMENT_CREATION_SPAWN_RAGING) : 0;
		if (iEra > 0)
			iNumInitialUnits += iEra * /*0*/ GD_INT_GET(BARBARIAN_NUM_UNITS_ENCAMPMENT_CREATION_SPAWN_PER_ERA) / 100;
	}

	if (iNumInitialUnits <= 0)
		iNumInitialUnits = 1;

	// Vox Populi: If this is the first turn, how many camps should we spawn?
	if (MOD_BALANCE_VP && bInitialSpawning)
	{
		iNumCampsToAdd *= vValidPlots.size();
		iNumCampsToAdd /= 1000;

		if (iNumCampsToAdd <= 0)
			iNumCampsToAdd = 1;
	}

	// Place the camps on the map
	do
	{
		// Bias towards coastal if we don't have the desired % of coastal camps (and there are valid plots)
		bool bWantsCoastal = !vValidCoastalPlots.empty();
		if (((iNumCampsInExistence * iCoastPercent) / 100) >= iNumCoastalCamps)
			bWantsCoastal = false;

		std::vector<CvPlot*>& vRelevantPlots = bWantsCoastal ? vValidCoastalPlots : vValidPlots;

		// No valid plots to place a camp? We're done.
		if (vRelevantPlots.empty())
			break;

		// Pick a plot
		uint uIndex = kGame.urandLimitExclusive(vRelevantPlots.size(), CvSeeder(vRelevantPlots.size()).mix(iNumCampsInExistence).mix(GC.getGame().getGameTurn()));
		CvPlot* pPlot = vRelevantPlots[uIndex];

		// Spawn a camp and units to defend it!
		pPlot->setRouteType(NO_ROUTE);
		pPlot->setImprovementType(eCamp);
		SpawnBarbarianUnits(pPlot, iNumInitialUnits, BARB_SPAWN_NEW_ENCAMPMENT);
		ActivateBarbSpawner(pPlot);

		// Show the new camp to any who have the policy
		for (std::vector<PlayerTypes>::iterator it = vThoseWhoSee.begin(); it != vThoseWhoSee.end(); it++)
		{
			TeamTypes eTeam = GET_PLAYER(*it).getTeam();
			if (pPlot->isRevealed(eTeam))
			{
				pPlot->setRevealedImprovementType(eTeam, eCamp);
				if (GC.getGame().getActivePlayer() == GET_PLAYER(*it).GetID())
					bUpdateMapFog = true;
			}
		}

		// Are we done?
		iNumCampsToAdd--;
		if (iNumCampsToAdd == 0)
			break;

		iNumCampsInExistence++;
		if (pPlot->isCoastalLand())
			iNumCoastalCamps++;

		// Check to see if any plots in the list are now too close to the new camp, and remove any that are
		vPotentialPlots = vValidPlots;
		vPotentialCoastalPlots = vValidCoastalPlots;
		vValidPlots.clear();
		vValidCoastalPlots.clear();

		int iNewCampX = pPlot->getX(), iNewCampY = pPlot->getY();
		for (unsigned int iI = 0; iI < vPotentialPlots.size(); iI++)
		{
			CvPlot* pLoopPlot = vValidPlots[iI];

			// Don't use the same plot again!
			if (pLoopPlot == pPlot)
				continue;

			if (plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), iNewCampX, iNewCampY) > iBarbCampMinDistance)
			{
				// This plot is still valid!
				vValidPlots.push_back(pLoopPlot);

				if (std::find(vPotentialCoastalPlots.begin(), vPotentialCoastalPlots.end(), pLoopPlot) != vPotentialCoastalPlots.end())
					vValidCoastalPlots.push_back(pLoopPlot);
			}
		}
		vPotentialPlots.clear();
		vPotentialCoastalPlots.clear();
	}
	while (iNumCampsToAdd > 0);

	if (bUpdateMapFog)
		GC.getMap().updateDeferredFog();

	// new barbarian camps can affect trade route paths cache
	GC.getGame().GetGameTrade()->InvalidateTradePathCache();
}

//	--------------------------------------------------------------------------------
/// Spawn Barbarian units near a plot - unleash the beast!
void CvBarbarians::SpawnBarbarianUnits(CvPlot* pPlot, int iNumUnits, BarbSpawnReason eReason)
{
	if (!pPlot || iNumUnits <= 0 || GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS))
		return;

	// ---------- SPAWNING RULES ---------- //
	// Plundered Trade Route - cannot use resources; can only spawn adjacent; no spawn cap
	// Encampment - can use unowned resources within 3 tiles; can only spawn adjacent; spawn cap of 2 within 3 tiles
	// City - can use resources owned by the city and unowned resources within 3 tiles; can use the UU of the player the city was captured from; can only spawn adjacent; spawn cap of 4 within 3 tiles
	// Uprising or Partisans - can use resources owned by the player being revolted against as well as their UUs; no spawn cap
	// Horde Quest or City-State capture - can use any resources owned by the City-State; if militaristic and UU is land, can also use their UU; no spawn cap
	// City Capture - unused by default - can use resources owned by the city and unowned resources within 3 tiles; can use the UU of the player the city was captured from; no spawn cap
	// Event - cannot use resources; no spawn cap
	// Non-Barbarian UUs cannot be used in Community Patch only.
	// The spawn cap is ignored if a unit is spawning into an undefended encampment or city.
	// Boats can only be spawned from encampments or captured cities on turn 30 or later.
	// ----------------------------------- //

	int iMaxBarbarians = INT_MAX, iMaxBarbarianRange = 0, iMinSpawnRadius = 1, iMaxSpawnRadius = 1;
	bool bNotBasePlot = false;

	switch (eReason)
	{
	case BARB_SPAWN_NEW_ENCAMPMENT:
	case BARB_SPAWN_CITY_STATE_CAPTURE:
		break;
	case BARB_SPAWN_PLUNDERED_TRADE_ROUTE:
		bNotBasePlot = true;
		break;
	case BARB_SPAWN_FROM_ENCAMPMENT:
		iMaxBarbarians = /*2*/ std::max(GD_INT_GET(MAX_BARBARIANS_FROM_CAMP_NEARBY), 1);
		iMaxBarbarianRange = /*4*/ GD_INT_GET(MAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE);
		break;
	case BARB_SPAWN_FROM_CITY:
		iMaxBarbarians = /*4*/ std::max(GD_INT_GET(MAX_BARBARIANS_FROM_CITY_NEARBY), 1);
		iMaxBarbarianRange = /*4*/ GD_INT_GET(MAX_BARBARIANS_FROM_CITY_NEARBY_RANGE);
		break;
	case BARB_SPAWN_CITY_CAPTURE:
	case BARB_SPAWN_UPRISING:
	case BARB_SPAWN_PARTISANS:
	case BARB_SPAWN_EVENT:
	case BARB_SPAWN_HORDE_QUEST:
		iMaxSpawnRadius = 5;
		bNotBasePlot = true;
		break;
	}

	// Are there too many Barbarian units nearby?
	int iCount = 0, iUnitLoop = 0, iX = pPlot->getX(), iY = pPlot->getY();
	bool bSpawnOnPlot = !bNotBasePlot;
	for (CvUnit* pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).nextUnit(&iUnitLoop))
	{
		if (!pLoopUnit->IsCanDefend() || pLoopUnit->isDelayedDeath())
			continue;

		CvPlot* pLoopPlot = pLoopUnit->plot();
		if (pLoopPlot)
		{
			if (pLoopPlot == pPlot)
				bSpawnOnPlot = false;
			else if (plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), iX, iY) <= iMaxBarbarianRange)
				iCount++;
		}
	}

	// If spawner is defended and spawn cap would be exceeded, do not spawn anything
	if (!bSpawnOnPlot && iCount >= iMaxBarbarians)
		return;

	// Can we use unique units?
	PlayerTypes eUniqueUnitPlayer = NO_PLAYER;
	if (MOD_BALANCE_VP)
	{
		if (eReason == BARB_SPAWN_FROM_CITY || eReason == BARB_SPAWN_CITY_CAPTURE)
		{
			eUniqueUnitPlayer = pPlot->getPlotCity()->getOriginalOwner();
		}
		else if (eReason == BARB_SPAWN_UPRISING || eReason == BARB_SPAWN_PARTISANS || eReason == BARB_SPAWN_HORDE_QUEST || eReason == BARB_SPAWN_CITY_STATE_CAPTURE)
		{
			eUniqueUnitPlayer = pPlot->getOwner();
		}
	}

	// Can we use resources?
	vector<ResourceTypes> vValidResources;
	if (eReason == BARB_SPAWN_NEW_ENCAMPMENT || eReason == BARB_SPAWN_FROM_ENCAMPMENT || eReason == BARB_SPAWN_FROM_CITY || eReason == BARB_SPAWN_CITY_CAPTURE)
	{
		// First check the current tile
		ResourceTypes eResource = pPlot->getResourceType();
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if (pkResource)
			vValidResources.push_back(eResource);

		// Now look for any others within 3 tiles
		for (int iI = 0; iI < RING_PLOTS[3]; iI++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pPlot,iI);
			if (!pLoopPlot)
				continue;

			if (pLoopPlot->isOwned() && pLoopPlot->getOwner() != BARBARIAN_PLAYER)
				continue;

			eResource = pLoopPlot->getResourceType();
			pkResource = GC.getResourceInfo(eResource);
			if (pkResource && std::find(vValidResources.begin(), vValidResources.end(), eResource) == vValidResources.end())
			{
				vValidResources.push_back(eResource);
			}
		}
	}
	else if (eReason == BARB_SPAWN_UPRISING || eReason == BARB_SPAWN_PARTISANS)
	{
		// We can use any resource owned by the city owner
		PlayerTypes eMajor = pPlot->getOwner();

		for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes)iResourceLoop;
			CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
			if (pkResource)
			{
				if (GET_PLAYER(eMajor).getNumResourceTotal(eResource) > 0)
					vValidResources.push_back(eResource);
			}
		}
	}
	else if (eReason == BARB_SPAWN_HORDE_QUEST || eReason == BARB_SPAWN_CITY_STATE_CAPTURE)
	{
		// First check the current tile
		PlayerTypes eMinor = pPlot->getOwner();
		ResourceTypes eResource = pPlot->getResourceType();
		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if (pkResource)
			vValidResources.push_back(eResource);

		// Now look for any others within 3 tiles
		for (int iI = 0; iI < RING_PLOTS[3]; iI++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pPlot,iI);
			if (!pLoopPlot || pLoopPlot->getOwner() != eMinor)
				continue;

			eResource = pLoopPlot->getResourceType();
			pkResource = GC.getResourceInfo(eResource);
			if (pkResource && std::find(vValidResources.begin(), vValidResources.end(), eResource) == vValidResources.end())
			{
				vValidResources.push_back(eResource);
			}
		}
	}

	// Are we spawning on the tile itself? First priority is to fill it!
	bool bSpawnedOnPlot = false;
	int iNumUnitsSpawned = 0;
	if (bSpawnOnPlot)
	{
		UnitAITypes ePreferredType = (eReason == BARB_SPAWN_FROM_CITY || eReason == BARB_SPAWN_CITY_STATE_CAPTURE) ? UNITAI_RANGED : UNITAI_DEFENSE;
		UnitTypes eUnit = GetRandomBarbarianUnitType(pPlot, ePreferredType, eUniqueUnitPlayer, vValidResources, pPlot->GetPseudoRandomSeed().mix(iNumUnits).mix(GC.getGame().getGameTurn()));
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if (pkUnitInfo)
		{
			CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY(), pkUnitInfo->GetDefaultUnitAIType());
			if (pUnit)
			{
				bSpawnedOnPlot = true;
				iCount++;
				iNumUnitsSpawned++;
				iNumUnits--;
				pUnit->finishMoves();

				if (MOD_EVENTS_BARBARIANS)
					GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansSpawnedUnit, pPlot->getX(), pPlot->getY(), eUnit);
			}
		}
	}

	// Make sure we're not exceeding any limits
	if (iCount >= iMaxBarbarians || iNumUnits == 0)
	{
		if (eReason == BARB_SPAWN_FROM_ENCAMPMENT || eReason == BARB_SPAWN_FROM_CITY)
		{
			// Reset the spawn counter
			if (bSpawnedOnPlot)
				ActivateBarbSpawner(pPlot);
		}
		if (GC.getLogging() && GC.getAILogging())
		{
			CvString strLogString;
			char const*reasons[] = {
				"New Encampment",
				"Spawned from Encampment",
				"Spawned from City",
				"Captured City",
				"Plundered Trade Route",
				"Partisans from City Razing",
				"Uprising from Unhappiness",
				"Horde or Rebellion Quest",
				"Captured City-State after failed Horde or Rebellion Quest",
				"Event" };

			strLogString.Format("Spawned single Barbarian unit. Spawn Reason: %s, X: %d, Y: %d", reasons[eReason], pPlot->getX(), pPlot->getY());
			GET_PLAYER(BARBARIAN_PLAYER).GetTacticalAI()->LogTacticalMessage(strLogString);
		}
		return;
	}
	else if ((iCount + iNumUnits) >= iMaxBarbarians)
	{
		iNumUnits = iMaxBarbarians - iCount;
	}

	// Where can we spawn Barbarians?
	bool bCanSpawnBoats = (eReason == BARB_SPAWN_FROM_ENCAMPMENT || eReason == BARB_SPAWN_FROM_CITY || eReason == BARB_SPAWN_CITY_CAPTURE) ? GC.getGame().getGameTurn() >= /*30 in CP, 20 in VP*/ GD_INT_GET(BARBARIAN_NAVAL_UNIT_START_TURN_SPAWN) : false;
	int iRingsCompleted = iMinSpawnRadius - 1;

	do
	{
		int iIteratorStart = iRingsCompleted > 0 ? RING_PLOTS[iRingsCompleted] : 0;
		int iNextRing = iRingsCompleted + 1;

		// Too far away?
		if (iNextRing > iMaxSpawnRadius)
			break;

		std::vector<CvPlot*> vBarbSpawnPlots;
		for (int iI = iIteratorStart; iI < RING_PLOTS[iNextRing]; iI++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pPlot,iI);
			if (!pLoopPlot || pLoopPlot == pPlot || pLoopPlot->isIce() || pLoopPlot->isLake())
				continue;

			if (pLoopPlot->isCity() && pLoopPlot->getOwner() != BARBARIAN_PLAYER)
				continue;

			if (!bCanSpawnBoats && pLoopPlot->isWater())
				continue;

			if (pLoopPlot->getNumUnits() > 0 || !pLoopPlot->isValidMovePlot(BARBARIAN_PLAYER))
				continue;

			vBarbSpawnPlots.push_back(pLoopPlot);
		}

		while (iNumUnits > 0)
		{
			if (vBarbSpawnPlots.empty())
				break;

			// Choose a random plot
			uint uIndex = GC.getGame().urandLimitExclusive(vBarbSpawnPlots.size(), pPlot->GetPseudoRandomSeed());
			CvPlot* pSpawnPlot = vBarbSpawnPlots[uIndex];
			UnitAITypes eUnitAI = pSpawnPlot->isWater() ? UNITAI_ATTACK_SEA : UNITAI_FAST_ATTACK;

			// Pick a unit
			UnitTypes eUnit = GetRandomBarbarianUnitType(pSpawnPlot, eUnitAI, eUniqueUnitPlayer, vValidResources, CvSeeder(vBarbSpawnPlots.size()).mix(iNumUnits).mix(GC.getGame().getGameTurn()).mix(iNumUnitsSpawned));
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
			if (pkUnitInfo)
			{
				// And spawn it!
				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pSpawnPlot->getX(), pSpawnPlot->getY(), eUnitAI);
				if (pUnit)
				{
					iNumUnitsSpawned++;
					iNumUnits--;
					pUnit->finishMoves();

					// If this is a revolt, notify the player
					if (iNumUnitsSpawned == 1 && eReason == BARB_SPAWN_UPRISING)
					{
						CvNotifications* pNotifications = GET_PLAYER(pPlot->getOwner()).GetNotifications();
						if (pNotifications)
						{
							Localization::String strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_REBELS");
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_REBELS_SUMMARY");
							pNotifications->Add(NOTIFICATION_REBELS, strMessage.toUTF8(), strSummary.toUTF8(), pPlot->getX(), pPlot->getY(), eUnit, BARBARIAN_PLAYER);
						}
					}

					if (MOD_EVENTS_BARBARIANS)
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansSpawnedUnit, pSpawnPlot->getX(), pSpawnPlot->getY(), eUnit);
				}
			}

			// Make sure to erase the plot we chose from the list!
			vBarbSpawnPlots.erase(vBarbSpawnPlots.begin() + uIndex);
		}

		iRingsCompleted++;
	}
	while (iRingsCompleted < iMaxSpawnRadius && iNumUnits > 0);

	// Reset the spawn counter
	if (eReason == BARB_SPAWN_FROM_ENCAMPMENT || eReason == BARB_SPAWN_FROM_CITY)
	{
		if (iNumUnitsSpawned > 0)
			ActivateBarbSpawner(pPlot);
	}

	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strLogString;
		char const*reasons[] = {
			"New Encampment",
			"Spawned from Encampment",
			"Spawned from City",
			"Captured City",
			"Plundered Trade Route",
			"Partisans from City Razing",
			"Uprising from Unhappiness",
			"Horde or Rebellion Quest",
			"Captured City-State after failed Horde or Rebellion Quest",
			"Event" };

		strLogString.Format("Spawned %d Barbarian unit(s). Spawn Reason: %s, units were spawned within %d plots of X: %d, Y: %d", iNumUnitsSpawned, reasons[eReason], iMaxSpawnRadius, pPlot->getX(), pPlot->getY());
		GET_PLAYER(BARBARIAN_PLAYER).GetTacticalAI()->LogTacticalMessage(strLogString);
	}
}

//	--------------------------------------------------------------------------------
UnitTypes CvBarbarians::GetRandomBarbarianUnitType(CvPlot* pPlot, UnitAITypes ePreferredUnitAI, PlayerTypes eUniqueUnitPlayer, vector<ResourceTypes>& vValidResources, CvSeeder additionalSeed)
{
	CvPlayerAI& kBarbarianPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	vector<OptionWithScore<UnitTypes>> candidates;
	DomainTypes eDomain = pPlot->getDomain();

	for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		const UnitTypes eLoopUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
		if (!pkUnitInfo)
			continue;

		// No ships on land, no land on water, no air units
		if (eDomain != (DomainTypes)pkUnitInfo->GetDomainType())
			continue;

		// No civilians
		if (pkUnitInfo->GetCombat() <= 0 && pkUnitInfo->GetRangedCombat() <= 0)
			continue;

		const UnitClassTypes eUnitClass = (UnitClassTypes)pkUnitInfo->GetUnitClassType();
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
		if (!pkUnitClassInfo)
			continue;

		// UU Shenanigans

		UnitTypes eDefaultUnit = (UnitTypes)pkUnitClassInfo->getDefaultUnitIndex();
		UnitTypes eBarbUnitOverride = kBarbarianPlayer.GetSpecificUnitType(eUnitClass);
		bool bIsUU = eLoopUnit != eDefaultUnit;

		// Are Barbarians not allowed to use this unit class? (XML override)
		if (eBarbUnitOverride == NO_UNIT)
			continue;

		// Barbarians have special replacements for a few units - if this is one of those cases, don't consider the default unit
		if (!bIsUU && eBarbUnitOverride != eDefaultUnit && eBarbUnitOverride != eLoopUnit)
			continue;

		// If this is another player's unique unit, we must be allowed to use it
		if (bIsUU && eLoopUnit != eBarbUnitOverride)
		{
			if (eUniqueUnitPlayer == NO_PLAYER)
				continue;

			if (GET_PLAYER(eUniqueUnitPlayer).isMajorCiv() && GET_PLAYER(eUniqueUnitPlayer).GetSpecificUnitType(eUnitClass) != eLoopUnit)
				continue;

			if (GET_PLAYER(eUniqueUnitPlayer).isMinorCiv() && GET_PLAYER(eUniqueUnitPlayer).GetMinorCivAI()->GetUniqueUnit() != eLoopUnit)
				continue;
		}

		// End UU Shenanigans

		// Barbarians can't spawn any units that can't be trained
		if (pkUnitInfo->GetProductionCost() == -1)
			continue;

		// Barbarians can't spawn any units that require policies or projects
		PolicyTypes ePolicy = (PolicyTypes)pkUnitInfo->GetPolicyType();
		if (ePolicy != NO_POLICY)
			continue;

		ProjectTypes ePrereqProject = (ProjectTypes)pkUnitInfo->GetProjectPrereq();
		if (ePrereqProject != NO_PROJECT)
			continue;

		// If the unit requires resources, that resource must be nearby and allowed to be used
		bool bRequiresResources = false;
		for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
			CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
			if (pkResourceInfo)
			{
				int iNumResourceRequired = pkUnitInfo->GetResourceQuantityRequirement(eResource);
				int iNumResourceTotal = MOD_UNITS_RESOURCE_QUANTITY_TOTALS ? pkUnitInfo->GetResourceQuantityTotal(eResource) : 0;

				if (iNumResourceRequired > 0 || iNumResourceTotal > 0)
				{
					bRequiresResources = true;
					if (std::find(vValidResources.begin(), vValidResources.end(), eResource) == vValidResources.end())
						continue;
				}
			}
		}

		// Must have prereq tech(s) and must NOT have obsolete tech
		TechTypes ePrereqTech = (TechTypes)pkUnitInfo->GetPrereqAndTech(), eObsoleteTech = (TechTypes)pkUnitInfo->GetObsoleteTech();
		if (ePrereqTech != NO_TECH && !GET_TEAM(BARBARIAN_TEAM).GetTeamTechs()->HasTech(ePrereqTech))
			continue;

		for (int iI = 0; iI < /*3*/ GD_INT_GET(NUM_UNIT_AND_TECH_PREREQS); iI++)
		{
			ePrereqTech = (TechTypes)pkUnitInfo->GetPrereqAndTechs(iI);
			if (ePrereqTech != NO_TECH && !GET_TEAM(BARBARIAN_TEAM).GetTeamTechs()->HasTech(ePrereqTech))
				continue;
		}

		if (eObsoleteTech != NO_TECH && GET_TEAM(BARBARIAN_TEAM).GetTeamTechs()->HasTech(eObsoleteTech))
			continue;

		// Game Unit Class Max
		if (GC.getGame().isUnitClassMaxedOut(eUnitClass))
			continue;

		// Team Unit Class Max
		if (GET_TEAM(BARBARIAN_TEAM).isUnitClassMaxedOut(eUnitClass))
			continue;

		// Player Unit Class Max
		if (kBarbarianPlayer.isUnitClassMaxedOut(eUnitClass))
			continue;

		// This is a valid unit! Now score it.
		int iScore = 0;
		if (pkUnitInfo->GetRangedCombat() > 0)
		{
			iScore += pkUnitInfo->GetRangedCombat();
		}
		else
		{
			iScore += pkUnitInfo->GetCombat();
		}

		// Units with no resource requirements have 50% more weight
		if (!bRequiresResources)
		{
			iScore *= /*150*/ GD_INT_GET(BARBARIAN_UNIT_SPAWN_NO_RESOURCE_MULTIPLIER);
			iScore /= 2;
		}

		// If unique units are permitted, they have 50% more weight as well
		if (bIsUU)
		{
			iScore *= /*150*/ GD_INT_GET(BARBARIAN_UNIT_SPAWN_UU_MULTIPLIER);
			iScore /= 2;
		}

		// Preferred unit AI type?
		if (pkUnitInfo->GetDefaultUnitAIType() == ePreferredUnitAI)
		{
			iScore *= /*200*/ GD_INT_GET(BARBARIAN_UNIT_SPAWN_PREFERRED_TYPE);
			iScore /= 100;
		}

		if (iScore > 0)
			candidates.push_back(OptionWithScore<UnitTypes>(eLoopUnit, iScore));
	}

	if (candidates.empty())
		return NO_UNIT;

	//choose from top 5
	int iNumCandidates = /*5*/ range(GD_INT_GET(BARBARIAN_UNIT_SPAWN_NUM_CANDIDATES), 1, candidates.size());
	UnitTypes eBestUnit = PseudoRandomChoiceByWeight(candidates, NO_UNIT, 5, pPlot->GetPseudoRandomSeed().mix(additionalSeed));

	//custom override
	if (MOD_EVENTS_BARBARIANS)
	{
		int iValue = 0;
		if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_BarbariansCampGetSpawnUnit, pPlot->getX(), pPlot->getY(), eBestUnit) == GAMEEVENTRETURN_VALUE) 
		{
			// Defend against modder stupidity!
			UnitTypes eUnitType = (UnitTypes)iValue;
			if (eUnitType != NO_UNIT && GC.getUnitInfo(eUnitType) != NULL)
			{
				eBestUnit = eUnitType;
			}
		}
	}

	return eBestUnit;
}