/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#include "CvGameCoreDLLPCH.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvMinorCivAI.h"
#include "CvDiplomacyAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvGrandStrategyAI.h"
#include "CvCitySpecializationAI.h"
#include "Fireworks/FVariableSystem.h"
#include "cvStopWatch.h"

// must be included after all other headers
#include "LintFree.h"

CvMilitaryAIStrategyXMLEntry::CvMilitaryAIStrategyXMLEntry(void):
	m_piPlayerFlavorValue(NULL),
	m_piCityFlavorValue(NULL),
	m_iWeightThreshold(0),
	m_piPersonalityFlavorThresholdMod(NULL),
	m_bNoMinorCivs(false),
	m_bOnlyMinorCivs(false),
	m_bUpdateCitySpecializations(false),
	m_iTechPrereq(NO_TECH),
	m_iTechObsolete(NO_TECH),
	m_iMinimumNumTurnsExecuted(0),
	m_iCheckTriggerTurnCount(0),
	m_iFirstTurnExecuted(0),
	m_eAdvisor(NO_ADVISOR_TYPE),
	m_iAdvisorCounselImportance(1)
{
}
//------------------------------------------------------------------------------
CvMilitaryAIStrategyXMLEntry::~CvMilitaryAIStrategyXMLEntry(void)
{
	SAFE_DELETE_ARRAY(m_piPlayerFlavorValue);
	SAFE_DELETE_ARRAY(m_piCityFlavorValue);
	SAFE_DELETE_ARRAY(m_piPersonalityFlavorThresholdMod);
}
//------------------------------------------------------------------------------
bool CvMilitaryAIStrategyXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_bNoMinorCivs				 = kResults.GetBool("NoMinorCivs");
	m_bOnlyMinorCivs		     = kResults.GetBool("OnlyMinorCivs");
	m_bUpdateCitySpecializations = kResults.GetBool("UpdateCitySpecializations");
	m_iCheckTriggerTurnCount	 = kResults.GetInt("CheckTriggerTurnCount");
	m_iFirstTurnExecuted            = kResults.GetInt("FirstTurnExecuted");
	m_iMinimumNumTurnsExecuted	 = kResults.GetInt("MinimumNumTurnsExecuted");
	m_iWeightThreshold			 = kResults.GetInt("WeightThreshold");

	const char* strAdvisor = kResults.GetText("Advisor");
	m_eAdvisor = NO_ADVISOR_TYPE;
	if(strAdvisor)
	{
		if(strcmp(strAdvisor, "MILITARY") == 0)
		{
			m_eAdvisor = ADVISOR_MILITARY;
		}
		else if(strcmp(strAdvisor, "ECONOMIC") == 0)
		{
			m_eAdvisor = ADVISOR_ECONOMIC;
		}
		else if(strcmp(strAdvisor, "FOREIGN") == 0)
		{
			m_eAdvisor = ADVISOR_FOREIGN;
		}
		else if(strcmp(strAdvisor, "SCIENCE") == 0)
		{
			m_eAdvisor = ADVISOR_SCIENCE;
		}
	}

	m_strAdvisorCounselText = kResults.GetText("AdvisorCounsel");

	//References
	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("TechObsolete");
	m_iTechObsolete = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTextVal, true);

	//Arrays
	const char* szType = GetType();
	kUtility.SetFlavors(m_piPlayerFlavorValue, "AIMilitaryStrategy_Player_Flavors",
	                    "AIMilitaryStrategyType", szType);
	kUtility.SetFlavors(m_piCityFlavorValue, "AIMilitaryStrategy_City_Flavors",
		"AIMilitaryStrategyType", szType);


	kUtility.SetFlavors(m_piPersonalityFlavorThresholdMod,
	                    "AIMilitaryStrategy_PersonalityFlavorThresholdMods",
	                    "AIMilitaryStrategyType", szType);

	return true;
}

/// What player flavors will be added by adopting this Strategy?
int CvMilitaryAIStrategyXMLEntry::GetPlayerFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPlayerFlavorValue ? m_piPlayerFlavorValue[i] : -1;
}

/// What city flavors will be added by adopting this Strategy?
int CvMilitaryAIStrategyXMLEntry::GetCityFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCityFlavorValue ? m_piCityFlavorValue[i] : -1;
}

/// The amount of weight a Strategy must accumulate to be adopted (if applicable)
int CvMilitaryAIStrategyXMLEntry::GetWeightThreshold() const
{
	return m_iWeightThreshold;
}

/// How do a player's Personality Flavors affect the Threshold for adopting a Strategy? (if applicable)
int CvMilitaryAIStrategyXMLEntry::GetPersonalityFlavorThresholdMod(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPersonalityFlavorThresholdMod ? m_piPersonalityFlavorThresholdMod[i] : -1;
}

/// Is this a valid strategy for Minor Civs to use?
bool CvMilitaryAIStrategyXMLEntry::IsNoMinorCivs() const
{
	return m_bNoMinorCivs;
}

/// Are ONLY Minor Civs allowed to use this?
bool CvMilitaryAIStrategyXMLEntry::IsOnlyMinorCivs() const
{
	return m_bOnlyMinorCivs;
}

/// Should we reprioritize city specializations when this strategy goes on or off
bool CvMilitaryAIStrategyXMLEntry::RequiresCitySpecializationUpdate() const
{
	return m_bUpdateCitySpecializations;
}

/// Technology prerequisite an AI Player must have to enable this Strategy
int CvMilitaryAIStrategyXMLEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}

/// Technology that obsoletes this Strategy for an AI Player
int CvMilitaryAIStrategyXMLEntry::GetTechObsolete() const
{
	return m_iTechObsolete;
}

/// Minimum # of turns this AI Strategy must be executed for - don't want the AI starting then stopping Strategies every turn
int CvMilitaryAIStrategyXMLEntry::GetMinimumNumTurnsExecuted() const
{
	return m_iMinimumNumTurnsExecuted;
}

/// Once this Strategy has been adopted, check the Trigger conditions every X turns to see if it's maintained
int CvMilitaryAIStrategyXMLEntry::GetCheckTriggerTurnCount() const
{
	return m_iCheckTriggerTurnCount;
}

/// First turn to check for adoption
int CvMilitaryAIStrategyXMLEntry::GetFirstTurnExecuted() const
{
	return m_iFirstTurnExecuted;
}

AdvisorTypes CvMilitaryAIStrategyXMLEntry::GetAdvisor() const
{
	return m_eAdvisor;
}

const char* CvMilitaryAIStrategyXMLEntry::GetAdvisorCounselText() const
{
	return m_strAdvisorCounselText.c_str();
}

int CvMilitaryAIStrategyXMLEntry::GetAdvisorCounselImportance() const
{
	return m_iAdvisorCounselImportance;
}

//=====================================
// CvMilitaryAIStrategyXMLEntries
//=====================================
/// Constructor
CvMilitaryAIStrategyXMLEntries::CvMilitaryAIStrategyXMLEntries(void)
{

}

/// Destructor
CvMilitaryAIStrategyXMLEntries::~CvMilitaryAIStrategyXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of AIStrategy entries
std::vector<CvMilitaryAIStrategyXMLEntry*>& CvMilitaryAIStrategyXMLEntries::GetMilitaryAIStrategyEntries()
{
	return m_paAIStrategyEntries;
}

/// Number of defined AIStrategies
int CvMilitaryAIStrategyXMLEntries::GetNumMilitaryAIStrategies()
{
	return m_paAIStrategyEntries.size();
}

/// Clear AIStrategy entries
void CvMilitaryAIStrategyXMLEntries::DeleteArray()
{
	for(std::vector<CvMilitaryAIStrategyXMLEntry*>::iterator it = m_paAIStrategyEntries.begin(); it != m_paAIStrategyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAIStrategyEntries.clear();
}

/// Get a specific entry
_Ret_maybenull_ CvMilitaryAIStrategyXMLEntry* CvMilitaryAIStrategyXMLEntries::GetEntry(int index)
{
	return m_paAIStrategyEntries[index];
}


//=====================================
// CvMilitaryAI
//=====================================

/// Constructor
CvMilitaryAI::CvMilitaryAI():
	m_pabUsingStrategy(NULL),
	m_paiTurnStrategyAdopted(NULL),
	m_aiTempFlavors(NULL),
	m_paeLastTurnWarState(NULL),
	m_paeLastTurnMilitaryThreat(NULL),
	m_paeLastTurnMilitaryStrength(NULL),
	m_paeLastTurnTargetValue(NULL),
	m_iTotalThreatWeight(1),
	m_iNumberOfTimesOpsBuildSkippedOver(0),
	m_eArmyTypeBeingBuilt(NO_ARMY_TYPE)
{
}

/// Destructor
CvMilitaryAI::~CvMilitaryAI(void)
{
	Uninit();
}

/// Initialize
void CvMilitaryAI::Init(CvMilitaryAIStrategyXMLEntries* pAIStrategies, CvPlayer* pPlayer, CvDiplomacyAI* pDiplomacyAI)
{
	// Store off the pointer to the AIStrategies active for this game
	m_pAIStrategies = pAIStrategies;
	m_pPlayer = pPlayer;
	m_pDiplomacyAI = pDiplomacyAI;

	// Initialize arrays
	CvAssertMsg(m_pabUsingStrategy==NULL, "about to leak memory, CvMilitaryAI::m_pabUsingStrategy");
	m_pabUsingStrategy = FNEW(bool[m_pAIStrategies->GetNumMilitaryAIStrategies()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiTurnStrategyAdopted==NULL, "about to leak memory, CvMilitaryAI::m_paiTurnStrategyAdopted");
	m_paiTurnStrategyAdopted = FNEW(int[m_pAIStrategies->GetNumMilitaryAIStrategies()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_aiTempFlavors==NULL, "about to leak memory, CvMilitaryAI::m_aiTempFlavors");
	m_aiTempFlavors = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paeLastTurnWarState==NULL, "about to leak memory, CvMilitaryAI::m_paeLastTurnWarState");
	m_paeLastTurnWarState = FNEW(int[MAX_CIV_PLAYERS], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paeLastTurnMilitaryThreat==NULL, "about to leak memory, CvMilitaryAI::m_paeLastTurnMilitaryThreat");
	m_paeLastTurnMilitaryThreat = FNEW(int[MAX_CIV_PLAYERS], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paeLastTurnMilitaryStrength==NULL, "about to leak memory, CvMilitaryAI::m_paeLastTurnMilitaryStrength");
	m_paeLastTurnMilitaryStrength = FNEW(int[MAX_CIV_PLAYERS], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paeLastTurnTargetValue==NULL, "about to leak memory, CvMilitaryAI::m_paeLastTurnTargetValue");
	m_paeLastTurnTargetValue = FNEW(int[MAX_CIV_PLAYERS], c_eCiv5GameplayDLL, 0);

	Reset();
}

/// Deallocate memory created in initialize
void CvMilitaryAI::Uninit()
{
	SAFE_DELETE_ARRAY(m_pabUsingStrategy);
	SAFE_DELETE_ARRAY(m_paiTurnStrategyAdopted);
	SAFE_DELETE_ARRAY(m_aiTempFlavors);
	SAFE_DELETE_ARRAY(m_paeLastTurnWarState);
	SAFE_DELETE_ARRAY(m_paeLastTurnMilitaryThreat);
	SAFE_DELETE_ARRAY(m_paeLastTurnMilitaryStrength);
	SAFE_DELETE_ARRAY(m_paeLastTurnTargetValue);
}

/// Reset AIStrategy status array to all false
void CvMilitaryAI::Reset()
{
	int iI;

	m_iTotalThreatWeight = 1;  // Don't ever assume there is no threat at all
	m_eArmyTypeBeingBuilt = NO_ARMY_TYPE;

	m_iNumLandUnits = 0;
	m_iNumRangedLandUnits = 0;
	m_iNumMobileLandUnits = 0;
	m_iNumAirUnits = 0;
	m_iNumAntiAirUnits = 0;
	m_iNumMeleeLandUnits = 0;
	m_iNumNavalUnits = 0;
	m_iNumLandUnitsInArmies = 0;
	m_iNumNavalUnitsInArmies = 0;
	m_iBarbarianCampCount = 0;
	m_iVisibleBarbarianCount = 0;
	m_iRecommendedMilitarySize = 0;
	m_iMandatoryReserveSize = 0;
	m_eLandDefenseState = NO_DEFENSE_STATE;
	m_eNavalDefenseState = NO_DEFENSE_STATE;
	m_iNumberOfTimesOpsBuildSkippedOver = 0;

	for(iI = 0; iI < m_pAIStrategies->GetNumMilitaryAIStrategies(); iI++)
	{
		m_pabUsingStrategy[iI] = false;
		m_paiTurnStrategyAdopted[iI] = -1;
		m_paeLastTurnWarState[iI] = NO_WAR_STATE_TYPE;
		m_paeLastTurnMilitaryThreat[iI] = NO_THREAT_VALUE;
		m_paeLastTurnMilitaryStrength[iI] = NO_STRENGTH_VALUE;
		m_paeLastTurnTargetValue[iI] = NO_TARGET_VALUE;
	}
}

/// Serialization read
void CvMilitaryAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	FAssertMsg(m_pAIStrategies != NULL && m_pAIStrategies->GetNumMilitaryAIStrategies() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream >> m_iTotalThreatWeight;

	if(uiVersion >= 3)
	{
		int temp;
		kStream >> temp;
		m_eArmyTypeBeingBuilt = (ArmyType)temp;
	}
	else
	{
		m_eArmyTypeBeingBuilt = NO_ARMY_TYPE;
	}

	if(uiVersion >= 4)
	{
		kStream >> m_iNumberOfTimesOpsBuildSkippedOver;
	}
	else
	{
		m_iNumberOfTimesOpsBuildSkippedOver = 0;
	}

	int iNumStrategies = 16; // 16 is the number that the old version had
	if(uiVersion > 1)
	{
		kStream >> iNumStrategies;
	}
#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 6011 ) // dereferencing null : no recovery during load, go ahead and crash here.
#endif//_MSC_VER
	ArrayWrapper<bool> wrapm_pabUsingStrategy(iNumStrategies, m_pabUsingStrategy);
#ifdef _MSC_VER
#pragma warning ( pop )
#endif//_MSC_VER
	kStream >> wrapm_pabUsingStrategy;

	ArrayWrapper<int> wrapm_paiTurnStrategyAdopted(iNumStrategies, m_paiTurnStrategyAdopted);
	kStream >> wrapm_paiTurnStrategyAdopted;

	ArrayWrapper<int> wrapm_paeLastTurnWarState(MAX_CIV_PLAYERS, m_paeLastTurnWarState);
	kStream >> wrapm_paeLastTurnWarState;

	ArrayWrapper<int> wrapm_paeLastTurnMilitaryThreat(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryThreat);
	kStream >> wrapm_paeLastTurnMilitaryThreat;

	ArrayWrapper<int> wrapm_paeLastTurnMilitaryStrength(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryStrength);
	kStream >> wrapm_paeLastTurnMilitaryStrength;

	ArrayWrapper<int> wrapm_paeLastTurnTargetValue(MAX_CIV_PLAYERS, m_paeLastTurnTargetValue);
	kStream >> wrapm_paeLastTurnTargetValue;
}

/// Serialization write
void CvMilitaryAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 4;
	kStream << uiVersion;

	FAssertMsg(GC.getNumMilitaryAIStrategyInfos() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream << m_iTotalThreatWeight;
	kStream << (int)m_eArmyTypeBeingBuilt;
	kStream << m_iNumberOfTimesOpsBuildSkippedOver;
	kStream << GC.getNumMilitaryAIStrategyInfos();
	kStream << ArrayWrapper<bool>(m_pAIStrategies->GetNumMilitaryAIStrategies(), m_pabUsingStrategy);
	kStream << ArrayWrapper<int>(m_pAIStrategies->GetNumMilitaryAIStrategies(), m_paiTurnStrategyAdopted);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnWarState);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryThreat);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryStrength);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnTargetValue);
}

/// Returns the Player object the Strategies are associated with
CvPlayer* CvMilitaryAI::GetPlayer()
{
	return m_pPlayer;
}

/// Returns AIStrategies object stored in this class
CvMilitaryAIStrategyXMLEntries* CvMilitaryAI::GetMilitaryAIStrategies()
{
	return m_pAIStrategies;
}

/// Returns whether or not a player has adopted this Strategy
bool CvMilitaryAI::IsUsingStrategy(MilitaryAIStrategyTypes eStrategy)
{
	return m_pabUsingStrategy[(int) eStrategy];
}

