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
	// This has been moved to CvEnums.h to make compatible for MOD_ACTIVE_DIPLOMACY
#if !defined(MOD_ACTIVE_DIPLOMACY)
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

	CvDiplomacyAI(void);
	~CvDiplomacyAI(void);
	void Init(CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;

	void update();

	CvPlayer* GetPlayer();
	TeamTypes GetTeam();

	void DoInitializePersonality();
	int GetRandomPersonalityWeight(int iOriginalValue) const;

	/////////////////////////////////////////////////////////
	// Turn Stuff
	/////////////////////////////////////////////////////////

	void DoTurn(DiplomacyPlayerType eTargetPlayer);
	void DoCounters();

	/////////////////////////////////////////////////////////
	// Opinion
	/////////////////////////////////////////////////////////

	void DoUpdateOpinions();
	void DoUpdateOnePlayerOpinion(PlayerTypes ePlayer);

	int GetMajorCivOpinionWeight(PlayerTypes ePlayer);
	MajorCivOpinionTypes GetMajorCivOpinion(PlayerTypes ePlayer) const;
	void SetMajorCivOpinion(PlayerTypes ePlayer, MajorCivOpinionTypes eOpinion);
	int GetNumMajorCivOpinion(MajorCivOpinionTypes eOpinion) const;

	void DoEstimateOtherPlayerOpinions();

	MajorCivOpinionTypes GetMajorCivOtherPlayerOpinion(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const;
	void SetMajorCivOtherPlayerOpinion(PlayerTypes ePlayer, PlayerTypes eWithPlayer, MajorCivOpinionTypes ePlayerOpinion);

	/////////////////////////////////////////////////////////
	// Approach
	/////////////////////////////////////////////////////////

	// Major Civs
	void DoUpdateMajorCivApproaches();
	MajorCivApproachTypes GetBestApproachTowardsMajorCiv(PlayerTypes ePlayer, int& iHighestWeight, bool bLookAtOtherPlayers, bool bLog, WarFaceTypes& eWarFace);

	MajorCivApproachTypes GetMajorCivApproach(PlayerTypes ePlayer, bool bHideTrueFeelings) const;
	void SetMajorCivApproach(PlayerTypes ePlayer, MajorCivApproachTypes eApproach);
	int GetNumMajorCivApproach(MajorCivApproachTypes eApproach) const;

	// Minor Civs
	void DoUpdateMinorCivApproaches();
	MinorCivApproachTypes GetBestApproachTowardsMinorCiv(PlayerTypes ePlayer, int& iHighestWeight, bool bLookAtOtherPlayers, bool bLog);

	MinorCivApproachTypes GetMinorCivApproach(PlayerTypes ePlayer) const;
	void SetMinorCivApproach(PlayerTypes ePlayer, MinorCivApproachTypes eApproach);
	int GetNumMinorCivApproach(MinorCivApproachTypes eApproach) const;

	void SetWantToRouteConnectToMinor(PlayerTypes eMinor, bool bWant);
	bool IsWantToRouteConnectToMinor(PlayerTypes eMinor);

	bool IsHasActiveGoldQuest();

	// Our guess as to another player's approach towards us
	MajorCivApproachTypes GetApproachTowardsUsGuess(PlayerTypes ePlayer) ;
	void SetApproachTowardsUsGuess(PlayerTypes ePlayer, MajorCivApproachTypes eApproach);
	int GetApproachTowardsUsGuessCounter(PlayerTypes ePlayer) const;
	void SetApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iValue);
	void ChangeApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iChange);

	void DoUpdateApproachTowardsUsGuesses();

	/////////////////////////////////////////////////////////
	// Demands
	/////////////////////////////////////////////////////////

	PlayerTypes GetDemandTargetPlayer() const;
	void SetDemandTargetPlayer(PlayerTypes ePlayer);
	void DoUpdateDemands();

	void DoStartDemandProcess(PlayerTypes ePlayer);
	void DoCancelHaltDemandProcess();

	void DoTestDemandReady();

	bool IsDemandReady();
	void SetDemandReady(bool bValue);

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

	bool IsWillingToMakePeaceWithHuman(PlayerTypes ePlayer);

	// What are we willing to give up to ePlayer to make peace?
	PeaceTreatyTypes GetTreatyWillingToOffer(PlayerTypes ePlayer) const;
	void SetTreatyWillingToOffer(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty);

	// What are we willing to accept from ePlayer to make peace?
	PeaceTreatyTypes GetTreatyWillingToAccept(PlayerTypes ePlayer) const;
	void SetTreatyWillingToAccept(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty);

	// Do we want peace with ePlayer? (looks at WantPeaceCounter)
	bool IsWantsPeaceWithPlayer(PlayerTypes ePlayer) const;

	// Want Peace Counter: how long have we wanted peace with ePlayer? (Looks at WarGoal)
	int GetWantPeaceCounter(PlayerTypes ePlayer) const;
	void SetWantPeaceCounter(PlayerTypes ePlayer, int iValue);
	void ChangeWantPeaceCounter(PlayerTypes ePlayer, int iChange);

	/////////////////////////////////////////////////////////
	// War & Military Assessment
	/////////////////////////////////////////////////////////

	void MakeWar();
	void DeclareWar(PlayerTypes ePlayer);

	// War Face: If we're planning for war, how are we acting towards ePlayer?
	WarFaceTypes GetWarFaceWithPlayer(PlayerTypes ePlayer) const;
	void SetWarFaceWithPlayer(PlayerTypes ePlayer, WarFaceTypes eFace);

	// Mustering For Attack: Is there Sneak Attack Operation completed and ready to roll against ePlayer?
	bool IsMusteringForAttack(PlayerTypes ePlayer) const;
	void SetMusteringForAttack(PlayerTypes ePlayer, bool bValue);

	// Set default values when we're attacked and its not our turn
	void DoSomeoneDeclaredWarOnMe(TeamTypes eTeam);

	// War State: How's the war with ePlayer going? (NO_WAR_STATE_TYPE if at peace)
	WarStateTypes GetWarState(PlayerTypes ePlayer) const;
	void SetWarState(PlayerTypes ePlayer, WarStateTypes eWarState);
	void DoUpdateWarStates();

	StateAllWars GetStateAllWars() const;
	void SetStateAllWars(StateAllWars eState);

	// War Projection: How do we think the war with ePlayer is going to end up? (NO_WAR_PROJECTION_TYPE if at peace)
	WarProjectionTypes GetWarProjection(PlayerTypes ePlayer) const;
	void SetWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection);
	WarProjectionTypes GetLastWarProjection(PlayerTypes ePlayer) const; // previous value of GetWarProjection
	void SetLastWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection);
	void DoUpdateWarProjections();

	int GetHighestWarscore();

#if defined(MOD_BALANCE_CORE)
	int GetWarScore(PlayerTypes ePlayer, bool bUsePeacetimeCalculation = false, bool bDebug=false);
#else
	int GetWarScore(PlayerTypes ePlayer);
#endif

	// War Goal: What is is our objective in the war against ePlayer (NO_WAR_GOAL_TYPE if at peace)
	WarGoalTypes GetWarGoal(PlayerTypes ePlayer) const;
	void SetWarGoal(PlayerTypes ePlayer, WarGoalTypes eWarGoal);
	void DoUpdateWarGoals();

	// Num Turns At War
#if defined(MOD_API_EXTENSIONS)
	int GetTeamNumTurnsAtWar(TeamTypes eTeam) const;
