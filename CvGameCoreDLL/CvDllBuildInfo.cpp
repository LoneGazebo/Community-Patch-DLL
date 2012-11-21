/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllBuildInfo.h"
#include "CvDllContext.h"

CvDllBuildInfo::CvDllBuildInfo(CvBuildInfo* pBuildInfo)
: m_pBuildInfo(pBuildInfo)
, m_uiRefCount(1)
{
	FAssertMsg(pBuildInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllBuildInfo::~CvDllBuildInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllBuildInfo::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvBuildInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllBuildInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllBuildInfo::DecrementReference()
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
unsigned int CvDllBuildInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllBuildInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllBuildInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllBuildInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvBuildInfo* CvDllBuildInfo::GetInstance()
{
	return m_pBuildInfo;
}
//------------------------------------------------------------------------------
const char* CvDllBuildInfo::GetType()
{
	return m_pBuildInfo->GetType();
}
//------------------------------------------------------------------------------
int CvDllBuildInfo::GetImprovement()
{
	return m_pBuildInfo->getImprovement();
}
//------------------------------------------------------------------------------
int CvDllBuildInfo::GetRoute()
{
	return m_pBuildInfo->getRoute();
}
//------------------------------------------------------------------------------