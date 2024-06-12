/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_ESPIONAGE_CLASSES_H
#define CIV5_ESPIONAGE_CLASSES_H

	
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvSpyPassiveBonusEntry
//!  \brief		A single entry in the spy passive bonus XML file
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpyPassiveBonusEntry : public CvBaseInfo
{
public:
	CvSpyPassiveBonusEntry(void);
	virtual ~CvSpyPassiveBonusEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	int GetNetworkPointsNeededScaled() const;
	int GetSciencePercentAdded() const;
	int GetTilesRevealed() const;
	bool IsRevealCityScreen() const;

protected:
	int m_iNetworkPointsNeeded;
	bool m_bNetworkPointsScaling;
	int m_iSciencePercentAdded;
	int m_iTilesRevealed;
	bool m_bRevealCityScreen;


private:
	CvSpyPassiveBonusEntry(const CvSpyPassiveBonusEntry&);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvSpyPassiveBonusXMLEntries
//!  \brief		Game-wide information about spy passive bonuses
//
//!  Key Attributes:
//!  - Contains an array of CvSpyPassiveBonusEntry from the above XML file
//!  - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpyPassiveBonusXMLEntries
{
public:
	CvSpyPassiveBonusXMLEntries(void);
	~CvSpyPassiveBonusXMLEntries(void);

	// Accessor functions
	std::vector<CvSpyPassiveBonusEntry*>& GetSpyPassiveBonusEntries();
	int GetNumSpyPassiveBonuses();
	CvSpyPassiveBonusEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvSpyPassiveBonusEntry*> m_paSpyPassiveBonusEntries;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvSpyPassiveBonusDiplomatEntry
//!  \brief		A single entry in the spy passive bonus diplomat XML file
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpyPassiveBonusDiplomatEntry : public CvBaseInfo
{
public:
	CvSpyPassiveBonusDiplomatEntry(void);
	virtual ~CvSpyPassiveBonusDiplomatEntry(void);

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	int GetNetworkPointsNeededScaled() const;
	bool IsReceiveIntrigue() const;
	bool IsRevealTrueApproaches() const;
	int GetVoteTradePercent() const;
	int GetTradeRouteGoldBonus() const;

protected:
	int m_iNetworkPointsNeeded;
	bool m_bNetworkPointsScaling;
	bool m_bReceiveIntrigue;
	bool m_bRevealTrueApproaches;
	int m_iVoteTradePercent;
	int m_iTradeRouteGoldBonus;


private:
	CvSpyPassiveBonusDiplomatEntry(const CvSpyPassiveBonusDiplomatEntry&);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvSpyPassiveBonusDiplomatXMLEntries
//!  \brief		Game-wide information about spy passive diplomat bonuses
//
//!  Key Attributes:
//!  - Contains an array of CvSpyPassiveBonusDiplomatEntry from the above XML file
//!  - One instance for the entire game
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpyPassiveBonusDiplomatXMLEntries
{
public:
	CvSpyPassiveBonusDiplomatXMLEntries(void);
	~CvSpyPassiveBonusDiplomatXMLEntries(void);

	// Accessor functions
	std::vector<CvSpyPassiveBonusDiplomatEntry*>& GetSpyPassiveBonusDiplomatEntries();
	int GetNumSpyPassiveBonusesDiplomat();
	CvSpyPassiveBonusDiplomatEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvSpyPassiveBonusDiplomatEntry*> m_paSpyPassiveBonusDiplomatEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS: CvEspionageSpy
//!  \brief All the information about a spy
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
enum CLOSED_ENUM CvSpyRank
{
    SPY_RANK_RECRUIT,
    SPY_RANK_AGENT,
    SPY_RANK_SPECIAL_AGENT,
    NUM_SPY_RANKS ENUM_META_VALUE
};

enum CLOSED_ENUM CvEspionageType
{
	ESPIONAGE_TYPE_KILL,
	ESPIONAGE_TYPE_IDENTIFY,
	ESPIONAGE_TYPE_DETECT,
	NUM_ESPIONAGE_TYPES ENUM_META_VALUE
};

enum CLOSED_ENUM CvSpyState
{
    SPY_STATE_UNASSIGNED,
    SPY_STATE_TRAVELLING,
    SPY_STATE_SURVEILLANCE, // CP only
    SPY_STATE_GATHERING_INTEL,
    SPY_STATE_RIG_ELECTION,
    SPY_STATE_COUNTER_INTEL,
	SPY_STATE_MAKING_INTRODUCTIONS, // CP only
	SPY_STATE_SCHMOOZE,
    SPY_STATE_DEAD,
    SPY_STATE_TERMINATED, // currently not used
    NUM_SPY_STATES ENUM_META_VALUE
};

enum CLOSED_ENUM CvSpyResult // what was the result of the last spy action
{
    SPY_RESULT_UNDETECTED,					// spy was not detected
    SPY_RESULT_DETECTED,					// a spy was detected in the city, but the defensive player can't tell which player
    SPY_RESULT_IDENTIFIED,					// a spy was detected and identified in the city
    SPY_RESULT_KILLED,						// a spy was detected, identified, and killed in the city
    SPY_RESULT_KILLED_NOT_IDENTIFIED,		// a spy was killed without being identified (VP only)
    NUM_SPY_RESULTS ENUM_META_VALUE
};

enum CLOSED_ENUM CvIntrigueType // What intrigue was uncovered?
{
    INTRIGUE_TYPE_DECEPTION,					// a civ is lying to another civ
    INTRIGUE_TYPE_BUILDING_ARMY,				// a civ is amassing an army
    INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY,		// a civ is amassing an army to attack over the water
    INTRIGUE_TYPE_ARMY_SNEAK_ATTACK,			// a civ is sending an army toward another civ
    INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK,		// a civ is sending a land invasion across the water toward another civ
	INTRIGUE_TYPE_BRIBE_WAR,					// a civ has bribed another one or has been bribed by another one to go to war
	INTRIGUE_TYPE_COOP_WAR,						// a civ has agreed with another one to go to war against a third civ
	INTRIGUE_TYPE_MILITARY_MIGHT_RISEN,			// the military might of a human player has risen strongly recently
	INTRIGUE_TYPE_CONSTRUCTING_WONDER_ANY_CITY, // a civ is constructing a wonder or one-time project in one of their cities (VP only)
	INTRIGUE_TYPE_BOUGHT_VOTES,					// a civ has brought world congress votes from another one (VP only)
	INTRIGUE_TYPE_SOLD_VOTES,					// a civ has sold world congress votes to another one (VP only)
	INTRIGUE_TYPE_CONSTRUCTING_WONDER,			// a civ is constructing a wonder
    NUM_INTRIGUE_TYPES ENUM_META_VALUE
};

class CvEspionageSpy
{
public:
	CvEspionageSpy();

	const char* GetSpyName(CvPlayer* pPlayer);

	PlayerTypes GetVassalDiplomatPlayer() const;
	CvSpyState GetSpyState() const;
	void SetSpyState(PlayerTypes eSpyOwner, int iSpyIndex, CvSpyState eSpyState);
	void SetSpyFocus(CityEventChoiceTypes eEventChoice);
	CityEventChoiceTypes GetSpyFocus() const;

	void SetTurnCounterspyMissionChanged(int iTurn);
	int GetTurnCounterspyMissionChanged() const;

	void SetTurnActiveMissionConducted(int iTurn);
	int GetTurnActiveMissionConducted() const;

	// Public data
	int m_iName;
	CvString m_sName;
	int m_iCityX;
	int m_iCityY;
	int m_iExperience;
	CvSpyRank m_eRank;
	int GetSpyRank(PlayerTypes eSpyOwner) const;
	CvSpyState m_eSpyState;
	int m_iReviveCounter; // after killed, counter to reincarnate a spy
	bool m_bIsDiplomat;
	PlayerTypes m_eVassalDiplomatPlayer; // for spies that are diplomats in the capital of a vassal
	bool m_bEvaluateReassignment; // used by the AI. Flag to indicate if the spy should be evaluated to be reassigned
	bool m_bPassive;
	int m_iTurnCounterspyMissionChanged;
	int m_iTurnActiveMissionConducted;
	CityEventChoiceTypes m_eSpyFocus; // focus type for events- events are classified.
};

FDataStream& operator>>(FDataStream&, CvEspionageSpy&);
FDataStream& operator<<(FDataStream&, const CvEspionageSpy&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvPlayerEspionage
//!  \brief		All the information about espionage relating to this player
//
//!  Key Attributes:
//!  - Core data in this class is a list of CvEspionageSpies
//!  - This object is created inside the CvPlayer object and accessed through CvPlayer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct ScoreCityEntry
{
	CvCity* m_pCity;
	int m_iScore;
	bool m_bDiplomat;
	CityEventChoiceTypes m_eMission;
};
struct ScoreCityEntryHighEval
{
	bool operator()(ScoreCityEntry const& a, ScoreCityEntry const& b) const
	{
		return a.m_iScore > b.m_iScore;
	}
};

struct ScoreCityEntryLowEval
{
	bool operator()(ScoreCityEntry const& a, ScoreCityEntry const& b) const
	{
		return a.m_iScore < b.m_iScore;
	}
};
struct SpyNotificationMessage
{
	int m_iCityX;
	int m_iCityY;
	PlayerTypes m_eAttackingPlayer;
	int m_iSpyResult;
	int m_iAmountStolen;
	TechTypes m_eStolenTech;
	int m_iMission;
	int m_iGWID;
};
struct IntrigueNotificationMessage
{
	PlayerTypes m_eDiscoveringPlayer;
	PlayerTypes m_eSourcePlayer;
	PlayerTypes m_eTargetPlayer;
	PlayerTypes m_eDiplomacyPlayer;
	BuildingTypes m_eBuilding;
	ProjectTypes m_eProject;
	UnitTypes m_eUnit;
	int m_iIntrigueType;
	int m_iTurnNum;
	int m_iCityX;
	int m_iCityY;
	CvString m_strSpyName;
	int iSpyID;
	bool m_bShared;
};
typedef vector<CvEspionageSpy> SpyList;
typedef vector<TechTypes> TechList;
typedef vector<TechList> PlayerTechList;
typedef vector<int> NumTechsToStealList;
typedef vector<int> NumGWToStealList;
typedef vector<int> NumSpyActionsDone;
typedef vector<int> MaxTechCost;
class CvPlayerEspionage
{
public:
	CvPlayerEspionage(void);
	~CvPlayerEspionage(void);
	void Init(CvPlayer* pPlayer);
	void Uninit(void);
	void Reset(void);

	// Functions invoked each player turn
	void DoTurn(void);
	void LogSpyStatus();

	void CreateSpy(PlayerTypes eAsDiplomatInCapitalOfPlayer = NO_PLAYER);
	void MoveDiplomatVassalToNewCity(PlayerTypes eVassal, CvCity* pNewCity);
	void DeleteDiplomatForVassal(PlayerTypes eFormerVassal);
	void ProcessSpy(uint uiSpyIndex);
	bool TriggerSpyFocusSetup(CvCity* pCity, int uiSpyIndex);
	bool DoStealTechnology(CvCity* pPlayerCity, PlayerTypes eTargetPlayer);
	bool DoStealGW(CvCity* pPlayerCity, int iGWID);

	void ProcessSpyMissionResult(PlayerTypes eSpyOwner, CvCity* pCity, int uiSpyIndex, CityEventChoiceTypes eMission);
	CvString GetEventHelpText(CityEventTypes eEvent, int uiSpyIndex);

	//Tooltips
	CvString GetSpyMissionTooltip(CvCity* pCity, uint uiSpyIndex);

	void UncoverIntrigue(uint uiSpyIndex);
	void UncoverCityBuildingWonder(uint uiSpyIndex);
	void GetRandomIntrigue(CvCity* pCity, uint uiSpyIndex);
	void GetNextSpyName(CvEspionageSpy* pSpy);
	bool IsSpyInCity(uint uiSpyIndex);
	CvCity* GetCityWithSpy(uint uiSpyIndex);
	CvEspionageSpy* GetSpyByID(uint uiSpyIndex);
	int  GetSpyIndexInCity(CvCity* pCity);
	bool CanEverMoveSpyTo(CvCity* pCity);
	bool CanMoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat, bool bForce = false);
	bool MoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat, bool bForce = false);
	int GetNumTurnsSpyMovementBlocked(uint uiSpyIndex);
	int GetNumTurnsSpyActiveMissionsBlocked(uint uiSpyIndex);
	bool ExtractSpyFromCity(uint uiSpyIndex);
	void LevelUpSpy(uint uiSpyIndex, int iExperience = 0);

	void SetPassive(uint uiSpyIndex, bool bPassive);

	void UpdateSpies();
	void UpdateCity(CvCity* pCity);

	int CalcPerTurn(int iSpyState, CvCity* pCity, int iSpyIndex, bool bGlobalCheck = false);
	int CalcRequired(int iSpyState, CvCity* pCity, int iSpyIndex, bool bGlobalCheck = false);
	int CalcNetworkPointsPerTurn(CvSpyState eSpyState, CvCity* pCity, int iSpyIndex, CvString* toolTipSink = NULL);

	const char* GetSpyRankName(int iRank) const;

	bool HasEstablishedSurveillance(uint uiSpyIndex);
	bool HasEstablishedSurveillanceInCity(CvCity* pCity);
	bool IsAnySurveillanceEstablished(PlayerTypes eTargetPlayer);

	int GetSpyReceivingIntrigues(PlayerTypes eTargetPlayer);

	bool IsDiplomat (uint uiSpyIndex);
	bool IsSchmoozing (uint uiSpyIndex);
	bool IsAnySchmoozing (CvCity* pCity);

	bool CanStageCoup(uint uiSpyIndex);
	bool CanStageCoup(CvCity* pCity);
	int GetCoupChanceOfSuccess(uint uiSpyIndex, bool bIgnoreEnemySpies = false);
	int GetTheoreticalChanceOfCoup(CvCity* pCity, int iMySpyRank = 0, bool bIgnoreEnemySpies = false);
	bool AttemptCoup(uint uiSpyIndex);

	int GetTurnsUntilStateComplete(uint uiSpyIndex);
	int GetNetworkPointsStored(uint uiSpyIndex);
	int GetMaxNetworkPointsStored(uint uiSpyIndex);
	int GetNetworkPointsPerTurn(uint uiSpyIndex);
	int GetPercentOfStateComplete(uint uiSpyIndex);

	int GetNumSpies(void);
	int GetNumAliveSpies(void);
	int GetNumAssignedSpies(void);
	int GetNumUnassignedSpies(void);

	std::vector<int> BuildGWList(CvCity* pCity);

	void BuildStealableTechList(PlayerTypes ePlayer);
	bool IsTechStealable(PlayerTypes ePlayer, TechTypes eTech);

	int GetNumTechsToSteal(PlayerTypes ePlayer);
	int GetNumSpyActionsDone(PlayerTypes ePlayer);

	bool IsMyDiplomatVisitingThem(PlayerTypes ePlayer, bool bIncludeTravelling = false);
	bool IsOtherDiplomatVisitingMe(PlayerTypes ePlayer);

	void AddSpyMessage(int iCityX, int iCityY, PlayerTypes eAttackingPlayer, int iSpyResult, CityEventChoiceTypes eMission = NO_EVENT_CHOICE_CITY, TechTypes eStolenTech = NO_TECH, int iAmountStolen = -1, int iGWID = -1);
	void UpdateSpyMessage(int iCityX, int iCityY, PlayerTypes eAttackingPlayer, TechTypes eStolenTech = NO_TECH, int iAmountStolen = -1, int iGWID = -1);
	void ProcessSpyMessages(void);

	void AddIntrigueMessage(PlayerTypes eDiscoveringPlayer, PlayerTypes eSourcePlayer, PlayerTypes eTargetPlayer, PlayerTypes eDiplomacyPlayer, BuildingTypes eBuilding, ProjectTypes eProject, UnitTypes eUnit, CvIntrigueType eIntrigueType, uint uiSpyIndex, CvCity* pCity, bool bShowNotification);
	Localization::String GetIntrigueMessage(uint uiIndex);
	bool HasRecentIntrigueAbout(PlayerTypes eTargetPlayer);
	IntrigueNotificationMessage* GetRecentIntrigueInfo(PlayerTypes eTargetPlayer);
	bool HasSharedIntrigue(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer, PlayerTypes eDiplomacyPlayer, CvIntrigueType eIntrigueType);
	bool HasSharedIntrigue(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer);
	int MarkRecentIntrigueAsShared(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer, PlayerTypes eDiplomacyPlayer, CvIntrigueType eIntrigueType);
	bool HasSharedIntrigueAboutMe(PlayerTypes eFromPlayer);

	CvString GetLogFileName(void) const;
	CvString GetScoringLogFileName(void) const;
	void LogEspionageMsg(CvString& strMsg);
	void LogEspionageScoringMsg(CvString& strMsg);

	SpyList m_aSpyList;
	std::vector<int> m_aiSpyListNameOrder;
	int m_iSpyListNameOrderIndex;

	CityEventTypes m_eSpyMissionEvent;
	CityEventTypes m_eCounterspyEvent;
	CityEventTypes GetSpyMissionEvent();
	CityEventTypes GetCounterspyEvent();

	PlayerTechList m_aaPlayerStealableTechList;
	NumTechsToStealList m_aiNumTechsToStealList;
	std::vector<SpyNotificationMessage> m_aSpyNotificationMessages; // cleared every turn after displayed for the player
	std::vector<IntrigueNotificationMessage> m_aIntrigueNotificationMessages; // cleared only between games
	NumSpyActionsDone m_aiNumSpyActionsDone;

private:
	CvPlayer* m_pPlayer;
};

FDataStream& operator>>(FDataStream&, CvPlayerEspionage&);
FDataStream& operator<<(FDataStream&, const CvPlayerEspionage&);


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvCityEspionage
//!  \brief		All the information about espionage relating to this player
//
//!  Key Attributes:
//!  - Core data in this class is the progress various civs have made on doing
//!     espionage in the city
//!  - This object is created inside the CvCity object and accessed through CvCity
//!  - This may be deprecated!
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyAssignmentList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyAmountProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyMaxAmountProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyNextPassiveBonusList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyRateProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyGoalProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> LastProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyResultList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> LastPotentialList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> NumTimesCityRobbedList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> CityPendingEventsList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> PassiveBonusList;
typedef Firaxis::Array<bool, MAX_MAJOR_CIVS> PassiveBonusBoolList;

class CvCityEspionage
{
public:
	CvCityEspionage(void);
	~CvCityEspionage(void);
	void Init(CvCity* pCity);
	void Uninit(void);
	void Reset(void);

	void SetActivity(PlayerTypes ePlayer, int iAmount, int iRate, int iGoal);
	void Process(PlayerTypes ePlayer);
	int GetAmount(PlayerTypes ePlayer) const;
	void ChangeAmount(PlayerTypes ePlayer, int iChange);
	void SetAmount(PlayerTypes ePlayer, int iValue);
	int GetRate(PlayerTypes ePlayer) const;
	void SetRate(PlayerTypes ePlayer, int iRate);
	int GetGoal(PlayerTypes ePlayer) const;
	void SetGoal(PlayerTypes ePlayer, int iGoal);
	int GetMaxAmount(PlayerTypes ePlayer) const;
	void SetMaxAmount(PlayerTypes ePlayer, int iValue);
	int GetNextPassiveBonus(PlayerTypes ePlayer) const;
	void SetNextPassiveBonus(PlayerTypes ePlayer, int iValue);
	bool HasReachedGoal(PlayerTypes ePlayer);
	void ResetProgress(PlayerTypes ePlayer);
	void ResetPassiveBonuses(PlayerTypes ePlayer);
	void SetLastProgress(PlayerTypes ePlayer, int iProgress);
	void SetLastPotential(PlayerTypes ePlayer, int iPotential);
	void SetLastBasePotential(PlayerTypes ePlayer, int iPotential);
	void SetSpyResult(PlayerTypes eSpyOwner, int iSpyIndex, int iResult);
	void SetRevealCityScreen(PlayerTypes ePlayer, bool bValue);
	bool GetRevealCityScreen(PlayerTypes ePlayer);
	void SetDiplomatTradeBonus(PlayerTypes ePlayer, int iValue);
	void ChangeDiplomatTradeBonus(PlayerTypes ePlayer, int iValue);
	int GetDiplomatTradeBonus(PlayerTypes ePlayer);
	void SetDiplomatReceiveIntrigues(PlayerTypes ePlayer, bool bValue);
	bool IsDiplomatReceiveIntrigues(PlayerTypes ePlayer);
	void SetDiplomatRevealTrueApproaches(PlayerTypes ePlayer, bool bValue);
	bool IsDiplomatRevealTrueApproaches(PlayerTypes ePlayer);
	void SetDiplomatVoteTradePercent(PlayerTypes ePlayer, int iValue);
	void ChangeDiplomatVoteTradePercent(PlayerTypes ePlayer, int iValue);
	int GetDiplomatVoteTradePercent(PlayerTypes ePlayer);
	void SetSciencePassivePerTurn(PlayerTypes ePlayer, int iValue);
	void ChangeSciencePassivePerTurn(PlayerTypes ePlayer, int iValue);
	int GetSciencePassivePerTurn(PlayerTypes ePlayer);
	void SetVisionBonus(PlayerTypes ePlayer, int iValue);
	void ChangeVisionBonus(PlayerTypes ePlayer, int iValue);
	int GetVisionBonus(PlayerTypes ePlayer);
	void AddNetworkPoints(PlayerTypes eSpyOwner, CvEspionageSpy* pSpy, int iNetworkPointsAdded, bool bInit = false);
	void AddNetworkPointsDiplomat(PlayerTypes eSpyOwner, CvEspionageSpy* pSpy, int iNetworkPointsAdded, bool bInit = false);
	void DoMission(PlayerTypes eSpyOwner, CityEventChoiceTypes eMission);
	int GetSpyResult(PlayerTypes eSpyOwner);

	void SetPendingEvents(PlayerTypes ePlayer, int iValue);
	void ChangePendingEvents(PlayerTypes ePlayer, int iChange);
	int GetPendingEvents(PlayerTypes ePlayer) const;

	bool HasCounterSpy() const;
	int GetCounterSpyID() const;
	CityEventChoiceTypes GetCounterSpyFocus() const;
	int GetCounterSpyRank() const;

	CvCity* m_pCity;
	SpyAssignmentList m_aiSpyAssignment;
	SpyAmountProgressList m_aiAmount; // how much has been collected so far
	SpyMaxAmountProgressList m_aiMaxAmount; // maximum amount that had once been collected during this stay
	SpyNextPassiveBonusList m_aiNextPassiveBonus; // how much is needed to active the next passive bonus
	SpyRateProgressList m_aiRate; // how much per turn
	SpyGoalProgressList m_aiGoal; // how many we need
	LastProgressList m_aiLastProgress; // the last progress we got from this city. This is recalculated when transitioning between surveillance and stealing a tech and while stealing techs
	LastPotentialList m_aiLastPotential; // the last potential we calculated from this city taking into account the spy stealing
	LastPotentialList m_aiLastBasePotential; // the last potential we calculated from this city without taking into account the spy
	SpyResultList m_aiResult; // what was the spy result this turn
	NumTimesCityRobbedList m_aiNumTimesCityRobbed; // how many times has this city had a tech stolen from it?
	CityPendingEventsList m_aiPendingEventsForPlayer;
	PassiveBonusBoolList m_abRevealCityScreen;
	PassiveBonusList m_aiDiplomatTradeBonus; // gold bonus to trade routes between the diplomat owner and the other civ
	PassiveBonusBoolList m_abDiplomatReceiveIntrigues; // the diplomat in this city can receives intrigues
	PassiveBonusBoolList m_abDiplomatRevealTrueApproaches; // the diplomat in this city causes the other player to give honest answers when asked about a third player
	PassiveBonusList m_aiDiplomatVoteTradePercent; // percentage of votes that can be traded
	PassiveBonusList m_aiSciencePassivePerTurn; // percentage of city's science given to the spy owner
	PassiveBonusList m_aiVisionBonus; // number of tiles visible around city
};

FDataStream& operator>>(FDataStream&, CvCityEspionage&);
FDataStream& operator<<(FDataStream&, const CvCityEspionage&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvEspionageAI
//!  \brief		The player-level AI for espionage
//
//!  Key Attributes:
//!  - Object is in the player class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef vector<int> EspionageAIOutOfTechTurnList;
typedef vector<int> EspionageAILastTurns;
typedef vector<int> EspionageAICount;
typedef vector<CvCity*> EspionageCityList;

class CvEspionageAI
{
public:
	CvEspionageAI(void);
	~CvEspionageAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit(void);
	void Reset(void);

	void DoTurn(void);
	void StealTechnology(void);
	void UpdateCivOutOfTechTurn(void);
	int GetPlayerModifier(PlayerTypes eTargetPlayer, bool bOnlyDiplo);
	void AttemptCoups(void);
	void PerformSpyMissions(void);

	int GetMissionScore(CvCity* pCity, CityEventChoiceTypes eMission, int iSpyIndex = -1, bool bLogAllChoices = false);
	CityEventChoiceTypes GetBestMissionInCity(CvCity* pCity, int& iScore, std::vector<int> aMissionList, int iSpyIndex = -1, bool bLogAllChoices = false);

	std::vector<ScoreCityEntry> BuildDiplomatCityList(bool bLogAllChoices = false);
	std::vector<ScoreCityEntry> BuildOffenseCityList(bool bLogAllChoices = false);
	std::vector<ScoreCityEntry> BuildDefenseCityList(bool bLogAllChoices = false);
	std::vector<ScoreCityEntry> BuildMinorCityList(bool bLogAllChoices = false);

	void EvaluateSpiesAssignedToTargetPlayer(PlayerTypes ePlayer);
	void EvaluateUnassignedSpies(void);
	void EvaluateDefensiveSpies(void);
	void EvaluateDiplomatSpies(void);
	void EvaluateMinorCivSpies(void);

	CvPlayer* m_pPlayer;
	EspionageAIOutOfTechTurnList m_aiCivOutOfTechTurn; // when a civ has run out of techs to steal relative to us

	EspionageAICount m_aiNumSpiesCaught; // how many spies we caught
	EspionageAICount m_aiNumSpiesKilled;   // how many spies we killed
	EspionageAICount m_aiNumSpiesDied;     // how many spies we controlled that were killed

	EspionageAILastTurns m_aiTurnLastSpyCaught; // last turn we caught a spy
	EspionageAILastTurns m_aiTurnLastSpyKilled; // last turn we killed a spy
	EspionageAILastTurns m_aiTurnLastSpyDied;   // last turn one of our spies was killed

	bool m_bUNCountdownStarted; // has the UN countdown started
	int m_iTurnEspionageStarted; // what turn espionage started
};

FDataStream& operator>>(FDataStream&, CvEspionageAI&);
FDataStream& operator<<(FDataStream&, const CvEspionageAI&);

#endif //CIV5_ESPIONAGE_CLASSES_H