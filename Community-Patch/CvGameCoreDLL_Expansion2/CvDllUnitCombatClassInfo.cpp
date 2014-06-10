/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllUnitCombatClassInfo.h"
#include "CvDllContext.h"


CvDllUnitCombatClassInfo::CvDllUnitCombatClassInfo(CvBaseInfo* pUnitCombatClassInfo)
	: m_pUnitCombatClassInfo(pUnitCombatClassInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pUnitCombatClassInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllUnitCombatClassInfo::~CvDllUnitCombatClassInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllUnitCombatClassInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvUnitCombatClassInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllUnitCombatClassInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllUnitCombatClassInfo::DecrementReference()
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
unsigned int CvDllUnitCombatClassInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllUnitCombatClassInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllUnitCombatClassInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllUnitCombatClassInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvBaseInfo* CvDllUnitCombatClassInfo::GetInstance()
{
	return m_pUnitCombatClassInfo;
}
//------------------------------------------------------------------------------
const char* CvDllUnitCombatClassInfo::GetType()
{
	return m_pUnitCombatClassInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllUnitCombatClassInfo::GetDescription()
{
	return m_pUnitCombatClassInfo->GetDescription();
}
//------------------------------------------------------------------------------
