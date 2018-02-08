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
#if defined(MOD_BALANCE_CORE)
#include "CvTypes.h"
#endif

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
const int iRandomRollSpyAction = 300;
const int iChancetoIdentifyCounterSpy = 275;
const int iChancetoDetectCounterSpy = 175;
const int iChancetoIdentifyNoCounterSpy = 275;
const int iChancetoDetectNoCounterSpy = 175;
const int iChancetoKillAA = 250;
const int iChancetoIdentifyAA = 150;
const int iSpyRankPower = 75;
PlayerTypes g_eSortPlayer = NO_PLAYER; // global - used for the sort

//=====================================
// CvEspionageSpy
//=====================================
/// Default Constructor
CvEspionageSpy::CvEspionageSpy()
	: m_iName(-1)
#if defined(MOD_BUGFIX_SPY_NAMES)
	, m_sName(NULL)
#endif
	, m_eRank(NUM_SPY_RANKS)
	, m_iCityX(-1)
	, m_iCityY(-1)
	, m_eSpyState(NUM_SPY_STATES)
	, m_iReviveCounter(0)
	, m_bIsDiplomat(false)
	, m_bEvaluateReassignment(true)
#if defined(MOD_API_ESPIONAGE)
	, m_bPassive(false)
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	, m_bIsThief(false)
	, m_iTurnActivated(-1)
	, m_iActionsDone(-1)
	, m_iCooldown(-1)
#endif
{
}

#if defined(MOD_BUGFIX_SPY_NAMES)
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
#endif

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
void CvEspionageSpy::SetSpyActiveTurn(int iGameTurn)
{
	m_iTurnActivated = iGameTurn;
}
int CvEspionageSpy::GetSpyActiveTurn()
{
	return m_iTurnActivated;
}
void CvEspionageSpy::SetAdvancedActions(int iValue)
{
	m_iActionsDone = iValue;
}
void CvEspionageSpy::ChangeAdvancedActions(int iValue)
{
	m_iActionsDone = GetAdvancedActions() + iValue;
}
int CvEspionageSpy::GetAdvancedActions()
{
	return m_iActionsDone;
}
void CvEspionageSpy::SetAdvancedActionsCooldown(int iValue)
{
	m_iCooldown = iValue;
}
void CvEspionageSpy::ChangeAdvancedActionsCooldown(int iValue)
{
	m_iCooldown = GetAdvancedActionsCooldown() + iValue;
}
int CvEspionageSpy::GetAdvancedActionsCooldown()
{
	return m_iCooldown;
}
#endif
/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvEspionageSpy& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_iName;
#if defined(MOD_BUGFIX_SPY_NAMES)
	MOD_SERIALIZE_READ(53, loadFrom, writeTo.m_sName, NULL);
#endif
	int iSpyRank;
	loadFrom >> iSpyRank;
	writeTo.m_eRank = (CvSpyRank)iSpyRank;
	loadFrom >> writeTo.m_iCityX;
	loadFrom >> writeTo.m_iCityY;

	int iSpyState;
	loadFrom >> iSpyState;
	writeTo.m_eSpyState = (CvSpyState)iSpyState;

	loadFrom >> writeTo.m_iReviveCounter;
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
#if defined(MOD_BALANCE_CORE_SPIES)
	MOD_SERIALIZE_READ(66, loadFrom, writeTo.m_iTurnActivated, -1);
	MOD_SERIALIZE_READ(66, loadFrom, writeTo.m_iActionsDone, -1);
	MOD_SERIALIZE_READ(66, loadFrom, writeTo.m_bIsThief, false);
	MOD_SERIALIZE_READ(66, loadFrom, writeTo.m_iCooldown, -1);
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
#if defined(MOD_BUGFIX_SPY_NAMES)
	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_sName);
#endif
	saveTo << (int)readFrom.m_eRank;
	saveTo << readFrom.m_iCityX;
	saveTo << readFrom.m_iCityY;
	saveTo << (int)readFrom.m_eSpyState;
	saveTo << readFrom.m_iReviveCounter;
	saveTo << readFrom.m_bIsDiplomat;
	saveTo << readFrom.m_bEvaluateReassignment;

#if defined(MOD_API_ESPIONAGE)
	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_bPassive);
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_iTurnActivated);
	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_iActionsDone);
	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_bIsThief);
	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_iCooldown);
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
#if defined(MOD_BALANCE_CORE)
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aPlayerStealableGWList[ui].clear();
	}
#endif

	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiNumTechsToStealList.push_back(0);
	}
#if defined(MOD_BALANCE_CORE)
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiNumGWToStealList.push_back(0);
	}
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
	m_aiNumTechsToStealList.clear();
#if defined(MOD_BALANCE_CORE)
	m_aiNumGWToStealList.clear();
	m_aiNumSpyActionsDone.clear();
#endif
	m_aIntrigueNotificationMessages.clear();
	m_aaPlayerStealableTechList.clear();
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiMaxTechCost[ui] = -1;
		m_aHeistLocations[ui].clear();
#if defined(MOD_BALANCE_CORE)
		m_aiNumSpyActionsDone[ui] = 0;
		m_aPlayerStealableGWList[ui].clear();
		m_aiMaxGWCost[ui] = -1;
#endif
	}
}


/// DoTurn
void CvPlayerEspionage::DoTurn()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvPlayerEspionage::DoTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );
	ProcessSpyMessages();

	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aHeistLocations[ui].clear();
	}

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
#if defined(MOD_BUGFIX_SPY_NAMES)
	GetNextSpyName(&kNewSpy);
#else
	kNewSpy.m_iName = GetNextSpyName();
#endif
	kNewSpy.m_bEvaluateReassignment = true;

#if defined(MOD_API_ESPIONAGE)
	kNewSpy.m_bPassive = false;
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
	kNewSpy.m_iTurnActivated = -1;
	kNewSpy.m_iActionsDone = 0;
	kNewSpy.m_iCooldown = 0;
#endif

	m_aSpyList.push_back(kNewSpy);

	CvNotifications* pNotifications = m_pPlayer->GetNotifications();
	if(pNotifications)
	{
#if defined(MOD_BUGFIX_SPY_NAMES)
		const char* szSpyName = kNewSpy.GetSpyName(m_pPlayer);
#else
		const char* szSpyName = m_pPlayer->getCivilizationInfo().getSpyNames(kNewSpy.m_iName);
#endif
		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_CREATED", szSpyName);
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_SPY_CREATED", szSpyName);
		pNotifications->Add(NOTIFICATION_SPY_CREATED_ACTIVE_PLAYER, strBuffer, strSummary, -1, -1, 0);
	}

	if(GC.getLogging())
	{
		CvString strMsg;
		strMsg.Format("New Spy, %d,", m_aSpyList.size() - 1);
#if defined(MOD_BUGFIX_SPY_NAMES)
		strMsg += GetLocalizedText(kNewSpy.GetSpyName(m_pPlayer));
#else
		strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(kNewSpy.m_iName));
#endif
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
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
	if (MOD_BALANCE_CORE_SPIES_ADVANCED)
	{
		if (pSpy->GetAdvancedActionsCooldown() > (GC.getBALANCE_SPY_SABOTAGE_RATE() * 15))
		{
			pSpy->SetAdvancedActionsCooldown(GC.getBALANCE_SPY_SABOTAGE_RATE() * 15);
		}
		else if (pSpy->GetAdvancedActionsCooldown() <= 0)
		{
			pSpy->SetAdvancedActionsCooldown(0);
		}
		else
		{
			pSpy->ChangeAdvancedActionsCooldown(-1 * GC.getGame().getJonRandNum(GC.getBALANCE_SPY_SABOTAGE_RATE(), "Random roll for cooldown"));
		}
	}
#endif
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
			}
			else // moved to an opponent's city
			{
#if defined(MOD_API_ESPIONAGE)
				pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_SURVEILLANCE);
#else
				pSpy->m_eSpyState = SPY_STATE_SURVEILLANCE;
#endif
				int iRate = CalcPerTurn(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
				int iGoal = CalcRequired(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
				pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);
#if defined(MOD_BALANCE_CORE_SPIES)
				pSpy->SetSpyActiveTurn(GC.getGame().getGameTurn());
				pSpy->SetAdvancedActions(0);
#endif
			}
		}
		break;
	case SPY_STATE_SURVEILLANCE:
		CvAssertMsg(pCity, "pCity not defined. What?");
		if(!pCity)
		{
			return;
		}

#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
		if(MOD_BALANCE_CORE_SPIES_ADVANCED)
		{
			if(!GET_PLAYER(eCityOwner).isMinorCiv())
			{
				AttemptAdvancedActions(uiSpyIndex);
			}
		}
#endif


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
#if defined(MOD_API_ESPIONAGE)
				if (!pSpy->m_bPassive) {
#endif
				BuildStealableTechList(eCityOwner);
#if defined(MOD_BALANCE_CORE)
				BuildStealableGWList(eCityOwner);
#endif
				// moved rate out here to set the potential
				int iBasePotentialRate = CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, -1);
				pCityEspionage->SetLastBasePotential(ePlayer, iBasePotentialRate);
#if defined(MOD_BALANCE_CORE)
				if (!pSpy->m_bIsThief && m_aaPlayerStealableTechList[eCityOwner].size() > 0)
#else
				if(m_aaPlayerStealableTechList[eCityOwner].size() > 0)
#endif
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
#if defined(MOD_BALANCE_CORE)
				else if (pSpy->m_bIsThief && m_aPlayerStealableGWList[eCityOwner].size() > 0)
				{
#if defined(MOD_API_ESPIONAGE)
					pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_PREPARING_HEIST);
#else
					pSpy->m_eSpyState = SPY_STATE_PREPARING_HEIST;
#endif
					pCityEspionage->ResetProgress(ePlayer);
					int iPotentialRate = CalcPerTurn(SPY_STATE_PREPARING_HEIST, pCity, uiSpyIndex);
					int iGoal = CalcRequired(SPY_STATE_PREPARING_HEIST, pCity, uiSpyIndex);
					pCityEspionage->SetActivity(ePlayer, 0, iPotentialRate, iGoal);
					pCityEspionage->SetLastProgress(ePlayer, iPotentialRate);
					pCityEspionage->SetLastPotential(ePlayer, iPotentialRate);
				}
#endif
				else
				{
					// if this is the first time they crossed the threshold
					if(!bLastQualified)
					{
						CvNotifications* pNotifications = m_pPlayer->GetNotifications();
						if(pNotifications)
						{
#if defined(MOD_BALANCE_CORE)
							Localization::String strSummary;
							Localization::String strNotification;
							if (pSpy->m_bIsThief)
							{
								strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_GREAT_WORK_S");
								strSummary << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
								strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_GREAT_WORK");
								strNotification << GetSpyRankName(pSpy->m_eRank);
							}
							else
							{
#endif
								strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH_S");
								strSummary << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
								strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH");
								strNotification << GetSpyRankName(pSpy->m_eRank);
#if defined(MOD_BALANCE_CORE)
							}
#endif
#if defined(MOD_BUGFIX_SPY_NAMES)
							strNotification << pSpy->GetSpyName(m_pPlayer);
#else
							strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
							strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
							pNotifications->Add(NOTIFICATION_SPY_CANT_STEAL_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
						}
					}
					int iRate = CalcPerTurn(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
					pCityEspionage->SetLastProgress(ePlayer, iRate);
					pCityEspionage->SetLastPotential(ePlayer, -1); // set the last potential back to zero so that
					pSpy->m_bEvaluateReassignment = true; // flag to re-evaluate because we can't steal
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
					if (MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->GetAdvancedActionsCooldown() <= (GC.getBALANCE_SPY_SABOTAGE_RATE() * 2))
					{
						pSpy->m_bEvaluateReassignment = false; // flag not to re-evaluate because high rank
						if(GC.getLogging())
						{
							CvString strMsg;
							strMsg.Format("Can't steal research, but staying because we're almost off cooldown: %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
							strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
							strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
							LogEspionageMsg(strMsg);
						}
					}
#endif
					if(GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Re-eval: can't steal research, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
						strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
						strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
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
		BuildStealableTechList(eCityOwner);
#if defined(MOD_BALANCE_CORE)
		BuildStealableGWList(eCityOwner);
#endif
		pCityEspionage->Process(ePlayer);
		// if the rate is too low, reassign the spy
		if (pCityEspionage->m_aiRate[m_pPlayer->GetID()] < 100)
		{
			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: potential too low, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
				LogEspionageMsg(strMsg);
			}
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
			if (MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->GetAdvancedActionsCooldown() <= (GC.getBALANCE_SPY_SABOTAGE_RATE() * 2))
			{
				pSpy->m_bEvaluateReassignment = false; // flag not to re-evaluate because high rank
				if(GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Nevermind, staying because we're almost off cooldown: %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
					LogEspionageMsg(strMsg);
				}
			}
#endif
		}
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
		if(MOD_BALANCE_CORE_SPIES_ADVANCED)
		{
			if(!GET_PLAYER(eCityOwner).isMinorCiv())
			{
				AttemptAdvancedActions(uiSpyIndex);
			}
		}
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
				LogEspionageMsg(strMsg);
			}
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
			if (MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->GetAdvancedActionsCooldown() <= (GC.getBALANCE_SPY_SABOTAGE_RATE() * 2))
			{
				pSpy->m_bEvaluateReassignment = false; // flag not to re-evaluate because high rank
				if(GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Nevermind, staying because we're almost off cooldown: %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
					LogEspionageMsg(strMsg);
				}
			}
#endif

			int iRate = CalcPerTurn(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
			int iGoal = CalcRequired(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
			int iAmount = iGoal; // put the player back at surveillance levels
			pCityEspionage->SetActivity(ePlayer, iAmount, iRate, iGoal);

			// set last progress rate
			iRate = CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex);
			pCityEspionage->SetLastProgress(ePlayer, iRate);

			// can't steal any techs from this civ
			m_aiNumTechsToStealList[eCityOwner] = 0;

			CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			if(pNotifications)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH_S");
				strSummary << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
				Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH");
				strNotification << GetSpyRankName(pSpy->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
				strNotification << pSpy->GetSpyName(m_pPlayer);
#else
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
				strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
				pNotifications->Add(NOTIFICATION_SPY_CANT_STEAL_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
		}
#if defined(MOD_BALANCE_CORE_SPIES)
		else if(MOD_BALANCE_CORE_SPIES && (pCityEspionage->HasReachedGoal(ePlayer)) && (pSpy->m_eSpyState != SPY_STATE_DEAD))
#else
		else if(pCityEspionage->HasReachedGoal(ePlayer))
#endif
		{
			HeistLocation kHeistLocation;
			kHeistLocation.m_iX = pCity->getX();
			kHeistLocation.m_iY = pCity->getY();
			m_aHeistLocations[eCityOwner].push_back(kHeistLocation);

			int iSpyResult;
			if(pCityEspionage->HasCounterSpy())
			{
				//Higher better for defense; lower better for offense.
				iSpyResult = GC.getGame().getJonRandNum(iRandomRollSpyAction, "Random roll for the result of a spy mission with a counterspy in the city");
				int iCounterspyIndex = GET_PLAYER(eCityOwner).GetEspionage()->GetSpyIndexInCity(pCity);
				iSpyResult += GET_PLAYER(eCityOwner).GetEspionage()->m_aSpyList[iCounterspyIndex].m_eRank * iSpyRankPower;
				iSpyResult *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
				iSpyResult /= 100;
				if (iSpyResult < iChancetoDetectCounterSpy /* 150 */)
				{
#if defined(MOD_EVENTS_ESPIONAGE)
					pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_DETECTED);
#else
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_DETECTED);
#endif
				}
				else if (iSpyResult < iChancetoIdentifyCounterSpy /* 250 */ )
				{
#if defined(MOD_EVENTS_ESPIONAGE)
					pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_IDENTIFIED);
#else
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_IDENTIFIED);
#endif
				}
				else
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
			else
			{
				iSpyResult = GC.getGame().getJonRandNum(iRandomRollSpyAction, "Random roll for the result of a spying mission without a counterspy in the city");
				iSpyResult *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
				iSpyResult /= 100;
				if (iSpyResult < iChancetoDetectNoCounterSpy /* 150 */ )
				{
#if defined(MOD_EVENTS_ESPIONAGE)
					pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_UNDETECTED);
#else
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_UNDETECTED);
#endif
				}
				else if (iSpyResult < iChancetoIdentifyNoCounterSpy /* 250 */ )
				{
#if defined(MOD_EVENTS_ESPIONAGE)
					pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_DETECTED);
#else
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_DETECTED);
#endif
				}
				else
				{
#if defined(MOD_EVENTS_ESPIONAGE)
					pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_IDENTIFIED);
#else
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_IDENTIFIED);
#endif
				}
			}

			CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(eCityOwner).GetEspionage();
			CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
			if(pDefendingPlayerEspionage)
			{
#if defined(MOD_BALANCE_CORE_SPIES)
				pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, NO_UNIT, false, 0, 0, false);
#else
				pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strSummary << pSpy->GetSpyName(m_pPlayer);
#else
					strSummary << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED");
					strNotification << GetSpyRankName(pSpy->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << pSpy->GetSpyName(m_pPlayer);
#else
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
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

				GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, (pSpy->m_eRank + 1), true, NO_PLAYER, NULL, false, pCity);

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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
#else
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << pSpy->GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
						strNotification << pCity->getNameKey();
						strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_SPY_STOLE_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, eCityOwner);
					}

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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
#else
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
#endif
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
#if defined(MOD_BALANCE_CORE)
		case SPY_STATE_PREPARING_HEIST:
			CvAssertMsg(pCity, "pCity not defined. What?");
			if (!pCity)
			{
				return;
			}
			BuildStealableGWList(eCityOwner);
			pCityEspionage->Process(ePlayer);
			// if the rate is too low, reassign the spy
			if (pCityEspionage->m_aiRate[m_pPlayer->GetID()] < 100)
			{
				pSpy->m_bEvaluateReassignment = true;
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Re-eval: potential too low, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
					LogEspionageMsg(strMsg);
				}
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
				if (MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->GetAdvancedActionsCooldown() <= (GC.getBALANCE_SPY_SABOTAGE_RATE() * 2))
				{
					pSpy->m_bEvaluateReassignment = false; // flag not to re-evaluate because high rank
					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Nevermind, staying because we're almost off cooldown: %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
						strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
						strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
						LogEspionageMsg(strMsg);
					}
				}
#endif
			}
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
			if (MOD_BALANCE_CORE_SPIES_ADVANCED)
			{
				if (!GET_PLAYER(eCityOwner).isMinorCiv())
				{
					AttemptAdvancedActions(uiSpyIndex);
				}
			}
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
			if (MOD_BALANCE_CORE_SPIES && (m_aPlayerStealableGWList[eCityOwner].size() == 0) && (pSpy->m_eSpyState != SPY_STATE_DEAD))
#else
			if (m_aPlayerStealableGWList[eCityOwner].size() == 0)
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
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Re-eval: m_aPlayerStealableGWList[eCityOwner].size() == 0, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
					LogEspionageMsg(strMsg);
				}
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
				if (MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->GetAdvancedActionsCooldown() <= (GC.getBALANCE_SPY_SABOTAGE_RATE() * 2))
				{
					pSpy->m_bEvaluateReassignment = false; // flag not to re-evaluate because high rank
					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Nevermind, staying because we're almost off cooldown: %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
						strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
						strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
						LogEspionageMsg(strMsg);
					}
				}
#endif

				int iRate = CalcPerTurn(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
				int iGoal = CalcRequired(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
				int iAmount = iGoal; // put the player back at surveillance levels
				pCityEspionage->SetActivity(ePlayer, iAmount, iRate, iGoal);

				// set last progress rate
				iRate = CalcPerTurn(SPY_STATE_PREPARING_HEIST, pCity, uiSpyIndex);
				pCityEspionage->SetLastProgress(ePlayer, iRate);

				// can't steal any techs from this civ
				m_aiNumGWToStealList[eCityOwner] = 0;

				CvNotifications* pNotifications = m_pPlayer->GetNotifications();
				if (pNotifications)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_GREAT_WORK_S");
					strSummary << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_GREAT_WORK");
					strNotification << GetSpyRankName(pSpy->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << pSpy->GetSpyName(m_pPlayer);
#else
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
					strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
					pNotifications->Add(NOTIFICATION_SPY_CANT_STEAL_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
				}
			}
#if defined(MOD_BALANCE_CORE_SPIES)
			else if (MOD_BALANCE_CORE_SPIES && (pCityEspionage->HasReachedGoal(ePlayer)) && (pSpy->m_eSpyState != SPY_STATE_DEAD))
#else
			else if (pCityEspionage->HasReachedGoal(ePlayer))
#endif
			{
				HeistLocation kHeistLocation;
				kHeistLocation.m_iX = pCity->getX();
				kHeistLocation.m_iY = pCity->getY();
				m_aHeistLocations[eCityOwner].push_back(kHeistLocation);

				int iSpyResult;
				if (pCityEspionage->HasCounterSpy())
				{
					iSpyResult = GC.getGame().getJonRandNum(300, "Random roll for the result of a spy mission with a counterspy in the city");
					int iCounterspyIndex = GET_PLAYER(eCityOwner).GetEspionage()->GetSpyIndexInCity(pCity);
					iSpyResult += GET_PLAYER(eCityOwner).GetEspionage()->m_aSpyList[iCounterspyIndex].m_eRank * iSpyRankPower;
					iSpyResult *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
					iSpyResult /= 100;
					if (iSpyResult < iChancetoDetectCounterSpy)
					{
#if defined(MOD_EVENTS_ESPIONAGE)
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_DETECTED);
#else
						pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_DETECTED);
#endif
					}
					else if (iSpyResult < iChancetoIdentifyCounterSpy)
					{
#if defined(MOD_EVENTS_ESPIONAGE)
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_IDENTIFIED);
#else
						pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_IDENTIFIED);
#endif
					}
					else
					{
#if defined(MOD_EVENTS_ESPIONAGE)
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_KILLED);
#else
						pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_KILLED);
#endif

					}
				}
				else
				{
					iSpyResult = GC.getGame().getJonRandNum(300, "Random roll for the result of a spying mission without a counterspy in the city");
					iSpyResult *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
					iSpyResult /= 100;
					if (iSpyResult < iChancetoDetectNoCounterSpy)
					{
#if defined(MOD_EVENTS_ESPIONAGE)
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_UNDETECTED);
#else
						pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_UNDETECTED);
#endif
					}
					else if (iSpyResult < iChancetoIdentifyNoCounterSpy)
					{
#if defined(MOD_EVENTS_ESPIONAGE)
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_DETECTED);
#else
						pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_DETECTED);
#endif
					}
					else
					{
#if defined(MOD_EVENTS_ESPIONAGE)
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_IDENTIFIED);
#else
						pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_IDENTIFIED);
