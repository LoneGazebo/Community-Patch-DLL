/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDllDeal.h"
#include "CvDllContext.h"

CvDllDeal::CvDllDeal(CvDeal* pDeal)
: m_pDeal(pDeal)
, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllDeal::~CvDllDeal()
{
}
//------------------------------------------------------------------------------
void* CvDllDeal::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvDeal1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllDeal::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllDeal::DecrementReference()
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
unsigned int CvDllDeal::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllDeal::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllDeal::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllDeal::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvDeal* CvDllDeal::GetInstance()
{
	return m_pDeal;
}
//------------------------------------------------------------------------------
PlayerTypes CvDllDeal::GetOtherPlayer(PlayerTypes eFromPlayer)
{
	return m_pDeal->GetOtherPlayer(eFromPlayer);
}
//------------------------------------------------------------------------------
PlayerTypes CvDllDeal::GetToPlayer()
{
	return m_pDeal->GetToPlayer();
}
//------------------------------------------------------------------------------
PlayerTypes CvDllDeal::GetFromPlayer()
{
	return m_pDeal->GetFromPlayer();
}
//------------------------------------------------------------------------------
unsigned int CvDllDeal::GetStartTurn()
{
	return m_pDeal->GetStartTurn();
}
//------------------------------------------------------------------------------
unsigned int CvDllDeal::GetDuration()
{
	return m_pDeal->GetDuration();
}
//------------------------------------------------------------------------------
unsigned int CvDllDeal::GetEndTurn()
{
	return m_pDeal->GetEndTurn();
}
//------------------------------------------------------------------------------
void CvDllDeal::CopyFrom(ICvDeal1* pOtherDeal)
{
	CvDeal* pkOtherDeal = GC.UnwrapDealPointer(pOtherDeal);
	if(pkOtherDeal != NULL)
		*m_pDeal = *pkOtherDeal;
}
//------------------------------------------------------------------------------
void CvDllDeal::Read(FDataStream& kStream)
{
	kStream >> *m_pDeal;
}
//------------------------------------------------------------------------------
void CvDllDeal::Write(FDataStream& kStream)
{
	kStream << *m_pDeal;
}
//------------------------------------------------------------------------------