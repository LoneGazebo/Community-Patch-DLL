/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaSupport.cpp
//!  \brief     Private implementation of the Gamecore Lua framework.
//!
//!		This file includes methods for registering game data w/ Lua.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "CvGameCoreDLLPCH.h"
#include "../CvGameCoreDLLPCH.h"
#include "../CustomMods.h"
#include "ICvDLLScriptSystem.h"
#include "CvLuaSupport.h"
#include "CvLuaEnums.h"
#include "CvLuaFractal.h"
#include "CvLuaGameInfo.h"
#include "!CvLuaApiExec.h"

#pragma warning(disable:4800 ) //forcing value to bool 'true' or 'false'

//---------------------------//
// Implement luaVp functions //
//---------------------------//

static int protectedWrapperAux( lua_State* L ) {
	try {
		const int args = lua_gettop(L);
		
		lua_pushvalue(L, lua_upvalueindex(1));
		for( int i = 1 ; i <= args ; ++i ) lua_pushvalue(L,i);
		const int errcode = lua_pcall(L,args,LUA_MULTRET,0);
		if( errcode == 0 ) return lua_gettop(L)-args;
		//TODO: Generate helpfull info on what went wrong using saved args and dbg_help/hint upvals
		lua_error(L); // Re-Throw 
	} catch (...) {}
	luaL_error(L,"A c++ exception was raised while executing lua!"); //This really shouldn't happen!
}

void luaVp_pushcclosure(lua_State* L, lua_CFunction func, int orig_upvals, lua_CVoidFunc dbg_help, const char* hint) {
	lua_pushcclosure_unsafe(L,func,orig_upvals);
	int upvals = 1 ;
	if( dbg_help ) { 
		lua_pushlightuserdata(L,(void*)dbg_help); ++upvals;
		if( hint ) { lua_pushstring(L,hint); ++upvals; }
	}
	lua_pushcclosure_unsafe(L,&protectedWrapperAux,upvals);
}



namespace {
	
	void pushInternalsTable(lua_State* L) { 
		lua_pushvalue(L, LUA_REGISTRYINDEX);
		lua_getfield(L,-1,"vpdllinternals");
		if( !lua_istable(L,-1) ) {
			lua_pop(L,1);
			lua_createtable(L,0,0);
			lua_pushvalue(L,-1);
			lua_setfield(L,-3,"vpdllinternals");
		}
		lua_replace(L,-2);
		return;
	}
	
	void pushCreateInternalTypeTable(lua_State* L,DWORD typeTag,const char* hint) {
		pushInternalsTable(L);
		lua_newtable(L);
		luaEx_pushtag(L,typeTag);
		lua_pushvalue(L,-2);
		lua_settable(L,-4);
		lua_replace(L,-2);
		if(hint) { luaEx_quickset(L,-1,"__hint",hint); }
	}
	
	void pushInternalTypeTable(lua_State* L,DWORD typeTag) {
		pushInternalsTable(L);
		luaEx_pushtag(L,typeTag);
		lua_rawget(L,-2);
		lua_replace(L,-2);
	}
	
	int fCFunctionExec(lua_State* L) {
		lua_CFunction f = (lua_CFunction)lua_touserdata(L,1);
		if(f) { lua_pop(L,1); f(L); return 0; }
		luaL_error(L,"??? - LuaSupport::LuaPCall");
	}
	
	int fCVoidFuncExec(lua_State* L) {
		lua_CVoidFunc f = (lua_CVoidFunc)lua_touserdata(L,1);
		if(f) { lua_pop(L,1); f(L); return 0; }
		luaL_error(L,"??? - LuaSupport::LuaPCall");
	}
	
	void handleCPCallError(lua_State*) { //TODO: ???
		ASSERT(true,"lua function failed!");
	}
}

int luaVp_pcall(lua_State* L, lua_CFunction f) {
	int code = lua_cpcall(L,fCFunctionExec,(void*)f) ;
	if( code!=0 ) handleCPCallError(L);
	return code ;
}