#endif
					}
				}

				CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(eCityOwner).GetEspionage();
				CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
				if (pDefendingPlayerEspionage)
				{
#if defined(MOD_BALANCE_CORE_SPIES)
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, NO_UNIT, false, 0, 0, false);
#else
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH);
#endif
				}

				CvDiplomacyAI* pDefendingPlayerDiploAI = GET_PLAYER(eCityOwner).GetDiplomacyAI();
				CvAssertMsg(pDefendingPlayerDiploAI, "Defending player diplo AI is null");
				if (pDefendingPlayerDiploAI)
				{
					if (pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_IDENTIFIED || pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_KILLED)
					{
						pDefendingPlayerDiploAI->ChangeNumTimesRobbedBy(ePlayer, 1);
					}
				}

				// spy killed in action
				if (pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_KILLED)
				{
					CvNotifications* pNotifications = m_pPlayer->GetNotifications();
					if (pNotifications)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED_GW_S");
#if defined(MOD_BUGFIX_SPY_NAMES)
						strSummary << pSpy->GetSpyName(m_pPlayer);
#else
						strSummary << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED_GW");
						strNotification << GetSpyRankName(pSpy->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << pSpy->GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
						strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
						strNotification << pCity->getNameKey();
						pNotifications->Add(NOTIFICATION_SPY_WAS_KILLED, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);

					}

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

					// level up the defending spy
					int iDefendingSpy = pCityEspionage->m_aiSpyAssignment[eCityOwner];
					if (pDefendingPlayerEspionage)
					{
						CvAssertMsg(iDefendingSpy >= 0, "No defending spy. This is ok if debugging and killing a spy without having a defending spy present, but should not occur when playing the game normally.");
						if (iDefendingSpy >= 0)
						{
							pDefendingPlayerEspionage->LevelUpSpy(iDefendingSpy);
						}
					}

					GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, (pSpy->m_eRank + 1), true, NO_PLAYER, NULL, false, pCity);

					// kill spy off
					ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
#if defined(MOD_API_ESPIONAGE)
					pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_DEAD); // have to official kill him after the extraction
#else
					pSpy->m_eSpyState = SPY_STATE_DEAD; // have to official kill him after the extraction
#endif

					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Killed, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
						strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
#else
						strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
#endif
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
					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Re-eval: spy completed mission, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
						strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
						strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
						LogEspionageMsg(strMsg);
					}
					int iCityOwner = (int)eCityOwner;
					m_aiNumGWToStealList[iCityOwner] = m_aiNumGWToStealList[iCityOwner] + 1;
					pCityEspionage->ResetProgress(ePlayer);
					int iRate = CalcPerTurn(SPY_STATE_PREPARING_HEIST, pCity, uiSpyIndex);
					int iGoal = CalcRequired(SPY_STATE_PREPARING_HEIST, pCity, uiSpyIndex);
					pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);
					pCityEspionage->SetLastProgress(ePlayer, iRate);
					pCityEspionage->m_aiNumTimesCityRobbed[eCityOwner]++;

					if (pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_IDENTIFIED)
					{
						CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(eCityOwner).GetEspionageAI();
						CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
						if (pDefenderEspionageAI)
						{
							pDefenderEspionageAI->m_aiTurnLastSpyCaught[m_pPlayer->GetID()] = GC.getGame().getGameTurn();
							pDefenderEspionageAI->m_aiNumSpiesCaught[m_pPlayer->GetID()]++;
						}
					}

					// this check was added because m_aiNumGWToStealList was getting out of whack somehow and this is a check to prevent the UI from going haywire
					CvAssertMsg(m_aiNumGWToStealList[iCityOwner] > 0, "m_aiNumGWToStealList[iCityOwner] <= 0, which shouldn't happen after you succeed at stealing");
					if (m_aiNumGWToStealList[iCityOwner] > 0)
					{
						CvNotifications* pNotifications = m_pPlayer->GetNotifications();
						if (pNotifications)
						{
							Localization::String strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_STEAL_GW_S");
							Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STEAL_GW");
							strNotification << GetSpyRankName(pSpy->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
							strNotification << pSpy->GetSpyName(m_pPlayer);
#else
							strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
							strNotification << pCity->getNameKey();
							strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
							pNotifications->Add(NOTIFICATION_SPY_PROMOTION, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, 0);
						}
						m_pPlayer->GetEspionageAI()->StealGreatWork();
					}
					else
					{
						m_aiNumGWToStealList[iCityOwner] = 0;
					}

#if !defined(NO_ACHIEVEMENTS)
					//Achievements!
					if (m_pPlayer->GetID() == GC.getGame().getActivePlayer())
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_12);
					}
#endif

					LevelUpSpy(uiSpyIndex);

					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Starting heist, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
						strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
#else
						strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
#endif
						strMsg += ",";
						strMsg += ",";
						strMsg += ",";
						strMsg += GET_PLAYER(eCityOwner).getCivilizationShortDescription();
						strMsg += ",";
						strMsg += pCity->getName();
						strMsg += ",";
						switch (pCityEspionage->m_aiResult[ePlayer])
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
				if (pSpy->m_eSpyState != SPY_STATE_DEAD && pSpy->m_eSpyState != SPY_STATE_TERMINATED)
#else
				if (pSpy->m_eSpyState != SPY_STATE_DEAD)
#endif
				{
					UncoverIntrigue(uiSpyIndex);
				}
			}
			break;
#endif
	case SPY_STATE_RIG_ELECTION:
		// resetting the value is done in MinorCivAI
		pCityEspionage->Process(ePlayer);
		break;
	case SPY_STATE_COUNTER_INTEL:
		break;
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
#if defined(MOD_BUGFIX_SPY_NAMES)
			GetNextSpyName(pSpy);
#else
			pSpy->m_iName = GetNextSpyName();
#endif
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
#if defined(MOD_BALANCE_CORE_SPIES)
			pSpy->m_iTurnActivated = -1;
			pSpy->m_iActionsDone = -1;
			pSpy->m_iCooldown = -1;
#endif

			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: spy killed, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
				LogEspionageMsg(strMsg);
			}

			CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			if(pNotifications)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_REVIVED_S");
				Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_REVIVED");
				strNotification << GetSpyRankName(pSpy->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
				strNotification << pSpy->GetSpyName(m_pPlayer);
#else
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
				pNotifications->Add(NOTIFICATION_SPY_REPLACEMENT, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}

			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Respawned spy, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
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
bool CvPlayerEspionage::CanAdvancedAction(uint uiSpyIndex, CvCity* pCity, CvAdvancedAction eAdvancedAction)
{
	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	if (pSpy->GetAdvancedActionsCooldown() > 0)
	{
		if (GC.getLogging())
		{
			CvString strMsg;
			strMsg.Format("Advanced Action: On Hold. Cooldown Remaining: %d,", pSpy->GetAdvancedActionsCooldown());
			LogEspionageMsg(strMsg);
		}
		return false;
	}

	if (eAdvancedAction == ADVANCED_ACTION_FAILURE && m_pPlayer->GetCannotFailSpies())
		return false;

	if (pCity != NULL)
	{
		switch(eAdvancedAction)
		{
			case ADVANCED_ACTION_BUILDING:
			{
				if (pCity->GetBlockBuildingDestruction() > 0)
					return false;
			}
			case ADVANCED_ACTION_WW:
			{
				if (pCity->GetBlockWWDestruction() > 0)
					return false;
			}
			case ADVANCED_ACTION_UNIT:
			{
				if (pCity->GetBlockUDestruction() > 0)
					return false;
			}
			case ADVANCED_ACTION_GP:
			{
				if (pCity->GetBlockGPDestruction() > 0)
					return false;
			}
			case ADVANCED_ACTION_GOLD_THEFT:
			{
				if (pCity->GetBlockGold() > 0)
					return false;
			}
			case ADVANCED_ACTION_SCIENCE_THEFT:
			{
				if (pCity->GetBlockScience() > 0)
					return false;
			}
			case ADVANCED_ACTION_UNREST:
			{
				if (pCity->GetBlockUnrest() > 0)
					return false;
			}
			case ADVANCED_ACTION_REBELLION:
			{
				if (pCity->GetBlockRebellion() > 0)
					return false;
			}
		}
	}
	return true;
}
std::vector<CvAdvancedAction> CvPlayerEspionage::GetAdvancedActionPool(CvCity* pCity, BuildingTypes &eBuilding, UnitTypes &eUnit, SpecialistTypes &eBestSpecialist, int iRank)
{
	std::vector<CvAdvancedAction> aiAdvancedAction;

	eBuilding = pCity->getProductionBuilding();	

	//Production setback for non-wonders
	if (eBuilding != NO_BUILDING)
	{
		CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
		CvAssertMsg(pBuildingInfo, "pBuildingInfo is null");
		if (pBuildingInfo && !::isWorldWonderClass(pBuildingInfo->GetBuildingClassInfo()))
		{
			//Can we affect this building?
			if (pCity->getProduction() > 0)
			{
				for (int iAdvancedActionLoop = (4 - iRank); iAdvancedActionLoop < m_pPlayer->GetAdvancedActionBuilding(); iAdvancedActionLoop++)
				{
					aiAdvancedAction.push_back(ADVANCED_ACTION_BUILDING);
				}
			}
		}
		else if (pBuildingInfo && ::isWorldWonderClass(pBuildingInfo->GetBuildingClassInfo()))
		{
			//Can we affect this building?
			if ((pCity->getProduction() > 0))
			{
				for (int iAdvancedActionLoop = (5 - iRank); iAdvancedActionLoop < m_pPlayer->GetAdvancedActionWonder(); iAdvancedActionLoop++)
				{
					aiAdvancedAction.push_back(ADVANCED_ACTION_WW);
				}
			}
		}
	}

	eUnit = pCity->getProductionUnit();

	//Production setback for military units
	if (eUnit != NO_UNIT)
	{
		CvUnitEntry* pUnitInfo = GC.getUnitInfo(eUnit);
		if (pUnitInfo->GetCombat() > 0)
		{
			//Can we affect this unit?
			if (pCity->getProduction() > 0)
			{
				for (int iAdvancedActionLoop = (3 - iRank); iAdvancedActionLoop < m_pPlayer->GetAdvancedActionUnit(); iAdvancedActionLoop++)
				{
					aiAdvancedAction.push_back(ADVANCED_ACTION_UNIT);
				}
			}
		}
	}
	//GP setback
	UnitTypes eGreatUnit = NO_UNIT;
	UnitClassTypes eUnitClass = NO_UNITCLASS;
	
	eBestSpecialist = NO_SPECIALIST;

	int iBestRate = 0;
	int iRate = 0;
	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
		CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
		if (pkSpecialistInfo)
		{
			// Does this Specialist spawn a GP?
			if (pkSpecialistInfo->getGreatPeopleUnitClass() != NO_UNITCLASS)
			{
				iRate = pCity->GetCityCitizens()->GetSpecialistGreatPersonProgress(eSpecialist);
				if (iRate > iBestRate)
				{
					eUnitClass = (UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass();
					eGreatUnit = (UnitTypes)pCity->getCivilizationInfo().getCivilizationUnits(eUnitClass);
					if (eGreatUnit != NO_UNIT)
					{
						iBestRate = iRate;
						eBestSpecialist = eSpecialist;
					}
					
				}
			}
		}
	}
	if (iBestRate != 0 && eBestSpecialist != NO_SPECIALIST)
	{
		for (int iAdvancedActionLoop = (5 - iRank); iAdvancedActionLoop < m_pPlayer->GetAdvancedActionGP(); iAdvancedActionLoop++)
		{
			aiAdvancedAction.push_back(ADVANCED_ACTION_GP);
		}
	}
	//Only possible if very unhappy
	if (GET_PLAYER(pCity->getOwner()).IsEmpireUnhappy())
	{
		if (GET_PLAYER(pCity->getOwner()).IsEmpireVeryUnhappy())
		{
			for (int iAdvancedActionLoop = (7 - iRank); iAdvancedActionLoop < m_pPlayer->GetAdvancedActionRebellion(); iAdvancedActionLoop++)
			{
				aiAdvancedAction.push_back(ADVANCED_ACTION_REBELLION);
			}
		}
		for (int iAdvancedActionLoop = (6 - iRank); iAdvancedActionLoop < m_pPlayer->GetAdvancedActionUnrest(); iAdvancedActionLoop++)
		{
			aiAdvancedAction.push_back(ADVANCED_ACTION_UNREST);
		}
	}
	//Science
	for (int iAdvancedActionLoop = (2 - iRank); iAdvancedActionLoop < m_pPlayer->GetAdvancedActionScience(); iAdvancedActionLoop++)
	{
		aiAdvancedAction.push_back(ADVANCED_ACTION_SCIENCE_THEFT);
	}
	//Gold
	for (int iAdvancedActionLoop = (1 - iRank); iAdvancedActionLoop < m_pPlayer->GetAdvancedActionGold(); iAdvancedActionLoop++)
	{
		aiAdvancedAction.push_back(ADVANCED_ACTION_GOLD_THEFT);
	}
	//Failure (only one in pool)
	aiAdvancedAction.push_back(ADVANCED_ACTION_FAILURE);

	return aiAdvancedAction;
}

/// Advanced Action - Determine if the spy can do an advanced action not related to techs or intrigue.
void CvPlayerEspionage::AttemptAdvancedActions(uint uiSpyIndex)
{
	
	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);

	//Can this spy advanced action?
	if (!CanAdvancedAction(uiSpyIndex, NULL, ADVANCED_ACTION_NONE))
		return;
	
	CvPlot* pCityPlot = GC.getMap().plot(pSpy->m_iCityX, pSpy->m_iCityY);
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;
	PlayerTypes ePlayer = m_pPlayer->GetID();
	PlayerTypes eCityOwner = NO_PLAYER;
	int iRank = pSpy->m_eRank + 1;
	int iCityValue = 0;

	//Let's give spies a few new tricks, eh?
	if(pCityPlot)
	{
		pCity = pCityPlot->getPlotCity();
		if(pCity)
		{
			eCityOwner = pCity->getOwner();
			pCityEspionage = pCity->GetCityEspionage();
			iRank += pCity->GetRank();
			iCityValue = (CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex) / 1000);
			iRank += m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(pCity->getOwner());
			if (MOD_BALANCE_CORE_SPIES_ADVANCED && GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
			{
				iRank += 1;
			}
		}
	}
	if(eCityOwner != NO_PLAYER)
	{
		BuildingTypes eBuilding = NO_BUILDING;
		UnitTypes eUnit = NO_UNIT;
		SpecialistTypes eBestSpecialist = NO_SPECIALIST;

		std::vector<CvAdvancedAction> aiAdvancedAction = GetAdvancedActionPool(pCity, eBuilding, eUnit, eBestSpecialist, iRank);

		//Do we have at least something good in here?
		if(aiAdvancedAction.size() > 1)
		{
			int iSpyResult = GC.getGame().getJonRandNum(iRandomRollSpyAction, "Random roll for the result of an advanced spy action");
			if (iSpyResult <= 50)
				return;
			iSpyResult += (pCity->GetEspionageModifier() + GET_PLAYER(eCityOwner).GetEspionageModifier() * -1);
			iSpyResult *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
			iSpyResult /= 100;
			iSpyResult -= iCityValue;

			bool bCanDie = false;
			if (pCityEspionage->HasCounterSpy())
			{
				int iCounterspyIndex = GET_PLAYER(eCityOwner).GetEspionage()->GetSpyIndexInCity(pCity);

				iSpyResult += (GET_PLAYER(eCityOwner).GetEspionage()->m_aSpyList[iCounterspyIndex].m_eRank + 1) * iSpyRankPower;

				int iHeat = 2 - GET_PLAYER(eCityOwner).GetEspionage()->m_aSpyList[iCounterspyIndex].m_eRank;

				if (iHeat < pSpy->GetAdvancedActions())
				{
					bCanDie = true;
				}
			}
			
			if (bCanDie && iSpyResult > iChancetoKillAA /* 275 */ )
			{
				pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_KILLED);
			}
			else if (iSpyResult > iChancetoIdentifyAA /* 175 */)
			{
				pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_IDENTIFIED);
			}
			else
			{
				pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_DETECTED);
			}
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Advanced Action: Attempting advanced action. Roll: %d,", iSpyResult);
				strMsg += " , ";
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
#endif
				strMsg += " , ";
				if(pCity)
				{
					strMsg += GET_PLAYER(pCity->getOwner()).getCivilizationShortDescription();
					strMsg += " , ";
					strMsg += pCity->getName();
				}
				LogEspionageMsg(strMsg);
			}

			CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(eCityOwner).GetEspionage();
			CvDiplomacyAI* pDefendingPlayerDiploAI = GET_PLAYER(eCityOwner).GetDiplomacyAI();

			// spy killed in action
			if(pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_KILLED)
			{
				CvNotifications* pNotifications = m_pPlayer->GetNotifications();
				if(pNotifications)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED_S_ADVANCED");
#if defined(MOD_BUGFIX_SPY_NAMES)
					strSummary << pSpy->GetSpyName(m_pPlayer);
#else
					strSummary << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
					strSummary << pCity->getNameKey();
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED_ADVANCED");
					strNotification << GetSpyRankName(pSpy->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << pSpy->GetSpyName(m_pPlayer);
#else
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
					strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
					strNotification << pCity->getNameKey();
					pNotifications->Add(NOTIFICATION_SPY_WAS_KILLED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);

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
				}

				CvAssertMsg(pDefendingPlayerDiploAI, "Defending player diplo AI is null");
				if(pDefendingPlayerDiploAI)
				{
					if(pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_KILLED)
					{
						pDefendingPlayerDiploAI->ChangeNumTimesRobbedBy(ePlayer, 1);
					}
				}

				GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, (pSpy->m_eRank + 1), true, NO_PLAYER, NULL, false, pCity);

				// level up the defending spy
				int iDefendingSpy = pCityEspionage->m_aiSpyAssignment[eCityOwner];
				if(pDefendingPlayerEspionage)
				{
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, NO_UNIT, false, 0, 0, false);
					CvAssertMsg(iDefendingSpy >= 0, "No defending spy. This is ok if debugging and killing a spy without having a defending spy present, but should not occur when playing the game normally.");
					if(iDefendingSpy >= 0)
					{
						pDefendingPlayerEspionage->LevelUpSpy(iDefendingSpy);
					}
				}
				// kill spy off
				ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
				pSpy->m_eSpyState = SPY_STATE_DEAD; // have to official kill him after the extraction
				if(GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Advanced Action: Spy killed");
					strMsg += " , ";
#if defined(MOD_BUGFIX_SPY_NAMES)
					strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
#else
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
#endif
					strMsg += " , ";
					if(pCity)
					{
						strMsg += GET_PLAYER(pCity->getOwner()).getCivilizationShortDescription();
						strMsg += " , ";
						strMsg += pCity->getName();
					}
					LogEspionageMsg(strMsg);
				}
			}

			if(pCityEspionage->m_aiResult[ePlayer] != SPY_RESULT_KILLED) // spy successfully completed advanced task - chance of getting one of these below.
			{
				int iTest = GC.getGame().getJonRandNum(aiAdvancedAction.size(), "Randomizing Advanced Actions");
				CvAdvancedAction eAdvancedAction = aiAdvancedAction[iTest];

				DoAdvancedAction(uiSpyIndex, pCity, eAdvancedAction, iRank, eBuilding, eUnit, eBestSpecialist);
			}
		}
	}
}
void CvPlayerEspionage::DoAdvancedAction(uint uiSpyIndex, CvCity* pCity, CvAdvancedAction eAdvancedAction, int iRank, BuildingTypes eBuilding, UnitTypes eUnit, SpecialistTypes eSpecialist)
{
	CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(pCity->getOwner()).GetEspionage();
	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	PlayerTypes ePlayer = m_pPlayer->GetID();
	PlayerTypes eCityOwner = pCity->getOwner();

	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);

	if (pSpy == NULL || pCity == NULL)
		return;

	//If we can't do this, we've wasted our turn.
	if (!CanAdvancedAction(uiSpyIndex, pCity, eAdvancedAction))
	{
		DoAdvancedActionLogging(ADVANCED_ACTION_NONE, -1, pCity, uiSpyIndex);
		return;
	}

	//Result improved by duration of stay.
	int iTurnsActive = (GC.getGame().getGameTurn() - pSpy->GetSpyActiveTurn());
	if (iTurnsActive > (iRank * 15))
	{
		iTurnsActive = (iRank * 15);
	}

	bool bSuccess = false;
	bool bSuccessButNoRewards = false;
	int iSetback = 0;

	switch (eAdvancedAction)
	{
		case ADVANCED_ACTION_BUILDING:
		{
			iSetback = (pCity->getProduction() * ((GC.getBALANCE_SPY_SABOTAGE_RATE() * 2) + iTurnsActive)) / 100;
			if (iSetback > pCity->getProduction())
			{
				iSetback = pCity->getProduction();
			}
			if (iSetback > 0)
			{
				pCity->setProduction(pCity->getProduction() - iSetback);

				CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
				if (pDefendingPlayerEspionage)
				{
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, eBuilding, NO_UNIT, false, iSetback, 0, false);
				}

				bSuccess = true;
			}
			break;
		}
		case ADVANCED_ACTION_WW:
		{
			iSetback = ((pCity->getProduction() * (iTurnsActive + (GC.getBALANCE_SPY_SABOTAGE_RATE() * 2))) / 100);
			if (iSetback > pCity->getProduction())
			{
				iSetback = pCity->getProduction();
			}
			if (iSetback > 0)
			{
				pCity->setProduction(pCity->getProduction() - iSetback);

				CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
				if (pDefendingPlayerEspionage)
				{
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, eBuilding, NO_UNIT, false, iSetback, 0, false);
				}
				bSuccess = true;
			}
			break;
		}
		case ADVANCED_ACTION_GP:
		{
			iSetback = (pCity->GetCityCitizens()->GetSpecialistGreatPersonProgressTimes100(eSpecialist) * (iTurnsActive + (GC.getBALANCE_SPY_SABOTAGE_RATE() * 2))) / 100;
			if (iSetback > pCity->GetCityCitizens()->GetSpecialistGreatPersonProgressTimes100(eSpecialist))
			{
				iSetback = pCity->GetCityCitizens()->GetSpecialistGreatPersonProgressTimes100(eSpecialist);
			}
			if (iSetback > 0)
			{
				CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
				if (pkSpecialistInfo)
				{
					pCity->GetCityCitizens()->ChangeSpecialistGreatPersonProgressTimes100(eSpecialist, (iSetback * -1));

					UnitClassTypes eUnitClass = (UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass();
					UnitTypes eGreatUnit = (UnitTypes)pCity->getCivilizationInfo().getCivilizationUnits(eUnitClass);

					//For tooltip.
					iSetback /= 100;

					CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
					if (pDefendingPlayerEspionage)
					{
						pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, eGreatUnit, false, iSetback, 0, false);
					}
					bSuccess = true;
				}
			}
			break;
		}
		case ADVANCED_ACTION_UNIT:
		{
			iSetback = (pCity->getProduction() * (iTurnsActive * (GC.getBALANCE_SPY_SABOTAGE_RATE() * 2))) / 100;
			if (iSetback > pCity->getProduction())
			{
				iSetback = pCity->getProduction();
			}
			if (iSetback > 0)
			{
				pCity->setProduction(pCity->getProduction() - iSetback);

				CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
				if (pDefendingPlayerEspionage)
				{
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, eUnit, false, iSetback, 0, false);
				}
				bSuccess = true;
			}
			break;
		}
		case ADVANCED_ACTION_UNREST:
		{
			iSetback = ((pCity->getFood() * ((GC.getBALANCE_SPY_SABOTAGE_RATE() * 2) + iTurnsActive)) / 100);
			if (iSetback > pCity->getFood())
			{
				iSetback = pCity->getFood();
			}
			if (iSetback > 0)
			{
				pCity->setFood((pCity->getFood() - iSetback));
				pCity->ChangeResistanceTurns(max(1, iTurnsActive / 10));

				CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
				if (pDefendingPlayerEspionage)
				{
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, NO_UNIT, false, 0, 0, true);
				}
				bSuccess = true;
			}
			break;
		}
		case ADVANCED_ACTION_REBELLION:
		{
			iSetback = ((pCity->GetMaxHitPoints() * (GC.getBALANCE_SPY_SABOTAGE_RATE() + iTurnsActive)) / 100);
			if (iSetback > pCity->GetMaxHitPoints())
			{
				iSetback = pCity->GetMaxHitPoints();
			}
			pCity->setDamage(iSetback);

			//Rebels!
			// In hundreds
			int iNumRebels = (iRank * iTurnsActive); //Based on rank of spy.
			int iExtraRoll = (iRank * iTurnsActive); //1+ Rebels maximum
			iNumRebels += GC.getGame().getJonRandNum(iExtraRoll, "Rebel count rand roll");
			iNumRebels /= 100;
			int iNumRebelTotal = max(3, iNumRebels);
			if (iNumRebelTotal > 0)
			{
				GC.getGame().DoSpawnUnitsAroundTargetCity(BARBARIAN_PLAYER, pCity, iNumRebelTotal, false, false, false, false);

				CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
				if (pDefendingPlayerEspionage)
				{
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, NO_UNIT, true, iSetback, 0, false);
				}
				bSuccess = true;
			}
			break;
		}
		case ADVANCED_ACTION_GOLD_THEFT:
		{
			iSetback = ((GET_PLAYER(pCity->getOwner()).GetTreasury()->GetGold() * ((GC.getBALANCE_SPY_SABOTAGE_RATE() / 2) + iTurnsActive)) / 100);
			if (iSetback > GET_PLAYER(pCity->getOwner()).GetTreasury()->GetGold())
			{
				iSetback = GET_PLAYER(pCity->getOwner()).GetTreasury()->GetGold();
			}
			if (iSetback > 0)
			{
				GET_PLAYER(pCity->getOwner()).GetTreasury()->ChangeGold((iSetback * -1));
				m_pPlayer->GetTreasury()->ChangeGold(iSetback);

				CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
				if (pDefendingPlayerEspionage)
				{
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, NO_UNIT, false, iSetback, 0, false);
				}
				bSuccess = true;
			}
			break;
		}
		case ADVANCED_ACTION_SCIENCE_THEFT:
		{
			iSetback = ((GET_PLAYER(pCity->getOwner()).GetScience() * ((GC.getBALANCE_SPY_SABOTAGE_RATE() * 2) + iTurnsActive)) / 100);
			if (iSetback > GET_PLAYER(pCity->getOwner()).GetScience())
			{
				iSetback = GET_PLAYER(pCity->getOwner()).GetScience();
			}
			if (iSetback > 0)
			{
				TechTypes eCurrentTech = m_pPlayer->GetPlayerTechs()->GetCurrentResearch();
				if (eCurrentTech == NO_TECH)
				{
					m_pPlayer->changeOverflowResearch(iSetback);
				}
				else
				{
					GET_TEAM(GET_PLAYER(m_pPlayer->GetID()).getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, iSetback, m_pPlayer->GetID());
				}

				TechTypes eCurrentTech2 = GET_PLAYER(pCity->getOwner()).GetPlayerTechs()->GetCurrentResearch();
				if (eCurrentTech2 == NO_TECH)
				{
					GET_PLAYER(pCity->getOwner()).changeOverflowResearch(-iSetback);
				}
				else
				{
					GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech2, -iSetback, pCity->getOwner());
				}

				CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
				if (pDefendingPlayerEspionage)
				{
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, NO_UNIT, false, 0, iSetback, false);
				}
				bSuccess = true;
			}
			break;
		}
		case ADVANCED_ACTION_FAILURE:
		{
			CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
			if (pDefendingPlayerEspionage)
			{
				pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer], NO_TECH, NO_BUILDING, NO_UNIT, false, 0, 0, false);
			}

			if (pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_IDENTIFIED)
			{
				CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(eCityOwner).GetEspionageAI();
				CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
				if (pDefenderEspionageAI)
				{
					pDefenderEspionageAI->m_aiTurnLastSpyCaught[m_pPlayer->GetID()] = GC.getGame().getGameTurn();
					pDefenderEspionageAI->m_aiNumSpiesCaught[m_pPlayer->GetID()]++;
				}
			}
			pSpy->ChangeAdvancedActionsCooldown(iTurnsActive);

			bSuccessButNoRewards = true;
			bSuccess = true;
			break;
		}
	}

	if (bSuccess)
	{
		if (!bSuccessButNoRewards)
		{
			DoAdvancedActionLevelUp(eAdvancedAction, uiSpyIndex, pCity, iSetback);
			if (eAdvancedAction != ADVANCED_ACTION_FAILURE)
			{
				m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_SPY_ATTACK, false, NO_GREATPERSON, NO_BUILDING, 0);
			}
		}
		DoAdvancedActionNotification(eAdvancedAction, iSetback, pCity, uiSpyIndex);
		DoAdvancedActionLogging(eAdvancedAction, iSetback, pCity, uiSpyIndex);
	}
}
void CvPlayerEspionage::DoAdvancedActionLevelUp(CvAdvancedAction eAdvancedAction, uint uiSpyIndex, CvCity* pCity, int iPassValue)
{
	CvDiplomacyAI* pDefendingPlayerDiploAI = GET_PLAYER(pCity->getOwner()).GetDiplomacyAI();
	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	PlayerTypes ePlayer = m_pPlayer->GetID();

	if (pSpy == NULL || pCity == NULL)
		return;

	pSpy->ChangeAdvancedActions(1);
	m_aiNumSpyActionsDone[pCity->getOwner()]++;
	pCityEspionage->m_aiNumTimesCityRobbed[pCity->getOwner()]++;
	pSpy->ChangeAdvancedActionsCooldown(iPassValue * 8);

	CvAssertMsg(pDefendingPlayerDiploAI, "Defending player diplo AI is null");
	if (pDefendingPlayerDiploAI)
	{
		if (pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_IDENTIFIED)
		{
			pDefendingPlayerDiploAI->ChangeNumTimesRobbedBy(ePlayer, 1);
		}
	}

	if (pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_IDENTIFIED)
	{
		CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(pCity->getOwner()).GetEspionageAI();
		CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
		if (pDefenderEspionageAI)
		{
			pDefenderEspionageAI->m_aiTurnLastSpyCaught[m_pPlayer->GetID()] = GC.getGame().getGameTurn();
			pDefenderEspionageAI->m_aiNumSpiesCaught[m_pPlayer->GetID()]++;
		}
	}

	int iNewResult = GC.getGame().getJonRandNum(100, "Random roll for the result of an advanced-action spy mission");
	int iValueNeeded = 101;
	switch (eAdvancedAction)
	{
		case ADVANCED_ACTION_BUILDING:
		{
			iValueNeeded = 70;
			break;
		}
		case ADVANCED_ACTION_WW:
		{
			iValueNeeded = 60;
			break;
		}
		case ADVANCED_ACTION_UNIT:
		{
			iValueNeeded = 65;
			break;
		}
		case ADVANCED_ACTION_GP:
		{
			iValueNeeded = 65;
			break;
		}
		case ADVANCED_ACTION_GOLD_THEFT:
		{
			iValueNeeded = 90;
			break;
		}
		case ADVANCED_ACTION_SCIENCE_THEFT:
		{
			iValueNeeded = 90;
			break;
		}
		case ADVANCED_ACTION_UNREST:
		{
			iValueNeeded = 55;
			break;
		}
		case ADVANCED_ACTION_REBELLION:
		{
			iValueNeeded = 55;
			break;
		}
	}

	if (iNewResult > iValueNeeded)
	{
		LevelUpSpy(uiSpyIndex);
	}
}
void CvPlayerEspionage::DoAdvancedActionNotification(CvAdvancedAction eAdvancedAction, int iPassValue, CvCity* pCity, uint uiSpyIndex)
{
	CvNotifications* pNotifications = m_pPlayer->GetNotifications();
	if (pNotifications)
	{
		CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
		Localization::String strSummary;
		Localization::String strNotification;

		switch (eAdvancedAction)
		{
			case ADVANCED_ACTION_BUILDING:
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_BUILDING_S");
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_BUIDLING");
				break;
			}
			case ADVANCED_ACTION_WW:
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_WONDER_S");
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_WONDER");
				break;
			}
			case ADVANCED_ACTION_UNIT:
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_UNIT_S");
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_UNIT");
				break;
			}
			case ADVANCED_ACTION_GP:
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_GP_S");
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_GP");
				break;
			}
			case ADVANCED_ACTION_GOLD_THEFT:
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_GOLD_S");
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_GOLD");
				break;
			}
			case ADVANCED_ACTION_SCIENCE_THEFT:
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_SCIENCE_S");
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_SCIENCE");
				break;
			}
			case ADVANCED_ACTION_UNREST:
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_RESISTANCE_S");
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_RESISTANCE");
				break;
			}
			case ADVANCED_ACTION_REBELLION:
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_REBELLION_S");
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_REBELLION_RESISTANCE");
				break;
			}
			case ADVANCED_ACTION_FAILURE:
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_NOTHING_S");
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_SETBACK_NOTHING");
				break;
			}
		}

		strSummary << pCity->getNameKey();

		strNotification << GetSpyRankName(pSpy->m_eRank);
