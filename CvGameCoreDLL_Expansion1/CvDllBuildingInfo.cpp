/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllBuildingInfo.h"
#include "CvDllContext.h"

CvDllBuildingInfo::CvDllBuildingInfo(CvBuildingEntry* pBuildingInfo)
	: m_pBuildingInfo(pBuildingInfo)
	, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllBuildingInfo::~CvDllBuildingInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllBuildingInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvBuildingInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllBuildingInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllBuildingInfo::DecrementReference()
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
unsigned int CvDllBuildingInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllBuildingInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllBuildingInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllBuildingInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
const char* CvDllBuildingInfo::GetType() const
{
	return m_pBuildingInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllBuildingInfo::GetText() const
{
	return m_pBuildingInfo->GetText();
}
//------------------------------------------------------------------------------
int CvDllBuildingInfo::GetPreferredDisplayPosition() const
{
	return m_pBuildingInfo->GetPreferredDisplayPosition();
}
//------------------------------------------------------------------------------
bool CvDllBuildingInfo::IsBorderObstacle() const
{
	return m_pBuildingInfo->IsBorderObstacle();
}
//------------------------------------------------------------------------------
bool CvDllBuildingInfo::IsPlayerBorderObstacle() const
{
	return m_pBuildingInfo->IsPlayerBorderObstacle();
}
//------------------------------------------------------------------------------
const char* CvDllBuildingInfo::GetArtDefineTag() const
{
	return m_pBuildingInfo->GetArtDefineTag();
}
//------------------------------------------------------------------------------
const bool CvDllBuildingInfo::GetArtInfoCulturalVariation() const
{
	return m_pBuildingInfo->GetArtInfoCulturalVariation();
}
//------------------------------------------------------------------------------
const bool CvDllBuildingInfo::GetArtInfoEraVariation() const
{
	return m_pBuildingInfo->GetArtInfoEraVariation();
}
//------------------------------------------------------------------------------
const bool CvDllBuildingInfo::GetArtInfoRandomVariation() const
{
	return m_pBuildingInfo->GetArtInfoRandomVariation();
}
//------------------------------------------------------------------------------
const char* CvDllBuildingInfo::GetWonderSplashAudio() const
{
	return m_pBuildingInfo->GetWonderSplashAudio();
}
//------------------------------------------------------------------------------