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
	m_iDominanceZoneID(-1),
	m_iTargetDistance(INT_MAX),
	m_bHasLOSToTarget(false)
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
	m_iTargetDistance = INT_MAX;
	m_bHasLOSToTarget = false;
}

bool CvTacticalAnalysisCell::CanUseForOperationGathering()
{
	if(IsImpassableTerrain() || IsImpassableTerritory() || GetEnemyMilitaryUnit() || GetNeutralMilitaryUnit() || GetNeutralCivilianUnit() || IsFriendlyTurnEndTile() || IsEnemyCity() || IsNeutralCity())
	{
		return false;
	}

	return true;
}

bool CvTacticalAnalysisCell::CanUseForOperationGatheringCheckWater(bool bWater)
{
	if(bWater != IsWater())
	{
		return false;
	}
	return CanUseForOperationGathering();
}

//=====================================
// CvTacticalDominanceZone
//=====================================
/// Constructor
CvTacticalDominanceZone::CvTacticalDominanceZone(void)
{
	m_iDominanceZoneID = -1;
	m_eTerritoryType = TACTICAL_TERRITORY_NONE;
	m_eDominanceFlag = TACTICAL_DOMINANCE_NO_UNITS_VISIBLE;
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
#if defined(MOD_BALANCE_CORE_MILITARY)
	m_iFriendlyMeleeUnitCount = 0;
	m_iEnemyMeleeUnitCount = 0;
	m_iNeutralUnitStrength = 0;
	m_iNeutralUnitCount = 0;
#endif
	m_iEnemyRangedUnitCount = 0;
	m_iEnemyNavalUnitCount = 0;
	m_iZoneValue = 0;
	m_iRangeClosestEnemyUnit = -1;
	m_bIsWater = false;
	m_bIsNavalInvasion = false;
	m_pTempZoneCenter = NULL;

#if defined(MOD_BALANCE_CORE_MILITARY)
	m_iAvgX = m_iAvgY = 0;
	m_iPlotCount = 0;
#endif
}

/// Retrieve city controlling this zone
CvCity* CvTacticalDominanceZone::GetZoneCity() const
{
	if(m_eOwner != NO_PLAYER)
	{
		return GET_PLAYER(m_eOwner).getCity(m_iCityID);
	}

	return NULL;
}

/// Set city controlling this zone
void CvTacticalDominanceZone::SetZoneCity(CvCity* pCity)
{
	if(pCity != NULL)
	{
		m_iCityID = pCity->GetID();
	}
	else
	{
		m_iCityID = -1;
	}
}

