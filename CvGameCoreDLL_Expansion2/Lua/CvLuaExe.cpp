#include "CvGameCoreDLLPCH.h"
#include "CvLuaExe.h"
#include "../Exe/CvExe.h"

namespace
{
//! Pushes `ok` and, when refused, the reason name. Returns the number pushed.
int PushResult(lua_State* L, Exe::Reason eReason)
{
	lua_pushboolean(L, eReason == Exe::REASON_OK);

	if (eReason == Exe::REASON_OK)
	{
		return 1;
	}

	lua_pushstring(L, Exe::GetReasonName(eReason));

	return 2;
}
} // namespace

//------------------------------------------------------------------------------
void CvLuaExe::Register(lua_State* L)
{
	FLua::Details::CCallWithErrorHandling(L, pRegister);
}

int CvLuaExe::pRegister(lua_State* L)
{
	lua_getglobal(L, "Exe");

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_createtable(L, 0, 5);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "Exe");
	}

	lua_pushcclosure(L, lGetBuildName, 0);
	lua_setfield(L, -2, "GetBuildName");

	lua_pushcclosure(L, lCanScheduleResync, 0);
	lua_setfield(L, -2, "CanScheduleResync");

	lua_pushcclosure(L, lTryScheduleResync, 0);
	lua_setfield(L, -2, "TryScheduleResync");

	lua_pushcclosure(L, lCanDisableEngineYieldIconManager, 0);
	lua_setfield(L, -2, "CanDisableEngineYieldIconManager");

	lua_pushcclosure(L, lTryDisableEngineYieldIconManager, 0);
	lua_setfield(L, -2, "TryDisableEngineYieldIconManager");

	lua_pop(L, 1);

	return 0;
}

//------------------------------------------------------------------------------
int CvLuaExe::lGetBuildName(lua_State* L)
{
	lua_pushstring(L, Exe::GetBuildName());
	return 1;
}

int CvLuaExe::lCanScheduleResync(lua_State* L)
{
	return PushResult(L, Exe::CanScheduleResync());
}

int CvLuaExe::lTryScheduleResync(lua_State* L)
{
	return PushResult(L, Exe::TryScheduleResync());
}

int CvLuaExe::lCanDisableEngineYieldIconManager(lua_State* L)
{
	return PushResult(L, Exe::CanDisableEngineYieldIconManager());
}

int CvLuaExe::lTryDisableEngineYieldIconManager(lua_State* L)
{
	return PushResult(L, Exe::TryDisableEngineYieldIconManager());
}
