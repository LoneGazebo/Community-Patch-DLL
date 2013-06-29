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

#define ENABLE_QUESTS_AT_START false
#define ENABLE_PERMANENT_WAR false
#define SAFE_ESTIMATE_NUM_QUESTS_PER_PLAYER (5)

enum MinorCivStatusTypes
{
    NO_MINOR_CIV_STATUS_TYPE = -1,

    MINOR_CIV_STATUS_NORMAL,
    MINOR_CIV_STATUS_ELEVATED,
    MINOR_CIV_STATUS_CRITICAL,

    NUM_MINOR_CIV_STATUS_TYPES,
};
FDataStream& operator<<(FDataStream&, const MinorCivStatusTypes&);
FDataStream& operator>>(FDataStream&, MinorCivStatusTypes&);

enum MinorCivPersonalityTypes
{
    NO_MINOR_CIV_PERSONALITY_TYPE = -1,

    MINOR_CIV_PERSONALITY_FRIENDLY,
    MINOR_CIV_PERSONALITY_NEUTRAL,
    MINOR_CIV_PERSONALITY_HOSTILE,
    MINOR_CIV_PERSONALITY_IRRATIONAL,

    NUM_MINOR_CIV_PERSONALITY_TYPES,
};
FDataStream& operator<<(FDataStream&, const MinorCivPersonalityTypes&);
FDataStream& operator>>(FDataStream&, MinorCivPersonalityTypes&);

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
    MINOR_CIV_QUEST_GIVE_GOLD,
    MINOR_CIV_QUEST_PLEDGE_TO_PROTECT,
    MINOR_CIV_QUEST_CONTEST_CULTURE,
    MINOR_CIV_QUEST_CONTEST_FAITH,
    MINOR_CIV_QUEST_CONTEST_TECHS,
    MINOR_CIV_QUEST_INVEST,
    MINOR_CIV_QUEST_BULLY_CITY_STATE,
    MINOR_CIV_QUEST_DENOUNCE_MAJOR,
    MINOR_CIV_QUEST_SPREAD_RELIGION,

    NUM_MINOR_CIV_QUEST_TYPES,
};
FDataStream& operator<<(FDataStream&, const MinorCivQuestTypes&);
FDataStream& operator>>(FDataStream&, MinorCivQuestTypes&);


typedef FStaticVector< PlayerTypes, MAX_PLAYERS, true, c_eCiv5GameplayDLL > CivsList;
typedef CvWeightedVector< PlayerTypes, MAX_PLAYERS, true > WeightedCivsList;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMinorCivQuest
//!  \brief		Quest given by a minor civ to a player
//
//!  Author:	Anton Strenger
//
//!  Key Attributes:
//!  - Will be contained inside the CvMinorCivAI of the minor civ that gave the quest
//!  - Plan is to be mostly accessed by CvMinorCivAI class, but perhaps by other gameplay classes as well
//!  - May be extended with functions for checking if quest is complete, etc.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMinorCivQuest
{
public:
	// Constants
	static const int NO_QUEST_DATA = -1;
	static const int NO_TURN = -1;

	// Functions

	CvMinorCivQuest();
	CvMinorCivQuest(PlayerTypes eMinor, PlayerTypes eAssignedPlayer, MinorCivQuestTypes eType);

	~CvMinorCivQuest();

	PlayerTypes GetMinor() const;
	PlayerTypes GetPlayerAssignedTo() const;
	MinorCivQuestTypes GetType() const;
	int GetStartTurn() const;
	int GetEndTurn() const;
	int GetTurnsRemaining(int iCurrentTurn) const;
	int GetPrimaryData() const;
	int GetSecondaryData() const;
	int GetInfluenceReward() const;

	// Contest helper functions
	int GetContestValueForPlayer(PlayerTypes ePlayer);
	int GetContestValueForLeader();
	CivsList GetContestLeaders();

	// Quest status for assigned player
	bool IsContestLeader(PlayerTypes ePlayer = NO_PLAYER);
	bool IsComplete();
	bool IsRevoked();
	bool IsExpired();
	bool IsObsolete();
	bool IsHandled();
	void SetHandled(bool bValue);

	// Starting and finishing
	void DoStartQuest(int iStartTurn);
	void DoStartQuestUsingExistingData(CvMinorCivQuest* pExistingQuest);
	bool DoFinishQuest();
	bool DoCancelQuest();

	// Public data
	PlayerTypes m_eMinor;
	PlayerTypes m_eAssignedPlayer;
	MinorCivQuestTypes m_eType;
	int m_iStartTurn;
	int m_iData1;
	int m_iData2;
	bool m_bHandled;
};
FDataStream& operator>>(FDataStream&, CvMinorCivQuest&);
FDataStream& operator<<(FDataStream&, const CvMinorCivQuest&);


