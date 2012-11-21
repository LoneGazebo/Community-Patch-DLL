/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllCombatInfo.h"
#include "CvDllContext.h"
#include "CvDllCity.h"
#include "CvDllPlot.h"
#include "CvDllUnit.h"

CvDllCombatInfo::CvDllCombatInfo(CvCombatInfo* pCombatInfo)
	: m_pCombatInfo(pCombatInfo)
	, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllCombatInfo::~CvDllCombatInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllCombatInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvCombatInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllCombatInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllCombatInfo::DecrementReference()
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
unsigned int CvDllCombatInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllCombatInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllCombatInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllCombatInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
ICvUnit1* CvDllCombatInfo::GetUnit(BattleUnitTypes unitType) const
{
	CvUnit* pkUnit = m_pCombatInfo->getUnit(unitType);
	if(pkUnit)
	{
		return new CvDllUnit(pkUnit);
	}
	else
	{
		return NULL;
	}
}
//------------------------------------------------------------------------------
ICvCity1* CvDllCombatInfo::GetCity(BattleUnitTypes unitType) const
{
	CvCity* pkCity = m_pCombatInfo->getCity(unitType);
	return (NULL != pkCity)? new CvDllCity(pkCity) : NULL;
}
//------------------------------------------------------------------------------
ICvPlot1* CvDllCombatInfo::GetPlot() const
{
	CvPlot* pkPlot = m_pCombatInfo->getPlot();
	return (NULL != pkPlot)? new CvDllPlot(pkPlot) : NULL;
}
//------------------------------------------------------------------------------
bool CvDllCombatInfo::GetAttackerAdvances() const
{
	return m_pCombatInfo->getAttackerAdvances();
}
//------------------------------------------------------------------------------
bool CvDllCombatInfo::GetVisualizeCombat() const
{
	return m_pCombatInfo->getVisualizeCombat();
}
//------------------------------------------------------------------------------
int CvDllCombatInfo::GetDamageInflicted(BattleUnitTypes unitType) const
{
	return m_pCombatInfo->getDamageInflicted(unitType);
}
//------------------------------------------------------------------------------
int CvDllCombatInfo::GetFinalDamage(BattleUnitTypes unitType) const
{
	return m_pCombatInfo->getFinalDamage(unitType);
}
//------------------------------------------------------------------------------
void* CvDllCombatInfo::TEMPGetRawCombatInfo() const
{
	return m_pCombatInfo;
}
//------------------------------------------------------------------------------
const CvCombatMemberEntry* CvDllCombatInfo::GetDamageMember(int iIndex) const
{
	if(iIndex < m_pCombatInfo->getMaxDamageMemberCount())
		return &m_pCombatInfo->getDamageMembers()[iIndex];
	return NULL;
}
//------------------------------------------------------------------------------
int CvDllCombatInfo::GetDamageMemberCount() const
{
	return m_pCombatInfo->getDamageMemberCount();
}
//------------------------------------------------------------------------------
int CvDllCombatInfo::GetMaxDamageMemberCount() const
{
	return m_pCombatInfo->getMaxDamageMemberCount();
}
//------------------------------------------------------------------------------
bool CvDllCombatInfo::GetDefenderRetaliates() const
{
	return m_pCombatInfo->getDefenderRetaliates();
}
//------------------------------------------------------------------------------
int	CvDllCombatInfo::GetNuclearDamageLevel() const
{
	return m_pCombatInfo->getAttackNuclearLevel();
}
//------------------------------------------------------------------------------
bool CvDllCombatInfo::GetAttackIsRanged() const
{
	return m_pCombatInfo->getAttackIsRanged();
}
//------------------------------------------------------------------------------
bool CvDllCombatInfo::GetAttackIsBombingMission() const
{
	return m_pCombatInfo->getAttackIsBombingMission();
}
//------------------------------------------------------------------------------
bool CvDllCombatInfo::GetAttackIsAirSweep() const
{
	return m_pCombatInfo->getAttackIsAirSweep();
}
//------------------------------------------------------------------------------
bool CvDllCombatInfo::GetDefenderCaptured() const
{
	return m_pCombatInfo->getDefenderCaptured();
}
