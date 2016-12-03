/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDangerPlots.h"
#include "CvGameCoreUtils.h"
#include "CvAStar.h"
#include "CvEnumSerialization.h"
#include "CvDiplomacyAI.h"
#include "CvMilitaryAI.h"
#include "CvMinorCivAI.h"
#include "FireWorks/FRemark.h"

// must be included after all other headers
#include "LintFree.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4505 ) // unreferenced local function has been removed.. needed by REMARK below
#endif//_MSC_VER

REMARK_GROUP("CvDangerPlots");

/// Constructor
CvDangerPlots::CvDangerPlots(void)
	: m_ePlayer(NO_PLAYER)
	, m_bArrayAllocated(false)
	, m_bDirty(false)
{
	m_fMajorWarMod = GC.getAI_DANGER_MAJOR_APPROACH_WAR();
	m_fMajorHostileMod = GC.getAI_DANGER_MAJOR_APPROACH_HOSTILE();
	m_fMajorDeceptiveMod = GC.getAI_DANGER_MAJOR_APPROACH_DECEPTIVE();
	m_fMajorGuardedMod = GC.getAI_DANGER_MAJOR_APPROACH_GUARDED();
	m_fMajorAfraidMod = GC.getAI_DANGER_MAJOR_APPROACH_AFRAID();
	m_fMajorFriendlyMod = GC.getAI_DANGER_MAJOR_APPROACH_FRIENDLY();
	m_fMajorNeutralMod = GC.getAI_DANGER_MAJOR_APPROACH_NEUTRAL();
	m_fMinorNeutralMinorMod = GC.getAI_DANGER_MINOR_APPROACH_NEUTRAL();
	m_fMinorFriendlyMod = GC.getAI_DANGER_MINOR_APPROACH_FRIENDLY();
	m_fMinorBullyMod = GC.getAI_DANGER_MINOR_APPROACH_BULLY();
	m_fMinorConquestMod = GC.getAI_DANGER_MINOR_APPROACH_CONQUEST();

	m_DangerPlots = NULL;
}

/// Destructor
CvDangerPlots::~CvDangerPlots(void)
{
	Uninit();
}

/// Initialize
void CvDangerPlots::Init(PlayerTypes ePlayer, bool bAllocate)
{
	Uninit();
	m_ePlayer = ePlayer;

	if(bAllocate)
	{
		int iGridSize = GC.getMap().numPlots();
		CvAssertMsg(iGridSize > 0, "iGridSize is zero");
		m_DangerPlots = FNEW(CvDangerPlotContents[iGridSize], c_eCiv5GameplayDLL, 0);
		m_bArrayAllocated = true;
		for(int i = 0; i < iGridSize; i++)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(i);
			m_DangerPlots[i].init(*pPlot);
		}
	}
}

/// Uninitialize
void CvDangerPlots::Uninit()
{
	m_ePlayer = NO_PLAYER;
	SAFE_DELETE_ARRAY(m_DangerPlots);
	m_bArrayAllocated = false;
	m_bDirty = false;
}

bool CvDangerPlots::UpdateDangerSingleUnit(CvUnit* pLoopUnit, bool bIgnoreVisibility, bool bRemember)
{
	if(ShouldIgnoreUnit(pLoopUnit, bIgnoreVisibility))
		return false;

	if(IsKnownAttacker(pLoopUnit->getOwner(),pLoopUnit->GetID()))
		return false;

	//for ranged every plot we can enter with movement left is a base for attack
	int iMinMovesLeft = pLoopUnit->IsCanAttackRanged() ? 1 : 0;

	//specialty for barbarian who won't leave camp
	if (pLoopUnit->isBarbarian() && pLoopUnit->plot()->getImprovementType()==(ImprovementTypes)GC.getBARBARIAN_CAMP_IMPROVEMENT())
		iMinMovesLeft = pLoopUnit->getMoves();

	//use the worst case assumption here, no ZOC (all intervening units have been killed)
	//the IGNORE_DANGER flag is extremely important here, otherwise we can get into endless loops
	//(when the pathfinder does a lazy danger update)
	ReachablePlots reachablePlots;
	int iFlags = CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_IGNORE_ZOC | CvUnit::MOVEFLAG_NO_EMBARK | CvUnit::MOVEFLAG_IGNORE_DANGER;
	TacticalAIHelpers::GetAllPlotsInReachThisTurn(pLoopUnit,pLoopUnit->plot(),reachablePlots,iFlags,iMinMovesLeft,-1,set<int>());

	if (pLoopUnit->IsCanAttackRanged())
	{
		//for ranged every tile we can enter with movement left is a base for attack
		std::set<int> attackableTiles;
		TacticalAIHelpers::GetPlotsUnderRangedAttackFrom(pLoopUnit,reachablePlots,attackableTiles,false,false);

		for (std::set<int>::iterator attackTile=attackableTiles.begin(); attackTile!=attackableTiles.end(); ++attackTile)
		{
			CvPlot* pAttackTile = GC.getMap().plotByIndexUnchecked(*attackTile);
			AssignUnitDangerValue(pLoopUnit, pAttackTile);
		}
	}
	else
	{
		//for melee every tile we can move into can be attacked
		for (ReachablePlots::iterator moveTile=reachablePlots.begin(); moveTile!=reachablePlots.end(); ++moveTile)
		{
			CvPlot* pMoveTile = GC.getMap().plotByIndexUnchecked(moveTile->iPlotIndex);
			AssignUnitDangerValue(pLoopUnit, pMoveTile);
		}
	}

	if (bRemember)
		AddKnownAttacker(pLoopUnit->getOwner(),pLoopUnit->GetID());

	return true;
}

