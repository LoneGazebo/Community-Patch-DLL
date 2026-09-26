// Add every function registered here to LuaCATS/Exe.d.lua as well.

#pragma once

#ifndef CV_LUA_EXE_H
#define CV_LUA_EXE_H

#include "!CvLuaApi.h"

class CvLuaExe
{
public:
	static void RegisterGlobals(lua_State* L);

protected:
	static int lGetBuildName(lua_State* L);
};

#endif // CV_LUA_EXE_H
