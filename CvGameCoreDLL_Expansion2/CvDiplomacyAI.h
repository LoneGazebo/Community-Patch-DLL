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

struct Opinion
{
	Localization::String m_str;
	int m_iValue;
};

#define WARSCORE_THRESHOLD_POSITIVE (+25)
#define WARSCORE_THRESHOLD_NEGATIVE (-25)

//=====================================
// CvDiplomacyAI
//=====================================

class CvDiplomacyAI
{
public:
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
	inline void SetTeam(TeamTypes eTeam) { m_eTeam = eTeam; }
	template<typename DiplomacyAI, typename Visitor>
	static void Serialize(DiplomacyAI& diplomacyAI, Visitor& visitor);
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream) const;
	void update();
	void SlotStateChange();

	// ************************************
	// Pointers
	// ************************************

	inline CvPlayer* GetPlayer() { return m_pPlayer; }
	inline const CvPlayer* GetPlayer() const { return m_pPlayer; }

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
	vector<PlayerTypes> GetOffensiveWarAllies(PlayerTypes eOtherPlayer, bool bIncludeMinors, bool bReverseMode) const;
	vector<PlayerTypes> GetDefensiveWarAllies(PlayerTypes eOtherPlayer, bool bIncludeMinors, bool bReverseMode, bool bNewWarsOnly) const;
	vector<PlayerTypes> GetWarAllies(PlayerTypes eOtherPlayer, bool bDefensive, bool bIncludeWars, bool bReverseMode, bool bNewWarsOnly) const;
	bool IsNuclearGandhi(bool bPotentially = false) const;

	// ************************************
	// Personality Flavors
	// ************************************

	int RandomizePersonalityFlavor(int iOriginalValue, const CvSeeder& seed);
	void DoInitializePersonality(bool bFirstInit);
	void SelectDefaultVictoryPursuits();

	int GetVictoryCompetitiveness() const;
	int GetWonderCompetitiveness() const;
	int GetMinorCivCompetitiveness() const;
	int GetBoldness() const;
	int GetDiploBalance() const;
	int GetWarmongerHate() const;
	int GetDoFWillingness() const;
	int GetDenounceWillingness() const;
	int GetWorkWithWillingness() const;
	int GetWorkAgainstWillingness() const;
	int GetLoyalty() const;
	int GetForgiveness() const;
	int GetNeediness() const;
	int GetMeanness() const;
	int GetChattiness() const;

	int GetMajorCivApproachBias(CivApproachTypes eApproach) const;
	int GetMinorCivApproachBias(CivApproachTypes eApproach, bool bHideAssert = false) const;

	int GetWarscoreThresholdPositive() const;
	int GetWarscoreThresholdNegative() const;

	VictoryPursuitTypes GetPrimaryVictoryPursuit() const;
	void SetPrimaryVictoryPursuit(VictoryPursuitTypes eVictoryPursuit);
	bool IsConqueror() const;
	bool IsDiplomat() const;
	bool IsCultural() const;
	bool IsScientist() const;

	VictoryPursuitTypes GetSecondaryVictoryPursuit() const;
	void SetSecondaryVictoryPursuit(VictoryPursuitTypes eVictoryPursuit);
	bool IsSecondaryConqueror() const;
	bool IsSecondaryDiplomat() const;
	bool IsSecondaryCultural() const;
	bool IsSecondaryScientist() const;

	VictoryPursuitTypes GetEternalVictoryPursuit() const;

	// ************************************
	// Memory Management
	// ************************************

	// ------------------------------------
	// Diplomatic Interactions
	// ------------------------------------

	// Diplo Statement Log
	int GetTurnStatementLastSent(PlayerTypes ePlayer, DiploStatementTypes eDiploStatement) const;
	void SetTurnStatementLastSent(PlayerTypes ePlayer, DiploStatementTypes eDiploStatement, int iTurn);
	int GetNumTurnsSinceStatementSent(PlayerTypes ePlayer, DiploStatementTypes eDiploStatement) const;
	int GetNumTurnsSinceSomethingSent(PlayerTypes ePlayer) const;

	// Messages sent to other players about protected Minor Civs
	bool HasSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor) const;
	void SetSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue);
	void ResetSentAttackProtectedMinorTaunts(PlayerTypes eMinor);

	// ------------------------------------
	// Victory Competition
	// ------------------------------------

	bool IsCompetingForVictory() const;
	void SetCompetingForVictory(bool bValue);

	VictoryPursuitTypes GetCurrentVictoryPursuit() const;
	void SetCurrentVictoryPursuit(VictoryPursuitTypes eVictoryPursuit);
	bool IsGoingForWorldConquest() const;
	bool IsGoingForDiploVictory() const;
	bool IsGoingForCultureVictory() const;
	bool IsGoingForSpaceshipVictory() const;

	bool IsEndgameAggressiveTo(PlayerTypes ePlayer) const;
	void SetEndgameAggressiveTo(PlayerTypes ePlayer, bool bValue);

	bool IsRecklessExpander(PlayerTypes ePlayer) const;
	void SetRecklessExpander(PlayerTypes ePlayer, bool bValue);

	bool IsWonderSpammer(PlayerTypes ePlayer) const;
	void SetWonderSpammer(PlayerTypes ePlayer, bool bValue);

	// ------------------------------------
	// Victory Progress
	// ------------------------------------

	int GetScoreVictoryProgress() const;
	int GetDominationVictoryProgress() const;
	int GetDiplomaticVictoryProgress() const;
	int GetScienceVictoryProgress() const;
	int GetCultureVictoryProgress() const;
	int GetLowestTourismInfluence() const;

	bool IsCloseToAnyVictoryCondition() const;
	bool IsCloseToWorldConquest() const;
	bool IsCloseToDiploVictory() const;
	bool IsCloseToSpaceshipVictory() const;
	bool IsCloseToCultureVictory() const;

	// ------------------------------------
	// Opinion & Approach
	// ------------------------------------

	// Opinion
	CivOpinionTypes GetCivOpinion(PlayerTypes ePlayer) const;
	void SetCivOpinion(PlayerTypes ePlayer, CivOpinionTypes eOpinion);

	int GetCachedOpinionWeight(PlayerTypes ePlayer) const;
	void SetCachedOpinionWeight(PlayerTypes ePlayer, int iWeight);

	// Approach
	CivApproachTypes GetCivApproach(PlayerTypes ePlayer) const;
	void SetCivApproach(PlayerTypes ePlayer, CivApproachTypes eApproach, bool bResetAttackOperations = true);

	CivApproachTypes GetCivStrategicApproach(PlayerTypes ePlayer) const;
	void SetCivStrategicApproach(PlayerTypes ePlayer, CivApproachTypes eApproach);

	int GetCachedSurfaceApproach(PlayerTypes ePlayer) const;
	void SetCachedSurfaceApproach(PlayerTypes ePlayer, int iApproach);

	CivApproachTypes GetSurfaceApproach(PlayerTypes ePlayer) const;
	int GetSurfaceApproachDealModifier(PlayerTypes ePlayer, bool bFromMe) const;
	CivApproachTypes GetVisibleApproachTowardsUs(PlayerTypes ePlayer) const; // Our guess as to another player's approach towards us

	bool IsWantsSneakAttack(PlayerTypes ePlayer) const; // Do we want to launch a Sneak Attack Operation against ePlayer?

	// Approach Values: Cached weight for each approach
	int GetPlayerApproachValue(PlayerTypes ePlayer, CivApproachTypes eApproach) const;
	void SetPlayerApproachValue(PlayerTypes ePlayer, CivApproachTypes eApproach, int iValue);
	CivApproachTypes GetHighestValueApproach(PlayerTypes ePlayer, bool bExcludeWar = false, bool bIncludeOverrides = false) const;
	PlayerTypes GetPlayerWithHighestApproachValue(CivApproachTypes eApproach, vector<PlayerTypes>& vPlayersToExclude) const;

	int GetPlayerStrategicApproachValue(PlayerTypes ePlayer, CivApproachTypes eApproach) const;
	void SetPlayerStrategicApproachValue(PlayerTypes ePlayer, CivApproachTypes eApproach, int iValue);
	PlayerTypes GetPlayerWithHighestStrategicApproachValue(CivApproachTypes eApproach, vector<PlayerTypes>& vPlayersToExclude) const;

	// ------------------------------------
	// Minor Civs
	// ------------------------------------

	PlayerTypes GetCSBullyTargetPlayer() const;
	void SetCSBullyTargetPlayer(PlayerTypes ePlayer);

	PlayerTypes GetCSWarTargetPlayer() const;
	void SetCSWarTargetPlayer(PlayerTypes ePlayer);

	bool IsWantToRouteConnectToMinor(PlayerTypes eMinor);
	void SetWantToRouteConnectToMinor(PlayerTypes eMinor, bool bValue);

	// ------------------------------------
	// Planning Exchanges
	// ------------------------------------

	bool IsMajorCompetitor(PlayerTypes ePlayer) const;
	void SetMajorCompetitor(PlayerTypes ePlayer, bool bValue);

	bool IsStrategicTradePartner(PlayerTypes ePlayer) const;
	void SetStrategicTradePartner(PlayerTypes ePlayer, bool bValue);

	// Cached Exchange Desires
	bool IsWantsDoFWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsDoFWithPlayer(PlayerTypes ePlayer, bool bValue);

	bool IsWantsDefensivePactWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsDefensivePactWithPlayer(PlayerTypes ePlayer, bool bValue);

	bool IsWantsToEndDoFWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsToEndDoFWithPlayer(PlayerTypes ePlayer, bool bValue);

	bool IsWantsToEndDefensivePactWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsToEndDefensivePactWithPlayer(PlayerTypes ePlayer, bool bValue);

	bool IsWantsResearchAgreementWithPlayer(PlayerTypes ePlayer) const;
	void SetWantsResearchAgreementWithPlayer(PlayerTypes ePlayer, bool bValue);

	// Key Players
	PlayerTypes GetMostValuableFriend() const;
	void SetMostValuableFriend(PlayerTypes ePlayer);
	PlayerTypes GetMostValuableAlly() const;
	void SetMostValuableAlly(PlayerTypes ePlayer);
	PlayerTypes GetBiggestCompetitor() const;
	void SetBiggestCompetitor(PlayerTypes ePlayer);
	PlayerTypes GetPrimeLeagueAlly() const;
	void SetPrimeLeagueAlly(PlayerTypes ePlayer);
	PlayerTypes GetPrimeLeagueCompetitor() const;
	void SetPrimeLeagueCompetitor(PlayerTypes ePlayer);
	PlayerTypes GetDemandTargetPlayer() const;
	void SetDemandTargetPlayer(PlayerTypes ePlayer);

	// ------------------------------------
	// Exchanges
	// ------------------------------------

	// Declaration of Friendship
	bool IsDoFAccepted(PlayerTypes ePlayer) const;
	void SetDoFAccepted(PlayerTypes ePlayer, bool bValue);
	int GetNumDoF(bool bVassalageException = false) const;
	bool IsDoFMessageTooSoon(PlayerTypes ePlayer) const;

	int GetDoFAcceptedTurn(PlayerTypes ePlayer) const;
	void SetDoFAcceptedTurn(PlayerTypes ePlayer, int iTurn);
	int GetTurnsSinceBefriendedPlayer(PlayerTypes ePlayer) const;

	DoFLevelTypes GetDoFType(PlayerTypes ePlayer) const;
	void SetDoFType(PlayerTypes ePlayer, DoFLevelTypes eDoFLevel);

	// Denouncement
	bool IsDenouncedPlayer(PlayerTypes ePlayer) const;
	void SetDenouncedPlayer(PlayerTypes ePlayer, bool bValue);
	bool IsDenouncedByPlayer(PlayerTypes ePlayer) const;
	bool IsDenounceMessageTooSoon(PlayerTypes ePlayer) const;

	int GetDenouncedPlayerTurn(PlayerTypes ePlayer) const;
	void SetDenouncedPlayerTurn(PlayerTypes ePlayer, int iTurn);
	int GetTurnsSinceDenouncedPlayer(PlayerTypes ePlayer) const;
	bool IsDenouncingPlayer(PlayerTypes ePlayer) const;

	int GetNumDenouncements() const;
	int GetNumDenouncementsOfPlayer() const;

	// Trade
	bool IsCantMatchDeal(PlayerTypes ePlayer);
	void SetCantMatchDeal(PlayerTypes ePlayer, bool bValue);

	// Demands
	int GetNumDemandsMade(PlayerTypes ePlayer) const;
	void SetNumDemandsMade(PlayerTypes ePlayer, int iValue);
	void ChangeNumDemandsMade(PlayerTypes ePlayer, int iChange);
	bool IsDemandMade(PlayerTypes ePlayer) const;

	int GetDemandMadeTurn(PlayerTypes ePlayer) const;
	void SetDemandMadeTurn(PlayerTypes ePlayer, int iTurn);

	int GetDemandTooSoonNumTurns(PlayerTypes ePlayer) const;
	void SetDemandTooSoonNumTurns(PlayerTypes ePlayer, int iValue);
	bool IsDemandTooSoon(PlayerTypes ePlayer) const;

	int GetNumConsecutiveDemandsTheyAccepted(PlayerTypes ePlayer) const;
	void SetNumConsecutiveDemandsTheyAccepted(PlayerTypes ePlayer, int iValue);
	void ChangeNumConsecutiveDemandsTheyAccepted(PlayerTypes ePlayer, int iChange);
	int GetDemandAcceptedTurn(PlayerTypes ePlayer) const;
	void SetDemandAcceptedTurn(PlayerTypes ePlayer, int iTurn);
	bool IsRecentDemandAccepted(PlayerTypes ePlayer) const;

	// Assistance Values
	int GetMaxRecentTradeValue() const;
	int GetRecentTradeValue(PlayerTypes ePlayer) const;
	void SetRecentTradeValue(PlayerTypes ePlayer, int iValue);
	void ChangeRecentTradeValue(PlayerTypes ePlayer, int iChange);

	int GetMaxCommonFoeValue() const;
	int GetCommonFoeValue(PlayerTypes ePlayer) const;
	void SetCommonFoeValue(PlayerTypes ePlayer, int iValue);
	void ChangeCommonFoeValue(PlayerTypes ePlayer, int iChange);

	int GetMaxRecentAssistValue() const;
	int GetMaxRecentFailedAssistValue() const;
	int GetRecentAssistValue(PlayerTypes ePlayer) const;
	void SetRecentAssistValue(PlayerTypes ePlayer, int iValue);
	void ChangeRecentAssistValue(PlayerTypes ePlayer, int iChange, bool bDecay = false);

	// ------------------------------------
	// Coop Wars
	// ------------------------------------

	CoopWarStates GetCoopWarState(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer) const;
	void SetCoopWarState(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer, CoopWarStates eNewState, bool bSkipLogging = false);
	bool IsLockedIntoCoopWar(PlayerTypes ePlayer) const;

	CoopWarStates GetGlobalCoopWarAgainstState(PlayerTypes ePlayer) const;
	CoopWarStates GetGlobalCoopWarWithState(PlayerTypes ePlayer, bool bExcludeOngoing = false) const;
	int GetNumCoopWarTargets() const;

	int GetCoopWarStateChangeTurn(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer) const;
	void SetCoopWarStateChangeTurn(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer, int iTurn);
	bool IsCoopWarMessageTooSoon(PlayerTypes eAskingPlayer, PlayerTypes eTargetPlayer) const;

	int GetCoopWarAgreementScore(PlayerTypes ePlayer) const;
	void SetCoopWarAgreementScore(PlayerTypes ePlayer, int iValue);
	void ChangeCoopWarAgreementScore(PlayerTypes ePlayer, int iChange);

	// ------------------------------------
	// War
	// ------------------------------------

	// Is it sane to attack ePlayer?
	bool IsSaneDiplomaticTarget(PlayerTypes ePlayer) const;
	void SetSaneDiplomaticTarget(PlayerTypes ePlayer, bool bValue);
	bool IsWarSane(PlayerTypes ePlayer) const;

	// Would we potentially attack ePlayer if someone else asked us to?
	bool IsPotentialWarTarget(PlayerTypes ePlayer) const;
	void SetPotentialWarTarget(PlayerTypes ePlayer, bool bValue);

	// Mustering For Attack: Is there Sneak Attack Operation completed and ready to roll against ePlayer?
	bool IsArmyInPlaceForAttack(PlayerTypes ePlayer) const;
	void SetArmyInPlaceForAttack(PlayerTypes ePlayer, bool bValue);

	// Did we want to start the war we're currently in with ePlayer?
	bool IsAggressor(PlayerTypes ePlayer) const;
	void SetAggressor(PlayerTypes ePlayer, bool bValue);

	// How many times have we gone to war?
	int GetNumWarsFought(PlayerTypes ePlayer) const;
	void SetNumWarsFought(PlayerTypes ePlayer, int iValue);
	void ChangeNumWarsFought(PlayerTypes ePlayer, int iChange);

	int GetNumWarsDeclaredOnUs(PlayerTypes ePlayer) const;
	void SetNumWarsDeclaredOnUs(PlayerTypes ePlayer, int iValue);
	void ChangeNumWarsDeclaredOnUs(PlayerTypes ePlayer, int iChange);

	// How much do we hate them for killing or capturing our civilians?
	int GetMaxCivilianKillerValue() const;
	int GetCivilianKillerValue(PlayerTypes ePlayer) const;
	void SetCivilianKillerValue(PlayerTypes ePlayer, int iValue);
	void ChangeCivilianKillerValue(PlayerTypes ePlayer, int iChange);

	// How many of our cities have they captured?
	int GetNumCitiesCapturedBy(PlayerTypes ePlayer) const;
	void SetNumCitiesCapturedBy(PlayerTypes ePlayer, int iValue);
	void ChangeNumCitiesCapturedBy(PlayerTypes ePlayer, int iChange);

	// War State: How's the war with ePlayer going? (NO_WAR_STATE_TYPE if at peace)
	WarStateTypes GetWarState(PlayerTypes ePlayer) const;
	void SetWarState(PlayerTypes ePlayer, WarStateTypes eWarState);

	StateAllWars GetStateAllWars() const;
	void SetStateAllWars(StateAllWars eState);

	int GetWarProgressScore(PlayerTypes ePlayer) const;
	void SetWarProgressScore(PlayerTypes ePlayer, int iValue);
	void ChangeWarProgressScore(PlayerTypes ePlayer, int iChange);

	// ------------------------------------
	// Peace
	// ------------------------------------

	// What are we willing to give up to ePlayer to make peace?
	PeaceTreatyTypes GetTreatyWillingToOffer(PlayerTypes ePlayer) const;
	void SetTreatyWillingToOffer(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty);

	// What are we willing to accept from ePlayer to make peace?
	PeaceTreatyTypes GetTreatyWillingToAccept(PlayerTypes ePlayer) const;
	void SetTreatyWillingToAccept(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty);

	// ------------------------------------
	// Backstabbing Penalties
	// ------------------------------------

	// Have we ever betrayed a friend?
	bool IsBackstabber() const;
	void SetBackstabber(bool bValue);

	// Is this friend untrustworthy?
	bool IsUntrustworthyFriend(PlayerTypes ePlayer) const; // our opinion alone
	void SetUntrustworthyFriend(PlayerTypes ePlayer, bool bValue);
	bool IsUntrustworthy(PlayerTypes ePlayer) const; // our opinion plus all of our teammates' opinions (true if anyone thinks they're a backstabber)
	void SetUntrustworthy(PlayerTypes ePlayer, bool bValue);

	// Did this friend ever betray us?
	bool WasEverBackstabbedBy(PlayerTypes ePlayer) const;
	void SetEverBackstabbedBy(PlayerTypes ePlayer, bool bValue);
	void SetBackstabbedBy(PlayerTypes ePlayer, bool bValue, bool bSkipReevaluation);

	// DoF Broken?
	bool IsDoFBroken(PlayerTypes ePlayer) const;
	void SetDoFBroken(PlayerTypes ePlayer, bool bValue, bool bSkipTraitorUpdate);
	int GetDoFBrokenTurn(PlayerTypes ePlayer) const;
	void SetDoFBrokenTurn(PlayerTypes ePlayer, int iTurn);
	int GetTurnsSinceDoFBroken(PlayerTypes ePlayer) const;

	// They denounced us while we were friends!
	bool IsFriendDenouncedUs(PlayerTypes ePlayer) const;
	void SetFriendDenouncedUs(PlayerTypes ePlayer, bool bValue);
	int GetFriendDenouncedUsTurn(PlayerTypes ePlayer) const;
	void SetFriendDenouncedUsTurn(PlayerTypes ePlayer, int iTurn);
	int GetNumFriendsDenouncedBy() const;
	int GetWeDenouncedFriendCount();

	// They declared war on us while we were friends!
	bool IsFriendDeclaredWarOnUs(PlayerTypes ePlayer) const;
	void SetFriendDeclaredWarOnUs(PlayerTypes ePlayer, bool bValue);
	int GetFriendDeclaredWarOnUsTurn(PlayerTypes ePlayer) const;
	void SetFriendDeclaredWarOnUsTurn(PlayerTypes ePlayer, int iTurn);
	int GetWeDeclaredWarOnFriendCount(PlayerTypes eObserver, bool bExcludeOtherBackstabbers = true);

	// ------------------------------------
	// Warmongering Penalties
	// ------------------------------------

	// Num Minors Attacked
	int GetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iChange, TeamTypes eAttackedTeam);

	// Num Minors Conquered
	int GetPlayerNumMinorsConquered(PlayerTypes ePlayer) const;
	void SetPlayerNumMinorsConquered(PlayerTypes ePlayer, int iValue);

	// Num Majors Attacked
	int GetOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer, int iChange, TeamTypes eAttackedTeam);

	// Num Majors Conquered
	int GetPlayerNumMajorsConquered(PlayerTypes ePlayer) const;
	void SetPlayerNumMajorsConquered(PlayerTypes ePlayer, int iValue);

	// The amount of warmonger hatred they generated
	int GetOtherPlayerWarmongerAmount(PlayerTypes ePlayer) const;
	int GetOtherPlayerWarmongerAmountTimes100(PlayerTypes ePlayer) const;
	void SetOtherPlayerWarmongerAmountTimes100(PlayerTypes ePlayer, int iValue);
	void ChangeOtherPlayerWarmongerAmountTimes100(PlayerTypes ePlayer, int iChange);

	// ------------------------------------
	// Aggressive Postures
	// ------------------------------------

	// Military Aggressive Posture: How aggressively has ePlayer positioned their Units in relation to us?
	AggressivePostureTypes GetMilitaryAggressivePosture(PlayerTypes ePlayer) const;
	void SetMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);

	// Expansion Aggressive Posture: How aggressively has ePlayer settled or conquered in proximity to us?
	AggressivePostureTypes GetExpansionAggressivePosture(PlayerTypes ePlayer) const;

	// Plot Buying Aggressive Posture: How aggressively is ePlayer buying land near us?
	AggressivePostureTypes GetPlotBuyingAggressivePosture(PlayerTypes ePlayer) const;
	void SetPlotBuyingAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);

	// ------------------------------------
	// Dispute Levels
	// ------------------------------------

	// Land Dispute
	DisputeLevelTypes GetLandDisputeLevel(PlayerTypes ePlayer) const;
	void SetLandDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel);

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

	// Tech Block
	BlockLevelTypes GetTechBlockLevel(PlayerTypes ePlayer) const;
	void SetTechBlockLevel(PlayerTypes ePlayer, BlockLevelTypes eBlockLevel);

	// Policy Block
	BlockLevelTypes GetPolicyBlockLevel(PlayerTypes ePlayer) const;
	void SetPolicyBlockLevel(PlayerTypes ePlayer, BlockLevelTypes eBlockLevel);

	// ------------------------------------
	// Threat Levels
	// ------------------------------------

	ThreatTypes GetWarmongerThreat(PlayerTypes ePlayer) const;
	void SetWarmongerThreat(PlayerTypes ePlayer, ThreatTypes eWarmongerThreat);

	// ------------------------------------
	// Strength Assessments
	// ------------------------------------

	// Economic Strength: How strong is ePlayer compared to US?
	StrengthTypes GetEconomicStrengthComparedToUs(PlayerTypes ePlayer) const;
	void SetEconomicStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eEconomicStrength);

	// Military Strength: How strong is ePlayer compared to US?
	StrengthTypes GetMilitaryStrengthComparedToUs(PlayerTypes ePlayer) const;
	void SetMilitaryStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eMilitaryStrength);
	StrengthTypes GetRawMilitaryStrengthComparedToUs(PlayerTypes ePlayer) const;
	void SetRawMilitaryStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eMilitaryStrength);

	// Target Value: how easy or hard of a target would ePlayer be to attack?
	TargetValueTypes GetTargetValue(PlayerTypes ePlayer) const;
	void SetTargetValue(PlayerTypes ePlayer, TargetValueTypes eTargetValue);
	TargetValueTypes GetRawTargetValue(PlayerTypes ePlayer) const;
	void SetRawTargetValue(PlayerTypes ePlayer, TargetValueTypes eTargetValue);

	bool IsEasyTarget(PlayerTypes ePlayer) const;
	void SetEasyTarget(PlayerTypes ePlayer, bool bValue);

	// ------------------------------------
	// Promises
	// ------------------------------------

	// Military Promise
	PromiseStates GetMilitaryPromiseState(PlayerTypes ePlayer) const;
	void SetMilitaryPromiseState(PlayerTypes ePlayer, PromiseStates ePromiseState);
	int GetMilitaryPromiseTurn(PlayerTypes ePlayer) const;
	void SetMilitaryPromiseTurn(PlayerTypes ePlayer, int iTurn);
	bool MadeMilitaryPromise(PlayerTypes ePlayer) const;
	int GetNumTurnsMilitaryPromise(PlayerTypes ePlayer) const;
	bool IgnoredMilitaryPromise(PlayerTypes ePlayer) const;
	bool BrokeMilitaryPromise(PlayerTypes ePlayer) const;

	// Expansion Promise
	PromiseStates GetExpansionPromiseState(PlayerTypes ePlayer) const;
	void SetExpansionPromiseState(PlayerTypes ePlayer, PromiseStates ePromiseState);
	bool MadeExpansionPromise(PlayerTypes ePlayer) const;
	bool IgnoredExpansionPromise(PlayerTypes ePlayer) const;
	bool BrokeExpansionPromise(PlayerTypes ePlayer) const;
	vector<PlayerTypes> GetPlayersWithNoSettlePolicy() const;
	int GetExpansionPromiseTurn(PlayerTypes ePlayer) const;
	void SetExpansionPromiseTurn(PlayerTypes ePlayer, int iTurn);
	int GetNumTurnsExpansionPromise(PlayerTypes ePlayer) const;
	bool IsDontSettleMessageTooSoon(PlayerTypes ePlayer) const;
	bool IsAngryAboutExpansion(PlayerTypes ePlayer) const;
	void SetAngryAboutExpansion(PlayerTypes ePlayer, bool bValue);
	bool EverRequestedExpansionPromise(PlayerTypes ePlayer) const;
	void SetEverRequestedExpansionPromise(PlayerTypes ePlayer, bool bValue);

	// Border Promise
	PromiseStates GetBorderPromiseState(PlayerTypes ePlayer) const;
	void SetBorderPromiseState(PlayerTypes ePlayer, PromiseStates ePromiseState);
	bool MadeBorderPromise(PlayerTypes ePlayer) const;
	bool IgnoredBorderPromise(PlayerTypes ePlayer) const;
	bool BrokeBorderPromise(PlayerTypes ePlayer) const;
	int GetBorderPromiseTurn(PlayerTypes ePlayer) const;
	void SetBorderPromiseTurn(PlayerTypes ePlayer, int iTurn);
	int GetNumTurnsBorderPromise(PlayerTypes ePlayer) const;
	AggressivePostureTypes GetBorderPromisePosture(PlayerTypes ePlayer) const;
	void SetBorderPromisePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture);
	bool EverMadeBorderPromise(PlayerTypes ePlayer) const;
	void SetEverMadeBorderPromise(PlayerTypes ePlayer, bool bValue);

	// Bully City-State Promise
	PromiseStates GetBullyCityStatePromiseState(PlayerTypes ePlayer) const;
	void SetBullyCityStatePromiseState(PlayerTypes ePlayer, PromiseStates ePromiseState);
	int GetBullyCityStatePromiseTurn(PlayerTypes ePlayer) const;
	void SetBullyCityStatePromiseTurn(PlayerTypes ePlayer, int iTurn);
	bool MadeBullyCityStatePromise(PlayerTypes ePlayer) const;
	bool IgnoredBullyCityStatePromise(PlayerTypes ePlayer) const;
	bool BrokeBullyCityStatePromise(PlayerTypes ePlayer) const;

	// Attack City-State Promise
	PromiseStates GetAttackCityStatePromiseState(PlayerTypes ePlayer) const;
	void SetAttackCityStatePromiseState(PlayerTypes ePlayer, PromiseStates ePromiseState);
	int GetAttackCityStatePromiseTurn(PlayerTypes ePlayer) const;
	void SetAttackCityStatePromiseTurn(PlayerTypes ePlayer, int iTurn);
	bool MadeAttackCityStatePromise(PlayerTypes ePlayer) const;
	bool IgnoredAttackCityStatePromise(PlayerTypes ePlayer) const;
	bool BrokeAttackCityStatePromise(PlayerTypes ePlayer) const;

	// Spy Promise
	PromiseStates GetSpyPromiseState(PlayerTypes ePlayer) const;
	void SetSpyPromiseState(PlayerTypes ePlayer, PromiseStates ePromiseState);
	int GetSpyPromiseTurn(PlayerTypes ePlayer) const;
	void SetSpyPromiseTurn(PlayerTypes ePlayer, int iTurn);
	bool MadeSpyPromise(PlayerTypes ePlayer) const;
	bool IgnoredSpyPromise(PlayerTypes ePlayer) const;
	bool BrokeSpyPromise(PlayerTypes ePlayer) const;
	bool IsStopSpyingMessageTooSoon(PlayerTypes ePlayer) const;

	// Religious Conversion Promise
	PromiseStates GetNoConvertPromiseState(PlayerTypes ePlayer) const;
	void SetNoConvertPromiseState(PlayerTypes ePlayer, PromiseStates ePromiseState);
	int GetNoConvertPromiseTurn(PlayerTypes ePlayer) const;
	void SetNoConvertPromiseTurn(PlayerTypes ePlayer, int iTurn);
	bool MadeNoConvertPromise(PlayerTypes ePlayer) const;
	bool IgnoredNoConvertPromise(PlayerTypes ePlayer) const;
	bool BrokeNoConvertPromise(PlayerTypes ePlayer) const;
	bool IsPlayerAskedNotToConvert(PlayerTypes ePlayer) const;
	void SetPlayerAskedNotToConvert(PlayerTypes ePlayer, bool bValue);
	bool HasEverConvertedCity(PlayerTypes ePlayer) const;
	void SetEverConvertedCity(PlayerTypes ePlayer, bool bValue);

	// Digging Promise
	PromiseStates GetNoDiggingPromiseState(PlayerTypes ePlayer) const;
	void SetNoDiggingPromiseState(PlayerTypes ePlayer, PromiseStates ePromiseState);
	int GetNoDiggingPromiseTurn(PlayerTypes ePlayer) const;
	void SetNoDiggingPromiseTurn(PlayerTypes ePlayer, int iTurn);
	bool MadeNoDiggingPromise(PlayerTypes ePlayer) const;
	bool BrokeNoDiggingPromise(PlayerTypes ePlayer) const;
	bool IgnoredNoDiggingPromise(PlayerTypes ePlayer) const;
	bool IsPlayerAskedNotToDig(PlayerTypes ePlayer) const;
	void SetPlayerAskedNotToDig(PlayerTypes ePlayer, bool bValue);

	// Coop War Promise
	bool BrokeCoopWarPromise(PlayerTypes ePlayer) const;
	void SetBrokeCoopWarPromise(PlayerTypes ePlayer, bool bValue);
	int GetBrokeCoopWarPromiseTurn(PlayerTypes ePlayer) const;
	void SetBrokeCoopWarPromiseTurn(PlayerTypes ePlayer, int iTurn);

	// Global Promise Checks
	bool BrokeAnyPromise(PlayerTypes ePlayer, int iWithinXTurns = -1) const;
	bool IgnoredAnyPromise(PlayerTypes ePlayer, int iWithinXTurns = -1) const;

	// ------------------------------------
	// Event Flags
	// ------------------------------------

	bool IsPlayerReturnedCapital(PlayerTypes ePlayer, bool bForPenaltyReduction = false) const;
	void SetPlayerReturnedCapital(PlayerTypes ePlayer, bool bValue);

	bool IsPlayerReturnedHolyCity(PlayerTypes ePlayer, bool bForPenaltyReduction = false) const;
	void SetPlayerReturnedHolyCity(PlayerTypes ePlayer, bool bValue);

	bool IsPlayerLiberatedCapital(PlayerTypes ePlayer) const;
	void SetPlayerLiberatedCapital(PlayerTypes ePlayer, bool bValue);

	bool IsPlayerLiberatedHolyCity(PlayerTypes ePlayer) const;
	void SetPlayerLiberatedHolyCity(PlayerTypes ePlayer, bool bValue);

	bool IsPlayerCapturedCapital(PlayerTypes ePlayer) const;
	void SetPlayerCapturedCapital(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerEverCapturedCapital(PlayerTypes ePlayer) const;
	void SetPlayerEverCapturedCapital(PlayerTypes ePlayer, bool bValue);
	bool IsCapitalCapturedBy(PlayerTypes ePlayer, bool bCurrently = false, bool bTeammates = true, bool bCheckEver = false) const;

	bool IsPlayerCapturedHolyCity(PlayerTypes ePlayer) const;
	void SetPlayerCapturedHolyCity(PlayerTypes ePlayer, bool bValue);
	bool IsPlayerEverCapturedHolyCity(PlayerTypes ePlayer) const;
	void SetPlayerEverCapturedHolyCity(PlayerTypes ePlayer, bool bValue);
	bool IsHolyCityCapturedBy(PlayerTypes ePlayer, bool bCurrently = false, bool bTeammates = true, bool bCheckEver = false) const;

	bool IsLiberator(PlayerTypes ePlayer, bool bIgnoreReturns = false, bool bOnlyMajorCities = false) const;

	bool IsResurrectorAttackedUs(PlayerTypes ePlayer) const;
	void SetResurrectorAttackedUs(PlayerTypes ePlayer, bool bValue);

	bool HasEverSanctionedUs(PlayerTypes ePlayer) const;
	void SetEverSanctionedUs(PlayerTypes ePlayer, bool bValue);

	bool HasEverUnsanctionedUs(PlayerTypes ePlayer) const;
	void SetEverUnsanctionedUs(PlayerTypes ePlayer, bool bValue);

	// ------------------------------------
	// # of times/points counters
	// ------------------------------------

	int GetNumCitiesLiberatedBy(PlayerTypes ePlayer) const;
	void SetNumCitiesLiberatedBy(PlayerTypes ePlayer, int iValue);
	void ChangeNumCitiesLiberatedBy(PlayerTypes ePlayer, int iChange);

	int GetNumCitiesEverLiberatedBy(PlayerTypes ePlayer) const;
	void SetNumCitiesEverLiberatedBy(PlayerTypes ePlayer, int iValue);
	void ChangeNumCitiesEverLiberatedBy(PlayerTypes ePlayer, int iChange);

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

	int GetNumTradeRoutesPlundered(PlayerTypes ePlayer) const;
	void SetNumTradeRoutesPlundered(PlayerTypes ePlayer, int iValue);
	void ChangeNumTradeRoutesPlundered(PlayerTypes ePlayer, int iChange);

	int GetNumWondersBeatenTo(PlayerTypes ePlayer) const;
	void SetNumWondersBeatenTo(PlayerTypes ePlayer, int iValue);
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

	int GetLikedTheirProposalValue(PlayerTypes ePlayer) const;
	void SetLikedTheirProposalValue(PlayerTypes ePlayer, int iValue);

	int GetSupportedOurProposalValue(PlayerTypes ePlayer) const;
	void SetSupportedOurProposalValue(PlayerTypes ePlayer, int iValue);
	bool IsSupportedOurProposalAndThenFoiledUs(PlayerTypes ePlayer) const;
	bool IsFoiledOurProposalAndThenSupportedUs(PlayerTypes ePlayer) const;

	int GetVotingHistoryScore(PlayerTypes ePlayer) const;
	void SetVotingHistoryScore(PlayerTypes ePlayer, int iValue);
	void ChangeVotingHistoryScore(PlayerTypes ePlayer, int iChange);

	int GetSupportedOurHostingValue(PlayerTypes ePlayer) const;
	void SetSupportedOurHostingValue(PlayerTypes ePlayer, int iValue);

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
	void SetLiberatedCitiesTurn(PlayerTypes ePlayer, int iTurn);
	bool IsCityRecentlyLiberatedBy(PlayerTypes ePlayer) const;

	int GetCiviliansReturnedToMeTurn(PlayerTypes ePlayer) const;
	void SetCiviliansReturnedToMeTurn(PlayerTypes ePlayer, int iTurn);

	int GetIntrigueSharedTurn(PlayerTypes ePlayer) const;
	void SetIntrigueSharedTurn(PlayerTypes ePlayer, int iTurn);

	bool IsPlayerForgaveForSpying(PlayerTypes ePlayer) const;
	void SetPlayerForgaveForSpying(PlayerTypes ePlayer, bool bValue);
	int GetForgaveForSpyingTurn(PlayerTypes ePlayer) const;
	void SetForgaveForSpyingTurn(PlayerTypes ePlayer, int iTurn);

	int GetLandmarksBuiltForMeTurn(PlayerTypes ePlayer) const;
	void SetLandmarksBuiltForMeTurn(PlayerTypes ePlayer, int iTurn);

	int GetPlottedAgainstUsTurn(PlayerTypes ePlayer) const;
	void SetPlottedAgainstUsTurn(PlayerTypes ePlayer, int iTurn);
	
	int GetPlunderedTradeRouteTurn(PlayerTypes ePlayer) const;
	void SetPlunderedTradeRouteTurn(PlayerTypes ePlayer, int iTurn);

	int GetBeatenToWonderTurn(PlayerTypes ePlayer) const;
	void SetBeatenToWonderTurn(PlayerTypes ePlayer, int iTurn);

	int GetLoweredOurInfluenceTurn(PlayerTypes ePlayer) const;
	void SetLoweredOurInfluenceTurn(PlayerTypes ePlayer, int iTurn);

	int GetOtherPlayerSidedWithProtectedMinorTurn(PlayerTypes ePlayer) const;
	void SetOtherPlayerSidedWithProtectedMinorTurn(PlayerTypes ePlayer, int iTurn);
	bool IsAngryAboutSidedWithProtectedMinor(PlayerTypes ePlayer) const;

	int GetOtherPlayerBulliedProtectedMinorTurn(PlayerTypes ePlayer) const;
	void SetOtherPlayerBulliedProtectedMinorTurn(PlayerTypes ePlayer, int iTurn);
	bool IsAngryAboutProtectedMinorBullied(PlayerTypes ePlayer) const;

	int GetOtherPlayerAttackedProtectedMinorTurn(PlayerTypes ePlayer) const;
	void SetOtherPlayerAttackedProtectedMinorTurn(PlayerTypes ePlayer, int iTurn);
	bool IsAngryAboutProtectedMinorAttacked(PlayerTypes ePlayer) const;

	int GetOtherPlayerKilledProtectedMinorTurn(PlayerTypes ePlayer) const;
	void SetOtherPlayerKilledProtectedMinorTurn(PlayerTypes ePlayer, int iTurn);
	bool IsAngryAboutProtectedMinorKilled(PlayerTypes ePlayer) const;

	int GetReligiousConversionTurn(PlayerTypes ePlayer) const;
	void SetReligiousConversionTurn(PlayerTypes ePlayer, int iTurn);

	int GetRobbedTurn(PlayerTypes ePlayer) const;
	void SetRobbedTurn(PlayerTypes ePlayer, int iTurn);

	int GetPerformedCoupTurn(PlayerTypes ePlayer) const;
	void SetPerformedCoupTurn(PlayerTypes ePlayer, int iTurn);

	int GetStoleArtifactTurn(PlayerTypes ePlayer) const;
	void SetStoleArtifactTurn(PlayerTypes ePlayer, int iTurn);

	int GetWeLikedTheirProposalTurn(PlayerTypes ePlayer) const;
	void SetWeLikedTheirProposalTurn(PlayerTypes ePlayer, int iTurn);
	bool WeLikedTheirProposal(PlayerTypes ePlayer) const;

	int GetWeDislikedTheirProposalTurn(PlayerTypes ePlayer) const;
	void SetWeDislikedTheirProposalTurn(PlayerTypes ePlayer, int iTurn);
	bool WeDislikedTheirProposal(PlayerTypes ePlayer) const;

	int GetTheySupportedOurProposalTurn(PlayerTypes ePlayer) const;
	void SetTheySupportedOurProposalTurn(PlayerTypes ePlayer, int iTurn);

	int GetTheyFoiledOurProposalTurn(PlayerTypes ePlayer) const;
	void SetTheyFoiledOurProposalTurn(PlayerTypes ePlayer, int iTurn);

	int GetTheySanctionedUsTurn(PlayerTypes ePlayer) const;
	void SetTheySanctionedUsTurn(PlayerTypes ePlayer, int iTurn);
	bool HasTriedToSanctionUs(PlayerTypes ePlayer) const;

	int GetTheyUnsanctionedUsTurn(PlayerTypes ePlayer) const;
	void SetTheyUnsanctionedUsTurn(PlayerTypes ePlayer, int iTurn);
	bool HasTriedToUnsanctionUs(PlayerTypes ePlayer) const;

	int GetTheySupportedOurHostingTurn(PlayerTypes ePlayer) const;
	void SetTheySupportedOurHostingTurn(PlayerTypes ePlayer, int iTurn);
	bool TheySupportedOurHosting(PlayerTypes ePlayer) const;

	// ------------------------------------
	// Player-Specific Memory Values
	// ------------------------------------

	bool WasHumanLastUpdate() const;

	bool HasEndedFriendshipThisTurn() const;
	void SetEndedFriendshipThisTurn(bool bValue);

	bool UpdatedWarProgressThisTurn() const;
	void SetUpdatedWarProgressThisTurn(bool bValue);

	int GetNumReevaluations() const;
	void SetNumReevaluations(int iValue);
	void ChangeNumReevaluations(int iChange);

	bool IsWaitingForDigChoice() const;
	void SetWaitingForDigChoice(bool bValue);

	bool IsAvoidDeals() const;
	void SetAvoidDeals(bool bValue);

	bool IsIgnoreWarmonger() const;
	void SetIgnoreWarmonger(bool bValue);

	PlayerTypes GetVassalPlayerToLiberate() const;
	void SetVassalPlayerToLiberate(PlayerTypes ePlayer);

	PlayerTypes GetOtherPlayerProtectedMinorBullied(PlayerTypes ePlayer) const;
	void SetOtherPlayerProtectedMinorBullied(PlayerTypes ePlayer, PlayerTypes eBulliedPlayer);

	PlayerTypes GetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer) const;
	void SetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer, PlayerTypes eAttackedPlayer);

	PlayerTypes GetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer) const;
	void SetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer, PlayerTypes eKilledPlayer);

	// ------------------------------------
	// Guesses
	// ------------------------------------

	// Guesses about other players' feelings towards us
	/*
	CivOpinionTypes GetOpinionTowardsUsGuess(PlayerTypes ePlayer) const;
	void SetOpinionTowardsUsGuess(PlayerTypes ePlayer, CivOpinionTypes eOpinion);
	CivApproachTypes GetApproachTowardsUsGuess(PlayerTypes ePlayer) const;
	void SetApproachTowardsUsGuess(PlayerTypes ePlayer, CivApproachTypes eApproach);
	int GetApproachTowardsUsGuessCounter(PlayerTypes ePlayer) const;
	void SetApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iValue);
	void ChangeApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iChange);
	*/

	// ------------------------------------
	// C4DF Values
	// ------------------------------------

	ShareOpinionResponseTypes GetShareOpinionResponse(PlayerTypes ePlayer) const;
	void SetShareOpinionResponse(PlayerTypes ePlayer, ShareOpinionResponseTypes eResponse);

	bool IsPlayerMoveTroopsRequestAccepted(PlayerTypes ePlayer) const;
	void SetPlayerMoveTroopsRequestAccepted(PlayerTypes ePlayer, bool bValue);

	bool IsTooSoonForMoveTroopsRequest(PlayerTypes ePlayer) const;

	bool IsOfferingGift(PlayerTypes ePlayer) const;	// We're offering a gift!
	void SetOfferingGift(PlayerTypes ePlayer, bool bValue);

	bool IsOfferedGift(PlayerTypes ePlayer) const;	// We offered a gift!
	void SetOfferedGift(PlayerTypes ePlayer, bool bValue);

	int GetHelpRequestAcceptedTurn(PlayerTypes ePlayer) const;
	void SetHelpRequestAcceptedTurn(PlayerTypes ePlayer, int iTurn);

	int GetHelpRequestTooSoonNumTurns(PlayerTypes ePlayer) const;
	void SetHelpRequestTooSoonNumTurns(PlayerTypes ePlayer, int iValue);
	bool IsHelpRequestTooSoon(PlayerTypes ePlayer) const;

	int GetMaxVassalProtectValue() const;
	int GetMaxVassalFailedProtectValue() const;
	int GetVassalProtectValue(PlayerTypes ePlayer) const;
	void SetVassalProtectValue(PlayerTypes ePlayer, int iValue);
	void ChangeVassalProtectValue(PlayerTypes ePlayer, int iChange, bool bDecay = false);

	bool IsMasterLiberatedMeFromVassalage(PlayerTypes ePlayer) const;
	void SetMasterLiberatedMeFromVassalage(PlayerTypes ePlayer, bool bValue);

	int GetVassalagePeacefullyRevokedTurn(PlayerTypes ePlayer) const;
	void SetVassalagePeacefullyRevokedTurn(PlayerTypes ePlayer, int iTurn);

	int GetVassalageForcefullyRevokedTurn(PlayerTypes ePlayer) const;
	void SetVassalageForcefullyRevokedTurn(PlayerTypes ePlayer, int iTurn);

	bool BrokeVassalAgreement(PlayerTypes ePlayer) const;
	void SetBrokeVassalAgreement(PlayerTypes ePlayer, bool bValue);

	int GetBrokeVassalAgreementTurn(PlayerTypes ePlayer) const;
	void SetBrokeVassalAgreementTurn(PlayerTypes ePlayer, int iTurn);

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

	void DoTurn(DiplomacyMode eDiploMode, PlayerTypes ePlayer=NO_PLAYER);

	// ------------------------------------
	// Test Backstabber Flag
	// ------------------------------------

	void TestBackstabberFlag();

	// ------------------------------------
	// Conquest Stats
	// ------------------------------------

	void DoUpdateConquestStats();

	// ------------------------------------
	// Coop Wars
	// ------------------------------------

	void DoUpdateCoopWarStates();
	bool CanStartCoopWar(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer);
	void DoStartCoopWar(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer);

	// ------------------------------------
	// Victory Competition
	// ------------------------------------

	void DoUpdateCompetingForVictory();
	bool IsEndgameAggressive() const;

	void DoUpdateCurrentVictoryPursuit();
	bool IsSeriousAboutVictory() const;
	void DoUpdateRecklessExpanders();
	void DoUpdateWonderSpammers();
	void DoUpdateVictoryDisputeLevels();
	void DoUpdateVictoryBlockLevels();
	void DoUpdateTechBlockLevels();
	void DoUpdatePolicyBlockLevels();

	// ------------------------------------
	// Military Stuff
	// ------------------------------------

	void DoUpdateWarStates();
	int GetWarScore(PlayerTypes ePlayer);
	int GetHighestWarscore();
	PlayerTypes GetHighestWarscorePlayer();

	void DoUpdatePlayerStrengthEstimates();
	int ComputeDynamicStrengthModifier(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer);

	void DoUpdateWarProgressScores();

	void DoUpdateWarmongerThreats(bool bUpdateOnly = false);

	int GetNumberOfThreatenedCities(PlayerTypes ePlayer);

	void DoUpdateEasyTargets();

	// ------------------------------------
	// Aggressive Postures
	// ------------------------------------

	int CountAggressiveMilitaryScore(PlayerTypes ePlayer, bool bHalveDefenders);
	void DoUpdateMilitaryAggressivePostures();

	void DoExpansionBickering();
	int GetExpansionBickerRange() const;

	void DoUpdatePlotBuyingAggressivePostures();

	// ------------------------------------
	// Dispute Levels
	// ------------------------------------

	void DoUpdateLandDisputeLevels();
	void DoUpdateWonderDisputeLevels();
	void DoUpdateMinorCivDisputeLevels();

	// ------------------------------------
	// Player Trustworthiness
	// ------------------------------------

	void TestBackstabbingPenalties();

	void TestUntrustworthyFriends();
	bool TestOnePlayerUntrustworthyFriend(PlayerTypes ePlayer);

	// ------------------------------------
	// War Sanity Checks
	// ------------------------------------

	void DoUpdateSaneDiplomaticTargets();

	bool IsWarWouldBankruptUs(PlayerTypes ePlayer, int iMinimumIncome = 0);
	int CalculateGoldPerTurnLostFromWar(PlayerTypes ePlayer);

	bool DoUpdateOnePlayerSaneDiplomaticTarget(PlayerTypes ePlayer, bool bImpulse);
	bool CanBackstab(PlayerTypes ePlayer) const;
	bool IsWillingToAttackFriend(PlayerTypes ePlayer, bool bDirect, bool bImpulse);

	// ------------------------------------
	// Opinion
	// ------------------------------------

	void DoUpdateOpinions();
	void DoUpdateOnePlayerOpinion(PlayerTypes ePlayer);
	int CalculateCivOpinionWeight(PlayerTypes ePlayer);
