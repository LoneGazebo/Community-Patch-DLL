/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
#if defined(MOD_BALANCE_CORE)
	return (index!=NO_MILITARYAISTRATEGY) ? m_paAIStrategyEntries[index] : NULL;
#else
	return m_paAIStrategyEntries[index];
#endif
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
	MOD_SERIALIZE_INIT_READ(kStream);

	FAssertMsg(m_pAIStrategies != NULL && m_pAIStrategies->GetNumMilitaryAIStrategies() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream >> m_iTotalThreatWeight;

	int temp;
	kStream >> temp;
	m_eArmyTypeBeingBuilt = (ArmyType)temp;

	kStream >> m_iNumberOfTimesOpsBuildSkippedOver;

#if defined(MOD_BALANCE_CORE_MILITARY)
	//first get how many entries we have
	kStream >> temp;
	for (int i=0; i<temp; i++)
	{
		int eEnemy, eAIOp;
		kStream >> eEnemy;
		kStream >> eAIOp;
		SCachedTarget target;
		kStream >> target.iTargetCity;
		kStream >> target.iMusterCity;
		kStream >> target.bAttackBySea;
		kStream >> target.iScore;
		kStream >> target.iTurnChosen;
		m_cachedTargets[(PlayerTypes)eEnemy][(AIOperationTypes)eAIOp] = target;
	}
#endif

	int iNumStrategies;
	kStream >> iNumStrategies;
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
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	FAssertMsg(GC.getNumMilitaryAIStrategyInfos() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream << m_iTotalThreatWeight;
	kStream << (int)m_eArmyTypeBeingBuilt;
	kStream << m_iNumberOfTimesOpsBuildSkippedOver;

#if defined(MOD_BALANCE_CORE_MILITARY)
	//first pass: count how many relevant cache entries we have
	int iCount = 0;
	for (CachedTargetsMap::iterator itE = m_cachedTargets.begin(); itE!=m_cachedTargets.end(); ++itE)
		for (CachedTargetsMap::value_type::second_type::iterator itOp = itE->second.begin(); itOp!=itE->second.end(); ++itOp)
			if (GC.getGame().getGameTurn() - itOp->second.iTurnChosen < 25)
				iCount++;

	kStream << iCount;

	//second pass: write them
	for (CachedTargetsMap::iterator itE = m_cachedTargets.begin(); itE!=m_cachedTargets.end(); ++itE)
		for (CachedTargetsMap::value_type::second_type::iterator itOp = itE->second.begin(); itOp!=itE->second.end(); ++itOp)
			if (GC.getGame().getGameTurn() - itOp->second.iTurnChosen < 25)
			{
				kStream << itE->first;
				kStream << itOp->first;
				kStream << itOp->second.iTargetCity;
				kStream << itOp->second.iMusterCity;
				kStream << itOp->second.bAttackBySea;
				kStream << itOp->second.iScore;
				kStream << itOp->second.iTurnChosen;
			}
#endif

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
		MakeOffensivePurchases();
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
#if defined(MOD_BALANCE_CORE_MILITARY)
	target = FindBestAttackTarget2(AI_OPERATION_SNEAK_CITY_ATTACK, eEnemy);
#else
	target = FindBestAttackTarget(AI_OPERATION_SNEAK_CITY_ATTACK, eEnemy);
#endif

	if(target.m_pTargetCity)
	{
		if(target.m_bAttackBySea)
		{
			if(IsAttackReady(MUFORMATION_NAVAL_INVASION, AI_OPERATION_SNEAK_CITY_ATTACK))
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
#if defined(MOD_BALANCE_CORE)
			if(IsAttackReady((MultiunitFormationTypes)MilitaryAIHelpers::GetBestFormationType(), AI_OPERATION_SNEAK_CITY_ATTACK))
#else
			if(IsAttackReady((GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, AI_OPERATION_SNEAK_CITY_ATTACK))
#endif
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
#if defined(MOD_BALANCE_CORE_MILITARY)
			if(target.m_pMusterCity == NULL)
			{
				CvCity* pNearestCoastalCity = m_pPlayer->GetMilitaryAI()->GetNearestCoastalCity(eEnemy);
				if(pNearestCoastalCity != NULL)
				{
					target.m_pMusterCity = pNearestCoastalCity;
				}
			}
			int iOperationID;
			bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SNEAK_ATTACK, &iOperationID, eEnemy);
			if (!bHasOperationUnderway)
			{
#endif
			CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SNEAK_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
			if(pOperation != NULL && !pOperation->ShouldAbort())
			{
				return true;
			}
#if defined(MOD_BALANCE_CORE_MILITARY)
		}
#endif
		}
		else
		{
#if defined(MOD_BALANCE_CORE_MILITARY)
			int iOperationID;
			bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_SMALL_CITY_ATTACK, &iOperationID, eEnemy);
			if (!bHasOperationUnderway)
			{
#endif
			CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_SMALL_CITY_ATTACK, eEnemy, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
			if(pOperation != NULL && !pOperation->ShouldAbort())
			{
				return true;
			}
#if defined(MOD_BALANCE_CORE_MILITARY)
		}
#endif
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
#if defined(MOD_BALANCE_CORE_MILITARY)
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID, eEnemy);
	if (!bHasOperationUnderway)
	{
#endif
	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_FAST_PILLAGERS, false, &iNumRequiredSlots, &iLandReservesUsed);
	if(iFilledSlots >= iNumRequiredSlots && iLandReservesUsed <= GetLandReservesAvailable())
	{
		CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_PILLAGE_ENEMY, eEnemy);
		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}
#if defined(MOD_BALANCE_CORE_MILITARY)
	}
#endif
	return false;
}


/// Send an army to take a city
bool CvMilitaryAI::RequestBasicAttack(PlayerTypes eEnemy, int iNumUnitsWillingBuild)
{
	CvMilitaryTarget target;

#if defined(MOD_BALANCE_CORE_MILITARY)
	target = FindBestAttackTarget2(AI_OPERATION_BASIC_CITY_ATTACK, eEnemy);
#else
	target = FindBestAttackTarget(AI_OPERATION_BASIC_CITY_ATTACK, eEnemy);
#endif

	return RequestSpecificAttack(target, iNumUnitsWillingBuild);
}

/// Send a navy to take a city
bool CvMilitaryAI::RequestPureNavalAttack(PlayerTypes eEnemy, int iNumUnitsWillingBuild)
{
	CvAIOperation* pOperation = NULL;
#if defined(MOD_BALANCE_CORE_MILITARY)
#else
	CvMilitaryTarget target;
#endif
	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;

#if defined(MOD_BALANCE_CORE_MILITARY)
	CvCity* pTargetCity = NULL;
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, &iOperationID, eEnemy);
	if (!bHasOperationUnderway)
	{
		pTargetCity = GetNearestCoastalCityEnemy(eEnemy);
		if(pTargetCity != NULL)
		{
			CvCity* pStartCity = GetNearestCoastalCity(eEnemy);
			if(pStartCity != NULL)
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_PURE_NAVAL_CITY_ATTACK, true, &iNumRequiredSlots, &iLandReservesUsed);
				if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild)
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, eEnemy, pTargetCity->getArea(), pTargetCity, pStartCity);
				}

				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
		}
	}
#else
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
#endif
	return false;
}

/// Request for an attack on a city state
bool CvMilitaryAI::RequestCityStateAttack(PlayerTypes eEnemy)
{
	CvMilitaryTarget target;
	CvAIOperation* pOperation = 0;

#if defined(MOD_BALANCE_CORE_MILITARY)
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_ATTACK, &iOperationID, eEnemy);
	if (!bHasOperationUnderway)
	{
		target = FindBestAttackTarget2(AI_OPERATION_CITY_STATE_ATTACK, eEnemy);
	}
#else
	target = FindBestAttackTarget(AI_OPERATION_CITY_STATE_ATTACK, eEnemy);
#endif

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
#if defined(MOD_BALANCE_CORE_MILITARY)
	if(kTarget.m_pTargetCity)
	{
		if(kTarget.m_bAttackBySea)
		{
			if(!GET_PLAYER(kTarget.m_pTargetCity->getOwner()).isMinorCiv())
			{
				int iOperationID;
				bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID, kTarget.m_pTargetCity->getOwner());
				if (!bHasOperationUnderway)
				{
					iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_INVASION, true, &iNumRequiredSlots, &iLandReservesUsed);
					if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild && iLandReservesUsed <= GetLandReservesAvailable())
					{
						pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pTargetCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
					}
				}
			}
			else if(GET_PLAYER(kTarget.m_pTargetCity->getOwner()).isMinorCiv())
			{
				int iOperationID;
				bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, &iOperationID, kTarget.m_pTargetCity->getOwner());
				if (!bHasOperationUnderway)
				{
					iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_CITY_STATE_INVASION, true, &iNumRequiredSlots, &iLandReservesUsed);
					if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild && iLandReservesUsed <= GetLandReservesAvailable())
					{
						pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pTargetCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
					}
				}
			}
		}
		else
		{
			if(!GET_PLAYER(kTarget.m_pTargetCity->getOwner()).isMinorCiv())
			{
#if defined(MOD_BALANCE_CORE)
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer,(MultiunitFormationTypes)MilitaryAIHelpers::GetBestFormationType(), false, &iNumRequiredSlots, &iLandReservesUsed);
#else
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer,(GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, false, &iNumRequiredSlots, &iLandReservesUsed);
#endif
				if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild && iLandReservesUsed <= GetLandReservesAvailable())
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_BASIC_CITY_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pTargetCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
				}
			}
			else if(GET_PLAYER(kTarget.m_pTargetCity->getOwner()).isMinorCiv())
			{
				int iOperationID;
				bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_ATTACK, &iOperationID, kTarget.m_pTargetCity->getOwner());
				if (!bHasOperationUnderway)
				{
					iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_SMALL_CITY_ATTACK_FORCE, false, &iNumRequiredSlots, &iLandReservesUsed);
					if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild && iLandReservesUsed <= GetLandReservesAvailable())
					{
						pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pTargetCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
				}
			}
				}
		}
		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}
#else
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
			iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer,(GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, false, &iNumRequiredSlots, &iLandReservesUsed);
			if((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild && iLandReservesUsed <= GetLandReservesAvailable())
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_BASIC_CITY_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pTargetCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
				if (pOperation != NULL && !pOperation->ShouldAbort() && kTarget.m_pTargetCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				{
					int iFlavorNaval = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
					int iNumSuperiority = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_SUPERIORITY);
					int iNumBombard = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_BOMBARDMENT);
					int iMaxOperations = iFlavorNaval / 2;
					if(GC.getMap().GetAIMapHint() & 1)
					{
						iMaxOperations *= 2;
					}
					if (iNumSuperiority+iNumBombard <= iMaxOperations)
						m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, NO_PLAYER);
		}
			}
		}
		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}
#endif
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
	bool bIsVenice = m_pPlayer->GetPlayerTraits()->IsNoAnnexing();

	// No units in puppet cities except for Venice!
	if(pCity->IsPuppet() && !bIsVenice)
	{
		return NULL;
	}

	// Get best unit with this AI type
#if defined(MOD_BALANCE_CORE)
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitType, true, false);
#else
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitType);
#endif
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
						CvUnit* pUnit = m_pPlayer->getUnit(iResult);
						m_pPlayer->GetTreasury()->LogExpenditure((CvString)pUnit->getUnitInfo().GetText(), iGoldCost, 7);
						m_pPlayer->GetTreasury()->ChangeGold(-iGoldCost);

#if defined(MOD_BUGFIX_MOVE_AFTER_PURCHASE)
						if (!pUnit->getUnitInfo().CanMoveAfterPurchase())
						{
#endif
							pUnit->setMoves(0);
#if defined(MOD_BUGFIX_MOVE_AFTER_PURCHASE)
						}
#endif

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
#if defined(MOD_BUGFIX_MOVE_AFTER_PURCHASE)
				if (!pUnit->getUnitInfo().CanMoveAfterPurchase())
				{
#endif
					pUnit->setMoves(0);
#if defined(MOD_BUGFIX_MOVE_AFTER_PURCHASE)
				}
#endif

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
						m_pPlayer->GetTreasury()->LogExpenditure((CvString)pkBuildingInfo->GetText(), iGoldCost, 8);
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
#if defined(MOD_BALANCE_CORE_MILITARY) && defined(MOD_BALANCE_CORE_GLOBAL_CITY_IDS)
CvCity* GetCityFromGlobalID(int iID)
{
	//the muster city for a given target can belong to any player, no only to ourselves
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{	
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive())
		{
			CvCity* pCity = GET_PLAYER(eLoopPlayer).getCityByGlobalID(iID);
			if (pCity!=NULL)
			{
				//sanity check
				if (pCity->getOwner()!=eLoopPlayer)
				{
					OutputDebugString("incorrect city owner!");
					return NULL;
				}
				return pCity;
			}
		}
	}
	return NULL;
}

