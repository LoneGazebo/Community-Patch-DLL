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
	m_fMinorNeutralrMod = GC.getAI_DANGER_MINOR_APPROACH_NEUTRAL();
	m_fMinorFriendlyMod = GC.getAI_DANGER_MINOR_APPROACH_FRIENDLY();
	m_fMinorBullyMod = GC.getAI_DANGER_MINOR_APPROACH_BULLY();
	m_fMinorConquestMod = GC.getAI_DANGER_MINOR_APPROACH_CONQUEST();

#ifdef AUI_DANGER_PLOTS_REMADE
	m_DangerPlots = NULL;
#endif
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
#ifdef AUI_DANGER_PLOTS_REMADE
		m_DangerPlots = FNEW(CvDangerPlotContents[iGridSize], c_eCiv5GameplayDLL, 0);
#else
		m_DangerPlots.resize(iGridSize);
#endif
		m_bArrayAllocated = true;
		for(int i = 0; i < iGridSize; i++)
		{
#ifdef AUI_DANGER_PLOTS_REMADE
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(i);
			m_DangerPlots[i].init(*pPlot);
#else
			m_DangerPlots[i] = 0;
#endif // AUI_DANGER_PLOTS_REMADE
		}
	}
}

/// Uninitialize
void CvDangerPlots::Uninit()
{
	m_ePlayer = NO_PLAYER;
#ifdef AUI_DANGER_PLOTS_REMADE
	SAFE_DELETE_ARRAY(m_DangerPlots);
#else
	m_DangerPlots.clear();
#endif
	m_bArrayAllocated = false;
	m_bDirty = false;
}

//helper function for city threat calculation
int SimulateDamageFromAttackOnCity(CvCity* pCity, const CvUnit* pUnit)
{
	if (!pUnit || !pCity || pUnit->isDelayedDeath() || pUnit->IsDead())
		return 0;
		
	int iDamage = 0;
	if (pUnit->IsCanAttackRanged())
	{
		if (pUnit->getDomainType() == DOMAIN_AIR)
			iDamage += pUnit->GetAirCombatDamage(NULL, pCity, false);
		else
			iDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false);
	}
	else
	{
		if (pUnit->isRangedSupportFire())
			iDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false);

		//just assume the unit can attack from it's current location - modifiers might be different, but thats acceptable
		iDamage += pUnit->getCombatDamage(pUnit->GetMaxAttackStrength(pUnit->plot(), pCity->plot(), NULL),
			pCity->getStrengthValue(), pUnit->getDamage(), false, false, true);

	}

	return iDamage;
}

