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
		m_DangerPlots.resize(iGridSize);
		m_bArrayAllocated = true;
		for(int i = 0; i < iGridSize; i++)
		{
			m_DangerPlots[i] = 0;
		}
	}
}

/// Uninitialize
void CvDangerPlots::Uninit()
{
	m_ePlayer = NO_PLAYER;
	m_DangerPlots.clear();
	m_bArrayAllocated = false;
	m_bDirty = false;
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
		m_DangerPlots[i] = 0;
	}

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
			if(pLoopUnit->canRangeStrike())
			{
				iRange += pLoopUnit->GetRange();
			}

			CvPlot* pUnitPlot = pLoopUnit->plot();
			AssignUnitDangerValue(pLoopUnit, pUnitPlot);
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

					if(!pLoopUnit->canMoveOrAttackInto(*pLoopPlot) && !pLoopUnit->canRangeStrikeAt(pLoopPlot->getX(),pLoopPlot->getY()))
					{
						continue;
					}

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

			int iRange = GC.getCITY_ATTACK_RANGE();
			CvPlot* pCityPlot = pLoopCity->plot();
			AssignCityDangerValue(pLoopCity, pCityPlot);
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

					AssignCityDangerValue(pLoopCity, pLoopPlot);
				}
			}
		}
	}

	// Citadels
	int iCitadelValue = GetDangerValueOfCitadel();
	int iPlotLoop;
	CvPlot* pPlot, *pAdjacentPlot;
	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		if(pPlot->isRevealed(thisTeam))
		{
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
							AddDanger(pAdjacentPlot->getX(), pAdjacentPlot->getY(), iCitadelValue);
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
		int iThreatValue = GetCityDanger(pLoopCity);
		pLoopCity->SetThreatValue(iThreatValue);
	}

	m_bDirty = false;
}

/// Add an amount of danger to a given tile
void CvDangerPlots::AddDanger(int iPlotX, int iPlotY, int iValue)
{
	const int idx = iPlotX + iPlotY * GC.getMap().getGridWidth();
	m_DangerPlots[idx] += iValue;
}

/// Return the danger value of a given plot
int CvDangerPlots::GetDanger(const CvPlot& pPlot) const
{
	const int idx = pPlot.getX() + pPlot.getY() * GC.getMap().getGridWidth();
	return m_DangerPlots[idx];
}

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
			bIgnoreInFriendlyTerritory = true; // ignore friendly territory
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

	return false;
}

/// Should this unit be ignored when creating the danger plots?
bool CvDangerPlots::ShouldIgnoreUnit(CvUnit* pUnit, bool bIgnoreVisibility)
{
	if(!pUnit->IsCanAttack())
	{
		return true;
	}

	if(!pUnit->plot()->isVisible(GET_PLAYER(m_ePlayer).getTeam()))
	{
		return true;
	}

	if(pUnit->isInvisible(GET_PLAYER(m_ePlayer).getTeam(), false))
	{
		return true;
	}

	CvPlot* pPlot = pUnit->plot();
	CvAssertMsg(pPlot, "Plot is null?")

	if(NULL != pPlot && !pPlot->isVisibleOtherUnit(m_ePlayer) && !bIgnoreVisibility)
	{
		return true;
	}

	// fix post-gold!
	if(pUnit->getDomainType() == DOMAIN_AIR)
	{
		return true;
	}

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
	// MAJIK NUMBARS TO MOVE TO XML
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
			// can the unit actually walk there
			if(!kPathFinder.GeneratePath(pUnit->getX(), pUnit->getY(), iPlotX, iPlotY, 0, true /*bReuse*/))
			{
				return;
			}

			CvAStarNode* pNode = kPathFinder.GetLastNode();
			int iTurnsAway = pNode->m_iData2;
			iTurnsAway = max(iTurnsAway, 1);

			int iUnitCombatValue = iBaseUnitCombatValue / iTurnsAway;
			iUnitCombatValue = ModifyDangerByRelationship(pUnit->getOwner(), pPlot, iUnitCombatValue);
			if (iTurnsAway <= 1)
			{
				iUnitCombatValue |= 0x1;
			}
			else
			{
				iUnitCombatValue &= ~0x1;
			}

			AddDanger(iPlotX, iPlotY, iUnitCombatValue);
		}
	}
}

//	-----------------------------------------------------------------------------------------------
/// Contains the calculations to do the danger value for the plot according to the city
void CvDangerPlots::AssignCityDangerValue(CvCity* pCity, CvPlot* pPlot)
{
	int iCombatValue = pCity->getStrengthValue();
	iCombatValue = ModifyDangerByRelationship(pCity->getOwner(), pPlot, iCombatValue);
	AddDanger(pPlot->getX(), pPlot->getY(), iCombatValue);
}

/// How much danger should we apply to a citadel?
int CvDangerPlots::GetDangerValueOfCitadel() const
{
	// Compute power of this player's strongest unit
	CvMilitaryAI* pMilitaryAI = GET_PLAYER(m_ePlayer).GetMilitaryAI();
	int iPower = pMilitaryAI->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);

	// Magic number to approximate danger from one turn of citadel damage
	return iPower * 50;
}

/// reads in danger plots info
void CvDangerPlots::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> m_ePlayer;
	kStream >> m_bArrayAllocated;

	int iGridSize;
	kStream >> iGridSize;

	m_DangerPlots.resize(iGridSize);
	for(int i = 0; i < iGridSize; i++)
	{
		kStream >> m_DangerPlots[i];
	}

	m_bDirty = false;
}

/// writes out danger plots info
void CvDangerPlots::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

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
