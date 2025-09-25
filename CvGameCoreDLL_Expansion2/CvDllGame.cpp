/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDllGame.h"
#include "CvDllContext.h"
#include "CvDllGameDeals.h"
#include "CvDllPlayer.h"
#include "CvDllPlot.h"
#include "CvDllRandom.h"
#include "CvDllUnit.h"
#include "CvDllCity.h"

#include "CvGameTextMgr.h"

CvDllGame::CvDllGame(CvGame* pGame)
	: m_pGame(pGame)
	, m_uiRefCount(1)
{
	if(gDLL)
		gDLL->GetGameCoreLock();
}
//------------------------------------------------------------------------------
CvDllGame::~CvDllGame()
{
	if(gDLL)
		gDLL->ReleaseGameCoreLock();
}
//------------------------------------------------------------------------------
void* CvDllGame::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvGame1::GetInterfaceId()	 ||
		guidInterface == ICvGame2::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllGame::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllGame::DecrementReference()
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
unsigned int CvDllGame::GetReferenceCount() const
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllGame::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllGame::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllGame::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
ICvPlayer1* CvDllGame::GetPlayer(const PlayerTypes ePlayer)
{
	return new CvDllPlayer(&CvPlayerAI::getPlayer(ePlayer));
}
//------------------------------------------------------------------------------
CvGame* CvDllGame::GetInstance()
{
	return m_pGame;
}
//------------------------------------------------------------------------------
PlayerTypes CvDllGame::GetActivePlayer()
{
	return m_pGame->getActivePlayer();
}
//------------------------------------------------------------------------------
TeamTypes CvDllGame::GetActiveTeam()
{
	return m_pGame->getActiveTeam();
}
//------------------------------------------------------------------------------
int CvDllGame::GetGameTurn() const
{
	return m_pGame->getGameTurn();
}
//------------------------------------------------------------------------------
void CvDllGame::ChangeNumGameTurnActive(int iChange, const char* why)
{
	std::string strWhy;
	if(why != NULL && strlen(why) > 0)
		strWhy = why;

	m_pGame->changeNumGameTurnActive(iChange, strWhy);
}
//------------------------------------------------------------------------------
int CvDllGame::CountHumanPlayersAlive() const
{
	return m_pGame->countHumanPlayersAlive();
}
//------------------------------------------------------------------------------
int CvDllGame::CountNumHumanGameTurnActive()
{
	return m_pGame->countNumHumanGameTurnActive();
}
//------------------------------------------------------------------------------
bool CvDllGame::CyclePlotUnits(ICvPlot1* pPlot, bool bForward, bool bAuto, int iCount)
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	return m_pGame->cyclePlotUnits(pkPlot, bForward, bAuto, iCount);
}
//------------------------------------------------------------------------------
void CvDllGame::CycleUnits(bool bClear, bool bForward, bool bWorkers)
{
	m_pGame->cycleUnits(bClear, bForward, bWorkers);
}
//------------------------------------------------------------------------------
void CvDllGame::DoGameStarted()
{
	m_pGame->DoGameStarted();

	if (MOD_EXE_HACKING)
		InitExeStuff();
}
//------------------------------------------------------------------------------
void CvDllGame::EndTurnTimerReset()
{
	m_pGame->endTurnTimerReset();
}
//------------------------------------------------------------------------------
void CvDllGame::EndTurnTimerSemaphoreDecrement()
{
	m_pGame->endTurnTimerSemaphoreDecrement();
}
//------------------------------------------------------------------------------
void CvDllGame::EndTurnTimerSemaphoreIncrement()
{
	m_pGame->endTurnTimerSemaphoreIncrement();
}
//------------------------------------------------------------------------------
int CvDllGame::GetAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl)
{
	return m_pGame->GetAction(iKeyStroke, bAlt, bShift, bCtrl);
}
//------------------------------------------------------------------------------
int CvDllGame::IsAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl)
{
	return m_pGame->IsAction(iKeyStroke, bAlt, bShift, bCtrl);
}
//------------------------------------------------------------------------------
ImprovementTypes CvDllGame::GetBarbarianCampImprovementType()
{
	return m_pGame->GetBarbarianCampImprovementType();
}
//------------------------------------------------------------------------------
int CvDllGame::GetElapsedGameTurns() const
{
	return m_pGame->getElapsedGameTurns();
}
//------------------------------------------------------------------------------
bool CvDllGame::GetFOW()
{
	return m_pGame->getFOW();
}
//------------------------------------------------------------------------------
ICvGameDeals1* CvDllGame::GetGameDeals()
{
	return new CvDllGameDeals(&m_pGame->GetGameDeals());
}
//------------------------------------------------------------------------------
GameSpeedTypes CvDllGame::GetGameSpeedType() const
{
	return m_pGame->getGameSpeedType();
}
//------------------------------------------------------------------------------
GameStateTypes CvDllGame::GetGameState()
{
	return m_pGame->getGameState();
}
//------------------------------------------------------------------------------
HandicapTypes CvDllGame::GetHandicapType() const
{
	return m_pGame->getHandicapType();
}
//------------------------------------------------------------------------------
ICvRandom1* CvDllGame::GetRandomNumberGenerator()
{
	return new CvDllRandom(&m_pGame->getJonRand());
}
//------------------------------------------------------------------------------
int CvDllGame::GetJonRandNum(int iNum, const char* pszLog)
{
	return m_pGame->getJonRandNum(iNum, pszLog);
}
//------------------------------------------------------------------------------
int CvDllGame::GetMaxTurns() const
{
	return m_pGame->getMaxTurns();
}
//------------------------------------------------------------------------------
int CvDllGame::GetNumGameTurnActive()
{
	return m_pGame->getNumGameTurnActive();
}
//------------------------------------------------------------------------------
PlayerTypes CvDllGame::GetPausePlayer()
{
	return m_pGame->getPausePlayer();
}
//------------------------------------------------------------------------------
bool CvDllGame::GetPbemTurnSent() const
{
	return m_pGame->getPbemTurnSent();
}
//------------------------------------------------------------------------------
ICvUnit1* CvDllGame::GetPlotUnit(ICvPlot1* pPlot, int iIndex)
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	CvUnit* pkUnit = m_pGame->getPlotUnit(pkPlot, iIndex);

	return (NULL != pkUnit)? new CvDllUnit(pkUnit) : NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllGame::GetVariableCitySizeFromPopulation(unsigned int nPopulation)
{
	return m_pGame->GetVariableCitySizeFromPopulation(nPopulation);
}
//------------------------------------------------------------------------------
VictoryTypes CvDllGame::GetVictory() const
{
	return m_pGame->getVictory();
}
//------------------------------------------------------------------------------
int CvDllGame::GetVotesNeededForDiploVictory() const
{
	return m_pGame->GetVotesNeededForDiploVictory();
}
//------------------------------------------------------------------------------
TeamTypes CvDllGame::GetWinner() const
{
	return m_pGame->getWinner();
}
//------------------------------------------------------------------------------
int CvDllGame::GetWinningTurn() const
{
	return m_pGame->GetWinningTurn();
}
//------------------------------------------------------------------------------
void CvDllGame::HandleAction(int iAction)
{
	m_pGame->handleAction(iAction);
}
//------------------------------------------------------------------------------
bool CvDllGame::HasTurnTimerExpired()
{
	ASSERT(0, "Obsolete");
	return false;
}
//------------------------------------------------------------------------------
bool CvDllGame::HasTurnTimerExpired(PlayerTypes playerID)
{
	return m_pGame->hasTurnTimerExpired(playerID);
}
//------------------------------------------------------------------------------
void CvDllGame::TurnTimerSync(float fCurTurnTime, float fTurnStartTime)
{
	m_pGame->TurnTimerSync(fCurTurnTime, fTurnStartTime);
}
//------------------------------------------------------------------------------
void CvDllGame::GetTurnTimerData(float& fCurTurnTime, float& fTurnStartTime)
{
	m_pGame->GetTurnTimerData(fCurTurnTime, fTurnStartTime);
}
//------------------------------------------------------------------------------
void CvDllGame::Init(HandicapTypes eHandicap)
{
	m_pGame->init(eHandicap);
}
//------------------------------------------------------------------------------
bool CvDllGame::Init2()
{
	return m_pGame->init2();
}
//------------------------------------------------------------------------------
void CvDllGame::InitScoreCalculation()
{
	m_pGame->initScoreCalculation();
}
//------------------------------------------------------------------------------
void CvDllGame::InitTacticalAnalysisMap(int iNumPlots)
{
	//handled in Tactical AI now ...
}
//------------------------------------------------------------------------------
bool CvDllGame::IsCityScreenBlocked()
{
	return m_pGame->IsCityScreenBlocked();
}
//------------------------------------------------------------------------------
bool CvDllGame::CanOpenCityScreen(PlayerTypes eOpener, ICvCity1* pkCity)
{
	CvCity* pCity = GC.UnwrapCityPointer(pkCity);
	return m_pGame->CanOpenCityScreen(eOpener, pCity);
}
//------------------------------------------------------------------------------
bool CvDllGame::IsDebugMode() const
{
	return m_pGame->isDebugMode();
}
//------------------------------------------------------------------------------
bool CvDllGame::IsFinalInitialized() const
{
	return m_pGame->isFinalInitialized();
}
//------------------------------------------------------------------------------
bool CvDllGame::IsGameMultiPlayer() const
{
	return m_pGame->isGameMultiPlayer();
}
//------------------------------------------------------------------------------
bool CvDllGame::IsHotSeat() const
{
	return m_pGame->isHotSeat();
}
//------------------------------------------------------------------------------
bool CvDllGame::IsMPOption(MultiplayerOptionTypes eIndex) const
{
	return m_pGame->isMPOption(eIndex);
}
//------------------------------------------------------------------------------
bool CvDllGame::IsNetworkMultiPlayer() const
{
	return m_pGame->isNetworkMultiPlayer();
}
//------------------------------------------------------------------------------
bool CvDllGame::IsOption(GameOptionTypes eIndex) const
{
	return m_pGame->isOption(eIndex);
}
//------------------------------------------------------------------------------
bool CvDllGame::IsPaused()
{
	return m_pGame->isPaused();
}
//------------------------------------------------------------------------------
bool CvDllGame::IsPbem() const
{
	return m_pGame->isPbem();
}
//------------------------------------------------------------------------------
bool CvDllGame::IsTeamGame() const
{
	return m_pGame->isTeamGame();
}
//------------------------------------------------------------------------------
void CvDllGame::LogGameState(bool bLogHeaders)
{
	m_pGame->LogGameState(bLogHeaders);
}
//------------------------------------------------------------------------------
void CvDllGame::ResetTurnTimer()
{
	m_pGame->resetTurnTimer();
}
//------------------------------------------------------------------------------
void CvDllGame::SelectAll(ICvPlot1* pPlot)
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	m_pGame->selectAll(pkPlot);
}
//------------------------------------------------------------------------------
void CvDllGame::SelectGroup(ICvUnit1* pUnit, bool bShift, bool bCtrl, bool bAlt)
{
	CvUnit* pkUnit = (NULL != pUnit)? static_cast<CvDllUnit*>(pUnit)->GetInstance() : NULL;
	m_pGame->selectGroup(pkUnit, bShift, bCtrl, bAlt);
}
//------------------------------------------------------------------------------
bool CvDllGame::SelectionListIgnoreBuildingDefense()
{
	return m_pGame->selectionListIgnoreBuildingDefense();
}
//------------------------------------------------------------------------------
void CvDllGame::SelectionListMove(ICvPlot1* pPlot, bool bShift)
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	m_pGame->selectionListMove(pkPlot, bShift);
}
//------------------------------------------------------------------------------
void CvDllGame::SelectSettler()
{
	m_pGame->SelectSettler();
}
//------------------------------------------------------------------------------
void CvDllGame::SelectUnit(ICvUnit1* pUnit, bool bClear, bool bToggle, bool bSound)
{
	CvUnit* pkUnit = (NULL != pUnit)? static_cast<CvDllUnit*>(pUnit)->GetInstance() : NULL;
	m_pGame->selectUnit(pkUnit, bClear, bToggle, bSound);
}
//------------------------------------------------------------------------------
void CvDllGame::SendPlayerOptions(bool bForce)
{
	m_pGame->sendPlayerOptions(bForce);
}
//------------------------------------------------------------------------------
void CvDllGame::SetDebugMode(bool bDebugMode)
{
	m_pGame->setDebugMode(bDebugMode);
}
//------------------------------------------------------------------------------
void CvDllGame::SetFinalInitialized(bool bNewValue)
{
	m_pGame->setFinalInitialized(bNewValue);
}
//------------------------------------------------------------------------------
void CvDllGame::SetInitialTime(unsigned int uiNewValue)
{
	m_pGame->setInitialTime(uiNewValue);
}
//------------------------------------------------------------------------------
void CvDllGame::SetMPOption(MultiplayerOptionTypes eIndex, bool bEnabled)
{
	m_pGame->setMPOption(eIndex, bEnabled);
}
//------------------------------------------------------------------------------
void CvDllGame::SetPausePlayer(PlayerTypes eNewValue)
{
	m_pGame->setPausePlayer(eNewValue);
}
//------------------------------------------------------------------------------
void CvDllGame::SetTimeStr(_Inout_z_cap_c_(256) char* szString, int iGameTurn, bool bSave)
{
	if(szString)
	{
		CvString strString;
		CvGameTextMgr::setDateStr(strString,
		                          iGameTurn,
		                          bSave,
		                          m_pGame->getCalendar(),
		                          m_pGame->getStartYear(),
		                          m_pGame->getGameSpeedType());

		strcpy_s(szString, 256, strString.c_str());
	}
}
//------------------------------------------------------------------------------
bool CvDllGame::TunerEverConnected() const
{
	return m_pGame->TunerEverConnected();
}
//------------------------------------------------------------------------------
void CvDllGame::Uninit()
{
	// There exists a bug that allows the game core to be shutdown early if mods are loaded.
	// Executing `SetGameViewRenderType(GameViewTypes.GAMEVIEW_NONE)` in Lua will trigger this.
	// 
	// Because the first step of the shutdown process deinitializes the game instance, this
	// should trap before the program has a chance to enter a broken state which hopefully
	// protects users from any exploits that could be leveraged while the state is broken.
	if (CvPreGame::gameStarted())
		BUILTIN_TRAP();

	m_pGame->uninit();
}
//------------------------------------------------------------------------------
void CvDllGame::UnitIsMoving()
{
	m_pGame->unitIsMoving();
}
//------------------------------------------------------------------------------
void CvDllGame::Update()
{
	m_pGame->update();
}
//------------------------------------------------------------------------------
void CvDllGame::UpdateSelectionList()
{
	m_pGame->updateSelectionList();
}
//------------------------------------------------------------------------------
void CvDllGame::UpdateTestEndTurn()
{
	m_pGame->updateTestEndTurn();
}
//------------------------------------------------------------------------------
void CvDllGame::Read(FDataStream& kStream)
{
	m_pGame->Read(kStream);
}
//------------------------------------------------------------------------------
void CvDllGame::Write(FDataStream& kStream) const
{
	m_pGame->Write(kStream);
#ifdef EA_EVENT_GAME_SAVE // Paz - set m_bSavedOnce = true AFTER the first save
	m_pGame->SetGameEventsSaveGame(true);
#endif
}
//------------------------------------------------------------------------------
void CvDllGame::ReadSupportingClassData(FDataStream& kStream)
{
	m_pGame->ReadSupportingClassData(kStream);
}
//------------------------------------------------------------------------------
void CvDllGame::WriteSupportingClassData(FDataStream& kStream) const
{
	m_pGame->WriteSupportingClassData(kStream);
}
//------------------------------------------------------------------------------
void CvDllGame::WriteReplay(FDataStream& kStream) const
{
	m_pGame->writeReplay(kStream);
}
//------------------------------------------------------------------------------
bool CvDllGame::CanMoveUnitTo(ICvUnit1* pUnit, ICvPlot1* pPlot) const
{
	if(pUnit == NULL || pPlot == NULL)
	{
		return false;
	}

	CvUnit* pkUnit = GC.UnwrapUnitPointer(pUnit);
	CvPlot* pkPlot = GC.UnwrapPlotPointer(pPlot);

	SPathFinderUserData data(pkUnit,CvUnit::MOVEFLAG_IGNORE_DANGER | CvUnit::MOVEFLAG_IGNORE_STACKING_SELF,1);

	// can the unit actually walk there
	return GC.GetPathFinder().DoesPathExist(pkUnit->getX(), pkUnit->getY(), pkPlot->getX(), pkPlot->getY(), data);
}

