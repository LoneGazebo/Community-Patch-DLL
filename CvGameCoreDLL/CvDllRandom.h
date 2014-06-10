/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"

class CvDllRandom : public ICvRandom1
{
public:
	CvDllRandom(_In_ CvRandom* pRandom);
	~CvDllRandom();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvRandom* GetInstance();

	void DLLCALL Init(unsigned long ulSeed);
	void DLLCALL Reset(unsigned long ulSeed = 0);

	void DLLCALL CopyFrom(ICvRandom1* pOther);

	unsigned short DLLCALL Get(unsigned short usNum, const char* pszLog = NULL);  //  Returns value from 0 to num-1 inclusive.
	float DLLCALL GetFloat();

	unsigned long DLLCALL GetSeed() const;

	void DLLCALL Read(FDataStream& kStream);
	void DLLCALL Write(FDataStream& kStream) const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	CvRandom* m_pRandom;
};