#if defined(MOD_BUGFIX_SPY_NAMES)
		strNotification << pSpy->GetSpyName(m_pPlayer);
#else
		strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
#endif
		strNotification << pCity->getNameKey();
		strNotification << iPassValue;

		pNotifications->Add(NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), pCity->getOwner());
	}
}
void CvPlayerEspionage::DoAdvancedActionLogging(CvAdvancedAction eAdvancedAction, int iPassValue, CvCity* pCity, uint uiSpyIndex)
{
	if (GC.getLogging())
	{
		CvString strMsg;
		CvString strAAMsg;
		strMsg += " , ";
#if defined(MOD_BUGFIX_SPY_NAMES)
		strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
#else
		strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
#endif

		switch (eAdvancedAction)
		{
			case ADVANCED_ACTION_BUILDING:
			{
				strAAMsg.Format("Advanced Action: Disrupted building. Production loss: %d,", iPassValue);
				break;
			}
			case ADVANCED_ACTION_WW:
			{
				strAAMsg.Format("Advanced Action: Sabotaged wonder. Damage: %d,", iPassValue);
				break;
			}
			case ADVANCED_ACTION_UNIT:
			{
				strAAMsg.Format("Advanced Action: Unit disrupted. Production: %d,", iPassValue);
				break;
			}
			case ADVANCED_ACTION_GP:
			{
				strAAMsg.Format("Advanced Action: Assassinated GP, Points taken: %d,", iPassValue);
				break;
			}
			case ADVANCED_ACTION_GOLD_THEFT:
			{
				strAAMsg.Format("Advanced Action: Gold stolen. Gold: %d,", iPassValue);
				break;
			}
			case ADVANCED_ACTION_SCIENCE_THEFT:
			{
				strMsg.Format("Advanced Action: Science pilfered. Science: %d,", iPassValue);
				break;
			}
			case ADVANCED_ACTION_REBELLION:
			{
				strAAMsg.Format("Advanced Action: Caused rebellion. Units: %d,", iPassValue);
				break;
			}
			case ADVANCED_ACTION_UNREST:
			{
				strAAMsg.Format("Advanced Action: Caused unrest, Food lost: %d,", iPassValue);
				break;
			}
			case ADVANCED_ACTION_FAILURE:
			{
				strAAMsg.Format("Advanced Action: Failed Last Advanced Action Test, City Owner Probably Alerted");
				break;
			}
			case ADVANCED_ACTION_NONE:
			{
				strAAMsg.Format("Advanced Action: Blocked by City or Player building");
				break;
			}
		}
		strMsg += strAAMsg;

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
CvString CvPlayerEspionage::GetSpyChanceAtCity(CvCity* pCity, uint uiSpyIndex, bool bNoBasic)
{
	if (uiSpyIndex == -1)
		return "";

	CvString strSpyAtCity = "";

	CvEspionageSpy* pSpy = &(m_aSpyList[uiSpyIndex]);
	if (!pSpy)
		return "";

	if (!bNoBasic)
	{
		switch (pSpy->m_eSpyState)
		{
		case SPY_STATE_UNASSIGNED:
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_UNASSIGNED_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));		
			break;
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
		case SPY_STATE_PREPARING_HEIST:
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_HEIST_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());		
			break;
#endif
		}
		strSpyAtCity += "[NEWLINE][NEWLINE]";
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

	//Defensive Spies:
	if (pCity != NULL && pCity->getOwner() == m_pPlayer->GetID())
	{
		if (pSpy->m_iCityX == pCity->getX() && pSpy->m_iCityY == pCity->getY())
		{
			int iSpeedBonuses = 0;
			int iCityEspMod = pCity->GetEspionageModifier() * -1;
			int iOurEspMod = (-1 * (GET_PLAYER(pCity->getOwner()).GetEspionageModifier() + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_SLOWER_MODIFIER)));

			iSpeedBonuses += iCityEspMod;
			iSpeedBonuses += iOurEspMod;

			int iUnhappinessMod = 0;
			if (MOD_BALANCE_CORE_HAPPINESS)
			{
				if (pCity->getPopulation() > 0)
				{
					iUnhappinessMod = (((pCity->getUnhappyCitizenCount()) * 10) / pCity->getPopulation());
					iUnhappinessMod *= -1;
				}
			}
			iSpeedBonuses += iUnhappinessMod;

			strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_BONUSES", iSpeedBonuses);
			if (iCityEspMod != 0)
				strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_CITY_MOD", iCityEspMod);
			if (iUnhappinessMod != 0)
				strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_UNHAPPINESS_MOD", iUnhappinessMod);
			if (iOurEspMod != 0)
				strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_OUR_MOD", iOurEspMod);

			if (pCity->GetCityReligions()->GetReligiousMajority() != NO_RELIGION)
			{
				int iSpyPressure = m_pPlayer->GetReligions()->GetSpyPressure(pCity->getOwner());
				if (iSpyPressure != 0)
				{
					strSpyAtCity += "[NEWLINE][NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_RELIGOUS_PRESSURE", iSpyPressure);
				}
			}

			strSpyAtCity += "[NEWLINE][NEWLINE]";

			strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_POTENTIAL");
			
			int iOurSpyPower = iRandomRollSpyAction;
			iOurSpyPower += (pSpy->m_eRank * iSpyRankPower);
			iOurSpyPower *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
			iOurSpyPower /= 100;

			int iKillChance = ((iOurSpyPower - iChancetoIdentifyCounterSpy) * 100) / iOurSpyPower;
			int iIdentifyChance = (((iChancetoIdentifyCounterSpy - iChancetoDetectCounterSpy) * 100) / iOurSpyPower);
			//Remainder
			int iDetectChance = (100 - iKillChance - iIdentifyChance);

			strSpyAtCity += "[NEWLINE]";
			strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_BONUSES_KILL", max(0, iKillChance));
			strSpyAtCity += "[NEWLINE]";
			strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_BONUSES_CATCH", max(0, iIdentifyChance));
			strSpyAtCity += "[NEWLINE]";
			strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_BONUSES_DETECT", max(0, iDetectChance));

			//City Rank:
			if (MOD_BALANCE_CORE_SPIES_ADVANCED)
			{
				strSpyAtCity += "[NEWLINE][NEWLINE]";

				strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_POTENTIAL_AA");

				int iSpyTotal = iRandomRollSpyAction;
				iSpyTotal += (100 + (pCity->GetEspionageModifier() + m_pPlayer->GetEspionageModifier() * -1));
				iSpyTotal *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
				iSpyTotal /= 100;
				int iCityValue = (CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex) / 1000);
				iSpyTotal += (pSpy->m_eRank + 1) * iSpyRankPower;
				iSpyTotal -= iCityValue;

				int iKillChance = (((iSpyTotal - iChancetoKillAA) * 100) / iSpyTotal);
				int iIdentifyChance = (((iChancetoKillAA - iChancetoIdentifyAA) * 100) / iSpyTotal);
				//Remainder
				int iDetectChance = (100 - iKillChance - iIdentifyChance);

				strSpyAtCity += "[NEWLINE]";
				strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_BONUSES_ADVANCED_ACTION_KILL", max(0, iKillChance));
				strSpyAtCity += "[NEWLINE]";
				strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_BONUSES_ADVANCED_ACTION_CATCH", max(0, iIdentifyChance));
				strSpyAtCity += "[NEWLINE]";
				strSpyAtCity += GetLocalizedText("TXT_KEY_DEFENSIVE_SPY_BONUSES_ADVANCED_ACTION_DETECT", max(0, iDetectChance));
			}
		}
	}
	//Offensive Spies:
	else if (pCity != NULL)
	{
		if (pSpy->m_iCityX == pCity->getX() && pSpy->m_iCityY == pCity->getY() && pSpy->m_eSpyState != SPY_STATE_TRAVELLING && pSpy->m_eSpyState != SPY_STATE_SURVEILLANCE)
		{
			if (GET_PLAYER(pCity->getOwner()).isMinorCiv())
			{
				strSpyAtCity += "[NEWLINE]";
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
						iOurVotes += (pCityEspionage->m_aiAmount[eEspionagePlayer] * (100 + GET_PLAYER(eEspionagePlayer).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER))) / 100;
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
			}
			else
			{
				int iSpeedBonuses = 0;
				int iCityEspMod = pCity->GetEspionageModifier();
				int iTheirEspMod = GET_PLAYER(pCity->getOwner()).GetEspionageModifier() + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_SLOWER_MODIFIER);
				int iOurEspMod = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER);

				iSpeedBonuses += iOurEspMod;
				iSpeedBonuses += iCityEspMod;
				iSpeedBonuses += iTheirEspMod;

				int iUnhappinessMod = 0;
				if (MOD_BALANCE_CORE_HAPPINESS)
				{
					if (pCity->getPopulation() > 0)
					{
						iUnhappinessMod = (((pCity->getUnhappyCitizenCount()) * 10) / pCity->getPopulation());
					}
				}

				iSpeedBonuses += iUnhappinessMod;

				int iSpyRank = pSpy->m_eRank;
				iSpyRank += m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(pCity->getOwner());
				if (MOD_BALANCE_CORE_SPIES_ADVANCED && GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
				{
					iSpyRank += 1;
				}
				int iSpyBonus = (GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT() * iSpyRank);
				iSpeedBonuses += iSpyBonus;

				strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES", iSpeedBonuses);
				if (iCityEspMod != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_CITY_MOD", iCityEspMod);
				if (iUnhappinessMod != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_UNHAPPINESS_MOD", iUnhappinessMod);
				if (iTheirEspMod != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_THEIR_MOD", iTheirEspMod);
				if (iOurEspMod != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_OUR_MOD", iOurEspMod);
				if (iSpyBonus != 0)
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_BONUSES_DETAIL_SPY_MOD", iSpyBonus);

				if (pCity->GetCityReligions()->GetReligiousMajority() != NO_RELIGION)
				{
					int iSpyPressure = m_pPlayer->GetReligions()->GetSpyPressure(pCity->getOwner());
					if (iSpyPressure != 0)
					{
						strSpyAtCity += "[NEWLINE][NEWLINE]";
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_RELIGOUS_PRESSURE", (iSpyPressure * GC.getGame().getGameSpeedInfo().getReligiousPressureAdjacentCity()));
					}
				}

				strSpyAtCity += "[NEWLINE][NEWLINE]";

				bool bCanDie = false;
				if (MOD_BALANCE_CORE_SPIES_ADVANCED)
				{				
					if (pCity->GetCityEspionage()->HasCounterSpy())
					{
						int iCounterspyIndex = GET_PLAYER(pCity->getOwner()).GetEspionage()->GetSpyIndexInCity(pCity);

						int iHeat = 2 - GET_PLAYER(pCity->getOwner()).GetEspionage()->m_aSpyList[iCounterspyIndex].m_eRank;

						if (iHeat < pSpy->GetAdvancedActions())
						{
							bCanDie = true;
						}
					}
					if (pSpy->GetAdvancedActionsCooldown() > 0)
					{
						int iAgents = (pSpy->GetAdvancedActionsCooldown() / max(1, GC.getBALANCE_SPY_SABOTAGE_RATE()));
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_ADVANCED_HUNTERS", max(1, iAgents), pSpy->GetSpyName(m_pPlayer));
					}
					else
					{
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_ADVANCED_ACTIONS_POSSIBLE", pSpy->GetSpyName(m_pPlayer));			
					}
					if (bCanDie)
					{
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_ADVANCED_DEATH_POSSIBILITY");
					}

					strSpyAtCity += "[NEWLINE][NEWLINE]";			
				}

				strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_RISKS");

				int iTheirSpyPower = iRandomRollSpyAction;
				if (pCity->GetCityEspionage()->HasCounterSpy())
				{
					int iCounterspyIndex = GET_PLAYER(pCity->getOwner()).GetEspionage()->GetSpyIndexInCity(pCity);
					iTheirSpyPower += GET_PLAYER(pCity->getOwner()).GetEspionage()->m_aSpyList[iCounterspyIndex].m_eRank * iSpyRankPower;
				}
				iTheirSpyPower *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
				iTheirSpyPower /= 100;

				if (pCity->GetCityEspionage()->HasCounterSpy())
				{
					strSpyAtCity += "[NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_COUNTERSPY_POSSIBLY_PRESENT");

					int iKillChance = (((iTheirSpyPower - iChancetoIdentifyCounterSpy) * 100) / iTheirSpyPower);
					int iIdentifyChance = (((iChancetoIdentifyCounterSpy - iChancetoDetectCounterSpy) * 100) / iTheirSpyPower);
					//Remainder
					int iDetectChance = (100 - iKillChance - iIdentifyChance);

					strSpyAtCity += "[NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_KILL", max(0, iKillChance));
					strSpyAtCity += "[NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_CATCH", max(0, iIdentifyChance));
					strSpyAtCity += "[NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_DETECT", max(0, iDetectChance));
				}
				else
				{
					int iIdentifyChance = (((iTheirSpyPower - iChancetoIdentifyNoCounterSpy) * 100) / iTheirSpyPower);
					int iDetectChance = (((iChancetoIdentifyNoCounterSpy - iChancetoDetectNoCounterSpy) * 100) / iTheirSpyPower);
					//Remainder
					int iUndetectChance = (100 - iIdentifyChance - iDetectChance);

					strSpyAtCity += "[NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_CATCH", max(0, iIdentifyChance));
					strSpyAtCity += "[NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_DETECT", max(0, iDetectChance));
					strSpyAtCity += "[NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_UNDETECTED", max(0, iUndetectChance));
				}

				//City Rank:
				if (MOD_BALANCE_CORE_SPIES_ADVANCED)
				{
					strSpyAtCity += "[NEWLINE][NEWLINE]";

					strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_RISKS_AA");

					int iSpyTotal = 300;
					iSpyTotal += (100 + (pCity->GetEspionageModifier() + m_pPlayer->GetEspionageModifier() * -1));
					iSpyTotal *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
					iSpyTotal /= 100;
					int iCityValue = (CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, uiSpyIndex) / 1000);
					if (pCity->GetCityEspionage()->HasCounterSpy())
					{
						int iCounterspyIndex = GET_PLAYER(pCity->getOwner()).GetEspionage()->GetSpyIndexInCity(pCity);
						iSpyTotal += GET_PLAYER(pCity->getOwner()).GetEspionage()->m_aSpyList[iCounterspyIndex].m_eRank * 100;
					}
					iSpyTotal -= iCityValue;

					if (bCanDie)
					{
						strSpyAtCity += "[NEWLINE]";
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_COUNTERSPY_POSSIBLY_PRESENT");

						int iKillChance = (((iSpyTotal - iChancetoKillAA) * 100) / iSpyTotal);
						int iIdentifyChance = (((iChancetoKillAA - iChancetoIdentifyAA) * 100) / iSpyTotal);
						//Remainder
						int iDetectChance = (100 - iKillChance - iIdentifyChance);

						strSpyAtCity += "[NEWLINE]";
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_ADVANCED_ACTION_KILL", max(0, iKillChance));
						strSpyAtCity += "[NEWLINE]";
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_ADVANCED_ACTION_CATCH", max(0, iIdentifyChance));
						strSpyAtCity += "[NEWLINE]";
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_ADVANCED_ACTION_DETECT", max(0, iDetectChance));
					}
					else
					{
						int iIdentifyChance = (((iSpyTotal - iChancetoIdentifyAA) * 100) / iSpyTotal);
						//Remainder
						int iDetectChance = (100 - iIdentifyChance);

						strSpyAtCity += "[NEWLINE]";
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_ADVANCED_ACTION_CATCH", max(0, iIdentifyChance));
						strSpyAtCity += "[NEWLINE]";
						strSpyAtCity += GetLocalizedText("TXT_KEY_OFFENSIVE_SPY_DANGER_ADVANCED_ACTION_DETECT", max(0, iDetectChance));
					}
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
		int iRank = pCity->GetRank();

		if (!bNoBasic)
		{
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_POTENTIAL_TT", pCity->getNameKey(), iRank);
		}
		else
		{
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_POTENTIAL_SHORT_TT", pCity->getNameKey(), iRank);
		}

		if (MOD_BALANCE_CORE_SPIES_ADVANCED)
		{
			strSpyAtCity += "[NEWLINE][NEWLINE]";
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_POTENTIAL_LIST_TT");
			if (GET_PLAYER(pCity->getOwner()).IsEmpireUnhappy())
			{
				if (pCity->GetBlockRebellion() <= 0)
				{
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_REBELLION");
				}
				if (pCity->GetBlockUnrest() <= 0)
				{
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_UNREST");
				}
			}
			if (pCity->GetBlockGPDestruction() <= 0)
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_GP_ASSASSINATION");
			}
			if (pCity->GetBlockWWDestruction() <= 0)
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_WONDER");
			}
			if (pCity->GetBlockBuildingDestruction() <= 0)
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_BUILDING");	
			}
			if (pCity->GetBlockUDestruction() <= 0)
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_UNIT");
			}
			if (pCity->GetBlockScience() <= 0)
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_SCIENCE");
			}
			if (pCity->GetBlockGold() <= 0)
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_GOLD");
			}
			
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_FAIL");
		}

		int iEspionageMod = GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER);
		if (iEspionageMod != 0)
		{
			strSpyAtCity += "[NEWLINE][NEWLINE]";
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_INCREASED_CHANCE_TO_CATCH_SPY", iEspionageMod);	
		}

		int iModifier = ((pCity->GetEspionageModifier() + m_pPlayer->GetEspionageModifier()) * -1);

		if (iModifier != 0)
		{
			strSpyAtCity += "[NEWLINE][NEWLINE]";
			strSpyAtCity += GetLocalizedText("TXT_KEY_CITY_DEFENSIVE_BONUSES", iModifier);
		}
	}
	else
	{
		int iRank = pCity->GetRank();;

		int iSpy = GetSpyIndexInCity(pCity);
		CvEspionageSpy* pSpy = &(m_aSpyList[iSpy]);

		if (pSpy != NULL && pSpy->m_eSpyState != SPY_STATE_TRAVELLING && pSpy->m_eSpyState != SPY_STATE_SURVEILLANCE)
		{

			if (pSpy != NULL)
			{
				iRank = (pSpy->m_eRank + 1);
			}

			iRank += m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(pCity->getOwner());
			if (MOD_BALANCE_CORE_SPIES_ADVANCED && GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
			{
				iRank += 1;
			}

			if (!bNoBasic)
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_POTENTIAL_TT", pCity->getNameKey(), iRank);
			}
			else
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_POTENTIAL_SHORT_TT", pCity->getNameKey(), iRank);
			}

			if (MOD_BALANCE_CORE_SPIES_ADVANCED)
			{
				strSpyAtCity += "[NEWLINE][NEWLINE]";
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_POTENTIAL_LIST_TT");

				BuildingTypes eBuilding;
				UnitTypes eUnit;
				SpecialistTypes eSpecialist;
				std::vector<CvAdvancedAction> aiAdvancedAction = GetAdvancedActionPool(pCity, eBuilding, eUnit, eSpecialist, iRank);

				int iTotalAdvancedActions = aiAdvancedAction.size();

				std::vector<int> aiAdvancedActionTotals;
				aiAdvancedActionTotals.resize(NUM_ADVANCED_ACTIONS);

				for (uint iAdvancedActionLoop = 0; iAdvancedActionLoop < aiAdvancedAction.size(); iAdvancedActionLoop++)
				{
					CvAdvancedAction eAdvancedAction = aiAdvancedAction[iAdvancedActionLoop];
					aiAdvancedActionTotals[(int)eAdvancedAction]++;
				}

				int iRemainder = 0;
				if (GET_PLAYER(pCity->getOwner()).IsEmpireUnhappy())
				{
					if (GET_PLAYER(pCity->getOwner()).IsEmpireVeryUnhappy() && pCity->GetBlockRebellion() <= 0)
					{
						int iPercent = (aiAdvancedActionTotals[(int)ADVANCED_ACTION_REBELLION] * 100 / iTotalAdvancedActions);
						iRemainder += iPercent;
						strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_REBELLION_OTHER", iPercent);
					}
					if (pCity->GetBlockUnrest() <= 0)
					{
						int iPercent = (aiAdvancedActionTotals[(int)ADVANCED_ACTION_UNREST] * 100 / iTotalAdvancedActions);
						iRemainder += iPercent;
						strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_UNREST_OTHER", iPercent);
					}
				}
				if (pCity->GetBlockGPDestruction() <= 0)
				{
					int iPercent = (aiAdvancedActionTotals[(int)ADVANCED_ACTION_GP] * 100 / iTotalAdvancedActions);
					iRemainder += iPercent;
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_GP_ASSASSINATION_OTHER", iPercent);
				}
				if (pCity->GetBlockWWDestruction() <= 0)
				{
					int iPercent = (aiAdvancedActionTotals[(int)ADVANCED_ACTION_WW] * 100 / iTotalAdvancedActions);
					iRemainder += iPercent;
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_WONDER_OTHER", iPercent);
				}
				if (pCity->GetBlockBuildingDestruction() <= 0)
				{
					int iPercent = (aiAdvancedActionTotals[(int)ADVANCED_ACTION_BUILDING] * 100 / iTotalAdvancedActions);
					iRemainder += iPercent;
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_BUILDING_OTHER", iPercent);
				}
				if (pCity->GetBlockUDestruction() <= 0)
				{
					int iPercent = (aiAdvancedActionTotals[(int)ADVANCED_ACTION_UNIT] * 100 / iTotalAdvancedActions);
					iRemainder += iPercent;
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_UNIT_OTHER", iPercent);
				}
				if (pCity->GetBlockScience() <= 0)
				{
					int iPercent = (aiAdvancedActionTotals[(int)ADVANCED_ACTION_SCIENCE_THEFT] * 100 / iTotalAdvancedActions);
					iRemainder += iPercent;
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_SCIENCE_OTHER", iPercent);
				}
				if (pCity->GetBlockGold() <= 0)
				{
					int iPercent = (aiAdvancedActionTotals[(int)ADVANCED_ACTION_GOLD_THEFT] * 100 / iTotalAdvancedActions);
					iRemainder += iPercent;
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_GOLD_OTHER", iPercent);
				}
				if (!m_pPlayer->IsCannotFailSpies())
				{
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_FAIL_OTHER", (100 - iRemainder));
				}
				else
				{
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_OWN_CITY_ADVANCED_FAIL_OTHER_NOTHING", (100 - iRemainder));
				}
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
		int iTempValue;
		uint uiTargetSlot = GC.getGame().getJonRandNum(aiMajorCivIndex.size(), "Randomizing aiMajorCivIndex list within UncoverIntrigue");
		iTempValue = aiMajorCivIndex[ui];
		aiMajorCivIndex[ui] = aiMajorCivIndex[uiTargetSlot];
		aiMajorCivIndex[uiTargetSlot] = iTempValue;
	}

	// go through to determine any intrigue
#if defined(MOD_BALANCE_CORE_SPIES)
	int iSpyRank = m_aSpyList[uiSpyIndex].m_eRank;
#endif
	// sending out a sneak attack
	for(uint ui = 0; ui < aiMajorCivIndex.size(); ui++)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes)aiMajorCivIndex[ui];
		// a player shouldn't target themselves for a sneak attack. That's strange.
		if(eTargetPlayer == eCityOwner)
		{
			continue;
		}

		// Don't tell other civs about what the shadow ai is thinking because that's incorrect information!
		if(GET_PLAYER(eCityOwner).isHuman())
		{
			continue;
		}

		CvAIOperation* pSneakAttackOperation = GET_PLAYER(eCityOwner).GetMilitaryAI()->GetSneakAttackOperation(eTargetPlayer);
		if(!pSneakAttackOperation)
		{
			continue;
		}

		CvCity* pTargetCity = NULL;
