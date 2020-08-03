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

	/////////////////////////////////////////////////////////
	// Diplomatic Interactions - Memory of what we've talked about with people!
	/////////////////////////////////////////////////////////

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

	// Messages sent to other players about their protected Minor Civs
	bool HasSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor);
	void SetSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue);
	void ResetSentAttackProtectedMinorTaunts(PlayerTypes eMinor);

	/////////////////////////////////////////////////////////
	// Opinion & Approach - What do we think of other major powers, and how are we acting towards them?
	/////////////////////////////////////////////////////////

	// Opinion
	MajorCivOpinionTypes GetMajorCivOpinion(PlayerTypes ePlayer) const;
	void SetMajorCivOpinion(PlayerTypes ePlayer, MajorCivOpinionTypes eOpinion);
	int GetNumMajorCivOpinion(MajorCivOpinionTypes eOpinion) const;

	// Approach
	MajorCivApproachTypes GetMajorCivApproach(PlayerTypes ePlayer, bool bHideTrueFeelings = false) const;
	void SetMajorCivApproach(PlayerTypes ePlayer, MajorCivApproachTypes eApproach);
	int GetNumMajorCivApproach(MajorCivApproachTypes eApproach, bool bHideTrueFeelings = false) const;

	// War Face: If we're planning war, what is our surface approach towards ePlayer?
	WarFaceTypes GetWarFace(PlayerTypes ePlayer) const;
	void SetWarFace(PlayerTypes ePlayer, WarFaceTypes eWarFace);

	// Approach Values: What is our current weight for a given approach towards ePlayer?
	int GetPlayerApproachValue(PlayerTypes ePlayer, MajorCivApproachTypes eApproach) const;
	void SetPlayerApproachValue(PlayerTypes ePlayer, MajorCivApproachTypes eApproach, int iValue);
	PlayerTypes GetPlayerWithHighestApproachValue(MajorCivApproachTypes eApproach) const;

	/////////////////////////////////////////////////////////
	// Minor Civs - How are we acting towards City-States?
	/////////////////////////////////////////////////////////

	MinorCivApproachTypes GetMinorCivApproach(PlayerTypes eMinor) const;
	void SetMinorCivApproach(PlayerTypes eMinor, MinorCivApproachTypes eApproach);
	int GetNumMinorCivApproach(MinorCivApproachTypes eApproach) const;

	bool IsWantToRouteConnectToMinor(PlayerTypes eMinor);
	void SetWantToRouteConnectToMinor(PlayerTypes eMinor, bool bWant);

	/////////////////////////////////////////////////////////
	// Planning Exchanges - Our plans for future dealings with other major powers.
	/////////////////////////////////////////////////////////

	// Declaration of Friendship Value
	int GetDoFValue(PlayerTypes ePlayer) const;
	void SetDoFValue(PlayerTypes ePlayer, int iValue);
	PlayerTypes GetMostValuableDoF(bool bIgnoreDoFs);

	// Competitor Value
	int GetCompetitorValue(PlayerTypes ePlayer) const;
	void SetCompetitorValue(PlayerTypes ePlayer, int iValue);
	PlayerTypes GetBiggestCompetitor() const;

	// Defensive Pact Value
	int GetDefensivePactValue(PlayerTypes ePlayer) const;
	void SetDefensivePactValue(PlayerTypes ePlayer, int iValue);
	PlayerTypes GetMostValuableDefensivePact(bool bIgnoreDPs);

	// Do we want a Declaration of Friendship with this player?
	bool IsWantsDoFWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsDoFWithPlayer(PlayerTypes ePlayer, bool bValue);
	int GetNumDoFsWanted(PlayerTypes eExcludedPlayer = NO_PLAYER) const;

	// Do we want a Defensive Pact with this player?
	bool IsWantsDefensivePactWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsDefensivePactWithPlayer(PlayerTypes ePlayer, bool bValue);
	int GetNumDefensivePactsWanted(PlayerTypes eExcludedPlayer = NO_PLAYER) const;

	// Do we want a Research Agreement with this player?
	bool IsWantsResearchAgreementWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsResearchAgreementWithPlayer(PlayerTypes ePlayer, bool bValue);
	int GetNumResearchAgreementsWanted() const;

	void DoAddWantsResearchAgreementWithPlayer(PlayerTypes ePlayer);
	void DoCancelWantsResearchAgreementWithPlayer(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Exchanges - Our current dealings with other major powers.
	/////////////////////////////////////////////////////////

	bool IsDoFAccepted(PlayerTypes ePlayer) const;
	void SetDoFAccepted(PlayerTypes ePlayer, bool bValue);
	int GetNumDoF() const;

	int GetDoFCounter(PlayerTypes ePlayer) const;
	void SetDoFCounter(PlayerTypes ePlayer, int iValue);
	void ChangeDoFCounter(PlayerTypes ePlayer, int iChange);
	bool IsDoFMessageTooSoon(PlayerTypes ePlayer) const;

	DoFLevelTypes GetDoFType(PlayerTypes ePlayer) const;
	void SetDoFType(PlayerTypes ePlayer, DoFLevelTypes eDoFLevel);

	int GetNumDefensePacts() const;

	bool IsDenouncedPlayer(PlayerTypes ePlayer) const;
	void SetDenouncedPlayer(PlayerTypes ePlayer, bool bValue);
	int GetNumDenouncements() const;
	int GetNumDenouncementsOfPlayer() const;
	bool IsDenouncedByPlayer(PlayerTypes ePlayer) const;

	int GetDenouncedPlayerCounter(PlayerTypes ePlayer) const;
	void SetDenouncedPlayerCounter(PlayerTypes ePlayer, int iValue);
	void ChangeDenouncedPlayerCounter(PlayerTypes ePlayer, int iChange);

	bool IsDenouncingPlayer(PlayerTypes ePlayer) const;

	bool IsCantMatchDeal(PlayerTypes ePlayer);
	void SetCantMatchDeal(PlayerTypes ePlayer, bool bValue);

	int GetNumDemandEverMade(PlayerTypes ePlayer) const;
	bool IsDemandEverMade(PlayerTypes ePlayer) const;

	int GetDemandMadeTurn(PlayerTypes ePlayer) const;
	void SetDemandMadeTurn(PlayerTypes ePlayer, int iValue);

	short GetDemandCounter(PlayerTypes ePlayer) const;
	void SetDemandCounter(PlayerTypes ePlayer, int iValue);
	void ChangeDemandCounter(PlayerTypes ePlayer, int iChange);

	int GetDemandTooSoonNumTurns(PlayerTypes ePlayer) const;
	bool IsDemandTooSoon(PlayerTypes ePlayer) const;

	int GetRecentTradeValue(PlayerTypes ePlayer) const;
	void SetRecentTradeValue(PlayerTypes ePlayer, int iValue);
	void ChangeRecentTradeValue(PlayerTypes ePlayer, int iChange);

	int GetRecentAssistValue(PlayerTypes ePlayer) const;
	void SetRecentAssistValue(PlayerTypes ePlayer, int iValue);
	void ChangeRecentAssistValue(PlayerTypes ePlayer, int iChange);

	int GetCommonFoeValue(PlayerTypes ePlayer) const;
	void SetCommonFoeValue(PlayerTypes ePlayer, int iValue);
	void ChangeCommonFoeValue(PlayerTypes ePlayer, int iChange);

	/////////////////////////////////////////////////////////
	// Coop Wars - What is our cooperative war state against other players?
	/////////////////////////////////////////////////////////

	CoopWarStates GetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;
	void SetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, CoopWarStates eValue);
	CoopWarStates GetGlobalCoopWarAgainstState(PlayerTypes ePlayer);
	CoopWarStates GetGlobalCoopWarWithState(PlayerTypes ePlayer);

	int GetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;
	void SetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iValue);
	void ChangeCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iChange);
	int GetGlobalCoopWarAgainstCounter(PlayerTypes ePlayer);
	int GetGlobalCoopWarWithCounter(PlayerTypes ePlayer);

	int GetNumTimesCoopWarDenied(PlayerTypes ePlayer) const;
	void SetNumTimesCoopWarDenied(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesCoopWarDenied(PlayerTypes ePlayer, int iValue);

	/////////////////////////////////////////////////////////
	// WAR! - What is our war situation with other players?
	/////////////////////////////////////////////////////////

	// Planning For Attack: Do we want to launch a sneak attack Operation against ePlayer?
	bool IsWantsSneakAttack(PlayerTypes ePlayer) const;
	void SetWantsSneakAttack(PlayerTypes ePlayer, bool bValue);

	// Mustering For Attack: Is there a sneak attack Operation completed and ready to roll against ePlayer?
	bool IsArmyInPlaceForAttack(PlayerTypes ePlayer) const;
	void SetArmyInPlaceForAttack(PlayerTypes ePlayer, bool bValue);

	// # of turns we've been at war with ePlayer
	int GetPlayerNumTurnsAtWar(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsAtWar(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsAtWar(PlayerTypes ePlayer, int iChange);
	int GetTeamNumTurnsAtWar(TeamTypes eTeam) const;

	// # of turns since we captured a city from ePlayer
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

	// How many cities have they captured from us?
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

	// War Projection: How do we think the war with ePlayer is going to end up?
	WarProjectionTypes GetWarProjection(PlayerTypes ePlayer) const;
	void SetWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection);

	WarProjectionTypes GetLastWarProjection(PlayerTypes ePlayer) const; // previous value of GetWarProjection
	void SetLastWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection);

	// War Goal: What is is our objective in the war against ePlayer (NO_WAR_GOAL_TYPE if at peace)
	WarGoalTypes GetWarGoal(PlayerTypes ePlayer) const;
	void SetWarGoal(PlayerTypes ePlayer, WarGoalTypes eWarGoal);

	/////////////////////////////////////////////////////////
	// PEACE - What is our peace willingness situation with other players?
	/////////////////////////////////////////////////////////

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

	/////////////////////////////////////////////////////////
	// BACKSTABBING PENALTIES - Have our friends betrayed us?
	/////////////////////////////////////////////////////////

	bool IsUntrustworthyFriend(PlayerTypes ePlayer) const;
	void SetUntrustworthyFriend(PlayerTypes ePlayer, bool bValue);
	bool IsTeamUntrustworthy(TeamTypes eTeam) const;

	bool WasEverBackstabbedBy(PlayerTypes ePlayer) const;
	void SetEverBackstabbedBy(PlayerTypes ePlayer, bool bValue);
	bool WasOurTeamEverBackstabbedBy(PlayerTypes ePlayer) const;

	bool IsDoFBroken(PlayerTypes ePlayer) const;
	void SetDoFBroken(PlayerTypes ePlayer, bool bValue);

	int GetDoFBrokenTurn(PlayerTypes ePlayer) const;
	void SetDoFBrokenTurn(PlayerTypes ePlayer, int iValue);

	bool IsFriendDenouncedUs(PlayerTypes ePlayer) const;
	void SetFriendDenouncedUs(PlayerTypes ePlayer, bool bValue);
	int GetNumFriendsDenouncedBy();
	int GetWeDenouncedFriendCount();
	
	int GetFriendDenouncedUsTurn(PlayerTypes ePlayer) const;
	void SetFriendDenouncedUsTurn(PlayerTypes ePlayer, int iValue);

	bool IsFriendDeclaredWarOnUs(PlayerTypes ePlayer) const;
	void SetFriendDeclaredWarOnUs(PlayerTypes ePlayer, bool bValue);
	int GetWeDeclaredWarOnFriendCount();
	
	int GetFriendDeclaredWarOnUsTurn(PlayerTypes ePlayer) const;
	void SetFriendDeclaredWarOnUsTurn(PlayerTypes ePlayer, int iValue);

	int GetPlayerBackstabCounter(PlayerTypes ePlayer) const;
	void SetPlayerBackstabCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerBackstabCounter(PlayerTypes ePlayer, int iChange);

	// ////////////////////////////////////
	// WARMONGERING PENALTIES - How dangerous is this player's attacking and conquering?
	// ////////////////////////////////////

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

	// Get the amount of warmonger hatred they generated
	int GetOtherPlayerWarmongerAmount(PlayerTypes ePlayer) const;
	void SetOtherPlayerWarmongerAmountTimes100(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerWarmongerAmountTimes100(PlayerTypes ePlayer, int iChange);
	int GetOtherPlayerWarmongerScore(PlayerTypes ePlayer);

	// ////////////////////////////////////
	// AGGRESSIVE POSTURES - How aggressively positioned is this player in relation to us?
	// ////////////////////////////////////

	// Military Aggressive Posture: How aggressively has ePlayer positioned their Units in relation to us?
	AggressivePostureTypes GetMilitaryAggressivePosture(PlayerTypes ePlayer) const;
	void SetMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);
	AggressivePostureTypes GetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer) const;
	void SetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);

	// Expansion Aggressive Posture: How aggressively has ePlayer positioned their Cities in relation to us?
	AggressivePostureTypes GetExpansionAggressivePosture(PlayerTypes ePlayer) const;
	void SetExpansionAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);

	// Plot Buying Aggressive Posture: How aggressively is ePlayer buying land near us?
	AggressivePostureTypes GetPlotBuyingAggressivePosture(PlayerTypes ePlayer) const;
	void SetPlotBuyingAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);
	void DoUpdatePlotBuyingAggressivePostures();

	// ////////////////////////////////////
	// DISPUTE LEVELS - How intense is our level of competition with another major power?
	// ////////////////////////////////////

	// Land Dispute - this can also apply to minor civs!
	DisputeLevelTypes GetLandDisputeLevel(PlayerTypes ePlayer) const;
	void SetLandDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);
	int GetTotalLandDisputeLevel();
	DisputeLevelTypes GetLastTurnLandDisputeLevel(PlayerTypes ePlayer) const;
	void SetLastTurnLandDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);

	// Victory Dispute
	DisputeLevelTypes GetVictoryDisputeLevel(PlayerTypes ePlayer) const;
	void SetVictoryDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);
	void DoUpdateVictoryDisputeLevels();

	// Victory Block
	BlockLevelTypes GetVictoryBlockLevel(PlayerTypes ePlayer) const;
	void SetVictoryBlockLevel(PlayerTypes ePlayer, BlockLevelTypes eBlockLevel);
	void DoUpdateVictoryBlockLevels();

	// Wonder Dispute
	DisputeLevelTypes GetWonderDisputeLevel(PlayerTypes ePlayer) const;
	void SetWonderDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);
	void DoUpdateWonderDisputeLevels();

	// Minor Civ Dispute
	DisputeLevelTypes GetMinorCivDisputeLevel(PlayerTypes ePlayer) const;
	void SetMinorCivDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);

	// ////////////////////////////////////
	// THREAT LEVELS - How much of a danger does this player pose to us?
	// ////////////////////////////////////

	ThreatTypes GetMilitaryThreat(PlayerTypes ePlayer) const;
	void SetMilitaryThreat(PlayerTypes ePlayer, ThreatTypes eMilitaryThreat);

	// Warmonger Threat - how much of a threat are these guys to run amok and break everything
	ThreatTypes GetWarmongerThreat(PlayerTypes ePlayer) const;
	void SetWarmongerThreat(PlayerTypes ePlayer, ThreatTypes eWarmongerThreat);

	// ////////////////////////////////////
	// STRENGTH ASSESSMENTS - How strong is this player compared to us?
	// ////////////////////////////////////

	// Military Strength: How strong is ePlayer compared to US?
	StrengthTypes GetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer) const;
	void SetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eMilitaryStrength);

	// Economic Strength: How strong is ePlayer compared to US?
	StrengthTypes GetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer) const;
	void SetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eEconomicStrength);

	// Target Value: how easy or hard of a target would ePlayer be to attack?
	TargetValueTypes GetPlayerTargetValue(PlayerTypes ePlayer) const;
	void SetPlayerTargetValue(PlayerTypes ePlayer, TargetValueTypes eTargetValue);
	
	// Easy Target: Is this player an easy attack target?
	bool IsEasyTarget(PlayerTypes ePlayer) const;
	void SetEasyTarget(PlayerTypes ePlayer, bool bValue);

	// ////////////////////////////////////
	// PROMISES - What is the status of the agreements we have made with this player?
	// ////////////////////////////////////

	// Military Promise
	bool IsPlayerMadeMilitaryPromise(PlayerTypes ePlayer) const;
	void SetPlayerMadeMilitaryPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenMilitaryPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenMilitaryPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredMilitaryPromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredMilitaryPromise(PlayerTypes ePlayer, bool bValue);
	int GetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer) const;
	void SetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iChange);
	int GetPlayerMadeMilitaryPromise(PlayerTypes ePlayer);
	int GetBrokenMilitaryPromiseTurn(PlayerTypes ePlayer) const;
	void SetBrokenMilitaryPromiseTurn(PlayerTypes ePlayer, int iValue);

	// Expansion Promise
	bool IsPlayerNoSettleRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerNoSettleRequestAccepted(PlayerTypes ePlayer, bool bValue);
	vector<PlayerTypes> GetPlayersWithNoSettlePolicy() const;
	int GetPlayerNoSettleRequestCounter(PlayerTypes ePlayer) const;
	void SetPlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iChange);
	bool IsDontSettleMessageTooSoon(PlayerTypes ePlayer) const;
	bool IsPlayerMadeExpansionPromise(PlayerTypes ePlayer, int iTestGameTurn = -1) const;
	void SetPlayerMadeExpansionPromise(PlayerTypes ePlayer, bool bValue);
	int GetPlayerMadeExpansionPromise(PlayerTypes ePlayer) const;
	bool IsPlayerBrokenExpansionPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenExpansionPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredExpansionPromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredExpansionPromise(PlayerTypes ePlayer, bool bValue);
	AggressivePostureTypes GetPlayerExpansionPromiseData(PlayerTypes ePlayer) const;
	pair<int,int> GetNoExpansionPromiseClosestCities(PlayerTypes eOtherPlayer) const;
	void SetNoExpansionPromiseClosestCities(PlayerTypes eOtherPlayer, pair<int,int> value);
	pair<int,int> GetLastTurnClosestCityPair(PlayerTypes eOtherPlayer) const;
	void SetLastTurnClosestCityPair(PlayerTypes eOtherPlayer, pair<int,int> value);
	int GetBrokenExpansionPromiseValue(PlayerTypes ePlayer) const;
	void SetBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iChange);
	int GetIgnoredExpansionPromiseValue(PlayerTypes ePlayer) const;
	void SetIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iChange);
	bool EverMadeExpansionPromise(PlayerTypes ePlayer) const;
	void SetEverMadeExpansionPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerNoSettleRequestEverAsked(PlayerTypes ePlayer) const;
	void SetPlayerNoSettleRequestEverAsked(PlayerTypes ePlayer, bool bValue);

	// Border Promise
	bool IsPlayerMadeBorderPromise(PlayerTypes ePlayer, int iTestGameTurn = -1) const;
	void SetPlayerMadeBorderPromise(PlayerTypes ePlayer, bool bValue);
	int GetPlayerMadeBorderPromise(PlayerTypes ePlayer);
	bool IsPlayerBrokenBorderPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenBorderPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredBorderPromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredBorderPromise(PlayerTypes ePlayer, bool bValue);
	AggressivePostureTypes GetPlayerBorderPromiseData(PlayerTypes ePlayer) const;
	void SetPlayerBorderPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue);
	int GetBrokenBorderPromiseValue(PlayerTypes ePlayer) const;
	void SetBrokenBorderPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeBrokenBorderPromiseValue(PlayerTypes ePlayer, int iChange);
	int GetIgnoredBorderPromiseValue(PlayerTypes ePlayer) const;
	void SetIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iChange);
	bool EverMadeBorderPromise(PlayerTypes ePlayer) const;
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
	bool IsPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer, bool bValue);

	// Spy Promise
	bool IsPlayerStopSpyingRequestEverAsked(PlayerTypes ePlayer) const;
	bool IsPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer, bool bValue);
	int GetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer) const;
	void SetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iChange);
	bool IsStopSpyingMessageTooSoon(PlayerTypes ePlayer) const;
	bool IsPlayerMadeSpyPromise(PlayerTypes ePlayer) const;
	void SetPlayerMadeSpyPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenSpyPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenSpyPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredSpyPromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredSpyPromise(PlayerTypes ePlayer, bool bValue);
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
	bool IsStopDiggingAcceptable(PlayerTypes ePlayer) const;
	bool IsPlayerMadeNoDiggingPromise(PlayerTypes ePlayer) const;
	void SetPlayerMadeNoDiggingPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenNoDiggingPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenNoDiggingPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredNoDiggingPromise(PlayerTypes ePlayer) const;
	void SetPlayerIgnoredNoDiggingPromise(PlayerTypes ePlayer, bool bValue);

	// Coop War Promise
	bool IsPlayerBrokenCoopWarPromise(PlayerTypes ePlayer) const;
	void SetPlayerBrokenCoopWarPromise(PlayerTypes ePlayer, bool bValue);

	// ////////////////////////////////////
	// OTHER DIPLOMATIC VALUES
	// ////////////////////////////////////

	// ////////////////////////////////////
	// Event Flags
	// ////////////////////////////////////

	bool IsPlayerForgaveForSpying(PlayerTypes ePlayer) const;
	void SetPlayerForgaveForSpying(PlayerTypes ePlayer, bool bValue);

	bool IsPlayerLiberatedCapital(PlayerTypes ePlayer) const;
	void SetPlayerLiberatedCapital(PlayerTypes ePlayer, bool bValue);

	bool IsPlayerCapturedCapital(PlayerTypes ePlayer) const;
	void SetPlayerCapturedCapital(PlayerTypes ePlayer, bool bValue);
	bool IsCapitalCapturedBy(PlayerTypes ePlayer) const;

	bool IsPlayerCapturedHolyCity(PlayerTypes ePlayer) const;
	void SetPlayerCapturedHolyCity(PlayerTypes ePlayer, bool bValue);
	bool IsHolyCityCapturedBy(PlayerTypes ePlayer) const;

	bool IsDoFEverAsked(PlayerTypes ePlayer) const;
	void SetDoFEverAsked(PlayerTypes ePlayer, bool bValue);

	// ////////////////////////////////////
	// # of times/points counters
	// ////////////////////////////////////

	// They liberated our cities!
	int GetNumCitiesLiberatedBy(PlayerTypes ePlayer) const;
	void ChangeNumCitiesLiberatedBy(PlayerTypes ePlayer, int iChange);
	void SetNumCitiesLiberatedBy(PlayerTypes ePlayer, int iValue);

	// They returned civilian units to us!
	int GetNumCiviliansReturnedToMe(PlayerTypes ePlayer) const;
	void SetNumCiviliansReturnedToMe(PlayerTypes ePlayer, int iValue);
	void ChangeNumCiviliansReturnedToMe(PlayerTypes ePlayer, int iChange);

	// They warned us about a plot to destroy us!
	int GetNumTimesIntrigueSharedBy(PlayerTypes ePlayer) const;
	void SetNumTimesIntrigueSharedBy(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesIntrigueSharedBy(PlayerTypes ePlayer, int iChange);

	// They built landmarks in our territory!
	int GetNumLandmarksBuiltForMe(PlayerTypes ePlayer) const;
	void SetNumLandmarksBuiltForMe(PlayerTypes ePlayer, int iValue);
	void ChangeNumLandmarksBuiltForMe(PlayerTypes ePlayer, int iChange);

	// They plotted to destroy us!
	int GetNumTimesTheyPlottedAgainstUs(PlayerTypes ePlayer) const;
	void SetNumTimesTheyPlottedAgainstUs(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesTheyPlottedAgainstUs(PlayerTypes ePlayer, int iChange);

	// They killed our civilians!
	int GetNumTimesRazed(PlayerTypes ePlayer) const;
	void SetNumTimesRazed(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesRazed(PlayerTypes ePlayer, int iChange);

	// They plundered our trade routes!
	int GetNumTradeRoutesPlundered(PlayerTypes ePlayer) const;
	void SetNumTradeRoutesPlundered(PlayerTypes ePlayer, int iValue);
	void ChangeNumTradeRoutesPlundered(PlayerTypes ePlayer, int iChange);

	// They beat us to completing World Wonders!
	int GetNumWondersBeatenTo(PlayerTypes ePlayer) const;
	void SetNumWondersBeatenTo(PlayerTypes ePlayer, int iNewValue);
	void ChangeNumWondersBeatenTo(PlayerTypes ePlayer, int iChange);

	// They stole our territory!
	int GetNumTimesCultureBombed(PlayerTypes ePlayer) const;
	void SetNumTimesCultureBombed(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesCultureBombed(PlayerTypes ePlayer, int iChange);

	// They lowered our Influence with City-States!
	int GetNumTimesTheyLoweredOurInfluence(PlayerTypes ePlayer) const;
	void SetNumTimesTheyLoweredOurInfluence(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesTheyLoweredOurInfluence(PlayerTypes ePlayer, int iChange);

	// They bullied our protected City-States!
	int GetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iChange);

	// They declared war on our protected City-States!
	int GetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iChange);

	// They killed our protected City-States!
	int GetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iChange);

	// They converted our cities to their religion (and we care)!
	int GetNegativeReligiousConversionPoints(PlayerTypes ePlayer) const;
	void SetNegativeReligiousConversionPoints(PlayerTypes ePlayer, int iValue);
	void ChangeNegativeReligiousConversionPoints(PlayerTypes ePlayer, int iChange);

	// Their spies stole from us!
	int GetNumTimesRobbedBy(PlayerTypes ePlayer) const;
	void SetNumTimesRobbedBy(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesRobbedBy(PlayerTypes ePlayer, int iChange);

	// They stole our City-State ally from us in a coup!
	int GetNumTimesPerformedCoupAgainstUs(PlayerTypes ePlayer) const;
	void SetNumTimesPerformedCoupAgainstUs(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesPerformedCoupAgainstUs(PlayerTypes ePlayer, int iChange);

	// They dug up archaeological artifacts in our lands!
	int GetNegativeArchaeologyPoints(PlayerTypes ePlayer) const;
	void SetNegativeArchaeologyPoints(PlayerTypes ePlayer, int iValue);
	void ChangeNegativeArchaeologyPoints(PlayerTypes ePlayer, int iChange);
	
	int GetNumArtifactsEverDugUp(PlayerTypes ePlayer) const;
	void SetNumArtifactsEverDugUp(PlayerTypes ePlayer, int iValue);
	void ChangeNumArtifactsEverDugUp(PlayerTypes ePlayer, int iChange);

	// They nuked us!
	int GetNumTimesNuked(PlayerTypes ePlayer) const;
	void SetNumTimesNuked(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesNuked(PlayerTypes ePlayer, int iChange);

	// ////////////////////////////////////
	// Turn counters
	// ////////////////////////////////////

	bool WasResurrectedBy(PlayerTypes ePlayer) const;
	bool WasResurrectedByAnyone() const;
	void SetResurrectedBy(PlayerTypes ePlayer, bool bValue);
	bool WasResurrectedThisTurnBy(PlayerTypes ePlayer) const;

	int GetLiberatedCitiesTurn(PlayerTypes ePlayer) const;
	void SetLiberatedCitiesTurn(PlayerTypes ePlayer, int iValue);

	int GetCiviliansReturnedToMeTurn(PlayerTypes ePlayer) const;
	void SetCiviliansReturnedToMeTurn(PlayerTypes ePlayer, int iValue);

	int GetIntrigueSharedTurn(PlayerTypes ePlayer) const;
	void SetIntrigueSharedTurn(PlayerTypes ePlayer, int iValue);
	
	int GetForgaveForSpyingTurn(PlayerTypes ePlayer) const;
	void SetForgaveForSpyingTurn(PlayerTypes ePlayer, int iValue);

	int GetLandmarksBuiltForMeTurn(PlayerTypes ePlayer) const;
	void SetLandmarksBuiltForMeTurn(PlayerTypes ePlayer, int iValue);

	int GetPlottedAgainstUsTurn(PlayerTypes ePlayer) const;
	void SetPlottedAgainstUsTurn(PlayerTypes ePlayer, int iValue);

	int GetPlunderedTradeRouteTurn(PlayerTypes ePlayer) const;
	void SetPlunderedTradeRouteTurn(PlayerTypes ePlayer, int iValue);

	// They sided with their Protected Minor (after we attacked/bullied it)
	int GetTurnsSinceOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer) const;
	void SetOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iChange);
	bool IsOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer) const;

	// Protected Minor Bullied
	int GetTurnsSincePlayerBulliedProtectedMinor(PlayerTypes eBullyPlayer) const;

	// Protected Minor Attacked
	int GetTurnsSincePlayerAttackedProtectedMinor(PlayerTypes ePlayer) const;
	void SetOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iChange);

	// Protected Minor Killed
	int GetTurnsSincePlayerKilledProtectedMinor(PlayerTypes ePlayer) const;
	void SetOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iChange);
	
	int GetReligiousConversionTurn(PlayerTypes ePlayer) const;
	void SetReligiousConversionTurn(PlayerTypes ePlayer, int iValue);
	
	int GetRobbedTurn(PlayerTypes ePlayer) const;
	void SetRobbedTurn(PlayerTypes ePlayer, int iValue);
	
	int GetPerformedCoupTurn(PlayerTypes ePlayer) const;
	void SetPerformedCoupTurn(PlayerTypes ePlayer, int iValue);

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

	// ////////////////////////////////////
	// Player-specific memory
	// ////////////////////////////////////

	PlayerTypes GetOtherPlayerProtectedMinorBullied(PlayerTypes eBullyPlayer) const;

	PlayerTypes GetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer, PlayerTypes eAttackedPlayer);

	PlayerTypes GetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer) const;
	void SetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer, PlayerTypes eKilledPlayer);

	// ////////////////////////////////////
	// GUESSES - What do we think the other players in the world are up to?
	// ////////////////////////////////////

	// Our guess as to another player's opinion of us
	MajorCivOpinionTypes GetOpinionTowardsUsGuess(PlayerTypes ePlayer) const;
	void SetOpinionTowardsUsGuess(PlayerTypes ePlayer, MajorCivOpinionTypes eOpinion);

	// Our guess as to another player's approach towards us
	MajorCivApproachTypes GetVisibleApproachTowardsUs(PlayerTypes ePlayer) const;
	MajorCivApproachTypes GetApproachTowardsUsGuess(PlayerTypes ePlayer) const;
	void SetApproachTowardsUsGuess(PlayerTypes ePlayer, MajorCivApproachTypes eApproach);
	int GetApproachTowardsUsGuessCounter(PlayerTypes ePlayer) const;
	void SetApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iValue);
	void ChangeApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iChange);

	// Guesses about other players' interactions with the world
	MajorCivOpinionTypes GetMajorCivOtherPlayerOpinion(PlayerTypes ePlayer, PlayerTypes eOtherPlayer) const;
	void SetMajorCivOtherPlayerOpinion(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, MajorCivOpinionTypes eOpinion);

	MajorCivApproachTypes GetMajorCivOtherPlayerApproach(PlayerTypes ePlayer, PlayerTypes eOtherPlayer) const;
	void SetMajorCivOtherPlayerApproach(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, MajorCivApproachTypes eApproach);
	
	int GetMajorCivOtherPlayerApproachCounter(PlayerTypes ePlayer, PlayerTypes eOtherPlayer) const;
	void SetMajorCivOtherPlayerApproachCounter(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, int iValue);
	void ChangeMajorCivOtherPlayerApproachCounter(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, int iChange);

	// Other Player War Value Lost: the int value of damage we've inflicted UPON ePlayer in war
	int GetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const;
	void SetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue);
	void ChangeOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iChange);

	// Stores current war values, for comparison later on
	void CacheOtherPlayerWarValuesLost();

	// Other Player Last Round War Value Lost: the int value of damage we've inflicted UPON ePlayer in war up until this turn
	int GetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const;
	void SetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue);

	// Other Player War Damage Level: how much damage we've inflicted UPON ePlayer
	WarDamageLevelTypes GetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const;
	void SetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, WarDamageLevelTypes eDamageLevel);

	ThreatTypes GetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eOtherPlayer) const;
	void SetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, ThreatTypes eThreatType);

	DisputeLevelTypes GetEstimateOtherPlayerLandDisputeLevel(PlayerTypes ePlayer, PlayerTypes eOtherPlayer) const;
	void SetEstimateOtherPlayerLandDisputeLevel(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, DisputeLevelTypes eDisputeLevel);

	DisputeLevelTypes GetEstimateOtherPlayerVictoryDisputeLevel(PlayerTypes ePlayer, PlayerTypes eOtherPlayer) const;
	void SetEstimateOtherPlayerVictoryDisputeLevel(PlayerTypes ePlayer, PlayerTypes eOtherPlayer, DisputeLevelTypes eDisputeLevel);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// ////////////////////////////////////
	// C4DF Values
	// ////////////////////////////////////

	// Sharing Opinion
	bool IsShareOpinionAccepted(PlayerTypes ePlayer) const;
	void SetShareOpinionAccepted(PlayerTypes ePlayer, bool bValue);

	int GetShareOpinionCounter(PlayerTypes ePlayer) const;
	void SetShareOpinionCounter(PlayerTypes ePlayer, int iValue);
	void ChangeShareOpinionCounter(PlayerTypes ePlayer, int iChange);
	bool IsShareOpinionTooSoon(PlayerTypes ePlayer) const;

	// Move Troops
	bool IsPlayerMoveTroopsRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerMoveTroopsRequestAccepted(PlayerTypes ePlayer, bool bValue);

	int GetPlayerMoveTroopsRequestCounter(PlayerTypes ePlayer) const;
	void SetPlayerMoveTroopsRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerMoveTroopsRequestCounter(PlayerTypes ePlayer, int iChange);

	bool IsTooSoonForMoveTroopsRequest(PlayerTypes ePlayer) const;

	// Gift
	bool IsOfferingGift(PlayerTypes ePlayer) const;	// We're offering a gift!
	void SetOfferingGift(PlayerTypes ePlayer, bool bValue);

	bool IsOfferedGift(PlayerTypes ePlayer) const;	// We offered a gift!
	void SetOfferedGift(PlayerTypes ePlayer, bool bValue);

	// Help Request
	bool IsHelpRequestEverMade(PlayerTypes ePlayer) const;

	int GetHelpRequestCounter(PlayerTypes ePlayer) const;
	void SetHelpRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangeHelpRequestCounter(PlayerTypes ePlayer, int iChange);
	
	int GetHelpRequestTooSoonNumTurns(PlayerTypes ePlayer) const;
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

	// How much gold have we taxed from him since we've known him?
	int GetVassalGoldPerTurnTaxedSinceVassalStarted(PlayerTypes ePlayer) const;
	void SetVassalGoldPerTurnTaxedSinceVassalStarted(PlayerTypes ePlayer, int iValue);
	void ChangeVassalGoldPerTurnTaxedSinceVassalStarted(PlayerTypes ePlayer, int iChange);
	
	// How much gold has our vassal collected since we've known him?
	int GetVassalGoldPerTurnCollectedSinceVassalStarted(PlayerTypes ePlayer) const;
	void SetVassalGoldPerTurnCollectedSinceVassalStarted(PlayerTypes ePlayer, int iValue);
	void ChangeVassalGoldPerTurnCollectedSinceVassalStarted(PlayerTypes ePlayer, int iChange);
