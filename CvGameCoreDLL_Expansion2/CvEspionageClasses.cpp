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
const int iSpyTurnsToMakeIntroductions = 5;
const int iIntrigueTurnsValid = 5;
const int iSpyRankPower = 50;
PlayerTypes g_eSortPlayer = NO_PLAYER; // global - used for the sort

// ================================================================================
//			CvSpyPassiveBonusEntry
// ================================================================================
CvSpyPassiveBonusEntry::CvSpyPassiveBonusEntry(void)
{
	m_iNetworkPointsNeeded = 0;
	m_iSciencePercentAdded = 0;
	m_iTilesRevealed = 0;
	m_bRevealCityScreen = false;
	m_bWonderConstructionNotification = false;
}

CvSpyPassiveBonusEntry::~CvSpyPassiveBonusEntry(void)
{
}

bool CvSpyPassiveBonusEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
	{
		return false;
	}

	m_iNetworkPointsNeeded = kResults.GetInt("NetworkPointsNeeded");
	m_iSciencePercentAdded = kResults.GetInt("SciencePercentAdded");
	m_iTilesRevealed = kResults.GetInt("TilesRevealed");
	m_bRevealCityScreen = kResults.GetBool("RevealCityScreen");
	m_bWonderConstructionNotification = kResults.GetBool("WonderConstructionNotification");

	return true;
}

int CvSpyPassiveBonusEntry::GetNetworkPointsNeededScaled() const
{
	int iNPNeeded = m_iNetworkPointsNeeded;
	iNPNeeded *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iNPNeeded /= 100;
	return iNPNeeded;
}
int CvSpyPassiveBonusEntry::GetSciencePercentAdded() const
{
	return m_iSciencePercentAdded;
}
int CvSpyPassiveBonusEntry::GetTilesRevealed() const
{
	return m_iTilesRevealed;
}
bool CvSpyPassiveBonusEntry::IsRevealCityScreen() const
{
	return m_bRevealCityScreen;
}
bool CvSpyPassiveBonusEntry::IsWonderConstructionNotification() const
{
	return m_bWonderConstructionNotification;
}

// ================================================================================
//			CvSpyPassiveBonusXMLEntries
// ================================================================================
CvSpyPassiveBonusXMLEntries::CvSpyPassiveBonusXMLEntries(void)
{
}

CvSpyPassiveBonusXMLEntries::~CvSpyPassiveBonusXMLEntries(void)
{
	DeleteArray();
}

// Returns vector of spy passive bonus entries
std::vector<CvSpyPassiveBonusEntry*>& CvSpyPassiveBonusXMLEntries::GetSpyPassiveBonusEntries()
{
	return m_paSpyPassiveBonusEntries;
}

// Returns number of defined spy passive bonuses
int CvSpyPassiveBonusXMLEntries::GetNumSpyPassiveBonuses()
{
	return m_paSpyPassiveBonusEntries.size();
}

// Clears spy passive bonus entries
void CvSpyPassiveBonusXMLEntries::DeleteArray()
{
	for (std::vector<CvSpyPassiveBonusEntry*>::iterator it = m_paSpyPassiveBonusEntries.begin(); it != m_paSpyPassiveBonusEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paSpyPassiveBonusEntries.clear();
}

// Returns a specific spy passive bonus entry
CvSpyPassiveBonusEntry* CvSpyPassiveBonusXMLEntries::GetEntry(int index)
{
	return m_paSpyPassiveBonusEntries[index];
}


// ================================================================================
//			CvSpyPassiveBonusDiplomatEntry
// ================================================================================
CvSpyPassiveBonusDiplomatEntry::CvSpyPassiveBonusDiplomatEntry(void)
{
	m_iNetworkPointsNeeded = 0;
	m_bReceiveIntrigue = false;
	m_iTradeRouteGoldBonus = 0;
}

CvSpyPassiveBonusDiplomatEntry::~CvSpyPassiveBonusDiplomatEntry(void)
{
}

bool CvSpyPassiveBonusDiplomatEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
	{
		return false;
	}

	m_iNetworkPointsNeeded = kResults.GetInt("NetworkPointsNeeded");
	m_bReceiveIntrigue = kResults.GetBool("ReceiveIntrigue");
	m_iTradeRouteGoldBonus = kResults.GetInt("TradeRouteGoldBonus");

	return true;
}

int CvSpyPassiveBonusDiplomatEntry::GetNetworkPointsNeededScaled() const
{
	int iNPNeeded = m_iNetworkPointsNeeded;
	iNPNeeded *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iNPNeeded /= 100;
	return iNPNeeded;
}
bool CvSpyPassiveBonusDiplomatEntry::IsReceiveIntrigue() const
{
	return m_bReceiveIntrigue;
}
int CvSpyPassiveBonusDiplomatEntry::GetTradeRouteGoldBonus() const
{
	return m_iTradeRouteGoldBonus;
}

// ================================================================================
//			CvSpyPassiveBonusDiplomatXMLEntries
// ================================================================================
CvSpyPassiveBonusDiplomatXMLEntries::CvSpyPassiveBonusDiplomatXMLEntries(void)
{
}

CvSpyPassiveBonusDiplomatXMLEntries::~CvSpyPassiveBonusDiplomatXMLEntries(void)
{
	DeleteArray();
}

// Returns vector of spy passive bonus diplomat entries
std::vector<CvSpyPassiveBonusDiplomatEntry*>& CvSpyPassiveBonusDiplomatXMLEntries::GetSpyPassiveBonusDiplomatEntries()
{
	return m_paSpyPassiveBonusDiplomatEntries;
}

// Returns number of defined spy passive bonuses diplomat
int CvSpyPassiveBonusDiplomatXMLEntries::GetNumSpyPassiveBonusesDiplomat()
{
	return m_paSpyPassiveBonusDiplomatEntries.size();
}

// Clears spy passive bonus diplomat entries
void CvSpyPassiveBonusDiplomatXMLEntries::DeleteArray()
{
	for (std::vector<CvSpyPassiveBonusDiplomatEntry*>::iterator it = m_paSpyPassiveBonusDiplomatEntries.begin(); it != m_paSpyPassiveBonusDiplomatEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paSpyPassiveBonusDiplomatEntries.clear();
}

// Returns a specific spy passive bonus diplomat entry
CvSpyPassiveBonusDiplomatEntry* CvSpyPassiveBonusDiplomatXMLEntries::GetEntry(int index)
{
	return m_paSpyPassiveBonusDiplomatEntries[index];
}

//=====================================
// CvEspionageSpy
//=====================================
/// Default Constructor
CvEspionageSpy::CvEspionageSpy()
	: m_iName(-1)
	, m_sName(NULL)
	, m_eRank(SPY_RANK_RECRUIT)
	, m_iExperience(0)
	, m_iCityX(-1)
	, m_iCityY(-1)
	, m_eSpyState(SPY_STATE_UNASSIGNED)
	, m_iReviveCounter(0)
	, m_eSpyFocus(NO_EVENT_CHOICE_CITY)
	, m_bIsDiplomat(false)
	, m_bEvaluateReassignment(true)
	, m_bPassive(false)
	, m_iTurnCounterspyMissionChanged(0)
	, m_iTurnSpyImprisoned(0)
	, m_iTurnActiveMissionConducted(0)
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

CvSpyState CvEspionageSpy::GetSpyState() const
{
	return m_eSpyState;
}

void CvEspionageSpy::SetSpyState(PlayerTypes eSpyOwner, int iSpyIndex, CvSpyState eSpyState)
{
	m_eSpyState = eSpyState;

	if (MOD_EVENTS_ESPIONAGE) 
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_EspionageState, (int) eSpyOwner, iSpyIndex, (int) eSpyState, m_iCityX, m_iCityY);
	}
}

void CvEspionageSpy::SetSpyFocus(CityEventChoiceTypes eEventChoice)
{
	m_eSpyFocus = eEventChoice;
}
CityEventChoiceTypes CvEspionageSpy::GetSpyFocus() const
{
	return m_eSpyFocus;
}

void CvEspionageSpy::SetTurnCounterspyMissionChanged(int iTurn)
{
	m_iTurnCounterspyMissionChanged = iTurn;
}
int CvEspionageSpy::GetTurnCounterspyMissionChanged() const
{
	return m_iTurnCounterspyMissionChanged;
}

void CvEspionageSpy::SetTurnSpyImprisoned(int iTurn)
{
	m_iTurnSpyImprisoned = iTurn;
}
int CvEspionageSpy::GetTurnSpyImprisoned() const
{
	return m_iTurnSpyImprisoned;
}

void CvEspionageSpy::SetTurnActiveMissionConducted(int iTurn)
{
	m_iTurnActiveMissionConducted = iTurn;
}
int CvEspionageSpy::GetTurnActiveMissionConducted() const
{
	return m_iTurnActiveMissionConducted;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvEspionageSpy& writeTo)
{
	uint uiVersion = 0;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	loadFrom >> writeTo.m_iName;
	MOD_SERIALIZE_READ(53, loadFrom, writeTo.m_sName, NULL);
	int iSpyRank = 0;
	loadFrom >> iSpyRank;
	writeTo.m_eRank = (CvSpyRank)iSpyRank;
	loadFrom >> writeTo.m_iExperience;
	loadFrom >> writeTo.m_iCityX;
	loadFrom >> writeTo.m_iCityY;

	int iSpyState = 0;
	loadFrom >> iSpyState;
	writeTo.m_eSpyState = (CvSpyState)iSpyState;

	int iSpyFocus = 0;
	loadFrom >> iSpyFocus;
	writeTo.m_eSpyFocus = (CityEventChoiceTypes)iSpyFocus;

	loadFrom >> writeTo.m_iReviveCounter;
	loadFrom >> writeTo.m_bIsDiplomat;
	
	loadFrom >> writeTo.m_bEvaluateReassignment;

	MOD_SERIALIZE_READ(23, loadFrom, writeTo.m_bPassive, false);

	loadFrom >> writeTo.m_iTurnCounterspyMissionChanged;
	loadFrom >> writeTo.m_iTurnSpyImprisoned;
	loadFrom >> writeTo.m_iTurnActiveMissionConducted;

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
	saveTo << readFrom.m_iExperience;
	saveTo << readFrom.m_iCityX;
	saveTo << readFrom.m_iCityY;
	saveTo << (int)readFrom.m_eSpyState;
	saveTo << (int)readFrom.m_eSpyFocus;
	saveTo << readFrom.m_iReviveCounter;
	saveTo << readFrom.m_bIsDiplomat;
	saveTo << readFrom.m_bEvaluateReassignment;

	MOD_SERIALIZE_WRITE(saveTo, readFrom.m_bPassive);

	saveTo << readFrom.m_iTurnCounterspyMissionChanged;
	saveTo << readFrom.m_iTurnSpyImprisoned;
	saveTo << readFrom.m_iTurnActiveMissionConducted;

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

	m_eCounterspyEvent = NO_EVENT_CITY;
	m_eSpyMissionEvent = NO_EVENT_CITY;

	// create m_aiSpyListNameOrder list
	for(int i = 0; i < m_pPlayer->getCivilizationInfo().getNumSpyNames(); i++)
	{
		m_aiSpyListNameOrder.push_back(i);
	}

#if !defined(MOD_CORE_REDUCE_RANDOMNESS)
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
		m_aiNumTechsToStealList.push_back(0);
		m_aiNumSpyActionsDone.push_back(0);
	}
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

	m_eCounterspyEvent = NO_EVENT_CITY;
	m_eSpyMissionEvent = NO_EVENT_CITY;

	m_aiNumTechsToStealList.clear();
	m_aiNumSpyActionsDone.clear();
	m_aIntrigueNotificationMessages.clear();
	m_aaPlayerStealableTechList.clear();
}


/// DoTurn
void CvPlayerEspionage::DoTurn()
{
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
	kNewSpy.m_iExperience = 0;
	kNewSpy.m_eSpyState = SPY_STATE_UNASSIGNED;
	kNewSpy.m_eSpyFocus = NO_EVENT_CHOICE_CITY;
	GetNextSpyName(&kNewSpy);
	kNewSpy.m_bEvaluateReassignment = true;
	kNewSpy.m_bPassive = false;
	kNewSpy.m_iTurnCounterspyMissionChanged = 0;
	kNewSpy.m_iTurnSpyImprisoned = 0;
	kNewSpy.m_iTurnActiveMissionConducted = 0;

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
	CvEspionageSpy* pSpy = GetSpyByID(uiSpyIndex);
	bool bLastQualified = false;
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
	switch(pSpy->GetSpyState())
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
		pCityEspionage->Process(ePlayer);
		// if the spy has completed travelling, move to location
		if(pCityEspionage->HasReachedGoal(ePlayer))
		{
			pCityEspionage->ResetProgress(ePlayer);
			if (pSpy->m_bIsDiplomat)
			{
				if (MOD_BALANCE_VP)
				{
					// in VP, Diplomats start schmoozing immediately after arriving in the city
					pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_SCHMOOZE);
					int iRate = CalcPerTurn(SPY_STATE_SCHMOOZE, pCity, uiSpyIndex);
					int iGoal = CalcRequired(SPY_STATE_SCHMOOZE, pCity, uiSpyIndex);
					pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);
				}
				else
				{
					pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_MAKING_INTRODUCTIONS);
					int iRate = CalcPerTurn(SPY_STATE_MAKING_INTRODUCTIONS, pCity, uiSpyIndex);
					int iGoal = CalcRequired(SPY_STATE_MAKING_INTRODUCTIONS, pCity, uiSpyIndex);
					pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);
				}
			}
			else if(pCity->getTeam() == m_pPlayer->getTeam())  // moved to a friendly city
			{
				pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_COUNTER_INTEL);
				if (MOD_BALANCE_VP)
				{
					// set focus
					if (TriggerSpyFocusSetup(pCity, uiSpyIndex))
					{
						pSpy->SetTurnCounterspyMissionChanged(GC.getGame().getGameTurn());
					}
				}
			}
			else // moved to an opponent's city
			{
				CvSpyState eState = SPY_STATE_SURVEILLANCE;
				if (MOD_BALANCE_VP)
				{
					if (GET_PLAYER(eCityOwner).isMinorCiv())
					{
						eState = SPY_STATE_RIG_ELECTION;
					}
					else
					{
						eState = SPY_STATE_GATHERING_INTEL;
						// notify if counterspy present
						if (pCityEspionage->HasCounterSpy())
						{
							CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(pCityEspionage->GetCounterSpyFocus());
							if (!pkEventChoiceInfo->isSecretMission())
							{
								const char* szMissionText = pkEventChoiceInfo->GetHelp();
								CvNotifications* pNotifications = m_pPlayer->GetNotifications();
								if (pNotifications)
								{
									Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_COUNTERSPY_DETECTED");
									strNotification << pCity->getNameKey();
									strNotification << GetSpyRankName(pSpy->m_eRank);
									strNotification << pSpy->GetSpyName(m_pPlayer);
									strNotification << szMissionText;
									CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_COUNTERSPY_DETECTED_S", pCity->getNameKey());
									pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_ALERT, strNotification.toUTF8(), strSummary, pCity->getX(), pCity->getY(), -1);
								}
							}
						}
					}
				}
				pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, eState);
				int iRate = CalcPerTurn(eState, pCity, uiSpyIndex, false);
				int iGoal = CalcRequired(eState, pCity, uiSpyIndex, false);
				pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);

			}
		}
		break;
	case SPY_STATE_IMPRISONED:
		// VP only
		if (GetNumTurnsSpyImprisoned(uiSpyIndex) == 0)
		{
			// release spy
			pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_UNASSIGNED);
			CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			if (pNotifications)
			{
				Localization::String strSummary;
				Localization::String strNotification;

				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RELEASED_PRISON_S");
				strSummary << GetSpyRankName(pSpy->m_eRank);
				strSummary << pSpy->GetSpyName(m_pPlayer);
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_RELEASED_PRISON");
				strNotification << GetSpyRankName(pSpy->m_eRank);
				strNotification << pSpy->GetSpyName(m_pPlayer);
				pNotifications->Add(NOTIFICATION_SPY_REPLACEMENT, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
		}
		break;
	case SPY_STATE_SURVEILLANCE:
		// only used in Community Patch, not in VP
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
				if (!pSpy->m_bPassive) 
				{
					pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_RIG_ELECTION);
					pCityEspionage->ResetProgress(ePlayer);
					int iRate = CalcPerTurn(SPY_STATE_RIG_ELECTION, pCity, uiSpyIndex);
					int iGoal = CalcRequired(SPY_STATE_RIG_ELECTION, pCity, uiSpyIndex);
					pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);
					pCityEspionage->SetLastProgress(ePlayer, iRate);
				}
			}
			else
			{
				if (!pSpy->m_bPassive)
				{
					BuildStealableTechList(eCityOwner);
					// moved rate out here to set the potential
					int iBasePotentialRate = CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, -1);
					pCityEspionage->SetLastBasePotential(ePlayer, iBasePotentialRate);
					if (m_aaPlayerStealableTechList[eCityOwner].size() > 0)
					{
						// TODO: need to proclaim surveillance somehow
						pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_GATHERING_INTEL);
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
						if (!bLastQualified)
						{
							CvNotifications* pNotifications = m_pPlayer->GetNotifications();
							if (pNotifications)
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
						if (GC.getLogging())
						{
							CvString strMsg;
							strMsg.Format("Re-eval: can't steal research, %d,", uiSpyIndex);
							strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
							LogEspionageMsg(strMsg);
						}

					}
				}

				UncoverIntrigue(uiSpyIndex);
				UncoverCityBuildingWonder(uiSpyIndex);
			}
		}
		break;
	case SPY_STATE_GATHERING_INTEL:
		CvAssertMsg(pCity, "pCity not defined. What?");
		if(!pCity)
		{
			return;
		}
		if (MOD_BALANCE_VP)
		{
			int iRate = CalcPerTurn(pSpy->GetSpyState(), pCity, uiSpyIndex);
			pCityEspionage->AddNetworkPoints(m_pPlayer->GetID(), pSpy, iRate);
			if (GD_INT_GET(ESPIONAGE_XP_PER_TURN_OFFENSIVE) != 0)
			{
				LevelUpSpy(uiSpyIndex, GD_INT_GET(ESPIONAGE_XP_PER_TURN_OFFENSIVE));
			}
			UncoverCityBuildingWonder(uiSpyIndex);
		}
		else
		{
			BuildStealableTechList(eCityOwner);
			pCityEspionage->Process(ePlayer);
			// if the rate is too low, reassign the spy
			if (pCityEspionage->m_aiRate[m_pPlayer->GetID()] < 100)
			{
				pSpy->m_bEvaluateReassignment = true;
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Re-eval: potential too low, %d,", uiSpyIndex);
					strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
					LogEspionageMsg(strMsg);
				}
			}
			if (m_aaPlayerStealableTechList[eCityOwner].size() == 0)
			{
				// set the spy back to surveillance mode
				pCityEspionage->ResetProgress(ePlayer);
				pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_SURVEILLANCE);
				pSpy->m_bEvaluateReassignment = true; // flag for reassignment
				if (GC.getLogging())
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
				if (pNotifications)
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
			else if (pCityEspionage->HasReachedGoal(ePlayer))
			{
				int iSpyResult;
				if (pCityEspionage->HasCounterSpy())
				{
					iSpyResult = GC.getGame().randRangeInclusive(1, 300, CvSeeder::fromRaw(0xe9deaa7f).mix(pCity->plot()->GetPseudoRandomSeed()).mix(m_pPlayer->GetID()).mix(GetNumSpyActionsDone(pCity->getOwner()))) - 1;
					int iCounterspyIndex = GET_PLAYER(eCityOwner).GetEspionage()->GetSpyIndexInCity(pCity);
					iSpyResult += GET_PLAYER(eCityOwner).GetEspionage()->m_aSpyList[iCounterspyIndex].m_eRank * 30;
					iSpyResult *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
					iSpyResult /= 100;
					if (iSpyResult < 100)
					{
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_DETECTED);
					}
					else if (iSpyResult < 200)
					{
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_IDENTIFIED);
					}
					else
					{
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_KILLED);

						CvPlayerAI& kCityOwner = GET_PLAYER(eCityOwner);
						CvPlayerAI& kSpyOwner = GET_PLAYER(ePlayer);

						if (strcmp(kCityOwner.getCivilizationTypeKey(), "CIVILIZATION_RUSSIA") == 0 &&
							strcmp(kSpyOwner.getCivilizationTypeKey(), "CIVILIZATION_ENGLAND") == 0)
						{
							//Do not trigger achievement if we're in the Smokey Skies scenario.
							bool bUsingXP1Scenario3 = gDLL->IsModActivated(CIV5_XP1_SCENARIO3_MODID);
							if (!bUsingXP1Scenario3 && eCityOwner == GC.getGame().getActivePlayer())
							{
								gDLL->UnlockAchievement(ACHIEVEMENT_XP1_25);
							}
						}
					}
				}
				else
				{
					iSpyResult = GC.getGame().randRangeInclusive(1, 300, CvSeeder::fromRaw(0x80599453).mix(pCity->plot()->GetPseudoRandomSeed()).mix(m_pPlayer->GetID()).mix(GetNumSpyActionsDone(pCity->getOwner()))) - 1;
					iSpyResult *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
					iSpyResult /= 100;
					if (iSpyResult < 100)
					{
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_UNDETECTED);
					}
					else if (iSpyResult < 200)
					{
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_DETECTED);
					}
					else
					{
						pCityEspionage->SetSpyResult(ePlayer, uiSpyIndex, SPY_RESULT_IDENTIFIED);
					}
				}

				CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(eCityOwner).GetEspionage();
				CvAssertMsg(pDefendingPlayerEspionage, "Defending player espionage is null");
				if (pDefendingPlayerEspionage)
				{
					pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[ePlayer]);
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

						// You broke the promise you made!
						if (GET_PLAYER(eCityOwner).GetDiplomacyAI()->IsPlayerMadeSpyPromise(m_pPlayer->GetID()))
						{
							GET_PLAYER(eCityOwner).GetDiplomacyAI()->SetPlayerSpyPromiseState(m_pPlayer->GetID(), PROMISE_STATE_BROKEN);
						}
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

					GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);

					// kill spy off
					ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
					pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_DEAD); // have to official kill him after the extraction

					if (GC.getLogging())
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
					if (GC.getLogging())
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

					if (pCityEspionage->m_aiResult[ePlayer] == SPY_RESULT_IDENTIFIED)
					{
						CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(eCityOwner).GetEspionageAI();
						CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");
						if (pDefenderEspionageAI)
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
						if (pNotifications)
						{
							Localization::String strSummary(GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_STEAL_TECH_S"));
							Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STEAL_TECH");
							strNotification << GetSpyRankName(pSpy->m_eRank);
							strNotification << pSpy->GetSpyName(m_pPlayer);
							strNotification << pCity->getNameKey();
							strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
							pNotifications->Add(NOTIFICATION_SPY_STOLE_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, eCityOwner);
						}
						m_aiNumSpyActionsDone[eCityOwner]++;
						m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_SPY_ATTACK, false, NO_GREATPERSON, NO_BUILDING, 1);
					}
					else
					{
						m_aiNumTechsToStealList[iCityOwner] = 0;
					}

					//Achievements!
					if (MOD_API_ACHIEVEMENTS && m_pPlayer->GetID() == GC.getGame().getActivePlayer())
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_12);

					LevelUpSpy(uiSpyIndex);

					if (GC.getLogging())
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
				if (pSpy->GetSpyState() != SPY_STATE_DEAD && pSpy->GetSpyState() != SPY_STATE_TERMINATED)
				{
					UncoverIntrigue(uiSpyIndex);
					UncoverCityBuildingWonder(uiSpyIndex);
				}
			}
		}
		break;
	case SPY_STATE_RIG_ELECTION:
		// resetting the value is done in MinorCivAI
		pCityEspionage->Process(ePlayer);
		if (MOD_BALANCE_VP && GD_INT_GET(ESPIONAGE_XP_PER_TURN_CITYSTATE) != 0)
		{
			LevelUpSpy(uiSpyIndex, GD_INT_GET(ESPIONAGE_XP_PER_TURN_CITYSTATE));
		}
		break;
	case SPY_STATE_COUNTER_INTEL:
		if (MOD_BALANCE_VP && GD_INT_GET(ESPIONAGE_XP_PER_TURN_COUNTERSPY) != 0)
		{
			LevelUpSpy(uiSpyIndex, GD_INT_GET(ESPIONAGE_XP_PER_TURN_COUNTERSPY));
		}
		break;
	case SPY_STATE_MAKING_INTRODUCTIONS:
		CvAssertMsg(pCity, "pCity not defined. What?");
		if(!pCity)
		{
			return;
		}
		pCityEspionage->Process(ePlayer);
		if(pCityEspionage->HasReachedGoal(ePlayer))
		{
			pCityEspionage->ResetProgress(ePlayer);
			pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_SCHMOOZE);
		}
		break;
	case SPY_STATE_SCHMOOZE:
		if (MOD_BALANCE_VP && GD_INT_GET(ESPIONAGE_XP_PER_TURN_DIPLOMAT) != 0)
		{
			//int iNetworkPointsAdded = CalcPerTurn(pSpy->GetSpyState(), pCity, uiSpyIndex);
			//pCityEspionage->AddNetworkPoints(m_pPlayer->GetID(), pSpy, iNetworkPointsAdded);
			LevelUpSpy(uiSpyIndex, GD_INT_GET(ESPIONAGE_XP_PER_TURN_DIPLOMAT));
		}
		UncoverIntrigue(uiSpyIndex);
		if (!MOD_BALANCE_VP)
		{
			UncoverCityBuildingWonder(uiSpyIndex);
		}
		break;
	case SPY_STATE_DEAD:
		pSpy->m_iReviveCounter++;
		if(pSpy->m_iReviveCounter >= GD_INT_GET(BALANCE_SPY_RESPAWN_TIMER))
		{
			GetNextSpyName(pSpy);
			pSpy->m_eRank = (CvSpyRank)m_pPlayer->GetStartingSpyRank();
			pSpy->m_iExperience = 0;
			pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_UNASSIGNED);
			pSpy->m_iCityX = -1;
			pSpy->m_iCityY = -1;
			pSpy->m_iReviveCounter = 0;
			pSpy->m_bEvaluateReassignment = true;
			pSpy->m_bPassive = false;
			pSpy->m_iTurnCounterspyMissionChanged = 0;
			pSpy->m_iTurnSpyImprisoned = 0;
			pSpy->m_iTurnActiveMissionConducted = 0;

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
	case SPY_STATE_TERMINATED:
		break; // Do nothing; A terminated spy doesn't come back.
	}

	// if we just established surveillance in the city, turn the lights on
	if(HasEstablishedSurveillance(uiSpyIndex) && !bHadSurveillance)
	{
		pCity->plot()->changeEspionageSight(m_pPlayer->getTeam(), pCity, /*1*/ GD_INT_GET(ESPIONAGE_SURVEILLANCE_SIGHT_RANGE), true);
	}
}

