/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllNetLoadGameInfo : public ICvNetLoadGameInfo1
{
public:
	CvDllNetLoadGameInfo();
	~CvDllNetLoadGameInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	bool DLLCALL Read(FDataStream& kStream);
	bool DLLCALL Write(FDataStream& kStream);
	bool DLLCALL Commit();

private:
	void DLLCALL Destroy();

	// pass the slot status thats in memory to handle kicked players. only used in multiplayer -tsmith 8.10.2010
	std::vector<SlotStatus> m_slotStatus;

	unsigned int m_uiRefCount;
};