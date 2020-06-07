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
//static 
short* CvBarbarians::m_aiPlotBarbCampSpawnCounter = NULL;
short* CvBarbarians::m_aiPlotBarbCampNumUnitsSpawned = NULL;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
short* CvBarbarians::m_aiPlotBarbCitySpawnCounter = NULL;
short* CvBarbarians::m_aiPlotBarbCityNumUnitsSpawned = NULL;
#endif

//	---------------------------------------------------------------------------
bool CvBarbarians::IsPlotValidForBarbCamp(CvPlot* pPlot)
{
	int iRange = 3;
	int iDY;

	int iPlotX = pPlot->getX();
	int iPlotY = pPlot->getY();

	CvMap& kMap = GC.getMap();
	for (int iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			int iLoopPlotX = iPlotX + iDX;
			int iLoopPlotY = iPlotY + iDY;

			// Cut off corners
			if (plotDistance(iPlotX, iPlotY, iLoopPlotX, iLoopPlotY) > iRange)
				continue;

			// If the counter is below -1 that means a camp was cleared recently
			CvPlot* pLoopPlot = kMap.plot(iLoopPlotX, iLoopPlotY);
			if (pLoopPlot)
			{
				if (m_aiPlotBarbCampSpawnCounter[pLoopPlot->GetPlotIndex()] < -1)
					return false;
			}
		}
	}

#if defined(MOD_EVENTS_BARBARIANS)
	if (MOD_EVENTS_BARBARIANS) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_BarbariansCanFoundCamp, iPlotX, iPlotY) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif
	
	return true;
}

//	--------------------------------------------------------------------------------
/// Camp cleared, so reset counter
void CvBarbarians::DoBarbCampCleared(CvPlot* pPlot, PlayerTypes ePlayer)
{
	m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] = -16;

	pPlot->AddArchaeologicalRecord(CvTypes::getARTIFACT_BARBARIAN_CAMP(), ePlayer, NO_PLAYER);

#if defined(MOD_BALANCE_CORE)
	if (ePlayer != NO_PLAYER)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

		// find closest city belonging to the player who cleared the camp

		int iBestCityID = -1;

		int iBestCityDistance = -1;

		int iDistance;

		CvCity* pLoopCity = NULL;
		int iLoop = 0;
		for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			CvPlot* pPlot = pLoopCity->plot();
			if (pPlot)
			{
				iDistance = plotDistance(pPlot->getX(), pPlot->getY(), pLoopCity->getX(), pLoopCity->getY());

				if (iBestCityDistance == -1 || iDistance < iBestCityDistance)
				{
					iBestCityID = pLoopCity->GetID();
					iBestCityDistance = iDistance;
				}
			}
		}

		CvCity* pBestCity = kPlayer.getCity(iBestCityID);

		if (pBestCity != NULL)
		{
			// call one for era scaling, and another for non-era scaling
			GET_PLAYER(ePlayer).doInstantYield(INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pBestCity);
			GET_PLAYER(ePlayer).doInstantYield(INSTANT_YIELD_TYPE_BARBARIAN_CAMP_CLEARED, false, NO_GREATPERSON, NO_BUILDING, 0, false, NO_PLAYER, NULL, false, pBestCity);
		}
	}
#endif

#if defined(MOD_EVENTS_BARBARIANS)
	if (MOD_EVENTS_BARBARIANS) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansCampCleared, pPlot->getX(), pPlot->getY(), ePlayer);
	}
#endif
}

#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
//	--------------------------------------------------------------------------------
/// Barbarian city captured, so reset counter
void CvBarbarians::DoBarbCityCleared(CvPlot* pPlot)
{
	m_aiPlotBarbCitySpawnCounter[pPlot->GetPlotIndex()] = -16;
}
#endif

//	--------------------------------------------------------------------------------
/// What turn are we now allowed to Spawn Barbarians on?
bool CvBarbarians::CanBarbariansSpawn()
{
	CvGame& kGame = GC.getGame();
	if (kGame.getElapsedGameTurns() < 10)
	{
		return false;
	}

	return true;
}

