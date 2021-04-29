/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreDLLUtil.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvDiplomacyAI.h"
#include "CvMilitaryAI.h"
#include "CvGrandStrategyAI.h"
#include "CvTechAI.h"
#include "CvInternalGameCoreUtils.h"
#include "CvInfosSerializationHelper.h"
#include "cvStopWatch.h"
#include "CvTypes.h"

#include "LintFree.h"

// consts to put in enums
const int iSpyTurnsToTravel = 1;
#if defined(MOD_BALANCE_CORE_SPIES)
const int iSpyTurnsToRevive = /*10*/ GC.getBALANCE_SPY_RESPAWN_TIMER();
#else
const int iSpyTurnsToRevive = 5;
#endif
const int iSpyTurnsToMakeIntroductions = 5;
const int iIntrigueTurnsValid = 5;
const int iSpyRankPower = 50;
PlayerTypes g_eSortPlayer = NO_PLAYER; // global - used for the sort

//=====================================
// CvEspionageSpy
//=====================================
/// Default Constructor
CvEspionageSpy::CvEspionageSpy()
	: m_iName(-1)
	, m_sName(NULL)
	, m_eRank(NUM_SPY_RANKS)
	, m_iCityX(-1)
	, m_iCityY(-1)
	, m_eSpyState(NUM_SPY_STATES)
	, m_iReviveCounter(0)
	, m_iPotentialAtStart(0)
	, m_eSpyFocus(NO_EVENT_CHOICE_CITY)
	, m_bIsDiplomat(false)
	, m_bEvaluateReassignment(true)
#if defined(MOD_API_ESPIONAGE)
	, m_bPassive(false)
#endif
{
}

const char* CvEspionageSpy::GetSpyName(CvPlayer* pPlayer)
{
	if (m_sName == NULL) {
		if (m_iName != -1) {
			m_sName = pPlayer->getCivilizationInfo().getSpyNames(m_iName);
		} else {
			return "TXT_KEY_SPY_NAME_UNKNOWN";
		}
	}
	
	return m_sName.c_str();
}

int CvEspionageSpy::GetSpyRank(PlayerTypes eSpyOwner) const
{
	return m_eRank + GET_PLAYER(eSpyOwner).GetPlayerTraits()->GetSpyExtraRankBonus();
}
#if defined(MOD_API_ESPIONAGE)
void CvEspionageSpy::SetSpyState(PlayerTypes eSpyOwner, int iSpyIndex, CvSpyState eSpyState)
{
	m_eSpyState = eSpyState;
#if defined(MOD_EVENTS_ESPIONAGE)
	if (MOD_EVENTS_ESPIONAGE) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_EspionageState, (int) eSpyOwner, iSpyIndex, (int) eSpyState, m_iCityX, m_iCityY);
	}
#endif
}
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
#endif

void CvEspionageSpy::SetSpyFocus(CityEventChoiceTypes eEventChoice)
{
	m_eSpyFocus = eEventChoice;
}
/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvEspionageSpy& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_iName;
	MOD_SERIALIZE_READ(53, loadFrom, writeTo.m_sName, NULL);
	int iSpyRank;
	loadFrom >> iSpyRank;
	writeTo.m_eRank = (CvSpyRank)iSpyRank;
	loadFrom >> writeTo.m_iCityX;
	loadFrom >> writeTo.m_iCityY;

	int iSpyState;
	loadFrom >> iSpyState;
	writeTo.m_eSpyState = (CvSpyState)iSpyState;

	int iSpyFocus;
	loadFrom >> iSpyFocus;
	writeTo.m_eSpyFocus = (CityEventChoiceTypes)iSpyFocus;

	loadFrom >> writeTo.m_iReviveCounter;
	loadFrom >> writeTo.m_iPotentialAtStart;

	if (uiVersion >= 1)
	{
		loadFrom >> writeTo.m_bIsDiplomat;
	}
	else
	{
		writeTo.m_bIsDiplomat = false;
	}
	
	loadFrom >> writeTo.m_bEvaluateReassignment;
	writeTo.m_bEvaluateReassignment = true;

#if defined(MOD_API_ESPIONAGE)
	MOD_SERIALIZE_READ(23, loadFrom, writeTo.m_bPassive, false);
#endif
	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvEspionageSpy& readFrom)
{
	uint uiVersion = 1;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_iName;
	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_sName);
	saveTo << (int)readFrom.m_eRank;
	saveTo << readFrom.m_iCityX;
	saveTo << readFrom.m_iCityY;
	saveTo << (int)readFrom.m_eSpyState;
	saveTo << (int)readFrom.m_eSpyFocus;
	saveTo << readFrom.m_iReviveCounter;
	saveTo << readFrom.m_iPotentialAtStart;
	saveTo << readFrom.m_bIsDiplomat;
	saveTo << readFrom.m_bEvaluateReassignment;

#if defined(MOD_API_ESPIONAGE)
	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_bPassive);
#endif

	return saveTo;
}

//=====================================
// CvPlayerEspionage
//=====================================
/// Default Constructor
CvPlayerEspionage::CvPlayerEspionage()
{
	Reset();
}

/// Destructor
CvPlayerEspionage::~CvPlayerEspionage(void)
{
	Reset();
}

/// Init
void CvPlayerEspionage::Init(CvPlayer* pPlayer)
{
	Reset();
	m_pPlayer = pPlayer;

	// catching empty players, minor civs, and barbarians
	if(m_pPlayer->getCivilizationType() == NO_CIVILIZATION || !m_pPlayer->getCivilizationInfo().isPlayable())
	{
		return;
	}

	// create m_aiSpyListNameOrder list
	for(int i = 0; i < m_pPlayer->getCivilizationInfo().getNumSpyNames(); i++)
	{
		m_aiSpyListNameOrder.push_back(i);
	}

#if defined(MOD_CORE_REDUCE_RANDOMNESS)
	//do nothing
#else
	for(uint ui = 0; ui < m_aiSpyListNameOrder.size(); ui++)
	{
		uint uiTempValue;
		uint uiTargetSlot = GC.getGame().getJonRandNum(m_aiSpyListNameOrder.size(), "Randomizing m_aiSpyListNameOrder list");
		uiTempValue = m_aiSpyListNameOrder[ui];
		m_aiSpyListNameOrder[ui] = m_aiSpyListNameOrder[uiTargetSlot];
		m_aiSpyListNameOrder[uiTargetSlot] = uiTempValue;
	}
#endif

	m_iSpyListNameOrderIndex = 0;

	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		TechList aTechList;
		m_aaPlayerStealableTechList.push_back(aTechList);
	}
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiNumTechsToStealList.push_back(0);
	}
#if defined(MOD_BALANCE_CORE)
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiNumSpyActionsDone.push_back(0);
	}
#endif
}

/// Uninit
void CvPlayerEspionage::Uninit()
{
	Reset();
}

void CvPlayerEspionage::Reset()
{
	m_pPlayer = NULL;
	m_aSpyList.clear();
	m_aiSpyListNameOrder.clear();
	m_iSpyListNameOrderIndex = -1;
	m_aSpyNotificationMessages.clear();

	m_aiNumTechsToStealList.clear();
	m_aIntrigueNotificationMessages.clear();
	m_aaPlayerStealableTechList.clear();

	m_aiNumSpyActionsDone = NumSpyActionsDone(MAX_MAJOR_CIVS, 0);
}


/// DoTurn
void CvPlayerEspionage::DoTurn()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayerEspionage::DoTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );
	ProcessSpyMessages();

	for(uint uiSpy = 0; uiSpy < m_aSpyList.size(); uiSpy++)
	{
		ProcessSpy(uiSpy);
	}
}

/// AddSpy - Grants the player a spy to use
void CvPlayerEspionage::CreateSpy()
{
	// don't create spies if espionage is disabled
	if(GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		return;
	}

	if (m_pPlayer->GetEspionageAI()->m_iTurnEspionageStarted == -1)
	{
		m_pPlayer->GetEspionageAI()->m_iTurnEspionageStarted = GC.getGame().getGameTurn();
	}

	CvEspionageSpy kNewSpy;
	kNewSpy.m_eRank = (CvSpyRank)m_pPlayer->GetStartingSpyRank();
	kNewSpy.m_eSpyState = SPY_STATE_UNASSIGNED;
	kNewSpy.m_eSpyFocus = NO_EVENT_CHOICE_CITY;
	GetNextSpyName(&kNewSpy);
	kNewSpy.m_bEvaluateReassignment = true;

#if defined(MOD_API_ESPIONAGE)
	kNewSpy.m_bPassive = false;
#endif

	m_aSpyList.push_back(kNewSpy);

	CvNotifications* pNotifications = m_pPlayer->GetNotifications();
	if(pNotifications)
	{
		const char* szSpyName = kNewSpy.GetSpyName(m_pPlayer);
		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_CREATED", szSpyName);
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_SPY_CREATED", szSpyName);
		pNotifications->Add(NOTIFICATION_SPY_CREATED_ACTIVE_PLAYER, strBuffer, strSummary, -1, -1, 0);
	}

	if(GC.getLogging())
	{
		CvString strMsg;
		strMsg.Format("New Spy, %d,", m_aSpyList.size() - 1);
		strMsg += GetLocalizedText(kNewSpy.GetSpyName(m_pPlayer));
		LogEspionageMsg(strMsg);
	}
}

/// Process a spy and run their missions
void CvPlayerEspionage::ProcessSpy(uint uiSpyIndex)
{
	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	bool bLastQualified;
	CvPlot* pCityPlot = GC.getMap().plot(pSpy->m_iCityX, pSpy->m_iCityY);
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;
	PlayerTypes ePlayer = m_pPlayer->GetID();
	PlayerTypes eCityOwner = NO_PLAYER;
	if(pCityPlot)
	{
		pCity = pCityPlot->getPlotCity();
		if(pCity)
		{
			eCityOwner = pCity->getOwner();
			pCityEspionage = pCity->GetCityEspionage();
		}
	}

	bool bHadSurveillance = HasEstablishedSurveillance(uiSpyIndex);
	switch(pSpy->m_eSpyState)
	{
	case SPY_STATE_UNASSIGNED:
		// spy is idling
		return;
		break;
	case SPY_STATE_TRAVELLING:
		// spy is traveling to location
		CvAssertMsg(pCity, "pCity not defined. What?");
		if(!pCity)
		{
			return;
		}
		// if the spy has completed travelling, move to location
		pCityEspionage->Process(ePlayer);
		if(pCityEspionage->HasReachedGoal(ePlayer))
		{
			pCityEspionage->ResetProgress(ePlayer);
			if (pSpy->m_bIsDiplomat)
			{
#if defined(MOD_API_ESPIONAGE)
				pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_MAKING_INTRODUCTIONS);
#else
				pSpy->m_eSpyState = SPY_STATE_MAKING_INTRODUCTIONS;
#endif
				int iRate = CalcPerTurn(SPY_STATE_MAKING_INTRODUCTIONS, pCity, uiSpyIndex);
				int iGoal = CalcRequired(SPY_STATE_MAKING_INTRODUCTIONS, pCity, uiSpyIndex);
				pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);				
			}
			else if(pCity->getTeam() == m_pPlayer->getTeam())  // moved to a friendly city
			{
				// moving to a defensive location
#if defined(MOD_API_ESPIONAGE)
				pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_COUNTER_INTEL);
#else
				pSpy->m_eSpyState = SPY_STATE_COUNTER_INTEL;
#endif
				if (pSpy->m_eSpyFocus == NO_EVENT_CHOICE_CITY)
					TriggerSpyFocusSetup(pCity, uiSpyIndex);
			}
			else // moved to an opponent's city
			{
#if defined(MOD_API_ESPIONAGE)
				pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_SURVEILLANCE);
#else
				pSpy->m_eSpyState = SPY_STATE_SURVEILLANCE;
#endif
				int iRate = CalcPerTurn(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex, false, true);
				int iGoal = CalcRequired(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex, false);
				pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);
			}
		}
		break;
	case SPY_STATE_SURVEILLANCE:
		CvAssertMsg(pCity, "pCity not defined. What?");
		if(!pCity)
		{
			return;
		}

		bLastQualified = pCityEspionage->HasReachedGoal(ePlayer);
		pCityEspionage->Process(ePlayer);
		if(pCityEspionage->HasReachedGoal(ePlayer))
		{
			if(GET_PLAYER(eCityOwner).isMinorCiv())
			{
#if defined(MOD_API_ESPIONAGE)
				if (!pSpy->m_bPassive) {
#endif
#if defined(MOD_API_ESPIONAGE)
				pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_RIG_ELECTION);
#else
				pSpy->m_eSpyState = SPY_STATE_RIG_ELECTION;
#endif
				pCityEspionage->ResetProgress(ePlayer);
				int iRate = CalcPerTurn(SPY_STATE_RIG_ELECTION, pCity, uiSpyIndex);
				int iGoal = CalcRequired(SPY_STATE_RIG_ELECTION, pCity, uiSpyIndex);
				pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);
				pCityEspionage->SetLastProgress(ePlayer, iRate);
#if defined(MOD_API_ESPIONAGE)
				}
#endif
			}
			else
			{
				if (MOD_BALANCE_CORE_SPIES_ADVANCED)
				{
					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Here for Spy Event!, %d,", uiSpyIndex);
						strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
						LogEspionageMsg(strMsg);
					}

					TriggerSpyFocusSetup(pCity, uiSpyIndex);
					return;
				}
#if defined(MOD_API_ESPIONAGE)
				if (!pSpy->m_bPassive) {
#endif
				BuildStealableTechList(eCityOwner);
				// moved rate out here to set the potential
				int iBasePotentialRate = CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, -1);
				pCityEspionage->SetLastBasePotential(ePlayer, iBasePotentialRate);
				if(m_aaPlayerStealableTechList[eCityOwner].size() > 0)
				{
					// TODO: need to proclaim surveillance somehow
#if defined(MOD_API_ESPIONAGE)
					pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_GATHERING_INTEL);
#else
					pSpy->m_eSpyState = SPY_STATE_GATHERING_INTEL;
#endif
					pCityEspionage->ResetProgress(ePlayer);
					int iPotentialRate = CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex);
					int iGoal = CalcRequired(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex);
					pCityEspionage->SetActivity(ePlayer, 0, iPotentialRate, iGoal);
					pCityEspionage->SetLastProgress(ePlayer, iPotentialRate);
					pCityEspionage->SetLastPotential(ePlayer, iPotentialRate);
				}
				else
				{
					// if this is the first time they crossed the threshold
					if(!bLastQualified)
					{
						CvNotifications* pNotifications = m_pPlayer->GetNotifications();
						if(pNotifications)
						{
							Localization::String strSummary;
							Localization::String strNotification;
							
							strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH_S");
							strSummary << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
							strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH");
							strNotification << GetSpyRankName(pSpy->m_eRank);

							strNotification << pSpy->GetSpyName(m_pPlayer);
							strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
							pNotifications->Add(NOTIFICATION_SPY_CANT_STEAL_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
						}
					}
					int iRate = CalcPerTurn(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
					pCityEspionage->SetLastProgress(ePlayer, iRate);
					pCityEspionage->SetLastPotential(ePlayer, -1); // set the last potential back to zero so that
					pSpy->m_bEvaluateReassignment = true; // flag to re-evaluate because we can't steal
					if(GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Re-eval: can't steal research, %d,", uiSpyIndex);
						strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
						LogEspionageMsg(strMsg);
					}

				}
#if defined(MOD_API_ESPIONAGE)
				}
#endif
				UncoverIntrigue(uiSpyIndex);
			}
		}
		break;
	case SPY_STATE_GATHERING_INTEL:
		CvAssertMsg(pCity, "pCity not defined. What?");
		if(!pCity)
		{
			return;
		}
		if (MOD_BALANCE_CORE_SPIES_ADVANCED)
		{
			pCityEspionage->Process(ePlayer);
			if (GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Spy Countdown for %d. Turns to completion: %d,", uiSpyIndex, GetTurnsUntilStateComplete(uiSpyIndex));
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
				LogEspionageMsg(strMsg);
			}
			UncoverIntrigue(uiSpyIndex);
			return;
		}
		BuildStealableTechList(eCityOwner);
		pCityEspionage->Process(ePlayer);
		// if the rate is too low, reassign the spy
		if (pCityEspionage->m_aiRate[m_pPlayer->GetID()] < 100)
		{
			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: potential too low, %d,", uiSpyIndex);
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
				LogEspionageMsg(strMsg);
			}
		}
#if defined(MOD_BALANCE_CORE_SPIES)
		if(MOD_BALANCE_CORE_SPIES && (m_aaPlayerStealableTechList[eCityOwner].size() == 0) && (pSpy->m_eSpyState != SPY_STATE_DEAD))
#else
		if(m_aaPlayerStealableTechList[eCityOwner].size() == 0)
#endif
		{
			// set the spy back to surveillance mode
			pCityEspionage->ResetProgress(ePlayer);
#if defined(MOD_API_ESPIONAGE)
			pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_SURVEILLANCE);
#else
			pSpy->m_eSpyState = SPY_STATE_SURVEILLANCE;
#endif
			pSpy->m_bEvaluateReassignment = true; // flag for reassignment
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: m_aaPlayerStealableTechList[eCityOwner].size() == 0, %d,", uiSpyIndex);
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
				LogEspionageMsg(strMsg);
			}

			int iRate = CalcPerTurn(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
			int iGoal = CalcRequired(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
			int iAmount = iGoal; // put the player back at surveillance levels
			pCityEspionage->SetActivity(ePlayer, iAmount, iRate, iGoal);

			// set last progress rate
			iRate = CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex);

			// can't steal any techs from this civ
			m_aiNumTechsToStealList[eCityOwner] = 0;

			CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			if(pNotifications)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH_S");
				strSummary << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
				Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH");
				strNotification << GetSpyRankName(pSpy->m_eRank);
				strNotification << pSpy->GetSpyName(m_pPlayer);
				strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
				pNotifications->Add(NOTIFICATION_SPY_CANT_STEAL_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
		}
		else if(pCityEspionage->HasReachedGoal(ePlayer))
		{
			CvSpyResult eResult = GetSpyRollResult(pCity);
			pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, eResult);
			if(pCityEspionage->HasCounterSpy())
			{			
				if (eResult == SPY_RESULT_KILLED)
				{
#if defined(MOD_EVENTS_ESPIONAGE)
					pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_KILLED);
#else
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_KILLED);
#endif

#if !defined(NO_ACHIEVEMENTS)
					CvPlayerAI& kCityOwner = GET_PLAYER(eCityOwner);
					CvPlayerAI& kSpyOwner = GET_PLAYER(ePlayer);

					if(	strcmp(kCityOwner.getCivilizationTypeKey(), "CIVILIZATION_RUSSIA") == 0 &&
						strcmp(kSpyOwner.getCivilizationTypeKey(), "CIVILIZATION_ENGLAND") == 0)
					{
						//Do not trigger achievement if we're in the Smokey Skies scenario.
						bool bUsingXP1Scenario3 = gDLL->IsModActivated(CIV5_XP1_SCENARIO3_MODID);
						if(!bUsingXP1Scenario3)
						{
							if(eCityOwner == GC.getGame().getActivePlayer())
								gDLL->UnlockAchievement(ACHIEVEMENT_XP1_25);
						}
					}
#endif
				}
			}

			CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(eCityOwner).GetEspionage();
			CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
			if(pDefendingPlayerEspionage)
			{
				pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH);
			}

			CvDiplomacyAI* pDefendingPlayerDiploAI = GET_PLAYER(eCityOwner).GetDiplomacyAI();
			CvAssertMsg(pDefendingPlayerDiploAI, "Defending player diplo AI is null");
			if(pDefendingPlayerDiploAI)
			{
				if(pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_IDENTIFIED || pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_KILLED)
				{
					pDefendingPlayerDiploAI->ChangeNumTimesRobbedBy(ePlayer, 1);
				}
			}

			// spy killed in action
			if(pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_KILLED)
			{
				CvNotifications* pNotifications = m_pPlayer->GetNotifications();
				if(pNotifications)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED_S");
					strSummary << pSpy->GetSpyName(m_pPlayer);
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED");
					strNotification << GetSpyRankName(pSpy->m_eRank);
					strNotification << pSpy->GetSpyName(m_pPlayer);
					strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
					strNotification << pCity->getNameKey();
					pNotifications->Add(NOTIFICATION_SPY_WAS_KILLED, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);

				}

				CvEspionageAI* pEspionageAI = m_pPlayer->GetEspionageAI();
				CvAssertMsg(pEspionageAI, "pEspionageAI is null");
				if(pEspionageAI)
				{
					pEspionageAI->m_aiTurnLastSpyDied[eCityOwner] = GC.getGame().getGameTurn();
					pEspionageAI->m_aiNumSpiesDied[eCityOwner]++;
				}
				CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(eCityOwner).GetEspionageAI();
				CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
				if(pDefenderEspionageAI)
				{
					pDefenderEspionageAI->m_aiTurnLastSpyKilled[m_pPlayer->GetID()] = GC.getGame().getGameTurn();
					pDefenderEspionageAI->m_aiNumSpiesKilled[m_pPlayer->GetID()]++;

					// You broke the promise you made!
					if (GET_PLAYER(eCityOwner).GetDiplomacyAI()->IsPlayerMadeSpyPromise(m_pPlayer->GetID()))
					{
						GET_PLAYER(eCityOwner).GetDiplomacyAI()->SetPlayerSpyPromiseState(m_pPlayer->GetID(), PROMISE_STATE_BROKEN);
					}
				}

				// level up the defending spy
				int iDefendingSpy = pCityEspionage->m_aiSpyAssignment[eCityOwner];
				if(pDefendingPlayerEspionage)
				{
					CvAssertMsg(iDefendingSpy >= 0, "No defending spy. This is ok if debugging and killing a spy without having a defending spy present, but should not occur when playing the game normally.");
					if(iDefendingSpy >= 0)
					{
						pDefendingPlayerEspionage->LevelUpSpy(iDefendingSpy);
					}
				}

				GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, (pSpy->GetSpyRank(eCityOwner) + 1), true, NO_PLAYER, NULL, false, pCity);

				// kill spy off
				ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
#if defined(MOD_API_ESPIONAGE)
				pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_DEAD); // have to official kill him after the extraction
#else
				pSpy->m_eSpyState = SPY_STATE_DEAD; // have to official kill him after the extraction
#endif

				if(GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Killed, %d,", uiSpyIndex);
					strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
					strMsg += ",";
					strMsg += ",";
					strMsg += ",";
					strMsg += GET_PLAYER(eCityOwner).getCivilizationShortDescription();
					strMsg += ",";
					strMsg += pCity->getName();
					strMsg += ",";
					strMsg += "Killed";
					LogEspionageMsg(strMsg);
				}
			}
			else // spy successfully completed mission
			{
				pSpy->m_bEvaluateReassignment = true; // flag spy for reassignment
				if(GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Re-eval: spy completed mission, %d,", uiSpyIndex);
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
					LogEspionageMsg(strMsg);
				}
				int iCityOwner = (int)eCityOwner;
				m_aiNumTechsToStealList[iCityOwner] = m_aiNumTechsToStealList[iCityOwner] + 1;
				pCityEspionage->ResetProgress(ePlayer);
				int iRate = CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex);
				int iGoal = CalcRequired(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex);
				pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);
				pCityEspionage->SetLastProgress(ePlayer, iRate);
				pCityEspionage->m_aiNumTimesCityRobbed[eCityOwner]++;

				if(pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_IDENTIFIED)
				{
					CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(eCityOwner).GetEspionageAI();
					CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
					if(pDefenderEspionageAI)
					{
						pDefenderEspionageAI->m_aiTurnLastSpyCaught[m_pPlayer->GetID()] = GC.getGame().getGameTurn();
						pDefenderEspionageAI->m_aiNumSpiesCaught[m_pPlayer->GetID()]++;

						// You broke the promise you made!
						if (GET_PLAYER(eCityOwner).GetDiplomacyAI()->IsPlayerMadeSpyPromise(m_pPlayer->GetID()))
						{
							GET_PLAYER(eCityOwner).GetDiplomacyAI()->SetPlayerSpyPromiseState(m_pPlayer->GetID(), PROMISE_STATE_BROKEN);
						}
					}
				}

				// this check was added because m_aiNumTechsToStealList was getting out of whack somehow and this is a check to prevent the UI from going haywire
				CvAssertMsg(m_aiNumTechsToStealList[iCityOwner] > 0, "m_aiNumTechsToStealList[iCityOwner] <= 0, which shouldn't happen after you succeed at stealing");
				if (m_aiNumTechsToStealList[iCityOwner] > 0)
				{
					CvNotifications* pNotifications = m_pPlayer->GetNotifications();
					if(pNotifications)
					{
						Localization::String strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_STEAL_TECH_S");
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STEAL_TECH");
						strNotification << GetSpyRankName(pSpy->m_eRank);
						strNotification << pSpy->GetSpyName(m_pPlayer);
						strNotification << pCity->getNameKey();
						strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_SPY_STOLE_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, eCityOwner);
					}
#if defined(MOD_BALANCE_CORE)
					m_aiNumSpyActionsDone[eCityOwner]++;
#endif
					m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_SPY_ATTACK, false, NO_GREATPERSON, NO_BUILDING, 1);
				}
				else
				{
					m_aiNumTechsToStealList[iCityOwner] = 0;
				}

#if !defined(NO_ACHIEVEMENTS)
				//Achievements!
				if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_12);
				}
#endif

				LevelUpSpy(uiSpyIndex);

				if(GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Stealing tech, %d,", uiSpyIndex);
					strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
					strMsg += ",";
					strMsg += ",";
					strMsg += ",";
					strMsg += GET_PLAYER(eCityOwner).getCivilizationShortDescription();
					strMsg += ",";
					strMsg += pCity->getName();
					strMsg += ",";
					switch(pCityEspionage->m_aiResult[ePlayer])
					{
					case SPY_RESULT_UNDETECTED:
						strMsg += "Undetected";
						break;
					case SPY_RESULT_DETECTED:
						strMsg += "Detected";
						break;
					case SPY_RESULT_IDENTIFIED:
						strMsg += "Identified";
						break;
					}
					LogEspionageMsg(strMsg);
				}
			}
#if defined(MOD_API_ESPIONAGE)
			if(pSpy->m_eSpyState != SPY_STATE_DEAD && pSpy->m_eSpyState != SPY_STATE_TERMINATED)
#else
			if(pSpy->m_eSpyState != SPY_STATE_DEAD)
#endif
			{
				UncoverIntrigue(uiSpyIndex);
			}
		}
		break;
	case SPY_STATE_RIG_ELECTION:
		// resetting the value is done in MinorCivAI
		pCityEspionage->Process(ePlayer);
		break;
	case SPY_STATE_COUNTER_INTEL:
	{
		if (pSpy->m_eSpyFocus == NO_EVENT_CHOICE_CITY)
			TriggerSpyFocusSetup(pCity, uiSpyIndex);
		break;
	}
	case SPY_STATE_MAKING_INTRODUCTIONS:
		// spy is traveling to location
		CvAssertMsg(pCity, "pCity not defined. What?");
		if(!pCity)
		{
			return;
		}
		// if the spy has completed travelling, move to location
		pCityEspionage->Process(ePlayer);
		if(pCityEspionage->HasReachedGoal(ePlayer))
		{
			pCityEspionage->ResetProgress(ePlayer);
#if defined(MOD_API_ESPIONAGE)
			pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_SCHMOOZE);
#else
			pSpy->m_eSpyState = SPY_STATE_SCHMOOZE;
#endif
		}
		break;
	case SPY_STATE_SCHMOOZE:
		if(pSpy->m_eSpyState != SPY_STATE_DEAD)
		{
			UncoverIntrigue(uiSpyIndex);
		}
		break;
	case SPY_STATE_DEAD:
		pSpy->m_iReviveCounter++;
		if(pSpy->m_iReviveCounter >= iSpyTurnsToRevive)
		{
			GetNextSpyName(pSpy);
			pSpy->m_eRank = (CvSpyRank)m_pPlayer->GetStartingSpyRank();
#if defined(MOD_API_ESPIONAGE)
			pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_UNASSIGNED);
#else
			pSpy->m_eSpyState = SPY_STATE_UNASSIGNED;
#endif
			pSpy->m_iCityX = -1;
			pSpy->m_iCityY = -1;
			pSpy->m_iReviveCounter = 0;
			pSpy->m_bEvaluateReassignment = true;
#if defined(MOD_API_ESPIONAGE)
			pSpy->m_bPassive = false;