bool CvPlayerEspionage::DoStealTechnology(CvCity* pPlayerCity, PlayerTypes eTargetPlayer)
{
	PlayerTypes eDefendingPlayer = pPlayerCity->getOwner();
	if (m_aaPlayerStealableTechList[eTargetPlayer].size() <= 0)
		return false;

	TeamTypes eTeam = m_pPlayer->getTeam();

	uint uGrab = GC.getGame().urandLimitExclusive(m_aaPlayerStealableTechList[eTargetPlayer].size(), CvSeeder::fromRaw(0x547fc7a8).mix(m_pPlayer->GetID()).mix(GET_PLAYER(eTargetPlayer).GetID()));
	TechTypes eStolenTech = m_aaPlayerStealableTechList[eTargetPlayer][uGrab];

	GET_TEAM(eTeam).setHasTech(eStolenTech, true, m_pPlayer->GetID(), true, true);
	GET_TEAM(eTeam).GetTeamTechs()->SetNoTradeTech(eStolenTech, true);
	GET_PLAYER(eDefendingPlayer).GetEspionage()->UpdateSpyMessage(pPlayerCity->getX(), pPlayerCity->getY(), m_pPlayer->GetID(), eStolenTech);
	
	// recalculate the num techs to steal list
	BuildStealableTechList((PlayerTypes)eTargetPlayer);

	return true;
}
bool CvPlayerEspionage::DoStealGW(CvCity* pPlayerCity, int iGWID)
{

	PlayerTypes eDefendingPlayer = pPlayerCity->getOwner();

	BuildingClassTypes eGWBuildingClass; //passed by ref
	int iSlot = -1; // Passed by reference below
	GreatWorkType eType = GC.getGame().GetGameCulture()->m_CurrentGreatWorks[iGWID].m_eType;
	GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(eType);
	int iCityID = 0;
	BuildingTypes eTargetBuilding = NO_BUILDING; // Passed by reference below
	int iTargetSlot = -1; // Passed by reference below
	GET_PLAYER(eDefendingPlayer).GetCulture()->GetGreatWorkLocation(iGWID, iCityID, eTargetBuilding, iTargetSlot);
	if (pPlayerCity->GetID() != iCityID)
	{
		// something went wrong...
		UNREACHABLE();
	}
	BuildingClassTypes eTargetBuildingClass = GC.getBuildingInfo(eTargetBuilding)->GetBuildingClassType();

	CvCity *pArtCity = m_pPlayer->GetCulture()->GetClosestAvailableGreatWorkSlot(pPlayerCity->getX(), pPlayerCity->getY(), eGreatWorkSlot, &eGWBuildingClass, &iSlot);
	if (pArtCity)
	{
		// remove the work from the targeted city ...
		pPlayerCity->GetCityBuildings()->SetBuildingGreatWork(eTargetBuildingClass, iTargetSlot, NO_GREAT_WORK);
		pPlayerCity->UpdateCityYields(YIELD_TOURISM);
		pPlayerCity->UpdateCityYields(YIELD_CULTURE);
		// ... and place it in the city of the spy owner
		pArtCity->GetCityBuildings()->SetBuildingGreatWork(eGWBuildingClass, iSlot, iGWID);
		pArtCity->UpdateCityYields(YIELD_TOURISM);
		pArtCity->UpdateCityYields(YIELD_CULTURE);
		// update spy notification with id of great work
		GET_PLAYER(eDefendingPlayer).GetEspionage()->UpdateSpyMessage(pPlayerCity->getX(), pPlayerCity->getY(), m_pPlayer->GetID(), NO_TECH, -1, iGWID);
		CvGameCulture *pCulture = GC.getGame().GetGameCulture();
		if (pCulture)
		{
			CvString strMsg;
			strMsg.Format("Great Work stolen by Spy! Name: %s. Stolen from: %s ", pCulture->GetGreatWorkName(iGWID).GetCString(), pPlayerCity->getNameKey());
			strMsg += ",";
			strMsg += ",";
			strMsg += GET_PLAYER(eDefendingPlayer).getCivilizationShortDescription();
			strMsg += ",";
		}

		if (m_pPlayer->GetID() == GC.getGame().getActivePlayer())
		{
			CvPopupInfo kPopup(BUTTONPOPUP_GREAT_WORK_COMPLETED_ACTIVE_PLAYER, iGWID);
			GC.GetEngineUserInterface()->AddPopup(kPopup);
		}
		return true;
	}
	return false;
}

// TriggerSpyFocusSetup - VP only. For humans: Displays the event to choose a new counterspy focus. For AI: Makes a choice for the counterspy focus.
bool CvPlayerEspionage::TriggerSpyFocusSetup(CvCity* pCity, int uiSpyIndex)
{
	CityEventTypes eSetupEvent = GetCounterspyEvent();

	if (eSetupEvent == NO_EVENT_CITY)
		return false;

	pCity->SetEventActive(eSetupEvent, true);

	if (!m_pPlayer->isHuman())
	{
		CvEspionageAI* pEspionageAI = m_pPlayer->GetEspionageAI();
		std::vector<int> aCounterspyMissionList;
		for (int i = 0; i < GC.getNumCityEventChoiceInfos(); i++)
		{
			CvModEventCityChoiceInfo* pkMissionInfo = GC.getCityEventChoiceInfo((CityEventChoiceTypes)i);
			if (pkMissionInfo->isCounterspyMission())
			{
				aCounterspyMissionList.push_back(i);
			}
		}
		int iScore = 0;
		CityEventChoiceTypes eBestMission = pEspionageAI->GetBestMissionInCity(pCity, iScore, aCounterspyMissionList, uiSpyIndex);
		if (eBestMission != NO_EVENT_CHOICE_CITY)
		{
			if (GC.getLogging() && GC.getAILogging())
			{
				CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eBestMission);
				if (pkEventChoiceInfo != NULL)
				{
					CvString playerName;
					FILogFile* pLog = NULL;
					CvString strBaseString;
					CvString strOutBuf;
					CvString strFileName = "EspionageLog.csv";
					playerName = m_pPlayer->getCivilizationShortDescription();
					pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
					strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
					strBaseString += playerName + ", ";
					strOutBuf.Format("AI selected counterspy focus: %s", pkEventChoiceInfo->GetDescription());
					strBaseString += strOutBuf;
					pLog->Msg(strBaseString);
				}
			}
			pCity->DoEventChoice(eBestMission, NO_EVENT_CITY, false, uiSpyIndex, m_pPlayer->GetID());
		}
		else
		{
			ExtractSpyFromCity(uiSpyIndex);
			return false;
		}
	}
	else
	{
		CvEspionageSpy* pSpy = GetSpyByID(uiSpyIndex);
		if (pSpy)
		{
			CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			if (pNotifications)
			{
				Localization::String strBuffer = Localization::Lookup("TXT_KEY_CHOOSE_EVENT_AA_CHOICE");
				strBuffer << GetSpyRankName(pSpy->m_eRank);
				strBuffer << pSpy->GetSpyName(m_pPlayer);

				Localization::String strSummary = Localization::Lookup("TXT_KEY_CHOOSE_EVENT_AA_TT");
				strSummary << GetSpyRankName(pSpy->m_eRank);
				strSummary << pSpy->GetSpyName(m_pPlayer);
				strSummary << pCity->getNameKey();

				pNotifications->Add((NotificationTypes)FString::Hash("NOTIFICATION_ESPIONAGE_AA"), strSummary.toUTF8(), strBuffer.toUTF8(), pCity->plot()->getX(), pCity->plot()->getY(), eSetupEvent, uiSpyIndex);
			}
		}
		pCity->GetCityEspionage()->ChangePendingEvents(m_pPlayer->GetID(), 1);
	}

	return true;
}
void CvPlayerEspionage::ProcessSpyMissionResult(PlayerTypes eSpyOwner, CvCity* pCity, int uiSpyIndex, CityEventChoiceTypes eMission)
{
	// used in VP only. In CP, Spy Results are processed in CvPlayerEspionage::ProcessSpy
	if (eMission == NO_EVENT_CHOICE_CITY)
		return;

	CvModEventCityChoiceInfo* pkMissionInfo = GC.getCityEventChoiceInfo(eMission);
	if (pkMissionInfo == NULL)
		return;

	CvEspionageSpy* pSpy = GetSpyByID(uiSpyIndex);

	PlayerTypes eCityOwner = pCity->getOwner();
	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();

	if (eCityOwner == eSpyOwner)
		return;

	CvPlayerEspionage* pDefendingPlayerEspionage = GET_PLAYER(eCityOwner).GetEspionage();
	CvDiplomacyAI* pDefendingPlayerDiploAI = GET_PLAYER(eCityOwner).GetDiplomacyAI();
	CvEspionageAI* pEspionageAI = m_pPlayer->GetEspionageAI();
	CvAssertMsg(pEspionageAI, "pEspionageAI is null");
	CvEspionageAI* pDefenderEspionageAI = GET_PLAYER(eCityOwner).GetEspionageAI();
	CvAssertMsg(pDefenderEspionageAI, "pDefenderEspionageAI is null");



	// Spy result is calculated based on the ID and capture chance of the selected mission
	int iIdentifyRoll = GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0xe9deaa7f).mix(pCity->plot()->GetPseudoRandomSeed()).mix(m_pPlayer->GetID()).mix((int)eMission).mix(GetNumSpyActionsDone(pCity->getOwner())));
	bool bIdentified = (iIdentifyRoll <= pkMissionInfo->GetSpyIdentificationChance());
	int iCaptureRoll = GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x80599453).mix(pCity->plot()->GetPseudoRandomSeed()).mix(m_pPlayer->GetID()).mix((int)eMission).mix(GetNumSpyActionsDone(pCity->getOwner())));
	bool bCaught = (iCaptureRoll <= pkMissionInfo->GetSpyCaptureChance());

	// counterspy?
	bool bKilled = false;
	CityEventChoiceTypes eCounterSpyFocus = pCityEspionage->GetCounterSpyFocus();
	if (eCounterSpyFocus != NO_EVENT_CHOICE_CITY)
	{
		CvModEventCityChoiceInfo* pkCounterspyFocusInfo = GC.getCityEventChoiceInfo(eCounterSpyFocus);
		if (pkCounterspyFocusInfo->isAlwaysIDSpies())
		{
			bIdentified = true;
		}
		if(bCaught && pkCounterspyFocusInfo->isKillCaughtSpies())
		{ 
			bKilled = true;
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				YieldTypes eYield = (YieldTypes)iI;
				if (eYield == NO_YIELD)
					continue;

				int iYield = pkCounterspyFocusInfo->getYieldOnSpyCaught(eYield);
				if (iYield > 0)
				{
					GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, iYield * (pCityEspionage->GetCounterSpyRank() + 1), true, NO_PLAYER, NULL, false, pCity, false, true, false, eYield);
				}
			}
		}
	}

	CvSpyResult eResult = NUM_SPY_RESULTS;
	if (bKilled)
	{
		eResult = SPY_RESULT_KILLED;
		GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
		GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_IDENTIFY, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
		GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE_OR_ID, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
	}
	else
	{
		if (bIdentified && bCaught)
		{
			eResult = SPY_RESULT_CAUGHT_AND_IDENTIFIED;
			GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
			GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_IDENTIFY, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
			GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE_OR_ID, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
		}
		else if (bIdentified)
		{
			eResult = SPY_RESULT_IDENTIFIED;
			GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_IDENTIFY, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
			GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE_OR_ID, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
		}
		else if (bCaught)
		{
			eResult = SPY_RESULT_CAUGHT;
			GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
			GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_DEFENSE_OR_ID, false, NO_GREATPERSON, NO_BUILDING, 0, true, NO_PLAYER, NULL, false, pCity);
		}
		else
		{
			eResult = SPY_RESULT_DETECTED;
		}
	}
	pCityEspionage->SetSpyResult(eSpyOwner, uiSpyIndex, eResult);

	m_aiNumSpyActionsDone[eCityOwner]++;
	pCityEspionage->m_aiNumTimesCityRobbed[eSpyOwner]++;
	m_pPlayer->doInstantYield(INSTANT_YIELD_TYPE_SPY_ATTACK, false, NO_GREATPERSON, NO_BUILDING);

	pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), eSpyOwner, eResult, eMission);

	if (bIdentified)
	{
		// Diplomatic Penalties
		if (pDefendingPlayerEspionage)
		{
			pDefendingPlayerDiploAI->ChangeNumTimesRobbedBy(eSpyOwner, 1);
			pDefenderEspionageAI->m_aiTurnLastSpyCaught[eSpyOwner] = GC.getGame().getGameTurn();
			pDefenderEspionageAI->m_aiNumSpiesCaught[eSpyOwner]++;
		}
		// You broke the promise you made!
		if (GET_PLAYER(eCityOwner).GetDiplomacyAI()->IsPlayerMadeSpyPromise(eSpyOwner))
		{
			GET_PLAYER(eCityOwner).GetDiplomacyAI()->SetPlayerSpyPromiseState(eSpyOwner, PROMISE_STATE_BROKEN);
		}

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			YieldTypes eYield = (YieldTypes)iI;
			int iSpyIDYield = GET_PLAYER(eCityOwner).getYieldForSpyID(eYield);
			if (iSpyIDYield > 0)
				GET_PLAYER(eCityOwner).doInstantYield(INSTANT_YIELD_TYPE_SPY_IDENTIFY, false, NO_GREATPERSON, NO_BUILDING, iSpyIDYield, true, NO_PLAYER, NULL, false, pCity, false, true, true, eYield);
		}
	}

	// spy killed in action
	if (bKilled)
	{
		if (pEspionageAI)
		{
			pEspionageAI->m_aiTurnLastSpyDied[eCityOwner] = GC.getGame().getGameTurn();
			pEspionageAI->m_aiNumSpiesDied[eCityOwner]++;
		}
		if (pDefenderEspionageAI)
		{
			pDefenderEspionageAI->m_aiTurnLastSpyKilled[eSpyOwner] = GC.getGame().getGameTurn();
			pDefenderEspionageAI->m_aiNumSpiesKilled[eSpyOwner]++;

		}

		// kill spy off
		ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
		pSpy->m_eSpyState = SPY_STATE_DEAD; // have to official kill him after the extraction
		if (GC.getLogging())
		{
			CvString strMsg;
			strMsg.Format("Spy killed");
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
		int iExperience = pkMissionInfo->GetNetworkPointsNeededScaled() / /*50*/ GD_INT_GET(ESPIONAGE_XP_MISSION_SUCCESS_DENOMINATOR);
		LevelUpSpy(uiSpyIndex, iExperience);
		pSpy->SetTurnActiveMissionConducted(GC.getGame().getGameTurn());

		if (bCaught)
		{
			pSpy->SetTurnSpyImprisoned(GC.getGame().getGameTurn());
			ExtractSpyFromCity(uiSpyIndex);
			pSpy->m_eSpyState = SPY_STATE_IMPRISONED;
			if (GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Spy imprisoned");
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
		
		// reset spy focus
		pSpy->SetSpyFocus(NO_EVENT_CHOICE_CITY);

		//AI should reeval to see if they want to stay.
		if (!m_pPlayer->isHuman())
		{
			pSpy->m_bEvaluateReassignment = true;
		}
	}

	CvNotifications* pNotifications = GET_PLAYER(eSpyOwner).GetNotifications();
	if (pNotifications)
	{
		NotificationTypes eNotification;
		Localization::String strSummary;
		Localization::String strNotification;
		if (bKilled)
		{
			eNotification = NOTIFICATION_SPY_YOU_STAGE_COUP_FAILURE;
			strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED_S");
			strSummary << GetSpyRankName(pSpy->m_eRank);
			strSummary << pSpy->GetSpyName(m_pPlayer);
			strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED");
			strNotification << GetSpyRankName(pSpy->m_eRank);
			strNotification << pSpy->GetSpyName(m_pPlayer);
			strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getAdjectiveKey();
		}
		else
		{
			eNotification = NOTIFICATION_SPY_YOU_STAGE_COUP_SUCCESS;
			if (bIdentified && bCaught)
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_IDENTIFIED_AND_CAUGHT_S");
				strSummary << GetSpyRankName(pSpy->m_eRank);
				strSummary << pSpy->GetSpyName(m_pPlayer);
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_IDENTIFIED_AND_CAUGHT");
				strNotification << GetSpyRankName(pSpy->m_eRank);
				strNotification << pSpy->GetSpyName(m_pPlayer);
				strNotification << GET_PLAYER(eCityOwner).getNameKey();
			}
			else if (bIdentified)
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_IDENTIFIED_S");
				strSummary << GetSpyRankName(pSpy->m_eRank);
				strSummary << pSpy->GetSpyName(m_pPlayer);
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_IDENTIFIED");
				strNotification << GetSpyRankName(pSpy->m_eRank);
				strNotification << pSpy->GetSpyName(m_pPlayer);
				strNotification << GET_PLAYER(eCityOwner).getNameKey();
			}
			else if (bCaught)
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_CAUGHT_S");
				strSummary << GetSpyRankName(pSpy->m_eRank);
				strSummary << pSpy->GetSpyName(m_pPlayer);
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_CAUGHT");
				strNotification << GetSpyRankName(pSpy->m_eRank);
				strNotification << pSpy->GetSpyName(m_pPlayer);
				strNotification << GET_PLAYER(eCityOwner).getNameKey();
			}
			else
			{
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_NOT_DETECTED_S");
				strSummary << GetSpyRankName(pSpy->m_eRank);
				strSummary << pSpy->GetSpyName(m_pPlayer);
				strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_NOT_DETECTED");
				strNotification << GetSpyRankName(pSpy->m_eRank);
				strNotification << pSpy->GetSpyName(m_pPlayer);
				strNotification << GET_PLAYER(eCityOwner).getNameKey();
			}
		}
		pNotifications->Add(eNotification, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
	}
}

