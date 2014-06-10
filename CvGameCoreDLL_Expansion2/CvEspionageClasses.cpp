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

#include "LintFree.h"

// consts to put in enums
const int iSpyTurnsToTravel = 1;
const int iSpyTurnsToRevive = 5;
const int iSpyTurnsToMakeIntroductions = 5;
const int iIntrigueTurnsValid = 5;
PlayerTypes g_eSortPlayer = NO_PLAYER; // global - used for the sort

//=====================================
// CvEspionageSpy
//=====================================
/// Default Constructor
CvEspionageSpy::CvEspionageSpy()
	: m_iName(-1)
	, m_eRank(NUM_SPY_RANKS)
	, m_iCityX(-1)
	, m_iCityY(-1)
	, m_eSpyState(NUM_SPY_STATES)
	, m_iReviveCounter(0)
	, m_bIsDiplomat(false)
	, m_bEvaluateReassignment(true)
{
}

/// Serialization read
FDataStream& operator>>(FDataStream& loadFrom, CvEspionageSpy& writeTo)
{
	uint uiVersion;
	loadFrom >> uiVersion;

	loadFrom >> writeTo.m_iName;
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

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvEspionageSpy& readFrom)
{
	uint uiVersion = 1;
	saveTo << uiVersion;

	saveTo << readFrom.m_iName;
	saveTo << (int)readFrom.m_eRank;
	saveTo << readFrom.m_iCityX;
	saveTo << readFrom.m_iCityY;
	saveTo << (int)readFrom.m_eSpyState;
	saveTo << readFrom.m_iReviveCounter;
	saveTo << readFrom.m_bIsDiplomat;
	saveTo << readFrom.m_bEvaluateReassignment;

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

	for(uint ui = 0; ui < m_aiSpyListNameOrder.size(); ui++)
	{
		uint uiTempValue;
		uint uiTargetSlot = GC.getGame().getJonRandNum(m_aiSpyListNameOrder.size(), "Randomizing m_aiSpyListNameOrder list");
		uiTempValue = m_aiSpyListNameOrder[ui];
		m_aiSpyListNameOrder[ui] = m_aiSpyListNameOrder[uiTargetSlot];
		m_aiSpyListNameOrder[uiTargetSlot] = uiTempValue;
	}
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
	m_aIntrigueNotificationMessages.clear();
	m_aaPlayerStealableTechList.clear();
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		m_aiMaxTechCost[ui] = -1;
		m_aHeistLocations[ui].clear();
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
	kNewSpy.m_iName = GetNextSpyName();
	kNewSpy.m_bEvaluateReassignment = true;

	m_aSpyList.push_back(kNewSpy);

	CvNotifications* pNotifications = m_pPlayer->GetNotifications();
	if(pNotifications)
	{
		const char* szSpyName = m_pPlayer->getCivilizationInfo().getSpyNames(kNewSpy.m_iName);
		CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_CREATED", szSpyName);
		CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_SPY_CREATED", szSpyName);
		pNotifications->Add(NOTIFICATION_SPY_CREATED_ACTIVE_PLAYER, strBuffer, strSummary, -1, -1, 0);
	}

	if(GC.getLogging())
	{
		CvString strMsg;
		strMsg.Format("New Spy, %d,", m_aSpyList.size() - 1);
		strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(kNewSpy.m_iName));
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
				pSpy->m_eSpyState = SPY_STATE_MAKING_INTRODUCTIONS;
				int iRate = CalcPerTurn(SPY_STATE_MAKING_INTRODUCTIONS, pCity, uiSpyIndex);
				int iGoal = CalcRequired(SPY_STATE_MAKING_INTRODUCTIONS, pCity, uiSpyIndex);
				pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);				
			}
			else if(pCity->getTeam() == m_pPlayer->getTeam())  // moved to a friendly city
			{
				// moving to a defensive location
				pSpy->m_eSpyState = SPY_STATE_COUNTER_INTEL;
			}
			else // moved to an opponent's city
			{
				pSpy->m_eSpyState = SPY_STATE_SURVEILLANCE;
				int iRate = CalcPerTurn(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
				int iGoal = CalcRequired(SPY_STATE_SURVEILLANCE, pCity, uiSpyIndex);
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
				pSpy->m_eSpyState = SPY_STATE_RIG_ELECTION;
				pCityEspionage->ResetProgress(ePlayer);
				int iRate = CalcPerTurn(SPY_STATE_RIG_ELECTION, pCity, uiSpyIndex);
				int iGoal = CalcRequired(SPY_STATE_RIG_ELECTION, pCity, uiSpyIndex);
				pCityEspionage->SetActivity(ePlayer, 0, iRate, iGoal);
				pCityEspionage->SetLastProgress(ePlayer, iRate);
			}
			else
			{
				BuildStealableTechList(eCityOwner);
				// moved rate out here to set the potential
				int iBasePotentialRate = CalcPerTurn(SPY_STATE_GATHERING_INTEL, pCity, -1);
				pCityEspionage->SetLastBasePotential(ePlayer, iBasePotentialRate);

				if(m_aaPlayerStealableTechList[eCityOwner].size() > 0)
				{
					// TODO: need to proclaim surveillance somehow
					pSpy->m_eSpyState = SPY_STATE_GATHERING_INTEL;
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
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH_S");
							strSummary << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
							Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_CANT_STEAL_TECH");
							strNotification << GetSpyRankName(pSpy->m_eRank);;
							strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
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
						strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
						LogEspionageMsg(strMsg);
					}

				}
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
		pCityEspionage->Process(ePlayer);
		// if the rate is too low, reassign the spy
		if (pCityEspionage->m_aiRate[m_pPlayer->GetID()] < 100)
		{
			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: potential too low, %d,", uiSpyIndex);
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
				LogEspionageMsg(strMsg);
			}
		}

		if(m_aaPlayerStealableTechList[eCityOwner].size() == 0)
		{
			// set the spy back to surveillance mode
			pCityEspionage->ResetProgress(ePlayer);
			pSpy->m_eSpyState = SPY_STATE_SURVEILLANCE;
			pSpy->m_bEvaluateReassignment = true; // flag for reassignment
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: m_aaPlayerStealableTechList[eCityOwner].size() == 0, %d,", uiSpyIndex);
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
				LogEspionageMsg(strMsg);
			}

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
				strNotification << GetSpyRankName(pSpy->m_eRank);;
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
				strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
				pNotifications->Add(NOTIFICATION_SPY_CANT_STEAL_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}
		}
		else if(pCityEspionage->HasReachedGoal(ePlayer))
		{
			HeistLocation kHeistLocation;
			kHeistLocation.m_iX = pCity->getX();
			kHeistLocation.m_iY = pCity->getY();
			m_aHeistLocations[eCityOwner].push_back(kHeistLocation);

			int iSpyResult;
			if(pCityEspionage->HasCounterSpy())
			{
				iSpyResult = GC.getGame().getJonRandNum(300, "Random roll for the result of a spy mission with a counterspy in the city");
				int iCounterspyIndex = GET_PLAYER(eCityOwner).GetEspionage()->GetSpyIndexInCity(pCity);
				iSpyResult += GET_PLAYER(eCityOwner).GetEspionage()->m_aSpyList[iCounterspyIndex].m_eRank * 30;
				iSpyResult *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
				iSpyResult /= 100;
				if(iSpyResult < 100)
				{
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_DETECTED);
				}
				else if(iSpyResult < 200)
				{
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_IDENTIFIED);
				}
				else
				{
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_KILLED);

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
				}
			}
			else
			{
				iSpyResult = GC.getGame().getJonRandNum(300, "Random roll for the result of a spying mission without a counterspy in the city");
				iSpyResult *= (100 + GET_PLAYER(pCity->getOwner()).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_CATCH_SPIES_MODIFIER));
				iSpyResult /= 100;
				if(iSpyResult < 100)
				{
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_UNDETECTED);
				}
				else if(iSpyResult < 200)
				{
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_DETECTED);
				}
				else
				{
					pCityEspionage->SetSpyResult(ePlayer, SPY_RESULT_IDENTIFIED);
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
					strSummary << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_WAS_KILLED");
					strNotification << GetSpyRankName(pSpy->m_eRank);
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
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

				// kill spy off
				ExtractSpyFromCity(uiSpyIndex); // move the dead body out so that someone else can move in
				pSpy->m_eSpyState = SPY_STATE_DEAD; // have to official kill him after the extraction

				if(GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Killed, %d,", uiSpyIndex);
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
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
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
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
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
						strNotification << pCity->getNameKey();
						strNotification << GET_PLAYER(eCityOwner).getCivilizationInfo().getShortDescriptionKey();
						pNotifications->Add(NOTIFICATION_SPY_STOLE_TECH, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, eCityOwner);
					}
				}
				else
				{
					m_aiNumTechsToStealList[iCityOwner] = 0;
				}

				//Achievements!
				if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
				{
					gDLL->UnlockAchievement(ACHIEVEMENT_XP1_12);
				}

				LevelUpSpy(uiSpyIndex);

				if(GC.getLogging())
				{
					CvString strMsg;
					strMsg.Format("Stealing tech, %d,", uiSpyIndex);
					strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
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
			if(pSpy->m_eSpyState != SPY_STATE_DEAD)
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
			pSpy->m_eSpyState = SPY_STATE_SCHMOOZE;
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
			pSpy->m_iName = GetNextSpyName();
			pSpy->m_eRank = (CvSpyRank)m_pPlayer->GetStartingSpyRank();
			pSpy->m_eSpyState = SPY_STATE_UNASSIGNED;
			pSpy->m_iCityX = -1;
			pSpy->m_iCityY = -1;
			pSpy->m_iReviveCounter = 0;
			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: spy killed, %d,", uiSpyIndex);
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
				LogEspionageMsg(strMsg);
			}

			CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			if(pNotifications)
			{
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_REVIVED_S");
				Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_SPY_REVIVED");
				strNotification << GetSpyRankName(pSpy->m_eRank);
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName);
				pNotifications->Add(NOTIFICATION_SPY_REPLACEMENT, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, -1);
			}

			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Respawned spy, %d,", uiSpyIndex);
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
				LogEspionageMsg(strMsg);
			}
		}
		break;
	}

	// if we just established surveillance in the city, turn the lights on
	if(HasEstablishedSurveillance(uiSpyIndex) && !bHadSurveillance)
	{
		pCity->plot()->changeAdjacentSight(m_pPlayer->getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), true, NO_INVISIBLE, NO_DIRECTION, false);
	}
}

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
		int iSpyRank = m_aSpyList[uiSpyIndex].m_eRank;
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
		case AI_OPERATION_SNEAK_CITY_ATTACK:
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, eRevealedTargetPlayer, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_ARMY_SNEAK_ATTACK, uiSpyIndex, pTargetCity, true);
		}
		break;
		case AI_OPERATION_NAVAL_SNEAK_ATTACK:
		{
			AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, eRevealedTargetPlayer, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_AMPHIBIOUS_SNEAK_ATTACK, uiSpyIndex, pTargetCity, true);
		}
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
				break;
			case ARMY_TYPE_NAVAL_INVASION:
				AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_BUILDING_AMPHIBIOUS_ARMY, uiSpyIndex, pCity, true);
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
				}
				else
				{
					AddIntrigueMessage(m_pPlayer->GetID(), eCityOwner, NO_PLAYER, NO_BUILDING, NO_PROJECT, INTRIGUE_TYPE_DECEPTION, uiSpyIndex, pCity, true);
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
	}
}

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

	if(!CanMoveSpyTo(pCity, uiSpyIndex, bAsDiplomat))
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
		m_aSpyList[uiSpyIndex].m_eSpyState = SPY_STATE_TRAVELLING;
		m_aSpyList[uiSpyIndex].m_bIsDiplomat = bAsDiplomat;
		int iRate = CalcPerTurn(SPY_STATE_TRAVELLING, pCity, uiSpyIndex);
		int iGoal = CalcRequired(SPY_STATE_TRAVELLING, pCity, uiSpyIndex);
		pCityEspionage->SetActivity(m_pPlayer->GetID(), 0, iRate, iGoal);
	}

	if(GC.getLogging())
	{
		CvString strMsg;
		strMsg.Format("Moving spy, %d,", uiSpyIndex);
		strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
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
	m_aSpyList[uiSpyIndex].m_eSpyState = SPY_STATE_UNASSIGNED;

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
		pCity->plot()->changeAdjacentSight(m_pPlayer->getTeam(), GC.getESPIONAGE_SURVEILLANCE_SIGHT_RANGE(), false, NO_INVISIBLE, NO_DIRECTION, false);
	}

	pCity->GetCityEspionage()->m_aiSpyAssignment[m_pPlayer->GetID()] = -1;
	pCity->GetCityEspionage()->ResetProgress(m_pPlayer->GetID());

	return true;
}