/// Signifies that eStrategy is now being utilized by this AI Player
void CvMilitaryAI::SetUsingStrategy(MilitaryAIStrategyTypes eStrategy, bool bValue)
{
	if(m_pabUsingStrategy[eStrategy] != bValue)
	{
		m_pabUsingStrategy[eStrategy] = bValue;

		if(bValue)
		{
			SetTurnStrategyAdopted(eStrategy, GC.getGame().getGameTurn());
		}
		else
		{
			SetTurnStrategyAdopted(eStrategy, -1);
		}

		LogStrategy(eStrategy, bValue);
	}
}

/// Returns the turn on which a Strategy was adopted (-1 if it hasn't been)
int CvMilitaryAI::GetTurnStrategyAdopted(MilitaryAIStrategyTypes eStrategy)
{
	return m_paiTurnStrategyAdopted[(int) eStrategy];
}

/// Sets the turn number eStrategy was most recently adopted
void CvMilitaryAI::SetTurnStrategyAdopted(MilitaryAIStrategyTypes eStrategy, int iValue)
{
	if(m_paiTurnStrategyAdopted[(int) eStrategy] != iValue)
	{
		m_paiTurnStrategyAdopted[(int) eStrategy] = iValue;
	}
}

/// Process through all the military activities for a player's turn
void CvMilitaryAI::DoTurn()
{
	AI_PERF_FORMAT("AI-perf.csv", ("MilitaryAI DoTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	ScanForBarbarians();
	UpdateThreats();
	UpdateWars();
	UpdateBaseData();
	UpdateDefenseState();
	UpdateMilitaryStrategies();

	if(!m_pPlayer->isHuman())
	{
		UpdateOperations();
		MakeEmergencyPurchases();
		RequestImprovements();
		DisbandObsoleteUnits();
	}

	LogMilitaryStatus();

	if(GetArmyBeingBuilt() != NO_ARMY_TYPE)
	{
		LogAvailableForces();
	}
}

/// Requests for sneak attack on a city of a player we're not at war with. Returns true if operation started.
bool CvMilitaryAI::RequestSneakAttack(PlayerTypes eEnemy)
{
	CvMilitaryTarget target;
	CvAIOperation* pOperation = 0;
	int iOperationID;

	// Let's only allow us to be sneak attacking one opponent at a time, so abort if already have one of these operations active against any opponent
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SNEAK_ATTACK, &iOperationID))
	{
		return false;
	}
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_SNEAK_CITY_ATTACK, &iOperationID))
	{
		return false;
	}

	target = FindBestAttackTarget(AI_OPERATION_SNEAK_CITY_ATTACK, eEnemy);
	if(target.m_pTargetCity)
	{
		if(target.m_bAttackBySea)
		{
			if(IsAttackReady(MUFORMATION_NAVAL_INVASION, AI_OPERATION_NAVAL_SNEAK_ATTACK))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SNEAK_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				m_iNumNavalAttacksRequested++;
				m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
			}
		}
		else
		{
			if(IsAttackReady((GC.getGame().getHandicapInfo().GetID() > 4) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, AI_OPERATION_SNEAK_CITY_ATTACK))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_SNEAK_CITY_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				m_iNumLandAttacksRequested++;
				m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
			}
		}
	}
	return false;
}

/// Send an army to force concessions
bool CvMilitaryAI::RequestShowOfForce(PlayerTypes eEnemy)
{
	CvMilitaryTarget target;

	target = FindBestAttackTarget(AI_OPERATION_SNEAK_CITY_ATTACK, eEnemy);
	if(target.m_pTargetCity)
	{
		if (target.m_bAttackBySea)
		{
			CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SNEAK_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
			if(pOperation != NULL && !pOperation->ShouldAbort())
			{
				return true;
			}
		}
		else
		{
			CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_SMALL_CITY_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
			if(pOperation != NULL && !pOperation->ShouldAbort())
			{
				return true;
			}
		}
	}

	return false;
}


/// Ask to send a nuke at an enemy
bool CvMilitaryAI::RequestNukeAttack(PlayerTypes eEnemy)
{
	if(m_pPlayer->getNumNukeUnits() > 0)
	{
		CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NUKE_ATTACK, eEnemy);
		if(pOperation != NULL && pOperation->GetOperationState() != AI_OPERATION_STATE_ABORTED)
		{
			return true;
		}
	}

	return false;
}


/// Send an army to force concessions
bool CvMilitaryAI::RequestPillageAttack(PlayerTypes eEnemy)
{
	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;
	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_FAST_PILLAGERS, false, &iNumRequiredSlots, &iLandReservesUsed);
	if(iFilledSlots >= iNumRequiredSlots && iLandReservesUsed <= GetLandReservesAvailable())
	{
		CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_PILLAGE_ENEMY, eEnemy);
		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}

	return false;
}


/// Send an army to take a city
bool CvMilitaryAI::RequestBasicAttack(PlayerTypes eEnemy, int iNumUnitsWillingBuild)
{
	CvMilitaryTarget target;

	target = FindBestAttackTarget(AI_OPERATION_BASIC_CITY_ATTACK, eEnemy);
	return RequestSpecificAttack(target, iNumUnitsWillingBuild);
}

/// Send a navy to take a city
bool CvMilitaryAI::RequestPureNavalAttack(PlayerTypes eEnemy, int iNumUnitsWillingBuild)
{
	CvAIOperation* pOperation = NULL;
	CvMilitaryTarget target;
	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;

	target = FindBestAttackTarget(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, eEnemy);
	if(target.m_pTargetCity)
	{
		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_PURE_NAVAL_CITY_ATTACK, true, &iNumRequiredSlots, &iLandReservesUsed);
		if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild)
		{
			pOperation = m_pPlayer->addAIOperation(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
		}

		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}
	return false;
}

/// Request for an attack on a city state
bool CvMilitaryAI::RequestCityStateAttack(PlayerTypes eEnemy)
{
	CvMilitaryTarget target;
	CvAIOperation* pOperation = 0;

	target = FindBestAttackTarget(AI_OPERATION_CITY_STATE_ATTACK, eEnemy);
	if(target.m_pTargetCity)
	{
		if(target.m_bAttackBySea)
		{
			if(IsAttackReady(MUFORMATION_CITY_STATE_INVASION, AI_OPERATION_CITY_STATE_NAVAL_ATTACK))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				m_iNumNavalAttacksRequested++;
				m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
			}
		}
		else
		{
			if(IsAttackReady(MUFORMATION_CITY_STATE_ATTACK_FORCE, AI_OPERATION_CITY_STATE_ATTACK))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				m_iNumLandAttacksRequested++;
				m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
			}
		}

		return true;
	}
	return false;
}

bool CvMilitaryAI::RequestSpecificAttack(CvMilitaryTarget kTarget, int iNumUnitsWillingToBuild)
{
	CvAIOperation* pOperation = NULL;
	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;

	if(kTarget.m_pTargetCity)
	{
		if(kTarget.m_bAttackBySea)
		{
			iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_INVASION, true, &iNumRequiredSlots, &iLandReservesUsed);
			if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild && iLandReservesUsed <= GetLandReservesAvailable())
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pTargetCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
			}
		}
		else
		{
			iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer,(GC.getGame().getHandicapInfo().GetID() > 4) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, false, &iNumRequiredSlots, &iLandReservesUsed);
			if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild && iLandReservesUsed <= GetLandReservesAvailable())
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_BASIC_CITY_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pTargetCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
			}
		}

		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}
	return false;
}

/// Get a pointer to the sneak attack operation against a target
CvAIOperation* CvMilitaryAI::GetSneakAttackOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_SNEAK_CITY_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}
	else
	{
		bool bHasOperationOfType = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SNEAK_ATTACK, &iOperationID, eEnemy);

		if(bHasOperationOfType)
		{
			return m_pPlayer->getAIOperation(iOperationID);
		}
	}

	return NULL;
}

/// Get a pointer to the show of force operation against a target
CvAIOperation* CvMilitaryAI::GetShowOfForceOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_SMALL_CITY_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}

	return NULL;
}

/// Get a pointer to the basic attack against a target
CvAIOperation* CvMilitaryAI::GetBasicAttackOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}
	else
	{
		bool bHasOperationOfType = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID, eEnemy);

		if(bHasOperationOfType)
		{
			return m_pPlayer->getAIOperation(iOperationID);
		}
	}

	return NULL;
}

/// Get a pointer to the basic attack against a target
CvAIOperation* CvMilitaryAI::GetCityStateAttackOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}
	else
	{
		bool bHasOperationOfType = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, &iOperationID, eEnemy);

		if(bHasOperationOfType)
		{
			return m_pPlayer->getAIOperation(iOperationID);
		}
	}

	return NULL;
}

/// Get a pointer to the pure naval operation against a target
CvAIOperation* CvMilitaryAI::GetPureNavalAttackOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}

	return NULL;
}

/// Spend money to quickly add a unit to a city
CvUnit* CvMilitaryAI::BuyEmergencyUnit(UnitAITypes eUnitType, CvCity* pCity)
{
	// No units in puppet cities
	if(pCity->IsPuppet())
	{
		return NULL;
	}

	// Get best unit with this AI type
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitType);
	if(eType != NO_UNIT)
	{
		// Can we buy the primary unit type at the start city?
		if(pCity->IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, eType, NO_BUILDING, NO_PROJECT, YIELD_GOLD))
		{
			int iGoldCost = pCity->GetPurchaseCost(eType);
			int iPriority = GC.getAI_GOLD_PRIORITY_UNIT();
			if(m_pPlayer->GetEconomicAI()->CanWithdrawMoneyForPurchase(PURCHASE_TYPE_UNIT, iGoldCost, iPriority))
			{
				if(pCity->getOwner() == m_pPlayer->GetID())		// Player must own the city or this will create a unit for another player
				{
					// This is an EXTRA build for the operation beyond any that are already assigned to this city, so pass in the right flag to CreateUnit()
					int iResult = pCity->CreateUnit(eType, NO_UNITAI, false /*bUseToSatisfyOperation*/);

					CvAssertMsg(iResult != FFreeList::INVALID_INDEX, "Unable to create unit");

					if (iResult != FFreeList::INVALID_INDEX)
					{
						m_pPlayer->GetTreasury()->ChangeGold(-iGoldCost);

						CvUnit* pUnit = m_pPlayer->getUnit(iResult);
						pUnit->setMoves(0);

						CvString szMsg;
						szMsg.Format("Emergency Unit Purchased: %s, ", pUnit->getUnitInfo().GetDescription());
						szMsg += pCity->getName();
						m_pPlayer->GetTacticalAI()->LogTacticalMessage(szMsg);

						return pUnit;
					}
					else
					{
						return NULL;
					}
				}
			}
		}

		// Try again with Faith
		if(pCity->IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, eType, NO_BUILDING, NO_PROJECT, YIELD_FAITH))
		{
			int iFaithCost = pCity->GetFaithPurchaseCost(eType, false /*bIncludeBeliefDiscounts*/);

			if(pCity->getOwner() == m_pPlayer->GetID())		// Player must own the city or this will create a unit for another player
			{
				m_pPlayer->ChangeFaith(-iFaithCost);

				// This is an EXTRA build for the operation beyond any that are already assigned to this city, so pass in the right flag to CreateUnit()
				int iResult = pCity->CreateUnit(eType, NO_UNITAI, false /*bUseToSatisfyOperation*/);

				CvAssertMsg(iResult != FFreeList::INVALID_INDEX, "Unable to create unit");
				CvUnit* pUnit = m_pPlayer->getUnit(iResult);
				pUnit->setMoves(0);

				CvString szMsg;
				szMsg.Format("Emergency Faith Unit Purchase: %s, ", pUnit->getUnitInfo().GetDescription());
				szMsg += pCity->getName();
				m_pPlayer->GetTacticalAI()->LogTacticalMessage(szMsg);

				return pUnit;
			}
		}
	}
	return NULL;
}

/// Spend money to quickly add a defensive building to a city
bool CvMilitaryAI::BuyEmergencyBuilding(CvCity* pCity)
{
	// Loop through adding the available buildings
	for(int iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{
		const BuildingTypes eBldg = static_cast<BuildingTypes>(iBldgLoop);
		CvBuildingEntry* pkBuildingInfo = GC.GetGameBuildings()->GetEntry(eBldg);
		if(pkBuildingInfo)
		{
			// Make sure this building can be built now
			if(pCity->canConstruct(eBldg) && pkBuildingInfo->GetDefenseModifier() > 0)
			{
				if(pCity->IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, NO_UNIT, eBldg, NO_PROJECT, YIELD_GOLD))
				{
					int iGoldCost = pCity->GetPurchaseCost(eBldg);
					int iPriority = GC.getAI_GOLD_PRIORITY_DEFENSIVE_BUILDING();
					if(m_pPlayer->GetEconomicAI()->CanWithdrawMoneyForPurchase(PURCHASE_TYPE_BUILDING, iGoldCost, iPriority))
					{
						m_pPlayer->GetTreasury()->ChangeGold(-iGoldCost);
						int iResult = pCity->CreateBuilding(eBldg);
						DEBUG_VARIABLE(iResult);
						CvAssertMsg(iResult != FFreeList::INVALID_INDEX, "Unable to create building");

						CvString szMsg;
						szMsg.Format("Emergency Building Purchased: %s, ", pkBuildingInfo->GetDescription());
						szMsg += pCity->getName();
						m_pPlayer->GetTacticalAI()->LogTacticalMessage(szMsg);
					}
				}
			}
		}
	}
	return false;
}

// FINDING BEST CITIES TO TARGET

/// Best target by land OR sea
CvMilitaryTarget CvMilitaryAI::FindBestAttackTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, int* piWinningScore)
{
	int iFriendlyLoop;
	int iEnemyLoop;
	CvCity* pFriendlyCity;
	CvCity* pEnemyCity;
	CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true> weightedTargetList;
	CvMilitaryTarget chosenTarget;

	// Build a list of all the possible start city/target city pairs
	for(pFriendlyCity = m_pPlayer->firstCity(&iFriendlyLoop); pFriendlyCity != NULL; pFriendlyCity = m_pPlayer->nextCity(&iFriendlyLoop))
	{
		for(pEnemyCity = GET_PLAYER(eEnemy).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eEnemy).nextCity(&iEnemyLoop))
		{
			if(pEnemyCity->plot()->isRevealed(m_pPlayer->getTeam()))
			{
				CvMilitaryTarget target;
				int iWeight;
				target.m_pMusterCity = pFriendlyCity;
				target.m_pTargetCity = pEnemyCity;

				if (eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK)
				{
					target.m_bAttackBySea = true;
					if (target.m_pMusterCity->isCoastal() && target.m_pTargetCity->isCoastal())
					{
						target.m_iPathLength = plotDistance(target.m_pMusterCity->getX(), target.m_pMusterCity->getY(), target.m_pTargetCity->getX(), target.m_pTargetCity->getY());
					}
				}

				else
				{
					ShouldAttackBySea(eEnemy, target);

					if (!pFriendlyCity->isCoastal() && target.m_bAttackBySea)
					{
						continue;
					}
				}

				if(target.m_iPathLength > 0)
				{
					// If a sea target, we haven't checked the path yet.  Do that now
					if (target.m_bAttackBySea)
					{
						if (!pFriendlyCity->isCoastal())
						{
							continue;
						}
						if (!pEnemyCity->isCoastal())
						{
							continue;
						}
						CvPlot *pSeaPlotNearMuster = GetCoastalPlotAdjacentToTarget(pFriendlyCity->plot(), NULL);
						CvPlot *pSeaPlotNearTarget = GetCoastalPlotAdjacentToTarget(pEnemyCity->plot(), NULL);
						if(!GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, pSeaPlotNearMuster, pSeaPlotNearTarget))
						{
							continue;
						}
					}

					iWeight = ScoreTarget(target, eAIOperationType);
					weightedTargetList.push_back(target, iWeight);
				}
			}
		}
	}

	// Didn't find anything, abort
	if(weightedTargetList.size() == 0)
	{
		chosenTarget.m_pTargetCity = NULL;   // Call off the attack
		if (piWinningScore)
		{
			*piWinningScore = -1;
		}
		return chosenTarget;
	}

	weightedTargetList.SortItems();
	LogAttackTargets(eAIOperationType, eEnemy, weightedTargetList);

	if(weightedTargetList.GetTotalWeight() > 0)
	{
		RandomNumberDelegate fcn;
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		int iNumChoices = max (1, (weightedTargetList.size() * 35/ 100));
		chosenTarget = weightedTargetList.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing attack target from top 35%% of choices");
		// if we need the winning score
		if (piWinningScore)
		{
			*piWinningScore = ScoreTarget(chosenTarget, eAIOperationType);
		}
		LogChosenTarget(eAIOperationType, eEnemy, chosenTarget);
	}
	else
	{
		chosenTarget.m_pTargetCity = NULL;   // Call off the attack
		if (piWinningScore)
		{
			*piWinningScore = -1;
		}
	}

	return chosenTarget;
}

