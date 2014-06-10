/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllGameAsynch.h"
#include "CvDllContext.h"

CvDllGameAsynch::CvDllGameAsynch(CvGame* pGame)
: m_pGame(pGame)
, m_uiRefCount(1)
{

}
//------------------------------------------------------------------------------
CvDllGameAsynch::~CvDllGameAsynch()
{

}
//------------------------------------------------------------------------------
void* CvDllGameAsynch::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvGame1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameAsynch::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllGameAsynch::DecrementReference()
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
unsigned int CvDllGameAsynch::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllGameAsynch::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllGameAsynch::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllGameAsynch::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}

//------------------------------------------------------------------------------
PlayerTypes CvDllGameAsynch::GetActivePlayer()
{
	return m_pGame->getActivePlayer();
}
//------------------------------------------------------------------------------
TeamTypes CvDllGameAsynch::GetActiveTeam()
{
	return m_pGame->getActiveTeam();
}
//------------------------------------------------------------------------------
int CvDllGameAsynch::GetGameTurn() const
{
	return m_pGame->getGameTurn();
}
//------------------------------------------------------------------------------
GameSpeedTypes CvDllGameAsynch::GetGameSpeedType() const
{
	return m_pGame->getGameSpeedType();
}
//------------------------------------------------------------------------------
GameStateTypes CvDllGameAsynch::GetGameState()
{
	return m_pGame->getGameState();
}
//------------------------------------------------------------------------------
HandicapTypes CvDllGameAsynch::GetHandicapType() const
{
	return m_pGame->getHandicapType();
}
//------------------------------------------------------------------------------
int CvDllGameAsynch::IsAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl)
{
	return m_pGame->IsAction(iKeyStroke, bAlt, bShift, bCtrl);
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsDebugMode() const
{
	return m_pGame->isDebugMode();
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsFinalInitialized() const
{
	return m_pGame->isFinalInitialized();
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsGameMultiPlayer() const
{
	return m_pGame->isGameMultiPlayer();
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsHotSeat() const
{
	return m_pGame->isHotSeat();
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsMPOption(MultiplayerOptionTypes eIndex) const
{
	return m_pGame->isMPOption(eIndex);
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsNetworkMultiPlayer() const
{
	return m_pGame->isNetworkMultiPlayer();
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsOption(GameOptionTypes eIndex) const
{
	return m_pGame->isOption(eIndex);
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsPaused()
{
	return m_pGame->isPaused();
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsPbem() const
{
	return m_pGame->isPbem();
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::IsTeamGame() const
{
	return m_pGame->isTeamGame();
}
//------------------------------------------------------------------------------
bool CvDllGameAsynch::TunerEverConnected() const
{
	return m_pGame->TunerEverConnected();
}