#endif

			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: spy killed, %d,", uiSpyIndex);
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
				LogEspionageMsg(strMsg);
			}

			CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			if(pNotifications)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_REVIVED_S");
				Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_REVIVED");
				strNotification << GetSpyRankName(pSpy->m_eRank);
				strNotification << pSpy->GetSpyName(m_pPlayer);
				pNotifications->Add(NOTIFICATION_SPY_REPLACEMENT, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}

			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Respawned spy, %d,", uiSpyIndex);
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
				LogEspionageMsg(strMsg);
			}
		}
		break;
	}

	// if we just established surveillance in the city, turn the lights on
	if(HasEstablishedSurveillance(uiSpyIndex) && !bHadSurveillance)
	{
#if defined(MOD_API_EXTENSIONS)
		pCity->plot()->changeAdjacentSight(m_pPlayer->getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), true, NO_INVISIBLE, NO_DIRECTION);
#else
		pCity->plot()->changeAdjacentSight(m_pPlayer->getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), true, NO_INVISIBLE, NO_DIRECTION, false);
#endif
	}
}
#if defined(MOD_BALANCE_CORE_SPIES)
void CvPlayerEspionage::ProcessSpyFocus()
{
	for (uint uiSpy = 0; uiSpy < m_aSpyList.size(); uiSpy++)
	{
		CvEspionageSpy* pSpy = &(m_aSpyList[uiSpy]);
		if ((int)pSpy->m_eSpyState != SPY_STATE_GATHERING_INTEL)
			continue;

		PlayerTypes eCityOwner = NO_PLAYER;
		//Let's give spies a few new tricks, eh?
		CvPlot* pCityPlot = GC.getMap().plot(pSpy->m_iCityX, pSpy->m_iCityY);
		CvCity* pCity = NULL;
		if (pCityPlot)
		{
			pCity = pCityPlot->getPlotCity();
			if (pCity)
			{
				eCityOwner = pCity->getOwner();
				if (!GET_PLAYER(eCityOwner).isMinorCiv())
				{
					if (pCity->GetCityEspionage()->HasReachedGoal(m_pPlayer->GetID()))
						DoSpyFocusEvent(uiSpy);
				}
			}
		}	
	}
}
/// Advanced Action - Determine if the spy can do an advanced action not related to techs or intrigue.
bool CvPlayerEspionage::DoSpyFocusEvent(uint uiSpyIndex)
{
	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	
	CvPlot* pCityPlot = GC.getMap().plot(pSpy->m_iCityX, pSpy->m_iCityY);
	CvCity* pCity = NULL;
	PlayerTypes eCityOwner = NO_PLAYER;

	//Let's give spies a few new tricks, eh?
	if(pCityPlot)
	{
		pCity = pCityPlot->getPlotCity();
		if(pCity)
		{
			eCityOwner = pCity->getOwner();
		}
	}
	if(eCityOwner != NO_PLAYER)
	{
		CityEventChoiceTypes eEventChoice = pSpy->m_eSpyFocus;
		if (eEventChoice != NO_EVENT_CHOICE_CITY)
		{
			pCity->DoEventChoice(eEventChoice, NO_EVENT_CITY, true, uiSpyIndex, m_pPlayer->GetID());
			if (m_pPlayer->isHuman())
			{
				CvPopupInfo kPopupInfo(BUTTONPOPUP_MODDER_7, eEventChoice, pCity->GetID(), pCity->getOwner());
				GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
			}
		}
	}
	return false;
}

CvWeightedVector<int> CvPlayerEspionage::GetRandomActionEventPool(CvCity* pCity)
{
	CvWeightedVector<int> m_eEventPool;

	if (pCity == NULL)
		return m_eEventPool;


	for (int iLoop = 0; iLoop < GC.getNumCityEventInfos(); iLoop++)
	{
		CityEventTypes eEvent = (CityEventTypes)iLoop;
		if (eEvent != NO_EVENT)
		{
			CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
			if (pkEventInfo == NULL || !pkEventInfo->isEspionage())
				continue;

			if (!pCity->IsCityEventValid(eEvent, true))
				continue;

			int iWeight = pkEventInfo->getRandomChance() + GC.getGame().getSmallFakeRandNum(pkEventInfo->getRandomChanceDelta(), m_pPlayer->GetTreasury()->GetLifetimeGrossGold());

			m_eEventPool.push_back(eEvent, iWeight);
		}
	}
	return m_eEventPool;
}
void CvPlayerEspionage::TriggerSpyFocusSetup(CvCity* pCity, int uiSpyIndex)
{
	bool bCounterSpy = false;
	if (pCity->getOwner() == m_pPlayer->GetID())
		bCounterSpy = true;
	CityEventTypes eSetupEvent = NO_EVENT_CITY;
	for (int iLoop = 0; iLoop < GC.getNumCityEventInfos(); iLoop++)
	{
		CityEventTypes eEvent = (CityEventTypes)iLoop;
		if (eEvent != NO_EVENT)
		{
			CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
			if (pkEventInfo == NULL)
				continue;

			if (!bCounterSpy && pkEventInfo->isEspionageSetup())
			{
				eSetupEvent = eEvent;
				break;
			}
			else if (bCounterSpy && pkEventInfo->IsCounterSpy())
			{
				eSetupEvent = eEvent;
				break;
			}
		}
	}

	if (eSetupEvent == NO_EVENT_CITY)
		return;

	if (!m_pPlayer->isHuman())
	{
		m_pPlayer->AI_DoEspionageEventChoice(eSetupEvent, uiSpyIndex, pCity);
		return;
	}

	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	CvNotifications* pNotifications = m_pPlayer->GetNotifications();
	if (pNotifications && pSpy)
	{
		Localization::String strBuffer = Localization::Lookup("TXT_KEY_CHOOSE_EVENT_AA_CHOICE");
		strBuffer << GetSpyRankName(pSpy->m_eRank);
		strBuffer << pSpy->GetSpyName(m_pPlayer);
		
		Localization::String strSummary = Localization::Lookup("TXT_KEY_CHOOSE_EVENT_AA_TT");
		strSummary << GetSpyRankName(pSpy->m_eRank);
		strSummary << pSpy->GetSpyName(m_pPlayer);
		strSummary << pCity->getNameKey();

		pNotifications->Add((NotificationTypes)FString::Hash("NOTIFICATION_ESPIONAGE_AA"), strSummary.toUTF8(), strBuffer.toUTF8(), pCity->plot()->getX(), pCity->plot()->getY(), eSetupEvent, uiSpyIndex);
		pCity->GetCityEspionage()->m_aiPendingEventsForPlayer[m_pPlayer->GetID()]++;
	}

	if (GC.getLogging())
	{
		CvString strMsg;
		strMsg.Format("Spy Setup Time for %d! Event: %d,", uiSpyIndex, (int)eSetupEvent);
		LogEspionageMsg(strMsg);
	}
}
void CvPlayerEspionage::CreateSpyChoiceEvent(CityEventTypes eEvent, CvCity* pCity, int uiSpyIndex)
{
	CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
	if (pkEventInfo == NULL)
		return;

	if (!m_pPlayer->isHuman())
	{
		m_pPlayer->AI_DoEspionageEventChoice(eEvent, uiSpyIndex, pCity);
		return;
	}

	CvNotifications* pNotifications = m_pPlayer->GetNotifications();
	if (pNotifications)
	{
		CvString strBuffer = GetLocalizedText("TXT_KEY_CHOOSE_EVENT_AA_CHOICE");
		CvString strSummary = GetLocalizedText("TXT_KEY_CHOOSE_EVENT_AA_TT");
		pNotifications->Add((NotificationTypes)FString::Hash("NOTIFICATION_ESPIONAGE_AA"), strSummary.c_str(), strBuffer.c_str(), pCity->plot()->getX(), pCity->plot()->getY(), eEvent, uiSpyIndex);
		pCity->GetCityEspionage()->m_aiPendingEventsForPlayer[m_pPlayer->GetID()]++;
	}

	if (GC.getLogging())
	{
		CvString strMsg;
		strMsg.Format("Spy Choice Time for %d! Event: %d,", uiSpyIndex, (int)eEvent);
		LogEspionageMsg(strMsg);
	}
}
CvSpyResult CvPlayerEspionage::ProcessSpyFocusResult(PlayerTypes ePlayer, CvCity* pCity, int uiSpyIndex, CityEventChoiceTypes eEventChoice, bool bDefer)
{
	pCity->GetCityEspionage()->m_aiPendingEventsForPlayer[m_pPlayer->GetID()]--;

	//no choice = no effect
	if (eEventChoice == NO_EVENT_CHOICE)
		return NUM_SPY_RESULTS;

	CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
	if (pkEventChoiceInfo == NULL)
		return NUM_SPY_RESULTS;

	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);

	PlayerTypes eCityOwner = pCity->getOwner();
	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();

	if (bDefer)
	{
		if (eCityOwner != m_pPlayer->GetID())
		{
			pSpy->m_iPotentialAtStart = pCity->GetEspionageRanking();
			pSpy->SetSpyFocus(eEventChoice);
			pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_GATHERING_INTEL);
			pCityEspionage->ResetProgress(ePlayer);
			int iPotentialRate = CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex);
			int iGoal = CalcRequired(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex);
			pCityEspionage->SetActivity(ePlayer, 0, iPotentialRate, iGoal);
			pCityEspionage->SetLastProgress(ePlayer, iPotentialRate);
			pCityEspionage->SetLastPotential(ePlayer, iPotentialRate);

			return NUM_SPY_RESULTS;
		}
		else
		{
			pSpy->m_iPotentialAtStart = pCity->GetEspionageRanking();
			pSpy->SetSpyFocus(eEventChoice);
			pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_COUNTER_INTEL);
			return NUM_SPY_RESULTS;
		}
	}

	if (eCityOwner == m_pPlayer->GetID())
		return NUM_SPY_RESULTS;

	//time to reset the spy
	pSpy->SetSpyFocus(NO_EVENT_CHOICE_CITY);
	pSpy->m_iPotentialAtStart = -1;

	CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(eCityOwner).GetEspionage();
	CvDiplomacyAI* pDefendingPlayerDiploAI = GET_PLAYER(eCityOwner).GetDiplomacyAI();
	
	m_aiNumSpyActionsDone[pCity->getOwner()]++;
	pCityEspionage->m_aiNumTimesCityRobbed[ePlayer]++;

	//reset ranking to 10.
	pCity->ResetEspionageRanking();

	CvSpyResult eResult = GetSpyRollResult(pCity, eEventChoice);
	pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, eResult);

	CvAssertMsg(pDefendingPlayerDiploAI, "Defending player diplo AI is null");
	if (pDefendingPlayerDiploAI)
	{
		if (pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_IDENTIFIED)
		{
			pDefendingPlayerDiploAI->ChangeNumTimesRobbedBy(ePlayer, 1);
		}
	}

	if (eResult == SPY_RESULT_IDENTIFIED)
	{
		CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(pCity->getOwner()).GetEspionageAI();
		CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
		if (pDefenderEspionageAI)
		{
			pDefenderEspionageAI->m_aiTurnLastSpyCaught[m_pPlayer->GetID()] = GC.getGame().getGameTurn();
			pDefenderEspionageAI->m_aiNumSpiesCaught[m_pPlayer->GetID()]++;
		}
	}

	// spy killed in action
	if (eResult == SPY_RESULT_KILLED)
	{
		CvEspionageAI* pEspionageAI = m_pPlayer->GetEspionageAI();
		CvAssertMsg(pEspionageAI, "pEspionageAI is null");
		if (pEspionageAI)
		{
			pEspionageAI->m_aiTurnLastSpyDied[eCityOwner] = GC.getGame().getGameTurn();
			pEspionageAI->m_aiNumSpiesDied[eCityOwner]++;
		}
		CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(eCityOwner).GetEspionageAI();
		CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
		if (pDefenderEspionageAI)
		{
			pDefenderEspionageAI->m_aiTurnLastSpyKilled[m_pPlayer->GetID()] = GC.getGame().getGameTurn();
			pDefenderEspionageAI->m_aiNumSpiesKilled[m_pPlayer->GetID()]++;
		}

		CvAssertMsg(pDefendingPlayerDiploAI, "Defending player diplo AI is null");
		if (pDefendingPlayerDiploAI)
		{
			if (pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_KILLED)
			{
				pDefendingPlayerDiploAI->ChangeNumTimesRobbedBy(ePlayer, 1);
			}
		}

		GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, (pSpy->GetSpyRank(eCityOwner) + 1), true, NO_PLAYER, NULL, false, pCity);

		// level up the defending spy
		int iDefendingSpy = pCityEspionage->m_aiSpyAssignment[eCityOwner];
		if (pDefendingPlayerEspionage)
		{
			pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH);
			CvAssertMsg(iDefendingSpy >= 0, "No defending spy. This is ok if debugging and killing a spy without having a defending spy present, but should not occur when playing the game normally.");
			if (iDefendingSpy >= 0)
			{
				pDefendingPlayerEspionage->LevelUpSpy(iDefendingSpy);
			}
		}
		// kill spy off
		ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
		pSpy->m_eSpyState = SPY_STATE_DEAD; // have to official kill him after the extraction
		if (GC.getLogging())
		{
			CvString strMsg;
			strMsg.Format("Advanced Action: Spy killed");
			strMsg += " , ";
			strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
			strMsg += " , ";
			if (pCity)
			{
				strMsg += GET_PLAYER(pCity->getOwner()).getCivilizationShortDescription();
				strMsg += " , ";
				strMsg += pCity->getName();
			}
			LogEspionageMsg(strMsg);
		}
	}
	else
	{
		DoSpyFocusLevelUp(uiSpyIndex);
		ExtractSpyFromCity(uiSpyIndex);
	}
	m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_SPY_ATTACK, false, NO_GREATPERSON, NO_BUILDING, 1);

	//Notifications
	CvNotifications* pOwnerNotifications = m_pPlayer->GetNotifications();
	
	if (eResult == SPY_RESULT_KILLED)
	{
		if (pOwnerNotifications)
		{
			Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED_S_ADVANCED");
			strSummary << pSpy->GetSpyName(m_pPlayer);
			strSummary << pCity->getNameKey();
			Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED_ADVANCED");
			strNotification << GetSpyRankName(pSpy->m_eRank);
			strNotification << pSpy->GetSpyName(m_pPlayer);
			strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
			strNotification << pCity->getNameKey();
			pOwnerNotifications->Add(NOTIFICATION_SPY_WAS_KILLED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
		}
	}

	//Let's do our notification stuff here.
	for (int iI = 0; iI < pkEventChoiceInfo->GetNumNotifications(); iI++)
	{
		CvString strNotificationString = pkEventChoiceInfo->GetNotificationInfo(iI)->GetNotificationString();
		if (strNotificationString != NULL && strNotificationString != "")
		{
			NotificationTypes eNotificationType = (NotificationTypes)FString::Hash(strNotificationString);

			if (eNotificationType != NO_NOTIFICATION_TYPE)
			{
				Localization::String strMessage;
				Localization::String strSummary;
				strSummary = Localization::Lookup(pkEventChoiceInfo->GetNotificationInfo(iI)->GetShortDescription());
				strSummary << GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
				strSummary << pCity->getNameKey();
				strSummary << GET_PLAYER(ePlayer).getCivilizationDescription();
				strSummary << GET_PLAYER(ePlayer).getName();
				strMessage = Localization::Lookup(pkEventChoiceInfo->GetNotificationInfo(iI)->GetDescription());
				strMessage << GET_PLAYER(ePlayer).getCivilizationShortDescriptionKey();
				strMessage << pCity->getNameKey();
				strMessage << GET_PLAYER(ePlayer).getCivilizationDescription();
				strMessage << GET_PLAYER(ePlayer).getName();
				strMessage << pCity->GetScaledHelpText(eEventChoice, true, uiSpyIndex, m_pPlayer->GetID());
				bool bGlobal = pkEventChoiceInfo->GetNotificationInfo(iI)->IsWorldEvent();
				bool bEspionage = pkEventChoiceInfo->GetNotificationInfo(iI)->IsEspionageEvent();
				int iX = -1;
				int iY = -1;
				int iVariable = pkEventChoiceInfo->GetNotificationInfo(iI)->GetVariable();
				if (pkEventChoiceInfo->GetNotificationInfo(iI)->IsNeedCityCoordinates())
				{
					iX = pCity->getX();
					iY = pCity->getY();
				}
				//special handling for espionage stuff!
				if (bEspionage)
				{
					CvNotifications* pTheirNotifications = GET_PLAYER(ePlayer).GetNotifications();
					if (pTheirNotifications)
					{
						if (pkEventChoiceInfo->GetNotificationInfo(iI)->IsNeedPlayerID())
						{
							pTheirNotifications->Add(eNotificationType, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, ePlayer, pCity->GetID());
						}
						else
						{
							pTheirNotifications->Add(eNotificationType, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, iVariable, pCity->GetID());
						}
					}
					CvNotifications* pOurNotifications = m_pPlayer->GetNotifications();
					if (pOurNotifications)
					{
						if (pkEventChoiceInfo->GetNotificationInfo(iI)->IsNeedPlayerID())
						{
							pOurNotifications->Add(eNotificationType, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, ePlayer, pCity->GetID());
						}
						else
						{
							pOurNotifications->Add(eNotificationType, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, iVariable, pCity->GetID());
						}
					}
				}

				for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					PlayerTypes eOtherPlayer = (PlayerTypes)iPlayerLoop;
					if (eOtherPlayer != NO_PLAYER && GET_PLAYER(eOtherPlayer).isMajorCiv())
					{
						//Not global? Skip all but me.
						if (!bGlobal && eOtherPlayer != ePlayer)
							continue;

						//Global? Seed only to known players.
						if (bGlobal && eOtherPlayer != ePlayer && !GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam()))
							continue;

						//Send out notifications!
						CvNotifications* pNotifications = GET_PLAYER(eOtherPlayer).GetNotifications();
						if (pNotifications && GC.getGame().getActivePlayer() == eOtherPlayer)
						{
							if (pkEventChoiceInfo->GetNotificationInfo(iI)->IsNeedPlayerID())
							{
								pNotifications->Add(eNotificationType, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, ePlayer, pCity->GetID());
							}
							else
							{
								pNotifications->Add(eNotificationType, strMessage.toUTF8(), strSummary.toUTF8(), iX, iY, iVariable, pCity->GetID());
							}
						}
					}
				}
			}
		}
	}

	return eResult;
}


void CvPlayerEspionage::DoSpyFocusLevelUp(uint uiSpyIndex)
{
	LevelUpSpy(uiSpyIndex);
}

// Helpers

CvString CvPlayerEspionage::GetEventHelpText(CityEventTypes eEvent, int uiSpyIndex)
{
	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	if (!pSpy)
		return "";

	CvPlot* pCityPlot = GC.getMap().plot(pSpy->m_iCityX, pSpy->m_iCityY);
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;
	PlayerTypes eCityOwner = NO_PLAYER;

	if (pCityPlot)
	{
		pCity = pCityPlot->getPlotCity();
		eCityOwner = pCity->getOwner();
		pCityEspionage = pCity->GetCityEspionage();
	}
	CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
	if (pkEventInfo == NULL || pCity == NULL)
	{
		return "";
	}

	Localization::String localizedCoreText = Localization::Lookup(pkEventInfo->GetHelp());
	localizedCoreText << GetSpyRankName(pSpy->m_eRank);
	localizedCoreText << pSpy->GetSpyName(m_pPlayer);
	localizedCoreText << pCity->getNameKey();

	CvString CoreTip = "";
	const char* const finallocalized = localizedCoreText.toUTF8();
	if (finallocalized)
	{
		CoreTip = finallocalized;
	}
	return CoreTip.c_str();
}

CvString CvPlayerEspionage::GetSpyInfo(uint uiSpyIndex, bool bNoBasic, CvCity* pCity)
{
	if (uiSpyIndex == -1)
		return "";

	CvString strSpyAtCity = "";

	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	if (!pSpy)
		return "";

	if (!bNoBasic)
	{
		if (pCity == NULL || pSpy->m_eSpyState == SPY_STATE_UNASSIGNED)
		{
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_UNASSIGNED_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
			strSpyAtCity += "[NEWLINE][NEWLINE]";
		}
		else
		{
			switch (pSpy->m_eSpyState)
			{
			case SPY_STATE_TRAVELLING:
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_TRAVELLING_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
				break;
			case SPY_STATE_SURVEILLANCE:
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_SURVEILLANCE_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
				break;
			case SPY_STATE_GATHERING_INTEL:
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_GATHERING_INTEL_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
				break;
			case SPY_STATE_RIG_ELECTION:
				strSpyAtCity += GetLocalizedText("TXT_KEY_SPY_STATE_RIGGING_ELECTION", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
				break;
			case SPY_STATE_COUNTER_INTEL:
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_COUNTER_INTEL_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
				break;
			case SPY_STATE_DEAD:
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_BUTTON_DISABLED_SPY_DEAD_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
				return strSpyAtCity;
			case SPY_STATE_MAKING_INTRODUCTIONS:
				strSpyAtCity += GetLocalizedText("TXT_KEY_SPY_STATE_MAKING_INTRODUCTIONS_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
				break;
			case SPY_STATE_SCHMOOZE:
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_UNASSIGNED_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
				break;
#if defined(MOD_API_LUA_EXTENSIONS) && defined(MOD_API_ESPIONAGE)
			case SPY_STATE_TERMINATED:
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_UNASSIGNED_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
				return strSpyAtCity;
#endif
			}
			strSpyAtCity += "[NEWLINE][NEWLINE]";
		}
	}

	if (pSpy->m_bIsDiplomat && pSpy->m_eSpyState != SPY_STATE_TRAVELLING && pSpy->m_eSpyState != SPY_STATE_SURVEILLANCE)
	{
		CvString strIntrigue = "";
		for (int i = m_aIntrigueNotificationMessages.size(); i > 0; i--)
		{
			if (m_aIntrigueNotificationMessages[i - 1].m_iTurnNum < (GC.getGame().getGameTurn() - 10))
			{
				continue;
			}
			if (m_aIntrigueNotificationMessages[i - 1].iSpyID == uiSpyIndex)
			{
				if (strIntrigue != "")
				{
					strIntrigue += "[NEWLINE]";
				}
				Localization::String strIntrigueMessage = GetIntrigueMessage(i - 1);
				strIntrigue += "[ICON_BULLET] ";
				strIntrigue += GetLocalizedText(strIntrigueMessage.toUTF8());
			}
		}
		if (strIntrigue != "")
		{
			strSpyAtCity += GetLocalizedText("TXT_KEY_SPY_INTRIGUE_DISCOVERED");
			strSpyAtCity += strIntrigue.GetCString();
		}
		return strSpyAtCity;
	}

	CityEventChoiceTypes eEventChoice = pSpy->m_eSpyFocus;
	if (eEventChoice != NO_EVENT_CHOICE_CITY)
	{
		CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
		if (pkEventChoiceInfo)
		{
			Localization::String strMessage = Localization::Lookup("TXT_KEY_ESPIONAGE_CURRENT_MISSION");
			strMessage << pCity->GetScaledHelpText(eEventChoice, false, uiSpyIndex, m_pPlayer->GetID());

			CvString pHelp = strMessage.toUTF8();

			if (strSpyAtCity != "")
				strSpyAtCity += "[NEWLINE][NEWLINE]";

			strSpyAtCity += pHelp;
		}
	}

	return strSpyAtCity;
}

CvString CvPlayerEspionage::GetSpyChanceAtCity(CvCity* pCity, uint uiSpyIndex, bool bNoBasic)
{
	if (uiSpyIndex == -1)
		return "";

	CvString strSpyAtCity = "";

	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	if (!pSpy || pCity == NULL)
		return "";

	if (pSpy->m_bIsDiplomat && pSpy->m_eSpyState == SPY_STATE_SCHMOOZE)
	{
		CvString strIntrigue = "";
		for (int i = m_aIntrigueNotificationMessages.size(); i > 0; i--)
		{
			if (m_aIntrigueNotificationMessages[i - 1].m_iTurnNum < (GC.getGame().getGameTurn() - 10))
			{
				continue;
			}
			if (m_aIntrigueNotificationMessages[i - 1].iSpyID == uiSpyIndex)
			{
				if (strIntrigue != "")
				{
					strIntrigue += "[NEWLINE]";
				}
				Localization::String strIntrigueMessage = GetIntrigueMessage(i - 1);
				strIntrigue += "[ICON_BULLET] ";
				strIntrigue += GetLocalizedText(strIntrigueMessage.toUTF8());
			}
		}
		if (strIntrigue != "")
		{
			strSpyAtCity += GetLocalizedText("TXT_KEY_SPY_INTRIGUE_DISCOVERED");
			strSpyAtCity += strIntrigue.GetCString();
		}
		return strSpyAtCity;
	}

	if (GetSpyIndexInCity(pCity) == uiSpyIndex)
	{
		//Defensive Spies:
		if (pCity->getOwner() == m_pPlayer->GetID())
		{
			strSpyAtCity += GetCityPotentialInfo(pCity, bNoBasic);
			strSpyAtCity += "[NEWLINE][NEWLINE]";

			strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_POTENTIAL");
			
			int iKillChance = GetDefenseChance(ESPIONAGE_TYPE_KILL, pCity, NO_EVENT_CHOICE_CITY, false);
			int iIdentifyChance = GetDefenseChance(ESPIONAGE_TYPE_IDENTIFY, pCity, NO_EVENT_CHOICE_CITY, false);

			strSpyAtCity += "[NEWLINE]";
			strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_BONUSES_CATCH", max(0, iIdentifyChance));
			strSpyAtCity += "[NEWLINE]";
			strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_BONUSES_KILL", max(0, iKillChance));

			if (pCity->GetCityReligions()->GetReligiousMajority() != NO_RELIGION)
			{
				int iSpyPressure = m_pPlayer->GetReligions()->GetSpyPressure(pCity->getOwner());
				if (iSpyPressure != 0)
				{
					strSpyAtCity += "[NEWLINE][NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_RELIGOUS_PRESSURE", iSpyPressure);
				}
			}
		}

		//Minor Spies
		else if (GET_PLAYER(pCity->getOwner()).isMinorCiv())
		{
			if (pSpy->m_eSpyState != SPY_STATE_TRAVELLING)
			{
				int iCoupSuccessChance = GetCoupChanceOfSuccess(uiSpyIndex);

				int iOurVotes = 0;
				int iTotalVotes = 0;

				for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
				{
					PlayerTypes eEspionagePlayer = (PlayerTypes)ui;
					CvPlayerEspionage* pPlayerEspionage = GET_PLAYER(eEspionagePlayer).GetEspionage();
					int iVotes = 0;

					if (!GET_PLAYER(eEspionagePlayer).isAlive())
					{
						continue;
					}

					CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
					int iSpyID = pCityEspionage->m_aiSpyAssignment[eEspionagePlayer];
					// if no spies are assigned here, continue
					if (iSpyID == -1)
					{
						continue;
					}

					// if the spy assigned here is not rigging the election yet, continue
					if (pPlayerEspionage->m_aSpyList[iSpyID].m_eSpyState != SPY_STATE_RIG_ELECTION)
					{
						continue;
					}

					iVotes += (pCityEspionage->m_aiAmount[eEspionagePlayer] * (100 + GET_PLAYER(eEspionagePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER))) / 100;
					if (eEspionagePlayer == m_pPlayer->GetID())
					{
						iOurVotes += (pCityEspionage->m_aiAmount[eEspionagePlayer] * (100 + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER))) / 100;
					}
					if (iVotes > 0)
					{
						iTotalVotes += iVotes;
					}
				}
				//We're the only one here?
				int iVoteChance = (iOurVotes * 100) / max(1, iTotalVotes);

				strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_COUP_CHANCE", iCoupSuccessChance);
				if (GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->GetAlly() == m_pPlayer->GetID())
				{
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_COUP_CHANCE_ALLIES");
				}
				strSpyAtCity += "[NEWLINE][NEWLINE]";
				strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_RIG_CHANCE", iVoteChance);

				if (pCity->GetCityReligions()->GetReligiousMajority() != NO_RELIGION)
				{
					int iSpyPressure = m_pPlayer->GetReligions()->GetSpyPressure(pCity->getOwner());
					if (iSpyPressure != 0)
					{
						strSpyAtCity += "[NEWLINE][NEWLINE]";
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_RELIGOUS_PRESSURE", iSpyPressure);
					}
				}
			}
		}
		else
		{
			if (pSpy->m_eSpyState == SPY_STATE_GATHERING_INTEL)
			{
				int iDefensePower = pCity->GetCityEspionage()->m_aiGoal[m_pPlayer->GetID()];
				int iOffensePower = pCity->GetCityEspionage()->m_aiRate[m_pPlayer->GetID()];
				int iCurrentPower = pCity->GetCityEspionage()->m_aiAmount[m_pPlayer->GetID()];

				strSpyAtCity += GetLocalizedText("TXT_KEY_CITY_ESPIONAGE_RATIO", iDefensePower, iOffensePower, iCurrentPower);

				CityEventChoiceTypes eEventChoice = pSpy->m_eSpyFocus;
				if (eEventChoice != NO_EVENT_CHOICE_CITY)
				{
					CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
					if (pkEventChoiceInfo)
					{
						Localization::String strMessage = Localization::Lookup("TXT_KEY_ESPIONAGE_CURRENT_MISSION");
						strMessage << pCity->GetScaledHelpText(eEventChoice, false, uiSpyIndex, m_pPlayer->GetID());

						CvString pHelp = strMessage.toUTF8();

						if (strSpyAtCity != "")
							strSpyAtCity += "[NEWLINE][NEWLINE]";
						strSpyAtCity += pHelp;
					}
				}

				if (strSpyAtCity != "")
					strSpyAtCity += "[NEWLINE][NEWLINE]";

				int iSpeedBonuses = 0;
				int iCityEspMod = pCity->GetEspionageModifier();
				int iTheirEspMod = GET_PLAYER(pCity->getOwner()).GetEspionageModifier() + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_SLOWER_MODIFIER);
				int iOurEspMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER);

				iSpeedBonuses += iOurEspMod;
				iSpeedBonuses += iCityEspMod;
				iSpeedBonuses += iTheirEspMod;

				int iInfluenceBonus = GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT() * m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(pCity->getOwner());
				iSpeedBonuses += iInfluenceBonus;
				int iOpenBordersBonus = 0;
				if (MOD_BALANCE_CORE_SPIES_ADVANCED && GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
				{
					iOpenBordersBonus = GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT();
				}
				iSpeedBonuses += iOpenBordersBonus;

				int iSpyRank = pSpy->GetSpyRank(m_pPlayer->GetID())+1;

				int iSpyBonus = (GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT() * iSpyRank);
				iSpeedBonuses += iSpyBonus;

				strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_BASE_POWER", GC.getESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT());
				strSpyAtCity += "[NEWLINE]";
				strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES", iSpeedBonuses);

				if (iCityEspMod != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_CITY_MOD", iCityEspMod);
				if (iTheirEspMod != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_THEIR_MOD", iTheirEspMod);
				if (iOurEspMod != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_OUR_MOD", iOurEspMod);
				if (iSpyBonus != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_SPY_MOD", iSpyBonus);
				if (iOpenBordersBonus != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_SPY_MOD_OB", iOpenBordersBonus);
				if (iInfluenceBonus != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_SPY_MOD_INFLUENCE", iInfluenceBonus);
			}

			if (pCity->GetCityReligions()->GetReligiousMajority() != NO_RELIGION)
			{
				int iSpyPressure = m_pPlayer->GetReligions()->GetSpyPressure(pCity->getOwner());
				if (iSpyPressure != 0)
				{
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_RELIGOUS_PRESSURE", (iSpyPressure * GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity()));
				}
			}
		}
	}

	return strSpyAtCity;
}
CvString CvPlayerEspionage::GetCityPotentialInfo(CvCity* pCity, bool bNoBasic)
{
	if (pCity == NULL)
		return "";

	CvString strSpyAtCity = "";

	if (pCity->getOwner() == m_pPlayer->GetID())
	{
		int iRank = pCity->GetEspionageRanking();

		if (!bNoBasic)
		{
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_POTENTIAL_TT", pCity->getNameKey(), iRank);
			strSpyAtCity += "[NEWLINE][NEWLINE]";
		}
		else
		{
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_POTENTIAL_SHORT_TT", pCity->getNameKey(), iRank);
			strSpyAtCity += "[NEWLINE][NEWLINE]";
		}

		int iYieldValue = pCity->getEconomicValue(pCity->getOwner()) * 100 / max(1, GC.getGame().getHighestEconomicValue());
		iYieldValue *= 8;
		iYieldValue /= 10;

		int iUnhappinessMod = 0;
		int iPop = pCity->getPopulation();
		if (iPop > 0)
		{
			iUnhappinessMod = (((pCity->getUnhappyCitizenCount()) * 100) / iPop);
		}

		//negative!
		int iCityEspionageModifier = pCity->GetEspionageModifier() * -1;
		//negative!
		int iPlayerEspionageModifier = GET_PLAYER(pCity->getOwner()).GetEspionageModifier() * -1;
		int iTheirPoliciesEspionageModifier = GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_SLOWER_MODIFIER);

		int iCounterSpy = 0;
		if (pCity->GetCityEspionage()->HasCounterSpy())
		{
			int iCounterspyIndex = GET_PLAYER(pCity->getOwner()).GetEspionage()->GetSpyIndexInCity(pCity);
			int iSpyRank = GET_PLAYER(pCity->getOwner()).GetEspionage()->m_aSpyList[iCounterspyIndex].GetSpyRank(pCity->getOwner()) + 1;
			iCounterSpy = GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT() * iSpyRank;
		}

		int iFinalModifier = iCityEspionageModifier + iPlayerEspionageModifier + iTheirPoliciesEspionageModifier + iCounterSpy;
		iFinalModifier -= (iYieldValue + iPop + iUnhappinessMod);

		strSpyAtCity += GetLocalizedText("TXT_KEY_POTENTIAL_CALCULATION", iFinalModifier);

		strSpyAtCity += "[NEWLINE][NEWLINE]";

		strSpyAtCity += GetLocalizedText("TXT_KEY_POTENTIAL_BREAKDOWN_NEGATIVE", iYieldValue, iPop, iUnhappinessMod);
		strSpyAtCity += "[NEWLINE]";
		strSpyAtCity += GetLocalizedText("TXT_KEY_POTENTIAL_BREAKDOWN_POSITIVE", iPlayerEspionageModifier + iTheirPoliciesEspionageModifier, iCityEspionageModifier, iCounterSpy);

		int iEspionageMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER);
		if (iEspionageMod != 0)
		{
			strSpyAtCity += "[NEWLINE][NEWLINE]";
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_INCREASED_CHANCE_TO_CATCH_SPY", iEspionageMod);	
		}
	}
	else
	{
		int iEspRank = pCity->GetEspionageRanking();

		int iSpy = GetSpyIndexInCity(pCity);
		CvEspionageSpy* pSpy = &(m_aSpyList[iSpy]);

		if (pSpy != NULL && (pSpy->m_eSpyState == SPY_STATE_GATHERING_INTEL || pSpy->m_eSpyState == SPY_STATE_SCHMOOZE))
		{
			if (!bNoBasic)
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_POTENTIAL_TT", pCity->getNameKey(), iEspRank);
			}
			else
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_POTENTIAL_SHORT_TT", pCity->getNameKey(), iEspRank);
			}

			int iEspionageMod = GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER);
			if (iEspionageMod != 0)
			{
				strSpyAtCity += "[NEWLINE][NEWLINE]";
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_INCREASED_CHANCE_TO_CATCH_SPY_AGAINST", iEspionageMod);
			}

			int iModifier = ((pCity->GetEspionageModifier() + GET_PLAYER(pCity->getOwner()).GetEspionageModifier()) * -1);

			if (iModifier != 0)
			{
				strSpyAtCity += "[NEWLINE][NEWLINE]";
				strSpyAtCity += GetLocalizedText("TXT_KEY_CITY_DEFENSIVE_BONUSES_AGAINST", iModifier);
			}
		}
	}

	return strSpyAtCity;
}