/// Is it better to attack this target by sea?
void CvMilitaryAI::ShouldAttackBySea(PlayerTypes eEnemy, CvMilitaryTarget& target)
{
	CvAStarNode* pPathfinderNode;
	int iPathLength = 0;
	int iPlotDistance = plotDistance(target.m_pMusterCity->getX(), target.m_pMusterCity->getY(), target.m_pTargetCity->getX(), target.m_pTargetCity->getY());

	// Can embark
	if(GET_TEAM(m_pPlayer->getTeam()).canEmbark())
	{
		// On different landmasses?
		if(target.m_pMusterCity->getArea() != target.m_pTargetCity->getArea())
		{
			target.m_bAttackBySea = true;
			target.m_iPathLength = iPlotDistance;
			return;
		}

		// No step path between muster point and target?
		if(!GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, target.m_pMusterCity->plot(), target.m_pTargetCity->plot()))
		{
			target.m_bAttackBySea = true;
			target.m_iPathLength = iPlotDistance;
			return;
		}

		// Land path is over twice as long as direct path
		pPathfinderNode = GC.getStepFinder().GetLastNode();
		if(pPathfinderNode != NULL)
		{
			iPathLength = pPathfinderNode->m_iData1;
			if(iPathLength > (2 * iPlotDistance))
			{
				target.m_bAttackBySea = true;
				target.m_iPathLength = iPlotDistance;
				return;
			}
		}
	}

	// Can't embark yet
	else
	{
		if(!GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, target.m_pMusterCity->plot(), target.m_pTargetCity->plot()))
		{
			target.m_iPathLength = -1;  // Call off attack, no path
			return;
		}

		else
		{
			pPathfinderNode = GC.getStepFinder().GetLastNode();
			if(pPathfinderNode != NULL)
			{
				iPathLength = pPathfinderNode->m_iData1;
			}
		}
	}

	target.m_bAttackBySea = false;
	target.m_iPathLength = iPathLength;
}

/// Come up with a target priority looking at distance, strength, approaches (high score = more desirable target)
int CvMilitaryAI::ScoreTarget(CvMilitaryTarget& target, AIOperationTypes eAIOperationType)
{
	unsigned long int uliRtnValue = 1;  // Start with a high base number since divide into it later

	// Take into account distance to target (and use higher multipliers for land paths)
	if(!target.m_bAttackBySea)
	{
		if(target.m_iPathLength < 10)
		{
			uliRtnValue *= 16;
		}
		else if(target.m_iPathLength < 15)
		{
			uliRtnValue *= 8;
		}
		else if(target.m_iPathLength < 20)
		{
			uliRtnValue *= 4;
		}
		else
		{
			uliRtnValue *= 2;
		}
	}
	else
	{
		if(target.m_iPathLength < 12)
		{
			uliRtnValue *= 5;
		}
		else if(target.m_iPathLength < 20)
		{
			uliRtnValue *= 3;
		}
		else if(target.m_iPathLength < 30)
		{
			uliRtnValue *= 2;
		}

		// If coming over sea, inland cities are trickier
		if(!target.m_pTargetCity->plot()->isCoastalLand())
		{
			uliRtnValue /= 2;
		}
	}

	// Is this a sneak attack?  If so distance is REALLY important (want to target spaces on edge of empire)
	// So let's cube what we have so far
	if (eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK || eAIOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK)
	{
		uliRtnValue = uliRtnValue * uliRtnValue * uliRtnValue;
	}

	CityAttackApproaches eApproaches;
	int iApproachMultiplier = 0;
	eApproaches = EvaluateMilitaryApproaches(target.m_pTargetCity, true /* Assume units coming by sea can disembark */, target.m_bAttackBySea);
	switch(eApproaches)
	{
	case ATTACK_APPROACH_UNRESTRICTED:
		iApproachMultiplier = 10;
		break;

	case ATTACK_APPROACH_OPEN:
		iApproachMultiplier = 8;
		break;

	case ATTACK_APPROACH_NEUTRAL:
		iApproachMultiplier = 4;
		break;

	case ATTACK_APPROACH_LIMITED:
		iApproachMultiplier = 2;
		break;

	case ATTACK_APPROACH_RESTRICTED:
		iApproachMultiplier = 1;
		break;

	case ATTACK_APPROACH_NONE:
		iApproachMultiplier = 0;
		break;
	}

	uliRtnValue *= iApproachMultiplier;

	// Don't want to start at a city that isn't connected to our capital
	if (!target.m_pMusterCity->IsRouteToCapitalConnected())
	{
		uliRtnValue /= 4;
	}

	// Strength data from tactical analysis map (use if map built by this time)
	CvTacticalDominanceZone* pZone;
	CvTacticalAnalysisMap* pTactMap = GC.getGame().GetTacticalAnalysisMap();
	if(pTactMap && pTactMap->GetPlayer() == m_pPlayer)
	{
		// Our power near muster point
		int iFriendlyStrength = 1;
		pZone = pTactMap->GetZoneByCity(target.m_pMusterCity, false /* Land */);
		if(pZone)
		{
			iFriendlyStrength = pZone->GetFriendlyStrength();
		}
		iFriendlyStrength = max(1, iFriendlyStrength);
		uliRtnValue *= iFriendlyStrength;

		// Their power near target
		int iEnemyStrength = 1;
		pZone = pTactMap->GetZoneByCity(target.m_pTargetCity, false /* Land */);
		if(pZone)
		{
			iEnemyStrength = pZone->GetEnemyStrength();
		}
		iEnemyStrength = max(1000, iEnemyStrength);
		uliRtnValue /= iEnemyStrength;
	}

	// Don't want it to already be targeted by an operation that's not well on its way
	if(m_pPlayer->IsCityAlreadyTargeted(target.m_pTargetCity, NO_DOMAIN, 50))
	{
		uliRtnValue /= 10;
	}

	// Defense value of target
	int iTargetStrengthMultipler = (5000 - target.m_pTargetCity->getStrengthValue()) / 100;
	iTargetStrengthMultipler = max(1, iTargetStrengthMultipler);
	uliRtnValue *= iTargetStrengthMultipler;

	return min(10000000, (int)uliRtnValue);
}

/// How open an approach do we have to this city if we want to attack it?
CityAttackApproaches CvMilitaryAI::EvaluateMilitaryApproaches(CvCity* pCity, bool bAttackByLand, bool bAttackBySea)
{
	CvPlot* pLoopPlot;
	CityAttackApproaches eRtnValue = ATTACK_APPROACH_UNRESTRICTED;
	int iNumBlocked = 0;

	// Look at each of the six plots around the city
	for(int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		pLoopPlot = plotDirection(pCity->getX(), pCity->getY(), ((DirectionTypes)iI));

		// Blocked if edge of map
		if(pLoopPlot == NULL)
		{
			iNumBlocked++;
		}
		else
		{
			// For now, assume no one coming in over a lake
			if(pLoopPlot->isLake())
			{
				iNumBlocked++;
			}
			// Coast but attack is not by sea?
			else if(pLoopPlot->isWater() && !bAttackBySea)
			{
				iNumBlocked++;
			}
			// Land
			else if(!pLoopPlot->isWater())
			{
				if(!bAttackByLand)
				{
					iNumBlocked++;
				}
				else
				{
					if(pLoopPlot->isImpassable() || pLoopPlot->isMountain())
					{
						iNumBlocked++;
					}
				}
			}
		}
	}

	switch(iNumBlocked)
	{
	case 0:
		eRtnValue = ATTACK_APPROACH_UNRESTRICTED;
		break;
	case 1:
	case 2:
		eRtnValue = ATTACK_APPROACH_OPEN;
		break;
	case 3:
		eRtnValue = ATTACK_APPROACH_NEUTRAL;
		break;
	case 4:
		eRtnValue = ATTACK_APPROACH_LIMITED;
		break;
	case 5:
		eRtnValue = ATTACK_APPROACH_RESTRICTED;
		break;
	case 6:
		eRtnValue = ATTACK_APPROACH_NONE;
		break;
	}

	return eRtnValue;
}

/// Find the port operation operations against this enemy should leave from
CvCity* CvMilitaryAI::GetNearestCoastalCity(PlayerTypes eEnemy) const
{
	CvCity* pBestCoastalCity = NULL;
	CvCity* pLoopCity, *pEnemyCity;
	int iLoop, iEnemyLoop;
	int iBestDistance = MAX_INT;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->isCoastal())
		{
			for(pEnemyCity = GET_PLAYER(eEnemy).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eEnemy).nextCity(&iEnemyLoop))
			{
				// Check all revealed enemy cities
				if(pEnemyCity->isCoastal() && pEnemyCity->plot()->isRevealed(m_pPlayer->getTeam()))
				{
					// On same body of water?
					if(OnSameBodyOfWater(pLoopCity, pEnemyCity))
					{
						int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pEnemyCity->getX(), pEnemyCity->getY());
						if(iDistance < iBestDistance)
						{
							iBestDistance = iDistance;
							pBestCoastalCity = pLoopCity;
						}
					}
				}
			}
		}
	}

	return pBestCoastalCity;
}

CvPlot* CvMilitaryAI::GetCoastalPlotAdjacentToTarget(CvPlot *pTarget, CvArmyAI *pArmy) const
{
	CvPlot *pCoastalPlot = NULL;
	UnitHandle pInitialUnit;
	int iBestDistance = MAX_INT;
	int iUnitID = -1;
	if (pArmy)
	{
		iUnitID = pArmy->GetFirstUnitID();
		pInitialUnit = m_pPlayer->getUnit(iUnitID);
	}

	// Find a coastal water tile adjacent to enemy city
	for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
	{
		CvPlot* pAdjacentPlot = plotDirection(pTarget->getX(), pTarget->getY(), ((DirectionTypes)iDirectionLoop));
		if(pAdjacentPlot != NULL && pAdjacentPlot->isWater() && pAdjacentPlot->isShallowWater())
		{
			// Check for path if we have a unit, otherwise don't worry about it
			if(pInitialUnit)
			{
				if (TurnsToReachTarget(pInitialUnit, pAdjacentPlot, false /*bReusePaths*/, true /*bIgnoreUnits*/, true /*bIgnoreStacking*/) < MAX_INT)
				{
					int iDistance = plotDistance(pInitialUnit->getX(), pInitialUnit->getY(), pTarget->getX(), pTarget->getY());
					if (iDistance < iBestDistance)
					{
						iBestDistance = iDistance;
						pCoastalPlot = pAdjacentPlot;
					}
				}
			}
			else
			{
				return pAdjacentPlot;
			}
		}
	}

	return pCoastalPlot;
}

// PROVIDE MILITARY DATA TO OTHER SUBSYSTEMS

/// See if the threats we are facing have changed
ThreatTypes CvMilitaryAI::GetHighestThreat()
{
	ThreatTypes eHighestThreat = THREAT_NONE;
	ThreatTypes eMilitaryThreatType;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
		if(eLoopPlayer != m_pPlayer->GetID() && m_pDiplomacyAI->IsPlayerValid(eLoopPlayer))
		{
			eMilitaryThreatType = m_pDiplomacyAI->GetMilitaryThreat(eLoopPlayer);
			if((int)eMilitaryThreatType > (int)eHighestThreat)
			{
				eHighestThreat = eMilitaryThreatType;
			}
		}
	}
	return eHighestThreat;
}

/// How threatening are the barbarians?
int CvMilitaryAI::GetBarbarianThreatTotal()
{
	int iRtnValue = 0;

	ScanForBarbarians();

	// Minor threat for each camp seen
	iRtnValue += GC.getAI_MILITARY_THREAT_WEIGHT_MAJOR() * m_iBarbarianCampCount;

	// One minor threat for every X barbarians
	iRtnValue += m_iVisibleBarbarianCount / GC.getAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT();

	return iRtnValue;
}

/// What is the numerical value of this threat level?
int CvMilitaryAI::GetThreatWeight(ThreatTypes eThreat)
{
	int iRtnValue = 0;

	switch(eThreat)
	{
	case THREAT_MINOR:
		iRtnValue = GC.getAI_MILITARY_THREAT_WEIGHT_MINOR();
		break;
	case THREAT_MAJOR:
		iRtnValue = GC.getAI_MILITARY_THREAT_WEIGHT_MAJOR();
		break;
	case THREAT_SEVERE:
		iRtnValue = GC.getAI_MILITARY_THREAT_WEIGHT_SEVERE();
		break;
	case THREAT_CRITICAL:
		iRtnValue = GC.getAI_MILITARY_THREAT_WEIGHT_CRITICAL();
		break;
	}

	return iRtnValue;
}

/// How many civs are we fighting?
int CvMilitaryAI::GetNumberCivsAtWarWith() const
{
	PlayerTypes eLoopPlayer;
	WarStateTypes eWarState;
	int iRtnValue = 0;

	// Let's figure out if we're at war
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
		if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);
			if(eWarState != NO_WAR_STATE_TYPE)
			{
				iRtnValue++;
			}
		}
	}

	return iRtnValue;
}

/// Which city is in the most danger now?
CvCity* CvMilitaryAI::GetMostThreatenedCity(int iOrder)
{
	// slewis - this is slow, but I did it quickly!

	FFastVector<CvCity*> m_apCities;
	m_apCities.push_back_copy(NULL, iOrder + 1);

	for(int iCycle = 0; iCycle < iOrder + 1; iCycle++)
	{
		CvCity* pCity = NULL;
		int iHighestThreatValue = 0;

		CvCity* pLoopCity;
		int iLoopCity = 0;
		for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
		{
			bool bCityAlreadyExamined = false;
			for(uint ui = 0; ui < m_apCities.size(); ui++)
			{
				if(pLoopCity == m_apCities[ui])
				{
					bCityAlreadyExamined = true;
					break;
				}
			}
			if(bCityAlreadyExamined)
			{
				continue;
			}

			int iThreatValue = pLoopCity->getThreatValue();
			iThreatValue = iThreatValue * pLoopCity->getPopulation();

			if(pLoopCity->isCapital())
			{
				iThreatValue = (iThreatValue * GC.getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL()) / 100;
			}

			if(iThreatValue > iHighestThreatValue)
			{
				pCity = pLoopCity;
				iHighestThreatValue = iThreatValue;
			}
		}

		if(pCity)
		{
			m_apCities[iCycle] = pCity;
		}
		else
		{
			// we didn't find a city, so bail
			break;
		}
	}

	return m_apCities[iOrder];
}

/// How big is our military compared to the recommended size?
int CvMilitaryAI::GetPercentOfRecommendedMilitarySize() const
{
	if(m_iRecommendedMilitarySize <= 0)
	{
		return 100;
	}
	else
	{
		return m_iNumLandUnits * 100 / m_iRecommendedMilitarySize;
	}
}

/// Log all potential attacks
void CvMilitaryAI::LogAttackTargets(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true>& weightedTargetList)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strLogName;

		// Find the name of this civ
		playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		FILogFile* pLog;
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "OperationalAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "OperationalAILog.csv";
		}
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";
		if(eAIOperationType == AI_OPERATION_BASIC_CITY_ATTACK)
		{
			strBaseString += "Basic Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK)
		{
			strBaseString += "Sneak Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK)
		{
			strBaseString += "Pure Naval Attack, ";
		}
		else
		{
			strBaseString += "City State Attack, ";
		}
		strBaseString += GET_PLAYER(eEnemy).getCivilizationShortDescription();
		strBaseString += ", ";

		// Dump out the weight of each buildable item
		for(int iI = 0; iI < weightedTargetList.size(); iI++)
		{
			CvMilitaryTarget target = weightedTargetList.GetElement(iI);
			int iWeight = weightedTargetList.GetWeight(iI);
			strTemp.Format("Target: %s, Muster: %s, %d, Distance: %d", target.m_pTargetCity->getName().GetCString(), target.m_pMusterCity->getName().GetCString(), iWeight, target.m_iPathLength);
			strOutBuf = strBaseString + strTemp;
			if(target.m_bAttackBySea)
			{
				strOutBuf += ", Sea";
			}
			else
			{
				strOutBuf += ", Land";
			}
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log chosen attack
void CvMilitaryAI::LogChosenTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, CvMilitaryTarget& target)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = m_pPlayer->getCivilizationShortDescription();

		// Open the log file
		FILogFile* pLog;
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "OperationalAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "OperationalAILog.csv";
		}
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";
		if(eAIOperationType == AI_OPERATION_BASIC_CITY_ATTACK)
		{
			strBaseString += "Basic Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK)
		{
			strBaseString += "Sneak Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK)
		{
			strBaseString += "Pure Naval Attack, ";
		}
		else
		{
			strBaseString += "City State Attack, ";
		}
		strBaseString += GET_PLAYER(eEnemy).getCivilizationShortDescription();
		strBaseString += ", ";

		strTemp.Format("CHOSEN: %s, Muster: %s", target.m_pTargetCity->getName().GetCString(), target.m_pMusterCity->getName().GetCString());
		strOutBuf = strBaseString + strTemp;
		if(target.m_bAttackBySea)
		{
			strOutBuf += ", Sea";
		}
		else
		{
			strOutBuf += ", Land";
		}
		pLog->Msg(strOutBuf);
	}
}

/// Log it if we issue a DOW
void CvMilitaryAI::LogDeclarationOfWar(PlayerTypes eOpponent)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString opponentName;
		CvString playerName;
		CvString strTemp;
		FILogFile* pLog;

		// Open the right file
		playerName = GetPlayer()->getCivilizationShortDescription();
		opponentName = GET_PLAYER(eOpponent).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// Strategy Info
		strTemp.Format("Declared War on: %s", opponentName.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}

/// Log it if we die
void CvMilitaryAI::LogCivilizationDestroyed()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strTemp;
		FILogFile* pLog;

		// Open the right file
		playerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// Strategy Info
		strTemp.Format("DESTROYED!");
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}

