/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include <CvGameCoreDLLPCH.h>
#include "CvLuaSupport.h"
#include "CvLuaFractal.h"
#include "..\CvFractal.h"

/*
	The following exposes CvFractal to Lua using this API:

	frac = Fractal.Create(int iNewXs, int iNewYs, int iGrain, flags, int iFracXExp, int iFracYExp)
	frac = Fractal.CreateRidged(int iNewXs, int iNewYs, int iGrain, flags, CyFractal& pRifts, int iFracXExp, int iFracYExp)
	frac = Fractal.CreateHinted()

	frac:GetHeight(x, y)
	frac:GetHeight(percentage)

	frac:BuildRidges()
*/

//------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------
bool HasFlag(lua_State *L, int idx, const char* flag)
{
	lua_getfield(L, idx, flag);
	return !lua_isnil(L, -1);
}
//------------------------------------------------------------------------------
int GetFractalFlags(lua_State* L, int idx)
{
	int flags = 0;
	luaL_checktype(L, idx, LUA_TTABLE);

	const int top = lua_gettop(L);

	if(HasFlag(L, idx, "FRAC_WRAP_X"))
		flags |= CvFractal::FRAC_WRAP_X;

	if(HasFlag(L, idx, "FRAC_WRAP_Y"))
		flags |= CvFractal::FRAC_WRAP_Y;

	if(HasFlag(L, idx, "FRAC_PERCENT"))
		flags |= CvFractal::FRAC_PERCENT;

	if(HasFlag(L, idx, "FRAC_POLAR"))
		flags |= CvFractal::FRAC_POLAR;

	if(HasFlag(L, idx, "FRAC_CENTER_RIFT"))
		flags |= CvFractal::FRAC_CENTER_RIFT;

	if(HasFlag(L, idx, "FRAC_INVERT_HEIGHTS"))
		flags |= CvFractal::FRAC_INVERT_HEIGHTS;

	lua_settop(L, top);

	return flags;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CvLuaFractal::Register(lua_State* L)
{
	FLua::Details::CCallWithErrorHandling(L, pRegister);
}
//-----------------------------------------------------------------------------------
int CvLuaFractal::pRegister(lua_State* L)
{
	lua_getglobal(L, "Fractal");
	if(lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_createtable(L, 0, 3);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "Fractal");
	}

	lua_pushcclosure(L, lCreate, 0);
	lua_setfield(L, -2, "Create");

	lua_pushcclosure(L, lCreateRifts, 0);
	lua_setfield(L, -2, "CreateRifts");

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaFractal::lBuildRidges(lua_State* L)
{
	//assume 'self' was pushed
	luaL_checktype(L, 1, LUA_TUSERDATA);
	CvFractal* pFractal = static_cast<CvFractal*>(lua_touserdata(L, 1));
	CvRandom& mapRand = GC.getGame().getMapRand();

	const int iNumSeeds		= lua_tointeger(L, 2);
	const int flags			= GetFractalFlags(L, 3);
	const int iBlendRidge	= lua_tointeger(L, 4);
	const int iBlendFract	= lua_tointeger(L, 5);

	pFractal->ridgeBuilder(mapRand, iNumSeeds, flags, iBlendRidge, iBlendFract);
	return 0;
}
//------------------------------------------------------------------------------
CvFractal* CvLuaFractal::CreateFractal(lua_State* L)
{
	//This will use Lua's memory manager and Lua is responsible for destruction.
	void* ptr = lua_newuserdata(L, sizeof(CvFractal));
	CvFractal* pFractal = new(ptr) CvFractal();

	lua_newtable(L);

	//__index
	lua_newtable(L);

	lua_pushvalue(L, -3);				//push userdata as upvalue
	lua_pushcclosure(L, lGetHeight, 1);
	lua_setfield(L, -2, "GetHeight");

	lua_pushvalue(L, -3);				//push userdata as upvalue
	lua_pushcclosure(L, lBuildRidges, 1);
	lua_setfield(L, -2, "BuildRidges");

	lua_setfield(L, -2, "__index");

	//__gc
	lua_pushcclosure(L, lDestroy, 0);
	lua_setfield(L, -2, "__gc");

	lua_setmetatable(L, -2);

	return pFractal;
}
//------------------------------------------------------------------------------
int CvLuaFractal::lCreate(lua_State* L)
{
	CvFractal* pFractal = CreateFractal(L);

	const int iNewXs = lua_tointeger(L, 1);
	const int iNewYs = lua_tointeger(L, 2);
	const int iGrain = lua_tointeger(L, 3);
	CvRandom& rand = GC.getGame().getMapRand();	//Temporary!!

	//calculate flags
	const int flags = GetFractalFlags(L, 4);

	const int iFracXExp = lua_isnil(L, 5)? CvFractal::DEFAULT_FRAC_X_EXP : lua_tointeger(L, 5);
	const int iFracYExp = lua_isnil(L, 6)? CvFractal::DEFAULT_FRAC_Y_EXP : lua_tointeger(L, 6);


	pFractal->fracInit(iNewXs, iNewYs, iGrain, rand, flags, NULL, iFracXExp, iFracYExp);

	return 1;
}
//------------------------------------------------------------------------------
int CvLuaFractal::lCreateRifts(lua_State* L)
{
	CvFractal* pFractal = CreateFractal(L);

	const int iNewXs = lua_tointeger(L, 1);
	const int iNewYs = lua_tointeger(L, 2);
	const int iGrain = lua_tointeger(L, 3);
	CvRandom& rand = GC.getGame().getMapRand();	//Temporary!!

	//calculate flags
	const int flags = GetFractalFlags(L, 4);

	CvFractal* pRidgeFrac = static_cast<CvFractal*>(lua_touserdata(L, 5));

	const int iFracXExp = luaL_optint(L, 6, CvFractal::DEFAULT_FRAC_X_EXP);
	const int iFracYExp = luaL_optint(L, 7, CvFractal::DEFAULT_FRAC_Y_EXP);

	pFractal->fracInit(iNewXs, iNewYs, iGrain, rand, flags, pRidgeFrac, iFracXExp, iFracYExp);
	return 1;
}
//------------------------------------------------------------------------------
int CvLuaFractal::lDestroy(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TUSERDATA);
	CvFractal* pFractal = static_cast<CvFractal*>(lua_touserdata(L, 1));

	//explicitly call destructor since we used placement new
	pFractal->~CvFractal();

	return 0;
}
//------------------------------------------------------------------------------
int CvLuaFractal::lGetHeight(lua_State* L)
{
	//assume 'self' was pushed
	luaL_checktype(L, 1, LUA_TUSERDATA);
	CvFractal* pFractal = static_cast<CvFractal*>(lua_touserdata(L, 1));

	int nargs = lua_gettop(L) - 1; //minus self
	switch(nargs)
	{
	case 1:
		{
			int percentage = lua_tointeger(L, 2);
			int height = pFractal->getHeightFromPercent(percentage);
			lua_pushinteger(L, height);
		}
		break;
	case 2:
		{
			int x = lua_tointeger(L, 2);
			int y = lua_tointeger(L, 3);
			int height = pFractal->getHeight(x, y);
			lua_pushinteger(L, height);
		}
		break;
	default:
		luaL_error(L, "Invalid number of arguments.");
	}

	return 1;
}
