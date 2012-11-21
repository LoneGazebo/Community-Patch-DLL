/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaArgsHandle.h
//!  \brief     Public header of CvLuaArgsHandle
//!
//!		This file includes a basic utility for managing the scope of
//!		script system arguments passed to the exe.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once

#include "ICvDLLScriptSystem.h"

//This is a simple utility class to manage construction and destruction of ScriptSystemArguments (used by GameEvents).
class CvLuaArgsHandle
{
public:
	CvLuaArgsHandle();
	CvLuaArgsHandle(uint uiReserve);
	~CvLuaArgsHandle();

	ICvEngineScriptSystemArgs1* operator->() const;
	ICvEngineScriptSystemArgs1* get() const;

private:
	ICvEngineScriptSystemArgs1* m_Ptr;
};

//------------------------------------------------------------------------------
// inline members
//------------------------------------------------------------------------------
inline ICvEngineScriptSystemArgs1* CvLuaArgsHandle::operator->() const
{
	return m_Ptr;
}
//------------------------------------------------------------------------------
inline ICvEngineScriptSystemArgs1* CvLuaArgsHandle::get() const
{
	return m_Ptr;
}