/// Updates the danger plots values to reflect threats across the map
void CvDangerPlots::UpdateDanger(bool bPretendWarWithAllCivs, bool bIgnoreVisibility)
{
	// danger plots have not been initialized yet, so no need to update
	if(!m_bArrayAllocated)
	{
		return;
	}

	// wipe out values
	int iGridSize = GC.getMap().numPlots();
	CvAssertMsg(iGridSize == m_DangerPlots.size(), "iGridSize does not match number of DangerPlots");
	for(int i = 0; i < iGridSize; i++)
	{
#ifdef AUI_DANGER_PLOTS_REMADE
		m_DangerPlots[i].clear();
#else
		m_DangerPlots[i] = 0;
#endif // AUI_DANGER_PLOTS_REMADE
	}

#ifdef AUI_DANGER_PLOTS_REMADE
	UnitSet previousKnownUnits = m_knownUnits;
	m_knownUnits.clear();
#endif

	CvPlayer& thisPlayer = GET_PLAYER(m_ePlayer);
	TeamTypes thisTeam = thisPlayer.getTeam();

	// for each opposing civ
	for(int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iPlayer;
		CvPlayer& loopPlayer = GET_PLAYER(ePlayer);
		TeamTypes eTeam = loopPlayer.getTeam();

		if(!loopPlayer.isAlive())
		{
			continue;
		}

		if(eTeam == thisTeam)
		{
			continue;
		}

		if(ShouldIgnorePlayer(ePlayer) && !bPretendWarWithAllCivs)
		{
			continue;
		}

		//for each unit
		int iLoop;
		CvUnit* pLoopUnit = NULL;
		for(pLoopUnit = loopPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = loopPlayer.nextUnit(&iLoop))
		{
			if(ShouldIgnoreUnit(pLoopUnit, bIgnoreVisibility))
			{
				continue;
			}

			int iRange = pLoopUnit->baseMoves();
#ifdef AUI_ASTAR_ROAD_RANGE
			IncreaseMoveRangeForRoads(pLoopUnit, iRange);
#endif

			if(pLoopUnit->canRangeStrike())
			{
				iRange += pLoopUnit->GetRange() - 1;
			}

			CvPlot* pUnitPlot = pLoopUnit->plot();
#ifndef AUI_DANGER_PLOTS_REMADE
			AssignUnitDangerValue(pLoopUnit, pUnitPlot);
#else
			//store the units which are being aggregated
			m_knownUnits.insert( std::make_pair(ePlayer,pLoopUnit->GetID()) );
#endif
			CvPlot* pLoopPlot = NULL;

			for(int iDX = -(iRange); iDX <= iRange; iDX++)
			{
				for(int iDY = -(iRange); iDY <= iRange; iDY++)
				{
					pLoopPlot = plotXYWithRangeCheck(pUnitPlot->getX(), pUnitPlot->getY(), iDX, iDY, iRange);
					if(!pLoopPlot || pLoopPlot == pUnitPlot)
					{
						continue;
					}

#ifndef  AUI_DANGER_PLOTS_REMADE
					if(!pLoopUnit->canMoveOrAttackInto(*pLoopPlot) && !pLoopUnit->canRangeStrikeAt(pLoopPlot->getX(),pLoopPlot->getY()))
					{
						continue;
					}
#endif //  AUI_DANGER_PLOTS_REMADE

					AssignUnitDangerValue(pLoopUnit, pLoopPlot);
				}
			}
		}

		// for each city
		CvCity* pLoopCity;
		for(pLoopCity = loopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = loopPlayer.nextCity(&iLoop))
		{
			if(ShouldIgnoreCity(pLoopCity, bIgnoreVisibility))
			{
				continue;
			}

#if defined(MOD_EVENTS_CITY_BOMBARD)
			int iRange = pLoopCity->getBombardRange();
#else
			int iRange = GC.getCITY_ATTACK_RANGE();
#endif
			CvPlot* pCityPlot = pLoopCity->plot();
#ifndef AUI_DANGER_PLOTS_REMADE
			AssignCityDangerValue(pLoopCity, pCityPlot);
#endif // AUI_DANGER_PLOTS_REMADE
			CvPlot* pLoopPlot = NULL;

			for(int iDX = -(iRange); iDX <= iRange; iDX++)
			{
				for(int iDY = -(iRange); iDY <= iRange; iDY++)
				{
					pLoopPlot = plotXYWithRangeCheck(pCityPlot->getX(), pCityPlot->getY(), iDX, iDY, iRange);
					if(!pLoopPlot)
					{
						continue;
					}
#ifdef AUI_DANGER_PLOTS_REMADE
					if (pLoopPlot == pCityPlot)
					{
						continue;
					}
#endif // AUI_DANGER_PLOTS_REMADE

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
			{
				int iRange = pVanishedUnit->baseMoves();
#ifdef AUI_ASTAR_ROAD_RANGE
				IncreaseMoveRangeForRoads(pVanishedUnit, iRange);
#endif
				if(pVanishedUnit->canRangeStrike())
					iRange += pVanishedUnit->GetRange()-1;

				CvPlot* pLoopPlot = NULL;
				CvPlot* pUnitPlot = pVanishedUnit->plot();
				for(int iDX = -iRange; iDX <= iRange; iDX++)
				{
					for(int iDY = -(iRange); iDY <= iRange; iDY++)
					{
						pLoopPlot = plotXYWithRangeCheck(pUnitPlot->getX(), pUnitPlot->getY(), iDX, iDY, iRange);
						if(!pLoopPlot || pLoopPlot == pUnitPlot)
							continue;
						AssignUnitDangerValue(pVanishedUnit, pLoopPlot);
					}
				}
			}
		}
	}

	// Citadels
#ifndef AUI_DANGER_PLOTS_REMADE
	int iCitadelValue = GetDangerValueOfCitadel();
#endif // AUI_DANGER_PLOTS_REMAD
	int iPlotLoop;
	CvPlot* pPlot, *pAdjacentPlot;
	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		if(pPlot->isRevealed(thisTeam))
		{
#ifdef AUI_DANGER_PLOTS_REMADE
			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				m_DangerPlots[iPlotLoop].m_iFlatPlotDamage = (GC.getFeatureInfo(pPlot->getFeatureType())->getTurnDamage());
			}
#endif // AUI_DANGER_PLOTS_REMADE
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
#ifdef AUI_DANGER_PLOTS_REMADE
							m_DangerPlots[iPlotLoop].m_pCitadel = pPlot;
#else
							AddDanger(pAdjacentPlot->getX(), pAdjacentPlot->getY(), iCitadelValue, true);
#endif // AUI_DANGER_PLOTS_REMADE
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
#ifdef AUI_DANGER_PLOTS_REMADE
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
					const UnitHandle pEnemy = pEvalPlot->getBestDefender(NO_PLAYER, thisPlayer.GetID(), NULL, true);
					if (pEnemy)
						iThreatValue += SimulateDamageFromAttackOnCity(pLoopCity,pEnemy.pointer());
				}
			}
#else
		int iThreatValue = GetCityDanger(pLoopCity);
#endif

		pLoopCity->SetThreatValue(iThreatValue);
	}

	m_bDirty = false;
}

#ifndef AUI_DANGER_PLOTS_REMADE
/// Add an amount of danger to a given tile
void CvDangerPlots::AddDanger(int iPlotX, int iPlotY, int iValue, bool bWithinOneMove)
{
	const int idx = iPlotX + iPlotY * GC.getMap().getGridWidth();
	if (iValue > 0)
	{
		if (bWithinOneMove)
		{
			iValue |= 0x1;
		}
		else
		{
			iValue &= ~0x1;
		}
	}

	m_DangerPlots[idx] += iValue;
}
#endif // AUI_DANGER_PLOTS_REMADE

#ifdef AUI_DANGER_PLOTS_REMADE
/// Return the maximum amount of damage that could be dealt to a non-specific unit at this plot
int CvDangerPlots::GetDanger(const CvPlot& pPlot, PlayerTypes ePlayer)
{
	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	return m_DangerPlots[idx].GetDanger(ePlayer);
}

/// Return the maximum amount of damage a city could take at this plot
int CvDangerPlots::GetDanger(const CvPlot& pPlot, CvCity* pCity, CvUnit* pPretendGarrison, int iAfterNIntercepts)
{
	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	if (pCity != NULL)
	{
		return m_DangerPlots[idx].GetDanger(pCity, pPretendGarrison, iAfterNIntercepts);
	}
	return m_DangerPlots[idx].GetDanger(NO_PLAYER);
}

/// Return the maximum amount of damage a unit could take at this plot
int CvDangerPlots::GetDanger(const CvPlot& pPlot, CvUnit* pUnit, int iAirAction, int iAfterNIntercepts)
#else
/// Return the danger value of a given plot
int CvDangerPlots::GetDanger(const CvPlot& pPlot) const
#endif // AUI_DANGER_PLOTS_REMADE
{
	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
#ifdef AUI_DANGER_PLOTS_REMADE
	if (pUnit)
	{
		return m_DangerPlots[idx].GetDanger(pUnit, iAirAction, iAfterNIntercepts);
	}
	return m_DangerPlots[idx].GetDanger(NO_PLAYER);
#else
	return m_DangerPlots[idx];
#endif // AUI_DANGER_PLOTS_REMADE
}

#ifdef AUI_DANGER_PLOTS_REMADE
/// Returns if the tile is in danger
bool CvDangerPlots::IsUnderImmediateThreat(const CvPlot& pPlot, PlayerTypes ePlayer)
{
	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	return m_DangerPlots[idx].IsUnderImmediateThreat(ePlayer);
}

