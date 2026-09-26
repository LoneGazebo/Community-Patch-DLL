// Add every function registered here to LuaCATS/Exe.d.lua as well.

#pragma once

#ifndef CV_LUA_EXE_H
#define CV_LUA_EXE_H

class CvLuaExe
{
public:
	static void Register(lua_State* L);

protected:
	static int pRegister(lua_State* L);

	static int lGetBuildName(lua_State* L);
};

#endif // CV_LUA_EXE_H
