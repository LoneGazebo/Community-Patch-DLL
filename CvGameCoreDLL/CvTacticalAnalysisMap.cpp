/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvTacticalAnalysisMap.h"
#include "CvMilitaryAI.h"
#include "cvStopWatch.h"
#include "CvDiplomacyAI.h"

#include "LintFree.h"

//=====================================
// CvTacticalAnalysisCell
//=====================================
/// Constructor
CvTacticalAnalysisCell::CvTacticalAnalysisCell(void):
m_pEnemyMilitary(NULL),
m_pEnemyCivilian(NULL),
m_pNeutralMilitary(NULL),
m_pNeutralCivilian(NULL),
m_pFriendlyMilitary(NULL),
m_pFriendlyCivilian(NULL),
m_iDefenseModifier(0),
m_iDeploymentScore(0),
m_eTargetType(AI_TACTICAL_TARGET_NONE),
m_iDominanceZoneID(-1)
{
	Clear();
}

/// Reinitialize data
void CvTacticalAnalysisCell::Clear()
{
	ClearFlags();
	m_pEnemyMilitary = NULL;
	m_pEnemyCivilian = NULL;
	m_pNeutralMilitary = NULL;
	m_pNeutralCivilian = NULL;
	m_pFriendlyMilitary = NULL;
	m_pFriendlyCivilian = NULL;
	m_iDefenseModifier = 0;
	m_iDeploymentScore = 0;
	m_eTargetType = AI_TACTICAL_TARGET_NONE;
	m_iDominanceZoneID = -1;
}

bool CvTacticalAnalysisCell::CanUseForOperationGathering()
{
	if (IsImpassableTerrain() || IsImpassableTerritory() || GetEnemyMilitaryUnit() || GetNeutralMilitaryUnit() || GetNeutralCivilianUnit() || IsFriendlyTurnEndTile() || IsEnemyCity() || IsNeutralCity())
	{
		return false;
	}

	return true;
}

bool CvTacticalAnalysisCell::CanUseForOperationGatheringCheckWater(bool bWater)
{
	if (bWater != IsWater() || IsImpassableTerrain() || IsImpassableTerritory() || GetEnemyMilitaryUnit() || GetNeutralMilitaryUnit() || GetNeutralCivilianUnit() || IsFriendlyTurnEndTile() || IsEnemyCity() || IsNeutralCity())
	{
		return false;
	}

	return true;
}

//=====================================
// CvTacticalDominanceZone
//=====================================
/// Constructor
CvTacticalDominanceZone::CvTacticalDominanceZone(void)
{
	m_iDominanceZoneID = -1;
	m_eTerritoryType = TACTICAL_TERRITORY_NONE;
	m_eDominanceFlag = TACTICAL_DOMINANCE_NO_UNITS_PRESENT;
	m_eOwner = NO_PLAYER;
	m_iCityID = -1;
	m_iAreaID = 0;
	m_iFriendlyStrength = 0;
	m_iEnemyStrength = 0;
	m_iFriendlyRangedStrength = 0;
	m_iEnemyRangedStrength = 0;
	m_iFriendlyUnitCount = 0;
	m_iEnemyUnitCount = 0;
	m_iFriendlyRangedUnitCount = 0;
	m_iEnemyRangedUnitCount = 0;
	m_iZoneValue = 0;
	m_iRangeClosestEnemyUnit = MAX_INT;
	m_bIsWater = false;
	m_bIsNavalInvasion = false;
	m_pTempZoneCenter = NULL;
}

/// Retrieve city controlling this zone
CvCity *CvTacticalDominanceZone::GetClosestCity() const
{
	if (m_eOwner != NO_PLAYER)
	{
		return GET_PLAYER(m_eOwner).getCity(m_iCityID);
	}

	return NULL;
}

/// Set city controlling this zone
void CvTacticalDominanceZone::SetClosestCity(CvCity *pCity)
{
	if (pCity != NULL)
	{
		m_iCityID = pCity->GetID();
	}
	else
	{
		m_iCityID = -1;
	}
}

/// Retrieve distance in hexes of closest enemy to center of this zone
int CvTacticalDominanceZone::GetRangeClosestEnemyUnit() const
{
	return m_iRangeClosestEnemyUnit;
}

/// Set distance in hexes of closest enemy to center of this zone
void CvTacticalDominanceZone::SetRangeClosestEnemyUnit(int iRange)
{
	m_iRangeClosestEnemyUnit = iRange;
}