#endif
	int GetPlayerNumTurnsAtWar(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsAtWar(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsAtWar(PlayerTypes ePlayer, int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetPlayerNumTurnsSinceCityCapture(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsSinceCityCapture(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsSinceCityCapture(PlayerTypes ePlayer, int iChange);

	int GetPlayerNumTurnsAtPeace(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsAtPeace(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsAtPeace(PlayerTypes ePlayer, int iChange);
#endif
	// How many times have we gone to war?
	int GetNumWarsFought(PlayerTypes ePlayer) const;
	void SetNumWarsFought(PlayerTypes ePlayer, int iValue);
	void ChangeNumWarsFought(PlayerTypes ePlayer, int iChange);

	// Military Strength: How strong is ePlayer compared to US?
	StrengthTypes GetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer) const;
	void SetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eMilitaryStrength);
	void DoUpdatePlayerMilitaryStrengths();
	void DoUpdateOnePlayerMilitaryStrength(PlayerTypes ePlayer);

	// Economic Strength: How strong is ePlayer compared to US?
	StrengthTypes GetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer) const;
	void SetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eEconomicStrength);
	void DoUpdatePlayerEconomicStrengths();
	void DoUpdateOnePlayerEconomicStrength(PlayerTypes ePlayer);

	// Target Value: how easy or hard of a target would ePlayer be to attack?
	TargetValueTypes GetPlayerTargetValue(PlayerTypes ePlayer) const;
	void SetPlayerTargetValue(PlayerTypes ePlayer, TargetValueTypes eTargetValue);
	void DoUpdatePlayerTargetValues();
	void DoUpdateOnePlayerTargetValue(PlayerTypes ePlayer);

	// War Damage Level: how much damage have we taken in a war against ePlayer? Looks at WarValueLost
	WarDamageLevelTypes GetWarDamageLevel(PlayerTypes ePlayer) const;
	void SetWarDamageLevel(PlayerTypes ePlayer, WarDamageLevelTypes eDamageLevel);
	void DoUpdateWarDamageLevel();
#if defined(MOD_BALANCE_CORE)
	int GetWarDamageValue(PlayerTypes ePlayer) const;
	void SetWarDamageValue(PlayerTypes ePlayer, int iValue);
#endif
	void DoWarDamageDecay();

	// War Value Lost: the int value of damage ePlayer has inflicted on us in war
	int GetWarValueLost(PlayerTypes ePlayer) const;
	void SetWarValueLost(PlayerTypes ePlayer, int iValue);
	void ChangeWarValueLost(PlayerTypes ePlayer, int iChange);

	// Other Player War Damage Level: how much damage we've inflicted UPON ePlayer
	WarDamageLevelTypes GetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const;
	void SetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, WarDamageLevelTypes eDamageLevel);
	void DoUpdateOtherPlayerWarDamageLevel();

	// Other Player War Value Lost: the int value of damage we've inflicted UPON ePlayer in war
	int GetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const;
	void SetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue);
	void ChangeOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iChange);

	// Stores current war values, for comparison later on
	void CacheOtherPlayerWarValuesLost();

	// Other Player Last Round War Value Lost: the int value of damage we've inflicted UPON ePlayer in war up until this turn
	int GetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const;
	void SetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue);

	// Military Aggressive Posture: How aggressively has ePlayer positioned their Units in relation to us?
	AggressivePostureTypes GetMilitaryAggressivePosture(PlayerTypes ePlayer) const;
	void SetMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);
	void DoUpdateMilitaryAggressivePostures();
	void DoUpdateOnePlayerMilitaryAggressivePosture(PlayerTypes ePlayer);

	AggressivePostureTypes GetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer) const;
	void SetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);

	/////////////////////////////////////////////////////////
	// Threats to this Player
	/////////////////////////////////////////////////////////

	ThreatTypes GetMilitaryThreat(PlayerTypes ePlayer) const;
	void SetMilitaryThreat(PlayerTypes ePlayer, ThreatTypes eMilitaryThreat);
	void DoUpdateMilitaryThreats();

	ThreatTypes GetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const;
	void SetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eWithPlayer, ThreatTypes eThreatType);
	void DoUpdateEstimateOtherPlayerMilitaryThreats();

	// Warmonger Threat - how much of a threat are these guys to run amok and break everything
	ThreatTypes GetWarmongerThreat(PlayerTypes ePlayer) const;
	void SetWarmongerThreat(PlayerTypes ePlayer, ThreatTypes eWarmongerThreat);
	void DoUpdateWarmongerThreats();

#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	int GetNumberOfThreatenedCities(PlayerTypes eEnemy);
#endif

	/////////////////////////////////////////////////////////
	// Planning Exchanges
	/////////////////////////////////////////////////////////

	void DoUpdatePlanningExchanges();

	bool IsWantsResearchAgreementWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsResearchAgreementWithPlayer(PlayerTypes ePlayer, bool bValue);

	int GetNumResearchAgreementsWanted() const;

	void DoAddWantsResearchAgreementWithPlayer(PlayerTypes ePlayer);
	void DoCancelWantsResearchAgreementWithPlayer(PlayerTypes ePlayer);
	bool IsCanMakeResearchAgreementRightNow(PlayerTypes ePlayer);

#if defined(MOD_BALANCE_CORE_DEALS)
	bool IsWantsSneakAttack(PlayerTypes ePlayer) const;
	void SetWantsSneakAttack(PlayerTypes ePlayer, bool bValue);

	bool IsWantsDefensivePactWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsDefensivePactWithPlayer(PlayerTypes ePlayer, bool bValue);

	int GetNumDefensivePactsWanted(PlayerTypes ePlayer = NO_PLAYER) const;

	void DoAddWantsDefensivePactWithPlayer(PlayerTypes ePlayer);
	void DoCancelWantsDefensivePactWithPlayer(PlayerTypes ePlayer);
	bool IsCanMakeDefensivePactRightNow(PlayerTypes ePlayer);

	bool IsGoodChoiceForDefensivePact(PlayerTypes ePlayer);
#endif
	/////////////////////////////////////////////////////////
	// Issues of Dispute
	/////////////////////////////////////////////////////////

	// Land Dispute
	DisputeLevelTypes GetLandDisputeLevel(PlayerTypes ePlayer) const;
	void SetLandDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);
	void DoUpdateLandDisputeLevels();
	int GetTotalLandDisputeLevel();

	DisputeLevelTypes GetLastTurnLandDisputeLevel(PlayerTypes ePlayer) const;

	DisputeLevelTypes GetEstimateOtherPlayerLandDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const;
	void SetEstimateOtherPlayerLandDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer, DisputeLevelTypes eDisputeLevel);
	void DoUpdateEstimateOtherPlayerLandDisputeLevels();

	bool IsPlayerRecklessExpander(PlayerTypes ePlayer);

	int GetBrokenExpansionPromiseValue(PlayerTypes ePlayer);
	void SetBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iChange);
	int GetIgnoredExpansionPromiseValue(PlayerTypes ePlayer);
	void SetIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iChange);
	int GetBrokenBorderPromiseValue(PlayerTypes ePlayer);
	void SetBrokenBorderPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeBrokenBorderPromiseValue(PlayerTypes ePlayer, int iChange);
	int GetIgnoredBorderPromiseValue(PlayerTypes ePlayer);
	void SetIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iValue);
	void ChangeIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iChange);

	int GetDeclaredWarOnFriendValue(PlayerTypes ePlayer);
	void ChangeDeclaredWarOnFriendValue(PlayerTypes ePlayer, int iChange);

	int GetNumCitiesLiberated(PlayerTypes ePlayer);
	void ChangeNumCitiesLiberated(PlayerTypes ePlayer, int iChange);

	int GetRecentTradeValue(PlayerTypes ePlayer);
	void ChangeRecentTradeValue(PlayerTypes ePlayer, int iChange);
	int GetCommonFoeValue(PlayerTypes ePlayer);
	void ChangeCommonFoeValue(PlayerTypes ePlayer, int iChange);
	int GetRecentAssistValue(PlayerTypes ePlayer);
	void ChangeRecentAssistValue(PlayerTypes ePlayer, int iChange);

	bool IsGaveAssistanceTo(PlayerTypes ePlayer);
	bool IsHasPaidTributeTo(PlayerTypes ePlayer);
	bool IsNukedBy(PlayerTypes ePlayer);
	bool IsCapitalCapturedBy(PlayerTypes ePlayer);
	bool IsHolyCityCapturedBy(PlayerTypes ePlayer);
	bool IsAngryAboutProtectedMinorKilled(PlayerTypes ePlayer);
	bool IsAngryAboutProtectedMinorAttacked(PlayerTypes ePlayer);
	bool IsAngryAboutProtectedMinorBullied(PlayerTypes ePlayer);
	bool IsAngryAboutSidedWithTheirProtectedMinor(PlayerTypes ePlayer);
	
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

	// Expansion Aggressive Posture: How aggressively has ePlayer positioned their Units in relation to us?
	AggressivePostureTypes GetExpansionAggressivePosture(PlayerTypes ePlayer) const;
	void SetExpansionAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);
	void DoUpdateExpansionAggressivePostures();
	void DoUpdateOnePlayerExpansionAggressivePosture(PlayerTypes ePlayer);

	// Plot Buying Aggressive Posture: How aggressively is ePlayer buying land near us?
	AggressivePostureTypes GetPlotBuyingAggressivePosture(PlayerTypes ePlayer) const;
	void SetPlotBuyingAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);
	void DoUpdatePlotBuyingAggressivePosture();

#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	// Victory Block
	BlockLevelTypes GetVictoryBlockLevel(PlayerTypes ePlayer) const;
	void SetVictoryBlockLevel(PlayerTypes ePlayer, BlockLevelTypes eBlockLevel);
	void DoUpdateVictoryBlockLevels();

	void DoRelationshipPairing();
	
	int GetDefensivePactValue(PlayerTypes ePlayer);
	PlayerTypes GetMostValuableDefensivePact(bool bIgnoreDPs);

	int GetDoFValue(PlayerTypes ePlayer);
	PlayerTypes GetMostValuableDoF(bool bIgnoreDoFs);

	int GetCompetitorValue(PlayerTypes ePlayer);
	PlayerTypes GetBiggestCompetitor();
