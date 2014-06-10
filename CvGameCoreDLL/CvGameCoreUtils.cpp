/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "CvInternalGameCoreUtils.h"
#include <algorithm>
#include "CvUnit.h"
#include "CvPlayerAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvRandom.h"
#include "CvAStar.h"
#include "CvCity.h"
#include "CvTeam.h"
#include "CvInfos.h"
#include "CvImprovementClasses.h"
#include "CvGlobals.h"

#include "ICvDLLUserInterface.h"

// must be included after all other headers
#include "LintFree.h"

/// This function will return the CvPlot associated with the Index (0 to 36) of a City at iX,iY.  The lower the Index the closer the Plot is to the City (roughly)
CvPlot* plotCity(int iX, int iY, int iIndex)
{
	int iDeltaHexX = 0;
	int iDeltaHexY = 0;
	if (iIndex < NUM_CITY_PLOTS)
	{
		iDeltaHexX = GC.getCityPlotX()[iIndex]; // getCityPlotX now uses hex-space coords
		iDeltaHexY = GC.getCityPlotY()[iIndex];
	}
	else
	{
		// loop till we find the ring this is on
		int iThisRing = 0;
		int iHighestValueOnThisRing = 0;
		int iLowestValueOnThisRing = 0;
		while (iHighestValueOnThisRing < iIndex)
		{
			iThisRing++;
			iLowestValueOnThisRing = iHighestValueOnThisRing + 1;
			iHighestValueOnThisRing += iThisRing*6;
		}
		// determine what side of the hex we are on
		int iDiff = (iIndex - iLowestValueOnThisRing);
		int iSide = iDiff / iThisRing;
		int iOffset = iDiff % iThisRing;

		switch (iSide)
		{
		case 0:
			iDeltaHexX = 0 + iOffset;
			iDeltaHexY = iThisRing - iOffset;
			break;
		case 1:
			iDeltaHexX = iThisRing;
			iDeltaHexY = 0 - iOffset;
			break;
		case 2:
			iDeltaHexX = iThisRing - iOffset;
			iDeltaHexY = -iThisRing;
			break;
		case 3:
			iDeltaHexX = 0 - iOffset;
			iDeltaHexY = -iThisRing + iOffset;
			break;
		case 4:
			iDeltaHexX = -iThisRing;
			iDeltaHexY = 0 + iOffset;
			break;
		case 5:
			iDeltaHexX = -iThisRing + iOffset;
			iDeltaHexY = iThisRing;
			break;
		default:
			return 0;
		}

	}
	// convert the city coord to hex-space coordinates
	int iCityHexX = xToHexspaceX(iX, iY);

	int iPlotHexX = iCityHexX + iDeltaHexX;
	int iPlotY = iY + iDeltaHexY; // Y is the same in both coordinate systems

	// convert from hex-space coordinates to the storage array
	int iPlotX = hexspaceXToX(iPlotHexX, iPlotY);

	return GC.getMap().plot( iPlotX , iPlotY );
}


int plotCityXY(const CvCity* pCity, const CvPlot* pPlot)
{

	int iDX;
	int iWrappedDX = dxWrap(pPlot->getX() - pCity->getX());
	int iWrappedDY = dyWrap(pPlot->getY() - pCity->getY());
	int iDY = iWrappedDY;

	// convert to hex-space coordinates - the coordinate system axes are E and NE (not orthogonal)
	int iCityHexX = xToHexspaceX(pCity->getX(), pCity->getY());
	int iPlotHexX = xToHexspaceX(pCity->getX() + iWrappedDX, pCity->getY() + iWrappedDY);

	iDX = dxWrap(iPlotHexX - iCityHexX);

	if (hexDistance(iDX, iDY) > CITY_PLOTS_RADIUS)
	{
		return -1;
	}
	else
	{
		return GC.getXYCityPlot((iDX + CITY_PLOTS_RADIUS), (iDY + CITY_PLOTS_RADIUS));
	}
}

DirectionTypes estimateDirection(int iDX, int iDY)
{
	const int displacementSize = 6;
	//														NE				E		SE					SW					W			NW
	static double displacements[displacementSize][2] = { {0.5, 0.866025}, {1, 0}, {0.5, -0.866025}, {-0.5, -0.866025}, {-1, 0}, {-0.5, -0.866025}};
	double maximum = 0;
	int maximumIndex = -1;
	for(int i=0;i<displacementSize;i++)
	{
		double dotProduct = iDX * displacements[i][0] + iDY * displacements[i][1];
		if(dotProduct > maximum)
		{
			maximum = dotProduct;
			maximumIndex = i;
		}
	}

	return (DirectionTypes) maximumIndex;
}