/// Returns if the unit is in immediate danger
bool CvDangerPlots::IsUnderImmediateThreat(const CvPlot& pPlot, CvUnit* pUnit)
{
	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	if (pUnit)
	{
		return m_DangerPlots[idx].GetDanger(pUnit);
	}
	return m_DangerPlots[idx].GetDanger(NO_PLAYER);
}

/// Returns if the unit is in immediate danger
bool CvDangerPlots::CouldAttackHere(const CvPlot& pPlot, CvUnit* pUnit)
{
	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	if (!pUnit)
	{
		return false;
	}
	return m_DangerPlots[idx].CouldAttackHere(pUnit);
}

/// Returns if the unit is in immediate danger
bool CvDangerPlots::CouldAttackHere(const CvPlot& pPlot, CvCity* pCity)
{
	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	if (!pCity)
	{
		return false;
	}
	return m_DangerPlots[idx].CouldAttackHere(pCity);
}

#else

/// Returns if the unit is in immediate danger
bool CvDangerPlots::IsUnderImmediateThreat(const CvPlot& pPlot) const
{
	return GetDanger(pPlot) & 0x1;
}

/// Sums the danger values of the plots around the city to determine the danger value of the city
int CvDangerPlots::GetCityDanger(CvCity* pCity)
{
	CvAssertMsg(pCity, "pCity is null");
	if(!pCity) return 0;

	CvAssertMsg(pCity->getOwner() == m_ePlayer, "City does not belong to us");

	CvPlot* pPlot = pCity->plot();
	int iEvalRange = GC.getAI_DIPLO_PLOT_RANGE_FROM_CITY_HOME_FRONT();

	int iDangerValue = 0;

	for(int iX = -iEvalRange; iX <= iEvalRange; iX++)
	{
		for(int iY = -iEvalRange; iY <= iEvalRange; iY++)
		{
			CvPlot* pEvalPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iX, iY, iEvalRange);
			if(!pEvalPlot)
			{
				continue;
			}

			iDangerValue += GetDanger(*pEvalPlot);
		}
	}

	return iDangerValue;
}

