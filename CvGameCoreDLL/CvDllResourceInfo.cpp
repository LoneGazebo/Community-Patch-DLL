/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllResourceInfo.h"
#include "CvDllContext.h"

CvDllResourceInfo::CvDllResourceInfo(CvResourceInfo* pResourceInfo)
: m_pResourceInfo(pResourceInfo)
, m_uiRefCount(1)
{
	FAssertMsg(pResourceInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllResourceInfo::~CvDllResourceInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllResourceInfo::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvResourceInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllResourceInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllResourceInfo::DecrementReference()
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
unsigned int CvDllResourceInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllResourceInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllResourceInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllResourceInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvResourceInfo* CvDllResourceInfo::GetInstance()
{
	return m_pResourceInfo;
}
//------------------------------------------------------------------------------
const char* CvDllResourceInfo::GetType()
{
	return m_pResourceInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllResourceInfo::GetDescription()
{
	return m_pResourceInfo->GetDescription();
}
//------------------------------------------------------------------------------
int CvDllResourceInfo::GetResourceClassType()
{
	return m_pResourceInfo->getResourceClassType();
}
//------------------------------------------------------------------------------
ResourceUsageTypes CvDllResourceInfo::GetResourceUsage()
{
	return m_pResourceInfo->getResourceUsage();
}
//------------------------------------------------------------------------------
const char* CvDllResourceInfo::GetIconString()
{
	return m_pResourceInfo->GetIconString();
}
//------------------------------------------------------------------------------
const char* CvDllResourceInfo::GetArtDefineTag()
{
	return m_pResourceInfo->getArtDefineTag();
}
//------------------------------------------------------------------------------
const char* CvDllResourceInfo::GetArtDefineTagHeavy()
{
	return m_pResourceInfo->getArtDefineTagHeavy();
}
//------------------------------------------------------------------------------
const char* CvDllResourceInfo::GetAltArtDefineTag()
{
	return m_pResourceInfo->getAltArtDefineTag();
}
//------------------------------------------------------------------------------
const char* CvDllResourceInfo::GetAltArtDefineTagHeavy()
{
	return m_pResourceInfo->getAltArtDefineTagHeavy();
}
//------------------------------------------------------------------------------
bool CvDllResourceInfo::IsTerrain(int i)
{
	return m_pResourceInfo->isTerrain(i);
}
//------------------------------------------------------------------------------
bool CvDllResourceInfo::IsFeature(int i)
{
	return m_pResourceInfo->isFeature(i);
}
//------------------------------------------------------------------------------
bool CvDllResourceInfo::IsFeatureTerrain(int i)
{
	return m_pResourceInfo->isFeatureTerrain(i);
}
//------------------------------------------------------------------------------