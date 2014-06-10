/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#include "CvDllInterfaces.h"

class CvBuildingEntry;

class CvDllBuildingInfo : public ICvBuildingInfo1
{
public:
	CvDllBuildingInfo(_In_ CvBuildingEntry* pBuildingInfo);
	~CvDllBuildingInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	const char* DLLCALL GetType() const;
	const char* DLLCALL GetText() const;
	int DLLCALL GetPreferredDisplayPosition() const;
	bool DLLCALL IsBorderObstacle() const;
	bool DLLCALL IsPlayerBorderObstacle() const;
	const char* DLLCALL GetArtDefineTag() const;
	const bool DLLCALL GetArtInfoCulturalVariation() const;
	const bool DLLCALL GetArtInfoEraVariation() const;
	const bool DLLCALL GetArtInfoRandomVariation() const;
	const char* DLLCALL GetWonderSplashAudio() const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvBuildingEntry* m_pBuildingInfo;
};