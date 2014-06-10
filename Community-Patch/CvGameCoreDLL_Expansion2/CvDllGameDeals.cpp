/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDllGameDeals.h"
#include "CvDllDeal.h"
#include "CvDllContext.h"

CvDllGameDeals::CvDllGameDeals(CvGameDeals* pGameDeals)
	: m_pGameDeals(pGameDeals)
	, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllGameDeals::~CvDllGameDeals()
{
}
//------------------------------------------------------------------------------
void* CvDllGameDeals::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvGameDeals1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameDeals::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameDeals::DecrementReference()
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
unsigned int CvDllGameDeals::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllGameDeals::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllGameDeals::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllGameDeals::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvGameDeals* CvDllGameDeals::GetInstance()
{
	return m_pGameDeals;
}
//------------------------------------------------------------------------------
void CvDllGameDeals::AddProposedDeal(ICvDeal1* pDeal)
{
	CvDeal* pkDeal = (NULL != pDeal)? static_cast<CvDllDeal*>(pDeal)->GetInstance() : NULL;
	if(pkDeal != NULL)
	{
		m_pGameDeals->AddProposedDeal(*pkDeal);
	}
}
//------------------------------------------------------------------------------
bool CvDllGameDeals::FinalizeDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bAccepted)
{
	return m_pGameDeals->FinalizeDeal(eFromPlayer, eToPlayer, bAccepted);
}
//------------------------------------------------------------------------------
ICvDeal1* CvDllGameDeals::GetTempDeal()
{
	CvDeal* pDeal = m_pGameDeals->GetTempDeal();
	return (NULL != pDeal)? new CvDllDeal(pDeal) : NULL;
}
//------------------------------------------------------------------------------
void CvDllGameDeals::SetTempDeal(ICvDeal1* pDeal)
{
	CvDeal* pkDeal = (NULL != pDeal)? static_cast<CvDllDeal*>(pDeal)->GetInstance() : NULL;
	m_pGameDeals->SetTempDeal(pkDeal);
}
//------------------------------------------------------------------------------
PlayerTypes CvDllGameDeals::HasMadeProposal(PlayerTypes eFromPlayer)
{
	return m_pGameDeals->HasMadeProposal(eFromPlayer);
}
//------------------------------------------------------------------------------
bool CvDllGameDeals::ProposedDealExists(PlayerTypes eFromPlayer, PlayerTypes eToPlayer)
{
	return m_pGameDeals->ProposedDealExists(eFromPlayer, eToPlayer);
}
//------------------------------------------------------------------------------
ICvDeal1* CvDllGameDeals::GetProposedDeal(PlayerTypes eFromPlayer, PlayerTypes eToPlayer)
{
	CvDeal* pDeal = m_pGameDeals->GetProposedDeal(eFromPlayer, eToPlayer);
	return (NULL != pDeal)? new CvDllDeal(pDeal) : NULL;
}
//------------------------------------------------------------------------------

ICvDeal1* CvDllGameDeals::GetCurrentDeal(PlayerTypes ePlayer, unsigned int index)
{
	CvDeal* pDeal = m_pGameDeals->GetCurrentDeal(ePlayer, index);
	return (NULL != pDeal)? new CvDllDeal(pDeal) : NULL;
}
//------------------------------------------------------------------------------
ICvDeal1* CvDllGameDeals::GetHistoricDeal(PlayerTypes ePlayer, unsigned int indx)
{
	CvDeal* pDeal = m_pGameDeals->GetHistoricDeal(ePlayer, indx);
	return (NULL != pDeal)? new CvDllDeal(pDeal) : NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameDeals::GetNumCurrentDeals(PlayerTypes ePlayer)
{
	return m_pGameDeals->GetNumCurrentDeals(ePlayer);
}
//------------------------------------------------------------------------------
unsigned int CvDllGameDeals::GetNumHistoricDeals(PlayerTypes ePlayer)
{
	return m_pGameDeals->GetNumHistoricDeals(ePlayer);
}
//------------------------------------------------------------------------------
unsigned int CvDllGameDeals::CreateDeal()
{
	return m_pGameDeals->CreateDeal();
}
//------------------------------------------------------------------------------
ICvDeal1* CvDllGameDeals::GetDeal(unsigned int index)
{
	CvDeal* pDeal = m_pGameDeals->GetDeal(index);
	return (NULL != pDeal)? new CvDllDeal(pDeal) : NULL;
}
//------------------------------------------------------------------------------
void CvDllGameDeals::DestroyDeal(unsigned int index)
{
	m_pGameDeals->DestroyDeal(index);
}
//------------------------------------------------------------------------------