#include "CvGameCoreDLLPCH.h"
#include "CvLuaExe.h"
#include "../Exe/CvExe.h"

//------------------------------------------------------------------------------
void CvLuaExe::RegisterGlobals(lua_State* L)
{
	lua_getglobal(L, "Exe");

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_createtable(L, 0, 1);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "Exe");
	}

	lua_pushcclosure(L, lGetBuildName, 0);
	lua_setfield(L, -2, "GetBuildName");

	lua_pop(L, 1);
}

//------------------------------------------------------------------------------
int CvLuaExe::lGetBuildName(lua_State* L)
{
	lua_pushstring(L, Exe::GetBuildName());
	return 1;
}
