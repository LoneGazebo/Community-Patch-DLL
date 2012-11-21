//------------------------------------------------------------------------------------------------
//
//  ***************** FIRAXIS GAME ENGINE   ********************
//
//! \file		FLuaTypeExposure.h
//! \author		Eric Jordan -- 3/18/2009
//! \brief		Use to expose C++ classes to lua.
//              FLUA_EXPOSE_TYPE exposes a class name to lua
//              FLUA_EXPOSE_INHERITANCE exposes the inheritance of a type to lua
//              FLUA_EXPOSE_MEMBER exposes a member function to lua
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef FLuaTypeExposure_h
#define FLuaTypeExposure_h
#pragma once

#include "FLuaCommon.h"

// FLUA_EXPOSE_TYPE(CLASS, NAME)
// Exposes CLASS to lua under with NAME as the type name in lua
#define FLUA_EXPOSE_TYPE_EX(CLASS, NAME) \
	static FLua::TypeExposure<CLASS>::NameRegistrar LuaRegTypeName_##NAME##(#NAME);

// FLUA_EXPOSE_TYPE(CLASS)
// Exposes CLASS to lua under with CLASS as the type name in lua
#define FLUA_EXPOSE_TYPE(CLASS) FLUA_EXPOSE_TYPE_EX(CLASS, CLASS)

// FLUA_EXPOSE_INHERITANCE_EX(PARENT, CHILD, REGISTRAR_NAME)
// Lets lua know that CHILD inherits from PARENT
// Use this when FLUA_EXPOSE_INHERITANCE generates an invalid name for the registrar
#define FLUA_EXPOSE_INHERITANCE_EX(CHILD, PARENT, REGISTRAR_NAME) \
	static FLua::TypeExposure<CHILD>::ParentRegistrar<PARENT> LuaReg_Inheritance_##REGISTRAR_NAME

// FLUA_EXPOSE_INHERITANCE(PARENT, CHILD)
// Lets lua know that CHILD inherits from PARENT
#define FLUA_EXPOSE_INHERITANCE(CHILD, PARENT) FLUA_EXPOSE_INHERITANCE_EX(CHILD, PARENT, ##PARENT##_##CHILD##)

