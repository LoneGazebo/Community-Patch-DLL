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
//  CLASS: CvEspionageSpy
//!  \brief All the information about a spy
//
//!  Key Attributes:
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
enum CvSpyRank
{
    SPY_RANK_RECRUIT,
    SPY_RANK_AGENT,
    SPY_RANK_SPECIAL_AGENT,
    NUM_SPY_RANKS
};

enum CvAdvancedAction
{
	ADVANCED_ACTION_NONE,
	ADVANCED_ACTION_UNIT,
	ADVANCED_ACTION_GP,
	ADVANCED_ACTION_BUILDING,
	ADVANCED_ACTION_WW,
	ADVANCED_ACTION_GOLD_THEFT,
	ADVANCED_ACTION_SCIENCE_THEFT,
	ADVANCED_ACTION_UNREST,
	ADVANCED_ACTION_REBELLION,
	ADVANCED_ACTION_FAILURE,

	NUM_ADVANCED_ACTIONS
};

enum CvSpyState
{
    SPY_STATE_UNASSIGNED,
    SPY_STATE_TRAVELLING,
    SPY_STATE_SURVEILLANCE,
    SPY_STATE_GATHERING_INTEL,
#if defined(MOD_BALANCE_CORE)
	SPY_STATE_PREPARING_HEIST,
#endif
    SPY_STATE_RIG_ELECTION,
    SPY_STATE_COUNTER_INTEL,
	SPY_STATE_MAKING_INTRODUCTIONS,
	SPY_STATE_SCHMOOZE,
    SPY_STATE_DEAD,
#if defined(MOD_API_ESPIONAGE)
    SPY_STATE_TERMINATED,
#endif
    NUM_SPY_STATES
};

enum CvSpyResult // what was the result of the last spy action
{
    SPY_RESULT_UNDETECTED, // spy was not detected
    SPY_RESULT_DETECTED,   // a spy was detected in the city, but the defensive player can't tell which player
    SPY_RESULT_IDENTIFIED, // a spy was detected and identified in the city
    SPY_RESULT_KILLED,     // a spy was detected, identified, and killed in the city
#if defined(MOD_API_ESPIONAGE)
    SPY_RESULT_ELIMINATED, // a spy was detected, identified, and killed in the city, in such an embarrassing way that another spy won't be recruited!
#endif
    NUM_SPY_RESULTS
};

enum CvIntrigueType // What intrigue was uncovered?
{
    INTRIGUE_TYPE_DECEPTION,			    // A civ is lying to another civ
    INTRIGUE_TYPE_BUILDING_ARMY,		    // A civ is amassing an army
    INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY, // A civ is amassing an army to attack over the water
    INTRIGUE_TYPE_ARMY_SNEAK_ATTACK,	    // A civ is sending an army toward another civ
    INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK,  // a civ is sending a land invasion across the water toward another civ
	INTRIGUE_TYPE_CONSTRUCTING_WONDER,		// A civ is constructing a wonder
    NUM_INTRIGUE_TYPES
};

class CvEspionageSpy
{
public:
	CvEspionageSpy();

#if defined(MOD_BUGFIX_SPY_NAMES)
	const char* GetSpyName(CvPlayer* pPlayer);
#endif

#if defined(MOD_API_ESPIONAGE)
	void SetSpyState(PlayerTypes eSpyOwner, int iSpyIndex, CvSpyState eSpyState);
#endif

