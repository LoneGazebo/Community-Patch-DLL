/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvDllCity.h"
#include "CvDllContext.h"
#include "CvDllMap.h"
#include "CvDllPlot.h"

CvDllMap::CvDllMap(CvMap* pMap)
	: m_pMap(pMap)
	, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllMap::~CvDllMap()
{
}
//------------------------------------------------------------------------------
void* CvDllMap::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvMap1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllMap::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllMap::DecrementReference()
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
unsigned int CvDllMap::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllMap::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllMap::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllMap::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvMap* CvDllMap::GetInstance()
{
	return m_pMap;
}
//------------------------------------------------------------------------------
void CvDllMap::Init(CvMapInitData* pInitData)
{
	m_pMap->init(pInitData);
}
//------------------------------------------------------------------------------
void CvDllMap::Uninit()
{
	m_pMap->uninit();
}
//------------------------------------------------------------------------------
void CvDllMap::UpdateSymbolVisibility()
{
	// Unused
}
//------------------------------------------------------------------------------
void CvDllMap::UpdateLayout(bool bDebug)
{
	m_pMap->updateLayout(bDebug);
}
//------------------------------------------------------------------------------
void CvDllMap::UpdateDeferredFog()
{
	m_pMap->updateDeferredFog();
}
//------------------------------------------------------------------------------
ICvCity1* CvDllMap::FindCity(int iX,
                             int iY,
                             PlayerTypes eOwner,
                             TeamTypes eTeam ,
                             bool bSameArea ,
                             bool bCoastalOnly,
                             TeamTypes eTeamAtWarWith,
                             DirectionTypes eDirection,
                             ICvCity1* pSkipCity)
{
	CvCity* pkSkipCity = NULL;
	if(pSkipCity)
	{
		pkSkipCity = static_cast<CvDllCity*>(pSkipCity)->GetInstance();
	}

	CvCity* pkCity = m_pMap->findCity(iX, iY, eOwner, eTeam, bSameArea, bCoastalOnly, eTeamAtWarWith, eDirection, pkSkipCity);
	return (NULL != pkCity)? new CvDllCity(pkCity) : NULL;
}
//------------------------------------------------------------------------------
int CvDllMap::NumPlots() const
{
	return m_pMap->numPlots();
}
//------------------------------------------------------------------------------
int CvDllMap::PlotNum(int iX, int iY) const
{
	return m_pMap->plotNum(iX, iY);
}
//------------------------------------------------------------------------------
int CvDllMap::GetGridWidth() const
{
	return m_pMap->getGridWidth();
}
//------------------------------------------------------------------------------
int CvDllMap::GetGridHeight() const
{
	return m_pMap->getGridHeight();
}
//------------------------------------------------------------------------------
bool CvDllMap::IsWrapX() const
{
	return m_pMap->isWrapX();
}
//------------------------------------------------------------------------------
bool CvDllMap::IsWrapY() const
{
	return m_pMap->isWrapY();
}
//------------------------------------------------------------------------------
ICvPlot1* CvDllMap::GetPlotByIndex(int iIndex) const
{
	if (m_pMap->numPlots() != 0)
	{
		CvPlot* pkPlot = m_pMap->plotByIndex(iIndex);
		return (NULL != pkPlot)? new CvDllPlot(pkPlot) : NULL;
	}
	return NULL;
}
//------------------------------------------------------------------------------
ICvPlot1* CvDllMap::GetPlot(int iX, int iY) const
{
	if (m_pMap->numPlots() != 0)
	{
		CvPlot* pkPlot = m_pMap->plot(iX, iY);
		return (NULL != pkPlot)? new CvDllPlot(pkPlot) : NULL;
	}
	return NULL;
}
//------------------------------------------------------------------------------
void CvDllMap::RecalculateLandmasses()
{
	m_pMap->recalculateLandmasses();
}
//------------------------------------------------------------------------------
void CvDllMap::Read(FDataStream& kStream)
{
	m_pMap->Read(kStream);
}
//------------------------------------------------------------------------------
void CvDllMap::Write(FDataStream& kStream) const
{
	m_pMap->Write(kStream);
}
//------------------------------------------------------------------------------
int CvDllMap::Validate()
{
	return m_pMap->Validate();
}
//------------------------------------------------------------------------------