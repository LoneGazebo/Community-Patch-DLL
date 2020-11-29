/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_AI_DIPLOMACY_H
#define CIV5_AI_DIPLOMACY_H

#include "CvDiplomacyAIEnums.h"
#include "CvDealClasses.h"
#include "CvMinorCivAI.h"

#define BULLY_DEBUGGING false

#if defined(MOD_EVENTS_DIPLO_MODIFIERS)
struct Opinion
{
	Localization::String m_str;
	int m_iValue;
};
#endif

struct DiploLogData
{
	DiploStatementTypes m_eDiploLogStatement;
	int m_iTurn;
};
FDataStream& operator<<(FDataStream&, const DiploLogData&);
FDataStream& operator>>(FDataStream&, DiploLogData&);

struct DeclarationLogData
{
	PublicDeclarationTypes m_eDeclaration;
	int m_iData1;
	int m_iData2;
	PlayerTypes m_eMustHaveMetPlayer;
	bool m_bActive;
	int m_iTurn;
};
FDataStream& operator<<(FDataStream&, const DeclarationLogData&);
FDataStream& operator>>(FDataStream&, DeclarationLogData&);

#define MAX_DIPLO_LOG_STATEMENTS 60
#define MAX_TURNS_SAFE_ESTIMATE 9999

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvDiplomacyAI
//!  \brief		Drives the diplomatic interaction of a player
//
//!  Author:	Jon Shafer
//
//!  Key Attributes:
//!  - Object created by CvPlayer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDiplomacyAI
{
public:
#if !defined(MOD_ACTIVE_DIPLOMACY)
	// This has been moved to CvEnums.h to make compatible for MOD_ACTIVE_DIPLOMACY
	enum DiplomacyPlayerType
	{
		DIPLO_FIRST_PLAYER		=  0,
		DIPLO_ALL_PLAYERS		= -1,
		DIPLO_AI_PLAYERS		= -2,
		DIPLO_HUMAN_PLAYERS		= -3
	};
#endif

	struct MinorGoldGiftInfo
	{
		PlayerTypes eMinor;
		PlayerTypes eMajorRival;
		bool bQuickBoost;
		int iGoldAmount;
	};

	// ************************************
	// Initialization & Serialization
	// ************************************

	CvDiplomacyAI(void);
	~CvDiplomacyAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;
	void update();

	// ************************************
	// Pointers
	// ************************************

	CvPlayer* GetPlayer();
	const CvPlayer* GetPlayer() const;
	TeamTypes GetTeam() const;

	// ************************************
	// Helper Functions
	// ************************************

	bool IsPlayerValid(PlayerTypes eOtherPlayer, bool bMyTeamIsValid = false) const;
	int GetNumValidMajorCivs() const;
	vector<PlayerTypes> GetAllValidMajorCivs() const;

	bool IsAtWar(PlayerTypes eOtherPlayer) const;
	bool IsAlwaysAtWar(PlayerTypes eOtherPlayer) const;
	bool IsTeammate(PlayerTypes eOtherPlayer) const;
	bool IsHasMet(PlayerTypes eOtherPlayer, bool bMyTeamIsValid = false) const;
	bool IsHasDefensivePact(PlayerTypes eOtherPlayer) const;
	bool IsHasResearchAgreement(PlayerTypes eOtherPlayer) const;
	bool IsHasEmbassy(PlayerTypes eOtherPlayer) const;
	bool IsHasOpenBorders(PlayerTypes eOtherPlayer) const;
	bool IsVassal(PlayerTypes eOtherPlayer) const;
	bool IsMaster(PlayerTypes eOtherPlayer) const;
	bool IsVoluntaryVassalage(PlayerTypes eOtherPlayer) const;

	// ************************************
	// Personality Values
	// ************************************

	int GetRandomPersonalityWeight(int iOriginalValue, int& iSeed);
	void DoInitializePersonality();
	void DoInitializeDiploPersonalityType();

	int GetVictoryCompetitiveness() const;
	int GetWonderCompetitiveness() const;
	int GetMinorCivCompetitiveness() const;
	int GetBoldness() const;
	int GetDiploBalance() const;
	int GetWarmongerHate() const;
	int GetDenounceWillingness() const;
	int GetDoFWillingness() const;
	int GetLoyalty() const;
	int GetNeediness() const;
	int GetForgiveness() const;
	int GetChattiness() const;
	int GetMeanness() const;

	int GetPersonalityMajorCivApproachBias(MajorCivApproachTypes eApproach) const;
	int GetPersonalityMinorCivApproachBias(MinorCivApproachTypes eApproach) const;

	DiploPersonalityTypes GetDiploPersonalityType() const;
	void SetDiploPersonalityType(DiploPersonalityTypes eDiploPersonalityType);
	bool IsConqueror() const;
	bool IsDiplomat() const;
	bool IsCultural() const;
	bool IsScientist() const;

	// Estimations of other players' tendencies
	int GetEstimatePlayerVictoryCompetitiveness(PlayerTypes ePlayer) const;
	int GetEstimatePlayerWonderCompetitiveness(PlayerTypes ePlayer) const;
	int GetEstimatePlayerMinorCivCompetitiveness(PlayerTypes ePlayer) const;
	int GetEstimatePlayerBoldness(PlayerTypes ePlayer) const;
	int GetEstimatePlayerDiploBalance(PlayerTypes ePlayer) const;
	int GetEstimatePlayerWarmongerHate(PlayerTypes ePlayer) const;
	int GetEstimatePlayerDenounceWillingness(PlayerTypes ePlayer) const;
	int GetEstimatePlayerDoFWillingness(PlayerTypes ePlayer) const;
	int GetEstimatePlayerLoyalty(PlayerTypes ePlayer) const;
	int GetEstimatePlayerNeediness(PlayerTypes ePlayer) const;
	int GetEstimatePlayerForgiveness(PlayerTypes ePlayer) const;
	int GetEstimatePlayerChattiness(PlayerTypes ePlayer) const;
	int GetEstimatePlayerMeanness(PlayerTypes ePlayer) const;	
	int GetEstimatePlayerMajorCivApproachBias(PlayerTypes ePlayer, MajorCivApproachTypes eApproach) const;
	int GetEstimatePlayerMinorCivApproachBias(PlayerTypes ePlayer, MinorCivApproachTypes eApproach) const;
	int GetEstimatePlayerFlavorValue(PlayerTypes ePlayer, FlavorTypes eFlavor) const;
	int GetDifferenceFromAverageFlavorValue(int iValue) const;

	// ************************************
	// Memory Management
	// ************************************

	// ------------------------------------
	// Diplomatic Interactions
	// ------------------------------------

	// Declaration Log
	void DoAddNewDeclarationToLog(PublicDeclarationTypes eDeclaration, int iData1, int iData2, PlayerTypes eMustHaveMetPlayer, bool bActive);
	PublicDeclarationTypes GetDeclarationLogTypeForIndex(int iIndex);
	int GetDeclarationLogData1ForIndex(int iIndex);
	int GetDeclarationLogData2ForIndex(int iIndex);
	PlayerTypes GetDeclarationLogMustHaveMetPlayerForIndex(int iIndex);
	bool IsDeclarationLogForIndexActive(int iIndex);
	void DoMakeDeclarationInactive(PublicDeclarationTypes eDeclaration, int iData1, int iData2);
	int GetDeclarationLogTurnForIndex(int iIndex);
	void SetDeclarationLogTurnForIndex(int iIndex, int iNewValue);
	void ChangeDeclarationLogTurnForIndex(int iIndex, int iChange);

	// Diplo Statement Log
	void DoAddNewStatementToDiploLog(PlayerTypes ePlayer, DiploStatementTypes eNewDiploLogStatement);
	DiploStatementTypes GetDiploLogStatementTypeForIndex(PlayerTypes ePlayer, int iIndex);
	int GetDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex);
	void SetDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex, int iNewValue);
	void ChangeDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex, int iChange);
	int GetNumTurnsSinceStatementSent(PlayerTypes ePlayer, DiploStatementTypes eDiploLogStatement);
#if defined(MOD_ACTIVE_DIPLOMACY)
	int GetNumTurnsSinceSomethingSent(PlayerTypes ePlayer);
