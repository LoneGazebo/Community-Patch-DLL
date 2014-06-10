/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvDiplomacyAI.h"
#include "CvGrandStrategyAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvCitySpecializationAI.h"
#include "CvDealAI.h"
#include "CvGameCoreUtils.h"
#include "CvNotifications.h"
#include "CvDiplomacyRequests.h"

// must be included after all other headers
#include "LintFree.h"

#ifdef _MSC_VER
#	pragma warning ( disable : 4351 ) // default initialization of arrays
#endif

CvDiplomacyAI::DiplomacyAIData::DiplomacyAIData() :
	m_aDiploLogStatementTurnCountScratchPad()
	, m_aiMajorCivOpinion()
	, m_aeMajorCivApproach()
	, m_aeApproachScratchPad()
	, m_aeMinorCivApproach()
	, m_aeApproachTowardsUsGuess()
	, m_aeApproachTowardsUsGuessCounter()
	, m_aeWantPeaceCounter()
	, m_aePeaceTreatyWillingToOffer()
	, m_aePeaceTreatyWillingToAccept()
	, m_aiNumWondersBeatenTo()
	, m_abMusteringForAttack()
	, m_abWantsResearchAgreementWithPlayer()
	, m_abWantToRouteToMinor()
	, m_aeWarFace()
	, m_aeWarState()
	, m_aeWarProjection()
	, m_aeWarGoal()
	, m_aiPlayerNumTurnsAtWar()
	, m_aiNumWarsFought()
	, m_aePlayerMilitaryStrengthComparedToUs()
	, m_aePlayerEconomicStrengthComparedToUs()
	, m_aePlayerTargetValue()
	, m_aePlayerLandDisputeLevel()
	, m_aePlayerLastTurnLandDisputeLevel()
	, m_aePlayerVictoryDisputeLevel()
	, m_aePlayerWonderDisputeLevel()
	, m_aePlayerMinorCivDisputeLevel()
	, m_aeMilitaryAggressivePosture()
	, m_aeLastTurnMilitaryAggressivePosture()
	, m_aeExpansionAggressivePosture()
	, m_aePlotBuyingAggressivePosture()
	, m_aeMilitaryThreat()
	, m_aeWarDamageLevel()
	, m_aiWarValueLost()
	, m_aeWarmongerThreat()
	, m_aiPersonalityMajorCivApproachBiases()
	, m_aiPersonalityMinorCivApproachBiases()
	, m_aDeclarationsLog()
	, m_abPlayerNoSettleRequest()
	, m_aiPlayerNoSettleRequestCounter()
	, m_abPlayerStopSpyingRequest()
	, m_aiPlayerStopSpyingRequestCounter()
	, m_aiDemandCounter()
	, m_aiDemandTooSoonNumTurns()
	, m_abDoFAccepted()
	, m_aiDoFCounter()
	, m_abDenouncedPlayer()
	, m_abFriendDenouncedUs()
	, m_abFriendDeclaredWarOnUs()
	, m_aiDenouncedPlayerCounter()
	, m_aiNumRequestsRefused()
	, m_aiNumCiviliansReturnedToMe()
	, m_aiResurrectedOnTurn()
	, m_aiNumTimesCultureBombed()
	, m_paiNegativeReligiousConversionPoints()
	, m_aiNumTimesNuked()
	, m_aiNumTimesRobbedBy()
	, m_aiNumTimesIntrigueSharedBy()
	, m_abPlayerMadeMilitaryPromise()
	, m_abPlayerBrokenMilitaryPromise()
	, m_abPlayerIgnoredMilitaryPromise()
	, m_aiPlayerMilitaryPromiseCounter()
	, m_aiPlayerMadeExpansionPromiseTurn()
	, m_abPlayerBrokenExpansionPromise()
	, m_abPlayerIgnoredExpansionPromise()
	, m_aePlayerExpansionPromiseData()
	, m_aiPlayerMadeBorderPromiseTurn()
	, m_abPlayerBrokenBorderPromise()
	, m_abPlayerIgnoredBorderPromise()
	, m_aePlayerBorderPromiseData()
	, m_abPlayerMadeAttackCityStatePromise()
	, m_abPlayerBrokenAttackCityStatePromise()
	, m_abPlayerIgnoredAttackCityStatePromise()
	, m_abPlayerMadeBullyCityStatePromise()
	, m_abPlayerBrokenBullyCityStatePromise()
	, m_abPlayerIgnoredBullyCityStatePromise()
	, m_abPlayerMadeSpyPromise()
	, m_abPlayerBrokenSpyPromise()
	, m_abPlayerIgnoredSpyPromise()
	, m_abPlayerForgaveForSpying()
	, m_abPlayerMadeNoConvertPromise()
	, m_abPlayerBrokenNoConvertPromise()
	, m_abPlayerIgnoredNoConvertPromise()
	, m_abPlayerAskedNotToConvert()
	, m_abPlayerAgreedNotToConvert()
	, m_abPlayerBrokenCoopWarPromise()
	, m_aiOtherPlayerTurnsSinceAttackedProtectedMinor()
	, m_aiOtherPlayerProtectedMinorAttacked()
	, m_aiOtherPlayerNumProtectedMinorsAttacked()
	, m_aiOtherPlayerTurnsSinceKilledProtectedMinor()
	, m_aiOtherPlayerProtectedMinorKilled()
	, m_aiOtherPlayerNumProtectedMinorsKilled()
	, m_aiOtherPlayerNumProtectedMinorsBullied()
	, m_aiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor()
	, m_aiOtherPlayerNumMinorsAttacked()
	, m_aiOtherPlayerNumMinorsConquered()
	, m_aiOtherPlayerNumMajorsAttacked()
	, m_aiOtherPlayerNumMajorsConquered()
	, m_apaeOtherPlayerMajorCivOpinion()
	, m_apaeOtherPlayerLandDisputeLevel()
	, m_apaeOtherPlayerVictoryDisputeLevel()
	, m_apaeOtherPlayerWonderDisputeLevel()
	, m_apaeOtherPlayerMinorCivDisputeLevel()
	, m_apaeOtherPlayerWarDamageLevel()
	, m_apaiOtherPlayerWarValueLost()
	, m_apaiOtherPlayerLastRoundWarValueLost()
	, m_apabSentAttackMessageToMinorCivProtector()
	, m_apaeOtherPlayerMilitaryThreat()
	, m_apaDiploStatementsLog()
	, m_apabWorkingAgainstPlayerAccepted()
	, m_apaiWorkingAgainstPlayerCounter()
	, m_apacCoopWarAcceptedState()
	, m_apaiCoopWarCounter()
	, m_aaeOtherPlayerMajorCivOpinion()
	, m_aaeOtherPlayerLandDisputeLevel()
	, m_aaeOtherPlayerVictoryDisputeLevel()
	, m_aaeOtherPlayerWarDamageLevel()
	, m_aaiOtherPlayerWarValueLost()
	, m_aaiOtherPlayerLastRoundWarValueLost()
	, m_aabSentAttackMessageToMinorCivProtector()
	, m_aaeOtherPlayerMilitaryThreat()
	, m_aaDiploStatementsLog()
	, m_aabWorkingAgainstPlayerAccepted()
	, m_aaiWorkingAgainstPlayerCounter()
	, m_aacCoopWarAcceptedState()
	, m_aaiCoopWarCounter()
{
}

//=====================================
// CvDiplomacyAI
//=====================================
/// Constructor
CvDiplomacyAI::CvDiplomacyAI():
	m_pDiploData(NULL),
	m_paDiploLogStatementTurnCountScratchPad(NULL),
	m_paeMajorCivOpinion(NULL),
	m_ppaaeOtherPlayerMajorCivOpinion(NULL),
	m_paeMajorCivApproach(NULL),
	m_paeApproachScratchPad(NULL),
	m_paeMinorCivApproach(NULL),
	m_paeApproachTowardsUsGuess(NULL),
	m_paeApproachTowardsUsGuessCounter(NULL),
	m_paeWantPeaceCounter(NULL),
	m_paePeaceTreatyWillingToOffer(NULL),
	m_paePeaceTreatyWillingToAccept(NULL),
	m_paiNumWondersBeatenTo(NULL),
	m_pabMusteringForAttack(NULL),
	m_pabWantsResearchAgreementWithPlayer(NULL),
	m_pabWantToRouteToMinor(NULL),
	m_paeWarFace(NULL),
	m_paeWarState(NULL),
	m_paeWarProjection(NULL),
	m_paeLastWarProjection(NULL),
	m_paeWarGoal(NULL),
	m_paiPlayerNumTurnsAtWar(NULL),
	m_paiNumWarsFought(NULL),
	m_paePlayerMilitaryStrengthComparedToUs(NULL),
	m_paePlayerEconomicStrengthComparedToUs(NULL),
	m_paePlayerTargetValue(NULL),
	m_paePlayerLandDisputeLevel(NULL),
	m_paePlayerLastTurnLandDisputeLevel(NULL),
	m_paePlayerVictoryDisputeLevel(NULL),
	m_paePlayerWonderDisputeLevel(NULL),
	m_paePlayerMinorCivDisputeLevel(NULL),
	m_ppaaeOtherPlayerLandDisputeLevel(NULL),
	m_ppaaeOtherPlayerVictoryDisputeLevel(NULL),

	m_paeWarDamageLevel(NULL),
	m_paiWarValueLost(NULL),
	m_ppaaeOtherPlayerWarDamageLevel(NULL),
	m_ppaaiOtherPlayerWarValueLost(NULL),
	m_ppaaiOtherPlayerLastRoundWarValueLost(NULL),
	m_ppaabSentAttackMessageToMinorCivProtector(NULL),

	m_paeMilitaryAggressivePosture(NULL),
	m_paeLastTurnMilitaryAggressivePosture(NULL),
	m_paeExpansionAggressivePosture(NULL),
	m_paePlotBuyingAggressivePosture(NULL),

	m_paeMilitaryThreat(NULL),
	m_ppaaeOtherPlayerMilitaryThreat(NULL),

	m_paeWarmongerThreat(NULL),

	m_paiPersonalityMajorCivApproachBiases(NULL),
	m_paiPersonalityMinorCivApproachBiases(NULL),

	m_pabPlayerNoSettleRequestAccepted(NULL),
	m_paiPlayerNoSettleRequestCounter(NULL),

	m_pabPlayerStopSpyingRequestAccepted(NULL),
	m_paiPlayerStopSpyingRequestCounter(NULL),

	m_paiDemandCounter(NULL),
	m_paiDemandTooSoonNumTurns(NULL),

	m_pabDoFAccepted(NULL),
	m_paiDoFCounter(NULL),

	m_pabDenouncedPlayer(NULL),
	m_pabFriendDenouncedUs(NULL),
	m_pabFriendDeclaredWarOnUs(NULL),
	m_paiDenouncedPlayerCounter(NULL),

	m_paiNumRequestsRefused(NULL),

	m_paiNumCiviliansReturnedToMe(NULL),
	m_paiResurrectedOnTurn(NULL),
	m_paiNumTimesCultureBombed(NULL),

	m_paiNegativeReligiousConversionPoints(NULL),

	m_paiNumTimesNuked(NULL),
	m_paiNumTimesRobbedBy(NULL),
	m_paiNumTimesIntrigueSharedBy(NULL),

	m_paiBrokenExpansionPromiseValue(NULL),
	m_paiIgnoredExpansionPromiseValue(NULL),
	m_paiBrokenBorderPromiseValue(NULL),
	m_paiIgnoredBorderPromiseValue(NULL),
	m_paiDeclaredWarOnFriendValue(NULL),
	m_paiNumCitiesLiberated(NULL),
	m_paiTradeValue(NULL),
	m_paiCommonFoeValue(NULL),
	m_paiAssistValue(NULL),

	m_ppaabWorkingAgainstPlayerAccepted(NULL),
	m_ppaaiWorkingAgainstPlayerCounter(NULL),

	m_ppaacCoopWarAcceptedState(NULL),
	m_ppaaiCoopWarCounter(NULL),

	m_pabPlayerMadeMilitaryPromise(NULL),
	m_pabPlayerBrokenMilitaryPromise(NULL),
	m_pabPlayerIgnoredMilitaryPromise(NULL),
	m_paiPlayerMilitaryPromiseCounter(NULL),

	m_paiPlayerMadeExpansionPromiseTurn(NULL),
	m_pabPlayerBrokenExpansionPromise(NULL),
	m_pabPlayerIgnoredExpansionPromise(NULL),
	m_paePlayerExpansionPromiseData(NULL),

	m_paiPlayerMadeBorderPromiseTurn(NULL),
	m_pabPlayerBrokenBorderPromise(NULL),
	m_pabPlayerIgnoredBorderPromise(NULL),
	m_paePlayerBorderPromiseData(NULL),

	m_pabPlayerMadeAttackCityStatePromise(NULL),
	m_pabPlayerBrokenAttackCityStatePromise(NULL),
	m_pabPlayerIgnoredAttackCityStatePromise(NULL),

	m_pabPlayerMadeBullyCityStatePromise(NULL),
	m_pabPlayerBrokenBullyCityStatePromise(NULL),
	m_pabPlayerIgnoredBullyCityStatePromise(NULL),

	m_pabPlayerMadeSpyPromise(NULL),
	m_pabPlayerBrokenSpyPromise(NULL),
	m_pabPlayerIgnoredSpyPromise(NULL),

	m_pabPlayerForgaveForSpying(NULL),

	m_pabPlayerMadeNoConvertPromise(NULL),
	m_pabPlayerBrokenNoConvertPromise(NULL),
	m_pabPlayerIgnoredNoConvertPromise(NULL),
	m_pabPlayerAskedNotToConvert(NULL),
	m_pabPlayerAgreedNotToConvert(NULL),

	m_pabPlayerBrokenCoopWarPromise(NULL),

	m_paiOtherPlayerTurnsSinceAttackedProtectedMinor(NULL),
	m_paiOtherPlayerProtectedMinorAttacked(NULL),
	m_paiOtherPlayerNumProtectedMinorsAttacked(NULL),

	m_paiOtherPlayerTurnsSinceKilledProtectedMinor(NULL),
	m_paiOtherPlayerProtectedMinorKilled(NULL),
	m_paiOtherPlayerNumProtectedMinorsKilled(NULL),

	m_paiOtherPlayerNumProtectedMinorsBullied(NULL),

	m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor(NULL),

	m_paiOtherPlayerNumMinorsAttacked(NULL),
	m_paiOtherPlayerNumMinorsConquered(NULL),
	m_paiOtherPlayerNumMajorsAttacked(NULL),
	m_paiOtherPlayerNumMajorsConquered(NULL),

	m_paiOtherPlayerWarmongerAmount(NULL),

	m_ppaDiploStatementsLog(NULL),

	m_paDeclarationsLog(NULL),

	m_eStateAllWars(STATE_ALL_WARS_NEUTRAL),

	m_eTargetPlayer(NO_PLAYER),
	m_eTestToPlayer(NO_PLAYER),
	m_eTestStatement(NO_DIPLO_STATEMENT_TYPE),
	m_iTestStatementArg1(-1)
{
}

/// Destructor
CvDiplomacyAI::~CvDiplomacyAI(void)
{
}

/// Initialize
void CvDiplomacyAI::Init(CvPlayer* pPlayer)
{
	// Store off the pointer to the Player active for this game
	m_pPlayer = pPlayer;

	CvAssertMsg(!m_pDiploData, "MEMORY LEAK, CvDiplomacyAI::m_pDiploData");
	m_pDiploData = FNEW(DiplomacyAIData, c_eCiv5GameplayDLL, 0);

	//Init array pointers
	m_paDiploLogStatementTurnCountScratchPad = &m_pDiploData->m_aDiploLogStatementTurnCountScratchPad[0];
	m_paeMajorCivOpinion = &m_pDiploData->m_aiMajorCivOpinion[0];
	m_paeMajorCivApproach = &m_pDiploData->m_aeMajorCivApproach[0];
	m_paeApproachScratchPad = &m_pDiploData->m_aeApproachScratchPad[0];
	m_paeMinorCivApproach = &m_pDiploData->m_aeMinorCivApproach[0];
	m_paeApproachTowardsUsGuess = &m_pDiploData->m_aeApproachTowardsUsGuess[0];
	m_paeApproachTowardsUsGuessCounter = &m_pDiploData->m_aeApproachTowardsUsGuessCounter[0];
	m_paeWantPeaceCounter = &m_pDiploData->m_aeWantPeaceCounter[0];
	m_paePeaceTreatyWillingToOffer = &m_pDiploData->m_aePeaceTreatyWillingToOffer[0];
	m_paePeaceTreatyWillingToAccept = &m_pDiploData->m_aePeaceTreatyWillingToAccept[0];
	m_paiNumWondersBeatenTo = &m_pDiploData->m_aiNumWondersBeatenTo[0];
	m_pabMusteringForAttack = &m_pDiploData->m_abMusteringForAttack[0];
	m_pabWantsResearchAgreementWithPlayer = &m_pDiploData->m_abWantsResearchAgreementWithPlayer[0];
	m_pabWantToRouteToMinor = &m_pDiploData->m_abWantToRouteToMinor[0];
	m_paeWarFace = &m_pDiploData->m_aeWarFace[0];
	m_paeWarState = &m_pDiploData->m_aeWarState[0];
	m_paeWarProjection = &m_pDiploData->m_aeWarProjection[0];
	m_paeLastWarProjection = &m_pDiploData->m_aeLastWarProjection[0];
	m_paeWarGoal = &m_pDiploData->m_aeWarGoal[0];
	m_paiPlayerNumTurnsAtWar = &m_pDiploData->m_aiPlayerNumTurnsAtWar[0];
	m_paiNumWarsFought = &m_pDiploData->m_aiNumWarsFought[0];
	m_paePlayerMilitaryStrengthComparedToUs = &m_pDiploData->m_aePlayerMilitaryStrengthComparedToUs[0];
	m_paePlayerEconomicStrengthComparedToUs = &m_pDiploData->m_aePlayerEconomicStrengthComparedToUs[0];
	m_paePlayerTargetValue = &m_pDiploData->m_aePlayerTargetValue[0];
	m_paePlayerLandDisputeLevel = &m_pDiploData->m_aePlayerLandDisputeLevel[0];
	m_paePlayerLastTurnLandDisputeLevel = &m_pDiploData->m_aePlayerLastTurnLandDisputeLevel[0];
	m_paePlayerVictoryDisputeLevel = &m_pDiploData->m_aePlayerVictoryDisputeLevel[0];
	m_paePlayerWonderDisputeLevel = &m_pDiploData->m_aePlayerWonderDisputeLevel[0];
	m_paePlayerMinorCivDisputeLevel = &m_pDiploData->m_aePlayerMinorCivDisputeLevel[0];
	m_paeMilitaryAggressivePosture = &m_pDiploData->m_aeMilitaryAggressivePosture[0];
	m_paeLastTurnMilitaryAggressivePosture = &m_pDiploData->m_aeLastTurnMilitaryAggressivePosture[0];
	m_paeExpansionAggressivePosture = &m_pDiploData->m_aeExpansionAggressivePosture[0];
	m_paePlotBuyingAggressivePosture = &m_pDiploData->m_aePlotBuyingAggressivePosture[0];
	m_paeMilitaryThreat = &m_pDiploData->m_aeMilitaryThreat[0];
	m_paeWarDamageLevel = &m_pDiploData->m_aeWarDamageLevel[0];
	m_paiWarValueLost = &m_pDiploData->m_aiWarValueLost[0];
	m_paeWarmongerThreat = &m_pDiploData->m_aeWarmongerThreat[0];
	m_paiPersonalityMajorCivApproachBiases = &m_pDiploData->m_aiPersonalityMajorCivApproachBiases[0];
	m_paiPersonalityMinorCivApproachBiases = &m_pDiploData->m_aiPersonalityMinorCivApproachBiases[0];
	m_paDeclarationsLog = &m_pDiploData->m_aDeclarationsLog[0];

	// Things a player has told the AI

	m_pabPlayerNoSettleRequestAccepted = &m_pDiploData->m_abPlayerNoSettleRequest[0];
	m_paiPlayerNoSettleRequestCounter = &m_pDiploData->m_aiPlayerNoSettleRequestCounter[0];

	m_pabPlayerStopSpyingRequestAccepted = &m_pDiploData->m_abPlayerStopSpyingRequest[0];
	m_paiPlayerStopSpyingRequestCounter = &m_pDiploData->m_aiPlayerStopSpyingRequestCounter[0];

	m_paiDemandCounter = &m_pDiploData->m_aiDemandCounter[0];
	m_paiDemandTooSoonNumTurns = &m_pDiploData->m_aiDemandTooSoonNumTurns[0];

	m_pabDoFAccepted = &m_pDiploData->m_abDoFAccepted[0];
	m_paiDoFCounter = &m_pDiploData->m_aiDoFCounter[0];

	m_pabDenouncedPlayer = &m_pDiploData->m_abDenouncedPlayer[0];
	m_pabFriendDenouncedUs = &m_pDiploData->m_abFriendDenouncedUs[0];
	m_pabFriendDeclaredWarOnUs = &m_pDiploData->m_abFriendDeclaredWarOnUs[0];
	m_paiDenouncedPlayerCounter = &m_pDiploData->m_aiDenouncedPlayerCounter[0];

	m_paiNumRequestsRefused = &m_pDiploData->m_aiNumRequestsRefused[0];

	m_paiNumCiviliansReturnedToMe = &m_pDiploData->m_aiNumCiviliansReturnedToMe[0];
	m_paiResurrectedOnTurn = &m_pDiploData->m_aiResurrectedOnTurn[0];
	m_paiNumTimesCultureBombed = &m_pDiploData->m_aiNumTimesCultureBombed[0];

	m_paiNegativeReligiousConversionPoints = &m_pDiploData->m_paiNegativeReligiousConversionPoints[0];

	m_paiNumTimesNuked = &m_pDiploData->m_aiNumTimesNuked[0];
	m_paiNumTimesRobbedBy = &m_pDiploData->m_aiNumTimesRobbedBy[0];
	m_paiNumTimesIntrigueSharedBy = &m_pDiploData->m_aiNumTimesIntrigueSharedBy[0];

	m_paiBrokenExpansionPromiseValue = &m_pDiploData->m_aiBrokenExpansionPromiseValue[0];
	m_paiIgnoredExpansionPromiseValue = &m_pDiploData->m_aiIgnoredExpansionPromiseValue[0];
	m_paiBrokenBorderPromiseValue = &m_pDiploData->m_aiBrokenBorderPromiseValue[0];
	m_paiIgnoredBorderPromiseValue = &m_pDiploData->m_aiIgnoredBorderPromiseValue[0];
	m_paiDeclaredWarOnFriendValue = &m_pDiploData->m_aiDeclaredWarOnFriendValue[0];
	m_paiNumCitiesLiberated = &m_pDiploData->m_aiNumCitiesLiberated[0];
	m_paiTradeValue = &m_pDiploData->m_aiTradeValue[0];
	m_paiCommonFoeValue = &m_pDiploData->m_aiCommonFoeValue[0];
	m_paiAssistValue = &m_pDiploData->m_aiAssistValue[0];

	// Player's response to AI statements

	m_pabPlayerMadeMilitaryPromise = &m_pDiploData->m_abPlayerMadeMilitaryPromise[0];
	m_pabPlayerBrokenMilitaryPromise = &m_pDiploData->m_abPlayerBrokenMilitaryPromise[0];
	m_pabPlayerIgnoredMilitaryPromise = &m_pDiploData->m_abPlayerIgnoredMilitaryPromise[0];
	m_paiPlayerMilitaryPromiseCounter = &m_pDiploData->m_aiPlayerMilitaryPromiseCounter[0];

	m_paiPlayerMadeExpansionPromiseTurn = &m_pDiploData->m_aiPlayerMadeExpansionPromiseTurn[0];
	m_pabPlayerBrokenExpansionPromise = &m_pDiploData->m_abPlayerBrokenExpansionPromise[0];
	m_pabPlayerIgnoredExpansionPromise = &m_pDiploData->m_abPlayerIgnoredExpansionPromise[0];
	m_paePlayerExpansionPromiseData = &m_pDiploData->m_aePlayerExpansionPromiseData[0];

	m_paiPlayerMadeBorderPromiseTurn = &m_pDiploData->m_aiPlayerMadeBorderPromiseTurn[0];
	m_pabPlayerBrokenBorderPromise = &m_pDiploData->m_abPlayerBrokenBorderPromise[0];
	m_pabPlayerIgnoredBorderPromise = &m_pDiploData->m_abPlayerIgnoredBorderPromise[0];
	m_paePlayerBorderPromiseData = &m_pDiploData->m_aePlayerBorderPromiseData[0];

	m_pabPlayerMadeAttackCityStatePromise = &m_pDiploData->m_abPlayerMadeAttackCityStatePromise[0];
	m_pabPlayerBrokenAttackCityStatePromise = &m_pDiploData->m_abPlayerBrokenAttackCityStatePromise[0];
	m_pabPlayerIgnoredAttackCityStatePromise = &m_pDiploData->m_abPlayerIgnoredAttackCityStatePromise[0];

	m_pabPlayerMadeBullyCityStatePromise = &m_pDiploData->m_abPlayerMadeBullyCityStatePromise[0];
	m_pabPlayerBrokenBullyCityStatePromise = &m_pDiploData->m_abPlayerBrokenBullyCityStatePromise[0];
	m_pabPlayerIgnoredBullyCityStatePromise = &m_pDiploData->m_abPlayerIgnoredBullyCityStatePromise[0];

	m_pabPlayerMadeSpyPromise = &m_pDiploData->m_abPlayerMadeSpyPromise[0];
	m_pabPlayerBrokenSpyPromise = &m_pDiploData->m_abPlayerBrokenSpyPromise[0];
	m_pabPlayerIgnoredSpyPromise = &m_pDiploData->m_abPlayerIgnoredSpyPromise[0];

	m_pabPlayerForgaveForSpying = &m_pDiploData->m_abPlayerForgaveForSpying[0];

	m_pabPlayerMadeNoConvertPromise = &m_pDiploData->m_abPlayerMadeNoConvertPromise[0];
	m_pabPlayerBrokenNoConvertPromise = &m_pDiploData->m_abPlayerBrokenNoConvertPromise[0];
	m_pabPlayerIgnoredNoConvertPromise = &m_pDiploData->m_abPlayerIgnoredNoConvertPromise[0];

	m_pabPlayerAskedNotToConvert = &m_pDiploData->m_abPlayerAskedNotToConvert[0];
	m_pabPlayerAgreedNotToConvert = &m_pDiploData->m_abPlayerAgreedNotToConvert[0];

	m_pabPlayerBrokenCoopWarPromise = &m_pDiploData->m_abPlayerBrokenCoopWarPromise[0];

	// Evaluation of Other Players' Tendencies
	m_paiOtherPlayerTurnsSinceAttackedProtectedMinor = &m_pDiploData->m_aiOtherPlayerTurnsSinceAttackedProtectedMinor[0];
	m_paiOtherPlayerProtectedMinorAttacked = &m_pDiploData->m_aiOtherPlayerProtectedMinorAttacked[0];
	m_paiOtherPlayerNumProtectedMinorsAttacked = &m_pDiploData->m_aiOtherPlayerNumProtectedMinorsAttacked[0];
	m_paiOtherPlayerTurnsSinceKilledProtectedMinor = &m_pDiploData->m_aiOtherPlayerTurnsSinceKilledProtectedMinor[0];
	m_paiOtherPlayerProtectedMinorKilled = &m_pDiploData->m_aiOtherPlayerProtectedMinorKilled[0];
	m_paiOtherPlayerNumProtectedMinorsKilled = &m_pDiploData->m_aiOtherPlayerNumProtectedMinorsKilled[0];
	m_paiOtherPlayerNumProtectedMinorsBullied = &m_pDiploData->m_aiOtherPlayerNumProtectedMinorsBullied[0];
	m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor = &m_pDiploData->m_aiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[0];
	m_paiOtherPlayerNumMinorsAttacked = &m_pDiploData->m_aiOtherPlayerNumMinorsAttacked[0];
	m_paiOtherPlayerNumMinorsConquered = &m_pDiploData->m_aiOtherPlayerNumMinorsConquered[0];
	m_paiOtherPlayerNumMajorsAttacked = &m_pDiploData->m_aiOtherPlayerNumMajorsAttacked[0];
	m_paiOtherPlayerNumMajorsConquered = &m_pDiploData->m_aiOtherPlayerNumMajorsConquered[0];
	m_paiOtherPlayerWarmongerAmount = &m_pDiploData->m_aiOtherPlayerWarmongerAmount[0];

	//Init 2D array pointers
	int iI;
	m_ppaaeOtherPlayerMajorCivOpinion = &m_pDiploData->m_apaeOtherPlayerMajorCivOpinion[0];
	for(iI = 0; iI <MAX_MAJOR_CIVS; iI++)
	{
		m_ppaaeOtherPlayerMajorCivOpinion[iI] = &m_pDiploData->m_aaeOtherPlayerMajorCivOpinion[ MAX_MAJOR_CIVS * iI ];
	}

	m_ppaaeOtherPlayerLandDisputeLevel = &m_pDiploData->m_apaeOtherPlayerLandDisputeLevel[0];
	for(iI = 0; iI <MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaeOtherPlayerLandDisputeLevel[iI] = &m_pDiploData->m_aaeOtherPlayerLandDisputeLevel[ REALLY_MAX_PLAYERS * iI ];
	}

	m_ppaaeOtherPlayerVictoryDisputeLevel = &m_pDiploData->m_apaeOtherPlayerVictoryDisputeLevel[0];
	for(iI = 0; iI <MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaeOtherPlayerVictoryDisputeLevel[iI] = &m_pDiploData->m_aaeOtherPlayerVictoryDisputeLevel[ REALLY_MAX_PLAYERS * iI ];
	}

	m_ppaaeOtherPlayerWarDamageLevel = &m_pDiploData->m_apaeOtherPlayerWarDamageLevel[0];
	for(iI = 0; iI <MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaeOtherPlayerWarDamageLevel[iI] = &m_pDiploData->m_aaeOtherPlayerWarDamageLevel[REALLY_MAX_PLAYERS * iI];
	}

	m_ppaaiOtherPlayerWarValueLost = &m_pDiploData->m_apaiOtherPlayerWarValueLost[0];
	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaiOtherPlayerWarValueLost[iI] = &m_pDiploData->m_aaiOtherPlayerWarValueLost[REALLY_MAX_PLAYERS*iI];
	}

	m_ppaaiOtherPlayerLastRoundWarValueLost = &m_pDiploData->m_apaiOtherPlayerLastRoundWarValueLost[0];
	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaiOtherPlayerLastRoundWarValueLost[iI] = &m_pDiploData->m_aaiOtherPlayerLastRoundWarValueLost[REALLY_MAX_PLAYERS*iI];
	}

	m_ppaabSentAttackMessageToMinorCivProtector = &m_pDiploData->m_apabSentAttackMessageToMinorCivProtector[0];
	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_ppaabSentAttackMessageToMinorCivProtector[iI] = &m_pDiploData->m_aabSentAttackMessageToMinorCivProtector[REALLY_MAX_PLAYERS* iI];
	}

	m_ppaaeOtherPlayerMilitaryThreat = &m_pDiploData->m_apaeOtherPlayerMilitaryThreat[0];
	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_ppaaeOtherPlayerMilitaryThreat[iI] = &m_pDiploData->m_aaeOtherPlayerMilitaryThreat[REALLY_MAX_PLAYERS*iI];
	}

	m_ppaDiploStatementsLog = &m_pDiploData->m_apaDiploStatementsLog[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaDiploStatementsLog[iI] = &m_pDiploData->m_aaDiploStatementsLog[iI * MAX_DIPLO_LOG_STATEMENTS];
	}

	m_ppaabWorkingAgainstPlayerAccepted = &m_pDiploData->m_apabWorkingAgainstPlayerAccepted[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaabWorkingAgainstPlayerAccepted[iI] = &m_pDiploData->m_aabWorkingAgainstPlayerAccepted[iI * MAX_MAJOR_CIVS];
	}

	m_ppaaiWorkingAgainstPlayerCounter = &m_pDiploData->m_apaiWorkingAgainstPlayerCounter[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaaiWorkingAgainstPlayerCounter[iI] = &m_pDiploData->m_aaiWorkingAgainstPlayerCounter[iI * MAX_MAJOR_CIVS];
	}

	m_ppaacCoopWarAcceptedState = &m_pDiploData->m_apacCoopWarAcceptedState[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaacCoopWarAcceptedState[iI] = &m_pDiploData->m_aacCoopWarAcceptedState[iI * MAX_MAJOR_CIVS];
	}

	m_ppaaiCoopWarCounter = &m_pDiploData->m_apaiCoopWarCounter[0];
	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_ppaaiCoopWarCounter[iI] = &m_pDiploData->m_aaiCoopWarCounter[iI * MAX_MAJOR_CIVS];
	}

	Reset();
}

/// Deallocate memory created in initialize
void CvDiplomacyAI::Uninit()
{

	//Init array pointers
	m_paDiploLogStatementTurnCountScratchPad = NULL;
	m_paeMajorCivOpinion = NULL;
	m_paeMajorCivApproach = NULL;
	m_paeApproachScratchPad = NULL;
	m_paeMinorCivApproach = NULL;
	m_paeApproachTowardsUsGuess = NULL;
	m_paeApproachTowardsUsGuessCounter = NULL;
	m_paeWantPeaceCounter = NULL;
	m_paePeaceTreatyWillingToOffer = NULL;
	m_paePeaceTreatyWillingToAccept = NULL;
	m_paiNumWondersBeatenTo = NULL;
	m_pabMusteringForAttack = NULL;
	m_pabWantsResearchAgreementWithPlayer = NULL;
	m_pabWantToRouteToMinor = NULL;
	m_paeWarFace = NULL;
	m_paeWarState = NULL;
	m_paeWarProjection = NULL;
	m_paeLastWarProjection = NULL;
	m_paeWarGoal = NULL;
	m_paiPlayerNumTurnsAtWar = NULL;
	m_paiNumWarsFought = NULL;
	m_paePlayerMilitaryStrengthComparedToUs = NULL;
	m_paePlayerEconomicStrengthComparedToUs = NULL;
	m_paePlayerTargetValue = NULL;
	m_paePlayerLandDisputeLevel = NULL;
	m_paePlayerLastTurnLandDisputeLevel = NULL;
	m_paePlayerVictoryDisputeLevel = NULL;
	m_paePlayerWonderDisputeLevel = NULL;
	m_paePlayerMinorCivDisputeLevel = NULL;
	m_paeMilitaryAggressivePosture = NULL;
	m_paeLastTurnMilitaryAggressivePosture = NULL;
	m_paeExpansionAggressivePosture = NULL;
	m_paePlotBuyingAggressivePosture = NULL;
	m_paeMilitaryThreat = NULL;
	m_paeWarDamageLevel = NULL;
	m_paiWarValueLost = NULL;
	m_paeWarmongerThreat = NULL;
	m_paiPersonalityMajorCivApproachBiases = NULL;
	m_paiPersonalityMinorCivApproachBiases = NULL;
	m_paDeclarationsLog = NULL;

	// Things a player has told the AI

	m_pabPlayerNoSettleRequestAccepted = NULL;
	m_paiPlayerNoSettleRequestCounter = NULL;

	m_pabPlayerStopSpyingRequestAccepted = NULL;
	m_paiPlayerStopSpyingRequestCounter = NULL;

	m_paiDemandCounter = NULL;
	m_paiDemandTooSoonNumTurns = NULL;

	m_pabDoFAccepted = NULL;
	m_paiDoFCounter = NULL;

	m_pabDenouncedPlayer = NULL;
	m_pabFriendDenouncedUs = NULL;
	m_pabFriendDeclaredWarOnUs = NULL;
	m_paiDenouncedPlayerCounter = NULL;

	m_paiNumRequestsRefused = NULL;

	m_paiNumCiviliansReturnedToMe = NULL;
	m_paiResurrectedOnTurn = NULL;
	m_paiNumTimesCultureBombed = NULL;

	m_paiNegativeReligiousConversionPoints = NULL;

	m_paiNumTimesNuked = NULL;
	m_paiNumTimesRobbedBy = NULL;
	m_paiNumTimesIntrigueSharedBy = NULL;

	m_paiBrokenExpansionPromiseValue = NULL;
	m_paiIgnoredExpansionPromiseValue = NULL;
	m_paiBrokenBorderPromiseValue = NULL;
	m_paiIgnoredBorderPromiseValue = NULL;
	m_paiDeclaredWarOnFriendValue = NULL;
	m_paiNumCitiesLiberated = NULL;
	m_paiTradeValue = NULL;
	m_paiCommonFoeValue = NULL;
	m_paiAssistValue = NULL;

	m_ppaabWorkingAgainstPlayerAccepted = NULL;
	m_ppaaiWorkingAgainstPlayerCounter = NULL;

	m_ppaacCoopWarAcceptedState = NULL;
	m_ppaaiCoopWarCounter = NULL;

	// Player's repsonse to AI statements

	m_pabPlayerMadeMilitaryPromise = NULL;
	m_pabPlayerBrokenMilitaryPromise = NULL;
	m_pabPlayerIgnoredMilitaryPromise = NULL;
	m_paiPlayerMilitaryPromiseCounter = NULL;

	m_paiPlayerMadeExpansionPromiseTurn = NULL;
	m_pabPlayerBrokenExpansionPromise = NULL;
	m_pabPlayerIgnoredExpansionPromise = NULL;
	m_paePlayerExpansionPromiseData = NULL;

	m_paiPlayerMadeBorderPromiseTurn = NULL;
	m_pabPlayerBrokenBorderPromise = NULL;
	m_pabPlayerIgnoredBorderPromise = NULL;
	m_paePlayerBorderPromiseData = NULL;

	m_pabPlayerMadeAttackCityStatePromise = NULL;
	m_pabPlayerBrokenAttackCityStatePromise = NULL;
	m_pabPlayerIgnoredAttackCityStatePromise = NULL;

	m_pabPlayerMadeBullyCityStatePromise = NULL;
	m_pabPlayerBrokenBullyCityStatePromise = NULL;
	m_pabPlayerIgnoredBullyCityStatePromise = NULL;

	m_pabPlayerMadeSpyPromise = NULL;
	m_pabPlayerBrokenSpyPromise = NULL;
	m_pabPlayerIgnoredSpyPromise = NULL;

	m_pabPlayerForgaveForSpying = NULL;

	m_pabPlayerMadeNoConvertPromise = NULL;
	m_pabPlayerBrokenNoConvertPromise = NULL;
	m_pabPlayerIgnoredNoConvertPromise = NULL;
	m_pabPlayerAskedNotToConvert = NULL;
	m_pabPlayerAgreedNotToConvert = NULL;

	m_pabPlayerBrokenCoopWarPromise = NULL;

	// Evaluation of Other Players' Tendencies
	m_paiOtherPlayerTurnsSinceAttackedProtectedMinor = NULL;
	m_paiOtherPlayerProtectedMinorAttacked = NULL;
	m_paiOtherPlayerNumProtectedMinorsAttacked = NULL;
	m_paiOtherPlayerTurnsSinceKilledProtectedMinor = NULL;
	m_paiOtherPlayerProtectedMinorKilled = NULL;
	m_paiOtherPlayerNumProtectedMinorsKilled = NULL;
	m_paiOtherPlayerNumProtectedMinorsBullied = NULL;
	m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor = NULL;
	m_paiOtherPlayerNumMinorsAttacked = NULL;
	m_paiOtherPlayerNumMinorsConquered = NULL;
	m_paiOtherPlayerNumMajorsAttacked = NULL;
	m_paiOtherPlayerNumMajorsConquered = NULL;
	m_paiOtherPlayerWarmongerAmount = NULL;

	m_ppaaeOtherPlayerLandDisputeLevel = NULL;
	m_ppaaeOtherPlayerVictoryDisputeLevel = NULL;
	m_ppaaeOtherPlayerWarDamageLevel = NULL;
	m_ppaaiOtherPlayerWarValueLost = NULL;
	m_ppaaiOtherPlayerLastRoundWarValueLost = NULL;
	m_ppaabSentAttackMessageToMinorCivProtector = NULL;
	m_ppaaeOtherPlayerMilitaryThreat = NULL;
	m_ppaDiploStatementsLog = NULL;

	delete m_pDiploData;
	m_pDiploData = NULL;
}

/// Reset everything to default state
void CvDiplomacyAI::Reset()
{
	int iI, iJ;

	for(iI = 0; iI < NUM_DIPLO_LOG_STATEMENT_TYPES; iI++)
	{
		m_paDiploLogStatementTurnCountScratchPad[iI] = NO_DIPLO_STATEMENT_TYPE;
	}

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		m_paeMajorCivOpinion[iI] = NO_MAJOR_CIV_OPINION_TYPE;

		for(iJ = 0; iJ < MAX_MAJOR_CIVS; iJ++)
		{
			m_ppaaeOtherPlayerMajorCivOpinion[iI][iJ] = NO_MAJOR_CIV_OPINION_TYPE;
			m_ppaabWorkingAgainstPlayerAccepted[iI][iJ] = false;
			m_ppaaiWorkingAgainstPlayerCounter[iI][iJ] = -1;
			m_ppaacCoopWarAcceptedState[iI][iJ] = NO_COOP_WAR_STATE;
			m_ppaaiCoopWarCounter[iI][iJ] = -1;
		}

		m_paeMajorCivApproach[iI] = NO_MAJOR_CIV_APPROACH;
		m_paeApproachTowardsUsGuess[iI] = NO_MAJOR_CIV_APPROACH;
		m_paeApproachTowardsUsGuessCounter[iI] = 0;

		m_paePeaceTreatyWillingToOffer[iI] = NO_PEACE_TREATY_TYPE;
		m_paePeaceTreatyWillingToAccept[iI] = NO_PEACE_TREATY_TYPE;

		m_paiOtherPlayerTurnsSinceAttackedProtectedMinor[iI] = -1;
		m_paiOtherPlayerProtectedMinorAttacked[iI] = NO_PLAYER;
		m_paiOtherPlayerNumProtectedMinorsAttacked[iI] = 0;

		m_paiOtherPlayerTurnsSinceKilledProtectedMinor[iI] = -1;
		m_paiOtherPlayerProtectedMinorKilled[iI] = NO_PLAYER;
		m_paiOtherPlayerNumProtectedMinorsKilled[iI] = 0;

		m_paiOtherPlayerNumProtectedMinorsBullied[iI] = 0;

		m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[iI] = -1;

		m_paiOtherPlayerNumMinorsAttacked[iI] = 0;
		m_paiOtherPlayerNumMinorsConquered[iI] = 0;
		m_paiOtherPlayerNumMajorsAttacked[iI] = 0;
		m_paiOtherPlayerNumMajorsConquered[iI] = 0;
		
		m_paiOtherPlayerWarmongerAmount[iI] = 0;

		m_pabWantsResearchAgreementWithPlayer[iI] = false;

		// Things a player has told this AI

		m_pabPlayerNoSettleRequestAccepted[iI] = false;
		m_paiPlayerNoSettleRequestCounter[iI] = -1;

		m_pabPlayerStopSpyingRequestAccepted[iI] = false;
		m_paiPlayerStopSpyingRequestCounter[iI] = -1;

		m_paiDemandCounter[iI] = -1;
		m_paiDemandTooSoonNumTurns[iI] = -1;

		m_pabDoFAccepted[iI] = false;
		m_paiDoFCounter[iI] = -1;

		m_pabDenouncedPlayer[iI] = false;
		m_pabFriendDenouncedUs[iI] = false;
		m_pabFriendDeclaredWarOnUs[iI] = false;
		m_paiDenouncedPlayerCounter[iI] = -1;

		m_paiNumRequestsRefused[iI] = 0;

		m_paiNumCiviliansReturnedToMe[iI] = 0;
		m_paiResurrectedOnTurn[iI] = -1;
		m_paiNumTimesCultureBombed[iI] = 0;

		m_paiNegativeReligiousConversionPoints[iI] = 0;

		m_paiNumTimesNuked[iI] = 0;
		m_paiNumTimesRobbedBy[iI] = 0;
		m_paiNumTimesIntrigueSharedBy[iI] = 0;

		m_paiBrokenExpansionPromiseValue[iI] = 0;
		m_paiIgnoredExpansionPromiseValue[iI] = 0;
		m_paiBrokenBorderPromiseValue[iI] = 0;
		m_paiIgnoredBorderPromiseValue[iI] = 0;
		m_paiDeclaredWarOnFriendValue[iI] = 0;
		m_paiNumCitiesLiberated[iI] = 0;
		m_paiTradeValue[iI] = 0;
		m_paiCommonFoeValue[iI] = 0;
		m_paiAssistValue[iI] = 0;

		// Player's repsonse to AI statements

		m_pabPlayerMadeMilitaryPromise[iI] = false;
		m_pabPlayerBrokenMilitaryPromise[iI] = false;
		m_pabPlayerIgnoredMilitaryPromise[iI] = false;
		m_paiPlayerMilitaryPromiseCounter[iI] = -1;

		m_paiPlayerMadeExpansionPromiseTurn[iI] = -1;
		m_pabPlayerBrokenExpansionPromise[iI] = false;
		m_pabPlayerIgnoredExpansionPromise[iI] = false;
		m_paePlayerExpansionPromiseData[iI] = NO_AGGRESSIVE_POSTURE_TYPE;

		m_paiPlayerMadeBorderPromiseTurn[iI] = -1;
		m_pabPlayerBrokenBorderPromise[iI] = false;
		m_pabPlayerIgnoredBorderPromise[iI] = false;
		m_paePlayerBorderPromiseData[iI] = NO_AGGRESSIVE_POSTURE_TYPE;

		m_pabPlayerMadeAttackCityStatePromise[iI] = false;
		m_pabPlayerBrokenAttackCityStatePromise[iI] = false;
		m_pabPlayerIgnoredAttackCityStatePromise[iI] = false;

		m_pabPlayerMadeBullyCityStatePromise[iI] = false;
		m_pabPlayerBrokenBullyCityStatePromise[iI] = false;
		m_pabPlayerIgnoredBullyCityStatePromise[iI] = false;

		m_pabPlayerMadeSpyPromise[iI] = false;
		m_pabPlayerBrokenSpyPromise[iI] = false;
		m_pabPlayerIgnoredSpyPromise[iI] = false;

		m_pabPlayerForgaveForSpying[iI] = false;

		m_pabPlayerMadeNoConvertPromise[iI] = false;
		m_pabPlayerBrokenNoConvertPromise[iI] = false;
		m_pabPlayerIgnoredNoConvertPromise[iI] = false;
		m_pabPlayerAskedNotToConvert[iI] = false;
		m_pabPlayerAgreedNotToConvert[iI] = false;

		m_pabPlayerBrokenCoopWarPromise[iI] = false;
	}

	for(iI = 0; iI < MAX_MINOR_CIVS; iI++)
	{
		m_paeMinorCivApproach[iI] = NO_MINOR_CIV_APPROACH;
		m_pabWantToRouteToMinor[iI] = true;
	}

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		m_paeApproachScratchPad[iI] = -1;

		m_paiNumWondersBeatenTo[iI] = 0;
		m_pabMusteringForAttack[iI] = false;

		m_paeWantPeaceCounter[iI] = 0;

		m_paeWarFace[iI] = NO_WAR_FACE_TYPE;
		m_paeWarState[iI] = NO_WAR_STATE_TYPE;
		m_paeWarProjection[iI] = NO_WAR_PROJECTION_TYPE;
		m_paeLastWarProjection[iI] = NO_WAR_PROJECTION_TYPE;
		m_paeWarGoal[iI] = NO_WAR_GOAL_TYPE;
		m_paiPlayerNumTurnsAtWar[iI] = 0;
		m_paiNumWarsFought[iI] = 0;

		m_paePlayerMilitaryStrengthComparedToUs[iI] = NO_STRENGTH_VALUE;
		m_paePlayerEconomicStrengthComparedToUs[iI] = NO_STRENGTH_VALUE;
		m_paePlayerTargetValue[iI] = NO_TARGET_VALUE;

		m_paePlayerLandDisputeLevel[iI] = NO_DISPUTE_LEVEL;
		m_paePlayerLastTurnLandDisputeLevel[iI] = NO_DISPUTE_LEVEL;
		m_paePlayerVictoryDisputeLevel[iI] = NO_DISPUTE_LEVEL;
		m_paePlayerWonderDisputeLevel[iI] = NO_DISPUTE_LEVEL;
		m_paePlayerMinorCivDisputeLevel[iI] = NO_DISPUTE_LEVEL;

		for(iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
		{
			m_ppaaeOtherPlayerLandDisputeLevel[iI][iJ] = NO_DISPUTE_LEVEL;
			m_ppaaeOtherPlayerVictoryDisputeLevel[iI][iJ] = NO_DISPUTE_LEVEL;

			m_ppaaeOtherPlayerWarDamageLevel[iI][iJ] = NO_WAR_DAMAGE_LEVEL_VALUE;
			m_ppaaiOtherPlayerWarValueLost[iI][iJ] = 0;
			m_ppaaiOtherPlayerLastRoundWarValueLost[iI][iJ] = 0;
			m_ppaabSentAttackMessageToMinorCivProtector[iI][iJ] = false;

			m_ppaaeOtherPlayerMilitaryThreat[iI][iJ] = NO_THREAT_VALUE;
		}

		m_paeWarDamageLevel[iI] = NO_WAR_DAMAGE_LEVEL_VALUE;
		m_paiWarValueLost[iI] = 0;

		m_paeMilitaryAggressivePosture[iI] = NO_AGGRESSIVE_POSTURE_TYPE;
		m_paeLastTurnMilitaryAggressivePosture[iI] = NO_AGGRESSIVE_POSTURE_TYPE;
		m_paeExpansionAggressivePosture[iI] = NO_AGGRESSIVE_POSTURE_TYPE;
		m_paePlotBuyingAggressivePosture[iI] = NO_AGGRESSIVE_POSTURE_TYPE;

		m_paeMilitaryThreat[iI] = NO_THREAT_VALUE;

		m_paeWarmongerThreat[iI] = NO_THREAT_VALUE;
	}

	m_eDemandTargetPlayer = NO_PLAYER;
	m_bDemandReady = false;

	m_iVictoryCompetitiveness = -1;
	m_iWonderCompetitiveness = -1;
	m_iMinorCivCompetitiveness = -1;
	m_iBoldness = -1;
	m_iDiploBalance = -1;
	m_iWarmongerHate = -1;
	m_iDenounceWillingness = -1;
	m_iDoFWillingness = -1;
	m_iLoyalty = -1;
	m_iNeediness = -1;
	m_iForgiveness = -1;
	m_iChattiness = -1;
	m_iMeanness = -1;

	for(iI = 0; iI < NUM_MAJOR_CIV_APPROACHES; iI++)
	{
		m_paiPersonalityMajorCivApproachBiases[iI] = 0;
	}

	for(iI = 0; iI < NUM_MINOR_CIV_APPROACHES; iI++)
	{
		m_paiPersonalityMinorCivApproachBiases[iI] = 0;
	}

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		for(iJ = 0; iJ < MAX_DIPLO_LOG_STATEMENTS; iJ++)
		{
			m_ppaDiploStatementsLog[iI][iJ].m_eDiploLogStatement = NO_DIPLO_STATEMENT_TYPE;
			m_ppaDiploStatementsLog[iI][iJ].m_iTurn = -1;
		}
	}

	for(iI = 0; iI < MAX_DIPLO_LOG_STATEMENTS; iI++)
	{
		m_paDeclarationsLog[iI].m_eDeclaration = NO_PUBLIC_DECLARATION_TYPE;
		m_paDeclarationsLog[iI].m_iTurn = -1;
	}

	m_eStateAllWars = STATE_ALL_WARS_NEUTRAL;

	m_aGreetPlayers.clear();
}

/// Serialization read
void CvDiplomacyAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	int iI;

	ArrayWrapper<char> wrapMajorOpinion(MAX_MAJOR_CIVS, m_paeMajorCivOpinion);
	kStream >> wrapMajorOpinion;

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		ArrayWrapper<char> wrapOtherMajorOpinion(MAX_MAJOR_CIVS, m_ppaaeOtherPlayerMajorCivOpinion[iI]);
		kStream >> wrapOtherMajorOpinion;

		ArrayWrapper<bool> wrapWorkingAgainstPlayerAccepted(MAX_MAJOR_CIVS, m_ppaabWorkingAgainstPlayerAccepted[iI]);
		kStream >> wrapWorkingAgainstPlayerAccepted;

		ArrayWrapper<short> workingAgainstPlayerCounter(MAX_MAJOR_CIVS, m_ppaaiWorkingAgainstPlayerCounter[iI]);
		kStream >> workingAgainstPlayerCounter;

		ArrayWrapper<char> wrapCoopWarAcceptedState(MAX_MAJOR_CIVS, m_ppaacCoopWarAcceptedState[iI]);
		kStream >> wrapCoopWarAcceptedState;

		ArrayWrapper<short> wrapCoopWarCounter(MAX_MAJOR_CIVS, m_ppaaiCoopWarCounter[iI]);
		kStream >> wrapCoopWarCounter;
	}

	ArrayWrapper<char> wrapm_paeMajorCivApproach(MAX_MAJOR_CIVS, m_paeMajorCivApproach);
	kStream >> wrapm_paeMajorCivApproach;

	ArrayWrapper<char> wrapm_paeMinorCivApproach(MAX_MINOR_CIVS, m_paeMinorCivApproach);
	kStream >> wrapm_paeMinorCivApproach;

	ArrayWrapper<char> wrapm_paeApproachTowardsUsGuess(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuess);
	kStream >> wrapm_paeApproachTowardsUsGuess;

	ArrayWrapper<char> wrapm_paeApproachTowardsUsGuessCounter(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuessCounter);
	kStream >> wrapm_paeApproachTowardsUsGuessCounter;


	ArrayWrapper<short> wrapm_paiNumWondersBeatenTo(MAX_CIV_PLAYERS, m_paiNumWondersBeatenTo);
	kStream >> wrapm_paiNumWondersBeatenTo;

	ArrayWrapper<bool> wrapm_pabMusteringForAttack(MAX_CIV_PLAYERS, m_pabMusteringForAttack);
	kStream >> wrapm_pabMusteringForAttack;


	ArrayWrapper<bool> wrapm_pabWantsResearchAgreementWithPlayer(MAX_MAJOR_CIVS, m_pabWantsResearchAgreementWithPlayer);
	kStream >> wrapm_pabWantsResearchAgreementWithPlayer;

	ArrayWrapper<bool> wrapm_pabWantToRouteToMinor(MAX_MINOR_CIVS, m_pabWantToRouteToMinor);
	kStream >> wrapm_pabWantToRouteToMinor;


	ArrayWrapper<short> wrapm_paeWantPeaceCounter(MAX_CIV_PLAYERS, m_paeWantPeaceCounter);
	kStream >> wrapm_paeWantPeaceCounter;

	ArrayWrapper<char> wrapm_paePeaceTreatyWillingToOffer(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToOffer);
	kStream >> wrapm_paePeaceTreatyWillingToOffer;

	ArrayWrapper<char> wrapm_paePeaceTreatyWillingToAccept(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToAccept);
	kStream >> wrapm_paePeaceTreatyWillingToAccept;


	ArrayWrapper<char> wrapm_paeWarFace(MAX_CIV_PLAYERS, m_paeWarFace);
	kStream >> wrapm_paeWarFace;

	ArrayWrapper<char> wrapm_paeWarState(MAX_CIV_PLAYERS, m_paeWarState);
	kStream >> wrapm_paeWarState;

	ArrayWrapper<char> wrapm_paeWarProjection(MAX_CIV_PLAYERS, m_paeWarProjection);
	kStream >> wrapm_paeWarProjection;

	if (uiVersion >= 26)
	{
		ArrayWrapper<char> wrapm_paeLastWarProjection(MAX_CIV_PLAYERS, m_paeLastWarProjection);
		kStream >> wrapm_paeLastWarProjection;
	}
	else
	{
		for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			m_paeLastWarProjection[iI] = NO_WAR_PROJECTION_TYPE;
		}
	}

	ArrayWrapper<char> wrapm_paeWarGoal(MAX_CIV_PLAYERS, m_paeWarGoal);
	kStream >> wrapm_paeWarGoal;

	ArrayWrapper<short> wrapm_paiPlayerNumTurnsAtWar(MAX_CIV_PLAYERS, m_paiPlayerNumTurnsAtWar);
	kStream >> wrapm_paiPlayerNumTurnsAtWar;

	if(uiVersion >= 4)
	{
		ArrayWrapper<short> wrapm_paiNumWarsFought(MAX_CIV_PLAYERS, m_paiNumWarsFought);
		kStream >> wrapm_paiNumWarsFought;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiNumWarsFought[iI] = 0;
		}
	}


	ArrayWrapper<char> wrapm_paePlayerMilitaryStrengthComparedToUs(MAX_CIV_PLAYERS, m_paePlayerMilitaryStrengthComparedToUs);
	kStream >> wrapm_paePlayerMilitaryStrengthComparedToUs;

	ArrayWrapper<char> wrapm_paePlayerEconomicStrengthComparedToUs(MAX_CIV_PLAYERS, m_paePlayerEconomicStrengthComparedToUs);
	kStream >> wrapm_paePlayerEconomicStrengthComparedToUs;

	ArrayWrapper<char> wrapm_paePlayerTargetValue(MAX_CIV_PLAYERS, m_paePlayerTargetValue);
	kStream >> wrapm_paePlayerTargetValue;


	ArrayWrapper<char> wrapm_paePlayerLandDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerLandDisputeLevel);
	kStream >> wrapm_paePlayerLandDisputeLevel;

	ArrayWrapper<char> wrapm_paePlayerLastTurnLandDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerLastTurnLandDisputeLevel);
	kStream >> wrapm_paePlayerLastTurnLandDisputeLevel;

	ArrayWrapper<char> wrapm_paePlayerVictoryDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerVictoryDisputeLevel);
	kStream >> wrapm_paePlayerVictoryDisputeLevel;

	ArrayWrapper<char> wrapm_paePlayerWonderDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerWonderDisputeLevel);
	kStream >> wrapm_paePlayerWonderDisputeLevel;

	ArrayWrapper<char> wrapm_paePlayerMinorCivDisputeLevel(MAX_CIV_PLAYERS, m_paePlayerMinorCivDisputeLevel);
	kStream >> wrapm_paePlayerMinorCivDisputeLevel;


	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		ArrayWrapper<char> wrapm_ppaaeOtherPlayerLandDisputeLevel(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerLandDisputeLevel[iI]);
		kStream >> wrapm_ppaaeOtherPlayerLandDisputeLevel;

		ArrayWrapper<char> wrapm_ppaaeOtherPlayerVictoryDisputeLevel(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerVictoryDisputeLevel[iI]);
		kStream >> wrapm_ppaaeOtherPlayerVictoryDisputeLevel;

		ArrayWrapper<char> wrapm_ppaaeOtherPlayerWarDamageLevel(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerWarDamageLevel[iI]);
		kStream >> wrapm_ppaaeOtherPlayerWarDamageLevel;

		ArrayWrapper<int> wrapm_ppaaiOtherPlayerWarValueLost(MAX_CIV_PLAYERS, m_ppaaiOtherPlayerWarValueLost[iI]);
		kStream >> wrapm_ppaaiOtherPlayerWarValueLost;

		ArrayWrapper<char> wrapm_ppaaeOtherPlayerMilitaryThreat(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerMilitaryThreat[iI]);
		kStream >> wrapm_ppaaeOtherPlayerMilitaryThreat;
	}

	if(uiVersion >= 29)
	{
		for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			ArrayWrapper<int> wrapm_ppaaiOtherPlayerLastRoundWarValueLost(MAX_CIV_PLAYERS, m_ppaaiOtherPlayerLastRoundWarValueLost[iI]);
			kStream >> wrapm_ppaaiOtherPlayerLastRoundWarValueLost;

			ArrayWrapper<bool> wrapm_ppaabSentAttackMessageToMinorCivProtector(MAX_CIV_PLAYERS, m_ppaabSentAttackMessageToMinorCivProtector[iI]);
			kStream >> wrapm_ppaabSentAttackMessageToMinorCivProtector;
		}
	}
	else
	{
		for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			for(int iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
			{
				m_ppaaiOtherPlayerLastRoundWarValueLost[iI][iJ] = 0;
				m_ppaabSentAttackMessageToMinorCivProtector[iI][iJ] = false;
			}
		}
	}

	ArrayWrapper<char> wrapm_paeWarDamageLevel(MAX_CIV_PLAYERS, m_paeWarDamageLevel);
	kStream >> wrapm_paeWarDamageLevel;

	ArrayWrapper<int> wrapm_paiWarValueLost(MAX_CIV_PLAYERS, m_paiWarValueLost);
	kStream >> wrapm_paiWarValueLost;

	ArrayWrapper<char> wrapm_paeMilitaryAggressivePosture(MAX_CIV_PLAYERS, m_paeMilitaryAggressivePosture);
	kStream >> wrapm_paeMilitaryAggressivePosture;

	ArrayWrapper<char> wrapm_paeLastTurnMilitaryAggressivePosture(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryAggressivePosture);
	kStream >> wrapm_paeLastTurnMilitaryAggressivePosture;

	ArrayWrapper<char> wrapm_paeExpansionAggressivePosture(MAX_CIV_PLAYERS, m_paeExpansionAggressivePosture);
	kStream >> wrapm_paeExpansionAggressivePosture;

	ArrayWrapper<char> wrapm_paePlotBuyingAggressivePosture(MAX_CIV_PLAYERS, m_paePlotBuyingAggressivePosture);
	kStream >> wrapm_paePlotBuyingAggressivePosture;

	ArrayWrapper<char> wrapm_paeMilitaryThreat(MAX_CIV_PLAYERS, m_paeMilitaryThreat);
	kStream >> wrapm_paeMilitaryThreat;

	ArrayWrapper<char> wrapm_paeWarmongerThreat(MAX_CIV_PLAYERS, m_paeWarmongerThreat);
	kStream >> wrapm_paeWarmongerThreat;

	ArrayWrapper<bool> wrapm_pabPlayerNoSettleRequestAccepted(MAX_MAJOR_CIVS, m_pabPlayerNoSettleRequestAccepted);
	kStream >> wrapm_pabPlayerNoSettleRequestAccepted;

	ArrayWrapper<short> wrapm_paiPlayerNoSettleRequestCounter(MAX_MAJOR_CIVS, m_paiPlayerNoSettleRequestCounter);
	kStream >> wrapm_paiPlayerNoSettleRequestCounter;

	if(uiVersion >= 15)
	{
		ArrayWrapper<bool> wrapm_pabPlayerStopSpyingRequestAccepted(MAX_MAJOR_CIVS, m_pabPlayerStopSpyingRequestAccepted);
		kStream >> wrapm_pabPlayerStopSpyingRequestAccepted;

		ArrayWrapper<short> wrapm_paiPlayerStopSpyingRequestCounter(MAX_MAJOR_CIVS, m_paiPlayerStopSpyingRequestCounter);
		kStream >> wrapm_paiPlayerStopSpyingRequestCounter;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_pabPlayerStopSpyingRequestAccepted[iI] = false;
			m_paiPlayerStopSpyingRequestCounter[iI] = -1;
		}
	}

	ArrayWrapper<short> wrapm_paiDemandCounter(MAX_MAJOR_CIVS, m_paiDemandCounter);
	kStream >> wrapm_paiDemandCounter;

	ArrayWrapper<short> wrapm_paiDemandTooSoonNumTurns(MAX_MAJOR_CIVS, m_paiDemandTooSoonNumTurns);
	kStream >> wrapm_paiDemandTooSoonNumTurns;

	ArrayWrapper<bool> wrapm_pabDoFAccepted(MAX_MAJOR_CIVS, m_pabDoFAccepted);
	kStream >> wrapm_pabDoFAccepted;

	ArrayWrapper<short> wrapm_paiDoFCounter(MAX_MAJOR_CIVS, m_paiDoFCounter);
	kStream >> wrapm_paiDoFCounter;

	if(uiVersion >= 3)
	{
		ArrayWrapper<bool> wrapm_pabDenouncedPlayer(MAX_MAJOR_CIVS, m_pabDenouncedPlayer);
		kStream >> wrapm_pabDenouncedPlayer;

		ArrayWrapper<bool> wrapm_pabFriendDenouncedUs(MAX_MAJOR_CIVS, m_pabFriendDenouncedUs);
		kStream >> wrapm_pabFriendDenouncedUs;

		ArrayWrapper<short> wrapm_paiDenouncedPlayerCounter(MAX_MAJOR_CIVS, m_paiDenouncedPlayerCounter);
		kStream >> wrapm_paiDenouncedPlayerCounter;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_pabDenouncedPlayer[iI] = false;
			m_pabFriendDenouncedUs[iI] = false;
			m_paiDenouncedPlayerCounter[iI] = 0;
		}
	}

	if(uiVersion >= 6)
	{
		ArrayWrapper<bool> wrapm_pabFriendDeclaredWarOnUs(MAX_MAJOR_CIVS, m_pabFriendDeclaredWarOnUs);
		kStream >> wrapm_pabFriendDeclaredWarOnUs;

		ArrayWrapper<short> wrapm_paiNumRequestsRefused(MAX_MAJOR_CIVS, m_paiNumRequestsRefused);
		kStream >> wrapm_paiNumRequestsRefused;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_pabFriendDeclaredWarOnUs[iI] = false;
			m_paiNumRequestsRefused[iI] = 0;
		}
	}

	ArrayWrapper<short> wrapm_paiNumCiviliansReturnedToMe(MAX_MAJOR_CIVS, m_paiNumCiviliansReturnedToMe);
	kStream >> wrapm_paiNumCiviliansReturnedToMe;

	if(uiVersion >= 11)
	{
		ArrayWrapper<short> wrapm_paiResurrected(MAX_MAJOR_CIVS, m_paiResurrectedOnTurn);
		kStream >> wrapm_paiResurrected;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiResurrectedOnTurn[iI] = -1;
		}
	}

	if(uiVersion >= 2)
	{
		ArrayWrapper<short> wrapm_paiNumTimesCultureBombed(MAX_MAJOR_CIVS, m_paiNumTimesCultureBombed);
		kStream >> wrapm_paiNumTimesCultureBombed;

	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiNumTimesCultureBombed[iI] = 0;
		}
	}

	if(uiVersion >= 16)
	{
		ArrayWrapper<short> wrapm_paiNegativeReligiousConversionPoints(MAX_MAJOR_CIVS, m_paiNegativeReligiousConversionPoints);
		kStream >> wrapm_paiNegativeReligiousConversionPoints;

	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiNegativeReligiousConversionPoints[iI] = 0;
		}
	}

	if(uiVersion >= 7)
	{
		ArrayWrapper<short> wrapm_paiNumTimesNuked(MAX_MAJOR_CIVS, m_paiNumTimesNuked);
		kStream >> wrapm_paiNumTimesNuked;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiNumTimesNuked[iI] = 0;
		}
	}

	if(uiVersion >= 13)
	{
		ArrayWrapper<short> wrapm_paiNumTimesRobbedBy(MAX_MAJOR_CIVS, m_paiNumTimesRobbedBy);
		kStream >> wrapm_paiNumTimesRobbedBy;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiNumTimesRobbedBy[iI] = 0;
		}
	}

	if(uiVersion >= 17)
	{
		ArrayWrapper<short> wrapm_paiNumTimesIntrigueSharedBy(MAX_MAJOR_CIVS, m_paiNumTimesIntrigueSharedBy);
		kStream >> wrapm_paiNumTimesIntrigueSharedBy;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiNumTimesIntrigueSharedBy[iI] = 0;
		}
	}

	if (uiVersion >= 22)
	{
		ArrayWrapper<short> wrapm_paiBrokenExpansionPromiseValue(MAX_MAJOR_CIVS, m_paiBrokenExpansionPromiseValue);
		kStream >> wrapm_paiBrokenExpansionPromiseValue;
		ArrayWrapper<short> wrapm_paiIgnoredExpansionPromiseValue(MAX_MAJOR_CIVS, m_paiIgnoredExpansionPromiseValue);
		kStream >> wrapm_paiIgnoredExpansionPromiseValue;		
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiBrokenExpansionPromiseValue[iI] = 0;
			m_paiIgnoredExpansionPromiseValue[iI] = 0;
		}
	}

	if (uiVersion >= 23)
	{
		ArrayWrapper<short> wrapm_paiBrokenBorderPromiseValue(MAX_MAJOR_CIVS, m_paiBrokenBorderPromiseValue);
		kStream >> wrapm_paiBrokenBorderPromiseValue;
		ArrayWrapper<short> wrapm_paiIgnoredBorderPromiseValue(MAX_MAJOR_CIVS, m_paiIgnoredBorderPromiseValue);
		kStream >> wrapm_paiIgnoredBorderPromiseValue;		
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiBrokenBorderPromiseValue[iI] = 0;
			m_paiIgnoredBorderPromiseValue[iI] = 0;
		}
	}

	if (uiVersion >= 25)
	{
		ArrayWrapper<short> wrapm_aiDeclaredWarOnFriendValue(MAX_MAJOR_CIVS, m_paiDeclaredWarOnFriendValue);
		kStream >> wrapm_aiDeclaredWarOnFriendValue;
	}
	else
	{
		for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiDeclaredWarOnFriendValue[iI] = 0;
		}
	}

	if(uiVersion >= 8)
	{
		ArrayWrapper<short> wrapm_paiTradeValue(MAX_MAJOR_CIVS, m_paiTradeValue);
		kStream >> wrapm_paiTradeValue;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiTradeValue[iI] = 0;
		}
	}

	if(uiVersion >= 9)
	{
		ArrayWrapper<short> wrapm_paiCommonFoeValue(MAX_MAJOR_CIVS, m_paiCommonFoeValue);
		kStream >> wrapm_paiCommonFoeValue;
		ArrayWrapper<short> wrapm_paiAssistValue(MAX_MAJOR_CIVS, m_paiAssistValue);
		kStream >> wrapm_paiAssistValue;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiCommonFoeValue[iI] = 0;
			m_paiAssistValue[iI] = 0;
		}
	}

	ArrayWrapper<bool> wrapm_pabPlayerMadeMilitaryPromise(MAX_MAJOR_CIVS, m_pabPlayerMadeMilitaryPromise);
	kStream >> wrapm_pabPlayerMadeMilitaryPromise;

	ArrayWrapper<bool> wrapm_pabPlayerBrokenMilitaryPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenMilitaryPromise);
	kStream >> wrapm_pabPlayerBrokenMilitaryPromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredMilitaryPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredMilitaryPromise);
	kStream >> wrapm_pabPlayerIgnoredMilitaryPromise;

	ArrayWrapper<short> wrapm_paiPlayerMilitaryPromiseCounter(MAX_MAJOR_CIVS, m_paiPlayerMilitaryPromiseCounter);
	kStream >> wrapm_paiPlayerMilitaryPromiseCounter;

	if (uiVersion >= 24)
	{
		ArrayWrapper<short> wrapm_paiPlayerMadeExpansionPromiseTurn(MAX_MAJOR_CIVS, m_paiPlayerMadeExpansionPromiseTurn);
		kStream >> wrapm_paiPlayerMadeExpansionPromiseTurn;		
	}
	else
	{
		bool abUnusedArray[MAX_MAJOR_CIVS];
		ArrayWrapper<bool> wrapm_pabUnused(MAX_MAJOR_CIVS, abUnusedArray);
		kStream >> wrapm_pabUnused;
		for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			m_paiPlayerMadeExpansionPromiseTurn[ui] = -1;
		}
	}

	ArrayWrapper<bool> wrapm_pabPlayerBrokenExpansionPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenExpansionPromise);
	kStream >> wrapm_pabPlayerBrokenExpansionPromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredExpansionPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredExpansionPromise);
	kStream >> wrapm_pabPlayerIgnoredExpansionPromise;

	ArrayWrapper<char> wrapm_paePlayerExpansionPromiseData(MAX_MAJOR_CIVS, m_paePlayerExpansionPromiseData);
	kStream >> wrapm_paePlayerExpansionPromiseData;

	if (uiVersion >= 24)
	{
		ArrayWrapper<short> wrapm_paiPlayerMadeBorderPromiseTurn(MAX_MAJOR_CIVS, m_paiPlayerMadeBorderPromiseTurn);
		kStream >> wrapm_paiPlayerMadeBorderPromiseTurn;
	}
	else
	{
		bool abUnusedArray[MAX_MAJOR_CIVS];
		ArrayWrapper<bool> wrapm_pabUnused(MAX_MAJOR_CIVS, abUnusedArray);
		kStream >> wrapm_pabUnused;
		for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			m_paiPlayerMadeBorderPromiseTurn[ui] = -1;
		}
	}

	ArrayWrapper<bool> wrapm_pabPlayerBrokenBorderPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenBorderPromise);
	kStream >> wrapm_pabPlayerBrokenBorderPromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredBorderPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredBorderPromise);
	kStream >> wrapm_pabPlayerIgnoredBorderPromise;

	ArrayWrapper<char> wrapm_paePlayerBorderPromiseData(MAX_MAJOR_CIVS, m_paePlayerBorderPromiseData);
	kStream >> wrapm_paePlayerBorderPromiseData;

	ArrayWrapper<bool> wrapm_pabPlayerMadeAttackCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerMadeAttackCityStatePromise);
	kStream >> wrapm_pabPlayerMadeAttackCityStatePromise;

	ArrayWrapper<bool> wrapm_pabPlayerBrokenAttackCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenAttackCityStatePromise);
	kStream >> wrapm_pabPlayerBrokenAttackCityStatePromise;

	ArrayWrapper<bool> wrapm_pabPlayerIgnoredAttackCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredAttackCityStatePromise);
	kStream >> wrapm_pabPlayerIgnoredAttackCityStatePromise;

	if(uiVersion >= 12)
	{
		ArrayWrapper<bool> wrapm_pabPlayerMadeBullyCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerMadeBullyCityStatePromise);
		kStream >> wrapm_pabPlayerMadeBullyCityStatePromise;

		ArrayWrapper<bool> wrapm_pabPlayerBrokenBullyCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenBullyCityStatePromise);
		kStream >> wrapm_pabPlayerBrokenBullyCityStatePromise;

		ArrayWrapper<bool> wrapm_pabPlayerIgnoredBullyCityStatePromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredBullyCityStatePromise);
		kStream >> wrapm_pabPlayerIgnoredBullyCityStatePromise;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_pabPlayerMadeBullyCityStatePromise[iI] = false;
			m_pabPlayerBrokenBullyCityStatePromise[iI] = false;
			m_pabPlayerIgnoredBullyCityStatePromise[iI] = false;
		}
	}

	if(uiVersion >= 14)
	{
		ArrayWrapper<bool> wrapm_pabPlayerMadeSpyPromise(MAX_MAJOR_CIVS, m_pabPlayerMadeSpyPromise);
		kStream >> wrapm_pabPlayerMadeSpyPromise;

		ArrayWrapper<bool> wrapm_pabPlayerBrokenSpyPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenSpyPromise);
		kStream >> wrapm_pabPlayerBrokenSpyPromise;

		ArrayWrapper<bool> wrapm_pabPlayerIgnoredSpyPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredSpyPromise);
		kStream >> wrapm_pabPlayerIgnoredSpyPromise;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_pabPlayerMadeSpyPromise[iI] = false;
			m_pabPlayerBrokenSpyPromise[iI] = false;
			m_pabPlayerIgnoredSpyPromise[iI] = false;
		}
	}

	if(uiVersion >= 19)
	{
		ArrayWrapper<bool> wrapm_pabPlayerForgaveForSpying(MAX_MAJOR_CIVS, m_pabPlayerForgaveForSpying);
		kStream >> wrapm_pabPlayerForgaveForSpying;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_pabPlayerForgaveForSpying[iI] = false;
		}
	}

	if(uiVersion >= 18)
	{
		ArrayWrapper<bool> wrapm_pabPlayerMadeNoConvertPromise(MAX_MAJOR_CIVS, m_pabPlayerMadeNoConvertPromise);
		kStream >> wrapm_pabPlayerMadeNoConvertPromise;

		ArrayWrapper<bool> wrapm_pabPlayerBrokenNoConvertPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenNoConvertPromise);
		kStream >> wrapm_pabPlayerBrokenNoConvertPromise;

		ArrayWrapper<bool> wrapm_pabPlayerIgnoredNoConvertPromise(MAX_MAJOR_CIVS, m_pabPlayerIgnoredNoConvertPromise);
		kStream >> wrapm_pabPlayerIgnoredNoConvertPromise;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_pabPlayerMadeNoConvertPromise[iI] = false;
			m_pabPlayerBrokenNoConvertPromise[iI] = false;
			m_pabPlayerIgnoredNoConvertPromise[iI] = false;
		}
	}

	if (uiVersion >= 30)
	{
		ArrayWrapper<bool> wrapm_pabPlayerAskedNotToConvert(MAX_MAJOR_CIVS, m_pabPlayerAskedNotToConvert);
		kStream >> wrapm_pabPlayerAskedNotToConvert;

		ArrayWrapper<bool> wrapm_pabPlayerAgreedNotToConvert(MAX_MAJOR_CIVS, m_pabPlayerAgreedNotToConvert);
		kStream >> wrapm_pabPlayerAgreedNotToConvert;
	}
	else
	{
		for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_pabPlayerAskedNotToConvert[iI] = false;
			m_pabPlayerAgreedNotToConvert[iI] = false;
		}
	}

	ArrayWrapper<bool> wrapm_pabPlayerBrokenCoopWarPromise(MAX_MAJOR_CIVS, m_pabPlayerBrokenCoopWarPromise);
	kStream >> wrapm_pabPlayerBrokenCoopWarPromise;


	kStream >> m_eDemandTargetPlayer;
	kStream >> m_bDemandReady;

	kStream >> m_iVictoryCompetitiveness;
	kStream >> m_iWonderCompetitiveness;
	kStream >> m_iMinorCivCompetitiveness;
	kStream >> m_iBoldness;
	kStream >> m_iDiploBalance;
	kStream >> m_iWarmongerHate;
	kStream >> m_iDenounceWillingness;
	kStream >> m_iDoFWillingness;

	if(uiVersion >= 5)
	{
		kStream >> m_iLoyalty;
		kStream >> m_iNeediness;
		kStream >> m_iForgiveness;
		kStream >> m_iChattiness;
		kStream >> m_iMeanness;
	}
	else
	{
		// Fill in some defaults based on some educated guesses
		m_iLoyalty = m_iDoFWillingness;
		m_iNeediness = m_iWarmongerHate;
		m_iForgiveness = m_iDoFWillingness;
		m_iChattiness = m_iDoFWillingness;
		m_iMeanness = m_iBoldness;
	}

	ArrayWrapper<char> wrapm_paiPersonalityMajorCivApproachBiases(NUM_MAJOR_CIV_APPROACHES, m_paiPersonalityMajorCivApproachBiases);
	kStream >> wrapm_paiPersonalityMajorCivApproachBiases;

	if(uiVersion <= 9)
	{
		// Versions 9 and less only had 4 approaches
		ArrayWrapper<char> wrapm_paiPersonalityMinorCivApproachBiases(4, m_paiPersonalityMinorCivApproachBiases);
		kStream >> wrapm_paiPersonalityMinorCivApproachBiases;
	}
	else
	{
		ArrayWrapper<char> wrapm_paiPersonalityMinorCivApproachBiases(NUM_MINOR_CIV_APPROACHES, m_paiPersonalityMinorCivApproachBiases);
		kStream >> wrapm_paiPersonalityMinorCivApproachBiases;
	}

	ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceAttackedProtectedMinor(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceAttackedProtectedMinor);
	kStream >> wrapm_paiOtherPlayerTurnsSinceAttackedProtectedMinor;

	ArrayWrapper<char> wrapm_paiOtherPlayerProtectedMinorAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorAttacked);
	kStream >> wrapm_paiOtherPlayerProtectedMinorAttacked;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumProtectedMinorsAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsAttacked);
	kStream >> wrapm_paiOtherPlayerNumProtectedMinorsAttacked;

	ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceKilledProtectedMinor(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceKilledProtectedMinor);
	kStream >> wrapm_paiOtherPlayerTurnsSinceKilledProtectedMinor;

	ArrayWrapper<char> wrapm_paiOtherPlayerProtectedMinorKilled(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorKilled);
	kStream >> wrapm_paiOtherPlayerProtectedMinorKilled;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumProtectedMinorsKilled(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsKilled);
	kStream >> wrapm_paiOtherPlayerNumProtectedMinorsKilled;

	if(uiVersion >= 20)
	{
		ArrayWrapper<char> wrapm_paiOtherPlayerNumProtectedMinorsBullied(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsBullied);
		kStream >> wrapm_paiOtherPlayerNumProtectedMinorsBullied;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiOtherPlayerNumProtectedMinorsBullied[iI] = 0;
		}
	}

	if (uiVersion >= 21)
	{
		ArrayWrapper<short> wrapm_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor);
		kStream >> wrapm_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor;
	}
	else
	{
		for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[iI] = -1;
		}
	}

	ArrayWrapper<char> wrapm_paiOtherPlayerNumMinorsAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsAttacked);
	kStream >> wrapm_paiOtherPlayerNumMinorsAttacked;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumMinorsConquered(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsConquered);
	kStream >> wrapm_paiOtherPlayerNumMinorsConquered;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumMajorsAttacked(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsAttacked);
	kStream >> wrapm_paiOtherPlayerNumMajorsAttacked;

	ArrayWrapper<char> wrapm_paiOtherPlayerNumMajorsConquered(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsConquered);
	kStream >> wrapm_paiOtherPlayerNumMajorsConquered;

	if (uiVersion >= 27)
	{
		ArrayWrapper<int> wrapm_paiOtherPlayerWarmongerAmount(MAX_MAJOR_CIVS, m_paiOtherPlayerWarmongerAmount);
		kStream >> wrapm_paiOtherPlayerWarmongerAmount;
	}
	else
	{
		for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiOtherPlayerWarmongerAmount[iI] = 0;
		}
	}

	if (uiVersion >= 28)
	{
		ArrayWrapper<short> wrapm_paiNumCitiesLiberated(MAX_MAJOR_CIVS, m_paiNumCitiesLiberated);
		kStream >> wrapm_paiNumCitiesLiberated;
	}
	else
	{
		for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		{
			m_paiNumCitiesLiberated[iI] = 0;
		}
	}

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		ArrayWrapper<DiploLogData> wrapm_ppaDiploStatementsLog(MAX_DIPLO_LOG_STATEMENTS, m_ppaDiploStatementsLog[iI]);
		kStream >> wrapm_ppaDiploStatementsLog;
	}

	ArrayWrapper<DeclarationLogData> wrapm_paDeclarationsLog(MAX_DIPLO_LOG_STATEMENTS, m_paDeclarationsLog);
	kStream >> wrapm_paDeclarationsLog;
	kStream >> m_eStateAllWars;
}

/// Serialization write
void CvDiplomacyAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 30;
	kStream << uiVersion;

	int iI;

	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paeMajorCivOpinion);

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_ppaaeOtherPlayerMajorCivOpinion[iI]);
		kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_ppaabWorkingAgainstPlayerAccepted[iI]);
		kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_ppaaiWorkingAgainstPlayerCounter[iI]);
		kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_ppaacCoopWarAcceptedState[iI]);
		kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_ppaaiCoopWarCounter[iI]);
	}

	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paeMajorCivApproach);
	kStream << ArrayWrapper<char>(MAX_MINOR_CIVS, m_paeMinorCivApproach);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuess);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paeApproachTowardsUsGuessCounter);

	kStream << ArrayWrapper<short>(MAX_CIV_PLAYERS, m_paiNumWondersBeatenTo);
	kStream << ArrayWrapper<bool>(MAX_CIV_PLAYERS, m_pabMusteringForAttack);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabWantsResearchAgreementWithPlayer);
	kStream << ArrayWrapper<bool>(MAX_MINOR_CIVS, m_pabWantToRouteToMinor);

	kStream << ArrayWrapper<short>(MAX_CIV_PLAYERS, m_paeWantPeaceCounter);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToOffer);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paePeaceTreatyWillingToAccept);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarFace);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarState);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarProjection);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeLastWarProjection);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarGoal);
	kStream << ArrayWrapper<short>(MAX_CIV_PLAYERS, m_paiPlayerNumTurnsAtWar);
	kStream << ArrayWrapper<short>(MAX_CIV_PLAYERS, m_paiNumWarsFought);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerMilitaryStrengthComparedToUs);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerEconomicStrengthComparedToUs);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerTargetValue);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerLandDisputeLevel);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerLastTurnLandDisputeLevel);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerVictoryDisputeLevel);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerWonderDisputeLevel);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlayerMinorCivDisputeLevel);

	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerLandDisputeLevel[iI]);
		kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerVictoryDisputeLevel[iI]);

		kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerWarDamageLevel[iI]);
		kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_ppaaiOtherPlayerWarValueLost[iI]);

		kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_ppaaeOtherPlayerMilitaryThreat[iI]);
	}

	// Added in Version 29
	for(iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_ppaaiOtherPlayerLastRoundWarValueLost[iI]);
		kStream << ArrayWrapper<bool>(MAX_CIV_PLAYERS, m_ppaabSentAttackMessageToMinorCivProtector[iI]);
	}

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarDamageLevel);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paiWarValueLost);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeMilitaryAggressivePosture);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryAggressivePosture);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeExpansionAggressivePosture);
	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paePlotBuyingAggressivePosture);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeMilitaryThreat);

	kStream << ArrayWrapper<char>(MAX_CIV_PLAYERS, m_paeWarmongerThreat);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerNoSettleRequestAccepted);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerNoSettleRequestCounter);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerStopSpyingRequestAccepted);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerStopSpyingRequestCounter);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDemandCounter);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDemandTooSoonNumTurns);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabDoFAccepted);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDoFCounter);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabDenouncedPlayer);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabFriendDenouncedUs);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDenouncedPlayerCounter);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabFriendDeclaredWarOnUs);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumRequestsRefused);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumCiviliansReturnedToMe);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiResurrectedOnTurn);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumTimesCultureBombed);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNegativeReligiousConversionPoints);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumTimesNuked);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumTimesRobbedBy);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumTimesIntrigueSharedBy);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiBrokenExpansionPromiseValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiIgnoredExpansionPromiseValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiBrokenBorderPromiseValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiIgnoredBorderPromiseValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiDeclaredWarOnFriendValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiTradeValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiCommonFoeValue);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiAssistValue);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeMilitaryPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenMilitaryPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredMilitaryPromise);
	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerMilitaryPromiseCounter);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerMadeExpansionPromiseTurn);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenExpansionPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredExpansionPromise);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paePlayerExpansionPromiseData);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiPlayerMadeBorderPromiseTurn);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenBorderPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredBorderPromise);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paePlayerBorderPromiseData);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeAttackCityStatePromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenAttackCityStatePromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredAttackCityStatePromise);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeBullyCityStatePromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenBullyCityStatePromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredBullyCityStatePromise);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeSpyPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenSpyPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredSpyPromise);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerForgaveForSpying);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerMadeNoConvertPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenNoConvertPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerIgnoredNoConvertPromise);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerAskedNotToConvert);
	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerAgreedNotToConvert);

	kStream << ArrayWrapper<bool>(MAX_MAJOR_CIVS, m_pabPlayerBrokenCoopWarPromise);

	kStream << m_eDemandTargetPlayer;
	kStream << m_bDemandReady;

	kStream << m_iVictoryCompetitiveness;
	kStream << m_iWonderCompetitiveness;
	kStream << m_iMinorCivCompetitiveness;
	kStream << m_iBoldness;
	kStream << m_iDiploBalance;
	kStream << m_iWarmongerHate;
	kStream << m_iDenounceWillingness;
	kStream << m_iDoFWillingness;
	kStream << m_iLoyalty;
	kStream << m_iNeediness;
	kStream << m_iForgiveness;
	kStream << m_iChattiness;
	kStream << m_iMeanness;

	kStream << ArrayWrapper<char>(NUM_MAJOR_CIV_APPROACHES, m_paiPersonalityMajorCivApproachBiases);
	kStream << ArrayWrapper<char>(NUM_MINOR_CIV_APPROACHES, m_paiPersonalityMinorCivApproachBiases);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceAttackedProtectedMinor);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorAttacked);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsAttacked);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceKilledProtectedMinor);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerProtectedMinorKilled);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsKilled);

	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumProtectedMinorsBullied);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor);

	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsAttacked);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMinorsConquered);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsAttacked);
	kStream << ArrayWrapper<char>(MAX_MAJOR_CIVS, m_paiOtherPlayerNumMajorsConquered);

	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_paiOtherPlayerWarmongerAmount);

	kStream << ArrayWrapper<short>(MAX_MAJOR_CIVS, m_paiNumCitiesLiberated);

	for(iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		kStream << ArrayWrapper<DiploLogData>(MAX_DIPLO_LOG_STATEMENTS, m_ppaDiploStatementsLog[iI]);
	}

	kStream << ArrayWrapper<DeclarationLogData>(MAX_DIPLO_LOG_STATEMENTS, m_paDeclarationsLog);
	kStream << m_eStateAllWars;
}

//	-----------------------------------------------------------------------------------------------
void CvDiplomacyAI::update()
{
	if(!m_aGreetPlayers.empty())
	{
		PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
		// In out list?
		PlayerTypesArray::iterator itr = std::find(m_aGreetPlayers.begin(), m_aGreetPlayers.end(), eActivePlayer);
		if(itr != m_aGreetPlayers.end())
		{
			m_aGreetPlayers.erase(itr);

			const char* szText = GetDiploStringForMessage(DIPLO_MESSAGE_INTRO);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), eActivePlayer, DIPLO_UI_STATE_DEFAULT_ROOT, szText, LEADERHEAD_ANIM_INTRO);
		}
	}
}
//	-----------------------------------------------------------------------------------------------
/// Returns the Player object this class is associated with
CvPlayer* CvDiplomacyAI::GetPlayer()
{
	return m_pPlayer;
}

/// Returns the Team ID this AI's player is associated with
TeamTypes CvDiplomacyAI::GetTeam()
{
	return m_pPlayer->getTeam();
}


/// Initializes Personality Members for this player (XML value + random element)
void CvDiplomacyAI::DoInitializePersonality()
{
	// AI Player
	if(!GetPlayer()->isHuman())
	{
		CvLeaderHeadInfo& playerLeaderInfo = GetPlayer()->getLeaderInfo();

		m_iVictoryCompetitiveness = GetRandomPersonalityWeight(playerLeaderInfo.GetVictoryCompetitiveness());
		CvAssertMsg(m_iVictoryCompetitiveness >= 0, "DIPLOMACY_AI: Victory Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iWonderCompetitiveness = GetRandomPersonalityWeight(playerLeaderInfo.GetWonderCompetitiveness());
		CvAssertMsg(m_iWonderCompetitiveness >= 0, "DIPLOMACY_AI: Victory Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iMinorCivCompetitiveness = GetRandomPersonalityWeight(playerLeaderInfo.GetMinorCivCompetitiveness());
		CvAssertMsg(m_iMinorCivCompetitiveness >= 0, "DIPLOMACY_AI: Minor Civ Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iBoldness = GetRandomPersonalityWeight(playerLeaderInfo.GetBoldness());
		CvAssertMsg(m_iBoldness >= 0, "DIPLOMACY_AI: Boldness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDiploBalance = GetRandomPersonalityWeight(playerLeaderInfo.GetDiploBalance());
		CvAssertMsg(m_iDiploBalance >= 0, "DIPLOMACY_AI: DiploBalance Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iWarmongerHate = GetRandomPersonalityWeight(playerLeaderInfo.GetWarmongerHate());
		CvAssertMsg(m_iWarmongerHate >= 0, "DIPLOMACY_AI: WarmongerHate Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDenounceWillingness = GetRandomPersonalityWeight(playerLeaderInfo.GetDenounceWillingness());
		CvAssertMsg(m_iDenounceWillingness >= 0, "DIPLOMACY_AI: DenounceWillingness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDoFWillingness = GetRandomPersonalityWeight(playerLeaderInfo.GetDoFWillingness());
		CvAssertMsg(m_iDoFWillingness >= 0, "DIPLOMACY_AI: DoFWillingness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iLoyalty = GetRandomPersonalityWeight(playerLeaderInfo.GetLoyalty());
		CvAssertMsg(m_iLoyalty >= 0, "DIPLOMACY_AI: Loyalty Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iNeediness = GetRandomPersonalityWeight(playerLeaderInfo.GetNeediness());
		CvAssertMsg(m_iNeediness >= 0, "DIPLOMACY_AI: Neediness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iForgiveness = GetRandomPersonalityWeight(playerLeaderInfo.GetForgiveness());
		CvAssertMsg(m_iForgiveness >= 0, "DIPLOMACY_AI: Forgiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iChattiness = GetRandomPersonalityWeight(playerLeaderInfo.GetChattiness());
		CvAssertMsg(m_iChattiness >= 0, "DIPLOMACY_AI: Chattiness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iMeanness = GetRandomPersonalityWeight(playerLeaderInfo.GetMeanness());
		CvAssertMsg(m_iMeanness >= 0, "DIPLOMACY_AI: Meanness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		for(int iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
		{
			m_paiPersonalityMajorCivApproachBiases[iApproachLoop] = GetRandomPersonalityWeight(playerLeaderInfo.GetMajorCivApproachBias(iApproachLoop));
			CvAssertMsg(m_paiPersonalityMajorCivApproachBiases[iApproachLoop] >= 0, "DIPLOMACY_AI: Personality Approach weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		}

		for(int iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
		{
			m_paiPersonalityMinorCivApproachBiases[iApproachLoop] = GetRandomPersonalityWeight(playerLeaderInfo.GetMinorCivApproachBias(iApproachLoop));
			CvAssertMsg(m_paiPersonalityMinorCivApproachBiases[iApproachLoop] >= 0, "DIPLOMACY_AI: Personality Approach weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		}
	}
	// Human player
	else
	{
		int iDefaultFlavorValue = GC.getDEFAULT_FLAVOR_VALUE();

		m_iVictoryCompetitiveness = iDefaultFlavorValue;
		CvAssertMsg(m_iVictoryCompetitiveness >= 0, "DIPLOMACY_AI: Victory Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iWonderCompetitiveness = iDefaultFlavorValue;
		CvAssertMsg(m_iWonderCompetitiveness >= 0, "DIPLOMACY_AI: Wonder Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iMinorCivCompetitiveness = iDefaultFlavorValue;
		CvAssertMsg(m_iMinorCivCompetitiveness >= 0, "DIPLOMACY_AI: Minor Civ Competitiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iBoldness = iDefaultFlavorValue;
		CvAssertMsg(m_iBoldness >= 0, "DIPLOMACY_AI: Boldness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDiploBalance = iDefaultFlavorValue;
		CvAssertMsg(m_iDiploBalance >= 0, "DIPLOMACY_AI: DiploBalance Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iWarmongerHate = iDefaultFlavorValue;
		CvAssertMsg(m_iWarmongerHate >= 0, "DIPLOMACY_AI: WarmongerHate Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDenounceWillingness = iDefaultFlavorValue;
		CvAssertMsg(m_iDenounceWillingness >= 0, "DIPLOMACY_AI: DenounceWillingness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iDoFWillingness = iDefaultFlavorValue;
		CvAssertMsg(m_iDoFWillingness >= 0, "DIPLOMACY_AI: DoFWillingness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iLoyalty = iDefaultFlavorValue;
		CvAssertMsg(m_iLoyalty >= 0, "DIPLOMACY_AI: Loyalty Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iNeediness = iDefaultFlavorValue;
		CvAssertMsg(m_iNeediness >= 0, "DIPLOMACY_AI: Neediness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iForgiveness = iDefaultFlavorValue;
		CvAssertMsg(m_iForgiveness >= 0, "DIPLOMACY_AI: Forgiveness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iChattiness = iDefaultFlavorValue;
		CvAssertMsg(m_iChattiness >= 0, "DIPLOMACY_AI: Chattiness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		m_iMeanness = iDefaultFlavorValue;
		CvAssertMsg(m_iMeanness >= 0, "DIPLOMACY_AI: Meanness Personality weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

		for(int iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
		{
			m_paiPersonalityMajorCivApproachBiases[iApproachLoop] = iDefaultFlavorValue;
			CvAssertMsg(m_paiPersonalityMajorCivApproachBiases[iApproachLoop] >= 0, "DIPLOMACY_AI: Personality Approach weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		}

		for(int iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
		{
			m_paiPersonalityMinorCivApproachBiases[iApproachLoop] = iDefaultFlavorValue;
			CvAssertMsg(m_paiPersonalityMinorCivApproachBiases[iApproachLoop] >= 0, "DIPLOMACY_AI: Personality Approach weight is negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")
		}
	}

	LogPersonality();
}

/// Returns a personality weight with a small random element
int CvDiplomacyAI::GetRandomPersonalityWeight(int iOriginalValue) const
{
	int iMin = /*0*/ GC.getPERSONALITY_FLAVOR_MIN_VALUE();
	int iMax = /*20*/ GC.getPERSONALITY_FLAVOR_MAX_VALUE();
	int iPlusMinus = /*2*/ GC.getFLAVOR_RANDOMIZATION_RANGE();

	int iAdjust = GC.getGame().getJonRandNum((iPlusMinus * 2 + 1), "Diplomacy AI Random Weight");
	int iRtnValue = iOriginalValue + iAdjust - iPlusMinus;

	if(iRtnValue < iMin)
		iRtnValue = iMin;
	else if(iRtnValue > iMax)
		iRtnValue = iMax;

	return iRtnValue;
}



// ************************************
// Actions/Tests
// ************************************



/// Runs every turn!  The order matters for a lot of this stuff, so be VERY careful about moving anything around (!)
void CvDiplomacyAI::DoTurn(PlayerTypes eTargetPlayer)
{
	m_eTargetPlayer = eTargetPlayer;
	// Military Stuff
	DoWarDamageDecay();
	DoUpdateWarDamageLevel();
	DoUpdatePlayerMilitaryStrengths();
	DoUpdatePlayerEconomicStrengths();

	DoUpdateWarmongerThreats();
	DoUpdateMilitaryThreats();
	DoUpdatePlayerTargetValues();
	DoUpdateWarStates();
	DoUpdateWarProjections();
	DoUpdateWarGoals();

	DoUpdatePeaceTreatyWillingness();

	// Issues of Dispute
	DoUpdateLandDisputeLevels();
	DoUpdateVictoryDisputeLevels();
	DoUpdateWonderDisputeLevels();
	DoUpdateMinorCivDisputeLevels();

	// Has any player gone back on any promises he made?
	DoTestPromises();

	// What we think other Players are up to
	DoUpdateOtherPlayerWarDamageLevel();
	DoUpdateEstimateOtherPlayerLandDisputeLevels();
	DoUpdateEstimateOtherPlayerVictoryDisputeLevels();
	DoUpdateEstimateOtherPlayerMilitaryThreats();
	DoEstimateOtherPlayerOpinions();
	LogOtherPlayerGuessStatus();

	// Look at the situation
	DoUpdateMilitaryAggressivePostures();
	DoUpdateExpansionAggressivePostures();
	DoUpdatePlotBuyingAggressivePosture();

	// Player Opinion & Approach
	DoUpdateApproachTowardsUsGuesses();

	DoUpdateOpinions();
	DoUpdateMajorCivApproaches();
	DoUpdateMinorCivApproaches();

	// These functions actually DO things, and we don't want the shadow AI behind a human player doing things for him
	if(!GetPlayer()->isHuman())
	{
		MakeWar();
		DoMakePeaceWithMinors();

		DoUpdateDemands();

		DoUpdatePlanningExchanges();
		DoContactMinorCivs();
		DoContactMajorCivs();
	}

	// Update Counters
	DoCounters();

	// Save our current war value estimates, so we can compare them with the updated estimates next turn
	CacheOtherPlayerWarValuesLost();

	LogStatus();
	LogWarStatus();
	LogStatements();

	m_eTargetPlayer = NO_PLAYER;
}

/// Increment our turn counters
void CvDiplomacyAI::DoCounters()
{
	int iItem;
	DiploStatementTypes eStatement;
	PublicDeclarationTypes eDeclaration;

	PlayerTypes eThirdPlayer;
	int iThirdPlayerLoop;

	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			// War Counter
			if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				ChangePlayerNumTurnsAtWar(eLoopPlayer, 1);
			// Reset Counter if not at war
			else if(GetPlayerNumTurnsAtWar(eLoopPlayer) > 0)
				SetPlayerNumTurnsAtWar(eLoopPlayer, 0);

			///////////////////////////////
			// Major Civs only!
			///////////////////////////////
			if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				// Trade value counter
				ChangeRecentTradeValue(eLoopPlayer, -GC.getDEAL_VALUE_PER_TURN_DECAY());
				ChangeCommonFoeValue(eLoopPlayer, -GC.getCOMMON_FOE_VALUE_PER_TURN_DECAY());
				if (GetRecentAssistValue(eLoopPlayer) > 0)
				{
					int iMin = MIN(GetRecentAssistValue(eLoopPlayer), GC.getDEAL_VALUE_PER_TURN_DECAY());
					ChangeRecentAssistValue(eLoopPlayer, -iMin);
				}
				else if (GetRecentAssistValue(eLoopPlayer) < 0)
				{
					int iMin = MIN(-GetRecentAssistValue(eLoopPlayer), GC.getDEAL_VALUE_PER_TURN_DECAY());
					ChangeRecentAssistValue(eLoopPlayer, iMin);
				}
				
				int iBrokenPromisePreValue = GetBrokenExpansionPromiseValue(eLoopPlayer);
				ChangeBrokenExpansionPromiseValue(eLoopPlayer, -GC.getEXPANSION_PROMISE_BROKEN_PER_TURN_DECAY());
				int iIgnoredPromisePreValue = GetIgnoredExpansionPromiseValue(eLoopPlayer);
				ChangeIgnoredExpansionPromiseValue(eLoopPlayer, -GC.getEXPANSION_PROMISE_IGNORED_PER_TURN_DECAY());

				// if the promise penalty of breaking a promise has expired, reset the ability to ask the promise again
				if ((iBrokenPromisePreValue != 0 && GetBrokenExpansionPromiseValue(eLoopPlayer) == 0 && GetIgnoredExpansionPromiseValue(eLoopPlayer) == 0) || 
					(iIgnoredPromisePreValue != 0 && GetIgnoredExpansionPromiseValue(eLoopPlayer) == 0 && GetBrokenExpansionPromiseValue(eLoopPlayer) == 0))
				{
					SetPlayerMadeExpansionPromise(eLoopPlayer, false);
					SetPlayerBrokenExpansionPromise(eLoopPlayer, false);
					SetPlayerIgnoredExpansionPromise(eLoopPlayer, false);
				}

				iBrokenPromisePreValue = GetBrokenBorderPromiseValue(eLoopPlayer);
				ChangeBrokenBorderPromiseValue(eLoopPlayer, -GC.getBORDER_PROMISE_BROKEN_PER_TURN_DECAY());
				iIgnoredPromisePreValue = GetIgnoredBorderPromiseValue(eLoopPlayer);
				ChangeIgnoredBorderPromiseValue(eLoopPlayer, -GC.getBORDER_PROMISE_IGNORED_PER_TURN_DECAY());

				// if the promise penalty of breaking a promise has expired, reset the ability to ask the promise again
				if ((iBrokenPromisePreValue != 0 && GetBrokenBorderPromiseValue(eLoopPlayer) == 0 && GetIgnoredBorderPromiseValue(eLoopPlayer) == 0) || 
					(iIgnoredPromisePreValue != 0 && GetIgnoredBorderPromiseValue(eLoopPlayer) == 0 && GetBrokenBorderPromiseValue(eLoopPlayer) == 0))
				{
					SetPlayerMadeBorderPromise(eLoopPlayer, false);
					SetPlayerBrokenBorderPromise(eLoopPlayer, false);
					SetPlayerIgnoredBorderPromise(eLoopPlayer, false);
				}

				ChangeDeclaredWarOnFriendValue(eLoopPlayer, -GC.getDECLARED_WAR_ON_FRIEND_PER_TURN_DECAY());

				// Diplo Statement Log Counter
				for(iItem = 0; iItem < MAX_DIPLO_LOG_STATEMENTS; iItem++)
				{
					eStatement = GetDiploLogStatementTypeForIndex(eLoopPlayer, iItem);

					if(eStatement != NO_DIPLO_STATEMENT_TYPE)
						ChangeDiploLogStatementTurnForIndex(eLoopPlayer, iItem, 1);
					else
						SetDiploLogStatementTurnForIndex(eLoopPlayer, iItem, 0);
				}

				// Attacked Protected Minor Counter
				if(GetOtherPlayerProtectedMinorAttacked(eLoopPlayer) != NO_PLAYER)
					ChangeOtherPlayerTurnsSinceAttackedProtectedMinor(eLoopPlayer, 1);

				// Killed Protected Minor Counter
				if(GetOtherPlayerProtectedMinorKilled(eLoopPlayer) != NO_PLAYER)
					ChangeOtherPlayerTurnsSinceKilledProtectedMinor(eLoopPlayer, 1);

				// They sided with their Protected Minor Counter
				if(IsOtherPlayerSidedWithProtectedMinor(eLoopPlayer))
					ChangeOtherPlayerTurnsSinceSidedWithProtectedMinor(eLoopPlayer, 1);

				// Did this player ask us not to settle near them?
				if(GetPlayerNoSettleRequestCounter(eLoopPlayer) > -1)
				{
					ChangePlayerNoSettleRequestCounter(eLoopPlayer, 1);

					if(GetPlayerNoSettleRequestCounter(eLoopPlayer) >= /*50*/ GC.getIC_MEMORY_TURN_EXPIRATION())
					{
						SetPlayerNoSettleRequestAccepted(eLoopPlayer, false);
						SetPlayerNoSettleRequestCounter(eLoopPlayer, -666);
					}
				}

				// Did this player ask us to stop spying on them?
				if(GetPlayerStopSpyingRequestCounter(eLoopPlayer) > -1)
				{
					ChangePlayerStopSpyingRequestCounter(eLoopPlayer, 1);

					if(GetPlayerStopSpyingRequestCounter(eLoopPlayer) >= /*50*/ GC.getSTOP_SPYING_MEMORY_TURN_EXPIRATION())
					{
						SetPlayerStopSpyingRequestAccepted(eLoopPlayer, false);
						SetPlayerStopSpyingRequestCounter(eLoopPlayer, -666);
					}
				}

				// Did this player make a demand of us?
				if(GetDemandCounter(eLoopPlayer) > -1)
					ChangeDemandCounter(eLoopPlayer, 1);

				// DoF?
				if(GetDoFCounter(eLoopPlayer) > -1)
					ChangeDoFCounter(eLoopPlayer, 1);

				// Denounced?
				if(GetDenouncedPlayerCounter(eLoopPlayer) > -1)
					ChangeDenouncedPlayerCounter(eLoopPlayer, 1);

				// Are we ready to forget our denunciation?
				if(IsDenouncedPlayer(eLoopPlayer) && GetDenouncedPlayerCounter(eLoopPlayer) >= GC.getGame().getGameSpeedInfo().getRelationshipDuration())
				{
					SetDenouncedPlayer(eLoopPlayer, false);
					SetDenouncedPlayerCounter(eLoopPlayer, -1);
					// Let's become open to becoming friends again
					SetDoFCounter(eLoopPlayer, -1);

					// They no longer hate us either
					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetDoFCounter(GetPlayer()->GetID(), -1);
					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetFriendDenouncedUs(GetPlayer()->GetID(), false);

					for(iThirdPlayerLoop = 0; iThirdPlayerLoop < MAX_MAJOR_CIVS; iThirdPlayerLoop++){
						eThirdPlayer = (PlayerTypes) iThirdPlayerLoop;

						// We may even do co-op wars in the future
						if(GetCoopWarCounter(eLoopPlayer, eThirdPlayer) < -1)
							SetCoopWarCounter(eLoopPlayer, eThirdPlayer, -1);
						GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetCoopWarCounter(GetPlayer()->GetID(), eThirdPlayer, -1);
					}

					//Notify the target of the denouncement that it has expired.
					CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
					if(pNotifications){
						CvString							strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_THEIR_DENUNCIATION_EXPIRED_S");
						Localization::String	strInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_THEIR_DENUNCIATION_EXPIRED");
						Localization::String strTemp = strInfo;
						strTemp << GET_PLAYER(GetPlayer()->GetID()).getCivilizationShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_DENUNCIATION_EXPIRED, strTemp.toUTF8(), strSummary, -1, -1, GetPlayer()->GetID(), eLoopPlayer);
					}
				}

				// Has our Friendship expired?
				if(IsDoFAccepted(eLoopPlayer) && GetDoFCounter(eLoopPlayer) >= GC.getGame().getGameSpeedInfo().getRelationshipDuration())
				{
					SetDoFCounter(eLoopPlayer, -1);
					SetDoFAccepted(eLoopPlayer, false);

					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetDoFCounter(GetPlayer()->GetID(), -1);
					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetDoFAccepted(GetPlayer()->GetID(), false);

					//Notify both parties that our friendship has expired.
					CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
					if (pNotifications){
						CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FRIENDSHIP_EXPIRED", GET_PLAYER(GetPlayer()->GetID()).getCivilizationShortDescriptionKey());
						CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_FRIENDSHIP_EXPIRED_S");
						pNotifications->Add(NOTIFICATION_FRIENDSHIP_EXPIRED, strBuffer, strSummary, -1, -1, GetPlayer()->GetID(), eLoopPlayer);				
					}

					pNotifications = GET_PLAYER(GetPlayer()->GetID()).GetNotifications();
					if (pNotifications){
						CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_FRIENDSHIP_EXPIRED", GET_PLAYER(eLoopPlayer).getCivilizationShortDescriptionKey());
						CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_FRIENDSHIP_EXPIRED_S");
						pNotifications->Add(NOTIFICATION_FRIENDSHIP_EXPIRED, strBuffer, strSummary, -1, -1, eLoopPlayer, GetPlayer()->GetID());				
					}
				}

				// Counters relating to a third party
				for(iThirdPlayerLoop = 0; iThirdPlayerLoop < MAX_MAJOR_CIVS; iThirdPlayerLoop++)
				{
					eThirdPlayer = (PlayerTypes) iThirdPlayerLoop;

					// Coop war
					if(GetCoopWarCounter(eLoopPlayer, eThirdPlayer) >= 0)
					{
						ChangeCoopWarCounter(eLoopPlayer, eThirdPlayer, 1);

						// AI players will always declare war at 10 turns, so we simplify things here - humans are handled by DoCoopWarTimeStatement()
						if(!GET_PLAYER(eLoopPlayer).isHuman())
						{
							if(GetCoopWarAcceptedState(eLoopPlayer, eThirdPlayer) == COOP_WAR_STATE_SOON &&
							        GetCoopWarCounter(eLoopPlayer, eThirdPlayer) == /*10*/ GC.getCOOP_WAR_SOON_COUNTER())
							{
								// Us
								SetCoopWarAcceptedState(eLoopPlayer, eThirdPlayer, COOP_WAR_STATE_ACCEPTED);
								SetCoopWarCounter(eLoopPlayer, eThirdPlayer, 0);
								DeclareWar(eThirdPlayer);
								GetPlayer()->GetMilitaryAI()->RequestShowOfForce(eThirdPlayer);
								//GetPlayer()->GetMilitaryAI()->RequestPillageAttack(eThirdPlayer);

								int iLockedTurns = /*15*/ GC.getCOOP_WAR_LOCKED_LENGTH();
								GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(GET_PLAYER(eThirdPlayer).getTeam(), iLockedTurns);

								// Them - handled as their turn cycles around!
								//GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetCoopWarAcceptedState(eLoopPlayer, eThirdPlayer, COOP_WAR_STATE_ACCEPTED);
								//GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->SetCoopWarCounter(eLoopPlayer, eThirdPlayer, 0);
								//GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->DeclareWar(eThirdPlayer);
							}
						}
					}

					// Working against a player
					//if (GetWorkingAgainstPlayerCounter(eLoopPlayer, eThirdPlayer) >= 0)
					//	ChangeWorkingAgainstPlayerCounter(eLoopPlayer, eThirdPlayer, 1);
				}
			}
		}
	}

	///////////////////////////////
	// Declaration Log Counter
	///////////////////////////////

	for(iItem = 0; iItem < MAX_DIPLO_LOG_STATEMENTS; iItem++)
	{
		eDeclaration = GetDeclarationLogTypeForIndex(iItem);

		if(eDeclaration != NO_PUBLIC_DECLARATION_TYPE)
		{
			ChangeDeclarationLogTurnForIndex(iItem, 1);
		}
		else
		{
			SetDeclarationLogTurnForIndex(iItem, 0);
		}
	}

}



// ************************************
// Player Opinion & Approach
// ************************************



/// What is our basic opinion of the role a player has in our game?
void CvDiplomacyAI::DoUpdateOpinions()
{
	// Loop through all (known) Majors
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerOpinion(eLoopPlayer);
	}
}

/// What is our basic opinion of the role a player has in our game?
void CvDiplomacyAI::DoUpdateOnePlayerOpinion(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(!IsPlayerValid(ePlayer))
		return;

	MajorCivOpinionTypes eOpinion;
	CvPlayer& kPlayerEvaluating = GET_PLAYER(ePlayer);

	// Teammates?
	if(GetPlayer()->getTeam() == kPlayerEvaluating.getTeam())
	{
		eOpinion = MAJOR_CIV_OPINION_ALLY;
	}
	// Different teams
	else
	{
		int iOpinionWeight = GetMajorCivOpinionWeight(ePlayer);

		if(iOpinionWeight >= /*50*/ GC.getOPINION_THRESHOLD_UNFORGIVABLE())
			eOpinion = MAJOR_CIV_OPINION_UNFORGIVABLE;
		else if(iOpinionWeight >= /*30*/ GC.getOPINION_THRESHOLD_ENEMY())
			eOpinion = MAJOR_CIV_OPINION_ENEMY;
		else if(iOpinionWeight >= /*10*/ GC.getOPINION_THRESHOLD_COMPETITOR())
			eOpinion = MAJOR_CIV_OPINION_COMPETITOR;
		else if(iOpinionWeight >= /*-10*/ GC.getOPINION_THRESHOLD_FAVORABLE())
			eOpinion = MAJOR_CIV_OPINION_NEUTRAL;
		else if(iOpinionWeight >= /*-30*/ GC.getOPINION_THRESHOLD_FRIEND())
			eOpinion = MAJOR_CIV_OPINION_FAVORABLE;
		else if(iOpinionWeight >= /*-50*/ GC.getOPINION_THRESHOLD_ALLY())
			eOpinion = MAJOR_CIV_OPINION_FRIEND;
		else
			eOpinion = MAJOR_CIV_OPINION_ALLY;

		// If we've agreed to work against someone, then the worst we can feel towards this guy is enemy
		//if (IsWorkingAgainstPlayer(ePlayer) && eOpinion < MAJOR_CIV_OPINION_COMPETITOR)
		//	eOpinion = MAJOR_CIV_OPINION_COMPETITOR;

	}

	// Finally, set the Opinion
	SetMajorCivOpinion(ePlayer, eOpinion);

	//LogOpinionUpdate(ePlayer, viOpinionWeights);
}

// What is the number value of our opinion towards ePlayer?
int CvDiplomacyAI::GetMajorCivOpinionWeight(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;

	iOpinionWeight += GetLandDisputeLevelScore(ePlayer);
	iOpinionWeight += GetWonderDisputeLevelScore(ePlayer);
	iOpinionWeight += GetMinorCivDisputeLevelScore(ePlayer);
	iOpinionWeight += GetWarmongerThreatScore(ePlayer);

	//////////////////////////////////////
	// Player has done nice stuff
	//////////////////////////////////////
	iOpinionWeight += GetCiviliansReturnedToMeScore(ePlayer);
	iOpinionWeight += GetResurrectedScore(ePlayer);
	iOpinionWeight += GetLiberatedCitiesScore(ePlayer);
	iOpinionWeight += GetEmbassyScore(ePlayer);
	iOpinionWeight += GetForgaveForSpyingScore(ePlayer);
	iOpinionWeight += GetTimesIntrigueSharedScore(ePlayer);

	//////////////////////////////////////
	// Player has asked us to do stuff we don't like
	//////////////////////////////////////
	iOpinionWeight += GetNoSetterRequestScore(ePlayer);
	iOpinionWeight += GetStopSpyingRequestScore(ePlayer);
	iOpinionWeight += GetDemandEverMadeScore(ePlayer);
	iOpinionWeight += GetTimesCultureBombedScore(ePlayer);
	iOpinionWeight += GetReligiousConversionPointsScore(ePlayer);

	iOpinionWeight += GetHasAdoptedHisReligionScore(ePlayer);
	iOpinionWeight += GetHasAdoptedMyReligionScore(ePlayer);
	
	iOpinionWeight += GetSameLatePoliciesScore(ePlayer);
	iOpinionWeight += GetDifferentLatePoliciesScore(ePlayer);
	
	//iOpinionWeight += GetTimesNukedScore(ePlayer); DUPLICATE of GetNukedScore below. Removing this from scoring.
	iOpinionWeight += GetTimesRobbedScore(ePlayer);

	//////////////////////////////////////
	// BROKEN PROMISES ;_;
	//////////////////////////////////////
	iOpinionWeight += GetBrokenMilitaryPromiseScore(ePlayer);
	iOpinionWeight += GetBrokenMilitaryPromiseWithAnybodyScore(ePlayer);
	iOpinionWeight += GetIgnoredMilitaryPromiseScore(ePlayer);
	iOpinionWeight += GetBrokenExpansionPromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredExpansionPromiseScore(ePlayer);
	iOpinionWeight += GetBrokenBorderPromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredBorderPromiseScore(ePlayer);

	iOpinionWeight += GetBrokenAttackCityStatePromiseScore(ePlayer);
	iOpinionWeight += GetBrokenAttackCityStatePromiseWithAnybodyScore(ePlayer);
	iOpinionWeight += GetIgnoredAttackCityStatePromiseScore(ePlayer);

	iOpinionWeight += GetBrokenBullyCityStatePromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredBullyCityStatePromiseScore(ePlayer);

	iOpinionWeight += GetBrokenNoConvertPromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredNoConvertPromiseScore(ePlayer);

	iOpinionWeight += GetBrokenSpyPromiseScore(ePlayer);
	iOpinionWeight += GetIgnoredSpyPromiseScore(ePlayer);

	iOpinionWeight += GetBrokenCoopWarPromiseScore(ePlayer);

	//////////////////////////////////////
	// PROTECTED MINORS
	//////////////////////////////////////
	iOpinionWeight += GetAngryAboutProtectedMinorKilledScore(ePlayer);
	iOpinionWeight += GetAngryAboutProtectedMinorAttackedScore(ePlayer);
	iOpinionWeight += GetAngryAboutProtectedMinorBulliedScore(ePlayer);

	//////////////////////////////////////
	// BULLIED MINORS
	//////////////////////////////////////
	iOpinionWeight += GetAngryAboutSidedWithProtectedMinorScore(ePlayer);

	//////////////////////////////////////
	// DECLARATION OF FRIENDSHIP
	//////////////////////////////////////
	iOpinionWeight += GetDOFAcceptedScore(ePlayer);
	iOpinionWeight += GetDOFWithAnyFriendScore(ePlayer);
	iOpinionWeight += GetDOFWithAnyEnemyScore(ePlayer);

	//////////////////////////////////////
	// FRIENDS NOT GETTING ALONG
	//////////////////////////////////////

	// Beging TraitorOpinion bit
	int iTraitorOpinion = GetFriendDenouncementScore(ePlayer);
	iTraitorOpinion = max(iTraitorOpinion, GetWeDenouncedFriendScore(ePlayer));
	iTraitorOpinion = max(iTraitorOpinion, GetFriendDenouncedUsScore(ePlayer));
	iTraitorOpinion = max(iTraitorOpinion, GetWeDeclaredWarOnFriendScore(ePlayer));
	iTraitorOpinion = max(iTraitorOpinion, GetFriendDeclaredWarOnUsScore(ePlayer));
	iOpinionWeight += iTraitorOpinion;
	// End TraitorOpinion bit

	//iOpinionWeight += GetRequestsRefusedScore(ePlayer);

	//////////////////////////////////////
	// DENOUNCING
	//////////////////////////////////////

	iOpinionWeight += GetDenouncedUsScore(ePlayer);
	iOpinionWeight += GetDenouncedThemScore(ePlayer);
	iOpinionWeight += GetDenouncedFriendScore(ePlayer);
	iOpinionWeight += GetDenouncedEnemyScore(ePlayer);

	//////////////////////////////////////
	// RECKLESS EXPANDER
	//////////////////////////////////////
	iOpinionWeight += GetRecklessExpanderScore(ePlayer);

	//////////////////////////////////////
	// JUNE 2011 ADDITIONS
	//////////////////////////////////////
	iOpinionWeight += GetRecentTradeScore(ePlayer);
	iOpinionWeight += GetCommonFoeScore(ePlayer);
	iOpinionWeight += GetRecentAssistScore(ePlayer);

	iOpinionWeight += GetNukedByScore(ePlayer);
	iOpinionWeight += GetCapitalCapturedByScore(ePlayer);

	iOpinionWeight += GetGaveAssistanceToScore(ePlayer);
	iOpinionWeight += GetPaidTributeToScore(ePlayer);

	//////////////////////////////////////
	// SCENARIO-SPECIFIC
	//////////////////////////////////////
	iOpinionWeight += GetScenarioModifier1(ePlayer);
	iOpinionWeight += GetScenarioModifier2(ePlayer);
	iOpinionWeight += GetScenarioModifier3(ePlayer);

	return iOpinionWeight;
}

/// What is our Diplomatic Opinion of this Major Civ?
MajorCivOpinionTypes CvDiplomacyAI::GetMajorCivOpinion(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (MajorCivOpinionTypes) m_paeMajorCivOpinion[ePlayer];
}

/// Sets what our Diplomatic Opinion is of a Major Civ
void CvDiplomacyAI::SetMajorCivOpinion(PlayerTypes ePlayer, MajorCivOpinionTypes eOpinion)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOpinion >= 0, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOpinion < NUM_MAJOR_CIV_OPINION_TYPES, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeMajorCivOpinion[ePlayer] = eOpinion;
}

/// How many Majors do we have a particular Opinion towards?
int CvDiplomacyAI::GetNumMajorCivOpinion(MajorCivOpinionTypes eOpinion) const
{
	CvAssertMsg(eOpinion >= 0, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOpinion < NUM_MAJOR_CIV_OPINION_TYPES, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iCount = 0;

	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		if(GetMajorCivOpinion((PlayerTypes) iMajorLoop) == eOpinion)
		{
			iCount++;
		}
	}

	return iCount;
}


/// What is our our guess as to other players' Opinions about everyone else?
void CvDiplomacyAI::DoEstimateOtherPlayerOpinions()
{
	MajorCivOpinionTypes eOpinion;

	int iOpinionWeight;

	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;

	// Loop through all (known) Majors
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			// Now loop through every player HE knows
			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;

				// Don't compare a player to himself
				if(eLoopPlayer != eLoopOtherPlayer)
				{
					// Do both we and the guy we're looking about know the third guy?
					if(IsPlayerValid(eLoopOtherPlayer, true) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{
						iOpinionWeight = 0;

						// Look at Land Dispute
						switch(GetEstimateOtherPlayerLandDisputeLevel(eLoopPlayer, eLoopOtherPlayer))
						{
						case DISPUTE_LEVEL_FIERCE:
							iOpinionWeight += /*30*/ GC.getOPINION_WEIGHT_LAND_FIERCE();
							break;
						case DISPUTE_LEVEL_STRONG:
							iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_LAND_STRONG();
							break;
						case DISPUTE_LEVEL_WEAK:
							iOpinionWeight += /*10*/ GC.getOPINION_WEIGHT_LAND_WEAK();
							break;
						case DISPUTE_LEVEL_NONE:
							iOpinionWeight += /*0*/ GC.getOPINION_WEIGHT_LAND_NONE();
							break;
						}

						// Look at Victory Competition Dispute
						switch(GetEstimateOtherPlayerVictoryDisputeLevel(eLoopPlayer, eLoopOtherPlayer))
						{
						case DISPUTE_LEVEL_FIERCE:
							iOpinionWeight += /*30*/ GC.getOPINION_WEIGHT_VICTORY_FIERCE();
							break;
						case DISPUTE_LEVEL_STRONG:
							iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_VICTORY_STRONG();
							break;
						case DISPUTE_LEVEL_WEAK:
							iOpinionWeight += /*10*/ GC.getOPINION_WEIGHT_VICTORY_WEAK();
							break;
						case DISPUTE_LEVEL_NONE:
							iOpinionWeight += /*0*/ GC.getOPINION_WEIGHT_VICTORY_NONE();
							break;
						}

						// Now do the final assessment
						if(iOpinionWeight >= /*50*/ GC.getOPINION_THRESHOLD_UNFORGIVABLE())
							eOpinion = MAJOR_CIV_OPINION_UNFORGIVABLE;
						else if(iOpinionWeight >= /*30*/ GC.getOPINION_THRESHOLD_ENEMY())
							eOpinion = MAJOR_CIV_OPINION_ENEMY;
						else if(iOpinionWeight >= /*10*/ GC.getOPINION_THRESHOLD_COMPETITOR())
							eOpinion = MAJOR_CIV_OPINION_COMPETITOR;
						else if(iOpinionWeight < /*-10*/ GC.getOPINION_THRESHOLD_FAVORABLE())
							eOpinion = MAJOR_CIV_OPINION_FAVORABLE;
						else if(iOpinionWeight < /*-30*/ GC.getOPINION_THRESHOLD_FRIEND())
							eOpinion = MAJOR_CIV_OPINION_FRIEND;
						else if(iOpinionWeight < /*-50*/ GC.getOPINION_THRESHOLD_ALLY())
							eOpinion = MAJOR_CIV_OPINION_ALLY;
						else
							eOpinion = MAJOR_CIV_OPINION_NEUTRAL;

						// Finally, set the Opinion
						SetMajorCivOtherPlayerOpinion(eLoopPlayer, eLoopOtherPlayer, eOpinion);
					}
				}
			}
		}
	}
}

/// What is our guess as to what a player's Diplomatic Opinion is of another player?
MajorCivOpinionTypes CvDiplomacyAI::GetMajorCivOtherPlayerOpinion(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (MajorCivOpinionTypes) m_ppaaeOtherPlayerMajorCivOpinion[ePlayer][eWithPlayer];
}

/// Sets what our is guess as to what a player's Diplomatic Opinion is of another player
void CvDiplomacyAI::SetMajorCivOtherPlayerOpinion(PlayerTypes ePlayer, PlayerTypes eWithPlayer, MajorCivOpinionTypes ePlayerOpinion)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayerOpinion >= 0, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayerOpinion < NUM_MAJOR_CIV_OPINION_TYPES, "DIPLOMACY_AI: Invalid MajorCivOpinionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerMajorCivOpinion[ePlayer][eWithPlayer] = ePlayerOpinion;
}


/// Determine our general approach to each Player we've met
void CvDiplomacyAI::DoUpdateMajorCivApproaches()
{
	int iPlayerLoop;
	PlayerTypes eLoopPlayer;

	// Transfer Approaches from last turn to a separate array so we still have access to the info, then clear out the real one
	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		m_paeApproachScratchPad[iPlayerLoop] = -1;

		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			m_paeApproachScratchPad[iPlayerLoop] = GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ false);
			SetMajorCivApproach(eLoopPlayer, NO_MAJOR_CIV_APPROACH);
		}
	}

	int iHighestWeight;

	MajorCivOpinionTypes eOpinion;
	int iNumCivsWeHaveThisOpinionTowards;

	FStaticVector<PlayerTypes, NUM_MAJOR_CIV_OPINION_TYPES* MAX_MAJOR_CIVS, true, c_eCiv5GameplayDLL, 0> vePlayerApproachOrder;
	CvWeightedVector<PlayerTypes, MAX_MAJOR_CIVS, true> veTiedPlayerApproachWeights;
	int iTiedPlayerVectorIndex;

	int iNumCivsWereLookingAt = 0;

	bool bAssert;

	// Look at the players we feel are our biggest opponents first
	for(int iOpinionLoop = 0; iOpinionLoop < NUM_MAJOR_CIV_OPINION_TYPES; iOpinionLoop++)
	{
		eOpinion = (MajorCivOpinionTypes) iOpinionLoop;

		iNumCivsWeHaveThisOpinionTowards = GetNumMajorCivOpinion(eOpinion);

		if(iNumCivsWeHaveThisOpinionTowards > 0)
		{
			veTiedPlayerApproachWeights.clear();

			WarFaceTypes eWarFace;	// This is not used, but must be passed into GetBestApproachTowardsMajorCiv() below

			// Loop through all (known) Majors and determine the order of who we pick our Approach for first
			for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				if(IsPlayerValid(eLoopPlayer))
				{
					if(GetMajorCivOpinion(eLoopPlayer) == eOpinion)
					{
						// If there's only civ we feel this way about then there's no need to look at anything else
						if(iNumCivsWeHaveThisOpinionTowards == 1)
						{
							iNumCivsWereLookingAt++;
							vePlayerApproachOrder.push_back(eLoopPlayer);
							break;
						}
						// More than one other leader we feel this way about - have to rank them now
						else
						{
							// Just grab the highest weight for sorting later
							GetBestApproachTowardsMajorCiv(eLoopPlayer, /*Passed by Ref*/ iHighestWeight, /*bLookAtOtherPlayers*/ false, /*bLog*/ false, /*Passed by Ref, but we won't use it*/ eWarFace);
							veTiedPlayerApproachWeights.push_back(eLoopPlayer, iHighestWeight);
						}
					}
				}
			}

			// Order players
			if(iNumCivsWeHaveThisOpinionTowards > 1)
			{
				veTiedPlayerApproachWeights.SortItems();

				// Now add sorted players to the overall vector that we'll look at in the next code block
				for(iTiedPlayerVectorIndex = 0; iTiedPlayerVectorIndex < (int) veTiedPlayerApproachWeights.size(); iTiedPlayerVectorIndex++)
				{
					eLoopPlayer = (PlayerTypes) veTiedPlayerApproachWeights.GetElement(iTiedPlayerVectorIndex);

					// Assert for Release build!
					if(eLoopPlayer < 0 || eLoopPlayer >= MAX_CIV_PLAYERS)
					{
						bAssert = true;
						CvAssert(false);
					}

					iNumCivsWereLookingAt++;
					vePlayerApproachOrder.push_back(eLoopPlayer);
				}
			}
		}
	}

	// Assert for Release build!
	if(iNumCivsWereLookingAt != (int) vePlayerApproachOrder.size())
	{
		bAssert = true;
		CvAssert(false);
	}

	MajorCivApproachTypes eApproach;

	WarFaceTypes eWarFace;

	// Now that we have our order for who to decide Approaches for, actually decide now!
	for(int iPlayerVectorIndex = 0; iPlayerVectorIndex < (int) vePlayerApproachOrder.size(); iPlayerVectorIndex++)
	{
		eLoopPlayer = vePlayerApproachOrder[iPlayerVectorIndex];

		// See which Approach is best
		eApproach = GetBestApproachTowardsMajorCiv(eLoopPlayer, /*Passed by Ref, but we won't use it*/ iHighestWeight, /*bLookAtOtherPlayers*/ true, /*bLog*/ true, /*Passed by Ref, used below*/ eWarFace);

		// If we're going to war and we haven't picked a War Face yet choose one
		if(eApproach == MAJOR_CIV_APPROACH_WAR && GetWarFaceWithPlayer(eLoopPlayer) == NO_WAR_FACE_TYPE)
			SetWarFaceWithPlayer(eLoopPlayer, eWarFace);

		CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		// Actually assign the (possibly) new Approach
		SetMajorCivApproach(eLoopPlayer, eApproach);
	}
}

/// What is the best approach to take towards a player?  Can also pass in iHighestWeight by reference if you just want to know what the player feels most strongly about without actually caring about WHAT it is
MajorCivApproachTypes CvDiplomacyAI::GetBestApproachTowardsMajorCiv(PlayerTypes ePlayer, int& iHighestWeight, bool bLookAtOtherPlayers, bool bLog, WarFaceTypes& eWarFace)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvTeam* pTeam = &GET_TEAM(eTeam);

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	MajorCivApproachTypes eApproach;
	// This vector is what we'll stuff the values into first, and pass it into our logging function (which can't take a CvWeightedVector, which we need to sort...)
	FStaticVector< int, 128, true, c_eCiv5GameplayDLL > viApproachWeights;

	int iApproachLoop;

	viApproachWeights.clear();

	for(iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
	{
		viApproachWeights.push_back(0);
	}

	////////////////////////////////////
	// NEUTRAL DEFAULT WEIGHT
	////////////////////////////////////

	viApproachWeights[MAJOR_CIV_APPROACH_NEUTRAL] += /*4*/ GC.getAPPROACH_NEUTRAL_DEFAULT();

	////////////////////////////////////
	// CURRENT APPROACH BIASES
	////////////////////////////////////

	// Bias for our current Approach.  This should prevent it from jumping around from turn-to-turn as much
	// We use the scratch pad here since the normal array has been cleared so that we have knowledge of who we've already assigned an Approach for this turn; this should be the only place the scratch pad is used
	MajorCivApproachTypes eOldApproach = (MajorCivApproachTypes) m_paeApproachScratchPad[ePlayer];
	if(eOldApproach == NO_MAJOR_CIV_APPROACH)
		eOldApproach = MAJOR_CIV_APPROACH_NEUTRAL;

	viApproachWeights[eOldApproach] += /*3*/ GC.getAPPROACH_BIAS_FOR_CURRENT();

	// If our previous Approach was deceptive, this gives us a bonus for war
	if(eOldApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*2*/ GC.getAPPROACH_WAR_CURRENTLY_DECEPTIVE();

	// If our previous Approach was Hostile, boost the strength (so we're unlikely to switch out of it)
	if(eOldApproach == MAJOR_CIV_APPROACH_HOSTILE)
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*5*/ GC.getAPPROACH_HOSTILE_CURRENTLY_HOSTILE();

	// Wanted war last turn bias: war must be calm or better to apply
	WarStateTypes eWarState = GetWarState(ePlayer);
	if(eWarState > WAR_STATE_STALEMATE)
	{
		// If we're planning a war then give it a bias so that we don't get away from it too easily
		if(eOldApproach == MAJOR_CIV_APPROACH_WAR)
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*3*/ GC.getAPPROACH_WAR_CURRENTLY_WAR();
	}

	// Conquest bias: must be a stalemate or better to apply (or not at war yet)
	if(eWarState == NO_WAR_STATE_TYPE || eWarState > WAR_STATE_DEFENSIVE)
	{
		if(IsGoingForWorldConquest())
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*5*/ GC.getAPPROACH_WAR_CONQUEST_GRAND_STRATEGY();
	}

	////////////////////////////////////
	// PERSONALITY
	////////////////////////////////////

	for(iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
	{
		viApproachWeights[iApproachLoop] += GetPersonalityMajorCivApproachBias((MajorCivApproachTypes) iApproachLoop);
	}

	////////////////////////////////////
	// OPINION
	////////////////////////////////////

	switch(GetMajorCivOpinion(ePlayer))
	{
	case MAJOR_CIV_OPINION_UNFORGIVABLE:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*10*/ GC.getAPPROACH_OPINION_UNFORGIVABLE_WAR();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*4*/ GC.getAPPROACH_OPINION_UNFORGIVABLE_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*0*/ GC.getAPPROACH_OPINION_UNFORGIVABLE_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*4*/ GC.getAPPROACH_OPINION_UNFORGIVABLE_GUARDED();
		break;
	case MAJOR_CIV_OPINION_ENEMY:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*8*/ GC.getAPPROACH_OPINION_ENEMY_WAR();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*4*/ GC.getAPPROACH_OPINION_ENEMY_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*1*/ GC.getAPPROACH_OPINION_ENEMY_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*4*/ GC.getAPPROACH_OPINION_ENEMY_GUARDED();
		break;
	case MAJOR_CIV_OPINION_COMPETITOR:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*4*/ GC.getAPPROACH_OPINION_COMPETITOR_WAR();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*4*/ GC.getAPPROACH_OPINION_COMPETITOR_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*2*/ GC.getAPPROACH_OPINION_COMPETITOR_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*2*/ GC.getAPPROACH_OPINION_COMPETITOR_GUARDED();
		break;
	case MAJOR_CIV_OPINION_NEUTRAL:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*0*/ GC.getAPPROACH_OPINION_NEUTRAL_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY]  += /*2*/ GC.getAPPROACH_OPINION_NEUTRAL_FRIENDLY();
		break;
	case MAJOR_CIV_OPINION_FAVORABLE:
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*-5*/ GC.getAPPROACH_OPINION_FAVORABLE_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*0*/ GC.getAPPROACH_OPINION_FAVORABLE_DECEPTIVE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*4*/ GC.getAPPROACH_OPINION_FAVORABLE_FRIENDLY();
		break;
	case MAJOR_CIV_OPINION_FRIEND:
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*-5*/ GC.getAPPROACH_OPINION_FRIEND_HOSTILE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*10*/ GC.getAPPROACH_OPINION_FRIEND_FRIENDLY();
		break;
	case MAJOR_CIV_OPINION_ALLY:
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*10*/ GC.getAPPROACH_OPINION_ALLY_FRIENDLY();
		break;
	}

	////////////////////////////////////
	// DECLARATION OF FRIENDSHIP
	////////////////////////////////////

	if(IsDoFAccepted(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*3*/ GC.getAPPROACH_DECEPTIVE_WORKING_WITH_PLAYER();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*15*/ GC.getAPPROACH_FRIENDLY_WORKING_WITH_PLAYER();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*-100*/ GC.getAPPROACH_HOSTILE_WORKING_WITH_PLAYER();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*-100*/ GC.getAPPROACH_GUARDED_WORKING_WITH_PLAYER();
	}

	////////////////////////////////////
	// DENOUNCE
	////////////////////////////////////

	// We denounced them
	if(IsDenouncedPlayer(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*10*/ GC.getAPPROACH_WAR_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*10*/ GC.getAPPROACH_HOSTILE_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*5*/ GC.getAPPROACH_GUARDED_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-100*/ GC.getAPPROACH_FRIENDLY_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-100*/ GC.getAPPROACH_DECEPTIVE_DENOUNCED();
	}

	// We were friends and they betrayed us!
	if(IsFriendDenouncedUs(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*10*/ GC.getAPPROACH_WAR_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*10*/ GC.getAPPROACH_HOSTILE_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*5*/ GC.getAPPROACH_GUARDED_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-100*/ GC.getAPPROACH_FRIENDLY_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-100*/ GC.getAPPROACH_DECEPTIVE_DENOUNCED();
	}

	// They denounced us
	if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(eMyPlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*10*/ GC.getAPPROACH_WAR_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*10*/ GC.getAPPROACH_HOSTILE_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*5*/ GC.getAPPROACH_GUARDED_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-100*/ GC.getAPPROACH_FRIENDLY_DENOUNCED();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-100*/ GC.getAPPROACH_DECEPTIVE_DENOUNCED();
	}

	////////////////////////////////////
	// WORKING AGAINST THIS PLAYER?
	////////////////////////////////////

	//if (IsWorkingAgainstPlayer(ePlayer))
	//{
	//	viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*10*/ GC.getAPPROACH_DECEPTIVE_WORKING_AGAINST_PLAYER();
	//	viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*10*/ GC.getAPPROACH_HOSTILE_WORKING_AGAINST_PLAYER();
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*10*/ GC.getAPPROACH_WAR_WORKING_AGAINST_PLAYER();
	//}

	////////////////////////////////////
	// MISTREATED OUR PROTECTED MINORS?
	////////////////////////////////////

	//antonjs: consider: add cases for IsAngryAboutProtectedMinorBullied, IsAngryAboutSidedWithTheirProtectedMinor

	if(IsAngryAboutProtectedMinorKilled(ePlayer) || IsAngryAboutProtectedMinorAttacked(ePlayer))
	{
		int iTurnsSinceMinorAttack = GetTurnsSincePlayerAttackedProtectedMinor(ePlayer);
		if (IsAngryAboutProtectedMinorKilled(ePlayer) || iTurnsSinceMinorAttack < /*50*/ GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS())
		{
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*6*/ GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_WAR();
			viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*2*/ GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_HOSTILE();
			viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*1*/ GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_GUARDED();
		}
		else
		{
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*4*/ GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_WAR();
			viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*1*/ GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_HOSTILE();
			viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*1*/ GC.getAPPROACH_ATTACKED_PROTECTED_MINOR_PAST_GUARDED();
		}
	}

	////////////////////////////////////
	// They made a demand
	////////////////////////////////////

	if(IsDemandEverMade(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-6*/ GC.getAPPROACH_DECEPTIVE_DEMAND();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-6*/ GC.getAPPROACH_FRIENDLY_DEMAND();
	}

	////////////////////////////////////
	// BROKEN PROMISES ;_;
	////////////////////////////////////

	// Broken military promise with us?
	if(IsPlayerBrokenMilitaryPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*4*/ GC.getAPPROACH_WAR_BROKEN_MILITARY_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-10*/ GC.getAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-10*/ GC.getAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE();
	}
	// Broken military promise with someone?
	else if(pTeam->IsBrokenMilitaryPromise())
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*5*/ GC.getAPPROACH_WAR_BROKEN_MILITARY_PROMISE_WORLD();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-4*/ GC.getAPPROACH_DECEPTIVE_BROKEN_MILITARY_PROMISE_WORLD();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-4*/ GC.getAPPROACH_FRIENDLY_BROKEN_MILITARY_PROMISE_WORLD();
	}
	// Ignored our request for them to make a military promise?
	else if(IsPlayerIgnoredMilitaryPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-4*/ GC.getAPPROACH_DECEPTIVE_IGNORED_MILITARY_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-4*/ GC.getAPPROACH_FRIENDLY_IGNORED_MILITARY_PROMISE();
	}

	// Broken Expansion promise with us?
	if(IsPlayerBrokenExpansionPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*4*/ GC.getAPPROACH_WAR_BROKEN_EXPANSION_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-6*/ GC.getAPPROACH_DECEPTIVE_BROKEN_EXPANSION_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-6*/ GC.getAPPROACH_FRIENDLY_BROKEN_EXPANSION_PROMISE();
	}
	// Ignored our request for them to make a Expansion promise?
	else if(IsPlayerIgnoredExpansionPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*3*/ GC.getAPPROACH_WAR_IGNORED_EXPANSION_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-4*/ GC.getAPPROACH_DECEPTIVE_IGNORED_EXPANSION_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-4*/ GC.getAPPROACH_FRIENDLY_IGNORED_EXPANSION_PROMISE();
	}

	// Broken Border promise with us?
	if(IsPlayerBrokenBorderPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*4*/ GC.getAPPROACH_WAR_BROKEN_BORDER_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-6*/ GC.getAPPROACH_DECEPTIVE_BROKEN_BORDER_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-6*/ GC.getAPPROACH_FRIENDLY_BROKEN_BORDER_PROMISE();
	}
	// Ignored our request for them to make a Border promise?
	else if(IsPlayerIgnoredBorderPromise(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*3*/ GC.getAPPROACH_WAR_IGNORED_BORDER_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-4*/ GC.getAPPROACH_DECEPTIVE_IGNORED_BORDER_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-4*/ GC.getAPPROACH_FRIENDLY_IGNORED_BORDER_PROMISE();
	}

	// Broken CityState attack promise with us?
	if(IsPlayerBrokenAttackCityStatePromise(ePlayer))
	{
		//antonjs: todo: rename these constants
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*4*/ GC.getAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-10*/ GC.getAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-10*/ GC.getAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE();
	}
	// Broken CityState attack promise with someone?
	else if(pTeam->IsBrokenCityStatePromise())
	{
		//antonjs: todo: rename these constants
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*3*/ GC.getAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE_WORLD();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-3*/ GC.getAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE_WORLD();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-3*/ GC.getAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE_WORLD();
	}
	// Ignored our request for them to make a CityState attack promise?
	else if(IsPlayerIgnoredAttackCityStatePromise(ePlayer))
	{
		//antonjs: todo: rename these constants
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*4*/ GC.getAPPROACH_WAR_IGNORED_CITY_STATE_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-4*/ GC.getAPPROACH_DECEPTIVE_IGNORED_CITY_STATE_PROMISE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-4*/ GC.getAPPROACH_FRIENDLY_IGNORED_CITY_STATE_PROMISE();
	}

	// Promise to not bully a city-state
	if(IsPlayerBrokenBullyCityStatePromise(ePlayer))
	{
		//antonjs: todo: custom constants
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*4*/ GC.getAPPROACH_WAR_BROKEN_CITY_STATE_PROMISE() / 2;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-10*/ GC.getAPPROACH_DECEPTIVE_BROKEN_CITY_STATE_PROMISE() / 2;
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-10*/ GC.getAPPROACH_FRIENDLY_BROKEN_CITY_STATE_PROMISE() / 2;
	}
	else if(IsPlayerIgnoredBullyCityStatePromise(ePlayer))
	{
		//antonjs: todo: custom constants
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*4*/ GC.getAPPROACH_WAR_IGNORED_CITY_STATE_PROMISE() / 2;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*-4*/ GC.getAPPROACH_DECEPTIVE_IGNORED_CITY_STATE_PROMISE() / 2;
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*-4*/ GC.getAPPROACH_FRIENDLY_IGNORED_CITY_STATE_PROMISE() / 2;
	}


	////////////////////////////////////
	// MILITARY THREAT
	////////////////////////////////////

	switch(GetMilitaryThreat(ePlayer))
	{
	case THREAT_CRITICAL:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*0*/ GC.getAPPROACH_DECEPTIVE_MILITARY_THREAT_CRITICAL();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*4*/ GC.getAPPROACH_GUARDED_MILITARY_THREAT_CRITICAL();
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] += /*4*/ GC.getAPPROACH_AFRAID_MILITARY_THREAT_CRITICAL();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*0*/ GC.getAPPROACH_FRIENDLY_MILITARY_THREAT_CRITICAL();
		break;
	case THREAT_SEVERE:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*0*/ GC.getAPPROACH_DECEPTIVE_MILITARY_THREAT_SEVERE();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*3*/ GC.getAPPROACH_GUARDED_MILITARY_THREAT_SEVERE();
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] += /*2*/ GC.getAPPROACH_AFRAID_MILITARY_THREAT_SEVERE();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*0*/ GC.getAPPROACH_FRIENDLY_MILITARY_THREAT_SEVERE();
		break;
	case THREAT_MAJOR:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*0*/ GC.getAPPROACH_DECEPTIVE_MILITARY_THREAT_MAJOR();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*2*/ GC.getAPPROACH_GUARDED_MILITARY_THREAT_MAJOR();
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] += /*1*/ GC.getAPPROACH_AFRAID_MILITARY_THREAT_MAJOR();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*0*/ GC.getAPPROACH_FRIENDLY_MILITARY_THREAT_MAJOR();
		break;
	case THREAT_MINOR:
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*0*/ GC.getAPPROACH_DECEPTIVE_MILITARY_THREAT_MINOR();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*0*/ GC.getAPPROACH_GUARDED_MILITARY_THREAT_MINOR();
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] += /*1*/ GC.getAPPROACH_AFRAID_MILITARY_THREAT_MINOR();
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*0*/ GC.getAPPROACH_FRIENDLY_MILITARY_THREAT_MINOR();
		break;
	case THREAT_NONE:
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*2*/ GC.getAPPROACH_HOSTILE_MILITARY_THREAT_NONE();
		break;
	}

	int iPlayerLoop;
	PlayerTypes eLoopPlayer;

	////////////////////////////////////
	// AT WAR RIGHT NOW
	////////////////////////////////////

	for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Don't look at the guy we're already thinking about or anyone on his team
		if(ePlayer != eLoopPlayer && GET_PLAYER(ePlayer).getTeam() != GET_PLAYER(eLoopPlayer).getTeam())
		{
			if(IsPlayerValid(eLoopPlayer))
			{
				if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					if(GetStateAllWars() == STATE_ALL_WARS_NEUTRAL)
						viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*-9*/ GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_NEUTRAL();
					else if(GetStateAllWars() == STATE_ALL_WARS_WINNING)
						viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*-3*/ GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_WINNING();
					else
						viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*-12*/ GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_LOSING();

					viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*-2*/ GC.getAPPROACH_HOSTILE_AT_WAR_WITH_PLAYER();
					viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*2*/ GC.getAPPROACH_DECEPTIVE_AT_WAR_WITH_PLAYER();
					viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*0*/ GC.getAPPROACH_GUARDED_AT_WAR_WITH_PLAYER();
					viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*2*/ GC.getAPPROACH_FRIENDLY_AT_WAR_WITH_PLAYER();
				}
			}
		}
	}

	////////////////////////////////////
	// APPROACHES TOWARDS OTHER PLAYERS
	////////////////////////////////////

	// Look at Approaches we've already adopted for players we feel more strongly about
	if(bLookAtOtherPlayers)
	{
		bool bMinorCiv;

		for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			// Don't look at the guy we're already thinking about
			if(eLoopPlayer != ePlayer)
			{
				if(IsPlayerValid(eLoopPlayer))
				{
					bMinorCiv = GET_PLAYER(eLoopPlayer).isMinorCiv();

					// Planning war with this player? (Can't ONLY use the War Approach because this could have been cleared before, but we have to also check it because it could have just been set for someone else earlier this turn)
					if(GetWarGoal(eLoopPlayer) == WAR_GOAL_PREPARE ||
					        (!bMinorCiv && GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_WAR))	// Major Civs only
					{
						viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*-12*/ GC.getAPPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER();
						viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*-2*/ GC.getAPPROACH_HOSTILE_PLANNING_WAR_WITH_ANOTHER_PLAYER();
						viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*2*/ GC.getAPPROACH_DECEPTIVE_PLANNING_WAR_WITH_ANOTHER_PLAYER();
						viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] += /*0*/ GC.getAPPROACH_GUARDED_PLANNING_WAR_WITH_ANOTHER_PLAYER();
						viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*2*/ GC.getAPPROACH_FRIENDLY_PLANNING_WAR_WITH_ANOTHER_PLAYER();
					}

					// Approaches already assigned to other higher-priority Majors
					if(!bMinorCiv)
					{
						switch(GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ false))
						{
						case MAJOR_CIV_APPROACH_HOSTILE:
							viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*-2*/ GC.getAPPROACH_WAR_HOSTILE_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*-2*/ GC.getAPPROACH_HOSTILE_HOSTILE_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*2*/ GC.getAPPROACH_DECEPTIVE_HOSTILE_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*2*/ GC.getAPPROACH_FRIENDLY_HOSTILE_WITH_ANOTHER_PLAYER();
							break;
						case MAJOR_CIV_APPROACH_AFRAID:
							viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*-4*/ GC.getAPPROACH_WAR_AFRAID_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] += /*-4*/ GC.getAPPROACH_HOSTILE_AFRAID_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*2*/ GC.getAPPROACH_DECEPTIVE_AFRAID_WITH_ANOTHER_PLAYER();
							viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] += /*2*/ GC.getAPPROACH_FRIENDLY_AFRAID_WITH_ANOTHER_PLAYER();
							break;
						}
					}
				}
			}
		}
	}

	////////////////////////////////////
	// WAR PROJECTION - how do we think a war against ePlayer will go?
	////////////////////////////////////

	switch(GetWarProjection(ePlayer))
	{
	case WAR_PROJECTION_DESTRUCTION:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*45*/ GC.getAPPROACH_WAR_PROJECTION_DESTRUCTION_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= /*45*/ GC.getAPPROACH_WAR_PROJECTION_DESTRUCTION_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *= /*125*/ GC.getAPPROACH_GUARDED_PROJECTION_DESTRUCTION_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_DEFEAT:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*60*/ GC.getAPPROACH_WAR_PROJECTION_DEFEAT_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= /*60*/ GC.getAPPROACH_WAR_PROJECTION_DEFEAT_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *= /*115*/ GC.getAPPROACH_GUARDED_PROJECTION_DEFEAT_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_STALEMATE:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*80*/ GC.getAPPROACH_WAR_PROJECTION_STALEMATE_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *= /*105*/ GC.getAPPROACH_GUARDED_PROJECTION_STALEMATE_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_UNKNOWN:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*100*/ GC.getAPPROACH_WAR_PROJECTION_UNKNOWN_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *= /*100*/ GC.getAPPROACH_GUARDED_PROJECTION_UNKNOWN_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_GOOD:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*150*/ GC.getAPPROACH_WAR_PROJECTION_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= /*150*/ GC.getAPPROACH_WAR_PROJECTION_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *= /*80*/ GC.getAPPROACH_GUARDED_PROJECTION_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	case WAR_PROJECTION_VERY_GOOD:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*180*/ GC.getAPPROACH_WAR_PROJECTION_VERY_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= /*180*/ GC.getAPPROACH_WAR_PROJECTION_VERY_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] *= /*60*/ GC.getAPPROACH_GUARDED_PROJECTION_VERY_GOOD_PERCENT();
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] /= 100;
		break;
	}

	////////////////////////////////////
	// Is this player a reckless expander?
	////////////////////////////////////

	if(IsPlayerRecklessExpander(ePlayer))
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*160*/ GC.getAPPROACH_WAR_RECKLESS_EXPANDER();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= /*160*/ GC.getAPPROACH_WAR_RECKLESS_EXPANDER();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
	}

	////////////////////////////////////
	// Is this player already in a war with someone who isn't us?
	////////////////////////////////////

	bool bThinkingAboutDogpiling = false;
	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
		if(eLoopPlayer != GET_PLAYER(ePlayer).GetID() && eLoopPlayer != m_pPlayer->GetID() && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			eWarState = GET_PLAYER(ePlayer).GetDiplomacyAI()->GetWarState(eLoopPlayer);
			if(eWarState != NO_WAR_STATE_TYPE)
			{
				bThinkingAboutDogpiling = true;
			}
		}
	}
	if (bThinkingAboutDogpiling)
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= 125;
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= 125;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
	}

	////////////////////////////////////
	// DISTANCE - the farther away a player is the less likely we are to want to attack them!
	////////////////////////////////////

	// Factor in distance
	switch(GetPlayer()->GetProximityToPlayer(ePlayer))
	{
	case PLAYER_PROXIMITY_NEIGHBORS:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*115*/ GC.getAPPROACH_WAR_PROXIMITY_NEIGHBORS();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= /*115*/ GC.getAPPROACH_WAR_PROXIMITY_NEIGHBORS();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		break;
	case PLAYER_PROXIMITY_CLOSE:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*100*/ GC.getAPPROACH_WAR_PROXIMITY_CLOSE();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= /*100*/ GC.getAPPROACH_WAR_PROXIMITY_CLOSE();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		break;
	case PLAYER_PROXIMITY_FAR:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*60*/ GC.getAPPROACH_WAR_PROXIMITY_FAR();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= /*60*/ GC.getAPPROACH_WAR_PROXIMITY_FAR();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		break;
	case PLAYER_PROXIMITY_DISTANT:
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*50*/ GC.getAPPROACH_WAR_PROXIMITY_DISTANT();
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= /*50*/ GC.getAPPROACH_WAR_PROXIMITY_DISTANT();
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
		break;
	}

	////////////////////////////////////
	// PEACE TREATY - have we made peace with this player before?  If so, reduce war weight
	////////////////////////////////////

	int iPeaceTreatyTurn = GET_TEAM(GetTeam()).GetTurnMadePeaceTreatyWithTeam(eTeam);
	if(iPeaceTreatyTurn > -1)
	{
		int iTurnsSincePeace = GC.getGame().getElapsedGameTurns() - iPeaceTreatyTurn;
		if(iTurnsSincePeace < /*25*/ GC.getTURNS_SINCE_PEACE_WEIGHT_DAMPENER())
		{
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*65*/ GC.getAPPROACH_WAR_HAS_MADE_PEACE_BEFORE_PERCENT();
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		}
	}

	////////////////////////////////////
	// DUEL - If there's only 2 players in this game, no friendly or deceptive
	////////////////////////////////////

	int iNumMajorsLeft = GC.getGame().countMajorCivsAlive();
	if(iNumMajorsLeft == 2)
	{
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] = 0;
	}

	////////////////////////////////////
	// COOP WAR - agreed to go to war with someone?
	////////////////////////////////////

	if(IsLockedIntoCoopWar(ePlayer))
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] += /*1000*/ GC.getCOOP_WAR_LOCKED_TURNS_WAR_WEIGHT();

	////////////////////////////////////
	// RANDOM FACTOR
	////////////////////////////////////

	int iRandAmount;
	for(iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
	{
		// Increase weights to hundreds to give us more fidelity
		viApproachWeights[iApproachLoop] *= 100;

		iRandAmount = viApproachWeights[iApproachLoop] * /*15*/ GC.getAPPROACH_RANDOM_PERCENT();

		// If the amount is negative, only bad things can happen.  Plus, it's not likely we're going to pick this anyways
		if(iRandAmount > 0)
		{
			iRandAmount /= 100;
			viApproachWeights[iApproachLoop] += GC.getGame().getJonRandNumVA(iRandAmount, "Randomizing Diplo AI Major Civ Approach Weight (%d; %d)", (int)GetPlayer()->GetID(), (int)ePlayer);
		}
	}

	////////////////////////////////////
	// CAN WE DECLARE WAR?
	////////////////////////////////////

	if(!GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(ePlayer).getTeam()))
	{
		// If we're already at war with this player don't cancel out the weight for them!
		if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
		{
			viApproachWeights[MAJOR_CIV_APPROACH_WAR] = 0;
		}
	}

	////////////////////////////////////
	// On the same team?
	////////////////////////////////////
	if(GetTeam() == GET_PLAYER(ePlayer).getTeam())
	{
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_HOSTILE] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_GUARDED] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_AFRAID] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_NEUTRAL] = 0;
		viApproachWeights[MAJOR_CIV_APPROACH_FRIENDLY] = 100;
	}

	////////////////////////////////////
	// MODIFY WAR BASED ON HUMAN DIFFICULTY LEVEL
	////////////////////////////////////

	if(GET_PLAYER(ePlayer).isHuman())
	{
		HandicapTypes eHandicap = GET_PLAYER(ePlayer).getHandicapType();
		int iWarMod = 100;

		CvHandicapInfo* pHandicap = GC.getHandicapInfo(eHandicap);
		if(pHandicap)
			iWarMod = pHandicap->getAIDeclareWarProb();

		viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= iWarMod;
		viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] *= iWarMod;
		viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] /= 100;
	}

	// This vector is what we'll use to sort
	CvWeightedVector< MajorCivApproachTypes, 128 > vApproachWeightsForSorting;
	vApproachWeightsForSorting.clear();

	// Transfer values from our normal int vector (which we need for logging) to the Weighted Vector we can sort
	for(iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
	{
		vApproachWeightsForSorting.push_back((MajorCivApproachTypes) iApproachLoop, viApproachWeights[iApproachLoop]);
	}

	vApproachWeightsForSorting.SortItems();

	eApproach = vApproachWeightsForSorting.GetElement(0);
	iHighestWeight = vApproachWeightsForSorting.GetWeight(0);

	// If we're going to war then update how we're acting
	if(eApproach == MAJOR_CIV_APPROACH_WAR)
	{
		eWarFace = GetWarFaceWithPlayer(ePlayer);

		// If we haven't set WarFace on a previous turn, figure out what it should be
		if(eWarFace == NO_WAR_FACE_TYPE)
		{
			MajorCivApproachTypes eTempApproach;

			// Use index of 1 since we already know element 0 is war; that will give us the most reasonable approach
			eTempApproach = vApproachWeightsForSorting.GetElement(1);

			// Pick among the Approach types
			switch(eTempApproach)
			{

			case MAJOR_CIV_APPROACH_HOSTILE:
				eWarFace = WAR_FACE_HOSTILE;
				break;

			case MAJOR_CIV_APPROACH_DECEPTIVE:
			case MAJOR_CIV_APPROACH_AFRAID:
			case MAJOR_CIV_APPROACH_FRIENDLY:

				// Denounced them?  If so, let's not be too friendly
				if(IsDenouncedPlayer(ePlayer))
				{
					eWarFace = WAR_FACE_NEUTRAL;
				}
				else
				{
					eWarFace = WAR_FACE_FRIENDLY;
				}
				break;

			default:
				eWarFace = WAR_FACE_NEUTRAL;
				break;
			}
		}
	}
	else
	{
		eWarFace = NO_WAR_FACE_TYPE;
	}

	// Don't want to log if we're just seeing what the highest weight is and don't care about what Approach we like
	if(bLog)
	{
		LogMajorCivApproachUpdate(ePlayer, viApproachWeights.begin(), eApproach, eOldApproach, eWarFace);
	}

	return eApproach;
}


/// What is our Diplomatic approach to this Major Civ?
MajorCivApproachTypes CvDiplomacyAI::GetMajorCivApproach(PlayerTypes ePlayer, bool bHideTrueFeelings) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = (MajorCivApproachTypes) m_paeMajorCivApproach[ePlayer];

	// If we're hiding our true feelings then use the War Face or Friendly if we're Deceptive
	if(bHideTrueFeelings)
	{
		// Deceptive = Friendly
		if(eApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		{
			eApproach = MAJOR_CIV_APPROACH_FRIENDLY;
		}

		// War Face
		else if(eApproach == MAJOR_CIV_APPROACH_WAR)
		{
			switch(GetWarFaceWithPlayer(ePlayer))
			{
			case WAR_FACE_HOSTILE:
				eApproach = MAJOR_CIV_APPROACH_HOSTILE;
				break;
			case WAR_FACE_FRIENDLY:
				eApproach = MAJOR_CIV_APPROACH_FRIENDLY;
				break;
			case WAR_FACE_NEUTRAL:
				eApproach = MAJOR_CIV_APPROACH_NEUTRAL;
				break;
			default:
				CvAssert(false);
				break;
			}
		}
	}

	return eApproach;
}

/// Sets what our Diplomatic approach is to a Major Civ
void CvDiplomacyAI::SetMajorCivApproach(PlayerTypes ePlayer, MajorCivApproachTypes eApproach)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach >= NO_MAJOR_CIV_APPROACH, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");		// NO_MAJOR_CIV_APPROACH is valid because the Approach gets reset at the start of every turn.  We have an assert there that tests it though
	CvAssertMsg(eApproach < NUM_MAJOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeMajorCivApproach[ePlayer] = eApproach;
}

/// How many Majors do we have a particular Approach towards?
int CvDiplomacyAI::GetNumMajorCivApproach(MajorCivApproachTypes eApproach) const
{
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MAJOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iCount = 0;

	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		if(GetMajorCivApproach((PlayerTypes) iMajorLoop, /*bHideTrueFeelings*/ false) == eApproach)
		{
			iCount++;
		}
	}

	return iCount;
}


/// Determine our general approach to each Minor Civ we've met
void CvDiplomacyAI::DoUpdateMinorCivApproaches()
{
	int iPlayerLoop;
	PlayerTypes eLoopPlayer;

	// Transfer Approaches from last turn to a separate array so we still have access to the info, then clear out the real one
	for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		m_paeApproachScratchPad[iPlayerLoop] = -1;

		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).isMinorCiv())
		{
			if(IsPlayerValid(eLoopPlayer))
			{
				m_paeApproachScratchPad[eLoopPlayer] = GetMinorCivApproach(eLoopPlayer);
				SetMinorCivApproach(eLoopPlayer, NO_MINOR_CIV_APPROACH);
			}
		}
	}

	int iHighestWeight;

	CvWeightedVector<PlayerTypes, MAX_MINOR_CIVS, true> vePlayerApproachWeights;
	int iPlayerVectorIndex;

	// Loop through all (known) Minors and determine the order of who we pick our Approach for first based on PROXIMITY - this is different from Majors
	for(iPlayerLoop = MAX_MAJOR_CIVS; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iHighestWeight = GetPlayer()->GetProximityToPlayer(eLoopPlayer);	// Note that the order in the PlayerProximityTYpes enum is very important here: be sure to verify that NEIGHBORS is the last entry
			vePlayerApproachWeights.push_back(eLoopPlayer, iHighestWeight);
		}
	}

	MinorCivApproachTypes eApproach;
	MinorCivApproachTypes eLastTurnApproach;

	// Now sort the list if there's anything in it
	if(vePlayerApproachWeights.size() > 0)
	{
		vePlayerApproachWeights.SortItems();

		// Now that Minors are sorted, ACTUALLY figure out what our Approach will be, taking everything into account
		for(iPlayerVectorIndex = 0; iPlayerVectorIndex < (int) vePlayerApproachWeights.size(); iPlayerVectorIndex++)
		{
			eLoopPlayer = (PlayerTypes) vePlayerApproachWeights.GetElement(iPlayerVectorIndex);

			// See which Approach is best
			eApproach = GetBestApproachTowardsMinorCiv(eLoopPlayer, /*Passed by Ref, but we won't use it*/ iHighestWeight, /*bLookAtOtherPlayers*/ true, /*bLog*/ true);

			eLastTurnApproach = (MinorCivApproachTypes) m_paeApproachScratchPad[eLoopPlayer];

			CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

			SetMinorCivApproach(eLoopPlayer, eApproach);
		}
	}

	// Debug check

#ifndef FINAL_RELEASE
	for(iPlayerLoop = 0; iPlayerLoop < MAX_MINOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).isMinorCiv())
		{
			if(IsPlayerValid(eLoopPlayer))
			{
				if(GetMinorCivApproach(eLoopPlayer) == NO_MINOR_CIV_APPROACH)
					CvAssertMsg(false, "DIPLOMACY_AI: Invalid MinorCivApproachType. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
			}
		}
	}
#endif
}


/// What is the best approach to take towards a Minor Civ?  Can also pass in iHighestWeight by reference if you just want to know what the player feels most strongly about without actually caring about WHAT it is
MinorCivApproachTypes CvDiplomacyAI::GetBestApproachTowardsMinorCiv(PlayerTypes ePlayer, int& iHighestWeight, bool bLookAtOtherPlayers, bool bLog)
{
	CvAssertMsg(ePlayer >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	MinorCivApproachTypes eApproach;
	// This vector is what we'll stuff the values into first, and pass it into our logging function (which can't take a CvWeightedVector, which we need to sort...)
	FStaticVector< int, 128, true, c_eCiv5GameplayDLL > viApproachWeights;

	int iApproachLoop;

	viApproachWeights.clear();

	for(iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
	{
		viApproachWeights.push_back(0);
	}

	////////////////////////////////////
	// NEUTRAL DEFAULT WEIGHT
	////////////////////////////////////

	viApproachWeights[MINOR_CIV_APPROACH_IGNORE] += /*1*/ GC.getMINOR_APPROACH_IGNORE_DEFAULT();

	////////////////////////////////////
	// CURRENT SITUATION BIASES
	////////////////////////////////////

	// Bias for our current Approach.  This should prevent it from jumping around from turn-to-turn as much
	// We use the scratch pad here since the normal array has been cleared so that we have knowledge of who we've already assigned an Approach for this turn; this should be the only place the scratch pad is used
	CvAssertMsg(m_paeApproachScratchPad[ePlayer] >= NO_MINOR_CIV_APPROACH && m_paeApproachScratchPad[ePlayer] < NUM_MINOR_CIV_APPROACHES, "m_paeApproachScratchPad[iScratchPadID] is out of bounds for minor civ approaches. Show Jon!");
	if(m_paeApproachScratchPad[ePlayer] < NO_MINOR_CIV_APPROACH || m_paeApproachScratchPad[ePlayer] >= NUM_MINOR_CIV_APPROACHES)
	{
		return NO_MINOR_CIV_APPROACH;
	}

	MinorCivApproachTypes eOldApproach = (MinorCivApproachTypes) m_paeApproachScratchPad[ePlayer];
	viApproachWeights[eOldApproach] += /*2*/ GC.getMINOR_APPROACH_BIAS_FOR_CURRENT();

	// If we're planning a war then give it a bias so that we don't get away from it too easily
	if(eOldApproach == MINOR_CIV_APPROACH_CONQUEST)
	{
		// Add some bias to Ignore to help us get out in case things get sticky
		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] += /*3*/ GC.getMINOR_APPROACH_IGNORE_CURRENTLY_WAR();

		// Don't give this bias if war is going poorly
		WarStateTypes eWarState = GetWarState(ePlayer);
		if(eWarState < WAR_STATE_STALEMATE)
		{
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*4*/ GC.getAPPROACH_WAR_CURRENTLY_WAR();
		}
	}
	// If we're Protective then knock Conquest down & pump Protective up
	else if(eOldApproach == MINOR_CIV_APPROACH_PROTECTIVE)
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-10*/ GC.getMINOR_APPROACH_WAR_CURRENTLY_PROTECTIVE();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*10*/ GC.getMINOR_APPROACH_PROTECTIVE_CURRENTLY_PROTECTIVE();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -10; //antonjs: todo: constant/XML
	}

	// If we're ALREADY at war with this player then we're much less likely to be Protective
	if(IsAtWar(ePlayer))
	{
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*-15*/ GC.getMINOR_APPROACH_PROTECTIVE_CURRENTLY_WAR();
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*-6*/ GC.getMINOR_APPROACH_FRIENDLY_CURRENTLY_WAR();
	}

	////////////////////////////////////
	// RESOURCES
	////////////////////////////////////

	int iNumWeLack = GET_PLAYER(ePlayer).GetMinorCivAI()->GetNumResourcesMajorLacks(GetPlayer()->GetID());
	if(iNumWeLack > 0)
	{
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += (iNumWeLack* /*1*/ GC.getMINOR_APPROACH_FRIENDLY_RESOURCES());
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += (iNumWeLack* /*1*/ GC.getMINOR_APPROACH_PROTECTIVE_RESOURCES());
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += (iNumWeLack/***/ /*1*/ /*GC.getMINOR_APPROACH_PROTECTIVE_RESOURCES()*/); // we could just take them after all
	}

	////////////////////////////////////
	// FRIENDS WITH MINOR
	////////////////////////////////////

	if(GET_PLAYER(ePlayer).GetMinorCivAI()->IsFriends(GetPlayer()->GetID()))
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-100*/ GC.getMINOR_APPROACH_WAR_FRIENDS();
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*4*/ GC.getMINOR_APPROACH_FRIENDLY_FRIENDS();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*2*/ GC.getMINOR_APPROACH_PROTECTIVE_FRIENDS(); //antonjs: todo: increase
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] -= 10; //antonjs: todo: constant/XML
	}

	////////////////////////////////////
	// PLEDGE TO PROTECT - have we pledged to protect this minor?
	////////////////////////////////////
	//antonjs: consider: disable this weight after a certain amount of turns, to have this player "reevaluate" its PtP
	if(GET_PLAYER(ePlayer).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
	{
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += 2; //antonjs: todo: constant/XML
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -1; //antonjs: todo: constant/XML
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += -2; //antonjs: todo: constant/XML
	}

	////////////////////////////////////
	// PERSONALITY
	////////////////////////////////////

	for(iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
	{
		viApproachWeights[iApproachLoop] += GetPersonalityMinorCivApproachBias((MinorCivApproachTypes) iApproachLoop);
	}

	////////////////////////////////////
	// CONQUEST GRAND STRATEGY
	////////////////////////////////////

	if(IsGoingForWorldConquest())
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*10*/ GC.getMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*-15*/ GC.getMINOR_APPROACH_PROTECTIVE_CONQUEST_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*-5*/ GC.getMINOR_APPROACH_FRIENDLY_CONQUEST_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 8; //antonjs: todo: constant/XML

		// if we're neighbors, make it even more likely we'll go to war
		if(GetPlayer()->GetProximityToPlayer(ePlayer) == PLAYER_PROXIMITY_NEIGHBORS)
		{
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*10*/ GC.getMINOR_APPROACH_WAR_CONQUEST_GRAND_STRATEGY_NEIGHBORS();
			viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 8; //antonjs: todo: constant/XML
		}
	}

	////////////////////////////////////
	// DIPLO GRAND STRATEGY
	////////////////////////////////////

	if(IsGoingForDiploVictory())
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-20*/ GC.getMINOR_APPROACH_WAR_DIPLO_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] += /*-15*/ GC.getMINOR_APPROACH_IGNORE_DIPLO_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -15; //antonjs: todo: constant/XML

		// Neighbors and Close: +5 to Protective
		if(GetPlayer()->GetProximityToPlayer(ePlayer) >= PLAYER_PROXIMITY_CLOSE)
			viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*5*/ GC.getMINOR_APPROACH_PROTECTIVE_DIPLO_GRAND_STRATEGY_NEIGHBORS();
	}

	////////////////////////////////////
	// CULTURE GRAND STRATEGY
	////////////////////////////////////

	if(IsGoingForCultureVictory())
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-20*/ GC.getMINOR_APPROACH_WAR_CULTURE_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] += /*-15*/ GC.getMINOR_APPROACH_IGNORE_CULTURE_GRAND_STRATEGY();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -10; //antonjs: todo: constant/XML

		// Minor is cultural
		if(GET_PLAYER(ePlayer).GetMinorCivAI()->GetTrait() == MINOR_CIV_TRAIT_CULTURED)
			viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*5*/ GC.getMINOR_APPROACH_PROTECTIVE_CULTURE_GRAND_STRATEGY_CST();
	}

	////////////////////////////////////
	// TRAITS THAT EFFECT MINORS - These are heavy handed, but that is intentional
	////////////////////////////////////
	int iBonusTraitMod = 0;
	iBonusTraitMod += (m_pPlayer->GetPlayerTraits()->GetCityStateFriendshipModifier() > 0) ? 100 : 0;
	iBonusTraitMod += (m_pPlayer->GetPlayerTraits()->GetCityStateBonusModifier() > 0) ? 100 : 0;
	iBonusTraitMod += (m_pPlayer->GetPlayerTraits()->GetCityStateCombatModifier() > 0) ? -100 : 0;
	if (iBonusTraitMod)
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] -= iBonusTraitMod;
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] -= iBonusTraitMod / 5;
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += iBonusTraitMod / 5;
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += iBonusTraitMod;
	}

	////////////////////////////////////
	// RELIGION
	////////////////////////////////////
	//antonjs: todo: befriend/protect religious minors if we have a religion strategy

	////////////////////////////////////
	// HAPPINESS
	////////////////////////////////////
	//antonjs: todo: befriend/protect mercantile minors if we are in need of happiness

	////////////////////////////////////
	// MILITARY THREAT
	////////////////////////////////////

	//switch (GetMilitaryThreat(ePlayer))
	//{
	//case THREAT_CRITICAL:
	//	//viApproachWeights[MAJOR_CIV_APPROACH_DECEPTIVE] += /*0*/ GC.getAPPROACH_DECEPTIVE_MILITARY_THREAT_CRITICAL();
	//	//break;
	//case THREAT_SEVERE:
	//case THREAT_MAJOR:
	//case THREAT_MINOR:
	//case THREAT_NONE:
	//	break;
	//}

	int iPlayerLoop;
	PlayerTypes eLoopPlayer;

	////////////////////////////////////
	// AT WAR RIGHT NOW
	////////////////////////////////////

	for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Don't look at the guy we're already thinking about or anyone on his team
		if(ePlayer != eLoopPlayer && GET_PLAYER(ePlayer).getTeam() != GET_PLAYER(eLoopPlayer).getTeam())
		{
			if(IsPlayerValid(eLoopPlayer))
			{
				if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					if(GetStateAllWars() == STATE_ALL_WARS_NEUTRAL)
						viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-9*/ GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_NEUTRAL();
					else if(GetStateAllWars() == STATE_ALL_WARS_WINNING)
						viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-3*/ GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_WINNING();
					else
						viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-12*/ GC.getAPPROACH_WAR_AT_WAR_WITH_PLAYER_WARS_LOSING();
				}
			}
		}
	}

	////////////////////////////////////
	// APPROACHES TOWARDS OTHER PLAYERS
	////////////////////////////////////

	// Look at Approaches we've already adopted for players we feel more strongly about
	if(bLookAtOtherPlayers)
	{
		bool bMinorCiv;

		// Major Civs
		for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			// Don't look at the guy we're already thinking about
			if(eLoopPlayer != ePlayer)
			{
				if(IsPlayerValid(eLoopPlayer))
				{
					bMinorCiv = GET_PLAYER(eLoopPlayer).isMinorCiv();

					// Planning war with this player? (Can't ONLY use the War Approach because this could have been cleared before, but we have to also check it because it could have just been set for someone else earlier this turn)
					if(GetWarGoal(eLoopPlayer) == WAR_GOAL_PREPARE ||
					        (!bMinorCiv && GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_WAR))	// Major Civs only
					{
						viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-20*/ GC.getMINOR_APPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER();
					}

					// Approaches already assigned to other higher-priority Minors
					if(bMinorCiv)
					{
						switch(GetMinorCivApproach(eLoopPlayer))
						{
						case MINOR_CIV_APPROACH_CONQUEST:
							viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-20*/ GC.getMINOR_APPROACH_WAR_PLANNING_WAR_WITH_ANOTHER_PLAYER();
							break;
						case MINOR_CIV_APPROACH_PROTECTIVE:
							// If we're already protecting other Minors then we've already been stretched thin
							viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*-2*/ GC.getMINOR_APPROACH_PROTECTIVE_WITH_ANOTHER_PLAYER();
							break;
						case MINOR_CIV_APPROACH_BULLY:
							// If we're already bullying another player, we don't want to piss off too many
							viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -2; //antonjs: todo: constant/XML
							break;
						}
					}
				}
			}
		}
	}

	////////////////////////////////////
	// PROXIMITY
	////////////////////////////////////

	switch(GetPlayer()->GetProximityToPlayer(ePlayer))
	{
	case PLAYER_PROXIMITY_NEIGHBORS:
		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] += /*-2*/ GC.getMINOR_APPROACH_IGNORE_PROXIMITY_NEIGHBORS();
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*-1*/ GC.getMINOR_APPROACH_FRIENDLY_PROXIMITY_NEIGHBORS();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*1*/ GC.getMINOR_APPROACH_PROTECTIVE_PROXIMITY_NEIGHBORS();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*1*/ GC.getMINOR_APPROACH_CONQUEST_PROXIMITY_NEIGHBORS();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 1; //antonjs: todo: constant/XML
		break;
	case PLAYER_PROXIMITY_CLOSE:
		viApproachWeights[MINOR_CIV_APPROACH_IGNORE] += /*-1*/ GC.getMINOR_APPROACH_IGNORE_PROXIMITY_CLOSE();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*1*/ GC.getMINOR_APPROACH_PROTECTIVE_PROXIMITY_CLOSE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-2*/ GC.getMINOR_APPROACH_CONQUEST_PROXIMITY_CLOSE();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 1; //antonjs: todo: constant/XML
		break;
	case PLAYER_PROXIMITY_FAR:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*2*/ GC.getMINOR_APPROACH_FRIENDLY_PROXIMITY_FAR();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-4*/ GC.getMINOR_APPROACH_CONQUEST_PROXIMITY_FAR();
		break;
	case PLAYER_PROXIMITY_DISTANT:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*2*/ GC.getMINOR_APPROACH_FRIENDLY_PROXIMITY_DISTANT();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*-10*/ GC.getMINOR_APPROACH_CONQUEST_PROXIMITY_DISTANT();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -2; //antonjs: todo: constant/XML
		break;
	}

	////////////////////////////////////
	// MINOR PERSONALITY
	////////////////////////////////////

	switch(GET_PLAYER(ePlayer).GetMinorCivAI()->GetPersonality())
	{
	case MINOR_CIV_PERSONALITY_FRIENDLY:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*0*/ GC.getMINOR_APPROACH_FRIENDLY_PERSONALITY_FRIENDLY();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*0*/ GC.getMINOR_APPROACH_PROTECTIVE_PERSONALITY_PROTECTIVE();
		break;
	case MINOR_CIV_PERSONALITY_NEUTRAL:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*0*/ GC.getMINOR_APPROACH_FRIENDLY_PERSONALITY_NEUTRAL();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*0*/ GC.getMINOR_APPROACH_PROTECTIVE_PERSONALITY_NEUTRAL();
		break;
	case MINOR_CIV_PERSONALITY_HOSTILE:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*-1*/ GC.getMINOR_APPROACH_FRIENDLY_PERSONALITY_HOSTILE();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*-2*/ GC.getMINOR_APPROACH_PROTECTIVE_PERSONALITY_HOSTILE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += /*1*/ GC.getMINOR_APPROACH_CONQUEST_PERSONALITY_HOSTILE();
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 1; //antonjs: todo: constant/XML
		break;
	case MINOR_CIV_PERSONALITY_IRRATIONAL:
		viApproachWeights[MINOR_CIV_APPROACH_FRIENDLY] += /*0*/ GC.getMINOR_APPROACH_FRIENDLY_PERSONALITY_IRRATIONAL();
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += /*0*/ GC.getMINOR_APPROACH_PROTECTIVE_PERSONALITY_IRRATIONAL();
		break;
	}

	////////////////////////////////////
	// MINOR TRAIT
	////////////////////////////////////

	// Since Militaristic CS are harder to successfully bully
	if(GET_PLAYER(ePlayer).GetMinorCivAI()->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
	{
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += -2; //antonjs: todo: XML
	}

	////////////////////////////////////
	// TRIBUTE HISTORY - have we bullied this player before?  If so, we are more likely to keep bullying
	////////////////////////////////////
	if(GET_PLAYER(ePlayer).GetMinorCivAI()->IsEverBulliedByMajor(GetPlayer()->GetID()))
	{
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 2; //antonjs: todo: constant/XML
	}

	////////////////////////////////////
	// QUESTS - are there any active quests that might sway our decision?
	////////////////////////////////////
	if(GET_PLAYER(ePlayer).GetMinorCivAI()->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_PLEDGE_TO_PROTECT))
	{
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += 3; //antonjs: todo: constant/XML
	}

	////////////////////////////////////
	// TARGET VALUE - can we conquer these guys?
	////////////////////////////////////

	int iPTV = GetPlayerTargetValue(ePlayer);
	if (GetPlayer()->GetPlayerTraits()->GetCityStateCombatModifier() > 0)
	{
		iPTV++;
		iPTV = iPTV > TARGET_VALUE_SOFT ? TARGET_VALUE_SOFT : iPTV;
	}
	switch(iPTV)
	{
	case TARGET_VALUE_IMPOSSIBLE:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *= /*10*/ GC.getMINOR_APPROACH_WAR_TARGET_IMPOSSIBLE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	case TARGET_VALUE_BAD:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *= /*20*/ GC.getMINOR_APPROACH_WAR_TARGET_BAD();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	case TARGET_VALUE_AVERAGE:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *= /*40*/ GC.getMINOR_APPROACH_WAR_TARGET_AVERAGE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	case TARGET_VALUE_FAVORABLE:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *= /*110*/ GC.getMINOR_APPROACH_WAR_TARGET_FAVORABLE();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	case TARGET_VALUE_SOFT:
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *= /*130*/ GC.getMINOR_APPROACH_WAR_TARGET_SOFT();
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		break;
	default:
		break;
	}

	//////////////////////////////////////
	//// WAR PROJECTION - how do we think a war against ePlayer will go?
	//////////////////////////////////////

	//switch (GetWarProjection(ePlayer))
	//{
	//case WAR_PROJECTION_DESTRUCTION:
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*25*/ GC.getMINOR_APPROACH_WAR_PROJECTION_DESTRUCTION_PERCENT();
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
	//	break;
	//case WAR_PROJECTION_DEFEAT:
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*40*/ GC.getMINOR_APPROACH_WAR_PROJECTION_DEFEAT_PERCENT();
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
	//	break;
	//case WAR_PROJECTION_STALEMATE:
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*60*/ GC.getMINOR_APPROACH_WAR_PROJECTION_STALEMATE_PERCENT();
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
	//	break;
	//case WAR_PROJECTION_UNKNOWN:
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*70*/ GC.getMINOR_APPROACH_WAR_PROJECTION_UNKNOWN_PERCENT();
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
	//	break;
	//case WAR_PROJECTION_GOOD:
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*100*/ GC.getMINOR_APPROACH_WAR_PROJECTION_GOOD_PERCENT();
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
	//	break;
	//case WAR_PROJECTION_VERY_GOOD:
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] *= /*110*/ GC.getMINOR_APPROACH_WAR_PROJECTION_VERY_GOOD_PERCENT();
	//	viApproachWeights[MAJOR_CIV_APPROACH_WAR] /= 100;
	//	break;
	//}

	////////////////////////////////////
	// PEACE TREATY - have we made peace with this player before?  If so, reduce war weight
	////////////////////////////////////

	int iPeaceTreatyTurn = GET_TEAM(GetTeam()).GetTurnMadePeaceTreatyWithTeam(eTeam);
	if(iPeaceTreatyTurn > -1)
	{
		int iTurnsSincePeace = GC.getGame().getElapsedGameTurns() - iPeaceTreatyTurn;
		if(iTurnsSincePeace < /*25*/ GC.getTURNS_SINCE_PEACE_WEIGHT_DAMPENER())
		{
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] *= /*65*/ GC.getAPPROACH_WAR_HAS_MADE_PEACE_BEFORE_PERCENT();
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] /= 100;
		}
	}

	////////////////////////////////////
	// DEBUGGING
	////////////////////////////////////
	if(BULLY_DEBUGGING)
	{
		viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] += 50;
		viApproachWeights[MINOR_CIV_APPROACH_BULLY] += 0;
		viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] += 0;
	}

	////////////////////////////////////
	// RANDOM FACTOR
	////////////////////////////////////

	int iRandAmount;
	for(iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
	{
		// Increase weights to hundreds to give us more fidelity
		viApproachWeights[iApproachLoop] *= 100;

		iRandAmount = viApproachWeights[iApproachLoop] * /*15*/ GC.getAPPROACH_RANDOM_PERCENT();

		// If the amount is negative, only bad things can happen.  Plus, it's not likely we're going to pick this anyways
		if(iRandAmount > 0)
		{
			iRandAmount /= 100;
			viApproachWeights[iApproachLoop] += GC.getGame().getJonRandNumVA(iRandAmount, "Randomizing Diplo AI Minor Civ Approach Weight (%d; %d)", (int)GetPlayer()->GetID(), (int)ePlayer);
		}
	}

	////////////////////////////////////
	// CAN WE PLEDGE TO PROTECT?
	////////////////////////////////////
	if(!GET_PLAYER(ePlayer).GetMinorCivAI()->CanMajorProtect(GetPlayer()->GetID()))
	{
		// Disfavor protective if we can't actually pledge protection!
		if (viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] > 0)
			viApproachWeights[MINOR_CIV_APPROACH_PROTECTIVE] = 0;
	}

	////////////////////////////////////
	// CAN WE DECLARE WAR?
	////////////////////////////////////
	if(!GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(ePlayer).getTeam()))
	{
		if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
		{
			// Disfavor conquest if we can't even do war with them!
			if (viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] > 0)
				viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] = 0;
		}
	}
	
	////////////////////////////////////
	// ALLIES WITH MINOR?
	////////////////////////////////////
	if(GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlly() == GetPlayer()->GetID())
	{
		// Disfavor conquest and bullying if they are our ally

		if (viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] > 0)
			viApproachWeights[MINOR_CIV_APPROACH_CONQUEST] = 0;

		if (viApproachWeights[MINOR_CIV_APPROACH_BULLY] > 0)
			viApproachWeights[MINOR_CIV_APPROACH_BULLY] = 0;
	}

	// This vector is what we'll use to sort
	CvWeightedVector< MinorCivApproachTypes, 128 > vApproachWeightsForSorting;
	vApproachWeightsForSorting.clear();

	// Transfer values from our normal int vector (which we need for logging) to the Weighted Vector we can sort
	for(iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
	{
		vApproachWeightsForSorting.push_back((MinorCivApproachTypes) iApproachLoop, viApproachWeights[iApproachLoop]);
	}

	vApproachWeightsForSorting.SortItems();

	eApproach = vApproachWeightsForSorting.GetElement(0);
	iHighestWeight = vApproachWeightsForSorting.GetWeight(0);

	// Don't want to log if we're just seeing what the highest weight is and don't care about what Approach we like
	if(bLog)
	{
		LogMinorCivApproachUpdate(ePlayer, viApproachWeights.begin(), eApproach, eOldApproach);
	}

	return eApproach;
}

/// What is our Diplomatic approach to this Minor Civ?
MinorCivApproachTypes CvDiplomacyAI::GetMinorCivApproach(PlayerTypes ePlayer) const
{
	// Remove the Majors from here, since we're only actually storing Data for the Minors
	PlayerTypes eMinor = (PlayerTypes)((int) ePlayer - MAX_MAJOR_CIVS);

	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMinor < MAX_MINOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (MinorCivApproachTypes) m_paeMinorCivApproach[eMinor];
}

/// Sets what our Diplomatic approach is to a Minor Civ
void CvDiplomacyAI::SetMinorCivApproach(PlayerTypes ePlayer, MinorCivApproachTypes eApproach)
{
	// Remove the Majors from here, since we're only actually storing Data for the Minors
	PlayerTypes eMinor = (PlayerTypes)((int) ePlayer - MAX_MAJOR_CIVS);

	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMinor < MAX_MINOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach >= NO_MINOR_CIV_APPROACH, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");		// NO_MINOR_CIV_APPROACH is valid because we use it to reset at the start of every turn.  We have an assert to test -1 there.
	CvAssertMsg(eApproach < NUM_MINOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeMinorCivApproach[eMinor] = eApproach;
}

/// How many Minors do we have a particular Approach towards?
int CvDiplomacyAI::GetNumMinorCivApproach(MinorCivApproachTypes eApproach) const
{
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MINOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MinorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iCount = 0;

	for(int iMinorLoop = 0; iMinorLoop < MAX_MINOR_CIVS; iMinorLoop++)
	{
		if(GetMinorCivApproach((PlayerTypes) iMinorLoop) == eApproach)
		{
			iCount++;
		}
	}

	return iCount;
}

/// Does this AI want to connect to a minor with a route
void CvDiplomacyAI::SetWantToRouteConnectToMinor(PlayerTypes eMinor, bool bWant)
{
	int iArrayIndex = eMinor - MAX_MAJOR_CIVS;
	CvAssertMsg(iArrayIndex >= 0 && iArrayIndex < MAX_MINOR_CIVS, "DIPLOMACY_AI: Index into array is out of bounds");

	if(IsWantToRouteConnectToMinor(eMinor) != bWant)
		m_pabWantToRouteToMinor[iArrayIndex] = bWant;
}

/// Does this AI want to connect to a minor with a route
bool CvDiplomacyAI::IsWantToRouteConnectToMinor(PlayerTypes eMinor)
{
	int iArrayIndex = eMinor - MAX_MAJOR_CIVS;
	CvAssertMsg(iArrayIndex >= 0 && iArrayIndex < MAX_MINOR_CIVS, "DIPLOMACY_AI: Index into array is out of bounds");
	return m_pabWantToRouteToMinor[iArrayIndex];
}

/// Does this AI have a gold quest active with any minor civ?
bool CvDiplomacyAI::IsHasActiveGoldQuest()
{
	//antonjs: consider: optimize
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
		if(GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_GIVE_GOLD))
			return true;
		if(GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_INVEST))
			return true;
	}
	return false;
}


/// Our guess as to another player's approach towards us
MajorCivApproachTypes CvDiplomacyAI::GetApproachTowardsUsGuess(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return GET_PLAYER(ePlayer).GetDiplomacyAI()->GetMajorCivApproach(GetPlayer()->GetID(), /*bHideTrueFeelings*/ true);

	//return (MajorCivApproachTypes) m_paeApproachTowardsUsGuess[ePlayer];
}

/// Sets our guess as to another player's approach towards us
void CvDiplomacyAI::SetApproachTowardsUsGuess(PlayerTypes ePlayer, MajorCivApproachTypes eApproach)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MAJOR_CIV_APPROACHES, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeApproachTowardsUsGuess[ePlayer] = eApproach;
}

/// How long have we thought ePlayer has had his Approach towards us
int CvDiplomacyAI::GetApproachTowardsUsGuessCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paeApproachTowardsUsGuessCounter[ePlayer];
}

/// Sets how long we've thought ePlayer has had his Approach towards us
void CvDiplomacyAI::SetApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting ApproachTowardsUsGuessCounter to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeApproachTowardsUsGuessCounter[ePlayer] = iValue;
}

/// Changes how long we've thought ePlayer has had his Approach towards us
void CvDiplomacyAI::ChangeApproachTowardsUsGuessCounter(PlayerTypes ePlayer, int iChange)
{
	SetApproachTowardsUsGuessCounter(ePlayer, GetApproachTowardsUsGuessCounter(ePlayer) + iChange);
}

/// See if there's anything we need to change with our guesses as to other players' Approaches towards us
void CvDiplomacyAI::DoUpdateApproachTowardsUsGuesses()
{
	PlayerTypes eLoopPlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			// We have a guess as to what another player's Approach is towards us
			if(GetApproachTowardsUsGuess(eLoopPlayer) != MAJOR_CIV_APPROACH_NEUTRAL)
			{
				ChangeApproachTowardsUsGuessCounter(eLoopPlayer, 1);

				if(GetApproachTowardsUsGuessCounter(eLoopPlayer) > 30)
				{
					SetApproachTowardsUsGuess(eLoopPlayer, MAJOR_CIV_APPROACH_NEUTRAL);
					SetApproachTowardsUsGuessCounter(eLoopPlayer, 0);
				}
			}
		}
	}
}



// ************************************
// Demands
// ************************************



/// Is there a player we're targeting to make a demand from, backed with force?
PlayerTypes CvDiplomacyAI::GetDemandTargetPlayer() const
{
	return (PlayerTypes) m_eDemandTargetPlayer;
}

/// Sets a player we're targeting to make a demand from, backed with force
void CvDiplomacyAI::SetDemandTargetPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= NO_PLAYER, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");	// We could be targeting no one to make a demand (duh)
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_eDemandTargetPlayer = ePlayer;
}

/// Update out desire to make a demand from a player
void CvDiplomacyAI::DoUpdateDemands()
{
	CvWeightedVector<PlayerTypes, MAX_MAJOR_CIVS, true> veDemandTargetPlayers;

	int iWeight;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			// Is eLoopPlayer a good target for making a demand?
			if(IsPlayerDemandAttractive(eLoopPlayer))
			{
				iWeight = GC.getGame().getJonRandNumVA(100, "DIPLOMACY_AI: Random weight for player to make demand of. (%d; %d)", (int)GetPlayer()->GetID(), (int)eLoopPlayer);
				veDemandTargetPlayers.push_back(eLoopPlayer, iWeight);
			}
		}
	}

	bool bCancelDemand;

	// Any valid possibilities?
	if(veDemandTargetPlayers.size() > 0)
	{
		bCancelDemand = false;

		// Only assign a player to be the target if we don't already have one
		if(GetDemandTargetPlayer() == NO_PLAYER)
		{
			veDemandTargetPlayers.SortItems();

			int iChanceOfDemand = /*100*/ GC.getDEMAND_RAND();	// Maybe change this later to a lower value (10%?) - leaving it at 100 for now because the AI has a bit of trouble getting everything together to make a demand right now

			if(iChanceOfDemand > GC.getGame().getJonRandNum(100, "DIPLOMACY_AI: Should AI make demand of player it's hostile towards?"))
			{
				DoStartDemandProcess(veDemandTargetPlayers.GetElement(0));
			}
		}
	}
	// No one we're hostile towards to make a demand of
	else
	{
		bCancelDemand = true;
	}

	// If we're planning on making a demand, make sure it's still a good idea
	if(GetDemandTargetPlayer() != NO_PLAYER)
	{
		if(!IsPlayerDemandAttractive(GetDemandTargetPlayer()))
		{
			bCancelDemand = true;
		}
	}

	// We're not hostile towards any one so cancel any demand work we have underway (if there's anything going on)
	if(bCancelDemand)
	{
		DoCancelHaltDemandProcess();
	}

	// See If we have a demand ready to make
	DoTestDemandReady();
}

/// AI has picked someone to make a demand of... what does this mean?
void CvDiplomacyAI::DoStartDemandProcess(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	SetDemandTargetPlayer(ePlayer);

	CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetShowOfForceOperation((ePlayer));

	// Not yet readying an attack
	if(pOperation == NULL && !IsMusteringForAttack(ePlayer))	// If we're "mustering" it means we had a Sneak Attack Operation that finished
	{
		if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
		{
			if(GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(ePlayer).getTeam()))
			{
				GetPlayer()->GetMilitaryAI()->RequestShowOfForce(ePlayer);
				SetWarGoal(ePlayer, WAR_GOAL_DEMAND);
			}
		}
	}
}

/// Stop any current progress towards making a demand
void CvDiplomacyAI::DoCancelHaltDemandProcess()
{
	PlayerTypes eDemandTarget = GetDemandTargetPlayer();

	// Are we actually targeting anyone for a demand?
	if(eDemandTarget != NO_PLAYER)
	{
		//CvAssertMsg(GetWarGoal(eDemandTarget) == WAR_GOAL_DEMAND, "DIPLOMACY_AI: AI was preparing to make a demand, but somehow doesn't have an appropriate WarGoal.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		if(GetWarGoal(eDemandTarget) == WAR_GOAL_DEMAND)
		{
			SetWarGoal(eDemandTarget, NO_WAR_GOAL_TYPE);

			// Get rid of the operation to put Units near them
			CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetShowOfForceOperation(eDemandTarget);
			if(pOperation != NULL)
			{
				pOperation->Kill(AI_ABORT_DIPLO_OPINION_CHANGE);
				SetMusteringForAttack(eDemandTarget, false);
			}
		}

		SetDemandTargetPlayer(NO_PLAYER);
		SetDemandReady(false);
	}
}

/// Are we ready to make a demand, backed with force?
void CvDiplomacyAI::DoTestDemandReady()
{
	PlayerTypes eDemandTarget = GetDemandTargetPlayer();

	// Are we actually targeting anyone for a demand?
	if(eDemandTarget != NO_PLAYER)
	{
		if(GetWarGoal(eDemandTarget) == WAR_GOAL_DEMAND)
		{
			CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetShowOfForceOperation(eDemandTarget);

			if(pOperation)
			{
				if(!IsAtWar(eDemandTarget))
				{
					// If we're at least 85% of the way to our objective, let loose the dogs of war!
					if(IsMusteringForAttack(eDemandTarget) || (pOperation != NULL && pOperation->PercentFromMusterPointToTarget() >= 85))	// If we're "mustering" it means we have a Sneak Attack Operation that's in position to attack
					{
						SetMusteringForAttack(eDemandTarget, false);

						SetDemandReady(true);
					}
				}
			}
		}
	}
}

/// Are we ready to make a demand to GetDemandTargetPlayer?
bool CvDiplomacyAI::IsDemandReady()
{
	return m_bDemandReady;
}

/// Sets that we are ready to make a demand to GetDemandTargetPlayer (or not)
void CvDiplomacyAI::SetDemandReady(bool bValue)
{
	m_bDemandReady = bValue;
}

/// Are we willing to make a demand of ePlayer?
bool CvDiplomacyAI::IsPlayerDemandAttractive(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Being hostile towards this guy? (ignore war face, as if we're planning a war already, making a demand doesn't mesh well with that)
	if(GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false) != MAJOR_CIV_APPROACH_HOSTILE)
	{
		return false;
	}

	// Player can't be distant
	if(GetPlayer()->GetProximityToPlayer(ePlayer) < PLAYER_PROXIMITY_FAR)
	{
		return false;
	}

	// If they're a bad or impossible target then that's not good at all for us
	if(GetPlayerTargetValue(ePlayer) < TARGET_VALUE_AVERAGE)
	{
		return false;
	}

	// Don't make demands of them too often
	if(GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_DEMAND) < 40)
	{
		return false;
	}

	// If we're planning a war or at war with anyone, making a demand is unwise
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GetWarGoal(eLoopPlayer) == WAR_GOAL_PREPARE)
		{
			return false;
		}

		if(IsAtWar(eLoopPlayer))
		{
			return false;
		}
	}

	// Player has to be on the same area as us
	if(GetPlayer()->getCapitalCity() != NULL)
	{
		if(GET_PLAYER(ePlayer).getCapitalCity() != NULL)
		{
			if(GetPlayer()->getCapitalCity()->getArea() == GET_PLAYER(ePlayer).getCapitalCity()->getArea())
			{
				return true;
			}
		}
	}

	return false;
}



/////////////////////////////////////////////////////////
// Requests
/////////////////////////////////////////////////////////




/// Is this AI willing to make a request of ePlayer
bool CvDiplomacyAI::IsMakeRequest(PlayerTypes ePlayer, CvDeal* pDeal, bool& bRandPassed)
{
	bool bFriendly = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true) == MAJOR_CIV_APPROACH_FRIENDLY;

	if(bFriendly && IsDoFAccepted(ePlayer))
	{
		// Is there something we want?
		bool bWantsSomething = false;
		// Is there a strong reason why we want something? (added to rand roll)
		int iWeightBias = 0;

		// Luxury Request
		if(!bWantsSomething)
			bWantsSomething = IsLuxuryRequest(ePlayer, pDeal, iWeightBias);
		// Gold Request
		if(!bWantsSomething)
			bWantsSomething = IsGoldRequest(ePlayer, pDeal, iWeightBias);

		if(bWantsSomething)
		{
			// Random element
			int iRand = GC.getGame().getJonRandNumVA(100, "Diplomacy AI: Friendly civ request roll. (%d; %d)", (int)GetPlayer()->GetID(), (int)ePlayer);

			iRand += iWeightBias;

			if(iRand >= 67)
			{
				bRandPassed = true;
				return true;
			}
			else
			{
				bRandPassed = false;
				return false;
			}
		}
	}

	return false;
}

/// Does this AI want something?
bool CvDiplomacyAI::IsLuxuryRequest(PlayerTypes ePlayer, CvDeal* pDeal, int& iWeightBias)
{
	iWeightBias = 0;

	ResourceTypes eLuxuryToAskFor = NO_RESOURCE;

	int iResourceLoop;

	// See if the other player has a Resource to trade
	for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);

		CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if(pkResourceInfo)
		{
			// Only look at Luxuries
			if(pkResourceInfo->getResourceUsage() != RESOURCEUSAGE_LUXURY)
				continue;

			// Any extras?
			if(GET_PLAYER(ePlayer).getNumResourceAvailable(eResource, /*bIncludeImport*/ false) < 2)
				continue;

			// Can they actually give us this item
			if(!pDeal->IsPossibleToTradeItem(ePlayer, GetPlayer()->GetID(), TRADE_ITEM_RESOURCES, eResource, 1))
				continue;

			eLuxuryToAskFor = eResource;
			break;
		}
	}

	// Didn't find something they could give us?
	if(eLuxuryToAskFor == NO_RESOURCE)
		return false;

	// See if there's any Luxuries WE can trade (because if there are then we shouldn't be asking for hand outs)
	for(iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
	{
		const ResourceTypes eResource = static_cast<ResourceTypes>(iResourceLoop);

		CvResourceInfo* pkResource = GC.getResourceInfo(eResource);
		if(pkResource)
		{
			// Only look at Luxuries
			if(pkResource->getResourceUsage() != RESOURCEUSAGE_LUXURY)
				continue;

			// Any extras?
			if(GetPlayer()->getNumResourceAvailable(eResource, /*bIncludeImport*/ false) < 2)
				continue;

			// Can they actually give us this item
			if(!pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), ePlayer, TRADE_ITEM_RESOURCES, eResource, 1))
				continue;

			// Found something we can trade to them, so abort
			return false;
		}
	}

	// Add a little something extra since we're in dire straights
	if(GetPlayer()->IsEmpireUnhappy())
		iWeightBias += 25;

	// Now seed the deal
	pDeal->AddResourceTrade(ePlayer, eLuxuryToAskFor, 1, GC.getGame().GetDealDuration());

	return true;
}

/// Does this AI want something?
bool CvDiplomacyAI::IsGoldRequest(PlayerTypes ePlayer, CvDeal* pDeal, int& iWeightBias)
{
	iWeightBias = 0;

	int iOurGold = GetPlayer()->GetTreasury()->GetGold();
	int iOurGPT = GetPlayer()->calculateGoldRate();
	int iOurExpenses = GetPlayer()->GetTreasury()->CalculateInflatedCosts();
	int iOurGrossIncome = iOurGPT + iOurExpenses;

	// If we have no expenses, don't ask (and also don't crash)
	if(iOurExpenses == 0)
		return false;

	// If we already have some gold saved up then don't bother
	if(iOurGold > 100)
		return false;

	// If we're making 35% more than we're spending then don't ask, we're doing alright
	if(iOurGrossIncome * 100 / iOurExpenses > 135)
		return false;

	int iTheirGold = GET_PLAYER(ePlayer).GetTreasury()->GetGold();
	int iTheirGPT = GET_PLAYER(ePlayer).calculateGoldRate();
	int iTheirExpenses = GET_PLAYER(ePlayer).GetTreasury()->CalculateInflatedCosts();
	int iTheirGrossIncome = iTheirGPT + iTheirExpenses;

	// Don't divide by zero please
	if(iTheirExpenses != 0)
	{
		// If they're making less than 35% more than they're spending then don't ask, they're not in great shape
		if(iTheirGrossIncome * 100 / iTheirExpenses < 135)
			return false;
	}
	else if(iTheirGPT <= iOurGPT)
	{
		return false;
	}

	// Add a little something extra since we're in dire straights
	if(iOurGPT < 0)
		iWeightBias += 25;

	// If we've made it this far we'd like to ask, so figure out how much we want to ask for
	int iGoldToAskFor = iTheirGPT * GC.getGame().GetDealDuration() / 5;
	int iGPTToAskFor = 0;

	if(iGoldToAskFor > iTheirGold)
	{
		iGoldToAskFor = 0;
		iGPTToAskFor = max(1, iTheirGPT / 6);
	}

	// Now seed the deal
	if(iGoldToAskFor > 0)
		pDeal->AddGoldTrade(ePlayer, iGoldToAskFor);
	else if(iGPTToAskFor > 0)
		pDeal->AddGoldPerTurnTrade(ePlayer, iGPTToAskFor, GC.getGame().GetDealDuration());

	return true;
}

/// Are we willing to swap embassies with ePlayer?
bool CvDiplomacyAI::IsEmbassyExchangeAcceptable(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

	switch(eApproach)
	{
	case MAJOR_CIV_APPROACH_WAR:
	case MAJOR_CIV_APPROACH_HOSTILE:
	case MAJOR_CIV_APPROACH_GUARDED:
		return false;
	case MAJOR_CIV_APPROACH_DECEPTIVE:
	case MAJOR_CIV_APPROACH_AFRAID:
	case MAJOR_CIV_APPROACH_FRIENDLY:
	case MAJOR_CIV_APPROACH_NEUTRAL:
		return true;
	}

	return false;
}

/// Do we want to have an embassy in the player's capital?
bool CvDiplomacyAI::WantsEmbassyAtPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// May want to make this logic more sophisticated eventually.  This will do for now

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE ||
	        eApproach == MAJOR_CIV_APPROACH_WAR)
	{
		return false;
	}

	return true;
}

/// Do we want Open Borders with eOtherPlayer? - this is only used for when to trigger an AI request, not whether or not the AI will accept a deal period
bool CvDiplomacyAI::IsWantsOpenBordersWithPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// May want to make this logic more sophisticated eventually.  This will do for now

	if(GetPlayer()->GetProximityToPlayer(ePlayer) != PLAYER_PROXIMITY_NEIGHBORS)
	{
		return false;
	}

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE || eApproach == MAJOR_CIV_APPROACH_GUARDED || eApproach == MAJOR_CIV_APPROACH_AFRAID)
	{
		return false;
	}

	switch(GetPlayerMilitaryStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_IMMENSE:
	case STRENGTH_POWERFUL:
	case STRENGTH_STRONG:
		return false;
	}

	return true;
}

/// Are we willing to swap Open Borders with ePlayer?
bool CvDiplomacyAI::IsOpenBordersExchangeAcceptable(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

	if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		return true;
	else if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		return true;

	return false;
}

// ************************************
// Peace!
// ************************************



/// Do we want to make peace with anyone Minors we're at war with?
void CvDiplomacyAI::DoMakePeaceWithMinors()
{
	if((int)m_eTargetPlayer >= (int)DIPLO_FIRST_PLAYER)
		return;

	PlayerTypes eLoopPlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				if(IsAtWar(eLoopPlayer))
				{
					if(IsWantsPeaceWithPlayer(eLoopPlayer) && GET_TEAM(GetPlayer()->getTeam()).GetNumTurnsLockedIntoWar(GET_PLAYER(eLoopPlayer).getTeam()) == 0)	// Locked into war for a period of time? (coop war, war deal, etc.)
					{
						if(!GET_PLAYER(eLoopPlayer).GetMinorCivAI()->IsPeaceBlocked(GetPlayer()->getTeam()))
						{
							GET_TEAM(GetTeam()).makePeace(GET_PLAYER(eLoopPlayer).getTeam());

							LogPeaceMade(eLoopPlayer);
						}
					}
				}
			}
		}
	}
}

/// Updates what peace treaties we're willing to offer and accept
void CvDiplomacyAI::DoUpdatePeaceTreatyWillingness()
{
	PeaceTreatyTypes eTreatyWillingToOffer, eTreatyWillingToAccept;
	int iWillingToOfferScore, iWillingToAcceptScore;
	WarProjectionTypes eWarProjection;

	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	TeamTypes eLoopTeam;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

			eTreatyWillingToOffer = NO_PEACE_TREATY_TYPE;
			eTreatyWillingToAccept = NO_PEACE_TREATY_TYPE;
			iWillingToOfferScore = 0;
			iWillingToAcceptScore = 0;

			if(IsAtWar(eLoopPlayer))
			{
				// Have to be at war with the human for a certain amount of time before the AI will agree to peace
				if(GET_PLAYER(eLoopPlayer).isHuman())
				{
					if(!IsWillingToMakePeaceWithHuman(eLoopPlayer))
					{
						SetTreatyWillingToOffer(eLoopPlayer, NO_PEACE_TREATY_TYPE);
						SetTreatyWillingToAccept(eLoopPlayer, NO_PEACE_TREATY_TYPE);

						continue;
					}
				}

				// If we're out for conquest then no peace!
				if(GetWarGoal(eLoopPlayer) != WAR_GOAL_CONQUEST)
				{
					eWarProjection = GetWarProjection(eLoopPlayer);

					// What we're willing to give up.  The higher the number the more we're willing to part with

//					if (IsWantsPeaceWithPlayer(eLoopPlayer))
					{
						// How is the war going?
						switch(eWarProjection)
						{
						case WAR_PROJECTION_DESTRUCTION:
							iWillingToOfferScore += /*100*/ GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_DESTRUCTION();
							break;
						case WAR_PROJECTION_DEFEAT:
							iWillingToOfferScore += /*60*/ GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_DEFEAT();
							break;
						case WAR_PROJECTION_STALEMATE:
							iWillingToOfferScore += /*20*/ GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_STALEMATE();
							break;
						case WAR_PROJECTION_UNKNOWN:
							iWillingToOfferScore += /*0*/ GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_UNKNOWN();
							break;
						case WAR_PROJECTION_GOOD:
							iWillingToOfferScore += /*-20*/ GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_GOOD();
							break;
						case WAR_PROJECTION_VERY_GOOD:
							iWillingToOfferScore += /*-50*/ GC.getPEACE_WILLINGNESS_OFFER_PROJECTION_VERY_GOOD();
							break;
						default:
							break;
						}

						// How much damage have we taken?
						switch(GetWarDamageLevel(eLoopPlayer))
						{
						case WAR_DAMAGE_LEVEL_NONE:
							iWillingToOfferScore += /*0*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_NONE();
							break;
						case WAR_DAMAGE_LEVEL_MINOR:
							iWillingToOfferScore += /*10*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MINOR();
							break;
						case WAR_DAMAGE_LEVEL_MAJOR:
							iWillingToOfferScore += /*20*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MAJOR();
							break;
						case WAR_DAMAGE_LEVEL_SERIOUS:
							iWillingToOfferScore += /*50*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_SERIOUS();
							break;
						case WAR_DAMAGE_LEVEL_CRIPPLED:
							iWillingToOfferScore += /*80*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_CRIPPLED();
							break;
						default:
							break;
						}

						// How much damage have we dished out?
						switch(GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetWarDamageLevel(GetPlayer()->GetID()))
						{
						case WAR_DAMAGE_LEVEL_NONE:
							iWillingToOfferScore -= /*0*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_NONE();
							break;
						case WAR_DAMAGE_LEVEL_MINOR:
							iWillingToOfferScore -= /*10*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MINOR();
							break;
						case WAR_DAMAGE_LEVEL_MAJOR:
							iWillingToOfferScore -= /*20*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_MAJOR();
							break;
						case WAR_DAMAGE_LEVEL_SERIOUS:
							iWillingToOfferScore -= /*50*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_SERIOUS();
							break;
						case WAR_DAMAGE_LEVEL_CRIPPLED:
							iWillingToOfferScore -= /*80*/ GC.getPEACE_WILLINGNESS_OFFER_WAR_DAMAGE_CRIPPLED();
							break;
						default:
							break;
						}

						// Do the final assessment
						if(iWillingToOfferScore >= /*180*/ GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_UN_SURRENDER())
							eTreatyWillingToOffer = PEACE_TREATY_UNCONDITIONAL_SURRENDER;
						else if(iWillingToOfferScore >= /*150*/ GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_CAPITULATION())
							eTreatyWillingToOffer = PEACE_TREATY_CAPITULATION;
						else if(iWillingToOfferScore >= /*120*/ GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_CESSION())
							eTreatyWillingToOffer = PEACE_TREATY_CESSION;
						else if(iWillingToOfferScore >= /*95*/ GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_SURRENDER())
							eTreatyWillingToOffer = PEACE_TREATY_SURRENDER;
						else if(iWillingToOfferScore >= /*70*/ GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_SUBMISSION())
							eTreatyWillingToOffer = PEACE_TREATY_SUBMISSION;
						else if(iWillingToOfferScore >= /*55*/ GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_BACKDOWN())
							eTreatyWillingToOffer = PEACE_TREATY_BACKDOWN;
						else if(iWillingToOfferScore >= /*40*/ GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_SETTLEMENT())
							eTreatyWillingToOffer = PEACE_TREATY_SETTLEMENT;
						else if(iWillingToOfferScore >= /*20*/ GC.getPEACE_WILLINGNESS_OFFER_THRESHOLD_ARMISTICE())
							eTreatyWillingToOffer = PEACE_TREATY_ARMISTICE;
						else	// War Score could be negative here, but we're already assuming this player wants peace.  But he's not willing to give up anything for it
							eTreatyWillingToOffer = PEACE_TREATY_WHITE_PEACE;

						// If they've broken a peace deal before then we're not going to give them anything
						if(GET_TEAM(eLoopTeam).IsHasBrokenPeaceTreaty())
						{
							if(eTreatyWillingToOffer > PEACE_TREATY_WHITE_PEACE)
								eTreatyWillingToOffer = PEACE_TREATY_WHITE_PEACE;
						}
					}

					// What we're willing to accept from eLoopPlayer.  The higher the number the more we want

					// How is the war going?
					switch(eWarProjection)
					{
					case WAR_PROJECTION_DESTRUCTION:
						iWillingToAcceptScore += /*-50*/ GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_DESTRUCTION();
						break;
					case WAR_PROJECTION_DEFEAT:
						iWillingToAcceptScore += /*-20*/ GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_DEFEAT();
						break;
					case WAR_PROJECTION_STALEMATE:
						iWillingToAcceptScore += /*-10*/ GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_STALEMATE();
						break;
					case WAR_PROJECTION_UNKNOWN:
						iWillingToAcceptScore += /*0*/ GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_UNKNOWN();
						break;
					case WAR_PROJECTION_GOOD:
						iWillingToAcceptScore += /*50*/ GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_GOOD();
						break;
					case WAR_PROJECTION_VERY_GOOD:
						iWillingToAcceptScore += /*100*/ GC.getPEACE_WILLINGNESS_ACCEPT_PROJECTION_VERY_GOOD();
						break;
					default:
						break;
					}

					// How easy would it be for us to squash them?
					switch(GetPlayerTargetValue(eLoopPlayer))
					{
					case TARGET_VALUE_IMPOSSIBLE:
						iWillingToAcceptScore += /*-50*/ GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_IMPOSSIBLE();
						break;
					case TARGET_VALUE_BAD:
						iWillingToAcceptScore += /*-20*/ GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_BAD();
						break;
					case TARGET_VALUE_AVERAGE:
						iWillingToAcceptScore += /*0*/ GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_AVERAGE();
						break;
					case TARGET_VALUE_FAVORABLE:
						iWillingToAcceptScore += /*20*/ GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_FAVORABLE();
						break;
					case TARGET_VALUE_SOFT:
						iWillingToAcceptScore += /*50*/ GC.getPEACE_WILLINGNESS_ACCEPT_TARGET_SOFT();
						break;
					default:
						break;
					}

					// Do the final assessment
					if(iWillingToAcceptScore >= /*150*/ GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_UN_SURRENDER())
						eTreatyWillingToAccept = PEACE_TREATY_UNCONDITIONAL_SURRENDER;
					else if(iWillingToAcceptScore >= /*115*/ GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CAPITULATION())
						eTreatyWillingToAccept = PEACE_TREATY_CAPITULATION;
					else if(iWillingToAcceptScore >= /*80*/ GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_CESSION())
						eTreatyWillingToAccept = PEACE_TREATY_CESSION;
					else if(iWillingToAcceptScore >= /*65*/ GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SURRENDER())
						eTreatyWillingToAccept = PEACE_TREATY_SURRENDER;
					else if(iWillingToAcceptScore >= /*50*/ GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SUBMISSION())
						eTreatyWillingToAccept = PEACE_TREATY_SUBMISSION;
					else if(iWillingToAcceptScore >= /*35*/ GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_BACKDOWN())
						eTreatyWillingToAccept = PEACE_TREATY_BACKDOWN;
					else if(iWillingToAcceptScore >= /*20*/ GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_SETTLEMENT())
						eTreatyWillingToAccept = PEACE_TREATY_SETTLEMENT;
					else if(iWillingToAcceptScore >= /*10*/ GC.getPEACE_WILLINGNESS_ACCEPT_THRESHOLD_ARMISTICE())
						eTreatyWillingToAccept = PEACE_TREATY_ARMISTICE;
					else// if (iWillingToAcceptScore >= /*0*/ GC.get())
						eTreatyWillingToAccept = PEACE_TREATY_WHITE_PEACE;

					// If we're losing all wars then let's go ahead and accept a white peace
					if (GetStateAllWars() == STATE_ALL_WARS_LOSING)
					{
						eTreatyWillingToAccept = PEACE_TREATY_WHITE_PEACE;
					}
				}
			}

			SetTreatyWillingToOffer(eLoopPlayer, eTreatyWillingToOffer);
			SetTreatyWillingToAccept(eLoopPlayer, eTreatyWillingToAccept);
		}
	}
}

/// Need some special rules for humans so that the AI isn't exploited
bool CvDiplomacyAI::IsWillingToMakePeaceWithHuman(PlayerTypes ePlayer)
{
	CvPlayer& kHumanPlayer = GET_PLAYER(ePlayer);
	if (kHumanPlayer.isHuman())
	{
		bool bWillMakePeace = GetPlayerNumTurnsAtWar(ePlayer) >= 5;

		if(!GET_TEAM(m_pPlayer->getTeam()).canChangeWarPeace(kHumanPlayer.getTeam()))
		{
			return false;
		}

		// If either of us are locked in, then we're not willing to make peace (this prevents weird greetings and stuff) - we use > 1 because it'll get decremented after it appears the human make peace again
		if(GET_TEAM(GetPlayer()->getTeam()).GetNumTurnsLockedIntoWar(kHumanPlayer.getTeam()) > 1)
			return false;
		if(GET_TEAM(kHumanPlayer.getTeam()).GetNumTurnsLockedIntoWar(GetPlayer()->getTeam()) > 1)
			return false;

		return bWillMakePeace;
	}
	return true;
}

// What are we willing to give up to ePlayer to make peace?
PeaceTreatyTypes CvDiplomacyAI::GetTreatyWillingToOffer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (PeaceTreatyTypes) m_paePeaceTreatyWillingToOffer[ePlayer];
}

// Sets what are we willing to give up to ePlayer to make peace
void CvDiplomacyAI::SetTreatyWillingToOffer(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTreaty >= NO_PEACE_TREATY_TYPE, "DIPLOMACY_AI: Invalid PeaceTreatyType Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTreaty < NUM_PEACE_TREATY_TYPES, "DIPLOMACY_AI: Invalid PeaceTreatyType Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePeaceTreatyWillingToOffer[ePlayer] = eTreaty;
}

// What are we willing to accept from ePlayer to make peace?
PeaceTreatyTypes CvDiplomacyAI::GetTreatyWillingToAccept(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (PeaceTreatyTypes) m_paePeaceTreatyWillingToAccept[ePlayer];
}

// Sets what are we willing to accept from ePlayer to make peace
void CvDiplomacyAI::SetTreatyWillingToAccept(PlayerTypes ePlayer, PeaceTreatyTypes eTreaty)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTreaty >= NO_PEACE_TREATY_TYPE, "DIPLOMACY_AI: Invalid PeaceTreatyType Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTreaty < NUM_PEACE_TREATY_TYPES, "DIPLOMACY_AI: Invalid PeaceTreatyType Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePeaceTreatyWillingToAccept[ePlayer] = eTreaty;
}

/// Do we actually want peace with ePlayer?
bool CvDiplomacyAI::IsWantsPeaceWithPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iRequestPeaceTurnThreshold = /*4*/ GC.getREQUEST_PEACE_TURN_THRESHOLD();

	if(!GET_TEAM(m_pPlayer->getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam()))
	{
		return false;
	}

	if(GetWantPeaceCounter(ePlayer) >= iRequestPeaceTurnThreshold)
	{
		return true;
	}

	return false;
}


/// How long have we had our current War Goal with ePlayer?
int CvDiplomacyAI::GetWantPeaceCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarGoalTypes) m_paeWantPeaceCounter[ePlayer];
}

/// Sets how long we've had our current War Goal with ePlayer
void CvDiplomacyAI::SetWantPeaceCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting WantPeaceCounter to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeWantPeaceCounter[ePlayer] = iValue;
}

/// Changes how long we've had our current War Goal with ePlayer
void CvDiplomacyAI::ChangeWantPeaceCounter(PlayerTypes ePlayer, int iChange)
{
	SetWantPeaceCounter(ePlayer, GetWantPeaceCounter(ePlayer) + iChange);
}



// ************************************
// War!
// ************************************



/// Handles declarations of War for this AI
void CvDiplomacyAI::DoMakeWarOnPlayer(PlayerTypes eTargetPlayer)
{
	CvAIOperation* pOperation;
	bool bWantToAttack = false;
	bool bDeclareWar = false;

	if(!IsPlayerValid(eTargetPlayer))
		return;

	if(GetWarGoal(eTargetPlayer) == WAR_GOAL_DEMAND)
		return;

	if(IsAtWar(eTargetPlayer))
		return;

	bool bAtWarWithAtLeastOneMajor = MilitaryAIHelpers::IsTestStrategy_AtWar(m_pPlayer);

	// Minor Civ
	if(GET_PLAYER(eTargetPlayer).isMinorCiv())
	{
		bWantToAttack = !bAtWarWithAtLeastOneMajor && (GetMinorCivApproach(eTargetPlayer) == MINOR_CIV_APPROACH_CONQUEST);
		pOperation = GetPlayer()->GetMilitaryAI()->GetCityStateAttackOperation(eTargetPlayer);
	}
	// Major Civ
	else
	{
		MajorCivApproachTypes eApproach = GetMajorCivApproach(eTargetPlayer, /*bHideTrueFeelings*/ false);
		bWantToAttack = (eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_DECEPTIVE);
		bWantToAttack = bWantToAttack && !bAtWarWithAtLeastOneMajor; // let's not get into another war right now
		pOperation = GetPlayer()->GetMilitaryAI()->GetSneakAttackOperation(eTargetPlayer);
	}

	// Not yet readying an attack
	if(pOperation == NULL && !IsMusteringForAttack(eTargetPlayer))	// If we're "mustering" it means we had a Sneak Attack Operation that finished
	{
		if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eTargetPlayer).getTeam()))
		{
			if(GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(eTargetPlayer).getTeam()))
			{
				// Want to declare war on someone
				if(bWantToAttack)
				{
					SetWarGoal(eTargetPlayer, WAR_GOAL_PREPARE);

					// Attack on minor
					if(GET_PLAYER(eTargetPlayer).isMinorCiv())
						GetPlayer()->GetMilitaryAI()->RequestCityStateAttack(eTargetPlayer);
					// Attack on major
					else
						GetPlayer()->GetMilitaryAI()->RequestSneakAttack(eTargetPlayer);
				}
			}
		}
	}
	// We already have an attack on the way
	else
	{
		// Our Approach with this player calls for war
		if(bWantToAttack)
		{
			if(!IsAtWar(eTargetPlayer))
			{
				if(GET_TEAM(GetTeam()).canDeclareWar(GET_PLAYER(eTargetPlayer).getTeam()))
				{
					// If we're at least 85% of the way to our objective, let loose the dogs of war!
					if(IsMusteringForAttack(eTargetPlayer) || (pOperation != NULL && pOperation->PercentFromMusterPointToTarget() >= 85))	// If we're "mustering" it means we have a Sneak Attack Operation that's in position to attack
					{
						bDeclareWar = true;
						SetMusteringForAttack(eTargetPlayer, false);
					}
				}
			}
		}

		// We were planning an attack, but changed our minds so abort
		else
		{
			if(pOperation != NULL)
			{
				pOperation->Kill(AI_ABORT_DIPLO_OPINION_CHANGE);
				SetWarGoal(eTargetPlayer, NO_WAR_GOAL_TYPE);
				SetMusteringForAttack(eTargetPlayer, false);
			}
		}

		// If our Sneak Attack is read then actually initiate the DoW
		if(bDeclareWar)
		{
			DeclareWar(eTargetPlayer);
		}
	}
}

/// Handles declarations of War for this AI
void CvDiplomacyAI::MakeWar()
{
	CvWeightedVector<int> playerList;
	int iWeight;

	if((int)m_eTargetPlayer >= (int)DIPLO_FIRST_PLAYER)
	{
		DoMakeWarOnPlayer(m_eTargetPlayer);
	}
	else
	{
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eTarget = (PlayerTypes)iPlayerLoop;
			if(IsValidUIDiplomacyTarget(eTarget) && IsPlayerValid(eTarget))
			{
				iWeight = (int)GetWarProjection(eTarget) + 1;

				// Square the distance enum to make it crucial
				iWeight *= (1 + (int)GetPlayer()->GetProximityToPlayer(eTarget));
				iWeight *= (1 + (int)GetPlayer()->GetProximityToPlayer(eTarget));

				if(iPlayerLoop < MAX_MAJOR_CIVS)
				{
					if(GetMajorCivOpinion(eTarget) == MAJOR_CIV_OPINION_UNFORGIVABLE)
					{
						iWeight *= 2;
					}

					iWeight *= 10;  // Make sure majors are looked at before city states
				}

				playerList.push_back(iPlayerLoop, iWeight);
			}
		}

		playerList.SortItems();

		for(int iI = 0; iI < playerList.size(); iI++)
		{
			DoMakeWarOnPlayer((PlayerTypes)playerList.GetElement(iI));
		}
	}
}

/// We've decided to declare war on someone
void CvDiplomacyAI::DeclareWar(PlayerTypes ePlayer)
{
	CvAssertMsg(!GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlly() != GetPlayer()->GetID(), "Major is declaring war on a city-state it is allied with! Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvTeam& kMyTeam = GET_TEAM(GetTeam());
	TeamTypes eTheirTeam = GET_PLAYER(ePlayer).getTeam();

	// Only do it if we are not already at war.
	if(!kMyTeam.isAtWar(eTheirTeam))
	{
		kMyTeam.declareWar(eTheirTeam);

		m_pPlayer->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NOW_AT_WAR);

		// Show scene to human
		if(!CvPreGame::isNetworkMultiplayerGame() && GC.getGame().getActivePlayer() == ePlayer)
		{
			const char* strText = GetDiploStringForMessage(DIPLO_MESSAGE_DOW_ROOT, ePlayer);
			gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_AI_DECLARED_WAR, strText, LEADERHEAD_ANIM_DECLARE_WAR);
		}

		LogWarDeclaration(ePlayer);
	}
}


/// If we're planning war with ePlayer, how are we acting towards him?
WarFaceTypes CvDiplomacyAI::GetWarFaceWithPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarFaceTypes) m_paeWarFace[ePlayer];
}

void CvDiplomacyAI::SetWarFaceWithPlayer(PlayerTypes ePlayer, WarFaceTypes eWarFace)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarFace >= 0, "DIPLOMACY_AI: Invalid WarFaceType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarFace < NUM_WAR_FACES, "DIPLOMACY_AI: Invalid WarFaceType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeWarFace[ePlayer] = eWarFace;
}

/// Are we building up for an attack on ePlayer?
bool CvDiplomacyAI::IsMusteringForAttack(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabMusteringForAttack[ePlayer];
}

/// Sets whether or not we're building up for an attack on ePlayer
void CvDiplomacyAI::SetMusteringForAttack(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabMusteringForAttack[ePlayer] = bValue;
}

/// Player was attacked by another!  Change appropriate Diplomacy stuff
void CvDiplomacyAI::DoSomeoneDeclaredWarOnMe(TeamTypes eTeam)
{
	PlayerTypes eLoopPlayer;

	// Loop through all players on our attacker's Team
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(GET_PLAYER(eLoopPlayer).getTeam() == eTeam)
			{
				// Major Civs
				if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					// This will be updated on turn cycling, but for now we're "shocked and disappointed!"
					SetWarGoal(eLoopPlayer, WAR_GOAL_PEACE);
//					SetMajorCivApproach(eLoopPlayer, MAJOR_CIV_APPROACH_DEFENSIVE_WAR);
				}
				// Minor Civs
				else
				{
//					SetMinorCivApproach(eLoopPlayer, MINOR_CIV_APPROACH_WAR);
				}

				m_pPlayer->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NOW_AT_WAR);
			}
		}
	}
}


/// What is the state of war with this Player
WarStateTypes CvDiplomacyAI::GetWarState(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarStateTypes) m_paeWarState[ePlayer];
}

void CvDiplomacyAI::SetWarState(PlayerTypes ePlayer, WarStateTypes eWarState)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarState >= NO_WAR_STATE_TYPE, "DIPLOMACY_AI: Invalid WarStateType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");	// NO_WAR_STATE_TYPE is a valid possibility (if we're not at war)
	CvAssertMsg(eWarState < NUM_WAR_STATES, "DIPLOMACY_AI: Invalid WarStateType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeWarState[ePlayer] = eWarState;
}

/// Updates what the state of war is with all Players
void CvDiplomacyAI::DoUpdateWarStates()
{
	WarStateTypes eWarState;

	int iMyLocalMilitaryStrength;
	int iEnemyInHisLandsMilitaryStrength;

	int iMyForeignMilitaryStrength;
	int iEnemyInMyLandsMilitaryStrength;

	int iMyPercentForeign;
	int iEnemyPercentInHisLands;

	int iMyPercentLocal;
	int iEnemyPercentInMyLands;

	int iLocalRatio;
	int iForeignRatio;

	int iWarStateValue;

	CvUnit* pLoopUnit;
	CvCity* pLoopCity;
	int iUnitOrCityLoop;
	int iPercentHealthLeft;

	// Reset overall war state
	int iStateAllWars = 0;   // Used to assess overall war state in this function
	SetStateAllWars(STATE_ALL_WARS_NEUTRAL);

	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			// War?
			if(IsAtWar(eLoopPlayer))
			{
				iMyLocalMilitaryStrength = 0;
				iEnemyInHisLandsMilitaryStrength = 0;

				iMyForeignMilitaryStrength = 0;
				iEnemyInMyLandsMilitaryStrength = 0;

				// Loop through our units
				for(pLoopUnit = GetPlayer()->firstUnit(&iUnitOrCityLoop); pLoopUnit != NULL; pLoopUnit = GetPlayer()->nextUnit(&iUnitOrCityLoop))
				{
					// On our home front
					if(pLoopUnit->plot()->IsHomeFrontForPlayer(GetPlayer()->GetID()))
						iMyLocalMilitaryStrength += pLoopUnit->GetPower();

					// Enemy's home front
					if(pLoopUnit->plot()->IsHomeFrontForPlayer(eLoopPlayer))
						iMyForeignMilitaryStrength += pLoopUnit->GetPower();
				}

				// Loop through our Enemy's units
				for(pLoopUnit = GET_PLAYER(eLoopPlayer).firstUnit(&iUnitOrCityLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(eLoopPlayer).nextUnit(&iUnitOrCityLoop))
				{
					// On our home front
					if(pLoopUnit->plot()->IsHomeFrontForPlayer(GetPlayer()->GetID()))
					{
						iEnemyInMyLandsMilitaryStrength += pLoopUnit->GetPower();
					}
					// Enemy's home front
					if(pLoopUnit->plot()->IsHomeFrontForPlayer(eLoopPlayer))
					{
						iEnemyInHisLandsMilitaryStrength += pLoopUnit->GetPower();
					}
				}

				// Loop through our Cities
				for(pLoopCity = GetPlayer()->firstCity(&iUnitOrCityLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iUnitOrCityLoop))
				{
					iPercentHealthLeft = (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()) * 100 / pLoopCity->GetMaxHitPoints();
					iMyLocalMilitaryStrength += (pLoopCity->GetPower() * iPercentHealthLeft / 100 / 100);
				}

				// Loop through our Enemy's Cities
				for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iUnitOrCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iUnitOrCityLoop))
				{
					iPercentHealthLeft = (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage()) * 100 / pLoopCity->GetMaxHitPoints();
					iEnemyInHisLandsMilitaryStrength += (pLoopCity->GetPower() * iPercentHealthLeft / 100 / 100);
				}

				// Percentage of our forces in our locales & each other's locales

				// No Units!
				if(iMyLocalMilitaryStrength + iMyForeignMilitaryStrength == 0)
				{
					iMyPercentLocal = 100;
					iMyPercentForeign = 0;
				}
				else
				{
					iMyPercentLocal = iMyLocalMilitaryStrength * 100 / (iMyLocalMilitaryStrength + iMyForeignMilitaryStrength);
					iMyPercentForeign = 100 - iMyPercentLocal;
				}

				// No Units!
				if(iEnemyInHisLandsMilitaryStrength + iEnemyInMyLandsMilitaryStrength == 0)
				{
					iEnemyPercentInHisLands = 100;
					iEnemyPercentInMyLands = 0;
				}
				else
				{
					iEnemyPercentInHisLands = iEnemyInHisLandsMilitaryStrength * 100 / (iEnemyInHisLandsMilitaryStrength + iEnemyInMyLandsMilitaryStrength);
					iEnemyPercentInMyLands = 100 - iEnemyPercentInHisLands;
				}

				// Ratio of Me VS Him in our two locales
				if(iMyLocalMilitaryStrength == 0)
				{
					iMyLocalMilitaryStrength = 1;
				}
				iLocalRatio = iMyLocalMilitaryStrength * 100 / (iMyLocalMilitaryStrength + iEnemyInMyLandsMilitaryStrength);

				if(iEnemyInHisLandsMilitaryStrength == 0)
				{
					iEnemyInHisLandsMilitaryStrength = 1;
				}
				iForeignRatio = iMyForeignMilitaryStrength * 100 / (iMyForeignMilitaryStrength + iEnemyInHisLandsMilitaryStrength);

				// Calm: Not much happening on either front
				if(iForeignRatio < /*25*/ GC.getWAR_STATE_CALM_THRESHOLD_FOREIGN_FORCES() && iLocalRatio > 100 - /*25*/ GC.getWAR_STATE_CALM_THRESHOLD_FOREIGN_FORCES())
				{
					eWarState = WAR_STATE_CALM;
				}
				// SOMETHING is happening
				else
				{
					iWarStateValue = 0;

					iWarStateValue += iLocalRatio;		// Will be between 0 and 100.  Anything less than 75 is bad news though!  We want a very high percentage in our own lands.
					iWarStateValue += iForeignRatio;	// Will be between 0 and 100.  Will vary wildly though, depending on the status of an offensive.  A number of 50 is very good.

					iWarStateValue /= 2;

					// Some Example WarStateValues:
					// Local		Foreign	WarStateValue
					// 100%		70%			85
					// 100%		40%			70
					// 80% 		30% :		55
					// 100% 		0% :			50
					// 60% 		40% :		50
					// 60% 		10% :		35
					// 40% 		0% :			20

					if(iWarStateValue >= /*75*/ GC.getWAR_STATE_THRESHOLD_NEARLY_WON())
					{
						eWarState = WAR_STATE_NEARLY_WON;
					}
					else if(iWarStateValue >= /*57*/ GC.getWAR_STATE_THRESHOLD_OFFENSIVE())
					{
						eWarState = WAR_STATE_OFFENSIVE;
					}
					else if(iWarStateValue >= /*42*/ GC.getWAR_STATE_THRESHOLD_STALEMATE())
					{
						eWarState = WAR_STATE_STALEMATE;
					}
					else if(iWarStateValue >= /*25*/ GC.getWAR_STATE_THRESHOLD_DEFENSIVE())
					{
						eWarState = WAR_STATE_DEFENSIVE;
					}
					else
					{
						eWarState = WAR_STATE_NEARLY_DEFEATED;
					}
				}

//////////////////////////////////////////////////////////
//				WAR STATE MODIFICATIONS - We crunched the numbers above, but are there any special cases to consider?
//////////////////////////////////////////////////////////

				// If the war is calm but they're an easy target consider us on Offense
				if(eWarState == WAR_STATE_CALM)
				{
					if(GetPlayerTargetValue(eLoopPlayer) >= TARGET_VALUE_FAVORABLE)
					{
						eWarState = WAR_STATE_OFFENSIVE;
					}
				}

				// If the other guy happens to have a guy or two near us but we vastly outnumber him overall, we're not really on the defensive
				if(eWarState <= WAR_STATE_DEFENSIVE)
				{
					if(iMyLocalMilitaryStrength >= iEnemyInMyLandsMilitaryStrength + iEnemyInMyLandsMilitaryStrength)
					{
						eWarState = WAR_STATE_STALEMATE;
					}
				}

				// If this is a major power, determine what the impact of this war is on our global situation
				if (!GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					if(eWarState == WAR_STATE_NEARLY_WON)
					{
						iStateAllWars += 2;
					}
					else if(eWarState == WAR_STATE_OFFENSIVE)
					{
						iStateAllWars += 1;
					}
					else if(eWarState == WAR_STATE_DEFENSIVE)
					{
						iStateAllWars -= 1;

						// If we are defensive in this war and our capital has been damaged, overall state should be defensive
						CvCity *pCapital = m_pPlayer->getCapitalCity();
						if (pCapital)
						{
							if (pCapital->getDamage() > 0)
							{
								SetStateAllWars(STATE_ALL_WARS_LOSING);
							}
						}
					}
					else if(eWarState == WAR_STATE_NEARLY_DEFEATED)
					{
						// If nearly defeated in any war, overall state should be defensive
						SetStateAllWars(STATE_ALL_WARS_LOSING);
					}
				}
			}
			// Not at war
			else
				eWarState = NO_WAR_STATE_TYPE;

			SetWarState(eLoopPlayer, eWarState);
		}
	}

	// Finalize overall assessment
	if(iStateAllWars < 0 || GetStateAllWars() == STATE_ALL_WARS_LOSING)
		SetStateAllWars(STATE_ALL_WARS_LOSING);
	else if(iStateAllWars > 0)
		SetStateAllWars(STATE_ALL_WARS_WINNING);
}

/// Overall assessment of winning or losing all the wars we are in
StateAllWars CvDiplomacyAI::GetStateAllWars() const
{
	return m_eStateAllWars;
}

void CvDiplomacyAI::SetStateAllWars(StateAllWars eState)
{
	CvAssertMsg(eState >= 0, "DIPLOMACY_AI: Invalid StateAllWar.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eState < NUM_STATES_ALL_WARS, "DIPLOMACY_AI: Invalid StateAllWar.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_eStateAllWars = eState;
}


/// What is the Projection of war with this Player
WarProjectionTypes CvDiplomacyAI::GetWarProjection(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarProjectionTypes) m_paeWarProjection[ePlayer];
}

/// Sets what ourProjection of war with this Player is
void CvDiplomacyAI::SetWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarProjection >= 0, "DIPLOMACY_AI: Invalid WarProjectionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarProjection < NUM_WAR_PROJECTION_TYPES, "DIPLOMACY_AI: Invalid WarProjectionType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeWarProjection[ePlayer] = eWarProjection;
}

/// previous value of GetWarProjection
WarProjectionTypes CvDiplomacyAI::GetLastWarProjection(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	return (WarProjectionTypes) m_paeLastWarProjection[ePlayer];

}

void CvDiplomacyAI::SetLastWarProjection(PlayerTypes ePlayer, WarProjectionTypes eWarProjection)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarProjection >= 0, "DIPLOMACY_AI: Invalid WarProjectionType.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarProjection < NUM_WAR_PROJECTION_TYPES, "DIPLOMACY_AI: Invalid WarProjectionType.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	m_paeLastWarProjection[ePlayer] = eWarProjection;

}


/// Updates what the Projection of war is with all Players
void CvDiplomacyAI::DoUpdateWarProjections()
{
	WarProjectionTypes eWarProjection;

	int iWarScore;

	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eWarProjection = NO_WAR_PROJECTION_TYPE;

			// War?
//			if (IsAtWar(eLoopPlayer))
			{
				iWarScore = GetWarScore(eLoopPlayer);

				// Do the final math
				if(iWarScore >= /*100*/ GC.getWAR_PROJECTION_THRESHOLD_VERY_GOOD())
					eWarProjection = WAR_PROJECTION_VERY_GOOD;
				else if(iWarScore >= /*25*/ GC.getWAR_PROJECTION_THRESHOLD_GOOD())
					eWarProjection = WAR_PROJECTION_GOOD;
				else if(iWarScore <= /*-100*/ GC.getWAR_PROJECTION_THRESHOLD_DESTRUCTION())
					eWarProjection = WAR_PROJECTION_DESTRUCTION;
				else if(iWarScore <= /*-25*/ GC.getWAR_PROJECTION_THRESHOLD_DEFEAT())
					eWarProjection = WAR_PROJECTION_DEFEAT;
				else if(iWarScore <= /*0*/ GC.getWAR_PROJECTION_THRESHOLD_STALEMATE())
					eWarProjection = WAR_PROJECTION_STALEMATE;
				else
					eWarProjection = WAR_PROJECTION_UNKNOWN;

				// If they're a bad target then the best we can do is a stalemate
				if(GetPlayerTargetValue(eLoopPlayer) <= TARGET_VALUE_BAD)
				{
					if(eWarProjection >= WAR_PROJECTION_GOOD)
						eWarProjection = WAR_PROJECTION_STALEMATE;
				}
			}

			if (GetWarProjection(eLoopPlayer) != NO_WAR_PROJECTION_TYPE)
			{
				SetLastWarProjection(eLoopPlayer, GetWarProjection(eLoopPlayer));
			}
			else
			{
				// for now, set it to be unknown because we can't set it to NO_WAR_PROJECTION_TYPE
				SetLastWarProjection(eLoopPlayer, WAR_PROJECTION_UNKNOWN);
			}

			SetWarProjection(eLoopPlayer, eWarProjection);
		}
	}
}

/// What is the integer value of how well we think the war with ePlayer is going?
int CvDiplomacyAI::GetWarScore(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iWarScore = 0;

	// Military Strength compared to us
	switch(GetPlayerMilitaryStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_PATHETIC:
		iWarScore += /*100*/ GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_PATHETIC();
		break;
	case STRENGTH_WEAK:
		iWarScore += /*60*/ GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_WEAK();
		break;
	case STRENGTH_POOR:
		iWarScore += /*25*/ GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POOR();
		break;
	case STRENGTH_AVERAGE:
		iWarScore += /*0*/ GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_AVERAGE();
		break;
	case STRENGTH_STRONG:
		iWarScore += /*-25*/ GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_STRONG();
		break;
	case STRENGTH_POWERFUL:
		iWarScore += /*-60*/ GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_POWERFUL();
		break;
	case STRENGTH_IMMENSE:
		iWarScore += /*-100*/ GC.getWAR_PROJECTION_THEIR_MILITARY_STRENGTH_IMMENSE();
		break;
	}

	// Economic Strength compared to us
	switch(GetPlayerEconomicStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_PATHETIC:
		iWarScore += /*50*/ GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_PATHETIC();
		break;
	case STRENGTH_WEAK:
		iWarScore += /*30*/ GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_WEAK();
		break;
	case STRENGTH_POOR:
		iWarScore += /*12*/ GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POOR();
		break;
	case STRENGTH_AVERAGE:
		iWarScore += /*0*/ GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_AVERAGE();
		break;
	case STRENGTH_STRONG:
		iWarScore += /*-12*/ GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_STRONG();
		break;
	case STRENGTH_POWERFUL:
		iWarScore += /*-30*/ GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_POWERFUL();
		break;
	case STRENGTH_IMMENSE:
		iWarScore += /*-50*/ GC.getWAR_PROJECTION_THEIR_ECONOMIC_STRENGTH_IMMENSE();
		break;
	}

	// War Damage inflicted on US
	switch(GetWarDamageLevel(ePlayer))
	{
	case WAR_DAMAGE_LEVEL_NONE:
		iWarScore += /*0*/ GC.getWAR_PROJECTION_WAR_DAMAGE_US_NONE();
		// If they're aggressively expanding, it makes them a better target to go after, If they've hurt us, this no longer applies
		if(IsPlayerRecklessExpander(ePlayer))
		{
			iWarScore += /*25*/ GC.getWAR_PROJECTION_RECKLESS_EXPANDER();
		}
		break;
	case WAR_DAMAGE_LEVEL_MINOR:
		iWarScore += /*-10*/ GC.getWAR_PROJECTION_WAR_DAMAGE_US_MINOR();
		break;
	case WAR_DAMAGE_LEVEL_MAJOR:
		iWarScore += /*-20*/ GC.getWAR_PROJECTION_WAR_DAMAGE_US_MAJOR();
		break;
	case WAR_DAMAGE_LEVEL_SERIOUS:
		iWarScore += /*-30*/ GC.getWAR_PROJECTION_WAR_DAMAGE_US_SERIOUS();
		break;
	case WAR_DAMAGE_LEVEL_CRIPPLED:
		iWarScore += /*-40*/ GC.getWAR_PROJECTION_WAR_DAMAGE_US_CRIPPLED();
		break;
	}

	// War Damage inflicted on THEM (less than what's been inflicted on us for the same amount of damage)
	switch(GetOtherPlayerWarDamageLevel(ePlayer, GetPlayer()->GetID()))
	{
	case WAR_DAMAGE_LEVEL_NONE:
		iWarScore += /*0*/ GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_NONE();
		break;
	case WAR_DAMAGE_LEVEL_MINOR:
		iWarScore += /*5*/ GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_MINOR();
		break;
	case WAR_DAMAGE_LEVEL_MAJOR:
		iWarScore += /*10*/ GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_MAJOR();
		break;
	case WAR_DAMAGE_LEVEL_SERIOUS:
		iWarScore += /*15*/ GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_SERIOUS();
		break;
	case WAR_DAMAGE_LEVEL_CRIPPLED:
		iWarScore += /*20*/ GC.getWAR_PROJECTION_WAR_DAMAGE_THEM_CRIPPLED();
		break;
	}

	// Decrease war score if we've been fighting for a long time - after 60 turns the effect is -20 on the WarScore
	int iTurnsAtWar = GetPlayerNumTurnsAtWar(ePlayer);
	iTurnsAtWar /= 3;
	iWarScore -= min(iTurnsAtWar, /*20*/ GC.getWAR_PROJECTION_WAR_DURATION_SCORE_CAP());

	return iWarScore;
}


/// What is the Goal of war with this Player
WarGoalTypes CvDiplomacyAI::GetWarGoal(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarGoalTypes) m_paeWarGoal[ePlayer];
}

/// Sets what the Goal of war is with this Players
void CvDiplomacyAI::SetWarGoal(PlayerTypes ePlayer, WarGoalTypes eWarGoal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarGoal >= NO_WAR_GOAL_TYPE, "DIPLOMACY_AI: Invalid WarGoalType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");	// NO_WAR_GOAL_TYPE is valid (if we're not at war)
	CvAssertMsg(eWarGoal < NUM_WAR_GOALS, "DIPLOMACY_AI: Invalid WarGoalType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	//if (GetWarGoal(ePlayer) == WAR_GOAL_DEMAND && eWarGoal != WAR_GOAL_DEMAND)
	//{
	//	CvAssert(false);
	//}

	m_paeWarGoal[ePlayer] = eWarGoal;
}

/// Updates what the Goal of war is with all Players
void CvDiplomacyAI::DoUpdateWarGoals()
{
	// Are we going for World conquest?  If so, then we want to fight our wars to the death
	bool bWorldConquest = false;

	if(IsGoingForWorldConquest())
	{
		bWorldConquest = true;
	}

	int iWarGoalValue;
	WarGoalTypes eWarGoal;

	//PlayerTypes eLoopOtherPlayer;
	//int iOtherPlayerLoop;
	bool bHigherUpsWantWar;

	bool bIsMinor;

	WarProjectionTypes eProjection;

	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eWarGoal = NO_WAR_GOAL_TYPE;

			if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				bIsMinor = (GET_PLAYER(eLoopPlayer).isMinorCiv());

				iWarGoalValue = 0;
				bHigherUpsWantWar = false;

				// Higher ups want war with this Minor Civ
				if(bIsMinor)
					bHigherUpsWantWar = GetMinorCivApproach(eLoopPlayer) == MINOR_CIV_APPROACH_CONQUEST;
				// Higher ups want war with this Major Civ
				else
					bHigherUpsWantWar = GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_WAR;

				eProjection = GetWarProjection(eLoopPlayer);

				//////////////////////////////
				// Higher ups want war, figure out what kind we're waging
				if(bHigherUpsWantWar)
				{
					// Minor Civs
					if(bIsMinor)

						// If we're going for the conquest victory, conquest is our goal with minors
						if(bWorldConquest)
							eWarGoal = WAR_GOAL_CONQUEST;
						else
							eWarGoal = WAR_GOAL_DAMAGE;

					// Major Civs - depends on how things are going
					else
					{
						// Default goal is Damage
						eWarGoal = WAR_GOAL_DAMAGE;

						// If we're locked into a coop war, we're out for conquest
						if(IsLockedIntoCoopWar(eLoopPlayer))
							eWarGoal = WAR_GOAL_CONQUEST;

						// If we think the war will go well, we can aim for conquest, which means we will not make peace
						if(eProjection >= WAR_PROJECTION_UNKNOWN)
						{
							// If they're unforgivable we're out to destroy them, no less
							if(GetMajorCivOpinion(eLoopPlayer) == MAJOR_CIV_OPINION_UNFORGIVABLE)
								eWarGoal = WAR_GOAL_CONQUEST;

							// Out for world conquest?
							else if(bWorldConquest)
								eWarGoal = WAR_GOAL_CONQUEST;
						}
					}
				}

				//////////////////////////////
				// Higher ups don't want to be at war, figure out how bad things are
				else
				{
					// If we're about to cause some mayhem then hold off on the peace stuff for a bit - not against Minors though
					if(!bIsMinor && GetWarState(eLoopPlayer) == WAR_STATE_NEARLY_WON && GetStateAllWars() != STATE_ALL_WARS_LOSING)\
					{
						eWarGoal = WAR_GOAL_DAMAGE;
					}
					// War isn't decisively in our favor, so we'll make peace if possible
					else
					{
						eWarGoal = WAR_GOAL_PEACE;
					}
				}
			}
			// Getting ready to attack
			else if(GetWarGoal(eLoopPlayer) == WAR_GOAL_PREPARE)
				eWarGoal = WAR_GOAL_PREPARE;
			// Getting ready to make a forceful demand
			else if(GetWarGoal(eLoopPlayer) == WAR_GOAL_DEMAND)
				eWarGoal = WAR_GOAL_DEMAND;


			// Update the counter for how long we've wanted peace for (used to determine when to ask for peace)
			if(eWarGoal == WAR_GOAL_PEACE)
				ChangeWantPeaceCounter(eLoopPlayer, 1);
			else
				SetWantPeaceCounter(eLoopPlayer, 0);

			// Set the War Goal
			SetWarGoal(eLoopPlayer, eWarGoal);
		}
	}
}

/// How many turns have we been at war with this Player?
int CvDiplomacyAI::GetPlayerNumTurnsAtWar(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPlayerNumTurnsAtWar[ePlayer];
}

/// Sets how many turns we've been at war with this Player
void CvDiplomacyAI::SetPlayerNumTurnsAtWar(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting PlayerNumTurnsAtWar to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paiPlayerNumTurnsAtWar[ePlayer] = iValue;
}

/// Sets how many turns we've been at war with this Player
void CvDiplomacyAI::ChangePlayerNumTurnsAtWar(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		SetPlayerNumTurnsAtWar(ePlayer, GetPlayerNumTurnsAtWar(ePlayer) + iChange);
	}
}

/// How many times have we gone to war?
int CvDiplomacyAI::GetNumWarsFought(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiNumWarsFought[ePlayer];
}

/// How many times have we gone to war?
void CvDiplomacyAI::SetNumWarsFought(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting NumWarsFought to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paiNumWarsFought[ePlayer] = iValue;
}

/// How many times have we gone to war?
void CvDiplomacyAI::ChangeNumWarsFought(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		SetNumWarsFought(ePlayer, GetNumWarsFought(ePlayer) + iChange);
	}
}

/// What is our assessment of this player's overall Military strength?
StrengthTypes CvDiplomacyAI::GetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (StrengthTypes) m_paePlayerMilitaryStrengthComparedToUs[ePlayer];
}

/// Set assessment of this player's overall Military strength
void CvDiplomacyAI::SetPlayerMilitaryStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eMilitaryStrength)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMilitaryStrength >= 0, "DIPLOMACY_AI: Invalid StrengthType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMilitaryStrength < NUM_STRENGTH_VALUES, "DIPLOMACY_AI: Invalid StrengthType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerMilitaryStrengthComparedToUs[ePlayer] = eMilitaryStrength;
}

/// Updates what our assessment is of all players' overall military strength
void CvDiplomacyAI::DoUpdatePlayerMilitaryStrengths()
{
	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerMilitaryStrength(eLoopPlayer);
	}
}

/// Updates what our assessment is of all players' overall military strength
void CvDiplomacyAI::DoUpdateOnePlayerMilitaryStrength(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	StrengthTypes eMilitaryStrength;

	int iBase = /*30*/ GC.getMILITARY_STRENGTH_BASE();
	int iMilitaryStrength = iBase + GetPlayer()->GetMilitaryMight();

	int iOtherPlayerMilitary;
	int iMilitaryRatio;

	if(IsPlayerValid(ePlayer, /*bMyTeamIsValid*/ true))
	{
		// Look at player's Military Strength
		//if (GetPlayer()->GetMilitaryMight() > 0)
		{
			iOtherPlayerMilitary = GET_PLAYER(ePlayer).GetMilitaryMight() + iBase;
			// Example: If another player has double the Military strength of us, the Ratio will be 200
			iMilitaryRatio = iOtherPlayerMilitary* /*100*/ GC.getMILITARY_STRENGTH_RATIO_MULTIPLIER() / iMilitaryStrength;
		}
		//else
		//{
		//	iMilitaryRatio = /*100*/ GC.getMILITARY_STRENGTH_RATIO_MULTIPLIER();
		//}

		//iMilitaryStrength += iMilitaryRatio;

		// Now do the final assessment
		if(iMilitaryRatio >= /*250*/ GC.getMILITARY_STRENGTH_IMMENSE_THRESHOLD())
			eMilitaryStrength = STRENGTH_IMMENSE;
		else if(iMilitaryRatio >= /*165*/ GC.getMILITARY_STRENGTH_POWERFUL_THRESHOLD())
			eMilitaryStrength = STRENGTH_POWERFUL;
		else if(iMilitaryRatio >= /*115*/ GC.getMILITARY_STRENGTH_STRONG_THRESHOLD())
			eMilitaryStrength = STRENGTH_STRONG;
		else if(iMilitaryRatio >= /*85*/ GC.getMILITARY_STRENGTH_AVERAGE_THRESHOLD())
			eMilitaryStrength = STRENGTH_AVERAGE;
		else if(iMilitaryRatio >= /*60*/ GC.getMILITARY_STRENGTH_POOR_THRESHOLD())
			eMilitaryStrength = STRENGTH_POOR;
		else if(iMilitaryRatio >= /*40*/ GC.getMILITARY_STRENGTH_WEAK_THRESHOLD())
			eMilitaryStrength = STRENGTH_WEAK;
		else
			eMilitaryStrength = STRENGTH_PATHETIC;

		// Set the value
		SetPlayerMilitaryStrengthComparedToUs(ePlayer, eMilitaryStrength);
	}
}


/// What is our assessment of this player's overall Economic strength?
StrengthTypes CvDiplomacyAI::GetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (StrengthTypes) m_paePlayerEconomicStrengthComparedToUs[ePlayer];
}

/// Set assessment of this player's overall Economic strength
void CvDiplomacyAI::SetPlayerEconomicStrengthComparedToUs(PlayerTypes ePlayer, StrengthTypes eEconomicStrength)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eEconomicStrength >= 0, "DIPLOMACY_AI: Invalid StrengthType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eEconomicStrength < NUM_STRENGTH_VALUES, "DIPLOMACY_AI: Invalid StrengthType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerEconomicStrengthComparedToUs[ePlayer] = eEconomicStrength;
}

/// Updates what our assessment is of all players' overall Economic strength
void CvDiplomacyAI::DoUpdatePlayerEconomicStrengths()
{
	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerEconomicStrength(eLoopPlayer);
	}
}

void CvDiplomacyAI::DoUpdateOnePlayerEconomicStrength(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iEconomicStrength;
	StrengthTypes eEconomicStrength;

	int iOtherPlayerEconomy;
	int iEconomicRatio;

	if(IsPlayerValid(ePlayer, /*bMyTeamIsValid*/ true))
	{
		iEconomicStrength = 0;

		// Look at player's Economic Strength
		if(GetPlayer()->GetEconomicMight() > 0)
		{
			iOtherPlayerEconomy = GET_PLAYER(ePlayer).GetEconomicMight();
			// Example: If another player has double the Economic strength of us, the Ratio will be 200
			iEconomicRatio = iOtherPlayerEconomy* /*100*/ GC.getECONOMIC_STRENGTH_RATIO_MULTIPLIER() / GetPlayer()->GetEconomicMight();
		}
		else
		{
			iEconomicRatio = /*100*/ GC.getECONOMIC_STRENGTH_RATIO_MULTIPLIER() * 5;
		}

		iEconomicStrength += iEconomicRatio;

		// Now do the final assessment
		if(iEconomicStrength >= /*250*/ GC.getECONOMIC_STRENGTH_IMMENSE_THRESHOLD())
			eEconomicStrength = STRENGTH_IMMENSE;
		else if(iEconomicStrength >= /*153*/ GC.getECONOMIC_STRENGTH_POWERFUL_THRESHOLD())
			eEconomicStrength = STRENGTH_POWERFUL;
		else if(iEconomicStrength >= /*120*/ GC.getECONOMIC_STRENGTH_STRONG_THRESHOLD())
			eEconomicStrength = STRENGTH_STRONG;
		else if(iEconomicStrength >= /*83*/ GC.getECONOMIC_STRENGTH_AVERAGE_THRESHOLD())
			eEconomicStrength = STRENGTH_AVERAGE;
		else if(iEconomicStrength >= /*65*/ GC.getECONOMIC_STRENGTH_POOR_THRESHOLD())
			eEconomicStrength = STRENGTH_POOR;
		else if(iEconomicStrength >= /*40*/ GC.getECONOMIC_STRENGTH_WEAK_THRESHOLD())
			eEconomicStrength = STRENGTH_WEAK;
		else
			eEconomicStrength = STRENGTH_PATHETIC;

		// Set the value
		SetPlayerEconomicStrengthComparedToUs(ePlayer, eEconomicStrength);
	}
}


/// What is our assessment of this player's value as a military target?
TargetValueTypes CvDiplomacyAI::GetPlayerTargetValue(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (TargetValueTypes) m_paePlayerTargetValue[ePlayer];
}

/// Set assessment of this player's value as a military target
void CvDiplomacyAI::SetPlayerTargetValue(PlayerTypes ePlayer, TargetValueTypes eTargetValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetValue >= 0, "DIPLOMACY_AI: Invalid TargetValueType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetValue < NUM_TARGET_VALUES, "DIPLOMACY_AI: Invalid TargetValueType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerTargetValue[ePlayer] = eTargetValue;
}

/// Updates what our assessment is of all players' value as a military target
void CvDiplomacyAI::DoUpdatePlayerTargetValues()
{
	PlayerTypes eLoopPlayer;
	// Loop through all (known) Players
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerTargetValue(eLoopPlayer);
	}
}

/// Updates what our assessment is of all players' value as a military target
void CvDiplomacyAI::DoUpdateOnePlayerTargetValue(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(!IsPlayerValid(ePlayer))
		return;

	int iTargetValue;
	TargetValueTypes eTargetValue;

	int iWarCount;

	CvCity* pLoopCity;
	int iCityLoop;

	int iOtherPlayerMilitaryStrength;
	int iCityStrengthMod;
	int iMilitaryRatio;

	int iMyMilitaryStrength = GetPlayer()->getPower();
	// Prevent divide by 0
	if(iMyMilitaryStrength == 0)
	{
		iMyMilitaryStrength = 1;
	}

	int iCityDamage;
	int iNumCities;

	int iThirdPartyLoop;
	PlayerTypes eThirdPartyPlayer;
	int iThirdPartyValue;
	StrengthTypes eThirdPartyStrength;
	PlayerProximityTypes eThirdPartyProximity;
	iTargetValue = 0;

	iCityDamage = 0;
	iNumCities = 0;

	iOtherPlayerMilitaryStrength = GET_PLAYER(ePlayer).getPower();

	// City Defensive Strength
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
	{
		iCityStrengthMod = pLoopCity->GetPower();

		iCityStrengthMod *= /*33*/ GC.getMILITARY_STRENGTH_CITY_MOD();
		iCityStrengthMod /= 100;

		iOtherPlayerMilitaryStrength += iCityStrengthMod;

		// Keep track of damage because we'll use it later as a global modifier for the player's strength
		iCityDamage += pLoopCity->getDamage();
		iNumCities++;
	}

	// Depending on how damaged a player's Cities are, he can become a much more attractive target
	if(iNumCities > 0)
	{
		iCityDamage /= iNumCities;
		iCityDamage *= 100;
		iCityDamage /= GC.getMAX_CITY_HIT_POINTS();
		// iCityDamage is now a percentage of global City damage
		iCityDamage *= iOtherPlayerMilitaryStrength;
		iCityDamage /= 200;	// divide by 200 instead of 100 so that if all Cities have no health it only HALVES our strength instead of taking it all the way to 0

		iOtherPlayerMilitaryStrength -= iCityDamage;
	}

	iMilitaryRatio = iOtherPlayerMilitaryStrength* /*100*/ GC.getMILITARY_STRENGTH_RATIO_MULTIPLIER() / iMyMilitaryStrength;
	// Example: If another player has double the Military strength of us, the Ratio will be 200

	iTargetValue += iMilitaryRatio;

	// Increase target value if the player is already at war with other players
	iWarCount = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getAtWarCount(true);
	// Reduce by 1 if WE'RE already at war with him
	if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()))
	{
		iWarCount--;
	}

	iTargetValue += (iWarCount* /*30*/ GC.getTARGET_ALREADY_WAR_EACH_PLAYER());

	// Factor in distance
	switch(GetPlayer()->GetProximityToPlayer(ePlayer))
	{
	case PLAYER_PROXIMITY_NEIGHBORS:
		iTargetValue += /*-10*/ GC.getTARGET_NEIGHBORS();
		break;
	case PLAYER_PROXIMITY_CLOSE:
		iTargetValue += /*0*/ GC.getTARGET_CLOSE();
		break;
	case PLAYER_PROXIMITY_FAR:
		iTargetValue += /*30*/ GC.getTARGET_FAR();
		break;
	case PLAYER_PROXIMITY_DISTANT:
		iTargetValue += /*80*/ GC.getTARGET_DISTANT();
		break;
	}

	// Factor in Friends this player has, if it's a minor
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		for(iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
		{
			eThirdPartyPlayer = (PlayerTypes) iThirdPartyLoop;

			if(IsPlayerValid(eThirdPartyPlayer))
			{
				// This works for human players as well (NetPledgeMinorProtection in Civ5NetMessage.cpp)
				if(GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlly() == eThirdPartyPlayer ||
				        GET_PLAYER(ePlayer).GetMinorCivAI()->IsProtectedByMajor(eThirdPartyPlayer))
				{
					iThirdPartyValue = 0;
					eThirdPartyStrength = GetPlayerMilitaryStrengthComparedToUs(eThirdPartyPlayer);
					eThirdPartyProximity	= GetPlayer()->GetProximityToPlayer(eThirdPartyPlayer);

					// How strong is this friend?
					if(eThirdPartyStrength == STRENGTH_PATHETIC)
						iThirdPartyValue += /* 0*/ GC.getTARGET_MINOR_BACKUP_PATHETIC();
					else if(eThirdPartyStrength == STRENGTH_WEAK)
						iThirdPartyValue += /* 0*/ GC.getTARGET_MINOR_BACKUP_WEAK();
					else if(eThirdPartyStrength == STRENGTH_POOR)
						iThirdPartyValue += /* 5*/ GC.getTARGET_MINOR_BACKUP_POOR();
					else if(eThirdPartyStrength == STRENGTH_AVERAGE)
						iThirdPartyValue += /* 15*/ GC.getTARGET_MINOR_BACKUP_AVERAGE();
					else if(eThirdPartyStrength == STRENGTH_STRONG)
						iThirdPartyValue += /* 25*/ GC.getTARGET_MINOR_BACKUP_STRONG();
					else if(eThirdPartyStrength == STRENGTH_POWERFUL)
						iThirdPartyValue += /* 35*/ GC.getTARGET_MINOR_BACKUP_POWERFUL();
					else if(eThirdPartyStrength == STRENGTH_IMMENSE)
						iThirdPartyValue += /* 50*/ GC.getTARGET_MINOR_BACKUP_IMMENSE();

					// How close is this guy to us?
					if(eThirdPartyProximity == PLAYER_PROXIMITY_DISTANT)
						iThirdPartyValue *= /* 100*/ GC.getTARGET_MINOR_BACKUP_DISTANT();
					else if(eThirdPartyProximity == PLAYER_PROXIMITY_FAR)
						iThirdPartyValue *= /* 115*/ GC.getTARGET_MINOR_BACKUP_FAR();
					else if(eThirdPartyProximity == PLAYER_PROXIMITY_CLOSE)
						iThirdPartyValue *= /* 150*/ GC.getTARGET_MINOR_BACKUP_CLOSE();
					else if(eThirdPartyProximity == PLAYER_PROXIMITY_NEIGHBORS)
						iThirdPartyValue *= /* 200*/ GC.getTARGET_MINOR_BACKUP_NEIGHBORS();

					iThirdPartyValue /= 100;

					// Add the strength of this friend in to the overall eval
					iTargetValue += iThirdPartyValue;
				}
			}
		}
	}

	// Now do the final assessment
	if(iTargetValue >= /*200*/ GC.getTARGET_IMPOSSIBLE_THRESHOLD())
		eTargetValue = TARGET_VALUE_IMPOSSIBLE;
	else if(iTargetValue >= /*125*/ GC.getTARGET_BAD_THRESHOLD())
		eTargetValue = TARGET_VALUE_BAD;
	else if(iTargetValue >= /*80*/ GC.getTARGET_AVERAGE_THRESHOLD())
		eTargetValue = TARGET_VALUE_AVERAGE;
	else if(iTargetValue >= /*50*/ GC.getTARGET_FAVORABLE_THRESHOLD())
		eTargetValue = TARGET_VALUE_FAVORABLE;
	else
		eTargetValue = TARGET_VALUE_SOFT;

	// If the player is expanding aggressively, bump things down a level
	if(eTargetValue < TARGET_VALUE_SOFT && IsPlayerRecklessExpander(ePlayer))
		eTargetValue = TargetValueTypes(eTargetValue + 1);

	// If it's a city-state and we've been at war for a LONG time, bump things up
	if(eTargetValue > TARGET_VALUE_IMPOSSIBLE && GetPlayerNumTurnsAtWar(ePlayer) > /*50*/ GC.getTARGET_INCREASE_WAR_TURNS())
		eTargetValue = TargetValueTypes(eTargetValue - 1);

	// If the player is too far from us then we can't consider them Soft
	if(eTargetValue == TARGET_VALUE_SOFT)
	{
		if(GetPlayer()->GetProximityToPlayer(ePlayer) <= PLAYER_PROXIMITY_FAR)
		{
			eTargetValue = TARGET_VALUE_FAVORABLE;
		}
	}

	// Set the value
	SetPlayerTargetValue(ePlayer, eTargetValue);
}



// ************************************
// Threats to this Player
// ************************************

/// How threatening is this player Militarily?
ThreatTypes CvDiplomacyAI::GetMilitaryThreat(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (ThreatTypes) m_paeMilitaryThreat[ePlayer];
}

/// Set how threatening this player is militarily
void CvDiplomacyAI::SetMilitaryThreat(PlayerTypes ePlayer, ThreatTypes eMilitaryThreat)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMilitaryThreat >= 0, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eMilitaryThreat < NUM_THREAT_VALUES, "DIPLOMACY_AI: Invalid v.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeMilitaryThreat[ePlayer] = eMilitaryThreat;
}

/// Updates how threatening each player is Militarily
void CvDiplomacyAI::DoUpdateMilitaryThreats()
{
	int iMilitaryThreat;
	ThreatTypes eMilitaryThreatType;

	int iWarCount;

	int iMyMilitaryStrength = GetPlayer()->getPower();
	// Prevent divide by 0
	if(iMyMilitaryStrength == 0)
	{
		iMyMilitaryStrength = 1;
	}

	// Add in City Defensive Strength
	CvCity* pLoopCity;
	int iCityLoop;
	int iCityStrengthMod;
	for(pLoopCity = GetPlayer()->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iCityLoop))
	{
		iCityStrengthMod = pLoopCity->GetPower();
		iCityStrengthMod *= (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage());
		iCityStrengthMod /= pLoopCity->GetMaxHitPoints();
		iCityStrengthMod /= 100;
		iCityStrengthMod *= /*33*/ GC.getMILITARY_STRENGTH_CITY_MOD();
		iCityStrengthMod /= 100;
		iMyMilitaryStrength += iCityStrengthMod;
	}

	int iOtherPlayerMilitaryStrength;
	int iMilitaryRatio;

	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iMilitaryThreat = 0;

			iOtherPlayerMilitaryStrength = GET_PLAYER(eLoopPlayer).getPower();

			// Example: If another player has double the Military strength of us, the Ratio will be 200
			iMilitaryRatio = iOtherPlayerMilitaryStrength* /*100*/ GC.getMILITARY_STRENGTH_RATIO_MULTIPLIER() / iMyMilitaryStrength;

			iMilitaryThreat += iMilitaryRatio;

			// At war: what is the current status of things?
			if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				// War State
				switch(GetWarState(eLoopPlayer))
				{
				case WAR_STATE_CALM:
					iMilitaryThreat += /*0*/ GC.getMILITARY_THREAT_WAR_STATE_CALM();
					break;
				case WAR_STATE_NEARLY_WON:
					iMilitaryThreat += /*-100*/ GC.getMILITARY_THREAT_WAR_STATE_NEARLY_WON();
					break;
				case WAR_STATE_OFFENSIVE:
					iMilitaryThreat += /*-40*/ GC.getMILITARY_THREAT_WAR_STATE_OFFENSIVE();
					break;
				case WAR_STATE_STALEMATE:
					iMilitaryThreat += /*30*/ GC.getMILITARY_THREAT_WAR_STATE_STALEMATE();
					break;
				case WAR_STATE_DEFENSIVE:
					iMilitaryThreat += /*80*/ GC.getMILITARY_THREAT_WAR_STATE_DEFENSIVE();
					break;
				case WAR_STATE_NEARLY_DEFEATED:
					iMilitaryThreat += /*150*/ GC.getMILITARY_THREAT_WAR_STATE_NEARLY_DEFEATED();
					break;
				}
			}

			// Factor in Friends this player has

			// TBD

			// Factor in distance
			switch(GetPlayer()->GetProximityToPlayer(eLoopPlayer))
			{
			case PLAYER_PROXIMITY_NEIGHBORS:
				iMilitaryThreat += /*100*/ GC.getMILITARY_THREAT_NEIGHBORS();
				break;
			case PLAYER_PROXIMITY_CLOSE:
				iMilitaryThreat += /*40*/ GC.getMILITARY_THREAT_CLOSE();
				break;
			case PLAYER_PROXIMITY_FAR:
				iMilitaryThreat += /*-40*/ GC.getMILITARY_THREAT_FAR();
				break;
			case PLAYER_PROXIMITY_DISTANT:
				iMilitaryThreat += /*-100*/ GC.getMILITARY_THREAT_DISTANT();
				break;
			}

			// Don't factor in # of players attacked or at war with now if we ARE at war with this guy already
			if(!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				// Increase threat based on how many Player's we've already seen this guy attack and conquer
				iMilitaryThreat += (GetOtherPlayerNumMinorsAttacked(eLoopPlayer) * /*20*/ GC.getMILITARY_THREAT_PER_MINOR_ATTACKED());
				iMilitaryThreat += (GetOtherPlayerNumMinorsConquered(eLoopPlayer) * /*10*/ GC.getMILITARY_THREAT_PER_MINOR_CONQUERED());
				iMilitaryThreat += (GetOtherPlayerNumMajorsAttacked(eLoopPlayer) * /*40*/ GC.getMILITARY_THREAT_PER_MAJOR_ATTACKED());
				iMilitaryThreat += (GetOtherPlayerNumMajorsConquered(eLoopPlayer) * /*20*/ GC.getMILITARY_THREAT_PER_MAJOR_CONQUERED());

				// Reduce the Threat (dramatically) if the player is already at war with other players
				iWarCount = GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).getAtWarCount(true);
				if(iWarCount > 0)
				{
					iMilitaryThreat += (/*-30*/ GC.getMILITARY_THREAT_ALREADY_WAR_EACH_PLAYER_MULTIPLIER() * iWarCount * iMilitaryThreat / 100);
				}
			}

			// Now do the final assessment
			if(iMilitaryThreat >= /*300*/ GC.getMILITARY_THREAT_CRITICAL_THRESHOLD())
				eMilitaryThreatType = THREAT_CRITICAL;
			else if(iMilitaryThreat >= /*220*/ GC.getMILITARY_THREAT_SEVERE_THRESHOLD())
				eMilitaryThreatType = THREAT_SEVERE;
			else if(iMilitaryThreat >= /*170*/ GC.getMILITARY_THREAT_MAJOR_THRESHOLD())
				eMilitaryThreatType = THREAT_MAJOR;
			else if(iMilitaryThreat >= /*100*/ GC.getMILITARY_THREAT_MINOR_THRESHOLD())
				eMilitaryThreatType = THREAT_MINOR;
			else
				eMilitaryThreatType = THREAT_NONE;

			// Set the Threat
			SetMilitaryThreat(eLoopPlayer, eMilitaryThreatType);
		}
	}
}


/// What is our guess as to the level of Military Threat one player feels from another?
ThreatTypes CvDiplomacyAI::GetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (ThreatTypes) m_ppaaeOtherPlayerMilitaryThreat[ePlayer][eWithPlayer];
}

void CvDiplomacyAI::SetEstimateOtherPlayerMilitaryThreat(PlayerTypes ePlayer, PlayerTypes eWithPlayer, ThreatTypes eThreatType)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eThreatType >= 0, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eThreatType < NUM_THREAT_VALUES, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerMilitaryThreat[ePlayer][eWithPlayer] = eThreatType;
}

/// Updates what our guess is as to the level of Military Threat one player feels from another
void CvDiplomacyAI::DoUpdateEstimateOtherPlayerMilitaryThreats()
{
	int iMilitaryThreat;
	ThreatTypes eMilitaryThreatType;

	int iWarCount;

	int iPlayerMilitaryStrength;

	CvCity* pLoopCity;
	int iCityLoop;
	int iCityStrengthMod;

	int iThirdPlayerMilitaryStrength;
	int iMilitaryRatio;

	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;

	// Loop through all (known) Majors
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iPlayerMilitaryStrength = GET_PLAYER(eLoopPlayer).GetMilitaryMight();

			// Add in City Defensive Strength
			for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iCityLoop))
			{
				iCityStrengthMod = pLoopCity->GetPower();
				iCityStrengthMod *= (pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage());
				iCityStrengthMod /= pLoopCity->GetMaxHitPoints();
				iCityStrengthMod /= 100;
				iCityStrengthMod *= /*33*/ GC.getMILITARY_STRENGTH_CITY_MOD();
				iCityStrengthMod /= 100;
				iPlayerMilitaryStrength += (MAX(iCityStrengthMod, 0));
			}

			// Prevent divide by 0
			if(iPlayerMilitaryStrength == 0)
				iPlayerMilitaryStrength = 1;

			// Now loop through every player HE knows
			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;

				// Don't compare a player to himself
				if(eLoopPlayer != eLoopOtherPlayer)
				{
					// Do both we and the guy we're looking about know the third guy?
					if(IsPlayerValid(eLoopOtherPlayer) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{
						eMilitaryThreatType = THREAT_NONE;
						iMilitaryThreat = 0;

						iThirdPlayerMilitaryStrength = GET_PLAYER(eLoopOtherPlayer).GetMilitaryMight();

						// Example: If another player has double the Military strength of us, the Ratio will be 200
						iMilitaryRatio = iThirdPlayerMilitaryStrength* /*100*/ GC.getMILITARY_STRENGTH_RATIO_MULTIPLIER() / iPlayerMilitaryStrength;

						iMilitaryThreat += iMilitaryRatio;

						//// At war: what is the current status of things?
						//if (GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
						//{
						//	// War State
						//	switch (GetWarState(eLoopPlayer))
						//	{
						//	case WAR_STATE_CALM:
						//		iMilitaryThreat += /*0*/ GC.getMILITARY_THREAT_WAR_STATE_CALM();
						//		break;
						//	case WAR_STATE_NEARLY_WON:
						//		iMilitaryThreat += /*-100*/ GC.getMILITARY_THREAT_WAR_STATE_NEARLY_WON();
						//		break;
						//	case WAR_STATE_OFFENSIVE:
						//		iMilitaryThreat += /*-40*/ GC.getMILITARY_THREAT_WAR_STATE_OFFENSIVE();
						//		break;
						//	case WAR_STATE_STALEMATE:
						//		iMilitaryThreat += /*30*/ GC.getMILITARY_THREAT_WAR_STATE_STALEMATE();
						//		break;
						//	case WAR_STATE_DEFENSIVE:
						//		iMilitaryThreat += /*80*/ GC.getMILITARY_THREAT_WAR_STATE_DEFENSIVE();
						//		break;
						//	case WAR_STATE_NEARLY_DEFEATED:
						//		iMilitaryThreat += /*150*/ GC.getMILITARY_THREAT_WAR_STATE_NEARLY_DEFEATED();
						//		break;
						//	}
						//}

						// Factor in Friends this player has

						// TBD

						// Factor in distance
						switch(GET_PLAYER(eLoopPlayer).GetProximityToPlayer(eLoopOtherPlayer))
						{
						case PLAYER_PROXIMITY_NEIGHBORS:
							iMilitaryThreat += /*100*/ GC.getMILITARY_THREAT_NEIGHBORS();
							break;
						case PLAYER_PROXIMITY_CLOSE:
							iMilitaryThreat += /*40*/ GC.getMILITARY_THREAT_CLOSE();
							break;
						case PLAYER_PROXIMITY_FAR:
							iMilitaryThreat += /*-40*/ GC.getMILITARY_THREAT_FAR();
							break;
						case PLAYER_PROXIMITY_DISTANT:
							iMilitaryThreat += /*-100*/ GC.getMILITARY_THREAT_DISTANT();
							break;
						}

						// Don't factor in # of players attacked or at war with now if we ARE at war with this guy already
//						if (!GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
						{
							// Increase threat based on how many Player's we've already seen this guy attack and conquer
							iMilitaryThreat += (GetOtherPlayerNumMinorsAttacked(eLoopOtherPlayer) * /*20*/ GC.getMILITARY_THREAT_PER_MINOR_ATTACKED());
							iMilitaryThreat += (GetOtherPlayerNumMinorsConquered(eLoopOtherPlayer) * /*10*/ GC.getMILITARY_THREAT_PER_MINOR_CONQUERED());
							iMilitaryThreat += (GetOtherPlayerNumMajorsAttacked(eLoopOtherPlayer) * /*40*/ GC.getMILITARY_THREAT_PER_MAJOR_ATTACKED());
							iMilitaryThreat += (GetOtherPlayerNumMajorsConquered(eLoopOtherPlayer) * /*20*/ GC.getMILITARY_THREAT_PER_MAJOR_CONQUERED());

							// Reduce the Threat (dramatically) if the player is already at war with other players
							iWarCount = GET_TEAM(GET_PLAYER(eLoopOtherPlayer).getTeam()).getAtWarCount(true);
							if(iWarCount > 0)
								iMilitaryThreat += (/*-30*/ GC.getMILITARY_THREAT_ALREADY_WAR_EACH_PLAYER_MULTIPLIER() * iWarCount * iMilitaryThreat / 100);
						}

						// Now do the final assessment
						if(iMilitaryThreat >= /*300*/ GC.getMILITARY_THREAT_CRITICAL_THRESHOLD())
							eMilitaryThreatType = THREAT_CRITICAL;
						else if(iMilitaryThreat >= /*220*/ GC.getMILITARY_THREAT_SEVERE_THRESHOLD())
							eMilitaryThreatType = THREAT_SEVERE;
						else if(iMilitaryThreat >= /*170*/ GC.getMILITARY_THREAT_MAJOR_THRESHOLD())
							eMilitaryThreatType = THREAT_MAJOR;
						else if(iMilitaryThreat >= /*100*/ GC.getMILITARY_THREAT_MINOR_THRESHOLD())
							eMilitaryThreatType = THREAT_MINOR;

						// Set the Threat
						SetEstimateOtherPlayerMilitaryThreat(eLoopPlayer, eLoopOtherPlayer, eMilitaryThreatType);
					}
				}
			}
		}
	}
}

/// How much of a threat are these guys to run amok and break everything
ThreatTypes CvDiplomacyAI::GetWarmongerThreat(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (ThreatTypes) m_paeWarmongerThreat[ePlayer];
}

/// Sets how much of a threat are these guys to run amok and break everything
void CvDiplomacyAI::SetWarmongerThreat(PlayerTypes ePlayer, ThreatTypes eWarmongerThreat)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarmongerThreat >= 0, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWarmongerThreat < NUM_THREAT_VALUES, "DIPLOMACY_AI: Invalid ThreatType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeWarmongerThreat[ePlayer] = eWarmongerThreat;
}

/// Updates how much of a threat each player is to run amok and break everything
void CvDiplomacyAI::DoUpdateWarmongerThreats()
{
	ThreatTypes eThreatType;

	int iThreatValue;

	int iNumPlayersKilled;
	int iNumPlayersEver = GC.getGame().countCivPlayersEverAlive();
	int iPlayersKilledPercent;
	int iWarmongerMod;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eThreatType = THREAT_NONE;
			iThreatValue = GetOtherPlayerWarmongerScore(eLoopPlayer);

			// Now do the final assessment
			if(iThreatValue >= /*200*/ GC.getWARMONGER_THREAT_CRITICAL_THRESHOLD())
				eThreatType = THREAT_CRITICAL;
			else if(iThreatValue >= /*150*/ GC.getWARMONGER_THREAT_SEVERE_THRESHOLD())
				eThreatType = THREAT_SEVERE;
			else if(iThreatValue >= /*100*/ GC.getWARMONGER_THREAT_MAJOR_THRESHOLD())
				eThreatType = THREAT_MAJOR;
			else if(iThreatValue >= /*50*/ GC.getWARMONGER_THREAT_MINOR_THRESHOLD())
				eThreatType = THREAT_MINOR;

			// Also test % of players killed (in case we're on a map with very few players or something)
			iNumPlayersKilled = GetOtherPlayerNumMinorsConquered(eLoopPlayer) + GetOtherPlayerNumMajorsConquered(eLoopPlayer);
			if(iNumPlayersKilled > 0)
			{
				iPlayersKilledPercent = iNumPlayersKilled * 100 / iNumPlayersEver;

				iWarmongerMod = GetWarmongerHate() - /*5*/ GC.getDEFAULT_FLAVOR_VALUE();	// Calculate difference from default
				iWarmongerMod *= /*10*/ GC.getWARMONGER_THREAT_PERSONALITY_MOD();	// This will range from -50 to 50 (%)
				iPlayersKilledPercent += (iPlayersKilledPercent * iWarmongerMod / 100);

				if(iPlayersKilledPercent >= /*40*/ GC.getWARMONGER_THREAT_CRITICAL_PERCENT_THRESHOLD())
					eThreatType = THREAT_CRITICAL;
				if(iPlayersKilledPercent >= /*25*/ GC.getWARMONGER_THREAT_SEVERE_PERCENT_THRESHOLD())
					eThreatType = THREAT_SEVERE;
			}

			// Set the Threat
			SetWarmongerThreat(eLoopPlayer, eThreatType);

			// decay score
			ChangeOtherPlayerWarmongerAmount(eLoopPlayer, /*-5*/GC.getWARMONGER_THREAT_PER_TURN_DECAY());
		}
	}
}



/////////////////////////////////////////////////////////
// Planning Exchanges
/////////////////////////////////////////////////////////



void CvDiplomacyAI::DoUpdatePlanningExchanges()
{
	//int iRandRoll;
	//int iThreshold;

	int iNumRAsWanted = GetNumResearchAgreementsWanted();

	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			// RESEARCH AGREEMENT
			// Do we already have a RA?
			if(!GET_TEAM(GetPlayer()->getTeam()).IsHasResearchAgreement(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				// If we're Friendly and have the appropriate Tech, there's a chance we want to make a Research Agreement
				if(!IsWantsResearchAgreementWithPlayer(eLoopPlayer))
				{
					//if(GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ true) == MAJOR_CIV_APPROACH_FRIENDLY)
					//{
						if(GET_TEAM(GetPlayer()->getTeam()).IsResearchAgreementTradingAllowedWithTeam(GET_PLAYER(eLoopPlayer).getTeam()) ||	   // We have Tech & embassy to make a RA
						        GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsResearchAgreementTradingAllowedWithTeam(GetPlayer()->getTeam())) // They have Tech & embassy to make RA
						{
							//iRandRoll = GC.getGame().getJonRandNumVA(100, "DIPLOMACY_AI: Random weight for player to make Research Agreement (%d; %d)", (int)GetPlayer()->GetID(), (int)eLoopPlayer);

							//iThreshold = /*8*/ GC.getWANT_RESEARCH_AGREEMENT_RAND();

							//iThreshold -= (iNumRAsWanted * 6);	// Reduce the likelihood of making another another RA if we've already got some on the books.  These things are expensive you know

							//if(iRandRoll < iThreshold)
							//{
								DoAddWantsResearchAgreementWithPlayer(eLoopPlayer);
								iNumRAsWanted++;	// This was calculated above, increment it by one since we know we've added another
							//}
						}
					//}
				}
			}

			// Already planning an RA?
			if(IsWantsResearchAgreementWithPlayer(eLoopPlayer))
			{
				bool bCancel = false;
				if(GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ true) == MAJOR_CIV_APPROACH_HOSTILE)
				{
					bCancel = true;
				}
				else if(!(GET_TEAM(GetPlayer()->getTeam()).IsResearchAgreementTradingAllowedWithTeam(GET_PLAYER(eLoopPlayer).getTeam()) ||	   // We have Tech & embassy to make a RA
					      GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsResearchAgreementTradingAllowedWithTeam(GetPlayer()->getTeam()))) // They have Tech & embassy to make RA
				{
					bCancel = true;
				}
	
				if (bCancel)
				{
					DoCancelWantsResearchAgreementWithPlayer(eLoopPlayer);
				}
			}
		}
	}
}

/// Does this AI want to make a Research Agreement with ePlayer?
bool CvDiplomacyAI::IsWantsResearchAgreementWithPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabWantsResearchAgreementWithPlayer[ePlayer];
}

/// Sets this AI want to want a Research Agreement with ePlayer
void CvDiplomacyAI::SetWantsResearchAgreementWithPlayer(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabWantsResearchAgreementWithPlayer[ePlayer] = bValue;
}

/// How many different players does this AI want a RA with?
int CvDiplomacyAI::GetNumResearchAgreementsWanted() const
{
	int iNum = 0;

	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsWantsResearchAgreementWithPlayer(eLoopPlayer))
			iNum++;
	}

	return iNum;
}

/// AI wants a Research Agreement with ePlayer, so handle everything that means
void CvDiplomacyAI::DoAddWantsResearchAgreementWithPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvAssertMsg(!IsWantsResearchAgreementWithPlayer(ePlayer), "DIPLOMACY_AI: AI trying to save for multiple Research Agreements with a player.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

	SetWantsResearchAgreementWithPlayer(ePlayer, true);

	// Start saving the Gold
	int iGoldAmount = GC.getGame().GetGameDeals()->GetTradeItemGoldCost(TRADE_ITEM_RESEARCH_AGREEMENT, GetPlayer()->GetID(), ePlayer);
	GetPlayer()->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_MAJOR_CIV_TRADE, iGoldAmount, /*Priority*/ 1);

	LogWantRA(ePlayer);
}

/// AI wants to cancel a Research Agreement with ePlayer, so handle everything that means
void CvDiplomacyAI::DoCancelWantsResearchAgreementWithPlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	CvAssertMsg(IsWantsResearchAgreementWithPlayer(ePlayer), "DIPLOMACY_AI: AI trying to cancel saving for a Research Agreements he doesn't have.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.")

	SetWantsResearchAgreementWithPlayer(ePlayer, false);

	GetPlayer()->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_MAJOR_CIV_TRADE);
}

/// Are we able to make a Research Agreement with ePlayer right now?
bool CvDiplomacyAI::IsCanMakeResearchAgreementRightNow(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Either side already have all techs?
	if(GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasResearchedAllTechs() || GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->HasResearchedAllTechs())
	{
		return false;
	}

	// We don't want a RA with this guy
	if(!IsWantsResearchAgreementWithPlayer(ePlayer))
	{
		return false;
	}

	// Already have a RA?
	if(GET_TEAM(GetPlayer()->getTeam()).IsHasResearchAgreement(GET_PLAYER(ePlayer).getTeam()))
	{
		return false;
	}

	// Can we have a research agreement right now?
	if(!(GET_TEAM(GetPlayer()->getTeam()).IsResearchAgreementTradingAllowedWithTeam(GET_PLAYER(ePlayer).getTeam())   || // We have Tech & embassy to make a RA
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsResearchAgreementTradingAllowedWithTeam(GetPlayer()->getTeam())))    // They have Tech & embassy to make RA
	{
		return false;
	}

	int iGoldAmount = GC.getGame().GetGameDeals()->GetTradeItemGoldCost(TRADE_ITEM_RESEARCH_AGREEMENT, GetPlayer()->GetID(), ePlayer);

	// We don't have enough Gold
	if(GetPlayer()->GetTreasury()->GetGold() < iGoldAmount)
	{
		return false;
	}

	// They don't have enough Gold
	if(GET_PLAYER(ePlayer).GetTreasury()->GetGold() < iGoldAmount)
	{
		return false;
	}

	return true;
}


// ************************************
// Issues of Dispute
// ************************************



/// What is our level of Dispute with a player over Land?
DisputeLevelTypes CvDiplomacyAI::GetLandDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerLandDisputeLevel[ePlayer];
}

/// Sets what is our level of Dispute with a player is over Land
void CvDiplomacyAI::SetLandDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerLandDisputeLevel[ePlayer] = eDisputeLevel;
}

/// Updates what is our level of Dispute with a player is over Land
void CvDiplomacyAI::DoUpdateLandDisputeLevels()
{
	PlayerTypes ePlayer;

	DisputeLevelTypes eDisputeLevel;

	int iLandDisputeWeight;
	int iExpansionFlavor;

	AggressivePostureTypes eAggression;
	PlayerProximityTypes eProximity;

	// Loop through all (known) Players
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		// Update last turn's values
		m_paePlayerLastTurnLandDisputeLevel[ePlayer] = GetLandDisputeLevel(ePlayer);

		if(IsPlayerValid(ePlayer))
		{
			eDisputeLevel = DISPUTE_LEVEL_NONE;

			iLandDisputeWeight = 0;

			// Expansion aggression
			eAggression = GetExpansionAggressivePosture(ePlayer);

			if(eAggression == AGGRESSIVE_POSTURE_NONE)
				iLandDisputeWeight += /*0*/ GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_NONE();
			else if(eAggression == AGGRESSIVE_POSTURE_LOW)
				iLandDisputeWeight += /*10*/ GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_LOW();
			else if(eAggression == AGGRESSIVE_POSTURE_MEDIUM)
				iLandDisputeWeight += /*32*/ GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_MEDIUM();
			else if(eAggression == AGGRESSIVE_POSTURE_HIGH)
				iLandDisputeWeight += /*50*/ GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_HIGH();
			else if(eAggression == AGGRESSIVE_POSTURE_INCREDIBLE)
				iLandDisputeWeight += /*60*/ GC.getLAND_DISPUTE_EXP_AGGRESSIVE_POSTURE_INCREDIBLE();

			// Plot Buying aggression
			eAggression = GetPlotBuyingAggressivePosture(ePlayer);

			if(eAggression == AGGRESSIVE_POSTURE_NONE)
				iLandDisputeWeight += /*0*/ GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_NONE();
			else if(eAggression == AGGRESSIVE_POSTURE_LOW)
				iLandDisputeWeight += /*5*/ GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_LOW();
			else if(eAggression == AGGRESSIVE_POSTURE_MEDIUM)
				iLandDisputeWeight += /*12*/ GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_MEDIUM();
			else if(eAggression == AGGRESSIVE_POSTURE_HIGH)
				iLandDisputeWeight += /*20*/ GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_HIGH();
			else if(eAggression == AGGRESSIVE_POSTURE_INCREDIBLE)
				iLandDisputeWeight += /*30*/ GC.getLAND_DISPUTE_PLOT_BUY_AGGRESSIVE_POSTURE_INCREDIBLE();

			// Look at our Proximity to the other Player
			eProximity = GetPlayer()->GetProximityToPlayer(ePlayer);

			if(eProximity == PLAYER_PROXIMITY_DISTANT)
				iLandDisputeWeight += /*0*/ GC.getLAND_DISPUTE_DISTANT();
			else if(eProximity == PLAYER_PROXIMITY_FAR)
				iLandDisputeWeight += /*10*/ GC.getLAND_DISPUTE_FAR();
			else if(eProximity == PLAYER_PROXIMITY_CLOSE)
				iLandDisputeWeight += /*18*/ GC.getLAND_DISPUTE_CLOSE();
			else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
				iLandDisputeWeight += /*30*/ GC.getLAND_DISPUTE_NEIGHBORS();

			// JON: Turned off to counter-balance the lack of the next block functioning
			// Is the player already cramped?
			if(GetPlayer()->IsCramped())
			{
				iLandDisputeWeight += /*0*/ GC.getLAND_DISPUTE_CRAMPED_MULTIPLIER();
			}

			// JON: Is this Strategy deprecated?

			// If we're not actually trying to expand any more, then we shouldn't be as upset about land!
			//EconomicAIStrategyTypes eStrategyNormalExpansion = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION", true);

			//if (eStrategyNormalExpansion != NO_ECONOMICAISTRATEGY)
			//{
			//	if (!GetPlayer()->GetEconomicAI()->IsUsingStrategy(eStrategyNormalExpansion))
			//	{
			//		iLandDisputeWeight += /*0*/ GC.getLAND_DISPUTE_NO_EXPANSION_STRATEGY();
			//	}
			//}

			// If the player has deleted the EXPANSION Flavor we have to account for that
			iExpansionFlavor = /*5*/ GC.getDEFAULT_FLAVOR_VALUE();

			for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
			{
				if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_EXPANSION")
				{
					iExpansionFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
					break;
				}
			}

			// Add weight for Player's natural EXPANSION preference
			iLandDisputeWeight *= iExpansionFlavor;

			// Now See what our new Dispute Level should be
			if(iLandDisputeWeight >= /*400*/ GC.getLAND_DISPUTE_FIERCE_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_FIERCE;
			else if(iLandDisputeWeight >= /*230*/ GC.getLAND_DISPUTE_STRONG_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_STRONG;
			else if(iLandDisputeWeight >= /*100*/ GC.getLAND_DISPUTE_WEAK_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_WEAK;

			// Actually set the Level
			SetLandDisputeLevel(ePlayer, eDisputeLevel);
		}
	}
}

/// Returns an integer that increases as the number and severity of land disputes rises
int CvDiplomacyAI::GetTotalLandDisputeLevel()
{
	int iRtnValue = 0; // slewis added, to fix a compile error. I'm guessing zero is correct.
	PlayerTypes ePlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(ePlayer))
		{
			switch(GetLandDisputeLevel(ePlayer))
			{
			case DISPUTE_LEVEL_FIERCE:
				iRtnValue += GC.getAI_DIPLO_LAND_DISPUTE_WEIGHT_FIERCE();
				break;
			case DISPUTE_LEVEL_STRONG:
				iRtnValue += GC.getAI_DIPLO_LAND_DISPUTE_WEIGHT_STRONG();
				break;
			case DISPUTE_LEVEL_WEAK:
				iRtnValue += GC.getAI_DIPLO_LAND_DISPUTE_WEIGHT_WEAK();
				break;
			}
		}
	}
	return iRtnValue;
}

/// What was our level of Dispute with a player over Land last turn?
DisputeLevelTypes CvDiplomacyAI::GetLastTurnLandDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerLastTurnLandDisputeLevel[ePlayer];
}

/// What is our guess as to the level of Dispute between two players over Land?
DisputeLevelTypes CvDiplomacyAI::GetEstimateOtherPlayerLandDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_ppaaeOtherPlayerLandDisputeLevel[ePlayer][eWithPlayer];
}

/// Sets what our guess is as to the level of Dispute between two players over Land
void CvDiplomacyAI::SetEstimateOtherPlayerLandDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerLandDisputeLevel[ePlayer][eWithPlayer] = eDisputeLevel;
}

/// Is ePlayer expanding recklessly?
bool CvDiplomacyAI::IsPlayerRecklessExpander(PlayerTypes ePlayer)
{
	// If the player is too far away from us, we don't care
	if(GetPlayer()->GetProximityToPlayer(ePlayer) < PLAYER_PROXIMITY_CLOSE)
		return false;

	// If the player has too few cities, don't worry about it
	int iNumCities = GET_PLAYER(ePlayer).getNumCities();
	if(iNumCities < 4)
		return false;

	double fAverageNumCities = 0;
	int iNumPlayers = 0;

	// Find out what the average is (minus the player we're looking at)
	PlayerTypes eLoopPlayer;
	CvPlayer* pPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		pPlayer = &GET_PLAYER(eLoopPlayer);

		// Not alive
		if(!pPlayer->isAlive())
			continue;

		// Not the guy we're looking at
		if(eLoopPlayer == ePlayer)
			continue;

		iNumPlayers++;
		fAverageNumCities += pPlayer->getNumCities();
	}

	// Not sure how this would happen, but we'll be safe anyways since we'll be dividing by this value
	if(iNumPlayers == 0)
	{
		CvAssertMsg(false, "0 players to evaluate when trying to identify if someone is a reckless expander. Not sure how this would happen without the game being over yet.");
		return false;
	}

	fAverageNumCities /= iNumPlayers;

	// Must have way more cities than the average player in the game
	if(iNumCities < fAverageNumCities * 1.5)
		return false;

	// If this guy's military is as big as ours, then it probably means he's just stronger than us
	if(GetPlayerMilitaryStrengthComparedToUs(ePlayer) >= STRENGTH_AVERAGE)
		return false;

	return true;
}
// Return value of expansion broken promise
int CvDiplomacyAI::GetBrokenExpansionPromiseValue(PlayerTypes ePlayer)
{
	return m_paiBrokenExpansionPromiseValue[(int)ePlayer];
}
// Setting the value of the expansion broken promise
void CvDiplomacyAI::SetBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiBrokenExpansionPromiseValue[(int)ePlayer] = iValue;
}
// Changing the value of the expansion broken promise
void CvDiplomacyAI::ChangeBrokenExpansionPromiseValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiBrokenExpansionPromiseValue[(int)ePlayer] += iChange;
		if (m_paiBrokenExpansionPromiseValue[(int)ePlayer] < 0)
		{
			m_paiBrokenExpansionPromiseValue[(int)ePlayer] = 0;
		}
	}
}
// Return value of expansion ignored promise
int CvDiplomacyAI::GetIgnoredExpansionPromiseValue(PlayerTypes ePlayer)
{
	return m_paiIgnoredExpansionPromiseValue[(int)ePlayer];	
}
// Setting the value of the expansion ignored promise
void CvDiplomacyAI::SetIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiIgnoredExpansionPromiseValue[(int)ePlayer] = iValue;
}
// Changing the value of the expansion ignored promise
void CvDiplomacyAI::ChangeIgnoredExpansionPromiseValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiIgnoredExpansionPromiseValue[(int)ePlayer] += iChange;
		if (m_paiIgnoredExpansionPromiseValue[(int)ePlayer] < 0)
		{
			m_paiIgnoredExpansionPromiseValue[(int)ePlayer] = 0;
		}
	}
}

int CvDiplomacyAI::GetBrokenBorderPromiseValue(PlayerTypes ePlayer)
{
	return m_paiBrokenBorderPromiseValue[(int)ePlayer];
}

void CvDiplomacyAI::SetBrokenBorderPromiseValue(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiBrokenBorderPromiseValue[(int)ePlayer] = iValue;

}

void CvDiplomacyAI::ChangeBrokenBorderPromiseValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiBrokenBorderPromiseValue[(int)ePlayer] += iChange;
		if (m_paiBrokenBorderPromiseValue[(int)ePlayer] < 0)
		{
			m_paiBrokenBorderPromiseValue[(int)ePlayer] = 0;
		}
	}
}

int CvDiplomacyAI::GetIgnoredBorderPromiseValue(PlayerTypes ePlayer)
{
	return m_paiIgnoredBorderPromiseValue[(int)ePlayer];	
}

void CvDiplomacyAI::SetIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiIgnoredBorderPromiseValue[(int)ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeIgnoredBorderPromiseValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiIgnoredBorderPromiseValue[(int)ePlayer] += iChange;
		if (m_paiIgnoredBorderPromiseValue[(int)ePlayer] < 0)
		{
			m_paiIgnoredBorderPromiseValue[(int)ePlayer] = 0;
		}
	}
}

int CvDiplomacyAI::GetDeclaredWarOnFriendValue(PlayerTypes ePlayer)
{
	return m_paiDeclaredWarOnFriendValue[(int)ePlayer];
}

void CvDiplomacyAI::ChangeDeclaredWarOnFriendValue(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiDeclaredWarOnFriendValue[(int)ePlayer] += iChange;
		if (m_paiDeclaredWarOnFriendValue[(int)ePlayer] < 0)
		{
			m_paiDeclaredWarOnFriendValue[(int)ePlayer] = 0;
		}
	}
}

int CvDiplomacyAI::GetNumCitiesLiberated(PlayerTypes ePlayer)
{
	return m_paiNumCitiesLiberated[(int)ePlayer];
}

void CvDiplomacyAI::ChangeNumCitiesLiberated(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	if (iChange != 0)
	{
		m_paiNumCitiesLiberated[(int)ePlayer] += iChange;
		CvAssertMsg(m_paiNumCitiesLiberated[(int)ePlayer] >= 0, "Num cities liberated is less than zero");
	}
}


/// Return value of recent trades
int CvDiplomacyAI::GetRecentTradeValue(PlayerTypes ePlayer)
{
	return m_paiTradeValue[(int)ePlayer];
}

/// Set value of recent trades
void CvDiplomacyAI::ChangeRecentTradeValue(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiTradeValue[ePlayer] += iChange;
		short iMaxOpinionValue = GC.getDEAL_VALUE_PER_OPINION_WEIGHT() * -(GC.getOPINION_WEIGHT_TRADE_MAX());

		// Must be between 0 and maximum possible boost to opinion
		if(m_paiTradeValue[ePlayer] < 0)
		{
			m_paiTradeValue[ePlayer] = 0;
		}
		else if(m_paiTradeValue[ePlayer] > iMaxOpinionValue)
		{
			m_paiTradeValue[ePlayer] = iMaxOpinionValue;
		}
	}
}

/// Return value of combat damage inflicted on a common enemy
int CvDiplomacyAI::GetCommonFoeValue(PlayerTypes ePlayer)
{
	return m_paiCommonFoeValue[(int)ePlayer];
}

/// Set value of combat damage inflicted on a common enemy
void CvDiplomacyAI::ChangeCommonFoeValue(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0 && ePlayer < MAX_MAJOR_CIVS)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiCommonFoeValue[ePlayer] += iChange;
		short iMaxOpinionValue = GC.getCOMMON_FOE_VALUE_PER_OPINION_WEIGHT() * -(GC.getOPINION_WEIGHT_COMMON_FOE_MAX());

		// Must be between 0 and maximum possible boost to opinion
		if(m_paiCommonFoeValue[ePlayer] < 0)
		{
			m_paiCommonFoeValue[ePlayer] = 0;
		}
		else if(m_paiCommonFoeValue[ePlayer] > iMaxOpinionValue)
		{
			m_paiCommonFoeValue[ePlayer] = iMaxOpinionValue;
		}
	}
}

/// Return value of aid given
int CvDiplomacyAI::GetRecentAssistValue(PlayerTypes ePlayer)
{
	return m_paiAssistValue[(int)ePlayer];
}

/// Set value of aid given
void CvDiplomacyAI::ChangeRecentAssistValue(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiAssistValue[ePlayer] += iChange;
		short iMaxOpinionValue = GC.getDEAL_VALUE_PER_OPINION_WEIGHT() * GC.getOPINION_WEIGHT_ASSIST_MAX();

		if (m_paiAssistValue[ePlayer] < -iMaxOpinionValue)
		{
			m_paiAssistValue[ePlayer] = -iMaxOpinionValue;
		}
		else if (m_paiAssistValue[ePlayer] > iMaxOpinionValue)
		{
			m_paiAssistValue[ePlayer] = iMaxOpinionValue;
		}
	}
}

bool CvDiplomacyAI::IsGaveAssistanceTo(PlayerTypes /*ePlayer*/)
{
	return false;
}

bool CvDiplomacyAI::IsHasPaidTributeTo(PlayerTypes /*ePlayer*/)
{
	return false;
}

bool CvDiplomacyAI::IsNukedBy(PlayerTypes ePlayer)
{
	return (GetNumTimesNuked(ePlayer) > 0);
}

bool CvDiplomacyAI::IsCapitalCapturedBy(PlayerTypes ePlayer)
{
	if(m_pPlayer->IsHasLostCapital() && m_pPlayer->GetCapitalConqueror() == ePlayer)
	{
		return true;
	}
	return false;
}

bool CvDiplomacyAI::IsAngryAboutProtectedMinorKilled(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	int iNumProtectedMinorsKilled = GetOtherPlayerNumProtectedMinorsKilled(ePlayer);
	if (iNumProtectedMinorsKilled > 0)
	{
		int iTurnsSinceMinorKilled = GetTurnsSincePlayerKilledProtectedMinor(ePlayer);
		if (iTurnsSinceMinorKilled > -1 && iTurnsSinceMinorKilled < /*125*/ GC.getOPINION_WEIGHT_KILLED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN()) //antonjs: consider: scale by game speed
		{
			return true;
		}
	}
	return false;
}

bool CvDiplomacyAI::IsAngryAboutProtectedMinorAttacked(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Anger over killed city-states trumps this, don't stack
	if (IsAngryAboutProtectedMinorKilled(ePlayer))
		return false;
	
	int iNumProtectedMinorsAttacked = GetOtherPlayerNumProtectedMinorsAttacked(ePlayer);
	if(iNumProtectedMinorsAttacked > 0)
	{
		int iTurnsSinceMinorAttack = GetTurnsSincePlayerAttackedProtectedMinor(ePlayer);
		if (iTurnsSinceMinorAttack > -1 && iTurnsSinceMinorAttack < /*75*/ GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN()) //antonjs: consider: scale by game speed
		{
			return true;
		}
	}
	return false;
}

bool CvDiplomacyAI::IsAngryAboutProtectedMinorBullied(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	int iNumProtectedMinorsBullied = GetOtherPlayerNumProtectedMinorsBullied(ePlayer);
	if (iNumProtectedMinorsBullied > 0)
	{
		int iTurnsSinceBully = GetTurnsSincePlayerBulliedProtectedMinor(ePlayer);
		if (iTurnsSinceBully > -1 && iTurnsSinceBully < /*75*/ GC.getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_NUM_TURNS_UNTIL_FORGIVEN()) //antonjs: consider: scale by game speed
		{
			return true;
		}
	}
	return false;
}

// Are we angry about ePlayer choosing to side with one of their protected minors that we attacked/bullied?
bool CvDiplomacyAI::IsAngryAboutSidedWithTheirProtectedMinor(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	// Has ePlayer ever done this?
	if (IsOtherPlayerSidedWithProtectedMinor(ePlayer))
	{
		// How long ago?
		int iTurnsSinceDispute = GetTurnsSinceOtherPlayerSidedWithProtectedMinor(ePlayer);
		if (iTurnsSinceDispute > -1 && iTurnsSinceDispute < /*25*/ GC.getOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR_NUM_TURNS_UNTIL_FORGIVEN()) //antonjs: consider: scale by game speed
		{
			return true;
		}
	}
	return false;
}


/// Updates what is our guess as to the levels of Dispute between other players over Land is
void CvDiplomacyAI::DoUpdateEstimateOtherPlayerLandDisputeLevels()
{
	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;

	PlayerTypes eLoopPlayer;
	int iPlayerLoop;

	// Cache world average # of Cities to compare each player we know against later
	int iWorldAverageNumCities = 0;
	int iNumPlayers = 0;
	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).isAlive())
		{
			iWorldAverageNumCities += GET_PLAYER(eLoopPlayer).getNumCities();
			iNumPlayers++;
		}
	}

	iWorldAverageNumCities *= 100;
	iWorldAverageNumCities /= iNumPlayers;

	int iCityRatio;
	int iExpansionFlavorGuess;

	int iLandDisputeWeight;

	DisputeLevelTypes eDisputeLevel;

	PlayerProximityTypes eProximity;

	// Loop through all (known) Majors
	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			// Before looking at anyone else, try to guess how much this player values Expansion based on how many Cities he has relative the rest of the world

			iCityRatio = GET_PLAYER(eLoopPlayer).getNumCities() * 10000;
			iCityRatio /= iWorldAverageNumCities;	// 100 means 1 City, which is why we multiplied by 10000 on the line above... 10000/100 = 100

			if(iCityRatio >= /*200*/ GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_10())
				iExpansionFlavorGuess = 10;
			else if(iCityRatio >= /*180*/ GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_9())
				iExpansionFlavorGuess = 9;
			else if(iCityRatio >= /*160*/ GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_8())
				iExpansionFlavorGuess = 8;
			else if(iCityRatio >= /*130*/ GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_7())
				iExpansionFlavorGuess = 7;
			else if(iCityRatio >= /*110*/ GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_6())
				iExpansionFlavorGuess = 6;
			else if(iCityRatio >= /*90*/ GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_5())
				iExpansionFlavorGuess = 5;
			else if(iCityRatio >= /*80*/ GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_4())
				iExpansionFlavorGuess = 4;
			else if(iCityRatio >= /*55*/ GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_3())
				iExpansionFlavorGuess = 3;
			else if(iCityRatio >= /*30*/ GC.getLAND_DISPUTE_CITY_RATIO_EXPANSION_GUESS_2())
				iExpansionFlavorGuess = 2;
			else
				iExpansionFlavorGuess = 1;

			LogOtherPlayerExpansionGuess(eLoopPlayer, iExpansionFlavorGuess);

			// Now loop through every player HE knows
			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;

				// Don't compare a player to himself
				if(eLoopPlayer != eLoopOtherPlayer)
				{
					// Do both we and the guy we're looking about know the third guy?
					if(IsPlayerValid(eLoopOtherPlayer) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{
						eDisputeLevel = DISPUTE_LEVEL_NONE;
						iLandDisputeWeight = 0;

						// Look at our Proximity to the other Player
						eProximity = GET_PLAYER(eLoopPlayer).GetProximityToPlayer(eLoopOtherPlayer);

						if(eProximity == PLAYER_PROXIMITY_DISTANT)
							iLandDisputeWeight += /*0*/ GC.getLAND_DISPUTE_DISTANT();
						else if(eProximity == PLAYER_PROXIMITY_FAR)
							iLandDisputeWeight += /*5*/ GC.getLAND_DISPUTE_FAR();
						else if(eProximity == PLAYER_PROXIMITY_CLOSE)
							iLandDisputeWeight += /*20*/ GC.getLAND_DISPUTE_CLOSE();
						else if(eProximity == PLAYER_PROXIMITY_NEIGHBORS)
							iLandDisputeWeight += /*40*/ GC.getLAND_DISPUTE_NEIGHBORS();

						// Is the player already cramped?  If so, multiply our current Weight by 1.5x
						if(GET_PLAYER(eLoopPlayer).IsCramped())
						{
							iLandDisputeWeight *= /*0*/ GC.getLAND_DISPUTE_CRAMPED_MULTIPLIER();
							iLandDisputeWeight /= 100;
						}

						// Add weight for Player's estimated EXPANSION preference
						iLandDisputeWeight *= iExpansionFlavorGuess;

						// Max Value (Cramped, Neighbors) - 60
						// EXPANSION 10	: 600
						// EXPANSION 5		: 300
						// EXPANSION 1		: 60

						// "Typical" Value (Not Cramped, Close) - 20
						// EXPANSION 10	: 200
						// EXPANSION 5		: 100
						// EXPANSION 1		: 20

						// Now See what our new Dispute Level should be
						if(iLandDisputeWeight >= /*300*/ GC.getLAND_DISPUTE_FIERCE_THRESHOLD())
							eDisputeLevel = DISPUTE_LEVEL_FIERCE;
						else if(iLandDisputeWeight >= /*200*/ GC.getLAND_DISPUTE_STRONG_THRESHOLD())
							eDisputeLevel = DISPUTE_LEVEL_STRONG;
						else if(iLandDisputeWeight >= /*100*/ GC.getLAND_DISPUTE_WEAK_THRESHOLD())
							eDisputeLevel = DISPUTE_LEVEL_WEAK;

						// Actually set the Level
						SetEstimateOtherPlayerLandDisputeLevel(eLoopPlayer, eLoopOtherPlayer, eDisputeLevel);
					}
				}
			}
		}
	}
}


/// How aggressively has this player settled in relation to us?
AggressivePostureTypes CvDiplomacyAI::GetExpansionAggressivePosture(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paeExpansionAggressivePosture[ePlayer];
}

/// Set how aggressively this player has settled in relation to us
void CvDiplomacyAI::SetExpansionAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture >= 0, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture < NUM_AGGRESSIVE_POSTURE_TYPES, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeExpansionAggressivePosture[ePlayer] = ePosture;
}

/// Updates how aggressively this player's Units are positioned in relation to us
void CvDiplomacyAI::DoUpdateExpansionAggressivePostures()
{
	if(GetPlayer()->getCapitalCity() == NULL)
		return;

	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerExpansionAggressivePosture(eLoopPlayer);
	}
}

/// Updates how aggressively this player's Units are positioned in relation to us
void CvDiplomacyAI::DoUpdateOnePlayerExpansionAggressivePosture(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(GetPlayer()->getCapitalCity() == NULL)
		return;

	if(!IsPlayerValid(ePlayer))
		return;

	int iMyCapitalX = GetPlayer()->getCapitalCity()->getX();
	int iMyCapitalY = GetPlayer()->getCapitalCity()->getY();

	AggressivePostureTypes eAggressivePosture;

	CvCity* pLoopCity;
	int iCityLoop;

	AggressivePostureTypes eMostAggressiveCityPosture;
	int iNumMostAggressiveCities;

	int iDistanceUsToThem, iDistanceThemToTheirCapital, iDistanceCapitals;
	int iTheirCapitalX, iTheirCapitalY;
	int iCityX, iCityY;

	// If they have no capital then, uh... just stop I guess
	if(GET_PLAYER(ePlayer).getCapitalCity() == NULL)
		return;

	iTheirCapitalX = GET_PLAYER(ePlayer).getCapitalCity()->getX();
	iTheirCapitalY = GET_PLAYER(ePlayer).getCapitalCity()->getY();

	iDistanceCapitals = plotDistance(iTheirCapitalX, iTheirCapitalY, iMyCapitalX, iMyCapitalY);

	eMostAggressiveCityPosture = AGGRESSIVE_POSTURE_NONE;
	iNumMostAggressiveCities = 0;

	// Loop through all of this player's Cities
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
	{
		// Don't look at their capital
		if(pLoopCity->isCapital())
			continue;

		// Don't look at Cities they've captured
		if(pLoopCity->getOriginalOwner() != pLoopCity->getOwner())
			continue;

		eAggressivePosture = AGGRESSIVE_POSTURE_NONE;

		iCityX = pLoopCity->getX();
		iCityY = pLoopCity->getY();

		// First calculate distances
		iDistanceUsToThem = plotDistance(iMyCapitalX, iMyCapitalY, iCityX, iCityY);
		iDistanceThemToTheirCapital = plotDistance(iTheirCapitalX, iTheirCapitalY, iCityX, iCityY);

		if(iDistanceUsToThem <= /*3*/ GC.getEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_HIGH())
			eAggressivePosture = AGGRESSIVE_POSTURE_HIGH;
		else if(iDistanceUsToThem <= /*5*/ GC.getEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_MEDIUM())
			eAggressivePosture = AGGRESSIVE_POSTURE_MEDIUM;
		else if(iDistanceUsToThem <= /*9*/ GC.getEXPANSION_CAPITAL_DISTANCE_AGGRESSIVE_POSTURE_LOW())
			eAggressivePosture = AGGRESSIVE_POSTURE_LOW;

		// If this City is closer to our capital than the other player's then it's immediately at least Mediumly aggressive
		if(eAggressivePosture == AGGRESSIVE_POSTURE_LOW)
		{
			if(iDistanceUsToThem < iDistanceThemToTheirCapital)
				eAggressivePosture = AGGRESSIVE_POSTURE_MEDIUM;
		}

		// If this City is further from their capital then our capitals are then it's super-aggressive
		if(eAggressivePosture >= AGGRESSIVE_POSTURE_MEDIUM)
		{
			if(iDistanceCapitals < iDistanceThemToTheirCapital)
				eAggressivePosture = AGGRESSIVE_POSTURE_INCREDIBLE;
		}

		// Increase number of Cities at this Aggressiveness level
		if(eAggressivePosture == eMostAggressiveCityPosture)
			iNumMostAggressiveCities++;

		// If this City is the most aggressive one yet, replace the old record
		else if(eAggressivePosture > eMostAggressiveCityPosture)
		{
			eMostAggressiveCityPosture = eAggressivePosture;
			iNumMostAggressiveCities = 0;
		}

		// If we're already at the max aggression level we don't need to look at more Cities
		if(eMostAggressiveCityPosture == AGGRESSIVE_POSTURE_INCREDIBLE)
			break;
	}

	// If we have multiple Cities that tie for being the highest then bump us up a level
	if(iNumMostAggressiveCities > 1)
	{
		// If every City is low then we don't care that much, and if we're already at the highest level we can't go higher
		if(eMostAggressiveCityPosture > AGGRESSIVE_POSTURE_LOW && eMostAggressiveCityPosture < AGGRESSIVE_POSTURE_INCREDIBLE)
		{
			eMostAggressiveCityPosture = AggressivePostureTypes(eMostAggressiveCityPosture + 1);
		}
	}

	SetExpansionAggressivePosture(ePlayer, eMostAggressiveCityPosture);
}


/// How aggressively is ePlayer buying land near us?
AggressivePostureTypes CvDiplomacyAI::GetPlotBuyingAggressivePosture(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paePlotBuyingAggressivePosture[ePlayer];
}

/// Set how aggressively this player has bought land near us
void CvDiplomacyAI::SetPlotBuyingAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture >= 0, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture < NUM_AGGRESSIVE_POSTURE_TYPES, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paePlotBuyingAggressivePosture[ePlayer] = ePosture;
}

/// Updates how aggressively ePlayer is buying land near us
void CvDiplomacyAI::DoUpdatePlotBuyingAggressivePosture()
{
	PlayerTypes eLoopPlayer;

	AggressivePostureTypes ePosture;

	int iCityLoop;
	CvCity* pLoopCity;

	int iAggressionScore;

	// Loop through all (known) Players
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iAggressionScore = 0;

			// Loop through all of our Cities to see if this player has bought land near them
			for(pLoopCity = GetPlayer()->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iCityLoop))
			{
				iAggressionScore += pLoopCity->AI_GetNumPlotsAcquiredByOtherPlayer(eLoopPlayer);
			}

			// Now See what our new Dispute Level should be
			if(iAggressionScore >= /*10*/ GC.getPLOT_BUYING_POSTURE_INCREDIBLE_THRESHOLD())
				ePosture = AGGRESSIVE_POSTURE_INCREDIBLE;
			else if(iAggressionScore >= /*7*/ GC.getPLOT_BUYING_POSTURE_HIGH_THRESHOLD())
				ePosture = AGGRESSIVE_POSTURE_HIGH;
			else if(iAggressionScore >= /*4*/ GC.getPLOT_BUYING_POSTURE_MEDIUM_THRESHOLD())
				ePosture = AGGRESSIVE_POSTURE_MEDIUM;
			else if(iAggressionScore >= /*2*/ GC.getPLOT_BUYING_POSTURE_LOW_THRESHOLD())
				ePosture = AGGRESSIVE_POSTURE_LOW;
			else
				ePosture = AGGRESSIVE_POSTURE_NONE;

			SetPlotBuyingAggressivePosture(eLoopPlayer, ePosture);
		}
	}
}


/// What is our level of Dispute with a player over Victory?
DisputeLevelTypes CvDiplomacyAI::GetVictoryDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerVictoryDisputeLevel[ePlayer];
}

/// Sets what is our level of Dispute with a player is over Victory
void CvDiplomacyAI::SetVictoryDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerVictoryDisputeLevel[ePlayer] = eDisputeLevel;
}

/// Updates what is our level of Dispute with a player is over Victory
void CvDiplomacyAI::DoUpdateVictoryDisputeLevels()
{
	AIGrandStrategyTypes eMyGrandStrategy = GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy();

	PlayerTypes ePlayer;

	DisputeLevelTypes eDisputeLevel;

	int iVictoryDisputeWeight;

	// Loop through all (known) Players
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(ePlayer))
		{
			eDisputeLevel = DISPUTE_LEVEL_NONE;

			// Minors can't really be an issue with Victory!
			if(!GET_PLAYER(ePlayer).isMinorCiv())
			{
				iVictoryDisputeWeight = 0;

				// Does the other player's (estimated) Grand Strategy match our own?
				if(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(ePlayer) == eMyGrandStrategy)
				{
					switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(ePlayer))
					{
					case GUESS_CONFIDENCE_POSITIVE:
						iVictoryDisputeWeight += /*14*/ GC.getVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_POSITIVE();
						break;
					case GUESS_CONFIDENCE_LIKELY:
						iVictoryDisputeWeight += /*10*/ GC.getVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_LIKELY();
						break;
					case GUESS_CONFIDENCE_UNSURE:
						iVictoryDisputeWeight += /*6*/ GC.getVICTORY_DISPUTE_GRAND_STRATEGY_MATCH_UNSURE();
						break;
					}
				}

				// Add weight for Player's competitiveness (0 - 10)
				iVictoryDisputeWeight *= GetVictoryCompetitiveness();

				// Grand Strategy Matches: 10
				// VictoryCompetitiveness 10	: 100
				// VictoryCompetitiveness 5		: 50
				// VictoryCompetitiveness 1		: 10

				// Now See what our new Dispute Level should be
				if(iVictoryDisputeWeight >= /*80*/ GC.getVICTORY_DISPUTE_FIERCE_THRESHOLD())
					eDisputeLevel = DISPUTE_LEVEL_FIERCE;
				else if(iVictoryDisputeWeight >= /*50*/ GC.getVICTORY_DISPUTE_STRONG_THRESHOLD())
					eDisputeLevel = DISPUTE_LEVEL_STRONG;
				else if(iVictoryDisputeWeight >= /*30*/ GC.getVICTORY_DISPUTE_WEAK_THRESHOLD())
					eDisputeLevel = DISPUTE_LEVEL_WEAK;
			}

			// Actually set the Level
			SetVictoryDisputeLevel(ePlayer, eDisputeLevel);
		}
	}
}


/// What is our guess as to the level of Dispute between two players over Victory?
DisputeLevelTypes CvDiplomacyAI::GetEstimateOtherPlayerVictoryDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_ppaaeOtherPlayerVictoryDisputeLevel[ePlayer][eWithPlayer];
}

/// Sets what our guess is as to the level of Dispute between two players over Victory
void CvDiplomacyAI::SetEstimateOtherPlayerVictoryDisputeLevel(PlayerTypes ePlayer, PlayerTypes eWithPlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eWithPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerVictoryDisputeLevel[ePlayer][eWithPlayer] = eDisputeLevel;
}

/// Updates what is our guess as to the levels of Dispute between other players over Victory is
void CvDiplomacyAI::DoUpdateEstimateOtherPlayerVictoryDisputeLevels()
{
	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;

	int iVictoryDisputeWeight;

	DisputeLevelTypes eDisputeLevel;

	// Loop through all (known) Majors
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			// Now loop through every player HE knows
			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;

				// Don't compare a player to himself
				if(eLoopPlayer != eLoopOtherPlayer)
				{
					// Do both we and the guy we're looking about know the third guy?
					if(IsPlayerValid(eLoopOtherPlayer) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{
						eDisputeLevel = DISPUTE_LEVEL_NONE;
						iVictoryDisputeWeight = 0;

						// Do we think their Grand Strategies match?
						if(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopPlayer) == GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopOtherPlayer))
						{
							switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopPlayer))
							{
							case GUESS_CONFIDENCE_POSITIVE:
								iVictoryDisputeWeight += /*7*/ GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_POSITIVE();
								break;
							case GUESS_CONFIDENCE_LIKELY:
								iVictoryDisputeWeight += /*5*/ GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_LIKELY();
								break;
							case GUESS_CONFIDENCE_UNSURE:
								iVictoryDisputeWeight += /*3*/ GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_UNSURE();
								break;
							}
							switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopOtherPlayer))
							{
							case GUESS_CONFIDENCE_POSITIVE:
								iVictoryDisputeWeight += /*7*/ GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_POSITIVE();
								break;
							case GUESS_CONFIDENCE_LIKELY:
								iVictoryDisputeWeight += /*5*/ GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_LIKELY();
								break;
							case GUESS_CONFIDENCE_UNSURE:
								iVictoryDisputeWeight += /*3*/ GC.getVICTORY_DISPUTE_OTHER_PLAYER_GRAND_STRATEGY_MATCH_UNSURE();
								break;
							}

							// Add weight for Player's competitiveness: assume default (5), since we can't actually know how competitive a player is
							iVictoryDisputeWeight *= /*5*/ GC.getDEFAULT_FLAVOR_VALUE();

							// Example Victory Dispute Weights
							// Positive on Both:		70
							// Positive, Unsure:		50
							// Unsure, Unsure:			30

							// Now See what our new Dispute Level should be
							if(iVictoryDisputeWeight >= /*70*/ GC.getVICTORY_DISPUTE_OTHER_PLAYER_FIERCE_THRESHOLD())
								eDisputeLevel = DISPUTE_LEVEL_FIERCE;
							else if(iVictoryDisputeWeight >= /*50*/ GC.getVICTORY_DISPUTE_OTHER_PLAYER_STRONG_THRESHOLD())
								eDisputeLevel = DISPUTE_LEVEL_STRONG;
							else if(iVictoryDisputeWeight >= /*30*/ GC.getVICTORY_DISPUTE_OTHER_PLAYER_WEAK_THRESHOLD())
								eDisputeLevel = DISPUTE_LEVEL_WEAK;
						}

						// Actually set the Level
						SetEstimateOtherPlayerVictoryDisputeLevel(eLoopPlayer, eLoopOtherPlayer, eDisputeLevel);
					}
				}
			}
		}
	}
}


/// What is our level of Dispute with a player over Wonder?
DisputeLevelTypes CvDiplomacyAI::GetWonderDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerWonderDisputeLevel[ePlayer];
}

/// Sets what is our level of Dispute with a player is over Wonder
void CvDiplomacyAI::SetWonderDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerWonderDisputeLevel[ePlayer] = eDisputeLevel;
}

/// Updates what is our level of Dispute with a player is over Wonder
void CvDiplomacyAI::DoUpdateWonderDisputeLevels()
{
	PlayerTypes ePlayer;

	DisputeLevelTypes eDisputeLevel;

	int iWonderDisputeWeight;

	// Loop through all (known) Players
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(ePlayer))
		{
			eDisputeLevel = DISPUTE_LEVEL_NONE;

			iWonderDisputeWeight = GetNumWondersBeatenTo(ePlayer);

			// Add weight for Player's competitiveness (0 - 10)
			iWonderDisputeWeight *= GetWonderCompetitiveness();

			// Now See what our new Dispute Level should be
			if(iWonderDisputeWeight >= /*10*/ GC.getWONDER_DISPUTE_FIERCE_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_FIERCE;
			else if(iWonderDisputeWeight >= /*7*/ GC.getWONDER_DISPUTE_STRONG_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_STRONG;
			else if(iWonderDisputeWeight >= /*5*/ GC.getWONDER_DISPUTE_WEAK_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_WEAK;

			// Actually set the Level
			SetWonderDisputeLevel(ePlayer, eDisputeLevel);
		}
	}
}


/// What is our level of Dispute with a player over Minor Civ Friendship?
DisputeLevelTypes CvDiplomacyAI::GetMinorCivDisputeLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (DisputeLevelTypes) m_paePlayerMinorCivDisputeLevel[ePlayer];
}

/// Sets what is our level of Dispute with a player is over Minor Civ Friendship
void CvDiplomacyAI::SetMinorCivDisputeLevel(PlayerTypes ePlayer, DisputeLevelTypes eDisputeLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel >= 0, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDisputeLevel < NUM_DISPUTE_LEVELS, "DIPLOMACY_AI: Invalid DisputeLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerMinorCivDisputeLevel[ePlayer] = eDisputeLevel;
}

/// Updates what is our level of Dispute with a player is over Minor Civ Friendship
void CvDiplomacyAI::DoUpdateMinorCivDisputeLevels()
{
	PlayerTypes ePlayer;

	DisputeLevelTypes eDisputeLevel;

	int iMinorCivDisputeWeight;

	// Personality factors in quite a bit here, which is why we square the value
	int iPersonalityMod = GetMinorCivCompetitiveness() * GetMinorCivCompetitiveness();	// Ranges from 0 to 100

	int iMinorCivLoop;
	PlayerTypes eMinor;

	// Loop through all (known) Players
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		ePlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(ePlayer))
		{
			eDisputeLevel = DISPUTE_LEVEL_NONE;
			iMinorCivDisputeWeight = 0;

			// Loop through all minors to check our relationship with them
			for(iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
			{
				eMinor = (PlayerTypes) iMinorCivLoop;

				// We have a PtP with this minor
				if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
				{
					// Player is Allies with this minor
					if(GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
						iMinorCivDisputeWeight += iPersonalityMod* /*10*/ GC.getMINOR_CIV_DISPUTE_ALLIES_WEIGHT();
					// Player is Friends with this minor
					else if(GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
						iMinorCivDisputeWeight += iPersonalityMod* /*5*/ GC.getMINOR_CIV_DISPUTE_FRIENDS_WEIGHT();
				}
			}

			// Now See what our new Dispute Level should be
			if(iMinorCivDisputeWeight >= /*700*/ GC.getMINOR_CIV_DISPUTE_FIERCE_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_FIERCE;
			else if(iMinorCivDisputeWeight >= /*400*/ GC.getMINOR_CIV_DISPUTE_STRONG_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_STRONG;
			else if(iMinorCivDisputeWeight >= /*200*/ GC.getMINOR_CIV_DISPUTE_WEAK_THRESHOLD())
				eDisputeLevel = DISPUTE_LEVEL_WEAK;

			// Actually set the Level
			SetMinorCivDisputeLevel(ePlayer, eDisputeLevel);
		}
	}
}

/// How many wonders has ePlayer beaten us to?
int CvDiplomacyAI::GetNumWondersBeatenTo(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiNumWondersBeatenTo[ePlayer];
}

/// How many wonders has ePlayer beaten us to?
void CvDiplomacyAI::SetNumWondersBeatenTo(PlayerTypes ePlayer, int iNewValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iNewValue >= 0, "DIPLOMACY_AI: Setting NumWondersBeatenTo to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiNumWondersBeatenTo[ePlayer] = iNewValue;
}

/// How many wonders has ePlayer beaten us to?
void CvDiplomacyAI::ChangeNumWondersBeatenTo(PlayerTypes ePlayer, int iChange)
{
	SetNumWondersBeatenTo(ePlayer, GetNumWondersBeatenTo(ePlayer) + iChange);
}

/// How much damage have we taken in a war against a particular Player?
WarDamageLevelTypes CvDiplomacyAI::GetWarDamageLevel(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarDamageLevelTypes) m_paeWarDamageLevel[ePlayer];
}

void CvDiplomacyAI::SetWarDamageLevel(PlayerTypes ePlayer, WarDamageLevelTypes eDamageLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDamageLevel >= 0, "DIPLOMACY_AI: Invalid WarDamageLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDamageLevel < NUM_WAR_DAMAGE_LEVEL_TYPES, "DIPLOMACY_AI: Invalid WarDamageLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paeWarDamageLevel[ePlayer] = eDamageLevel;
}

/// Updates how much damage have we taken in a war against all Players
void CvDiplomacyAI::DoUpdateWarDamageLevel()
{
	PlayerTypes eLoopPlayer;

	WarDamageLevelTypes eWarDamageLevel;

	int iValueLost;
	int iCurrentValue;
	int iValueLostRatio;

	CvCity* pLoopCity;
	CvUnit* pLoopUnit;
	int iValueLoop;

	// Calculate the value of what we have currently
	// This is invariant so we will just do it once
	iCurrentValue = 0;

	int iTypicalPower = m_pPlayer->GetMilitaryAI()->GetPowerOfStrongestBuildableUnit(DOMAIN_LAND);
	// City value
	for(pLoopCity = GetPlayer()->firstCity(&iValueLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iValueLoop))
	{
		iCurrentValue += (pLoopCity->getPopulation() * /*150*/ GC.getWAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER());
		if (pLoopCity->IsOriginalCapital()) // anybody's
		{
			iCurrentValue *= 3;
			iCurrentValue /= 2;
		}
	}

	// Unit value
	for(pLoopUnit = GetPlayer()->firstUnit(&iValueLoop); pLoopUnit != NULL; pLoopUnit = GetPlayer()->nextUnit(&iValueLoop))
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
		if(pkUnitInfo)
		{
			int iUnitValue = pkUnitInfo->GetPower();
			if(iTypicalPower > 0)
			{
				iUnitValue = iUnitValue* /*100*/ GC.getDEFAULT_WAR_VALUE_FOR_UNIT() / iTypicalPower;
			}
			else
			{
				iUnitValue = /*100*/ GC.getDEFAULT_WAR_VALUE_FOR_UNIT();
			}
			iCurrentValue += iUnitValue;
		}
	}

	// Loop through all (known) Players
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			eWarDamageLevel = WAR_DAMAGE_LEVEL_NONE;

			iValueLost = GetWarValueLost(eLoopPlayer);

			if(iValueLost > 0)
			{
				// Total original value is the current value plus the amount lost, so compute the percentage on that
				if(iCurrentValue > 0)
					iValueLostRatio = iValueLost * 100 / (iCurrentValue + iValueLost);
				else
					iValueLostRatio = iValueLost;

				if(iValueLostRatio >= /*67*/ GC.getWAR_DAMAGE_LEVEL_THRESHOLD_CRIPPLED())
					eWarDamageLevel = WAR_DAMAGE_LEVEL_CRIPPLED;
				else if(iValueLostRatio >= /*50*/ GC.getWAR_DAMAGE_LEVEL_THRESHOLD_SERIOUS())
					eWarDamageLevel = WAR_DAMAGE_LEVEL_SERIOUS;
				else if(iValueLostRatio >= /*25*/ GC.getWAR_DAMAGE_LEVEL_THRESHOLD_MAJOR())
					eWarDamageLevel = WAR_DAMAGE_LEVEL_MAJOR;
				else if(iValueLostRatio >= /*10*/ GC.getWAR_DAMAGE_LEVEL_THRESHOLD_MINOR())
					eWarDamageLevel = WAR_DAMAGE_LEVEL_MINOR;
			}

			SetWarDamageLevel(eLoopPlayer, eWarDamageLevel);
		}
	}
}

/// Every turn we're at peace war damage goes down a bit
void CvDiplomacyAI::DoWarDamageDecay()
{
	if((int)m_eTargetPlayer >= (int)DIPLO_FIRST_PLAYER)
		return;

	int iValue;

	TeamTypes eLoopThirdTeam;
	PlayerTypes eLoopThirdPlayer;
	int iThirdPlayerLoop;

	// Loop through all (known) Players
	TeamTypes eLoopTeam;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

		if(IsPlayerValid(eLoopPlayer, /*bMyTeamIsValid*/ true))
		{
			// Update war damage we've suffered
			if(!IsAtWar(eLoopPlayer))
			{
				iValue = GetWarValueLost(eLoopPlayer);

				if(iValue > 0)
				{
					// Go down by 1/20th every turn at peace
					iValue /= 20;

					// Make sure it's changing by at least 1
					iValue = max(1, iValue);

					ChangeWarValueLost(eLoopPlayer, -iValue);
				}
			}

			// Update war damage other players have suffered from our viewpoint
			for(iThirdPlayerLoop = 0; iThirdPlayerLoop < MAX_CIV_PLAYERS; iThirdPlayerLoop++)
			{
				eLoopThirdPlayer = (PlayerTypes) iThirdPlayerLoop;
				eLoopThirdTeam = GET_PLAYER(eLoopThirdPlayer).getTeam();

				// These two players not at war?
				if(!GET_TEAM(eLoopThirdTeam).isAtWar(eLoopTeam))
				{
					iValue = GetOtherPlayerWarValueLost(eLoopPlayer, eLoopThirdPlayer);

					if(iValue > 0)
					{
						// Go down by 1/20th every turn at peace
						iValue /= 20;

						// Make sure it's changing by at least 1
						iValue = max(1, iValue);

						ChangeOtherPlayerWarValueLost(eLoopPlayer, eLoopThirdPlayer, -iValue);
					}
				}
			}
		}
	}
}

// What is the value of stuff (Units & Cities) lost in a war against a particular Player?
int CvDiplomacyAI::GetWarValueLost(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiWarValueLost[ePlayer];
}

// Sets the value of stuff (Units & Cities) lost in a war against a particular Player
void CvDiplomacyAI::SetWarValueLost(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting WarValueLost to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiWarValueLost[ePlayer] = iValue;

	// Reset War Damage
	if(iValue == 0)
		m_paeWarDamageLevel[ePlayer] = WAR_DAMAGE_LEVEL_NONE;
}

// Changes the value of stuff (Units & Cities) lost in a war against a particular Player
void CvDiplomacyAI::ChangeWarValueLost(PlayerTypes ePlayer, int iChange)
{
	SetWarValueLost(ePlayer, GetWarValueLost(ePlayer) + iChange);

	if(iChange > 0)
	{
		// Loop through all the other major civs and see if any of them are fighting us.  If so they are happy this player damaged us.
		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(eLoopPlayer != ePlayer && eLoopPlayer != m_pPlayer->GetID() && IsPlayerValid(eLoopPlayer))
			{
				// Are they at war with me too?
				CvPlayer& kOtherPlayer = GET_PLAYER(eLoopPlayer);
				if(IsAtWar(eLoopPlayer))
				{
					kOtherPlayer.GetDiplomacyAI()->ChangeCommonFoeValue(ePlayer, iChange);
				}
			}
		}
	}
}


/// See how much damage we think players we know have suffered in war against other players
WarDamageLevelTypes CvDiplomacyAI::GetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (WarDamageLevelTypes) m_ppaaeOtherPlayerWarDamageLevel[ePlayer][eLostToPlayer];
}

/// Set how much damage we think players we know have suffered in war against other players
void CvDiplomacyAI::SetOtherPlayerWarDamageLevel(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, WarDamageLevelTypes eDamageLevel)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDamageLevel >= 0, "DIPLOMACY_AI: Invalid WarDamageLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDamageLevel < NUM_WAR_DAMAGE_LEVEL_TYPES, "DIPLOMACY_AI: Invalid WarDamageLevelType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaeOtherPlayerWarDamageLevel[ePlayer][eLostToPlayer] = eDamageLevel;
}

/// Updates what is our guess as to amount of war damage a player has suffered to another player
void CvDiplomacyAI::DoUpdateOtherPlayerWarDamageLevel()
{
	PlayerTypes eLoopOtherPlayer;
	int iOtherPlayerLoop;

	int iValueLost;
	int iCurrentValue;
	int iValueLostRatio;

	CvCity* pLoopCity;
	int iValueLoop;

	WarDamageLevelTypes eWarDamageLevel;

	// Loop through all (known) Majors
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			// Now loop through every player HE knows
			for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
			{
				eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;

				// Don't compare a player to himself
				if(eLoopPlayer != eLoopOtherPlayer)
				{
					// Do both we and the guy we're looking about know the third guy?
					if(IsPlayerValid(eLoopOtherPlayer) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
					{
						// At War?
						if(GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isAtWar(GET_PLAYER(eLoopOtherPlayer).getTeam()))
						{
							iValueLost = GetOtherPlayerWarValueLost(eLoopPlayer, eLoopOtherPlayer);

							// Calculate the value of what we have currently
							iCurrentValue = 0;

							// City value
							if(GET_PLAYER(eLoopPlayer).getNumCities() > 0)
							{
								for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iValueLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iValueLoop))
								{
									iCurrentValue += (pLoopCity->getPopulation() * /*150*/ GC.getWAR_DAMAGE_LEVEL_INVOLVED_CITY_POP_MULTIPLIER());
									if (pLoopCity->IsOriginalCapital()) // anybody's
									{
										iCurrentValue *= 3;
										iCurrentValue /= 2;
									}
								}
							}

							// Prevents divide by 0
							iCurrentValue = max(1,iCurrentValue);

							iValueLostRatio = iValueLost * 100 / iCurrentValue;

							if(iValueLostRatio >= /*50*/ GC.getWAR_DAMAGE_LEVEL_THRESHOLD_CRIPPLED())
								eWarDamageLevel = WAR_DAMAGE_LEVEL_CRIPPLED;
							else if(iValueLostRatio >= /*35*/ GC.getWAR_DAMAGE_LEVEL_THRESHOLD_SERIOUS())
								eWarDamageLevel = WAR_DAMAGE_LEVEL_SERIOUS;
							else if(iValueLostRatio >= /*20*/ GC.getWAR_DAMAGE_LEVEL_THRESHOLD_MAJOR())
								eWarDamageLevel = WAR_DAMAGE_LEVEL_MAJOR;
							else if(iValueLostRatio >= /*10*/ GC.getWAR_DAMAGE_LEVEL_THRESHOLD_MINOR())
								eWarDamageLevel = WAR_DAMAGE_LEVEL_MINOR;
							else
								eWarDamageLevel = WAR_DAMAGE_LEVEL_NONE;
						}

						else
							eWarDamageLevel = WAR_DAMAGE_LEVEL_NONE;

						SetOtherPlayerWarDamageLevel(eLoopPlayer, eLoopOtherPlayer, eWarDamageLevel);
					}
				}
			}
		}
	}
}

// What is the value of stuff (Units & Cities) we estimate a player has lost in a war against a another Player?
int CvDiplomacyAI::GetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_ppaaiOtherPlayerWarValueLost[ePlayer][eLostToPlayer];
}

// Sets the value of stuff (Units & Cities) we estimate a player has lost in a war against a another Player
void CvDiplomacyAI::SetOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting OtherPlayerWarValueLost to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaiOtherPlayerWarValueLost[ePlayer][eLostToPlayer] = iValue;

	// Reset War Damage
	if(iValue == 0)
		m_ppaaeOtherPlayerWarDamageLevel[ePlayer][eLostToPlayer] = WAR_DAMAGE_LEVEL_NONE;
}

// Changes the value of stuff (Units & Cities) we estimate a player has lost in a war against a another Player
void CvDiplomacyAI::ChangeOtherPlayerWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iChange)
{
	SetOtherPlayerWarValueLost(ePlayer, eLostToPlayer, GetOtherPlayerWarValueLost(ePlayer, eLostToPlayer) + iChange);
}

// Cache the current war value lost in a separate place, so we can track when it changes value
void CvDiplomacyAI::CacheOtherPlayerWarValuesLost()
{
	for(int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
	{
		PlayerTypes ePlayer = (PlayerTypes) iPlayer;
		for(int iLostToPlayer = 0; iLostToPlayer < MAX_CIV_PLAYERS; iLostToPlayer++)
		{
			PlayerTypes eLostToPlayer = (PlayerTypes) iLostToPlayer;
			SetOtherPlayerLastRoundWarValueLost(ePlayer, eLostToPlayer, GetOtherPlayerWarValueLost(ePlayer, eLostToPlayer));
		}
	}
}

// Old estimate of value ePlayer has lost in war with eLostToPlayer (before this turn)
int CvDiplomacyAI::GetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_ppaaiOtherPlayerLastRoundWarValueLost[ePlayer][eLostToPlayer];
}

// Set old estimate of value ePlayer has lost in war with eLostToPlayer (before this turn)
void CvDiplomacyAI::SetOtherPlayerLastRoundWarValueLost(PlayerTypes ePlayer, PlayerTypes eLostToPlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eLostToPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eLostToPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting OtherPlayerLastRoundWarValueLost to a negative value.  Please send Anton your save file and version.");
	m_ppaaiOtherPlayerLastRoundWarValueLost[ePlayer][eLostToPlayer] = iValue;
}

/// How aggressively are this player's Units positioned in relation to us?
AggressivePostureTypes CvDiplomacyAI::GetMilitaryAggressivePosture(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paeMilitaryAggressivePosture[ePlayer];
}

/// Set how aggressively this player has stationed his military Units in relation to us
void CvDiplomacyAI::SetMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture >= 0, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture < NUM_AGGRESSIVE_POSTURE_TYPES, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeMilitaryAggressivePosture[ePlayer] = ePosture;
}


/// How aggressively are this player's Units positioned in relation to us? (previous turn)
AggressivePostureTypes CvDiplomacyAI::GetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paeLastTurnMilitaryAggressivePosture[ePlayer];
}

/// Set how aggressively this player has stationed his military Units in relation to us (previous turn)
void CvDiplomacyAI::SetLastTurnMilitaryAggressivePosture(PlayerTypes ePlayer, AggressivePostureTypes ePosture)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture >= 0, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePosture < NUM_AGGRESSIVE_POSTURE_TYPES, "DIPLOMACY_AI: Invalid AggressivePostureType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	m_paeLastTurnMilitaryAggressivePosture[ePlayer] = ePosture;
}

/// Updates how aggressively this player's Units are positioned in relation to us
void CvDiplomacyAI::DoUpdateMilitaryAggressivePostures()
{
	// Loop through all (known) Players
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		DoUpdateOnePlayerMilitaryAggressivePosture(eLoopPlayer);
	}
}

/// Updates how aggressively this player's Units are positioned in relation to us
void CvDiplomacyAI::DoUpdateOnePlayerMilitaryAggressivePosture(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(!IsPlayerValid(ePlayer))
		return;

	AggressivePostureTypes eAggressivePosture;
	AggressivePostureTypes eLastTurnAggressivePosture;

	int iUnitValueOnMyHomeFront;
	int iValueToAdd;
	bool bIsAtWarWithSomeone;

	CvUnit* pLoopUnit;
	int iUnitLoop;

	int iOtherPlayerLoop;
	PlayerTypes eLoopOtherPlayer;

	// Keep a record of last turn
	eLastTurnAggressivePosture = GetMilitaryAggressivePosture(ePlayer);
	if(eLastTurnAggressivePosture != NO_AGGRESSIVE_POSTURE_TYPE)
		SetLastTurnMilitaryAggressivePosture(ePlayer, eLastTurnAggressivePosture);

	iUnitValueOnMyHomeFront = 0;
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());
	bIsAtWarWithSomeone = (kTeam.getAtWarCount(false) > 0);

	TeamTypes eOurTeam = GetTeam();
	PlayerTypes eOurPlayerID = GetPlayer()->GetID();

	// Loop through the other guy's units
	for(pLoopUnit = kPlayer.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iUnitLoop))
	{
		// Don't be scared of noncombat Units!
		if(pLoopUnit->IsCombatUnit())
		{
			CvPlot* pUnitPlot = pLoopUnit->plot();
			// Can we actually see this Unit's Plot?  No cheating!
			if(pUnitPlot->isVisible(eOurTeam))
			{
				// On our home front
				if(pUnitPlot->IsHomeFrontForPlayer(eOurPlayerID))
				{
					// At war with someone?  Because if this is Unit in the vicinity of another player he's already at war with, don't count this Unit as aggressive
					if(bIsAtWarWithSomeone)
					{
						// Loop through all players...
						for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_CIV_PLAYERS; iOtherPlayerLoop++)
						{
							eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;

							// Don't look at us or see if this player is at war with himself
							if(eLoopOtherPlayer != ePlayer && eLoopOtherPlayer != eOurPlayerID)
							{
								// At war with this player?
								if(kTeam.isAtWar(GET_PLAYER(eLoopOtherPlayer).getTeam()))
								{
									if(GET_PLAYER(eLoopOtherPlayer).isAlive())
									{
										if(pUnitPlot->IsHomeFrontForPlayer(eLoopOtherPlayer))
										{
											continue;
										}
									}
								}
							}
						}
					}

					iValueToAdd = 10;

					// If the Unit is in the other player's territory, halve it's "aggression value," since he may just be defending himself
					if(pLoopUnit->plot()->isOwned())
					{
						if(pLoopUnit->plot()->getOwner() == ePlayer)
							iValueToAdd /= 2;
					}

					// Maybe look at Unit Power here instead?
					iUnitValueOnMyHomeFront += iValueToAdd;
				}
			}
		}
	}

	// So how threatening is he being?
	if(iUnitValueOnMyHomeFront >= /*80*/ GC.getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_INCREDIBLE())
		eAggressivePosture = AGGRESSIVE_POSTURE_INCREDIBLE;
	else if(iUnitValueOnMyHomeFront >= /*50*/ GC.getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_HIGH())
		eAggressivePosture = AGGRESSIVE_POSTURE_HIGH;
	else if(iUnitValueOnMyHomeFront >= /*30*/ GC.getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_MEDIUM())
		eAggressivePosture = AGGRESSIVE_POSTURE_MEDIUM;
	else if(iUnitValueOnMyHomeFront >= /*10*/ GC.getMILITARY_AGGRESSIVE_POSTURE_THRESHOLD_LOW())
		eAggressivePosture = AGGRESSIVE_POSTURE_LOW;
	else
		eAggressivePosture = AGGRESSIVE_POSTURE_NONE;

	SetMilitaryAggressivePosture(ePlayer, eAggressivePosture);
}



// ************************************
// Personality Members
// ************************************



/// How much is this player annoyed when he thinks another player is going for the same victory type
int CvDiplomacyAI::GetVictoryCompetitiveness() const
{
	return m_iVictoryCompetitiveness;
}

/// How annoyed does this player get when beaten to a wonder?
int CvDiplomacyAI::GetWonderCompetitiveness() const
{
	return m_iWonderCompetitiveness;
}

/// How annoyed does this player get when another player is befriending "their" minor civs?
int CvDiplomacyAI::GetMinorCivCompetitiveness() const
{
	return m_iMinorCivCompetitiveness;
}

/// How likely is this player to go for World Conquest?
int CvDiplomacyAI::GetBoldness() const
{
	return m_iBoldness;
}

/// How much does a leader want to maintain a balance of power in the world?
int CvDiplomacyAI::GetDiploBalance() const
{
	return m_iDiploBalance;
}

/// How much does this AI get angry when someone's being a warmonger?
int CvDiplomacyAI::GetWarmongerHate() const
{
	return m_iWarmongerHate;
}

/// How much is this AI willing to work with someone AGAINST another player?
int CvDiplomacyAI::GetDenounceWillingness() const
{
	return m_iDenounceWillingness;
}

/// How much is this AI willing to work WITH someone?
int CvDiplomacyAI::GetDoFWillingness() const
{
	return m_iDoFWillingness;
}

/// How willing is this AI to backstab its friends?
int CvDiplomacyAI::GetLoyalty() const
{
	return m_iLoyalty;
}

/// How much does the AI want the support of its friends in rough times?
int CvDiplomacyAI::GetNeediness() const
{
	return m_iNeediness;
}

/// How willing is the AI to overlook transgressions between friends?
int CvDiplomacyAI::GetForgiveness() const
{
	return m_iForgiveness;
}

/// How much does the AI like to pop up and talk
int CvDiplomacyAI::GetChattiness() const
{
	return m_iChattiness;
}

/// How much does the AI like to talk smack
int CvDiplomacyAI::GetMeanness() const
{
	return m_iMeanness;
}

/// What is our bias towards a particular Major Civ Approach?
int CvDiplomacyAI::GetPersonalityMajorCivApproachBias(MajorCivApproachTypes eApproach) const
{
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Trying to queury invalid Major Approach for personality bias.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MAJOR_CIV_APPROACHES, "DIPLOMACY_AI: Trying to queury invalid Major Approach for personality bias.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPersonalityMajorCivApproachBiases[eApproach];
}

/// What is our bias towards a particular Minor Civ Approach?
int CvDiplomacyAI::GetPersonalityMinorCivApproachBias(MinorCivApproachTypes eApproach) const
{
	CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Trying to queury invalid Minor Approach for personality bias.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eApproach < NUM_MINOR_CIV_APPROACHES, "DIPLOMACY_AI: Trying to queury invalid Minor Approach for personality bias.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPersonalityMinorCivApproachBiases[eApproach];
}



// ************************************
// Evaluation of Other Players' Tendencies
// ************************************



/// ePlayer made peace with someone, so figure out what that means
void CvDiplomacyAI::DoWeMadePeaceWithSomeone(TeamTypes eOtherTeam)
{
	CvAssertMsg(eOtherTeam >= 0, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherTeam < MAX_CIV_TEAMS, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	PlayerTypes eThirdPlayer;
	int iThirdPlayerLoop;

	PlayerTypes ePeacePlayer;
	for(int iPeacePlayerLoop = 0; iPeacePlayerLoop < MAX_CIV_PLAYERS; iPeacePlayerLoop++)
	{
		ePeacePlayer = (PlayerTypes) iPeacePlayerLoop;

		if(GET_PLAYER(ePeacePlayer).getTeam() == eOtherTeam)
		{
			// In case we had an ongoing operation, kill it
			SetMusteringForAttack(ePeacePlayer, false);

			if(!GET_PLAYER(ePeacePlayer).isMinorCiv())
			{
				// If we made peace, reset coop war and working against status
				for(iThirdPlayerLoop = 0; iThirdPlayerLoop < MAX_MAJOR_CIVS; iThirdPlayerLoop++)
				{
					eThirdPlayer = (PlayerTypes) iThirdPlayerLoop;

					SetCoopWarAcceptedState(eThirdPlayer, ePeacePlayer, NO_COOP_WAR_STATE);
					//SetWorkingAgainstPlayerAccepted(eThirdPlayer, ePeacePlayer, false);
					//SetWorkingAgainstPlayerCounter(eThirdPlayer, ePeacePlayer, -666);
				}

				// If we made peace with someone, set our Approach with them to Neutral to bias against another war
				if(GetMajorCivApproach(ePeacePlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_WAR)
					SetMajorCivApproach(ePeacePlayer, MAJOR_CIV_APPROACH_NEUTRAL);
			}
			else
			{
				// Reset flags for taunt messages we sent to other civs about attacking this minor
				ResetSentAttackProtectedMinorTaunts(ePeacePlayer);
			}
		}
	}
}

/// ePlayer declared war on someone, so figure out what that means
void CvDiplomacyAI::DoPlayerDeclaredWarOnSomeone(PlayerTypes ePlayer, TeamTypes eOtherTeam)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherTeam >= 0, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherTeam <= MAX_CIV_TEAMS, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Disregard barbarians
	if(GET_TEAM(eOtherTeam).isBarbarian())
		return;

	CvAssertMsg(eOtherTeam < MAX_CIV_TEAMS, "DIPLOMACY_AI: Invalid Team Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iThirdPartyLoop;
	PlayerTypes eThirdParty;
	PlayerTypes eThem;

	bool bWasFriends;

	PlayerTypes eAttackedPlayer;
	for(int iAttackedPlayerLoop = 0; iAttackedPlayerLoop < MAX_CIV_PLAYERS; iAttackedPlayerLoop++)
	{
		eAttackedPlayer = (PlayerTypes) iAttackedPlayerLoop;

		if(GET_PLAYER(eAttackedPlayer).getTeam() == eOtherTeam)
		{
			bWasFriends = false;

			// We're involved SOMEHOW
			if(ePlayer == GetPlayer()->GetID() || eAttackedPlayer == GetPlayer()->GetID())
			{
				eThem = GetPlayer()->GetID() != ePlayer ? ePlayer : eAttackedPlayer;

				ChangeNumWarsFought(eThem, 1);

				// Only stuff for major civs
				if(!GET_PLAYER(eThem).isMinorCiv())
				{
					// WAS working with this player
					if(IsDoFAccepted(eThem))
					{
						bWasFriends = true;

						SetDoFAccepted(eThem, false);
						SetDoFCounter(eThem, -666);
					}

					// HAD agreed to not settle nearby
					if(IsPlayerNoSettleRequestAccepted(eThem))
					{
						SetPlayerNoSettleRequestAccepted(eThem, false);
						SetPlayerNoSettleRequestCounter(eThem, -666);
					}

					// HAD agreed not to spy on them
					if(IsPlayerStopSpyingRequestAccepted(eThem))
					{
						SetPlayerStopSpyingRequestAccepted(eThem, false);
						SetPlayerStopSpyingRequestCounter(eThem, -666);
					}

					// Third party arrangements
					for(iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
					{
						eThirdParty = (PlayerTypes) iThirdPartyLoop;

						// WAS working with the guy we're now at war with against someone else
						//if (IsWorkingAgainstPlayerAccepted(eThem, eThirdParty))
						//{
						//	SetWorkingAgainstPlayerAccepted(eThem, eThirdParty, false);
						//	SetWorkingAgainstPlayerCounter(eThem, eThirdParty, -666);
						//}

						// WAS in or planning a coop war with the guy we're now at war with
						if(GetCoopWarAcceptedState(eThem, eThirdParty) >= COOP_WAR_STATE_SOON)
						{
							SetCoopWarAcceptedState(eThem, eThirdParty, NO_COOP_WAR_STATE);
							SetCoopWarCounter(eThem, eThirdParty, -666);
						}
					}
				}

				// If WE were attacked, change our Approach to war so that it's not inappropriate next time the turn cycles
				if(eAttackedPlayer == GetPlayer()->GetID())
				{
					// WAS friends with this player
					if(bWasFriends)
					{
						SetFriendDeclaredWarOnUs(eThem, true);
						ChangeDeclaredWarOnFriendValue(eThem, GC.getOPINION_WEIGHT_WAR_FRIEND_EACH() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
					}

					if(!GET_PLAYER(ePlayer).isMinorCiv())
					{
						SetMajorCivApproach(ePlayer, MAJOR_CIV_APPROACH_WAR);
						SetWarFaceWithPlayer(ePlayer, WAR_FACE_NEUTRAL);
					}

					// Player broke a promise that he wasn't going to attack us
					if(IsPlayerMadeMilitaryPromise(ePlayer))
						SetPlayerBrokenMilitaryPromise(ePlayer, true);

					// Clear civilians returned so they don't affect relations any more
					if(GetNumCiviliansReturnedToMe(ePlayer) > 0)
						ChangeNumCiviliansReturnedToMe(ePlayer, -GetNumCiviliansReturnedToMe(ePlayer));

					// Clear positive diplomatic values
					ChangeRecentTradeValue(ePlayer, -GetRecentTradeValue(ePlayer));
					ChangeCommonFoeValue(ePlayer, -GetCommonFoeValue(ePlayer));
					if (GetRecentAssistValue(ePlayer) < 0)
					{
						ChangeRecentAssistValue(ePlayer, -GetRecentAssistValue(ePlayer));
					}
					SetDoFCounter(ePlayer, -1);
					SetDoFAccepted(ePlayer, false);
				}
			}

			// Did WE attack someone?
			if(ePlayer == GetPlayer()->GetID())
			{
			}
			// If it was someone else see what it means
			else if(IsPlayerValid(eAttackedPlayer))
			{
				if(GET_PLAYER(eAttackedPlayer).isMinorCiv())
				{
					// Did they attack a Minor we're protecting?
					if(GET_PLAYER(eAttackedPlayer).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
					{
						SetOtherPlayerTurnsSinceAttackedProtectedMinor(ePlayer, 0);
						SetOtherPlayerProtectedMinorAttacked(ePlayer, eAttackedPlayer);
						ChangeOtherPlayerNumProtectedMinorsAttacked(ePlayer, 1);
					}
				}
			}
		}
	}
}

/// ePlayer killed eDeadPlayer, so figure out what that means
void CvDiplomacyAI::DoPlayerKilledSomeone(PlayerTypes ePlayer, PlayerTypes eDeadPlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeadPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeadPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(IsPlayerValid(ePlayer))
	{
		// Minor Civ
		if(GET_PLAYER(eDeadPlayer).isMinorCiv())
		{
			ChangeOtherPlayerNumMinorsConquered(ePlayer, 1);

			// Did they kill a Minor we're protecting?
			if(GET_PLAYER(eDeadPlayer).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
			{
				SetOtherPlayerTurnsSinceKilledProtectedMinor(ePlayer, 0);
				SetOtherPlayerProtectedMinorKilled(ePlayer, eDeadPlayer);
				ChangeOtherPlayerNumProtectedMinorsKilled(ePlayer, 1);

				// Player broke a promise that he wasn't going to kill the Minor
				if(IsPlayerMadeAttackCityStatePromise(ePlayer))
					SetPlayerBrokenAttackCityStatePromise(ePlayer, true);
			}
		}
		// Major Civ
		else
			ChangeOtherPlayerNumMajorsConquered(ePlayer, 1);
	}
}

/// ePlayer bullied eOtherPlayer, so figure out what that means
void CvDiplomacyAI::DoPlayerBulliedSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// The bully was someone else
	if(IsPlayerValid(ePlayer) && ePlayer != GetPlayer()->GetID())
	{
		if(GET_PLAYER(eOtherPlayer).isMinorCiv())
		{
			// Did they bully a Minor we're protecting?
			if(GET_PLAYER(eOtherPlayer).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
			{				
				ChangeOtherPlayerNumProtectedMinorsBullied(ePlayer, 1);
			}
		}
	}
}

/// Someone met a new player
void CvDiplomacyAI::DoPlayerMetSomeone(PlayerTypes ePlayer, PlayerTypes eOtherPlayer)
{
	// Have to have met both players (since this function is called upon EVERY contact)
	if(IsPlayerValid(ePlayer) && IsPlayerValid(eOtherPlayer))
	{
		if(!GET_PLAYER(ePlayer).isMinorCiv())
		{
			// Catch up on Public Declarations
			PublicDeclarationTypes eDeclaration;
			int iData1;
			int iData2;
			bool bActive;

			for(int iLoop = 0; iLoop < MAX_DIPLO_LOG_STATEMENTS; iLoop++)
			{
				// Did ePlayer just meet the person this declaration is about?
				if(GetDeclarationLogMustHaveMetPlayerForIndex(iLoop) == eOtherPlayer)
				{
					eDeclaration = GetDeclarationLogTypeForIndex(iLoop);

					// Does this entry in the log exist?
					if(eDeclaration != NO_PUBLIC_DECLARATION_TYPE)
					{
						bActive = IsDeclarationLogForIndexActive(iLoop);

						// Is this still an active Declaration?
						if(bActive)
						{
							iData1 = GetDeclarationLogData1ForIndex(iLoop);
							iData2 = GetDeclarationLogData2ForIndex(iLoop);

							DoMakePublicDeclaration(eDeclaration, iData1, iData2, eOtherPlayer, ePlayer);
						}
					}
				}
			}
		}
	}
}

/// How many turns has it been since ePlayer attacked a Minor we were protecting?
int CvDiplomacyAI::GetTurnsSincePlayerAttackedProtectedMinor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(GetOtherPlayerProtectedMinorAttacked(ePlayer) == NO_PLAYER)
		return -1;

	return m_paiOtherPlayerTurnsSinceAttackedProtectedMinor[ePlayer];
}

/// Sets when the last time ePlayer attacked a Minor we were protecting was
void CvDiplomacyAI::SetOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting OtherPlayerTurnsSinceAttackedProtectedMinor to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerTurnsSinceAttackedProtectedMinor[ePlayer] = iValue;
}

/// Changes when the last time ePlayer Attacked a Minor we were protecting was
void CvDiplomacyAI::ChangeOtherPlayerTurnsSinceAttackedProtectedMinor(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerTurnsSinceAttackedProtectedMinor(ePlayer, GetTurnsSincePlayerAttackedProtectedMinor(ePlayer) + iChange);
}

/// Who was the last Minor ePlayer attacked that we were protecting?
PlayerTypes CvDiplomacyAI::GetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (PlayerTypes) m_paiOtherPlayerProtectedMinorAttacked[ePlayer];
}

/// Sets who the last Minor that ePlayer attacked that we were protecting is
void CvDiplomacyAI::SetOtherPlayerProtectedMinorAttacked(PlayerTypes ePlayer, PlayerTypes eAttackedPlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eAttackedPlayer >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eAttackedPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerProtectedMinorAttacked[ePlayer] = eAttackedPlayer;
}

/// How many ProtectedMinors have we seen this Player attack
int CvDiplomacyAI::GetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumProtectedMinorsAttacked[ePlayer];
}

/// Sets how many ProtectedMinors have we seen this Player attack
void CvDiplomacyAI::SetOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of protected Minors to be negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumProtectedMinorsAttacked[ePlayer] = iValue;
}

/// Changes how many ProtectedMinors have we seen this Player attack
void CvDiplomacyAI::ChangeOtherPlayerNumProtectedMinorsAttacked(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumProtectedMinorsAttacked(ePlayer, GetOtherPlayerNumProtectedMinorsAttacked(ePlayer) + iChange);
}

/// How many turns has it been since ePlayer Killed a Minor we were protecting?
int CvDiplomacyAI::GetTurnsSincePlayerKilledProtectedMinor(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(GetOtherPlayerProtectedMinorKilled(ePlayer) == NO_PLAYER)
		return -1;

	return m_paiOtherPlayerTurnsSinceKilledProtectedMinor[ePlayer];
}

/// Sets when the last time ePlayer Killed a Minor we were protecting was
void CvDiplomacyAI::SetOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of turns since protected Minor was killed to be negative.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerTurnsSinceKilledProtectedMinor[ePlayer] = iValue;
}

/// Changes when the last time ePlayer Killed a Minor we were protecting was
void CvDiplomacyAI::ChangeOtherPlayerTurnsSinceKilledProtectedMinor(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerTurnsSinceKilledProtectedMinor(ePlayer, GetTurnsSincePlayerKilledProtectedMinor(ePlayer) + iChange);
}

/// Who was the last Minor ePlayer Killed that we were protecting?
PlayerTypes CvDiplomacyAI::GetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (PlayerTypes) m_paiOtherPlayerProtectedMinorKilled[ePlayer];
}

/// Sets who the last Minor that ePlayer Killed that we were protecting is
void CvDiplomacyAI::SetOtherPlayerProtectedMinorKilled(PlayerTypes ePlayer, PlayerTypes eKilledPlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eKilledPlayer >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eKilledPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerProtectedMinorKilled[ePlayer] = eKilledPlayer;
}

/// How many ProtectedMinors have we seen this Player attack
int CvDiplomacyAI::GetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumProtectedMinorsKilled[ePlayer];
}

/// Sets how many ProtectedMinors have we seen this Player attack
void CvDiplomacyAI::SetOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumProtectedMinorsKilled[ePlayer] = iValue;
}

/// Changes how many ProtectedMinors have we seen this Player attack
void CvDiplomacyAI::ChangeOtherPlayerNumProtectedMinorsKilled(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumProtectedMinorsKilled(ePlayer, GetOtherPlayerNumProtectedMinorsKilled(ePlayer) + iChange);
}

/// How many turns has it been since this player bullied a minor we were protecting?
int CvDiplomacyAI::GetTurnsSincePlayerBulliedProtectedMinor(PlayerTypes eBullyPlayer)
{
	CvAssertMsg(eBullyPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eBullyPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");

	PlayerTypes eMostRecentBulliedMinor = GetOtherPlayerProtectedMinorBullied(eBullyPlayer);
	if(eMostRecentBulliedMinor == NO_PLAYER)
		return -1;

	int iMostRecentBullyTurn = GET_PLAYER(eMostRecentBulliedMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(eBullyPlayer);
	return (GC.getGame().getGameTurn() - iMostRecentBullyTurn);
}

/// Which of our protected minors has this player most recently bullied?
PlayerTypes CvDiplomacyAI::GetOtherPlayerProtectedMinorBullied(PlayerTypes eBullyPlayer)
{
	CvAssertMsg(eBullyPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eBullyPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");

	int iMostRecentBullyTurn = -1;
	PlayerTypes eMostRecentBulliedMinor = NO_PLAYER;

	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinor = (PlayerTypes) iMinorLoop;
		if(IsPlayerValid(eMinor))
		{
			if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
			{
				if(GET_PLAYER(eMinor).GetMinorCivAI()->IsEverBulliedByMajor(eBullyPlayer))
				{
					int iBullyTurn = GET_PLAYER(eMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(eBullyPlayer);
					if(iBullyTurn > iMostRecentBullyTurn)
					{
						iMostRecentBullyTurn = iBullyTurn;
						eMostRecentBulliedMinor = eMinor;
					}
				}
			}
		}
	}

	return eMostRecentBulliedMinor;
}

/// How many ProtectedMinors have we seen this Player bully
int CvDiplomacyAI::GetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_paiOtherPlayerNumProtectedMinorsBullied[ePlayer];
}

/// Sets how many ProtectedMinors have we seen this Player bully
void CvDiplomacyAI::SetOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of protected Minors to be negative.  Please send Anton your save file and version.");
	m_paiOtherPlayerNumProtectedMinorsBullied[ePlayer] = iValue;
}

/// Changes how many ProtectedMinors have we seen this Player bully
void CvDiplomacyAI::ChangeOtherPlayerNumProtectedMinorsBullied(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumProtectedMinorsBullied(ePlayer, GetOtherPlayerNumProtectedMinorsBullied(ePlayer) + iChange);
}

// We bullied or attacked their protected minor, and they didn't take our side!
bool CvDiplomacyAI::IsOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return false;

	return (GetTurnsSinceOtherPlayerSidedWithProtectedMinor(ePlayer) >= 0); // -1 means it never happened, as defined in Reset()
}

int CvDiplomacyAI::GetTurnsSinceOtherPlayerSidedWithProtectedMinor(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return -1; // -1 means it never happened, as defined in Reset()

	return m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[ePlayer];
}

void CvDiplomacyAI::SetOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	m_paiOtherPlayerTurnsSinceSidedWithTheirProtectedMinor[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeOtherPlayerTurnsSinceSidedWithProtectedMinor(PlayerTypes ePlayer, int iChange)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	if (ePlayer < 0 || ePlayer >= MAX_MAJOR_CIVS) return;

	SetOtherPlayerTurnsSinceSidedWithProtectedMinor(ePlayer, GetTurnsSinceOtherPlayerSidedWithProtectedMinor(ePlayer) + iChange);
}

/// How many Minors have we seen this Player attack
int CvDiplomacyAI::GetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumMinorsAttacked[ePlayer];
}

/// Sets how many Minors have we seen this Player attack
void CvDiplomacyAI::SetOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of minors attacked to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumMinorsAttacked[ePlayer] = iValue;
}

/// Changes how many Minors have we seen this Player attack
void CvDiplomacyAI::ChangeOtherPlayerNumMinorsAttacked(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumMinorsAttacked(ePlayer, GetOtherPlayerNumMinorsAttacked(ePlayer) + iChange);
	ChangeOtherPlayerWarmongerAmount(ePlayer, iChange * /*500 */ GC.getWARMONGER_THREAT_MINOR_ATTACKED_WEIGHT());
}

/// How many Minors have we seen this Player conquer
int CvDiplomacyAI::GetOtherPlayerNumMinorsConquered(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumMinorsConquered[ePlayer];
}

/// Sets how many Minors have we seen this Player conquer
void CvDiplomacyAI::SetOtherPlayerNumMinorsConquered(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of minors conquered to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumMinorsConquered[ePlayer] = iValue;
}

/// Changes how many Minors have we seen this Player conquer
void CvDiplomacyAI::ChangeOtherPlayerNumMinorsConquered(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumMinorsConquered(ePlayer, GetOtherPlayerNumMinorsConquered(ePlayer) + iChange);
	ChangeOtherPlayerWarmongerAmount(ePlayer, iChange * /*1000*/ GC.getWARMONGER_THREAT_MINOR_CONQUERED_WEIGHT());
}

/// How many Majors have we seen this Player attack
int CvDiplomacyAI::GetOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumMajorsAttacked[ePlayer];
}

/// Sets how many Majors have we seen this Player attack
void CvDiplomacyAI::SetOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of Majors attacked to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumMajorsAttacked[ePlayer] = iValue;
}

/// Changes how many Majors have we seen this Player attack
void CvDiplomacyAI::ChangeOtherPlayerNumMajorsAttacked(PlayerTypes ePlayer, int iChange, TeamTypes eAttackedTeam)
{
	if(iChange > 0)
	{
		PlayerTypes eAttackedPlayer;
		for(int iAttackedPlayerLoop = 0; iAttackedPlayerLoop < MAX_MAJOR_CIVS; iAttackedPlayerLoop++)
		{
			eAttackedPlayer = (PlayerTypes) iAttackedPlayerLoop;

			// Player must be on this team
			if(GET_PLAYER(eAttackedPlayer).getTeam() != eAttackedTeam)
				continue;

			// Don't ACTUALLY count this if we're at war with the guy also
			if(IsAtWar(eAttackedPlayer))
				return;

			// Don't ACTUALLY count this if we have a pact of secrecy against him
			//if (IsWorkingAgainstPlayer(eAttackedPlayer))
			//	return;
		}
	}

	SetOtherPlayerNumMajorsAttacked(ePlayer, GetOtherPlayerNumMajorsAttacked(ePlayer) + iChange);
	ChangeOtherPlayerWarmongerAmount(ePlayer, iChange * /*500 */ GC.getWARMONGER_THREAT_MAJOR_ATTACKED_WEIGHT());
}

/// How many Majors have we seen this Player conquer
int CvDiplomacyAI::GetOtherPlayerNumMajorsConquered(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiOtherPlayerNumMajorsConquered[ePlayer];
}

/// Sets how many Majors have we seen this Player conquer
void CvDiplomacyAI::SetOtherPlayerNumMajorsConquered(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iValue >= 0, "DIPLOMACY_AI: Setting number of Majors conquered to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiOtherPlayerNumMajorsConquered[ePlayer] = iValue;
}

/// Changes how many Majors have we seen this Player conquer
void CvDiplomacyAI::ChangeOtherPlayerNumMajorsConquered(PlayerTypes ePlayer, int iChange)
{
	SetOtherPlayerNumMajorsConquered(ePlayer, GetOtherPlayerNumMajorsConquered(ePlayer) + iChange);
	ChangeOtherPlayerWarmongerAmount(ePlayer, iChange * /*1000 */ GC.getWARMONGER_THREAT_MAJOR_CONQUERED_WEIGHT());
}

// Get amount of warmongerishness felt toward this player
int CvDiplomacyAI::GetOtherPlayerWarmongerAmount(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");	
	return m_paiOtherPlayerWarmongerAmount[ePlayer];
}

// Change amount of warmongerishness felt toward this player
void CvDiplomacyAI::ChangeOtherPlayerWarmongerAmount(PlayerTypes ePlayer, int iChangeAmount)
{
	int iNewValue = m_paiOtherPlayerWarmongerAmount[ePlayer] + iChangeAmount;
	iNewValue = max(0, iNewValue);
	m_paiOtherPlayerWarmongerAmount[ePlayer] = iNewValue;
}

// The value of the warmonger amount adjusted by how much this player hates warmongers
int CvDiplomacyAI::GetOtherPlayerWarmongerScore(PlayerTypes ePlayer)
{
	int iReturnValue = GetOtherPlayerWarmongerAmount(ePlayer);
	iReturnValue *= GetWarmongerHate();
	iReturnValue /= 100;
	return iReturnValue;
}

/////////////////////////////////////////////////////////
// Contact
/////////////////////////////////////////////////////////

/// First contact between this player and another
void CvDiplomacyAI::DoFirstContact(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(ePlayer != GetPlayer()->GetID())
	{
		DoFirstContactInitRelationship(ePlayer);

		//// Major Civ
		//if (!GET_PLAYER(ePlayer).isMinorCiv())
		//{
		//	SetPlayerMilitaryStrengthComparedToUs(ePlayer, STRENGTH_AVERAGE);
		//	SetMajorCivApproach(ePlayer, MAJOR_CIV_APPROACH_NEUTRAL);
		//	SetMajorCivOpinion(ePlayer, MAJOR_CIV_OPINION_NEUTRAL);
		//}
		//// Minor Civ
		//else
		//	SetMinorCivApproach(ePlayer, MINOR_CIV_APPROACH_IGNORE);

		// Default States, will be updated on turn cycling
		//SetPlayerTargetValue(ePlayer, TARGET_VALUE_AVERAGE);

		// Humans don't say hi to one another through the shadow diplo AI and, uh, don't show up in MP please
		if(!GC.getGame().isNetworkMultiPlayer())	// KWG: Candidate for !GC.getGame().IsOption(GAMEOPTION_SIMULTANEOUS_TURNS)
		{
			if(!GetPlayer()->isHuman())
			{
				// Should fire off a diplo message when we meet a human
				if(GET_PLAYER(ePlayer).isHuman())
				{
					if(!IsAtWar(ePlayer))
					{
						if(GC.getGame().isFinalInitialized())
						{
							if(std::find(m_aGreetPlayers.begin(), m_aGreetPlayers.end(), ePlayer) == m_aGreetPlayers.end())
							{
								// Put in the list of people to greet when their turn comes up.
								m_aGreetPlayers.push_back(ePlayer);
							}
						}
					}
				}
			}
			else
			{
				// Human to Human will just send a notification
				CvPlayer& kTargetPlayer = GET_PLAYER(ePlayer);
				if(kTargetPlayer.isHuman())
				{
					if(!IsAtWar(ePlayer))
					{
						if(GC.getGame().isFinalInitialized())
						{
							CvNotifications* pNotifications = kTargetPlayer.GetNotifications();
							if(pNotifications)
							{
								CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_MET_MINOR_CIV", GetPlayer()->getNameKey());
								pNotifications->Add(NOTIFICATION_GENERIC, strBuffer, strBuffer, -1, -1, GetPlayer()->GetID());
							}
						}
					}
				}
			}
		}

		// Catch up on public declarations this player has made

		if(!GET_PLAYER(ePlayer).isMinorCiv())
		{
			PublicDeclarationTypes eDeclaration;
			int iData1;
			int iData2;
			PlayerTypes eMustHaveMetPlayer;
			bool bActive;

			for(int iLoop = 0; iLoop < MAX_DIPLO_LOG_STATEMENTS; iLoop++)
			{
				eDeclaration = GetDeclarationLogTypeForIndex(iLoop);

				// Does this entry in the log exist?
				if(eDeclaration != NO_PUBLIC_DECLARATION_TYPE)
				{
					bActive = IsDeclarationLogForIndexActive(iLoop);

					iData1 = GetDeclarationLogData1ForIndex(iLoop);
					iData2 = GetDeclarationLogData2ForIndex(iLoop);

					// Validate active status - minors must be alive
					if(eDeclaration == PUBLIC_DECLARATION_PROTECT_MINOR)
					{
						CvAssert(((PlayerTypes) iData1) != NO_PLAYER);
						if(((PlayerTypes) iData1) != NO_PLAYER)
						{
							if(!GET_PLAYER((PlayerTypes) iData1).isAlive())
								bActive = false;
						}
					}
					else if(eDeclaration == PUBLIC_DECLARATION_ABANDON_MINOR)
					{
						CvAssert(((PlayerTypes) iData1) != NO_PLAYER);
						if(((PlayerTypes) iData1) != NO_PLAYER)
						{
							if(!GET_PLAYER((PlayerTypes) iData1).isAlive())
								bActive = false;
						}
					}

					// Is this still an active Declaration?
					if(bActive)
					{
						eMustHaveMetPlayer = GetDeclarationLogMustHaveMetPlayerForIndex(iLoop);

						DoMakePublicDeclaration(eDeclaration, iData1, iData2, eMustHaveMetPlayer, ePlayer);
					}
				}
			}
		}
	}
}

void CvDiplomacyAI::DoFirstContactInitRelationship(PlayerTypes ePlayer)
{
	DoUpdateOnePlayerMilitaryStrength(ePlayer);
	DoUpdateOnePlayerEconomicStrength(ePlayer);

	DoUpdateOnePlayerMilitaryAggressivePosture(ePlayer);
	DoUpdateOnePlayerExpansionAggressivePosture(ePlayer);

	DoUpdateOnePlayerTargetValue(ePlayer);

	// Major Civ
	if(!GET_PLAYER(ePlayer).isMinorCiv())
	{
		DoUpdateOnePlayerOpinion(ePlayer);

		/////////////////
		// APPROACH
		/////////////////

		int iHighestWeight;
		WarFaceTypes eWarFace;

		// See which Approach is best
		MajorCivApproachTypes eApproach = GetBestApproachTowardsMajorCiv(ePlayer, /*Passed by Ref, but we won't use it*/ iHighestWeight, /*bLookAtOtherPlayers*/ true, /*bLog*/ true, /*Passed by Ref, used below*/ eWarFace);

		// If we're going to war and we haven't picked a War Face yet choose one
		if(eApproach == MAJOR_CIV_APPROACH_WAR && GetWarFaceWithPlayer(ePlayer) == NO_WAR_FACE_TYPE)
			SetWarFaceWithPlayer(ePlayer, eWarFace);

		CvAssertMsg(eApproach >= 0, "DIPLOMACY_AI: Invalid MajorCivApproachType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		// Actually assign the (possibly) new Approach
		SetMajorCivApproach(ePlayer, eApproach);
	}

	// Minor civ
	else
	{
		SetMinorCivApproach(ePlayer, MINOR_CIV_APPROACH_IGNORE);
	}
}
//	-----------------------------------------------------------------------------------------------
/// Player killed us
void CvDiplomacyAI::DoKilledByPlayer(PlayerTypes ePlayer)
{
	if(ePlayer == GC.getGame().getActivePlayer() && !GC.getGame().isNetworkMultiPlayer())
	{
		const char* szText = GetDiploStringForMessage(DIPLO_MESSAGE_DEFEATED);
		gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_DEFEATED);

		if(!GC.getGame().isGameMultiPlayer())
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_DESTROY_CIV);

			CvAchievementUnlocker::AlexanderConquest(ePlayer);
		}
	}
}
//	-------------------------------------------------------------------------------------------------------------------
/// Say hi to someone else
void CvDiplomacyAI::DoSendStatementToPlayer(PlayerTypes ePlayer, DiploStatementTypes eStatement, int iData1, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eStatement >= 0, "DIPLOMACY_AI: Invalid DiploStatementType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eStatement < NUM_DIPLO_LOG_STATEMENT_TYPES, "DIPLOMACY_AI: Invalid DiploStatementType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* szText;
	//bool bShouldShowLeaderScene = GC.getGame().getActivePlayer() == ePlayer;
	bool bHuman = GET_PLAYER(ePlayer).isHuman();
	bool bShouldShowLeaderScene = bHuman;

	// Aggressive Military warning
	if(eStatement == DIPLO_STATEMENT_AGGRESSIVE_MILITARY_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			if(IsActHostileTowardsHuman(ePlayer))
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_HOSTILE_AGGRESSIVE_MILITARY_WARNING);
			else
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_AGGRESSIVE_MILITARY_WARNING);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_AGGRESSIVE_MILITARY_WARNING, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}

	// Player Killed a City State we're protecting
	else if(eStatement == DIPLO_STATEMENT_KILLED_PROTECTED_CITY_STATE)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if(eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

				szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_KILLED_PROTECTED_CITY_STATE, NO_PLAYER, strMinorCivKey);
				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
	}

	// Player Attacked a City State we're protecting
	else if(eStatement == DIPLO_STATEMENT_ATTACKED_PROTECTED_CITY_STATE)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if(eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

				szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_ATTACKED_PROTECTED_CITY_STATE, NO_PLAYER, strMinorCivKey);
				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_ATTACKED_MINOR_CIV, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
	}

	// Player Bullied a City State we're protecting
	else if(eStatement == DIPLO_STATEMENT_BULLIED_PROTECTED_CITY_STATE)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if(eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

				szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_BULLIED_PROTECTED_CITY_STATE, NO_PLAYER, strMinorCivKey);
				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_BULLIED_MINOR_CIV, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
	}

	// Serious Expansion warning
	else if(eStatement == DIPLO_STATEMENT_EXPANSION_SERIOUS_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EXPANSION_SERIOUS_WARNING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_SERIOUS_WARNING, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// Expansion warning
	else if(eStatement == DIPLO_STATEMENT_EXPANSION_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EXPANSION_WARNING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_EXPANSION_WARNING, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}

	// Expansion Broken Promise
	else if(eStatement == DIPLO_STATEMENT_EXPANSION_BROKEN_PROMISE)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EXPANSION_BROKEN_PROMISE);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// Serious Plot Buying warning
	else if(eStatement == DIPLO_STATEMENT_PLOT_BUYING_SERIOUS_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PLOT_BUYING_SERIOUS_WARNING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_SERIOUS_WARNING, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// Plot Buying warning
	else if(eStatement == DIPLO_STATEMENT_PLOT_BUYING_WARNING)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PLOT_BUYING_WARNING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_YOU_PLOT_BUYING_WARNING, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}

	// Plot Buying broken promise
	else if(eStatement == DIPLO_STATEMENT_PLOT_BUYING_BROKEN_PROMISE)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PLOT_BUYING_BROKEN_PROMISE);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// We attacked a Minor someone has a PtP with
	else if(eStatement == DIPLO_STATEMENT_WE_ATTACKED_YOUR_MINOR)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if(eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();
				if(IsActHostileTowardsHuman(ePlayer))
					szText = GetDiploStringForMessage(DIPLO_MESSAGE_HOSTILE_WE_ATTACKED_YOUR_MINOR, NO_PLAYER, strMinorCivKey);
				else
					szText = GetDiploStringForMessage(DIPLO_MESSAGE_WE_ATTACKED_YOUR_MINOR, NO_PLAYER, strMinorCivKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_I_ATTACKED_YOUR_MINOR_CIV, szText, LEADERHEAD_ANIM_POSITIVE, eMinorCiv);

				// Extra flag, since diplo log does not save which minor civ the message was about
				SetSentAttackProtectedMinorTaunt(ePlayer, eMinorCiv, true);
			}
		}
	}

	// We bullied a Minor someone has a PtP with
	else if(eStatement == 	DIPLO_STATEMENT_WE_BULLIED_YOUR_MINOR)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			CvAssert(eMinorCiv != NO_PLAYER);
			if (eMinorCiv != NO_PLAYER)
			{
				const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();
				if(IsActHostileTowardsHuman(ePlayer))
					szText = GetDiploStringForMessage(DIPLO_MESSAGE_HOSTILE_WE_BULLIED_YOUR_MINOR, NO_PLAYER, strMinorCivKey);
				else
					szText = GetDiploStringForMessage(DIPLO_MESSAGE_WE_BULLIED_YOUR_MINOR, NO_PLAYER, strMinorCivKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_I_BULLIED_YOUR_MINOR_CIV, szText, LEADERHEAD_ANIM_POSITIVE, eMinorCiv);
			}
		}
	}

	// We'd like to work with a player
	else if(eStatement == DIPLO_STATEMENT_WORK_WITH_US)
	{
		// Send message to human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_WITH_US);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_WORK_WITH_US, szText, LEADERHEAD_ANIM_REQUEST);
		}
		// AI resolution
		else if(!bHuman)
		{
			SetDoFCounter(ePlayer, 0);
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFCounter(GetPlayer()->GetID(), 0);

			// Accept - reject is assumed from the counter
			if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDoFAcceptable(GetPlayer()->GetID()))
			{
				SetDoFAccepted(ePlayer, true);
				GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFAccepted(GetPlayer()->GetID(), true);

				LogDoF(ePlayer);
			}
		}
	}

	// We no longer want to work with a player
	else if(eStatement == DIPLO_STATEMENT_END_WORK_WITH_US)
	{
		SetDoFAccepted(ePlayer, false);
		SetDoFCounter(ePlayer, -666);

		// If we had agreed to not settle near the player, break that off
		SetPlayerNoSettleRequestAccepted(ePlayer, false);
		SetPlayerNoSettleRequestCounter(ePlayer, -666);

		// If we had agreed to not spy on the player, break that off
		SetPlayerStopSpyingRequestAccepted(ePlayer, false);
		SetPlayerStopSpyingRequestCounter(ePlayer, -666);

		// Send message to human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_END_WORK_WITH_US, ePlayer);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
		else if(!bHuman)
		{
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFAccepted(GetPlayer()->GetID(), false);
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFCounter(GetPlayer()->GetID(), -666);
		}
	}

	// Denounce
	else if(eStatement == DIPLO_STATEMENT_DENOUNCE)
	{
		DoDenouncePlayer(ePlayer);
		LogDenounce(ePlayer);

		// Send message to human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_AGAINST_SOMEONE, ePlayer);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// Denounce Friend (backstab)
	else if(eStatement == DIPLO_STATEMENT_DENOUNCE_FRIEND)
	{
		DoDenouncePlayer(ePlayer);
		LogDenounce(ePlayer, /*bBackstab*/ true);

		// Send message to human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_AI_DOF_BACKSTAB, ePlayer);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// Request Friend Denounce Someone
	else if(eStatement == DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE)
	{
		PlayerTypes eTarget = (PlayerTypes) iData1;
		CvAssert(eTarget != NO_PLAYER);
		if(eTarget != NO_PLAYER)
		{
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();

			// Send message to human
			if(bShouldShowLeaderScene)
			{
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_DOF_AI_DENOUNCE_REQUEST, ePlayer, strTargetCivKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_AI_REQUEST_DENOUNCE, szText, LEADERHEAD_ANIM_POSITIVE, eTarget);
			}
			else if(!bHuman)
			{
				bool bAgree = IsDenounceAcceptable(eTarget, /*bBias*/ true);

				LogFriendRequestDenounce(ePlayer, eTarget, bAgree);

				if(bAgree)
				{
					GET_PLAYER(ePlayer).GetDiplomacyAI()->DoDenouncePlayer(eTarget);
					GET_PLAYER(ePlayer).GetDiplomacyAI()->LogDenounce(eTarget);

					// Denounced a human?
					if(eTarget == GC.getGame().getActivePlayer())
					{
						szText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_AGAINST_SOMEONE, eTarget);
						CvDiplomacyRequests::SendRequest(ePlayer, eTarget, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
					}
				}
				else
				{
					// Oh, you're gonna say no, are you?
					if(IsFriendDenounceRefusalUnacceptable(ePlayer, eTarget))
					{
						DoDenouncePlayer(ePlayer);
						LogDenounce(ePlayer, /*bBackstab*/ false, /*bRefusal*/ true);
					}
				}
			}
		}
	}

	// We no longer want to work with a player against someone else
	//else if (eStatement == DIPLO_STATEMENT_END_WORK_AGAINST_SOMEONE)
	//{
	//	PlayerTypes eAgainstPlayer = (PlayerTypes) iData1;

	//	SetWorkingAgainstPlayerAccepted(ePlayer, eAgainstPlayer, false);
	//	SetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayer, -666);

	//	// Send message to human
	//	if (bShouldShowLeaderScene)
	//	{
	//		const char* strAgainstPlayerKey = GET_PLAYER(eAgainstPlayer).getNameKey();
	//		szText = GetDiploStringForMessage(DIPLO_MESSAGE_END_WORK_AGAINST_SOMEONE, ePlayer, strAgainstPlayerKey);

	//		gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_NEGATIVE);
	//	}
	//	else if (!bHuman)
	//	{
	//		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetWorkingAgainstPlayerAccepted(GetPlayer()->GetID(), eAgainstPlayer, false);
	//		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetWorkingAgainstPlayerCounter(GetPlayer()->GetID(), eAgainstPlayer, -666);
	//	}
	//}

	// We'd like to declare war on someone
	else if(eStatement == DIPLO_STATEMENT_COOP_WAR_REQUEST)
	{
		PlayerTypes eAgainstPlayer = (PlayerTypes) iData1;
		CvAssert(eAgainstPlayer != NO_PLAYER);
		if(eAgainstPlayer != NO_PLAYER)
		{
			// Send message to human
			if(bShouldShowLeaderScene)
			{
				const char* strAgainstPlayerKey = GET_PLAYER(eAgainstPlayer).getNameKey();
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_REQUEST, ePlayer, strAgainstPlayerKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_COOP_WAR, szText, LEADERHEAD_ANIM_POSITIVE, eAgainstPlayer);
			}
			// AI resolution
			else if(!bHuman)
			{
				SetCoopWarCounter(ePlayer, eAgainstPlayer, 0);
				GET_PLAYER(ePlayer).GetDiplomacyAI()->SetCoopWarCounter(GetPlayer()->GetID(), eAgainstPlayer, 0);

				// Will they agree?
				CoopWarStates eAcceptedState = GET_PLAYER(ePlayer).GetDiplomacyAI()->GetWillingToAgreeToCoopWarState(GetPlayer()->GetID(), eAgainstPlayer);
				GET_PLAYER(ePlayer).GetDiplomacyAI()->SetCoopWarAcceptedState(GetPlayer()->GetID(), eAgainstPlayer, eAcceptedState);

				if(eAcceptedState == COOP_WAR_STATE_ACCEPTED)
				{
					DeclareWar(eAgainstPlayer);
					GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);

					GET_PLAYER(ePlayer).GetDiplomacyAI()->DeclareWar(eAgainstPlayer);
					GET_PLAYER(ePlayer).GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);

					int iLockedTurns = /*15*/ GC.getCOOP_WAR_LOCKED_LENGTH();
					GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(GET_PLAYER(eAgainstPlayer).getTeam(), iLockedTurns);
					GET_TEAM(GET_PLAYER(ePlayer).getTeam()).ChangeNumTurnsLockedIntoWar(GET_PLAYER(eAgainstPlayer).getTeam(), iLockedTurns);
				}

				LogCoopWar(ePlayer, eAgainstPlayer, eAcceptedState);

				// If the other player didn't agree then we don't need to change our state from what it was (NO_STATE)
				if(eAcceptedState != COOP_WAR_STATE_REJECTED)
					SetCoopWarAcceptedState(ePlayer, eAgainstPlayer, eAcceptedState);
			}
		}
	}

	// We'd like to declare war on someone
	else if(eStatement == DIPLO_STATEMENT_COOP_WAR_TIME)
	{
		PlayerTypes eAgainstPlayer = (PlayerTypes) iData1;
		CvAssert(eAgainstPlayer != NO_PLAYER);
		if(eAgainstPlayer != NO_PLAYER)
		{
			// Send message to human
			if(bShouldShowLeaderScene)
			{
				const char* strAgainstPlayerKey = GET_PLAYER(eAgainstPlayer).getNameKey();
				szText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_TIME, ePlayer, strAgainstPlayerKey);

				CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_COOP_WAR_TIME, szText, LEADERHEAD_ANIM_POSITIVE, eAgainstPlayer);
			}
		}

		// No AI resolution! This is handled automatically in DoCounters() - no need for diplo exchange
	}

	// We're making a demand of Player
	else if(eStatement == DIPLO_STATEMENT_DEMAND)
	{
		// Active human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_DEMAND);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_DEMAND, szText, LEADERHEAD_ANIM_DEMAND);
		}
		// AI player
		else if(!bHuman)
		{
			// For now the AI will always give in

			CvDeal kDeal = *pDeal;

			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}

	// We're making a request of Player
	else if(eStatement == DIPLO_STATEMENT_REQUEST)
	{
		// Active human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_REQUEST);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_REQUEST, szText, LEADERHEAD_ANIM_REQUEST);
		}
		// AI player
		else if(!bHuman)
		{
			// For now the AI will always give in - may eventually write additional logic here

			CvDeal kDeal = *pDeal;

			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}

	// Player has a Luxury we'd like
	else if(eStatement == DIPLO_STATEMENT_LUXURY_TRADE)
	{
		// Active human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_LUXURY_TRADE);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}
		// Offer to an AI player
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;

			// Don't need to call DoOffer because we check to see if the deal works for both sides BEFORE sending
			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}

	// Offer Embassy Exchange
	else if(eStatement == DIPLO_STATEMENT_EMBASSY_EXCHANGE)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EMBASSY_EXCHANGE);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;

			// Don't need to call DoOffer because we check to see if the deal works for both sides BEFORE sending
			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}

	// Offer Embassy
	else if(eStatement == DIPLO_STATEMENT_EMBASSY_OFFER)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_EMBASSY_OFFER);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;

			// Don't need to call DoOffer because we check to see if the deal works for both sides BEFORE sending
			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}

	// Offer Open Borders Exchange
	else if(eStatement == DIPLO_STATEMENT_OPEN_BORDERS_EXCHANGE)
	{
		// Active human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_OPEN_BORDERS_EXCHANGE);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}
		// Offer to an AI player
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;

			// Don't need to call DoOffer because we check to see if the deal works for both sides BEFORE sending
			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}

	// Offer Open Borders
	else if(eStatement == DIPLO_STATEMENT_OPEN_BORDERS_OFFER)
	{
		// Active human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_OPEN_BORDERS_OFFER);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}
		// Offer to an AI player
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;

			// Don't need to call DoOffer because we check to see if the deal works for both sides BEFORE sending
			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}

	// Offer plans to make Research Agreement
	else if(eStatement == DIPLO_STATEMENT_PLAN_RESEARCH_AGREEMENT)
	{
		// Active human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PLAN_RESEARCH_AGREEMENT);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_DISCUSS_PLAN_RESEARCH_AGREEMENT, szText, LEADERHEAD_ANIM_REQUEST);
		}
		// Offer to an AI player
		else if(!bHuman)
		{
			if(!GET_PLAYER(ePlayer).GetDiplomacyAI()->IsWantsResearchAgreementWithPlayer(GetPlayer()->GetID()))
				GET_PLAYER(ePlayer).GetDiplomacyAI()->DoAddWantsResearchAgreementWithPlayer(GetPlayer()->GetID());	// just auto-reciprocate right now
		}
	}

	// Offer Research Agreement
	else if(eStatement == DIPLO_STATEMENT_RESEARCH_AGREEMENT_OFFER)
	{
		// Active human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_RESEARCH_AGREEMENT_OFFER);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
		}
		// Offer to an AI player
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;

			// Don't need to call DoOffer because we check to see if the deal works for both sides BEFORE sending
			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}

	// Offer to renew deal
	else if(eStatement == DIPLO_STATEMENT_RENEW_DEAL)
	{
		// Active human
		if(bShouldShowLeaderScene)
		{
			int iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer;
			DiploMessageTypes eMessageType = NUM_DIPLO_MESSAGE_TYPES;
			bool bCantMatchOffer;
			bool bDealAcceptable = m_pPlayer->GetDealAI()->IsDealWithHumanAcceptable(pDeal, ePlayer, iDealValueToMe, iValueImOffering, iValueTheyreOffering, iAmountOverWeWillRequest, iAmountUnderWeWillOffer, bCantMatchOffer);

			if(!bDealAcceptable)
			{
				if(iValueTheyreOffering > iValueImOffering)
				{
					bDealAcceptable = true;
				}
			}

			if(bDealAcceptable)
			{
				eMessageType = DIPLO_MESSAGE_RENEW_DEAL;
			}
			// We want more from this Deal
			else if(iDealValueToMe > -75 &&
			        iValueImOffering < (iValueTheyreOffering * 5))	// The total value of the deal might not be that bad, but if he's asking for WAY more than he's offering (e.g. something for nothing) then it's not unacceptable, but insulting
			{
				eMessageType = DIPLO_MESSAGE_WANT_MORE_RENEW_DEAL;
			}
			else
			{
				CvDeal* pRenewDeal = GetDealToRenew();
				if (pRenewDeal)
				{
					pRenewDeal->m_bCheckedForRenewal = true;
				}
				ClearDealToRenew();
			}

			if(eMessageType != NUM_DIPLO_MESSAGE_TYPES)
			{
				CvDeal* pRenewDeal = GetDealToRenew();
				if (pRenewDeal)
				{
					pRenewDeal->m_bCheckedForRenewal = true;
				}
				szText = GetDiploStringForMessage(eMessageType);
				CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_REQUEST);
			}
		}
		// Offer to an AI player
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;
			int iDealType = -1;
			CvDeal* pRenewedDeal = m_pPlayer->GetDiplomacyAI()->GetDealToRenew(&iDealType);
			if(pRenewedDeal)
			{	
				if (iDealType != 0) // this is not a historic deal, so don't change the resource allocations
				{
					CvGameDeals::PrepareRenewDeal(m_pPlayer->GetDiplomacyAI()->GetDealToRenew(), &kDeal);
				}
				pRenewedDeal->m_bCheckedForRenewal = true;
				m_pPlayer->GetDiplomacyAI()->ClearDealToRenew();
			}

			// Don't need to call DoOffer because we check to see if the deal works for both sides BEFORE sending
			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);
		}
	}

	// They're now unforgivable
	else if(eStatement == DIPLO_STATEMENT_NOW_UNFORGIVABLE)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_NOW_UNFORGIVABLE);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// They're now an enemy
	else if(eStatement == DIPLO_STATEMENT_NOW_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_NOW_ENEMY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// They caught one of our spies
	else if(eStatement == DIPLO_STATEMENT_CAUGHT_YOUR_SPY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_CAUGHT_YOUR_SPY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_CAUGHT_YOUR_SPY, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// They killed one of our spies
	else if(eStatement == DIPLO_STATEMENT_KILLED_YOUR_SPY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_KILLED_YOUR_SPY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_KILLED_YOUR_SPY, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}

	// We killed one of their spies
	else if(eStatement == DIPLO_STATEMENT_KILLED_MY_SPY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_KILLED_MY_SPY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_KILLED_MY_SPY, szText, LEADERHEAD_ANIM_DEFEATED);
		}
	}

	// We (the AI) have intrigue information to share with them
	else if(eStatement == DIPLO_STATEMENT_SHARE_INTRIGUE)
	{
		IntrigueNotificationMessage* pNotificationMessage = GetPlayer()->GetEspionage()->GetRecentIntrigueInfo(ePlayer);
		CvAssertMsg(pNotificationMessage, "pNotificationMessage is null. Whut?");
		if (pNotificationMessage)
		{
			CvAssertMsg(pNotificationMessage->m_eSourcePlayer != NO_PLAYER, "There is no plotter! What's going on");
			PlayerTypes ePlotterPlayer = pNotificationMessage->m_eSourcePlayer;
			CvIntrigueType eIntrigueType = (CvIntrigueType)pNotificationMessage->m_iIntrigueType;
			// don't share intrigue about two parties if they are already at war
			if (!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(ePlotterPlayer).getTeam()))
			{
				CvCity* pCity = NULL;
				if(pNotificationMessage->m_iCityX != -1 && pNotificationMessage->m_iCityY != -1)
				{
					CvPlot* pPlot = GC.getMap().plot(pNotificationMessage->m_iCityX, pNotificationMessage->m_iCityY);
					if(pPlot)
					{
						pCity = pPlot->getPlotCity();
					}
				}

				// add the notification to the 
				GET_PLAYER(ePlayer).GetEspionage()->AddIntrigueMessage(m_pPlayer->GetID(), ePlotterPlayer, ePlayer, NO_BUILDING, NO_PROJECT, eIntrigueType, 0, pCity, false);

				if(bShouldShowLeaderScene)
				{
					const char* szPlayerName;
					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(ePlotterPlayer).isHuman())
					{
						szPlayerName = GET_PLAYER(ePlotterPlayer).getNickName();
					}
					else
					{
						szPlayerName = GET_PLAYER(ePlotterPlayer).getNameKey();
					}

					szText = "";

					switch(eIntrigueType)
					{
					case INTRIGUE_TYPE_ARMY_SNEAK_ATTACK:
						if(pCity)
						{
							szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_KNOWN_CITY, NO_PLAYER, szPlayerName, pCity->getNameKey());
						}
						else
						{
							szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_UNKNOWN_CITY, NO_PLAYER, szPlayerName);
						}
						break;
					case INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK:
						if(pCity)
						{
							szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_KNOWN_CITY, NO_PLAYER, szPlayerName, pCity->getNameKey());
						}
						else
						{
							szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_UNKNOWN_CITY, NO_PLAYER, szPlayerName);
						}
						break;
					case INTRIGUE_TYPE_DECEPTION:
						szText = GetDiploStringForMessage(DIPLO_MESSAGE_SHARE_INTRIGUE, NO_PLAYER, szPlayerName);
						break;
					default:
						CvAssertMsg(false, "Unknown intrigue type");
						break;
					}

					CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
				}

			}

			// mark the messages as shared so the player isn't told the same thing repeatedly
			for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)ui;
				GET_PLAYER(eLoopPlayer).GetEspionage()->MarkRecentIntrigueAsShared(ePlayer, ePlotterPlayer, eIntrigueType);
			}
		}
	}

	// Stop converting our cities
	else if(eStatement == DIPLO_STATEMENT_STOP_CONVERSIONS)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_CONVERSIONS);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_STOP_CONVERSIONS, szText, LEADERHEAD_ANIM_DEFEATED);
		}
	}

	// Insult
	else if(eStatement == DIPLO_STATEMENT_INSULT)
	{
		// Change other players' guess as to our Approach (right now it falls in line exactly with the Approach...)
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuess(GetPlayer()->GetID(), MAJOR_CIV_APPROACH_HOSTILE);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuessCounter(GetPlayer()->GetID(), 0);

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_INSULT_ROOT);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// Compliment
	else if(eStatement == DIPLO_STATEMENT_COMPLIMENT)
	{
		// Change other players' guess as to our Approach (right now it falls in line exactly with the Approach...)
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuess(GetPlayer()->GetID(), MAJOR_CIV_APPROACH_FRIENDLY);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuessCounter(GetPlayer()->GetID(), 0);

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_COMPLIMENT);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	// Boot-kissing of a stronger power
	else if(eStatement == DIPLO_STATEMENT_BOOT_KISSING)
	{
		// Change other players' guess as to our Approach (right now it falls in line exactly with the Approach...)
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuess(GetPlayer()->GetID(), MAJOR_CIV_APPROACH_AFRAID);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetApproachTowardsUsGuessCounter(GetPlayer()->GetID(), 0);

		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_BOOT_KISSING);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	// We're warning a player his warmongering behavior is attracting attention
	else if(eStatement == DIPLO_STATEMENT_WARMONGER)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_WARMONGER);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// We're warning a player his interactions with city states is not to our liking
	else if(eStatement == DIPLO_STATEMENT_MINOR_CIV_COMPETITION)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eMinorCiv = (PlayerTypes) iData1;
			const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

			szText = GetDiploStringForMessage(DIPLO_MESSAGE_MINOR_CIV_COMPETITION, NO_PLAYER, strMinorCivKey);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_NEGATIVE);
		}
	}

	// Human befriended an enemy of this AI!
	else if(eStatement == DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DOFED_ENEMY, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// Human denounced a friend of this AI!
	else if(eStatement == DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DENOUNCED_FRIEND, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// Human denounced an enemy of this AI!
	else if(eStatement == DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DENOUNCED_ENEMY, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	// Human befriended a friend of this AI!
	else if(eStatement == DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DOFED_FRIEND, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	// AI befriended an enemy of the human!
	else if(eStatement == DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DENOUNCE_SO_AI_DOF, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// AI denounced a friend of the human!
	else if(eStatement == DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DOF_SO_AI_DENOUNCE, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, szText, LEADERHEAD_ANIM_HATE_NEGATIVE);
		}
	}

	// AI denounced an enemy of the human!
	else if(eStatement == DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DENOUNCE_SO_AI_DENOUNCE, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	// AI befriended a friend of the human!
	else if(eStatement == DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND)
	{
		if(bShouldShowLeaderScene)
		{
			PlayerTypes eTarget = (PlayerTypes) iData1;
			const char* strTargetCivKey = GET_PLAYER(eTarget).getCivilizationShortDescriptionKey();
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DOF_SO_AI_DOF, ePlayer, strTargetCivKey);

			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	// AI chose same late game policy tree!
	else if(eStatement == DIPLO_STATEMENT_SAME_POLICIES_FREEDOM)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_SAME_POLICIES_FREEDOM);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	else if(eStatement == DIPLO_STATEMENT_SAME_POLICIES_ORDER)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_SAME_POLICIES_ORDER);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	else if(eStatement == DIPLO_STATEMENT_SAME_POLICIES_AUTOCRACY)
	{
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_SAME_POLICIES_AUTOCRACY);
			CvDiplomacyRequests::SendRequest(GetPlayer()->GetID(), ePlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, szText, LEADERHEAD_ANIM_POSITIVE);
		}
	}

	// Do we want peace with ePlayer?
	else if(eStatement == DIPLO_STATEMENT_REQUEST_PEACE)
	{
		// Active human
		if(bShouldShowLeaderScene)
		{
			szText = GetDiploStringForMessage(DIPLO_MESSAGE_PEACE_OFFER);
			CvDiplomacyRequests::SendDealRequest(GetPlayer()->GetID(), ePlayer, pDeal, DIPLO_UI_STATE_TRADE_AI_MAKES_OFFER, szText, LEADERHEAD_ANIM_POSITIVE);
		}
		// Offer to an AI player
		else if(!bHuman)
		{
			CvDeal kDeal = *pDeal;

			// Don't need to call DoOffer because we check to see if the deal works for both sides BEFORE sending
			GC.getGame().GetGameDeals()->AddProposedDeal(kDeal);
			GC.getGame().GetGameDeals()->FinalizeDeal(GetPlayer()->GetID(), ePlayer, true);

			LogPeaceMade(ePlayer);
		}
	}
}
//	-------------------------------------------------------------------------------------------------------------------
/// Does this AI have something to say to the world this turn?
void CvDiplomacyAI::DoMakePublicDeclaration(PublicDeclarationTypes eDeclaration, int iData1, int iData2, PlayerTypes eMustHaveMetPlayer, PlayerTypes eForSpecificPlayer)
{
	// Don't give Public Declarations if we're a human
	if(GetPlayer()->isHuman())
	{
		return;
	}

	CvAssertMsg(eDeclaration >= 0, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeclaration < NUM_PUBLIC_DECLARATION_TYPES, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eForSpecificPlayer >= NO_PLAYER, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");	// We can be sending to no specific player
	CvAssertMsg(eForSpecificPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "";
	//LeaderheadAnimationTypes eAnimation;
	//DiploUIStateTypes eDiploState = DIPLO_UI_STATE_DEFAULT_ROOT;

	bool bActive = true;

	// We're protecting a Minor Civ, so watch out!
	if(eDeclaration == PUBLIC_DECLARATION_PROTECT_MINOR)
	{
		PlayerTypes eMinorCiv = (PlayerTypes) iData1;
		const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

		strText = GetDiploStringForMessage(DIPLO_MESSAGE_DECLARATION_PROTECT_CITY_STATE, NO_PLAYER, strMinorCivKey);
		//eAnimation = LEADERHEAD_ANIM_POSITIVE;
		//eDiploState = DIPLO_UI_STATE_DISCUSS_PROTECT_MINOR_CIV;
	}

	// We're no longer protecting a Minor Civ, sorry...
	if(eDeclaration == PUBLIC_DECLARATION_ABANDON_MINOR)
	{
		// Make previous declarations to protect this Minor inactive
		DoMakeDeclarationInactive(PUBLIC_DECLARATION_PROTECT_MINOR, iData1, iData2);

		// No point in telling new people we JUST meet we're not protecting someone any more...
		bActive = false;

		PlayerTypes eMinorCiv = (PlayerTypes) iData1;
		const char* strMinorCivKey = GET_PLAYER(eMinorCiv).getNameKey();

		strText = GetDiploStringForMessage(DIPLO_MESSAGE_DECLARATION_ABANDON_CITY_STATE, NO_PLAYER, strMinorCivKey);
		//eAnimation = LEADERHEAD_ANIM_POSITIVE;
	}



	// Should also send to the other AIs here somehow



	// If our declaration is only for a specific player (e.g. we just met them) take that into account
	if(eForSpecificPlayer != NO_PLAYER)
	{
	}

	// Only add this Declaration to the log if it's for everyone, as announcements to specific people are only for catching them up after meeting them later
	if(eForSpecificPlayer == NO_PLAYER)
	{
		DoAddNewDeclarationToLog(eDeclaration, iData1, iData2, eMustHaveMetPlayer, bActive);

		//Send notification to everyone that can get it.
		for(int iCurPlayer = 0; iCurPlayer < MAX_MAJOR_CIVS; ++iCurPlayer){
			PlayerTypes eCurPlayer = (PlayerTypes) iCurPlayer;
			CvPlayerAI& kCurPlayer = GET_PLAYER(eCurPlayer);
			if(IsPlayerValid(eCurPlayer) 
				&& (eMustHaveMetPlayer == NO_PLAYER || GET_TEAM(kCurPlayer.getTeam()).isHasMet(GET_PLAYER(eMustHaveMetPlayer).getTeam()))){
				CvNotifications* pNotifications = GET_PLAYER(eCurPlayer).GetNotifications();
				if(pNotifications){
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_DECLARATION");
					strSummary << GetPlayer()->getCivilizationShortDescriptionKey();
					pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText, strSummary.toUTF8(), -1, -1, -1);
				}
			}
		}
	}
	else
	{
		//send notification to the specific player.
		CvPlayerAI& kSpecificPlayer = GET_PLAYER(eForSpecificPlayer);
		if(IsPlayerValid(eForSpecificPlayer) 
			&& (eMustHaveMetPlayer == NO_PLAYER || GET_TEAM(kSpecificPlayer.getTeam()).isHasMet(GET_PLAYER(eMustHaveMetPlayer).getTeam()))){	
			CvNotifications* pNotifications = kSpecificPlayer.GetNotifications();
			if(pNotifications){
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DIPLOMACY_DECLARATION");
				strSummary << GetPlayer()->getCivilizationShortDescriptionKey();
				pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText, strSummary.toUTF8(), -1, -1, -1);
			}
		}
	}

	LogPublicDeclaration(eDeclaration, iData1, eForSpecificPlayer);
}

/// Anyone we want to chat with?
void CvDiplomacyAI::DoContactMajorCivs()
{
	// NOTE: This function is broken up into two sections: AI contact opportunities, and then human contact opportunities
	// This is to prevent a nasty bug where the AI will continue making decisions as the diplo screen is firing up. Making humans
	// handled at the end prevents the Diplo AI from having this problem

	// Loop through AI Players
	PlayerTypes eLoopPlayer;
	int iPlayerLoop;

	for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(!IsPlayerValid(eLoopPlayer))
			continue;

		// No humans
		if(GET_PLAYER(eLoopPlayer).isHuman())
			continue;

		DoContactPlayer(eLoopPlayer);
	}

	// Loop through HUMAN Players - if we're not in MP
	if(!CvPreGame::isNetworkMultiplayerGame())
	{
		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(!IsPlayerValid(eLoopPlayer))
				continue;

			// No AI
			if(!GET_PLAYER(eLoopPlayer).isHuman())
				continue;

			DoContactPlayer(eLoopPlayer);
		}
	}
}

/// Individual contact opportunity
void CvDiplomacyAI::DoContactPlayer(PlayerTypes ePlayer)
{
	if(!IsValidUIDiplomacyTarget(ePlayer))
		return;		// Can't contact the this player at the moment.

	int iDiploLogStatement;
	DiploStatementTypes eStatement;

	// We can use this deal pointer to form a trade offer
	CvDeal* pDeal = GC.getGame().GetGameDeals()->GetTempDeal();

	// These can be used for info about deal items, e.g. what Minor Civ we're telling the guy to stay away from, etc.
	int iData1;
	int iData2;

	// If this is the same turn we've met a player, don't send anything his way quite yet - wait until we've said hello at least
	if(GET_TEAM(GetTeam()).GetTurnsSinceMeetingTeam(GET_PLAYER(ePlayer).getTeam()) == 0)
		return;

	// Clear out the scratch pad
	for(int iLoop = 0; iLoop < NUM_DIPLO_LOG_STATEMENT_TYPES; iLoop++)
	{
		m_paDiploLogStatementTurnCountScratchPad[iLoop] = MAX_TURNS_SAFE_ESTIMATE;
	}

	// Make a scratch pad keeping track of the last time we sent each message.  This way we can know what we've said in the past already - this member array will be used in the function calls below
	for(iDiploLogStatement = 0; iDiploLogStatement < MAX_DIPLO_LOG_STATEMENTS; iDiploLogStatement++)
	{
		eStatement = GetDiploLogStatementTypeForIndex(ePlayer, iDiploLogStatement);

		if(eStatement != NO_DIPLO_STATEMENT_TYPE)
		{
			CvAssert(eStatement < NUM_DIPLO_LOG_STATEMENT_TYPES);

			m_paDiploLogStatementTurnCountScratchPad[eStatement] = GetDiploLogStatementTurnForIndex(ePlayer, iDiploLogStatement);
		}
	}

	eStatement = NO_DIPLO_STATEMENT_TYPE;

	iData1 = -1;
	iData2 = -1;

	pDeal->ClearItems();
	pDeal->SetFromPlayer(GetPlayer()->GetID());
	pDeal->SetToPlayer(ePlayer);

	// JON: Add in some randomization here?
	// How predictable do we want the AI to be with regards to what state they're in?

	// Note that the order in which the following functions are called is very important to how the AI behaves - first come, first served

	// AT PEACE
	if(!IsAtWar(ePlayer))
	{
		DoCoopWarTimeStatement(ePlayer, eStatement, iData1);
		DoCoopWarStatement(ePlayer, eStatement, iData1);

		// Some things we don't say to teammates
		if(GetPlayer()->getTeam() != GET_PLAYER(ePlayer).getTeam())
		{
			DoMakeDemand(ePlayer, eStatement, pDeal);

			// STATEMENTS - all members but ePlayer passed by address
			DoAggressiveMilitaryStatement(ePlayer, eStatement);
			DoKilledCityStateStatement(ePlayer, eStatement, iData1);
			DoAttackedCityStateStatement(ePlayer, eStatement, iData1);
			DoBulliedCityStateStatement(ePlayer, eStatement, iData1);
			//DoSeriousExpansionWarningStatement(ePlayer, eStatement);
			DoExpansionWarningStatement(ePlayer, eStatement);
			DoExpansionBrokenPromiseStatement(ePlayer, eStatement);
			//DoSeriousPlotBuyingWarningStatement(ePlayer, eStatement);
			DoPlotBuyingWarningStatement(ePlayer, eStatement);
			DoPlotBuyingBrokenPromiseStatement(ePlayer, eStatement);

			DoWeAttackedYourMinorStatement(ePlayer, eStatement, iData1);
			DoWeBulliedYourMinorStatement(ePlayer, eStatement, iData1);

			DoKilledYourSpyStatement(ePlayer, eStatement);
			DoKilledMySpyStatement(ePlayer, eStatement);
			DoCaughtYourSpyStatement(ePlayer, eStatement);

			DoConvertedMyCityStatement(ePlayer, eStatement);

			DoDoFStatement(ePlayer, eStatement);
			DoDenounceFriendStatement(ePlayer, eStatement);
			DoDenounceStatement(ePlayer, eStatement);
			DoRequestFriendDenounceStatement(ePlayer, eStatement, iData1);
			//DoWorkAgainstSomeoneStatement(ePlayer, eStatement, iData1);
			//DoEndWorkAgainstSomeoneStatement(ePlayer, eStatement, iData1);
		}

		//	OFFERS - all members but ePlayer passed by address
		DoLuxuryTrade(ePlayer, eStatement, pDeal);
		DoEmbassyExchange(ePlayer, eStatement, pDeal);
		DoEmbassyOffer(ePlayer, eStatement, pDeal);
		DoOpenBordersExchange(ePlayer, eStatement, pDeal);
		DoOpenBordersOffer(ePlayer, eStatement, pDeal);
		DoResearchAgreementOffer(ePlayer, eStatement, pDeal);
		DoRenewExpiredDeal(ePlayer, eStatement, pDeal);
		DoShareIntrigueStatement(ePlayer, eStatement);
		//DoResearchAgreementPlan(ePlayer, eStatement);

		DoRequest(ePlayer, eStatement, pDeal);

		// Second set of things we don't say to teammates
		if(GetPlayer()->getTeam() != GET_PLAYER(ePlayer).getTeam())
		{
			//DoNowUnforgivableStatement(ePlayer, eStatement);
			//DoNowEnemyStatement(ePlayer, eStatement);

			DoHostileStatement(ePlayer, eStatement);
			//DoFriendlyStatement(ePlayer, eStatement);
			DoAfraidStatement(ePlayer, eStatement);
			DoWarmongerStatement(ePlayer, eStatement);
			DoMinorCivCompetitionStatement(ePlayer, eStatement, iData1);

			// Don't bother with this fluff stuff it's just AI on AI stuff
			if(GET_PLAYER(ePlayer).isHuman())
			{
				DoAngryBefriendedEnemy(ePlayer, eStatement, iData1);
				DoAngryDenouncedFriend(ePlayer, eStatement, iData1);
				DoHappyDenouncedEnemy(ePlayer, eStatement, iData1);
				DoHappyBefriendedFriend(ePlayer, eStatement, iData1);
				DoFYIBefriendedHumanEnemy(ePlayer, eStatement, iData1);
				DoFYIDenouncedHumanFriend(ePlayer, eStatement, iData1);
				DoFYIDenouncedHumanEnemy(ePlayer, eStatement, iData1);
				DoFYIBefriendedHumanFriend(ePlayer, eStatement, iData1);
				DoHappySamePolicyTree(ePlayer, eStatement);
			}
		}
	}

	// AT WAR
	else if(!GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		//	OFFERS - all members but ePlayer passed by address
		DoPeaceOffer(ePlayer, eStatement, pDeal);
	}

#if !defined(FINAL_RELEASE)
	// Check for an optional message injection from the Tuner
	if(eStatement == NO_DIPLO_STATEMENT_TYPE && m_eTestStatement != NO_DIPLO_STATEMENT_TYPE && ePlayer == m_eTestToPlayer)
	{
		eStatement = m_eTestStatement;
		iData1 = m_iTestStatementArg1;

		m_eTestStatement = NO_DIPLO_STATEMENT_TYPE;
	}
#endif

	// Now see if it's a valid time to send this message (we may have already sent it)
	if(eStatement != NO_DIPLO_STATEMENT_TYPE)
	{
		//if (bSendStatement)
		{
			LogStatementToPlayer(ePlayer, eStatement);

			DoSendStatementToPlayer(ePlayer, eStatement, iData1, pDeal);
			DoAddNewStatementToDiploLog(ePlayer, eStatement);
		}
	}
}

/// Anyone we want to chat with?
void CvDiplomacyAI::DoContactMinorCivs()
{
	// If the player has deleted the DIPLOMACY Flavor we have to account for that
	int iDiplomacyFlavor = /*5*/ GC.getDEFAULT_FLAVOR_VALUE();
	int iGoldFlavor = /*5*/ GC.getDEFAULT_FLAVOR_VALUE();
	int iTileImprovementFlavor = /*5*/ GC.getDEFAULT_FLAVOR_VALUE();
	int iExpansionFlavor = /*5*/ GC.getDEFAULT_FLAVOR_VALUE();

	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_DIPLOMACY")
		{
			iDiplomacyFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_GOLD")
		{
			iGoldFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_TILE_IMPROVEMENT")
		{
			iTileImprovementFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
		else if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iExpansionFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
	}
	EconomicAIStrategyTypes eFoundCity = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_FOUND_CITY");
	EconomicAIStrategyTypes eExpandLikeCrazy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY");
	EconomicAIStrategyTypes eExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	EconomicAIStrategyTypes eNeedHappiness = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS");
	EconomicAIStrategyTypes eNeedHappinessCritical = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL");
	EconomicAIStrategyTypes eLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	bool bFoundCity = (eFoundCity != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eFoundCity) : false;
	bool bExpandLikeCrazy = (eExpandLikeCrazy != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eExpandLikeCrazy) : false;
	bool bExpandToOtherContinents = (eExpandToOtherContinents != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eExpandToOtherContinents) : false;
	bool bNeedHappiness = (eNeedHappiness != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNeedHappiness) : false;
	bool bNeedHappinessCritical = (eNeedHappinessCritical != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eNeedHappinessCritical) : false;
	bool bLosingMoney = (eLosingMoney != NO_ECONOMICAISTRATEGY) ? GetPlayer()->GetEconomicAI()->IsUsingStrategy(eLosingMoney) : false;

	// **************************
	// Would we like to buyout a minor this turn?  (Austria UA)
	// **************************
	bool bWantsToBuyout = false;
	if(GetPlayer()->GetPlayerTraits()->IsAbleToAnnexCityStates())
	{
		if(bFoundCity || bExpandLikeCrazy || bExpandToOtherContinents ||
		        GetStateAllWars() == STATE_ALL_WARS_LOSING ||
		        IsGoingForWorldConquest() ||
		        m_pPlayer->calculateGoldRate() > 100)
		{
			bWantsToBuyout = true;
		}
		else
		{
			int iThreshold = iExpansionFlavor * 5; //antonjs: todo: xml
			int iRandRoll = GC.getGame().getJonRandNum(100, "Diplomacy AI: good turn to buyout a minor?");

			if(iRandRoll < iThreshold)
				bWantsToBuyout = true;
		}
	}

	// **************************
	// Would we like to give a gold gift this turn?
	// **************************
	bool bWantsToMakeGoldGift = false;

	// If we're a highly diplomatic leader, then always look for an opportunity
	if(iDiplomacyFlavor >= /*4*/ GC.getMC_ALWAYS_GIFT_DIPLO_THRESHOLD() ||
	        IsGoingForDiploVictory() ||
	        IsGoingForCultureVictory() ||
	        GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT) ||
	        IsHasActiveGoldQuest() ||
	        m_pPlayer->calculateGoldRate() > 100) // if we are very wealthy always do this
	{
		bWantsToMakeGoldGift = true;
	}
	// Otherwise, do a random roll
	else
	{
		int iThreshold = iDiplomacyFlavor* /*5*/ GC.getMC_SOMETIMES_GIFT_RAND_MULTIPLIER();
		int iRandRoll = GC.getGame().getJonRandNum(100, "Diplomacy AI: good turn to make a gold gift to a minor?");

		// Threshold will be 15 for a player (3 flavor * 5)
		// Threshold will be 5 for non-diplomatic player (2 flavor * 5)

		if(iRandRoll < iThreshold)
			bWantsToMakeGoldGift = true;
	}

	// **************************
	// Would we like to get a unit by bullying this turn?
	// **************************
	bool bWantsToBullyUnit = false;

	if(GetPlayer()->GetEconomicAI()->GetWorkersToCitiesRatio() < 0.25 &&  //antonjs: todo: XML
	        GetPlayer()->GetEconomicAI()->GetImprovedToImprovablePlotsRatio() < 0.50) //antonjs: todo: XML
	{
		bWantsToBullyUnit = true;
	}
	// Otherwise, do a random roll
	else
	{
		int iThreshold = iTileImprovementFlavor * 3; //antonjs: todo: XML
		int iRandRoll = GC.getGame().getJonRandNum(100, "Diplomacy AI: good turn to bully a unit (worker) from a minor?");

		if(iRandRoll < iThreshold)
			bWantsToBullyUnit = true;
	}
	//antonjs: todo: if too many workers then set to false (ex. want to disband workers you have)

	// **************************
	// Would we like to get some gold by bullying this turn?
	// **************************
	bool bWantsToBullyGold = false;

	if(iGoldFlavor >= 6 ||  //antonjs: todo: GC.getMC_ALWAYS_BULLY_GOLD_THRESHOLD()
	        IsGoingForWorldConquest() ||
	        GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_UNIT) ||
	        GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_BUILDING) ||
	        bLosingMoney ||
	        m_pPlayer->calculateGoldRate() < 0) // if we are losing gold per turn
	{
		bWantsToBullyGold = true;
	}
	// Otherwise, do a random roll
	else
	{
		int iThreshold = iGoldFlavor * 4; //antonjs: todo: XML
		int iRandRoll = GC.getGame().getJonRandNum(100, "Diplomacy AI: good turn to bully gold from a minor?");

		if(iRandRoll < iThreshold)
			bWantsToBullyGold = true;
	}

	if(BULLY_DEBUGGING)
	{
		//bWantsToMakeGoldGift = false;
		//bWantsToBullyGold = true;
		//bWantsToBullyUnit = true;
	}

	CvWeightedVector<PlayerTypes, MAX_PLAYERS, true> veMinorsToBuyout; // Austria UA
	CvWeightedVector<MinorGoldGiftInfo, MAX_PLAYERS, true> veMinorsToGiveGold;
	CvWeightedVector<PlayerTypes, MAX_PLAYERS, true> veMinorsToBullyGold;
	CvWeightedVector<PlayerTypes, MAX_PLAYERS, true> veMinorsToBullyUnit;

	int iLargeGift = /*1000*/ GC.getMINOR_GOLD_GIFT_LARGE();
	int iMediumGift = /*500*/ GC.getMINOR_GOLD_GIFT_MEDIUM();
	int iSmallGift = /*250*/ GC.getMINOR_GOLD_GIFT_SMALL();
	int iLargeGiftFriendship;
	int iMediumGiftFriendship;
	int iSmallGiftFriendship;
	bool bMediumGiftAllies;
	bool bSmallGiftAllies;

	PlayerTypes eID = GetPlayer()->GetID();

	CvMinorCivInfo* pMinorInfo;
	CvPlayer* pMinor;
	CvMinorCivAI* pMinorCivAI;

	bool bIntruding;

	int iOtherMajorLoop;
	PlayerTypes eOtherMajor;
	int iFriendshipWithMinor;
	int iOtherPlayerFriendshipWithMinor;

	bool bWantsToConnect;

	MinorCivApproachTypes eApproach;

	int iGrowthFlavor = GetPlayer()->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) GC.getInfoTypeForString("FLAVOR_GROWTH"));

	// Loop through all (known) Minors
	PlayerTypes eMinor;
	TeamTypes eMinorTeam;
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		eMinor = (PlayerTypes) iMinorLoop;

		pMinor = &GET_PLAYER(eMinor);
		pMinorCivAI = pMinor->GetMinorCivAI();
		eMinorTeam = pMinor->getTeam();

		bWantsToConnect = false;
		bool bWantsToGiveGoldToThisMinor = false;
		bool bWantsToBullyUnitFromThisMinor = false;
		bool bWantsToBullyGoldFromThisMinor = false;
		bool bWantsToBuyoutThisMinor = false;

		if(IsPlayerValid(eMinor))
		{
			// Can't do anything with minors we're at war with, besides make peace (which isn't done here, but in DoMakePeaceWithMinors())
			if(IsAtWar(eMinor))
				continue;

			eApproach = GetMinorCivApproach(eMinor);

			// Do we want to change our protection of this minor?
			DoUpdateMinorCivProtection(eMinor, eApproach);

			// Do we want to connect to this player?
			if(pMinorCivAI->IsActiveQuestForPlayer(eID, MINOR_CIV_QUEST_ROUTE))
			{
				if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE ||
				        eApproach == MINOR_CIV_APPROACH_FRIENDLY)
				{
					if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
						bWantsToConnect = true;
				}
			}

			bIntruding = true;

			// We have open borders so we're definitely not intruding
			if(pMinorCivAI->IsPlayerHasOpenBorders(eID))
				bIntruding = false;

			else
			{
				// Cares and doesn't yet have enough friendship for Open Borders
				if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE || eApproach == MINOR_CIV_APPROACH_FRIENDLY)
					bIntruding = false;
			}

			pMinorCivAI->SetMajorIntruding(eID, bIntruding);

			// Calculate desirability to buyout this minor
			if(bWantsToBuyout)
			{
				int iValue = 100; //antonjs: todo: xml
				// Only bother if we actually can buyout
				CvCity* pMinorCapital = pMinor->getCapitalCity();
				if(GetPlayer()->GetPlayerTraits()->IsAbleToAnnexCityStates() && pMinorCivAI->CanMajorBuyout(eID) && pMinorCapital != NULL)
				{
					// Determine presence of player cities on this continent
					int iMinorArea = pMinorCapital->getArea();
					CvArea* pMinorArea = GC.getMap().getArea(iMinorArea);
					bool bPresenceInArea = false;
					int iMajorCapitalsInArea = 0;
					if(pMinorArea)
					{
						// Do we have a city here?
						if(pMinorArea->getCitiesPerPlayer(eID) > 0)
							bPresenceInArea = true;

						// Does another major civ have their capital here? (must be visible)
						for(int iMajorRivalLoop = 0; iMajorRivalLoop < MAX_MAJOR_CIVS; iMajorRivalLoop++)
						{
							PlayerTypes eMajorRivalLoop = (PlayerTypes) iMajorRivalLoop;
							if(eMajorRivalLoop == eID)
								continue;

							if(GET_PLAYER(eMajorRivalLoop).isAlive())
							{
								CvCity* pCapital = GET_PLAYER(eMajorRivalLoop).getCapitalCity();
								if(pCapital && pCapital->plot())
								{
									CvPlot* pPlot = pCapital->plot();
									if(pPlot->isVisible(GetPlayer()->getTeam()))
										iMajorCapitalsInArea++;
								}
							}
						}
					}
					else
					{
						CvAssertMsg(false, "Could not lookup minor civ's area! Please send Anton your save file and version.");
					}

					// How many units does the city-state have?
					int iMinorMilitaryUnits = 0;
					int iMinorUnits = 0;
					CvUnit* pLoopUnit;
					int iLoop;
					for(pLoopUnit = pMinor->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pMinor->nextUnit(&iLoop))
					{
						if(pLoopUnit->IsCanAttack() && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
						{
							iMinorMilitaryUnits++;
						}
						iMinorUnits++;
					}

					// Foreign continent
					if(!bPresenceInArea)
					{
						// Military foothold to attack other majors
						if(IsGoingForWorldConquest() && iMajorCapitalsInArea > 0)
						{
							iValue += 100; //antonjs: todo: xml
						}
						// Expansion
						else if(bExpandToOtherContinents)
						{
							iValue += 60; //antonjs: todo: xml
						}
						else
						{
							iValue += -50; //antonjs: todo: xml
						}
					}
					// Continent we have presence on
					else
					{
						// Proximity plays a large factor, since we don't want a remote, isolated city
						if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
						{
							iValue += 100; //antonjs: todo: xml
							// Military units could come to our rescue quickly
							if(GetStateAllWars() == STATE_ALL_WARS_LOSING)
							{
								if(iMinorMilitaryUnits > 0)  //antonjs: todo: xml
								{
									iValue += (iMinorMilitaryUnits) * 10; //antonjs: todo: xml
								}
								else
								{
									iValue -= 50; //antonjs: todo: xml
								}
							}
						}
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_CLOSE)
						{
							iValue += 10; //antonjs: todo: xml
						}
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_FAR)
						{
							iValue += -50; //antonjs: todo: xml
						}
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_DISTANT)
						{
							iValue += -100; //antonjs: todo: xml
						}
					}

					// Military units - How many, and can we support them?
					if(GetPlayer()->GetNumUnitsSupplied() >= GetPlayer()->getNumUnits() + iMinorUnits)
					{
						iValue += (iMinorMilitaryUnits) * 5;
					}

					// Happiness
					if(bNeedHappiness)
						iValue += -50; //antonjs: todo: xml
					if(bNeedHappinessCritical)
						iValue += -150; //antonjs: todo: xml

					// Potential bonuses lost
					MinorCivTraitTypes eTrait = pMinorCivAI->GetTrait();
					if(eTrait == MINOR_CIV_TRAIT_CULTURED && IsGoingForCultureVictory())
					{
						iValue += -70; //antonjs: todo: xml
					}
					else if(eTrait == MINOR_CIV_TRAIT_MERCANTILE)
					{
						if(bNeedHappiness)
							iValue += -100; //antonjs: todo: xml
						if(bNeedHappinessCritical)
							iValue += -150; //antonjs: todo: xml
					}

					// Time to decide - Do we want it enough?
					if(iValue > 100)  //antonjs: todo: xml
					{
						veMinorsToBuyout.push_back(eMinor, iValue);
						bWantsToBuyoutThisMinor = true;
					}
				}
			}

			// Calculate desirability to give this minor gold
			if(bWantsToMakeGoldGift && !bWantsToBuyoutThisMinor)
			{
				int iValue = /*100*/ GC.getMC_GIFT_WEIGHT_THRESHOLD();
				// If we're not protective or friendly, then don't bother with minor diplo
				if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE || eApproach == MINOR_CIV_APPROACH_FRIENDLY)
				{
					MinorGoldGiftInfo sGiftInfo;
					sGiftInfo.eMinor = eMinor;
					sGiftInfo.eMajorRival = NO_PLAYER;
					sGiftInfo.bQuickBoost = false;
					sGiftInfo.iGoldAmount = 0;

					// if we are rich we are more likely to, conversely if we are poor...
					iValue += min(max(0, m_pPlayer->calculateGoldRate() - 50),100);

					pMinorInfo = GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType());

					// Diplo victory makes us more likely to spend gold
					if(IsGoingForDiploVictory())
						iValue += /*100*/ GC.getMC_GIFT_WEIGHT_DIPLO_VICTORY();
					// double up if this is the home stretch
					if(GC.getGame().GetUnitedNationsCountdown() > 0)
					{
						iValue += /*100*/ GC.getMC_GIFT_WEIGHT_DIPLO_VICTORY();
					}
					// Going for Culture victory, focus on Cultural city states
					else if(IsGoingForCultureVictory())
					{
						if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_CULTURED)
							iValue += /*100*/ GC.getMC_GIFT_WEIGHT_CULTURE_VICTORY();
					}
					// Going for Conquest victory, focus on Militaristic city states
					else if(IsGoingForWorldConquest())
					{
						if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
							iValue += /*100*/ GC.getMC_GIFT_WEIGHT_CONQUEST_VICTORY();
					}

					//antonjs: todo: work extra gold quest INF potential into the friends/allies/passing logic as well
					// Gold gift quest is active, so we would get more bang for our bucks
					if(pMinorCivAI->IsActiveQuestForPlayer(eID, MINOR_CIV_QUEST_GIVE_GOLD))
					{
						iValue += 150; //antonjs: todo: constant/XML
					}

					// Invest quest is active, so we would get more bang for our bucks
					if(pMinorCivAI->IsActiveQuestForPlayer(eID, MINOR_CIV_QUEST_INVEST))
					{
						iValue += 100; //antonjs: todo: constant/XML
					}

					// having traits that give us bonuses also make us want to spend gold
					if(m_pPlayer->GetPlayerTraits()->GetCityStateFriendshipModifier() > 0 || m_pPlayer->GetPlayerTraits()->GetCityStateBonusModifier())
					{
						iValue += /*100*/ GC.getMC_GIFT_WEIGHT_DIPLO_VICTORY();
					}

					// Nearly everyone likes to grow
					if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_MARITIME && !GetPlayer()->IsEmpireUnhappy())
					{
						iValue += /*20*/ GC.getMC_GIFT_WEIGHT_MARITIME_GROWTH() * iGrowthFlavor * max(1, GetPlayer()->getNumCities() / 3);
					}

					// Slight negative weight towards militaristic
					if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_MILITARISTIC && !IsGoingForWorldConquest())
						iValue += /*-50*/ GC.getMC_GIFT_WEIGHT_MILITARISTIC();

					// If they have a resource we don't have, add extra weight
					int iResourcesWeLack = pMinorCivAI->GetNumResourcesMajorLacks(eID);
					if(iResourcesWeLack > 0)
						iValue += (iResourcesWeLack* /*80*/ GC.getMC_GIFT_WEIGHT_RESOURCE_WE_NEED());

					// If we're protective this is worth more than if we're friendly
					if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE)
						iValue += /*10*/ GC.getMC_GIFT_WEIGHT_PROTECTIVE();

					// If the minor is hostile, then reduce the weighting
					if(pMinorCivAI->GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
						iValue += /*-20*/ GC.getMC_GIFT_WEIGHT_HOSTILE();

					// The closer we are the better
					if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
						iValue += /*5*/ GC.getMC_GIFT_WEIGHT_NEIGHBORS();
					else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_CLOSE)
						iValue += /*4*/ GC.getMC_GIFT_WEIGHT_CLOSE();
					else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_FAR)
						iValue += /*3*/ GC.getMC_GIFT_WEIGHT_FAR();

					iLargeGiftFriendship = pMinorCivAI->GetFriendshipFromGoldGift(eID, iLargeGift);
					iMediumGiftFriendship = pMinorCivAI->GetFriendshipFromGoldGift(eID, iMediumGift);
					iSmallGiftFriendship = pMinorCivAI->GetFriendshipFromGoldGift(eID, iSmallGift);

					iFriendshipWithMinor = pMinorCivAI->GetEffectiveFriendshipWithMajor(eID);

					// Loop through other players to see if we can pass them
					for(iOtherMajorLoop = 0; iOtherMajorLoop < MAX_MAJOR_CIVS; iOtherMajorLoop++)
					{
						eOtherMajor = (PlayerTypes) iOtherMajorLoop;

						// Player must be alive
						if(!GET_PLAYER(eOtherMajor).isAlive())
							continue;

						iOtherPlayerFriendshipWithMinor = pMinorCivAI->GetEffectiveFriendshipWithMajor(eOtherMajor);

						// Player must have friendship with this major
						if(iOtherPlayerFriendshipWithMinor <= 0)
							continue;

						// They must have more friendship with this guy than us
						if(iFriendshipWithMinor <= iOtherPlayerFriendshipWithMinor)
							continue;

						// Only care if we'll actually be Allies or better
						bMediumGiftAllies = iFriendshipWithMinor + iMediumGiftFriendship >= pMinorCivAI->GetAlliesThreshold();
						bSmallGiftAllies = iFriendshipWithMinor + iSmallGiftFriendship >= pMinorCivAI->GetAlliesThreshold();

						// If we can pass them with a small gift, great
						if(bSmallGiftAllies && iOtherPlayerFriendshipWithMinor - iFriendshipWithMinor < iSmallGiftFriendship)
						{
							iValue += /*15*/ GC.getMC_SMALL_GIFT_WEIGHT_PASS_OTHER_PLAYER();
							sGiftInfo.bQuickBoost = true;
							sGiftInfo.eMajorRival = eOtherMajor;
						}
						// If a medium gift passes them up, that's good too
						else if(bMediumGiftAllies && iOtherPlayerFriendshipWithMinor - iFriendshipWithMinor < iMediumGiftFriendship)
						{
							iValue += /*10*/ GC.getMC_GIFT_WEIGHT_PASS_OTHER_PLAYER();
							sGiftInfo.eMajorRival = eOtherMajor;
						}
						// We're behind and we can't catch up right now, so zero-out the value
						else
							iValue = 0;
					}

					// Are we already allies?
					if(pMinorCivAI->IsAllies(eID))
					{
						// Are we close to losing our status?
						if(pMinorCivAI->IsCloseToNotBeingAllies(eID))
						{
							iValue += /*150*/ GC.getMC_GIFT_WEIGHT_ALMOST_NOT_ALLIES();
							sGiftInfo.bQuickBoost = true;
						}
						// Not going to lose status, so not worth going after this guy
						else
							iValue = 0;
					}
					// Are we already Friends?
					else if(pMinorCivAI->IsFriends(eID))
					{
						// Are we close to losing our status?
						if(pMinorCivAI->IsCloseToNotBeingFriends(eID))
						{
							iValue += /*125*/ GC.getMC_GIFT_WEIGHT_ALMOST_NOT_FRIENDS();
							sGiftInfo.bQuickBoost = true;
						}
						// Not going to lose status, so not worth going after this guy
						else if(!IsGoingForDiploVictory() || GC.getGame().GetUnitedNationsCountdown() == 0)
							iValue = 0;
					}

					// Did we bully you recently?  If so, giving you gold now would be very odd.
					if(pMinorCivAI->IsRecentlyBulliedByMajor(eID))
					{
						iValue -= 100; //antonjs: todo: constant/XML
					}

					//antonjs: consider: different behavior to CS that have been bullied by others, bullied by rival, etc.

					// Do we want it enough?
					if(iValue > GC.getMC_GIFT_WEIGHT_THRESHOLD())
					{
						veMinorsToGiveGold.push_back(sGiftInfo, iValue);
						bWantsToGiveGoldToThisMinor = true;
					}
				}
			}

			// Calculate desirability to bully a unit from this minor
			if(bWantsToBullyUnit && !bWantsToBuyoutThisMinor && !bWantsToGiveGoldToThisMinor)  //antonjs: todo: xml
			{
				int iValue = 100; //antonjs: todo: XML, bully threshold
				if(eApproach == MINOR_CIV_APPROACH_BULLY)
				{
					// Only bother if we can successfully bully
					if(pMinor->GetMinorCivAI()->CanMajorBullyUnit(eID))
					{
						// The closer we are the better, because the unit travels less distance to get home
						if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
							iValue += 50;
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_CLOSE)
							iValue += 30;
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_FAR)
							iValue += -30;
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_DISTANT)
							iValue += -50;

						//antonjs: consider: knock it down if is there a chance the worker will get captured by a nearby rival

						//antonjs: consider: if military unit, it would be a good thing to get it near a rival or ongoing war

						// If this minor has a PtP from someone, bullying it could have big consequences
						if(pMinor->GetMinorCivAI()->IsProtectedByAnyMajor())
						{
							iValue += -20;
							//antonjs: consider: scale based on which major is protecting it
						}
						else
						{
							iValue += 20;
						}

						//antonjs: consider: allies or friends with another major
						//antonjs: consider: distance to other majors

						// If we are getting a bonus, don't mess that up!
						if(pMinor->GetMinorCivAI()->IsAllies(eID) || pMinor->GetMinorCivAI()->IsFriends(eID))
						{
							iValue = 0;
						}

						// Do we want it enough?
						if(iValue > 100)  //antonjs: todo: XML for threshold
						{
							veMinorsToBullyUnit.push_back(eMinor, iValue);
							bWantsToBullyUnitFromThisMinor = true;
						}
					}
				}
			}

			// Calculate desirability to bully gold from this minor
			if(bWantsToBullyGold && !bWantsToBuyoutThisMinor && !bWantsToGiveGoldToThisMinor && !bWantsToBullyUnitFromThisMinor)
			{
				int iValue = 100; //antonjs: todo: XML, bully threshold
				if(eApproach == MINOR_CIV_APPROACH_BULLY)
				{
					// Only bother if we can successfully bully
					if(pMinor->GetMinorCivAI()->CanMajorBullyGold(eID))
					{
						// The closer we are the better
						if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_NEIGHBORS)
							iValue += 40;
						else if(GetPlayer()->GetProximityToPlayer(eMinor) == PLAYER_PROXIMITY_CLOSE)
							iValue += 20;

						// We like to keep bullying the same minor
						if(pMinor->GetMinorCivAI()->IsEverBulliedByMajor(eID))
						{
							iValue += 20;
						}

						// If we have not bullied this minor recently, but someone else has, it might be good to wait for an opportunity to gain a lot of INF
						if(!pMinor->GetMinorCivAI()->IsRecentlyBulliedByMajor(eID) && pMinor->GetMinorCivAI()->IsRecentlyBulliedByAnyMajor())
						{
							iValue += -10;
							//antonjs: consider: but if everyone near the minor has bullied it, then there is nobody to come to its rescue, so we can bully safely
						}

						// If this minor has a PtP from someone, bullying it could have big consequences
						if(pMinor->GetMinorCivAI()->IsProtectedByAnyMajor())
						{
							iValue += -10;
							//antonjs: consider: scale based on which major is protecting it
						}
						else
						{
							iValue += 10;
						}

						//antonjs: consider: allies or friends another major
						//antonjs: consider: distance to other majors

						// If we are getting a bonus, don't mess that up!
						if(pMinor->GetMinorCivAI()->IsAllies(eID) || pMinor->GetMinorCivAI()->IsFriends(eID))
						{
							iValue = 0;
						}

						// Do we want it enough?
						if(iValue > 100)  //antonjs: todo: XML for threshold
						{
							veMinorsToBullyGold.push_back(eMinor, iValue);
							bWantsToBullyGoldFromThisMinor = true;
						}
					}
				}
			}

		}

		SetWantToRouteConnectToMinor(eMinor, bWantsToConnect);
	}

	int iGoldReserve = GetPlayer()->GetTreasury()->GetGold();

	// Do we want to buyout a minor?
	if(veMinorsToBuyout.size() > 0)
	{
		veMinorsToBuyout.SortItems();
		int iGoldLeft = GetPlayer()->GetTreasury()->GetGold();
		PlayerTypes eLoopMinor = NO_PLAYER;
		for(int i = 0; i < veMinorsToBuyout.size(); i++)
		{
			eLoopMinor = veMinorsToBuyout.GetElement(i);
			int iBuyoutCost = GET_PLAYER(eLoopMinor).GetMinorCivAI()->GetBuyoutCost(eID);
			if(iGoldLeft >= iBuyoutCost)
			{
				if(GET_PLAYER(eLoopMinor).GetMinorCivAI()->CanMajorBuyout(eID))
				{
					GC.getGame().DoMinorBuyout(eID, eLoopMinor);
					iGoldLeft -= iBuyoutCost;
					break; // Don't buyout more than once in a single turn
				}
				else
				{
					CvAssertMsg(false, "Chose a minor to buyout that cannot actually be bought! Please send Anton your save file and version.");
				}
			}
			else
			{
				if(!GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT))
				{
					LogMinorCivBuyout(eLoopMinor, iBuyoutCost, /*bSaving*/ true);

					int iArbitraryPriority = 100;	// Note: this relates to the plot buy score in CvEconomicAI::DoPlotPurchases
					GetPlayer()->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT, iBuyoutCost, iArbitraryPriority);
				}
			}
		}
	}

	// Do we want to give someone Gold enough to actually do it?
	if(veMinorsToGiveGold.size() > 0)
	{
		veMinorsToGiveGold.SortItems(); // Sort from highest desirability to lowest
		for(int i = 0; i < veMinorsToGiveGold.size(); i++)
		{
			int iGoldLeft = GetPlayer()->GetTreasury()->GetGold();
			MinorGoldGiftInfo sGift = veMinorsToGiveGold.GetElement(i);
			sGift.iGoldAmount = 0;

			if(iGoldLeft >= iSmallGift && sGift.bQuickBoost)
				sGift.iGoldAmount = iSmallGift;
			else if(iGoldLeft >= iLargeGift)
				sGift.iGoldAmount = iLargeGift;
			else if(iGoldLeft >= iMediumGift)
				sGift.iGoldAmount = iMediumGift;

			int iOldFriendship = GET_PLAYER(sGift.eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(eID);

			// Able to give a gift?  Don't gift more than half of the gold we have in one turn
			if(sGift.iGoldAmount > 0 && iGoldLeft >= (iGoldReserve / 2))
			{
				GET_PLAYER(sGift.eMinor).GetMinorCivAI()->DoGoldGiftFromMajor(GetPlayer()->GetID(), sGift.iGoldAmount); //antonjs: todo: go through CvGame instead?

				LogMinorCivGiftGold(sGift.eMinor, iOldFriendship, sGift.iGoldAmount, /*bSaving*/ false, sGift.bQuickBoost, sGift.eMajorRival);

				if(GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT))
					GetPlayer()->GetEconomicAI()->CancelSaveForPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT);
			}
			// Can't afford gift yet, so start saving
			else
			{
				if(!GetPlayer()->GetEconomicAI()->IsSavingForThisPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT))
				{
					int iAmountToSaveFor = iMediumGift;

					if(sGift.bQuickBoost)
						iAmountToSaveFor = iSmallGift;

					LogMinorCivGiftGold(sGift.eMinor, iOldFriendship, iAmountToSaveFor, /*bSaving*/ true, sGift.bQuickBoost, sGift.eMajorRival);

					int iArbitraryPriority = 100;	// Note: this relates to the plot buy score in CvEconomicAI::DoPlotPurchases
					GetPlayer()->GetEconomicAI()->StartSaveForPurchase(PURCHASE_TYPE_MINOR_CIV_GIFT, iAmountToSaveFor, iArbitraryPriority);
				}
			}
		}
	}

	// Do we want a unit enough to bully someone?
	if(veMinorsToBullyUnit.size() > 0)
	{
		veMinorsToBullyUnit.SortItems();
		PlayerTypes eLoopMinor = NO_PLAYER;
		for(int i = 0; i < veMinorsToBullyUnit.size(); i++)
		{
			eLoopMinor = veMinorsToBullyUnit.GetElement(i);
			CvAssertMsg(eLoopMinor != NO_PLAYER, "Trying to bully a unit from NO_PLAYER! Please send Anton your save file and version.");
			if(GET_PLAYER(eLoopMinor).GetMinorCivAI()->CanMajorBullyUnit(eID))
			{
				GC.getGame().DoMinorBullyUnit(eID, eLoopMinor);
				break; // Don't bully a unit more than once in a single turn
			}
			else
			{
				CvAssertMsg(false, "Chose a minor to bully unit from that cannot actually be bullied! Please send Anton your save file and version.");
			}
		}
	}

	// Do we want gold enough to bully someone?
	if(veMinorsToBullyGold.size() > 0)
	{
		veMinorsToBullyGold.SortItems();
		PlayerTypes eLoopMinor = NO_PLAYER;
		for(int i = 0; i < veMinorsToBullyGold.size(); i++)
		{
			eLoopMinor = veMinorsToBullyGold.GetElement(i);
			CvAssertMsg(eLoopMinor != NO_PLAYER, "Trying to bully gold from NO_PLAYER! Please send Anton your save file and version.");
			if(GET_PLAYER(eLoopMinor).GetMinorCivAI()->CanMajorBullyGold(eID))
			{
				GC.getGame().DoMinorBullyGold(eID, eLoopMinor);
			}
			else
			{
				CvAssertMsg(false, "Chose a minor to bully gold from that cannot actually be bullied! Please send Anton your save file and version.");
			}
		}
	}
}

void CvDiplomacyAI::DoUpdateMinorCivProtection(PlayerTypes eMinor, MinorCivApproachTypes eApproach)
{
	// Only change protection if this player is not human controlled!
	if(!GetPlayer()->isHuman())
	{
		if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE)
		{
			// We are protective, so do a PtP if we are able to and haven't already
			if(GET_PLAYER(eMinor).GetMinorCivAI()->CanMajorStartProtection(GetPlayer()->GetID()))
			{
				GC.getGame().DoMinorPledgeProtection(GetPlayer()->GetID(), eMinor, true);
				DoMakePublicDeclaration(PUBLIC_DECLARATION_PROTECT_MINOR, eMinor, -1, eMinor);
			}
		}
		else
		{
			// We are not protective, so revoke PtP if we can
			if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()) && GET_PLAYER(eMinor).GetMinorCivAI()->CanMajorWithdrawProtection(GetPlayer()->GetID()))
			{
				GC.getGame().DoMinorPledgeProtection(GetPlayer()->GetID(), eMinor, false);
				DoMakePublicDeclaration(PUBLIC_DECLARATION_ABANDON_MINOR, eMinor, -1, eMinor);
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoCoopWarTimeStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		// Don't send this to AI players - coop war timer is automatically handled in DoCounters()
		if(!GET_PLAYER(ePlayer).isHuman())
			return;

		CvTeam* pTeam = &GET_TEAM(GET_PLAYER(ePlayer).getTeam());

		PlayerTypes eTargetPlayer;
		TeamTypes eTargetTeam;

		for(int iTargetLoop = 0; iTargetLoop < MAX_MAJOR_CIVS; iTargetLoop++)
		{
			eTargetPlayer = (PlayerTypes) iTargetLoop;

			// Agreed to go to war soon... what's the counter at?
			if(GetCoopWarAcceptedState(ePlayer, eTargetPlayer) == COOP_WAR_STATE_SOON)
			{
				if(GetCoopWarCounter(ePlayer, eTargetPlayer) == /*10*/ GC.getCOOP_WAR_SOON_COUNTER())
				{
					eTargetTeam = GET_PLAYER(eTargetPlayer).getTeam();

					// If we're already at war, don't bother
					if(!pTeam->isAtWar(eTargetTeam))
					{
						eStatement = DIPLO_STATEMENT_COOP_WAR_TIME;
						iData1 = eTargetPlayer;

						// Don't evaluate other players
						break;
					}
					// Human is already at war - process what we would have if he'd agreed at this point
					else
					{
						SetCoopWarAcceptedState(ePlayer, eTargetPlayer, COOP_WAR_STATE_ACCEPTED);

						// AI declaration
						if(!IsAtWar(eTargetPlayer))
						{
							DeclareWar(eTargetPlayer);
							GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eTargetPlayer, 1);
						}
					}
				}
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoCoopWarStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		// slewis - added so that a player already at war wouldn't try to start another war with another player. The AI should try to only have one war going at a time if possible.
		if (GET_TEAM(GetTeam()).getAtWarCount(true) == 0)
		{
			PlayerTypes eTargetPlayer;
			if(DoTestCoopWarDesire(ePlayer, /*passed by address*/ eTargetPlayer))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_COOP_WAR_REQUEST;
				int iTurnsBetweenStatements = 10;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					bool bSendStatement = true;

					//// 1 in 2 chance we don't actually send the message (don't want full predictability)
					//if (50 < GC.getGame().getJonRandNum(100, "Diplomacy AI: rand roll to see if we ask to work with a player"))
					//	bSendStatement = false;

					if(bSendStatement)
					{
						eStatement = eTempStatement;
						iData1 = eTargetPlayer;
					}

					// Add this statement to the log so we don't evaluate it again until time has passed
					else
						DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
				}
			}	
		}
	}
}

/// Possible Contact Statement - Demand
void CvDiplomacyAI::DoMakeDemand(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetDemandTargetPlayer() == ePlayer)
		{
			if(IsDemandReady())
			{
				if(GetPlayer()->GetDealAI()->IsMakeDemand(ePlayer, /*pDeal can be modified in this function*/ pDeal))
				{
					DiploStatementTypes eTempStatement = DIPLO_STATEMENT_DEMAND;
					int iTurnsBetweenStatements = 40;

					if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
						eStatement = eTempStatement;
				}
				else
				{
					// Clear out the deal if we don't want to offer it so that it's not tainted for the next trade possibility we look at
					pDeal->ClearItems();
				}
			}
		}
	}
}

/// Possible Contact Statement - guy has his military positioned aggressively near us
void CvDiplomacyAI::DoAggressiveMilitaryStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		bool bSendStatement = false;

		// They must be able to declare war on us
		if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canDeclareWar(GetPlayer()->getTeam()))
			return;

		// They're HIGH this turn and weren't last turn
		if(GetMilitaryAggressivePosture(ePlayer) >= AGGRESSIVE_POSTURE_HIGH && GetLastTurnMilitaryAggressivePosture(ePlayer) < AGGRESSIVE_POSTURE_HIGH)
			bSendStatement = true;

		// They're MEDIUM this turn and were NONE last turn
		else if(GetMilitaryAggressivePosture(ePlayer) >= AGGRESSIVE_POSTURE_MEDIUM && GetLastTurnMilitaryAggressivePosture(ePlayer) <= AGGRESSIVE_POSTURE_NONE)
			bSendStatement = true;

		// We're working together, so don't worry about it
		if(IsDoFAccepted(ePlayer))
			return;

		// Check other player status
		PlayerTypes eThirdParty;
		for(int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
		{
			eThirdParty = (PlayerTypes) iThirdPartyLoop;

			// Are we at war with the same player?
			if(IsAtWar(eThirdParty) && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(eThirdParty).getTeam()))
				return;

			// Are they at war with anyone we're neighbors with?
			if(GetPlayer()->GetProximityToPlayer(eThirdParty) == PLAYER_PROXIMITY_NEIGHBORS && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(eThirdParty).getTeam()))
				return;
		}

		if(bSendStatement)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_AGGRESSIVE_MILITARY_WARNING;
			int iTurnsBetweenStatements = 40;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoKilledCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsPlayerMadeAttackCityStatePromise(ePlayer))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_KILLED_PROTECTED_CITY_STATE;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
			{
				PlayerTypes eMinorCiv = GetOtherPlayerProtectedMinorKilled(ePlayer);
				if(eMinorCiv != NO_PLAYER)
				{
					CvAssert(eMinorCiv >= MAX_MAJOR_CIVS);
					CvAssert(eMinorCiv < MAX_CIV_PLAYERS);

					eStatement = eTempStatement;

					iData1 = eMinorCiv;
				}
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoAttackedCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Attacked a City State we're protective towards
	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetTurnsSincePlayerAttackedProtectedMinor(ePlayer) == 0)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_ATTACKED_PROTECTED_CITY_STATE;
			int iTurnsBetweenStatements = 1;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
			{
				PlayerTypes eMinorCiv = GetOtherPlayerProtectedMinorAttacked(ePlayer);
				if(eMinorCiv != NO_PLAYER)
				{
					CvAssert(eMinorCiv >= MAX_MAJOR_CIVS);
					CvAssert(eMinorCiv < MAX_CIV_PLAYERS);
					// Minor civ must still be alive!
					if(GET_PLAYER(eMinorCiv).isAlive())
					{	
						eStatement = eTempStatement;
						iData1 = eMinorCiv;
					}
				}
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoBulliedCityStateStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		// Bullied a City State we're protective towards
		if(GetTurnsSincePlayerBulliedProtectedMinor(ePlayer) == 0)
		{
			// Have we asked you to make a promise before?
			if(IsPlayerBrokenBullyCityStatePromise(ePlayer) ||
			        IsPlayerIgnoredBullyCityStatePromise(ePlayer))
			{
				// We don't even want to bother with you again, so do nothing
			}
			else if(IsPlayerMadeBullyCityStatePromise(ePlayer))
			{
				// You broke the promise you made!
				SetPlayerBrokenBullyCityStatePromise(ePlayer, true);
				SetPlayerMadeBullyCityStatePromise(ePlayer, false);
			}
			// Otherwise, ask you to make a promise
			else
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_BULLIED_PROTECTED_CITY_STATE;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					PlayerTypes eMinorCiv = GetOtherPlayerProtectedMinorBullied(ePlayer);
					if(eMinorCiv != NO_PLAYER)
					{
						CvAssert(eMinorCiv >= MAX_MAJOR_CIVS);
						CvAssert(eMinorCiv < MAX_CIV_PLAYERS);
						// Minor civ must still be alive!
						if(GET_PLAYER(eMinorCiv).isAlive())
						{
							eStatement = eTempStatement;
							iData1 = eMinorCiv;
						}
					}
				}
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoExpansionWarningStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Comment on aggressive expansion
	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		bool bSendStatement = false;
		if (GET_PLAYER(ePlayer).GetTurnsSinceSettledLastCity() < GC.getEXPANSION_BICKER_TIMEOUT() && !EverMadeExpansionPromise(ePlayer) && !IsPlayerMadeExpansionPromise(ePlayer) && !IsPlayerIgnoredExpansionPromise(ePlayer) && !IsPlayerBrokenExpansionPromise(ePlayer))
		{
			// We're fiercely opposed to their expansion
			if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_FIERCE)
				bSendStatement = true;

			// Have a strong dispute over land now, and didn't last turn
			else if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG && GetLastTurnLandDisputeLevel(ePlayer) < DISPUTE_LEVEL_STRONG)
			{
				if(GetExpansionAggressivePosture(ePlayer) >= AGGRESSIVE_POSTURE_MEDIUM)
					bSendStatement = true;
			}
		}

		if(bSendStatement)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_EXPANSION_WARNING;
			int iTurnsBetweenStatements = (GC.getEXPANSION_PROMISE_TURNS_EFFECTIVE() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent()) / 100;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoExpansionBrokenPromiseStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Tell the player he broke a promise
	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsPlayerBrokenExpansionPromise(ePlayer))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_EXPANSION_BROKEN_PROMISE;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoPlotBuyingWarningStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Comment on aggressive Plot Buying
	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		bool bSendStatement = false;
		if (!EverMadeBorderPromise(ePlayer) && !IsPlayerMadeBorderPromise(ePlayer) && !IsPlayerBrokenBorderPromise(ePlayer) && !IsPlayerIgnoredBorderPromise(ePlayer))
		{
			if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
			{
				// We've spotten them buying up Plots
				if(GetPlotBuyingAggressivePosture(ePlayer) >= AGGRESSIVE_POSTURE_LOW)
					bSendStatement = true;
			}
		}

		if(bSendStatement)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_PLOT_BUYING_WARNING;
			int iTurnsBetweenStatements = (GC.getBORDER_PROMISE_TURNS_EFFECTIVE() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent()) / 100;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}

	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoPlotBuyingBrokenPromiseStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Tell the player he broke a promise
	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsPlayerBrokenBorderPromise(ePlayer))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_PLOT_BUYING_BROKEN_PROMISE;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}

/// Possible Contact Statement - Have we attacked a minor that is protected by someone?
void CvDiplomacyAI::DoWeAttackedYourMinorStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinorCivLoop;
			if(GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(ePlayer))
			{
				if(IsAtWar(eMinor))
				{
					// Did we deal them damage last turn?
					if(GetOtherPlayerWarValueLost(eMinor, GetPlayer()->GetID()) > GetOtherPlayerLastRoundWarValueLost(eMinor, GetPlayer()->GetID()))
					{
						// Has this message not yet been sent during this war?
						if(!HasSentAttackProtectedMinorTaunt(ePlayer, eMinor))
						{
							DiploStatementTypes eTempStatement = DIPLO_STATEMENT_WE_ATTACKED_YOUR_MINOR;
							int iTurnsBetweenStatements = 1;
							if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
							{
								CvAssert(eMinor >= MAX_MAJOR_CIVS);
								CvAssert(eMinor < MAX_CIV_PLAYERS);

								eStatement = eTempStatement;
								iData1 = eMinor;
							}
						}
					}
				}
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoWeBulliedYourMinorStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		PlayerTypes eMinor;
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			eMinor = (PlayerTypes) iMinorCivLoop;

			// Minor must be alive
			if(!GET_PLAYER(eMinor).isAlive())
				continue;

			// Did we bully this minor last turn?
			if(GET_PLAYER(eMinor).GetMinorCivAI()->IsEverBulliedByMajor(GetPlayer()->GetID()))
			{
				if(GET_PLAYER(eMinor).GetMinorCivAI()->GetTurnLastBulliedByMajor(GetPlayer()->GetID()) == (GC.getGame().getGameTurn() - 1))
				{
					// Is this minor protected by this player?
					if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(ePlayer))
					{
						DiploStatementTypes eTempStatement = DIPLO_STATEMENT_WE_BULLIED_YOUR_MINOR;
						int iTurnsBetweenStatements = 1;

						if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
						{
							CvAssert(eMinor >= MAX_MAJOR_CIVS);
							CvAssert(eMinor < MAX_CIV_PLAYERS);

							eStatement = eTempStatement;
							iData1 = eMinor;
						}
					}
				}
			}
		}
	}
}

void CvDiplomacyAI::DoCaughtYourSpyStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(m_pPlayer->GetEspionageAI()->m_aiTurnLastSpyCaught[ePlayer] == GC.getGame().getGameTurn())
		{
			// Have we asked you to make a promise before?
			if(IsPlayerBrokenSpyPromise(ePlayer) || IsPlayerIgnoredSpyPromise(ePlayer))
			{
				// We don't even want to bother with you again, so do nothing
			}
			else if(IsPlayerMadeSpyPromise(ePlayer))
			{
				// You broke the promise you made!
				SetPlayerBrokenSpyPromise(ePlayer, true);
				SetPlayerMadeSpyPromise(ePlayer, false);
			}
			// Otherwise, ask you to make a promise
			else
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_CAUGHT_YOUR_SPY;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}

void CvDiplomacyAI::DoKilledYourSpyStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(m_pPlayer->GetEspionageAI()->m_aiNumSpiesKilled[ePlayer] > 0)
		{
			if(m_pPlayer->GetEspionageAI()->m_aiTurnLastSpyKilled[ePlayer] == GC.getGame().getGameTurn())
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_KILLED_YOUR_SPY;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}

void CvDiplomacyAI::DoKilledMySpyStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(m_pPlayer->GetEspionageAI()->m_aiTurnLastSpyDied[ePlayer] == GC.getGame().getGameTurn() - 1)
		{
			// Have we asked you to make a promise before?
			if(IsPlayerBrokenSpyPromise(ePlayer) || IsPlayerIgnoredSpyPromise(ePlayer))
			{
				// We don't even want to bother with you again, so do nothing
			}
			else if(IsPlayerMadeSpyPromise(ePlayer))
			{
				// You broke the promise you made!
				SetPlayerBrokenSpyPromise(ePlayer, true);
				SetPlayerMadeSpyPromise(ePlayer, false);
			}
			// Otherwise, ask you to make a promise
			else
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_KILLED_MY_SPY;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}

void CvDiplomacyAI::DoShareIntrigueStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		IntrigueNotificationMessage* pNotificationMessage = m_pPlayer->GetEspionage()->GetRecentIntrigueInfo(ePlayer);
		if (pNotificationMessage)
		{
			// if this player has an untold plot against a player
			if(pNotificationMessage->m_eSourcePlayer != NO_PLAYER)
			{
				bool bIsNewIntrigue = true;
				// has any other player told the player about this plot?
				for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
				{
					PlayerTypes eOtherPlayer = (PlayerTypes)ui;
					// don't evalute the plotting player
					if(eOtherPlayer == pNotificationMessage->m_eSourcePlayer)
					{
						continue;
					}

					if(GET_PLAYER(eOtherPlayer).GetEspionage()->HasSharedIntrigue(ePlayer, pNotificationMessage->m_eSourcePlayer, (CvIntrigueType)(pNotificationMessage->m_iIntrigueType)))
					{
						bIsNewIntrigue = false;
						break;
					}
				}

				if(bIsNewIntrigue)
				{
					MajorCivApproachTypes eApproachType = m_pPlayer->GetDiplomacyAI()->GetMajorCivApproach(ePlayer, false);
					if (eApproachType != MAJOR_CIV_APPROACH_GUARDED && eApproachType != MAJOR_CIV_APPROACH_HOSTILE && eApproachType != MAJOR_CIV_APPROACH_WAR)
					{
						DiploStatementTypes eTempStatement = DIPLO_STATEMENT_SHARE_INTRIGUE;
						int iTurnsBetweenStatements = 1;
						if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) > iTurnsBetweenStatements)
						{
							eStatement = eTempStatement;
						}
					}
				}
				else
				{
					// mark this as shared so it doesn't try to interrupt the player
					m_pPlayer->GetEspionage()->MarkRecentIntrigueAsShared(ePlayer, pNotificationMessage->m_eSourcePlayer, (CvIntrigueType)(pNotificationMessage->m_iIntrigueType));
				}
			}
		}
	}
}

void CvDiplomacyAI::DoConvertedMyCityStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetNegativeReligiousConversionPoints(ePlayer) >= GC.getRELIGION_DIPLO_HIT_THRESHOLD())
		{
			// Have we asked you to make a promise before?
			if(IsPlayerBrokenNoConvertPromise(ePlayer) ||
			        IsPlayerIgnoredNoConvertPromise(ePlayer))
			{
				// We don't even want to bother with you again, so do nothing
			}
			else if(IsPlayerMadeNoConvertPromise(ePlayer))
			{
				// You broke the promise you made!
				SetPlayerBrokenNoConvertPromise(ePlayer, true);
				SetPlayerMadeNoConvertPromise(ePlayer, false);
			}
			// Otherwise, ask you to make a promise
			else
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_STOP_CONVERSIONS;
				int iTurnsBetweenStatements = 1;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoDoFStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		// Have we already made the agreement?
		if(!IsDoFAccepted(ePlayer))
		{
			if(IsDoFAcceptable(ePlayer))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_WORK_WITH_US;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 60 &&
				        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_WORK_WITH_US_RANDFAILED) >= 10)
				{
					bool bSendStatement = true;

					// Chance we don't actually send the message (don't want full predictability)
					//if (GC.getGame().getJonRandNum(100, "Diplomacy AI: rand roll to see if we ask to work with a player") < 50)
					//	bSendStatement = false;

					if(bSendStatement)
						eStatement = eTempStatement;

					// Add this statement to the log so we don't evaluate it again until 10 turns has come back around
					else
						DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_WORK_WITH_US_RANDFAILED);
				}
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoDenounceFriendStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		// Must have already made the agreement
		if(IsDoFAccepted(ePlayer))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_DENOUNCE_FRIEND;
			int iTurnsBetweenStatements = 1;

			// Done working with this guy?
			if(IsDenounceFriendAcceptable(ePlayer))
			{
				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}

			// Add this statement to the log so we don't evaluate it again until 20 turns has come back around
			//else
			//	DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsDenounceAcceptable(ePlayer, /*bBias*/ false))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_DENOUNCE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 60 &&
			        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_DENOUNCE_RANDFAILED) >= 10)
			{
				bool bSendStatement = true;

				// 1 in 2 chance we don't actually send the message (don't want full predictability)
				//if (50 < GC.getGame().getJonRandNum(100, "Diplomacy AI: rand roll to see if we ask to work with a player"))
				//	bSendStatement = false;

				if(bSendStatement)
				{
					eStatement = eTempStatement;
				}

				// Add this statement to the log so we don't evaluate it again until time has passed
				else
					DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_DENOUNCE_RANDFAILED);
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoRequestFriendDenounceStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		bool bRandFailed;

		PlayerTypes eTarget = GetRequestFriendToDenounce(ePlayer, bRandFailed);
		if(eTarget != NO_PLAYER)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
			        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE_RANDFAILED) >= 10)
			{
				if(!bRandFailed)
				{
					eStatement = eTempStatement;
					iData1 = eTarget;
				}
				// Add this statement to the log so we don't evaluate it again until time has passed
				else
				{
					DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_DENOUNCE_RANDFAILED);
				}
			}
		}
	}
}

/// Possible Contact Statement
//void CvDiplomacyAI::DoWorkAgainstSomeoneStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1)
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//
//	if (eStatement == NO_DIPLO_STATEMENT_TYPE)
//	{
//		PlayerTypes eTargetPlayer;
//
//		if (DoTestWorkingAgainstPlayersDesire(ePlayer, eTargetPlayer))
//		{
//			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_DENOUNCE;
//
//			if (GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 60 &&
//				GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_DENOUNCE_RANDFAILED) >= 10)
//			{
//				bool bSendStatement = true;
//
//				// 1 in 2 chance we don't actually send the message (don't want full predictability)
//				//if (50 < GC.getGame().getJonRandNum(100, "Diplomacy AI: rand roll to see if we ask to work with a player"))
//				//	bSendStatement = false;
//
//				if (bSendStatement)
//				{
//					eStatement = eTempStatement;
//					iData1 = eTargetPlayer;
//				}
//
//				// Add this statement to the log so we don't evaluate it again until time has passed
//				else
//					DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_DENOUNCE_RANDFAILED);
//			}
//		}
//	}
//}

/// Possible Contact Statement
//void CvDiplomacyAI::DoEndWorkAgainstSomeoneStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement, int &iData1)
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//
//	if (eStatement == NO_DIPLO_STATEMENT_TYPE)
//	{
//		PlayerTypes eTargetPlayer;
//
//		if (!DoTestContinueWorkingAgainstPlayersDesire(ePlayer, eTargetPlayer))
//		{
//			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_END_WORK_AGAINST_SOMEONE;
//			int iTurnsBetweenStatements = 1;
//
//			if (GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
//			{
//				eStatement = eTempStatement;
//				iData1 = eTargetPlayer;
//			}
//
//			// Add this statement to the log so we don't evaluate it again until time has passed
//			//else
//			//	DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
//		}
//	}
//}

/// Possible Contact Statement - Luxury Trade
void CvDiplomacyAI::DoLuxuryTrade(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetPlayer()->GetDealAI()->IsMakeOfferForLuxuryResource(ePlayer, /*pDeal can be modified in this function*/ pDeal))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_LUXURY_TRADE;
			int iTurnsBetweenStatements = 20;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
		else
		{
			// Clear out the deal if we don't want to offer it so that it's not tainted for the next trade possibility we look at
			pDeal->ClearItems();
		}
	}
}

/// Possibile Contact Statement - Embassy Exchange
void CvDiplomacyAI::DoEmbassyExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		// Can both sides open an embassy
		if(pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), ePlayer, TRADE_ITEM_ALLOW_EMBASSY) &&
		        pDeal->IsPossibleToTradeItem(ePlayer, GetPlayer()->GetID(), TRADE_ITEM_ALLOW_EMBASSY))
		{
			// Does this guy want to exchange embassies?
			if(IsEmbassyExchangeAcceptable(ePlayer))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_EMBASSY_EXCHANGE;
				int iTurnsBetweenStatements = 20;

				if ((GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements) && GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_EMBASSY_OFFER) >= 10)
				{
					bool bSendStatement = false;

					// AI
					if(!GET_PLAYER(ePlayer).isHuman())
					{
						if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsEmbassyExchangeAcceptable(GetPlayer()->GetID()))
							bSendStatement = true;
					}
					// Human
					else
						bSendStatement = true;

					// 1 in 2 chance we don't actually send the message (don't want full predictability)
					if(50 < GC.getGame().getJonRandNum(100, "Diplomacy AI: rand rool to see if we ask to exchange embassies"))
						bSendStatement = false;

					if(bSendStatement)
					{
						pDeal->AddAllowEmbassy(GetPlayer()->GetID());
						pDeal->AddAllowEmbassy(ePlayer);

						eStatement = eTempStatement;
					}
					else
						DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
				}
			}
		}
	}
}

/// Possible Contact Statement - Embassy
void CvDiplomacyAI::DoEmbassyOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetPlayer()->GetDealAI()->MakeOfferForEmbassy(ePlayer, /*pDeal can be modified in this function*/ pDeal))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_EMBASSY_OFFER;
			int iTurnsBetweenStatements = 20;

			if ((GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements) && GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_EMBASSY_EXCHANGE) >= 10)
				eStatement = eTempStatement;
		}
		else
		{
			// Clear out the deal if we don't want to offer it so that it's not tainted for the next trade possibility we look at
			pDeal->ClearItems();
		}
	}
}

/// Possible Contact Statement - Open Borders Exchange
void CvDiplomacyAI::DoOpenBordersExchange(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		int iDuration = GC.getGame().GetDealDuration();

		// Can both sides trade OB?
		if(pDeal->IsPossibleToTradeItem(GetPlayer()->GetID(), ePlayer, TRADE_ITEM_OPEN_BORDERS, iDuration) &&
		        pDeal->IsPossibleToTradeItem(ePlayer, GetPlayer()->GetID(), TRADE_ITEM_OPEN_BORDERS, iDuration))
		{
			// Does this guy want to exchange OB?
			if(IsOpenBordersExchangeAcceptable(ePlayer))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_OPEN_BORDERS_EXCHANGE;
				int iTurnsBetweenStatements = 20;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				{
					bool bSendStatement = false;

					// AI
					if(!GET_PLAYER(ePlayer).isHuman())
					{
						if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsOpenBordersExchangeAcceptable(GetPlayer()->GetID()))
							bSendStatement = true;
					}
					// Human
					else
						bSendStatement = true;

					// 1 in 2 chance we don't actually send the message (don't want full predictability)
					if(50 < GC.getGame().getJonRandNum(100, "Diplomacy AI: rand roll to see if we ask to exchange open borders"))
						bSendStatement = false;

					if(bSendStatement)
					{
						// OB on each side
						pDeal->AddOpenBorders(GetPlayer()->GetID(), iDuration);
						pDeal->AddOpenBorders(ePlayer, iDuration);

						eStatement = eTempStatement;
					}
					// Add this statement to the log so we don't evaluate it again until 20 turns has come back around
					else
						DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
				}
			}
		}
	}
}

/// Possible Contact Statement - Open Borders
void CvDiplomacyAI::DoOpenBordersOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetPlayer()->GetDealAI()->IsMakeOfferForOpenBorders(ePlayer, /*pDeal can be modified in this function*/ pDeal))
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_OPEN_BORDERS_OFFER;
			int iTurnsBetweenStatements = 20;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
		else
		{
			// Clear out the deal if we don't want to offer it so that it's not tainted for the next trade possibility we look at
			pDeal->ClearItems();
		}
	}
}

/// Possible Contact Statement - Research Agreement Offer
void CvDiplomacyAI::DoResearchAgreementOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(IsCanMakeResearchAgreementRightNow(ePlayer))
		{
			if(GetPlayer()->GetDealAI()->IsMakeOfferForResearchAgreement(ePlayer, /*pDeal can be modified in this function*/ pDeal))
			{
				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_RESEARCH_AGREEMENT_OFFER;
				int iTurnsBetweenStatements = 20;

				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}
			else
			{
				// Clear out the deal if we don't want to offer it so that it's not tainted for the next trade possibility we look at
				pDeal->ClearItems();
			}
		}
	}
}

// Possible Contact Statement - Renew Recently Expired Deal
void CvDiplomacyAI::DoRenewExpiredDeal(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if (GetGlobalCoopWarAcceptedState(ePlayer) >= COOP_WAR_STATE_SOON)
		{
			return;
		}

		CvDeal* pTargetDeal = NULL;
		CvGameDeals* pGameDeals = GC.getGame().GetGameDeals();

		for(int iDealTypes = 0; iDealTypes < 2; iDealTypes++)
		{
			int iNumDeals = 0;
			if(iDealTypes == 0)
			{
				iNumDeals = pGameDeals->GetNumHistoricDeals(ePlayer);
			}
			else
			{
				iNumDeals = pGameDeals->GetNumCurrentDeals(ePlayer);
			}

			for(int iDeal = 0; iDeal < iNumDeals; iDeal++)
			{
				CvDeal* pCurrentDeal = NULL;
				if(iDealTypes == 0)
				{
					pCurrentDeal = pGameDeals->GetHistoricDeal(ePlayer, iDeal);
				}
				else
				{
					pCurrentDeal = pGameDeals->GetCurrentDeal(ePlayer, iDeal);
				}

				// if this deal has already been renewed or cancelled, then move along
				if(pCurrentDeal->m_bConsideringForRenewal || pCurrentDeal->m_bDealCancelled || pCurrentDeal->m_bCheckedForRenewal)
				{
					continue;
				}

				// if they don't involve the player, bail
				if(!(pCurrentDeal->m_eFromPlayer == m_pPlayer->GetID() || pCurrentDeal->m_eToPlayer == m_pPlayer->GetID()))
				{
					continue;
				}

				// if the deal can be renewed (no peace treaties, etc)
				if(!pCurrentDeal->IsPotentiallyRenewable())
				{
					continue;
				}

				// Check to see if the deal is still active
				if(pCurrentDeal->m_iFinalTurn > GC.getGame().getElapsedGameTurns())
				{
					continue;
				}

				bool bCanTradeItems = true;

				// if the deal can be fully renewed
				TradedItemList::iterator it;
				CvDeal kTempDeal;
				// only evaluate renewed deals if we're looking at current deals
				if (iDealTypes == 1) 
				{
					pCurrentDeal->m_bConsideringForRenewal = true;
				}
				for(it = pCurrentDeal->m_TradedItems.begin(); it != pCurrentDeal->m_TradedItems.end(); ++it)
				{
					PlayerTypes eOtherPlayer;
					if (it->m_eFromPlayer == pCurrentDeal->m_eFromPlayer)
					{
						eOtherPlayer = pCurrentDeal->m_eToPlayer;
					}
					else
					{
						eOtherPlayer = pCurrentDeal->m_eFromPlayer;
					}

					if(!kTempDeal.IsPossibleToTradeItem(it->m_eFromPlayer, eOtherPlayer, it->m_eItemType, it->m_iData1, it->m_iData2))
					{
						bCanTradeItems = false;
						break;
					}
				}
				if (iDealTypes == 1)
				{
					pCurrentDeal->m_bConsideringForRenewal = false;
				}

				if(!bCanTradeItems)
				{
					continue;
				}

				pTargetDeal = pCurrentDeal;
				break;
			}

			if (pTargetDeal)
			{
				break;
			}
		}

		if (pTargetDeal)
		{
			// copy the target deal into the new deal
			*pDeal = *pTargetDeal;
			pTargetDeal->m_bConsideringForRenewal = true;
			pDeal->m_iFinalTurn = -1;

			if(!GET_PLAYER(ePlayer).isHuman())
			{
				bool bAbleToEqualize = m_pPlayer->GetDealAI()->DoEqualizeDealWithAI(pDeal, ePlayer);
				if(!bAbleToEqualize)
				{
					pDeal->ClearItems();
					ClearDealToRenew();
					return;
				}
			}

			eStatement = DIPLO_STATEMENT_RENEW_DEAL;
		}
		else
		{
			pDeal->ClearItems();
		}
	}
}

/// Possible Contact Statement - Plan Research Agreement
//void CvDiplomacyAI::DoResearchAgreementPlan(PlayerTypes ePlayer, DiploStatementTypes &eStatement)
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//
//	if (eStatement == NO_DIPLO_STATEMENT_TYPE)
//	{
//		// AI must want RA
//		if (IsWantsResearchAgreementWithPlayer(ePlayer))
//		{
//			// We can't make a RA RIGHT NOW (otherwise we'd use the offer in the function above this one)
//			if (!IsCanMakeResearchAgreementRightNow(ePlayer))
//			{
//				// Can't ALREADY have a RA
//				if (!GET_TEAM(GetPlayer()->getTeam()).IsHasResearchAgreement(GET_PLAYER(ePlayer).getTeam()))
//				{
//					DiploStatementTypes eTempStatement = DIPLO_STATEMENT_PLAN_RESEARCH_AGREEMENT;
//					int iTurnsBetweenStatements = 20;
//
//					if (GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
//					{
//						eStatement = eTempStatement;
//					}
//				}
//			}
//		}
//	}
//}

/// Possible Contact Statement - Request
void CvDiplomacyAI::DoRequest(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_REQUEST;

		// If a request was accepted or rejected, wait 60 turns. If we rolled for rand and failed, wait 15 turns before we try again
		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 60 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_REQUEST_RANDFAILED) >= 15)
		{
			bool bRandPassed;	// This is used to see if we WOULD have made a request, but the rand roll failed (so add an entry to the log)
			bool bMakeRequest = IsMakeRequest(ePlayer, pDeal, bRandPassed);

			// Want to make a request of ePlayer? Pass pDeal in to see if there's actually anything we want
			if(bMakeRequest)
			{
				eStatement = eTempStatement;
				pDeal->SetRequestingPlayer(GetPlayer()->GetID());
			}

			// Clear out the deal if we don't want to offer it so that it's not tainted for the next trade possibility we look at
			else
				pDeal->ClearItems();

			// Add this statement to the log so we don't evaluate it again until 15 turns has come back around
			if(!bRandPassed)
				DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_REQUEST_RANDFAILED);
		}
	}
}

/// Possible Contact Statement - Gifrt
void CvDiplomacyAI::DoGift(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_GIFT;

		// If a request was accepted or rejected, wait 60 turns. If we rolled for rand and failed, wait 15 turns before we try again
		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 60 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_GIFT_RANDFAILED) >= 15)
		{
			bool bRandPassed = false;	// This is used to see if we WOULD have made a gift, but the rand roll failed (so add an entry to the log)
			bool bMakeGift = false;//IsMakeGift(ePlayer, pDeal, bRandPassed);

			// Want to make a request of ePlayer? Pass pDeal in to see if there's actually anything we want
			if(bMakeGift)
			{
				eStatement = eTempStatement;
				pDeal->SetRequestingPlayer(GetPlayer()->GetID());
			}

			// Clear out the deal if we don't want to offer it so that it's not tainted for the next trade possibility we look at
			else
				pDeal->ClearItems();

			// Add this statement to the log so we don't evaluate it again until 15 turns has come back around
			if(!bRandPassed)
				DoAddNewStatementToDiploLog(ePlayer, DIPLO_STATEMENT_GIFT_RANDFAILED);
		}
	}
}

/// Possible Contact Statement
//void CvDiplomacyAI::DoNowUnforgivableStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement)
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//
//	if (eStatement == NO_DIPLO_STATEMENT_TYPE)
//	{
//		bool bSendStatement = false;
//
//		// Unforgivable!
//		if (GetMajorCivOpinion(ePlayer) == MAJOR_CIV_OPINION_UNFORGIVABLE)
//		{
//			// Our approach (real or fake) can't be Friendly
//			if (GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true) != MAJOR_CIV_APPROACH_FRIENDLY)
//			{
//				bSendStatement = true;
//			}
//		}
//
//		if (bSendStatement)
//		{
//			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_NOW_UNFORGIVABLE;
//			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;
//
//			if (GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
//			{
//				eStatement = eTempStatement;
//			}
//		}
//	}
//}

/// Possible Contact Statement
//void CvDiplomacyAI::DoNowEnemyStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement)
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//
//	if (eStatement == NO_DIPLO_STATEMENT_TYPE)
//	{
//		bool bSendStatement = false;
//
//		// Don't show this message if we've already given a more severe one
//		if (m_paDiploLogStatementTurnCountScratchPad[DIPLO_STATEMENT_NOW_UNFORGIVABLE] == MAX_TURNS_SAFE_ESTIMATE)
//		{
//			// An enemy
//			if (GetMajorCivOpinion(ePlayer) == MAJOR_CIV_OPINION_ENEMY)
//			{
//				// Our approach (real or fake) can't be Friendly
//				if (GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true) != MAJOR_CIV_APPROACH_FRIENDLY)
//				{
//					bSendStatement = true;
//				}
//			}
//
//			if (bSendStatement)
//			{
//				DiploStatementTypes eTempStatement = DIPLO_STATEMENT_NOW_ENEMY;
//				int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;
//
//				if (GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
//				{
//					eStatement = eTempStatement;
//				}
//			}
//		}
//	}
//}

/// Possible Contact Statement
void CvDiplomacyAI::DoHostileStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		{
			TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

			// If we've made peace recently, don't go mouthing off right away
			int iPeaceTreatyTurn = GET_TEAM(GetTeam()).GetTurnMadePeaceTreatyWithTeam(eTeam);
			if(iPeaceTreatyTurn != -1)
			{
				int iTurnsSincePeace = GC.getGame().getElapsedGameTurns() - iPeaceTreatyTurn;
				if(iTurnsSincePeace < /*25*/ GC.getTURNS_SINCE_PEACE_WEIGHT_DAMPENER())
					return;
			}

			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_INSULT;
			int iTurnsBetweenStatements = 35;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}

/// Possible Contact Statement
//void CvDiplomacyAI::DoFriendlyStatement(PlayerTypes ePlayer, DiploStatementTypes &eStatement)
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//
//	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);
//
//	if (eStatement == NO_DIPLO_STATEMENT_TYPE)
//	{
//		if (eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
//		{
//			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_COMPLIMENT;
//			int iTurnsBetweenStatements = 35;
//
//			if (GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
//			{
//				eStatement = eTempStatement;
//			}
//		}
//	}
//}

/// Possible Contact Statement
void CvDiplomacyAI::DoAfraidStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_BOOT_KISSING;
			int iTurnsBetweenStatements = 35;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
				eStatement = eTempStatement;
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoWarmongerStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetWarmongerThreat(ePlayer) >= THREAT_SEVERE)
		{
			bool bSendStatement = true;

			// Don't send statement if we're going for conquest ourselves
			if(IsGoingForWorldConquest())
				bSendStatement = false;

			// 2 in 3 chance we don't actually send the message (don't want to bombard the player from all sides)
			if(33 < GC.getGame().getJonRandNum(100, "Diplomacy AI: rand roll to see if we warn a player about being a warmonger"))
				bSendStatement = false;

			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_WARMONGER;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(bSendStatement)
			{
				if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					eStatement = eTempStatement;
			}
			// Add this statement to the log so we don't evaluate it again next turn
			else
				DoAddNewStatementToDiploLog(ePlayer, eTempStatement);
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoMinorCivCompetitionStatement(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1, bool bIgnoreTurnsBetweenLimit)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		if(GetMinorCivDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		{
			DiploStatementTypes eTempStatement = DIPLO_STATEMENT_MINOR_CIV_COMPETITION;
			int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

			if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements || bIgnoreTurnsBetweenLimit)
			{
				// Find a city state we're upset over
				PlayerTypes eMinor;
				for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
				{
					eMinor = (PlayerTypes) iMinorLoop;

					// We have a PtP with this minor
					if(GET_PLAYER(eMinor).GetMinorCivAI()->IsProtectedByMajor(GetPlayer()->GetID()))
					{
						if(GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
						{
							iData1 = eMinor;
							break;
						}
						else if(GET_PLAYER(eMinor).GetMinorCivAI()->IsFriends(ePlayer))
						{
							iData1 = eMinor;
							break;
						}
					}
				}

				// Don't change the statement unless we found a minor to complain about
				if(iData1 != NO_PLAYER)
				{
					eStatement = eTempStatement;
				}
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoAngryBefriendedEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// We denounced the leader we're talking to - no use whining at this point
	if(IsDenouncedPlayer(ePlayer) || IsAtWar(ePlayer) || GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true) == MAJOR_CIV_APPROACH_HOSTILE)
		return;

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			// Loop through all players until we find one that is our enemy, that ePlayer befriended
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Must be alive
				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;

				// Can't be either of us
				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;

				// We haven't denounced this guy
				if(!IsDenouncedPlayer(eLoopPlayer))
					continue;

				// They haven't befriended this guy
				if(!pTheirDiploAI->IsDoFAccepted(eLoopPlayer))
					continue;

				// Too much time has passed (or maybe we already sent a message recently)
				if(pTheirDiploAI->GetDoFCounter(eLoopPlayer) > 1)
					continue;

				// Found a match!

				int iWeight = GetMeanness();		// Usually ranges from 3 to 7
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");

				// We're mean enough to say something
				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}
				// We're going to be nice!
				else
				{
					eStatement = DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY_RANDFAILED;
				}

				// We're done in here
				break;
			}
		}
	}
}

void CvDiplomacyAI::DoAngryDenouncedFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// We denounced the leader we're talking to - no use whining at this point
	if(IsDenouncedPlayer(ePlayer) || IsAtWar(ePlayer) || GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true) == MAJOR_CIV_APPROACH_HOSTILE)
		return;

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			// Loop through all players until we find one that is our friend, that ePlayer denounced
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Must be alive
				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;

				// Can't be either of us
				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;

				// We haven't befriended this guy
				if(!IsDoFAccepted(eLoopPlayer))
					continue;

				// They haven't denounced this guy
				if(!pTheirDiploAI->IsDenouncedPlayer(eLoopPlayer))
					continue;

				// Too much time has passed (or maybe we already sent a message recently)
				if(pTheirDiploAI->GetDenouncedPlayerCounter(eLoopPlayer) > 1)
					continue;

				// Found a match!

				int iWeight = GetMeanness();		// Usually ranges from 3 to 7
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");

				// We're mean enough to say something
				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}
				// We're going to be nice!
				else
				{
					eStatement = DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND_RANDFAILED;
				}

				// We're done in here
				break;
			}
		}
	}
}

void CvDiplomacyAI::DoHappyDenouncedEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// We denounced the leader we're talking to - no use talking at this point
	if(IsDenouncedPlayer(ePlayer) || IsAtWar(ePlayer) || GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true) == MAJOR_CIV_APPROACH_HOSTILE)
		return;

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			// Loop through all players until we find one that is our enemy, that ePlayer denounced
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Must be alive
				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;

				// Can't be either of us
				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;

				// We haven't denounced this guy
				if(!IsDenouncedPlayer(eLoopPlayer))
					continue;

				// They haven't denounced this guy
				if(!pTheirDiploAI->IsDenouncedPlayer(eLoopPlayer))
					continue;

				// Too much time has passed (or maybe we already sent a message recently)
				if(pTheirDiploAI->GetDenouncedPlayerCounter(eLoopPlayer) > 1)
					continue;

				// Found a match!

				int iWeight = GetChattiness();		// Usually ranges from 3 to 7
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");

				// We're chatty enough to say something
				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}
				// We're going to be nice!
				else
				{
					eStatement = DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY_RANDFAILED;
				}

				// We're done in here
				break;
			}
		}
	}
}

void CvDiplomacyAI::DoHappyBefriendedFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// We denounced the leader we're talking to - no use talking at this point
	if(IsDenouncedPlayer(ePlayer) || IsAtWar(ePlayer) || GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true) == MAJOR_CIV_APPROACH_HOSTILE)
		return;

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			// Loop through all players until we find one that is our friend, that ePlayer DoFed
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Must be alive
				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;

				// Can't be either of us
				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;

				// We haven't DoFed this guy
				if(!IsDoFAccepted(eLoopPlayer))
					continue;

				// They haven't DoFed this guy
				if(!pTheirDiploAI->IsDoFAccepted(eLoopPlayer))
					continue;

				// Too much time has passed (or maybe we already sent a message recently)
				if(pTheirDiploAI->GetDoFCounter(eLoopPlayer) > 1)
					continue;

				// Found a match!

				int iWeight = GetChattiness();		// Usually ranges from 3 to 7
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");

				// We're chatty enough to say something
				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}
				// We're going to be nice!
				else
				{
					eStatement = DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND_RANDFAILED;
				}

				// We're done in here
				break;
			}
		}
	}
}

/// Possible Contact Statement - Peace
void CvDiplomacyAI::DoPeaceOffer(PlayerTypes ePlayer, DiploStatementTypes& eStatement, CvDeal* pDeal)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		CvTeam* pOurTeam = &GET_TEAM(GetPlayer()->getTeam());
		TeamTypes eTheirTeam = GET_PLAYER(ePlayer).getTeam();

		// Have to have been at war for at least a little while
		if(GetPlayerNumTurnsAtWar(ePlayer) > 5)
		{
			// We can't be locked into war with them, or them with us
			if(pOurTeam->GetNumTurnsLockedIntoWar(eTheirTeam) == 0 &&
			        GET_TEAM(eTheirTeam).GetNumTurnsLockedIntoWar(GetPlayer()->getTeam()) == 0)
			{
				if(IsWantsPeaceWithPlayer(ePlayer))
				{
					DiploStatementTypes eTempStatement = DIPLO_STATEMENT_REQUEST_PEACE;
					int iTurnsBetweenStatements = 10;

					if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
					{
						if(GetPlayer()->GetDealAI()->IsOfferPeace(ePlayer, /*pDeal can be modified in this function*/ pDeal, false /*bEqualizingDeals*/))
						{
							eStatement = eTempStatement;
						}
						else
						{
							// Clear out the deal if we don't want to offer it so that it's not tainted for the next trade possibility we look at
							pDeal->ClearItems();
						}
					}
				}
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoFYIBefriendedHumanEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			int iWeight;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;

			// Loop through all players until we find one that we just made friends with, that ePlayer denounced
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Must be alive
				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;

				// Can't be either of us
				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;

				// We haven't befriended this guy
				if(!IsDoFAccepted(eLoopPlayer))
					continue;

				// They haven't denounced this guy
				if(!pTheirDiploAI->IsDenouncedPlayer(eLoopPlayer))
					continue;

				// Too much time has passed (or maybe we already sent a message recently)
				if(GetDoFCounter(eLoopPlayer) > 1)
					continue;

				eOpinion = GetMajorCivOpinion(ePlayer);
				eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

				// Don't say mean things if we like ePlayer
				if(eOpinion >= MAJOR_CIV_OPINION_FAVORABLE)
					continue;
				if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
					continue;

				iWeight = 0;

				if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
					iWeight += 2;
				else if(eOpinion == MAJOR_CIV_OPINION_ENEMY)
					iWeight += 5;
				else if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
					iWeight += 10;

				iWeight += GetMeanness();		// Usually ranges from 3 to 7
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");

				// We're mean enough to say something
				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}
				// We're going to be nice!
				else
				{
					eStatement = DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY_RANDFAILED;
				}

				// We're done in here
				break;
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoFYIDenouncedHumanFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			int iWeight;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;

			// Loop through all players until we find one that we just denoucned, that ePlayer has befriended
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Must be alive
				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;

				// Can't be either of us
				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;

				// We haven't denounced this guy
				if(!IsDenouncedPlayer(eLoopPlayer))
					continue;

				// They haven't befriended this guy
				if(!pTheirDiploAI->IsDoFAccepted(eLoopPlayer))
					continue;

				// Too much time has passed (or maybe we already sent a message recently)
				if(GetDenouncedPlayerCounter(eLoopPlayer) > 1)
					continue;

				eOpinion = GetMajorCivOpinion(ePlayer);
				eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

				// Don't say mean things if we like ePlayer
				if(eOpinion >= MAJOR_CIV_OPINION_FAVORABLE)
					continue;
				if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
					continue;

				iWeight = 0;

				if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
					iWeight += 2;
				else if(eOpinion == MAJOR_CIV_OPINION_ENEMY)
					iWeight += 5;
				else if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
					iWeight += 10;

				iWeight += GetMeanness();		// Usually ranges from 3 to 7
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");

				// We're mean enough to say something
				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}
				// We're going to be nice!
				else
				{
					eStatement = DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND_RANDFAILED;
				}

				// We're done in here
				break;
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoFYIDenouncedHumanEnemy(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			int iWeight;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;

			// Loop through all players until we find one that we just denounced, that ePlayer has denounced
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Must be alive
				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;

				// Can't be either of us
				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;

				// We haven't denounced this guy
				if(!IsDenouncedPlayer(eLoopPlayer))
					continue;

				// They haven't denounced this guy
				if(!pTheirDiploAI->IsDenouncedPlayer(eLoopPlayer))
					continue;

				// Too much time has passed (or maybe we already sent a message recently)
				if(GetDenouncedPlayerCounter(eLoopPlayer) > 1)
					continue;

				eOpinion = GetMajorCivOpinion(ePlayer);
				eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

				// Don't say nice things if we dislike ePlayer
				if(eOpinion <= MAJOR_CIV_OPINION_COMPETITOR)
					continue;
				if(eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_HOSTILE)
					continue;

				iWeight = 0;

				if(eOpinion == MAJOR_CIV_OPINION_FAVORABLE)
					iWeight += 2;
				else if(eOpinion == MAJOR_CIV_OPINION_FRIEND)
					iWeight += 5;
				else if(eOpinion == MAJOR_CIV_OPINION_ALLY)
					iWeight += 10;

				if(eOpinion == MAJOR_CIV_APPROACH_FRIENDLY)
					iWeight += 2;

				// Add weight if they're strong
				if(GetPlayerMilitaryStrengthComparedToUs(ePlayer) > STRENGTH_AVERAGE)
					iWeight += 3;

				iWeight += GetChattiness();		// Usually ranges from 3 to 7
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");

				// We're mean enough to say something
				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}
				// We're going to be nice!
				else
				{
					eStatement = DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY_RANDFAILED;
				}

				// We're done in here
				break;
			}
		}
	}
}

/// Possible Contact Statement
void CvDiplomacyAI::DoFYIBefriendedHumanFriend(PlayerTypes ePlayer, DiploStatementTypes& eStatement, int& iData1)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND;

		if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= 50 &&
		        GetNumTurnsSinceStatementSent(ePlayer, DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND_RANDFAILED) >= 20)
		{
			CvDiplomacyAI* pTheirDiploAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

			int iWeight;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;

			// Loop through all players until we find one that we just befriended, that ePlayer has befriended
			PlayerTypes eLoopPlayer;
			for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Must be alive
				if(!GET_PLAYER(eLoopPlayer).isAlive())
					continue;

				// Can't be either of us
				if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
					continue;

				// We haven't befriended this guy
				if(!IsDoFAccepted(eLoopPlayer))
					continue;

				// They haven't befriended this guy
				if(!pTheirDiploAI->IsDoFAccepted(eLoopPlayer))
					continue;

				// Too much time has passed (or maybe we already sent a message recently)
				if(GetDoFCounter(eLoopPlayer) > 1)
					continue;

				eOpinion = GetMajorCivOpinion(ePlayer);
				eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

				// Don't say nice things if we dislike ePlayer
				if(eOpinion <= MAJOR_CIV_OPINION_COMPETITOR)
					continue;
				if(eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_HOSTILE)
					continue;

				iWeight = 0;

				if(eOpinion == MAJOR_CIV_OPINION_FAVORABLE)
					iWeight += 2;
				else if(eOpinion == MAJOR_CIV_OPINION_FRIEND)
					iWeight += 5;
				else if(eOpinion == MAJOR_CIV_OPINION_ALLY)
					iWeight += 10;

				if(eOpinion == MAJOR_CIV_APPROACH_FRIENDLY)
					iWeight += 2;

				iWeight += GetChattiness();		// Usually ranges from 3 to 7
				iWeight += GC.getGame().getJonRandNum(7, "Random roll for AI statement: 0");

				// We're mean enough to say something
				if(iWeight >= 7)
				{
					eStatement = eTempStatement;
					iData1 = eLoopPlayer;
				}
				// We're going to be nice!
				else
				{
					eStatement = DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND_RANDFAILED;
				}

				// We're done in here
				break;
			}
		}
	}
}

void CvDiplomacyAI::DoHappySamePolicyTree(PlayerTypes ePlayer, DiploStatementTypes& eStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eStatement == NO_DIPLO_STATEMENT_TYPE)
	{
		DiploStatementTypes eTempStatement = NO_DIPLO_STATEMENT_TYPE;

		PolicyBranchTypes eMyBranch = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
		PolicyBranchTypes eTheirBranch = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
		if(eMyBranch != NO_POLICY_BRANCH_TYPE && eTheirBranch != NO_POLICY_BRANCH_TYPE && eMyBranch == eTheirBranch)
		{
			// Don't say nice things if we dislike ePlayer
			bool bSkip = false;
			MajorCivApproachTypes eApproach;
			MajorCivOpinionTypes eOpinion;
			eOpinion = GetMajorCivOpinion(ePlayer);
			eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);
			if(eOpinion <= MAJOR_CIV_OPINION_COMPETITOR)
				bSkip = true;
			if(eApproach == MAJOR_CIV_APPROACH_WAR || eApproach == MAJOR_CIV_APPROACH_HOSTILE)
				bSkip = true;

			// Check chattiness to see if we send the message this turn
			if(!bSkip && GetChattiness() > GC.getGame().getJonRandNum(10, "Diplomacy AI: rand roll to see if we congratulate a player on the same policy tree choice"))
			{
				if(eMyBranch == GC.getInfoTypeForString("POLICY_BRANCH_FREEDOM", true /*bHideAssert*/))
					eTempStatement = DIPLO_STATEMENT_SAME_POLICIES_FREEDOM;
				else if(eMyBranch == GC.getInfoTypeForString("POLICY_BRANCH_ORDER", true /*bHideAssert*/))
					eTempStatement = DIPLO_STATEMENT_SAME_POLICIES_ORDER;
				else if(eMyBranch == GC.getInfoTypeForString("POLICY_BRANCH_AUTOCRACY", true /*bHideAssert*/))
					eTempStatement = DIPLO_STATEMENT_SAME_POLICIES_AUTOCRACY;

				if(eTempStatement != NO_DIPLO_STATEMENT_TYPE)
				{
					int iTurnsBetweenStatements = MAX_TURNS_SAFE_ESTIMATE;

					if(GetNumTurnsSinceStatementSent(ePlayer, eTempStatement) >= iTurnsBetweenStatements)
						eStatement = eTempStatement;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////
// Diplo stuff relating to UI
/////////////////////////////////////////////////////////


/// Initiate diplo screen with default state
void CvDiplomacyAI::DoBeginDiploWithHuman()
{
	if(!GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		LeaderheadAnimationTypes eAnimation = LEADERHEAD_ANIM_NEUTRAL_HELLO;
		const char* szText = GetGreetHumanMessage(eAnimation);

		gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_DEFAULT_ROOT, szText, eAnimation);
	}
}

/// Initiate diplo screen after the player has clicked on the notification to threaten the thief.
void CvDiplomacyAI::DoBeginDiploWithHumanEspionageResult()
{
	if(!GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR) && !IsAtWar(GC.getGame().getActivePlayer()))
	{
		LeaderheadAnimationTypes eAnimation = LEADERHEAD_ANIM_NEUTRAL_HELLO;
		const char* szText = GetDiploStringForMessage(DIPLO_MESSAGE_CONFRONT_YOU_KILLED_MY_SPY, GC.getGame().getActivePlayer());
		gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_CONFRONT_YOU_KILLED_MY_SPY, szText, eAnimation);
	}
}

/// Initiate diplo screen after the player has clicked on the notification that involves intrigue
void CvDiplomacyAI::DoBeginDiploWithHumanInDiscuss()
{
	if(!GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		LeaderheadAnimationTypes eAnimation = LEADERHEAD_ANIM_NEUTRAL_HELLO;
		const char* szText = GetGreetHumanMessage(eAnimation);
		gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, szText, eAnimation);
	}
}


/// What should an AI leader say for a particular situation?
const char* CvDiplomacyAI::GetDiploStringForMessage(DiploMessageTypes eDiploMessage, PlayerTypes eForPlayer, const char* szOptionalKey1, const char* szOptionalKey2)
{
	CvAssertMsg(eDiploMessage >= 0, "DIPLOMACY_AI: Invalid DiploMessageType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDiploMessage < NUM_DIPLO_MESSAGE_TYPES, "DIPLOMACY_AI: Invalid DiploMessageType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eForPlayer >= NO_PLAYER, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");	// NO_PLAYER is valid because eForPlayer is used when we need specific data (e.g. for declaring war)
	CvAssertMsg(eForPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText;

	switch(eDiploMessage)
	{
		//////////////////////////////////////////////////////////////
		// AI greeting messages
		//////////////////////////////////////////////////////////////

		// Intro
	case DIPLO_MESSAGE_INTRO:
		strText = GetDiploTextFromTag("RESPONSE_FIRST_GREETING");
		break;

		// Defeated
	case DIPLO_MESSAGE_DEFEATED:
		strText = GetDiploTextFromTag("RESPONSE_DEFEATED");
		break;

		// Repeat Hello (after the player has gone into the screen too many times)
	case DIPLO_MESSAGE_GREETING_REPEAT_TOO_MUCH:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_REPEAT_TOO_MUCH");
		break;

		// Repeat Hello
	case DIPLO_MESSAGE_GREETING_REPEAT:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_REPEAT");
		break;

		// Repeat Hello Hostile
	case DIPLO_MESSAGE_GREETING_HOSTILE_REPEAT:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_REPEAT");
		break;

		// Polite Hello
	case DIPLO_MESSAGE_GREETING_FRIENDLY_HELLO:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_POLITE_HELLO");
		break;

		// Neutral Hello
	case DIPLO_MESSAGE_GREETING_NEUTRAL_HELLO:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_NEUTRAL_HELLO");
		break;

		// Hostile Hello
	case DIPLO_MESSAGE_GREETING_HOSTILE_HELLO:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HELLO");
		break;

		// Greeting: Human is crushing this AI
	case DIPLO_MESSAGE_GREETING_DESTRUCTION_LOOMS:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DESTRUCTION_LOOMS");
		break;

		// Greeting: At War with human but wants peace
	case DIPLO_MESSAGE_GREETING_AT_WAR_WANTS_PEACE:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AT_WAR_WANTS_PEACE");
		break;

		// Greeting: At War with human and is being hostile
	case DIPLO_MESSAGE_GREETING_AT_WAR_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AT_WAR_HOSTILE");
		break;

		// Greeting: At War with human and now ready to accept surrender
	case DIPLO_MESSAGE_GREETING_WILL_ACCEPT_SURRENDER:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_WILL_ACCEPT_SURRENDER");
		break;

		// Greeting: Research Agreement
	case DIPLO_MESSAGE_GREETING_RESEARCH_AGREEMENT:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_RESEARCH_AGREEMENT");
		break;

		// Greeting: Human has broken military promises
	case DIPLO_MESSAGE_GREETING_BROKEN_MILITARY_PROMISE:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_BROKEN_MILITARY_PROMISE");
		break;

		// Greeting: Players are working together
	case DIPLO_MESSAGE_GREETING_WORKING_WITH:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_WORKING_WITH");
		break;

		// Greeting: Players are working together
	case DIPLO_MESSAGE_GREETING_WORKING_AGAINST:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_WORKING_AGAINST", szOptionalKey1);
		break;

		// Greeting: Players in (or planning) a coop war against a third player
	case DIPLO_MESSAGE_GREETING_COOP_WAR:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_COOP_WAR", szOptionalKey1);
		break;

		// Greeting: Human at war Hostile
	case DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_AT_WAR:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HUMAN_AT_WAR");
		break;

		// Greeting: Human at war
	case DIPLO_MESSAGE_GREETING_HUMAN_AT_WAR:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HUMAN_AT_WAR");
		break;

		// Greeting: Human aggressive military (hostile)
	case DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_MILITARY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_AGGRESSIVE_MILITARY");
		break;

		// Greeting: Human aggressive military
	case DIPLO_MESSAGE_GREETING_AGGRESSIVE_MILITARY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AGGRESSIVE_MILITARY");
		break;

		// Greeting: Human aggressive expansion (hostile)
	case DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_EXPANSION:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_AGGRESSIVE_EXPANSION");
		break;

		// Greeting: Human aggressive expansion
	case DIPLO_MESSAGE_GREETING_AGGRESSIVE_EXPANSION:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AGGRESSIVE_EXPANSION");
		break;

		// Greeting: Human aggressive plot buying (hostile)
	case DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_PLOT_BUYING:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_AGGRESSIVE_PLOT_BUYING");
		break;

		// Greeting: Human aggressive plot buying
	case DIPLO_MESSAGE_GREETING_AGGRESSIVE_PLOT_BUYING:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_AGGRESSIVE_PLOT_BUYING");
		break;

		// Greeting: Human has a strong military
	case DIPLO_MESSAGE_GREETING_FRIENDLY_STRONG_MILITARY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_FRIENDLY_STRONG_MILITARY");
		break;

		// Greeting: Human has a strong economy
	case DIPLO_MESSAGE_GREETING_FRIENDLY_STRONG_ECONOMY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_FRIENDLY_STRONG_ECONOMY");
		break;

		// Greeting: Human has few Cities
	case DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_FEW_CITIES:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HUMAN_FEW_CITIES");
		break;

		// Greeting: Human has a small army
	case DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_SMALL_ARMY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HUMAN_SMALL_ARMY");
		break;

		// Greeting: Human beats up people
	case DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_IS_WARMONGER:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_HOSTILE_HUMAN_IS_WARMONGER");
		break;

		//////////////////////////////////////////////////////////////
		// AI has a trade offer for the human
		//////////////////////////////////////////////////////////////

		// AI asks the player what deal he's offering
	case DIPLO_MESSAGE_DOT_DOT_DOT:
		strText = GetDiploTextFromTag("RESPONSE_DOT_DOT_DOT");
		break;

		// AI asks the player what deal he's offering
	case DIPLO_MESSAGE_LETS_HEAR_IT:
		strText = GetDiploTextFromTag("RESPONSE_LETS_HEAR_IT");
		break;

		// AI asking the Human to make peace
	case DIPLO_MESSAGE_PEACE_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_PEACE_OFFER");
		break;

		// AI is making a demand
	case DIPLO_MESSAGE_DEMAND:
		strText = GetDiploTextFromTag("RESPONSE_DEMAND");
		break;

		// AI is making a request
	case DIPLO_MESSAGE_REQUEST:
		strText = GetDiploTextFromTag("RESPONSE_REQUEST");
		break;

		// AI wants a Luxury someone has
	case DIPLO_MESSAGE_LUXURY_TRADE:
		strText = GetDiploTextFromTag("RESPONSE_LUXURY_TRADE");
		break;

		// AI offers equal embassy exchange agreement
	case DIPLO_MESSAGE_EMBASSY_EXCHANGE:
		strText = GetDiploTextFromTag("RESPONSE_EMBASSY_EXCHANGE");
		break;

	case DIPLO_MESSAGE_EMBASSY_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_EMBASSY_OFFER");
		break;

		// AI offers equal Open Borders agreement
	case DIPLO_MESSAGE_OPEN_BORDERS_EXCHANGE:
		strText = GetDiploTextFromTag("RESPONSE_OPEN_BORDERS_EXCHANGE");
		break;

		// AI asks for Open Borders
	case DIPLO_MESSAGE_OPEN_BORDERS_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_OPEN_BORDERS_OFFER");
		break;

		// AI wants to make RA agreement with this player at some point soon
	case DIPLO_MESSAGE_PLAN_RESEARCH_AGREEMENT:
		strText = GetDiploTextFromTag("RESPONSE_PLAN_RESEARCH_AGREEMENT");
		break;

		// AI wants RA with player
	case DIPLO_MESSAGE_RESEARCH_AGREEMENT_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_RESEARCH_AGREEMENT_OFFER");
		break;

	case DIPLO_MESSAGE_RENEW_DEAL:
		strText = GetDiploTextFromTag("RESPONSE_RENEW_DEAL");
		break;

	case DIPLO_MESSAGE_WANT_MORE_RENEW_DEAL:
		strText = GetDiploTextFromTag("RESPONSE_WANT_MORE_RENEW_DEAL");
		break;

		//////////////////////////////////////////////////////////////
		// Generic AI messages to another player; some friendship, some warnings, etc.
		//////////////////////////////////////////////////////////////

		// AI is warning player about his military placement
	case DIPLO_MESSAGE_HOSTILE_AGGRESSIVE_MILITARY_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_AGGRESSIVE_MILITARY_WARNING");
		break;

		// AI is warning player about his military placement
	case DIPLO_MESSAGE_AGGRESSIVE_MILITARY_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_AGGRESSIVE_MILITARY_WARNING");
		break;

		// AI is SERIOUSLY warning player about his expansion
	case DIPLO_MESSAGE_EXPANSION_SERIOUS_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_EXPANSION_SERIOUS_WARNING");
		break;

		// AI is warning player about his expansion
	case DIPLO_MESSAGE_EXPANSION_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_EXPANSION_WARNING");
		break;

		// AI is telling the player he broke a promise
	case DIPLO_MESSAGE_EXPANSION_BROKEN_PROMISE:
		strText = GetDiploTextFromTag("RESPONSE_EXPANSION_BROKEN_PROMISE");
		break;

		// AI is SERIOUSLY warning player about his plot buying
	case DIPLO_MESSAGE_PLOT_BUYING_SERIOUS_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_PLOT_BUYING_SERIOUS_WARNING");
		break;

		// AI is warning player about his plot buying
	case DIPLO_MESSAGE_PLOT_BUYING_WARNING:
		strText = GetDiploTextFromTag("RESPONSE_PLOT_BUYING_WARNING");
		break;

		// AI is telling the player he broke a promise
	case DIPLO_MESSAGE_PLOT_BUYING_BROKEN_PROMISE:
		strText = GetDiploTextFromTag("RESPONSE_PLOT_BUYING_BROKEN_PROMISE");
		break;

		// AI attacked a Minor the human has Friendship with (hostile)
	case DIPLO_MESSAGE_HOSTILE_WE_ATTACKED_YOUR_MINOR:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_ATTACKED_YOUR_MINOR", szOptionalKey1);
		break;

		// AI attacked a Minor the human has Friendship with
	case DIPLO_MESSAGE_WE_ATTACKED_YOUR_MINOR:
		strText = GetDiploTextFromTag("RESPONSE_WE_ATTACKED_YOUR_MINOR", szOptionalKey1);
		break;

		// AI bullied a Minor the human has a PtP with (hostile)
	case DIPLO_MESSAGE_HOSTILE_WE_BULLIED_YOUR_MINOR:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_BULLIED_YOUR_MINOR", szOptionalKey1);
		break;

		// AI bullied a Minor the human has a PtP with
	case DIPLO_MESSAGE_WE_BULLIED_YOUR_MINOR:
		strText = GetDiploTextFromTag("RESPONSE_WE_BULLIED_YOUR_MINOR", szOptionalKey1);
		break;

		// AI would like to work with a player
	case DIPLO_MESSAGE_WORK_WITH_US:
		strText = GetDiploTextFromTag("RESPONSE_WORK_WITH_US");
		break;

		// AI is done working with a player
	case DIPLO_MESSAGE_END_WORK_WITH_US:
		strText = GetEndDoFMessage(eForPlayer);
		break;

		// AI would like to work against someone with a player
	case DIPLO_MESSAGE_WORK_AGAINST_SOMEONE:
		strText = GetDiploTextFromTag("RESPONSE_WORK_AGAINST_SOMEONE", szOptionalKey1);
		break;

		// AI is done working with a player against someone
	case DIPLO_MESSAGE_END_WORK_AGAINST_SOMEONE:
		strText = GetEndWorkAgainstSomeoneMessage(eForPlayer, szOptionalKey1);
		break;

		// AI would like to declare war on someone with a player
	case DIPLO_MESSAGE_COOP_WAR_REQUEST:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_REQUEST", szOptionalKey1);
		break;

		// AI calls up and says it's time to declare war on someone with a player
	case DIPLO_MESSAGE_COOP_WAR_TIME:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_TIME", szOptionalKey1);
		break;

		// AI is telling player he's unforgivable
	case DIPLO_MESSAGE_NOW_UNFORGIVABLE:
		strText = GetDiploTextFromTag("RESPONSE_NOW_UNFORGIVABLE");
		break;

		// AI is telling player he's an enemy
	case DIPLO_MESSAGE_NOW_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_NOW_ENEMY");
		break;

		// AI is making fun of the player
	case DIPLO_MESSAGE_INSULT_ROOT:
		strText = GetInsultHumanMessage();
		break;

		// AI is making fun of the player
	case DIPLO_MESSAGE_INSULT_GENERIC:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_GENERIC");
		break;

		// AI is making fun of the player because of his military
	case DIPLO_MESSAGE_INSULT_MILITARY:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_MILITARY");
		break;

		// AI is making fun of the player because of his lack of nukes
	case DIPLO_MESSAGE_INSULT_NUKE:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_NUKE");
		break;

		// AI is making fun of the player because he picks on minors
	case DIPLO_MESSAGE_INSULT_BULLY:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_BULLY");
		break;

		// AI is making fun of the player because his people are unhappy
	case DIPLO_MESSAGE_INSULT_UNHAPPINESS:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_UNHAPPINESS");
		break;

		// AI is making fun of the player because he doesn't have many Cities
	case DIPLO_MESSAGE_INSULT_CITIES:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_CITIES");
		break;

		// AI is making fun of the player because of his population
	case DIPLO_MESSAGE_INSULT_POPULATION:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_POPULATION");
		break;

		// AI is making fun of the player because of his Culture
	case DIPLO_MESSAGE_INSULT_CULTURE:
		strText = GetDiploTextFromTag("RESPONSE_INSULT_CULTURE");
		break;

		// AI is being nice to the player
	case DIPLO_MESSAGE_COMPLIMENT:
		strText = GetDiploTextFromTag("RESPONSE_COMPLIMENT");
		break;

		// AI is afraid of the player
	case DIPLO_MESSAGE_BOOT_KISSING:
		strText = GetDiploTextFromTag("RESPONSE_BOOT_KISSING");
		break;

		// AI is warning player about being a warmonger
	case DIPLO_MESSAGE_WARMONGER:
		strText = GetDiploTextFromTag("RESPONSE_WARMONGER");
		break;

		// AI is warning player about getting too cozy with a minor
	case DIPLO_MESSAGE_MINOR_CIV_COMPETITION:
		strText = GetDiploTextFromTag("RESPONSE_MINOR_CIV_COMPETITION", szOptionalKey1);
		break;

		// AI is pleased with the human
	case DIPLO_MESSAGE_PLEASED:
		strText = GetDiploTextFromTag("RESPONSE_PLEASED");
		break;

		// AI is thankful towards the human
	case DIPLO_MESSAGE_THANKFUL:
		strText = GetDiploTextFromTag("RESPONSE_THANKFUL");
		break;

		// AI is disappointed in the human
	case DIPLO_MESSAGE_DISAPPOINTED:
		strText = GetDiploTextFromTag("RESPONSE_DISAPPOINTED");
		break;

		// Human has done something bad, and now we're gonna show 'em
	case DIPLO_MESSAGE_SO_BE_IT:
		strText = GetDiploTextFromTag("RESPONSE_SO_BE_IT");
		break;

		// Human returned a captured civilian to us!
	case DIPLO_MESSAGE_RETURNED_CIVILIAN:
		strText = GetDiploTextFromTag("RESPONSE_RETURNED_CIVILIAN");
		break;

		// Human Culture Bombed us!
	case DIPLO_MESSAGE_CULTURE_BOMBED:
		strText = GetDiploTextFromTag("RESPONSE_CULTURE_BOMBED");
		break;

		//////////////////////////////////////////////////////////////
		// AI has a public declaration to make to the world
		//////////////////////////////////////////////////////////////

		// AI is protecting a City State
	case DIPLO_MESSAGE_DECLARATION_PROTECT_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_DECLARATION_PROTECT_CITY_STATE", szOptionalKey1, GetPlayer()->getCivilizationShortDescriptionKey());
		break;

		// AI is no longer protecting a City State
	case DIPLO_MESSAGE_DECLARATION_ABANDON_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_DECLARATION_ABANDON_CITY_STATE", szOptionalKey1, GetPlayer()->getCivilizationShortDescriptionKey());
		break;

		//////////////////////////////////////////////////////////////
		// Human is asking the AI for something
		//////////////////////////////////////////////////////////////

		// Human has asked us about something recently, and we still say no
	case DIPLO_MESSAGE_REPEAT_NO:
		strText = GetDiploTextFromTag("RESPONSE_REPEAT_NO");
		break;

		// Human asks us not to settle near him, and we say yes
	case DIPLO_MESSAGE_DONT_SETTLE_YES:
		strText = GetDiploTextFromTag("RESPONSE_DONT_SETTLE_YES");
		break;

		// Human asks us not to settle near him, but we say no
	case DIPLO_MESSAGE_DONT_SETTLE_NO:
		strText = GetDiploTextFromTag("RESPONSE_DONT_SETTLE_NO");
		break;

		// Human asks us to work with him, and we say yes
	case DIPLO_MESSAGE_WORK_WITH_US_YES:
		strText = GetDiploTextFromTag("RESPONSE_WORK_WITH_US_YES");
		break;

		// Human asks us to work with him, but we say no
	case DIPLO_MESSAGE_WORK_WITH_US_NO:
		strText = GetDiploTextFromTag("RESPONSE_WORK_WITH_US_NO");
		break;

		// Human asks us to work against someone, and we say yes
	case DIPLO_MESSAGE_WORK_AGAINST_SOMEONE_YES:
		strText = GetDiploTextFromTag("RESPONSE_WORK_AGAINST_SOMEONE_YES");
		break;

		// Human asks us to work against someone, but we say no
	case DIPLO_MESSAGE_WORK_AGAINST_SOMEONE_NO:
		strText = GetDiploTextFromTag("RESPONSE_WORK_AGAINST_SOMEONE_NO");
		break;

		// Human asks us to declare war on someone, and we say yes
	case DIPLO_MESSAGE_COOP_WAR_YES:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_YES");
		break;

		// Human asks us to declare war on someone, but we say no
	case DIPLO_MESSAGE_COOP_WAR_NO:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_NO");
		break;

		// Human asks us to declare war someone, and we say soon
	case DIPLO_MESSAGE_COOP_WAR_SOON:
		strText = GetDiploTextFromTag("RESPONSE_COOP_WAR_SOON");
		break;

		// Human makes demand of us, and we say yes
	case DIPLO_MESSAGE_HUMAN_DEMAND_YES:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_YES");
		break;

		// Human makes demand of us, and we say no
	case DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_WEAK:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_REFUSE_WEAK");
		break;

		// Human makes demand of us, and we say no
	case DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_REFUSE_HOSTILE");
		break;

		// Human makes demand of us, and we tell him he's asking for too much
	case DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_TOO_MUCH:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_REFUSE_TOO_MUCH");
		break;

		// Human makes demand of us, and we say too soon
	case DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_TOO_SOON:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DEMAND_REFUSE_TOO_SOON");
		break;

		//////////////////////////////////////////////////////////////
		// AI popped up to tell the human something, human responded and now we're responding back
		//////////////////////////////////////////////////////////////

		// We noticed human military buildup and he said he was going to kill us now (hostile)
	case DIPLO_MESSAGE_HUMAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_AGGRESSIVE_MILITARY_WARNING_BAD");
		break;

		// We noticed human military buildup and he said betsu ni (hostile)
	case DIPLO_MESSAGE_HUMAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_AGGRESSIVE_MILITARY_WARNING_GOOD");
		break;

		// We noticed human military buildup and he said he was going to kill us now :(
	case DIPLO_MESSAGE_HUMAN_AGGRESSIVE_MILITARY_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_AGGRESSIVE_MILITARY_WARNING_BAD");
		break;

		// We noticed human military buildup and he said betsu ni
	case DIPLO_MESSAGE_HUMAN_AGGRESSIVE_MILITARY_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_AGGRESSIVE_MILITARY_WARNING_GOOD");
		break;

		// AI attacked Human's Minor, Human vows revenge, our response (hostile)
	case DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_ATTACKED_MINOR_BAD");
		break;

		// AI attacked Human's Minor, Human forgives us, our response (hostile)
	case DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_ATTACKED_MINOR_GOOD");
		break;

		// AI attacked Human's Minor, Human vows revenge, our response
	case DIPLO_MESSAGE_HUMAN_WE_ATTACKED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_WE_ATTACKED_MINOR_BAD");
		break;

		// AI attacked Human's Minor, Human forgives us, our response
	case DIPLO_MESSAGE_HUMAN_WE_ATTACKED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_WE_ATTACKED_MINOR_GOOD");
		break;

		// AI bullied Human's Minor, Human vows revenge, our response (hostile)
	case DIPLO_MESSAGE_HUMAN_HOSTILE_WE_BULLIED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_BULLIED_MINOR_BAD");
		break;

		// AI bullied Human's Minor, Human forgives us, our response (hostile)
	case DIPLO_MESSAGE_HUMAN_HOSTILE_WE_BULLIED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HOSTILE_WE_BULLIED_MINOR_GOOD");
		break;

		// AI bullied Human's Minor, Human vows revenge, our response
	case DIPLO_MESSAGE_HUMAN_WE_BULLIED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_WE_BULLIED_MINOR_BAD");
		break;

		// AI bullied Human's Minor, Human forgives us, our response
	case DIPLO_MESSAGE_HUMAN_WE_BULLIED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_WE_BULLIED_MINOR_GOOD");
		break;

		// Other player attacked a City State this AI is protective of
	case DIPLO_MESSAGE_HUMAN_ATTACKED_PROTECTED_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_PROTECTED_CITY_STATE", szOptionalKey1);
		break;

		// AI is upset that human won't stop attacking a Minor
	case DIPLO_MESSAGE_HUMAN_ATTACKED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_ATTACKED_MINOR_BAD");
		break;

		// AI is happy that human says he'll stop attacking a Minor
	case DIPLO_MESSAGE_HUMAN_ATTACKED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_ATTACKED_MINOR_GOOD");
		break;

		// Other player killed a City State this AI is protective of
	case DIPLO_MESSAGE_HUMAN_KILLED_PROTECTED_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_KILLED_PROTECTED_CITY_STATE", szOptionalKey1);
		break;

		// AI is upset that human killed a Minor
	case DIPLO_MESSAGE_HUMAN_KILLED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_KILLED_MINOR_BAD");
		break;

		// AI is willing to receive gift from human to ease relations after killing a protected Minor
	case DIPLO_MESSAGE_HUMAN_KILLED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_KILLED_MINOR_GOOD");
		break;

		// Other player bullied a City State this AI is protective of
	case DIPLO_MESSAGE_HUMAN_BULLIED_PROTECTED_CITY_STATE:
		strText = GetDiploTextFromTag("RESPONSE_BULLIED_PROTECTED_CITY_STATE", szOptionalKey1);
		break;

		// AI is upset that human won't stop bullying a Minor
	case DIPLO_MESSAGE_HUMAN_BULLIED_MINOR_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_BULLIED_MINOR_BAD");
		break;

		// AI is happy that human says he'll stop bullying a Minor
	case DIPLO_MESSAGE_HUMAN_BULLIED_MINOR_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_BULLIED_MINOR_GOOD");
		break;

		// AI mad at human because he won't heed our serious expansion warning
	case DIPLO_MESSAGE_HUMAN_SERIOUS_EXPANSION_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SERIOUS_EXPANSION_WARNING_BAD");
		break;

		// AI is willing to receive gift from human to ease relations after serious expansion warning
	case DIPLO_MESSAGE_HUMAN_SERIOUS_EXPANSION_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SERIOUS_EXPANSION_WARNING_GOOD");
		break;

		// AI mad at human because he won't heed our expansion warning
	case DIPLO_MESSAGE_HUMAN_EXPANSION_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_EXPANSION_WARNING_BAD");
		break;

		// AI is happy human has said he won't expand near him in the future
	case DIPLO_MESSAGE_HUMAN_EXPANSION_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_EXPANSION_WARNING_GOOD");
		break;

		// AI mad at human because he won't heed our serious plot buying warning
	case DIPLO_MESSAGE_HUMAN_SERIOUS_PLOT_BUYING_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SERIOUS_PLOT_BUYING_WARNING_BAD");
		break;

		// AI is willing to receive gift from human to ease relations after serious plot buying warning
	case DIPLO_MESSAGE_HUMAN_SERIOUS_PLOT_BUYING_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_SERIOUS_PLOT_BUYING_WARNING_GOOD");
		break;

		// AI mad at human because he won't heed our plot buying warning
	case DIPLO_MESSAGE_HUMAN_PLOT_BUYING_WARNING_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_PLOT_BUYING_WARNING_BAD");
		break;

		// AI is happy human has said he won't buy plots near him in the future
	case DIPLO_MESSAGE_HUMAN_PLOT_BUYING_WARNING_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_PLOT_BUYING_WARNING_GOOD");
		break;

		//////////////////////////////////////////////////////////////
		// Peace messages - DEPRECATED???
		//////////////////////////////////////////////////////////////

		// Player requests peace from AI, AI is is curious what the offer will be
	case DIPLO_MESSAGE_PEACE_WHAT_WILL_HUMAN_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_PEACE_WHAT_WILL_HUMAN_OFFER");
		break;

		// Player requests peace from AI, AI is happy
	case DIPLO_MESSAGE_PEACE_MADE_BY_HUMAN_GRACIOUS:
		strText = GetDiploTextFromTag("RESPONSE_PEACE_MADE_BY_HUMAN_GRACIOUS");
		break;

		// Player requests peace from AI but it says no
	case DIPLO_MESSAGE_NO_PEACE:
		strText = GetDiploTextFromTag("RESPONSE_NO_PEACE");
		break;

		// Player requests peace from AI on the same turn it attacked...
	case DIPLO_MESSAGE_TOO_SOON_NO_PEACE:
		strText = GetDiploTextFromTag("RESPONSE_TOO_SOON_NO_PEACE");
		break;

		//////////////////////////////////////////////////////////////
		// Trade responses
		//////////////////////////////////////////////////////////////

		// Human asks to AI for a bad deal too many times
	case DIPLO_MESSAGE_REPEAT_TRADE_TOO_MUCH:
		strText = GetDiploTextFromTag("RESPONSE_REPEAT_TRADE_TOO_MUCH");
		break;

		// Human asks to AI for a bad deal again
	case DIPLO_MESSAGE_REPEAT_TRADE:
		strText = GetDiploTextFromTag("RESPONSE_REPEAT_TRADE");
		break;

		// AI accepts a generous trade offer
	case DIPLO_MESSAGE_TRADE_ACCEPT_GENEROUS:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_ACCEPT_GENEROUS");
		break;

		// AI accepts a reasonable trade offer
	case DIPLO_MESSAGE_TRADE_ACCEPT_ACCEPTABLE:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_ACCEPT_ACCEPTABLE");
		break;

		// Human gave into AI demand
	case DIPLO_MESSAGE_TRADE_ACCEPT_AI_DEMAND:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_ACCEPT_AI_DEMAND");
		break;

		// AI accepts concessions from the human
	case DIPLO_MESSAGE_TRADE_ACCEPT_HUMAN_CONCESSIONS:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_ACCEPT_HUMAN_CONCESSIONS");
		break;

		// AI rejects an unreasonable trade offer
	case DIPLO_MESSAGE_TRADE_REJECT_UNACCEPTABLE:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_REJECT_UNACCEPTABLE");
		break;

		// AI rejects an insulting trade offer
	case DIPLO_MESSAGE_TRADE_REJECT_INSULTING:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_REJECT_INSULTING");
		break;

		// AI sees a good deal it doesn't need to change
	case DIPLO_MESSAGE_TRADE_DEAL_UNCHANGED:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_DEAL_UNCHANGED");
		break;

		// AI offers a counter-deal
	case DIPLO_MESSAGE_TRADE_AI_MAKES_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_AI_MAKES_OFFER");
		break;

		// AI doesn't see a way to make proposed deal work
	case DIPLO_MESSAGE_TRADE_NO_DEAL_POSSIBLE:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_NO_DEAL_POSSIBLE");
		break;

	case DIPLO_MESSAGE_TRADE_CANT_MATCH_OFFER:
		strText = GetDiploTextFromTag("RESPONSE_TRADE_CANT_MATCH_OFFER");
		break;

		//////////////////////////////////////////////////////////////
		// Human declared war on AI, what is the AI's response?
		//////////////////////////////////////////////////////////////

		// AI attacked by human player
	case DIPLO_MESSAGE_ATTACKED_ROOT:
		strText = GetAttackedByHumanMessage();
		break;

		// Attacked: Hostile response
	case DIPLO_MESSAGE_ATTACKED_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_HOSTILE");
		break;

		// Attacked: Hostile response (Weak player)
	case DIPLO_MESSAGE_ATTACKED_WEAK_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_WEAK_HOSTILE");
		break;

		// Attacked: Hostile response (Strong player)
	case DIPLO_MESSAGE_ATTACKED_STRONG_HOSTILE:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_STRONG_HOSTILE");
		break;

		// Attacked: Excited response
	case DIPLO_MESSAGE_ATTACKED_EXCITED:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_EXCITED");
		break;

		// Attacked: Excited response (Weak Player)
	case DIPLO_MESSAGE_ATTACKED_WEAK_EXCITED:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_WEAK_EXCITED");
		break;

		// Attacked: Excited response (Strong player)
	case DIPLO_MESSAGE_ATTACKED_STRONG_EXCITED:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_STRONG_EXCITED");
		break;

		// Attacked: Sad response
	case DIPLO_MESSAGE_ATTACKED_SAD:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_SAD");
		break;

		// Attacked: Betrayed response
	case DIPLO_MESSAGE_ATTACKED_BETRAYED:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_BETRAYED");
		break;

		// AI DoW:  response
	case DIPLO_MESSAGE_ATTACKED_MILITARY_PROMISE_BROKEN:
		strText = GetDiploTextFromTag("RESPONSE_ATTACKED_MILITARY_PROMISE_BROKEN");
		break;

		//////////////////////////////////////////////////////////////
		// AI is declaring war on human, what does he say?
		//////////////////////////////////////////////////////////////

		// AI declaring war on human
	case DIPLO_MESSAGE_DOW_ROOT:
		strText = GetWarMessage(eForPlayer);
		break;

		// AI DoW:  response
	case DIPLO_MESSAGE_DOW_GENERIC:
		strText = GetDiploTextFromTag("RESPONSE_DOW_GENERIC");
		break;

		// AI DoW:  response
	case DIPLO_MESSAGE_DOW_LAND:
		strText = GetDiploTextFromTag("RESPONSE_DOW_LAND");
		break;

		// AI DoW:  response
	case DIPLO_MESSAGE_DOW_WORLD_CONQUEST:
		strText = GetDiploTextFromTag("RESPONSE_DOW_WORLD_CONQUEST");
		break;

		// AI DoW:  response
	case DIPLO_MESSAGE_DOW_OPPORTUNITY:
		strText = GetDiploTextFromTag("RESPONSE_DOW_OPPORTUNITY");
		break;

		// AI DoW:  response
	case DIPLO_MESSAGE_DOW_DESPERATE:
		strText = GetDiploTextFromTag("RESPONSE_DOW_DESPERATE");
		break;

		// AI DoW:  response
	case DIPLO_MESSAGE_DOW_BETRAYAL:
		strText = GetDiploTextFromTag("RESPONSE_DOW_BETRAYAL");
		break;

		// AI DoW:  response
	case DIPLO_MESSAGE_DOW_WEAK_BETRAYAL:
		strText = GetDiploTextFromTag("RESPONSE_DOW_WEAK_BETRAYAL");
		break;

		// AI DoW:  response
	case DIPLO_MESSAGE_DOW_REGRET:
		strText = GetDiploTextFromTag("RESPONSE_DOW_REGRET");
		break;

		// AI declares war because human refused to give in to a demand
	case DIPLO_MESSAGE_WAR_DEMAND_REFUSED:
		strText = GetDiploTextFromTag("RESPONSE_WAR_DEMAND_REFUSED");
		break;

		/////////////////////////////////
		// Post Civ 5 Release Mish-Mash of stuff
		/////////////////////////////////

		// AI asks a player to denounce another player
	case DIPLO_MESSAGE_DOF_AI_DENOUNCE_REQUEST:
		strText = GetDiploTextFromTag("RESPONSE_DOF_AI_DENOUNCE_REQUEST", szOptionalKey1);
		break;

		// AI asks a player to declare war on another player
	case DIPLO_MESSAGE_DOF_AI_WAR_REQUEST:
		strText = GetDiploTextFromTag("RESPONSE_DOF_AI_WAR_REQUEST", szOptionalKey1);
		break;

		// AI asked the player to do something, but they didn't and now the AI is pissed
	case DIPLO_MESSAGE_DOF_NOT_HONORED:
		strText = GetDiploTextFromTag("RESPONSE_DOF_NOT_HONORED");
		break;

		// AI denouncing the human, who had been a friend
	case DIPLO_MESSAGE_AI_DOF_BACKSTAB:
		strText = GetDiploTextFromTag("RESPONSE_AI_DOF_BACKSTAB");
		break;

		// AI response to being denounced by the human
	case DIPLO_MESSAGE_RESPONSE_TO_BEING_DENOUNCED:
		strText = GetDiploTextFromTag("RESPONSE_RESPONSE_TO_BEING_DENOUNCED");
		break;

		// AI response to human DoFing a friend
	case DIPLO_MESSAGE_HUMAN_DOFED_FRIEND:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DOFED_FRIEND", szOptionalKey1);
		break;

		// AI response to human DoFing an enemy
	case DIPLO_MESSAGE_HUMAN_DOFED_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DOFED_ENEMY", szOptionalKey1);
		break;

		// AI response to human Denouncing a friend
	case DIPLO_MESSAGE_HUMAN_DENOUNCED_FRIEND:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DENOUNCED_FRIEND", szOptionalKey1);
		break;

		// AI response to human Denouncing an enemy
	case DIPLO_MESSAGE_HUMAN_DENOUNCED_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DENOUNCED_ENEMY", szOptionalKey1);
		break;

		// AI DoFed someone because the human DoFed them (nice)
	case DIPLO_MESSAGE_HUMAN_DOF_SO_AI_DOF:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DOF_SO_AI_DOF", szOptionalKey1);
		break;

		// AI Denounced someone because the human Denounced them (nice)
	case DIPLO_MESSAGE_HUMAN_DENOUNCE_SO_AI_DENOUNCE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DENOUNCE_SO_AI_DENOUNCE", szOptionalKey1);
		break;

		// AI Denounced someone because the human DoFed them (mean)
	case DIPLO_MESSAGE_HUMAN_DOF_SO_AI_DENOUNCE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DOF_SO_AI_DENOUNCE", szOptionalKey1);
		break;

		// AI DoFed someone because the human Denounced them (mean)
	case DIPLO_MESSAGE_HUMAN_DENOUNCE_SO_AI_DOF:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_DENOUNCE_SO_AI_DOF", szOptionalKey1);
		break;

		// Saying hello - AI denounced the human
	case DIPLO_MESSAGE_GREETING_DENOUNCED_BY_AI:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DENOUNCED_BY_AI");
		break;

		// Saying hello - human denounced the AI
	case DIPLO_MESSAGE_GREETING_DENOUNCED_AI:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DENOUNCED_AI");
		break;

		// Saying hello - Human has a DoF with an enemy of the AI's
	case DIPLO_MESSAGE_GREETING_OUR_DOF_WITH_AI_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_OUR_DOF_WITH_ENEMY_OF_AI", szOptionalKey1);
		break;

		// Saying hello - Human has a DoF with a friend of the AI's
	case DIPLO_MESSAGE_GREETING_OUR_DOF_WITH_AI_FRIEND:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_OUR_DOF_WITH_FRIEND_OF_AI", szOptionalKey1);
		break;

		// Saying hello - Human has denounced a friend of the AI's
	case DIPLO_MESSAGE_GREETING_DENOUNCED_AI_FRIEND:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DENOUNCED_FRIEND_OF_AI", szOptionalKey1);
		break;

		// Saying hello - Human has denounced an enemy of the AI's
	case DIPLO_MESSAGE_GREETING_DENOUNCED_AI_ENEMY:
		strText = GetDiploTextFromTag("RESPONSE_GREETING_DENOUNCED_ENEMY_OF_AI", szOptionalKey1);
		break;

		// Human asked for a DoF, but the AI hasn't known him for long enough
	case DIPLO_MESSAGE_TOO_SOON_FOR_DOF:
		strText = GetDiploTextFromTag("RESPONSE_TOO_SOON_FOR_DOF");
		break;

		// Same late game policy tree
	case DIPLO_MESSAGE_SAME_POLICIES_FREEDOM:
		strText = GetDiploTextFromTag("RESPONSE_SAME_POLICIES_FREEDOM");
		break;
	case DIPLO_MESSAGE_SAME_POLICIES_ORDER:
		strText = GetDiploTextFromTag("RESPONSE_SAME_POLICIES_ORDER");
		break;
	case DIPLO_MESSAGE_SAME_POLICIES_AUTOCRACY:
		strText = GetDiploTextFromTag("RESPONSE_SAME_POLICIES_AUTOCRACY");
		break;

		/////////////////////////////////
		// Espionage messages
		/////////////////////////////////

		// AI response to catching one of your spies
	case DIPLO_MESSAGE_CAUGHT_YOUR_SPY:
		strText = GetDiploTextFromTag("RESPONSE_CAUGHT_YOUR_SPY");
		break;

		// AI response to killing one of your spies
	case DIPLO_MESSAGE_KILLED_YOUR_SPY:
		strText = GetDiploTextFromTag("RESPONSE_KILLED_YOUR_SPY");
		break;

		// AI response to you killing one of their spies
	case DIPLO_MESSAGE_KILLED_MY_SPY:
		strText = GetDiploTextFromTag("RESPONSE_KILLED_MY_SPY");
		break;

	case DIPLO_MESSAGE_CONFRONT_YOU_KILLED_MY_SPY:
		strText = GetDiploTextFromTag("RESPONSE_CONFRONT_YOU_KILLED_MY_SPY");
		break;

		// AI response to you killing one of their spies
	case DIPLO_MESSAGE_STOP_CONVERSIONS:
		strText = GetDiploTextFromTag("RESPONSE_STOP_CONVERSIONS");
		break;

	case DIPLO_MESSAGE_HUMAN_CAUGHT_YOUR_SPY_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_CAUGHT_YOUR_SPY_GOOD");
		break;

	case DIPLO_MESSAGE_HUMAN_CAUGHT_YOUR_SPY_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_CAUGHT_YOUR_SPY_BAD");
		break;

		// Human asks us not to spy him, and we say yes
	case DIPLO_MESSAGE_STOP_SPYING_YES:
		strText = GetDiploTextFromTag("RESPONSE_STOP_SPYING_YES");
		break;

		// Human asks us not to spy on him, but we say no
	case DIPLO_MESSAGE_STOP_SPYING_NO:
		strText = GetDiploTextFromTag("RESPONSE_STOP_SPYING_NO");
		break;

	case DIPLO_MESSAGE_WARNED_ABOUT_INTRIGUE:
		strText = GetDiploTextFromTag("RESPONSE_WARNED_ABOUT_INTRIGUE", szOptionalKey1);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_DECEPTION", szOptionalKey1);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_KNOWN_CITY:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_KNOWN_CITY", szOptionalKey1, szOptionalKey2);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_UNKNOWN_CITY:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_ARMY_SNEAK_ATTACK_UNKNOWN_CITY", szOptionalKey1);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_KNOWN_CITY:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_KNOWN_CITY", szOptionalKey1, szOptionalKey2);
		break;

	case DIPLO_MESSAGE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_UNKNOWN_CITY:
		strText = GetDiploTextFromTag("RESPONSE_SHARE_INTRIGUE_AMPHIBIOUS_SNEAK_ATTACK_UNKNOWN_CITY", szOptionalKey1);
		break;

		// Human catches enemy spy and does not forgive the thief
	case DIPLO_MESSAGE_HUMAN_KILLED_MY_SPY_UNFORGIVEN:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_KILLED_MY_SPY_UNFORGIVEN");
		break;

		// Human catches enemy spy and forgives the thief
	case DIPLO_MESSAGE_HUMAN_KILLED_MY_SPY_FORGIVEN:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_KILLED_MY_SPY_FORGIVEN");
		break;

	case DIPLO_MESSAGE_HUMAN_STOP_CONVERSIONS_GOOD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_CONVERSIONS_GOOD");
		break;

	case DIPLO_MESSAGE_HUMAN_STOP_CONVERSIONS_BAD:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_CONVERSIONS_BAD");
		break;

	// human asks AI to stop sending missionaries and prophets. The AI player agrees.
	case DIPLO_MESSAGE_STOP_CONVERSIONS_AGREE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_CONVERSIONS_AGREE");
		break;

	// human asks AI to stop sending missionaries and prophets. The AI player disagrees.
	case DIPLO_MESSAGE_STOP_CONVERSIONS_DISAGREE:
		strText = GetDiploTextFromTag("RESPONSE_HUMAN_STOP_CONVERSIONS_DISAGREE");
		break;

		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////

		// Should always have a state we're handling
	default:
		strText = "NO MESSAGE. Trying to get Diplo string.  Something has gone wrong, somehow.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.";
		CvAssert(false);
		break;
	}

	return strText;
}


/// Message from UI to gameplay about something that should happen with regards to diplomacy
void CvDiplomacyAI::DoFromUIDiploEvent(PlayerTypes eFromPlayer, FromUIDiploEventTypes eEvent, int iArg1, int iArg2)
{
	CvAssertMsg(eEvent >= 0, "DIPLOMACY_AI: Invalid FromUIDiploEventType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eEvent < NUM_FROM_UI_DIPLO_EVENTS, "DIPLOMACY_AI: Invalid FromUIDiploEventType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "";

	TeamTypes eFromTeam = GET_PLAYER(eFromPlayer).getTeam();

	// Are we processing this message on the active player's computer?
	bool bActivePlayer = GC.getGame().getActivePlayer() == eFromPlayer;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	switch(eEvent)
	{
		// *********************************************
		// Player declares war on the AI
		// *********************************************
	case FROM_UI_DIPLO_EVENT_HUMAN_DECLARES_WAR:
	{
		GET_TEAM(eFromTeam).declareWar(GetTeam());

		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_ROOT);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN, strText, LEADERHEAD_ANIM_ATTACKED, iArg1);
		}

		break;
	}

	// *********************************************
	// Player requests peace
	// *********************************************
	case FROM_UI_DIPLO_EVENT_HUMAN_NEGOTIATE_PEACE:
	{
		if(bActivePlayer)
		{
			PeaceTreatyTypes ePeaceTreatyImWillingToOffer = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToOffer(eFromPlayer);
			PeaceTreatyTypes ePeaceTreatyImWillingToAccept = GetPlayer()->GetDiplomacyAI()->GetTreatyWillingToAccept(eFromPlayer);

			// Does the AI actually want peace?
			if(IsWillingToMakePeaceWithHuman(eFromPlayer) &&
			        ePeaceTreatyImWillingToOffer >= PEACE_TREATY_WHITE_PEACE && ePeaceTreatyImWillingToAccept >= PEACE_TREATY_WHITE_PEACE)
			{
				//if (IsWantsPeaceWithPlayer(eFromPlayer))
				//{
				//	gDLL->sendChangeWar(GetTeam(), /*bWar*/ false);

				// Now add peace items to the UI deal so that it's ready for us to make an offer
				//pUIDeal->ClearItems();
				//pUIDeal->SetPlayer1(eFromPlayer);	// The order of these is very important!
				//pUIDeal->SetPlayer2(eMyPlayer);	// The order of these is very important!
				//pUIDeal->AddPeaceTreaty(eMyPlayer, GC.getPEACE_TREATY_LENGTH());
				//pUIDeal->AddPeaceTreaty(eFromPlayer, GC.getPEACE_TREATY_LENGTH());

				// This is essentially the same as the human opening the trade screen
				GetPlayer()->GetDealAI()->DoTradeScreenOpened();

				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PEACE_WHAT_WILL_HUMAN_OFFER);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_TRADE, strText, LEADERHEAD_ANIM_LETS_HEAR_IT);
			}
			else
			{
				// Player declared war and wants peace right away.  Uh huh, right.
				if(GetPlayerNumTurnsAtWar(eFromPlayer) < 1)
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_TOO_SOON_NO_PEACE);
				// Don't want peace for some other reason
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_NO_PEACE);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_WAR_DECLARED_BY_HUMAN, strText, LEADERHEAD_ANIM_NO);
			}
		}

		break;
	}

	// *********************************************
	// Player wants to discuss something with the AI
	// *********************************************
	case FROM_UI_DIPLO_EVENT_HUMAN_WANTS_DISCUSSION:
	{
		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_LETS_HEAR_IT);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_LETS_HEAR_IT);
		}

		break;
	}

	// *********************************************
	// Player told the AI to not settle near him
	// *********************************************
	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_DONT_SETTLE:
	{
		// Player has asked this before
		if(IsDontSettleMessageTooSoon(eFromPlayer))
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_NO);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
			}
		}
		// AI gives a new answer
		else
		{
			SetPlayerNoSettleRequestCounter(eFromPlayer, 0);

			bool bAcceptable;

			// Teammates
			if(GetPlayer()->getTeam() == GET_PLAYER(eFromPlayer).getTeam())
				bAcceptable = true;
			// Not teammates
			else
				bAcceptable = IsDontSettleAcceptable(eFromPlayer);

			if(bAcceptable)
				SetPlayerNoSettleRequestAccepted(eFromPlayer, true);

			if(bActivePlayer)
			{
				if(bAcceptable)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_DONT_SETTLE_YES);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NEGATIVE);
				}
				else
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_DONT_SETTLE_NO);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
				}
			}
		}

		break;
	}

	// *********************************************
	// Player told the AI to not spy on him
	// *********************************************
	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPYING:
	{
		// Player has asked this before
		if(IsStopSpyingMessageTooSoon(eFromPlayer))
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_NO);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
			}
		}
		// AI gives a new answer
		else
		{
			SetPlayerStopSpyingRequestCounter(eFromPlayer, 0);

			bool bAcceptable;

			// Teammates
			if(GetPlayer()->getTeam() == GET_PLAYER(eFromPlayer).getTeam())
			{
				bAcceptable = true;
			}
			// Not teammates
			else
			{
				bAcceptable = IsStopSpyingAcceptable(eFromPlayer);
			}

			if(bAcceptable)
			{
				SetPlayerStopSpyingRequestAccepted(eFromPlayer, true);
				m_pPlayer->GetEspionageAI()->EvaluateSpiesAssignedToTargetPlayer(eFromPlayer);
			}

			if(bActivePlayer)
			{
				DiploUIStateTypes eStateType = DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED;
				if(iArg1 == 1)
				{
					eStateType = DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT;
				}

				LeaderheadAnimationTypes eAnimType;

				if(bAcceptable)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_SPYING_YES);
					eAnimType = LEADERHEAD_ANIM_NEGATIVE;
				}
				else
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_SPYING_NO);
					eAnimType = LEADERHEAD_ANIM_NO;
				}

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, eStateType, strText, eAnimType);
			}
		}

		break;
	}

	// *********************************************
	// Player told the AI to not spread their religion
	// *********************************************
	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_STOP_SPREADING_RELIGION:
	{
		if (bActivePlayer)
		{	
			SetPlayerAskedNotToConvert(eFromPlayer, true);
			DiploUIStateTypes eStateType = DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED;
			if(iArg1 == 1)
			{
				eStateType = DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT;
			}

			LeaderheadAnimationTypes eAnimType;

			bool bIsYes = IsStopSpreadingReligionAcceptable(eFromPlayer);
			if (bIsYes)
			{					
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_CONVERSIONS_AGREE);
				eAnimType = LEADERHEAD_ANIM_NEGATIVE;
			}
			else
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_STOP_CONVERSIONS_DISAGREE);
				eAnimType = LEADERHEAD_ANIM_NO;
			}

			SetPlayerAgreeNotToConvert(eFromPlayer, bIsYes);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, eStateType, strText, eAnimType);
		}
		break;
	}

	// *********************************************
	// Player asked if the AI will work with him
	// *********************************************
	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_WORK_WITH_US:
	{
		// AI hasn't known the human for long enough yet
		if(IsTooEarlyForDoF(eFromPlayer))
		{
			SetDoFCounter(eFromPlayer, 0);
			GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, 0);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_TOO_SOON_FOR_DOF);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
			}
		}
		// Player has asked this before
		else if(IsDoFMessageTooSoon(eFromPlayer))
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_NO);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
			}
		}
		// AI gives a new answer
		else
		{
			SetDoFCounter(eFromPlayer, 0);
			GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, 0);

			bool bAcceptable = IsDoFAcceptable(eFromPlayer);
			if(bAcceptable)
			{
				SetDoFAccepted(eFromPlayer, true);
				GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFAccepted(eMyPlayer, true);
			}

			if(bActivePlayer)
			{
				if(bAcceptable)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_WITH_US_YES);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_POSITIVE);
				}
				else
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_WORK_WITH_US_NO);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_NO);
				}
			}
		}

		break;
	}

	// *********************************************
	// Player said he's done working with the AI
	// *********************************************
	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_END_WORK_WITH_US:
	{
		SetDoFAccepted(eFromPlayer, false);
		GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFAccepted(eMyPlayer, false);
		SetDoFCounter(eFromPlayer, -666);
		GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, -666);

		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
		}

		break;
	}

	// *********************************************
	// Human refused to give in to demand
	// *********************************************
	case FROM_UI_DIPLO_EVENT_DEMAND_HUMAN_REFUSAL:
	{
		// Does the AI declare war?
		bool bDeclareWar = false;

		if(GC.getGame().getJonRandNum(100, "Human demand refusal war rand.") < 50)
		{
			bDeclareWar = true;

			GET_TEAM(GetTeam()).declareWar(GET_PLAYER(eFromPlayer).getTeam());
			m_pPlayer->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_NOW_AT_WAR);
			LogWarDeclaration(eFromPlayer);

			GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eFromPlayer, 3);
		}

		if(bActivePlayer)
		{
			if(bDeclareWar)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_WAR_DEMAND_REFUSED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
			else
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_SO_BE_IT);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		break;
	}

	// *********************************************
	// Human refused to give in to a request
	// *********************************************
	case FROM_UI_DIPLO_EVENT_REQUEST_HUMAN_REFUSAL:
	{
		GetPlayer()->GetDiplomacyAI()->ChangeRecentAssistValue(eFromPlayer, iArg1);

		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_HUMAN, strText, LEADERHEAD_ANIM_NEGATIVE);
		}

		break;
	}

	// *********************************************
	// AI is telling us he sees our military buildup on his borders
	// *********************************************
	case FROM_UI_DIPLO_EVENT_AGGRESSIVE_MILITARY_WARNING_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		// Human says he means no harm
		if(iArg1 == 1)
		{
			SetPlayerMadeMilitaryPromise(eFromPlayer, true);
			SetPlayerMilitaryPromiseCounter(eFromPlayer, 0);

			if(bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_GOOD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_AGGRESSIVE_MILITARY_WARNING_GOOD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}
		// Human told the AI to die
		else if(iArg1 == 2)
		{
			GET_TEAM(eFromTeam).declareWar(GetTeam());

			SetPlayerIgnoredMilitaryPromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_AGGRESSIVE_MILITARY_WARNING_BAD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_AGGRESSIVE_MILITARY_WARNING_BAD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}

		break;
	}

	// *********************************************
	// AI has attacked a Minor the human is friends with, what did the human say in response?
	// *********************************************
	case FROM_UI_DIPLO_EVENT_I_ATTACKED_YOUR_MINOR_CIV_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID,  iArg2 is MINOR ID from DiscussionDialog.lua
		PlayerTypes eMinor = (PlayerTypes) iArg2;
		CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
		CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
		if (!(eMinor >= MAX_MAJOR_CIVS && eMinor < MAX_CIV_PLAYERS))
		{
			// Fail gracefully, allow UI to continue
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			break;
		}

		CvPlayer* pMinor = &GET_PLAYER(eMinor);
		CvAssertMsg(pMinor, "Error triggering gameplay effects for diplomacy event involving a Protected city-state. Please send Anton your save file and version.");
		if (!pMinor)
		{
			// Fail gracefully, allow UI to continue
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			break;
		}
		
		CvAssertMsg(pMinor->GetMinorCivAI()->IsProtectedByMajor(eFromPlayer), "Diplomacy event involving a Protected city-state when there was no Pledge active. Please send Anton your save file and Version.");

		// Human says he forgives the AI
		if(iArg1 == 1)
		{
			GC.getGame().DoMinorPledgeProtection(eFromPlayer, eMinor, false, true); // Pledge is broken!
			
			if (bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_GOOD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_WE_ATTACKED_MINOR_GOOD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}
		// Human said he'd get revenge
		else if(iArg1 == 2)
		{
			SetOtherPlayerTurnsSinceSidedWithProtectedMinor(eFromPlayer, 0);

			if (bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_WE_ATTACKED_MINOR_BAD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}

	// *********************************************
	// AI has bullied a Minor the human has a PtP with, what did the human say in response?
	// *********************************************
	case FROM_UI_DIPLO_EVENT_I_BULLIED_YOUR_MINOR_CIV_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID,  iArg2 is MINOR ID from DiscussionDialog.lua
		PlayerTypes eMinor = (PlayerTypes) iArg2;
		CvAssertMsg(eMinor >= MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
		CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
		if (!(eMinor >= MAX_MAJOR_CIVS && eMinor < MAX_CIV_PLAYERS))
		{
			// Fail gracefully, allow UI to continue
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			break;
		}
		
		CvPlayer* pMinor = &GET_PLAYER(eMinor);
		CvAssertMsg(pMinor, "Error triggering gameplay effects for breaking a Pledge to Protect a city-state. Please send Anton your save file and version.");
		if (!pMinor)
		{
			// Fail gracefully, allow UI to continue
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_ATTACKED_MINOR_BAD);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			break;
		}
		
		CvAssertMsg(pMinor->GetMinorCivAI()->IsProtectedByMajor(eFromPlayer), "Diplomacy event involving a Protected city-state when there was no Pledge active. Please send Anton your save file and Version.");

		// Human says he forgives the AI
		if(iArg1 == 1)
		{
			GC.getGame().DoMinorPledgeProtection(eFromPlayer, eMinor, false, true); // Pledge is broken!
		
			if (bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_BULLIED_MINOR_GOOD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_WE_BULLIED_MINOR_GOOD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}
		// Human said he'd get revenge
		else if(iArg1 == 2)
		{
			SetOtherPlayerTurnsSinceSidedWithProtectedMinor(eFromPlayer, 0);

			if (bActivePlayer)
			{
				if(IsActHostileTowardsHuman(eFromPlayer))
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_HOSTILE_WE_BULLIED_MINOR_BAD);
				else
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_WE_BULLIED_MINOR_BAD);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}

	// *********************************************
	// AI warned human about attacking a protected Minor
	// *********************************************
	case FROM_UI_DIPLO_EVENT_ATTACKED_MINOR_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		// Human told the AI it's none of his business
		if(iArg1 == 1)
		{
			SetPlayerIgnoredAttackCityStatePromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_ATTACKED_MINOR_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
		// Human said he'd withdraw
		else if(iArg1 == 2)
		{
			SetPlayerMadeAttackCityStatePromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_ATTACKED_MINOR_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}

	// *********************************************
	// AI is mad at human for killing a protected Minor
	// *********************************************
	case FROM_UI_DIPLO_EVENT_KILLED_MINOR_RESPONSE:
	{
		break;
	}

	// *********************************************
	// AI warned human about bullying a protected Minor
	// *********************************************
	case FROM_UI_DIPLO_EVENT_BULLIED_MINOR_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		// Human told the AI it's none of his business
		if(iArg1 == 1)
		{
			SetPlayerIgnoredBullyCityStatePromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_BULLIED_MINOR_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
		// Human said he'd withdraw
		else if(iArg1 == 2)
		{
			SetPlayerMadeBullyCityStatePromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_BULLIED_MINOR_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}

	// *********************************************
	// AI is seriously warning the human about his expansion
	// *********************************************
	case FROM_UI_DIPLO_EVENT_EXPANSION_SERIOUS_WARNING_RESPONSE:
	{
		break;
	}

	// *********************************************
	// AI warned human about his expansion
	// *********************************************
	case FROM_UI_DIPLO_EVENT_EXPANSION_WARNING_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		// Human told the AI it's none of his business
		if(iArg1 == 1)
		{
			if (!IsPlayerIgnoredExpansionPromise(eFromPlayer))
			{
				SetIgnoredExpansionPromiseValue(eFromPlayer, GC.getOPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
			}
			SetPlayerIgnoredExpansionPromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_EXPANSION_WARNING_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
		// Human said he wouldn't settle near us again
		else if(iArg1 == 2)
		{
			SetPlayerMadeExpansionPromise(eFromPlayer, true);
			SetPlayerExpansionPromiseData(eFromPlayer, GetExpansionAggressivePosture(eFromPlayer));

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_EXPANSION_WARNING_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}

	// *********************************************
	// AI is seriously warning the human about his plot buying
	// *********************************************
	case FROM_UI_DIPLO_EVENT_PLOT_BUYING_SERIOUS_WARNING_RESPONSE:
	{
		break;
	}

	// *********************************************
	// AI warned human about his plot buying
	// *********************************************
	case FROM_UI_DIPLO_EVENT_PLOT_BUYING_WARNING_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		// Human told the AI it's none of his business
		if(iArg1 == 1)
		{
			if (!IsPlayerIgnoredBorderPromise(eFromPlayer))
			{
				SetIgnoredBorderPromiseValue(eFromPlayer, GC.getOPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
			}
			SetPlayerIgnoredBorderPromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_PLOT_BUYING_WARNING_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
		// Human said he wouldn't buy land near us again
		else if(iArg1 == 2)
		{
			SetPlayerMadeBorderPromise(eFromPlayer, true);
			SetPlayerBorderPromiseData(eFromPlayer, GetPlotBuyingAggressivePosture(eFromPlayer));

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_PLOT_BUYING_WARNING_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}

	// *********************************************
	// AI asked human if he'd like to work together
	// *********************************************
	case FROM_UI_DIPLO_EVENT_WORK_WITH_US_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		SetDoFCounter(eFromPlayer, 0);
		GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, 0);

		// Human agrees
		if(iArg1 == 1)
		{
			SetDoFAccepted(eFromPlayer, true);
			GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetDoFAccepted(eMyPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}
		// Human says sorry, no
		else if(iArg1 == 2)
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}

	// *********************************************
	// AI asked human if he'd like to work against someone
	// *********************************************
	//case FROM_UI_DIPLO_EVENT_WORK_AGAINST_SOMEONE_RESPONSE:
	//	{
	//		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

	//		PlayerTypes eAgainstPlayer = (PlayerTypes) iArg2;

	//		SetWorkingAgainstPlayerCounter(eFromPlayer, eAgainstPlayer, 0);
	//		GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetWorkingAgainstPlayerCounter(eMyPlayer, eAgainstPlayer, 0);

	//		// Human says sorry, no
	//		if (iArg1 == 1 || iArg1 == 2)
	//		{
	//			if (bActivePlayer)
	//			{
	//				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
	//				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
	//			}
	//		}
	//		// Human agrees
	//		else if (iArg1 == 3)
	//		{
	//			SetWorkingAgainstPlayerAccepted(eFromPlayer, eAgainstPlayer, true);
	//			GET_PLAYER(eFromPlayer).GetDiplomacyAI()->SetWorkingAgainstPlayerAccepted(eMyPlayer, eAgainstPlayer, true);

	//			if (bActivePlayer)
	//			{
	//				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
	//				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
	//			}
	//		}

	//		break;
	//	}

	// *********************************************
	// Human denounced us!
	// *********************************************
	case FROM_UI_DIPLO_EVENT_DENOUNCE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		GET_PLAYER(eFromPlayer).GetDiplomacyAI()->DoDenouncePlayer(eMyPlayer);

		if(bActivePlayer)
		{
			strText = GetDiploStringForMessage(DIPLO_MESSAGE_RESPONSE_TO_BEING_DENOUNCED);
			gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
		}

		break;
	}

	// *********************************************
	// Human asked AI to go to war against someone
	// *********************************************
	case FROM_UI_DIPLO_EVENT_COOP_WAR_OFFER:
	{
		// **** NOTE **** - iArg1 is Player ID from DiscussionDialog.lua

		PlayerTypes eAgainstPlayer = (PlayerTypes) iArg1;

		// Too soon?
		if(IsCoopWarMessageTooSoon(eFromPlayer, eAgainstPlayer))
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_REPEAT_NO);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}
		else
		{
			// Set the counter (this will handle rejection as well)
			SetCoopWarCounter(eFromPlayer, eAgainstPlayer, 0);

			CoopWarStates eAcceptableState;

			// Teammates
			if(GetPlayer()->getTeam() == GET_PLAYER(eFromPlayer).getTeam())
				eAcceptableState = COOP_WAR_STATE_SOON;
			// Not teammates
			else
				eAcceptableState = GetWillingToAgreeToCoopWarState(eFromPlayer, eAgainstPlayer);

			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, eAcceptableState);

			// Accepted
			if(eAcceptableState == COOP_WAR_STATE_ACCEPTED)
			{
				// AI declaration
				DeclareWar(eAgainstPlayer);
				GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);

				// Human declaration
				TeamTypes eAgainstTeam = GET_PLAYER(eAgainstPlayer).getTeam();
				GET_TEAM(eFromTeam).declareWar(eAgainstTeam);

				int iLockedTurns = /*15*/ GC.getCOOP_WAR_LOCKED_LENGTH();
				GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
				GET_TEAM(eFromTeam).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);

				if(bActivePlayer)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_YES);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
				}
			}
			// Soon
			else if(eAcceptableState == COOP_WAR_STATE_SOON)
			{
				if(bActivePlayer)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_SOON);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
				}
			}
			// Rejected
			else
			{
				if(bActivePlayer)
				{
					strText = GetDiploStringForMessage(DIPLO_MESSAGE_COOP_WAR_NO);
					gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
				}
			}
		}

		break;
	}

	// *********************************************
	// AI asked human if he'd declare war against someone
	// *********************************************
	case FROM_UI_DIPLO_EVENT_COOP_WAR_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		PlayerTypes eAgainstPlayer = (PlayerTypes) iArg2;

		SetCoopWarCounter(eFromPlayer, eAgainstPlayer, 0);

		// Human says sorry, no
		if(iArg1 == 1 || iArg1 == 2)
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}
		// Human says "soon"
		if(iArg1 == 3)
		{
			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, COOP_WAR_STATE_SOON);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}
		// Human agrees
		else if(iArg1 == 4)
		{
			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, COOP_WAR_STATE_ACCEPTED);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}

			// AI declaration
			DeclareWar(eAgainstPlayer);
			GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);

			// Human declaration
			TeamTypes eAgainstTeam = GET_PLAYER(eAgainstPlayer).getTeam();
			GET_TEAM(eFromTeam).declareWar(eAgainstTeam);

			int iLockedTurns = /*15*/ GC.getCOOP_WAR_LOCKED_LENGTH();
			GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
			GET_TEAM(eFromTeam).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
		}

		break;
	}

	// *********************************************
	// AI asked human if he'd declare war against someone earlier, and now it's time to put your money where your mouth is
	// *********************************************
	case FROM_UI_DIPLO_EVENT_COOP_WAR_NOW_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		PlayerTypes eAgainstPlayer = (PlayerTypes) iArg2;

		SetCoopWarCounter(eFromPlayer, eAgainstPlayer, 0);

		// Human agrees
		if(iArg1 == 1)
		{
			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, COOP_WAR_STATE_ACCEPTED);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}

			// AI declaration
			DeclareWar(eAgainstPlayer);
			GetPlayer()->GetMilitaryAI()->RequestBasicAttack(eAgainstPlayer, 1);

			// Human declaration
			TeamTypes eAgainstTeam = GET_PLAYER(eAgainstPlayer).getTeam();
			GET_TEAM(eFromTeam).declareWar(eAgainstTeam);

			int iLockedTurns = /*15*/ GC.getCOOP_WAR_LOCKED_LENGTH();
			GET_TEAM(GetPlayer()->getTeam()).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
			GET_TEAM(eFromTeam).ChangeNumTurnsLockedIntoWar(eAgainstTeam, iLockedTurns);
		}
		// Human says no
		if(iArg1 == 2)
		{
			SetCoopWarAcceptedState(eFromPlayer, eAgainstPlayer, NO_COOP_WAR_STATE);
			SetPlayerBrokenCoopWarPromise(eFromPlayer, true);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}

	// *********************************************
	// Human made a demand. NOTE: Unlike the other messages in here, this one is actually triggered from inside CvDealAI on a player's local machine
	// *********************************************
	case FROM_UI_DIPLO_EVENT_HUMAN_DEMAND:
	{
		// **** NOTE **** - iArg1 is RESPONSE TYPE from CvDealAI::DoHumanDemand()
		DemandResponseTypes eResponse = (DemandResponseTypes) iArg1;

		// THIS is the important part of the message - it seeds the demand timer on all players' machines
		DoDemandMade(eFromPlayer);

		if(bActivePlayer)
		{
			// Demand agreed to
			if(eResponse == DEMAND_RESPONSE_ACCEPT)
			{
				strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_YES);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT, strText, LEADERHEAD_ANIM_YES);
			}
			// Demand rebuffed
			else
			{
				if(eResponse == DEMAND_RESPONSE_REFUSE_WEAK)
					strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_WEAK);

				else if(eResponse == DEMAND_RESPONSE_REFUSE_HOSTILE)
					strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_HOSTILE);

				else if(eResponse == DEMAND_RESPONSE_REFUSE_TOO_MUCH)
					strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_TOO_MUCH);

				else if(eResponse == DEMAND_RESPONSE_REFUSE_TOO_SOON)
					strText = GetPlayer()->GetDiplomacyAI()->GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_DEMAND_REFUSE_TOO_SOON);

				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_RETURN_TO_ROOT, strText, LEADERHEAD_ANIM_NO);
			}
		}

		break;
	}

	// *********************************************
	// AI asked human if he'd like to make a Research Agreement in the future
	// *********************************************
	case FROM_UI_DIPLO_EVENT_PLAN_RA_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		// Human agrees
		if(iArg1 == 1)
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}
		// Human says sorry, no
		else if(iArg1 == 2)
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}

			DoCancelWantsResearchAgreementWithPlayer(eFromPlayer);
		}

		break;
	}

	// *********************************************
	// AI asked human if he'd denounce a third party
	// *********************************************
	case FROM_UI_DIPLO_EVENT_AI_REQUEST_DENOUNCE_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua
		PlayerTypes eAgainstPlayer = (PlayerTypes) iArg2;

		// Human agrees
		if(iArg1 == 1)
		{
			GET_PLAYER(eFromPlayer).GetDiplomacyAI()->DoDenouncePlayer(eAgainstPlayer);

			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_PLEASED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}
		// Human says sorry, no
		else if(iArg1 == 2)
		{
			// Oh, you're gonna say no, are you?
			if(IsFriendDenounceRefusalUnacceptable(eFromPlayer, eAgainstPlayer))
			{
				DoDenouncePlayer(eFromPlayer);
				LogDenounce(eFromPlayer, /*bBackstab*/ false, /*bRefusal*/ true);

				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DOF_NOT_HONORED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION_MEAN_AI, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
			else if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_DISAPPOINTED);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}

		break;
	}

	// *********************************************
	// AI warned human about spying
	// *********************************************
	case FROM_UI_DIPLO_EVENT_CAUGHT_YOUR_SPY_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		// Human told the AI it's none of his business
		if(iArg1 == 1)
		{
			SetPlayerIgnoredSpyPromise(eFromPlayer, true);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_CAUGHT_YOUR_SPY_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
		// Human said he'd withdraw
		else if(iArg1 == 2)
		{
			SetPlayerMadeSpyPromise(eFromPlayer, true);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_CAUGHT_YOUR_SPY_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}

	// *********************************************
	// Human warn AI about spying
	// *********************************************
	case FROM_UI_DIPLO_EVENT_KILLED_MY_SPY_RESPONSE:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		// Human did not forgive AI player
		if(iArg1 == 1)
		{
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_KILLED_MY_SPY_UNFORGIVEN);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_NEGATIVE);
			}
		}
		// Human forgave AI player
		else if(iArg1 == 2)
		{
			SetPlayerForgaveForSpying(eFromPlayer, true);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_KILLED_MY_SPY_FORGIVEN);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}

	case FROM_UI_DIPLO_EVENT_HUMAN_DISCUSSION_SHARE_INTRIGUE:
	{
		PlayerTypes ePlottingPlayer = (PlayerTypes)iArg1;
		CvIntrigueType eIntrigueType = (CvIntrigueType)iArg2;
		CvAssert(CvPlayerAI::IsValid(ePlottingPlayer));
		if (CvPlayerAI::IsValid(ePlottingPlayer))
		{
			ChangeNumTimesIntrigueSharedBy(eFromPlayer, 1);
			GET_PLAYER(eFromPlayer).GetEspionage()->MarkRecentIntrigueAsShared(eMyPlayer, ePlottingPlayer, eIntrigueType);
			if (bActivePlayer)
			{
				if(GET_PLAYER(eFromPlayer).GetEspionage()->HasSharedIntrigueAboutMe(eMyPlayer))
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_37);
				}

				bool bUsingXP1Scenario3 = gDLL->IsModActivated(CIV5_XP1_SCENARIO3_MODID);
				if(bUsingXP1Scenario3)
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_45);
				}

				strText = GetDiploStringForMessage(DIPLO_MESSAGE_WARNED_ABOUT_INTRIGUE, NO_PLAYER, GET_PLAYER(ePlottingPlayer).getCivilizationAdjectiveKey());
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_DISCUSS_HUMAN_INVOKED, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}
		break;
	}

	// *********************************************
	// AI warned about religious conversions
	// *********************************************
	case FROM_UI_DIPLO_EVENT_STOP_CONVERSIONS:
	{
		// **** NOTE **** - iArg1 is BUTTON ID from DiscussionDialog.lua

		// Human told the AI it's none of his business
		if(iArg1 == 1)
		{
			SetPlayerIgnoredNoConvertPromise(eFromPlayer, true);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_STOP_CONVERSIONS_BAD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_HATE_NEGATIVE);
			}
		}
		// Human said he'd withdraw
		else if(iArg1 == 2)
		{
			SetPlayerMadeNoConvertPromise(eFromPlayer, true);
			int iAdjustmentToJustBelowThreshold = GC.getRELIGION_DIPLO_HIT_THRESHOLD() - GetNegativeReligiousConversionPoints(eFromPlayer) - 1;
			ChangeNegativeReligiousConversionPoints(eFromPlayer, iAdjustmentToJustBelowThreshold);
			if(bActivePlayer)
			{
				strText = GetDiploStringForMessage(DIPLO_MESSAGE_HUMAN_STOP_CONVERSIONS_GOOD);
				gDLL->GameplayDiplomacyAILeaderMessage(eMyPlayer, DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
			}
		}

		break;
	}

	// Should always have a state we're handling
	default:
		CvAssert(false);
		break;
	}
}

/// Is the AI acting mean to the active human player?
bool CvDiplomacyAI::IsActHostileTowardsHuman(PlayerTypes eHuman)
{
	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(eHuman);
	MajorCivApproachTypes eVisibleApproach = GetMajorCivApproach(eHuman, /*bHideTrueFeelings*/ true);

	bool bAtWar = IsAtWar(eHuman);
	bool bAtWarButWantsPeace = bAtWar &&					// Have to be at war
	                           GetTreatyWillingToOffer(eHuman) >= PEACE_TREATY_WHITE_PEACE && GetTreatyWillingToAccept(eHuman) >= PEACE_TREATY_WHITE_PEACE &&		// High-level AI has to want peace
	                           IsWillingToMakePeaceWithHuman(eHuman);						// Special rules for peace with human (turn count) have to be met

	if(eVisibleApproach == MAJOR_CIV_APPROACH_HOSTILE)	// Hostile Approach
		return true;
	else if(bAtWar && !bAtWarButWantsPeace)		// At war and don't want peace
		return true;
	else if(eOpinion <= MAJOR_CIV_OPINION_ENEMY && eVisibleApproach != MAJOR_CIV_APPROACH_FRIENDLY)	// Enemy or worse, and not pretending to be friendly
		return true;

	return false;
}

/// AI is greeting the human - what does he say?
const char* CvDiplomacyAI::GetGreetHumanMessage(LeaderheadAnimationTypes& eAnimation)
{
	PlayerTypes eHuman = GC.getGame().getActivePlayer();
	CvPlayer* pHuman = &GET_PLAYER(eHuman);
	TeamTypes eHumanTeam = pHuman->getTeam();
	CvTeam* pHumanTeam = &GET_TEAM(eHumanTeam);

	MajorCivApproachTypes eVisibleApproach = GetMajorCivApproach(eHuman, /*bHideTrueFeelings*/ true);
	WarProjectionTypes eWarProjection = GetWarProjection(eHuman);
	DisputeLevelTypes eLandDispute = GetLandDisputeLevel(eHuman);
	AggressivePostureTypes eMilitaryPosture = GetMilitaryAggressivePosture(eHuman);
	AggressivePostureTypes eExpansionPosture = GetExpansionAggressivePosture(eHuman);
	AggressivePostureTypes ePlotBuyingPosture = GetPlotBuyingAggressivePosture(eHuman);
	StrengthTypes eMilitaryStrength = GetPlayerMilitaryStrengthComparedToUs(eHuman);
	StrengthTypes eEconomicStrength = GetPlayerEconomicStrengthComparedToUs(eHuman);
	int iNumPlayersAttacked = GetOtherPlayerNumMinorsAttacked(eHuman) + GetOtherPlayerNumMajorsAttacked(eHuman);
	int iNumPlayersKilled = GetOtherPlayerNumMinorsConquered(eHuman) + GetOtherPlayerNumMajorsConquered(eHuman);

	bool bAtWar = IsAtWar(eHuman);
	bool bAtWarButWantsPeace = bAtWar &&					// Have to be at war
	                           GetTreatyWillingToOffer(eHuman) >= PEACE_TREATY_WHITE_PEACE && GetTreatyWillingToAccept(eHuman) >= PEACE_TREATY_WHITE_PEACE &&		// High-level AI has to want peace
	                           IsWillingToMakePeaceWithHuman(eHuman);						// Special rules for peace with human (turn count) have to be met

	// Most Greetings are added to a vector to be picked from randomly
	// However, some are returned immediately, as they "fit" well enough that we DEFINITELY want to use that specific greeting
	FStaticVector<DiploMessageTypes, NUM_DIPLO_MESSAGE_TYPES, true, c_eCiv5GameplayDLL, 0> veValidGreetings;

	// Determine of the AI is being hostile to the player

	bool bHostile = IsActHostileTowardsHuman(eHuman);

	////////////////////////////////////////////
	// Pick Greeting Animation
	////////////////////////////////////////////

	if(bHostile)
		eAnimation = LEADERHEAD_ANIM_HATE_HELLO;
	else
		eAnimation = LEADERHEAD_ANIM_NEUTRAL_HELLO;

	////////////////////////////////////////////
	// Repeated Greetings
	////////////////////////////////////////////

	int iTimesScreenOpened = GC.GetEngineUserInterface()->GetStartDiploRepeatCount();

	if(iTimesScreenOpened > 4)
		return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_REPEAT_TOO_MUCH);

	else if(iTimesScreenOpened > 2)
	{
		if(bHostile)
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_HOSTILE_REPEAT);
		else
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_REPEAT);
	}

	////////////////////////////////////////////
	// War Greetings
	////////////////////////////////////////////

	// At war but wants peace
	if(bAtWarButWantsPeace)
	{
		if (eWarProjection == WAR_PROJECTION_DESTRUCTION)
		{
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_DESTRUCTION_LOOMS);
		}
		else if (GetTreatyWillingToAccept(eHuman) >= PEACE_TREATY_CAPITULATION)
		{
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_WILL_ACCEPT_SURRENDER);
		}
		else
		{
			return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_AT_WAR_WANTS_PEACE);
		}
	}
	else if(bAtWar)
		return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_AT_WAR_HOSTILE);

	////////////////////////////////////////////
	// Situational Greetings
	////////////////////////////////////////////

	// Working on a Research Agreement - JON: Disabled because this could be invoked if the AI wanted one without the human's knowledge
	//if (IsWantsResearchAgreementWithPlayer(eHuman))
	//	return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_RESEARCH_AGREEMENT);

	// Player has broken promises about playing nice militarily
	if(eVisibleApproach != MAJOR_CIV_APPROACH_FRIENDLY)
	{
		if(pHumanTeam->IsBrokenMilitaryPromise())
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_BROKEN_MILITARY_PROMISE);
	}

	// Players are working together
	if(IsDoFAccepted(eHuman))
		veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_WORKING_WITH);

	// Working against anyone?
	//for (int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
	//{
	//	// Must be alive
	//	if (!GET_PLAYER((PlayerTypes) iThirdPartyLoop).isAlive())
	//		continue;

	//	if (IsWorkingAgainstPlayerAccepted(eHuman, (PlayerTypes) iThirdPartyLoop))
	//		veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_WORKING_AGAINST);
	//}

	// Coop War against anyone?
	for(int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
	{
		// Must be alive
		if(!GET_PLAYER((PlayerTypes) iThirdPartyLoop).isAlive())
			continue;

		if(GetCoopWarAcceptedState(eHuman, (PlayerTypes) iThirdPartyLoop) >= COOP_WAR_STATE_SOON)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_COOP_WAR);
	}

	// Human at war with someone?
	if(pHumanTeam->getAtWarCount(true) > 0)
	{
		if(bHostile)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_AT_WAR);
		else
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HUMAN_AT_WAR);
	}

	// Military Aggressive Posture
	if(eMilitaryPosture >= AGGRESSIVE_POSTURE_MEDIUM)
		if(bHostile)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_MILITARY);
		else
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_AGGRESSIVE_MILITARY);

	// Land Dispute
	if(eLandDispute >= DISPUTE_LEVEL_STRONG)
	{
		// Expansion
		if(eExpansionPosture >= AGGRESSIVE_POSTURE_MEDIUM)
			if(bHostile)
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_EXPANSION);
			else
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_AGGRESSIVE_EXPANSION);

		// Plot buying
		if(ePlotBuyingPosture >= AGGRESSIVE_POSTURE_MEDIUM)
			if(bHostile)
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_AGGRESSIVE_PLOT_BUYING);
			else
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_AGGRESSIVE_PLOT_BUYING);
	}

	////////////////////////////////////////////
	// Friendly Greetings
	////////////////////////////////////////////

	if(!bHostile)
	{
		if(eVisibleApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		{
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_FRIENDLY_HELLO);

			// Military Strength
			if(eMilitaryStrength >= STRENGTH_STRONG)
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_FRIENDLY_STRONG_MILITARY);

			// Economic Strength
			if(eEconomicStrength >= STRENGTH_STRONG)
				veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_FRIENDLY_STRONG_ECONOMY);
		}
		else
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_NEUTRAL_HELLO);
	}

	////////////////////////////////////////////
	// Hostile Greetings
	////////////////////////////////////////////

	else
	{
		veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HELLO);

		// Human has at least 1 fewer City
		if(pHuman->getNumCities() < GetPlayer()->getNumCities())
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_FEW_CITIES);

		// Human's military is weak
		if(eMilitaryStrength <= STRENGTH_POOR)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_SMALL_ARMY);

		// Human has attacked/killed some folks
		if(iNumPlayersAttacked >= 2 || iNumPlayersKilled >= 1)
			veValidGreetings.push_back(DIPLO_MESSAGE_GREETING_HOSTILE_HUMAN_IS_WARMONGER);
	}

	// Pick a random greeting from the valid ones (if there are any)
	if(veValidGreetings.size() > 0)
	{
		int iIndex = GC.getGame().getAsyncRandNum(veValidGreetings.size(), "Diplomacy AI: Picking random Greeting for AI to give to human.");

		DiploMessageTypes eGreetingType = veValidGreetings[iIndex];
		const char* strOptionalKey = "";
		bool bFoundPlayer = false;

		// Working against someone picked?
		//if (eGreetingType == DIPLO_MESSAGE_GREETING_WORKING_AGAINST)
		//{
		//	int iThirdPartyLoop = 0;

		//	// Loop until we've picked a random guy
		//	do
		//	{
		//		// Reset to beginning of list
		//		if (iThirdPartyLoop >= MAX_MAJOR_CIVS)
		//			iThirdPartyLoop = 0;

		//		if (IsWorkingAgainstPlayerAccepted(eHuman, (PlayerTypes) iThirdPartyLoop))
		//		{
		//			// Rand roll
		//			if (GC.getGame().getAsyncRandNum(100, "Diplomacy AI: Random against player greeting.") < 33)
		//			{
		//				strOptionalKey = GET_PLAYER((PlayerTypes) iThirdPartyLoop).getNameKey();
		//				bFoundPlayer = true;
		//			}
		//		}

		//		iThirdPartyLoop++;

		//	} while (!bFoundPlayer);
		//}

		// Coop War picked?
		if(eGreetingType == DIPLO_MESSAGE_GREETING_COOP_WAR)
		{
			int iThirdPartyLoop = 0;

			// Loop until we've picked a random guy
			do
			{
				// Reset to beginning of list
				if(iThirdPartyLoop >= MAX_MAJOR_CIVS)
					iThirdPartyLoop = 0;

				if(GetCoopWarAcceptedState(eHuman, (PlayerTypes) iThirdPartyLoop) >= COOP_WAR_STATE_SOON)
				{
					// Rand roll
					if(GC.getGame().getAsyncRandNum(100, "Diplomacy AI: Random coop war greeting.") < 33)
					{
						strOptionalKey = GET_PLAYER((PlayerTypes) iThirdPartyLoop).getNameKey();
						bFoundPlayer = true;
					}
				}

				iThirdPartyLoop++;

			}
			while(!bFoundPlayer);
		}

		return GetDiploStringForMessage(eGreetingType, NO_PLAYER, strOptionalKey);
	}

	// NOTHING else fits so use generic neutral greeting
	return GetDiploStringForMessage(DIPLO_MESSAGE_GREETING_NEUTRAL_HELLO);
}

/// AI is insulting the human
const char* CvDiplomacyAI::GetInsultHumanMessage()
{
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	StrengthTypes eMilitaryStrengthComparedToUs = GetPlayerMilitaryStrengthComparedToUs(ePlayer);

	FStaticVector<DiploMessageTypes, NUM_DIPLO_MESSAGE_TYPES, true, c_eCiv5GameplayDLL, 0> veValidInsults;

	// They're weak militarily
	if(eMilitaryStrengthComparedToUs < STRENGTH_AVERAGE)
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_MILITARY);

	// We have nukes and they don't
	if(kPlayer.getNumNukeUnits() == 0 && m_pPlayer->getNumNukeUnits() > 0)
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_NUKE);

	// They've attacked a lot of minor civs
	else if(GetOtherPlayerNumMinorsAttacked(ePlayer) > 0)
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_BULLY);

	// Their empire is unhappy
	else if(kPlayer.IsEmpireUnhappy())
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_UNHAPPINESS);

	// They have fewer Cities than us
	else if(kPlayer.getNumCities() < m_pPlayer->getNumCities())
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_CITIES);

	// They have a low population
	else if(kPlayer.getTotalPopulation() < m_pPlayer->getTotalPopulation())
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_POPULATION);

	// They have less Culture us
	else if(kPlayer.GetJONSCultureEverGenerated() < m_pPlayer->GetJONSCultureEverGenerated())
		veValidInsults.push_back(DIPLO_MESSAGE_INSULT_CULTURE);

	// Pick a random insult from the valid ones
	if(veValidInsults.size() > 0)
	{
		int iIndex = GC.getGame().getAsyncRandNum(veValidInsults.size(), "Picking random insult for AI to give to human.");

		return GetDiploStringForMessage(veValidInsults[iIndex]);
	}

	// Needed to fall back on generic insult
	return GetDiploStringForMessage(DIPLO_MESSAGE_INSULT_GENERIC);
}

/// The human declares war on the AI, how does he respond?
const char* CvDiplomacyAI::GetAttackedByHumanMessage()
{
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
	StrengthTypes eMilitaryStrengthComparedToUs = GetPlayerMilitaryStrengthComparedToUs(ePlayer);

	// Military Promise Broken
	// This player said he wasn't going to attack us but did anyways
	if(IsPlayerBrokenMilitaryPromise(ePlayer))
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_MILITARY_PROMISE_BROKEN);

	// Sad
	// If we felt the other player was an Ally or Friend then we're just plain sad that they attacked us
	if(eOpinion == MAJOR_CIV_OPINION_ALLY ||
	        eOpinion == MAJOR_CIV_OPINION_FRIEND)
	{
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_SAD);
	}

	// Betrayed	-	TBD
	// This should be related to active deals, e.g. Open Borders, luxuries, etc.
	if(false)
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_BETRAYED);

	// Excited
	// A player with the Conquest Grand Strategy will be excited
	// May also look at Boldness or Offense flavor in here eventually
	{
		if(IsGoingForWorldConquest())
		{
			// They are WEAKER than us
			if(eMilitaryStrengthComparedToUs <= STRENGTH_WEAK)
				return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_STRONG_EXCITED);

			// They are STRONGER than us
			else if(eMilitaryStrengthComparedToUs >= STRENGTH_POWERFUL)
				return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_WEAK_EXCITED);

			// Average strength
			return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_EXCITED);
		}
	}

	// Hostile
	// This is the default response to being attacked
	// We are STRONGER than the person who attacked us
	if(eMilitaryStrengthComparedToUs <= STRENGTH_WEAK)
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_STRONG_HOSTILE);

	// We are WEAKER than the person who attacked us
	else if(eMilitaryStrengthComparedToUs >= STRENGTH_POWERFUL)
		return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_WEAK_HOSTILE);

	// Average strength
	return GetDiploStringForMessage(DIPLO_MESSAGE_ATTACKED_HOSTILE);
}

/// The AI is declaring war on a human, what does he say?
const char* CvDiplomacyAI::GetWarMessage(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "OMG I HAVE NO DATA (DECLARING WAR) - Please send Jon this with your last 5 autosaves and what changelist # you're playing.";

	// Only show the message for the active human
	if(GC.getGame().getActivePlayer() == ePlayer)
	{
		MajorCivApproachTypes eApproachHidingTrueFeelings = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);
		StrengthTypes eMilitaryStrengthComparedToUs = GetPlayerMilitaryStrengthComparedToUs(ePlayer);

		// Betrayal
		// We were Friendly (either real or fake), so we can presume this AI is betraying
		if(eApproachHidingTrueFeelings == MAJOR_CIV_APPROACH_FRIENDLY)
		{
			// Betrayal, and we're weak
			if(eMilitaryStrengthComparedToUs >= STRENGTH_STRONG)
				return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_WEAK_BETRAYAL);

			// Normal betrayal
			else
				return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_BETRAYAL);
		}

		// Land Dispute
		// If Land Dispute is Strong or higher then this is probably a strong contributer to the DoW
		if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
			return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_LAND);

		// Desperate
		// The AI is desperate at this point in the game, and views a DoW as one of its only ways to slow an opponent
		if(eMilitaryStrengthComparedToUs >= STRENGTH_STRONG)
			return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_DESPERATE);

		// World Conquest
		// A player with the Conquest Grand Strategy brags about his goals
		if(IsGoingForWorldConquest())
			return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_WORLD_CONQUEST);

		// Opportunity
		// The AI sees an opportunity because the opponent is weak
		if(eMilitaryStrengthComparedToUs <= STRENGTH_POOR)
			return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_OPPORTUNITY);

		// Generic DoW... probably shouldn't ever really use this
		return GetDiploStringForMessage(DIPLO_MESSAGE_DOW_GENERIC);
	}

	return strText;
}

/// The AI is breaking up with the human
const char* CvDiplomacyAI::GetEndDoFMessage(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "OMG I HAVE NO DATA (DECLARING WAR) - Please send Jon this with your last 5 autosaves and what changelist # you're playing.";

	// Guy is a warmonger
	if(GetWarmongerThreat(ePlayer) >= THREAT_MAJOR)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_WARMONGER");
	// Guy is getting too friendly with our minors
	else if(GetMinorCivDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_MINORS");
	// Guy is setting near us and we don't like it
	else if(GetLandDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_LAND");
	// Guy built wonders we wanted
	else if(GetWonderDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_WONDERS");
	// Guy built wonders we wanted
	else if(GetVictoryDisputeLevel(ePlayer) >= DISPUTE_LEVEL_STRONG)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_VICTORY");
	else
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_WITH_US_DEFAULT");

	return strText;
}

/// The AI is done working against someone
const char* CvDiplomacyAI::GetEndWorkAgainstSomeoneMessage(PlayerTypes ePlayer, const char* strAgainstPlayerKey)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	const char* strText = "OMG I HAVE NO DATA (DECLARING WAR) - Please send Jon this with your last 5 autosaves and what changelist # you're playing.";

	if(ePlayer >=0 && ePlayer < MAX_MAJOR_CIVS)
		strText = GetDiploTextFromTag("RESPONSE_END_WORK_AGAINST_SOMEONE_DEFAULT", strAgainstPlayerKey);

	return strText;
}

/// Wrapper function for getting text from the Diplo Text system
const char* CvDiplomacyAI::GetDiploTextFromTag(const char* strTag, const char* szOptionalKey1, const char* szOptionalKey2)
{
	// This is the leader text tag from the XML
	const char* strLeaderTag = GetPlayer()->getLeaderInfo().GetType();

	// This has to be a member so that our strings don't go out of scope when we leave this function
	m_strDiploText = GC.getGame().GetDiploResponse(strLeaderTag, strTag, szOptionalKey1, szOptionalKey2);

	return m_strDiploText.toUTF8();
}



/////////////////////////////////////////////////////////
// Coop War Request
/////////////////////////////////////////////////////////



/// Do we want to declare war on anyone with ePlayer?
bool CvDiplomacyAI::DoTestCoopWarDesire(PlayerTypes ePlayer, PlayerTypes& eChosenTargetPlayer)
{
	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);

	// If player is planning War, always say no
	if(eApproach == MAJOR_CIV_APPROACH_WAR)
		return false;
	// If player is Hostile, always say no
	else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return false;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);

	// If player is unforgivable, always say no
	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return false;
	// If player is an enemy, always say no
	//else if (eOpinion == MAJOR_CIV_OPINION_ENEMY)
	//	return false;

	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestPlayerScore = 0;

	int iTempScore;

	// Loop through all players to see if we can find a good target
	PlayerTypes eTargetPlayerLoop;
	for(int iTargetPlayerLoop = 0; iTargetPlayerLoop < MAX_MAJOR_CIVS; iTargetPlayerLoop++)
	{
		eTargetPlayerLoop = (PlayerTypes) iTargetPlayerLoop;

		// Player must be valid
		if(!IsPlayerValid(eTargetPlayerLoop))
			continue;

		// Don't test player Target himself
		if(eTargetPlayerLoop == ePlayer)
			continue;

		// Have we already made the agreement?
		if(GetCoopWarAcceptedState(ePlayer, eTargetPlayerLoop) != NO_COOP_WAR_STATE)
			continue;

		// 30 turn buffer if we've been rejected before
		if(GetCoopWarCounter(ePlayer, eTargetPlayerLoop) >= 0 && GetCoopWarCounter(ePlayer, eTargetPlayerLoop) < 30)
			continue;

		iTempScore = GetCoopWarScore(ePlayer, eTargetPlayerLoop, /*bAskedByPlayer*/ false);

		if(iTempScore > iBestPlayerScore)
		{
			iBestPlayerScore = iTempScore;
			eBestPlayer = eTargetPlayerLoop;
		}
	}

	// Found someone?
	if(eBestPlayer != NO_PLAYER)
	{
		eChosenTargetPlayer = eBestPlayer;
		return true;
	}

	return false;
}

/// Willing to agree to go to war with ePlayer against eTargetPlayer?
CoopWarStates CvDiplomacyAI::GetWillingToAgreeToCoopWarState(PlayerTypes ePlayer, PlayerTypes eTargetPlayer)
{
	// Player is willing to agree. War now, or soon?
	if(GetCoopWarScore(ePlayer, eTargetPlayer, /*bAskedByPlayer*/ true) >= 15)
	{
		if(GetPlayerTargetValue(eTargetPlayer) >= TARGET_VALUE_FAVORABLE)
			return COOP_WAR_STATE_ACCEPTED;
		else
			return COOP_WAR_STATE_SOON;
	}

	return COOP_WAR_STATE_REJECTED;
}

/// Does this AI want to ask ePlayer to go to war with eTargetPlayer?
int CvDiplomacyAI::GetCoopWarScore(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, bool bAskedByPlayer)
{
	MajorCivApproachTypes eApproachTowardsPlayer = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);
	MajorCivOpinionTypes eOpinionTowardsPlayer = GetMajorCivOpinion(ePlayer);
	MajorCivApproachTypes eApproachTowardsTarget = GetMajorCivApproach(eTargetPlayer, /*bHideTrueFeelings*/ false);
	MajorCivOpinionTypes eOpinionTowardsTarget = GetMajorCivOpinion(eTargetPlayer);

	// Both players must be able to declare war
	TeamTypes eTargetTeam = GET_PLAYER(eTargetPlayer).getTeam();
	if(!GET_TEAM(GetPlayer()->getTeam()).canDeclareWar(eTargetTeam))
		return 0;
	if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).canDeclareWar(eTargetTeam))
		return 0;

	// If player is inquiring, he has to be planning a war already
	if(!bAskedByPlayer)
	{
		if((eApproachTowardsTarget != MAJOR_CIV_APPROACH_WAR) && (eApproachTowardsTarget != MAJOR_CIV_APPROACH_DECEPTIVE))
			return 0;
	}

	// If player is planning War, always say no
	if(eApproachTowardsPlayer == MAJOR_CIV_APPROACH_WAR)
		return 0;
	// If player is Hostile, always say no
	else if(eApproachTowardsPlayer == MAJOR_CIV_APPROACH_HOSTILE)
		return 0;

	// If player is unforgivable, always say no
	if(eOpinionTowardsPlayer == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return 0;
	// If player is an enemy, always say no
	//else if (eOpinion == MAJOR_CIV_OPINION_ENEMY)
	//	return false;

	// Only players we've met, are alive, etc.
	if(!IsPlayerValid(eTargetPlayer))
		return 0;

	// Don't work Target people we're working WITH!
	if(IsDoFAccepted(eTargetPlayer))
		return 0;

	int iWeight = 0;

	// ePlayer asked us, so if we like him we're more likely to accept
	if(bAskedByPlayer)
	{
		if(eApproachTowardsPlayer == MAJOR_CIV_APPROACH_FRIENDLY || eApproachTowardsPlayer == MAJOR_CIV_APPROACH_DECEPTIVE)
			iWeight += 2;
		else if(eOpinionTowardsPlayer >= MAJOR_CIV_OPINION_FAVORABLE)
			iWeight += 2;
	}

	// Weight for Approach
	if(eApproachTowardsTarget == MAJOR_CIV_APPROACH_WAR)
		iWeight += 5;
	if(eApproachTowardsTarget == MAJOR_CIV_APPROACH_DECEPTIVE)
		iWeight += 2;

	if (GET_PLAYER(ePlayer).GetDiplomacyAI()->IsGoingForWorldConquest())
	{
		iWeight += 3;
	}
	else if (GET_PLAYER(ePlayer).GetDiplomacyAI()->IsGoingForDiploVictory())
	{
		iWeight -= 2;
	}

	// Weight for Opinion
	if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_UNFORGIVABLE)
		iWeight += 12;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_ENEMY)
		iWeight += 8;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_COMPETITOR)
		iWeight += 4;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_FAVORABLE)
		iWeight += -1;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_FRIEND)
		iWeight += -5;
	else if(eOpinionTowardsTarget == MAJOR_CIV_OPINION_ALLY)
		iWeight += -10;

	// Weight for expanding too fast
	if(IsPlayerRecklessExpander(eTargetPlayer))
	{
		iWeight += 3;
	}

	// Weight for warmonger threat
	switch(GetWarmongerThreat(eTargetPlayer))
	{
	case THREAT_MINOR:
		iWeight += 1;
		break;
	case THREAT_MAJOR:
		iWeight += 3;
		break;
	case THREAT_SEVERE:
		iWeight += 4;
		break;
	case THREAT_CRITICAL:
		iWeight += 5;
		break;
	}

	// If we're working with ePlayer then increase weight (if we're already willing to work Target this guy)
	if(iWeight > 0 && IsDoFAccepted(ePlayer))
		iWeight += 4;

	// If we're working with ePlayer then increase weight (if we're already willing to work Target this guy)
	//if (iWeight > 0 && IsWorkingAgainstPlayer(eTargetPlayer))
	//	iWeight += 4;

	// Base Personality value; ranges from 0 to 10 (ish)
	//iWeight += GetWorkTargetWillingness();

	// Weight mod for target value
	switch(GetPlayerTargetValue(eTargetPlayer))
	{
	case TARGET_VALUE_IMPOSSIBLE:
		iWeight *= 75;
		iWeight /= 100;
		break;
	case TARGET_VALUE_BAD:
		iWeight *= 88;
		iWeight /= 100;
		break;
	case TARGET_VALUE_AVERAGE:
		iWeight *= 100;
		iWeight /= 100;
		break;
	case TARGET_VALUE_FAVORABLE:
		iWeight *= 110;
		iWeight /= 100;
		break;
	case TARGET_VALUE_SOFT:
		iWeight *= 120;
		iWeight /= 100;
		break;
	}

	// Rand
	iWeight += GC.getGame().getJonRandNum(5, "Diplomacy AI: Rand for whether AI wants to enter a coop war.");

	// Weight must be high enough for us to return a true desire
	if(iWeight >= 15)
		return iWeight;

	return 0;
}

/// Has ePlayer asked to work Target eTargetPlayer lately?
bool CvDiplomacyAI::IsCoopWarMessageTooSoon(PlayerTypes ePlayer, PlayerTypes eTargetPlayer) const
{
	if(GetCoopWarCounter(ePlayer, eTargetPlayer) >= 0 &&
	        GetCoopWarCounter(ePlayer, eTargetPlayer) < 30)
		return true;

	return false;
}

/// Has ePlayer ever asked about working Target us?
bool CvDiplomacyAI::IsCoopWarEverAsked(PlayerTypes ePlayer, PlayerTypes eTargetPlayer) const
{
	// Counter set to -1 at start. Gets reset to -666
	if(GetCoopWarCounter(ePlayer, eTargetPlayer) == -1)
		return false;

	return true;
}

CoopWarStates CvDiplomacyAI::GetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eTargetPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (CoopWarStates) m_ppaacCoopWarAcceptedState[ePlayer][eTargetPlayer];
}

void CvDiplomacyAI::SetCoopWarAcceptedState(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, CoopWarStates eValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(eValue != GetCoopWarAcceptedState(ePlayer, eTargetPlayer))
	{
		m_ppaacCoopWarAcceptedState[ePlayer][eTargetPlayer] = eValue;
	}
}

short CvDiplomacyAI::GetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eTargetPlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_ppaaiCoopWarCounter[ePlayer][eTargetPlayer];
}

void CvDiplomacyAI::SetCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eTargetPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaaiCoopWarCounter[ePlayer][eTargetPlayer] = iValue;
}

void CvDiplomacyAI::ChangeCoopWarCounter(PlayerTypes ePlayer, PlayerTypes eTargetPlayer, int iChange)
{
	SetCoopWarCounter(ePlayer, eTargetPlayer, GetCoopWarCounter(ePlayer, eTargetPlayer) + iChange);
}

/// Are we done working with anyone Target someone else?
bool CvDiplomacyAI::DoTestContinueCoopWarsDesire(PlayerTypes ePlayer, PlayerTypes& eTargetPlayer)
{
	// Loop through all players to see if we can find a good target
	PlayerTypes eTargetPlayerLoop;
	for(int iTargetPlayerLoop = 0; iTargetPlayerLoop < MAX_MAJOR_CIVS; iTargetPlayerLoop++)
	{
		eTargetPlayerLoop = (PlayerTypes) iTargetPlayerLoop;

		if(GetCoopWarAcceptedState(ePlayer, eTargetPlayerLoop) == COOP_WAR_STATE_ACCEPTED)
		{
			if(!IsContinueCoopWar(ePlayer, eTargetPlayerLoop))
			{
				eTargetPlayer = eTargetPlayerLoop;
				return false;
			}
		}
	}

	return true;
}

/// Do we want to continue working Target ePlayer?
bool CvDiplomacyAI::IsContinueCoopWar(PlayerTypes ePlayer, PlayerTypes eTargetPlayer)
{
	CvAssertMsg(GetCoopWarAcceptedState(ePlayer, eTargetPlayer) == COOP_WAR_STATE_ACCEPTED, "Diplomacy AI: Testing whether we should continue working Target a player, but we aren't right now. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	MajorCivApproachTypes eApproachTowardsPlayer = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);
	MajorCivOpinionTypes eOpinionTowardsPlayer = GetMajorCivOpinion(ePlayer);
	MajorCivApproachTypes eApproachTowardsTarget = GetMajorCivApproach(eTargetPlayer, /*bHideTrueFeelings*/ false);
	MajorCivOpinionTypes eOpinionTowardsTarget = GetMajorCivOpinion(eTargetPlayer);

	// Changed our approach towards ePlayer
	if(eApproachTowardsPlayer == MAJOR_CIV_APPROACH_HOSTILE ||
	        GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true) == MAJOR_CIV_APPROACH_HOSTILE)
		return false;

	// If we're enemies or unforgivable then we're done working with this guy
	if(eOpinionTowardsPlayer <= MAJOR_CIV_OPINION_ENEMY)
		return false;

	// If we've decided we like the guy now, we're done
	if(eApproachTowardsTarget == MAJOR_CIV_APPROACH_FRIENDLY)
		return false;

	if(eOpinionTowardsTarget >= MAJOR_CIV_OPINION_FRIEND)
		return false;

	// If we've worked together for at least 20 turns, and are competitors, there's a chance we break things off
	//if (GetCoopWarCounter(ePlayer) > 20)
	//{
	//	if (eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
	//	{
	//		int iChance = 10 - GetWorkTargetWillingness();
	//		int iRand = GC.getGame().getJonRandNum(100, "Diplomacy AI: Rand for whether AI wants to work Target player");

	//		if (iRand < iChance)
	//			return false;
	//	}
	//}

	return true;
}

/// Check everyone we know to see if we're planning a coop war against them
CoopWarStates CvDiplomacyAI::GetGlobalCoopWarAcceptedState(PlayerTypes ePlayer)
{
	CoopWarStates eBestState = NO_COOP_WAR_STATE;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(GetCoopWarAcceptedState(eLoopPlayer, ePlayer) > eBestState)
				eBestState = GetCoopWarAcceptedState(eLoopPlayer, ePlayer);
		}
	}

	return eBestState;
}

/// What is the SHORTEST amount of time on any coop counter?
int CvDiplomacyAI::GetGlobalCoopWarCounter(PlayerTypes ePlayer)
{
	int iBestCount = MAX_TURNS_SAFE_ESTIMATE;

	int iTempCount;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			iTempCount = GetCoopWarCounter(eLoopPlayer, ePlayer);

			// No valid count against this guy
			if(iTempCount < 0)
				continue;

			if(iTempCount < iBestCount)
				iBestCount = iTempCount;
		}
	}

	return iBestCount;
}

/// Are we locked into a wawr with ePlayer?
bool CvDiplomacyAI::IsLockedIntoCoopWar(PlayerTypes ePlayer)
{
	CoopWarStates eCoopWarState = GetGlobalCoopWarAcceptedState(ePlayer);

	if(eCoopWarState == COOP_WAR_STATE_ACCEPTED || eCoopWarState == COOP_WAR_STATE_SOON)
	{
		if(GetGlobalCoopWarCounter(ePlayer) <= /*20*/ GC.getCOOP_WAR_LOCKED_TURNS())
			return true;
	}

	return false;
}



/////////////////////////////////////////////////////////
// Human Demand
/////////////////////////////////////////////////////////



void CvDiplomacyAI::DoDemandMade(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Reset counter
	SetDemandCounter(ePlayer, 0);

	// See how long it'll be before we might agree to another demand

	int iNumTurns = /*20*/ GC.getDEMAND_TURN_LIMIT_MIN();
	int iRand = GC.getGame().getJonRandNum(/*10*/ GC.getDEMAND_TURN_LIMIT_RAND(), "Diplomacy AI: Number of turns before demand can be accepted.");
	iNumTurns += iRand;

	m_paiDemandTooSoonNumTurns[ePlayer] = iNumTurns;
}

bool CvDiplomacyAI::IsDemandTooSoon(PlayerTypes ePlayer) const
{
	int iDemandTooSoonNumTurns = GetDemandTooSoonNumTurns(ePlayer);

	// Haven't gotten a demand before
	if(iDemandTooSoonNumTurns == -1)
		return false;

	int iCounter = GetDemandCounter(ePlayer);

	if(iCounter > -1 && iCounter < iDemandTooSoonNumTurns)
		return true;

	return false;
}

short CvDiplomacyAI::GetDemandTooSoonNumTurns(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiDemandTooSoonNumTurns[ePlayer];
}

bool CvDiplomacyAI::IsDemandEverMade(PlayerTypes ePlayer) const
{
	if(GetDemandCounter(ePlayer) == -1)
		return false;

	return true;
}

short CvDiplomacyAI::GetDemandCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiDemandCounter[ePlayer];
}

void CvDiplomacyAI::SetDemandCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiDemandCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeDemandCounter(PlayerTypes ePlayer, int iChange)
{
	SetDemandCounter(ePlayer, GetDemandCounter(ePlayer) + iChange);
}



/////////////////////////////////////////////////////////
// Don't Settle Request
/////////////////////////////////////////////////////////



/// Is this AI willing to not settle near ePlayer?
bool CvDiplomacyAI::IsDontSettleAcceptable(PlayerTypes ePlayer) const
{
	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

	// If player is afraid, always say yes
	if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		return true;

	// If player is Hostile, always say no
	else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return false;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);

	// If player is unforgivable, always say no
	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return false;

	// If the player has deleted the EXPANSION Flavor we have to account for that
	int iExpansionFlavor = /*5*/ GC.getDEFAULT_FLAVOR_VALUE();

	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iExpansionFlavor = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
			break;
		}
	}

	// Default threshold
	// EX: 8 - 10 Expansion = -2 * 5 = -10%
	// EX: 8 - 7 Expansion = 1 * 5 = 5%
	// EX: 8 - 5 Expansion = 3 * 5 = 15%
	// EX: 8 - 2 Expansion = 6 * 5 = 30%
	int iThreshold = (/*8*/ GC.getDONT_SETTLE_FLAVOR_BASE() - iExpansionFlavor) * /*5*/ GC.getDONT_SETTLE_FLAVOR_MULTIPLIER();

	bool bFriendly = eApproach == MAJOR_CIV_APPROACH_FRIENDLY;

	// If player is Friendly, add weight
	if(bFriendly)
		iThreshold += /*30*/ GC.getDONT_SETTLE_FRIENDLY();

	int iMilitaryMod = 0;

	// Military Strength compared to us
	switch(GetPlayerMilitaryStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_PATHETIC:
		if(!bFriendly)
			iMilitaryMod += /*-30*/ GC.getDONT_SETTLE_STRENGTH_PATHETIC();
		break;
	case STRENGTH_WEAK:
		if(!bFriendly)
			iMilitaryMod += /*-20*/ GC.getDONT_SETTLE_STRENGTH_WEAK();
		break;
	case STRENGTH_POOR:
		if(!bFriendly)
			iMilitaryMod += /*-10*/ GC.getDONT_SETTLE_STRENGTH_POOR();
		break;
	case STRENGTH_AVERAGE:
		iMilitaryMod += /*0*/ GC.getDONT_SETTLE_STRENGTH_AVERAGE();
		break;
	case STRENGTH_STRONG:
		iMilitaryMod += /*25*/ GC.getDONT_SETTLE_STRENGTH_STRONG();
		break;
	case STRENGTH_POWERFUL:
		iMilitaryMod += /*40*/ GC.getDONT_SETTLE_STRENGTH_POWERFUL();
		break;
	case STRENGTH_IMMENSE:
		iMilitaryMod += /*60*/ GC.getDONT_SETTLE_STRENGTH_IMMENSE();
		break;
	}

	// Military Aggressive Posture - MULTIPLIER
	switch(GetMilitaryAggressivePosture(ePlayer))
	{
	case AGGRESSIVE_POSTURE_NONE:
		iMilitaryMod *= /*100*/ GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_NONE();
		break;
	case AGGRESSIVE_POSTURE_LOW:
		iMilitaryMod *= /*100*/ GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_LOW();
		break;
	case AGGRESSIVE_POSTURE_MEDIUM:
		iMilitaryMod *= /*150*/ GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_MEDIUM();
		break;
	case AGGRESSIVE_POSTURE_HIGH:
		iMilitaryMod *= /*200*/ GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_HIGH();
		break;
	case AGGRESSIVE_POSTURE_INCREDIBLE:
		iMilitaryMod *= /*300*/ GC.getDONT_SETTLE_MOD_MILITARY_POSTURE_INCREDIBLE();
		break;
	}

	// Bring it out of hundreds
	iMilitaryMod /= 100;

	iThreshold += iMilitaryMod;

	int iRand = GC.getGame().getJonRandNum(/*100*/ GC.getDONT_SETTLE_RAND(), "Diplomacy AI: will AI agree to not settle near a player?");

	// Passes the test?
	if(iRand < iThreshold)
		return true;

	return false;
}

/// Has ePlayer asked about us not settling near him lately?
bool CvDiplomacyAI::IsDontSettleMessageTooSoon(PlayerTypes ePlayer) const
{
	if(GetPlayerNoSettleRequestCounter(ePlayer) >= 0)
		return true;

	return false;
}

/// Has ePlayer ever asked about us not settling near him?
bool CvDiplomacyAI::IsPlayerNoSettleRequestEverAsked(PlayerTypes ePlayer) const
{
	// Counter set to -1 at start. Gets reset to -666
	if(GetPlayerNoSettleRequestCounter(ePlayer) == -1)
		return false;

	return true;
}

bool CvDiplomacyAI::IsPlayerNoSettleRequestRejected(PlayerTypes ePlayer) const
{
	// If the counter is -1, it means he hasn't asked
	if(GetPlayerNoSettleRequestCounter(ePlayer) < 0)
		return false;

	// Did the player actually accept?
	if(IsPlayerNoSettleRequestAccepted(ePlayer))
		return false;

	return true;
}

bool CvDiplomacyAI::IsPlayerNoSettleRequestAccepted(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerNoSettleRequestAccepted[ePlayer];
}

void CvDiplomacyAI::SetPlayerNoSettleRequestAccepted(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsPlayerNoSettleRequestAccepted(ePlayer))
	{
		m_pabPlayerNoSettleRequestAccepted[ePlayer] = bValue;

		PlayerTypes eID = GetPlayer()->GetID();

		int iPlotLoop;

		// Add blocker to plots nearby ePlayer
		if(bValue)
		{
			int iLoopX, iLoopY;
			CvPlot* pNearbyPlot;
			int iRange = 6;

			// Loop through all of this player's Cities and create a barrier around them
			CvCity* pLoopCity;
			int iCityLoop;
			for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iCityLoop))
			{
				for(iLoopX = -(iRange); iLoopX <= iRange; iLoopX++)
				{
					for(iLoopY = -(iRange); iLoopY <= iRange; iLoopY++)
					{
						pNearbyPlot = plotXY(pLoopCity->getX(), pLoopCity->getY(), iLoopX, iLoopY);

						if(pNearbyPlot != NULL)
						{
							if(plotDistance(pNearbyPlot->getX(), pNearbyPlot->getY(), pLoopCity->getX(), pLoopCity->getY()) <= iRange)
							{
								pNearbyPlot->SetNoSettling(eID, true);
							}
						}
					}
				}
			}
		}
		// Remove blocker from all plots
		else
		{
			int iNumPlots = GC.getMap().numPlots();
			for(iPlotLoop = 0; iPlotLoop < iNumPlots; iPlotLoop++)
			{
				GC.getMap().plotByIndexUnchecked(iPlotLoop)->SetNoSettling(eID, false);
			}
		}
	}
}

short CvDiplomacyAI::GetPlayerNoSettleRequestCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPlayerNoSettleRequestCounter[ePlayer];
}

void CvDiplomacyAI::SetPlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiPlayerNoSettleRequestCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangePlayerNoSettleRequestCounter(PlayerTypes ePlayer, int iChange)
{
	SetPlayerNoSettleRequestCounter(ePlayer, GetPlayerNoSettleRequestCounter(ePlayer) + iChange);
}

/////////////////////////////////////////////////////////
// Stop Spying Request
/////////////////////////////////////////////////////////

/// Is this AI willing to stop spying on ePlayer?
bool CvDiplomacyAI::IsStopSpyingAcceptable(PlayerTypes ePlayer) const
{
	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);

	// If player is afraid, always say yes
	if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		return true;

	// If player is Hostile, always say no
	else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return false;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);

	// If player is unforgivable, always say no
	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return false;

	return true;
}

/// Has ePlayer asked about us not spying on him lately?
bool CvDiplomacyAI::IsStopSpyingMessageTooSoon(PlayerTypes ePlayer) const
{
	if(GetPlayerStopSpyingRequestCounter(ePlayer) >= 0)
	{
		return true;
	}

	return false;
}

/// Has ePlayer ever asked about us not spying on him?
bool CvDiplomacyAI::IsPlayerStopSpyingRequestEverAsked(PlayerTypes ePlayer) const
{
	// Counter set to -1 at start. Gets reset to -666
	if(GetPlayerStopSpyingRequestCounter(ePlayer) == -1)
	{
		return false;
	}

	return true;
}

bool CvDiplomacyAI::IsPlayerStopSpyingRequestRejected(PlayerTypes ePlayer) const
{
	// If the counter is -1, it means he hasn't asked
	if(GetPlayerStopSpyingRequestCounter(ePlayer) < 0)
		return false;

	// Did the player actually accept?
	if(IsPlayerStopSpyingRequestAccepted(ePlayer))
		return false;

	return true;
}

bool CvDiplomacyAI::IsPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerStopSpyingRequestAccepted[ePlayer];
}

void CvDiplomacyAI::SetPlayerStopSpyingRequestAccepted(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsPlayerStopSpyingRequestAccepted(ePlayer))
	{
		m_pabPlayerStopSpyingRequestAccepted[ePlayer] = bValue;
	}
}

short CvDiplomacyAI::GetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPlayerStopSpyingRequestCounter[ePlayer];
}

void CvDiplomacyAI::SetPlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	m_paiPlayerStopSpyingRequestCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangePlayerStopSpyingRequestCounter(PlayerTypes ePlayer, int iChange)
{
	SetPlayerStopSpyingRequestCounter(ePlayer, GetPlayerStopSpyingRequestCounter(ePlayer) + iChange);
}


///////////////////////////////
// Working With Player
///////////////////////////////



/// Is this AI willing to work with ePlayer?
bool CvDiplomacyAI::IsDoFAcceptable(PlayerTypes ePlayer)
{
	// Can't declare friendship with a civ you're at war with
	if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()) || GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		return false;
	}

	// Haven't known this guy for long enough
	if(IsTooEarlyForDoF(ePlayer))
		return false;

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);

	// If player is planning War, always say no
	if(eApproach == MAJOR_CIV_APPROACH_WAR)
		return false;
	// If player is Hostile, always say no
	else if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return false;
	// If player is afraid, always say yes
	else if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		return true;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);

	// If player is unforgivable, always say no
	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		return false;
	// If player is an enemy, always say no
	else if(eOpinion == MAJOR_CIV_OPINION_ENEMY)
		return false;

	// Has there been a denouncement either direction?
	if(IsDenouncedPlayer(ePlayer))
		return false;
	if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(GetPlayer()->GetID()))
		return false;

	// Are we working AGAINST ePlayer with someone else?
	//if (IsWorkingAgainstPlayer(ePlayer))
	//	return false;

	int iWeight = 0;

	// Base Personality value; ranges from 0 to 10 (ish)
	iWeight += GetDoFWillingness();

	// Weight for Approach
	if(eApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
		iWeight += 3;
	else if(eApproach == MAJOR_CIV_APPROACH_GUARDED)
		iWeight += -1;
	else if(eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
		iWeight += 3;

	// Weight for Opinion
	if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
		iWeight += -3;
	else if(eOpinion == MAJOR_CIV_OPINION_FAVORABLE)
		iWeight += 2;
	else if(eOpinion == MAJOR_CIV_OPINION_FRIEND)
		iWeight += 5;
	else if(eOpinion == MAJOR_CIV_OPINION_ALLY)
		iWeight += 10;

	// Rand
	iWeight += GC.getGame().getJonRandNum(5, "Diplomacy AI: Rand for whether AI wants to work with player");

	if(iWeight >= /*12*/ GC.getDOF_THRESHOLD())
		return true;

	return false;
}

/// AI won't agree to a DoF until they've known a player for at least a few turns
bool CvDiplomacyAI::IsTooEarlyForDoF(PlayerTypes ePlayer)
{
	int iDoFBuffer = /*20*/ GC.getDOF_TURN_BUFFER();

	if(GET_TEAM(GetTeam()).GetTurnsSinceMeetingTeam(GET_PLAYER(ePlayer).getTeam()) < iDoFBuffer)
		return true;

	return false;
}

/// Has ePlayer asked to work with us lately?
bool CvDiplomacyAI::IsDoFMessageTooSoon(PlayerTypes ePlayer) const
{
	if(GetDoFCounter(ePlayer) >= 0 &&
	        GetDoFCounter(ePlayer) < /*20*/ GC.getDOF_TURN_BUFFER())
		return true;

	return false;
}

/// Has ePlayer ever asked about working with us?
bool CvDiplomacyAI::IsDoFEverAsked(PlayerTypes ePlayer) const
{
	// Counter set to -1 at start. Gets reset to -666
	if(GetDoFCounter(ePlayer) == -1)
		return false;

	return true;
}

bool CvDiplomacyAI::IsDoFRejected(PlayerTypes ePlayer) const
{
	// If the counter is -1, it means he hasn't asked
	if(GetDoFCounter(ePlayer) == -1)
		return false;

	// Did the player actually accept?
	if(IsDoFAccepted(ePlayer))
		return false;

	return true;
}

bool CvDiplomacyAI::IsDoFAccepted(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabDoFAccepted[ePlayer];
}

void CvDiplomacyAI::SetDoFAccepted(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsDoFAccepted(ePlayer))
	{
		m_pabDoFAccepted[ePlayer] = bValue;

		m_pPlayer->recomputeGreatPeopleModifiers();

		if(bValue){
			// Someone made a DoF, send out notifications to everyone
			Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_DOF");
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DOF_S");
			for(int iCurPlayer = 0; iCurPlayer < MAX_MAJOR_CIVS; ++iCurPlayer){
				PlayerTypes eCurPlayer = (PlayerTypes) iCurPlayer;
				CvPlayerAI& kCurPlayer = GET_PLAYER(eCurPlayer);
				CvNotifications* pNotifications = GET_PLAYER(eCurPlayer).GetNotifications();
				if(iCurPlayer != ePlayer && iCurPlayer !=GetPlayer()->GetID() && pNotifications){
					const char* strThisPlayerName;
					const char* strOtherPlayerName;

					CvTeam* pCurTeam = &GET_TEAM(kCurPlayer.getTeam());

					// Have we met these guys yet?
					bool bHasMetThisTeam = pCurTeam->isHasMet(GetPlayer()->getTeam());
					if(bHasMetThisTeam)
						strThisPlayerName = GetPlayer()->getCivilizationShortDescriptionKey();
					else
						strThisPlayerName = "TXT_KEY_UNMET_PLAYER";

					bool bHasMetOtherTeam = pCurTeam->isHasMet(GET_PLAYER(ePlayer).getTeam());
					if(bHasMetOtherTeam)
						strOtherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
					else
						strOtherPlayerName = "TXT_KEY_UNMET_PLAYER";

					//Only display notification if we've met one of the players.
					if(bHasMetThisTeam || bHasMetOtherTeam){
						Localization::String tempInfoStr = strText;
						tempInfoStr << strThisPlayerName << strOtherPlayerName;
						Localization::String tempSummaryStr = strSummary;
						tempSummaryStr << strThisPlayerName << strOtherPlayerName;
						pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, tempInfoStr.toUTF8(), tempSummaryStr.toUTF8(), -1, -1, GetPlayer()->GetID(), ePlayer);
					}
				}
			}
		}
	}
}

short CvDiplomacyAI::GetDoFCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiDoFCounter[ePlayer];
}

void CvDiplomacyAI::SetDoFCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiDoFCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeDoFCounter(PlayerTypes ePlayer, int iChange)
{
	SetDoFCounter(ePlayer, GetDoFCounter(ePlayer) + iChange);
}

int CvDiplomacyAI::GetNumDoF()
{
	int iRtnValue = 0;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(IsDoFAccepted(eLoopPlayer))
			{
				iRtnValue++;
			}
		}
	}

	return iRtnValue;
}

/// Are we done with ePlayer, and now want to Denounce him?
bool CvDiplomacyAI::IsDenounceFriendAcceptable(PlayerTypes ePlayer)
{
	CvAssertMsg(IsDoFAccepted(ePlayer), "Diplomacy AI: Testing whether we should Denounce a Friend, but we aren't right now. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Changed our approach towards ePlayer
	MajorCivApproachTypes eTrueApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);
	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true);
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE || eTrueApproach == MAJOR_CIV_APPROACH_HOSTILE)
		return true;

	// If we're enemies or unforgivable then we have no need to work with this guy any more
	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
	if(eOpinion <= MAJOR_CIV_OPINION_ENEMY)
		return true;
	if(eOpinion >= MAJOR_CIV_OPINION_FAVORABLE)
		return false;

	int iChance = 0;

	// If we've worked together for at least 30 turns, and are competitors, there's a chance we break things off (based on Loyalty)
	if(GetDoFCounter(ePlayer) > 30)
	{
		if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
		{
			// 0% chance if value is 9
			// 4% chance if value is 2

			iChance = 10 - GetLoyalty();
			iChance /= 2;
		}
	}

	// Going for conquest, and they're weak and close
	if(IsGoingForWorldConquest())
	{
		if(GetPlayerMilitaryStrengthComparedToUs(ePlayer) <= STRENGTH_POOR)
		{
			if(GetPlayer()->GetProximityToPlayer(ePlayer) >= PLAYER_PROXIMITY_CLOSE)
				iChance += 5;
		}
	}

	int iRand = GC.getGame().getJonRandNum(100, "Diplomacy AI: Rand for whether AI wants to Denounce a Friend");
	if(iRand < iChance)
		return true;

	return false;
}

/// Does ePlayer have a DoF with anyone we have a DoF with?
bool CvDiplomacyAI::IsPlayerDoFwithAnyFriend(PlayerTypes ePlayer) const
{
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsDoFAccepted(eLoopPlayer) && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer))
			return true;
	}

	return false;
}

/// Does ePlayer have a DoF with anyone we have denounced?
bool CvDiplomacyAI::IsPlayerDoFwithAnyEnemy(PlayerTypes ePlayer) const
{
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsDenouncedPlayer(eLoopPlayer) && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDoFAccepted(eLoopPlayer))
			return true;
	}

	return false;
}



///////////////////////////////
// Denounce Player
///////////////////////////////

void CvDiplomacyAI::DoDenouncePlayer(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	PlayerTypes eMyPlayer = GetPlayer()->GetID();
	TeamTypes eMyTeam = GetPlayer()->getTeam();
	TeamTypes eTheirTeam = GET_PLAYER(ePlayer).getTeam();

	SetDenouncedPlayer(ePlayer, true);

	SetDenouncedPlayerCounter(ePlayer, 0);

	// close both embassies
	GET_TEAM(eMyTeam).CloseEmbassyAtTeam(eTheirTeam);
	GET_TEAM(eTheirTeam).CloseEmbassyAtTeam(eMyTeam);

	// WAS working with this player
	if(IsDoFAccepted(ePlayer))
	{
		SetDoFAccepted(ePlayer, false);
		SetDoFCounter(ePlayer, -666);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFAccepted(eMyPlayer, false);
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetDoFCounter(eMyPlayer, -666);

		// They now view us as a traitor!
		GET_PLAYER(ePlayer).GetDiplomacyAI()->SetFriendDenouncedUs(eMyPlayer, true);
	}

	// Going to war, if so don't pretend to be friendly
	if(GetWarFaceWithPlayer(ePlayer) == WAR_FACE_FRIENDLY)
	{
		SetWarFaceWithPlayer(ePlayer, WAR_FACE_NEUTRAL);
	}

	// Third party arrangements
	PlayerTypes eThirdParty;
	for(int iThirdPartyLoop = 0; iThirdPartyLoop < MAX_MAJOR_CIVS; iThirdPartyLoop++)
	{
		eThirdParty = (PlayerTypes) iThirdPartyLoop;

		// WAS in or planning a coop war with the guy we're now at war with
		if(GetCoopWarAcceptedState(ePlayer, eThirdParty) >= COOP_WAR_STATE_SOON)
		{
			SetCoopWarAcceptedState(ePlayer, eThirdParty, NO_COOP_WAR_STATE);
			SetCoopWarCounter(ePlayer, eThirdParty, -666);
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetCoopWarAcceptedState(eMyPlayer, eThirdParty, NO_COOP_WAR_STATE);
			GET_PLAYER(ePlayer).GetDiplomacyAI()->SetCoopWarCounter(eMyPlayer, eThirdParty, -666);
		}
	}

	Localization::String someoneDenounceInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_DENOUNCE");
	Localization::String someoneDenounceSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DENOUNCE_S");
	Localization::String youDenounceInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_DENOUNCE");
	Localization::String youDenounceSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_YOU_DENOUNCE_S");
	Localization::String denounceYouInfo = Localization::Lookup("TXT_KEY_NOTIFICATION_DENOUNCE_YOU");
	Localization::String denounceYouSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_DENOUNCED_YOU_S");
	for(int iCurPlayer = 0; iCurPlayer < MAX_MAJOR_CIVS; ++iCurPlayer){
		PlayerTypes eCurPlayer = (PlayerTypes) iCurPlayer;
		CvPlayerAI& kCurPlayer = GET_PLAYER(eCurPlayer);
		CvNotifications* pNotifications = GET_PLAYER(eCurPlayer).GetNotifications();
		if(pNotifications)
		{
			const char* strThisPlayerName;
			const char* strOtherPlayerName;

			CvTeam* pNotifyTeam = &GET_TEAM(kCurPlayer.getTeam());

			bool bFromMe = GetPlayer()->getTeam() == kCurPlayer.getTeam();
			bool bAtMe = GET_PLAYER(ePlayer).getTeam() == kCurPlayer.getTeam();

			// Have we met these guys yet?
			if(!bFromMe)
			{
				if(pNotifyTeam->isHasMet(GetPlayer()->getTeam()))
					strThisPlayerName = GetPlayer()->getCivilizationShortDescriptionKey();
				else
					strThisPlayerName = "TXT_KEY_UNMET_PLAYER";
			}

			if(!bAtMe)
			{
				if(pNotifyTeam->isHasMet(GET_PLAYER(ePlayer).getTeam()))
					strOtherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
				else
					strOtherPlayerName = "TXT_KEY_UNMET_PLAYER";
			}

			Localization::String strText;
			Localization::String strSummary;

			if(bFromMe)
			{
				strText = youDenounceInfo;
				strText << strOtherPlayerName;
				strSummary = youDenounceSummary;
				strSummary << strOtherPlayerName;
			}
			else if(bAtMe)
			{
				strText = denounceYouInfo;
				strText << strThisPlayerName;
				strSummary = denounceYouSummary;
				strSummary << strThisPlayerName;
			}
			else
			{
				bool bHasMetThisTeam = pNotifyTeam->isHasMet(GetPlayer()->getTeam());
				bool bHasMetOtherTeam = pNotifyTeam->isHasMet(GET_PLAYER(ePlayer).getTeam());

				//Only display notification if we've met either team.
				if(bHasMetThisTeam || bHasMetOtherTeam)
				{
					strText = someoneDenounceInfo;
					strText << strThisPlayerName << strOtherPlayerName;
					strSummary = someoneDenounceSummary;
					strSummary << strThisPlayerName << strOtherPlayerName;
				}
				else
				{
					continue;
				}
			}

			pNotifications->Add(NOTIFICATION_DIPLOMACY_DECLARATION, strText.toUTF8(), strSummary.toUTF8(), -1, -1, GetPlayer()->GetID(), ePlayer);
		}
	}
}

// Does this player feel it's time to denounce ePlayer?
bool CvDiplomacyAI::IsDenounceAcceptable(PlayerTypes ePlayer, bool bBias)
{
	// Can't denounce with a civ you're at war with
	if(GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam()) || GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
	{
		return false;
	}

	// If we've already denounced, it's no good
	if(IsDenouncedPlayer(ePlayer))
		return false;

	// If we're friends, return false - this is handled in IsDenounceFriendAcceptable
	if(IsDoFAccepted(ePlayer))
		return false;

	int iWeight = GetDenounceWeight(ePlayer, bBias);
	if(iWeight >= 18)
		return true;

	return false;
}

int CvDiplomacyAI::GetDenounceWeight(PlayerTypes ePlayer, bool bBias)
{
	int iWeight = 0;

	// Base Personality value; ranges from 0 to 10 (ish)
	iWeight += GetDenounceWillingness();

	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);

	// Hostile: Bonus
	if(eApproach == MAJOR_CIV_APPROACH_HOSTILE)
		iWeight += 6;
	// Afraid: Penalty
	else if(eApproach == MAJOR_CIV_APPROACH_AFRAID)
		iWeight += -10;

	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);

	// Unforgivable: Big Bonus
	if(eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
		iWeight += 10;
	// Enemy: Bonus
	else if(eOpinion == MAJOR_CIV_OPINION_ENEMY)
		iWeight += 5;
	// Competitor: Small Bonus
	else if(eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
		iWeight += 2;
	// Good Relations: Penalty
	else if(eOpinion == MAJOR_CIV_OPINION_FAVORABLE)
		iWeight += -10;
	else if(eOpinion == MAJOR_CIV_OPINION_FRIEND)
		iWeight += -25;
	else if(eOpinion == MAJOR_CIV_OPINION_ALLY)
		iWeight += -50;

	// We are at war
	if(IsAtWar(ePlayer))
		iWeight += 2;

	MajorCivApproachTypes eThirdPartyApproach;

	// Look for other players we like or are strong, and add a bonus if they've denounced this guy, or are at war with him
	PlayerTypes eThirdParty;
	CvDiplomacyAI* pThirdPartyDiplo;
	for(int iThirdParty = 0; iThirdParty < MAX_MAJOR_CIVS; iThirdParty++)
	{
		eThirdParty = (PlayerTypes) iThirdParty;
		pThirdPartyDiplo = GET_PLAYER(eThirdParty).GetDiplomacyAI();

		// War or Denounced ePlayer, so we know eThirdParty doesn't like him
		if(!pThirdPartyDiplo->IsDenouncedPlayer(ePlayer)) // && !pThirdPartyDiplo->IsAtWar(ePlayer))
			continue;

		// We must not be on bad relations with eThirdParty
		if(GetMajorCivOpinion(eThirdParty) <= MAJOR_CIV_OPINION_COMPETITOR)
			continue;

		// If we're hostile or planning war, we don't care about this guy
		eThirdPartyApproach = GetMajorCivApproach(eThirdParty, /*bHideTrueFeelings*/ false);
		if(eThirdPartyApproach == MAJOR_CIV_APPROACH_HOSTILE || eThirdPartyApproach == MAJOR_CIV_APPROACH_WAR)
			continue;

		// We're close to this guy who's at war - want to gain favor
		if(GetPlayer()->GetProximityToPlayer(eThirdParty) == PLAYER_PROXIMITY_NEIGHBORS)
			iWeight += 1;

		// Are they strong?
		if(GetPlayerMilitaryStrengthComparedToUs(eThirdParty) > STRENGTH_AVERAGE)
			iWeight += (GetPlayerMilitaryStrengthComparedToUs(eThirdParty) - STRENGTH_AVERAGE);	// Ex: if they're immense, this will add 3 to the weight

		// Are we friends with them?
		if(IsDoFAccepted(eThirdParty))
			iWeight += 4;
	}

	// Rand: 0-5
	iWeight += GC.getGame().getJonRandNum(5, "Diplomacy AI: Rand for whether AI wants to work with player");

	// Used when friends are asking us to denounce someone
	if(bBias)
		iWeight += 3;

	return iWeight;
}

bool CvDiplomacyAI::IsDenouncedPlayer(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabDenouncedPlayer[ePlayer];
}

void CvDiplomacyAI::SetDenouncedPlayer(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsDenouncedPlayer(ePlayer))
	{
		m_pabDenouncedPlayer[ePlayer] = bValue;
	}
}

/// Denouncing this turn?
bool CvDiplomacyAI::IsDenouncingPlayer(PlayerTypes ePlayer) const
{
	return (IsDenouncedPlayer(ePlayer) && GetDenouncedPlayerCounter(ePlayer) == 1);
}

short CvDiplomacyAI::GetDenouncedPlayerCounter(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiDenouncedPlayerCounter[ePlayer];
}

void CvDiplomacyAI::SetDenouncedPlayerCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiDenouncedPlayerCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangeDenouncedPlayerCounter(PlayerTypes ePlayer, int iChange)
{
	SetDenouncedPlayerCounter(ePlayer, GetDenouncedPlayerCounter(ePlayer) + iChange);
}

// They've denounced someone we have a DoF with
bool CvDiplomacyAI::IsPlayerDenouncedFriend(PlayerTypes ePlayer) const
{
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsDoFAccepted(eLoopPlayer) && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(eLoopPlayer))
			return true;
	}

	return false;
}

// They've denounced someone we've denounced
bool CvDiplomacyAI::IsPlayerDenouncedEnemy(PlayerTypes ePlayer) const
{
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsDenouncedPlayer(eLoopPlayer) && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(eLoopPlayer))
			return true;
	}

	return false;
}



///////////////////////////////
// Requests of Friends
///////////////////////////////



PlayerTypes CvDiplomacyAI::GetRequestFriendToDenounce(PlayerTypes ePlayer, bool& bRandFailed)
{
	bRandFailed = false;

	// Must be friends with ePlayer
	if(!IsDoFAccepted(ePlayer))
		return NO_PLAYER;

	PlayerTypes eTarget = NO_PLAYER;

	int iWeight;

	int iBestWeight = -1;

	CvDiplomacyAI* pTheirAI = GET_PLAYER(ePlayer).GetDiplomacyAI();
	CvTeam* pTheirTeam = &GET_TEAM(GET_PLAYER(ePlayer).getTeam());

	PlayerTypes eLoopPlayer;
	CvPlayer* pLoopPlayer;
	TeamTypes eLoopTeam;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		pLoopPlayer = &GET_PLAYER(eLoopPlayer);
		eLoopTeam = pLoopPlayer->getTeam();

		// Must be alive
		if(!pLoopPlayer->isAlive())
			continue;

		// Can't be either of us
		if(eLoopPlayer == GetPlayer()->GetID() || eLoopPlayer == ePlayer)
			continue;

		// Don't pick someone they haven't met
		if(!pTheirTeam->isHasMet(eLoopTeam))
			continue;

		// Don't pick someone they're at war with
		if(pTheirTeam->isAtWar(eLoopTeam))
			continue;

		// Don't pick someone they've already denounced
		if(pTheirAI->IsDenouncedPlayer(eLoopPlayer))
			continue;

		// Only look at players we've denounced, or at war with
		if(!IsAtWar(eLoopPlayer) && !IsDenouncedPlayer(eLoopPlayer))
			continue;

		// Rand roll
		iWeight = GetDenounceWeight(ePlayer, /*bBias*/ false);

		iWeight += GetNeediness();		// Generally ranges from 3 to 7

		if(iWeight >= 23 && iWeight > iBestWeight)
		{
			iBestWeight = iWeight;

			eTarget = eLoopPlayer;
			bRandFailed = false;		// Set this here as well, because we could have failed a roll on another player, and then succeeded on a later one
		}
		else
			bRandFailed = true;
	}

	return eTarget;
}

bool CvDiplomacyAI::IsFriendDenounceRefusalUnacceptable(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer)
{
	ePlayer;
	eAgainstPlayer;

	// Rand roll
	int iDesire = GetDenounceWeight(eAgainstPlayer, /*bBias*/ false);
	iDesire += GetNeediness();		// Generally ranges from 3 to 7
	iDesire -= GetForgiveness();	// Generally ranges from 3 to 7

	if(iDesire >= 23)	//  Note: 23 is required to even ask (see function above this one)
		return true;

	return false;
}



///////////////////////////////
// Problems between Friends
///////////////////////////////



/// Has this guy had problems with too many of his friends? If so, then his word isn't worth much
bool CvDiplomacyAI::IsUntrustworthyFriend()
{
	// If you've DoWed any friends, you're a bad person
	if(GetWeDeclaredWarOnFriendCount() > 0)
		return true;

	// If you've denounced at least 2 of your friends, you're the problem
	if(GetWeDenouncedFriendCount() >= 2)
		return true;

	return false;
}

/// How many former friends have denounced US???
int CvDiplomacyAI::GetNumFriendsDenouncedBy()
{
	int iNum = 0;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsFriendDenouncedUs(eLoopPlayer))
		{
			// If this guy is untrustworthy, he doesn't count
			if(!GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsUntrustworthyFriend())
				iNum++;
		}
	}

	return iNum;
}

bool CvDiplomacyAI::IsFriendDenouncedUs(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabFriendDenouncedUs[ePlayer];
}

void CvDiplomacyAI::SetFriendDenouncedUs(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsFriendDenouncedUs(ePlayer))
	{
		m_pabFriendDenouncedUs[ePlayer] = bValue;
	}
}

/// How many friends have WE Denounced ?
int CvDiplomacyAI::GetWeDenouncedFriendCount()
{
	int iNum = 0;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsFriendDenouncedUs(eMyPlayer))
			iNum++;
	}

	return iNum;
}

bool CvDiplomacyAI::IsFriendDeclaredWarOnUs(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabFriendDeclaredWarOnUs[ePlayer];
}

void CvDiplomacyAI::SetFriendDeclaredWarOnUs(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if(bValue != IsFriendDeclaredWarOnUs(ePlayer))
	{
		m_pabFriendDeclaredWarOnUs[ePlayer] = bValue;
	}
}

/// How many friends have WE Declared War on ?
int CvDiplomacyAI::GetWeDeclaredWarOnFriendCount()
{
	int iNum = 0;

	PlayerTypes eMyPlayer = GetPlayer()->GetID();

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsFriendDeclaredWarOnUs(eMyPlayer))
			iNum++;
	}

	return iNum;
}

///////////////////////////////
// Working Against Player
///////////////////////////////



/// Do we want to work against anyone with ePlayer?
//bool CvDiplomacyAI::DoTestWorkingAgainstPlayersDesire(PlayerTypes ePlayer, PlayerTypes &eChosenAgainstPlayer)
//{
//	MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);
//
//	// If player is planning War, always say no
//	if (eApproach == MAJOR_CIV_APPROACH_WAR)
//		return false;
//	// If player is Hostile, always say no
//	else if (eApproach == MAJOR_CIV_APPROACH_HOSTILE)
//		return false;
//
//	MajorCivOpinionTypes eOpinion = GetMajorCivOpinion(ePlayer);
//
//	// If player is unforgivable, always say no
//	if (eOpinion == MAJOR_CIV_OPINION_UNFORGIVABLE)
//		return false;
//	// If player is an enemy, always say no
//	//else if (eOpinion == MAJOR_CIV_OPINION_ENEMY)
//	//	return false;
//
//	PlayerTypes eBestPlayer = NO_PLAYER;
//	int iBestPlayerScore = 0;
//
//	CvTeam* pOtherTeam = &GET_TEAM(GET_PLAYER(ePlayer).getTeam());
//
//	int iTempScore;
//
//	// Loop through all players to see if we can find a good target
//	PlayerTypes eAgainstPlayerLoop;
//	for (int iAgainstPlayerLoop = 0; iAgainstPlayerLoop < MAX_MAJOR_CIVS; iAgainstPlayerLoop++)
//	{
//		eAgainstPlayerLoop = (PlayerTypes) iAgainstPlayerLoop;
//
//		// Player must be valid
//		if (!IsPlayerValid(eAgainstPlayerLoop))
//			continue;
//
//		// Don't test player against himself
//		if (eAgainstPlayerLoop == ePlayer)
//			continue;
//
//		// Players must have met one another
//		if (!pOtherTeam->isHasMet(GET_PLAYER(eAgainstPlayerLoop).getTeam()))
//			continue;
//
//		// Have we already made the agreement?
//		if (IsWorkingAgainstPlayerAccepted(ePlayer, eAgainstPlayerLoop))
//			continue;
//
//		// 15 turn buffer if we've been rejected before
//		if (GetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayerLoop) >= 0 && GetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayerLoop) < 15)
//			continue;
//
//		iTempScore = GetWorkingAgainstPlayerAcceptableScore(ePlayer, eAgainstPlayerLoop, /*bAskedByPlayer*/ false);
//
//		if (iTempScore > iBestPlayerScore)
//		{
//			iBestPlayerScore = iTempScore;
//			eBestPlayer = eAgainstPlayerLoop;
//		}
//	}
//
//	// Found someone?
//	if (eBestPlayer != NO_PLAYER)
//	{
//		eChosenAgainstPlayer = eBestPlayer;
//		return true;
//	}
//
//	return false;
//}
//
///// Is this AI willing to work Against ePlayer?
//int CvDiplomacyAI::GetWorkingAgainstPlayerAcceptableScore(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bAskedByPlayer)
//{
//	MajorCivApproachTypes eApproachTowardsPlayer = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);
//	MajorCivOpinionTypes eOpinionTowardsPlayer = GetMajorCivOpinion(ePlayer);
//	MajorCivOpinionTypes eOpinionTowardsAgainst = GetMajorCivOpinion(eAgainstPlayer);
//
//	// If player is planning War, always say no
//	if (eApproachTowardsPlayer == MAJOR_CIV_APPROACH_WAR)
//		return 0;
//	// If player is Hostile, always say no
//	else if (eApproachTowardsPlayer == MAJOR_CIV_APPROACH_HOSTILE)
//		return 0;
//
//	// If player is unforgivable, always say no
//	if (eOpinionTowardsPlayer == MAJOR_CIV_OPINION_UNFORGIVABLE)
//		return 0;
//	// If player is an enemy, always say no
//	else if (eOpinionTowardsPlayer == MAJOR_CIV_OPINION_ENEMY)
//		return 0;
//
//	// Are we working AGAINST ePlayer with someone else?
//	if (IsWorkingAgainstPlayer(ePlayer))
//		return 0;
//
//	// Only players we've met, are alive, etc.
//	if (!IsPlayerValid(eAgainstPlayer))
//		return 0;
//
//	// Don't work against people we're working WITH!
//	if (IsDoFAccepted(eAgainstPlayer))
//		return 0;
//
//	int iWeight = 0;
//
//	// ePlayer asked us, so if we like him we're more likely to accept
//	if (bAskedByPlayer)
//	{
//		if (eApproachTowardsPlayer == MAJOR_CIV_APPROACH_FRIENDLY)
//			iWeight += 2;
//		else if (eOpinionTowardsPlayer <= MAJOR_CIV_OPINION_FAVORABLE)
//			iWeight += 2;
//	}
//
//	// Weight for Approach
//	//if (eApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
//	//	iWeight += 3;
//	//else if (eApproach == MAJOR_CIV_APPROACH_GUARDED)
//	//	iWeight += -1;
//	//else if (eApproach == MAJOR_CIV_APPROACH_FRIENDLY)
//	//	iWeight += 3;
//
//	// Weight for Opinion
//	if (eOpinionTowardsAgainst == MAJOR_CIV_OPINION_UNFORGIVABLE)
//		iWeight += 10;
//	else if (eOpinionTowardsAgainst == MAJOR_CIV_OPINION_ENEMY)
//		iWeight += 7;
//	else if (eOpinionTowardsAgainst == MAJOR_CIV_OPINION_COMPETITOR)
//		iWeight += 4;
//	else if (eOpinionTowardsAgainst == MAJOR_CIV_OPINION_FAVORABLE)
//		iWeight += -1;
//	else if (eOpinionTowardsAgainst == MAJOR_CIV_OPINION_FRIEND)
//		iWeight += -5;
//	else if (eOpinionTowardsAgainst == MAJOR_CIV_OPINION_ALLY)
//		iWeight += -10;
//
//	// If we're working with ePlayer then increase weight (if we're already willing to work against this guy)
//	if (iWeight > 0 && IsDoFAccepted(ePlayer))
//		iWeight += 2;
//
//	// Base Personality value; ranges from 0 to 10 (ish)
//	iWeight += GetDenounceWillingness();
//
//	// Rand
//	iWeight += GC.getGame().getJonRandNum(5, "Diplomacy AI: Rand for whether AI wants to work Against player");
//
//	// Weight must be high enough for us to return a true desire
//	if (iWeight >= 12)
//		return iWeight;
//
//	return 0;
//}
//
///// Has ePlayer asked to work Against eAgainstPlayer lately?
//bool CvDiplomacyAI::IsWorkingAgainstPlayerMessageTooSoon(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const
//{
//	if (GetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayer) >= 0 &&
//		GetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayer) < 30)
//		return true;
//
//	return false;
//}
//
///// Has ePlayer ever asked about working Against us?
//bool CvDiplomacyAI::IsWorkingAgainstPlayerEverAsked(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const
//{
//	// Counter set to -1 at start. Gets reset to -666
//	if (GetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayer) == -1)
//		return false;
//
//	return true;
//}
//
//bool CvDiplomacyAI::IsWorkingAgainstPlayerRejected(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const
//{
//	// If the counter is -1, it means he hasn't asked
//	if (GetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayer) == -1)
//		return false;
//
//	// Did the player actually accept?
//	if (IsWorkingAgainstPlayerAccepted(ePlayer, eAgainstPlayer))
//		return false;
//
//	return true;
//}
//
//bool CvDiplomacyAI::IsWorkingAgainstPlayerAccepted(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(eAgainstPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(eAgainstPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	return m_ppaabWorkingAgainstPlayerAccepted[ePlayer][eAgainstPlayer];
//}
//
//void CvDiplomacyAI::SetWorkingAgainstPlayerAccepted(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bValue)
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(eAgainstPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(eAgainstPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//
//	if (bValue != IsWorkingAgainstPlayerAccepted(ePlayer, eAgainstPlayer))
//	{
//		m_ppaabWorkingAgainstPlayerAccepted[ePlayer][eAgainstPlayer] = bValue;
//	}
//}
//
//short CvDiplomacyAI::GetWorkingAgainstPlayerCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer) const
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(eAgainstPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(eAgainstPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	return m_ppaaiWorkingAgainstPlayerCounter[ePlayer][eAgainstPlayer];
//}
//
//void CvDiplomacyAI::SetWorkingAgainstPlayerCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iValue)
//{
//	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(eAgainstPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	CvAssertMsg(eAgainstPlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//	m_ppaaiWorkingAgainstPlayerCounter[ePlayer][eAgainstPlayer] = iValue;
//}
//
//void CvDiplomacyAI::ChangeWorkingAgainstPlayerCounter(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, int iChange)
//{
//	SetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayer, GetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayer) + iChange);
//}
//
///// Are we done working with anyone against someone else?
//bool CvDiplomacyAI::DoTestContinueWorkingAgainstPlayersDesire(PlayerTypes ePlayer, PlayerTypes &eAgainstPlayer)
//{
//	// Loop through all players to see if we can find a good target
//	PlayerTypes eAgainstPlayerLoop;
//	for (int iAgainstPlayerLoop = 0; iAgainstPlayerLoop < MAX_MAJOR_CIVS; iAgainstPlayerLoop++)
//	{
//		eAgainstPlayerLoop = (PlayerTypes) iAgainstPlayerLoop;
//
//		if (IsWorkingAgainstPlayerAccepted(ePlayer, eAgainstPlayerLoop))
//		{
//			if (!IsContinueWorkingAgainstPlayer(ePlayer, eAgainstPlayerLoop))
//			{
//				eAgainstPlayer = eAgainstPlayerLoop;
//				return false;
//			}
//		}
//	}
//
//	return true;
//}
//
///// Do we want to continue working Against ePlayer?
//bool CvDiplomacyAI::IsContinueWorkingAgainstPlayer(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer)
//{
//	CvAssertMsg(IsWorkingAgainstPlayerAccepted(ePlayer, eAgainstPlayer), "Diplomacy AI: Testing whether we should continue working Against a player, but we aren't right now. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
//
//	// If this guy is dead, don't get out of the agreement, as that looks weird
//	if (!GET_PLAYER(eAgainstPlayer).isAlive())
//		return true;
//
//	MajorCivApproachTypes eApproachTowardsPlayer = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);
//	MajorCivOpinionTypes eOpinionTowardsPlayer = GetMajorCivOpinion(ePlayer);
//	MajorCivApproachTypes eApproachTowardsAgainst = GetMajorCivApproach(eAgainstPlayer, /*bHideTrueFeelings*/ false);
//	MajorCivOpinionTypes eOpinionTowardsAgainst = GetMajorCivOpinion(eAgainstPlayer);
//
//	// Locked in for at least 20 turns
//	if (GetWorkingAgainstPlayerCounter(ePlayer, eAgainstPlayer) < 20)
//		return true;
//
//	// If we're enemies or unforgivable then we're done working with this guy
//	if (eOpinionTowardsPlayer <= MAJOR_CIV_OPINION_ENEMY)
//		return false;
//
//	// Changed our approach towards ePlayer
//	if (eApproachTowardsPlayer == MAJOR_CIV_APPROACH_HOSTILE ||
//		GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ true) == MAJOR_CIV_APPROACH_HOSTILE)
//		return false;
//
//	// If we've decided we like the guy now, we're done
//	if (eApproachTowardsAgainst == MAJOR_CIV_APPROACH_FRIENDLY)
//		return false;
//
//	if (eOpinionTowardsAgainst >= MAJOR_CIV_OPINION_FRIEND)
//		return false;
//
//	// If we've worked together for at least 20 turns, and are competitors, there's a chance we break things off
//	//if (GetWorkingAgainstPlayerCounter(ePlayer) > 20)
//	//{
//	//	if (eOpinion == MAJOR_CIV_OPINION_COMPETITOR)
//	//	{
//	//		int iChance = 10 - GetDenounceWillingness();
//	//		int iRand = GC.getGame().getJonRandNum(100, "Diplomacy AI: Rand for whether AI wants to work Against player");
//
//	//		if (iRand < iChance)
//	//			return false;
//	//	}
//	//}
//
//	return true;
//}
//
///// Check everyone we know to see if we're working with them against ePlayer
//bool CvDiplomacyAI::IsWorkingAgainstPlayer(PlayerTypes ePlayer)
//{
//	PlayerTypes eLoopPlayer;
//	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
//	{
//		eLoopPlayer = (PlayerTypes) iPlayerLoop;
//
//		if (IsPlayerValid(eLoopPlayer))
//		{
//			if (IsWorkingAgainstPlayerAccepted(eLoopPlayer, ePlayer))
//				return true;
//		}
//	}
//
//	return false;
//}



/////////////////////////////////////////////////////////
// A Player's adherence to this AI's statements
/////////////////////////////////////////////////////////



void CvDiplomacyAI::DoTestPromises()
{
	AggressivePostureTypes eOldPosture;

	// Loop through all (known) Majors
	PlayerTypes eLoopPlayer;
	TeamTypes eLoopTeam;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

		if(IsPlayerValid(eLoopPlayer))
		{
			// Military Promise
			if(IsPlayerMadeMilitaryPromise(eLoopPlayer))
			{
				ChangePlayerMilitaryPromiseCounter(eLoopPlayer, 1);

				// Expired?
				if(GetPlayerMilitaryPromiseCounter(eLoopPlayer) > 20)
				{
					SetPlayerMadeMilitaryPromise(eLoopPlayer, false);
					SetPlayerMilitaryPromiseCounter(eLoopPlayer, -1);
				}
			}

			// Expansion Promise
			if(IsPlayerMadeExpansionPromise(eLoopPlayer))
			{
				eOldPosture = GetPlayerExpansionPromiseData(eLoopPlayer);

				// Player has been up to no good since we last warned him
				if(GetExpansionAggressivePosture(eLoopPlayer) > eOldPosture)
				{
					if (!IsPlayerBrokenExpansionPromise(eLoopPlayer))
					{
						SetBrokenExpansionPromiseValue(eLoopPlayer, GC.getOPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
						SetPlayerBrokenExpansionPromise(eLoopPlayer, true);
					}
				}
			}
			else if (IsPlayerMadeExpansionPromise(eLoopPlayer, GC.getGame().getGameTurn() - 1)) // if the expansion promise expired last turn, announce it
			{
				// fire off notification that promise has expired
				CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_EXPANSION_PROMISE_EXPIRED", GET_PLAYER(GetPlayer()->GetID()).getCivilizationShortDescriptionKey());
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_EXPANSION_PROMISE_EXPIRED_S");
					pNotifications->Add(NOTIFICATION_EXPANSION_PROMISE_EXPIRED, strBuffer, strSummary, -1, -1, GetPlayer()->GetID(), eLoopPlayer);
				}

			}

			// Border Promise
			if(IsPlayerMadeBorderPromise(eLoopPlayer))
			{
				eOldPosture = GetPlayerBorderPromiseData(eLoopPlayer);

				// Player has been up to no good since we last warned him
				if(GetPlotBuyingAggressivePosture(eLoopPlayer) > eOldPosture)
				{
					if (!IsPlayerBrokenBorderPromise(eLoopPlayer))
					{
						SetBrokenBorderPromiseValue(eLoopPlayer, GC.getOPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
						SetPlayerBrokenBorderPromise(eLoopPlayer, true);
					}
				}
			}
			else if (IsPlayerMadeBorderPromise(eLoopPlayer, GC.getGame().getGameTurn() - 1)) // if the border promise expired last turn, announce it
			{
				// fire off notification that promise has expired
				CvNotifications* pNotifications = GET_PLAYER(eLoopPlayer).GetNotifications();
				if(pNotifications)
				{
					CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_BORDER_PROMISE_EXPIRED", GET_PLAYER(GetPlayer()->GetID()).getCivilizationShortDescriptionKey());
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_BORDER_PROMISE_EXPIRED_S");
					pNotifications->Add(NOTIFICATION_BORDER_PROMISE_EXPIRED, strBuffer, strSummary, -1, -1, GetPlayer()->GetID(), eLoopPlayer);
				}

			}
		}
	}
}

bool CvDiplomacyAI::IsPlayerMadeMilitaryPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerMadeMilitaryPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeMilitaryPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerMadeMilitaryPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerBrokenMilitaryPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenMilitaryPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenMilitaryPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenMilitaryPromise[ePlayer] = bValue;

	if(bValue)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).SetBrokenMilitaryPromise(true);
	}
}

bool CvDiplomacyAI::IsPlayerIgnoredMilitaryPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerIgnoredMilitaryPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredMilitaryPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerIgnoredMilitaryPromise[ePlayer] = bValue;
}

short CvDiplomacyAI::GetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiPlayerMilitaryPromiseCounter[ePlayer];
}

void CvDiplomacyAI::SetPlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paiPlayerMilitaryPromiseCounter[ePlayer] = iValue;
}

void CvDiplomacyAI::ChangePlayerMilitaryPromiseCounter(PlayerTypes ePlayer, int iChange)
{
	SetPlayerMilitaryPromiseCounter(ePlayer, GetPlayerMilitaryPromiseCounter(ePlayer) + iChange);
}

bool CvDiplomacyAI::IsPlayerMadeExpansionPromise(PlayerTypes ePlayer, int iTestGameTurn)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (m_paiPlayerMadeExpansionPromiseTurn[ePlayer] < 0)
	{
		return false;
	}

	int iGameTurn;
	if (iTestGameTurn != -1)
	{
		iGameTurn = iTestGameTurn;
	}
	else
	{
		iGameTurn = GC.getGame().getGameTurn();
	}

	int iTurnDifference = iGameTurn - m_paiPlayerMadeExpansionPromiseTurn[ePlayer];
	int iTimeOutTurns = (GC.getEXPANSION_PROMISE_TURNS_EFFECTIVE() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent()) / 100;
	if (iTurnDifference < iTimeOutTurns)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CvDiplomacyAI::SetPlayerMadeExpansionPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (bValue)
	{
		m_paiPlayerMadeExpansionPromiseTurn[ePlayer] = GC.getGame().getGameTurn();
	}
	else
	{
		m_paiPlayerMadeExpansionPromiseTurn[ePlayer] = -1;
		SetPlayerExpansionPromiseData(ePlayer, NO_AGGRESSIVE_POSTURE_TYPE);
	}
}

bool CvDiplomacyAI::IsPlayerBrokenExpansionPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenExpansionPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenExpansionPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenExpansionPromise[ePlayer] = bValue;

	if(bValue)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).SetBrokenExpansionPromise(true);
	}
}

bool CvDiplomacyAI::IsPlayerIgnoredExpansionPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerIgnoredExpansionPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredExpansionPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerIgnoredExpansionPromise[ePlayer] = bValue;
}

AggressivePostureTypes CvDiplomacyAI::GetPlayerExpansionPromiseData(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paePlayerExpansionPromiseData[ePlayer];
}

void CvDiplomacyAI::SetPlayerExpansionPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerExpansionPromiseData[ePlayer] = eValue;
}

bool CvDiplomacyAI::EverMadeExpansionPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (m_paiPlayerMadeExpansionPromiseTurn[ePlayer] < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool CvDiplomacyAI::IsPlayerMadeBorderPromise(PlayerTypes ePlayer, int iTestGameTurn)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (m_paiPlayerMadeBorderPromiseTurn[ePlayer] < 0)
	{
		return false;
	}

	int iGameTurn;
	if (iTestGameTurn != -1)
	{
		iGameTurn = iTestGameTurn;
	}
	else
	{
		iGameTurn = GC.getGame().getGameTurn();
	}

	int iTurnDifference = iGameTurn - m_paiPlayerMadeBorderPromiseTurn[ePlayer];
	int iTimeOutTurns = (GC.getBORDER_PROMISE_TURNS_EFFECTIVE() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent()) / 100;
	if (iTurnDifference < iTimeOutTurns)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CvDiplomacyAI::SetPlayerMadeBorderPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	if (bValue)
	{
		m_paiPlayerMadeBorderPromiseTurn[ePlayer] = GC.getGame().getGameTurn();
	}
	else
	{
		m_paiPlayerMadeBorderPromiseTurn[ePlayer] = GC.getGame().getGameTurn();
		SetPlayerBorderPromiseData(ePlayer, NO_AGGRESSIVE_POSTURE_TYPE);
	}
}

bool CvDiplomacyAI::IsPlayerBrokenBorderPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenBorderPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenBorderPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenBorderPromise[ePlayer] = bValue;

	if(bValue)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).SetBrokenBorderPromise(true);
	}
}

bool CvDiplomacyAI::IsPlayerIgnoredBorderPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerIgnoredBorderPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredBorderPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerIgnoredBorderPromise[ePlayer] = bValue;
}

AggressivePostureTypes CvDiplomacyAI::GetPlayerBorderPromiseData(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return (AggressivePostureTypes) m_paePlayerBorderPromiseData[ePlayer];
}

void CvDiplomacyAI::SetPlayerBorderPromiseData(PlayerTypes ePlayer, AggressivePostureTypes eValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paePlayerBorderPromiseData[ePlayer] = eValue;
}

bool CvDiplomacyAI::EverMadeBorderPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if (m_paiPlayerMadeBorderPromiseTurn[ePlayer] < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CvDiplomacyAI::IsPlayerMadeAttackCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerMadeAttackCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeAttackCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerMadeAttackCityStatePromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerBrokenAttackCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenAttackCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenAttackCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenAttackCityStatePromise[ePlayer] = bValue;

	if(bValue)
	{
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).SetBrokenCityStatePromise(true);
	}
}

bool CvDiplomacyAI::IsPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerIgnoredAttackCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredAttackCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerIgnoredAttackCityStatePromise[ePlayer] = bValue;
}

// Promise to not bully a City-State
bool CvDiplomacyAI::IsPlayerMadeBullyCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_pabPlayerMadeBullyCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeBullyCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	m_pabPlayerMadeBullyCityStatePromise[ePlayer] = bValue;
}

// Promise to not bully a City-State
bool CvDiplomacyAI::IsPlayerBrokenBullyCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_pabPlayerBrokenBullyCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenBullyCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	m_pabPlayerBrokenBullyCityStatePromise[ePlayer] = bValue;
}

// Promise to not bully a City-State
bool CvDiplomacyAI::IsPlayerIgnoredBullyCityStatePromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_pabPlayerIgnoredBullyCityStatePromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredBullyCityStatePromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	m_pabPlayerIgnoredBullyCityStatePromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerMadeSpyPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerMadeSpyPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeSpyPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerMadeSpyPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerBrokenSpyPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerBrokenSpyPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenSpyPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerBrokenSpyPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerIgnoredSpyPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerIgnoredSpyPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredSpyPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerIgnoredSpyPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerForgaveForSpying(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerForgaveForSpying[ePlayer];
}

void CvDiplomacyAI::SetPlayerForgaveForSpying(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerForgaveForSpying[ePlayer] = bValue;
}


bool CvDiplomacyAI::IsPlayerMadeNoConvertPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerMadeNoConvertPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerMadeNoConvertPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerMadeNoConvertPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerBrokenNoConvertPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerBrokenNoConvertPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenNoConvertPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerBrokenNoConvertPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerIgnoredNoConvertPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerIgnoredNoConvertPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerIgnoredNoConvertPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerIgnoredNoConvertPromise[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerAskedNotToConvert(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerAskedNotToConvert[ePlayer];
}

void CvDiplomacyAI::SetPlayerAskedNotToConvert(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerAskedNotToConvert[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsPlayerAgreeNotToConvert(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	return m_pabPlayerAgreedNotToConvert[ePlayer];
}

void CvDiplomacyAI::SetPlayerAgreeNotToConvert(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis your save file and version.");
	m_pabPlayerAgreedNotToConvert[ePlayer] = bValue;
}

bool CvDiplomacyAI::IsStopSpreadingReligionAcceptable(PlayerTypes ePlayer)
{
	FlavorTypes eFlavor = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION");
	if (eFlavor == NO_FLAVOR)
	{
		return false;
	}
	int iFlavorReligionValue = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);

	if (iFlavorReligionValue <= 5)
	{
		return true;
	}
	else if (iFlavorReligionValue >= 8)
	{
		return false;
	}
	else
	{
		MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, true);
		switch (eApproach)
		{
		case MAJOR_CIV_APPROACH_DECEPTIVE:
		case MAJOR_CIV_APPROACH_AFRAID:
		case MAJOR_CIV_APPROACH_FRIENDLY:
		case MAJOR_CIV_APPROACH_NEUTRAL:
			return true;
			break;
		case MAJOR_CIV_APPROACH_WAR:
		case MAJOR_CIV_APPROACH_HOSTILE:
		case MAJOR_CIV_APPROACH_GUARDED:
			return false;
			break;
		}
	}

	return false;
}


bool CvDiplomacyAI::IsPlayerBrokenCoopWarPromise(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_pabPlayerBrokenCoopWarPromise[ePlayer];
}

void CvDiplomacyAI::SetPlayerBrokenCoopWarPromise(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_pabPlayerBrokenCoopWarPromise[ePlayer] = bValue;
}

/// Returned civilians?
int CvDiplomacyAI::GetNumCiviliansReturnedToMe(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumCiviliansReturnedToMe[ePlayer];
}

/// Returned civilians?
void CvDiplomacyAI::ChangeNumCiviliansReturnedToMe(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumCiviliansReturnedToMe[ePlayer] += iChange;
		CvAssertMsg(m_paiNumCiviliansReturnedToMe[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Civilians returned. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		// Message for human
		if(iChange > 0)
		{
			if(!GC.getGame().isNetworkMultiPlayer())	// KWG: Candidate for !GC.getGame().IsOption(GAMEOPTION_SIMULTANEOUS_TURNS)
			{
				if(GC.getGame().getActivePlayer() == ePlayer)
				{
					GC.GetEngineUserInterface()->SetForceDiscussionModeQuitOnBack(true);		// Set force quit so that when discuss mode pops up the Back button won't go to leader root
					const char* strText = GetDiploStringForMessage(DIPLO_MESSAGE_RETURNED_CIVILIAN);
					gDLL->GameplayDiplomacyAILeaderMessage(GetPlayer()->GetID(), DIPLO_UI_STATE_BLANK_DISCUSSION, strText, LEADERHEAD_ANIM_POSITIVE);
				}
			}
		}
	}
}

/// Brought player back to life?
bool CvDiplomacyAI::WasResurrectedBy(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiResurrectedOnTurn[ePlayer] != -1;
}

bool CvDiplomacyAI::WasResurrectedThisTurnBy(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paiResurrectedOnTurn[ePlayer] == GC.getGame().getGameTurn();
}

void CvDiplomacyAI::SetResurrectedBy(PlayerTypes ePlayer, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	if(bValue)
	{
		m_paiResurrectedOnTurn[ePlayer] = GC.getGame().getGameTurn();
	}
	else
	{
		m_paiResurrectedOnTurn[ePlayer] = -1;
	}
}

/// Culture Bombed?
int CvDiplomacyAI::GetNumTimesCultureBombed(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumTimesCultureBombed[ePlayer];
}

/// Culture Bombed?
void CvDiplomacyAI::ChangeNumTimesCultureBombed(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumTimesCultureBombed[ePlayer] += iChange;
		CvAssertMsg(m_paiNumTimesCultureBombed[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Culture Bombs returned. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	}
}

/// Converted religion of my cities?
int CvDiplomacyAI::GetNegativeReligiousConversionPoints(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNegativeReligiousConversionPoints[ePlayer];
}

/// Converted religion of my cities?
void CvDiplomacyAI::ChangeNegativeReligiousConversionPoints(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNegativeReligiousConversionPoints[ePlayer] += iChange;
		CvAssertMsg(m_paiNegativeReligiousConversionPoints[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Religious Conversions returned. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	}
}

/// Nuked?
int CvDiplomacyAI::GetNumTimesNuked(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumTimesNuked[ePlayer];
}

/// Nuked?
void CvDiplomacyAI::ChangeNumTimesNuked(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumTimesNuked[ePlayer] += iChange;
		CvAssertMsg(m_paiNumTimesNuked[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Culture Bombs returned. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	}
}

/// Robbed by?
int CvDiplomacyAI::GetNumTimesRobbedBy(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumTimesRobbedBy[ePlayer];
}

/// Robbed by?
void CvDiplomacyAI::ChangeNumTimesRobbedBy(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumTimesRobbedBy[ePlayer] += iChange;
		CvAssertMsg(m_paiNumTimesRobbedBy[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Robbed By returned. Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	}
}

/// Intrigue was shared by the player?
int CvDiplomacyAI::GetNumTimesIntrigueSharedBy(PlayerTypes ePlayer) const
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

	return m_paiNumTimesIntrigueSharedBy[ePlayer];
}

/// Intrigue was shared by the player?
void CvDiplomacyAI::ChangeNumTimesIntrigueSharedBy(PlayerTypes ePlayer, int iChange)
{
	if(iChange != 0)
	{
		CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
		CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send slewis this with your last 5 autosaves and what changelist # you're playing.");

		m_paiNumTimesIntrigueSharedBy[ePlayer] += iChange;
		CvAssertMsg(m_paiNumTimesIntrigueSharedBy[ePlayer] >= 0, "DIPLOMACY_AI: Invalid # of Robbed By returned. Please send slewis this with your last 5 autosaves and what changelist # you're playing.");
	}
}

/////////////////////////////////////////////////////////
// Opinion modifiers
/////////////////////////////////////////////////////////
int CvDiplomacyAI::GetLandDisputeLevelScore(PlayerTypes ePlayer)
{
	// Look at Land Dispute
	int iOpinionWeight = 0;
	switch(GetLandDisputeLevel(ePlayer))
	{
	case DISPUTE_LEVEL_FIERCE:
		iOpinionWeight += /*30*/ GC.getOPINION_WEIGHT_LAND_FIERCE();
		break;
	case DISPUTE_LEVEL_STRONG:
		iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_LAND_STRONG();
		break;
	case DISPUTE_LEVEL_WEAK:
		iOpinionWeight += /*10*/ GC.getOPINION_WEIGHT_LAND_WEAK();
		break;
	case DISPUTE_LEVEL_NONE:
		iOpinionWeight += /*-6*/ GC.getOPINION_WEIGHT_LAND_NONE();
		break;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetWonderDisputeLevelScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Look at Wonder Competition Dispute
	switch(GetWonderDisputeLevel(ePlayer))
	{
	case DISPUTE_LEVEL_FIERCE:
		iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_WONDER_FIERCE();
		break;
	case DISPUTE_LEVEL_STRONG:
		iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_WONDER_STRONG();
		break;
	case DISPUTE_LEVEL_WEAK:
		iOpinionWeight += /*10*/ GC.getOPINION_WEIGHT_WONDER_WEAK();
		break;
	case DISPUTE_LEVEL_NONE:
		iOpinionWeight += /*0*/ GC.getOPINION_WEIGHT_WONDER_NONE();
		break;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetMinorCivDisputeLevelScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Look at Minor Civ Friendship Dispute
	switch(GetMinorCivDisputeLevel(ePlayer))
	{
	case DISPUTE_LEVEL_FIERCE:
		iOpinionWeight += /*30*/ GC.getOPINION_WEIGHT_MINOR_CIV_FIERCE();
		break;
	case DISPUTE_LEVEL_STRONG:
		iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_MINOR_CIV_STRONG();
		break;
	case DISPUTE_LEVEL_WEAK:
		iOpinionWeight += /*10*/ GC.getOPINION_WEIGHT_MINOR_CIV_WEAK();
		break;
	case DISPUTE_LEVEL_NONE:
		iOpinionWeight += /*0*/ GC.getOPINION_WEIGHT_MINOR_CIV_NONE();
		break;
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetWarmongerThreatScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = GetOtherPlayerWarmongerScore(ePlayer) / 2;
	if (iOpinionWeight < 5)
	{
		iOpinionWeight = 0;
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetCiviliansReturnedToMeScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNumCiviliansReturnedToMe(ePlayer) > 0)
		iOpinionWeight += (/*-20*/ GC.getOPINION_WEIGHT_RETURNED_CIVILIAN() * GetNumCiviliansReturnedToMe(ePlayer));
	return iOpinionWeight;
}

int CvDiplomacyAI::GetResurrectedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(WasResurrectedBy(ePlayer))
		iOpinionWeight += (/*-200*/ GC.getOPINION_WEIGHT_RESURRECTED());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetLiberatedCitiesScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	int iNumCitiesLiberated = GetNumCitiesLiberated(ePlayer);
	if (iNumCitiesLiberated >= 3)
	{
		iOpinionWeight = /*-60*/ GC.getOPINION_WEIGHT_LIBERATED_THREE_CITIES();
	}
	else if (iNumCitiesLiberated >= 2)
	{
		iOpinionWeight = /*-50*/ GC.getOPINION_WEIGHT_LIBERATED_TWO_CITIES();
	}
	else if (iNumCitiesLiberated >= 1)
	{
		iOpinionWeight = /*-30*/ GC.getOPINION_WEIGHT_LIBERATED_ONE_CITY();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetEmbassyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GET_TEAM(GET_PLAYER(GetPlayer()->GetID()).getTeam()).HasEmbassyAtTeam(GET_PLAYER(ePlayer).getTeam()))
		iOpinionWeight += (/*-1*/ GC.getOPINION_WEIGHT_EMBASSY());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetForgaveForSpyingScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsPlayerForgaveForSpying(ePlayer))
		iOpinionWeight += /*-10*/ GC.getOPINION_WEIGHT_FORGAVE_FOR_SPYING();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetNoSetterRequestScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsPlayerNoSettleRequestEverAsked(ePlayer))
		iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_ASKED_NO_SETTLE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetStopSpyingRequestScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsPlayerStopSpyingRequestEverAsked(ePlayer))
		iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_ASKED_STOP_SPYING();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDemandEverMadeScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsDemandEverMade(ePlayer))
		iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_MADE_DEMAND_OF_US();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetTimesCultureBombedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNumTimesCultureBombed(ePlayer) > 0)
		iOpinionWeight += (GetNumTimesCultureBombed(ePlayer) * /*30*/ GC.getOPINION_WEIGHT_CULTURE_BOMBED());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetReligiousConversionPointsScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNegativeReligiousConversionPoints(ePlayer) > 0)
	{
		iOpinionWeight += (GetNegativeReligiousConversionPoints(ePlayer) * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion() * /*2*/ GC.getOPINION_WEIGHT_PER_NEGATIVE_CONVERSION());
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetHasAdoptedHisReligionScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(m_pPlayer->GetReligions()->HasOthersReligionInMostCities(ePlayer))
	{
		iOpinionWeight += /*-5*/ GC.getOPINION_WEIGHT_ADOPTING_HIS_RELIGION() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetHasAdoptedMyReligionScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(m_pPlayer->GetReligions()->HasOthersReligionInMostCities(ePlayer))
	{
		// this is commented out because the score should be zero if you have more religions in his city
		//iOpinionWeight += /*-5*/ GC.getOPINION_WEIGHT_ADOPTING_HIS_RELIGION() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion();
	}
	else if(GET_PLAYER(ePlayer).GetReligions()->HasOthersReligionInMostCities(m_pPlayer->GetID()))
	{
		iOpinionWeight += /*-3*/ GC.getOPINION_WEIGHT_ADOPTING_MY_RELIGION() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetSameLatePoliciesScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	PolicyBranchTypes eMyBranch = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirBranch = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
	if(eMyBranch != NO_POLICY_BRANCH_TYPE && eTheirBranch != NO_POLICY_BRANCH_TYPE)
	{
		if(eMyBranch == eTheirBranch)
		{
			iOpinionWeight += /*-5*/ GC.getOPINION_WEIGHT_SAME_LATE_POLICIES() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiplpEmphasisLatePolicies();
		}
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDifferentLatePoliciesScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	PolicyBranchTypes eMyBranch = m_pPlayer->GetPlayerPolicies()->GetLateGamePolicyTree();
	PolicyBranchTypes eTheirBranch = GET_PLAYER(ePlayer).GetPlayerPolicies()->GetLateGamePolicyTree();
	if(eMyBranch != NO_POLICY_BRANCH_TYPE && eTheirBranch != NO_POLICY_BRANCH_TYPE)
	{
		if(eMyBranch != eTheirBranch)
		{
			iOpinionWeight += /*5*/ GC.getOPINION_WEIGHT_DIFFERENT_LATE_POLICIES() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiplpEmphasisLatePolicies();
		}
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetTimesRobbedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNumTimesRobbedBy(ePlayer) > 0)
		iOpinionWeight += (GetNumTimesRobbedBy(ePlayer) * /*20*/ GC.getOPINION_WEIGHT_ROBBED_BY());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetTimesIntrigueSharedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetNumTimesIntrigueSharedBy(ePlayer) > 0)
		iOpinionWeight += (GetNumTimesIntrigueSharedBy(ePlayer) * /*-20*/ GC.getOPINION_WEIGHT_INTRIGUE_SHARED_BY());
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenMilitaryPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broken military promise with us?
	if(IsPlayerBrokenMilitaryPromise(ePlayer))
		iOpinionWeight += /*40*/ GC.getOPINION_WEIGHT_BROKEN_MILITARY_PROMISE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenMilitaryPromiseWithAnybodyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broken military promise with us?
	if(IsPlayerBrokenMilitaryPromise(ePlayer))
	{
	}
	else if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsBrokenMilitaryPromise())
	{
		iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_BROKEN_MILITARY_PROMISE_WORLD();
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredMilitaryPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broken military promise with us?
	if(IsPlayerBrokenMilitaryPromise(ePlayer))
	{

	}
	else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsBrokenMilitaryPromise())
	{

	}
	else if(IsPlayerIgnoredMilitaryPromise(ePlayer))
	{
		iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_IGNORED_MILITARY_PROMISE();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenExpansionPromiseScore (PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (GetBrokenExpansionPromiseValue(ePlayer) > 0)
	{
		int iWeightChange = GetBrokenExpansionPromiseValue(ePlayer) / (GC.getBROKEN_EXPANSION_PROMISE_PER_OPINION_WEIGHT() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
		if(iWeightChange > /*20*/ GC.getOPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_EXPANSION_PROMISE_BROKE_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredExpansionPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (GetBrokenExpansionPromiseValue(ePlayer) > 0)
	{
	}
	else if (GetIgnoredExpansionPromiseValue(ePlayer) > 0)
	{
		int iWeightChange = GetIgnoredExpansionPromiseValue(ePlayer) / (GC.getIGNORED_EXPANSION_PROMISE_PER_OPINION_WEIGHT() *  GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
		if(iWeightChange > /*15*/ GC.getOPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_EXPANSION_PROMISE_IGNORED_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenBorderPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broken Border promise with us?
	//if(IsPlayerBrokenBorderPromise(ePlayer))
	//	iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_BROKEN_BORDER_PROMISE();

	if (GetBrokenBorderPromiseValue(ePlayer) > 0)
	{
		int iWeightChange = GetBrokenBorderPromiseValue(ePlayer) / (GC.getBROKEN_BORDER_PROMISE_PER_OPINION_WEIGHT() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
		if (iWeightChange > /*20*/ GC.getOPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX())
		{
			iWeightChange += GC.getOPINION_WEIGHT_BORDER_PROMISE_BROKE_MAX();
		}
		iOpinionWeight += iWeightChange;
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredBorderPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broken Border promise with us?
	//if(IsPlayerBrokenBorderPromise(ePlayer))
	//{
	//}
	//// Ignored our request for them to make a Border promise?
	//else if(IsPlayerIgnoredBorderPromise(ePlayer))
	//{
	//	iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_IGNORED_BORDER_PROMISE();
	//}
	if (GetBrokenBorderPromiseValue(ePlayer) > 0)
	{
	}
	else if (GetIgnoredBorderPromiseValue(ePlayer) > 0)
	{
		int iWeightChange = GetIgnoredBorderPromiseValue(ePlayer) / (GC.getIGNORED_BORDER_PROMISE_PER_OPINION_WEIGHT() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
		if (iWeightChange > /*15*/ GC.getOPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_BORDER_PROMISE_IGNORED_MAX();
		}
		iOpinionWeight += iWeightChange;
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenAttackCityStatePromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broken CityState attack promise with us?
	if(IsPlayerBrokenAttackCityStatePromise(ePlayer))
		iOpinionWeight += /*40*/ GC.getOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE(); //antonjs: todo: rename
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenAttackCityStatePromiseWithAnybodyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broken CityState attack promise with us?
	if(IsPlayerBrokenAttackCityStatePromise(ePlayer))
	{
		// Broken CityState attack promise with someone?
	}
	else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsBrokenCityStatePromise())
	{
		iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_BROKEN_CITY_STATE_PROMISE_WORLD(); //antonjs: todo: rename
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredAttackCityStatePromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broken CityState attack promise with us?
	if(IsPlayerBrokenAttackCityStatePromise(ePlayer))
	{
		// Broken CityState attack promise with someone?
	}
	else if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).IsBrokenCityStatePromise())
	{
		// Ignored our request for them to make a CityState attack promise?
	}
	else if(IsPlayerIgnoredAttackCityStatePromise(ePlayer))
	{
		iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_IGNORED_CITY_STATE_PROMISE(); //antonjs: todo: rename
	}

	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenBullyCityStatePromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broke a promise to not bully a city-state?
	if(IsPlayerBrokenBullyCityStatePromise(ePlayer))
		iOpinionWeight += 20; //antonjs: todo: xml
	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredBullyCityStatePromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broke a promise to not bully a city-state?
	if(IsPlayerBrokenBullyCityStatePromise(ePlayer))
	{
		// Ignored a promise to not bully a city-state?
	}
	else if(IsPlayerIgnoredBullyCityStatePromise(ePlayer))
	{
		iOpinionWeight += 10; //antonjs: todo: xml
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenNoConvertPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broke a promise to not convert cities again?
	if(IsPlayerBrokenNoConvertPromise(ePlayer))
	{
		iOpinionWeight += /*8*/ GC.getOPINION_WEIGHT_BROKEN_NO_CONVERT_PROMISE() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredNoConvertPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Broke a promise to not convert cities again?
	if(IsPlayerBrokenNoConvertPromise(ePlayer))
	{
	}
	else if(IsPlayerIgnoredNoConvertPromise(ePlayer))
	{
		iOpinionWeight += /*4*/ GC.getOPINION_WEIGHT_IGNORED_NO_CONVERT_PROMISE() * GC.getEraInfo(GC.getGame().getCurrentEra())->getDiploEmphasisReligion();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetBrokenSpyPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsPlayerBrokenSpyPromise(ePlayer))
	{
		iOpinionWeight += GC.getOPINION_WEIGHT_BROKEN_SPY_PROMISE();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetIgnoredSpyPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsPlayerBrokenSpyPromise(ePlayer))
	{

	}
	else if (IsPlayerIgnoredSpyPromise(ePlayer))
	{
		iOpinionWeight += GC.getOPINION_WEIGHT_IGNORED_SPY_PROMISE();
	}
	return iOpinionWeight;
}


int CvDiplomacyAI::GetBrokenCoopWarPromiseScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// Didn't go to war with us when he said he would
	if(IsPlayerBrokenCoopWarPromise(ePlayer))
		iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_BROKEN_COOP_WAR_PROMISE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetAngryAboutProtectedMinorKilledScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsAngryAboutProtectedMinorKilled(ePlayer))
	{
		iOpinionWeight += /*40*/ GC.getOPINION_WEIGHT_KILLED_PROTECTED_MINOR();
		if (GetOtherPlayerNumProtectedMinorsKilled(ePlayer) > 1)
			iOpinionWeight += /*10*/ GC.getOPINION_WEIGHT_KILLED_MANY_PROTECTED_MINORS();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetAngryAboutProtectedMinorAttackedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsAngryAboutProtectedMinorAttacked(ePlayer))
	{
		if (GetTurnsSincePlayerAttackedProtectedMinor(ePlayer) < /*50*/ GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY_NUM_TURNS()) //antonjs: consider: scale by game speed
			iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_RECENTLY();
		else
			iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_ATTACKED_PROTECTED_MINOR_WHILE_AGO();

		if (GetOtherPlayerNumProtectedMinorsAttacked(ePlayer) > 1)
			iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_ATTACKED_MANY_PROTECTED_MINORS();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetAngryAboutProtectedMinorBulliedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsAngryAboutProtectedMinorBullied(ePlayer))
	{
		if (GetTurnsSincePlayerBulliedProtectedMinor(ePlayer) < /*50*/ GC.getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY_NUM_TURNS()) //antonjs: consider: scale by game speed
			iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_RECENTLY();
		else
			iOpinionWeight += /*10*/ GC.getOPINION_WEIGHT_BULLIED_PROTECTED_MINOR_WHILE_AGO();

		if (GetOtherPlayerNumProtectedMinorsBullied(ePlayer) > 1)					
			iOpinionWeight += /*10*/ GC.getOPINION_WEIGHT_BULLIED_MANY_PROTECTED_MINORS();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetAngryAboutSidedWithProtectedMinorScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsAngryAboutSidedWithTheirProtectedMinor(ePlayer))
	{
		iOpinionWeight += /*5*/ GC.getOPINION_WEIGHT_SIDED_WITH_THEIR_MINOR();
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDOFAcceptedScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// We are friends
	if(IsDoFAccepted(ePlayer))
		iOpinionWeight += /*-35*/ GC.getOPINION_WEIGHT_DOF();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDOFWithAnyFriendScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// They have a DoF with at least one other player we have a DoF with
	if(IsPlayerDoFwithAnyFriend(ePlayer))
		iOpinionWeight += /*-15*/ GC.getOPINION_WEIGHT_DOF_WITH_FRIEND();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDOFWithAnyEnemyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// They have a DoF with at least one other player we have DENOUNCED
	if(IsPlayerDoFwithAnyEnemy(ePlayer))
		iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_DOF_WITH_ENEMY();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetFriendDenouncementScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;
	// How many of their (trustworthy) friends have denounced them?
	PlayerTypes eLoopPlayer;

	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != GetPlayer()->GetID() && GET_PLAYER(ePlayer).GetDiplomacyAI()->IsFriendDenouncedUs(eLoopPlayer))
		{
			// If this guy is untrustworthy, he doesn't count
			if(!GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsUntrustworthyFriend())
			{
				// Do we like this player who denounced them more than the player we're evaluating?
				if(GetMajorCivOpinion(eLoopPlayer) > GetMajorCivOpinion(ePlayer))
				{
					iTraitorOpinion += /*20*/ GC.getOPINION_WEIGHT_DENOUNCED_BY_FRIEND_EACH();
				}

				// We actually more friendly with the player we're evaluating than denouncer
				else if(GetMajorCivOpinion(eLoopPlayer) < GetMajorCivOpinion(ePlayer))
				{
					iTraitorOpinion += /*-10*/ GC.getOPINION_WEIGHT_DENOUNCED_BY_FRIEND_DONT_LIKE();
				}
			}
		}
	}

	return iTraitorOpinion;
}

int CvDiplomacyAI::GetWeDenouncedFriendScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;
	int iDenouncedFriends = GET_PLAYER(ePlayer).GetDiplomacyAI()->GetWeDenouncedFriendCount();
	if(iDenouncedFriends > 0)
		iTraitorOpinion = iDenouncedFriends* /*15*/ GC.getOPINION_WEIGHT_DENOUNCED_FRIEND_EACH();
	return iTraitorOpinion;
}

int CvDiplomacyAI::GetFriendDenouncedUsScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;
	// We had a DoF, and they later denounced us!
	if(IsFriendDenouncedUs(ePlayer))
		iTraitorOpinion = /*35*/ GC.getOPINION_WEIGHT_DENOUNCED_ME_FRIENDS();
	return iTraitorOpinion;
}

int CvDiplomacyAI::GetWeDeclaredWarOnFriendScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;
	int iDeclaredWarOnFriendSum = 0;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		iDeclaredWarOnFriendSum += GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetDeclaredWarOnFriendValue(ePlayer);
	}

	iTraitorOpinion = iDeclaredWarOnFriendSum / (GC.getDECLARED_WAR_ON_FRIEND_PER_OPINION_WEIGHT() * GC.getGame().getGameSpeedInfo().getOpinionDurationPercent());
	return iTraitorOpinion;
}

int CvDiplomacyAI::GetFriendDeclaredWarOnUsScore(PlayerTypes ePlayer)
{
	int iTraitorOpinion = 0;
	// We had a DoF, and they later declared war on us!
	if(IsFriendDeclaredWarOnUs(ePlayer))
		iTraitorOpinion = /*60*/ GC.getOPINION_WEIGHT_WAR_ME_FRIENDS();
	return iTraitorOpinion;
}

int CvDiplomacyAI::GetDenouncedUsScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// They denounced us!
	if(GET_PLAYER(ePlayer).GetDiplomacyAI()->IsDenouncedPlayer(GetPlayer()->GetID()))
		iOpinionWeight += /*35*/ GC.getOPINION_WEIGHT_DENOUNCED_ME();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDenouncedThemScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// We denounced them
	if(IsDenouncedPlayer(ePlayer))
		iOpinionWeight += /*35*/ GC.getOPINION_WEIGHT_DENOUNCED_THEM();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDenouncedFriendScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// They've denounced someone we have a DoF with
	if(IsPlayerDenouncedFriend(ePlayer))
		iOpinionWeight += /*15*/ GC.getOPINION_WEIGHT_DENOUNCED_FRIEND();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetDenouncedEnemyScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	// They've denounced someone we've denounced
	if(IsPlayerDenouncedEnemy(ePlayer))
		iOpinionWeight += /*-15*/ GC.getOPINION_WEIGHT_DENOUNCED_ENEMY();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetRecklessExpanderScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsPlayerRecklessExpander(ePlayer))
		iOpinionWeight += /*35*/ GC.getOPINION_WEIGHT_RECKLESS_EXPANDER();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetRecentTradeScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetRecentTradeValue(ePlayer) > 0)
	{
		int iWeightChange = -1 * GetRecentTradeValue(ePlayer) / GC.getDEAL_VALUE_PER_OPINION_WEIGHT();
		if(iWeightChange < /*-30*/ GC.getOPINION_WEIGHT_TRADE_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_TRADE_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetCommonFoeScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetCommonFoeValue(ePlayer) > 0)
	{
		int iWeightChange = -1 * GetCommonFoeValue(ePlayer) / GC.getCOMMON_FOE_VALUE_PER_OPINION_WEIGHT();
		if(iWeightChange < /*-50*/ GC.getOPINION_WEIGHT_COMMON_FOE_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_COMMON_FOE_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}	
	return iOpinionWeight;
}

int CvDiplomacyAI::GetRecentAssistScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(GetRecentAssistValue(ePlayer) != 0)
	{
		int iWeightChange = GetRecentAssistValue(ePlayer) / GC.getDEAL_VALUE_PER_OPINION_WEIGHT();
		if (iWeightChange < -GC.getOPINION_WEIGHT_ASSIST_MAX())
		{
			iWeightChange = -GC.getOPINION_WEIGHT_ASSIST_MAX();
		}
		else if (iWeightChange > GC.getOPINION_WEIGHT_ASSIST_MAX())
		{
			iWeightChange = GC.getOPINION_WEIGHT_ASSIST_MAX();
		}
		iOpinionWeight +=  iWeightChange;
	}
	return iOpinionWeight;
}

int CvDiplomacyAI::GetNukedByScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsNukedBy(ePlayer))
		iOpinionWeight += /*100*/ GC.getOPINION_WEIGHT_NUKED_MAX();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetCapitalCapturedByScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if(IsCapitalCapturedBy(ePlayer))
		iOpinionWeight += /*80*/ GC.getOPINION_WEIGHT_CAPTURED_CAPITAL();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetGaveAssistanceToScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsGaveAssistanceTo(ePlayer))
		iOpinionWeight += /*20*/ GC.getOPINION_WEIGHT_GAVE_ASSISTANCE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetPaidTributeToScore(PlayerTypes ePlayer)
{
	int iOpinionWeight = 0;
	if (IsHasPaidTributeTo(ePlayer))
		iOpinionWeight += /*30*/ GC.getOPINION_WEIGHT_PAID_TRIBUTE();
	return iOpinionWeight;
}

int CvDiplomacyAI::GetScenarioModifier1(PlayerTypes ePlayer)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(ePlayer);

		int iValue = 0;
		if (LuaSupport::CallAccumulator(pkScriptSystem, "GetScenarioDiploModifier1", args.get(), iValue))
		{
			return iValue;
		}
	}

	return 0;
}

int CvDiplomacyAI::GetScenarioModifier2(PlayerTypes ePlayer)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(ePlayer);

		int iValue = 0;
		if (LuaSupport::CallAccumulator(pkScriptSystem, "GetScenarioDiploModifier2", args.get(), iValue))
		{
			return iValue;
		}
	}

	return 0;
}

int CvDiplomacyAI::GetScenarioModifier3(PlayerTypes ePlayer)
{
	ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
	if(pkScriptSystem)
	{
		CvLuaArgsHandle args;
		args->Push(m_pPlayer->GetID());
		args->Push(ePlayer);

		int iValue = 0;
		if (LuaSupport::CallAccumulator(pkScriptSystem, "GetScenarioDiploModifier3", args.get(), iValue))
		{
			return iValue;
		}
	}

	return 0;

}

/////////////////////////////////////////////////////////
// Miscellaneous
/////////////////////////////////////////////////////////

/// Will this player liberate a Minor's City that is now owns?
bool CvDiplomacyAI::DoPossibleMinorLiberation(PlayerTypes eMinor, int iCityID)
{
	bool bLiberate = false;

	if(GetMinorCivApproach(eMinor) == MINOR_CIV_APPROACH_FRIENDLY || GetMinorCivApproach(eMinor) == MINOR_CIV_APPROACH_PROTECTIVE || IsGoingForDiploVictory())
	{
		bLiberate = true;
	}

	if(bLiberate)
	{
		GetPlayer()->DoLiberatePlayer(eMinor, iCityID);
	}

	return bLiberate;
}

/// How many players that we're Competitive or more with is ePlayer at war with?
int CvDiplomacyAI::GetNumOurEnemiesPlayerAtWarWith(PlayerTypes ePlayer)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iAtWarCount = 0;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	TeamTypes eLoopTeam;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		if(IsPlayerValid(eLoopPlayer))
		{
			if(IsAtWar(eLoopPlayer))
			{
				eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

				if(GET_TEAM(eTeam).isAtWar(eLoopTeam))
				{
					iAtWarCount++;
				}
			}
		}
	}

	return iAtWarCount;
}

/// Does this player want to conquer the world?
bool CvDiplomacyAI::IsGoingForWorldConquest()
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}

/// Does this player want to win by diplo?
bool CvDiplomacyAI::IsGoingForDiploVictory()
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}

/// Does this player want to win by diplo?
bool CvDiplomacyAI::IsGoingForCultureVictory()
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}

bool CvDiplomacyAI::IsGoingForSpaceshipVictory()
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP");

	if(eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}


/// Helper function: is this a valid player to be looking at?  (e.g. are they alive, do we know them, etc.)
bool CvDiplomacyAI::IsPlayerValid(PlayerTypes eOtherPlayer, bool bMyTeamIsValid)
{
	CvAssertMsg(eOtherPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Alive?
	if(!GET_PLAYER(eOtherPlayer).isAlive())
	{
		return false;
	}

	// REALLY Alive? (For some reason a player can be "alive" but have no Cities, Units, etc... grrrr)
	if(GET_PLAYER(eOtherPlayer).getNumCities() == 0)
	{
		return false;
	}

	// A player we've met?
	if(!GET_TEAM(GetTeam()).isHasMet(GET_PLAYER(eOtherPlayer).getTeam()))
	{
		return false;
	}

	// Teammate?
	if(!bMyTeamIsValid)
	{
		if(GetTeam() == GET_PLAYER(eOtherPlayer).getTeam())
		{
			return false;
		}
	}

	return true;
}

/// Have we approached another civ about attacking their protected minor?
bool CvDiplomacyAI::HasSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	return m_ppaabSentAttackMessageToMinorCivProtector[ePlayer][eMinor];
}

/// Set flag for whether we have sent a message to ePlayer about attacking their protected eMinor
void CvDiplomacyAI::SetSentAttackProtectedMinorTaunt(PlayerTypes ePlayer, PlayerTypes eMinor, bool bValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(ePlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	m_ppaabSentAttackMessageToMinorCivProtector[ePlayer][eMinor] = bValue;
}

/// Reset whether we have approached another civ about attacking a protected minor (ie. once peace is made with the minor)
void CvDiplomacyAI::ResetSentAttackProtectedMinorTaunts(PlayerTypes eMinor)
{
	CvAssertMsg(eMinor >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	CvAssertMsg(eMinor < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Anton your save file and version.");
	for(int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		SetSentAttackProtectedMinorTaunt((PlayerTypes)iI, eMinor, false);
	}
}

/// We talked to someone, so keep a record of it
void CvDiplomacyAI::DoAddNewStatementToDiploLog(PlayerTypes ePlayer, DiploStatementTypes eNewDiploLogStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eNewDiploLogStatement >= 0, "DIPLOMACY_AI: Invalid Diplo Log Statement Type.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eNewDiploLogStatement < NUM_DIPLO_LOG_STATEMENT_TYPES, "DIPLOMACY_AI: Invalid Diplo Log Statement Type.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Bump current entries back so we can put the new one at index 0
	for(int iI = MAX_DIPLO_LOG_STATEMENTS - 1; iI > 0; iI--)
	{
		// Nothing in this entry to move?
		if(m_ppaDiploStatementsLog[ePlayer][iI-1].m_eDiploLogStatement != NO_DIPLO_STATEMENT_TYPE)
		{
			m_ppaDiploStatementsLog[ePlayer][iI].m_eDiploLogStatement = m_ppaDiploStatementsLog[ePlayer][iI-1].m_eDiploLogStatement;
			m_ppaDiploStatementsLog[ePlayer][iI].m_iTurn = m_ppaDiploStatementsLog[ePlayer][iI-1].m_iTurn;
		}
	}

	m_ppaDiploStatementsLog[ePlayer][0].m_eDiploLogStatement = eNewDiploLogStatement;
	m_ppaDiploStatementsLog[ePlayer][0].m_iTurn = 0;
}

/// Returns the DiploLogStatementType associated with the index passed in
DiploStatementTypes CvDiplomacyAI::GetDiploLogStatementTypeForIndex(PlayerTypes ePlayer, int iIndex)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_ppaDiploStatementsLog[ePlayer][iIndex].m_eDiploLogStatement;
}

/// Returns the DiploLogStatement turn number associated with the index passed in
int CvDiplomacyAI::GetDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return m_ppaDiploStatementsLog[ePlayer][iIndex].m_iTurn;
}

/// Sets the DiploLogStatement turn number associated with the index passed in
void CvDiplomacyAI::SetDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex, int iNewValue)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iNewValue >= 0, "DIPLOMACY_AI: Setting DiploLogStatementTurnForIndex to a negative value.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_ppaDiploStatementsLog[ePlayer][iIndex].m_iTurn = iNewValue;
}

/// Changes the DiploLogStatement turn number associated with the index passed in
void CvDiplomacyAI::ChangeDiploLogStatementTurnForIndex(PlayerTypes ePlayer, int iIndex, int iChange)
{
	SetDiploLogStatementTurnForIndex(ePlayer, iIndex, GetDiploLogStatementTurnForIndex(ePlayer, iIndex) + iChange);
}

/// How long has it been since we sent eDiploLogStatement?
int CvDiplomacyAI::GetNumTurnsSinceStatementSent(PlayerTypes ePlayer, DiploStatementTypes eDiploLogStatement)
{
	CvAssertMsg(ePlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(ePlayer < MAX_MAJOR_CIVS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDiploLogStatement >= 0, "DIPLOMACY_AI: Invalid Diplo Log Statement Type.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDiploLogStatement < NUM_DIPLO_LOG_STATEMENT_TYPES, "DIPLOMACY_AI: Invalid Diplo Log Statement Type.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	int iMostRecentTurn = MAX_TURNS_SAFE_ESTIMATE;

	int iLoopTurnNum;
	DiploStatementTypes eLoopStatement;

	for(int iI = 0; iI < MAX_DIPLO_LOG_STATEMENTS; iI++)
	{
		eLoopStatement = GetDiploLogStatementTypeForIndex(ePlayer, iI);

		if(eLoopStatement != NO_DIPLO_STATEMENT_TYPE)
		{
			if(eLoopStatement == eDiploLogStatement)
			{
				iLoopTurnNum = GetDiploLogStatementTurnForIndex(ePlayer, iI);

				if(iMostRecentTurn == MAX_TURNS_SAFE_ESTIMATE || iLoopTurnNum > iMostRecentTurn)
				{
					iMostRecentTurn = iLoopTurnNum;

					// Now break out, otherwise we'll find later entries and think it's been longer since we sent something than it really has been!
					break;
				}
			}
		}
	}

	return iMostRecentTurn;
}

/// AI is doing something with a Minor civ
void CvDiplomacyAI::LogMinorCivGiftGold(PlayerTypes eMinor, int iOldFriendship, int iGold, bool bSaving, bool bWantQuickBoost, PlayerTypes ePlayerTryingToPass)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		// Their Name
		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		// Gift amount
		strTemp.Format("Gold Gift: %d", iGold);
		strOutBuf += ", " + strTemp;

		strTemp.Format("Friendship: %d to %d", iOldFriendship, GET_PLAYER(eMinor).GetMinorCivAI()->GetEffectiveFriendshipWithMajor(GetPlayer()->GetID()));
		strOutBuf += ", " + strTemp;

		LogGrandStrategy(strOutBuf);

		// City State type
		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		// Saving Gold for Gift
		if(bSaving)
			strOutBuf += ", (SAVING) ";

		// Wants a quick boost
		if(bWantQuickBoost)
			strOutBuf += ", Boost! ";

		// Trying to pass up a player!
		if(ePlayerTryingToPass != NO_PLAYER)
		{
			strOutBuf += ", Passing ";
			strOutBuf += GET_PLAYER(ePlayerTryingToPass).getCivilizationShortDescription();
		}

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

/// AI is doing something with a Minor civ
void CvDiplomacyAI::LogMinorCivBullyGold(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, int iGold, bool bSuccess, int iBullyMetricScore)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		// Their Name
		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		// Gold tribute
		strTemp.Format("Gold Bully: %d", iGold);
		strOutBuf += ", " + strTemp;

		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;

		LogGrandStrategy(strOutBuf);

		// City State type
		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		// Did it work?
		if(bSuccess)
		{
			strOutBuf += ", ";
			strOutBuf += "Success!";
		}
		else
		{
			strOutBuf += ", ";
			strOutBuf += "Failure";
		}

		// Append the bully metric score
		strTemp.Format("Bully Score: %d", iBullyMetricScore);
		strOutBuf += ", " + strTemp;

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

void CvDiplomacyAI::LogMinorCivBullyUnit(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, UnitTypes eUnit, bool bSuccess, int iBullyMetricScore)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		// Their Name
		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		// Unit tribute
		strTemp.Format("Unit Bully: %s", eUnit == (UnitTypes) GC.getInfoTypeForString("UNIT_WORKER") ? "Worker" : "Other unit");
		strOutBuf += ", " + strTemp;

		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;

		LogGrandStrategy(strOutBuf);

		// City State type
		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		// Did it work?
		if(bSuccess)
		{
			strOutBuf += ", ";
			strOutBuf += "Success!";
		}
		else
		{
			strOutBuf += ", ";
			strOutBuf += "Failure";
		}

		// Append the bully metric score
		strTemp.Format("Bully Score: %d", iBullyMetricScore);
		strOutBuf += ", " + strTemp;

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

/// A quest was given to us, so log it
void CvDiplomacyAI::LogMinorCivQuestReceived(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		// Their Name
		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		// Quest Info
		strTemp = "RECEIVED QUEST";
		strOutBuf += ", " + strTemp;

		LogMinorCivQuestType(strOutBuf, eType);

		// Friendship Change
		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;

		// City State type
		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

/// We finished a quest! Now log it
void CvDiplomacyAI::LogMinorCivQuestFinished(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		// Their Name
		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		// Quest Info
		strTemp = "FINISHED QUEST";
		strOutBuf += ", " + strTemp;

		LogMinorCivQuestType(strOutBuf, eType);

		// Friendship Change
		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;

		// City State type
		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

/// A quest became obsolete and was cancelled, so log it
void CvDiplomacyAI::LogMinorCivQuestCancelled(PlayerTypes eMinor, int iOldFriendshipTimes100, int iNewFriendshipTimes100, MinorCivQuestTypes eType)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		// Their Name
		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		// Quest Info
		strTemp = "cancelled";
		strOutBuf += ", " + strTemp;

		LogMinorCivQuestType(strOutBuf, eType);

		// Friendship Change
		strTemp.Format("Friendship: %d to %d", iOldFriendshipTimes100 / 100, iNewFriendshipTimes100 / 100);
		strOutBuf += ", " + strTemp;

		// City State type
		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

/// A minor civ was bought, and possession changed
void CvDiplomacyAI::LogMinorCivBuyout(PlayerTypes eMinor, int iGoldPaid, bool bSaving)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
			strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
		else
			strLogName = "DiplomacyAI_MinorCiv_Log.csv";

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		// Their Name
		otherPlayerName = GET_PLAYER(eMinor).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		strTemp.Format("BUYOUT: %d", iGoldPaid);
		strOutBuf += ", " + strTemp;

		// Spacing
		strOutBuf += ", , ";

		// City State type
		strOutBuf += ", ";
		strOutBuf += GC.getMinorCivInfo(GET_PLAYER(eMinor).GetMinorCivAI()->GetMinorCivType())->GetType();

		// Saving Gold for Gift
		if(bSaving)
			strOutBuf += ", (SAVING) ";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

/// We made a public declaration, so keep a record of it
void CvDiplomacyAI::DoAddNewDeclarationToLog(PublicDeclarationTypes eDeclaration, int iData1, int iData2, PlayerTypes eMustHaveMetPlayer, bool bActive)
{
	CvAssertMsg(eDeclaration >= 0, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeclaration < NUM_PUBLIC_DECLARATION_TYPES, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	// Bump current entries back so we can put the new one at index 0
	for(int iI = MAX_DIPLO_LOG_STATEMENTS - 1; iI > 0; iI--)
	{
		// Nothing in this entry to move?
		if(m_paDeclarationsLog[iI-1].m_eDeclaration != NO_PUBLIC_DECLARATION_TYPE)
		{
			m_paDeclarationsLog[iI].m_eDeclaration = m_paDeclarationsLog[iI-1].m_eDeclaration;
			m_paDeclarationsLog[iI].m_iData1 = m_paDeclarationsLog[iI-1].m_iData1;
			m_paDeclarationsLog[iI].m_iData2 = m_paDeclarationsLog[iI-1].m_iData2;
			m_paDeclarationsLog[iI].m_eMustHaveMetPlayer = m_paDeclarationsLog[iI-1].m_eMustHaveMetPlayer;
			m_paDeclarationsLog[iI].m_bActive = m_paDeclarationsLog[iI-1].m_bActive;
			m_paDeclarationsLog[iI].m_iTurn = m_paDeclarationsLog[iI-1].m_iTurn;
		}
	}

	m_paDeclarationsLog[0].m_eDeclaration = eDeclaration;
	m_paDeclarationsLog[0].m_iData1 = iData1;
	m_paDeclarationsLog[0].m_iData2 = iData2;
	m_paDeclarationsLog[0].m_eMustHaveMetPlayer = eMustHaveMetPlayer;
	m_paDeclarationsLog[0].m_bActive = bActive;
	m_paDeclarationsLog[0].m_iTurn = 0;
}

/// Returns the DeclarationLogType associated with the index passed in
PublicDeclarationTypes CvDiplomacyAI::GetDeclarationLogTypeForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_eDeclaration;
}

/// Returns the DeclarationLog iData1 number associated with the index passed in
int CvDiplomacyAI::GetDeclarationLogData1ForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_iData1;
}

/// Returns the DeclarationLog iData2 number associated with the index passed in
int CvDiplomacyAI::GetDeclarationLogData2ForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_iData2;
}

/// Does whoever is listening to our declaration have to have met someone?
PlayerTypes CvDiplomacyAI::GetDeclarationLogMustHaveMetPlayerForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_eMustHaveMetPlayer;
}

/// Returns the DeclarationLog iData2 number associated with the index passed in
bool CvDiplomacyAI::IsDeclarationLogForIndexActive(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_bActive;
}

// Make Declaration invalid (so that it doesn't appear for new players we meet, for example)
void CvDiplomacyAI::DoMakeDeclarationInactive(PublicDeclarationTypes eDeclaration, int iData1, int iData2)
{
	CvAssertMsg(eDeclaration >= 0, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eDeclaration < NUM_DIPLO_LOG_STATEMENT_TYPES, "DIPLOMACY_AI: Invalid PublicDeclarationType.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	for(int iLoop = 0; iLoop < MAX_DIPLO_LOG_STATEMENTS; iLoop++)
	{
		// DeclarationType match?
		if(m_paDeclarationsLog[iLoop].m_eDeclaration == eDeclaration)
		{
			// iData1 match?
			if(m_paDeclarationsLog[iLoop].m_iData1 == iData1)
			{
				// iData2 match?
				if(m_paDeclarationsLog[iLoop].m_iData2 == iData2)
				{
					m_paDeclarationsLog[iLoop].m_bActive = false;
				}
			}
		}
	}
}

/// Returns the DeclarationLog turn number associated with the index passed in
int CvDiplomacyAI::GetDeclarationLogTurnForIndex(int iIndex)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	return m_paDeclarationsLog[iIndex].m_iTurn;
}

/// Sets the DeclarationLog turn number associated with the index passed in
void CvDiplomacyAI::SetDeclarationLogTurnForIndex(int iIndex, int iNewValue)
{
	CvAssertMsg(iIndex >= 0, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(iIndex < MAX_DIPLO_LOG_STATEMENTS, "DIPLOMACY_AI: Invalid DiploLogStatement Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	m_paDeclarationsLog[iIndex].m_iTurn = iNewValue;
}

/// Changes the DeclarationLog turn number associated with the index passed in
void CvDiplomacyAI::ChangeDeclarationLogTurnForIndex(int iIndex, int iChange)
{
	SetDeclarationLogTurnForIndex(iIndex, GetDeclarationLogTurnForIndex(iIndex) + iChange);
}

/// Deal to renew
CvDeal* CvDiplomacyAI::GetDealToRenew(int* piDealType)
{
	CvDeal* pTargetDeal = NULL;
	CvGameDeals* pGameDeals = GC.getGame().GetGameDeals();

	if (piDealType)
	{
		*piDealType = -1;
	}

	for(int iDealTypes = 0; iDealTypes < 2; iDealTypes++)
	{
		int iNumDeals = 0;
		if(iDealTypes == 0)
		{
			iNumDeals = pGameDeals->GetNumHistoricDeals(m_pPlayer->GetID());
		}
		else
		{
			iNumDeals = pGameDeals->GetNumCurrentDeals(m_pPlayer->GetID());
		}

		for(int iDeal = 0; iDeal < iNumDeals; iDeal++)
		{
			CvDeal* pCurrentDeal = NULL;
			if(iDealTypes == 0)
			{
				pCurrentDeal = pGameDeals->GetHistoricDeal(m_pPlayer->GetID(), iDeal);
			}
			else
			{
				pCurrentDeal = pGameDeals->GetCurrentDeal(m_pPlayer->GetID(), iDeal);
			}

			if (pCurrentDeal->m_bConsideringForRenewal)
			{
				CvAssertMsg(pTargetDeal == NULL, "pTargetDeal should be null");
				if (!pTargetDeal)
				{
					pTargetDeal = pCurrentDeal;
					if (piDealType)
					{
						*piDealType = iDealTypes;
					}
				}
			}
		}
	}

	return pTargetDeal;
}

void CvDiplomacyAI::ClearDealToRenew()
{
	CvGameDeals* pGameDeals = GC.getGame().GetGameDeals();
	int iNumMarkedToRenew = 0;

	for(int iDealTypes = 0; iDealTypes < 2; iDealTypes++)
	{
		int iNumDeals = 0;
		if(iDealTypes == 0)
		{
			iNumDeals = pGameDeals->GetNumHistoricDeals(m_pPlayer->GetID());
		}
		else
		{
			iNumDeals = pGameDeals->GetNumCurrentDeals(m_pPlayer->GetID());
		}

		for(int iDeal = 0; iDeal < iNumDeals; iDeal++)
		{
			CvDeal* pCurrentDeal = NULL;
			if(iDealTypes == 0)
			{
				pCurrentDeal = pGameDeals->GetHistoricDeal(m_pPlayer->GetID(), iDeal);
			}
			else
			{
				pCurrentDeal = pGameDeals->GetCurrentDeal(m_pPlayer->GetID(), iDeal);
			}

			if (pCurrentDeal->m_bConsideringForRenewal)
			{
				iNumMarkedToRenew++;
				CvAssertMsg(iNumMarkedToRenew <= 1, "iNumMarkedToRenew is greater than one. Too many deals are being renewed at the same time.");
				pCurrentDeal->m_bConsideringForRenewal = false;
			}
		}
	}
}

// ************************************
// PRIVATE
// ************************************



/// Helper function to determine if we're at war with a player
bool CvDiplomacyAI::IsAtWar(PlayerTypes eOtherPlayer)
{
	CvAssertMsg(eOtherPlayer >= 0, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
	CvAssertMsg(eOtherPlayer < MAX_CIV_PLAYERS, "DIPLOMACY_AI: Invalid Player Index.  Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

	return GET_TEAM(GetTeam()).isAtWar(GET_PLAYER(eOtherPlayer).getTeam());
}



// ************************************
// LOGGING
// ************************************



/// Log public declaration made by this AI
void CvDiplomacyAI::LogPublicDeclaration(PublicDeclarationTypes eDeclaration, int iData1, PlayerTypes eForSpecificPlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		// JON: Disabling some logspam
		if(eForSpecificPlayer != NO_PLAYER)
			return;

		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		strOutBuf = strBaseString;

		// Is this declaration for a specific player (one we met after we made the declaration?)
		if(eForSpecificPlayer != NO_PLAYER)
		{
			otherPlayerName = GET_PLAYER(eForSpecificPlayer).getCivilizationShortDescription();
			strTemp = "To " + otherPlayerName;
			strOutBuf += ", " + strTemp;
		}

		PlayerTypes eMinorCiv;

		bool bMinorMessage = false;

		switch(eDeclaration)
		{
		case PUBLIC_DECLARATION_PROTECT_MINOR:
			eMinorCiv = (PlayerTypes) iData1;
			otherPlayerName = GET_PLAYER(eMinorCiv).getCivilizationShortDescription();
			strTemp = "We're now protecting " + otherPlayerName + "!";
			bMinorMessage = true;
			break;
		case PUBLIC_DECLARATION_ABANDON_MINOR:
			eMinorCiv = (PlayerTypes) iData1;
			otherPlayerName = GET_PLAYER(eMinorCiv).getCivilizationShortDescription();
			strTemp = "We've abandoned " + otherPlayerName + "!";
			bMinorMessage = true;
			break;
		default:
			strTemp.Format("Unknown Declaration!!!");
			break;
		}

		strOutBuf += ", " + strTemp;

		pLog->Msg(strOutBuf);

		// Also send message to Minor Civ log if applicable
		if(bMinorMessage)
		{
			// Open the log file
			if(GC.getPlayerAndCityAILogSplit())
				strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
			else
				strLogName = "DiplomacyAI_MinorCiv_Log.csv";

			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

/// Log war declaration
void CvDiplomacyAI::LogWarDeclaration(PlayerTypes ePlayer, int iTotalWarWeight)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ", ***** WAR DECLARATION! *****, " + otherPlayerName;

		if(iTotalWarWeight >= 0)
		{
			CvString strWarWeight;
			strWarWeight.Format(", %d, ", iTotalWarWeight);
			strOutBuf += strWarWeight;
		}

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");

		// Want this in DiploMessage Log
//		if (!GET_PLAYER(ePlayer).isMinorCiv())
		{
			if(GC.getPlayerAndCityAILogSplit())
			{
				strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
			}
			else
			{
				strLogName = "DiplomacyAI_Messages_Log.csv";
			}
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}

		// Log it to military AI too
		m_pPlayer->GetMilitaryAI()->LogDeclarationOfWar(ePlayer);
	}
}


/// Log war declaration
void CvDiplomacyAI::LogPeaceMade(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ", ***** PEACE MADE! *****, " + otherPlayerName;

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");

		// Want this in DiploMessage Log
//		if (!GET_PLAYER(ePlayer).isMinorCiv())
		{
			if(GC.getPlayerAndCityAILogSplit())
			{
				strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
			}
			else
			{
				strLogName = "DiplomacyAI_Messages_Log.csv";
			}
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}

		// Log it to military AI too
		m_pPlayer->GetMilitaryAI()->LogPeace(ePlayer);
	}
}


/// Log working with a player
void CvDiplomacyAI::LogDoF(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ",***** NOW FRIENDS " + otherPlayerName + "! *****";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


/// Log working with a player
void CvDiplomacyAI::LogDenounce(PlayerTypes ePlayer, bool bBackstab, bool bRefusal)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvString otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		//CvString againstPlayerName = GET_PLAYER(eAgainstPlayer).getCivilizationShortDescription();

		if(bBackstab)
			strOutBuf = strBaseString + ",***** BACKSTABBED " + otherPlayerName + "! *****";
		else if(bRefusal)
			strOutBuf = strBaseString + ",***** REFUSED TO FORGIVE " + otherPlayerName + "! *****";
		else
			strOutBuf = strBaseString + ",***** DENOUNCED " + otherPlayerName + "! *****";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

/// Log friend asks another to denounce someone
void CvDiplomacyAI::LogFriendRequestDenounce(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, bool bAgreed)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvString otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		CvString againstPlayerName = GET_PLAYER(eAgainstPlayer).getCivilizationShortDescription();

		// Did they actually agree?
		if(bAgreed)
			strOutBuf = strBaseString + ",***** CONVINCED " + otherPlayerName + " TO DENOUNCE " + againstPlayerName + "! *****";
		else
			strOutBuf = strBaseString + ", ASKED " + otherPlayerName + " TO DENOUNCE " + againstPlayerName + "!";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


/// Log working with a player
void CvDiplomacyAI::LogCoopWar(PlayerTypes ePlayer, PlayerTypes eAgainstPlayer, CoopWarStates eAcceptedState)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvString otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		CvString againstPlayerName = GET_PLAYER(eAgainstPlayer).getCivilizationShortDescription();

		// Did they actually agree?
		if(eAcceptedState == COOP_WAR_STATE_ACCEPTED)
			strOutBuf = strBaseString + ",***** NOW coop war with " + otherPlayerName + " against " + againstPlayerName + "! *****";
		// Asked and player said soon
		else if(eAcceptedState == COOP_WAR_STATE_SOON)
			strOutBuf = strBaseString + ",***** SOON coop war with " + otherPlayerName + " against " + againstPlayerName + "! *****";
		// Asked but player declined
		else if(eAcceptedState == COOP_WAR_STATE_REJECTED)
			strOutBuf = strBaseString + ", wanted coop war with " + otherPlayerName + " against " + againstPlayerName + "!";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

/// Log player wanting a RA
void CvDiplomacyAI::LogWantRA(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + otherPlayerName + ", Wants Research Agreement!";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}


/// Log Opinion Update
void CvDiplomacyAI::LogOpinionUpdate(PlayerTypes ePlayer, std::vector<int>& viOpinionValues)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Approach_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Approach_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strTemp.Format("OPINION");
		strOutBuf += ", " + strTemp;

		MajorCivOpinionTypes eMajorCivOpinion;
		//		MinorCivOpinionTypes eMinorCivOpinion;

		// Major Civs
		if(!GET_PLAYER(ePlayer).isMinorCiv())
		{
			// Current Opinion
			switch(GetMajorCivOpinion(ePlayer))
			{
			case MAJOR_CIV_OPINION_ALLY:
				strTemp.Format("** ALLY **");
				break;
			case MAJOR_CIV_OPINION_FRIEND:
				strTemp.Format("FRIEND");
				break;
			case MAJOR_CIV_OPINION_FAVORABLE:
				strTemp.Format("Favorable");
				break;
			case MAJOR_CIV_OPINION_NEUTRAL:
				strTemp.Format("N");
				break;
			case MAJOR_CIV_OPINION_COMPETITOR:
				strTemp.Format("Competitor");
				break;
			case MAJOR_CIV_OPINION_ENEMY:
				strTemp.Format("ENEMY");
				break;
			case MAJOR_CIV_OPINION_UNFORGIVABLE:
				strTemp.Format("**UNFORGIVABLE**");
				break;
			default:
				strTemp.Format("XXX");
				break;
			}
			strOutBuf += ", " + strTemp;

			for(int iMajorCivOpinionLoop = 0; iMajorCivOpinionLoop < NUM_MAJOR_CIV_OPINION_TYPES; iMajorCivOpinionLoop++)
			{
				eMajorCivOpinion = (MajorCivOpinionTypes) iMajorCivOpinionLoop;

				switch(eMajorCivOpinion)
				{
				case MAJOR_CIV_OPINION_ALLY:
					strTemp.Format("Ally");
					break;
				case MAJOR_CIV_OPINION_FRIEND:
					strTemp.Format("Friend");
					break;
				case MAJOR_CIV_OPINION_NEUTRAL:
					strTemp.Format("Neutral");
					break;
				case MAJOR_CIV_OPINION_COMPETITOR:
					strTemp.Format("Competitor");
					break;
				case MAJOR_CIV_OPINION_ENEMY:
					strTemp.Format("Enemy");
					break;
				default:
					strTemp.Format("XXX");
					break;
				}
				strOutBuf += ", " + strTemp;

				strTemp.Format("%d", viOpinionValues[eMajorCivOpinion]);
				strOutBuf += ", " + strTemp;
			}

			strOutBuf = strBaseString + strOutBuf;
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log Major Civ Approach Update
void CvDiplomacyAI::LogMajorCivApproachUpdate(PlayerTypes ePlayer, const int* aiApproachValues, MajorCivApproachTypes eNewMajorCivApproach, MajorCivApproachTypes eOldApproach, WarFaceTypes eNewWarFace)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Approach_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Approach_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		MajorCivApproachTypes eMajorCivApproach;

		// Major Civs
		if(!GET_PLAYER(ePlayer).isMinorCiv())
		{
			LogMajorCivApproach(strOutBuf, eNewMajorCivApproach, eNewWarFace);

			if(eNewMajorCivApproach != eOldApproach)
			{
				LogMajorCivApproach(strOutBuf, eOldApproach, eNewWarFace);
			}
			else
			{
				strTemp.Format("---");
				strOutBuf += ", " + strTemp;
			}

			LogOpinion(strOutBuf, ePlayer);

			LogWarmongerThreat(strOutBuf, ePlayer);

			LogMilitaryThreat(strOutBuf, ePlayer);

			LogTargetValue(strOutBuf, ePlayer);

			LogWarGoal(strOutBuf, ePlayer);

			LogMilitaryAggressivePosture(strOutBuf, ePlayer);

			LogProximity(strOutBuf, ePlayer);

			LogLandDispute(strOutBuf, ePlayer);

			LogVictoryDispute(strOutBuf, ePlayer);

			LogWonderDispute(strOutBuf, ePlayer);

			LogMinorCivDispute(strOutBuf, ePlayer);

			strTemp.Format("---");
			strOutBuf += ", " + strTemp;

			// Weights for each possible Approach
			for(int iMajorCivApproachLoop = 0; iMajorCivApproachLoop < NUM_MAJOR_CIV_APPROACHES; iMajorCivApproachLoop++)
			{
				eMajorCivApproach = (MajorCivApproachTypes) iMajorCivApproachLoop;

				switch(eMajorCivApproach)
				{
				case MAJOR_CIV_APPROACH_WAR:
					strTemp.Format("War");
					break;
				case MAJOR_CIV_APPROACH_HOSTILE:
					strTemp.Format("Hostile");
					break;
				case MAJOR_CIV_APPROACH_DECEPTIVE:
					strTemp.Format("Deceptive");
					break;
				case MAJOR_CIV_APPROACH_GUARDED:
					strTemp.Format("Guarded");
					break;
				case MAJOR_CIV_APPROACH_AFRAID:
					strTemp.Format("Afraid");
					break;
				case MAJOR_CIV_APPROACH_FRIENDLY:
					strTemp.Format("Friendly");
					break;
				case MAJOR_CIV_APPROACH_NEUTRAL:
					strTemp.Format("Neutral");
					break;
				default:
					strTemp.Format("XXX");
					break;
				}
				strOutBuf += ", " + strTemp;

				strTemp.Format("%d", aiApproachValues[eMajorCivApproach] / 100);
				strOutBuf += ", " + strTemp;
			}

			strOutBuf = strBaseString + strOutBuf;
			pLog->Msg(strOutBuf);
		}
	}
}


/// Log Minor Civ Approach Update
void CvDiplomacyAI::LogMinorCivApproachUpdate(PlayerTypes ePlayer, const int* aiApproachValues, MinorCivApproachTypes eNewMinorCivApproach, MinorCivApproachTypes eOldApproach)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Approach_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Approach_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Name
		strBaseString += playerName;

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strBaseString += ", --- " + otherPlayerName;

		MinorCivApproachTypes eMinorCivApproach;

		// Minor Civs only
		if(GET_PLAYER(ePlayer).isMinorCiv())
		{
			LogMinorCivApproach(strOutBuf, eNewMinorCivApproach);

			if(eNewMinorCivApproach != eOldApproach)
			{
				LogMinorCivApproach(strOutBuf, eOldApproach);
			}
			else
			{
				strTemp.Format("---");
				strOutBuf += ", " + strTemp;
			}

			//LogOpinion(strOutBuf, ePlayer);
			strOutBuf += ", ";

			//LogWarmongerThreat(strOutBuf, ePlayer);
			strOutBuf += ", ";

			//LogMilitaryThreat(strOutBuf, ePlayer);
			strOutBuf += ", ";

			LogTargetValue(strOutBuf, ePlayer);

			LogWarGoal(strOutBuf, ePlayer);

			LogMilitaryAggressivePosture(strOutBuf, ePlayer);

			LogProximity(strOutBuf, ePlayer);

			LogLandDispute(strOutBuf, ePlayer);

			//LogVictoryDispute(strOutBuf, ePlayer);
			strOutBuf += ", ";

			strOutBuf += ", ---";

			// Weights for each possible Approach
			for(int iMinorCivApproachLoop = 0; iMinorCivApproachLoop < NUM_MINOR_CIV_APPROACHES; iMinorCivApproachLoop++)
			{
				eMinorCivApproach = (MinorCivApproachTypes) iMinorCivApproachLoop;

				switch(eMinorCivApproach)
				{
				case MINOR_CIV_APPROACH_IGNORE:
					strTemp.Format("Ignore");
					break;
				case MINOR_CIV_APPROACH_FRIENDLY:
					strTemp.Format("Friendly");
					break;
				case MINOR_CIV_APPROACH_PROTECTIVE:
					strTemp.Format("Protective");
					break;
				case MINOR_CIV_APPROACH_CONQUEST:
					strTemp.Format("Conquest");
					break;
				case MINOR_CIV_APPROACH_BULLY:
					strTemp.Format("Bully");
					break;
				default:
					strTemp.Format("XXX");
					break;
				}
				strOutBuf += ", " + strTemp;

				strTemp.Format("%d", aiApproachValues[eMinorCivApproach] / 100);
				strOutBuf += ", " + strTemp;
			}

			strOutBuf = strBaseString + strOutBuf;
			pLog->Msg(strOutBuf);
		}
	}
}

// Log the Personality of this player (Flavors & Personality Traits)
void CvDiplomacyAI::LogPersonality()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "AI_Personality_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "AI_Personality_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, PERSONALITY, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		// Loop through all Flavors
		for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
		{
			strTemp.Format("%s, %d", GC.getFlavorTypes((FlavorTypes) iFlavorLoop).GetCString(), GetPlayer()->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes) iFlavorLoop));
			strOutBuf = strBaseString + ", " + strTemp;
			pLog->Msg(strOutBuf);
		}

		MajorCivApproachTypes eMajorCivApproach;

		// Loop through all Approaches to see what this player's bias is
		for(int iApproachLoop = 0; iApproachLoop < NUM_MAJOR_CIV_APPROACHES; iApproachLoop++)
		{
			eMajorCivApproach = (MajorCivApproachTypes) iApproachLoop;

			switch(eMajorCivApproach)
			{
			case MAJOR_CIV_APPROACH_WAR:
				strTemp.Format("War");
				break;
			case MAJOR_CIV_APPROACH_HOSTILE:
				strTemp.Format("Hostile");
				break;
			case MAJOR_CIV_APPROACH_DECEPTIVE:
				strTemp.Format("Deceptive");
				break;
			case MAJOR_CIV_APPROACH_GUARDED:
				strTemp.Format("Guarded");
				break;
			case MAJOR_CIV_APPROACH_AFRAID:
				strTemp.Format("Afraid");
				break;
			case MAJOR_CIV_APPROACH_FRIENDLY:
				strTemp.Format("Friendly");
				break;
			case MAJOR_CIV_APPROACH_NEUTRAL:
				strTemp.Format("Neutral");
				break;
			default:
				strTemp.Format("XXX");
				break;
			}
			strOutBuf = strBaseString + ", " + strTemp;

			strTemp.Format("%d", GetPersonalityMajorCivApproachBias(eMajorCivApproach));
			strOutBuf += ", " + strTemp;
			pLog->Msg(strOutBuf);
		}

		MinorCivApproachTypes eMinorCivApproach;

		// Loop through all Approaches to see what this player's bias is
		for(int iApproachLoop = 0; iApproachLoop < NUM_MINOR_CIV_APPROACHES; iApproachLoop++)
		{
			eMinorCivApproach = (MinorCivApproachTypes) iApproachLoop;

			switch(eMinorCivApproach)
			{
			case MINOR_CIV_APPROACH_IGNORE:
				strTemp.Format("Ignore");
				break;
			case MINOR_CIV_APPROACH_FRIENDLY:
				strTemp.Format("Friendly");
				break;
			case MINOR_CIV_APPROACH_PROTECTIVE:
				strTemp.Format("Protective");
				break;
			case MINOR_CIV_APPROACH_CONQUEST:
				strTemp.Format("Conquest");
				break;
			case MINOR_CIV_APPROACH_BULLY:
				strTemp.Format("Bully");
				break;
			default:
				strTemp.Format("XXX");
				break;
			}
			strOutBuf = strBaseString + ", " + strTemp;

			strTemp.Format("%d", GetPersonalityMinorCivApproachBias(eMinorCivApproach));
			strOutBuf += ", " + strTemp;
			pLog->Msg(strOutBuf);
		}

		strTemp.Format("VICTORY COMPETITIVENESS, %d", GetVictoryCompetitiveness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("WONDER COMPETITIVENESS, %d", GetWonderCompetitiveness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("MINOR CIV COMPETITIVENESS, %d", GetMinorCivCompetitiveness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("BOLDNESS, %d", GetBoldness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("DIPLO BALANCE, %d", GetDiploBalance());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("WARMONGER HATE, %d", GetWarmongerHate());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("DENOUNCE, %d", GetDenounceWillingness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("DoF, %d", GetDoFWillingness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("LOYALTY, %d", GetLoyalty());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("NEEDINESS, %d", GetNeediness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("FORGIVENESS, %d", GetForgiveness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("CHATTINESS, %d", GetChattiness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);

		strTemp.Format("MEANNESS, %d", GetMeanness());
		strOutBuf = strBaseString + ", " + strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Log diplomatic status with other Players
void CvDiplomacyAI::LogStatus()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strMinorString;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		int iPlayerLoop;
		PlayerTypes eLoopPlayer;

		// Loop through all (known) Players
		for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(IsPlayerValid(eLoopPlayer))
			{
				strOutBuf = strBaseString;

				LogGrandStrategy(strOutBuf);

				// Name
				if(GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					strMinorString = "--- ";
				}

				otherPlayerName = GET_PLAYER(eLoopPlayer).getCivilizationShortDescription();
				strOutBuf += ", " + strMinorString + otherPlayerName;

				// Major Civ
				if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					if(IsDoFAccepted(eLoopPlayer))
						strOutBuf += ", WW";
					else
						strOutBuf += ", ";

					if(IsDenouncedPlayer(eLoopPlayer))
						strOutBuf += ", DEN";
					else
						strOutBuf += ", ";

					//if (IsWorkingAgainstPlayer(eLoopPlayer))
					//	strOutBuf += ", WA";
					//else
					//	strOutBuf += ", ";

					if(GetGlobalCoopWarAcceptedState(eLoopPlayer) == COOP_WAR_STATE_ACCEPTED)
						strOutBuf += ", CW";
					else if(GetGlobalCoopWarAcceptedState(eLoopPlayer) == COOP_WAR_STATE_SOON)
						strOutBuf += ", CWS";
					else
						strOutBuf += ", ";

					LogMajorCivApproach(strOutBuf, GetMajorCivApproach(eLoopPlayer, /*bHideTrueFeelings*/ false), GetWarFaceWithPlayer(eLoopPlayer));
					LogOpinion(strOutBuf, eLoopPlayer);
				}
				// Minor Civ
				else
				{
					strOutBuf += ", ";		// Working With
					strOutBuf += ", ";		// Working Against
					strOutBuf += ", ";		// Coop War

					LogMinorCivApproach(strOutBuf, GetMinorCivApproach(eLoopPlayer));
					strOutBuf += ", ";		// Opinion
				}

				LogProximity(strOutBuf, eLoopPlayer);

				LogWarState(strOutBuf, eLoopPlayer);

				LogEconomicStrength(strOutBuf, eLoopPlayer);

				LogMilitaryStrength(strOutBuf, eLoopPlayer);
				LogTargetValue(strOutBuf, eLoopPlayer);
				LogMilitaryAggressivePosture(strOutBuf, eLoopPlayer);
				LogWarmongerThreat(strOutBuf, eLoopPlayer);
				LogMilitaryThreat(strOutBuf, eLoopPlayer);

				LogExpansionAggressivePosture(strOutBuf, eLoopPlayer);
				LogPlotBuyingAggressivePosture(strOutBuf, eLoopPlayer);
				LogLandDispute(strOutBuf, eLoopPlayer);
				LogVictoryDispute(strOutBuf, eLoopPlayer);
				LogWonderDispute(strOutBuf, eLoopPlayer);
				LogMinorCivDispute(strOutBuf, eLoopPlayer);

				// Other Player's Estimated Grand Strategy
				if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					AIGrandStrategyTypes eGrandStrategy = GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopPlayer);

					CvAIGrandStrategyXMLEntry* pEntry = (eGrandStrategy != NO_AIGRANDSTRATEGY)? GC.getAIGrandStrategyInfo(eGrandStrategy) : NULL;
					if(pEntry != NULL)
					{
						strTemp.Format("GSS %s", pEntry->GetType());
						strOutBuf += ", " + strTemp;

						// Confidence in our guess
						switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopPlayer))
						{
						case GUESS_CONFIDENCE_POSITIVE:
							strTemp.Format("POSITIVE");
							break;
						case GUESS_CONFIDENCE_LIKELY:
							strTemp.Format("Likely");
							break;
						case GUESS_CONFIDENCE_UNSURE:
							strTemp.Format("Unsure");
							break;
						default:
							strTemp.Format("XXX");
							break;
						}
					}
					else
					{
						strTemp.Format("No GS Guess, ");
					}
				}
				else
				{
					strTemp.Format(", ");
				}
				strOutBuf += ", " + strTemp;

				pLog->Msg(strOutBuf);
			}
		}
	}
}


/// Log Diplomatic War status with other Players
void CvDiplomacyAI::LogWarStatus()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strMinorString;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_War_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_War_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		int iPlayerLoop;
		PlayerTypes eLoopPlayer;

		bool bLogPlayer;

		// Loop through all (known) Players
		for(iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(IsPlayerValid(eLoopPlayer))
			{
				if(IsAtWar(eLoopPlayer))
				{
					bLogPlayer = true;
				}
				else if(GetWarState(eLoopPlayer) != NO_WAR_STATE_TYPE)
				{
					bLogPlayer = true;
				}
				else if(GetWarGoal(eLoopPlayer) != NO_WAR_GOAL_TYPE)
				{
					bLogPlayer = true;
				}
				else
				{
					bLogPlayer = false;
				}

				// Should we actually record a log entry for this player?
				if(bLogPlayer)
				{
					strOutBuf = strBaseString;

					// Name
					if(GET_PLAYER(eLoopPlayer).isMinorCiv())
					{
						strMinorString = "--- ";
					}

					otherPlayerName = GET_PLAYER(eLoopPlayer).getCivilizationShortDescription();
					strOutBuf += ", " + strMinorString + otherPlayerName;

					// Approach
					if(GET_PLAYER(eLoopPlayer).isMinorCiv())
					{
						if(GetMinorCivApproach(eLoopPlayer) == MINOR_CIV_APPROACH_CONQUEST)
						{
							strOutBuf += ", APP: CONQUEST";
						}
						else
						{
							strOutBuf += ", APP: o";
						}
					}
					else
					{
						if(GetMajorCivApproach(eLoopPlayer, false) == MAJOR_CIV_APPROACH_WAR)
						{
							strOutBuf += ", APP: WAR";
						}
						else
						{
							strOutBuf += ", APP: o";
						}
					}

					// Wants to conquer the world?
					if(IsGoingForWorldConquest())
					{
						strOutBuf += ", WC";
					}
					else
					{
						strOutBuf += ", ";
					}

					// Wants peace with eLoopPlayer?
					if(IsWantsPeaceWithPlayer(eLoopPlayer))
					{
						strOutBuf += ", Wants Peace!!!";
					}
					else
					{
						strOutBuf += ", ";
					}

					LogWarGoal(strOutBuf, eLoopPlayer);
					LogWarState(strOutBuf, eLoopPlayer);
					LogWarProjection(strOutBuf, eLoopPlayer);

					LogWarPeaceWillingToOffer(strOutBuf, eLoopPlayer);
					LogWarPeaceWillingToAccept(strOutBuf, eLoopPlayer);

					// # of turns at War
					if(GetPlayerNumTurnsAtWar(eLoopPlayer) > 0)
					{
						strTemp.Format("%d", GetPlayerNumTurnsAtWar(eLoopPlayer));
						strOutBuf += ", " + strTemp;
					}
					else
					{
						strOutBuf += ", ";
					}

					LogWarDamage(strOutBuf, eLoopPlayer);

					LogMilitaryThreat(strOutBuf, eLoopPlayer);

					LogProximity(strOutBuf, eLoopPlayer);
					LogTargetValue(strOutBuf, eLoopPlayer);

					LogMilitaryStrength(strOutBuf, eLoopPlayer);
					LogEconomicStrength(strOutBuf, eLoopPlayer);

					pLog->Msg(strOutBuf);
				}
			}
		}
	}
}


/// Log Statements, to make sure our record is solid
void CvDiplomacyAI::LogStatements()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strMinorString;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Statement_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Statement_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		int iPlayerLoop;
		PlayerTypes eLoopPlayer;

		CvString strStatementLine;

		int iItem;
		DiploStatementTypes eStatement;
		int iTurn;

		// Loop through all (known) Players
		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(IsPlayerValid(eLoopPlayer))
			{
				otherPlayerName = GET_PLAYER(eLoopPlayer).getCivilizationShortDescription();

				// Diplo Statement Log Counter
				for(iItem = 0; iItem < MAX_DIPLO_LOG_STATEMENTS; iItem++)
				{
					eStatement = GetDiploLogStatementTypeForIndex(eLoopPlayer, iItem);

					if(eStatement != NO_DIPLO_STATEMENT_TYPE)
					{
						iTurn = GetDiploLogStatementTurnForIndex(eLoopPlayer, iItem);

						strStatementLine.Format(", Statement: %d, Index: %d, Turn %d", eStatement, iItem, iTurn);

						strOutBuf = strBaseString;

						strOutBuf += ", " + otherPlayerName;
						strOutBuf += strStatementLine;

						pLog->Msg(strOutBuf);
					}
				}
			}
		}
	}
}

/// Log our guess as to another player's Expansion Flavor preference
void CvDiplomacyAI::LogOtherPlayerExpansionGuess(PlayerTypes ePlayer, int iGuess)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Guess_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Guess_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + "EXPANSION, " + otherPlayerName;

		CvString strExpansionGuess;
		strExpansionGuess.Format(", %d", iGuess);
		strOutBuf += strExpansionGuess;

		pLog->Msg(strOutBuf);
	}
}

/// Log our guesses as to what other Players are up to
void CvDiplomacyAI::LogOtherPlayerGuessStatus()
{
	if((int)m_eTargetPlayer >= (int)DIPLO_FIRST_PLAYER)
		return;

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString thirdPlayerName;
		CvString strOtherPlayerGrandStrategy;
		CvString strThirdPlayerGrandStrategy;
		CvString strDesc;
		CvString strLogName;
		CvString strTemp;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Guess_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Guess_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName;

		AIGrandStrategyTypes eGrandStrategy;

		int iPlayerLoop;
		PlayerTypes eLoopPlayer;
		int iOtherPlayerLoop;
		PlayerTypes eLoopOtherPlayer;

		// Loop through all (known) Players
		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			if(IsPlayerValid(eLoopPlayer))
			{
				otherPlayerName = GET_PLAYER(eLoopPlayer).getCivilizationShortDescription();

				eGrandStrategy = GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopPlayer);

				CvAIGrandStrategyXMLEntry* pEntry = (eGrandStrategy != NO_AIGRANDSTRATEGY)? GC.getAIGrandStrategyInfo(eGrandStrategy) : NULL;
				if(pEntry != NULL)
				{
					strTemp.Format("%s", pEntry->GetType());
					strOtherPlayerGrandStrategy = strTemp;

					// Confidence in our guess
					switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopPlayer))
					{
					case GUESS_CONFIDENCE_POSITIVE:
						strTemp.Format("POSITIVE");
						break;
					case GUESS_CONFIDENCE_LIKELY:
						strTemp.Format("Likely");
						break;
					case GUESS_CONFIDENCE_UNSURE:
						strTemp.Format("unsure");
						break;
					default:
						strTemp.Format("XXX");
						break;
					}

					strOtherPlayerGrandStrategy += ", " + strTemp;
				}
				else
				{
					strOtherPlayerGrandStrategy.Format("No GS Guess, ");
				}

				// Now loop through other players this guy knows
				for(iOtherPlayerLoop = 0; iOtherPlayerLoop < MAX_MAJOR_CIVS; iOtherPlayerLoop++)
				{
					eLoopOtherPlayer = (PlayerTypes) iOtherPlayerLoop;

					// Don't check a player against himself
					if(eLoopPlayer != eLoopOtherPlayer)
					{
						// Do both we and the guy we're looking about know the third guy?
						if(IsPlayerValid(eLoopOtherPlayer, true) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopOtherPlayer))
						{
							thirdPlayerName = GET_PLAYER(eLoopOtherPlayer).getCivilizationShortDescription();

							eGrandStrategy = GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategy(eLoopOtherPlayer);
							if(eGrandStrategy != NO_AIGRANDSTRATEGY)
							{
								strTemp.Format("%s", GC.getAIGrandStrategyInfo(eGrandStrategy)->GetType());
								strThirdPlayerGrandStrategy = strTemp;

								// Confidence in our guess
								switch(GetPlayer()->GetGrandStrategyAI()->GetGuessOtherPlayerActiveGrandStrategyConfidence(eLoopOtherPlayer))
								{
								case GUESS_CONFIDENCE_POSITIVE:
									strTemp.Format("Positive");
									break;
								case GUESS_CONFIDENCE_LIKELY:
									strTemp.Format("Likely");
									break;
								case GUESS_CONFIDENCE_UNSURE:
									strTemp.Format("Unsure");
									break;
								default:
									strTemp.Format("XXX");
									break;
								}

								strThirdPlayerGrandStrategy += ", " + strTemp;
							}
							else
							{
								strThirdPlayerGrandStrategy.Format("No GS Guess, ");
							}

							strOutBuf = strBaseString + ", " + otherPlayerName + ", " + strOtherPlayerGrandStrategy + ", " + thirdPlayerName + ", " + strThirdPlayerGrandStrategy;

							// Overall Opinion
							switch(GetMajorCivOtherPlayerOpinion(eLoopPlayer, eLoopOtherPlayer))
							{
							case MAJOR_CIV_OPINION_ALLY:
								strTemp.Format("OPN **ALLY**");
								break;
							case MAJOR_CIV_OPINION_FRIEND:
								strTemp.Format("OPN FRIEND");
								break;
							case MAJOR_CIV_OPINION_FAVORABLE:
								strTemp.Format("OPN Favorable");
								break;
							case MAJOR_CIV_OPINION_NEUTRAL:
								strTemp.Format("OPN N");
								break;
							case MAJOR_CIV_OPINION_COMPETITOR:
								strTemp.Format("OPN Competitor");
								break;
							case MAJOR_CIV_OPINION_ENEMY:
								strTemp.Format("OPN ENEMY");
								break;
							case MAJOR_CIV_OPINION_UNFORGIVABLE:
								strTemp.Format("OPN **UNFORGIVABLE**");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;

							// Military Threat
							switch(GetEstimateOtherPlayerMilitaryThreat(eLoopPlayer, eLoopOtherPlayer))
							{
							case THREAT_CRITICAL:
								strTemp.Format("M_THT **CRITICAL**");
								break;
							case THREAT_SEVERE:
								strTemp.Format("M_THT SEVERE");
								break;
							case THREAT_MAJOR:
								strTemp.Format("M_THT Major");
								break;
							case THREAT_MINOR:
								strTemp.Format("M_THT mnr");
								break;
							case THREAT_NONE:
								strTemp.Format("M_THT N");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;

							// War Damage
							switch(GetOtherPlayerWarDamageLevel(eLoopPlayer, eLoopOtherPlayer))
							{
							case WAR_DAMAGE_LEVEL_CRIPPLED:
								strTemp.Format("W_DMG **CRIPPLED*");
								break;
							case WAR_DAMAGE_LEVEL_SERIOUS:
								strTemp.Format("W_DMG SERIOUS");
								break;
							case WAR_DAMAGE_LEVEL_MAJOR:
								strTemp.Format("W_DMG Major");
								break;
							case WAR_DAMAGE_LEVEL_MINOR:
								strTemp.Format("W_DMG mnr");
								break;
							case WAR_DAMAGE_LEVEL_NONE:
								strTemp.Format("");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;

							// Proximity
							switch(GET_PLAYER(eLoopPlayer).GetProximityToPlayer(eLoopOtherPlayer))
							{
							case PLAYER_PROXIMITY_NEIGHBORS:
								strTemp.Format("PRX Neighbors");
								break;
							case PLAYER_PROXIMITY_CLOSE:
								strTemp.Format("PRX Close");
								break;
							case PLAYER_PROXIMITY_FAR:
								strTemp.Format("PRX Far");
								break;
							case PLAYER_PROXIMITY_DISTANT:
								strTemp.Format("PRX Distant");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;

							// Land Dispute
							switch(GetEstimateOtherPlayerLandDisputeLevel(eLoopPlayer, eLoopOtherPlayer))
							{
							case DISPUTE_LEVEL_FIERCE:
								strTemp.Format("LND **FIERCE**");
								break;
							case DISPUTE_LEVEL_STRONG:
								strTemp.Format("LND STRONG");
								break;
							case DISPUTE_LEVEL_WEAK:
								strTemp.Format("LND Weak");
								break;
							case DISPUTE_LEVEL_NONE:
								strTemp.Format("LND N");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;

							// Victory Dispute
							switch(GetEstimateOtherPlayerVictoryDisputeLevel(eLoopPlayer, eLoopOtherPlayer))
							{
							case DISPUTE_LEVEL_FIERCE:
								strTemp.Format("VCT **FIERCE**");
								break;
							case DISPUTE_LEVEL_STRONG:
								strTemp.Format("VCT STRONG");
								break;
							case DISPUTE_LEVEL_WEAK:
								strTemp.Format("VCT Weak");
								break;
							case DISPUTE_LEVEL_NONE:
								strTemp.Format("VCT N");
								break;
							default:
								strTemp.Format("XXX");
								break;
							}
							strOutBuf += ", " + strTemp;

							pLog->Msg(strOutBuf);
						}
					}
				}
			}
		}
	}
}

/// Log Grand Strategy
void CvDiplomacyAI::LogGrandStrategy(CvString& strString)
{
	CvString strTemp;

	AIGrandStrategyTypes eGrandStrategy =GetPlayer()->GetGrandStrategyAI()->GetActiveGrandStrategy();

	if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST"))
	{
		strTemp.Format("WC");
	}
	else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP"))
	{
		strTemp.Format("Spaceship");
	}
	else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS"))
	{
		strTemp.Format("Diplomacy");
	}
	else if(eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
	{
		strTemp.Format("Culture");
	}

	strString += ", " + strTemp;
}

/// Log Current Approach towards Major
void CvDiplomacyAI::LogMajorCivApproach(CvString& strString, MajorCivApproachTypes eNewMajorCivApproach, WarFaceTypes eNewWarFace)
{
	CvString strTemp;

	switch(eNewMajorCivApproach)
	{
	case MAJOR_CIV_APPROACH_WAR:
		switch(eNewWarFace)
		{
		case WAR_FACE_HOSTILE:
			strTemp.Format("** WAR (H) **");
			break;
		case WAR_FACE_NEUTRAL:
			strTemp.Format("** WAR (N) **");
			break;
		case WAR_FACE_FRIENDLY:
			strTemp.Format("** WAR (F) **");
			break;
		default:
			strTemp.Format("** WAR **");
			break;
		}
		break;
	case MAJOR_CIV_APPROACH_HOSTILE:
		strTemp.Format("HOSTILE");
		break;
	case MAJOR_CIV_APPROACH_DECEPTIVE:
		strTemp.Format("DECEPTIVE");
		break;
	case MAJOR_CIV_APPROACH_GUARDED:
		strTemp.Format("GUARDED");
		break;
	case MAJOR_CIV_APPROACH_AFRAID:
		strTemp.Format("AFRAID");
		break;
	case MAJOR_CIV_APPROACH_FRIENDLY:
		strTemp.Format("FRIENDLY");
		break;
	case MAJOR_CIV_APPROACH_NEUTRAL:
		strTemp.Format("N");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}


/// Log Current Approach towards Minor
void CvDiplomacyAI::LogMinorCivApproach(CvString& strString, MinorCivApproachTypes eNewMinorCivApproach)
{
	CvString strTemp;

	switch(eNewMinorCivApproach)
	{
	case MINOR_CIV_APPROACH_IGNORE:
		strTemp.Format("IG");
		break;
	case MINOR_CIV_APPROACH_FRIENDLY:
		strTemp.Format("FRIENDLY");
		break;
	case MINOR_CIV_APPROACH_PROTECTIVE:
		strTemp.Format("PROTECTIVE");
		break;
	case MINOR_CIV_APPROACH_CONQUEST:
		strTemp.Format("** CONQUEST **");
		break;
	case MINOR_CIV_APPROACH_BULLY:
		strTemp.Format("BULLY");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}

/// Log a quest from a Minor
void CvDiplomacyAI::LogMinorCivQuestType(CvString& strString, MinorCivQuestTypes eQuestType)
{
	CvString strTemp;

	switch(eQuestType)
	{
	case MINOR_CIV_QUEST_ROUTE:
		strTemp.Format("Route");
		break;
	case MINOR_CIV_QUEST_KILL_CAMP:
		strTemp.Format("Kill Camp");
		break;
	case MINOR_CIV_QUEST_CONNECT_RESOURCE:
		strTemp.Format("Connect Resource");
		break;
	case MINOR_CIV_QUEST_CONSTRUCT_WONDER:
		strTemp.Format("Construct Wonder");
		break;
	case MINOR_CIV_QUEST_GREAT_PERSON:
		strTemp.Format("Great Person");
		break;
	case MINOR_CIV_QUEST_KILL_CITY_STATE:
		strTemp.Format("Kill City State");
		break;
	case MINOR_CIV_QUEST_FIND_PLAYER:
		strTemp.Format("Find Player");
		break;
	case MINOR_CIV_QUEST_FIND_NATURAL_WONDER:
		strTemp.Format("Find Natural Wonder");
		break;
	case MINOR_CIV_QUEST_GIVE_GOLD:
		strTemp.Format("Give Gold");
		break;
	case MINOR_CIV_QUEST_PLEDGE_TO_PROTECT:
		strTemp.Format("Pledge to Protect");
		break;
	case MINOR_CIV_QUEST_CONTEST_CULTURE:
		strTemp.Format("Contest Culture");
		break;
	case MINOR_CIV_QUEST_CONTEST_FAITH:
		strTemp.Format("Contest Faith");
		break;
	case MINOR_CIV_QUEST_CONTEST_TECHS:
		strTemp.Format("Contest Techs");
		break;
	case MINOR_CIV_QUEST_INVEST:
		strTemp.Format("Invest");
		break;
	case MINOR_CIV_QUEST_BULLY_CITY_STATE:
		strTemp.Format("Bully City-State");
		break;
	case MINOR_CIV_QUEST_DENOUNCE_MAJOR:
		strTemp.Format("Denounce Major");
		break;
	case MINOR_CIV_QUEST_SPREAD_RELIGION:
		strTemp.Format("Spread Religion");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}

	strString += ", " + strTemp;
}

/// Log Current Opinion of Major
void CvDiplomacyAI::LogOpinion(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Opinion
	switch(GetMajorCivOpinion(ePlayer))
	{
	case MAJOR_CIV_OPINION_ALLY:
		strTemp.Format("OPN **ALLY**");
		break;
	case MAJOR_CIV_OPINION_FRIEND:
		strTemp.Format("OPN FRIEND");
		break;
	case MAJOR_CIV_OPINION_FAVORABLE:
		strTemp.Format("OPN Favorable");
		break;
	case MAJOR_CIV_OPINION_NEUTRAL:
		strTemp.Format("OPN N");
		break;
	case MAJOR_CIV_OPINION_COMPETITOR:
		strTemp.Format("OPN Competitor");
		break;
	case MAJOR_CIV_OPINION_ENEMY:
		strTemp.Format("OPN ENEMY");
		break;
	case MAJOR_CIV_OPINION_UNFORGIVABLE:
		strTemp.Format("OPN **UNFORGIVABLE**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}

/// Log Warmonger Threat
void CvDiplomacyAI::LogWarmongerThreat(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Don't fill out this field for Minors, as it just makes the log harder to read
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		switch(GetWarmongerThreat(ePlayer))
		{
		case THREAT_CRITICAL:
			strTemp.Format("W_THT **CRITICAL**");
			break;
		case THREAT_SEVERE:
			strTemp.Format("W_THT SEVERE");
			break;
		case THREAT_MAJOR:
			strTemp.Format("W_THT Major");
			break;
		case THREAT_MINOR:
			strTemp.Format("W_THT mnr");
			break;
		case THREAT_NONE:
			strTemp.Format("W_THT N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}

/// Log Military Threat
void CvDiplomacyAI::LogMilitaryThreat(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Don't fill out this field for Minors, as it just makes the log harder to read
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		switch(GetMilitaryThreat(ePlayer))
		{
		case THREAT_CRITICAL:
			strTemp.Format("M_THT **CRITICAL**");
			break;
		case THREAT_SEVERE:
			strTemp.Format("M_THT SEVERE");
			break;
		case THREAT_MAJOR:
			strTemp.Format("M_THT Major");
			break;
		case THREAT_MINOR:
			strTemp.Format("M_THT mnr");
			break;
		case THREAT_NONE:
			strTemp.Format("M_THT N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}

/// Log Military Strength
void CvDiplomacyAI::LogMilitaryStrength(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Military Strength
	switch(GetPlayerMilitaryStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_IMMENSE:
		strTemp.Format("Their MSTR **IMMENSE**");
		break;
	case STRENGTH_POWERFUL:
		strTemp.Format("Their MSTR POWERFUL");
		break;
	case STRENGTH_STRONG:
		strTemp.Format("Their MSTR Strong");
		break;
	case STRENGTH_AVERAGE:
		strTemp.Format("Their MSTR A");
		break;
	case STRENGTH_POOR:
		strTemp.Format("Their MSTR Poor");
		break;
	case STRENGTH_WEAK:
		strTemp.Format("Their MSTR WEAK");
		break;
	case STRENGTH_PATHETIC:
		strTemp.Format("Their MSTR **PATHETIC**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;

}

/// Log Economic Strength
void CvDiplomacyAI::LogEconomicStrength(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Economic Strength
	switch(GetPlayerEconomicStrengthComparedToUs(ePlayer))
	{
	case STRENGTH_IMMENSE:
		strTemp.Format("Their ESTR **IMMENSE**");
		break;
	case STRENGTH_POWERFUL:
		strTemp.Format("Their ESTR POWERFUL");
		break;
	case STRENGTH_STRONG:
		strTemp.Format("Their ESTR Strong");
		break;
	case STRENGTH_AVERAGE:
		strTemp.Format("Their ESTR A");
		break;
	case STRENGTH_POOR:
		strTemp.Format("Their ESTR Poor");
		break;
	case STRENGTH_WEAK:
		strTemp.Format("Their ESTR Weak");
		break;
	case STRENGTH_PATHETIC:
		strTemp.Format("Their ESTR Pathetic");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;

}

/// Log Target Value
void CvDiplomacyAI::LogTargetValue(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Target Value
	switch(GetPlayerTargetValue(ePlayer))
	{
	case TARGET_VALUE_IMPOSSIBLE:
		strTemp.Format("TGT Impossible");
		break;
	case TARGET_VALUE_BAD:
		strTemp.Format("TGT Bad");
		break;
	case TARGET_VALUE_AVERAGE:
		strTemp.Format("TGT A");
		break;
	case TARGET_VALUE_FAVORABLE:
		strTemp.Format("TGT Favorable");
		break;
	case TARGET_VALUE_SOFT:
		strTemp.Format("TGT Soft");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}

/// Log War Goal
void CvDiplomacyAI::LogWarGoal(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// War Goal
	switch(GetWarGoal(ePlayer))
	{
	case WAR_GOAL_DEMAND:
		strTemp.Format("WGL Demand");
		break;
	case WAR_GOAL_PREPARE:
		strTemp.Format("WGL Preparing");
		break;
	case WAR_GOAL_CONQUEST:
		strTemp.Format("WGL Conquest");
		break;
	case WAR_GOAL_DAMAGE:
		strTemp.Format("WGL Damage");
		break;
	case WAR_GOAL_PEACE:
		strTemp.Format("WGL PEACE");
		break;
	default:
		if(IsAtWar(ePlayer))
		{
			strTemp.Format("XXX");
		}
		else
		{
			strTemp.Format("");
		}
		break;
	}
	strString += ", " + strTemp;
}

/// Log Peace Treaty Willing to Offer
void CvDiplomacyAI::LogWarPeaceWillingToOffer(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// No Minor Civs
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		// War Goal
		switch(GetTreatyWillingToOffer(ePlayer))
		{
		case NO_PEACE_TREATY_TYPE:
			strTemp.Format("");
			break;
		case PEACE_TREATY_WHITE_PEACE:
			strTemp.Format("PeaceOff White Peace (0)");
			break;
		case PEACE_TREATY_ARMISTICE:
			strTemp.Format("PeaceOff Armistice (1)");
			break;
		case PEACE_TREATY_SETTLEMENT:
			strTemp.Format("PeaceOff Settlement (2)");
			break;
		case PEACE_TREATY_BACKDOWN:
			strTemp.Format("PeaceOff Backdown (3)");
			break;
		case PEACE_TREATY_SUBMISSION:
			strTemp.Format("PeaceOff Submission (4)");
			break;
		case PEACE_TREATY_SURRENDER:
			strTemp.Format("PeaceOff Surrender (5)");
			break;
		case PEACE_TREATY_CESSION:
			strTemp.Format("PeaceOff Cession (6)");
			break;
		case PEACE_TREATY_CAPITULATION:
			strTemp.Format("PeaceOff CAPITULATION (7)");
			break;
		case PEACE_TREATY_UNCONDITIONAL_SURRENDER:
			strTemp.Format("PeaceOff **UN SURRENDER (8)**");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}

/// Log Peace Treaty Willing to Accept
void CvDiplomacyAI::LogWarPeaceWillingToAccept(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// No Minor Civs
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		// War Goal
		switch(GetTreatyWillingToAccept(ePlayer))
		{
		case NO_PEACE_TREATY_TYPE:
			strTemp.Format("");
			break;
		case PEACE_TREATY_WHITE_PEACE:
			strTemp.Format("PeaceAcc White Peace (0)");
			break;
		case PEACE_TREATY_ARMISTICE:
			strTemp.Format("PeaceAcc Armistice (1)");
			break;
		case PEACE_TREATY_SETTLEMENT:
			strTemp.Format("PeaceAcc Settlement (2)");
			break;
		case PEACE_TREATY_BACKDOWN:
			strTemp.Format("PeaceAcc Backdown (3)");
			break;
		case PEACE_TREATY_SUBMISSION:
			strTemp.Format("PeaceAcc Submission (4)");
			break;
		case PEACE_TREATY_SURRENDER:
			strTemp.Format("PeaceAcc Surrender (5)");
			break;
		case PEACE_TREATY_CESSION:
			strTemp.Format("PeaceAcc Cession (6)");
			break;
		case PEACE_TREATY_CAPITULATION:
			strTemp.Format("PeaceAcc CAPITULATION (7)");
			break;
		case PEACE_TREATY_UNCONDITIONAL_SURRENDER:
			strTemp.Format("PeaceAcc **UN SURRENDER (8)**");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}


/// Log War State
void CvDiplomacyAI::LogWarState(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	bool bShowOperationProgress = false;

	// Log progress towards Sneak Attack Operation launch if we're not yet at war
	if(GetWarGoal(ePlayer) == WAR_GOAL_PREPARE)
	{
		CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetSneakAttackOperation(ePlayer);

		if(pOperation)
		{
			bShowOperationProgress = true;
			int iOperationPercentMustered = pOperation->PercentFromMusterPointToTarget();

			if(iOperationPercentMustered > 0)
			{
				strTemp.Format("PREP PRGS %2d", iOperationPercentMustered);
			}
			else
			{
				strTemp.Format("PREP Gathering");
			}
		}
	}
	// Preparing a demand?
	if(GetWarGoal(ePlayer) == WAR_GOAL_DEMAND)
	{
		CvAIOperation* pOperation = GetPlayer()->GetMilitaryAI()->GetShowOfForceOperation(ePlayer);

		if(pOperation)
		{
			bShowOperationProgress = true;
			int iOperationPercentMustered = pOperation->PercentFromMusterPointToTarget();

			if(iOperationPercentMustered > 0)
			{
				strTemp.Format("DMND PRGS %2d", iOperationPercentMustered);
			}
			else
			{
				strTemp.Format("DMND Gathering");
			}
		}
	}

	if(!bShowOperationProgress)
	{
		// War State
		switch(GetWarState(ePlayer))
		{
		case WAR_STATE_CALM:
			strTemp.Format("WST Calm");
			break;
		case WAR_STATE_NEARLY_WON:
			strTemp.Format("WST Nearly Won");
			break;
		case WAR_STATE_OFFENSIVE:
			strTemp.Format("WST Offensive");
			break;
		case WAR_STATE_STALEMATE:
			strTemp.Format("WST Stalemate");
			break;
		case WAR_STATE_DEFENSIVE:
			strTemp.Format("WST DEFENSIVE");
			break;
		case WAR_STATE_NEARLY_DEFEATED:
			strTemp.Format("WST **NEARLY DEFEATED**");
			break;
		default:
			if(IsAtWar(ePlayer))
			{
				strTemp.Format("XXX");
			}
			else
			{
				strTemp.Format("");
			}
			break;
		}
	}

	strString += ", " + strTemp;
}

/// Log War Projection
void CvDiplomacyAI::LogWarProjection(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// War Projection
	switch(GetWarProjection(ePlayer))
	{
	case WAR_PROJECTION_DESTRUCTION:
		strTemp.Format("W_PRJ **DESTRUCTION**");
		break;
	case WAR_PROJECTION_DEFEAT:
		strTemp.Format("W_PRJ DEFEAT");
		break;
	case WAR_PROJECTION_STALEMATE:
		strTemp.Format("W_PRJ Stalemate");
		break;
	case WAR_PROJECTION_UNKNOWN:
		strTemp.Format("W_PRJ U");
		break;
	case WAR_PROJECTION_GOOD:
		strTemp.Format("W_PRJ GOOD");
		break;
	case WAR_PROJECTION_VERY_GOOD:
		strTemp.Format("W_PRJ ** VERY GOOD **");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}

/// Log War Damage
void CvDiplomacyAI::LogWarDamage(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// War Damage
	switch(GetWarDamageLevel(ePlayer))
	{
	case WAR_DAMAGE_LEVEL_CRIPPLED:
		strTemp.Format("W_DMG **CRIPPLED**");
		break;
	case WAR_DAMAGE_LEVEL_SERIOUS:
		strTemp.Format("W_DMG SERIOUS");
		break;
	case WAR_DAMAGE_LEVEL_MAJOR:
		strTemp.Format("W_DMG Major");
		break;
	case WAR_DAMAGE_LEVEL_MINOR:
		strTemp.Format("W_DMG mnr");
		break;
	case WAR_DAMAGE_LEVEL_NONE:
		strTemp.Format("");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}

/// Log Military Aggressive Posture
void CvDiplomacyAI::LogMilitaryAggressivePosture(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Military Aggressive Posture
	switch(GetMilitaryAggressivePosture(ePlayer))
	{
	case AGGRESSIVE_POSTURE_NONE:
		strTemp.Format("");
		break;
	case AGGRESSIVE_POSTURE_LOW:
		strTemp.Format("MAGG Low");
		break;
	case AGGRESSIVE_POSTURE_MEDIUM:
		strTemp.Format("MAGG Medium");
		break;
	case AGGRESSIVE_POSTURE_HIGH:
		strTemp.Format("MAGG HIGH");
		break;
	case AGGRESSIVE_POSTURE_INCREDIBLE:
		strTemp.Format("MAGG **INCREDIBLE**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}

/// Log Expansion Aggressive Posture
void CvDiplomacyAI::LogExpansionAggressivePosture(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Expansion Aggressive Posture
	switch(GetExpansionAggressivePosture(ePlayer))
	{
	case AGGRESSIVE_POSTURE_NONE:
		strTemp.Format("");
		break;
	case AGGRESSIVE_POSTURE_LOW:
		strTemp.Format("EAGG Low");
		break;
	case AGGRESSIVE_POSTURE_MEDIUM:
		strTemp.Format("EAGG Medium");
		break;
	case AGGRESSIVE_POSTURE_HIGH:
		strTemp.Format("EAGG HIGH");
		break;
	case AGGRESSIVE_POSTURE_INCREDIBLE:
		strTemp.Format("EAGG **INCREDIBLE**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}

/// Log Plot Buying Aggressive Posture
void CvDiplomacyAI::LogPlotBuyingAggressivePosture(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Plot Buying Aggressive Posture
	switch(GetPlotBuyingAggressivePosture(ePlayer))
	{
	case AGGRESSIVE_POSTURE_NONE:
		strTemp.Format("");
		break;
	case AGGRESSIVE_POSTURE_LOW:
		strTemp.Format("BAGG Low");
		break;
	case AGGRESSIVE_POSTURE_MEDIUM:
		strTemp.Format("BAGG Medium");
		break;
	case AGGRESSIVE_POSTURE_HIGH:
		strTemp.Format("BAGG HIGH");
		break;
	case AGGRESSIVE_POSTURE_INCREDIBLE:
		strTemp.Format("BAGG **INCREDIBLE**");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}

/// Log Land Dispute
void CvDiplomacyAI::LogLandDispute(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Don't fill out this field for Minors, as it just makes the log harder to read
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		switch(GetLandDisputeLevel(ePlayer))
		{
		case DISPUTE_LEVEL_FIERCE:
			strTemp.Format("LND **FIERCE**");
			break;
		case DISPUTE_LEVEL_STRONG:
			strTemp.Format("LND STRONG");
			break;
		case DISPUTE_LEVEL_WEAK:
			strTemp.Format("LND Weak");
			break;
		case DISPUTE_LEVEL_NONE:
			strTemp.Format("LND N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}

/// Log Victory Dispute
void CvDiplomacyAI::LogVictoryDispute(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Don't fill out this field for Minors, as it just makes the log harder to read
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		// Victory Dispute
		switch(GetVictoryDisputeLevel(ePlayer))
		{
		case DISPUTE_LEVEL_FIERCE:
			strTemp.Format("VCT **FIERCE**");
			break;
		case DISPUTE_LEVEL_STRONG:
			strTemp.Format("VCT STRONG");
			break;
		case DISPUTE_LEVEL_WEAK:
			strTemp.Format("VCT Weak");
			break;
		case DISPUTE_LEVEL_NONE:
			strTemp.Format("VCT N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}

/// Log Wonder Dispute
void CvDiplomacyAI::LogWonderDispute(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Don't fill out this field for Minors, as it just makes the log harder to read
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		// Victory Dispute
		switch(GetWonderDisputeLevel(ePlayer))
		{
		case DISPUTE_LEVEL_FIERCE:
			strTemp.Format("WND **FIERCE**");
			break;
		case DISPUTE_LEVEL_STRONG:
			strTemp.Format("WND STRONG");
			break;
		case DISPUTE_LEVEL_WEAK:
			strTemp.Format("WND Weak");
			break;
		case DISPUTE_LEVEL_NONE:
			strTemp.Format("WND N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}

/// Log Minor Civ Dispute
void CvDiplomacyAI::LogMinorCivDispute(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Don't fill out this field for Minors, as it just makes the log harder to read
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		strTemp.Format("");
	}
	else
	{
		// Victory Dispute
		switch(GetMinorCivDisputeLevel(ePlayer))
		{
		case DISPUTE_LEVEL_FIERCE:
			strTemp.Format("MCF **FIERCE**");
			break;
		case DISPUTE_LEVEL_STRONG:
			strTemp.Format("MCF STRONG");
			break;
		case DISPUTE_LEVEL_WEAK:
			strTemp.Format("MCF Weak");
			break;
		case DISPUTE_LEVEL_NONE:
			strTemp.Format("MCF N");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
	}

	strString += ", " + strTemp;
}

/// Log Proximity
void CvDiplomacyAI::LogProximity(CvString& strString, PlayerTypes ePlayer)
{
	CvString strTemp;

	// Proximity
	switch(GetPlayer()->GetProximityToPlayer(ePlayer))
	{
	case PLAYER_PROXIMITY_NEIGHBORS:
		strTemp.Format("PRX Neighbors");
		break;
	case PLAYER_PROXIMITY_CLOSE:
		strTemp.Format("PRX Close");
		break;
	case PLAYER_PROXIMITY_FAR:
		strTemp.Format("PRX Far");
		break;
	case PLAYER_PROXIMITY_DISTANT:
		strTemp.Format("PRX Distant");
		break;
	default:
		strTemp.Format("XXX");
		break;
	}
	strString += ", " + strTemp;
}

/// AI sent a message to someone... what is it?!?!? :o
void CvDiplomacyAI::LogStatementToPlayer(PlayerTypes ePlayer, DiploStatementTypes eMessage)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strLogName;

		CvString strOutBuf;
		CvString strBaseString;

		CvString playerName;
		CvString otherPlayerName;

		CvString strTemp;

		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Turn number
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());

		// Our Name
		strBaseString += playerName;

		// Their Name
		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strBaseString += ", " + otherPlayerName;

		strOutBuf = strBaseString;

		MajorCivApproachTypes eApproach = GetMajorCivApproach(ePlayer, /*bHideTrueFeelings*/ false);

		bool bMinorMessage = false;

		switch(eMessage)
		{
		case DIPLO_STATEMENT_REQUEST_PEACE:
			strTemp.Format("We request peace!!!");
			break;
		case DIPLO_STATEMENT_AGGRESSIVE_MILITARY_WARNING:
			strTemp.Format("Your Military is stationed aggressively!");
			break;
		case DIPLO_STATEMENT_KILLED_PROTECTED_CITY_STATE:
			strTemp.Format("You Killed a Protected City State!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_ATTACKED_PROTECTED_CITY_STATE:
			strTemp.Format("You Attacked a Protected City State!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_BULLIED_PROTECTED_CITY_STATE:
			strTemp.Format("You Bullied a Protected City State!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_EXPANSION_SERIOUS_WARNING:
			strTemp.Format("Expansion SERIOUS Warning!");
			break;
		case DIPLO_STATEMENT_EXPANSION_WARNING:
			strTemp.Format("Expansion Warning!");
			break;
		case DIPLO_STATEMENT_EXPANSION_BROKEN_PROMISE:
			strTemp.Format("Expansion Broken Promise!");
			break;
		case DIPLO_STATEMENT_PLOT_BUYING_SERIOUS_WARNING:
			strTemp.Format("Plot Buying SERIOUS Warning!");
			break;
		case DIPLO_STATEMENT_PLOT_BUYING_WARNING:
			strTemp.Format("Plot Buying Warning!");
			break;
		case DIPLO_STATEMENT_PLOT_BUYING_BROKEN_PROMISE:
			strTemp.Format("Plot Buying Broken Promise!");
			break;
		case DIPLO_STATEMENT_WE_ATTACKED_YOUR_MINOR:
			strTemp.Format("Haha! We attacked your Minor!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_WE_BULLIED_YOUR_MINOR:
			strTemp.Format("Haha! We bullied your Minor!");
			bMinorMessage = true;
			break;
		case DIPLO_STATEMENT_WORK_WITH_US:
			strTemp.Format("Wanna team up?");
			break;
		case DIPLO_STATEMENT_END_WORK_WITH_US:
			strTemp.Format("***** We're done working with you. *****");
			break;
		case DIPLO_STATEMENT_DENOUNCE:
			strTemp.Format("Denounce!");
			break;
		case DIPLO_STATEMENT_DENOUNCE_RANDFAILED:
			strTemp.Format("Denounce RANDFAILED");
			break;
		case DIPLO_STATEMENT_END_WORK_AGAINST_SOMEONE:
			strTemp.Format("***** We're done working against someone with you. *****");
			break;
		case DIPLO_STATEMENT_COOP_WAR_REQUEST:
			strTemp.Format("Wanna coop war against someone?");
			break;
		case DIPLO_STATEMENT_COOP_WAR_TIME:
			strTemp.Format("It's time to coop war against someone!");
			break;
		case DIPLO_STATEMENT_NOW_UNFORGIVABLE:
			strTemp.Format("***** You are Unforgivable! *****");
			break;
		case DIPLO_STATEMENT_NOW_ENEMY:
			strTemp.Format("***** You are an Enemy! *****");
			break;
		case DIPLO_STATEMENT_DEMAND:
			strTemp.Format("***** Give in to my demands or else! *****");
			break;
		case DIPLO_STATEMENT_REQUEST:
			strTemp.Format("***** Can you spare something for a friend? *****");
			break;
		case DIPLO_STATEMENT_REQUEST_RANDFAILED:
			strTemp.Format("***** Request RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_LUXURY_TRADE:
			strTemp.Format("***** You have a Luxury I would like *****");
			break;
		case DIPLO_STATEMENT_OPEN_BORDERS_EXCHANGE:
			strTemp.Format("Open Borders Exchange");
			break;
		case DIPLO_STATEMENT_OPEN_BORDERS_OFFER:
			strTemp.Format("Open Borders Offer");
			break;
		case DIPLO_STATEMENT_PLAN_RESEARCH_AGREEMENT:
			strTemp.Format("Research Plan");
			break;
		case DIPLO_STATEMENT_RESEARCH_AGREEMENT_OFFER:
			strTemp.Format("Research Agreement Offer");
			break;
		case DIPLO_STATEMENT_RENEW_DEAL:
			strTemp.Format("Renew Deal");
			break;
		case DIPLO_STATEMENT_INSULT:
			strTemp.Format("Insult");
			break;
		case DIPLO_STATEMENT_COMPLIMENT:
			if(eApproach == MAJOR_CIV_APPROACH_DECEPTIVE)
			{
				strTemp.Format("DECEPTIVE Compliment");
			}
			else
			{
				strTemp.Format("Compliment");
			}
			break;
		case DIPLO_STATEMENT_BOOT_KISSING:
			strTemp.Format("Boot Kissing");
			break;
		case DIPLO_STATEMENT_WARMONGER:
			strTemp.Format("Warmonger!");
			break;
		case DIPLO_STATEMENT_DENOUNCE_FRIEND:
			strTemp.Format("***** DENOUNCING A FRIEND *****");
			break;
		case DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE:
			strTemp.Format("***** DENOUNCE SOMEONE FOR ME? *****");
			break;
		case DIPLO_STATEMENT_REQUEST_FRIEND_DENOUNCE_RANDFAILED:
			strTemp.Format("***** Denounce someone for me? RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_REQUEST_FRIEND_WAR:
			strTemp.Format("***** DECLARE WAR ON SOMEONE FOR ME? *****");
			break;
		case DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY:
			strTemp.Format("***** YOU BEFRIENDED AN ENEMY OF MINE! *****");
			break;
		case DIPLO_STATEMENT_ANGRY_BEFRIEND_ENEMY_RANDFAILED:
			strTemp.Format("***** You befriended an enemy of mine! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND:
			strTemp.Format("***** YOU DENOUNCED A FRIEND OF MINE! *****");
			break;
		case DIPLO_STATEMENT_ANGRY_DENOUNCED_FRIEND_RANDFAILED:
			strTemp.Format("***** You denounced a friend of mine! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY:
			strTemp.Format("***** YAY - YOU DENOUNCED AN ENEMY! *****");
			break;
		case DIPLO_STATEMENT_HAPPY_DENOUNCED_ENEMY_RANDFAILED:
			strTemp.Format("***** Yay - you denounced an enemy! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND:
			strTemp.Format("***** YAY - YOU BEFRIENDED A FRIEND! *****");
			break;
		case DIPLO_STATEMENT_HAPPY_BEFRIENDED_FRIEND_RANDFAILED:
			strTemp.Format("***** Yay - you befriended a friend! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY:
			strTemp.Format("***** JUST FYI - I BEFRIENDED YOUR ENEMY! *****");
			break;
		case DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_ENEMY_RANDFAILED:
			strTemp.Format("***** Just FYI - I befriended your enemy! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND:
			strTemp.Format("***** JUST FYI - I DENOUNCED YOUR FRIEND! *****");
			break;
		case DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_FRIEND_RANDFAILED:
			strTemp.Format("***** Just FYI - I denounced your friend! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY:
			strTemp.Format("***** JUST FYI - I DENOUNCED YOUR ENEMY! *****");
			break;
		case DIPLO_STATEMENT_FYI_DENOUNCED_HUMAN_ENEMY_RANDFAILED:
			strTemp.Format("***** Just FYI - I denounced your enemy! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND:
			strTemp.Format("***** JUST FYI - I BEFRIENDED YOUR FRIEND! *****");
			break;
		case DIPLO_STATEMENT_FYI_BEFRIEND_HUMAN_FRIEND_RANDFAILED:
			strTemp.Format("***** Just FYI - I befriended your friend! RANDFAILED *****");
			break;
		case DIPLO_STATEMENT_SAME_POLICIES_AUTOCRACY:
		case DIPLO_STATEMENT_SAME_POLICIES_FREEDOM:
		case DIPLO_STATEMENT_SAME_POLICIES_ORDER:
			strTemp.Format("***** Yay! We like the same late game social policy tree *****");
			break;
		default:
			strTemp.Format("Unknown message!!!");
			break;
		}

		strOutBuf += ", " + strTemp;

		// Don't log these messages for now - they have their own function
		if(eMessage != DIPLO_STATEMENT_DENOUNCE &&
		        eMessage != DIPLO_STATEMENT_COOP_WAR_REQUEST)
			pLog->Msg(strOutBuf);

		// Also send message to Minor Civ log if applicable
		if(bMinorMessage)
		{
			// Open the log file
			if(GC.getPlayerAndCityAILogSplit())
				strLogName = "DiplomacyAI_MinorCiv_Log_" + playerName + ".csv";
			else
				strLogName = "DiplomacyAI_MinorCiv_Log.csv";

			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

//	-------------------------------------------------------------------------------------
void CvDiplomacyAI::TestUIDiploStatement(PlayerTypes eToPlayer, DiploStatementTypes eStatement, int iArg1)
{
	m_eTestToPlayer = eToPlayer;
	m_eTestStatement = eStatement;
	m_iTestStatementArg1 = iArg1;
}

//	-------------------------------------------------------------------------------------
void CvDiplomacyAI::LogOpenEmbassy(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ",***** OPENED EMBASSY @ " + otherPlayerName + "! *****";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

//	-------------------------------------------------------------------------------------
void CvDiplomacyAI::LogCloseEmbassy(PlayerTypes ePlayer)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString otherPlayerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "DiplomacyAI_Messages_Log_" + playerName + ".csv";
		}
		else
		{
			strLogName = "DiplomacyAI_Messages_Log.csv";
		}

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		otherPlayerName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		strOutBuf = strBaseString + ",***** CLOSED EMBASSY @ " + otherPlayerName + "! *****";

		pLog->Msg(strOutBuf);

		OutputDebugString("\n");
		OutputDebugString(strOutBuf);
		OutputDebugString("\n");
	}
}

//	-------------------------------------------------------------------------------------
//	Returns true if the target is valid to show a UI to immediately.
//	This will return true if the source and destination are both AI.
bool CvDiplomacyAI::IsValidUIDiplomacyTarget(PlayerTypes eTargetPlayer)
{
	if(eTargetPlayer != NO_PLAYER)
	{
		CvPlayer& kTarget = GET_PLAYER(eTargetPlayer);
		if(m_eTargetPlayer == NO_PLAYER || m_eTargetPlayer == eTargetPlayer || ((DiplomacyPlayerType)m_eTargetPlayer == DIPLO_AI_PLAYERS && !kTarget.isHuman()) || ((DiplomacyPlayerType)m_eTargetPlayer == DIPLO_HUMAN_PLAYERS && kTarget.isHuman()))
			return true;
	}

	return false;
}


FDataStream& operator<<(FDataStream& saveTo, const DiploLogData& readFrom)
{
	saveTo << readFrom.m_eDiploLogStatement;
	saveTo << readFrom.m_iTurn;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, DiploLogData& writeTo)
{
	loadFrom >> writeTo.m_eDiploLogStatement;
	loadFrom >> writeTo.m_iTurn;
	return loadFrom;
}

FDataStream& operator<<(FDataStream& saveTo, const DeclarationLogData& readFrom)
{
	saveTo << readFrom.m_eDeclaration;
	saveTo << readFrom.m_iData1;
	saveTo << readFrom.m_iData2;
	saveTo << readFrom.m_eMustHaveMetPlayer;
	saveTo << readFrom.m_bActive;
	saveTo << readFrom.m_iTurn;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, DeclarationLogData& writeTo)
{
	loadFrom >> writeTo.m_eDeclaration;
	loadFrom >> writeTo.m_iData1;
	loadFrom >> writeTo.m_iData2;
	loadFrom >> writeTo.m_eMustHaveMetPlayer;
	loadFrom >> writeTo.m_bActive;
	loadFrom >> writeTo.m_iTurn;
	return loadFrom;
}