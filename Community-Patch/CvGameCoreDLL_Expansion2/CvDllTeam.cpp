/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllTeam.h"
#include "CvDllContext.h"

CvDllTeam::CvDllTeam(CvTeam* pTeam)
	: m_pTeam(pTeam)
	, m_uiRefCount(1)
{
}
//------------------------------------------------------------------------------
CvDllTeam::~CvDllTeam()
{
}
//------------------------------------------------------------------------------
void* CvDllTeam::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvTeam1::GetInterfaceId())
	{
		IncrementReference();
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
unsigned int CvDllTeam::IncrementReference()
{
	++m_uiRefCount;
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
unsigned int CvDllTeam::DecrementReference()
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
unsigned int CvDllTeam::GetReferenceCount()
{
	return m_uiRefCount;
}
//------------------------------------------------------------------------------
void CvDllTeam::Destroy()
{
	DecrementReference();
}
//------------------------------------------------------------------------------
void CvDllTeam::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllTeam::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
CvTeam* CvDllTeam::GetInstance()
{
	return m_pTeam;
}
//------------------------------------------------------------------------------
bool CvDllTeam::CanEmbarkAllWaterPassage() const
{
	return m_pTeam->canEmbarkAllWaterPassage();
}
//------------------------------------------------------------------------------
int CvDllTeam::GetAtWarCount(bool bIgnoreMinors) const
{
	return m_pTeam->getAtWarCount(bIgnoreMinors);
}
//------------------------------------------------------------------------------
EraTypes CvDllTeam::GetCurrentEra() const
{
	return m_pTeam->GetCurrentEra();
}
//------------------------------------------------------------------------------
PlayerTypes CvDllTeam::GetLeaderID() const
{
	return m_pTeam->getLeaderID();
}
//------------------------------------------------------------------------------
int CvDllTeam::GetProjectCount(ProjectTypes eIndex) const
{
	return m_pTeam->getProjectCount(eIndex);
}
//------------------------------------------------------------------------------
int CvDllTeam::GetTotalSecuredVotes() const
{
	return m_pTeam->GetTotalSecuredVotes();
}
//------------------------------------------------------------------------------
void CvDllTeam::Init(TeamTypes eID)
{
	m_pTeam->init(eID);
}
//------------------------------------------------------------------------------
bool CvDllTeam::IsAlive() const
{
	return m_pTeam->isAlive();
}
//------------------------------------------------------------------------------
bool CvDllTeam::IsAtWar(TeamTypes eIndex) const
{
	return m_pTeam->isAtWar(eIndex);
}
//------------------------------------------------------------------------------
bool CvDllTeam::IsBarbarian() const
{
	return m_pTeam->isBarbarian();
}
//------------------------------------------------------------------------------
bool CvDllTeam::IsBridgeBuilding() const
{
	return m_pTeam->isBridgeBuilding();
}
//------------------------------------------------------------------------------
bool CvDllTeam::IsHasMet(TeamTypes eIndex) const
{
	return m_pTeam->isHasMet(eIndex);
}
//------------------------------------------------------------------------------
bool CvDllTeam::IsHomeOfUnitedNations() const
{
	return m_pTeam->IsHomeOfUnitedNations();
}
//------------------------------------------------------------------------------
void CvDllTeam::Uninit()
{
	m_pTeam->uninit();
}
//------------------------------------------------------------------------------
void CvDllTeam::Read(FDataStream& kStream)
{
	m_pTeam->Read(kStream);
}
//------------------------------------------------------------------------------
void CvDllTeam::Write(FDataStream& kStream) const
{
	m_pTeam->Write(kStream);
}
//------------------------------------------------------------------------------
int CvDllTeam::GetTechCount() const
{
	CvTeamTechs* pkTeamTechs = m_pTeam->GetTeamTechs();
	if (pkTeamTechs)
	{
		return pkTeamTechs->GetNumTechsKnown();
	}
	return 0;
}
//------------------------------------------------------------------------------
int CvDllTeam::GetTechs(TechTypes* pkTechArray, uint uiArraySize) const
{
	int iFoundTechs = 0;
	CvTeamTechs* pkTeamTechs = m_pTeam->GetTeamTechs();
	if (pkTeamTechs)
	{
		for (int iTechLoop = 0; iTechLoop < GC.getNumTechInfos() && uiArraySize--; iTechLoop++)
		{
			if (pkTeamTechs->HasTech((TechTypes) iTechLoop) || pkTeamTechs->GetTechCount((TechTypes)iTechLoop) > 0)
			{
				*pkTechArray++ = (TechTypes)iTechLoop;
				++iFoundTechs;
			}
		}
	}
	return iFoundTechs;
}
//------------------------------------------------------------------------------
