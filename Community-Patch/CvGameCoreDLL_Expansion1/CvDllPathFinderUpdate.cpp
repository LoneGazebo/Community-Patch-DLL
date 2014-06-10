/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllPathFinderUpdate.h"
#include "CvDllContext.h"


CvDllPathFinderUpdate::CvDllPathFinderUpdate(int iX, int iY, int iTurnNumber)
	: m_iX(iX)
	, m_iY(iY)
	, m_iTurnNumber(iTurnNumber)
	, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllPathFinderUpdate::~CvDllPathFinderUpdate()
{
}
//------------------------------------------------------------------------------
void* CvDllPathFinderUpdate::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvPathFinderUpdate1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllPathFinderUpdate::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllPathFinderUpdate::DecrementReference()
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
unsigned int CvDllPathFinderUpdate::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllPathFinderUpdate::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllPathFinderUpdate::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllPathFinderUpdate::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
int CvDllPathFinderUpdate::GetX() const
{
	return m_iX;
}
//------------------------------------------------------------------------------
int CvDllPathFinderUpdate::GetY() const
{
	return m_iY;
}
//------------------------------------------------------------------------------
int CvDllPathFinderUpdate::GetTurnNumber() const
{
	return m_iTurnNumber;
}
//------------------------------------------------------------------------------
CvDllPathFinderUpdateList::CvDllPathFinderUpdateList(const std::vector<CvDllPathFinderUpdateListData>& updates)
	: m_uiRefCount(1)
	, m_updates(updates.begin(), updates.end())
	, m_iIndex(-1)
{
}
//------------------------------------------------------------------------------
CvDllPathFinderUpdateList::~CvDllPathFinderUpdateList()
{
}
//------------------------------------------------------------------------------
void* CvDllPathFinderUpdateList::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvEnumerator::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllPathFinderUpdateList::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllPathFinderUpdateList::DecrementReference()
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
unsigned int CvDllPathFinderUpdateList::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllPathFinderUpdateList::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllPathFinderUpdateList::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllPathFinderUpdateList::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
bool CvDllPathFinderUpdateList::MoveNext()
{
	if(m_iIndex == -1)	// In the uninitialized state?
	{
		if(!m_updates.empty())	// Any in the list?
		{
			m_iIndex = 0;	// then we can start
			return true;
		}
		else
			return false;	// Nope, just leave as -1 and return false
	}

	if((size_t)m_iIndex < m_updates.size())
		m_iIndex++;

	return (size_t)m_iIndex < m_updates.size();
}
//------------------------------------------------------------------------------
void CvDllPathFinderUpdateList::Reset()
{
	m_iIndex = -1;
}
//------------------------------------------------------------------------------
ICvUnknown* CvDllPathFinderUpdateList::GetCurrent()
{
	if(m_iIndex > -1 && (size_t)m_iIndex < m_updates.size())
	{
		const CvDllPathFinderUpdateListData& data = m_updates[m_iIndex];

		return new CvDllPathFinderUpdate(data.iX, data.iY, data.iTurnNumber);
	}

	return NULL;
}
//------------------------------------------------------------------------------
