/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvCivilizationInfo;

class CvDllCivilizationInfo : public ICvCivilizationInfo1
{
public:
	CvDllCivilizationInfo(_In_ CvCivilizationInfo* pCivilizationInfo);
	~CvDllCivilizationInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvCivilizationInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();
	bool DLLCALL IsAIPlayable();
	bool DLLCALL IsPlayable();
	const char* DLLCALL GetShortDescription();
	int DLLCALL GetDefaultPlayerColor();
	int DLLCALL GetArtStyleType();
	int DLLCALL GetNumCityNames();
	const char* DLLCALL GetArtStylePrefix();
	const char* DLLCALL GetArtStyleSuffix();
	const char* DLLCALL GetDawnOfManAudio();
	int DLLCALL GetCivilizationBuildings(int i);
	bool DLLCALL IsLeaders(int i);
	const char* DLLCALL GetCityNames(int i);
	const char* DLLCALL GetSoundtrackKey();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvCivilizationInfo* m_pCivilizationInfo;
};