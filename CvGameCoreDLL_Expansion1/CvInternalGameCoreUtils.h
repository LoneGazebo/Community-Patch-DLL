/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
// CvInternalGameCoreUtils.h
//
// CvGameCoreUtils.h consists of a set of utility functions used by both
// the DLL and the application.
// Because the application references the header file, this can cause some
// reference issues in which not all types are fully defined.
// Rather than hide the internal utilities inside of a #ifdef _DLL block,
// I'm storing them in here.
//
// ONLY DLL SOURCE SHOULD INCLUDE THIS FILE. DO NOT INCLUDE THIS IN
// ANOTHER HEADER OR A SOURCE OUTSIDE OF CvGameCore.
//////////////////////////////////////////////////////////////////////////
#pragma once

bool isWorldWonderClass(const CvBuildingClassInfo& kBuildingClass);
bool isTeamWonderClass(const CvBuildingClassInfo& kBuildingClass);
bool isNationalWonderClass(const CvBuildingClassInfo& kBuildingClass);
bool isLimitedWonderClass(const CvBuildingClassInfo& kBuildingClass);
int limitedWonderClassLimit(const CvBuildingClassInfo& kBuildingClass);
//------------------------------------------------------------------------------
int getWorldSizeMaxConscript(const CvPolicyEntry& kPolicy);
