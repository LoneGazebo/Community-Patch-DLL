/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllVictoryInfo.h"
#include "CvDllContext.h"


CvDllVictoryInfo::CvDllVictoryInfo(CvVictoryInfo* pVictoryInfo)
	: m_pVictoryInfo(pVictoryInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pVictoryInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllVictoryInfo::~CvDllVictoryInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllVictoryInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvVictoryInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllVictoryInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllVictoryInfo::DecrementReference()
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
unsigned int CvDllVictoryInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllVictoryInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllVictoryInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllVictoryInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvVictoryInfo* CvDllVictoryInfo::GetInstance()
{
	return m_pVictoryInfo;
}
//------------------------------------------------------------------------------
const char* CvDllVictoryInfo::GetType()
{
	return m_pVictoryInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllVictoryInfo::GetDescription()
{
	return m_pVictoryInfo->GetDescription();
}
//------------------------------------------------------------------------------