class CvPlayer;

typedef FStaticVector< CvMinorCivQuest, SAFE_ESTIMATE_NUM_QUESTS_PER_PLAYER, false, c_eCiv5GameplayDLL > QuestListForPlayer; // will grow size if needed
typedef FStaticVector< QuestListForPlayer, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL > QuestListForAllPlayers;

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
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void ResetQuestList();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	void DoPickInitialItems();

	CvPlayer* GetPlayer();

	MinorCivTypes GetMinorCivType() const;

	MinorCivPersonalityTypes GetPersonality() const;
	void DoPickPersonality();

	MinorCivTraitTypes GetTrait() const;

	bool IsHasUniqueUnit() const;
	UnitTypes GetUniqueUnit() const;
	void DoPickUniqueUnit();

	// ******************************
	// Main functions
	// ******************************

	void DoTurn();

	void DoChangeAliveStatus(bool bAlive);

	void DoFirstContactWithMajor(TeamTypes eTeam, bool bSuppressMessages);

	void DoTestEndWarsVSMinors(PlayerTypes eOldAlly, PlayerTypes eNewAlly);

	void DoTurnStatus();
	MinorCivStatusTypes GetStatus() const;

	void DoAddStartingResources(CvPlot* pCityPlot);
	void DoRemoveStartingResources(CvPlot* pCityPlot);

	void AddNotification(CvString sString, CvString sSummaryString, PlayerTypes ePlayer, int iX = -1, int iY = -1);
	void AddQuestNotification(CvString sString, CvString sSummaryString, PlayerTypes ePlayer, int iX = -1, int iY = -1, bool bNewQuest = false);
	void AddBuyoutNotification(CvString sString, CvString sSummaryString, PlayerTypes ePlayer, int iX = -1, int iY = -1);

	// ******************************
	// Threatened by Barbarians event
	// ******************************

	bool IsThreateningBarbariansEventActiveForPlayer(PlayerTypes ePlayer);
	int GetNumThreateningBarbarians();

	void DoTestThreatenedAnnouncement();
	int GetTurnsSinceThreatenedAnnouncement() const;
	void SetTurnsSinceThreatenedAnnouncement(int iValue);
	void ChangeTurnsSinceThreatenedAnnouncement(int iChange);
	void DoTestThreatenedAnnouncementForPlayer(PlayerTypes ePlayer);
	bool IsPlayerCloseEnoughForThreatenedAnnouncement(PlayerTypes eMajor);
	
	void DoThreateningBarbKilled(PlayerTypes eKillingPlayer, int iX, int iY);

	// ******************************
	// War with Major pseudo-quest
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

	int GetFirstPossibleTurnForPersonalQuests() const;
	int GetFirstPossibleTurnForGlobalQuests() const;
	int GetMaxActivePersonalQuestsForPlayer() const;
	int GetMaxActiveGlobalQuests() const;

	void DoTestStartGlobalQuest();
	void DoTestStartPersonalQuest(PlayerTypes ePlayer);
	void AddQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType, int iStartTurn);
	void AddQuestCopyForPlayer(PlayerTypes ePlayer, CvMinorCivQuest* pQuest);
	void DoTestQuestsOnFirstContact(PlayerTypes eMajor);

	void DoTestActiveQuests(bool bTestComplete, bool bTestObsolete);
	void DoTestActiveQuestsForPlayer(PlayerTypes ePlayer, bool bTestComplete, bool bTestObsolete, MinorCivQuestTypes eSpecifyQuestType = NO_MINOR_CIV_QUEST_TYPE);
	void DoCompletedQuests();
	WeightedCivsList CalculateFriendshipFromQuests();
	void DoCompletedQuestsForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eSpecifyQuestType = NO_MINOR_CIV_QUEST_TYPE);
	void DoObsoleteQuests();
	void DoObsoleteQuestsForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eSpecifyQuestType = NO_MINOR_CIV_QUEST_TYPE);
	void DoQuestsCleanup();
	void DoQuestsCleanupForPlayer(PlayerTypes ePlayer);

	bool IsEnabledQuest(MinorCivQuestTypes eQuest);
	bool IsValidQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eQuest);
	bool IsValidQuestCopyForPlayer(PlayerTypes ePlayer, CvMinorCivQuest* pQuest);
	bool IsGlobalQuest(MinorCivQuestTypes eQuest) const;
	bool IsPersonalQuest(MinorCivQuestTypes eQuest) const;
	int GetMinPlayersNeededForQuest(MinorCivQuestTypes eQuest) const;
	int GetPersonalityQuestBias(MinorCivQuestTypes eQuest);

	int GetNumActiveGlobalQuests() const;
	int GetNumActiveQuestsForPlayer(PlayerTypes ePlayer) const;
	int GetNumActivePersonalQuestsForPlayer(PlayerTypes ePlayer) const;
	bool IsActiveQuestForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType);
	void EndAllActiveQuestsForPlayer(PlayerTypes ePlayer);

	void DoTestSeedGlobalQuestCountdown(bool bForceSeed = false);
	int GetGlobalQuestCountdown();
	void SetGlobalQuestCountdown(int iValue);
	void ChangeGlobalQuestCountdown(int iChange);
	void DoTestSeedQuestCountdownForPlayer(PlayerTypes ePlayer, bool bForceSeed = false);
	int GetQuestCountdownForPlayer(PlayerTypes ePlayer);
	void SetQuestCountdownForPlayer(PlayerTypes ePlayer, int iValue);
	void ChangeQuestCountdownForPlayer(PlayerTypes ePlayer, int iChange);

	// For debugging and testing:
	bool AddQuestIfAble(PlayerTypes eMajor, MinorCivQuestTypes eQuest);

	// Specific Details for Quests:

	int GetQuestData1(PlayerTypes ePlayer, MinorCivQuestTypes eType) const;
	int GetQuestData2(PlayerTypes ePlayer, MinorCivQuestTypes eType) const;
	int GetQuestTurnsRemaining(PlayerTypes ePlayer, MinorCivQuestTypes eType, int iGameTurn) const;
	bool IsContestLeader(PlayerTypes ePlayer, MinorCivQuestTypes eType);
	int GetContestValueForLeader(MinorCivQuestTypes eType);
	int GetContestValueForPlayer(PlayerTypes ePlayer, MinorCivQuestTypes eType);

	bool IsRouteConnectionEstablished(PlayerTypes eMajor) const;
	void SetRouteConnectionEstablished(PlayerTypes eMajor, bool bValue);
	CvPlot* GetBestNearbyCampToKill();
	ResourceTypes GetNearbyResourceForQuest(PlayerTypes ePlayer);
	BuildingTypes GetBestWonderForQuest(PlayerTypes ePlayer);
	UnitTypes GetBestGreatPersonForQuest(PlayerTypes ePlayer);
	PlayerTypes GetBestCityStateTarget();
	PlayerTypes GetMostRecentBullyForQuest() const;
	bool IsWantsMinorDead(PlayerTypes eMinor);
	PlayerTypes GetBestPlayerToFind(PlayerTypes ePlayer);
	bool IsGoodTimeForNaturalWonderQuest(PlayerTypes ePlayer);
	bool IsGoodTimeForGiveGoldQuest();
	bool IsGoodTimeForPledgeToProtectQuest();
	bool IsGoodTimeForDenounceMajorQuest();

	// ******************************
	// ***** Friendship *****
	// ******************************

	void DoFriendship();

	int GetFriendshipChangePerTurnTimes100(PlayerTypes ePlayer);

	int GetEffectiveFriendshipWithMajorTimes100(PlayerTypes ePlayer);
	int GetBaseFriendshipWithMajorTimes100(PlayerTypes ePlayer) const;
	void SetFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iNum, bool bFromQuest = false);
	void ChangeFriendshipWithMajorTimes100(PlayerTypes ePlayer, int iChange, bool bFromQuest = false);

	int GetEffectiveFriendshipWithMajor(PlayerTypes ePlayer);
	int GetBaseFriendshipWithMajor(PlayerTypes ePlayer) const;
	void SetFriendshipWithMajor(PlayerTypes ePlayer, int iNum, bool bFromQuest = false);
	void ChangeFriendshipWithMajor(PlayerTypes ePlayer, int iChange, bool bFromQuest = false);

	int GetFriendshipAnchorWithMajor(PlayerTypes ePlayer);
	
	void ResetFriendshipWithMajor(PlayerTypes ePlayer);

	int GetAngerFreeIntrusionCounter(PlayerTypes ePlayer) const;
	void SetAngerFreeIntrusionCounter(PlayerTypes ePlayer, int iNum);
	void ChangeAngerFreeIntrusionCounter(PlayerTypes ePlayer, int iChange);

	void DoUpdateAlliesResourceBonus(PlayerTypes eNewAlly, PlayerTypes eOldAlly);

	int GetMostFriendshipWithAnyMajor(PlayerTypes& eBestPlayer);
	PlayerTypes GetAlly() const;
	void SetAlly(PlayerTypes eNewAlly);
	int GetAlliedTurns() const;

	bool IsAllies(PlayerTypes ePlayer) const;
	bool IsFriends(PlayerTypes ePlayer);

	bool IsEverFriends(PlayerTypes ePlayer);
	void SetEverFriends(PlayerTypes ePlayer, bool bValue);

	bool IsCloseToNotBeingAllies(PlayerTypes ePlayer);
	bool IsCloseToNotBeingFriends(PlayerTypes ePlayer);

	int GetFriendshipLevelWithMajor(PlayerTypes ePlayer);
	int GetFriendshipNeededForNextLevel(PlayerTypes ePlayer);

	void DoFriendshipChangeEffects(PlayerTypes ePlayer, int iOldFriendship, int iNewFriendship, bool bFromQuest = false, bool bIgnoreMinorDeath = false);

	bool IsFriendshipAboveFriendsThreshold(int iFriendship) const;
	int GetFriendsThreshold() const;
	bool IsFriendshipAboveAlliesThreshold(int iFriendship) const;
	int GetAlliesThreshold() const;
	void DoSetBonus(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, bool bSuppressNotifications = false, bool bPassedBySomeone = false, PlayerTypes eNewAlly = NO_PLAYER);

	void DoIntrusion();

	// DEPRECATED
	bool IsMajorIntruding(PlayerTypes eMajor) const;
	void SetMajorIntruding(PlayerTypes eMajor, bool bValue);
	// END DEPRECATED

	bool IsPlayerHasOpenBorders(PlayerTypes ePlayer);
	bool IsPlayerHasOpenBordersAutomatically(PlayerTypes ePlayer);

	void DoLiberationByMajor(PlayerTypes eLiberator, TeamTypes eConquerorTeam);

	// Protection
	void DoChangeProtectionFromMajor(PlayerTypes eMajor, bool bProtect, bool bPledgeNowBroken = false);
	bool CanMajorProtect(PlayerTypes eMajor);
	bool CanMajorStartProtection(PlayerTypes eMajor);
	bool CanMajorWithdrawProtection(PlayerTypes eMajor);
	bool IsProtectedByMajor(PlayerTypes eMajor) const;
	bool IsProtectedByAnyMajor() const;
	
	int GetTurnLastPledgedProtectionByMajor(PlayerTypes eMajor) const;
	void SetTurnLastPledgedProtectionByMajor(PlayerTypes eMajor, int iTurn);
	int GetTurnLastPledgeBrokenByMajor(PlayerTypes eMajor) const;
	void SetTurnLastPledgeBrokenByMajor(PlayerTypes eMajor, int iTurn);

	// ************************************
	// ***** Friendship - with Benefits ***** - slewis: woah
	// ************************************

	bool DoMajorCivEraChange(PlayerTypes ePlayer, EraTypes eNewEra);

	int GetScienceFriendshipBonus();
	int GetScienceFriendshipBonusTimes100();
	int GetCurrentScienceFriendshipBonusTimes100(PlayerTypes ePlayer);

	// Culture bonuses
	int GetCultureFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetCultureFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetCurrentCultureFlatBonus(PlayerTypes ePlayer);
	int GetCulturePerBuildingFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetCulturePerBuildingAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetCurrentCulturePerBuildingBonus(PlayerTypes ePlayer);
	int GetCurrentCultureBonus(PlayerTypes ePlayer);

	// Happiness bonuses
	int GetHappinessFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetHappinessFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetCurrentHappinessFlatBonus(PlayerTypes ePlayer);
	int GetHappinessPerLuxuryFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetHappinessPerLuxuryAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetCurrentHappinessPerLuxuryBonus(PlayerTypes ePlayer);
	int GetCurrentHappinessBonus(PlayerTypes ePlayer);

	// Faith bonuses
	int GetFaithFlatFriendshipBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA) const;
	int GetFaithFlatAlliesBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA) const;
	int GetCurrentFaithFlatBonus(PlayerTypes ePlayer);
	int GetCurrentFaithBonus(PlayerTypes ePlayer);

	// Food bonuses
	int GetFriendsCapitalFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetFriendsOtherCityFoodBonus(PlayerTypes ePlayer, EraTypes eAssumeEra = NO_ERA);
	int GetAlliesCapitalFoodBonus(PlayerTypes ePlayer);
	int GetAlliesOtherCityFoodBonus(PlayerTypes ePlayer);
	int GetCurrentCapitalFoodBonus(PlayerTypes ePlayer);
	int GetCurrentOtherCityFoodBonus(PlayerTypes ePlayer);

	// Unit bonuses
	void DoSeedUnitSpawnCounter(PlayerTypes ePlayer, bool bBias = false);
	int GetUnitSpawnCounter(PlayerTypes ePlayer);
	void SetUnitSpawnCounter(PlayerTypes ePlayer, int iValue);
	void ChangeUnitSpawnCounter(PlayerTypes ePlayer, int iChange);
	bool IsUnitSpawningAllowed(PlayerTypes ePlayer);
	bool IsUnitSpawningDisabled(PlayerTypes ePlayer) const;
	void SetUnitSpawningDisabled(PlayerTypes ePlayer, bool bValue);
	void DoSpawnUnit(PlayerTypes eMajor);
	void DoUnitSpawnTurn();
	int GetCurrentSpawnEstimate(PlayerTypes ePlayer);

	// Buyout by major civ (Austria UA)
	bool IsBoughtOut() const;
	PlayerTypes GetMajorBoughtOutBy() const;
	void SetMajorBoughtOutBy(PlayerTypes eMajor);
	bool CanMajorBuyout(PlayerTypes eMajor);
	int GetBuyoutCost(PlayerTypes eMajor);
	void DoBuyout(PlayerTypes eMajor);

	// ************************************
	// ***** Bullying *****
	// ************************************

	int GetBullyGoldAmount(PlayerTypes eBullyPlayer);

	int CalculateBullyMetric(PlayerTypes eBullyPlayer);

	bool CanMajorBullyGold(PlayerTypes ePlayer);
	bool CanMajorBullyGold(PlayerTypes ePlayer, int iSpecifiedBullyMetric);
	bool CanMajorBullyUnit(PlayerTypes ePlayer);
	bool CanMajorBullyUnit(PlayerTypes ePlayer, int iSpecifiedBullyMetric);

	void DoMajorBullyGold(PlayerTypes eBully, int iGold);
	void DoMajorBullyUnit(PlayerTypes eBully, UnitTypes eUnitType);
	
	void DoBulliedByMajorReaction(PlayerTypes eBully, int iInfluenceChangeTimes100);

	bool IsEverBulliedByAnyMajor() const;
	bool IsEverBulliedByMajor(PlayerTypes ePlayer) const;
	bool IsRecentlyBulliedByAnyMajor() const; //antonjs: consider: replace with a new fn, GetTurnLastBulliedByAnyMajor
	bool IsRecentlyBulliedByMajor(PlayerTypes ePlayer) const; //antonjs: consider: replace with GetTurnLastBulliedByMajor
	int GetTurnLastBulliedByMajor(PlayerTypes ePlayer) const;
	void SetTurnLastBulliedByMajor(PlayerTypes ePlayer, int iTurn);

	// ****************
	// *** Election ***
	// ****************
	void DoElection();

	// ***********************************
	// ***** General Minor Civ Stuff *****
	// ***********************************

	int GetNumUnitsGifted(PlayerTypes ePlayer);
	void SetNumUnitsGifted(PlayerTypes ePlayer, int iValue);
	void ChangeNumUnitsGifted(PlayerTypes ePlayer, int iChange);

	int GetNumGoldGifted(PlayerTypes ePlayer) const;
	void SetNumGoldGifted(PlayerTypes ePlayer, int iValue);
	void ChangeNumGoldGifted(PlayerTypes ePlayer, int iChange);

	void DoGoldGiftFromMajor(PlayerTypes ePlayer, int iGold);
	int GetFriendshipFromGoldGift(PlayerTypes eMajor, int iGold);

	bool CanMajorGiftTileImprovement(PlayerTypes eMajor);
	bool IsLackingGiftableTileImprovementAtPlot(PlayerTypes eMajor, int iPlotX, int iPlotY);
	bool CanMajorGiftTileImprovementAtPlot(PlayerTypes eMajor, int iPlotX, int iPlotY);
	int GetGiftTileImprovementCost(PlayerTypes eMajor);
	void DoTileImprovementGiftFromMajor(PlayerTypes eMajor, int iPlotX, int iPlotY);

	void DoNowAtWarWithTeam(TeamTypes eTeam);
	void DoNowPeaceWithTeam(TeamTypes eTeam);

	bool IsPeaceBlocked(TeamTypes eTeam) const;

	void DoTeamDeclaredWarOnMe(TeamTypes eEnemyTeam);
	bool IsPermanentWar(TeamTypes eTeam) const;
	void SetPermanentWar(TeamTypes eTeam, bool bValue);

	bool IsWaryOfTeam(TeamTypes eTeam) const;
	void SetWaryOfTeam(TeamTypes eTeam, bool bValue);

	// ******************************
	// ***** Misc Helper Functions *****
	// ******************************

	bool IsHasMetPlayer(PlayerTypes ePlayer);
	bool IsAtWarWithPlayersTeam(PlayerTypes ePlayer);

	int GetNumResourcesMajorLacks(PlayerTypes eMajor);

	TechTypes GetGoodTechPlayerDoesntHave(PlayerTypes ePlayer, int iRoughTechValue) const;

	bool IsSameReligionAsMajor(PlayerTypes eMajor);

	CvString GetStatusChangeDetails(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies);
	pair<CvString, CvString> GetStatusChangeNotificationStrings(PlayerTypes ePlayer, bool bAdd, bool bFriends, bool bAllies, PlayerTypes eOldAlly, PlayerTypes eNewAlly);
	CvString GetNamesListAsString(CivsList veNames);

	int GetMajorScratchPad(PlayerTypes ePlayer) const;
	void SetMajorScratchPad(PlayerTypes ePlayer, int iNum);

	bool IsDisableNotifications() const;
	void SetDisableNotifications(bool bDisableNotifications);