/// Updates the danger plots values to reflect threats across the map
void CvDangerPlots::UpdateDanger(bool bPretendWarWithAllCivs, bool bIgnoreVisibility)
{
	// danger plots have not been initialized yet, so no need to update
	if(!m_bArrayAllocated)
		return;

	// wipe out values
	int iGridSize = GC.getMap().numPlots();
	CvAssertMsg(iGridSize == m_DangerPlots.size(), "iGridSize does not match number of DangerPlots");
	for(int i = 0; i < iGridSize; i++)
	{
		m_DangerPlots[i].clear();
	}

	//units we know from last turn
	UnitSet previousKnownUnits = m_knownUnits;
	m_knownUnits.clear();

	CvPlayer& thisPlayer = GET_PLAYER(m_ePlayer);
	TeamTypes thisTeam = thisPlayer.getTeam();

	// for each opposing civ
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iPlayer;
		CvPlayer& loopPlayer = GET_PLAYER(ePlayer);
		TeamTypes eTeam = loopPlayer.getTeam();

		if(!loopPlayer.isAlive())
			continue;

		if(eTeam == thisTeam)
			continue;

		if(ShouldIgnorePlayer(ePlayer) && !bPretendWarWithAllCivs)
			continue;

		//for each unit
		int iLoop;
		CvUnit* pLoopUnit = NULL;
		for(pLoopUnit = loopPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = loopPlayer.nextUnit(&iLoop))
		{
			UpdateDangerSingleUnit(pLoopUnit, bIgnoreVisibility, true);
		}

		// for each city
		CvCity* pLoopCity;
		for(pLoopCity = loopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = loopPlayer.nextCity(&iLoop))
		{
			if(ShouldIgnoreCity(pLoopCity, bIgnoreVisibility))
				continue;

#if defined(MOD_EVENTS_CITY_BOMBARD)
			bool bIndirectFireAllowed = false;
			int iRange = pLoopCity->getBombardRange(bIndirectFireAllowed);
#else
			int iRange = GC.getCITY_ATTACK_RANGE();
#endif
			CvPlot* pCityPlot = pLoopCity->plot();
			CvPlot* pLoopPlot = NULL;
			for(int iDX = -(iRange); iDX <= iRange; iDX++)
			{
				for(int iDY = -(iRange); iDY <= iRange; iDY++)
				{
					pLoopPlot = plotXYWithRangeCheck(pCityPlot->getX(), pCityPlot->getY(), iDX, iDY, iRange);
					if(!pLoopPlot || pLoopPlot == pCityPlot)
						continue;

#if defined(MOD_EVENTS_CITY_BOMBARD)
					if (!bIndirectFireAllowed && !pCityPlot->canSeePlot(pLoopPlot, NO_TEAM, iRange, NO_DIRECTION))
						continue;
#endif
					AssignCityDangerValue(pLoopCity, pLoopPlot);
				}
			}
		}
	}

	// now compare the new known units with the previous known units
	for (UnitSet::iterator it = previousKnownUnits.begin(); it != previousKnownUnits.end(); ++it)
	{
		//might have made peace ...
		if (ShouldIgnorePlayer(it->first))
			continue;

		if (m_knownUnits.find(*it) == m_knownUnits.end())
		{
			CvUnit* pVanishedUnit = GET_PLAYER(it->first).getUnit(it->second);

			//it's still there, but moved out of sight - nevertheless count is, a human would do that as well
			//do not add it to the known units though, so next turn we will have forgotten about it
			if (pVanishedUnit)
				UpdateDangerSingleUnit(pVanishedUnit, true, false);
		}
	}

	int iPlotLoop;
	CvPlot* pPlot, *pAdjacentPlot;
	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		if(pPlot->isRevealed(thisTeam))
		{
			//remember the plot based damage, but it depends on the unit's promotions also, so we won't apply it directly
			int iPlotDamage = 0;
			if (pPlot->getFeatureType() != NO_FEATURE)
				iPlotDamage += (GC.getFeatureInfo(pPlot->getFeatureType())->getTurnDamage());
			if (pPlot->getTerrainType() != NO_FEATURE)
				iPlotDamage += (GC.getTerrainInfo(pPlot->getTerrainType())->getTurnDamage());

			m_DangerPlots[iPlotLoop].m_bFlatPlotDamage = (iPlotDamage>0);

			ImprovementTypes eImprovement = pPlot->getRevealedImprovementType(thisTeam);
			if(eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage() > 0)
			{
				if(!ShouldIgnoreCitadel(pPlot, bIgnoreVisibility))
				{
					for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iI));

						if(pAdjacentPlot != NULL)
						{
							m_DangerPlots[iPlotLoop].m_pCitadel = pPlot;
						}
					}
				}
			}
		}
	}

	// testing city danger values
	CvCity* pLoopCity;
	int iLoopCity = 0;
	for(pLoopCity = thisPlayer.firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = thisPlayer.nextCity(&iLoopCity))
	{
		//adding danger would count each unit multiple times, is biased towards fast units
		//so we pretend they would all attack the city and tally up the damage
		//question is, what about our own defensive units in the area. should we count those as well?
		int iEvalRange = 4;
		int iThreatValue = 0;
		for(int iX = -iEvalRange; iX <= iEvalRange; iX++)
			for(int iY = -iEvalRange; iY <= iEvalRange; iY++)
			{
				CvPlot* pEvalPlot = plotXYWithRangeCheck(pLoopCity->getX(), pLoopCity->getY(), iX, iY, iEvalRange);
				if (pEvalPlot)
				{
					const CvUnit* pEnemy = pEvalPlot->getBestDefender(NO_PLAYER, thisPlayer.GetID(), NULL, true);
					if (pEnemy)
					{
						int iAttackerDamage = 0; //to be ignored
						iThreatValue += TacticalAIHelpers::GetSimulatedDamageFromAttackOnCity(pLoopCity,pEnemy,pEnemy->plot(),iAttackerDamage);
					}
				}
			}

		pLoopCity->SetThreatValue(iThreatValue);
	}

	m_bDirty = false;
}