CvMilitaryTarget CvMilitaryAI::FindBestAttackTarget2(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, int* piWinningScore)
{
	int ciAgeLimit = 20;

	if (eEnemy >= MAX_CIV_PLAYERS)
	{
		if (piWinningScore)
			*piWinningScore = 0;
		return CvMilitaryTarget();
	}

	int iNewScore = 0;
	CvMilitaryTarget new_target;
	bool bFoundInCache = false;

	//todo: compare score across different enemies / op types to see if this makes sense at all

	CachedTargetsMap::iterator itE = m_cachedTargets.find(eEnemy);
	if (itE != m_cachedTargets.end())
	{
		CachedTargetsMap::value_type::second_type::iterator itOp = itE->second.find(eAIOperationType);
		if (itOp != itE->second.end())
		{
			bFoundInCache = true;

			// important - this must be a reference!
			SCachedTarget& cachedTarget = itOp->second;

			CvCity* pCachedTargetCity = GetCityFromGlobalID(cachedTarget.iTargetCity);
			CvCity* pCachedMusterCity = GetCityFromGlobalID(cachedTarget.iMusterCity);

			//check if the cached target is still good
			if (GC.getGame().getGameTurn() - cachedTarget.iTurnChosen >= ciAgeLimit)
				cachedTarget.iScore = 0;

			//naval operations don't have a muster city
			if(eAIOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK || eAIOperationType == AI_OPERATION_CITY_STATE_NAVAL_ATTACK || eAIOperationType == AI_OPERATION_NAVAL_ATTACK)
			{
 				if (pCachedTargetCity == NULL || 
					pCachedTargetCity->getOwner() != eEnemy)
				{
					cachedTarget.iScore = 0;
				}
			}
			else
			{
				//land based op, check muster city too
				if (pCachedTargetCity == NULL || 
					pCachedMusterCity == NULL ||
					pCachedTargetCity->getOwner() != eEnemy || 
					!GET_TEAM( GET_PLAYER(pCachedMusterCity->getOwner()).getTeam() ).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam()) )
				{
					cachedTarget.iScore = 0;
				}
			}

			//check the current situation
			new_target = FindBestAttackTarget(eAIOperationType, eEnemy, &iNewScore);

			//if we can't use the old one anymore or the new target is much better
			if(iNewScore > cachedTarget.iScore*1.4)
			{
				if(new_target.m_pTargetCity)
				{
					cachedTarget.iTargetCity = new_target.m_pTargetCity->GetGlobalID();
					if (new_target.m_pMusterCity)
						cachedTarget.iMusterCity = new_target.m_pMusterCity->GetGlobalID();
					cachedTarget.bAttackBySea = new_target.m_bAttackBySea;
					cachedTarget.iScore = iNewScore;
					cachedTarget.iTurnChosen = GC.getGame().getGameTurn();			

					//important, update the cache
					m_cachedTargets[eEnemy][eAIOperationType] = cachedTarget;

					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strOutBuf = CvString::format("%d, %s, refreshed our attack target, %s, Muster: %s",
							GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription(), new_target.m_pTargetCity->getName().c_str(), 
							new_target.m_pMusterCity ? new_target.m_pMusterCity->getName().c_str() : "NONE");
						FILogFile* pLog = LOGFILEMGR.GetLog("OperationalAILog.csv", FILogFile::kDontTimeStamp);
						if (pLog)
							pLog->Msg(strOutBuf);
					}
				}
			}
			else
			{
				//may be null!
				new_target.m_pTargetCity = pCachedTargetCity;
				new_target.m_pMusterCity = pCachedMusterCity;
				new_target.m_bAttackBySea = cachedTarget.bAttackBySea;
				iNewScore = cachedTarget.iScore;

				if(GC.getLogging() && GC.getAILogging() && pCachedTargetCity)
				{
					CvString strOutBuf = CvString::format("%d, %s, keeping cached attack target, %s, Muster: %s",
						GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription(), pCachedTargetCity->getName().c_str(), 
						pCachedMusterCity ? pCachedMusterCity->getName().c_str() : "NONE");
					FILogFile* pLog = LOGFILEMGR.GetLog("OperationalAILog.csv", FILogFile::kDontTimeStamp);
					if (pLog)
						pLog->Msg(strOutBuf);
				}
			}
		}
	}

	if (!bFoundInCache)
	{
		new_target = FindBestAttackTarget(eAIOperationType, eEnemy, &iNewScore);

		if (new_target.m_pTargetCity)
		{
			SCachedTarget memory;
			memory.iTargetCity = new_target.m_pTargetCity->GetGlobalID();
			if (new_target.m_pMusterCity)
				memory.iMusterCity = new_target.m_pMusterCity->GetGlobalID();
			memory.bAttackBySea = new_target.m_bAttackBySea;
			memory.iScore = iNewScore;
			memory.iTurnChosen = GC.getGame().getGameTurn();

			//update the cache
			m_cachedTargets[eEnemy][eAIOperationType] = memory;

			if(GC.getLogging() && GC.getAILogging())
			{
				CvString strOutBuf = CvString::format("%d, %s, found new attack target, %s, Muster: %s",
					GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription(), new_target.m_pTargetCity->getName().c_str(), 
					new_target.m_pMusterCity ? new_target.m_pMusterCity->getName().c_str() : "NONE");
				FILogFile* pLog = LOGFILEMGR.GetLog("OperationalAILog.csv", FILogFile::kDontTimeStamp);
				if (pLog)
					pLog->Msg(strOutBuf);
			}
		}
	}

	//either the cached one or we updated it
	if (piWinningScore)
		*piWinningScore = iNewScore;

	return new_target;
}

#endif

/// Best target by land OR sea
CvMilitaryTarget CvMilitaryAI::FindBestAttackTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, int* piWinningScore)
{
	int iFriendlyLoop;
	int iEnemyLoop;
	int iUnitLoop;
	CvCity* pFriendlyCity;
	CvCity* pEnemyCity;
	CvUnit* pLoopUnit;
	static CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true> weightedTargetList;
	CvMilitaryTarget chosenTarget;
	CvPlayer &kEnemy = GET_PLAYER(eEnemy);
#if defined(MOD_BALANCE_CORE_MILITARY)
	PlayerTypes eLoopPlayer;
	//Let's loop through all valid cities in the world
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(GET_PLAYER(eLoopPlayer).isAlive())
		{
			bool bIsMinor = false;
			if(GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				if(GET_PLAYER(eLoopPlayer).GetMinorCivAI()->IsAllies(m_pPlayer->GetID()))
				{
					bIsMinor = true;
				}
			}
			if(eLoopPlayer != eEnemy && (bIsMinor || GET_PLAYER(eLoopPlayer).GetID() == m_pPlayer->GetID() || GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam())))
			{
				// Estimate the relative strength of units near our cities and near their cities (can't use TacticalAnalysisMap because we may not be at war - and that it isn't current if we are calling this from the DiploAI)
				for (pFriendlyCity = GET_PLAYER(eLoopPlayer).firstCity(&iFriendlyLoop); pFriendlyCity != NULL; pFriendlyCity = GET_PLAYER(eLoopPlayer).nextCity(&iFriendlyLoop))
				{
					CvPlot* pPlot = pFriendlyCity->plot();
					//If there aren't at least 12 non-occupied plots around this city, abort.
					if(pPlot != NULL && !m_pPlayer->GetTacticalAI()->ScoreDeploymentPlotsHelper(pPlot, 12, 3))
					{
						continue;
					}
					int iX = pPlot->getX();
					int iY = pPlot->getY();
					bool bGeneralInTheVicinity = false;
					int iPower = 0;
					for (pLoopUnit = GET_PLAYER(eLoopPlayer).firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(eLoopPlayer).nextUnit(&iUnitLoop))
					{
						if (pLoopUnit->IsCombatUnit())
						{
							int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
							if (iDistance <= 5)
							{
								iPower += pLoopUnit->GetPower();
							}
						}
						if (!bGeneralInTheVicinity && pLoopUnit->IsGreatGeneral())
						{
							int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
							if (iDistance <= 5)
							{
								bGeneralInTheVicinity = true;
							}
						}
					}
					if (bGeneralInTheVicinity)
					{
						iPower *= 11;
						iPower /= 10;
					}
					pFriendlyCity->iScratch = iPower;
				}
			}
			else if(eLoopPlayer == eEnemy)
			{
				for(pEnemyCity = kEnemy.firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = kEnemy.nextCity(&iEnemyLoop))
				{
					CvPlot* pPlot = pEnemyCity->plot();
					//If there aren't at least 12 non-occupied plots around this city, abort.
					if(pPlot != NULL && !m_pPlayer->GetTacticalAI()->ScoreDeploymentPlotsHelper(pPlot, 12, 3))
					{
						continue;
					}
					if(pPlot->isRevealed(m_pPlayer->getTeam()))
					{
						int iX = pPlot->getX();
						int iY = pPlot->getY();
						bool bGeneralInTheVicinity = false;
						int iPower = 0;
						for (pLoopUnit = kEnemy.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kEnemy.nextUnit(&iUnitLoop))
						{
							if (pLoopUnit->IsCombatUnit())
							{
								int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
								if (iDistance <= 5)
								{
									iPower += pLoopUnit->GetPower();
								}
							}
							if (!bGeneralInTheVicinity && pLoopUnit->IsGreatGeneral())
							{
								int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
								if (iDistance <= 5)
								{
									bGeneralInTheVicinity = true;
								}
							}
						}
						if (bGeneralInTheVicinity)
						{
							iPower *= 12;
							iPower /= 10;
						}
						pEnemyCity->iScratch = iPower;
					}
				}
			}
		}
	}
	// Build a list of all the possible start city/target city pairs
	static CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true> prelimWeightedTargetList;
	prelimWeightedTargetList.clear();
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(GET_PLAYER(eLoopPlayer).isAlive())
		{
			bool bIsMinor = false;
			if(GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				if(GET_PLAYER(eLoopPlayer).GetMinorCivAI()->IsAllies(m_pPlayer->GetID()))
				{
					bIsMinor = true;
				}
			}
			if(eLoopPlayer != eEnemy && (bIsMinor || GET_PLAYER(eLoopPlayer).GetID() == m_pPlayer->GetID() || GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).IsAllowsOpenBordersToTeam(m_pPlayer->getTeam())))
			{
				for(pFriendlyCity = GET_PLAYER(eLoopPlayer).firstCity(&iFriendlyLoop); pFriendlyCity != NULL; pFriendlyCity = GET_PLAYER(eLoopPlayer).nextCity(&iFriendlyLoop))
				{
					for(pEnemyCity = GET_PLAYER(eEnemy).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eEnemy).nextCity(&iEnemyLoop))
					{
						if(pEnemyCity->plot()->isRevealed(m_pPlayer->getTeam()))
						{
							CvMilitaryTarget target;
							int iWeight;
							target.m_pMusterCity = pFriendlyCity;
							target.m_pTargetCity = pEnemyCity;
							target.iMusterNearbyUnitPower = pFriendlyCity->iScratch;
							target.iTargetNearbyUnitPower = pEnemyCity->iScratch;
							target.m_bAttackBySea = false;
							CvAStarNode* pPathfinderNode;
							bool bSuccess = false;
							if (eAIOperationType == AI_OPERATION_NAVAL_ATTACK || eAIOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK || eAIOperationType == AI_OPERATION_CITY_STATE_NAVAL_ATTACK || eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK)
							{
								//No coastal cities owned that can path to target? We can't build a navy, now can we?
								if(GetNearestCoastalCity(eLoopPlayer) == NULL)
								{
									continue;
								}
								if (!target.m_pMusterCity->isCoastal() || !target.m_pTargetCity->isCoastal())
								{
									continue;
								}
								if(GC.GetInternationalTradeRouteWaterFinder().GeneratePath(pFriendlyCity->getX(), pFriendlyCity->getY(), pEnemyCity->getX(), pEnemyCity->getY()))
								{
									pPathfinderNode = GC.GetInternationalTradeRouteWaterFinder().GetLastNode();
									if(pPathfinderNode != NULL)
									{
										target.m_bAttackBySea = true;
										bSuccess = true;
										target.m_iPathLength = (pPathfinderNode->m_iTotalCost / 100);
									}
								}
								if(!bSuccess)
								{
									continue;
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
								iWeight = (30000 - target.m_iPathLength);   // Start by using the path length as the weight, shorter paths have higher weight
								prelimWeightedTargetList.push_back(target, iWeight);
							}
						}
					}
				}
			}
		}
	}
	// Let's score the 10 shortest paths ... anything more than that means there are too many interior cities from one (or both) sides being considered
	prelimWeightedTargetList.SortItems();
	weightedTargetList.clear();
	int iTargetsConsidered = 0;
	for (int iI = 0; iI < prelimWeightedTargetList.size() && iTargetsConsidered < 10; iI++)
	{
		CvMilitaryTarget target = prelimWeightedTargetList.GetElement(iI);
		int iWeight;

		iWeight = ScoreTarget(target, eAIOperationType);

		if (iWeight > 0)
		{
			if (weightedTargetList.GetElement( weightedTargetList.size()-1 ) == target)
			{
				OutputDebugString("repeated target! why??");
			}

			weightedTargetList.push_back(target, iWeight);
			iTargetsConsidered++;
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

	//just take the best one
	chosenTarget = weightedTargetList.GetElement(0);
	if (piWinningScore)
	{
		*piWinningScore = ScoreTarget(chosenTarget, eAIOperationType);
	}

	return chosenTarget;
}
#else
	// Estimate the relative strength of units near our cities and near their cities (can't use TacticalAnalysisMap because we may not be at war - and that it isn't current if we are calling this from the DiploAI)
	for (pFriendlyCity = m_pPlayer->firstCity(&iFriendlyLoop); pFriendlyCity != NULL; pFriendlyCity = m_pPlayer->nextCity(&iFriendlyLoop))
					{
		CvPlot* pPlot = pFriendlyCity->plot();
		int iX = pPlot->getX();
		int iY = pPlot->getY();
		bool bGeneralInTheVicinity = false;
		int iPower = 0;
		for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
					{
			if (pLoopUnit->IsCombatUnit())
						{
				int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
				if (iDistance <= 5)
							{
					iPower += pLoopUnit->GetPower();
								}
							}
			if (!bGeneralInTheVicinity && pLoopUnit->IsGreatGeneral())
							{
				int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
				if (iDistance <= 5)
								{
					bGeneralInTheVicinity = true;
								}
							}
		}
		if (bGeneralInTheVicinity)
							{
			iPower *= 11;
			iPower /= 10;
							}
		pFriendlyCity->iScratch = iPower;
						}
	for(pEnemyCity = kEnemy.firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = kEnemy.nextCity(&iEnemyLoop))
			{
		CvPlot* pPlot = pEnemyCity->plot();
		if(pPlot->isRevealed(m_pPlayer->getTeam()))
				{
			int iX = pPlot->getX();
			int iY = pPlot->getY();
			bool bGeneralInTheVicinity = false;
			int iPower = 0;
			for (pLoopUnit = kEnemy.firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = kEnemy.nextUnit(&iUnitLoop))
					{
				if (pLoopUnit->IsCombatUnit())
						{
					int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
					if (iDistance <= 5)
							{
						iPower += pLoopUnit->GetPower();
					}
				}
				if (!bGeneralInTheVicinity && pLoopUnit->IsGreatGeneral())
								{
					int iDistance = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iX, iY);
					if (iDistance <= 5)
									{
						bGeneralInTheVicinity = true;
											}
										}
									}
			if (bGeneralInTheVicinity)
			{
				iPower *= 11;
				iPower /= 10;
								}
			pEnemyCity->iScratch = iPower;
							}
						}
	// Build a list of all the possible start city/target city pairs
	static CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true> prelimWeightedTargetList;
	prelimWeightedTargetList.clear();
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
				target.iMusterNearbyUnitPower = pFriendlyCity->iScratch;
				target.iTargetNearbyUnitPower = pEnemyCity->iScratch;
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
								iWeight = (10000 - target.m_iPathLength);   // Start by using the path length as the weight, shorter paths have higher weight
								prelimWeightedTargetList.push_back(target, iWeight);
							}
						}
					}
				}

	// Let's score the 25 shortest paths ... anything more than that means there are too many interior cities from one (or both) sides being considered
	prelimWeightedTargetList.SortItems();
	weightedTargetList.clear();
	int iTargetsConsidered = 0;
	for (int iI = 0; iI < prelimWeightedTargetList.size() && iTargetsConsidered < 25; iI++)
	{
		CvMilitaryTarget target = prelimWeightedTargetList.GetElement(iI);
		int iWeight;

		// If a sea target, we haven't checked the path yet.  Do that now
		if (target.m_bAttackBySea)
		{
			if (!target.m_pMusterCity->isCoastal())
			{
				continue;
			}
			if (!target.m_pTargetCity->isCoastal())
			{
				continue;
			}
			CvPlot *pSeaPlotNearMuster = GetCoastalPlotAdjacentToTarget(target.m_pMusterCity->plot(), NULL);
			CvPlot *pSeaPlotNearTarget = GetCoastalPlotAdjacentToTarget(target.m_pTargetCity->plot(), NULL);
			if(!GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, pSeaPlotNearMuster, pSeaPlotNearTarget))
			{
				continue;
			}
									}

		iWeight = ScoreTarget(target, eAIOperationType);

		if (weightedTargetList.GetElement( weightedTargetList.size()-1 ) == target)
		{
			OutputDebugString("repeated target! why??");
		}

		weightedTargetList.push_back(target, iWeight);
		iTargetsConsidered++;
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