bool atWar(TeamTypes eTeamA, TeamTypes eTeamB)
{
	if ((eTeamA == NO_TEAM) || (eTeamB == NO_TEAM))
	{
		return false;
	}

	CvAssert(GET_TEAM(eTeamA).isAtWar(eTeamB) == GET_TEAM(eTeamB).isAtWar(eTeamA));
	CvAssert((eTeamA != eTeamB) || !(GET_TEAM(eTeamA).isAtWar(eTeamB)));

	return GET_TEAM(eTeamA).isAtWar(eTeamB);
}

bool isPotentialEnemy(TeamTypes, TeamTypes)
{
	return false;
}

CvCity* getCity(IDInfo city)
{
	if ((city.eOwner >= 0) && city.eOwner < MAX_PLAYERS)
	{
		return (GET_PLAYER((PlayerTypes)city.eOwner).getCity(city.iID));
	}

	return NULL;
}

CvUnit* getUnit(const IDInfo& unit)
{
	if ((unit.eOwner >= 0) && unit.eOwner < MAX_PLAYERS)
	{
		return (GET_PLAYER((PlayerTypes)unit.eOwner).getUnit(unit.iID));
	}

	return NULL;
}

bool isBeforeUnitCycle(const CvUnit* pFirstUnit, const CvUnit* pSecondUnit)
{
	CvAssert(pFirstUnit != NULL);
	CvAssert(pSecondUnit != NULL);
	CvAssert(pFirstUnit != pSecondUnit);

	if(!pFirstUnit || !pSecondUnit)
		return false;

	if (pFirstUnit->getOwner() != pSecondUnit->getOwner())
	{
		return (pFirstUnit->getOwner() < pSecondUnit->getOwner());
	}

	if (pFirstUnit->getDomainType() != pSecondUnit->getDomainType())
	{
		return (pFirstUnit->getDomainType() < pSecondUnit->getDomainType());
	}

	if (pFirstUnit->GetBaseCombatStrength() != pSecondUnit->GetBaseCombatStrength())
	{
		return (pFirstUnit->GetBaseCombatStrength() > pSecondUnit->GetBaseCombatStrength());
	}

	if (pFirstUnit->getUnitType() != pSecondUnit->getUnitType())
	{
		return (pFirstUnit->getUnitType() > pSecondUnit->getUnitType());
	}

	if (pFirstUnit->getLevel() != pSecondUnit->getLevel())
	{
		return (pFirstUnit->getLevel() > pSecondUnit->getLevel());
	}

	if (pFirstUnit->getExperience() != pSecondUnit->getExperience())
	{
		return (pFirstUnit->getExperience() > pSecondUnit->getExperience());
	}

	return (pFirstUnit->GetID() < pSecondUnit->GetID());
}

/// Is this a valid Promotion for the UnitCombatType?
bool IsPromotionValidForUnitCombatType(PromotionTypes ePromotion, UnitTypes eUnit)
{
	CvUnitEntry* unitInfo = GC.getUnitInfo(eUnit);
	CvPromotionEntry* promotionInfo = GC.getPromotionInfo(ePromotion);

	if(unitInfo == NULL || promotionInfo == NULL)
		return false;

	// No combat class (civilians)
	if (unitInfo->GetUnitCombatType() == NO_UNITCOMBAT)
	{
		return false;
	}

	// Combat class not valid for this Promotion
	if (!(promotionInfo->GetUnitCombatClass(unitInfo->GetUnitCombatType())))
	{
		return false;
	}

	return true;
}

bool isPromotionValid(PromotionTypes ePromotion, UnitTypes eUnit, bool bLeader)
{
	CvUnitEntry* unitInfo = GC.getUnitInfo(eUnit);
	CvPromotionEntry* promotionInfo = GC.getPromotionInfo(ePromotion);

	if(unitInfo == NULL || promotionInfo == NULL)
		return false;

	// Can this Promotion not be chosen through normal leveling?
	if (promotionInfo->IsCannotBeChosen())
	{
		return false;
	}

	// If a Unit gets a Promotion for free then hand it out, no questions asked
	if (unitInfo->GetFreePromotions(ePromotion))
	{
		return true;
	}

	// If this isn't a combat Unit, no Promotion
	if (unitInfo->GetUnitCombatType() == NO_UNITCOMBAT)
	{
		return false;
	}

	// Is this a valid Promotion for the UnitCombatType?
	if (!::IsPromotionValidForUnitCombatType(ePromotion, eUnit))
	{
		return false;
	}

	if (!bLeader && promotionInfo->IsLeader())
	{
		return false;
	}

	// If the Unit only has one move then Blitz is not useful
	if (unitInfo->GetMoves() == 1)
	{
		if (promotionInfo->IsBlitz())
		{
			return false;
		}
	}

	// Promotion Prereqs
	if (NO_PROMOTION != promotionInfo->GetPrereqPromotion())
	{
		if (!isPromotionValid((PromotionTypes)promotionInfo->GetPrereqPromotion(), eUnit, bLeader))
		{
			return false;
		}
	}

	PromotionTypes ePrereq1 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion1();
	PromotionTypes ePrereq2 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion2();
	PromotionTypes ePrereq3 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion3();
	PromotionTypes ePrereq4 = (PromotionTypes)promotionInfo->GetPrereqOrPromotion4();
	if (ePrereq1 != NO_PROMOTION ||
		ePrereq2 != NO_PROMOTION ||
		ePrereq3 != NO_PROMOTION ||
		ePrereq4 != NO_PROMOTION)
	{
		bool bValid = false;
		if (!bValid)
		{
			if (NO_PROMOTION != ePrereq1 && isPromotionValid(ePrereq1, eUnit, bLeader))
			{
				bValid = true;
			}
		}

		if (!bValid)
		{
			if (NO_PROMOTION != ePrereq2 && isPromotionValid(ePrereq2, eUnit, bLeader))
			{
				bValid = true;
			}
		}

		if (!bValid)
		{
			if (NO_PROMOTION != ePrereq3 && isPromotionValid(ePrereq3, eUnit, bLeader))
			{
				bValid = true;
			}
		}

		if (!bValid)
		{
			if (NO_PROMOTION != ePrereq4 && isPromotionValid(ePrereq4, eUnit, bLeader))
			{
				bValid = true;
			}
		}

		if (!bValid)
		{
			return false;
		}
	}

	return true;
}