#endif

	// ////////////////////////////////////
	// OVERALL STATUS ACROSS ALL OTHER CIVS
	// ////////////////////////////////////

	StateAllWars GetStateAllWars() const;
	void SetStateAllWars(StateAllWars eState);

	bool IsDemandReady();
	void SetDemandReady(bool bValue);

	PlayerTypes GetDemandTargetPlayer() const;
	void SetDemandTargetPlayer(PlayerTypes ePlayer);

	PlayerTypes GetCSBullyTargetPlayer() const;
	void SetCSBullyTargetPlayer(PlayerTypes ePlayer);

	PlayerTypes GetCSWarTargetPlayer() const;
	void SetCSWarTargetPlayer(PlayerTypes ePlayer);


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
	void DoEstimateOtherPlayerOpinions();

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

	void DoUpdateEasyTargets();
	bool DoTestOnePlayerEasyTarget(PlayerTypes ePlayer);

	// Set default values when we're attacked and its not our turn
	void DoSomeoneDeclaredWarOnMe(TeamTypes eTeam);

	void DoUpdateWarStates();

	void DoUpdateWarProjections();

	int GetHighestWarscore(bool bOnlyCurrentWars = true);
	PlayerTypes GetHighestWarscorePlayer(bool bOnlyCurrentWars = true);

	int GetWarScore(PlayerTypes ePlayer, bool bUsePeacetimeCalculation = false, bool bDebug = false);

	void DoUpdateWarGoals();