/// Log it if we capture a city
void CvMilitaryAI::LogCityCaptured(CvCity* pCity, PlayerTypes eOldOwner)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strOldOwnerName;
		CvString strCityName;
		CvString strTemp;
		FILogFile* pLog;

		// Open the right file
		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		strOldOwnerName = GET_PLAYER(eOldOwner).getCivilizationShortDescription();
		strCityName = pCity->getName();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		// Strategy Info
		strTemp = "Captured: " + strCityName + ", ";
		strTemp += "From: " + strOldOwnerName;
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}

/// Log it if we raze a city
void CvMilitaryAI::LogCityRazed(CvCity* pCity, PlayerTypes eOldOwner)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		CvString strOldOwnerName;
		CvString strCityName;
		CvString strTemp;
		FILogFile* pLog;

		// Open the right file
		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		strOldOwnerName = GET_PLAYER(eOldOwner).getCivilizationShortDescription();
		strCityName = pCity->getName();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		// Strategy Info
		strTemp = "Razed: " + strCityName + ", ";
		strTemp += "From: " + strOldOwnerName;
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}

/// Log it if we make peace
void CvMilitaryAI::LogPeace(PlayerTypes eOpponent)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strOpponentName;
		CvString strPlayerName;
		CvString strTemp;
		FILogFile* pLog;

		// Open the right file
		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		strOpponentName = GET_PLAYER(eOpponent).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		// Strategy Info
		strTemp.Format("Made peace with: %s", strOpponentName.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}

/// Log it if we make peace
void CvMilitaryAI::LogPeace(TeamTypes eOpponentTeam)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strOpponentName;
		CvString strPlayerName;
		CvString strTemp;
		FILogFile* pLog;

		// Open the right file
		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		// Collect the names of the players on the team
		for(int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
			if(kPlayer.isAlive() && kPlayer.getTeam() == eOpponentTeam)
			{
				if(strOpponentName.GetLength() != 0)
					strOpponentName += ", ";

				strOpponentName += kPlayer.getCivilizationShortDescription();
			}
		}
		// Strategy Info
		strTemp.Format("Made peace with team: %d (%s)", (int)eOpponentTeam, strOpponentName.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}


/// Log that a unit is being scrapped because of the deficit checks
void CvMilitaryAI::LogDeficitScrapUnit(UnitHandle pUnit)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strTemp;
		CvString playerName;
		FILogFile* pLog;

		// Open the right file
		playerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		strOutBuf.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strOutBuf += playerName + ", ";
		strTemp.Format("Scrapping %s, X: %d, Y: %d, ", pUnit->getUnitInfo().GetDescription(), pUnit->getX(), pUnit->getY());
		strOutBuf += strTemp;
		strOutBuf += "by DoDeficit(), , ";   //extra space so format is consistent with LogScrapUnit()
		if(pUnit->getDomainType() == DOMAIN_LAND)
		{
			strTemp.Format("Num Land Units: %d, In Armies %d, Rec Size: %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecommendedMilitarySize);
		}
		else
		{
			strTemp.Format("Num Naval Units: %d, In Armies %d, ", m_iNumNavalUnits, m_iNumNavalUnitsInArmies);
		}
		strOutBuf += strTemp;
		pLog->Msg(strOutBuf);
	}
}

// PRIVATE METHODS

/// Compute data we need later to make decisions during the turn processing
void CvMilitaryAI::UpdateBaseData()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateBaseData, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	// Count how many Military Units I have right now
	CvUnit* pLoopUnit;
	int iLoop;

	m_iNumLandUnits = 0;
	m_iNumRangedLandUnits = 0;
	m_iNumMobileLandUnits = 0;
	m_iNumAirUnits = 0;
	m_iNumAntiAirUnits = 0;
	m_iNumMeleeLandUnits = 0;
	m_iNumNavalUnits = 0;
	m_iNumLandUnitsInArmies = 0;
	m_iNumNavalUnitsInArmies = 0;

	for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// Don't count civilians or exploration units
		if(pLoopUnit->IsCanAttack() && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{
			if(pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				m_iNumLandUnits++;

				if(pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
				{
					m_iNumLandUnitsInArmies++;
				}

				if(pLoopUnit->IsCanAttackRanged())
				{
					m_iNumRangedLandUnits++;
				}
				else if(pLoopUnit->getExtraIntercept() > 0)
				{
					// I'm an anti-air unit
					m_iNumAntiAirUnits++;
				}
				else if(pLoopUnit->getUnitInfo().GetMoves() > 2)
				{
					m_iNumMobileLandUnits++;
				}
				else
				{
					m_iNumMeleeLandUnits++;
				}
			}
			else if(pLoopUnit->getDomainType() == DOMAIN_SEA)
			{
				m_iNumNavalUnits++;

				if(pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
				{
					m_iNumNavalUnitsInArmies++;
				}
			}
			else if(pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				m_iNumAirUnits++;
			}
		}
	}

	float fMultiplier;
	int iNumUnitsWanted = 0;
	bool bNavalMap = false;
	EconomicAIStrategyTypes eStrategyNavalMap = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NAVAL_MAP");
	if (m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyNavalMap))
	{
		bNavalMap = true;
	}
	int iFlavorOffense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	int iFlavorDefense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"));

	// Scale up or down based on true threat level and a bit by flavors (multiplier should range from about 0.5 to about 1.5)
	fMultiplier = (float)0.40 + (((float)(m_pPlayer->GetMilitaryAI()->GetHighestThreat() + iFlavorOffense + iFlavorDefense)) / (float)100.0);

	// first get the number of defenders that we think we need

	// Start with 3, to protect the capital
	iNumUnitsWanted = GC.getAI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS();

	// 1 Unit per City & 1 per Settler
	iNumUnitsWanted += (int)(m_pPlayer->getNumCities() * /*1.0*/ GC.getAI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY());
	iNumUnitsWanted += m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);

	m_iMandatoryReserveSize = (int)((float)iNumUnitsWanted * fMultiplier);

	// add in a few for the difficulty level (all above Chieftain are boosted)
	int iDifficulty = max(0,GC.getGame().getHandicapInfo().GetID() - 1);
	m_iMandatoryReserveSize += iDifficulty;

	m_iMandatoryReserveSize = max(1,m_iMandatoryReserveSize);

	// now we add in the strike forces we think we will need
	if(m_pPlayer->isMinorCiv())
	{
		m_iMandatoryReserveSize = max(1,m_iMandatoryReserveSize/2);
		iNumUnitsWanted = 0;
	}
	else
	{
		iNumUnitsWanted = 7; // size of a basic attack

		// if we are going for conquest we want at least one more task force
		bool bConquestGrandStrategy = false;
		AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
		if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
		{
			if(m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eConquestGrandStrategy)
			{
				bConquestGrandStrategy = true;
			}
		}
		if(bConquestGrandStrategy)
		{
			iNumUnitsWanted *= 2;
		}

		// add in a few more if the player is bold
		iNumUnitsWanted += m_pPlayer->GetDiplomacyAI()->GetBoldness();

		// add in more if we are playing on a high difficulty
		iNumUnitsWanted += iDifficulty;

		iNumUnitsWanted = (int)((float)iNumUnitsWanted * fMultiplier);

		iNumUnitsWanted = max(1,iNumUnitsWanted);
	}

	if (bNavalMap)
	{
		iNumUnitsWanted *= 2;
		iNumUnitsWanted /= 3;
		m_iMandatoryReserveSize *= 2;
		m_iMandatoryReserveSize /= 3;
	}

	m_iRecommendedMilitarySize = m_iMandatoryReserveSize + iNumUnitsWanted;
}

/// Update how we're doing on defensive units
void CvMilitaryAI::UpdateDefenseState()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateDefenseState, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	int iLandUnitsNotInArmies;
	int iNavalUnitsNotInArmies;

	// Derive data we'll need
	iLandUnitsNotInArmies = m_iNumLandUnits;
	iNavalUnitsNotInArmies = m_iNumNavalUnits;

	if(iLandUnitsNotInArmies < m_iMandatoryReserveSize)
	{
		m_eLandDefenseState = DEFENSE_STATE_CRITICAL;
	}
	else if(iLandUnitsNotInArmies < m_iRecommendedMilitarySize)
	{
		m_eLandDefenseState = DEFENSE_STATE_NEEDED;
	}
	else if(iLandUnitsNotInArmies < m_iRecommendedMilitarySize * 5 / 4)
	{
		m_eLandDefenseState = DEFENSE_STATE_NEUTRAL;
	}
	else
	{
		m_eLandDefenseState = DEFENSE_STATE_ENOUGH;
	}

	int iNavySize = MilitaryAIHelpers::ComputeRecommendedNavySize(m_pPlayer);

	if(iNavalUnitsNotInArmies <= (iNavySize / 2))
	{
		m_eNavalDefenseState = DEFENSE_STATE_CRITICAL;
	}
	else if(iNavalUnitsNotInArmies <= iNavySize)
	{
		m_eNavalDefenseState = DEFENSE_STATE_NEEDED;
	}
	else if(iNavalUnitsNotInArmies <= iNavySize * 5 / 4)
	{
		m_eNavalDefenseState = DEFENSE_STATE_NEUTRAL;
	}
	else
	{
		m_eNavalDefenseState = DEFENSE_STATE_ENOUGH;
	}
}

/// Count up barbarian camps and units visible to us
void CvMilitaryAI::ScanForBarbarians()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("ScanForBarbarians, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	int iPlotLoop;
	CvPlot* pPlot;

	m_iBarbarianCampCount = 0;
	m_iVisibleBarbarianCount = 0;

	TeamTypes eTeam = m_pPlayer->getTeam();

	// Look at revealed Barbarian camps and visible units
	for(iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		if(pPlot->isRevealed(eTeam))
		{
			if(pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				m_iBarbarianCampCount++;

				// Count it as 5 camps if sitting inside our territory, that is annoying!
				if(pPlot->getOwner() == m_pPlayer->GetID())
				{
					m_iBarbarianCampCount += 4;
				}
			}
		}

		if(pPlot->isVisible(eTeam))
		{
			if(pPlot->plotCount(NULL, -1, -1, BARBARIAN_PLAYER, BARBARIAN_TEAM))
			{
				m_iVisibleBarbarianCount++;
			}
		}
	}
}

/// See if the threats we are facing have changed
void CvMilitaryAI::UpdateThreats()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateThreats, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	ThreatTypes eMilitaryThreatType;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
		if(eLoopPlayer != m_pPlayer->GetID() && m_pDiplomacyAI->IsPlayerValid(eLoopPlayer))
		{
			eMilitaryThreatType = m_pDiplomacyAI->GetMilitaryThreat(eLoopPlayer);
			if(eMilitaryThreatType != m_paeLastTurnMilitaryThreat[eLoopPlayer])
			{
				// Change in threat level
				if(eMilitaryThreatType > m_paeLastTurnMilitaryThreat[eLoopPlayer])
				{
					ThreatIncrease(eMilitaryThreatType, (ThreatTypes)m_paeLastTurnMilitaryThreat[eLoopPlayer]);
				}
				else
				{
					ThreatDecrease(eMilitaryThreatType, (ThreatTypes)m_paeLastTurnMilitaryThreat[eLoopPlayer]);
				}

				// Save off new value
				m_paeLastTurnMilitaryThreat[eLoopPlayer] = eMilitaryThreatType;
			}
		}
	}
}

/// Respond to an increased threat
void CvMilitaryAI::ThreatIncrease(ThreatTypes eNewThreat, ThreatTypes eOldThreat)
{
	// Subtract off old threat
	m_iTotalThreatWeight -= GetThreatWeight(eOldThreat);

	// Add on new threat
	m_iTotalThreatWeight += GetThreatWeight(eNewThreat);

	// FUTURE: Add anything else we'd like to do to individually handle new threats from a single power
}

/// React to a decreased in threat
void CvMilitaryAI::ThreatDecrease(ThreatTypes eNewThreat, ThreatTypes eOldThreat)
{
	// Subtract off old threat
	m_iTotalThreatWeight -= GetThreatWeight(eOldThreat);

	// Add on new threat
	m_iTotalThreatWeight += GetThreatWeight(eNewThreat);

	// FUTURE: Add anything else we'd like to do to individually handle diminishing threats from a single power
}

/// See if the wars we are fighting have changed status
void CvMilitaryAI::UpdateWars()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateWars, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	WarStateTypes eWarState;

	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
		if(eLoopPlayer != m_pPlayer->GetID() && m_pDiplomacyAI->IsPlayerValid(eLoopPlayer))
		{
			eWarState = m_pDiplomacyAI->GetWarState(eLoopPlayer);
			if(eWarState != m_paeLastTurnWarState[eLoopPlayer])
			{
				WarStateChange(eLoopPlayer, eWarState, (WarStateTypes)m_paeLastTurnWarState[eLoopPlayer]);

				// Save off new value
				m_paeLastTurnWarState[eLoopPlayer] = eWarState;
			}
		}
	}
}

/// Respond to a change in war state
void CvMilitaryAI::WarStateChange(PlayerTypes ePlayer, WarStateTypes eNewWarState, WarStateTypes eOldWarState)
{
	LogWarStateChange(ePlayer, eNewWarState, eOldWarState);
}