//	--------------------------------------------------------------------------------
/// Determines when to Spawn a new Barb Unit from a Camp
bool CvBarbarians::ShouldSpawnBarbFromCamp(CvPlot* pPlot)
{
#if defined(MOD_EVENTS_BARBARIANS)
	if (MOD_EVENTS_BARBARIANS) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_BarbariansCampCanSpawnUnit, pPlot->getX(), pPlot->getY()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif
	
	if (m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] == 0)
	{
		return true;
	}

	return false;
}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
//	--------------------------------------------------------------------------------
/// Determines when to Spawn a new Barb Unit from a City
bool CvBarbarians::ShouldSpawnBarbFromCity(CvPlot* pPlot)
{
	if (m_aiPlotBarbCitySpawnCounter[pPlot->GetPlotIndex()] == 0)
	{
		return true;
	}

	return false;
}
#endif
//	--------------------------------------------------------------------------------
/// Gameplay informing us when a Camp has either been created or spawned a Unit so we can reseed the spawn counter
void CvBarbarians::DoCampActivationNotice(CvPlot* pPlot)
{
	if (!pPlot)
		return;

	CvGame& kGame = GC.getGame();
	// Default to between 8 and 12 turns per spawn
#if defined(MOD_CORE_REDUCE_RANDOMNESS)
	int iSpawnTurnAddition = 12;
	bool isMP = kGame.isReallyNetworkMultiPlayer();
	int iRand = kGame.getSmallFakeRandNum(10, *pPlot);
	int iAddVal = isMP ? 5 : iRand;
	int iNumTurnsToSpawn = iSpawnTurnAddition + iAddVal;
#else
	int iNumTurnsToSpawn = 8 + kGame.getJonRandNum(5, "Barb Spawn Rand call");
#endif

	if (kGame.isOption(GAMEOPTION_CHILL_BARBARIANS))
	{
		iNumTurnsToSpawn *= 2;
	}

	// Raging
	if (kGame.isOption(GAMEOPTION_RAGING_BARBARIANS))
		iNumTurnsToSpawn /= 2;

#if defined(MOD_BUGFIX_BARB_CAMP_SPAWNING)
	if (m_aiPlotBarbCampNumUnitsSpawned == NULL) {
		// Probably means we are being called as CvWorldBuilderMapLoaded is adding camps, MapInit() will follow soon and set everything up correctly
		return;
	}
#endif
		
	// Num Units Spawned
	int iNumUnitsSpawned = m_aiPlotBarbCampNumUnitsSpawned[pPlot->GetPlotIndex()];

	// Reduce turns between spawn if we've pumped out more guys (meaning we're further into the game)
	iNumTurnsToSpawn -= min(3, iNumUnitsSpawned);	// -1 turns if we've spawned one Unit, -3 turns if we've spawned three

	// Increment # of barbs spawned from this camp
	m_aiPlotBarbCampNumUnitsSpawned[pPlot->GetPlotIndex()]++;	// This starts at -1 so when a camp is first created it will bump up to 0, which is correct

	//// If it's too early to spawn then add in a small amount to delay things a bit - between 3 and 6 extra turns
	//if (CanBarbariansSpawn())
	//{
	//	iNumTurnsToSpawn += 3;
	//	iNumTurnsToSpawn += auto_ptr<ICvGame1> pGame = GameCore::GetGame();\n.getJonRandNum(4, "Early game Barb Spawn Rand call");
	//}

	// Difficulty level can add time between spawns
	iNumTurnsToSpawn += kGame.getHandicapInfo().getBarbSpawnMod();

	// Game Speed can increase or decrease amount of time between spawns (ranges from 67 on Quick to 400 on Marathon)
	CvGameSpeedInfo* pGameSpeedInfo = GC.getGameSpeedInfo(kGame.getGameSpeedType());
	if (pGameSpeedInfo)
	{
		iNumTurnsToSpawn *= pGameSpeedInfo->getBarbPercent();
		iNumTurnsToSpawn /= 100;
	}

	m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] = iNumTurnsToSpawn;
}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
//	--------------------------------------------------------------------------------
/// Gameplay informing us when a City has either been acquired or spawned a Unit so we can reseed the spawn counter
void CvBarbarians::DoCityActivationNotice(CvPlot* pPlot)
{
	if (!pPlot)
		return;

	CvGame& kGame = GC.getGame();
	// Default to between 8 and 12 turns per spawn
	//bumped a bit - too many barbs gets annoying.

#if defined(MOD_CORE_REDUCE_RANDOMNESS)
	bool isMP = kGame.isReallyNetworkMultiPlayer();
	int iRand = kGame.getSmallFakeRandNum(10, *pPlot);
	int iAddVal = isMP ? 5 : iRand;
	int iNumTurnsToSpawn = 7 + iAddVal;
#else
	int iNumTurnsToSpawn = 15 + kGame.getJonRandNum(5, "Barb Spawn Rand call");
#endif

	if (kGame.isOption(GAMEOPTION_CHILL_BARBARIANS))
	{
		iNumTurnsToSpawn *= 3;
		iNumTurnsToSpawn /= 2;
	}

	// Raging
	if (kGame.isOption(GAMEOPTION_RAGING_BARBARIANS))
		iNumTurnsToSpawn /= 2;

#if defined(MOD_BUGFIX_BARB_CAMP_SPAWNING)
	if (m_aiPlotBarbCityNumUnitsSpawned == NULL) {
		return;
	}
#endif
		
	// Num Units Spawned
	int iNumUnitsSpawned = m_aiPlotBarbCityNumUnitsSpawned[pPlot->GetPlotIndex()];

	// Reduce turns between spawn if we've pumped out more guys (meaning we're further into the game)
	iNumTurnsToSpawn -= min(3, iNumUnitsSpawned);	// -1 turns if we've spawned one Unit, -3 turns if we've spawned three

	// Increment # of barbs spawned from this city
	m_aiPlotBarbCityNumUnitsSpawned[pPlot->GetPlotIndex()]++;	// This starts at -1 so when a city is first acquired it will bump up to 0, which is correct

	// Difficulty level can add time between spawns
	iNumTurnsToSpawn += kGame.getHandicapInfo().getBarbSpawnMod();

	// Game Speed can increase or decrease amount of time between spawns (ranges from 67 on Quick to 400 on Marathon)
	CvGameSpeedInfo* pGameSpeedInfo = GC.getGameSpeedInfo(kGame.getGameSpeedType());
	if (pGameSpeedInfo)
	{
		iNumTurnsToSpawn *= pGameSpeedInfo->getBarbPercent();
		iNumTurnsToSpawn /= 100;
	}

	m_aiPlotBarbCitySpawnCounter[pPlot->GetPlotIndex()] = iNumTurnsToSpawn;
}
//	--------------------------------------------------------------------------------
/// Gameplay informing us when a city has been attacked - make it more likely to spawn
void CvBarbarians::DoCityAttacked(CvPlot* pPlot)
{
	int iCounter = m_aiPlotBarbCitySpawnCounter[pPlot->GetPlotIndex()];

	// Halve the amount of time to spawn
	int iNewValue = iCounter / 2;
	iNewValue--;
	if (iNewValue <= 0)
	{
		iNewValue = 0;
	}

	m_aiPlotBarbCitySpawnCounter[pPlot->GetPlotIndex()] = iNewValue;
}
#endif
//	--------------------------------------------------------------------------------
/// Gameplay informing us when a camp has been attacked - make it more likely to spawn
void CvBarbarians::DoCampAttacked(CvPlot* pPlot)
{
	int iCounter = m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()];

	// Halve the amount of time to spawn
	int iNewValue = iCounter / 2;