#if defined(MOD_BALANCE_CORE_MILITARY)
void CvTacticalDominanceZone::Extend(CvPlot* pPlot)
{
	if (!pPlot)
		return;

	if (m_iPlotCount==0)
	{
		m_iPlotCount = 1;
		m_iAvgX = pPlot->getX() * 1000;
		m_iAvgY = pPlot->getY() * 1000;
	}
	else
	{
		//need to take care of wrap-around ... should probably do all this in hex-space
		int iHalfWidth = GC.getMap().getGridWidth()*1000/2;
		int iHalfHeight = GC.getMap().getGridHeight()*1000/2;

		int iDX = pPlot->getX()*1000 - m_iAvgX;
		int iDY = pPlot->getY()*1000 - m_iAvgY;

		if (GC.getMap().isWrapX() && iDX < -iHalfWidth)
			iDX += 2*iHalfWidth;
		if (GC.getMap().isWrapX() && iDX > +iHalfWidth)
			iDX -= 2*iHalfWidth;
		if (GC.getMap().isWrapY() && iDY < -iHalfHeight)
			iDY += 2*iHalfHeight;
		if (GC.getMap().isWrapY() && iDY > +iHalfHeight)
			iDY -= 2*iHalfHeight;

		m_iPlotCount++;
		m_iAvgX += iDX/m_iPlotCount;
		m_iAvgY += iDY/m_iPlotCount;

		if (m_iAvgX<0)
			m_iAvgX += iHalfWidth*2;
		if (m_iAvgX>iHalfWidth*2)
			m_iAvgX -= iHalfWidth*2;
		if (m_iAvgY<0)
			m_iAvgY += iHalfHeight*2;
		if (m_iAvgY>iHalfHeight*2)
			m_iAvgY += iHalfHeight*2;
	}
}
#endif

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
	if(m_eTerritoryType == TACTICAL_TERRITORY_FRIENDLY)
	{
		if(m_eDominanceFlag == TACTICAL_DOMINANCE_FRIENDLY)
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_WINNING;
		}
		else if(m_eDominanceFlag == TACTICAL_DOMINANCE_EVEN)
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_EVEN;
		}
		else
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_LOSING;
		}
	}

	else if(m_eTerritoryType == TACTICAL_TERRITORY_ENEMY)
	{
		if(m_eDominanceFlag == TACTICAL_DOMINANCE_FRIENDLY)
		{
			return AI_TACTICAL_MOVE_ZONE_ENEMY_WINNING;
		}
		else if(m_eDominanceFlag == TACTICAL_DOMINANCE_EVEN)
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
	m_pPlayer(NULL),
	m_iNumPlots(0),
	m_iTurnBuilt(-1)
{
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

	if(m_pPlots)
	{
		SAFE_DELETE_ARRAY(m_pPlots);
	}
	m_pPlots = FNEW(CvTacticalAnalysisCell[iNumPlots], c_eCiv5GameplayDLL, 0);
	m_iNumPlots = iNumPlots;

	m_iDominancePercentage = GC.getAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE();
}

#if defined(MOD_BALANCE_CORE_MILITARY)
PlayerTypes CvTacticalAnalysisMap::GetCurrentPlayer() const
{
	if (m_pPlayer)
		return m_pPlayer->GetID();
	else
		return NO_PLAYER;
}

void CvTacticalDominanceZone::AddNeighboringZone(int iZoneID)
{ 
	if (iZoneID==m_iDominanceZoneID || iZoneID==-1)
		return;

	std::vector<int>::iterator it = std::find(m_vNeighboringZones.begin(),m_vNeighboringZones.end(),iZoneID);

	if (it==m_vNeighboringZones.end())
		m_vNeighboringZones.push_back(iZoneID);
}

void CvTacticalAnalysisMap::EstablishZoneNeighborhood()
{
	//walk over the map and see which zones are adjacent
	int iW = GC.getMap().getGridWidth();
	int iH = GC.getMap().getGridHeight();

	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		m_DominanceZones[iI].ClearNeighboringZones();
	}

	for (int i=0; i<iW; i++)
	{
		for (int j=0; j<iH; j++)
		{
			CvPlot* pA = GC.getMap().plot(i,j);
			CvPlot* pB = GC.getMap().plot(i,j+1);
			CvPlot* pC = GC.getMap().plot(i+1,j);

			CvTacticalAnalysisCell* cA = pA ? GetCell( pA->GetPlotIndex() ) : NULL;
			CvTacticalAnalysisCell* cB = pB ? GetCell( pB->GetPlotIndex() ) : NULL;
			CvTacticalAnalysisCell* cC = pC ? GetCell( pC->GetPlotIndex() ) : NULL;

			if (cA && cB)
			{
				int iA = cA->GetDominanceZone();
				int iB = cB->GetDominanceZone();
				if (iA!=-1 && iB!=-1 && 
					GetZoneByID(iA)->GetTerritoryType()!=TACTICAL_TERRITORY_NO_OWNER &&
					GetZoneByID(iB)->GetTerritoryType()!=TACTICAL_TERRITORY_NO_OWNER
					)
				{
					GetZoneByID(iA)->AddNeighboringZone(iB);
					GetZoneByID(iB)->AddNeighboringZone(iA);
				}
			}
			if (cA && cC)
			{
				int iA = cA->GetDominanceZone();
				int iC = cC->GetDominanceZone();
				if (iA!=-1 && iC!=-1 &&
					GetZoneByID(iA)->GetTerritoryType()!=TACTICAL_TERRITORY_NO_OWNER &&
					GetZoneByID(iC)->GetTerritoryType()!=TACTICAL_TERRITORY_NO_OWNER
					)
				{
					GetZoneByID(iA)->AddNeighboringZone(iC);
					GetZoneByID(iC)->AddNeighboringZone(iA);
				}
			}
		}
	}
}

#endif

bool CvTacticalAnalysisMap::IsUpToDate(CvPlayer* pPlayer)
{
	if(m_pPlots && pPlayer == m_pPlayer && m_iTurnBuilt == GC.getGame().getGameTurn())
		return true;

	return false;
}

/// Fill the map with data for this AI player's turn
void CvTacticalAnalysisMap::RefreshDataForNextPlayer(CvPlayer* pPlayer)
{
	if(m_pPlots)
	{
		if(!IsUpToDate(pPlayer))
		{
			m_pPlayer = pPlayer;
			m_iTurnBuilt = GC.getGame().getGameTurn();
			m_iTacticalRange = ((GC.getAI_TACTICAL_RECRUIT_RANGE() + GC.getGame().getCurrentEra()) * 2) / 3;  // Have this increase as game goes on
			m_iUnitStrengthMultiplier = GC.getAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER() * m_iTacticalRange;

			AI_PERF_FORMAT("AI-perf.csv", ("Tactical Analysis Map, Turn %d, %s", GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription()) );

#if !defined(MOD_BALANCE_CORE_MILITARY)
			// AI civs build this map every turn
			//if (!m_pPlayer->isHuman() && !m_pPlayer->isBarbarian())
			if(!m_pPlayer->isBarbarian())
#endif
			{
				m_DominanceZones.clear();

				AddTemporaryZones();

				for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
				{
					CvAssertMsg((iI < m_iNumPlots), "Plot to be accessed exceeds allocation!");

					CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);
					if(pPlot == NULL)
					{
						// Erase this cell
						m_pPlots[iI].Clear();
					}
					else
					{
						if(PopulateCell(iI, pPlot))
						{
							AddToDominanceZones(iI, &m_pPlots[iI]);
						}
					}
				}

#if defined(MOD_BALANCE_CORE_MILITARY)
				//barbarians don't care about tactical dominance
				if(!m_pPlayer->isBarbarian())
				{
					EstablishZoneNeighborhood();
					CalculateMilitaryStrengths();
					PrioritizeZones();
					LogZones();
				}
#else
				CalculateMilitaryStrengths();
				PrioritizeZones();
				LogZones();
#endif

				BuildEnemyUnitList();
				MarkCellsNearEnemy();
			}
		}
	}
}

