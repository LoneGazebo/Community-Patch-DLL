#pragma once

// This file should be included by CvDLLInterfaces.h

/*
	Some guidelines...

	Design Guidelines:
	* Must derive from ICvUnknown either directly or indirectly.
	* Must have a GUID defined in the global scope.
	* Must contain an inline function GetInterfaceId() that returns the GUID.
	* Must not change after being defined.
	* All functions must be declared with DLLCALL as their calling convention.
	* Functions that return interface instances must be of type ICvUnknown* or version 1 of the interface type.

	Naming Conventions:
	* Interfaces must be prefixed with ICv
	* Interfaces other than ICvUnknown must be post-fixed with a version number starting at 1.
	* Globally scoped GUIDs that relate to specific interfaces must be named guid<InterfaceName>.

	Usage Guidelines
	* Interface instances must be explicitly deleted in order to be correctly released by the DLL.
	* It's safe and encouraged to use interface instances inside of auto_ptr and shared_ptr types.

	Implementation Guidelines:
	* QueryInterface must support all interfaces that the implementation derives from (including ICvUnknown).
	* New instances may be returned as a result of a QueryInterface call.
	* NULL must be returned if QueryInterface does not support the interface.
	* A single implementation may implement multiple interfaces.
	* It's the implementer's responsibility that interface instances are cleaned up properly via Destroy().
*/

#include "ICvDLLUserInterface.h"

// {9B606E9A-670C-46fb-BEC9-E66A2E8E726B}
static const GUID guidICvNetMessageHandler3 = 
{ 0x9b606e9a, 0x670c, 0x46fb, { 0xbe, 0xc9, 0xe6, 0x6a, 0x2e, 0x8e, 0x72, 0x6b } };

// {19EE7781-C484-414e-97C9-B74EBF046E7E}
static const GUID guidICvGame2 = 
{ 0x19ee7781, 0xc484, 0x414e, { 0x97, 0xc9, 0xb7, 0x4e, 0xbf, 0x4, 0x6e, 0x7e } };

// {75075151-0EF9-4b8b-ADFB-0860C1BA2FDF}
static const GUID guidICvPlayer2 = 
{ 0x75075151, 0xef9, 0x4b8b, { 0xad, 0xfb, 0x8, 0x60, 0xc1, 0xba, 0x2f, 0xdf } };

// {F5016D0D-684E-497f-A64D-4651AE713285}
static const GUID guidICvPlayer3 = 
{ 0xf5016d0d, 0x684e, 0x497f, { 0xa6, 0x4d, 0x46, 0x51, 0xae, 0x71, 0x32, 0x85 } };

// {9671D0AE-F2D1-4d17-8A8D-C211BB05D391}
static const GUID guidICvPreGame2 = 
{ 0x9671d0ae, 0xf2d1, 0x4d17, { 0x8a, 0x8d, 0xc2, 0x11, 0xbb, 0x5, 0xd3, 0x91 } };

// {CF7820CD-E960-41ef-9C5A-9D030C3BC30B}
static const GUID guidICvGameContext2 = 
{ 0xcf7820cd, 0xe960, 0x41ef, { 0x9c, 0x5a, 0x9d, 0x3, 0xc, 0x3b, 0xc3, 0xb } };

// {8921E7A6-A1FD-4301-94B9-D583CC102A11}
static const GUID guidICvGameContext3 = 
{ 0x8921e7a6, 0xa1fd, 0x4301, { 0x94, 0xb9, 0xd5, 0x83, 0xcc, 0x10, 0x2a, 0x11 } };

// {03C93CEC-650A-43de-9E1B-FE15D75EBA92}
static const GUID guidICvUnit2 = 
{ 0x3c93cec, 0x650a, 0x43de, { 0x9e, 0x1b, 0xfe, 0x15, 0xd7, 0x5e, 0xba, 0x92 } };

// {6F009D2A-247E-4818-AC1C-5197A8A79559}
static const GUID guidICvWorldBuilderMapLoader2 = 
{ 0x6f009d2a, 0x247e, 0x4818, { 0xac, 0x1c, 0x51, 0x97, 0xa8, 0xa7, 0x95, 0x59 } };



//------------------------------------------------------------------------------
// Game Interfaces
//------------------------------------------------------------------------------
class ICvGame2 : public ICvGame1
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvGame2; }

	virtual bool DLLCALL HasTurnTimerExpired(PlayerTypes playerID) = 0;
	virtual void DLLCALL TurnTimerSync(float fCurTurnTime, float fTurnStartTime) = 0;
	virtual void DLLCALL GetTurnTimerData(float& fCurTurnTime, float& fTurnStartTime) = 0;
	virtual void DLLCALL NetMessageStaticsReset() = 0;
	virtual void DLLCALL SetLastTurnAICivsProcessed() = 0;
	virtual bool DLLCALL GetGreatWorkAudio(int GreatWorkIndex, char* strSound, int length) = 0;
};

