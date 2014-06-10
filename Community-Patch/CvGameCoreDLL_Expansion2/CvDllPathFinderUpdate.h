/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once
#include "CvDllInterfaces.h"


class CvDllPathFinderUpdate : public ICvPathFinderUpdate1
{
public:
	CvDllPathFinderUpdate(int iX, int iY, int iTurnNumber);
	~CvDllPathFinderUpdate();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	CvMinorCivInfo* GetInstance();

	int DLLCALL GetX() const;
	int DLLCALL GetY() const;
	int DLLCALL GetTurnNumber() const;

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	int m_iX;
	int m_iY;
	int m_iTurnNumber;
};
//------------------------------------------------------------------------------
struct CvDllPathFinderUpdateListData
{
	int iX;
	int iY;
	int iTurnNumber;
};
//------------------------------------------------------------------------------
class CvDllPathFinderUpdateList : public ICvEnumerator
{
public:
	CvDllPathFinderUpdateList(const std::vector<CvDllPathFinderUpdateListData>& updates);
	~CvDllPathFinderUpdateList();

	void* DLLCALL QueryInterface(GUID guidInterface);

	unsigned int IncrementReference();
	unsigned int DecrementReference();
	unsigned int GetReferenceCount();

	static void operator delete(void* p);
	static void* operator new(size_t bytes);

	bool DLLCALL MoveNext();
	void DLLCALL Reset();

	ICvUnknown* DLLCALL GetCurrent();

private:
	void DLLCALL Destroy();

	unsigned int m_uiRefCount;
	const std::vector<CvDllPathFinderUpdateListData> m_updates;
	int m_iIndex;
};
//------------------------------------------------------------------------------