// Find all our enemies (combat units)
void CvTacticalAnalysisMap::BuildEnemyUnitList()
{
	CvTacticalAnalysisEnemy enemy;
	m_EnemyUnits.clear();
#if defined(MOD_BALANCE_CORE_MILITARY)
	m_EnemyCities.clear();
#endif

	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		const PlayerTypes ePlayer = (PlayerTypes)iPlayer;
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		const TeamTypes eTeam = kPlayer.getTeam();

		// for each opposing civ
		if(kPlayer.isAlive() && GET_TEAM(eTeam).isAtWar(m_pPlayer->getTeam()))
		{
			int iLoop;
			CvUnit* pLoopUnit = NULL;
#if defined(MOD_BALANCE_CORE_MILITARY)
			CvCity* pLoopCity;
			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				if (pLoopCity->plot()->isRevealed(m_pPlayer->getTeam()))
					m_EnemyCities.push_back(pLoopCity);

			for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
				if(pLoopUnit->IsCanAttack() && pLoopUnit->plot()->isVisible(m_pPlayer->getTeam()))
					m_EnemyUnits.push_back(pLoopUnit);
#else
			for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				// Make sure this unit can attack
				if(pLoopUnit->IsCanAttack())
				{
					m_EnemyUnits.push_back(pLoopUnit);
				}
			}
#endif
		}
	}
}

// Indicate the plots we might want to move to that the enemy can attack
void CvTacticalAnalysisMap::MarkCellsNearEnemy()
{
#if defined(MOD_BALANCE_CORE)
	for(unsigned int iUnitIndex = 0;  iUnitIndex < m_EnemyUnits.size(); iUnitIndex++)
	{
		CvUnit* pUnit = m_EnemyUnits[iUnitIndex];
		ReachablePlots tiles;

		//for ranged every plot we can enter with movement left is a base for attack
		int iMinMovesLeft = pUnit->IsCanAttackRanged() ? 1 : 0;
		if (pUnit->isMustSetUpToRangedAttack())
			iMinMovesLeft += GC.getMOVE_DENOMINATOR();

		//be a bit conservative here, use ZOC - if one of our units is killed, this is not correct anymore
		//therefore we later do a dilation filter on the cells
		TacticalAIHelpers::GetAllPlotsInReachThisTurn(pUnit,pUnit->plot(),tiles,false,true,false,iMinMovesLeft);

		for (ReachablePlots::iterator moveTile=tiles.begin(); moveTile!=tiles.end(); ++moveTile)
		{
			CvPlot* pMoveTile = GC.getMap().plotByIndexUnchecked(moveTile->iPlotIndex);
			int iPlotIndex = GC.getMap().plotNum(pMoveTile->getX(),pMoveTile->getY());

			if (pUnit->IsCanAttackRanged())
			{
				m_pPlots[iPlotIndex].SetEnemyCanMovePast(true);
				std::set<int> rangedPlots;
				//this generates some overlap, but preventing that is about as bad as ignoring it
				TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pUnit,pMoveTile,rangedPlots);
				for (std::set<int>::iterator attackTile=rangedPlots.begin(); attackTile!=rangedPlots.end(); ++attackTile)
				{
					m_pPlots[*attackTile].SetSubjectToAttack(true);
				}
			}
			else
			{
				//for melee every tile he can move into can be attacked
				m_pPlots[iPlotIndex].SetSubjectToAttack(true);
				if (moveTile->iMovesLeft>0)
					m_pPlots[iPlotIndex].SetEnemyCanMovePast(true);
			}
		}
	}

	//do the dilation
	std::vector<int> vCellsToMark;
	for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		//nothing to do
		if (m_pPlots[iPlotLoop].IsSubjectToAttack())
			continue;

		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		//danger plots are calculated without ZOC
		if (m_pPlayer->GetPossibleAttackers(*pPlot).size()>0)
		{
			//check whether neighbors are subject to attack with ZOC
			CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pPlot);
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
				if (pAdjacentPlot)
				{
					if (m_pPlots[pAdjacentPlot->GetPlotIndex()].IsSubjectToAttack())
					{
						vCellsToMark.push_back(iPlotLoop);
						break;
					}
				}
			}
		}
	}
	//this should give a nice compromise
	for (size_t iI = 0; iI < vCellsToMark.size(); iI++)
		m_pPlots[vCellsToMark[iI]].SetSubjectToAttack(true);