#if defined(MOD_BALANCE_CORE_SPIES)
#else
		int iSpyRank = m_aSpyList[uiSpyIndex].m_eRank;
#endif
		iSpyRank += m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eCityOwner);

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
		else
		{
			if(GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER(eTargetPlayer).getTeam()))
			{
				eRevealedTargetPlayer = eTargetPlayer;
			}
		}

		switch(pSneakAttackOperation->GetOperationType())
		{
		case AI_OPERATION_CITY_SNEAK_ATTACK:
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, eRevealedTargetPlayer, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_ARMY_SNEAK_ATTACK, uiSpyIndex, pTargetCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
			if (MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
			{
				int iNewResult = GC.getGame().getJonRandNum(100, "Random roll for the result of diplomatic intrigue");
				if(iNewResult >= 80)
				{
					LevelUpSpy(uiSpyIndex);
				}
			}
#endif
		}
		break;
		case AI_OPERATION_NAVAL_INVASION_SNEAKY:
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, eRevealedTargetPlayer, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK, uiSpyIndex, pTargetCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
			if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
			{
				int iNewResult = GC.getGame().getJonRandNum(100, "Random roll for the result of diplomatic intrigue");
				if(iNewResult >= 80)
				{
					LevelUpSpy(uiSpyIndex);
				}
			}
#endif
		}
#if defined(MOD_BALANCE_CORE)
		case AI_OPERATION_NAVAL_ONLY_CITY_ATTACK:
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, eRevealedTargetPlayer, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK, uiSpyIndex, pTargetCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
			if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
			{
				int iNewResult = GC.getGame().getJonRandNum(100, "Random roll for the result of diplomatic intrigue");
				if(iNewResult >= 80)
				{
					LevelUpSpy(uiSpyIndex);
				}
			}
#endif
		}
#endif
		break;
		}

		// If a sneak attack is reported, bust out of the loop
		break;
	}

	// building up an army
	if(!GET_PLAYER(eCityOwner).isHuman())
	{
		ArmyType eArmyType = GET_PLAYER(eCityOwner).GetMilitaryAI()->GetArmyBeingBuilt();
		if(eArmyType != NO_ARMY_TYPE)
		{
			switch(eArmyType)
			{
			case ARMY_TYPE_LAND:
				AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_ARMY, uiSpyIndex, pCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
				if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
				{
					int iNewResult = GC.getGame().getJonRandNum(100, "Random roll for the result of diplomatic intrigue");
					if(iNewResult >= 85)
					{
						LevelUpSpy(uiSpyIndex);
					}
				}
#endif
				break;
			case ARMY_TYPE_NAVAL_INVASION:
				AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY, uiSpyIndex, pCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
				if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
				{
					int iNewResult = GC.getGame().getJonRandNum(100, "Random roll for the result of diplomatic intrigue");
					if(iNewResult >= 85)
					{
						LevelUpSpy(uiSpyIndex);
					}
				}
#endif
				break;
			}
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

		MajorCivApproachTypes eSurfaceApproach = pTargetDiploAI->GetMajorCivApproach(eOtherOtherPlayer, true);
		MajorCivApproachTypes eHonestApproach = pTargetDiploAI->GetMajorCivApproach(eOtherOtherPlayer, false);

		// if the current approach is a dangerous approach
		if(eHonestApproach == MAJOR_CIV_APPROACH_DECEPTIVE || eHonestApproach == MAJOR_CIV_APPROACH_WAR)
		{
			// if the surface approach hides this
			if(eSurfaceApproach == MAJOR_CIV_APPROACH_FRIENDLY || eSurfaceApproach == MAJOR_CIV_APPROACH_NEUTRAL)
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
						int iNewResult = GC.getGame().getJonRandNum(100, "Random roll for the result of diplomatic intrigue");
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
						int iNewResult = GC.getGame().getJonRandNum(100, "Random roll for the result of diplomatic intrigue");
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
		AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, eBuilding, eProject, INTRIGUE_TYPE_CONSTRUCTING_WONDER, uiSpyIndex, pCity, true);
#if defined(MOD_BALANCE_CORE_SPIES_ADVANCED)
		if(MOD_BALANCE_CORE_SPIES_ADVANCED && pSpy->m_bIsDiplomat && (iSpyRank <= SPY_RANK_AGENT))
		{
			int iNewResult = GC.getGame().getJonRandNum(100, "Random roll for the result of diplomatic intrigue");
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
		int iTempValue;
		uint uiTargetSlot = GC.getGame().getJonRandNum(aiMajorCivIndex.size(), "Randomizing aiMajorCivIndex list within UncoverIntrigue");
		iTempValue = aiMajorCivIndex[ui];
		aiMajorCivIndex[ui] = aiMajorCivIndex[uiTargetSlot];
		aiMajorCivIndex[uiTargetSlot] = iTempValue;
	}
	// sending out a sneak attack
	for(uint ui = 0; ui < aiMajorCivIndex.size(); ui++)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes)aiMajorCivIndex[ui];
		// a player shouldn't target themselves for a sneak attack. That's strange.
		if(eTargetPlayer == eOtherPlayer)
		{
			continue;
		}

		// Don't tell other civs about what the shadow ai is thinking because that's incorrect information!
		if(GET_PLAYER(eOtherPlayer).isHuman())
		{
			continue;
		}

		CvAIOperation* pSneakAttackOperation = GET_PLAYER(eOtherPlayer).GetMilitaryAI()->GetSneakAttackOperation(eTargetPlayer);
		if(!pSneakAttackOperation)
		{
			continue;
		}

		CvCity* pTargetCity = NULL;

		CvPlot* pPlot = pSneakAttackOperation->GetTargetPlot();
		if(pPlot)
		{
			pTargetCity = pPlot->getPlotCity();
		}

		PlayerTypes eRevealedTargetPlayer = NO_PLAYER;
		eRevealedTargetPlayer = (PlayerTypes)MAX_MAJOR_CIVS; // hack to indicate that we shouldn't know the target due to our low spy rank

		if(GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER(eTargetPlayer).getTeam()))
		{
			eRevealedTargetPlayer = eTargetPlayer;
		}

		switch(pSneakAttackOperation->GetOperationType())
		{
		case AI_OPERATION_CITY_SNEAK_ATTACK:
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, eRevealedTargetPlayer, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_ARMY_SNEAK_ATTACK, uiSpyIndex, pTargetCity, true);
		}
		break;
		case AI_OPERATION_NAVAL_INVASION_SNEAKY:
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, eRevealedTargetPlayer, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK, uiSpyIndex, pTargetCity, true);
		}
		break;
		}

		// If a sneak attack is reported, bust out of the loop
		break;
	}

	// building up an army
	if(!GET_PLAYER(eOtherPlayer).isHuman())
	{
		ArmyType eArmyType = GET_PLAYER(eOtherPlayer).GetMilitaryAI()->GetArmyBeingBuilt();
		if(eArmyType != NO_ARMY_TYPE)
		{
			switch(eArmyType)
			{
			case ARMY_TYPE_LAND:
				AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_ARMY, uiSpyIndex, pCity, true);
				break;
			case ARMY_TYPE_NAVAL_INVASION:
				AddIntrigueMessage(m_pPlayer->GetID(), eOtherPlayer, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY, uiSpyIndex, pCity, true);
				break;
			}
		}
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

		MajorCivApproachTypes eSurfaceApproach = pTargetDiploAI->GetMajorCivApproach(eOtherOtherPlayer, true);
		MajorCivApproachTypes eHonestApproach = pTargetDiploAI->GetMajorCivApproach(eOtherOtherPlayer, false);

		// if the current approach is a dangerous approach
		if(eHonestApproach == MAJOR_CIV_APPROACH_DECEPTIVE || eHonestApproach == MAJOR_CIV_APPROACH_WAR)
		{
			// if the surface approach hides this
			if(eSurfaceApproach == MAJOR_CIV_APPROACH_FRIENDLY || eSurfaceApproach == MAJOR_CIV_APPROACH_NEUTRAL)
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
#if defined(MOD_BUGFIX_SPY_NAMES)
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
#endif

#if defined(MOD_BUGFIX_SPY_NAMES)
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
	int iCivOffset = GC.getGame().getJonRandNum(iMaxCivs, "Civ offset");

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
	int iPlayerOffset = GC.getGame().getJonRandNum(MAX_MAJOR_CIVS, "Player offset");

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
#else
int CvPlayerEspionage::GetNextSpyName()
{
	CvAssertMsg((uint)m_iSpyListNameOrderIndex < m_aiSpyListNameOrder.size(), "m_iSpyListNameOrderIndex out of bounds probably because this civ doesn't have spy names.")
	if((uint)m_iSpyListNameOrderIndex >= m_aiSpyListNameOrder.size())
	{
		return -1;
	}

	int iName = m_aiSpyListNameOrder[m_iSpyListNameOrderIndex];
	m_iSpyListNameOrderIndex++;
	if(m_iSpyListNameOrderIndex >= (int)m_aiSpyListNameOrder.size())
	{
		m_iSpyListNameOrderIndex = 0;
	}
	return iName;
}
#endif

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
#if defined(MOD_BALANCE_CORE)
bool CvPlayerEspionage::CanMoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat, bool bAsThief)
#else
bool CvPlayerEspionage::CanMoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat)
#endif
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
#if defined(MOD_BALANCE_CORE)
	if (MOD_BALANCE_CORE_SPIES_ADVANCED && bAsThief)
	{
		if (pCity->isCapital())
			return false;

		if (GET_PLAYER(pCity->getOwner()).isMinorCiv())
			return false;
	}
#endif

	return true;
}

/// MoveSpyTo - Move a spy into this city
#if defined(MOD_BALANCE_CORE)
bool CvPlayerEspionage::MoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat, bool bAsThief)
#else
bool CvPlayerEspionage::MoveSpyTo(CvCity* pCity, uint uiSpyIndex, bool bAsDiplomat)
#endif
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	//Workaround for netcode.
	if (MOD_BALANCE_CORE_SPIES_ADVANCED && bAsDiplomat && m_pPlayer->isHuman() && pCity && !pCity->isCapital())
	{
		bAsThief = true;
		bAsDiplomat = false;
	}
#endif
	if (!CanMoveSpyTo(pCity, uiSpyIndex, bAsDiplomat, bAsThief))
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

		m_aSpyList[uiSpyIndex].m_iCityX = pCity->getX();
		m_aSpyList[uiSpyIndex].m_iCityY = pCity->getY();
		pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] = uiSpyIndex;
#if defined(MOD_API_ESPIONAGE)
		m_aSpyList[uiSpyIndex].SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_TRAVELLING);
#else
		m_aSpyList[uiSpyIndex].m_eSpyState = SPY_STATE_TRAVELLING;
#endif
		m_aSpyList[uiSpyIndex].m_bIsDiplomat = bAsDiplomat;
#if defined(MOD_BALANCE_CORE)
		if (MOD_BALANCE_CORE_SPIES_ADVANCED)
		{
			m_aSpyList[uiSpyIndex].m_bIsThief = bAsThief;
		}
