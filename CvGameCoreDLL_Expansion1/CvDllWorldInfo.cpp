/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllWorldInfo.h"
#include "CvDllContext.h"


CvDllWorldInfo::CvDllWorldInfo(CvWorldInfo* pWorldInfo)
	: m_pWorldInfo(pWorldInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pWorldInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllWorldInfo::~CvDllWorldInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllWorldInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvWorldInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllWorldInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllWorldInfo::DecrementReference()
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
unsigned int CvDllWorldInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllWorldInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllWorldInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllWorldInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvWorldInfo* CvDllWorldInfo::GetInstance()
{
	return m_pWorldInfo;
}
//------------------------------------------------------------------------------
const char* CvDllWorldInfo::GetType()
{
	return m_pWorldInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllWorldInfo::GetDescriptionKey()
{
	return m_pWorldInfo->GetDescriptionKey();
}
//------------------------------------------------------------------------------
int CvDllWorldInfo::GetDefaultPlayers()
{
	return m_pWorldInfo->getDefaultPlayers();
}
//------------------------------------------------------------------------------
int CvDllWorldInfo::GetDefaultMinorCivs()
{
	return m_pWorldInfo->getDefaultMinorCivs();
}
//------------------------------------------------------------------------------