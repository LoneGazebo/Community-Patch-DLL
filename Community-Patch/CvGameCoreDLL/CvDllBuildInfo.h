/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvBuildInfo;

class CvDllBuildInfo : public ICvBuildInfo1
{
public:
	CvDllBuildInfo(_In_ CvBuildInfo* pBuildInfo);
	~CvDllBuildInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new (size_t bytes);

	CvBuildInfo* GetInstance();

	const char* DLLCALL GetType();
	int DLLCALL GetImprovement();
	int DLLCALL GetRoute();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvBuildInfo* m_pBuildInfo;
};