//------------------------------------------------------------------------------
void CvDllGame::NetMessageStaticsReset()
{
	m_pGame->NetMessageStaticsReset();
}
//------------------------------------------------------------------------------
bool CvDllGame::GetGreatWorkAudio(int GreatWorkIndex, char* strSound, int length)
{
	if (GreatWorkIndex == -1)
	{
		return false;
	}

	const GreatWorkType eType = m_pGame->GetGameCulture()->GetGreatWorkType(GreatWorkIndex);
	if(eType != NO_GREAT_WORK)
	{
		const CvString audio = CultureHelpers::GetGreatWorkAudio(eType);

		if(audio.GetLength() <= length)
		{
			if(strcpy_s(strSound, length, audio.c_str()) == 0)
			{
				return true;
			}
		}
	}

	return false;
}
//------------------------------------------------------------------------------
void CvDllGame::SetLastTurnAICivsProcessed()
{
	m_pGame->SetLastTurnAICivsProcessed();
}
//------------------------------------------------------------------------------
bool endsWith(const char* str, const char* ending)
{
	size_t str_len = strlen(str);
	size_t ending_len = strlen(ending);
	return str_len >= ending_len && !strcmp(str + str_len - ending_len, ending);
}
void CvDllGame::InitExeStuff()
{
	// Here we are going to do some hacking on .exe code
	// 
	// civ5 exes are CEG-protected (some kind of steam out-of-box protection).
	// It disallows you to patch .exe directly, but still we are able to patch it in runtime
	// and do everything we want (I'm not sure, but it looks like so).
	// 
	// Reversing .exe is pretty simple because .exe files are not obfuscated in any way,
	// so you can just load them into ghidra/ida/etc, find the addresses you need and then 
	// patch them here.
	// (!) Moreover, civ5 linux executable is compiled with all the namings preserved, 
	// so things become even more easier because everything what you need is:
	// - Find address in linux executable.
	// - Map it somehow to .exe address (orient by string constants, 
	//   similar code instructions and so on).
	// - Patch it here.
	//
	// todo: support dx9, tablet (?) binaries
	// todo: some modern way of patching
	// todo: code caves
	CvBinType binType;

	char moduleName[1024];
	if (!GetModuleFileName(NULL, moduleName, sizeof(moduleName)))
	{
		// todo: log error (GetLastError)
		binType = BIN_UNKNOWN;
	}
	else if (endsWith(moduleName, "CivilizationV.exe"))
		binType = BIN_DX9;
	else if (endsWith(moduleName, "CivilizationV_DX11.exe"))
		binType = BIN_DX11;
	else if (endsWith(moduleName, "CivilizationV_Tablet.exe"))
		binType = BIN_TABLET;
	else
	{
		// todo: log moduleName
		binType = BIN_UNKNOWN;
	}

	m_pGame->SetExeBinType(binType);

	if (binType == BIN_DX11)
	{
		DWORD baseAddr = (DWORD) GetModuleHandleA(NULL);
		DWORD headersOffset = 0x400000;
		DWORD totalOffset = baseAddr - headersOffset;

		int* s_wantForceResync = reinterpret_cast<int*>(0x02dd2f68 + totalOffset);
		m_pGame->SetExeWantForceResyncPointer(s_wantForceResync);
	}

	/*{
	    // the very basic example of how to fill something with NOPs
		DWORD old_protect;
		DWORD hookLocation = 0x51e031;
		DWORD hookResultAddress = hookLocation + totalOffset;
		if (VirtualProtect((void*)(hookResultAddress), 16, PAGE_EXECUTE_READWRITE, &old_protect))
		{
			*(unsigned char*)(hookResultAddress) = 0x90;
			*(unsigned char*)(hookResultAddress + 1) = 0x90;
			*(unsigned char*)(hookResultAddress + 2) = 0x90;
			*(unsigned char*)(hookResultAddress + 3) = 0x90;
			*(unsigned char*)(hookResultAddress + 4) = 0x90;
			*(unsigned char*)(hookResultAddress + 5) = 0x90;
			*(unsigned char*)(hookResultAddress + 6) = 0x90;
			*(unsigned char*)(hookResultAddress + 7) = 0x90;
			*(unsigned char*)(hookResultAddress + 8) = 0x90;
			*(unsigned char*)(hookResultAddress + 9) = 0x90;
			*(unsigned char*)(hookResultAddress + 10) = 0x90;
			*(unsigned char*)(hookResultAddress + 11) = 0x90;
			*(unsigned char*)(hookResultAddress + 12) = 0x90;
			*(unsigned char*)(hookResultAddress + 13) = 0x90;
			*(unsigned char*)(hookResultAddress + 14) = 0x90;
			*(unsigned char*)(hookResultAddress + 15) = 0x90;
			VirtualProtect((void*)(hookResultAddress), 16, old_protect, &old_protect);
		}
	}*/
}