	// Public data
	int m_iName;
#if defined(MOD_BUGFIX_SPY_NAMES)
	CvString m_sName;
#endif
	int m_iCityX;
	int m_iCityY;
	CvSpyRank m_eRank;
	int GetSpyRank(PlayerTypes eSpyOwner) const;
	CvSpyState m_eSpyState;
	int m_iReviveCounter; // after killed, counter to reincarnate a spy
	bool m_bIsDiplomat;
	bool m_bEvaluateReassignment; // used by the AI. Flag to indicate if the spy should be evaluated to be reassigned
#if defined(MOD_API_ESPIONAGE)
	bool m_bPassive;
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	bool m_bIsThief;
	void SetSpyActiveTurn(int iGameTurn);
	int GetSpyActiveTurn();
	void ChangeAdvancedActions(int iValue);
	void SetAdvancedActions(int iValue);
	int GetAdvancedActions();
	int m_iTurnActivated;
	int m_iActionsDone;
	int m_iCooldown;
	void ChangeAdvancedActionsCooldown(int iValue);
	void SetAdvancedActionsCooldown(int iValue);
	int GetAdvancedActionsCooldown();
#endif
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
struct HeistLocation
{
	int m_iX;
	int m_iY;
};
#if defined(MOD_BALANCE_CORE)
struct GWList
{
	int m_iGreatWorkIndex;
};
#endif
struct SpyNotificationMessage
{
	int m_iCityX;
	int m_iCityY;
	PlayerTypes m_eAttackingPlayer;
	int m_iSpyResult;
	TechTypes m_eStolenTech;
#if defined(MOD_BALANCE_CORE_SPIES)
	BuildingTypes m_eDamagedBuilding;
	UnitTypes m_eDamagedUnit;
	bool m_bDamagedCity;
	int m_iGold;
	int m_iScience;
	bool m_bRebellion;
	int m_iGWIndex;
#endif
};
struct IntrigueNotificationMessage
{
	PlayerTypes m_eDiscoveringPlayer;
	PlayerTypes m_eSourcePlayer;
	PlayerTypes m_eTargetPlayer;
	BuildingTypes m_eBuilding;
	ProjectTypes m_eProject;
	int m_iIntrigueType;
	int m_iTurnNum;
	int m_iCityX;
	int m_iCityY;
	CvString m_strSpyName;
	int iSpyID;
	bool m_bShared;
};
typedef FStaticVector<CvEspionageSpy, 14, false, c_eCiv5GameplayDLL > SpyList;
typedef FStaticVector<TechTypes, 1, false, c_eCiv5GameplayDLL> TechList;
typedef FStaticVector<TechList, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> PlayerTechList;
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> NumTechsToStealList;
#if defined(MOD_BALANCE_CORE)
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> NumGWToStealList;
typedef Firaxis::Array<std::vector<GWList>, MAX_MAJOR_CIVS> GreatWorkStealList;
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> NumSpyActionsDone;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> MaxGWCost;
#endif
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> MaxTechCost;
typedef Firaxis::Array<std::vector<HeistLocation>, MAX_MAJOR_CIVS> HeistLocationList;

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

	void CreateSpy(void);
	void ProcessSpy(uint uiSpyIndex);
#if defined(MOD_BALANCE_CORE_SPIES)
	void AttemptAdvancedActions(uint uiSpyIndex);
	void DoAdvancedAction(uint uiSpyIndex, CvCity* pCity, CvAdvancedAction eAdvancedAction, int iRank, BuildingTypes eBuilding, UnitTypes eUnit, SpecialistTypes eSpecialist);
	bool CanAdvancedAction(uint uiSpyIndex, CvCity* pCity, CvAdvancedAction eAdvancedAction);
	std::vector<CvAdvancedAction> GetAdvancedActionPool(CvCity* pCity, BuildingTypes &eBuilding, UnitTypes &eUnit, SpecialistTypes &eSpecialist, int iRank);
	void DoAdvancedActionNotification(CvAdvancedAction eAdvancedAction, int iPassValue, CvCity* pCity, uint uiSpyIndex);
	void DoAdvancedActionLevelUp(CvAdvancedAction eAdvancedAction, uint uiSpyIndex, CvCity* pCity, int iPassValue);
	void DoAdvancedActionLogging(CvAdvancedAction eAdvancedAction, int iPassValue, CvCity* pCity, uint uiSpyIndex);

	CvString GetSpyChanceAtCity(CvCity* pCity, uint uiSpyIndex, bool bNoBasic);
	CvString GetCityPotentialInfo(CvCity* pCity, bool bNoBasic);
#endif
	void UncoverIntrigue(uint uiSpyIndex);
#if defined(MOD_BALANCE_CORE)
	void GetRandomIntrigue(CvCity* pCity, uint uiSpyIndex);
#endif
#if defined(MOD_BUGFIX_SPY_NAMES)
	void GetNextSpyName(CvEspionageSpy* pSpy);
#else
	int  GetNextSpyName(void);
#endif
	bool IsSpyInCity(uint uiSpyIndex);
	CvCity* GetCityWithSpy(uint uiSpyIndex);
	int  GetSpyIndexInCity(CvCity* pCity);
	bool CanEverMoveSpyTo(CvCity* pCity);
#if defined(MOD_BALANCE_CORE)
	bool CanMoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat, bool bAsThief);
	bool MoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat, bool bAsThief = false);
#else
	bool CanMoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat);
	bool MoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat);
#endif
	bool ExtractSpyFromCity(uint uiSpyIndex);
	void LevelUpSpy(uint uiSpyIndex);