/// Start or stop military strategies to get flavors set properly
void CvMilitaryAI::UpdateMilitaryStrategies()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateMilitaryStrategies, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	int iStrategiesLoop = 0;

	// Loop through all strategies
	for(iStrategiesLoop = 0; iStrategiesLoop < GetMilitaryAIStrategies()->GetNumMilitaryAIStrategies(); iStrategiesLoop++)
	{
		MilitaryAIStrategyTypes eStrategy = (MilitaryAIStrategyTypes) iStrategiesLoop;
		CvMilitaryAIStrategyXMLEntry* pStrategy = GetMilitaryAIStrategies()->GetEntry(iStrategiesLoop);

		if(pStrategy == NULL)	// Can have holes in the list
			continue;

		// Minor Civs can't run some Strategies
		if(GetPlayer()->isMinorCiv() && pStrategy->IsNoMinorCivs())
			continue;

		// Some strategies ONLY for Minor Civs
		if(!GetPlayer()->isMinorCiv() && pStrategy->IsOnlyMinorCivs())
			continue;

		bool bTestStrategyStart = true;

		// Do we already have this Strategy adopted?
		if(IsUsingStrategy(eStrategy))
			bTestStrategyStart = false;

		else
		{
			// Has the prereq Tech necessary?
			if(pStrategy->GetTechPrereq() != NO_TECH && !GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechPrereq()))
			{
				bTestStrategyStart = false;
			}

			// Has the Tech which obsoletes this Strategy?
			if(bTestStrategyStart && pStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechObsolete()))
			{
				bTestStrategyStart = false;
			}

			// Not time to check this yet?
			if(GC.getGame().getGameTurn() < pStrategy->GetFirstTurnExecuted())
			{
				bTestStrategyStart = false;
			}
		}

		bool bTestStrategyEnd = false;

		// Strategy is active, check to see if we should even try to disable it
		if(IsUsingStrategy(eStrategy))
		{
			if(pStrategy->GetCheckTriggerTurnCount() > 0)
			{
				// Is it a turn where we want to check to see if this Strategy is maintained?
				if((GC.getGame().getGameTurn() - GetTurnStrategyAdopted(eStrategy)) % pStrategy->GetCheckTriggerTurnCount() == 0)
					bTestStrategyEnd = true;
			}

			if(bTestStrategyEnd && pStrategy->GetMinimumNumTurnsExecuted() > 0)
			{
				// Has the minimum # of turns passed for this Strategy?
				if(GC.getGame().getGameTurn() < GetTurnStrategyAdopted(eStrategy) + pStrategy->GetMinimumNumTurnsExecuted())
					bTestStrategyEnd = false;
			}
		}

		// Check Strategy Triggers
		// Functionality and existence of specific Strategies is hardcoded here, but data is stored in XML so it's easier to modify

		if(bTestStrategyStart || bTestStrategyEnd)
		{
			bool bStrategyShouldBeActive = false;

			// Has the Tech which obsoletes this Strategy? If so, Strategy should be deactivated regardless of other factors
			if(pStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechObsolete()))
			{
				bStrategyShouldBeActive = false;
			}

			// Strategy isn't obsolete, so test triggers as normal
			else
			{
				CvString strStrategyName = (CvString) pStrategy->GetType();

				// Check all of the Strategy Triggers
				if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_MILITARY_UNITS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughMilitaryUnits(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_EMPIRE_DEFENSE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EmpireDefense(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EmpireDefenseCritical(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_NAVAL_UNITS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughNavalUnits(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_NAVAL_UNITS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedNavalUnits(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_NAVAL_UNITS_CRITICAL")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedNavalUnitsCritical(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_WAR_MOBILIZATION")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_WarMobilization(eStrategy, m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_AT_WAR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_AtWar(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_MINOR_CIV_GENERAL_DEFENSE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_MinorCivGeneralDefense();
				else if(strStrategyName == "MILITARYAISTRATEGY_MINOR_CIV_THREAT_ELEVATED")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_MinorCivThreatElevated(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_MINOR_CIV_THREAT_CRITICAL")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_MinorCivThreatCritical(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_ERADICATE_BARBARIANS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EradicateBarbarians(eStrategy, m_pPlayer, m_iBarbarianCampCount, m_iVisibleBarbarianCount);
				else if(strStrategyName == "MILITARYAISTRATEGY_WINNING_WARS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_WinningWars(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_LOSING_WARS")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_LosingWars(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_RANGED")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedRangedUnits(m_pPlayer, m_iNumRangedLandUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_RANGED")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughRangedUnits(m_pPlayer, m_iNumRangedLandUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_RANGED_EARLY")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedRangedDueToEarlySneakAttack(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_MOBILE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedMobileUnits(m_pPlayer, m_iNumMobileLandUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_MOBILE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughMobileUnits(m_pPlayer, m_iNumMobileLandUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_AIR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedAirUnits(m_pPlayer, m_iNumAirUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_AIR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughAirUnits(m_pPlayer, m_iNumAirUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_NUKE")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedANuke(m_pPlayer);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_ANTIAIR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedAntiAirUnits(m_pPlayer, m_iNumAntiAirUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_ANTIAIR")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughAntiAirUnits(m_pPlayer, m_iNumAntiAirUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_AIR_CARRIER")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedAirCarriers(m_pPlayer);

			}

			// This variable keeps track of whether or not we should be doing something (i.e. Strategy is active now but should be turned off, OR Strategy is inactive and should be enabled)
			bool bAdoptOrEndStrategy = false;

			// Strategy should be on, and if it's not, turn it on
			if(bStrategyShouldBeActive)
			{
				if(bTestStrategyStart)
					bAdoptOrEndStrategy = true;
				else if(bTestStrategyEnd)
					bAdoptOrEndStrategy = false;
			}
			// Strategy should be off, and if it's not, turn it off
			else
			{
				if(bTestStrategyStart)
					bAdoptOrEndStrategy = false;
				else if(bTestStrategyEnd)
					bAdoptOrEndStrategy = true;
			}

			// Flavor propagation
			if(bAdoptOrEndStrategy)
			{
				int iFlavorLoop;

				// We should adopt this Strategy
				if(bTestStrategyStart)
				{
					SetUsingStrategy(eStrategy, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = pStrategy->GetPlayerFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = pStrategy->GetCityFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, false);

					if(pStrategy->RequiresCitySpecializationUpdate())
						GetPlayer()->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_STRATEGY_NOW_ON);
				}
				// End the Strategy
				else if(bTestStrategyEnd)
				{
					SetUsingStrategy(eStrategy, false);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = -pStrategy->GetPlayerFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = -pStrategy->GetCityFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, false);

					if(pStrategy->RequiresCitySpecializationUpdate())
						GetPlayer()->GetCitySpecializationAI()->SetSpecializationsDirty(SPECIALIZATION_UPDATE_STRATEGY_NOW_OFF);
				}
			}
		}
	}
}

/// Abort or start operations as appropriate given the current threats and war states
void CvMilitaryAI::UpdateOperations()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateOperations, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	int iOperationID;
	int iNumRequiredSlots;
	int iLandReservesUsed;
	int iFilledSlots;

	int iPlayerLoop;
	PlayerTypes eLoopPlayer;
	WarStateTypes eWarState;
	CvAIOperation* pOperation;

	MilitaryAIStrategyTypes eStrategyBarbs = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
	MilitaryAIStrategyTypes eStrategyFightAWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL");

	// SEE IF THERE ARE OPERATIONS THAT NEED TO BE ABORTED

	// Are we willing to risk pressing forward vs. barbarians?
	bool bWillingToAcceptRisk = (m_iTotalThreatWeight / 2) < GetBarbarianThreatTotal();
	if(m_pPlayer->GetPlayerTraits()->GetLandBarbarianConversionPercent() > 0)
	{
		bWillingToAcceptRisk = true;
	}

	// if they have one of our civilians
	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	CvUnit* pLoopUnit = NULL;
	int iBarbLoop;
	for (pLoopUnit = BarbPlayer.firstUnit(&iBarbLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iBarbLoop))
	{
		if (pLoopUnit->GetOriginalOwner() == m_pPlayer->GetID() && (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER))
		{
			bWillingToAcceptRisk = true;
			break;
		}
	}

	//
	// Operations vs. Barbarians
	//
	// If have aborted the eradicate barbarian strategy or if the threat level from civs is significantly higher than from barbs, we better abort all of them
	if(!IsUsingStrategy(eStrategyBarbs) || IsUsingStrategy(eStrategyFightAWar) || !bWillingToAcceptRisk)
	{
		bool bFoundOneToDelete = true;
		while(bFoundOneToDelete)
		{
			bFoundOneToDelete = false;
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_DESTROY_BARBARIAN_CAMP, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
		}
	}

	//
	// Operation vs. Other Civs
	//
	// Are our wars over?
	if(!IsUsingStrategy(eStrategyFightAWar))
	{
		bool bFoundOneToDelete = true;
		while(bFoundOneToDelete)
		{
			bFoundOneToDelete = false;
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
		}
	}

	else
	{
		// Are any of our strategies inappropriate given the type of war we are fighting
		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			// Is this a player we have relations with?
			if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				// If we've made peace with this player, abort all operations related to him
				// slewis - added the check for STATE_ALL_WARS_LOSING so that if the player is losing all wars, that they will cancel scheduled attacks
				if(GET_TEAM(m_pPlayer->getTeam()).isForcePeace(GET_PLAYER(eLoopPlayer).getTeam()) || m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
				{
					pOperation = GetSneakAttackOperation(eLoopPlayer);
					if(pOperation)
					{
						pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					pOperation = GetBasicAttackOperation(eLoopPlayer);
					if(pOperation)
					{
						pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					pOperation = GetShowOfForceOperation(eLoopPlayer);
					if(pOperation)
					{
						pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					pOperation = GetPureNavalAttackOperation(eLoopPlayer);
					if(pOperation)
					{
						pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
				}

				eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);
				switch(eWarState)
				{
					// If we are dominant, shouldn't be running a defensive strategy
				case WAR_STATE_NEARLY_WON:
				case WAR_STATE_OFFENSIVE:
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					break;

					// If we are losing, make sure attacks are not running
				case WAR_STATE_DEFENSIVE:
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					break;

					// If nearly defeated, call off all operations in enemy territory
				case WAR_STATE_NEARLY_DEFEATED:
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation->GetEnemy() == eLoopPlayer)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					break;

				case WAR_STATE_CALM:
				case WAR_STATE_STALEMATE:
				default:
					break;
				}
			}
		}

		// Are there city defense operations for cities that no longer need defending?
		CvCity* pLoopCity;
		int iLoop;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(pLoopCity->getThreatValue() == 0)
			{
				if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID, NO_PLAYER, pLoopCity->plot()))
				{
					pOperation = m_pPlayer->getAIOperation(iOperationID);
					pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
				}
			}
		}

		// Are we running a rapid response tactic and the overall threat level is very low?
		if(m_iTotalThreatWeight <= GC.getAI_MILITARY_THREAT_WEIGHT_MAJOR())
		{
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID))
			{
				pOperation = m_pPlayer->getAIOperation(iOperationID);
				pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
			}
		}
	}

	// SEE WHAT OPERATIONS WE SHOULD ADD
	//
	// Operation vs. Barbarians
	//
	// If running the eradicate barbarian strategy, the threat is low (no higher than 1 major threat), we're not at war, /*and we have enough units*/, then launch a new operation.
	// Which one is based on whether or not we saw any barbarian camps
	if(IsUsingStrategy(eStrategyBarbs) && !IsUsingStrategy(eStrategyFightAWar) && !IsUsingStrategy(eBuildCriticalDefenses) && !m_pPlayer->haveAIOperationOfType(AI_OPERATION_DESTROY_BARBARIAN_CAMP)
	        && bWillingToAcceptRisk)
	{
		// We should have AI build for this
		m_pPlayer->addAIOperation(AI_OPERATION_DESTROY_BARBARIAN_CAMP);
	}

	//
	// Operation vs. Other Civs
	//
	// If at war, consider launching an operation
	if(IsUsingStrategy(eStrategyFightAWar))
	{
		// check nuke launches
		// Loop through each enemy
		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			// Is this a player we have relations with?
			if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				bool bLaunchNuke = false;
				// only evaluate nukes when we have nukes and we've declared war on someone
				if (m_pPlayer->getNumNukeUnits() > 0 && GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam())) 
				{
					// they nuked us, so we can nuke them.
					if (m_pPlayer->GetDiplomacyAI()->GetNumTimesNuked(eLoopPlayer) > 0)
					{	
						bLaunchNuke = true;
					}
					// if we already nuked them, uhhh, keep it up!
					else if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->GetNumTimesNuked(m_pPlayer->GetID()) > 0)
					{
						bLaunchNuke = true;
					}
					else 
					{
						bool bRollForNuke = false;
						WarProjectionTypes eLastWarProjection = m_pPlayer->GetDiplomacyAI()->GetLastWarProjection(eLoopPlayer);
						WarProjectionTypes eCurrentWarProjection = m_pPlayer->GetDiplomacyAI()->GetWarProjection(eLoopPlayer);
						if (eCurrentWarProjection == WAR_PROJECTION_DESTRUCTION)
						{
							// roll every turn
							bRollForNuke = true;
						}
						else if (eCurrentWarProjection != WAR_PROJECTION_UNKNOWN && eCurrentWarProjection != NO_WAR_PROJECTION_TYPE && 
							eLastWarProjection != WAR_PROJECTION_UNKNOWN && eLastWarProjection != NO_WAR_PROJECTION_TYPE && 
							eCurrentWarProjection < eLastWarProjection)
						{
							// roll for nukes!
							bRollForNuke = true;
						}

						if (bRollForNuke)
						{
							int iFlavorNuke = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_USE_NUKE"));
							int iRoll  = GC.getGame().getJonRandNum(10, "Roll to see if we're going to nuke!");
							int iRoll2 = GC.getGame().getJonRandNum(10, "Second roll to see if we're going to nuke!");
							if (iRoll < iFlavorNuke && iRoll2 < iFlavorNuke)
							{
								bLaunchNuke = true;
							}
						}
					}
				}

				if (bLaunchNuke)
				{
					RequestNukeAttack(eLoopPlayer);
				}

			}
		}

		// if we're not considered to be losing all wars, then consider launching operations against other civs
		if (m_pPlayer->GetDiplomacyAI()->GetStateAllWars() != STATE_ALL_WARS_LOSING)
		{
			CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true> weightedTargetList;

			// make list of scores for each player
			for (iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (eLoopPlayer == m_pPlayer->GetID() || !m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
				{
					continue;
				}

				eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);

				// if we're not at war
				if (eWarState == NO_WAR_STATE_TYPE)
				{
					continue;
				}

				// don't declare any operations if we are in a defensive or nearly defeated state
				if (eWarState == WAR_STATE_DEFENSIVE || eWarState == WAR_STATE_NEARLY_DEFEATED)
				{
					continue;
				}

				bool bRequestAttack = false;

				StrengthTypes eMilitaryStrength = m_pPlayer->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eLoopPlayer);
				TargetValueTypes eTargetType = NO_TARGET_VALUE;

				if(eMilitaryStrength <= STRENGTH_AVERAGE || (GC.getAI_MILITARY_IGNORE_BAD_ODDS() == 1))
				{
					eTargetType = m_pPlayer->GetDiplomacyAI()->GetPlayerTargetValue(eLoopPlayer);
					if(eTargetType > TARGET_VALUE_IMPOSSIBLE)
					{
						bRequestAttack = true;
					}
				}

				if (!bRequestAttack)
				{
					continue;
				}

				CvMilitaryTarget target;
				int iScore;
				target = FindBestAttackTarget(AI_OPERATION_BASIC_CITY_ATTACK, eLoopPlayer, &iScore);
				if(target.m_pTargetCity)
				{
					int iNumUnitsWillingBuild = 1;
					if(target.m_bAttackBySea)
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_INVASION, true, &iNumRequiredSlots, &iLandReservesUsed);
						if((iNumRequiredSlots - iFilledSlots) > iNumUnitsWillingBuild || iLandReservesUsed > GetLandReservesAvailable())
						{
							continue;
						}
					}
					else
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, (GC.getGame().getHandicapInfo().GetID() > 4) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, false, &iNumRequiredSlots, &iLandReservesUsed);
						if((iNumRequiredSlots - iFilledSlots) > iNumUnitsWillingBuild || iLandReservesUsed > GetLandReservesAvailable())
						{
							continue;
						}
					}
				}
				else
				{
					continue;
				}

				// add logic in here for current war state
				int iAdjustedByTargetValueScore = iScore * eTargetType;
				weightedTargetList.push_back(target, iAdjustedByTargetValueScore);
			}

			// roll from list of players
			// make sure the list has something in it
			if(weightedTargetList.size() != 0)
			{
				weightedTargetList.SortItems();
				LogAttackTargets(AI_OPERATION_BASIC_CITY_ATTACK, m_pPlayer->GetID(), weightedTargetList);

				if(weightedTargetList.GetTotalWeight() > 0)
				{
					RandomNumberDelegate fcn;
					fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
					CvMilitaryTarget chosenTarget = weightedTargetList.ChooseByWeight(&fcn, "Choosing attack target by weight");

					// declare that attack
					RequestSpecificAttack(chosenTarget, 1);

					LogChosenTarget(AI_OPERATION_BASIC_CITY_ATTACK, m_pPlayer->GetID(), chosenTarget);
				}
			}

			/*
			for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				eLoopPlayer = (PlayerTypes) iPlayerLoop;

				// Is this a player we have relations with?
				if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
				{
					eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);

					switch(eWarState)
					{
						// If roughly equal in number, let's try to annoy him with raids
					case WAR_STATE_STALEMATE:
						//iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_FAST_PILLAGERS, false, &iNumRequiredSlots, &iLandReservesUsed);

						//// Not willing to build units to get this off the ground
						//if(iFilledSlots >= iNumRequiredSlots && iLandReservesUsed <= GetLandReservesAvailable())
						//{
						//	m_pPlayer->addAIOperation(AI_OPERATION_PILLAGE_ENEMY, eLoopPlayer);
						//}
						//break;

						// If we are dominant, time to take down one of his cities
					case WAR_STATE_NEARLY_WON:
					case WAR_STATE_OFFENSIVE:
						RequestBasicAttack(eLoopPlayer, 1);
						break;

						// No one near the other, let's look at a strength comparison before deciding what to do
					case WAR_STATE_CALM:
						{
							bool bRequestAttack = false;

							// slewis - add callbacks here to see if we need to attack anyways
							StrengthTypes eMilitaryStrength = m_pPlayer->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eLoopPlayer);

							bool bAttackAnyway = true;

							if(eMilitaryStrength <= STRENGTH_AVERAGE || bAttackAnyway)
							{
								TargetValueTypes eTargetType = m_pPlayer->GetDiplomacyAI()->GetPlayerTargetValue(eLoopPlayer);
								if(eTargetType > TARGET_VALUE_IMPOSSIBLE)
								{
									bRequestAttack = true;
								}
							}

							if(bRequestAttack)
							{
								RequestBasicAttack(eLoopPlayer, 1);
							}
							//else
							//{
							//	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_FAST_PILLAGERS, false, &iNumRequiredSlots, &iLandReservesUsed);

							//	// Not willing to build units to get this off the ground
							//	if(iFilledSlots >= iNumRequiredSlots && iLandReservesUsed <= GetLandReservesAvailable())
							//	{
							//		m_pPlayer->addAIOperation(AI_OPERATION_PILLAGE_ENEMY, eLoopPlayer);
							//	}
							//}
						}

					case WAR_STATE_DEFENSIVE:
					case WAR_STATE_NEARLY_DEFEATED:
						{
							//CvCity *pThreatenedCity;
							//int iOperationID;

							//// Start by seeing if we have a city that needs defense
							//for (int iI = 0; iI < m_pPlayer->getNumCities(); iI++)
							//{
							//	pThreatenedCity = GetMostThreatenedCity(iI);
							//	if (pThreatenedCity == NULL)
							//	{
							//		break;
							//	}

							//	// Do we already have an operation protecting this city?
							//	if (!m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID, NO_PLAYER, pThreatenedCity->plot()))
							//	{
							//		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_CLOSE_CITY_DEFENSE, false, &iNumRequiredSlots);

							//		// Not willing to build units to get this off the ground
							//		if (iFilledSlots >= iNumRequiredSlots)
							//		{
							//			m_pPlayer->addAIOperation(AI_OPERATION_CITY_CLOSE_DEFENSE, eLoopPlayer);
							//		}
							//	}
							//}

							//// With any extra defensive bandwidth let's add rapid response forces
							//iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_RAPID_RESPONSE_FORCE, false, &iNumRequiredSlots);

							//// Not willing to build units to get this off the ground
							//if (iFilledSlots >= iNumRequiredSlots)
							//{
							//	m_pPlayer->addAIOperation(AI_OPERATION_RAPID_RESPONSE, eLoopPlayer);
							//}
						}
						break;

					default:
						break;
					}
				}
			}*/
		}
	
		// naval attack
		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			// Is this a player we have relations with?
			if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);

				// if we're not at war
				if (eWarState == NO_WAR_STATE_TYPE)
				{
					continue;
				}

				// Naval operations as part of a war effort, but only one at a time
				bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, &iOperationID, NO_PLAYER);
				if (!bHasOperationUnderway)
				{
					RequestPureNavalAttack(eLoopPlayer, 1);
				}
			}
		}
	}

	//
	// Naval operations (vs. opportunity targets)
	//
	// Total number of these operations can't exceed (FLAVOR_NAVAL / 2)
	int iFlavorNaval = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
	int iNumSuperiority = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_SUPERIORITY);
	int iNumBombard = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_BOMBARDMENT);
	int iMaxOperations = iFlavorNaval / 2;
	if(GC.getMap().GetAIMapHint() & 1)
	{
		iMaxOperations *= 2;
	}

	if((iNumSuperiority + iNumBombard) <= iMaxOperations)
	{
		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_SQUADRON, true, &iNumRequiredSlots);

		// Not willing to build units to get this off the ground
		if(iFilledSlots >= iNumRequiredSlots)
		{
			// If I have a colonization operation underway, start up naval superiority as extra escorts
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_COLONIZE, &iOperationID))
			{
				m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, NO_PLAYER);
				return;
			}
		}

		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_BOMBARDMENT, true, &iNumRequiredSlots);

		// Not willing to build units to get this off the ground
		if(iFilledSlots >= iNumRequiredSlots)
		{
			// If fighting off barbarians, start naval bombardment, but only if don't have one
			if(iNumBombard == 0 && IsUsingStrategy(eStrategyBarbs))
			{
				m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_BOMBARDMENT, NO_PLAYER);
				return;
			}
		}
	}
}

