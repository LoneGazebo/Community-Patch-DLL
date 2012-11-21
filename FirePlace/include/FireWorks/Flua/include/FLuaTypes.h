//------------------------------------------------------------------------------------------------
//
//  ***************** FIRAXIS GAME ENGINE   ********************
//
//! \file		FLuaTypes.h
//! \author		Eric Jordan -- 3/12/2009
//! \brief		Classes that wrap lua types for C++
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2009 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------

#ifndef FLuaTypes_h
#define FLuaTypes_h
#pragma once

#include "FLuaCommon.h"

// FLUA_EXPOSE_TO_TABLE_EX(FUNC, NAME)
// Adds FUNC to TABLE with NAME as the function name in lua
#define FLUA_EXPOSE_TO_TABLE_EX(FUNC, NAME, TABLE) \
	void LuaExpose_##NAME##(lua_State *L) { \
		FLua::Table t(L, #TABLE); \
		if( !t.Valid() ) { \
			t = FLua::Table::Create(L); \
			FLua::Table kGlobals(L); \
			kGlobals[#TABLE] = t; \
		} \
		t.AddMemberFunction(#NAME, &FUNC); \
	} \
	static FLua::StaticFunctions::CustomRegistrar LuaReg_##NAME##(&LuaExpose_##NAME##)

// FLUA_EXPOSE_TO_TABLE(FUNC)
// Adds FUNC to TABLE with FUNC as the function name in lua
#define FLUA_EXPOSE_TO_TABLE(FUNC, TABLE) FLUA_EXPOSE_TO_TABLE_EX(FUNC, FUNC, TABLE)

namespace FLua
{
	class Metatable;
	class String;

	// SharedLuaRegistry - Determines if FLua may assume that all lua states share the same registry.
	class SharedLuaRegistry
	{
	public:
		static inline void Set(bool bAllowed) { sm_bAllowed = bAllowed; }
		static inline bool Get() { return sm_bAllowed; }

	private:
		static bool sm_bAllowed;
	};

	class Value
	{
	public:
		Value();
		Value(const Value &kRhs);
		Value(const StackValue &kStackValue); // Get from lua stack value
		Value(lua_State *L, int iStackIndex); // Get from the lua stack
		Value(lua_State *L, _In_z_ const char *szName); // Find with FLua::GetGlobal
		~Value();

		void Swap(Value &kRhs);

		const Value &operator =(const Value &kRhs);
		void Set(lua_State *L, int iStackIndex); // Get from the lua stack
		void Set(lua_State *L, _In_z_ const char *szName); // Find with FLua::GetGlobal

		inline void Clear() { Unref(); }
		void LuaStateDestroyed(); // Warning: Only call if the lua state has already been destroyed

		inline bool Valid() const { return m_pkLuaState && m_iRef != LUA_NOREF; }

		const bool operator ==(const Value &kRhs) const;
		inline const bool operator !=(const Value &kRhs) const { return !(operator==(kRhs)); }

		bool Push() const;

		Type GetType() const;
		_Ret_z_ const char *GetTypeName() const;

		bool IsNil() const;

		String ToString() const;
		const void *ToPointer() const;

		// Basic type conversions
		inline operator bool() const { return GetAs<bool>(); }
		inline operator int() const { return GetAs<int>(); }
		inline operator char() const { return GetAs<char>(); }
		inline operator unsigned int() const { return GetAs<unsigned int>(); }
		inline operator float() const { return GetAs<float>(); }
		inline operator double() const { return GetAs<double>(); }
		inline operator const char*() const { return GetAs<const char*>(); }
		template<class T> _Ret_opt_ operator T*() const { return GetAs<T*>(); }
		
		// GetAs - Interpret this value as a specific C++ type
		template<class T>
		T GetAs() const {
			T ret = T(); // Don't use with reference types!!! ...EVER!!!

			// Try pushing this value onto the lua stack
			if( Push() ) {
				// Get the lua analog for this type off of the lua stack
				typedef Details::LuaAnalog<T>::Result Analog;
				Analog analog = Details::Get<Analog>(m_pkLuaState, lua_gettop(m_pkLuaState));

				// Validate the value from lua
				bool bValid(true); Details::ArgValidator<T>::Validate(analog, bValid);

				// Convert and return the value if it is valid
				if( bValid ) ret = Details::FromLuaAnalog<T>::Convert(analog);

				// Finally pop the lua value off of the stack
				lua_pop(m_pkLuaState, 1);
			}
			return ret;
		}

		// IsOfType - Determines if the value may be cast to a pointer of type T
		template<class T>
		bool IsOfType() const {
			if( Push() ) {
				bool bCompatible = Details::CompatibleType<T>(m_pkLuaState, lua_gettop(m_pkLuaState));
				lua_pop(m_pkLuaState, 1);
				return bCompatible;
			}
			else return false;
		}

		Metatable GetMetatable() const;

		// FromCFunction - Makes an FLua::Value out of a C function
		template<class TFunc>
		static Value FromCFunction(lua_State *L, TFunc pfn, _In_z_ const char *szFuncName)
		{
			Details::PushFunction(L, pfn, szFuncName);
			Value fnFunc(L, -1);
			lua_pop(L, 1);
			return fnFunc;
		}

		inline lua_State *GetLuaState() const { return m_pkLuaState; }

		// WARNING: LuaValue owns this lua ref!
		inline int GetRef() const { return m_iRef; }

	private:
		void Unref();

		lua_State *m_pkLuaState;
		int m_iRef;
	};

	class String
	{
	public:
		String() : m_kLuaVal() {}
		String(const String &kRhs) : m_kLuaVal(kRhs.m_kLuaVal) {}
		String(const Value &kLuaVal) : m_kLuaVal(kLuaVal) {}
		String(const StackValue &kStackValue) : m_kLuaVal(kStackValue) {}
		String(lua_State *L, int iStackIndex) : m_kLuaVal(L, iStackIndex) {}
		~String() {}

		static String Create(lua_State *L, _In_z_ const char *sz);

		inline operator const char*() const { return c_str(); }
		_Ret_opt_ const char *c_str() const;

		inline void Swap(String &kRhs) { m_kLuaVal.Swap(kRhs.m_kLuaVal); }

		inline const String &operator =(const String &kRhs) { m_kLuaVal = kRhs.m_kLuaVal; return *this; }
		inline const String &operator =(const Value &kLuaValue) { m_kLuaVal = kLuaValue; return *this; }
		inline void Set(lua_State *L, int iStackIndex) { m_kLuaVal.Set(L, iStackIndex); }
		inline void Set(lua_State *L, _In_z_ const char *szName) { m_kLuaVal.Set(L, szName); }

		inline void Clear() { m_kLuaVal.Clear(); }

		// Warning: Only call if the lua state has already been destroyed
		inline void LuaStateDestroyed() { m_kLuaVal.LuaStateDestroyed(); }

		bool Valid() const;

		inline const bool operator ==(const String &kRhs) const { return m_kLuaVal == kRhs.m_kLuaVal; }
		inline const bool operator !=(const String &kRhs) const { return !(m_kLuaVal == kRhs.m_kLuaVal); }
		inline const bool operator ==(const Value &kRhs) const { return m_kLuaVal == kRhs; }
		inline const bool operator !=(const Value &kRhs) const { return !(m_kLuaVal == kRhs); }

		// Concat operators
		String operator +(const String &sRhs) const;
		String operator +( _In_z_ const char *szRhs) const;

		// Prepend and Append functionality
		void Prepend(const String &sStart);
		void Prepend( _In_z_ const char *szStart);
		void Append(const String &sEnd);
		void Append( _In_z_ const char *szEnd);

		bool Push() const;

		inline const Value &GetLuaValue() const { return m_kLuaVal; }
		inline lua_State *GetLuaState() const { return m_kLuaVal.GetLuaState(); }

	private:
		Value m_kLuaVal;
	};

	class Table
	{
	public:
		Table() : m_kLuaVal() {}
		Table(const Table &kRhs) : m_kLuaVal(kRhs.m_kLuaVal) {}
		Table(const Value &kLuaVal) : m_kLuaVal(kLuaVal) {}
		Table(const StackValue &kStackValue) : m_kLuaVal(kStackValue) {}
		Table(lua_State *L, int iStackIndex) : m_kLuaVal(L, iStackIndex) {}
		Table(lua_State *L, const char *szName) : m_kLuaVal(L, szName) {}
		explicit Table(lua_State *L) : m_kLuaVal(L, LUA_GLOBALSINDEX) {} // Globals table for lua state
		~Table() {}

		static Table Create(lua_State *L, unsigned int uiArrayReserve = 0, unsigned int uiNonArrayReserve = 0);

		inline void Swap(Table &kRhs) { m_kLuaVal.Swap(kRhs.m_kLuaVal); }

		inline const Table &operator =(const Table &kRhs) { m_kLuaVal = kRhs.m_kLuaVal; return *this; }
		inline const Table &operator =(const Value &kLuaValue) { m_kLuaVal = kLuaValue; return *this; }
		inline void Set(lua_State *L, int iStackIndex) { m_kLuaVal.Set(L, iStackIndex); }
		inline void Set(lua_State *L, const char *szName) { m_kLuaVal.Set(L, szName); }

		inline void Clear() { m_kLuaVal.Clear(); }

		// Warning: Only call if the lua state has already been destroyed
		inline void LuaStateDestroyed() { m_kLuaVal.LuaStateDestroyed(); }

		bool Valid() const;

		inline const bool operator ==(const Table &kRhs) const { return m_kLuaVal == kRhs.m_kLuaVal; }
		inline const bool operator !=(const Table &kRhs) const { return !(m_kLuaVal == kRhs.m_kLuaVal); }
		inline const bool operator ==(const Value &kRhs) const { return m_kLuaVal == kRhs; }
		inline const bool operator !=(const Value &kRhs) const { return !(m_kLuaVal == kRhs); }

		bool Push() const;

		Metatable GetMetatable() const;
		void SetMetatable(const Table &kMetaTable);

		bool UsesWeakKeys() const;
		bool UsesWeakValues() const;
		inline void UseWeakKeys(bool bUseWeakKeys = true) { SetMode(bUseWeakKeys, UsesWeakValues()); }
		inline void UseWeakValues(bool bUseWeakValues = true) { SetMode(UsesWeakKeys(), bUseWeakValues); }

		_Ret_opt_z_ const char *GetMode() const;
		void SetMode( _In_z_ const char *szMode);
		void SetMode(bool bWeakKeys, bool bWeakValues);

		inline const Value &GetLuaValue() const { return m_kLuaVal; }
		inline lua_State *GetLuaState() const { return m_kLuaVal.GetLuaState(); }

		// WARNING: LuaValue owns this lua ref!
		inline int GetRef() const { return m_kLuaVal.GetRef(); }

		// Represents a field for the [const char*] operator
		class Field {
		private:
			friend Table;
			template<class T> Field(const Table &kTable, T key) : m_kTable((Table&)kTable) {
				typedef Details::PushAnalog<T>::Result KeyAnalog;
				lua_State *L = kTable.GetLuaState();
				if( L != NULL )
				{
					KeyAnalog keyAnalog = Details::ToLuaAnalog<KeyAnalog>::Convert(key);
					lua_checkstack(L, 1);
					Details::Push(L, keyAnalog); // Push the key
					m_kKey.Set(L, -1);	// Ref the key
					lua_pop(L, 1); // Pop the key
				}
			}

		public:
			Field(const Field &kRhs) : m_kTable(kRhs.m_kTable), m_kKey(kRhs.m_kKey) {}
			const Field &operator=(const Field &kRhs);

			Type GetType() const { return m_kTable.GetField(m_kKey).GetType(); }
			_Ret_z_ const char *GetTypeName() const { return m_kTable.GetField(m_kKey).GetTypeName(); }
			bool Exists() const { return !m_kTable.GetField(m_kKey).IsNil(); }
			inline FLua::String ToString() const { return m_kTable.GetField(m_kKey).ToString(); }
			template<class T> bool IsOfType() const { return m_kTable.GetField(m_kKey).IsOfType<T>(); }
			template<class T> operator T() const { return m_kTable.GetField(m_kKey); }
			template<class T> inline const Field &operator =(T val) { m_kTable.SetField<T>(m_kKey, val); return *this; }

			inline const bool operator ==(const Field &kRhs) const { return kRhs.m_kTable.GetField(kRhs.m_kKey) == m_kTable.GetField(m_kKey); }
			inline const bool operator !=(const Field &kRhs) const { return kRhs.m_kTable.GetField(kRhs.m_kKey) != m_kTable.GetField(m_kKey); }

		private:
			Table &m_kTable;
			Value m_kKey;
		};

		// [] operators
		template<class T> inline Field operator [](T key) { return Field(*this, key); }
		template<class T> inline const Field operator [](T key) const { return Field(*this, key); }

		// GetField - Get the value for a given field
		const Value GetField( _In_z_ const char *szField) const;
		const Value GetField(int iIndex) const;
		const Value GetField(const Value &kKey) const;
		bool PushField(const Value &kKey) const;

		// SetField - Assign a value to a field
		template<class T>
		void SetField( _In_z_ const char *szField, T val) {
			if( Push() ) {
				lua_State *L = m_kLuaVal.GetLuaState();
				typedef Details::PushAnalog<T>::Result Analog;
				Analog analog = Details::ToLuaAnalog<Analog>::Convert(val);
				lua_checkstack(L, 1);
				Details::Push(L, analog);
				lua_setfield(L, -2, szField);
				lua_pop(L, 1); // Pop the table
			}
		}

		template<class T>
		void SetField(int iIndex, T val) {
			if( Push() ) {
				lua_State *L = m_kLuaVal.GetLuaState();
				typedef Details::PushAnalog<T>::Result Analog;
				Analog analog = Details::ToLuaAnalog<Analog>::Convert(val);
				lua_checkstack(L, 1);
				Details::Push(L, analog);
				lua_rawseti(L, -2, iIndex);
				lua_pop(L, 1); // Pop the table
			}
		}

		template<class T>
		void SetField(const Value &kKey, T val) {
			lua_State *L = m_kLuaVal.GetLuaState();
			if( L != kKey.GetLuaState() ) {
				Details::Error("Key sent to FLua::Table::SetField belongs to a different lua state");
			}
			else if( Push() ) {
				if( kKey.Push() ) {
					if( lua_isnil(L, -1) )
					{
						lua_pop(L, 1); // Pop the nil key
						Details::Error("nil key sent to FLua::Table::SetField");
					}
					else
					{
						typedef Details::PushAnalog<T>::Result Analog;
						Analog analog = Details::ToLuaAnalog<Analog>::Convert(val);
						lua_checkstack(L, 1);
						Details::Push(L, analog); // Push the value
						lua_rawset(L, -3); // Set the field (pops key and value)
					}
				}
				else Details::Error("Invalid key sent to FLua::Table::SetField");
				lua_pop(L, 1); // Pop the table
			}
		}

		// SetFieldToNil - nil out a field
		void SetFieldToNil( _In_z_ const char *szField);
		void SetFieldToNil(int iIndex);

        template<class TFunc>
        inline void AddMemberFunction( _In_z_ const char *szName, TFunc pfn)
        {
            SetField(szName, FLua::Value::FromCFunction(GetLuaState(), pfn, szName));
        }

		template<class TIterator>
		void Insert(const TIterator &first, const TIterator &last)
		{
			for( TIterator itr = first; itr != last; ++itr )
				(*this)[itr->first] = itr->second;
		}

		class iterator
		{
			friend Table;

			struct Pos
			{
				Value first; // Key
				Value second; // Value
			};

		public:
			iterator();
			iterator(const iterator &kRhs);

			const iterator &operator =(const iterator &kRhs);

			const bool operator ==(const iterator &kRhs) const;
			inline const bool operator !=(const iterator &kRhs) const { return !(operator ==(kRhs)); }

			inline const iterator &operator++() { Advance(); return *this; }
			inline iterator operator++(int) { iterator kTemp(*this); Advance(); return kTemp; }

			inline const Pos *operator *() const { return &m_kPos; }
			inline const Pos *operator ->() const { return &m_kPos; }

		private:
			void Advance();

			const Table *m_pkTable;
			Pos m_kPos;
		};

		iterator begin() const;
		iterator end() const;

	private:
		Value m_kLuaVal;
	};

	class Metatable : public Table
	{
	public:
		Metatable(const Value &kLuaValue); // Get the metatable for a FLua::Value
		Metatable(lua_State *L, int iStackIndex); // Get the metatable for the value at iStackIndex 
		Metatable(lua_State *L, _In_z_ const char *szName); // Get the metatable for a global
	};

	class Function
	{
	public:
		Function() : m_kLuaVal() {}
		Function(const Function &kRhs) : m_kLuaVal(kRhs.m_kLuaVal) {}
		Function(const Value &kLuaVal) : m_kLuaVal(kLuaVal) {}
		Function(const StackValue &kStackValue) : m_kLuaVal(kStackValue) {}
		Function(lua_State *L, int iStackIndex) : m_kLuaVal(L, iStackIndex) {}
		Function(lua_State *L, _In_z_ const char *szName) : m_kLuaVal(L, szName) {}
		~Function() {}

		static Function Create(lua_State *L, _In_z_ const char *szCode);

		inline void Swap(Function &kRhs) { m_kLuaVal.Swap(kRhs.m_kLuaVal); }

		inline const Function &operator =(const Function &kRhs) { m_kLuaVal = kRhs.m_kLuaVal; return *this; }
		inline const Function &operator =(const Value &kLuaValue) { m_kLuaVal = kLuaValue; return *this; }
		inline void Set(lua_State *L, int iStackIndex) { m_kLuaVal.Set(L, iStackIndex); }
		inline void Set(lua_State *L, _In_z_ const char *szName) { m_kLuaVal.Set(L, szName); }

		inline void Clear() { m_kLuaVal.Clear(); }

		// Warning: Only call if the lua state has already been destroyed
		inline void LuaStateDestroyed() { m_kLuaVal.LuaStateDestroyed(); }

		bool Valid() const;

		inline const bool operator ==(const Function &kRhs) const { return m_kLuaVal == kRhs.m_kLuaVal; }
		inline const bool operator !=(const Function &kRhs) const { return !(m_kLuaVal == kRhs.m_kLuaVal); }
		inline const bool operator ==(const Value &kRhs) const { return m_kLuaVal == kRhs; }
		inline const bool operator !=(const Value &kRhs) const { return !(m_kLuaVal == kRhs); }

		bool Push() const;

		inline const Value &GetLuaValue() const { return m_kLuaVal; }
		inline lua_State *GetLuaState() const { return m_kLuaVal.GetLuaState(); }

		// WARNING: LuaValue owns this lua ref!
		inline int GetRef() const { return m_kLuaVal.GetRef(); }

		// 0 args
		Value operator()() const {
			Value kRetVal;
			Details::LockAccess();
			if( Push() ) {
				lua_State *L = GetLuaState();
				if( Details::CallWithErrorHandling(L, 0, 1) ) {
					kRetVal.Set(L, -1); // Get the return value
					lua_pop(L, 1); // Clean up the stack
				}
			}
			else Details::Error(_T("Attempt to call a FLua::Function with an invalid function"));
			Details::UnlockAccess();
			return kRetVal;
		}

		template<class TRet>
		TRet CallAsDelegate() const { return (TRet)operator()(); }

		// 1 arg
		template <class TArg0>
		Value operator()(TArg0 a0) const {
			Value kRetVal;
			Details::LockAccess();
			if( Push() ) {
				typedef Details::PushAnalog<TArg0>::Result Analog0;
				Analog0 arg0 = Details::ToLuaAnalog<Analog0>::Convert(a0);

				lua_State *L = GetLuaState();
				lua_checkstack(L, 1);
				Details::Push(L, arg0);

				if( Details::CallWithErrorHandling(L, 1, 1) ) {
					kRetVal.Set(L, -1);
					lua_pop(L, 1);
				}
			}
			else Details::Error(_T("Attempt to call a FLua::Function with an invalid function"));
			Details::UnlockAccess();
			return kRetVal;
		}

		template<class TRet, class TArg0>
		TRet CallAsDelegate(TArg0 a0) const { return (TRet)operator()<TArg0>(a0); }

		// 2 args
		template <class TArg0, class TArg1>
		Value operator()(TArg0 a0, TArg1 a1) const {
			Value kRetVal;
			Details::LockAccess();
			if( Push() ) {
				typedef Details::PushAnalog<TArg0>::Result Analog0;
				typedef Details::PushAnalog<TArg1>::Result Analog1;

				Analog0 arg0 = Details::ToLuaAnalog<Analog0>::Convert(a0);
				Analog1 arg1 = Details::ToLuaAnalog<Analog1>::Convert(a1);

				lua_State *L = GetLuaState();
				lua_checkstack(L, 2);
				Details::Push(L, arg0);
				Details::Push(L, arg1);

				if( Details::CallWithErrorHandling(L, 2, 1) ) {
					kRetVal.Set(L, -1);
					lua_pop(L, 1);
				}
			}
			else Details::Error(_T("Attempt to call a FLua::Function with an invalid function"));
			Details::UnlockAccess();
			return kRetVal;
		}

		template<class TRet, class TArg0, class TArg1>
		TRet CallAsDelegate(TArg0 a0, TArg1 a1) const { return (TRet)operator()<TArg0, TArg1>(a0, a1); }

		// 3 args
		template <class TArg0, class TArg1, class TArg2>
		Value operator()(TArg0 a0, TArg1 a1, TArg2 a2) const {
			Value kRetVal;
			Details::LockAccess();
			if( Push() ) {
				typedef Details::PushAnalog<TArg0>::Result Analog0;
				typedef Details::PushAnalog<TArg1>::Result Analog1;
				typedef Details::PushAnalog<TArg2>::Result Analog2;

				Analog0 arg0 = Details::ToLuaAnalog<Analog0>::Convert(a0);
				Analog1 arg1 = Details::ToLuaAnalog<Analog1>::Convert(a1);
				Analog2 arg2 = Details::ToLuaAnalog<Analog2>::Convert(a2);

				lua_State *L = GetLuaState();
				lua_checkstack(L, 3);
				Details::Push(L, arg0);
				Details::Push(L, arg1);
				Details::Push(L, arg2);

				if( Details::CallWithErrorHandling(L, 3, 1) ) {
					kRetVal.Set(L, -1);
					lua_pop(L, 1);
				}
			}
			else Details::Error(_T("Attempt to call a FLua::Function with an invalid function"));
			Details::UnlockAccess();
			return kRetVal;
		}

		template<class TRet, class TArg0, class TArg1, class TArg2>
		TRet CallAsDelegate(TArg0 a0, TArg1 a1, TArg2 a2) const { return (TRet)operator()<TArg0, TArg1, TArg2>(a0, a1, a2); }

		// 4 args
		template <class TArg0, class TArg1, class TArg2, class TArg3>
		Value operator()(TArg0 a0, TArg1 a1, TArg2 a2, TArg3 a3) const {
			Value kRetVal;
			Details::LockAccess();
			if( Push() ) {
				typedef Details::PushAnalog<TArg0>::Result Analog0;
				typedef Details::PushAnalog<TArg1>::Result Analog1;
				typedef Details::PushAnalog<TArg2>::Result Analog2;
				typedef Details::PushAnalog<TArg3>::Result Analog3;

				Analog0 arg0 = Details::ToLuaAnalog<Analog0>::Convert(a0);
				Analog1 arg1 = Details::ToLuaAnalog<Analog1>::Convert(a1);
				Analog2 arg2 = Details::ToLuaAnalog<Analog2>::Convert(a2);
				Analog3 arg3 = Details::ToLuaAnalog<Analog3>::Convert(a3);

				lua_State *L = GetLuaState();
				lua_checkstack(L, 4);
				Details::Push(L, arg0);
				Details::Push(L, arg1);
				Details::Push(L, arg2);
				Details::Push(L, arg3);

				if( Details::CallWithErrorHandling(L, 4, 1) ) {
					kRetVal.Set(L, -1);
					lua_pop(L, 1);
				}
			}
			else Details::Error(_T("Attempt to call a FLua::Function with an invalid function"));
			Details::UnlockAccess();
			return kRetVal;
		}

		template<class TRet, class TArg0, class TArg1, class TArg2, class TArg3>
		TRet CallAsDelegate(TArg0 a0, TArg1 a1, TArg2 a2, TArg3 a3) const {
			return (TRet)operator()<TArg0, TArg1, TArg2, TArg3>(a0, a1, a2, a3);
		}

		// 5 args
		template <class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		Value operator()(TArg0 a0, TArg1 a1, TArg2 a2, TArg3 a3, TArg4 a4) const {
			Value kRetVal;
			Details::LockAccess();
			if( Push() ) {
				typedef Details::PushAnalog<TArg0>::Result Analog0;
				typedef Details::PushAnalog<TArg1>::Result Analog1;
				typedef Details::PushAnalog<TArg2>::Result Analog2;
				typedef Details::PushAnalog<TArg3>::Result Analog3;
				typedef Details::PushAnalog<TArg4>::Result Analog4;

				Analog0 arg0 = Details::ToLuaAnalog<Analog0>::Convert(a0);
				Analog1 arg1 = Details::ToLuaAnalog<Analog1>::Convert(a1);
				Analog2 arg2 = Details::ToLuaAnalog<Analog2>::Convert(a2);
				Analog3 arg3 = Details::ToLuaAnalog<Analog3>::Convert(a3);
				Analog4 arg4 = Details::ToLuaAnalog<Analog4>::Convert(a4);

				lua_State *L = GetLuaState();
				lua_checkstack(L, 5);
				Details::Push(L, arg0);
				Details::Push(L, arg1);
				Details::Push(L, arg2);
				Details::Push(L, arg3);
				Details::Push(L, arg4);

				if( Details::CallWithErrorHandling(L, 5, 1) ) {
					kRetVal.Set(L, -1);
					lua_pop(L, 1);
				}
			}
			else Details::Error(_T("Attempt to call a FLua::Function with an invalid function"));
			Details::UnlockAccess();
			return kRetVal;
		}

		template<class TRet, class TArg0, class TArg1, class TArg2, class TArg3, class TArg4>
		TRet CallAsDelegate(TArg0 a0, TArg1 a1, TArg2 a2, TArg3 a3, TArg4 a4) const {
			return (TRet)operator()<TArg0, TArg1, TArg2, TArg3, TArg4>(a0, a1, a2, a3, a4);
		}

	private:
		Value m_kLuaVal;
	};

	// Stack manipulation for lua types
	namespace Details
	{
		// For Values
		template<> struct LuaAnalog<Value>{ typedef StackValue Result; };
		template<> struct LuaAnalog<Value&>{ typedef Value Result; };
		template<> struct LuaAnalog<const Value&>{ typedef StackValue Result; };

		template<> struct PushAnalog<Value>{ typedef Value Result; };
		template<> struct PushAnalog<const Value>{ typedef Value Result; };
		template<> struct PushAnalog<Value&>{ typedef Value Result; };
		template<> struct PushAnalog<const Value&>{ typedef Value Result; };

		template<> static const char *DescribeType<Value>() { return "variant"; }

		template<> static inline Value Get(lua_State *L, int idx) { return Value(L, idx); }
		static inline void Push(lua_State *L, const Value &kVal) {
			lua_State *pkValState = kVal.GetLuaState();
			if( pkValState == NULL )
			{
				lua_pushnil(L);
			}
			else if( pkValState != L )
			{
				if( SharedLuaRegistry::Get() )
				{
					lua_checkstack(L, 1);
					int iRef = kVal.GetRef();
					if( iRef == LUA_NOREF )
						lua_pushnil(L);
					else
						lua_getref(L, iRef);
				}
				else
				{
					Error(_T("Can't push lua value.  It belongs to a dfferent lua state"));
					lua_checkstack(L, 1);
					lua_pushnil(L);
				}
			}
			else if( !kVal.Push() )
			{
				lua_checkstack(L, 1);
				lua_pushnil(L);
			}
		}

		// For Strings
		template<> struct LuaAnalog<String>{ typedef StackValue Result; };
		template<> struct LuaAnalog<String&>{ typedef String Result; };
		template<> struct LuaAnalog<const String&>{ typedef StackValue Result; };

		template<> struct PushAnalog<String>{ typedef String Result; };
		template<> struct PushAnalog<const String>{ typedef String Result; };
		template<> struct PushAnalog<String&>{ typedef String Result; };
		template<> struct PushAnalog<const String&>{ typedef String Result; };

		template<> static const char *DescribeType<String>() { return "string"; }

		template<> static inline String Get(lua_State *L, int idx) { return String(L, idx); }
		static inline void Push(lua_State *L, const String &sVal) {
			lua_State *pkValState = sVal.GetLuaState();
			if( pkValState != L )
			{
				lua_checkstack(L, 1);
				lua_pushstring(L, sVal);
			}
			else if( !sVal.Push() )
			{
				lua_checkstack(L, 1);
				lua_pushstring(L, "");
			}
		}

		// For Functions
		template<> struct LuaAnalog<Function>{ typedef StackValue Result; };
		template<> struct LuaAnalog<Function&>{ typedef Function Result; };
		template<> struct LuaAnalog<const Function&>{ typedef StackValue Result; };

		template<> struct PushAnalog<Function>{ typedef Function Result; };
		template<> struct PushAnalog<const Function>{ typedef Function Result; };
		template<> struct PushAnalog<Function&>{ typedef Function Result; };
		template<> struct PushAnalog<const Function&>{ typedef Function Result; };

		template<> static const char *DescribeType<Function>() { return "function"; }

		template<> static inline Function Get(lua_State *L, int idx) { return Function(L, idx); }
		static inline void Push(lua_State *L, const Function &kVal) {
			if( kVal.GetLuaState() != L )
			{
				if( SharedLuaRegistry::Get() )
				{
					lua_checkstack(L, 1);
					int iRef = kVal.GetRef();
					if( iRef == LUA_NOREF )
						lua_pushnil(L);
					else
						lua_getref(L, iRef);
				}
				else
				{
					Error(_T("Can't push function.  It belongs to a dfferent lua state"));
					lua_checkstack(L, 1);
					lua_pushnil(L);
				}
			}
			else if( !kVal.Push() )
			{
				lua_checkstack(L, 1);
				lua_pushnil(L);
			}
		}

		template<>
		struct ArgValidator<Function>{
			static inline void Validate(StackValue &arg, bool &bValid) {
				if( arg.GetType() != TYPE_FUNCTION ) {
					// TODO: Figure out what to do about potentially needing to convert typename string to wide char
					Error(_T("Bad argument: Expected function but got %s"), arg.GetTypeName());
					bValid = false;
				}
			}
		};

		template<> struct ArgValidator<const Function&> : public ArgValidator<Function> {};

		template<>
		struct ArgValidator<Function&>{
			static inline void Validate(Function &arg, bool &bValid) {
				if( !arg.Valid() ) {
					// TODO: Figure out what to do about potentially needing to convert typename string to wide char
					Error(_T("Bad argument: Expected function but got %s"), arg.GetLuaValue().GetTypeName());
					bValid = false;
				}
			}
		};

		// For Tables
		template<> struct LuaAnalog<Table>{ typedef StackValue Result; };
		template<> struct LuaAnalog<Table&>{ typedef Table Result; };
		template<> struct LuaAnalog<const Table&>{ typedef StackValue Result; };

		template<> struct PushAnalog<Table>{ typedef Table Result; };
		template<> struct PushAnalog<const Table>{ typedef Table Result; };
		template<> struct PushAnalog<Table&>{ typedef Table Result; };
		template<> struct PushAnalog<const Table&>{ typedef Table Result; };

		template<> static const char *DescribeType<Table>() { return "table"; }

		static inline void Push(lua_State *L, const Table &kVal) {
			lua_State *pkValState = kVal.GetLuaState();
			if( pkValState == NULL )
			{
				lua_checkstack(L, 1);
				lua_pushnil(L);
			}
			else if( pkValState != L )
			{
				if( SharedLuaRegistry::Get() )
				{
					lua_checkstack(L, 1);
					int iRef = kVal.GetRef();
					if( iRef == LUA_NOREF )
						lua_pushnil(L);
					else
						lua_getref(L, iRef);
				}
				else
				{
					Error(_T("Can't push table.  It belongs to a dfferent lua state"));
					lua_checkstack(L, 1);
					lua_pushnil(L);
				}
			}
			else if( !kVal.Push() )
			{
				lua_checkstack(L, 1);
				lua_pushnil(L);
			}
		}

		// For Table Fields
		template<> struct PushAnalog<Table::Field>{ typedef Value Result; };
		template<> struct PushAnalog<const Table::Field>{ typedef Value Result; };
		template<> struct PushAnalog<Table::Field&>{ typedef Value Result; };
		template<> struct PushAnalog<const Table::Field&>{ typedef Value Result; };
	}
}

#endif
