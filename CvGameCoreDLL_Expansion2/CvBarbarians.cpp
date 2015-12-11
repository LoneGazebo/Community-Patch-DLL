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
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
#include "CvMinorCivAI.h"
#endif
//static 
short* CvBarbarians::m_aiPlotBarbCampSpawnCounter = NULL;
short* CvBarbarians::m_aiPlotBarbCampNumUnitsSpawned = NULL;
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
short* CvBarbarians::m_aiPlotBarbCitySpawnCounter = NULL;
short* CvBarbarians::m_aiPlotBarbCityNumUnitsSpawned = NULL;
#endif

FStaticVector<DirectionTypes, 6, true, c_eCiv5GameplayDLL, 0> CvBarbarians::m_aeValidBarbSpawnDirections;

#if defined(MOD_BALANCE_CORE_MILITARY)
std::vector<CvPlot*> CvBarbarians::m_vPlotsWithCamp;
#endif


//	---------------------------------------------------------------------------
bool CvBarbarians::IsPlotValidForBarbCamp(CvPlot* pPlot)
{
	int iRange = 4;
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

#if defined(MOD_EVENTS_BARBARIANS)
	if (MOD_EVENTS_BARBARIANS) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansCampCleared, pPlot->getX(), pPlot->getY(), ePlayer);
	}
#endif
#if defined(MOD_BALANCE_CORE_MILITARY)
	std::vector<CvPlot*>::iterator it = std::find(m_vPlotsWithCamp.begin(),m_vPlotsWithCamp.end(),pPlot);
	if (it!=m_vPlotsWithCamp.end())
		m_vPlotsWithCamp.erase(it);
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
	CvGame& kGame = GC.getGame();
	// Default to between 8 and 12 turns per spawn
#if defined(MOD_BALANCE_CORE)
	int iNumTurnsToSpawn = 12 + kGame.getJonRandNum(10, "Barb Spawn Rand call");
#else
	int iNumTurnsToSpawn = 8 + kGame.getJonRandNum(5, "Barb Spawn Rand call");
#endif

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

	// Difficulty level can add time between spawns (e.g. Settler is +8 turns)
	CvHandicapInfo* pHandicapInfo = GC.getHandicapInfo(kGame.getHandicapType());
	if (pHandicapInfo)
		iNumTurnsToSpawn += pHandicapInfo->getBarbSpawnMod();

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
/// Gameplay informing us when a Camp has either been created or spawned a Unit so we can reseed the spawn counter
void CvBarbarians::DoCityActivationNotice(CvPlot* pPlot)
{
	CvGame& kGame = GC.getGame();
	// Default to between 8 and 12 turns per spawn
	//bumped a bit - too many barbs gets annoying.
	int iNumTurnsToSpawn = 15 + kGame.getJonRandNum(5, "Barb Spawn Rand call");

	// Raging
	if (kGame.isOption(GAMEOPTION_RAGING_BARBARIANS))
		iNumTurnsToSpawn /= 2;

#if defined(MOD_BUGFIX_BARB_CAMP_SPAWNING)
	if (m_aiPlotBarbCityNumUnitsSpawned == NULL) {
		// Probably means we are being called as CvWorldBuilderMapLoaded is adding camps, MapInit() will follow soon and set everything up correctly
		return;
	}
#endif
		
	// Num Units Spawned
	int iNumUnitsSpawned = m_aiPlotBarbCityNumUnitsSpawned[pPlot->GetPlotIndex()];

	// Reduce turns between spawn if we've pumped out more guys (meaning we're further into the game)
	iNumTurnsToSpawn -= min(3, iNumUnitsSpawned);	// -1 turns if we've spawned one Unit, -3 turns if we've spawned three

	// Increment # of barbs spawned from this camp
	m_aiPlotBarbCityNumUnitsSpawned[pPlot->GetPlotIndex()]++;	// This starts at -1 so when a camp is first created it will bump up to 0, which is correct

	//// If it's too early to spawn then add in a small amount to delay things a bit - between 3 and 6 extra turns
	//if (CanBarbariansSpawn())
	//{
	//	iNumTurnsToSpawn += 3;
	//	iNumTurnsToSpawn += auto_ptr<ICvGame1> pGame = GameCore::GetGame();\n.getJonRandNum(4, "Early game Barb Spawn Rand call");
	//}

	// Difficulty level can add time between spawns (e.g. Settler is +8 turns)
	CvHandicapInfo* pHandicapInfo = GC.getHandicapInfo(kGame.getHandicapType());
	if (pHandicapInfo)
		iNumTurnsToSpawn += pHandicapInfo->getBarbSpawnMod();

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
/// Gameplay informing a camp has been attacked - make it more likely to spawn
void CvBarbarians::DoCityAttacked(CvPlot* pPlot)
{
	int iCounter = m_aiPlotBarbCitySpawnCounter[pPlot->GetPlotIndex()];

	// Halve the amount of time to spawn
	int iNewValue = iCounter / 2;

	m_aiPlotBarbCitySpawnCounter[pPlot->GetPlotIndex()] = iNewValue;
}
#endif
//	--------------------------------------------------------------------------------
/// Gameplay informing a camp has been attacked - make it more likely to spawn
void CvBarbarians::DoCampAttacked(CvPlot* pPlot)
{
	int iCounter = m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()];

	// Halve the amount of time to spawn
	int iNewValue = iCounter / 2;

	m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] = iNewValue;
}