/// Return the maximum amount of damage that could be dealt to a non-specific unit at this plot
int CvDangerPlots::GetDanger(const CvPlot& pPlot, PlayerTypes ePlayer)
{
	if(!m_bArrayAllocated)
		return 0;

	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	return m_DangerPlots[idx].GetDanger(ePlayer);
}

/// Return the maximum amount of damage a city could take at this plot
int CvDangerPlots::GetDanger(const CvPlot& pPlot, CvCity* pCity, const CvUnit* pPretendGarrison)
{
	if(!m_bArrayAllocated)
		return 0;

	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	if (pCity != NULL)
	{
		return m_DangerPlots[idx].GetDanger(pCity, pPretendGarrison);
	}
	return m_DangerPlots[idx].GetDanger(NO_PLAYER);
}

/// Return the maximum amount of damage a unit could take at this plot
int CvDangerPlots::GetDanger(const CvPlot& pPlot, const CvUnit* pUnit, AirActionType iAirAction)
{
	if(!m_bArrayAllocated)
		return 0;

	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	if (pUnit)
	{
		return m_DangerPlots[idx].GetDanger(pUnit, iAirAction);
	}
	return m_DangerPlots[idx].GetDanger(NO_PLAYER);
}

std::vector<CvUnit*> CvDangerPlots::GetPossibleAttackers(const CvPlot& Plot) const
{
	if(!m_bArrayAllocated)
		return std::vector<CvUnit*>();

	const int idx = Plot.getX() + Plot.getY() * GC.getMap().getGridWidth();

	return m_DangerPlots[idx].GetPossibleAttackers();
}

bool CvDangerPlots::IsKnownAttacker(PlayerTypes eOwner, int iUnitID) const
{
	return m_knownUnits.find(std::make_pair(eOwner, iUnitID)) != m_knownUnits.end();
}

void CvDangerPlots::AddKnownAttacker(PlayerTypes eOwner, int iUnitID)
{
	m_knownUnits.insert(std::make_pair(eOwner, iUnitID));
}

//	------------------------------------------------------------------------------------------------
//	Returns true if the relationship of the danger plots owner and the input player and plot owner
//	would result in a 0 danger.  This helps avoid costly path finder calls if the end result will be 0.
bool CvDangerPlots::IsDangerByRelationshipZero(PlayerTypes ePlayer, CvPlot* pPlot)
{
	CvAssertMsg(pPlot, "No plot passed in?");
	bool bIgnoreInFriendlyTerritory = false;

	// Full value if a player we're at war with
	if(GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
	{
		return false;
	}

	// if it's a human player, ignore neutral units
	if(GET_PLAYER(m_ePlayer).isHuman())
	{
		return true;
	}

	bool bResultMultiplierIsZero = false;
	if(GET_PLAYER(m_ePlayer).isMinorCiv())  // if the evaluator is a minor civ
	{
		if(!GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))  // and they're not at war with the other player
		{
#ifdef AUI_DANGER_PLOTS_FIX_IS_DANGER_BY_RELATIONSHIP_ZERO_MINORS_IGNORE_ALL_NONWARRED
			return true;
#else
			bIgnoreInFriendlyTerritory = true; // ignore friendly territory
#endif // AUI_DANGER_PLOTS_FIX_IS_DANGER_BY_RELATIONSHIP_ZERO_MINORS_IGNORE_ALL_NONWARRED
		}
	}
	else if(!GET_PLAYER(ePlayer).isMinorCiv())
	{
		// should we be using bHideTrueFeelings?
		switch(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false))
		{
		case MAJOR_CIV_APPROACH_WAR:
			bResultMultiplierIsZero = m_fMajorWarMod == 0.f;
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			bResultMultiplierIsZero = m_fMajorHostileMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			bResultMultiplierIsZero = m_fMajorDeceptiveMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			bResultMultiplierIsZero = m_fMajorGuardedMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			bResultMultiplierIsZero = m_fMajorAfraidMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			bResultMultiplierIsZero = m_fMajorFriendlyMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			bResultMultiplierIsZero = m_fMajorNeutralMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		}
	}
	else
	{
		switch(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetMinorCivApproach(ePlayer))
		{
		case MINOR_CIV_APPROACH_IGNORE:
			bResultMultiplierIsZero = m_fMinorNeutralMinorMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MINOR_CIV_APPROACH_FRIENDLY:
			bResultMultiplierIsZero = m_fMinorFriendlyMod == 0.f;
			bIgnoreInFriendlyTerritory = true;
			break;
		case MINOR_CIV_APPROACH_BULLY:
			bResultMultiplierIsZero = (m_fMinorBullyMod == 0.f);
			break;
		case MINOR_CIV_APPROACH_CONQUEST:
			bResultMultiplierIsZero = m_fMinorConquestMod == 0.f;
			break;
		}
	}

	// if the plot is in our own territory and, with the current approach, we should ignore danger values in our own territory
	// zero out the value
	if(pPlot && pPlot->getOwner() == m_ePlayer && bIgnoreInFriendlyTerritory)
	{
		return true;
	}

	return bResultMultiplierIsZero;
}


