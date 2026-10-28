#pragma once
#ifndef CVLUAOBJECTWRAPPER
#define CVLUAOBJECTWRAPPER

#include "!CvLuaApi.h"

// TEMPORARY COMPAT - REMOVE SOON! //
template<class FUNCTYPE> 
int BasicLuaMethod(lua_State* L,FUNCTYPE func) { return luaEx_exec(L,func); }

// HELPERS FOR BULK INLINE DEFINING AND PUSHING OF LUA API FUNCTIONS //

struct lua_StateEx {
	operator lua_State*() { return L; }
	lua_StateEx(lua_State* in) : L(in) {}
private:
	lua_State* L;
};

template<class T>
void operator<<(lua_StateEx L,const T& val) { luaEx_push(L,val); }

// THE MSVC GODS DEMAND ((VOID)0) SACRIFICES! //

#define luafsq_start_impl(luastate,target,unsafe) {\
lua_State* mcL = luastate; int mcI = target; bool mcUnsafe = unsafe; { const char* mcN = NULL; struct mcS{\
static int mcF(lua_State*){return 0;} static int nullfunc(lua_State* L, int top){((void)0);{((void)0)

#define luafsq_func_impl(name,...) } return lua_gettop(L)-top; }}; luaEx_putfunc(mcL,mcI,mcN,&mcS::mcF); }{\
	const char* mcN = #name;\
	struct mcS {\
		static int mcF(lua_State* L) { return luaEx_macroexec(L,f_##name); }\
		static int f_##name(lua_State* mcL_##__VA_ARGS__) { lua_StateEx L(mcL_##name); int top = lua_gettop(L); if(true){((void)0);

#define luafsq_wrap(name,func,...) } return lua_gettop(L)-top; }}; luaEx_putfunc(mcL,mcI,mcN,&mcS::mcF); }{ const char* mcN = #name;\
	struct mcS { static int mcF(lua_State* L) { return luaEx_exec<__VA_ARGS__>(L,&func); } static int nullfunc(lua_State* L, int top){((void)0);{((void)0)

#define luafsq_end() } return lua_gettop(L)-top; }}; luaEx_putfunc(mcL,mcI,mcN,&mcS::mcF,mcUnsafe); } }

#define luafsq_start(N,T) luafsq_start_impl(N,T,false)
#define luafsq_func(name,...) luafsq_func_impl(name,name,__VA_ARGS__)

inline int luaEx_macroexec(lua_State* L,int(*func)(lua_State*)) { return func(L); }

template<class A>
inline int luaEx_macroexec(lua_State* L,int(*func)(lua_State*,A)) { return func(L,luaEx_to<A>(L,1)); }

template<class A,class B>
inline int luaEx_macroexec(lua_State* L,int(*func)(lua_State*,A,B)) { return func(L,luaEx_to<A>(L,1),luaEx_to<B>(L,2)); }

template<class A,class B,class C>
inline int luaEx_macroexec(lua_State* L,int(*func)(lua_State*,A,B,C)) { return func(L,luaEx_to<A>(L,1),luaEx_to<B>(L,2),luaEx_to<C>(L,3)); }

template<class A,class B,class C,class D>
inline int luaEx_macroexec(lua_State* L,int(*func)(lua_State*,A,B,C,D)) { return func(L,luaEx_to<A>(L,1),luaEx_to<B>(L,2),luaEx_to<C>(L,3),luaEx_to<D>(L,4)); }


// SCRIPT GENERATED EXECUTION TEMPLATES //

template<class R>
int luaEx_exec(lua_State* L,R(*func)()) {
    luaEx_push<R>( L, func() );
    return 1;
}

template<>
inline int luaEx_exec(lua_State* L,void(*func)()) {
    func();
    return 0;
}

template<class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)() ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)() );
    return 1;
}

template<class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)() const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)() );
    return 1;
}

template<class T>
int luaEx_exec(lua_State* L, void (T::*func)() ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)();
    return 0;
}

template<class T>
int luaEx_exec(lua_State* L, void (T::*func)() const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)();
    return 0;
}

template<class A, class R>
int luaEx_exec(lua_State* L,R(*func)(A)) {
    luaEx_push<R>( L, func(luaEx_to<A>(L,1)) );
    return 1;
}

template<class A>
int luaEx_exec(lua_State* L,void(*func)(A)) {
    func(luaEx_to<A>(L,1));
    return 0;
}

template<class A, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1)) );
    return 1;
}

template<class A, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1)) );
    return 1;
}

template<class A, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1));
    return 0;
}

template<class A, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1));
    return 0;
}

template<class A, class B, class R>
int luaEx_exec(lua_State* L,R(*func)(A, B)) {
    luaEx_push<R>( L, func(luaEx_to<A>(L,1), luaEx_to<B>(L,2)) );
    return 1;
}

template<class A, class B>
int luaEx_exec(lua_State* L,void(*func)(A, B)) {
    func(luaEx_to<A>(L,1), luaEx_to<B>(L,2));
    return 0;
}

template<class A, class B, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A, B) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2)) );
    return 1;
}

template<class A, class B, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A, B) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2)) );
    return 1;
}

template<class A, class B, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A, B) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2));
    return 0;
}

template<class A, class B, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A, B) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2));
    return 0;
}

template<class A, class B, class C, class R>
int luaEx_exec(lua_State* L,R(*func)(A, B, C)) {
    luaEx_push<R>( L, func(luaEx_to<A>(L,1), luaEx_to<B>(L,2), luaEx_to<C>(L,3)) );
    return 1;
}

template<class A, class B, class C>
int luaEx_exec(lua_State* L,void(*func)(A, B, C)) {
    func(luaEx_to<A>(L,1), luaEx_to<B>(L,2), luaEx_to<C>(L,3));
    return 0;
}

