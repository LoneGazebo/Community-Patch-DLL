/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDllDiplomacyAI.h"
#include "CvDllContext.h"
#include "CvDiplomacyAI.h"

CvDllDiplomacyAI::CvDllDiplomacyAI(CvDiplomacyAI* pDiplomacyAI)
: m_pDiplomacyAI(pDiplomacyAI)
, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllDiplomacyAI::~CvDllDiplomacyAI()
{
}
//------------------------------------------------------------------------------
void* CvDllDiplomacyAI::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvDiplomacyAI1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllDiplomacyAI::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllDiplomacyAI::DecrementReference()
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
unsigned int CvDllDiplomacyAI::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllDiplomacyAI::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllDiplomacyAI::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllDiplomacyAI::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvDiplomacyAI* CvDllDiplomacyAI::GetInstance()
{
	return m_pDiplomacyAI;
}
//------------------------------------------------------------------------------
void CvDllDiplomacyAI::DoBeginDiploWithHuman()
{
	m_pDiplomacyAI->DoBeginDiploWithHuman();
}
//------------------------------------------------------------------------------
const char* CvDllDiplomacyAI::GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer, const char* szOptionalKey1)
{
	return m_pDiplomacyAI->GetDiploStringForMessage(eDiploMessage, eForPlayer, szOptionalKey1);
}
//------------------------------------------------------------------------------
void CvDllDiplomacyAI::TestUIDiploStatement(PlayerTypes eToPlayer, DiploStatementTypes eStatement, int iArg1)
{
	m_pDiplomacyAI->TestUIDiploStatement(eToPlayer, eStatement, iArg1);
}
//------------------------------------------------------------------------------