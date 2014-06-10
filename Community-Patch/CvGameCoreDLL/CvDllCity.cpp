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
#include "CvDllPlot.h"

CvDllCity::CvDllCity(CvCity* pCity)
: m_pCity(pCity)
, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllCity::~CvDllCity()
{
}
//------------------------------------------------------------------------------
void* CvDllCity::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
		guidInterface == ICvCity1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllCity::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllCity::DecrementReference()
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
unsigned int CvDllCity::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllCity::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllCity::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllCity::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvCity* CvDllCity::GetInstance()
{
	return m_pCity;
}
//------------------------------------------------------------------------------
int CvDllCity::GetID() const
{
	return m_pCity->GetID();
}
//------------------------------------------------------------------------------
PlayerTypes CvDllCity::GetOwner() const
{
	return m_pCity->getOwner();
}
//------------------------------------------------------------------------------
int CvDllCity::GetMaxHitPoints() const
{
	return m_pCity->GetMaxHitPoints();
}
//------------------------------------------------------------------------------
void CvDllCity::GetPosition(int& iX, int& iY) const
{
	iX = m_pCity->getX();
	iY = m_pCity->getY();
}
//------------------------------------------------------------------------------
ICvPlot1* CvDllCity::Plot() const
{
	CvPlot* pkPlot = m_pCity->plot();
	return (NULL != pkPlot)? new CvDllPlot(pkPlot) : NULL;
}
//------------------------------------------------------------------------------
int CvDllCity::GetPopulation() const
{
	return m_pCity->getPopulation();
}
//------------------------------------------------------------------------------
const char* CvDllCity::GetName() const
{
	return m_pCity->getNameKey();
}
//------------------------------------------------------------------------------
bool CvDllCity::CanBuyPlot(int iPlotX, int iPlotY, bool bIgnoreCost)
{
	return m_pCity->CanBuyPlot(iPlotX, iPlotY, bIgnoreCost);
}
//------------------------------------------------------------------------------
CitySizeTypes CvDllCity::GetCitySizeType() const
{
	return m_pCity->getCitySizeType();
}
//------------------------------------------------------------------------------
const char* CvDllCity::GetCityBombardEffectTag() const
{
	return m_pCity->GetCityBombardEffectTag();
}
//------------------------------------------------------------------------------
TeamTypes CvDllCity::GetTeam() const
{
	return m_pCity->getTeam();
}
//------------------------------------------------------------------------------
bool CvDllCity::IsPuppet() const
{
	return m_pCity->IsPuppet();
}
//------------------------------------------------------------------------------
int CvDllCity::GetX() const
{
	return m_pCity->getX();
}
//------------------------------------------------------------------------------
int CvDllCity::GetY() const
{
	return m_pCity->getY();
}
//------------------------------------------------------------------------------
int CvDllCity::GetStrengthValue(bool bForRangeStrike) const
{
	return m_pCity->getStrengthValue(bForRangeStrike);
}
//------------------------------------------------------------------------------
int CvDllCity::FoodDifference(bool bBottom) const
{
	return m_pCity->foodDifference(bBottom);
}
//------------------------------------------------------------------------------
int CvDllCity::GetFoodTurnsLeft() const
{
	return m_pCity->getFoodTurnsLeft();
}
//------------------------------------------------------------------------------
int CvDllCity::GetYieldRate(int eIndex) const
{
	if(eIndex >= -1 && eIndex < NUM_YIELD_TYPES)
		return m_pCity->getYieldRate(static_cast<YieldTypes>(eIndex));
	else
		return 0;
}
//------------------------------------------------------------------------------
int CvDllCity::GetJONSCulturePerTurn() const
{
	return m_pCity->getJONSCulturePerTurn();
}
//------------------------------------------------------------------------------
IDInfo CvDllCity::GetIDInfo() const
{
	return m_pCity->GetIDInfo();
}
//------------------------------------------------------------------------------
bool CvDllCity::IsWorkingPlot(ICvPlot1* pPlot) const
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	return (NULL != pkPlot)? m_pCity->GetCityCitizens()->IsWorkingPlot(pkPlot) : NULL;
}
//------------------------------------------------------------------------------
bool CvDllCity::CanWork(ICvPlot1* pPlot) const
{
	CvPlot* pkPlot = (NULL != pPlot)? static_cast<CvDllPlot*>(pPlot)->GetInstance() : NULL;
	return (NULL != pkPlot)? m_pCity->GetCityCitizens()->IsCanWork(pkPlot) : NULL;
}
//------------------------------------------------------------------------------
ICvPlot1* CvDllCity::GetCityPlotFromIndex(int iIndex) const
{
	CvPlot* pkPlot = m_pCity->GetCityCitizens()->GetCityPlotFromIndex(iIndex);
	return (NULL != pkPlot)? new CvDllPlot(pkPlot) : NULL;
}
//------------------------------------------------------------------------------
FAutoArchive& CvDllCity::GetSyncArchive()
{
	return m_pCity->getSyncArchive();
}
//------------------------------------------------------------------------------