#endif

	// Messages sent to other players about protected Minor Civs
	bool HasSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor);
	void SetSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue);
	void ResetSentAttackProtectedMinorTaunts(PlayerTypes eMinor);

	// ------------------------------------
	// Opinion & Approach
	// ------------------------------------

	// Opinion
	MajorCivOpinionTypes GetMajorCivOpinion(PlayerTypes ePlayer) const;
	void SetMajorCivOpinion(PlayerTypes ePlayer, MajorCivOpinionTypes eOpinion);
	int GetNumMajorCivOpinion(MajorCivOpinionTypes eOpinion) const;

	// Approach
	MajorCivApproachTypes GetMajorCivApproach(PlayerTypes ePlayer, bool bHideTrueFeelings = false) const;
	void SetMajorCivApproach(PlayerTypes ePlayer, MajorCivApproachTypes eApproach);

	MajorCivApproachTypes GetMajorCivStrategicApproach(PlayerTypes ePlayer) const;
	void SetMajorCivStrategicApproach(PlayerTypes ePlayer, MajorCivApproachTypes eApproach);

	int GetNumMajorCivApproach(MajorCivApproachTypes eApproach) const;

	// War Face: If we're planning for war, how are we acting towards ePlayer?
	WarFaceTypes GetWarFace(PlayerTypes ePlayer) const;
	void SetWarFace(PlayerTypes ePlayer, WarFaceTypes eFace);

	// Approach Values: Cached weight for each approach
	int GetPlayerApproachValue(PlayerTypes ePlayer, MajorCivApproachTypes eApproach) const;
	void SetPlayerApproachValue(PlayerTypes ePlayer, MajorCivApproachTypes eApproach, int iValue);
	PlayerTypes GetPlayerWithHighestApproachValue(MajorCivApproachTypes eApproach) const;

	// ------------------------------------
	// Minor Civs
	// ------------------------------------

	MinorCivApproachTypes GetMinorCivApproach(PlayerTypes eMinor) const;
	void SetMinorCivApproach(PlayerTypes eMinor, MinorCivApproachTypes eApproach);
	int GetNumMinorCivApproach(MinorCivApproachTypes eApproach) const;

	PlayerTypes GetCSBullyTargetPlayer() const;
	void SetCSBullyTargetPlayer(PlayerTypes ePlayer);

	PlayerTypes GetCSWarTargetPlayer() const;
	void SetCSWarTargetPlayer(PlayerTypes ePlayer);

	bool IsWantToRouteConnectToMinor(PlayerTypes eMinor);
	void SetWantToRouteConnectToMinor(PlayerTypes eMinor, bool bValue);

	// ------------------------------------
	// Planning Exchanges
	// ------------------------------------

	// Relationship Valuation
	int GetDoFValue(PlayerTypes ePlayer);
	PlayerTypes GetMostValuableDoF(bool bIgnoreDoFs);
	PlayerTypes GetNextBestDoF();

	int GetCompetitorValue(PlayerTypes ePlayer) const;
	PlayerTypes GetBiggestCompetitor() const;
	
	int GetDefensivePactValue(PlayerTypes ePlayer);
	PlayerTypes GetMostValuableDefensivePact(bool bIgnoreDPs);
	PlayerTypes GetNextBestDefensivePact();

	// Cached Exchange Desires
	bool IsWantsDoFWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsDoFWithPlayer(PlayerTypes ePlayer, bool bValue);
	int GetNumDoFsWanted(PlayerTypes ePlayer = NO_PLAYER) const;

	bool IsWantsDefensivePactWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsDefensivePactWithPlayer(PlayerTypes ePlayer, bool bValue);
	int GetNumDefensivePactsWanted(PlayerTypes ePlayer = NO_PLAYER) const;

	bool IsWantsResearchAgreementWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsResearchAgreementWithPlayer(PlayerTypes ePlayer, bool bValue);
	int GetNumResearchAgreementsWanted() const;

	// Demand Target
	PlayerTypes GetDemandTargetPlayer() const;
	void SetDemandTargetPlayer(PlayerTypes ePlayer);
	bool IsDemandReady();
	void SetDemandReady(bool bValue);

	// ------------------------------------
	// Exchanges
	// ------------------------------------

	// Declaration of Friendship
	bool IsDoFAccepted(PlayerTypes ePlayer) const;
	void SetDoFAccepted(PlayerTypes ePlayer, bool bValue);
	int GetNumDoF();

	short GetDoFCounter(PlayerTypes ePlayer) const;
	void SetDoFCounter(PlayerTypes ePlayer, int iValue);
	void ChangeDoFCounter(PlayerTypes ePlayer, int iChange);
	bool IsDoFMessageTooSoon(PlayerTypes ePlayer) const;

	DoFLevelTypes GetDoFType(PlayerTypes ePlayer) const;
	void SetDoFType(PlayerTypes ePlayer, DoFLevelTypes eDoFLevel);

	// Denouncement
	bool IsDenouncedPlayer(PlayerTypes ePlayer) const;
	void SetDenouncedPlayer(PlayerTypes ePlayer, bool bValue);
	bool IsDenouncedByPlayer(PlayerTypes ePlayer) const;

	int GetDenouncedPlayerTurn(PlayerTypes ePlayer) const;
	void SetDenouncedPlayerTurn(PlayerTypes ePlayer, int iTurn);
	int GetTurnsSinceDenouncedPlayer(PlayerTypes ePlayer) const;
	bool IsDenouncingPlayer(PlayerTypes ePlayer) const;

	int GetNumDenouncements();
	int GetNumDenouncementsOfPlayer();

	// Trade
	bool IsCantMatchDeal(PlayerTypes ePlayer);
	void SetCantMatchDeal(PlayerTypes ePlayer, bool bValue);

	// Demands
	int GetNumDemandEverMade(PlayerTypes ePlayer) const;
	void SetNumDemandEverMade(PlayerTypes ePlayer, int iValue);
	bool IsDemandEverMade(PlayerTypes ePlayer) const;

	int GetDemandMadeTurn(PlayerTypes ePlayer) const;
	void SetDemandMadeTurn(PlayerTypes ePlayer, int iValue);

	short GetDemandCounter(PlayerTypes ePlayer) const;
	void SetDemandCounter(PlayerTypes ePlayer, int iValue);
	void ChangeDemandCounter(PlayerTypes ePlayer, int iChange);

	short GetDemandTooSoonNumTurns(PlayerTypes ePlayer) const;
	bool IsDemandTooSoon(PlayerTypes ePlayer) const;

	// Assistance Values
	int GetRecentTradeValue(PlayerTypes ePlayer);
	void SetRecentTradeValue(PlayerTypes ePlayer, int iValue);
	void ChangeRecentTradeValue(PlayerTypes ePlayer, int iChange);
	
	int GetCommonFoeValue(PlayerTypes ePlayer);
	void SetCommonFoeValue(PlayerTypes ePlayer, int iValue);
	void ChangeCommonFoeValue(PlayerTypes ePlayer, int iChange);
	
	int GetRecentAssistValue(PlayerTypes ePlayer);
	void SetRecentAssistValue(PlayerTypes ePlayer, int iValue);
	void ChangeRecentAssistValue(PlayerTypes ePlayer, int iChange);

	// ------------------------------------
	// Coop Wars
	// ------------------------------------

	CoopWarStates GetCoopWarState(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer) const;
	void SetCoopWarState(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer, CoopWarStates eNewState);
	bool IsLockedIntoCoopWar(PlayerTypes ePlayer) const;

	CoopWarStates GetGlobalCoopWarAgainstState(PlayerTypes ePlayer) const;
	CoopWarStates GetGlobalCoopWarWithState(PlayerTypes ePlayer) const;
	int GetNumCoopWarTargets();

	int GetCoopWarStateChangeTurn(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer) const;
	void SetCoopWarStateChangeTurn(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer, int iTurn);
	bool IsCoopWarMessageTooSoon(PlayerTypes eAskingPlayer, PlayerTypes eTargetPlayer) const;

	int GetCoopWarScore(PlayerTypes ePlayer) const;
	void SetCoopWarScore(PlayerTypes ePlayer, int iValue);
	void ChangeCoopWarScore(PlayerTypes ePlayer, int iChange);

	// ------------------------------------
	// War
	// ------------------------------------

	// Do we want to launch a Sneak Attack Operation against ePlayer?
	bool IsWantsSneakAttack(PlayerTypes ePlayer) const;
	void SetWantsSneakAttack(PlayerTypes ePlayer, bool bValue);

	// Mustering For Attack: Is there Sneak Attack Operation completed and ready to roll against ePlayer?
	bool IsArmyInPlaceForAttack(PlayerTypes ePlayer) const;
	void SetArmyInPlaceForAttack(PlayerTypes ePlayer, bool bValue);

	// Did we want to start the war we're currently in with ePlayer?
	bool IsAggressor(PlayerTypes ePlayer) const;
	void SetAggressor(PlayerTypes ePlayer, bool bValue);

	// Num Turns At War
	int GetPlayerNumTurnsAtWar(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsAtWar(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsAtWar(PlayerTypes ePlayer, int iChange);
	int GetTeamNumTurnsAtWar(TeamTypes eTeam) const;
	int GetPlayerNumTurnsSinceCityCapture(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsSinceCityCapture(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsSinceCityCapture(PlayerTypes ePlayer, int iChange);

	// How many times have we gone to war?
	int GetNumWarsFought(PlayerTypes ePlayer) const;
	void SetNumWarsFought(PlayerTypes ePlayer, int iValue);
	void ChangeNumWarsFought(PlayerTypes ePlayer, int iChange);

	int GetNumWarsDeclaredOnUs(PlayerTypes ePlayer) const;
	void SetNumWarsDeclaredOnUs(PlayerTypes ePlayer, int iValue);
	void ChangeNumWarsDeclaredOnUs(PlayerTypes ePlayer, int iChange);

	// How many of our cities have they captured?
	int GetNumCitiesCapturedBy(PlayerTypes ePlayer) const;
	void SetNumCitiesCapturedBy(PlayerTypes ePlayer, int iValue);
	void ChangeNumCitiesCapturedBy(PlayerTypes ePlayer, int iChange);

	// War Value Lost: the int value of damage ePlayer has inflicted on us in war
	int GetWarValueLost(PlayerTypes ePlayer) const;
	void SetWarValueLost(PlayerTypes ePlayer, int iValue);
	void ChangeWarValueLost(PlayerTypes ePlayer, int iChange);

	// War Damage Level: how much damage have we taken in a war against ePlayer? Looks at WarValueLost
	int GetWarDamageValue(PlayerTypes ePlayer) const;
	void SetWarDamageValue(PlayerTypes ePlayer, int iValue);
	WarDamageLevelTypes GetWarDamageLevel(PlayerTypes ePlayer) const;
	void SetWarDamageLevel(PlayerTypes ePlayer, WarDamageLevelTypes eDamageLevel);

	// War State: How's the war with ePlayer going? (NO_WAR_STATE_TYPE if at peace)
	WarStateTypes GetWarState(PlayerTypes ePlayer) const;
	void SetWarState(PlayerTypes ePlayer, WarStateTypes eWarState);

	StateAllWars GetStateAllWars() const;
	void SetStateAllWars(StateAllWars eState);

	// War Projection: How do we think the war with ePlayer is going to end up? (NO_WAR_PROJECTION_TYPE if at peace)
	WarProjectionTypes GetWarProjection(PlayerTypes ePlayer) const;
	void SetWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection);
	WarProjectionTypes GetLastWarProjection(PlayerTypes ePlayer) const; // previous value of GetWarProjection
	void SetLastWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection);

	// War Goal: What is our objective in the war against ePlayer (NO_WAR_GOAL_TYPE if at peace)
	WarGoalTypes GetWarGoal(PlayerTypes ePlayer) const;
	void SetWarGoal(PlayerTypes ePlayer, WarGoalTypes eWarGoal);

	// ------------------------------------
	// Peace
	// ------------------------------------

	int GetPlayerNumTurnsAtPeace(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsAtPeace(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsAtPeace(PlayerTypes ePlayer, int iChange);

	// What are we willing to give up to ePlayer to make peace?
	PeaceTreatyTypes GetTreatyWillingToOffer(PlayerTypes ePlayer) const;
	void SetTreatyWillingToOffer(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty);

	// What are we willing to accept from ePlayer to make peace?
	PeaceTreatyTypes GetTreatyWillingToAccept(PlayerTypes ePlayer) const;
	void SetTreatyWillingToAccept(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty);

	// Want Peace Counter: how long have we wanted peace with ePlayer? (Looks at WarGoal)
	int GetWantPeaceCounter(PlayerTypes ePlayer) const;
	void SetWantPeaceCounter(PlayerTypes ePlayer, int iValue);
	void ChangeWantPeaceCounter(PlayerTypes ePlayer, int iChange);

	// ------------------------------------
	// Backstabbing Penalties
	// ------------------------------------

	// Is this friend untrustworthy?
	bool IsUntrustworthyFriend(PlayerTypes ePlayer) const;
	bool IsTeamUntrustworthy(TeamTypes eTeam) const;
	void SetUntrustworthyFriend(PlayerTypes ePlayer, bool bValue);

	// Did this friend ever betray us?
	bool WasEverBackstabbedBy(PlayerTypes ePlayer) const;
	bool WasOurTeamEverBackstabbedBy(PlayerTypes ePlayer) const;
	void SetEverBackstabbedBy(PlayerTypes ePlayer, bool bValue);

	// DoF Broken?
	bool IsDoFBroken(PlayerTypes ePlayer) const;
	void SetDoFBroken(PlayerTypes ePlayer, bool bValue);
	int GetDoFBrokenTurn(PlayerTypes ePlayer) const;
	void SetDoFBrokenTurn(PlayerTypes ePlayer, int iValue);

	// They denounced us while we were friends!
	bool IsFriendDenouncedUs(PlayerTypes ePlayer) const;
	void SetFriendDenouncedUs(PlayerTypes ePlayer, bool bValue);
	int GetFriendDenouncedUsTurn(PlayerTypes ePlayer) const;
	void SetFriendDenouncedUsTurn(PlayerTypes ePlayer, int iValue);
	int GetNumFriendsDenouncedBy();
	int GetWeDenouncedFriendCount();

	// They declared war on us while we were friends!
	bool IsFriendDeclaredWarOnUs(PlayerTypes ePlayer) const;
	void SetFriendDeclaredWarOnUs(PlayerTypes ePlayer, bool bValue);
	int GetFriendDeclaredWarOnUsTurn(PlayerTypes ePlayer) const;
	void SetFriendDeclaredWarOnUsTurn(PlayerTypes ePlayer, int iValue);
	int GetWeDeclaredWarOnFriendCount();

	// Backstab counter
	short GetPlayerBackstabCounter(PlayerTypes ePlayer) const;
	void SetPlayerBackstabCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerBackstabCounter(PlayerTypes ePlayer, int iChange);

	// ------------------------------------
	// Warmongering Penalties
	// ------------------------------------

	// Num Minors Attacked
	int GetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iChange, TeamTypes eAttackedTeam);

	// Num Minors Conquered
	int GetOtherPlayerNumMinorsConquered(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumMinorsConquered(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumMinorsConquered(PlayerTypes ePlayer, int iChange);

	// Num Majors Attacked
	int GetOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer, int iChange, TeamTypes eAttackedTeam);

	// Num Majors Conquered
	int GetOtherPlayerNumMajorsConquered(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumMajorsConquered(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumMajorsConquered(PlayerTypes ePlayer, int iChange);

	// The amount of warmonger hatred they generated
	int GetOtherPlayerWarmongerAmount(PlayerTypes ePlayer) const;
	int GetOtherPlayerWarmongerAmountTimes100(PlayerTypes ePlayer) const;
	void SetOtherPlayerWarmongerAmountTimes100(PlayerTypes ePlayer, int iNewValue);
	void ChangeOtherPlayerWarmongerAmountTimes100(PlayerTypes ePlayer, int iChangeAmount);

	// ------------------------------------
	// Aggressive Postures
	// ------------------------------------

	// Military Aggressive Posture: How aggressively has ePlayer positioned their Units in relation to us?
	AggressivePostureTypes GetMilitaryAggressivePosture(PlayerTypes ePlayer) const;
	void SetMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);

	// Expansion Aggressive Posture: How aggressively has ePlayer settled or conquered in proximity to us?
	AggressivePostureTypes GetExpansionAggressivePosture(PlayerTypes ePlayer) const;
	void SetExpansionAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);

	// Plot Buying Aggressive Posture: How aggressively is ePlayer buying land near us?
	AggressivePostureTypes GetPlotBuyingAggressivePosture(PlayerTypes ePlayer) const;
	void SetPlotBuyingAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);

	// ------------------------------------
	// Dispute Levels
	// ------------------------------------

	// Land Dispute
	DisputeLevelTypes GetLandDisputeLevel(PlayerTypes ePlayer) const;
	void SetLandDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);
	int GetTotalLandDisputeLevel();
	DisputeLevelTypes GetLastTurnLandDisputeLevel(PlayerTypes ePlayer) const;

	// Victory Dispute
	DisputeLevelTypes GetVictoryDisputeLevel(PlayerTypes ePlayer) const;
	void SetVictoryDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);

	// Victory Block
	BlockLevelTypes GetVictoryBlockLevel(PlayerTypes ePlayer) const;
	void SetVictoryBlockLevel(PlayerTypes ePlayer, BlockLevelTypes eBlockLevel);

	// Wonder Dispute
	DisputeLevelTypes GetWonderDisputeLevel(PlayerTypes ePlayer) const;
	void SetWonderDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);

	// Minor Civ Dispute
	DisputeLevelTypes GetMinorCivDisputeLevel(PlayerTypes ePlayer) const;
	void SetMinorCivDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);

	// ------------------------------------
	// Threat Levels
	// ------------------------------------

	ThreatTypes GetMilitaryThreat(PlayerTypes ePlayer) const;
	void SetMilitaryThreat(PlayerTypes ePlayer, ThreatTypes eMilitaryThreat);

	ThreatTypes GetWarmongerThreat(PlayerTypes ePlayer) const;
	void SetWarmongerThreat(PlayerTypes ePlayer, ThreatTypes eWarmongerThreat);

	// ------------------------------------
	// Strength Assessments
	// ------------------------------------

	// Military Strength: How strong is ePlayer compared to US?
	StrengthTypes GetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer) const;
	void SetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eMilitaryStrength);

	// Economic Strength: How strong is ePlayer compared to US?
	StrengthTypes GetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer) const;
	void SetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eEconomicStrength);

	// Target Value: how easy or hard of a target would ePlayer be to attack?
	TargetValueTypes GetPlayerTargetValue(PlayerTypes ePlayer) const;
	void SetPlayerTargetValue(PlayerTypes ePlayer, TargetValueTypes eTargetValue);
	bool IsEasyTarget(PlayerTypes ePlayer) const;
	void SetEasyTarget(PlayerTypes ePlayer, bool bValue);

	// ------------------------------------
	// Promises
	// ------------------------------------

	// Military Promise
	bool IsPlayerMadeMilitaryPromise(PlayerTypes ePlayer) const;
	void SetPlayerMadeMilitaryPromise(PlayerTypes ePlayer, bool bValue);
	int GetPlayerMadeMilitaryPromise(PlayerTypes ePlayer);
	short GetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer) const;
	void SetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iChange);
	bool IsPlayerBrokenMilitaryPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenMilitaryPromise(PlayerTypes ePlayer, bool bValue);
	int GetBrokenMilitaryPromiseTurn(PlayerTypes ePlayer) const;
	void SetBrokenMilitaryPromiseTurn(PlayerTypes ePlayer, int iValue);

	// Expansion Promise
	bool IsPlayerNoSettleRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerNoSettleRequestAccepted(PlayerTypes ePlayer, bool bValue);
	vector<PlayerTypes> GetPlayersWithNoSettlePolicy() const;
	short GetPlayerNoSettleRequestCounter(PlayerTypes ePlayer) const;
	void SetPlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iChange);
	bool IsDontSettleMessageTooSoon(PlayerTypes ePlayer) const;
	bool IsPlayerMadeExpansionPromise(PlayerTypes ePlayer, int iTestGameTurn = -1) const;
	void SetPlayerMadeExpansionPromise(PlayerTypes ePlayer, bool bValue);
	int GetPlayerMadeExpansionPromise(PlayerTypes ePlayer) const;
	AggressivePostureTypes GetPlayerExpansionPromiseData(PlayerTypes ePlayer);
	bool IsPlayerBrokenExpansionPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenExpansionPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredExpansionPromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredExpansionPromise(PlayerTypes ePlayer, bool bValue);
	pair<int,int> GetNoExpansionPromiseClosestCities(PlayerTypes eOtherPlayer);
	void SetNoExpansionPromiseClosestCities(PlayerTypes eOtherPlayer, pair<int,int> value);
	pair<int,int> GetLastTurnClosestCityPair(PlayerTypes eOtherPlayer);
	void SetLastTurnClosestCityPair(PlayerTypes eOtherPlayer, pair<int,int> value);
	int GetBrokenExpansionPromiseValue(PlayerTypes ePlayer);
	void SetBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iChange);
	int GetIgnoredExpansionPromiseValue(PlayerTypes ePlayer);
	void SetIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iChange);
	bool IsPlayerNoSettleRequestEverAsked(PlayerTypes ePlayer) const;
	void SetPlayerNoSettleRequestEverAsked(PlayerTypes ePlayer, bool bValue);
	bool EverMadeExpansionPromise(PlayerTypes ePlayer);
	void SetEverMadeExpansionPromise(PlayerTypes ePlayer, bool bValue);

	// Border Promise
	bool IsPlayerMadeBorderPromise(PlayerTypes ePlayer, int iTestGameTurn = -1);
	void SetPlayerMadeBorderPromise(PlayerTypes ePlayer, bool bValue);
	int GetPlayerMadeBorderPromise(PlayerTypes ePlayer);
	AggressivePostureTypes GetPlayerBorderPromiseData(PlayerTypes ePlayer);
	void SetPlayerBorderPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue);
	bool IsPlayerBrokenBorderPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenBorderPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredBorderPromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredBorderPromise(PlayerTypes ePlayer, bool bValue);
	int GetBrokenBorderPromiseValue(PlayerTypes ePlayer);
	void SetBrokenBorderPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeBrokenBorderPromiseValue(PlayerTypes ePlayer, int iChange);
	int GetIgnoredBorderPromiseValue(PlayerTypes ePlayer);
	void SetIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iChange);
	bool EverMadeBorderPromise(PlayerTypes ePlayer);
	void SetEverMadeBorderPromise(PlayerTypes ePlayer, bool bValue);

	// Bully City-State Promise
	bool IsPlayerMadeBullyCityStatePromise(PlayerTypes ePlayer) const;
	void SetPlayerMadeBullyCityStatePromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenBullyCityStatePromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenBullyCityStatePromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredBullyCityStatePromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredBullyCityStatePromise(PlayerTypes ePlayer, bool bValue);

	// Attack City-State Promise
	bool IsPlayerMadeAttackCityStatePromise(PlayerTypes ePlayer) const;
	void SetPlayerMadeAttackCityStatePromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenAttackCityStatePromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenAttackCityStatePromise(PlayerTypes ePlayer, bool bValue);
	int GetBrokenAttackCityStatePromiseTurn(PlayerTypes ePlayer) const;
	void SetBrokenAttackCityStatePromiseTurn(PlayerTypes ePlayer, int iValue);
	bool IsPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer, bool bValue);

	// Spy Promise
	bool IsPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer, bool bValue);
	short GetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer) const;
	void SetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iChange);
	bool IsStopSpyingMessageTooSoon(PlayerTypes ePlayer) const;
	bool IsPlayerMadeSpyPromise(PlayerTypes ePlayer) const;
	void SetPlayerMadeSpyPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenSpyPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenSpyPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredSpyPromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredSpyPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerStopSpyingRequestEverAsked(PlayerTypes ePlayer) const;
	void SetPlayerStopSpyingRequestEverAsked(PlayerTypes ePlayer, bool bValue);

	// Religious Conversion Promise
	bool IsPlayerAskedNotToConvert(PlayerTypes ePlayer) const;
	void SetPlayerAskedNotToConvert(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerAgreeNotToConvert(PlayerTypes ePlayer) const;
	void SetPlayerAgreeNotToConvert(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerMadeNoConvertPromise(PlayerTypes ePlayer) const;
	void SetPlayerMadeNoConvertPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenNoConvertPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenNoConvertPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredNoConvertPromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredNoConvertPromise(PlayerTypes ePlayer, bool bValue);
	bool HasPlayerEverConvertedCity(PlayerTypes ePlayer) const;
	void SetPlayerEverConvertedCity(PlayerTypes ePlayer, bool bValue);

	// Digging Promise
	bool IsPlayerAskedNotToDig(PlayerTypes ePlayer) const;
	void SetPlayerAskedNotToDig(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerAgreeNotToDig(PlayerTypes ePlayer) const;
	void SetPlayerAgreeNotToDig(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerMadeNoDiggingPromise(PlayerTypes ePlayer) const;
	void SetPlayerMadeNoDiggingPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenNoDiggingPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenNoDiggingPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredNoDiggingPromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredNoDiggingPromise(PlayerTypes ePlayer, bool bValue);

	// Coop War Promise
	bool IsPlayerBrokenCoopWarPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenCoopWarPromise(PlayerTypes ePlayer, bool bValue);

	// ------------------------------------
	// Event Flags
	// ------------------------------------

	bool IsPlayerForgaveForSpying(PlayerTypes ePlayer) const;
	void SetPlayerForgaveForSpying(PlayerTypes ePlayer, bool bValue);

	bool IsPlayerLiberatedCapital(PlayerTypes ePlayer) const;
	void SetPlayerLiberatedCapital(PlayerTypes ePlayer, bool bValue);

	bool IsDoFEverAsked(PlayerTypes ePlayer) const;
	void SetDoFEverAsked(PlayerTypes ePlayer, bool bValue);

	bool IsPlayerCapturedCapital(PlayerTypes ePlayer) const;
	void SetPlayerCapturedCapital(PlayerTypes ePlayer, bool bValue);
	bool IsCapitalCapturedBy(PlayerTypes ePlayer) const;

	bool IsPlayerCapturedHolyCity(PlayerTypes ePlayer) const;
	void SetPlayerCapturedHolyCity(PlayerTypes ePlayer, bool bValue);
	bool IsHolyCityCapturedBy(PlayerTypes ePlayer) const;

	// ------------------------------------
	// # of times/points counters
	// ------------------------------------

	int GetNumCitiesLiberatedBy(PlayerTypes ePlayer);
	void SetNumCitiesLiberatedBy(PlayerTypes ePlayer, int iValue);
	void ChangeNumCitiesLiberatedBy(PlayerTypes ePlayer, int iChange);

	int GetNumCiviliansReturnedToMe(PlayerTypes ePlayer) const;
	void SetNumCiviliansReturnedToMe(PlayerTypes ePlayer, int iValue);
	void ChangeNumCiviliansReturnedToMe(PlayerTypes ePlayer, int iChange);

	int GetNumTimesIntrigueSharedBy(PlayerTypes ePlayer) const;
	void SetNumTimesIntrigueSharedBy(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesIntrigueSharedBy(PlayerTypes ePlayer, int iChange);

	int GetNumLandmarksBuiltForMe(PlayerTypes ePlayer) const;
	void SetNumLandmarksBuiltForMe(PlayerTypes ePlayer, int iValue);
	void ChangeNumLandmarksBuiltForMe(PlayerTypes ePlayer, int iChange);

	int GetNumTimesTheyPlottedAgainstUs(PlayerTypes ePlayer) const;
	void SetNumTimesTheyPlottedAgainstUs(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesTheyPlottedAgainstUs(PlayerTypes ePlayer, int iChange);

	int GetNumTimesRazed(PlayerTypes ePlayer) const;
	void SetNumTimesRazed(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesRazed(PlayerTypes ePlayer, int iChange);

	int GetNumTradeRoutesPlundered(PlayerTypes ePlayer) const;
	void SetNumTradeRoutesPlundered(PlayerTypes ePlayer, int iValue);
	void ChangeNumTradeRoutesPlundered(PlayerTypes ePlayer, int iChange);

	int GetNumWondersBeatenTo(PlayerTypes ePlayer) const;
	void SetNumWondersBeatenTo(PlayerTypes ePlayer, int iNewValue);
	void ChangeNumWondersBeatenTo(PlayerTypes ePlayer, int iChange);

	int GetNumTimesCultureBombed(PlayerTypes ePlayer) const;
	void SetNumTimesCultureBombed(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesCultureBombed(PlayerTypes ePlayer, int iChange);

	int GetNumTimesTheyLoweredOurInfluence(PlayerTypes ePlayer) const;
	void SetNumTimesTheyLoweredOurInfluence(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesTheyLoweredOurInfluence(PlayerTypes ePlayer, int iChange);

	int GetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iChange);

	int GetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iChange);

	int GetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iChange);

	int GetNegativeReligiousConversionPoints(PlayerTypes ePlayer) const;
	void SetNegativeReligiousConversionPoints(PlayerTypes ePlayer, int iValue);
	void ChangeNegativeReligiousConversionPoints(PlayerTypes ePlayer, int iChange);

	int GetNumTimesRobbedBy(PlayerTypes ePlayer) const;
	void SetNumTimesRobbedBy(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesRobbedBy(PlayerTypes ePlayer, int iChange);

	int GetNumTimesPerformedCoupAgainstUs(PlayerTypes ePlayer) const;
	void SetNumTimesPerformedCoupAgainstUs(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesPerformedCoupAgainstUs(PlayerTypes ePlayer, int iChange);

	int GetNegativeArchaeologyPoints(PlayerTypes ePlayer) const;
	void SetNegativeArchaeologyPoints(PlayerTypes ePlayer, int iValue);
	void ChangeNegativeArchaeologyPoints(PlayerTypes ePlayer, int iChange);

	int GetNumArtifactsEverDugUp(PlayerTypes ePlayer) const;
	void SetNumArtifactsEverDugUp(PlayerTypes ePlayer, int iValue);
	void ChangeNumArtifactsEverDugUp(PlayerTypes ePlayer, int iChange);

	int GetNumTimesNuked(PlayerTypes ePlayer) const;
	void SetNumTimesNuked(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesNuked(PlayerTypes ePlayer, int iChange);

	// ------------------------------------
	// Turn counters
	// ------------------------------------

	bool WasResurrectedBy(PlayerTypes ePlayer) const;
	bool WasResurrectedThisTurnBy(PlayerTypes ePlayer) const;
	void SetResurrectedBy(PlayerTypes ePlayer, bool bValue);
	bool WasResurrectedByAnyone() const;

	int GetLiberatedCitiesTurn(PlayerTypes ePlayer) const;
	void SetLiberatedCitiesTurn(PlayerTypes ePlayer, int iChange);

	int GetCiviliansReturnedToMeTurn(PlayerTypes ePlayer) const;
	void SetCiviliansReturnedToMeTurn(PlayerTypes ePlayer, int iChange);

	int GetIntrigueSharedTurn(PlayerTypes ePlayer) const;
	void SetIntrigueSharedTurn(PlayerTypes ePlayer, int iChange);

	int GetForgaveForSpyingTurn(PlayerTypes ePlayer) const;
	void SetForgaveForSpyingTurn(PlayerTypes ePlayer, int iChange);

	int GetLandmarksBuiltForMeTurn(PlayerTypes ePlayer) const;
	void SetLandmarksBuiltForMeTurn(PlayerTypes ePlayer, int iChange);

	int GetPlottedAgainstUsTurn(PlayerTypes ePlayer) const;
	void SetPlottedAgainstUsTurn(PlayerTypes ePlayer, int iChange);
	
	int GetPlunderedTradeRouteTurn(PlayerTypes ePlayer) const;
	void SetPlunderedTradeRouteTurn(PlayerTypes ePlayer, int iChange);

	// They sided with their Protected Minor (after we attacked/bullied it)
	int GetTurnsSinceOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer) const;
	void SetOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iChange);
	bool IsOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer) const;

	int GetTurnsSincePlayerBulliedProtectedMinor(PlayerTypes eBullyPlayer) const;

	int GetTurnsSincePlayerAttackedProtectedMinor(PlayerTypes ePlayer) const;
	void SetOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iChange);

	int GetTurnsSincePlayerKilledProtectedMinor(PlayerTypes ePlayer) const;
	void SetOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iChange);

	int GetReligiousConversionTurn(PlayerTypes ePlayer) const;
	void SetReligiousConversionTurn(PlayerTypes ePlayer, int iChange);

	int GetRobbedTurn(PlayerTypes ePlayer) const;
	void SetRobbedTurn(PlayerTypes ePlayer, int iChange);
	
	int GetPerformedCoupTurn(PlayerTypes ePlayer) const;
	void SetPerformedCoupTurn(PlayerTypes ePlayer, int iChange);

	int GetTurnsSinceWeLikedTheirProposal(PlayerTypes ePlayer);
	void SetTurnsSinceWeLikedTheirProposal(PlayerTypes ePlayer, int iValue);
	void ChangeTurnsSinceWeLikedTheirProposal(PlayerTypes ePlayer, int iChange);

	int GetTurnsSinceWeDislikedTheirProposal(PlayerTypes ePlayer);
	void SetTurnsSinceWeDislikedTheirProposal(PlayerTypes ePlayer, int iValue);
	void ChangeTurnsSinceWeDislikedTheirProposal(PlayerTypes ePlayer, int iChange);

	int GetTurnsSinceTheySupportedOurProposal(PlayerTypes ePlayer);
	void SetTurnsSinceTheySupportedOurProposal(PlayerTypes ePlayer, int iValue);
	void ChangeTurnsSinceTheySupportedOurProposal(PlayerTypes ePlayer, int iChange);

	int GetTurnsSinceTheyFoiledOurProposal(PlayerTypes ePlayer);
	void SetTurnsSinceTheyFoiledOurProposal(PlayerTypes ePlayer, int iValue);
	void ChangeTurnsSinceTheyFoiledOurProposal(PlayerTypes ePlayer, int iChange);

	int GetTurnsSinceTheySupportedOurHosting(PlayerTypes ePlayer);
	void SetTurnsSinceTheySupportedOurHosting(PlayerTypes ePlayer, int iValue);
	void ChangeTurnsSinceTheySupportedOurHosting(PlayerTypes ePlayer, int iChange);

	// ------------------------------------
	// Player-Specific Memory Values
	// ------------------------------------

	PlayerTypes GetOtherPlayerProtectedMinorBullied(PlayerTypes eBullyPlayer) const;
	PlayerTypes GetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer) const;
	PlayerTypes GetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer) const;

	// ------------------------------------
	// Guesses
	// ------------------------------------

	// Guesses about other players' feelings towards us
	MajorCivOpinionTypes GetOpinionTowardsUsGuess(PlayerTypes ePlayer) const;
	void SetOpinionTowardsUsGuess(PlayerTypes ePlayer, MajorCivOpinionTypes eOpinion);
	MajorCivApproachTypes GetApproachTowardsUsGuess(PlayerTypes ePlayer) const;
	void SetApproachTowardsUsGuess(PlayerTypes ePlayer, MajorCivApproachTypes eApproach);
	int GetApproachTowardsUsGuessCounter(PlayerTypes ePlayer) const;
	void SetApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iValue);
	void ChangeApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iChange);

	// Guesses about other players' interactions with the world
	// Other Player War Value Lost: the int value of damage others have inflicted UPON ePlayer in war
	int GetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const;
	void SetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue);
	void ChangeOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iChange);

	int GetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const;
	void SetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue);

	void CacheOtherPlayerWarValuesLost();

	WarDamageLevelTypes GetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const;
	void SetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, WarDamageLevelTypes eDamageLevel);
	ThreatTypes GetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const;
	void SetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eWithPlayer, ThreatTypes eThreatType);

	// ------------------------------------
	// C4DF Values
	// ------------------------------------

	bool IsShareOpinionAccepted(PlayerTypes ePlayer) const;
	void SetShareOpinionAccepted(PlayerTypes ePlayer, bool bValue);

	short GetShareOpinionCounter(PlayerTypes ePlayer) const;
	void SetShareOpinionCounter(PlayerTypes ePlayer, int iValue);
	void ChangeShareOpinionCounter(PlayerTypes ePlayer, int iChange);
	bool IsShareOpinionTooSoon(PlayerTypes ePlayer) const;

	bool IsPlayerMoveTroopsRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerMoveTroopsRequestAccepted(PlayerTypes ePlayer, bool bValue);

	int GetPlayerMoveTroopsRequestCounter(PlayerTypes ePlayer) const;
	void SetPlayerMoveTroopsRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerMoveTroopsRequestCounter(PlayerTypes ePlayer, int iChange);
	bool IsTooSoonForMoveTroopsRequest(PlayerTypes ePlayer) const;

	bool IsOfferingGift(PlayerTypes ePlayer) const;	// We're offering a gift!
	void SetOfferingGift(PlayerTypes ePlayer, bool bValue);

	bool IsOfferedGift(PlayerTypes ePlayer) const;	// We offered a gift!
	void SetOfferedGift(PlayerTypes ePlayer, bool bValue);

	bool IsHelpRequestEverMade(PlayerTypes ePlayer) const;
	void SetHelpRequestEverMade(PlayerTypes ePlayer, bool bValue);

	short GetHelpRequestCounter(PlayerTypes ePlayer) const;
	void SetHelpRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangeHelpRequestCounter(PlayerTypes ePlayer, int iChange);

	short GetHelpRequestTooSoonNumTurns(PlayerTypes ePlayer) const;
	bool IsHelpRequestTooSoon(PlayerTypes ePlayer) const;

	bool IsHasPaidTributeTo(PlayerTypes ePlayer) const;

	int GetNumTimesDemandedWhileVassal(PlayerTypes ePlayer) const;
	void SetNumTimesDemandedWhileVassal(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesDemandedWhileVassal(PlayerTypes ePlayer, int iChange);

	int GetVassalProtectValue(PlayerTypes ePlayer) const;
	void SetVassalProtectValue(PlayerTypes ePlayer, int iValue);
	void ChangeVassalProtectValue(PlayerTypes ePlayer, int iChange);

	int GetVassalFailedProtectValue(PlayerTypes ePlayer) const;
	void SetVassalFailedProtectValue(PlayerTypes ePlayer, int iValue);
	void ChangeVassalFailedProtectValue(PlayerTypes ePlayer, int iChange);

	bool IsMasterLiberatedMeFromVassalage(PlayerTypes ePlayer) const;
	void SetMasterLiberatedMeFromVassalage(PlayerTypes ePlayer, bool bValue);

	int GetTurnsSinceVassalagePeacefullyRevoked(PlayerTypes ePlayer) const;
	void SetTurnsSinceVassalagePeacefullyRevoked(PlayerTypes ePlayer, int iValue);
	void ChangeTurnsSinceVassalagePeacefullyRevoked(PlayerTypes ePlayer, int iChange);

	int GetTurnsSinceVassalageForcefullyRevoked(PlayerTypes ePlayer) const;
	void SetTurnsSinceVassalageForcefullyRevoked(PlayerTypes ePlayer, int iValue);
	void ChangeTurnsSinceVassalageForcefullyRevoked(PlayerTypes ePlayer, int iChange);

	bool IsPlayerBrokenVassalAgreement(PlayerTypes ePlayer) const;
	void SetPlayerBrokenVassalAgreement(PlayerTypes ePlayer, bool bValue);

	int GetPlayerBrokenVassalAgreementTurn(PlayerTypes ePlayer) const;
	void SetPlayerBrokenVassalAgreementTurn(PlayerTypes ePlayer, int iValue);

	bool IsVassalTaxRaised(PlayerTypes ePlayer) const;
	void SetVassalTaxRaised(PlayerTypes ePlayer, bool bValue);

	bool IsVassalTaxLowered(PlayerTypes ePlayer) const;
	void SetVassalTaxLowered(PlayerTypes ePlayer, bool bValue);

	// How much gold has our vassal collected since we've known him?
	int GetVassalGoldPerTurnCollectedSinceVassalStarted(PlayerTypes ePlayer) const;
	void SetVassalGoldPerTurnCollectedSinceVassalStarted(PlayerTypes ePlayer, int iValue);
	void ChangeVassalGoldPerTurnCollectedSinceVassalStarted(PlayerTypes ePlayer, int iChange);

	// How much gold have we taxed from him since we've known him?
	int GetVassalGoldPerTurnTaxedSinceVassalStarted(PlayerTypes ePlayer) const;
	void SetVassalGoldPerTurnTaxedSinceVassalStarted(PlayerTypes ePlayer, int iValue);
	void ChangeVassalGoldPerTurnTaxedSinceVassalStarted(PlayerTypes ePlayer, int iChange);

	// ************************************
	// Turn Stuff
	// ************************************

	void DoTurn(DiplomacyPlayerType eTargetPlayer);
	void DoCounters();

	/////////////////////////////////////////////////////////
	// Opinion
	/////////////////////////////////////////////////////////

	void DoUpdateOpinions();
	void DoUpdateOnePlayerOpinion(PlayerTypes ePlayer);

	int GetMajorCivOpinionWeight(PlayerTypes ePlayer);

	//void DoUpdateOpinionTowardsUsGuesses();

	/////////////////////////////////////////////////////////
	// Approach
	/////////////////////////////////////////////////////////

	// Major Civs
	void DoUpdateMajorCivApproaches(vector<PlayerTypes>& vPlayersToReevaluate, bool bBetweenTurnsUpdate = true);
	void SelectBestApproachTowardsMajorCiv(PlayerTypes ePlayer, bool bFirstPass, vector<PlayerTypes>& vPlayersToUpdate, vector<PlayerTypes>& vPlayersToReevaluate, std::map<PlayerTypes, MajorCivApproachTypes>& oldApproaches, bool bBetweenTurnsUpdate = true);
	
	// Special case approach updates
	void DoUpdateApproachTowardsTeammate(PlayerTypes ePlayer);
	void DoUpdateAlwaysWarApproachTowardsMajorCiv(PlayerTypes ePlayer);
	void DoUpdateHumanApproachTowardsMajorCiv(PlayerTypes ePlayer);
	void DoUpdateMajorCivApproachIfWeHaveNoCities(PlayerTypes ePlayer);
	void DoUpdateMajorCivApproachIfTheyHaveNoCities(PlayerTypes ePlayer);

	// Minor Civs
	void DoUpdateMinorCivApproaches();
	MinorCivApproachTypes GetBestApproachTowardsMinorCiv(PlayerTypes ePlayer, bool bLookAtOtherPlayers, std::map<PlayerTypes, MinorCivApproachTypes>& oldApproaches, bool bLog);

	bool IsHasActiveGoldQuest();

	// Our guess as to another player's approach towards us
	MajorCivApproachTypes GetVisibleApproachTowardsUs(PlayerTypes ePlayer) const;

	void DoUpdateApproachTowardsUsGuesses();
	void DoEstimateOtherPlayerApproaches();

	/////////////////////////////////////////////////////////
	// Demands
	/////////////////////////////////////////////////////////

	void DoUpdateDemands();

	void DoStartDemandProcess(PlayerTypes ePlayer);
	void DoCancelHaltDemandProcess();

	void DoTestDemandReady();

	bool IsPlayerDemandAttractive(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Requests
	/////////////////////////////////////////////////////////

	bool IsMakeRequest(PlayerTypes ePlayer, CvDeal* pDeal, bool& bRandPassed);
	bool IsLuxuryRequest(PlayerTypes ePlayer, CvDeal* pDeal, int& iWeightBias);
	bool IsGoldRequest(PlayerTypes ePlayer, CvDeal* pDeal, int& iWeightBias);

	bool IsEmbassyExchangeAcceptable(PlayerTypes ePlayer);
	bool WantsEmbassyAtPlayer(PlayerTypes ePlayer);
	bool IsWantsOpenBordersWithPlayer(PlayerTypes ePlayer);
	bool IsWillingToGiveOpenBordersToPlayer(PlayerTypes ePlayer);
	bool IsOpenBordersExchangeAcceptable(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Peace
	/////////////////////////////////////////////////////////

	void DoMakePeaceWithMinors();
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	void DoMakePeaceWithVassals();
#endif
	void DoUpdatePeaceTreatyWillingness();

	// Do we want peace with ePlayer? (looks at WantPeaceCounter)
	bool IsWantsPeaceWithPlayer(PlayerTypes ePlayer) const;

	/////////////////////////////////////////////////////////
	// War & Military Assessment
	/////////////////////////////////////////////////////////

	void MakeWar();
	bool DeclareWar(PlayerTypes ePlayer);
	bool DeclareWar(TeamTypes eTeam);
	
	// Easy Target: Is this player an easy attack target?
	void DoUpdateEasyTargets();
	bool DoTestOnePlayerEasyTarget(PlayerTypes ePlayer);

	// Set default values when we're attacked and its not our turn
	void DoSomeoneDeclaredWarOnMe(TeamTypes eTeam);
	void DoUpdateWarProjections();

	int GetHighestWarscore(bool bOnlyCurrentWars = true);
	PlayerTypes GetHighestWarscorePlayer(bool bOnlyCurrentWars = true);

	int GetWarScore(PlayerTypes ePlayer, bool bUsePeacetimeCalculation = false, bool bDebug = false);
	void DoUpdateWarStates();
	void DoUpdateWarGoals();

	int CountUnitsAroundEnemyCities(PlayerTypes ePlayer, int iTurnRange) const;
	bool HasVisibilityOfEnemyCityDanger(CvCity* pCity) const;

	// Military Rating: How skilled is ePlayer at war?
	int ComputeAverageMajorMilitaryRating(PlayerTypes eExcludedPlayer = NO_PLAYER);
	int ComputeRatingStrengthAdjustment(PlayerTypes ePlayer);

	// Military Strength: How strong is ePlayer compared to US?
	void DoUpdatePlayerMilitaryStrengths();

	// Economic Strength: How strong is ePlayer compared to US?
	void DoUpdatePlayerEconomicStrengths();

	// Target Value: how easy or hard of a target would ePlayer be to attack?
	void DoUpdatePlayerTargetValues();
	void DoUpdateOnePlayerTargetValue(PlayerTypes ePlayer);
	
	// War Sanity Checks: What would we lose if we went to war?
	int CalculateGoldPerTurnLostFromWar(PlayerTypes ePlayer, bool bOtherPlayerEstimate, bool bIgnoreDPs);
	bool IsWarWouldBackstabFriend(PlayerTypes ePlayer);
	bool IsWarWouldBackstabFriendTeamCheck(PlayerTypes ePlayer);

	void DoUpdateWarDamageLevels();
	void DoWarDamageDecay();

	void DoUpdateOtherPlayerWarDamageLevels();

	void DoUpdateMilitaryAggressivePostures();
	void DoUpdateOnePlayerMilitaryAggressivePosture(PlayerTypes ePlayer);

	AggressivePostureTypes GetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer) const;
	void SetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);

	/////////////////////////////////////////////////////////
	// Threats to this Player
	/////////////////////////////////////////////////////////

	void DoUpdateMilitaryThreats();

	void DoUpdateEstimateOtherPlayerMilitaryThreats();

	// Warmonger Threat - how much of a threat are these guys to run amok and break everything
	void DoUpdateWarmongerThreats(bool bUpdateOnly = false);

	int GetNumberOfThreatenedCities(PlayerTypes eEnemy);

	/////////////////////////////////////////////////////////
	// Planning Exchanges
	/////////////////////////////////////////////////////////

	void DoUpdatePlanningExchanges();

	void DoAddWantsResearchAgreementWithPlayer(PlayerTypes ePlayer);
	void DoCancelWantsResearchAgreementWithPlayer(PlayerTypes ePlayer);
	bool IsCanMakeResearchAgreementRightNow(PlayerTypes ePlayer);

	bool IsPhonyWar(PlayerTypes ePlayer, bool bFromApproachSelection = false) const;
	bool IsWantsToConquer(PlayerTypes ePlayer) const;
	bool IsPotentialMilitaryTargetOrThreat(PlayerTypes ePlayer, bool bFromApproachSelection = false) const;
	
	bool IsGoodChoiceForDoF(PlayerTypes ePlayer);

	bool IsCanMakeDefensivePactRightNow(PlayerTypes ePlayer);

	bool IsGoodChoiceForDefensivePact(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Issues of Dispute
	/////////////////////////////////////////////////////////

	// Land Dispute
	void DoUpdateLandDisputeLevels();

	bool IsPlayerRecklessExpander(PlayerTypes ePlayer);
	bool IsPlayerWonderSpammer(PlayerTypes ePlayer);

	bool IsNukedBy(PlayerTypes ePlayer) const;
	bool IsAngryAboutProtectedMinorKilled(PlayerTypes ePlayer) const;
	bool IsAngryAboutProtectedMinorAttacked(PlayerTypes ePlayer) const;
	bool IsAngryAboutProtectedMinorBullied(PlayerTypes ePlayer) const;
	bool IsAngryAboutSidedWithTheirProtectedMinor(PlayerTypes ePlayer) const;

	// Expansion Aggressive Posture: How aggressively has ePlayer positioned their Units in relation to us?
	void DoUpdateExpansionAggressivePostures();
	void DoUpdateOnePlayerExpansionAggressivePosture(PlayerTypes ePlayer);
	pair<int,int> GetClosestCityPair(PlayerTypes ePlayer);

	void DoUpdatePlotBuyingAggressivePostures();

	// Victory Block
	void DoUpdateVictoryBlockLevels();

	void DoRelationshipPairing();
	
	bool IsStrategicTradePartner(PlayerTypes ePlayer) const;
	bool IsMajorCompetitor(PlayerTypes ePlayer) const;
	bool IsEarlyGameCompetitor(PlayerTypes ePlayer);

	bool IsIgnorePolicyDifferences(PlayerTypes ePlayer) const;
	bool IsIgnoreReligionDifferences(PlayerTypes ePlayer) const;
	bool IsIgnoreIdeologyDifferences(PlayerTypes ePlayer) const;

	// Victory Dispute
	void DoUpdateVictoryDisputeLevels();

	// Wonder Dispute
	void DoUpdateWonderDisputeLevels();

	// Minor Civ Diplo Dispute
	void DoUpdateMinorCivDisputeLevels();
	bool IsMinorCivTroublemaker(PlayerTypes ePlayer, bool bIgnoreBullying = false) const;
	bool IsPrimeLeagueCompetitor(PlayerTypes ePlayer) const;
	
	// Tech Dispute (for scientific civs)
	DisputeLevelTypes GetTechDisputeLevel(PlayerTypes ePlayer) const;
	
	// Diplomacy AI Options
	bool IsCompetingForVictory() const;
	bool IsEndgameAggressive() const;
	bool IsEndgameAggressiveTo(PlayerTypes ePlayer) const;
	bool ShouldHideDisputeMods(PlayerTypes ePlayer) const;
	bool ShouldHideNegativeMods(PlayerTypes ePlayer) const;

	/////////////////////////////////////////////////////////
	// Evaluation of Other Players' Tendencies
	/////////////////////////////////////////////////////////

	// Someone had some kind of interaction with another player

	void DoWeMadePeaceWithSomeone(TeamTypes eOtherTeam);
	void DoPlayerDeclaredWarOnSomeone(PlayerTypes ePlayer, TeamTypes eOtherTeam, bool bDefensivePact);
	void DoPlayerKilledSomeone(PlayerTypes ePlayer, PlayerTypes eDeadPlayer);
	void DoPlayerBulliedSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);
	void DoPlayerMetSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);

	// Protected Minor Attacked
	void SetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer, PlayerTypes eAttackedPlayer);

	// Protected Minors Killed
	void SetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer, PlayerTypes eKilledPlayer);

	// Protected Minor Bullied
	

	int GetOtherPlayerWarmongerScore(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Contact
	/////////////////////////////////////////////////////////

	void DoFirstContact(PlayerTypes ePlayer);
	void DoFirstContactInitRelationship(PlayerTypes ePlayer);

	void DoKilledByPlayer(PlayerTypes ePlayer);

	void DoSendStatementToPlayer(PlayerTypes ePlayer, DiploStatementTypes eMessage, int iData1, CvDeal* pDeal);

	void DoMakePublicDeclaration(PublicDeclarationTypes eDeclaration, int iData1 = -1, int iData2 = -1, PlayerTypes eMustHaveMetPlayer = NO_PLAYER, PlayerTypes eForSpecificPlayer = NO_PLAYER);

	void DoContactMajorCivs();
	void DoContactPlayer(PlayerTypes ePlayer);

	void DoContactMinorCivs();

	// Possible contact options follow:

	void DoUpdateMinorCivProtection(PlayerTypes eMinor, MinorCivApproachTypes eApproach);

	void DoCoopWarTimeStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoCoopWarStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);

	void DoMakeDemand(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);

	void DoAggressiveMilitaryStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoKilledCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoAttackedCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoBulliedCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	//void DoSeriousExpansionWarningStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoExpansionWarningStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoExpansionBrokenPromiseStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	//void DoSeriousPlotBuyingWarningStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoPlotBuyingWarningStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoPlotBuyingBrokenPromiseStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

	void DoWeAttackedYourMinorStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoWeBulliedYourMinorStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);

	void DoCaughtYourSpyStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoKilledYourSpyStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoKilledMySpyStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

	void DoShareIntrigueStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

	void DoConvertedMyCityStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoDugUpMyYardStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

	void DoDoFStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoEndDoFStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoDenounceFriendStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoRequestFriendDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);

	void DoLuxuryTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoEmbassyExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoEmbassyOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoOpenBordersExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoOpenBordersOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoResearchAgreementOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoStrategicTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoDefensivePactOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoCityExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoThirdPartyWarTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoThirdPartyPeaceTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoVoteTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoRenewExpiredDeal(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);

	void DoRequest(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoGift(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);

	//void DoNowUnforgivableStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	//void DoNowEnemyStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);

	void DoHostileStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	//void DoFriendlyStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoAfraidStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoWarmongerStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoMinorCivCompetitionStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1, bool bIgnoreTurnsBetweenLimit = false);

	void DoAngryBefriendedEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoAngryDenouncedFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoHappyDenouncedEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoHappyBefriendedFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoFYIBefriendedHumanEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoFYIDenouncedHumanFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoFYIDenouncedHumanEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoFYIBefriendedHumanFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoHappySamePolicyTree(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoIdeologicalStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoVictoryCompetitionStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoVictoryBlockStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

	void DoWeLikedTheirProposal(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoWeDislikedTheirProposal(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoTheySupportedOurProposal(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoTheyFoiledOurProposal(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoTheySupportedOurHosting(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

	void DoPeaceOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);

	/////////////////////////////////////////////////////////
	// Diplo stuff relating to UI
	/////////////////////////////////////////////////////////

	void DoBeginDiploWithHuman();
	void DoBeginDiploWithHumanEspionageResult();
	void DoBeginDiploWithHumanInDiscuss();

	const char* GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer = NO_PLAYER);
	const char* GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer, const Localization::String& strOptionalKey1);
	const char* GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer, const Localization::String& strOptionalKey1, const Localization::String& strOptionalKey2);
	
	void DoFromUIDiploEvent(PlayerTypes eFromPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2);

	bool IsActHostileTowardsHuman(PlayerTypes eHuman, bool bIgnoreCurrentWar = false) const;

	const char* GetGreetHumanMessage(LeaderheadAnimationTypes& eAnimation);
	const char* GetInsultHumanMessage();

	const char* GetOfferText(PlayerTypes ePlayer);
	int	GetDenounceMessage(PlayerTypes ePlayer);
	const char* GetDenounceMessageValue(int iValue);

	const char* GetAttackedByHumanMessage();
	const char* GetWarMessage(PlayerTypes ePlayer);
	const char* GetEndDoFMessage(PlayerTypes ePlayer);
	const char* GetEndWorkAgainstSomeoneMessage(PlayerTypes ePlayer, const Localization::String& strAgainstPlayerKey);

	const char* GetDiploTextFromTag(const char* strTag);
	const char* GetDiploTextFromTag(const char* strTag, const Localization::String& strOptionalKey1);
	const char* GetDiploTextFromTag(const char* strTag, const Localization::String& strOptionalKey1, const Localization::String& strOptionalKey2);

	/////////////////////////////////////////////////////////
	// Things a player has told this AI
	/////////////////////////////////////////////////////////

	// Coop Wars
	bool IsValidCoopWarTarget(PlayerTypes eTargetPlayer, bool bIgnoreCanDeclareWar);
	bool CanRequestCoopWar(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer);
	bool CanStartCoopWar(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer);

	void DoUpdateCoopWarStates();
	void DoStartCoopWar(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer);

	bool DoTestCoopWarDesire(PlayerTypes ePlayer, PlayerTypes& eChosenTargetPlayer);
	int GetCoopWarDesireScore(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer);

	CoopWarStates RespondToCoopWarRequest(PlayerTypes eAskingPlayer, PlayerTypes eTargetPlayer);
	bool IsCoopWarRequestUnacceptable(PlayerTypes eAskingPlayer, PlayerTypes eTargetPlayer);
	void DoWarnCoopWarTarget(PlayerTypes eAskingPlayer, PlayerTypes eTargetPlayer);

	void CancelCoopWarsAgainstPlayer(PlayerTypes ePlayer);
	void CancelCoopWarsWithPlayer(PlayerTypes ePlayer, bool bPenalty);
	void CancelAllCoopWars();

	// Human Demand
	void DoDemandMade(PlayerTypes ePlayer, DemandResponseTypes eDemand);

	// No Settling
	bool IsDontSettleAcceptable(PlayerTypes ePlayer) const;

	// Stop Spying
	bool IsStopSpyingAcceptable(PlayerTypes ePlayer) const;

	// Working With Player
	bool IsDoFAcceptable(PlayerTypes ePlayer);
	bool IsTooEarlyForDoF(PlayerTypes ePlayer);

	int GetNumRA();
	int GetNumDefensePacts();

	int GetNumSamePolicies(PlayerTypes ePlayer);

	bool IsDenounceFriendAcceptable(PlayerTypes ePlayer);

	bool IsPlayerDoFWithAnyFriend(PlayerTypes ePlayer) const;
	bool IsPlayerDoFWithAnyEnemy(PlayerTypes ePlayer) const;
	bool IsPlayerDPWithAnyFriend(PlayerTypes ePlayer) const;
	bool IsPlayerDPWithAnyEnemy(PlayerTypes ePlayer) const;

	// Religion
	bool IsPlayerSameReligion(PlayerTypes ePlayer) const;
	bool IsPlayerOpposingReligion(PlayerTypes ePlayer) const;

	// Ideology
	bool IsPlayerSameIdeology(PlayerTypes ePlayer) const;
	bool IsPlayerOpposingIdeology(PlayerTypes ePlayer) const;

	// Denounced Player
	void DoDenouncePlayer(PlayerTypes ePlayer);

	bool IsDenounceAcceptable(PlayerTypes ePlayer, bool bBias = false);
	int GetDenounceWeight(PlayerTypes ePlayer, bool bBias);

	bool IsPlayerDenouncedFriend(PlayerTypes ePlayer) const;
	bool IsPlayerDenouncedEnemy(PlayerTypes ePlayer) const;

	// Requests of Friends
	PlayerTypes GetRequestFriendToDenounce(PlayerTypes ePlayer, bool& bRandFailed);
	bool IsFriendDenounceRefusalUnacceptable(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer);

	void DoTestUntrustworthyFriends();
	bool DoTestOnePlayerUntrustworthyFriend(PlayerTypes ePlayer);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// Contact Statements
	void DoMapsOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoTechOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoGenerousOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);

	// Requests
	bool IsTechRequest(PlayerTypes ePlayer, CvDeal* pDeal, int& iWeightBias);

	bool WantsMapsFromPlayer(PlayerTypes ePlayer);

	// Offers
	bool IsMakeGenerousOffer(PlayerTypes ePlayer, CvDeal* pDeal, bool& bRandPassed);
	bool IsGoldGenerousOffer(PlayerTypes ePlayer, CvDeal* pDeal);
	bool IsLuxuryGenerousOffer(PlayerTypes ePlayer, CvDeal* pDeal);
	bool IsTechGenerousOffer(PlayerTypes ePlayer, CvDeal* pDeal);

	// Sharing Opinion
	bool IsShareOpinionAcceptable(PlayerTypes ePlayer);
	bool IsTooEarlyForShareOpinion(PlayerTypes ePlayer);

	// Help Request
	void DoHelpRequestMade(PlayerTypes ePlayer, DemandResponseTypes eResponse);

	// Vassals
	void DoVassalTaxChanged(TeamTypes eTeam, bool bTaxesLowered);

	VassalTreatmentTypes GetVassalTreatmentLevel(PlayerTypes ePlayer) const;
	CvString GetVassalTreatmentToolTip(PlayerTypes ePlayer) const;

	int GetVassalScore(PlayerTypes ePlayer) const;
	int GetMasterScore(PlayerTypes ePlayer) const;

	int GetVassalTreatedScore(PlayerTypes ePlayer) const;
	int GetVassalDemandScore(PlayerTypes ePlayer) const;
	int GetVassalTaxScore(PlayerTypes ePlayer) const;
	int GetVassalTradeRouteScore(PlayerTypes ePlayer) const;
	int GetVassalReligionScore(PlayerTypes ePlayer) const;

	bool IsWantToLiberateVassal(PlayerTypes ePlayer) const;
	int GetMasterLiberatedMeFromVassalageScore(PlayerTypes ePlayer) const;

	int GetVassalProtectScore(PlayerTypes ePlayer) const;
	int GetHappyAboutVassalagePeacefullyRevokedScore(PlayerTypes ePlayer) const;
	int GetAngryAboutVassalageForcefullyRevokedScore(PlayerTypes ePlayer) const;

	int GetTooManyVassalsScore(PlayerTypes ePlayer) const;
	int GetSameMasterScore(PlayerTypes ePlayer) const;

	int GetBrokenVassalAgreementScore(PlayerTypes ePlayer) const;
	
	int GetVassalFailedProtectScore(PlayerTypes ePlayer) const;

	bool IsVassalageAcceptable(PlayerTypes ePlayer, bool bWar = false);

	bool IsEndVassalageAcceptable(PlayerTypes ePlayer);
	bool IsEndVassalageRequestAcceptable(PlayerTypes eHuman);
	void DoBecomeVassalageStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoMakeVassalageStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoEndVassalageStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoRevokeVassalageStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);

	void DoLiberateMyVassalStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoDetermineTaxRateForVassals();
	void DoDetermineTaxRateForVassalOnePlayer(PlayerTypes ePlayer);

	void DoVassalTaxesRaisedStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoVassalTaxesLoweredStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

	bool IsHappyAboutPlayerVassalagePeacefullyRevoked(PlayerTypes ePlayer) const;
	bool IsAngryAboutPlayerVassalageForcefullyRevoked(PlayerTypes ePlayer) const;

	void DoWeMadeVassalageWithSomeone(TeamTypes eTeam, bool bVoluntary);
	void DoWeEndedVassalageWithSomeone(TeamTypes eTeam);

	//GlobalStateTypes GetGlobalState(PlayerTypes ePlayer) const;
	//void SetGlobalState(PlayerTypes ePlayer, GlobalStateTypes eGlobalState);

	//void DoUpdateGlobalStates();
	//void DoUpdateGlobalStateForOnePlayer(PlayerTypes ePlayer);
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	MoveTroopsResponseTypes GetMoveTroopsRequestResponse(PlayerTypes ePlayer, bool bJustChecking = false);
	void DoLiberatedFromVassalage(TeamTypes eTeam);
#endif

	/////////////////////////////////////////////////////////
	// A Player's adherence to statements made to this AI
	/////////////////////////////////////////////////////////

	void DoTestPromises();

	// Player asks the AI not to convert
	bool IsStopSpreadingReligionAcceptable(PlayerTypes ePlayer);

	// Player asks the AI not to dig
	bool IsStopDiggingAcceptable(PlayerTypes ePlayer) const;

	/////////////////////////////////////////////////////////
	// Opinion modifiers
	/////////////////////////////////////////////////////////
	int GetBaseOpinionScore(PlayerTypes ePlayer);
	int GetLandDisputeLevelScore(PlayerTypes ePlayer);
	int GetWonderDisputeLevelScore(PlayerTypes ePlayer);
	int GetMinorCivDisputeLevelScore(PlayerTypes ePlayer);
	int GetTechDisputeLevelScore(PlayerTypes ePlayer);
	int GetVictoryDisputeLevelScore(PlayerTypes ePlayer);
	int GetVictoryBlockLevelScore(PlayerTypes ePlayer);
	int GetWarmongerThreatScore(PlayerTypes ePlayer);
	int GetCiviliansReturnedToMeScore(PlayerTypes ePlayer);
	int GetLandmarksBuiltForMeScore(PlayerTypes ePlayer);
	int GetResurrectedScore(PlayerTypes ePlayer);
	int GetLiberatedCapitalScore(PlayerTypes ePlayer);
	int GetLiberatedCitiesScore(PlayerTypes ePlayer);
	int GetEmbassyScore(PlayerTypes ePlayer);
	int GetForgaveForSpyingScore(PlayerTypes ePlayer);
	int GetNoSettleRequestScore(PlayerTypes ePlayer);
	int GetStopSpyingRequestScore(PlayerTypes ePlayer);
	int GetDemandEverMadeScore(PlayerTypes ePlayer);
	int GetTimesCultureBombedScore(PlayerTypes ePlayer);
	int GetReligiousConversionPointsScore(PlayerTypes ePlayer);
	int GetReligionScore(PlayerTypes ePlayer);
	int GetIdeologyScore(PlayerTypes ePlayer);
	int GetTimesRobbedScore(PlayerTypes ePlayer);
	int GetTradeRoutesPlunderedScore(PlayerTypes ePlayer);
	int GetTimesPlottedAgainstUsScore(PlayerTypes ePlayer);
	int GetTimesPerformedCoupScore(PlayerTypes ePlayer);
	int GetDugUpMyYardScore(PlayerTypes ePlayer);
	int GetTimesIntrigueSharedScore(PlayerTypes ePlayer);
	int GetBrokenMilitaryPromiseScore(PlayerTypes ePlayer);
	int GetBrokenMilitaryPromiseWithAnybodyScore(PlayerTypes ePlayer);
	int GetBrokenExpansionPromiseScore(PlayerTypes ePlayer);
	int GetIgnoredExpansionPromiseScore(PlayerTypes ePlayer);
	int GetBrokenBorderPromiseScore(PlayerTypes ePlayer);
	int GetIgnoredBorderPromiseScore(PlayerTypes ePlayer);
	int GetBrokenAttackCityStatePromiseScore(PlayerTypes ePlayer);
	int GetBrokenAttackCityStatePromiseWithAnybodyScore(PlayerTypes ePlayer);
	int GetIgnoredAttackCityStatePromiseScore(PlayerTypes ePlayer);
	int GetBrokenBullyCityStatePromiseScore(PlayerTypes ePlayer);
	int GetIgnoredBullyCityStatePromiseScore(PlayerTypes ePlayer);
	int GetBrokenNoConvertPromiseScore(PlayerTypes ePlayer);
	int GetIgnoredNoConvertPromiseScore(PlayerTypes ePlayer);
	int GetBrokenNoDiggingPromiseScore(PlayerTypes ePlayer);
	int GetIgnoredNoDiggingPromiseScore(PlayerTypes ePlayer);
	int GetBrokenSpyPromiseScore(PlayerTypes ePlayer);
	int GetIgnoredSpyPromiseScore(PlayerTypes ePlayer);
	int GetBrokenCoopWarPromiseScore(PlayerTypes ePlayer);
	int GetAngryAboutProtectedMinorKilledScore(PlayerTypes ePlayer);
	int GetAngryAboutProtectedMinorAttackedScore(PlayerTypes ePlayer);
	int GetAngryAboutProtectedMinorBulliedScore(PlayerTypes ePlayer);
	int GetAngryAboutSidedWithProtectedMinorScore(PlayerTypes ePlayer);
	int GetDOFAcceptedScore(PlayerTypes ePlayer);
	int GetDOFWithAnyFriendScore(PlayerTypes ePlayer);
	int GetDOFWithAnyEnemyScore(PlayerTypes ePlayer);
	int GetResearchAgreementScore(PlayerTypes ePlayer);
	int GetDPAcceptedScore(PlayerTypes ePlayer);
	int GetDPWithAnyFriendScore(PlayerTypes ePlayer);
	int GetDPWithAnyEnemyScore(PlayerTypes ePlayer);
	int GetOpenBordersScore(PlayerTypes ePlayer);
	MajorCivOpinionTypes GetNeighborOpinion(PlayerTypes ePlayer) const;
	bool MusteringForNeighborAttack(PlayerTypes ePlayer) const;
	int GetFriendDenouncementScore(PlayerTypes ePlayer);
	int GetWeDenouncedFriendScore(PlayerTypes ePlayer);
	int GetFriendDenouncedUsScore(PlayerTypes ePlayer);
	int GetWeDeclaredWarOnFriendScore(PlayerTypes ePlayer);
	int GetFriendDeclaredWarOnUsScore(PlayerTypes ePlayer);
	int GetMutualDenouncementScore(PlayerTypes ePlayer);
	int GetDenouncedUsScore(PlayerTypes ePlayer);
	int GetDenouncedThemScore(PlayerTypes ePlayer);
	int GetDenouncedFriendScore(PlayerTypes ePlayer);
	int GetDenouncedEnemyScore(PlayerTypes ePlayer);
	int GetRecklessExpanderScore(PlayerTypes ePlayer);
	int GetWonderSpammerScore(PlayerTypes ePlayer);
	int GetRecentTradeScore(PlayerTypes ePlayer);
	int GetCommonFoeScore(PlayerTypes ePlayer);
	int GetRecentAssistScore(PlayerTypes ePlayer);
	int GetNukedByScore(PlayerTypes ePlayer);
	int GetCitiesRazedScore(PlayerTypes ePlayer);
	int GetCitiesRazedGlobalScore(PlayerTypes ePlayer);
	int GetPtPSameCSScore(PlayerTypes ePlayer);
	int GetPolicyScore(PlayerTypes ePlayer);
	int GetCapitalCapturedByScore(PlayerTypes ePlayer);
	int GetHolyCityCapturedByScore(PlayerTypes ePlayer);
	int GetLikedTheirProposalScore(PlayerTypes ePlayer);
	int GetDislikedTheirProposalScore(PlayerTypes ePlayer);
	int GetSupportedMyProposalScore(PlayerTypes ePlayer);
	int GetFoiledMyProposalScore(PlayerTypes ePlayer);
	int GetSupportedMyHostingScore(PlayerTypes ePlayer);
#if defined(MOD_EVENTS_DIPLO_MODIFIERS)
	int GetDiploModifiers(PlayerTypes ePlayer, std::vector<Opinion>& aOpinions);
#endif
	int GetScenarioModifier1(PlayerTypes ePlayer);
	int GetScenarioModifier2(PlayerTypes ePlayer);
	int GetScenarioModifier3(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Miscellaneous
	/////////////////////////////////////////////////////////

	bool DoPossibleMinorLiberation(PlayerTypes eMinor, int iCityID);
	bool DoPossibleMajorLiberation(PlayerTypes eMajor, PlayerTypes eOldOwner, CvCity* pCity);

	bool IsPlayerBadTheftTarget(PlayerTypes ePlayer, TheftTypes eTheftType, const CvPlot* pPlot = NULL);

	int GetNumOurEnemiesPlayerAtWarWith(PlayerTypes ePlayer);

	bool IsGoingForWorldConquest() const;
	bool IsGoingForDiploVictory() const;
	bool IsGoingForCultureVictory() const;
	bool IsGoingForSpaceshipVictory() const;

	bool IsCloseToAnyVictoryCondition() const;
	bool IsCloseToSSVictory() const;
	bool IsCloseToDominationVictory() const;
	bool IsCloseToCultureVictory() const;
	bool IsCloseToDiploVictory() const;

	// Minor Civ Log
	void LogMinorCivGiftTile(PlayerTypes ePlayer);
	void LogMinorCivGiftGold(PlayerTypes ePlayer, int iOldFriendship, int iGold, bool bSaving, bool bWantQuickBoost, PlayerTypes ePlayerTryingToPass);
	void LogMinorCivBullyGold(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, int iGold, bool bSuccess, int iBullyMetricScore);
	void LogMinorCivBullyUnit(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, UnitTypes eUnit, bool bSuccess, int iBullyMetricScore);
	void LogMinorCivBullyHeavy(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, YieldTypes eYield, int iValue, bool bSuccess, int iBullyMetricScore);
	void LogMinorCivQuestReceived(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType);
	void LogMinorCivQuestFinished(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType);
	void LogMinorCivQuestCancelled(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType);
	void LogMinorCivBuyout(PlayerTypes eMinor, int iGoldPaid, bool bSaving);

	CvDeal* GetDealToRenew(PlayerTypes eOtherPlayer = NO_PLAYER);
	void CleanupRenewDeals(PlayerTypes eOtherPlayer);
	void CancelRenewDeal(PlayerTypes eOtherPlayer = NO_PLAYER, RenewalReason eReason = NO_REASON, bool bJustLogging = false);

	void KilledPlayerCleanup (PlayerTypes eKilledPlayer);

	// Methods for injecting tests
	void TestUIDiploStatement(PlayerTypes eToPlayer, DiploStatementTypes eStatement, int iArg1);

	void LogOpenEmbassy(PlayerTypes ePlayer);
	void LogCloseEmbassy(PlayerTypes ePlayer);

private:
	bool IsValidUIDiplomacyTarget(PlayerTypes eTargetPlayer);

	void DoMakeWarOnPlayer(PlayerTypes eTargetPlayer);

	void LogPublicDeclaration(PublicDeclarationTypes eDeclaration, int iData1, PlayerTypes eForSpecificPlayer = NO_PLAYER);
	void LogWarDeclaration(PlayerTypes ePlayer, int iTotalWarWeight = -1);
	void LogPeaceMade(PlayerTypes ePlayer);
	void LogDoF(PlayerTypes ePlayer);
	void LogBrokenDoF(PlayerTypes ePlayer);
	void LogDenounce(PlayerTypes ePlayer, bool bBackstab = false, bool bRefusal = false);
	void LogFriendRequestDenounce(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bAgreed);
	void LogCoopWar(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, CoopWarStates eState);
	void LogWantRA(PlayerTypes ePlayer);
	void LogWantDP(PlayerTypes ePlayer);

	void LogOpinionUpdate(PlayerTypes ePlayer, std::vector<int>& viOpinionValues);
	void LogApproachValueDeltas(PlayerTypes ePlayer, const int* aiApproachValues, const int* aiScratchValues);
	void LogMajorCivWarmongerUpdate(PlayerTypes ePlayer, int iValue, bool bUpdateLogsSpecial);

	void LogMajorCivApproachUpdate(PlayerTypes ePlayer, const int* aiApproachValues, MajorCivApproachTypes eNewMajorCivApproach, MajorCivApproachTypes eOldApproach, WarFaceTypes eNewWarFace);
	void LogMinorCivApproachUpdate(PlayerTypes ePlayer, const int* aiApproachValues, MinorCivApproachTypes eNewMinorCivApproach, MinorCivApproachTypes eOldApproach);
	void LogPersonality();
	void LogStatus();
	void LogWarStatus();
	void LogStatements();

	void LogOtherPlayerExpansionGuess(PlayerTypes ePlayer, int iGuess);
	void LogOtherPlayerGuessStatus();

	void LogGrandStrategy(CvString& strString);

	void LogMajorCivApproach(CvString& strString, MajorCivApproachTypes eNewMajorCivApproach, WarFaceTypes eNewWarFace);
	void LogMinorCivApproach(CvString& strString, MinorCivApproachTypes eNewMajorCivApproach);
	void LogMinorCivQuestType(CvString& strString, MinorCivQuestTypes eQuestType);
	void LogOpinion(CvString& strString, PlayerTypes ePlayer);
	void LogWarmongerThreat(CvString& strString, PlayerTypes ePlayer);
	void LogMilitaryThreat(CvString& strString, PlayerTypes ePlayer);
	void LogMilitaryStrength(CvString& strString, PlayerTypes ePlayer);
	void LogTargetValue(CvString& strString, PlayerTypes ePlayer);
	void LogEconomicStrength(CvString& strString, PlayerTypes ePlayer);
	void LogWarGoal(CvString& strString, PlayerTypes ePlayer);
	void LogWarPeaceWillingToOffer(CvString& strString, PlayerTypes ePlayer);
	void LogWarPeaceWillingToAccept(CvString& strString, PlayerTypes ePlayer);
	void LogWarState(CvString& strString, PlayerTypes ePlayer);
	void LogWarProjection(CvString& strString, PlayerTypes ePlayer);
	void LogWarDamage(CvString& strString, PlayerTypes ePlayer);
	void LogMilitaryAggressivePosture(CvString& strString, PlayerTypes ePlayer);
	void LogExpansionAggressivePosture(CvString& strString, PlayerTypes ePlayer);
	void LogPlotBuyingAggressivePosture(CvString& strString, PlayerTypes ePlayer);
	void LogLandDispute(CvString& strString, PlayerTypes ePlayer);
	void LogVictoryDispute(CvString& strString, PlayerTypes ePlayer);
	void LogVictoryBlock(CvString& strString, PlayerTypes ePlayer);
	void LogWonderDispute(CvString& strString, PlayerTypes ePlayer);
	void LogMinorCivDispute(CvString& strString, PlayerTypes ePlayer);
	void LogProximity(CvString& strString, PlayerTypes ePlayer);

	void LogStatementToPlayer(PlayerTypes ePlayer, DiploStatementTypes eMessage);

	CvPlayer* m_pPlayer;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool IsCapitulationAcceptable(PlayerTypes ePlayer);
	bool IsVoluntaryVassalageAcceptable(PlayerTypes ePlayer);
	//void LogGlobalState(CvString& strString, PlayerTypes ePlayer);
#endif

	// Need a string member so that it doesn't go out of scope after translation
	Localization::String m_strDiploText;

	//-----------------------------------------------------
	// Structure to hold the bulk of the diplo data
	//-----------------------------------------------------
	struct DiplomacyAIData
	{
		DiplomacyAIData();

		//Arrays
		short m_aDiploLogStatementTurnCountScratchPad[NUM_DIPLO_LOG_STATEMENT_TYPES];
		char m_aeMajorCivOpinion[MAX_MAJOR_CIVS];
		char m_aeMajorCivApproach[MAX_MAJOR_CIVS];
		char m_aeMajorCivStrategicApproach[MAX_MAJOR_CIVS];
		char m_aeMinorCivApproach[REALLY_MAX_PLAYERS-MAX_MAJOR_CIVS];
		char m_aeOpinionTowardsUsGuess[MAX_MAJOR_CIVS];
		char m_aeApproachTowardsUsGuess[MAX_MAJOR_CIVS];
		char m_aeApproachTowardsUsGuessCounter[MAX_MAJOR_CIVS];
		short m_aeWantPeaceCounter[REALLY_MAX_PLAYERS];
		char m_aePeaceTreatyWillingToOffer[MAX_MAJOR_CIVS];
		char m_aePeaceTreatyWillingToAccept[MAX_MAJOR_CIVS];
		short m_aiNumWondersBeatenTo[REALLY_MAX_PLAYERS];
		bool m_abArmyInPlaceForAttack[REALLY_MAX_PLAYERS];
		bool m_abEasyTarget[REALLY_MAX_PLAYERS];
		bool m_abWantsResearchAgreementWithPlayer[MAX_MAJOR_CIVS];
		bool m_abWantsDoFWithPlayer[MAX_MAJOR_CIVS];
		bool m_abWantsDefensivePactWithPlayer[MAX_MAJOR_CIVS];
		bool m_abAggressor[REALLY_MAX_PLAYERS];
		bool m_abWantsSneakAttack[MAX_MAJOR_CIVS];
		bool m_abWantToRouteToMinor[REALLY_MAX_PLAYERS-MAX_MAJOR_CIVS];
		char m_aeWarFace[MAX_MAJOR_CIVS];
		char m_aeWarState[REALLY_MAX_PLAYERS];
		char m_aeWarProjection[REALLY_MAX_PLAYERS];
		char m_aeLastWarProjection[REALLY_MAX_PLAYERS];
		char m_aeWarGoal[REALLY_MAX_PLAYERS];
		short m_aiPlayerNumTurnsAtWar[REALLY_MAX_PLAYERS];
		short m_aiPlayerNumTurnsAtPeace[REALLY_MAX_PLAYERS];
		short m_aiPlayerNumTurnsSinceCityCapture[REALLY_MAX_PLAYERS];
		short m_aiNumWarsFought[REALLY_MAX_PLAYERS];
		short m_aiNumWarsDeclaredOnUs[REALLY_MAX_PLAYERS];
		char m_aePlayerMilitaryStrengthComparedToUs[REALLY_MAX_PLAYERS];
		char m_aePlayerEconomicStrengthComparedToUs[REALLY_MAX_PLAYERS];
		char m_aePlayerTargetValue[REALLY_MAX_PLAYERS];
		char m_aePlayerLandDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aePlayerLastTurnLandDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aePlayerVictoryDisputeLevel[REALLY_MAX_PLAYERS];

		char m_aePlayerVictoryBlockLevel[MAX_MAJOR_CIVS];
		bool m_abCantMatchDeal[MAX_MAJOR_CIVS];

		char m_aePlayerWonderDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aePlayerMinorCivDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aeMilitaryAggressivePosture[REALLY_MAX_PLAYERS];
		char m_aeLastTurnMilitaryAggressivePosture[REALLY_MAX_PLAYERS];
		char m_aeExpansionAggressivePosture[REALLY_MAX_PLAYERS];
		char m_aePlotBuyingAggressivePosture[REALLY_MAX_PLAYERS];
		char m_aeMilitaryThreat[REALLY_MAX_PLAYERS];
		char m_aeWarDamageLevel[REALLY_MAX_PLAYERS];
		int m_aiWarValueLost[REALLY_MAX_PLAYERS];
		char m_aeWarmongerThreat[REALLY_MAX_PLAYERS];
		char m_aiPersonalityMajorCivApproachBiases[NUM_MAJOR_CIV_APPROACHES];
		char m_aiPersonalityMinorCivApproachBiases[NUM_MINOR_CIV_APPROACHES];
		DeclarationLogData m_aDeclarationsLog[MAX_DIPLO_LOG_STATEMENTS];

		// Things a player has told the AI

		bool m_abPlayerNoSettleRequest[MAX_MAJOR_CIVS];
		short m_aiPlayerNoSettleRequestCounter[MAX_MAJOR_CIVS];

		bool m_abPlayerStopSpyingRequest[MAX_MAJOR_CIVS];
		short m_aiPlayerStopSpyingRequestCounter[MAX_MAJOR_CIVS];
		short m_aiPlayerBackstabCounter[MAX_MAJOR_CIVS];

		short m_aiDefensivePactValue[MAX_MAJOR_CIVS];
		short m_aiDoFValue[MAX_MAJOR_CIVS];
		short m_aiCompetitorValue[MAX_MAJOR_CIVS];
		char m_aiCoopWarScore[MAX_MAJOR_CIVS];

		short m_aiDemandCounter[MAX_MAJOR_CIVS];
		short m_aiDemandTooSoonNumTurns[MAX_MAJOR_CIVS];

		bool m_abDoFAccepted[MAX_MAJOR_CIVS];

		bool m_abDoFBroken[MAX_MAJOR_CIVS];
		char m_aeDoFType[MAX_MAJOR_CIVS];
		short m_aiBrokenMilitaryPromiseTurn[MAX_MAJOR_CIVS];
		short m_aiBrokenAttackCityStatePromiseTurn[MAX_MAJOR_CIVS];
		short m_aiDoFBrokenTurn[MAX_MAJOR_CIVS];
		bool m_abEverBackstabbedBy[MAX_MAJOR_CIVS];
		short m_aiFriendDenouncedUsTurn[MAX_MAJOR_CIVS];
		short m_aiFriendDeclaredWarOnUsTurn[MAX_MAJOR_CIVS];
		pair<int,int> m_paNoExpansionPromise[MAX_MAJOR_CIVS];
		pair<int,int> m_paLastTurnEmpireDistance[MAX_MAJOR_CIVS];

		short m_aiDoFCounter[MAX_MAJOR_CIVS];

		bool m_abUntrustworthyFriend[MAX_MAJOR_CIVS];
		bool m_abFriendDenouncedUs[MAX_MAJOR_CIVS];
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		bool m_abOfferingGift[MAX_MAJOR_CIVS];
		bool m_abOfferedGift[MAX_MAJOR_CIVS];
#endif
		bool m_abFriendDeclaredWarOnUs[MAX_MAJOR_CIVS];
		short m_aiDenouncedPlayerTurn[MAX_MAJOR_CIVS];

		short m_aiNumCiviliansReturnedToMe[MAX_MAJOR_CIVS];
		short m_aiNumLandmarksBuiltForMe[MAX_MAJOR_CIVS];
		short m_aiResurrectedOnTurn[MAX_MAJOR_CIVS];
		short m_aiNumTimesCultureBombed[MAX_MAJOR_CIVS];

		short m_aiNegativeReligiousConversionPoints[MAX_MAJOR_CIVS];
		short m_aiNegativeArchaeologyPoints[MAX_MAJOR_CIVS];
		short m_aiNumCitiesCaptured[REALLY_MAX_PLAYERS];
		short m_aiNumTimesRazed[REALLY_MAX_PLAYERS];
		short m_aiNumTradeRoutesPlundered[REALLY_MAX_PLAYERS];
		bool m_abPlayerEverMadeBorderPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerEverMadeExpansionPromise[MAX_MAJOR_CIVS];
		bool m_abDoFEverAsked[MAX_MAJOR_CIVS];
		bool m_abHelpRequestEverMade[MAX_MAJOR_CIVS];
		int m_aiDemandEverMade[MAX_MAJOR_CIVS];
		short m_aiDemandMadeTurn[MAX_MAJOR_CIVS];
		int m_aiArtifactsEverDugUp[MAX_MAJOR_CIVS];
		bool m_abEverConvertedCity[MAX_MAJOR_CIVS];
		bool m_abPlayerNoSettleRequestEverAsked[MAX_MAJOR_CIVS];
		bool m_abPlayerStopSpyingRequestEverAsked[MAX_MAJOR_CIVS];
		short m_aiNumLandmarksBuiltForMeTurn[MAX_MAJOR_CIVS];
		short m_aiCiviliansReturnedToMeTurn[MAX_MAJOR_CIVS];
		short m_aiWarDamageValue[MAX_MAJOR_CIVS];
		short m_aiPlayerForgaveForSpyingTurn[MAX_MAJOR_CIVS];
		short m_aiLiberatedCitiesTurn[MAX_MAJOR_CIVS];
		short m_aiIntrigueSharedTurn[MAX_MAJOR_CIVS];
		short m_aiReligiousConversionTurn[MAX_MAJOR_CIVS];
		short m_aiPlunderedTradeRouteTurn[MAX_MAJOR_CIVS];
		short m_aiTimesRobbedTurn[MAX_MAJOR_CIVS];
		short m_aiPlottedAgainstUsTurn[MAX_MAJOR_CIVS];
		short m_aiPerformedCoupTurn[MAX_MAJOR_CIVS];
		short m_aiNumTimesNuked[MAX_MAJOR_CIVS];
		short m_aiNumTimesRobbedBy[MAX_MAJOR_CIVS];
		short m_aiNumTimesIntrigueSharedBy[MAX_MAJOR_CIVS];

		short m_aiBrokenExpansionPromiseValue[MAX_MAJOR_CIVS];
		short m_aiIgnoredExpansionPromiseValue[MAX_MAJOR_CIVS];
		short m_aiBrokenBorderPromiseValue[MAX_MAJOR_CIVS];
		short m_aiIgnoredBorderPromiseValue[MAX_MAJOR_CIVS];

		bool m_abPlayerLiberatedCapital[MAX_MAJOR_CIVS];
		bool m_abPlayerCapturedCapital[MAX_MAJOR_CIVS];
		bool m_abPlayerCapturedHolyCity[MAX_MAJOR_CIVS];
		short m_aiNumCitiesLiberated[MAX_MAJOR_CIVS];

		short m_aiTradeValue[MAX_MAJOR_CIVS];
		short m_aiCommonFoeValue[MAX_MAJOR_CIVS];
		short m_aiAssistValue[MAX_MAJOR_CIVS];

		// Player's response to AI statements
		bool m_abPlayerMadeMilitaryPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenMilitaryPromise[MAX_MAJOR_CIVS];
		short m_aiPlayerMilitaryPromiseCounter[MAX_MAJOR_CIVS];

		short m_aiPlayerMadeExpansionPromiseTurn[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenExpansionPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredExpansionPromise[MAX_MAJOR_CIVS];
		char m_aePlayerExpansionPromiseData[MAX_MAJOR_CIVS];

		short m_aiPlayerMadeBorderPromiseTurn[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenBorderPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredBorderPromise[MAX_MAJOR_CIVS];
		char m_aePlayerBorderPromiseData[MAX_MAJOR_CIVS];

		bool m_abPlayerMadeAttackCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenAttackCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredAttackCityStatePromise[MAX_MAJOR_CIVS];

		bool m_abPlayerMadeBullyCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenBullyCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredBullyCityStatePromise[MAX_MAJOR_CIVS];

		bool m_abPlayerMadeSpyPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenSpyPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredSpyPromise[MAX_MAJOR_CIVS];

		bool m_abPlayerForgaveForSpying[MAX_MAJOR_CIVS];

		bool m_abPlayerMadeNoConvertPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenNoConvertPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredNoConvertPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerAskedNotToConvert[MAX_MAJOR_CIVS];
		bool m_abPlayerAgreedNotToConvert[MAX_MAJOR_CIVS];

		bool m_abPlayerMadeNoDiggingPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenNoDiggingPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredNoDiggingPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerAskedNotToDig[MAX_MAJOR_CIVS];
		bool m_abPlayerAgreedNotToDig[MAX_MAJOR_CIVS];

		bool m_abPlayerBrokenCoopWarPromise[MAX_MAJOR_CIVS];

		// Evaluation of Other Players' Tendencies
		short m_aiOtherPlayerTurnsSinceAttackedProtectedMinor[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerProtectedMinorAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumProtectedMinorsAttacked[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceKilledProtectedMinor[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerProtectedMinorKilled[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumProtectedMinorsKilled[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumProtectedMinorsBullied[MAX_MAJOR_CIVS];

		short m_aiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[MAX_MAJOR_CIVS];
		
		char m_aiOtherPlayerNumMinorsAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMinorsConquered[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMajorsAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMajorsConquered[MAX_MAJOR_CIVS];
		int m_aiOtherPlayerWarmongerAmountTimes100[MAX_MAJOR_CIVS];

		char m_aiTheyPlottedAgainstUs[MAX_MAJOR_CIVS];
		char m_aiTheyLoweredOurInfluence[MAX_MAJOR_CIVS];
		char m_aiPerformedCoupAgainstUs[MAX_MAJOR_CIVS];

		short m_aiOtherPlayerTurnsSinceWeLikedTheirProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceWeDislikedTheirProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceTheySupportedOurProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceTheyFoiledOurProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceTheySupportedOurHosting[MAX_MAJOR_CIVS];

		//2D Arrays
		int* m_apaeApproachValues[MAX_MAJOR_CIVS];
		char* m_apaeOtherPlayerWarDamageLevel[REALLY_MAX_PLAYERS];
		int* m_apaiOtherPlayerWarValueLost[REALLY_MAX_PLAYERS];
		int* m_apaiOtherPlayerLastRoundWarValueLost[REALLY_MAX_PLAYERS];
		bool* m_apabSentAttackMessageToMinorCivProtector[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerMilitaryThreat[REALLY_MAX_PLAYERS];
		DiploLogData* m_apaDiploStatementsLog[MAX_MAJOR_CIVS];

		char* m_apaeCoopWarState[MAX_MAJOR_CIVS];
		short* m_apaiCoopWarStateChangeTurn[MAX_MAJOR_CIVS];

		int m_aaeApproachValues[MAX_MAJOR_CIVS* NUM_MAJOR_CIV_APPROACHES];
		char m_aaeOtherPlayerWarDamageLevel[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		int m_aaiOtherPlayerWarValueLost[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		int m_aaiOtherPlayerLastRoundWarValueLost[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		bool m_aabSentAttackMessageToMinorCivProtector[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		char m_aaeOtherPlayerMilitaryThreat[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		DiploLogData m_aaDiploStatementsLog[MAX_MAJOR_CIVS* MAX_DIPLO_LOG_STATEMENTS];

		char m_aaeCoopWarState[MAX_MAJOR_CIVS* MAX_MAJOR_CIVS];
		short m_aaiCoopWarStateChangeTurn[MAX_MAJOR_CIVS* MAX_MAJOR_CIVS];
#if defined(MOD_ACTIVE_DIPLOMACY)
		float m_aTradePriority[MAX_MAJOR_CIVS]; // current ai to human trade priority
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		bool m_abShareOpinionAccepted[MAX_MAJOR_CIVS];
		short m_aiShareOpinionCounter[MAX_MAJOR_CIVS];

		short m_aiHelpRequestCounter[MAX_MAJOR_CIVS];
		short m_aiHelpRequestTooSoonNumTurns[MAX_MAJOR_CIVS];

		short m_aiNumTimesDemandedWhenVassal[MAX_MAJOR_CIVS];
		bool m_abDemandAcceptedWhenVassal[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenVassalAgreement[MAX_MAJOR_CIVS];
		short m_aiBrokenVassalAgreementTurn[MAX_MAJOR_CIVS];

		short m_aiPlayerVassalageFailedProtectValue[MAX_MAJOR_CIVS];
		short m_aiPlayerVassalageProtectValue[MAX_MAJOR_CIVS];
		short m_aiPlayerVassalageTurnsSincePeacefullyRevokedVassalage[MAX_MAJOR_CIVS];
		short m_aiPlayerVassalageTurnsSinceForcefullyRevokedVassalage[MAX_MAJOR_CIVS];

		bool m_abMoveTroopsRequestAccepted[MAX_MAJOR_CIVS];
		short m_aiMoveTroopsRequestCounter[MAX_MAJOR_CIVS];
		
		bool m_abMasterLiberatedMeFromVassalage[MAX_MAJOR_CIVS];

		bool m_abVassalTaxRaised[MAX_MAJOR_CIVS];
		bool m_abVassalTaxLowered[MAX_MAJOR_CIVS];

		int m_aiVassalGoldPerTurnTaxedSinceVassalStarted[MAX_MAJOR_CIVS];
		int m_aiVassalGoldPerTurnCollectedSinceVassalStarted[MAX_MAJOR_CIVS];
#endif
	};
	DiplomacyAIData* m_pDiploData;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool* m_pabShareOpinionAccepted;
	short* m_paiShareOpinionCounter;

	short* m_paiHelpRequestCounter;
	short* m_paiHelpRequestTooSoonNumTurns;

	short* m_paiPlayerVassalageFailedProtectValue;
	short* m_paiPlayerVassalageProtectValue;

	short* m_paiPlayerVassalageTurnsSincePeacefullyRevokedVassalage;
	short* m_paiPlayerVassalageTurnsSinceForcefullyRevokedVassalage;

	short* m_paiNumTimesDemandedWhenVassal;
	bool* m_pabDemandAcceptedWhenVassal;
	bool* m_pabPlayerBrokenVassalAgreement;
	short* m_paiBrokenVassalAgreementTurn;

	bool* m_pabMoveTroopsRequestAccepted;
	short* m_paiMoveTroopsRequestCounter;

	bool* m_pabMasterLiberatedMeFromVassalage;

	bool* m_pabVassalTaxRaised;
	bool* m_pabVassalTaxLowered;

	int* m_paiVassalGoldPerTurnTaxedSinceVassalStarted;
	int* m_paiVassalGoldPerTurnCollectedSinceVassalStarted;

#endif

	// Scratch pad to keep track of Diplo Messages we've sent out in the past
	short* m_paDiploLogStatementTurnCountScratchPad;

	char* m_paeMajorCivOpinion;
	int** m_ppaaeApproachValues;

	char* m_paeMajorCivApproach;
	char* m_paeMajorCivStrategicApproach;
	char* m_paeMinorCivApproach;
	char* m_paeOpinionTowardsUsGuess;
	char* m_paeApproachTowardsUsGuess;
	char* m_paeApproachTowardsUsGuessCounter;

	char m_eDiploPersonalityType;

	char m_eDemandTargetPlayer;
	bool m_bDemandReady;

	char m_eCSWarTarget;
	char m_eCSBullyTarget;

	short* m_paeWantPeaceCounter;
	char* m_paePeaceTreatyWillingToOffer;
	char* m_paePeaceTreatyWillingToAccept;

	short* m_paiNumWondersBeatenTo;
	bool* m_pabArmyInPlaceForAttack;
	bool* m_pabEasyTarget;

	bool* m_pabWantsResearchAgreementWithPlayer;

	bool* m_pabWantsDoFWithPlayer;
	bool* m_pabWantsDefensivePactWithPlayer;
	bool* m_pabAggressor;
	bool* m_pabWantsSneakAttack;

	short* m_paiDefensivePactValue;
	short* m_paiDoFValue;
	short* m_paiCompetitorValue;
	char* m_paiCoopWarScore;

	bool* m_pabWantToRouteToMinor;

	char* m_paeWarFace;
	char* m_paeWarState;
	char* m_paeWarProjection;
	char* m_paeLastWarProjection;
	char* m_paeWarGoal;
	short* m_paiPlayerNumTurnsAtWar;
	short* m_paiPlayerNumTurnsAtPeace;
	short* m_paiPlayerNumTurnsSinceCityCapture;
	short* m_paiWarDamageValue;
	short* m_paiNumWarsFought;
	short* m_paiNumWarsDeclaredOnUs;

	char* m_paePlayerMilitaryStrengthComparedToUs;
	char* m_paePlayerEconomicStrengthComparedToUs;
	char* m_paePlayerTargetValue;

	char* m_paeWarDamageLevel;
	int* m_paiWarValueLost;
	char** m_ppaaeOtherPlayerWarDamageLevel;
	int** m_ppaaiOtherPlayerWarValueLost;
	int** m_ppaaiOtherPlayerLastRoundWarValueLost;
	bool** m_ppaabSentAttackMessageToMinorCivProtector;

	char* m_paeMilitaryAggressivePosture;
	char* m_paeLastTurnMilitaryAggressivePosture;
	char* m_paeExpansionAggressivePosture;
	char* m_paePlotBuyingAggressivePosture;

	char* m_paePlayerLandDisputeLevel;
	char* m_paePlayerLastTurnLandDisputeLevel;
	char* m_paePlayerVictoryDisputeLevel;

	char* m_paePlayerVictoryBlockLevel;
	bool* m_pabCantMatchDeal;

	char* m_paePlayerWonderDisputeLevel;
	char* m_paePlayerMinorCivDisputeLevel;

	char* m_paeMilitaryThreat;
	char** m_ppaaeOtherPlayerMilitaryThreat;

	char* m_paeWarmongerThreat;

	// Things a player has told the AI

	bool* m_pabPlayerNoSettleRequestAccepted;
	short* m_paiPlayerNoSettleRequestCounter;

	bool* m_pabPlayerStopSpyingRequestAccepted;
	short* m_paiPlayerStopSpyingRequestCounter;

	short* m_paiPlayerBackstabCounter;

	short* m_paiDemandCounter;
	short* m_paiDemandTooSoonNumTurns;

	bool* m_pabDoFAccepted;

	bool* m_pabDoFBroken;
	char* m_paeDoFType;
	short* m_paiBrokenMilitaryPromiseTurn;
	short* m_paiBrokenAttackCityStatePromiseTurn;
	short* m_paiDoFBrokenTurn;
	bool* m_pabEverBackstabbedBy;
	short* m_paiFriendDenouncedUsTurn;
	short* m_paiFriendDeclaredWarOnUsTurn;
	pair<int,int>* m_paNoExpansionPromise;
	pair<int,int>* m_paLastTurnEmpireDistance;

	short* m_paiDoFCounter;

	bool* m_pabUntrustworthyFriend;
	bool* m_pabFriendDenouncedUs;
	bool* m_pabFriendDeclaredWarOnUs;
	short* m_paiDenouncedPlayerTurn;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool* m_pabOfferingGift;
	bool* m_pabOfferedGift;
#endif

	short* m_paiNumCiviliansReturnedToMe;
	short* m_paiNumLandmarksBuiltForMe;
	short* m_paiResurrectedOnTurn; // slewis - the "resurrected by" player liberated the city of an otherwise dead player and brought them back into the game on the given turn
	short* m_paiNumTimesCultureBombed;
	short* m_paiNegativeReligiousConversionPoints;
	short* m_paiNegativeArchaeologyPoints;

	short* m_paiNumCitiesCaptured;
	short* m_paiNumTimesRazed;
	short* m_paiNumTradeRoutesPlundered;
	short* m_paiDemandMadeTurn;
	short* m_paiNumLandmarksBuiltForMeTurn;
	short* m_paiCiviliansReturnedToMeTurn;
	short* m_paiPlayerForgaveForSpyingTurn;
	short* m_paiLiberatedCitiesTurn;
	short* m_paiIntrigueSharedTurn;
	short* m_paiReligiousConversionTurn;
	short* m_paiPlunderedTradeRouteTurn;
	short* m_paiTimesRobbedTurn;
	short* m_paiPlottedAgainstUsTurn;
	short* m_paiPerformedCoupTurn;
	short* m_paiNumTimesNuked;
	short* m_paiNumTimesRobbedBy;
	short* m_paiNumTimesIntrigueSharedBy;

	short* m_paiBrokenExpansionPromiseValue;
	short* m_paiIgnoredExpansionPromiseValue;
	short* m_paiBrokenBorderPromiseValue;
	short* m_paiIgnoredBorderPromiseValue;
	short* m_paiTradeValue;
	short* m_paiCommonFoeValue;
	short* m_paiAssistValue;

	bool* m_pabPlayerLiberatedCapital;
	bool* m_pabPlayerCapturedCapital;
	bool* m_pabPlayerCapturedHolyCity;
	short* m_paiNumCitiesLiberated;

	char** m_ppaaeCoopWarState;
	short** m_ppaaiCoopWarStateChangeTurn;

	// Player's response to AI statements
	bool* m_pabPlayerMadeMilitaryPromise;
	bool* m_pabPlayerBrokenMilitaryPromise;
	short* m_paiPlayerMilitaryPromiseCounter;

	short* m_paiPlayerMadeExpansionPromiseTurn;
	bool* m_pabPlayerBrokenExpansionPromise;
	bool* m_pabPlayerIgnoredExpansionPromise;
	char* m_paePlayerExpansionPromiseData;

	short* m_paiPlayerMadeBorderPromiseTurn;
	bool* m_pabPlayerBrokenBorderPromise;
	bool* m_pabPlayerIgnoredBorderPromise;
	char* m_paePlayerBorderPromiseData;
	bool* m_pabPlayerEverMadeBorderPromise;
	bool* m_pabPlayerEverMadeExpansionPromise;
	bool* m_pabDoFEverAsked;
	bool* m_pabHelpRequestEverMade;
	int*  m_paiDemandEverMade;
	int*  m_paiArtifactsEverDugUp;
	bool* m_pabEverConvertedCity;
	bool* m_pabPlayerNoSettleRequestEverAsked;
	bool* m_pabPlayerStopSpyingRequestEverAsked;

	bool* m_pabPlayerMadeAttackCityStatePromise;
	bool* m_pabPlayerBrokenAttackCityStatePromise;
	bool* m_pabPlayerIgnoredAttackCityStatePromise;

	bool* m_pabPlayerMadeBullyCityStatePromise;
	bool* m_pabPlayerBrokenBullyCityStatePromise;
	bool* m_pabPlayerIgnoredBullyCityStatePromise;

	bool* m_pabPlayerMadeSpyPromise;
	bool* m_pabPlayerBrokenSpyPromise;
	bool* m_pabPlayerIgnoredSpyPromise;

	bool* m_pabPlayerForgaveForSpying;

	bool* m_pabPlayerMadeNoConvertPromise;
	bool* m_pabPlayerBrokenNoConvertPromise;
	bool* m_pabPlayerIgnoredNoConvertPromise;
	bool* m_pabPlayerAskedNotToConvert;
	bool* m_pabPlayerAgreedNotToConvert;

	bool* m_pabPlayerMadeNoDiggingPromise;
	bool* m_pabPlayerBrokenNoDiggingPromise;
	bool* m_pabPlayerIgnoredNoDiggingPromise;
	bool* m_pabPlayerAskedNotToDig;
	bool* m_pabPlayerAgreedNotToDig;

	bool* m_pabPlayerBrokenCoopWarPromise;

	// Personality Members

	char m_iVictoryCompetitiveness;
	char m_iWonderCompetitiveness;
	char m_iMinorCivCompetitiveness;
	char m_iBoldness;
	char m_iDiploBalance;
	char m_iWarmongerHate;
	char m_iDenounceWillingness;
	char m_iDoFWillingness;
	char m_iLoyalty;
	char m_iNeediness;
	char m_iForgiveness;
	char m_iChattiness;
	char m_iMeanness;

	char* m_paiPersonalityMajorCivApproachBiases;
	char* m_paiPersonalityMinorCivApproachBiases;

	// Evaluation of Other Players' Tendencies

	short* m_paiOtherPlayerTurnsSinceAttackedProtectedMinor;
	char* m_paiOtherPlayerProtectedMinorAttacked;
	char* m_paiOtherPlayerNumProtectedMinorsAttacked;

	short* m_paiOtherPlayerTurnsSinceKilledProtectedMinor;
	char* m_paiOtherPlayerProtectedMinorKilled;
	char* m_paiOtherPlayerNumProtectedMinorsKilled;

	char* m_paiOtherPlayerNumProtectedMinorsBullied;

	short* m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor;

	char* m_paiOtherPlayerNumMinorsAttacked;
	char* m_paiOtherPlayerNumMinorsConquered;
	char* m_paiOtherPlayerNumMajorsAttacked;
	char* m_paiOtherPlayerNumMajorsConquered;
	int* m_paiOtherPlayerWarmongerAmountTimes100;

	char* m_paiTheyPlottedAgainstUs;
	char* m_paiTheyLoweredOurInfluence;
	char* m_paiPerformedCoupAgainstUs;

	short* m_paiOtherPlayerTurnsSinceWeLikedTheirProposal;
	short* m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal;
	short* m_paiOtherPlayerTurnsSinceTheySupportedOurProposal;
	short* m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal;
	short* m_paiOtherPlayerTurnsSinceTheySupportedOurHosting;

	// Memory of what we've talked about with people
	DiploLogData** m_ppaDiploStatementsLog;

	DeclarationLogData* m_paDeclarationsLog;

	// Overall status across all other civs
	StateAllWars m_eStateAllWars;

	// Other
	typedef std::vector<PlayerTypes> PlayerTypesArray;
	PlayerTypesArray	m_aGreetPlayers;
	// JdH =>
	void DoUpdateHumanTradePriority(PlayerTypes ePlayer, int iOpinionWeight);
	// JdH <=
	DiplomacyPlayerType	m_eTargetPlayer;
#if defined(MOD_ACTIVE_DIPLOMACY)
	DiplomacyPlayerType	m_eTargetPlayerType;
#endif
	// Data members for injecting test messages
	PlayerTypes			m_eTestToPlayer;
	DiploStatementTypes m_eTestStatement;
	int					m_iTestStatementArg1;
};

namespace CvDiplomacyAIHelpers
{
	int GetWarmongerOffset(CvCity* pCity = NULL, PlayerTypes eWarmonger = NO_PLAYER, PlayerTypes ePlayer = NO_PLAYER, WarmongerTriggerTypes eWarmongerTrigger = NO_WARMONGER_TRIGGER_TYPE);
	CvString GetWarmongerPreviewString(PlayerTypes eCurrentOwner = NO_PLAYER, CvCity* pCity = NULL, PlayerTypes eActivePlayer = NO_PLAYER);
	CvString GetLiberationPreviewString(PlayerTypes eOriginalOwner = NO_PLAYER, CvCity* pCity = NULL, PlayerTypes eActivePlayer = NO_PLAYER);
	void ApplyWarmongerPenalties(PlayerTypes eConqueror, PlayerTypes eConquered, CvCity* pCity);
	int GetPlayerCaresValue(PlayerTypes eConqueror, PlayerTypes eConquered, CvCity* pCity, PlayerTypes eCaringPlayer, bool bLiberation = false);
}

#endif //CIV5_AI_DIPLOMACY_H