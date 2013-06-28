/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "CvDllInterfaces.h"

class CvDllNetworkSyncronization : public ICvNetworkSyncronization1
{
public:
	CvDllNetworkSyncronization();
	~CvDllNetworkSyncronization();

	void* DLLCALL QueryInterface(GUID guidInterface);

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	void DLLCALL ClearCityDeltas();
	void DLLCALL ClearPlayerDeltas();
	void DLLCALL ClearPlotDeltas();
	void DLLCALL ClearRandomDeltas();
	void DLLCALL ClearUnitDeltas();

	void DLLCALL SyncCities();
	void DLLCALL SyncPlayers();
	void DLLCALL SyncPlots();
	void DLLCALL SyncUnits();

private:
	void DLLCALL Destroy();
};