#if defined(MOD_BALANCE_CORE)
	int CountUnitsAroundEnemyCities(PlayerTypes ePlayer, int iTurnRange) const;
	bool HasVisibilityOfEnemyCityDanger(CvCity* pCity) const;
#endif

	// Military Rating: How skilled is ePlayer at war?
	int ComputeAverageMajorMilitaryRating(PlayerTypes eExcludedPlayer = NO_PLAYER);
	int ComputeRatingStrengthAdjustment(PlayerTypes ePlayer);

	void DoUpdatePlayerMilitaryStrengths();
	void DoUpdatePlayerEconomicStrengths();
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

	/////////////////////////////////////////////////////////
	// Threats to this Player
	/////////////////////////////////////////////////////////

	void DoUpdateMilitaryThreats();
	
	void DoUpdateEstimateOtherPlayerMilitaryThreats();

	void DoUpdateWarmongerThreats(bool bUpdateOnly = false);

#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	int GetNumberOfThreatenedCities(PlayerTypes eEnemy);
#endif

	/////////////////////////////////////////////////////////
	// Planning Exchanges
	/////////////////////////////////////////////////////////

	void DoUpdatePlanningExchanges();

	bool IsCanMakeResearchAgreementRightNow(PlayerTypes ePlayer);

#if defined(MOD_BALANCE_CORE_DEALS)
	bool IsWantsToConquer(PlayerTypes ePlayer) const;
	bool IsPotentialMilitaryTargetOrThreat(PlayerTypes ePlayer, bool bFromApproachSelection = false) const;

	PlayerTypes GetNextBestDoF();
	bool IsGoodChoiceForDoF(PlayerTypes ePlayer);

	PlayerTypes GetNextBestDefensivePact();
	bool IsCanMakeDefensivePactRightNow(PlayerTypes ePlayer);
	bool IsGoodChoiceForDefensivePact(PlayerTypes ePlayer);