// Helpers

CvString CvPlayerEspionage::GetEventHelpText(CityEventTypes eEvent, int uiSpyIndex)
{
	CvEspionageSpy* pSpy = GetSpyByID(uiSpyIndex);
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

CvString CvPlayerEspionage::GetSpyMissionTooltip(CvCity* pCity, uint uiSpyIndex)
{
	if (uiSpyIndex == -1)
		return "";

	CvString strSpyAtCity = "";

	CvEspionageSpy* pSpy = GetSpyByID(uiSpyIndex);
	if (!pSpy)
		return "";

	if (MOD_BALANCE_VP && pSpy->GetSpyState() != SPY_STATE_TERMINATED && pSpy->GetSpyState() != SPY_STATE_DEAD && pSpy->m_eRank < (NUM_SPY_RANKS - 1))
	{
		int iExperienceDenominator = /*100*/ GD_INT_GET(ESPIONAGE_SPY_EXPERIENCE_DENOMINATOR);
		iExperienceDenominator *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iExperienceDenominator /= 100;
		strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_EXPERIENCE", pSpy->m_iExperience, iExperienceDenominator);
		strSpyAtCity += "[NEWLINE][NEWLINE]";
	}

	CvSpyState eSpyState = pSpy->GetSpyState();
	if (pCity == NULL)
	{
		if (eSpyState == SPY_STATE_UNASSIGNED)
		{
			if (!strSpyAtCity.empty())
				strSpyAtCity += "[NEWLINE][NEWLINE]";

			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_UNASSIGNED_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
		}
		else if (eSpyState == SPY_STATE_IMPRISONED)
		{
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_IMPRISONED_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
		}
		else if (eSpyState == SPY_STATE_TERMINATED || pSpy->GetSpyState() == SPY_STATE_DEAD)
		{
			if (!strSpyAtCity.empty())
				strSpyAtCity += "[NEWLINE][NEWLINE]";

			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_BUTTON_DISABLED_SPY_DEAD_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
		}
	}
	else
	{
		if (eSpyState == SPY_STATE_TRAVELLING)
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_TRAVELLING_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
		else if (eSpyState == SPY_STATE_SURVEILLANCE)
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_SURVEILLANCE_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
		else if (eSpyState == SPY_STATE_GATHERING_INTEL)
		{
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_GATHERING_INTEL_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
			if (MOD_BALANCE_VP)
			{
				CvString strNetworkPoints = "";
				CalcNetworkPointsPerTurn(eSpyState, pCity, uiSpyIndex, &strNetworkPoints);
				strSpyAtCity += "[NEWLINE]";
				strSpyAtCity += strNetworkPoints;
			}
		}
		else if (eSpyState == SPY_STATE_RIG_ELECTION)
		{
			int iRiggingInfluence = /*20 in CP, 30 in VP*/ GD_INT_GET(ESPIONAGE_INFLUENCE_GAINED_FOR_RIGGED_ELECTION);
			iRiggingInfluence *= 100 + m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIG_ELECTION_INFLUENCE_MODIFIER);
			iRiggingInfluence /= 100;
			if (MOD_BALANCE_VP)
			{
				int iEra = m_pPlayer->GetCurrentEra();
				if (iEra <= 0)
				{
					iEra = 1;
				}
				iRiggingInfluence *= iEra;
				iRiggingInfluence *= (1 + GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->GetNumConsecutiveSuccessfulRiggings(m_pPlayer->GetID()));
			}
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_RIGGING_ELECTIONS_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey(), iRiggingInfluence);
		}
		else if (eSpyState == SPY_STATE_COUNTER_INTEL)
		{
			if (MOD_BALANCE_VP)
			{
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_COUNTER_INTEL_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
				CityEventChoiceTypes eActiveMission = pSpy->GetSpyFocus();
				if (eActiveMission != NO_EVENT_CHOICE_CITY)
				{
					CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eActiveMission);
					if (pkEventChoiceInfo)
					{
						strSpyAtCity += "[NEWLINE][NEWLINE]";
						CvString strActiveMission = Localization::Lookup(pkEventChoiceInfo->GetHelp()).toUTF8();
						strSpyAtCity += GetLocalizedText("TXT_KEY_EO_COUNTERSPY_ACTIVE_FOCUS_TT", strActiveMission);
					}
				}
			}
			else
			{
				int iCatchSpiesChance = 33; // base chance
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_COUNTER_INTEL_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
				strSpyAtCity += "[NEWLINE]";
				int iTempValue = pSpy->m_eRank * 10;
				if (iTempValue > 0)
				{
					strSpyAtCity += "[NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_COUNTER_INTEL_SPY_RANK_TT", iTempValue, GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
				}
				iCatchSpiesChance *= (100 + iTempValue);
				iCatchSpiesChance /= 100;
				iTempValue = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER);
				if (iTempValue > 0)
				{
					strSpyAtCity += "[NEWLINE]";
					strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_COUNTER_INTEL_POLICY_TT", iTempValue, GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
				}
				iCatchSpiesChance *= (100 + iTempValue);
				iCatchSpiesChance /= 100;
				strSpyAtCity += "[NEWLINE]";
				strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_COUNTER_INTEL_SUM_TT", iCatchSpiesChance);
			}
		}
		else if (eSpyState == SPY_STATE_TERMINATED || eSpyState == SPY_STATE_DEAD)
			strSpyAtCity += GetLocalizedText("TXT_KEY_EO_SPY_BUTTON_DISABLED_SPY_DEAD_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
		else if (eSpyState == SPY_STATE_MAKING_INTRODUCTIONS)
			strSpyAtCity += GetLocalizedText("TXT_KEY_SPY_STATE_MAKING_INTRODUCTIONS_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
		else if (eSpyState == SPY_STATE_SCHMOOZE)
			strSpyAtCity += GetLocalizedText("TXT_KEY_SPY_STATE_SCHMOOZING_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer), pCity->getNameKey());
		else if (eSpyState == SPY_STATE_UNASSIGNED)
			strSpyAtCity += GetLocalizedText("TXT_KEY_SPY_STATE_UNASSIGNED_TT", GetSpyRankName(pSpy->m_eRank), pSpy->GetSpyName(m_pPlayer));
	}

	return strSpyAtCity;
}

/// UncoverIntrigue - Determine if the spy uncovers any secret information and pass it along to the player
void CvPlayerEspionage::UncoverIntrigue(uint uiSpyIndex)
{
	CvEspionageSpy* pSpy = GetSpyByID(uiSpyIndex);
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
		uint uiTargetSlot = GC.getGame().urandLimitExclusive(aiMajorCivIndex.size(), pCity->plot()->GetPseudoRandomSeed().mix(ui));
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

		// Ignore vassals
		if (GET_TEAM(GET_PLAYER(eCityOwner).getTeam()).IsVassalOfSomeone())
			continue;

		// Ignore vassals
		if (GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).IsVassalOfSomeone())
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
		iSpyRank = m_aSpyList[uiSpyIndex].GetSpyRank(ePlayer) + m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eCityOwner);

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


		if(MOD_BALANCE_VP && (iSpyRank <= SPY_RANK_AGENT))
		{
			LevelUpSpy(uiSpyIndex, /*15*/ GD_INT_GET(ESPIONAGE_XP_UNCOVER_INTRIGUE));
		}

		// If a sneak attack is reported, bust out of the loop
		break;
	}

	// building up an army
	if(!GET_PLAYER(eCityOwner).isHuman())
	{
		if (GET_PLAYER(eCityOwner).GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_LAND))
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_ARMY, uiSpyIndex, pCity, true);

			if (MOD_BALANCE_CORE_SPIES_ADVANCED)
			{
				LevelUpSpy(uiSpyIndex, /*15*/ GD_INT_GET(ESPIONAGE_XP_UNCOVER_INTRIGUE));
			}
		}
		else if (GET_PLAYER(eCityOwner).GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_NAVAL) || (GET_PLAYER(eCityOwner).GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_COMBINED)))
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY, uiSpyIndex, pCity, true);

			if(MOD_BALANCE_CORE_SPIES_ADVANCED)
			{
				LevelUpSpy(uiSpyIndex, /*15*/ GD_INT_GET(ESPIONAGE_XP_UNCOVER_INTRIGUE));
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

		// Ignore vassals
		if (GET_TEAM(GET_PLAYER(eOtherOtherPlayer).getTeam()).IsVassal(GET_PLAYER(eCityOwner).getTeam()))
			continue;

		// Ignore vassals
		if (GET_TEAM(GET_PLAYER(eCityOwner).getTeam()).IsVassal(GET_PLAYER(eOtherOtherPlayer).getTeam()))
			continue;
		
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

					if(MOD_BALANCE_CORE_SPIES_ADVANCED)
					{
						LevelUpSpy(uiSpyIndex, /*15*/ GD_INT_GET(ESPIONAGE_XP_UNCOVER_INTRIGUE));
					}
				}
				else
				{
					AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_DECEPTION, uiSpyIndex, pCity, true);

					if(MOD_BALANCE_CORE_SPIES_ADVANCED)
					{
						LevelUpSpy(uiSpyIndex, /*15*/ GD_INT_GET(ESPIONAGE_XP_UNCOVER_INTRIGUE));
					}
				}
				break; // we reported intrigue, now bail out
			}
		}
	}
}

/// UncoverIntrigue - Determine if the spy uncovers any secret information and pass it along to the player
void CvPlayerEspionage::UncoverCityBuildingWonder(uint uiSpyIndex)
{
	CvEspionageSpy* pSpy = GetSpyByID(uiSpyIndex);
	CvPlot* pCityPlot = GC.getMap().plot(pSpy->m_iCityX, pSpy->m_iCityY);
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;
	PlayerTypes ePlayer = m_pPlayer->GetID();
	PlayerTypes eCityOwner = NO_PLAYER;
	if (pCityPlot)
	{
		pCity = pCityPlot->getPlotCity();
		eCityOwner = pCity->getOwner();
		pCityEspionage = pCity->GetCityEspionage();
	}

	CvAssertMsg(pCity, "Spy needs to be in city to uncover intrigue");
	if (!pCity)
	{
		return;
	}

	CvAssertMsg(ePlayer != eCityOwner, "Uncover intrigue should not occur when a spy is in their own city");
	if (ePlayer == eCityOwner)
	{
		return;
	}

	if (!MOD_BALANCE_VP || pCity->GetCityEspionage()->GetRevealCityScreen(m_pPlayer->GetID()))
	{
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
		}
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
		uint uiTargetSlot = GC.getGame().urandLimitExclusive(aiMajorCivIndex.size(), pCity->plot()->GetPseudoRandomSeed().mix(ui));
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
		CvEspionageSpy* pSpy = pkPlayerEspionage->GetSpyByID(uiSpy);

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
		int iOffset = GC.getGame().randRangeExclusive(0, iCivSpyNames, CvSeeder(iCivSpyNames));

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
	int iCivOffset = GC.getGame().randRangeExclusive(0, iMaxCivs, m_pPlayer->GetPseudoRandomSeed());
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
	int iPlayerOffset = GC.getGame().randRangeExclusive(0, MAX_MAJOR_CIVS, m_pPlayer->GetPseudoRandomSeed());

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

	return m_aSpyList[uiSpyIndex].m_iCityX != -1 && m_aSpyList[uiSpyIndex].m_iCityY != -1;
}

CvCity* CvPlayerEspionage::GetCityWithSpy(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "uiSpyIndex is out of bounds");
	if(uiSpyIndex >= m_aSpyList.size())
	{
		return NULL;
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

CvEspionageSpy* CvPlayerEspionage::GetSpyByID(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "uiSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return NULL;
	}

	return &m_aSpyList[uiSpyIndex];

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


/// CanEverMoveSpyTo - May a spy move into this city at any point (used for building lists)
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
	if (uiSpyIndex >= 0)
	{
		CvCity* pCurrentCity = GetCityWithSpy(uiSpyIndex);
		if (pCurrentCity != NULL)
		{
			if (pCurrentCity->GetCityEspionage()->GetPendingEvents(m_pPlayer->GetID()) > 0)
				return false;
		}
	}
	// This allows the player to move the spy off the board
	if(!pCity)
	{
		return true;
	}

	if(!CanEverMoveSpyTo(pCity))
	{
		return false;
	}

	if (GetNumTurnsSpyMovementBlocked(uiSpyIndex) > 0)
	{
		return false;
	}

	if (GetNumTurnsSpyImprisoned(uiSpyIndex) > 0)
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
		if (!GET_PLAYER(pCity->getOwner()).isMajorCiv())
			return false;

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

		//time to reset the spy

		m_aSpyList[uiSpyIndex].m_iCityX = pCity->getX();
		m_aSpyList[uiSpyIndex].m_iCityY = pCity->getY();
		pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] = uiSpyIndex;
		m_aSpyList[uiSpyIndex].SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_TRAVELLING);
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

///  GetNumTurnsSpyMovementBlocked - Get number of remaining turns for which the spy can't move or change missions
int CvPlayerEspionage::GetNumTurnsSpyMovementBlocked(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return -1;
	}
	CvEspionageSpy* pSpy = &m_aSpyList[uiSpyIndex];
	if (pSpy->GetTurnCounterspyMissionChanged() == 0)
		return 0;

	return max(0, GD_INT_GET(ESPIONAGE_COUNTERSPY_CHANGE_FOCUS_COOLDOWN) + pSpy->GetTurnCounterspyMissionChanged() - GC.getGame().getGameTurn());
}

///  GetNumTurnsSpyActiveMissionsBlocked - Get number of remaining turns for which the spy can't perform active missions
int CvPlayerEspionage::GetNumTurnsSpyActiveMissionsBlocked(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return -1;
	}
	CvEspionageSpy* pSpy = &m_aSpyList[uiSpyIndex];
	if (pSpy->GetTurnActiveMissionConducted() == 0)
		return 0;

	return max(0, GC.getGame().GetTurnsBetweenMinorCivElections() + pSpy->GetTurnActiveMissionConducted() - GC.getGame().getGameTurn());
}

///  GetNumTurnsSpyMovementBlocked - Get number of remaining turns for which the spy can't move or change missions
int CvPlayerEspionage::GetNumTurnsSpyImprisoned(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return -1;
	}
	CvEspionageSpy* pSpy = &m_aSpyList[uiSpyIndex];
	if(pSpy->GetSpyState() != SPY_STATE_IMPRISONED)
		return 0;

	return max(0, GD_INT_GET(ESPIONAGE_IMPRISONMENT_COOLDOWN) + pSpy->GetTurnSpyImprisoned() - GC.getGame().getGameTurn());
}