#if defined(MOD_BALANCE_CORE)
	//And reduce by one to 'round up' a change.
	iNewValue--;
	if (iNewValue <= 0)
	{
		iNewValue = 0;
	}
#endif

	m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] = iNewValue;
}

//	---------------------------------------------------------------------------
/// Called every turn
void CvBarbarians::DoCampSpawnCounter()
{
	CvGame &kGame = GC.getGame();
	const ImprovementTypes eCamp = kGame.GetBarbarianCampImprovementType();

	CvMap& kMap = GC.getMap();
	int iWorldNumPlots = kMap.numPlots();
	for (int iPlotLoop = 0; iPlotLoop < iWorldNumPlots; iPlotLoop++)
	{
		if (m_aiPlotBarbCampSpawnCounter[iPlotLoop] > 0)
		{
			// No Camp here any more
			CvPlot* pPlot = kMap.plotByIndex(iPlotLoop);
			if (pPlot->getImprovementType() != eCamp)
			{
				m_aiPlotBarbCampSpawnCounter[iPlotLoop] = -1;
				m_aiPlotBarbCampNumUnitsSpawned[iPlotLoop] = -1;
			}
			else
			{
				m_aiPlotBarbCampSpawnCounter[iPlotLoop]--;
			}
		}

		// Counter is negative, meaning a camp was cleared here recently and isn't allowed to respawn in the area for a while
		else if (m_aiPlotBarbCampSpawnCounter[iPlotLoop] < -1)
		{
			m_aiPlotBarbCampSpawnCounter[iPlotLoop]++;
		}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		if (m_aiPlotBarbCitySpawnCounter[iPlotLoop] > 0)
		{
			// No City here any more
			CvPlot* pPlot = kMap.plotByIndex(iPlotLoop);
			if (pPlot->isCity() && pPlot->getOwner() != BARBARIAN_PLAYER)
			{
				m_aiPlotBarbCitySpawnCounter[iPlotLoop] = -1;
				m_aiPlotBarbCityNumUnitsSpawned[iPlotLoop] = -1;
			}
			else
			{
				m_aiPlotBarbCitySpawnCounter[iPlotLoop]--;
			}
		}

		// Counter is negative, meaning a camp was cleared here recently and isn't allowed to respawn in the area for a while
		else if (m_aiPlotBarbCitySpawnCounter[iPlotLoop] < -1)
		{
			m_aiPlotBarbCitySpawnCounter[iPlotLoop]++;
		}
#endif
	}
}