#if defined(MOD_BALANCE_CORE_MILITARY)
	//just take the best one
	chosenTarget = weightedTargetList.GetElement(0);
	if (piWinningScore)
		*piWinningScore = ScoreTarget(chosenTarget, eAIOperationType);
#else

	if(weightedTargetList.GetTotalWeight() > 0)
	{
		RandomNumberDelegate fcn;
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
		int iNumChoices = max (1, (weightedTargetList.size() * 25 / 100));
		chosenTarget = weightedTargetList.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing attack target from top 25%% of choices");
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
#endif

	return chosenTarget;
}
#endif
/// Is it better to attack this target by sea?
void CvMilitaryAI::ShouldAttackBySea(PlayerTypes eEnemy, CvMilitaryTarget& target)
{
	CvAStarNode* pPathfinderNode;
	int iPathLength = 0;
#if defined(MOD_BALANCE_CORE_MILITARY)
	int iWaterStep = 0;
#else
	int iPlotDistance = plotDistance(target.m_pMusterCity->getX(), target.m_pMusterCity->getY(), target.m_pTargetCity->getX(), target.m_pTargetCity->getY());
#endif
	// Can embark
	if(GET_TEAM(m_pPlayer->getTeam()).canEmbark())
	{
#if defined(MOD_BALANCE_CORE_MILITARY)
		// Step path between muster point and target?
		if(GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, target.m_pMusterCity->plot(), target.m_pTargetCity->plot()))
		{
			pPathfinderNode = GC.getStepFinder().GetLastNode();
			if(pPathfinderNode != NULL)
			{
				iPathLength = pPathfinderNode->m_iTotalCost;

				int iEnemyPlots = GC.getStepFinder().CountPlotsOwnedByXInPath(eEnemy);
				//up to 3 plots over enemy territory is good, more costs extra 
				iPathLength += MAX(0, iEnemyPlots-3);
			}
		}
		// Water path between muster point and target?
		if(GC.GetInternationalTradeRouteWaterFinder().GeneratePath(target.m_pMusterCity->getX(), target.m_pMusterCity->getY(), target.m_pTargetCity->getX(), target.m_pTargetCity->getY()))
		{
			pPathfinderNode = GC.GetInternationalTradeRouteWaterFinder().GetLastNode();
			if(pPathfinderNode != NULL)
			{
				iWaterStep = (pPathfinderNode->m_iTotalCost / 100);
			}
			if(iWaterStep > 0)
			{
				// On different landmasses?
				if(target.m_pMusterCity->getArea() != target.m_pTargetCity->getArea())
				{
					target.m_bAttackBySea = true;
					target.m_iPathLength = iWaterStep;
					return;
				}
				// There is no land path, or land path is over twice as long as water path
				if((iPathLength == 0) || (iPathLength > (2 * iWaterStep)))
				{
					target.m_bAttackBySea = true;
					target.m_iPathLength = iWaterStep;
					return;
				}
			}
		}
		if(target.m_pTargetCity->isCoastal() && iWaterStep > 0)
		{
			int iTargetValue = 0;
			iTargetValue = EvaluateMilitaryApproaches(target.m_pTargetCity, true, false);
			//Is their city relatively difficult to access via land? Go for a sea attack then.
			if(iTargetValue == ATTACK_APPROACH_RESTRICTED)
			{
				target.m_bAttackBySea = true;
				target.m_iPathLength = iWaterStep;
				return;
			}
		}
		if(iWaterStep > 0)
		{
			// Is the route too difficult? We're looking for lots of impassable plots here. It is infinitely easier to travel by sea if our formation will break down because of narrow paths.
			int iRange = 0;
			int iDX = 0;
			int iDY = 0;
			int iNumBad = 0;
			int iNumPlots = 0;
			CvAStarNode* pNode;
			CvPlot* pCurrentPlot;		
			if(GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, target.m_pMusterCity->plot(), target.m_pTargetCity->plot()))
			{
				GC.getStepFinder().SetData(&eEnemy);
				bool bRoute = GC.getStepFinder().GeneratePath(target.m_pMusterCity->getX(), target.m_pMusterCity->getY(), target.m_pTargetCity->getX(), target.m_pTargetCity->getY(), m_pPlayer->GetID(), false);
				{
					if(bRoute)
					{
						pNode = GC.getStepFinder().GetLastNode();

						while(pNode)
						{
							pCurrentPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);

							if(pCurrentPlot != NULL)
							{
								//Look 2 hexes from each point on plot. That's a narrow enough area to cause problems.
								iRange = 2;
								for(iDX = -(iRange); iDX <= iRange; iDX++)
								{
									for(iDY = -(iRange); iDY <= iRange; iDY++)
									{
										CvPlot* pLoopPlot = plotXYWithRangeCheck(pCurrentPlot->getX(), pCurrentPlot->getY(), iDX, iDY, iRange);

										if(pLoopPlot != NULL)
										{
											if(pLoopPlot->isImpassable() || pLoopPlot->isWater())
											{
												iNumBad++;
											}
											iNumPlots++;
										}
									}
								}
								//More than 50% of plots en route impassable? This is a treacherous path.
								if(iNumPlots > 0)
								{
									if(iNumBad >= (iNumPlots / 2))
									{
										target.m_bAttackBySea = true;
										target.m_iPathLength = iWaterStep;
										return;
									}
								}
							}
							pNode = pNode->m_pParent;
						}
					}
				}
			}
		}
	
#else
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
#endif
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
#if defined(MOD_BALANCE_CORE_MILITARY)
				iPathLength = pPathfinderNode->m_iTotalCost;
#else
				iPathLength = pPathfinderNode->m_iData1;
#endif
			}
		}
	}

	target.m_bAttackBySea = false;
	target.m_iPathLength = iPathLength;
}


#if defined(MOD_BALANCE_CORE_MILITARY)

/// Come up with a target priority looking at distance, strength, approaches (high score = more desirable target)
int CvMilitaryAI::ScoreTarget(CvMilitaryTarget& target, AIOperationTypes eAIOperationType)
{
	// Don't want it to already be targeted by an operation that's not on its way
	if((eAIOperationType == AI_OPERATION_NAVAL_ATTACK ||
		eAIOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK ||
		eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_CITY_STATE_NAVAL_ATTACK) &&
		m_pPlayer->IsCityAlreadyTargeted(target.m_pTargetCity, DOMAIN_SEA, 50))
	{	
		return 0;
	}
	else if((eAIOperationType == AI_OPERATION_BASIC_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_SMALL_CITY_ATTACK) &&
		m_pPlayer->IsCityAlreadyTargeted(target.m_pTargetCity, DOMAIN_LAND, 50))
	{	
		return 0;
	}

	// Don't want it to already be targeted by an operation that's not on its way
	if((eAIOperationType == AI_OPERATION_NAVAL_ATTACK ||
		eAIOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK ||
		eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_CITY_STATE_NAVAL_ATTACK) &&
		m_pPlayer->IsMusterCityAlreadyTargeted(target.m_pMusterCity, DOMAIN_SEA, 50))
	{	
		return 0;
	}
	else if((eAIOperationType == AI_OPERATION_BASIC_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_SMALL_CITY_ATTACK) &&
		m_pPlayer->IsMusterCityAlreadyTargeted(target.m_pMusterCity, DOMAIN_LAND, 50))
	{	
		return 0;
	}

	float fDistWeightInterpolated = 1;
	if(!target.m_bAttackBySea)
	{
		// interpolate linearly between a low and a high distance
		float fDistanceLow = 8, fWeightLow = 10;
		float fDistanceHigh = 24, fWeightHigh = 1;

		// Is this a sneakattack?  If so distance is REALLY important (want to target spaces on edge of empire)
		if (eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK)
		{
			fDistanceLow /= 2;
			fDistanceHigh /= 2;
		}

		float fSlope = (fWeightHigh-fWeightLow) / (fDistanceHigh-fDistanceLow);
		fDistWeightInterpolated = (target.m_iPathLength-fDistanceLow) * fSlope + fWeightLow;
		fDistWeightInterpolated = min( fWeightLow, max( fWeightHigh, fDistWeightInterpolated ) );

		// Double if we can assemble troops in muster city with airlifts
		if (target.m_pMusterCity->CanAirlift())
			fDistWeightInterpolated *= 2;
	}
	else
	{
		// interpolate linearly between a low and a high distance
		float fDistanceLow = 12, fWeightLow = 5;
		float fDistanceHigh = 36, fWeightHigh = 1;

		// Is this a sneak attack?  If so distance is REALLY important (want to target spaces on edge of empire)
		if (eAIOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK)
		{
			fDistanceLow /= 2;
			fDistanceHigh /= 2;
		}

		float fSlope = (fWeightHigh-fWeightLow) / (fDistanceHigh-fDistanceLow);
		fDistWeightInterpolated = (target.m_iPathLength-fDistanceLow) * fSlope + fWeightLow;
		fDistWeightInterpolated = min( fWeightLow, max( fWeightHigh, fDistWeightInterpolated ) );

		// If coming over sea, inland cities are trickier
		if(!target.m_pTargetCity->plot()->isCoastalLand(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
			fDistWeightInterpolated /= 2;
	}

	CityAttackApproaches eApproaches;
	float fApproachMultiplier = 1;
	if(eAIOperationType == AI_OPERATION_NAVAL_ATTACK ||
		eAIOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK ||
		eAIOperationType == AI_OPERATION_PURE_NAVAL_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_CITY_STATE_NAVAL_ATTACK)
	{	
		eApproaches = EvaluateMilitaryApproaches(target.m_pTargetCity, false /* Assume units coming by sea can disembark */, true);
	}
	else if(eAIOperationType == AI_OPERATION_BASIC_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_SNEAK_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_SMALL_CITY_ATTACK)
	{	
		eApproaches = EvaluateMilitaryApproaches(target.m_pTargetCity, true /* Assume units coming by sea can disembark */, false);
	}
	else
	{
		eApproaches = EvaluateMilitaryApproaches(target.m_pTargetCity, true /* Assume units coming by sea can disembark */, target.m_bAttackBySea);
	}
	//bail if hopeless
	if (eApproaches==ATTACK_APPROACH_NONE)
		return 0;

	switch(eApproaches)
	{
	case ATTACK_APPROACH_UNRESTRICTED:
		fApproachMultiplier = 1.0f;
		break;

	case ATTACK_APPROACH_OPEN:
		fApproachMultiplier = 0.8f;
		break;

	case ATTACK_APPROACH_NEUTRAL:
		fApproachMultiplier = 0.6f;
		break;

	case ATTACK_APPROACH_LIMITED:
		fApproachMultiplier = 0.3f;
		break;

	case ATTACK_APPROACH_RESTRICTED:
		fApproachMultiplier = 0.1f;
		break;

	case ATTACK_APPROACH_NONE:
		fApproachMultiplier = 0;
		break;
	}

	//strength values for each target are estimated before calling this function
	float fFriendlyStrength = (float)target.iMusterNearbyUnitPower;
	float fEnemyStrength = target.iTargetNearbyUnitPower + (target.m_pTargetCity->getStrengthValue() / 50.f);
	fFriendlyStrength = max(1.f, fFriendlyStrength);
	fEnemyStrength = max(1.f, fEnemyStrength);
	float fStrengthRatio = min( 10.f, fFriendlyStrength / fEnemyStrength );

	//bail if hopeless
	if (fStrengthRatio < 0.5f)
	{
		return 0;
	}

	float fDesirability = 1;
	if (target.m_pTargetCity->IsOriginalCapital())
	{
		fDesirability *= GC.getAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL();
		fDesirability /= 100;
	}
	if (target.m_pTargetCity->getOriginalOwner() == m_pPlayer->GetID())
	{
		fDesirability *= GC.getAI_MILITARY_RECAPTURING_OWN_CITY();
		fDesirability /= 100;
	}
	int iRange = 8;

	// Within 8 hexes? This is a real good target for a sneak attack.
	if(eAIOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK || AI_OPERATION_SNEAK_CITY_ATTACK)
	{
		if(target.m_pMusterCity && target.m_pTargetCity)
		{
			if(::plotDistance(target.m_pMusterCity->getX(), target.m_pMusterCity->getY(), target.m_pTargetCity->getX(), target.m_pTargetCity->getY()) < iRange)
			{
				fDesirability *= 10;
			}
		}
	}
	
#if defined(MOD_DIPLOMACY_CITYSTATES)
	if (MOD_DIPLOMACY_CITYSTATES) {
		for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinorLoop;
			if(eMinor != NO_PLAYER && GET_PLAYER(eMinor).isAlive())
			{
				CvPlayer* pMinor = &GET_PLAYER(eMinor);
				if(pMinor->GetMinorCivAI()->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_LIBERATION))
				{
					if(target.m_pTargetCity->getOriginalOwner() == pMinor->GetMinorCivAI()->GetQuestData1(m_pPlayer->GetID(), MINOR_CIV_QUEST_LIBERATION))
					{
						fDesirability *= GC.getAI_MILITARY_RECAPTURING_CITY_STATE();
						fDesirability /= 100;
					}	
				}
			}
		}
	}
#endif
#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;
			if(target.m_pTargetCity->IsHasResourceLocal(eResource, true) && (GC.getMap().getNumResources(eResource) > 0))
			{
				//Will this get us closer to a monopoly?
				int iBonus = (((m_pPlayer->getNumResourceTotal(eResource, false) + m_pPlayer->getResourceExport(eResource)) * 100) / GC.getMap().getNumResources(eResource));
				if(iBonus > 0)
				{
					fDesirability *= 3;
					fDesirability /= 2;
					break;
				}
			}
		}
	}