#endif

	// Victory Dispute
	DisputeLevelTypes GetVictoryDisputeLevel(PlayerTypes ePlayer) const;
	void SetVictoryDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);
	void DoUpdateVictoryDisputeLevels();

	DisputeLevelTypes GetEstimateOtherPlayerVictoryDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const;
	void SetEstimateOtherPlayerVictoryDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer, DisputeLevelTypes eDisputeLevel);
	void DoUpdateEstimateOtherPlayerVictoryDisputeLevels();

	// Wonder Dispute
	DisputeLevelTypes GetWonderDisputeLevel(PlayerTypes ePlayer) const;
	void SetWonderDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);
	void DoUpdateWonderDisputeLevels();

	int GetNumWondersBeatenTo(PlayerTypes ePlayer) const;
	void SetNumWondersBeatenTo(PlayerTypes ePlayer, int iNewValue);
	void ChangeNumWondersBeatenTo(PlayerTypes ePlayer, int iChange);

	// Minor Civ Diplo Dispute
	DisputeLevelTypes GetMinorCivDisputeLevel(PlayerTypes ePlayer) const;
	void SetMinorCivDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);
	void DoUpdateMinorCivDisputeLevels();

	/////////////////////////////////////////////////////////
	// Personality Members
	/////////////////////////////////////////////////////////

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

	/////////////////////////////////////////////////////////
	// Evaluation of Other Players' Tendencies
	/////////////////////////////////////////////////////////

	// Someone had some kind of interaction with another player

	void DoWeMadePeaceWithSomeone(TeamTypes eOtherTeam);
#if defined(MOD_BALANCE_CORE)
	void DoPlayerDeclaredWarOnSomeone(PlayerTypes ePlayer, TeamTypes eOtherTeam, bool bDefensivePact);
#else
	void DoPlayerDeclaredWarOnSomeone(PlayerTypes ePlayer, TeamTypes eOtherTeam);
#endif
	void DoPlayerKilledSomeone(PlayerTypes ePlayer, PlayerTypes eDeadPlayer);
	void DoPlayerBulliedSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);
	void DoPlayerMetSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);

	// Protected Minor Attacked
	int GetTurnsSincePlayerAttackedProtectedMinor(PlayerTypes ePlayer) const;
	void SetOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iChange);
	PlayerTypes GetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer, PlayerTypes eAttackedPlayer);

	int GetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iChange);

	// Protected Minors Killed
	int GetTurnsSincePlayerKilledProtectedMinor(PlayerTypes ePlayer) const;
	void SetOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iChange);
	PlayerTypes GetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer) const;
	void SetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer, PlayerTypes eKilledPlayer);

	int GetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iChange);

	// Protected Minor Bullied
	int GetTurnsSincePlayerBulliedProtectedMinor(PlayerTypes eBullyPlayer);
	PlayerTypes GetOtherPlayerProtectedMinorBullied(PlayerTypes eBullyPlayer);

	int GetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iChange);

	// They sided with their Protected Minor (after we attacked/bullied it)
	bool IsOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer);
	int GetTurnsSinceOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer);
	void SetOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iChange);

	// Num Minors Attacked
	int GetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iChange);

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

#if defined(MOD_BALANCE_CORE)
	int GetNumTimesTheyPlottedAgainstUs(PlayerTypes ePlayer) const;
	void SetNumTimesTheyPlottedAgainstUs(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesTheyPlottedAgainstUs(PlayerTypes ePlayer, int iChange);

	int GetNumTimesTheyLoweredOurInfluence(PlayerTypes ePlayer) const;
	void SetNumTimesTheyLoweredOurInfluence(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesTheyLoweredOurInfluence(PlayerTypes ePlayer, int iChange);
#endif

	// Get the amount of warmonger hatred they generated
	int GetOtherPlayerWarmongerAmount(PlayerTypes ePlayer);
#if defined(MOD_API_EXTENSIONS)
	void ChangeOtherPlayerWarmongerAmountTimes100(PlayerTypes ePlayer, int iChange);
#else
	void ChangeOtherPlayerWarmongerAmount(PlayerTypes ePlayer, int iChange);
#endif
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
	bool IsCantMatchDeal(PlayerTypes ePlayer);
	void SetCantMatchDeal(PlayerTypes ePlayer, bool bValue);
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

	bool IsCoopWarMessageTooSoon(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;

	bool IsCoopWarEverAsked(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;

	CoopWarStates GetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;
	void SetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, CoopWarStates eValue);

	short GetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;
	void SetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iValue);
	void ChangeCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iChange);

	bool DoTestContinueCoopWarsDesire(PlayerTypes ePlayer, PlayerTypes& eAgainstPlayer);
	bool IsContinueCoopWar(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer);

	CoopWarStates GetGlobalCoopWarAcceptedState(PlayerTypes ePlayer);
	int GetGlobalCoopWarCounter(PlayerTypes ePlayer);
	bool IsLockedIntoCoopWar(PlayerTypes ePlayer);

	// Human Demand
	void DoDemandMade(PlayerTypes ePlayer);
	bool IsDemandTooSoon(PlayerTypes ePlayer) const;
	short GetDemandTooSoonNumTurns(PlayerTypes ePlayer) const;

	int GetNumDemandEverMade(PlayerTypes ePlayer) const;
	bool IsDemandEverMade(PlayerTypes ePlayer) const;

	short GetDemandCounter(PlayerTypes ePlayer) const;
	void SetDemandCounter(PlayerTypes ePlayer, int iValue);
	void ChangeDemandCounter(PlayerTypes ePlayer, int iChange);

	// No Settling
	bool IsDontSettleAcceptable(PlayerTypes ePlayer) const;
	bool IsDontSettleMessageTooSoon(PlayerTypes ePlayer) const;

	bool IsPlayerNoSettleRequestEverAsked(PlayerTypes ePlayer) const;

	bool IsPlayerNoSettleRequestRejected(PlayerTypes ePlayer) const;
	bool IsPlayerNoSettleRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerNoSettleRequestAccepted(PlayerTypes ePlayer, bool bValue);

	short GetPlayerNoSettleRequestCounter(PlayerTypes ePlayer) const;
	void SetPlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iChange);

	// Stop Spying
	bool IsStopSpyingAcceptable(PlayerTypes ePlayer) const;
	bool IsStopSpyingMessageTooSoon(PlayerTypes ePlayer) const;

	bool IsPlayerStopSpyingRequestEverAsked(PlayerTypes ePlayer) const;

	bool IsPlayerStopSpyingRequestRejected(PlayerTypes ePlayer) const;
	bool IsPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer, bool bValue);

	short GetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer) const;
	void SetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iChange);

#if defined(MOD_BALANCE_CORE)
	short GetPlayerBackstabCounter(PlayerTypes ePlayer) const;
	void SetPlayerBackstabCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerBackstabCounter(PlayerTypes ePlayer, int iChange);
