/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDllPlot.h"
#include "CvDllContext.h"
#include "CvDllCity.h"
#include "CvDllUnit.h"

CvDllPlot::CvDllPlot(CvPlot* pPlot)
	: m_pPlot(pPlot)
	, m_uiRefCount(1)
{
	FAssertMsg(pPlot != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllPlot::~CvDllPlot()
{
}
//------------------------------------------------------------------------------
void* CvDllPlot::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvPlot1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllPlot::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllPlot::DecrementReference()
{
	if(m_uiRefCount == 1)
	{
		delete this;
		return 0;
	}
	else
	{
		--m_uiRefCount;
		return m_uiRefCount;
	}
}
//------------------------------------------------------------------------------
unsigned int CvDllPlot::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllPlot::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllPlot::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllPlot::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvPlot* CvDllPlot::GetInstance()
{
	return m_pPlot;
}
//------------------------------------------------------------------------------
TeamTypes CvDllPlot::GetTeam() const
{
	return m_pPlot->getTeam();
}
//------------------------------------------------------------------------------
FogOfWarModeTypes CvDllPlot::GetActiveFogOfWarMode() const
{
	return m_pPlot->GetActiveFogOfWarMode();
}
//------------------------------------------------------------------------------
void CvDllPlot::UpdateCenterUnit()
{
	m_pPlot->updateCenterUnit();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsAdjacent(ICvPlot1* pPlot) const
{
	CvPlot* pkPlot = (pPlot != NULL)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	return m_pPlot->isAdjacent(pkPlot);
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsRiver() const
{
	return m_pPlot->isRiver();
}
//------------------------------------------------------------------------------
ICvPlot1* CvDllPlot::GetNeighboringPlot(DirectionTypes eDirection) const
{
	CvPlot* pkPlot = m_pPlot->getNeighboringPlot(eDirection);
	return (NULL != pkPlot)? new CvDllPlot(pkPlot) : NULL;
}
//------------------------------------------------------------------------------
int CvDllPlot::GetBuildTime(BuildTypes eBuild, PlayerTypes ePlayer) const
{
	return m_pPlot->getBuildTime(eBuild, ePlayer);
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsVisible(TeamTypes eTeam, bool bDebug) const
{
	return m_pPlot->isVisible(eTeam, bDebug);
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsCity() const
{
	return m_pPlot->isCity();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsEnemyCity(ICvUnit1* pUnit) const
{
	CvUnit* pkUnit = (NULL != pUnit)? static_cast<CvDllUnit*>(pUnit)->GetInstance() : NULL;
	if(pkUnit != NULL)
		return m_pPlot->isEnemyCity(*pkUnit);
	else
		return false;
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsFighting() const
{
	return m_pPlot->isFighting();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsTradeRoute(PlayerTypes ePlayer) const
{
	return m_pPlot->IsTradeRoute(ePlayer);
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsImpassable() const
{
	return m_pPlot->isImpassable();
}
//------------------------------------------------------------------------------
void CvDllPlot::GetPosition(int& iX, int& iY) const
{
	iX = m_pPlot->getX();
	iY = m_pPlot->getY();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsNEOfRiver() const
{
	return m_pPlot->isNEOfRiver();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsWOfRiver() const
{
	return m_pPlot->isWOfRiver();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsNWOfRiver() const
{
	return m_pPlot->isNWOfRiver();
}
//------------------------------------------------------------------------------
FlowDirectionTypes CvDllPlot::GetRiverEFlowDirection() const
{
	return m_pPlot->getRiverEFlowDirection();
}
//------------------------------------------------------------------------------
FlowDirectionTypes CvDllPlot::GetRiverSEFlowDirection() const
{
	return m_pPlot->getRiverSEFlowDirection();
}
//------------------------------------------------------------------------------
FlowDirectionTypes CvDllPlot::GetRiverSWFlowDirection() const
{
	return m_pPlot->getRiverSWFlowDirection();
}
//------------------------------------------------------------------------------
PlayerTypes CvDllPlot::GetOwner() const
{
	return m_pPlot->getOwner();
}
//------------------------------------------------------------------------------
PlotTypes CvDllPlot::GetPlotType() const
{
	return m_pPlot->getPlotType();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsWater() const
{
	return m_pPlot->isWater();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsHills() const
{
	return m_pPlot->isHills();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsOpenGround() const
{
	return m_pPlot->isOpenGround();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsMountain() const
{
	return m_pPlot->isMountain();
}
//------------------------------------------------------------------------------
TerrainTypes CvDllPlot::GetTerrainType() const
{
	return m_pPlot->getTerrainType();
}
//------------------------------------------------------------------------------
FeatureTypes CvDllPlot::GetFeatureType() const
{
	return m_pPlot->getFeatureType();
}
//------------------------------------------------------------------------------
ResourceTypes CvDllPlot::GetResourceType(TeamTypes eTeam) const
{
	return m_pPlot->getResourceType(eTeam);
}
//------------------------------------------------------------------------------
int CvDllPlot::GetNumResource() const
{
	return m_pPlot->getNumResource();
}
//------------------------------------------------------------------------------
ImprovementTypes CvDllPlot::GetImprovementType() const
{
	return m_pPlot->getImprovementType();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsImprovementPillaged() const
{
	return m_pPlot->IsImprovementPillaged();
}
//------------------------------------------------------------------------------
GenericWorldAnchorTypes CvDllPlot::GetWorldAnchor() const
{
	return m_pPlot->GetWorldAnchor();
}
//------------------------------------------------------------------------------
int CvDllPlot::GetWorldAnchorData() const
{
	return m_pPlot->GetWorldAnchorData();
}
//------------------------------------------------------------------------------
RouteTypes CvDllPlot::GetRouteType() const
{
	return m_pPlot->getRouteType();
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsRoutePillaged() const
{
	return m_pPlot->IsRoutePillaged();
}
//------------------------------------------------------------------------------
ICvCity1* CvDllPlot::GetPlotCity() const
{
	CvCity* pkCity = m_pPlot->getPlotCity();
	return (NULL != pkCity)? new CvDllCity(pkCity) : NULL;
}
//------------------------------------------------------------------------------
ICvCity1* CvDllPlot::GetWorkingCity() const
{
	CvCity* pkCity = m_pPlot->getWorkingCity();
	return (NULL != pkCity)? new CvDllCity(pkCity) : NULL;
}
//------------------------------------------------------------------------------
bool CvDllPlot::IsRevealed(TeamTypes eTeam, bool bDebug) const
{
	return m_pPlot->isRevealed(eTeam, bDebug);
}
//------------------------------------------------------------------------------
ImprovementTypes CvDllPlot::GetRevealedImprovementType(TeamTypes eTeam, bool bDebug) const
{
	return m_pPlot->getRevealedImprovementType(eTeam, bDebug);
}
//------------------------------------------------------------------------------
int CvDllPlot::GetBuildProgress(BuildTypes eBuild) const
{
	return m_pPlot->getBuildProgress(eBuild);
}
//------------------------------------------------------------------------------
bool CvDllPlot::GetAnyBuildProgress() const
{
	return m_pPlot->getAnyBuildProgress();
}
//------------------------------------------------------------------------------
void CvDllPlot::UpdateLayout(bool bDebug)
{
	return m_pPlot->updateLayout(bDebug);
}
//------------------------------------------------------------------------------
ICvUnit1* CvDllPlot::GetCenterUnit()
{
	CvUnit* pkUnit = m_pPlot->getCenterUnit().pointer();
	return (NULL != pkUnit)? new CvDllUnit(pkUnit) : NULL;
}
//------------------------------------------------------------------------------
int CvDllPlot::GetNumUnits() const
{
	return m_pPlot->getNumUnits();
}
//------------------------------------------------------------------------------
ICvUnit1* CvDllPlot::GetUnitByIndex(int iIndex) const
{
	CvUnit* pkUnit = m_pPlot->getUnitByIndex(iIndex);
	return (NULL != pkUnit)? new CvDllUnit(pkUnit) : NULL;
}
//------------------------------------------------------------------------------
void CvDllPlot::AddUnit(ICvUnit1* pUnit, bool bUpdate)
{
	CvUnit* pkUnit = (NULL != pUnit)? static_cast<CvDllUnit*>(pUnit)->GetInstance() : NULL;
	return m_pPlot->addUnit(pkUnit, bUpdate);
}
//------------------------------------------------------------------------------
void CvDllPlot::RemoveUnit(ICvUnit1* pUnit, bool bUpdate)
{
	CvUnit* pkUnit = (NULL != pUnit)? static_cast<CvDllUnit*>(pUnit)->GetInstance() : NULL;
	return m_pPlot->removeUnit(pkUnit, bUpdate);
}
//------------------------------------------------------------------------------
const IDInfo* CvDllPlot::NextUnitNode(const IDInfo* pNode) const
{
	return m_pPlot->nextUnitNode(pNode);
}
//------------------------------------------------------------------------------
IDInfo* CvDllPlot::NextUnitNode(IDInfo* pNode)
{
	return m_pPlot->nextUnitNode(pNode);
}
//------------------------------------------------------------------------------
IDInfo* CvDllPlot::HeadUnitNode()
{
	return m_pPlot->headUnitNode();
}
//------------------------------------------------------------------------------
int CvDllPlot::GetPlotIndex() const
{
	return m_pPlot->GetPlotIndex();
}
//------------------------------------------------------------------------------
char CvDllPlot::GetContinentType() const
{
	return m_pPlot->GetContinentType();
}
//------------------------------------------------------------------------------
FAutoArchive& CvDllPlot::GetSyncArchive()
{
	return m_pPlot->getSyncArchive();
}
//------------------------------------------------------------------------------