//	---------------------------------------------------------------------------
/// Called every turn
void CvBarbarians::BeginTurn()
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

#if defined(MOD_BALANCE_CORE_MILITARY)
	 m_vPlotsWithCamp.clear();
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
	CvPlot* pLoopPlot;

	ImprovementTypes eCamp = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();

	bool bAlwaysRevealedBarbCamp = false;

	// Is there an appropriate Improvement to place as a Barb Camp?
	if(eCamp != NO_IMPROVEMENT)
	{
		CvMap& kMap = GC.getMap();
		// Figure out how many Nonvisible tiles we have to base # of camps to spawn on
		for(int iI = 0; iI < kMap.numPlots(); iI++)
		{
			pLoopPlot = kMap.plotByIndexUnchecked(iI);

			// See how many camps we already have
			if(pLoopPlot->getImprovementType() == eCamp)
			{
				iNumCampsInExistence++;
			}

			if(!pLoopPlot->isWater())
			{
				if(!pLoopPlot->isVisibleToCivTeam())
				{
					iNumNotVisiblePlots++;
				}
			}
		}

		int iNumValidCampPlots;

		iNumValidCampPlots = iNumNotVisiblePlots;

		int iFogTilesPerBarbarianCamp = kMap.getWorldInfo().getFogTilesPerBarbarianCamp();
		int iCampTargetNum = (iFogTilesPerBarbarianCamp != 0)? iNumValidCampPlots / iFogTilesPerBarbarianCamp : 0;//getHandicapInfo().getFogTilesPerBarbarianCamp();
		int iNumCampsToAdd = iCampTargetNum - iNumCampsInExistence;

		int iMaxCampsThisArea;

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
				if(kGame.getJonRandNum(/*2*/ GC.getBARBARIAN_CAMP_ODDS_OF_NEW_CAMP_SPAWNING(), "Random roll to see if Barb Camp spawns this turn") > 0)
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
			int iPlotIndex = -1;
			int iNumPlots = kMap.numPlots();

			UnitTypes eBestUnit;

			int iNumLandPlots = kMap.getLandPlots();

			// Do a random roll to bias in favor of Coastal land Tiles so that the Barbs will spawn Boats :) - required 1/6 of the time
			bool bWantsCoastal = kGame.getJonRandNum(/*6*/ GC.getBARBARIAN_CAMP_COASTAL_SPAWN_ROLL(), "Barb Camp Plot-Finding Roll - Coastal Bias 1") == 0 ? true : false;

			int iPlayerCapitalMinDistance = /*4*/ GC.getBARBARIAN_CAMP_MINIMUM_DISTANCE_CAPITAL();
			int iBarbCampMinDistance = /*7*/ GC.getBARBARIAN_CAMP_MINIMUM_DISTANCE_ANOTHER_CAMP();
			int iMaxDistanceToLook = iPlayerCapitalMinDistance > iBarbCampMinDistance ? iPlayerCapitalMinDistance : iBarbCampMinDistance;
			int iPlotDistance;

			int iDX, iDY;
			CvPlot* pNearbyCampPlot;
			bool bSomethingTooClose;

			CvString strBuffer;

			int iPlayerLoop;

			// Find Plots to put the Camps
			do
			{
				iCount++;

				iPlotIndex = kGame.getJonRandNum(iNumPlots, "Barb Camp Plot-Finding Roll");

				pLoopPlot = kMap.plotByIndex(iPlotIndex);

				// Plot must be valid (not Water, nonvisible)
				if(!pLoopPlot->isWater())
				{
					if(pLoopPlot->isValidEndTurnPlot(BARBARIAN_PLAYER) && pLoopPlot->getArea()!=-1)
					{
#if defined(MOD_BUGFIX_BARB_CAMP_TERRAINS)
						CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eCamp);
						if(MOD_BUGFIX_BARB_CAMP_TERRAINS == false || pkImprovementInfo == NULL || (pkImprovementInfo->GetTerrainMakesValid(pLoopPlot->getTerrainType()) && pkImprovementInfo->GetFeatureMakesValid(pLoopPlot->getFeatureType()))) {
#endif
						if(!pLoopPlot->isOwned() && !pLoopPlot->isVisibleToCivTeam())
						{
							// JON: NO RESOURCES FOR NOW, MAY REPLACE WITH SOMETHING COOLER
							if(pLoopPlot->getResourceType() == NO_RESOURCE)
							{
								// No camps on 1-tile islands
								if(kMap.getArea(pLoopPlot->getArea())->getNumTiles() > 1)
								{
									if(pLoopPlot->isCoastalLand() || !bWantsCoastal)
									{
										// Max Camps for this area
										iMaxCampsThisArea = iCampTargetNum * pLoopPlot->area()->getNumTiles() / iNumLandPlots;
										// Add 1 just in case the above algorithm rounded something off
										iMaxCampsThisArea++;

										// Already enough Camps in this Area?
										if(pLoopPlot->area()->getNumImprovements(eCamp) <= iMaxCampsThisArea)
										{
											// Don't look at Tiles that already have a Camp
											if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
											{
												// Don't look at Tiles that can't have an improvement
												if(pLoopPlot->getFeatureType() == NO_FEATURE || !GC.getFeatureInfo(pLoopPlot->getFeatureType())->isNoImprovement())
												{
													bSomethingTooClose = false;

													// Look at nearby Plots to make sure another camp isn't too close
													for(iDX = -(iMaxDistanceToLook); iDX <= iMaxDistanceToLook; iDX++)
													{
														for(iDY = -(iMaxDistanceToLook); iDY <= iMaxDistanceToLook; iDY++)
														{
															pNearbyCampPlot = plotXY(pLoopPlot->getX(), pLoopPlot->getY(), iDX, iDY);

															if(pNearbyCampPlot != NULL)
															{
																iPlotDistance = plotDistance(pNearbyCampPlot->getX(), pNearbyCampPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());

																// Can't be too close to a player
																if(iPlotDistance <= iPlayerCapitalMinDistance)
																{
																	if(pNearbyCampPlot->isCity())
																	{
																		if(pNearbyCampPlot->getPlotCity()->isCapital())
																		{
																			// Only care about Majors' capitals
																			if(pNearbyCampPlot->getPlotCity()->getOwner() < MAX_MAJOR_CIVS)
																			{
																				bSomethingTooClose = true;
																				break;
																			}
																		}
																	}
																}

																// Can't be too close to another Camp
																if(iPlotDistance <= iBarbCampMinDistance)
																{
																	if(pNearbyCampPlot->getImprovementType() == eCamp)
																	{
																		bSomethingTooClose = true;
																		break;
																	}
																}
															}
														}
														if(bSomethingTooClose)
														{
															break;
														}
													}

													// Found a camp too close, check another Plot
													if(bSomethingTooClose)
														continue;

													// Last check
													if(!CvBarbarians::IsPlotValidForBarbCamp(pLoopPlot))
														continue;

													pLoopPlot->setImprovementType(eCamp);
#if !defined(MOD_BUGFIX_BARB_CAMP_SPAWNING)
													// The notification has been moved into the CvPlot::setImprovementType() method
													DoCampActivationNotice(pLoopPlot);
#endif

#if defined(MOD_EVENTS_BARBARIANS)
													eBestUnit = GetRandomBarbarianUnitType(pLoopPlot, UNITAI_DEFENSE);
#else
													eBestUnit = GetRandomBarbarianUnitType(kMap.getArea(pLoopPlot->getArea()), UNITAI_DEFENSE);
#endif

													if(eBestUnit != NO_UNIT)
													{
														GET_PLAYER(BARBARIAN_PLAYER).initUnit(eBestUnit, pLoopPlot->getX(), pLoopPlot->getY(), GC.getUnitInfo(eBestUnit)->GetDefaultUnitAIType());
#if defined(MOD_EVENTS_BARBARIANS)
														if (MOD_EVENTS_BARBARIANS) {
															GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansSpawnedUnit, pLoopPlot->getX(), pLoopPlot->getY(), eBestUnit);
														}
#endif
													}

													// If we should update Camp visibility (for Policy), do so
													PlayerTypes ePlayer;
													TeamTypes eTeam;
													for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
													{
														ePlayer = (PlayerTypes) iPlayerLoop;
														eTeam = GET_PLAYER(ePlayer).getTeam();

														if(GET_PLAYER(ePlayer).IsAlwaysSeeBarbCamps())
														{
															if(pLoopPlot->isRevealed(eTeam))
															{
																pLoopPlot->setRevealedImprovementType(eTeam, eCamp);
																if(GC.getGame().getActivePlayer() == ePlayer)
																	bAlwaysRevealedBarbCamp = true;
															}
														}
													}

													// Add another Unit adjacent to the Camp to stir up some trouble (JON: Disabled for now 09/12/09)
													//doSpawnBarbarianUnit(pLoopPlot);

													iNumCampsToAdd--;

													// Seed the next Camp for Coast or not
													bWantsCoastal = kGame.getJonRandNum(/*5*/ GC.getBARBARIAN_CAMP_COASTAL_SPAWN_ROLL(), "Barb Camp Plot-Finding Roll - Coastal Bias 2") == 0 ? true : false;
												}
											}
										}
									}
								}
							}
						}
#if defined(MOD_BUGFIX_BARB_CAMP_TERRAINS)
						}
#endif
					}
				}
			}
			while(iNumCampsToAdd > 0 && iCount < iNumLandPlots);
		}
	}

