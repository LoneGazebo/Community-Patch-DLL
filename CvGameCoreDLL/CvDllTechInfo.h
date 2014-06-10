/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once

#include "CvDllInterfaces.h"

class CvDllTechInfo : public ICvTechInfo1
{
public:
	CvDllTechInfo(_In_ CvTechEntry* pTechInfo);
	~CvDllTechInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new (size_t bytes);

	const char* DLLCALL GetDescription() const;
	int DLLCALL GetEra() const;
	const char* DLLCALL GetSound() const;
	const char* DLLCALL GetSoundMP() const;
	const char* DLLCALL GetType() const;
	const char* DLLCALL GetText() const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvTechEntry* m_pTechInfo;
};