#endif

	// Look at every cell on the map
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);
		if(m_pPlots[iI].IsRevealed() && !m_pPlots[iI].IsImpassableTerrain() && !m_pPlots[iI].IsImpassableTerritory())
		{
			// Friendly cities always safe
			if(!m_pPlots[iI].IsFriendlyCity())
			{
				if(!pPlot->isVisibleToEnemyTeam(m_pPlayer->getTeam()))
				{
					m_pPlots[iI].SetNotVisibleToEnemy(true);
				}
				else
				{
#if defined(MOD_BALANCE_CORE)
#else
					bool bMarkedIt = false;   // Set true once we've found one that enemy can move past (worst case)
					for(unsigned int iUnitIndex = 0;  iUnitIndex < m_EnemyUnits.size() && !bMarkedIt; iUnitIndex++)
					{
						CvUnit* pUnit = m_EnemyUnits[iUnitIndex];
						if(pUnit->getArea() == pPlot->getArea())
						{
							// Distance check before hitting pathfinder
							int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pPlot->getX(), pPlot->getY());
							if(iDistance == 0)
							{
								m_pPlots[iI].SetSubjectToAttack(true);
								m_pPlots[iI].SetEnemyCanMovePast(true);
								bMarkedIt = true;
							}
							else if(iDistance <= pUnit->baseMoves())
							{
								int iTurnsToReach;

								// Its ok to reuse paths because when ignoring units, we don't use the tactical analysis map (which we are building)
								iTurnsToReach = pUnit->TurnsToReachTarget(pPlot, true /*bIgnoreUnits*/);

								if(iTurnsToReach <= 1)
								{
									m_pPlots[iI].SetSubjectToAttack(true);
								}
								if(iTurnsToReach == 0)
								{
									m_pPlots[iI].SetEnemyCanMovePast(true);
									bMarkedIt = true;
								}
							}
						}
					}
#endif

					// Check adjacent plots for enemy citadels
					if(!m_pPlots[iI].IsSubjectToAttack())
					{
#if defined(MOD_BALANCE_CORE)
						if ( pPlot->IsNearEnemyCitadel( m_pPlayer->GetID() ) )
								m_pPlots[iI].SetSubjectToAttack(true);

						for(unsigned int iCityIndex = 0;  iCityIndex < m_EnemyCities.size(); iCityIndex++)
						{
							CvCity* pCity = m_EnemyCities[iCityIndex];
							if (pCity->canRangeStrikeAt( pPlot->getX(), pPlot->getY() ))
								m_pPlots[iI].SetSubjectToAttack(true);
						}
#else
						CvPlot* pAdjacentPlot;
						for(int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
						{
							pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)jJ));
							if(pAdjacentPlot != NULL && pAdjacentPlot->getOwner() != NO_PLAYER)
							{
								if(atWar(m_pPlayer->getTeam(), GET_PLAYER(pAdjacentPlot->getOwner()).getTeam()))
								{
									ImprovementTypes eImprovement = pAdjacentPlot->getImprovementType();
									if(eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage() > 0)
									{
										m_pPlots[iI].SetSubjectToAttack(true);
										break;
									}
								}
							}
						}
#endif
					}
				}
			}
		}
	}
}

// Clear all dynamic data flags from the map
void CvTacticalAnalysisMap::ClearDynamicFlags()
{
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		// Erase this cell
		m_pPlots[iI].SetWithinRangeOfTarget(false);
		m_pPlots[iI].SetHelpsProvidesFlankBonus(false);
		m_pPlots[iI].SetSafeForDeployment(false);
		m_pPlots[iI].SetDeploymentScore(0);
		m_pPlots[iI].SetTargetDistance(INT_MAX);
		m_pPlots[iI].SetHasLOS(false);
	}
}