#if defined(MOD_BALANCE_CORE_MILITARY)
	 m_vPlotsWithCamp.clear();

	// Is there an appropriate Improvement to place as a Barb Camp?
	if(eCamp != NO_IMPROVEMENT)
	{
		CvMap& kMap = GC.getMap();
		// Figure out how many Nonvisible tiles we have to base # of camps to spawn on
		for(int iI = 0; iI < kMap.numPlots(); iI++)
		{
			pLoopPlot = kMap.plotByIndexUnchecked(iI);

			// See how many camps we already have
			if(pLoopPlot->getImprovementType() == eCamp)
			{
				m_vPlotsWithCamp.push_back(pLoopPlot);
			}
		}
	}
#endif


	if(bAlwaysRevealedBarbCamp)
		GC.getMap().updateDeferredFog();
}

//	--------------------------------------------------------------------------------
#if defined(MOD_EVENTS_BARBARIANS)
UnitTypes CvBarbarians::GetRandomBarbarianUnitType(CvPlot* pPlot, UnitAITypes eUnitAI)
{
	CvArea* pArea = GC.getMap().getArea(pPlot->getArea());
#else
UnitTypes CvBarbarians::GetRandomBarbarianUnitType(CvArea* pArea, UnitAITypes eUnitAI)
{
#endif
	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;
	int iValue = 0;

	CvPlayerAI& kBarbarianPlayer = GET_PLAYER(BARBARIAN_PLAYER);

	CvGame &kGame = GC.getGame();

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
			{
				continue;
			}

			CvUnitEntry& kUnit = *pkUnitInfo;

			bValid = (kUnit.GetCombat() > 0);
			if(bValid)
			{
				// Unit has combat strength, make sure it isn't only defensive (and with no ranged combat ability)
				if(kUnit.GetRange() == 0)
				{
					for(int iLoop = 0; iLoop < GC.getNumPromotionInfos(); iLoop++)
					{
						const PromotionTypes ePromotion = static_cast<PromotionTypes>(iLoop);
						CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(ePromotion);
						if(pkPromotionInfo)
						{
							if(kUnit.GetFreePromotions(iLoop))
							{
								if(pkPromotionInfo->IsOnlyDefensive())
								{
									bValid = false;
									break;
								}
							}
						}
					}
				}
			}

			if(bValid)
			{
				if(pArea->isWater() && kUnit.GetDomainType() != DOMAIN_SEA)
				{
					bValid = false;
				}
				else if(!pArea->isWater() && kUnit.GetDomainType() != DOMAIN_LAND)
				{
					bValid = false;
				}
			}

			if(bValid)
			{
				if(!GET_PLAYER(BARBARIAN_PLAYER).canTrain(eLoopUnit))
				{
					bValid = false;
				}
			}

			if(bValid)
			{
				iValue = (1 + kGame.getJonRandNum(1000, "Barb Unit Selection"));

				if(kUnit.GetUnitAIType(eUnitAI))
				{
					iValue += 200;
				}

				if(iValue > iBestValue)
				{
					eBestUnit = eLoopUnit;
					iBestValue = iValue;
				}
			}
		}
	}