void CvPlayerEspionage::ChangeCounterspyMission(uint uiSpyIndex, CityEventChoiceTypes eNewMission)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return;
	}

	CvEspionageSpy* pSpy = &m_aSpyList[uiSpyIndex];
	PlayerTypes eSpyOwner = m_pPlayer->GetID();

	int iCityX = pSpy->m_iCityX;
	int iCityY = pSpy->m_iCityY;

	CvPlot* pPlot = GC.getMap().plot(iCityX, iCityY);
	CvAssertMsg(pPlot, "Spy coordinates did not point to plot");

	CvCity* pCity = pPlot->getPlotCity();
	CvAssertMsg(pCity, "No city in plot pointed to by spy");

	if (pCity != NULL && pCity->getOwner() == eSpyOwner)
	{
		CityEventChoiceTypes ePreviousFocus = pSpy->GetSpyFocus();
		if (ePreviousFocus != eNewMission)
		{
			// try to cancel current mission
			if (ePreviousFocus != NO_EVENT_CHOICE_CITY)
			{
				// can't change missions right now
				if (GetNumTurnsSpyMovementBlocked(uiSpyIndex) > 0)
				{
					return;
				}
				pCity->SetEventChoiceActive(ePreviousFocus, true);
				pCity->DoCancelEventChoice(ePreviousFocus);

				// notify other players
				CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(ePreviousFocus);
				if (!pkEventChoiceInfo->isSecretMission())
				{
					const char* szMissionText = pkEventChoiceInfo->GetHelp();
					for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
					{
						const PlayerTypes ePlayer = static_cast<PlayerTypes>(i);
						CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
						if (pNotifications)
						{
							// does the player have a spy in this city?
							CvPlayerEspionage* pPlayerEspionage = GET_PLAYER(ePlayer).GetEspionage();
							if (pPlayerEspionage && pPlayerEspionage->GetSpyIndexInCity(pCity) != -1)
							{
								CvEspionageSpy* pSpy = pPlayerEspionage->GetSpyByID(pPlayerEspionage->GetSpyIndexInCity(pCity));
								if (pSpy->GetSpyState() == SPY_STATE_GATHERING_INTEL)
								{
									CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_COUNTERSPY_ENDED", pCity->getNameKey(), szMissionText);
									CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_COUNTERSPY_ENDED_S", pCity->getNameKey());
									pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_ALERT, strBuffer, strSummary, pCity->getX(), pCity->getY(), -1);
								}
							}
						}
					}
				}
			}
			if (eNewMission != NO_EVENT_CHOICE_CITY)
			{
				pCity->SetEventChoiceActive(eNewMission, true);
				pCity->DoEventChoice(eNewMission, NO_EVENT_CITY, true, uiSpyIndex, eSpyOwner);
				pSpy->SetTurnCounterspyMissionChanged(GC.getGame().getGameTurn());

				// notify other players
				CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eNewMission);
				if (!pkEventChoiceInfo->isSecretMission())
				{
					const char* szMissionText = pkEventChoiceInfo->GetHelp();
					for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
					{
						const PlayerTypes ePlayer = static_cast<PlayerTypes>(i);
						CvNotifications* pNotifications = GET_PLAYER(ePlayer).GetNotifications();
						if (pNotifications)
						{
							// does the player have a spy in this city?
							CvPlayerEspionage* pPlayerEspionage = GET_PLAYER(ePlayer).GetEspionage();
							if (pPlayerEspionage && pPlayerEspionage->GetSpyIndexInCity(pCity) != -1)
							{
								CvEspionageSpy* pSpy = pPlayerEspionage->GetSpyByID(pPlayerEspionage->GetSpyIndexInCity(pCity));
								if (pSpy->GetSpyState() == SPY_STATE_GATHERING_INTEL)
								{
									CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_COUNTERSPY_STARTED", pCity->getNameKey(), szMissionText);
									CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_COUNTERSPY_STARTED_S", pCity->getNameKey());
									pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_ALERT, strBuffer, strSummary, pCity->getX(), pCity->getY(), -1);
								}
							}
						}
					}
				}
			}
			pSpy->SetSpyFocus(eNewMission);
			pCity->GetCityEspionage()->SetPendingEvents(eSpyOwner, 0);
		}
	}
	else
	{
		UNREACHABLE();
	}

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

	CvEspionageSpy* pSpy = &m_aSpyList[uiSpyIndex];
	bool bHadSurveillance = HasEstablishedSurveillance(uiSpyIndex);

	int iCityX = pSpy->m_iCityX;
	int iCityY = pSpy->m_iCityY;

	CvPlot* pPlot = GC.getMap().plot(iCityX, iCityY);
	CvAssertMsg(pPlot, "Spy coordinates did not point to plot");
	if (!pPlot)
	{
		return false;
	}

	CvCity* pCity = pPlot->getPlotCity();
	CvAssertMsg(pCity, "No city in plot pointed to by spy");
	if (!pCity)
	{
		return false;
	}

	//counterspy?
	if (pCity != NULL && pCity->getOwner() == m_pPlayer->GetID() && MOD_BALANCE_VP)
	{
		ChangeCounterspyMission(uiSpyIndex, NO_EVENT_CHOICE_CITY);
	}
	// Spy was rigging an election? Reset streak count
	if (pCity != NULL && GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		GET_PLAYER(pCity->getOwner()).GetMinorCivAI()->ResetNumConsecutiveSuccessfulRiggings(m_pPlayer->GetID());
	}

	pSpy->m_iCityX = -1;
	pSpy->m_iCityY = -1;
	pSpy->SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_UNASSIGNED);

	PlayerTypes ePlayer = m_pPlayer->GetID();
	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	pCityEspionage->m_aiSpyAssignment[ePlayer] = -1;
	pCityEspionage->ResetProgress(ePlayer);
	pCityEspionage->ResetPassiveBonuses(ePlayer);

	if (bHadSurveillance)
	{
		pCity->plot()->changeEspionageSight(m_pPlayer->getTeam(), pCity, /*1*/ GD_INT_GET(ESPIONAGE_SURVEILLANCE_SIGHT_RANGE), false);
	}

	pCityEspionage->SetPendingEvents(ePlayer, 0);

	return true;
}

/// LevelUpSpy - Move spy to next experience level
void CvPlayerEspionage::LevelUpSpy(uint uiSpyIndex, int iExperience)
{
	// if the spy can level up and it's not dead
	if(m_aSpyList[uiSpyIndex].m_eRank < NUM_SPY_RANKS - 1 && !(m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_DEAD || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_TERMINATED))
	{
		bool bCanLevel = true;
		if (iExperience > 0)
		{
			int iCurrentExperience = m_aSpyList[uiSpyIndex].m_iExperience;
			iCurrentExperience += iExperience;
			int iExperienceDenominator = /*100*/ GD_INT_GET(ESPIONAGE_SPY_EXPERIENCE_DENOMINATOR);
			iExperienceDenominator *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iExperienceDenominator /= 100;
			if (iCurrentExperience < iExperienceDenominator)
			{
				bCanLevel = false;
				m_aSpyList[uiSpyIndex].m_iExperience = iCurrentExperience;
			}
			else
			{
				m_aSpyList[uiSpyIndex].m_iExperience = iCurrentExperience % iExperienceDenominator;
			}
		}
		if (bCanLevel)
		{
			CvSpyRank eOriginalRank = m_aSpyList[uiSpyIndex].m_eRank;

			// announce promotion through notification
			m_aSpyList[uiSpyIndex].m_eRank = (CvSpyRank)(m_aSpyList[uiSpyIndex].m_eRank + 1);

			CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			if (pNotifications)
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
}

/// SetPassive - Passive spies will not attempt to rig elections or steal technology
void CvPlayerEspionage::SetPassive(uint uiSpyIndex, bool bPassive) {
	m_aSpyList[uiSpyIndex].m_bPassive = bPassive;
}

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
int CvPlayerEspionage::CalcPerTurn(int iSpyState, CvCity* pCity, int iSpyIndex, bool bGlobalCheck)
{
	CvAssertMsg(pCity, "pCity is null");
	switch(iSpyState)
	{
	case SPY_STATE_TRAVELLING:
	{
		int iBase = 1;
		if (!bGlobalCheck && m_pPlayer->GetPlayerTraits()->GetSpyMoveRateBonus() != 0)
		{
			iBase += m_pPlayer->GetPlayerTraits()->GetSpyMoveRateBonus();
		}
		return iBase;
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
			if (MOD_BALANCE_CORE_SPIES_ADVANCED)
			{
				/* Network Points */
				return CalcNetworkPointsPerTurn((CvSpyState)iSpyState, pCity, iSpyIndex);
			}
			else
			{
				PlayerTypes eCityOwner = pCity->getOwner();
				int iBaseYieldRate = pCity->getYieldRateTimes100(YIELD_SCIENCE, false);
				iBaseYieldRate *= GD_INT_GET(ESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT);
				iBaseYieldRate /= 100;
				iBaseYieldRate *= GC.getGame().getGameSpeedInfo().getSpyRatePercent();
				iBaseYieldRate /= 100;

				int iCityEspionageModifier = pCity->GetEspionageModifier();
				int iPlayerEspionageModifier = GET_PLAYER(eCityOwner).GetEspionageModifier();
				int iTheirPoliciesEspionageModifier = GET_PLAYER(eCityOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_SLOWER_MODIFIER);
				int iMyPoliciesEspionageModifier = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER);
				int iFinalModifier = (iBaseYieldRate * (100 + iCityEspionageModifier + iPlayerEspionageModifier + iTheirPoliciesEspionageModifier + iMyPoliciesEspionageModifier)) / 100;

				int iResult = max(iFinalModifier, 1);
				return iResult;
			}
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
				iResult = (iSpyRank + 1);
				if (!MOD_BALANCE_VP)
				{
					iResult *= (iSpyRank + 1);
				}
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
int CvPlayerEspionage::CalcRequired(int iSpyState, CvCity* pCity, int iSpyIndex, bool bGlobalCheck)
{
	DEBUG_VARIABLE(iSpyIndex);
	CvAssertMsg(pCity, "pCity is null");
	CvAssertMsg(iSpyIndex >= 0, "iSpyIndex is out of bounds");

	switch (iSpyState)
	{
	case SPY_STATE_TRAVELLING:
	{
		if (MOD_BALANCE_CORE_SPIES_ADVANCED)
		{
			int iTravel = iSpyTurnsToTravel;
			if (pCity != NULL && (pCity->getOwner() != m_pPlayer->GetID()))
			{
				if (m_pPlayer->GetProximityToPlayer(pCity->getOwner()) == PLAYER_PROXIMITY_NEIGHBORS)
				{
					iTravel += 1;
				}
				else if (m_pPlayer->GetProximityToPlayer(pCity->getOwner()) == PLAYER_PROXIMITY_CLOSE)
				{
					iTravel += 2;
				}
				else if (m_pPlayer->GetProximityToPlayer(pCity->getOwner()) == PLAYER_PROXIMITY_FAR)
				{
					iTravel += 3;
				}
				else if (m_pPlayer->GetProximityToPlayer(pCity->getOwner()) == PLAYER_PROXIMITY_DISTANT)
				{
					iTravel += 4;
				}
			}
			if (GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()))
			{
				iTravel /= 2;
				if (iTravel < 1)
				{
					iTravel = 1;
				}
			}
			return iTravel;
		}
		else
		{
			return iSpyTurnsToTravel;
		}
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
				return 0;
			}
			else
			{

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
				iMaxTechCost *= 100;

				return iMaxTechCost;
			}
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

int CvPlayerEspionage::CalcNetworkPointsPerTurn(CvSpyState eSpyState, CvCity* pCity, int iSpyIndex, CvString* toolTipSink)
{
	if (!MOD_BALANCE_VP)
		return 0;

	if (!pCity)
		return 0;

	if (eSpyState != SPY_STATE_GATHERING_INTEL && eSpyState != SPY_STATE_SCHMOOZE)
		return 0;

	int iNP = 0;
	int iTemp = 0;

	PlayerTypes ePlayer = m_pPlayer->GetID();
	PlayerTypes eCityOwner = pCity->getOwner();

	// Base Influence
	iTemp = /*20*/ GD_INT_GET(ESPIONAGE_NP_BASE);
	iNP += iTemp;
	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_BASE_TT", iTemp);

	// Spy Level
	int iRank = m_aSpyList[iSpyIndex].GetSpyRank(ePlayer);
	iTemp = iRank * /*10*/ GD_INT_GET(ESPIONAGE_NP_PER_SPY_RANK);
	iNP += iTemp;
	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_SPY_RANK_TT", iTemp);

	// Cultural Influence
	int iInfluenceLevel = (int) m_pPlayer->GetCulture()->GetInfluenceLevel(eCityOwner);
	iTemp = max(0, (iInfluenceLevel - 1) * /*10*/ GD_INT_GET(ESPIONAGE_NP_CULTURAL_INFLUENCE)); // 10 per influence level above exotic
	iNP += iTemp;
	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_CULTURAL_INFLUENCE_TT", iTemp);

	// Tech Difference
	int iTechDifference = GET_TEAM(GET_PLAYER(pCity->getOwner()).getTeam()).GetTeamTechs()->GetNumTechsKnown() - GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown();
	iTemp = max(0, min(iTechDifference, /*10*/ GD_INT_GET(ESPIONAGE_NP_MAX_NUM_TECH))) * /*2*/ GD_INT_GET(ESPIONAGE_NP_PER_TECHNOLOGY_BEHIND);
	iNP += iTemp;
	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_TECH_DIFFERENCE_TT", iTemp);

	// Player-level modifiers (policies etc.)
	iTemp = m_pPlayer->GetEspionageNetworkPoints();
	iNP += iTemp;
	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_POLICIES_TT", iTemp);

	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	if (pCityEspionage && pCityEspionage->HasCounterSpy())
	{
		CityEventChoiceTypes eCounterspyFocus = pCityEspionage->GetCounterSpyFocus();
		if (eCounterspyFocus != NO_EVENT_CHOICE_CITY)
		{
			CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eCounterspyFocus);
			if (pkEventChoiceInfo && pkEventChoiceInfo->getCounterspyNPRateReduction() > 0 )
			{
				iTemp = -pkEventChoiceInfo->getCounterspyNPRateReduction() * (pCityEspionage->GetCounterSpyRank() + 1);
				iNP += iTemp;
				GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_COUNTERSPY_NETWORK_TT", iTemp);
			}
		}
	}

	int iMyPoliciesEspionageModifier = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER);
	if (iMyPoliciesEspionageModifier > 0)
	{
		iTemp *= 100 + iMyPoliciesEspionageModifier;
		iTemp /= 100;
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_POLICYMOD_FASTER_TT", iTemp);
	}

	iNP = max(iNP, 0);

	int iSecurity = pCity->CalculateCitySecurity();
	if (iNP > 0 && iSecurity > 0)
	{
		// NP before Security:
		if(toolTipSink)
			(*toolTipSink) += "[NEWLINE]---------------------------";

		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_BEFORE_SECURITY_TT", iNP);

		int iSecurityPercentReduction = iSecurity * /*160*/ GD_INT_GET(ESPIONAGE_NP_REDUCTION_PER_SECURITY_POINT) / 100;
		GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_SECURITY_MOD_TT", -iSecurityPercentReduction);
		iNP *= (100 - iSecurityPercentReduction);
		iNP /= 100;

		if (toolTipSink)
			(*toolTipSink) += "[NEWLINE]---------------------------";

	}
	

	GC.getGame().BuildProdModHelpText(toolTipSink, "TXT_KEY_EO_NETWORK_POINTS_FINAL_TT", iNP, "", true);
	return iNP;
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
	else if (m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_RIG_ELECTION || (!MOD_BALANCE_VP && (m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE)))
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

	return m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE;
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

	// no coups in VP
	if (MOD_BALANCE_VP)
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
int CvPlayerEspionage::GetCoupChanceOfSuccess(uint uiSpyIndex, bool bIgnoreEnemySpies)
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
	int iMySpyRank = m_aSpyList[uiSpyIndex].GetSpyRank(m_pPlayer->GetID());
	return GetTheoreticalChanceOfCoup(pCity, iMySpyRank, bIgnoreEnemySpies);
}

int CvPlayerEspionage::GetTheoreticalChanceOfCoup(CvCity* pCity, int iMySpyRank, bool bIgnoreEnemySpies)
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
	if (!bIgnoreEnemySpies && pCityEspionage->m_aiSpyAssignment[eAllyPlayer] != -1)
	{
		bNoAllySpy = false;
		int iAllySpyIndex = pCityEspionage->m_aiSpyAssignment[eAllyPlayer];
		iAllySpyRank = GET_PLAYER(eAllyPlayer).GetEspionage()->m_aSpyList[iAllySpyIndex].GetSpyRank(eAllyPlayer);
	}

	int iAllyInfluence = pMinorCivAI->GetEffectiveFriendshipWithMajorTimes100(eAllyPlayer);
	int iMyInfluence = pMinorCivAI->GetEffectiveFriendshipWithMajorTimes100(m_pPlayer->GetID());
	int iDeltaInfluence = iAllyInfluence - iMyInfluence;

	float fNobodyBonus = 0.5;
	float fMultiplyConstant = 3.0f;
	float fSpyLevelDeltaZero = 0.0f;
	float fSpyLevelDeltaOne = 1.5f;
	float fSpyLevelDeltaTwo = 2.25;
	float fSpyLevelDeltaThree = 2.6f;
	float fSpyLevelDeltaFour = 2.8f;

	float fAllySpyValue = 0.0f;
	float fMySpyValue = 0.0;

	int iSpyRankModifier = iMySpyRank;
	iSpyRankModifier += m_pPlayer->GetCulture()->GetInfluenceCityStateSpyRankBonus(eCityOwner);
	switch (iSpyRankModifier)
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

	float fSpyMultiplier = fAllySpyValue - fMySpyValue + fMultiplyConstant;
	if (bNoAllySpy)
	{
		fSpyMultiplier *= fNobodyBonus;
	}

	int iResultPercentage = 100 - (int)((iDeltaInfluence * fSpyMultiplier) / 100);

	if (iResultPercentage > 85)
	{
		iResultPercentage = 85;
	}
	else if (iResultPercentage < 0)
	{
		iResultPercentage = 0;
	}

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
		strMsg += GetLocalizedText(m_aSpyList[uiSpyIndex].GetSpyName(m_pPlayer));
		LogEspionageMsg(strMsg);
	}

	bool bAttemptSuccess = false;
	int iRandRoll = GC.getGame().randRangeInclusive(1, 100, CvSeeder::fromRaw(0x12dd2b78).mix(pCity->plot()->GetPseudoRandomSeed()));
	if (iRandRoll <= GetCoupChanceOfSuccess(uiSpyIndex))
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
				int iNewInfluence = aiNewInfluenceValueTimes100[ui] - /*2000*/ (GD_INT_GET(ESPIONAGE_COUP_OTHER_PLAYERS_INFLUENCE_DROP) * 100);
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
		aiNewInfluenceValueTimes100[m_pPlayer->GetID()] = min(aiNewInfluenceValueTimes100[m_pPlayer->GetID()], -1000);
		bAttemptSuccess = false;

		// kill the spy
		ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
		m_aSpyList[uiSpyIndex].SetSpyState(m_pPlayer->GetID(), uiSpyIndex, SPY_STATE_DEAD); // have to official kill him after the extraction
	}
	// do others influence first so that the potential coup person will be the ally
	pMinorCivAI->SetDisableNotifications(true);
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if(!GET_PLAYER(ePlayer).isAlive())
		{
			continue;
		}
		pMinorCivAI->ResetNumConsecutiveSuccessfulRiggings(ePlayer);

		// skip the spy player
		if(ePlayer == m_pPlayer->GetID())
		{
			continue;
		}

		// send notification to other civs if they have met this minor
		bool bMetMinor = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(eCityOwner).getTeam());
		bool bMetMajor = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasMet(GET_PLAYER(m_pPlayer->GetID()).getTeam());
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
				if (bMetMajor)
				{
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
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_UNKNOWN_SUCCESS_S");
					strSummary << pCity->getNameKey();
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_UNKNOWN_SUCCESS");
					strNotification << pCity->getNameKey();
					strNotification << GET_PLAYER(ePreviousAlly).getCivilizationAdjectiveKey();
				}
			}
			else
			{
				if (bMetMajor)
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
				else
				{
					eNotification = NOTIFICATION_SPY_STAGE_COUP_FAILURE;
					strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_UNKNOWN_FAILURE_S");
					strSummary << pCity->getNameKey();
					strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_STAGE_COUP_UNKNOWN_FAILURE");
					strNotification << pCity->getNameKey();
					strNotification << GET_PLAYER(ePreviousAlly).getCivilizationAdjectiveKey();
				}
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

	if (bAttemptSuccess)
	{
		//Achievements!
		if (MOD_API_ACHIEVEMENTS && m_pPlayer->GetID() == GC.getGame().getActivePlayer())
			gDLL->UnlockAchievement(ACHIEVEMENT_XP1_13);
	}

	pMinorCivAI->SetCoupAttempted(m_pPlayer->GetID(), true);

	// Inform other alive minors, in case they had a quest that this fulfills (or fails)
	if (m_pPlayer->isMajorCiv())
	{
		for (int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinorCivLoop;
			if (eMinor != eCityOwner && GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
				GET_PLAYER(eMinor).GetMinorCivAI()->DoTestActiveQuestsForPlayer(m_pPlayer->GetID(), /*bTestComplete*/ true, /*bTestObsolete*/ true, MINOR_CIV_QUEST_COUP);
		}
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
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;
	int iAmountLeft = 0;
	int iTurnsLeft = 0;

	switch(m_aSpyList[uiSpyIndex].m_eSpyState)
	{
	case SPY_STATE_TRAVELLING:
	case SPY_STATE_SURVEILLANCE:
	case SPY_STATE_MAKING_INTRODUCTIONS:
	case SPY_STATE_GATHERING_INTEL:
		pCity = GetCityWithSpy(uiSpyIndex);
		CvAssertMsg(pCity, "GetCityWithSpy returned null. Has the wrong task");
		if (!pCity)
		{
			return -1;
		}
		pCityEspionage = pCity->GetCityEspionage();
		if (MOD_BALANCE_VP && m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL)
		{
			return -1;
		}
		else
		{
			iAmountLeft = pCityEspionage->m_aiGoal[ePlayer] - pCityEspionage->m_aiAmount[ePlayer];
			if (pCityEspionage->m_aiRate[ePlayer] != 0)
			{
				iTurnsLeft = iAmountLeft / pCityEspionage->m_aiRate[ePlayer];
				if (iAmountLeft % pCityEspionage->m_aiRate[ePlayer] > 0)
				{
					iTurnsLeft++;
				}
			}
			return iTurnsLeft;
		}
	case SPY_STATE_RIG_ELECTION:
		return GC.getGame().GetTurnsUntilMinorCivElection();
		break;
	case SPY_STATE_UNASSIGNED:
	case SPY_STATE_COUNTER_INTEL:
	case SPY_STATE_SCHMOOZE:
	case SPY_STATE_IMPRISONED:
	case SPY_STATE_DEAD:
	case SPY_STATE_TERMINATED:
		// no end time
		return -1;
		break;
	}

	return -1;
}


/// GetNetworkPointsStored - How many network points have been collected by the spy on his current mission?
int CvPlayerEspionage::GetNetworkPointsStored(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return -2;
	}

	PlayerTypes ePlayer = m_pPlayer->GetID();
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;

	if (MOD_BALANCE_VP && (/*m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE ||*/ m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL))
	{
		pCity = GetCityWithSpy(uiSpyIndex);
		if (pCity)
		{
			pCityEspionage = pCity->GetCityEspionage();
			return pCityEspionage->m_aiAmount[ePlayer];
		}
	}

	return -1;
}

/// GetNetworkPointsStored - What is the highest amount of network points the spy has ever had collected during his current stay?
int CvPlayerEspionage::GetMaxNetworkPointsStored(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return -2;
	}

	PlayerTypes ePlayer = m_pPlayer->GetID();
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;

	if (MOD_BALANCE_VP && (/*m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE ||*/ m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL))
	{
		pCity = GetCityWithSpy(uiSpyIndex);
		if (pCity)
		{
			pCityEspionage = pCity->GetCityEspionage();
			return pCityEspionage->m_aiMaxAmount[ePlayer];
		}
	}

	return -1;
}


