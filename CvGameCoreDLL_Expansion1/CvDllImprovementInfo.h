/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "CvDllInterfaces.h"

class CvDllImprovementInfo : public ICvImprovementInfo1
{
public:
	CvDllImprovementInfo(_In_ CvImprovementEntry* pImprovementInfo);
	~CvDllImprovementInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	const char* DLLCALL GetType() const;
	const char* DLLCALL GetText() const;
	bool DLLCALL IsWater() const;
	bool DLLCALL IsDestroyedWhenPillaged() const;
	bool DLLCALL IsGoody() const;
	const char* DLLCALL GetArtDefineTag() const;
	ImprovementUsageTypes DLLCALL GetImprovementUsage() const;
	int DLLCALL GetWorldSoundscapeScriptId() const;
	bool DLLCALL GetTerrainMakesValid(int i) const;
	bool DLLCALL IsImprovementResourceMakesValid(int i) const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvImprovementEntry* m_pImprovementInfo;
};