int getPopulationAsset(int iPopulation)
{
	return (iPopulation * 2);
}

int getLandPlotsAsset(int iLandPlots)
{
	return iLandPlots;
}

int getPopulationPower(int iPopulation)
{
	return (iPopulation / 2);
}

int getPopulationScore(int iPopulation)
{
	return iPopulation;
}

int getLandPlotsScore(int iLandPlots)
{
	return iLandPlots;
}

int getTechScore(TechTypes eTech)
{
	CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
	if(pkTechInfo == NULL)
		return 0;
	else
		return pkTechInfo->GetEra() + 1;
}

int getWonderScore(BuildingClassTypes eWonderClass)
{
	CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eWonderClass);
	if(pkBuildingClassInfo)
	{
		if(isLimitedWonderClass(*pkBuildingClassInfo))
			return 5;
	}

	return 0;
}

ImprovementTypes finalImprovementUpgrade(ImprovementTypes eImprovement, int iCount)
{
	CvAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");

	if (iCount > GC.getNumImprovementInfos())
	{
		return NO_IMPROVEMENT;
	}

	if (GC.getImprovementInfo(eImprovement)->GetImprovementUpgrade() != NO_IMPROVEMENT)
	{
		return finalImprovementUpgrade(((ImprovementTypes)(GC.getImprovementInfo(eImprovement)->GetImprovementUpgrade())), (iCount + 1));
	}
	else
	{
		return eImprovement;
	}
}

bool isTechRequiredForUnit(TechTypes eTech, UnitTypes eUnit)
{
	CvUnitEntry* info = GC.getUnitInfo(eUnit);
	if(info == NULL)
	{
		return false;
	}

	if (info->GetPrereqAndTech() == eTech)
	{
		return true;
	}

	for (int iI = 0; iI < GC.getNUM_UNIT_AND_TECH_PREREQS(); iI++)
	{
		if (info->GetPrereqAndTechs(iI) == eTech)
		{
			return true;
		}
	}

	return false;
}

bool isTechRequiredForBuilding(TechTypes eTech, BuildingTypes eBuilding)
{
	CvBuildingEntry* info = GC.getBuildingInfo(eBuilding);
	if(info)
	{
		if (info->GetPrereqAndTech() == eTech)
		{
			return true;
		}

		for (int iI = 0; iI < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iI++)
		{
			if (info->GetPrereqAndTechs(iI) == eTech)
			{
				return true;
			}
		}
	}

	return false;
}

bool isTechRequiredForProject(TechTypes eTech, ProjectTypes eProject)
{
	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(pkProjectInfo)
	{
		if (pkProjectInfo->GetTechPrereq() == eTech)
		{
			return true;
		}
	}

	return false;
}

bool isWorldUnitClass(UnitClassTypes eUnitClass)
{
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	if(pkUnitClassInfo)
	{
		return (pkUnitClassInfo->getMaxGlobalInstances() != -1);
	}
	return false;
}

bool isTeamUnitClass(UnitClassTypes eUnitClass)
{
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	if(pkUnitClassInfo)
	{
		return (pkUnitClassInfo->getMaxTeamInstances() != -1);
	}
	return false;
}

bool isNationalUnitClass(UnitClassTypes eUnitClass)
{
	CvUnitClassInfo* pkUnitClassInfo = GC.getUnitClassInfo(eUnitClass);
	if(pkUnitClassInfo)
	{
		return (pkUnitClassInfo->getMaxPlayerInstances() != -1);
	}
	return false;
}

