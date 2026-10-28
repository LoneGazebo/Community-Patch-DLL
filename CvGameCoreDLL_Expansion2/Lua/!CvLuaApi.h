#pragma once
#ifndef INCLUDED_CVLUAAPI
#define INCLUDED_CVLUAAPI

#include "lua.hpp"

typedef void(*lua_CVoidFunc)(lua_State*);
typedef unsigned long DWORD;

inline bool luaL_optbool(lua_State* L, int idx, bool bdefault) { return lua_isnone(L,idx) ? bdefault : (lua_toboolean(L,idx)==1) ; }
inline void lua_pushcclosure_unsafe( lua_State* L, lua_CFunction f, int n = 0 ) { lua_pushcclosure(L,f,n); }

//----------------------------------------------------------------------------//
// The following luaVp prefixed functions are implemented in CvLuaSupport.cpp //
//----------------------------------------------------------------------------//

int luaVp_pcall(lua_State* L,lua_CFunction f);
int luaVp_pcall(lua_State* L,lua_CVoidFunc f);
void luaVp_pushcclosure(lua_State* L, lua_CFunction f, int orig_upvals = 0, lua_CVoidFunc lPushDbgInfo = NULL , const char* hint = NULL);
#define lua_pushcclosure luaVp_pushcclosure //Redirect everything to use our version (protected wrapper)

void* luaVp_pushCreateObject(lua_State* L, DWORD typeTag, void* instptr, size_t size );
void* luaVp_tryPushObject(lua_State* L, DWORD typeTag, void* instptr);

void luaVp_registerMetatable(lua_State* L, DWORD typeTag, lua_CFunction pushMtt, const char* hint = NULL);
void luaVp_applyMetatable(lua_State* L, int idx, DWORD typeTag);

//------------------------------//
// A few small helper functions //
//------------------------------//

inline void luaEx_quickset(lua_State* L,int table,const char* key,int v) { lua_pushvalue(L,table); lua_pushinteger(L,v); lua_setfield(L,-2,key); lua_pop(L,1); }
inline void luaEx_quickset(lua_State* L,int table,const char* key,bool v) { lua_pushvalue(L,table); lua_pushboolean(L,v); lua_setfield(L,-2,key); lua_pop(L,1); }
inline void luaEx_quickset(lua_State* L,int table,const char* key,const char* v) { lua_pushvalue(L,table); lua_pushstring(L,v); lua_setfield(L,-2,key); lua_pop(L,1); }
inline void luaEx_pushtag(lua_State* L,DWORD tag) { lua_pushlightuserdata(L,(void*)tag); }

inline void luaEx_putfunc(lua_State* L,int table,const char* name,lua_CFunction func,bool unsafe=false) {
	if(name==NULL||func==NULL) return;
	lua_pushvalue(L,table); 
	if(unsafe) { lua_pushcclosure_unsafe(L,func); } else { lua_pushcclosure(L,func); } 
	lua_setfield(L,-2,name); 
	lua_pop(L,1); 
}

//-------------------------------------------------------------------//
// luaEx_to and luaEx_push - overloaded templates for typed push/get //
//-------------------------------------------------------------------//

template<class T> T luaEx_to(lua_State* L, int idx) { return static_cast<T>(lua_tointeger(L,idx)); }

template<> inline int luaEx_to(lua_State* L, int idx) { return lua_tointeger(L,idx); }
template<> inline bool luaEx_to(lua_State* L, int idx) { return lua_toboolean(L,idx)!=0; }
template<> inline const char* luaEx_to(lua_State* L, int idx) { return lua_tostring(L,idx); }

template<class T> void luaEx_push(lua_State* L, T in) { lua_pushinteger(L,in); }

template<> inline void luaEx_push(lua_State* L,const int in) { lua_pushinteger(L,in); }
template<> inline void luaEx_push(lua_State* L,const bool in) { lua_pushboolean(L,in); }
template<> inline void luaEx_push(lua_State* L,const char* in) { lua_pushstring(L,in); }

//----------------------------//
// Type & Traits declarations //
//----------------------------//

namespace CvLuaGamecoreTypes { template<class T> struct TypeGlobals; }