int CvPlayerEspionage::GetDefenseChance(CvEspionageType eEspionage, CvCity* pCity, CityEventChoiceTypes eEventChoice, bool bPreview)
{
	//Defense is based on the defensive capabilities of the city and its risk, then reduced by potency of spy there.
	int iBaseDefense = 5;
	int iChancetoIdentify = 50;
	int iChancetoKill = 0;

	if (eEventChoice != NO_EVENT_CHOICE_CITY)
	{
		CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
		if (pkEventChoiceInfo)
		{
			iChancetoKill += pkEventChoiceInfo->GetDeathModifier();
			iChancetoIdentify += pkEventChoiceInfo->GetIdentificationModifier();
		}
	}

	//chance to identify starts at inverse City Value% + Counterspy Power.

	//Chance to detect decreases based on city potency. More SECURITY = less likely!
	int iDefensePower = 5 * pCity->GetEspionageRanking();
	iDefensePower += iBaseDefense;

	PlayerTypes eOwner = pCity->getOwner();
	CvPlayer& kPlayer = GET_PLAYER(eOwner);

	int iDefModifiers = 0;
	if (pCity->GetCityEspionage()->HasCounterSpy() && !bPreview)
	{
		int iCounterSpy = kPlayer.GetEspionage()->GetSpyIndexInCity(pCity);
		CvEspionageSpy* pSpy = &kPlayer.GetEspionage()->m_aSpyList[iCounterSpy];

		iChancetoKill = (pSpy->m_eRank+1) * GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT();
		if (pSpy && pSpy->m_eSpyFocus != NO_EVENT_CHOICE_CITY)
		{
			CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(pSpy->m_eSpyFocus);
			if (pkEventChoiceInfo != NULL)
			{
				iChancetoKill += pkEventChoiceInfo->GetDeathModifier();
				iChancetoIdentify += pkEventChoiceInfo->GetIdentificationModifier();
			}
		}
	}

	//and increased again by player potency
	iDefModifiers = GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER);
	iChancetoIdentify += iDefModifiers;
	iChancetoKill += iDefModifiers;

	switch (eEspionage)
	{
		case ESPIONAGE_TYPE_DETECT:
		{
			break;
		}
		case ESPIONAGE_TYPE_IDENTIFY:
		{
			iDefensePower *= iChancetoIdentify;
			iDefensePower /= 100;
			break;
		}
		case ESPIONAGE_TYPE_KILL:
		{
			iDefensePower *= iChancetoKill;
			iDefensePower /= 100;
			break;
		}
	}

	//Defense can never be greater than 100% or less than 5%
	return range(iDefensePower, 0, 100);
}

CvSpyResult CvPlayerEspionage::GetSpyRollResult(CvCity* pCity, CityEventChoiceTypes eEventChoice)
{
	int iResult = GC.getGame().getSmallFakeRandNum(100, pCity->plot()->GetPlotIndex() + m_pPlayer->GetTreasury()->GetLifetimeGrossGold());
	if (iResult <= 0)
		iResult = 1;

	int iKillChance = GetDefenseChance(ESPIONAGE_TYPE_KILL, pCity, eEventChoice);
	int iIdentifyChance = GetDefenseChance(ESPIONAGE_TYPE_IDENTIFY, pCity, eEventChoice);

	//success! we didn't die...
	if (iResult > iKillChance)
	{
		if (iResult > iIdentifyChance)
			return SPY_RESULT_DETECTED;
		else
			return SPY_RESULT_IDENTIFIED;
	}
	else
		return SPY_RESULT_KILLED;
}
#endif

/// UncoverIntrigue - Determine if the spy uncovers any secret information and pass it along to the player
void CvPlayerEspionage::UncoverIntrigue(uint uiSpyIndex)
{
	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	CvPlot* pCityPlot = GC.getMap().plot(pSpy->m_iCityX, pSpy->m_iCityY);
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;
	PlayerTypes ePlayer = m_pPlayer->GetID();
	PlayerTypes eCityOwner = NO_PLAYER;
	if(pCityPlot)
	{
		pCity = pCityPlot->getPlotCity();
		eCityOwner = pCity->getOwner();
		pCityEspionage = pCity->GetCityEspionage();
	}

	CvAssertMsg(pCity, "Spy needs to be in city to uncover intrigue");
	if(!pCity)
	{
		return;
	}

	CvAssertMsg(ePlayer != eCityOwner, "Uncover intrigue should not occur when a spy is in their own city");
	if(ePlayer == eCityOwner)
	{
		return;
	}

	// make a list of the active civs
	std::vector<int> aiMajorCivIndex;
	for(int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		if(GET_PLAYER((PlayerTypes)i).isAlive())
		{
			aiMajorCivIndex.push_back(i);
		}
	}

	// randomize that list
	for(uint ui = 0; ui < aiMajorCivIndex.size(); ui++)
	{
		uint uiTargetSlot = GC.getGame().getSmallFakeRandNum(aiMajorCivIndex.size(),pCity->plot()->GetPlotIndex()+ui);
		int iTempValue = aiMajorCivIndex[ui];
		aiMajorCivIndex[ui] = aiMajorCivIndex[uiTargetSlot];
		aiMajorCivIndex[uiTargetSlot] = iTempValue;
	}

	// go through to determine any intrigue
	int iSpyRank = 0;
	for(uint ui = 0; ui < aiMajorCivIndex.size(); ui++)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes)aiMajorCivIndex[ui];
		// a player shouldn't target themselves for a sneak attack. That's strange.
		if(eTargetPlayer == eCityOwner)
			continue;

		// Don't tell other civs about what the shadow ai is thinking because that's incorrect information!
		if(GET_PLAYER(eCityOwner).isHuman())
			continue;

		// Should maybe randomize the order?
		CvAIOperation* pSneakAttackOperation = m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_CITY_ATTACK_COMBINED,eTargetPlayer);
		if (!pSneakAttackOperation)
			pSneakAttackOperation = m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_CITY_ATTACK_NAVAL,eTargetPlayer);
		if (!pSneakAttackOperation)
			pSneakAttackOperation = m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_CITY_ATTACK_LAND,eTargetPlayer);
		if (!pSneakAttackOperation)
			continue;

		CvCity* pTargetCity = NULL;
#if defined(MOD_BALANCE_CORE_SPIES)
		iSpyRank = m_aSpyList[uiSpyIndex].GetSpyRank(ePlayer) + m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eCityOwner);
#else
		iSpyRank = m_aSpyList[uiSpyIndex].GetSpyRank(ePlayer);
#endif

		if(iSpyRank >= SPY_RANK_AGENT)
		{
			CvPlot* pPlot = pSneakAttackOperation->GetTargetPlot();
			if(pPlot)
			{
				pTargetCity = pPlot->getPlotCity();
			}
		}

		PlayerTypes eRevealedTargetPlayer = NO_PLAYER;
		if (iSpyRank == SPY_RANK_RECRUIT)
		{
			eRevealedTargetPlayer = (PlayerTypes)MAX_MAJOR_CIVS; // hack to indicate that we shouldn't know the target due to our low spy rank
		}
		else if(GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER(eTargetPlayer).getTeam()))
		{
			eRevealedTargetPlayer = eTargetPlayer;
		}

		CvIntrigueType eType = pSneakAttackOperation->GetOperationType()==AI_OPERATION_CITY_ATTACK_LAND ? INTRIGUE_TYPE_ARMY_SNEAK_ATTACK : INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK;
		AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, eRevealedTargetPlayer, NO_BUILDING, NO_PROJECT, eType, uiSpyIndex, pTargetCity, true);

#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
		if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
		{
			int iNewResult = GC.getGame().getSmallFakeRandNum(100, *pCity->plot());
			if(iNewResult >= 80)
			{
				LevelUpSpy(uiSpyIndex);
			}
		}
#endif

		// If a sneak attack is reported, bust out of the loop
		break;
	}

	// building up an army
	if(!GET_PLAYER(eCityOwner).isHuman())
	{
		if (GET_PLAYER(eCityOwner).GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_LAND))
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_ARMY, uiSpyIndex, pCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
			if (MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
			{
				int iNewResult = GC.getGame().getSmallFakeRandNum(100, *pCity->plot());
				if (iNewResult >= 85)
				{
					LevelUpSpy(uiSpyIndex);
				}
			}
#endif
		}
		else if (GET_PLAYER(eCityOwner).GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_NAVAL) || (GET_PLAYER(eCityOwner).GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_COMBINED)))
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY, uiSpyIndex, pCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
			if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
			{
				int iNewResult = GC.getGame().getSmallFakeRandNum(100, *pCity->plot());
				if(iNewResult >= 85)
				{
					LevelUpSpy(uiSpyIndex);
				}
			}
#endif
		}
	}

	// deception!
	CvDiplomacyAI* pTargetDiploAI = GET_PLAYER(eCityOwner).GetDiplomacyAI();
	CvAssertMsg(pTargetDiploAI, "pTargetDiploAI is null");
	if(!pTargetDiploAI)
	{
		return;
	}

	for(uint ui = 0; ui < aiMajorCivIndex.size(); ui++)
	{
		PlayerTypes eOtherOtherPlayer = (PlayerTypes)aiMajorCivIndex[ui];
		// doesn't make sense for player to give information on themselves
		if(eOtherOtherPlayer == eCityOwner)
		{
			continue;
		}

		// Don't tell other civs about what the shadow ai is thinking because that's incorrect information!
		if(GET_PLAYER(eCityOwner).isHuman())
		{
			continue;
		}

		CivApproachTypes eSurfaceApproach = pTargetDiploAI->GetSurfaceApproach(eOtherOtherPlayer);
		CivApproachTypes eHonestApproach = pTargetDiploAI->GetCivApproach(eOtherOtherPlayer);

		// if the current approach is a dangerous approach
		if(eHonestApproach == CIV_APPROACH_DECEPTIVE || eHonestApproach == CIV_APPROACH_WAR)
		{
			// if the surface approach hides this
			if(eSurfaceApproach == CIV_APPROACH_FRIENDLY || eSurfaceApproach == CIV_APPROACH_NEUTRAL || eSurfaceApproach == CIV_APPROACH_GUARDED)
			{
				if(GET_TEAM(GET_PLAYER(eCityOwner).getTeam()).isAtWar(GET_PLAYER(eOtherOtherPlayer).getTeam()))
				{
					// If the teams are already at war, this isn't notable
					continue;
				}

				if(GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER(eOtherOtherPlayer).getTeam()))
				{
					AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, eOtherOtherPlayer, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_DECEPTION, uiSpyIndex, pCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
					if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
					{
						int iNewResult = GC.getGame().getSmallFakeRandNum(100, *pCity->plot());
						if(iNewResult >= 90)
						{
							LevelUpSpy(uiSpyIndex);
						}
					}
#endif
				}
				else
				{
					AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_DECEPTION, uiSpyIndex, pCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
					if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
					{
						int iNewResult = GC.getGame().getSmallFakeRandNum(100, *pCity->plot());
						if(iNewResult >= 90)
						{
							LevelUpSpy(uiSpyIndex);
						}
					}
#endif
				}
				break; // we reported intrigue, now bail out
			}
		}
	}

	ProjectTypes eProject = pCity->getProductionProject();
	BuildingTypes eBuilding = pCity->getProductionBuilding();
	bool bNotifyAboutConstruction = false;
	if (eProject != NO_PROJECT)
	{
		CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
		CvProjectEntry& pProjectInfo = *pkProjectInfo;
		if (!pProjectInfo.IsRepeatable())
		{
			bNotifyAboutConstruction = true;
		}
	}
	else if (eBuilding != NO_BUILDING)
	{
		CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
		CvAssertMsg(pBuildingInfo, "pBuildingInfo is null");
		if (pBuildingInfo)
		{
			if (::isWorldWonderClass(pBuildingInfo->GetBuildingClassInfo()))
			{
				bNotifyAboutConstruction = true;
			}
		}
	}

	if (bNotifyAboutConstruction)
	{
		AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, eBuilding, eProject, INTRIGUE_TYPE_CONSTRUCTING_WONDER, uiSpyIndex, pCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
		if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
		{
			int iNewResult = GC.getGame().getSmallFakeRandNum(100, *pCity->plot());
			if(iNewResult >= 90)
			{
				LevelUpSpy(uiSpyIndex);
			}
		}
#endif
	}
}
#if defined(MOD_BALANCE_CORE)
/// UncoverIntrigue - Determine if the spy uncovers any secret information and pass it along to the player
void CvPlayerEspionage::GetRandomIntrigue(CvCity* pCity, uint uiSpyIndex)
{
	if(pCity == NULL)
	{
		return;
	}
	PlayerTypes eOtherPlayer = pCity->getOwner();
	if(eOtherPlayer == NO_PLAYER)
	{
		return;
	}
	// make a list of the active civs
	std::vector<int> aiMajorCivIndex;
	for(int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		if(GET_PLAYER((PlayerTypes)i).isAlive())
		{
			aiMajorCivIndex.push_back(i);
		}
	}

	// randomize that list
	for(uint ui = 0; ui < aiMajorCivIndex.size(); ui++)
	{
		uint uiTargetSlot = GC.getGame().getSmallFakeRandNum(aiMajorCivIndex.size(),pCity->plot()->GetPlotIndex()+ui);
		int iTempValue = aiMajorCivIndex[ui];
		aiMajorCivIndex[ui] = aiMajorCivIndex[uiTargetSlot];
		aiMajorCivIndex[uiTargetSlot] = iTempValue;
	}
	// sending out a sneak attack
	for(uint ui = 0; ui < aiMajorCivIndex.size(); ui++)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes)aiMajorCivIndex[ui];
		// a player shouldn't target themselves for a sneak attack. That's strange.
		if(eTargetPlayer == eOtherPlayer)
			continue;

		// Don't tell other civs about what the shadow ai is thinking because that's incorrect information!
		if(GET_PLAYER(eOtherPlayer).isHuman())
			continue;

		// Should maybe randomize the order?
		CvAIOperation* pSneakAttackOperation = m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_CITY_ATTACK_COMBINED,eTargetPlayer);
		if (!pSneakAttackOperation)
			pSneakAttackOperation = m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_CITY_ATTACK_NAVAL,eTargetPlayer);
		if (!pSneakAttackOperation)
			pSneakAttackOperation = m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_CITY_ATTACK_LAND,eTargetPlayer);
		if (!pSneakAttackOperation)
			continue;

		CvCity* pTargetCity = pSneakAttackOperation->GetTargetPlot()->getPlotCity();
		PlayerTypes eRevealedTargetPlayer = (PlayerTypes)MAX_MAJOR_CIVS; // hack to indicate that we shouldn't know the target due to our low spy rank
		if(GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER(eTargetPlayer).getTeam()))
		{
			eRevealedTargetPlayer = eTargetPlayer;
		}

		CvIntrigueType eType = pSneakAttackOperation->GetOperationType() == AI_OPERATION_CITY_ATTACK_LAND ? INTRIGUE_TYPE_ARMY_SNEAK_ATTACK : INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK;
		AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, eRevealedTargetPlayer, NO_BUILDING, NO_PROJECT, eType, uiSpyIndex, pTargetCity, true);
		// If a sneak attack is reported, bust out of the loop
		break;
	}

	// building up an army
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		if (GET_PLAYER(eOtherPlayer).GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_LAND))
			AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_ARMY, uiSpyIndex, pCity, true);
		else if (GET_PLAYER(eOtherPlayer).GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_NAVAL) || GET_PLAYER(eOtherPlayer).GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_COMBINED))
			AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY, uiSpyIndex, pCity, true);
	}

	// deception!
	CvDiplomacyAI* pTargetDiploAI = GET_PLAYER(eOtherPlayer).GetDiplomacyAI();
	CvAssertMsg(pTargetDiploAI, "pTargetDiploAI is null");
	if(!pTargetDiploAI)
	{
		return;
	}

	for(uint ui = 0; ui < aiMajorCivIndex.size(); ui++)
	{
		PlayerTypes eOtherOtherPlayer = (PlayerTypes)aiMajorCivIndex[ui];
		// doesn't make sense for player to give information on themselves
		if(eOtherOtherPlayer == eOtherPlayer)
		{
			continue;
		}

		// Don't tell other civs about what the shadow ai is thinking because that's incorrect information!
		if(GET_PLAYER(eOtherPlayer).isHuman())
		{
			continue;
		}

		CivApproachTypes eSurfaceApproach = pTargetDiploAI->GetSurfaceApproach(eOtherOtherPlayer);
		CivApproachTypes eHonestApproach = pTargetDiploAI->GetCivApproach(eOtherOtherPlayer);

		// if the current approach is a dangerous approach
		if (eHonestApproach == CIV_APPROACH_DECEPTIVE || eHonestApproach == CIV_APPROACH_WAR)
		{
			// if the surface approach hides this
			if (eSurfaceApproach == CIV_APPROACH_FRIENDLY || eSurfaceApproach == CIV_APPROACH_NEUTRAL || eSurfaceApproach == CIV_APPROACH_GUARDED)
			{
				if(GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).isAtWar(GET_PLAYER(eOtherOtherPlayer).getTeam()))
				{
					// If the teams are already at war, this isn't notable
					continue;
				}

				if(GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER(eOtherOtherPlayer).getTeam()))
				{
					AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, eOtherOtherPlayer, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_DECEPTION, uiSpyIndex, pCity, true);
				}
				else
				{
					AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_DECEPTION, uiSpyIndex, pCity, true);
				}
				break; // we reported intrigue, now bail out
			}
		}
	}

	ProjectTypes eProject = pCity->getProductionProject();
	BuildingTypes eBuilding = pCity->getProductionBuilding();
	bool bNotifyAboutConstruction = false;
	if (eProject != NO_PROJECT)
	{
		bNotifyAboutConstruction = true;
	}
	else if (eBuilding != NO_BUILDING)
	{
		CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
		CvAssertMsg(pBuildingInfo, "pBuildingInfo is null");
		if (pBuildingInfo)
		{
			if (::isWorldWonderClass(pBuildingInfo->GetBuildingClassInfo()))
			{
				bNotifyAboutConstruction = true;
			}
		}
	}

	if (bNotifyAboutConstruction)
	{
		AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, NO_PLAYER, eBuilding, eProject, INTRIGUE_TYPE_CONSTRUCTING_WONDER, uiSpyIndex, pCity, true);
	}
}
#endif

bool isSpyNameInUse(CvPlayer* pPlayer, const char* szSpyName)
{
	CvPlayerEspionage* pkPlayerEspionage = pPlayer->GetEspionage();

	for (uint uiSpy = 0; uiSpy < pkPlayerEspionage->m_aSpyList.size(); ++uiSpy) {
		CvEspionageSpy* pSpy = &(pkPlayerEspionage->m_aSpyList[uiSpy]);

		if (strcmp(szSpyName, pSpy->GetSpyName(pPlayer)) == 0) {
			return true;
		}
	}

	return false;
}

bool isSpyNameInUse(const char* szSpyName)
{
	for (int iPlayer = 0; iPlayer < MAX_MAJOR_CIVS; iPlayer++) {
		CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (thisPlayer.isEverAlive()) {
			if (isSpyNameInUse(&thisPlayer, szSpyName)) {
				return true;
			}
		}
	}
	
	return false;
}

bool pickSpyName(const CvCivilizationInfo& kCivInfo, CvEspionageSpy* pSpy)
{
	int iCivSpyNames = kCivInfo.getNumSpyNames();
	if (iCivSpyNames > 0)
	{
#if defined(MOD_CORE_REDUCE_RANDOMNESS)
		int iOffset = GC.getGame().getSmallFakeRandNum(iCivSpyNames, iCivSpyNames);
#else
		int iOffset = GC.getGame().getJonRandNum(iCivSpyNames, "Spy name offset");
#endif

		for (int i = 0; i < iCivSpyNames; i++) {
			const char* szSpyName = kCivInfo.getSpyNames((i + iOffset) % iCivSpyNames);

			if (!isSpyNameInUse(szSpyName)) {
				pSpy->m_sName = szSpyName;
				// CUSTOMLOG("Using spy name %s (from civ %s)", szSpyName, pkCivInfo->GetDescription());
				return true;
			}
		}
	} else {
		CUSTOMLOG("WARNING! Civilization %s appears to be missing spy names", kCivInfo.GetDescription());
	}

	return false;
}

bool isCivInPlay(const CivilizationTypes eCiv)
{
	for (int iPlayer = 0; iPlayer < MAX_MAJOR_CIVS; iPlayer++) {
		CvPlayerAI& thisPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (thisPlayer.isEverAlive() && thisPlayer.getCivilizationType() == eCiv) {
			return true;
		}
	}

	return false;
}