bool isLimitedUnitClass(UnitClassTypes eUnitClass)
{
	return (isWorldUnitClass(eUnitClass) || isTeamUnitClass(eUnitClass) || isNationalUnitClass(eUnitClass));
}

bool isWorldProject(ProjectTypes eProject)
{
	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(pkProjectInfo)
	{
		return (pkProjectInfo->GetMaxGlobalInstances() != -1);
	}
	return false;
}

bool isTeamProject(ProjectTypes eProject)
{
	CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
	if(pkProjectInfo)
	{
		return (pkProjectInfo->GetMaxTeamInstances() != -1);
	}
	return false;
}

bool isLimitedProject(ProjectTypes eProject)
{
	return (isWorldProject(eProject) || isTeamProject(eProject));
}

bool OnSameBodyOfWater(CvCity *pCity1, CvCity *pCity2)
{
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot1 = plotDirection(pCity1->getX(), pCity1->getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot1 != NULL && pAdjacentPlot1->isWater())
		{
			for (int jJ = 0; jJ < NUM_DIRECTION_TYPES; jJ++)
			{
				CvPlot* pAdjacentPlot2 = plotDirection(pCity2->getX(), pCity2->getY(), ((DirectionTypes)jJ));
				if (pAdjacentPlot2 != NULL && pAdjacentPlot2->isWater())
				{
					if (pAdjacentPlot2->getArea() == pAdjacentPlot1->getArea())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

TechTypes getDiscoveryTech(UnitTypes eUnit, PlayerTypes ePlayer)
{
	TechTypes eBestTech = NO_TECH;
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
	if(pkUnitInfo)
	{
		int iBestValue = 0;
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			const TechTypes eTech = static_cast<TechTypes>(iI);
			CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
			if(pkTechInfo)
			{
				if (kPlayer.GetPlayerTechs()->CanResearch(eTech))
				{
					int iValue = 0;

					for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
					{
						iValue += (pkTechInfo->GetFlavorValue(iJ) * pkUnitInfo->GetFlavorValue(iJ));
					}

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestTech = eTech;
					}
				}
			}
		}
	}

	return eBestTech;
}


bool PUF_isPlayer(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getOwner() == iData1);
}

bool PUF_isTeam(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getTeam() == iData1);
}

bool PUF_isCombatTeam(const CvUnit* pUnit, int iData1, int iData2)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	CvAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	return (GET_PLAYER(pUnit->getCombatOwner((TeamTypes)iData2, *(pUnit->plot()))).getTeam() == iData1 && !pUnit->isInvisible((TeamTypes)iData2, false, false));
}

bool PUF_isOtherPlayer(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getOwner() != iData1);
}

bool PUF_isOtherTeam(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	TeamTypes eTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	if (pUnit->canCoexistWithEnemyUnit(eTeam))
	{
		return false;
	}

	return (pUnit->getTeam() != eTeam);
}

bool PUF_isEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	CvAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, *(pUnit->plot()))).getTeam();

	if (pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}

	return (iData2 ? eOtherTeam != eOurTeam : atWar(eOtherTeam, eOurTeam));
}

bool PUF_isVisible(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return !(pUnit->isInvisible(GET_PLAYER((PlayerTypes)iData1).getTeam(), false));
}

bool PUF_isVisibleDebug(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return !(pUnit->isInvisible(GET_PLAYER((PlayerTypes)iData1).getTeam(), true));
}

bool PUF_canSiege(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return pUnit->canSiege(GET_PLAYER((PlayerTypes)iData1).getTeam());
}

bool PUF_isPotentialEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	CvAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, *(pUnit->plot()))).getTeam();

	if (pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}
	return (iData2 ? eOtherTeam != eOurTeam : isPotentialEnemy(eOtherTeam, eOurTeam));
}

bool PUF_canDeclareWar( const CvUnit* pUnit, int iData1, int iData2)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	CvAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, *(pUnit->plot()))).getTeam();

	if (pUnit->canCoexistWithEnemyUnit(eOtherTeam))
	{
		return false;
	}

	return (iData2 ? false : GET_TEAM(eOtherTeam).canDeclareWar(eOurTeam));
}

bool PUF_canDefend(const CvUnit* pUnit, int, int)
{
	return pUnit->IsCanDefend();
}

bool PUF_cannotDefend(const CvUnit* pUnit, int, int)
{
	return !(pUnit->IsCanDefend());
}

bool PUF_canDefendEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	CvAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isEnemy(pUnit, iData1, iData2));
}

bool PUF_canDefendPotentialEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isPotentialEnemy(pUnit, iData1, iData2));
}

bool PUF_isFighting(const CvUnit* pUnit, int, int)
{
	return pUnit->isFighting();
}