#endif
		int iRate = CalcPerTurn(SPY_STATE_TRAVELLING, pCity, uiSpyIndex);
		int iGoal = CalcRequired(SPY_STATE_TRAVELLING, pCity, uiSpyIndex);
		pCityEspionage->SetActivity(m_pPlayer->GetID(), 0, iRate, iGoal);
		if (GC.getLogging() && bAsThief)
		{
			CvString strMsg;
			strMsg.Format("HEIST TIME: Moving Spy to City steal art: %s,", pCity->getNameKey());
			LogEspionageMsg(strMsg);
		}
	}

	if(GC.getLogging())
	{
		CvString strMsg;
		strMsg.Format("Moving spy, %d,", uiSpyIndex);
#if defined(MOD_BUGFIX_SPY_NAMES)
		strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
#else
		strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
			const char* szSpyName = m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
			const char* szSpyName = m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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

/// CalcPerTurn - How much the spy will do at this task per turn
int CvPlayerEspionage::CalcPerTurn(int iSpyState, CvCity* pCity, int iSpyIndex)
{
	CvAssertMsg(pCity, "pCity is null");
	switch(iSpyState)
	{
	case SPY_STATE_TRAVELLING:
	{
		return 1;
	}
	break;
	case SPY_STATE_SURVEILLANCE:
	{
		return 1;
	}
	break;
	case SPY_STATE_GATHERING_INTEL:
	{
		if(pCity)
		{
			PlayerTypes eCityOwner = pCity->getOwner();
#if defined(MOD_BALANCE_CORE_SPIES)
			int iBaseYieldRate = pCity->getYieldRateTimes100(YIELD_SCIENCE, false);
			iBaseYieldRate += pCity->getYieldRateTimes100(YIELD_GOLD, false);
#else
			int iBaseYieldRate = pCity->getYieldRateTimes100(YIELD_SCIENCE, false);
#endif	
			iBaseYieldRate *= GC.getESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT();
			iBaseYieldRate *= GC.getGame().getGameSpeedInfo().getSpyRatePercent();
			iBaseYieldRate /= 10000;
			int iCityEspionageModifier = pCity->GetEspionageModifier();
			int iPlayerEspionageModifier = GET_PLAYER(eCityOwner).GetEspionageModifier();
			int iTheirPoliciesEspionageModifier = GET_PLAYER(eCityOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_SLOWER_MODIFIER);
			int iMyPoliciesEspionageModifier = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER);
#if defined(MOD_BALANCE_CORE_SPIES)
			int iUnhappinessMod = 0;
			if(pCity->getPopulation() > 0)
			{
				iUnhappinessMod = (((pCity->getUnhappyCitizenCount()) * 10) / pCity->getPopulation());
			}
			int iFinalModifier = (iBaseYieldRate * (100 + iUnhappinessMod + iCityEspionageModifier + iPlayerEspionageModifier + iTheirPoliciesEspionageModifier + iMyPoliciesEspionageModifier)) / 100;
#else
			int iFinalModifier = (iBaseYieldRate * (100 + iCityEspionageModifier + iPlayerEspionageModifier + iTheirPoliciesEspionageModifier + iMyPoliciesEspionageModifier)) / 100;
#endif

			int iResult = max(iFinalModifier, 1);
			if(iSpyIndex >= 0)
			{
				int iSpyRank = m_aSpyList[iSpyIndex].m_eRank;
				iSpyRank += m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eCityOwner);
				if (MOD_BALANCE_CORE_SPIES_ADVANCED && GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
				{
					iSpyRank += 1;
				}
				iResult *= 100 + (GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT() * iSpyRank);
				iResult /= 100;
			}

			return iResult;
		}
	}
	break;
#if defined(MOD_BALANCE_CORE)
	case SPY_STATE_PREPARING_HEIST:
	{
		if (pCity)
		{
			PlayerTypes eCityOwner = pCity->getOwner();
#if defined(MOD_BALANCE_CORE_SPIES)
			int iBaseYieldRate = pCity->getYieldRateTimes100(YIELD_CULTURE, false);
			iBaseYieldRate += pCity->getYieldRateTimes100(YIELD_GOLD, false);
#else
			int iBaseYieldRate = pCity->getYieldRateTimes100(YIELD_SCIENCE, false);
#endif	
			iBaseYieldRate *= GC.getESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT();
			iBaseYieldRate *= GC.getGame().getGameSpeedInfo().getSpyRatePercent();
			iBaseYieldRate /= 10000;
			int iCityEspionageModifier = pCity->GetEspionageModifier();
			int iPlayerEspionageModifier = GET_PLAYER(eCityOwner).GetEspionageModifier();
			int iTheirPoliciesEspionageModifier = GET_PLAYER(eCityOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_GW_SLOWER_MODIFIER);
			int iMyPoliciesEspionageModifier = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_GW_FASTER_MODIFIER);
#if defined(MOD_BALANCE_CORE_SPIES)
			int iUnhappinessMod = 0;
			if (pCity->getPopulation() > 0)
			{
				iUnhappinessMod = (((pCity->getUnhappyCitizenCount()) * 10) / pCity->getPopulation());
			}
			int iFinalModifier = (iBaseYieldRate * (100 + iUnhappinessMod + iCityEspionageModifier + iPlayerEspionageModifier + iTheirPoliciesEspionageModifier + iMyPoliciesEspionageModifier)) / 100;
#else
			int iFinalModifier = (iBaseYieldRate * (100 + iCityEspionageModifier + iPlayerEspionageModifier + iTheirPoliciesEspionageModifier + iMyPoliciesEspionageModifier)) / 100;
#endif

			int iResult = max(iFinalModifier, 1);
			if (iSpyIndex >= 0)
			{
				int iSpyRank = m_aSpyList[iSpyIndex].m_eRank;
				iSpyRank += m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eCityOwner);
				if (MOD_BALANCE_CORE_SPIES_ADVANCED && GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
				{
					iSpyRank += 1;
				}
				iResult *= 100 + (GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT() * iSpyRank);
				iResult /= 100;
			}

			return iResult;
		}
	}
	break;
#endif
	case SPY_STATE_RIG_ELECTION:
	{
		if(pCity)
		{
			int iResult = 1;
			if(iSpyIndex >= 0)
			{
				int iSpyRank = m_aSpyList[iSpyIndex].m_eRank;
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
		return 1;
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
int CvPlayerEspionage::CalcRequired(int iSpyState, CvCity* pCity, int iSpyIndex)
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

			CvAssertMsg(m_aiMaxTechCost[ePlayer] >= 0, "m_aiMaxTechCost[ePlayer] is below zero");
			uint uiMaxTechCostAdjusted = m_aiMaxTechCost[ePlayer];			
			uiMaxTechCostAdjusted *= GC.getESPIONAGE_GATHERING_INTEL_COST_PERCENT();
			uiMaxTechCostAdjusted /= 100;
#if defined(MOD_BALANCE_CORE)
			if (GET_PLAYER(ePlayer).GetCurrentEra() <= (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true /*bHideAssert*/))
			{
				uiMaxTechCostAdjusted *= (100 + GC.getOPEN_BORDERS_MODIFIER_TRADE_GOLD());
				uiMaxTechCostAdjusted /= 100;
			}
			if(GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
			{
				uiMaxTechCostAdjusted *= (100 - GC.getOPEN_BORDERS_MODIFIER_TRADE_GOLD());
				uiMaxTechCostAdjusted /= 100;
			}
#endif
			int iMaxTechCostAdjusted = uiMaxTechCostAdjusted;
			CvAssertMsg(m_aiMaxTechCost[ePlayer] >= 0, "iMaxTechCostAdjusted is below zero. Overflow!");
			return iMaxTechCostAdjusted;
		}
	}
	break;
#if defined(MOD_BALANCE_CORE)
	case SPY_STATE_PREPARING_HEIST:
	{
		if (pCity)
		{
			PlayerTypes ePlayer = pCity->getOwner();
			CvAssertMsg(!GET_PLAYER(ePlayer).isMinorCiv(), "Can't spy on a minor civ");
			CvAssertMsg(!GET_PLAYER(ePlayer).isBarbarian(), "Can't spy on barbarians");
			if (GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).isBarbarian())
			{
				return -1;
			}

			CvAssertMsg(m_aiMaxGWCost[ePlayer] >= 0, "m_aiMaxGWCost[ePlayer] is below zero");
			uint uiMaxGWAdjusted = m_aiMaxGWCost[ePlayer];
			uiMaxGWAdjusted *= GC.getESPIONAGE_GATHERING_INTEL_COST_PERCENT();
			uiMaxGWAdjusted /= 100;
#if defined(MOD_BALANCE_CORE)
			if (GET_PLAYER(ePlayer).GetCurrentEra() <= (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true /*bHideAssert*/))
			{
				uiMaxGWAdjusted *= (100 + GC.getOPEN_BORDERS_MODIFIER_TRADE_GOLD());
				uiMaxGWAdjusted /= 100;
			}
			if (GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
			{
				uiMaxGWAdjusted *= (100 - GC.getOPEN_BORDERS_MODIFIER_TRADE_GOLD());
				uiMaxGWAdjusted /= 100;
			}
#endif
			int iMaxGWCostAdjusted = uiMaxGWAdjusted;
			CvAssertMsg(m_aiMaxGWCost[ePlayer] >= 0, "iMaxTechCostAdjusted is below zero. Overflow!");
			return iMaxGWCostAdjusted;
		}
	}
	break;
#endif
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
	else if (m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_RIG_ELECTION || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_PREPARING_HEIST)
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

#if defined(MOD_BALANCE_CORE)
bool CvPlayerEspionage::IsThief(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	return m_aSpyList[uiSpyIndex].m_bIsThief;
}
bool CvPlayerEspionage::IsPreparingHeist(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return false;
	}

	if (m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_PREPARING_HEIST)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CvPlayerEspionage::IsAnyPreparingHeist(CvCity* pCity)
{
	for (uint uiSpy = 0; uiSpy < m_aSpyList.size(); uiSpy++)
	{
		// can't have established surveillance when not in a city
		if (!IsSpyInCity(uiSpy))
		{
			continue;
		}

		CvPlot* pPlot = GC.getMap().plot(m_aSpyList[uiSpy].m_iCityX, m_aSpyList[uiSpy].m_iCityY);
		CvAssertMsg(pPlot, "Spy coordinates did not point to plot");
		if (!pPlot)
		{
			return false;
		}
		CvCity* pPlotCity = pPlot->getPlotCity();
		CvAssertMsg(pPlotCity, "There is not a city at the specified plot");
		if (!pPlotCity)
		{
			return false;
		}
		if (pCity == pPlotCity && IsPreparingHeist(uiSpy))
		{
			return true;
		}
	}

	return false;
}
#endif
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
		iAllySpyRank = GET_PLAYER(eAllyPlayer).GetEspionage()->m_aSpyList[iAllySpyIndex].m_eRank;
		bNoAllySpy = true;
	}

	int iAllyInfluence = pMinorCivAI->GetEffectiveFriendshipWithMajorTimes100(eAllyPlayer);
	int iMyInfluence = pMinorCivAI->GetEffectiveFriendshipWithMajorTimes100(m_pPlayer->GetID());
	int iDeltaInfluence = iAllyInfluence - iMyInfluence;
#if defined(MOD_BALANCE_CORE)
	iDeltaInfluence /= 2;
	if(iDeltaInfluence >  7000)
	{
		iDeltaInfluence = 7000;
	}
#endif
	//float fNobodyBonus = 0.5;
	//float fMultiplyConstant = 3.0f;
	//float fSpyLevelDeltaZero = 0.0f;
	//float fSpyLevelDeltaOne = 1.5f;
	//float fSpyLevelDeltaTwo = 2.25;
	//float fSpyLevelDeltaThree = 3.0f;

	float fNobodyBonus = GC.getESPIONAGE_COUP_NOBODY_BONUS();
	float fMultiplyConstant = GC.getESPIONAGE_COUP_MULTIPLY_CONSTANT();
	float fSpyLevelDeltaZero = GC.getESPIONAGE_COUP_SPY_LEVEL_DELTA_ZERO();
	float fSpyLevelDeltaOne = GC.getESPIONAGE_COUP_SPY_LEVEL_DELTA_ONE();
	float fSpyLevelDeltaTwo = GC.getESPIONAGE_COUP_SPY_LEVEL_DELTA_TWO();
	float fSpyLevelDeltaThree = GC.getESPIONAGE_COUP_SPY_LEVEL_DELTA_THREE();
	float fSpyLevelDeltaFour = GC.getESPIONAGE_COUP_SPY_LEVEL_DELTA_FOUR();

	float fAllySpyValue = 0.0f;
	float fMySpyValue = 0.0;

	int iMySpyRank = m_aSpyList[uiSpyIndex].m_eRank;
	iMySpyRank += m_pPlayer->GetCulture()->GetInfluenceCityStateSpyRankBonus(eCityOwner);
	switch (iMySpyRank)
	{
	case 0:
		fMySpyValue = fSpyLevelDeltaZero;
		break;
	case 1:
		fMySpyValue = fSpyLevelDeltaOne;
		break;
	case 2:
		fMySpyValue = fSpyLevelDeltaTwo;
		break;
	case 3:
		fMySpyValue = fSpyLevelDeltaThree;
		break;
	case 4:
		fMySpyValue = fSpyLevelDeltaFour;
		break;
	}

	switch (iAllySpyRank)
	{
	case 0:
		fAllySpyValue = fSpyLevelDeltaZero;
		break;
	case 1:
		fAllySpyValue = fSpyLevelDeltaOne;
		break;
	case 2:
		fAllySpyValue = fSpyLevelDeltaTwo;
		break;
	}	

	float fSpyMultipier = fAllySpyValue - fMySpyValue + fMultiplyConstant;
	if (bNoAllySpy)
	{
		fSpyMultipier *= fNobodyBonus;
	}

	int iResultPercentage = 100 - (int)((iDeltaInfluence * fSpyMultipier) / 100);


#if defined(MOD_BALANCE_CORE)
	if (MOD_BALANCE_CORE_SPIES_ADVANCED)
	{
		iResultPercentage *= (100 + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER));
		iResultPercentage /= 100;
	}
#endif

	if(iResultPercentage > 70)
	{
		iResultPercentage = 70;
	}
#if defined(MOD_BALANCE_CORE)
	else if(iResultPercentage < 5)
	{
		iResultPercentage = 5;
	}
#else
	else if(iResultPercentage < 0)
	{
		iResultPercentage = 0;
	}
#endif
	//int iAdjustedAllyInfluenceTimes100 = iAllyInfluence * (100 + m_aSpyList[uiSpyIndex].m_eRank * 100);
	//int iAdjustedAllyInfluence = iAdjustedAllyInfluenceTimes100 / 100;
	//int iResultPercentage = 0;
	//if (iAdjustedAllyInfluence != 0)
	//{
	//	iResultPercentage = 100 - ((iDeltaInfluence * 100) / iAdjustedAllyInfluence);
	//}

	return iResultPercentage;
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
#if defined(MOD_BUGFIX_SPY_NAMES)
		strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
#else
		strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
#endif
		LogEspionageMsg(strMsg);
	}

	bool bAttemptSuccess = false;
	int iRandRoll = GC.getGame().getJonRandNum(100, "Roll for the result of an attempted coup");
	if(iRandRoll <= GetCoupChanceOfSuccess(uiSpyIndex))
	{
		// swap influence from ally to 2nd place ally
		int iInfluenceTemp = aiNewInfluenceValueTimes100[ePreviousAlly];
		aiNewInfluenceValueTimes100[ePreviousAlly] = aiNewInfluenceValueTimes100[m_pPlayer->GetID()];
		aiNewInfluenceValueTimes100[m_pPlayer->GetID()] = iInfluenceTemp;

		// reduce the influence of all the other players
		for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			if(ui == m_pPlayer->GetID())
			{
				continue;
			}

			// only drop the influence if they have positive influence
			if(aiNewInfluenceValueTimes100[ui] > 0)
			{
				int iNewInfluence = aiNewInfluenceValueTimes100[ui] - (GC.getESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP() * 100);
				iNewInfluence = max(iNewInfluence, 0);
				aiNewInfluenceValueTimes100[ui] = iNewInfluence;

				GET_PLAYER((PlayerTypes)ui).GetDiplomacyAI()->ChangeNumTimesTheyLoweredOurInfluence(m_pPlayer->GetID(), 1);
				GET_PLAYER((PlayerTypes)ui).GetDiplomacyAI()->ChangeNumTimesTheyPlottedAgainstUs(m_pPlayer->GetID(), 1);
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
#if defined(MOD_BUGFIX_SPY_NAMES)
			strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
			strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
			strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
			strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BALANCE_CORE)
	case SPY_STATE_PREPARING_HEIST:
#endif
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
#if defined(MOD_BALANCE_CORE)
	case SPY_STATE_PREPARING_HEIST:
#endif
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

	int iMaxTechCost = -1;

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

		// try to find the most expensive tech that can be researched
		int iTechCost = m_pPlayer->GetPlayerTechs()->GetResearchCost(eTech) * 100;
		if(iTechCost > iMaxTechCost)
		{
			iMaxTechCost = iTechCost;
		}
	}

	m_aiMaxTechCost[ePlayer] = iMaxTechCost;
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
#if defined(MOD_BALANCE_CORE)
/// BuildStealableGWList - Go through opponents list and see what techs you can steal from them.
void CvPlayerEspionage::BuildStealableGWList(PlayerTypes ePlayer)
{
	CvAssertMsg((uint)ePlayer < m_aPlayerStealableGWList.size(), "ePlayer out of bounds");
	if ((uint)ePlayer >= m_aPlayerStealableGWList.size())
	{
		return;
	}

	m_aPlayerStealableGWList[ePlayer].clear();

	if (!GET_PLAYER(ePlayer).isAlive())
	{
		return;
	}

	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
	GreatWorkSlotType eMusicSlot = CvTypes::getGREAT_WORK_SLOT_MUSIC();
	GreatWorkSlotType eWritingSlot = CvTypes::getGREAT_WORK_SLOT_LITERATURE();

	int iMaxCultureCost = 0;
	int iOpenArtSlots = 0;
	int iOpenWritingSlots = 0;
	int iOpenMusicSlots = 0;
	CvWeightedVector<int, SAFE_ESTIMATE_NUM_BUILDINGS, true> artChoices;
	CvCity* pLoopCity;
	int iLoop;
	for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		iOpenArtSlots += pLoopCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);
		iOpenMusicSlots += pLoopCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eMusicSlot);
		iOpenWritingSlots += pLoopCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eWritingSlot);
	}
	if (iOpenArtSlots <= 0 && iOpenWritingSlots <= 0 && iOpenMusicSlots <= 0)
		return;

	for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		int iSpy = m_pPlayer->GetEspionage()->GetSpyIndexInCity(pLoopCity);
		if (iSpy != -1)
		{
			CvEspionageSpy* pSpy = &(m_aSpyList[iSpy]);
			if (!pSpy->m_bIsThief)
				continue;

			int iNumGreatWorks = pLoopCity->GetCityBuildings()->GetNumGreatWorks();

			if (iNumGreatWorks <= 0)
				continue;

			for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
			{
				const CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(ePlayer).getCivilizationInfo();
				BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
				if (eBuilding != NO_BUILDING)
				{
					CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
					if (pkBuilding)
					{
						if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
						{
							if (pkBuilding->GetGreatWorkSlotType() == eArtArtifactSlot && iOpenArtSlots > 0)
							{
								int iNumSlots = pkBuilding->GetGreatWorkCount();
								if (iNumSlots > 0)
								{
									for (int iI = 0; iI < iNumSlots; iI++)
									{
										int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
										if (iGreatWorkIndex != -1 && !m_pPlayer->GetCulture()->ControlsGreatWork(iGreatWorkIndex))
										{
											// add to list!
											GWList kGWList;
											kGWList.m_iGreatWorkIndex = iGreatWorkIndex;
											m_aPlayerStealableGWList[ePlayer].push_back(kGWList);

											
											CvGreatWork work = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWorkIndex];

											int CultureCost = GET_PLAYER(ePlayer).getNextPolicyCost() * 15;
											if (CultureCost > iMaxCultureCost)
											{
												iMaxCultureCost = CultureCost;
											}
										}
									}
								}
							}
							else if (pkBuilding->GetGreatWorkSlotType() == eMusicSlot && iOpenMusicSlots > 0)
							{
								int iNumSlots = pkBuilding->GetGreatWorkCount();
								if (iNumSlots > 0)
								{
									for (int iI = 0; iI < iNumSlots; iI++)
									{
										int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
										if (iGreatWorkIndex != -1 && !m_pPlayer->GetCulture()->ControlsGreatWork(iGreatWorkIndex))
										{
											// add to list!
											GWList kGWList;
											kGWList.m_iGreatWorkIndex = iGreatWorkIndex;
											m_aPlayerStealableGWList[ePlayer].push_back(kGWList);

											CvGreatWork work = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWorkIndex];

											int CultureCost = GET_PLAYER(ePlayer).getNextPolicyCost() * 15;
											if (CultureCost > iMaxCultureCost)
											{
												iMaxCultureCost = CultureCost;
											}
										}
									}
								}
							}
							else if (pkBuilding->GetGreatWorkSlotType() == eWritingSlot && iOpenWritingSlots > 0)
							{
								int iNumSlots = pkBuilding->GetGreatWorkCount();
								if (iNumSlots > 0)
								{
									for (int iI = 0; iI < iNumSlots; iI++)
									{
										int iGreatWorkIndex = pLoopCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
										if (iGreatWorkIndex != -1 && !m_pPlayer->GetCulture()->ControlsGreatWork(iGreatWorkIndex))
										{
											// add to list!
											GWList kGWList;
											kGWList.m_iGreatWorkIndex = iGreatWorkIndex;
											m_aPlayerStealableGWList[ePlayer].push_back(kGWList);

											CvGreatWork work = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGreatWorkIndex];

											int CultureCost = GET_PLAYER(ePlayer).getNextPolicyCost() * 15;
											if (CultureCost > iMaxCultureCost)
											{
												iMaxCultureCost = CultureCost;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	m_aiMaxGWCost[ePlayer] = iMaxCultureCost;
}

/// IsTechStealable - Check to see if you can steal this tech from an opponent
bool CvPlayerEspionage::IsGWStealable(PlayerTypes ePlayer, int iGreatWorkIndex)
{
	CvAssertMsg((uint)ePlayer < m_aPlayerStealableGWList.size(), "ePlayer out of bounds");
	if ((uint)ePlayer >= m_aPlayerStealableGWList.size())
	{
		return false;
	}

	for (uint ui = 0; ui < m_aPlayerStealableGWList[ePlayer].size(); ui++)
	{
		if (m_aPlayerStealableGWList[ePlayer][ui].m_iGreatWorkIndex == iGreatWorkIndex)
		{
			return true;
		}
	}

	return false;
}
/// GetNumTechsToSteal - How many techs you can steal from a given player
int CvPlayerEspionage::GetNumGWToSteal(PlayerTypes ePlayer)
{
	CvAssertMsg((uint)ePlayer < m_aiNumGWToStealList.size(), "ePlayer out of bounds");
	if ((uint)ePlayer >= m_aiNumGWToStealList.size())
	{
		return -1;
	}

	return m_aiNumGWToStealList[ePlayer];
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
#endif

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
#if defined(MOD_BALANCE_CORE_SPIES)
void CvPlayerEspionage::AddSpyMessage(int iCityX, int iCityY, PlayerTypes eAttackingPlayer, int iSpyResult, TechTypes eStolenTech, BuildingTypes eBuilding, UnitTypes eUnit, bool bUnrest, int iValue, int iScienceValue, bool bRebel, int iGreatWorkIndex)
#else
void CvPlayerEspionage::AddSpyMessage(int iCityX, int iCityY, PlayerTypes eAttackingPlayer, int iSpyResult, TechTypes eStolenTech)
#endif
{
	// go through all messages to see if this is an update
	for(uint ui = 0; ui < m_aSpyNotificationMessages.size(); ui++)
	{
		if(m_aSpyNotificationMessages[ui].m_iCityX == iCityX && m_aSpyNotificationMessages[ui].m_iCityY == iCityY && m_aSpyNotificationMessages[ui].m_eAttackingPlayer == eAttackingPlayer)
		{
			m_aSpyNotificationMessages[ui].m_iSpyResult = iSpyResult;
			m_aSpyNotificationMessages[ui].m_eStolenTech = eStolenTech;
#if defined(MOD_BALANCE_CORE_SPIES)
			if(MOD_BALANCE_CORE_SPIES)
			{
				m_aSpyNotificationMessages[ui].m_eDamagedBuilding = eBuilding;
				m_aSpyNotificationMessages[ui].m_eDamagedUnit = eUnit;
				m_aSpyNotificationMessages[ui].m_bDamagedCity = bUnrest;
				m_aSpyNotificationMessages[ui].m_iGold = iValue;
				m_aSpyNotificationMessages[ui].m_iScience = iScienceValue;
				m_aSpyNotificationMessages[ui].m_bRebellion = bRebel;
				m_aSpyNotificationMessages[ui].m_iGWIndex = iGreatWorkIndex;
			}
#endif
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
#if defined(MOD_BALANCE_CORE_SPIES)
	if(MOD_BALANCE_CORE_SPIES)
	{
		kMessage.m_eDamagedBuilding = eBuilding;
		kMessage.m_eDamagedUnit = eUnit;
		kMessage.m_bDamagedCity = bUnrest;
		kMessage.m_iGold = iValue;
		kMessage.m_iScience = iScienceValue;
		kMessage.m_bRebellion = bRebel;
		kMessage.m_iGWIndex = iGreatWorkIndex;
	}
#endif

#if defined(MOD_EVENTS_ESPIONAGE)
	if (MOD_EVENTS_ESPIONAGE) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_EspionageNotificationData, iCityX, iCityY, eAttackingPlayer, m_pPlayer->GetID(), iSpyResult, eStolenTech, eBuilding, eUnit, bUnrest, iValue, iScienceValue, bRebel, iGreatWorkIndex);
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
#if defined(MOD_BALANCE_CORE_SPIES)
		int iGWIndex = -1;
		if (m_aSpyNotificationMessages[ui].m_iGWIndex != -1)
		{
			iGWIndex = m_aSpyNotificationMessages[ui].m_iGWIndex;
		}

		CvBuildingEntry* pBuildingInfo = NULL;
		CvUnitEntry* pUnitInfo = NULL;
		bool bUnrest = false;
		bool bRebellion = false;
		int iGold = 0;
		int iScience = 0;
		int iProduction = 0;
		int iDamage = 0;
		bool bGreatPerson = false;
		bool bAdvanced = false;
		if (!pTechEntry && iGWIndex == -1 && MOD_BALANCE_CORE_SPIES && GC.getBALANCE_SPY_SABOTAGE_RATE() > 0)
		{
			if(m_aSpyNotificationMessages[ui].m_eDamagedBuilding != NO_BUILDING)
			{
				pBuildingInfo = GC.getBuildingInfo(m_aSpyNotificationMessages[ui].m_eDamagedBuilding);
				iProduction = m_aSpyNotificationMessages[ui].m_iGold;
			}
			else if(m_aSpyNotificationMessages[ui].m_eDamagedUnit != NO_UNIT)
			{
				pUnitInfo = GC.getUnitInfo(m_aSpyNotificationMessages[ui].m_eDamagedUnit);
				iProduction = m_aSpyNotificationMessages[ui].m_iGold;
				// Get the right unit of this class for this civ
				if(pUnitInfo)
				{
					SpecialUnitTypes eSpecialUnitGreatPerson = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_PEOPLE");
					if(pUnitInfo->GetSpecialUnitType() == eSpecialUnitGreatPerson)
					{
						bGreatPerson = true;
						bAdvanced = true;
					}
				}
			}
			else if(m_aSpyNotificationMessages[ui].m_bDamagedCity != false)
			{
				bUnrest = true;
				bAdvanced = true;
				iDamage = m_aSpyNotificationMessages[ui].m_iGold;
			}
			else if(m_aSpyNotificationMessages[ui].m_bRebellion != false)
			{
				bAdvanced = true;
				bRebellion = true;
			}
			else if(m_aSpyNotificationMessages[ui].m_iGold > 0)
			{
				bAdvanced = true;
				iGold = m_aSpyNotificationMessages[ui].m_iGold;
			}
			else if(m_aSpyNotificationMessages[ui].m_iScience > 0)
			{
				bAdvanced = true;
				iScience = m_aSpyNotificationMessages[ui].m_iScience;
			}
			else
			{
				bAdvanced = true;
			}
			if(pBuildingInfo != NULL)
			{
				switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
				{
				case SPY_RESULT_UNDETECTED:
					// nothing happens, nobody is notified
					break;
				case SPY_RESULT_DETECTED:
				{
					// notify defending player that a spy of unknown origin damaged a building
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_BUILDING_DAMAGED_SPY_DETECTED_S");
					strSummary << pBuildingInfo->GetDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if (pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_BUILDING_DAMAGED_SPY_DETECTED_W_TIP");
					}
					else // defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_BUILDING_DAMAGED_SPY_DETECTED");
					}
					strNotification << pBuildingInfo->GetDescriptionKey();
					strNotification << pCity->getNameKey();
					strNotification << iProduction;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
					bAdvanced = true;
				}
				break;
				case SPY_RESULT_IDENTIFIED:
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_BUILDING_DAMAGED_SPY_IDENTIFIED_S");
					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
					strSummary << pBuildingInfo->GetDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_BUILDING_DAMAGED_SPY_IDENTIFIED_W_TIP");
					}
					else
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_BUILDING_DAMAGED_SPY_IDENTIFIED");
					}
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
					strNotification << pBuildingInfo->GetDescriptionKey();
					strNotification << pCity->getNameKey();
					strNotification << iProduction;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

					pEspionageAI->m_aiNumSpiesCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer]++;
					pEspionageAI->m_aiTurnLastSpyCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer] = GC.getGame().getGameTurn();
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
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
						strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
	#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
	#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
	#endif
						strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();

						pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
					
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
			else if((pUnitInfo != NULL) && !bGreatPerson)
			{
				switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
				{
				case SPY_RESULT_UNDETECTED:
					// nothing happens, nobody is notified
					break;
				case SPY_RESULT_DETECTED:
				{
					// notify defending player that a spy of unknown damaged a unit
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_UNIT_DAMAGED_SPY_DETECTED_S");
					strSummary << pUnitInfo->GetDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_UNIT_DAMAGED_SPY_DETECTED_W_TIP");
					}
					else // defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_UNIT_DAMAGED_SPY_DETECTED");
					}
					strNotification << pUnitInfo->GetDescriptionKey();
					strNotification << pCity->getNameKey();
					strNotification << iProduction;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
				break;
				case SPY_RESULT_IDENTIFIED:
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_UNIT_DAMAGED_SPY_IDENTIFIED_S");
					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
					strSummary << pUnitInfo->GetDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_UNIT_DAMAGED_SPY_IDENTIFIED_W_TIP");
					}
					else
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_UNIT_DAMAGED_SPY_IDENTIFIED");
					}
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
					strNotification << pUnitInfo->GetDescriptionKey();
					strNotification << pCity->getNameKey();
					strNotification << iProduction;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

					pEspionageAI->m_aiNumSpiesCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer]++;
					pEspionageAI->m_aiTurnLastSpyCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer] = GC.getGame().getGameTurn();
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
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
						strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
	#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
	#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
	#endif
						strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();

						pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
					
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
			else if((pUnitInfo != NULL) && bGreatPerson)
			{
				switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
				{
				case SPY_RESULT_UNDETECTED:
					// nothing happens, nobody is notified
					break;
				case SPY_RESULT_DETECTED:
				{
					// notify defending player that a spy of unknown damaged a unit
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GP_DAMAGED_SPY_DETECTED_S");
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GP_DAMAGED_SPY_DETECTED_W_TIP");
					}
					else // defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GP_DAMAGED_SPY_DETECTED");
					}
					strNotification << pUnitInfo->GetDescriptionKey();
					strNotification << pCity->getNameKey();
					strNotification << iProduction;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
				break;
				case SPY_RESULT_IDENTIFIED:
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GP_DAMAGED_SPY_IDENTIFIED_S");
					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GP_DAMAGED_SPY_IDENTIFIED_W_TIP");
					}
					else
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GP_DAMAGED_SPY_IDENTIFIED");
					}
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
					strNotification << pUnitInfo->GetDescriptionKey();
					strNotification << pCity->getNameKey();
					strNotification << iProduction;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

					pEspionageAI->m_aiNumSpiesCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer]++;
					pEspionageAI->m_aiTurnLastSpyCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer] = GC.getGame().getGameTurn();
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
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
						strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
	#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
	#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
	#endif
						strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();

						pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
					
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
			else if(bRebellion)
			{
				switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
				{
				case SPY_RESULT_UNDETECTED:
					// nothing happens, nobody is notified
					break;
				case SPY_RESULT_DETECTED:
				{
					// notify defending player that a spy of unknown damaged a city
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_DAMAGED_SPY_DETECTED_S");
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_DAMAGED_SPY_DETECTED_W_TIP");
					}
					else // defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_DAMAGED_SPY_DETECTED");
					}
					strNotification << pCity->getNameKey();
					strNotification << iDamage;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
				break;
				case SPY_RESULT_IDENTIFIED:
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_DAMAGED_SPY_IDENTIFIED_S");
					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_DAMAGED_SPY_IDENTIFIED_W_TIP");
					}
					else
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_DAMAGED_SPY_IDENTIFIED");
					}
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
					strNotification << pCity->getNameKey();
					strNotification << iDamage;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

					pEspionageAI->m_aiNumSpiesCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer]++;
					pEspionageAI->m_aiTurnLastSpyCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer] = GC.getGame().getGameTurn();
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
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
						strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
	#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
	#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
	#endif
						strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();

						pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
					
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
			else if(bUnrest)
			{
				switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
				{
				case SPY_RESULT_UNDETECTED:
					// nothing happens, nobody is notified
					break;
				case SPY_RESULT_DETECTED:
				{
					// notify defending player that a spy of unknown damaged a city
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_REBEL_ARMED_SPY_DETECTED_S");
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_REBEL_ARMED_SPY_DETECTED_W_TIP");
					}
					else // defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_REBEL_ARMED_SPY_DETECTED");
					}
					strNotification << pCity->getNameKey();
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
				break;
				case SPY_RESULT_IDENTIFIED:
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_REBEL_ARMED_SPY_IDENTIFIED_S");
					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_REBEL_ARMED_SPY_IDENTIFIED_W_TIP");
					}
					else
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_REBEL_ARMED_SPY_IDENTIFIED");
					}
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
					strNotification << pCity->getNameKey();
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

					pEspionageAI->m_aiNumSpiesCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer]++;
					pEspionageAI->m_aiTurnLastSpyCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer] = GC.getGame().getGameTurn();
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
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
						strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
	#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
	#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
	#endif
						strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();

						pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
					
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
			else if(iGold > 0)
			{
				switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
				{
				case SPY_RESULT_UNDETECTED:
					// nothing happens, nobody is notified
					break;
				case SPY_RESULT_DETECTED:
				{
					// notify defending player that a spy of unknown damaged a city
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLD_STOLEN_SPY_DETECTED_S");
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLD_STOLEN_SPY_DETECTED_W_TIP");
					}
					else // defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLD_STOLEN_SPY_DETECTED");
					}
					strNotification << pCity->getNameKey();
					strNotification << iGold;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
				break;
				case SPY_RESULT_IDENTIFIED:
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLD_STOLEN_SPY_IDENTIFIED_S");
					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLD_STOLEN_SPY_IDENTIFIED_W_TIP");
					}
					else
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GOLD_STOLEN_SPY_IDENTIFIED");
					}
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
					strNotification << pCity->getNameKey();
					strNotification << iGold;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

					pEspionageAI->m_aiNumSpiesCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer]++;
					pEspionageAI->m_aiTurnLastSpyCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer] = GC.getGame().getGameTurn();
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
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
						strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
	#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
	#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
	#endif
						strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();

						pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
					
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
			else if(iScience > 0)
			{
				switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
				{
				case SPY_RESULT_UNDETECTED:
					// nothing happens, nobody is notified
					break;
				case SPY_RESULT_DETECTED:
				{
					// notify defending player that a spy of unknown damaged a city
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SCIENCE_STOLEN_SPY_DETECTED_S");
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SCIENCE_STOLEN_SPY_DETECTED_W_TIP");
					}
					else // defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SCIENCE_STOLEN_SPY_DETECTED");
					}
					strNotification << pCity->getNameKey();
					strNotification << iScience;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
				break;
				case SPY_RESULT_IDENTIFIED:
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SCIENCE_STOLEN_SPY_IDENTIFIED_S");
					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SCIENCE_STOLEN_SPY_IDENTIFIED_W_TIP");
					}
					else
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SCIENCE_STOLEN_SPY_IDENTIFIED");
					}
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
					strNotification << pCity->getNameKey();
					strNotification << iScience;
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

					pEspionageAI->m_aiNumSpiesCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer]++;
					pEspionageAI->m_aiTurnLastSpyCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer] = GC.getGame().getGameTurn();
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
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
						strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
	#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
	#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
	#endif
						strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();

						pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
					
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
			else
			{
				switch(m_aSpyNotificationMessages[ui].m_iSpyResult)
				{
				case SPY_RESULT_UNDETECTED:
					// nothing happens, nobody is notified
					break;
				case SPY_RESULT_DETECTED:
				{
					// notify defending player that a spy of unknown damaged a city
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_DETECTED_S");
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_DETECTED_W_TIP");
					}
					else // defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_DETECTED");
					}
					strNotification << pCity->getNameKey();
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
				}
				break;
				case SPY_RESULT_IDENTIFIED:
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_IDENTIFIED_S");
					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
					strSummary << pCity->getNameKey();

					Localization::String strNotification;
					if(pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_IDENTIFIED_W_TIP");
					}
					else
					{
						strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_IDENTIFIED");
					}
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
					strNotification << pCity->getNameKey();
					pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

					pEspionageAI->m_aiNumSpiesCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer]++;
					pEspionageAI->m_aiTurnLastSpyCaught[m_aSpyNotificationMessages[ui].m_eAttackingPlayer] = GC.getGame().getGameTurn();
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
						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
						strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
	#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
	#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
	#endif
						strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();

						pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
					
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
		}
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
		if (bMultiplayer || (!bAdvanced && !pTechEntry && iGWIndex == -1))
