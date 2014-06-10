/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvMapGenerator.h"
#include "CvGame.h"
#include "cvStopWatch.h"

// must be included after all other headers
#include "LintFree.h"

struct GetMapInitDataArgs
{
	ICvEngineScriptSystem1* pkScriptSystem;
	CvMapInitData* pkMapInitData;
	int WorldSize;
	bool bSuccess;
};
//------------------------------------------------------------------------------
struct GetGameInitialItemsOverridesArgs
{
	ICvEngineScriptSystem1* pkScriptSystem;
	CvGameInitialItemsOverrides* pkOverrides;
	bool bSuccess;
};
//------------------------------------------------------------------------------
CvMapGenerator::CvMapGenerator(const char* szMapScriptName)
: m_pkLuaState(NULL)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	m_pkLuaState = pkScriptSystem->CreateLuaThread("Map Script");
	const bool bLoaded = pkScriptSystem->LoadFile(m_pkLuaState, szMapScriptName);
	DEBUG_VARIABLE(bLoaded);
	CvAssertMsg(bLoaded, "Cannot load mapscript.");
}
//------------------------------------------------------------------------------
CvMapGenerator::~CvMapGenerator()
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	pkScriptSystem->FreeLuaThread(m_pkLuaState);
}
//------------------------------------------------------------------------------
bool CvMapGenerator::GetMapInitData(CvMapInitData& kData, WorldSizeTypes eWorldSize)
{
	CvWorldInfo	kWorldInfo;
	Database::Connection& db = *GC.GetGameDatabase();

	//Query
	Database::Results kQuery;
	db.Execute(kQuery, "SELECT * from Worlds where ID = ? LIMIT 1");
	kQuery.Bind(1, eWorldSize);

	if(kQuery.Step())
	{
		kWorldInfo.CacheResult(kQuery);
	}
	else
	{
		CvAssertMsg(false, "Could not find world size entry.")
	}

	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();

	kData.m_iGridH = kWorldInfo.getGridHeight();
	kData.m_iGridW = kWorldInfo.getGridWidth();

	GetMapInitDataArgs args;
	args.pkScriptSystem = pkScriptSystem;
	args.pkMapInitData = &kData;
	args.WorldSize = kWorldInfo.GetID();
	args.bSuccess = false;

	pkScriptSystem->CallCFunction(m_pkLuaState, pGetMapInitData, &args);
	return args.bSuccess;
}
//------------------------------------------------------------------------------------------------
void CvMapGenerator::GenerateRandomMap()
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();

	pkScriptSystem->CallCFunction(m_pkLuaState, pGenerateRandomMap, pkScriptSystem);

	lua_gc(m_pkLuaState, LUA_GCCOLLECT, 0);
}
//------------------------------------------------------------------------------
bool CvMapGenerator::GetGameInitialItemsOverrides(CvGameInitialItemsOverrides& kOverrides)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();

	GetGameInitialItemsOverridesArgs args;
	args.pkScriptSystem = pkScriptSystem;
	args.pkOverrides = &kOverrides;
	args.bSuccess = false;

	pkScriptSystem->CallCFunction(m_pkLuaState, pGetGameInitialItemsOverrides, &args);
	return args.bSuccess;
}
//------------------------------------------------------------------------------
int CvMapGenerator::pGetMapInitData(lua_State* L)
{
	cvStopWatch stopWatch("CvMapGenerator - GetMapInitData()");
	GetMapInitDataArgs* pArgs = (GetMapInitDataArgs*)lua_touserdata(L, 1);

	pArgs->bSuccess = false;
	lua_getglobal(L, "GetMapInitData");
	if(lua_isfunction(L, -1))
	{
		lua_pushinteger(L, pArgs->WorldSize);
		pArgs->pkScriptSystem->CallFunction(L, 1, 1);
		const int t = lua_gettop(L);
		if(lua_istable(L, t))
		{
			lua_getfield(L, t, "Width");
			if(!lua_isnoneornil(L, -1))
				pArgs->pkMapInitData->m_iGridW = lua_tointeger(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, t, "Height");
			if(!lua_isnoneornil(L, -1))
				pArgs->pkMapInitData->m_iGridH = lua_tointeger(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, t, "WrapX");
			if(!lua_isnoneornil(L, -1))
				pArgs->pkMapInitData->m_bWrapX = lua_toboolean(L, -1);
			lua_pop(L, 1);

			lua_getfield(L, t, "WrapY");
			if(!lua_isnoneornil(L, -1))
				pArgs->pkMapInitData->m_bWrapY = lua_toboolean(L, -1);

			lua_getfield(L, t, "TopLatitude");
			if(!lua_isnoneornil(L, -1))
				pArgs->pkMapInitData->m_iTopLatitude = lua_tointeger(L, -1);

			lua_getfield(L, t, "BottomLatitude");
			if(!lua_isnoneornil(L, -1))
				pArgs->pkMapInitData->m_iBottomLatitude = lua_tointeger(L, -1);

			pArgs->bSuccess = true;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvMapGenerator::pGetGameInitialItemsOverrides(lua_State* L)
{
	cvStopWatch stopWatch("CvMapGenerator - GetGameInitialItemsOverrides()");
	GetGameInitialItemsOverridesArgs* pArgs = (GetGameInitialItemsOverridesArgs*)lua_touserdata(L, 1);

	pArgs->bSuccess = false;
	lua_getglobal(L, "GetGameInitialItemsOverrides");
	if(lua_isfunction(L, -1))
	{
		pArgs->pkScriptSystem->CallFunction(L, 0, 1);
		const int t = lua_gettop(L);
		if(lua_istable(L, t))
		{

			//Initial Free Techs per Team
			lua_getfield(L, t, "GrantInitialFreeTechsPerTeam");
			if(lua_istable(L, -1))
			{
				lua_pushnil(L);  /* first key */
				while (lua_next(L, -2) != 0)
				{
					if(lua_isnumber(L, -2) && lua_isboolean(L, -1))
					{
						const TeamTypes eTeam = static_cast<TeamTypes>(lua_tointeger(L, -2));
						const bool bOverride = lua_toboolean(L, -1);
						if(eTeam >= 0 && eTeam < MAX_TEAMS)
						{
							pArgs->pkOverrides->GrantInitialFreeTechsPerTeam[eTeam] = bOverride;
						}
					}
					lua_pop(L, 1);
				}
			}
			lua_pop(L, 1);

			//Initial Gold per Player
			lua_getfield(L, t, "GrantInitialGoldPerPlayer");
			if(lua_istable(L, -1))
			{
				lua_pushnil(L);  /* first key */
				while (lua_next(L, -2) != 0)
				{
					if(lua_isnumber(L, -2) && lua_isboolean(L, -1))
					{
						const PlayerTypes ePlayer = static_cast<PlayerTypes>(lua_tointeger(L, -2));
						const bool bOverride = lua_toboolean(L, -1);
						if(ePlayer >= 0 && ePlayer < MAX_PLAYERS)
						{
							pArgs->pkOverrides->GrantInitialGoldPerPlayer[ePlayer] = bOverride;
						}
					}
					lua_pop(L, 1);
				}
			}
			lua_pop(L, 1);

			//Initial Culture per Player
			lua_getfield(L, t, "GrantInitialCulturePerPlayer");
			if(lua_istable(L, -1))
			{
				lua_pushnil(L);  /* first key */
				while (lua_next(L, -2) != 0)
				{
					if(lua_isnumber(L, -2) && lua_isboolean(L, -1))
					{
						const PlayerTypes ePlayer = static_cast<PlayerTypes>(lua_tointeger(L, -2));
						const bool bOverride = lua_toboolean(L, -1);
						if(ePlayer >= 0 && ePlayer < MAX_PLAYERS)
						{
							pArgs->pkOverrides->GrantInitialCulturePerPlayer[ePlayer] = bOverride;
						}
					}
					lua_pop(L, 1);
				}
			}
			lua_pop(L, 1);

			//Clear research queue per Player
			lua_getfield(L, t, "ClearResearchQueuePerPlayer");
			if(lua_istable(L, -1))
			{
				lua_pushnil(L);  /* first key */
				while (lua_next(L, -2) != 0)
				{
					if(lua_isnumber(L, -2) && lua_isboolean(L, -1))
					{
						const PlayerTypes ePlayer = static_cast<PlayerTypes>(lua_tointeger(L, -2));
						const bool bOverride = lua_toboolean(L, -1);
						if(ePlayer >= 0 && ePlayer < MAX_PLAYERS)
						{
							pArgs->pkOverrides->ClearResearchQueuePerPlayer[ePlayer] = bOverride;
						}
					}
					lua_pop(L, 1);
				}
			}
			lua_pop(L, 1);

			//Initial Units per Player
			lua_getfield(L, t, "GrantInitialUnitsPerPlayer");
			if(lua_istable(L, -1))
			{
				lua_pushnil(L);  /* first key */
				while (lua_next(L, -2) != 0)
				{
					if(lua_isnumber(L, -2) && lua_isboolean(L, -1))
					{
						const PlayerTypes ePlayer = static_cast<PlayerTypes>(lua_tointeger(L, -2));
						const bool bOverride = lua_toboolean(L, -1);
						if(ePlayer >= 0 && ePlayer < MAX_PLAYERS)
						{
							pArgs->pkOverrides->GrantInitialUnitsPerPlayer[ePlayer] = bOverride;
						}
					}
					lua_pop(L, 1);
				}
			}
			lua_pop(L, 1);
			pArgs->bSuccess = true;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------
int CvMapGenerator::pGenerateRandomMap(lua_State* L)
{
	cvStopWatch stopWatch("CvMapGenerator - GenerateRandomMap()");
	ICvEngineScriptSystem1* pkScriptSystem = (ICvEngineScriptSystem1*)lua_touserdata(L, 1);

	lua_getglobal(L, "GenerateMap");
	if(lua_isnil(L, -1))
		luaL_error(L, "Cannot find map script entry point GenerateMap()");

	pkScriptSystem->CallFunction(L, 0, 0);

	return 0;
}
//------------------------------------------------------------------------------