/// Should this player be ignored when creating the danger plots?
bool CvDangerPlots::ShouldIgnorePlayer(PlayerTypes ePlayer)
{
	//if one is major and the other a minor (but no barbarian)
	if(GET_PLAYER(m_ePlayer).isMinorCiv() != GET_PLAYER(ePlayer).isMinorCiv() && !GET_PLAYER(ePlayer).isBarbarian() && !GET_PLAYER(m_ePlayer).isBarbarian())
	{
		CvPlayer* pMinor = NULL;
		CvPlayer* pMajor;

		if(GET_PLAYER(m_ePlayer).isMinorCiv())
		{
			pMinor = &GET_PLAYER(m_ePlayer);
			pMajor = &GET_PLAYER(ePlayer);
		}
		else
		{
			pMinor = &GET_PLAYER(ePlayer);
			pMajor = &GET_PLAYER(m_ePlayer);
		}

		if(pMinor->GetMinorCivAI()->IsFriends(pMajor->GetID()))
		{
			return true;
		}

		// if we're a major, we should ignore minors that are not at war with us
		if (!GET_PLAYER(m_ePlayer).isMinorCiv())
		{
			TeamTypes eMajorTeam = pMajor->getTeam();
			TeamTypes eMinorTeam = pMinor->getTeam();
			if (!GET_TEAM(eMajorTeam).isAtWar(eMinorTeam))
			{
				return true;
			}
		}
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	//ignore if one is vassal of the other
	if ( GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).IsVassal(GET_PLAYER(ePlayer).getTeam()) ||
		 GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsVassal(GET_PLAYER(m_ePlayer).getTeam()) )
		 return true;
#endif

#if defined(MOD_BALANCE_CORE_MILITARY)
	//ignore if at peace
	if ( !GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()) )
		 return true;
#endif

	return false;
}

/// Should this unit be ignored when creating the danger plots?
bool CvDangerPlots::ShouldIgnoreUnit(CvUnit* pUnit, bool bIgnoreVisibility)
{
	if(!m_bArrayAllocated || m_ePlayer==NO_PLAYER || !pUnit)
		return true;

	if(!pUnit->IsCanAttack())
	{
		return true;
	}

#if defined(AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG) || defined(AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS)
	if (pUnit->isInvisible(GET_PLAYER(m_ePlayer).getTeam(), false))
	{
		return true;
	}
#endif // AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG

	if (pUnit->getDomainType() == DOMAIN_AIR)
	{
		return true;
	}

#ifdef AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG
	if (!GET_PLAYER(m_ePlayer).isMinorCiv() && !GET_PLAYER(m_ePlayer).isBarbarian() && pUnit->isBarbarian() && pUnit->plot()->isRevealed(GET_PLAYER(m_ePlayer).getTeam()))
		if (pUnit->plot()->isAdjacentVisible(GET_PLAYER(m_ePlayer).getTeam()))
			bIgnoreVisibility = true;
#endif // AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG

#ifdef AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS
	if (GET_PLAYER(m_ePlayer).isMinorCiv() && !GET_PLAYER(pUnit->getOwner()).isMinorCiv() && !pUnit->isBarbarian() && 
		GET_PLAYER(m_ePlayer).GetClosestCity(pUnit->plot(), AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS, true))
		bIgnoreVisibility = true;
#endif // AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS

	if(!pUnit->plot()->isVisible(GET_PLAYER(m_ePlayer).getTeam()) && !bIgnoreVisibility)
	{
		return true;
	}

#if !defined(AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG) && !defined(AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS)
	if(pUnit->isInvisible(GET_PLAYER(m_ePlayer).getTeam(), false))
	{
		return true;
	}
#endif // AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG

	return false;
}

/// Should this city be ignored when creating the danger plots?
bool CvDangerPlots::ShouldIgnoreCity(CvCity* pCity, bool bIgnoreVisibility)
{
	// ignore unseen cities
	if(!pCity->isRevealed(GET_PLAYER(m_ePlayer).getTeam(), false)  && !bIgnoreVisibility)
	{
		return true;
	}

	return false;
}

