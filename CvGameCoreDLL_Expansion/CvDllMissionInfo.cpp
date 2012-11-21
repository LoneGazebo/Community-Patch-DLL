/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllMissionInfo.h"
#include "CvDllContext.h"

CvDllMissionInfo::CvDllMissionInfo(CvMissionInfo* pMissionInfo)
	: m_pMissionInfo(pMissionInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pMissionInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllMissionInfo::~CvDllMissionInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllMissionInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvMissionInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllMissionInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllMissionInfo::DecrementReference()
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
unsigned int CvDllMissionInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllMissionInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllMissionInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllMissionInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvMissionInfo* CvDllMissionInfo::GetInstance()
{
	return m_pMissionInfo;
}
//------------------------------------------------------------------------------
const char* CvDllMissionInfo::GetType()
{
	return m_pMissionInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllMissionInfo::GetDescription()
{
	return m_pMissionInfo->GetDescription();
}
//------------------------------------------------------------------------------
