#pragma once
#ifndef CVLUAINSTANCETEMPLATES
#define CVLUAINSTANCETEMPLATES

#include "!CvLuaApiExec.h"
#include "!CvScriptingUID.h"

struct LuaTaggedInstanceUserdata {
	
	static const size_t TAG = sizeof(size_t) == 4 ? 0x4C544955 : (size_t)0x64644C5449550001 ;
	size_t global_tag;
	ScriptingUID uid;
	static void constructPush(lua_State* L, const ScriptingUID& uid, uid_ttag_t type_tag) {
		LuaTaggedInstanceUserdata* self = (LuaTaggedInstanceUserdata*)luaVp_pushCreateUIDObject(L,sizeof(LuaTaggedInstanceUserdata),uid.index(),type_tag);
		self->global_tag = TAG;
		self->uid = uid;
	}
	static bool isValidUD(void* ud, uid_ttag_t type_tag) {
		if( LuaTaggedInstanceUserdata* self = (LuaTaggedInstanceUserdata*)ud )
			return self->global_tag == TAG && self->uid.isValid() && self->uid.isType(type_tag);
		return false;
	}
	static void* fromStack(lua_State* L, uid_ttag_t type_tag, int idx, const char** errmsg) {
		const char* ret = NULL; if(!errmsg) errmsg = &ret;
		
		if(lua_type(L,idx) != LUA_TUSERDATA ) { *errmsg = "Invalid type!(lua-userdata expected)"; return NULL; } 
		
		LuaTaggedInstanceUserdata* self = (LuaTaggedInstanceUserdata*)lua_touserdata(L,idx);
		
		if( self->global_tag != TAG ) { *errmsg = "Invalid userdata!"; return NULL; }
		
		if( !self->uid.isValid() ) { *errmsg = "Object deleted!"; return NULL; }
		
		if( !self->uid.isType(type_tag) ) { *errmsg = "Invalid type!(dll)"; return NULL; }
		
		return self->uid.retrieve() ;
	}
};

template<class Impl, class T>
class CvLuaDynamicInstance {//: public CvLuaObjectWrapper<T,Impl,2> {
public:
	typedef CvLuaGamecoreTypes::TypeGlobals<T> TG ;
	
	static int CreateMetatable(lua_State* L)
	{
		Impl::CreateFunctable(L);
		
		lua_createtable(L,0,4);
		lua_pushstring(L,"__index");
		lua_pushvalue(L,1);
		lua_rawset(L,2);
		
		lua_replace(L,1);
		
		Impl::CustomizeMTT(L);
		
		lua_pushstring(L,"__metatable");
		lua_pushboolean(L,false);
		lua_rawset(L,1);
		
		return 1;
	}
	
	static void RegisterGlobals(lua_State* L) {
		luaVp_registerMetatable(L,TG::TypeTag(),&CreateMetatable,TG::TypeName());
		Impl::CustomRegister(L);
	}
	
	static void Push(lua_State* L, T* p) {
		const GlobalUIDSystem::UID uid = GlobalUIDSystem::UID::from(p);
		const uid_index_t index = uid.index();
		if( !index ) { lua_pushnil(L); return; }
		if( void* ud = luaVp_tryPushUIDObject(L,index) )
		{
			if( LuaTaggedInstanceUserdata::isValidUD(ud,TG::TypeTag()) ) { return; }
			lua_pop(L,1);
		}
		LuaTaggedInstanceUserdata::constructPush(L,uid,TG::TypeTag());
	}
	
	static T* TryGetInstance(lua_State* L, int idx, const char** errmsg=NULL) {
		return (T*) LuaTaggedInstanceUserdata::fromStack(L,TG::TypeTag(),idx,errmsg) ;
	}
	
	static T* GetInstance(lua_State* L, int idx=1,bool raise_error=true) {
		const char* errmsg = "Something went wrong in a completely unimaginable way. Sorry.";
		T* ptr = Impl::TryGetInstance(L,idx,&errmsg);
		if(!ptr) {
			ptr = Impl::HandleMissingInstance(L,idx);
			if( raise_error && !ptr ) { luaL_error(L,errmsg); }
		}
		return ptr;
	}
	
	static T* HandleMissingInstance(lua_State* L,int idx) { return NULL; }
	static void CustomizeMTT(lua_State* L) {} ;
	static void CustomRegister(lua_State* L) {} ;
};

template<class Impl,class T>
class CvLuaStaticInstance {//: public CvLuaObjectWrapper<T,Impl,1> {
public:
	typedef CvLuaGamecoreTypes::TypeGlobals<T> TG ;
	static void RegisterGlobals(lua_State* L) {
		luaVp_registerMetatable(L,TG::TypeTag(),&CreateMetatable,TG::TypeName());
		lua_newuserdata(L,0);
		luaVp_applyMetatable(L,1,TG::TypeTag());
		lua_setfield(L,LUA_GLOBALSINDEX,TG::TypeName());
		Impl::CustomRegister(L);
	}
	
	static int CreateMetatable(lua_State* L)
	{
		Impl::CreateFunctable(L);
		
		lua_createtable(L,0,4);
		lua_pushstring(L,"__index");
		lua_pushvalue(L,1);
		lua_rawset(L,2);
		
		lua_replace(L,1);
		
		Impl::CustomizeMTT(L);
		
		lua_pushstring(L,"__metatable");
		lua_pushboolean(L,false);
		lua_rawset(L,1);
		
		return 1;
	}
	
	static T* GetInstance(lua_State* L=NULL,int=0,bool=true) { return Impl::GetStaticInstance(); }
	static void CustomizeMTT(lua_State* L) {} ;
	static void CustomRegister(lua_State* L) {} ;
};

#endif