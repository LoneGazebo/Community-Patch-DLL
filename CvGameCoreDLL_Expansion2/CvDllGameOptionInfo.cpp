/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllGameOptionInfo.h"
#include "CvDllContext.h"

CvDllGameOptionInfo::CvDllGameOptionInfo(CvGameOptionInfo* pGameOptionInfo)
	: m_pGameOptionInfo(pGameOptionInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pGameOptionInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllGameOptionInfo::~CvDllGameOptionInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllGameOptionInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvGameOptionInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameOptionInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameOptionInfo::DecrementReference()
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
unsigned int CvDllGameOptionInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllGameOptionInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllGameOptionInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllGameOptionInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvGameOptionInfo* CvDllGameOptionInfo::GetInstance()
{
	return m_pGameOptionInfo;
}
//------------------------------------------------------------------------------
const char* CvDllGameOptionInfo::GetType()
{
	return m_pGameOptionInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllGameOptionInfo::GetDescription()
{
	return m_pGameOptionInfo->GetDescription();
}
//------------------------------------------------------------------------------
bool CvDllGameOptionInfo::GetDefault()
{
	return m_pGameOptionInfo->getDefault();
}
//------------------------------------------------------------------------------