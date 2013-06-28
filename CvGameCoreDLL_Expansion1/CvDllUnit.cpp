/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllUnit.h"
#include "CvDllContext.h"
#include "CvDllMissionData.h"
#include "CvDllPlot.h"
#include "CvDllUnitInfo.h"

CvDllUnit::CvDllUnit(CvUnit* pUnit)
	: m_pUnit(pUnit)
	, m_uiRefCount(1)
{
	FAssertMsg(pUnit != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllUnit::~CvDllUnit()
{
}
//------------------------------------------------------------------------------
void* CvDllUnit::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvUnit1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllUnit::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllUnit::DecrementReference()
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
unsigned int CvDllUnit::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllUnit::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllUnit::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllUnit::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvUnit* CvDllUnit::GetInstance()
{
	return m_pUnit;
}
//------------------------------------------------------------------------------
int CvDllUnit::GetID() const
{
	return m_pUnit->GetID();
}
//------------------------------------------------------------------------------
PlayerTypes CvDllUnit::GetOwner() const
{
	return m_pUnit->getOwner();
}
//------------------------------------------------------------------------------
int CvDllUnit::GetDamage() const
{
	return m_pUnit->getDamage();
}
//------------------------------------------------------------------------------
int CvDllUnit::GetMoves() const
{
	return m_pUnit->getMoves();
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsSelected() const
{
	return m_pUnit->IsSelected();
}
//------------------------------------------------------------------------------
int CvDllUnit::GetMaxHitPoints() const
{
	return m_pUnit->GetMaxHitPoints();
}
//------------------------------------------------------------------------------
CivilizationTypes CvDllUnit::GetCivilizationType() const
{
	return m_pUnit->getCivilizationType();
}
//------------------------------------------------------------------------------
TeamTypes CvDllUnit::GetTeam() const
{
	return m_pUnit->getTeam();
}
//------------------------------------------------------------------------------
DomainTypes CvDllUnit::GetDomainType() const
{
	return m_pUnit->getDomainType();
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsCombatUnit() const
{
	return m_pUnit->IsCombatUnit();
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsBarbarian() const
{
	return m_pUnit->isBarbarian();
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsHoveringUnit() const
{
	return m_pUnit->IsHoveringUnit();
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsInvisible(TeamTypes eTeam, bool bDebug) const
{
	return m_pUnit->isInvisible(eTeam, bDebug);
}
//------------------------------------------------------------------------------
bool CvDllUnit::CanMoveImpassable() const
{
	return m_pUnit->canMoveImpassable();
}
//------------------------------------------------------------------------------
DirectionTypes CvDllUnit::GetFacingDirection(bool checkLineOfSightProperty) const
{
	return m_pUnit->getFacingDirection(checkLineOfSightProperty);
}
//------------------------------------------------------------------------------
ICvPlot1* CvDllUnit::GetPlot() const
{
	CvPlot* pkPlot = m_pUnit->plot();
	return (NULL != pkPlot)? new CvDllPlot(pkPlot) : NULL;
}
//------------------------------------------------------------------------------
ICvUnitInfo1* DLLCALL CvDllUnit::GetUnitInfo() const
{
	return new CvDllUnitInfo(&m_pUnit->getUnitInfo());
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsEmbarked() const
{
	return m_pUnit->isEmbarked();
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsGarrisoned() const
{
	return m_pUnit->IsGarrisoned();
}
//------------------------------------------------------------------------------
ICvMissionData1* CvDllUnit::GetHeadMissionData() const
{
	const MissionData* pData = m_pUnit->GetHeadMissionData();
	return (NULL != pData)? new CvDllMissionData(pData) : NULL;
}
//------------------------------------------------------------------------------
UnitCombatTypes CvDllUnit::GetUnitCombatType()
{
	return m_pUnit->getUnitCombatType();
}
//------------------------------------------------------------------------------
int CvDllUnit::GetX() const
{
	return m_pUnit->getX();
}
//------------------------------------------------------------------------------
int CvDllUnit::GetY() const
{
	return m_pUnit->getY();
}
//------------------------------------------------------------------------------
void CvDllUnit::GetPosition(int& iX, int& iY) const
{
	iX = m_pUnit->getX();
	iY = m_pUnit->getY();
}
//------------------------------------------------------------------------------
bool CvDllUnit::CanSwapWithUnitHere(ICvPlot1* pPlot) const
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	FAssert(pkPlot != NULL);
	if(pkPlot != NULL)
		return m_pUnit->CanSwapWithUnitHere(*pkPlot);
	else
		return false;
}
//------------------------------------------------------------------------------
bool CvDllUnit::CanEmbarkOnto(ICvPlot1* pOriginPlot, ICvPlot1* pTargetPlot, bool bOverrideEmbarkedCheck) const
{
	CvPlot* pkOriginPlot = (NULL != pOriginPlot)? static_cast<CvDllPlot*>(pOriginPlot)->GetInstance() : NULL;
	CvPlot* pkTargetPlot = (NULL != pTargetPlot)? static_cast<CvDllPlot*>(pTargetPlot)->GetInstance() : NULL;
	FAssert(pkOriginPlot != NULL);
	FAssert(pkTargetPlot != NULL);

	if(pkOriginPlot != NULL && pkTargetPlot != NULL)
		return m_pUnit->canEmbarkOnto(*pkOriginPlot, *pkTargetPlot, bOverrideEmbarkedCheck);
	else
		return false;
}
//------------------------------------------------------------------------------
bool CvDllUnit::CanDisembarkOnto(ICvPlot1* pOriginPlot, ICvPlot1* pTargetPlot, bool bOverrideEmbarkedCheck) const
{
	CvPlot* pkOriginPlot = (NULL != pOriginPlot)? static_cast<CvDllPlot*>(pOriginPlot)->GetInstance() : NULL;
	CvPlot* pkTargetPlot = (NULL != pTargetPlot)? static_cast<CvDllPlot*>(pTargetPlot)->GetInstance() : NULL;
	FAssert(pkOriginPlot != NULL);
	FAssert(pkTargetPlot != NULL);
	if(pkOriginPlot != NULL && pkTargetPlot != NULL)
		return m_pUnit->canDisembarkOnto(*pkOriginPlot, *pkTargetPlot, bOverrideEmbarkedCheck);
	else
		return false;
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsFirstTimeSelected() const
{
	return m_pUnit->IsFirstTimeSelected();
}
//------------------------------------------------------------------------------
UnitTypes CvDllUnit::GetUnitType() const
{
	return m_pUnit->getUnitType();
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsDelayedDeathExported() const
{
	return m_pUnit->isDelayedDeathExported();
}
//------------------------------------------------------------------------------
bool CvDllUnit::IsBusy() const
{
	return m_pUnit->IsBusy();
}
//------------------------------------------------------------------------------
void CvDllUnit::SetupGraphical()
{
	m_pUnit->setupGraphical();
}
//------------------------------------------------------------------------------
CvString CvDllUnit::GetName() const
{
	return m_pUnit->getName();
}
//------------------------------------------------------------------------------
void CvDllUnit::DoCommand(CommandTypes eCommand, int iData1, int iData2)
{
	m_pUnit->doCommand(eCommand, iData1, iData2);
}
//------------------------------------------------------------------------------
bool CvDllUnit::CanDoInterfaceMode(InterfaceModeTypes eInterfaceMode, bool bTestVisibility)
{
	return m_pUnit->CanDoInterfaceMode(eInterfaceMode, bTestVisibility);
}
//------------------------------------------------------------------------------
IDInfo CvDllUnit::GetIDInfo() const
{
	return m_pUnit->GetIDInfo();
}
//------------------------------------------------------------------------------
int CvDllUnit::MovesLeft() const
{
	return m_pUnit->movesLeft();
}
//------------------------------------------------------------------------------
bool CvDllUnit::CanMoveInto(ICvPlot1* pPlot, byte bMoveFlags) const
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	if(pkPlot != NULL)
		return m_pUnit->canMoveInto(*pkPlot, bMoveFlags);
	else
		return false;
}
//------------------------------------------------------------------------------
TeamTypes CvDllUnit::GetDeclareWarMove(ICvPlot1* pPlot) const
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	if(pkPlot != NULL)
		return m_pUnit->GetDeclareWarMove(*pkPlot);
	else
		return NO_TEAM;
}
//------------------------------------------------------------------------------
bool CvDllUnit::ReadyToSelect() const
{
	return m_pUnit->ReadyToSelect();
}
//------------------------------------------------------------------------------
bool CvDllUnit::CanFound() const
{
	return m_pUnit->isFound();
}
//------------------------------------------------------------------------------
bool CvDllUnit::CanWork() const
{
	return m_pUnit->IsWork();
}
//------------------------------------------------------------------------------
FAutoArchive& CvDllUnit::GetSyncArchive()
{
	return m_pUnit->getSyncArchive();
}
//------------------------------------------------------------------------------