void CvPlayerEspionage::GetNextSpyName(CvEspionageSpy* pSpy)
{
	// CUSTOMLOG("GetNextSpyName for %s", m_pPlayer->getCivilizationInfo().GetDescription());

	// This is a complete rewrite, using similiar logic to that for city names
	// If there is a spare spy name for this player, use that
	// Otherwise, pick a spy name not in use at random from the civs not in the game
	// Otherwise, pick a spy name not in use at random from the civs in the game
	// Otherwise, use the default unknown spy name

	// As there is now no concept of "name index", just set this to -1 (unknown)
	pSpy->m_iName = -1;

	// Try to locate a spy name within the player's civ
	if (pickSpyName(m_pPlayer->getCivilizationInfo(), pSpy)) {
		return;
	}

	// Try to locate a spy name not in use by a civ not in the game
	int iMaxCivs = GC.getNumCivilizationInfos();
	int iCivOffset = GC.getGame().getSmallFakeRandNum(iMaxCivs, m_pPlayer->GetPseudoRandomSeed());
	for (int i = 0; i < GC.getNumCivilizationInfos(); i++) {
		const CivilizationTypes eCiv = static_cast<CivilizationTypes>((i + iCivOffset) % iMaxCivs);
		CvCivilizationInfo* pkCivilizationInfo = GC.getCivilizationInfo(eCiv);
		
		if (pkCivilizationInfo != NULL && pkCivilizationInfo->getNumSpyNames() > 0) {
			if (isCivInPlay(eCiv)) {
				continue;
			}

			if (pickSpyName(*pkCivilizationInfo, pSpy)) {
				return;
			}
		}
	}

	// Try to locate a spy name not in use by a civ in the game
	int iPlayerOffset = GC.getGame().getSmallFakeRandNum(MAX_MAJOR_CIVS, m_pPlayer->GetPseudoRandomSeed());

	for (int i = 0; i < MAX_MAJOR_CIVS; i++) {
		const PlayerTypes ePlayer = static_cast<PlayerTypes>((i + iPlayerOffset) % MAX_MAJOR_CIVS);
		CvPlayerAI& thisPlayer = GET_PLAYER(ePlayer);
		if (thisPlayer.isEverAlive() && thisPlayer.getCivilizationInfo().getNumSpyNames() > 0) {
			if (pickSpyName(thisPlayer.getCivilizationInfo(), pSpy)) {
				return;
			}
		}
	}

	// Just use the unknown spy name
	pSpy->m_sName = "TXT_KEY_SPY_NAME_UNKNOWN";
	CUSTOMLOG("Using unknown spy name TXT_KEY_SPY_NAME_UNKNOWN");
}

/// IsSpyInCity - Checks to see if spy is in a city
bool CvPlayerEspionage::IsSpyInCity(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "uiSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	if(m_aSpyList[uiSpyIndex].m_iCityX != -1 && m_aSpyList[uiSpyIndex].m_iCityY != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CvCity* CvPlayerEspionage::GetCityWithSpy(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "uiSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	if(m_aSpyList[uiSpyIndex].m_iCityX == -1 && m_aSpyList[uiSpyIndex].m_iCityY == -1)
	{
		return NULL;
	}

	CvPlot* pCityPlot = GC.getMap().plot(m_aSpyList[uiSpyIndex].m_iCityX, m_aSpyList[uiSpyIndex].m_iCityY);
	CvAssertMsg(pCityPlot, "pCityPlot is null. This should have been caught above");
	CvCity* pCity = NULL;
	if(pCityPlot)
	{
		pCity = pCityPlot->getPlotCity();
		CvAssertMsg(pCity, "Spy is pointing to empty city, what's up with that");
	}

	return pCity;
}


int CvPlayerEspionage::GetSpyIndexInCity(CvCity* pCity)
{
	CvAssertMsg(pCity, "pCity is null. Can't do much with this");
	if(!pCity)
	{
		return -1;
	}

	return pCity->GetCityEspionage()->m_aiSpyAssignment[m_pPlayer->GetID()];
}


/// CanEverMoveSpyTo - May a spy move into this city at any point (used for building lists
bool CvPlayerEspionage::CanEverMoveSpyTo(CvCity* pCity)
{
	// allow moving into empty spaces
	if(!pCity)
	{
		return true;
	}

	CvPlot* pPlot = pCity->plot();
	CvAssertMsg(pPlot, "No plot() for pCity");
	if(!pPlot)
	{
		return false;
	}

	if(!GET_TEAM(m_pPlayer->getTeam()).isHasMet(pCity->getTeam()))
	{
		return false;
	}

	// check to see if the plot can be seen
	if(!pPlot->isRevealed(m_pPlayer->getTeam()))
	{
		return false;
	}

#if defined(MOD_EVENTS_ESPIONAGE)
	if (MOD_EVENTS_ESPIONAGE) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_EspionageCanMoveSpyTo, m_pPlayer->GetID(), pCity->getOwner(), pCity->GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif
	
	return true;
}

/// CanMoveSpyTo - May a spy move into this city
bool CvPlayerEspionage::CanMoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat)
{
	// This allows the player to move the spy off the board
	if(!pCity)
	{
		return true;
	}

	if(!CanEverMoveSpyTo(pCity))
	{
		return false;
	}

	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	// check to see if there is someone already there
	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	CvAssertMsg(pCityEspionage, "pCityEspionage is null");
	if(!pCityEspionage)
	{
		return false;
	}

	PlayerTypes ePlayerID = m_pPlayer->GetID();
	if(pCityEspionage->m_aiSpyAssignment[ePlayerID] != -1)
	{
		return false;
	}

	if (bAsDiplomat)
	{
		// diplomatic spy must be in capital
		if (!pCity->isCapital() || pCity->getOwner() == m_pPlayer->GetID())
		{
			return false;
		}

		if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(pCity->getTeam()))
		{
			return false;
		}
	}

	return true;
}

/// MoveSpyTo - Move a spy into this city
bool CvPlayerEspionage::MoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}
	if (!CanMoveSpyTo(pCity, uiSpyIndex, bAsDiplomat))
	{
		return false;
	}

	CvCity* pOldCity = NULL;

	if(IsSpyInCity(uiSpyIndex))
	{
		pOldCity = GetCityWithSpy(uiSpyIndex);
		// trying to move into a city where the spy is already at
		if(pCity == pOldCity)
		{
			return true;
		}

		if(!ExtractSpyFromCity(uiSpyIndex))
		{
			CvAssertMsg(false, "Failed to extract spy");
			return false;
		}
	}

	if(pCity)
	{
		CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
		CvAssertMsg(pCityEspionage, "Could not get CvCityEspionage from pCity");
		if(!pCityEspionage)
		{
			return false;
		}

		//time to reset the spys
		m_aSpyList[uiSpyIndex].m_iPotentialAtStart = -1;
		m_aSpyList[uiSpyIndex].m_eSpyFocus = NO_EVENT_CHOICE_CITY;
		m_aSpyList[uiSpyIndex].m_iCityX = pCity->getX();
		m_aSpyList[uiSpyIndex].m_iCityY = pCity->getY();
		pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] = uiSpyIndex;
#if defined(MOD_API_ESPIONAGE)
		m_aSpyList[uiSpyIndex].SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_TRAVELLING);
#else
		m_aSpyList[uiSpyIndex].m_eSpyState = SPY_STATE_TRAVELLING;
#endif
		m_aSpyList[uiSpyIndex].m_bIsDiplomat = bAsDiplomat;
		int iRate = CalcPerTurn(SPY_STATE_TRAVELLING, pCity, uiSpyIndex);
		int iGoal = CalcRequired(SPY_STATE_TRAVELLING, pCity, uiSpyIndex);
		pCityEspionage->SetActivity(m_pPlayer->GetID(), 0, iRate, iGoal);
	}

	if(GC.getLogging())
	{
		CvString strMsg;
		strMsg.Format("Moving spy, %d,", uiSpyIndex);
		strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
		strMsg += ",";
		if(pOldCity)
		{
			strMsg += GET_PLAYER(pOldCity->getOwner()).getCivilizationShortDescription();
			strMsg += ",";
			strMsg += pOldCity->getName();
		}
		else
		{
			strMsg += "Unassigned";
			strMsg += ",";
		}
		strMsg += ",";

		if(pCity)
		{
			strMsg += GET_PLAYER(pCity->getOwner()).getCivilizationShortDescription();
			strMsg += ",";
			strMsg += pCity->getName();
		}
		else
		{
			strMsg += "Unassigned";
			strMsg += ",";
		}
		strMsg += ",";

		if (bAsDiplomat)
		{
			strMsg += "Diplomat";
		}
		strMsg += ",";

		LogEspionageMsg(strMsg);
	}

	return true;
}

/// RemoveSpyFromCity - Remove a spy from this city
bool CvPlayerEspionage::ExtractSpyFromCity(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	if(!IsSpyInCity(uiSpyIndex))
	{
		return true;
	}

	bool bHadSurveillance = HasEstablishedSurveillance(uiSpyIndex);

	int iCityX = m_aSpyList[uiSpyIndex].m_iCityX;
	int iCityY = m_aSpyList[uiSpyIndex].m_iCityY;

	m_aSpyList[uiSpyIndex].m_eSpyFocus = NO_EVENT_CHOICE_CITY;
	m_aSpyList[uiSpyIndex].m_iCityX = -1;
	m_aSpyList[uiSpyIndex].m_iCityY = -1;
#if defined(MOD_API_ESPIONAGE)
	m_aSpyList[uiSpyIndex].SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_UNASSIGNED);
#else
	m_aSpyList[uiSpyIndex].m_eSpyState = SPY_STATE_UNASSIGNED;
#endif

	CvPlot* pPlot = GC.getMap().plot(iCityX, iCityY);
	CvAssertMsg(pPlot, "Spy coordinates did not point to plot");
	if(!pPlot)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	CvAssertMsg(pCity, "No city in plot pointed to by spy");
	if(!pCity)
	{
		return false;
	}

	// turn off visibility of city
	if(bHadSurveillance)
	{
#if defined(MOD_API_EXTENSIONS)
		pCity->plot()->changeAdjacentSight(m_pPlayer->getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), false, NO_INVISIBLE, NO_DIRECTION);
#else
		pCity->plot()->changeAdjacentSight(m_pPlayer->getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), false, NO_INVISIBLE, NO_DIRECTION, false);
#endif
	}

	pCity->GetCityEspionage()->m_aiSpyAssignment[m_pPlayer->GetID()] = -1;
	pCity->GetCityEspionage()->ResetProgress(m_pPlayer->GetID());

	if (pCity->GetCityEspionage()->m_aiPendingEventsForPlayer[m_pPlayer->GetID()] > 0)
		pCity->GetCityEspionage()->m_aiPendingEventsForPlayer[m_pPlayer->GetID()]--;

	return true;
}

/// LevelUpSpy - Move spy to next experience level
void CvPlayerEspionage::LevelUpSpy(uint uiSpyIndex)
{
	// if the spy can level up and it's not dead
#if defined(MOD_API_ESPIONAGE)
	if(m_aSpyList[uiSpyIndex].m_eRank < NUM_SPY_RANKS - 1 && !(m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_DEAD || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_TERMINATED))
#else
	if(m_aSpyList[uiSpyIndex].m_eRank < NUM_SPY_RANKS - 1 && m_aSpyList[uiSpyIndex].m_eSpyState != SPY_STATE_DEAD)
#endif
	{
		CvSpyRank eOriginalRank = m_aSpyList[uiSpyIndex].m_eRank;

		// announce promotion through notification
		m_aSpyList[uiSpyIndex].m_eRank = (CvSpyRank)(m_aSpyList[uiSpyIndex].m_eRank + 1);

		CvNotifications* pNotifications = m_pPlayer->GetNotifications();
		if(pNotifications)
		{
			const char* szSpyName = m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
			const char* szOldPromotion = GetSpyRankName(eOriginalRank);
			const char* szNewPromotion = GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_PROMOTED", szSpyName, szOldPromotion, szNewPromotion);
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_SPY_PROMOTED", szSpyName);
			pNotifications->Add(NOTIFICATION_SPY_PROMOTION, strBuffer, strSummary, -1, -1, 0);
		}
	}
}

#if defined(MOD_API_ESPIONAGE)
/// SetPassive - Passive spies will not attempt to rig elections or steal technology
void CvPlayerEspionage::SetPassive(uint uiSpyIndex, bool bPassive) {
	m_aSpyList[uiSpyIndex].m_bPassive = bPassive;
}

/// SetOutcome - Forces the outcome of an (external) espionage activity
void CvPlayerEspionage::SetOutcome(uint uiSpyIndex, uint uiSpyResult, bool bAffectsDiplomacy) {
	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	PlayerTypes eCityOwner = NO_PLAYER;
	CvPlot* pCityPlot = GC.getMap().plot(pSpy->m_iCityX, pSpy->m_iCityY);
	if (pCityPlot) {
		CvCity* pCity = pCityPlot->getPlotCity();
		if (pCity) {
			eCityOwner = pCity->getOwner();
		}
	}

	switch(uiSpyResult) {
	case SPY_RESULT_UNDETECTED:
		// Nothing much happening here
		break;
	case SPY_RESULT_DETECTED:
		// The defended will be mad, but not know who to be mad at!
		break;
	case SPY_RESULT_IDENTIFIED:
		if (bAffectsDiplomacy && eCityOwner != NO_PLAYER) {
			CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(eCityOwner).GetEspionageAI();
			CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
			if (pDefenderEspionageAI) {
				pDefenderEspionageAI->m_aiTurnLastSpyCaught[m_pPlayer->GetID()] = GC.getGame().getGameTurn();
				pDefenderEspionageAI->m_aiNumSpiesCaught[m_pPlayer->GetID()]++;
			}
		}
		break;
	case SPY_RESULT_KILLED:
	case SPY_RESULT_ELIMINATED:
		ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
#if defined(MOD_API_ESPIONAGE)
		pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, (uiSpyResult == SPY_RESULT_KILLED) ? SPY_STATE_DEAD : SPY_STATE_TERMINATED);
#else
		pSpy->m_eSpyState = (uiSpyResult == SPY_RESULT_KILLED) ? SPY_STATE_DEAD : SPY_STATE_TERMINATED;
#endif

		if (bAffectsDiplomacy && eCityOwner != NO_PLAYER) {
			CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(eCityOwner).GetEspionageAI();
			CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
			if (pDefenderEspionageAI) {
				pDefenderEspionageAI->m_aiTurnLastSpyKilled[m_pPlayer->GetID()] = GC.getGame().getGameTurn();
				pDefenderEspionageAI->m_aiNumSpiesKilled[m_pPlayer->GetID()]++;
			}

			CvEspionageAI* pEspionageAI = m_pPlayer->GetEspionageAI();
			CvAssertMsg(pEspionageAI, "pEspionageAI is null");
			if (pEspionageAI) {
				pEspionageAI->m_aiTurnLastSpyDied[eCityOwner] = GC.getGame().getGameTurn();
				pEspionageAI->m_aiNumSpiesDied[eCityOwner]++;
			}
		}
		break;
	}
}
#endif

/// UpdateCity - This is called when a policy is adopted that modifies how quickly spies can steal technology
void CvPlayerEspionage::UpdateSpies()
{
	for(uint uiSpy = 0; uiSpy < m_aSpyList.size(); uiSpy++)
	{
		CvCity* pCity = GetCityWithSpy(uiSpy);
		if (pCity)
		{
			UpdateCity(pCity);
		}
	}
}

/// UpdateCity - This is called when a city builds a defensive building that will effect the rate of espionage generated
void CvPlayerEspionage::UpdateCity(CvCity* pCity)
{
	CvAssertMsg(pCity, "pCity is null");
	if(pCity == NULL)
	{
		return;
	}

	int iSpyIndex = GetSpyIndexInCity(pCity);

	// if there isn't a spy in the city, bail out
	if(iSpyIndex == -1)
	{
		return;
	}

	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	CvAssertMsg(pCityEspionage, "pCityEspionage is null");
	if(pCityEspionage == NULL)
	{
		return;
	}
	pCityEspionage->m_aiLastBasePotential[m_pPlayer->GetID()] = CalcPerTurn(m_aSpyList[iSpyIndex].m_eSpyState, pCity, -1);
	pCityEspionage->m_aiLastPotential[m_pPlayer->GetID()] = CalcPerTurn(m_aSpyList[iSpyIndex].m_eSpyState, pCity, iSpyIndex);
	pCityEspionage->m_aiRate[m_pPlayer->GetID()] = pCityEspionage->m_aiLastPotential[m_pPlayer->GetID()];
}

int CvPlayerEspionage::GetSpyPower(CvCity* pCity, int iSpyIndex)
{
	int iBaseSpyPower = GC.getESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT();

	int iSpyRank = m_aSpyList[iSpyIndex].GetSpyRank(m_pPlayer->GetID()) + 1;
	iSpyRank += m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(pCity->getOwner());
	if (MOD_BALANCE_CORE_SPIES_ADVANCED && GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
	{
		iSpyRank += 1;
	}
	iSpyRank *= GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT();

	int iMyPoliciesEspionageModifier = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER);

	iBaseSpyPower *= iMyPoliciesEspionageModifier + GC.getGame().getGameSpeedInfo().getSpyRatePercent() + iSpyRank;
	iBaseSpyPower /= 100;

	return iBaseSpyPower;
}
int CvPlayerEspionage::GetSpyResistance(CvCity* pCity, bool bConsiderPotentialSpy)
{
	int iBaseResistance = GC.getESPIONAGE_GATHERING_INTEL_COST_PERCENT();

	int iYieldValue = pCity->getEconomicValue(pCity->getOwner()) * 100 / max(1, GC.getGame().getHighestEconomicValue());
	iYieldValue *= 8;
	iYieldValue /= 10;

	int iUnhappinessMod = 0;
	int iPop = pCity->getPopulation();
	if (iPop > 0)
	{
		iUnhappinessMod = (((pCity->getUnhappyCitizenCount()) * 100) / iPop);
	}

	//negative!
	int iCityEspionageModifier = pCity->GetEspionageModifier() * -1;
	//negative!
	int iPlayerEspionageModifier = GET_PLAYER(pCity->getOwner()).GetEspionageModifier() * -1;
	int iTheirPoliciesEspionageModifier = GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_SLOWER_MODIFIER);

	int iCounterSpy = 0;
	if (pCity->GetCityEspionage()->HasCounterSpy() || bConsiderPotentialSpy)
	{
		if (bConsiderPotentialSpy)
		{
			iCounterSpy = GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT();
		}
		else
		{
			int iCounterspyIndex = GET_PLAYER(pCity->getOwner()).GetEspionage()->GetSpyIndexInCity(pCity);
			int iSpyRank = GET_PLAYER(pCity->getOwner()).GetEspionage()->m_aSpyList[iCounterspyIndex].GetSpyRank(pCity->getOwner()) + 1;
			iCounterSpy = GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT() * iSpyRank;
		}
	}

	int iFinalModifier = iCityEspionageModifier + iPlayerEspionageModifier + iTheirPoliciesEspionageModifier + iCounterSpy;
	iFinalModifier -= (iYieldValue + iPop + iUnhappinessMod);

	iBaseResistance *= 100 + iFinalModifier;
	iBaseResistance /= 100;

	return iBaseResistance;
}

/// CalcPerTurn - How much the spy will do at this task per turn
int CvPlayerEspionage::CalcPerTurn(int iSpyState, CvCity* pCity, int iSpyIndex, bool bGlobalCheck, bool bFirstTime)
{
	int iBase = 1;
	if (!bGlobalCheck && m_pPlayer->GetPlayerTraits()->GetSpyMoveRateBonus() != 0)
	{
		iBase += m_pPlayer->GetPlayerTraits()->GetSpyMoveRateBonus();
	}
	CvAssertMsg(pCity, "pCity is null");
	switch(iSpyState)
	{
	case SPY_STATE_TRAVELLING:
	{
		return iBase;
	}
	break;
	case SPY_STATE_SURVEILLANCE:
	{
		if (MOD_BALANCE_CORE_SPIES_ADVANCED && !bFirstTime)
			return GetSpyPower(pCity, iSpyIndex);

		return 1;
	}
	break;
	case SPY_STATE_GATHERING_INTEL:
	{
		if(pCity)
		{
			if (MOD_BALANCE_CORE_SPIES_ADVANCED)
				return GetSpyPower(pCity, iSpyIndex);

			//careful with overflow
			PlayerTypes eCityOwner = pCity->getOwner();
			int iBaseYieldRate = pCity->getYieldRateTimes100(YIELD_SCIENCE, false); //science is key
			iBaseYieldRate += pCity->getYieldRateTimes100(YIELD_PRODUCTION, false); //production too!
			iBaseYieldRate /= 100;
			iBaseYieldRate *= GC.getESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT();
			iBaseYieldRate /= 100;
			iBaseYieldRate *= GC.getGame().getGameSpeedInfo().getSpyRatePercent();
			iBaseYieldRate /= 100;

			int iCityEspionageModifier = pCity->GetEspionageModifier();
			int iPlayerEspionageModifier = GET_PLAYER(eCityOwner).GetEspionageModifier();
			int iTheirPoliciesEspionageModifier = GET_PLAYER(eCityOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_SLOWER_MODIFIER);
			int iMyPoliciesEspionageModifier = !bGlobalCheck ? m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER) : 0;
			int iFinalModifier = (iBaseYieldRate * (100 + iCityEspionageModifier + iPlayerEspionageModifier + iTheirPoliciesEspionageModifier + iMyPoliciesEspionageModifier)) / 100;

			int iResult = max(iFinalModifier, 1);
			if(iSpyIndex >= 0)
			{
				int iSpyRank = m_aSpyList[iSpyIndex].GetSpyRank(m_pPlayer->GetID()) + 1;
				iSpyRank += m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eCityOwner);
				if (MOD_BALANCE_CORE_SPIES_ADVANCED && GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
				{
					iSpyRank += 1;
				}
				iResult *= 100 + (GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT() * iSpyRank);
				iResult /= 100;
			}

			iResult *= GC.getGame().getCurrentEra() + 1;

			return iResult;
		}
	}
	break;
	case SPY_STATE_RIG_ELECTION:
	{
		if(pCity)
		{
			int iResult = 1;
			if(iSpyIndex >= 0)
			{
				int iSpyRank = m_aSpyList[iSpyIndex].GetSpyRank(m_pPlayer->GetID());
				iSpyRank += m_pPlayer->GetCulture()->GetInfluenceCityStateSpyRankBonus(pCity->getOwner());
				iResult = (iSpyRank + 1) * (iSpyRank + 1);
			}
			return iResult;
		}
	}
	break;
	case SPY_STATE_COUNTER_INTEL:
	{
		return 0;
	}
	break;
	case SPY_STATE_MAKING_INTRODUCTIONS:
	{
		return iBase;
	}
	break;
	case SPY_STATE_SCHMOOZE:
	{
		return 0;
	}
	break;
	}

	CvAssertMsg(false, "CalcPerTurn cannot handle that iSpyState");
	return -1;
}

/// CalcRequired - How much the spy is needed to do to accomplish this task
int CvPlayerEspionage::CalcRequired(int iSpyState, CvCity* pCity, int iSpyIndex, bool bGlobalCheck)
{
	DEBUG_VARIABLE(iSpyIndex);
	CvAssertMsg(pCity, "pCity is null");
	CvAssertMsg(iSpyIndex >= 0, "iSpyIndex is out of bounds");

	switch(iSpyState)
	{
	case SPY_STATE_TRAVELLING:
	{
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
		int iTravel = iSpyTurnsToTravel;
		if(MOD_BALANCE_CORE_SPIES_ADVANCED && pCity != NULL && (pCity->getOwner() != m_pPlayer->GetID()))
		{		
			if(m_pPlayer->GetProximityToPlayer(pCity->getOwner()) == PLAYER_PROXIMITY_NEIGHBORS)
			{
				iTravel += 1;
			}
			else if(m_pPlayer->GetProximityToPlayer(pCity->getOwner()) == PLAYER_PROXIMITY_CLOSE)
			{
				iTravel += 2;
			}
			else if(m_pPlayer->GetProximityToPlayer(pCity->getOwner()) == PLAYER_PROXIMITY_FAR)
			{
				iTravel += 3;
			}
			else if(m_pPlayer->GetProximityToPlayer(pCity->getOwner()) == PLAYER_PROXIMITY_DISTANT)
			{
				iTravel += 4;
			}
		}
		if(GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
		{
			iTravel /= 2;
			if(iTravel < 1)
			{
				iTravel = 1;
			}
		}
		return iTravel;
#else
		}
		return iSpyTurnsToTravel;
#endif
	}
	break;
	case SPY_STATE_SURVEILLANCE:
	{
		int iTime = 3;
		if (pCity)
		{
			iTime = m_pPlayer->GetCulture()->GetInfluenceSurveillanceTime(pCity->getOwner());
		}
		return iTime;
	}
	break;
	case SPY_STATE_GATHERING_INTEL:
	{
		if(pCity)
		{
			PlayerTypes ePlayer = pCity->getOwner();
			CvAssertMsg(!GET_PLAYER(ePlayer).isMinorCiv(), "Can't spy on a minor civ");
			CvAssertMsg(!GET_PLAYER(ePlayer).isBarbarian(), "Can't spy on barbarians");
			if(GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).isBarbarian())
			{
				return -1;
			}

			if (MOD_BALANCE_CORE_SPIES_ADVANCED)
			{
				CvEspionageSpy* pSpy = &(m_aSpyList[iSpyIndex]);
				return pCity->GetEspionageRankingForEspionage(m_pPlayer->GetID(), pSpy->m_eSpyFocus);
			}

			int iMaxTechCost = 0;
			
			for (size_t i = 0; i < m_aaPlayerStealableTechList[ePlayer].size(); i++)
			{
				CvTechEntry* pkTechInfo = GC.getTechInfo(m_aaPlayerStealableTechList[ePlayer][i]);
				if (pkTechInfo)
				{
					int iTechCost = pkTechInfo->GetResearchCost();

					iTechCost *= (GC.getMap().getWorldInfo().getResearchPercent());
					iTechCost /= 100;
					iTechCost *= (GC.getGame().getGameSpeedInfo().getResearchPercent());
					iTechCost /= 100;
					iTechCost *= (GC.getGame().getStartEraInfo().getResearchPercent());
					iTechCost /= 100;

					iTechCost *= 100 + (GC.getGame().getCurrentEra() * 5);
					iTechCost /= 100;

					//not being able to counterspy is lame.
					if (!bGlobalCheck && GET_PLAYER(ePlayer).GetEspionage()->GetNumSpies() <= 0)
					{
						iTechCost *= 150;
						iTechCost /= 100;
					}

					iMaxTechCost = max(iMaxTechCost, iTechCost);
				}
			}

			if (GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
			{
				iMaxTechCost *= 100;
				iMaxTechCost /= 150;
			}

			iMaxTechCost *= GC.getESPIONAGE_GATHERING_INTEL_COST_PERCENT();
			iMaxTechCost /= 100;

			return iMaxTechCost;
		}
	}
	break;
	case SPY_STATE_RIG_ELECTION:
	{
		return GC.getGame().GetTurnsUntilMinorCivElection();
	}
	break;
	case SPY_STATE_MAKING_INTRODUCTIONS:
	{
		return (iSpyTurnsToMakeIntroductions * GC.getGame().getGameSpeedInfo().getLeaguePercent()) / 100;
	}
	break;
	}

	CvAssertMsg(false, "CalcRequired cannot handle that iSpyState");
	return -1;
}


/// GetSpyRankName - What's the textkey of the rank for a spy
const char* CvPlayerEspionage::GetSpyRankName(int iRank) const
{
	switch(iRank)
	{
	case SPY_RANK_RECRUIT:
		return "TXT_KEY_SPY_RANK_0";
		break;
	case SPY_RANK_AGENT:
		return "TXT_KEY_SPY_RANK_1";
		break;
	case SPY_RANK_SPECIAL_AGENT:
		return "TXT_KEY_SPY_RANK_2";
		break;
	}

	return "";
}

/// HasEstablishedSurveillance - Has the spy established surveillance in their city
bool CvPlayerEspionage::HasEstablishedSurveillance(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	if(!IsSpyInCity(uiSpyIndex))
	{
		return false;
	}

	CvCity* pCity = GetCityWithSpy(uiSpyIndex);
	if(pCity == NULL)
		return false;

	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	if(m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SURVEILLANCE && pCityEspionage->HasReachedGoal(m_pPlayer->GetID()))
	{
		return true;
	}
#if defined(MOD_BALANCE_CORE)
	else if (m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_RIG_ELECTION || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE)
#else
	else if(m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_RIG_ELECTION || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE)
#endif
	{
		return true;
	}

	return false;
}

