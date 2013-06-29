/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllPlayerColorInfo.h"
#include "CvDllContext.h"

CvDllPlayerColorInfo::CvDllPlayerColorInfo(CvPlayerColorInfo* pPlayerColorInfo)
	: m_pPlayerColorInfo(pPlayerColorInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pPlayerColorInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllPlayerColorInfo::~CvDllPlayerColorInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllPlayerColorInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvPlayerColorInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllPlayerColorInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllPlayerColorInfo::DecrementReference()
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
unsigned int CvDllPlayerColorInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllPlayerColorInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllPlayerColorInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllPlayerColorInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvPlayerColorInfo* CvDllPlayerColorInfo::GetInstance()
{
	return m_pPlayerColorInfo;
}
//------------------------------------------------------------------------------
const char* CvDllPlayerColorInfo::GetType()
{
	return m_pPlayerColorInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllPlayerColorInfo::GetDescription()
{
	return m_pPlayerColorInfo->GetDescription();
}
//------------------------------------------------------------------------------
ColorTypes CvDllPlayerColorInfo::GetColorTypePrimary()
{
	return static_cast<ColorTypes>(m_pPlayerColorInfo->GetColorTypePrimary());
}
//------------------------------------------------------------------------------
ColorTypes CvDllPlayerColorInfo::GetColorTypeSecondary()
{
	return static_cast<ColorTypes>(m_pPlayerColorInfo->GetColorTypeSecondary());
}
//------------------------------------------------------------------------------