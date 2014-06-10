/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDllDealAI.h"
#include "CvDllContext.h"
#include "CvDllDeal.h"
#include "CvDllPlayer.h"
#include "CvDealAI.h"

CvDllDealAI::CvDllDealAI(CvDealAI* pDealAI)
: m_pDealAI(pDealAI)
, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllDealAI::~CvDllDealAI()
{
}
//------------------------------------------------------------------------------
void* CvDllDealAI::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvDealAI1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllDealAI::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllDealAI::DecrementReference()
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
unsigned int CvDllDealAI::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllDealAI::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllDealAI::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllDealAI::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvDealAI* CvDllDealAI::GetInstance()
{
	return m_pDealAI;
}
//------------------------------------------------------------------------------
ICvPlayer1* CvDllDealAI::GetPlayer()
{
	CvPlayerAI* pkPlayer = (CvPlayerAI*)m_pDealAI->GetPlayer();
	return (NULL != pkPlayer)? new CvDllPlayer(pkPlayer) : NULL;
}
//------------------------------------------------------------------------------
int CvDllDealAI::DoHumanOfferDealToThisAI(ICvDeal1* pDeal)
{
	CvDeal* pkDeal = (NULL != pDeal)? static_cast<CvDllDeal*>(pDeal)->GetInstance() : NULL;
	return m_pDealAI->DoHumanOfferDealToThisAI(pkDeal);
}
//------------------------------------------------------------------------------
void CvDllDealAI::DoAcceptedDeal(PlayerTypes eFromPlayer, ICvDeal1* pDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering)
{
	CvDeal* pkDeal = (NULL != pDeal)? static_cast<CvDllDeal*>(pDeal)->GetInstance() : NULL;
	if(pkDeal != NULL)
		m_pDealAI->DoAcceptedDeal(eFromPlayer, *pkDeal, iDealValueToMe, iValueImOffering, iValueTheyreOffering);
}
//------------------------------------------------------------------------------
int CvDllDealAI::DoHumanDemand(ICvDeal1* pDeal)
{
	CvDeal* pkDeal = (NULL != pDeal)? static_cast<CvDllDeal*>(pDeal)->GetInstance() : NULL;
	return m_pDealAI->DoHumanDemand(pkDeal);
}
//------------------------------------------------------------------------------
void CvDllDealAI::DoAcceptedDemand(PlayerTypes eFromPlayer, ICvDeal1* pDeal)
{
	CvDeal* pkDeal = (NULL != pDeal)? static_cast<CvDllDeal*>(pDeal)->GetInstance() : NULL;
	if(pkDeal != NULL)
		m_pDealAI->DoAcceptedDemand(eFromPlayer, *pkDeal);
}
//------------------------------------------------------------------------------
bool CvDllDealAI::DoEqualizeDealWithHuman(ICvDeal1* pDeal, PlayerTypes eOtherPlayer, bool bDontChangeMyExistingItems, bool bDontChangeTheirExistingItems, bool &bDealGoodToBeginWith, bool &bCantMatchDeal)
{
	CvDeal* pkDeal = (NULL != pDeal)? static_cast<CvDllDeal*>(pDeal)->GetInstance() : NULL;
	return m_pDealAI->DoEqualizeDealWithHuman(pkDeal, eOtherPlayer, bDontChangeMyExistingItems, bDontChangeTheirExistingItems, bDealGoodToBeginWith, bCantMatchDeal);
}
//------------------------------------------------------------------------------