#if defined(MOD_EVENTS_BARBARIANS)
	if (MOD_EVENTS_BARBARIANS) {
		int iValue = 0;
		if (GAMEEVENTINVOKE_VALUE(iValue, GAMEEVENT_BarbariansCampGetSpawnUnit, pPlot->getX(), pPlot->getY(), eBestUnit) == GAMEEVENTRETURN_VALUE) {
			// Defend against modder stupidity!
			UnitTypes eUnitType = (UnitTypes)iValue;
			if (eUnitType != NO_UNIT && GC.getUnitInfo(eUnitType) != NULL) {
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

	if(kGame.isOption(GAMEOPTION_NO_BARBARIANS))
	{
		return;
	}

	if(!CanBarbariansSpawn())
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
				DoSpawnBarbarianUnit(pLoopPlot, false, false);
				DoCampActivationNotice(pLoopPlot);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Unit spawned in camp. X: %d, Y: %d", pLoopPlot->getX(), pLoopPlot->getY());
					if(GET_PLAYER(BARBARIAN_PLAYER).GetID() != NO_PLAYER)
					{
						GET_PLAYER(BARBARIAN_PLAYER).GetTacticalAI()->LogTacticalMessage(strLogString);
					}
				}
			}
		}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
		// Found a city to spawn near
		if (MOD_DIPLOMACY_CITYSTATES_QUESTS && pLoopPlot && pLoopPlot->isCity() && pLoopPlot->isBarbarian())
		{
			if(ShouldSpawnBarbFromCity(pLoopPlot))
			{
				DoSpawnBarbarianUnit(pLoopPlot, false, false);
				DoCityActivationNotice(pLoopPlot);
				
				//Grow and strengthen, my precious!
				CvCity* pCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY());
				if(pCity != NULL)
				{ 
					pCity->changePopulation(1);
					pCity->setFood((pCity->growthThreshold() - 1));				}
				if(pCity != NULL && GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Unit spawned in barbarian city of %s. City grows by 1. X: %d, Y: %d", pCity->getName().c_str(), pLoopPlot->getX(), pLoopPlot->getY());
					if(GET_PLAYER(BARBARIAN_PLAYER).GetID() != NO_PLAYER)
					{
						GET_PLAYER(BARBARIAN_PLAYER).GetTacticalAI()->LogTacticalMessage(strLogString);
					}
				}
			}
		}