/// Mix ownership of zone and who is dominant to get a unique classification for the zone
TacticalMoveZoneType CvTacticalDominanceZone::GetZoneType() const
{
	if (m_eTerritoryType == TACTICAL_TERRITORY_FRIENDLY)
	{
		if (m_eDominanceFlag == TACTICAL_DOMINANCE_FRIENDLY)
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_WINNING;
		}
		else if (m_eDominanceFlag == TACTICAL_DOMINANCE_EVEN)
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_EVEN;
		}
		else
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_LOSING;
		}
	}

	else if (m_eTerritoryType == TACTICAL_TERRITORY_ENEMY)
	{
		if (m_eDominanceFlag == TACTICAL_DOMINANCE_FRIENDLY)
		{
			return AI_TACTICAL_MOVE_ZONE_ENEMY_WINNING;
		}
		else if (m_eDominanceFlag == TACTICAL_DOMINANCE_EVEN)
		{
			return AI_TACTICAL_MOVE_ZONE_ENEMY_EVEN;
		}
		else
		{
			return AI_TACTICAL_MOVE_ZONE_ENEMY_LOSING;
		}
	}

	else
	{
		return AI_TACTICAL_MOVE_ZONE_UNOWNED;
	}
}
//=====================================
// CvTacticalAnalysisMap
//=====================================
/// Constructor
CvTacticalAnalysisMap::CvTacticalAnalysisMap(void) :
	m_pPlots(NULL),
	m_iDominancePercentage(25),
	m_iUnitStrengthMultiplier(5),
	m_iTacticalRange(8),
	m_iTempZoneRadius(5),
	m_iBestFriendlyRange(0),
	m_bIgnoreLOS(false),
	m_pPlayer(NULL),
	m_iNumPlots(0)
{
	m_bIsBuilt = false;
	m_iTurnBuilt = -1;
	m_bAtWar = false;
	m_DominanceZones.clear();
}

/// Destructor
CvTacticalAnalysisMap::~CvTacticalAnalysisMap(void)
{
	SAFE_DELETE_ARRAY(m_pPlots);
}

/// Initialize
void CvTacticalAnalysisMap::Init(int iNumPlots)
{
	// Time building of these maps
	AI_PERF("AI-perf-tact.csv", "CvTacticalAnalysisMap::Init()" );

	if (m_pPlots)
	{
		SAFE_DELETE_ARRAY(m_pPlots);
	}
	m_pPlots = FNEW(CvTacticalAnalysisCell[iNumPlots], c_eCiv5GameplayDLL, 0);
	m_iNumPlots = iNumPlots;

	m_iDominancePercentage = GC.getAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE();
	m_iTacticalRange = GC.getAI_TACTICAL_RECRUIT_RANGE();
	m_iUnitStrengthMultiplier = GC.getAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER() * m_iTacticalRange;
	m_iTempZoneRadius = GC.getAI_TACTICAL_MAP_TEMP_ZONE_RADIUS();
}

/// Fill the map with data for this AI player's turn
void CvTacticalAnalysisMap::RefreshDataForNextPlayer(CvPlayer *pPlayer)
{
	if (m_pPlots)
	{
		if (pPlayer != m_pPlayer || m_iTurnBuilt < GC.getGame().getGameTurn())
		{
			m_pPlayer = pPlayer;
			m_iTurnBuilt = GC.getGame().getGameTurn();

			// Time building of these maps
			AI_PERF_FORMAT("AI-perf.csv", ("Tactical Analysis Map, Turn %d, %s", GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription()) );

			m_bIsBuilt = false;

			// AI civs build this map every turn
			//if (!m_pPlayer->isHuman() && !m_pPlayer->isBarbarian())
			if (!m_pPlayer->isBarbarian())
			{
				m_DominanceZones.clear();

				AddTemporaryZones();

				for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
				{
					CvAssertMsg((iI < m_iNumPlots), "Plot to be accessed exceeds allocation!");

					CvPlot *pPlot = GC.getMap().plotByIndexUnchecked(iI);
					if (pPlot == NULL)
					{
						// Erase this cell
						m_pPlots[iI].Clear();
					}
					else
					{
						if (PopulateCell(iI, pPlot))
						{
							AddToDominanceZones(iI, &m_pPlots[iI]);
						}
					}
				}

				CalculateMilitaryStrengths();
				PrioritizeZones();
				LogZones();
				BuildEnemyUnitList();
				MarkCellsNearEnemy();

				m_bIsBuilt = true;
			}
		}
	}
}

// Find all our enemies (combat units)
void CvTacticalAnalysisMap::BuildEnemyUnitList()
{
	CvTacticalAnalysisEnemy enemy;
	m_EnemyUnits.clear();

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		const PlayerTypes ePlayer = (PlayerTypes)iPlayer;
		CvPlayer &kPlayer = GET_PLAYER(ePlayer);
		const TeamTypes eTeam = kPlayer.getTeam();

		// for each opposing civ
		if (kPlayer.isAlive() && GET_TEAM(eTeam).isAtWar(m_pPlayer->getTeam()))
		{
			int iLoop;
			CvUnit* pLoopUnit = NULL;
			for (pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				// Make sure this unit can attack
				if (pLoopUnit->IsCanAttack())
				{
					m_EnemyUnits.push_back(pLoopUnit);
				}
			}
		}
	}
}

