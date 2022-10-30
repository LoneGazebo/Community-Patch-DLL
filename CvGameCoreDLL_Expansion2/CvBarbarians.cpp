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
bool CvBarbarians::CanBarbariansSpawn()
{
	return GC.getGame().getElapsedGameTurns() >= 10;
}

int CvBarbarians::GetBarbSpawnerCounter(CvPlot* pPlot)
{
	if (pPlot != 0) {
		return m_aiBarbSpawnerCounter[pPlot->GetPlotIndex()];
}

	return -1;
}

void CvBarbarians::SetBarbSpawnerCounter(CvPlot* pPlot, int iNewValue)
{
	if (pPlot != 0) {
		m_aiBarbSpawnerCounter[pPlot->GetPlotIndex()] = iNewValue;
}
}

void CvBarbarians::ChangeBarbSpawnerCounter(CvPlot* pPlot, int iChange)
{
	SetBarbSpawnerCounter(pPlot, GetBarbSpawnerCounter(pPlot) + iChange);
}

void CvBarbarians::DoBarbSpawnerAttacked(CvPlot* pPlot)
{
	// Halve the current value and subtract 1 to round up
	int iNewValue = (GetBarbSpawnerCounter(pPlot) / 2) - 1;
	SetBarbSpawnerCounter(pPlot, std::max(iNewValue, 0));
}

bool CvBarbarians::ShouldSpawnBarbFromCamp(CvPlot* pPlot)
{
	if (MOD_EVENTS_BARBARIANS && GAMEEVENTINVOKE_TESTALL(GAMEEVENT_BarbariansCampCanSpawnUnit, pPlot->getX(), pPlot->getY()) == GAMEEVENTRETURN_FALSE) {
		return false;
}

	return GetBarbSpawnerCounter(pPlot) == 0;
}

bool CvBarbarians::ShouldSpawnBarbFromCity(CvPlot* pPlot)
{
	return GetBarbSpawnerCounter(pPlot) == 0;
}

//	---------------------------------------------------------------------------
int CvBarbarians::GetBarbSpawnerNumUnitsSpawned(CvPlot* pPlot)
{
	if (pPlot != 0) {
		return m_aiBarbSpawnerNumUnitsSpawned[pPlot->GetPlotIndex()];
}

	return -1;
}

void CvBarbarians::SetBarbSpawnerNumUnitsSpawned(CvPlot* pPlot, int iNewValue)
{
	if (pPlot != 0) {
		m_aiBarbSpawnerNumUnitsSpawned[pPlot->GetPlotIndex()] = iNewValue;
}
}

void CvBarbarians::ChangeBarbSpawnerNumUnitsSpawned(CvPlot* pPlot, int iChange)
{
	SetBarbSpawnerNumUnitsSpawned(pPlot, GetBarbSpawnerNumUnitsSpawned(pPlot) + iChange);
}

//	---------------------------------------------------------------------------
/// Gameplay informing us when a Camp/City has either been created or spawned a Unit so we can reseed the spawn counter
void CvBarbarians::ActivateBarbSpawner(CvPlot* pPlot)
{
	if (pPlot == 0) {
		return;
}

	// Probably means this is being called as CvWorldBuilderMapLoaded is adding camps, MapInit() will follow soon and set everything up correctly
	if (m_aiBarbSpawnerNumUnitsSpawned == NULL) {
		return;
}

	CvGame& kGame = GC.getGame();
	int iNumTurnsToSpawn = 0;

	// Cities spawn units 2x as often as camps
	if (pPlot->isCity())
	{
		iNumTurnsToSpawn = 6;
		iNumTurnsToSpawn += kGame.isReallyNetworkMultiPlayer() ? 3 : kGame.getSmallFakeRandNum(5, *pPlot);
	}
	else
	{
		iNumTurnsToSpawn = 12;
		iNumTurnsToSpawn += kGame.isReallyNetworkMultiPlayer() ? 5 : kGame.getSmallFakeRandNum(10, *pPlot);
	}

	// Chill
	if (kGame.isOption(GAMEOPTION_CHILL_BARBARIANS))
	{
		if (pPlot->isCity())
		{
			iNumTurnsToSpawn *= 3;
			iNumTurnsToSpawn /= 2;
		}
		else {
			iNumTurnsToSpawn *= 2;
}
	}

	// Raging
	if (kGame.isOption(GAMEOPTION_RAGING_BARBARIANS)) {
		iNumTurnsToSpawn /= 2;
}

	// Reduce turns between spawn if we've pumped out more guys (meaning we're further into the game)
	// -1 turns if we've spawned one Unit, -3 turns if we've spawned three
	iNumTurnsToSpawn -= min(3, GetBarbSpawnerNumUnitsSpawned(pPlot));

	// Increment # of barbs spawned
	// This starts at -1, so when a camp is first created it'll bump up to 0, which is correct
	ChangeBarbSpawnerNumUnitsSpawned(pPlot, 1);

	// Difficulty level can add time between spawns
	iNumTurnsToSpawn += kGame.getHandicapInfo().getBarbSpawnMod();

	// Game Speed can increase or decrease amount of time between spawns (ranges from 67 on Quick to 400 on Marathon)
	CvGameSpeedInfo* pGameSpeedInfo = GC.getGameSpeedInfo(kGame.getGameSpeedType());
	if (pGameSpeedInfo != 0)
	{
		iNumTurnsToSpawn *= pGameSpeedInfo->getBarbPercent();
		iNumTurnsToSpawn /= 100;
	}

	SetBarbSpawnerCounter(pPlot, std::max(iNumTurnsToSpawn, 2));
}