#endif

#if defined(MOD_BALANCE_CORE_BARBARIAN_THEFT)
		//Let's make Barbs scarier. If they end their move next to a city, let's have them pillage some Gold from the City. If they end their turn in owned land, let's give them a smaller chance to do so.
		if(pLoopPlot != NULL && MOD_BALANCE_CORE_BARBARIAN_THEFT)
		{
			CvUnit* pUnit = pLoopPlot->getUnitByIndex(0);
			int iBarbStrength = 0;
			int iCityStrength = 0;
			if(pUnit != NULL && pUnit->isBarbarian() && pUnit->IsCombatUnit() && pUnit->plot()->GetAdjacentCity() != NULL)
			{
				CvCity* pCity = pUnit->plot()->GetAdjacentCity();
				if(pCity && pCity->getOwner() != NO_PLAYER && !GET_PLAYER(pCity->getOwner()).isMinorCiv() && !GET_PLAYER(pCity->getOwner()).isBarbarian())
				{
					iCityStrength = pCity->getStrengthValue(false);
#ifdef AUI_BINOM_RNG
					iCityStrength += GC.getGame().getJonRandNumBinom(iCityStrength, "Barbarian Random Strength Bump");
#else
					iCityStrength += GC.getGame().getJonRandNum(iCityStrength, "Barbarian Random Strength Bump");
#endif
					iCityStrength /= 100;
					iBarbStrength = (pUnit->GetBaseCombatStrength(true) * 10);
					iBarbStrength += GC.getGame().getJonRandNum(iBarbStrength, "Barbarian Random Strength Bump");
#ifdef AUI_BINOM_RNG
					iBarbStrength +=GC.getGame().getJonRandNumBinom(iBarbStrength, "Barbarian Random Strength Bump");
#else
					iBarbStrength += GC.getGame().getJonRandNum(iBarbStrength, "Barbarian Random Strength Bump");
#endif
					if(iBarbStrength > iCityStrength)
					{
						int iTheft = (iBarbStrength - iCityStrength);

						if(iTheft > 0)
						{
							pCity->changeDamage((iTheft / 4));
							pUnit->changeDamage((iTheft / 4));
#ifdef AUI_BINOM_RNG
							int iYield = GC.getGame().getJonRandNumBinom(10, "Barbarian Theft Value");
#else
							int iYield = GC.getGame().getJonRandNum(10, "Barbarian Theft Value");
#endif			
							if(iYield <= 2)
							{
								int iGold = ((pCity->getBaseYieldRate(YIELD_GOLD) * iTheft) / 100);
								if(iGold > 0)
								{
									GET_PLAYER(pCity->getOwner()).GetTreasury()->ChangeGold(-iGold);

									Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT_CITY_DETAILED");
									strMessage << iGold;
									strMessage << pCity->getNameKey();
									Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_GOLD_THEFT_CITY");
									strSummary << pCity->getNameKey();

									CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
									if(pNotification)
									{
										pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
									}
								}
							}
							else if(iYield <= 4)
							{
								int iCulture = ((pCity->getJONSCulturePerTurn() * iTheft) / 100);
								if(iCulture > 0)
								{
									GET_PLAYER(pCity->getOwner()).changeJONSCulture(-iCulture);

									Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_CULTURE_THEFT_CITY_DETAILED");
									strMessage << iCulture;
									strMessage << pCity->getNameKey();
									Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_CULTURE_THEFT_CITY");
									strSummary << pCity->getNameKey();

									CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
									if(pNotification)
									{
										pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
									}
								}
							}
							else if(iYield <= 6)
							{
								TechTypes eCurrentTech = GET_PLAYER(pCity->getOwner()).GetPlayerTechs()->GetCurrentResearch();
								int iScience = 0;
								if(eCurrentTech != NO_TECH)
								{
									iScience = ((pCity->getBaseYieldRate(YIELD_SCIENCE) * iTheft) / 100);
									if(iScience > 0)
									{
										GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, -iScience, pCity->getOwner());

										Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_SCIENCE_THEFT_CITY_DETAILED");
										strMessage << iScience;
										strMessage << pCity->getNameKey();
										Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_SCIENCE_THEFT_CITY");
										strSummary << pCity->getNameKey();

										CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
										if(pNotification)
										{
											pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
										}
									}
								}
							}
							else if(iYield <= 8)
							{
								int iFood = ((pCity->getBaseYieldRate(YIELD_FOOD) * iTheft) / 100);
								if(iFood > 0)
								{
									if(iFood > pCity->getFood())
									{
										pCity->setFood(0);
									}
									else
									{
										pCity->changeFood(-iFood);
									}

									Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_FOOD_THEFT_CITY_DETAILED");
									strMessage << iFood;
									strMessage << pCity->getNameKey();
									Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_FOOD_THEFT_CITY");
									strSummary << pCity->getNameKey();

									CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
									if(pNotification)
									{
										pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
									}
								}
							}
							else
							{
								if((pCity->getProduction() > 0) && (pCity->getProductionTurnsLeft() >= 2) && (pCity->getProductionTurnsLeft() != INT_MAX))
								{
									int iProduction = ((pCity->getBaseYieldRate(YIELD_PRODUCTION) * iTheft) / 100);
									if(iProduction > 0)
									{
										if(iProduction > pCity->getProduction())
										{
											pCity->setProduction(0);
										}
										else
										{
											pCity->changeProduction(-iProduction);
										}

										Localization::String strMessage = Localization::Lookup("TXT_KEY_BARBARIAN_PRODUCTION_THEFT_CITY_DETAILED");
										strMessage << iProduction;
										strMessage << pCity->getNameKey();
										Localization::String strSummary = Localization::Lookup("TXT_KEY_BARBARIAN_PRODUCTION_THEFT_CITY");
										strSummary << pCity->getNameKey();

										CvNotifications* pNotification = GET_PLAYER(pCity->getOwner()).GetNotifications();
										if(pNotification)
										{
											pNotification->Add(NOTIFICATION_GENERIC, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
										}
									}
								}
							}
						}
					}
				}
			}
		}