#else
		if(bMultiplayer || !pTechEntry)
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
#else
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
#endif
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
#if defined(MOD_BALANCE_CORE)
		else if (iGWIndex != -1)
		{
			switch (m_aSpyNotificationMessages[ui].m_iSpyResult)
			{
			case SPY_RESULT_UNDETECTED:
				// nothing happens, nobody is notified
				break;
			case SPY_RESULT_DETECTED:
			{
				// notify defending player that a spy of unknown origin stole a tech
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GW_STOLEN_SPY_DETECTED_S");
				strSummary << GC.getGame().GetGameCulture()->GetGreatWorkName(iGWIndex);

				Localization::String strNotification;
				if (pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GW_STOLEN_SPY_DETECTED_W_TIP");
				}
				else // defensive spy
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GW_STOLEN_SPY_DETECTED");
				}
				strNotification << GC.getGame().GetGameCulture()->GetGreatWorkName(iGWIndex);
				strNotification << pCity->getNameKey();
				pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
			break;
			case SPY_RESULT_IDENTIFIED:
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_GW_STOLEN_SPY_IDENTIFIED_S");
				strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getShortDescriptionKey();
				strSummary << GC.getGame().GetGameCulture()->GetGreatWorkName(iGWIndex);

				Localization::String strNotification;
				if (pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GW_STOLEN_SPY_IDENTIFIED_W_TIP");
				}
				else
				{
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_GW_STOLEN_SPY_IDENTIFIED");
				}
				strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
				strNotification << GC.getGame().GetGameCulture()->GetGreatWorkName(iGWIndex);
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
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
		str << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
		str << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
				strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
				strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
						strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
					strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
				strNotification << m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer);
#else
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
#endif
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
#if defined(MOD_BALANCE_CORE)
			m_pPlayer->ChangeNumCivsConstructingWonder(m_aIntrigueNotificationMessages[uiIndex].m_eBuilding, 1);
#endif
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
	for (uint uiCiv = 0; uiCiv < uiNumCivs; uiCiv++)
	{
		int iNumGWToSteal;
		loadFrom >> iNumGWToSteal;
		writeTo.m_aiNumGWToStealList.push_back(iNumGWToSteal);
	}
	loadFrom >> uiNumCivs;
	for(uint uiCiv = 0; uiCiv < uiNumCivs; uiCiv++)
	{
		int iNumSpyActionsDone;
		loadFrom >> iNumSpyActionsDone;
		writeTo.m_aiNumSpyActionsDone.push_back(iNumSpyActionsDone);
	}
	int iMaxGWCostEntries;
	loadFrom >> iMaxGWCostEntries;
	for (int i = 0; i < iMaxGWCostEntries; i++)
	{
		loadFrom >> writeTo.m_aiMaxGWCost[i];
	}
#endif

	int iMaxTechCostEntries;
	loadFrom >> iMaxTechCostEntries;
	for(int i = 0; i < iMaxTechCostEntries; i++)
	{
		loadFrom >> writeTo.m_aiMaxTechCost[i];
	}


	int iNumCivs;
	loadFrom >> iNumCivs;
	for(int iCiv = 0; iCiv < iNumCivs; iCiv++)
	{
		int iNumEntries;
		loadFrom >> iNumEntries;
		for(int i = 0; i < iNumEntries; i++)
		{
			HeistLocation kHeistLocation;
			loadFrom >> kHeistLocation.m_iX;
			loadFrom >> kHeistLocation.m_iY;
			writeTo.m_aHeistLocations[iCiv].push_back(kHeistLocation);
		}
	}
#if defined(MOD_BALANCE_CORE)
	loadFrom >> iNumCivs;
	for (int iCiv = 0; iCiv < iNumCivs; iCiv++)
	{
		int iNumEntries;
		loadFrom >> iNumEntries;
		for (int i = 0; i < iNumEntries; i++)
		{
			GWList kGWList;
			loadFrom >> kGWList.m_iGreatWorkIndex;
			writeTo.m_aPlayerStealableGWList[iCiv].push_back(kGWList);
		}
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
#if defined(MOD_BALANCE_CORE_SPIES)
		if(MOD_BALANCE_CORE_SPIES)
		{
			loadFrom >> kMessage.m_eDamagedBuilding;
			loadFrom >> kMessage.m_eDamagedUnit;
			loadFrom >> kMessage.m_bDamagedCity;
			loadFrom >> kMessage.m_iGold;
			loadFrom >> kMessage.m_iScience;
			loadFrom >> kMessage.m_bRebellion;
			loadFrom >> kMessage.m_iGWIndex;
		}
#endif
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
	saveTo << readFrom.m_aiNumGWToStealList.size();
	for (uint uiCiv = 0; uiCiv < readFrom.m_aiNumGWToStealList.size(); uiCiv++)
	{
		saveTo << readFrom.m_aiNumGWToStealList[uiCiv];
	}
	saveTo << readFrom.m_aiNumSpyActionsDone.size();
	for(uint uiCiv = 0; uiCiv < readFrom.m_aiNumSpyActionsDone.size(); uiCiv++)
	{
		saveTo << readFrom.m_aiNumSpyActionsDone[uiCiv];
	}

	saveTo << MAX_MAJOR_CIVS;
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiMaxGWCost[ui];
	}
#endif

	saveTo << MAX_MAJOR_CIVS;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiMaxTechCost[ui];
	}

	saveTo << MAX_MAJOR_CIVS;
	for(uint uiCiv = 0; uiCiv < MAX_MAJOR_CIVS; uiCiv++)
	{
		saveTo << readFrom.m_aHeistLocations[uiCiv].size();
		for(uint ui = 0; ui < readFrom.m_aHeistLocations[uiCiv].size(); ui++)
		{
			saveTo << readFrom.m_aHeistLocations[uiCiv][ui].m_iX;
			saveTo << readFrom.m_aHeistLocations[uiCiv][ui].m_iY;
		}
	}
#if defined(MOD_BALANCE_CORE)
	saveTo << MAX_MAJOR_CIVS;
	for (uint uiCiv = 0; uiCiv < MAX_MAJOR_CIVS; uiCiv++)
	{
		saveTo << readFrom.m_aPlayerStealableGWList[uiCiv].size();
		for (uint ui = 0; ui < readFrom.m_aPlayerStealableGWList[uiCiv].size(); ui++)
		{
			saveTo << readFrom.m_aPlayerStealableGWList[uiCiv][ui].m_iGreatWorkIndex;
		}
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
#if defined(MOD_BALANCE_CORE_SPIES)
		if(MOD_BALANCE_CORE_SPIES)
		{
			saveTo << readFrom.m_aSpyNotificationMessages[ui].m_eDamagedBuilding;
			saveTo << readFrom.m_aSpyNotificationMessages[ui].m_eDamagedUnit;
			saveTo << readFrom.m_aSpyNotificationMessages[ui].m_bDamagedCity;
			saveTo << readFrom.m_aSpyNotificationMessages[ui].m_iGold;
			saveTo << readFrom.m_aSpyNotificationMessages[ui].m_iScience;
			saveTo << readFrom.m_aSpyNotificationMessages[ui].m_bRebellion;
			saveTo << readFrom.m_aSpyNotificationMessages[ui].m_iGWIndex;
		}
#endif
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
#if defined(MOD_BALANCE_CORE)
	m_aiCivOutOfGWTurn.clear();
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiCivOutOfGWTurn.push_back(-1);
	}
#endif

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

	StealTechnology();
	UpdateCivOutOfTechTurn();
