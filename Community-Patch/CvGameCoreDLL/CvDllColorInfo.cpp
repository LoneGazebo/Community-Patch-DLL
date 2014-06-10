/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllColorInfo.h"
#include "CvDllContext.h"

CvDllColorInfo::CvDllColorInfo(CvColorInfo* pColorInfo)
: m_pColorInfo(pColorInfo)
, m_uiRefCount(1)
{
	FAssertMsg(pColorInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllColorInfo::~CvDllColorInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllColorInfo::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvColorInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllColorInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllColorInfo::DecrementReference()
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
unsigned int CvDllColorInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllColorInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllColorInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllColorInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvColorInfo* CvDllColorInfo::GetInstance()
{
	return m_pColorInfo;
}
//------------------------------------------------------------------------------
const char* CvDllColorInfo::GetType()
{
	return m_pColorInfo->GetType();
}
//------------------------------------------------------------------------------
const CvColorA& CvDllColorInfo::GetColor()
{
	return m_pColorInfo->GetColor();
}
//------------------------------------------------------------------------------