/// Should this city be ignored when creating the danger plots?
bool CvDangerPlots::ShouldIgnoreCitadel(CvPlot* pCitadelPlot, bool bIgnoreVisibility)
{
	// ignore unseen cities
	if(!pCitadelPlot->isRevealed(GET_PLAYER(m_ePlayer).getTeam())  && !bIgnoreVisibility)
	{
		return true;
	}

	PlayerTypes eOwner = pCitadelPlot->getOwner();
	if(eOwner != NO_PLAYER)
	{
		// Our own citadels aren't dangerous
		if(eOwner == m_ePlayer)
		{
			return true;
		}

		if(!atWar(GET_PLAYER(m_ePlayer).getTeam(), GET_PLAYER(eOwner).getTeam()))
		{
			return true;
		}
	}

	return false;
}

//	-----------------------------------------------------------------------------------------------
/// Contains the calculations to do the danger value for the plot according to the unit
void CvDangerPlots::AssignUnitDangerValue(CvUnit* pUnit, CvPlot* pPlot)
{
	if (!m_bArrayAllocated)
		return;

	if (IsDangerByRelationshipZero(pUnit->getOwner(), pPlot))
		return;

	if(GC.getGame().getGameTurn() <= 1)
		return;

	const int iPlotX = pPlot->getX();
	const int iPlotY = pPlot->getY();
	const int idx = GC.getMap().plotNum(iPlotX, iPlotY);
	
	m_DangerPlots[idx].m_apUnits.push_back( std::make_pair(pUnit->getOwner(),pUnit->GetID()) );
}

//	-----------------------------------------------------------------------------------------------
/// Contains the calculations to do the danger value for the plot according to the city
void CvDangerPlots::AssignCityDangerValue(CvCity* pCity, CvPlot* pPlot)
{
	if (!m_bArrayAllocated)
		return;

	if (IsDangerByRelationshipZero(pCity->getOwner(), pPlot))
		return;

	const int idx = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
	m_DangerPlots[idx].m_apCities.push_back( std::make_pair(pCity->getOwner(),pCity->GetID()) );
}

/// reads in danger plots info
void CvDangerPlots::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_ePlayer;
	kStream >> m_bArrayAllocated;

	int iGridSize;
	kStream >> iGridSize;

	Uninit();
	m_DangerPlots = FNEW(CvDangerPlotContents[iGridSize], c_eCiv5GameplayDLL, 0);
	m_bArrayAllocated = true;

	for (int i = 0; i < iGridSize; i++)
	{
		kStream >> m_DangerPlots[i];
	}

	m_knownUnits.clear();
	int iCount;
	kStream >> iCount;
	for (int i=0; i<iCount; i++)
	{
		int iTemp1,iTemp2;
		kStream >> iTemp1;
		kStream >> iTemp2;
		m_knownUnits.insert( std::make_pair((PlayerTypes)iTemp1,iTemp2) );
	}

	m_bDirty = false;
}

/// writes out danger plots info
void CvDangerPlots::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_ePlayer;
	kStream << m_bArrayAllocated;

	int iGridSize = GC.getMap().getGridWidth() * GC.getMap().getGridHeight();
	kStream << iGridSize;
	for(int i = 0; i < iGridSize; i++)
	{
		kStream << m_DangerPlots[i];
	}


#if defined(MOD_BALANCE_CORE)
	kStream << m_knownUnits.size();
	for (UnitSet::const_iterator it=m_knownUnits.begin(); it!=m_knownUnits.end(); ++it)
	{
		kStream << it->first;
		kStream << it->second;
	}
#endif

}

//	-----------------------------------------------------------------------------------------------
void CvDangerPlots::SetDirty()
{
	m_bDirty = true;
}

// Get the maximum damage unit could receive at this plot in the next turn
int CvDangerPlotContents::GetDanger(PlayerTypes ePlayer)
{
	if (!m_pPlot)
		return 0;

	// Damage from terrain - since we don't know the unit, just assume 20
	int iPlotDamage = m_bFlatPlotDamage ? 20 : 0;

	// Damage from units
	CvPlot* pAttackerPlot = NULL;

	for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
	{
		CvUnit* pUnit = GET_PLAYER(it->first).getUnit(it->second);

		if ( !pUnit || pUnit->isDelayedDeath() || pUnit->IsDead())
		{
			continue;
		}

		pAttackerPlot = NULL;
		if (pUnit->IsCanAttackRanged())
		{
			if (pUnit->getDomainType() == DOMAIN_AIR)
			{
				iPlotDamage += pUnit->GetAirCombatDamage(NULL, NULL, false, 0, m_pPlot);
			}
			else
			{
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, NULL, false, 0, m_pPlot);
			}
		}
		else
		{
			if (plotDistance(m_iX, m_iY, pUnit->getX(), pUnit->getY()) == 1)
			{
				pAttackerPlot = pUnit->plot();
			}
			//we don't know the defender strength, so assume it's equal to attacker strength!
			iPlotDamage += pUnit->getCombatDamage(
				pUnit->GetMaxAttackStrength(pAttackerPlot, m_pPlot, NULL),
				pUnit->GetBaseCombatStrength()*100, 
				pUnit->getDamage(), false, false, false);

			if (pUnit->isRangedSupportFire())
			{
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, NULL, false, 0, m_pPlot, pAttackerPlot);
			}
		}
	}
	
	// Damage from cities
	for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end(); ++it)
	{
		CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);

		if (pCity && pCity->getTeam() != GET_PLAYER(ePlayer).getTeam())
			iPlotDamage += pCity->rangeCombatDamage(NULL, NULL, false, m_pPlot);
	}

	// Damage from features
	iPlotDamage += GetDamageFromFeatures(ePlayer);

	return iPlotDamage;
}