int luaVp_pcall(lua_State* L,lua_CVoidFunc f) {
	int code = lua_cpcall(L,fCVoidFuncExec,(void*)f) ;
	if( code!=0 ) handleCPCallError(L);
	return code ;
}

void luaVp_registerMetatable(lua_State* L, DWORD typeTag, lua_CFunction generate, const char* hint) {
	pushCreateInternalTypeTable(L,typeTag,hint);
	lua_pushcclosure_unsafe(L,generate);
	lua_pcall(L,0,1,0);
	lua_setfield(L,-2,"__meta");
	lua_pop(L,1);
}

void luaVp_applyMetatable(lua_State* L, int idx, DWORD typeTag) {
	lua_pushvalue(L,idx);
	pushInternalTypeTable(L,typeTag);
	lua_getfield(L,-1,"__meta");
	lua_setmetatable(L,-3);
	lua_pop(L,2);
}

void* luaVp_pushCreateObject(lua_State* L,DWORD typeTag,void* uid,size_t size) {
	pushInternalTypeTable(L,typeTag);
	void* ret = lua_newuserdata(L,size);
	luaVp_applyMetatable(L,-1,typeTag);
	lua_pushlightuserdata(L,uid);
	lua_pushvalue(L,-2);
	lua_settable(L,-4);
	lua_replace(L,-2);
	return ret;
}

void* luaVp_tryPushObject(lua_State* L,DWORD typeTag,void* uid) {
	pushInternalTypeTable(L,typeTag);
	lua_pushlightuserdata(L,uid);
	lua_gettable(L,-2);
	if( void* ret = lua_touserdata(L,-1) ) { lua_replace(L,-2); return ret; }
	lua_pop(L,2);
	return NULL;
}

// Anyonymous namespace for helper functions 

namespace {
	void RegisterSupportStuff(lua_State* L) {
		
		// This is currently little more than a syntax/feature test for the luafsq macros //
		lua_newtable(L);
		
		//luafsq_start(lua_State* L,int target_table) - enables use of luafsq_func and luafsq_wrap
		luafsq_start(L,1);
		
		//vputils.vpnewproxy() : Classic lua function, manipulating the lua stack
		luafsq_func(vpnewproxy) {
			if(lua_type(L,1)!=LUA_TTABLE) luaL_error(L,"Expected a table!");
			lua_newuserdata(L,0);
			lua_insert(L,1);
			lua_setmetatable(L,1);
			return 1;
		}
		
		//vputils.HelloWorld() : Return statement is optional.
		//All surplus objects on the lua stack compared to function start are returned
		luafsq_func(HelloWorld) { lua_pushstring(L,"Hello World!"); }
		
		//vputils.hasGamecoreLock() : << operator can be used to push any type supported
		luafsq_func(hasGamecoreLock) { 
			L << gDLL->HasGameCoreLock();
			L << "Turns out lua is never run with the gamecore lock?!";
		}
		
		//vputils.SetFaith(Player,number): Automatic argument parsing for supported types
		luafsq_func(SetFaith,CvPlayer* in,int faith) { in->SetFaithTimes100(faith*100); }
		
		//vputils.SetFaithT100(Player,number): Wrap functions directly(both free and member)
		//For member functions first lua argument is the object
		luafsq_wrap(SetFaithT100,CvPlayer::SetFaithTimes100);
		
		//vputils.GetProdTurnsForUnit(City,number unit_type): CvCity::getProductionTurnsLeft is overloaded...
		//... provide argument types to narrow the compiler's choices
		luafsq_wrap(GetProdTurnsForUnit,CvCity::getProductionTurnsLeft,UnitTypes);
		
		luafsq_end(); // End the function stream
		
		lua_setglobal(L,"vputils"); //Puts the table we streamed our functions to into L's global environment
		// ------------------------------------------------------------------------------ //
	}
	
	bool skipGamecoreCreation = false ;
	lua_State* supportState = NULL ;

