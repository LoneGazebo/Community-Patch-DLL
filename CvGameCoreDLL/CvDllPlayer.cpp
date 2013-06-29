/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllPlayer.h"
#include "CvDllContext.h"
#include "CvDllDealAI.h"
#include "CvDllDiplomacyAI.h"
#include "CvDllCity.h"
#include "CvDllPlot.h"
#include "CvDllUnit.h"

#include "CvDiplomacyRequests.h"
#include "CvMinorCivAI.h"


CvDllPlayer::CvDllPlayer(CvPlayerAI* pPlayer)
: m_pPlayer(pPlayer)
, m_uiRefCount(1)
{
	if (gDLL)
		gDLL->GetGameCoreLock();
}
//------------------------------------------------------------------------------
CvDllPlayer::~CvDllPlayer()
{
	if (gDLL)
		gDLL->ReleaseGameCoreLock();
}
//------------------------------------------------------------------------------
void* CvDllPlayer::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvPlayer1::GetInterfaceId() ||
		guidInterface == ICvPlayer2::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllPlayer::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllPlayer::DecrementReference()
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
unsigned int CvDllPlayer::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllPlayer::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllPlayer::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllPlayer::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvPlayerAI* CvDllPlayer::GetInstance()
{
	return m_pPlayer;
}
//------------------------------------------------------------------------------
void CvDllPlayer::Init(PlayerTypes eID)
{
	m_pPlayer->init(eID);
}
//------------------------------------------------------------------------------
void CvDllPlayer::SetupGraphical()
{
	m_pPlayer->setupGraphical();
}
//------------------------------------------------------------------------------
void CvDllPlayer::LaterInit ()
{
	m_pPlayer->gameStartInit();
}
//------------------------------------------------------------------------------
void CvDllPlayer::Uninit()
{
	m_pPlayer->uninit();
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsHuman() const
{
	return m_pPlayer->isHuman();
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsBarbarian() const
{
	return m_pPlayer->isBarbarian();
}
//------------------------------------------------------------------------------
const char* CvDllPlayer::GetName() const
{
	return m_pPlayer->getName();
}
//------------------------------------------------------------------------------
const char* const CvDllPlayer::GetNickName() const
{
	return m_pPlayer->getNickName();
}
//------------------------------------------------------------------------------
ArtStyleTypes CvDllPlayer::GetArtStyleType() const
{
	return m_pPlayer->getArtStyleType();
}
//------------------------------------------------------------------------------
ICvUnit1* CvDllPlayer::GetFirstReadyUnit()
{
	//HACK! This should return an immutable CvUnit interface.
	const CvUnit* pkUnit = m_pPlayer->GetFirstReadyUnit();
	return (NULL != pkUnit)? new CvDllUnit(const_cast<CvUnit*>(pkUnit)) : NULL;
}
//------------------------------------------------------------------------------
bool CvDllPlayer::HasBusyUnit() const
{
	return m_pPlayer->hasBusyUnit();
}
//------------------------------------------------------------------------------
bool CvDllPlayer::HasBusyUnitOrCity() const
{
	return m_pPlayer->hasBusyUnitOrCity();
}
//------------------------------------------------------------------------------
int CvDllPlayer::GetScore(bool bFinal, bool bVictory) const
{
	return m_pPlayer->GetScore(bFinal, bVictory);
}
//------------------------------------------------------------------------------
ICvPlot1* CvDllPlayer::GetStartingPlot() const
{
	CvPlot* pkPlot = m_pPlayer->getStartingPlot();
	return (NULL != pkPlot)? new CvDllPlot(pkPlot) : NULL;
}
//------------------------------------------------------------------------------
ICvCity1* CvDllPlayer::GetCapitalCity()
{
	CvCity* pkCity = m_pPlayer->getCapitalCity();
	return (NULL != pkCity)? new CvDllCity(pkCity) : NULL;
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsHasLostCapital() const
{
	return m_pPlayer->IsHasLostCapital();
}
//------------------------------------------------------------------------------
void CvDllPlayer::SetStartTime(uint uiStartTime)
{
	m_pPlayer->setStartTime(uiStartTime);
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsMinorCiv() const
{
	return m_pPlayer->isMinorCiv();
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsAlive() const
{
	return m_pPlayer->isAlive();
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsEverAlive() const
{
	return m_pPlayer->isEverAlive();
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsTurnActive() const
{
	return m_pPlayer->isTurnActive();
}
//------------------------------------------------------------------------------
void CvDllPlayer::SetTurnActive(bool bNewValue, bool bDoTurn)
{
	m_pPlayer->setTurnActive(bNewValue, bDoTurn);
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsSimultaneousTurns() const
{
	return m_pPlayer->isSimultaneousTurns();
}
//------------------------------------------------------------------------------
PlayerTypes CvDllPlayer::GetID() const
{
	return m_pPlayer->GetID();
}
//------------------------------------------------------------------------------
HandicapTypes CvDllPlayer::GetHandicapType() const
{
	return m_pPlayer->getHandicapType();
}
//------------------------------------------------------------------------------
CivilizationTypes CvDllPlayer::GetCivilizationType() const
{
	return m_pPlayer->getCivilizationType();
}
//------------------------------------------------------------------------------
LeaderHeadTypes CvDllPlayer::GetLeaderType() const
{
	return m_pPlayer->getLeaderType();
}
//------------------------------------------------------------------------------
const char* CvDllPlayer::GetLeaderTypeKey() const
{
	return m_pPlayer->getLeaderTypeKey();
}
//------------------------------------------------------------------------------
EraTypes CvDllPlayer::GetCurrentEra() const
{
	return m_pPlayer->GetCurrentEra();
}
//------------------------------------------------------------------------------
TeamTypes CvDllPlayer::GetTeam() const
{
	return m_pPlayer->getTeam();
}
//------------------------------------------------------------------------------
PlayerColorTypes CvDllPlayer::GetPlayerColor() const
{
	return m_pPlayer->getPlayerColor();
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsOption(PlayerOptionTypes eIndex) const
{
	return m_pPlayer->isOption(eIndex);
}
//------------------------------------------------------------------------------
bool CvDllPlayer::IsOptionKey(PlayerOptionTypes eOptionID) const
{
	return m_pPlayer->isOption(eOptionID);		// Firaxis DLLs have been updated to handle an ID, rather than an index
}
//------------------------------------------------------------------------------
ICvCity1* CvDllPlayer::FirstCity(int* pIterIdx, bool bRev)
{
	CvCity* pkCity = m_pPlayer->firstCity(pIterIdx, bRev);
	return (NULL != pkCity)? new CvDllCity(pkCity) : NULL;
}
//------------------------------------------------------------------------------
ICvCity1* CvDllPlayer::NextCity(int* pIterIdx, bool bRev)
{
	CvCity* pkCity = m_pPlayer->nextCity(pIterIdx, bRev);
	return (NULL != pkCity)? new CvDllCity(pkCity) : NULL;
}
//------------------------------------------------------------------------------
int CvDllPlayer::GetNumCities() const
{
	return m_pPlayer->getNumCities();
}
//------------------------------------------------------------------------------
ICvCity1* CvDllPlayer::GetCity(int iID)
{
	CvCity* pkCity = m_pPlayer->getCity(iID);
	return (NULL != pkCity)? new CvDllCity(pkCity) : NULL;
}
//------------------------------------------------------------------------------
ICvUnit1* CvDllPlayer::GetUnit(int iID)
{
	CvUnit* pkUnit = m_pPlayer->getUnit(iID);
	if(pkUnit)
	{
		return new CvDllUnit(pkUnit);
	}

	return NULL;
}
//------------------------------------------------------------------------------
ICvUnit1* CvDllPlayer::FirstUnit(int* pIterIdx, bool bRev)
{
	CvUnit* pkUnit = m_pPlayer->firstUnit(pIterIdx, bRev);
	return (NULL != pkUnit)? new CvDllUnit(pkUnit) : NULL;
}
//------------------------------------------------------------------------------
ICvUnit1* CvDllPlayer::NextUnit(int* pIterIdx, bool bRev)
{
	CvUnit* pkUnit = m_pPlayer->nextUnit(pIterIdx, bRev);
	return (NULL != pkUnit)? new CvDllUnit(pkUnit) : NULL;
}
//------------------------------------------------------------------------------
int CvDllPlayer::GetPlotDanger(ICvPlot1* pPlot) const
{
	CvDllPlot* pDllPlot = static_cast<CvDllPlot*>(pPlot);
	CvPlot& kPlot = (*pDllPlot->GetInstance());
	return m_pPlayer->GetPlotDanger(kPlot);
}
//------------------------------------------------------------------------------
int CvDllPlayer::GetCityDistanceHighwaterMark() const
{
	return m_pPlayer->GetCityDistanceHighwaterMark();
}
//------------------------------------------------------------------------------
void CvDllPlayer::Disconnected()
{
	m_pPlayer->disconnected();
}
//------------------------------------------------------------------------------
void CvDllPlayer::Reconnected()
{
	m_pPlayer->reconnected();
}
//------------------------------------------------------------------------------
void CvDllPlayer::SetBusyUnitUpdatesRemaining(int iUpdateCount)
{
	m_pPlayer->setBusyUnitUpdatesRemaining(iUpdateCount);
}
//------------------------------------------------------------------------------
bool CvDllPlayer::HasUnitsThatNeedAIUpdate() const
{
	return m_pPlayer->hasUnitsThatNeedAIUpdate();
}
//------------------------------------------------------------------------------
int CvDllPlayer::GetNumPolicyBranchesFinished() const
{
	return m_pPlayer->GetPlayerPolicies()->GetNumPolicyBranchesFinished();
}
//------------------------------------------------------------------------------
void CvDllPlayer::Read(FDataStream& kStream)
{
	m_pPlayer->Read(kStream);
}
//------------------------------------------------------------------------------
void CvDllPlayer::Write(FDataStream& kStream) const
{
	m_pPlayer->Write(kStream);
}
//------------------------------------------------------------------------------
FAutoArchive& CvDllPlayer::GetSyncArchive()
{
	return m_pPlayer->getSyncArchive();
}
//------------------------------------------------------------------------------
int CvDllPlayer::GetGold() const
{
	return m_pPlayer->GetTreasury()->GetGold();
}
//------------------------------------------------------------------------------
int CvDllPlayer::CalculateBaseNetGold()
{
	return m_pPlayer->GetTreasury()->CalculateBaseNetGold();
}
//------------------------------------------------------------------------------
const char* CvDllPlayer::GetEmbarkedGraphicOverride()
{
	return m_pPlayer->GetEmbarkedGraphicOverride();
}
//------------------------------------------------------------------------------
MinorCivTypes CvDllPlayer::GetMinorCivType() const
{
	MinorCivTypes eType = NO_MINORCIV;
	CvMinorCivAI* pAI = m_pPlayer->GetMinorCivAI();
	if(pAI != NULL)
	{
		eType = pAI->GetMinorCivType();
	}

	return eType;
}
//------------------------------------------------------------------------------
ICvDiplomacyAI1* CvDllPlayer::GetDiplomacyAI()
{
	CvDiplomacyAI* pAI = m_pPlayer->GetDiplomacyAI();
	return (NULL != pAI)? new CvDllDiplomacyAI(pAI) : NULL;
}
//------------------------------------------------------------------------------
ICvDealAI1* CvDllPlayer::GetDealAI()
{
	CvDealAI* pAI = m_pPlayer->GetDealAI();
	return (NULL != pAI)? new CvDllDealAI(pAI) : NULL;
}
//------------------------------------------------------------------------------
bool CvDllPlayer::AddDiplomacyRequest(PlayerTypes ePlayerID, DiploUIStateTypes eDiploType, const char* pszMessage, LeaderheadAnimationTypes eAnimationType, int iExtraGameData)
{
	return m_pPlayer->GetDiplomacyRequests()->Add(ePlayerID, eDiploType, pszMessage, eAnimationType, iExtraGameData);
}
//------------------------------------------------------------------------------
void CvDllPlayer::ActiveDiplomacyRequestComplete()
{
	m_pPlayer->GetDiplomacyRequests()->ActiveRequestComplete();
}
//------------------------------------------------------------------------------
int CvDllPlayer::AddNotification(NotificationTypes eNotificationType, const char* strMessage, const char* strSummary, int iX, int iY, int iGameDataIndex, int iExtraGameData)
{
	return m_pPlayer->GetNotifications()->Add(eNotificationType, strMessage, strSummary, iX, iY, iGameDataIndex, iExtraGameData);
}
//------------------------------------------------------------------------------
void CvDllPlayer::ActivateNotification(int iLookupIndex)
{
	m_pPlayer->GetNotifications()->Activate(iLookupIndex);
}
//------------------------------------------------------------------------------
void CvDllPlayer::DismissNotification(int iLookupIndex, bool bUserInvoked)
{
	m_pPlayer->GetNotifications()->Dismiss(iLookupIndex, bUserInvoked);
}
//------------------------------------------------------------------------------
bool CvDllPlayer::MayUserDismissNotification(int iLookupIndex)
{
	return m_pPlayer->GetNotifications()->MayUserDismiss(iLookupIndex);
}
//------------------------------------------------------------------------------
void CvDllPlayer::RebroadcastNotifications(void)
{
	m_pPlayer->GetNotifications()->Rebroadcast();
}
//------------------------------------------------------------------------------
bool CvDllPlayer::GetCurrentResearchTech(TechTypes* pkTech, int *pkTurnsLeft) const
{
	CvPlayerTechs* pkTechs = m_pPlayer->GetPlayerTechs();
	if (pkTechs)
	{
		if (pkTech)
			*pkTech = pkTechs->GetCurrentResearch();
		if (pkTurnsLeft)
			*pkTurnsLeft = pkTechs->GetResearchTurnsLeft(pkTechs->GetCurrentResearch(), false);
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool CvDllPlayer::HasTurnTimerExpired()
{
	return m_pPlayer->hasTurnTimerExpired();
}

//------------------------------------------------------------------------------
