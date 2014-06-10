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

//static 
short* CvBarbarians::m_aiPlotBarbCampSpawnCounter = NULL;
short* CvBarbarians::m_aiPlotBarbCampNumUnitsSpawned = NULL;
FStaticVector<DirectionTypes, 6, true, c_eCiv5GameplayDLL, 0> CvBarbarians::m_aeValidBarbSpawnDirections;

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

	return true;
}

//	--------------------------------------------------------------------------------
/// Camp cleared, so reset counter
void CvBarbarians::DoBarbCampCleared(CvPlot* pPlot)
{
	m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] = -16;
}

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
	if (m_aiPlotBarbCampSpawnCounter[pPlot->GetPlotIndex()] == 0)
	{
		return true;
	}

	return false;
}

//	--------------------------------------------------------------------------------
/// Gameplay informing us when a Camp has either been created or spawned a Unit so we can reseed the spawn counter
void CvBarbarians::DoCampActivationNotice(CvPlot* pPlot)
{
	CvGame& kGame = GC.getGame();
	// Default to between 8 and 12 turns per spawn
	int iNumTurnsToSpawn = 8 + kGame.getJonRandNum(5, "Barb Spawn Rand call");

	// Raging
	if (kGame.isOption(GAMEOPTION_RAGING_BARBARIANS))
		iNumTurnsToSpawn /= 2;

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

		// Default values
		for (iI = 0; iI < iWorldNumPlots; ++iI)
		{
			m_aiPlotBarbCampSpawnCounter[iI] = -1;
			m_aiPlotBarbCampNumUnitsSpawned[iI] = -1;
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
}

//	---------------------------------------------------------------------------
/// Serialization Read
void CvBarbarians::Read(FDataStream& kStream, uint uiParentVersion)
{
	// Version number to maintain backwards compatibility
	uint uiVersion = 0;

	if (uiParentVersion >= 2)
		kStream >> uiVersion;	

	int iWorldNumPlots = GC.getMap().numPlots();
	MapInit(iWorldNumPlots);	// Map will have been initialized/unserialized by now so this is ok.

	kStream >> ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampSpawnCounter);
	kStream >> ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampNumUnitsSpawned);
}

//	---------------------------------------------------------------------------
/// Serialization Write
void CvBarbarians::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	int iWorldNumPlots = GC.getMap().numPlots();
	kStream << ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampSpawnCounter);
	kStream << ArrayWrapper<short>(iWorldNumPlots, m_aiPlotBarbCampNumUnitsSpawned);
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
					if(!pLoopPlot->isImpassable() && !pLoopPlot->isMountain())
					{
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
													DoCampActivationNotice(pLoopPlot);

													eBestUnit = GetRandomBarbarianUnitType(kMap.getArea(pLoopPlot->getArea()), UNITAI_DEFENSE);

													if(eBestUnit != NO_UNIT)
													{
														GET_PLAYER(BARBARIAN_PLAYER).initUnit(eBestUnit, pLoopPlot->getX(), pLoopPlot->getY(), (UnitAITypes) GC.getUnitInfo(eBestUnit)->GetDefaultUnitAIType());
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
					}
				}
			}
			while(iNumCampsToAdd > 0 && iCount < iNumLandPlots);
		}
	}

	if(bAlwaysRevealedBarbCamp)
		GC.getMap().updateDeferredFog();
}

//	--------------------------------------------------------------------------------
UnitTypes CvBarbarians::GetRandomBarbarianUnitType(CvArea* pArea, UnitAITypes eUnitAI)
{
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
				bool bFound = false;
				bool bRequires = false;
				for(int i = 0; i < GC.getNUM_UNIT_AND_TECH_PREREQS(); ++i)
				{
					TechTypes eTech = (TechTypes) kUnit.GetPrereqAndTechs(i);

					if(NO_TECH != eTech)
					{
						bRequires = true;

						if(GET_TEAM(BARBARIAN_TEAM).GetTeamTechs()->HasTech(eTech))
						{
							bFound = true;
							break;
						}
					}
				}

				if(bRequires && !bFound)
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
		if(pLoopPlot->getImprovementType() == eCamp)
		{
			if(ShouldSpawnBarbFromCamp(pLoopPlot))
			{
				DoSpawnBarbarianUnit(pLoopPlot);

				DoCampActivationNotice(pLoopPlot);
			}
		}
	}
}

//	--------------------------------------------------------------------------------
/// Spawn a Barbarian Unit somewhere adjacent to pPlot
void CvBarbarians::DoSpawnBarbarianUnit(CvPlot* pPlot)
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
		eUnit = GetRandomBarbarianUnitType(GC.getMap().getArea(pPlot->getArea()), UNITAI_FAST_ATTACK);

		if (eUnit != NO_UNIT)
		{
			CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pPlot->getX(), pPlot->getY(), UNITAI_FAST_ATTACK);
			pUnit->finishMoves();
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

	if(iNumNearbyUnits <= /*2*/ GC.getMAX_BARBARIANS_FROM_CAMP_NEARBY())
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
					if(!pLoopPlot->isImpassable() && !pLoopPlot->isMountain())
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

			eUnit = GetRandomBarbarianUnitType(GC.getMap().getArea(pSpawnPlot->getArea()), eUnitAI);

			if(eUnit != NO_UNIT)
			{
				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pSpawnPlot->getX(), pSpawnPlot->getY(), eUnitAI);
			}
		}
	}
}