// Mark cells we can use to bomb a specific target
void CvTacticalAnalysisMap::SetTargetFlankBonusCells(CvPlot* pTarget)
{
	CvPlot* pLoopPlot;
	int iPlotIndex;

	// No flank attacks on units at sea (where all combat is bombards)
	if(pTarget->isWater())
	{
		return;
	}

	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iI));
		if(pLoopPlot != NULL)
		{
			iPlotIndex = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());
			if(m_pPlots[iPlotIndex].IsRevealed() && !m_pPlots[iPlotIndex].IsImpassableTerrain() && !m_pPlots[iPlotIndex].IsImpassableTerritory())
			{
				if(!m_pPlots[iPlotIndex].IsFriendlyCity() && !m_pPlots[iPlotIndex].IsEnemyCity() && !m_pPlots[iPlotIndex].IsNeutralCity())
				{
					if(!m_pPlots[iPlotIndex].IsFriendlyTurnEndTile() && m_pPlots[iPlotIndex].GetEnemyMilitaryUnit() == NULL)
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
	CvTemporaryZone* pZone;
	CvTacticalAI* pTacticalAI = m_pPlayer->GetTacticalAI();

	if(pTacticalAI)
	{
		pTacticalAI->DropObsoleteZones();

		pZone = pTacticalAI->GetFirstTemporaryZone();
		while(pZone)
		{
			// Can't be a city zone (which is just used to boost priority but not establish a new zone)
			if(pZone->GetTargetType() != AI_TACTICAL_TARGET_CITY)
			{
				CvPlot* pPlot = GC.getMap().plot(pZone->GetX(), pZone->GetY());
				if(pPlot)
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
bool CvTacticalAnalysisMap::PopulateCell(int iIndex, CvPlot* pPlot)
{
	CvUnit* pLoopUnit;
	int iUnitLoop;
	CvTacticalAnalysisCell& cell = m_pPlots[iIndex];

	cell.Clear();

	cell.SetRevealed(pPlot->isRevealed(m_pPlayer->getTeam()));
	cell.SetVisible(pPlot->isVisible(m_pPlayer->getTeam()));
	cell.SetImpassableTerrain(!pPlot->isValidMovePlot(m_pPlayer->GetID()));
	cell.SetWater(pPlot->isWater());
	cell.SetOcean(pPlot->isWater() && !pPlot->isShallowWater());

	bool bImpassableTerritory = false;
	if(pPlot->isOwned())
	{
		TeamTypes eMyTeam = m_pPlayer->getTeam();
		TeamTypes ePlotTeam = pPlot->getTeam();

		if(eMyTeam != ePlotTeam && !GET_TEAM(eMyTeam).isAtWar(ePlotTeam) && !GET_TEAM(ePlotTeam).IsAllowsOpenBordersToTeam(eMyTeam))
		{
			bImpassableTerritory = true;
		}
		else if(pPlot->isCity())
		{
			if(pPlot->getOwner() == m_pPlayer->GetID())
			{
				cell.SetFriendlyCity(true);
			}
			else if(GET_TEAM(eMyTeam).isAtWar(ePlotTeam))
			{
				cell.SetEnemyCity(true);
			}
			else
			{
				cell.SetNeutralCity(true);
			}
		}

		if(m_pPlayer->GetID() == pPlot->getOwner())
		{
			cell.SetOwnTerritory(true);
		}
		else if(GET_TEAM(eMyTeam).isFriendlyTerritory(ePlotTeam))
		{
			cell.SetFriendlyTerritory(true);
		}
		else if(GET_TEAM(ePlotTeam).isAtWar(eMyTeam))
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

	if(pPlot->getNumUnits() > 0)
	{
		for(iUnitLoop = 0; iUnitLoop < pPlot->getNumUnits(); iUnitLoop++)
		{
			pLoopUnit = pPlot->getUnitByIndex(iUnitLoop);
			if(!pLoopUnit) continue;
			if(pLoopUnit->getOwner() == m_pPlayer->GetID())
			{
				if(pLoopUnit->IsCombatUnit())
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
			else if(pLoopUnit->isEnemy(m_pPlayer->getTeam()))
			{
				if(pLoopUnit->IsCombatUnit())
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
				if(pLoopUnit->IsCombatUnit())
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
	if(cell.IsImpassableTerrain() || cell.IsImpassableTerritory() || !cell.IsRevealed())
	{
		bAdd = false;
	}
	return bAdd;
}

/// Add data for this cell into dominance zone information
void CvTacticalAnalysisMap::AddToDominanceZones(int iIndex, CvTacticalAnalysisCell* pCell)
{
	CvPlot* pPlot = GC.getMap().plotByIndex(iIndex);

	// Compute zone data for this cell
	m_TempZone.SetAreaID(pPlot->getArea());
	m_TempZone.SetWater(pPlot->isWater());

	int iCityDistance = GC.getGame().GetClosestCityDistance(pPlot);
	CvCity* pCity = GC.getGame().GetClosestCity(pPlot);
	PlayerTypes eOwnerPlayer = NO_PLAYER;
	TeamTypes eOwnerTeam = NO_TEAM;

	//for plots far away from a city, check the owner
	if (iCityDistance>3)
	{
		eOwnerTeam = pPlot->getTeam();
		eOwnerPlayer = pPlot->getOwner();
	}
	else //look at the city
	{
		eOwnerTeam = pCity->getTeam();
		eOwnerPlayer = pCity->getOwner();
	}

	m_TempZone.SetOwner(eOwnerPlayer);
	m_TempZone.SetZoneCity(pCity);
	if(eOwnerTeam==NO_TEAM)
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_NO_OWNER);
	}
	else if(eOwnerTeam == m_pPlayer->getTeam())
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_FRIENDLY);
	}
	else if(GET_TEAM(m_pPlayer->getTeam()).isAtWar(eOwnerTeam))
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_ENEMY);
	}
	else
	{
		m_TempZone.SetTerritoryType(TACTICAL_TERRITORY_NEUTRAL);
	}

	// Now see if we already have a matching zone
	CvTacticalDominanceZone* pZone = FindExistingZone(pPlot);
	if(!pZone)
	{
		// Data populated, now add to vector
		m_TempZone.SetDominanceZoneID(m_DominanceZones.size());
		m_DominanceZones.push_back(m_TempZone);
		pZone = &m_DominanceZones[m_DominanceZones.size() - 1];
	}

	// Set zone for this cell
	pCell->SetDominanceZone(pZone->GetDominanceZoneID());
#if defined(MOD_BALANCE_CORE_MILITARY)
	pZone->Extend(pPlot);
#endif
}

/// Calculate military presences in each owned dominance zone
void CvTacticalAnalysisMap::CalculateMilitaryStrengths()
{
	TeamTypes eTeam = m_pPlayer->getTeam();

	// Loop through the dominance zones
	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		CvTacticalDominanceZone* pZone = &m_DominanceZones[iI];
		CvCity *pClosestCity = pZone->GetZoneCity();
		if(pClosestCity)
		{
			// Start with strength of the city itself
			int iCityHitPoints = pClosestCity->GetMaxHitPoints() - pClosestCity->getDamage();
			int iStrength = m_iTacticalRange * pClosestCity->getStrengthValue() * iCityHitPoints / GC.getMAX_CITY_HIT_POINTS();

			if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
			{
				pZone->AddFriendlyStrength(iStrength);
				pZone->AddFriendlyRangedStrength(pClosestCity->getStrengthValue(true));
			}
			else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
			{
				pZone->AddEnemyStrength(iStrength);
				pZone->AddEnemyRangedStrength(pClosestCity->getStrengthValue(true));
			}
			else
			{
				pZone->AddNeutralStrength(iStrength);
			}
		}

		// check all units in the world
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);
			bool bEnemy = GET_TEAM(eTeam).isAtWar(kPlayer.getTeam());
			bool bFriendly = (eTeam==kPlayer.getTeam());

			int iLoop;
			for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				if(!pLoopUnit->IsCombatUnit())
					continue;

				bool bUnitMayBeRelevant = (pLoopUnit->getDomainType() == DOMAIN_AIR ||
						pLoopUnit->isRanged() || //ranged power is cross-domain!
						(pLoopUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) ||
						(pLoopUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater()));

				if (!bUnitMayBeRelevant)
					continue;

				CvPlot* pPlot = pLoopUnit->plot();
				if(!pPlot)
					continue;

				//a little cheating for AI - invisible units still count with reduced strength
				bool bVisible = pPlot->isVisible(eTeam) || pPlot->isAdjacentVisible(eTeam, false);

				//if there is not city, just assume a flat distance
				int iDistance = 0;
				if (pClosestCity)
				{
					iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pClosestCity->getX(), pClosestCity->getY());
					if (iDistance > m_iTacticalRange)
						continue;
				}

				int iMultiplier = m_iTacticalRange + MIN(3 - iDistance, 0);  // 3 because action may still be spread out over the zone
				if(iMultiplier > 0)
				{
					int iUnitStrength = pLoopUnit->GetBaseCombatStrengthConsideringDamage();

					//unit might disembark ... so don't count it for water zone, but for adjacent land
					if(iUnitStrength == 0 && pLoopUnit->isEmbarked() && !pZone->IsWater())
						iUnitStrength = pLoopUnit->GetBaseCombatStrength();

					int iRangedStrength = pLoopUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, true) / 100;

					if(!bVisible)
					{
						iUnitStrength /= 2;
						iRangedStrength /= 2;
					}

					if (bEnemy)
					{
						pZone->AddEnemyStrength(iUnitStrength*iMultiplier*m_iUnitStrengthMultiplier);
						pZone->AddEnemyRangedStrength(iRangedStrength*iMultiplier*m_iUnitStrengthMultiplier);

						pZone->AddEnemyUnitCount(1);

						if(pLoopUnit->isRanged())
							pZone->AddEnemyRangedUnitCount(1);
						else
							pZone->AddEnemyMeleeUnitCount(1);

						//this only applies for enemy zones? why?
						if(pLoopUnit->getDomainType() == DOMAIN_SEA)
							pZone->AddEnemyNavalUnitCount(1);

						//again only for enemies
						if(pZone->GetRangeClosestEnemyUnit()<0 || iDistance<pZone->GetRangeClosestEnemyUnit())
							pZone->SetRangeClosestEnemyUnit(iDistance);

						//CvString msg;
						//msg.Format("Zone %d, Enemy %s %d at %d,%d - distance %d, strength %d, ranged strength %d",
						//	pZone->GetDominanceZoneID(), pLoopUnit->getName().c_str(), pLoopUnit->GetID(), 
						//	pLoopUnit->getX(), pLoopUnit->getY(),	iDistance, iUnitStrength, iRangedStrength);
						//m_pPlayer->GetTacticalAI()->LogTacticalMessage(msg, true /*bSkipLogDominanceZone*/);

					}
					else if (bFriendly)
					{
						pZone->AddFriendlyStrength(iUnitStrength*iMultiplier*m_iUnitStrengthMultiplier);
						pZone->AddFriendlyRangedStrength(iRangedStrength*iMultiplier*m_iUnitStrengthMultiplier);

						pZone->AddFriendlyUnitCount(1);

						if(pLoopUnit->isRanged())
							pZone->AddFriendlyRangedUnitCount(1);
						else
							pZone->AddFriendlyMeleeUnitCount(1);

						//CvString msg;
						//msg.Format("Zone %d, Friendly %s %d at %d,%d - distance %d, strength %d, ranged strength %d",
						//	pZone->GetDominanceZoneID(), pLoopUnit->getName().c_str(), pLoopUnit->GetID(), 
						//	pLoopUnit->getX(), pLoopUnit->getY(),	iDistance, iUnitStrength, iRangedStrength);
						//m_pPlayer->GetTacticalAI()->LogTacticalMessage(msg, true /*bSkipLogDominanceZone*/);
					}
					else
					{
						//neutral has only very few stats
						pZone->AddNeutralStrength(iUnitStrength*iMultiplier*m_iUnitStrengthMultiplier);
						pZone->AddNeutralUnitCount(1);
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
	CvTacticalDominanceZone* pZone;
	int iBaseValue;
	int iMultiplier;
	CvCity* pClosestCity = NULL;

	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		// Find the zone and compute dominance here
		pZone = &m_DominanceZones[iI];
		eTacticalDominanceFlags eDominance = ComputeDominance(pZone);

		// Establish a base value for the region
		iBaseValue = 1;

		// Temporary zone?
		if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
		{
			iMultiplier = 1000;
		}
		else
		{
			pClosestCity = pZone->GetZoneCity();

			if(pClosestCity)
			{
				iBaseValue += (1 + (int)sqrt((float)pClosestCity->getPopulation()));

				if(pClosestCity->isCapital() && !pClosestCity->GetPlayer()->isMinorCiv())
				{
					iBaseValue *= 2;
				}

				if(m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pClosestCity))
				{
					iBaseValue *= 20;
				}

				else if (pClosestCity->isVisible(m_pPlayer->getTeam(), false))
				{
					iBaseValue *= 4;

					// How damaged is this visible city?
					int iMaxDamageMultiplier = 10;
					int iDamage = pClosestCity->getDamage();
					if (iDamage > (pClosestCity->GetMaxHitPoints() / iMaxDamageMultiplier))
					{
						iBaseValue *= (int)((iDamage + 1) * 10 / pClosestCity->GetMaxHitPoints());
					}
				}

#if defined(MOD_BALANCE_CORE)
				if (m_pPlayer->IsCityAlreadyTargeted(pClosestCity) || m_pPlayer->GetMilitaryAI()->IsCurrentAttackTarget(pClosestCity) )
				{
					iBaseValue *= 10;
				}
#endif
			}

			if(!pZone->IsWater())
			{
				iBaseValue *= 3;
			}

			// Now compute a multiplier based on current conditions here
			iMultiplier = 1;
			if(eDominance == TACTICAL_DOMINANCE_ENEMY)
			{
				if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
#if defined(MOD_BALANCE_CORE_MILITARY)
					iMultiplier = 1;
#else
					iMultiplier = 2;
#endif
				}
				else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
#if defined(MOD_BALANCE_CORE_MILITARY)
					iMultiplier = 8;
#else
					iMultiplier = 6;
#endif
				}
			}
			else if(eDominance == TACTICAL_DOMINANCE_EVEN)
			{
				if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier = 4;
				}
				else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier = 4;
				}
			}
			else if(eDominance == TACTICAL_DOMINANCE_FRIENDLY)
			{
				if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier = 8;
				}
				else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier = 1;
				}
			}
			if(!m_pPlayer->isMinorCiv())
			{
				if(m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
				{
					if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
					{
#if defined(MOD_BALANCE_CORE_MILITARY)
						iMultiplier *= 4;
#else
						iMultiplier *= 2;
#endif
					}
				}
				else if(m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
				{
					if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
					{
						iMultiplier *= 4;
					}
				}
			}
		}

		// Save off the value for this zone
		if((iBaseValue * iMultiplier) <= 0)
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
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString szLogMsg;
		CvTacticalDominanceZone* pZone;

		for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
		{
			pZone = &m_DominanceZones[iI];

			//don't blow up the logs for empty zones
			if ( pZone->GetFriendlyStrength()==0 &&  pZone->GetEnemyStrength()==0)
				continue;

			szLogMsg.Format("Zone ID: %d, Size: %d, City: %s, Area ID: %d, Value: %d, FRIENDLY Str: %d (%d), Ranged: %d (%d), ENEMY Str: %d (%d), Ranged: %d (%d), Closest Enemy: %d",
			                pZone->GetDominanceZoneID(), pZone->GetNumPlots(), pZone->GetZoneCity() ? pZone->GetZoneCity()->getName().c_str() : "none", pZone->GetAreaID(), pZone->GetDominanceZoneValue(),
			                pZone->GetFriendlyStrength(), pZone->GetFriendlyUnitCount(), pZone->GetFriendlyRangedStrength(), pZone->GetFriendlyRangedUnitCount(),
			                pZone->GetEnemyStrength(), pZone->GetEnemyUnitCount(), pZone->GetEnemyRangedStrength(), pZone->GetEnemyRangedUnitCount(), pZone->GetRangeClosestEnemyUnit());
			if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
			{
				szLogMsg += ", Friendly";
			}
			else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
			{
				szLogMsg += ", Enemy";
			}
			else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_EVEN)
			{
				szLogMsg += ", Even";
			}
			else if(pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_NO_UNITS_VISIBLE)
			{
				szLogMsg += ", No Units Visible";
			}

			if(pZone->IsWater())
			{
				szLogMsg += ", Water";
			}
			else
			{
				szLogMsg += ", Land";
			}

			if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
			{
				szLogMsg += ", Temporary Zone";
			}
			else if(pZone->GetZoneCity())
			{
				if (m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pZone->GetZoneCity()))
				{
					szLogMsg += " (Temp)";
				}
			}

			m_pPlayer->GetTacticalAI()->LogTacticalMessage(szLogMsg, true /*bSkipLogDominanceZone*/);
		}
	}
}

