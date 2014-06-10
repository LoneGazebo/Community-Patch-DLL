/*	-------------------------------------------------------------------------------------------------------
	© 1991-2013 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#ifndef CVTARGETING_H
#define CVTARGETING_H

#pragma once

class CvTargeting 
{
public:
	static bool CanSeeDisplacementPlot(int startX, int startY, int dx, int dy, int fromLevel);
};

#endif