int CvDangerPlots::ModifyDangerByRelationship(PlayerTypes ePlayer, CvPlot* pPlot, int iDanger)
{
	CvAssertMsg(pPlot, "No plot passed in?");
	bool bIgnoreInFriendlyTerritory = false;
	int iResult = iDanger;

	// Full value if a player we're at war with
	if(GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
	{
		return iResult;
	}

	// if it's a human player, ignore neutral units
	if(GET_PLAYER(m_ePlayer).isHuman())
	{
		return 0;
	}

	if(GET_PLAYER(m_ePlayer).isMinorCiv())  // if the evaluator is a minor civ
	{
		if(!GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))  // and they're not at war with the other player
		{
			bIgnoreInFriendlyTerritory = true; // ignore friendly territory
		}
	}
	else if(!GET_PLAYER(ePlayer).isMinorCiv())
	{
		// should we be using bHideTrueFeelings?
		switch(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false))
		{
		case MAJOR_CIV_APPROACH_WAR:
			iResult = (int)(iResult * m_fMajorWarMod);
			break;
		case MAJOR_CIV_APPROACH_HOSTILE:
			iResult = (int)(iResult * m_fMajorHostileMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_DECEPTIVE:
			iResult = (int)(iResult * m_fMajorDeceptiveMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_GUARDED:
			iResult = (int)(iResult * m_fMajorGuardedMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_AFRAID:
			iResult = (int)(iResult * m_fMajorAfraidMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_FRIENDLY:
			iResult = (int)(iResult * m_fMajorFriendlyMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MAJOR_CIV_APPROACH_NEUTRAL:
			iResult = (int)(iResult * m_fMajorNeutralMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		}
	}
	else
	{
		switch(GET_PLAYER(m_ePlayer).GetDiplomacyAI()->GetMinorCivApproach(ePlayer))
		{
		case MINOR_CIV_APPROACH_IGNORE:
			iResult = (int)(iResult * m_fMinorNeutralrMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MINOR_CIV_APPROACH_FRIENDLY:
			iResult = (int)(iResult * m_fMinorFriendlyMod);
			bIgnoreInFriendlyTerritory = true;
			break;
		case MINOR_CIV_APPROACH_BULLY:
			iResult = (int)(iResult * m_fMinorBullyMod);
			break;
		case MINOR_CIV_APPROACH_CONQUEST:
			iResult = (int)(iResult * m_fMinorConquestMod);
			break;
		}
	}

	// if the plot is in our own territory and, with the current approach, we should ignore danger values in our own territory
	// zero out the value
	if(pPlot && pPlot->getOwner() == m_ePlayer && bIgnoreInFriendlyTerritory)
	{
		iResult = 0;
	}

	return iResult;
}
#endif // AUI_DANGER_PLOTS_REMADE

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
			bResultMultiplierIsZero = m_fMinorNeutralrMod == 0.f;
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

	return false;
}

/// Should this unit be ignored when creating the danger plots?
bool CvDangerPlots::ShouldIgnoreUnit(CvUnit* pUnit, bool bIgnoreVisibility)
{
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

#ifdef AUI_DANGER_PLOTS_REMADE
	if (pUnit->getDomainType() == DOMAIN_AIR)
	{
		return true;
	}
#endif // AUI_DANGER_PLOTS_REMADE

#ifdef AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG
	if (!GET_PLAYER(m_ePlayer).isMinorCiv() && !GET_PLAYER(m_ePlayer).isBarbarian() && pUnit->isBarbarian() && pUnit->plot()->isRevealed(GET_PLAYER(m_ePlayer).getTeam()))
#ifdef AUI_DANGER_PLOTS_REMADE
		if (pUnit->plot()->isAdjacentVisible(GET_PLAYER(m_ePlayer).getTeam()))
#endif // AUI_DANGER_PLOTS_REMADE
		bIgnoreVisibility = true;
#endif // AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG
#ifdef AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS
	if (GET_PLAYER(m_ePlayer).isMinorCiv() && !GET_PLAYER(pUnit->getOwner()).isMinorCiv() && !pUnit->isBarbarian() && 
		GET_PLAYER(m_ePlayer).GetClosestFriendlyCity(*pUnit->plot(), AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS))
		bIgnoreVisibility = true;
#endif // AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS

#ifdef AUI_DANGER_PLOTS_FIX_SHOULD_IGNORE_UNIT_IGNORE_VISIBILITY_PLOT
	if(!pUnit->plot()->isVisible(GET_PLAYER(m_ePlayer).getTeam()) && !bIgnoreVisibility)
#else
	if(!pUnit->plot()->isVisible(GET_PLAYER(m_ePlayer).getTeam()))
#endif // AUI_DANGER_PLOTS_FIX_SHOULD_IGNORE_UNIT_IGNORE_VISIBILITY_PLOT
	{
		return true;
	}

#if !defined(AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG) && !defined(AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MINORS_SEE_MAJORS)
	if(pUnit->isInvisible(GET_PLAYER(m_ePlayer).getTeam(), false))
	{
		return true;
	}
#endif // AUI_DANGER_PLOTS_SHOULD_IGNORE_UNIT_MAJORS_SEE_BARBARIANS_IN_FOG

	CvPlot* pPlot = pUnit->plot();
	CvAssertMsg(pPlot, "Plot is null?")

	if(NULL != pPlot && !pPlot->isVisibleOtherUnit(m_ePlayer) && !bIgnoreVisibility)
	{
		return true;
	}

#ifndef AUI_DANGER_PLOTS_REMADE
	// fix post-gold!
	if(pUnit->getDomainType() == DOMAIN_AIR)
	{
		return true;
	}
#endif // AUI_DANGER_PLOTS_REMADE

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
#ifdef AUI_DANGER_PLOTS_REMADE
void CvDangerPlots::AssignUnitDangerValue(CvUnit* pUnit, CvPlot* pPlot, bool bReuse)
{
	if (IsDangerByRelationshipZero(pUnit->getOwner(), pPlot))
		return;

	const int iPlotX = pPlot->getX();
	const int iPlotY = pPlot->getY();
	const int idx = GC.getMap().plotNum(iPlotX, iPlotY);
	
	// Check to see if we're already in this plot
	if (pUnit->plot() == pPlot)
	{
		m_DangerPlots[idx].m_apUnits.push_back( std::make_pair(pUnit->getOwner(),pUnit->GetID()) );
		m_DangerPlots[idx].m_apMoveOnlyUnits.push_back( std::make_pair(pUnit->getOwner(),pUnit->GetID()) );
		return;
	}

	// Check to see if another player has already done the pathfinding for us
	DangerPlotList& vpUnitDangerPlotList = pUnit->GetDangerPlotList(false);
	DangerPlotList& vpUnitDangerPlotMoveOnlyList = pUnit->GetDangerPlotList(true);
	bool bInList = pPlot->getNumTimesInList(vpUnitDangerPlotList, true) > 0;
	bool bInMoveOnlyList = pPlot->getNumTimesInList(vpUnitDangerPlotMoveOnlyList, true) > 0;

	if (bInList)
		m_DangerPlots[idx].m_apUnits.push_back( std::make_pair(pUnit->getOwner(),pUnit->GetID()) );

	if (bInMoveOnlyList)
		m_DangerPlots[idx].m_apMoveOnlyUnits.push_back( std::make_pair(pUnit->getOwner(),pUnit->GetID()) );

	if (bInList || bInMoveOnlyList)
		return;

	CvIgnoreUnitsPathFinder& kPathFinder = GC.getIgnoreUnitsPathFinder();
#ifdef AUI_ASTAR_TURN_LIMITER
	kPathFinder.SetData(pUnit, 1);
#else
	kPathFinder.SetData(pUnit);
#endif // AUI_ASTAR_TURN_LIMITER

	CvAStarNode* pNode = NULL;
	int iTurnsAway = MAX_INT;

	if (pUnit->IsCanAttackRanged())
	{
		if (kPathFinder.GeneratePath(pUnit->getX(), pUnit->getY(), iPlotX, iPlotY, MOVE_UNITS_IGNORE_DANGER | MOVE_IGNORE_STACKING, bReuse /*bReuse*/))
		{
			pNode = kPathFinder.GetLastNode();
			if (pNode)
			{
				iTurnsAway = pNode->m_iData2;
			}
		}
		if (iTurnsAway <= 1)
		{
			m_DangerPlots[idx].m_apMoveOnlyUnits.push_back( std::make_pair(pUnit->getOwner(),pUnit->GetID()) );
			vpUnitDangerPlotMoveOnlyList.push_back(std::make_pair(pPlot, true));
		}
		else
		{
			vpUnitDangerPlotMoveOnlyList.push_back(std::make_pair(pPlot, false));
		}
		if (pUnit->canMoveAndRangedStrike(pPlot))
		{
			m_DangerPlots[idx].m_apUnits.push_back( std::make_pair(pUnit->getOwner(),pUnit->GetID()) );
			vpUnitDangerPlotList.push_back(std::make_pair(pPlot, true));
		}
		else
		{
			vpUnitDangerPlotList.push_back(std::make_pair(pPlot, false));
		}
	}
	else
	{
		if (kPathFinder.GeneratePath(pUnit->getX(), pUnit->getY(), iPlotX, iPlotY, MOVE_UNITS_IGNORE_DANGER | MOVE_IGNORE_STACKING, bReuse /*bReuse*/))
		{
			pNode = kPathFinder.GetLastNode();
			if (pNode)
			{
				iTurnsAway = pNode->m_iData2;
			}
		}
		if (iTurnsAway <= 1)
		{
			m_DangerPlots[idx].m_apUnits.push_back( std::make_pair(pUnit->getOwner(),pUnit->GetID()) );
			m_DangerPlots[idx].m_apMoveOnlyUnits.push_back( std::make_pair(pUnit->getOwner(),pUnit->GetID()) );
			vpUnitDangerPlotList.push_back(std::make_pair(pPlot, true));
			vpUnitDangerPlotMoveOnlyList.push_back(std::make_pair(pPlot, true));
		}
		else
		{
			vpUnitDangerPlotList.push_back(std::make_pair(pPlot, false));
			vpUnitDangerPlotMoveOnlyList.push_back(std::make_pair(pPlot, false));
		}
	}
#else
void CvDangerPlots::AssignUnitDangerValue(CvUnit* pUnit, CvPlot* pPlot)
{
	// MAJIK NUMBARS TO MOVE TO XML
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER) && !defined(AUI_DANGER_PLOTS_TWEAKED_RANGED)
	int iTurnsAway = 0;
#endif
	int iCombatValueCalc = 100;
	int iBaseUnitCombatValue = pUnit->GetBaseCombatStrengthConsideringDamage() * iCombatValueCalc;
	// Combat capable?  If not, the calculations will always result in 0, so just skip it.
	if(iBaseUnitCombatValue > 0)
	{
		// Will any danger be zero'ed out?
		if(!IsDangerByRelationshipZero(pUnit->getOwner(), pPlot))
		{
			//int iDistance = plotDistance(pUnitPlot->getX(), pUnitPlot->getY(), pPlot->getX(), pPlot->getY());
			//int iRange = pUnit->baseMoves();
			//FAssertMsg(iRange > 0, "0 range? Uh oh");

			CvIgnoreUnitsPathFinder& kPathFinder = GC.getIgnoreUnitsPathFinder();
			kPathFinder.SetData(pUnit);

			int iPlotX = pPlot->getX();
			int iPlotY = pPlot->getY();
			
#ifdef AUI_DANGER_PLOTS_TWEAKED_RANGED
			int iTurnsAway = MAX_INT;
			// if unit is ranged, different algorithm must be used
			if (pUnit->isRanged())
			{
#ifdef AUI_DANGER_PLOTS_ADD_DANGER_CONSIDER_TERRAIN_STRENGTH_MODIFICATION
				if (pUnit->plot() == pPlot)
					iBaseUnitCombatValue = pUnit->GetMaxDefenseStrength(pUnit->plot(), NULL) * iCombatValueCalc;
				else
					iBaseUnitCombatValue = pUnit->GetMaxRangedCombatStrength(NULL, NULL, true, true) * iCombatValueCalc;
#endif // AUI_DANGER_PLOTS_ADD_DANGER_CONSIDER_TERRAIN_STRENGTH_MODIFICATION
				if (pUnit->canEverRangeStrikeAt(pPlot->getX(), pPlot->getY()))
				{
					iTurnsAway = 1;
				}
			}
#ifdef AUI_DANGER_PLOTS_ADD_DANGER_CONSIDER_TERRAIN_STRENGTH_MODIFICATION
			else
			{
				if (pUnit->plot() == pPlot)
					iBaseUnitCombatValue = pUnit->GetMaxDefenseStrength(pUnit->plot(), NULL) * iCombatValueCalc;
				else if (pUnit->plot()->isAdjacent(pPlot))
					iBaseUnitCombatValue = pUnit->GetMaxAttackStrength(pUnit->plot(), pPlot, NULL) * iCombatValueCalc;
				else
					iBaseUnitCombatValue = pUnit->GetMaxAttackStrength(NULL, pPlot, NULL) * iCombatValueCalc;
			}
#endif // AUI_DANGER_PLOTS_ADD_DANGER_CONSIDER_TERRAIN_STRENGTH_MODIFICATION

			// iTurnsAway is only greater than 1 if unit cannot ranged strike onto the tile
			if (iTurnsAway > 1)
			{
				CvAStarNode* pNode = NULL;
				// can the unit actually walk there
				if (kPathFinder.GeneratePath(pUnit->getX(), pUnit->getY(), iPlotX, iPlotY, MOVE_UNITS_IGNORE_DANGER, true /*bReuse*/))
				{
					pNode = kPathFinder.GetLastNode();
				}
				
				if (pNode)
					iTurnsAway = pNode->m_iData2;
				else
					return;
				if (pUnit->GetRange() > 1)
					iTurnsAway -= 1;
				iTurnsAway += pUnit->getMustSetUpToRangedAttackCount();
				iTurnsAway = MAX(iTurnsAway, 1);
			}
#else
			// can the unit actually walk there
				if(!kPathFinder.GeneratePath(pUnit->getX(), pUnit->getY(), iPlotX, iPlotY, 0, true /*bReuse*/))
				{
					return;
				}

				CvAStarNode* pNode = kPathFinder.GetLastNode();
#if defined(MOD_AI_SMART_FLEE_FROM_DANGER)
				iTurnsAway = (pNode->m_iData2);
#else
				int iTurnsAway = pNode->m_iData2;
#endif
				iTurnsAway = max(iTurnsAway, 1);
#endif // AUI_DANGER_PLOTS_TWEAKED_RANGED

			int iUnitCombatValue = iBaseUnitCombatValue / iTurnsAway;
			iUnitCombatValue = ModifyDangerByRelationship(pUnit->getOwner(), pPlot, iUnitCombatValue);
#if defined(MOD_BALANCE_CORE_MILITARY)
			//Embarked? Scary!
			if(pPlot->isWater() && pUnit->getDomainType() != DOMAIN_SEA)
			{
				iUnitCombatValue *= 3;
				iUnitCombatValue /= 2;
			}
#endif
			AddDanger(iPlotX, iPlotY, iUnitCombatValue, iTurnsAway <= 1);
		}
	}
#endif // AUI_DANGER_PLOTS_REMADE
}

//	-----------------------------------------------------------------------------------------------
/// Contains the calculations to do the danger value for the plot according to the city
void CvDangerPlots::AssignCityDangerValue(CvCity* pCity, CvPlot* pPlot)
{
#ifdef AUI_DANGER_PLOTS_REMADE
	if (IsDangerByRelationshipZero(pCity->getOwner(), pPlot))
		return;

	const int idx = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
	m_DangerPlots[idx].m_apCities.push_back( std::make_pair(pCity->getOwner(),pCity->GetID()) );
#else
	int iCombatValue = pCity->getStrengthValue();
#if defined(MOD_BALANCE_CORE_MILITARY)
	//If city is strong, we should increase its threat.
	CvMilitaryAI* pMilitaryAI = GET_PLAYER(m_ePlayer).GetMilitaryAI();
	int iPower = pMilitaryAI->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);
	if(iCombatValue > iPower)
	{
		iCombatValue *= 2;
	}
	//If plot is water, increase the value. Water is dangerous!
	if(pPlot->isWater())
	{
		iCombatValue *= 3;
		iCombatValue /= 2;
	}
#endif
	iCombatValue = ModifyDangerByRelationship(pCity->getOwner(), pPlot, iCombatValue);
	AddDanger(pPlot->getX(), pPlot->getY(), iCombatValue, false);
#endif // AUI_DANGER_PLOTS_REMADE
}

#ifndef AUI_DANGER_PLOTS_REMADE
/// How much danger should we apply to a citadel?
int CvDangerPlots::GetDangerValueOfCitadel() const
{
	// Compute power of this player's strongest unit
	CvMilitaryAI* pMilitaryAI = GET_PLAYER(m_ePlayer).GetMilitaryAI();
	int iPower = pMilitaryAI->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);

	// Magic number to approximate danger from one turn of citadel damage
	return iPower * 50;
}
#endif

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

#ifdef AUI_DANGER_PLOTS_REMADE
	Uninit();
	m_DangerPlots = FNEW(CvDangerPlotContents[iGridSize], c_eCiv5GameplayDLL, 0);
	m_bArrayAllocated = true;

	for (int i = 0; i < iGridSize; i++)
	{
		kStream >> m_DangerPlots[i];
	}

#else
	m_DangerPlots.resize(iGridSize);
	for(int i = 0; i < iGridSize; i++)
	{
		kStream >> m_DangerPlots[i];
	}
#endif

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
}

//	-----------------------------------------------------------------------------------------------
void CvDangerPlots::SetDirty()
{
	m_bDirty = true;
}

#ifdef AUI_DANGER_PLOTS_REMADE
// Get the maximum damage unit could receive at this plot in the next turn
int CvDangerPlotContents::GetDanger(PlayerTypes ePlayer)
{
	if (!m_pPlot)
		return 0;

	// Damage from terrain
	int iPlotDamage = m_iFlatPlotDamage;

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
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
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
#endif
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
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
			if (pUnit->isRangedSupportFire())
			{
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, NULL, false, 0, m_pPlot, pAttackerPlot);
			}
#endif
		}
	}
	
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	// Damage from cities
	for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end(); ++it)
	{
		CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);

		if (pCity && pCity->getTeam() != GET_PLAYER(ePlayer).getTeam())
			iPlotDamage += pCity->rangeCombatDamage(NULL, NULL, false, m_pPlot);
	}
