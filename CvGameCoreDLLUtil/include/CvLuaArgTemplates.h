
#pragma once 
#ifndef CVLUAARGTEMPLATES
#define CVLUAARGTEMPLATES

namespace CvLuaArgs
{
	//To
	template<typename T>
	inline T toValue(lua_State* L, int idx)
	{
		//This is pretty unsafe, but common.
		//Assume T is a poorly designed enum =(
		return (T)lua_tointeger(L, idx);
	}

	template<> inline int toValue(lua_State* L, int idx)
	{
		return lua_tointeger(L, idx);
	}

	template<> inline bool toValue(lua_State* L, int idx)
	{
		return lua_toboolean(L, idx) != 0;
	}

	//Push
	template<typename T>
	inline void pushValue(lua_State* L, T t)
	{
		//This is pretty unsafe, but common.
		//Assume T is a poorly designed enum =(
		lua_pushinteger(L, t);
	}

	template<>
	inline void pushValue(lua_State* L, int t)
	{
		lua_pushinteger(L, t);
	}

	template<>
	inline void pushValue(lua_State* L, bool t)
	{
		lua_pushboolean(L, t);
	}

}

#endif //CVLUAARGTEMPLATES