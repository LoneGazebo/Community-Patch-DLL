/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllPromotionInfo.h"
#include "CvDllContext.h"


CvDllPromotionInfo::CvDllPromotionInfo(CvPromotionEntry* pPromotionInfo)
	: m_pPromotionInfo(pPromotionInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pPromotionInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllPromotionInfo::~CvDllPromotionInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllPromotionInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvPromotionInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllPromotionInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllPromotionInfo::DecrementReference()
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
unsigned int CvDllPromotionInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllPromotionInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllPromotionInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllPromotionInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvPromotionEntry* CvDllPromotionInfo::GetInstance()
{
	return m_pPromotionInfo;
}
//------------------------------------------------------------------------------
const char* CvDllPromotionInfo::GetType()
{
	return m_pPromotionInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllPromotionInfo::GetDescription()
{
	return m_pPromotionInfo->GetDescription();
}
//------------------------------------------------------------------------------