//	--------------------------------------------------------------------------------
/// Camp cleared, so reset counter
void CvBarbarians::DoBarbCampCleared(CvPlot* pPlot, PlayerTypes ePlayer, CvUnit* pUnit)
{
	if (pPlot == 0) {
		return;
}

	int iCooldownTimer = /*15*/ GD_INT_GET(BARBARIAN_CAMP_CLEARED_MIN_TURNS_TO_RESPAWN);
	SetBarbSpawnerCounter(pPlot, (iCooldownTimer * -1) - 1);

	pPlot->AddArchaeologicalRecord(CvTypes::getARTIFACT_BARBARIAN_CAMP(), ePlayer, NO_PLAYER);

	if (ePlayer != NO_PLAYER)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
		CvCity* pBestCity = kPlayer.GetClosestCityByPlots(pPlot);
		if (pBestCity != 0)
		{
			int iNumGold = GC.getGame().getHandicapInfo().getBarbCampGold();
			kPlayer.doInstantYield(INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED, false, NO_GREATPERSON, NO_BUILDING, iNumGold, MOD_BALANCE_VP, NO_PLAYER, NULL, false, pBestCity, false, true, false, YIELD_GOLD, pUnit);

			if (GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) > 0 || GET_PLAYER(ePlayer).GetBarbarianCombatBonus(true) > 0)
			{
				int iCulturePoints = (GET_PLAYER(ePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CULTURE_FROM_BARBARIAN_KILLS) / 5);
				if (iCulturePoints <= 0) {
					iCulturePoints = GET_PLAYER(ePlayer).GetBarbarianCombatBonus(true);
}

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
				iNumGold *= (100 + GET_PLAYER(ePlayer).GetPlayerTraits()->GetPlunderModifier());
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

					if (!minorPlayer.isAlive()) {
						continue;
}

					CvMinorCivAI* pMinorCivAI = minorPlayer.GetMinorCivAI();
					pMinorCivAI->DoTestActiveQuestsForPlayer(ePlayer, /*bTestComplete*/ true, /*bTestObsolete*/ false, MINOR_CIV_QUEST_KILL_CAMP);
				}
			}
		}
	}

	if (MOD_EVENTS_BARBARIANS) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansCampCleared, pPlot->getX(), pPlot->getY(), ePlayer);
}
}

/// Barbarian city captured, so reset counter
void CvBarbarians::DoBarbCityCleared(CvPlot* pPlot)
{
	int iCooldownTimer = /*15*/ GD_INT_GET(BARBARIAN_CAMP_CLEARED_MIN_TURNS_TO_RESPAWN);
	SetBarbSpawnerCounter(pPlot, (iCooldownTimer * -1) - 1);
}