#endif
	/////////////////////////////////////////////////////////
	// Issues of Dispute
	/////////////////////////////////////////////////////////

	void DoUpdateLandDisputeLevels();

	void DoUpdateEstimateOtherPlayerLandDisputeLevels();

	void DoUpdateEstimateOtherPlayerVictoryDisputeLevels();

	bool IsPlayerRecklessExpander(PlayerTypes ePlayer);
	bool IsPlayerWonderSpammer(PlayerTypes ePlayer);

	bool IsNukedBy(PlayerTypes ePlayer) const;
	bool IsAngryAboutProtectedMinorKilled(PlayerTypes ePlayer) const;
	bool IsAngryAboutProtectedMinorAttacked(PlayerTypes ePlayer) const;
	bool IsAngryAboutProtectedMinorBullied(PlayerTypes ePlayer) const;
	bool IsAngryAboutSidedWithTheirProtectedMinor(PlayerTypes ePlayer) const;

	void DoUpdateExpansionAggressivePostures();
	void DoUpdateOnePlayerExpansionAggressivePosture(PlayerTypes ePlayer);
	pair<int,int> GetClosestCityPair(PlayerTypes ePlayer);

	void DoRelationshipPairing();
	
	bool IsStrategicTradePartner(PlayerTypes ePlayer) const;
	bool IsMajorCompetitor(PlayerTypes ePlayer) const;
	bool IsEarlyGameCompetitor(PlayerTypes ePlayer);

	bool IsIgnorePolicyDifferences(PlayerTypes ePlayer);
	bool IsIgnoreReligionDifferences(PlayerTypes ePlayer);
	bool IsIgnoreIdeologyDifferences(PlayerTypes ePlayer);

	void DoUpdateMinorCivDisputeLevels();
	bool IsMinorCivTroublemaker(PlayerTypes ePlayer, bool bIgnoreBullying = false) const;
	bool IsPrimeLeagueCompetitor(PlayerTypes ePlayer) const;
	
	// Tech Dispute
	DisputeLevelTypes GetTechDisputeLevel(PlayerTypes ePlayer) const;

	// Diplomacy AI Options
	bool IsCompetingForVictory() const;
	bool IsEndgameAggressive() const;

	/////////////////////////////////////////////////////////
	// Evaluation of Other Players' Tendencies
	/////////////////////////////////////////////////////////

	// Someone had some kind of interaction with another player

	void DoWeMadePeaceWithSomeone(TeamTypes eOtherTeam);
	void DoPlayerDeclaredWarOnSomeone(PlayerTypes ePlayer, TeamTypes eOtherTeam, bool bDefensivePact);
	void DoPlayerKilledSomeone(PlayerTypes ePlayer, PlayerTypes eDeadPlayer);
	void DoPlayerBulliedSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);
	void DoPlayerMetSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);

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
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	void DoEndDoFStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
#endif
	void DoDenounceFriendStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoRequestFriendDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	//void DoWorkAgainstSomeoneStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	//void DoEndWorkAgainstSomeoneStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);

	void DoLuxuryTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoEmbassyExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoEmbassyOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoOpenBordersExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoOpenBordersOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoResearchAgreementOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