//	--------------------------------------------------------------------------------
void CvBarbarians::MapInit(int iWorldNumPlots)
{
	Uninit();

	int iI;

	if (iWorldNumPlots > 0)
	{
		if (m_aiPlotBarbCampSpawnCounter == NULL)
		{
			m_aiPlotBarbCampSpawnCounter = FNEW(short[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
		}
		if (m_aiPlotBarbCampNumUnitsSpawned == NULL)
		{
			m_aiPlotBarbCampNumUnitsSpawned = FNEW(short[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
		}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		if (m_aiPlotBarbCitySpawnCounter == NULL)
		{
			m_aiPlotBarbCitySpawnCounter = FNEW(short[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
		}
		if (m_aiPlotBarbCityNumUnitsSpawned == NULL)
		{
			m_aiPlotBarbCityNumUnitsSpawned = FNEW(short[iWorldNumPlots], c_eCiv5GameplayDLL, 0);
		}
#endif

		// Default values
		for (iI = 0; iI < iWorldNumPlots; ++iI)
		{
			m_aiPlotBarbCampSpawnCounter[iI] = -1;
			m_aiPlotBarbCampNumUnitsSpawned[iI] = -1;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
			m_aiPlotBarbCitySpawnCounter[iI] = -1;
			m_aiPlotBarbCityNumUnitsSpawned[iI] = -1;
#endif
		}
	}
}

//	---------------------------------------------------------------------------
/// Uninit
void CvBarbarians::Uninit()
{
	if (m_aiPlotBarbCampSpawnCounter != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiPlotBarbCampSpawnCounter);
	}

	if (m_aiPlotBarbCampNumUnitsSpawned != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiPlotBarbCampNumUnitsSpawned);
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	if (m_aiPlotBarbCitySpawnCounter != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiPlotBarbCitySpawnCounter);
	}
	if (m_aiPlotBarbCityNumUnitsSpawned != NULL)
	{
		SAFE_DELETE_ARRAY(m_aiPlotBarbCityNumUnitsSpawned);
	}
#endif
}

//	---------------------------------------------------------------------------
/// Serialization Read
void CvBarbarians::Read(FDataStream& kStream, uint uiParentVersion)
{
	// Version number to maintain backwards compatibility
	uint uiVersion = 0;

	kStream >> uiVersion;	
	MOD_SERIALIZE_INIT_READ(kStream);

	int iWorldNumPlots = GC.getMap().numPlots();
	MapInit(iWorldNumPlots);	// Map will have been initialized/unserialized by now so this is ok.
	kStream >> ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampSpawnCounter);
	kStream >> ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampNumUnitsSpawned);
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	kStream >> ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCitySpawnCounter);
	kStream >> ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCityNumUnitsSpawned);
#endif
}

//	---------------------------------------------------------------------------
/// Serialization Write
void CvBarbarians::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	int iWorldNumPlots = GC.getMap().numPlots();
	kStream << ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampSpawnCounter);
	kStream << ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampNumUnitsSpawned);
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	kStream << ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCitySpawnCounter);
	kStream << ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCityNumUnitsSpawned);
#endif
}

