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
	m_iDeploymentScore(0),
	m_eTargetType(AI_TACTICAL_TARGET_NONE),
	m_iZoneID(-1),
	m_iTargetDistance(INT_MAX),
	m_bHasLOSToTarget(false)
{
	Clear();
}

/// Reinitialize data
void CvTacticalAnalysisCell::Clear()
{
	ClearFlags();
	m_iDeploymentScore = 0;
	m_eTargetType = AI_TACTICAL_TARGET_NONE;
	m_iZoneID = -1;
	m_iTargetDistance = INT_MAX;
	m_bHasLOSToTarget = false;
}

bool CvTacticalAnalysisCell::CanUseForOperationGathering()
{
	if(IsImpassableTerrain() || IsImpassableTerritory() || IsEnemyCombatUnit() || IsNeutralCombatUnit() || IsFriendlyTurnEndTile() || IsEnemyCity() || IsNeutralCity())
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
	m_iZoneID = -1;
	m_eTerritoryType = TACTICAL_TERRITORY_NONE;
	m_eOverallDominanceFlag = TACTICAL_DOMINANCE_NO_UNITS_VISIBLE;
	m_eOwner = NO_PLAYER;
	m_iCityID = -1;
	m_iAreaID = 0;
	m_iFriendlyMeleeStrength = 0;
	m_iEnemyMeleeStrength = 0;
	m_iFriendlyRangedStrength = 0;
	m_iEnemyRangedStrength = 0;
	m_iFriendlyNavalStrength = 0;
	m_iEnemyNavalStrength = 0;
	m_iFriendlyNavalRangedStrength = 0;
	m_iEnemyNavalRangedStrength = 0;
	m_iFriendlyUnitCount = 0;
	m_iEnemyUnitCount = 0;
	m_iNeutralUnitStrength = 0;
	m_iNeutralUnitCount = 0;
	m_iEnemyNavalUnitCount = 0;
	m_iFriendlyNavalUnitCount = 0;
	m_iZoneValue = 0;
	m_iRangeClosestEnemyUnit = -1;
	m_bIsWater = false;
	m_bIsNavalInvasion = false;

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

eTacticalDominanceFlags CvTacticalDominanceZone::GetRangedDominanceFlag(int iDominancePercentage) const
{
	if ( GetEnemyRangedStrength() <= 0 && GetFriendlyRangedStrength() > 0)
	{
		return TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		int iRatio = (GetFriendlyRangedStrength() * 100) / max(1, GetEnemyRangedStrength());
		if (iRatio > 100 + iDominancePercentage)
		{
			return TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if (iRatio < 100 - iDominancePercentage)
		{
			return TACTICAL_DOMINANCE_ENEMY;
		}
	}

	return TACTICAL_DOMINANCE_EVEN;
}

eTacticalDominanceFlags CvTacticalDominanceZone::GetUnitCountDominanceFlag(int iDominancePercentage) const
{
	if (GetTotalEnemyUnitCount() <= 0 && GetTotalFriendlyUnitCount() > 0)
	{
		return TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		int iRatio = (GetTotalFriendlyUnitCount() * 100) / max(1, GetTotalEnemyUnitCount());
		if (iRatio > 100 + iDominancePercentage)
		{
			return TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if (iRatio < 100 - iDominancePercentage)
		{
			return TACTICAL_DOMINANCE_ENEMY;
		}
	}

	return TACTICAL_DOMINANCE_EVEN;
}

eTacticalDominanceFlags CvTacticalDominanceZone::GetNavalRangedDominanceFlag(int iDominancePercentage) const
{
	if (GetEnemyNavalRangedStrength() <= 0 && GetFriendlyNavalRangedStrength() > 0)
	{
		return TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		int iRatio = (GetFriendlyNavalRangedStrength() * 100) / max(1, GetEnemyNavalRangedStrength());
		if (iRatio > 100 + iDominancePercentage)
		{
			return TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if (iRatio < 100 - iDominancePercentage)
		{
			return TACTICAL_DOMINANCE_ENEMY;
		}
	}

	return TACTICAL_DOMINANCE_EVEN;
}

eTacticalDominanceFlags CvTacticalDominanceZone::GetNavalUnitCountDominanceFlag(int iDominancePercentage) const
{
	if (GetEnemyNavalUnitCount() <= 0 && GetFriendlyNavalUnitCount() > 0)
	{
		return TACTICAL_DOMINANCE_FRIENDLY;
	}
	else
	{
		int iRatio = (GetFriendlyNavalUnitCount() * 100) / max(1, GetEnemyNavalUnitCount());
		if (iRatio > 100 + iDominancePercentage)
		{
			return TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if (iRatio < 100 - iDominancePercentage)
		{
			return TACTICAL_DOMINANCE_ENEMY;
		}
	}

	return TACTICAL_DOMINANCE_EVEN;
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
		if(m_eOverallDominanceFlag == TACTICAL_DOMINANCE_FRIENDLY)
		{
			return AI_TACTICAL_MOVE_ZONE_FRIENDLY_WINNING;
		}
		else if(m_eOverallDominanceFlag == TACTICAL_DOMINANCE_EVEN)
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
		if(m_eOverallDominanceFlag == TACTICAL_DOMINANCE_FRIENDLY)
		{
			return AI_TACTICAL_MOVE_ZONE_ENEMY_WINNING;
		}
		else if(m_eOverallDominanceFlag == TACTICAL_DOMINANCE_EVEN)
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
	m_iDominancePercentage(25),
	m_iUnitStrengthMultiplier(5),
	m_iTacticalRange(6),
	m_ePlayer(NO_PLAYER),
	m_iTurnBuilt(-1)
{
}

/// Destructor
CvTacticalAnalysisMap::~CvTacticalAnalysisMap(void)
{
}

/// Initialize
void CvTacticalAnalysisMap::Init(PlayerTypes ePlayer)
{
	// Time building of these maps
	AI_PERF("AI-perf-tact.csv", "CvTacticalAnalysisMap::Init()" );

	m_ePlayer = ePlayer;
	m_vCells.clear();
	m_vCells.resize( GC.getMap().numPlots() );
	m_DominanceZones.clear();
	m_iDominancePercentage = GC.getAI_TACTICAL_MAP_DOMINANCE_PERCENTAGE();

	m_iTurnBuilt = -1;
	m_iTacticalRange = 6;
	m_iUnitStrengthMultiplier = 5;

}

int CvTacticalDominanceZone::GetBorderScore() const
{
	int iCount = 0;
	for (size_t i = 0; i < m_vNeighboringZones.size(); i++)
	{
		CvTacticalDominanceZone* pNeighbor = GET_PLAYER(m_eOwner).GetTacticalAI()->GetTacticalAnalysisMap()->GetZoneByID(m_vNeighboringZones[i]);
		if (!pNeighbor)
			continue;
		
		if (pNeighbor->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
			iCount+=3;

		if (pNeighbor->GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL)
			iCount+=2;

		if (pNeighbor->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER)
			iCount+=1;
	}
	
	return iCount;
}

void CvTacticalDominanceZone::AddNeighboringZone(int iZoneID)
{ 
	if (iZoneID==m_iZoneID || iZoneID==-1)
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

	for(size_t iI = 0; iI < m_DominanceZones.size(); iI++)
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

bool CvTacticalAnalysisMap::IsUpToDate()
{
	return (m_iTurnBuilt == GC.getGame().getGameTurn() && m_vCells.size()==GC.getMap().numPlots());
}

/// Fill the map with data for this AI player's turn
void CvTacticalAnalysisMap::Refresh()
{
	if(!IsUpToDate())
	{
		//can happen in the first turn ...
		if (m_vCells.size()!=GC.getMap().numPlots())
			Init(m_ePlayer);

		m_iTurnBuilt = GC.getGame().getGameTurn();
		m_iTacticalRange = ((GC.getAI_TACTICAL_RECRUIT_RANGE() + GC.getGame().getCurrentEra()) * 3) / 5;  // Have this increase as game goes on
		m_iUnitStrengthMultiplier = GC.getAI_TACTICAL_MAP_UNIT_STRENGTH_MULTIPLIER() * m_iTacticalRange;

		AI_PERF_FORMAT("AI-perf.csv", ("Tactical Analysis Map, Turn %d, %s", GC.getGame().getGameTurn(), GET_PLAYER(m_ePlayer).getCivilizationShortDescription()) );

		m_DominanceZones.clear();
		AddTemporaryZones();

		for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
//			CvAssertMsg((iI < m_iNumPlots), "Plot to be accessed exceeds allocation!");

			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);
			if(pPlot == NULL)
			{
				// Erase this cell
				m_vCells[iI].Clear();
			}
			else
			{
				if(PopulateCell(iI, pPlot))
				{
					AddToDominanceZones(iI, &m_vCells[iI]);
				}
			}
		}

		//do this before anything else
		UpdateZoneIds();

		//barbarians don't care about tactical dominance
		if(m_ePlayer!=BARBARIAN_PLAYER)
		{
			EstablishZoneNeighborhood();
			CalculateMilitaryStrengths();
			PrioritizeZones();
			LogZones();
		}

		BuildEnemyUnitList();
		MarkCellsNearEnemy();
	}
}

//make it so that the zone ids correspond to the city ids (easier debugging)
void CvTacticalAnalysisMap::UpdateZoneIds()
{
	map<int, int> old2new;

	for (size_t i = 0; i < m_DominanceZones.size(); i++)
	{
		CvCity* pZoneCity = m_DominanceZones[i].GetZoneCity();
		int iOld = m_DominanceZones[i].GetZoneID();
		int iNew = m_DominanceZones[i].GetZoneID();
		if (pZoneCity)
		{
			if (m_DominanceZones[i].IsWater())
				m_DominanceZones[i].SetZoneID(-pZoneCity->GetID());
			else
				m_DominanceZones[i].SetZoneID(+pZoneCity->GetID());

			iNew = m_DominanceZones[i].GetZoneID();
		}
		old2new[iOld] = iNew;
	}

	for (size_t i = 0; i < m_vCells.size(); i++)
		if (m_vCells[i].GetDominanceZone()!=-1)
			m_vCells[i].SetDominanceZone( old2new[m_vCells[i].GetDominanceZone()] );

	m_IdLookup.clear();
	for (size_t i = 0; i < m_DominanceZones.size(); i++)
		m_IdLookup[m_DominanceZones[i].GetZoneID()] = i;
}

// Find all our enemies (combat units)
void CvTacticalAnalysisMap::BuildEnemyUnitList()
{
	m_EnemyUnits.clear();
	m_EnemyCities.clear();

	TeamTypes ourTeam = GET_PLAYER(m_ePlayer).getTeam();
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		const PlayerTypes ePlayer = (PlayerTypes)iPlayer;
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		const TeamTypes eTeam = kPlayer.getTeam();

		// for each opposing civ
		if(kPlayer.isAlive() && GET_TEAM(eTeam).isAtWar(ourTeam))
		{
			int iLoop;
			CvUnit* pLoopUnit = NULL;
			CvCity* pLoopCity;

			for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				if (pLoopCity->plot()->isRevealed(ourTeam))
					m_EnemyCities.push_back(pLoopCity->GetIDInfo());

			for(pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
				if(pLoopUnit->IsCanAttack() && pLoopUnit->plot()->isVisible(ourTeam))
					m_EnemyUnits.push_back(pLoopUnit->GetIDInfo());
		}
	}
}

// Indicate the plots we might want to move to that the enemy can attack
void CvTacticalAnalysisMap::MarkCellsNearEnemy()
{
	PromotionTypes eDamagePromotion = NO_PROMOTION;
	for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
	{
		const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iJ);
		CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
		if (pkPromotionInfo != NULL)
		{
			if (pkPromotionInfo->GetNearbyEnemyDamage() > 0)
			{
				eDamagePromotion = eLoopPromotion;
				break;
			}
		}
	}

	for(unsigned int iUnitIndex = 0;  iUnitIndex < m_EnemyUnits.size(); iUnitIndex++)
	{
		CvUnit* pUnit = getUnit(m_EnemyUnits[iUnitIndex]);
		
		//for ranged every plot we can enter with movement left is a base for attack
		int iMinMovesLeft = pUnit->IsCanAttackRanged() ? 1 : 0;

		//be a bit conservative here, use ZOC - if one of our units is killed, this is not correct anymore
		//therefore we later do a dilation filter on the cells
		ReachablePlots tiles = pUnit->GetAllPlotsInReachThisTurn(false,true,false,iMinMovesLeft);

		for (ReachablePlots::iterator moveTile=tiles.begin(); moveTile!=tiles.end(); ++moveTile)
		{
			CvPlot* pMoveTile = GC.getMap().plotByIndexUnchecked(moveTile->iPlotIndex);
			int iPlotIndex = GC.getMap().plotNum(pMoveTile->getX(),pMoveTile->getY());

			if (pUnit->IsCanAttackRanged())
			{
				std::set<int> rangedPlots;
				//this generates some overlap, but preventing that is about as bad as ignoring it
				TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pUnit,pMoveTile,rangedPlots,false,false);
				for (std::set<int>::iterator attackTile=rangedPlots.begin(); attackTile!=rangedPlots.end(); ++attackTile)
				{
					m_vCells[*attackTile].SetSubjectToAttack(true);
				}
			}
			else
			{
				//for melee every tile he can move into can be attacked
				m_vCells[iPlotIndex].SetSubjectToAttack(true);
			}
		}
	}

	//do the dilation
	std::vector<int> vCellsToMark;
	for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		//nothing to do
		if (m_vCells[iPlotLoop].IsSubjectToAttack())
			continue;

		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		//danger plots are calculated without ZOC
		if (GET_PLAYER(m_ePlayer).GetPossibleAttackers(*pPlot).size()>0)
		{
			//check whether neighbors are subject to attack with ZOC
			CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pPlot);
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				CvPlot* pAdjacentPlot = aPlotsToCheck[iI];
				if (pAdjacentPlot)
				{
					if (m_vCells[pAdjacentPlot->GetPlotIndex()].IsSubjectToAttack())
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
		m_vCells[vCellsToMark[iI]].SetSubjectToAttack(true);

	// Look at every cell on the map
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);

		//important
		m_vCells[iI].SetVisibleToEnemy(pPlot->isVisibleToEnemy(m_ePlayer));

		if(m_vCells[iI].IsRevealed() && !m_vCells[iI].IsImpassableTerrain() && !m_vCells[iI].IsImpassableTerritory())
		{
			// Check adjacent plots for enemy citadels
			if (pPlot->IsNearEnemyCitadel(m_ePlayer, 0, eDamagePromotion))
					m_vCells[iI].SetSubjectToAttack(true);

			for(unsigned int iCityIndex = 0;  iCityIndex < m_EnemyCities.size(); iCityIndex++)
			{
				CvCity* pCity = getCity( m_EnemyCities[iCityIndex] );
				if (pCity->canRangeStrikeAt( pPlot->getX(), pPlot->getY() ))
					m_vCells[iI].SetSubjectToAttack(true);
			}
		}

		//safe by definition
		if (m_vCells[iI].IsFriendlyCity() && !pPlot->getPlotCity()->isInDangerOfFalling())
			m_vCells[iI].SetSubjectToAttack(false);
	}
}

// Clear all dynamic data flags from the map
void CvTacticalAnalysisMap::ClearDynamicFlags()
{
	for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		// Erase this cell
		m_vCells[iI].SetSafeForDeployment(false);
		m_vCells[iI].SetDeploymentScore(0);
		m_vCells[iI].SetTargetDistance(INT_MAX);
		m_vCells[iI].SetHasLOS(false);
	}
}

// PRIVATE FUNCTIONS

/// Add in any temporary dominance zones from tactical AI
void CvTacticalAnalysisMap::AddTemporaryZones()
{
	CvTacticalAI* pTacticalAI = GET_PLAYER(m_ePlayer).GetTacticalAI();
	if(pTacticalAI)
	{
		pTacticalAI->DropObsoleteZones();

		CvTemporaryZone* pZone = pTacticalAI->GetFirstTemporaryZone();
		while(pZone)
		{
			// Can't be a city zone (which is just used to boost priority but not establish a new zone)
			if(pZone->GetTargetType() != AI_TACTICAL_TARGET_CITY)
			{
				CvPlot* pPlot = GC.getMap().plot(pZone->GetX(), pZone->GetY());
				if(pPlot)
				{
					CvTacticalDominanceZone newZone;
					newZone.SetTerritoryType(TACTICAL_TERRITORY_TEMP_ZONE);
					newZone.SetOwner(NO_PLAYER);
					newZone.SetAreaID(pPlot->getArea());
					newZone.SetWater(pPlot->isWater());
					newZone.Extend(pPlot);
					newZone.SetNavalInvasion(pZone->IsNavalInvasion());

					newZone.SetZoneID(m_DominanceZones.size());
					AddNewDominanceZone(newZone);
				}
			}

			pZone = pTacticalAI->GetNextTemporaryZone();
		}
	}
}

/// Update data for a cell: returns whether or not to add to dominance zones
bool CvTacticalAnalysisMap::PopulateCell(int iIndex, CvPlot* pPlot)
{
	CvTacticalAnalysisCell& cell = m_vCells[iIndex];
	TeamTypes ourTeam = GET_PLAYER(m_ePlayer).getTeam();

	cell.Clear();

	cell.SetRevealed(pPlot->isRevealed(ourTeam));
	cell.SetVisible(pPlot->isVisible(ourTeam));
	cell.SetImpassableTerrain(!pPlot->isValidMovePlot(m_ePlayer));
	cell.SetWater(pPlot->isWater());
	cell.SetOcean(pPlot->isWater() && !pPlot->isShallowWater());

	bool bImpassableTerritory = false;
	if(pPlot->isOwned())
	{
		TeamTypes ePlotTeam = pPlot->getTeam();

		if(ourTeam != ePlotTeam && !GET_TEAM(ourTeam).isAtWar(ePlotTeam) && !GET_TEAM(ePlotTeam).IsAllowsOpenBordersToTeam(ourTeam))
		{
			bImpassableTerritory = true;
		}
		else if(pPlot->isCity())
		{
			if(pPlot->getOwner() == m_ePlayer)
			{
				cell.SetFriendlyCity(true);
			}
			else if(GET_TEAM(ourTeam).isAtWar(ePlotTeam))
			{
				cell.SetEnemyCity(true);
			}
			else
			{
				cell.SetNeutralCity(true);
			}
		}

		if(m_ePlayer == pPlot->getOwner())
		{
			cell.SetOwnTerritory(true);
		}
		else if(GET_TEAM(ourTeam).isFriendlyTerritory(ePlotTeam))
		{
			cell.SetFriendlyTerritory(true);
		}
		else if(GET_TEAM(ePlotTeam).isAtWar(ourTeam))
		{
			cell.SetEnemyTerritory(true);
		}
	}
	else
	{
		cell.SetUnclaimedTerritory(true);
	}

	cell.SetImpassableTerritory(bImpassableTerritory);

	CvUnit* pDefender = pPlot->getBestDefender(NO_PLAYER,m_ePlayer);
	if (pDefender)
	{
		if (pDefender->isEnemy(ourTeam))
			cell.SetEnemyCombatUnit(true);
		else if (pDefender->getOwner()!=m_ePlayer)
			cell.SetNeutralCombatUnit(true);
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
	TeamTypes ourTeam = GET_PLAYER(m_ePlayer).getTeam();
	CvPlot* pPlot = GC.getMap().plotByIndex(iIndex);

	// Compute zone data for this cell
	CvTacticalDominanceZone newZone;
	newZone.SetAreaID(pPlot->getArea());
	newZone.SetWater(pPlot->isWater());

	int iCityDistance = GC.getGame().GetClosestCityDistanceInTurns(pPlot);
	CvCity* pCity = GC.getGame().GetClosestCityByEstimatedTurns(pPlot);
	PlayerTypes eOwnerPlayer = NO_PLAYER;
	TeamTypes eOwnerTeam = NO_TEAM;

	//for plots far away from a city, check the owner
	if (iCityDistance>2)
	{
		eOwnerTeam = pPlot->getTeam();
		eOwnerPlayer = pPlot->getOwner();

		//there is almost always a closest city, but we're not always interested
		if (pCity && eOwnerPlayer!=pCity->getOwner())
			pCity = NULL;
	}
	else if (pCity) //look at the city
	{
		eOwnerTeam = pCity->getTeam();
		eOwnerPlayer = pCity->getOwner();
	}

	newZone.SetOwner(eOwnerPlayer);
	newZone.SetZoneCity(pCity);
	newZone.Extend(pPlot);

	if(eOwnerTeam==NO_TEAM)
	{
		newZone.SetTerritoryType(TACTICAL_TERRITORY_NO_OWNER);
	}
	else if(eOwnerTeam == ourTeam)
	{
		newZone.SetTerritoryType(TACTICAL_TERRITORY_FRIENDLY);
	}
	else if(GET_TEAM(ourTeam).isAtWar(eOwnerTeam))
	{
		newZone.SetTerritoryType(TACTICAL_TERRITORY_ENEMY);
	}
	else
	{
		newZone.SetTerritoryType(TACTICAL_TERRITORY_NEUTRAL);
	}

	// Now see if we already have a matching zone
	CvTacticalDominanceZone* pZone = MergeWithExistingZone(&newZone);
	if (!pZone)
	{
		newZone.SetZoneID(m_DominanceZones.size());
		pZone = AddNewDominanceZone(newZone);
	}

	// Set zone for this cell
	pCell->SetDominanceZone(pZone->GetZoneID());
	pZone->Extend(pPlot);
}

CvTacticalDominanceZone* CvTacticalAnalysisMap::AddNewDominanceZone(CvTacticalDominanceZone& zone)
{
	m_IdLookup[zone.GetZoneID()] = m_DominanceZones.size();
	m_DominanceZones.push_back(zone);
	return &m_DominanceZones.back();
}

/// Calculate military presences in each owned dominance zone
void CvTacticalAnalysisMap::CalculateMilitaryStrengths()
{
	TeamTypes eTeam = GET_PLAYER(m_ePlayer).getTeam();

	// Loop through the dominance zones
	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		CvTacticalDominanceZone* pZone = &m_DominanceZones[iI];
		CvCity *pClosestCity = pZone->GetZoneCity();
		if(pClosestCity)
		{
			// Start with strength of the city itself
			int iCityHitPoints = pClosestCity->GetMaxHitPoints() - pClosestCity->getDamage();
			int iStrength = pClosestCity->getStrengthValue() * iCityHitPoints / GC.getMAX_CITY_HIT_POINTS();

			if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
			{
				pZone->AddFriendlyMeleeStrength(iStrength);
				pZone->AddFriendlyRangedStrength(pClosestCity->getStrengthValue(true));
			}
			else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
			{
				pZone->AddEnemyMeleeStrength(iStrength);
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
						((pLoopUnit->getDomainType() == DOMAIN_SEA || (pLoopUnit->isEmbarked() && pClosestCity) && pZone->IsWater())));
						//embarked melee still count in water zone if there's a city to attack/defend

				if (!bUnitMayBeRelevant)
					continue;

				CvPlot* pPlot = pLoopUnit->plot();
				if(!pPlot)
					continue;

				//a little cheating for AI - invisible units still count with reduced strength
				bool bVisible = pPlot->isVisible(eTeam) || pPlot->isAdjacentVisible(eTeam, false);
				bool bZoneTypeMismatch = (pLoopUnit->getDomainType() == DOMAIN_LAND && pZone->IsWater()) || (pLoopUnit->getDomainType() == DOMAIN_SEA && !pZone->IsWater());

				//embarked units and crossdomain count only partially
				bool bReducedStrength = pLoopUnit->isEmbarked() || bZoneTypeMismatch;

				//if there is a city, units in adjacent zones can also count
				int iDistance = 0;
				if (pClosestCity)
				{
					iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pClosestCity->getX(), pClosestCity->getY());
					if (iDistance > m_iTacticalRange)
						continue;
					else if (iDistance > (m_iTacticalRange / 2))
					{
						if (bZoneTypeMismatch)
							continue;
						else
							bReducedStrength = true;
					}
					else
					{
						//if on another continent, they can't easily take part in the fight
						if (!pClosestCity->isMatchingArea(pLoopUnit->plot()))
							bReducedStrength = true;
					}
				}
				else
				{
					//if there is no city, the unit must be in the zone itself
					if ( GetCell(pLoopUnit->plot()->GetPlotIndex())->GetDominanceZone() != pZone->GetZoneID() )
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

					if(!bVisible || bReducedStrength)
					{
						iUnitStrength /= 2;
						iRangedStrength /= 2;
					}

					if (bEnemy)
					{
#if defined(MOD_BALANCE_CORE_MILITARY_LOGGING)
						//CvString msg;
						//msg.Format("Zone %d, Enemy %s %d with %d hp at %d,%d - distance %d, strength %d, ranged strength %d (total %d)",
						//	pZone->GetZoneID(), pLoopUnit->getName().c_str(), pLoopUnit->GetID(), pLoopUnit->GetCurrHitPoints(),
						//	pLoopUnit->getX(), pLoopUnit->getY(),	iDistance, iUnitStrength, iRangedStrength, pZone->GetOverallEnemyStrength());
						//GET_PLAYER(m_ePlayer).GetTacticalAI()->LogTacticalMessage(msg, true /*bSkipLogDominanceZone*/);
#endif

						if (pLoopUnit->getDomainType() == DOMAIN_SEA)
						{
							pZone->AddEnemyNavalStrength(iUnitStrength*iMultiplier*m_iUnitStrengthMultiplier);
							pZone->AddEnemyNavalRangedStrength(iRangedStrength*iMultiplier*m_iUnitStrengthMultiplier);
							pZone->AddEnemyNavalUnitCount(1);
						}
						else
						{
							pZone->AddEnemyMeleeStrength(iUnitStrength*iMultiplier*m_iUnitStrengthMultiplier);
							pZone->AddEnemyRangedStrength(iRangedStrength*iMultiplier*m_iUnitStrengthMultiplier);
							pZone->AddEnemyUnitCount(1);
						}

						//again only for enemies
						if(pZone->GetRangeClosestEnemyUnit()<0 || iDistance<pZone->GetRangeClosestEnemyUnit())
							pZone->SetRangeClosestEnemyUnit(iDistance);
					}
					else if (bFriendly)
					{

#if defined(MOD_BALANCE_CORE_MILITARY_LOGGING)
						//CvString msg;
						//msg.Format("Zone %d, Friendly %s %d with %d hp at %d,%d - distance %d, strength %d, ranged strength %d (total %d)",
						//	pZone->GetZoneID(), pLoopUnit->getName().c_str(), pLoopUnit->GetID(), pLoopUnit->GetCurrHitPoints(),
						//	pLoopUnit->getX(), pLoopUnit->getY(), iDistance, iUnitStrength, iRangedStrength, pZone->GetOverallFriendlyStrength());
						//GET_PLAYER(m_ePlayer).GetTacticalAI()->LogTacticalMessage(msg, true /*bSkipLogDominanceZone*/);
#endif

						if (pLoopUnit->getDomainType() == DOMAIN_SEA)
						{
							pZone->AddFriendlyNavalStrength(iUnitStrength*iMultiplier*m_iUnitStrengthMultiplier);
							pZone->AddFriendlyNavalRangedStrength(iRangedStrength*iMultiplier*m_iUnitStrengthMultiplier);
							pZone->AddFriendlyNavalUnitCount(1);
						}
						else
						{
							pZone->AddFriendlyMeleeStrength(iUnitStrength*iMultiplier*m_iUnitStrengthMultiplier);
							pZone->AddFriendlyRangedStrength(iRangedStrength*iMultiplier*m_iUnitStrengthMultiplier);
							pZone->AddFriendlyUnitCount(1);
						}
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
	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		// Find the zone and compute dominance here
		CvTacticalDominanceZone* pZone = &m_DominanceZones[iI];
		eTacticalDominanceFlags eDominance = ComputeDominance(pZone);

		// Establish a base value for the region
		int iBaseValue = 1;
		int iMultiplier = 1;

		// Temporary zone?
		if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE)
		{
			iMultiplier = 10;
		}
		else
		{
			CvCity* pClosestCity = pZone->GetZoneCity();
			if(pClosestCity && pClosestCity->isAdjacentToArea(pZone->GetAreaID()))
			{
				iBaseValue += (1 + (int)sqrt((float)pClosestCity->getPopulation()));

				if(GET_PLAYER(m_ePlayer).GetTacticalAI()->IsTemporaryZoneCity(pClosestCity))
				{
					iBaseValue *= 2;
				}

				else if (pClosestCity->isVisible( GET_PLAYER(m_ePlayer).getTeam(), false))
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
				if (GET_PLAYER(m_ePlayer).IsCityAlreadyTargeted(pClosestCity) || GET_PLAYER(m_ePlayer).GetMilitaryAI()->IsCurrentAttackTarget(pClosestCity) )
				{
					iBaseValue *= 2;
				}

				if (pClosestCity->GetPlayer()->isMinorCiv())
				{
					//At war with ally of this minor? Greatly reduce priority.
					PlayerTypes eAlly = pClosestCity->GetPlayer()->GetMinorCivAI()->GetAlly();
					if (eAlly != NO_PLAYER && GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(eAlly).getTeam()))
					{
						iBaseValue = 1;
					}
				}
#endif
			}

			if(!pZone->IsWater())
			{
				iBaseValue *= 3;
			}

			// Now compute a multiplier based on current conditions here
			if(eDominance == TACTICAL_DOMINANCE_ENEMY)
			{
				if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier = 1;
				}
				else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier = 8;
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
			if(!GET_PLAYER(m_ePlayer).isMinorCiv())
			{
				if(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
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
				else if(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
				{
					if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
					{
						iMultiplier *= 4;
					}
				}
			}
		}

		// Save off the value for this zone
		pZone->SetDominanceZoneValue( iBaseValue * iMultiplier);
	}

	std::stable_sort(m_DominanceZones.begin(), m_DominanceZones.end());

	m_IdLookup.clear();
	for (size_t i = 0; i < m_DominanceZones.size(); i++)
		m_IdLookup[m_DominanceZones[i].GetZoneID()] = i;
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
			if ( pZone->GetOverallFriendlyStrength()==0 &&  pZone->GetOverallEnemyStrength()==0)
				continue;

			szLogMsg.Format("Zone ID: %d, %s, Size: %d, City: %s, Area ID: %d, Value: %d, FRIENDLY Str: %d (%d), Ranged: %d (naval %d), ENEMY Str: %d (%d), Ranged: %d (naval %d), Closest Enemy: %d",
			                pZone->GetZoneID(), pZone->IsWater() ? "Water" : "Land", pZone->GetNumPlots(), pZone->GetZoneCity() ? pZone->GetZoneCity()->getName().c_str() : "none", pZone->GetAreaID(), pZone->GetDominanceZoneValue(),
			                pZone->GetOverallFriendlyStrength(), pZone->GetTotalFriendlyUnitCount(), pZone->GetFriendlyRangedStrength(), pZone->GetFriendlyNavalRangedStrength(),
			                pZone->GetOverallEnemyStrength(), pZone->GetTotalEnemyUnitCount(), pZone->GetEnemyRangedStrength(), pZone->GetEnemyNavalRangedStrength(), pZone->GetRangeClosestEnemyUnit());
			if(pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
			{
				szLogMsg += ", Friendly";
			}
			else if(pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
			{
				szLogMsg += ", Enemy";
			}
			else if(pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_EVEN)
			{
				szLogMsg += ", Even";
			}
			else if(pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_NO_UNITS_VISIBLE)
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
				if (GET_PLAYER(m_ePlayer).GetTacticalAI()->IsTemporaryZoneCity(pZone->GetZoneCity()))
				{
					szLogMsg += " (Temp)";
				}
			}
			if (pZone->IsNavalInvasion())
			{
				szLogMsg += ", NAVAL INVASION";
			}

			GET_PLAYER(m_ePlayer).GetTacticalAI()->LogTacticalMessage(szLogMsg, true /*bSkipLogDominanceZone*/);
		}
	}
}

/// Can this cell go in an existing dominance zone?
CvTacticalDominanceZone* CvTacticalAnalysisMap::MergeWithExistingZone(CvTacticalDominanceZone* pNewZone)
{
	for(unsigned int iI = 0; iI < m_DominanceZones.size(); iI++)
	{
		CvTacticalDominanceZone* pZone = &m_DominanceZones[iI];

		// If this is a temporary zone, matches if unowned and close enough
		if((pZone->GetTerritoryType() == TACTICAL_TERRITORY_TEMP_ZONE) &&
		        (pNewZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || pNewZone->GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL) &&
		        (plotDistance(pNewZone->GetCenterX(), pNewZone->GetCenterY(), pZone->GetCenterX(), pZone->GetCenterY()) <= m_iTacticalRange))
		{
			return pZone;
		}

		// If not friendly or enemy, just 1 zone per area
		if((pZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || pZone->GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL) &&
		        (pNewZone->GetTerritoryType() == TACTICAL_TERRITORY_NO_OWNER || pNewZone->GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL))
		{
			if(pZone->GetAreaID() == pNewZone->GetAreaID())
			{
				return pZone;
			}
		}

		// Otherwise everything needs to match
		if(pZone->GetTerritoryType() == pNewZone->GetTerritoryType() &&
		        pZone->GetOwner() == pNewZone->GetOwner() &&
		        pZone->GetAreaID() == pNewZone->GetAreaID() &&
		        pZone->GetZoneCity() == pNewZone->GetZoneCity())
		{
			return pZone;
		}
	}

	return NULL;
}

/// Retrieve a dominance zone
CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZoneByIndex(int iIndex)
{
	if(iIndex < 0 || iIndex >= (int)m_DominanceZones.size())
		return NULL;
	return &m_DominanceZones[iIndex];
}

/// Retrieve a dominance zone by closest city
CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZoneByCity(CvCity* pCity, bool bWater)
{
	if (!pCity)
		return NULL;

	//water zones have negative ids
	return GetZoneByID(pCity->GetID()*(bWater ? -1 : +1));
}

#if defined(MOD_BALANCE_CORE)
/// Retrieve a dominance zone by ID
CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZoneByID(int iID)
{
	map<int, int>::iterator it = m_IdLookup.find(iID);
	if (it != m_IdLookup.end())
		return GetZoneByIndex(it->second);

	return NULL;
}

CvTacticalDominanceZone * CvTacticalAnalysisMap::GetZoneByPlot(CvPlot * pPlot)
{
	if (pPlot)
	{
		int iPlotIndex = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
		CvTacticalAnalysisCell* pCell = GetCell(iPlotIndex);
		CvTacticalDominanceZone* pZone = pCell ? GetZoneByID(pCell->GetDominanceZone()) : NULL;
		return pZone;
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

	if(pZone && pZone->GetZoneCity()) //city check is to skip the potentially very large ocean zone
		return (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY);

	return false;
}

/// Who is dominant in this one zone?
eTacticalDominanceFlags CvTacticalAnalysisMap::ComputeDominance(CvTacticalDominanceZone* pZone)
{
	// Look at ratio of friendly to enemy strength
	if (!pZone)
		return TACTICAL_DOMINANCE_NO_UNITS_VISIBLE;

	if(pZone->GetOverallEnemyStrength()+pZone->GetOverallFriendlyStrength()<=0)
	{
		pZone->SetOverallDominanceFlag(TACTICAL_DOMINANCE_NO_UNITS_VISIBLE);
	}
	else
	{
		// Otherwise compute it by strength
		if (pZone->GetOverallEnemyStrength() <= 0 && pZone->GetTotalFriendlyUnitCount() > 0) //make sure the denominator is valid later on
		{
			pZone->SetOverallDominanceFlag(TACTICAL_DOMINANCE_FRIENDLY);
		}
		else if (pZone->GetTotalEnemyUnitCount()==1 && pZone->GetTotalFriendlyUnitCount()<=1) //both are weak, one unit might tip the balance
		{
			pZone->SetOverallDominanceFlag(TACTICAL_DOMINANCE_EVEN);
		}
		else
		{
			if ((pZone->GetZoneCity() != NULL && pZone->GetZoneCity()->isCoastal()) || pZone->IsWater())
			{
				if (pZone->GetEnemyNavalUnitCount() < pZone->GetFriendlyNavalUnitCount())
				{
					pZone->SetNavalInvasion(true); //only true if we actually have naval units
				}
				else
				{
					pZone->SetNavalInvasion(false);
				}
			}

			int iRatio = (pZone->GetOverallFriendlyStrength() * 100) / max(1, pZone->GetOverallEnemyStrength());
			if (iRatio > 100 + m_iDominancePercentage)
			{
				pZone->SetOverallDominanceFlag(TACTICAL_DOMINANCE_FRIENDLY);
			}
			else if (iRatio < 100 - m_iDominancePercentage)
			{
				pZone->SetOverallDominanceFlag(TACTICAL_DOMINANCE_ENEMY);
			}
			else
			{
				pZone->SetOverallDominanceFlag(TACTICAL_DOMINANCE_EVEN);
			}
		}
	}

	return pZone->GetOverallDominanceFlag();
}

#if defined(MOD_BALANCE_CORE_MILITARY_LOGGING)
void CvTacticalAnalysisMap::Dump()
{
	if (m_ePlayer==NO_PLAYER)
		return;

	bool bLogging = !GC.getGame().isNetworkMultiPlayer() && GC.getLogging() && GC.getAILogging() && (GET_PLAYER(m_ePlayer).isMajorCiv() || GET_PLAYER(m_ePlayer).isBarbarian()) && GET_PLAYER(m_ePlayer).IsAtWar();
	if (bLogging)
	{
		CvString fname = CvString::format( "TacticalCells_%s_%03d.txt", GET_PLAYER(m_ePlayer).getCivilizationAdjective(), GC.getGame().getGameTurn() );
		FILogFile* pLog=LOGFILEMGR.GetLog( fname.c_str(), FILogFile::kDontTimeStamp );
		if (pLog)
		{
			pLog->Msg( "#x,y,visible,terrain,owner,enemy,targettype,underattack,zoneid,dominance,zonetype,fstrength,estrength,city\n" );
			for (int i=0; i<GC.getMap().numPlots(); i++)
			{
				CvTacticalAnalysisCell* pCell = GetCell(i);

				if (!pCell->IsRevealed())
					continue;

				CvTacticalDominanceZone* pZone = GetZoneByID( pCell->GetDominanceZone() );

				int iZoneFriendlyStrength = pZone ? pZone->GetFriendlyRangedStrength() + pZone->GetOverallFriendlyStrength() : -1;
				int iZoneEnemyStrength = pZone ? pZone->GetEnemyRangedStrength() + pZone->GetOverallEnemyStrength() : -1;
				CvCity* pCity = pZone ? pZone->GetZoneCity() : NULL;

				CvString dump = CvString::format( "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s\n", 
					GC.getMap().plotByIndex(i)->getX(), GC.getMap().plotByIndex(i)->getY(),
					pCell->IsVisible(), (int)GC.getMap().plotByIndex(i)->getTerrainType(), (int)GC.getMap().plotByIndex(i)->getOwner(), 
					(int)pCell->IsEnemyTerritory(), (int)pCell->GetTargetType(), (int)pCell->IsSubjectToAttack(), 
					pZone ? pZone->GetZoneID() : -1, pZone ? pZone->GetOverallDominanceFlag() : -1, pZone ? pZone->GetTerritoryType() : -1, 
					iZoneFriendlyStrength, iZoneEnemyStrength, pCity ? pCity->getName().c_str() : "no city" );
				pLog->Msg( dump.c_str() );
			}
			pLog->Close();
		}
	}
}
#endif


FDataStream& operator<<(FDataStream& saveTo, const CvTacticalAnalysisMap& readFrom)
{
	saveTo << readFrom.m_iDominancePercentage;
	saveTo << readFrom.m_iUnitStrengthMultiplier;
	saveTo << readFrom.m_iTacticalRange;
	saveTo << readFrom.m_ePlayer;
	saveTo << readFrom.m_iTurnBuilt;

	saveTo << readFrom.m_vCells.size();
	for (size_t i=0; i<readFrom.m_vCells.size(); i++)
		saveTo << readFrom.m_vCells[i];
	saveTo << readFrom.m_DominanceZones.size();
	for (size_t i=0; i<readFrom.m_DominanceZones.size(); i++)
		saveTo << readFrom.m_DominanceZones[i];
	saveTo << readFrom.m_EnemyUnits.size();
	for (size_t i=0; i<readFrom.m_EnemyUnits.size(); i++)
	{
		saveTo << readFrom.m_EnemyUnits[i].eOwner;
		saveTo << readFrom.m_EnemyUnits[i].iID;
	}
	saveTo << readFrom.m_EnemyCities.size();
	for (size_t i=0; i<readFrom.m_EnemyCities.size(); i++)
	{
		saveTo << readFrom.m_EnemyCities[i].eOwner;
		saveTo << readFrom.m_EnemyCities[i].iID;
	}

	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CvTacticalAnalysisMap& writeTo)
{
	loadFrom >> writeTo.m_iDominancePercentage;
	loadFrom >> writeTo.m_iUnitStrengthMultiplier;
	loadFrom >> writeTo.m_iTacticalRange;
	loadFrom >> writeTo.m_ePlayer;
	loadFrom >> writeTo.m_iTurnBuilt;

	int tmp;
	loadFrom >> tmp;
	writeTo.m_vCells.clear();
	for (int i=0; i<tmp; i++)
	{
		CvTacticalAnalysisCell tmp2;
		loadFrom >> tmp2; writeTo.m_vCells.push_back(tmp2);
	}
	loadFrom >> tmp;
	writeTo.m_DominanceZones.clear();
	for (int i=0; i<tmp; i++)
	{
		CvTacticalDominanceZone tmp2;
		loadFrom >> tmp2; writeTo.AddNewDominanceZone(tmp2);
	}

	loadFrom >> tmp;
	writeTo.m_EnemyUnits.clear();
	for (int i=0; i<tmp; i++)
	{
		IDInfo tmp2;
		loadFrom >> tmp2.eOwner; loadFrom >> tmp2.iID; writeTo.m_EnemyUnits.push_back(tmp2);
	}
	loadFrom >> tmp;
	writeTo.m_EnemyCities.clear();
	for (int i=0; i<tmp; i++)
	{
		IDInfo tmp2;
		loadFrom >> tmp2.eOwner; loadFrom >> tmp2.iID; writeTo.m_EnemyCities.push_back(tmp2);
	}

	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const CvTacticalDominanceZone& readFrom)
{
	saveTo << readFrom.m_iZoneID;
	saveTo << readFrom.m_eTerritoryType;
	saveTo << readFrom.m_eOverallDominanceFlag;
	saveTo << readFrom.m_eOwner;
	saveTo << readFrom.m_iCityID;
	saveTo << readFrom.m_iAreaID;
	saveTo << readFrom.m_iFriendlyMeleeStrength;
	saveTo << readFrom.m_iEnemyMeleeStrength;
	saveTo << readFrom.m_iFriendlyRangedStrength;
	saveTo << readFrom.m_iEnemyRangedStrength;
	saveTo << readFrom.m_iFriendlyNavalStrength;
	saveTo << readFrom.m_iEnemyNavalStrength;
	saveTo << readFrom.m_iFriendlyNavalRangedStrength;
	saveTo << readFrom.m_iEnemyNavalRangedStrength;
	saveTo << readFrom.m_iFriendlyUnitCount;
	saveTo << readFrom.m_iEnemyUnitCount;
	saveTo << 0;
	saveTo << 0;
	saveTo << 0;
	saveTo << readFrom.m_iNeutralUnitCount;
	saveTo << readFrom.m_iNeutralUnitStrength;
	saveTo << 0;
	saveTo << readFrom.m_iEnemyNavalUnitCount;
	saveTo << readFrom.m_iFriendlyNavalUnitCount;
	saveTo << readFrom.m_iZoneValue;
	saveTo << readFrom.m_iRangeClosestEnemyUnit;
	saveTo << readFrom.m_bIsWater;
	saveTo << readFrom.m_bIsNavalInvasion;
	saveTo << readFrom.m_iAvgX;
	saveTo << readFrom.m_iAvgY;
	saveTo << readFrom.m_iPlotCount;

	saveTo << readFrom.m_vNeighboringZones.size();
	for (size_t i=0; i<readFrom.m_vNeighboringZones.size(); i++)
		saveTo << readFrom.m_vNeighboringZones[i];

	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CvTacticalDominanceZone& writeTo)
{
	int tmp;

	loadFrom >> writeTo.m_iZoneID;
	loadFrom >> tmp; writeTo.m_eTerritoryType = (eDominanceTerritoryTypes)tmp;
	loadFrom >> tmp; writeTo.m_eOverallDominanceFlag = (eTacticalDominanceFlags)tmp;
	loadFrom >> writeTo.m_eOwner;
	loadFrom >> writeTo.m_iCityID;
	loadFrom >> writeTo.m_iAreaID;
	loadFrom >> writeTo.m_iFriendlyMeleeStrength;
	loadFrom >> writeTo.m_iEnemyMeleeStrength;
	loadFrom >> writeTo.m_iFriendlyRangedStrength;
	loadFrom >> writeTo.m_iEnemyRangedStrength;
	loadFrom >> writeTo.m_iFriendlyNavalStrength;
	loadFrom >> writeTo.m_iEnemyNavalStrength;
	loadFrom >> writeTo.m_iFriendlyNavalRangedStrength;
	loadFrom >> writeTo.m_iEnemyNavalRangedStrength;
	loadFrom >> writeTo.m_iFriendlyUnitCount;
	loadFrom >> writeTo.m_iEnemyUnitCount;
	loadFrom >> tmp; //dummy for compatibility
	loadFrom >> tmp; //dummy for compatibility
	loadFrom >> tmp; //dummy for compatibility
	loadFrom >> writeTo.m_iNeutralUnitCount;
	loadFrom >> writeTo.m_iNeutralUnitStrength;
	loadFrom >> tmp; //dummy for compatibility
	loadFrom >> writeTo.m_iEnemyNavalUnitCount;
	loadFrom >> writeTo.m_iFriendlyNavalUnitCount;
	loadFrom >> writeTo.m_iZoneValue;
	loadFrom >> writeTo.m_iRangeClosestEnemyUnit;
	loadFrom >> writeTo.m_bIsWater;
	loadFrom >> writeTo.m_bIsNavalInvasion;
	loadFrom >> writeTo.m_iAvgX;
	loadFrom >> writeTo.m_iAvgY;
	loadFrom >> writeTo.m_iPlotCount;

	loadFrom >> tmp;
	for (int i=0; i<tmp; i++)
	{
		int tmp2;
		loadFrom >> tmp2; writeTo.m_vNeighboringZones.push_back(tmp2);
	}

	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const CvTacticalAnalysisCell& readFrom)
{
	saveTo << readFrom.m_uiFlags;
	saveTo << readFrom.m_iDeploymentScore;
	saveTo << readFrom.m_eTargetType;
	saveTo << readFrom.m_iZoneID;
	saveTo << readFrom.m_iTargetDistance;
	saveTo << readFrom.m_bHasLOSToTarget;
	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CvTacticalAnalysisCell& writeTo)
{
	int tmp;

	loadFrom >> writeTo.m_uiFlags;
	loadFrom >> writeTo.m_iDeploymentScore;
	loadFrom >> tmp; writeTo.m_eTargetType = (AITacticalTargetType)tmp;
	loadFrom >> writeTo.m_iZoneID;
	loadFrom >> writeTo.m_iTargetDistance;
	loadFrom >> writeTo.m_bHasLOSToTarget;
	return loadFrom;
}
