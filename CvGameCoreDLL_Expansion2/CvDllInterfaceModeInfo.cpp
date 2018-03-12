/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllInterfaceModeInfo.h"
#include "CvDllContext.h"

CvDllInterfaceModeInfo::CvDllInterfaceModeInfo(CvInterfaceModeInfo* pInterfaceModeInfo)
	: m_pInterfaceModeInfo(pInterfaceModeInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pInterfaceModeInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllInterfaceModeInfo::~CvDllInterfaceModeInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllInterfaceModeInfo::QueryInterface(GUID guidInterface)
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
unsigned int CvDllInterfaceModeInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllInterfaceModeInfo::DecrementReference()
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
unsigned int CvDllInterfaceModeInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllInterfaceModeInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllInterfaceModeInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllInterfaceModeInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvInterfaceModeInfo* CvDllInterfaceModeInfo::GetInstance()
{
	return m_pInterfaceModeInfo;
}
//------------------------------------------------------------------------------
int CvDllInterfaceModeInfo::GetMissionType()
{
	return m_pInterfaceModeInfo->getMissionType();
}
//------------------------------------------------------------------------------