#if defined(MOD_API_ESPIONAGE)
	void SetPassive(uint uiSpyIndex, bool bPassive);
	void SetOutcome(uint uiSpyIndex, uint uiSpyResult, bool bAffectsDiplomacy = true);
#endif

	void UpdateSpies();
	void UpdateCity(CvCity* pCity);

	int CalcPerTurn(int iSpyState, CvCity* pCity, int iSpyIndex);
	int CalcRequired(int iSpyState, CvCity* pCity, int iSpyIndex);

	const char* GetSpyRankName(int iRank) const;

	bool HasEstablishedSurveillance(uint uiSpyIndex);
	bool HasEstablishedSurveillanceInCity(CvCity* pCity);
	bool IsAnySurveillanceEstablished(PlayerTypes eTargetPlayer);

	bool IsDiplomat (uint uiSpyIndex);
	bool IsSchmoozing (uint uiSpyIndex);
	bool IsAnySchmoozing (CvCity* pCity);

#if defined(MOD_BALANCE_CORE)
	bool IsThief(uint uiSpyIndex);
	bool IsPreparingHeist(uint uiSpyIndex);
	bool IsAnyPreparingHeist(CvCity* pCity);
#endif

	bool CanStageCoup(uint uiSpyIndex);
	int GetCoupChanceOfSuccess(uint uiSpyIndex);
	bool AttemptCoup(uint uiSpyIndex);

	int GetTurnsUntilStateComplete(uint uiSpyIndex);
	int GetPercentOfStateComplete(uint uiSpyIndex);

	int GetNumSpies(void);
	int GetNumAliveSpies(void);
	int GetNumAssignedSpies(void);
	int GetNumUnassignedSpies(void);

	void BuildStealableTechList(PlayerTypes ePlayer);
	bool IsTechStealable(PlayerTypes ePlayer, TechTypes eTech);

	int GetNumTechsToSteal(PlayerTypes ePlayer);
#if defined(MOD_BALANCE_CORE)
	int GetNumSpyActionsDone(PlayerTypes ePlayer);

	void BuildStealableGWList(PlayerTypes ePlayer);
	bool IsGWStealable(PlayerTypes ePlayer, int iGreatWorkIndex);
	int GetNumGWToSteal(PlayerTypes ePlayer);
#endif

	bool IsMyDiplomatVisitingThem(PlayerTypes ePlayer, bool bIncludeTravelling = false);
	bool IsOtherDiplomatVisitingMe(PlayerTypes ePlayer);

#if defined(MOD_BALANCE_CORE_SPIES)
	void AddSpyMessage(int iCityX, int iCityY, PlayerTypes ePlayer, int iSpyResult, TechTypes eStolenTech, BuildingTypes eBuilding, UnitTypes eUnit, bool bUnrest, int iValue, int iScienceValue, bool bRebel, int iGreatWorkIndex = -1);
#else
	void AddSpyMessage(int iCityX, int iCityY, PlayerTypes ePlayer, int iSpyResult, TechTypes eStolenTech);
#endif
	void ProcessSpyMessages(void);

	void AddIntrigueMessage(PlayerTypes eDiscoveringPlayer, PlayerTypes eSourcePlayer, PlayerTypes eTargetPlayer, BuildingTypes eBuilding, ProjectTypes eProject, CvIntrigueType eIntrigueType, uint uiSpyIndex, CvCity* pCity, bool bShowNotification);
	Localization::String GetIntrigueMessage(uint uiIndex);
	bool HasRecentIntrigueAbout(PlayerTypes eTargetPlayer);
	IntrigueNotificationMessage* GetRecentIntrigueInfo(PlayerTypes eTargetPlayer);
	bool HasSharedIntrigue(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer, CvIntrigueType eIntrigueType);
	bool HasSharedIntrigue(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer);
	int MarkRecentIntrigueAsShared(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer, CvIntrigueType eIntrigueType);
	bool HasSharedIntrigueAboutMe(PlayerTypes eFromPlayer);

	CvString GetLogFileName(void) const;
	void LogEspionageMsg(CvString& strMsg);

	SpyList m_aSpyList;
	std::vector<int> m_aiSpyListNameOrder;
	int m_iSpyListNameOrderIndex;

	PlayerTechList m_aaPlayerStealableTechList;
	NumTechsToStealList m_aiNumTechsToStealList;
	HeistLocationList m_aHeistLocations;
	std::vector<SpyNotificationMessage> m_aSpyNotificationMessages; // cleared every turn after displayed for the player
	std::vector<IntrigueNotificationMessage> m_aIntrigueNotificationMessages; // cleared only between games