class ICvNetMessageHandler3 : public ICvNetMessageHandler2
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvNetMessageHandler3; }

	virtual void DLLCALL ResponseIgnoreWarning(PlayerTypes ePlayer, TeamTypes eRivalTeam) = 0;
	virtual void DLLCALL ResponseArchaeologyChoice(PlayerTypes ePlayer, ArchaeologyChoiceType eChoice) = 0;
	virtual void DLLCALL ResponseIdeologyChoice(PlayerTypes ePlayer, PolicyBranchTypes eChoice) = 0;
	virtual void DLLCALL ResponseLeagueVoteEnact(LeagueTypes eLeague, int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice) = 0;
	virtual void DLLCALL ResponseLeagueVoteRepeal(LeagueTypes eLeague, int iResolutionID, PlayerTypes eVoter, int iNumVotes, int iChoice) = 0;
	virtual void DLLCALL ResponseLeagueVoteAbstain(LeagueTypes eLeague, PlayerTypes eVoter, int iNumVotes) = 0;
	virtual void DLLCALL ResponseLeagueProposeEnact(LeagueTypes eLeague, ResolutionTypes eResolution, PlayerTypes eProposer, int iChoice) = 0;
	virtual void DLLCALL ResponseLeagueProposeRepeal(LeagueTypes eLeague, int iResolutionID, PlayerTypes eProposer) = 0;
	virtual void DLLCALL ResponseLeagueEditName(LeagueTypes eLeague, PlayerTypes ePlayer, const char* szCustomName) = 0;
	virtual void DLLCALL ResponseGoodyChoice(PlayerTypes ePlayer, int iPlotX, int iPlotY, GoodyTypes eGoody, int iUnitID) = 0;
	virtual void DLLCALL ResponseSetSwappableGreatWork(PlayerTypes ePlayer, int iWorkClass, int iWorkIndex) = 0;
	virtual void DLLCALL ResponseSwapGreatWorks(PlayerTypes ePlayer1, int iWorkIndex1, PlayerTypes PlayerTypes2, int iWorkIndex2) = 0;
	virtual void DLLCALL ResponseMoveGreatWorks(PlayerTypes ePlayer, int iCity1, int iBuildingClass1, int iWorkIndex1, 
																																	 int iCity2, int iBuildingClass2, int iWorkIndex2) = 0;
	virtual void DLLCALL ResponseChangeIdeology(PlayerTypes ePlayer) = 0;
};

class ICvPlayer2 : public ICvPlayer1
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPlayer2; }

	virtual bool DLLCALL IsSimultaneousTurns() const = 0;
	virtual bool DLLCALL HasTurnTimerExpired() = 0;
	virtual bool DLLCALL IsOptionKey(PlayerOptionTypes eOptionID) const = 0;
};

class ICvPlayer3 : public ICvPlayer2
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPlayer3; }

	virtual bool DLLCALL MayNotAnnex() = 0;
};


class ICvPreGame2 : public ICvPreGame1
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvPreGame2; }

	virtual int DLLCALL pitBossTurnTime() = 0;
	virtual void DLLCALL setPitBossTurnTime(int turnTime) = 0;

	virtual bool DLLCALL isTurnNotifySteamInvite(PlayerTypes p) const = 0;
	virtual void DLLCALL setTurnNotifySteamInvite(PlayerTypes p, bool wantsSteamInvite) = 0;

	virtual bool DLLCALL isTurnNotifyEmail(PlayerTypes p) const = 0;
	virtual void DLLCALL setTurnNotifyEmail(PlayerTypes p, bool wantsEmail) = 0;

	virtual const CvString& DLLCALL getTurnNotifyEmailAddress(PlayerTypes p) const = 0;
	virtual void DLLCALL setTurnNotifyEmailAddress(PlayerTypes p, const CvString& emailAddress) = 0;

	virtual void DLLCALL VerifyHandicap(PlayerTypes p) = 0;
	virtual void DLLCALL ReseatConnectedPlayers() = 0;
};

class ICvGameContext2 : public ICvGameContext1
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvGameContext2; }

	virtual void DLLCALL SetEngineUserInterface(ICvUserInterface2* pUI) = 0;
};

class ICvGameContext3 : public ICvGameContext2
{
public:
	static GUID DLLCALL GetInterfaceId() {return guidICvGameContext3; }

	virtual HANDLE DLLCALL Debug_GetHeap() const = 0;
};

class ICvUnit2 : public ICvUnit1
{
public:
	static GUID DLLCALL GetInterfaceId() { return guidICvUnit2; }

	virtual bool DLLCALL IsTrade() const = 0;
};

class ICvWorldBuilderMapLoader2 : public ICvWorldBuilderMapLoader1
{
public:
	static GUID DLLCALL GetInterfaceId() {return guidICvWorldBuilderMapLoader2; }

	virtual WorldSizeTypes DLLCALL GetWorldSizeType() const = 0;
};