#if defined(MOD_ACTIVE_DIPLOMACY)
	void DoUpdateHumanTradePriority(PlayerTypes ePlayer, int iOpinionWeight); // <= JdH
#endif

	//void DoUpdateApproachTowardsUsGuesses();

	// ------------------------------------
	// Global Politics
	// ------------------------------------

	void DoUpdateGlobalPolitics();
	void DoReevaluatePlayer(PlayerTypes ePlayer, bool bMajorEvent = false, bool bCancelExchanges = true, bool bFromResurrection = false);
	void DoReevaluateEveryone(bool bMajorEvent = false, bool bCancelExchanges = true, bool bFromResurrection = false);
	void DoReevaluatePlayers(vector<PlayerTypes>& vTargetPlayers, bool bMajorEvent = false, bool bCancelExchanges = true, bool bFromResurrection = false);
	void DoUpdateMajorCompetitors();
	void DoUpdateMajorCivApproaches(vector<PlayerTypes>& vPlayersToReevaluate, bool bStrategic);

	// Priority approach updates
	void SelectAlwaysWarApproach(PlayerTypes ePlayer);
	void SelectHumanApproach(PlayerTypes ePlayer);
	void SelectApproachIfWeHaveNoCities(PlayerTypes ePlayer);
	void SelectApproachIfTheyHaveNoCities(PlayerTypes ePlayer);

	// Post approach updates
	void SelectApproachTowardsMaster(PlayerTypes ePlayer);
	void SelectApproachTowardsVassal(PlayerTypes ePlayer);

	// Main approach update function
	void SelectBestApproachTowardsMajorCiv(PlayerTypes ePlayer, bool bStrategic, vector<PlayerTypes>& vValidPlayers, vector<PlayerTypes>& vPlayersToReevaluate, std::map<PlayerTypes, CivApproachTypes>& oldApproaches);

	// Planning Exchanges
	void DoRelationshipPairing();
	void DoUpdatePlanningExchanges();
	void DoUpdatePrimeLeagueAlly();
	bool AvoidExchangesWithPlayer(PlayerTypes ePlayer, bool bWarOnly = false, bool bIgnoreSelfApproach = false) const;
	bool IsGoodChoiceForDoF(PlayerTypes ePlayer);
	bool IsGoodChoiceForDefensivePact(PlayerTypes ePlayer);
	bool IsGoodChoiceForResearchAgreement(PlayerTypes ePlayer);
	bool IsCanMakeResearchAgreementRightNow(PlayerTypes ePlayer);
	PlayerTypes GetHighestScoringDefensivePact(vector<PlayerTypes>& vAcceptableChoices, vector<PlayerTypes>& vPlayersToExclude);
	int ScoreDefensivePactChoice(PlayerTypes eChoice, bool bCoastal);

	// War!
	void DoUpdateWarTargets();

	// Minor Civ Approach
	void DoUpdateMinorCivApproaches();
	void SelectBestApproachTowardsMinorCiv(PlayerTypes ePlayer);

	// ------------------------------------
	// Peace Treaty Willingness
	// ------------------------------------

	void DoUpdatePeaceTreatyWillingness(bool bMyTurn = false);
	void DoUpdatePeaceTreatyOffers(vector<TeamTypes>& vMakePeaceTeams, bool bCriticalState);
	int GetComparativeDanger(PlayerTypes ePlayer, vector<PlayerTypes>& vOurWarAllies, vector<PlayerTypes>& vTheirWarAllies, int& iTheirDanger, bool& bSeriousDangerUs, bool& bSeriousDangerThem, vector<int>& vEnemyCitiesEndangered, vector<int>& vEnemyCitiesEndangeredByUs);
	bool IsWantsPeaceWithPlayer(PlayerTypes ePlayer) const;
	bool IsPeaceBlocked(PlayerTypes ePlayer) const;
	PeaceBlockReasons GetPeaceBlockReason(PlayerTypes ePlayer) const;
	void FmtPeaceBlockReasonLogStr(CvString& str, PlayerTypes eThisPlayer, PlayerTypes eAtWarPlayer, PeaceBlockReasons eReason);
	int CountUnitsAroundEnemyCities(PlayerTypes ePlayer, int iTurnRange) const;
	void RefusePeaceTreaty(PlayerTypes ePlayer, const CvString& strLogMessage);
	void LogPeaceWillingnessReason(PlayerTypes ePlayer, const CvString& strLogMessage);

	// ------------------------------------
	// Vassal Taxation
	// ------------------------------------

	void DetermineVassalTaxRates();
	void DoVassalTaxChanged(TeamTypes eMasterTeam, bool bTaxesLowered);

	// ------------------------------------
	// Demands
	// ------------------------------------

	void DoUpdateDemands();
	bool IsValidDemandTarget(PlayerTypes ePlayer, int& iDemandValueScore);
	int GetPlayerDemandValueScore(PlayerTypes ePlayer);

	// ------------------------------------
	// WAR!
	// ------------------------------------

	void MakeWar();
	void DoMakeWarOnPlayer(PlayerTypes eTargetPlayer);
	bool DeclareWar(PlayerTypes ePlayer);
	bool DeclareWar(TeamTypes eTeam);

	// ************************************
	// City-State Diplomacy
	// ************************************

	// ************************************
	// Diplomatic Interactions
	// ************************************

	// ************************************
	// Counters
	// ************************************

	void DoCounters();

	// ------------------------------------
	// Below this line things are not sorted...
	// ------------------------------------

	/////////////////////////////////////////////////////////
	// Approach
	/////////////////////////////////////////////////////////

	bool IsHasActiveGoldQuest();

	/////////////////////////////////////////////////////////
	// Requests
	/////////////////////////////////////////////////////////

	bool IsMakeRequest(PlayerTypes ePlayer, CvDeal* pDeal, bool& bRandPassed);
	bool IsLuxuryRequest(PlayerTypes ePlayer, CvDeal* pDeal, int& iWeightBias);
	bool IsGoldRequest(PlayerTypes ePlayer, CvDeal* pDeal, int& iWeightBias);

	bool IsEmbassyExchangeAcceptable(PlayerTypes ePlayer);
	bool WantsEmbassyAtPlayer(PlayerTypes ePlayer) const;
	bool IsWantsOpenBordersWithPlayer(PlayerTypes ePlayer);
	bool IsWillingToGiveOpenBordersToPlayer(PlayerTypes ePlayer);
	bool IsOpenBordersExchangeAcceptable(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Planning Exchanges
	/////////////////////////////////////////////////////////

	bool IsPhonyWar(PlayerTypes ePlayer, bool bIgnoreCurrentApproach = false) const;
	bool IsWantsToConquer(PlayerTypes ePlayer) const;
	bool IsPotentialMilitaryTargetOrThreat(PlayerTypes ePlayer, bool bFromApproachSelection) const;

	/////////////////////////////////////////////////////////
	// Issues of Dispute
	/////////////////////////////////////////////////////////

	bool IsNukedBy(PlayerTypes ePlayer) const;

	bool IsFriendOrAlly(PlayerTypes ePlayer) const;
	bool IsEarlyGameCompetitor(PlayerTypes ePlayer);

	bool IsIgnorePolicyDifferences(PlayerTypes ePlayer) const;
	bool IsIgnoreReligionDifferences(PlayerTypes ePlayer) const;
	bool IsIgnoreIdeologyDifferences(PlayerTypes ePlayer) const;

	bool IsMinorCivTroublemaker(PlayerTypes ePlayer, bool bIgnoreBullying = false) const;
	
	// Diplomacy AI Options
	bool ShouldHideDisputeMods(PlayerTypes ePlayer) const;
	bool ShouldHideNegativeMods(PlayerTypes ePlayer) const;

	/////////////////////////////////////////////////////////
	// Evaluation of Other Players' Tendencies
	/////////////////////////////////////////////////////////

	// Someone had some kind of interaction with another player
	void DoWeMadePeaceWithSomeone(TeamTypes eOtherTeam);
	void DoPlayerDeclaredWarOnSomeone(PlayerTypes ePlayer, TeamTypes eOtherTeam, bool bDefensivePact);
	void DoPlayerBulliedSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);

	int GetOtherPlayerWarmongerScore(PlayerTypes ePlayer) const;

	/////////////////////////////////////////////////////////
	// Contact
	/////////////////////////////////////////////////////////

	void DoFirstContact(PlayerTypes ePlayer);
	void DoFirstContactInitRelationship(PlayerTypes ePlayer);

	void DoKilledByPlayer(PlayerTypes ePlayer);

	void DoSendStatementToPlayer(PlayerTypes ePlayer, DiploStatementTypes eStatement, int iData1, CvDeal* pDeal);

	void DoContactMajorCivs();
	void DoContactPlayer(PlayerTypes ePlayer);

	void DoContactMinorCivs();

	// Possible contact options follow:

	void DoUpdateMinorCivProtection(PlayerTypes eMinor);

	//void DoCoopWarTimeStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);
	void DoCoopWarStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1);

	void DoMakeDemand(PlayerTypes ePlayer);

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
	void DoEndDoFStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement) const;
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
	CvDeal* DoRenewExpiredDeal(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

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

	bool IsActHostileTowardsHuman(PlayerTypes eHuman) const;

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
	bool IsValidCoopWarTarget(PlayerTypes eTargetPlayer, bool bAtWarException);
	bool CanRequestCoopWar(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer);

	bool DoTestCoopWarDesire(PlayerTypes eAllyPlayer, PlayerTypes& eChosenTargetPlayer);
	int GetCoopWarDesireScore(PlayerTypes eAllyPlayer, PlayerTypes eTargetPlayer, bool bAllyRequest);

	CoopWarStates RespondToCoopWarRequest(PlayerTypes eAskingPlayer, PlayerTypes eTargetPlayer);
	bool IsCoopWarRequestUnacceptable(PlayerTypes eAskingPlayer, PlayerTypes eTargetPlayer) const;
	void DoWarnCoopWarTarget(PlayerTypes eAskingPlayer, PlayerTypes eTargetPlayer);

	void CancelCoopWarsAgainstPlayer(PlayerTypes ePlayer, bool bNotify);
	void CancelCoopWarsWithPlayer(PlayerTypes ePlayer, bool bPenalty);
	void CancelAllCoopWars();

	// Human Demand
	void DoDemandMade(PlayerTypes ePlayer, DemandResponseTypes eResponse);

	// No Settling
	bool IsDontSettleAcceptable(PlayerTypes ePlayer);

	// Stop Spying
	bool IsStopSpyingAcceptable(PlayerTypes ePlayer);

	// Working With Player
	bool IsDoFAcceptable(PlayerTypes ePlayer);
	bool IsEndDoFAcceptable(PlayerTypes ePlayer, bool bIgnoreCurrentDoF = false);
	bool IsTooEarlyForDoF(PlayerTypes ePlayer);

	int GetNumRA() const;
	int GetNumDefensePacts() const;

	int GetNumSamePolicies(PlayerTypes ePlayer);

	bool IsDenounceFriendAcceptable(PlayerTypes ePlayer);

	bool IsPlayerDoFWithAnyFriend(PlayerTypes ePlayer) const;
	bool IsPlayerDoFWithAnyEnemy(PlayerTypes ePlayer) const;
	bool IsPlayerDPWithAnyFriend(PlayerTypes ePlayer) const;
	bool IsPlayerDPWithAnyEnemy(PlayerTypes ePlayer) const;
	int GetNumMutualFriends(PlayerTypes ePlayer) const;
	int GetNumEnemyFriends(PlayerTypes ePlayer) const;
	int GetNumMutualDefensePacts(PlayerTypes ePlayer) const;
	int GetNumEnemyDefensePacts(PlayerTypes ePlayer) const;

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
	int GetNumFriendsDenounced(PlayerTypes ePlayer) const;
	int GetNumEnemiesDenounced(PlayerTypes ePlayer) const;

	// Requests of Friends
	PlayerTypes GetRequestFriendToDenounce(PlayerTypes ePlayer, bool& bRandFailed);
	bool IsFriendDenounceRefusalUnacceptable(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer);

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
	VassalTreatmentTypes GetVassalTreatmentLevel(PlayerTypes ePlayer);
	CvString GetVassalTreatmentToolTip(PlayerTypes ePlayer);

	void DetermineVassalToLiberate();
	bool IsWantToLiberateVassal(PlayerTypes ePlayer, int& iScoreForLiberate) const;

	bool IsVassalageAcceptable(PlayerTypes ePlayer, bool bMasterEvaluation); // can be called in either direction, for the master or the vassal
	bool IsCapitulationAcceptable(PlayerTypes ePlayer); // vassal only
	bool IsVoluntaryVassalageAcceptable(PlayerTypes ePlayer); // vassal only
	bool IsVoluntaryVassalageRequestAcceptable(PlayerTypes ePlayer); // master only, evaluates all players on the potential vassal's team

	bool IsEndVassalageAcceptable(PlayerTypes ePlayer); // can be called in either direction, for the master or the vassal
	bool IsEndVassalageWithPlayerAcceptable(PlayerTypes ePlayer); // vassal only, evaluates one master
	bool IsEndVassalageRequestAcceptable(PlayerTypes ePlayer); // master only, evaluates one vassal

	void DoBecomeVassalageStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoMakeVassalageStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);
	void DoEndVassalageStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoRevokeVassalageStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal);

	void DoLiberateMyVassalStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

	void DoVassalTaxesRaisedStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);
	void DoVassalTaxesLoweredStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement);

	bool IsHappyAboutPlayerVassalagePeacefullyRevoked(PlayerTypes ePlayer);
	bool IsAngryAboutPlayerVassalageForcefullyRevoked(PlayerTypes ePlayer);

	void DoWeMadeVassalageWithSomeone(TeamTypes eMasterTeam, bool bVoluntary);
	void DoWeEndedVassalageWithSomeone(TeamTypes eTeam);

	MoveTroopsResponseTypes GetMoveTroopsRequestResponse(PlayerTypes ePlayer, bool bJustChecking = false);
	void DoLiberatedFromVassalage(TeamTypes eTeam, bool bSkipPopup);

	// Player asks the AI not to convert
	bool IsStopSpreadingReligionAcceptable(PlayerTypes ePlayer);

	// Player asks the AI not to dig
	bool IsStopDiggingAcceptable(PlayerTypes ePlayer);

	CivOpinionTypes GetNeighborOpinion(PlayerTypes ePlayer) const;
	bool MusteringForNeighborAttack(PlayerTypes ePlayer) const;

	/////////////////////////////////////////////////////////
	// Opinion modifiers
	/////////////////////////////////////////////////////////

	int AdjustModifierDuration(int iDuration, int iFlavorValue, bool bInvertModifier = false, bool bGamespeed = true) const;
	int AdjustTimedModifier(int iValue, int iDuration, int iTurn, TimedModifierTypes eModifierType, int iStacks = 1, int iFirstStackValue = 0);
	int AdjustConditionalModifier(int iValue, int iFlavorValue, bool bInvertModifier = false) const;
	void TestOpinionModifiers();
	int GetBaseOpinionScore(PlayerTypes ePlayer);

	// Dispute Levels
	int GetLandDisputeLevelScore(PlayerTypes ePlayer);
	int GetWonderDisputeLevelScore(PlayerTypes ePlayer);
	int GetMinorCivDisputeLevelScore(PlayerTypes ePlayer);
	int GetTechBlockLevelScore(PlayerTypes ePlayer);
	int GetPolicyBlockLevelScore(PlayerTypes ePlayer);
	int GetVictoryDisputeLevelScore(PlayerTypes ePlayer) const;
	int GetVictoryBlockLevelScore(PlayerTypes ePlayer) const;
	int GetRecklessExpanderScore(PlayerTypes ePlayer);
	int GetWonderSpammerScore(PlayerTypes ePlayer);

	// War Stuff
	int GetMilitaryAggressivePostureScore(PlayerTypes ePlayer);
	int GetWarmongerThreatScore(PlayerTypes ePlayer);
	int GetTradeRoutesPlunderedScore(PlayerTypes ePlayer);
	int GetCivilianKillerScore(PlayerTypes ePlayer) const;
	int GetCivilianKillerGlobalScore(PlayerTypes ePlayer);
	int GetNukedByScore(PlayerTypes ePlayer) const;
	int GetHolyCityCapturedByScore(PlayerTypes ePlayer);
	int GetCapitalCapturedByScore(PlayerTypes ePlayer);

	// Player has done nice stuff
	int GetRecentTradeScore(PlayerTypes ePlayer) const;
	int GetRecentAssistScore(PlayerTypes ePlayer) const;
	int GetCommonFoeScore(PlayerTypes ePlayer) const;
	int GetCiviliansReturnedToMeScore(PlayerTypes ePlayer);
	int GetLandmarksBuiltForMeScore(PlayerTypes ePlayer);
	int GetResurrectedScore(PlayerTypes ePlayer) const;
	int GetLiberatedCapitalScore(PlayerTypes ePlayer);
	int GetLiberatedHolyCityScore(PlayerTypes ePlayer);
	int GetLiberatedCitiesScore(PlayerTypes ePlayer);
	int GetReturnedCapitalScore(PlayerTypes ePlayer);
	int GetReturnedHolyCityScore(PlayerTypes ePlayer);
	int GetEmbassyScore(PlayerTypes ePlayer);
	int GetDiplomatScore(PlayerTypes ePlayer);
	int GetForgaveForSpyingScore(PlayerTypes ePlayer);
	int GetTimesIntrigueSharedScore(PlayerTypes ePlayer);

	// Player has done mean stuff
	int GetTimesCultureBombedScore(PlayerTypes ePlayer) const;
	int GetTimesRobbedScore(PlayerTypes ePlayer);
	int GetTimesPlottedAgainstUsScore(PlayerTypes ePlayer);
	int GetTimesPerformedCoupScore(PlayerTypes ePlayer);
	int GetStoleArtifactsScore(PlayerTypes ePlayer);

	// Player has asked us to do things we don't like
	int GetNoSettleRequestScore(PlayerTypes ePlayer);
	int GetStopSpyingRequestScore(PlayerTypes ePlayer);
	int GetDemandMadeScore(PlayerTypes ePlayer);

	// Denouncing
	int GetDenouncedScore(PlayerTypes ePlayer) const;
	int GetDenouncedFriendScore(PlayerTypes ePlayer) const;
	int GetDenouncedEnemyScore(PlayerTypes ePlayer) const;
	int GetDenouncedByOurFriendScore(PlayerTypes ePlayer) const;

	// Promises
	int GetBrokenMilitaryPromiseScore(PlayerTypes ePlayer) const;
	int GetBrokenMilitaryPromiseWithAnybodyScore(PlayerTypes ePlayer);
	int GetIgnoredMilitaryPromiseScore(PlayerTypes ePlayer);
	int GetBrokenExpansionPromiseScore(PlayerTypes ePlayer);
	int GetIgnoredExpansionPromiseScore(PlayerTypes ePlayer);
	int GetBrokenBorderPromiseScore(PlayerTypes ePlayer);
	int GetIgnoredBorderPromiseScore(PlayerTypes ePlayer);
	int GetBrokenAttackCityStatePromiseScore(PlayerTypes ePlayer) const;
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

	// Religion / Ideology
	int GetPolicyScore(PlayerTypes ePlayer);
	int GetReligiousConversionPointsScore(PlayerTypes ePlayer);
	int GetReligionScore(PlayerTypes ePlayer);
	int GetIdeologyScore(PlayerTypes ePlayer);

	// Protected Minors
	int GetPtPSameCSScore(PlayerTypes ePlayer);
	int GetAngryAboutProtectedMinorKilledScore(PlayerTypes ePlayer);
	int GetAngryAboutProtectedMinorAttackedScore(PlayerTypes ePlayer);
	int GetAngryAboutProtectedMinorBulliedScore(PlayerTypes ePlayer);
	int GetAngryAboutSidedWithProtectedMinorScore(PlayerTypes ePlayer);

	// Declaration of Friendship
	int GetDOFAcceptedScore(PlayerTypes ePlayer) const;
	int GetDOFWithAnyFriendScore(PlayerTypes ePlayer) const;
	int GetDOFWithAnyEnemyScore(PlayerTypes ePlayer) const;

	// Trade Agreements
	int GetDPAcceptedScore(PlayerTypes ePlayer) const;
	int GetDPWithAnyFriendScore(PlayerTypes ePlayer) const;
	int GetDPWithAnyEnemyScore(PlayerTypes ePlayer) const;
	int GetOpenBordersScore(PlayerTypes ePlayer);
	int GetResearchAgreementScore(PlayerTypes ePlayer);

	// Traitor Opinion
	int GetFriendDenouncementScore(PlayerTypes ePlayer);
	int GetPlayerDenouncedFriendScore(PlayerTypes ePlayer);
	int GetFriendDenouncedUsScore(PlayerTypes ePlayer) const;
	int GetPlayerAttackedVassalScore(PlayerTypes ePlayer);
	int GetMasterAttackedUsScore(PlayerTypes ePlayer) const;
	int GetPlayerAttackedFriendScore(PlayerTypes ePlayer);
	int GetFriendAttackedUsScore(PlayerTypes ePlayer) const;
	int GetPlayerAttackedResurrectedCivScore(PlayerTypes ePlayer);
	int GetResurrectorAttackedUsScore(PlayerTypes ePlayer) const;

	// World Congress
	int GetVotingHistoryOpinionScore(PlayerTypes ePlayer);
	int GetLikedTheirProposalScore(PlayerTypes ePlayer);
	int GetDislikedTheirProposalScore(PlayerTypes ePlayer);
	int GetSupportedOurProposalScore(PlayerTypes ePlayer);
	int GetSupportedMyHostingScore(PlayerTypes ePlayer);
	int GetSanctionedUsScore(PlayerTypes ePlayer);

	// Vassalage
	int GetVassalScore(PlayerTypes ePlayer) const;
	int GetVassalTreatedScore(PlayerTypes ePlayer);
	int GetVassalDemandScore(PlayerTypes ePlayer);
	int GetVassalTheftScore(PlayerTypes ePlayer);
	int GetVassalDenouncementScore(PlayerTypes ePlayer) const;
	int GetVassalTaxScore(PlayerTypes ePlayer);
	int GetVassalProtectScore(PlayerTypes ePlayer) const;
	int GetVassalTradeRouteScore(PlayerTypes ePlayer);
	int GetVassalOpenBordersScore(PlayerTypes ePlayer) const;
	int GetVassalReligionScore(PlayerTypes ePlayer);
	int GetMasterScore(PlayerTypes ePlayer) const;
	int GetTooManyVassalsScore(PlayerTypes ePlayer) const;
	int GetSameMasterScore(PlayerTypes ePlayer);
	int GetMasterLiberatedMeFromVassalageScore(PlayerTypes ePlayer) const;
	int GetHappyAboutVassalagePeacefullyRevokedScore(PlayerTypes ePlayer);
	int GetAngryAboutVassalageForcefullyRevokedScore(PlayerTypes ePlayer);

	// Modmod Modifiers
	int GetDiploModifiers(PlayerTypes ePlayer, std::vector<Opinion>& aOpinions);
	int GetScenarioModifier1(PlayerTypes ePlayer);
	int GetScenarioModifier2(PlayerTypes ePlayer);
	int GetScenarioModifier3(PlayerTypes ePlayer);

	/////////////////////////////////////////////////////////
	// Miscellaneous
	/////////////////////////////////////////////////////////

	bool IsTryingToLiberate(CvCity* pCity);
	bool DoPossibleMajorLiberation(CvCity* pCity);

	bool IsBadTheftTarget(PlayerTypes ePlayer, TheftTypes eTheftType, const CvPlot* pPlot = NULL);

	int GetNumOurEnemiesPlayerAtWarWith(PlayerTypes ePlayer) const;

	// Minor Civ Log
	void LogMinorCivGiftTile(PlayerTypes eMinor);
	void LogMinorCivGiftGold(PlayerTypes eMinor, int iOldFriendship, int iGold, bool bSaving, bool bWantQuickBoost, PlayerTypes ePlayerTryingToPass);
	void LogMinorCivBullyGold(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, int iGold, bool bSuccess, int iBullyMetricScore);
	void LogMinorCivBullyUnit(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, UnitTypes eUnit, bool bSuccess, int iBullyMetricScore);
	void LogMinorCivBullyHeavy(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, YieldTypes eYield, int iValue, bool bSuccess, int iBullyMetricScore);
	void LogMinorCivQuestReceived(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType);
	void LogMinorCivQuestFinished(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType);
	void LogMinorCivQuestCancelled(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType);
	void LogMinorCivBuyout(PlayerTypes eMinor, int iGoldPaid, bool bSaving);
	void LogMinorStatusChange(PlayerTypes eMinor, const char* msg);

	std::vector<CvDeal*> GetDealsToRenew(PlayerTypes eOtherPlayer = NO_PLAYER, bool bOnlyCheckedDeals = false);
	void CancelRenewDeal(PlayerTypes eOtherPlayer = NO_PLAYER, RenewalReason eReason = NO_REASON, bool bJustLogging = false, CvDeal* pPassDeal = NULL, bool bOnlyCheckedDeals = false, bool bSendNetworkMessage = true);

	// Methods for injecting tests
	void TestUIDiploStatement(PlayerTypes eToPlayer, DiploStatementTypes eStatement, int iArg1);

	void LogOpenEmbassy(PlayerTypes ePlayer);
	void LogCloseEmbassy(PlayerTypes ePlayer);