#endif


	// Working With Player
	bool IsDoFAcceptable(PlayerTypes ePlayer);
	bool IsTooEarlyForDoF(PlayerTypes ePlayer);
	bool IsDoFMessageTooSoon(PlayerTypes ePlayer) const;

	bool IsDoFEverAsked(PlayerTypes ePlayer) const;

	bool IsDoFRejected(PlayerTypes ePlayer) const;
	bool IsDoFAccepted(PlayerTypes ePlayer) const;
	void SetDoFAccepted(PlayerTypes ePlayer, bool bValue);
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	void SetDoFType(PlayerTypes ePlayer, DoFLevelTypes eDoFLevel);
	DoFLevelTypes GetDoFType(PlayerTypes ePlayer) const;

	void SetNumTimesCoopWarDenied(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesCoopWarDenied(PlayerTypes ePlayer, int iValue);
	int GetNumTimesCoopWarDenied(PlayerTypes ePlayer) const;

	bool IsDoFBroken(PlayerTypes ePlayer) const;
	void SetDoFBroken(PlayerTypes ePlayer, bool bValue);
#endif

	short GetDoFCounter(PlayerTypes ePlayer) const;
	void SetDoFCounter(PlayerTypes ePlayer, int iValue);
	void ChangeDoFCounter(PlayerTypes ePlayer, int iChange);

	int GetNumDoF();
#if defined(MOD_DIPLOMACY_CITYSTATES_RESOLUTIONS)
	int GetNumRA();
	int GetNumDefensePacts();
#endif
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	int GetNumDenouncements();
	int GetNumDenouncementsOfPlayer();
	int GetNumSamePolicies(PlayerTypes ePlayer);

	void SetPromiseNumberOwnedCities(PlayerTypes eOtherPlayer, int iPlotIndex);
	int GetPromiseNumberOwnedCities(PlayerTypes eOtherPlayer);

	void SetPromisePlotOtherPlayer(PlayerTypes eOtherPlayer, int iPlotIndex);
	int GetPromisePlotOtherPlayer(PlayerTypes eOtherPlayer);

	void SetLastTurnCenterofMass(PlayerTypes eOtherPlayer, int iPlotIndex);
	int GetLastTurnCenterofMass(PlayerTypes eOtherPlayer);
#endif
	bool IsDenounceFriendAcceptable(PlayerTypes ePlayer);

	bool IsPlayerDoFwithAnyFriend(PlayerTypes ePlayer) const;
	bool IsPlayerDoFwithAnyEnemy(PlayerTypes ePlayer) const;

	// Denounced Player
	void DoDenouncePlayer(PlayerTypes ePlayer);

	bool IsDenounceAcceptable(PlayerTypes ePlayer, bool bBias);
	int GetDenounceWeight(PlayerTypes ePlayer, bool bBias);

	bool IsDenouncedPlayer(PlayerTypes ePlayer) const;
	void SetDenouncedPlayer(PlayerTypes ePlayer, bool bValue);
	bool IsDenouncingPlayer(PlayerTypes ePlayer) const;

	short GetDenouncedPlayerCounter(PlayerTypes ePlayer) const;
	void SetDenouncedPlayerCounter(PlayerTypes ePlayer, int iValue);
	void ChangeDenouncedPlayerCounter(PlayerTypes ePlayer, int iChange);

	bool IsPlayerDenouncedFriend(PlayerTypes ePlayer) const;
	bool IsPlayerDenouncedEnemy(PlayerTypes ePlayer) const;

	// Requests of Friends

	PlayerTypes GetRequestFriendToDenounce(PlayerTypes ePlayer, bool& bRandFailed);
	bool IsFriendDenounceRefusalUnacceptable(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer);

	// Problems between friends

	bool IsUntrustworthyFriend();
	int GetNumFriendsDenouncedBy();

	bool IsFriendDenouncedUs(PlayerTypes ePlayer) const;	// They denounced us while we were friends!
	void SetFriendDenouncedUs(PlayerTypes ePlayer, bool bValue);
	int GetWeDenouncedFriendCount();

	bool IsFriendDeclaredWarOnUs(PlayerTypes ePlayer) const;	// They declared war on us while we were friends!
	void SetFriendDeclaredWarOnUs(PlayerTypes ePlayer, bool bValue);
	int GetWeDeclaredWarOnFriendCount();

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// Contact Statements
	void DoMapsOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoTechOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoGenerousOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);

	bool IsOfferingGift(PlayerTypes ePlayer) const;	// We're offering a gift!
	void SetOfferingGift(PlayerTypes ePlayer, bool bValue);

	bool IsOfferedGift(PlayerTypes ePlayer) const;	// We offered a gift!
	void SetOfferedGift(PlayerTypes ePlayer, bool bValue);

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
	bool IsShareOpinionTooSoon(PlayerTypes ePlayer) const;
	
	bool IsShareOpinionAccepted(PlayerTypes ePlayer) const;
	void SetShareOpinionAccepted(PlayerTypes ePlayer, bool bValue);

	short GetShareOpinionCounter(PlayerTypes ePlayer) const;
	void SetShareOpinionCounter(PlayerTypes ePlayer, int iValue);
	void ChangeShareOpinionCounter(PlayerTypes ePlayer, int iChange);

	// Help Request
	void DoHelpRequestMade(PlayerTypes ePlayer);
	bool IsHelpRequestTooSoon(PlayerTypes ePlayer) const;
	short GetHelpRequestTooSoonNumTurns(PlayerTypes ePlayer) const;

	bool IsHelpRequestEverMade(PlayerTypes ePlayer) const;

	short GetHelpRequestCounter(PlayerTypes ePlayer) const;
	void SetHelpRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangeHelpRequestCounter(PlayerTypes ePlayer, int iChange);

	// Vassals
	void DoVassalTaxChanged(TeamTypes eTeam, bool bTaxesLowered);

	bool IsVassalTaxRaised(PlayerTypes ePlayer) const;
	void SetVassalTaxRaised(PlayerTypes ePlayer, bool bValue);

	VassalTreatmentTypes GetVassalTreatmentLevel(PlayerTypes ePlayer) const;
	CvString GetVassalTreatmentToolTip(PlayerTypes ePlayer) const;
	
	// How much gold has our vassal collected since we've known him?
	int GetVassalGoldPerTurnCollectedSinceVassalStarted(PlayerTypes ePlayer) const;
	void SetVassalGoldPerTurnCollectedSinceVassalStarted(PlayerTypes ePlayer, int iValue);
	void ChangeVassalGoldPerTurnCollectedSinceVassalStarted(PlayerTypes ePlayer, int iChange);
	
	// How much gold have we taxed from him since we've known him?
	int GetVassalGoldPerTurnTaxedSinceVassalStarted(PlayerTypes ePlayer) const;
	void SetVassalGoldPerTurnTaxedSinceVassalStarted(PlayerTypes ePlayer, int iValue);
	void ChangeVassalGoldPerTurnTaxedSinceVassalStarted(PlayerTypes ePlayer, int iChange);

	bool IsVassalTaxLowered(PlayerTypes ePlayer) const;
	void SetVassalTaxLowered(PlayerTypes ePlayer, bool bValue);

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

	int GetVassalProtectValue(PlayerTypes ePlayer) const;
	void ChangeVassalProtectValue(PlayerTypes ePlayer, int iChange);

	int GetTooManyVassalsScore(PlayerTypes ePlayer) const;

	int GetBrokenVassalAgreementScore(PlayerTypes ePlayer) const;
	void SetBrokenVassalAgreement(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenVassalAgreement(PlayerTypes ePlayer) const;
	
	int GetVassalFailedProtectScore(PlayerTypes ePlayer) const;
	int GetVassalFailedProtectValue(PlayerTypes ePlayer) const;
	void ChangeVassalFailedProtectValue(PlayerTypes ePlayer, int iChange);

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

	int GetTurnsSinceVassalagePeacefullyRevoked(PlayerTypes ePlayer) const;
	void SetTurnsSinceVassalagePeacefullyRevoked(PlayerTypes ePlayer, int iValue);
	void ChangeTurnsSinceVassalagePeacefullyRevoked(PlayerTypes ePlayer, int iChange);

	int GetTurnsSinceVassalageForcefullyRevoked(PlayerTypes ePlayer) const;
	void SetTurnsSinceVassalageForcefullyRevoked(PlayerTypes ePlayer, int iValue);
	void ChangeTurnsSinceVassalageForcefullyRevoked(PlayerTypes ePlayer, int iChange);

	bool IsHappyAboutPlayerVassalagePeacefullyRevoked(PlayerTypes ePlayer) const;
	bool IsAngryAboutPlayerVassalageForcefullyRevoked(PlayerTypes ePlayer) const;

	int GetNumTimesDemandedWhileVassal(PlayerTypes ePlayer) const;
	void SetNumTimesDemandedWhileVassal(PlayerTypes ePlayer, int iValue);
	void ChangeNumTimesDemandedWhileVassal(PlayerTypes ePlayer, int iChange);

	void DoWeMadeVassalageWithSomeone(TeamTypes eTeam, bool bVoluntary);
	void DoWeEndedVassalageWithSomeone(TeamTypes eTeam);

	//GlobalStateTypes GetGlobalState(PlayerTypes ePlayer) const;
	//void SetGlobalState(PlayerTypes ePlayer, GlobalStateTypes eGlobalState);

	//void DoUpdateGlobalStates();
	//void DoUpdateGlobalStateForOnePlayer(PlayerTypes ePlayer);
#endif

	// Working Against Player
	//bool DoTestWorkingAgainstPlayersDesire(PlayerTypes ePlayer, PlayerTypes &eChosenAgainstPlayer);

	//int GetWorkingAgainstPlayerAcceptableScore(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bAskedByPlayer);
	//bool IsWorkingAgainstPlayerMessageTooSoon(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;

	//bool IsWorkingAgainstPlayerEverAsked(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;

	//bool IsWorkingAgainstPlayerRejected(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;
	//bool IsWorkingAgainstPlayerAccepted(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;
	//void SetWorkingAgainstPlayerAccepted(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bValue);

	//short GetWorkingAgainstPlayerCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;
	//void SetWorkingAgainstPlayerCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iValue);
	//void ChangeWorkingAgainstPlayerCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iChange);

	//bool DoTestContinueWorkingAgainstPlayersDesire(PlayerTypes ePlayer, PlayerTypes &eAgainstPlayer);
	//bool IsContinueWorkingAgainstPlayer(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer);

	//bool IsWorkingAgainstPlayer(PlayerTypes ePlayer);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	int IsMoveTroopsRequestAcceptable(PlayerTypes ePlayer, bool bJustChecking = false);
	
	bool IsPlayerMoveTroopsRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerMoveTroopsRequestAccepted(PlayerTypes ePlayer, bool bValue);

	int GetPlayerMoveTroopsRequestCounter(PlayerTypes ePlayer) const;
	void SetPlayerMoveTroopsRequestCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerMoveTroopsRequestCounter(PlayerTypes ePlayer, int iChange);

	bool IsTooSoonForMoveTroopsRequest(PlayerTypes ePlayer) const;

	bool IsMasterLiberatedMeFromVassalage(PlayerTypes ePlayer) const;
	void SetMasterLiberatedMeFromVassalage(PlayerTypes ePlayer, bool bValue);
	void DoLiberatedFromVassalage(TeamTypes eTeam);
#endif

	/////////////////////////////////////////////////////////
	// A Player's adherence to statements made to this AI
	/////////////////////////////////////////////////////////

	void DoTestPromises();

	bool IsPlayerMadeMilitaryPromise(PlayerTypes ePlayer);
	void SetPlayerMadeMilitaryPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenMilitaryPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenMilitaryPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredMilitaryPromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredMilitaryPromise(PlayerTypes ePlayer, bool bValue);
	short GetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer);
	void SetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iValue);
	void ChangePlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetPlayerMadeExpansionPromise(PlayerTypes ePlayer);
