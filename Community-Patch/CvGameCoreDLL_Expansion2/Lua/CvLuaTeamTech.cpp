/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvLuaTeamTech.cpp
//!  \brief     Private implementation to CvLuaTeamTech.
//!
//!		This file includes the implementation for a Lua TeamTech instance.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <CvGameCoreDLLPCH.h>
#include "CvLuaSupport.h"
#include "CvLuaTeamTech.h"

//Utility macro for registering methods
#define Method(Name)			\
	lua_pushcclosure(L, l##Name, 0);	\
	lua_setfield(L, t, #Name);

//------------------------------------------------------------------------------
void CvLuaTeamTech::HandleMissingInstance(lua_State* L)
{
	DefaultHandleMissingInstance(L);
}
//------------------------------------------------------------------------------
void CvLuaTeamTech::PushMethods(lua_State* L, int t)
{
	Method(SetHasTech);
	Method(HasTech);

	Method(GetNumTechsKnown);
	Method(HasResearchedAllTechs);

	Method(GetLastTechAcquired);

	Method(SetNoTradeTech);
	Method(IsNoTradeTech);
	Method(IncrementTechCount);
	Method(GetTechCount);
	Method(SetResearchProgress);
	Method(SetResearchProgressTimes100);
	Method(GetResearchProgress);
	Method(GetResearchProgressTimes100);
	Method(ChangeResearchProgress);
	Method(ChangeResearchProgressTimes100);
	Method(ChangeResearchProgressPercent);
	Method(GetResearchCost);
	Method(GetResearchLeft);
}
//------------------------------------------------------------------------------
const char* CvLuaTeamTech::GetTypeName()
{
	return "TeamTech";
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Lua Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//void SetHasTech(TechTypes eIndex, bool bNewValue);
int CvLuaTeamTech::lSetHasTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::SetHasTech);
}

//------------------------------------------------------------------------------
//bool HasTech(TechTypes eIndex);
int CvLuaTeamTech::lHasTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::HasTech);
}

//------------------------------------------------------------------------------
//bool GetNumTechsKnown(TechTypes eIndex);
int CvLuaTeamTech::lGetNumTechsKnown(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetNumTechsKnown);
}

//------------------------------------------------------------------------------
//bool HasResearchedAllTechs();
int CvLuaTeamTech::lHasResearchedAllTechs(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::HasResearchedAllTechs);
}

//------------------------------------------------------------------------------
//bool GetLastTechAcquired(TechTypes eIndex);
int CvLuaTeamTech::lGetLastTechAcquired(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetLastTechAcquired);
}

//------------------------------------------------------------------------------
//void SetNoTradeTech(TechTypes eIndex, bool bNewValue);
int CvLuaTeamTech::lSetNoTradeTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::SetNoTradeTech);
}

//------------------------------------------------------------------------------
//bool IsNoTradeTech(TechTypes eIndex);
int CvLuaTeamTech::lIsNoTradeTech(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::IsNoTradeTech);
}

//------------------------------------------------------------------------------
//void IncrementTechCount(TechTypes eIndex);
int CvLuaTeamTech::lIncrementTechCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::IncrementTechCount);
}

//------------------------------------------------------------------------------
//int GetTechCount(TechTypes eIndex);
int CvLuaTeamTech::lGetTechCount(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetTechCount);
}

//------------------------------------------------------------------------------
//void SetResearchProgress(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer);
int CvLuaTeamTech::lSetResearchProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::SetResearchProgress);
}

//------------------------------------------------------------------------------
//void SetResearchProgressTimes100(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer);
int CvLuaTeamTech::lSetResearchProgressTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::SetResearchProgressTimes100);
}

//------------------------------------------------------------------------------
//int GetResearchProgress(TechTypes eIndex);
int CvLuaTeamTech::lGetResearchProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetResearchProgress);
}

//------------------------------------------------------------------------------
//int GetResearchProgressTimes100 (TechTypes eIndex);
int CvLuaTeamTech::lGetResearchProgressTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetResearchProgressTimes100);
}

//------------------------------------------------------------------------------
//void ChangeResearchProgress(TechTypes eIndex, int iChange, PlayerTypes ePlayer);
int CvLuaTeamTech::lChangeResearchProgress(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::ChangeResearchProgress);
}

//------------------------------------------------------------------------------
//void ChangeResearchProgressTimes100(TechTypes eIndex, int iChange, PlayerTypes ePlayer);
int CvLuaTeamTech::lChangeResearchProgressTimes100(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::ChangeResearchProgressTimes100);
}

//------------------------------------------------------------------------------
//int ChangeResearchProgressPercent(TechTypes eIndex, int iPercent, PlayerTypes ePlayer);
int CvLuaTeamTech::lChangeResearchProgressPercent(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::ChangeResearchProgressPercent);
}

//------------------------------------------------------------------------------
//int GetResearchCost(TechTypes eTech);
int CvLuaTeamTech::lGetResearchCost(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetResearchCost);
}

//------------------------------------------------------------------------------
//int GetResearchLeft(TechTypes eTech);
int CvLuaTeamTech::lGetResearchLeft(lua_State* L)
{
	return BasicLuaMethod(L, &CvTeamTechs::GetResearchLeft);
}
