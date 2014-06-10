/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDllRandom.h"
#include "CvDllCity.h"
#include "CvDllUnit.h"
#include "CvDllContext.h"

CvDllRandom::CvDllRandom(CvRandom* pRandom)
: m_pRandom(pRandom)
, m_uiRefCount(1)
{
	FAssertMsg(pRandom != NULL, "SHOULD NOT HAPPEN");
}
//------------------------------------------------------------------------------
CvDllRandom::~CvDllRandom()
{
}
//------------------------------------------------------------------------------
void* CvDllRandom::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvRandom1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllRandom::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllRandom::DecrementReference()
{
	if(m_uiRefCount == 1)
	{
		delete this;
		return 0;
	}
	else
	{
		--m_uiRefCount;
		return m_uiRefCount;
	}
}
//------------------------------------------------------------------------------
unsigned int CvDllRandom::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllRandom::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllRandom::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllRandom::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvRandom* CvDllRandom::GetInstance()
{
	return m_pRandom;
}
//------------------------------------------------------------------------------
void CvDllRandom::Init(unsigned long ulSeed)
{
	m_pRandom->init(ulSeed);
}
//------------------------------------------------------------------------------
void CvDllRandom::Reset(unsigned long ulSeed)
{
	m_pRandom->reset(ulSeed);
}
//------------------------------------------------------------------------------
void CvDllRandom::CopyFrom(ICvRandom1* pOther)
{
	CvRandom* pkOther = GC.UnwrapRandomPointer(pOther);
	if(pkOther != NULL)
		*m_pRandom = *pkOther;
}
//------------------------------------------------------------------------------
unsigned short CvDllRandom::Get(unsigned short usNum, const char* pszLog)
{
	return m_pRandom->get(usNum, pszLog);
}
//------------------------------------------------------------------------------
float CvDllRandom::GetFloat()
{
	return m_pRandom->getFloat();
}
//------------------------------------------------------------------------------
unsigned long CvDllRandom::GetSeed() const
{
	return m_pRandom->getSeed();
}
//------------------------------------------------------------------------------
void CvDllRandom::Read(FDataStream& kStream)
{
	kStream >> *m_pRandom;
}
//------------------------------------------------------------------------------
void CvDllRandom::Write(FDataStream& kStream) const
{
	kStream << *m_pRandom;
}
//------------------------------------------------------------------------------