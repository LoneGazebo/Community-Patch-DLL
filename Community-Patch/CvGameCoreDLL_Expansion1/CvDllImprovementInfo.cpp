/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllImprovementInfo.h"
#include "CvDllContext.h"

CvDllImprovementInfo::CvDllImprovementInfo(CvImprovementEntry* pImprovementInfo)
	: m_pImprovementInfo(pImprovementInfo)
	, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllImprovementInfo::~CvDllImprovementInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllImprovementInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvImprovementInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllImprovementInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllImprovementInfo::DecrementReference()
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
unsigned int CvDllImprovementInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllImprovementInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllImprovementInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllImprovementInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
const char* CvDllImprovementInfo::GetType() const
{
	return m_pImprovementInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllImprovementInfo::GetText() const
{
	return m_pImprovementInfo->GetText();
}
//------------------------------------------------------------------------------
bool CvDllImprovementInfo::IsWater() const
{
	return m_pImprovementInfo->IsWater();
}
//------------------------------------------------------------------------------
bool CvDllImprovementInfo::IsDestroyedWhenPillaged() const
{
	return m_pImprovementInfo->IsDestroyedWhenPillaged();
}
//------------------------------------------------------------------------------
bool CvDllImprovementInfo::IsGoody() const
{
	return m_pImprovementInfo->IsGoody();
}
//------------------------------------------------------------------------------
const char* CvDllImprovementInfo::GetArtDefineTag() const
{
	return m_pImprovementInfo->GetArtDefineTag();
}
//------------------------------------------------------------------------------
ImprovementUsageTypes CvDllImprovementInfo::GetImprovementUsage() const
{
	return m_pImprovementInfo->GetImprovementUsage();
}
//------------------------------------------------------------------------------
int CvDllImprovementInfo::GetWorldSoundscapeScriptId() const
{
	return m_pImprovementInfo->GetWorldSoundscapeScriptId();
}
//------------------------------------------------------------------------------
bool CvDllImprovementInfo::GetTerrainMakesValid(int i) const
{
	return m_pImprovementInfo->GetTerrainMakesValid(i);
}
//------------------------------------------------------------------------------
bool CvDllImprovementInfo::IsImprovementResourceMakesValid(int i) const
{
	return m_pImprovementInfo->IsImprovementResourceMakesValid(i);
}
//------------------------------------------------------------------------------