/// Can this cell go in an existing dominance zone?
CvTacticalDominanceZone* CvTacticalAnalysisMap::FindExistingZone(CvPlot* pPlot)
{
	CvTacticalDominanceZone* pZone;

	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		pZone = &m_DominanceZones[iI];

		// If this is a temporary zone, matches if unowned and close enough
		if((pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE) &&
		        (m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL) &&
		        (plotDistance(pPlot->getX(), pPlot->getY(), pZone->GetTempZoneCenter()->getX(), pZone->GetTempZoneCenter()->getY()) <= m_iTacticalRange))
		{
			return pZone;
		}

		// If not friendly or enemy, just 1 zone per area
		if((pZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || pZone->GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL) &&
		        (m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || m_TempZone.GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL))
		{
			if(pZone->GetAreaID() == m_TempZone.GetAreaID())
			{
				return pZone;
			}
		}

		// Otherwise everything needs to match
		if(pZone->GetTerritoryType() == m_TempZone.GetTerritoryType() &&
		        pZone->GetOwner() == m_TempZone.GetOwner() &&
		        pZone->GetAreaID() == m_TempZone.GetAreaID() &&
		        pZone->GetZoneCity() == m_TempZone.GetZoneCity())
		{
			return pZone;
		}
	}

	return NULL;
}