#endif			
	
	// Economic value of target
	float fEconomicValue = 1.0;
	fEconomicValue = (float)target.m_pTargetCity->getEconomicValue( GetPlayer()->GetID(), 100 );
	fEconomicValue = sqrt(fEconomicValue/100);

	//everything together now
	int iRtnValue = (int)(100 * fDistWeightInterpolated * fApproachMultiplier * fStrengthRatio * fDesirability * fEconomicValue);

	if(GC.getLogging() && GC.getAILogging())
	{
		// Open the right file
		CvString playerName = GetPlayer()->getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		CvString msg = CvString::format( "Evaluating attack on %s from base in %s. Approach is %.2f. Distance is %d (weight %.2f), desirability %.2f, strength ratio %.2f, economic value %.2f --> score %d\n",
			target.m_pTargetCity->getName().c_str(), target.m_pMusterCity->getName().c_str(), fApproachMultiplier, target.m_iPathLength, fDistWeightInterpolated, fDesirability, fStrengthRatio, fEconomicValue, iRtnValue );
		pLog->Msg( msg.c_str() );
	}

	return iRtnValue;
}

#else

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

		// Double if we can assemble troops in muster city with airlifts
		if (target.m_pMusterCity->CanAirlift())
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

	// should probably give a bonus if these cities are adjacent

	// Don't want to start at a city that isn't connected to our capital
	if (!target.m_pMusterCity->IsRouteToCapitalConnected() && !target.m_pMusterCity->isCapital())
	{
		uliRtnValue /= 4;
	}

	// this won't work if we are "just checking" as the zone are only built for actual war war opponents
	// TODO come up with a better way to do this that is always correct

	int iFriendlyStrength = target.iMusterNearbyUnitPower;
	int iEnemyStrength = target.iTargetNearbyUnitPower + (target.m_pTargetCity->getStrengthValue() / 50);
	iFriendlyStrength = max(1, iFriendlyStrength);
	iEnemyStrength = max(1, iEnemyStrength);
	int iRatio = 1;
	iRatio = (iFriendlyStrength * 100) / iEnemyStrength;
	iRatio = min(1000, iRatio);
	uliRtnValue *= iRatio;

	if (target.m_pTargetCity->IsOriginalCapital())
	{
		uliRtnValue *= GC.getAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL();
		uliRtnValue /= 100;
	}

	if (target.m_pTargetCity->getOriginalOwner() == m_pPlayer->GetID())
	{
		uliRtnValue *= GC.getAI_MILITARY_RECAPTURING_OWN_CITY();
		uliRtnValue /= 100;
	}
	
#if defined(MOD_DIPLOMACY_CITYSTATES)
	if (MOD_DIPLOMACY_CITYSTATES) {
		for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinorLoop;
			if (target.m_pTargetCity->getOriginalOwner() == eMinor)
			{
				CvPlayer* pMinor = &GET_PLAYER(eMinor);
				if(pMinor->GetMinorCivAI()->GetQuestData1(m_pPlayer->GetID(), MINOR_CIV_QUEST_LIBERATION) == eMinor)
				{
					uliRtnValue *= GC.getAI_MILITARY_RECAPTURING_CITY_STATE();
					uliRtnValue /= 100;
				}
			}
		}
	}
#endif

	// Don't want it to already be targeted by an operation that's not well on its way
	if(m_pPlayer->IsCityAlreadyTargeted(target.m_pTargetCity, NO_DOMAIN, 50))
	{
		uliRtnValue /= 10;
	}

	uliRtnValue /= 1000;

	// Economic value of target
	unsigned long int iEconomicValue = 1 + (target.m_pTargetCity->getPopulation() / 3);
	// TODO: unhardcode this
	// filter out all but the most productive
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_FOOD, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_PRODUCTION, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_SCIENCE, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_GOLD, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_CULTURE, false) / 10;
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_FAITH, false) / 10;
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_TOURISM, false) / 10;
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
	iEconomicValue += target.m_pTargetCity->getYieldRateTimes100(YIELD_GOLDEN_AGE_POINTS, false) / 10;
#endif
	uliRtnValue *= iEconomicValue;

	uliRtnValue /= 10;

	return min(10000000, (int)uliRtnValue & 0x7fffffff);
}

#endif //MOD_BALANCE_CORE_MILITARY

/// How open an approach do we have to this city if we want to attack it?
CityAttackApproaches CvMilitaryAI::EvaluateMilitaryApproaches(CvCity* pCity, bool bAttackByLand, bool bAttackBySea)
{
	CvPlot* pLoopPlot;
	CityAttackApproaches eRtnValue = ATTACK_APPROACH_UNRESTRICTED;
	int iNumBlocked = 0;

#if defined(MOD_BALANCE_CORE_MILITARY)
	//this returns the _largest_ water area
	CvArea* pCityOcean = pCity->plot()->waterArea();
	CvArea* pCityContinent = pCity->plot()->area();

	//Expanded to look at three hexes around each city - will give a better understanding of approach.
	int iNumPlots = 0;
	int iNumTough = 0;
	int iTotal = 0;
	int iDX = 0;
	int iDY = 0;
	int iRange = 3;
	for(iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for(iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXYWithRangeCheck(pCity->getX(), pCity->getY(), iDX, iDY, iRange);
			iNumPlots++;

			// Blocked if edge of map
			if(pLoopPlot == NULL)
			{
				iNumBlocked++;
				continue;
			}

			bool bBlocked = false;
			bool bHarmful = false;
			bool bTough = false;

			//cannot go here
			if(pLoopPlot->isImpassable() || pLoopPlot->isCity())
				bBlocked = true;

			//should not go here
			if ( pLoopPlot->IsNearEnemyCitadel( GetPlayer()->GetID() ) )
				bHarmful = true;

			//makes us slow
			if(	pLoopPlot->isHills()		|| 
				pLoopPlot->IsRoughFeature()	||
				pLoopPlot->isRiver() )
				bTough = true;

			//other continent?
			if ( !pLoopPlot->isWater() && pLoopPlot->area() != pCityContinent )
				bBlocked = true;

			//other ocean/lake?
			if ( pLoopPlot->isWater() && pLoopPlot->area() != pCityOcean )
				bBlocked = true;

			if(bAttackByLand && !bAttackBySea)
				//siege weapons cannot set up here
				if(	pLoopPlot->isWater() )
					bBlocked = true;

			if(bAttackBySea && !bAttackByLand)
				//ships cannot go here
				if( !pLoopPlot->isWater() )
					bBlocked = true;

			//todo: what about air attack?

			if (bBlocked)
				iNumBlocked++;
			else if (bHarmful)
				iNumTough++;
			else if (bTough)
				iNumTough++;
		}
	}
	iNumBlocked = (iNumTough / 8) + iNumBlocked;
	iTotal = (iNumBlocked * 100) / /*36*/ iNumPlots;
	//We want a number between 0 and 100
	if (iTotal<10)
		eRtnValue = ATTACK_APPROACH_UNRESTRICTED;
	else if (iTotal<30)
		eRtnValue = ATTACK_APPROACH_OPEN;
	else if (iTotal<50)
		eRtnValue = ATTACK_APPROACH_NEUTRAL;
	else if (iTotal<70)
		eRtnValue = ATTACK_APPROACH_LIMITED;
	else if (iTotal<90)
		eRtnValue = ATTACK_APPROACH_RESTRICTED;
	else
		eRtnValue = ATTACK_APPROACH_NONE;
#else
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
					if(pLoopPlot->isImpassable())
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
#endif
	return eRtnValue;
}

/// Find the port operation operations against this enemy should leave from
CvCity* CvMilitaryAI::GetNearestCoastalCity(PlayerTypes eEnemy) const
{
	CvCity* pBestCoastalCity = NULL;
	CvCity* pLoopCity, *pEnemyCity;
	int iLoop, iEnemyLoop;
	int iBestDistance = MAX_INT;
#if defined(MOD_BALANCE_CORE)
	bool bIsBad = false;
#endif

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
#if defined(MOD_BALANCE_CORE)
		if(pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
#else
		if(pLoopCity->isCoastal())
#endif
		{
			for(pEnemyCity = GET_PLAYER(eEnemy).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eEnemy).nextCity(&iEnemyLoop))
			{
				// Check all revealed enemy cities
#if defined(MOD_BALANCE_CORE)
				if(pEnemyCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
#else
				if(pEnemyCity->isCoastal() && pEnemyCity->plot()->isRevealed(m_pPlayer->getTeam()))
#endif
				{
					// On same body of water?
					if(OnSameBodyOfWater(pLoopCity, pEnemyCity))
					{
#if defined(MOD_BALANCE_CORE)
						CvAStarNode* pNode;
						CvPlot* pCurrentPlot;		
						if(GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, pLoopCity->plot(), pEnemyCity->plot()))
						{
							GC.getStepFinder().SetData(&eEnemy);
							bool bRoute = GC.getStepFinder().GeneratePath(pLoopCity->getX(), pLoopCity->getY(), pEnemyCity->getX(), pEnemyCity->getY(), m_pPlayer->GetID(), false);
							{
								if(bRoute)
								{
									pNode = GC.getStepFinder().GetLastNode();

									while(pNode)
									{
										pCurrentPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);

										if(pCurrentPlot != NULL)
										{
											if(pCurrentPlot->getPlotType() == PLOT_OCEAN && !GET_TEAM(m_pPlayer->getTeam()).canEmbarkAllWaterPassage())
											{
												bIsBad = true;
												break;
											}
										}
										pNode = pNode->m_pParent;
									}
								}
							}
						}
						if(!bIsBad)
						{
#endif
						int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pEnemyCity->getX(), pEnemyCity->getY());
						if(iDistance < iBestDistance)
						{
							iBestDistance = iDistance;
							pBestCoastalCity = pLoopCity;
						}
#if defined(MOD_BALANCE_CORE)
						}
#endif
					}
				}
			}
		}
	}

	return pBestCoastalCity;
}
#if defined(MOD_BALANCE_CORE_MILITARY)
/// Find the port operation operations against this enemy should attack
CvCity* CvMilitaryAI::GetNearestCoastalCityEnemy(PlayerTypes eEnemy) const
{
	CvCity* pBestCoastalCity = NULL;
	CvCity* pLoopCity, *pEnemyCity;
	int iLoop, iEnemyLoop;
	int iBestDistance = MAX_INT;
	bool bIsBad = false;

	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			for(pEnemyCity = GET_PLAYER(eEnemy).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eEnemy).nextCity(&iEnemyLoop))
			{
				// Check all revealed enemy cities
				if(pEnemyCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				{
					// On same body of water?
					if(OnSameBodyOfWater(pLoopCity, pEnemyCity))
					{
						CvAStarNode* pNode;
						CvPlot* pCurrentPlot;		
						if(GC.getStepFinder().DoesPathExist(m_pPlayer->GetID(), eEnemy, pLoopCity->plot(), pEnemyCity->plot()))
						{
							GC.getStepFinder().SetData(&eEnemy);
							bool bRoute = GC.getStepFinder().GeneratePath(pLoopCity->getX(), pLoopCity->getY(), pEnemyCity->getX(), pEnemyCity->getY(), m_pPlayer->GetID(), false);
							{
								if(bRoute)
								{
									pNode = GC.getStepFinder().GetLastNode();

									while(pNode)
									{
										pCurrentPlot = GC.getMap().plotCheckInvalid(pNode->m_iX, pNode->m_iY);

										if(pCurrentPlot != NULL)
										{
											if(pCurrentPlot->getPlotType() == PLOT_OCEAN && !GET_TEAM(m_pPlayer->getTeam()).canEmbarkAllWaterPassage())
											{
												bIsBad = true;
												break;
											}
										}
										pNode = pNode->m_pParent;
									}
								}
							}
						}
						if(!bIsBad)
						{
							int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pEnemyCity->getX(), pEnemyCity->getY());
							if(iDistance < iBestDistance)
							{
								iBestDistance = iDistance;
								pBestCoastalCity = pEnemyCity;
							}
						}
					}
				}
			}
		}
	}

	return pBestCoastalCity;
}
#endif
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

	// Major threat for each camp seen