#endif
	bool IsPlayerMadeExpansionPromise(PlayerTypes ePlayer, int iTestGameTurn = -1);
	void SetPlayerMadeExpansionPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenExpansionPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenExpansionPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredExpansionPromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredExpansionPromise(PlayerTypes ePlayer, bool bValue);
	AggressivePostureTypes GetPlayerExpansionPromiseData(PlayerTypes ePlayer);
	void SetPlayerExpansionPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue);
	bool EverMadeExpansionPromise(PlayerTypes ePlayer);
#if defined(MOD_BALANCE_CORE)
	void SetEverMadeExpansionPromise(PlayerTypes ePlayer, bool bValue);
#endif
#if defined(MOD_BALANCE_CORE)
	int GetPlayerMadeBorderPromise(PlayerTypes ePlayer);
#endif
	bool IsPlayerMadeBorderPromise(PlayerTypes ePlayer, int iTestGameTurn = -1);
	void SetPlayerMadeBorderPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenBorderPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenBorderPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredBorderPromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredBorderPromise(PlayerTypes ePlayer, bool bValue);
	AggressivePostureTypes GetPlayerBorderPromiseData(PlayerTypes ePlayer);
	void SetPlayerBorderPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue);
	bool EverMadeBorderPromise(PlayerTypes ePlayer);
#if defined(MOD_BALANCE_CORE)
	void SetEverMadeBorderPromise(PlayerTypes ePlayer, bool bValue);
	void SetDoFEverAsked(PlayerTypes ePlayer, bool bValue);
	void SetHelpRequestEverMade(PlayerTypes ePlayer, bool bValue);
	void SetNumDemandEverMade(PlayerTypes ePlayer, int iValue);
	void SetPlayerNoSettleRequestEverAsked(PlayerTypes ePlayer, bool bValue);
	void SetPlayerStopSpyingRequestEverAsked(PlayerTypes ePlayer, bool bValue);
#endif

	// Promise to not attack a City-State
	bool IsPlayerMadeAttackCityStatePromise(PlayerTypes ePlayer);
	void SetPlayerMadeAttackCityStatePromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenAttackCityStatePromise(PlayerTypes ePlayer);
	void SetPlayerBrokenAttackCityStatePromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer, bool bValue);

	// Promise to not bully a City-State
	bool IsPlayerMadeBullyCityStatePromise(PlayerTypes ePlayer);
	void SetPlayerMadeBullyCityStatePromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenBullyCityStatePromise(PlayerTypes ePlayer);
	void SetPlayerBrokenBullyCityStatePromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredBullyCityStatePromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredBullyCityStatePromise(PlayerTypes ePlayer, bool bValue);

	// Promise to not spy any more
	bool IsPlayerMadeSpyPromise(PlayerTypes ePlayer);
	void SetPlayerMadeSpyPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenSpyPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenSpyPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredSpyPromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredSpyPromise(PlayerTypes ePlayer, bool bValue);

	// Forgive for spying
	bool IsPlayerForgaveForSpying(PlayerTypes ePlayer);
	void SetPlayerForgaveForSpying(PlayerTypes ePlayer, bool bValue);

	// Promise to stop religious conversions
	bool IsPlayerMadeNoConvertPromise(PlayerTypes ePlayer);
	void SetPlayerMadeNoConvertPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenNoConvertPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenNoConvertPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredNoConvertPromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredNoConvertPromise(PlayerTypes ePlayer, bool bValue);

	// Player asks the AI not to convert
	bool IsPlayerAskedNotToConvert(PlayerTypes ePlayer);
	void SetPlayerAskedNotToConvert(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerAgreeNotToConvert(PlayerTypes ePlayer);
	void SetPlayerAgreeNotToConvert(PlayerTypes ePlayer, bool bValue);

	bool IsStopSpreadingReligionAcceptable(PlayerTypes ePlayer);

	// Promise to stop digging up our land for archaeological artifacts
	bool IsPlayerMadeNoDiggingPromise(PlayerTypes ePlayer);
	void SetPlayerMadeNoDiggingPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenNoDiggingPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenNoDiggingPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredNoDiggingPromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredNoDiggingPromise(PlayerTypes ePlayer, bool bValue);

	// Player asks the AI not to dig
	bool IsPlayerAskedNotToDig(PlayerTypes ePlayer);
	void SetPlayerAskedNotToDig(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerAgreeNotToDig(PlayerTypes ePlayer);
	void SetPlayerAgreeNotToDig(PlayerTypes ePlayer, bool bValue);
	bool IsStopDiggingAcceptable(PlayerTypes ePlayer);

	bool IsPlayerBrokenCoopWarPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenCoopWarPromise(PlayerTypes ePlayer, bool bValue);

	int GetNumCiviliansReturnedToMe(PlayerTypes ePlayer) const;
	void ChangeNumCiviliansReturnedToMe(PlayerTypes ePlayer, int iChange);

	int GetNumLandmarksBuiltForMe(PlayerTypes ePlayer) const;
	void ChangeNumLandmarksBuiltForMe(PlayerTypes ePlayer, int iChange);

#if defined(MOD_BALANCE_CORE)
	int GetLandmarksBuiltForMeTurn(PlayerTypes ePlayer) const;
	void SetLandmarksBuiltForMeTurn(PlayerTypes ePlayer, int iChange);

	int GetCiviliansReturnedToMeTurn(PlayerTypes ePlayer) const;
	void SetCiviliansReturnedToMeTurn(PlayerTypes ePlayer, int iChange);

	int GetLiberatedCitiesTurn(PlayerTypes ePlayer) const;
	void SetLiberatedCitiesTurn(PlayerTypes ePlayer, int iChange);
	
	int GetForgaveForSpyingTurn(PlayerTypes ePlayer) const;
	void SetForgaveForSpyingTurn(PlayerTypes ePlayer, int iChange);
	
	int GetIntrigueSharedTurn(PlayerTypes ePlayer) const;
	void SetIntrigueSharedTurn(PlayerTypes ePlayer, int iChange);
	
	int GetReligiousConversionTurn(PlayerTypes ePlayer) const;
	void SetReligiousConversionTurn(PlayerTypes ePlayer, int iChange);
	
	int GetRobbedTurn(PlayerTypes ePlayer) const;
	void SetRobbedTurn(PlayerTypes ePlayer, int iChange);
#endif

	bool WasResurrectedBy(PlayerTypes ePlayer) const;
	bool WasResurrectedThisTurnBy(PlayerTypes ePlayer) const;
	void SetResurrectedBy(PlayerTypes ePlayer, bool bValue);

	int GetNumTimesCultureBombed(PlayerTypes ePlayer) const;
	void ChangeNumTimesCultureBombed(PlayerTypes ePlayer, int iChange);

	int GetNegativeReligiousConversionPoints(PlayerTypes ePlayer) const;
	void ChangeNegativeReligiousConversionPoints(PlayerTypes ePlayer, int iChange);

	int GetNegativeArchaeologyPoints(PlayerTypes ePlayer) const;
	void ChangeNegativeArchaeologyPoints(PlayerTypes ePlayer, int iChange);
#if defined(MOD_BALANCE_CORE)
	int GetNumTimesRazed(PlayerTypes ePlayer) const;
	void ChangeNumTimesRazed(PlayerTypes ePlayer, int iChange);
#endif
	int GetNumTimesNuked(PlayerTypes ePlayer) const;
	void ChangeNumTimesNuked(PlayerTypes ePlayer, int iChange);

	int GetNumTimesRobbedBy(PlayerTypes ePlayer) const;
	void ChangeNumTimesRobbedBy(PlayerTypes ePlayer, int iChange);

	int GetNumTimesIntrigueSharedBy(PlayerTypes ePlayer) const;
	void ChangeNumTimesIntrigueSharedBy(PlayerTypes ePlayer, int iChange);

	/////////////////////////////////////////////////////////
	// Opinion modifiers
	/////////////////////////////////////////////////////////
	int GetLandDisputeLevelScore(PlayerTypes ePlayer);
	int GetWonderDisputeLevelScore(PlayerTypes ePlayer);
	int GetMinorCivDisputeLevelScore(PlayerTypes ePlayer);
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	int GetVictoryDisputeLevelScore(PlayerTypes ePlayer);
	int GetVictoryBlockLevelScore(PlayerTypes ePlayer);
	int GetHasReligionFounderDifferenceScore(PlayerTypes ePlayer);
#endif
	int GetWarmongerThreatScore(PlayerTypes ePlayer);
	int GetCiviliansReturnedToMeScore(PlayerTypes ePlayer);
	int GetLandmarksBuiltForMeScore(PlayerTypes ePlayer);
	int GetResurrectedScore(PlayerTypes ePlayer);
	int GetLiberatedCitiesScore(PlayerTypes ePlayer);
	int GetEmbassyScore(PlayerTypes ePlayer);
	int GetForgaveForSpyingScore(PlayerTypes ePlayer);
	int GetNoSetterRequestScore(PlayerTypes ePlayer);
	int GetStopSpyingRequestScore(PlayerTypes ePlayer);
	int GetDemandEverMadeScore(PlayerTypes ePlayer);
	int GetTimesCultureBombedScore(PlayerTypes ePlayer);
	int GetReligiousConversionPointsScore(PlayerTypes ePlayer);
	int GetHasAdoptedHisReligionScore(PlayerTypes ePlayer);
	int GetHasAdoptedMyReligionScore(PlayerTypes ePlayer);
	int GetSameLatePoliciesScore(PlayerTypes ePlayer);
	int GetDifferentLatePoliciesScore(PlayerTypes ePlayer);
	int GetTimesRobbedScore(PlayerTypes ePlayer);
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
#if defined(MOD_BALANCE_CORE_DEALS)
	int GetDPAcceptedScore(PlayerTypes ePlayer);
	int GetDPWithAnyFriendScore(PlayerTypes ePlayer);
	int GetDPWithAnyEnemyScore(PlayerTypes ePlayer);
	int GetOpenBordersScore(PlayerTypes ePlayer);
	MajorCivOpinionTypes GetNeighborOpinion(PlayerTypes ePlayer) const;
	bool MusteringForNeighborAttack(PlayerTypes ePlayer) const;
#endif
	int GetFriendDenouncementScore(PlayerTypes ePlayer);
	int GetWeDenouncedFriendScore(PlayerTypes ePlayer);
	int GetFriendDenouncedUsScore(PlayerTypes ePlayer);
	int GetWeDeclaredWarOnFriendScore(PlayerTypes ePlayer);
	int GetFriendDeclaredWarOnUsScore(PlayerTypes ePlayer);
	int GetDenouncedUsScore(PlayerTypes ePlayer);
	int GetDenouncedThemScore(PlayerTypes ePlayer);
	int GetDenouncedFriendScore(PlayerTypes ePlayer);
	int GetDenouncedEnemyScore(PlayerTypes ePlayer);
	int GetRecklessExpanderScore(PlayerTypes ePlayer);
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
	int GetGaveAssistanceToScore(PlayerTypes ePlayer);
	int GetPaidTributeToScore(PlayerTypes ePlayer);
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

	int GetNumOurEnemiesPlayerAtWarWith(PlayerTypes ePlayer);

	bool IsGoingForWorldConquest();
	bool IsGoingForDiploVictory();
	bool IsGoingForCultureVictory();
	bool IsGoingForSpaceshipVictory();

#if defined(MOD_BALANCE_CORE)
	bool IsCloseToSSVictory();
	bool IsCloseToDominationVictory();
	bool IsCloseToCultureVictory();
	bool IsCloseToDiploVictory();
#endif

	bool IsPlayerValid(PlayerTypes eOtherPlayer, bool bMyTeamIsValid = false);

	// Messages sent to other players about protected Minor Civs
	bool HasSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor);
	void SetSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue);
	void ResetSentAttackProtectedMinorTaunts(PlayerTypes eMinor);

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
	// Minor Civ Log
	void LogMinorCivGiftTile(PlayerTypes ePlayer);
	void LogMinorCivGiftGold(PlayerTypes ePlayer, int iOldFriendship, int iGold, bool bSaving, bool bWantQuickBoost, PlayerTypes ePlayerTryingToPass);
	void LogMinorCivBullyGold(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, int iGold, bool bSuccess, int iBullyMetricScore);
	void LogMinorCivBullyUnit(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, UnitTypes eUnit, bool bSuccess, int iBullyMetricScore);
	void LogMinorCivQuestReceived(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType);
	void LogMinorCivQuestFinished(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType);
	void LogMinorCivQuestCancelled(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType);
	void LogMinorCivBuyout(PlayerTypes eMinor, int iGoldPaid, bool bSaving);

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

	CvDeal* GetDealToRenew(int* piDealType = NULL, PlayerTypes eOtherPlayer = NO_PLAYER);
	void ClearDealToRenew();

	void KilledPlayerCleanup (PlayerTypes eKilledPlayer);

	// Methods for injecting tests
	void TestUIDiploStatement(PlayerTypes eToPlayer, DiploStatementTypes eStatement, int iArg1);

	void LogOpenEmbassy(PlayerTypes ePlayer);
	void LogCloseEmbassy(PlayerTypes ePlayer);