	void executeSupportEvent(const char* event) {
		lua_getglobal(supportState,"_eventfunc");
		lua_pushstring(supportState,event);
		lua_pcall(supportState,1,0,0);
	}
	
	template<class T>
	void RegisterInterface(lua_State* L) { luaVp_pcall(L,&CvLuaGamecoreTypes::TypeGlobals<T>::RegisterGlobals); }

	void DoRegisterScriptData(lua_State* L)
	{
		//Register some plain datatables
		CvLuaEnums::RegisterGlobals(L);
		CvLuaGameInfo::RegisterGlobals(L);

		//Register CvFractals interface
		CvLuaFractal::RegisterGlobals(L);
		
		//Register statics - Game and Map
		RegisterInterface<CvGame>(L);
		RegisterInterface<CvMap>(L);
		
		//Register dynamic interface types
		RegisterInterface<CvArea>(L);
		RegisterInterface<CvCity>(L);
		RegisterInterface<CvDeal>(L);
		RegisterInterface<CvLeague>(L);
		RegisterInterface<CvPlot>(L);
		RegisterInterface<CvPlayer>(L);
		RegisterInterface<CvTeam>(L);
		RegisterInterface<CvTeamTechs>(L);
		RegisterInterface<CvUnit>(L);
		
		executeSupportEvent("gc_post");
	}
}

//----------------------------------//
// Implement Lua::Support functions //
//----------------------------------//

void LuaSupport::NotifyDatabaseCached() {
	if( supportState == NULL ) {
		ICvEngineScriptSystem1* sys = gDLL->GetScriptSystem();
		skipGamecoreCreation = true;
		supportState = sys->CreateLuaThread("!VPDLL");
		skipGamecoreCreation = false;
		RegisterSupportStuff(supportState); 
		RunScript(supportState,"GamecoreInit.lua");
	}
	executeSupportEvent("gc_reset");
}
void LuaSupport::NotifyDllUnload() { executeSupportEvent("shutdown"); } // engine's FreeLuaThread seems to not work at this point. Tell our lua context to manually clean up!

void LuaSupport::LuaPCall(lua_State* L,lua_CFunction f) { luaVp_pcall(L,f); }
void LuaSupport::LuaPCall(lua_State* L,lua_CVoidFunc f) { luaVp_pcall(L,f); }

void LuaSupport::RegisterScriptData( lua_State* L ) { if( !skipGamecoreCreation ) DoRegisterScriptData(L); }
void LuaSupport::RunScript(lua_State* L, const char* script) { gDLL->GetScriptSystem()->LoadFile(L,script); }

//------------------------------------------------------------------------------
bool LuaSupport::CallHook(ICvEngineScriptSystem1* pkScriptSystem, const char* szName, ICvEngineScriptSystemArgs1* args, bool& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallHook(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
bool LuaSupport::CallTestAll(ICvEngineScriptSystem1* pkScriptSystem, const char* szName, ICvEngineScriptSystemArgs1* args, bool& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallTestAll(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
bool LuaSupport::CallTestAny(ICvEngineScriptSystem1* pkScriptSystem, const char* szName, ICvEngineScriptSystemArgs1* args, bool& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallTestAny(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
bool LuaSupport::CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, int& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallAccumulator(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
bool LuaSupport::CallAccumulator(_In_ ICvEngineScriptSystem1* pkScriptSystem, _In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, float& value)
{
	if (MOD_API_DISABLE_LUA_HOOKS)
		return false;

	// Must release our lock so that if the main thread has the Lua lock and is waiting for the Game Core lock, we don't freeze
	bool bHadLock = gDLL->HasGameCoreLock();
	if(bHadLock)
		gDLL->ReleaseGameCoreLock();
	bool bResult = pkScriptSystem->CallAccumulator(szName, args, value);
	if(bHadLock)
		gDLL->GetGameCoreLock();
	return bResult;
}

//------------------------------------------------------------------------------
