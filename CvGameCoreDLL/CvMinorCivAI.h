/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_MINOR_CIV_AI_H
#define CIV5_MINOR_CIV_AI_H

#include <CvLocalization.h>

#define SAFE_ESTIMATE_NUM_XML_WIDGETS 100

#define QUEST_DEBUGGING false

enum MinorCivStatusTypes
{
	NO_MINOR_CIV_STATUS_TYPE = -1,

	MINOR_CIV_STATUS_NORMAL,
	MINOR_CIV_STATUS_ELEVATED,
	MINOR_CIV_STATUS_CRITICAL,

	NUM_MINOR_CIV_STATUS_TYPES,
};
FDataStream & operator<<(FDataStream &, const MinorCivStatusTypes &);
FDataStream & operator>>(FDataStream &, MinorCivStatusTypes &);

enum MinorCivPersonalityTypes
{
	NO_MINOR_CIV_PERSONALITY_TYPE = -1,

	MINOR_CIV_PERSONALITY_FRIENDLY,
	MINOR_CIV_PERSONALITY_NEUTRAL,
	MINOR_CIV_PERSONALITY_HOSTILE,
	MINOR_CIV_PERSONALITY_IRRATIONAL,

	NUM_MINOR_CIV_PERSONALITY_TYPES,
};
FDataStream & operator<<(FDataStream &, const MinorCivPersonalityTypes &);
FDataStream & operator>>(FDataStream &, MinorCivPersonalityTypes &);

enum MinorCivQuestTypes
{
	NO_MINOR_CIV_QUEST_TYPE = -1,

	MINOR_CIV_QUEST_ROUTE,
	MINOR_CIV_QUEST_KILL_CAMP,
	MINOR_CIV_QUEST_CONNECT_RESOURCE,
	MINOR_CIV_QUEST_CONSTRUCT_WONDER,
	MINOR_CIV_QUEST_GREAT_PERSON,
	MINOR_CIV_QUEST_KILL_CITY_STATE,
	MINOR_CIV_QUEST_FIND_PLAYER,
	MINOR_CIV_QUEST_FIND_NATURAL_WONDER,

	NUM_MINOR_CIV_QUEST_TYPES,
};
FDataStream & operator<<(FDataStream &, const MinorCivQuestTypes &);
FDataStream & operator>>(FDataStream &, MinorCivQuestTypes &);