#endif

	// Damage from features
	iPlotDamage += GetDamageFromFeatures(ePlayer);

	return iPlotDamage;
}

// Get the maximum damage unit could receive at this plot in the next turn (update this with CvUnitCombat changes!)
int CvDangerPlotContents::GetDanger(CvUnit* pUnit, int iAirAction, int iAfterNIntercepts)
{
	if (!m_pPlot || !pUnit)
		return 0;

	// Air units only take damage from interceptions
	if (pUnit->getDomainType() == DOMAIN_AIR)
	{
		if (iAirAction & AIR_ACTION_INTERCEPT) // Max damage from a potential air sweep against our intercept
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
#ifdef AUI_UNIT_GET_NTH_BEST_INTERCEPTOR
			CvUnit* pInterceptor = pUnit->GetNthBestInterceptor(*m_pPlot, iAfterNIntercepts);
#else
			CvUnit* pInterceptor = pUnit->GetBestInterceptor(*m_pPlot);
#endif
			if (pInterceptor)
			{
				// Air sweeps take modified damage from interceptors
				if (iAirAction & AIR_ACTION_SWEEP)
				{
					if (pInterceptor->getDomainType() != DOMAIN_AIR)
					{
						return (pInterceptor->GetInterceptionDamage(pUnit, false) * GC.getAIR_SWEEP_INTERCEPTION_DAMAGE_MOD() / 100);
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

	//simple caching for speedup
	SUnitStats unitStats(pUnit);
	if (unitStats==m_lastUnit)
		return m_lastResult;

	//otherwise calculate from scratch
	int iPlotDamage = 0;
	if (m_iFlatPlotDamage != 0 && (pUnit->atPlot(*m_pPlot) || pUnit->canMoveInto(*m_pPlot)))
	{
		// Damage from plot (no unit in tile)
		iPlotDamage = m_iFlatPlotDamage;
	}
	CvCity* pFriendlyCity = m_pPlot->getPlotCity();
	if (pFriendlyCity && !m_pPlot->isFriendlyCity(*pUnit, false))
		pFriendlyCity = NULL;

	// Civilians can be captured
	if (!pUnit->IsCombatUnit() && (!m_pPlot->isWater() || pUnit->getDomainType() != DOMAIN_LAND || m_pPlot->isValidDomainForAction(*pUnit)))
	{
		// If plot contains an enemy unit, mark it as max danger
		if (m_pPlot->getBestDefender(NO_PLAYER, pUnit->getOwner(),NULL,true))
		{
			return MAX_INT;
		}
		for (DangerUnitVector::iterator it = m_apMoveOnlyUnits.begin(); it < m_apMoveOnlyUnits.end(); ++it)
		{
			CvUnit* pAttacker = GET_PLAYER(it->first).getUnit(it->second);

			if ( pAttacker && !pAttacker->isDelayedDeath() && !pAttacker->IsDead())
			{
				// If in a city and the city will survive all attack, return a danger value of 1
				if (pFriendlyCity)
				{
					if (GetDanger(pFriendlyCity) + pFriendlyCity->getDamage() < pFriendlyCity->GetMaxHitPoints())
					{
						// Trivial amount to indicate that the plot can still be attacked
						++iPlotDamage;
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
					// If have a defender and it will survive all attack, return a danger value of 1
					if (pBestDefender && (pBestDefender->isWaiting() || !pBestDefender->canMove()))
					{
						if (GetDanger(pBestDefender)<pBestDefender->GetCurrHitPoints())
						{
							// Trivial amount to indicate that the plot can still be attacked
							++iPlotDamage;
						}
					}
				}
				// If Civilian would be captured on this tile (only happens if iPlotDamage isn't modified), return MAX_INT
				if (iPlotDamage == m_iFlatPlotDamage)
				{
					return MAX_INT;
				}

				// Damage from features
				iPlotDamage += GetDamageFromFeatures(pUnit->getOwner());

				return iPlotDamage;
			}
		}
	}

	// Garrisoning in a city will have the city's health stats replace the unit's health stats (capturing a city with a garrisoned unit destroys the garrisoned unit)
	if (pFriendlyCity)
	{
		// If the city survives all possible attacks this turn, so will the unit
		if (GetDanger(pFriendlyCity, (pUnit->getDomainType() == DOMAIN_LAND ? pUnit : NULL)) + pFriendlyCity->getDamage() < pFriendlyCity->GetMaxHitPoints())
		{
			// Trivial amount to indicate that the plot can still be attacked
			++iPlotDamage;

			// Damage from features
			iPlotDamage += GetDamageFromFeatures(pUnit->getOwner());

			return iPlotDamage;
		}
		else
		{
			return MAX_INT;
		}
	}

	CvPlot* pAttackerPlot = NULL;
	CvUnit* pInterceptor = NULL;
	// Damage from units
	for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
	{
		CvUnit* pAttacker = GET_PLAYER(it->first).getUnit(it->second);

		if (!pAttacker || pAttacker->isDelayedDeath() || pAttacker->IsDead())
		{
			continue;
		}

		pAttackerPlot = NULL;
		if (pAttacker->plot() != m_pPlot)
		{				
			if (pAttacker->IsCanAttackRanged())
			{
				if (pAttacker->getDomainType() == DOMAIN_AIR)
				{
#ifdef AUI_UNIT_GET_NTH_BEST_INTERCEPTOR
					pInterceptor = pAttacker->GetNthBestInterceptor(*m_pPlot, iAfterNIntercepts, pUnit);
#else
					pInterceptor = pAttacker->GetBestInterceptor(*m_pPlot, pUnit);
#endif
					int iInterceptDamage = 0;
					if (pInterceptor)
					{
						// Always assume interception is successful
						iInterceptDamage = pInterceptor->GetInterceptionDamage(pUnit, false);
						++iAfterNIntercepts;
					}
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
					iPlotDamage += pAttacker->GetAirCombatDamage(pUnit, NULL, false, iInterceptDamage, m_pPlot);
#else
					iPlotDamage += pAttacker->GetAirCombatDamage(pUnit, NULL, false, iInterceptDamage);
#endif
				}
				else
				{
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
					iPlotDamage += pAttacker->GetRangeCombatDamage(pUnit, NULL, false, 0, m_pPlot);
#else
					iPlotDamage += pAttacker->GetRangeCombatDamage(pUnit, NULL, false);
#endif
				}
			}
			else
			{
				if (plotDistance(m_iX, m_iY, pUnit->getX(), pUnit->getY()) == 1)
				{
					pAttackerPlot = pAttacker->plot();
				}
				iPlotDamage += pAttacker->getCombatDamage(
					pAttacker->GetMaxAttackStrength(pAttackerPlot, m_pPlot, pUnit),
					pUnit->GetMaxDefenseStrength(m_pPlot, pAttacker), pUnit->getDamage(), false, false, false);
				if (pAttacker->isRangedSupportFire())
				{
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
					iPlotDamage += pAttacker->GetRangeCombatDamage(pUnit, NULL, false, 0, m_pPlot, pAttackerPlot);
#else
					iPlotDamage += pAttacker->GetRangeCombatDamage(pUnit, NULL, false);
#endif
				}
			}
		}
	}

	// Damage from cities
	for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end(); ++it)
	{
		CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);

		if (!pCity || pCity->getTeam() == pUnit->getTeam())
		{
			continue;
		}

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
		if(pCity->GetID() != -1)
		{
			iPlotDamage += pCity->rangeCombatDamage(pUnit, NULL, false, m_pPlot);
		}
#else
		iPlotDamage += pCity->rangeCombatDamage(pUnit, NULL, false);
#endif
	}

	// Damage from features
	iPlotDamage += GetDamageFromFeatures(pUnit->getOwner());

	//update cache
	m_lastUnit = unitStats;
	m_lastResult = iPlotDamage;

	//done
	return iPlotDamage;
}

// Can this tile be attacked by an enemy unit or city next turn?
bool CvDangerPlotContents::IsUnderImmediateThreat(PlayerTypes ePlayer)
{
	if (!m_pPlot)
		return false;

	// Terrain damage
	if (m_iFlatPlotDamage > 0)
	{
		return true;
	}

	// Cities in range
	for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end(); ++it)
	{
		CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);

		if (pCity && pCity->getTeam() != GET_PLAYER(ePlayer).getTeam())
		{
			return true;
		}
	}

	// Units in range
	for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
	{
		CvUnit* pUnit = GET_PLAYER(it->first).getUnit(it->second);

		if (pUnit && !pUnit->isDelayedDeath() && !pUnit->IsDead())
		{
			return true;
		}
	}

	// Citadel etc in range
	if (GetDamageFromFeatures(ePlayer) > 0)
	{
		return true;
	}

	return false;
}

// Can this tile be attacked by an enemy unit or city next turn?
bool CvDangerPlotContents::IsUnderImmediateThreat(CvUnit* pUnit)
{
	if (!m_pPlot || !pUnit)
		return false;

	// Air units operate off of intercepts instead of units/cities that can attack them
	if (pUnit->getDomainType() == DOMAIN_AIR)
	{
		if (pUnit->GetBestInterceptor(*m_pPlot))
		{
			return true;
		}
	}
	else
	{
		// Cities in range
		for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end(); ++it)
		{
			CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);

			if (pCity && pCity->getTeam() != pUnit->getTeam())
			{
				return true;
			}
		}

		// Units in range
		if (pUnit->IsCombatUnit())
		{
			for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
			{
				CvUnit* pUnit = GET_PLAYER(it->first).getUnit(it->second);
				if (pUnit && !pUnit->isDelayedDeath() && !pUnit->IsDead())
				{
					return true;
				}
			}
		}
		else
		{
			for (DangerUnitVector::iterator it = m_apMoveOnlyUnits.begin(); it < m_apMoveOnlyUnits.end(); ++it)
			{
				CvUnit* pUnit = GET_PLAYER(it->first).getUnit(it->second);
				if (pUnit && !pUnit->isDelayedDeath() && !pUnit->IsDead())
				{
					return true;
				}
			}
		}

		// Citadel etc in range
		if (GetDamageFromFeatures(pUnit->getOwner()) > 0)
		{
			return true;
		}
	}

	// Terrain damage is greater than heal rate
	int iMinimumDamage = m_iFlatPlotDamage;
	if ((pUnit->plot() == m_pPlot && !pUnit->isEmbarked()) ||
		(pUnit->isAlwaysHeal() && (!m_pPlot->isWater() || pUnit->getDomainType() != DOMAIN_LAND || m_pPlot->isValidDomainForAction(*pUnit))))
	{
		iMinimumDamage -= pUnit->healRate(m_pPlot);
	}
	if (iMinimumDamage > 0)
	{
		return true;
	}

	return false;
}

