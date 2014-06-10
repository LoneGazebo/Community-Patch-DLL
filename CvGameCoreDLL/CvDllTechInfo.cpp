/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllTechInfo.h"
#include "CvDllContext.h"

CvDllTechInfo::CvDllTechInfo(CvTechEntry* pTechInfo)
: m_pTechInfo(pTechInfo)
, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllTechInfo::~CvDllTechInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllTechInfo::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvTechInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllTechInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllTechInfo::DecrementReference()
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
unsigned int CvDllTechInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllTechInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllTechInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllTechInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
const char* CvDllTechInfo::GetDescription() const
{
	return m_pTechInfo->GetDescription();
}
//------------------------------------------------------------------------------
int CvDllTechInfo::GetEra() const
{
	return m_pTechInfo->GetEra();
}
//------------------------------------------------------------------------------
const char* CvDllTechInfo::GetSound() const
{
	return m_pTechInfo->GetSound();
}
//------------------------------------------------------------------------------
const char* CvDllTechInfo::GetSoundMP() const
{
	return m_pTechInfo->GetSoundMP();
}
//------------------------------------------------------------------------------
const char* CvDllTechInfo::GetType() const
{
	return m_pTechInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllTechInfo::GetText() const
{
	return m_pTechInfo->GetText();
}
//------------------------------------------------------------------------------