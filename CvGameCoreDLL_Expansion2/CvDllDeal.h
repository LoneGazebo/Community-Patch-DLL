/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllDeal : public ICvDeal1
{
public:
	CvDllDeal(_In_ CvDeal* pDeal);
	~CvDllDeal();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvDeal* GetInstance();

	PlayerTypes DLLCALL GetOtherPlayer(PlayerTypes eFromPlayer);
	PlayerTypes DLLCALL GetToPlayer();
	PlayerTypes DLLCALL GetFromPlayer();
	unsigned int DLLCALL GetStartTurn();
	unsigned int DLLCALL GetDuration();
	unsigned int DLLCALL GetEndTurn();
	void DLLCALL CopyFrom(ICvDeal1* pOtherDeal);
	void DLLCALL Read(FDataStream& kStream);
	void DLLCALL Write(FDataStream& kStream);

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvDeal* m_pDeal;
};