// Indicate the plots we might want to move to that the enemy can attack
void CvTacticalAnalysisMap::MarkCellsNearEnemy()
{
	int iDistance;

	// Look at every cell on the map
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		bool bMarkedIt = false;   // Set true once we've found one that enemy can move past (worst case)

		CvPlot *pPlot = GC.getMap().plotByIndexUnchecked(iI);
		if (m_pPlots[iI].IsRevealed() && !m_pPlots[iI].IsImpassableTerrain() && !m_pPlots[iI].IsImpassableTerritory())
		{
			// Friendly cities always safe
			if (!m_pPlots[iI].IsFriendlyCity())
			{
				if (!pPlot->isVisibleToEnemyTeam(m_pPlayer->getTeam()))
				{
					m_pPlots[iI].SetNotVisibleToEnemy(true);
				}
				else
				{
					for (unsigned int iUnitIndex = 0;  iUnitIndex < m_EnemyUnits.size() && !bMarkedIt; iUnitIndex++)
					{
						CvUnit *pUnit = m_EnemyUnits[iUnitIndex];
						if (pUnit->getArea() == pPlot->getArea())
						{
							// Distance check before hitting pathfinder
							iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
							if (iDistance == 0)
							{
								m_pPlots[iI].SetSubjectToAttack(true);
								m_pPlots[iI].SetEnemyCanMovePast(true);
								bMarkedIt = true;
							}

							// TEMPORARY OPTIMIZATION: Assumes can't use roads or RR
							else if (iDistance <= pUnit->baseMoves())
							{
								int iTurnsToReach;
								iTurnsToReach = TurnsToReachTarget(pUnit, pPlot, true /*bReusePaths*/, true /*bIgnoreUnits*/);	// Its ok to reuse paths because when ignoring units, we don't use the tactical analysis map (which we are building)
								if (iTurnsToReach <= 1)
								{
									m_pPlots[iI].SetSubjectToAttack(true);
								}
								if (iTurnsToReach == 0)
								{
									m_pPlots[iI].SetEnemyCanMovePast(true);
									bMarkedIt = true;
								}
							}
						}
					}

					// Check adjacent plots for enemy citadels
					if (!m_pPlots[iI].IsSubjectToAttack())
					{
						CvPlot *pAdjacentPlot;
						for (int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
						{
							pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)jJ));
							if (pAdjacentPlot != NULL && pAdjacentPlot->getOwner() != NO_PLAYER)
							{
								if (atWar(m_pPlayer->getTeam(), GET_PLAYER(pAdjacentPlot->getOwner()).getTeam()))
								{
									ImprovementTypes eImprovement = pAdjacentPlot->getImprovementType();
									if (eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage() > 0)
									{
										m_pPlots[iI].SetSubjectToAttack(true);
										break;
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

// Clear all dynamic data flags from the map
void CvTacticalAnalysisMap::ClearDynamicFlags()
{
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		// Erase this cell
		m_pPlots[iI].SetWithinRangeOfTarget(false);
		m_pPlots[iI].SetHelpsProvidesFlankBonus(false);
		m_pPlots[iI].SetSafeForDeployment(false);
		m_pPlots[iI].SetDeploymentScore(0);
	}
}

// Mark cells we can use to bomb a specific target
void CvTacticalAnalysisMap::SetTargetBombardCells(CvPlot *pTarget, int iRange, bool bIgnoreLOS)
{
	int iDX, iDY;
	CvPlot *pLoopPlot;
	int iPlotIndex;
	int iPlotDistance;

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(pTarget->getX(), pTarget->getY(), iDX, iDY);
			if (pLoopPlot != NULL)
			{
				iPlotDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pTarget->getX(), pTarget->getY());
				if (iPlotDistance > 0 && iPlotDistance <= iRange)
				{
					iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
					if (m_pPlots[iPlotIndex].IsRevealed() && !m_pPlots[iPlotIndex].IsImpassableTerrain() && !m_pPlots[iPlotIndex].IsImpassableTerritory())
					{
						if (!m_pPlots[iPlotIndex].IsEnemyCity() && !m_pPlots[iPlotIndex].IsNeutralCity())
						{
							if (bIgnoreLOS || pLoopPlot->canSeePlot(pTarget, m_pPlayer->getTeam(), iRange, NO_DIRECTION))
							{
								m_pPlots[iPlotIndex].SetWithinRangeOfTarget(true);
							}
						}
					}
				}
			}
		}
	}
}

// Mark cells we can use to bomb a specific target
void CvTacticalAnalysisMap::SetTargetFlankBonusCells(CvPlot *pTarget)
{
	CvPlot *pLoopPlot;
	int iPlotIndex;

	// No flank attacks on units at sea (where all combat is bombards)
	if (pTarget->isWater())
	{
		return;
	}

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iI));
		if (pLoopPlot != NULL)
		{
			iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
			if (m_pPlots[iPlotIndex].IsRevealed() && !m_pPlots[iPlotIndex].IsImpassableTerrain() && !m_pPlots[iPlotIndex].IsImpassableTerritory())
			{
				if (!m_pPlots[iPlotIndex].IsFriendlyCity() && !m_pPlots[iPlotIndex].IsEnemyCity() && !m_pPlots[iPlotIndex].IsNeutralCity())
				{
					if (!m_pPlots[iPlotIndex].IsFriendlyTurnEndTile() && m_pPlots[iPlotIndex].GetEnemyMilitaryUnit() == NULL)
					{
						m_pPlots[iPlotIndex].SetHelpsProvidesFlankBonus(true);
					}
				}
			}
		}
	}
}

