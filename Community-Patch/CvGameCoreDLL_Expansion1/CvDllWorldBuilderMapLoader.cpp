/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvDllWorldBuilderMapLoader.h"
#include "CvDllContext.h"

#include "CvMapGenerator.h"
#include "CvWorldBuilderMapLoader.h"


CvDllWorldBuilderMapLoader::CvDllWorldBuilderMapLoader()
{
}
//------------------------------------------------------------------------------
CvDllWorldBuilderMapLoader::~CvDllWorldBuilderMapLoader()
{
}
//------------------------------------------------------------------------------
void* CvDllWorldBuilderMapLoader::QueryInterface(GUID guidInterface)
{
	if(	guidInterface == ICvUnknown::GetInterfaceId() ||
	    guidInterface == ICvWorldBuilderMapLoader1::GetInterfaceId() ||
		guidInterface == ICvWorldBuilderMapLoader2::GetInterfaceId())
	{
		return this;
	}

	return NULL;
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::Destroy()
{
	// Do nothing.
	// This is a static class whose instance is managed externally.
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::operator delete(void* p)
{
	CvDllGameContext::Free(p);
}
//------------------------------------------------------------------------------
void* CvDllWorldBuilderMapLoader::operator new(size_t bytes)
{
	return CvDllGameContext::Allocate(bytes);
}
//------------------------------------------------------------------------------
const CvWorldBuilderMapLoaderMapInfo& CvDllWorldBuilderMapLoader::GetCurrentMapInfo()
{
	return CvWorldBuilderMapLoader::GetCurrentMapInfo();
}
//------------------------------------------------------------------------------
bool CvDllWorldBuilderMapLoader::Preload(const wchar_t* wszFilename, bool bScenario)
{
	return CvWorldBuilderMapLoader::Preload(wszFilename, bScenario);
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::SetupGameOptions()
{
	CvWorldBuilderMapLoader::SetupGameOptions();
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::SetupPlayers()
{
	CvWorldBuilderMapLoader::SetupPlayers();
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::SetInitialItems(bool bFirstCall)
{
	CvWorldBuilderMapLoader::SetInitialItems(bFirstCall);
}
//------------------------------------------------------------------------------
bool CvDllWorldBuilderMapLoader::InitMap()
{
	return CvWorldBuilderMapLoader::InitMap();
}
//------------------------------------------------------------------------------
bool CvDllWorldBuilderMapLoader::Save(const wchar_t* wszFilename, const char* szMapName)
{
	return CvWorldBuilderMapLoader::Save(wszFilename, szMapName);
}
//------------------------------------------------------------------------------
int CvDllWorldBuilderMapLoader::LoadModData(lua_State* L)
{
	return CvWorldBuilderMapLoader::LoadModData(L);
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::ValidateTerrain()
{
	CvWorldBuilderMapLoader::ValidateTerrain();
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::ValidateCoast()
{
	CvWorldBuilderMapLoader::ValidateCoast();
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::ClearResources()
{
	CvWorldBuilderMapLoader::ClearResources();
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::ClearGoodies()
{
	CvWorldBuilderMapLoader::ClearGoodies();
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::ResetPlayerSlots()
{
	CvWorldBuilderMapLoader::ResetPlayerSlots();
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::MapPlayerToSlot(uint uiPlayer, PlayerTypes ePlayerSlot)
{
	CvWorldBuilderMapLoader::MapPlayerToSlot(uiPlayer, ePlayerSlot);
}
//------------------------------------------------------------------------------
unsigned int CvDllWorldBuilderMapLoader::PreviewPlayableCivCount(const wchar_t* wszFilename)
{
	return CvWorldBuilderMapLoader::PreviewPlayableCivCount(wszFilename);
}
//------------------------------------------------------------------------------
int CvDllWorldBuilderMapLoader::GetMapPreview(lua_State* L)
{
	return CvWorldBuilderMapLoader::GetMapPreview(L);
}
//------------------------------------------------------------------------------
int CvDllWorldBuilderMapLoader::GetMapPlayers(lua_State* L)
{
	return CvWorldBuilderMapLoader::GetMapPlayers(L);
}
//------------------------------------------------------------------------------
int CvDllWorldBuilderMapLoader::AddRandomItems(lua_State* L)
{
	return CvWorldBuilderMapLoader::AddRandomItems(L);
}
//------------------------------------------------------------------------------
int CvDllWorldBuilderMapLoader::ScatterResources(lua_State* L)
{
	return CvWorldBuilderMapLoader::ScatterResources(L);
}
//------------------------------------------------------------------------------
int CvDllWorldBuilderMapLoader::ScatterGoodies(lua_State* L)
{
	return CvWorldBuilderMapLoader::ScatterGoodies(L);
}
//------------------------------------------------------------------------------
PlayerTypes CvDllWorldBuilderMapLoader::GetMapPlayerSlot(uint uiPlayer)
{
	return CvWorldBuilderMapLoader::GetMapPlayerSlot(uiPlayer);
}
//------------------------------------------------------------------------------
int CvDllWorldBuilderMapLoader::GetMapPlayerCount()
{
	return CvWorldBuilderMapLoader::GetMapPlayerCount();
}
//------------------------------------------------------------------------------
void CvDllWorldBuilderMapLoader::GenerateRandomMap(const char* szMapScript)
{
	CvMapGenerator kGenerator(szMapScript);
	kGenerator.GenerateRandomMap();
}
//------------------------------------------------------------------------------
WorldSizeTypes CvDllWorldBuilderMapLoader::GetWorldSizeType() const
{
	return CvWorldBuilderMapLoader::GetCurrentWorldSizeType();
}
//------------------------------------------------------------------------------