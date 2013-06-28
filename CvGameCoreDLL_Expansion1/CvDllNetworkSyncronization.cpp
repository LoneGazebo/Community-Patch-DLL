/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllNetworkSyncronization.h"
#include "CvDllContext.h"
#include "CvCity.h"
#include "CvPlayer.h"
#include "CvPlot.h"
#include "CvUnit.h"


CvDllNetworkSyncronization::CvDllNetworkSyncronization()
{
}
//------------------------------------------------------------------------------
CvDllNetworkSyncronization::~CvDllNetworkSyncronization()
{
}
//------------------------------------------------------------------------------
void* CvDllNetworkSyncronization::QueryInterface(GUID guidInterface)
{
	if(guidInterface == ICvUnknown::GetInterfaceId() ||
	        guidInterface == ICvNetworkSyncronization1::GetInterfaceId())
	{
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::Destroy()
{
	// Do nothing.
	// This is a static class whose instance is managed externally.
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllNetworkSyncronization::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::ClearCityDeltas()
{
	FSerialization::ClearCityDeltas();
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::ClearPlayerDeltas()
{
	FSerialization::ClearPlayerDeltas();
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::ClearPlotDeltas()
{
	FSerialization::ClearPlotDeltas();
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::ClearRandomDeltas()
{
	GC.getGame().getJonRand().clearCallstacks();
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::ClearUnitDeltas()
{
	FSerialization::ClearUnitDeltas();
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::SyncCities()
{
	FSerialization::SyncCities();
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::SyncPlayers()
{
	FSerialization::SyncPlayer();
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::SyncPlots()
{
	FSerialization::SyncPlots();
}
//------------------------------------------------------------------------------
void CvDllNetworkSyncronization::SyncUnits()
{
	FSerialization::SyncUnits();
}
//------------------------------------------------------------------------------