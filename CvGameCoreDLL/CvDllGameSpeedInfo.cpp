/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllGameSpeedInfo.h"
#include "CvDllContext.h"

CvDllGameSpeedInfo::CvDllGameSpeedInfo(CvGameSpeedInfo* pGameSpeedInfo)
: m_pGameSpeedInfo(pGameSpeedInfo)
, m_uiRefCount(1)
{
	FAssertMsg(pGameSpeedInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllGameSpeedInfo::~CvDllGameSpeedInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllGameSpeedInfo::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvGameSpeedInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameSpeedInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameSpeedInfo::DecrementReference()
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
unsigned int CvDllGameSpeedInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllGameSpeedInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllGameSpeedInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllGameSpeedInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvGameSpeedInfo* CvDllGameSpeedInfo::GetInstance()
{
	return m_pGameSpeedInfo;
}
//------------------------------------------------------------------------------
const char* CvDllGameSpeedInfo::GetType()
{
	return m_pGameSpeedInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllGameSpeedInfo::GetDescription()
{
	return m_pGameSpeedInfo->GetDescription();
}
//------------------------------------------------------------------------------
int CvDllGameSpeedInfo::GetBarbPercent()
{
	return m_pGameSpeedInfo->getBarbPercent();
}
//------------------------------------------------------------------------------