int CvDangerPlotContents::GetAirUnitDamage(const CvUnit* pUnit, AirActionType iAirAction)
{
	if (pUnit->getDomainType() != DOMAIN_AIR)
		return 0;
	
	if (iAirAction == AIR_ACTION_INTERCEPT) // Max damage from a potential air sweep against our intercept
	{
		int iBestAirSweepDamage = 0;
		int iCurrentAirSweepDamage = 0;
		for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
		{
			CvUnit* pAttacker = GET_PLAYER(it->first).getUnit(it->second);

			if (!pAttacker || !pAttacker->canAirSweep() || pAttacker->isDelayedDeath() || pAttacker->IsDead())
			{
				continue;
			}
			int iAttackerStrength = pAttacker->GetMaxRangedCombatStrength(pUnit, /*pCity*/ NULL, true, false);
			iAttackerStrength *= (100 + pAttacker->GetAirSweepCombatModifier());
			iAttackerStrength /= 100;
			int iDefenderStrength = pUnit->GetMaxRangedCombatStrength(pUnit, /*pCity*/ NULL, false, false);
			iCurrentAirSweepDamage = pUnit->getCombatDamage(iDefenderStrength, iAttackerStrength,
				pUnit->getDamage(), /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);

			// It's a slower to have this in the unit loop instead of after the best damage has been calculated, but it's also more accurate
			if (iCurrentAirSweepDamage >= pAttacker->GetCurrHitPoints())
			{
				int iReceiverDamage = pAttacker->getCombatDamage(iAttackerStrength, iDefenderStrength,
					pAttacker->getDamage(), /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);
				if (iReceiverDamage >= pUnit->GetCurrHitPoints())
				{
					if (iReceiverDamage + pUnit->getDamage() > iCurrentAirSweepDamage + pAttacker->getDamage())
					{
						iCurrentAirSweepDamage = pUnit->GetCurrHitPoints() - 1;
					}
				}
			}
			if (iCurrentAirSweepDamage > iBestAirSweepDamage)
			{
				iBestAirSweepDamage = iCurrentAirSweepDamage;
			}
		}
		return iBestAirSweepDamage;
	}
	else
	{
		CvUnit* pInterceptor = pUnit->GetBestInterceptor(*m_pPlot);
		if (pInterceptor)
		{
			// Air sweeps take modified damage from interceptors
			if (iAirAction == AIR_ACTION_SWEEP)
			{
				if (pInterceptor->getDomainType() != DOMAIN_AIR)
				{
					return (pInterceptor->GetInterceptionDamage(pUnit, false) * (100+GC.getAIR_SWEEP_INTERCEPTION_DAMAGE_MOD()))/100;
				}
				else
				{
					int iAttackerStrength = pUnit->GetMaxRangedCombatStrength(pInterceptor, /*pCity*/ NULL, true, false);
					iAttackerStrength *= (100 + pUnit->GetAirSweepCombatModifier());
					iAttackerStrength /= 100;
					int iDefenderStrength = pInterceptor->GetMaxRangedCombatStrength(pUnit, /*pCity*/ NULL, false, false);
					int iReceiveDamage = pInterceptor->getCombatDamage(iDefenderStrength, iAttackerStrength,
						pInterceptor->getDamage(), /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);
					if (iReceiveDamage >= pUnit->GetCurrHitPoints())
					{
						int iDamageDealt = pUnit->getCombatDamage(iAttackerStrength, iDefenderStrength,
							pUnit->getDamage(), /*bIncludeRand*/ false, /*bAttackerIsCity*/ false, /*bDefenderIsCity*/ false);
						if (iDamageDealt >= pInterceptor->GetCurrHitPoints())
						{
							if (iDamageDealt + pInterceptor->getDamage() > iReceiveDamage + pUnit->getDamage())
							{
								iReceiveDamage = pUnit->GetCurrHitPoints() - 1;
							}
						}
					}
					return iReceiveDamage;
				}
			}
			else
			{
				// Always assume interception is successful
				return pInterceptor->GetInterceptionDamage(pUnit, false);
			}
		}
	}

	return 0;
}