/// Spend money on units/buildings for military contingencies
//  NOTE: The defensive side of this is done in dominance zone processing in the Tactical AI; this is spending to speed offensive operations
void CvMilitaryAI::MakeEmergencyPurchases()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("MakeEmergencyPurchases, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	CvAIOperation* nextOp;

	// Are we winning all the wars we are in?
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(!IsUsingStrategy(eStrategyAtWar) || m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		// Is there an operation waiting for one more unit?
		nextOp = m_pPlayer->getFirstAIOperation();
		while(nextOp != NULL)
		{
			if(nextOp->HasOneMoreSlotToFill())
			{
				// Can we buy a unit to fill that slot?
				if(nextOp->BuyFinalUnit())
				{
					break;
				}
			}
			nextOp = m_pPlayer->getNextAIOperation();
		}
	}
}

void CvMilitaryAI::RequestImprovements()
{

}

/// Delete older units no longer needed by military AI
void CvMilitaryAI::DisbandObsoleteUnits()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("DisbandObsoleteUnits, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	bool bInDeficit = false;
	bool bConquestGrandStrategy = false;
	UnitHandle pNavalUnit;
	UnitHandle pLandUnit;
	int iNavalScore = MAX_INT;
	int iLandScore = MAX_INT;

	// Don't do this if at war
	if(GetNumberCivsAtWarWith() > 0)
	{
		return;
	}

	// Don't do this if we're a minor civ
	if(m_pPlayer->isMinorCiv())
	{
		return;
	}

	// Are we running at a deficit?
	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	bInDeficit = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);

	int iGoldSpentOnUnits = m_pPlayer->GetTreasury()->GetExpensePerTurnUnitMaintenance();
	int iAverageGoldPerUnit = iGoldSpentOnUnits / (max(1,m_pPlayer->getNumUnits()));

	// if our units maintenance cost is high we may want to scrap some obsolete stuff
	bInDeficit = bInDeficit || iAverageGoldPerUnit > 5;

	// Are we running anything other than the Conquest Grand Strategy?
	AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
	if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eConquestGrandStrategy)
		{
			bConquestGrandStrategy = true;
		}
	}

	// Look for obsolete land units if in deficit or have sufficient units
	if(bInDeficit || (m_eLandDefenseState <= DEFENSE_STATE_NEUTRAL && !bConquestGrandStrategy))
	{
		pLandUnit = FindBestUnitToScrap(true /*bLand*/, false /*bDeficitForcedDisband*/, iLandScore);
	}

	// Look for obsolete naval units if in deficit or have sufficient units
	if(bInDeficit || (m_eNavalDefenseState <= DEFENSE_STATE_NEUTRAL && !bConquestGrandStrategy))
	{
		pNavalUnit = FindBestUnitToScrap(false/*bNaval*/, false /*bDeficitForcedDisband*/, iNavalScore);
	}

	if(iLandScore < MAX_INT && (m_eLandDefenseState <= m_eNavalDefenseState || iLandScore <= iNavalScore))
	{
		if(pLandUnit)
		{
			pLandUnit->scrap();
			LogScrapUnit(pLandUnit, bInDeficit, bConquestGrandStrategy);
		}
	}
	else if(iNavalScore < MAX_INT)
	{
		if(pNavalUnit)
		{
			pNavalUnit->scrap();
			LogScrapUnit(pNavalUnit, bInDeficit, bConquestGrandStrategy);
		}
	}
}

/// Do we have the forces at hand for an attack?
bool CvMilitaryAI::IsAttackReady(MultiunitFormationTypes eFormation, AIOperationTypes eOperationType) const
{
	int iOperationID;

	// Do we already have an operation of this type that is building units?
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(eOperationType, &iOperationID, NO_PLAYER);
	if (bHasOperationUnderway)
	{
		if (m_pPlayer->getAIOperation(iOperationID)->GetOperationState() != AI_OPERATION_STATE_RECRUITING_UNITS)
		{
			return false;
		}
	}

	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;

	bool bRequiresNavalMoves = false;
	if(eFormation == MUFORMATION_NAVAL_INVASION || eFormation == MUFORMATION_NAVAL_SQUADRON || eFormation == MUFORMATION_CITY_STATE_INVASION || eFormation == MUFORMATION_PURE_NAVAL_CITY_ATTACK)
	{
		bRequiresNavalMoves = true;
	}

	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eFormation, bRequiresNavalMoves, &iNumRequiredSlots, &iLandReservesUsed);
	if(iFilledSlots >= iNumRequiredSlots)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/// Score the strength of the units for a domain; best candidate to scrap (with lowest score) is returned. Only supports land and naval units
UnitHandle CvMilitaryAI::FindBestUnitToScrap(bool bLand, bool bDeficitForcedDisband, int& iReturnedScore)
{
	CvUnit* pLoopUnit;
	int iUnitLoop;
	UnitHandle pBestUnit;
	int iScore;
	int iBestScore = MAX_INT;

	for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		bool bSkipThisOne = false;

		if(!pLoopUnit->IsCombatUnit())
		{
			continue;
		}

		if(bLand && pLoopUnit->getDomainType() != DOMAIN_LAND)
		{
			continue;
		}

		if(!bLand && pLoopUnit->getDomainType() != DOMAIN_SEA)
		{
			continue;
		}

		// Following checks are for the case where the AI is trying to decide if it is a good idea to disband this unit (as opposed to when the game is FORCING the player to disband one)
		if(!bDeficitForcedDisband)
		{
			if(bLand && m_eLandDefenseState == DEFENSE_STATE_CRITICAL)
			{
				continue;
			}
			else if(!bLand && m_eNavalDefenseState == DEFENSE_STATE_CRITICAL)
			{
				continue;
			}

			// Is it in an army?
			if(pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
			{
				continue;
			}

			// Can I still build this unit? If so too new to scrap
			if(bLand && m_pPlayer->canTrain(pLoopUnit->getUnitType(), false /*bContinue*/, false /*bTestVisible*/, true /*bIgnoreCost*/))
			{
				continue;
			}

			// Is this a unit who has an obsolete tech that I have researched?
			CvUnitEntry& pUnitInfo = pLoopUnit->getUnitInfo();
			if((TechTypes)pUnitInfo.GetObsoleteTech() == NO_TECH)
			{
				continue;
			}

			if(!GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetObsoleteTech())))
			{
				continue;
			}

			// Is this unit's INTRINSIC power less than half that of the best unit I can build for this domain?
			if((pLoopUnit->getUnitInfo().GetPower() * 2) >= GetPowerOfStrongestBuildableUnit(pLoopUnit->getDomainType()))
			{
				continue;
			}

			// Does this unit's upgrade require a resource?
			UnitTypes eUpgradeUnit = pLoopUnit->GetUpgradeUnitType();
			if(eUpgradeUnit != NO_UNIT)
			{
				CvUnitEntry* pUpgradeUnitInfo = GC.GetGameUnits()->GetEntry(eUpgradeUnit);
				if(pUpgradeUnitInfo != NULL)
				{
					for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos() && !bSkipThisOne; iResourceLoop++)
					{
						ResourceTypes eResource = (ResourceTypes) iResourceLoop;
						int iNumResourceNeeded = pUpgradeUnitInfo->GetResourceQuantityRequirement(eResource);

						if(iNumResourceNeeded > 0)
						{
							if(m_pPlayer->getNumResourceTotal(eResource) > 0)
							{
								if(bLand && m_eLandDefenseState > DEFENSE_STATE_NEUTRAL)
								{
									bSkipThisOne  = true;    // We'll wait and try to upgrade this one, our unit count isn't that bad
								}
								else if(!bLand && m_eNavalDefenseState > DEFENSE_STATE_NEUTRAL)
								{
									bSkipThisOne  = true;    // We'll wait and try to upgrade this one, our unit count isn't that bad
								}
							}
						}
					}
				}
			}
		}

		// Can I scrap this unit?
		if(!bSkipThisOne && pLoopUnit->canScrap())
		{
			iScore = pLoopUnit->GetPower();

			if(iScore < iBestScore)
			{
				iBestScore = iScore;
				iReturnedScore = iBestScore;
				pBestUnit = pLoopUnit;
			}
		}
	}

	return pBestUnit;
}

/// Which unit should be built next for our army
UnitTypes CvMilitaryAI::GetUnitForArmy(CvCity* pCity) const
{
	if(m_eArmyTypeBeingBuilt == NO_ARMY_TYPE)
	{
		return NO_UNIT;
	}

	// Look for required units first
	MultiunitFormationTypes eFormation;
	if(m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION)
	{
		eFormation = MUFORMATION_NAVAL_INVASION;
	}
	else
	{
		eFormation = (GC.getGame().getHandicapInfo().GetID() > 4) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE;
	}
	UnitAITypes eUnitAIType = MilitaryAIHelpers::FirstSlotCityCanFill(m_pPlayer, eFormation, (m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION), pCity->isCoastal(), false /*bSecondaryUnit*/);
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType);
	if(eType == NO_UNIT)
	{
		eUnitAIType = MilitaryAIHelpers::FirstSlotCityCanFill(m_pPlayer, eFormation, (m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION), pCity->isCoastal(), true /*bSecondaryUnit*/);
		eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType);
	}
	return eType;
}

/// Do we want to move this air unit to a new base?
bool CvMilitaryAI::WillAirUnitRebase(CvUnit* pUnit) const
{
	CvPlot* pUnitPlot = pUnit->plot();

	CvPlot* pBestPlot = NULL;

	// Is this unit in a base in danger?
	bool bNeedsToMove = false;
	if (pUnitPlot->isCity())
	{
		if (pUnitPlot->getPlotCity()->getDamage() > (GC.getMAX_CITY_HIT_POINTS() / 5))
		{
			bNeedsToMove = true;
		}
	}
	else
	{
		CvUnit *pCarrier = pUnit->getTransportUnit();
		if (pCarrier)
		{
			if (pCarrier->getDamage() > (GC.getMAX_HIT_POINTS() / 5))
			{
				bNeedsToMove = true;
			}
		}
	}

	// Is this a fighter that doesn't have any useful missions nearby
	if (pUnit->canAirPatrol(NULL) || pUnit->canAirSweep())
	{
		int iNumNearbyEnemyAirUnits = GetNumEnemyAirUnitsInRange(pUnitPlot, pUnit->GetRange(), true /*bCountFighters*/, true /*bCountBombers*/);
		if (iNumNearbyEnemyAirUnits == 0 && !GetBestAirSweepTarget(pUnit))
		{
			bNeedsToMove = true;
		}
	}

	if (!bNeedsToMove)
	{
		return false;
	}

	// first look for open carrier slots
	int iLoopUnit = 0;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		CvPlot* pLoopUnitPlot = pLoopUnit->plot();

		if(pLoopUnit->getDamage() > (GC.getMAX_HIT_POINTS() / 5))  // this might not be a good place to land
		{
			continue;
		}

		if(pBestPlot != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot, pLoopUnitPlot->getX(),pLoopUnitPlot->getY()))
		{
			continue;
		}

		if(!pUnit->canLoadUnit(*pLoopUnit, *pLoopUnitPlot))
		{
			continue;
		}
		
		// Found somewhere to rebase to
		return true;
	}

	CvCity* pLoopCity;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		CvPlot* pTarget = pLoopCity->plot();

		if(pLoopCity->getDamage() > (GC.getMAX_CITY_HIT_POINTS() / 5))
		{
			continue;
		}

		if (pTarget != pUnitPlot && !pUnit->canRebaseAt(pUnitPlot, pTarget->getX(),pTarget->getY()))
		{
			continue;
		}

		// Found somewhere to rebase to
		return true;
	}

	return false;
}