/// Retrieve a dominance zone
CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZone(int iIndex)
{
	if(iIndex < 0 || iIndex >= (int)m_DominanceZones.size())
		return 0;
	return &m_DominanceZones[iIndex];
}

/// Retrieve a dominance zone by closest city
CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZoneByCity(CvCity* pCity, bool bWater)
{
	CvTacticalDominanceZone* pZone;
	for(int iI = 0; iI < GetNumZones(); iI++)
	{
		pZone = GetZone(iI);
		if(pZone->GetZoneCity() == pCity && pZone->IsWater() == bWater)
		{
			return pZone;
		}
	}

	return NULL;
}

#if defined(MOD_BALANCE_CORE)
/// Retrieve a dominance zone by ID
CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZoneByID(int iID)
{
	CvTacticalDominanceZone* pZone;
	for(int iI = 0; iI < GetNumZones(); iI++)
	{
		pZone = GetZone(iI);
		if(pZone->GetDominanceZoneID()==iID)
		{
			return pZone;
		}
	}

	return NULL;
}
#endif

// Is this plot in dangerous territory?
bool CvTacticalAnalysisMap::IsInEnemyDominatedZone(CvPlot* pPlot)
{
	int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
	CvTacticalAnalysisCell* pCell = GetCell(iPlotIndex);
	CvTacticalDominanceZone* pZone = GetZoneByID(pCell->GetDominanceZone());

	if(pZone)
		return (pZone->GetDominanceFlag() == TACTICAL_DOMINANCE_ENEMY);

	return false;
}