private:
	bool IsValidUIDiplomacyTarget(PlayerTypes eTargetPlayer);

	bool IsAtWar(PlayerTypes eOtherPlayer);
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

	bool IsVassal(PlayerTypes eOtherPlayer) const;
	int GetNumVassals(PlayerTypes eOtherPlayer) const;
	
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
		char m_aiMajorCivOpinion[MAX_MAJOR_CIVS];
		char m_aeMajorCivApproach[MAX_MAJOR_CIVS];
		char m_aeApproachScratchPad[REALLY_MAX_PLAYERS];
		char m_aeMinorCivApproach[REALLY_MAX_PLAYERS-MAX_MAJOR_CIVS];
		char m_aeApproachTowardsUsGuess[MAX_MAJOR_CIVS];
		char m_aeApproachTowardsUsGuessCounter[MAX_MAJOR_CIVS];
		short m_aeWantPeaceCounter[REALLY_MAX_PLAYERS];
		char m_aePeaceTreatyWillingToOffer[MAX_MAJOR_CIVS];
		char m_aePeaceTreatyWillingToAccept[MAX_MAJOR_CIVS];
		short m_aiNumWondersBeatenTo[REALLY_MAX_PLAYERS];
		bool m_abMusteringForAttack[REALLY_MAX_PLAYERS];
		bool m_abWantsResearchAgreementWithPlayer[MAX_MAJOR_CIVS];
#if defined(MOD_BALANCE_CORE_DEALS)
		bool m_abWantsDefensivePactWithPlayer[MAX_MAJOR_CIVS];
		bool m_abWantsSneakAttack[MAX_MAJOR_CIVS];
#endif
		bool m_abWantToRouteToMinor[REALLY_MAX_PLAYERS-MAX_MAJOR_CIVS];
		char m_aeWarFace[REALLY_MAX_PLAYERS];
		char m_aeWarState[REALLY_MAX_PLAYERS];
		char m_aeWarProjection[REALLY_MAX_PLAYERS];
		char m_aeLastWarProjection[REALLY_MAX_PLAYERS];
		char m_aeWarGoal[REALLY_MAX_PLAYERS];
		short m_aiPlayerNumTurnsAtWar[REALLY_MAX_PLAYERS];
#if defined(MOD_BALANCE_CORE)
		short m_aiPlayerNumTurnsAtPeace[REALLY_MAX_PLAYERS];
		short m_aiPlayerNumTurnsSinceCityCapture[REALLY_MAX_PLAYERS];
#endif
		short m_aiNumWarsFought[REALLY_MAX_PLAYERS];
		char m_aePlayerMilitaryStrengthComparedToUs[REALLY_MAX_PLAYERS];
		char m_aePlayerEconomicStrengthComparedToUs[REALLY_MAX_PLAYERS];
		char m_aePlayerTargetValue[REALLY_MAX_PLAYERS];
		char m_aePlayerLandDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aePlayerLastTurnLandDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aePlayerVictoryDisputeLevel[REALLY_MAX_PLAYERS];
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
		char m_aePlayerVictoryBlockLevel[MAX_MAJOR_CIVS];
		bool m_abCantMatchDeal[MAX_MAJOR_CIVS];