private:
	CvPlayer* m_pPlayer;
	MinorCivTypes m_minorCivType;
	MinorCivPersonalityTypes m_ePersonality;
	MinorCivStatusTypes m_eStatus;
	UnitTypes m_eUniqueUnit;

	int m_iTurnsSinceThreatenedByBarbarians;
	int m_iGlobalQuestCountdown;

	bool m_abWarQuestAgainstMajor[MAX_MAJOR_CIVS];
	int m_aaiNumEnemyUnitsLeftToKillByMajor[MAX_MAJOR_CIVS][MAX_MAJOR_CIVS];

	bool m_abRouteConnectionEstablished[MAX_MAJOR_CIVS];

	PlayerTypes m_eAlly;
	int m_iTurnAllied;
	PlayerTypes m_eMajorBoughtOutBy;

	int m_aiFriendshipWithMajorTimes100[MAX_MAJOR_CIVS];
	int m_aiAngerFreeIntrusionCounter[MAX_MAJOR_CIVS];
	int m_aiPlayerQuests[MAX_MAJOR_CIVS]; //antonjs: DEPRECATED
	int m_aiQuestData1[MAX_MAJOR_CIVS]; //antonjs: DEPRECATED
	int m_aiQuestData2[MAX_MAJOR_CIVS]; //antonjs: DEPRECATED
	int m_aiQuestCountdown[MAX_MAJOR_CIVS];
	int m_aiUnitSpawnCounter[MAX_MAJOR_CIVS];
	int m_aiNumUnitsGifted[MAX_MAJOR_CIVS];
	int m_aiNumGoldGifted[MAX_MAJOR_CIVS];
	int m_aiTurnLastBullied[MAX_MAJOR_CIVS];
	int m_aiTurnLastPledged[MAX_MAJOR_CIVS];
	int m_aiTurnLastBrokePledge[MAX_MAJOR_CIVS];
	bool m_abUnitSpawningDisabled[MAX_MAJOR_CIVS];
	bool m_abMajorIntruding[MAX_MAJOR_CIVS];
	bool m_abEverFriends[MAX_MAJOR_CIVS];
	bool m_abPledgeToProtect[MAX_MAJOR_CIVS];
	bool m_abPermanentWar[REALLY_MAX_TEAMS];
	bool m_abWaryOfTeam[REALLY_MAX_TEAMS];
	QuestListForAllPlayers m_QuestsGiven;

	int m_aiMajorScratchPad[MAX_MAJOR_CIVS];
	bool m_bDisableNotifications;
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