/// Who is dominant in this one zone?
eTacticalDominanceFlags CvTacticalAnalysisMap::ComputeDominance(CvTacticalDominanceZone* pZone)
{
	// Look at ratio of friendly to enemy strength
#if defined(MOD_BALANCE_CORE_MILITARY)
	if (!pZone)
		return TACTICAL_DOMINANCE_NO_UNITS_VISIBLE;

	if(pZone->GetEnemyRangedStrength()+pZone->GetEnemyStrength()+pZone->GetFriendlyRangedStrength()+pZone->GetFriendlyStrength()<=0)
	{
		pZone->SetDominanceFlag(TACTICAL_DOMINANCE_NO_UNITS_VISIBLE);
	}
	else
	{
		// Otherwise compute it by strength
		if(pZone->GetEnemyStrength()+pZone->GetEnemyRangedStrength()<=0)
		{
			pZone->SetDominanceFlag(TACTICAL_DOMINANCE_FRIENDLY);
		}
		else
		{
			int iRatio = ((pZone->GetFriendlyStrength()+pZone->GetFriendlyRangedStrength())*100) / (pZone->GetEnemyStrength()+pZone->GetEnemyRangedStrength());
#else
	if(pZone->GetTerritoryType() != TACTICAL_TERRITORY_ENEMY && pZone->GetEnemyUnitCount() <= 0)
	{
		pZone->SetDominanceFlag(TACTICAL_DOMINANCE_NO_UNITS_VISIBLE);
	}
	else
	{
		// Otherwise compute it by strength
		if(pZone->GetEnemyStrength() <= 0)
		{
			pZone->SetDominanceFlag(TACTICAL_DOMINANCE_FRIENDLY);
		}
		else
		{
			int iRatio = (pZone->GetFriendlyStrength()  * 100) / pZone->GetEnemyStrength();
#endif
			if(iRatio > 100 + m_iDominancePercentage)
			{
				pZone->SetDominanceFlag(TACTICAL_DOMINANCE_FRIENDLY);
			}
			else if(iRatio < 100 - m_iDominancePercentage)
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

#if defined(MOD_BALANCE_CORE_MILITARY_LOGGING)
void CvTacticalAnalysisMap::Dump()
{
	if (m_pPlayer==NULL)
		return;

	bool bLogging = GC.getLogging() && GC.getAILogging() && (m_pPlayer->isMajorCiv() || m_pPlayer->isBarbarian()) && m_pPlayer->IsAtWar();
	if (bLogging)
	{
		CvString fname = CvString::format( "TacticalCells_%s_%03d.txt", m_pPlayer->getCivilizationAdjective(), GC.getGame().getGameTurn() );
		FILogFile* pLog=LOGFILEMGR.GetLog( fname.c_str(), FILogFile::kDontTimeStamp );
		if (pLog)
		{
			pLog->Msg( "#x,y,visible,terrain,owner,enemy,defensemod,targettype,underattack,zoneid,dominance,zonetype,fstrength,estrength,city\n" );
			for (int i=0; i<m_iNumPlots; i++)
			{
				CvTacticalAnalysisCell* pCell = GetCell(i);

				if (!pCell->IsRevealed())
					continue;

				CvTacticalDominanceZone* pZone = GetZoneByID( pCell->GetDominanceZone() );

				int iZoneFriendlyStrength = pZone ? pZone->GetFriendlyRangedStrength() + pZone->GetFriendlyStrength() : -1;
				int iZoneEnemyStrength = pZone ? pZone->GetEnemyRangedStrength() + pZone->GetEnemyStrength() : -1;
				CvCity* pCity = pZone ? pZone->GetZoneCity() : NULL;

				CvString dump = CvString::format( "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n", 
					GC.getMap().plotByIndex(i)->getX(), GC.getMap().plotByIndex(i)->getY(),
					pCell->IsVisible(), (int)GC.getMap().plotByIndex(i)->getTerrainType(), (int)GC.getMap().plotByIndex(i)->getOwner(), (int)pCell->IsEnemyTerritory(),
					pCell->GetDefenseModifier(), (int)pCell->GetTargetType(), (int)pCell->IsSubjectToAttack(), 
					pZone ? pZone->GetDominanceZoneID() : -1, pZone ? pZone->GetDominanceFlag() : -1, pZone ? pZone->GetTerritoryType() : -1, 
					iZoneFriendlyStrength, iZoneEnemyStrength, pCity ? pCity->getName().c_str() : "no city" );
				pLog->Msg( dump.c_str() );
			}
			pLog->Close();
		}
	}
}
#endif