/// Assess nearby enemy air assets
int CvMilitaryAI::GetNumEnemyAirUnitsInRange(CvPlot* pCenterPlot, int iRange, bool bCountFighters, bool bCountBombers) const
{
	int iRtnValue = 0;

	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && kPlayer.GetID() != m_pPlayer->GetID())
		{
			if (atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
			{
				// Loop through each of their cities
				int iLoop;
				CvCity* pLoopCity;
				for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					CvPlot *pCityPlot = pLoopCity->plot();
					if (pCityPlot->isRevealed(m_pPlayer->getTeam()) && plotDistance(pCenterPlot->getX(), pCenterPlot->getY(), pLoopCity->getX(), pLoopCity->getY()) <= iRange)
					{
						for (int iUnitLoop = 0; iUnitLoop < pCityPlot->getNumUnits(); iUnitLoop++)
						{
							CvUnit *pUnit = pCityPlot->getUnitByIndex(iUnitLoop);
							{
								if (pUnit->getDomainType() == DOMAIN_AIR)
								{
									if (pUnit->IsAirSweepCapable() || pUnit->canAirDefend())
									{
										if (bCountFighters) iRtnValue++;
									}
									else
									{
										if (bCountBombers) iRtnValue++;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return iRtnValue;
}

/// See if this fighter has an air sweep target we like
CvPlot *CvMilitaryAI::GetBestAirSweepTarget(CvUnit* pFighter) const
{
	CvPlot *pBestTarget = NULL;
	int iBestCount = 0;

	// Loop through all the players
	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if(kPlayer.isAlive() && kPlayer.GetID() != m_pPlayer->GetID())
		{
			if (atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
			{
				// Loop through each of their cities
				int iLoop;
				CvCity* pLoopCity;
				for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				{
					CvPlot *pCityPlot = pLoopCity->plot();
					if (pCityPlot->isVisible(m_pPlayer->getTeam()) && plotDistance(pFighter->getX(), pFighter->getY(), pLoopCity->getX(), pLoopCity->getY()) <= pFighter->GetRange())
					{
						if (pFighter->canAirSweepAt(pCityPlot->getX(), pCityPlot->getY()))
						{
							int iCountFighters = 0;

							for (int iUnitLoop = 0; iUnitLoop < pCityPlot->getNumUnits(); iUnitLoop++)
							{
								CvUnit *pUnit = pCityPlot->getUnitByIndex(iUnitLoop);
								{
									if (pUnit->getDomainType() == DOMAIN_AIR)
									{
										if (pUnit->IsAirSweepCapable() || pUnit->canAirDefend())
										{
											iCountFighters++;
										}
									}
								}
							}

							if (iCountFighters > iBestCount)
							{
								iBestCount = iCountFighters;
								pBestTarget = pCityPlot;
							}
						}
					}
				}
			}
		}
	}

	return pBestTarget;
}

/// How strong is the best unit we can train for this domain?
int CvMilitaryAI::GetPowerOfStrongestBuildableUnit(DomainTypes eDomain)
{
	int iRtnValue = 0;
	for(int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iI);
		CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
		if(pkUnitEntry != NULL && pkUnitEntry->GetDomainType() == eDomain)
		{
			int iThisPower = pkUnitEntry->GetPower();		// Test the power first, it is much less costly than testing canTrain
			if(iThisPower > iRtnValue)
			{
				if(m_pPlayer->canTrain(eUnit, false /*bContinue*/, false /*bTestVisible*/, true /*bIgnoreCost*/))
				{
					iRtnValue = iThisPower;
				}
			}
		}
	}

	return iRtnValue;
}

/// Log that a strategy is being turned on or off
void CvMilitaryAI::LogStrategy(MilitaryAIStrategyTypes eStrategy, bool bValue)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strLogName;
		FILogFile* pLog;

		// Open the right file - strategies are logged to the flavor log, not the military one
		playerName = GetPlayer()->getCivilizationShortDescription();
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "FlavorAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "FlavorAILog.csv";
		}
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// Strategy Info
		CvMilitaryAIStrategyXMLEntry* pEntry = GC.getMilitaryAIStrategyInfo(eStrategy);
		const char* szType = (pEntry != NULL)? pEntry->GetType() : NULL;

		strTemp.Format("%s, %d", szType, bValue);
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		// Also - write to Other Logs, so that we know what the player is doing in there as well

		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "TechAILog_" + playerName + ".csv";
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);

			CvString cityName;

			CvCity* pLoopCity;
			int iLoop;

			for(pLoopCity = GetPlayer()->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoop))
			{
				cityName = pLoopCity->getName();
				strLogName= "CityStrategyAILog_" + playerName + "_" + cityName + ".csv";
				pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
				pLog->Msg(strOutBuf);
			}
		}
		else
		{
			pLog = LOGFILEMGR.GetLog("CityStrategyAILog.csv", FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
			pLog = LOGFILEMGR.GetLog("TechAILog.csv", FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log some military status info each turn
void CvMilitaryAI::LogMilitaryStatus()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvCity* pCity;
		CvString cityName;
		FILogFile* pLog;

		// Open the right file
		playerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Very first update (to write header row?)
		if(GC.getGame().getGameTurn() == 1 && m_pPlayer->GetID() == 0)
		{
			strTemp.Format("Turn, Player, Cities, Settlers, Civ Threat, Barb Threat, Land Units, Land In Armies, Rec Land Size, Land Reserve, Naval Units, Naval In Armies, Rec Naval Size, Most Threatened, Danger");
			pLog->Msg(strTemp);
		}

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// City info
		strTemp.Format("%d, %d, ", m_pPlayer->getNumCities(), m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true));
		strOutBuf = strBaseString + strTemp;

		//Threat Info
		strTemp.Format("%d, %d, ", m_iTotalThreatWeight, GetBarbarianThreatTotal());
		strOutBuf += strTemp;

		// Military size Info
		int iNavalSize = MilitaryAIHelpers::ComputeRecommendedNavySize(m_pPlayer);
		strTemp.Format("%d, %d, %d, %d, %d, %d, %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecommendedMilitarySize, m_iMandatoryReserveSize, m_iNumNavalUnits, m_iNumNavalUnitsInArmies, iNavalSize);
		strOutBuf += strTemp;

		// Most threatened city
		pCity = GetMostThreatenedCity();
		if(pCity != NULL)
		{
			cityName = pCity->getName();
			strOutBuf += cityName;
			strTemp.Format(", %d", pCity->getThreatValue());
			strOutBuf += strTemp;
		}

		pLog->Msg(strOutBuf);
	}
}

/// Log some military status info each turn
void CvMilitaryAI::LogAvailableForces()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString playerName;
		CvString strTemp;
		FILogFile* pLog;
		int iCapitalX = 0;
		int iCapitalY = 0;
		CvCity* pCapital = GetPlayer()->getCapitalCity();
		if(pCapital)
		{
			iCapitalX = pCapital->getX();
			iCapitalY = pCapital->getY();
		}

		// Open the right file
		playerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		if(m_eArmyTypeBeingBuilt == ARMY_TYPE_LAND)
		{
			strTemp = "Land Army Under Construction";
		}
		else
		{
			strTemp = "Naval Invasion Under Construction";
		}
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		// Loop through our units
		CvUnit* pLoopUnit;
		int iLoop;
		for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
		{
			// Never want immobile/dead units, explorers, ones that have already moved or automated human units
			if(pLoopUnit->TurnProcessed() || pLoopUnit->isDelayedDeath() || pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN ||  pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE || !pLoopUnit->canMove() || pLoopUnit->isHuman())
			{
				continue;
			}

			// Now down to land and sea units ... in these groups our unit must have a base combat strength ... or be a great general/admiral
			else if(!pLoopUnit->IsCombatUnit() && !(pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral()))
			{
				continue;
			}

			// No units finishing up operations
			else if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() >= GC.getGame().getGameTurn())
			{
				continue;
			}

			int iArmyID = pLoopUnit->getArmyID();
			int iDist = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iCapitalX, iCapitalY);
			strTemp.Format("%s, Damage=%d, ArmyID=%d, X=%d, Y=%d, DistFromCap=%d", pLoopUnit->getName().GetCString(), pLoopUnit->getDamage(), iArmyID, pLoopUnit->getX(), pLoopUnit->getY(), iDist);
			strOutBuf = strBaseString + strTemp;

			pLog->Msg(strOutBuf);
		}
	}
}

/// Log some military status info each turn
void CvMilitaryAI::LogWarStateChange(PlayerTypes ePlayer, WarStateTypes eNewWarState, WarStateTypes eOldWarState)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString opponentName;
		FILogFile* pLog;

		// Open the right file
		playerName = GetPlayer()->getCivilizationShortDescription();
		opponentName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", vs. " + opponentName + ", New War State: ";

		// New War State
		switch(eNewWarState)
		{
		case WAR_STATE_CALM:
			strTemp.Format("Calm");
			break;
		case WAR_STATE_NEARLY_WON:
			strTemp.Format("Nearly Won");
			break;
		case WAR_STATE_OFFENSIVE:
			strTemp.Format("Offensive");
			break;
		case WAR_STATE_STALEMATE:
			strTemp.Format("Stalemate");
			break;
		case WAR_STATE_DEFENSIVE:
			strTemp.Format("Defensive");
			break;
		case WAR_STATE_NEARLY_DEFEATED:
			strTemp.Format("Nearly Defeated");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
		strOutBuf = strBaseString + strTemp + ", Old War State: ";

		// Old War State
		switch(eOldWarState)
		{
		case WAR_STATE_CALM:
			strTemp.Format("Calm");
			break;
		case WAR_STATE_NEARLY_WON:
			strTemp.Format("Nearly Won");
			break;
		case WAR_STATE_OFFENSIVE:
			strTemp.Format("Offensive");
			break;
		case WAR_STATE_STALEMATE:
			strTemp.Format("Stalemate");
			break;
		case WAR_STATE_DEFENSIVE:
			strTemp.Format("Defensive");
			break;
		case WAR_STATE_NEARLY_DEFEATED:
			strTemp.Format("Nearly Defeated");
			break;
		default:
			strTemp.Format("XXX");
			break;
		}
		strOutBuf += strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Log that a unit is being scrapped
void CvMilitaryAI::LogScrapUnit(UnitHandle pUnit, bool bDeficit, bool bConquest)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strTemp;
		CvString playerName;
		FILogFile* pLog;

		// Open the right file
		playerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		strOutBuf.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strOutBuf += playerName + ", ";

		strTemp.Format("Scrapping %s, X: %d, Y: %d, ", pUnit->getUnitInfo().GetDescription(), pUnit->getX(), pUnit->getY());
		strOutBuf += strTemp;
		if(bDeficit)
		{
			strOutBuf += "IN DEFICIT, ";
		}
		else
		{
			strOutBuf += "Finances ok, ";
		}
		if(bConquest)
		{
			strOutBuf += "CONQUEST, ";
		}
		else
		{
			strOutBuf += "Other GS, ";
		}
		if(pUnit->getDomainType() == DOMAIN_LAND)
		{
			strTemp.Format("Num Land Units: %d, In Armies %d, Rec Size: %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecommendedMilitarySize);
		}
		else
		{
			strTemp.Format("Num Naval Units: %d, In Armies %d, ", m_iNumNavalUnits, m_iNumNavalUnitsInArmies);
		}
		strOutBuf += strTemp;
		pLog->Msg(strOutBuf);
	}
}

/// Log a message to the high-level summary log
void CvMilitaryAI::LogMilitarySummaryMessage(const CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName = m_pPlayer->getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName, true/*bSummary*/), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

/// Build log filename
CvString CvMilitaryAI::GetLogFileName(CvString& playerName, bool bSummary) const
{
	CvString strLogName;

	if (bSummary)
	{
		strLogName = "MilitarySummary.csv";
	}
	else
	{
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "MilitaryAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "MilitaryAILog.csv";
		}
	}

	return strLogName;
}

// NON-MEMBER FUNCTIONS
//
// These are functions that do not need access to the internals of the CvMilitaryAI class.
// Keep them as non-member functions to:
// a) simplify the class (improving encapsulation, reducing coupling)
// b) allow their general use by other classes

// Figure out what the WeightThreshold Mod should be by looking at the Flavors for this player & the Strategy
int MilitaryAIHelpers::GetWeightThresholdModifier(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iWeightThresholdModifier = 0;

	CvMilitaryAIStrategyXMLEntries* pkAIStrategies = pPlayer->GetMilitaryAI()->GetMilitaryAIStrategies();
	CvFlavorManager* pkFlavorManager = pPlayer->GetFlavorManager();
	// Look at all Flavors for the Player & this Strategy
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		FlavorTypes eFlavor = (FlavorTypes) iFlavorLoop;
		int iPersonalityFlavor = pkFlavorManager->GetPersonalityIndividualFlavor(eFlavor);
		CvMilitaryAIStrategyXMLEntry* pkEntry = pkAIStrategies->GetEntry(eStrategy);
		CvAssert(pkEntry != NULL);
		if(pkEntry)
		{
			int iStrategyFlavorMod = pkEntry->GetPersonalityFlavorThresholdMod(eFlavor);

			iWeightThresholdModifier += (iPersonalityFlavor * iStrategyFlavorMod);
		}
	}

	return iWeightThresholdModifier;
}

/// "Enough Military Units" Player Strategy: Does this player have too many military units?  If so, adjust flavors
bool MilitaryAIHelpers::IsTestStrategy_EnoughMilitaryUnits(CvPlayer* pPlayer)
{
	// Are we running at a deficit?
	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	bool bInDeficit = pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);

	// Are we running anything other than the Conquest Grand Strategy?
	AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
	if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(bInDeficit || pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() != eConquestGrandStrategy || pPlayer->GetMilitaryAI()->GetPercentOfRecommendedMilitarySize() > 125)
		{
			if(pPlayer->GetMilitaryAI()->GetLandDefenseState() == DEFENSE_STATE_ENOUGH)
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}

	return false;
}

/// "Empire Defense" Player Strategy: Adjusts military flavors if the player doesn't have the recommended number of units
bool MilitaryAIHelpers::IsTestStrategy_EmpireDefense(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetLandDefenseState() == DEFENSE_STATE_NEEDED)
	{
		return true;
	}

	return false;
}

/// "Empire Defense" Player Strategy: If we have less than 1 unit per city (tweaked a bit by threat level), we NEED some units
bool MilitaryAIHelpers::IsTestStrategy_EmpireDefenseCritical(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetLandDefenseState() == DEFENSE_STATE_CRITICAL)
	{
		return true;
	}

	return false;
}

/// "Enough Naval Units" Strategy: build navies
bool MilitaryAIHelpers::IsTestStrategy_EnoughNavalUnits(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetNavalDefenseState() == DEFENSE_STATE_ENOUGH)
	{
		return true;
	}

	return false;
}

/// "Need Naval Units" Strategy: build navies
bool MilitaryAIHelpers::IsTestStrategy_NeedNavalUnits(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetNavalDefenseState() == DEFENSE_STATE_NEEDED)
	{
		return true;
	}

	return false;
}

/// "Need Naval Units Critical" Strategy: build navies NOW
bool MilitaryAIHelpers::IsTestStrategy_NeedNavalUnitsCritical(CvPlayer* pPlayer)
{
	if(pPlayer->GetMilitaryAI()->GetNavalDefenseState() == DEFENSE_STATE_CRITICAL)
	{
		return true;
	}

	return false;
}

/// "War Mobilization" Player Strategy: Does this player want to mobilize for war?  If so, adjust flavors
bool MilitaryAIHelpers::IsTestStrategy_WarMobilization(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iCurrentWeight = 0;
	PlayerTypes eOtherPlayer;

	// If we're at war don't bother with this Strategy
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");

	if(eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return false;
		}
	}

	// Are we running the Conquest Grand Strategy?
	AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
	if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eConquestGrandStrategy)
		{
			iCurrentWeight += 25;
		}
	}

	CvDiplomacyAI* pkDiplomacyAI = pPlayer->GetDiplomacyAI();
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eOtherPlayer = (PlayerTypes) iMajorLoop;

		// Mobilize for war is automatic if we are preparing a sneak attack
		if(pkDiplomacyAI->GetWarGoal(eOtherPlayer) == WAR_GOAL_PREPARE || pPlayer->GetMilitaryAI()->GetArmyBeingBuilt() != NO_ARMY_TYPE)
		{
			iCurrentWeight += 100;
		}

		// Add in weight for each civ we're on really bad terms with
		else if(pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_WAR ||
		        pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_HOSTILE ||
		        pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_AFRAID)
		{
			iCurrentWeight += 50;
		}

		// And some if on fairly bad terms
		// Add in weight for each civ we're on really bad terms with
		else if(pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_GUARDED ||
		        pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_DECEPTIVE)
		{
			iCurrentWeight += 25;
		}
	}

	CvMilitaryAIStrategyXMLEntry* pStrategy = pPlayer->GetMilitaryAI()->GetMilitaryAIStrategies()->GetEntry(eStrategy);
	CvAssert(pStrategy != NULL);
	if(pStrategy)
	{
		int iWeightThreshold = pStrategy->GetWeightThreshold();	// 100
		if(iCurrentWeight >= iWeightThreshold)
		{
			return true;
		}
	}

	return false;
}

/// "At War" Player Strategy: If the player is at war, increase OFFENSE, DEFENSE and MILITARY_TRAINING.  Then look into which operation(s) to run
bool MilitaryAIHelpers::IsTestStrategy_AtWar(CvPlayer* pPlayer)
{
	return (pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith() > 0);
}

/// "Minor Civ GeneralDefense" Player Strategy: Prioritize CITY_DEFENSE and DEFENSE
bool MilitaryAIHelpers::IsTestStrategy_MinorCivGeneralDefense()
{
	return true;
}

/// "Minor Civ Threat Elevated" Player Strategy: If a Minor Civ is in danger, turn CITY_DEFENSE and DEFENSE up
bool MilitaryAIHelpers::IsTestStrategy_MinorCivThreatElevated(CvPlayer* pPlayer)
{
	if(pPlayer->GetMinorCivAI()->GetStatus() == MINOR_CIV_STATUS_ELEVATED)
	{
		return true;
	}

	return false;
}