//	--------------------------------------------------------------------------------
void CvBarbarians::DoCamps()
{
	CvGame& kGame = GC.getGame();

	if(kGame.isOption(GAMEOPTION_NO_BARBARIANS))
	{
		return;
	}

	int iNumNotVisiblePlots = 0;
	int iNumCampsInExistence = 0;

	ImprovementTypes eCamp = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();

	bool bAlwaysRevealedBarbCamp = false;

	// Is there an appropriate Improvement to place as a Barb Camp?
	if(eCamp != NO_IMPROVEMENT)
	{
		CvMap& kMap = GC.getMap();
		std::vector<CvPlot*> vCoastalPlots,vAllPlots;

		// Figure out how many Nonvisible tiles we have to base # of camps to spawn on
		for(int iI = 0; iI < kMap.numPlots(); iI++)
		{
			CvPlot* pLoopPlot = kMap.plotByIndexUnchecked(iI);

			// See how many camps we already have
			if(pLoopPlot->getImprovementType() == eCamp)
			{
				iNumCampsInExistence++;
			}

			//No water obviously
			if(pLoopPlot->isWater() || !pLoopPlot->isValidMovePlot(BARBARIAN_PLAYER))
				continue;

			//Discount all owned plots.
			if (pLoopPlot->isOwned() || pLoopPlot->isAdjacentOwned())
				continue;

			// No camps on 1-tile islands
			if(pLoopPlot->getArea()==-1 || kMap.getArea(pLoopPlot->getArea())->getNumTiles() == 1)
				continue;

			// No camps on resources or improvements
			if(/*pLoopPlot->getResourceType() != NO_RESOURCE || */ pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				continue;

			// Don't look at Tiles that can't have an improvement
			if(pLoopPlot->getFeatureType() != NO_FEATURE && GC.getFeatureInfo(pLoopPlot->getFeatureType())->isNoImprovement())
				continue;

			bool bMinors = MOD_BALANCE_CORE_BARBARIAN_THEFT;
			if (GC.getGame().getGameTurn() <= 5)
			{
				bMinors = false;
			}
			// No new camps on plots in sight of a player
			if (pLoopPlot->isVisibleToCivTeam(true, bMinors))
				continue;

			iNumNotVisiblePlots++;

			vAllPlots.push_back(pLoopPlot);

#if defined(MOD_CORE_REDUCE_RANDOMNESS)
			if (vAllPlots.size()>1)
			{
				//do one iteration of a fisher-yates shuffle
				int iSwap = kGame.isReallyNetworkMultiPlayer() ? vAllPlots.size() / 2 : kGame.getSmallFakeRandNum(vAllPlots.size(), pLoopPlot->GetPlotIndex()+vAllPlots.size());
				std::swap(vAllPlots[iSwap],vAllPlots.back());
			}
#endif

			if (pLoopPlot->isCoastalLand())
			{
				vCoastalPlots.push_back(pLoopPlot);

#if defined(MOD_CORE_REDUCE_RANDOMNESS)
				if (vCoastalPlots.size()>1)
				{
					//do one iteration of a fisher-yates shuffle
					int iSwap = kGame.isReallyNetworkMultiPlayer() ? vCoastalPlots.size() / 2 : kGame.getSmallFakeRandNum(vCoastalPlots.size(), pLoopPlot->GetPlotIndex()+vCoastalPlots.size());
					std::swap(vCoastalPlots[iSwap],vCoastalPlots.back());
				}
#endif
			}
		}

		int iNumValidCampPlots = iNumNotVisiblePlots;
		int iFogTilesPerBarbarianCamp = kMap.getWorldInfo().getFogTilesPerBarbarianCamp();
		int iCampTargetNum = (iFogTilesPerBarbarianCamp != 0)? iNumValidCampPlots / iFogTilesPerBarbarianCamp : 0;
		int iNumCampsToAdd = iCampTargetNum - iNumCampsInExistence;

		if(iNumCampsToAdd > 0 && GC.getBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING() > 0) // slewis - added the barbarian chance for the FoR scenario
		{
			// First turn of the game add 1/3 of the Target number of Camps
			if(kGame.getElapsedGameTurns() == 0)
			{
				iNumCampsToAdd *= /*33*/ GC.getBARBARIAN_CAMP_FIRST_TURN_PERCENT_OF_TARGET_TO_ADD();
				iNumCampsToAdd /= 100;
			}
			// Every other turn of the game there's a 1 in 2 chance of adding a new camp if we're still below the target
			else
			{
#if defined(MOD_CORE_REDUCE_RANDOMNESS)
				if (kGame.isReallyNetworkMultiPlayer() ? GC.getGame().getGameTurn() % 2 == 0 : kGame.getSmallFakeRandNum(GC.getBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING(), iNumValidCampPlots) == 0)
#else
				if(kGame.getJonRandNum(/*2*/ GC.getBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING(), "Random roll to see if Barb Camp spawns this turn") > 0)
#endif
				{
					iNumCampsToAdd = 1;
				}
				else
				{
					iNumCampsToAdd = 0;
				}
			}

			// Don't want to get stuck in an infinite or almost so loop
			int iCount = 0;
			int iNumPlots = kMap.numPlots();

			UnitTypes eBestUnit;

			int iNumLandPlots = kMap.getLandPlots();
			int iPlayerCapitalMinDistance = /*4*/ GC.getBARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL();
			int iBarbCampMinDistance = /*7*/ GC.getBARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP();
			int iMaxDistanceToLook = iPlayerCapitalMinDistance > iBarbCampMinDistance ? iPlayerCapitalMinDistance : iBarbCampMinDistance;
			int iPlayerLoop;
#if defined(MOD_BALANCE_CORE)
			ResourceTypes eCloseResource = NO_RESOURCE;
			ResourceClassTypes eResourceClassRush = (ResourceClassTypes)GC.getInfoTypeForString("RESOURCECLASS_RUSH");
#endif
			// Find Plots to put the Camps
			do
			{
				iCount++;

				// Do a random roll to bias in favor of Coastal land Tiles so that the Barbs will spawn Boats :) - required 1/6 of the time
#if defined(MOD_CORE_REDUCE_RANDOMNESS)
				// make sure we have suitable coastal plots!
				bool bWantsCoastal = false;
				if (!vCoastalPlots.empty() && vCoastalPlots.size() * 3 > vAllPlots.size())
					bWantsCoastal = true;

				// if we don't have any valid plots left at all, then bail
				if (vAllPlots.empty())
					break;

				std::vector<CvPlot*>& vRelevantPlots = bWantsCoastal ? vCoastalPlots : vAllPlots;

				if (vRelevantPlots.size() <= 0 || vRelevantPlots.empty())
					break;

				int iPlotIndex = kGame.isReallyNetworkMultiPlayer() ? vRelevantPlots.size() / 2 : kGame.getSmallFakeRandNum(vRelevantPlots.size(), vRelevantPlots.size());
#else
				bool bWantsCoastal = kGame.getJonRandNum(/*6*/ GC.getBARBARIAN_CAMP_COASTAL_SPAWN_ROLL(), "Barb Camp Plot-Finding Roll - Coastal Bias") == 0 ? true : false;
				int iPlotIndex = kGame.getJonRandNum( bWantsCoastal ? vCoastalPlots.size() : vAllPlots.size(), "Barb Camp Plot-Finding Roll");
#endif

				CvPlot* pLoopPlot = vRelevantPlots[iPlotIndex];

				//now do some additional (expensive) checks we shouldn't do above
#if defined(MOD_BUGFIX_BARB_CAMP_TERRAINS)
				CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eCamp);
				if(MOD_BUGFIX_BARB_CAMP_TERRAINS == false || pkImprovementInfo == NULL || 
					(pkImprovementInfo->GetTerrainMakesValid(pLoopPlot->getTerrainType()) && pkImprovementInfo->GetFeatureMakesValid(pLoopPlot->getFeatureType())))
				{
#endif
					// Max Camps for this area
					int iMaxCampsThisArea = iCampTargetNum * pLoopPlot->area()->getNumTiles() / iNumLandPlots + 1;

					// Already enough Camps in this Area?
					if (pLoopPlot->area()->getNumImprovements(eCamp) <= iMaxCampsThisArea)
					{
						bool bSomethingTooClose = false;

						// Look at nearby Plots to make sure another camp isn't too close
						for (int iDX = -(iMaxDistanceToLook); iDX <= iMaxDistanceToLook; iDX++)
						{
							for (int iDY = -(iMaxDistanceToLook); iDY <= iMaxDistanceToLook; iDY++)
							{
								CvPlot* pNearbyCampPlot = plotXY(pLoopPlot->getX(), pLoopPlot->getY(), iDX, iDY);

								if (pNearbyCampPlot != NULL)
								{
									int iPlotDistance = plotDistance(pNearbyCampPlot->getX(), pNearbyCampPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
#if defined(MOD_BALANCE_CORE)
									if (iPlotDistance <= 2)
									{
										if (pNearbyCampPlot->getResourceType(BARBARIAN_TEAM) != NO_RESOURCE && GC.getResourceInfo(pNearbyCampPlot->getResourceType())->getResourceUsage() == RESOURCEUSAGE_STRATEGIC && GC.getResourceInfo(pNearbyCampPlot->getResourceType())->getResourceClassType() == eResourceClassRush)
										{
											eCloseResource = pNearbyCampPlot->getResourceType();
										}
									}
#endif
									// Can't be too close to a player
									if (iPlotDistance <= iPlayerCapitalMinDistance)
									{
										if (pNearbyCampPlot->isCity())
										{
											if (pNearbyCampPlot->getPlotCity()->isCapital())
											{
												// Only care about Majors' capitals
												if (pNearbyCampPlot->getPlotCity()->getOwner() < MAX_MAJOR_CIVS)
												{
													bSomethingTooClose = true;
													break;
												}
											}
										}
									}

									// Can't be too close to another Camp
									if (iPlotDistance <= iBarbCampMinDistance)
									{
										if (pNearbyCampPlot->getImprovementType() == eCamp)
										{
											bSomethingTooClose = true;
											break;
										}
									}
								}
							}

							if (bSomethingTooClose)
							{
								break;
							}
						}

						// Found a camp or a city too close, check another one
						if (bSomethingTooClose || !CvBarbarians::IsPlotValidForBarbCamp(pLoopPlot))
						{
#if defined(MOD_CORE_REDUCE_RANDOMNESS)
							//if we're using the fake random generator, it will produce the same candidate every time, need to remove it from the pool
							vRelevantPlots.erase(vRelevantPlots.begin() + iPlotIndex);
#endif
							continue;
						}

						pLoopPlot->setImprovementType(eCamp);
						eBestUnit = GetRandomBarbarianUnitType(pLoopPlot, UNITAI_DEFENSE, eCloseResource);
						if (eBestUnit != NO_UNIT)
						{
							CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eBestUnit, pLoopPlot->getX(), pLoopPlot->getY(), GC.getUnitInfo(eBestUnit)->GetDefaultUnitAIType());
							if (pUnit)
								pUnit->finishMoves();
#if defined(MOD_EVENTS_BARBARIANS)
							if (MOD_EVENTS_BARBARIANS) {
								GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansSpawnedUnit, pLoopPlot->getX(), pLoopPlot->getY(), eBestUnit);
							}
#endif
						}

						// If we should update Camp visibility (for Policy), do so
						PlayerTypes ePlayer;
						TeamTypes eTeam;
						for (iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
						{
							ePlayer = (PlayerTypes)iPlayerLoop;
							eTeam = GET_PLAYER(ePlayer).getTeam();

							if (GET_PLAYER(ePlayer).IsAlwaysSeeBarbCamps())
							{
								if (pLoopPlot->isRevealed(eTeam))
								{
									pLoopPlot->setRevealedImprovementType(eTeam, eCamp);
									if (GC.getGame().getActivePlayer() == ePlayer)
										bAlwaysRevealedBarbCamp = true;
								}
							}
						}

						// Add another Unit adjacent to the Camp to stir up some trouble (JON: Disabled for now 09/12/09)
						if (!GC.getGame().isOption(GAMEOPTION_CHILL_BARBARIANS))
						{
							DoSpawnBarbarianUnit(pLoopPlot, true, true);
						}

						iNumCampsToAdd--;
					}
#if defined(MOD_BUGFIX_BARB_CAMP_TERRAINS)
				}
				else
				{
#if defined(MOD_CORE_REDUCE_RANDOMNESS)
					//if we're using the fake random generator, it will produce the same candidate every time, need to remove it from the pool
					vRelevantPlots.erase(vRelevantPlots.begin() + iPlotIndex);
#endif
				}
#endif
			}
			while(iNumCampsToAdd > 0 && iCount < iNumLandPlots);
		}
	}

	if(bAlwaysRevealedBarbCamp)
		GC.getMap().updateDeferredFog();
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
		if(pkUnitClassInfo == NULL)
			continue;

		const UnitTypes eLoopUnit = ((UnitTypes)(kBarbarianPlayer.getCivilizationInfo().getCivilizationUnits(iUnitClassLoop)));
		if(eLoopUnit != NO_UNIT)
		{
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eLoopUnit);
			if(pkUnitInfo == NULL)
				continue;

			CvUnitEntry& kUnit = *pkUnitInfo;
			if (pPlot->getDomain() != kUnit.GetDomainType())
				continue;

			if (!GET_PLAYER(BARBARIAN_PLAYER).canBarbariansTrain(eLoopUnit, false, eNearbyResource))
				continue;

			int iValue = 0;
			if (kUnit.GetRangedCombat() > 0)
			{
				iValue += kUnit.GetRangedCombat();
			}
			else if (kUnit.GetCombat() > 0)
			{
				iValue += kUnit.GetCombat();
			}
			else
				continue; //no civilians!

			// Resource Requirements
			for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
			{
				const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);
				CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
				if (pkResourceInfo)
				{
					const int iNumResource = GC.getUnitInfo(eLoopUnit)->GetResourceQuantityRequirement(eResource);

					if (iNumResource > 0)
					{
						iValue += 100;
						break;
					}

#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
					if (MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
					{
						int iNumResourceTotal = GC.getUnitInfo(eLoopUnit)->GetResourceQuantityTotal(eResource);

						if (iNumResourceTotal > 0)
						{
							iValue += 100;
							break;
						}
					}
#endif
				}
			}

			if(kUnit.GetUnitAIType(eUnitAI))
				iValue *= 2;

			if (iValue>0)
				candidates.push_back( OptionWithScore<UnitTypes>(eLoopUnit, iValue));
		}
	}

	//choose from top 3
	UnitTypes eBestUnit = PseudoRandomChoiceByWeight(candidates, NO_UNIT, 3, pPlot->GetPlotIndex());