#if defined(MOD_BALANCE_CORE)
	iRtnValue += GC.getAI_MILITARY_THREAT_WEIGHT_SEVERE() * m_iBarbarianCampCount;
#else
	iRtnValue += GC.getAI_MILITARY_THREAT_WEIGHT_MAJOR() * m_iBarbarianCampCount;
#endif

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
#if defined(MOD_BALANCE_CORE)
int CvMilitaryAI::GetNumberCivsAtWarWith(bool bMinor) const
#else
int CvMilitaryAI::GetNumberCivsAtWarWith() const
#endif
{
	PlayerTypes eLoopPlayer;
#if !defined(MOD_BALANCE_CORE)
	WarStateTypes eWarState;
#endif
	int iRtnValue = 0;

	// Let's figure out if we're at war
#if defined(MOD_BALANCE_CORE)
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
#else
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
#endif
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
#if defined(MOD_BALANCE_CORE)
		if(GET_PLAYER(eLoopPlayer).isMinorCiv() && !bMinor)
		{
			continue;
		}
		if(eLoopPlayer != m_pPlayer->GetID())
		{
			if(GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
			{
#else
		if(eLoopPlayer != m_pPlayer->GetID() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);
			if(eWarState != NO_WAR_STATE_TYPE)
			{
#endif
				iRtnValue++;
			}
		}
	}

	return iRtnValue;
}

/// Which city is in the most danger now?
CvCity* CvMilitaryAI::GetMostThreatenedCity(int iOrder)
{
#if defined(MOD_BALANCE_CORE_MILITARY)
	std::vector<std::pair<CvCity*,int>> vCities;
	struct sort_pred {
		bool operator()(const std::pair<CvCity*,int> &left, const std::pair<CvCity*,int> &right) {
			return left.second > right.second;
		}
	};

	CvCity* pLoopCity;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		//with the new danger plots, the stored threat value should be accurate
		int iThreatValue = pLoopCity->getThreatValue();

		//ignore it if attack would be pointless - times two because a long siege will favor the city
		if (iThreatValue < GC.getCITY_HIT_POINTS_HEALED_PER_TURN()*2)
			continue;

		//scale it a bit with city value and remaining hitpoints
		float fScale = 1000 * sqrt((float)pLoopCity->getPopulation()) / MAX(1,pLoopCity->GetMaxHitPoints() - pLoopCity->getDamage());

		//OutputDebugString( CvString::format("%s - threat %d - scale %.3f\n", pLoopCity->getName().c_str(), iThreatValue, fScale ).c_str() );

		// Is this a temporary zone city? If so, we need to support it ASAP.
		if(m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pLoopCity))
		{
			iThreatValue = (iThreatValue * GC.getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL() * 2) / 100;
		}
		else if(pLoopCity->isCapital())
		{
			iThreatValue = (iThreatValue * GC.getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL()) / 100;
		}

		vCities.push_back( std::make_pair( pLoopCity, (int)(iThreatValue*fScale) ) );
	}

	std::sort(vCities.begin(), vCities.end(), sort_pred());
	if (iOrder<0 || iOrder>=(int)vCities.size()) 
		return NULL;
	else
		return vCities[iOrder].first;

#else

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
#endif
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

#if defined(MOD_BALANCE_CORE_MILITARY)
	//Look at neighbors - if they're stronger than us, let's increase our amount.
	PlayerTypes eOtherPlayer;
	int iNumParity = 0;
	int iHighestParity = 0;
	int iNumOwnedArmyUnits = 0;
	int iNumTheirArmyUnits = 0;
	if(MOD_BALANCE_CORE_MILITARY && !m_pPlayer->isMinorCiv())
	{
		for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
		{
			if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->IsCombatUnit())
			{
				iNumOwnedArmyUnits++;
			}
		}
		for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			eOtherPlayer = (PlayerTypes) iMajorLoop;
			if(eOtherPlayer != NO_PLAYER && !GET_PLAYER(eOtherPlayer).isMinorCiv() && (eOtherPlayer != m_pPlayer->GetID()))
			{
				MajorCivApproachTypes eApproachType = GetPlayer()->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false);
				if((eApproachType < MAJOR_CIV_APPROACH_GUARDED) && (GET_PLAYER(eOtherPlayer).GetProximityToPlayer(GetPlayer()->GetID()) >= PLAYER_PROXIMITY_CLOSE))
				{
					for(CvUnit* pLoopUnit = GET_PLAYER(eOtherPlayer).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(eOtherPlayer).nextUnit(&iLoop))
					{
						if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->IsCombatUnit())
						{
							iNumTheirArmyUnits++;
						}
					}
					//Let's try to achieve military parity with the largest number of enemy units.
					iNumParity = (iNumTheirArmyUnits - iNumOwnedArmyUnits);
					if(iNumParity > iHighestParity)
					{
						iHighestParity = iNumParity;
					}
				}
			}
		}
	}
#endif

	// now we add in the strike forces we think we will need
	if(m_pPlayer->isMinorCiv())
	{
		m_iMandatoryReserveSize = max(1,m_iMandatoryReserveSize/2);
		iNumUnitsWanted = 0;
	}
	else
	{
#if defined(MOD_BALANCE_CORE_MILITARY)
		iNumUnitsWanted = /*8*/ GC.getBALANCE_BASIC_ATTACK_ARMY_SIZE(); // size of a basic attack - embiggened!
#else
		iNumUnitsWanted = 7; // size of a basic attack 
#endif

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
#if defined(MOD_BALANCE_CORE_MILITARY)
		iHighestParity *= 2;
		iHighestParity /= 3;
#endif
	}

	m_iRecommendedMilitarySize = m_iMandatoryReserveSize + iNumUnitsWanted;
#if defined(MOD_BALANCE_CORE_MILITARY)
	if((m_iRecommendedMilitarySize + iHighestParity) <= GetPlayer()->GetNumUnitsSupplied())
	{
		m_iRecommendedMilitarySize += iHighestParity;
	}
	//We don't want to go over this, but we need every troop we can muster.
	else
	{
		m_iRecommendedMilitarySize = GetPlayer()->GetNumUnitsSupplied();
	}
#endif
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

				// Count it as 10 camps if sitting inside our territory, that is annoying!
				if(pPlot->getOwner() == m_pPlayer->GetID())
				{
					m_iBarbarianCampCount += 9;
				}
#if defined(MOD_BALANCE_CORE)
				// Count it as -5 camps if sitting inside someone else's territory.
				if(pPlot->getOwner() != m_pPlayer->GetID() && pPlot->getOwner() != NO_PLAYER)
				{
					m_iBarbarianCampCount -= 5;
				}
#endif

				// See how close it is to each of our cities, if less than 10 tiles, treat it as 5 camps
				else
				{
					int iTolerableDistance = 10;
					int iCityLoop;
					CvCity *pLoopCity;
					for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
					{
						if (pLoopCity)
						{
							int iDist = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());
							if (iDist < iTolerableDistance)
							{
								m_iBarbarianCampCount += 4;
								break;
							}
						}
					}
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
#if defined(MOD_BALANCE_CORE)
				else if(strStrategyName == "MILITARYAISTRATEGY_ERADICATE_BARBARIANS_CRITICAL")
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EradicateBarbariansCritical(eStrategy, m_pPlayer, m_iBarbarianCampCount, m_iVisibleBarbarianCount);
#endif
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

				// Never found it?  Assume it is scenario-specific and should be on unless disabled by Lua hook
				else
				{
					bStrategyShouldBeActive = true;
				}

				// Check Lua hook
				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if(pkScriptSystem && bStrategyShouldBeActive)
				{
					CvLuaArgsHandle args;
					args->Push(iStrategiesLoop);
					args->Push(m_pPlayer->GetID());

					// Attempt to execute the game events.
					// Will return false if there are no registered listeners.
					bool bResult = false;
					if(LuaSupport::CallTestAll(pkScriptSystem, "MilitaryStrategyCanActivate", args.get(), bResult))
					{
						// Check the result.
						if(bResult == false)
						{
							bStrategyShouldBeActive = false;
						}
					}
				}
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
#if defined(MOD_BALANCE_CORE_MILITARY)
#if defined(MOD_BALANCE_CORE)
	if(m_pPlayer->isMinorCiv() || m_pPlayer->isBarbarian())
	{
		return;
	}
#endif
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateOperations, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	int iOperationID;
	int iNumRequiredSlots;
	int iFilledSlots;

	int iPlayerLoop;
	PlayerTypes eLoopPlayer;
	WarStateTypes eWarState;
	CvAIOperation* pOperation;

	MilitaryAIStrategyTypes eStrategyBarbs = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
	MilitaryAIStrategyTypes eStrategyBarbsCritical = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS_CRITICAL");
	MilitaryAIStrategyTypes eStrategyFightAWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
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
	if(!bWillingToAcceptRisk && IsUsingStrategy(eStrategyBarbsCritical))
	{
		bWillingToAcceptRisk = true;
	}
#if defined(MOD_DIPLOMACY_CITYSTATES_QUESTS)
	int iMinorLoop;
	PlayerTypes eMinor;
	if(MOD_DIPLOMACY_CITYSTATES_QUESTS && !bWillingToAcceptRisk)
	{
		// Defend CSs that need help
		for(iMinorLoop = 0; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
		{
			eMinor = (PlayerTypes) iMinorLoop;
			if(GET_PLAYER(eMinor).isMinorCiv() && GET_PLAYER(eMinor).isAlive())
			{
				TeamTypes eLoopTeam;
				for(int iTeamLoop = 0; iTeamLoop < MAX_CIV_TEAMS; iTeamLoop++)
				{
					eLoopTeam = (TeamTypes) iTeamLoop;

					if(GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(m_pPlayer->GetID()))
					{
						if(GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_HORDE) || GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_REBELLION))
						{
							bWillingToAcceptRisk = true;
							break;
						}
					}
				}
			}
		}
	}
#endif
#if defined(MOD_BALANCE_CORE_MILITARY)
	//Let's hunt barbs if we want to expand
	EconomicAIStrategyTypes eStrategyExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	if(m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyExpandToOtherContinents))
	{
		if((m_iTotalThreatWeight / 4) < GetBarbarianThreatTotal())
		{
			bWillingToAcceptRisk = true;
		}
	}
	EconomicAIStrategyTypes eNeedRecon = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON");
	if(m_pPlayer->GetEconomicAI()->IsUsingStrategy(eNeedRecon))
	{
		if((m_iTotalThreatWeight / 4) < GetBarbarianThreatTotal())
		{
			bWillingToAcceptRisk = true;
		}
	}
	EconomicAIStrategyTypes eEarlyExpansion = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
	if(m_pPlayer->GetEconomicAI()->IsUsingStrategy(eEarlyExpansion))
	{
		if((m_iTotalThreatWeight / 4) < GetBarbarianThreatTotal())
		{
			bWillingToAcceptRisk = true;
		}
	}
#endif
	//
	// Operations vs. Barbarians
	//
	// If have aborted the eradicate barbarian strategy or if the threat level from civs is significantly higher than from barbs, we better abort all of them
#if defined(MOD_BALANCE_CORE)
	if(!IsUsingStrategy(eStrategyBarbs) || (IsUsingStrategy(eStrategyFightAWar) && !bWillingToAcceptRisk))
#else
	if(!IsUsingStrategy(eStrategyBarbs) || IsUsingStrategy(eStrategyFightAWar) || !bWillingToAcceptRisk)