bool PUF_isDomainType(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getDomainType() == iData1);
}

bool PUF_isUnitType(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getUnitType() == iData1);
}

bool PUF_isUnitAIType(const CvUnit* pUnit, int iData1, int)
{
	CvAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->AI_getUnitAIType() == iData1);
}

bool PUF_isSelected(const CvUnit* pUnit, int, int)
{
	return pUnit->IsSelected();
}

bool PUF_makeInfoBarDirty(CvUnit* pUnit, int, int)
{
	pUnit->setInfoBarDirty(true);
	return true;
}

bool PUF_isNoMission(const CvUnit* pUnit, int, int)
{
	return (pUnit->GetActivityType() != ACTIVITY_MISSION);
}

bool PUF_isFiniteRange(const CvUnit* pUnit, int, int)
{
	return ((pUnit->getDomainType() != DOMAIN_AIR) || (pUnit->getUnitInfo().GetRange() > 0));
}


int baseYieldToSymbol(int iNumYieldTypes, int iYieldStack)
{
	int iReturn;	// holds the return value we will be calculating

	// get the base value for the iReturn value
	iReturn = iNumYieldTypes * GC.getMAX_YIELD_STACK();
	// then add the offset to the return value
	iReturn += iYieldStack;

	// return the value we have calculated
	return iReturn;
}


bool isPickableName(const char* szName)
{
	if (szName)
	{
		int iLen = _tcslen(szName);

		if (!_tcsicmp(&szName[iLen-6], "NOPICK"))
		{
			return false;
		}
	}

	return true;
}

void shuffleArray(int* piShuffle, int iNum, CvRandom& rand)
{
	int iI, iJ;

	for (iI = 0; iI < iNum; iI++)
	{
		piShuffle[iI] = iI;
	}

	for (iI = 0; iI < iNum; iI++)
	{
		iJ = (rand.get(iNum - iI, NULL) + iI);

		if (iI != iJ)
		{
			int iTemp = piShuffle[iI];
			piShuffle[iI] = piShuffle[iJ];
			piShuffle[iJ] = iTemp;
		}
	}
}

int getTurnYearForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	return (getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed) / DB.Count("Months"));
}


int getTurnMonthForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	int iTurnMonth;
	int iTurnCount;
	int iI;

	CvGameSpeedInfo* pkGameSpeedInfo = GC.getGameSpeedInfo(eSpeed);
	if(pkGameSpeedInfo == NULL)
	{
		//This function requires a valid game speed type!
		CvAssert(pkGameSpeedInfo);
		return 0;
	}

	const int iNumMonths = DB.Count("Months");

	iTurnMonth = iStartYear * iNumMonths;

	switch (eCalendar)
	{
	case CALENDAR_DEFAULT:
		{
			iTurnCount = 0;

			const int iGameSpeedNumTurnIncrements = pkGameSpeedInfo->getNumTurnIncrements();

			for (iI = 0; iI < iGameSpeedNumTurnIncrements; iI++)
			{
				const GameTurnInfo& gameTurnInfo = pkGameSpeedInfo->getGameTurnInfo(iI);
				if (iGameTurn > (iTurnCount + gameTurnInfo.iNumGameTurnsPerIncrement))
				{
					iTurnMonth += (gameTurnInfo.iMonthIncrement * gameTurnInfo.iNumGameTurnsPerIncrement);
					iTurnCount += gameTurnInfo.iNumGameTurnsPerIncrement;
				}
				else
				{
					iTurnMonth += (gameTurnInfo.iMonthIncrement * (iGameTurn - iTurnCount));
					iTurnCount += (iGameTurn - iTurnCount);
					break;
				}
			}

			if (iGameTurn > iTurnCount)
			{
				iTurnMonth += (pkGameSpeedInfo->getGameTurnInfo(iGameSpeedNumTurnIncrements - 1).iMonthIncrement * (iGameTurn - iTurnCount));
			}
		}
		break;

	case CALENDAR_BI_YEARLY:
		iTurnMonth += (2 * iGameTurn * iNumMonths);
		break;

	case CALENDAR_YEARS:
	case CALENDAR_TURNS:
		iTurnMonth += iGameTurn * iNumMonths;
		break;

	case CALENDAR_SEASONS:
		iTurnMonth += (iGameTurn * iNumMonths) / DB.Count("Seasons");
		break;

	case CALENDAR_MONTHS:
		iTurnMonth += iGameTurn;
		break;

	case CALENDAR_WEEKS:
		iTurnMonth += iGameTurn / GC.getWEEKS_PER_MONTHS();
		break;

	default:
		CvAssert(false);
	}

	return iTurnMonth;
}


void boolsToString(const bool* pBools, int iNumBools, CvString* szOut)
{
	*szOut = "";
	int i;
	for(i=0;i<iNumBools;i++)
	{
		*szOut += pBools[i] ? "1" : "0";
	}
}