/// HasEstablishedSurveillanceInCity - Has any spy established espionage in this city?
bool CvPlayerEspionage::HasEstablishedSurveillanceInCity(CvCity* pCity)
{
	CvAssertMsg(pCity, "pCity is null");
	if(!pCity)
	{
		return false;
	}

	int iX = pCity->getX();
	int iY = pCity->getY();

	for(uint uiSpy = 0; uiSpy < m_aSpyList.size(); uiSpy++)
	{
		if(m_aSpyList[uiSpy].m_iCityX == iX && m_aSpyList[uiSpy].m_iCityY == iY && HasEstablishedSurveillance(uiSpy))
		{
			return true;
		}
	}

	return false;
}

/// IsAnySurveillanceEstablished - Has any spy established surveillance in this civilization?
bool CvPlayerEspionage::IsAnySurveillanceEstablished(PlayerTypes eTargetPlayer)
{
	for(uint uiSpy = 0; uiSpy < m_aSpyList.size(); uiSpy++)
	{
		// can't have established surveillance when not in a city
		if(!IsSpyInCity(uiSpy))
		{
			continue;
		}

		CvPlot* pPlot = GC.getMap().plot(m_aSpyList[uiSpy].m_iCityX, m_aSpyList[uiSpy].m_iCityY);
		CvAssertMsg(pPlot, "Spy coordinates did not point to plot");
		if(!pPlot)
		{
			return false;
		}
		CvCity* pCity = pPlot->getPlotCity();
		CvAssertMsg(pCity, "There is not a city at the specified plot");
		if(!pCity)
		{
			return false;
		}
		if(HasEstablishedSurveillance(uiSpy) && pCity->getOwner() == eTargetPlayer)
		{
			return true;
		}
	}

	return false;
}

bool CvPlayerEspionage::IsDiplomat (uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	return m_aSpyList[uiSpyIndex].m_bIsDiplomat;
}

bool CvPlayerEspionage::IsSchmoozing (uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	if (m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CvPlayerEspionage::IsAnySchmoozing (CvCity* pCity)
{
	for(uint uiSpy = 0; uiSpy < m_aSpyList.size(); uiSpy++)
	{
		// can't have established surveillance when not in a city
		if(!IsSpyInCity(uiSpy))
		{
			continue;
		}

		CvPlot* pPlot = GC.getMap().plot(m_aSpyList[uiSpy].m_iCityX, m_aSpyList[uiSpy].m_iCityY);
		CvAssertMsg(pPlot, "Spy coordinates did not point to plot");
		if(!pPlot)
		{
			return false;
		}
		CvCity* pPlotCity = pPlot->getPlotCity();
		CvAssertMsg(pPlotCity, "There is not a city at the specified plot");
		if(!pPlotCity)
		{
			return false;
		}
		if(pCity == pPlotCity && IsSchmoozing(uiSpy))
		{
			return true;
		}
	}

	return false;
}
/// CanStageCoup - Can a spy currently stage a coup at a city-state?
bool CvPlayerEspionage::CanStageCoup(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	if(!IsSpyInCity(uiSpyIndex))
	{
		return false;
	}

	CvCity* pCity = GetCityWithSpy(uiSpyIndex);
	CvAssertMsg(pCity, "pCity is null!");
	if(!pCity)
	{
		return false;
	}

	if(!HasEstablishedSurveillance(uiSpyIndex))
	{
		return false;
	}

	PlayerTypes eCityOwner = pCity->getOwner();
	if(!GET_PLAYER(eCityOwner).isMinorCiv())
	{
		return false;
	}

	CvMinorCivAI* pMinorCivAI = GET_PLAYER(eCityOwner).GetMinorCivAI();
	PlayerTypes eMinorCivAlly = pMinorCivAI->GetAlly();
#if defined(MOD_BALANCE_CORE)
	if(pMinorCivAI->GetPermanentAlly() == eMinorCivAlly)
	{
		return false;
	}
	if(pMinorCivAI->GetCoupCooldown() > 0)
	{
		return false;
	}
#endif

	if(eMinorCivAlly != NO_PLAYER && eMinorCivAlly != m_pPlayer->GetID())
	{
		return true;
	}

#if defined(MOD_EVENTS_ESPIONAGE)
	if (MOD_EVENTS_ESPIONAGE) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_EspionageCanStageCoup, m_pPlayer->GetID(), eCityOwner, pCity->GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif
	
	return false;
}
bool CvPlayerEspionage::CanStageCoup(CvCity* pCity)
{
	PlayerTypes eCityOwner = pCity->getOwner();
	if (!GET_PLAYER(eCityOwner).isMinorCiv())
	{
		return false;
	}

	CvMinorCivAI* pMinorCivAI = GET_PLAYER(eCityOwner).GetMinorCivAI();
	PlayerTypes eMinorCivAlly = pMinorCivAI->GetAlly();
#if defined(MOD_BALANCE_CORE)
	if (pMinorCivAI->GetPermanentAlly() == eMinorCivAlly)
	{
		return false;
	}
	if (pMinorCivAI->GetCoupCooldown() > 0)
	{
		return false;
	}
#endif

	if (eMinorCivAlly != NO_PLAYER && eMinorCivAlly != m_pPlayer->GetID())
	{
		return true;
	}

#if defined(MOD_EVENTS_ESPIONAGE)
	if (MOD_EVENTS_ESPIONAGE) {
		if (GAMEEVENTINVOKE_TESTALL(GAMEEVENT_EspionageCanStageCoup, m_pPlayer->GetID(), eCityOwner, pCity->GetID()) == GAMEEVENTRETURN_FALSE) {
			return false;
		}
	}
#endif

	return false;
}

/// GetCoupChangeOfSuccess - What is the % chance of success that a spy will be able to pull off a coup?
int CvPlayerEspionage::GetCoupChanceOfSuccess(uint uiSpyIndex)
{
	// if you can't stage a coup, then the likelihood is zero!
	if(!CanStageCoup(uiSpyIndex))
	{
		return 0;
	}

	CvCity* pCity = GetCityWithSpy(uiSpyIndex);
	CvAssertMsg(pCity, "No city");
	if(!pCity)
	{
		return 0;
	}

	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	CvAssertMsg(pCityEspionage, "No city espionage");
	if(!pCityEspionage)
	{
		return 0;
	}

	PlayerTypes eCityOwner = pCity->getOwner();
	CvAssertMsg(GET_PLAYER(eCityOwner).isMinorCiv(), "Owner is not a minor civ");
	if(!GET_PLAYER(eCityOwner).isMinorCiv())
	{
		return 0;
	}

	CvMinorCivAI* pMinorCivAI = GET_PLAYER(eCityOwner).GetMinorCivAI();
	CvAssertMsg(pMinorCivAI, "pMinorCivAI is null");
	if(!pMinorCivAI)
	{
		return 0;
	}

	PlayerTypes eAllyPlayer = pMinorCivAI->GetAlly();
	int iAllySpyRank = 0;
	bool bNoAllySpy = true;
	if(pCityEspionage->m_aiSpyAssignment[eAllyPlayer] != -1)
	{
		int iAllySpyIndex = pCityEspionage->m_aiSpyAssignment[eAllyPlayer];
		iAllySpyRank = GET_PLAYER(eAllyPlayer).GetEspionage()->m_aSpyList[iAllySpyIndex].GetSpyRank(eAllyPlayer);
		bNoAllySpy = false;
	}

	int iAllyInfluence = pMinorCivAI->GetEffectiveFriendshipWithMajor(eAllyPlayer);
	int iMyInfluence = max(0, pMinorCivAI->GetEffectiveFriendshipWithMajor(m_pPlayer->GetID()));

	int iMaxChance = 100;

	iMaxChance -= iAllyInfluence / 10;
	
	iMaxChance += iMyInfluence / 10;

	if (iMaxChance <= 5)
		iMaxChance = 5;
	else if (iMaxChance >= 25)
		iMaxChance = 25;

	int iMySpyRank = m_aSpyList[uiSpyIndex].GetSpyRank(m_pPlayer->GetID());
	iMySpyRank += m_pPlayer->GetCulture()->GetInfluenceCityStateSpyRankBonus(eCityOwner);
	iMySpyRank -= iAllySpyRank;
	
	iMaxChance *= 100 + (iMySpyRank * (int)GC.getESPIONAGE_COUP_MULTIPLY_CONSTANT());
	iMaxChance /= 100;

	iMaxChance *= (100 + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER));
	iMaxChance /= 100;

	return range(iMaxChance, 10, 90);
}

int CvPlayerEspionage::GetTheoreticalChanceOfCoup(CvCity* pCity)
{
	// if you can't stage a coup, then the likelihood is zero!
	if (!CanStageCoup(pCity))
	{
		return 0;
	}

	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	CvAssertMsg(pCityEspionage, "No city espionage");
	if (!pCityEspionage)
	{
		return 0;
	}

	PlayerTypes eCityOwner = pCity->getOwner();
	CvAssertMsg(GET_PLAYER(eCityOwner).isMinorCiv(), "Owner is not a minor civ");
	if (!GET_PLAYER(eCityOwner).isMinorCiv())
	{
		return 0;
	}

	CvMinorCivAI* pMinorCivAI = GET_PLAYER(eCityOwner).GetMinorCivAI();
	CvAssertMsg(pMinorCivAI, "pMinorCivAI is null");
	if (!pMinorCivAI)
	{
		return 0;
	}

	PlayerTypes eAllyPlayer = pMinorCivAI->GetAlly();
	int iAllySpyRank = 0;
	bool bNoAllySpy = true;
	if (pCityEspionage->m_aiSpyAssignment[eAllyPlayer] != -1)
	{
		int iAllySpyIndex = pCityEspionage->m_aiSpyAssignment[eAllyPlayer];
		iAllySpyRank = GET_PLAYER(eAllyPlayer).GetEspionage()->m_aSpyList[iAllySpyIndex].GetSpyRank(eAllyPlayer);
		bNoAllySpy = false;
	}

	int iAllyInfluence = pMinorCivAI->GetEffectiveFriendshipWithMajor(eAllyPlayer);
	int iMyInfluence = max(0, pMinorCivAI->GetEffectiveFriendshipWithMajor(m_pPlayer->GetID()));

	int iMaxChance = 100;

	iMaxChance -= iAllyInfluence / 10;

	iMaxChance += iMyInfluence / 10;

	if (iMaxChance <= 5)
		iMaxChance = 5;
	else if (iMaxChance >= 25)
		iMaxChance = 25;

	int iMySpyRank = 1;
	iMySpyRank += m_pPlayer->GetCulture()->GetInfluenceCityStateSpyRankBonus(eCityOwner);
	iMySpyRank -= iAllySpyRank;

	iMaxChance *= 100 + (iMySpyRank * (int)GC.getESPIONAGE_COUP_MULTIPLY_CONSTANT());
	iMaxChance /= 100;

	iMaxChance *= (100 + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER));
	iMaxChance /= 100;

	return range(iMaxChance, 10, 90);
}

/// AttemptCoup - Have a spy try to overthrow a city state. If success, the spy's owner becomes the ally. If failure, the spy dies.
bool CvPlayerEspionage::AttemptCoup(uint uiSpyIndex)
{
	// if you're not allowed to stage a coup here, the coup fails
	if(!CanStageCoup(uiSpyIndex))
	{
		return false;
	}

	CvCity* pCity = GetCityWithSpy(uiSpyIndex);
	CvAssertMsg(pCity, "Spy isn't in a city.");
	if(!pCity)
	{
		return false;
	}

	PlayerTypes eCityOwner = pCity->getOwner();
	if(!GET_PLAYER(eCityOwner).isMinorCiv())
	{
		// this city state is not a minor civ
		return false;
	}

	CvMinorCivAI* pMinorCivAI = GET_PLAYER(eCityOwner).GetMinorCivAI();

	PlayerTypes ePreviousAlly = pMinorCivAI->GetAlly();
	CvAssertMsg(ePreviousAlly != NO_PLAYER, "City state has no ally. Something's going wrong");
	if(ePreviousAlly == NO_PLAYER)
	{
		return false;
	}

	int aiNewInfluenceValueTimes100[MAX_MAJOR_CIVS];
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		aiNewInfluenceValueTimes100[ui] = pMinorCivAI->GetEffectiveFriendshipWithMajorTimes100((PlayerTypes)ui);
	}

	m_aSpyList[uiSpyIndex].m_bEvaluateReassignment = true; // flag for reassignment
	if(GC.getLogging())
	{
		CvString strMsg;
		strMsg.Format("Re-eval: attempting coup, %d,", uiSpyIndex);
		strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
		LogEspionageMsg(strMsg);
	}

	bool bAttemptSuccess = false;
	int iRandRoll = GC.getGame().getSmallFakeRandNum(100, *pCity->plot());
	if(iRandRoll <= GetCoupChanceOfSuccess(uiSpyIndex))
	{
		// swap influence from ally to 2nd place ally
		int iInfluenceTemp = aiNewInfluenceValueTimes100[ePreviousAlly];
		aiNewInfluenceValueTimes100[ePreviousAlly] = aiNewInfluenceValueTimes100[m_pPlayer->GetID()];
		aiNewInfluenceValueTimes100[m_pPlayer->GetID()] = iInfluenceTemp;

		// reduce the influence of all the other players
		for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes) ui;

			if (eLoopPlayer == m_pPlayer->GetID())
			{
				continue;
			}

			// only drop the influence if they have positive influence
			if (aiNewInfluenceValueTimes100[ui] > 0)
			{
				int iNewInfluence = aiNewInfluenceValueTimes100[ui] - (GC.getESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP() * 100);
				iNewInfluence = max(iNewInfluence, 0);
				aiNewInfluenceValueTimes100[ui] = iNewInfluence;

				// Update diplomacy
				if (eLoopPlayer == ePreviousAlly)
				{
					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesPerformedCoupAgainstUs(m_pPlayer->GetID(), 1);
					GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyLoweredOurInfluence(m_pPlayer->GetID(), 2);
				}
				else
				{
					// Don't apply the diplo penalty if this player hates the previous ally (or they're at war).
					if (!GET_PLAYER(eLoopPlayer).IsAtWarWith(ePreviousAlly) && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetCivOpinion(ePreviousAlly) != CIV_OPINION_UNFORGIVABLE
						&& !GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->WasEverBackstabbedBy(ePreviousAlly) && !GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsUntrustworthy(ePreviousAlly))
					{
						GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->ChangeNumTimesTheyLoweredOurInfluence(m_pPlayer->GetID(), 1);
					}
				}
			}
		}

		bAttemptSuccess = true;
	}
	else
	{
		// reduce influence of player
		// right now move the influence into a negative space
		aiNewInfluenceValueTimes100[m_pPlayer->GetID()] = (-10 * 100);
		bAttemptSuccess = false;

		// kill the spy
		ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
#if defined(MOD_API_ESPIONAGE)
		m_aSpyList[uiSpyIndex].SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_DEAD); // have to official kill him after the extraction
#else
		m_aSpyList[uiSpyIndex].m_eSpyState = SPY_STATE_DEAD; // have to official kill him after the extraction
#endif
	}
#if defined(MOD_BALANCE_CORE)
	int iCoolDown = 20;
	iCoolDown *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iCoolDown /= 100;
	pMinorCivAI->SetCoupCooldown(iCoolDown);
#endif
	// do others influence first so that the potential coup person will be the ally
	pMinorCivAI->SetDisableNotifications(true);
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if(!GET_PLAYER(ePlayer).isAlive())
		{
			continue;
		}

		// skip the spy player
		if(ePlayer == m_pPlayer->GetID())
		{
			continue;
		}

		// send notification to other civs if they have met this minor
		bool bMetMinor = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eCityOwner).getTeam());
		bool bNotify = false;
		int iFriendship = pMinorCivAI->GetEffectiveFriendshipWithMajor(ePlayer);
		int iRelationshipAnchor = pMinorCivAI->GetFriendshipAnchorWithMajor(ePlayer);
		bool bFriends = pMinorCivAI->IsFriends(ePlayer);

		if (iFriendship > iRelationshipAnchor || bFriends)
		{
			bNotify = true;
		}

		CvPlayerEspionage* pOtherEspionage = GET_PLAYER(ePlayer).GetEspionage();
		int iSpyIndex = pOtherEspionage->GetSpyIndexInCity(pCity);
		if (iSpyIndex >= 0)
		{
			bNotify = true;
		}

		pMinorCivAI->SetFriendshipWithMajorTimes100(ePlayer, aiNewInfluenceValueTimes100[ui]);

		CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
		if(pNotifications && bMetMinor && bNotify)
		{
			NotificationTypes eNotification;
			Localization::String strSummary;
			Localization::String strNotification;
			if(bAttemptSuccess)
			{
				eNotification = NOTIFICATION_SPY_STAGE_COUP_SUCCESS;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_SUCCESS_S");
				strSummary << m_pPlayer->getCivilizationAdjectiveKey();
				strSummary << pCity->getNameKey();
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_SUCCESS");
				strNotification << m_pPlayer->getCivilizationShortDescriptionKey();
				strNotification << pCity->getNameKey();
				strNotification << GET_PLAYER(ePreviousAlly).getCivilizationAdjectiveKey();
			}
			else
			{
				eNotification = NOTIFICATION_SPY_STAGE_COUP_FAILURE;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_FAILURE_S");
				strSummary << m_pPlayer->getCivilizationAdjectiveKey();
				strSummary << pCity->getNameKey();
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_FAILURE");
				strNotification << m_pPlayer->getCivilizationShortDescriptionKey();
				strNotification << pCity->getNameKey();
				strNotification << GET_PLAYER(ePreviousAlly).getCivilizationAdjectiveKey();
			}
			pNotifications->Add(eNotification, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
		}
	}

	if (bAttemptSuccess)
	{
		if (GET_TEAM(GET_PLAYER(eCityOwner).getTeam()).isAtWar(m_pPlayer->getTeam()))
			GET_TEAM(GET_PLAYER(eCityOwner).getTeam()).makePeace(m_pPlayer->getTeam(), false);
	}

	pMinorCivAI->SetFriendshipWithMajorTimes100(m_pPlayer->GetID(), aiNewInfluenceValueTimes100[m_pPlayer->GetID()]);

	pMinorCivAI->SetDisableNotifications(false);
	// send notification to player
	CvNotifications* pNotifications = m_pPlayer->GetNotifications();
	if(pNotifications)
	{
		NotificationTypes eNotification;
		Localization::String strSummary;
		Localization::String strNotification;
		if(bAttemptSuccess)
		{
			eNotification = NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS_S");
			strSummary << pCity->getNameKey();
			strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS");
			strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
			strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
			strNotification << pCity->getNameKey();
			strNotification << GET_PLAYER(ePreviousAlly).getCivilizationAdjectiveKey();
		}
		else
		{
			eNotification = NOTIFICATION_SPY_YOU_STAGE_COUP_FAILURE;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_YOU_STAGE_COUP_FAILURE_S");
			strSummary << pCity->getNameKey();
			strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_YOU_STAGE_COUP_FAILURE");
			strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
			strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
			strNotification << pCity->getNameKey();
			strNotification << GET_PLAYER(ePreviousAlly).getCivilizationAdjectiveKey();
		}
		pNotifications->Add(eNotification, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
	}

#if !defined(NO_ACHIEVEMENTS)
	//Achievements!
	if(bAttemptSuccess && m_pPlayer->GetID() == GC.getGame().getActivePlayer())
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_XP1_13);
	}
#endif

	if (bAttemptSuccess)
	{
		m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_SPY_ATTACK, false, NO_GREATPERSON, NO_BUILDING, 1);
		pMinorCivAI->SetCoupAttempted(m_pPlayer->GetID(), true);
	}

	// Update City banners and game info
	GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);

	return bAttemptSuccess;
}

/// GetTurnsUntilStateComplete - How many turns until this spy's current state is resolved?
int CvPlayerEspionage::GetTurnsUntilStateComplete(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return -2;
	}

	PlayerTypes ePlayer = m_pPlayer->GetID();
	CvCity* pCity;
	CvCityEspionage* pCityEspionage;
	int iAmountLeft = 0;
	int iTurnsLeft = 0;

	switch(m_aSpyList[uiSpyIndex].m_eSpyState)
	{
	case SPY_STATE_UNASSIGNED:
		// no end time
		return -1;
		break;
	case SPY_STATE_TRAVELLING:
	case SPY_STATE_SURVEILLANCE:
	case SPY_STATE_GATHERING_INTEL:
	case SPY_STATE_MAKING_INTRODUCTIONS:
		pCity = GetCityWithSpy(uiSpyIndex);
		CvAssertMsg(pCity, "GetCityWithSpy returned null. Has the wrong task");
		if(!pCity)
		{
			return -1;
		}
		pCityEspionage = pCity->GetCityEspionage();
		iAmountLeft = pCityEspionage->m_aiGoal[ePlayer] - pCityEspionage->m_aiAmount[ePlayer];
		if(pCityEspionage->m_aiRate[ePlayer] != 0)
		{
			iTurnsLeft = iAmountLeft / pCityEspionage->m_aiRate[ePlayer];
			if(iAmountLeft % pCityEspionage->m_aiRate[ePlayer] > 0)
			{
				iTurnsLeft++;
			}
		}
		return iTurnsLeft;
	case SPY_STATE_RIG_ELECTION:
		return GC.getGame().GetTurnsUntilMinorCivElection();
		break;
	case SPY_STATE_COUNTER_INTEL:
	case SPY_STATE_SCHMOOZE:
		// no end time
		return -1;
		break;
	case SPY_STATE_DEAD:
		// no end time
		return -1;
		break;
#if defined(MOD_API_ESPIONAGE)
	case SPY_STATE_TERMINATED:
		// no end time
		return -1;
		break;
#endif
	}

	return -1;
}

/// GetPercentOfStateComplete - What percentage of this spy's current state is complete?
int CvPlayerEspionage::GetPercentOfStateComplete(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return -2;
	}

	PlayerTypes ePlayer = m_pPlayer->GetID();
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;

	switch(m_aSpyList[uiSpyIndex].m_eSpyState)
	{
	case SPY_STATE_UNASSIGNED:
		// no end time
		return -1;
		break;
	case SPY_STATE_TRAVELLING:
	case SPY_STATE_SURVEILLANCE:
	case SPY_STATE_GATHERING_INTEL:
	case SPY_STATE_MAKING_INTRODUCTIONS:
		pCity = GetCityWithSpy(uiSpyIndex);
		if (pCity)
		{
			pCityEspionage = pCity->GetCityEspionage();
			CvAssertMsg(pCityEspionage->m_aiGoal[ePlayer] != 0, "Trying to div by zero!");
			if(pCityEspionage->m_aiGoal[ePlayer] != 0)
			{
				return (pCityEspionage->m_aiAmount[ePlayer] * 100) / pCityEspionage->m_aiGoal[ePlayer];
			}
		}
		return -1;
	case SPY_STATE_RIG_ELECTION:
		if(GC.getGame().GetTurnsBetweenMinorCivElections() != 0)
		{
			return ((GC.getGame().GetTurnsBetweenMinorCivElections() - GC.getGame().GetTurnsUntilMinorCivElection()) * 100) / GC.getGame().GetTurnsBetweenMinorCivElections();
		}
		else
		{
			return -1;
		}
		break;
	case SPY_STATE_COUNTER_INTEL:
	case SPY_STATE_SCHMOOZE:
		// no end time
		return -1;
		break;
	case SPY_STATE_DEAD:
		// no end time
		return -1;
		break;
#if defined(MOD_API_ESPIONAGE)
	case SPY_STATE_TERMINATED:
		// no end time
		return -1;
		break;
#endif
	}

	return -1;
}


/// GetNumSpies - Number of spies the player has earned
int CvPlayerEspionage::GetNumSpies(void)
{
	return m_aSpyList.size();
}

/// GetNumAliveSpies - Number of spies the player has that are alive
int CvPlayerEspionage::GetNumAliveSpies(void)
{
	int iCount = 0;
	for(uint ui = 0; ui < m_aSpyList.size(); ui++)
	{
#if defined(MOD_API_ESPIONAGE)
		if(!(m_aSpyList[ui].m_eSpyState == SPY_STATE_DEAD || m_aSpyList[ui].m_eSpyState == SPY_STATE_TERMINATED))
#else
		if(m_aSpyList[ui].m_eSpyState != SPY_STATE_DEAD)
#endif
		{
			iCount++;
		}
	}

	return iCount;
}

/// GetNumAssignedSpies - Number of spies that the player has assigned to tasks
int CvPlayerEspionage::GetNumAssignedSpies(void)
{
	int iCount = 0;
	for(uint ui = 0; ui < m_aSpyList.size(); ui++)
	{
#if defined(MOD_API_ESPIONAGE)
		if(m_aSpyList[ui].m_eSpyState == SPY_STATE_DEAD || m_aSpyList[ui].m_eSpyState == SPY_STATE_TERMINATED)
#else
		if(m_aSpyList[ui].m_eSpyState == SPY_STATE_DEAD)
#endif
		{
			continue;
		}

		if(IsSpyInCity(ui))
		{
			iCount++;
		}
	}

	return iCount;
}

/// GetNumUnassignedSpies - Number of spies that are alive but unassigned to tasks
int CvPlayerEspionage::GetNumUnassignedSpies(void)
{
	return GetNumAliveSpies() - GetNumAssignedSpies();
}

/// BuildStealableTechList - Go through opponents list and see what techs you can steal from them.
void CvPlayerEspionage::BuildStealableTechList(PlayerTypes ePlayer)
{
	CvAssertMsg((uint)ePlayer < m_aaPlayerStealableTechList.size(), "ePlayer out of bounds");
	if((uint)ePlayer >= m_aaPlayerStealableTechList.size())
	{
		return;
	}

	m_aaPlayerStealableTechList[ePlayer].clear();

	if(!GET_PLAYER(ePlayer).isAlive())
	{
		return;
	}

	CvPlayerTechs* pMyPlayerTechs = m_pPlayer->GetPlayerTechs();
	CvPlayerTechs* pOtherPlayerTechs = GET_PLAYER(ePlayer).GetPlayerTechs();
	for(int iTechLoop = 0; iTechLoop < pOtherPlayerTechs->GetTechs()->GetNumTechs(); iTechLoop++)
	{
		TechTypes eTech = (TechTypes)iTechLoop;

		// Does the other player already have this tech?
		if(!GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->HasTech(eTech))
		{
			continue;
		}

		// can we not research this tech?
		if(!pMyPlayerTechs->CanResearch(eTech))
		{
			continue;
		}

		// add to list!
		m_aaPlayerStealableTechList[ePlayer].push_back(eTech);
		}
	}

/// IsTechStealable - Check to see if you can steal this tech from an opponent
bool CvPlayerEspionage::IsTechStealable(PlayerTypes ePlayer, TechTypes eTech)
{
	CvAssertMsg((uint)ePlayer < m_aaPlayerStealableTechList.size(), "ePlayer out of bounds");
	if((uint)ePlayer >= m_aaPlayerStealableTechList.size())
	{
		return false;
	}

	for(uint ui = 0; ui < m_aaPlayerStealableTechList[ePlayer].size(); ui++)
	{
		if(m_aaPlayerStealableTechList[ePlayer][ui] == eTech)
		{
			return true;
		}
	}

	return false;
}

/// GetNumTechsToSteal - How many techs you can steal from a given player
int CvPlayerEspionage::GetNumTechsToSteal(PlayerTypes ePlayer)
{
	CvAssertMsg((uint)ePlayer < m_aiNumTechsToStealList.size(), "ePlayer out of bounds");
	if((uint)ePlayer >= m_aiNumTechsToStealList.size())
	{
		return -1;
	}

	return m_aiNumTechsToStealList[ePlayer];
}
int CvPlayerEspionage::GetNumSpyActionsDone(PlayerTypes ePlayer)
{
	CvAssertMsg((uint)ePlayer < m_aiNumSpyActionsDone.size(), "ePlayer out of bounds");
	if((uint)ePlayer >= m_aiNumSpyActionsDone.size())
	{
		return -1;
	}

	return m_aiNumSpyActionsDone[ePlayer];
}