bool CvDangerPlotContents::CouldAttackHere(CvUnit* pAttacker)
{
	if (!m_pPlot || !pAttacker)
		return false;

	for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end(); ++it)
	{
		CvUnit* pUnit = GET_PLAYER(it->first).getUnit(it->second);
		if (pUnit == pAttacker)
		{
			if ((m_pPlot->getPlotCity() && GET_TEAM(pAttacker->getTeam()).isAtWar(m_pPlot->getPlotCity()->getTeam())) ||
				m_pPlot->getBestDefender(NO_PLAYER, pAttacker->getOwner(), pAttacker, true))
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

bool CvDangerPlotContents::CouldAttackHere(CvCity* pAttacker)
{
	if (!m_pPlot || !pAttacker)
		return false;

	for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end(); ++it)
	{
		CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);

		if (pCity == pAttacker)
		{
			if ((m_pPlot->getPlotCity() && GET_TEAM(pAttacker->getTeam()).isAtWar(m_pPlot->getPlotCity()->getTeam())) ||
				m_pPlot->getBestDefender(NO_PLAYER, pAttacker->getOwner(), NULL, true))
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

// Get the maximum damage city could receive this turn if it were in this plot
int CvDangerPlotContents::GetDanger(CvCity* pCity, CvUnit* pPretendGarrison, int iAfterNIntercepts)
{
	if (!m_pPlot || !pCity)
		return 0;

	int iPlotDamage = 0;
	CvPlot* pCityPlot = pCity->plot();
	const int iCityX = pCityPlot->getX();
	const int iCityY = pCityPlot->getY();
	const int iMaxNoCaptureDamage = pCity->GetMaxHitPoints() - pCity->getDamage() - 1;

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	if (pPretendGarrison != NULL)
		pCity->OverrideGarrison(pPretendGarrison);
#endif

	CvPlot* pAttackerPlot = NULL;
	CvUnit* pInterceptor = NULL;

	// Damage from ranged units and melees that cannot capture 
	for (DangerUnitVector::iterator it = m_apUnits.begin(); it < m_apUnits.end() && iPlotDamage < iMaxNoCaptureDamage; ++it)
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
#ifdef AUI_UNIT_GET_NTH_BEST_INTERCEPTOR
				pInterceptor = pUnit->GetNthBestInterceptor(*m_pPlot, iAfterNIntercepts);
#else
				pInterceptor = pUnit->GetBestInterceptor(*m_pPlot);
#endif
				int iInterceptDamage = 0;
				if (pInterceptor)
				{
					// Always assume interception is successful
					iInterceptDamage = pInterceptor->GetInterceptionDamage(pUnit, false);
					++iAfterNIntercepts;
				}
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
				iPlotDamage += pUnit->GetAirCombatDamage(NULL, pCity, false, iInterceptDamage, m_pPlot);
#else
				iPlotDamage += pUnit->GetAirCombatDamage(NULL, pCity, false, iInterceptDamage);
#endif
			}
			else
			{
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false, 0, m_pPlot);
#else
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false);
#endif
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
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false, 0, pCityPlot);
#else
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false);
#endif
			}
		}
	}

	// Damage from cities
	for (DangerCityVector::iterator it = m_apCities.begin(); it < m_apCities.end() && iPlotDamage < iMaxNoCaptureDamage; ++it)
	{
		CvCity* pCity = GET_PLAYER(it->first).getCity(it->second);

		if (!pCity || pCity->getTeam() == pCity->getTeam())
		{
			continue;
		}

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
		iPlotDamage += pCity->rangeCombatDamage(NULL, pCity, false, pCityPlot);
#else
		iPlotDamage += pCity->rangeCombatDamage(NULL, pCity, false);
#endif
	}

	if (iPlotDamage > iMaxNoCaptureDamage)
		iPlotDamage = iMaxNoCaptureDamage;

	// Damage from melee units
	for (DangerUnitVector::iterator it = m_apMoveOnlyUnits.begin(); it < m_apMoveOnlyUnits.end(); ++it)
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

			if (pUnit->isRangedSupportFire() && iPlotDamage < iMaxNoCaptureDamage)
			{
#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false, 0, pCityPlot);
#else
				iPlotDamage += pUnit->GetRangeCombatDamage(NULL, pCity, false);
#endif
			}
		}
	}

#ifdef AUI_UNIT_EXTRA_IN_OTHER_PLOT_HELPERS
	pCity->UnsetGarrisonOverride();
#endif

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

#endif // AUI_DANGER_PLOTS_REMADE
