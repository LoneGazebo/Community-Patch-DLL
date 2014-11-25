/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// WARNING! This must be unique for each DLL!
// If you are a modder and you are creating your own DLL from the original source, this must be changed to a unique GUID.
// Use the Visual Studio Create GUID option in the Tools menu to create a new GUID.
#if !defined(CUSTOM_MODS_H)
// {BF9BF7F0-E078-4d4e-8A3E-84712F85AA2B}
static const GUID CIV5_XP2_DLL_GUID = 
{ 0xbf9bf7f0, 0xe078, 0x4d4e, { 0x8a, 0x3e, 0x84, 0x71, 0x2f, 0x85, 0xaa, 0x2b } };
#else
static const GUID CIV5_XP2_DLL_GUID = MOD_DLL_GUID;
#endif

static const char* CIV5_XP2_DLL_VERSION = "1.0.0";
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------