private:
	/// Helper functions to return the Player and Team IDs more conveniently
	inline PlayerTypes GetID() const { return (PlayerTypes)m_eID; }
	inline TeamTypes GetTeam() const { return (TeamTypes)m_eTeam; }
	inline bool NotMe(PlayerTypes eOtherPlayer) const { return eOtherPlayer != (PlayerTypes)m_eID; }
	inline bool NotTeam(PlayerTypes eOtherPlayer) const { return eOtherPlayer == NO_PLAYER || GET_PLAYER(eOtherPlayer).getTeam() != (TeamTypes)m_eTeam; }

	// Estimations of other players' tendencies
	int EstimateVictoryCompetitiveness(PlayerTypes ePlayer) const;
	int EstimateWonderCompetitiveness(PlayerTypes ePlayer) const;
	int EstimateMinorCivCompetitiveness(PlayerTypes ePlayer) const;
	int EstimateBoldness(PlayerTypes ePlayer) const;
	int EstimateDiploBalance(PlayerTypes ePlayer) const;
	int EstimateWarmongerHate(PlayerTypes ePlayer) const;
	int EstimateDoFWillingness(PlayerTypes ePlayer) const;
	int EstimateDenounceWillingness(PlayerTypes ePlayer) const;
	int EstimateWorkWithWillingness(PlayerTypes ePlayer) const;
	int EstimateWorkAgainstWillingness(PlayerTypes ePlayer) const;
	int EstimateLoyalty(PlayerTypes ePlayer) const;
	int EstimateForgiveness(PlayerTypes ePlayer) const;
	int EstimateNeediness(PlayerTypes ePlayer) const;
	int EstimateMeanness(PlayerTypes ePlayer) const;
	int EstimateChattiness(PlayerTypes ePlayer) const;
	int EstimateMajorCivApproachBias(PlayerTypes ePlayer, CivApproachTypes eApproach) const;
	int EstimateMinorCivApproachBias(PlayerTypes ePlayer, CivApproachTypes eApproach) const;
	int EstimateFlavorValue(PlayerTypes ePlayer, FlavorTypes eFlavor) const;

	bool IsValidUIDiplomacyTarget(PlayerTypes eTargetPlayer);

	void LogWarDeclaration(PlayerTypes ePlayer, int iTotalWarWeight = -1);
	void LogPeaceMade(PlayerTypes ePlayer);
	void LogDoF(PlayerTypes ePlayer);
	void LogBrokenDoF(PlayerTypes ePlayer);
	void LogDenounce(PlayerTypes ePlayer, bool bBackstab = false, bool bRefusal = false);
	void LogFriendRequestDenounce(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bAgreed);
	void LogCoopWar(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, CoopWarStates eState);
	void LogWantRA(PlayerTypes ePlayer);
	void LogWantDP(PlayerTypes ePlayer);

	void LogApproachValueDeltas(PlayerTypes ePlayer, const int* aiApproachValues, const int* aiScratchValues);
	void LogMajorCivWarmongerUpdate(PlayerTypes ePlayer, int iValue, bool bUpdateLogsSpecial);

	void LogMajorCivApproachUpdate(PlayerTypes ePlayer, const int* aiApproachValues, CivApproachTypes eNewMajorCivApproach, CivApproachTypes eOldApproach, CivApproachTypes eSurfaceApproach);
	void LogMinorCivApproachUpdate(PlayerTypes ePlayer, const int* aiApproachValues, CivApproachTypes eNewMinorCivApproach, CivApproachTypes eOldApproach);
	void LogPersonality();
	void LogStatus();
	void LogWarStatus();

	void LogGrandStrategy(CvString& strString);

	void LogMajorCivApproach(CvString& strString, CivApproachTypes eNewMajorCivApproach, CivApproachTypes eSurfaceApproach);
	void LogMinorCivApproach(CvString& strString, CivApproachTypes eNewMinorCivApproach);
	void LogMinorCivQuestType(CvString& strString, MinorCivQuestTypes eQuestType);
	void LogOpinion(CvString& strString, PlayerTypes ePlayer) const;
	void LogWarmongerThreat(CvString& strString, PlayerTypes ePlayer);
	void LogMilitaryStrength(CvString& strString, PlayerTypes ePlayer) const;
	void LogTargetValue(CvString& strString, PlayerTypes ePlayer) const;
	void LogEconomicStrength(CvString& strString, PlayerTypes ePlayer) const;
	void LogWarPeaceWillingToOffer(CvString& strString, PlayerTypes ePlayer) const;
	void LogWarPeaceWillingToAccept(CvString& strString, PlayerTypes ePlayer) const;
	void LogWarState(CvString& strString, PlayerTypes ePlayer);
	void LogMilitaryAggressivePosture(CvString& strString, PlayerTypes ePlayer) const;
	void LogPlotBuyingAggressivePosture(CvString& strString, PlayerTypes ePlayer) const;
	void LogLandDispute(CvString& strString, PlayerTypes ePlayer) const;
	void LogVictoryDispute(CvString& strString, PlayerTypes ePlayer) const;
	void LogVictoryBlock(CvString& strString, PlayerTypes ePlayer) const;
	void LogWonderDispute(CvString& strString, PlayerTypes ePlayer) const;
	void LogMinorCivDispute(CvString& strString, PlayerTypes ePlayer) const;
	void LogProximity(CvString& strString, PlayerTypes ePlayer);

	void LogStatementToPlayer(PlayerTypes ePlayer, DiploStatementTypes eMessage);

	// ************************************
	// Memory Values
	// ************************************

	CvPlayer* m_pPlayer;
	unsigned char m_eID;
	unsigned char m_eTeam;

	// Need a string member so that it doesn't go out of scope after translation
	Localization::String m_strDiploText;

	// Personality Values
	unsigned char m_iVictoryCompetitiveness;
	unsigned char m_iWonderCompetitiveness;
	unsigned char m_iMinorCivCompetitiveness;
	unsigned char m_iBoldness;
	unsigned char m_iDiploBalance;
	unsigned char m_iWarmongerHate;
	unsigned char m_iDoFWillingness;
	unsigned char m_iDenounceWillingness;
	unsigned char m_iWorkWithWillingness;
	unsigned char m_iWorkAgainstWillingness;
	unsigned char m_iLoyalty;
	unsigned char m_iForgiveness;
	unsigned char m_iNeediness;
	unsigned char m_iMeanness;
	unsigned char m_iChattiness;
	unsigned char m_aiMajorCivApproachBiases[NUM_CIV_APPROACHES];
	unsigned char m_iMinorCivWarBias;
	unsigned char m_iMinorCivHostileBias;
	unsigned char m_iMinorCivNeutralBias;
	unsigned char m_iMinorCivFriendlyBias;

	// Key Players
	PlayerTypes m_eMostValuableFriend;
	PlayerTypes m_eMostValuableAlly;
	PlayerTypes m_eBiggestCompetitor;
	PlayerTypes m_ePrimeLeagueAlly;
	PlayerTypes m_ePrimeLeagueCompetitor;
	PlayerTypes m_eDemandTargetPlayer;
	PlayerTypes m_eCSWarTarget;
	PlayerTypes m_eCSBullyTarget;

	// Other Global Memory
	bool m_bAvoidDeals; // Not serialized!
	bool m_bIgnoreWarmonger; // Not serialized!
	PlayerTypes m_eVassalPlayerToLiberate; // Not serialized!
	bool m_bWasHumanLastUpdate;
	bool m_bEndedFriendshipThisTurn;
	bool m_bUpdatedWarProgressThisTurn;
	int m_iNumReevaluations; // Used for RNG
	bool m_bWaitingForDigChoice;
	bool m_bBackstabber;
	bool m_bCompetingForVictory;
	VictoryPursuitTypes m_ePrimaryVictoryPursuit;
	VictoryPursuitTypes m_eSecondaryVictoryPursuit;
	VictoryPursuitTypes m_eCurrentVictoryPursuit;
	StateAllWars m_eStateAllWars;

	// Diplomatic Interactions
	int m_aaiTurnStatementLastSent[MAX_MAJOR_CIVS][NUM_DIPLO_STATEMENT_TYPES];
	bool m_aabSentAttackMessageToMinorCivProtector[MAX_MAJOR_CIVS][MAX_MINOR_CIVS];

	// Opinion & Approach
	char m_aeCivOpinion[MAX_MAJOR_CIVS];
	short m_aiCachedOpinionWeight[MAX_MAJOR_CIVS];
	char m_aeCivApproach[MAX_CIV_PLAYERS];
	char m_aeCivStrategicApproach[MAX_MAJOR_CIVS];
	char m_aeCachedSurfaceApproach[MAX_MAJOR_CIVS];
	int m_aaiApproachValues[MAX_MAJOR_CIVS][NUM_CIV_APPROACHES];
	int m_aaiStrategicApproachValues[MAX_MAJOR_CIVS][NUM_CIV_APPROACHES];

	// Minor Civs
	bool m_abWantToRouteToMinor[MAX_MINOR_CIVS];

	// Planning Exchanges
	bool m_abMajorCompetitor[MAX_MAJOR_CIVS];
	bool m_abStrategicTradePartner[MAX_MAJOR_CIVS];
	bool m_abWantsDoFWithPlayer[MAX_MAJOR_CIVS];
	bool m_abWantsDefensivePactWithPlayer[MAX_MAJOR_CIVS];
	bool m_abWantsToEndDoFWithPlayer[MAX_MAJOR_CIVS];
	bool m_abWantsToEndDefensivePactWithPlayer[MAX_MAJOR_CIVS];
	bool m_abWantsResearchAgreementWithPlayer[MAX_MAJOR_CIVS];

	// Exchanges
	int m_aiDoFAcceptedTurn[MAX_MAJOR_CIVS];
	char m_aeDoFType[MAX_MAJOR_CIVS];
	int m_aiDenouncedPlayerTurn[MAX_MAJOR_CIVS];
	bool m_abCantMatchDeal[MAX_MAJOR_CIVS];
	unsigned char m_aiNumDemandsMade[MAX_MAJOR_CIVS];
	int m_aiDemandMadeTurn[MAX_MAJOR_CIVS];
	char m_aiDemandTooSoonNumTurns[MAX_MAJOR_CIVS];
	unsigned char m_aiNumConsecutiveDemandsTheyAccepted[MAX_MAJOR_CIVS];
	int m_aiDemandAcceptedTurn[MAX_MAJOR_CIVS];
	unsigned short m_aiTradeValue[MAX_MAJOR_CIVS];
	unsigned short m_aiCommonFoeValue[MAX_MAJOR_CIVS];
	short m_aiAssistValue[MAX_MAJOR_CIVS];

	// Coop Wars
	char m_aaeCoopWarState[MAX_MAJOR_CIVS][MAX_MAJOR_CIVS];
	int m_aaiCoopWarStateChangeTurn[MAX_MAJOR_CIVS][MAX_MAJOR_CIVS];
	char m_aiCoopWarAgreementScore[MAX_MAJOR_CIVS];

	// War
	bool m_abSaneDiplomaticTarget[MAX_CIV_PLAYERS];
	bool m_abPotentialWarTarget[MAX_CIV_PLAYERS];
	bool m_abArmyInPlaceForAttack[MAX_CIV_PLAYERS];
	bool m_abAggressor[MAX_CIV_PLAYERS];
	unsigned char m_aiNumWarsFought[MAX_CIV_PLAYERS];
	unsigned char m_aiNumWarsDeclaredOnUs[MAX_MAJOR_CIVS];
	unsigned short m_aiCivilianKillerValue[MAX_MAJOR_CIVS];
	unsigned char m_aiNumCitiesCaptured[MAX_PLAYERS];
	char m_aeWarState[MAX_CIV_PLAYERS];
	short m_aiWarProgressScore[MAX_CIV_PLAYERS];

	// Peace
	char m_aePeaceTreatyWillingToOffer[MAX_MAJOR_CIVS];
	char m_aePeaceTreatyWillingToAccept[MAX_MAJOR_CIVS];

	// Backstabbing Penalties
	bool m_abUntrustworthyFriend[MAX_MAJOR_CIVS];
	bool m_abEverBackstabbedBy[MAX_MAJOR_CIVS];
	int m_aiDoFBrokenTurn[MAX_MAJOR_CIVS];
	int m_aiFriendDenouncedUsTurn[MAX_MAJOR_CIVS];
	int m_aiFriendDeclaredWarOnUsTurn[MAX_MAJOR_CIVS];

	// Warmongering Penalties
	unsigned char m_aiNumMinorsAttacked[MAX_MAJOR_CIVS];
	unsigned char m_aiNumMinorsConquered[MAX_MAJOR_CIVS];
	unsigned char m_aiNumMajorsAttacked[MAX_MAJOR_CIVS];
	unsigned char m_aiNumMajorsConquered[MAX_MAJOR_CIVS];
	int m_aiWarmongerAmountTimes100[MAX_MAJOR_CIVS];

	// Aggressive Postures
	char m_aeMilitaryAggressivePosture[MAX_CIV_PLAYERS];
	char m_aePlotBuyingAggressivePosture[MAX_MAJOR_CIVS];

	// Dispute Levels
	bool m_abEndgameAggressiveTo[MAX_MAJOR_CIVS];
	bool m_abRecklessExpander[MAX_MAJOR_CIVS];
	bool m_abWonderSpammer[MAX_MAJOR_CIVS];
	char m_aeLandDisputeLevel[MAX_CIV_PLAYERS];
	char m_aeVictoryDisputeLevel[MAX_MAJOR_CIVS];
	char m_aeVictoryBlockLevel[MAX_MAJOR_CIVS];
	char m_aeWonderDisputeLevel[MAX_MAJOR_CIVS];
	char m_aeMinorCivDisputeLevel[MAX_MAJOR_CIVS];
	char m_aeTechBlockLevel[MAX_MAJOR_CIVS];
	char m_aePolicyBlockLevel[MAX_MAJOR_CIVS];

	// Threat Levels
	char m_aeWarmongerThreat[MAX_MAJOR_CIVS];

	// Strength Assessments
	char m_aeEconomicStrengthComparedToUs[MAX_CIV_PLAYERS];
	char m_aeMilitaryStrengthComparedToUs[MAX_CIV_PLAYERS];
	char m_aeRawMilitaryStrengthComparedToUs[MAX_CIV_PLAYERS];
	char m_aeTargetValue[MAX_CIV_PLAYERS];
	char m_aeRawTargetValue[MAX_CIV_PLAYERS];
	bool m_abEasyTarget[MAX_CIV_PLAYERS];

	// PROMISES
	// Military Promise
	char m_aeMilitaryPromiseState[MAX_MAJOR_CIVS];
	int m_aiMilitaryPromiseTurn[MAX_MAJOR_CIVS];

	// Expansion Promise
	char m_aeExpansionPromiseState[MAX_MAJOR_CIVS];
	int m_aiExpansionPromiseTurn[MAX_MAJOR_CIVS];
	bool m_abAngryAboutExpansion[MAX_MAJOR_CIVS];
	bool m_abEverRequestedExpansionPromise[MAX_MAJOR_CIVS];

	// Border Promise
	char m_aeBorderPromiseState[MAX_MAJOR_CIVS];
	char m_aeBorderPromisePosture[MAX_MAJOR_CIVS];
	int m_aiBorderPromiseTurn[MAX_MAJOR_CIVS];
	bool m_abEverMadeBorderPromise[MAX_MAJOR_CIVS];

	// Bully City-State Promise
	char m_aeBullyCityStatePromiseState[MAX_MAJOR_CIVS];
	int m_aiBullyCityStatePromiseTurn[MAX_MAJOR_CIVS];

	// Attack City-State Promise
	char m_aeAttackCityStatePromiseState[MAX_MAJOR_CIVS];
	int m_aiAttackCityStatePromiseTurn[MAX_MAJOR_CIVS];

	// Spy Promise
	char m_aeSpyPromiseState[MAX_MAJOR_CIVS];
	int m_aiSpyPromiseTurn[MAX_MAJOR_CIVS];

	// No Convert Promise
	char m_aeNoConvertPromiseState[MAX_MAJOR_CIVS];
	int m_aiNoConvertPromiseTurn[MAX_MAJOR_CIVS];
	bool m_abAskedNotToConvert[MAX_MAJOR_CIVS];
	bool m_abEverConvertedCity[MAX_MAJOR_CIVS];

	// No Digging Promise
	char m_aeNoDiggingPromiseState[MAX_MAJOR_CIVS];
	int m_aiNoDiggingPromiseTurn[MAX_MAJOR_CIVS];
	bool m_abAskedNotToDig[MAX_MAJOR_CIVS];

	// Coop War Promise
	int m_aiBrokenCoopWarPromiseTurn[MAX_MAJOR_CIVS];

	// END PROMISES
	// Event Flags
	bool m_abReturnedCapital[MAX_MAJOR_CIVS];
	bool m_abReturnedHolyCity[MAX_MAJOR_CIVS];
	bool m_abLiberatedCapital[MAX_MAJOR_CIVS];
	bool m_abLiberatedHolyCity[MAX_MAJOR_CIVS];
	bool m_abCapturedCapital[MAX_MAJOR_CIVS];
	bool m_abCapturedHolyCity[MAX_MAJOR_CIVS];
	bool m_abEverCapturedCapital[MAX_MAJOR_CIVS];
	bool m_abEverCapturedHolyCity[MAX_MAJOR_CIVS];
	bool m_abResurrectorAttackedUs[MAX_MAJOR_CIVS];
	bool m_abEverSanctionedUs[MAX_MAJOR_CIVS];
	bool m_abEverUnsanctionedUs[MAX_MAJOR_CIVS];

	// # of times/points counters
	unsigned char m_aiNumCitiesLiberated[MAX_MAJOR_CIVS];
	unsigned char m_aiNumCitiesEverLiberated[MAX_MAJOR_CIVS];
	unsigned char m_aiNumCiviliansReturnedToMe[MAX_MAJOR_CIVS];
	unsigned char m_aiNumTimesIntrigueSharedBy[MAX_MAJOR_CIVS];
	unsigned char m_aiNumLandmarksBuiltForMe[MAX_MAJOR_CIVS];
	unsigned char m_aiTheyPlottedAgainstUs[MAX_MAJOR_CIVS];
	unsigned char m_aiNumTradeRoutesPlundered[MAX_MAJOR_CIVS];
	unsigned char m_aiNumWondersBeatenTo[MAX_MAJOR_CIVS];
	unsigned char m_aiNumTimesCultureBombed[MAX_MAJOR_CIVS];
	unsigned char m_aiTheyLoweredOurInfluence[MAX_MAJOR_CIVS];
	unsigned char m_aiNumProtectedMinorsBullied[MAX_MAJOR_CIVS];
	unsigned char m_aiNumProtectedMinorsAttacked[MAX_MAJOR_CIVS];
	unsigned char m_aiNumProtectedMinorsKilled[MAX_MAJOR_CIVS];
	unsigned short m_aiNegativeReligiousConversionPoints[MAX_MAJOR_CIVS];
	unsigned char m_aiNumTimesRobbedBy[MAX_MAJOR_CIVS];
	unsigned char m_aiPerformedCoupAgainstUs[MAX_MAJOR_CIVS];
	char m_aiLikedTheirProposalValue[MAX_MAJOR_CIVS];
	char m_aiSupportedOurProposalValue[MAX_MAJOR_CIVS];
	short m_aiVotingHistoryScore[MAX_MAJOR_CIVS];
	char m_aiSupportedOurHostingValue[MAX_MAJOR_CIVS];
	unsigned char m_aiNegativeArchaeologyPoints[MAX_MAJOR_CIVS];
	unsigned char m_aiArtifactsEverDugUp[MAX_MAJOR_CIVS];
	unsigned char m_aiNumTimesNuked[MAX_MAJOR_CIVS];

	// Turn counters
	int m_aiResurrectedOnTurn[MAX_MAJOR_CIVS];
	int m_aiLiberatedCitiesTurn[MAX_MAJOR_CIVS];
	int m_aiCiviliansReturnedToMeTurn[MAX_MAJOR_CIVS];
	int m_aiIntrigueSharedTurn[MAX_MAJOR_CIVS];
	int m_aiPlayerForgaveForSpyingTurn[MAX_MAJOR_CIVS];
	int m_aiLandmarksBuiltForMeTurn[MAX_MAJOR_CIVS];
	int m_aiPlottedAgainstUsTurn[MAX_MAJOR_CIVS];
	int m_aiPlunderedTradeRouteTurn[MAX_MAJOR_CIVS];
	int m_aiBeatenToWonderTurn[MAX_MAJOR_CIVS];
	int m_aiLoweredOurInfluenceTurn[MAX_MAJOR_CIVS];
	int m_aiSidedWithProtectedMinorTurn[MAX_MAJOR_CIVS];
	int m_aiBulliedProtectedMinorTurn[MAX_MAJOR_CIVS];
	int m_aiAttackedProtectedMinorTurn[MAX_MAJOR_CIVS];
	int m_aiKilledProtectedMinorTurn[MAX_MAJOR_CIVS];
	int m_aiReligiousConversionTurn[MAX_MAJOR_CIVS];
	int m_aiTimesRobbedTurn[MAX_MAJOR_CIVS];
	int m_aiPerformedCoupTurn[MAX_MAJOR_CIVS];
	int m_aiStoleArtifactTurn[MAX_MAJOR_CIVS];
	int m_aiWeLikedTheirProposalTurn[MAX_MAJOR_CIVS];
	int m_aiWeDislikedTheirProposalTurn[MAX_MAJOR_CIVS];
	int m_aiTheySupportedOurProposalTurn[MAX_MAJOR_CIVS];
	int m_aiTheyFoiledOurProposalTurn[MAX_MAJOR_CIVS];
	int m_aiTheySanctionedUsTurn[MAX_MAJOR_CIVS];
	int m_aiTheyUnsanctionedUsTurn[MAX_MAJOR_CIVS];
	int m_aiTheySupportedOurHostingTurn[MAX_MAJOR_CIVS];

	// Player-Specific Memory
	char m_aeProtectedMinorBullied[MAX_MAJOR_CIVS];
	char m_aeProtectedMinorAttacked[MAX_MAJOR_CIVS];
	char m_aeProtectedMinorKilled[MAX_MAJOR_CIVS];
	
	// GUESSES
	// Guesses about other players' feelings towards us
	//char m_aeOpinionTowardsUsGuess[MAX_MAJOR_CIVS];
	//char m_aeApproachTowardsUsGuess[MAX_MAJOR_CIVS];
	//char m_aeApproachTowardsUsGuessCounter[MAX_MAJOR_CIVS];

	// C4DF Values
	char m_aeShareOpinionResponse[MAX_MAJOR_CIVS];
	int m_aiHelpRequestAcceptedTurn[MAX_MAJOR_CIVS];
	char m_aiHelpRequestTooSoonNumTurns[MAX_MAJOR_CIVS];
	bool m_abOfferingGift[MAX_MAJOR_CIVS];
	bool m_abOfferedGift[MAX_MAJOR_CIVS];
	int m_aiBrokeVassalAgreementTurn[MAX_MAJOR_CIVS];
	short m_aiVassalProtectValue[MAX_MAJOR_CIVS];
	int m_aiPlayerVassalagePeacefullyRevokedTurn[MAX_MAJOR_CIVS];
	int m_aiPlayerVassalageForcefullyRevokedTurn[MAX_MAJOR_CIVS];
	bool m_abMoveTroopsRequestAccepted[MAX_MAJOR_CIVS];
	bool m_abMasterLiberatedMeFromVassalage[MAX_MAJOR_CIVS];
	bool m_abVassalTaxRaised[MAX_MAJOR_CIVS];
	bool m_abVassalTaxLowered[MAX_MAJOR_CIVS];
	int m_aiVassalGoldPerTurnTaxedSinceVassalStarted[MAX_MAJOR_CIVS];
	int m_aiVassalGoldPerTurnCollectedSinceVassalStarted[MAX_MAJOR_CIVS];

	float m_aTradePriority[MAX_MAJOR_CIVS]; // current ai to human trade priority

	// Other
	typedef std::vector<PlayerTypes> PlayerTypesArray;
	PlayerTypesArray m_aGreetPlayers;

	DiplomacyMode m_eDiploMode;
	PlayerTypes	m_eTargetPlayer;

	// Data members for injecting test messages
	PlayerTypes			m_eTestToPlayer;
	DiploStatementTypes m_eTestStatement;
	int					m_iTestStatementArg1;
};