/// GetNetworkPointsPerTurn - How many network points have been collected by the spy on its current mission?
int CvPlayerEspionage::GetNetworkPointsPerTurn(uint uiSpyIndex)
{
	CvAssertMsg(uiSpyIndex < m_aSpyList.size(), "iSpyIndex is out of bounds");
	if (uiSpyIndex >= m_aSpyList.size())
	{
		return -2;
	}

	PlayerTypes ePlayer = m_pPlayer->GetID();
	CvCity* pCity = NULL;
	CvCityEspionage* pCityEspionage = NULL;

	if (MOD_BALANCE_VP && (/*m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE ||*/ m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL))
	{
		pCity = GetCityWithSpy(uiSpyIndex);
		if (pCity)
		{
			pCityEspionage = pCity->GetCityEspionage();
			return pCityEspionage->m_aiRate[ePlayer];
		}
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
	case SPY_STATE_COUNTER_INTEL:
	case SPY_STATE_SCHMOOZE:
	case SPY_STATE_DEAD:
	case SPY_STATE_TERMINATED:
	case SPY_STATE_IMPRISONED:
		// no end time
		return -1;
	case SPY_STATE_TRAVELLING:
	case SPY_STATE_SURVEILLANCE:
	case SPY_STATE_GATHERING_INTEL:
	case SPY_STATE_MAKING_INTRODUCTIONS:
	{
		pCity = GetCityWithSpy(uiSpyIndex);
		if (pCity)
		{
			if (MOD_BALANCE_VP && m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL)
			{
				return -1;
			}
			else
			{
				pCityEspionage = pCity->GetCityEspionage();
				CvAssertMsg(pCityEspionage->m_aiGoal[ePlayer] != 0, "Trying to div by zero!");
				if (pCityEspionage->m_aiGoal[ePlayer] != 0)
				{
					return (pCityEspionage->m_aiAmount[ePlayer] * 100) / pCityEspionage->m_aiGoal[ePlayer];
				}
			}
		}
		else
		{
			return -1;
		}
	}
	case SPY_STATE_RIG_ELECTION:
		if(GC.getGame().GetTurnsBetweenMinorCivElections() != 0)
		{
			return ((GC.getGame().GetTurnsBetweenMinorCivElections() - GC.getGame().GetTurnsUntilMinorCivElection()) * 100) / GC.getGame().GetTurnsBetweenMinorCivElections();
		}
		else
		{
			return -1;
		}
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
		if(!(m_aSpyList[ui].m_eSpyState == SPY_STATE_DEAD || m_aSpyList[ui].m_eSpyState == SPY_STATE_TERMINATED))
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
		if(m_aSpyList[ui].m_eSpyState == SPY_STATE_DEAD || m_aSpyList[ui].m_eSpyState == SPY_STATE_TERMINATED)
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

std::vector<int> CvPlayerEspionage::BuildGWList(CvCity* pCity)
{
	std::vector<int> GWIds;
	if (pCity == NULL || pCity->GetCityBuildings()->GetNumGreatWorks() <= 0)
		return GWIds;

	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
	GreatWorkSlotType eMusicSlot = CvTypes::getGREAT_WORK_SLOT_MUSIC();
	GreatWorkSlotType eWritingSlot = CvTypes::getGREAT_WORK_SLOT_LITERATURE();

	int iOpenArtSlots = 0;
	int iOpenWritingSlots = 0;
	int iOpenMusicSlots = 0;

	int iLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		iOpenArtSlots += pLoopCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);
		iOpenMusicSlots += pLoopCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eMusicSlot);
		iOpenWritingSlots += pLoopCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots(eWritingSlot);
	}
	if (iOpenArtSlots <= 0 && iOpenWritingSlots <= 0 && iOpenMusicSlots <= 0)
		return GWIds;

	int iNumGreatWorks = pCity->GetCityBuildings()->GetNumGreatWorks();

	if (iNumGreatWorks <= 0)
		return GWIds;

	PlayerTypes ePlayer = pCity->getOwner();

	for (int iBuildingClassLoop = 0; iBuildingClassLoop < GC.getNumBuildingClassInfos(); iBuildingClassLoop++)
	{
		const CvCivilizationInfo& playerCivilizationInfo = GET_PLAYER(ePlayer).getCivilizationInfo();
		BuildingTypes eBuilding = NO_BUILDING;
		// If the option to check for all buildings in a class is enabled, we loop through all buildings in the city
		if (MOD_BUILDINGS_THOROUGH_PREREQUISITES)
		{
			eBuilding = pCity->GetCityBuildings()->GetBuildingTypeFromClass((BuildingClassTypes)iBuildingClassLoop);
		}
		else
		{
			eBuilding = (BuildingTypes)playerCivilizationInfo.getCivilizationBuildings((BuildingClassTypes)iBuildingClassLoop);
		}
		if (eBuilding != NO_BUILDING)
		{
			CvBuildingEntry *pkBuilding = GC.getBuildingInfo(eBuilding);
			if (pkBuilding)
			{
				if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					if (pkBuilding->GetGreatWorkSlotType() == eArtArtifactSlot && iOpenArtSlots > 0)
					{
						int iNumSlots = pkBuilding->GetGreatWorkCount();
						if (iNumSlots > 0)
						{
							for (int iI = 0; iI < iNumSlots; iI++)
							{
								int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
								if (iGreatWorkIndex != -1 && !m_pPlayer->GetCulture()->ControlsGreatWork(iGreatWorkIndex))
								{
									GWIds.push_back(iGreatWorkIndex);
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
								int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
								if (iGreatWorkIndex != -1 && !m_pPlayer->GetCulture()->ControlsGreatWork(iGreatWorkIndex))
								{
									GWIds.push_back(iGreatWorkIndex);
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
								int iGreatWorkIndex = pCity->GetCityBuildings()->GetBuildingGreatWork((BuildingClassTypes)iBuildingClassLoop, iI);
								if (iGreatWorkIndex != -1 && !m_pPlayer->GetCulture()->ControlsGreatWork(iGreatWorkIndex))
								{
									// add to list!
									GWIds.push_back(iGreatWorkIndex);
								}
							}
						}
					}
				}
			}
		}
	}
	return GWIds;
}


/// BuildStealableTechList - Go through opponents list and see what techs you can steal from them.
void CvPlayerEspionage::BuildStealableTechList(PlayerTypes ePlayer)
{
	if (ePlayer > MAX_MAJOR_CIVS)
		return;

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
	CvAssertMsg((uint)ePlayer < m_aaPlayerStealableTechList.size(), "ePlayer out of bounds");
	if((uint)ePlayer >= m_aaPlayerStealableTechList.size())
	{
		return -1;
	}

	return m_aaPlayerStealableTechList[ePlayer].size();
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

	// They are our vassal, so yes, we have a diplomat already
	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetMaster() == m_pPlayer->getTeam())
	{
		return true;
	}

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
void CvPlayerEspionage::AddSpyMessage(int iCityX, int iCityY, PlayerTypes eAttackingPlayer, int iSpyResult, CityEventChoiceTypes eMission, TechTypes eStolenTech, int iAmountStolen, int iGWID)
{
	SpyNotificationMessage kMessage;
	kMessage.m_iCityX = iCityX;
	kMessage.m_iCityY = iCityY;
	kMessage.m_eAttackingPlayer = eAttackingPlayer;
	kMessage.m_iSpyResult = iSpyResult;
	kMessage.m_iAmountStolen = iAmountStolen;
	kMessage.m_eStolenTech = eStolenTech;
	kMessage.m_iMission = (int)eMission;
	kMessage.m_iGWID = iGWID;

#if defined(MOD_EVENTS_ESPIONAGE)
	if (MOD_EVENTS_ESPIONAGE) {
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_EspionageNotificationData, iCityX, iCityY, eAttackingPlayer, m_pPlayer->GetID(), iSpyResult, eStolenTech, iAmountStolen);
	}
#endif

	m_aSpyNotificationMessages.push_back(kMessage);
}

