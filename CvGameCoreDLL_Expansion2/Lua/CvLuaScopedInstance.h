/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#pragma once
#ifndef CVLUASCOPEDNSTANCE_H

#include "CvLuaMethodWrapper.h"
#include "FObjectHandle.h"

template<class Derived, class InstanceType>
class CvLuaScopedInstance : public CvLuaMethodWrapper<Derived, InstanceType>
{
public:
	static void Push(lua_State* L, InstanceType* pkType);
	static void Push(lua_State* L, FObjectHandle<InstanceType> handle)
	{
		Push(L, handle.pointer());
	}
	static InstanceType* GetInstance(lua_State* L, int idx = 1, bool bErrorOnFail = true);

	static void PushTypeTable(lua_State* L);

	//! Used by CvLuaMethodWrapper to know where first argument is.
	static const int GetStartingArgIndex();

protected:
	static void DefaultHandleMissingInstance(lua_State* L);
};



//------------------------------------------------------------------------------
// template members
//------------------------------------------------------------------------------
template<class Derived, class InstanceType>
void CvLuaScopedInstance<Derived, InstanceType>::PushTypeTable(lua_State* L)
{
		lua_getglobal(L, "LuaTypes");
		if(lua_isnil(L,-1))
		{
			lua_pop(L,1);
			lua_newtable(L);

			lua_pushvalue(L,-1);
			lua_setglobal(L,"LuaTypes");
		}

		// -1 : Types{} = _ENV["LuaTypes"] 

		lua_getfield(L, -1, Derived::GetTypeName());

		// -1 : Type{}/nil = Types["TypeName"]
		// -2 : Types{}

		if(lua_isnil(L, -1))
		{
			//Typename wasn't found, time to build it.
			lua_pop(L, 1);
			lua_newtable(L);

			//Create weak __instances table.
			lua_pushstring(L, "__instances");
			lua_newtable(L);

			// -1 : {}
			// -2 : "__instances"
			// -3 : Type{}
			// -4 : Types{}

			//Create __instances.mt
			lua_createtable(L, 0, 1);
			lua_pushstring(L, "__mode");
			lua_pushstring(L, "v");
			lua_rawset(L, -3);				// mt.__mode = "v";	

			// -1 : { __mode = "v" }
			// -2 : {}
			// -3 : "__instances"
			// -4 : Type{}
			// -5 : Types{}

			lua_setmetatable(L, -2);
			lua_rawset(L, -3);				//type.__instances = t;

			// -1 : Types{ __instances = {} }
			// -2 : Types{}

			//type.__metatable = { __index = type } ;
			lua_pushstring(L,"__index");
			lua_pushvalue(L,-2);

			// -1 : Type{ __instances = {} }
			// -2 : "__index"
			// -3 : Type{ __instances = {} }
			// -4 : Types{}

			lua_rawset(L,-3);

			// -1 : Type{ ... }
			// -2 : Types{}

			lua_pushstring(L, Derived::GetTypeName());
			lua_pushvalue(L, -2);

			// -1 : Type{ ... }
			// -2 : $TypeName$
			// -3 : Type{ ... }
			// -4 : Types{}

			lua_rawset(L,-4);
			Derived::PushMethods(L, lua_gettop(L));
		}

		// -1 : Type{}
		// -2 : Types{}
		lua_replace(L,-2);
}

template<class Derived, class InstanceType>
void CvLuaScopedInstance<Derived, InstanceType>::Push(lua_State* L, InstanceType* pkType)
{
	if(pkType)
	{
		Derived::PushTypeTable(L);

		// -1 : Type{}

		lua_pushstring(L, "__instances");
		lua_rawget(L, -2);

		// -1 : Instances{} = Type["__instances"]
		// -2 : Type{}

		lua_pushlightuserdata(L, pkType);
		lua_rawget(L, -2);					//retrieve type.__instances[pkType]

		// -1 : Obj{}/nil = Instances[pkType]
		// -2 : Instances{}
		// -3 : Type{}

		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			lua_createtable(L, 0, 1);

			//Push and assign pointer
			lua_pushstring(L,"__instance");
			lua_pushlightuserdata(L, pkType);
			lua_rawset(L, -3);

			//Push and assign metatable
			lua_pushvalue(L,-3);
			lua_setmetatable(L, -2);

			//Assign it in instances
			lua_pushlightuserdata(L, pkType);
			lua_pushvalue(L, -2);
			lua_rawset(L, -4);				//__instances[pkType] = t;
		}

		// -1 : Obj{}
		// -2 : Instances{}
		// -3 : Type{}

		lua_replace(L,-3);

		// -1 : Instances{}
		// -2 : Obj{}

		lua_pop(L,1);
		
	}
	else
	{
		lua_pushnil(L);
	}
}
//------------------------------------------------------------------------------
template<class Derived, class InstanceType>
InstanceType* CvLuaScopedInstance<Derived, InstanceType>::GetInstance(lua_State* L, int idx, bool bErrorOnFail)
{
#if defined(VPDEBUG)
	// Track Lua state for debugging
	// if (GC.getLogging())
	// {
	// 	char szDebugInfo[256];
	// 	sprintf_s(szDebugInfo, "Lua GetInstance - Stack size: %d, Index: %d, Type: %s\n",
	// 		lua_gettop(L), idx, lua_typename(L, lua_type(L, idx)));
	// 	OutputDebugString(szDebugInfo);
	// }
#endif

	const int stack_size = lua_gettop(L);
	bool bFail = true;

	InstanceType* pkInstance = NULL;
	if (lua_type(L, idx) == LUA_TTABLE)
	{
		lua_getfield(L, idx, "__instance");
		if (lua_type(L, -1) == LUA_TLIGHTUSERDATA)
		{
			pkInstance = static_cast<InstanceType*>(lua_touserdata(L, -1));
			if (pkInstance)
			{
				bFail = false;
			}
#if defined(VPDEBUG)
			else
			{
				OutputDebugString("Warning: NULL instance in valid userdata\n");
			}
#endif
		}
#if defined(VPDEBUG)
		else
		{
			OutputDebugString("Warning: __instance is not lightuserdata\n");
		}
#endif
	}
#if defined(VPDEBUG)
	else if (bErrorOnFail  || (lua_type(L, idx) != LUA_TNIL && lua_type(L, idx) != LUA_TNONE))
	{
		char szWarning[256];
		sprintf_s(szWarning, "Warning: Expected table or no value at index %d, got %s\n",
			idx, lua_typename(L, lua_type(L, idx)));
		OutputDebugString(szWarning);
	}
#endif

	lua_settop(L, stack_size);

	if (bFail && bErrorOnFail)
	{
		if (idx == 1)
			luaL_error(L, "Not a valid instance.  Either the instance is NULL or you used '.' instead of ':'.");
		Derived::HandleMissingInstance(L);
	}

	return pkInstance;
}
//------------------------------------------------------------------------------
template<class Derived, class InstanceType>
const int CvLuaScopedInstance<Derived, InstanceType>::GetStartingArgIndex()
{
	return 2;
}
//------------------------------------------------------------------------------
template<class Derived, class InstanceType>
void CvLuaScopedInstance<Derived, InstanceType>::DefaultHandleMissingInstance(lua_State* L)
{
	luaL_error(L, "Instance does not exist.");
}


#endif //CVLUASCOPEDNSTANCE_H