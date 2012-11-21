/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//!	 \file		CvGameCoreDLL.h
//!  \brief     Public header of the Civ5 GameCore DLL.
//!
//!		This header includes all DLLExposed methods used by Civilization5.
//!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma once
#ifndef CvGameCoreDLL_h
#define CvGameCoreDLL_h

#include <CvGameCoreDLLUtil.h>
#include <CvGameDatabase.h>
#include "CvPoint.h"
#include "CvDefines.h"
#include "CvStructs.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvAStar.h"

//DLL Interfaces (Should be moved to CvGameCoreDLLUtil?)
#include "ICvDLLDatabaseUtility.h"


#endif //CvGameCoreDLL_h