#endif
	}
#if defined(MOD_BALANCE_CORE)
	int iOtherMinorLoop;
	PlayerTypes eOtherMinor;
	TeamTypes eLoopTeam;
	for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
	{
		eLoopTeam = (TeamTypes) iTeamLoop;

		// Another Minor
		if(!GET_TEAM(eLoopTeam).isMinorCiv())
			continue;

		// They not alive!
		if(!GET_TEAM(eLoopTeam).isAlive())
			continue;

		for(iOtherMinorLoop = 0; iOtherMinorLoop < MAX_CIV_TEAMS; iOtherMinorLoop++)
		{
			eOtherMinor = (PlayerTypes) iOtherMinorLoop;

			if(eOtherMinor == NO_PLAYER)
				continue;

			// Other minor is on this team
			if(GET_PLAYER(eOtherMinor).getTeam() == eLoopTeam)
			{
				if(GET_PLAYER(eOtherMinor).GetMinorCivAI()->GetTurnsSinceRebellion() > 0)
				{
					int iRebellionSpawn = GET_PLAYER(eOtherMinor).GetMinorCivAI()->GetTurnsSinceRebellion();

					GET_PLAYER(eOtherMinor).GetMinorCivAI()->ChangeTurnsSinceRebellion(-1);

					//Rebel Spawn - once every 4 turns (on default speed)
					if ((iRebellionSpawn == /*20*/(GC.getMINOR_QUEST_REBELLION_TIMER() * 100) / 100)
					|| (iRebellionSpawn == /*16*/(GC.getMINOR_QUEST_REBELLION_TIMER() * 80) / 100)
					|| (iRebellionSpawn == /*12*/(GC.getMINOR_QUEST_REBELLION_TIMER() * 60) / 100)
					|| (iRebellionSpawn == /*8*/ (GC.getMINOR_QUEST_REBELLION_TIMER() * 40) / 100)
					|| (iRebellionSpawn == /*4*/ (GC.getMINOR_QUEST_REBELLION_TIMER() * 20) / 100))
					{
						GET_PLAYER(eOtherMinor).GetMinorCivAI()->DoRebellion();
					}
				}
			}
		}
	}
