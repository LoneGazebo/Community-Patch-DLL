/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
//
//  PURPOSE: Public definition of CvGameTextMgr.
//
#pragma once
#ifndef CIV5_GAME_TEXT_MGR_H
#define CIV5_GAME_TEXT_MGR_H

#pragma warning( disable: 4251 )	// needs to have dll-interface to be used by clients of class

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	CvGameTextMgr
//		A suite of static methods used to generate common text sequences.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameTextMgr
{
public:

	static void setYearStr(CvString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
	static void setDateStr(CvString& szString, int iGameTurn, bool bSave, CalendarTypes eCalendar, int iStartYear, GameSpeedTypes eSpeed);
};

#endif	//CIV5_GAME_TEXT_MGR_H