#endif
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
#if defined(MOD_BALANCE_CORE)
		short m_aiPlayerBackstabCounter[MAX_MAJOR_CIVS];

		short m_aiDefensivePactValue[MAX_MAJOR_CIVS];
		short m_aiDoFValue[MAX_MAJOR_CIVS];
		short m_aiCompetitorValue[MAX_MAJOR_CIVS];
#endif

		short m_aiDemandCounter[MAX_MAJOR_CIVS];
		short m_aiDemandTooSoonNumTurns[MAX_MAJOR_CIVS];

		bool m_abDoFAccepted[MAX_MAJOR_CIVS];
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
		bool m_abDoFBroken[MAX_MAJOR_CIVS];
		char m_aeDoFType[MAX_MAJOR_CIVS];
		short m_aiNumTimesCoopWarDenied[MAX_MAJOR_CIVS];
		short m_aiPromisePlot[MAX_MAJOR_CIVS];
		short m_aiPromiseNumberOwnedCities[MAX_MAJOR_CIVS];
		short m_aiLastTurnCoM[MAX_MAJOR_CIVS];
#endif
		short m_aiDoFCounter[MAX_MAJOR_CIVS];

		bool m_abDenouncedPlayer[MAX_MAJOR_CIVS];
		bool m_abFriendDenouncedUs[MAX_MAJOR_CIVS];
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		bool m_abOfferingGift[MAX_MAJOR_CIVS];
		bool m_abOfferedGift[MAX_MAJOR_CIVS];
#endif
		bool m_abFriendDeclaredWarOnUs[MAX_MAJOR_CIVS];
		short m_aiDenouncedPlayerCounter[MAX_MAJOR_CIVS];

		short m_aiNumRequestsRefused[MAX_MAJOR_CIVS];

		short m_aiNumCiviliansReturnedToMe[MAX_MAJOR_CIVS];
		short m_aiNumLandmarksBuiltForMe[MAX_MAJOR_CIVS];
		short m_aiResurrectedOnTurn[MAX_MAJOR_CIVS];
		short m_aiNumTimesCultureBombed[MAX_MAJOR_CIVS];

		short m_paiNegativeReligiousConversionPoints[MAX_MAJOR_CIVS];

		short m_paiNegativeArchaeologyPoints[MAX_MAJOR_CIVS];
#if defined(MOD_BALANCE_CORE)
		short m_aiNumTimesRazed[REALLY_MAX_PLAYERS];
		bool m_abPlayerEverMadeBorderPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerEverMadeExpansionPromise[MAX_MAJOR_CIVS];
		bool m_abDoFEverAsked[MAX_MAJOR_CIVS];
		bool m_abHelpRequestEverMade[MAX_MAJOR_CIVS];
		int m_aiDemandEverMade[MAX_MAJOR_CIVS];
		bool m_abPlayerNoSettleRequestEverAsked[MAX_MAJOR_CIVS];
		bool m_abPlayerStopSpyingRequestEverAsked[MAX_MAJOR_CIVS];
		short m_aiNumLandmarksBuiltForMeTurn[MAX_MAJOR_CIVS];
		short m_aiCiviliansReturnedToMeTurn[MAX_MAJOR_CIVS];
		short m_aiWarDamageValue[MAX_MAJOR_CIVS];
		short m_aiPlayerForgaveForSpyingTurn[MAX_MAJOR_CIVS];
		short m_aiLiberatedCitiesTurn[MAX_MAJOR_CIVS];
		short m_aiIntrigueSharedTurn[MAX_MAJOR_CIVS];
		short m_aiReligiousConversionTurn[MAX_MAJOR_CIVS];
		short m_aiTimesRobbedTurn[MAX_MAJOR_CIVS];
#endif
		short m_aiNumTimesNuked[MAX_MAJOR_CIVS];
		short m_aiNumTimesRobbedBy[MAX_MAJOR_CIVS];
		short m_aiNumTimesIntrigueSharedBy[MAX_MAJOR_CIVS];

		short m_aiBrokenExpansionPromiseValue[MAX_MAJOR_CIVS];
		short m_aiIgnoredExpansionPromiseValue[MAX_MAJOR_CIVS];
		short m_aiBrokenBorderPromiseValue[MAX_MAJOR_CIVS];
		short m_aiIgnoredBorderPromiseValue[MAX_MAJOR_CIVS];

		short m_aiDeclaredWarOnFriendValue[MAX_MAJOR_CIVS];
		short m_aiNumCitiesLiberated[MAX_MAJOR_CIVS];

		short m_aiTradeValue[MAX_MAJOR_CIVS];
		short m_aiCommonFoeValue[MAX_MAJOR_CIVS];
		short m_aiAssistValue[MAX_MAJOR_CIVS];

		// Player's response to AI statements

		bool m_abPlayerMadeMilitaryPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenMilitaryPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredMilitaryPromise[MAX_MAJOR_CIVS];
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

#if defined(MOD_API_EXTENSIONS)
		char m_aiTheyPlottedAgainstUs[MAX_MAJOR_CIVS];
		char m_aiTheyLoweredOurInfluence[MAX_MAJOR_CIVS];
		int m_aiOtherPlayerWarmongerAmountTimes100[MAX_MAJOR_CIVS];
#else
		int m_aiOtherPlayerWarmongerAmount[MAX_MAJOR_CIVS];