//
// caller must call SAFE_DELETE_ARRAY on ppBools - caller should not be outside the DLL either
//
void stringToBools(const char* szString, int* iNumBools, bool** ppBools)
{
	CvAssertMsg(szString, "null string");
	if (szString)
	{
		*iNumBools = strlen(szString);
		*ppBools = FNEW(bool[*iNumBools], c_eCiv5GameplayDLL, 0);
		int i;
		for(i=0;i<*iNumBools;i++)
		{
			(*ppBools)[i] = (szString[i]=='1');
		}
	}
}

// these string functions should only be used under chipotle cheat code (not internationalized)

void getDirectionTypeString(CvString& strString, DirectionTypes eDirectionType)
{
	switch (eDirectionType)
	{
	case NO_DIRECTION: strString = "NO_DIRECTION"; break;

	//case DIRECTION_NORTH: strString = "north"; break;
	case DIRECTION_NORTHEAST: strString = "northeast"; break;
	case DIRECTION_EAST: strString = "east"; break;
	case DIRECTION_SOUTHEAST: strString = "southeast"; break;
	//case DIRECTION_SOUTH: strString = "south"; break;
	case DIRECTION_SOUTHWEST: strString = "southwest"; break;
	case DIRECTION_WEST: strString = "west"; break;
	case DIRECTION_NORTHWEST: strString = "northwest"; break;

	default: strString = CvString::format("UNKNOWN_DIRECTION(%d)", eDirectionType); break;
	}
}

void getActivityTypeString(CvString& strString, ActivityTypes eActivityType)
{
	switch (eActivityType)
	{
	case NO_ACTIVITY: strString			= "NO_ACTIVITY"; break;
	case ACTIVITY_AWAKE: strString		= "ACTIVITY_AWAKE"; break;
	case ACTIVITY_HOLD: strString		= "ACTIVITY_HOLD"; break;
	case ACTIVITY_SLEEP: strString		= "ACTIVITY_SLEEP"; break;
	case ACTIVITY_HEAL: strString		= "ACTIVITY_HEAL"; break;
	case ACTIVITY_SENTRY: strString		= "ACTIVITY_SENTRY"; break;
	case ACTIVITY_INTERCEPT: strString	= "ACTIVITY_SENTRY"; break;
	case ACTIVITY_MISSION: strString	= "ACTIVITY_MISSION"; break;

	default: strString = CvString::format("UNKNOWN_ACTIVITY(%d)", eActivityType); break;
	}
}

void getMissionAIString(CvString& strString, MissionAITypes eMissionAI)
{
	switch (eMissionAI)
	{
	case NO_MISSIONAI: strString = "NO_MISSIONAI"; break;

	case MISSIONAI_SHADOW: strString = "MISSIONAI_SHADOW"; break;
	case MISSIONAI_GROUP: strString = "MISSIONAI_GROUP"; break;
	case MISSIONAI_LOAD_ASSAULT: strString = "MISSIONAI_LOAD_ASSAULT"; break;
	case MISSIONAI_LOAD_SETTLER: strString = "MISSIONAI_LOAD_SETTLER"; break;
	case MISSIONAI_LOAD_SPECIAL: strString = "MISSIONAI_LOAD_SPECIA"; break;
	case MISSIONAI_GUARD_CITY: strString = "MISSIONAI_GUARD_CITY"; break;
	case MISSIONAI_GUARD_RESOURCE: strString = "MISSIONAI_GUARD_RESOURCE"; break;
	case MISSIONAI_GUARD_SPY: strString = "MISSIONAI_GUARD_SPY"; break;
	case MISSIONAI_ATTACK_SPY: strString = "MISSIONAI_ATTACK_SPY"; break;
	case MISSIONAI_SPREAD: strString = "MISSIONAI_SPREAD"; break;
	case MISSIONAI_CONSTRUCT: strString = "MISSIONAI_CONSTRUCT"; break;
	case MISSIONAI_HURRY: strString = "MISSIONAI_HURRY"; break;
	case MISSIONAI_GREAT_WORK: strString = "MISSIONAI_GREAT_WORK"; break;
	case MISSIONAI_EXPLORE: strString = "MISSIONAI_EXPLORE"; break;
	case MISSIONAI_BLOCKADE: strString = "MISSIONAI_BLOCKADE"; break;
	case MISSIONAI_PILLAGE: strString = "MISSIONAI_PILLAGE"; break;
	case MISSIONAI_FOUND: strString = "MISSIONAI_FOUND"; break;
	case MISSIONAI_BUILD: strString = "MISSIONAI_BUILD"; break;
	case MISSIONAI_ASSAULT: strString = "MISSIONAI_ASSAULT"; break;
	case MISSIONAI_CARRIER: strString = "MISSIONAI_CARRIER"; break;
	case MISSIONAI_PICKUP: strString = "MISSIONAI_PICKUP"; break;

	default: strString = CvString::format("UNKOWN_MISSION_AI(%d)", eMissionAI); break;
	}
}

