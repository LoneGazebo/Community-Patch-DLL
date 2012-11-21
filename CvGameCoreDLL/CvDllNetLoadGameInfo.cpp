/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDllNetLoadGameInfo.h"
#include "CvDllContext.h"

CvDllNetLoadGameInfo::CvDllNetLoadGameInfo()
: m_uiRefCount(1)
{
	m_slotStatus = CvPreGame::GetSlotStatus();
}
//------------------------------------------------------------------------------
CvDllNetLoadGameInfo::~CvDllNetLoadGameInfo()
{
}
//------------------------------------------------------------------------------
void* CvDllNetLoadGameInfo::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvNetLoadGameInfo1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllNetLoadGameInfo::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllNetLoadGameInfo::DecrementReference()
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
unsigned int CvDllNetLoadGameInfo::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllNetLoadGameInfo::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllNetLoadGameInfo::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllNetLoadGameInfo::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
bool CvDllNetLoadGameInfo::Read(FDataStream& kStream)
{
	kStream >> m_slotStatus;
	return true;
}
//------------------------------------------------------------------------------
bool CvDllNetLoadGameInfo::Write(FDataStream& kStream)
{
	kStream << m_slotStatus;
	return true;
}
//------------------------------------------------------------------------------
bool CvDllNetLoadGameInfo::Commit()
{
	CvPreGame::setAllSlotStatus(m_slotStatus);
	return true;
}
//------------------------------------------------------------------------------