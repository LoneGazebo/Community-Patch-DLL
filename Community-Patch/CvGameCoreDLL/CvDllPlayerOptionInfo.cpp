/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllPlayerOptionInfo.h"
#include "CvDllContext.h"

CvDllPlayerOptionInfo::CvDllPlayerOptionInfo(CvPlayerOptionInfo* pPlayerOptionInfo)
: m_pPlayerOptionInfo(pPlayerOptionInfo)
, m_uiRefCount(1)
{
	FAssertMsg(pPlayerOptionInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllPlayerOptionInfo::~CvDllPlayerOptionInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllPlayerOptionInfo::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvPlayerOptionInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllPlayerOptionInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllPlayerOptionInfo::DecrementReference()
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
unsigned int CvDllPlayerOptionInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllPlayerOptionInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllPlayerOptionInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllPlayerOptionInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvPlayerOptionInfo* CvDllPlayerOptionInfo::GetInstance()
{
	return m_pPlayerOptionInfo;
}
//------------------------------------------------------------------------------
bool CvDllPlayerOptionInfo::GetDefault()
{
	return m_pPlayerOptionInfo->getDefault();
}
//------------------------------------------------------------------------------