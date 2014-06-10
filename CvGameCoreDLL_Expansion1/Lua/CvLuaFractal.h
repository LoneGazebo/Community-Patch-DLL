/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaFractal.h
//!  \brief     Public interface to CvLuaFractal.
//!
//!		This file includes the interface for exposing CvFractal to Lua.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once

class CvFractal;

class CvLuaFractal
{
public:
	//! Registers 'Fractal' with Lua.
	static void Register(lua_State* L);

protected:

	//! Protected call to register 'Fractal'.
	static int pRegister(lua_State* L);

	//! Utility for generating a CvFractal instance.
	static CvFractal* CreateFractal(lua_State* L);

	//! Creates/initializes a fractal using standard params.
	static int lCreate(lua_State* L);

	//! Creates/initializes a fractal using another instance for rifts.
	static int lCreateRifts(lua_State* L);

	//! Lua equivilant of CvFractal::RidgeBuilder.
	static int lBuildRidges(lua_State* L);

	//! GC for Fractal instance
	static int lDestroy(lua_State* L);

	//! Lua equivilant of GetHeight and GetHeightFromPercent.
	static int lGetHeight(lua_State* L);
};