#define LuaTypes_DeclareStatic( Tag, Name, CLASS ) class CLASS;\
namespace CvLuaGamecoreTypes { template<> struct TypeGlobals<CLASS> {\
	static const char* TypeName() { return #Name; }\
	static DWORD TypeTag() { return Tag; }\
	static void RegisterGlobals(lua_State*);\
	static const bool IsStatic = true;\
	static CLASS* GetInstance(lua_State* L,int idx,bool err=true);\
};}

#define LuaTypes_DefineStatic( CLASS, Implementer ) \
void CvLuaGamecoreTypes::TypeGlobals<CLASS>::RegisterGlobals(lua_State* L) { Implementer::RegisterGlobals(L); }\
CLASS* CvLuaGamecoreTypes::TypeGlobals<CLASS>::GetInstance(lua_State*,int,bool) { return Implementer::GetInstance(); }

#define LuaTypes_DeclareInstanced( Tag, Name, CLASS ) class CLASS ;\
namespace CvLuaGamecoreTypes { template<> struct TypeGlobals<CLASS> {\
	static const char* TypeName() { return #Name; }\
	static DWORD TypeTag() { return Tag; }\
	static void RegisterGlobals(lua_State* L);\
	static void PushInstance(lua_State* L,CLASS* inst);\
	static CLASS* GetInstance(lua_State* L,int idx,bool err=true);\
	static CLASS* TryGetInstance(lua_State* L,int idx);\
	static const bool IsStatic = false;\
};}\
\
template<> inline CLASS* luaEx_to(lua_State* L,int idx) { return CvLuaGamecoreTypes::TypeGlobals<CLASS>::GetInstance(L,idx); }\
template<> inline const CLASS* luaEx_to(lua_State* L,int idx) { return CvLuaGamecoreTypes::TypeGlobals<CLASS>::GetInstance(L,idx); }\
template<> inline CLASS& luaEx_to(lua_State* L,int idx) { return *CvLuaGamecoreTypes::TypeGlobals<CLASS>::GetInstance(L,idx); }\
template<> inline const CLASS& luaEx_to(lua_State* L,int idx) { return *CvLuaGamecoreTypes::TypeGlobals<CLASS>::GetInstance(L,idx); }\
template<> inline void luaEx_push(lua_State* L,CLASS* inst) { CvLuaGamecoreTypes::TypeGlobals<CLASS>::PushInstance(L,inst); }\
template<> inline void luaEx_push(lua_State* L,CLASS& inst) { CvLuaGamecoreTypes::TypeGlobals<CLASS>::PushInstance(L,&inst); }\
template<> inline void luaEx_push(lua_State* L,const CLASS& inst) { CvLuaGamecoreTypes::TypeGlobals<CLASS>::PushInstance(L,const_cast<CLASS*>(&inst)); }\
inline CLASS* luaEx_to##Name(lua_State* L,int idx,bool err=true) { return CvLuaGamecoreTypes::TypeGlobals<CLASS>::GetInstance(L,idx,err); }\
inline void luaEx_push##Name(lua_State* L,CLASS* inst) { CvLuaGamecoreTypes::TypeGlobals<CLASS>::PushInstance(L,inst); }

#define LuaTypes_DefineInstanced( CLASS, Implementer ) \
void CvLuaGamecoreTypes::TypeGlobals<CLASS>::RegisterGlobals(lua_State* L) { Implementer::RegisterGlobals(L); }\
void CvLuaGamecoreTypes::TypeGlobals<CLASS>::PushInstance(lua_State* L,CLASS* inst) { return Implementer::Push(L,inst); }\
CLASS* CvLuaGamecoreTypes::TypeGlobals<CLASS>::GetInstance(lua_State* L,int idx,bool err) { return Implementer::GetInstance(L,idx,err); }\
CLASS* CvLuaGamecoreTypes::TypeGlobals<CLASS>::TryGetInstance(lua_State* L,int idx) { return Implementer::TryGetInstance(L,idx); }

LuaTypes_DeclareStatic( 0x4D41505F /*MAP_*/, Map, CvMap );
LuaTypes_DeclareStatic( 0x47414D45 /*GAME*/, Game, CvGame );

LuaTypes_DeclareInstanced( 0x41524541 /*AREA*/, Area, CvArea );
LuaTypes_DeclareInstanced( 0x43495459 /*CITY*/, City, CvCity );
LuaTypes_DeclareInstanced( 0x4445414C /*DEAL*/, Deal, CvDeal );
LuaTypes_DeclareInstanced( 0x4C454147 /*LEAG*/, League, CvLeague );

LuaTypes_DeclareInstanced( 0x504C5952 /*PLYR*/, PlayerBase, CvPlayer );
LuaTypes_DeclareInstanced( 0x504C5952 /*PLYR*/, Player, CvPlayerAI );

LuaTypes_DeclareInstanced( 0x504C4F54 /*PLOT*/, Plot, CvPlot );
LuaTypes_DeclareInstanced( 0x5445414D /*TEAM*/, Team, CvTeam );
LuaTypes_DeclareInstanced( 0x54455445 /*TETE*/, TeamTechs, CvTeamTechs );
LuaTypes_DeclareInstanced( 0x554E4954 /*UNIT*/, Unit, CvUnit );

#endif