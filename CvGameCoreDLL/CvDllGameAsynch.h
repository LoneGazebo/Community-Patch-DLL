/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllGameAsynch : public ICvGameAsynch1
{
public:
	CvDllGameAsynch(CvGame* pGame);
	~CvDllGameAsynch();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	virtual PlayerTypes DLLCALL GetActivePlayer();
	virtual TeamTypes	DLLCALL GetActiveTeam();
	virtual int			DLLCALL GetGameTurn() const;
	virtual GameSpeedTypes DLLCALL GetGameSpeedType() const;
	virtual GameStateTypes DLLCALL GetGameState();
	virtual HandicapTypes DLLCALL GetHandicapType() const;
	virtual int			DLLCALL IsAction(int iKeyStroke, bool bAlt, bool bShift, bool bCtrl);
	virtual bool		DLLCALL IsDebugMode() const;
	virtual bool		DLLCALL IsFinalInitialized() const;
	virtual bool		DLLCALL IsGameMultiPlayer() const;
	virtual bool		DLLCALL IsHotSeat() const;
	virtual bool		DLLCALL IsMPOption(MultiplayerOptionTypes eIndex) const;
	virtual bool		DLLCALL IsNetworkMultiPlayer() const;
	virtual bool		DLLCALL IsOption(GameOptionTypes eIndex) const;
	virtual bool		DLLCALL IsPaused();
	virtual bool		DLLCALL IsPbem() const;
	virtual bool		DLLCALL IsTeamGame() const;
	virtual bool		DLLCALL TunerEverConnected() const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvGame* m_pGame;
};
