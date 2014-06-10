/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "CvDllInterfaces.h"

class CvDiplomacyAI;

class CvDllDiplomacyAI : public ICvDiplomacyAI1
{
public:
	CvDllDiplomacyAI(_In_ CvDiplomacyAI* pDiplomacyAI);
	~CvDllDiplomacyAI();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvDiplomacyAI* GetInstance();

	void DLLCALL DoBeginDiploWithHuman();
	const char* DLLCALL GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer = NO_PLAYER, const char* szOptionalKey1 = "");
	void DLLCALL TestUIDiploStatement(PlayerTypes eToPlayer, DiploStatementTypes eStatement, int iArg1);

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvDiplomacyAI* m_pDiplomacyAI;
};