/// LevelUpSpy - Move spy to next experience level
void CvPlayerEspionage::LevelUpSpy(uint uiSpyIndex)
{
	// if the spy can level up and it's not dead
	if(m_aSpyList[uiSpyIndex].m_eRank < NUM_SPY_RANKS - 1 && m_aSpyList[uiSpyIndex].m_eSpyState != SPY_STATE_DEAD)
	{
		CvSpyRank eOriginalRank = m_aSpyList[uiSpyIndex].m_eRank;

		// announce promotion through notification
		m_aSpyList[uiSpyIndex].m_eRank = (CvSpyRank)(m_aSpyList[uiSpyIndex].m_eRank + 1);

		CvNotifications* pNotifications = m_pPlayer->GetNotifications();
		if(pNotifications)
		{
			const char* szSpyName = m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
			const char* szOldPromotion = GetSpyRankName(eOriginalRank);
			const char* szNewPromotion = GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
			CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_SPY_PROMOTED", szSpyName, szOldPromotion, szNewPromotion);
			CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_SPY_PROMOTED", szSpyName);
			pNotifications->Add(NOTIFICATION_SPY_PROMOTION, strBuffer, strSummary, -1, -1, 0);
		}
	}
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
			int iBaseYieldRate = pCity->getYieldRateTimes100(YIELD_SCIENCE, false);
			iBaseYieldRate *= GC.getESPIONAGE_GATHERING_INTEL_RATE_BASE_PERCENT();
			iBaseYieldRate *= GC.getGame().getGameSpeedInfo().getSpyRatePercent();
			iBaseYieldRate /= 10000;
			int iCityEspionageModifier = pCity->GetEspionageModifier();
			int iPlayerEspionageModifier = GET_PLAYER(eCityOwner).GetEspionageModifier();
			int iTheirPoliciesEspionageModifier = GET_PLAYER(eCityOwner).GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_SLOWER_MODIFIER);
			int iMyPoliciesEspionageModifier = m_pPlayer->GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER);
			int iFinalModifier = (iBaseYieldRate * (100 + iCityEspionageModifier + iPlayerEspionageModifier + iTheirPoliciesEspionageModifier + iMyPoliciesEspionageModifier)) / 100;

			int iResult = max(iFinalModifier, 1);
			if(iSpyIndex >= 0)
			{
				int iSpyRank = m_aSpyList[iSpyIndex].m_eRank;
				iSpyRank += m_pPlayer->GetCulture()->GetInfluenceMajorCivSpyRankBonus(eCityOwner);
				iResult *= 100 + (GC.getESPIONAGE_GATHERING_INTEL_RATE_BY_SPY_RANK_PERCENT() * iSpyRank);
				iResult /= 100;
			}

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
		return iSpyTurnsToTravel;
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
			int iMaxTechCostAdjusted = uiMaxTechCostAdjusted;
			CvAssertMsg(m_aiMaxTechCost[ePlayer] >= 0, "iMaxTechCostAdjusted is below zero. Overflow!");
			return iMaxTechCostAdjusted;
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
	else if(m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_GATHERING_INTEL || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_RIG_ELECTION || m_aSpyList[uiSpyIndex].m_eSpyState == SPY_STATE_SCHMOOZE)
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

	if(eMinorCivAlly != NO_PLAYER && eMinorCivAlly != m_pPlayer->GetID())
	{
		return true;
	}

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

	if(iResultPercentage > 85)
	{
		iResultPercentage = 85;
	}
	else if(iResultPercentage < 0)
	{
		iResultPercentage = 0;
	}

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
		strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName));
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
		m_aSpyList[uiSpyIndex].m_eSpyState = SPY_STATE_DEAD; // have to official kill him after the extraction
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
			strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
			strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
			strNotification << pCity->getNameKey();
			strNotification << GET_PLAYER(ePreviousAlly).getCivilizationAdjectiveKey();
		}
		pNotifications->Add(eNotification, strNotification.toUTF8(), strSummary.toUTF8(), pCity->getX(), pCity->getY(), -1);
	}

	//Achievements!
	if(bAttemptSuccess && m_pPlayer->GetID() == GC.getGame().getActivePlayer())
	{
		gDLL->UnlockAchievement(ACHIEVEMENT_XP1_13);
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
		if(m_aSpyList[ui].m_eSpyState != SPY_STATE_DEAD)
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
		if(m_aSpyList[ui].m_eSpyState == SPY_STATE_DEAD)
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

bool CvPlayerEspionage::IsMyDiplomatVisitingThem(PlayerTypes ePlayer, bool bIncludeTravelling)
{
	CvCity* pTheirCapital = GET_PLAYER(ePlayer).getCapitalCity();
	if (!pTheirCapital)
	{
		return false;
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
void CvPlayerEspionage::AddSpyMessage(int iCityX, int iCityY, PlayerTypes eAttackingPlayer, int iSpyResult, TechTypes eStolenTech)
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
				Localization::String strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_TECH_STOLEN_SPY_DETECTED_WO_TECH_S");;
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
					Localization::String strNotification = Localization::Lookup("TXT_KEY_NOTIFICATION_KILLED_A_SPY");
					strNotification << GetSpyRankName(m_aSpyList[iDefendingSpy].m_eRank);
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[iDefendingSpy].m_iName);
					strNotification << GET_PLAYER(m_aSpyNotificationMessages[ui].m_eAttackingPlayer).getCivilizationAdjectiveKey();
					strNotification << pCity->getNameKey();

					pNotifications->Add(NOTIFICATION_SPY_KILLED_A_SPY, strNotification.toUTF8(), strSummary.toUTF8(), -1, -1, m_aSpyNotificationMessages[ui].m_eAttackingPlayer);
				
					//Achievements
					if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
					{
						gDLL->UnlockAchievement(ACHIEVEMENT_XP1_15);
					}
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
	if (eDiscoveringPlayer == m_pPlayer->GetID())
	{
		Localization::String str = Localization::Lookup("TXT_KEY_SPY_FULL_NAME");
		str << GetSpyRankName(m_aSpyList[uiSpyIndex].m_eRank);
		str << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
						strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
					strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
				strNotification << m_pPlayer->getCivilizationInfo().getSpyNames(m_aSpyList[uiSpyIndex].m_iName);
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
		writeTo.m_aIntrigueNotificationMessages.push_back(kMessage);
	}

	return loadFrom;
}

/// Serialization write
FDataStream& operator<<(FDataStream& saveTo, const CvPlayerEspionage& readFrom)
{
	uint uiVersion = 0;
	saveTo << uiVersion;

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
void CvCityEspionage::SetSpyResult(PlayerTypes eSpyOwner, int iResult)
{
	m_aiResult[eSpyOwner] = iResult;
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

	AI_PERF_FORMAT("AI-perf.csv", ("Espionage AI, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	StealTechnology();
	UpdateCivOutOfTechTurn();
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
			if (pSpy->m_eSpyState == SPY_STATE_DEAD)
			{
				continue;
			}

			pSpy->m_bEvaluateReassignment = true;
			if(GC.getLogging())
			{
				CvString strMsg;
				strMsg.Format("Re-eval: UN constructed/reassign, %d,", ui);
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
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
		if(pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD)
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
				pEspionage->MoveSpyTo(NULL, uiSpy, false);
			}
		}
	}

	// go through spy list again and put spies in locations where they are needed
	for(uint uiSpy = 0; uiSpy < pEspionage->m_aSpyList.size(); uiSpy++)
	{
		// dead spies are not processed
		if(pEspionage->m_aSpyList[uiSpy].m_eSpyState == SPY_STATE_DEAD)
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

				pEspionage->MoveSpyTo(pCity, uiSpy, true);
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
		else if(iTargetOffensiveSpies > iCorrectlyAssignedOffenseSpies)
		{
			for(int iCity = 0; iCity < iTargetOffensiveSpies && iCity < (int)apCityOffense.size(); iCity++)
			{
				CvCity* pCity = apCityOffense[iCity];

				// if a spy is already in this city, skip it
				if(pEspionage->GetSpyIndexInCity(pCity) != -1)
				{
					continue;
				}

				pEspionage->MoveSpyTo(pCity, uiSpy, false);
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
							pDefendingPlayerEspionage->AddSpyMessage(pCity->getX(), pCity->getY(), m_pPlayer->GetID(), pCityEspionage->m_aiResult[m_pPlayer->GetID()], eStolenTech);
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

	// if going for diplo or conquest, evaluate the offensive spies before assigning CS spies
	if (pDiploAI->IsGoingForDiploVictory())
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
		int iNumDiplomats = min(iNumRemainingSpies, 2);
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
			if (pCulture->WantsDiplomatDoingPropaganda(eTargetPlayer))
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
}

void CvEspionageAI::BuildDefenseCityList(EspionageCityList& aDefenseCityList)
{
	CvPlayerEspionage* pEspionage = m_pPlayer->GetEspionage();

	aDefenseCityList.clear();
	PlayerTypes ePlayer = m_pPlayer->GetID();

	std::vector<ScoreCityEntry> aCityScores;

	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		ScoreCityEntry kEntry;
		kEntry.m_pCity = pLoopCity;
		kEntry.m_iScore = pEspionage->CalcPerTurn(SPY_STATE_GATHERING_INTEL, pLoopCity, -1);
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
		if (pSpy->m_eSpyState == SPY_STATE_DEAD)
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
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
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
		if (pSpy->m_eSpyState == SPY_STATE_DEAD)
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
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
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
		if (pSpy->m_eSpyState == SPY_STATE_DEAD)
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
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
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
				strMsg += GetLocalizedText(m_pPlayer->getCivilizationInfo().getSpyNames(pSpy->m_iName));
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