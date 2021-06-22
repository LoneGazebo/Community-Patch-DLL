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

template<typename PopupInfo, typename Visitor>
static void SerializePopupInfo(PopupInfo& popupInfo, Visitor& visitor)
{
	visitor(popupInfo.iData1);
	visitor(popupInfo.iData2);
	visitor(popupInfo.iData3);
	visitor(popupInfo.iFlags);
	visitor(popupInfo.bOption1);
	visitor(popupInfo.bOption2);

	visitor(popupInfo.eButtonPopupType);

	visitor(popupInfo.szText);
}

FDataStream& operator<<(FDataStream& kStream, const CvPopupInfo& kPopupInfo)
{
	CvStreamSaveVisitor serialVisitor(kStream);
	SerializePopupInfo(kPopupInfo, serialVisitor);
	return kStream;
}
//------------------------------------------------------------------------------
FDataStream& operator>>(FDataStream& kStream, CvPopupInfo& kPopupInfo)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	SerializePopupInfo(kPopupInfo, serialVisitor);
	return kStream;
}