/// UpdateMessage - updates an existing spy message with additional data about the mission
void CvPlayerEspionage::UpdateSpyMessage(int iCityX, int iCityY, PlayerTypes eAttackingPlayer, TechTypes eStolenTech, int iAmountStolen, int iGWID)
{
	// go through all messages to find the one to update
	for (uint ui = 0; ui < m_aSpyNotificationMessages.size(); ui++)
	{
		if (m_aSpyNotificationMessages[ui].m_iCityX == iCityX && m_aSpyNotificationMessages[ui].m_iCityY == iCityY && m_aSpyNotificationMessages[ui].m_eAttackingPlayer == eAttackingPlayer)
		{
			m_aSpyNotificationMessages[ui].m_eStolenTech = eStolenTech;
			m_aSpyNotificationMessages[ui].m_iAmountStolen = iAmountStolen;
			m_aSpyNotificationMessages[ui].m_iGWID = iGWID;
			// found the appropriate message, now leaving the function
			return;
		}
	}

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

		if (!MOD_BALANCE_VP)
		{
			CvTechEntry* pTechEntry = NULL;
			if (m_aSpyNotificationMessages[ui].m_eStolenTech != NO_TECH)
			{
				pTechEntry = GC.getTechInfo(m_aSpyNotificationMessages[ui].m_eStolenTech);
			}
			if (bMultiplayer || !pTechEntry)
			{
				switch (m_aSpyNotificationMessages[ui].m_iSpyResult)
				{
				case SPY_RESULT_UNDETECTED:
					// do nothing
					break;
				case SPY_RESULT_DETECTED:
					// notify defending player that a spy of unknown origin stole something
				{
					Localization::String strSummary(GetLocalizedText("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_DETECTED_WO_TECH_S"));
					Localization::String strNotification;

					if (pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)
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

					if (pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)
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
					if (iDefendingSpy >= 0)
					{
						Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY_S");

						strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();

						Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
						strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
						strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
						strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
						strNotification << pCity->getNameKey();

						pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

						//Achievements
						if (MOD_API_ACHIEVEMENTS && m_pPlayer->GetID() == GC.getGame().getActivePlayer())
							gDLL->UnlockAchievement(ACHIEVEMENT_XP1_15);
					}
				}
				break;
				}
			}
			else
			{
				switch (m_aSpyNotificationMessages[ui].m_iSpyResult)
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
					if (pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
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
					if (pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()] == -1)  // no defensive spy
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
		else
		{
			// VP
			CvString strEffectText = pCity->GetScaledSpyEffectText((CityEventChoiceTypes)m_aSpyNotificationMessages[ui].m_iMission, false, m_aSpyNotificationMessages[ui].m_eStolenTech, m_aSpyNotificationMessages[ui].m_iGWID, m_aSpyNotificationMessages[ui].m_iAmountStolen);

			switch (m_aSpyNotificationMessages[ui].m_iSpyResult)
			{
			case SPY_RESULT_UNDETECTED:
				// do nothing
				break;
			case SPY_RESULT_CAUGHT_AND_IDENTIFIED:
			case SPY_RESULT_IDENTIFIED:
			{
				Localization::String strMessage;
				Localization::String strSummary;
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_EVENT_SUCCEEDED_ESPIONAGE");
				strMessage << pCity->getNameKey();
				strMessage << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
				strMessage << strEffectText;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_EVENT_SUCCEEDED_ESPIONAGE_T_ESPIONAGE");
				strSummary << pCity->getNameKey();
				strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationInfo().getAdjectiveKey();
				pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_IDENTIFIED, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
			}
			break;
			case SPY_RESULT_CAUGHT:
			case SPY_RESULT_DETECTED:
			{
				Localization::String strMessage;
				Localization::String strSummary;
				
				strMessage = Localization::Lookup("TXT_KEY_NOTIFICATION_EVENT_SUCCEEDED_ESPIONAGE_UNKNOWN");
				strMessage << pCity->getNameKey();
				strMessage << strEffectText;
				strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_EVENT_SUCCEEDED_T_ESPIONAGE_UNKNOWN");
				strSummary << pCity->getNameKey();
				pNotifications->Add(NOTIFICATION_TECH_STOLEN_SPY_DETECTED, strMessage.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
			}
			break;
			case SPY_RESULT_KILLED:
				// notify defending player that they killed a spy
			{
				int iDefendingSpy = pCityEspionage->m_aiSpyAssignment[m_pPlayer->GetID()];
				CvAssertMsg(iDefendingSpy >= 0, "iDefendingSpy value is out of bounds");
				if (iDefendingSpy >= 0)
				{
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY_S");

					strSummary << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();

					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
					strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
					strNotification << m_aSpyList[iDefendingSpy].GetSpyName(m_pPlayer);
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
					strNotification << pCity->getNameKey();
					strNotification << strEffectText;

					pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, m_aSpyNotificationMessages[ui].m_eAttackingPlayer);

					//Achievements
					if (MOD_API_ACHIEVEMENTS && m_pPlayer->GetID() == GC.getGame().getActivePlayer())
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_15);
				}
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
	return pMessage && pMessage->m_eSourcePlayer != NO_PLAYER && pMessage->m_iIntrigueType != NUM_INTRIGUE_TYPES;
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

CvString CvPlayerEspionage::GetScoringLogFileName() const
{
	CvString strLogName;
	strLogName = "EspionageScoringLog.csv";
	return strLogName;
}

void CvPlayerEspionage::LogEspionageMsg(CvString& strMsg)
{
	if(GC.getLogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		FILogFile* pLog = NULL;

		pLog = LOGFILEMGR.GetLog(GetLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d,", GC.getGame().getElapsedGameTurns());
		strBaseString += m_pPlayer->getCivilizationShortDescription();
		strBaseString += ",";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}
void CvPlayerEspionage::LogEspionageScoringMsg(CvString& strMsg)
{
	if (GC.getLogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		FILogFile* pLog = NULL;

		pLog = LOGFILEMGR.GetLog(GetScoringLogFileName(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d,", GC.getGame().getElapsedGameTurns());
		strBaseString += m_pPlayer->getCivilizationShortDescription();
		strBaseString += ",";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

/// GetSpyMissionEvent - returns the event type of the city event for spy missions
CityEventTypes CvPlayerEspionage::GetSpyMissionEvent()
{
	// cached for performance
	if (m_eSpyMissionEvent != NO_EVENT_CITY)
	{
		return m_eSpyMissionEvent;
	}

	CityEventTypes eSetupEvent = NO_EVENT_CITY;
	for (int iLoop = 0; iLoop < GC.getNumCityEventInfos(); iLoop++)
	{
		CityEventTypes eEvent = (CityEventTypes)iLoop;
		if (eEvent != NO_EVENT_CITY)
		{
			CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
			if (pkEventInfo == NULL)
				continue;

			if (pkEventInfo->isEspionageSetup())
			{
				eSetupEvent = eEvent;
				break;
			}
		}
	}
	m_eSpyMissionEvent = eSetupEvent;
	return m_eSpyMissionEvent;
}

/// GetCounterspyEvent - returns the event type of the city event for counterspy focus selection
CityEventTypes CvPlayerEspionage::GetCounterspyEvent()
{
	// cached for performance
	if (m_eCounterspyEvent != NO_EVENT_CITY)
	{
		return m_eCounterspyEvent;
	}

	CityEventTypes eCounterspyEvent = NO_EVENT_CITY;
	for (int iLoop = 0; iLoop < GC.getNumCityEventInfos(); iLoop++)
	{
		CityEventTypes eEvent = (CityEventTypes)iLoop;
		if (eEvent != NO_EVENT_CITY)
		{
			CvModCityEventInfo* pkEventInfo = GC.getCityEventInfo(eEvent);
			if (pkEventInfo == NULL)
				continue;

			if (pkEventInfo->IsCounterSpy())
			{
				eCounterspyEvent = eEvent;
				break;
			}
		}
	}
	m_eCounterspyEvent = eCounterspyEvent;
	return m_eCounterspyEvent;
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvPlayerEspionage& writeTo)
{
	uint uiVersion = 0;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	int iNumSpies = 0;
	loadFrom >> iNumSpies;

	CvEspionageSpy kTempSpy;

	for(int i = 0; i < iNumSpies; i++)
	{
		loadFrom >> kTempSpy;
		writeTo.m_aSpyList.push_back(kTempSpy);
	}

	int iNumSpyNames = 0;
	loadFrom >> iNumSpyNames;
	for(int i = 0; i < iNumSpyNames; i++)
	{
		int iSpyNameIndex = 0;
		loadFrom >> iSpyNameIndex;
		writeTo.m_aiSpyListNameOrder.push_back(iSpyNameIndex);
	}

	loadFrom >> writeTo.m_iSpyListNameOrderIndex;

	uint uiNumCivs = 0;
	loadFrom >> uiNumCivs;
	for(uint uiCiv = 0; uiCiv < uiNumCivs; uiCiv++)
	{
		TechList aTechList;
		writeTo.m_aaPlayerStealableTechList.push_back(aTechList);

		uint uiNumTechs = 0;
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
		int iNumTechsToSteal = 0;
		loadFrom >> iNumTechsToSteal;
		writeTo.m_aiNumTechsToStealList.push_back(iNumTechsToSteal);
	}
#if defined(MOD_BALANCE_CORE)
	loadFrom >> uiNumCivs;
	for(uint uiCiv = 0; uiCiv < uiNumCivs; uiCiv++)
	{
		int iNumSpyActionsDone = 0;
		loadFrom >> iNumSpyActionsDone;
		writeTo.m_aiNumSpyActionsDone.push_back(iNumSpyActionsDone);
	}
#endif

	int iNumMessages = 0;
	loadFrom >> iNumMessages;
	for(int i = 0; i < iNumMessages; i++)
	{
		SpyNotificationMessage kMessage;
		loadFrom >> kMessage.m_iCityX;
		loadFrom >> kMessage.m_iCityY;
		loadFrom >> kMessage.m_eAttackingPlayer;
		loadFrom >> kMessage.m_iSpyResult;
		loadFrom >> kMessage.m_iAmountStolen;
		loadFrom >> kMessage.m_eStolenTech;
		loadFrom >> kMessage.m_iMission;
		loadFrom >> kMessage.m_iGWID;
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
		int iProjectType = 0;
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
		saveTo << readFrom.m_aSpyNotificationMessages[ui].m_iAmountStolen;
		saveTo << readFrom.m_aSpyNotificationMessages[ui].m_eStolenTech;
		saveTo << readFrom.m_aSpyNotificationMessages[ui].m_iMission;
		saveTo << readFrom.m_aSpyNotificationMessages[ui].m_iGWID;
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
		m_aiMaxAmount[ui] = -1;
		m_aiNextPassiveBonus[ui] = -1;
		m_aiRate[ui] = -1;
		m_aiGoal[ui] = -1;
		m_aiLastProgress[ui] = -1;
		m_aiLastPotential[ui] = -1;
		m_aiLastBasePotential[ui] = -1;
		m_aiResult[ui] = SPY_RESULT_UNDETECTED;
		m_aiNumTimesCityRobbed[ui] = 0;
		m_aiPendingEventsForPlayer[ui] = 0;
		m_abRevealCityScreen[ui] = false;
		m_aiDiplomatTradeBonus[ui] = false;
		m_aiSciencePassivePerTurn[ui] = 0;
		m_aiVisionBonus[ui] = 0;
	}
}

/// SetActivity - setting the values that get processed
void CvCityEspionage::SetActivity(PlayerTypes ePlayer, int iAmount, int iRate, int iGoal)
{
	CvAssertMsg(m_aiAmount[ePlayer] == -1, "Amount has not been reset");
	CvAssertMsg(m_aiRate[ePlayer] == -1, "Rate has not been reset");
	CvAssertMsg(m_aiGoal[ePlayer] == -1, "Goal has not been reset");
	m_aiAmount[ePlayer] = iAmount;
	m_aiMaxAmount[ePlayer] = iAmount;
	m_aiRate[ePlayer] = iRate;
	m_aiGoal[ePlayer] = iGoal;
}

/// SetRate
void CvCityEspionage::SetRate(PlayerTypes ePlayer, int iRate)
{
	m_aiRate[ePlayer] = iRate;
}

/// Process - move one step of the rate into the amount
void CvCityEspionage::Process(PlayerTypes ePlayer)
{
	CvAssertMsg(m_aiAmount[ePlayer] != -1, "Amount has not been set");
	CvAssertMsg(m_aiRate[ePlayer] != -1, "Rate has not been set");
	CvAssertMsg(m_aiGoal[ePlayer] != -1, "Goal has not been set");
	m_aiAmount[ePlayer] += m_aiRate[ePlayer];
	m_aiMaxAmount[ePlayer] = max(m_aiMaxAmount[ePlayer], m_aiAmount[ePlayer]);
}

int CvCityEspionage::GetAmount(PlayerTypes ePlayer)
{
	return m_aiAmount[ePlayer];
}

/// GetMaxAmount - get maximum amount ever stored during the current stay of the spy. used for passive bonuses
int CvCityEspionage::GetMaxAmount(PlayerTypes ePlayer)
{
	return m_aiMaxAmount[ePlayer];
}

/// GetNextPassiveBonus - get threshold for the next passive bonus that will be unlocked
int CvCityEspionage::GetNextPassiveBonus(PlayerTypes ePlayer)
{
	return m_aiNextPassiveBonus[ePlayer];
}

/// SetNextPassiveBonus
void CvCityEspionage::SetNextPassiveBonus(PlayerTypes ePlayer, int iValue)
{
	m_aiNextPassiveBonus[ePlayer] = iValue;
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
	m_aiMaxAmount[ePlayer] = -1;
	m_aiNextPassiveBonus[ePlayer] = -1;
	m_aiGoal[ePlayer] = -1;
	m_aiRate[ePlayer] = -1;
}

/// ResetPassiveBonuses - reset all passive bonuses accumulated
void CvCityEspionage::ResetPassiveBonuses(PlayerTypes ePlayer)
{
	SetSciencePassivePerTurn(ePlayer, 0);
	SetDiplomatTradeBonus(ePlayer, 0);
	SetRevealCityScreen(ePlayer, false);
	SetVisionBonus(ePlayer, 0);
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
void CvCityEspionage::SetSpyResult(PlayerTypes eSpyOwner, int iSpyIndex, int iResult)
{
	m_aiResult[eSpyOwner] = iResult;
	if (MOD_EVENTS_ESPIONAGE)
	{
		GAMEEVENTINVOKE_HOOK(GAMEEVENT_EspionageResult, (int) eSpyOwner, iSpyIndex, iResult, m_pCity->getX(), m_pCity->getY());
	}
}

void CvCityEspionage::SetRevealCityScreen(PlayerTypes ePlayer, bool bValue)
{
	if (m_abRevealCityScreen[ePlayer] != bValue)
	{
		m_abRevealCityScreen[ePlayer] = bValue;
		// reveal city plot
		m_pCity->plot()->changeEspionageSight(GET_PLAYER(ePlayer).getTeam(), m_pCity, 0, bValue);
	}
}

bool CvCityEspionage::GetRevealCityScreen(PlayerTypes ePlayer)
{
	return m_abRevealCityScreen[ePlayer];
}

void CvCityEspionage::SetDiplomatTradeBonus(PlayerTypes ePlayer, int iValue)
{
	m_aiDiplomatTradeBonus[ePlayer] = iValue;
}

void CvCityEspionage::ChangeDiplomatTradeBonus(PlayerTypes ePlayer, int iValue)
{
	if (iValue != 0)
	{
		m_aiDiplomatTradeBonus[ePlayer] = m_aiDiplomatTradeBonus[ePlayer] + iValue;
	}
}

int CvCityEspionage::GetDiplomatTradeBonus(PlayerTypes ePlayer)
{
	return m_aiDiplomatTradeBonus[ePlayer];
}


void CvCityEspionage::SetSciencePassivePerTurn(PlayerTypes ePlayer, int iValue)
{
	m_aiSciencePassivePerTurn[ePlayer] = iValue;
}

void CvCityEspionage::ChangeSciencePassivePerTurn(PlayerTypes ePlayer, int iValue)
{
	if (iValue != 0)
	{
		m_aiSciencePassivePerTurn[ePlayer] = m_aiSciencePassivePerTurn[ePlayer] + iValue;
	}
}

int CvCityEspionage::GetSciencePassivePerTurn(PlayerTypes ePlayer)
{
	return m_aiSciencePassivePerTurn[ePlayer];
}


void CvCityEspionage::SetVisionBonus(PlayerTypes ePlayer, int iValue)
{
	// remove old vision bonus
	if (m_aiVisionBonus[ePlayer] > 0)
	{
		m_pCity->plot()->changeEspionageSight(GET_PLAYER(ePlayer).getTeam(), m_pCity, m_aiVisionBonus[ePlayer], false);
	}
	// set new vision bonus
	m_aiVisionBonus[ePlayer] = iValue;
	if (iValue > 0)
	{
		m_pCity->plot()->changeEspionageSight(GET_PLAYER(ePlayer).getTeam(), m_pCity, iValue, true);
	}
}

void CvCityEspionage::ChangeVisionBonus(PlayerTypes ePlayer, int iValue)
{
	if (iValue != 0)
	{
		SetVisionBonus(ePlayer, GetVisionBonus(ePlayer) + iValue);
	}
}

int CvCityEspionage::GetVisionBonus(PlayerTypes ePlayer)
{
	return m_aiVisionBonus[ePlayer];
}

void CvCityEspionage::AddNetworkPoints(PlayerTypes eSpyOwner, CvEspionageSpy* pSpy, int iNetworkPointsAdded)
{
	if (iNetworkPointsAdded == 0)
		return;

	CvPlayer* pPlayer = &GET_PLAYER(eSpyOwner);

	int iPassiveBonusThresholdBefore = GetMaxAmount(eSpyOwner);
	int iActiveBonusThresholdBefore = GetAmount(eSpyOwner);
	// how much NP do we need to get the next passive bonus?
	int iNextPassiveBonus = GetNextPassiveBonus(eSpyOwner);
	if (iNextPassiveBonus == -1)
	{
		// initialize iNextPassiveBonus
		iNextPassiveBonus = INT_MAX;
		for (int i = 0; i < GC.getNumSpyPassiveBonusInfos(); i++)
		{
			CvSpyPassiveBonusEntry* pPassiveBonusInfo = GC.getSpyPassiveBonusInfo((SpyPassiveBonusTypes)i);
			int iNPNeeded = pPassiveBonusInfo->GetNetworkPointsNeededScaled();
			if (iNPNeeded > iPassiveBonusThresholdBefore)
			{
				iNextPassiveBonus = min(iNextPassiveBonus, iNPNeeded);
			}
		}
		SetNextPassiveBonus(eSpyOwner, iNextPassiveBonus);
	}
	// add network points gained this turn to total
	SetRate(eSpyOwner, iNetworkPointsAdded);
	SetLastProgress(eSpyOwner, iNetworkPointsAdded);
	Process(eSpyOwner);
	int iPassiveBonusThresholdAfter = GetMaxAmount(eSpyOwner);
	int iActiveBonusThresholdAfter = GetAmount(eSpyOwner);

	// check for passive bonuses that have been unlocked
	if (iPassiveBonusThresholdAfter >= iNextPassiveBonus)
	{
		// activate passive bonuses
		// calculate next threshold
		iNextPassiveBonus = INT_MAX;
		for (int i = 0; i < GC.getNumSpyPassiveBonusInfos(); i++)
		{
			CvSpyPassiveBonusEntry* pPassiveBonusInfo = GC.getSpyPassiveBonusInfo((SpyPassiveBonusTypes)i);
			int iNPNeeded = pPassiveBonusInfo->GetNetworkPointsNeededScaled();
			if (iNPNeeded > iPassiveBonusThresholdAfter)
			{
				// passive bonus not unlocked yet
				iNextPassiveBonus = min(iNextPassiveBonus, iNPNeeded);
			}
			else if (iNPNeeded > iPassiveBonusThresholdBefore)
			{
				// these bonus is now unlocked!
				if (pPassiveBonusInfo->IsRevealCityScreen())
				{
					SetRevealCityScreen(eSpyOwner, true);
				}
				ChangeSciencePassivePerTurn(eSpyOwner, pPassiveBonusInfo->GetSciencePercentAdded());
				ChangeVisionBonus(eSpyOwner, pPassiveBonusInfo->GetTilesRevealed());
				// Notification
				CvNotifications* pNotifications = GET_PLAYER(eSpyOwner).GetNotifications();
				if (pNotifications)
				{
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_NEW_PASSIVE_BONUS");
					strNotification << pPlayer->GetEspionage()->GetSpyRankName(pSpy->m_eRank);
					strNotification << pSpy->GetSpyName(pPlayer);
					strNotification << m_pCity->getNameKey();
					strNotification << pPassiveBonusInfo->GetHelp();
					CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_NEW_PASSIVE_BONUS_S");
					pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_SUCCESS, strNotification.toUTF8(), strSummary, m_pCity->getX(), m_pCity->getY(), 0);
				}
			}
		}
		SetNextPassiveBonus(eSpyOwner, iNextPassiveBonus);
	}

	// notifications for active missions
	CvNotifications* pNotifications = GET_PLAYER(eSpyOwner).GetNotifications();
	if (pNotifications)
	{
		int iNumMissionsNowActive = 0;
		CvString strMissions = "";
		for (int iLoop = 0; iLoop < GC.getNumCityEventChoiceInfos(); iLoop++)
		{
			CityEventChoiceTypes eEventChoice = (CityEventChoiceTypes)iLoop;
			if (eEventChoice != NO_EVENT_CHOICE_CITY)
			{
				CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eEventChoice);
				if (pkEventChoiceInfo && pkEventChoiceInfo->isEspionageMission())
				{
					int iNP = pkEventChoiceInfo->GetNetworkPointsNeededScaled();
					if (iNP > iActiveBonusThresholdBefore && iNP <= iActiveBonusThresholdAfter)
					{
						strMissions += "[NEWLINE][ICON_BULLET] ";
						strMissions += Localization::Lookup(pkEventChoiceInfo->GetDescription()).toUTF8();
						iNumMissionsNowActive++;
					}
				}
			}
		}
		if (iNumMissionsNowActive > 0)
		{
			Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_NEW_MISSIONS_AVAILABLE");
			strNotification << pPlayer->GetEspionage()->GetSpyRankName(pSpy->m_eRank);
			strNotification << pSpy->GetSpyName(pPlayer);
			strNotification << m_pCity->getNameKey();
			strNotification << iNumMissionsNowActive;
			strNotification << strMissions;
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_NEW_MISSIONS_AVAILABLE_S");
			pNotifications->Add(NOTIFICATION_SPY_RIG_ELECTION_SUCCESS, strNotification.toUTF8(), strSummary, m_pCity->getX(), m_pCity->getY(), 0);
		}
	}
}

void CvCityEspionage::DoMission(PlayerTypes eSpyOwner, CityEventChoiceTypes eMission)
{
	CvModEventCityChoiceInfo* pkEventChoiceInfo = GC.getCityEventChoiceInfo(eMission);
	// apply mission cost

	int iNewNPValue = m_aiAmount[eSpyOwner] - pkEventChoiceInfo->GetNetworkPointsNeededScaled();
	if (iNewNPValue < 0)
	{
		UNREACHABLE();
	}

	m_aiAmount[eSpyOwner] = iNewNPValue;
}


int CvCityEspionage::GetSpyResult(PlayerTypes eSpyOwner)
{
	return m_aiResult[eSpyOwner];
}

void CvCityEspionage::SetPendingEvents(PlayerTypes ePlayer, int iValue)
{
	CvAssert(iValue >= 0);
	m_aiPendingEventsForPlayer[ePlayer] = iValue;
}

void CvCityEspionage::ChangePendingEvents(PlayerTypes ePlayer, int iChange)
{
	m_aiPendingEventsForPlayer[ePlayer] += iChange;
	CvAssert(m_aiPendingEventsForPlayer[ePlayer] >= 0);
}

int CvCityEspionage::GetPendingEvents(PlayerTypes ePlayer) const
{
	return m_aiPendingEventsForPlayer[ePlayer];
}
/// HasCounterSpy - if this city is occupied
bool CvCityEspionage::HasCounterSpy() const
{
	int iSpyID = m_aiSpyAssignment[m_pCity->getOwner()];
	if (iSpyID == -1)
		return false;

	CvEspionageSpy* pSpy = GET_PLAYER(m_pCity->getOwner()).GetEspionage()->GetSpyByID(iSpyID);
	return pSpy && pSpy->GetSpyState() == SPY_STATE_COUNTER_INTEL;
}
int CvCityEspionage::GetCounterSpyID() const
{
	return m_aiSpyAssignment[m_pCity->getOwner()];
}
CityEventChoiceTypes CvCityEspionage::GetCounterSpyFocus() const
{
	if (!HasCounterSpy())
		return NO_EVENT_CHOICE_CITY;

	int iSpyID = GetCounterSpyID();
	CvEspionageSpy* pSpy = GET_PLAYER(m_pCity->getOwner()).GetEspionage()->GetSpyByID(iSpyID);
	if (!pSpy)
		return NO_EVENT_CHOICE_CITY;

	return pSpy->GetSpyFocus();
}
int CvCityEspionage::GetCounterSpyRank() const
{
	if (!HasCounterSpy())
		return -1;

	int iSpyID = GetCounterSpyID();
	CvEspionageSpy* pSpy = GET_PLAYER(m_pCity->getOwner()).GetEspionage()->GetSpyByID(iSpyID);
	if (!pSpy)
		return -1;

	return pSpy->GetSpyRank(m_pCity->getOwner());
}

// serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvCityEspionage& writeTo)
{
	uint uiVersion = 0;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	uint uiNumSpyAssignment = 0;
	loadFrom >> uiNumSpyAssignment;
	for(uint ui = 0; ui < uiNumSpyAssignment; ui++)
	{
		loadFrom >> writeTo.m_aiSpyAssignment[ui];
	}

	// amount
	uint uiNumAmounts = 0;
	loadFrom >> uiNumAmounts;
	for(uint ui = 0; ui < uiNumAmounts; ui++)
	{
		loadFrom >> writeTo.m_aiAmount[ui];
	}
	
	// max amount
	uint uiNumMaxAmounts = 0;
	loadFrom >> uiNumMaxAmounts;
	for(uint ui = 0; ui < uiNumMaxAmounts; ui++)
	{
		loadFrom >> writeTo.m_aiMaxAmount[ui];
	}
	
	// next passive bonus
	uint uiNumNextPassiveBonus = 0;
	loadFrom >> uiNumNextPassiveBonus;
	for(uint ui = 0; ui < uiNumNextPassiveBonus; ui++)
	{
		loadFrom >> writeTo.m_aiNextPassiveBonus[ui];
	}

	// rate
	uint uiNumRates = 0;
	loadFrom >> uiNumRates;
	for(uint ui = 0; ui < uiNumRates; ui++)
	{
		loadFrom >> writeTo.m_aiRate[ui];
	}

	// goal
	uint uiNumGoals = 0;
	loadFrom >> uiNumGoals;
	for(uint ui = 0; ui < uiNumGoals; ui++)
	{
		loadFrom >> writeTo.m_aiGoal[ui];
	}

	// last progress
	int iNumLastProgresses = 0;
	loadFrom >> iNumLastProgresses;
	for(int i = 0; i < iNumLastProgresses; i++)
	{
		loadFrom >> writeTo.m_aiLastProgress[i];
	}

	// last potential
	int iNumLastPotential = 0;
	loadFrom >> iNumLastPotential;
	for (int i = 0; i < iNumLastPotential; i++)
	{
		loadFrom >> writeTo.m_aiLastPotential[i];
	}

	// last base potential
	int iNumLastBasePotential = 0;
	loadFrom >> iNumLastBasePotential;
	for (int i = 0; i < iNumLastBasePotential; i++)
	{
		loadFrom >> writeTo.m_aiLastBasePotential[i];
	}

	// results
	int iNumResults = 0;
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

	int iNumPendingEvents = 0;
	loadFrom >> iNumPendingEvents;
	for (int i = 0; i < iNumPendingEvents; i++)
	{
		loadFrom >> writeTo.m_aiPendingEventsForPlayer[i];
	}

	int iDiplomatTradeBonus = 0;
	loadFrom >> iDiplomatTradeBonus;
	for (int i = 0; i < iDiplomatTradeBonus; i++)
	{
		loadFrom >> writeTo.m_aiDiplomatTradeBonus[i];
	}

	int iNumSciencePassive = 0;
	loadFrom >> iNumSciencePassive;
	for (int i = 0; i < iNumSciencePassive; i++)
	{
		loadFrom >> writeTo.m_aiSciencePassivePerTurn[i];
	}

	int iNumRevealCityScreen = 0;
	loadFrom >> iNumRevealCityScreen;
	for (int i = 0; i < iNumRevealCityScreen; i++)
	{
		loadFrom >> writeTo.m_abRevealCityScreen[i];
	}

	int iNumVisionBonus = 0;
	loadFrom >> iNumVisionBonus;
	for (int i = 0; i < iNumVisionBonus; i++)
	{
		loadFrom >> writeTo.m_aiVisionBonus[i];
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

	// max amount
	saveTo << MAX_MAJOR_CIVS;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiMaxAmount[ui];
	}

	// next passive bonuse
	saveTo << MAX_MAJOR_CIVS;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiNextPassiveBonus[ui];
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

	saveTo << MAX_MAJOR_CIVS;
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiDiplomatTradeBonus[ui];
	}

	saveTo << MAX_MAJOR_CIVS;
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiSciencePassivePerTurn[ui];
	}

	saveTo << MAX_MAJOR_CIVS;
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_abRevealCityScreen[ui];
	}

	saveTo << MAX_MAJOR_CIVS;
	for (uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		saveTo << readFrom.m_aiVisionBonus[ui];
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
	if (m_pPlayer->isHuman() || m_pPlayer->isMinorCiv() || m_pPlayer->isBarbarian())
	{
		return;
	}
#if defined(MOD_BALANCE_CORE)
	//No spies? No need!
	if (m_pPlayer->GetEspionage()->GetNumAliveSpies() <= 0)
		return;
#endif

	if (!MOD_BALANCE_CORE_SPIES_ADVANCED)
	{
		StealTechnology();
		UpdateCivOutOfTechTurn();
		AttemptCoups();
	}
	else
	{
		PerformSpyMissions();
	}

	// mark these spies available to reassign
	EvaluateUnassignedSpies();
	EvaluateDefensiveSpies();
	EvaluateDiplomatSpies();
	EvaluateMinorCivSpies();

	// when the UN countdown starts, mark all spies to be reassigned
	if (!m_bUNCountdownStarted && GC.getGame().IsUnitedNationsActive())
	{
		m_bUNCountdownStarted = true;
		CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
		for (uint ui = 0; ui < pEspionage->m_aSpyList.size(); ui++)
		{
			CvEspionageSpy* pSpy = &(pEspionage->m_aSpyList[ui]);
			// don't process dead spies
			if (pSpy->GetSpyState() == SPY_STATE_DEAD || pSpy->GetSpyState() == SPY_STATE_TERMINATED || pSpy->GetSpyState() == SPY_STATE_IMPRISONED || pEspionage->GetNumTurnsSpyMovementBlocked(ui) > 0)
			{
				continue;
			}

			pSpy->m_bEvaluateReassignment = true;
			if (GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: UN constructed/reassign, %d,", ui);
				strMsg += GetLocalizedText(pSpy->GetSpyName(m_pPlayer));
				pEspionage->LogEspionageMsg(strMsg);
			}
		}
	}

	//check for need to check
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	bool bNeedEval = false;
	for (uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
	{
		// if the spy is flagged to be reassigned
		if (pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment)
		{
			bNeedEval = true;
			break;
		}
	}
	if (!bNeedEval)
		return;

	std::vector<ScoreCityEntry> apCityDiplomat = BuildDiplomatCityList();
	std::vector<ScoreCityEntry> apCityOffense = BuildOffenseCityList();
	std::vector<ScoreCityEntry> apCityDefense = BuildDefenseCityList();
	std::vector<ScoreCityEntry> apCityMinor = BuildMinorCityList();

	std::vector<ScoreCityEntry> aCityScores;

	std::stable_sort(apCityDiplomat.begin(), apCityDiplomat.end(), ScoreCityEntryHighEval());
	std::stable_sort(apCityOffense.begin(), apCityOffense.end(), ScoreCityEntryHighEval());
	std::stable_sort(apCityDefense.begin(), apCityDefense.end(), ScoreCityEntryHighEval());
	std::stable_sort(apCityMinor.begin(), apCityMinor.end(), ScoreCityEntryHighEval());

	for (uint i = 0; i < apCityDiplomat.size(); i++)
	{
		//reduce score for each of this type, so we don't just go full in on one type of spy
		apCityDiplomat[i].m_iScore = apCityDiplomat[i].m_iScore + (i * -5);
		aCityScores.push_back(apCityDiplomat[i]);
	}
	for (uint i = 0; i < apCityOffense.size(); i++)
	{
		//reduce score for each of this type, so we don't just go full in on one type of spy
		apCityOffense[i].m_iScore = apCityOffense[i].m_iScore + (i * -5);
		aCityScores.push_back(apCityOffense[i]);
	}
	for (uint i = 0; i < apCityDefense.size(); i++)
	{
		//reduce score for each of this type, so we don't just go full in on one type of spy
		apCityDefense[i].m_iScore = apCityDefense[i].m_iScore + (i * -5);
		aCityScores.push_back(apCityDefense[i]);
	}
	for (uint i = 0; i < apCityMinor.size(); i++)
	{
		//reduce score for each of this type, so we don't just go full in on one type of spy
		apCityMinor[i].m_iScore = apCityMinor[i].m_iScore + (i * -5);
		aCityScores.push_back(apCityMinor[i]);
	}

	std::stable_sort(aCityScores.begin(), aCityScores.end(), ScoreCityEntryHighEval());

	if (GC.getLogging())
	{
		for (uint i = 0; i < (uint)aCityScores.size(); i++)
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
			if (aCityScores[i].m_eMission != NO_EVENT_CHOICE_CITY && GC.getCityEventChoiceInfo(aCityScores[i].m_eMission))
			{
				strMsg += GC.getCityEventChoiceInfo(aCityScores[i].m_eMission)->GetDescription();
				strMsg += ",";
			}
			strMsg += GET_PLAYER(aCityScores[i].m_pCity->getOwner()).isMinorCiv() ? "minor civ," : "";
			strMsg += ",";
			strMsg += GET_PLAYER(aCityScores[i].m_pCity->getOwner()).getCivilizationShortDescription();
			strMsg += ",";
			pEspionage->LogEspionageScoringMsg(strMsg);
		}
	}

	for (uint i = 0; i < aCityScores.size(); i++)
	{
		CvCity* pCity = aCityScores[i].m_pCity;
		if (!pCity)
			continue;

		// is a spy already in this city?
		if (pEspionage->GetSpyIndexInCity(pCity) != -1)
		{
			uint uiSpyIndex = pEspionage->GetSpyIndexInCity(pCity);
			// if diplomat status doesn't match with what we want, remove the spy
			if (pEspionage->m_aSpyList[uiSpyIndex].m_bIsDiplomat != aCityScores[i].m_bDiplomat)
			{
				pEspionage->ExtractSpyFromCity(pEspionage->GetSpyIndexInCity(pCity));
			}
			else if (MOD_BALANCE_VP && aCityScores[i].m_eMission != NO_EVENT_CHOICE_CITY && pEspionage->m_aSpyList[uiSpyIndex].GetSpyFocus() != aCityScores[i].m_eMission)
			{
				// if the focus doesn't match with what we want, change it
				if (pCity->getOwner() == m_pPlayer->GetID())
				{
					// counterspy. we need to make sure the spy may change focus right now and notifications are given to other players
					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Changing Counterspy Focus! ");
						strMsg += pCity->getName();
						strMsg += ", ";
						strMsg += GC.getCityEventChoiceInfo(aCityScores[i].m_eMission)->GetDescription();
						pEspionage->LogEspionageMsg(strMsg);
					}
					pEspionage->ChangeCounterspyMission(uiSpyIndex, aCityScores[i].m_eMission);
				}
				else
				{
					// offensive spy. we can just change the focus in this case, as it's only used for the AI-internal evaluation what mission to go for
					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Changing Spy Mission Focus! ");
						strMsg += pCity->getName();
						strMsg += ", ";
						strMsg += GC.getCityEventChoiceInfo(aCityScores[i].m_eMission)->GetDescription();
						pEspionage->LogEspionageMsg(strMsg);
					}
					pEspionage->m_aSpyList[uiSpyIndex].SetSpyFocus(aCityScores[i].m_eMission);
				}
				// we're happy now. skip the city and skip the spy too!
				pEspionage->m_aSpyList[uiSpyIndex].m_bEvaluateReassignment = false;
				continue;
			}
			else
			{
				// otherwise, the spy is good there. skip the city and skip the spy too!
				pEspionage->m_aSpyList[uiSpyIndex].m_bEvaluateReassignment = false;
				continue;
			}
		}

		bool bNoSpiesLeft = true;
		// todo: sort spies by level, move highest-level spies to most important cities
		for (uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
		{
			// if the spy is flagged to be reassigned
			if (!pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment)
				continue;

			// dead spies are not processed
			if (pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD || pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_TERMINATED || pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_IMPRISONED || pEspionage->GetNumTurnsSpyMovementBlocked(uiSpy) > 0)
			{
				continue;
			}

			bNoSpiesLeft = false;

			if (GC.getLogging())
			{
				CvString strMsg;
				pEspionage->GetCityWithSpy(uiSpy) ? strMsg.Format("Moving Spy in %s to new city: ", pEspionage->GetCityWithSpy(uiSpy)->getName().c_str()) : strMsg.Format("Moving unassigned Spy to new city: ");
				strMsg += pCity->getName();
				if (aCityScores[i].m_eMission != NO_EVENT_CHOICE_CITY)
				{
					strMsg += ", ";
					strMsg += GC.getCityEventChoiceInfo(aCityScores[i].m_eMission)->GetDescription();
				}
				pEspionage->LogEspionageMsg(strMsg);
			}

			pEspionage->MoveSpyTo(pCity, uiSpy, aCityScores[i].m_bDiplomat);
			pEspionage->m_aSpyList[uiSpy].SetSpyFocus(aCityScores[i].m_eMission);
			pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment = false;
			break;

		}
		if (bNoSpiesLeft)
			break;
	}
}


/// VP only: Checks if any offensive spies can perform the mission they're assigned to
/// Check if spies should be reassigned if the mission requirements are not fulfilled
void CvEspionageAI::PerformSpyMissions()
{
	if (!MOD_BALANCE_VP)
		return;

	PlayerTypes ePlayer = m_pPlayer->GetID();
	CityEventTypes eSpyMissionParentEvent = m_pPlayer->GetEspionage()->GetSpyMissionEvent();
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();

	// if the spy has collected a lot of NP already, we're less willing to reassign them
	int iHighNPThreshold = /*20*/ GD_INT_GET(ESPIONAGE_NP_BASE) * 40;
	iHighNPThreshold *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iHighNPThreshold /= 100;

	std::vector<int> aSpyMissionList;
	for (int i = 0; i < GC.getNumCityEventChoiceInfos(); i++)
	{
		CvModEventCityChoiceInfo* pkMissionInfo = GC.getCityEventChoiceInfo((CityEventChoiceTypes)i);
		if (pkMissionInfo->isEspionageMission())
		{
			aSpyMissionList.push_back(i);
		}
	}

	for (uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
	{
		CvEspionageSpy* pSpy = &pEspionage->m_aSpyList[uiSpy];
		if (pSpy->GetSpyState() != SPY_STATE_GATHERING_INTEL)
			continue;

		CvCity* pCity = pEspionage->GetCityWithSpy(uiSpy);
		CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
		//todo: regular logging of all spies and their status every 5 turns

		// if no mission selected, select one now
		if (pSpy->GetSpyFocus() == NO_EVENT_CHOICE_CITY)
		{
			int iScore = 0;
			CityEventChoiceTypes eBestMission = GetBestMissionInCity(pCity, iScore, aSpyMissionList, uiSpy);
			if (eBestMission == NO_EVENT_CHOICE_CITY)
			{
				pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment = true;
				break;
			}
			if (GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("AI has chosen a Spy Mission: ");
				strMsg += pCity->getName();
				strMsg += ", ";
				strMsg += GC.getCityEventChoiceInfo(eBestMission)->GetDescription();
				pEspionage->LogEspionageMsg(strMsg);
			}
			pSpy->SetSpyFocus(eBestMission);
		}
		
		int iCurrentMissionScore = GetMissionScore(pCity, pSpy->GetSpyFocus(), uiSpy);
		// low mission score? reeval
		if (iCurrentMissionScore < 20)
		{
			if (iCurrentMissionScore == -1)
			{
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Mission requirements no longer met, reeval: %s, %s, %d", pCity->getName().c_str(), GC.getCityEventChoiceInfo(pSpy->GetSpyFocus())->GetDescription(), iCurrentMissionScore);
					pEspionage->LogEspionageMsg(strMsg);
				}
			}
			else
			{
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Low Mission Score, reeval: %s, %s, %d", pCity->getName().c_str(), GC.getCityEventChoiceInfo(pSpy->GetSpyFocus())->GetDescription(), iCurrentMissionScore);
					pEspionage->LogEspionageMsg(strMsg);
				}
			}
			pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment = true;
			break;
		}

		// check if it's better to switch missions
		int iBestMissionScore = 0;
		CityEventChoiceTypes eBestMission = GetBestMissionInCity(pCity, iBestMissionScore, aSpyMissionList, uiSpy);
		if (iBestMissionScore > iCurrentMissionScore)
		{
			if (GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Switching to mission with better score: %s, %s, %d", pCity->getName().c_str(), GC.getCityEventChoiceInfo(eBestMission)->GetDescription(), iBestMissionScore);
				pEspionage->LogEspionageMsg(strMsg);
			}
			pSpy->SetSpyFocus(eBestMission);
			iCurrentMissionScore = iBestMissionScore;
		}

		CityEventChoiceTypes eMission = pSpy->GetSpyFocus();
		CvModEventCityChoiceInfo* pkMissionInfo = GC.getCityEventChoiceInfo(eMission);

		// if selected mission can be conducted now, do it ...
		if (pCity->IsCityEventChoiceValidEspionage(eMission, eSpyMissionParentEvent, uiSpy, ePlayer))
		{
			// ... unless it's the "target defense" mission, which we use tactically
			if (pkMissionInfo->getSapCityTurns() > 0)
			{
				if (pCity->plot()->GetNumEnemyUnitsAdjacent(pCity->getTeam(), NO_DOMAIN) >= 2 && pCity->getDamageTakenLastTurn() > 5)
				{
					if (GC.getLogging())
					{
						CvString strMsg;
						strMsg.Format("Performing espionage mission! %s, %s, %d", pCity->getName().c_str(), GC.getCityEventChoiceInfo(eMission)->GetDescription(), iCurrentMissionScore);
						pEspionage->LogEspionageMsg(strMsg);
					}
					// city under siege, do the mission now!
					pCity->DoEventChoice(eMission, NO_EVENT_CITY, false, uiSpy, ePlayer);
					break;
				}
				else
				{
					// wait until the right moment
					break;
				}
			}
			else
			{
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Performing espionage mission! %s, %s, %d", pCity->getName().c_str(), GC.getCityEventChoiceInfo(eMission)->GetDescription(), iCurrentMissionScore);
					pEspionage->LogEspionageMsg(strMsg);
				}
				pCity->DoEventChoice(eMission, NO_EVENT_CITY, false, uiSpy, ePlayer);
				break;
			}
		}
		
		// check if the spy is making slow progress
		if (pCityEspionage->m_aiRate[ePlayer] <= 15 || (pCityEspionage->m_aiAmount[ePlayer] > iHighNPThreshold && pCityEspionage->m_aiRate[ePlayer] <= 5))
		{	
			// opportunistic check if any other mission can be done now
			CityEventChoiceTypes eOpportunityMission = NO_EVENT_CHOICE_CITY;
			for (size_t i = 0; i < aSpyMissionList.size(); i++)
			{
				CityEventChoiceTypes eLoopMission = (CityEventChoiceTypes)aSpyMissionList[i];
				if (pCity->IsCityEventChoiceValidEspionage(eLoopMission, eSpyMissionParentEvent, uiSpy, ePlayer))
				{
					eOpportunityMission = eLoopMission;
				}
			}
			if (eOpportunityMission != NO_EVENT_CHOICE_CITY)
			{
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Slow progress, perform other mission instead! ");
					strMsg += pCity->getName();
					strMsg += ", ";
					strMsg += GC.getCityEventChoiceInfo(eOpportunityMission)->GetDescription();
					pEspionage->LogEspionageMsg(strMsg);
				}
				pCity->DoEventChoice(eMission, NO_EVENT_CITY, false, uiSpy, ePlayer);
				break;
			}
			
			if (GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Spy not making enough progress, reevaluating: %s, %s, %d/%d (+%d)",
					pCity->getName().c_str(), pkMissionInfo->GetDescription(), pCityEspionage->m_aiAmount[ePlayer], pkMissionInfo->GetNetworkPointsNeededScaled(), pCityEspionage->m_aiRate[ePlayer]);
				pEspionage->LogEspionageMsg(strMsg);
			}
			pEspionage->m_aSpyList[uiSpy].m_bEvaluateReassignment = true;
			break;
		}
	}
}

/// Checks to see if there are any technologies to steal
/// If so, steals them! (CP only)
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
			pEspionage->m_aiNumSpyActionsDone[eDefendingPlayer]++;
			
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
		int iSpyRank = m_pPlayer->GetEspionage()->GetSpyByID(uiSpy)->GetSpyRank(m_pPlayer->GetID());

		int iChanceOfSuccess = pEspionage->GetCoupChanceOfSuccess(uiSpy);
		if (iChanceOfSuccess >= 60 + 10*iSpyRank)
		{
			int iRoll = GC.getGame().randRangeInclusive(1, 100, m_pPlayer->GetPseudoRandomSeed().mix(GET_PLAYER(pCity->getOwner()).GetPseudoRandomSeed()).mix(uiSpy));
			if (iRoll <= iChanceOfSuccess)
			{
				pEspionage->AttemptCoup(uiSpy);
			}
		}
	}
}

/// VP only. Returns AI evaluation of a mission in a city
int CvEspionageAI::GetMissionScore(CvCity* pCity, CityEventChoiceTypes eMission, int iSpyIndex)
{
	CvModEventCityChoiceInfo* pkMissionInfo = GC.getCityEventChoiceInfo(eMission);
	if (!pkMissionInfo)
		return -1;

	if (GET_PLAYER(pCity->getOwner()).isMinorCiv())
		return -1;

	PlayerTypes ePlayer = m_pPlayer->GetID();
	CvDiplomacyAI* pDiplomacyAI = GET_PLAYER(ePlayer).GetDiplomacyAI();

	if (ePlayer == pCity->getOwner())
	{
		// own city. evaluate only counterspy missions
		if (!pkMissionInfo->isCounterspyMission())
			return -1;

		CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();

		int iScore = GD_INT_GET(ESPIONAGE_XP_PER_TURN_COUNTERSPY);
		if (pkMissionInfo->isCounterspyBlockSapCity())
		{
			// this is only interesting if we're under siege
			if (!m_pPlayer->IsAtPeaceAllMajors() || pCity->isUnderSiege())
			{
				// value based on the damage percent the city has taken
				iScore += min(50, 100 * pCity->getDamage() / pCity->GetMaxHitPoints()) * 4;

				if (pCity->isCapital())
				{
					iScore *= 2;
				}
			}
		}
		else
		{
			// irrelevant unless in capital
			if (pCity->isCapital())
			{
				iScore += 15;
				// how many spy actions have been performed against us?
				int iTurnsOfEspionage = GC.getGame().getGameTurn() - m_pPlayer->GetEspionageAI()->m_iTurnEspionageStarted;
				iTurnsOfEspionage *= 100;
				iTurnsOfEspionage /= GC.getGame().getGameSpeedInfo().getTrainPercent();
				if (m_pPlayer->GetEspionageAI()->m_iTurnEspionageStarted != 0 && iTurnsOfEspionage > 0)
				{
					int iNumCityRobbed = 0;
					for (int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
					{
						iNumCityRobbed += pCityEspionage->m_aiNumTimesCityRobbed[(PlayerTypes)iLoopPlayer];
					}
					iScore += min(75, 1000 * iNumCityRobbed / iTurnsOfEspionage);
				}

				if (iSpyIndex >= 0 && m_pPlayer->GetEspionage()->GetSpyByID(iSpyIndex))
				{
					int iSpyRank = m_pPlayer->GetEspionage()->GetSpyByID(iSpyIndex)->m_eRank;
					// this is only to differentiate between the two following missions.
					// higher-level spies should go for the np rate reduction because its more effective for them
					if (iSpyRank == 0 && pkMissionInfo->isAlwaysIDSpies())
					{
						iScore += 1;
					}
					else if (iSpyRank > 0 && pkMissionInfo->getCounterspyNPRateReduction() > 0)
					{
						iScore += 1;
					}
				}
			}
		}
		return iScore;
	}
	else
	{
		// foreign city. evaluate only offensive missions
		if (!pkMissionInfo->isEspionageMission())
			return -1;

		PlayerTypes eTargetPlayer = pCity->getOwner();

		// mission requirements (excluding cooldowns and network point requirements) not met?
		CityEventTypes eParentEvent = m_pPlayer->GetEspionage()->GetSpyMissionEvent();
		if (!pCity->IsCityEventChoiceValidEspionage(eMission, eParentEvent, iSpyIndex, ePlayer, true, true))
			return -1;

		int iScore = 0;
		if (pkMissionInfo->getStealTech() > 0)
		{
			iScore +=  15 + min(m_pPlayer->GetEspionage()->GetNumTechsToSteal(eTargetPlayer) * 15, 75);
			if (pDiplomacyAI->IsGoingForSpaceshipVictory())
			{
				iScore += 25;
			}
		}
		if (pkMissionInfo->getStealGW() > 0)
		{
			iScore += 15 + min(pCity->GetCityCulture()->GetNumGreatWorks() * 10, 50);
			if (pDiplomacyAI->IsGoingForCultureVictory())
			{
				iScore += 25;
				// if we want to go for culture victory and still need a lot of influence, focus on this mission
				if (!pDiplomacyAI->IsCloseToCultureVictory())
				{
					iScore += 25;
				}
			}
			if (GET_PLAYER(eTargetPlayer).GetDiplomacyAI()->IsCloseToCultureVictory())
			{
				// try to stop another player from winning?
				iScore += 25;
			}
		}
		if (pkMissionInfo->getNoTourismTurns() > 0)
		{
			iScore = min(pCity->GetBaseTourism() / 5, 75);
			if (GET_PLAYER(eTargetPlayer).GetDiplomacyAI()->IsCloseToCultureVictory())
			{
				iScore += 25;
			}
		}
		if (pkMissionInfo->getRandomBarbs() > 0)
		{
			if (pDiplomacyAI->GetCivApproach(eTargetPlayer) <= MAJOR_CIV_APPROACH_HOSTILE)
			{
				iScore += 10;
			}
		}
		if (pkMissionInfo->getRemoveTurnsOfProductionProgress() > 0)
		{
			// only check while we're in the city
			if (iSpyIndex >= 0 && pCity->GetCityEspionage()->GetRevealCityScreen(ePlayer))
			{
				BuildingTypes eBuilding = pCity->getProductionBuilding();
				if (eBuilding != NO_BUILDING)
				{
					CvBuildingEntry* pBuildingInfo = GC.getBuildingInfo(eBuilding);
					CvAssertMsg(pBuildingInfo, "pBuildingInfo is null");
					if (pBuildingInfo)
					{
						if (::isWorldWonderClass(pBuildingInfo->GetBuildingClassInfo()))
						{
							// city is constructing a wonder. check progress
							if (pCity->getProduction() >= pBuildingInfo->GetProductionCost() / 2)
							{
								// are we constructing the same wonder somewhere
								CvCity* pLoopCity = NULL;
								int iLoop = 0;
								for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
								{
									if (pLoopCity->getProductionBuilding() == eBuilding)
									{
										// todo: compare progress. do mission only if it helps us
										iScore += 100;
										break;
									}
								}
							}
						}
					}
				}
			}
		}
		if (pkMissionInfo->getSpecialistsGreatPersonPointsPerTurn() != 0)
		{
			iScore = pCity->GetCityCitizens()->GetTotalSpecialistCount() + 3 * GET_PLAYER(ePlayer).getCapitalCity()->GetCityCitizens()->GetTotalSpecialistCount();
		}
		if (pkMissionInfo->getStealFromTreasuryPercent() > 0)
		{
			int iStealPercent = pkMissionInfo->getStealFromTreasuryPercent();
			// inverse scaling with game speed
			iStealPercent *= 100;
			iStealPercent /= GC.getGame().getGameSpeedInfo().getTrainPercent();
			int iAmount = GET_PLAYER(eTargetPlayer).GetTreasury()->GetGold() * iStealPercent / 100;

			iScore += iAmount / 100;
		}
		if (pkMissionInfo->getSapCityTurns() > 0)
		{
			if (m_pPlayer->IsAtWarWith(eTargetPlayer) || pDiplomacyAI->GetCivApproach(eTargetPlayer) == CIV_APPROACH_WAR)
			{
				// do we have a border with the city?
				int iBorderTiles = 0;
				for (int iI = RING1_PLOTS; iI < RING5_PLOTS; iI++)
				{
					CvPlot* pLoopPlot = pCity->GetCityCitizens()->GetCityPlotFromIndex(iI);
					if (!pLoopPlot)
						continue;

					if (pCity->GetID() != pLoopPlot->getOwningCityID() && pLoopPlot->getOwner() == ePlayer)
						iBorderTiles++;
				}
				iScore += min(100, iBorderTiles * 15);
			}
			if (iSpyIndex > 0)
			{
				// increase score if it's now a good opportunity to do the mission
				if (m_pPlayer->IsAtWarWith(eTargetPlayer) && pCity->plot()->GetNumEnemyUnitsAdjacent(pCity->getTeam(), NO_DOMAIN) >= 2 && pCity->getDamageTakenLastTurn() > 5)
				{
					int iDamagePercent = 100 * pCity->getDamage() / pCity->GetMaxHitPoints();
					iScore += iDamagePercent * 2 + pCity->getDamageTakenLastTurn() * 5;
				}
			}
		}
		if (iSpyIndex >= 0 && iScore > 0)
		{
			// add current progress
			int iProgress = m_pPlayer->GetEspionage()->GetNetworkPointsStored(iSpyIndex);
			iProgress *= 100;
			iProgress /= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iScore += iProgress / 20;
		}
		iScore += GD_INT_GET(ESPIONAGE_XP_PER_TURN_OFFENSIVE);
		return iScore;
	}
}

/// VP only. Evaluates which mission to select in a city. iScore (passed by reference) returns the score of the mission. 
CityEventChoiceTypes CvEspionageAI::GetBestMissionInCity(CvCity* pCity, int& iScore, std::vector<int> aMissionList, int iSpyIndex)
{
	if (GET_PLAYER(pCity->getOwner()).isMinorCiv())
		return NO_EVENT_CHOICE_CITY;

	CityEventChoiceTypes eBestMission = NO_EVENT_CHOICE_CITY;
	iScore = -1;
	int iMissionScore = -1;
	for (size_t i = 0; i < aMissionList.size(); i++)
	{
		iMissionScore = GetMissionScore(pCity, (CityEventChoiceTypes)aMissionList[i], iSpyIndex);
		if (iMissionScore > iScore)
		{
			iScore = iMissionScore;
			eBestMission = (CityEventChoiceTypes)aMissionList[i];
		}
	}
	
	return eBestMission;
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

		if (pDiploAI->IsMaster(eTargetPlayer))
		{
			// diplomat always in city
			continue;
		}

		// if we can't see it, we can't move a diplomat there.
		if (!pCapitalCity->isRevealed(m_pPlayer->getTeam(), false, false))
		{
			continue;
		}

		iNumPlayers++;

		ScoreCityEntry kEntry;

		kEntry.m_pCity = pCapitalCity;

		int iLeagueScore = 0;
		CvLeagueAI::DiplomatUsefulnessLevels eUsefulness = m_pPlayer->GetLeagueAI()->GetDiplomatUsefulnessAtCiv(eTargetPlayer);
		switch (eUsefulness)
		{
		case(CvLeagueAI::DIPLOMAT_USEFULNESS_NONE) :
			continue;
		case(CvLeagueAI::DIPLOMAT_USEFULNESS_LOW) :
			iLeagueScore += 25;
			break;
		case(CvLeagueAI::DIPLOMAT_USEFULNESS_MEDIUM) :
			iLeagueScore += 50;
			break;
		case(CvLeagueAI::DIPLOMAT_USEFULNESS_HIGH) :
			iLeagueScore += 100;
			break;
		default:
			continue;
		}

		if (pDiploAI->IsGoingForDiploVictory())
			iLeagueScore *= 2;

		// tourism bonus
		int iTourismScore = 0;
		if (pDiploAI->IsGoingForCultureVictory())
		{
			int iInfluenceOn = m_pPlayer->GetCulture()->GetInfluenceOn(eTargetPlayer);
			int iLifetimeCulture = GET_PLAYER(eTargetPlayer).GetJONSCultureEverGenerated();
			int iPercent = 0;

			if (iLifetimeCulture > 0)
			{
				iPercent = iInfluenceOn * 100 / iLifetimeCulture;
			}

			if (iPercent < 100)
			{
				iTourismScore = min(50, (100 - iPercent) * 3);

				if (pDiploAI->IsCloseToCultureVictory())
				{
					// if this is one of the last holdouts, go for a diplomat here
					iTourismScore += 100;
				}
			}
		}

		kEntry.m_iScore = iLeagueScore + iTourismScore;
		kEntry.m_bDiplomat = true;
		kEntry.m_eMission = NO_EVENT_CHOICE_CITY;
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

	// for performance reasons, make a list of all the possible spy missions
	std::vector<int> aSpyMissionList;
	if (MOD_BALANCE_VP)
	{
		for (int i = 0; i < GC.getNumCityEventChoiceInfos(); i++)
		{
			CvModEventCityChoiceInfo* pkMissionInfo = GC.getCityEventChoiceInfo((CityEventChoiceTypes)i);
			if (pkMissionInfo->isEspionageMission())
			{
				aSpyMissionList.push_back(i);
			}
		}
	}

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
			continue;

		//if we can't steal from them, we don't care!
		if (!MOD_BALANCE_CORE_SPIES_ADVANCED)
		{
			m_pPlayer->GetEspionage()->BuildStealableTechList(eTargetPlayer);
			if (m_pPlayer->GetEspionage()->m_aaPlayerStealableTechList[eTargetPlayer].size() == 0)
				continue;
		}

		TeamTypes eTargetTeam = GET_PLAYER(eTargetPlayer).getTeam();
		CvDiplomacyAI* pTargetDiploAI = GET_PLAYER(eTargetPlayer).GetDiplomacyAI();

		int iDiploModifier = 0; // player-related diplo modifier
		for (int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes)iMinorLoop;
			if (eMinor != NO_PLAYER)
			{
				CvPlayer* pMinor = &GET_PLAYER(eMinor);
				if (pMinor && pMinor->isMinorCiv())
				{
					CvMinorCivAI* pMinorCivAI = pMinor->GetMinorCivAI();
					if (pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_SPY_ON_MAJOR))
					{
						if (pMinorCivAI->GetQuestData1(m_pPlayer->GetID(), MINOR_CIV_QUEST_SPY_ON_MAJOR) == eTargetPlayer)
						{
							iDiploModifier += 25;
						}
					}
				}
			}
		}

		if (GET_TEAM(eTeam).isAtWar(eTargetTeam) || pDiploAI->GetCivApproach(eTargetPlayer) == CIV_APPROACH_WAR)
		{
			// bonus targeting!
			iDiploModifier += 40;
		}
		else // we're not at war with them, so look at other factors
		{
			// if we've denounced them or they've denounced us, spy bonus!
			if (pDiploAI->IsDenouncedPlayer(eTargetPlayer) || pTargetDiploAI->IsDenouncedPlayer(ePlayer))
			{
				iDiploModifier += 33;
			}

			if (GET_TEAM(eTeam).IsHasResearchAgreement(eTargetTeam))
			{
				iDiploModifier -= 15;
			}

			if (pDiploAI->GetCivApproach(eTargetPlayer) >= CIV_APPROACH_AFRAID)
			{
				if (pDiploAI->GetCivApproach(eTargetPlayer) == CIV_APPROACH_FRIENDLY)
				{
					iDiploModifier -= 30;
				}
				else
				{
					iDiploModifier -= 15;
				}
			}
			if (pDiploAI->GetCivApproach(eTargetPlayer) <= CIV_APPROACH_GUARDED)
			{
				iDiploModifier += 15;
			}
		}
		int iCultureMod = m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eTargetPlayer);
		if (iCultureMod > 0 && pDiploAI->GetCivApproach(eTargetPlayer) != CIV_APPROACH_FRIENDLY)
		{
			iDiploModifier += iCultureMod * 15;
		}
		//Spread our spies out a bit.
		for (uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
		{
			if (!pEspionage->GetSpyByID(uiSpy)->m_bEvaluateReassignment)
			{
				CvCity* pCity = pEspionage->GetCityWithSpy(uiSpy);
				if (pCity)
				{
					if (pCity->getOwner() == eTargetPlayer)
					{
						iDiploModifier -= 15;
						break;
					}
				}
			}
		}

		ReligionTypes eReligion = m_pPlayer->GetReligions()->GetStateReligion();
		if (eReligion != NO_RELIGION)
		{
			CvGameReligions* pReligions = GC.getGame().GetGameReligions();
			const CvReligion* pMyReligion = pReligions->GetReligion(eReligion, m_pPlayer->GetID());

			CvCity* pHolyCity = pMyReligion->GetHolyCity();
			iDiploModifier += pMyReligion->m_Beliefs.GetHappinessFromForeignSpies(m_pPlayer->GetID(), pHolyCity, true) * 25;
		}
		iDiploModifier *= m_pPlayer->GetDiplomacyAI()->GetDiploBalance();
		iDiploModifier /= 5;

		int iLoop = 0;
		for (CvCity* pLoopCity = GET_PLAYER(eTargetPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(eTargetPlayer).nextCity(&iLoop))
		{
			CvPlot* pCityPlot = pLoopCity->plot();
			CvAssertMsg(pCityPlot, "City plot is null!");
			if (!pCityPlot || !pLoopCity->isRevealed(m_pPlayer->getTeam(),false,true))
				continue;

			ScoreCityEntry kEntry;
			kEntry.m_pCity = pLoopCity;
			kEntry.m_bDiplomat = false;

			if (!MOD_BALANCE_VP)
			{
				//hmm...sometimes we want more, sometimes we want less...
				int iValue = 0;
				iValue = pLoopCity->getEconomicValue(ePlayer) / 1000;

				//spy already here? don't discount!
				CvEspionageSpy* pSpy = pEspionage->GetSpyByID(pEspionage->GetSpyIndexInCity(pLoopCity));
				if (pSpy && pSpy->GetSpyState() == SPY_STATE_GATHERING_INTEL)
					iValue += 50;
				
				iValue *= 100 + iDiploModifier + (pLoopCity->isCapital() ? 20 : 0);
				iValue /= 100;

				if (iValue <= 0)
					continue;

				kEntry.m_iScore = iValue;
				kEntry.m_eMission = NO_EVENT_CHOICE_CITY;
			}
			else
			{
				int iValue = 0; // passed by reference
				CityEventChoiceTypes eBestMission = GetBestMissionInCity(pLoopCity, iValue, aSpyMissionList, pEspionage->GetSpyIndexInCity(pLoopCity));
				if (iValue <= 0)
					continue;

				// city security
				iValue *= (100 - pLoopCity->CalculateCitySecurity());
				iValue /= 100;

				// diplo modifier
				iValue *= 100 + iDiploModifier;
				iValue /= 100;

				kEntry.m_iScore = iValue;
				kEntry.m_eMission = eBestMission;
			}

			aCityScores.push_back(kEntry);
		}
	}
	return aCityScores;
}

