/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllPolicyInfo.h"
#include "CvDllContext.h"

CvDllPolicyInfo::CvDllPolicyInfo(CvPolicyEntry* pPolicyInfo)
	: m_pPolicyInfo(pPolicyInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pPolicyInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllPolicyInfo::~CvDllPolicyInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllPolicyInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvPolicyInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllPolicyInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllPolicyInfo::DecrementReference()
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
unsigned int CvDllPolicyInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllPolicyInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllPolicyInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllPolicyInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvPolicyEntry* CvDllPolicyInfo::GetInstance()
{
	return m_pPolicyInfo;
}
//------------------------------------------------------------------------------
const char* CvDllPolicyInfo::GetType()
{
	return m_pPolicyInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllPolicyInfo::GetDescription()
{
	return m_pPolicyInfo->GetDescription();
}
//------------------------------------------------------------------------------