#endif

		short m_aiOtherPlayerTurnsSinceWeLikedTheirProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceWeDislikedTheirProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceTheySupportedOurProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceTheyFoiledOurProposal[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceTheySupportedOurHosting[MAX_MAJOR_CIVS];

		//2D Arrays
		char* m_apaeOtherPlayerMajorCivOpinion[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerLandDisputeLevel[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerVictoryDisputeLevel[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerWonderDisputeLevel[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerMinorCivDisputeLevel[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerWarDamageLevel[REALLY_MAX_PLAYERS];
		int* m_apaiOtherPlayerWarValueLost[REALLY_MAX_PLAYERS];
		int* m_apaiOtherPlayerLastRoundWarValueLost[REALLY_MAX_PLAYERS];
		bool* m_apabSentAttackMessageToMinorCivProtector[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerMilitaryThreat[REALLY_MAX_PLAYERS];
		DiploLogData* m_apaDiploStatementsLog[MAX_MAJOR_CIVS];

		bool* m_apabWorkingAgainstPlayerAccepted[MAX_MAJOR_CIVS];
		short* m_apaiWorkingAgainstPlayerCounter[MAX_MAJOR_CIVS];

		char* m_apacCoopWarAcceptedState[MAX_MAJOR_CIVS];
		short* m_apaiCoopWarCounter[MAX_MAJOR_CIVS];

		char m_aaeOtherPlayerMajorCivOpinion[MAX_MAJOR_CIVS* MAX_MAJOR_CIVS];
		char m_aaeOtherPlayerLandDisputeLevel[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		char m_aaeOtherPlayerVictoryDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aaeOtherPlayerWarDamageLevel[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		int m_aaiOtherPlayerWarValueLost[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		int m_aaiOtherPlayerLastRoundWarValueLost[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		bool m_aabSentAttackMessageToMinorCivProtector[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		char m_aaeOtherPlayerMilitaryThreat[REALLY_MAX_PLAYERS* REALLY_MAX_PLAYERS];
		DiploLogData m_aaDiploStatementsLog[MAX_MAJOR_CIVS* MAX_DIPLO_LOG_STATEMENTS];

		bool m_aabWorkingAgainstPlayerAccepted[MAX_MAJOR_CIVS* MAX_MAJOR_CIVS];
		short m_aaiWorkingAgainstPlayerCounter[MAX_MAJOR_CIVS* MAX_MAJOR_CIVS];

		char m_aacCoopWarAcceptedState[MAX_MAJOR_CIVS* MAX_MAJOR_CIVS];
		short m_aaiCoopWarCounter[MAX_MAJOR_CIVS* MAX_MAJOR_CIVS];
#if defined(MOD_ACTIVE_DIPLOMACY)
		float m_aTradePriority[MAX_MAJOR_CIVS]; // current ai to human trade priority
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
		bool m_abShareOpinionAccepted[MAX_MAJOR_CIVS];
		short m_aiShareOpinionCounter[MAX_MAJOR_CIVS];

		short m_aiHelpRequestCounter[MAX_MAJOR_CIVS];
		short m_aiHelpRequestTooSoonNumTurns[MAX_MAJOR_CIVS];

		short m_aiNumTimesDemandedWhenVassal[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenVassalAgreement[MAX_MAJOR_CIVS];

		short m_aiPlayerVassalageFailedProtectValue[MAX_MAJOR_CIVS];
		short m_aiPlayerVassalageProtectValue[MAX_MAJOR_CIVS];
		bool m_abPlayerVassalagePeacefullyRevokedVassal[MAX_MAJOR_CIVS];
		bool m_abPlayerVassalageForcefullyRevokedVassal[MAX_MAJOR_CIVS];
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

	bool* m_pabPlayerVassalagePeacefullyRevokedVassal;
	bool* m_pabPlayerVassalageForcefullyRevokedVassal;

	short* m_paiPlayerVassalageTurnsSincePeacefullyRevokedVassalage;
	short* m_paiPlayerVassalageTurnsSinceForcefullyRevokedVassalage;

	short* m_paiNumTimesDemandedWhenVassal;
	bool* m_pabPlayerBrokenVassalAgreement;

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
	char** m_ppaaeOtherPlayerMajorCivOpinion;

	char* m_paeMajorCivApproach;
	char* m_paeApproachScratchPad;
	char* m_paeMinorCivApproach;
	char* m_paeApproachTowardsUsGuess;
	char* m_paeApproachTowardsUsGuessCounter;

	char m_eDemandTargetPlayer;
	bool m_bDemandReady;

	short* m_paeWantPeaceCounter;
	char* m_paePeaceTreatyWillingToOffer;
	char* m_paePeaceTreatyWillingToAccept;

	short* m_paiNumWondersBeatenTo;
	bool* m_pabMusteringForAttack;

	bool* m_pabWantsResearchAgreementWithPlayer;
#if defined(MOD_BALANCE_CORE_DEALS)
	bool* m_pabWantsDefensivePactWithPlayer;
	bool* m_pabWantsSneakAttack;

	short* m_paiDefensivePactValue;
	short* m_paiDoFValue;
	short* m_paiCompetitorValue;
#endif
	bool* m_pabWantToRouteToMinor;

	char* m_paeWarFace;
	char* m_paeWarState;
	char* m_paeWarProjection;
	char* m_paeLastWarProjection;
	char* m_paeWarGoal;
	short* m_paiPlayerNumTurnsAtWar;
#if defined(MOD_BALANCE_CORE)
	short* m_paiPlayerNumTurnsAtPeace;
	short* m_paiPlayerNumTurnsSinceCityCapture;
	short* m_paiWarDamageValue;
#endif
	short* m_paiNumWarsFought;

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
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	char* m_paePlayerVictoryBlockLevel;
	bool* m_pabCantMatchDeal;
#endif
	char* m_paePlayerWonderDisputeLevel;
	char* m_paePlayerMinorCivDisputeLevel;
	char** m_ppaaeOtherPlayerLandDisputeLevel;
	char** m_ppaaeOtherPlayerVictoryDisputeLevel;

	char* m_paeMilitaryThreat;
	char** m_ppaaeOtherPlayerMilitaryThreat;

	char* m_paeWarmongerThreat;

	// Things a player has told the AI

	bool* m_pabPlayerNoSettleRequestAccepted;
	short* m_paiPlayerNoSettleRequestCounter;

	bool* m_pabPlayerStopSpyingRequestAccepted;
	short* m_paiPlayerStopSpyingRequestCounter;

#if defined(MOD_BALANCE_CORE)
	short* m_paiPlayerBackstabCounter;
#endif

	short* m_paiDemandCounter;
	short* m_paiDemandTooSoonNumTurns;

	bool* m_pabDoFAccepted;
#if defined(MOD_BALANCE_CORE_DIPLOMACY)
	bool* m_pabDoFBroken;
	char* m_paeDoFType;
	short* m_paiNumTimesCoopWarDenied;
	short* m_paiPromisePlot;
	short* m_paiPromiseNumberOwnedCities;
	short* m_paiLastTurnCoM;
#endif
	short* m_paiDoFCounter;

	bool* m_pabDenouncedPlayer;
	bool* m_pabFriendDenouncedUs;
	bool* m_pabFriendDeclaredWarOnUs;
	short* m_paiDenouncedPlayerCounter;
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	bool* m_pabOfferingGift;
	bool* m_pabOfferedGift;
#endif
	short* m_paiNumRequestsRefused;

	short* m_paiNumCiviliansReturnedToMe;
	short* m_paiNumLandmarksBuiltForMe;
	short* m_paiResurrectedOnTurn; // slewis - the "resurrected by" player liberated the city of an otherwise dead player and brought them back into the game on the given turn
	short* m_paiNumTimesCultureBombed;
	short* m_paiNegativeReligiousConversionPoints;
	short* m_paiNegativeArchaeologyPoints;

#if defined(MOD_BALANCE_CORE)
	short* m_paiNumTimesRazed;
	short* m_paiNumLandmarksBuiltForMeTurn;
	short* m_paiCiviliansReturnedToMeTurn;
	short* m_paiPlayerForgaveForSpyingTurn;
	short* m_paiLiberatedCitiesTurn;
	short* m_paiIntrigueSharedTurn;
	short* m_paiReligiousConversionTurn;
	short* m_paiTimesRobbedTurn;
#endif
	short* m_paiNumTimesNuked;
	short* m_paiNumTimesRobbedBy;
	short* m_paiNumTimesIntrigueSharedBy;

	short* m_paiBrokenExpansionPromiseValue;
	short* m_paiIgnoredExpansionPromiseValue;
	short* m_paiBrokenBorderPromiseValue;
	short* m_paiIgnoredBorderPromiseValue;
	short* m_paiDeclaredWarOnFriendValue;
	short* m_paiTradeValue;
	short* m_paiCommonFoeValue;
	short* m_paiAssistValue;

	short* m_paiNumCitiesLiberated;

	bool** m_ppaabWorkingAgainstPlayerAccepted;
	short** m_ppaaiWorkingAgainstPlayerCounter;

	char** m_ppaacCoopWarAcceptedState;
	short** m_ppaaiCoopWarCounter;

	// Player's repsonse to AI statements

	bool* m_pabPlayerMadeMilitaryPromise;
	bool* m_pabPlayerBrokenMilitaryPromise;
	bool* m_pabPlayerIgnoredMilitaryPromise;
	short* m_paiPlayerMilitaryPromiseCounter;

	short* m_paiPlayerMadeExpansionPromiseTurn;
	bool* m_pabPlayerBrokenExpansionPromise;
	bool* m_pabPlayerIgnoredExpansionPromise;
	char* m_paePlayerExpansionPromiseData;

	short* m_paiPlayerMadeBorderPromiseTurn;
	bool* m_pabPlayerBrokenBorderPromise;
	bool* m_pabPlayerIgnoredBorderPromise;
	char* m_paePlayerBorderPromiseData;
#if defined(MOD_BALANCE_CORE)
	bool* m_pabPlayerEverMadeBorderPromise;
	bool* m_pabPlayerEverMadeExpansionPromise;
	bool* m_pabDoFEverAsked;
	bool* m_pabHelpRequestEverMade;
	int*  m_paiDemandEverMade;
	bool* m_pabPlayerNoSettleRequestEverAsked;
	bool* m_pabPlayerStopSpyingRequestEverAsked;
#endif

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

	int m_iVictoryCompetitiveness;
	int m_iWonderCompetitiveness;
	int m_iMinorCivCompetitiveness;
	int m_iBoldness;
	int m_iDiploBalance;
	int m_iWarmongerHate;
	int m_iDenounceWillingness;
	int m_iDoFWillingness;
	int m_iLoyalty;
	int m_iNeediness;
	int m_iForgiveness;
	int m_iChattiness;
	int m_iMeanness;

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
#if defined(MOD_API_EXTENSIONS)
	char* m_paiTheyPlottedAgainstUs;
	char* m_paiTheyLoweredOurInfluence;
	int*  m_paiOtherPlayerWarmongerAmountTimes100;
#else
	int*  m_paiOtherPlayerWarmongerAmount;
#endif

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
#if defined(MOD_CONFIG_AI_IN_XML)
	int GetWarmongerOffset(bool bIsCapital, CvCity* pCity = NULL, PlayerTypes eWarmonger = NO_PLAYER);
	CvString GetWarmongerPreviewString(PlayerTypes eCurrentOwner = NO_PLAYER, bool bIsCapital = false, CvCity* pCity = NULL, PlayerTypes eActivePlayer = NO_PLAYER);
	CvString GetLiberationPreviewString(PlayerTypes eOriginalOwner = NO_PLAYER, bool bIsCapital = false, CvCity* pCity = NULL, PlayerTypes eActivePlayer = NO_PLAYER);
	void ApplyWarmongerPenalties(PlayerTypes eConqueror, PlayerTypes eConquered, bool bIsCapital, CvCity* pCity);
	int GetPlayerCaresValue(PlayerTypes eConqueror, PlayerTypes eConquered, bool bIsCapital, CvCity* pCity, PlayerTypes eCaringPlayer, bool bLiberation = false);
#else
	CvString GetWarmongerPreviewString(PlayerTypes eCurrentOwner);
	CvString GetLiberationPreviewString(PlayerTypes eOriginalOwner);
	void ApplyWarmongerPenalties(PlayerTypes eConqueror, PlayerTypes eConquered);
#endif
}

#endif //CIV5_AI_DIPLOMACY_H