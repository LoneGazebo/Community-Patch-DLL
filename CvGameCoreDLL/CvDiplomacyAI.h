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

struct DiploLogData
{
	DiploStatementTypes m_eDiploLogStatement;
	int m_iTurn;
};
FDataStream & operator<<(FDataStream &, const DiploLogData &);
FDataStream & operator>>(FDataStream &, DiploLogData &);

struct DeclarationLogData
{
	PublicDeclarationTypes m_eDeclaration;
	int m_iData1;
	int m_iData2;
	PlayerTypes m_eMustHaveMetPlayer;
	bool m_bActive;
	int m_iTurn;
};
FDataStream & operator<<(FDataStream &, const DeclarationLogData &);
FDataStream & operator>>(FDataStream &, DeclarationLogData &);

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
	enum DiplomacyPlayerType
	{
		DIPLO_FIRST_PLAYER		=  0,
		DIPLO_ALL_PLAYERS		= -1,
		DIPLO_AI_PLAYERS		= -2,
		DIPLO_HUMAN_PLAYERS		= -3
	};

	CvDiplomacyAI(void);
	~CvDiplomacyAI(void);
	void Init(CvPlayer *pPlayer);
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

	void DoTurn(PlayerTypes eTargetPlayer);
	void DoCounters();

	/////////////////////////////////////////////////////////
	// Opinion
	/////////////////////////////////////////////////////////

	void DoUpdateOpinions();
	void DoUpdateOnePlayerOpinion(PlayerTypes ePlayer);

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
	MajorCivApproachTypes GetBestApproachTowardsMajorCiv(PlayerTypes ePlayer, int &iHighestWeight, bool bLookAtOtherPlayers, bool bLog, WarFaceTypes &eWarFace);

	MajorCivApproachTypes GetMajorCivApproach(PlayerTypes ePlayer, bool bHideTrueFeelings) const;
	void SetMajorCivApproach(PlayerTypes ePlayer, MajorCivApproachTypes eApproach);
	int GetNumMajorCivApproach(MajorCivApproachTypes eApproach) const;

	// Minor Civs
	void DoUpdateMinorCivApproaches();
	void DoMinorCivApproachChange(PlayerTypes eMinor, MinorCivApproachTypes eApproach, MinorCivApproachTypes eLastTurnApproach);
	MinorCivApproachTypes GetBestApproachTowardsMinorCiv(PlayerTypes ePlayer, int &iHighestWeight, bool bLookAtOtherPlayers, bool bLog);

	MinorCivApproachTypes GetMinorCivApproach(PlayerTypes ePlayer) const;
	void SetMinorCivApproach(PlayerTypes ePlayer, MinorCivApproachTypes eApproach);
	int GetNumMinorCivApproach(MinorCivApproachTypes eApproach) const;

	void SetWantToRouteConnectToMinor (PlayerTypes eMinor, bool bWant);
	bool IsWantToRouteConnectToMinor (PlayerTypes eMinor);

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

	bool IsMakeRequest(PlayerTypes ePlayer, CvDeal* pDeal, bool &bRandPassed);
	bool IsLuxuryRequest(PlayerTypes ePlayer, CvDeal* pDeal, int &iWeightBias);
	bool IsGoldRequest(PlayerTypes ePlayer, CvDeal* pDeal, int &iWeightBias);

	bool IsWantsOpenBordersWithPlayer(PlayerTypes ePlayer);
	bool IsOpenBordersExchangeAcceptable(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Peace
	/////////////////////////////////////////////////////////

	void DoMakePeaceWithMinors();

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
	void DoUpdateWarProjections();
	int GetWarScore(PlayerTypes ePlayer);

	// War Goal: What is is our objective in the war against ePlayer (NO_WAR_GOAL_TYPE if at peace)
	WarGoalTypes GetWarGoal(PlayerTypes ePlayer) const;
	void SetWarGoal(PlayerTypes ePlayer, WarGoalTypes eWarGoal);
	void DoUpdateWarGoals();

	// Num Turns At War
	int GetPlayerNumTurnsAtWar(PlayerTypes ePlayer) const;
	void SetPlayerNumTurnsAtWar(PlayerTypes ePlayer, int iValue);
	void ChangePlayerNumTurnsAtWar(PlayerTypes ePlayer, int iChange);

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

	int GetRecentTradeValue(PlayerTypes ePlayer);
	void ChangeRecentTradeValue(PlayerTypes ePlayer, int iChange);
	int GetCommonFoeValue(PlayerTypes ePlayer);
	void ChangeCommonFoeValue(PlayerTypes ePlayer, int iChange);
	int GetRecentAssistValue(PlayerTypes ePlayer);
	void ChangeRecentAssistValue(PlayerTypes ePlayer, int iChange);
	bool IsLiberatedCapital(PlayerTypes ePlayer);
	bool IsLiberatedCity(PlayerTypes ePlayer);
	bool IsGaveAssistanceTo(PlayerTypes ePlayer);
	bool IsHasPaidTributeTo(PlayerTypes ePlayer);
	bool IsNukedBy(PlayerTypes ePlayer);
	bool IsCapitalCapturedBy(PlayerTypes ePlayer);

	// Expansion Aggressive Posture: How aggressively has ePlayer positioned their Units in relation to us?
	AggressivePostureTypes GetExpansionAggressivePosture(PlayerTypes ePlayer) const;
	void SetExpansionAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);
	void DoUpdateExpansionAggressivePostures();
	void DoUpdateOnePlayerExpansionAggressivePosture(PlayerTypes ePlayer);

	// Plot Buying Aggressive Posture: How aggressively is ePlayer buying land near us?
	AggressivePostureTypes GetPlotBuyingAggressivePosture(PlayerTypes ePlayer) const;
	void SetPlotBuyingAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);
	void DoUpdatePlotBuyingAggressivePosture();

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
	void DoPlayerDeclaredWarOnSomeone(PlayerTypes ePlayer, TeamTypes eOtherTeam);
	void DoPlayerKilledSomeone(PlayerTypes ePlayer, PlayerTypes eDeadPlayer);

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

	void DoCoopWarTimeStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	void DoCoopWarStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);

	void DoMakeDemand(PlayerTypes ePlayer, DiploStatementTypes &eStatement, CvDeal* pDeal);

	void DoAggressiveMilitaryStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoKilledCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	void DoAttackedCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	//void DoSeriousExpansionWarningStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoExpansionWarningStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoExpansionBrokenPromiseStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	//void DoSeriousPlotBuyingWarningStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoPlotBuyingWarningStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoPlotBuyingBrokenPromiseStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);

	void DoWeAttackedYourMinorStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);

	void DoDoFStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoDenounceFriendStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoRequestFriendDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	//void DoWorkAgainstSomeoneStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	//void DoEndWorkAgainstSomeoneStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);

	void DoLuxuryTrade(PlayerTypes ePlayer, DiploStatementTypes &eStatement, CvDeal* pDeal);
	void DoOpenBordersExchange(PlayerTypes ePlayer, DiploStatementTypes &eStatement, CvDeal* pDeal);
	void DoOpenBordersOffer(PlayerTypes ePlayer, DiploStatementTypes &eStatement, CvDeal* pDeal);
	void DoResearchAgreementOffer(PlayerTypes ePlayer, DiploStatementTypes &eStatement, CvDeal* pDeal);
	//void DoResearchAgreementPlan(PlayerTypes ePlayer, DiploStatementTypes &eStatement);

	void DoRequest(PlayerTypes ePlayer, DiploStatementTypes &eStatement, CvDeal* pDeal);
	void DoGift(PlayerTypes ePlayer, DiploStatementTypes &eStatement, CvDeal* pDeal);

	//void DoNowUnforgivableStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	//void DoNowEnemyStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);

	void DoHostileStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	//void DoFriendlyStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoAfraidStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoWarmongerStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement);
	void DoMinorCivCompetitionStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1, bool bIgnoreTurnsBetweenLimit = false);

	void DoAngryBefriendedEnemy(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	void DoAngryDenouncedFriend(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	void DoHappyDenouncedEnemy(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	void DoHappyBefriendedFriend(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	void DoFYIBefriendedHumanEnemy(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	void DoFYIDenouncedHumanFriend(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	void DoFYIDenouncedHumanEnemy(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);
	void DoFYIBefriendedHumanFriend(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1);

	void DoPeaceOffer(PlayerTypes ePlayer, DiploStatementTypes &eStatement, CvDeal* pDeal);

	/////////////////////////////////////////////////////////
	// Diplo stuff relating to UI
	/////////////////////////////////////////////////////////

	void DoBeginDiploWithHuman();

	const char* GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer = NO_PLAYER, const char* szOptionalKey1 = "");
	void DoFromUIDiploEvent(PlayerTypes eFromPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2);

	bool IsActHostileTowardsHuman(PlayerTypes eHuman);

	const char* GetGreetHumanMessage(LeaderheadAnimationTypes &eAnimation);
	const char* GetInsultHumanMessage();
	const char* GetAttackedByHumanMessage();
	const char* GetWarMessage(PlayerTypes ePlayer);
	const char* GetEndDoFMessage(PlayerTypes ePlayer);
	const char* GetEndWorkAgainstSomeoneMessage(PlayerTypes ePlayer, const char* strAgainstPlayerKey);

	const char* GetDiploTextFromTag(const char* strTag, const char* szOptionalKey1 = "", const char* szOptionalKey2 = "");

	/////////////////////////////////////////////////////////
	// Things a player has told this AI
	/////////////////////////////////////////////////////////

	// Coop War
	bool DoTestCoopWarDesire(PlayerTypes ePlayer, PlayerTypes &eChosenTargetPlayer);

	CoopWarStates GetWillingToAgreeToCoopWarState(PlayerTypes ePlayer, PlayerTypes eTargetPlayer);
	int GetCoopWarScore(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, bool bAskedByPlayer);

	bool IsCoopWarMessageTooSoon(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;

	bool IsCoopWarEverAsked(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;

	CoopWarStates GetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;
	void SetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, CoopWarStates eValue);

	short GetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const;
	void SetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iValue);
	void ChangeCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iChange);

	bool DoTestContinueCoopWarsDesire(PlayerTypes ePlayer, PlayerTypes &eAgainstPlayer);
	bool IsContinueCoopWar(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer);

	CoopWarStates GetGlobalCoopWarAcceptedState(PlayerTypes ePlayer);
	int GetGlobalCoopWarCounter(PlayerTypes ePlayer);
	bool IsLockedIntoCoopWar(PlayerTypes ePlayer);

	// Human Demand
	void DoDemandMade(PlayerTypes ePlayer);
	bool IsDemandTooSoon(PlayerTypes ePlayer) const;
	short GetDemandTooSoonNumTurns(PlayerTypes ePlayer) const;

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

	// Working With Player
	bool IsDoFAcceptable(PlayerTypes ePlayer);
	bool IsTooEarlyForDoF(PlayerTypes ePlayer);
	bool IsDoFMessageTooSoon(PlayerTypes ePlayer) const;

	bool IsDoFEverAsked(PlayerTypes ePlayer) const;

	bool IsDoFRejected(PlayerTypes ePlayer) const;
	bool IsDoFAccepted(PlayerTypes ePlayer) const;
	void SetDoFAccepted(PlayerTypes ePlayer, bool bValue);

	short GetDoFCounter(PlayerTypes ePlayer) const;
	void SetDoFCounter(PlayerTypes ePlayer, int iValue);
	void ChangeDoFCounter(PlayerTypes ePlayer, int iChange);

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

	PlayerTypes GetRequestFriendToDenounce(PlayerTypes ePlayer, bool &bRandFailed);
	bool IsFriendDenounceRefusalUnacceptable(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer);

	// Problems between friends

	bool IsUntrustworthyFriend();
	int GetNumFriendsDenouncedBy();

	bool IsFriendDenouncedUs(PlayerTypes ePlayer) const;	// They denounced us while we were friends!
	void SetFriendDenouncedUs(PlayerTypes ePlayer, bool bValue);
	int GetWeDenouncedFriendCount();

	bool IsFriendDeclaredWarOnUs(PlayerTypes ePlayer) const;	// They declared war on u s while we were friends!
	void SetFriendDeclaredWarOnUs(PlayerTypes ePlayer, bool bValue);
	int GetWeDeclaredWarOnFriendCount();

	short GetNumRequestsRefused(PlayerTypes ePlayer) const;
	void SetNumRequestsRefused(PlayerTypes ePlayer, int iValue);
	void ChangeNumRequestsRefused(PlayerTypes ePlayer, int iChange);

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

	bool IsPlayerMadeExpansionPromise(PlayerTypes ePlayer);
	void SetPlayerMadeExpansionPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenExpansionPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenExpansionPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredExpansionPromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredExpansionPromise(PlayerTypes ePlayer, bool bValue);
	AggressivePostureTypes GetPlayerExpansionPromiseData(PlayerTypes ePlayer);
	void SetPlayerExpansionPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue);

	bool IsPlayerMadeBorderPromise(PlayerTypes ePlayer);
	void SetPlayerMadeBorderPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenBorderPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenBorderPromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredBorderPromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredBorderPromise(PlayerTypes ePlayer, bool bValue);
	AggressivePostureTypes GetPlayerBorderPromiseData(PlayerTypes ePlayer);
	void SetPlayerBorderPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue);

	bool IsPlayerMadeCityStatePromise(PlayerTypes ePlayer);
	void SetPlayerMadeCityStatePromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerBrokenCityStatePromise(PlayerTypes ePlayer);
	void SetPlayerBrokenCityStatePromise(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerIgnoredCityStatePromise(PlayerTypes ePlayer);
	void SetPlayerIgnoredCityStatePromise(PlayerTypes ePlayer, bool bValue);

	bool IsPlayerBrokenCoopWarPromise(PlayerTypes ePlayer);
	void SetPlayerBrokenCoopWarPromise(PlayerTypes ePlayer, bool bValue);

	int GetNumCiviliansReturnedToMe(PlayerTypes ePlayer) const;
	void ChangeNumCiviliansReturnedToMe(PlayerTypes ePlayer, int iChange);

	int GetNumTimesCultureBombed(PlayerTypes ePlayer) const;
	void ChangeNumTimesCultureBombed(PlayerTypes ePlayer, int iChange);

	int GetNumTimesNuked(PlayerTypes ePlayer) const;
	void ChangeNumTimesNuked(PlayerTypes ePlayer, int iChange);

	/////////////////////////////////////////////////////////
	// Miscellaneous
	/////////////////////////////////////////////////////////

	bool DoPossibleMinorLiberation(PlayerTypes eMinor, int iCityID);

	int GetNumOurEnemiesPlayerAtWarWith(PlayerTypes ePlayer);

	bool IsGoingForWorldConquest();
	bool IsGoingForDiploVictory();
	bool IsGoingForCultureVictory();
	bool IsGoingForSpaceshipVictory();

	bool IsPlayerValid(PlayerTypes eOtherPlayer, bool bMyTeamIsValid = false);

	// Diplo Statement Log
	void DoAddNewStatementToDiploLog(PlayerTypes ePlayer, DiploStatementTypes eNewDiploLogStatement);

	DiploStatementTypes GetDiploLogStatementTypeForIndex(PlayerTypes ePlayer, int iIndex);
	int GetDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex);
	void SetDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex, int iNewValue);
	void ChangeDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex, int iChange);

	int GetNumTurnsSinceStatementSent(PlayerTypes ePlayer, DiploStatementTypes eDiploLogStatement);

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

	// Methods for injecting tests
	void TestUIDiploStatement(PlayerTypes eToPlayer, DiploStatementTypes eStatement, int iArg1);