#if defined(MOD_BALANCE_CORE)
	MaxGWCost m_aiMaxGWCost;
	NumGWToStealList m_aiNumGWToStealList;
	GreatWorkStealList m_aPlayerStealableGWList;
	NumSpyActionsDone m_aiNumSpyActionsDone;
#endif
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
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyRateProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyGoalProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> LastProgressList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> SpyResultList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> LastPotentialList;
typedef Firaxis::Array<int, MAX_MAJOR_CIVS> NumTimesCityRobbedList;

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
	bool HasReachedGoal(PlayerTypes ePlayer);
	void ResetProgress(PlayerTypes ePlayer);
	void SetLastProgress(PlayerTypes ePlayer, int iProgress);
	void SetLastPotential(PlayerTypes ePlayer, int iPotential);
	void SetLastBasePotential(PlayerTypes ePlayer, int iPotential);
#if defined(MOD_EVENTS_ESPIONAGE)
	void SetSpyResult(PlayerTypes eSpyOwner, int iSpyIndex, int iResult);
#else
	void SetSpyResulttsp(PlayerTypes ePlayer, int iResult);
#endif

	bool HasCounterSpy();

	CvCity* m_pCity;
	SpyAssignmentList m_aiSpyAssignment;
	SpyAmountProgressList m_aiAmount; // how much has been collected so far
	SpyRateProgressList m_aiRate; // how much per turn
	SpyGoalProgressList m_aiGoal; // how many we need
	LastProgressList m_aiLastProgress; // the last progress we got from this city. This is recalculated when transitioning between surveillance and stealing a tech and while stealing techs
	LastPotentialList m_aiLastPotential; // the last potential we calculated from this city taking into account the spy stealing
	LastPotentialList m_aiLastBasePotential; // the last potential we calculated from this city without taking into account the spy
	SpyResultList m_aiResult; // what was the spy result this turn
	NumTimesCityRobbedList m_aiNumTimesCityRobbed; // how many times has this city had a tech stolen from it?
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
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> EspionageAIOutOfTechTurnList;
#if defined(MOD_BALANCE_CORE)
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> EspionageAIOutOfGWTurnList;
#endif
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> EspionageAILastTurns;
typedef FStaticVector<int, MAX_MAJOR_CIVS, false, c_eCiv5GameplayDLL> EspionageAICount;
typedef std::vector<CvCity*> EspionageCityList;

class CvEspionageAI
{
public:
	// check to see if the countdown clock is started
	enum {
		PLAN_DEFEND_CS_FOR_WIN,
		PLAN_ATTACK_CS_TO_PREVENT_DEFEAT,
		PLAN_COLLECT_VOTES,
		PLAN_PLAY_NORMAL
	};

	CvEspionageAI(void);
	~CvEspionageAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit(void);
	void Reset(void);

	void DoTurn(void);
	void StealTechnology(void);
#if defined(MOD_BALANCE_CORE)
	void StealGreatWork(void);
	void UpdateCivOutOfGWTurn();
#endif
	void UpdateCivOutOfTechTurn(void);
	void AttemptCoups(void);
	void FindTargetSpyNumbers(int* piTargetOffensiveSpies, int* piTargetDefensiveSpies, int* piTargetCityStateSpies, int* piTargetDiplomatSpies);
	void BuildDiplomatCityList(EspionageCityList& aEspionageCityList);
	void BuildOffenseCityList(EspionageCityList& aOffenseCityList);
	void BuildDefenseCityList(EspionageCityList& aDefenseCityList);
	void BuildMinorCityList(EspionageCityList& aMinorCityList);

	int GetCityStatePlan(PlayerTypes* peThreatPlayer = NULL);

	void EvaluateSpiesAssignedToTargetPlayer(PlayerTypes ePlayer);
	void EvaluateUnassignedSpies(void);
	void EvaluateDefensiveSpies(void);
	void EvaluateDiplomatSpies(void);

	CvPlayer* m_pPlayer;
	EspionageAIOutOfTechTurnList m_aiCivOutOfTechTurn; // when a civ has run out of techs to steal relative to us
#if defined(MOD_BALANCE_CORE)
	EspionageAIOutOfGWTurnList m_aiCivOutOfGWTurn; // when a civ has run out of techs to steal relative to us
#endif

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