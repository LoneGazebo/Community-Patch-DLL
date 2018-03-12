/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

class FDataStream;
struct CvPopupInfo;

FDataStream& operator <<(FDataStream& kStream, const CvPopupInfo& kPopupInfo);
FDataStream& operator >>(FDataStream& kStream, CvPopupInfo& kPopupInfo);