#if defined(MOD_EVENTS_BARBARIANS)
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
#endif
	
	return eBestUnit;
}

//	--------------------------------------------------------------------------------
void CvBarbarians::DoUnits()
{
	CvGame& kGame = GC.getGame();

	if(!CanBarbariansSpawn())
	{
		return;
	}


	if (kGame.isOption(GAMEOPTION_NO_BARBARIANS))
	{
		return;
	}

	ImprovementTypes eCamp = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();

	CvMap& kMap = GC.getMap();
	for(int iPlotLoop = 0; iPlotLoop < kMap.numPlots(); iPlotLoop++)
	{
		CvPlot* pLoopPlot = kMap.plotByIndexUnchecked(iPlotLoop);

		// Found a Camp to spawn near
		if (pLoopPlot && pLoopPlot->getImprovementType() == eCamp)
		{
			if(ShouldSpawnBarbFromCamp(pLoopPlot))
			{
				DoSpawnBarbarianUnit(pLoopPlot, false, true);
				DoCampActivationNotice(pLoopPlot);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Unit spawned in camp. X: %d, Y: %d", pLoopPlot->getX(), pLoopPlot->getY());
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
	int iRange = GC.getMAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE();
	CvGame& kGame = GC.getGame();

	if (pPlot == 0)
		return;

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
					if (pUnit && pUnit->isBarbarian())
					{
						iNumNearbyUnits++;
					}
				}
			}
		}
	}

	// is this camp/city empty - first priority is to fill it
	if (pPlot && pPlot->GetNumCombatUnits() == 0 && (!pPlot->isCity() || (pPlot->isCity() && pPlot->isBarbarian())))
	{
		UnitTypes eUnit = GetRandomBarbarianUnitType(pPlot, UNITAI_RANGED, eCloseResource);
		if (eUnit != NO_UNIT)
		{
			CvUnitEntry* pkUnitDef = GC.getUnitInfo(eUnit);	
			CvUnit* pUnit = NULL;
			if(pkUnitDef)
			{
				pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY(), pkUnitDef->GetDefaultUnitAIType());
				if (pUnit)
					pUnit->finishMoves();
			}

			return;
		}
	}

	if(iNumNearbyUnits <= /*2*/ GC.getMAX_BARBARIANS_FROM_CAMP_NEARBY() || bIgnoreMaxBarbarians)
	{
		// Barbs only get boats after some period of time has passed
		bool bCanSpawnBoats = kGame.getElapsedGameTurns() > /*30*/ GC.getBARBARIAN_NAVAL_UNIT_START_TURN_SPAWN();

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
#if defined(MOD_CORE_REDUCE_RANDOMNESS)
			int iIndex = kGame.isReallyNetworkMultiPlayer() ? vBalidBarbSpawnPlots.size() / 2 : kGame.getSmallFakeRandNum(vBalidBarbSpawnPlots.size(), *pPlot);
#else
			int iIndex = kGame.getJonRandNum(vBalidBarbSpawnPlots.size(), "Barb Unit Location Spawn Roll");
#endif
			CvPlot* pSpawnPlot = vBalidBarbSpawnPlots[iIndex];
			UnitAITypes eUnitAI = pSpawnPlot->isWater() ? UNITAI_ATTACK_SEA : UNITAI_FAST_ATTACK;
			UnitTypes eUnit = GetRandomBarbarianUnitType(pSpawnPlot, eUnitAI, eCloseResource);
			if(eUnit != NO_UNIT)
			{
				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pSpawnPlot->getX(), pSpawnPlot->getY(), eUnitAI);
				if (bFinishMoves && pUnit)
				{
					pUnit->finishMoves();
				}

#if defined(MOD_BUGFIX_MINOR)
				// Stop units from plundered trade routes ending up in the ocean
				if(bIgnoreMaxBarbarians && pUnit)
				{
					if (!pUnit->jumpToNearestValidPlot())
						pUnit->kill(false);	// Could not find a valid spot!

#if defined(MOD_EVENTS_BARBARIANS)
					if (pUnit != NULL && MOD_EVENTS_BARBARIANS) {
						GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansSpawnedUnit, pSpawnPlot->getX(), pSpawnPlot->getY(), eUnit);
					}
#endif
				}
#endif
			}
		}
	}
}