bool CvPlayerEspionage::IsMyDiplomatVisitingThem(PlayerTypes ePlayer, bool bIncludeTravelling)
{
	CvCity* pTheirCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if (!pTheirCapital)
	{
		return false;
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	// They are our vassal, so yes, we have a diplomat already
	if(MOD_DIPLOMACY_CIV4_FEATURES && GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetMaster() == m_pPlayer->getTeam())
	{
		return true;
	}
#endif

	int iSpyIndex = GetSpyIndexInCity(pTheirCapital);

	if (iSpyIndex < 0)
	{
		return false;
	}
	if (!m_aSpyList[iSpyIndex].m_bIsDiplomat)
	{
		return false;
	}

	if (!bIncludeTravelling)
	{
		// If the spy is not schmoozing yet, then it doesn't count as a diplomat yet
		if (!IsSchmoozing(iSpyIndex))
		{
			return false;
		}
	}

	return true;
}

bool CvPlayerEspionage::IsOtherDiplomatVisitingMe(PlayerTypes ePlayer)
{
	return GET_PLAYER(ePlayer).GetEspionage()->IsMyDiplomatVisitingThem(m_pPlayer->GetID());
}

/// AddMessage - This function is called by another player's PlayerEspionage class. It records the spy activity to be played back at the beginning
///              of the turn. This prevents notifications from coming in during someone's turn, allowing players to game the system by watching when
///              the messages come in to determine which player did the spying.
void CvPlayerEspionage::AddSpyMessage(int iCityX, int iCityY, PlayerTypes eAttackingPlayer, int iSpyResult, TechTypes eStolenTech, int iGreatWorkIndex)
{
	// go through all messages to see if this is an update
	for(uint ui = 0; ui < m_aSpyNotificationMessages.size(); ui++)
	{
		if(m_aSpyNotificationMessages[ui].m_iCityX == iCityX && m_aSpyNotificationMessages[ui].m_iCityY == iCityY && m_aSpyNotificationMessages[ui].m_eAttackingPlayer == eAttackingPlayer)
		{
			m_aSpyNotificationMessages[ui].m_iSpyResult = iSpyResult;
			m_aSpyNotificationMessages[ui].m_eStolenTech = eStolenTech;
			// found the appropriate message, now leaving the function
			return;
		}
	}

	// no matching message found so adding it to the back of the list
	SpyNotificationMessage kMessage;
	kMessage.m_iCityX = iCityX;
	kMessage.m_iCityY = iCityY;
	kMessage.m_eAttackingPlayer = eAttackingPlayer;
	kMessage.m_iSpyResult = iSpyResult;
	kMessage.m_eStolenTech = eStolenTech;

#if defined(MOD_EVENTS_ESPIONAGE)
	if (MOD_EVENTS_ESPIONAGE) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_EspionageNotificationData, iCityX, iCityY, eAttackingPlayer, m_pPlayer->GetID(), iSpyResult, eStolenTech, iGreatWorkIndex);
	}
#endif

	m_aSpyNotificationMessages.push_back(kMessage);
}

/// ProcessSpyMessages - This is called at the beginning of the player's turn and takes the messages queued up and sends them to the notification system. It then clears the list.
void CvPlayerEspionage::ProcessSpyMessages()
{
	CvNotifications* pNotifications = m_pPlayer->GetNotifications();
	if(!pNotifications)
	{
		m_aSpyNotificationMessages.clear();
		return;
	}

	bool bMultiplayer = GC.getGame().countHumanPlayersEverAlive() > 1;
	CvEspionageAI* pEspionageAI = m_pPlayer->GetEspionageAI();

	for(uint ui = 0; ui < m_aSpyNotificationMessages.size(); ui++)
	{
		CvPlot* pPlot = GC.getMap().plot(m_aSpyNotificationMessages[ui].m_iCityX, m_aSpyNotificationMessages[ui].m_iCityY);
		CvCity* pCity = NULL;
		CvCityEspionage* pCityEspionage = NULL;
		CvAssertMsg(pPlot, "Could not find plot location for SpyNotificationMessages");
		if(pPlot)
		{
			pCity = pPlot->getPlotCity();
			CvAssertMsg(pCity, "Plot does not contain a city");
			if(pCity)
			{
				CvAssertMsg(pCity->getOwner() == m_pPlayer->GetID(), "City does not belong to player. This is very strange");
				pCityEspionage = pCity->GetCityEspionage();
			}
		}

		if(!pCity || !pCityEspionage)
		{
			continue;
		}

		CvTechEntry* pTechEntry = NULL;
		if(m_aSpyNotificationMessages[ui].m_eStolenTech != NO_TECH)
		{
			pTechEntry = GC.getTechInfo(m_aSpyNotificationMessages[ui].m_eStolenTech);
		}
		if(bMultiplayer || !pTechEntry)
		{
			switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
			{
			case SPY_RESULT_UNDETECTED:
				// do nothing
				break;
			case SPY_RESULT_DETECTED:
				// notify defending player that a spy of unknown origin stole something
			{
				Localization::String strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_DETECTED_WO_TECH_S");
				Localization::String strNotification;

				if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_DETECTED_WO_TECH_W_TIP");
				}
				else
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_DETECTED_WO_TECH");
				}
				strNotification << pCity->getNameKey();
				pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
			break;
			case SPY_RESULT_IDENTIFIED:
				// notify defending player that a spy of known origin stole something
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED_WO_TECH_S");
				strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
				Localization::String strNotification;

				if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED_WO_TECH_W_TIP");
				}
				else
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED_WO_TECH");
				}
				strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
				strNotification << pCity->getNameKey();
				pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
			}
			break;
			case SPY_RESULT_KILLED:
				// notify defending player that they killed a spy
			{
				int iDefendingSpy = pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()];
				CvAssertMsg(iDefendingSpy >= 0, "iDefendingSpy value is out of bounds");
				if(iDefendingSpy >= 0)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY_S");
					
					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
#if defined(MOD_BALANCE_CORE_SPIES)
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY_ADVANCED");
#else
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
#endif
					strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
					strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
					strNotification << pCity->getNameKey();

					pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
				
#if !defined(NO_ACHIEVEMENTS)
					//Achievements
					if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_15);
					}
#endif
				}
			}
			break;
			}
		}
#if defined(MOD_BALANCE_CORE_SPIES)
		else if(pTechEntry)
#else
		else
#endif
		{
			switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
			{
			case SPY_RESULT_UNDETECTED:
				// nothing happens, nobody is notified
				break;
			case SPY_RESULT_DETECTED:
			{
				// notify defending player that a spy of unknown origin stole a tech
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_DETECTED_S");
				strSummary << pTechEntry->GetDescriptionKey();

				Localization::String strNotification;
				if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_DETECTED_W_TIP");
				}
				else // defensive spy
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_DETECTED");
				}
				strNotification << pTechEntry->GetDescriptionKey();
				strNotification << pCity->getNameKey();
				pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
			break;
			case SPY_RESULT_IDENTIFIED:
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED_S");
				strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
				strSummary << pTechEntry->GetDescriptionKey();

				Localization::String strNotification;
				if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED_W_TIP");
				}
				else
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED");
				}
				strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
				strNotification << pTechEntry->GetDescriptionKey();
				strNotification << pCity->getNameKey();
				pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

				pEspionageAI->m_aiNumSpiesCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer]++;
				pEspionageAI->m_aiTurnLastSpyCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer] = GC.getGame().getGameTurn();
			}
			break;
			case SPY_RESULT_KILLED:
				// notify defending player that they killed a spy
			{
				CvAssertMsg(false, "This code was not expected to be called");
			}
			break;
			}
		}
	}

	m_aSpyNotificationMessages.clear();
}
/// AddIntrigueMessage - This is called when a piece of intrigue is found out by a spy. The reason it is kept in a list is to prevent repeat messages warning about the
///                      same event by the same spy
void CvPlayerEspionage::AddIntrigueMessage(PlayerTypes eDiscoveringPlayer, PlayerTypes eSourcePlayer, PlayerTypes eTargetPlayer, BuildingTypes eBuilding, ProjectTypes eProject, CvIntrigueType eIntrigueType, uint uiSpyIndex, CvCity* pCity, bool bShowNotification)
{
	CvAssertMsg(GetNumSpies() > 0, "How can you add an intrigue message when there are no spies?");

	int iCityX = -1;
	int iCityY = -1;
	if(pCity)
	{
		iCityX = pCity->getX();
		iCityY = pCity->getY();
	}

	// go through all the intrigue messages to see if there is a duplicate
	for(uint ui = 0; ui < m_aIntrigueNotificationMessages.size(); ui++)
	{
		if (m_aIntrigueNotificationMessages[ui].m_eSourcePlayer == eSourcePlayer &&
			m_aIntrigueNotificationMessages[ui].m_eTargetPlayer == eTargetPlayer &&
			m_aIntrigueNotificationMessages[ui].m_iIntrigueType == eIntrigueType)
		{
			bool bDuplicateTimer = GC.getGame().getGameTurn() - m_aIntrigueNotificationMessages[ui].m_iTurnNum < 30;

			if (eIntrigueType == INTRIGUE_TYPE_DECEPTION || eIntrigueType == INTRIGUE_TYPE_BUILDING_ARMY || eIntrigueType == INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY)
			{
				if(bDuplicateTimer)
				{
					// if duplicate is found, exit the function
					return;
				}
			}
			else if (m_aIntrigueNotificationMessages[ui].m_eBuilding == eBuilding &&
					 m_aIntrigueNotificationMessages[ui].m_eProject == eProject &&
					 m_aIntrigueNotificationMessages[ui].m_iCityX == iCityX &&
					 m_aIntrigueNotificationMessages[ui].m_iCityY == iCityY)
			{
				if(bDuplicateTimer)
				{
					// if duplicate is found, exit the function
					return;
				}
			}
		}
	}

	// no matching message found so adding it to the list
	IntrigueNotificationMessage kMessage;
	kMessage.m_eDiscoveringPlayer = eDiscoveringPlayer;
	kMessage.m_eSourcePlayer = eSourcePlayer;
	kMessage.m_eTargetPlayer = eTargetPlayer;
	kMessage.m_eBuilding = eBuilding;
	kMessage.m_eProject = eProject;
	kMessage.m_iIntrigueType = eIntrigueType;
	kMessage.m_iCityX = iCityX;
	kMessage.m_iCityY = iCityY;
	kMessage.m_iTurnNum = GC.getGame().getGameTurn();
	kMessage.m_strSpyName = "";
	kMessage.iSpyID = uiSpyIndex;
	if (eDiscoveringPlayer == m_pPlayer->GetID())
	{
		Localization::String str = Localization::Lookup("TXT_KEY_SPY_FULL_NAME");
		str << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
		str << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
		kMessage.m_strSpyName = str.toUTF8();
	}
	kMessage.m_bShared = false;
	m_aIntrigueNotificationMessages.push_back(kMessage);

	if (bShowNotification)
	{
		CvNotifications* pNotifications = m_pPlayer->GetNotifications();
		CvAssertMsg(pNotifications, "No notifications found. This is wrong!");
		if(!pNotifications)
		{
			return;
		}

		switch(kMessage.m_iIntrigueType)
		{
		case INTRIGUE_TYPE_DECEPTION:
			{
				Localization::String strSummary;
				Localization::String strNotification;
				if(kMessage.m_eTargetPlayer == m_pPlayer->GetID())  // if we found intrigue related to us
				{
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_PLOTTING_AGAINST_YOU_S");
					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
					}
					else
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
					}

					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_PLOTTING_AGAINST_YOU");
					strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
					CvAssertMsg(pCity, "City should be defined but is null");
					if(pCity)
					{
						strNotification << pCity->getNameKey();
					}
					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
					{
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
					}
					else
					{
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
					}
				}
				else if(kMessage.m_eTargetPlayer == NO_PLAYER)  // if we don't know who the intrigue information is about
				{
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_PLOTTING_AGAINST_UNKNOWN_S");
					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
					}
					else
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
					}

					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_PLOTTING_AGAINST_UNKNOWN");
					strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
					CvAssertMsg(pCity, "City should be defined but is null");
					if(pCity)
					{
						strNotification << pCity->getNameKey();
					}
					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
					{
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
					}
					else
					{
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
					}
				}
				else // if we found intrigue related to another player
				{
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_PLOTTING_AGAINST_KNOWN_S");
					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
					}
					else
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
					}

					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eTargetPlayer).isHuman())
					{
						strSummary << GET_PLAYER(kMessage.m_eTargetPlayer).getNickName();
					}
					else
					{
						strSummary << GET_PLAYER(kMessage.m_eTargetPlayer).getNameKey();
					}

					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_PLOTTING_AGAINST_KNOWN");
					strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
					CvAssertMsg(pCity, "City should be defined but is null");
					if(pCity)
					{
						strNotification << pCity->getNameKey();
					}
					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
					{
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
					}
					else
					{
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
					}

					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eTargetPlayer).isHuman())
					{
						strNotification << GET_PLAYER(kMessage.m_eTargetPlayer).getNickName();
					}
					else
					{
						strNotification << GET_PLAYER(kMessage.m_eTargetPlayer).getNameKey();
					}
				}
				pNotifications->Add(NOTIFICATION_INTRIGUE_DECEPTION, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, kMessage.m_eTargetPlayer);
			}
			break;
		case INTRIGUE_TYPE_BUILDING_ARMY:
			{
				Localization::String strSummary;
				Localization::String strNotification;

				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_BUILDING_ARMY_S");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
				{
					strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
				}
				else
				{
					strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
				}

				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_BUILDING_ARMY");
				strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
				strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
				{
					strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
				}
				else
				{
					strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
				}

				pNotifications->Add(NOTIFICATION_INTRIGUE_BUILDING_SNEAK_ATTACK_ARMY, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
			break;
		case INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY:
			{
				Localization::String strSummary;
				Localization::String strNotification;

				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_BUILDING_AMPHIBIOUS_ARMY_S");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
				{
					strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
				}
				else
				{
					strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
				}

				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_BUILDING_AMPHIBIOUS_ARMY");
				strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
				strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
				{
					strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
				}
				else
				{
					strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
				}

				pNotifications->Add(NOTIFICATION_INTRIGUE_BUILDING_SNEAK_ATTACK_AMPHIBIOUS, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
			break;

		case INTRIGUE_TYPE_ARMY_SNEAK_ATTACK:
			{
				Localization::String strSummary;
				Localization::String strNotification;
				NotificationTypes eNotification;

				// targeting me
				if(kMessage.m_eTargetPlayer == m_pPlayer->GetID())
				{
					if(pCity)
					{
						eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_KNOWN;
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_KNOWN_S");
						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
						}
						strSummary << pCity->getNameKey();

						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_KNOWN");
						strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();
					}
					else
					{
						eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_UNKNOWN;
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_UNKNOWN_S");
						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
						}

						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_UNKNOWN");
						strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
					}
				}
				// targeting somebody else I know
				else if(kMessage.m_eTargetPlayer != NO_PLAYER && kMessage.m_eTargetPlayer != MAX_MAJOR_CIVS)
				{
					if(pCity)
					{
						eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_KNOWN;
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_KNOWN_S");
						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
						}
						strSummary << pCity->getNameKey();

						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_KNOWN");
						strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
						strNotification << GET_PLAYER(kMessage.m_eTargetPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();
					}
					else
					{
						eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_UNKNOWN;
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_UNKNOWN_S");
						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
						}

						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eTargetPlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eTargetPlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eTargetPlayer).getNameKey();
						}

						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_UNKNOWN");
						strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
						strNotification << GET_PLAYER(kMessage.m_eTargetPlayer).getCivilizationDescriptionKey();
					}
				}
				// targeting an unknown player
				else
				{
					eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_UNKNOWN;
					// if the spy wasn't high enough rank to know who was being attacked
					if (kMessage.m_eTargetPlayer == MAX_MAJOR_CIVS)
					{
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_SOMEONE_S");
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_SOMEONE");
					}
					else
					{
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_UNKNOWN_S");
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_UNKNOWN");
					}

					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
					}
					else
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
					}

					strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
					strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
				}

				pNotifications->Add(eNotification, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, kMessage.m_eTargetPlayer);
			}
			break;
		case INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK:
			{
				Localization::String strSummary;
				Localization::String strNotification;
				NotificationTypes eNotification;

				// targeting me
				if(kMessage.m_eTargetPlayer == m_pPlayer->GetID())
				{
					if(pCity)
					{
						eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_KNOWN;
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_KNOWN_S");
						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
						}
						strSummary << pCity->getNameKey();

						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_KNOWN");
						strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();
					}
					else
					{
						eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_UNKNOWN;
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_UNKNOWN_S");
						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
						}

						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_UNKNOWN");
						strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
					}
				}
				// targeting somebody else I know
				else if(kMessage.m_eTargetPlayer != NO_PLAYER && kMessage.m_eTargetPlayer != MAX_MAJOR_CIVS)
				{
					if(pCity)
					{
						eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_KNOWN;
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_KNOWN_S");
						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
						}
						strSummary << pCity->getNameKey();

						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_KNOWN");
						strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
						strNotification << GET_PLAYER(kMessage.m_eTargetPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();
					}
					else
					{
						eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_UNKNOWN;
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_UNKNOWN_S");
						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
						}

						if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eTargetPlayer).isHuman())
						{
							strSummary << GET_PLAYER(kMessage.m_eTargetPlayer).getNickName();
						}
						else
						{
							strSummary << GET_PLAYER(kMessage.m_eTargetPlayer).getNameKey();
						}

						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_UNKNOWN");
						strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
						strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
						strNotification << GET_PLAYER(kMessage.m_eTargetPlayer).getCivilizationDescriptionKey();
					}
				}
				// targeting an unknown player
				else
				{
					eNotification = NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_UNKNOWN;

					// if the spy wasn't high enough rank to know who was being attacked
					if (kMessage.m_eTargetPlayer == MAX_MAJOR_CIVS)
					{
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_SOMEONE_S");
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_SOMEONE");
					}
					else
					{
						strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_UNKNOWN_S");
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_UNKNOWN");
					}

					if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
					}
					else
					{
						strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
					}

					strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
					strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getCivilizationAdjectiveKey();
				}

				pNotifications->Add(eNotification, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, kMessage.m_eTargetPlayer);
			}
			break;
		case INTRIGUE_TYPE_CONSTRUCTING_WONDER:
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_CONSTRUCT_WONDER_S");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
				{
					strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
				}
				else
				{
					strSummary << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
				}
				
				if (kMessage.m_eBuilding != NO_BUILDING)
				{
					CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(kMessage.m_eBuilding);
					CvAssertMsg(pBuildingInfo, "pBuildingInfo is null");
					if (pBuildingInfo)
					{
						strSummary << pBuildingInfo->GetTextKey();
					}
				}
				else if (kMessage.m_eProject != NO_PROJECT)
				{
					CvProjectEntry* pProjectInfo = GC.getProjectInfo(kMessage.m_eProject);
					CvAssertMsg(pProjectInfo, "pProjectInfo is null");
					if (pProjectInfo)
					{
						strSummary << pProjectInfo->GetTextKey();
					}
				}

				Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_INTRIGUE_CONSTRUCT_WONDER");
				strNotification << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
				strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(kMessage.m_eSourcePlayer).isHuman())
				{
					strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNickName();
				}
				else
				{
					strNotification << GET_PLAYER(kMessage.m_eSourcePlayer).getNameKey();
				}

				if (kMessage.m_eBuilding != NO_BUILDING)
				{
					CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(kMessage.m_eBuilding);
					CvAssertMsg(pBuildingInfo, "pBuildingInfo is null");
					if (pBuildingInfo)
					{
						strNotification << pBuildingInfo->GetTextKey();
					}
				}
				else if (kMessage.m_eProject != NO_PROJECT)
				{
					CvProjectEntry* pProjectInfo = GC.getProjectInfo(kMessage.m_eProject);
					CvAssertMsg(pProjectInfo, "pProjectInfo is null");
					if (pProjectInfo)
					{
						strNotification << pProjectInfo->GetTextKey();
					}
				}				

				if (pCity)
				{
					strNotification << pCity->getNameKey();
				}

				pNotifications->Add(NOTIFICATION_INTRIGUE_CONSTRUCTING_WONDER, strNotification.toUTF8(), strSummary.toUTF8(), kMessage.m_iCityX, kMessage.m_iCityY, -1);
			}
			break;
		}
	}
}

/// GetIntrigueMessage - Getting the intrigue message in another text format for the intrigue message display
Localization::String CvPlayerEspionage::GetIntrigueMessage(uint uiIndex)
{
	CvAssertMsg(uiIndex < m_aIntrigueNotificationMessages.size(), "uiIndex out of bounds");
	if(uiIndex >= m_aIntrigueNotificationMessages.size())
	{
		return "";
	}

	Localization::String strResult = "";

	// get the city if it exists
	CvCity* pCity = NULL;
	if(m_aIntrigueNotificationMessages[uiIndex].m_iCityX != -1 && m_aIntrigueNotificationMessages[uiIndex].m_iCityY)
	{
		CvPlot* pPlot = GC.getMap().plot(m_aIntrigueNotificationMessages[uiIndex].m_iCityX, m_aIntrigueNotificationMessages[uiIndex].m_iCityY);
		if(pPlot)
		{
			pCity = pPlot->getPlotCity();
		}
	}

	switch(m_aIntrigueNotificationMessages[uiIndex].m_iIntrigueType)
	{
	case INTRIGUE_TYPE_DECEPTION:
		if(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer == m_pPlayer->GetID())  // if we found intrigue related to us
		{
			strResult = Localization::Lookup("TXT_KEY_INTRIGUE_PLOTTING_AGAINST_YOU");
			if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
			}
			else
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
			}
#if defined(MOD_BALANCE_CORE)
			m_pPlayer->GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer, 1);
#endif
		}
		else if(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer == NO_PLAYER)  // if we don't know who the intrigue information is about
		{
			strResult = Localization::Lookup("TXT_KEY_INTRIGUE_PLOTTING_AGAINST_UNKNOWN");
			if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
			}
			else
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
			}
		}
		else // if we found intrigue related to another player
		{
			strResult = Localization::Lookup("TXT_KEY_INTRIGUE_PLOTTING_AGAINST_KNOWN");
			if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
			}
			else
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
			}

			if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).isHuman())
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).getNickName();
			}
			else
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).getNameKey();
			}
		}
		break;
	case INTRIGUE_TYPE_BUILDING_ARMY:
		strResult = Localization::Lookup("TXT_KEY_INTRIGUE_BUILDING_ARMY");
		if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
		{
			strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
		}
		else
		{
			strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
		}
		break;
	case INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY:
		strResult = Localization::Lookup("TXT_KEY_INTRIGUE_BUILDING_AMPHIBIOUS_ARMY");
		if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
		{
			strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
		}
		else
		{
			strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
		}
		break;
	case INTRIGUE_TYPE_ARMY_SNEAK_ATTACK:
		// player is target
		if(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer == m_pPlayer->GetID())
		{
			if(pCity)
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_KNOWN");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
				}
				strResult << pCity->getNameKey();
			}
			else
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_YOU_CITY_UNKNOWN");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
				}
			}
#if defined(MOD_BALANCE_CORE)
			m_pPlayer->GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer, 1);
#endif
		}
		// other player is target
		else if(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer != NO_PLAYER && m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer != MAX_MAJOR_CIVS)
		{
			if(pCity)
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_KNOWN");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
				}
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).getCivilizationAdjectiveKey();
				strResult << pCity->getNameKey();
			}
			else
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_KNOWN_CITY_UNKNOWN");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
				}

				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).getNameKey();
				}
			}
		}
		// unknown player is target
		else
		{
			if (m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer == MAX_MAJOR_CIVS)
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_SOMEONE");
			}
			else
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_ARMY_AGAINST_UNKNOWN");
			}
			if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
			}
			else
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
			}
		}
		break;
	case INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK:
		// player is target
		if(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer == m_pPlayer->GetID())
		{
			if(pCity)
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_KNOWN");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
				}
				strResult << pCity->getNameKey();
			}
			else
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_YOU_CITY_UNKNOWN");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
				}
			}
#if defined(MOD_BALANCE_CORE)
			m_pPlayer->GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer, 1);
#endif
		}
		// other player is target
		else if(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer != NO_PLAYER && m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer != MAX_MAJOR_CIVS)
		{
			if(pCity)
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_KNOWN");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
				}
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).getCivilizationAdjectiveKey();
				strResult << pCity->getNameKey();
			}
			else
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_KNOWN_CITY_UNKNOWN");
				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
				}

				if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).isHuman())
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).getNickName();
				}
				else
				{
					strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer).getNameKey();
				}
			}
		}
		// unknown player is target
		else
		{
			if (m_aIntrigueNotificationMessages[uiIndex].m_eTargetPlayer == MAX_MAJOR_CIVS)
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_SOMEONE");
			}
			else
			{
				strResult = Localization::Lookup("TXT_KEY_INTRIGUE_SNEAK_ATTACK_AMPHIB_AGAINST_UNKNOWN");
			}

			if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
			}
			else
			{
				strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
			}
		}
		break;
	case INTRIGUE_TYPE_CONSTRUCTING_WONDER:
		if (pCity)
		{
			strResult = Localization::Lookup("TXT_KEY_INTRIGUE_CONSTRUCT_WONDER");
		}
		else
		{
			strResult = Localization::Lookup("TXT_KEY_INTRIGUE_CONSTRUCT_WONDER_WO_CITY");
		}
		if(GC.getGame().isGameMultiPlayer() && GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).isHuman())
		{
			strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNickName();
		}
		else
		{
			strResult << GET_PLAYER(m_aIntrigueNotificationMessages[uiIndex].m_eSourcePlayer).getNameKey();
		}
		if (m_aIntrigueNotificationMessages[uiIndex].m_eBuilding != NO_BUILDING)
		{
			CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(m_aIntrigueNotificationMessages[uiIndex].m_eBuilding);
			CvAssertMsg(pBuildingInfo, "pBuildingInfo is null");
			if (pBuildingInfo)
			{
				strResult << pBuildingInfo->GetTextKey();
			}
		}
		else if (m_aIntrigueNotificationMessages[uiIndex].m_eProject != NO_PROJECT)
		{
			CvProjectEntry* pProjectInfo = GC.getProjectInfo(m_aIntrigueNotificationMessages[uiIndex].m_eProject);
			CvAssertMsg(pProjectInfo, "pProjectInfo is null");
			if (pProjectInfo)
			{
				strResult << pProjectInfo->GetTextKey();
			}
		}

		if (pCity)
		{
			strResult << pCity->getNameKey();
		}
		break;
	}

	return strResult;
}

