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

/*
	Class dedicated to handling player specific achievements.
	Currently supports the following achievements:
     * ACHIEVEMENT_XP1_29 - As Carthage, Attack a Roman Unit with an African Forest elephant from a mountain tile
	 * ACHIEVEMENT_XP1_27 - Select the Papal Primacy Belief and Ally with 12 city states
	 * ACHIEVEMENT_XP1_31 - As Sweden, Start a turn with your great general stacked with a Hakkapelitta
	 * ACHIEVEMENT_XP1_32 - As Ethiopia, earn 5 great prophets
	 * ACHIEVEMENT_XP1_33 - As Byzantines, sink 10 Greek ships with a Dromon

*/
class CvPlayerAchievements
{
public:
	CvPlayerAchievements(const CvPlayer& kPlayer);

	//Design Note:
	//Try to keep these methods very generic and almost representative of event handlers.
	//These methods may increment the progress of multiple achievements.
	void AlliedWithCityState(PlayerTypes eCityStateAlly);
	void AddUnit(CvUnit* pUnit);
	void AttackedUnitWithUnit(CvUnit* pAttackingUnit, CvUnit* pDefendingUnit);
	void BoughtCityState(int iNumUnits);
	void KilledUnitWithUnit(CvUnit* pKillingUnit, CvUnit* pKilledUnit);
	void StartTurn();
	void EndTurn();

	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

private:
	const CvPlayer& m_kPlayer;

	int m_iAchievement_XP1_32_Progress;
	int m_iAchievement_XP1_33_Progress;

	//Cached types to save from querying the database every check.
	BeliefTypes m_ePapalPrimacyType;
	CivilizationTypes m_eEthiopiaType;
	CivilizationTypes m_eByzantinesType;
	CivilizationTypes m_eGreekType;
	CivilizationTypes m_eCarthageType;
	CivilizationTypes m_eRomeType;
	CivilizationTypes m_eSwedenType;
	UnitTypes m_eDromonType;
	UnitTypes m_eGreatProphetType;
	UnitTypes m_eAfricanForestElephantType;
	UnitTypes m_eGreatGeneralType;
	UnitTypes m_eHakkapelittaType;
	UnitTypes m_eSkyFortress;
};

#endif // CVACHIEVEMENTUNLOCKER_H