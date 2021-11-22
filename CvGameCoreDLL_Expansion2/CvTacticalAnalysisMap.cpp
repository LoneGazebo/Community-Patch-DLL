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
#include <sstream>
#include <iomanip>

#include "LintFree.h"

bool g_bLogZones = false;

//=====================================
// CvTacticalDominanceZone
//=====================================
/// Constructor
CvTacticalDominanceZone::CvTacticalDominanceZone(void)
{
	m_iZoneID = -1;
	m_eTerritoryType = TACTICAL_TERRITORY_NONE;
	m_eOverallDominanceFlag = TACTICAL_DOMINANCE_NO_UNITS_VISIBLE;
	m_ePosture = TACTICAL_POSTURE_NONE;
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
		m_eOwner = pCity->getOwner();
		m_iCityID = pCity->GetID();
	}
	else
	{
		m_eOwner = NO_PLAYER;
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
		//avoid overflow
		int iRatio = int(0.5f + 100 * ( float(GetFriendlyRangedStrength()) / max(1u, GetEnemyRangedStrength())));
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
		//avoid overflow
		int iRatio = int(0.5f + 100 * ( float(GetFriendlyNavalRangedStrength()) / max(1u, GetEnemyNavalRangedStrength())));
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

//=====================================
// CvTacticalAnalysisMap
//=====================================
/// Constructor
CvTacticalAnalysisMap::CvTacticalAnalysisMap(void) :
	m_ePlayer(NO_PLAYER),
	m_iTurnSliceBuilt(-1)
{
}

/// Destructor
CvTacticalAnalysisMap::~CvTacticalAnalysisMap(void)
{
}

/// Initialize
void CvTacticalAnalysisMap::Reset(PlayerTypes ePlayer)
{
	m_ePlayer = ePlayer;
	m_iTurnSliceBuilt = -1;

	m_vPlotZoneID = vector<int>( GC.getMap().numPlots(), -1 );
	m_vDominanceZones.clear();
	m_IdLookup.clear();
}

int CvTacticalDominanceZone::GetBorderScore(DomainTypes eDomain, CvCity** ppWorstNeighborCity) const
{
	int iSum = 0;

	if (ppWorstNeighborCity)
		*ppWorstNeighborCity = NULL;
	int iWorstScore = 0;

	CvTacticalAnalysisMap* pTactMap = GET_PLAYER(m_eOwner).GetTacticalAI()->GetTacticalAnalysisMap();
	for (size_t i = 0; i < m_vNeighboringZones.size(); i++)
	{
		CvTacticalDominanceZone* pNeighbor = pTactMap->GetZoneByID(m_vNeighboringZones[i]);
		if (!pNeighbor || pNeighbor->GetNumPlots() < 5) //ignore very small zones
			continue;

		if (eDomain != NO_DOMAIN && pNeighbor->GetDomain() != eDomain)
			continue;

		int iScore = 0;

		if (pNeighbor->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
			iScore += 8;

		if (pNeighbor->GetTerritoryType() == TACTICAL_TERRITORY_NEUTRAL)
		{
			iScore += 2;

			//similar to GetPlayersAtWarWithInFuture() ...
			if (pNeighbor->GetOwner() != NO_PLAYER && GET_PLAYER(pNeighbor->GetOwner()).isMajorCiv())
			{
				if (GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetVisibleApproachTowardsUs(pNeighbor->GetOwner()) <= CIV_APPROACH_GUARDED)
					iScore++;
				if (GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetCivApproach(pNeighbor->GetOwner()) <= CIV_APPROACH_GUARDED)
					iScore++;
				if (GET_PLAYER(m_eOwner).GetDiplomacyAI()->GetCivOpinion(pNeighbor->GetOwner()) == CIV_OPINION_ENEMY)
					iScore++;
			}
		}

		//try to take into account relative strength
		iScore += (pNeighbor->GetTotalEnemyUnitCount() + pNeighbor->GetNeutralUnitCount()) / max(1,GetTotalFriendlyUnitCount());
		
		if (pNeighbor->GetTerritoryType() == TACTICAL_TERRITORY_NONE)
			iScore += 1;

		if (iScore > iWorstScore)
		{
			iWorstScore = iScore;
			if (ppWorstNeighborCity)
				*ppWorstNeighborCity = pNeighbor->GetZoneCity();
		}

		iSum += iScore;
	}
	
	return iSum;
}

bool CvTacticalDominanceZone::HasNeighborZone(PlayerTypes eOwner) const
{
	CvTacticalAnalysisMap* pTactMap = GET_PLAYER(m_eOwner).GetTacticalAI()->GetTacticalAnalysisMap();
	for (size_t i = 0; i < m_vNeighboringZones.size(); i++)
	{
		CvTacticalDominanceZone* pNeighbor = pTactMap->GetZoneByID(m_vNeighboringZones[i]);
		if (pNeighbor && pNeighbor->GetOwner() == eOwner)
			return true;
	}

	return false;
}

/// Who is dominant in this one zone?
eTacticalDominanceFlags CvTacticalDominanceZone::SetOverallDominance(int iDominancePercentage)
{
	// Look at ratio of friendly to enemy strength
	if(GetOverallEnemyStrength()+GetOverallFriendlyStrength()<=0)
	{
		m_eOverallDominanceFlag = TACTICAL_DOMINANCE_NO_UNITS_VISIBLE; //also covers cities ...
	}
	else
	{
		// Otherwise compute it by strength
		if (GetOverallEnemyStrength() <= 0 && GetTotalFriendlyUnitCount() > 0) //make sure the denominator is valid later on
		{
			m_eOverallDominanceFlag = TACTICAL_DOMINANCE_FRIENDLY;
		}
		else if (GetTotalEnemyUnitCount()==1 && GetTotalFriendlyUnitCount()<=1) //both are weak, one unit might tip the balance
		{
			m_eOverallDominanceFlag = TACTICAL_DOMINANCE_EVEN;
		}
		else
		{
			//a bit complex to make sure there is no overflow
			int iRatio = int(0.5f + 100 * ( float(GetOverallFriendlyStrength()) / max(1u, GetOverallEnemyStrength())));
			if (iRatio > 100 + iDominancePercentage)
			{
				m_eOverallDominanceFlag = TACTICAL_DOMINANCE_FRIENDLY;
			}
			else if (iRatio < 100 - iDominancePercentage)
			{
				m_eOverallDominanceFlag = TACTICAL_DOMINANCE_ENEMY;
			}
			else
			{
				m_eOverallDominanceFlag = TACTICAL_DOMINANCE_EVEN;
			}
		}
	}

	return m_eOverallDominanceFlag;
}


eTacticalPosture CvTacticalDominanceZone::SelectPostureSingleZone(int iDominancePercent)
{
	m_ePosture = TACTICAL_POSTURE_NONE;

	eTacticalDominanceFlags eOverallDominance = SetOverallDominance(iDominancePercent);
	eTacticalDominanceFlags eRangedDominance = IsWater() ? GetNavalRangedDominanceFlag(iDominancePercent) : GetRangedDominanceFlag(iDominancePercent);

	//are our forces mostly ranged and the enemy has a lot of melee units?
	bool bInDangerOfCounterattack = IsWater() ? GetEnemyNavalStrength() > 2 * GetFriendlyNavalStrength() && GetFriendlyNavalRangedStrength() > 2 * GetFriendlyNavalStrength() :
		GetEnemyMeleeStrength() > 2 * GetFriendlyMeleeStrength() && GetFriendlyRangedStrength() > 2 * GetFriendlyMeleeStrength();

	//don't get into big fights over nothing
	if (GetZoneCity() == NULL)
	{
		m_ePosture = TACTICAL_POSTURE_EXPLOIT_FLANKS;
		return m_ePosture;
	}

	// Choice based on whose territory this is
	switch (GetTerritoryType())
	{
	case TACTICAL_TERRITORY_ENEMY:
	{
		// Default for this zone
		m_ePosture = TACTICAL_POSTURE_ATTRITION;

		//try to grab it ...
		CvCity *pClosestCity = GetZoneCity();
		if (pClosestCity && pClosestCity->isInDangerOfFalling())
		{
			m_ePosture = TACTICAL_POSTURE_SURGICAL_CITY_STRIKE;
		}
		// Withdraw if enemy dominant overall or we are vulnerable to counterattacks
		else if (eOverallDominance == TACTICAL_DOMINANCE_ENEMY || bInDangerOfCounterattack)
		{
			m_ePosture = TACTICAL_POSTURE_WITHDRAW;
		}
		else if (eOverallDominance == TACTICAL_DOMINANCE_EVEN)
		{
			if (GetEnemyMeleeStrength() > GetFriendlyMeleeStrength())
			{
				//if we have ranged dominance, keep our risk lower
				m_ePosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? TACTICAL_POSTURE_ATTRITION : TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}
			else
			{
				//if we have ranged dominance and melee dominance, go all in
				m_ePosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? TACTICAL_POSTURE_STEAMROLL : TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}
		}
		else if (eOverallDominance == TACTICAL_DOMINANCE_FRIENDLY)
		{
			m_ePosture = TACTICAL_POSTURE_STEAMROLL;
		}
		break;
	}

	case TACTICAL_TERRITORY_NEUTRAL:
	case TACTICAL_TERRITORY_NONE:
	{
		// Default for this zone
		m_ePosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? TACTICAL_POSTURE_ATTRITION : TACTICAL_POSTURE_EXPLOIT_FLANKS;

		if (eOverallDominance == TACTICAL_DOMINANCE_ENEMY)
		{
			if (GetEnemyMeleeStrength() > GetFriendlyMeleeStrength())
			{
				//if we have ranged dominance, keep our risk lower
				m_ePosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? TACTICAL_POSTURE_ATTRITION : TACTICAL_POSTURE_WITHDRAW;
			}
			else
			{
				//if we have ranged dominance, keep our risk lower
				m_ePosture = (eRangedDominance == TACTICAL_DOMINANCE_FRIENDLY) ? TACTICAL_POSTURE_EXPLOIT_FLANKS : TACTICAL_POSTURE_WITHDRAW;
			}
		}

		break;
	}
	case TACTICAL_TERRITORY_FRIENDLY:
	{
		m_ePosture = (eOverallDominance == TACTICAL_DOMINANCE_ENEMY) ? TACTICAL_POSTURE_HEDGEHOG : TACTICAL_POSTURE_COUNTERATTACK;
		break;
	}
	}

	return m_ePosture;
}

//overwrite the posture based on specific conditions in neighbor zones
eTacticalPosture CvTacticalDominanceZone::SelectPostureMultiZone(vector<CvTacticalDominanceZone*> vNeighbors)
{
	for (size_t i = 0; i < vNeighbors.size(); i++)
	{
		//play it safe ...
		if (vNeighbors[i] == NULL)
			continue;

		//if there is a neighboring land zone with a lot of ranged firepower, be careful
		if (IsWater() && GetPosture() == TACTICAL_POSTURE_STEAMROLL)
		{
			if (!vNeighbors[i]->IsWater() &&
				vNeighbors[i]->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY &&
				vNeighbors[i]->GetEnemyRangedStrength() > GetFriendlyNavalRangedStrength())
			{
				m_ePosture = TACTICAL_POSTURE_EXPLOIT_FLANKS;
			}
		}

		//withdraw if there is a neighboring zone which needs support 
		if (GetTerritoryType() != TACTICAL_TERRITORY_FRIENDLY)
		{
			if (IsWater() == vNeighbors[i]->IsWater() &&
				vNeighbors[i]->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY &&
				vNeighbors[i]->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
			{
				CvCity* pCity = vNeighbors[i]->GetZoneCity();
				if (!pCity || !pCity->isInDangerOfFalling())
					m_ePosture = TACTICAL_POSTURE_WITHDRAW;
			}
		}
	}

	return m_ePosture;
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

	for(size_t iI = 0; iI < m_vDominanceZones.size(); iI++)
	{
		m_vDominanceZones[iI].ClearNeighboringZones();
	}

	for (int i=0; i<iW; i++)
	{
		for (int j=0; j<iH; j++)
		{
			CvPlot* pA = GC.getMap().plot(i,j);
			CvPlot* pB = GC.getMap().plot(i,j+1); //next row
			CvPlot* pC = GC.getMap().plot(i+1,j); //next col

			int iA = pA ? GetDominanceZoneID(pA->GetPlotIndex()) : 0;
			int iB = pB ? GetDominanceZoneID(pB->GetPlotIndex()) : 0;
			int iC = pC ? GetDominanceZoneID(pC->GetPlotIndex()) : 0;

			//ID 0 is the "unknown zone"
			if (iA!=0 && iB!=0)
			{
				if (GetZoneByID(iA)->GetTerritoryType()!=TACTICAL_TERRITORY_NONE ||
					GetZoneByID(iB)->GetTerritoryType()!=TACTICAL_TERRITORY_NONE)
				{
					GetZoneByID(iA)->AddNeighboringZone(iB);
					GetZoneByID(iB)->AddNeighboringZone(iA);
				}
			}
			if (iA!=0 && iC!=0)
			{
				if (GetZoneByID(iA)->GetTerritoryType()!=TACTICAL_TERRITORY_NONE ||
					GetZoneByID(iC)->GetTerritoryType()!=TACTICAL_TERRITORY_NONE)
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
	//not initialized
	if (m_vPlotZoneID.size() != GC.getMap().numPlots())
		return false;
	
	//explicitly invalidated
	if (m_iTurnSliceBuilt == -1)
		return false;

	//otherwise consider it up to date if it's not our turn
	if (m_ePlayer != GC.getGame().getActivePlayer())
		return true;

	//default check for age
	return (m_iTurnSliceBuilt == GC.getGame().getTurnSlice());
}

void CvTacticalAnalysisMap::Invalidate()
{
	m_iTurnSliceBuilt = -1;
}

/// Fill the map with data for this AI player's turn
void CvTacticalAnalysisMap::RefreshIfOutdated()
{
	if (IsUpToDate())
		return;

	//this is where the sausage is made
	CreateDominanceZones();

	//barbarians don't care about tactical dominance
	if(m_ePlayer!=BARBARIAN_PLAYER)
	{
		EstablishZoneNeighborhood();
		CalculateMilitaryStrengths();
		UpdatePostures();
		PrioritizeZones();

		//only temporary measure, creates a huge amount of logs
		if (g_bLogZones)
			LogZones();
	}
}

int CvTacticalAnalysisMap::GetNumZones() 
{ 
	RefreshIfOutdated();

	return m_vDominanceZones.size(); 
};

/// Add data for this cell into dominance zone information
void CvTacticalAnalysisMap::CreateDominanceZones()
{
	//important, set this first so that lookups don't get us into an infinite loop
	m_iTurnSliceBuilt = GC.getGame().getTurnSlice();
	m_vPlotZoneID = vector<int>( GC.getMap().numPlots(), 0 );
	m_vDominanceZones.clear();
	m_IdLookup.clear();

	//first create the unknown zone
	m_vDominanceZones.push_back( CvTacticalDominanceZone() );
	m_vDominanceZones[0].SetZoneID(0);
	m_IdLookup[0] = 0;

	//not all plots belong to a city
	std::tr1::unordered_set<CvPlot*> nonCityZonePlots;

	//don't make our zones too large
	int iMaxRange = 4;

	TeamTypes eOurTeam = GET_PLAYER(m_ePlayer).getTeam();
	CvTacticalDominanceZone* pCurrentZone = NULL;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iI);

		//some plot will be part of the "unknown zone"
		if (!pPlot || !pPlot->isRevealed(eOurTeam))
		{
			m_vPlotZoneID[iI] = 0;
			continue;
		}

		//is this plot close to a city?
		int iCityDistance = GC.getGame().GetClosestCityDistanceInPlots(pPlot);
		if (iCityDistance > iMaxRange)
		{
			//figure those out later
			nonCityZonePlots.insert(pPlot);
			continue;
		}

		//which city should this belong to?
		CvCity* pZoneCity = NULL;
		if (iCityDistance < 3)
			pZoneCity = GC.getGame().GetClosestCityByPlots(pPlot);
		else if ( pPlot->isOwned() )
			pZoneCity = pPlot->getOwningCity();
		else
			pZoneCity = GC.getGame().GetClosestCityByPathLength(pPlot);

		//should not happen
		if (!pZoneCity)
		{
			nonCityZonePlots.insert(pPlot);
			continue;
		}

		//now it gets interesting
		int iZoneID = pZoneCity->GetID();
		if (pPlot->isWater())
			iZoneID *= -1;

		//chances are it's the same zone as before
		if (!pCurrentZone || pCurrentZone->GetZoneID() != iZoneID)
		{
			pCurrentZone = GetZoneByID(iZoneID);

			//still not found? then make a new one
			if (!pCurrentZone)
			{
				CvTacticalDominanceZone newZone;
				newZone.SetZoneID(iZoneID);
				newZone.SetZoneCity(pZoneCity);

				//this is actually tricky with lakes and islands ...
				newZone.SetAreaID(pPlot->getArea());

				if (pZoneCity->getTeam() == eOurTeam)
					newZone.SetTerritoryType(TACTICAL_TERRITORY_FRIENDLY);
				else if (GET_TEAM(eOurTeam).isAtWar(pZoneCity->getTeam()))
					newZone.SetTerritoryType(TACTICAL_TERRITORY_ENEMY);
				else
					newZone.SetTerritoryType(TACTICAL_TERRITORY_NEUTRAL);

				//make sure we can find it later
				m_IdLookup[iZoneID] = m_vDominanceZones.size();
				m_vDominanceZones.push_back(newZone);

				pCurrentZone = &m_vDominanceZones.back();
			}
		}
	
		//the main thing ...
		m_vPlotZoneID[iI] = iZoneID;
		pCurrentZone->Extend(pPlot);
	}

	//now assign unique ids to all the little scraps of non-city zones
	int iNonCityBaseId = 100000; //should be safe
	while (!nonCityZonePlots.empty())
	{
		vector<CvPlot*> stack;
		stack.push_back(*nonCityZonePlots.begin());
		nonCityZonePlots.erase(nonCityZonePlots.begin());

		int newId = iNonCityBaseId++;
		if (stack.back()->isWater())
			newId *= -1;

		CvTacticalDominanceZone newZone;
		newZone.SetZoneID(newId);
		newZone.SetZoneCity(NULL);
		newZone.SetAreaID(stack.back()->getArea());
		newZone.SetTerritoryType(TACTICAL_TERRITORY_NEUTRAL);

		while (!stack.empty())
		{
			CvPlot* current = stack.back(); stack.pop_back();
			if (current == NULL)
				continue;
			newZone.Extend(current);
			m_vPlotZoneID[current->GetPlotIndex()] = newId;

			CvPlot** neighbors = GC.getMap().getNeighborsUnchecked(current);
			for (int i = 0; i < NUM_DIRECTION_TYPES; i++)
			{
				CvPlot* neighbor = neighbors[i];
				if (!neighbor)
					continue;
				
				//must be same domain but do not create extra zones for small lakes or islands
				if (neighbor->getDomain() == current->getDomain() || neighbor->area()->getNumTiles()<4 || current->area()->getNumTiles()<4)
				{
					std::tr1::unordered_set<CvPlot*>::iterator it = nonCityZonePlots.find(neighbor);
					if (it != nonCityZonePlots.end())
					{
						stack.push_back(*it);
						nonCityZonePlots.erase(it);
					}
				}
			}
		}

		//make sure we can find it later
		m_IdLookup[newZone.GetZoneID()] = m_vDominanceZones.size();
		m_vDominanceZones.push_back(newZone);
	}
}

/// Calculate military presences in each owned dominance zone
void CvTacticalAnalysisMap::CalculateMilitaryStrengths()
{
	CvPlayer& thisPlayer = GET_PLAYER(m_ePlayer);
	TeamTypes eTeam = thisPlayer.getTeam();

	//weigh units close to the center of the zone higher - assume unit mobility increases over time
	int iMaxDistance = (GC.getGame().getCurrentEra() + /*8*/ GD_INT_GET(AI_TACTICAL_RECRUIT_RANGE)) / 2;	
	int iBias = 2; // some bias because action may still be spread out over the zone
	int iUnitStrengthMultiplier = 1; //relative to cities

	//performance optimization, create a visibility lookup table
	int iGridSize = GC.getMap().numPlots();
	vector<bool> visible(iGridSize, false);
	for (int i = 0; i < iGridSize; i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(i);
		if (pPlot->isVisible(eTeam))
		{
			visible[i] = true;

			//for our plots we know that all adjacent plots are visible anyway
			//but for far-away plots we set adjacent plots visible as well
			//a small AI cheat where a human can simple make an educated guess
			if (pPlot->getTeam() != eTeam)
			{
				CvPlot** aNeighbors = GC.getMap().getNeighborsUnchecked(pPlot);
				for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
				{
					if (aNeighbors[iI])
						visible[aNeighbors[iI]->GetPlotIndex()] = true;
				}
			}
		}
	}

	// Loop through the dominance zones
	for(unsigned int iI = 0; iI < m_vDominanceZones.size(); iI++)
	{
		CvTacticalDominanceZone* pZone = &m_vDominanceZones[iI];
		CvCity *pZoneCity = pZone->GetZoneCity();

		if(pZoneCity)
		{
			// Start with strength of the city itself
			int iCityHitPoints = pZoneCity->GetMaxHitPoints() - pZoneCity->getDamage();
			int iStrength = (pZoneCity->getStrengthValue() * iCityHitPoints) / pZoneCity->GetMaxHitPoints();

			if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				pZone->AddFriendlyRangedStrength(iStrength);
			else if(pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				pZone->AddEnemyRangedStrength(iStrength);
			else
				pZone->AddNeutralStrength(iStrength);
		}

		// check all units in the world
		// ignore the barbarians here! do not base tactical decisions on transients
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayerLoop);
			bool bEnemy = GET_TEAM(eTeam).isAtWar(kPlayer.getTeam());
			bool bFriendly =
				(eTeam == kPlayer.getTeam()) || //on the same team
				(thisPlayer.isMinorCiv() && thisPlayer.GetMinorCivAI()->IsAllies(kPlayer.GetID())) || //for minors also count their ally as friendly
				(thisPlayer.isMajorCiv() && thisPlayer.GetPlayersAtWarWith()==kPlayer.GetPlayersAtWarWith()); //for majors if we fight exactly the same enemies

			int iLoop;
			for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
			{
				if(pLoopUnit->isDelayedDeath())
					continue;
				
				if (!pLoopUnit->IsCombatUnit() && !pLoopUnit->IsCanAttackRanged()) //check for combat units is not enough because that excludes air units!
					continue;

				CvPlot* pPlot = pLoopUnit->plot();
				if(!pPlot)
					continue;

				bool bZoneTypeMatch = 
							(pLoopUnit->getDomainType() == DOMAIN_AIR) ||
							(pLoopUnit->getDomainType() == DOMAIN_LAND && !pZone->IsWater()) || 
							(pLoopUnit->getDomainType() == DOMAIN_SEA && pZone->IsWater());

				if (!bZoneTypeMatch && !pLoopUnit->IsCanAttackRanged())
					continue;

				//a little cheating for AI - invisible units still count with reduced strength
				bool bReducedStrength = pLoopUnit->isEmbarked() || !visible[pPlot->GetPlotIndex()] || !bZoneTypeMatch;

				int iPlotDistance = 0;
				//if there is a city, units in adjacent zones can also count
				if (pZoneCity)
				{
					iPlotDistance = plotDistance(*pPlot, *pZoneCity->plot());
					if (iPlotDistance>iMaxDistance && GetDominanceZoneID(pLoopUnit->plot()->GetPlotIndex()) != pZone->GetZoneID())
						continue; //unit is too far away and outside the zone
				}
				else
				{
					//if there is no city, the unit must be in the zone itself, distance is zero by definition
					if ( GetDominanceZoneID(pLoopUnit->plot()->GetPlotIndex()) != pZone->GetZoneID() )
						continue;
				}

				int iEffectiveDistance = MAX(0,iPlotDistance - iBias);
				int iDistanceMultiplier = MAX(1,iMaxDistance - iEffectiveDistance); 
				int iMobilityMultiplier = max(pLoopUnit->baseMoves(false), pLoopUnit->GetRange()) / 2;

				int iTotalMultiplier = iDistanceMultiplier * iMobilityMultiplier * iUnitStrengthMultiplier;
				int iUnitStrength = pLoopUnit->GetMaxAttackStrength(NULL,NULL,NULL,true,true) * iTotalMultiplier;
				int iRangedStrength = pLoopUnit->GetMaxRangedCombatStrength(NULL, /*pCity*/ NULL, true, NULL, NULL, true, true) * iTotalMultiplier;

				//consider citadels ... they actually boost defensive strength but whatever
				if (TacticalAIHelpers::IsPlayerCitadel(pLoopUnit->plot(), pLoopUnit->getOwner()))
					iUnitStrength *= 2;

				if(bReducedStrength)
				{
					iUnitStrength /= 2;
					iRangedStrength /= 2;
				}

				if (bEnemy)
				{
					if (pLoopUnit->getDomainType() == DOMAIN_SEA)
					{
						pZone->AddEnemyNavalStrength(iUnitStrength);
						pZone->AddEnemyNavalRangedStrength(iRangedStrength);
						pZone->AddEnemyNavalUnitCount(1);
					}
					else if (pLoopUnit->getDomainType() == DOMAIN_LAND)
					{
						pZone->AddEnemyMeleeStrength(iUnitStrength);
						pZone->AddEnemyRangedStrength(iRangedStrength);
						pZone->AddEnemyUnitCount(1);
					}
					else if (pLoopUnit->getDomainType() == DOMAIN_AIR)
					{
						if ( pPlot->isWater() )
							pZone->AddEnemyNavalRangedStrength(iRangedStrength);
						else
							pZone->AddEnemyRangedStrength(iRangedStrength);
					}
				}
				else if (bFriendly)
				{

					if (pLoopUnit->getDomainType() == DOMAIN_SEA)
					{
						pZone->AddFriendlyNavalStrength(iUnitStrength);
						pZone->AddFriendlyNavalRangedStrength(iRangedStrength);
						pZone->AddFriendlyNavalUnitCount(1);
					}
					else if (pLoopUnit->getDomainType() == DOMAIN_LAND)
					{
						pZone->AddFriendlyMeleeStrength(iUnitStrength);
						pZone->AddFriendlyRangedStrength(iRangedStrength);
						pZone->AddFriendlyUnitCount(1);
					}
					else if (pLoopUnit->getDomainType() == DOMAIN_AIR)
					{
						if ( pPlot->isWater() )
							pZone->AddFriendlyNavalRangedStrength(iRangedStrength);
						else
							pZone->AddFriendlyRangedStrength(iRangedStrength);
					}
				}
				else
				{
					//neutral has only very few stats
					pZone->AddNeutralStrength(iUnitStrength);
					pZone->AddNeutralUnitCount(1);
				}
			}
		}
	}
}

/// Establish order of zone processing for the turn
void CvTacticalAnalysisMap::PrioritizeZones()
{
	int iMostValuableCity = 1;
	for (unsigned int iI = 0; iI < m_vDominanceZones.size(); iI++)
	{
		CvCity* pZoneCity = m_vDominanceZones[iI].GetZoneCity();
		if (pZoneCity)
			iMostValuableCity = max(iMostValuableCity, pZoneCity->getEconomicValue(m_ePlayer));
	}

	// Loop through the dominance zones
	for (unsigned int iI = 0; iI < m_vDominanceZones.size(); iI++)
	{
		// Find the zone and compute dominance here
		CvTacticalDominanceZone* pZone = &m_vDominanceZones[iI];
		eTacticalDominanceFlags eDominance = pZone->GetOverallDominanceFlag();

		// Establish a base value for the region
		int iBaseValue = 1;
		int iMultiplier = 1;

		CvCity* pZoneCity = pZone->GetZoneCity();
		if (pZoneCity && pZoneCity->isAdjacentToArea(pZone->GetAreaID()))
		{
			//should we take into account distance to the border as well? probably dominance is enough
			iBaseValue += (int)sqrt(pZoneCity->getEconomicValue(m_ePlayer)*100.f/iMostValuableCity);

			if (GET_PLAYER(m_ePlayer).GetTacticalAI()->IsInFocusArea(pZoneCity->plot()))
			{
				iBaseValue *= 3;
			}

			if (pZoneCity->isVisible(GET_PLAYER(m_ePlayer).getTeam(), false))
			{
				iBaseValue *= 2;

				// How damaged is this visible city?
				int iMaxDamageMultiplier = 10;
				int iDamage = pZoneCity->getDamage();
				if (iDamage > (pZoneCity->GetMaxHitPoints() / iMaxDamageMultiplier))
				{
					iBaseValue *= (int)((iDamage + 1) * 10 / pZoneCity->GetMaxHitPoints());
				}
			}

#if defined(MOD_BALANCE_CORE)
			if (GET_PLAYER(m_ePlayer).IsTargetCityForOperation(pZoneCity,false) ||
				GET_PLAYER(m_ePlayer).IsTargetCityForOperation(pZoneCity,true) ||
				GET_PLAYER(m_ePlayer).GetMilitaryAI()->IsPreferredAttackTarget(pZoneCity))
			{
				iBaseValue *= 2;
			}

			if (pZoneCity->GetPlayer()->isMinorCiv())
			{
				//At war with ally of this minor? Greatly reduce priority.
				PlayerTypes eAlly = pZoneCity->GetPlayer()->GetMinorCivAI()->GetAlly();
				if (eAlly != NO_PLAYER && GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(eAlly).getTeam()))
				{
					iBaseValue = 1;
				}
			}
#endif
		}

		if (!pZone->IsWater())
		{
			iBaseValue *= 3;
		}

		// Now compute a multiplier based on current conditions here
		if (eDominance == TACTICAL_DOMINANCE_ENEMY)
		{
			if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
			{
				iMultiplier = 1;
			}
			else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
			{
				iMultiplier = 8;
			}
		}
		else if (eDominance == TACTICAL_DOMINANCE_EVEN && pZone->GetTotalEnemyUnitCount()>1) //make sure there's an actual fight going on
		{
			if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
			{
				iMultiplier = 4;
			}
			else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
			{
				iMultiplier = 4;
			}
		}
		else if (eDominance == TACTICAL_DOMINANCE_FRIENDLY)
		{
			if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
			{
				iMultiplier = 8;
			}
			else if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
			{
				iMultiplier = 1;
			}
		}
		if (!GET_PLAYER(m_ePlayer).isMinorCiv())
		{
			if (GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
			{
				if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_ENEMY)
				{
					iMultiplier *= 4;
				}
			}
			else if (GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
			{
				if (pZone->GetTerritoryType() == TACTICAL_TERRITORY_FRIENDLY)
				{
					iMultiplier *= 4;
				}
			}
		}

		// Save off the value for this zone
		pZone->SetDominanceZoneValue(iBaseValue * iMultiplier);
	}

	std::stable_sort(m_vDominanceZones.begin(), m_vDominanceZones.end());

	m_IdLookup.clear();
	for (size_t i = 0; i < m_vDominanceZones.size(); i++)
		m_IdLookup[m_vDominanceZones[i].GetZoneID()] = i;
}

/// Establish postures for each dominance zone (taking into account last posture)
void CvTacticalAnalysisMap::UpdatePostures()
{
	// first pass, look at zones in isolation
	for (unsigned int iI = 0; iI < m_vDominanceZones.size(); iI++)
	{
		CvTacticalDominanceZone* pZone = &m_vDominanceZones[iI];

		//todo: should we include the previous posture in the logic?
		//but we've thrown it away at this point ...
		pZone->SelectPostureSingleZone(/*40*/ GD_INT_GET(AI_TACTICAL_MAP_DOMINANCE_PERCENTAGE));
	}

	// second pass, look at neighbors as well
	// note that we don't check for convergence ...
	for (unsigned int iI = 0; iI < m_vDominanceZones.size(); iI++)
	{
		CvTacticalDominanceZone* pZone = &m_vDominanceZones[iI];
		//need to look up the neighbors manually ...
		vector<CvTacticalDominanceZone*> vNeighbors;
		vector<int> vNeighborIds = pZone->GetNeighboringZones();
		for (size_t j = 0; j < vNeighborIds.size(); j++)
			vNeighbors.push_back( GetZoneByID(vNeighborIds[j]) );

		//todo: should we include the previous posture in the logic?
		//but we've thrown it away at this point ...
		pZone->SelectPostureMultiZone(vNeighbors);
	}
}

/// Log dominance zone data
void CvTacticalAnalysisMap::LogZones()
{
	if (GC.getLogging() && GC.getAILogging() && GET_PLAYER(m_ePlayer).isMajorCiv())
	{
		std::stringstream ss;
		ss << "c:\\temp\\DominanceZones_" << GET_PLAYER(m_ePlayer).getCivilizationAdjective() << "_" << std::setfill('0') << std::setw(3) << GC.getGame().getGameTurn() << ".txt";
		std::ofstream of(ss.str().c_str());
		if (of.good())
		{
			of << "#x,y,terrain,owner,zoneid\n";
			for (size_t i = 0; i < m_vPlotZoneID.size(); i++)
			{
				CvPlot* pPlot = GC.getMap().plotByIndex(i);
				if (pPlot->isRevealed(GET_PLAYER(m_ePlayer).getTeam()))
				{
					CvString dump = CvString::format("%d,%d,%d,%d,%d\n", pPlot->getX(), pPlot->getY(), pPlot->getTerrainType(), pPlot->getOwner(), m_vPlotZoneID[i]);
					of << dump.c_str();
				}
			}

			of << "#summary------------------\n";

			for (unsigned int iI = 0; iI < m_vDominanceZones.size(); iI++)
			{
				CvTacticalDominanceZone* pZone = &m_vDominanceZones[iI];

				//don't blow up the logs for empty zones
				if (pZone->GetOverallFriendlyStrength() == 0 && pZone->GetOverallEnemyStrength() == 0)
					continue;

				CvString szLogMsg;
				szLogMsg.Format("#Zone ID: %d, %s, Size: %d, City: %s, Area ID: %d, Value: %d, FRIENDLY Str: %d (%d), Ranged: %d (naval %d), ENEMY Str: %d (%d), Ranged: %d (naval %d)",
					pZone->GetZoneID(), pZone->IsWater() ? "Water" : "Land", pZone->GetNumPlots(), pZone->GetZoneCity() ? pZone->GetZoneCity()->getName().c_str() : "none", pZone->GetAreaID(), pZone->GetDominanceZoneValue(),
					pZone->GetOverallFriendlyStrength(), pZone->GetTotalFriendlyUnitCount(), pZone->GetFriendlyRangedStrength(), pZone->GetFriendlyNavalRangedStrength(),
					pZone->GetOverallEnemyStrength(), pZone->GetTotalEnemyUnitCount(), pZone->GetEnemyRangedStrength(), pZone->GetEnemyNavalRangedStrength());

				if (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_FRIENDLY)
				{
					szLogMsg += ", Friendly";
				}
				else if (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY)
				{
					szLogMsg += ", Enemy";
				}
				else if (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_EVEN)
				{
					szLogMsg += ", Even";
				}
				else if (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_NO_UNITS_VISIBLE)
				{
					szLogMsg += ", No Units Visible";
				}

				if (pZone->IsWater())
				{
					szLogMsg += ", Water";
				}
				else
				{
					szLogMsg += ", Land";
				}

				if (pZone->GetZoneCity())
				{
					if (GET_PLAYER(m_ePlayer).GetTacticalAI()->IsInFocusArea(pZone->GetZoneCity()->plot()))
					{
						szLogMsg += " (Focus)";
					}
				}

				of << szLogMsg.c_str() << "\n";
			}

			of.close();
		}
	}
}

/// Retrieve a dominance zone
CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZoneByIndex(int iIndex)
{
	RefreshIfOutdated();

	if(iIndex < 0 || iIndex >= (int)m_vDominanceZones.size())
		return NULL;
	return &m_vDominanceZones[iIndex];
}

/// Retrieve a dominance zone by closest city
CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZoneByCity(const CvCity* pCity, bool bWater)
{
	RefreshIfOutdated();

	if (!pCity)
		return NULL;

	//water zones have negative ids
	return GetZoneByID(pCity->GetID()*(bWater ? -1 : +1));
}

/// Retrieve a dominance zone by ID
CvTacticalDominanceZone* CvTacticalAnalysisMap::GetZoneByID(int iID)
{
	RefreshIfOutdated();

	map<int, int>::iterator it = m_IdLookup.find(iID);
	if (it != m_IdLookup.end())
		return GetZoneByIndex(it->second);

	return NULL;
}

CvTacticalDominanceZone * CvTacticalAnalysisMap::GetZoneByPlot(const CvPlot * pPlot)
{
	RefreshIfOutdated();

	if (!pPlot || pPlot->GetPlotIndex()>=(int)m_vPlotZoneID.size())
		return NULL;

	return GetZoneByID(m_vPlotZoneID[pPlot->GetPlotIndex()]);
}

int CvTacticalAnalysisMap::GetDominanceZoneID(int iPlotIndex)
{
	RefreshIfOutdated();

	if (iPlotIndex<0 || iPlotIndex>=(int)m_vPlotZoneID.size())
		return NULL;

	return m_vPlotZoneID[iPlotIndex];
}

// Is this plot in dangerous territory?
bool CvTacticalAnalysisMap::IsInEnemyDominatedZone(const CvPlot* pPlot)
{
	RefreshIfOutdated();

	if (!pPlot || pPlot->GetPlotIndex()>=(int)m_vPlotZoneID.size())
		return NULL;

	CvTacticalDominanceZone* pZone = GetZoneByID(m_vPlotZoneID[pPlot->GetPlotIndex()]);

	if(pZone && pZone->GetZoneCity()) //city check is to skip the potentially very large ocean zone
		return (pZone->GetOverallDominanceFlag() == TACTICAL_DOMINANCE_ENEMY);

	return false;
}

FDataStream& operator<<(FDataStream& saveTo, const CvTacticalAnalysisMap& readFrom)
{
	saveTo << readFrom.m_ePlayer;
	saveTo << readFrom.m_iTurnSliceBuilt;

	saveTo << readFrom.m_vPlotZoneID;
	saveTo << readFrom.m_vDominanceZones;
	saveTo << readFrom.m_IdLookup;

	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CvTacticalAnalysisMap& writeTo)
{
	loadFrom >> writeTo.m_ePlayer;
	loadFrom >> writeTo.m_iTurnSliceBuilt;

	loadFrom >> writeTo.m_vPlotZoneID;
	loadFrom >> writeTo.m_vDominanceZones;
	loadFrom >> writeTo.m_IdLookup;

	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const CvTacticalDominanceZone& readFrom)
{
	saveTo << readFrom.m_iZoneID;
	saveTo << readFrom.m_eTerritoryType;
	saveTo << readFrom.m_eOverallDominanceFlag;
	saveTo << readFrom.m_ePosture;
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
	saveTo << readFrom.m_iNeutralUnitCount;
	saveTo << readFrom.m_iNeutralUnitStrength;
	saveTo << readFrom.m_iEnemyNavalUnitCount;
	saveTo << readFrom.m_iFriendlyNavalUnitCount;
	saveTo << readFrom.m_iZoneValue;
	saveTo << 0u;
	saveTo << readFrom.m_iAvgX;
	saveTo << readFrom.m_iAvgY;
	saveTo << readFrom.m_iPlotCount;

	saveTo << readFrom.m_vNeighboringZones;

	return saveTo;
}
FDataStream& operator>>(FDataStream& loadFrom, CvTacticalDominanceZone& writeTo)
{
	int tmp;

	loadFrom >> writeTo.m_iZoneID;
	loadFrom >> tmp; writeTo.m_eTerritoryType = (eDominanceTerritoryTypes)tmp;
	loadFrom >> tmp; writeTo.m_eOverallDominanceFlag = (eTacticalDominanceFlags)tmp;
	loadFrom >> tmp; writeTo.m_ePosture = (eTacticalPosture)tmp;
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
	loadFrom >> writeTo.m_iNeutralUnitCount;
	loadFrom >> writeTo.m_iNeutralUnitStrength;
	loadFrom >> writeTo.m_iEnemyNavalUnitCount;
	loadFrom >> writeTo.m_iFriendlyNavalUnitCount;
	loadFrom >> writeTo.m_iZoneValue;
	loadFrom >> tmp; //dummy
	loadFrom >> writeTo.m_iAvgX;
	loadFrom >> writeTo.m_iAvgY;
	loadFrom >> writeTo.m_iPlotCount;

	loadFrom >> writeTo.m_vNeighboringZones;

	return loadFrom;
}