#endif
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
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_BOMBARDMENT, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(!bWillingToAcceptRisk && m_pPlayer->haveAIOperationOfType(AI_OPERATION_ALLY_DEFENSE, &iOperationID))
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
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SUPERIORITY, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_ATTACK, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NUKE_ATTACK, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
		}
	}

	if(m_pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith() > 0)
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
				if(GET_TEAM(m_pPlayer->getTeam()).isForcePeace(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_SNEAK_CITY_ATTACK, &iOperationID, eLoopPlayer))
					{
						m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID, eLoopPlayer))
					{
						m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, &iOperationID, eLoopPlayer))
					{
						m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID, eLoopPlayer))
					{
						m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SNEAK_ATTACK, &iOperationID, eLoopPlayer))
					{
						m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SUPERIORITY, &iOperationID, eLoopPlayer))
					{
						m_pPlayer->getAIOperation(iOperationID)->Kill(AI_ABORT_WAR_STATE_CHANGE);
					}
				}
				eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);
				switch(eWarState)
				{
					// If we are dominant, shouldn't be running a defensive strategy
				case WAR_STATE_NEARLY_WON:
				case WAR_STATE_OFFENSIVE:
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID, eLoopPlayer))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID, eLoopPlayer))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					break;

					// If we are losing, let's save up our pillagers
				case WAR_STATE_DEFENSIVE:
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID, eLoopPlayer))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					break;

					// If nearly defeated, call off all operations in enemy territory
				case WAR_STATE_NEARLY_DEFEATED:
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_BASIC_CITY_ATTACK, &iOperationID, eLoopPlayer))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID, eLoopPlayer))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ATTACK, &iOperationID, eLoopPlayer))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PURE_NAVAL_CITY_ATTACK, &iOperationID, eLoopPlayer))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_SMALL_CITY_ATTACK, &iOperationID, eLoopPlayer))
					{
						pOperation = m_pPlayer->getAIOperation(iOperationID);
						if(pOperation)
						{
							pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, &iOperationID, eLoopPlayer))
					{
						if(GET_PLAYER(eLoopPlayer).isMinorCiv() && GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() != NO_PLAYER)
						{
							pOperation = m_pPlayer->getAIOperation(iOperationID);
							if(pOperation)
							{
								pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
							}
						}
					}
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_ATTACK, &iOperationID, eLoopPlayer))
					{
						if(GET_PLAYER(eLoopPlayer).isMinorCiv() && GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() != NO_PLAYER)
						{
							pOperation = m_pPlayer->getAIOperation(iOperationID);
							if(pOperation)
							{
								pOperation->Kill(AI_ABORT_WAR_STATE_CHANGE);
							}
						}
					}
					break;
				}
			}
		}
	}
	// SEE WHAT OPERATIONS WE SHOULD ADD
	//
	// Operation vs. Barbarians
	//
	// If running the eradicate barbarian strategy, the threat is low (no higher than 1 major threat), we're not at war, /*and we have enough units*/, then launch a new operation.
	// Which one is based on whether or not we saw any barbarian camps
	if(IsUsingStrategy(eStrategyBarbs) && !IsUsingStrategy(eStrategyFightAWar) && bWillingToAcceptRisk)
	{
		bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_DESTROY_BARBARIAN_CAMP, &iOperationID);
		if (!bHasOperationUnderway)
		{
			iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_ANTI_BARBARIAN_TEAM, false, &iNumRequiredSlots);

			// Not willing to build units to get this off the ground
			if(iFilledSlots >= iNumRequiredSlots)
			{
				// We should have AI build for this
				m_pPlayer->addAIOperation(AI_OPERATION_DESTROY_BARBARIAN_CAMP);
			}
		}
		bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_BOMBARDMENT, &iOperationID);
		if (!bHasOperationUnderway)
		{
			iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_BOMBARDMENT, true, &iNumRequiredSlots);

			// Not willing to build units to get this off the ground
			if(iFilledSlots >= iNumRequiredSlots)
			{
				m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_BOMBARDMENT, NO_PLAYER);
			}
		}
	}

	//
	// Operation vs. Other Civs
	//
	// If at war, consider launching an operation
	if (GetNumberCivsAtWarWith() > 0)
	{
		// check nuke launches
		// Loop through each enemy
		for(iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;

			// Is this a player we have relations with?
			if(eLoopPlayer != m_pPlayer->GetID() && GET_PLAYER(eLoopPlayer).getTeam() != m_pPlayer->getTeam() && m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
			{
				bool bLaunchNuke = false;
				eWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer);

				// if we're not at war
				if(!GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					continue;
				}
				// only evaluate nukes when we have nukes and we've declared war on someone
				if (m_pPlayer->getNumNukeUnits() > 0) 
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

				bool bRequestAttack = false;

				StrengthTypes eMilitaryStrength = m_pPlayer->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(eLoopPlayer);

				if(eMilitaryStrength <= STRENGTH_AVERAGE)
				{
					bRequestAttack = true;
				}

				if (!bRequestAttack)
				{
					continue;
				}

				CvMilitaryTarget target;
				int iScore;
				if(!GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					if(eWarState <= WAR_STATE_CALM)
					{
						bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID, eLoopPlayer);
						if (!bHasOperationUnderway)
						{
							iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_RAPID_RESPONSE_FORCE, false, &iNumRequiredSlots);

							// Not willing to build units to get this off the ground
							if (iFilledSlots >= iNumRequiredSlots)
							{
								m_pPlayer->addAIOperation(AI_OPERATION_RAPID_RESPONSE, eLoopPlayer);
							}
						}
					}
					if (eWarState < WAR_STATE_STALEMATE)
					{
						CvCity* pTarget = GetMostThreatenedCity(0);
						if(pTarget != NULL)
						{
							bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID, NO_PLAYER, pTarget->plot());
							if (!bHasOperationUnderway)
							{
								iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_CLOSE_CITY_DEFENSE, false, &iNumRequiredSlots);

								// Gogogo!
								if (iFilledSlots > 0)
								{
									m_pPlayer->addAIOperation(AI_OPERATION_CITY_CLOSE_DEFENSE, NO_PLAYER, -1, pTarget, pTarget);
								}
							}
						}
						pTarget = GetMostThreatenedCity(1);
						if(pTarget != NULL)
						{
							bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID, NO_PLAYER, pTarget->plot());
							if (!bHasOperationUnderway)
							{
								iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_CLOSE_CITY_DEFENSE, false, &iNumRequiredSlots);

								//Gogogogo....lightly.
								if (iFilledSlots > (iNumRequiredSlots / 2))
								{
									m_pPlayer->addAIOperation(AI_OPERATION_CITY_CLOSE_DEFENSE, NO_PLAYER, -1, pTarget, pTarget);
								}
							}
						}
						pTarget = GetMostThreatenedCity(2);
						if(pTarget != NULL)
						{
							bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID, NO_PLAYER, pTarget->plot());
							if (!bHasOperationUnderway)
							{
								iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_CLOSE_CITY_DEFENSE, false, &iNumRequiredSlots);

								// Not willing to build units to get this off the ground
								if (iFilledSlots >= iNumRequiredSlots)
								{
									m_pPlayer->addAIOperation(AI_OPERATION_CITY_CLOSE_DEFENSE, NO_PLAYER, -1, pTarget, pTarget);
								}
							}
						}
					}
					else if (eWarState >= WAR_STATE_STALEMATE)
					{
						if(GET_PLAYER(eLoopPlayer).getCapitalCity() != NULL && m_pPlayer->getCapitalCity() != NULL)
						{	
							if(GET_PLAYER(eLoopPlayer).getCapitalCity()->getArea() == m_pPlayer->getCapitalCity()->getArea())
							{
#if defined(MOD_BALANCE_CORE)
								iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, (MultiunitFormationTypes)MilitaryAIHelpers::GetBestFormationType(), false, &iNumRequiredSlots);
#else
								iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_BASIC_CITY_ATTACK_FORCE, false, &iNumRequiredSlots);
#endif
								// Not willing to build units to get this off the ground
								if (iFilledSlots >= iNumRequiredSlots)
								{
									target = FindBestAttackTarget2(AI_OPERATION_BASIC_CITY_ATTACK, eLoopPlayer, &iScore);
									{
										if(target.m_pTargetCity && !target.m_bAttackBySea)
										{
											m_pPlayer->addAIOperation(AI_OPERATION_BASIC_CITY_ATTACK, eLoopPlayer, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
										}
										else if(target.m_pTargetCity && target.m_bAttackBySea)
										{
												m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ATTACK, eLoopPlayer, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
										}
									}
								}

								bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID);
								if (!bHasOperationUnderway)
								{
									iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_FAST_PILLAGERS, false, &iNumRequiredSlots);

									// Not willing to build units to get this off the ground
									if (iFilledSlots >= iNumRequiredSlots)
									{
										m_pPlayer->addAIOperation(AI_OPERATION_PILLAGE_ENEMY, eLoopPlayer);
									}
								}
							}
							else
							{
								iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_INVASION, true, &iNumRequiredSlots);
								// Not willing to build units to get this off the ground
								if (iFilledSlots >= iNumRequiredSlots)
								{
									target = FindBestAttackTarget2(AI_OPERATION_NAVAL_ATTACK, eLoopPlayer, &iScore);
									{
										if(target.m_pTargetCity && target.m_bAttackBySea)
										{
											m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ATTACK, eLoopPlayer, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
										}
										else if(target.m_pTargetCity && !target.m_bAttackBySea)
										{
											m_pPlayer->addAIOperation(AI_OPERATION_BASIC_CITY_ATTACK, eLoopPlayer, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
										}
									}
								}
							}
							int iFlavorNaval = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
							RequestPureNavalAttack(eLoopPlayer, (iFlavorNaval / 2));
						}
					}

					CvCity* pLoopCity;
					int iCityLoop;
					int iHighestThreat = 0;
					int iHighestDamage = 0;
					int iMostBad = 0;
					CvCity* pTargetCity = NULL;
					iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_SQUADRON, true, &iNumRequiredSlots);
					//Let's look for a coastal city in danger, and send some support over if we can.
					for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
					{
						if(pLoopCity != NULL)
						{
							if(pLoopCity->isCoastal())
							{
								if (!m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SUPERIORITY, &iOperationID, eLoopPlayer, pLoopCity->plot()))
								{
									if(pLoopCity->getThreatValue() > iHighestThreat)
									{
										iHighestThreat = pLoopCity->getThreatValue();
										pTargetCity = pLoopCity;
									}
									if(pLoopCity->getDamage() > iHighestDamage)
									{
										iHighestDamage = pLoopCity->getDamage();
										pTargetCity = pLoopCity;
									}
									int iDX, iDY;
									int iRange = 3;
									int iBad = 0;
									for(iDX = -(iRange); iDX <= iRange; iDX++)
									{
										for(iDY = -(iRange); iDY <= iRange; iDY++)
										{
											CvPlot* pLoopPlot = plotXYWithRangeCheck(pLoopCity->getX(), pLoopCity->getY(), iDX, iDY, iRange);

											if(pLoopPlot != NULL)
											{
												if(pLoopPlot->GetNumCombatUnits() > 0)
												{
													if(pLoopPlot->getUnitByIndex(0)->getOwner() == eLoopPlayer)
													{
														iBad++;
													}
												}
											}
										}
									}
									if(iBad > iMostBad)
									{
										iMostBad = iBad;
										pTargetCity = pLoopCity;
									}
								}
							}
						}
					}
					if(pTargetCity == NULL)
					{
						//All our cities are safe?
						//Let's look for an enemy coastal city and harass it.
						CvCity* pEnemyCity;
						int iLoop, iEnemyLoop;
						int iBestDistance = MAX_INT;
						int iMostBad = MAX_INT;
						for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
						{
							if(pLoopCity->isCoastal())
							{
								for(pEnemyCity = GET_PLAYER(eLoopPlayer).firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = GET_PLAYER(eLoopPlayer).nextCity(&iEnemyLoop))
								{
									// Check all revealed enemy cities
									if(pEnemyCity->isCoastal())
									{	
										if (!m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SUPERIORITY, &iOperationID, eLoopPlayer, pEnemyCity->plot()))
										{
											// On same body of water?
											if(OnSameBodyOfWater(pLoopCity, pEnemyCity))
											{
												int iDX, iDY;
												int iRange = 3;
												int iBad = 0;
												for(iDX = -(iRange); iDX <= iRange; iDX++)
												{
													for(iDY = -(iRange); iDY <= iRange; iDY++)
													{
														CvPlot* pLoopPlot = plotXYWithRangeCheck(pEnemyCity->getX(), pEnemyCity->getY(), iDX, iDY, iRange);

														if(pLoopPlot != NULL)
														{
															if(pLoopPlot->GetNumCombatUnits() > 0)
															{
																if(pLoopPlot->getUnitByIndex(0)->getOwner() == eLoopPlayer && (pLoopPlot->getUnitByIndex(0)->getDomainType() == DOMAIN_SEA))
																{
																	iBad++;
																}
															}
														}
													}
												}
												int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pEnemyCity->getX(), pEnemyCity->getY());
												if((iDistance < iBestDistance) && (iBad < iMostBad))
												{
													iMostBad = iBad;
													iBestDistance = iDistance;
													pTargetCity = pEnemyCity;
												}
											}
										}
									}
								}
							}
						}									
					}
					if(pTargetCity != NULL)
					{	
						if(iFilledSlots >= iNumRequiredSlots)
						{
							CvCity* pLoopMusterCity = GetNearestCoastalCity(eLoopPlayer);
							if(pLoopMusterCity != NULL)
							{
								m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, eLoopPlayer, pTargetCity->getArea(), pTargetCity, pLoopMusterCity);
							}
						}
					}
				}
				else if(GET_PLAYER(eLoopPlayer).isMinorCiv() && eWarState >= WAR_STATE_DEFENSIVE)
				{
					target = FindBestAttackTarget2(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, eLoopPlayer, &iScore);
					if(target.m_pTargetCity && target.m_bAttackBySea)
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_CITY_STATE_INVASION, true, &iNumRequiredSlots);
						// Not willing to build units to get this off the ground
						if (iFilledSlots >= iNumRequiredSlots)
						{
							m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_NAVAL_ATTACK, eLoopPlayer, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
						}
					}
					else if(target.m_pTargetCity && !target.m_bAttackBySea)
					{
#if defined(MOD_BALANCE_CORE)
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, (MultiunitFormationTypes)MilitaryAIHelpers::GetBestFormationType(), false, &iNumRequiredSlots);
#else
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_BASIC_CITY_ATTACK_FORCE, false, &iNumRequiredSlots);
#endif
						// Not willing to build units to get this off the ground
						if (iFilledSlots >= iNumRequiredSlots)
						{
							m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_ATTACK, eLoopPlayer, target.m_pTargetCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
						}
					}
				}
			}
		}
	}
#else
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
					if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID))
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
			static CvWeightedVector<CvMilitaryTarget, SAFE_ESTIMATE_NUM_CITIES* 10, true> weightedTargetList;
			weightedTargetList.clear();

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
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE, false, &iNumRequiredSlots, &iLandReservesUsed);
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
	}
	//
	// Naval operations (vs. opportunity targets)
	//
	// Total number of these operations can't exceed (FLAVOR_NAVAL / 2)
	int iFlavorNaval = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
	int iNumSuperiority = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_SUPERIORITY);
	int iNumBombard = m_pPlayer->numOperationsOfType(AI_OPERATION_NAVAL_BOMBARDMENT);
	int iMaxOperations = iFlavorNaval + 1;
	int iMaxOperations = iFlavorNaval;
	iMaxOperations *= 2;
	iMaxOperations /= 3;
	if(GC.getMap().GetAIMapHint() & 1)
	{
		iMaxOperations *= 2;
	}
	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_SQUADRON, true, &iNumRequiredSlots);

	// Not willing to build units to get this off the ground
	if(iFilledSlots >= iNumRequiredSlots)
	{
		// If I have a colonization operation underway, start up naval superiority as extra escorts
		if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_QUICK_COLONIZE, &iOperationID))
		{
			m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, NO_PLAYER);
			return;
		}
		else if (IsUsingStrategy(eStrategyFightAWar))
		{
			m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, NO_PLAYER);
			return;
		}
	}
	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_BOMBARDMENT, true, &iNumRequiredSlots);

	// Not willing to build units to get this off the ground
	if(iFilledSlots >= iNumRequiredSlots)
	{
		if(iNumBombard == 0 && IsUsingStrategy(eStrategyBarbs))
		{
			m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_BOMBARDMENT, NO_PLAYER);
			return;
		}
	}
#endif
}

/// Spend money on units/buildings for military contingencies
//  NOTE: The defensive side of this is done in dominance zone processing in the Tactical AI; this is spending to speed operations
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