#endif
}

//	--------------------------------------------------------------------------------
/// Spawn a Barbarian Unit somewhere adjacent to pPlot
void CvBarbarians::DoSpawnBarbarianUnit(CvPlot* pPlot, bool bIgnoreMaxBarbarians, bool bFinishMoves)
{
	int iNumNearbyUnits;
	int iNearbyUnitLoop;
	int iRange = GC.getMAX_BARBARIANS_FROM_CAMP_NEARBY_RANGE();
	int iX;
	int iY;
	CvPlot* pNearbyPlot;
	DirectionTypes eDirection;

	CvGame& kGame = GC.getGame();

	if (pPlot == 0)
		return;

	// is this camp empty - first priority is to fill it
	if (pPlot && pPlot->GetNumCombatUnits() == 0)
	{
		UnitTypes eUnit;
#if defined(MOD_BALANCE_CORE_MILITARY)
		ImprovementTypes eCamp = (ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT();
		if(pPlot->getImprovementType() == eCamp)
		{
#if defined(MOD_EVENTS_BARBARIANS)
			eUnit = GetRandomBarbarianUnitType(pPlot, UNITAI_FAST_ATTACK);
#else
			eUnit = GetRandomBarbarianUnitType(GC.getMap().getArea(pPlot->getArea()), UNITAI_RANGED);
#endif
		}
		else
		{
#endif
#if defined(MOD_EVENTS_BARBARIANS)
			eUnit = GetRandomBarbarianUnitType(pPlot, UNITAI_FAST_ATTACK);
#else
			eUnit = GetRandomBarbarianUnitType(GC.getMap().getArea(pPlot->getArea()), UNITAI_FAST_ATTACK);
#endif
#if defined(MOD_BALANCE_CORE_MILITARY)
		}
#endif

		if (eUnit != NO_UNIT)
		{
#if defined(MOD_BALANCE_CORE_MILITARY)
			CvUnitEntry* pkUnitDef = GC.getUnitInfo(eUnit);	
			CvUnit* pUnit = NULL;
			if(pkUnitDef)
			{
				pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY(), pkUnitDef->GetDefaultUnitAIType());
				pUnit->finishMoves();
			}
#else
			CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY(), UNITAI_FAST_ATTACK);
			pUnit->finishMoves();
			
#if defined(MOD_EVENTS_BARBARIANS)
			if (MOD_EVENTS_BARBARIANS) {
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_BarbariansSpawnedUnit, pPlot->getX(), pPlot->getY(), eUnit);
			}
#endif
#endif
			return;
		}
	}

	m_aeValidBarbSpawnDirections.clear();

	// Look at nearby Plots to see if there are already too many Barbs nearby
	iNumNearbyUnits = 0;

	for(iX = -iRange; iX <= iRange; iX++)
	{
		for(iY = -iRange; iY <= iRange; iY++)
		{
			// Cut off the corners of the area we're looking at that we don't want
			pNearbyPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iX, iY, iRange);

			if(pNearbyPlot != NULL)
			{
				if(pNearbyPlot->getNumUnits() > 0)
				{
					for(iNearbyUnitLoop = 0; iNearbyUnitLoop < pNearbyPlot->getNumUnits(); iNearbyUnitLoop++)
					{
						const CvUnit* const unit = pNearbyPlot->getUnitByIndex(iNearbyUnitLoop);
						if(unit && pNearbyPlot->getUnitByIndex(iNearbyUnitLoop)->isBarbarian())
						{
							iNumNearbyUnits++;
						}
					}
				}
			}
		}
	}

	if(iNumNearbyUnits <= /*2*/ GC.getMAX_BARBARIANS_FROM_CAMP_NEARBY() || bIgnoreMaxBarbarians)
	{
		CvPlot* pLoopPlot;

		// Barbs only get boats after some period of time has passed
		bool bCanSpawnBoats = kGame.getElapsedGameTurns() > /*30*/ GC.getBARBARIAN_NAVAL_UNIT_START_TURN_SPAWN();

		// Look to see if adjacent Tiles are valid locations to spawn a Unit
		for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
		{
			eDirection = (DirectionTypes) iDirectionLoop;
			pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), eDirection);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getNumUnits() == 0)
				{
					if(pLoopPlot->isValidEndTurnPlot(BARBARIAN_PLAYER))
					{
						if(!pLoopPlot->isCity())
						{
							if(!pLoopPlot->isLake())
							{
								// Water Tiles are only valid when the Barbs have the proper Tech
								if(!pLoopPlot->isWater() || bCanSpawnBoats)
								{
									m_aeValidBarbSpawnDirections.push_back(eDirection);
								}
							}
						}
					}
				}
			}
		}

		// Any valid locations?
		if(m_aeValidBarbSpawnDirections.size() > 0)
		{
			int iIndex = kGame.getJonRandNum(m_aeValidBarbSpawnDirections.size(), "Barb Unit Location Spawn Roll");
			eDirection = (DirectionTypes) m_aeValidBarbSpawnDirections[iIndex];
			CvPlot* pSpawnPlot = plotDirection(pPlot->getX(), pPlot->getY(), eDirection);
			UnitAITypes eUnitAI;
			UnitTypes eUnit;

			// Naval Barbs
			if(pSpawnPlot->isWater())
			{
				eUnitAI = UNITAI_ATTACK_SEA;
			}
			// Land Barbs
			else
			{
				eUnitAI = UNITAI_FAST_ATTACK;
			}

#if defined(MOD_EVENTS_BARBARIANS)
			eUnit = GetRandomBarbarianUnitType(pSpawnPlot, eUnitAI);
#else
			eUnit = GetRandomBarbarianUnitType(GC.getMap().getArea(pSpawnPlot->getArea()), eUnitAI);
#endif

			if(eUnit != NO_UNIT)
			{
				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pSpawnPlot->getX(), pSpawnPlot->getY(), eUnitAI);
				if (bFinishMoves)
				{
					pUnit->finishMoves();
				}

#if defined(MOD_BUGFIX_MINOR)
				// Stop units from plundered trade routes ending up in the ocean
				if(bIgnoreMaxBarbarians)
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