/// "Minor Civ Threat Critical" Player Strategy: If a Minor Civ is in danger, turn CITY_DEFENSE and DEFENSE up
bool MilitaryAIHelpers::IsTestStrategy_MinorCivThreatCritical(CvPlayer* pPlayer)
{
	if(pPlayer->GetMinorCivAI()->GetStatus() == MINOR_CIV_STATUS_CRITICAL)
	{
		return true;
	}

	return false;
}

/// "Eradicate Barbarians" Player Strategy: If there is a large group of barbarians units or camps near our civilization, increase OFFENSE
bool MilitaryAIHelpers::IsTestStrategy_EradicateBarbarians(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer, int iBarbarianCampCount, int iVisibleBarbarianCount)
{
	int iStrategyWeight;
	PlayerTypes eOtherPlayer;

	// If we're at war don't bother with this Strategy (unless iti s clear we are already winning)
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			if(!pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
			{
				return false;
			}
		}
	}

	// We ARE getting our guys back
	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	CvUnit* pLoopUnit = NULL;
	int iLoop;
	for (pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
	{
		if (pLoopUnit->GetOriginalOwner() == pPlayer->GetID() && (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE || pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER))
		{
			return true;
		}
	}

	CvMilitaryAIStrategyXMLEntry* pStrategy = pPlayer->GetMilitaryAI()->GetMilitaryAIStrategies()->GetEntry(eStrategy);
	CvAssert(pStrategy != NULL);
	if(pStrategy)
	{
		iStrategyWeight = iBarbarianCampCount * 50 + iVisibleBarbarianCount * 25;   // Two visible camps or 3 roving Barbarians will trigger this
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		if(iStrategyWeight >= iWeightThreshold * 2 && iVisibleBarbarianCount > 0) // barbs are a big threat, our sneak attack plan can wait
		{
			return true;
		}
		else if(iStrategyWeight >= iWeightThreshold)
		{
			// Also don't bother if we're building up for a sneak attack
			CvDiplomacyAI* pkDiplomacyAI = pPlayer->GetDiplomacyAI();
			for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
			{
				eOtherPlayer = (PlayerTypes) iMajorLoop;
				if(pkDiplomacyAI->GetWarGoal(eOtherPlayer) == WAR_GOAL_PREPARE)
				{
					return false;
				}
			}

			return true;
		}
		// If we have an operation of this type running, we don't want to turn this strategy off
		else if(pPlayer->haveAIOperationOfType(AI_OPERATION_DESTROY_BARBARIAN_CAMP))
		{
			return true;
		}
	}

	return false;
}

/// "Winning Wars" Strategy: boost OFFENSE over DEFENSE
bool MilitaryAIHelpers::IsTestStrategy_WinningWars(CvPlayer* pPlayer)
{
	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		return true;
	}
	return false;
}

/// "Losing Wars" Strategy: boost DEFENSE over OFFENSE
bool MilitaryAIHelpers::IsTestStrategy_LosingWars(CvPlayer* pPlayer)
{
	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
	{
		return true;
	}
	return false;
}

/// "Enough Ranged" Player Strategy: If a player has too many ranged units
bool MilitaryAIHelpers::IsTestStrategy_EnoughRangedUnits(CvPlayer* pPlayer, int iNumRanged, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RANGED"));
	int iRatio = iNumRanged * 10 / max(1,iNumMelee+iNumRanged);
	return (iRatio >= iFlavorRange);
}

/// "Need Ranged" Player Strategy: If a player has too many melee units
bool MilitaryAIHelpers::IsTestStrategy_NeedRangedUnits(CvPlayer* pPlayer, int iNumRanged, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RANGED"));
	int iRatio = iNumRanged * 10 / max(1,iNumMelee+iNumRanged);
	return (iRatio <= iFlavorRange / 2);
}

/// "Need Ranged Early" Player Strategy: If a player is planning a early sneak attack we need to make sure that ranged are built since this will block this
bool MilitaryAIHelpers::IsTestStrategy_NeedRangedDueToEarlySneakAttack(CvPlayer* pPlayer)
{
	MilitaryAIStrategyTypes eStrategyWarMob = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
	if(eStrategyWarMob != NO_MILITARYAISTRATEGY)
	{
		if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyWarMob))
		{
			return true;
		}
	}
	return false;
}

/// "Enough Mobile" Player Strategy: If a player has too many mobile units
bool MilitaryAIHelpers::IsTestStrategy_EnoughMobileUnits(CvPlayer* pPlayer, int iNumMobile, int iNumMelee)
{
	int iFlavorMobile = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_MOBILE"));
	int iRatio = iNumMobile * 10 / max(1,iNumMelee+iNumMobile);
	return (iRatio >= iFlavorMobile);
}

/// "Need Mobile" Player Strategy: If a player has too many slow units
bool MilitaryAIHelpers::IsTestStrategy_NeedMobileUnits(CvPlayer* pPlayer, int iNumMobile, int iNumMelee)
{
	int iFlavorMobile = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_MOBILE"));
	int iRatio = iNumMobile * 10 / max(1,iNumMelee+iNumMobile);
	return (iRatio <= iFlavorMobile / 2);
}

/// "Enough Air" Player Strategy: If a player has too many air units
bool MilitaryAIHelpers::IsTestStrategy_EnoughAirUnits(CvPlayer* pPlayer, int iNumAir, int iNumMelee)
{
	int iFlavorAir = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_AIR"));
	int iRatio = iNumAir * 10 / max(1,iNumMelee+iNumAir);
	return (iRatio >= iFlavorAir);
}

/// "Need Air" Player Strategy: If a player has too few flying units
bool MilitaryAIHelpers::IsTestStrategy_NeedAirUnits(CvPlayer* pPlayer, int iNumAir, int iNumMelee)
{
	int iFlavorAir = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_AIR"));
	int iRatio = iNumAir * 10 / max(1,iNumMelee+iNumAir);
	return (iRatio <= iFlavorAir / 2);
}

/// "Need A Nuke" Player Strategy: If a player has no nukes but he could
bool MilitaryAIHelpers::IsTestStrategy_NeedANuke(CvPlayer* pPlayer)
{
	if(GC.getGame().isNoNukes())
	{
		return false;
	}

	int iFlavorNuke = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NUKE"));
	int iNumNukes = pPlayer->getNumNukeUnits();

	return (iNumNukes < iFlavorNuke / 3);
}

/// "Enough Anti-Air" Player Strategy: If a player has too many AA units
bool MilitaryAIHelpers::IsTestStrategy_EnoughAntiAirUnits(CvPlayer* pPlayer, int iNumAA, int iNumMelee)
{
	bool bAnyAirforce = false;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			if(GET_PLAYER(eLoopPlayer).GetMilitaryAI()->HasAirforce())
			{
				bAnyAirforce = true;
				break;
			}
		}
	}

	if(bAnyAirforce)
	{
		int iRatio = (iNumAA * 10) / max(1,iNumMelee+iNumAA);
		return (iRatio > 2);
	}
	else
	{
		return true;
	}
}

/// "Need Anti-Air" Player Strategy: If a player has too many AA units
bool MilitaryAIHelpers::IsTestStrategy_NeedAntiAirUnits(CvPlayer* pPlayer, int iNumAA, int iNumMelee)
{
	bool bAnyAirforce = false;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != pPlayer->GetID() && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			if(GET_PLAYER(eLoopPlayer).GetMilitaryAI()->HasAirforce())
			{
				bAnyAirforce = true;
				break;
			}
		}
	}

	if(bAnyAirforce)
	{
		int iRatio = (iNumAA * 10) / max(1,iNumMelee+iNumAA);
		return (iRatio <= 2);
	}
	else
	{
		return false;
	}
}

/// "Need Air Carrier" Player Strategy: If a player needs some aircraft carriers
bool MilitaryAIHelpers::IsTestStrategy_NeedAirCarriers(CvPlayer* pPlayer)
{
	int iNumLoadableAirUnits = 0;
	int iNumTotalCargoSpace = 0;
	CvUnit* pLoopUnit;
	int iLoop;
	SpecialUnitTypes eSpecialUnitPlane = (SpecialUnitTypes) GC.getInfoTypeForString("SPECIALUNIT_FIGHTER");
	for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
	{
		// Don't count civilians or exploration units
		if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{
			if(pLoopUnit->cargoSpace() > 0)
			{
				if(pLoopUnit->specialCargo() != NO_SPECIALUNIT)
				{
					if(pLoopUnit->specialCargo() != eSpecialUnitPlane)
					{
						continue;
					}
				}

				if (pLoopUnit->domainCargo() != NO_DOMAIN)
				{
					if (pLoopUnit->domainCargo() != DOMAIN_AIR)
					{
						continue;
					}
				}
				iNumTotalCargoSpace += pLoopUnit->cargoSpace();
			}
			else if (pLoopUnit->getSpecialUnitType() == eSpecialUnitPlane)
			{
				iNumLoadableAirUnits += 1;
			}
		}
	}

	if (iNumLoadableAirUnits > iNumTotalCargoSpace)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// MORE NON-MEMBER FUNCTIONS

///	How many military units should we have given current threats?
int MilitaryAIHelpers::ComputeRecommendedNavySize(CvPlayer* pPlayer)
{
	int iNumUnitsWanted = 0;
	int iFlavorNaval = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
	// cap at 10?

	double dMultiplier;

	// Start with 1
	iNumUnitsWanted = 1;

	int iNumCoastalCities = 0;
	int iLoop;
	CvCity* pCity;
	for(pCity = pPlayer->firstCity(&iLoop); pCity != NULL; pCity = pPlayer->nextCity(&iLoop))
	{
		if(pCity->isCoastal(-1))
		{
			iNumCoastalCities++;
		}
	}

	iNumUnitsWanted += iNumCoastalCities;
	// Scale up or down based on true threat level and a bit by flavors (multiplier should range from about 0.75 to 2.0)
	dMultiplier = (double)0.75 + ((double)pPlayer->GetMilitaryAI()->GetHighestThreat() / (double)4.0) + ((double)(iFlavorNaval) / (double)40.0);
	iNumUnitsWanted = (int)((double)iNumUnitsWanted * dMultiplier* /*0.67*/ GC.getAI_STRATEGY_NAVAL_UNITS_PER_CITY());

	iNumUnitsWanted = max(1,iNumUnitsWanted);

	EconomicAIStrategyTypes eStrategyNavalMap = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NAVAL_MAP");
	EconomicAIStrategyTypes eExpandOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	if (pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyNavalMap) || pPlayer->GetEconomicAI()->IsUsingStrategy(eExpandOtherContinents))
	{
		iNumUnitsWanted *= 3;
		iNumUnitsWanted /= 2;
	}

	if (pPlayer->getCivilizationInfo().isCoastalCiv())
	{
		iNumUnitsWanted *= 3;
		iNumUnitsWanted /= 2;
	}

	// if we are going for conquest we want at least one more task force
	int iGT = GC.getGame().getGameTurn();
	iGT = min(iGT,200);
	AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
	if(eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if(pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eConquestGrandStrategy)
		{
			iNumUnitsWanted += (10 * iGT) / 200;
		}
	}

	return iNumUnitsWanted;
}

/// How many slots in this army can we fill right now with available units?
int MilitaryAIHelpers::NumberOfFillableSlots(CvPlayer* pPlayer, MultiunitFormationTypes formation, bool bRequiresNavalMoves, int* piNumberSlotsRequired, int* piNumberLandReservesUsed)
{
	CvUnit* pLoopUnit;
	int iLoop;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL > slotsToFill;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL >::iterator it;
	int iWillBeFilled = 0;
	int iLandReservesUsed = 0;

	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(formation);
	for(int iThisSlotIndex = 0; iThisSlotIndex < thisFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
	{
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(iThisSlotIndex);
		slotsToFill.push_back(thisSlotEntry);
	}

	bool bMustBeDeepWaterNaval = GET_TEAM(pPlayer->getTeam()).canEmbarkAllWaterPassage() && thisFormation->IsRequiresNavalUnitConsistency();

	for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
	{
		// Don't count scouts
		if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{
			// Don't count units that are damaged too heavily
			if(pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
			{
				if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX && pLoopUnit->canRecruitFromTacticalAI())
				{
					if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
					{
						if(!bRequiresNavalMoves || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
						{
							if (!bMustBeDeepWaterNaval || pLoopUnit->getDomainType() != DOMAIN_SEA || !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
							{
								for(it = slotsToFill.begin(); it != slotsToFill.end(); it++)
								{
									CvFormationSlotEntry slotEntry = *it;
									CvUnitEntry& kUnitInfo = pLoopUnit->getUnitInfo();
									if(kUnitInfo.GetUnitAIType((UnitAITypes)slotEntry.m_primaryUnitType) ||
										kUnitInfo.GetUnitAIType((UnitAITypes)slotEntry.m_secondaryUnitType))
									{
										slotsToFill.erase(it);
										iWillBeFilled++;

										if(pLoopUnit->getDomainType() == DOMAIN_LAND)
										{
											iLandReservesUsed++;
										}
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Now go back through remaining slots and see how many were required, we'll need that many more units
	if(piNumberSlotsRequired != NULL)
	{
		(*piNumberSlotsRequired) = iWillBeFilled;
		for(int iThisSlotIndex = 0; iThisSlotIndex < (int)slotsToFill.size(); iThisSlotIndex++)
		{
			if(slotsToFill[iThisSlotIndex].m_requiredSlot)
			{
				(*piNumberSlotsRequired)++;
			}
		}
	}

	if(piNumberLandReservesUsed != NULL)
	{
		*piNumberLandReservesUsed = iLandReservesUsed;
	}
	return iWillBeFilled;
}

/// Army needs more units, which should we build next?
UnitAITypes MilitaryAIHelpers::FirstSlotCityCanFill(CvPlayer* pPlayer, MultiunitFormationTypes formation, bool bRequiresNavalMoves, bool bAtCoastalCity, bool bSecondaryUnit)
{
	CvUnit* pLoopUnit;
	int iLoop;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL > slotsToFill;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL >::iterator it;

	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(formation);
	for(int iThisSlotIndex = 0; iThisSlotIndex < thisFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
	{
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(iThisSlotIndex);
		slotsToFill.push_back(thisSlotEntry);
	}

	bool bMustBeDeepWaterNaval = GET_TEAM(pPlayer->getTeam()).canEmbarkAllWaterPassage() && thisFormation->IsRequiresNavalUnitConsistency();

	for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
	{
		// Don't count scouts
		if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{
			// Don't count units that are damaged too heavily
			if(pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
			{
				if(pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX && pLoopUnit->canRecruitFromTacticalAI())
				{
					if(pLoopUnit->GetDeployFromOperationTurn() + GC.getAI_TACTICAL_MAP_TEMP_ZONE_TURNS() < GC.getGame().getGameTurn())
					{
						if(!bRequiresNavalMoves || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
						{
							if (!bMustBeDeepWaterNaval || pLoopUnit->getDomainType() != DOMAIN_SEA || !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
							{
								for(it = slotsToFill.begin(); it != slotsToFill.end(); it++)
								{
									CvFormationSlotEntry slotEntry = *it;
									CvUnitEntry& kUnitInfo = pLoopUnit->getUnitInfo();
									if(kUnitInfo.GetUnitAIType((UnitAITypes)slotEntry.m_primaryUnitType) ||
										kUnitInfo.GetUnitAIType((UnitAITypes)slotEntry.m_secondaryUnitType))
									{
										slotsToFill.erase(it);
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// If coastal city, try to find the first one that is a naval AI type
	if(bAtCoastalCity)
	{
		for(int iThisSlotIndex = 0; iThisSlotIndex < (int)slotsToFill.size(); iThisSlotIndex++)
		{
			if(slotsToFill[iThisSlotIndex].m_requiredSlot)
			{
				UnitAITypes eType = (UnitAITypes)slotsToFill[iThisSlotIndex].m_primaryUnitType;
				if(eType == UNITAI_ASSAULT_SEA || eType == UNITAI_ATTACK_SEA || eType == UNITAI_RESERVE_SEA || eType == UNITAI_ESCORT_SEA)
				{
					if(!bSecondaryUnit)
					{
						return (UnitAITypes)slotsToFill[iThisSlotIndex].m_primaryUnitType;
					}
					else
					{
						return (UnitAITypes)slotsToFill[iThisSlotIndex].m_secondaryUnitType;
					}
				}
			}
		}
	}

	// Now go back through remaining slots and find first required one
	for(int iThisSlotIndex = 0; iThisSlotIndex < (int)slotsToFill.size(); iThisSlotIndex++)
	{
		if(slotsToFill[iThisSlotIndex].m_requiredSlot)
		{
			if(!bSecondaryUnit)
			{
				return (UnitAITypes)slotsToFill[iThisSlotIndex].m_primaryUnitType;
			}
			else
			{
				return (UnitAITypes)slotsToFill[iThisSlotIndex].m_secondaryUnitType;
			}
		}
	}

	return NO_UNITAI;
}