//	--------------------------------------------------------------------------------
//return false if stealing is impossible and the unit should do something else instead
bool CvBarbarians::DoStealFromCity(CvUnit* pUnit, CvCity* pCity)
{
	if (!MOD_BALANCE_CORE_BARBARIAN_THEFT) {
		return false;
}

	if ((pUnit == 0) || !pUnit->IsCanAttack() || (pCity == 0)) {
		return false;
}

	//don't steal from ourselves
	if (pCity->isBarbarian()) {
		return false;
}

	//can't steal if guarded
	if (pCity->HasGarrison()) {
		return false;
}

	//pretend the unit attacks this city
	//city pays them off so they don't do damage
	int iAttackerDamage = 0;
	int iDefenderDamage = TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(pCity, pUnit, pUnit->plot(), iAttackerDamage);

	//tried but failed
	if (iDefenderDamage <= 0 || iAttackerDamage >= pUnit->GetCurrHitPoints()) {
		return true;
}

	//they get x turns worth of yields
	int iTheftTurns = max(1, iDefenderDamage / 30 + GC.getGame().getSmallFakeRandNum(5, pUnit->GetID() + GET_PLAYER(pCity->getOwner()).GetPseudoRandomSeed()));

	//but they lose some health in exchange
	pUnit->changeDamage( GC.getGame().getSmallFakeRandNum( min(pUnit->GetCurrHitPoints(),30), iDefenderDamage + GET_PLAYER(pCity->getOwner()).GetPseudoRandomSeed() ) );

	//which yield is affected?
	int iYield = GC.getGame().getSmallFakeRandNum(10, pUnit->plot()->GetPlotIndex() + GET_PLAYER(pCity->getOwner()).GetPseudoRandomSeed());
	if (iYield <= 2)
	{
		int iGold = min(pCity->getBaseYieldRate(YIELD_GOLD) * iTheftTurns, pUnit->GetCurrHitPoints());
		if (iGold > 0)
		{
			GET_PLAYER(pCity->getOwner()).GetTreasury()->ChangeGold(-iGold);

			Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT_CITY_DETAILED");
			strMessage << iGold;
			strMessage << pCity->getNameKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT_CITY");
			strSummary << pCity->getNameKey();

			CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
			if (pNotification != 0)
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
		int iCulture = min(pCity->getJONSCulturePerTurn() * iTheftTurns, pUnit->GetCurrHitPoints());
		if (iCulture > 0)
		{
			GET_PLAYER(pCity->getOwner()).changeJONSCulture(-iCulture);

			Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_CULTURE_THEFT_CITY_DETAILED");
			strMessage << iCulture;
			strMessage << pCity->getNameKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_CULTURE_THEFT_CITY");
			strSummary << pCity->getNameKey();

			CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
			if (pNotification != 0)
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
			iScience = min(pCity->getBaseYieldRate(YIELD_SCIENCE) * iTheftTurns, pUnit->GetCurrHitPoints());
			if (iScience > 0)
			{
				GET_TEAM(pCity->getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, -iScience, pCity->getOwner());

				Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_SCIENCE_THEFT_CITY_DETAILED");
				strMessage << iScience;
				strMessage << pCity->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_SCIENCE_THEFT_CITY");
				strSummary << pCity->getNameKey();

				CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
				if (pNotification != 0)
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
		int iFood = min(pCity->getBaseYieldRate(YIELD_FOOD) * iTheftTurns, pUnit->GetCurrHitPoints());
		if (iFood > 0)
		{
			pCity->changeFood(-iFood);

			Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_FOOD_THEFT_CITY_DETAILED");
			strMessage << iFood;
			strMessage << pCity->getNameKey();
			Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_FOOD_THEFT_CITY");
			strSummary << pCity->getNameKey();

			CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
			if (pNotification != 0)
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
			int iProduction = min(pCity->getBaseYieldRate(YIELD_PRODUCTION) * iTheftTurns, pUnit->GetCurrHitPoints());
			if (iProduction > 0)
			{
				pCity->changeProduction(-iProduction);

				Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_PRODUCTION_THEFT_CITY_DETAILED");
				strMessage << iProduction;
				strMessage << pCity->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_PRODUCTION_THEFT_CITY");
				strSummary << pCity->getNameKey();

				CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
				if (pNotification != 0)
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
	if (pCity == 0) {
		return false;
}

	CvPlayer& owner = GET_PLAYER(pCity->getOwner());
	if (!owner.isMinorCiv() || !owner.GetMinorCivAI()->IsReadyForTakeOver()) {
		return false;
}

	// create a barbarian unit in the city, taking it over
	UnitTypes eUnit = GC.getGame().GetRandomSpawnUnitType(BARBARIAN_PLAYER, /*bIncludeUUs*/ true, /*bIncludeRanged*/ true);
	if (eUnit != NO_UNIT)
	{
		GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pCity->getX(), pCity->getY());
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
void CvBarbarians::DoCamps()
{
	CvGame& kGame = GC.getGame();
	ImprovementTypes eCamp = (ImprovementTypes)GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT);
	ImprovementTypes eEmbassy = (ImprovementTypes)GD_INT_GET(EMBASSY_IMPROVEMENT);
	CvImprovementEntry* pkCampInfo = GC.getImprovementInfo(eCamp);
	int iGameTurn = GC.getGame().getGameTurn();
	int iInitialSpawnTurn = /*0 in CP, 2 in VP*/ std::max(GD_INT_GET(BARBARIAN_INITIAL_SPAWN_TURN), 0);
	if (eCamp == NO_IMPROVEMENT || pkCampInfo == NULL) {
		return;
}
	if (kGame.isOption(GAMEOPTION_NO_BARBARIANS) || kGame.getElapsedGameTurns() < iInitialSpawnTurn) {
		return;
}

	int iNumCampsToAdd = 0;
	bool bInitialSpawning = kGame.getElapsedGameTurns() == iInitialSpawnTurn;

	// At the end of turn 2, spawn camps in 1/8 of eligible tiles
	// In Community Patch only at the end of turn 0, spawn 1/3 of the maximum number of camps
	if (bInitialSpawning)
	{
		iNumCampsToAdd = /*125 in VP, 33 in CP*/ GD_INT_GET(BARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD);
	}
	else
	{
		// In VP, spawn a camp every 1-3 turns, depending on game options
		if (MOD_BALANCE_VP)
		{
			int iGameTurn = kGame.getElapsedGameTurns() - iInitialSpawnTurn;
			int iSpawnRate = 2;
			if (kGame.isOption(GAMEOPTION_RAGING_BARBARIANS))
			{
				iSpawnRate--;
			}
			if (kGame.isOption(GAMEOPTION_CHILL_BARBARIANS))
			{
				iSpawnRate++;
			}

			if (iGameTurn % iSpawnRate == 0) {
				iNumCampsToAdd = 1;
}
		}
		// In Community Patch only, 1/2 chance of spawning a camp each turn (after turn 0)
		else if (GD_INT_GET(BARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING) > 0)
		{
			if (kGame.isReallyNetworkMultiPlayer() ? kGame.getGameTurn() % 2 == 0 : kGame.getSmallFakeRandNum(/*2*/ GD_INT_GET(BARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING), kGame.getGameTurn()*kGame.getGameTurn()) == 0) {
				iNumCampsToAdd = 1;
}
		}
	}

	CvMap& theMap = GC.getMap();
	int iNumCampsInExistence = 0;
	int iNumCoastalCamps = 0;
	int iMajorCapitalMinDistance = /*4*/ GD_INT_GET(BARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL);
	int iBarbCampMinDistance = /*4*/ GD_INT_GET(BARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP);
	int iRecentlyClearedCampMinDistance = /*2*/ GD_INT_GET(BARBARIAN_CAMP_MINIMUM_DISTANCE_RECENTLY_CLEARED_CAMP);
	std::vector<CvPlot*> vPotentialPlots,vPotentialCoastalPlots;
	std::vector<int> MajorCapitals,BarbCamps,RecentlyClearedBarbCamps;

	// Iterate through all plots
	for (int iI = 0; iI < theMap.numPlots(); iI++)
	{
		CvPlot* pLoopPlot = theMap.plotByIndexUnchecked(iI);
		ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

		// Existing Barbarian camp here
		if (eImprovement == eCamp)
		{
			if (GetBarbSpawnerCounter(pLoopPlot) > 0) {
				ChangeBarbSpawnerCounter(pLoopPlot, -1);
}

			if (iBarbCampMinDistance > 0)
				BarbCamps.push_back(pLoopPlot->GetPlotIndex());

			iNumCampsInExistence++;
			if (pLoopPlot->isCoastalLand()) {
				iNumCoastalCamps++;
}

			continue;
		}
		// City here
		else if (pLoopPlot->isCity())
		{
			// Barbarian city?
			if (pLoopPlot->getOwner() == BARBARIAN_PLAYER)
			{
				if (GetBarbSpawnerCounter(pLoopPlot) > 0) {
					ChangeBarbSpawnerCounter(pLoopPlot, -1);
}
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
			if (iRecentlyClearedCampMinDistance > 0)
				RecentlyClearedBarbCamps.push_back(pLoopPlot->GetPlotIndex());

			continue;
		}

		// Not adding any camps this turn? No need to proceed!
		if (iNumCampsToAdd <= 0) {
			continue;
}

		// No camps on Improvements in Community Patch only, and no camps on embassies ever
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (!MOD_BALANCE_VP || eImprovement == eEmbassy) {
				continue;
}
		}

		// No camps on Natural Wonders
		if (pLoopPlot->getFeatureType() != NO_FEATURE && GC.getFeatureInfo(pLoopPlot->getFeatureType())->isNoImprovement()) {
			continue;
}

		// No camps on n-tile islands
		if (pLoopPlot->getLandmass() == -1 || theMap.getLandmass(pLoopPlot->getLandmass())->getNumTiles() <= /*1*/ GD_INT_GET(BARBARIAN_CAMP_MINIMUM_ISLAND_SIZE)) {
			continue;
}

		// No camps on water, in owned territory, adjacent to owned territory, or in impassable tiles
		if (pLoopPlot->isWater() || pLoopPlot->isOwned() || pLoopPlot->IsAdjacentOwnedByTeamOtherThan(BARBARIAN_TEAM) || !pLoopPlot->isValidMovePlot(BARBARIAN_PLAYER)) {
			continue;
}

		// Custom rule preventing Barbarian camps from spawning here?
		if (MOD_EVENTS_BARBARIANS && GAMEEVENTINVOKE_TESTALL(GAMEEVENT_BarbariansCanFoundCamp, pLoopPlot->getX(), pLoopPlot->getY()) == GAMEEVENTRETURN_FALSE) {
			continue;
}

		// No camps on plots in sight of a civ or City-State in Community Patch only
		if (!MOD_BALANCE_VP && pLoopPlot->isVisibleToAnyTeam(false)) {
			continue;
}

		// No camps on occupied plots or those adjacent to a non-Barbarian unit
		if (pLoopPlot->getNumUnits() > 0 || pLoopPlot->IsEnemyUnitAdjacent(BARBARIAN_TEAM)) {
			continue;
}

		// Are Barbarian camps allowed to spawn on a tile with this terrain and this feature?
		if (!pkCampInfo->GetTerrainMakesValid(pLoopPlot->getTerrainType()) || !pkCampInfo->GetFeatureMakesValid(pLoopPlot->getFeatureType())) {
			continue;
}

		// This plot is POTENTIALLY eligible, add it to the list
		vPotentialPlots.push_back(pLoopPlot);

		if (vPotentialPlots.size() > 1)
		{
			//do one iteration of a fisher-yates shuffle
			int iSwap = kGame.isReallyNetworkMultiPlayer() ? vPotentialPlots.size() / 2 : kGame.getSmallFakeRandNum(vPotentialPlots.size(), pLoopPlot->GetPlotIndex()*vPotentialPlots.size()*(iGameTurn+1));
			std::swap(vPotentialPlots[iSwap],vPotentialPlots.back());
		}

		if (pLoopPlot->isCoastalLand())
		{
			vPotentialCoastalPlots.push_back(pLoopPlot);

			if (vPotentialCoastalPlots.size() > 1)
			{
				//do one iteration of a fisher-yates shuffle
				int iSwap = kGame.isReallyNetworkMultiPlayer() ? vPotentialCoastalPlots.size() / 2 : kGame.getSmallFakeRandNum(vPotentialCoastalPlots.size(), pLoopPlot->GetPlotIndex()*vPotentialCoastalPlots.size()*(iGameTurn+1));
				std::swap(vPotentialCoastalPlots[iSwap],vPotentialCoastalPlots.back());
			}
		}
	}

	// Not adding any camps this turn? We're done here!
	if (iNumCampsToAdd <= 0) {
		return;
}

	// How many camps can we have on the map? VP: No limit!
	int iMaximumCamps = MOD_BALANCE_VP ? vPotentialPlots.size() : vPotentialPlots.size() / std::max(theMap.getWorldInfo().getFogTilesPerBarbarianCamp(), 1);
	if (iMaximumCamps <= 0 || iNumCampsInExistence >= iMaximumCamps) {
		return;
}

	// If this is the initial spawning, let's calculate how many camps to place on the map
	if (bInitialSpawning)
	{
		iNumCampsToAdd *= iMaximumCamps;
		iNumCampsToAdd /= MOD_BALANCE_VP ? 1000 : 100;
	}

	// Make sure not to exceed the limit
	if ((iNumCampsInExistence + iNumCampsToAdd) >= iMaximumCamps) {
		iNumCampsToAdd = iMaximumCamps - iNumCampsInExistence;
}

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
	std::vector<PlayerTypes> vThoseWhoSee;
	if (iNumCampsToAdd > 0)
	{
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes ePlayer = (PlayerTypes)iPlayerLoop;
			if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).IsAlwaysSeeBarbCamps())
				vThoseWhoSee.push_back(ePlayer);
		}
	}

	// How many camps should be prioritized to be on the coast?
	int iCoastPercent = /*33*/ GD_INT_GET(BARBARIAN_CAMP_COASTAL_SPAWN_ROLL);

	// Place the camps on the map
	bool bUpdateMapFog = false;
	while (iNumCampsToAdd > 0)
	{
		// Bias towards coastal if we don't have the desired % of coastal camps (and there are valid plots)
		bool bWantsCoastal = !vValidCoastalPlots.empty();
		if (((iNumCampsInExistence * iCoastPercent) / 100) >= iNumCoastalCamps) {
			bWantsCoastal = false;
}

		std::vector<CvPlot*>& vRelevantPlots = bWantsCoastal ? vValidCoastalPlots : vValidPlots;

		// No valid plots to place a camp? We're done.
		if (vRelevantPlots.empty()) {
			break;
}

		// Pick a plot
		int iIndex = kGame.isReallyNetworkMultiPlayer() ? vRelevantPlots.size() / 2 : kGame.getSmallFakeRandNum(vRelevantPlots.size(), vRelevantPlots.size()*iNumCampsInExistence+GC.getGame().getGameTurn());
		CvPlot* pPlot = vRelevantPlots[iIndex];

		// Spawn a camp and a unit to defend it!
		pPlot->setImprovementType(eCamp);
		DoSpawnBarbarianUnit(pPlot, true, true);

		// Add another Unit adjacent to the camp to stir up some trouble
		if (!GC.getGame().isOption(GAMEOPTION_CHILL_BARBARIANS)) {
			DoSpawnBarbarianUnit(pPlot, true, true);
}

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

		iNumCampsToAdd--;
		iNumCampsInExistence++;

		// We're done!
		if (iNumCampsToAdd == 0) {
			break;
}

		// Check to see if any plots in the list are now too close to the new camp, and remove any that are
		vPotentialPlots = vValidPlots;
		vPotentialCoastalPlots = vValidCoastalPlots;
		vValidPlots.clear();
		vValidCoastalPlots.clear();

		int iNewCampX = pPlot->getX();
		int iNewCampY = pPlot->getY();
		for (unsigned int iI = 0; iI < vPotentialPlots.size(); iI++)
		{
			CvPlot* pLoopPlot = vValidPlots[iI];

			// Don't use the same plot again!
			if (pLoopPlot == pPlot)
				continue;

			int iX = pLoopPlot->getX(), iY = pLoopPlot->getY();
			int iDistance = plotDistance(iX, iY, iNewCampX, iNewCampY);
			if (iDistance > iBarbCampMinDistance)
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

	if (bUpdateMapFog) {
		GC.getMap().updateDeferredFog();
}
}

//	--------------------------------------------------------------------------------
void CvBarbarians::DoUnits()
{
	CvGame& kGame = GC.getGame();
	if (!CanBarbariansSpawn() || kGame.isOption(GAMEOPTION_NO_BARBARIANS)) {
		return;
}

	ImprovementTypes eCamp = (ImprovementTypes)GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT);
	CvMap& theMap = GC.getMap();
	for (int iPlotLoop = 0; iPlotLoop < theMap.numPlots(); iPlotLoop++)
	{
		CvPlot* pLoopPlot = theMap.plotByIndexUnchecked(iPlotLoop);
		if (pLoopPlot == 0) {
			continue;
}

		// Found a Camp to spawn near
		if (pLoopPlot->getImprovementType() == eCamp)
		{
			if (ShouldSpawnBarbFromCamp(pLoopPlot))
			{
				DoSpawnBarbarianUnit(pLoopPlot, false, true);
				ActivateBarbSpawner(pLoopPlot);
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Unit spawned from Barbarian Encampment at X: %d, Y: %d", pLoopPlot->getX(), pLoopPlot->getY());
					GET_PLAYER(BARBARIAN_PLAYER).GetTacticalAI()->LogTacticalMessage(strLogString);
				}
			}
		}
		// Found a City to spawn near
		else if (pLoopPlot->isCity() && pLoopPlot->getOwner() == BARBARIAN_PLAYER)
		{
			if (ShouldSpawnBarbFromCity(pLoopPlot))
			{
				DoSpawnBarbarianUnit(pLoopPlot, false, true);
				ActivateBarbSpawner(pLoopPlot);
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Unit spawned from Barbarian City of %s at X: %d, Y: %d", pLoopPlot->getPlotCity()->getName().c_str(), pLoopPlot->getX(), pLoopPlot->getY());
					GET_PLAYER(BARBARIAN_PLAYER).GetTacticalAI()->LogTacticalMessage(strLogString);
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Spawn a Barbarian Unit somewhere adjacent to pPlot
void CvBarbarians::DoSpawnBarbarianUnit(CvPlot* pPlot, bool bIgnoreMaxBarbarians, bool bFinishMoves)
{
	int iRange = /*4*/ GD_INT_GET(MAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE);
	CvGame& kGame = GC.getGame();

	if (pPlot == 0) {
		return;
}

	// Look at nearby Plots to see if there are already too many Barbs nearby
	int iNumNearbyUnits = 0;
	ResourceTypes eCloseResource = NO_RESOURCE;
	ResourceClassTypes eResourceClassRush = (ResourceClassTypes)GC.getInfoTypeForString("RESOURCECLASS_RUSH");

	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			// Cut off the corners of the area we're looking at that we don't want
			CvPlot* pNearbyPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iX, iY, iRange);

			if (pNearbyPlot != NULL)
			{
				if (pNearbyPlot->getResourceType() != NO_RESOURCE && 
					GC.getResourceInfo(pNearbyPlot->getResourceType())->getResourceUsage() == RESOURCEUSAGE_STRATEGIC && 
					GC.getResourceInfo(pNearbyPlot->getResourceType())->getResourceClassType() == eResourceClassRush)
				{
					eCloseResource = pNearbyPlot->getResourceType();
				}

				for (int iNearbyUnitLoop = 0; iNearbyUnitLoop < pNearbyPlot->getNumUnits(); iNearbyUnitLoop++)
				{
					const CvUnit* pUnit = pNearbyPlot->getUnitByIndex(iNearbyUnitLoop);
					if ((pUnit != 0) && pUnit->isBarbarian())
					{
						iNumNearbyUnits++;
					}
				}
			}
		}
	}

	// is this camp/city empty - first priority is to fill it
	if ((pPlot != 0) && pPlot->GetNumCombatUnits() == 0 && (!pPlot->isCity() || (pPlot->isCity() && pPlot->isBarbarian())))
	{
		UnitTypes eUnit = GetRandomBarbarianUnitType(pPlot, UNITAI_RANGED, eCloseResource);
		if (eUnit != NO_UNIT)
		{
			CvUnitEntry* pkUnitDef = GC.getUnitInfo(eUnit);	
			CvUnit* pUnit = NULL;
			if(pkUnitDef != 0)
			{
				pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY(), pkUnitDef->GetDefaultUnitAIType());
				if (pUnit != 0) {
					pUnit->finishMoves();
}
			}

			return;
		}
	}

	if (iNumNearbyUnits <= /*2*/ GD_INT_GET(MAX_BARBARIANS_FROM_CAMP_NEARBY) || bIgnoreMaxBarbarians)
	{
		// Barbs only get boats after some period of time has passed
		bool bCanSpawnBoats = kGame.getElapsedGameTurns() > /*30*/ GD_INT_GET(BARBARIAN_NAVAL_UNIT_START_TURN_SPAWN);

		std::vector<CvPlot*> vBalidBarbSpawnPlots;

		// Look to see if adjacent Tiles are valid locations to spawn a Unit
		for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
		{
			DirectionTypes eDirection = (DirectionTypes) iDirectionLoop;
			CvPlot* pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), eDirection);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getNumUnits() == 0)
				{
					if(pLoopPlot->isValidMovePlot(BARBARIAN_PLAYER))
					{
						if(!pLoopPlot->isCity())
						{
							if(!pLoopPlot->isLake() && !pLoopPlot->isIce())
							{
								// Water Tiles are only valid when the Barbs have the proper Tech
								if(!pLoopPlot->isWater() || bCanSpawnBoats)
								{
									vBalidBarbSpawnPlots.push_back(pLoopPlot);
								}
							}
						}
					}
				}
			}
		}

		// Any valid locations?
		if(vBalidBarbSpawnPlots.size() > 0)
		{
			int iIndex = kGame.isReallyNetworkMultiPlayer() ? vBalidBarbSpawnPlots.size() / 2 : kGame.getSmallFakeRandNum(vBalidBarbSpawnPlots.size(), *pPlot);

			CvPlot* pSpawnPlot = vBalidBarbSpawnPlots[iIndex];
			UnitAITypes eUnitAI = pSpawnPlot->isWater() ? UNITAI_ATTACK_SEA : UNITAI_FAST_ATTACK;
			UnitTypes eUnit = GetRandomBarbarianUnitType(pSpawnPlot, eUnitAI, eCloseResource);
			if(eUnit != NO_UNIT)
			{
				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pSpawnPlot->getX(), pSpawnPlot->getY(), eUnitAI);
				if (bFinishMoves && (pUnit != 0))
				{
					pUnit->finishMoves();
				}

				// Stop units from plundered trade routes ending up in the ocean
				if (bIgnoreMaxBarbarians && (pUnit != 0))
				{
					if (!pUnit->jumpToNearestValidPlot()) {
						pUnit->kill(false);	// Could not find a valid spot!
}

					if ((pUnit != 0) && MOD_EVENTS_BARBARIANS) {
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansSpawnedUnit, pSpawnPlot->getX(), pSpawnPlot->getY(), eUnit);
}
				}
			}
		}
	}
}