/// Spend money on units/buildings to supply units that can fuel future military operations
void CvMilitaryAI::MakeOffensivePurchases()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("MakeOffensivePurchases, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	CvUnit *pUnit;

	if (m_pPlayer->isMinorCiv())
	{
		return;
	}

	// Are we winning all the wars we are in?
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(!IsUsingStrategy(eStrategyAtWar) || m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		// Do we have a high offensive personality flavor and our military could be larger?
		if (m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) >= 7 &&
			(GetPercentOfRecommendedMilitarySize() < 100 || m_eNavalDefenseState > DEFENSE_STATE_ENOUGH))
		{
			// Do we have operations running (if so let them recruit the units)?
			if (GetArmyBeingBuilt() == NO_ARMY_TYPE)
			{
				CvCity *pCity = m_pPlayer->getCapitalCity();
				if (pCity == NULL)
				{
					return;
				}

				// Do we need naval units most?
				bool bNeedNaval = m_eNavalDefenseState > m_eLandDefenseState;
				if (bNeedNaval)
				{
					// Get a different city if capital is not coastal
					if (!pCity->isCoastal())
					{
						CvCity* pLoopCity;
						int iCityLoop;
						bNeedNaval = false;
						for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL && !bNeedNaval; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
						{
							if(pLoopCity->isCoastal())
							{
								bNeedNaval = true;
								pCity = pLoopCity;
							}
						}
					}
				}

				if (bNeedNaval)
				{
					pUnit = BuyEmergencyUnit(UNITAI_ASSAULT_SEA, pCity);
					if (!pUnit)
					{
						pUnit = BuyEmergencyUnit(UNITAI_ATTACK_SEA, pCity);
					}
				}
				else
				{
					if (m_iNumMeleeLandUnits <= m_iNumRangedLandUnits)
					{
						pUnit = BuyEmergencyUnit(UNITAI_FAST_ATTACK, pCity);
						if (!pUnit)
						{
							pUnit = BuyEmergencyUnit(UNITAI_ATTACK, pCity);
							if (!pUnit)
							{
								pUnit = BuyEmergencyUnit(UNITAI_DEFENSE, pCity);
							}
						}
					}
					else
					{
						pUnit = BuyEmergencyUnit(UNITAI_CITY_BOMBARD, pCity);
						if (!pUnit)
						{
							pUnit = BuyEmergencyUnit(UNITAI_RANGED, pCity);
						}
					}
				}
			}
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
#if defined(MOD_BALANCE_CORE)
	if(GetNumberCivsAtWarWith(false) > 0)
#else
	if(GetNumberCivsAtWarWith() > 0)
#endif
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

#if defined(MOD_BALANCE_CORE_MILITARY)
	if(eOperationType == AI_OPERATION_SNEAK_CITY_ATTACK || eOperationType == AI_OPERATION_NAVAL_SNEAK_ATTACK)
	{
		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eFormation, bRequiresNavalMoves, &iNumRequiredSlots, &iLandReservesUsed);
		if(iFilledSlots > 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
#endif
	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eFormation, bRequiresNavalMoves, &iNumRequiredSlots, &iLandReservesUsed);
	if(iFilledSlots >= iNumRequiredSlots)
	{
		return true;
	}
	else
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	}
#endif
}

/// Score the strength of the units for a domain; best candidate to scrap (with lowest score) is returned. Only supports land and naval units
UnitHandle CvMilitaryAI::FindBestUnitToScrap(bool bLand, bool bDeficitForcedDisband, int& iReturnedScore)
{
	CvUnit* pLoopUnit;
	int iUnitLoop;
	UnitHandle pBestUnit;
	int iScore;
	int iBestScore = MAX_INT;

#if defined(MOD_BALANCE_CORE)

	for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		bool bIsUseless = false;
		bool bStillNeeded = false;

		if(!pLoopUnit->IsCombatUnit())
			continue;

		if(bLand && pLoopUnit->getDomainType() != DOMAIN_LAND)
			continue;

		if(!bLand && pLoopUnit->getDomainType() != DOMAIN_SEA)
			continue;

		// Following checks are for the case where the AI is trying to decide if it is a good idea to disband this unit (as opposed to when the game is FORCING the player to disband one)
		if(!bDeficitForcedDisband)
		{
			//needed later
			CvUnitEntry& pUnitInfo = pLoopUnit->getUnitInfo();

			if(bLand && m_eLandDefenseState == DEFENSE_STATE_CRITICAL)
				bStillNeeded = true;
			else if(!bLand && m_eNavalDefenseState == DEFENSE_STATE_CRITICAL)
				bStillNeeded = true;

			// Is it in an army?
			if(pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
				bStillNeeded = true;

			// Can I still build this unit? If so too new to scrap
			if(bLand && m_pPlayer->canTrain(pLoopUnit->getUnitType(), false /*bContinue*/, true /*bTestVisible*/, true /*bIgnoreCost*/))
				//But not for scouts - let's pick those off first.
				if(bLand && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE)
				{
					bStillNeeded = true;
				}
				else if(!bLand && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
				{
					bStillNeeded = true;
				}

			// Is this a ship on a water body without enemies?
			if (!bLand)
			{
				CvArea* pWaterBody = pLoopUnit->plot()->waterArea();
				if (pWaterBody)
				{
					int iForeignCities = pWaterBody->getNumCities() - pWaterBody->getCitiesPerPlayer(m_pPlayer->GetID());
					int iForeignUnits = pWaterBody->getNumUnits() - pWaterBody->getUnitsPerPlayer(m_pPlayer->GetID());

					if (iForeignCities == 0 && iForeignUnits == 0)
						bIsUseless = true;
				}
			}

			//Failsafe to keep AI from deleting advanced start settlers
			//Probably useless because of the combat unit check above
			if(m_pPlayer->GetNumCitiesFounded() < 3)
				if(pUnitInfo.IsFound() || pUnitInfo.IsFoundAbroad())
					bStillNeeded = true;

			// Is this a unit who has an obsolete tech that I have researched?
			if((TechTypes)pUnitInfo.GetObsoleteTech() != NO_TECH && !GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetObsoleteTech())))
				bStillNeeded = true;

			// Is this unit's INTRINSIC power less than half that of the best unit I can build for this domain?
			if((pLoopUnit->getUnitInfo().GetPower() * 2) >= GetPowerOfStrongestBuildableUnit(pLoopUnit->getDomainType()))
				bStillNeeded = true;

			// Does this unit's upgrade require a resource?
			UnitTypes eUpgradeUnit = pLoopUnit->GetUpgradeUnitType();
			if(eUpgradeUnit != NO_UNIT)
			{
				CvUnitEntry* pUpgradeUnitInfo = GC.GetGameUnits()->GetEntry(eUpgradeUnit);
				if(pUpgradeUnitInfo != NULL)
				{
					for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
					{
						ResourceTypes eResource = (ResourceTypes) iResourceLoop;
						int iNumResourceNeeded = pUpgradeUnitInfo->GetResourceQuantityRequirement(eResource);

						// Minor issue: this only works correctly if a unit has only one required resource ...
						if(iNumResourceNeeded > 0)
						{
							if(m_pPlayer->getNumResourceTotal(eResource) > 0)
							{
								// We'll wait and try to upgrade this one, our unit count isn't that bad
								if(bLand && m_eLandDefenseState > DEFENSE_STATE_NEUTRAL)
									bStillNeeded = true;
								else if(!bLand && m_eNavalDefenseState > DEFENSE_STATE_NEUTRAL)
									bStillNeeded = true;
							}
						}
					}
				}
			}
		}

		// Can I scrap this unit?
		if( (!bStillNeeded || bIsUseless) && pLoopUnit->canScrap())
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

#else

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
#endif

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
#if defined(MOD_BALANCE_CORE)
		eFormation = (MultiunitFormationTypes)MilitaryAIHelpers::GetBestFormationType();
#else
		eFormation = (GC.getGame().getHandicapInfo().GetID() > 4 && !(GC.getMap().GetAIMapHint() & 1)) ? MUFORMATION_BIGGER_CITY_ATTACK_FORCE : MUFORMATION_BASIC_CITY_ATTACK_FORCE;
#endif
	}
	UnitAITypes eUnitAIType = MilitaryAIHelpers::FirstSlotCityCanFill(m_pPlayer, eFormation, (m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION), pCity->isCoastal(), false /*bSecondaryUnit*/);
#if defined(MOD_BALANCE_CORE)
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType, true, true);
#else	
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType);
#endif
	if(eType == NO_UNIT)
	{
		eUnitAIType = MilitaryAIHelpers::FirstSlotCityCanFill(m_pPlayer, eFormation, (m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION), pCity->isCoastal(), true /*bSecondaryUnit*/);
#if defined(MOD_BALANCE_CORE)
		eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType, true, true);
#else			
		eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType);
#endif
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
		if (pUnitPlot->getPlotCity()->getDamage() > (pUnitPlot->getPlotCity()->GetMaxHitPoints() / 5))
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
#if defined(MOD_BALANCE_CORE_MILITARY)
	if(MOD_BALANCE_CORE_MILITARY)
	{
		//Is this a bomber that lacks useful missions?
		if (pUnit->canAirAttack() && !pUnit->canAirSweep())
		{
			int iDX, iDY;
			int iRange = pUnit->GetRange();
			bool bEnemyCity = false;
			bool bEnemyUnit = false;
			for(iDX = -(iRange); iDX <= iRange; iDX++)
			{
				for(iDY = -(iRange); iDY <= iRange; iDY++)
				{
					CvPlot* pLoopPlot = plotXYWithRangeCheck(pUnit->getX(), pUnit->getY(), iDX, iDY, iRange);

					if(pLoopPlot != NULL)
					{
						//Is there an enemy city nearby?
						if(pLoopPlot->isCity() && (pLoopPlot->getOwner() != pUnit->getOwner()))
						{
							if(atWar(GET_PLAYER(pLoopPlot->getOwner()).getTeam(), m_pPlayer->getTeam()))
							{
								//There's a target here.
								bEnemyCity = true;
								break;
							}
						}
						//What about a unit?
						if(pLoopPlot->getNumUnits() > 0)
						{
							CvUnit* pUnit = pLoopPlot->getUnitByIndex(0);
							if(pUnit->getOwner() != m_pPlayer->GetID())
							{
								if(atWar(GET_PLAYER(pUnit->getOwner()).getTeam(), m_pPlayer->getTeam()))
								{
									//There's a target here.
									bEnemyUnit = true;
									break;
								}
							}
						}
					}
				}
			}
			if(bEnemyCity || bEnemyUnit)
			{
				bNeedsToMove = false;
			}
			else
			{
				bNeedsToMove = true;
			}
		}
		//Let's stop peace-time shifting of units. It is absurd. Only shift if mustering for an attack or at war.
		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if(GET_PLAYER(eLoopPlayer).isAlive() && !GET_PLAYER(eLoopPlayer).isBarbarian())
			{
				if(GET_TEAM(m_pPlayer->getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
				{
					bNeedsToMove = true;
					break;
				}
				else
				{
					bNeedsToMove = false;
				}
			}
		}
		//Are there air units that have already moved to this city this round? Don't rebase then, that is silly, as you are probably just shuffling them back and forth.
		int iLoopUnit = 0;
		for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
		{
			if(pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				if((pLoopUnit->canRebase(pLoopUnit->plot())) && (pLoopUnit->plot() == pUnit->plot()))
				{
					return false;
				}
			}
		}
	}
#endif

	if (!bNeedsToMove)
	{
		return false;
	}

	// first look for open carrier slots in carriers within operations
	int iLoopUnit = 0;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		CvPlot* pLoopUnitPlot = pLoopUnit->plot();

		if(pLoopUnit->getDamage() > (GC.getMAX_HIT_POINTS() / 5))  // this might not be a good place to land
		{
			continue;
		}

		if (pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
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

	// then look for open carrier slots in carriers NOT in operations
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		CvPlot* pLoopUnitPlot = pLoopUnit->plot();

		if(pLoopUnit->getDamage() > (GC.getMAX_HIT_POINTS() / 5))  // this might not be a good place to land
		{
			continue;
		}

		if (pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
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

		if(pLoopCity->getDamage() > (pLoopCity->GetMaxHitPoints() / 5))
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

#if defined(MOD_AI_SMART_AIR_TACTICS)
/// AMS: Get all possible interceptions on that plot, doesn't use visibility to offset AI inability to remember air attacks.
int CvMilitaryAI::GetMaxPossibleInterceptions(CvPlot* pTargetPlot, bool bCountPercents) const
{
	int iRtnValue = 0;
	int iLoopUnit;
	CvUnit* pLoopUnit;

	// Loop through all the players
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);

		if (kPlayer.isAlive() && kPlayer.GetID() != m_pPlayer->GetID())
		{
			if (atWar(kPlayer.getTeam(), m_pPlayer->getTeam()))
			{
				// Loop through their units looking for intercept capable units
				iLoopUnit = 0;
				for (pLoopUnit = kPlayer.firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoopUnit))
				{
					// Must be able to intercept this turn
					if (!pLoopUnit->isDelayedDeath() && pLoopUnit->canAirDefend() && !pLoopUnit->isInCombat() && !pLoopUnit->isOutOfInterceptions())
					{
						// Must either be a non-air Unit, or an air Unit that hasn't moved this turn and is on intercept duty
						if ((pLoopUnit->getDomainType() != DOMAIN_AIR) || !(pLoopUnit->hasMoved() && pLoopUnit->GetActivityType() == ACTIVITY_INTERCEPT))
						{
							// Test range
							if (plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= pLoopUnit->getUnitInfo().GetAirInterceptRange())
							{
								if (pLoopUnit->currInterceptionProbability() > 0)
								{
									if (bCountPercents)
										iRtnValue += pLoopUnit->currInterceptionProbability();
									else
										iRtnValue++;
								}
							}
						}
					}
				}
			}
		}
	}

	if (bCountPercents)
		iRtnValue /= 100;

	return iRtnValue;
}
#endif