#if defined(MOD_BALANCE_CORE)
	UpdateCivOutOfGWTurn();
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
				pEspionage->LogEspionageMsg(strMsg);
			}
		}
	}

	int iTargetOffensiveSpies = 0;
	int iTargetDefensiveSpies = 0;
	int iTargetCityStateSpies = 0;
	int iTargetDiplomatSpies  = 0;
	FindTargetSpyNumbers(&iTargetOffensiveSpies, &iTargetDefensiveSpies, &iTargetCityStateSpies, &iTargetDiplomatSpies);

	std::vector<CvCity*> apCityDiplomat;
	std::vector<CvCity*> apCityOffense;
	std::vector<CvCity*> apCityDefense;
	std::vector<CvCity*> apCityMinor;
	
	if (iTargetDiplomatSpies > 0)
	{
		BuildDiplomatCityList(apCityDiplomat);
	}

	if (iTargetOffensiveSpies > 0)
	{
		BuildOffenseCityList(apCityOffense);
	}

	if (iTargetDefensiveSpies > 0)
	{
		BuildDefenseCityList(apCityDefense);
	}

	if (iTargetCityStateSpies > 0)
	{
		BuildMinorCityList(apCityMinor);
	}

	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();

	// loop through spies an extract all that aren't in a correct location
	int iCorrectlyAssignedDiplomatSpies = 0;
	int iCorrectlyAssignedDefenseSpies = 0;
	int iCorrectlyAssignedOffenseSpies = 0;
	int iCorrectlyAssignedCityStateSpies = 0;

	for(uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
	{
		// dead spies are not processed
#if defined(MOD_API_ESPIONAGE)
		if(pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD || pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_TERMINATED)
#else
		if(pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD)
#endif
		{
			continue;
		}

		CvCity* pSpyCity = pEspionage->GetCityWithSpy(uiSpy);

		// spies that are not assigned don't need to be processed
		if(!pSpyCity)
		{
			continue;
		}

		bool bAlreadyAssigned = false;

		// diplomat cities
		if (!bAlreadyAssigned)
		{
			// see if the spy is in any of the diplomat cities
			for(int iDiplomatIndex = 0; iDiplomatIndex < iTargetDiplomatSpies && iDiplomatIndex < (int)apCityDiplomat.size(); iDiplomatIndex++)
			{
				if(pSpyCity == apCityDiplomat[iDiplomatIndex] && pEspionage->m_aSpyList[uiSpy].m_bIsDiplomat)
				{
					iCorrectlyAssignedDiplomatSpies++;
					bAlreadyAssigned = true;
					break;
				}
			}
		}

		// defensive cities
		if(!bAlreadyAssigned)
		{
			// see if the spy is in any of the defensive cities
			for(int iDefensiveIndex = 0; iDefensiveIndex < iTargetDefensiveSpies && iDefensiveIndex < (int)apCityDefense.size(); iDefensiveIndex++)
			{
				if(pSpyCity == apCityDefense[iDefensiveIndex])
				{
					iCorrectlyAssignedDefenseSpies++;
					bAlreadyAssigned = true;
					break;
				}
			}
		}

		// offensive cities
		if(!bAlreadyAssigned)
		{
			// see if the spy is in any of the offensive cities
			for(int iOffensiveIndex = 0; iOffensiveIndex < iTargetOffensiveSpies && iOffensiveIndex < (int)apCityOffense.size(); iOffensiveIndex++)
			{
				if(pSpyCity == apCityOffense[iOffensiveIndex] && !pEspionage->m_aSpyList[uiSpy].m_bIsDiplomat)
				{
					iCorrectlyAssignedOffenseSpies++;
					bAlreadyAssigned = true;
					break;
				}
			}
		}

		// city-states
		if (!bAlreadyAssigned)
		{
			// see if the spy is in any of the city-states
			for (int iCityStateIndex = 0; iCityStateIndex < iTargetCityStateSpies && iCityStateIndex < (int)apCityMinor.size(); iCityStateIndex++)
			{
				if (pSpyCity == apCityMinor[iCityStateIndex])
				{
					iCorrectlyAssignedCityStateSpies++;
					bAlreadyAssigned = true;
					break;
				}
			}
		}

		// if the spy is flagged to be reassigned
		if (pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment)
		{
			// if the spy is already assigned to a correct city, mark them as assigned
			if (bAlreadyAssigned)
			{
				pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment = false;		
			}
			else
			{
				// move spy out of target city
#if defined(MOD_BALANCE_CORE)
				pEspionage->MoveSpyTo(NULL, uiSpy, false, false);
#else
				pEspionage->MoveSpyTo(NULL, uiSpy, false);
#endif
			}
		}
	}

	// go through spy list again and put spies in locations where they are needed
	for(uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
	{
		// dead spies are not processed
#if defined(MOD_API_ESPIONAGE)
		if(pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD || pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_TERMINATED)
#else
		if(pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD)
#endif
		{
			continue;
		}

		// if this spy is not to be re-evaluated
		if (!pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment)
		{
			continue;
		}

		// we're assigning the spy now, so reset their evaluate statement
		pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment = false;

		CvCity* pSpyCity = pEspionage->GetCityWithSpy(uiSpy);

		// spies that are already are in a city are assumed to be in the right place
		if(pSpyCity)
		{
			continue;
		}

		// if we have remaining defensive spies to assign
		if(iTargetDiplomatSpies > iCorrectlyAssignedDiplomatSpies)
		{
			// walk through cities and an open spot for the spy
			for(int iCity = 0; iCity < iTargetDiplomatSpies && iCity < (int)apCityDiplomat.size(); iCity++)
			{
				CvCity* pCity = apCityDiplomat[iCity];

				// if a spy is already in this city, skip it
				if(pEspionage->GetSpyIndexInCity(pCity) != -1)
				{
					continue;
				}
#if defined(MOD_BALANCE_CORE)
				pEspionage->MoveSpyTo(pCity, uiSpy, true, false);
#else
				pEspionage->MoveSpyTo(pCity, uiSpy, true);
#endif
				iCorrectlyAssignedDefenseSpies++;
				break;
			}
		}
		// if we have remaining defensive spies to assign
		else if(iTargetDefensiveSpies > iCorrectlyAssignedDefenseSpies)
		{
			// walk through cities and an open spot for the spy
			for(int iCity = 0; iCity < iTargetDefensiveSpies && iCity < (int)apCityDefense.size(); iCity++)
			{
				CvCity* pCity = apCityDefense[iCity];

				// if a spy is already in this city, skip it
				if(pEspionage->GetSpyIndexInCity(pCity) != -1)
				{
					continue;
				}

				pEspionage->MoveSpyTo(pCity, uiSpy, false);
				iCorrectlyAssignedDefenseSpies++;
				break;
			}
		}
		// if we have remaining offensive spies to assign
		else if (iTargetOffensiveSpies > iCorrectlyAssignedOffenseSpies)
		{
			for (int iCity = 0; iCity < iTargetOffensiveSpies && iCity < (int)apCityOffense.size(); iCity++)
			{
				CvCity* pCity = apCityOffense[iCity];

				// if a spy is already in this city, skip it
				if (pEspionage->GetSpyIndexInCity(pCity) != -1)
				{
					continue;
				}
#if defined(MOD_BALANCE_CORE)
				bool bWantThief = false;
				if (pCity->GetCityBuildings()->GetNumGreatWorks() > 0 && !pEspionage->IsAnyPreparingHeist(pCity) && pEspionage->CanMoveSpyTo(pCity, uiSpy, false, true))
				{
					if (m_pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory() || m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory())
					{
						bWantThief = true;
					}
					else if (GET_PLAYER(pCity->getOwner()).GetDiplomacyAI()->IsCloseToCultureVictory())
					{
						bWantThief = true;
					}
					else if (GET_PLAYER(pCity->getOwner()).GetDiplomacyAI()->IsGoingForCultureVictory())
					{
						bWantThief = true;
					}
				}
				
				pEspionage->MoveSpyTo(pCity, uiSpy, false, bWantThief);
#else
				pEspionage->MoveSpyTo(pCity, uiSpy, false);
#endif
				iCorrectlyAssignedOffenseSpies++;
				break;
			}
		}
		// if we have remaining city-state spies to assign
		else if (iTargetCityStateSpies > iCorrectlyAssignedCityStateSpies)
		{
			for (int iCity = 0; iCity < iTargetCityStateSpies && iCity < (int)apCityMinor.size(); iCity++)
			{
				CvCity* pCity = apCityMinor[iCity];

				// if a spy is already in this city, skip it
				if (pEspionage->GetSpyIndexInCity(pCity) != -1)
				{
					continue;
				}

				pEspionage->MoveSpyTo(pCity, uiSpy, false);
				iCorrectlyAssignedCityStateSpies++;
				break;
			}
		}
	}
	// go through spy list again and put spies in locations where they are needed
	for (uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
	{
		// dead spies are not processed
#if defined(MOD_API_ESPIONAGE)
		if (pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD || pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_TERMINATED)
#else
		if (pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD)
#endif
		{
			continue;
		}
		//Unassigned spies remaining? Defend!
		if (pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_UNASSIGNED)
		{
			CvCity* pPlayerCity;
			int iCityLoop;
			for (pPlayerCity = m_pPlayer->firstCity(&iCityLoop); pPlayerCity != NULL; pPlayerCity = m_pPlayer->nextCity(&iCityLoop))
			{
				if (pPlayerCity == NULL)
					continue;

				// if a spy is already in this city, skip it
				if (pEspionage->GetSpyIndexInCity(pPlayerCity) != -1)
				{
					continue;
				}

				pEspionage->MoveSpyTo(pPlayerCity, uiSpy, false);
				iCorrectlyAssignedDefenseSpies++;
				break;
			}
		}
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
		int iHeistLocationCounter = 0;
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

			// send out notifications to the parties that were stolen from
			CvPlot* pPlot = GC.getMap().plot(pEspionage->m_aHeistLocations[uiDefendingPlayer][iHeistLocationCounter].m_iX, pEspionage->m_aHeistLocations[uiDefendingPlayer][iHeistLocationCounter].m_iY);
			CvAssertMsg(pPlot, "Could not find plot location for HeistLocation");
			if(pPlot)
			{
				CvCity* pCity = pPlot->getPlotCity();
				CvAssertMsg(pCity, "Plot does not contain a city");
				if(pCity)
				{
					CvAssertMsg(pCity->getOwner() == uiDefendingPlayer, "City does not belong to player. This is very strange");
					CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
					CvAssertMsg(pCityEspionage, "City did not have CityEspionage data");
					if(pCityEspionage)
					{
						CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(eDefendingPlayer).GetEspionage();
						if(pDefendingPlayerEspionage)
						{						
#if defined(MOD_BALANCE_CORE_SPIES)
							pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[m_pPlayer->GetID()], eStolenTech, NO_BUILDING, NO_UNIT, false, 0, 0, false);
#else
							pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[m_pPlayer->GetID()], eStolenTech);
#endif
						}
					}
				}
			}
			iHeistLocationCounter++;

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
#if defined(MOD_BALANCE_CORE)
/// Checks to see if there are any GWs to steal
/// If so, steals them!
void CvEspionageAI::StealGreatWork()
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();


	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
	GreatWorkSlotType eMusicSlot = CvTypes::getGREAT_WORK_SLOT_MUSIC();
	GreatWorkSlotType eWritingSlot = CvTypes::getGREAT_WORK_SLOT_LITERATURE();

	int iStuffStolen = 0;

	// try to steal technologies
	for (uint uiDefendingPlayer = 0; uiDefendingPlayer < MAX_MAJOR_CIVS; uiDefendingPlayer++)
	{
		PlayerTypes eDefendingPlayer = (PlayerTypes)uiDefendingPlayer;
		int iHeistLocationCounter = 0;
		while (pEspionage->m_aiNumGWToStealList[uiDefendingPlayer] > 0)
		{
			// steal a tech
			CvAssertMsg(pEspionage->m_aPlayerStealableGWList[uiDefendingPlayer].size() > 0, "pEspionage->m_aPlayerStealableGWList[uiPlayer] list is empty. Not good");
			
			int iGrab = GC.getGame().getJonRandNum(pEspionage->m_aPlayerStealableGWList[uiDefendingPlayer].size(), "Random roll to see if we should attempt a coup");
			int iCityLoop;
			CvCity* pPlayerCity = NULL;
			int iGreatWorkIndex;
			for (pPlayerCity = GET_PLAYER(eDefendingPlayer).firstCity(&iCityLoop); pPlayerCity != NULL; pPlayerCity = GET_PLAYER(eDefendingPlayer).nextCity(&iCityLoop))
			{
				if (iStuffStolen > 0)
				{
					pEspionage->m_aiNumGWToStealList[uiDefendingPlayer] = 0;
					break;
				}
				if (pPlayerCity != NULL)
				{
					for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
					{
						if (iStuffStolen > 0)
						{
							pEspionage->m_aiNumGWToStealList[uiDefendingPlayer] = 0;
							break;
						}
						const CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(eDefendingPlayer).getCivilizationInfo();
						BuildingTypes eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
						if (eBuilding != NO_BUILDING)
						{
							CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
							if (pkBuilding)
							{
								if (pPlayerCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0 && (pkBuilding->GetGreatWorkSlotType() == eArtArtifactSlot || pkBuilding->GetGreatWorkSlotType() == eMusicSlot || pkBuilding->GetGreatWorkSlotType() == eWritingSlot))
								{
									int iNumSlots = pkBuilding->GetGreatWorkCount();
									if (iNumSlots > 0)
									{
										for (int iI = 0; iI < iNumSlots; iI++)
										{
											if (iStuffStolen > 0)
											{
												pEspionage->m_aiNumGWToStealList[uiDefendingPlayer] = 0;
												break;
											}
											iGreatWorkIndex = pPlayerCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
											if (iGreatWorkIndex != -1 && iGreatWorkIndex == pEspionage->m_aPlayerStealableGWList[uiDefendingPlayer][iGrab].m_iGreatWorkIndex)
											{
												// and create great work at home
												BuildingClassTypes eGWBuildingClass;
												int iGWSlot;
												CvCity *pArtCity = m_pPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(pPlayerCity->getX(), pPlayerCity->getY(), pkBuilding->GetGreatWorkSlotType(), &eGWBuildingClass, &iGWSlot);
												if (pArtCity)
												{
													// remove existing great works
													pPlayerCity->GetCityBuildings()->SetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI, -1);
													pArtCity->GetCityBuildings()->SetBuildingGreatWork(eGWBuildingClass, iGWSlot, iGreatWorkIndex);
													iStuffStolen++;

													pPlayerCity->GetCityCulture()->UpdateThemingBonusIndex((BuildingClassTypes)iBuildingClassLoop);
													pArtCity->GetCityCulture()->UpdateThemingBonusIndex(eGWBuildingClass);
													if ((GC.getLogging() && GC.getAILogging()))
													{
														CvGameCulture *pCulture = GC.getGame().GetGameCulture();
														if (pCulture)
														{
															CvString strMsg;
															strMsg.Format("Great Work STOLEN by Spy! Name: %s. Stolen from: %s ", pCulture->GetGreatWorkName(iGreatWorkIndex).GetCString(), pPlayerCity->getNameKey());
															strMsg += ",";
															strMsg += ",";
															strMsg += GET_PLAYER(eDefendingPlayer).getCivilizationShortDescription();
															strMsg += ",";
															pEspionage->LogEspionageMsg(strMsg);
														}
													}
													if (m_pPlayer->GetID() == GC.getGame().getActivePlayer())
													{
														CvPopupInfo kPopup(BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, iGreatWorkIndex);
														GC.GetEngineUserInterface()->AddPopup(kPopup);
													}
													pEspionage->m_aiNumSpyActionsDone[eDefendingPlayer]++;

													m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_SPY_ATTACK, false, NO_GREATPERSON, NO_BUILDING, 1);

													// send out notifications to the parties that were stolen from
													CvPlot* pPlot = GC.getMap().plot(pEspionage->m_aHeistLocations[uiDefendingPlayer][iHeistLocationCounter].m_iX, pEspionage->m_aHeistLocations[uiDefendingPlayer][iHeistLocationCounter].m_iY);
													CvAssertMsg(pPlot, "Could not find plot location for HeistLocation");
													if (pPlot)
													{
														CvCity* pCity = pPlot->getPlotCity();
														CvAssertMsg(pCity, "Plot does not contain a city");
														if (pCity)
														{
															CvAssertMsg(pCity->getOwner() == uiDefendingPlayer, "City does not belong to player. This is very strange");
															CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
															CvAssertMsg(pCityEspionage, "City did not have CityEspionage data");
															if (pCityEspionage)
															{
																CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(eDefendingPlayer).GetEspionage();
																if (pDefendingPlayerEspionage)
																{
																	CvGameCulture *pCulture = GC.getGame().GetGameCulture();
																	if (pCulture)
																	{
#if defined(MOD_BALANCE_CORE_SPIES)
																		pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[m_pPlayer->GetID()], NO_TECH, NO_BUILDING, NO_UNIT, false, 0, 0, false, iGreatWorkIndex);
#else
																		pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[m_pPlayer->GetID()], NO_TECH);
																	
#endif
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			iHeistLocationCounter++;

			// recalculate the num techs to steal list
			pEspionage->BuildStealableGWList((PlayerTypes)uiDefendingPlayer);
			if (pEspionage->m_aPlayerStealableGWList[uiDefendingPlayer].size() > 0 && pEspionage->m_aiNumGWToStealList[uiDefendingPlayer] > 0)
			{
				pEspionage->m_aiNumGWToStealList[uiDefendingPlayer]--;
			}
			else
			{
				pEspionage->m_aiNumGWToStealList[uiDefendingPlayer] = 0;
			}
		}
	}
}
#endif
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
#if defined(MOD_BALANCE_CORE)
void CvEspionageAI::UpdateCivOutOfGWTurn()
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();

	// determine which civs have run out of techs to steal
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eOtherPlayer = (PlayerTypes)ui;

		if (m_pPlayer->GetID() == eOtherPlayer)
		{
			continue;
		}

		// if surveillance has not been established, continue
		if (!pEspionage->IsAnySurveillanceEstablished(eOtherPlayer))
		{
			continue;
		}

		pEspionage->BuildStealableGWList(eOtherPlayer);
		if (pEspionage->m_aPlayerStealableGWList[eOtherPlayer].size() > 0)
		{
			continue;
		}
		m_aiCivOutOfGWTurn[eOtherPlayer] = GC.getGame().getGameTurn();
		if (GC.getLogging())
		{
			CvString strMsg;
			strMsg.Format("AI no more Great Works to steal,");
			strMsg += ",";
			strMsg += ",";
			strMsg += GET_PLAYER(eOtherPlayer).getCivilizationShortDescription();
			strMsg += ",";
			pEspionage->LogEspionageMsg(strMsg);
		}
	}
}
#endif

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
		int iRoll = GC.getGame().getJonRandNum(100, "Random roll to see if we should attempt a coup");
		if (iRoll < iChanceOfSuccess)
		{
			pEspionage->AttemptCoup(uiSpy);
		}
	}
}

// Figure out how the spies should be allocated
void CvEspionageAI::FindTargetSpyNumbers(int* piTargetOffensiveSpies, int* piTargetDefensiveSpies, int* piTargetCityStateSpies, int* piTargetDiplomatSpies)
{
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();

	int iNumRemainingSpies = m_pPlayer->GetEspionage()->GetNumAliveSpies();
	if(iNumRemainingSpies == 0)
	{
		*piTargetDefensiveSpies = 0;
		*piTargetOffensiveSpies = 0;
		*piTargetCityStateSpies = 0;
		*piTargetDiplomatSpies  = 0;
		return;
	}

	bool bAllocatedOffensiveSpies = false;
	bool bAllocatedDefensiveSpies = false;
	bool bAllocatedCityStateSpies = false;
	bool bAllocatedDiplomatSpies  = false;
	float fTechPositionRatio = m_pPlayer->GetPlayerTechs()->GetTechAI()->GetTechRatio();
#if defined(MOD_BALANCE_CORE)
	//Increase this slightly to encourage more offensive spies.
	fTechPositionRatio += 0.10f;
#endif
	// if going for diplo or conquest, evaluate the offensive spies before assigning CS spies
#if defined(MOD_BALANCE_CORE)
	if (pDiploAI->IsGoingForDiploVictory() && GC.getGame().GetGameLeagues()->GetActiveLeague() != NULL)
#else
	if (pDiploAI->IsGoingForDiploVictory())
#endif
	{
		// find out how many diplomats we need
		float fNumDiplomats = 0.0f;
		for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			PlayerTypes ePlayer = (PlayerTypes)ui;
			// must not be me and must be alive
			if (ePlayer == m_pPlayer->GetID() || !GET_PLAYER(ePlayer).isAlive())
			{
				continue;
			}

			CvLeagueAI::DiplomatUsefulnessLevels eUsefulnessLevel = m_pPlayer->GetLeagueAI()->GetDiplomatUsefulnessAtCiv(ePlayer);
			switch (eUsefulnessLevel)
			{
			case CvLeagueAI::DIPLOMAT_USEFULNESS_HIGH:
				fNumDiplomats += 1.0f;
				break;
			case CvLeagueAI::DIPLOMAT_USEFULNESS_MEDIUM:
				fNumDiplomats += 0.5f;
				break;
			case CvLeagueAI::DIPLOMAT_USEFULNESS_LOW:
				fNumDiplomats += 0.25f;
				break;
			case CvLeagueAI::DIPLOMAT_USEFULNESS_NONE:
				break;
			}
		}

		// assign spies to be diplomats
		int iNumDiplomats = (int)floor(fNumDiplomats);
#if defined(MOD_BALANCE_CORE_SPIES)
		int iFlavorDiplomacy = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
		iNumDiplomats += (int)(iFlavorDiplomacy * fNumDiplomats);
#endif
		*piTargetDiplomatSpies = min(iNumRemainingSpies, iNumDiplomats);
		bAllocatedDiplomatSpies = true;
		iNumRemainingSpies -= *piTargetDiplomatSpies;

		// assign offensive spies according to tech position
		*piTargetOffensiveSpies = (int)ceil(fTechPositionRatio * iNumRemainingSpies);
		bAllocatedOffensiveSpies = true;
		iNumRemainingSpies -= *piTargetOffensiveSpies;

		int iNumCityStates = 0;
		for(uint ui = MAX_MAJOR_CIVS; ui < MAX_CIV_PLAYERS; ui++)
		{
			PlayerTypes ePlayer = (PlayerTypes)ui;
			if (!GET_PLAYER(ePlayer).isAlive())
			{
				continue;
			}

			if(!GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam()))
			{
				continue;
			}

			// don't interact with city-states that are at war with us
			if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(m_pPlayer->getTeam()))
			{
				continue;
			}

			if (GET_PLAYER(ePlayer).isMinorCiv())
			{
				iNumCityStates++;
			}
		}

		*piTargetCityStateSpies = min(iNumRemainingSpies, iNumCityStates);
		bAllocatedCityStateSpies = true;
	}
	else if (pDiploAI->IsGoingForWorldConquest())
	{
		// assign offensive spies according to tech position
		*piTargetOffensiveSpies = (int)ceil(fTechPositionRatio * iNumRemainingSpies);
		bAllocatedOffensiveSpies = true;
		iNumRemainingSpies -= *piTargetOffensiveSpies;

		// Assign the rest to militaristic city-states
		// for now, assign spies to the militaristic city states
		int iNumMilitaristicCityStates = 0;
		for(uint ui = MAX_MAJOR_CIVS; ui < MAX_CIV_PLAYERS; ui++)
		{
			PlayerTypes ePlayer = (PlayerTypes)ui;
			if (!GET_PLAYER(ePlayer).isAlive())
			{
				continue;
			}

			if(!GET_TEAM(m_pPlayer->getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam()))
			{
				continue;
			}

			// don't interact with city-states that are at war with us
			if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(m_pPlayer->getTeam()))
			{
				continue;
			}

			if (GET_PLAYER(ePlayer).isMinorCiv() && GET_PLAYER(ePlayer).GetMinorCivAI()->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
			{
				iNumMilitaristicCityStates++;
			}
		}

		*piTargetCityStateSpies = min(iNumRemainingSpies, iNumMilitaristicCityStates);
		bAllocatedCityStateSpies = true;
	}
	else if (pDiploAI->IsGoingForCultureVictory())
	{
		// assign two spies to be diplomats
#if defined(MOD_BALANCE_CORE_SPIES)
		int iFlavorDiplomacy = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
		int iNumDiplomats = min(iNumRemainingSpies, (iFlavorDiplomacy / 3));
#else
		int iNumDiplomats = min(iNumRemainingSpies, 2);
#endif
		iNumDiplomats = min(iNumDiplomats, m_pPlayer->GetCulture()->GetMaxPropagandaDiplomatsWanted());
		*piTargetDiplomatSpies = iNumDiplomats;

		if (iNumDiplomats > 0)
		{
			bAllocatedDiplomatSpies = true;
			iNumRemainingSpies -= *piTargetDiplomatSpies;
		}
	}
	else if (pDiploAI->IsGoingForSpaceshipVictory())
	{
		// assign none to CS
		*piTargetCityStateSpies = 0;
		bAllocatedCityStateSpies = true;
	}
	else
	{
		// assign 1/3rd to CS
		*piTargetCityStateSpies = iNumRemainingSpies / 3;
		bAllocatedCityStateSpies = true;
	}

	iNumRemainingSpies -= *piTargetCityStateSpies;
	if (!bAllocatedOffensiveSpies)
	{
		*piTargetOffensiveSpies = (int)ceil(fTechPositionRatio * iNumRemainingSpies);
		bAllocatedOffensiveSpies = true;
		iNumRemainingSpies -= *piTargetOffensiveSpies;
	}

	*piTargetDefensiveSpies = iNumRemainingSpies;
	bAllocatedDefensiveSpies = true;

	// rationalization
	std::vector<CvCity*> apOtherCityList;
	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes)ui;

		// don't count the player's own cities
		if(eTargetPlayer == m_pPlayer->GetID())
		{
			continue;
		}

		for(pLoopCity = GET_PLAYER(eTargetPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eTargetPlayer).nextCity(&iLoop))
		{
			CvPlot* pCityPlot = pLoopCity->plot();
			CvAssertMsg(pCityPlot, "City plot is null!");
			if(pCityPlot)
			{
				if(pCityPlot->isRevealed(m_pPlayer->getTeam()))
				{
					apOtherCityList.push_back(pLoopCity);
				}
			}
		}
	}

	while(*piTargetDefensiveSpies > m_pPlayer->getNumCities())
	{
		if(*piTargetOffensiveSpies < (int)apOtherCityList.size())
		{
			(*piTargetDefensiveSpies)--;
			(*piTargetOffensiveSpies)++;
		}
		else
		{
			// spies will be unused
			(*piTargetDefensiveSpies)--;
		}
	}

	while(*piTargetOffensiveSpies > (int)apOtherCityList.size())
	{
		if(*piTargetDefensiveSpies < m_pPlayer->getNumCities())
		{
			(*piTargetDefensiveSpies)++;
			(*piTargetOffensiveSpies)--;
		}
		else
		{
			// spies will be unused
			(*piTargetOffensiveSpies)--;
		}
	}
}

struct ScoreCityEntry
{
	CvCity* m_pCity;
	int m_iScore;
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

void CvEspionageAI::BuildDiplomatCityList(EspionageCityList& aDiplomatCityList)
{
	aDiplomatCityList.clear();
	CvPlayerCulture* pCulture = m_pPlayer->GetCulture();
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();

	if (pDiploAI->IsGoingForCultureVictory())
	{
		for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			PlayerTypes eTargetPlayer = (PlayerTypes)ui;
			if (pCulture->WantsDiplomatDoingPropaganda(eTargetPlayer) && GET_PLAYER(eTargetPlayer).getCapitalCity() != NULL)
			{
				aDiplomatCityList.push_back(GET_PLAYER(eTargetPlayer).getCapitalCity());
			}
		}
	}
	else // Going for diplo victory and others
	{
		std::vector<ScoreCityEntry> aCityScores;
		for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
		{
			PlayerTypes eTargetPlayer = (PlayerTypes)ui;

			// don't count the player's own cities
			if(eTargetPlayer == m_pPlayer->GetID() || !GET_PLAYER(eTargetPlayer).isAlive())
			{
				continue;
			}

			if (m_pPlayer->GetLeagueAI()->GetDiplomatUsefulnessAtCiv(eTargetPlayer) == CvLeagueAI::DIPLOMAT_USEFULNESS_NONE)
			{
				continue;
			}

			CvCity* pCapitalCity = GET_PLAYER(eTargetPlayer).getCapitalCity();
			if (!pCapitalCity)
			{
				continue;
			}

			// if we can't see it, we can't move a diplomat there.
			if (!pCapitalCity->isRevealed(m_pPlayer->getTeam(), false))
			{
				continue;
			}

			ScoreCityEntry kEntry;
			kEntry.m_pCity = pCapitalCity;
			kEntry.m_iScore = m_pPlayer->GetLeagueAI()->GetDiplomatUsefulnessAtCiv(eTargetPlayer);
			aCityScores.push_back(kEntry);
		}

		// sort
		std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryHighEval());

		// transfer values to OffenseCityList
		for(uint ui = 0; ui < aCityScores.size(); ui++)
		{
			aDiplomatCityList.push_back(aCityScores[ui].m_pCity);
		}
	}
}

void CvEspionageAI::BuildOffenseCityList(EspionageCityList& aOffenseCityList)
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();

	aOffenseCityList.clear();
	PlayerTypes ePlayer = m_pPlayer->GetID();
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();
#if !defined(MOD_BALANCE_CORE_SPIES)
	// find the most expensive available research
	int iMaxTechCost = -1;
	CvPlayerTechs* pMyPlayerTechs = m_pPlayer->GetPlayerTechs();

	for(int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
	{
		const TechTypes eTech = static_cast<TechTypes>(iTechLoop);
		CvTechEntry* pkTechInfo = GC.getTechInfo(eTech);
		if(!pkTechInfo)
		{
			continue;
		}

		// try to find the most expensive tech that can be researched
		if(pMyPlayerTechs->CanResearch(eTech))
		{
			int iTechCost = m_pPlayer->GetPlayerTechs()->GetResearchCost(eTech) * 100;
			if(iTechCost > iMaxTechCost)
			{
				iMaxTechCost = iTechCost;
			}
		}
	}
#endif
	std::vector<ScoreCityEntry> aCityScores;
	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes eTargetPlayer = (PlayerTypes)ui;

		// don't count the player's own cities
		if(eTargetPlayer == ePlayer)
		{
			continue;
		}

		TeamTypes eTargetTeam = GET_PLAYER(eTargetPlayer).getTeam();
		CvDiplomacyAI* pTargetDiploAI = GET_PLAYER(eTargetPlayer).GetDiplomacyAI();

		for(pLoopCity = GET_PLAYER(eTargetPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eTargetPlayer).nextCity(&iLoop))
		{
			CvPlot* pCityPlot = pLoopCity->plot();
			CvAssertMsg(pCityPlot, "City plot is null!");
			if(!pCityPlot)
			{
				continue;
			}

			if(!pCityPlot->isRevealed(GET_PLAYER(ePlayer).getTeam()))
			{
				continue;
			}
#if !defined(MOD_BALANCE_CORE_SPIES)
			CvCityEspionage* pCityEspionage = pLoopCity->GetCityEspionage();
			int iNumRemainingTurns = MAX_INT;
			int iSpyIndex = pEspionage->GetSpyIndexInCity(pLoopCity);
			// if there is a spy currently stealing there, use the actual data
			if(iSpyIndex >= 0 && pEspionage->m_aSpyList[iSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL)
			{
				if(pCityEspionage->m_aiRate[ePlayer] != 0)
				{
					iNumRemainingTurns = (pCityEspionage->m_aiGoal[ePlayer] - pCityEspionage->m_aiAmount[ePlayer]) / pCityEspionage->m_aiRate[ePlayer];
				}
			}
			// if we have any history with the city, use that data instead
			else if(pCityEspionage->m_aiLastBasePotential[m_pPlayer->GetID()] > 0)
			{
				int iCost = iMaxTechCost;
				if(pEspionage->m_aiMaxTechCost[eTargetPlayer] > 0)
				{
					iCost = pEspionage->m_aiMaxTechCost[eTargetPlayer];
				}

				iNumRemainingTurns = iCost / pCityEspionage->m_aiLastBasePotential[m_pPlayer->GetID()];
			}
			// estimate the number of turns using the population
			else
			{
				int iCost = iMaxTechCost;
				if(pEspionage->m_aiMaxTechCost[eTargetPlayer] > 0)
				{
					iCost = pEspionage->m_aiMaxTechCost[eTargetPlayer];
				}
				if(pLoopCity->getPopulation() > 0)
				{
					iNumRemainingTurns = iCost / (pLoopCity->getPopulation() * 100);
				}
			}
#endif
#if defined(MOD_BALANCE_CORE_SPIES)
			int iValue = pLoopCity->GetRank() * 10;
			if(iValue <= 0)
				continue;
			//Randomness to try and make the AI less spammy
			iValue += GC.getGame().getJonRandNum(iValue, "Randomizing amount of turns left to make AI less spammy");
			int iAdvancedPenalty = pLoopCity->GetBlockBuildingDestruction() + pLoopCity->GetBlockWWDestruction() + pLoopCity->GetBlockUDestruction() + pLoopCity->GetBlockGPDestruction() + pLoopCity->GetBlockGold() + pLoopCity->GetBlockScience() + pLoopCity->GetBlockRebellion() + pLoopCity->GetBlockUnrest();
			if(iAdvancedPenalty > 0)
			{
				iValue -= min(iValue, iAdvancedPenalty * 10);
			}
			if (pLoopCity->GetCityEspionage()->HasCounterSpy())
			{
				iValue -= 50;
			}
			int iDiploModifier = 100;
			if (pDiploAI->GetWarGoal(eTargetPlayer) == WAR_GOAL_PREPARE)
			{
				iDiploModifier += 25;
			}
			else if (GET_TEAM(eTeam).isAtWar(eTargetTeam))
			{
				// ignore promises
				// bonus targeting!
				iDiploModifier += 50;
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
									iDiploModifier += 100;
								}
							}
							if (pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_UNIT_COUP_CITY))
							{
								if (pMinorCivAI->GetQuestData1(m_pPlayer->GetID(), MINOR_CIV_QUEST_UNIT_COUP_CITY) == eTargetPlayer)
								{
									iDiploModifier += 100;
								}
							}
						}
					}
				}
				if (GET_PLAYER(eTargetPlayer).isMajorCiv())
				{
					// if we promised not to spy, make it less likely that we will spy
					if (pDiploAI->IsPlayerStopSpyingRequestAccepted(eTargetPlayer))
					{
						// target far less frequently
						iDiploModifier -= 300;
					}

					// if we've denounced them or they've denounced us, spy bonus!
					if (pDiploAI->IsDenouncedPlayer(eTargetPlayer) || pTargetDiploAI->IsDenouncedPlayer(ePlayer))
					{
						iDiploModifier += 50;
					}
					else if (pDiploAI->IsDoFAccepted(eTargetPlayer))
					{
						iDiploModifier -= 300;
					}

					if (GET_TEAM(eTeam).IsHasResearchAgreement(eTargetTeam))
					{
						iDiploModifier -= 100;
					}

					if (GET_TEAM(eTeam).IsHasDefensivePact(eTargetTeam))
					{
						iDiploModifier -= 100;
					}

					if (GET_TEAM(eTeam).IsAllowsOpenBordersToTeam(eTargetTeam))
					{
						iDiploModifier -= 33;
					}

					if (GET_TEAM(eTargetTeam).IsAllowsOpenBordersToTeam(eTeam))
					{
						iDiploModifier += 33;
					}
					if (pLoopCity->isCapital())
					{
						iDiploModifier += 50;
					}
					if (pDiploAI->GetMajorCivApproach(eTargetPlayer, false) == MAJOR_CIV_APPROACH_FRIENDLY)
					{
						iDiploModifier -= 50;
					}
					if (pDiploAI->GetMajorCivApproach(eTargetPlayer, false) == MAJOR_CIV_APPROACH_DECEPTIVE)
					{
						iDiploModifier += 100;
					}
					//Spread our spies out a bit.
					for (uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
					{
						CvCity* pCity = pEspionage->GetCityWithSpy(uiSpy);
						if (pCity)
						{
							if (pCity->getOwner() == eTargetPlayer)
							{
								iDiploModifier -= 100;
								break;
							}
						}
					}
					if (!pLoopCity->isCapital() && MOD_BALANCE_CORE_SPIES_ADVANCED)
					{
						int iNumGreatWorks = pLoopCity->GetCityBuildings()->GetNumGreatWorks();
						if (iNumGreatWorks > 0)
						{
							if (m_pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory() || m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory() || GET_PLAYER(eTargetPlayer).GetDiplomacyAI()->IsCloseToCultureVictory())
							{
								iDiploModifier += 50 * iNumGreatWorks;
							}
						}
					}
				}
			}
			ScoreCityEntry kEntry;
			kEntry.m_pCity = pLoopCity;

			iValue *= (100 + iDiploModifier);
			iValue /= 100;

			if (iValue <= 0)
				iValue = 1;

			kEntry.m_iScore = iValue;

			aCityScores.push_back(kEntry);
		}
	}
	// sort
	std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryHighEval());

	// transfer values to OffenseCityList
	for(uint ui = 0; ui < aCityScores.size(); ui++)
	{
		aOffenseCityList.push_back(aCityScores[ui].m_pCity);
	}