// PRIVATE FUNCTIONS

/// Add in any temporary dominance zones from tactical AI
void CvTacticalAnalysisMap::AddTemporaryZones()
{
	CvTemporaryZone *pZone;
	CvTacticalAI *pTacticalAI = m_pPlayer->GetTacticalAI();

	if (pTacticalAI)
	{
		pTacticalAI->DropObsoleteZones();

		pZone = pTacticalAI->GetFirstTemporaryZone();
		while (pZone)
		{
			// Can't be a city zone (which is just used to boost priority but not establish a new zone)
			if (pZone->GetTargetType() != AI_TACTICAL_TARGET_CITY)
			{
				CvPlot *pPlot = GC.getMap().plot(pZone->GetX(), pZone->GetY());
				if (pPlot)
				{
					CvTacticalDominanceZone newZone;
					newZone.SetDominanceZoneID(m_DominanceZones.size());
					newZone.SetTerritoryType(TACTICAL_TERRITORY_TEMP_ZONE);
					newZone.SetOwner(NO_PLAYER);
					newZone.SetAreaID(pPlot->getArea());
					newZone.SetWater(pPlot->isWater());
					newZone.SetTempZoneCenter(pPlot);
					newZone.SetNavalInvasion(pZone->IsNavalInvasion());
					m_DominanceZones.push_back(newZone);
				}
			}

			pZone = pTacticalAI->GetNextTemporaryZone();
		}
	}
}

