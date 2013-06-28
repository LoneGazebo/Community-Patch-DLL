/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllDealAI : public ICvDealAI1
{
public:
	CvDllDealAI(_In_ CvDealAI* pDealAI);
	~CvDllDealAI();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvDealAI* GetInstance();

	ICvPlayer1* DLLCALL GetPlayer();
	int DLLCALL DoHumanOfferDealToThisAI(ICvDeal1* pDeal);
	void DLLCALL DoAcceptedDeal(PlayerTypes eFromPlayer, ICvDeal1* pDeal, int iDealValueToMe, int iValueImOffering, int iValueTheyreOffering);
	int DLLCALL DoHumanDemand(ICvDeal1* pDeal);
	void DLLCALL DoAcceptedDemand(PlayerTypes eFromPlayer, ICvDeal1* pDeal);
	bool DLLCALL DoEqualizeDealWithHuman(ICvDeal1* pDeal, PlayerTypes eOtherPlayer, bool bDontChangeMyExistingItems, bool bDontChangeTheirExistingItems, bool& bDealGoodToBeginWith, bool& bCantMatchOffer);

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvDealAI* m_pDealAI;
};