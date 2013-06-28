/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvMinorCivInfo;

class CvDllMinorCivInfo : public ICvMinorCivInfo1
{
public:
	CvDllMinorCivInfo(_In_ CvMinorCivInfo* pMinorCivInfo);
	~CvDllMinorCivInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvMinorCivInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();
	int DLLCALL GetDefaultPlayerColor() const;
	int DLLCALL GetArtStyleType() const;
	int DLLCALL GetNumCityNames() const;
	const char* DLLCALL GetArtStylePrefix() const;
	const char* DLLCALL GetArtStyleSuffix() const;
	const char* DLLCALL GetCityNames(int i) const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvMinorCivInfo* m_pMinorCivInfo;
};