#else
			int iDiploModifier = 1;

			if (pDiploAI->GetWarGoal(eTargetPlayer) == WAR_GOAL_PREPARE)
			{
				iDiploModifier = 1;
			}
			else if (GET_TEAM(eTeam).isAtWar(eTargetTeam))
			{
				// ignore promises
				// bonus targeting!
				iDiploModifier = 1;
			}
			else // we're not at war with them, so look at other factors
			{
				// raise our diplo modifier by a scale of 10 so that we're less likely to target those we aren't at war with
				iDiploModifier = 10;

				// if we promised not to spy, make it less likely that we will spy
				if (pDiploAI->IsPlayerStopSpyingRequestAccepted(eTargetPlayer))
				{
					// target far less frequently
					iDiploModifier *= 100;
				}

				// if we've denounced them or they've denounced us, spy bonus!
				if (pDiploAI->IsDenouncedPlayer(eTargetPlayer) || pTargetDiploAI->IsDenouncedPlayer(ePlayer))
				{
					iDiploModifier /= 2;
				}
				else if (pDiploAI->IsDoFAccepted(eTargetPlayer))
				{
					iDiploModifier *= 50;
				}

				if (GET_TEAM(eTeam).IsHasResearchAgreement(eTargetTeam))
				{
					iDiploModifier *= 5;
				}

				if (GET_TEAM(eTeam).IsHasDefensivePact(eTargetTeam))
				{
					iDiploModifier *= 50;
				}

				if (GET_TEAM(eTeam).IsAllowsOpenBordersToTeam(eTargetTeam))
				{
					iDiploModifier *= 2;
				}

				if (GET_TEAM(eTargetTeam).IsAllowsOpenBordersToTeam(eTeam))
				{
					iDiploModifier *= 2;
				}
			}
			ScoreCityEntry kEntry;
			kEntry.m_pCity = pLoopCity;

			int iScore = iNumRemainingTurns * iDiploModifier;
			if (m_aiCivOutOfTechTurn[eTargetPlayer] == GC.getGame().getGameTurn())
			{
				iScore = MAX_INT;
			}
			else if(m_aiCivOutOfTechTurn[eTargetPlayer] > 0)
			{
				iScore = iScore * m_aiCivOutOfTechTurn[eTargetPlayer];
			}

			kEntry.m_iScore = iScore;

			aCityScores.push_back(kEntry);

		}
	}

	// sort
	std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryLowEval());

	// transfer values to OffenseCityList
	for(uint ui = 0; ui < aCityScores.size(); ui++)
	{
		aOffenseCityList.push_back(aCityScores[ui].m_pCity);
	}
#endif
}

void CvEspionageAI::BuildDefenseCityList(EspionageCityList& aDefenseCityList)
{
#if !defined(MOD_BALANCE_CORE_SPIES)
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
#endif

	aDefenseCityList.clear();
	PlayerTypes ePlayer = m_pPlayer->GetID();

	std::vector<ScoreCityEntry> aCityScores;

	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		ScoreCityEntry kEntry;
		kEntry.m_pCity = pLoopCity;
#if defined(MOD_BALANCE_CORE_SPIES)
		kEntry.m_iScore = pLoopCity->GetRank();
		if(pLoopCity->isCapital())
		{
			kEntry.m_iScore += 1;
		}
		if(kEntry.m_iScore <= 0)
			continue;
#else
		kEntry.m_iScore = pEspionage->CalcPerTurn(SPY_STATE_GATHERING_INTEL, pLoopCity, -1);
#endif
		aCityScores.push_back(kEntry);
	}

	// sort
	std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryHighEval());

	// transfer values to aDefenseCityList
	for(uint ui = 0; ui < aCityScores.size(); ui++)
	{
		aDefenseCityList.push_back(aCityScores[ui].m_pCity);
	}
}

void CvEspionageAI::BuildMinorCityList(EspionageCityList& aMinorCityList)
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	CvDiplomacyAI* pDiploAI = m_pPlayer->GetDiplomacyAI();

	aMinorCityList.clear();

	int iGrowthFlavor = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) GC.getInfoTypeForString("FLAVOR_GROWTH"));
	PlayerTypes eCurrentDiploThreat = NO_PLAYER;
	int iCityStatePlan = GetCityStatePlan(&eCurrentDiploThreat);

	std::vector<ScoreCityEntry> aCityScores;
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
		CvMinorCivInfo* pMinorInfo = GC.getMinorCivInfo(pMinorCivAI->GetMinorCivType());
		MinorCivApproachTypes eApproach = pDiploAI->GetMinorCivApproach(eTargetPlayer);
		int iFriendshipWithMinor = pMinorCivAI->GetEffectiveFriendshipWithMajor(m_pPlayer->GetID());
		
		bool bAlreadyScoredCity = false;
		for (pLoopCity = GET_PLAYER(eTargetPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eTargetPlayer).nextCity(&iLoop))
		{
			if(!pEspionage->CanEverMoveSpyTo(pLoopCity))
			{
				continue;
			}

			// if we've already scored a city for this minor civ, we don't need to score another because the values would be the same
			if (bAlreadyScoredCity)
			{
				break;
			}
			bAlreadyScoredCity = true;

			ScoreCityEntry kEntry;
			kEntry.m_pCity = pLoopCity;

			int iValue = 100;
			switch (iCityStatePlan)
			{
			case PLAN_PLAY_NORMAL:
				// If we're not protective or friendly, then don't bother with minor diplo
				if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE || eApproach == MINOR_CIV_APPROACH_FRIENDLY)
				{
					// Nearly everyone likes to grow
					if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_MARITIME && !m_pPlayer->IsEmpireUnhappy())
					{
						iValue += /*20*/ GC.getMC_GIFT_WEIGHT_MARITIME_GROWTH() * iGrowthFlavor * max(1, m_pPlayer->getNumCities() / 3);
					}

					// Slight negative weight towards militaristic
					if(pMinorInfo->GetMinorCivTrait() == MINOR_CIV_TRAIT_MILITARISTIC && !pDiploAI->IsGoingForWorldConquest())
					{
						iValue += /*-50*/ GC.getMC_GIFT_WEIGHT_MILITARISTIC();
					}

					// If they have a resource we don't have, add extra weight
					int iResourcesWeLack = pMinorCivAI->GetNumResourcesMajorLacks(m_pPlayer->GetID());
					if(iResourcesWeLack > 0)
					{
						iValue += (iResourcesWeLack* /*80*/ GC.getMC_GIFT_WEIGHT_RESOURCE_WE_NEED());
					}

					// If we're protective this is worth more than if we're friendly
					if(eApproach == MINOR_CIV_APPROACH_PROTECTIVE)
					{
						iValue += /*10*/ GC.getMC_GIFT_WEIGHT_PROTECTIVE();
					}

					// If the minor is hostile, then reduce the weighting
					if(pMinorCivAI->GetPersonality() == MINOR_CIV_PERSONALITY_HOSTILE)
					{
						iValue += /*-20*/ GC.getMC_GIFT_WEIGHT_HOSTILE();
					}

					// The closer we are the better
					if(m_pPlayer->GetProximityToPlayer(eTargetPlayer) == PLAYER_PROXIMITY_NEIGHBORS)
						iValue += /*5*/ GC.getMC_GIFT_WEIGHT_NEIGHBORS();
					else if(m_pPlayer->GetProximityToPlayer(eTargetPlayer) == PLAYER_PROXIMITY_CLOSE)
						iValue += /*4*/ GC.getMC_GIFT_WEIGHT_CLOSE();
					else if(m_pPlayer->GetProximityToPlayer(eTargetPlayer) == PLAYER_PROXIMITY_FAR)
						iValue += /*3*/ GC.getMC_GIFT_WEIGHT_FAR();

					// Did we bully you recently?  If so, giving you gold now would be very odd.
					if(pMinorCivAI->IsRecentlyBulliedByMajor(m_pPlayer->GetID()))
					{
						iValue -= 100; //antonjs: todo: constant/XML
					}

					// if we're allies
					if (pMinorCivAI->IsAllies(m_pPlayer->GetID()))
					{
						// and we're secure as their allies, ignore the scoring
						if (!pMinorCivAI->IsCloseToNotBeingAllies(m_pPlayer->GetID()))
						{
							iValue = 0;
						}
					}
					else
					{
						// count however much we've invested into the friendship toward maintaining the friendship
						iValue += iFriendshipWithMinor;
					}
				}

				if (pDiploAI->IsGoingForCultureVictory())
				{
					// no modifiers
				}
				else if (pDiploAI->IsGoingForWorldConquest())
				{
					if (pMinorCivAI->GetTrait() == MINOR_CIV_TRAIT_MILITARISTIC)
					{
						iValue += 999;
					}
				}
				else if (pDiploAI->IsGoingForDiploVictory())
				{
					// no modifiers
				}
				else if (pDiploAI->IsGoingForSpaceshipVictory())
				{
					// no modifiers
				}
				else // the player doesn't know how they're going to try to win yet
				{
					// no modifiers
				}
				break;

			case PLAN_COLLECT_VOTES:
				// if we're allies
				if (pMinorCivAI->IsAllies(m_pPlayer->GetID()))
				{
					// and we're secure as their allies, ignore the scoring
					if (!pMinorCivAI->IsCloseToNotBeingAllies(m_pPlayer->GetID()))
					{
						iValue = 0;
					}
				}
				else
				{
					// if the minor is allied with someone else
					if (pMinorCivAI->GetAlly() != NO_PLAYER)
					{
						iValue += iFriendshipWithMinor;
					}
					else
					{
						// count however much we've invested into the friendship toward maintaining the friendship
						iValue += iFriendshipWithMinor * 2;
					}
				}
				break;
			case PLAN_ATTACK_CS_TO_PREVENT_DEFEAT:
				// attack the least protected cities
				if (pMinorCivAI->IsAllies(eCurrentDiploThreat))
				{
					iValue = MAX_INT - pMinorCivAI->GetEffectiveFriendshipWithMajor(eCurrentDiploThreat);
				}
				break;
			case PLAN_DEFEND_CS_FOR_WIN:
				// defend the least defended cities!
				if (pMinorCivAI->IsAllies(eCurrentDiploThreat))
				{
					iValue = MAX_INT - pMinorCivAI->GetEffectiveFriendshipWithMajor(eCurrentDiploThreat);
				}
			}

			kEntry.m_iScore = iValue;
			aCityScores.push_back(kEntry);
		}
	}

	// sort
	std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryHighEval());

	// transfer values to OffenseCityList
	for(uint ui = 0; ui < aCityScores.size(); ui++)
	{
		aMinorCityList.push_back(aCityScores[ui].m_pCity);
	}
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
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
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
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
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
			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: defensive spy, %d,", ui);
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
				pEspionage->LogEspionageMsg(strMsg);
			}

		}
	}
}

// does not move the spies, only flags them to be moved
void CvEspionageAI::EvaluateDiplomatSpies(void)
{
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
			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: diplomat spy, %d,", ui);
#if defined(MOD_BUGFIX_SPY_NAMES)
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
#else
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
#endif
				pEspionage->LogEspionageMsg(strMsg);
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
#if defined(MOD_BALANCE_CORE)
	loadFrom >> uiCount;
	for (uint ui = 0; ui < uiCount; ui++)
	{
		loadFrom >> iValue;
		writeTo.m_aiCivOutOfGWTurn.push_back(iValue);
	}
#endif

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
#if defined(MOD_BALANCE_CORE)
	saveTo << readFrom.m_aiCivOutOfGWTurn.size();
	for (uint ui = 0; ui < readFrom.m_aiCivOutOfGWTurn.size(); ui++)
	{
		saveTo << readFrom.m_aiCivOutOfGWTurn[ui];
	}
#endif

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