// Get the maximum damage unit could receive at this plot in the next turn (update this with CvUnitCombat changes!)
int CvDangerPlotContents::GetDanger(const CvUnit* pUnit, AirActionType iAirAction)
{
	if (!m_pPlot || !pUnit)
		return 0;

	// Air units only take damage from interceptions
	if (pUnit->getDomainType() == DOMAIN_AIR)
		return GetAirUnitDamage(pUnit, iAirAction);

	//simple caching for speedup
	SUnitInfo unitStats(pUnit);
	for (size_t i=0; i<m_lastResults.size(); i++)
		if ( unitStats == m_lastResults[i].first )
			return m_lastResults[i].second;

	//otherwise calculate from scratch
	int iPlotDamage = 0;

	CvCity* pFriendlyCity = NULL;
	if ( m_pPlot->isFriendlyCity(*pUnit,true) )
		pFriendlyCity = m_pPlot->getPlotCity();

	// Civilians can be captured - unless they would need to be embarked on this plot
	if (!pUnit->IsCombatUnit() && pUnit->isNativeDomain(m_pPlot))
	{
		// If plot contains an enemy unit, mark it as max danger
		if (m_pPlot->getBestDefender(NO_PLAYER, pUnit->getOwner(), NULL, true))
		{
			return MAX_INT;
		}

		for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
		{
			CvUnit* pAttacker = GET_PLAYER(it->first).getUnit(it->second);

			if ( pAttacker && !pAttacker->isDelayedDeath() && !pAttacker->IsDead() )
			{
				// If in a city and the city can be captured, we are in highest danger
				if (pFriendlyCity)
				{
					if (GetDanger(pFriendlyCity) + pFriendlyCity->getDamage() > pFriendlyCity->GetMaxHitPoints())
					{
						return MAX_INT;
					}
				}
				// Look for a possible plot defender
				else 
				{
					IDInfo* pUnitNode = m_pPlot->headUnitNode();
					CvUnit* pBestDefender = NULL;
					while (pUnitNode != NULL)
					{
						pBestDefender = ::getUnit(*pUnitNode);
						pUnitNode = m_pPlot->nextUnitNode(pUnitNode);

						if (pBestDefender && pBestDefender->getOwner() == pUnit->getOwner())
						{
							//fix endless recursion with stacked embarked civilians: defender must also be able to attack
							if (pBestDefender->IsCanDefend() && pBestDefender->IsCanAttack())
							{
								if (pBestDefender != pUnit)
								{
									if (pBestDefender->isWaiting() || !(pBestDefender->canMove()))
									{
										break;
									}
								}
							}
						}
						pBestDefender = NULL;
					}
					// If there is a defender and it might be killed, high danger
					if (pBestDefender && (pBestDefender->isWaiting() || !pBestDefender->canMove()))
					{
						if (GetDanger(pBestDefender) > pBestDefender->GetCurrHitPoints())
						{
							return INT_MAX;
						}
					}
					else if (pBestDefender==NULL)
					{
						//Civilian could be captured on this tile
						if (pAttacker->isNativeDomain(m_pPlot))
							return MAX_INT;
						else
						{
							int iAttackerDamage = 0; //ignore this
							if (pAttacker->plot() != m_pPlot)
								iPlotDamage += TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pUnit,pAttacker,m_pPlot,pAttacker->plot(),iAttackerDamage);
						}
					}
				}
			}
		}

		// Damage from features (citadel)
		iPlotDamage += GetDamageFromFeatures(pUnit->getOwner());
		iPlotDamage += m_bFlatPlotDamage ? m_pPlot->getTurnDamage(pUnit->ignoreTerrainDamage(), pUnit->ignoreFeatureDamage(), pUnit->extraTerrainDamage(), pUnit->extraFeatureDamage()) : 0;

		// Damage from cities
		for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end(); ++it)
		{
			CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);
			if (!pCity || pCity->getTeam() == pUnit->getTeam())
				continue;

			iPlotDamage += pCity->rangeCombatDamage(pUnit, NULL, false, m_pPlot);
		}

		//update cache
		m_lastResults.push_back( std::make_pair(unitStats,iPlotDamage) );
		if (m_lastResults.size()==DANGER_MAX_CACHE_SIZE)
			m_lastResults.erase(m_lastResults.begin());

		return iPlotDamage;
	}

	// Capturing a city with a garrisoned unit destroys the garrisoned unit
	if (pFriendlyCity)
	{
		int iCityDanger = GetDanger(pFriendlyCity, (pUnit->getDomainType() == DOMAIN_LAND ? pUnit : NULL));
		if (iCityDanger + pFriendlyCity->getDamage() < pFriendlyCity->GetMaxHitPoints() + 50) //add a margin for error
		{
			if (pUnit->CanGarrison())
			{
				// Reconstruct the amount of damage the garrison would absorb for the city
				int iUnitShare = (iCityDanger*2*pUnit->GetMaxHitPoints()) / pFriendlyCity->GetMaxHitPoints();

				// Damage from features
				return iUnitShare + GetDamageFromFeatures(pUnit->getOwner());
			}
			else
				return 0;
		}
		else
		{
			return MAX_INT;
		}
	}

	// Damage from units
	// EXTREMELY IMPORTANT THAT NO RNG IS USED FOR PREDICTION!
	// Otherwise a tooltip or similar can change the game state
	for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
	{
		CvUnit* pAttacker = GET_PLAYER(it->first).getUnit(it->second);
		if (!pAttacker || pAttacker->isDelayedDeath() || pAttacker->IsDead())
			continue;

		int iAttackerDamage = 0; //ignore this
		if (pAttacker->plot() != m_pPlot)
			iPlotDamage += TacticalAIHelpers::GetSimulatedDamageFromAttackOnUnit(pUnit,pAttacker,m_pPlot,pAttacker->plot(),iAttackerDamage);
	}

	// Damage from cities
	for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end(); ++it)
	{
		CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);
		if (!pCity || pCity->getTeam() == pUnit->getTeam())
			continue;

		iPlotDamage += pCity->rangeCombatDamage(pUnit, NULL, false, m_pPlot);
	}

	// Damage from surrounding features (citadel) and the plot itself
	iPlotDamage += GetDamageFromFeatures(pUnit->getOwner());
	iPlotDamage += m_bFlatPlotDamage ? m_pPlot->getTurnDamage(pUnit->ignoreTerrainDamage(), pUnit->ignoreFeatureDamage(), pUnit->extraTerrainDamage(), pUnit->extraFeatureDamage()) : 0;

	//update cache
	m_lastResults.push_back( std::make_pair(unitStats,iPlotDamage) );
	if (m_lastResults.size()==DANGER_MAX_CACHE_SIZE)
		m_lastResults.erase(m_lastResults.begin());

	//done
	return iPlotDamage;
}