void getUnitAIString(CvString& strString, UnitAITypes eUnitAI)
{
	// note, GC.getUnitAIInfo(eUnitAI).getDescription() is a international friendly way to get string (but it will be longer)

	switch (eUnitAI)
	{
	case NO_UNITAI: strString = "no unitAI"; break;

	case UNITAI_UNKNOWN: strString = "unknown"; break;
	case UNITAI_SETTLE: strString = "settle"; break;
	case UNITAI_WORKER: strString = "worker"; break;
	case UNITAI_ATTACK: strString = "attack"; break;
	case UNITAI_CITY_BOMBARD: strString = "bombard city"; break;
	case UNITAI_FAST_ATTACK: strString = "fast attack"; break;
	case UNITAI_DEFENSE: strString = "defense"; break;
	case UNITAI_COUNTER: strString = "counter"; break;
	case UNITAI_RANGED: strString = "ranged"; break;
	case UNITAI_CITY_SPECIAL: strString = "city special"; break;
	case UNITAI_EXPLORE: strString = "explore"; break;
	case UNITAI_ARTIST: strString = "artist"; break;
	case UNITAI_SCIENTIST: strString = "scientist"; break;
	case UNITAI_GENERAL: strString = "general"; break;
	case UNITAI_MERCHANT: strString = "merchant"; break;
	case UNITAI_ENGINEER: strString = "engineer"; break;
	case UNITAI_ICBM: strString = "icbm"; break;
	case UNITAI_WORKER_SEA: strString = "worker sea"; break;
	case UNITAI_ATTACK_SEA: strString = "attack sea"; break;
	case UNITAI_RESERVE_SEA: strString = "reserve sea"; break;
	case UNITAI_ESCORT_SEA: strString = "escort sea"; break;
	case UNITAI_EXPLORE_SEA: strString = "explore sea"; break;
	case UNITAI_ASSAULT_SEA: strString = "assault sea"; break;
	case UNITAI_SETTLER_SEA: strString = "settler sea"; break;
	case UNITAI_CARRIER_SEA: strString = "carrier sea"; break;
	case UNITAI_MISSILE_CARRIER_SEA: strString = "missile carrier"; break;
	case UNITAI_PIRATE_SEA: strString = "pirate sea"; break;
	case UNITAI_ATTACK_AIR: strString = "attack air"; break;
	case UNITAI_DEFENSE_AIR: strString = "defense air"; break;
	case UNITAI_CARRIER_AIR: strString = "carrier air"; break;
	case UNITAI_PARADROP: strString = "paradrop"; break;
	case UNITAI_SPACESHIP_PART: strString = "spaceship part"; break;
	case UNITAI_TREASURE: strString = "treasure"; break;

	default: strString = CvString::format("unknown(%d)", eUnitAI); break;
	}
}

void getRotatedPosition(int inHexspaceX, int inHexspaceY, DirectionTypes rotatedDirection, int& outRotatedX, int& outRotatedY)
{
	outRotatedX = inHexspaceX;
	outRotatedY = inHexspaceY;

	// early out if the facing is NE as that is the base rotation that the data is supposed to be stored in
	// also early out if we are looking at the pivot
	if (DIRECTION_NORTHEAST == rotatedDirection || (inHexspaceX == 0 && inHexspaceY == 0))
	{
		return;
	};

	// find the ring that this is on
	int ring = hexDistance(inHexspaceX, inHexspaceY);

	// find the nearest spike direction
	DirectionTypes spikeDirection = hexspaceSpikeDirection(inHexspaceX, inHexspaceY);

	int spikeX = 0;
	int spikeY = 0;
	switch (spikeDirection)
	{
	case DIRECTION_NORTHEAST:
		{
			spikeY = ring;
		}
		break;
	case DIRECTION_EAST:
		{
			spikeX = ring;
		}
		break;
	case DIRECTION_SOUTHEAST:
		{
			spikeX = ring;
			spikeY = -ring;
		}
		break;
	case DIRECTION_SOUTHWEST:
		{
			spikeY = -ring;
		}
		break;
	case DIRECTION_WEST:
		{
			spikeX = -ring;
		}
		break;
	case DIRECTION_NORTHWEST:
		{
			spikeX = -ring;
			spikeY = ring;
		}
		break;
	}

	// find the offset of this point from the spike
	int offsetOnThisRing = hexDistance(spikeX-inHexspaceX,spikeY-inHexspaceY);

	// find the rotated spike
	int newSpikeX = 0;
	int newSpikeY = 0;
	DirectionTypes newSpikeDirection = (DirectionTypes) ((spikeDirection + rotatedDirection) % (NUM_DIRECTION_TYPES));
	switch (newSpikeDirection)
	{
	case DIRECTION_NORTHEAST:
		{
			newSpikeY = ring;
			// add in the offset in the appropriate direction
			outRotatedX = newSpikeX+offsetOnThisRing;
			outRotatedY = newSpikeY-offsetOnThisRing;
		}
		break;
	case DIRECTION_EAST:
		{
			newSpikeX = ring;
			// add in the offset in the appropriate direction
			outRotatedX = newSpikeX;
			outRotatedY = newSpikeY-offsetOnThisRing;
		}
		break;
	case DIRECTION_SOUTHEAST:
		{
			newSpikeX = ring;
			newSpikeY = -ring;
			// add in the offset in the appropriate direction
			outRotatedX = newSpikeX-offsetOnThisRing;
			outRotatedY = newSpikeY;
		}
		break;
	case DIRECTION_SOUTHWEST:
		{
			newSpikeY = -ring;
			// add in the offset in the appropriate direction
			outRotatedX = newSpikeX-offsetOnThisRing;
			outRotatedY = newSpikeY+offsetOnThisRing;
		}
		break;
	case DIRECTION_WEST:
		{
			newSpikeX = -ring;
			// add in the offset in the appropriate direction
			outRotatedX = newSpikeX;
			outRotatedY = newSpikeY+offsetOnThisRing;
		}
		break;
	case DIRECTION_NORTHWEST:
		{
			newSpikeX = -ring;
			newSpikeY = ring;
			// add in the offset in the appropriate direction
			outRotatedX = newSpikeX+offsetOnThisRing;
			outRotatedY = newSpikeY;
		}
		break;
	}
}