/// Update data for a cell: returns whether or not to add to dominance zones
bool CvTacticalAnalysisMap::PopulateCell(int iIndex, CvPlot *pPlot)
{
	CvUnit *pLoopUnit;
	int iUnitLoop;
	CvTacticalAnalysisCell &cell = m_pPlots[iIndex];

	cell.Clear();

	cell.SetRevealed(pPlot->isRevealed(m_pPlayer->getTeam()));
	cell.SetVisible(pPlot->isVisible(m_pPlayer->getTeam()));
	cell.SetImpassableTerrain(pPlot->isImpassable());
	cell.SetWater(pPlot->isWater());
	cell.SetOcean(pPlot->isWater() && !pPlot->isShallowWater());

	bool bImpassableTerritory = false;
	if (pPlot->isOwned())
	{
		TeamTypes eMyTeam = m_pPlayer->getTeam();
		TeamTypes ePlotTeam = pPlot->getTeam();

		if (eMyTeam != ePlotTeam && !GET_TEAM(eMyTeam).isAtWar(ePlotTeam) && !GET_TEAM(ePlotTeam).IsAllowsOpenBordersToTeam(eMyTeam))
		{
			bImpassableTerritory = true;
		}
		else if (pPlot->isCity())
		{
			if (pPlot->getOwner() == m_pPlayer->GetID())
			{
				cell.SetFriendlyCity(true);
			}
			else if (GET_TEAM(eMyTeam).isAtWar(ePlotTeam))
			{
				cell.SetEnemyCity(true);
			}
			else
			{
				cell.SetNeutralCity(true);
			}
		}

		if (m_pPlayer->GetID() == pPlot->getOwner())
		{
			cell.SetOwnTerritory(true);
		}

		if (GET_TEAM(eMyTeam).isFriendlyTerritory(ePlotTeam))
		{
			cell.SetFriendlyTerritory(true);
		}

		if (GET_TEAM(ePlotTeam).isAtWar(ePlotTeam))
		{
			cell.SetEnemyTerritory(true);
		}
	}
	else
	{
		cell.SetUnclaimedTerritory(true);
	}

	cell.SetImpassableTerritory(bImpassableTerritory);
	cell.SetDefenseModifier(pPlot->defenseModifier(NO_TEAM, true));

	if (pPlot->getNumUnits() > 0)
	{
		for (iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
		{
			pLoopUnit = pPlot->getUnitByIndex(iUnitLoop);
			if(!pLoopUnit) continue;
			if (pLoopUnit->getOwner() == m_pPlayer->GetID())
			{
				if (pLoopUnit->IsCombatUnit())
				{
					// CvAssertMsg(!cell.GetFriendlyMilitaryUnit(), "Two friendly military units in a hex, please show Ed and send save.");
					cell.SetFriendlyMilitaryUnit(pLoopUnit);
				}
				else
				{
					// CvAssertMsg(!cell.GetFriendlyCivilianUnit(), "Two friendly civilian units in a hex, please show Ed and send save.");
					cell.SetFriendlyCivilianUnit(pLoopUnit);
				}
			}
			else if (pLoopUnit->isEnemy(m_pPlayer->getTeam()))
			{
				if (pLoopUnit->IsCombatUnit())
				{
					// CvAssertMsg(!cell.GetEnemyMilitaryUnit(), "Two enemy military units in a hex, please show Ed and send save.");
					cell.SetEnemyMilitaryUnit(pLoopUnit);
				}
				else
				{
					// CvAssertMsg(!cell.GetEnemyCivilianUnit(), "Two enemy civilian units in a hex, please show Ed and send save.");
					cell.SetEnemyCivilianUnit(pLoopUnit);
				}
			}
			else
			{
				if (pLoopUnit->IsCombatUnit())
				{
					// CvAssertMsg(!cell.GetNeutralMilitaryUnit(), "Two neutral military units in a hex, please show Ed and send save.");
					cell.SetNeutralMilitaryUnit(pLoopUnit);
				}
				else
				{
					// CvAssertMsg(!cell.GetNeutralCivilianUnit(), "Two neutral civilian units in a hex, please show Ed and send save.");
					cell.SetNeutralCivilianUnit(pLoopUnit);
				}
			}
		}
	}

	// Figure out whether or not to add this to a dominance zone
	bool bAdd = true;
	if (cell.IsImpassableTerrain() || cell.IsImpassableTerritory() || !cell.IsRevealed())
	{
		bAdd = false;
	}
	return bAdd;
}

/// Add data for this cell into dominance zone information
void CvTacticalAnalysisMap::AddToDominanceZones(int iIndex, CvTacticalAnalysisCell *pCell)
{
	CvPlot *pPlot = GC.getMap().plotByIndex(iIndex);

	// Compute zone data for this cell
	m_TempZone.SetAreaID(pPlot->getArea());
	m_TempZone.SetOwner(pPlot->getOwner());
	m_TempZone.SetWater(pPlot->isWater());
	if (!pPlot->isOwned())
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_NO_OWNER);
	}
	else if (pPlot->getTeam() == m_pPlayer->getTeam())
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_FRIENDLY);
	}
	else if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(pPlot->getTeam()))
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_ENEMY);
	}
	else
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_NEUTRAL);
	}
	m_TempZone.SetClosestCity(NULL);
	if (m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_ENEMY ||
		m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL ||
		m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
	{
		int iLoop;
		int iBestDistance = MAX_INT;
		CvCity *pBestCity = NULL;

		for (CvCity *pLoopCity = GET_PLAYER(m_TempZone.GetOwner()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(m_TempZone.GetOwner()).nextCity(&iLoop))
		{
			int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());
			if (iDistance < iBestDistance)
			{
				iBestDistance = iDistance;
				pBestCity = pLoopCity;
			}
		}

		if (pBestCity != NULL)
		{
			m_TempZone.SetClosestCity(pBestCity);
		}
	}

	// Now see if we already have a matching zone
	CvTacticalDominanceZone *pZone = FindExistingZone(pPlot);
	if (!pZone)
	{
		// Data populated, now add to vector
		m_TempZone.SetDominanceZoneID(m_DominanceZones.size());
		m_DominanceZones.push_back(m_TempZone);
		pZone = &m_DominanceZones[m_DominanceZones.size() - 1];
	}

	// If this isn't owned territory, update zone with military strength info
	if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER ||
		pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
	{
		CvUnit *pFriendlyUnit = pCell->GetFriendlyMilitaryUnit();
		if (pFriendlyUnit)
		{
			if (pFriendlyUnit->getDomainType() == DOMAIN_AIR ||
				(pFriendlyUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) ||
				(pFriendlyUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater()))
			{
				int iStrength = pFriendlyUnit->GetBaseCombatStrengthConsideringDamage();
				if (iStrength == 0 && pFriendlyUnit->isEmbarked() && !pZone->IsWater())
				{
					iStrength = pFriendlyUnit->GetBaseCombatStrength(true);
				}
				pZone->AddFriendlyStrength(iStrength * m_iUnitStrengthMultiplier);
				pZone->AddFriendlyRangedStrength(pFriendlyUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true));
				if (pFriendlyUnit->GetRange() > GetBestFriendlyRange())
				{
					SetBestFriendlyRange(pFriendlyUnit->GetRange());
				}
				if (pFriendlyUnit->IsRangeAttackIgnoreLOS())
				{
					SetIgnoreLOS(true);
				}
				pZone->AddFriendlyUnitCount(1);
				if (pFriendlyUnit->isRanged())
				{
					pZone->AddFriendlyRangedUnitCount(1);
				}
			}
		}

		CvUnit *pEnemyUnit = pCell->GetEnemyMilitaryUnit();
		if (pEnemyUnit)
		{
			if (pEnemyUnit->getDomainType() == DOMAIN_AIR ||
				(pEnemyUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) ||
				(pEnemyUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater()))
			{
				int iStrength = pEnemyUnit->GetBaseCombatStrengthConsideringDamage();
				if (iStrength == 0 && pEnemyUnit->isEmbarked() && !pZone->IsWater())
				{
					iStrength = pEnemyUnit->GetBaseCombatStrength(true);
				}
				pZone->AddEnemyStrength(iStrength * m_iUnitStrengthMultiplier);
				pZone->AddEnemyRangedStrength(pEnemyUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true));
				pZone->AddEnemyUnitCount(1);
				if (pEnemyUnit->isRanged())
				{
					pZone->AddEnemyRangedUnitCount(1);
				}
			}
		}
	}

	// Set zone for this cell
	pCell->SetDominanceZone(pZone->GetDominanceZoneID());
}

