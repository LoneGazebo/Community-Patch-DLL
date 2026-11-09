#pragma once
#ifndef CVLUAINSTANCETEMPLATES
#define CVLUAINSTANCETEMPLATES

#include "!CvLuaApiExec.h"
#include "CvStructs.h"

struct LuaTaggedAutoPointer{
	DWORD tag;
	AutoInvalidatedPointerBase ptr;
	
	//Factory
	template<class T>
	static LuaTaggedAutoPointer* ConstructAndPush( lua_State* L, T* ptr, DWORD tag ) {
		LuaTaggedAutoPointer* ud = (LuaTaggedAutoPointer*) luaVp_pushCreateObject(L,tag,ptr,sizeof(LuaTaggedAutoPointer));
		ud->tag = tag;
		((AutoInvalidatedPointer<T>*)(&ud->ptr))->construct(ptr);
		return ud;
	}
	
	static void* ValidPtr( void* inst, DWORD tag_ ) {
		if( LuaTaggedAutoPointer* tmp = (LuaTaggedAutoPointer*)inst )
			return tmp->tag == tag_ ? tmp->ptr.get() : NULL ;
		return NULL;
	}
	static void Destroy( void* inst ) { if( LuaTaggedAutoPointer* tmp = (LuaTaggedAutoPointer*)inst ) { tmp->ptr.destroy(); } }
	
	static void* CheckFromStack(lua_State* L, DWORD tag, int idx ) {
		return (lua_type(L,idx) == LUA_TUSERDATA) ? ValidPtr(lua_touserdata(L,idx),tag) : NULL ;
	}
	
	static int finalizer(lua_State* L) { LuaTaggedAutoPointer::Destroy(lua_touserdata(L,1)); return 0; }
private:
	~LuaTaggedAutoPointer();
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
		
		lua_pushstring(L,"__gc");
		lua_pushcclosure_unsafe(L,&LuaTaggedAutoPointer::finalizer);
		lua_rawset(L,1);
		
		return 1;
	}
	
	static void RegisterGlobals(lua_State* L) {
		luaVp_registerMetatable(L,TG::TypeTag(),&CreateMetatable,TG::TypeName());
		Impl::CustomRegister(L);
	}
	
	static void Push(lua_State* L, T* p) {
		if(!p) { lua_pushnil(L); return; }
		if( !( LuaTaggedAutoPointer::ValidPtr( luaVp_tryPushObject(L,TG::TypeTag(),p), TG::TypeTag() ) ) )
			LuaTaggedAutoPointer::ConstructAndPush<T>(L,p,TG::TypeTag());
	}
	
	static T* TryGetInstance(lua_State* L, int idx) {
		return (T*) LuaTaggedAutoPointer::CheckFromStack(L,TG::TypeTag(),idx) ;
	}
	
	static T* GetInstance(lua_State* L, int idx=1,bool raise_error=true) {
		T* ptr = TryGetInstance(L,idx);
		if(!ptr) { 
			ptr = Impl::HandleMissingInstance(L,idx);
			if( raise_error && !ptr ) { luaL_error(L,"Invalid instance!"); }
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