class CvPlayer;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMinorCivAI
//!  \brief		Special Information relating to Minor Civs only
//
//!  Author:	Jon Shafer
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvPlayerState object within CvPlayer class
//!  - Should be one instance for each Minor Civ (right now there's one for EVERY Player though)
//!  - Accessed by any class that needs information relating to Minor Civs
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMinorCivAI
{
public:
	CvMinorCivAI(void);
	~CvMinorCivAI(void);
	void Init(CvPlayer *pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	CvPlayer* GetPlayer();

	MinorCivTypes GetMinorCivType() const;

	MinorCivPersonalityTypes GetPersonality() const;
	void DoPickPersonality();

	MinorCivTraitTypes GetTrait() const;

	// ******************************
	// Main functions
	// ******************************

	void DoTurn();

	void DoChangeAliveStatus(bool bAlive);

	void DoFirstContactWithMajor(TeamTypes eTeam);

	void DoTestEndWarsVSMinors(PlayerTypes eOldAlly, PlayerTypes eNewAlly);

	void DoTurnStatus();
	MinorCivStatusTypes GetStatus() const;

	void AddNotification(Localization::String strString, Localization::String strSummaryString, PlayerTypes ePlayer, int iX = -1, int iY = -1);
	void AddQuestNotification(Localization::String strString, Localization::String strSummaryString, PlayerTypes ePlayer, int iX = -1, int iY = -1);

	// ******************************
	// Fend off Barbarians
	// ******************************

	void DoTestBarbarianQuest();

	int GetNumThreateningBarbarians();
	void DoThreateningBarbKilled(PlayerTypes eKillingPlayer, int iX, int iY);

	int GetTurnsSinceThreatenedByBarbarians() const;
	void SetTurnsSinceThreatenedByBarbarians(int iValue);
	void ChangeTurnsSinceThreatenedByBarbarians(int iChange);

	bool IsPlayerCloseEnoughForBarbQuestAnnouncement(PlayerTypes eMajor);

	void DoLaunchBarbariansQuestForPlayerNotification(PlayerTypes ePlayer);

	// ******************************
	// War with Major Quest
	// ******************************

	void DoTestWarWithMajorQuest();

	void DoTestProxyWarNotification();

	void DoLaunchWarWithMajorQuest(TeamTypes eAttackingTeam);

	bool IsWarQuestWithAnyoneActive() const;
	bool IsWarQuestWithMajorActive(PlayerTypes eEnemy) const;
	void SetWarQuestWithMajorActive(PlayerTypes eEnemy, bool bValue);

	void DoUnitDeathWarQuestImplications(PlayerTypes eLosingPlayer, PlayerTypes eKillingPlayer);

	int GetNumEnemyUnitsLeftToKillByMajor(PlayerTypes eMajor, PlayerTypes eEnemy) const;
	void SetNumEnemyUnitsLeftToKillByMajor(PlayerTypes eMajor, PlayerTypes eEnemy, int iValue);
	void ChangeNumEnemyUnitsLeftToKillByMajor(PlayerTypes eMajor, PlayerTypes eEnemy, int iChange);

	void DoLaunchWarQuestForPlayerNotification(PlayerTypes ePlayer, PlayerTypes eEnemy);

	// ******************************
	// ***** Quests *****
	// ******************************

	void DoTurnQuests();

	int GetFirstPossibleTurnForQuests();

	void DoTestQuestStart(PlayerTypes ePlayer);
	void DoStartQuest(PlayerTypes ePlayer, MinorCivQuestTypes eQuest);
	void DoTestQuestComplete(PlayerTypes ePlayer);
	void DoTestQuestObsolete(PlayerTypes ePlayer);

	bool IsValidQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eQuest);

	MinorCivQuestTypes GetActiveQuestForPlayer(PlayerTypes ePlayer);
	void SetActiveQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eQuest);
	void EndActiveQuestForPlayer(PlayerTypes ePlayer);

	int GetPersonalityQuestBias(MinorCivQuestTypes eQuest);

	void DoSeedQuestCountdown(PlayerTypes ePlayer);
	int GetQuestCountdown(PlayerTypes ePlayer);
	void SetQuestCountdown(PlayerTypes ePlayer, int iValue);
	void ChangeQuestCountdown(PlayerTypes ePlayer, int iChange);

	// Specific Details for Quests:

	int GetQuestData1(PlayerTypes ePlayer) const;
	void SetQuestData1(PlayerTypes ePlayer, int iValue);
	int GetQuestData2(PlayerTypes ePlayer) const;
	void SetQuestData2(PlayerTypes ePlayer, int iValue);

	bool IsRouteConnectionEstablished(PlayerTypes eMajor) const;
	void SetRouteConnectionEstablished(PlayerTypes eMajor, bool bValue);

	CvPlot* GetBestNearbyCampToKill();

	ResourceTypes GetNearbyResourceForQuest(PlayerTypes ePlayer);

	BuildingTypes GetBestWonderForQuest(PlayerTypes ePlayer);

	UnitTypes GetBestGreatPersonForQuest(PlayerTypes ePlayer);

	PlayerTypes GetBestCityStateTarget();
	bool IsWantsMinorDead(PlayerTypes eMinor);

	PlayerTypes GetBestPlayerToFind(PlayerTypes ePlayer);

	bool IsGoodTimeForNaturalWonderQuest(PlayerTypes ePlayer);

	// ******************************
	// ***** Friendship *****
	// ******************************

	void DoFriendship();

	int GetFriendshipChangePerTurnTimes100(PlayerTypes ePlayer) const;

	int GetFriendshipWithMajorTimes100(PlayerTypes ePlayer) const;
	void SetFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iNum);
	void ChangeFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iChange, bool bFromQuest = false);
	int GetFriendshipWithMajor(PlayerTypes ePlayer) const;
	void SetFriendshipWithMajor(PlayerTypes ePlayer, int iNum);
	void ChangeFriendshipWithMajor(PlayerTypes ePlayer, int iChange, bool bFromQuest = false);

	int GetAngerFreeIntrusionCounter(PlayerTypes ePlayer) const;
	void SetAngerFreeIntrusionCounter(PlayerTypes ePlayer, int iNum);
	void ChangeAngerFreeIntrusionCounter(PlayerTypes ePlayer, int iChange);

	void DoUpdateAlliesResourceBonus(PlayerTypes eNewAlly, PlayerTypes eOldAlly);

	int GetMostFriendshipWithAnyMajor(PlayerTypes& eBestPlayer);
	PlayerTypes GetAlly() const;
	void SetAlly(PlayerTypes eNewAlly);

	bool IsAllies(PlayerTypes ePlayer) const;
	bool IsFriends(PlayerTypes ePlayer) const;

	bool IsEverFriends(PlayerTypes ePlayer);
	void SetEverFriends(PlayerTypes ePlayer, bool bValue);

	bool IsCloseToNotBeingAllies(PlayerTypes ePlayer);
	bool IsCloseToNotBeingFriends(PlayerTypes ePlayer);

	int GetFriendshipLevelWithMajor(PlayerTypes ePlayer);
	int GetFriendshipNeededForNextLevel(PlayerTypes ePlayer);

	void DoFriendshipChangeEffects(PlayerTypes ePlayer, int iOldFriendship, int iNewFriendship);

	bool IsFriendshipAboveFriendsThreshold(int iFriendship) const;
	int GetFriendsThreshold() const;
	bool IsFriendshipAboveAlliesThreshold(int iFriendship) const;
	int GetAlliesThreshold() const;
	void DoSetBonus(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, bool bPassedBySomeone = false, PlayerTypes eNewAlly = NO_PLAYER);

	void DoIntrusion();

	// DEPRECATED
	bool IsMajorIntruding(PlayerTypes eMajor) const;
	void SetMajorIntruding(PlayerTypes eMajor, bool bValue);
	// END DEPRECATED

	bool IsPlayerHasOpenBorders(PlayerTypes ePlayer);
	bool IsPlayerHasOpenBordersAutomatically(PlayerTypes ePlayer);

	void DoLiberationByMajor(PlayerTypes eMajor);

	// ************************************
	// ***** Friendship - with Benefits ***** - slewis: woah
	// ************************************

	bool DoMajorCivEraChange(PlayerTypes ePlayer, EraTypes eNewEra);

	int GetScienceFriendshipBonus ();
	int GetScienceFriendshipBonusTimes100 ();
	int GetCurrentScienceFriendshipBonusTimes100 (PlayerTypes ePlayer);

	int GetCultureFriendshipBonus (PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetCultureAlliesBonus (PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetCurrentCultureBonus (PlayerTypes ePlayer);

	int GetHappinessFriendshipBonus(PlayerTypes ePlayer);
	int GetCurrentHappinessFriendshipBonus(PlayerTypes ePlayer);

	int GetFriendsCapitalFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetFriendsOtherCityFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetAlliesCapitalFoodBonus(PlayerTypes ePlayer);
	int GetAlliesOtherCityFoodBonus(PlayerTypes ePlayer);
	int GetCurrentCapitalFoodBonus(PlayerTypes ePlayer);
	int GetCurrentOtherCityFoodBonus(PlayerTypes ePlayer);

	// Unit spawning
	void DoSeedUnitSpawnCounter(PlayerTypes ePlayer, bool bBias = false);
	int GetUnitSpawnCounter(PlayerTypes ePlayer);
	void SetUnitSpawnCounter(PlayerTypes ePlayer, int iValue);
	void ChangeUnitSpawnCounter(PlayerTypes ePlayer, int iChange);

	bool IsUnitSpawningAllowed(PlayerTypes ePlayer);

	bool IsUnitSpawningDisabled(PlayerTypes ePlayer) const;
	void SetUnitSpawningDisabled(PlayerTypes ePlayer, bool bValue);

	void DoSpawnUnit(PlayerTypes ePlayer);
	void DoUnitSpawnTurn();

	int GetCurrentSpawnEstimate(PlayerTypes ePlayer);

	// ******************************
	// ***** General Minor Civ Stuff *****
	// ******************************

	int GetNumUnitsGifted(PlayerTypes ePlayer);
	void SetNumUnitsGifted(PlayerTypes ePlayer, int iValue);
	void ChangeNumUnitsGifted(PlayerTypes ePlayer, int iChange);

	void DoGoldGiftFromMajor(PlayerTypes ePlayer, int iGold);
	int GetFriendshipFromGoldGift(PlayerTypes eMajor, int iGold);

	void DoNowAtWarWithTeam(TeamTypes eTeam);
	void DoNowPeaceWithTeam();

	bool IsPeaceBlocked(TeamTypes eTeam) const;

	void DoTeamDeclaredWarOnMe(TeamTypes eEnemyTeam);
	bool IsPermanentWar(TeamTypes eTeam) const;
	void SetPermanentWar(TeamTypes eTeam, bool bValue);

	// ******************************
	// ***** Misc Helper Functions *****
	// ******************************

	bool IsHasMetPlayer(PlayerTypes ePlayer);
	bool IsAtWarWithPlayersTeam(PlayerTypes ePlayer);

	int GetNumResourcesMajorLacks(PlayerTypes eMajor);

	TechTypes GetGoodTechPlayerDoesntHave(PlayerTypes ePlayer, int iRoughTechValue) const;

	int GetMajorScratchPad(PlayerTypes ePlayer) const;
	void SetMajorScratchPad(PlayerTypes ePlayer, int iNum);

private:
	CvPlayer *m_pPlayer;
	MinorCivTypes m_minorCivType;
	MinorCivPersonalityTypes m_ePersonality;
	MinorCivStatusTypes m_eStatus;

	int m_iTurnsSinceThreatenedByBarbarians;

	bool m_abWarQuestAgainstMajor[MAX_MAJOR_CIVS];
	int m_aaiNumEnemyUnitsLeftToKillByMajor[MAX_MAJOR_CIVS][MAX_MAJOR_CIVS];

	bool m_abRouteConnectionEstablished[MAX_MAJOR_CIVS];

	PlayerTypes m_eAlly;

	int m_aiFriendshipWithMajorTimes100[MAX_MAJOR_CIVS];
	int m_aiAngerFreeIntrusionCounter[MAX_MAJOR_CIVS];
	int m_aiPlayerQuests[MAX_MAJOR_CIVS];
	int m_aiQuestData1[MAX_MAJOR_CIVS];
	int m_aiQuestData2[MAX_MAJOR_CIVS];
	int m_aiQuestCountdown[MAX_MAJOR_CIVS];
	int m_aiUnitSpawnCounter[MAX_MAJOR_CIVS];
	int m_aiNumUnitsGifted[MAX_MAJOR_CIVS];
	bool m_abUnitSpawningDisabled[MAX_MAJOR_CIVS];
	bool m_abMajorIntruding[MAX_MAJOR_CIVS];
	bool m_abEverFriends[MAX_MAJOR_CIVS];
	bool m_abPermanentWar[REALLY_MAX_TEAMS];

	int m_aiMajorScratchPad[MAX_MAJOR_CIVS];
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class :	CvMinorCivInfo
//
//  DESC:		Similar class to CvCivilizationInfo.
//				Stores Minor Civ-specific information, since Minors are not
//				detailed in CvCivilizationInfo
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMinorCivInfo : public CvBaseInfo
{
public:
	CvMinorCivInfo();
	virtual ~CvMinorCivInfo();

	int getDefaultPlayerColor() const;
	int getArtStyleType() const;
	int getNumCityNames() const;
	const char* getArtStylePrefix() const;
	const char* getArtStyleSuffix() const;

	const char* getShortDescription() const;
	const char* getShortDescriptionKey() const;
	void setShortDescriptionKey(const char* szVal);

	const char* getAdjective() const;
	const char* getAdjectiveKey() const;
	void setAdjectiveKey(const char* szVal);

	const char* getFlagTexture() const;
	const char* getArtDefineTag() const;
	void setArtDefineTag(const char* szVal);
	void setArtStylePrefix(const char* szVal);
	void setArtStyleSuffix(const char* szVal);

	int GetMinorCivTrait() const;

	// Deprecated Members
	const char* getAdjectiveKeyWide() const;
	const char* getShortDescriptionKeyWide() const;

	// Arrays
	int getFlavorValue(int i) const;
	const std::string& getCityNames(int i) const;

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

protected:
	int m_iDefaultPlayerColor;
	int m_iArtStyleType;
	int m_iNumLeaders;				 // the number of leaders the Civ has, this is needed so that random leaders can be generated easily
	int m_iMinorCivTrait;

	bool m_bAIPlayable;
	bool m_bPlayable;

	CvString m_strArtDefineTag;
	CvString m_strArtStylePrefix;
	CvString m_strArtStyleSuffix;

	CvString m_strShortDescriptionKey;
	CvString m_wstrShortDescription;

	CvString m_strAdjectiveKey;
	CvString m_wstrAdjective;

	//Deprecated members (please remove these once the char mess is organized)
	CvString m_wstrAdjectiveKey;
	CvString m_wstrShortDescriptionKey;

	// Arrays
	int* m_piFlavorValue;
	std::vector<CvString> m_vCityNames;
};

#endif //CIV5_MINOR_CIV_AI_H