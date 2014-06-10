/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#ifndef CVACHIEVEMENTUNLOCKER_H
#define CVACHIEVEMENTUNLOCKER_H

//	Utility methods to test the parameters for individual achievements.
//	Most of the tests in this file are for achievements that have multiple parameters
//	that can be achieved in any order.  This keeps the code in a single place.
//	It is not strictly necessary to put all achievement tests in this file especially if the
//	test requires many parameters or is adequately tested in a single location in the game core.

class CvAchievementUnlocker
{
public:
	static bool Check_PSG();
	static void FarmImprovementPillaged();
	static void EndTurn();
	static void AlexanderConquest(PlayerTypes ePlayer);
private:
	static int ms_iNumImprovementsPillagedPerTurn;

};

#endif // CVACHIEVEMENTUNLOCKER_H