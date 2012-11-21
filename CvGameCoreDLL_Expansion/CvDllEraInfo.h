/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvEraInfo;
class CvDllEraInfo : public ICvEraInfo1
{
public:
	CvDllEraInfo(_In_ CvEraInfo* pEraInfo);
	~CvDllEraInfo();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvEraInfo* GetInstance();

	const char* DLLCALL GetType();
	const char* DLLCALL GetDescription();
	int DLLCALL GetNumEraVOs();
	const char* DLLCALL GetEraVO(int iIndex);
	const char* DLLCALL GetArtPrefix();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvEraInfo* m_pEraInfo;
};