#if defined(MOD_BALANCE_CORE_DEALS)
	void DoStrategicTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoDefensivePactOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoCityTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoCityExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoThirdPartyWarTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoThirdPartyPeaceTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoVoteTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
#endif
	void DoRenewExpiredDeal(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	//void DoResearchAgreementPlan(PlayerTypes ePlayer, DiploStatementTypes &eStatement);

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
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	void DoVictoryCompetitionStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoVictoryBlockStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
#endif

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

	bool IsActHostileTowardsHuman(PlayerTypes eHuman);

	const char* GetGreetHumanMessage(LeaderheadAnimationTypes& eAnimation);
	const char* GetInsultHumanMessage();
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	const char* GetOfferText(PlayerTypes ePlayer);
	int	GetDenounceMessage(PlayerTypes ePlayer);
	const char* GetDenounceMessageValue(int iValue);
#endif
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

	// Coop War
	bool DoTestCoopWarDesire(PlayerTypes ePlayer, PlayerTypes& eChosenTargetPlayer);

	CoopWarStates GetWillingToAgreeToCoopWarState(PlayerTypes ePlayer, PlayerTypes eTargetPlayer);
	int GetCoopWarScore(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, bool bAskedByPlayer);
#if defined(MOD_BALANCE_CORE)
	bool IsCoopWarRequestUnacceptable(PlayerTypes eAskingPlayer, PlayerTypes eTargetPlayer);
#endif

	bool IsCoopWarMessageTooSoon(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;

	bool IsCoopWarEverAsked(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;

	bool DoTestContinueCoopWarsDesire(PlayerTypes ePlayer, PlayerTypes& eAgainstPlayer);
	bool IsContinueCoopWar(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer);

	bool IsLockedIntoCoopWar(PlayerTypes ePlayer);

	// Human Demand
	void DoDemandMade(PlayerTypes ePlayer, DemandResponseTypes eDemand);

	// No Settling
	bool IsDontSettleAcceptable(PlayerTypes ePlayer) const;


	// Working With Player
	bool IsDoFAcceptable(PlayerTypes ePlayer);
	bool IsTooEarlyForDoF(PlayerTypes ePlayer);
	
	int GetBrokenAttackCityStatePromiseTurn(PlayerTypes ePlayer) const;
	void SetBrokenAttackCityStatePromiseTurn(PlayerTypes ePlayer, int iValue);

	int GetNumRA();
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

	void SetHelpRequestEverMade(PlayerTypes ePlayer, bool bValue);
	void SetNumDemandEverMade(PlayerTypes ePlayer, int iValue);

	// Stop Spying
	bool IsStopSpyingAcceptable(PlayerTypes ePlayer) const;
	bool IsStopSpreadingReligionAcceptable(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Opinion modifiers
	/////////////////////////////////////////////////////////
	int GetBaseOpinionScore(PlayerTypes ePlayer);
	int GetLandDisputeLevelScore(PlayerTypes ePlayer);
	int GetWonderDisputeLevelScore(PlayerTypes ePlayer);
	int GetMinorCivDisputeLevelScore(PlayerTypes ePlayer);
	int GetTechDisputeLevelScore(PlayerTypes ePlayer);
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	int GetVictoryDisputeLevelScore(PlayerTypes ePlayer);
	int GetVictoryBlockLevelScore(PlayerTypes ePlayer);
#endif
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
#if defined(MOD_BALANCE_CORE)
	int GetTradeRoutesPlunderedScore(PlayerTypes ePlayer);
	int GetTimesPlottedAgainstUsScore(PlayerTypes ePlayer);
	int GetTimesPerformedCoupScore(PlayerTypes ePlayer);
#endif
	int GetDugUpMyYardScore(PlayerTypes ePlayer);
	int GetTimesIntrigueSharedScore(PlayerTypes ePlayer);
	int GetBrokenMilitaryPromiseScore(PlayerTypes ePlayer);
	int GetBrokenMilitaryPromiseWithAnybodyScore(PlayerTypes ePlayer);
	int GetIgnoredMilitaryPromiseScore(PlayerTypes ePlayer);
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
#if defined(MOD_BALANCE_CORE_DEALS)
	int GetOpenBordersScore(PlayerTypes ePlayer);
	MajorCivOpinionTypes GetNeighborOpinion(PlayerTypes ePlayer) const;
	bool MusteringForNeighborAttack(PlayerTypes ePlayer) const;
#endif
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
#if defined(MOD_BALANCE_CORE)
	int GetCitiesRazedScore(PlayerTypes ePlayer);
	int GetCitiesRazedGlobalScore(PlayerTypes ePlayer);
	int GetPtPSameCSScore(PlayerTypes ePlayer);
	int GetPolicyScore(PlayerTypes ePlayer);
#endif
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

#if defined(MOD_BALANCE_CORE)
	bool IsCloseToAnyVictoryCondition() const;
	bool IsCloseToSSVictory() const;
	bool IsCloseToDominationVictory() const;
	bool IsCloseToCultureVictory() const;
	bool IsCloseToDiploVictory() const;
#endif

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

	CvDeal* GetDealToRenew(int* piDealType = NULL, PlayerTypes eOtherPlayer = NO_PLAYER);
	void ClearDealToRenew();

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
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	void LogBrokenDoF(PlayerTypes ePlayer);
#endif
	void LogDenounce(PlayerTypes ePlayer, bool bBackstab = false, bool bRefusal = false);
	void LogFriendRequestDenounce(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bAgreed);
	void LogCoopWar(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, CoopWarStates eAcceptedState);
	void LogWantRA(PlayerTypes ePlayer);
#if defined(MOD_BALANCE_CORE_DEALS)
	void LogWantDP(PlayerTypes ePlayer);
#endif

	void LogOpinionUpdate(PlayerTypes ePlayer, std::vector<int>& viOpinionValues);
#if defined(MOD_BALANCE_CORE)
	void LogApproachValueDeltas(PlayerTypes ePlayer, const int* aiApproachValues, const int* aiScratchValues);
#endif
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
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	void LogVictoryBlock(CvString& strString, PlayerTypes ePlayer);
#endif
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

		// 1D ARRAYS
		// Personality Values
		char m_aiPersonalityMajorCivApproachBiases[NUM_MAJOR_CIV_APPROACHES];
		char m_aiPersonalityMinorCivApproachBiases[NUM_MINOR_CIV_APPROACHES];

		// Diplomatic Interactions
		DeclarationLogData m_aDeclarationsLog[MAX_DIPLO_LOG_STATEMENTS];
		short m_aDiploLogStatementTurnCountScratchPad[NUM_DIPLO_LOG_STATEMENT_TYPES];

		// Opinion & Approach
		char m_aeMajorCivOpinion[MAX_MAJOR_CIVS];
		char m_aeMajorCivApproach[MAX_MAJOR_CIVS];
		char m_aeWarFace[MAX_MAJOR_CIVS];

		// Minor Civs
		char m_aeMinorCivApproach[MAX_MINOR_CIVS];
		bool m_abWantToRouteToMinor[MAX_MINOR_CIVS];

		// Planning Exchanges
		short m_aiDoFValue[MAX_MAJOR_CIVS];
		short m_aiCompetitorValue[MAX_MAJOR_CIVS];
		short m_aiDefensivePactValue[MAX_MAJOR_CIVS];
		bool m_abWantsDoFWithPlayer[MAX_MAJOR_CIVS];
		bool m_abWantsDefensivePactWithPlayer[MAX_MAJOR_CIVS];
		bool m_abWantsResearchAgreementWithPlayer[MAX_MAJOR_CIVS];

		// Exchanges
		bool m_abDoFAccepted[MAX_MAJOR_CIVS];
		short m_aiDoFCounter[MAX_MAJOR_CIVS];
		char m_aeDoFType[MAX_MAJOR_CIVS];
		bool m_abDenouncedPlayer[MAX_MAJOR_CIVS];
		short m_aiDenouncedPlayerCounter[MAX_MAJOR_CIVS];
		bool m_abCantMatchDeal[MAX_MAJOR_CIVS];
		int m_aiDemandEverMade[MAX_MAJOR_CIVS]; // short
		short m_aiDemandMadeTurn[MAX_MAJOR_CIVS];
		short m_aiDemandCounter[MAX_MAJOR_CIVS];
		short m_aiDemandTooSoonNumTurns[MAX_MAJOR_CIVS];
		short m_aiTradeValue[MAX_MAJOR_CIVS];
		short m_aiAssistValue[MAX_MAJOR_CIVS];
		short m_aiCommonFoeValue[MAX_MAJOR_CIVS];

		// Coop Wars
		short m_aiNumTimesCoopWarDenied[MAX_MAJOR_CIVS];

		// War
		bool m_abWantsSneakAttack[MAX_CIV_PLAYERS];
		bool m_abArmyInPlaceForAttack[MAX_CIV_PLAYERS];
		short m_aiPlayerNumTurnsAtWar[MAX_CIV_PLAYERS];
		short m_aiPlayerNumTurnsSinceCityCapture[MAX_CIV_PLAYERS];
		short m_aiNumWarsFought[MAX_CIV_PLAYERS];
		short m_aiNumWarsDeclaredOnUs[MAX_CIV_PLAYERS];
		short m_aiNumCitiesCaptured[MAX_CIV_PLAYERS];
		int m_aiWarValueLost[MAX_CIV_PLAYERS];
		short m_aiWarDamageValue[MAX_CIV_PLAYERS];
		char m_aeWarDamageLevel[MAX_CIV_PLAYERS];
		char m_aeWarState[MAX_CIV_PLAYERS];
		char m_aeWarProjection[MAX_CIV_PLAYERS];
		char m_aeLastWarProjection[MAX_CIV_PLAYERS];
		char m_aeWarGoal[MAX_CIV_PLAYERS];

		// Peace
		short m_aiPlayerNumTurnsAtPeace[MAX_CIV_PLAYERS];
		char m_aePeaceTreatyWillingToOffer[MAX_MAJOR_CIVS];
		char m_aePeaceTreatyWillingToAccept[MAX_MAJOR_CIVS];
		short m_aiWantPeaceCounter[MAX_CIV_PLAYERS];

		// Backstabbing Penalties
		bool m_abUntrustworthyFriend[MAX_MAJOR_CIVS];
		bool m_abEverBackstabbedBy[MAX_MAJOR_CIVS];
		bool m_abDoFBroken[MAX_MAJOR_CIVS];
		short m_aiDoFBrokenTurn[MAX_MAJOR_CIVS];
		bool m_abFriendDenouncedUs[MAX_MAJOR_CIVS];
		short m_aiFriendDenouncedUsTurn[MAX_MAJOR_CIVS];
		bool m_abFriendDeclaredWarOnUs[MAX_MAJOR_CIVS];
		short m_aiFriendDeclaredWarOnUsTurn[MAX_MAJOR_CIVS];
		short m_aiPlayerBackstabCounter[MAX_MAJOR_CIVS];

		// Warmongering Penalties
		char m_aiOtherPlayerNumMinorsAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMinorsConquered[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMajorsAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMajorsConquered[MAX_MAJOR_CIVS];
		int m_aiOtherPlayerWarmongerAmountTimes100[MAX_MAJOR_CIVS];

		// Aggressive Postures
		char m_aeMilitaryAggressivePosture[MAX_CIV_PLAYERS];
		char m_aeLastTurnMilitaryAggressivePosture[MAX_CIV_PLAYERS];
		char m_aeExpansionAggressivePosture[MAX_CIV_PLAYERS];
		char m_aePlotBuyingAggressivePosture[MAX_CIV_PLAYERS];

		// Dispute Levels
		char m_aePlayerLandDisputeLevel[MAX_CIV_PLAYERS];
		char m_aePlayerLastTurnLandDisputeLevel[MAX_CIV_PLAYERS];
		char m_aePlayerVictoryDisputeLevel[MAX_MAJOR_CIVS];
		char m_aePlayerVictoryBlockLevel[MAX_MAJOR_CIVS];
		char m_aePlayerWonderDisputeLevel[MAX_MAJOR_CIVS];
		char m_aePlayerMinorCivDisputeLevel[MAX_MAJOR_CIVS];

		// Threat Levels
		char m_aeMilitaryThreat[MAX_CIV_PLAYERS];
		char m_aeWarmongerThreat[MAX_MAJOR_CIVS];

		// Strength Assessments
		char m_aePlayerMilitaryStrengthComparedToUs[MAX_CIV_PLAYERS];
		char m_aePlayerEconomicStrengthComparedToUs[MAX_CIV_PLAYERS];
		char m_aePlayerTargetValue[MAX_CIV_PLAYERS];
		bool m_abEasyTarget[MAX_CIV_PLAYERS];

		// PROMISES
		// Military Promise
		bool m_abPlayerMadeMilitaryPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenMilitaryPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredMilitaryPromise[MAX_MAJOR_CIVS];
		short m_aiPlayerMilitaryPromiseCounter[MAX_MAJOR_CIVS];
		short m_aiBrokenMilitaryPromiseTurn[MAX_MAJOR_CIVS];

		// Expansion Promise
		bool m_abPlayerNoSettleRequest[MAX_MAJOR_CIVS];
		short m_aiPlayerNoSettleRequestCounter[MAX_MAJOR_CIVS];
		short m_aiPlayerMadeExpansionPromiseTurn[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenExpansionPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredExpansionPromise[MAX_MAJOR_CIVS];
		char m_aePlayerExpansionPromiseData[MAX_MAJOR_CIVS];
		pair<int,int> m_paNoExpansionPromise[MAX_MAJOR_CIVS];
		pair<int,int> m_paLastTurnEmpireDistance[MAX_MAJOR_CIVS];
		short m_aiBrokenExpansionPromiseValue[MAX_MAJOR_CIVS];
		short m_aiIgnoredExpansionPromiseValue[MAX_MAJOR_CIVS];
		bool m_abPlayerEverMadeExpansionPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerNoSettleRequestEverAsked[MAX_MAJOR_CIVS];

		// Border Promise
		short m_aiPlayerMadeBorderPromiseTurn[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenBorderPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredBorderPromise[MAX_MAJOR_CIVS];
		char m_aePlayerBorderPromiseData[MAX_MAJOR_CIVS];
		short m_aiBrokenBorderPromiseValue[MAX_MAJOR_CIVS];
		short m_aiIgnoredBorderPromiseValue[MAX_MAJOR_CIVS];
		bool m_abPlayerEverMadeBorderPromise[MAX_MAJOR_CIVS];

		// Bully City-State Promise
		bool m_abPlayerMadeBullyCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenBullyCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredBullyCityStatePromise[MAX_MAJOR_CIVS];

		// Attack City-State Promise
		bool m_abPlayerMadeAttackCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenAttackCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredAttackCityStatePromise[MAX_MAJOR_CIVS];
		short m_aiBrokenAttackCityStatePromiseTurn[MAX_MAJOR_CIVS];

		// Spy Promise
		bool m_abPlayerStopSpyingRequest[MAX_MAJOR_CIVS];
		short m_aiPlayerStopSpyingRequestCounter[MAX_MAJOR_CIVS];
		bool m_abPlayerMadeSpyPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenSpyPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredSpyPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerStopSpyingRequestEverAsked[MAX_MAJOR_CIVS];

		// Religious Conversion Promise
		bool m_abPlayerAskedNotToConvert[MAX_MAJOR_CIVS];
		bool m_abPlayerAgreedNotToConvert[MAX_MAJOR_CIVS];
		bool m_abPlayerMadeNoConvertPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenNoConvertPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredNoConvertPromise[MAX_MAJOR_CIVS];
		bool m_abEverConvertedCity[MAX_MAJOR_CIVS];

		// Digging Promise
		bool m_abPlayerAskedNotToDig[MAX_MAJOR_CIVS];
		bool m_abPlayerAgreedNotToDig[MAX_MAJOR_CIVS];
		bool m_abPlayerMadeNoDiggingPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenNoDiggingPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredNoDiggingPromise[MAX_MAJOR_CIVS];

		// Coop War Promise
		bool m_abPlayerBrokenCoopWarPromise[MAX_MAJOR_CIVS];

		// OTHER DIPLOMATIC VALUES
		// Event flags
		bool m_abPlayerForgaveForSpying[MAX_MAJOR_CIVS];
		bool m_abPlayerLiberatedCapital[MAX_MAJOR_CIVS];
		bool m_abDoFEverAsked[MAX_MAJOR_CIVS];
		bool m_abPlayerCapturedCapital[MAX_MAJOR_CIVS];
		bool m_abPlayerCapturedHolyCity[MAX_MAJOR_CIVS];

		// # of times/points counters
		short m_aiNumCitiesLiberated[MAX_MAJOR_CIVS];
		short m_aiNumCiviliansReturnedToMe[MAX_MAJOR_CIVS];
		short m_aiNumTimesIntrigueSharedBy[MAX_MAJOR_CIVS];
		short m_aiNumLandmarksBuiltForMe[MAX_MAJOR_CIVS];
		char m_aiTheyPlottedAgainstUs[MAX_MAJOR_CIVS];
		short m_aiNumTimesRazed[MAX_MAJOR_CIVS];
		short m_aiNumTradeRoutesPlundered[MAX_MAJOR_CIVS];
		short m_aiNumWondersBeatenTo[MAX_MAJOR_CIVS];
		short m_aiNumTimesCultureBombed[MAX_MAJOR_CIVS];
		char m_aiTheyLoweredOurInfluence[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumProtectedMinorsBullied[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumProtectedMinorsAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumProtectedMinorsKilled[MAX_MAJOR_CIVS];
		short m_aiNegativeReligiousConversionPoints[MAX_MAJOR_CIVS];
		short m_aiNumTimesRobbedBy[MAX_MAJOR_CIVS];
		char m_aiPerformedCoupAgainstUs[MAX_MAJOR_CIVS];
		short m_aiNegativeArchaeologyPoints[MAX_MAJOR_CIVS];
		int m_aiArtifactsEverDugUp[MAX_MAJOR_CIVS]; // char
		short m_aiNumTimesNuked[MAX_MAJOR_CIVS];

		// Turn counters
		short m_aiResurrectedOnTurn[MAX_MAJOR_CIVS];
		short m_aiLiberatedCitiesTurn[MAX_MAJOR_CIVS];
		short m_aiCiviliansReturnedToMeTurn[MAX_MAJOR_CIVS];
		short m_aiIntrigueSharedTurn[MAX_MAJOR_CIVS];
		short m_aiPlayerForgaveForSpyingTurn[MAX_MAJOR_CIVS];
		short m_aiNumLandmarksBuiltForMeTurn[MAX_MAJOR_CIVS];
		short m_aiPlottedAgainstUsTurn[MAX_MAJOR_CIVS];
		short m_aiPlunderedTradeRouteTurn[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceAttackedProtectedMinor[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceKilledProtectedMinor[MAX_MAJOR_CIVS];
		short m_aiReligiousConversionTurn[MAX_MAJOR_CIVS];
		short m_aiTimesRobbedTurn[MAX_MAJOR_CIVS];
		short m_aiPerformedCoupTurn[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceWeLikedTheirProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceWeDislikedTheirProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceTheySupportedOurProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceTheyFoiledOurProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceTheySupportedOurHosting[MAX_MAJOR_CIVS];

		// Player-specific memory
		char m_aiOtherPlayerProtectedMinorAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerProtectedMinorKilled[MAX_MAJOR_CIVS];

		// GUESSES
		// Guesses about other players' feelings towards us
		char m_aeOpinionTowardsUsGuess[MAX_MAJOR_CIVS];
		char m_aeApproachTowardsUsGuess[MAX_MAJOR_CIVS];
		char m_aiApproachTowardsUsGuessCounter[MAX_MAJOR_CIVS];

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		// C4DF Values
		bool m_abShareOpinionAccepted[MAX_MAJOR_CIVS];
		short m_aiShareOpinionCounter[MAX_MAJOR_CIVS];
		bool m_abMoveTroopsRequestAccepted[MAX_MAJOR_CIVS];
		short m_aiMoveTroopsRequestCounter[MAX_MAJOR_CIVS];
		bool m_abOfferingGift[MAX_MAJOR_CIVS];
		bool m_abOfferedGift[MAX_MAJOR_CIVS];
		bool m_abHelpRequestEverMade[MAX_MAJOR_CIVS];
		short m_aiHelpRequestCounter[MAX_MAJOR_CIVS];
		short m_aiHelpRequestTooSoonNumTurns[MAX_MAJOR_CIVS];
		bool m_abDemandAcceptedWhenVassal[MAX_MAJOR_CIVS];
		short m_aiNumTimesDemandedWhenVassal[MAX_MAJOR_CIVS];
		short m_aiPlayerVassalageProtectValue[MAX_MAJOR_CIVS];
		short m_aiPlayerVassalageFailedProtectValue[MAX_MAJOR_CIVS];
		bool m_abMasterLiberatedMeFromVassalage[MAX_MAJOR_CIVS];
		short m_aiPlayerVassalageTurnsSincePeacefullyRevokedVassalage[MAX_MAJOR_CIVS];
		short m_aiPlayerVassalageTurnsSinceForcefullyRevokedVassalage[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenVassalAgreement[MAX_MAJOR_CIVS];
		short m_aiBrokenVassalAgreementTurn[MAX_MAJOR_CIVS];	
		bool m_abVassalTaxRaised[MAX_MAJOR_CIVS];
		bool m_abVassalTaxLowered[MAX_MAJOR_CIVS];
		int m_aiVassalGoldPerTurnTaxedSinceVassalStarted[MAX_MAJOR_CIVS];
		int m_aiVassalGoldPerTurnCollectedSinceVassalStarted[MAX_MAJOR_CIVS];
#endif

		// 2D ARRAYS
		// Diplomatic Interactions
		DiploLogData m_aaDiploStatementsLog[MAX_MAJOR_CIVS * MAX_DIPLO_LOG_STATEMENTS];
		DiploLogData* m_apaDiploStatementsLog[MAX_MAJOR_CIVS];
		bool m_aabSentAttackMessageToMinorCivProtector[MAX_MAJOR_CIVS * MAX_MINOR_CIVS];
		bool* m_apabSentAttackMessageToMinorCivProtector[MAX_MAJOR_CIVS];

		// Opinion & Approach
		int m_aaiApproachValues[MAX_MAJOR_CIVS * NUM_MAJOR_CIV_APPROACHES];
		int* m_apaiApproachValues[MAX_MAJOR_CIVS];

		// Coop Wars
		char m_aaeCoopWarAcceptedState[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
		char* m_apaeCoopWarAcceptedState[MAX_MAJOR_CIVS];
		short m_aaiCoopWarCounter[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
		short* m_apaiCoopWarCounter[MAX_MAJOR_CIVS];

		// GUESSES
		// Guesses about other players' interactions with the world
		char m_aaeOtherPlayerMajorCivOpinion[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
		char* m_apaeOtherPlayerMajorCivOpinion[MAX_MAJOR_CIVS];
		char m_aaeOtherPlayerMajorCivApproach[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
		char* m_apaeOtherPlayerMajorCivApproach[MAX_MAJOR_CIVS];
		short m_aaiOtherPlayerMajorCivApproachCounter[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
		short* m_apaiOtherPlayerMajorCivApproachCounter[MAX_MAJOR_CIVS];
		int m_aaiOtherPlayerWarValueLost[MAX_CIV_PLAYERS * MAX_CIV_PLAYERS];
		int* m_apaiOtherPlayerWarValueLost[MAX_CIV_PLAYERS];
		int m_aaiOtherPlayerLastRoundWarValueLost[MAX_CIV_PLAYERS * MAX_CIV_PLAYERS];
		int* m_apaiOtherPlayerLastRoundWarValueLost[MAX_CIV_PLAYERS];
		char m_aaeOtherPlayerWarDamageLevel[MAX_CIV_PLAYERS * MAX_CIV_PLAYERS];
		char* m_apaeOtherPlayerWarDamageLevel[MAX_CIV_PLAYERS];
		char m_aaeOtherPlayerMilitaryThreat[MAX_MAJOR_CIVS * MAX_CIV_PLAYERS];
		char* m_apaeOtherPlayerMilitaryThreat[MAX_MAJOR_CIVS];
		char m_aaeOtherPlayerLandDisputeLevel[MAX_MAJOR_CIVS * MAX_CIV_PLAYERS];
		char* m_apaeOtherPlayerLandDisputeLevel[MAX_MAJOR_CIVS];
		char m_aaeOtherPlayerVictoryDisputeLevel[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
		char* m_apaeOtherPlayerVictoryDisputeLevel[MAX_MAJOR_CIVS];

		// Miscellaneous
#if defined(MOD_ACTIVE_DIPLOMACY)
		float m_aTradePriority[MAX_MAJOR_CIVS]; // current ai to human trade priority
#endif
	};

	DiplomacyAIData* m_pDiploData;

	// Personality Values
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
	char m_eDiploPersonalityType;

	// Diplomatic Interactions
	// (Memory of what we've talked about with people)
	DeclarationLogData* m_paDeclarationsLog;
	DiploLogData** m_ppaDiploStatementsLog;
	short* m_paDiploLogStatementTurnCountScratchPad; // CANDIDATE FOR DELETION!
	bool** m_ppaabSentAttackMessageToMinorCivProtector;

	// Opinion & Approach
	char* m_paeMajorCivOpinion;
	char* m_paeMajorCivApproach;
	char* m_paeWarFace;
	int** m_ppaaiApproachValues;

	// Minor Civs
	char* m_paeMinorCivApproach;
	bool* m_pabWantToRouteToMinor;

	// Planning Exchanges
	short* m_paiDoFValue; // RENAMING LIKELY
	short* m_paiCompetitorValue;
	short* m_paiDefensivePactValue;
	bool* m_pabWantsDoFWithPlayer; // RENAMING LIKELY
	bool* m_pabWantsDefensivePactWithPlayer;
	bool* m_pabWantsResearchAgreementWithPlayer;

	// Exchanges
	bool* m_pabDoFAccepted; // CANDIDATE FOR DELETION!
	short* m_paiDoFCounter;
	char* m_paeDoFType; // RENAMING LIKELY
	bool* m_pabDenouncedPlayer; // CANDIDATE FOR DELETION!
	short* m_paiDenouncedPlayerCounter;
	bool* m_pabCantMatchDeal;
	int* m_paiDemandEverMade;
	short* m_paiDemandMadeTurn; // CANDIDATE FOR DELETION!
	short* m_paiDemandCounter; // CANDIDATE FOR DELETION!
	short* m_paiDemandTooSoonNumTurns; // CANDIDATE FOR DELETION!
	short* m_paiTradeValue;
	short* m_paiAssistValue;
	short* m_paiCommonFoeValue;

	// Coop Wars
	char** m_ppaaeCoopWarAcceptedState; // RENAMING LIKELY
	short** m_ppaaiCoopWarCounter; // RENAMING LIKELY
	short* m_paiNumTimesCoopWarDenied; // RENAMING LIKELY

	// War
	bool* m_pabWantsSneakAttack; // RENAMING LIKELY
	bool* m_pabArmyInPlaceForAttack;
	short* m_paiPlayerNumTurnsAtWar;
	short* m_paiPlayerNumTurnsSinceCityCapture;
	short* m_paiNumWarsFought;
	short* m_paiNumWarsDeclaredOnUs;
	short* m_paiNumCitiesCaptured;
	int* m_paiWarValueLost;
	short* m_paiWarDamageValue;
	char* m_paeWarDamageLevel;
	char* m_paeWarState;
	char* m_paeWarProjection;
	char* m_paeLastWarProjection;
	char* m_paeWarGoal;

	// Peace
	short* m_paiPlayerNumTurnsAtPeace;
	char* m_paePeaceTreatyWillingToOffer;
	char* m_paePeaceTreatyWillingToAccept;
	short* m_paiWantPeaceCounter;

	// Backstabbing Penalties
	bool* m_pabUntrustworthyFriend;
	bool* m_pabEverBackstabbedBy;
	bool* m_pabDoFBroken; // CANDIDATE FOR DELETION!
	short* m_paiDoFBrokenTurn;	
	bool* m_pabFriendDenouncedUs; // CANDIDATE FOR DELETION!
	short* m_paiFriendDenouncedUsTurn;
	bool* m_pabFriendDeclaredWarOnUs; // CANDIDATE FOR DELETION!
	short* m_paiFriendDeclaredWarOnUsTurn;
	short* m_paiPlayerBackstabCounter; // CANDIDATE FOR DELETION!

	// Warmongering Penalties
	char* m_paiOtherPlayerNumMinorsAttacked;
	char* m_paiOtherPlayerNumMinorsConquered;
	char* m_paiOtherPlayerNumMajorsAttacked;
	char* m_paiOtherPlayerNumMajorsConquered;
	int* m_paiOtherPlayerWarmongerAmountTimes100;

	// Aggressive Postures
	char* m_paeMilitaryAggressivePosture;
	char* m_paeLastTurnMilitaryAggressivePosture;
	char* m_paeExpansionAggressivePosture;
	char* m_paePlotBuyingAggressivePosture;

	// Dispute Levels
	char* m_paePlayerLandDisputeLevel; // RENAMING LIKELY
	char* m_paePlayerLastTurnLandDisputeLevel; // RENAMING LIKELY
	char* m_paePlayerVictoryDisputeLevel; // RENAMING LIKELY
	char* m_paePlayerVictoryBlockLevel; // RENAMING LIKELY
	char* m_paePlayerWonderDisputeLevel; // RENAMING LIKELY
	char* m_paePlayerMinorCivDisputeLevel; // RENAMING LIKELY

	// Threat Levels
	char* m_paeMilitaryThreat;
	char* m_paeWarmongerThreat;

	// Strength Assessments
	char* m_paePlayerMilitaryStrengthComparedToUs; // RENAMING LIKELY
	char* m_paePlayerEconomicStrengthComparedToUs; // RENAMING LIKELY
	char* m_paePlayerTargetValue; // RENAMING LIKELY
	bool* m_pabEasyTarget;

	// PROMISES
	// Military Promise
	bool* m_pabPlayerMadeMilitaryPromise; // CANDIDATE FOR DELETION!
	bool* m_pabPlayerBrokenMilitaryPromise; // RENAMING LIKELY
	bool* m_pabPlayerIgnoredMilitaryPromise; // RENAMING LIKELY
	short* m_paiPlayerMilitaryPromiseCounter; // CANDIDATE FOR DELETION!
	short* m_paiBrokenMilitaryPromiseTurn;

	// Expansion Promise
	bool* m_pabPlayerNoSettleRequestAccepted; // CANDIDATE FOR DELETION!
	short* m_paiPlayerNoSettleRequestCounter; // CANDIDATE FOR DELETION!
	short* m_paiPlayerMadeExpansionPromiseTurn; // RENAMING LIKELY
	bool* m_pabPlayerBrokenExpansionPromise; // RENAMING LIKELY
	bool* m_pabPlayerIgnoredExpansionPromise; // RENAMING LIKELY
	char* m_paePlayerExpansionPromiseData; // CANDIDATE FOR DELETION!
	pair<int,int>* m_paNoExpansionPromise; // CANDIDATE FOR DELETION!
	pair<int,int>* m_paLastTurnEmpireDistance; // CANDIDATE FOR DELETION!
	short* m_paiBrokenExpansionPromiseValue; // CANDIDATE FOR DELETION!
	short* m_paiIgnoredExpansionPromiseValue; // CANDIDATE FOR DELETION!
	bool* m_pabPlayerEverMadeExpansionPromise; // CANDIDATE FOR DELETION!
	bool* m_pabPlayerNoSettleRequestEverAsked; // CANDIDATE FOR DELETION!

	// Border Promise
	short* m_paiPlayerMadeBorderPromiseTurn; // RENAMING LIKELY
	bool* m_pabPlayerBrokenBorderPromise; // RENAMING LIKELY
	bool* m_pabPlayerIgnoredBorderPromise; // RENAMING LIKELY
	char* m_paePlayerBorderPromiseData; // RENAMING LIKELY
	short* m_paiBrokenBorderPromiseValue; // CANDIDATE FOR DELETION!
	short* m_paiIgnoredBorderPromiseValue; // CANDIDATE FOR DELETION!
	bool* m_pabPlayerEverMadeBorderPromise; // CANDIDATE FOR DELETION!

	// Bully City-State Promise
	bool* m_pabPlayerMadeBullyCityStatePromise; // RENAMING LIKELY
	bool* m_pabPlayerBrokenBullyCityStatePromise; // RENAMING LIKELY
	bool* m_pabPlayerIgnoredBullyCityStatePromise; // RENAMING LIKELY

	// Attack City-State Promise
	bool* m_pabPlayerMadeAttackCityStatePromise; // RENAMING LIKELY
	bool* m_pabPlayerBrokenAttackCityStatePromise; // CANDIDATE FOR DELETION!
	bool* m_pabPlayerIgnoredAttackCityStatePromise; // RENAMING LIKELY
	short* m_paiBrokenAttackCityStatePromiseTurn;

	// Spy Promise
	bool* m_pabPlayerStopSpyingRequestAccepted; // CANDIDATE FOR DELETION!
	short* m_paiPlayerStopSpyingRequestCounter; // CANDIDATE FOR DELETION!
	bool* m_pabPlayerMadeSpyPromise; // RENAMING LIKELY
	bool* m_pabPlayerBrokenSpyPromise; // RENAMING LIKELY
	bool* m_pabPlayerIgnoredSpyPromise; // RENAMING LIKELY
	bool* m_pabPlayerStopSpyingRequestEverAsked; // CANDIDATE FOR DELETION!

	// Religious Conversion Promise
	bool* m_pabPlayerAskedNotToConvert; // RENAMING LIKELY
	bool* m_pabPlayerAgreedNotToConvert; // RENAMING LIKELY
	bool* m_pabPlayerMadeNoConvertPromise; // RENAMING LIKELY
	bool* m_pabPlayerBrokenNoConvertPromise; // RENAMING LIKELY
	bool* m_pabPlayerIgnoredNoConvertPromise; // RENAMING LIKELY
	bool* m_pabEverConvertedCity;

	// Digging Promise
	bool* m_pabPlayerAskedNotToDig; // RENAMING LIKELY
	bool* m_pabPlayerAgreedNotToDig; // RENAMING LIKELY
	bool* m_pabPlayerMadeNoDiggingPromise; // RENAMING LIKELY
	bool* m_pabPlayerBrokenNoDiggingPromise; // RENAMING LIKELY
	bool* m_pabPlayerIgnoredNoDiggingPromise; // RENAMING LIKELY

	// Coop War Promise
	bool* m_pabPlayerBrokenCoopWarPromise; // RENAMING LIKELY

	// OTHER DIPLOMATIC VALUES
	// Event flags
	bool* m_pabPlayerForgaveForSpying; // CANDIDATE FOR DELETION!
	bool* m_pabPlayerLiberatedCapital; // RENAMING LIKELY
	bool* m_pabDoFEverAsked;
	bool* m_pabPlayerCapturedCapital; // RENAMING LIKELY
	bool* m_pabPlayerCapturedHolyCity; // RENAMING LIKELY

	// # of times/points counters
	short* m_paiNumCitiesLiberated;
	short* m_paiNumCiviliansReturnedToMe;
	short* m_paiNumTimesIntrigueSharedBy;
	short* m_paiNumLandmarksBuiltForMe;
	char* m_paiTheyPlottedAgainstUs; // RENAMING LIKELY
	short* m_paiNumTimesRazed;
	short* m_paiNumTradeRoutesPlundered;
	short* m_paiNumWondersBeatenTo;
	short* m_paiNumTimesCultureBombed; // RENAMING LIKELY
	char* m_paiTheyLoweredOurInfluence; // RENAMING LIKELY
	char* m_paiOtherPlayerNumProtectedMinorsBullied; // RENAMING LIKELY
	char* m_paiOtherPlayerNumProtectedMinorsAttacked; // RENAMING LIKELY
	char* m_paiOtherPlayerNumProtectedMinorsKilled; // RENAMING LIKELY
	short* m_paiNegativeReligiousConversionPoints;
	short* m_paiNumTimesRobbedBy;
	char* m_paiPerformedCoupAgainstUs; // RENAMING LIKELY
	short* m_paiNegativeArchaeologyPoints;
	int* m_paiArtifactsEverDugUp; // RENAMING LIKELY
	short* m_paiNumTimesNuked;

	// Turn counters
	short* m_paiResurrectedOnTurn;
	short* m_paiLiberatedCitiesTurn;
	short* m_paiCiviliansReturnedToMeTurn;
	short* m_paiIntrigueSharedTurn;
	short* m_paiPlayerForgaveForSpyingTurn;
	short* m_paiNumLandmarksBuiltForMeTurn; // RENAMING LIKELY
	short* m_paiPlottedAgainstUsTurn;
	short* m_paiPlunderedTradeRouteTurn;
	short* m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor; // RENAMING LIKELY
	short* m_paiOtherPlayerTurnsSinceAttackedProtectedMinor; // RENAMING LIKELY
	short* m_paiOtherPlayerTurnsSinceKilledProtectedMinor; // RENAMING LIKELY
	short* m_paiReligiousConversionTurn;
	short* m_paiTimesRobbedTurn; // RENAMING LIKELY
	short* m_paiPerformedCoupTurn;
	short* m_paiOtherPlayerTurnsSinceWeLikedTheirProposal; // RENAMING LIKELY
	short* m_paiOtherPlayerTurnsSinceWeDislikedTheirProposal; // RENAMING LIKELY
	short* m_paiOtherPlayerTurnsSinceTheySupportedOurProposal; // RENAMING LIKELY
	short* m_paiOtherPlayerTurnsSinceTheyFoiledOurProposal; // RENAMING LIKELY
	short* m_paiOtherPlayerTurnsSinceTheySupportedOurHosting; // RENAMING LIKELY

	// Player-specific memory
	char* m_paiOtherPlayerProtectedMinorAttacked; // CANDIDATE FOR DELETION!
	char* m_paiOtherPlayerProtectedMinorKilled; // CANDIDATE FOR DELETION!

	// GUESSES
	// Guesses about other players' feelings towards us
	char* m_paeOpinionTowardsUsGuess;
	char* m_paeApproachTowardsUsGuess;
	char* m_paiApproachTowardsUsGuessCounter;

	// Guesses about other players' interactions with the world
	char** m_ppaaeOtherPlayerMajorCivOpinion;
	char** m_ppaaeOtherPlayerMajorCivApproach;
	short** m_ppaaiOtherPlayerMajorCivApproachCounter;
	int** m_ppaaiOtherPlayerWarValueLost;
	int** m_ppaaiOtherPlayerLastRoundWarValueLost; // CANDIDATE FOR DELETION!
	char** m_ppaaeOtherPlayerWarDamageLevel;
	char** m_ppaaeOtherPlayerMilitaryThreat;
	char** m_ppaaeOtherPlayerLandDisputeLevel;
	char** m_ppaaeOtherPlayerVictoryDisputeLevel;

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// C4DF Values
	bool* m_pabShareOpinionAccepted; // CANDIDATE FOR DELETION!
	short* m_paiShareOpinionCounter; // CANDIDATE FOR DELETION!
	bool* m_pabMoveTroopsRequestAccepted; // CANDIDATE FOR DELETION!
	short* m_paiMoveTroopsRequestCounter; // CANDIDATE FOR DELETION!
	bool* m_pabOfferingGift;
	bool* m_pabOfferedGift;
	bool* m_pabHelpRequestEverMade;
	short* m_paiHelpRequestCounter; // CANDIDATE FOR DELETION!
	short* m_paiHelpRequestTooSoonNumTurns; // CANDIDATE FOR DELETION!
	bool* m_pabDemandAcceptedWhenVassal; // RENAMING LIKELY
	short* m_paiNumTimesDemandedWhenVassal; // CANDIDATE FOR DELETION!
	short* m_paiPlayerVassalageProtectValue; // RENAMING LIKELY
	short* m_paiPlayerVassalageFailedProtectValue; // RENAMING LIKELY
	bool* m_pabMasterLiberatedMeFromVassalage;
	short* m_paiPlayerVassalageTurnsSincePeacefullyRevokedVassalage; // RENAMING LIKELY
	short* m_paiPlayerVassalageTurnsSinceForcefullyRevokedVassalage; // RENAMING LIKELY
	bool* m_pabPlayerBrokenVassalAgreement; // CANDIDATE FOR DELETION!
	short* m_paiBrokenVassalAgreementTurn;
	bool* m_pabVassalTaxRaised;
	bool* m_pabVassalTaxLowered;
	int* m_paiVassalGoldPerTurnTaxedSinceVassalStarted; // CANDIDATE FOR DELETION!
	int* m_paiVassalGoldPerTurnCollectedSinceVassalStarted; // CANDIDATE FOR DELETION!
#endif

	// Overall status across all other civs
	StateAllWars m_eStateAllWars;
	char m_eDemandTargetPlayer;
	bool m_bDemandReady;
	char m_eCSWarTarget;
	char m_eCSBullyTarget;

	// Miscellaneous
	typedef std::vector<PlayerTypes> PlayerTypesArray;
	PlayerTypesArray	m_aGreetPlayers;
	// JdH =>
	void DoUpdateHumanTradePriority(PlayerTypes ePlayer, int iOpinionWeight);
	// JdH <=
	DiplomacyPlayerType	m_eTargetPlayer;
#if defined(MOD_ACTIVE_DIPLOMACY)
	DiplomacyPlayerType	m_eTargetPlayerType;
#endif
	// Data members for injecting test messages  // CANDIDATES FOR DELETION!
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