/// Calculate military presences in each owned dominance zone
void CvTacticalAnalysisMap::CalculateMilitaryStrengths()
{
	// Loop through the dominance zones
	CvTacticalDominanceZone *pZone;
	CvCity *pClosestCity = NULL;
	int iDistance;
	int iMultiplier;
	int iLoop;
	CvUnit* pLoopUnit;
	TeamTypes eTeam;

	eTeam = m_pPlayer->getTeam();

	for (unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		pZone = &m_DominanceZones[iI];

		if (pZone->GetTerritoryType() != TACTICAL_TERRITORY_NO_OWNER)
		{
			pClosestCity = pZone->GetClosestCity();
			if (pClosestCity)
			{
				// Start with strength of the city itself
				int iStrength = pClosestCity->getStrengthValue() * m_iTacticalRange;
				if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					pZone->AddFriendlyStrength(iStrength);
					pZone->AddFriendlyRangedStrength(pClosestCity->getStrengthValue());
				}
				else
				{
					pZone->AddEnemyStrength(iStrength);
					pZone->AddEnemyRangedStrength(pClosestCity->getStrengthValue());
				}

				// Loop through all of OUR units first
				for (pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
				{
					if (pLoopUnit->IsCombatUnit())
					{
						if (pLoopUnit->getDomainType() == DOMAIN_AIR ||
							(pLoopUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) ||
							(pLoopUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater()))
						{
							iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pClosestCity->getX(), pClosestCity->getY());
							iMultiplier = (m_iTacticalRange + 1 - iDistance);
							if (iMultiplier > 0)
							{
								int iUnitStrength = pLoopUnit->GetBaseCombatStrengthConsideringDamage();
								if (iUnitStrength == 0 && pLoopUnit->isEmbarked() && !pZone->IsWater())
								{
									iUnitStrength = pLoopUnit->GetBaseCombatStrength(true);
								}
								pZone->AddFriendlyStrength(iUnitStrength * iMultiplier * m_iUnitStrengthMultiplier);
								pZone->AddFriendlyRangedStrength(pLoopUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true));
								if (pLoopUnit->GetRange() > GetBestFriendlyRange())
								{
									SetBestFriendlyRange(pLoopUnit->GetRange());
								}
								if (pLoopUnit->IsRangeAttackIgnoreLOS())
								{
									SetIgnoreLOS(true);
								}
								pZone->AddFriendlyUnitCount(1);
								if (pLoopUnit->isRanged())
								{
									pZone->AddFriendlyRangedUnitCount(1);
								}
							}
						}
					}
				}

				// Repeat for all visible enemy units (or adjacent to visible)
				for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					CvPlayer &kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);
					if (GET_TEAM(eTeam).isAtWar(kPlayer.getTeam()))
					{
						for (pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
						{
							if (pLoopUnit->IsCombatUnit())
							{
								if (pLoopUnit->getDomainType() == DOMAIN_AIR ||
									(pLoopUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) ||
									(pLoopUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater()))
								{
									CvPlot *pPlot;
									pPlot = pLoopUnit->plot();
									if (pPlot)
									{
										if (pPlot->isVisible(eTeam) || pPlot->isAdjacentVisible(eTeam))
										{
											iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pClosestCity->getX(), pClosestCity->getY());
											iMultiplier = (m_iTacticalRange + 1 - iDistance);
											if (iMultiplier > 0)
											{
												int iUnitStrength = pLoopUnit->GetBaseCombatStrengthConsideringDamage();
												if (iUnitStrength == 0 && pLoopUnit->isEmbarked() && !pZone->IsWater())
												{
													iUnitStrength = pLoopUnit->GetBaseCombatStrength(true);
												}
												pZone->AddEnemyStrength(iUnitStrength * iMultiplier * m_iUnitStrengthMultiplier);
												pZone->AddEnemyRangedStrength(pLoopUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true));
												pZone->AddEnemyUnitCount(1);
												if (iDistance < pZone->GetRangeClosestEnemyUnit())
												{
													pZone->SetRangeClosestEnemyUnit(iDistance);
												}
												if (pLoopUnit->isRanged())
												{
													pZone->AddEnemyRangedUnitCount(1);
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
		}
	}
}

/// Establish order of zone processing for the turn
void CvTacticalAnalysisMap::PrioritizeZones()
{
	// Loop through the dominance zones
	CvTacticalDominanceZone *pZone;
	int iBaseValue;
	int iMultiplier;
	CvCity *pClosestCity = NULL;

	for (unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		// Find the zone and compute dominance here
		pZone = &m_DominanceZones[iI];
		eTacticalDominanceFlags eDominance = ComputeDominance(pZone);

		// Establish a base value for the region
		iBaseValue = 1;

		// Temporary zone?
		if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
		{
			iMultiplier = 200;
		}
		else
		{
			pClosestCity = pZone->GetClosestCity();

			if (pClosestCity)
			{
				iBaseValue += pZone->GetClosestCity()->getPopulation();

				if (pClosestCity->isCapital())
				{
					iBaseValue *= 2;
				}

				// How damaged is this city?
				int iDamage = pClosestCity->getDamage();
				if (iDamage > 0)
				{
					iBaseValue *= ((iDamage + 2)/ 2);
				}

				if (m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pClosestCity))
				{
					iBaseValue *= 3;
				}

			}
			if (!pZone->IsWater())
			{
				iBaseValue *= 8;
			}

			// Now compute a multiplier based on current conditions here
			iMultiplier = 1;
			if (eDominance == TACTICAL_DOMINANCE_ENEMY)
			{
				if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier = 2;
				}
				else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier = 4;
				}
			}
			else if (eDominance == TACTICAL_DOMINANCE_EVEN)
			{
				if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier = 3;
				}
				else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier = 3;
				}
			}
			else if (eDominance == TACTICAL_DOMINANCE_FRIENDLY)
			{
				if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier = 4;
				}
				else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier = 1;
				}
			}
			if (!m_pPlayer->isMinorCiv())
			{
				if (m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
				{
					if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
					{
						iMultiplier *= 2;
					}
				}
				else if (m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
				{
					if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
					{
						iMultiplier *= 4;
					}
				}
			}
		}

		// Save off the value for this zone
		if ((iBaseValue * iMultiplier) <= 0)
		{
			FAssertMsg((iBaseValue * iMultiplier) > 0, "Invalid Dominance Zone Value");
		}
		pZone->SetDominanceZoneValue(iBaseValue * iMultiplier);
	}

	std::stable_sort(m_DominanceZones.begin(), m_DominanceZones.end());
}