FDataStream& operator>>(FDataStream&, CvDiplomacyAI&);
FDataStream& operator<<(FDataStream&, const CvDiplomacyAI&);

namespace CvDiplomacyAIHelpers
{
	CvString GetWarmongerPreviewString(PlayerTypes eCurrentOwner = NO_PLAYER, CvCity* pCity = NULL, PlayerTypes eActivePlayer = NO_PLAYER);
	CvString GetLiberationPreviewString(PlayerTypes eOriginalOwner = NO_PLAYER, CvCity* pCity = NULL, PlayerTypes eActivePlayer = NO_PLAYER);
	int GetWarmongerTriggerPenalty(PlayerTypes eWarmonger = NO_PLAYER, TeamTypes eDefendingTeam = NO_TEAM, PlayerTypes eObserver = NO_PLAYER, WarmongerTriggerTypes eWarmongerTrigger = NO_WARMONGER_TRIGGER_TYPE);
	void ApplyWarmongerPenalties(CvCity* pCity, PlayerTypes eConqueror, PlayerTypes eCityOwner);
	void ApplyLiberationBonuses(CvCity* pCity, PlayerTypes eLiberator, PlayerTypes eNewOwner);
	int GetCityWarmongerValue(CvCity* pCity, PlayerTypes eConqueror, PlayerTypes eCityOwner, PlayerTypes eObserver);
	int GetCityLiberationValue(CvCity* pCity, PlayerTypes eLiberator, PlayerTypes eNewOwner, PlayerTypes eObserver);
	bool BackstabbedPlayer(PlayerTypes eBackstabber, PlayerTypes eVictim, bool bIncludeDoFEnd);
	bool IgnoresBackstabbing(PlayerTypes eObserver, PlayerTypes eVictim, bool bCheckCurrent = false);
	bool ProposedSanctionsBlockingDiplomacy(PlayerTypes ePlayer, PlayerTypes eOtherPlayer);
}

#endif //CIV5_AI_DIPLOMACY_H