/// Assess nearby enemy air assets
#if defined(MOD_AI_SMART_AIR_TACTICS)
// Add half of unit range to the calculations.
int CvMilitaryAI::GetNumEnemyAirUnitsInRange(CvPlot* pCenterPlot, int iRange, bool bCountFighters, bool bCountBombers) const
#else
int CvMilitaryAI::GetNumEnemyAirUnitsInRange(CvPlot* pCenterPlot, int /*iRange*/, bool bCountFighters, bool bCountBombers) const
#endif
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
				// Loop through their units looking for bombers (this will allow us to find bombers on carriers also
				int iLoopUnit = 0;
				for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoopUnit))
				{
					if (pLoopUnit->getDomainType() == DOMAIN_AIR)
					{
#if defined(MOD_AI_SMART_AIR_TACTICS)
						// Just to keep fighters closer to high range bombers (stealth bombers)
						int iAcceptableDistance = MOD_AI_SMART_AIR_TACTICS ? min(pLoopUnit->GetRange(), 12) + (iRange / 2) : 10;
						// distance was checked to a fixed 10 value
						if ( plotDistance(pCenterPlot->getX(), pCenterPlot->getY(), pLoopUnit->getX(), pLoopUnit->getY()) <= iAcceptableDistance )
#else
						if ( plotDistance(pCenterPlot->getX(), pCenterPlot->getY(), pLoopUnit->getX(), pLoopUnit->getY()) <= 10 )
#endif
						{
							// Let's not factor in revealed or visible - As a human I can remember past attacks and intuit whether a bomber could be in range of the city, AIs don't have great intuition...
							if (pLoopUnit->IsAirSweepCapable() || pLoopUnit->canAirDefend())
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

				// Loop through each of their cities
				//int iLoop;
				//CvCity* pLoopCity;
				//for(pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
				//{
				//	CvPlot *pCityPlot = pLoopCity->plot();
				//	if (pCityPlot->isRevealed(m_pPlayer->getTeam()) && plotDistance(pCenterPlot->getX(), pCenterPlot->getY(), pLoopCity->getX(), pLoopCity->getY()) <= iRange)
				//	{
				//		for (int iUnitLoop = 0; iUnitLoop < pCityPlot->getNumUnits(); iUnitLoop++)
				//		{
				//			CvUnit *pUnit = pCityPlot->getUnitByIndex(iUnitLoop);
				//			{
				//				if (pUnit->getDomainType() == DOMAIN_AIR)
				//				{
				//					if (pUnit->IsAirSweepCapable() || pUnit->canAirDefend())
				//					{
				//						if (bCountFighters) iRtnValue++;
				//					}
				//					else
				//					{
				//						if (bCountBombers) iRtnValue++;
				//					}
				//				}
				//			}
				//		}
				//	}
				//}
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
#if defined(MOD_BALANCE_CORE)
//Let's run a test to get minor civs to send assaults at nearby cities.
void CvMilitaryAI::MinorAttackTest()
{
	if(m_pPlayer->IsAtWarAnyMajor() && m_pPlayer->isMinorCiv())
	{
		CvMilitaryTarget target;
		int iOperationID;

		if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_SMALL_CITY_ATTACK, &iOperationID))
		{
			return;
		}
		if(m_pPlayer->getCapitalCity() == NULL)
		{
			return;
		}
		PlayerTypes eLoopPlayer;
		for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			eLoopPlayer = (PlayerTypes) iPlayerLoop;
			if(GET_PLAYER(eLoopPlayer).GetID() != m_pPlayer->GetID())
			{
				if(GET_PLAYER(eLoopPlayer).isAlive() && GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isAtWar(m_pPlayer->getTeam()))
				{
					if(GET_PLAYER(eLoopPlayer).GetProximityToPlayer(m_pPlayer->GetID()) >= PLAYER_PROXIMITY_CLOSE)
					{
						bool bHasOperationUnderway = false;
						bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_BOMBARDMENT, &iOperationID);
						bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID);
						if (!bHasOperationUnderway)
						{
							target = FindBestAttackTarget2(AI_OPERATION_SMALL_CITY_ATTACK, eLoopPlayer);
							if(target.m_pTargetCity && target.m_bAttackBySea && m_pPlayer->getCapitalCity()->isCoastal())
							{
								bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_BOMBARDMENT, &iOperationID);
								if (!bHasOperationUnderway)
								{
									int iNumRequiredSlots;
									int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_NAVAL_BOMBARDMENT, true, &iNumRequiredSlots);

									// Not willing to build units to get this off the ground
									if(iFilledSlots >= iNumRequiredSlots)
									{
										m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_BOMBARDMENT, eLoopPlayer);
										break;
									}
								}
							}
							else if(target.m_pTargetCity)
							{
								bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID);
								if (!bHasOperationUnderway)
								{
									int iNumRequiredSlots;
									int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_FAST_PILLAGERS, false, &iNumRequiredSlots);

									// Not willing to build units to get this off the ground
									if(iFilledSlots >= iNumRequiredSlots)
									{
										m_pPlayer->addAIOperation(AI_OPERATION_PILLAGE_ENEMY, eLoopPlayer);
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
}
#endif
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
#if defined(MOD_BALANCE_CORE)
	if(pPlayer->IsCramped())
	{
		iCurrentWeight += 5;
	}
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iCurrentWeight += pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
		if(GC.getFlavorTypes((FlavorTypes) iFlavorLoop) == "FLAVOR_OFFENSE")
		{
			iCurrentWeight += pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes) iFlavorLoop);
		}
	}
#endif

	CvDiplomacyAI* pkDiplomacyAI = pPlayer->GetDiplomacyAI();
	for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
	{
		eOtherPlayer = (PlayerTypes) iMajorLoop;
#if defined(MOD_BALANCE_CORE)
		if(pkDiplomacyAI->GetVictoryBlockLevel(eOtherPlayer) > BLOCK_LEVEL_WEAK)
		{
			iCurrentWeight += 5;
		}
		if(pkDiplomacyAI->GetVictoryDisputeLevel(eOtherPlayer) > DISPUTE_LEVEL_WEAK)
		{
			iCurrentWeight += 5;
		}
#endif

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
#if defined(MOD_BALANCE_CORE)
bool MilitaryAIHelpers::IsTestStrategy_AtWar(CvPlayer* pPlayer, bool bMinor)
#else
bool MilitaryAIHelpers::IsTestStrategy_AtWar(CvPlayer* pPlayer)
#endif
{
#if defined(MOD_BALANCE_CORE)
	if(pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith(bMinor) > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
#else
	return (pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith() > 0);
#endif
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

	// If we're at war don't bother with this Strategy (unless it is clear we are already winning)
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
#if defined(MOD_BALANCE_CORE_MILITARY)
		// If we have an operation of this type running, we don't want to turn this strategy off
		else if(pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_BOMBARDMENT))
		{
			return true;
		}
		//Let's hunt barbs if we want to expand
		EconomicAIStrategyTypes eStrategyExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
		if(pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyExpandToOtherContinents))
		{
			return true;
		}
#endif
	}

	return false;
}
#if defined(MOD_BALANCE_CORE)
/// "Eradicate Barbarians" Player Strategy: If there is a large group of barbarians units or camps near our civilization, increase OFFENSE
bool MilitaryAIHelpers::IsTestStrategy_EradicateBarbariansCritical(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer, int iBarbarianCampCount, int iVisibleBarbarianCount)
{
	int iStrategyWeight;

	CvMilitaryAIStrategyXMLEntry* pStrategy = pPlayer->GetMilitaryAI()->GetMilitaryAIStrategies()->GetEntry(eStrategy);
	CvAssert(pStrategy != NULL);
	if(pStrategy)
	{
		iStrategyWeight = iBarbarianCampCount * 50 + iVisibleBarbarianCount * 25;
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		if(iStrategyWeight >= iWeightThreshold * 4 && iVisibleBarbarianCount > 1) // barbs are a HUGE threat
		{
			return true;
		}
	}

	return false;
}
#endif
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
#if defined(MOD_BALANCE_CORE_MILITARY)
	if(pPlayer->GetMilitaryAI()->GetArmyBeingBuilt() == ARMY_TYPE_LAND)
	{
		return true;
	}
#endif
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
#if defined(MOD_CONFIG_AI_IN_XML)
		// The original code simplifies to 4*iNumAA > iNumMelee
		return (GD_INT_GET(AI_CONFIG_MILITARY_MELEE_PER_AA)*iNumAA > iNumMelee);
#else
		int iRatio = (iNumAA * 10) / max(1,iNumMelee+iNumAA);
		return (iRatio > 2);
#endif
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
#if defined(MOD_CONFIG_AI_IN_XML)
		// This original code simplifies to 4*iNumAA <= iNumMelee
		return (GD_INT_GET(AI_CONFIG_MILITARY_MELEE_PER_AA)*iNumAA <= iNumMelee);
#else
		int iRatio = (iNumAA * 10) / max(1,iNumMelee+iNumAA);
		return (iRatio <= 2);
#endif
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

#if defined(MOD_CONFIG_AI_IN_XML)
	// Why would we ever want to load EVERY plane onto a carrier?
	int iFactor = GC.getAI_CONFIG_MILITARY_AIRCRAFT_PER_CARRIER_SPACE();
	if (iNumLoadableAirUnits > iFactor*iNumTotalCargoSpace)
#else
	if (iNumLoadableAirUnits > iNumTotalCargoSpace)
#endif
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
#if defined(MOD_BALANCE_CORE_MILITARY)
		iNumUnitsWanted = /*3*/ GC.getBALANCE_NAVY_START_SIZE();
#else
		iNumUnitsWanted = 1;
#endif
	

	int iNumCoastalCities = 0;
	int iLoop;
	CvCity* pCity;
	for(pCity = pPlayer->firstCity(&iLoop); pCity != NULL; pCity = pPlayer->nextCity(&iLoop))
	{
#if defined(MOD_BALANCE_CORE_MILITARY)
		if(pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
#else
		if(pCity->isCoastal(-1))
#endif
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
#if defined(MOD_BALANCE_CORE_MILITARY)
	//Look at neighbors - if they're stronger than us, let's increase our amount.
	PlayerTypes eOtherPlayer;
	int iNumParity = 0;
	int iHighestParity = 0;
	int iNumOwnedNavalUnits = 0;
	int iNumTheirNavalUnits = 0;
	if(MOD_BALANCE_CORE_MILITARY && !pPlayer->isMinorCiv())
	{
		for(CvUnit* pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
		{
			if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->IsCombatUnit())
			{
				iNumOwnedNavalUnits++;
			}
		}
		for(int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			eOtherPlayer = (PlayerTypes) iMajorLoop;
			if(eOtherPlayer != NO_PLAYER && !GET_PLAYER(eOtherPlayer).isMinorCiv() && (eOtherPlayer != pPlayer->GetID()))
			{
				MajorCivApproachTypes eApproachType = pPlayer->GetDiplomacyAI()->GetMajorCivApproach(eOtherPlayer, false);
				if((eApproachType != MAJOR_CIV_APPROACH_FRIENDLY) && (GET_PLAYER(eOtherPlayer).GetProximityToPlayer(pPlayer->GetID()) >= PLAYER_PROXIMITY_CLOSE))
				{
					for(CvUnit* pLoopUnit = GET_PLAYER(eOtherPlayer).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(eOtherPlayer).nextUnit(&iLoop))
					{
						if (pLoopUnit != NULL && pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->IsCombatUnit())
						{
							iNumTheirNavalUnits++;
						}
					}
					//Let's try to achieve military parity with 1/3 the largest number of enemy units.
					iNumParity = (iNumTheirNavalUnits - iNumOwnedNavalUnits);
					if(iNumParity > iHighestParity)
					{
						iHighestParity = iNumParity;
					}
				}
			}
		}
	}
	if(iHighestParity > 0)
	{
		if(iHighestParity > iNumUnitsWanted)
		{
			return iHighestParity;
		}
		return iNumUnitsWanted;
	}
#endif

	return iNumUnitsWanted;
}
#if defined(MOD_BALANCE_CORE)
int MilitaryAIHelpers::GetBestFormationType()
{
	int iTurnMin = 100;
	iTurnMin *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iTurnMin /= 100;

	//Granular, for later variation (if needed)
	if(GC.getGame().getGameTurn() <= iTurnMin)
	{
		return (int)MUFORMATION_EARLY_RUSH;
	}
	else if(GC.getGame().getGameTurn() <= iTurnMin * 2)
	{
		return (int)MUFORMATION_SMALL_CITY_ATTACK_FORCE;
	}
	else if(GC.getGame().getGameTurn() <= (iTurnMin * 3))
	{
		return (int)MUFORMATION_BASIC_CITY_ATTACK_FORCE;
	}
	else if(GC.getGame().getGameTurn() > (iTurnMin * 3))
	{
		return (int)MUFORMATION_BIGGER_CITY_ATTACK_FORCE;
	}
	return (int)MUFORMATION_BASIC_CITY_ATTACK_FORCE;
}
#endif
/// How many slots in this army can we fill right now with available units?
int MilitaryAIHelpers::NumberOfFillableSlots(CvPlayer* pPlayer, MultiunitFormationTypes formation, bool bRequiresNavalMoves, int* piNumberSlotsRequired, int* piNumberLandReservesUsed)
{
	CvUnit* pLoopUnit;
	int iLoop;
#if defined(MOD_BALANCE_CORE)
	FStaticVector< CvFormationSlotEntry, 30, false, c_eCiv5GameplayDLL > slotsToFill;
	FStaticVector< CvFormationSlotEntry, 30, false, c_eCiv5GameplayDLL >::iterator it;
#else
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL > slotsToFill;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL >::iterator it;
#endif
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
#if defined(MOD_BALANCE_CORE)
	FStaticVector< CvFormationSlotEntry, 30, false, c_eCiv5GameplayDLL > slotsToFill;
	FStaticVector< CvFormationSlotEntry, 30, false, c_eCiv5GameplayDLL >::iterator it;
#else
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL > slotsToFill;
	FStaticVector< CvFormationSlotEntry, 10, false, c_eCiv5GameplayDLL >::iterator it;
#endif

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