/// Log dominance zone data
void CvTacticalAnalysisMap::LogZones()
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString szLogMsg;
		CvTacticalDominanceZone *pZone;

		for (unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
		{
			pZone = &m_DominanceZones[iI];

			// Log all zones with at least some unit present
//			if (pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_NO_UNITS_PRESENT && pZone->GetDominanceFlag() != TACTICAL_DOMINANCE_NOT_VISIBLE)
//			{
			szLogMsg.Format("Zone ID: %d, Area ID: %d, Value: %d, FRIENDLY Str: %d (%d), Ranged: %d (%d), ENEMY Str: %d (%d), Ranged: %d (%d), Closest Enemy: %d",
					pZone->GetDominanceZoneID(), pZone->GetAreaID(), pZone->GetDominanceZoneValue(),
					pZone->GetFriendlyStrength(), pZone->GetFriendlyUnitCount(), pZone->GetFriendlyRangedStrength(), pZone->GetFriendlyRangedUnitCount(),
					pZone->GetEnemyStrength(), pZone->GetEnemyUnitCount(), pZone->GetEnemyRangedStrength(), pZone->GetEnemyRangedUnitCount(), pZone->GetRangeClosestEnemyUnit());
				if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
				{
					szLogMsg += ", Friendly";
				}
				else if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
				{
					szLogMsg += ", Enemy";
				}
				else if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_EVEN)
				{
					szLogMsg += ", Even";
				}
				else if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_NO_UNITS_PRESENT)
				{
					szLogMsg += ", No Units";
				}
				else if (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_NOT_VISIBLE)
				{
					szLogMsg += ", Not Visible";
				}

				if (pZone->IsWater())
				{
					szLogMsg += ", Water";
				}
				else
				{
					szLogMsg += ", Land";
				}

				if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
				{
					szLogMsg += ", Temporary Zone";
				}
				else if (pZone->GetClosestCity())
				{
					szLogMsg += ", " + pZone->GetClosestCity()->getName();
				}

				m_pPlayer->GetTacticalAI()->LogTacticalMessage(szLogMsg, true /*bSkipLogDominanceZone*/);
//			}
		}
	}
}