template<class A, class B, class C, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A, B, C) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3)) );
    return 1;
}

template<class A, class B, class C, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A, B, C) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3)) );
    return 1;
}

template<class A, class B, class C, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A, B, C) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3));
    return 0;
}

template<class A, class B, class C, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A, B, C) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3));
    return 0;
}

template<class A, class B, class C, class D, class R>
int luaEx_exec(lua_State* L,R(*func)(A, B, C, D)) {
    luaEx_push<R>( L, func(luaEx_to<A>(L,1), luaEx_to<B>(L,2), luaEx_to<C>(L,3), luaEx_to<D>(L,4)) );
    return 1;
}

template<class A, class B, class C, class D>
int luaEx_exec(lua_State* L,void(*func)(A, B, C, D)) {
    func(luaEx_to<A>(L,1), luaEx_to<B>(L,2), luaEx_to<C>(L,3), luaEx_to<D>(L,4));
    return 0;
}

template<class A, class B, class C, class D, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A, B, C, D) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3), luaEx_to<D>(L,i0+4)) );
    return 1;
}

template<class A, class B, class C, class D, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A, B, C, D) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3), luaEx_to<D>(L,i0+4)) );
    return 1;
}

template<class A, class B, class C, class D, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A, B, C, D) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3), luaEx_to<D>(L,i0+4));
    return 0;
}

template<class A, class B, class C, class D, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A, B, C, D) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3), luaEx_to<D>(L,i0+4));
    return 0;
}

template<class A, class B, class C, class D, class E, class R>
int luaEx_exec(lua_State* L,R(*func)(A, B, C, D, E)) {
    luaEx_push<R>( L, func(luaEx_to<A>(L,1), luaEx_to<B>(L,2), luaEx_to<C>(L,3), luaEx_to<D>(L,4), luaEx_to<E>(L,5)) );
    return 1;
}

template<class A, class B, class C, class D, class E>
int luaEx_exec(lua_State* L,void(*func)(A, B, C, D, E)) {
    func(luaEx_to<A>(L,1), luaEx_to<B>(L,2), luaEx_to<C>(L,3), luaEx_to<D>(L,4), luaEx_to<E>(L,5));
    return 0;
}

template<class A, class B, class C, class D, class E, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A, B, C, D, E) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3), luaEx_to<D>(L,i0+4), luaEx_to<E>(L,i0+5)) );
    return 1;
}

template<class A, class B, class C, class D, class E, class R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(A, B, C, D, E) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3), luaEx_to<D>(L,i0+4), luaEx_to<E>(L,i0+5)) );
    return 1;
}

template<class A, class B, class C, class D, class E, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A, B, C, D, E) ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3), luaEx_to<D>(L,i0+4), luaEx_to<E>(L,i0+5));
    return 0;
}

template<class A, class B, class C, class D, class E, class T>
int luaEx_exec(lua_State* L, void (T::*func)(A, B, C, D, E) const ) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    const T* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(luaEx_to<A>(L,i0+1), luaEx_to<B>(L,i0+2), luaEx_to<C>(L,i0+3), luaEx_to<D>(L,i0+4), luaEx_to<E>(L,i0+5));
    return 0;
}
#endif

// GENERATING SCRIPT - LUA //

/*

local abc = { "A","B","C","D","E" }

function opt1(st) return st == "" and "" or st..", " end 
function opt2(st) return st == "" and "" or ", "..st end

function gen_free_ret(tmpl,list,args)
    out = string.format([[
template<%sclass R>
int luaEx_exec(lua_State* L,R(*func)(%s)) {
    luaEx_push<R>( L, func(%s) );
    return 1;
}]],opt1(tmpl),list,args);
    print(out.."\n");
end

function gen_free_void(tmpl,list,args)
    out = string.format([[
template<%s>
%sint luaEx_exec(lua_State* L,void(*func)(%s)) {
    func(%s);
    return 0;
}]],tmpl,(tmpl=="" and "inline " or ""),list,args);
    print(out.."\n");
end

function gen_bound_ret(const,tmpl,list,args,argsX)
    out = string.format([[
template<%sclass R, class T>
int luaEx_exec(lua_State* L, R (T::*func)(%s) %s) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    %sT* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    luaEx_push<R>( L, (inst->*func)(%s) );
    return 1;
}]],opt1(tmpl),list,const,const,argsX);
    print(out.."\n");
end

function gen_bound_void(const,tmpl,list,args,argsX)
    out = string.format([[
template<%sclass T>
int luaEx_exec(lua_State* L, void (T::*func)(%s) %s) {
    const int i0 = CvLuaGamecoreTypes::TypeGlobals<T>::IsStatic ? 0 : 1 ;
    %sT* inst = CvLuaGamecoreTypes::TypeGlobals<T>::GetInstance(L,1);
    (inst->*func)(%s);
    return 0;
}]],opt1(tmpl),list,const,const,argsX);
    print(out.."\n");
end

function gen_all(...)
    gen_free_ret(...);
    gen_free_void(...);
    gen_bound_ret("",...);
    gen_bound_ret("const ",...)
    gen_bound_void("",...);
    gen_bound_void("const ",...)
end

local t = ""
local l = ""
local s = ""
local x = ""
function adv(i)
    t = opt1(t).."class "..abc[i];
    l = opt1(l)..abc[i];
    s = opt1(s).."luaEx_to<"..abc[i]..">(L,"..tostring(i)..")";
    x = opt1(x).."luaEx_to<"..abc[i]..">(L,i0+"..tostring(i)..")";
end

for i = 1,#abc do
    gen_all(t,l,s,x);
    adv(i)
end
gen_all(t,l,s,x);

*/

