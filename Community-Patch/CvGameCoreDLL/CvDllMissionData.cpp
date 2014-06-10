/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllMissionData.h"
#include "CvDllContext.h"

CvDllMissionData::CvDllMissionData(const MissionData* pMissionData)
: m_pMissionData(pMissionData)
, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllMissionData::~CvDllMissionData()
{
}
//------------------------------------------------------------------------------
void* CvDllMissionData::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvMissionData1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllMissionData::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllMissionData::DecrementReference()
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
unsigned int CvDllMissionData::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllMissionData::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllMissionData::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllMissionData::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
int CvDllMissionData::GetData1() const
{
	return m_pMissionData->iData1;
}
//------------------------------------------------------------------------------
int CvDllMissionData::GetData2() const
{
	return m_pMissionData->iData2;
}
//------------------------------------------------------------------------------
int CvDllMissionData::GetFlags() const
{
	return m_pMissionData->iFlags;
}
//------------------------------------------------------------------------------
int CvDllMissionData::GetPushTurn() const
{
	return m_pMissionData->iPushTurn;
}
//------------------------------------------------------------------------------
MissionTypes CvDllMissionData::GetMissionType() const
{
	return m_pMissionData->eMissionType;
}
//------------------------------------------------------------------------------
