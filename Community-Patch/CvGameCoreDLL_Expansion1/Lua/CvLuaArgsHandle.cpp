/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaArgsHandle.cpp
//!  \brief     Private implementation of CvLuaArgsHandle
//!
//!		This file includes a basic utility for managing the scope of
//!		script system arguments passed to the exe.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "CvGameCoreDLLPCH.h"
#include "CvLuaArgsHandle.h"

CvLuaArgsHandle::CvLuaArgsHandle()
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		m_Ptr = pkScriptSystem->CreateArgs();
	}
}
CvLuaArgsHandle::CvLuaArgsHandle(uint uiReserve)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		m_Ptr = pkScriptSystem->CreateArgs(uiReserve);
	}
}
//------------------------------------------------------------------------------
CvLuaArgsHandle::~CvLuaArgsHandle()
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		pkScriptSystem->DestroyArgs(m_Ptr);
	}
}