// Get the maximum damage city could receive this turn if it were in this plot
int CvDangerPlotContents::GetDanger(CvCity* pCity, const CvUnit* pPretendGarrison)
{
	if (!m_pPlot || !pCity)
		return 0;

	int iPlotDamage = 0;
	CvPlot* pCityPlot = pCity->plot();
	const int iCityX = pCityPlot->getX();
	const int iCityY = pCityPlot->getY();

	CvCityGarrisonOverride guard(pCity,pPretendGarrison);

	CvPlot* pAttackerPlot = NULL;
	CvUnit* pInterceptor = NULL;

	// Damage from ranged units and melees that cannot capture 
	for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
	{
		CvUnit* pUnit = GET_PLAYER(it->first).getUnit(it->second);
		if (!pUnit || pUnit->isDelayedDeath() || pUnit->IsDead())
		{
			continue;
		}

		pAttackerPlot = NULL;
		
		if (pUnit->IsCanAttackRanged())
		{
			if (pUnit->getDomainType() == DOMAIN_AIR)
			{
				pInterceptor = pUnit->GetBestInterceptor(*m_pPlot);
				int iInterceptDamage = 0;
				if (pInterceptor)
				{
					// Always assume interception is successful
					iInterceptDamage = pInterceptor->GetInterceptionDamage(pUnit, false);
				}
				iPlotDamage += pUnit->GetAirCombatDamage(NULL, pCity, false, iInterceptDamage, m_pPlot);
			}
			else
			{
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false, 0, m_pPlot);
			}
		}
		else if (pUnit->isNoCapture())
		{
			if (plotDistance(iCityX, iCityY, pUnit->getX(), pUnit->getY()) == 1)
			{
				pAttackerPlot = pUnit->plot();
			}
			iPlotDamage += pUnit->getCombatDamage(pUnit->GetMaxAttackStrength(pAttackerPlot, pCityPlot, NULL),
				pCity->getStrengthValue(), pUnit->getDamage(), false, false, true);
			
			if (pUnit->isRangedSupportFire())
			{
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false, 0, pCityPlot);
			}
		}
	}

	// Damage from cities
	for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end(); ++it)
	{
		CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);

		if (!pCity || pCity->getTeam() == pCity->getTeam())
		{
			continue;
		}

		iPlotDamage += pCity->rangeCombatDamage(NULL, pCity, false, pCityPlot);
	}

	// Damage from melee units
	for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
	{
		CvUnit* pUnit = GET_PLAYER(it->first).getUnit(it->second);
		if (!pUnit || pUnit->isDelayedDeath() || pUnit->IsDead())
		{
			continue;
		}

		pAttackerPlot = NULL;

		if (!pUnit->IsCanAttackRanged() && !pUnit->isNoCapture())
		{
			if (plotDistance(iCityX, iCityY, pUnit->getX(), pUnit->getY()) == 1)
			{
				pAttackerPlot = pUnit->plot();
			}

			iPlotDamage += pUnit->getCombatDamage(pUnit->GetMaxAttackStrength(pAttackerPlot, pCityPlot, NULL),
				pCity->getStrengthValue(), pUnit->getDamage(), false, false, true);

			if (pUnit->isRangedSupportFire())
			{
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false, 0, pCityPlot);
			}
		}
	}

	//if we have a garrison, split the damage
	CvUnit* pGarrison = pCity->GetGarrisonedUnit();
	if (pGarrison)
	{
		int iUnitShare = (iPlotDamage*2*pGarrison->GetMaxHitPoints())/(pCity->GetMaxHitPoints()+2*pGarrison->GetMaxHitPoints());
		iPlotDamage -= iUnitShare;
	}

	return iPlotDamage;
}

// Get the amount of damage a citadel would deal to a unit
int CvDangerPlotContents::GetDamageFromFeatures(PlayerTypes ePlayer) const
{
	if (m_pCitadel && ePlayer != NO_PLAYER)
	{
		ImprovementTypes eImprovement = m_pCitadel->getImprovementType();
		CvTeam& kTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam());

		// Citadel still here and can fire?
		if (eImprovement != NO_IMPROVEMENT && !m_pCitadel->IsImprovementPillaged() && m_pCitadel->getOwner() != NO_PLAYER &&
			kTeam.isAtWar(m_pCitadel->getTeam()))
		{
			return GC.getImprovementInfo(eImprovement)->GetNearbyEnemyDamage();
		}
	}

	return 0;
};

std::vector<CvUnit*> CvDangerPlotContents::GetPossibleAttackers() const
{
	//ignore cities
	std::vector<CvUnit*> vResult;
	for (DangerUnitVector::const_iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
	{
		CvUnit* pAttacker = GET_PLAYER(it->first).getUnit(it->second);
		if (pAttacker)
			vResult.push_back(pAttacker);
	}

	return vResult;
}