//	---------------------------------------------------------------------------
static uint SkipGUIDSeparators(const char* pszGUID, uint uiStartIndex)
{
	UINT uiLength = strlen(pszGUID);
	if (uiStartIndex < uiLength)
	{
		do
		{
			char ch = pszGUID[uiStartIndex];
			if (ch == '{' || ch == '-' || ch == ' ')
				++uiStartIndex;
			else
				break;

		} while (uiStartIndex < uiLength);
	}
	return uiStartIndex;
}

//	---------------------------------------------------------------------------
static bool GetHexDigitValue(char ch, uint &uiValue)
{
	if (ch >= '0' && ch <= '9')
		uiValue = (uint) (ch - '0');
	else
		if (ch >= 'a' && ch <= 'f')
			uiValue = (uint) ((ch - 'a') + 10);
		else
			if (ch >= 'A' && ch <= 'F')
				uiValue = (uint) ((ch - 'A') + 10);
			else
				return false;

	return true;
}
//	---------------------------------------------------------------------------
template <class T>
bool GetGUIDSegment(const char* pszGUID, uint *puiIndex, T &kDest)
{
	*puiIndex = SkipGUIDSeparators(pszGUID, *puiIndex);

	kDest = 0;
	UINT uiLength = strlen(pszGUID);
	if (*puiIndex < uiLength)
	{
		UINT uiDigitCount = sizeof(T) * 2;
		do
		{
			UINT uiValue = 0;
			if (GetHexDigitValue( pszGUID[*puiIndex], uiValue ))
				kDest = (kDest * 16) + uiValue;
			else
				return false;
			*puiIndex += 1;
		} while (*puiIndex < uiLength  && --uiDigitCount);

		return true;
	}
	else
		return false;
}
//	---------------------------------------------------------------------------
bool ExtractGUID(const char* pszGUID, GUID& kGUID, UINT *puiStartIndex /* = NULL */)
{
	if (pszGUID)
	{
		UINT uiIndex = (puiStartIndex != NULL)?(*puiStartIndex):0;

		if (GetGUIDSegment(pszGUID, &uiIndex, kGUID.Data1))
		{
			if (GetGUIDSegment(pszGUID, &uiIndex, kGUID.Data2))
			{
				if (GetGUIDSegment(pszGUID, &uiIndex, kGUID.Data3))
				{
					for (int iByte = 0; iByte < 8; ++iByte)
					{
						if (!GetGUIDSegment(pszGUID, &uiIndex, kGUID.Data4[iByte]))
						{
							return false;
						}
					}

					if (puiStartIndex)
						*puiStartIndex = uiIndex;
					return true;
				}
			}
		}
	}

	return false;
}

//	---------------------------------------------------------------------------
void ClearGUID(GUID& kGUID)
{
	memset(&kGUID, 0, sizeof(GUID));
}

//	---------------------------------------------------------------------------
bool IsGUIDEmpty(const GUID& kGUID)
{
	return kGUID.Data1 == 0 && kGUID.Data2 == 0 && kGUID.Data3 == 0 && *(INT32*)&kGUID.Data4[0] == 0 && *(INT32*)&kGUID.Data4[4] == 0;
}
