/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllCivilizationInfo.h"
#include "CvDllContext.h"

CvDllCivilizationInfo::CvDllCivilizationInfo(CvCivilizationInfo* pCivilizationInfo)
	: m_pCivilizationInfo(pCivilizationInfo)
	, m_uiRefCount(1)
{
	FAssertMsg(pCivilizationInfo != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllCivilizationInfo::~CvDllCivilizationInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllCivilizationInfo::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvCivilizationInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllCivilizationInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllCivilizationInfo::DecrementReference()
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
unsigned int CvDllCivilizationInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllCivilizationInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllCivilizationInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllCivilizationInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvCivilizationInfo* CvDllCivilizationInfo::GetInstance()
{
	return m_pCivilizationInfo;
}
//------------------------------------------------------------------------------
const char* CvDllCivilizationInfo::GetType()
{
	return m_pCivilizationInfo->GetType();
}
//------------------------------------------------------------------------------
const char* CvDllCivilizationInfo::GetDescription()
{
	return m_pCivilizationInfo->GetDescription();
}
//------------------------------------------------------------------------------
bool CvDllCivilizationInfo::IsAIPlayable()
{
	return m_pCivilizationInfo->isAIPlayable();
}
//------------------------------------------------------------------------------
bool CvDllCivilizationInfo::IsPlayable()
{
	return m_pCivilizationInfo->isPlayable();
}
//------------------------------------------------------------------------------
const char* CvDllCivilizationInfo::GetShortDescription()
{
	return m_pCivilizationInfo->getShortDescription();
}
//------------------------------------------------------------------------------
int CvDllCivilizationInfo::GetDefaultPlayerColor()
{
	return m_pCivilizationInfo->getDefaultPlayerColor();
}
//------------------------------------------------------------------------------
int CvDllCivilizationInfo::GetArtStyleType()
{
	return m_pCivilizationInfo->getArtStyleType();
}
//------------------------------------------------------------------------------
int CvDllCivilizationInfo::GetNumCityNames()
{
	return m_pCivilizationInfo->getNumCityNames();
}
//------------------------------------------------------------------------------
const char* CvDllCivilizationInfo::GetArtStylePrefix()
{
	return m_pCivilizationInfo->getArtStylePrefix();
}
//------------------------------------------------------------------------------
const char* CvDllCivilizationInfo::GetArtStyleSuffix()
{
	return m_pCivilizationInfo->getArtStyleSuffix();
}
//------------------------------------------------------------------------------
const char* CvDllCivilizationInfo::GetDawnOfManAudio()
{
	return m_pCivilizationInfo->GetDawnOfManAudio();
}
//------------------------------------------------------------------------------
int CvDllCivilizationInfo::GetCivilizationBuildings(int i)
{
	return m_pCivilizationInfo->getCivilizationBuildings(i);
}
//------------------------------------------------------------------------------
bool CvDllCivilizationInfo::IsLeaders(int i)
{
	return m_pCivilizationInfo->isLeaders(i);
}
//------------------------------------------------------------------------------
const char* CvDllCivilizationInfo::GetCityNames(int i)
{
	return m_pCivilizationInfo->getCityNames(i);
}
//------------------------------------------------------------------------------
const char* CvDllCivilizationInfo::GetSoundtrackKey()
{
	return m_pCivilizationInfo->getSoundtrackKey();
}
//------------------------------------------------------------------------------