// FLUA_EXPOSE_MEMBER_REX(CLASS, FUNC, THUNK_NAME, CATEGORY_FLAGS)
// Exposes the member function FUNC on type CLASS with a thunk name of THUNK_NAME
// Restrict the exposure to only states that fall into one of the provided category flags
// Use this when FLUA_EXPOSE_MEMBER_R generates and invalid name for the thunk function or registrar
#define FLUA_EXPOSE_MEMBER_REX(CLASS, FUNC, THUNK_NAME, CATEGORY_FLAGS) \
	int LuaThunk_##CLASS##_##THUNK_NAME##(lua_State *L) { \
	return FLua::Details::Call<CLASS>(&CLASS::FUNC, L, #CLASS "::" #THUNK_NAME); \
	} \
	int LuaToString_##CLASS##_##THUNK_NAME##(lua_State *L) { \
	char szDescription[256]; \
	FLua::Details::DescribeFunc(&CLASS::FUNC, #THUNK_NAME, szDescription, 256); \
	lua_pushstring(L, szDescription); \
	return 1; \
	} \
	static FLua::TypeExposure<##CLASS##>::FunctionRegistrar LuaReg_##CLASS##_##THUNK_NAME## \
	(#THUNK_NAME, &LuaThunk_##CLASS##_##THUNK_NAME##, &LuaToString_##CLASS##_##THUNK_NAME##, FLua::Details::DoConstCheck(&CLASS::FUNC), CATEGORY_FLAGS)

// FLUA_EXPOSE_MEMBER_EX(CLASS, FUNC, THUNK_NAME)
// Exposes the member function FUNC on type CLASS with a thunk name of THUNK_NAME.
// Use this when FLUA_EXPOSE_MEMBER generates and invalid name for the thunk function or registrar
#define FLUA_EXPOSE_MEMBER_EX(CLASS, FUNC, THUNK_NAME) FLUA_EXPOSE_MEMBER_REX(CLASS, FUNC, THUNK_NAME, FLua::TypeExposures::sm_uiUnrestrictedExposure)

// FLUA_EXPOSE_MEMBER(CLASS, FUNC)
// Exposes the member function FUNC on type CLASS
// Restrict the exposure to only states that fall into one of the provided category flags
#define FLUA_EXPOSE_MEMBER_R(CLASS, FUNC, CATEGORY_FLAGS) FLUA_EXPOSE_MEMBER_REX(CLASS, FUNC, FUNC, CATEGORY_FLAGS)

// FLUA_EXPOSE_MEMBER(CLASS, FUNC)
// Exposes the member function FUNC on type CLASS
#define FLUA_EXPOSE_MEMBER(CLASS, FUNC) FLUA_EXPOSE_MEMBER_EX(CLASS, FUNC, FUNC)

// FLUA_EXPOSE_MEMBER_AS_NONCONST(CLASS, FUNC)
// Exposes the member function FUNC on type CLASS.  Use this for when there are a const and noncost
// version of a function with the same name.  It will always expose the nonconst version.
#define FLUA_EXPOSE_MEMBER_AS_NONCONST(CLASS, FUNC) \
	int LuaThunk_##CLASS##_##FUNC##(lua_State *L) { \
	return FLua::Details::Call<CLASS>(&CLASS::FUNC, L, #CLASS "::" #FUNC); \
	} \
	int LuaToString_##CLASS##_##FUNC##(lua_State *L) { \
	char szDescription[256]; \
	FLua::Details::DescribeFunc(&CLASS::FUNC, #FUNC, szDescription, 256); \
	lua_pushstring(L, szDescription); \
	return 1; \
	} \
	static FLua::TypeExposure<##CLASS##>::FunctionRegistrar LuaReg_##CLASS##_##FUNC## \
	(#FUNC, &LuaThunk_##CLASS##_##FUNC##, &LuaToString_##CLASS##_##FUNC##, false, FLua::TypeExposures::sm_uiUnrestrictedExposure)

// FLUA_SUPPORT_ENUM(ENUM)
// Allows an enum to be used in lua exposures.  Does not expose the members of the enum.
#define FLUA_SUPPORT_ENUM(ENUM) \
	namespace FLua { \
	namespace Details { \
	template<> static inline ENUM Get(lua_State *L, int idx) { return (ENUM)lua_tointeger(L, idx); } \
	static inline void Push(lua_State *L, const ENUM &eVal) { lua_pushinteger(L, eVal); } \
	template<> static const char *DescribeType<ENUM>() { return #ENUM; } \
		} \
	}

namespace FLua
{
	// TypeExposures - Handles common functionality for exposed types
	class TypeExposures
	{
	public:
		static const unsigned int sm_uiMaxTypeExposures = 1024;
		static const unsigned int sm_uiUnrestrictedExposure = (unsigned int)-1; // Category flags for unrestricted exposure

		static void Register(lua_State *L, unsigned int uiCategoryFlags)
		{
			TypeExposures::PushTypesTable(L); // Push the types table
			TypeExposures::PushMembersTable(L); // Push the members table to map member functions

			TypeExposures &kRef = Ref();
			for( size_t i = 0; i < kRef.m_uiTypeEntryCount; ++i )
				(*kRef.m_aTypeEntries[i])(L, uiCategoryFlags);

			lua_pop(L, 2); // Pop members table and types table
		}

		static void PushTypesTable(lua_State *L) {
			lua_getglobal(L, "ExposedTypes");
			if( !lua_istable(L, -1) ) {
				lua_pop(L, 1); // Get nil off of the stack
				lua_newtable(L); // Create a new table

				// Assign to global value
				lua_pushvalue(L, -1);
				lua_setglobal(L, "ExposedTypes");

				// Create metatable for type tables
				lua_newtable(L);
				lua_pushcfunction(L, TypeToString);
				lua_setfield(L, -2, "__tostring");
				lua_rawseti(L, -2, 1); // Store as the first array member of the ExposedTypes table
			}
		}

		static void PushMembersTable(lua_State *L) {
			lua_getglobal(L, "ExposedMembers");
			if( !lua_istable(L, -1) ) {
				lua_pop(L, 1); // Get nil off of the stack
				lua_newtable(L); // Create a new table

				// Assign to global value
				lua_pushvalue(L, -1);
				lua_setglobal(L, "ExposedMembers");
			}
		}

		static int DefaultToString(lua_State *L) {
			if( lua_getmetatable(L, -1) ) { // The C type info is in the metatable
				lua_getfield(L, -1, "CTypeName"); // Push CTypeName field from metatable
				lua_getfield(L, -3, "CData"); // Push CData field from original table
				Details::CData kCData = (Details::CData)lua_touserdata(L, -1); // Get CData
				lua_pop(L, 1); // Pop CData field
				char szPtr[32];	// String buffer for pointer address
				sprintf_s(szPtr, 32, ": %p", kCData.GetPtr()); // Format the pointer address
				lua_pushstring(L, szPtr); // Push the formated pointer address onto the stack
				lua_concat(L, 2); // Concat CTypeName with the pointer address
				lua_remove(L, -2); // Remove metatable from the stack
			}
			else { // If there was no metatable then something is seriously wrong
				Details::Error("Missing metatable for table representing a C Object.");
				lua_pushstring(L, "Malformed C Reflection");
			}

			return 1;
		}

		// TypeToString - Serves as the tostring function for type metatables
		static int TypeToString(lua_State *L) {
			lua_pushstring(L, "Metatable for type: ");
			lua_getfield(L, -2, "CTypeName"); // Get the CTypeName
			if( !lua_isstring(L, -1) )
			{	// Invalid CTypeName field
				lua_pop(L, 1); // Pop invalid CTypename
				lua_pushstring(L, "Unknown");
			}
			lua_concat(L, 2); // Format final string
			return 1;
		}

		static int Equals(lua_State *L) {
			bool bEqual = false;
			if( lua_istable(L, 1) && lua_istable(L, 2) ) {
				// Get the CData fields
				lua_getfield(L, 1, "CData");
				lua_getfield(L, 2, "CData");
				if( lua_isnil(L, 3) ) {
					if( lua_isnil(L, 4) )
						bEqual = lua_topointer(L, 1) == lua_topointer(L, 2);
				}
				else {
					Details::CData kCDataLhs = (Details::CData)lua_touserdata(L, 3);
					Details::CData kCDataRhs = (Details::CData)lua_touserdata(L, 4);
					bEqual = kCDataLhs.GetPtr() == kCDataRhs.GetPtr();
				}
				lua_pop(L, 2); // Pop off CData fields
			}

			lua_pushboolean(L, bEqual);
			return 1;
		}

		// IndexFunction - This is the way that member functions are resolved.  It is horrifying.
		// Rather than keep the index tables in the metatables they are kept in the members table.
		// This is so exposure restrictions will remain relevant even if a value is passed from
		// one state to another while keeping the same metatable.  The index tables are retrieved
		// by indexing into the members table with the metatable.  Members may also come from 
		// inheritance so the base type's metatable must be put through the same process if the
		// key is not resolved.
		static int IndexFunction(lua_State *L) {
			// Table at -2, Key at -1
			PushMembersTable(L); // Push members table for key resolution
			if( lua_getmetatable(L, -3) != 0 ) // Get the metatable for the table
			{	// Table has valid metatable
				bool bContinue = true;
				do 
				{
					lua_pushvalue(L, -1); // Push copy for lua_rawget
					lua_rawget(L, -3); // Use the metatable as a key into the members table
					if( lua_istable(L, -1) )
					{	// Valid index table
						lua_pushvalue(L, -4); // Push the key
						lua_rawget(L, -2); // Try getting the value
						if( !lua_isnil(L, -1) )
						{	// Got the value from the index table
							lua_insert(L, -4); // Insert the value below the local variables
							lua_pop(L, 3); // Pop the index table, metatable, and members table
							return 1;
						}
						else
						{	// Value does not exist in this index table
							lua_pop(L, 2); // Pop nil value and index table
						}
					}
					else
					{	// Invalid index table
						lua_pop(L, 1); // Pop Index Table
					}

					// See if the metatable has a metatable
					lua_getfield(L, -1, "BaseType");
					if( lua_istable(L, -1) )
					{	// Valid BaseType metatable
						lua_remove(L, -2); // Remove old metatable
					}
					else
					{	// BaseType metatable does not exist
						lua_pop(L, 3); // Pop nil, metatable and the members Table
						bContinue = false; // Stop the loop
					}
				} while (bContinue);
			}
			else
			{	// No metatable
				lua_pop(L, 1); // Pop members table
			}
			lua_pushnil(L); // No value found for this key
			return 1;
		}

		typedef void (*ExposeTypeFn)(lua_State *L, unsigned int uiCategoryFlags);
		static void AddExposedType(ExposeTypeFn pfnExposeFn)
		{
			TypeExposures &kRef = Ref();

			// If there are no free entries for type exposures then we're in trouble
			assert(kRef.m_uiTypeEntryCount < sm_uiMaxTypeExposures && "Not enough space for exposing types");
			if( kRef.m_uiTypeEntryCount < sm_uiMaxTypeExposures )
				kRef.m_aTypeEntries[kRef.m_uiTypeEntryCount++] = pfnExposeFn;
		}

	private:
		TypeExposures() : m_uiTypeEntryCount(0) {}
		static inline TypeExposures &Ref() { static TypeExposures kInst; return kInst; }

		ExposeTypeFn m_aTypeEntries[sm_uiMaxTypeExposures];
		size_t m_uiTypeEntryCount;
	};

	// TypeExposure - Handles exposure of type T to lua
	template<class T>
	class TypeExposure
	{
	public:
		// sm_uiMaxFunctions - Determines the maximum number of member functions.
		static const unsigned int sm_uiMaxFunctions = 64;

		// NameRegistrar - Registers name for type
		struct NameRegistrar {
			inline NameRegistrar(const char *szName) {
				Ref().m_szName = szName;
				sprintf_s(Ref().m_szConstName, 64, "const %s", szName);

				Details::TypeNameHolder<T>::Get() = szName;
				Details::TypeNameHolder<const T>::Get() = Ref().m_szConstName;
			}
		};

		// ParentRegistrar - Registers parent for T as TParent
		template<class TParent>
		struct ParentRegistrar {
			inline ParentRegistrar(){
				TypeExposure &kRef = Ref();
				kRef.m_pfnPushParentMetatable = TypeExposure<TParent>::PushMetaTable;
				kRef.m_pfnPushConstParentMetatable = TypeExposure<TParent>::PushConstMetaTable;
			}
		};

		// FunctionRegistrar - Registers member functions
		struct FunctionRegistrar {
			inline FunctionRegistrar(const char *szName, lua_CFunction pfnThunk, lua_CFunction pfnToString, bool bIsConst, unsigned int uiCategoryFlags) {
				TypeExposure &kRef = Ref();

				// If there are no free entries for functions then we're in trouble
				assert(kRef.m_uiFuncEntryCount < sm_uiMaxFunctions && "Not enough space for exposing functions");

				// Add the entry
				kRef.m_aFuncEntries[kRef.m_uiFuncEntryCount++].Set(szName, pfnThunk, pfnToString, bIsConst, uiCategoryFlags);
			}
		};

		// PushMetaTable - Push the metatable for type T onto the stack
		static void PushMetaTable(lua_State *L)
		{
			TypeExposure &kRef = Ref();

			// Try to find the metatable from the types table
			TypeExposures::PushTypesTable(L);
			lua_getfield(L, -1, kRef.m_szName);
			if( !lua_istable(L, -1) ) {
				lua_pop(L, 2); // Get nil and types table off of stack
				kRef.CreateMetaTable(L); // Metatable not found so create it
			}
			else
				lua_remove(L, -2); // Get types table off of stack
		}

		// PushConstMetaTable - Push the metatable for type const T onto the stack
		static void PushConstMetaTable(lua_State *L)
		{
			TypeExposure &kRef = Ref();

			// Try to find the metatable from the types table
			TypeExposures::PushTypesTable(L);
			lua_getfield(L, -1, kRef.m_szConstName);
			if( !lua_istable(L, -1) ) {
				lua_pop(L, 2); // Get nil and types table off of stack
				kRef.CreateConstMetaTable(L); // Metatable not found so create it
			}
			else
				lua_remove(L, -2); // Get types table off of stack
		}

	private:
		struct FuncEntry
		{
			inline void Set(const char *szName, lua_CFunction pfnThunk, lua_CFunction pfnToString, bool bConstFunc, unsigned int uiCategoryFlags)
			{
				m_szName = szName;
				m_pfnThunk = pfnThunk;
				m_pfnToString = pfnToString;
				m_bConstFunc = bConstFunc;
				m_uiCategoryFlags = uiCategoryFlags;
			}

			const char *m_szName;
			unsigned int m_uiCategoryFlags;
			lua_CFunction m_pfnThunk;
			lua_CFunction m_pfnToString;
			bool m_bConstFunc;
		};

		TypeExposure() :
			m_uiFuncEntryCount(0), m_pfnPushParentMetatable(NULL), m_pfnPushConstParentMetatable(NULL)
		{ 
			m_szName = "Unkown";
			Details::GetPushMetaTableFn<T>() = &PushMetaTable;
			Details::GetPushMetaTableFn<const T>() = &PushConstMetaTable;

			TypeExposures::AddExposedType(&TypeExposure::ExposeMembers);
		}

		static inline TypeExposure &Ref() { static TypeExposure kInst; return kInst; }

		// CreateMetaTable - Create a metatable for T and push it onto the stack
		void CreateMetaTable(lua_State *L)
		{
			// Metatable
			lua_newtable(L);

			// Add type name
			lua_pushstring(L, m_szName);
			lua_setfield(L, -2, "CTypeName");

			// Add the default __tostring function for type exposures
			lua_pushcfunction(L, &TypeExposures::DefaultToString);
			lua_setfield(L, -2, "__tostring");

			// Add the equals function
			lua_pushcfunction(L, &TypeExposures::Equals);
			lua_setfield(L, -2, "__eq");

			// Add indexing function
			lua_pushcfunction(L, TypeExposures::IndexFunction);
			lua_setfield(L, -2, "__index");

			// Add parent table
			if( m_pfnPushParentMetatable )
			{
				(*m_pfnPushParentMetatable)(L);
				lua_setfield(L, -2, "BaseType");
			}

			TypeExposures::PushTypesTable(L);

			// Try getting the metatable for type tables
			lua_rawgeti(L, -1, 1);
			if( lua_istable(L, -1) )
			{	// Valid metatable for type tables
				lua_setmetatable(L, -3);
			}
			else
			{	// Invalid metatable for type tables
				lua_pop(L, 1); // Pop invalid table
			}

			lua_pushvalue(L, -2); // Push created metatable onto the top of the stack.
			lua_setfield(L, -2, m_szName); // Assign created metatable table to types table
			lua_pop(L, 1); // Pop types table
		}

		// CreateTypeMemberTable - Create the members table for this type.
		void CreateTypeMemberTable(lua_State *L)
		{
			lua_newtable(L); // Table for this types member functions
			for( size_t i = 0; i < m_uiFuncEntryCount; ++i )
			{
				const FuncEntry &kEntry = m_aFuncEntries[i];
				lua_pushcfunction(L, kEntry.m_pfnThunk);
				Details::SetToStringForFunc(L, kEntry.m_pfnToString);
				lua_setfield(L, -2, kEntry.m_szName);
			}
		}

		void CreateTypeMemberTable(lua_State *L, unsigned int uiCategoryFlags)
		{
			lua_newtable(L); // Table for this types member functions
			for( size_t i = 0; i < m_uiFuncEntryCount; ++i )
			{
				const FuncEntry &kEntry = m_aFuncEntries[i];
				if( kEntry.m_uiCategoryFlags == TypeExposures::sm_uiUnrestrictedExposure || (kEntry.m_uiCategoryFlags & uiCategoryFlags) != 0 )
				{
					lua_pushcfunction(L, kEntry.m_pfnThunk);
					Details::SetToStringForFunc(L, kEntry.m_pfnToString);
					lua_setfield(L, -2, kEntry.m_szName);
				}
			}
		}

		// CreateConstMetaTable - Create a metatable for const T and push it onto the stack
		void CreateConstMetaTable(lua_State *L)
		{
			// Metatable
			lua_newtable(L);

			// Add type name
			lua_pushstring(L, m_szConstName);
			lua_setfield(L, -2, "CTypeName");

			// Add the default __tostring function for type exposures
			lua_pushcfunction(L, &TypeExposures::DefaultToString);
			lua_setfield(L, -2, "__tostring");

			// Add the equals function
			lua_pushcfunction(L, &TypeExposures::Equals);
			lua_setfield(L, -2, "__eq");

			// Add indexing function
			lua_pushcfunction(L, TypeExposures::IndexFunction);
			lua_setfield(L, -2, "__index");

			// Add parent table
			if( m_pfnPushConstParentMetatable )
			{
				(*m_pfnPushConstParentMetatable)(L);
				lua_setfield(L, -2, "BaseType");
			}

			TypeExposures::PushTypesTable(L);

			// Try getting the metatable for type tables
			lua_rawgeti(L, -1, 1);
			if( lua_istable(L, -1) )
			{	// Valid metatable for type tables
				lua_setmetatable(L, -3);
			}
			else
			{	// Invalid metatable for type tables
				lua_pop(L, 1); // Pop invalid table
			}

			lua_pushvalue(L, -2); // Push created metatable onto the top of the stack.
			lua_setfield(L, -2, m_szConstName); // Assign created metatable table to types table
			lua_pop(L, 1); // Pop types table
		}

		// CreateConstTypeMemberTable - Create the const members table for this type.
		void CreateConstTypeMemberTable(lua_State *L)
		{
			lua_newtable(L); // Table for this types member functions
			for( size_t i = 0; i < m_uiFuncEntryCount; ++i )
			{
				const FuncEntry &kEntry = m_aFuncEntries[i];
				if( kEntry.m_bConstFunc )
				{
					lua_pushcfunction(L, kEntry.m_pfnThunk);
					Details::SetToStringForFunc(L, kEntry.m_pfnToString);
					lua_setfield(L, -2, kEntry.m_szName);
				}
			}
		}

		void CreateConstTypeMemberTable(lua_State *L, unsigned int uiCategoryFlags)
		{
			lua_newtable(L); // Table for this types member functions
			for( size_t i = 0; i < m_uiFuncEntryCount; ++i )
			{
				const FuncEntry &kEntry = m_aFuncEntries[i];
				if( kEntry.m_bConstFunc && (kEntry.m_uiCategoryFlags == TypeExposures::sm_uiUnrestrictedExposure || (kEntry.m_uiCategoryFlags & uiCategoryFlags) != 0) )
				{
					lua_pushcfunction(L, kEntry.m_pfnThunk);
					Details::SetToStringForFunc(L, kEntry.m_pfnToString);
					lua_setfield(L, -2, kEntry.m_szName);
				}
			}
		}

		// ExposeMembers - Create the exposures for this type restricted by uiCategoryFlags.
		// Expects MembersTable at -1 and TypesTable at -2.  Will crash otherwise.
		static void ExposeMembers(lua_State *L, unsigned int uiCategoryFlags)
		{
			TypeExposure &kRef = Ref();

			// Try to find the metatable from the types table
			lua_getfield(L, -2, kRef.m_szName);
			if( !lua_istable(L, -1) )
			{
				lua_pop(L, 1); // Pop nil off of stack
				kRef.CreateMetaTable(L); // Metatable not found so create it
			}

			kRef.CreateTypeMemberTable(L); // Create the member table for this type
			lua_rawset(L, -3); // Map the member table using the metatable as a key

			// Try to find the const metatable from the types table
			lua_getfield(L, -2, kRef.m_szConstName);
			if( !lua_istable(L, -1) )
			{
				lua_pop(L, 1); // Pop nil off of stack
				kRef.CreateConstMetaTable(L); // Metatable not found so create it
			}

			kRef.CreateConstTypeMemberTable(L); // Create the member table for this type
			lua_rawset(L, -3); // Map the member table using the metatable as a key
		}

		const char *m_szName;
		char m_szConstName[64];
		FuncEntry m_aFuncEntries[sm_uiMaxFunctions];
		size_t m_uiFuncEntryCount;

		Details::PushMetaTableFn m_pfnPushParentMetatable;
		Details::PushMetaTableFn m_pfnPushConstParentMetatable;
	};

	namespace Details
	{
		// Call specializations for lua c functions
		template<class TClass>
		static int Call(lua_CFunction pfn, lua_State *L, const TCHAR *szLuaFnName) {
			return pfn(L);
		}

		// Call specializations for non-const member functions

		// 0 args with no return value
		template<class TClass, class TFuncOwner>
		static int Call(void(TFuncOwner::*pfn)(), lua_State *L, const TCHAR *szLuaFnName) {
			if( lua_gettop(L) == 1 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) 
				{
					FLUA_SAVECALLSTACK(L);
					(pkObj->*pfn)();
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and no arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 0 args with return value
		template<class TClass, class TFuncOwner, class TRet>
		static int Call(TRet(TFuncOwner::*pfn)(), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			if( lua_gettop(L) == 1 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					TRet ret = (pkObj->*pfn)();
					RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
					Push( L, pushMe );
					return 1;
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and no arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 1 arg with no return value
		template<class TClass, class TFuncOwner, class TArg0>
		static int Call(void(TFuncOwner::*pfn)(TArg0), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 2 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 1 argument."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 1 arg with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 2 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 1 argument."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 2 args with no return value
		template<class TClass, class TFuncOwner, class TArg0, class TArg1>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 3 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 2 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 2 args with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 3 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 2 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 3 args with no return value
		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 4 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
									  FromLuaAnalog<TArg2>::Convert(arg2));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 3 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 3 args with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 4 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
												 FromLuaAnalog<TArg2>::Convert(arg2));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 3 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 4 args with no return value
		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 5 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2),
									  FromLuaAnalog<TArg3>::Convert(arg3));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 4 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 4 args with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 5 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                         FromLuaAnalog<TArg2>::Convert(arg2),
												 FromLuaAnalog<TArg3>::Convert(arg3));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 4 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 5 args with no return value
		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4), lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 6 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					Analog4 arg4 = Get<Analog4>(L, 6); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2),
						              FromLuaAnalog<TArg3>::Convert(arg3),
									  FromLuaAnalog<TArg4>::Convert(arg4));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 5 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 5 args with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4), lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 6 ) {
				TClass *pkObj = FLua::Details::Get<TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					Analog4 arg4 = Get<Analog4>(L, 6); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                         FromLuaAnalog<TArg2>::Convert(arg2),
						                         FromLuaAnalog<TArg3>::Convert(arg3),
												 FromLuaAnalog<TArg4>::Convert(arg4));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 5 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// Call specializations for const member functions

		// 0 args with no return value
		template<class TClass, class TFuncOwner>
		static int Call(void(TFuncOwner::*pfn)()const, lua_State *L, const TCHAR *szLuaFnName) {
			if( lua_gettop(L) == 1 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) 
				{
					FLUA_SAVECALLSTACK(L);
					(pkObj->*pfn)();
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and no arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 0 args with return value
		template<class TClass, class TFuncOwner, class TRet>
		static int Call(TRet(TFuncOwner::*pfn)()const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			if( lua_gettop(L) == 1 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					TRet ret = (pkObj->*pfn)();
					RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
					Push( L, pushMe );
					return 1;
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and no arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 1 arg with no return value
		template<class TClass, class TFuncOwner, class TArg0>
		static int Call(void(TFuncOwner::*pfn)(TArg0)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 2 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 1 argument."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 1 arg with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			if( lua_gettop(L) == 2 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 1 argument."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 2 args with no return value
		template<class TClass, class TFuncOwner, class TArg0, class TArg1>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 3 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 2 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 2 args with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			if( lua_gettop(L) == 3 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0), FromLuaAnalog<TArg1>::Convert(arg1));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 2 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 3 args with no return value
		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 4 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 3 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 3 args with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			if( lua_gettop(L) == 4 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                          FromLuaAnalog<TArg2>::Convert(arg2));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 3 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 4 args with no return value
		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 5 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2),
						              FromLuaAnalog<TArg3>::Convert(arg3));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 4 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 4 args with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2, class TArg3>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			if( lua_gettop(L) == 5 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                         FromLuaAnalog<TArg2>::Convert(arg2),
						                         FromLuaAnalog<TArg3>::Convert(arg3));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 4 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 5 args with no return value
		template<class TClass, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(void(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 6 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					Analog4 arg4 = Get<Analog4>(L, 6); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
					if( bValidArgs )
						(pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						              FromLuaAnalog<TArg1>::Convert(arg1),
						              FromLuaAnalog<TArg2>::Convert(arg2),
						              FromLuaAnalog<TArg3>::Convert(arg3),
						              FromLuaAnalog<TArg4>::Convert(arg4));
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 5 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// 5 args with return value
		template<class TClass, class TFuncOwner, class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static int Call(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4)const, lua_State *L, const TCHAR *szLuaFnName) {
			typedef PushAnalog<TRet>::Result RetAnalog;
			typedef LuaAnalog<TArg0>::Result Analog0;
			typedef LuaAnalog<TArg1>::Result Analog1;
			typedef LuaAnalog<TArg2>::Result Analog2;
			typedef LuaAnalog<TArg3>::Result Analog3;
			typedef LuaAnalog<TArg4>::Result Analog4;
			if( lua_gettop(L) == 6 ) {
				const TClass *pkObj = FLua::Details::Get<const TClass*>(L, 1);
				if( pkObj != NULL) {
					FLUA_SAVECALLSTACK(L);
					bool bValidArgs = true;
					Analog0 arg0 = Get<Analog0>(L, 2); ArgValidator<TArg0>::Validate(arg0, bValidArgs);
					Analog1 arg1 = Get<Analog1>(L, 3); ArgValidator<TArg1>::Validate(arg1, bValidArgs);
					Analog2 arg2 = Get<Analog2>(L, 4); ArgValidator<TArg2>::Validate(arg2, bValidArgs);
					Analog3 arg3 = Get<Analog3>(L, 5); ArgValidator<TArg3>::Validate(arg3, bValidArgs);
					Analog4 arg4 = Get<Analog4>(L, 6); ArgValidator<TArg4>::Validate(arg4, bValidArgs);
					if( bValidArgs ) {
						TRet ret = (pkObj->*pfn)(FromLuaAnalog<TArg0>::Convert(arg0),
						                         FromLuaAnalog<TArg1>::Convert(arg1),
						                         FromLuaAnalog<TArg2>::Convert(arg2),
						                         FromLuaAnalog<TArg3>::Convert(arg3),
						                         FromLuaAnalog<TArg4>::Convert(arg4));
						RetAnalog pushMe = ToLuaAnalog<RetAnalog>::Convert(ret);
						Push( L, pushMe );
						return 1;
					}
					else Error(_T("%s could not be called: received an invalid argument."), szLuaFnName);
				}
				else Error(_T("%s could not be called: Call cannot be made on NULL"), szLuaFnName);
			}
			else Error(_T("%s could not be called: %i values on stack.  Expected 1 parent table and 5 arguments."), szLuaFnName, lua_gettop(L) );
			return 0;
		}

		// DescribeFunc specializations for non-const member functions

		// 0 args
		template<class TRet, class TFuncOwner>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(), const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( void )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName);
		}

		// 1 arg
		template<class TRet, class TFuncOwner, class TArg0>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0), const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get());
		}

		// 2 args
		template<class TRet, class TFuncOwner, class TArg0, class TArg1>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get());
		}

		// 3 args
		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get());
		}

		// 4 args
		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get());
		}

		// 5 args
		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4), const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s, %s%s )",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get(),
				DescribeType<TArg4>(), PtrDescription<TArg4>::Get());
		}

		// DescribeFunc specializations for const member functions

		// 0 args
		template<class TRet, class TFuncOwner>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)()const, const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( void ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName);
		}

		// 1 arg
		template<class TRet, class TFuncOwner, class TArg0>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0)const, const char *szFuncName, char *szDescription, size_t uiSize) {
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get());
		}

		// 2 args
		template<class TRet, class TFuncOwner, class TArg0, class TArg1>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1)const, const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get());
		}

		// 3 args
		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2)const, const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get());
		}

		// 4 args
		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3)const, const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get());
		}

		// 5 args
		template<class TRet, class TFuncOwner, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		static void DescribeFunc(TRet(TFuncOwner::*pfn)(TArg0, TArg1, TArg2, TArg3, TArg4)const, const char *szFuncName, char *szDescription, size_t uiSize)
		{
			sprintf_s(szDescription, uiSize, "%s%s %s( %s%s, %s%s, %s%s, %s%s, %s%s ) const",
				DescribeType<TRet>(), PtrDescription<TRet>::Get(), szFuncName,
				DescribeType<TArg0>(), PtrDescription<TArg0>::Get(),
				DescribeType<TArg1>(), PtrDescription<TArg1>::Get(),
				DescribeType<TArg2>(), PtrDescription<TArg2>::Get(),
				DescribeType<TArg3>(), PtrDescription<TArg3>::Get(),
				DescribeType<TArg4>(), PtrDescription<TArg4>::Get());
		}
	}
}

#endif
