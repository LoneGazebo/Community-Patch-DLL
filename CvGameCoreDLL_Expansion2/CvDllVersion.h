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
// {C3B2B6B3-439C-480b-8536-1CF39FCFC682}
// {83F34FF0-3D60-43e7-AEDB-82C79867EE3F}
static const GUID CIV5_XP2_DLL_GUID = 
{ 0x83f34ff0, 0x3d60, 0x43e7, { 0xae, 0xdb, 0x82, 0xc7, 0x98, 0x67, 0xee, 0x3f } };
#else
static const GUID CIV5_XP2_DLL_GUID = MOD_DLL_GUID;
#endif

static const char* CIV5_XP2_DLL_VERSION = "1.0.0";
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