/// HasRecentIntrigueAbout - Is there recent (within the last few turns) intrigue that the player can share about the target player
bool CvPlayerEspionage::HasRecentIntrigueAbout(PlayerTypes eTargetPlayer)
{
	IntrigueNotificationMessage* pMessage = GetRecentIntrigueInfo(eTargetPlayer);
	if(pMessage && pMessage->m_eSourcePlayer != NO_PLAYER && pMessage->m_iIntrigueType != NUM_INTRIGUE_TYPES)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/// GetRecentIntrigueInfo - Gets the information about the target player that is most recent
IntrigueNotificationMessage* CvPlayerEspionage::GetRecentIntrigueInfo(PlayerTypes eTargetPlayer)
{
	/// GetRecentIntriguePlotter: returns the player id of whoever is currently plotting against this player
	for(uint ui = 0; ui < m_aIntrigueNotificationMessages.size(); ui++)
	{
		// if we've already shared this information, don't count it
		if(m_aIntrigueNotificationMessages[ui].m_bShared)
		{
			continue;
		}

		// if the message isn't about the target, ignore
		if(m_aIntrigueNotificationMessages[ui].m_eTargetPlayer != eTargetPlayer)
		{
			continue;
		}

		// if the message is too long ago, ignore
		if(m_aIntrigueNotificationMessages[ui].m_iTurnNum < (GC.getGame().getGameTurn() - iIntrigueTurnsValid))  // todo: make 5 an xml global
		{
			continue;
		}

		return &(m_aIntrigueNotificationMessages[ui]);
	}

	return NULL;
}

/// HasSharedIntrigue - Has this player shared information in the last few turns about this point of intrigue?
bool CvPlayerEspionage::HasSharedIntrigue(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer, CvIntrigueType eIntrigueType)
{
	for(uint ui = 0; ui < m_aIntrigueNotificationMessages.size(); ui++)
	{
		IntrigueNotificationMessage* pMessage = &(m_aIntrigueNotificationMessages[ui]);

		// if we HAVE NOT shared this information, don't count it
		if(!pMessage->m_bShared)
		{
			continue;
		}

		// if we did not discover the message, we cannot share the intrigue
		if (pMessage->m_eDiscoveringPlayer != m_pPlayer->GetID())
		{
			continue;
		}

		if(pMessage->m_eTargetPlayer != eTargetPlayer)
		{
			continue;
		}

		if(pMessage->m_eSourcePlayer != eSourcePlayer)
		{
			continue;
		}

		if(pMessage->m_iIntrigueType != eIntrigueType)
		{
			continue;
		}

		// has this intrigue been shared the last 30 turns?
		if((GC.getGame().getGameTurn() - m_aIntrigueNotificationMessages[ui].m_iTurnNum) > 30)
		{
			continue;
		}

		return true;
	}

	return false;
}

/// HasSharedIntrigue - Has this player shared any information in the last few turns?
bool CvPlayerEspionage::HasSharedIntrigue(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer)
{
	for(uint ui = 0; ui < m_aIntrigueNotificationMessages.size(); ui++)
	{
		IntrigueNotificationMessage* pMessage = &(m_aIntrigueNotificationMessages[ui]);

		// if we HAVE NOT shared this information, don't count it
		if(!pMessage->m_bShared)
		{
			continue;
		}

		// can't share intrigue we didn't discover
		if (pMessage->m_eDiscoveringPlayer != m_pPlayer->GetID())
		{
			continue;
		}

		if(pMessage->m_eTargetPlayer != eTargetPlayer)
		{
			continue;
		}

		if(pMessage->m_eSourcePlayer != eSourcePlayer)
		{
			continue;
		}

		return true;
	}

	return false;
}


/// MarkRecentIntrigueAsShared - Set the m_bShared value on messages that have been shared. Returns the number of messages that were modified
int CvPlayerEspionage::MarkRecentIntrigueAsShared(PlayerTypes eTargetPlayer, PlayerTypes eSourcePlayer, CvIntrigueType eIntrigueType)
{
	int iNumShared = 0;
	for(uint ui = 0; ui < m_aIntrigueNotificationMessages.size(); ui++)
	{
		IntrigueNotificationMessage* pMessage = &(m_aIntrigueNotificationMessages[ui]);
		if(pMessage->m_bShared)
		{
			continue;
		}

		// can't share intrigue messages we don't own
		if (pMessage->m_eDiscoveringPlayer != m_pPlayer->GetID())
		{
			continue;
		}

		if(pMessage->m_eTargetPlayer != eTargetPlayer)
		{
			continue;
		}

		if(pMessage->m_eSourcePlayer != eSourcePlayer)
		{
			continue;
		}

		if(pMessage->m_iIntrigueType != eIntrigueType)
		{
			continue;
		}

		// this message has not been shared. Marking it as shared.
		pMessage->m_bShared = true;
		iNumShared++;
	}

	return iNumShared;
}

bool CvPlayerEspionage::HasSharedIntrigueAboutMe(PlayerTypes eFromPlayer)
{
	for(std::vector<IntrigueNotificationMessage>::iterator it = m_aIntrigueNotificationMessages.begin();
		it != m_aIntrigueNotificationMessages.end(); ++it)
	{
		if((*it).m_eDiscoveringPlayer == eFromPlayer)
			return true;
	}

	return false;
}

CvString CvPlayerEspionage::GetLogFileName() const
{
	CvString strLogName;
	strLogName = "EspionageLog.csv";
	return strLogName;
}

void CvPlayerEspionage::LogEspionageMsg(CvString& strMsg)
{
	if(GC.getLogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		FILogFile* pLog;

		pLog = LOGFILEMGR.GetLog(GetLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d,", GC.getGame().getElapsedGameTurns());
		strBaseString += m_pPlayer->getCivilizationShortDescription();
		strBaseString += ",";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvPlayerEspionage& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	int iNumSpies;
	loadFrom >> iNumSpies;

	CvEspionageSpy kTempSpy;

	for(int i = 0; i < iNumSpies; i++)
	{
		loadFrom >> kTempSpy;
		writeTo.m_aSpyList.push_back(kTempSpy);
	}

	int iNumSpyNames;
	loadFrom >> iNumSpyNames;
	for(int i = 0; i < iNumSpyNames; i++)
	{
		int iSpyNameIndex;
		loadFrom >> iSpyNameIndex;
		writeTo.m_aiSpyListNameOrder.push_back(iSpyNameIndex);
	}

	loadFrom >> writeTo.m_iSpyListNameOrderIndex;

	uint uiNumCivs;
	loadFrom >> uiNumCivs;
	for(uint uiCiv = 0; uiCiv < uiNumCivs; uiCiv++)
	{
		TechList aTechList;
		writeTo.m_aaPlayerStealableTechList.push_back(aTechList);

		uint uiNumTechs;
		loadFrom >> uiNumTechs;
		for(uint uiTech = 0; uiTech < uiNumTechs; uiTech++)
		{
			TechTypes eTech;
			loadFrom >> eTech;
			writeTo.m_aaPlayerStealableTechList[uiCiv].push_back(eTech);
		}
	}

	loadFrom >> uiNumCivs;
	for(uint uiCiv = 0; uiCiv < uiNumCivs; uiCiv++)
	{
		int iNumTechsToSteal;
		loadFrom >> iNumTechsToSteal;
		writeTo.m_aiNumTechsToStealList.push_back(iNumTechsToSteal);
	}
#if defined(MOD_BALANCE_CORE)
	loadFrom >> uiNumCivs;
	for(uint uiCiv = 0; uiCiv < uiNumCivs; uiCiv++)
	{
		int iNumSpyActionsDone;
		loadFrom >> iNumSpyActionsDone;
		writeTo.m_aiNumSpyActionsDone.push_back(iNumSpyActionsDone);
	}
#endif

	int iNumMessages;
	loadFrom >> iNumMessages;
	for(int i = 0; i < iNumMessages; i++)
	{
		SpyNotificationMessage kMessage;
		loadFrom >> kMessage.m_iCityX;
		loadFrom >> kMessage.m_iCityY;
		loadFrom >> kMessage.m_eAttackingPlayer;
		loadFrom >> kMessage.m_iSpyResult;
		loadFrom >> kMessage.m_eStolenTech;
		writeTo.m_aSpyNotificationMessages.push_back(kMessage);
	}

	loadFrom >> iNumMessages;
	for(int i = 0; i < iNumMessages; i++)
	{
		IntrigueNotificationMessage kMessage;
		loadFrom >> kMessage.m_eDiscoveringPlayer;
		loadFrom >> kMessage.m_eSourcePlayer;
		loadFrom >> kMessage.m_eTargetPlayer;
		loadFrom >> kMessage.m_eBuilding;
		int iProjectType;
		loadFrom >> iProjectType;
		kMessage.m_eProject = (ProjectTypes)iProjectType;
		loadFrom >> kMessage.m_iIntrigueType;
		loadFrom >> kMessage.m_iTurnNum;
		loadFrom >> kMessage.m_iCityX;
		loadFrom >> kMessage.m_iCityY;
		loadFrom >> kMessage.m_strSpyName;
		loadFrom >> kMessage.m_bShared;
		loadFrom >> kMessage.iSpyID;
		writeTo.m_aIntrigueNotificationMessages.push_back(kMessage);
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvPlayerEspionage& readFrom)
{
	uint uiVersion = 0;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_aSpyList.size();
	for(uint i = 0; i < readFrom.m_aSpyList.size(); i++)
	{
		saveTo << readFrom.m_aSpyList[i];
	}

	saveTo << readFrom.m_aiSpyListNameOrder.size();
	for(uint ui = 0; ui < readFrom.m_aiSpyListNameOrder.size(); ui++)
	{
		saveTo << readFrom.m_aiSpyListNameOrder[ui];
	}

	saveTo << readFrom.m_iSpyListNameOrderIndex;

	saveTo << readFrom.m_aaPlayerStealableTechList.size();
	for(uint uiCiv = 0; uiCiv < readFrom.m_aaPlayerStealableTechList.size(); uiCiv++)
	{
		saveTo << readFrom.m_aaPlayerStealableTechList[uiCiv].size();
		for(uint uiTech = 0; uiTech < readFrom.m_aaPlayerStealableTechList[uiCiv].size(); uiTech++)
		{
			saveTo << readFrom.m_aaPlayerStealableTechList[uiCiv][uiTech];
		}
	}

	saveTo << readFrom.m_aiNumTechsToStealList.size();
	for(uint uiCiv = 0; uiCiv < readFrom.m_aiNumTechsToStealList.size(); uiCiv++)
	{
		saveTo << readFrom.m_aiNumTechsToStealList[uiCiv];
	}
#if defined(MOD_BALANCE_CORE)
	
	saveTo << readFrom.m_aiNumSpyActionsDone.size();
	for(uint uiCiv = 0; uiCiv < readFrom.m_aiNumSpyActionsDone.size(); uiCiv++)
	{
		saveTo << readFrom.m_aiNumSpyActionsDone[uiCiv];
	}

#endif

	saveTo << readFrom.m_aSpyNotificationMessages.size();
	for(uint ui = 0; ui < readFrom.m_aSpyNotificationMessages.size(); ui++)
	{
		saveTo << readFrom.m_aSpyNotificationMessages[ui].m_iCityX;
		saveTo << readFrom.m_aSpyNotificationMessages[ui].m_iCityY;
		saveTo << readFrom.m_aSpyNotificationMessages[ui].m_eAttackingPlayer;
		saveTo << readFrom.m_aSpyNotificationMessages[ui].m_iSpyResult;
		saveTo << readFrom.m_aSpyNotificationMessages[ui].m_eStolenTech;
	}

	saveTo << readFrom.m_aIntrigueNotificationMessages.size();
	for(uint ui = 0; ui < readFrom.m_aIntrigueNotificationMessages.size(); ui++)
	{
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_eDiscoveringPlayer;
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_eSourcePlayer;
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_eTargetPlayer;
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_eBuilding;
		saveTo << (int)(readFrom.m_aIntrigueNotificationMessages[ui].m_eProject);
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_iIntrigueType;
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_iTurnNum;
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_iCityX;
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_iCityY;
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_strSpyName;
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].m_bShared;
		saveTo << readFrom.m_aIntrigueNotificationMessages[ui].iSpyID;
	}

	return saveTo;
}

// Constructor
CvCityEspionage::CvCityEspionage(void)
{
	Reset();
}

// Destructor
CvCityEspionage::~CvCityEspionage(void)
{
}

// Init
void CvCityEspionage::Init(CvCity* pCity)
{
	Reset();

	m_pCity = pCity;
}

// Uninit
void CvCityEspionage::Uninit(void)
{
	Reset();
}

// Reset
void CvCityEspionage::Reset(void)
{
	m_pCity = NULL;

	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiSpyAssignment[ui] = -1;
		m_aiAmount[ui] = -1;
		m_aiRate[ui] = -1;
		m_aiGoal[ui] = -1;
		m_aiLastProgress[ui] = -1;
		m_aiLastPotential[ui] = -1;
		m_aiLastBasePotential[ui] = -1;
		m_aiResult[ui] = SPY_RESULT_UNDETECTED;
		m_aiNumTimesCityRobbed[ui] = 0;
		m_aiPendingEventsForPlayer[ui] = 0;
	}
}

/// SetActivity - setting the values that get processed
void CvCityEspionage::SetActivity(PlayerTypes ePlayer, int iAmount, int iRate, int iGoal)
{
	CvAssertMsg(m_aiAmount[ePlayer] == -1, "Amount has not been reset");
	CvAssertMsg(m_aiRate[ePlayer] == -1, "Rate has not been reset");
	CvAssertMsg(m_aiGoal[ePlayer] == -1, "Goal has not been reset");
	m_aiAmount[ePlayer] = iAmount;
	m_aiRate[ePlayer] = iRate;
	m_aiGoal[ePlayer] = iGoal;
}

/// Process - move one step of the rate into the amount
void CvCityEspionage::Process(PlayerTypes ePlayer)
{
	CvAssertMsg(m_aiAmount[ePlayer] != -1, "Amount has not been set");
	CvAssertMsg(m_aiRate[ePlayer] != -1, "Rate has not been set");
	CvAssertMsg(m_aiGoal[ePlayer] != -1, "Goal has not been set");
	m_aiAmount[ePlayer] += m_aiRate[ePlayer];
}

/// HasReachedGoal - is the amount >= the goal
bool CvCityEspionage::HasReachedGoal(PlayerTypes ePlayer)
{
	CvAssertMsg(m_aiAmount[ePlayer] != -1, "Amount has not been set");
	CvAssertMsg(m_aiRate[ePlayer] != -1, "Rate has not been set");
	CvAssertMsg(m_aiGoal[ePlayer] != -1, "Goal has not been set");
	return m_aiAmount[ePlayer] >= m_aiGoal[ePlayer];
}

/// ResetProgress - reset the amount, rate, and goal for a player
void CvCityEspionage::ResetProgress(PlayerTypes ePlayer)
{
	m_aiAmount[ePlayer] = -1;
	m_aiGoal[ePlayer] = -1;
	m_aiRate[ePlayer] = -1;
}

/// SetLastProgress - set the amount of progress that the city will remember for the display
void CvCityEspionage::SetLastProgress(PlayerTypes ePlayer, int iProgress)
{
	m_aiLastProgress[ePlayer] = iProgress;
}

/// SetLastPotential - set the amount of potential last calculated by the city
void CvCityEspionage::SetLastPotential(PlayerTypes ePlayer, int iPotential)
{
	m_aiLastPotential[ePlayer] = iPotential;
}

/// SetLastBasePotential - set the amount of potential last calculated by the city without regarding the spy
void CvCityEspionage::SetLastBasePotential(PlayerTypes ePlayer, int iPotential)
{
	m_aiLastBasePotential[ePlayer] = iPotential;
}

/// SetSpyResult - set what happened this turn to the spy. Used for AI and notifications
#if defined(MOD_EVENTS_ESPIONAGE)
void CvCityEspionage::SetSpyResult(PlayerTypes eSpyOwner, int iSpyIndex, int iResult)
#else
void CvCityEspionage::SetSpyResult(PlayerTypes eSpyOwner, int iResult)
#endif
{
	m_aiResult[eSpyOwner] = iResult;

#if defined(MOD_EVENTS_ESPIONAGE)
	if (MOD_EVENTS_ESPIONAGE) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_EspionageResult, (int) eSpyOwner, iSpyIndex, iResult, m_pCity->getX(), m_pCity->getY());
	}
#endif
}

bool CvCityEspionage::HasPendingEvents(PlayerTypes ePlayer) const
{
	return m_aiPendingEventsForPlayer[ePlayer] > 0;
}
/// HasCounterSpy - if this city is occupied
bool CvCityEspionage::HasCounterSpy()
{
	return m_aiSpyAssignment[m_pCity->getOwner()] != -1;
}

// serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvCityEspionage& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	uint uiNumSpyAssignment;
	loadFrom >> uiNumSpyAssignment;
	for(uint ui = 0; ui < uiNumSpyAssignment; ui++)
	{
		loadFrom >> writeTo.m_aiSpyAssignment[ui];
	}

	// amount
	uint uiNumAmounts;
	loadFrom >> uiNumAmounts;
	for(uint ui = 0; ui < uiNumAmounts; ui++)
	{
		loadFrom >> writeTo.m_aiAmount[ui];
	}

	// rate
	uint uiNumRates;
	loadFrom >> uiNumRates;
	for(uint ui = 0; ui < uiNumRates; ui++)
	{
		loadFrom >> writeTo.m_aiRate[ui];
	}

	// goal
	uint uiNumGoals;
	loadFrom >> uiNumGoals;
	for(uint ui = 0; ui < uiNumGoals; ui++)
	{
		loadFrom >> writeTo.m_aiGoal[ui];
	}

	// last progress
	int iNumLastProgresses;
	loadFrom >> iNumLastProgresses;
	for(int i = 0; i < iNumLastProgresses; i++)
	{
		loadFrom >> writeTo.m_aiLastProgress[i];
	}

	// last potential
	int iNumLastPotential;
	loadFrom >> iNumLastPotential;
	for (int i = 0; i < iNumLastPotential; i++)
	{
		loadFrom >> writeTo.m_aiLastPotential[i];
	}

	// last base potential
	int iNumLastBasePotential;
	loadFrom >> iNumLastBasePotential;
	for (int i = 0; i < iNumLastBasePotential; i++)
	{
		loadFrom >> writeTo.m_aiLastBasePotential[i];
	}

	// results
	int iNumResults;
	loadFrom >> iNumResults;
	for(int i = 0; i < iNumResults; i++)
	{
		loadFrom >> writeTo.m_aiResult[i];
	}

	// num times robbed
	loadFrom >> iNumResults;
	for (int i = 0; i < iNumResults; i++)
	{
		loadFrom >> writeTo.m_aiNumTimesCityRobbed[i];
	}

	loadFrom >> iNumResults;
	for (int i = 0; i < iNumResults; i++)
	{
		loadFrom >> writeTo.m_aiPendingEventsForPlayer[i];
	}

	return loadFrom;
}

// serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvCityEspionage& readFrom)
{
	uint uiVersion = 0;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << MAX_MAJOR_CIVS;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiSpyAssignment[ui];
	}

	// amount
	saveTo << MAX_MAJOR_CIVS;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiAmount[ui];
	}

	// rate
	saveTo << MAX_MAJOR_CIVS;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiRate[ui];
	}

	// goal
	saveTo << MAX_MAJOR_CIVS;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiGoal[ui];
	}

	// last progress
	saveTo << MAX_MAJOR_CIVS;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiLastProgress[ui];
	}

	// last potential
	saveTo << MAX_MAJOR_CIVS;
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiLastPotential[ui];
	}

	// last base potential
	saveTo << MAX_MAJOR_CIVS;
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiLastBasePotential[ui];
	}

	// results
	saveTo << MAX_MAJOR_CIVS;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiResult[ui];
	}

	// num times robbed
	saveTo << MAX_MAJOR_CIVS;
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiNumTimesCityRobbed[ui];
	}

	saveTo << MAX_MAJOR_CIVS;
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiPendingEventsForPlayer[ui];
	}

	return saveTo;
}

//=====================================
// CvEspionageAI
//=====================================
/// Constructor
CvEspionageAI::CvEspionageAI(void):
	m_pPlayer(NULL),
	m_bUNCountdownStarted(false),
	m_iTurnEspionageStarted(-1)
{
}

/// Destructor
CvEspionageAI::~CvEspionageAI(void)
{
	Uninit();
}

/// Initialize class data
void CvEspionageAI::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	Reset();
}

/// Cleanup
void CvEspionageAI::Uninit()
{
	Reset();
}

/// Reset
void CvEspionageAI::Reset()
{
	m_aiCivOutOfTechTurn.clear();
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiCivOutOfTechTurn.push_back(-1);
	}

	m_aiNumSpiesCaught.clear();
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiNumSpiesCaught.push_back(0);
	}

	m_aiNumSpiesKilled.clear();
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiNumSpiesKilled.push_back(0);
	}

	m_aiNumSpiesDied.clear();
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiNumSpiesDied.push_back(0);
	}

	m_aiTurnLastSpyCaught.clear();
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiTurnLastSpyCaught.push_back(-1);
	}

	m_aiTurnLastSpyKilled.clear();
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiTurnLastSpyKilled.push_back(-1);
	}

	m_aiTurnLastSpyDied.clear();
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiTurnLastSpyDied.push_back(-1);
	}

	m_bUNCountdownStarted = false;
	m_iTurnEspionageStarted = -1;
}

// sort player numbers
struct PlayerAndTechTurn
{
	int iPlayerIndex;
	int iOutOfTechTurn;
	int iLargestProgress;
};

struct PlayerAndTechTurnEval
{
	bool operator()(PlayerAndTechTurn const& a, PlayerAndTechTurn const& b) const
	{
		CvAssertMsg(g_eSortPlayer != NO_PLAYER, "g_eSortPlayer not defined when sort was called");
		if(a.iOutOfTechTurn < b.iOutOfTechTurn) return true;
		if(a.iOutOfTechTurn > b.iOutOfTechTurn) return false;
		if(a.iLargestProgress > b.iLargestProgress) return true;
		if(a.iLargestProgress < b.iLargestProgress) return false;
		return false;
	}
};

/// Called every turn to see what to do with their spies
/// To be called after CvEspionagePlayer::DoTurn is run
void CvEspionageAI::DoTurn()
{
	// no humans, minor civs, or barbarians allowed!
	if(m_pPlayer->isHuman() || m_pPlayer->isMinorCiv() || m_pPlayer->isBarbarian())
	{
		return;
	}
#if defined(MOD_BALANCE_CORE)
	//No spies? No need!
	if(m_pPlayer->GetEspionage()->GetNumAliveSpies() <= 0)
		return;
#endif

	AI_PERF_FORMAT("AI-perf.csv", ("Espionage AI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	if (!MOD_BALANCE_CORE_SPIES_ADVANCED)
	{
		StealTechnology();
		UpdateCivOutOfTechTurn();
	}
	AttemptCoups();

	// mark these spies available to reassign
	EvaluateUnassignedSpies();
	EvaluateDefensiveSpies();
	EvaluateDiplomatSpies();

	// when the UN countdown starts, mark all spies to be reassigned
	if (!m_bUNCountdownStarted && GC.getGame().IsUnitedNationsActive())
	{
		m_bUNCountdownStarted = true;
		CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
		for (uint ui = 0; ui < pEspionage->m_aSpyList.size(); ui++)
		{
			CvEspionageSpy* pSpy = &(pEspionage->m_aSpyList[ui]);
			// don't process dead spies
#if defined(MOD_API_ESPIONAGE)
			if (pSpy->m_eSpyState == SPY_STATE_DEAD || pSpy->m_eSpyState == SPY_STATE_TERMINATED)
#else
			if (pSpy->m_eSpyState == SPY_STATE_DEAD)
#endif
			{
				continue;
			}

			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: UN constructed/reassign, %d,", ui);
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
				pEspionage->LogEspionageMsg(strMsg);
			}
		}
	}

	std::vector<ScoreCityEntry> apCityDiplomat = BuildDiplomatCityList();
	std::vector<ScoreCityEntry> apCityOffense = BuildOffenseCityList();
	std::vector<ScoreCityEntry> apCityDefense = BuildDefenseCityList();
	std::vector<ScoreCityEntry> apCityMinor = BuildMinorCityList();

	std::vector<ScoreCityEntry> aCityScores;

	for (uint i = 0; i < apCityDiplomat.size(); i++)
		aCityScores.push_back(apCityDiplomat[i]);
	for (uint i = 0; i < apCityOffense.size(); i++)
		aCityScores.push_back(apCityOffense[i]);
	for (uint i = 0; i < apCityDefense.size(); i++)
		aCityScores.push_back(apCityDefense[i]);
	for (uint i = 0; i < apCityMinor.size(); i++)
		aCityScores.push_back(apCityMinor[i]);

	std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryHighEval());

	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();

	if (GC.getLogging())
	{
		for (uint i = 0; i < min((uint)pEspionage->GetNumSpies(), (uint)aCityScores.size()); i++)
		{
			CvString strScore = "";
			strScore.Format("Score: %d,", aCityScores[i].m_iScore);

			CvString strMsg;
			strMsg.Format("Top City Choices");
			strMsg += ",";
			strMsg += aCityScores[i].m_pCity->getName();
			strMsg += ",";
			strMsg += strScore;
			strMsg += ",";
			strMsg += aCityScores[i].m_bDiplomat ? "as diplomat" : "as spy";
			strMsg += ",";
			strMsg += aCityScores[i].m_pCity->IsOriginalMinorCapital() ? "minor civ" : "major civ";
			strMsg += ",";
			strMsg += GET_PLAYER(aCityScores[i].m_pCity->getOwner()).getCivilizationShortDescription();
			strMsg += ",";
			pEspionage->LogEspionageMsg(strMsg);
		}
	}

	for (uint i = 0; i < aCityScores.size(); i++)
	{
		CvCity* pCity = aCityScores[i].m_pCity;
		if (!pCity)
			continue;

		// if a spy is already in this city, skip it
		if (pEspionage->GetSpyIndexInCity(pCity) != -1)
		{
			//skip the spy too!
			pEspionage->m_aSpyList[pEspionage->GetSpyIndexInCity(pCity)].m_bEvaluateReassignment = false;
			continue;
		}

		bool bNoSpiesLeft = true;
		for (uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
		{
			// if the spy is flagged to be reassigned
			if (!pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment)
				continue;

			bNoSpiesLeft = false;

			// dead spies are not processed
#if defined(MOD_API_ESPIONAGE)
			if (pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD || pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_TERMINATED)
#else
			if (pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD)
#endif
			{
				continue;
			}
			pEspionage->MoveSpyTo(pCity, uiSpy, aCityScores[i].m_bDiplomat);
			pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment = false;
			break;

		}
		if (bNoSpiesLeft)
			break;
	}
}

/// Checks to see if there are any technologies to steal
/// If so, steals them!
void CvEspionageAI::StealTechnology()
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();

	// try to steal technologies
	for(uint uiDefendingPlayer = 0; uiDefendingPlayer < MAX_MAJOR_CIVS; uiDefendingPlayer++)
	{
		PlayerTypes eDefendingPlayer = (PlayerTypes)uiDefendingPlayer;
		while(pEspionage->m_aiNumTechsToStealList[uiDefendingPlayer] > 0)
		{
			// steal a tech
			CvAssertMsg(pEspionage->m_aaPlayerStealableTechList[uiDefendingPlayer].size() > 0, "pEspionage->m_aaPlayerStealableTechList[uiPlayer] list is empty. Not good");
			TeamTypes eTeam = m_pPlayer->getTeam();
			TechTypes eStolenTech = pEspionage->m_aaPlayerStealableTechList[uiDefendingPlayer][0];
			GET_TEAM(eTeam).setHasTech(eStolenTech, true, m_pPlayer->GetID(), true, true);
			GET_TEAM(eTeam).GetTeamTechs()->SetNoTradeTech(eStolenTech, true);
#if defined(MOD_BALANCE_CORE)
			pEspionage->m_aiNumSpyActionsDone[eDefendingPlayer]++;
#endif
			
			m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_SPY_ATTACK, false, NO_GREATPERSON, NO_BUILDING, 1);

			// recalculate the num techs to steal list
			pEspionage->BuildStealableTechList((PlayerTypes)uiDefendingPlayer);
			if(pEspionage->m_aaPlayerStealableTechList[uiDefendingPlayer].size() > 0 && pEspionage->m_aiNumTechsToStealList[uiDefendingPlayer] > 0)
			{
				pEspionage->m_aiNumTechsToStealList[uiDefendingPlayer]--;
			}
			else
			{
				pEspionage->m_aiNumTechsToStealList[uiDefendingPlayer] = 0;
			}
		}
	}
}
void CvEspionageAI::UpdateCivOutOfTechTurn()
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();

	// determine which civs have run out of techs to steal
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eOtherPlayer = (PlayerTypes)ui;

		if(m_pPlayer->GetID() == eOtherPlayer)
		{
			continue;
		}

		// if surveillance has not been established, continue
		if(!pEspionage->IsAnySurveillanceEstablished(eOtherPlayer))
		{
			continue;
		}

		pEspionage->BuildStealableTechList(eOtherPlayer);
		if(pEspionage->m_aaPlayerStealableTechList[eOtherPlayer].size() > 0)
		{
			continue;
		}
		m_aiCivOutOfTechTurn[eOtherPlayer] = GC.getGame().getGameTurn();
		if(GC.getLogging())
		{
			CvString strMsg;
			strMsg.Format("AI no more tech to steal,");
			strMsg += ",";
			strMsg += ",";
			strMsg += GET_PLAYER(eOtherPlayer).getCivilizationShortDescription();
			strMsg += ",";
			pEspionage->LogEspionageMsg(strMsg);
		}
	}
}

void CvEspionageAI::AttemptCoups()
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	for(uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
	{
		if (!pEspionage->CanStageCoup(uiSpy))
		{
			continue;
		}
	
		// special Lena fix: no couping on CS that are allies with your friend
		CvCity* pCity = pEspionage->GetCityWithSpy(uiSpy);
		if (pCity && GET_PLAYER(pCity->getOwner()).isMinorCiv())
		{
			CvMinorCivAI* pMinorCivAI = GET_PLAYER(pCity->getOwner()).GetMinorCivAI();
			if (pMinorCivAI)
			{
				PlayerTypes eCurrentAlly = pMinorCivAI->GetAlly();
				if (eCurrentAlly != NO_PLAYER && eCurrentAlly != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsDoFAccepted(eCurrentAlly))
				{
					continue;
				}
			}
		}

		int iChanceOfSuccess = pEspionage->GetCoupChanceOfSuccess(uiSpy);
		if (iChanceOfSuccess >= 50)
		{
			int iRoll = GC.getGame().getSmallFakeRandNum(100, m_pPlayer->GetPseudoRandomSeed() + uiSpy);
			if (iRoll < iChanceOfSuccess)
			{
				pEspionage->AttemptCoup(uiSpy);
			}
		}
	}
}