private:
	bool IsValidUIDiplomacyTarget(PlayerTypes eTargetPlayer);

	bool IsAtWar(PlayerTypes eOtherPlayer);
	void DoMakeWarOnPlayer(PlayerTypes eTargetPlayer);

	void LogPublicDeclaration(PublicDeclarationTypes eDeclaration, int iData1, PlayerTypes eForSpecificPlayer = NO_PLAYER);
	void LogWarDeclaration(PlayerTypes ePlayer, int iTotalWarWeight = -1);
	void LogPeaceMade(PlayerTypes ePlayer);
	void LogDoF(PlayerTypes ePlayer);
	void LogDenounce(PlayerTypes ePlayer, bool bBackstab = false, bool bRefusal = false);
	void LogFriendRequestDenounce(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bAgreed);
	void LogCoopWar(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, CoopWarStates eAcceptedState);
	void LogWantRA(PlayerTypes ePlayer);

	void LogOpinionUpdate(PlayerTypes ePlayer, std::vector<int> &viOpinionValues);
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
	void LogWonderDispute(CvString& strString, PlayerTypes ePlayer);
	void LogMinorCivDispute(CvString& strString, PlayerTypes ePlayer);
	void LogProximity(CvString& strString, PlayerTypes ePlayer);

	void LogMinorCivGift(PlayerTypes ePlayer, int iOldFriendship, int iGold, bool bSaving, bool bWantQuickBoost, PlayerTypes ePlayerTryingToPass);

	void LogStatementToPlayer(PlayerTypes ePlayer, DiploStatementTypes eMessage);

	CvPlayer *m_pPlayer;

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
		bool m_abWantToRouteToMinor[REALLY_MAX_PLAYERS-MAX_MAJOR_CIVS];
		char m_aeWarFace[REALLY_MAX_PLAYERS];
		char m_aeWarState[REALLY_MAX_PLAYERS];
		char m_aeWarProjection[REALLY_MAX_PLAYERS];
		char m_aeWarGoal[REALLY_MAX_PLAYERS];
		short m_aiPlayerNumTurnsAtWar[REALLY_MAX_PLAYERS];
		short m_aiNumWarsFought[REALLY_MAX_PLAYERS];
		char m_aePlayerMilitaryStrengthComparedToUs[REALLY_MAX_PLAYERS];
		char m_aePlayerEconomicStrengthComparedToUs[REALLY_MAX_PLAYERS];
		char m_aePlayerTargetValue[REALLY_MAX_PLAYERS];
		char m_aePlayerLandDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aePlayerLastTurnLandDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aePlayerVictoryDisputeLevel[REALLY_MAX_PLAYERS];
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

		short m_aiDemandCounter[MAX_MAJOR_CIVS];
		short m_aiDemandTooSoonNumTurns[MAX_MAJOR_CIVS];

		bool m_abDoFAccepted[MAX_MAJOR_CIVS];
		short m_aiDoFCounter[MAX_MAJOR_CIVS];

		bool m_abDenouncedPlayer[MAX_MAJOR_CIVS];
		bool m_abFriendDenouncedUs[MAX_MAJOR_CIVS];
		bool m_abFriendDeclaredWarOnUs[MAX_MAJOR_CIVS];
		short m_aiDenouncedPlayerCounter[MAX_MAJOR_CIVS];

		short m_aiNumRequestsRefused[MAX_MAJOR_CIVS];

		short m_aiNumCiviliansReturnedToMe[MAX_MAJOR_CIVS];
		short m_aiNumTimesCultureBombed[MAX_MAJOR_CIVS];

		short m_aiNumTimesNuked[MAX_MAJOR_CIVS];
		short m_aiTradeValue[MAX_MAJOR_CIVS];
		short m_aiCommonFoeValue[MAX_MAJOR_CIVS];
		short m_aiAssistValue[MAX_MAJOR_CIVS];

		// Player's repsonse to AI statements

		bool m_abPlayerMadeMilitaryPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenMilitaryPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredMilitaryPromise[MAX_MAJOR_CIVS];
		short m_aiPlayerMilitaryPromiseCounter[MAX_MAJOR_CIVS];

		bool m_abPlayerMadeExpansionPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenExpansionPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredExpansionPromise[MAX_MAJOR_CIVS];
		char m_aePlayerExpansionPromiseData[MAX_MAJOR_CIVS];

		bool m_abPlayerMadeBorderPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenBorderPromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredBorderPromise[MAX_MAJOR_CIVS];
		char m_aePlayerBorderPromiseData[MAX_MAJOR_CIVS];

		bool m_abPlayerMadeCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerBrokenCityStatePromise[MAX_MAJOR_CIVS];
		bool m_abPlayerIgnoredCityStatePromise[MAX_MAJOR_CIVS];

		bool m_abPlayerBrokenCoopWarPromise[MAX_MAJOR_CIVS];

		// Evaluation of Other Players' Tendencies
		short m_aiOtherPlayerTurnsSinceAttackedProtectedMinor[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerProtectedMinorAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumProtectedMinorsAttacked[MAX_MAJOR_CIVS];
		short m_aiOtherPlayerTurnsSinceKilledProtectedMinor[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerProtectedMinorKilled[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumProtectedMinorsKilled[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMinorsAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMinorsConquered[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMajorsAttacked[MAX_MAJOR_CIVS];
		char m_aiOtherPlayerNumMajorsConquered[MAX_MAJOR_CIVS];

		//2D Arrays
		char* m_apaeOtherPlayerMajorCivOpinion[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerLandDisputeLevel[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerVictoryDisputeLevel[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerWonderDisputeLevel[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerMinorCivDisputeLevel[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerWarDamageLevel[REALLY_MAX_PLAYERS];
		int* m_apaiOtherPlayerWarValueLost[REALLY_MAX_PLAYERS];
		char* m_apaeOtherPlayerMilitaryThreat[REALLY_MAX_PLAYERS];
		DiploLogData* m_apaDiploStatementsLog[MAX_MAJOR_CIVS];

		bool* m_apabWorkingAgainstPlayerAccepted[MAX_MAJOR_CIVS];
		short* m_apaiWorkingAgainstPlayerCounter[MAX_MAJOR_CIVS];

		char* m_apacCoopWarAcceptedState[MAX_MAJOR_CIVS];
		short* m_apaiCoopWarCounter[MAX_MAJOR_CIVS];

		char m_aaeOtherPlayerMajorCivOpinion[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
		char m_aaeOtherPlayerLandDisputeLevel[REALLY_MAX_PLAYERS * REALLY_MAX_PLAYERS];
		char m_aaeOtherPlayerVictoryDisputeLevel[REALLY_MAX_PLAYERS];
		char m_aaeOtherPlayerWarDamageLevel[REALLY_MAX_PLAYERS * REALLY_MAX_PLAYERS];
		int m_aaiOtherPlayerWarValueLost[REALLY_MAX_PLAYERS * REALLY_MAX_PLAYERS];
		char m_aaeOtherPlayerMilitaryThreat[REALLY_MAX_PLAYERS * REALLY_MAX_PLAYERS];
		DiploLogData m_aaDiploStatementsLog[MAX_MAJOR_CIVS*MAX_DIPLO_LOG_STATEMENTS];

		bool m_aabWorkingAgainstPlayerAccepted[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
		short m_aaiWorkingAgainstPlayerCounter[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];

		char m_aacCoopWarAcceptedState[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
		short m_aaiCoopWarCounter[MAX_MAJOR_CIVS * MAX_MAJOR_CIVS];
	};
	DiplomacyAIData* m_pDiploData;

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
	bool* m_pabWantToRouteToMinor;

	char* m_paeWarFace;
	char* m_paeWarState;
	char* m_paeWarProjection;
	char* m_paeWarGoal;
	short* m_paiPlayerNumTurnsAtWar;
	short* m_paiNumWarsFought;

	char* m_paePlayerMilitaryStrengthComparedToUs;
	char* m_paePlayerEconomicStrengthComparedToUs;
	char* m_paePlayerTargetValue;

	char* m_paeWarDamageLevel;
	int* m_paiWarValueLost;
	char** m_ppaaeOtherPlayerWarDamageLevel;
	int** m_ppaaiOtherPlayerWarValueLost;

	char* m_paeMilitaryAggressivePosture;
	char* m_paeLastTurnMilitaryAggressivePosture;
	char* m_paeExpansionAggressivePosture;
	char* m_paePlotBuyingAggressivePosture;

	char* m_paePlayerLandDisputeLevel;
	char* m_paePlayerLastTurnLandDisputeLevel;
	char* m_paePlayerVictoryDisputeLevel;
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

	short* m_paiDemandCounter;
	short* m_paiDemandTooSoonNumTurns;

	bool* m_pabDoFAccepted;
	short* m_paiDoFCounter;

	bool* m_pabDenouncedPlayer;
	bool* m_pabFriendDenouncedUs;
	bool* m_pabFriendDeclaredWarOnUs;
	short* m_paiDenouncedPlayerCounter;

	short* m_paiNumRequestsRefused;

	short* m_paiNumCiviliansReturnedToMe;
	short* m_paiNumTimesCultureBombed;

	short* m_paiNumTimesNuked;
	short* m_paiTradeValue;
	short* m_paiCommonFoeValue;
	short* m_paiAssistValue;

	bool** m_ppaabWorkingAgainstPlayerAccepted;
	short** m_ppaaiWorkingAgainstPlayerCounter;

	char** m_ppaacCoopWarAcceptedState;
	short** m_ppaaiCoopWarCounter;

	// Player's repsonse to AI statements

	bool* m_pabPlayerMadeMilitaryPromise;
	bool* m_pabPlayerBrokenMilitaryPromise;
	bool* m_pabPlayerIgnoredMilitaryPromise;
	short* m_paiPlayerMilitaryPromiseCounter;

	bool* m_pabPlayerMadeExpansionPromise;
	bool* m_pabPlayerBrokenExpansionPromise;
	bool* m_pabPlayerIgnoredExpansionPromise;
	char* m_paePlayerExpansionPromiseData;

	bool* m_pabPlayerMadeBorderPromise;
	bool* m_pabPlayerBrokenBorderPromise;
	bool* m_pabPlayerIgnoredBorderPromise;
	char* m_paePlayerBorderPromiseData;

	bool* m_pabPlayerMadeCityStatePromise;
	bool* m_pabPlayerBrokenCityStatePromise;
	bool* m_pabPlayerIgnoredCityStatePromise;

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

	char* m_paiOtherPlayerNumMinorsAttacked;
	char* m_paiOtherPlayerNumMinorsConquered;
	char* m_paiOtherPlayerNumMajorsAttacked;
	char* m_paiOtherPlayerNumMajorsConquered;

	// Memory of what we've talked about with people
	DiploLogData** m_ppaDiploStatementsLog;

	DeclarationLogData* m_paDeclarationsLog;

	// Overall status across all other civs
	StateAllWars m_eStateAllWars;

	// Other

	typedef std::vector<PlayerTypes> PlayerTypesArray;
	PlayerTypesArray	m_aGreetPlayers;

	PlayerTypes			m_eTargetPlayer;

	// Data members for injecting test messages
	PlayerTypes			m_eTestToPlayer;
	DiploStatementTypes m_eTestStatement;
	int					m_iTestStatementArg1;
};

#endif //CIV5_AI_DIPLOMACY_H