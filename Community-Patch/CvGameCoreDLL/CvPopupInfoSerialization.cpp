/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvPopupInfoSerialization.h"
#include "CvEnumSerialization.h"
#include "FStlContainerSerialization.h"

// Include this after all other headers.
#include "LintFree.h"

FDataStream& operator<<(FDataStream& kStream, const CvPopupInfo& kPopupInfo)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;

	kStream << kPopupInfo.iData1;
	kStream << kPopupInfo.iData2;
	kStream << kPopupInfo.iData3;
	kStream << kPopupInfo.iFlags;
	kStream << kPopupInfo.bOption1;
	kStream << kPopupInfo.bOption2;

	kStream << kPopupInfo.eButtonPopupType;

	CvString strDummy = kPopupInfo.szText;
	kStream << strDummy;

	return kStream;
}
//------------------------------------------------------------------------------
FDataStream & operator>>(FDataStream& kStream, CvPopupInfo& kPopupInfo)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	kStream >> kPopupInfo.iData1;
	kStream >> kPopupInfo.iData2;
	kStream >> kPopupInfo.iData3;
	kStream >> kPopupInfo.iFlags;
	kStream >> kPopupInfo.bOption1;
	kStream >> kPopupInfo.bOption2;

	kStream >> kPopupInfo.eButtonPopupType;

	CvString strDummy;
	kStream >> strDummy;

	strcpy_s(kPopupInfo.szText, strDummy.c_str());

	return kStream;
}