std::vector<ScoreCityEntry> CvEspionageAI::BuildDiplomatCityList()
{
//	int iVal = 100;

//	CvPlayerCulture* pCulture = m_pPlayer->GetCulture();
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();

	std::vector<ScoreCityEntry> aCityScores;
	int iNumPlayers = 0;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes)ui;

		// don't count the player's own cities
		if(eTargetPlayer == m_pPlayer->GetID() || !GET_PLAYER(eTargetPlayer).isAlive())
		{
			continue;
		}

		CvCity* pCapitalCity = GET_PLAYER(eTargetPlayer).getCapitalCity();
		if (!pCapitalCity)
		{
			continue;
		}

		if (m_pPlayer->GetLeagueAI()->GetDiplomatUsefulnessAtCiv(eTargetPlayer) == CvLeagueAI::DIPLOMAT_USEFULNESS_NONE)
		{
			continue;
		}

		// if we can't see it, we can't move a diplomat there.
		if (!pCapitalCity->isRevealed(m_pPlayer->getTeam(), false))
		{
			continue;
		}

		iNumPlayers++;

		ScoreCityEntry kEntry;

		kEntry.m_pCity = pCapitalCity;

		CvLeagueAI::DiplomatUsefulnessLevels eUsefulness = m_pPlayer->GetLeagueAI()->GetDiplomatUsefulnessAtCiv(eTargetPlayer);
		switch (eUsefulness)
		{
		case(CvLeagueAI::DIPLOMAT_USEFULNESS_NONE) :
			continue;
		case(CvLeagueAI::DIPLOMAT_USEFULNESS_LOW) :
			kEntry.m_iScore = 25;
			break;
		case(CvLeagueAI::DIPLOMAT_USEFULNESS_MEDIUM) :
			kEntry.m_iScore = 50;
			break;
		case(CvLeagueAI::DIPLOMAT_USEFULNESS_HIGH) :
			kEntry.m_iScore = 100;
			break;
		default:
			continue;
		}

		if (pDiploAI->IsGoingForDiploVictory())
			kEntry.m_iScore *= 2;

		kEntry.m_bDiplomat = true;
		aCityScores.push_back(kEntry);
	}

	std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryHighEval());

	int iDesired = max(1, iNumPlayers / 4);
	if (pDiploAI->IsGoingForDiploVictory())
		iDesired *= 2;

	int iTotal = 0;
	for (uint i = 0; i < (uint)aCityScores.size(); i++)
	{
		if (iTotal >= iDesired)
		{
			aCityScores[i].m_iScore *= (90 - (iTotal * 10));
			aCityScores[i].m_iScore /= 100;
		}

		iTotal++;
	}
	return aCityScores;
} 

std::vector<ScoreCityEntry> CvEspionageAI::BuildOffenseCityList()
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	PlayerTypes ePlayer = m_pPlayer->GetID();
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();

	std::vector<ScoreCityEntry> aCityScores;
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes)i;

		// don't count the player's own cities
		if (eTargetPlayer == ePlayer)
		{
			continue;
		}

		if (GET_PLAYER(eTargetPlayer).isMinorCiv())
			continue;

		if (!m_pPlayer->GetDiplomacyAI()->IsHasMet(eTargetPlayer))
			continue;

		// If we promised not to spy or it's a bad idea to spy on them, then don't spy on them!
		if (pDiploAI->IsPlayerBadTheftTarget(eTargetPlayer, THEFT_TYPE_SPY))
		{
			continue;
		}

		TeamTypes eTargetTeam = GET_PLAYER(eTargetPlayer).getTeam();
		CvDiplomacyAI* pTargetDiploAI = GET_PLAYER(eTargetPlayer).GetDiplomacyAI();

		CvCity* pLoopCity;
		int iLoop;
		for (pLoopCity = GET_PLAYER(eTargetPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eTargetPlayer).nextCity(&iLoop))
		{
			CvPlot* pCityPlot = pLoopCity->plot();
			CvAssertMsg(pCityPlot, "City plot is null!");
			if(!pCityPlot)
			{
				continue;
			}

			if (!pCityPlot->isRevealed(m_pPlayer->getTeam()))
			{
				continue;
			}

			int iValue = 0;

			//hmm...sometimes we want more, sometimes we want less...

			//if they're stronger than us in spies, let's go for their weak cities
			if (GET_PLAYER(eTargetPlayer).GetEspionage()->GetNumSpies() > pEspionage->GetNumSpies())
			{
				iValue = (100+GC.getESPIONAGE_GATHERING_INTEL_COST_PERCENT()) - pLoopCity->GetEspionageRankingForEspionage();
				iValue /= 10;
			}
			//if we are stronger than them, target their best cities.
			else
			{
				iValue = pLoopCity->GetEspionageRankingForEspionage();
				iValue /= 10;
			}

			int iDiploModifier = 100;
			if (pDiploAI->GetWarGoal(eTargetPlayer) == WAR_GOAL_PREPARE)
			{
				iDiploModifier += 15;
			}
			else if (GET_TEAM(eTeam).isAtWar(eTargetTeam))
			{
				// ignore promises
				// bonus targeting!
				iDiploModifier += 15;
			}
			else // we're not at war with them, so look at other factors
			{
				
				for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
				{
					PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
					if (eMinor != NO_PLAYER)
					{
						CvPlayer* pMinor = &GET_PLAYER(eMinor);
						if (pMinor && pMinor->isMinorCiv())
						{
							CvMinorCivAI* pMinorCivAI = pMinor->GetMinorCivAI();
							if (pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_UNIT_STEAL_FROM))
							{
								if (pMinorCivAI->GetQuestData1(m_pPlayer->GetID(), MINOR_CIV_QUEST_UNIT_STEAL_FROM) == eTargetPlayer)
								{
									iDiploModifier += 25;
								}
							}
						}
					}
				}
				// if we've denounced them or they've denounced us, spy bonus!
				if (pDiploAI->IsDenouncedPlayer(eTargetPlayer) || pTargetDiploAI->IsDenouncedPlayer(ePlayer))
				{
					iDiploModifier += 33;
				}

				if (GET_TEAM(eTeam).IsHasResearchAgreement(eTargetTeam))
				{
					iDiploModifier -= 100;
				}

				if (GET_TEAM(eTeam).IsAllowsOpenBordersToTeam(eTargetTeam))
				{
					iDiploModifier += 25;
				}

				if (GET_TEAM(eTargetTeam).IsAllowsOpenBordersToTeam(eTeam))
				{
					iDiploModifier += 25;
				}
				if (pLoopCity->isCapital())
				{
					iDiploModifier += 25;
				}
				if (pDiploAI->GetCivApproach(eTargetPlayer) >= CIV_APPROACH_AFRAID)
				{
					iDiploModifier -= 15;
				}
				if (pDiploAI->GetCivApproach(eTargetPlayer) <= CIV_APPROACH_GUARDED)
				{
					iDiploModifier += 15;
				}
				int iCultureMod = m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eTargetPlayer);
				if (iCultureMod > 0)
				{
					iDiploModifier += iCultureMod * 25;
				}
				//Spread our spies out a bit.
				for (uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
				{
					CvCity* pCity = pEspionage->GetCityWithSpy(uiSpy);
					if (pCity)
					{
						if (pCity->getOwner() == eTargetPlayer)
						{
							iDiploModifier -= 25;
							break;
						}
					}
				}
			}

			ReligionTypes eReligion = m_pPlayer->GetReligions()->GetCurrentReligion();
			if (eReligion != NO_RELIGION)
			{
				CvGameReligions* pReligions = GC.getGame().GetGameReligions();
				const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());

				CvCity* pHolyCity = pMyReligion->GetHolyCity();
				iDiploModifier += pMyReligion->m_Beliefs.GetHappinessFromForeignSpies(m_pPlayer->GetID(), pHolyCity, true) * 10;
			}

			ScoreCityEntry kEntry;
			kEntry.m_pCity = pLoopCity;

			iValue *= iDiploModifier;
			iValue /= 100;

			if (iValue <= 0)
				continue;

			kEntry.m_iScore = iValue;
			kEntry.m_bDiplomat = false;

			aCityScores.push_back(kEntry);
		}
	}
	return aCityScores;
}

std::vector<ScoreCityEntry> CvEspionageAI::BuildDefenseCityList()
{
	std::vector<ScoreCityEntry> aCityScores;
	PlayerTypes ePlayer = m_pPlayer->GetID();

	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		ScoreCityEntry kEntry;
		kEntry.m_pCity = pLoopCity;

		int iValue = (100 + GC.getESPIONAGE_GATHERING_INTEL_COST_PERCENT()) - pLoopCity->GetEspionageRankingForEspionage();
		iValue /= 10;

		if (MOD_BALANCE_CORE_SPIES_ADVANCED)
		{
			//would adding a counterspy here help things? If not, reduce interest.
			int iResistance = m_pPlayer->GetEspionage()->GetSpyResistance(pLoopCity, true);
			if (iResistance < GC.getESPIONAGE_GATHERING_INTEL_COST_PERCENT())
				iValue /= 2;
		}

		kEntry.m_iScore = iValue;
		if(pLoopCity->isCapital())
		{
			kEntry.m_iScore *= 150;
			kEntry.m_iScore /= 100;
		}
		for (int i = 0; i < MAX_MAJOR_CIVS; i++)
		{
			PlayerTypes eTargetPlayer = (PlayerTypes)i;

			// don't count the player's own cities
			if (eTargetPlayer == ePlayer)
			{
				continue;
			}

			if (GET_PLAYER(eTargetPlayer).isMinorCiv())
				continue;

			if (!m_pPlayer->GetDiplomacyAI()->IsHasMet(eTargetPlayer))
				continue;

			kEntry.m_iScore += pLoopCity->GetCityEspionage()->m_aiNumTimesCityRobbed[eTargetPlayer];
		}

		if(kEntry.m_iScore <= 0)
			continue;

		kEntry.m_bDiplomat = false;

		aCityScores.push_back(kEntry);
	}

	std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryHighEval());

	int iDesired = max(1, m_pPlayer->getNumCities() / 4);
	int iTotal = 0;

	for (uint i = 0; i < (uint)aCityScores.size(); i++)
	{
		if (iTotal >= iDesired)
		{
			aCityScores[i].m_iScore *= (90 - (iTotal * 10));
			aCityScores[i].m_iScore /= 100;
		}

		iTotal++;
	}
	return aCityScores;
}

std::vector<ScoreCityEntry> CvEspionageAI::BuildMinorCityList()
{
	std::vector<ScoreCityEntry> aCityScores;

	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();

	PlayerTypes eCurrentDiploThreat = NO_PLAYER;
	int iCityStatePlan = GetCityStatePlan(&eCurrentDiploThreat);

	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	for(uint ui = MAX_MAJOR_CIVS; ui < MAX_CIV_PLAYERS; ui++)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes)ui;

		// only count minor civs
		if (!GET_PLAYER(eTargetPlayer).isMinorCiv())
		{
			continue;
		}

		if (!GET_PLAYER(eTargetPlayer).isAlive())
		{
			continue;
		}

		// if we haven't met the civ, ignore
		if(!GET_TEAM(m_pPlayer->getTeam()).isHasMet((GET_PLAYER(eTargetPlayer).getTeam())))
		{
			continue;
		}

		// if at war with the minor civ, do not try to assign a spy in the city
		if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eTargetPlayer).getTeam()))
		{
			continue;
		}

		CvMinorCivAI* pMinorCivAI = GET_PLAYER(eTargetPlayer).GetMinorCivAI();
		CivApproachTypes eApproach = pDiploAI->GetCivApproach(eTargetPlayer);
		int iFriendshipWithMinor = pMinorCivAI->GetEffectiveFriendshipWithMajor(m_pPlayer->GetID());

		int iModifier = 0;

		for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
			if (eMinor != NO_PLAYER)
			{
				CvPlayer* pMinor = &GET_PLAYER(eMinor);
				if (pMinor && pMinor->isMinorCiv())
				{
					CvMinorCivAI* pMinorCivAI = pMinor->GetMinorCivAI();
					if (pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_UNIT_COUP_CITY))
					{
						if (pMinorCivAI->GetQuestData1(m_pPlayer->GetID(), MINOR_CIV_QUEST_UNIT_COUP_CITY) == eTargetPlayer)
						{
							iModifier += 50;
						}
					}
				}
			}
		}
		
		bool bAlreadyScoredCity = false;
		for (pLoopCity = GET_PLAYER(eTargetPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eTargetPlayer).nextCity(&iLoop))
		{
			if(!pEspionage->CanEverMoveSpyTo(pLoopCity))
			{
				continue;
			}

			if (pMinorCivAI->IsAllies(m_pPlayer->GetID()) && !pMinorCivAI->IsCloseToNotBeingAllies(m_pPlayer->GetID()))
				continue;

			// if we've already scored a city for this minor civ, we don't need to score another because the values would be the same
			if (bAlreadyScoredCity)
			{
				break;
			}
			bAlreadyScoredCity = true;

			ScoreCityEntry kEntry;
			kEntry.m_pCity = pLoopCity;

			int iValue = pEspionage->GetTheoreticalChanceOfCoup(pLoopCity);
			switch (m_pPlayer->GetProximityToPlayer(eTargetPlayer))
			{
			case PLAYER_PROXIMITY_NEIGHBORS:
				iValue += 40;
				break;
			case PLAYER_PROXIMITY_CLOSE:
				iValue += 30;
				break;
			case PLAYER_PROXIMITY_FAR:
				iValue += 20;
				break;
			case PLAYER_PROXIMITY_DISTANT:
				iValue += 10;
				break;
			}
			switch (iCityStatePlan)
			{
			case PLAN_PLAY_NORMAL:
				// If we're not protective or friendly, then don't bother with minor diplo
				if (eApproach >= CIV_APPROACH_NEUTRAL)
				{
					// Nearly everyone likes to grow
					if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME && !m_pPlayer->IsEmpireUnhappy())
					{
						iModifier += 5;
					}

					// If unhappy, prioritize mercantile
					if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE && m_pPlayer->IsEmpireUnhappy())
					{
						iModifier += 5;
					}

					// Slight negative weight towards militaristic
					if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC && !pDiploAI->IsGoingForWorldConquest() && !m_pPlayer->GetPlayerTraits()->IsWarmonger())
					{
						iModifier -= 5;
					}

					// If they have a resource we don't have, add extra weight
					int iResourcesWeLack = pMinorCivAI->GetNumResourcesMajorLacks(m_pPlayer->GetID());
					if (iResourcesWeLack > 0)
					{
						iModifier += iResourcesWeLack * 5;
					}

					// If the minor is hostile, then reduce the weighting
					if (pMinorCivAI->GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
					{
						iModifier -= 10;
					}

					// count however much we've invested into the friendship toward maintaining the friendship
					iModifier += min(25, (iFriendshipWithMinor / 25));
				}

				if (eApproach > CIV_APPROACH_HOSTILE)
				{
					if (pDiploAI->IsGoingForCultureVictory())
					{
						if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_CULTURED)
						{
							iModifier += 10;
						}
						else if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME && !m_pPlayer->IsEmpireUnhappy())
						{
							iModifier += 5;
						}
						else if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
						{
							iModifier += 5;
						}
					}
					else if (pDiploAI->IsGoingForWorldConquest())
					{
						if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
						{
							iModifier += 10;
						}
						else if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
						{
							iModifier += 5;
						}
					}
					else if (pDiploAI->IsGoingForSpaceshipVictory())
					{
						if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MARITIME && !m_pPlayer->IsEmpireUnhappy())
						{
							iModifier += 10;
						}
						else if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
						{
							iModifier += 5;
						}
						else if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MERCANTILE)
						{
							iModifier += 5;
						}
					}
					else // diplo victory, or the player doesn't know how they're going to try to win yet
					{
						// no modifiers
					}
				}
				break;

			case PLAN_COLLECT_VOTES:
				// if the minor is allied with someone else
				if (pMinorCivAI->GetAlly() != NO_PLAYER)
				{
					iModifier += min(25, (iFriendshipWithMinor / 25));
				}
				else
				{
					// count however much we've invested into the friendship toward maintaining the friendship
					iModifier += min(30, (iFriendshipWithMinor / 15));
				}
				break;
			case PLAN_ATTACK_CS_TO_PREVENT_DEFEAT:
				// attack the least protected cities
				if (pMinorCivAI->IsAllies(eCurrentDiploThreat))
				{
					iModifier += min(40, pMinorCivAI->GetEffectiveFriendshipWithMajor(eCurrentDiploThreat)/25);
				}
				break;
			case PLAN_DEFEND_CS_FOR_WIN:
				// defend the least defended cities!
				if (pMinorCivAI->IsAllies(eCurrentDiploThreat))
				{
					iModifier += min(40, pMinorCivAI->GetEffectiveFriendshipWithMajor(eCurrentDiploThreat) / 25);
				}
			}

			iValue *= 100 + iModifier;
			iValue /= 100;

			kEntry.m_bDiplomat = false;
			kEntry.m_iScore = iValue;
			aCityScores.push_back(kEntry);
		}
	}

	std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryHighEval());

	int iDesired = max(1, GC.getGame().GetNumMinorCivsAlive() / 5);
	if (pDiploAI->IsGoingForDiploVictory())
		iDesired *= 2;

	int iTotal = 0;
	for (uint i = 0; i < (uint)aCityScores.size(); i++)
	{
		if (iTotal >= iDesired)
		{
			aCityScores[i].m_iScore *= (90 - (iTotal * 10));
			aCityScores[i].m_iScore /= 100;
		}

		iTotal++;
	}
	return aCityScores;
}

int CvEspionageAI::GetCityStatePlan(PlayerTypes* peDiploThreat)
{
	int iCurrentPlan = PLAN_PLAY_NORMAL; 
	PlayerTypes eCurrentDiploThreat = NO_PLAYER;
	if (m_bUNCountdownStarted)
	{
		int aiMajorCivVotes[MAX_MAJOR_CIVS];
		for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			aiMajorCivVotes[ui] = 0;
		}

		int iAliveMinorCivs = 0;

		for(uint ui = MAX_MAJOR_CIVS; ui < MAX_CIV_PLAYERS; ui++)
		{
			PlayerTypes eTargetPlayer = (PlayerTypes)ui;

			// only count minor civs
			if (!GET_PLAYER(eTargetPlayer).isMinorCiv())
			{
				continue;
			}

			if (!GET_PLAYER(eTargetPlayer).isAlive())
			{
				continue;
			}

			iAliveMinorCivs++;

			PlayerTypes eAllyPlayer = GET_PLAYER(eTargetPlayer).GetMinorCivAI()->GetAlly();
			if (eAllyPlayer != NO_PLAYER)
			{
				aiMajorCivVotes[eAllyPlayer]++;
			}
		}

		int iMaxVotes = 0;
		for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			PlayerTypes eTargetPlayer = (PlayerTypes)ui;
			if (aiMajorCivVotes[eTargetPlayer] > iMaxVotes)
			{
				iMaxVotes = aiMajorCivVotes[eTargetPlayer];
				eCurrentDiploThreat = eTargetPlayer;
			}
		}

		if (eCurrentDiploThreat != NO_PLAYER)
		{
			if (iMaxVotes >= iAliveMinorCivs / 2)
			{
				if (eCurrentDiploThreat == m_pPlayer->GetID())
				{
					iCurrentPlan = PLAN_DEFEND_CS_FOR_WIN;
				}
				else
				{
					iCurrentPlan = PLAN_ATTACK_CS_TO_PREVENT_DEFEAT;
				}
			}
			else if (iMaxVotes >= iAliveMinorCivs / 4)
			{
				iCurrentPlan = PLAN_COLLECT_VOTES;
			}
		}
	}

	if (peDiploThreat)
	{
		*peDiploThreat = eCurrentDiploThreat;
	}
	return iCurrentPlan;
}

// does not move the spies, only flags them to be moved
void CvEspionageAI::EvaluateSpiesAssignedToTargetPlayer(PlayerTypes ePlayer)
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	for (uint ui = 0; ui < pEspionage->m_aSpyList.size(); ui++)
	{
		CvEspionageSpy* pSpy = &(pEspionage->m_aSpyList[ui]);
		// don't process dead spies
#if defined(MOD_API_ESPIONAGE)
		if (pSpy->m_eSpyState == SPY_STATE_DEAD || pSpy->m_eSpyState == SPY_STATE_TERMINATED)
#else
		if (pSpy->m_eSpyState == SPY_STATE_DEAD)
#endif
		{
			continue;
		}

		CvCity* pCity = pEspionage->GetCityWithSpy(ui);
		if (pCity && pCity->getOwner() == ePlayer)
		{
			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: assigned to promise player, %d,", ui);
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
				pEspionage->LogEspionageMsg(strMsg);
			}
		}
	}
}

// does not move the spies, only flags them to be moved
void CvEspionageAI::EvaluateUnassignedSpies(void)
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	for (uint ui = 0; ui < pEspionage->m_aSpyList.size(); ui++)
	{
		CvEspionageSpy* pSpy = &(pEspionage->m_aSpyList[ui]);
		// don't process dead spies
#if defined(MOD_API_ESPIONAGE)
		if (pSpy->m_eSpyState == SPY_STATE_DEAD || pSpy->m_eSpyState == SPY_STATE_TERMINATED)
#else
		if (pSpy->m_eSpyState == SPY_STATE_DEAD)
#endif
		{
			continue;
		}

		CvCity* pCity = pEspionage->GetCityWithSpy(ui);
		if (!pCity)
		{
			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: unassigned spy, %d,", ui);
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
				pEspionage->LogEspionageMsg(strMsg);
			}
		}
	}
}

// does not move the spies, only flags them to be moved
void CvEspionageAI::EvaluateDefensiveSpies(void)
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	for (uint ui = 0; ui < pEspionage->m_aSpyList.size(); ui++)
	{
		CvEspionageSpy* pSpy = &(pEspionage->m_aSpyList[ui]);
		// don't process dead spies
#if defined(MOD_API_ESPIONAGE)
		if (pSpy->m_eSpyState == SPY_STATE_DEAD || pSpy->m_eSpyState == SPY_STATE_TERMINATED)
#else
		if (pSpy->m_eSpyState == SPY_STATE_DEAD)
#endif
		{
			continue;
		}

		CvCity* pCity = pEspionage->GetCityWithSpy(ui);
		if (pCity && pCity->getOwner() == m_pPlayer->GetID())
		{
			//low, and increasing? Don't leave!
			if (MOD_BALANCE_CORE_SPIES_ADVANCED)
			{
				int iResistance = pEspionage->GetSpyResistance(pCity);
				if (pCity->GetEspionageRanking() <= 6)
				{
					//is our resistance better than average?
					if (iResistance > GC.getESPIONAGE_GATHERING_INTEL_COST_PERCENT())
					{
						pSpy->m_bEvaluateReassignment = false;
						if (GC.getLogging())
						{
							CvString strMsg;
							strMsg.Format("Re-eval: defensive spy keep them here, need improvement, %d,", ui);
							strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
							pEspionage->LogEspionageMsg(strMsg);
						}
					}
					else
					{
						pSpy->m_bEvaluateReassignment = true;
						if (GC.getLogging())
						{
							CvString strMsg;
							strMsg.Format("Re-eval: defensive spy not helping enough, let's be aggressive instead, %d,", ui);
							strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
							pEspionage->LogEspionageMsg(strMsg);
						}
					}
				}
				else
				{
					pSpy->m_bEvaluateReassignment = true;
					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Re-eval: defensive spy, rank has improved, %d,", ui);
						strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
						pEspionage->LogEspionageMsg(strMsg);
					}
				}
				
			}
			else
			{
				pSpy->m_bEvaluateReassignment = true;
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Re-eval: defensive spy, %d,", ui);
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
					pEspionage->LogEspionageMsg(strMsg);
				}
			}
		}
	}
}

// does not move the spies, only flags them to be moved
void CvEspionageAI::EvaluateDiplomatSpies(void)
{
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	for (uint ui = 0; ui < pEspionage->m_aSpyList.size(); ui++)
	{
		CvEspionageSpy* pSpy = &(pEspionage->m_aSpyList[ui]);
		// don't process dead spies
		if (pSpy->m_eSpyState == SPY_STATE_DEAD)
		{
			continue;
		}

		if (pSpy->m_bIsDiplomat)
		{
			if (pLeague && pLeague->GetTurnsUntilSession() > 5)
			{
				pSpy->m_bEvaluateReassignment = true;
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Re-eval: diplomat spy, %d,", ui);
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
					pEspionage->LogEspionageMsg(strMsg);
				}
			}
			else
			{
				pSpy->m_bEvaluateReassignment = false;
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Re-eval: diplomat spy, let's wait until session is over, %d,", ui);
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
					pEspionage->LogEspionageMsg(strMsg);
				}
			}
		}
	}
}

// serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvEspionageAI& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	uint uiCount;
	loadFrom >> uiCount;
	int iValue;
	for(uint ui = 0; ui < uiCount; ui++)
	{
		loadFrom >> iValue;
		writeTo.m_aiCivOutOfTechTurn.push_back(iValue);
	}

	loadFrom >> uiCount;
	for(uint ui = 0; ui < uiCount; ui++)
	{
		loadFrom >> iValue;
		writeTo.m_aiNumSpiesCaught.push_back(iValue);
	}

	loadFrom >> uiCount;
	for(uint ui = 0; ui < uiCount; ui++)
	{
		loadFrom >> iValue;
		writeTo.m_aiNumSpiesKilled.push_back(iValue);
	}

	loadFrom >> uiCount;
	for(uint ui = 0; ui < uiCount; ui++)
	{
		loadFrom >> iValue;
		writeTo.m_aiNumSpiesDied.push_back(iValue);
	}

	loadFrom >> uiCount;
	for(uint ui = 0; ui < uiCount; ui++)
	{
		loadFrom >> iValue;
		writeTo.m_aiTurnLastSpyCaught.push_back(iValue);
	}

	loadFrom >> uiCount;
	for(uint ui = 0; ui < uiCount; ui++)
	{
		loadFrom >> iValue;
		writeTo.m_aiTurnLastSpyKilled.push_back(iValue);
	}

	loadFrom >> uiCount;
	for(uint ui = 0; ui < uiCount; ui++)
	{
		loadFrom >> iValue;
		writeTo.m_aiTurnLastSpyDied.push_back(iValue);
	}

	loadFrom >> writeTo.m_bUNCountdownStarted;
	loadFrom >> writeTo.m_iTurnEspionageStarted;
	
	return loadFrom;
}

//// serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvEspionageAI& readFrom)
{
	uint uiVersion = 0;
	saveTo << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(saveTo);

	saveTo << readFrom.m_aiCivOutOfTechTurn.size();
	for(uint ui = 0; ui < readFrom.m_aiCivOutOfTechTurn.size(); ui++)
	{
		saveTo << readFrom.m_aiCivOutOfTechTurn[ui];
	}

	saveTo << readFrom.m_aiNumSpiesCaught.size();
	for(uint ui = 0; ui < readFrom.m_aiNumSpiesCaught.size(); ui++)
	{
		saveTo << readFrom.m_aiNumSpiesCaught[ui];
	}

	saveTo << readFrom.m_aiNumSpiesKilled.size();
	for(uint ui = 0; ui < readFrom.m_aiNumSpiesKilled.size(); ui++)
	{
		saveTo << readFrom.m_aiNumSpiesKilled[ui];
	}

	saveTo << readFrom.m_aiNumSpiesDied.size();
	for(uint ui = 0; ui < readFrom.m_aiNumSpiesDied.size(); ui++)
	{
		saveTo << readFrom.m_aiNumSpiesDied[ui];
	}

	saveTo << readFrom.m_aiTurnLastSpyCaught.size();
	for(uint ui = 0; ui < readFrom.m_aiTurnLastSpyCaught.size(); ui++)
	{
		saveTo << readFrom.m_aiTurnLastSpyCaught[ui];
	}

	saveTo << readFrom.m_aiTurnLastSpyKilled.size();
	for(uint ui = 0; ui < readFrom.m_aiTurnLastSpyKilled.size(); ui++)
	{
		saveTo << readFrom.m_aiTurnLastSpyKilled[ui];
	}

	saveTo << readFrom.m_aiTurnLastSpyDied.size();
	for(uint ui = 0; ui < readFrom.m_aiTurnLastSpyDied.size(); ui++)
	{
		saveTo << readFrom.m_aiTurnLastSpyDied[ui];
	}

	saveTo << readFrom.m_bUNCountdownStarted;
	saveTo << readFrom.m_iTurnEspionageStarted;

	return saveTo;
}
