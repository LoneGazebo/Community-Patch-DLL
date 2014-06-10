/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvHandicapInfo;

class CvDllHandicapInfo : public ICvHandicapInfo1
{
public:
	CvDllHandicapInfo(_In_ CvHandicapInfo* pHandicapInfo);
	~CvDllHandicapInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new (size_t bytes);

	CvHandicapInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();
	int DLLCALL GetBarbSpawnMod();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvHandicapInfo* m_pHandicapInfo;
};