/// Can this cell go in an existing dominance zone?
CvTacticalDominanceZone *CvTacticalAnalysisMap::FindExistingZone(CvPlot *pPlot)
{
	CvTacticalDominanceZone *pZone;

	for (unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		pZone = &m_DominanceZones[iI];

		// If this is a temporary zone, matches if unowned and close enough
		if ((pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE) &&
			(m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL) &&
			(plotDistance(pPlot->getX(), pPlot->getY(), pZone->GetTempZoneCenter()->getX(), pZone->GetTempZoneCenter()->getY()) <= m_iTempZoneRadius))
		{
			return pZone;
		}

		// If not friendly or enemy, just 1 zone per area
		if ((pZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || pZone->GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL) &&
			(m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL))
		{
			if (pZone->GetAreaID() == m_TempZone.GetAreaID())
			{
				return pZone;
			}
		}

		// Otherwise everything needs to match
		if (pZone->GetTerritoryType() == m_TempZone.GetTerritoryType() &&
			pZone->GetOwner() == m_TempZone.GetOwner() &&
			pZone->GetAreaID() == m_TempZone.GetAreaID() &&
			pZone->GetClosestCity() == m_TempZone.GetClosestCity())
		{
			return pZone;
		}
	}

	return NULL;
}

/// Retrieve a dominance zone
CvTacticalDominanceZone *CvTacticalAnalysisMap::GetZone(int iIndex)
{
	if(iIndex < 0 || iIndex >= (int)m_DominanceZones.size())
		return 0;
	return &m_DominanceZones[iIndex];
}

/// Retrieve a dominance zone by closest city
CvTacticalDominanceZone *CvTacticalAnalysisMap::GetZoneByCity(CvCity *pCity, bool bWater)
{
	CvTacticalDominanceZone *pZone;
	for (int iI = 0; iI < GetNumZones(); iI++)
	{
		pZone = GetZone(iI);
		if (pZone->GetClosestCity() == pCity && pZone->IsWater() == bWater)
		{
			return pZone;
		}
	}

	return NULL;
}

// Is this plot in dangerous territory?
bool CvTacticalAnalysisMap::IsInEnemyDominatedZone(CvPlot *pPlot)
{
	CvTacticalAnalysisCell *pCell;
	int iPlotIndex;
	CvTacticalDominanceZone *pZone;

	iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
	pCell = GetCell(iPlotIndex);

	for (int iI = 0; iI < GetNumZones(); iI++)
	{
		pZone = GetZone(iI);
		if (pZone->GetDominanceZoneID() == pCell->GetDominanceZone())
		{
			return (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY || pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_NOT_VISIBLE);
		}
	}

	return false;
}

/// Who is dominant in this one zone?
eTacticalDominanceFlags CvTacticalAnalysisMap::ComputeDominance(CvTacticalDominanceZone *pZone)
{
	bool bTempZone = false;
	if (pZone->GetClosestCity())
	{
		bTempZone = m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pZone->GetClosestCity());
	}

	// Look at ratio of friendly to enemy strength
	if (pZone->GetFriendlyStrength() + pZone->GetEnemyStrength() <= 0)
	{
		pZone->SetDominanceFlag(TACTICAL_DOMINANCE_NO_UNITS_PRESENT);
	}

	// Enemy zone that we can't see (that isn't one of our temporary targets?
	else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY && !(pZone->GetClosestCity()->plot())->isVisible(m_pPlayer->getTeam()) && !bTempZone)
	{
		pZone->SetDominanceFlag(TACTICAL_DOMINANCE_NOT_VISIBLE);
	}
	else
	{
		bool bEnemyCanSeeOurCity = false;
		if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
				if (kPlayer.isAlive())
				{
					TeamTypes eOtherTeam = kPlayer.getTeam();
					if (eOtherTeam != m_pPlayer->getTeam())
					{
						if (GET_TEAM(eOtherTeam).isAtWar(m_pPlayer->getTeam()))
						{
							if (pZone->GetClosestCity()->plot()->isVisible(eOtherTeam))
							{
								bEnemyCanSeeOurCity = true;
								break;
							}
						}
					}
				}
			}
		}

		if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY && !bEnemyCanSeeOurCity)
		{
			pZone->SetDominanceFlag(TACTICAL_DOMINANCE_NOT_VISIBLE);
		}

		// Otherwise compute it by strength
		else if (pZone->GetEnemyStrength() <= 0)
		{
			pZone->SetDominanceFlag(TACTICAL_DOMINANCE_FRIENDLY);
		}
		else
		{
			int iRatio = (pZone->GetFriendlyStrength()  * 100) / pZone->GetEnemyStrength();
			if (iRatio > 100 + m_iDominancePercentage)
			{
				pZone->SetDominanceFlag(TACTICAL_DOMINANCE_FRIENDLY);
			}
			else if (iRatio < 100 - m_iDominancePercentage)
			{
				pZone->SetDominanceFlag(TACTICAL_DOMINANCE_ENEMY);
			}
			else
			{
				pZone->SetDominanceFlag(TACTICAL_DOMINANCE_EVEN);
			}
		}
	}

	return pZone->GetDominanceFlag();
}