//	--------------------------------------------------------------------------------
UnitTypes CvBarbarians::GetRandomBarbarianUnitType(CvPlot* pPlot, UnitAITypes eUnitAI, ResourceTypes eNearbyResource)
{
	CvPlayerAI& kBarbarianPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	CvGame &kGame = GC.getGame();

	vector<OptionWithScore<UnitTypes>> candidates;
	for(int iUnitClassLoop = 0; iUnitClassLoop < GC.getNumUnitClassInfos(); iUnitClassLoop++)
	{
		bool bValid = false;
		CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo((UnitClassTypes)iUnitClassLoop);
		if(pkUnitClassInfo == NULL) {
			continue;
}

		const UnitTypes eLoopUnit = ((UnitTypes)(kBarbarianPlayer.getCivilizationInfo().getCivilizationUnits(iUnitClassLoop)));
		if(eLoopUnit != NO_UNIT)
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
			if(pkUnitInfo == NULL) {
				continue;
}

			CvUnitEntry& kUnit = *pkUnitInfo;
			if (pPlot->getDomain() != kUnit.GetDomainType()) {
				continue;
}

			if (!GET_PLAYER(BARBARIAN_PLAYER).canBarbariansTrain(eLoopUnit, false, eNearbyResource)) {
				continue;
}

			int iValue = 0;
			if (kUnit.GetRangedCombat() > 0)
			{
				iValue += kUnit.GetRangedCombat();
			}
			else if (kUnit.GetCombat() > 0)
			{
				iValue += kUnit.GetCombat();
			}
			else {
				continue; //no civilians!
}

			if (GC.getUnitInfo(eLoopUnit)->GetResourceType() != NO_RESOURCE)
			{
				iValue += 100;
			}

			if(kUnit.GetUnitAIType(eUnitAI)) {
				iValue *= 2;
}

			if (iValue>0)
				candidates.push_back( OptionWithScore<UnitTypes>(eLoopUnit, iValue));
		}
	}

	//choose from top 3
	UnitTypes eBestUnit = PseudoRandomChoiceByWeight(candidates, NO_UNIT, 3, pPlot->GetPlotIndex());

	if (MOD_EVENTS_BARBARIANS)
	{
		int iValue = 0;
		if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_BarbariansCampGetSpawnUnit, pPlot->getX(), pPlot->getY(), eBestUnit) == GAMEEVENTRETURN_VALUE) {
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