std::vector<ScoreCityEntry> CvEspionageAI::BuildDefenseCityList()
{
	std::vector<ScoreCityEntry> aCityScores;
	PlayerTypes ePlayer = m_pPlayer->GetID();

	if (m_pPlayer->GetDiplomacyAI()->GetNumValidMajorCivs() <= 0)
		return aCityScores;

	bool bNoForeignSpies = true;
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

		if (GET_PLAYER(eTargetPlayer).GetEspionage()->GetNumSpies() > 0)
		{
			bNoForeignSpies = false;
			break;
		}
	}

	if(bNoForeignSpies)
		return aCityScores;

	// for performance reasons, make a list of all the possible counterspy missions
	std::vector<int> aCounterspyMissionList;
	if (MOD_BALANCE_VP)
	{
		for (int i = 0; i < GC.getNumCityEventChoiceInfos(); i++)
		{
			CvModEventCityChoiceInfo* pkMissionInfo = GC.getCityEventChoiceInfo((CityEventChoiceTypes)i);
			if (pkMissionInfo->isCounterspyMission())
			{
				aCounterspyMissionList.push_back(i);
			}
		}
	}
	
	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	for (pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if (!MOD_BALANCE_VP)
		{
			ScoreCityEntry kEntry;
			kEntry.m_pCity = pLoopCity;
			int iValue = 0;
			iValue = pLoopCity->getEconomicValue(ePlayer) / 1000;

			if (pLoopCity->isCapital())
			{
				iValue *= 110;
				iValue /= 100;
			}

			if (pLoopCity->getHappinessDelta() < 0)
			{
				iValue *= 110;
				iValue /= 100;
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

				iValue += pLoopCity->GetCityEspionage()->m_aiNumTimesCityRobbed[eTargetPlayer] * 2;
			}

			kEntry.m_iScore = iValue;

			if (kEntry.m_iScore <= 0)
				continue;

			kEntry.m_bDiplomat = false;
			kEntry.m_eMission = NO_EVENT_CHOICE_CITY;

			aCityScores.push_back(kEntry);
		}
		else
		{
			ScoreCityEntry kEntry;
			kEntry.m_pCity = pLoopCity;

			int iValue = 0; // passed by reference
			CityEventChoiceTypes eBestMission = GetBestMissionInCity(pLoopCity, iValue, aCounterspyMissionList, m_pPlayer->GetEspionage()->GetSpyIndexInCity(pLoopCity));
			kEntry.m_iScore = iValue;

			if (kEntry.m_iScore <= 0)
				continue;

			kEntry.m_bDiplomat = false;
			kEntry.m_eMission = eBestMission;

			aCityScores.push_back(kEntry);
		}
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
					if (pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_COUP))
					{
						if (pMinorCivAI->GetQuestData1(m_pPlayer->GetID(), MINOR_CIV_QUEST_COUP) == eTargetPlayer)
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

			if (pMinorCivAI->GetPermanentAlly() != NO_PLAYER)
				continue;

			if (pMinorCivAI->IsAllies(m_pPlayer->GetID()))
			{
				if (!MOD_BALANCE_VP && !pMinorCivAI->IsCloseToNotBeingAllies(m_pPlayer->GetID()))
					continue;
				if (MOD_BALANCE_VP)
				{
					// do we have much more influence than the competitors?
					int iInfluenceSecondBest = 0;
					for(int iLoopPlayer = 0; iLoopPlayer < MAX_MAJOR_CIVS; iLoopPlayer++)
					{
						PlayerTypes eLoopPlayer = (PlayerTypes)iLoopPlayer;
						if (eLoopPlayer == m_pPlayer->GetID())
							continue;

						iInfluenceSecondBest = max(iInfluenceSecondBest, pMinorCivAI->GetEffectiveFriendshipWithMajor(eLoopPlayer));
					}
					int iInfluenceDifference = pMinorCivAI->GetEffectiveFriendshipWithMajor(m_pPlayer->GetID()) - iInfluenceSecondBest;
					int iEra = max(1, (int)m_pPlayer->GetCurrentEra());
					if (iInfluenceDifference >= 150 * iEra)
						continue;
				}
			}

			// if we've already scored a city for this minor civ, we don't need to score another because the values would be the same
			if (bAlreadyScoredCity)
			{
				break;
			}
			bAlreadyScoredCity = true;

			ScoreCityEntry kEntry;
			kEntry.m_pCity = pLoopCity;

			int iValue = 0;
			if (!MOD_BALANCE_VP)
			{
				iValue = pEspionage->GetTheoreticalChanceOfCoup(pLoopCity, 0, true);
			}
			else
			{
				iValue = 20;
				iValue += min(60, 20 * pMinorCivAI->GetNumConsecutiveSuccessfulRiggings(m_pPlayer->GetID()));
			}
			switch (m_pPlayer->GetProximityToPlayer(eTargetPlayer))
			{
			case NO_PLAYER_PROXIMITY:
				UNREACHABLE(); // Since they have a city there should always be a proximity.
			case PLAYER_PROXIMITY_NEIGHBORS:
				iValue += 30;
				break;
			case PLAYER_PROXIMITY_CLOSE:
				iValue += 20;
				break;
			case PLAYER_PROXIMITY_FAR:
				iValue += 10;
				break;
			case PLAYER_PROXIMITY_DISTANT:
				iValue += 5;
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
			kEntry.m_eMission = NO_EVENT_CHOICE_CITY;
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
		if (pSpy->GetSpyState() == SPY_STATE_DEAD || pSpy->GetSpyState() == SPY_STATE_TERMINATED)
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
		if (pSpy->GetSpyState() == SPY_STATE_DEAD || pSpy->GetSpyState() == SPY_STATE_TERMINATED || pSpy->GetSpyState() == SPY_STATE_IMPRISONED || pEspionage->GetNumTurnsSpyMovementBlocked(ui) > 0)
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
		if (pSpy->GetSpyState() == SPY_STATE_DEAD || pSpy->GetSpyState() == SPY_STATE_TERMINATED || pSpy->GetSpyState() == SPY_STATE_IMPRISONED || pEspionage->GetNumTurnsSpyMovementBlocked(ui) > 0)
		{
			continue;
		}

		CvCity* pCity = pEspionage->GetCityWithSpy(ui);
		if (pCity && pCity->getOwner() == m_pPlayer->GetID())
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

// does not move the spies, only flags them to be moved
void CvEspionageAI::EvaluateDiplomatSpies(void)
{
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	for (uint ui = 0; ui < pEspionage->m_aSpyList.size(); ui++)
	{
		CvEspionageSpy* pSpy = &(pEspionage->m_aSpyList[ui]);
		// don't process dead spies
		if (pSpy->GetSpyState() == SPY_STATE_DEAD || pSpy->GetSpyState() == SPY_STATE_TERMINATED || pSpy->GetSpyState() == SPY_STATE_IMPRISONED || pEspionage->GetNumTurnsSpyMovementBlocked(ui) > 0)
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


// does not move the spies, only flags them to be moved
void CvEspionageAI::EvaluateMinorCivSpies(void)
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();
	for (uint ui = 0; ui < pEspionage->m_aSpyList.size(); ui++)
	{
		CvCity* pCity = pEspionage->GetCityWithSpy(ui);
		if (pCity && GET_PLAYER(pCity->getOwner()).isMinorCiv())
		{
			// if at war, reevaluate
			if (GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(pCity->getOwner()).getTeam()))
			{
				CvEspionageSpy* pSpy = &(pEspionage->m_aSpyList[ui]);
				pSpy->m_bEvaluateReassignment = true;
				if (GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Re-eval: minor civ at war, %d,", ui);
					strMsg += GetLocalizedText(pCity->getNameKey());
					pEspionage->LogEspionageMsg(strMsg);
				}
			}
		}
	}
}

// serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvEspionageAI& writeTo)
{
	uint uiVersion = 0;
	loadFrom >> uiVersion;
	MOD_SERIALIZE_INIT_READ(loadFrom);

	uint uiCount = 0;
	loadFrom >> uiCount;
	int iValue = 0;
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
