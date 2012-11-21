/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllEraInfo.h"
#include "CvDllContext.h"

CvDllEraInfo::CvDllEraInfo(CvEraInfo* pEraInfo)
	: m_pEraInfo(pEraInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pEraInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllEraInfo::~CvDllEraInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllEraInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvEraInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllEraInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllEraInfo::DecrementReference()
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
unsigned int CvDllEraInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllEraInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllEraInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllEraInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvEraInfo* CvDllEraInfo::GetInstance()
{
	return m_pEraInfo;
}
//------------------------------------------------------------------------------
const char* CvDllEraInfo::GetType()
{
	return m_pEraInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllEraInfo::GetDescription()
{
	return m_pEraInfo->GetDescription();
}
//------------------------------------------------------------------------------
int CvDllEraInfo::GetNumEraVOs()
{
	return m_pEraInfo->GetNumEraVOs();
}
//------------------------------------------------------------------------------
const char* CvDllEraInfo::GetEraVO(int iIndex)
{
	return m_pEraInfo->GetEraVO(iIndex);
}
//------------------------------------------------------------------------------
const char* CvDllEraInfo::GetArtPrefix()
{
	return m_pEraInfo->getArtPrefix();
}
//------------------------------------------------------------------------------