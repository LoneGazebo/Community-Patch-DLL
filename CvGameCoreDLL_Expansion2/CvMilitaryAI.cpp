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
#include "cvStopWatch.h"

// must be included after all other headers
#include "LintFree.h"

// set this to 1 in debugger if needed
int gDebugOutput = 0;

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
	m_iNumberOfTimesSettlerBuildSkippedOver(0),
	m_eArmyTypeBeingBuilt(NO_ARMY_TYPE),
	m_aiWarFocus(NULL)
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

	CvAssertMsg(m_aiWarFocus == NULL, "about to leak memory, CvMilitaryAI::m_aiWarFocus");
	m_aiWarFocus = FNEW(int[MAX_MAJOR_CIVS], c_eCiv5GameplayDLL, 0);

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
	SAFE_DELETE_ARRAY(m_aiWarFocus);
}

/// Reset AIStrategy status array to all false
void CvMilitaryAI::Reset()
{
	int iI;

	m_iTotalThreatWeight = 1;  // Don't ever assume there is no threat at all
	m_eArmyTypeBeingBuilt = NO_ARMY_TYPE;
	m_cachedTargets.clear();
	m_cachedLandDistances.clear();
	m_cachedWaterDistances.clear();

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
	m_iNumberOfTimesSettlerBuildSkippedOver = 0;
#if defined(MOD_BALANCE_CORE)
	for (iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		m_aiWarFocus[iI] = WARTYPE_UNDEFINED;
	m_iRecNavySize = 0;
	m_iFreeCarrier = 0;
	m_iFreeCargo = 0;
#endif

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
	kStream >> m_iNumberOfTimesSettlerBuildSkippedOver;

#if defined(MOD_BALANCE_CORE_MILITARY)
	kStream >> m_iNumAntiAirUnits;
	kStream >> m_iNumLandUnits;
	kStream >> m_iNumNavalUnits;
	kStream >> m_iRecommendedMilitarySize;
	kStream >> m_iRecNavySize;
	kStream >> m_iFreeCarrier;
	kStream >> m_iFreeCargo;
	//first get how many entries we have
	m_cachedTargets.clear();
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
		kStream >> target.bOcean;
		kStream >> target.bNoLandPath;
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

	ArrayWrapper<int> wrapm_aiWarFocus(MAX_MAJOR_CIVS, m_aiWarFocus);
	kStream >> wrapm_aiWarFocus;
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
	kStream << m_iNumberOfTimesSettlerBuildSkippedOver;

#if defined(MOD_BALANCE_CORE_MILITARY)
	kStream << m_iNumAntiAirUnits;
	kStream << m_iNumLandUnits;
	kStream << m_iNumNavalUnits;
	kStream << m_iRecommendedMilitarySize;
	kStream << m_iRecNavySize;
	kStream << m_iFreeCarrier;
	kStream << m_iFreeCargo;

	//first pass: count how many relevant cache entries we have
	int iCount = 0;
	for (CachedTargetsMap::iterator itE = m_cachedTargets.begin(); itE!=m_cachedTargets.end(); ++itE)
		iCount += itE->second.size();
	kStream << iCount;

	//second pass: write them
	for (CachedTargetsMap::iterator itE = m_cachedTargets.begin(); itE!=m_cachedTargets.end(); ++itE)
	{
		for (CachedTargetsMap::value_type::second_type::iterator itOp = itE->second.begin(); itOp!=itE->second.end(); ++itOp)
		{
			kStream << itE->first;
			kStream << itOp->first;
			kStream << itOp->second.iTargetCity;
			kStream << itOp->second.iMusterCity;
			kStream << itOp->second.bAttackBySea;
			kStream << itOp->second.bOcean;
			kStream << itOp->second.bNoLandPath;
			kStream << itOp->second.iScore;
			kStream << itOp->second.iTurnChosen;
		}
	}	
#endif

	kStream << GC.getNumMilitaryAIStrategyInfos();
	kStream << ArrayWrapper<bool>(m_pAIStrategies->GetNumMilitaryAIStrategies(), m_pabUsingStrategy);
	kStream << ArrayWrapper<int>(m_pAIStrategies->GetNumMilitaryAIStrategies(), m_paiTurnStrategyAdopted);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnWarState);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryThreat);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnMilitaryStrength);
	kStream << ArrayWrapper<int>(MAX_CIV_PLAYERS, m_paeLastTurnTargetValue);
	kStream << ArrayWrapper<int>(MAX_MAJOR_CIVS, m_aiWarFocus);
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
	UpdateWarType();

	if(!m_pPlayer->isHuman())
	{
		UpdateOperations();
		MakeEmergencyPurchases();
		RequestImprovements();
		DisbandObsoleteUnits();
	}

	if(!m_pPlayer->isMinorCiv())
	{
		LogMilitaryStatus();

		if(GetArmyBeingBuilt() != NO_ARMY_TYPE)
		{
			LogAvailableForces();
		}
	}
}

/// Requests for sneak attack on a city of a player we're not at war with. Returns true if operation started.
bool CvMilitaryAI::RequestSneakAttack(PlayerTypes eEnemy)
{
	CvAIOperation* pOperation = 0;
	int iOperationID;
	int iNumUnitsWillingBuild = 1;
	// Let's only allow us to be sneak attacking one opponent at a time, so abort if already have one of these operations active against any opponent
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_INVASION_SNEAKY, &iOperationID))
	{
		return false;
	}
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_SNEAK_ATTACK, &iOperationID))
	{
		return false;
	}
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, &iOperationID))
	{
		return false;
	}
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_INVASION_SNEAKY, &iOperationID))
	{
		return false;
	}
	if(m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
	{
		return false;
	}
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer == NO_PLAYER || eLoopPlayer == m_pPlayer->GetID())
		{
			continue;
		}
		if(m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer) == WAR_STATE_DEFENSIVE || m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer) == WAR_STATE_NEARLY_DEFEATED) 
		{
			return false;
		}
	}
	CvMilitaryTarget target = FindBestAttackTargetCached(AI_OPERATION_CITY_SNEAK_ATTACK, eEnemy);
	if (!target.m_pTargetCity || !target.m_pMusterCity)
	{
		target = FindBestAttackTargetCached(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, eEnemy);
	}
	if (!target.m_pTargetCity || !target.m_pMusterCity)
	{
		target = FindBestAttackTargetCached(AI_OPERATION_NAVAL_INVASION_SNEAKY, eEnemy);
	}
	if(target.m_pTargetCity && target.m_pMusterCity)
	{
		if(target.m_bNoLandPath || target.m_bAttackBySea)
		{
			int iNumRequiredSlots;
			int iLandReservesUsed;

			int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
			if (iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity, target.m_bOcean);
				if (pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForNavalAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
				if (iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_INVASION_SNEAKY, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity, target.m_bOcean);
					if (pOperation != NULL && !pOperation->ShouldAbort())
					{
						return true;
					}
				}
			}
			m_iNumNavalAttacksRequested++;
			m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
		}
		else
		{
			int iNumRequiredSlots;
			int iLandReservesUsed;
			int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
			if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_SNEAK_ATTACK, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);

				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			m_iNumLandAttacksRequested++;
			m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
		}
	}
	return false;
}

/// Send an army to force concessions
bool CvMilitaryAI::RequestShowOfForce(PlayerTypes eEnemy)
{
	// Let's only allow us to be sneak attacking one opponent at a time, so abort if already have one of these operations active against any opponent
	if (GetShowOfForceOperation(eEnemy) != NULL)
		return false;

	if (m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
		return false;

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer == NO_PLAYER || eLoopPlayer == m_pPlayer->GetID())
		{
			continue;
		}
		if (m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer) == WAR_STATE_DEFENSIVE || m_pPlayer->GetDiplomacyAI()->GetWarState(eLoopPlayer) == WAR_STATE_NEARLY_DEFEATED)
		{
			return false;
		}
	}

	CvAIOperation* pOperation = 0;
	int iNumUnitsWillingBuild = 1;

	CvMilitaryTarget target = FindBestAttackTargetCached(AI_OPERATION_CITY_SNEAK_ATTACK, eEnemy);
	if (!target.m_pTargetCity || !target.m_pMusterCity)
	{
		target = FindBestAttackTargetCached(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, eEnemy);
	}
	if (!target.m_pTargetCity || !target.m_pMusterCity)
	{
		target = FindBestAttackTargetCached(AI_OPERATION_NAVAL_INVASION_SNEAKY, eEnemy);
	}
	if (target.m_pTargetCity && target.m_pMusterCity)
	{
		if (target.m_bNoLandPath || target.m_bAttackBySea)
		{
			int iNumRequiredSlots;
			int iLandReservesUsed;

			int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
			if (iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity, target.m_bOcean);
				if (pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForNavalAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
				if (iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_INVASION_SNEAKY, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity, target.m_bOcean);
					if (pOperation != NULL && !pOperation->ShouldAbort())
					{
						return true;
					}
				}
			}
			m_iNumNavalAttacksRequested++;
			m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
		}
		else
		{
			int iNumRequiredSlots;
			int iLandReservesUsed;
			int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
			if (iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_SNEAK_ATTACK, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);

				if (pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			m_iNumLandAttacksRequested++;
			m_eArmyTypeBeingBuilt = (m_iNumLandAttacksRequested > m_iNumNavalAttacksRequested) ? ARMY_TYPE_LAND : ARMY_TYPE_NAVAL_INVASION;
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
	int iNumUnitsWillingBuild = 0;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID);
	if (!bHasOperationUnderway && GET_PLAYER(eEnemy).getCapitalCity() != NULL)
	{
		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MUFORMATION_FAST_PILLAGERS, false, false, GET_PLAYER(eEnemy).getCapitalCity()->plot(), GET_PLAYER(eEnemy).getCapitalCity()->plot(), &iNumRequiredSlots, &iLandReservesUsed);
#else
	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, MUFORMATION_FAST_PILLAGERS, false, &iNumRequiredSlots, &iLandReservesUsed);
#endif
	if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
	{
		CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_PILLAGE_ENEMY, eEnemy);
		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			m_iNumLandUnitsInArmies += iFilledSlots;
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
	CvMilitaryTarget target = FindBestAttackTargetCached(AI_OPERATION_CITY_BASIC_ATTACK, eEnemy);

	return RequestSpecificAttack(target, iNumUnitsWillingBuild);
}

/// Send a navy to take a city
bool CvMilitaryAI::RequestPureNavalAttack(PlayerTypes eEnemy, int iNumUnitsWillingBuild)
{
	CvAIOperation* pOperation = NULL;
	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;

	CvMilitaryTarget target;
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, &iOperationID);
	if (!bHasOperationUnderway)
	{
		target = FindBestAttackTargetCached(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, eEnemy);

		if(target.m_pTargetCity && target.m_pMusterCity)
		{
			if(target.m_bAttackBySea)
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
				if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity, target.m_bOcean);
				}

				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// Request for an attack on a city state
bool CvMilitaryAI::RequestCityStateAttack(PlayerTypes eEnemy)
{
	CvAIOperation* pOperation = 0;
	int iOperationID;
	int iNumUnitsWillingBuild = 2;
	// Let's only allow us to be sneak attacking one opponent at a time, so abort if already have one of these operations active against any opponent
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_INVASION_SNEAKY, &iOperationID))
	{
		return false;
	}
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_SNEAK_ATTACK, &iOperationID))
	{
		return false;
	}
	if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, &iOperationID))
	{
		return false;
	}
	CvMilitaryTarget target = FindBestAttackTargetCached(AI_OPERATION_CITY_SNEAK_ATTACK, eEnemy);

	if(target.m_pTargetCity && target.m_pMusterCity)
	{
		if(target.m_bNoLandPath)
		{
			int iNumRequiredSlots;
			int iLandReservesUsed;
			int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForNavalAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
			if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_INVASION_SNEAKY, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity, target.m_bOcean);
				if(pOperation != NULL && !pOperation->ShouldAbort())
				{
					return true;
				}
			}
			else
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
				if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity, target.m_bOcean);
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
		}
		else
		{
			int iNumRequiredSlots;
			int iLandReservesUsed;
			int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack(), target.m_bAttackBySea, target.m_bOcean, target.m_pMusterCity->plot(), target.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
			if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
			{
				pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_SNEAK_ATTACK, eEnemy, target.m_pMusterCity->getArea(), target.m_pTargetCity, target.m_pMusterCity);
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

bool CvMilitaryAI::RequestSpecificAttack(CvMilitaryTarget kTarget, int iNumUnitsWillingToBuild)
{
	CvAIOperation* pOperation = NULL;
	int iNumRequiredSlots = 0;
	int iLandReservesUsed = 0;
	int iFilledSlots = 0;
	if(kTarget.m_pTargetCity && kTarget.m_pMusterCity)
	{
		//sanity check
		if (!m_pPlayer->IsAtWarWith(kTarget.m_pTargetCity->getOwner()) && !GET_TEAM(m_pPlayer->getTeam()).canDeclareWar(GET_PLAYER(kTarget.m_pTargetCity->getOwner()).getTeam()))
			OutputDebugString("warning: invalid attack target!\n");

		if(kTarget.m_bNoLandPath)
		{
			if(!GET_PLAYER(kTarget.m_pTargetCity->getOwner()).isMinorCiv())
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, kTarget.m_pTargetCity->getOwner(), MilitaryAIHelpers::GetCurrentBestFormationTypeForNavalAttack(), kTarget.m_bAttackBySea, kTarget.m_bOcean, kTarget.m_pMusterCity->plot(), kTarget.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
				if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild))
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_INVASION, kTarget.m_pTargetCity->getOwner(), kTarget.m_pMusterCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity, kTarget.m_bOcean);
				}
			}
			else if(GET_PLAYER(kTarget.m_pTargetCity->getOwner()).isMinorCiv())
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, kTarget.m_pTargetCity->getOwner(), MUFORMATION_CITY_STATE_INVASION, kTarget.m_bAttackBySea, kTarget.m_bOcean, kTarget.m_pMusterCity->plot(), kTarget.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
				if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild))
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_INVASION_CITY_STATE, kTarget.m_pTargetCity->getOwner(), kTarget.m_pMusterCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity, kTarget.m_bOcean);
				}
			}
		}
		else
		{
			if(!GET_PLAYER(kTarget.m_pTargetCity->getOwner()).isMinorCiv())
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, kTarget.m_pTargetCity->getOwner(), MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack(), kTarget.m_bAttackBySea, kTarget.m_bOcean, kTarget.m_pMusterCity->plot(), kTarget.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
				if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild))
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_BASIC_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pMusterCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
				}
			}
			else if(GET_PLAYER(kTarget.m_pTargetCity->getOwner()).isMinorCiv())
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, kTarget.m_pTargetCity->getOwner(), MUFORMATION_SMALL_CITY_ATTACK_FORCE, kTarget.m_bAttackBySea, kTarget.m_bOcean, kTarget.m_pMusterCity->plot(), kTarget.m_pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
				if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingToBuild))
				{
					pOperation = m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_ATTACK, kTarget.m_pTargetCity->getOwner(), kTarget.m_pMusterCity->getArea(), kTarget.m_pTargetCity, kTarget.m_pMusterCity);
				}
			}
		}
		if(pOperation != NULL && !pOperation->ShouldAbort())
		{
			return true;
		}
	}

	return false;
}

bool CvMilitaryAI::RequestBullyingOperation(PlayerTypes eEnemy)
{
	if (!GET_PLAYER(eEnemy).isMinorCiv())
		return false;

	CvCity* pTargetCity = GET_PLAYER(eEnemy).getCapitalCity();
	if (!pTargetCity)
		return false;

	CvCity* pMusterCity = m_pPlayer->GetClosestCityByEstimatedTurns(pTargetCity->plot());
	if (!pMusterCity)
		return false;

	//do not set a player - that way we can traverse unrevealed plots and foreign territory
	SPathFinderUserData data(NO_PLAYER, PT_GENERIC_REACHABLE_PLOTS, -1, MINOR_POWER_COMPARISON_RADIUS);
	ReachablePlots relevantPlots = GC.GetStepFinder().GetPlotsInReach(pTargetCity->plot(), data);

	//taken from CalculateBullyScore
	pair<int, int> localPower = TacticalAIHelpers::EstimateLocalUnitPower(relevantPlots, GET_PLAYER(eEnemy).getTeam(), m_pPlayer->getTeam(), false);
	int iLocalPowerRatio = int((localPower.second * 100.f) / (localPower.first + pTargetCity->GetPower()));

	//check if we have a chance ...
	int iCurrentBullyMetric = GET_PLAYER(eEnemy).GetMinorCivAI()->CalculateBullyScore(m_pPlayer->GetID(), true);
	if (iLocalPowerRatio > 100 || iCurrentBullyMetric < -200)
		return false;

	int iDistanceTurns = m_pPlayer->GetCityDistanceInEstimatedTurns(pTargetCity->plot());
	if (pMusterCity->getArea() == pTargetCity->getArea() || iDistanceTurns <= 4) //if the target is very close assume we can embark or don't even need to
	{
		if (iDistanceTurns > 12)
			return false;

		// Let's only allow us to be bullying one opponent at a time, so abort if already have one of these operations active against any opponent
		if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_BULLY_CITY_STATE))
			return false;

		//don't try to build additional units, only do this if we have enough at hand
		int iNumRequiredSlots, iLandReservesUsed;
		int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MUFORMATION_CITY_STATE_ATTACK_FORCE, false, false, pMusterCity->plot(), pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
		if (iFilledSlots >= iNumRequiredSlots)
		{
			CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_BULLY_CITY_STATE, eEnemy, -1, pTargetCity, pMusterCity);
			if (pOperation != NULL && !pOperation->ShouldAbort())
			{
				return true;
			}
		}
	}
	else
	{
		// Let's only allow us to be bullying one opponent at a time, so abort if already have one of these operations active against any opponent
		if (m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_BULLY_CITY_STATE))
			return false;

		//don't try to build additional units, only do this if we have enough at hand
		int iNumRequiredSlots, iLandReservesUsed;
		int iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MUFORMATION_NAVAL_SQUADRON, false, m_pPlayer->CanCrossOcean(), pMusterCity->plot(), pTargetCity->plot(), &iNumRequiredSlots, &iLandReservesUsed);
		if (iFilledSlots >= iNumRequiredSlots)
		{
			CvAIOperation* pOperation = m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_BULLY_CITY_STATE, eEnemy, -1, pTargetCity, pMusterCity, m_pPlayer->CanCrossOcean());
			if (pOperation != NULL && !pOperation->ShouldAbort())
			{
				return true;
			}
		}
	}

	return false;
}

/// Get a pointer to the sneak attack operation against a target
CvAIOperation* CvMilitaryAI::GetSneakAttackOperation(PlayerTypes eEnemy)
{
	CvAIOperation* pOp = m_pPlayer->getFirstAIOperation();
	while (pOp)
	{
		if (eEnemy == NO_PLAYER || pOp->GetEnemy() == eEnemy)
		{
			if (pOp->IsOffensive() && pOp->IsAllowedDuringPeace() && !pOp->IsShowOfForce())
				return pOp;
		}

		pOp = m_pPlayer->getNextAIOperation();
	}

	return NULL;
}

/// Get a pointer to the show of force operation against a target
CvAIOperation* CvMilitaryAI::GetShowOfForceOperation(PlayerTypes eEnemy)
{
	CvAIOperation* pOp = m_pPlayer->getFirstAIOperation();
	while (pOp)
	{
		if (eEnemy == NO_PLAYER || pOp->GetEnemy() == eEnemy)
		{
			if (pOp->IsShowOfForce())
				return pOp;
		}

		pOp = m_pPlayer->getNextAIOperation();
	}

	return NULL;
}

/// Get a pointer to the basic attack against a target
CvAIOperation* CvMilitaryAI::GetBasicAttackOperation(PlayerTypes eEnemy)
{
	int iOperationID;
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_BASIC_ATTACK, &iOperationID, eEnemy);

	if(bHasOperationUnderway)
	{
		return m_pPlayer->getAIOperation(iOperationID);
	}
	else
	{
		bool bHasOperationOfType = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_INVASION, &iOperationID, eEnemy);

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
		bool bHasOperationOfType = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_INVASION_CITY_STATE, &iOperationID, eEnemy);

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
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, &iOperationID, eEnemy);

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
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitType, true);
#else
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitType);
#endif
	if(eType != NO_UNIT)
	{
#if defined(MOD_BALANCE_CORE)
		CvUnitEntry* pUnitInfo = GC.GetGameUnits()->GetEntry(eType);
		if(pUnitInfo && pUnitInfo->GetUnitAIType(eUnitType))
		{
#endif
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
					int iResult = pCity->CreateUnit(eType, NO_UNITAI, REASON_BUY, false /*bUseToSatisfyOperation*/);

					CvAssertMsg(iResult != -1, "Unable to create unit");

					if (iResult != -1)
					{
						CvUnit* pUnit = m_pPlayer->getUnit(iResult);
						m_pPlayer->GetTreasury()->LogExpenditure((CvString)pUnit->getUnitInfo().GetText(), iGoldCost, 7);
						m_pPlayer->GetTreasury()->ChangeGold(-iGoldCost);

#if defined(MOD_BUGFIX_MOVE_AFTER_PURCHASE)
						if (!pUnit->getUnitInfo().CanMoveAfterPurchase())
						{
#endif
							pUnit->finishMoves();
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
				int iResult = pCity->CreateUnit(eType, NO_UNITAI, REASON_BUY, false /*bUseToSatisfyOperation*/);

				CvAssertMsg(iResult != -1, "Unable to create unit");
				CvUnit* pUnit = m_pPlayer->getUnit(iResult);
#if defined(MOD_BUGFIX_MOVE_AFTER_PURCHASE)
				if (!pUnit->getUnitInfo().CanMoveAfterPurchase())
				{
#endif
					pUnit->finishMoves();
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
#if defined(MOD_BALANCE_CORE)
	}
#endif
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
						CvAssertMsg(iResult != -1, "Unable to create building");

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
#if defined(MOD_BALANCE_CORE_MILITARY)

CvCity* GetCityFromGlobalID(int iID)
{
	//the muster city for a given target can belong to any player, no only to ourselves
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{	
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive())
		{
			CvCity* pCity = GET_PLAYER(eLoopPlayer).getCity(iID);
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

//-----------------------------------------
// basic intra-turn caching. not serialized.
//-----------------------------------------
int CvMilitaryAI::GetCachedAttackTargetWaterDistance(CvCity* pCity, CvCity* pOtherCity, int iMaxTurnsAway)
{
	if (!pCity->hasSharedAdjacentArea(pOtherCity))
		return -1;

	PlayerTypes eIntendedEnemy = pOtherCity->getOwner();
	CachedDistancesMap::iterator itO = m_cachedWaterDistances.find(pCity);
	if (itO != m_cachedWaterDistances.end())
	{
		CachedDistancesMap::value_type::second_type::iterator itD = itO->second.find(pOtherCity);
		if (itD != itO->second.end())
			return itD->second;
	}

	int iDistance = -1;
	SPathFinderUserData data(m_pPlayer->GetID(), PT_GENERIC_ANY_AREA, pOtherCity->getOwner(), iMaxTurnsAway);
	data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;

	//performance optimization
	if (plotDistance(*pCity->plot(), *pOtherCity->plot()) < iMaxTurnsAway * 2)
	{
		if (m_pPlayer->CanCrossOcean())
		{
			SPath path = GC.GetStepFinder().GetPath(pCity->plot(), pOtherCity->plot(), data);
			if (!!path && PathIsGood(path, eIntendedEnemy))
				iDistance = path.iTotalTurns;
		}
		else if (GET_TEAM(m_pPlayer->getTeam()).canEmbark())
		{
			//try without ocean
			data.iFlags |= CvUnit::MOVEFLAG_NO_OCEAN;
			SPath path = GC.GetStepFinder().GetPath(pCity->plot(), pOtherCity->plot(), data);
			if (!!path && PathIsGood(path, eIntendedEnemy))
				iDistance = path.iTotalTurns;
		}
	}

	//update the cache
	m_cachedWaterDistances[pCity][pOtherCity] = iDistance;

	/*
	if (GC.getLogging() && GC.getAILogging() && iDistance!=-1)
	{
		CvString strOutBuf = CvString::format("%03d, %s, updated WATER distance map between cities, %s, Muster: %s, Distance: %d",
			GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription(), pOtherCity->getName().c_str(),
			pCity->getName().c_str(), iDistance);
		CvString playerName = GetPlayer()->getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);
		if (pLog)
			pLog->Msg(strOutBuf);
	}
	*/

	return iDistance;
}

//-----------------------------------------
// basic intra-turn caching. not serialized.
//-----------------------------------------
int CvMilitaryAI::GetCachedAttackTargetLandDistance(CvCity* pCity, CvCity* pOtherCity, int iMaxTurnsAway)
{
	if (pCity->getArea() != pOtherCity->getArea())
		return -1;

	PlayerTypes eIntendedEnemy = pOtherCity->getOwner();
	CachedDistancesMap::iterator itO = m_cachedLandDistances.find(pCity);
	if (itO != m_cachedLandDistances.end())
	{
		CachedDistancesMap::value_type::second_type::iterator itD = itO->second.find(pOtherCity);
		if (itD != itO->second.end())
			return itD->second;
	}

	SPathFinderUserData data(m_pPlayer->GetID(), PT_GENERIC_SAME_AREA, pOtherCity->getOwner(), iMaxTurnsAway);
	data.iFlags |= CvUnit::MOVEFLAG_NO_EMBARK;
	data.iFlags |= CvUnit::MOVEFLAG_APPROX_TARGET_RING1;

	int iDistance = -1;
	//performance optimization
	if (plotDistance(*pCity->plot(), *pOtherCity->plot()) < iMaxTurnsAway * 2)
	{
		SPath path = GC.GetStepFinder().GetPath(pCity->plot(), pOtherCity->plot(), data);
		if (!!path && PathIsGood(path, eIntendedEnemy))
			iDistance = path.iTotalTurns;
	}

	//update the cache
	m_cachedLandDistances[pCity][pOtherCity] = iDistance;

	/*
	if (GC.getLogging() && GC.getAILogging() && iDistance!=-1)
	{
		CvString strOutBuf = CvString::format("%03d, %s, updated LAND distance map between cities, %s, Muster: %s, Distance: %d",
			GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription(), pOtherCity->getName().c_str(),
			pCity->getName().c_str(), iDistance);
		CvString playerName = GetPlayer()->getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);
		if (pLog)
			pLog->Msg(strOutBuf);
	}
	*/

	return iDistance;
}

void CvMilitaryAI::ResetDistanceCaches()
{
	m_cachedLandDistances.clear();
	m_cachedWaterDistances.clear();
}

bool CvMilitaryAI::PathIsGood(const SPath & path, PlayerTypes eIntendedEnemy)
{
	//define short paths as safe (start and dest plot are included in count)
	if (path.vPlots.size() < 4)
		return true;

	//ignore beginning and end of path!
	for (size_t i = 3; i < path.vPlots.size() - 3; i++)
	{
		CvPlot* pPlot = GC.getMap().plot(path.vPlots[i].x,path.vPlots[i].y);
		int iCityDistance = GC.getGame().GetClosestCityDistanceInPlots(pPlot);

		//if we're far from a city it's ok
		if (iCityDistance > 3 && !pPlot->isOwned())
			continue;

		//else check whose city it is
		CvCity* pCity = GC.getGame().GetClosestCityByPlots(pPlot);
		if (!pCity)
			continue;

		//passing through a warzone? not good
		if (pCity->getOwner() != eIntendedEnemy && m_pPlayer->IsAtWarWith(pCity->getOwner()))
			return false;

		//passing another city of our enemy? then we should attack that one first
		if (pCity->getOwner()==eIntendedEnemy)
			return false;
	}

	return true;
}

bool CvMilitaryAI::IsCurrentAttackTarget(CvCity* pCity)
{
	if (!pCity || pCity->getOwner()==m_pPlayer->GetID())
		return false;

	CachedTargetsMap::iterator itEnemy = m_cachedTargets.find( pCity->getOwner() );
	if (itEnemy != m_cachedTargets.end())
	{
		for (std::map<AIOperationTypes,SCachedTarget>::iterator itTarget = itEnemy->second.begin(); itTarget != itEnemy->second.end(); ++itTarget)
		{
			if (itTarget->second.iTargetCity == pCity->GetID())
			{
				return true;
			}
		}
	}

	return false;
}

bool CvMilitaryAI::HaveValidAttackTarget(PlayerTypes eEnemy)
{
	CachedTargetsMap::iterator itE = m_cachedTargets.find(eEnemy);
	if (itE != m_cachedTargets.end())
	{
		for (CachedTargetsMap::value_type::second_type::iterator itOp = itE->second.begin(); itOp != itE->second.end(); ++itOp)
		{
			const SCachedTarget& cachedTarget = itOp->second;
			CvCity* pCachedTargetCity = GetCityFromGlobalID(cachedTarget.iTargetCity);
			CvCity* pCachedMusterCity = GetCityFromGlobalID(cachedTarget.iMusterCity);
			if (pCachedTargetCity != NULL && pCachedMusterCity != NULL)
			{
				return true;
			}
		}
	}
	return false;
}

CvMilitaryTarget CvMilitaryAI::FindBestAttackTargetCached(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, int* piWinningScore)
{
	int ciAgeLimit = 5; //don't switch targets too often but update our cached targets from time and time

	if (eEnemy >= MAX_PLAYERS) //barb cities are valid targets!
	{
		if (piWinningScore)
			*piWinningScore = 0;
		return CvMilitaryTarget();
	}

	//todo: compare score across op types to see if this makes sense at all
	CachedTargetsMap::iterator itE = m_cachedTargets.find(eEnemy);
	if (itE != m_cachedTargets.end())
	{
		CachedTargetsMap::value_type::second_type::iterator itOp = itE->second.find(eAIOperationType);
		if (itOp != itE->second.end())
		{
			// important - this must be a reference!
			SCachedTarget& cachedTarget = itOp->second;

			CvCity* pCachedTargetCity = GetCityFromGlobalID(cachedTarget.iTargetCity);
			CvCity* pCachedMusterCity = GetCityFromGlobalID(cachedTarget.iMusterCity);

			//check if we can still use it
			bool bInvalidTarget = (pCachedTargetCity == NULL || pCachedMusterCity == NULL || pCachedTargetCity->getOwner() != eEnemy || pCachedMusterCity->getOwner() != m_pPlayer->GetID());
			bool bWantNewTarget = (GC.getGame().getGameTurn() - cachedTarget.iTurnChosen >= ciAgeLimit);

			//only search for a new target if we didn't already do so this turn
			CvMilitaryTarget newTarget;
			if (bInvalidTarget || bWantNewTarget)
			{
			int iNewScore = 0;
				newTarget = FindBestAttackTarget(eAIOperationType, eEnemy, &iNewScore);

				//new target valid?
				if (newTarget.m_pTargetCity && newTarget.m_pMusterCity)
				{
					cachedTarget.iTargetCity = newTarget.m_pTargetCity->GetID();
					cachedTarget.iMusterCity = newTarget.m_pMusterCity->GetID();
					cachedTarget.bAttackBySea = newTarget.m_bAttackBySea;
					cachedTarget.bOcean = newTarget.m_bOcean;
					cachedTarget.bNoLandPath = newTarget.m_bNoLandPath;
					cachedTarget.iScore = iNewScore;
					cachedTarget.iTurnChosen = GC.getGame().getGameTurn();

					//important, update the cache
					m_cachedTargets[eEnemy][eAIOperationType] = cachedTarget;

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strOutBuf = CvString::format("%03d, %s, refreshed our attack target, %s, Muster: %s, optype: %d, score: %d",
							GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription(), newTarget.m_pTargetCity->getName().c_str(),
							newTarget.m_pMusterCity ? newTarget.m_pMusterCity->getName().c_str() : "NONE", eAIOperationType, iNewScore);
						CvString playerName = GetPlayer()->getCivilizationShortDescription();
						FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);
						if (pLog)
							pLog->Msg(strOutBuf);
					}

					if (piWinningScore)
						*piWinningScore = iNewScore;

					return newTarget;
				}
				//no new valid target?
				else
				{
					//don't keep the old invalid target around
					itE->second.erase(itOp);

					if (piWinningScore)
						*piWinningScore = 0;

					return CvMilitaryTarget();
				}
			}
			else //old one still valid and couldn't find a better target
			{
				newTarget.m_pTargetCity = pCachedTargetCity;
				newTarget.m_pMusterCity = pCachedMusterCity;
				newTarget.m_bAttackBySea = cachedTarget.bAttackBySea;
				newTarget.m_bOcean = cachedTarget.bOcean;
				newTarget.m_bNoLandPath = cachedTarget.bNoLandPath;

				/*
				if(GC.getLogging() && GC.getAILogging() && pCachedTargetCity)
				{
					CvString strOutBuf = CvString::format("%03d, %s, keeping cached attack target, %s, muster: %s, optype: %d",
						GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription(), pCachedTargetCity->getName().c_str(), 
						pCachedMusterCity ? pCachedMusterCity->getName().c_str() : "NONE", eAIOperationType);
					CvString playerName = GetPlayer()->getCivilizationShortDescription();
					FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);
					if (pLog)
						pLog->Msg(strOutBuf);
				}
				*/

				if (piWinningScore)
					*piWinningScore = cachedTarget.iScore;

				return newTarget;
			}
		}
	}

	//no cache hit, start from scratch
	int iNewScore = 0;
	CvMilitaryTarget newTarget = FindBestAttackTarget(eAIOperationType, eEnemy, &iNewScore);
	if (newTarget.m_pTargetCity && newTarget.m_pMusterCity)
	{
		SCachedTarget memory;
		memory.iTargetCity = newTarget.m_pTargetCity->GetID();
		memory.iMusterCity = newTarget.m_pMusterCity->GetID();
		memory.bAttackBySea = newTarget.m_bAttackBySea;
		memory.bOcean = newTarget.m_bOcean;
		memory.bNoLandPath = newTarget.m_bNoLandPath;
		memory.iScore = iNewScore;
		memory.iTurnChosen = GC.getGame().getGameTurn();

		//update the cache
		m_cachedTargets[eEnemy][eAIOperationType] = memory;

		if(GC.getLogging() && GC.getAILogging())
		{
			CvString strOutBuf = CvString::format("%03d, %s, found new attack target, %s, muster: %s, optype: %d, score: %d",
				GC.getGame().getGameTurn(), m_pPlayer->getCivilizationShortDescription(), newTarget.m_pTargetCity->getName().c_str(), 
				newTarget.m_pMusterCity ? newTarget.m_pMusterCity->getName().c_str() : "NONE", eAIOperationType, iNewScore);
			CvString playerName = GetPlayer()->getCivilizationShortDescription();
			FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);
			if (pLog)
				pLog->Msg(strOutBuf);
		}
	}

	//either the cached one or we updated it
	if (piWinningScore)
		*piWinningScore = iNewScore;

	return newTarget;
}

/// Clear cached targets so we can try something else.
void CvMilitaryAI::ClearCachedTargets()
{
	m_cachedTargets.clear();
}

//override the diplo AI method so that it returns a sensible result for barbarians
bool CvMilitaryAI::IsPlayerValid(PlayerTypes eOtherPlayer)
{
	if (eOtherPlayer == BARBARIAN_PLAYER)
		return true;

	return m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eOtherPlayer);
}

/// Best target by land OR sea
CvMilitaryTarget CvMilitaryAI::FindBestAttackTargetGlobal(AIOperationTypes eAIOperationType, int* piWinningScore, bool bCheckWar)
{
	CvMilitaryTarget bestTarget;
	int iBestScore = 0;

	for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
		if(eLoopPlayer != GetPlayer()->GetID() && IsPlayerValid(eLoopPlayer))
		{
			if(bCheckWar)
			{
				if(!GET_TEAM(GetPlayer()->getTeam()).isAtWar(GET_PLAYER(eLoopPlayer).getTeam()))
					continue;
			}

			int iNewScore = 0;
			CvMilitaryTarget newTarget = FindBestAttackTargetCached(eAIOperationType,eLoopPlayer,&iNewScore);

			if (iNewScore>iBestScore)
			{
				iBestScore = iNewScore;
				bestTarget = newTarget;
			}
		}
	}

	if (piWinningScore)
		*piWinningScore = iBestScore;

	return bestTarget;
}
#endif

/// Best target by land OR sea
CvMilitaryTarget CvMilitaryAI::FindBestAttackTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, int* piWinningScore)
{
	vector<OptionWithScore<CvMilitaryTarget>> targets;
	CvPlayer &kEnemy = GET_PLAYER(eEnemy);
	int iFriendlyLoop;
	int iEnemyLoop;

	bool bNavalOp = false;
	if (eAIOperationType == AI_OPERATION_NAVAL_INVASION ||
		eAIOperationType == AI_OPERATION_NAVAL_INVASION_SNEAKY ||
		eAIOperationType == AI_OPERATION_NAVAL_INVASION_CITY_STATE ||
		eAIOperationType == AI_OPERATION_NAVAL_ONLY_CITY_ATTACK)
	{
		bNavalOp = true;
	}

	int iMaxTurns = 29; //going further out really makes little sense
	int iMinExposureScore = 50; //border cities are often strongly guarded ... so we need to attack even if we're outnumbered

	// sometimes we were forced into a war and don't even want to attack the enemy
	// in that case apply a higher standard
	if (!m_pPlayer->GetDiplomacyAI()->IsWantsToConquer(eEnemy))
	{
		iMaxTurns = 8; //only very close cities
		iMinExposureScore = 150; //only if the enemy city is isolated
	}

	// check for enticing enemy cities
	vector<OptionWithScore<CvCity*>> mostExposedEnemyCities;
	for (CvCity* pEnemyCity = kEnemy.firstCity(&iEnemyLoop); pEnemyCity != NULL; pEnemyCity = kEnemy.nextCity(&iEnemyLoop))
	{
		//higher than 100 is good for attack, lower is bad
		int iExposureScore = pEnemyCity->GetExposureScore(m_pPlayer->GetID(),bNavalOp);

		//don't take unnecessary risks
		if (iExposureScore < iMinExposureScore)
			continue;

		mostExposedEnemyCities.push_back( OptionWithScore<CvCity*>(pEnemyCity,iExposureScore) );
	}

	//look at the top 5 possible targets only
	std::sort(mostExposedEnemyCities.begin(), mostExposedEnemyCities.end());
	for (size_t i = 0; i < min(5u, mostExposedEnemyCities.size()); i++)
	{
		CvCity* pEnemyCity = mostExposedEnemyCities[i].option;

		//find possible muster cities
		vector<OptionWithScore<CvCity*>> closestOwnedCities;
		for (CvCity* pFriendlyCity = m_pPlayer->firstCity(&iFriendlyLoop); pFriendlyCity != NULL; pFriendlyCity = m_pPlayer->nextCity(&iFriendlyLoop))
		{
			if (TacticalAIHelpers::CountDeploymentPlots(pFriendlyCity->plot(), 3, m_pPlayer->getTeam(), bNavalOp)<8)
				continue;

			//plot distance may be misleading but it's fast
			//use negative distance because we sort descending later
			int iDistance = plotDistance( *pEnemyCity->plot(), *pFriendlyCity->plot() );
			closestOwnedCities.push_back( OptionWithScore<CvCity*>(pFriendlyCity,-iDistance) );
		}

		std::sort(closestOwnedCities.begin(), closestOwnedCities.end());
		for (size_t j = 0; j < min(5u, closestOwnedCities.size()); j++)
		{
			CvCity* pFriendlyCity = closestOwnedCities[j].option;

			CvMilitaryTarget target;
			target.m_pMusterCity = pFriendlyCity;
			target.m_pTargetCity = pEnemyCity;
			target.m_iExposureScore = mostExposedEnemyCities[i].score;

			//this is the important (and also costly) part
			CheckApproachFromLandAndSea(target, eAIOperationType, iMaxTurns);
			if (target.m_iPathLength == MAX_INT || target.m_iPathLength == -1)
				continue;

			if (bNavalOp && !target.m_bAttackBySea)
				continue;

			if (!bNavalOp && target.m_bNoLandPath)
				continue;

			int iWeight = ScoreTarget(target, eAIOperationType);
			if (iWeight > 0)
				targets.push_back(OptionWithScore<CvMilitaryTarget>(target, iWeight));
		}
	}

	// didn't find anything, abort
	if(targets.empty())
	{
		if (piWinningScore)
			*piWinningScore = -1;
		return CvMilitaryTarget();
	}
	//just take the best one
	std::sort(targets.begin(), targets.end());
	if (piWinningScore)
		*piWinningScore = targets.front().score;
	return targets.front().option;
}

/// Is it better to attack this target by sea?
void CvMilitaryAI::CheckApproachFromLandAndSea(CvMilitaryTarget& target, AIOperationTypes eAIOperationType, int iMaxTurnsAway)
{
	//start out with a land attack with invalid distance
	target.m_bAttackBySea = false;
	target.m_bOcean = false;
	target.m_bNoLandPath = false;
	target.m_iPathLength = MAX_INT;

	if (target.m_pMusterCity == NULL || target.m_pTargetCity == NULL)
		return;

	int iWaterLength = -1;
	int iLandLength = -1;

	//Only check naval if we're looking at a naval op!
	if (eAIOperationType == AI_OPERATION_NAVAL_INVASION || eAIOperationType == AI_OPERATION_NAVAL_INVASION_SNEAKY || eAIOperationType == AI_OPERATION_NAVAL_ONLY_CITY_ATTACK || eAIOperationType == AI_OPERATION_NAVAL_INVASION_CITY_STATE)
	{
		if (target.m_pMusterCity->isCoastal() && target.m_pTargetCity->isCoastal())
		{
			iWaterLength = GetCachedAttackTargetWaterDistance(target.m_pMusterCity, target.m_pTargetCity, iMaxTurnsAway);

			//Theoretically can make it via sea? Mark true so that naval ops can target this city.
			if (iWaterLength != -1)
			{
				target.m_bAttackBySea = true;
			}
		}
	}

	//Land and sea ops need to look here.
	iLandLength = GetCachedAttackTargetLandDistance(target.m_pMusterCity, target.m_pTargetCity, iMaxTurnsAway);
		
	if (iWaterLength == -1 && iLandLength == -1)
		return;

	if (iLandLength == -1)
	{
		target.m_bNoLandPath = true;
	}

	// There is no land path, or land path is over 2x as long as water path
	if (iWaterLength != -1)
	{
		if ((target.m_bNoLandPath) || (iLandLength > (2 * iWaterLength)))
		{
			target.m_iPathLength = iWaterLength;
			if (m_pPlayer->CanCrossOcean())
			{
				target.m_bOcean = true;
			}
			return;
		}
	}
		
	if (!target.m_bNoLandPath && iWaterLength != -1)
	{
		if (eAIOperationType == AI_OPERATION_NAVAL_INVASION || eAIOperationType == AI_OPERATION_NAVAL_INVASION_SNEAKY)
		{
			int iEvalInvasionApproach = EvaluateMilitaryApproaches(target.m_pTargetCity, true, true);
			if (iEvalInvasionApproach == ATTACK_APPROACH_RESTRICTED)
			{
				target.m_bAttackBySea = false;
				return;
			}
			else
			{
				int iEvalLandApproach = EvaluateMilitaryApproaches(target.m_pTargetCity, true, false);
				int iEvalSeaApproach = EvaluateMilitaryApproaches(target.m_pTargetCity, false, true);
				//Is their city relatively difficult to access via land? Go for a sea attack then.
				if (iEvalLandApproach == ATTACK_APPROACH_RESTRICTED && iEvalSeaApproach > iEvalLandApproach)
				{
					target.m_bAttackBySea = true;
					if (m_pPlayer->CanCrossOcean())
					{
						target.m_bOcean = true;
						target.m_iPathLength = iWaterLength;
						return;
					}
					else
					{
						target.m_iPathLength = iWaterLength;
						return;
					}
				}
			}
		}
		else
		{
			int iEvalLandApproach = EvaluateMilitaryApproaches(target.m_pTargetCity, true, false);
			int iEvalSeaApproach = EvaluateMilitaryApproaches(target.m_pTargetCity, false, true);
			//Is their city relatively difficult to access via land? Go for a sea attack then.
			if (iEvalLandApproach == ATTACK_APPROACH_RESTRICTED && iEvalSeaApproach > iEvalLandApproach)
			{
				target.m_bAttackBySea = true;
				if (m_pPlayer->CanCrossOcean())
				{
					target.m_bOcean = true;
					target.m_iPathLength = iWaterLength;
					return;
				}
				else
				{
					target.m_iPathLength = iWaterLength;
					return;
				}
			}
		}	
	}
	if (!target.m_bNoLandPath && target.m_pMusterCity->getArea() == target.m_pTargetCity->getArea())
	{
		target.m_iPathLength = iLandLength;
		return;
	}
}


#if defined(MOD_BALANCE_CORE_MILITARY)

/// Come up with a target priority looking at distance, strength, approaches (high score = more desirable target)
int CvMilitaryAI::ScoreTarget(CvMilitaryTarget& target, AIOperationTypes eAIOperationType)
{
	if(target.m_pMusterCity == NULL || target.m_pTargetCity == NULL)
	{
		return 0;
	}

	DomainTypes eDomain = DOMAIN_LAND;
	if (eAIOperationType == AI_OPERATION_NAVAL_INVASION ||
		eAIOperationType == AI_OPERATION_NAVAL_INVASION_SNEAKY ||
		eAIOperationType == AI_OPERATION_NAVAL_ONLY_CITY_ATTACK ||
		eAIOperationType == AI_OPERATION_NAVAL_INVASION_CITY_STATE)
	{
		eDomain = DOMAIN_SEA;
	}

	float fDistWeightInterpolated = 1;
	if(!target.m_bAttackBySea)
	{
		// interpolate linearly between a low and a high distance
		float fDistanceLow = 8, fWeightLow = 10;
		float fDistanceHigh = 24, fWeightHigh = 1;

		// Is this a sneakattack?  If so distance is REALLY important (want to target spaces on edge of empire)
		if (eAIOperationType == AI_OPERATION_CITY_SNEAK_ATTACK)
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
		float fDistanceLow = 8, fWeightLow = 6;
		float fDistanceHigh = 36, fWeightHigh = 1;

		// Is this a sneak attack?  If so distance is REALLY important (want to target spaces on edge of empire)
		if (eAIOperationType == AI_OPERATION_NAVAL_INVASION_SNEAKY || eAIOperationType == AI_OPERATION_NAVAL_ONLY_CITY_ATTACK)
		{
			fDistanceLow /= 2;
			fDistanceHigh /= 2;
		}

		float fSlope = (fWeightHigh-fWeightLow) / (fDistanceHigh-fDistanceLow);
		fDistWeightInterpolated = (target.m_iPathLength-fDistanceLow) * fSlope + fWeightLow;
		fDistWeightInterpolated = min( fWeightLow, max( fWeightHigh, fDistWeightInterpolated ) );

		// If coming over sea, inland cities are trickier
		if(!target.m_pTargetCity->isCoastal())
			fDistWeightInterpolated /= 2;
	}

	CityAttackApproaches eApproaches;
	float fApproachMultiplier = 1;
	if(eAIOperationType == AI_OPERATION_NAVAL_ONLY_CITY_ATTACK)
	{	
		//naval only
		eApproaches = EvaluateMilitaryApproaches(target.m_pTargetCity, false, true);
	}
	else if(eAIOperationType == AI_OPERATION_NAVAL_INVASION ||
		eAIOperationType == AI_OPERATION_NAVAL_INVASION_SNEAKY ||
		eAIOperationType == AI_OPERATION_NAVAL_INVASION_CITY_STATE)
	{	
		//mixed
		eApproaches = EvaluateMilitaryApproaches(target.m_pTargetCity, true, true);
	}
	else if(eAIOperationType == AI_OPERATION_CITY_BASIC_ATTACK ||
		eAIOperationType == AI_OPERATION_CITY_SNEAK_ATTACK)
	{	
		//land only
		eApproaches = EvaluateMilitaryApproaches(target.m_pTargetCity, true, false);
	}
	else
	{
		//automatic ...
		eApproaches = EvaluateMilitaryApproaches(target.m_pTargetCity, true, target.m_bAttackBySea);
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

	//scores for each target are estimated before calling this function
	float fExposureScore = min(10.f, target.m_iExposureScore / 100.f);
	float fDesirability = 100.f;

	bool bMinorButMajorWar = false;
	if(m_pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith(false) > 0)
	{
		if(GET_PLAYER(target.m_pTargetCity->getOwner()).isMinorCiv())
		{
			bMinorButMajorWar = true;
		}
		if(GET_PLAYER(target.m_pTargetCity->getOwner()).isBarbarian())
		{
			bMinorButMajorWar = true;
		}
	}

	if (target.m_pTargetCity->IsOriginalCapital() && (m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest() || m_pPlayer->GetDiplomacyAI()->IsCloseToDominationVictory()))
	{
		fDesirability *= GC.getAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL();
		fDesirability /= 100;
	}
	if (target.m_pTargetCity->getOriginalOwner() == m_pPlayer->GetID())
	{
		fDesirability *= GC.getAI_MILITARY_RECAPTURING_OWN_CITY();
		fDesirability /= 100;
	}
	//Going after a City-State? Let's not force it, in the event that we're fighting a major civ.
	if(bMinorButMajorWar)
	{
		if(GET_PLAYER(target.m_pTargetCity->getOwner()).isMinorCiv())
		{
			PlayerTypes eAlly = GET_PLAYER(target.m_pTargetCity->getOwner()).GetMinorCivAI()->GetAlly();
			if(eAlly != NO_PLAYER)
			{
				if(GET_TEAM(GET_PLAYER(eAlly).getTeam()).isAtWar(GetPlayer()->getTeam()))
				{
					fDesirability *= 100;
					fDesirability /= max(1, GC.getAI_MILITARY_CAPTURING_ORIGINAL_CAPITAL());
				}
			}
			else
			{
				//We should not be targeting CSs like this!
				return 0;
			}
		}
		else if(GET_PLAYER(target.m_pTargetCity->getOwner()).isBarbarian())
		{
			// if it's right on our doorstep an attack is helpful. barbarian cities are easy targets
			if (target.m_iPathLength>17)
				return 0;
		}
	}
	//If we were given a quest to go to war with this player, that should influence our decision. Plus, it probably means he's a total jerk.
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinor = (PlayerTypes) iMinorLoop;
		if(eMinor != NO_PLAYER)
		{
			CvPlayer* pMinor = &GET_PLAYER(eMinor);
			if(pMinor)
			{
				CvMinorCivAI* pMinorCivAI = pMinor->GetMinorCivAI();
				TeamTypes eConquerorTeam = GET_TEAM(pMinor->getTeam()).GetKilledByTeam();

				if(pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_WAR))
				{
					if(pMinorCivAI->GetQuestData1(GetPlayer()->GetID(), MINOR_CIV_QUEST_WAR) == target.m_pTargetCity->getOwner())
					{
						fDesirability *= GC.getAI_MILITARY_RECAPTURING_OWN_CITY();
						fDesirability /= 100;
					}
				}
				if(pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_LIBERATION))
				{
					if(pMinorCivAI->GetQuestData1(GetPlayer()->GetID(), MINOR_CIV_QUEST_LIBERATION) == eMinor)
					{
						if(eConquerorTeam == target.m_pTargetCity->getTeam())
						{
							fDesirability *= GC.getAI_MILITARY_RECAPTURING_OWN_CITY();
							fDesirability /= 100;
						}
					}
				}
				if(pMinorCivAI && pMinorCivAI->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_UNIT_GET_CITY))
				{
					int iX = pMinorCivAI->GetQuestData1(GetPlayer()->GetID(), MINOR_CIV_QUEST_UNIT_GET_CITY);
					int iY = pMinorCivAI->GetQuestData2(GetPlayer()->GetID(), MINOR_CIV_QUEST_UNIT_GET_CITY);

					CvPlot* pPlot = GC.getMap().plot(iX, iY);
					if(pPlot != NULL && pPlot->isCity())
					{
						if(pPlot->getOwner() == target.m_pTargetCity->getOwner())
						{
							fDesirability *= GC.getAI_MILITARY_RECAPTURING_OWN_CITY();
							fDesirability /= 100;
						}
					}
				}
			}
		}
	}

	//Venice special - prefer to attack cities between our far-flung bases
	bool bIsVenice = m_pPlayer->GetPlayerTraits()->IsNoAnnexing();
	if (bIsVenice && m_pPlayer->getCapitalCity())
	{
		CvPlot* pTargetPlot = target.m_pTargetCity->plot();
		int iDistToCapital = plotDistance(*pTargetPlot,*m_pPlayer->getCapitalCity()->plot());
		fDesirability *= 1 + ( m_pPlayer->GetCityDistanceInPlots(pTargetPlot) / float(iDistToCapital) );
	}

#if defined(MOD_BALANCE_CORE)
	//If we get instant yields from conquering specific terrain types, look for cities with those
	for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;

		if (eYield == NO_YIELD)
			continue;

		for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
		{
			TerrainTypes eTerrain = (TerrainTypes)iTerrain;

			if (eTerrain == NO_TERRAIN)
				continue;

			if (m_pPlayer->GetPlayerTraits()->GetYieldChangeFromTileConquest(eTerrain, eYield) > 0)
			{
				int iNumTerrain = target.m_pTargetCity->CountTerrain(eTerrain); // not using CountAllOwnedTerrain to save performance
				if (iNumTerrain > 0)
				{
					fDesirability *= 100 + (3 * iNumTerrain);
					fDesirability /= 100;
				}
			}
		}
	}
#endif

	//Muster already targeted by operation? De-emphasize.
	if (m_pPlayer->IsMusterCityAlreadyTargeted(target.m_pMusterCity, eDomain, 0, -1))
	{
		fDesirability /= 2;
	}

	//Naval invasion? Let's target the same cities that we're moving on purely.
	if (eDomain == DOMAIN_SEA)
	{
		if (eAIOperationType == AI_OPERATION_NAVAL_INVASION && m_pPlayer->IsCityAlreadyTargeted(target.m_pTargetCity, eDomain, 0, -1, AI_OPERATION_NAVAL_ONLY_CITY_ATTACK))
		{
			fDesirability *= 150;
			fDesirability /= 100;
		}
		//Same, but flipped.
		else if (eAIOperationType == AI_OPERATION_NAVAL_ONLY_CITY_ATTACK && m_pPlayer->IsCityAlreadyTargeted(target.m_pTargetCity, eDomain, 0, -1, AI_OPERATION_NAVAL_INVASION))
		{
			fDesirability *= 150;
			fDesirability /= 100;
		}
	}
	else
	{
		//Land basic attack? Let's move on cities that we're attacking navally.
		if (target.m_pTargetCity->isCoastal())
		{
			if (eAIOperationType == AI_OPERATION_CITY_BASIC_ATTACK && m_pPlayer->IsCityAlreadyTargeted(target.m_pTargetCity, NO_DOMAIN, 50, -1, AI_OPERATION_NAVAL_ONLY_CITY_ATTACK))
			{
				fDesirability *= 150;
				fDesirability /= 100;
			}
			else if (eAIOperationType == AI_OPERATION_NAVAL_ONLY_CITY_ATTACK && m_pPlayer->IsCityAlreadyTargeted(target.m_pTargetCity, NO_DOMAIN, 50, -1, AI_OPERATION_CITY_BASIC_ATTACK))
			{
				fDesirability *= 150;
				fDesirability /= 100;
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
					fDesirability *= 125;
					fDesirability /= 100;
					break;
				}
			}
		}
	}
#endif
	fDesirability /= 100;
	
	// Economic value of target
	float fEconomicValue = 1.0;
	fEconomicValue = (float)target.m_pTargetCity->getEconomicValue( GetPlayer()->GetID() );
	fEconomicValue = sqrt(fEconomicValue/250);

	//everything together now
	int iRtnValue = (int)(100 * fDistWeightInterpolated * fApproachMultiplier * fExposureScore * fDesirability * fEconomicValue);

	/*
	if(GC.getLogging() && GC.getAILogging())
	{
		// Open the right file
		CvString playerName = GetPlayer()->getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		CvString msg = CvString::format( "%03d, %s is evaluating attack on %s from base in %s. Approach is %.2f. Distance is %d (weight %.2f), desirability %.2f, exposure score %.2f, economic value %.2f --> score %d\n", 
			GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription(), target.m_pTargetCity->getName().c_str(), target.m_pMusterCity->getName().c_str(), 
			fApproachMultiplier, target.m_iPathLength, fDistWeightInterpolated, fDesirability, fExposureScore, fEconomicValue, iRtnValue );
		pLog->Msg( msg.c_str() );
	}
	*/

	return iRtnValue;
}
#endif

/// How open an approach do we have to this city if we want to attack it?
CityAttackApproaches CvMilitaryAI::EvaluateMilitaryApproaches(CvCity* pCity, bool bAttackByLand, bool bAttackBySea)
{
	if (!pCity || !pCity->plot()->isRevealed(m_pPlayer->getTeam()))
		return ATTACK_APPROACH_NONE;

	CityAttackApproaches eRtnValue = ATTACK_APPROACH_UNRESTRICTED;
	int iRefDist = m_pPlayer->GetCityDistanceInEstimatedTurns(pCity->plot());

	//Expanded to look at three hexes around each city - will give a better understanding of approach.
	int iNumPlots = 0;
	int iNumBlocked = 0;
	int iNumTough = 0;
	int iMaxRing = 3; //at most 5
	
	for (int iRing = 1; iRing <= iMaxRing; iRing++)
	{
		for (int i = RING_PLOTS[iRing-1]; i < RING_PLOTS[iRing]; i++)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pCity->plot(),i);
			if(pLoopPlot == NULL)
				continue;

			//inner rings count more
			int iWeight = 1 + iMaxRing - iRing;

			bool bBlocked = false;
			bool bTough = false;

			//cannot go here? important, ignore territory checks (typically we are at peace without open borders)
			if(!pLoopPlot->isValidMovePlot(m_pPlayer->GetID(),false) || pLoopPlot->isCity())
				bBlocked = true;

			//ignore plots which are "behind" the city if it's a foreign city (AI always takes the shortest path)
			if (m_pPlayer->GetID() != pCity->getOwner() && m_pPlayer->GetCityDistanceInEstimatedTurns(pLoopPlot) > iRefDist)
				bBlocked = true;

			//ignore plots owned by third parties
			if (pLoopPlot->isOwned() && pLoopPlot->getTeam() != m_pPlayer->getTeam() && pLoopPlot->getTeam() != pCity->getTeam())
				bBlocked = !GET_TEAM(m_pPlayer->getTeam()).IsAllowsOpenBordersToTeam( pLoopPlot->getTeam() );

			//should not go here
			if (GetPlayer()->GetPlotDanger(*pLoopPlot, true) > 9)
				bTough = true;

			//makes us slow
			if(	pLoopPlot->isRoughGround() )
				bTough = true;

			//correct area?
			if ( !pCity->isMatchingArea(pLoopPlot) )
				bBlocked = true;

			if (bAttackByLand && !bAttackBySea)
				//siege weapons cannot set up here
				if (pLoopPlot->isWater())
					bBlocked = true;

			if (bAttackBySea && !bAttackByLand)
				//ships cannot go here
				if (!pLoopPlot->isWater())
					bBlocked = true;

			//Invasion logic slightly different - we need lots of coastal plots.
			if (bAttackBySea && bAttackByLand)
				if (!pLoopPlot->isCoastalLand() && !pLoopPlot->isShallowWater())
					bTough = true;

			//todo: what about air attack?
			iNumPlots+=iWeight;

			if (bBlocked)
				iNumBlocked+=iWeight;
			else if (bTough)
				iNumTough+=iWeight;
		}
	}

	//with weighting, maximum score for 3 rings is 6*3 + 12*2 + 18 = 60
	int iFree = iNumPlots - iNumBlocked - iNumTough/2;

	if (iFree > 27)
		eRtnValue = ATTACK_APPROACH_UNRESTRICTED;
	else if (iFree > 17)
		eRtnValue = ATTACK_APPROACH_OPEN;
	else if (iFree > 11)
		eRtnValue = ATTACK_APPROACH_NEUTRAL;
	else if (iFree > 7)
		eRtnValue = ATTACK_APPROACH_LIMITED;
	else if (iFree > 3)
		eRtnValue = ATTACK_APPROACH_RESTRICTED;
	else
		eRtnValue = ATTACK_APPROACH_NONE;

	if (gDebugOutput)
	{
		const char* approachName[] = { "none", "restricted", "limited", "neutral", "open", "unrestricted" };
		const char* mode = bAttackBySea ? (bAttackByLand ? "combined" : "naval") : (bAttackByLand ? "land" : "invalid");
		OutputDebugString(CvString::format("%s attack approach on %s is %s for %s (score %d)\n", mode, pCity->getNameKey(), approachName[eRtnValue], m_pPlayer->getNameKey(), iFree).c_str());
	}

	return eRtnValue;
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

	// Major threat for each camp seen
#if defined(MOD_BALANCE_CORE)
	iRtnValue += GC.getAI_MILITARY_THREAT_WEIGHT_SEVERE() * m_iBarbarianCampCount;
	// One minor threat for every X barbarians
	iRtnValue += m_iVisibleBarbarianCount / GC.getAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT();
#else
	ScanForBarbarians();
	iRtnValue += GC.getAI_MILITARY_THREAT_WEIGHT_MAJOR() * m_iBarbarianCampCount;
	// One minor threat for every X barbarians
	iRtnValue += m_iVisibleBarbarianCount / GC.getAI_MILITARY_BARBARIANS_FOR_MINOR_THREAT();
#endif

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
		if(GET_PLAYER(eLoopPlayer).isBarbarian())
		{
			continue;
		}
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

vector<CvCity*> CvMilitaryAI::GetThreatenedCities(bool bIncludeFutureThreats, bool bCoastalOnly)
{
	std::vector<std::pair<CvCity*,int>> vCities;
	struct PrSortByScore {
		bool operator()(const std::pair<CvCity*,int> &left, const std::pair<CvCity*,int> &right) {
			return left.second > right.second;
		}
	};

	CvTacticalAnalysisMap* pTactMap = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap();

	CvCity* pLoopCity;
	int iLoopCity = 0;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if (bCoastalOnly && !pLoopCity->isCoastal())
			continue;

		//with the new danger plots, the stored threat value should be accurate
		int iThreatValue = pLoopCity->getThreatValue();

		if (pTactMap)
		{
			CvTacticalDominanceZone* pLandZone = pTactMap->GetZoneByCity(pLoopCity,false);
			CvTacticalDominanceZone* pWaterZone = pTactMap->GetZoneByCity(pLoopCity,true);

			if (pLandZone && pLandZone->GetOverallDominanceFlag()==TACTICAL_DOMINANCE_ENEMY)
				iThreatValue += 50;
			if (pWaterZone && pWaterZone->GetOverallDominanceFlag()==TACTICAL_DOMINANCE_ENEMY)
				iThreatValue += 50;
		}

		// Is this a temporary zone city? If so, we need to support it ASAP.
		if (m_pPlayer->GetTacticalAI()->IsTemporaryZoneCity(pLoopCity))
			iThreatValue += GC.getAI_MILITARY_CITY_THREAT_WEIGHT_CAPITAL();

		float fScale = 1.f;
		if (bIncludeFutureThreats)
		{
			int iNeutral = 0;
			int iBad = 0;
			int iSuperBad = 0;

			//check the wider area for enemy tiles. may also be on another landmass
			for(int i=2; i<RING5_PLOTS; i++)
			{
				CvPlot* pLoopNearbyPlot = iterateRingPlots(pLoopCity->plot(), i);

				//Don't want them adjacent to cities, but we do want to check for plot ownership.
				if (pLoopNearbyPlot != NULL && pLoopNearbyPlot->isRevealed(m_pPlayer->getTeam()))
				{
					if((pLoopNearbyPlot->getOwner() != m_pPlayer->GetID()) && (pLoopNearbyPlot->getOwner() != NO_PLAYER) && !(GET_PLAYER(pLoopNearbyPlot->getOwner()).isMinorCiv()))
					{
						PlayerTypes pNeighborNearby = pLoopNearbyPlot->getOwner();
						if(pNeighborNearby != NULL)
						{
							if(m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(pNeighborNearby) == MAJOR_CIV_OPINION_NEUTRAL)
							{
								iNeutral++;
							}
							else if(m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(pNeighborNearby) == MAJOR_CIV_OPINION_COMPETITOR)
							{
								iBad++;
							}
							else if(m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(pNeighborNearby) < MAJOR_CIV_OPINION_COMPETITOR)
							{
								iSuperBad++;
							}
						}
					}
				}
			}

			iThreatValue += (iNeutral * 5);
			iThreatValue += (iBad * 10);
			iThreatValue += (iSuperBad * 25);
		}

		//note: we don't consider a cities size or economic importance here
		//after all, small border cities are especially vulnerable

		//tolerate some danger
		if(iThreatValue <= GC.getCITY_HEAL_RATE())
			continue;

		//OutputDebugString( CvString::format("%s - threat %d - scale %.3f\n", pLoopCity->getName().c_str(), iThreatValue, fScale ).c_str() );
		vCities.push_back( std::make_pair( pLoopCity, (int)(iThreatValue*fScale) ) );
	}

	std::stable_sort(vCities.begin(), vCities.end(), PrSortByScore());

	vector<CvCity*> result;
	for (size_t i=0; i<vCities.size(); i++)
	{
		//keep only the top half of the scores
		if (vCities[i].second < vCities[0].second/2)
			break;

		result.push_back(vCities[i].first);
	}

	return result;
}

/// Which city is in the most danger now?
CvCity* CvMilitaryAI::GetMostThreatenedCity(bool bIncludeFutureThreats, bool bCoastalOnly)
{
	vector<CvCity*> allCities = GetThreatenedCities(bIncludeFutureThreats, bCoastalOnly);
	if (allCities.empty())
		return 0;
	else
		return allCities.front();
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

/// Log chosen attack
void CvMilitaryAI::LogChosenTarget(AIOperationTypes eAIOperationType, PlayerTypes eEnemy, CvMilitaryTarget& target)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString strLogName;

		// Open the log file
		CvString playerName = GetPlayer()->getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";
		if(eAIOperationType == AI_OPERATION_CITY_BASIC_ATTACK)
		{
			strBaseString += "Basic Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_CITY_SNEAK_ATTACK)
		{
			strBaseString += "Sneak Attack, ";
		}
		else if(eAIOperationType == AI_OPERATION_NAVAL_ONLY_CITY_ATTACK)
		{
			strBaseString += "Pure Naval Attack, ";
		}
		else if (eAIOperationType == AI_OPERATION_NAVAL_INVASION)
		{
			strBaseString += "Naval Invasion, ";
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
#if defined(MOD_BALANCE_CORE)
		if(target.m_bOcean)
		{
			strOutBuf += ", Ocean";
		}
#endif
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

/// Log it if we try to DOW a vassal
void CvMilitaryAI::LogVassalFailure(TeamTypes eOpponentTeam)
{
	if (GC.getLogging() && GC.getAILogging())
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
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (kPlayer.isAlive() && kPlayer.getTeam() == eOpponentTeam)
			{
				if (strOpponentName.GetLength() != 0)
					strOpponentName += ", ";

				strOpponentName += kPlayer.getCivilizationShortDescription();
			}
		}
		// Strategy Info
		strTemp.Format("Tried to DOW a vassal without war on master first...WTF! : %d (%s)", (int)eOpponentTeam, strOpponentName.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}
}

/// Log that a unit is being scrapped because of the deficit checks
void CvMilitaryAI::LogDeficitScrapUnit(CvUnit* pUnit)
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
	m_iNumLandUnits = 0;
	m_iNumRangedLandUnits = 0;
	m_iNumMobileLandUnits = 0;
	m_iNumAirUnits = 0;
	m_iNumAntiAirUnits = 0;
	m_iNumMeleeLandUnits = 0;
	m_iNumNavalUnits = 0;
	m_iNumLandUnitsInArmies = 0;
	m_iNumNavalUnitsInArmies = 0;
	m_iRecNavySize = 0;

	int iLoop;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// Don't count civilians or exploration units
		if(pLoopUnit->IsCanAttack() && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{
			if(pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				m_iNumLandUnits++;

				if(pLoopUnit->getArmyID() != -1)
				{
					m_iNumLandUnitsInArmies++;
				}

				if(pLoopUnit->IsCanAttackRanged())
				{
					m_iNumRangedLandUnits++;
				}
				else if(pLoopUnit->canIntercept())
				{
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
				if(pLoopUnit->getArmyID() != -1)
				{
					m_iNumNavalUnitsInArmies++;
				}
				m_iNumNavalUnits++;
			}
			else if(pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				m_iNumAirUnits++;
			}
		}
	}

	EconomicAIStrategyTypes eStrategyNavalMap = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NAVAL_MAP");
	bool bNavalMap = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyNavalMap);
	int iFlavorOffense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	int iFlavorDefense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"));

	// Scale up or down based on true threat level and a bit by flavors (multiplier should range from about 0.5 to about 1.5)
	float fMultiplier = (float)0.2 + (((float)(m_pPlayer->GetMilitaryAI()->GetHighestThreat() + iFlavorOffense + iFlavorDefense)) / (float)100.0);

	// first get the number of defenders that we think we need

	// Start with 3, to protect the capital
	int iNumUnitsWanted = GC.getAI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS();

	// 1 Unit per City & 1 per Settler
	iNumUnitsWanted += (int)(m_pPlayer->getNumCities() * /*1.0*/ GC.getAI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY());
	iNumUnitsWanted += m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);

#if defined(MOD_BALANCE_CORE_MILITARY)
	//Look at neighbors - if they're stronger than us, let's increase our amount.
	if (MOD_BALANCE_CORE_MILITARY && m_pPlayer->isMajorCiv())
	{
		for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			PlayerTypes eOtherPlayer = (PlayerTypes) iMajorLoop;
			if (eOtherPlayer != NO_PLAYER && GET_PLAYER(eOtherPlayer).isMajorCiv() && GET_PLAYER(eOtherPlayer).isAlive() && (eOtherPlayer != m_pPlayer->GetID()))
			{
				if (GetPlayer()->GetDiplomacyAI()->IsPotentialMilitaryTargetOrThreat(eOtherPlayer))
				{
					fMultiplier += 0.2f;
				}
			}
		}
	}

	int iCityLoop;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		if (pLoopCity && (pLoopCity->isUnderSiege() || pLoopCity->isBorderCity()))
		{
			fMultiplier += .25f;
		}
	}
#endif

	m_iMandatoryReserveSize = (int)(iNumUnitsWanted * fMultiplier);

#if defined(MOD_BALANCE_CORE)
	//More wonders = much greater need for units to defend them!
	m_iMandatoryReserveSize += ((m_pPlayer->GetNumWonders() / 5));
#endif

	// add in a few for the difficulty level (all above Chieftain are boosted)
	//int iDifficulty = max(0,GC.getGame().getHandicapInfo().GetID() - 1);
	//m_iMandatoryReserveSize += iDifficulty;

	m_iMandatoryReserveSize = max(3,m_iMandatoryReserveSize);

	// now we add in the strike forces we think we will need
	if(m_pPlayer->isMinorCiv())
	{
		iNumUnitsWanted = 0;
	}
	else
	{
		iNumUnitsWanted = /*8*/ GC.getBALANCE_BASIC_ATTACK_ARMY_SIZE(); // size of a basic attack - embiggened!
		iNumUnitsWanted += m_pPlayer->GetCurrentEra() * 2;

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
		iNumUnitsWanted += (m_pPlayer->GetDiplomacyAI()->GetBoldness() / 2);

		// add in more if we are playing on a high difficulty
		//iNumUnitsWanted += iDifficulty;

		iNumUnitsWanted = (int)(iNumUnitsWanted * fMultiplier);

		iNumUnitsWanted = max(1,iNumUnitsWanted);
	}

	if (bNavalMap)
	{
		iNumUnitsWanted *= 2;
		iNumUnitsWanted /= 3;
		m_iMandatoryReserveSize /= 4;
	}

	m_iRecommendedMilitarySize = m_iMandatoryReserveSize + iNumUnitsWanted;

	int iFlavorNaval = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
	iFlavorNaval += m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_RECON"));
	iFlavorNaval /= 2;

#if defined(MOD_BALANCE_CORE_MILITARY)
	if(m_iRecommendedMilitarySize > GetPlayer()->GetNumUnitsSupplied())
	{
		m_iRecommendedMilitarySize = GetPlayer()->GetNumUnitsSupplied();
	}

	int iNavalPercent = iFlavorNaval > 7 ? 40 : 25;
	int iNavalReserve = GetPlayer()->GetNumUnitsSupplied() * iNavalPercent;
	iNavalReserve /= 100;

	m_iRecommendedMilitarySize -= iNavalReserve;

	//And navy, because we reference this somewhat often.
	m_iRecNavySize = MilitaryAIHelpers::ComputeRecommendedNavySize(m_pPlayer, iNavalReserve);
#endif
}

/// Update how we're doing on defensive units
void CvMilitaryAI::UpdateDefenseState()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateDefenseState, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	// Derive data we'll need
	int iLandUnitsNotInArmies = m_iNumLandUnits;
	int iNavalUnitsNotInArmies = m_iNumNavalUnits;

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

	if (m_eLandDefenseState <= DEFENSE_STATE_NEUTRAL)
	{
		int iCityLoop;
		for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
		{
			if (pLoopCity->isUnderSiege() || pLoopCity->isInDangerOfFalling())
			{
				m_eLandDefenseState = DEFENSE_STATE_CRITICAL;
				break;
			}
		}
	}

	if(iNavalUnitsNotInArmies <= (m_iRecNavySize / 2))
	{
		m_eNavalDefenseState = DEFENSE_STATE_CRITICAL;
	}
	else if(iNavalUnitsNotInArmies <= m_iRecNavySize)
	{
		m_eNavalDefenseState = DEFENSE_STATE_NEEDED;
	}
	else if(iNavalUnitsNotInArmies <= m_iRecNavySize * 5 / 4)
	{
		m_eNavalDefenseState = DEFENSE_STATE_NEUTRAL;
	}
	else
	{
		m_eNavalDefenseState = DEFENSE_STATE_ENOUGH;
	}

	if (m_eNavalDefenseState <= DEFENSE_STATE_NEUTRAL)
	{
		int iCityLoop;
		for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
		{
			if (pLoopCity->isCoastal() && (pLoopCity->isUnderSiege() || pLoopCity->isInDangerOfFalling()))
			{
				m_eNavalDefenseState = DEFENSE_STATE_CRITICAL;
				break;
			}
		}
	}
}

/// Count up barbarian camps and units visible to us
void CvMilitaryAI::ScanForBarbarians()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("ScanForBarbarians, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

#if defined(MOD_BALANCE_CORE)
	int iLastTurnBarbarianCount = m_iVisibleBarbarianCount;
#endif

	m_iBarbarianCampCount = 0;
	m_iVisibleBarbarianCount = 0;

	TeamTypes eTeam = m_pPlayer->getTeam();

	// Look at revealed Barbarian camps and visible units
	for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if(pPlot->isRevealed(eTeam))
		{
			if(pPlot->getImprovementType() == GC.getBARBARIAN_CAMP_IMPROVEMENT())
			{
				m_iBarbarianCampCount++;

				// Count it as 10 camps if sitting inside our territory, that is annoying!
				if(pPlot->getOwner() == m_pPlayer->GetID())
				{
					m_iBarbarianCampCount += 2;
				}
#if defined(MOD_BALANCE_CORE)
				// Count it as -5 camps if sitting inside someone else's territory.
				if(pPlot->getOwner() != m_pPlayer->GetID() && pPlot->getOwner() != NO_PLAYER)
				{
					m_iBarbarianCampCount -= 2;
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
								m_iBarbarianCampCount += 2;
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

#if defined(MOD_BALANCE_CORE)
	//try to smooth the count a bit
	if (m_iVisibleBarbarianCount < iLastTurnBarbarianCount)
		m_iVisibleBarbarianCount = iLastTurnBarbarianCount-1;
#endif

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

void CvMilitaryAI::DoNuke(PlayerTypes ePlayer)
{
	bool bLaunchNuke = false;
	WarProjectionTypes eCurrentWarProjection = m_pPlayer->GetDiplomacyAI()->GetWarProjection(ePlayer);
	WarStateTypes eCurrentWarState = m_pPlayer->GetDiplomacyAI()->GetWarState(ePlayer);
	// only evaluate nukes when we have nukes and we've declared war on someone
	if (m_pPlayer->getNumNukeUnits() > 0) 
	{
		// they nuked us, so we can nuke them.
		if (m_pPlayer->GetDiplomacyAI()->GetNumTimesNuked(ePlayer) > 0)
		{	
			bLaunchNuke = true;
		}
		// if we already nuked them, uhhh, keep it up!
		else if (GET_PLAYER(ePlayer).GetDiplomacyAI()->GetNumTimesNuked(m_pPlayer->GetID()) > 0)
		{
			bLaunchNuke = true;
		}
		// if we will surely lose this war anyway, we might as well nuke them!
		else if (eCurrentWarProjection == WAR_PROJECTION_DESTRUCTION || eCurrentWarState == WAR_STATE_NEARLY_DEFEATED)
		{
			bLaunchNuke = true;
		}
		else 
		{
			bool bRollForNuke = false;
			if(GET_PLAYER(ePlayer).isMajorCiv())
			{
				MajorCivOpinionTypes eMajorCivOpinion = m_pPlayer->GetDiplomacyAI()->GetMajorCivOpinion(ePlayer);
				if (eCurrentWarProjection <= WAR_PROJECTION_DEFEAT || eCurrentWarState == WAR_STATE_DEFENSIVE)
				{
					// roll every turn
					bRollForNuke = true;
				}
				else if(eMajorCivOpinion <= MAJOR_CIV_OPINION_ENEMY)
				{
					bRollForNuke = true;
				}
				else if(m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest())
				{
					bRollForNuke = true;
				}
#if defined(MOD_BALANCE_CORE)
				else if (GET_PLAYER(ePlayer).GetDiplomacyAI()->IsCloseToAnyVictoryCondition())
				{
					bRollForNuke = true;
				}
#endif
				else if (m_pPlayer->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(ePlayer) >= STRENGTH_POWERFUL)
				{
					bRollForNuke = true;
				}
				if (bRollForNuke)
				{
					int iFlavorNuke = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_USE_NUKE"));
					int iRoll = GC.getGame().getSmallFakeRandNum(10, GET_PLAYER(ePlayer).GetPseudoRandomSeed());
					int iRoll2 = GC.getGame().getSmallFakeRandNum(10, m_pPlayer->GetPseudoRandomSeed());
					if (iRoll < iFlavorNuke && iRoll2 < iFlavorNuke)
					{
						bLaunchNuke = true;
					}
				}
			}
		}
	}

	if (bLaunchNuke)
	{
		RequestNukeAttack(ePlayer);
	}
}

void CvMilitaryAI::DoBarbs()
{
	int iOperationID;
	int iFilledSlots;
	int iNumRequiredSlots;
	MilitaryAIStrategyTypes eStrategyBarbs = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
	MilitaryAIStrategyTypes eStrategyBarbsCritical = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS_CRITICAL");
	bool bStrategyFightAWar =  MilitaryAIHelpers::IsTestStrategy_AtWar(m_pPlayer, true);
	// SEE IF THERE ARE OPERATIONS THAT NEED TO BE ABORTED

	// Are we willing to risk pressing forward vs. barbarians?
	bool bWillingToAcceptRisk = (m_iTotalThreatWeight / 2) < GetBarbarianThreatTotal() ||
		m_pPlayer->GetPlayerTraits()->GetLandBarbarianConversionPercent() > 0 || 
		IsUsingStrategy(eStrategyBarbsCritical);

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
	//
	// Operations vs. Barbarians
	//
	// If have aborted the eradicate barbarian strategy or if the threat level from civs is significantly higher than from barbs, we better abort all of them
	if(!IsUsingStrategy(eStrategyBarbs) || (bStrategyFightAWar && !bWillingToAcceptRisk))
	{
		bool bFoundOneToDelete = true;
		while(bFoundOneToDelete)
		{
			bFoundOneToDelete = false;
			if(m_pPlayer->haveAIOperationOfType(AI_OPERATION_DESTROY_BARBARIAN_CAMP, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->SetToAbort(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
			if(!bWillingToAcceptRisk && m_pPlayer->haveAIOperationOfType(AI_OPERATION_ALLY_DEFENSE, &iOperationID))
			{
				m_pPlayer->getAIOperation(iOperationID)->SetToAbort(AI_ABORT_WAR_STATE_CHANGE);
				bFoundOneToDelete = true;
			}
		}
	}

	if(!m_pPlayer->IsAtWar())
	{
		// SEE WHAT OPERATIONS WE SHOULD ADD
		//
		// Operation vs. Barbarians
		//
		// If running the eradicate barbarian strategy, the threat is low (no higher than 1 major threat), we're not at war, /*and we have enough units*/, then launch a new operation.
		// Which one is based on whether or not we saw any barbarian camps

		if(IsUsingStrategy(eStrategyBarbs) && !bStrategyFightAWar && bWillingToAcceptRisk)
		{
			bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_DESTROY_BARBARIAN_CAMP, &iOperationID);
			if (!bHasOperationUnderway)
			{
				CvPlot* pTarget = OperationalAIHelpers::FindBestBarbCamp(m_pPlayer->GetID(),NULL);
				if(pTarget != NULL)
				{
					iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, NO_PLAYER, MUFORMATION_ANTI_BARBARIAN_TEAM, false, false, pTarget, pTarget, &iNumRequiredSlots);

					// Not willing to build units to get this off the ground
					if(iFilledSlots >= iNumRequiredSlots)
					{
						// We should have AI build for this
						m_pPlayer->addAIOperation(AI_OPERATION_DESTROY_BARBARIAN_CAMP);
					}
				}
			}
		}
	}
}
void CvMilitaryAI::SetupDefenses(PlayerTypes ePlayer)
{
	if(ePlayer == NO_PLAYER)
		return;

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
		strOpponentName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		// Strategy Info
		strTemp.Format("War Declared! ***** %s ****** Setting up defenses.", strOpponentName.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		LogMilitarySummaryMessage(strTemp);
	}

	int iOperationID;
	int iNumRequiredSlots;
	int iFilledSlots;

	CvCity* pMostThreatenedCity = m_pPlayer->GetThreatenedCityByRank();
	if (pMostThreatenedCity != NULL)
	{
		bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID, ePlayer);
		if (!bHasOperationUnderway)
		{
			iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, ePlayer, MUFORMATION_RAPID_RESPONSE_FORCE, false, false, pMostThreatenedCity->plot(), pMostThreatenedCity->plot(), &iNumRequiredSlots);
			if (iFilledSlots > 0)
			{
				m_pPlayer->addAIOperation(AI_OPERATION_RAPID_RESPONSE, ePlayer, pMostThreatenedCity->getArea(), pMostThreatenedCity, pMostThreatenedCity);
			}
		}
	}
	CvCity* pMostThreatenedCoastalCity = m_pPlayer->GetThreatenedCityByRank(0, true);
	if (pMostThreatenedCoastalCity != NULL)
	{
		CvPlot* pCoastalPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pMostThreatenedCoastalCity->plot());
		if (pCoastalPlot != NULL)
		{
			bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SUPERIORITY, &iOperationID, ePlayer, pMostThreatenedCoastalCity->plot());
			if (!bHasOperationUnderway)
			{
				iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, ePlayer, MUFORMATION_NAVAL_SQUADRON, true, m_pPlayer->CanCrossOcean(), pCoastalPlot, pCoastalPlot, &iNumRequiredSlots);
				if (iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= 0))
				{
					m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, ePlayer, pMostThreatenedCoastalCity->getArea(), pMostThreatenedCoastalCity, pMostThreatenedCoastalCity, m_pPlayer->CanCrossOcean());
				}
			}
		}
	}
}
/// Abort or start operations as appropriate given the current threats and war states
void CvMilitaryAI::CheckLandDefenses(PlayerTypes eEnemy, CvCity* pThreatenedCity)
{
	if(eEnemy == NO_PLAYER || !pThreatenedCity)
		return;
	
	WarStateTypes eWarState = GET_PLAYER(eEnemy).isMajorCiv() ? m_pPlayer->GetDiplomacyAI()->GetWarState(eEnemy) : WAR_STATE_OFFENSIVE;
	if (eWarState >= WAR_STATE_STALEMATE)
		return;

	int iOperationID;
	int iNumRequiredSlots;
	int iFilledSlots;
	int iNumUnitsWillingBuild = 2;

	if (eWarState == WAR_STATE_DEFENSIVE)
		m_pPlayer->StopAllLandOffensiveOperationsAgainstPlayer(eEnemy, true, AI_ABORT_WAR_STATE_CHANGE);
	// If we are really losing, let's pull back everywhere.	
	else if (eWarState == WAR_STATE_NEARLY_DEFEATED)
	{
		for(int iPlayerLoop2 = 0; iPlayerLoop2 < MAX_MAJOR_CIVS; iPlayerLoop2++)
		{
			PlayerTypes eLoopPlayer2 = (PlayerTypes) iPlayerLoop2;

			// Is this a player we have relations with?
			if(eLoopPlayer2 != m_pPlayer->GetID() && IsPlayerValid(eLoopPlayer2))
			{
				m_pPlayer->StopAllLandOffensiveOperationsAgainstPlayer(eLoopPlayer2, true, AI_ABORT_WAR_STATE_CHANGE);
			}
		}
	}

	//Let's make sure our base defenses are up.
	bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, &iOperationID, eEnemy);
	CvPlot* pStartPlot = OperationalAIHelpers::FindEnemiesNearPlot(m_pPlayer->GetID(), eEnemy, DOMAIN_LAND, true, pThreatenedCity->getArea(), pThreatenedCity->plot());
	if (!bHasOperationUnderway && pStartPlot != NULL && pStartPlot->getOwningCity() != NULL)
	{
		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MUFORMATION_RAPID_RESPONSE_FORCE, false, false, pStartPlot, pStartPlot, &iNumRequiredSlots);
		if (iFilledSlots > 0)
		{
			m_pPlayer->addAIOperation(AI_OPERATION_RAPID_RESPONSE, eEnemy, pStartPlot->getArea(), pStartPlot->getOwningCity(), pStartPlot->getOwningCity());
		}
	}

	bool bIsEnemyZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pThreatenedCity->plot());
	if (bIsEnemyZone || !m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_CLOSE_DEFENSE, &iOperationID, eEnemy, pThreatenedCity->plot()))
	{
		iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eEnemy, MUFORMATION_CLOSE_CITY_DEFENSE, false, false, pThreatenedCity->plot(), pThreatenedCity->plot(), &iNumRequiredSlots);
		if(iFilledSlots > 1 && (bIsEnemyZone || ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild)))
		{
			m_pPlayer->addAIOperation(AI_OPERATION_CITY_CLOSE_DEFENSE, eEnemy, pThreatenedCity->getArea(), pThreatenedCity, pThreatenedCity);
		}
	}
}
/// Abort or start operations as appropriate given the current threats and war states
void CvMilitaryAI::CheckSeaDefenses(PlayerTypes ePlayer, CvCity* pThreatenedCity)
{
	if(ePlayer == NO_PLAYER || !pThreatenedCity)
		return;

	WarStateTypes eWarState = GET_PLAYER(ePlayer).isMajorCiv() ? m_pPlayer->GetDiplomacyAI()->GetWarState(ePlayer) : WAR_STATE_OFFENSIVE;
	if (eWarState >= WAR_STATE_STALEMATE)
		return;

	if (eWarState ==  WAR_STATE_DEFENSIVE)
		m_pPlayer->StopAllSeaOffensiveOperationsAgainstPlayer(ePlayer, true, AI_ABORT_WAR_STATE_CHANGE);
	//if we are losing badly, pull back everywhere
	else if (eWarState == WAR_STATE_NEARLY_DEFEATED)
	{
		for(int iPlayerLoop2 = 0; iPlayerLoop2 < MAX_MAJOR_CIVS; iPlayerLoop2++)
		{
			PlayerTypes eLoopPlayer2 = (PlayerTypes) iPlayerLoop2;

			// Is this a player we have relations with?
			if(eLoopPlayer2 != m_pPlayer->GetID() && ePlayer != eLoopPlayer2 && IsPlayerValid(eLoopPlayer2))
			{
				m_pPlayer->StopAllSeaOffensiveOperationsAgainstPlayer(eLoopPlayer2, true, AI_ABORT_WAR_STATE_CHANGE);
			}
		}
	}

	int iOperationID;
	int iNumRequiredSlots;
	int iFilledSlots;
	int iNumUnitsWillingBuild = 0;	

	if (!pThreatenedCity->isCoastal())
		return;

	CvPlot* pCoastalPlot = MilitaryAIHelpers::GetCoastalPlotNearPlot(pThreatenedCity->plot());
	if(pCoastalPlot != NULL)
	{
		bool bIsEnemyZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pThreatenedCity->plot());
		bool bHasOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_SUPERIORITY, &iOperationID, ePlayer, pThreatenedCity->plot());
		if (!bHasOperationUnderway || bIsEnemyZone)
		{
			iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, ePlayer, MUFORMATION_NAVAL_SQUADRON, true, m_pPlayer->CanCrossOcean(), pCoastalPlot, pCoastalPlot, &iNumRequiredSlots);
			if (iFilledSlots > 1 && (bIsEnemyZone || ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild)))
			{
				m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, ePlayer, pCoastalPlot->getArea(), pThreatenedCity, pThreatenedCity, m_pPlayer->CanCrossOcean());
			}
		}
	}
}
void CvMilitaryAI::DoLandAttacks(PlayerTypes ePlayer)
{
	//Not perfect, as some operations are mixed, but it will keep us from sending everyone to slaughter all at once.
	int iReservesTotal = ((m_iNumLandUnits + m_iNumNavalUnits) - (m_iNumNavalUnitsInArmies + m_iNumLandUnitsInArmies));
	if (iReservesTotal >= m_iMandatoryReserveSize || m_pPlayer->GetNumOffensiveOperations(DOMAIN_LAND) <= 0)
	{
		int iOperationID;
		int iNumRequiredSlots;
		int iFilledSlots;
		int iNumUnitsWillingBuild = 2;
		WarStateTypes eWarState = GET_PLAYER(ePlayer).isMajorCiv() ? m_pPlayer->GetDiplomacyAI()->GetWarState(ePlayer) : WAR_STATE_OFFENSIVE;
		switch(eWarState)
		{
			// If we are dominant, shouldn't be running a defensive strategy
			case WAR_STATE_NEARLY_WON:
			case WAR_STATE_OFFENSIVE:
				m_pPlayer->StopAllLandDefensiveOperationsAgainstPlayer(ePlayer, AI_ABORT_WAR_STATE_CHANGE);
				break;
		}

		if (GET_PLAYER(ePlayer).isMinorCiv())
		{
			PlayerTypes eAlly = GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlly();
			if (eAlly != NO_PLAYER && GET_TEAM(GET_PLAYER(eAlly).getTeam()).isAtWar(m_pPlayer->getTeam()))
			{
				return;
			}
		}

		if(eWarState >= WAR_STATE_STALEMATE)
		{
			if(GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).isBarbarian())
			{
				CvMilitaryTarget targetLand = FindBestAttackTargetCached(AI_OPERATION_CITY_STATE_ATTACK, ePlayer);
				if(targetLand.m_pTargetCity && targetLand.m_pMusterCity && !targetLand.m_bNoLandPath)
				{
					bool bHasCSOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_STATE_ATTACK, &iOperationID, ePlayer, targetLand.m_pMusterCity->plot());
					if (!bHasCSOperationUnderway)
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, ePlayer, MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack(), targetLand.m_bAttackBySea, targetLand.m_bOcean, targetLand.m_pMusterCity->plot(), targetLand.m_pTargetCity->plot(), &iNumRequiredSlots);
						if (iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
						{
							m_pPlayer->addAIOperation(AI_OPERATION_CITY_STATE_ATTACK, ePlayer, targetLand.m_pMusterCity->getArea(), targetLand.m_pTargetCity, targetLand.m_pMusterCity);
							m_iNumLandUnitsInArmies += iFilledSlots;
						}
					}
				}
			}
			else
			{

				bool bHasPillageUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY, &iOperationID, ePlayer);
				CvMilitaryTarget targetLand = FindBestAttackTargetCached(AI_OPERATION_CITY_BASIC_ATTACK, ePlayer);
				if (targetLand.m_pTargetCity && targetLand.m_pMusterCity && !targetLand.m_bNoLandPath)
				{
					bool bHasBasicOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_CITY_BASIC_ATTACK, &iOperationID, ePlayer, targetLand.m_pMusterCity->plot());
					if (!bHasBasicOperationUnderway)
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, ePlayer, MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack(), targetLand.m_bAttackBySea, targetLand.m_bOcean, targetLand.m_pMusterCity->plot(), targetLand.m_pTargetCity->plot(), &iNumRequiredSlots);
						if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
						{
							m_pPlayer->addAIOperation(AI_OPERATION_CITY_BASIC_ATTACK, ePlayer, targetLand.m_pMusterCity->getArea(), targetLand.m_pTargetCity, targetLand.m_pMusterCity);
							m_iNumLandUnitsInArmies += iFilledSlots;
						}
					}
				}
				if (eWarState >= WAR_STATE_CALM && !bHasPillageUnderway)
				{
					RequestPillageAttack(ePlayer);
				}
			}
		}
	}
}
void CvMilitaryAI::DoSeaAttacks(PlayerTypes ePlayer)
{
	//Not perfect, as some operations are mixed, but it will keep us from sending everyone to slaughter all at once.
	int iReservesTotal = ((m_iNumLandUnits + m_iNumNavalUnits) - (m_iNumNavalUnitsInArmies + m_iNumLandUnitsInArmies));
	if (iReservesTotal >= m_iMandatoryReserveSize || m_pPlayer->GetNumOffensiveOperations(DOMAIN_SEA) <= 0)
	{
		int iNumRequiredSlots;
		int iFilledSlots;
		int iReservesUsed;
		int iNumUnitsWillingBuild = 2;
		
		WarStateTypes eWarState = GET_PLAYER(ePlayer).isMajorCiv() ? m_pPlayer->GetDiplomacyAI()->GetWarState(ePlayer) : WAR_STATE_OFFENSIVE;
		switch(eWarState)
		{
			// If we are dominant, shouldn't be running a defensive strategy
			case WAR_STATE_NEARLY_WON:
			case WAR_STATE_OFFENSIVE:
				m_pPlayer->StopAllSeaDefensiveOperationsAgainstPlayer(ePlayer, AI_ABORT_WAR_STATE_CHANGE);
				break;
		}

		if (GET_PLAYER(ePlayer).isMinorCiv())
		{
			PlayerTypes eAlly = GET_PLAYER(ePlayer).GetMinorCivAI()->GetAlly();
			if (eAlly != NO_PLAYER && GET_TEAM(GET_PLAYER(eAlly).getTeam()).isAtWar(m_pPlayer->getTeam()))
			{
				return;
			}
		}
		int iOperationID;
		if(eWarState >= WAR_STATE_STALEMATE)
		{	
			if(GET_PLAYER(ePlayer).isMinorCiv() || GET_PLAYER(ePlayer).isBarbarian())
			{			
				CvMilitaryTarget targetSea = FindBestAttackTargetCached(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, ePlayer);
				if(targetSea.m_pTargetCity && targetSea.m_pMusterCity && targetSea.m_bAttackBySea)
				{
					bool bHasPureOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, &iOperationID, ePlayer, targetSea.m_pMusterCity->plot());
					if (!bHasPureOperationUnderway)
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, ePlayer, MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack(), targetSea.m_bAttackBySea, targetSea.m_bOcean, targetSea.m_pMusterCity->plot(), targetSea.m_pTargetCity->plot(), &iNumRequiredSlots);
						if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
						{
							m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, ePlayer, targetSea.m_pMusterCity->getArea(), targetSea.m_pTargetCity, targetSea.m_pMusterCity, targetSea.m_bOcean);
							m_iNumNavalUnitsInArmies += iFilledSlots;
						}
					}
				}
				targetSea = FindBestAttackTargetCached(AI_OPERATION_NAVAL_INVASION_CITY_STATE, ePlayer);
				if(targetSea.m_pTargetCity && targetSea.m_pMusterCity && targetSea.m_bAttackBySea)
				{
					bool bHasMixedCSOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_INVASION_CITY_STATE, &iOperationID, ePlayer, targetSea.m_pMusterCity->plot());
					if (!bHasMixedCSOperationUnderway)
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, ePlayer, MUFORMATION_CITY_STATE_INVASION, targetSea.m_bAttackBySea, targetSea.m_bOcean, targetSea.m_pMusterCity->plot(), targetSea.m_pTargetCity->plot(), &iNumRequiredSlots, &iReservesUsed);
						if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
						{
							m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_INVASION_CITY_STATE, ePlayer, targetSea.m_pMusterCity->getArea(), targetSea.m_pTargetCity, targetSea.m_pMusterCity, targetSea.m_bOcean);
							m_iNumNavalUnitsInArmies += iFilledSlots;
						}
					}
				}
			}
			else
			{
				CvMilitaryTarget targetSea = FindBestAttackTargetCached(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, ePlayer);
				if(targetSea.m_pTargetCity && targetSea.m_pMusterCity && targetSea.m_bAttackBySea)
				{
					bool bHasPureOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, &iOperationID, ePlayer, targetSea.m_pMusterCity->plot());
					if (!bHasPureOperationUnderway)
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, ePlayer, MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack(), targetSea.m_bAttackBySea, targetSea.m_bOcean, targetSea.m_pMusterCity->plot(), targetSea.m_pTargetCity->plot(), &iNumRequiredSlots);
						if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
						{
							m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, ePlayer, targetSea.m_pMusterCity->getArea(), targetSea.m_pTargetCity, targetSea.m_pMusterCity, targetSea.m_bOcean);
							m_iNumNavalUnitsInArmies += iFilledSlots;
						}
					}
				}
				targetSea = FindBestAttackTargetCached(AI_OPERATION_NAVAL_INVASION, ePlayer);
				if(targetSea.m_pTargetCity && targetSea.m_pMusterCity && targetSea.m_bAttackBySea)
				{
					bool bHasMixedOperationUnderway = m_pPlayer->haveAIOperationOfType(AI_OPERATION_NAVAL_INVASION, &iOperationID, ePlayer, targetSea.m_pMusterCity->plot());
					if (!bHasMixedOperationUnderway)
					{
						iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, ePlayer, MilitaryAIHelpers::GetCurrentBestFormationTypeForNavalAttack(), targetSea.m_bAttackBySea, targetSea.m_bOcean, targetSea.m_pMusterCity->plot(), targetSea.m_pTargetCity->plot(), &iNumRequiredSlots);
						if(iFilledSlots > 0 && ((iNumRequiredSlots - iFilledSlots) <= iNumUnitsWillingBuild))
						{
							m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_INVASION, ePlayer, targetSea.m_pMusterCity->getArea(), targetSea.m_pTargetCity, targetSea.m_pMusterCity, targetSea.m_bOcean);
							m_iNumNavalUnitsInArmies += iFilledSlots;
						}
					}
				}
			}
		}
	}
}
int CvMilitaryAI::GetEnemyLandValue(PlayerTypes ePlayer, CvMilitaryTarget& globaltarget, int iGlobalTargetScore)
{
	int iValue = GET_PLAYER(ePlayer).GetMilitaryMight();
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		//Bonus to Genghis, eh?
		if(m_pPlayer->GetPlayerTraits()->IsBullyAnnex())
		{
			iValue *= 5;
		}
		else
		{
			iValue /= 2;
		}
	}
	else
	{
		int iWinningScore;
		CvMilitaryTarget thistarget = GetPlayer()->GetMilitaryAI()->FindBestAttackTargetCached(AI_OPERATION_CITY_BASIC_ATTACK, ePlayer, &iWinningScore);
		if (thistarget.m_pTargetCity != NULL && thistarget.m_pMusterCity != NULL && !thistarget.m_bNoLandPath)
		{
			iValue += iWinningScore;

			if (thistarget == globaltarget)
			{
				iValue *= 5;
			}
			else if (iGlobalTargetScore > 3 * iWinningScore)
				return 0;

			if (GetPlayer()->GetDiplomacyAI()->GetBiggestCompetitor() == ePlayer)
			{
				iValue *= 5;
			}
		}
		else
			return 0;
	}
	return iValue;
}
int CvMilitaryAI::GetEnemySeaValue(PlayerTypes ePlayer, CvMilitaryTarget& globaltarget, int iGlobalTargetScore)
{
	int iValue = GET_PLAYER(ePlayer).GetMilitaryMight();
	if(GET_PLAYER(ePlayer).isMinorCiv())
	{
		//Bonus to Genghis, eh?
		if(m_pPlayer->GetPlayerTraits()->IsBullyAnnex())
		{
			iValue *= 2;
		}
		else
		{
			iValue /= 2;
		}
	}
	else
	{
		int iWinningScore;
		CvMilitaryTarget thistarget = GetPlayer()->GetMilitaryAI()->FindBestAttackTargetCached(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, ePlayer, &iWinningScore);
		if (thistarget.m_pTargetCity != NULL && thistarget.m_pMusterCity != NULL && thistarget.m_bAttackBySea)
		{
			iValue += iWinningScore;

			//Best global?
			if (globaltarget == thistarget)
			{
				iValue *= 5;
			}
			else if (iGlobalTargetScore > 3 * iWinningScore)
				return 0;

			if (GetPlayer()->GetDiplomacyAI()->GetBiggestCompetitor() == ePlayer)
			{
				iValue *= 5;
			}
		}
		else
			return 0;

	}
	return iValue;
}
/// Abort or start operations as appropriate given the current threats and war states
void CvMilitaryAI::UpdateOperations()
{
	//only major players set up operations
	if(!m_pPlayer->isMajorCiv())
		return;

	AI_PERF_FORMAT("Military-AI-perf.csv", ("UpdateOperations, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	//check the two most threatened cities for defense
	CvCity* pThreatenedCityA = m_pPlayer->GetThreatenedCityByRank(0);
	CvCity* pThreatenedCityB = m_pPlayer->GetThreatenedCityByRank(1);

	//cached distances might be invalid because of new/conquered cities or changed alliances ...
	ResetDistanceCaches();

	///////////////////////////////
	//////////////////////
	// Operation vs. Barbs
	//////////////////////
	//////////////////////////////
	DoBarbs();

	///////////////////////////////
	//////////////////////
	// Operation vs. Other Civs
	//////////////////////
	//////////////////////////////

	int iBestValue;
	CvMilitaryTarget bestTargetLand = GetPlayer()->GetMilitaryAI()->FindBestAttackTargetGlobal(AI_OPERATION_CITY_BASIC_ATTACK, &iBestValue, false);

	vector<OptionWithScore<PlayerTypes>> veLandThreatWeights;
	// Are any of our strategies inappropriate given the type of war we are fighting
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
		if(eLoopPlayer != m_pPlayer->GetID() && IsPlayerValid(eLoopPlayer))
		{
			// If we cannot declare war on this player, abort all offensive operations related to him
			if(!m_pPlayer->IsAtWarWith(eLoopPlayer) && !GET_TEAM(m_pPlayer->getTeam()).canDeclareWar(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				m_pPlayer->StopAllLandOffensiveOperationsAgainstPlayer(eLoopPlayer,true,AI_ABORT_DIPLO_OPINION_CHANGE);
				m_pPlayer->StopAllSeaOffensiveOperationsAgainstPlayer(eLoopPlayer,true,AI_ABORT_DIPLO_OPINION_CHANGE);
			}

			// If the other player cannot declare on us, abort defensive ops too
			if(GET_TEAM(m_pPlayer->getTeam()).isForcePeace(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				m_pPlayer->StopAllLandDefensiveOperationsAgainstPlayer(eLoopPlayer,AI_ABORT_DIPLO_OPINION_CHANGE);
				m_pPlayer->StopAllSeaDefensiveOperationsAgainstPlayer(eLoopPlayer,AI_ABORT_DIPLO_OPINION_CHANGE);
			}

			// if we're not at war with this player
			if(!m_pPlayer->IsAtWarWith(eLoopPlayer))
				continue;

			veLandThreatWeights.push_back( OptionWithScore<PlayerTypes>(eLoopPlayer, GetEnemyLandValue(eLoopPlayer, bestTargetLand, iBestValue)) );
		}
	}

	std::sort(veLandThreatWeights.begin(), veLandThreatWeights.end());
	for(size_t iThreatCivs = 0; iThreatCivs < veLandThreatWeights.size(); iThreatCivs++)
	{
		PlayerTypes eLoopPlayer = veLandThreatWeights[iThreatCivs].option;
		//Defense check.
		if(!GET_PLAYER(eLoopPlayer).isMinorCiv() && !GET_PLAYER(eLoopPlayer).isBarbarian())
		{
			if(pThreatenedCityA == NULL)
				m_pPlayer->StopAllLandDefensiveOperationsAgainstPlayer(eLoopPlayer, AI_ABORT_WAR_STATE_CHANGE);

					CheckLandDefenses(eLoopPlayer,pThreatenedCityA);
					CheckLandDefenses(eLoopPlayer,pThreatenedCityB);
				}

				DoNuke(eLoopPlayer);

		if(veLandThreatWeights[iThreatCivs].score > 0)
			DoLandAttacks(eLoopPlayer);
	}

	//same for naval  ...
	CvCity* pThreatenedCoastalCityA = m_pPlayer->GetThreatenedCityByRank(0, true);
	CvCity* pThreatenedCoastalCityB = m_pPlayer->GetThreatenedCityByRank(1, true);

	CvMilitaryTarget bestTargetSea = GetPlayer()->GetMilitaryAI()->FindBestAttackTargetGlobal(AI_OPERATION_NAVAL_ONLY_CITY_ATTACK, &iBestValue, false);

	vector<OptionWithScore<PlayerTypes>> veSeaThreatWeights;
	// Are any of our strategies inappropriate given the type of war we are fighting
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
		if(eLoopPlayer != m_pPlayer->GetID() && IsPlayerValid(eLoopPlayer))
		{
			// if we're not at war with this player
			if (!m_pPlayer->IsAtWarWith(eLoopPlayer))
				continue;

			veSeaThreatWeights.push_back( OptionWithScore<PlayerTypes>(eLoopPlayer, GetEnemySeaValue(eLoopPlayer, bestTargetSea, iBestValue)) );
		}
	}

	std::sort(veSeaThreatWeights.begin(), veSeaThreatWeights.end());
	for(int iThreatCivs = 0; iThreatCivs < (int)veSeaThreatWeights.size(); iThreatCivs++)
	{
		PlayerTypes eLoopPlayer = veSeaThreatWeights[iThreatCivs].option;
		//Defense check.
		if(!GET_PLAYER(eLoopPlayer).isMinorCiv() && !GET_PLAYER(eLoopPlayer).isBarbarian())
		{
			if (pThreatenedCoastalCityA == NULL)
				m_pPlayer->StopAllLandDefensiveOperationsAgainstPlayer(eLoopPlayer, AI_ABORT_WAR_STATE_CHANGE);

			CheckSeaDefenses(eLoopPlayer, pThreatenedCoastalCityA);
			CheckSeaDefenses(eLoopPlayer, pThreatenedCoastalCityB);
		}

		if(veSeaThreatWeights[iThreatCivs].score > 0)
			DoSeaAttacks(eLoopPlayer);
	}
}

/// Spend money on units/buildings for military contingencies
//  NOTE: The defensive side of this is done in dominance zone processing in the Tactical AI; this is spending to speed operations
void CvMilitaryAI::MakeEmergencyPurchases()
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("MakeEmergencyPurchases, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	// Are we winning all the wars we are in?
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(!IsUsingStrategy(eStrategyAtWar) || m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		// Is there an operation waiting for one more unit?
		CvAIOperation* nextOp = m_pPlayer->getFirstAIOperation();
		while(nextOp != NULL)
		{
			// Can we buy a unit to fill that slot?
			if(nextOp->BuyFinalUnit())
				break;

			nextOp = m_pPlayer->getNextAIOperation();
		}
	}
}

void CvMilitaryAI::RequestImprovements()
{

}

/// Delete older units no longer needed by military AI
void CvMilitaryAI::DisbandObsoleteUnits(int iMaxUnits)
{
	AI_PERF_FORMAT("Military-AI-perf.csv", ("DisbandObsoleteUnits, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), m_pPlayer->getCivilizationShortDescription()) );

	bool bInDeficit = false;
	bool bConquestGrandStrategy = false;

	if (m_pPlayer->isBarbarian())
		return;

	// Don't do this if at war
	if(GetNumberCivsAtWarWith(false) > 0)
	{
		if (m_pPlayer->GetDiplomacyAI()->GetStateAllWars() != STATE_ALL_WARS_WINNING)
			return;
	}

	if (m_pPlayer->isMinorCiv())
	{
		if (m_pPlayer->GetMinorCivAI()->IsRecentlyBulliedByAnyMajor())
			return;

		if (m_pPlayer->GetMinorCivAI()->GetNumThreateningBarbarians() > 0)
			return;
	}
	else
	{
		// Are we running at a deficit?
		EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
		bInDeficit = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);
		// Are we running anything other than the Conquest Grand Strategy?
		AIGrandStrategyTypes eConquestGrandStrategy = (AIGrandStrategyTypes)GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
		if (eConquestGrandStrategy != NO_AIGRANDSTRATEGY)
		{
			if (m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eConquestGrandStrategy)
			{
				bConquestGrandStrategy = true;
			}
		}
	}

	// do we have way too many units?
	int iMaxExcessUnits = bConquestGrandStrategy ? 5 : 3;
	bool bOverSupplyCap = (m_pPlayer->GetNumUnitsOutOfSupply() > iMaxExcessUnits);

	//nothing to do
	if (!bInDeficit && !bOverSupplyCap)
		return;

	for (int i=0; i<iMaxUnits; i++)
	{
		CvUnit* pNavalUnit = NULL;
		CvUnit* pLandUnit = NULL;
		int iNavalScore = MAX_INT;
		int iLandScore = MAX_INT;

		// Look for obsolete land units if in deficit or have sufficient units
		if (bInDeficit || bOverSupplyCap)
		{
			//we may be in deficit, but we're not forced to disband yet
			pLandUnit = FindBestUnitToScrap(true /*bLand*/, false /*bDeficitForcedDisband*/, iLandScore);
		}

		// Look for obsolete naval units if in deficit or have sufficient units
		if (bInDeficit || (m_eNavalDefenseState <= DEFENSE_STATE_NEUTRAL && !bConquestGrandStrategy))
		{
			//we may be in deficit, but we're not forced to disband yet
			pNavalUnit = FindBestUnitToScrap(false/*bNaval*/, false /*bDeficitForcedDisband*/, iNavalScore);
		}

		if (iLandScore < MAX_INT && (m_eLandDefenseState <= m_eNavalDefenseState || iLandScore <= iNavalScore))
		{
			if (pLandUnit && !pLandUnit->isDelayedDeath())
			{
				bool bGifted = false;
				// Don't do this if we're a minor civ
				if (!m_pPlayer->isMinorCiv())
				{
					PlayerTypes eMinor = m_pPlayer->GetBestGiftTarget();
					if (eMinor != NO_PLAYER)
					{
						GET_PLAYER(eMinor).AddIncomingUnit(m_pPlayer->GetID(), pLandUnit);
						bGifted = true;
					}
				}
				if (bGifted)
				{
					LogGiftUnit(pLandUnit, bInDeficit, bOverSupplyCap);
				}
				else
				{
					if (pLandUnit->canScrap())
						pLandUnit->scrap();
					else
						pLandUnit->kill(true);

					LogScrapUnit(pLandUnit, bInDeficit, bOverSupplyCap);
				}
			}
		}
		else if (iNavalScore < MAX_INT)
		{
			if (pNavalUnit && !pNavalUnit->isDelayedDeath())
			{
				bool bGifted = false;
				// Don't do this if we're a minor civ
				if (!m_pPlayer->isMinorCiv())
				{
					PlayerTypes eMinor = m_pPlayer->GetBestGiftTarget();
					if (eMinor != NO_PLAYER)
					{
						GET_PLAYER(eMinor).AddIncomingUnit(m_pPlayer->GetID(), pNavalUnit);
						bGifted = true;
					}
				}
				if (bGifted)
				{
					LogGiftUnit(pNavalUnit, bInDeficit, bOverSupplyCap);
				}
				else
				{
					if (pNavalUnit->canScrap())
						pNavalUnit->scrap();
					else
						pNavalUnit->kill(true);
					LogScrapUnit(pNavalUnit, bInDeficit, bOverSupplyCap);
				}
			}
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
#if defined(MOD_BALANCE_CORE)
	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, NO_PLAYER, eFormation, bRequiresNavalMoves, false, NULL, NULL, &iNumRequiredSlots, &iLandReservesUsed);
#else
	iFilledSlots = MilitaryAIHelpers::NumberOfFillableSlots(m_pPlayer, eFormation, bRequiresNavalMoves, &iNumRequiredSlots, &iLandReservesUsed);
#endif
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
CvUnit* CvMilitaryAI::FindBestUnitToScrap(bool bLand, bool bDeficitForcedDisband, int& iReturnedScore)
{
	CvUnit* pLoopUnit = NULL;
	int iUnitLoop;
	CvUnit* pBestUnit = NULL;
	int iScore;
	int iBestScore = MAX_INT;

#if defined(MOD_BALANCE_CORE)

	for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		//needed later
		CvUnitEntry& pUnitInfo = pLoopUnit->getUnitInfo();

		bool bIsUseless = false;
		bool bStillNeeded = false;

		if(!pLoopUnit->IsCombatUnit())
			continue;

		if (!pLoopUnit->canScrap())
			continue;

		//Failsafe to keep AI from deleting advanced start settlers
		//Probably useless because of the combat unit check above
		if (m_pPlayer->GetNumCitiesFounded() < 3)
			if (pUnitInfo.IsFound() || pUnitInfo.IsFoundAbroad())
				continue;

		if(bLand && pLoopUnit->getDomainType() != DOMAIN_LAND)
			continue;

		if(!bLand && pLoopUnit->getDomainType() != DOMAIN_SEA)
			continue;

		// Following checks are for the case where the AI is trying to decide if it is a good idea to disband this unit (as opposed to when the game is FORCING the player to disband one)
		if(!bDeficitForcedDisband)
		{
			if (bLand && m_eLandDefenseState == DEFENSE_STATE_CRITICAL)
				continue;
			else if(!bLand && m_eNavalDefenseState == DEFENSE_STATE_CRITICAL)
				continue;

			// Is it in an army?
			if(pLoopUnit->getArmyID() != -1 || TacticalAIHelpers::GetFirstTargetInRange(pLoopUnit)!=NULL)
				continue;

			// Is this a ship on a water body without enemies?
			if (!bLand && pLoopUnit->plot()->isWater())
			{
				CvArea* pWaterBody = GC.getMap().getArea( pLoopUnit->plot()->getArea() );
				if (pWaterBody)
				{
					int iForeignCities = pWaterBody->getNumCities() - pWaterBody->getCitiesPerPlayer(m_pPlayer->GetID());
					int iForeignUnits = pWaterBody->getNumUnits() - pWaterBody->getUnitsPerPlayer(m_pPlayer->GetID());

					if (iForeignCities == 0 && iForeignUnits == 0)
						bIsUseless = true;
				}
			}

			// Is this a unit who has an obsolete tech that I have researched?
			if ((TechTypes)pUnitInfo.GetObsoleteTech() != NO_TECH && GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetObsoleteTech())))
				bIsUseless = true;

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
#if defined(MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
						int iNumResourceTotalNeeded = 0;
						if (MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
						{
							iNumResourceTotalNeeded = pUpgradeUnitInfo->GetResourceQuantityTotal(eResource);
						}

						// Minor issue: this only works correctly if a unit has only one required resource ...
						if(iNumResourceNeeded > 0 || (MOD_UNITS_RESOURCE_QUANTITY_TOTALS && iNumResourceTotalNeeded > 0))
#else
						if(iNumResourceNeeded > 0)
#endif
						{
							if(m_pPlayer->getNumResourceTotal(eResource) > 0)
							{
								// We'll wait and try to upgrade this one, our unit count isn't that bad
								if (bLand && m_eLandDefenseState > DEFENSE_STATE_NEUTRAL)
								{
									bStillNeeded = true;
									break;
								}
								else if (!bLand && m_eNavalDefenseState > DEFENSE_STATE_NEUTRAL)
								{
									bStillNeeded = true;
									break;
								}
							}
						}
					}
				}
			}
		}

		// Can I scrap this unit?
		if( (!bStillNeeded || bIsUseless))
		{
			iScore = pLoopUnit->GetPower()*pLoopUnit->getUnitInfo().GetProductionCost();

			//prefer units which are at home, meaning we get some money back
			if (pLoopUnit->canScrap())
				iScore = (iScore * 8) / 10;

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
			if(pLoopUnit->getArmyID() != -1)
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
		eFormation = MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack();
	}
	UnitAITypes eUnitAIType = MilitaryAIHelpers::FirstSlotCityCanFill(m_pPlayer, eFormation, (m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION), pCity->isCoastal(), false /*bSecondaryUnit*/);
#if defined(MOD_BALANCE_CORE)
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType, true);
#else	
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType);
#endif
	if(eType == NO_UNIT)
	{
		eUnitAIType = MilitaryAIHelpers::FirstSlotCityCanFill(m_pPlayer, eFormation, (m_eArmyTypeBeingBuilt == ARMY_TYPE_NAVAL_INVASION), pCity->isCoastal(), true /*bSecondaryUnit*/);
#if defined(MOD_BALANCE_CORE)
		eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType, true);
#else			
		eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitAIType);
#endif
	}
	return eType;
}

/// Assess nearby enemy air assets
#if defined(MOD_AI_SMART_AIR_TACTICS)
// Add half of unit range to the calculations.
int CvMilitaryAI::GetNumEnemyAirUnitsInRange(CvPlot* pCenterPlot, int iRange, bool bCountFighters, bool bCountBombers) const
#else
int CvMilitaryAI::GetNumEnemyAirUnitsInRange(CvPlot* pCenterPlot, int /*iRange*/, bool bCountFighters, bool bCountBombers) const
#endif
{
	int iRtnValue = 0;

	// Loop through all players' Units (that we're at war with) to see if they can intercept
	const std::vector<PlayerTypes>& vEnemies = m_pPlayer->GetPlayersAtWarWith();

	for(size_t iI = 0; iI < vEnemies.size(); iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(vEnemies[iI]);

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
											iCountFighters+=2;
										}
										else if (!pUnit->isSuicide() && !pUnit->isCargo())
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
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName,true), FILogFile::kDontTimeStamp);

		// Very first update (to write header row?)
		if(GC.getGame().getGameTurn() == 1 && m_pPlayer->GetID() == 0)
		{
			strTemp.Format("Turn, Player, Cities, Settlers, CivThreat, BarbThreat, LandUnits, LandArmySize, RecLandSize, LandReserve, NavalUnits, NavalArmySize, RecNavySize, TotalUnits, MilitaryUnits, SupplyLimit, NoSupplyUnits, OutOfSupply, WarCount, MostEndangeredCity, Danger");
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
		strTemp.Format("%d, %d, %d, %d, %d, %d, %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecommendedMilitarySize, m_iMandatoryReserveSize, m_iNumNavalUnits, m_iNumNavalUnitsInArmies, m_iRecNavySize);
		strOutBuf += strTemp;

		// Unit supply
		strTemp.Format("%d, %d, %d, %d, %d, %d, ", 
			m_pPlayer->getNumUnits(), m_pPlayer->getNumMilitaryUnits(), m_pPlayer->GetNumUnitsSupplied(), m_pPlayer->getNumUnitsSupplyFree(), 
			m_pPlayer->GetNumUnitsOutOfSupply(), m_pPlayer->GetPlayersAtWarWith().size());
		strOutBuf += strTemp;

		// Most threatened city
		pCity = m_pPlayer->GetThreatenedCityByRank();
		if(pCity != NULL)
		{
			cityName = pCity->getName();
			cityName.Replace(' ', '_'); //easier spreadsheet import
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
		/*
		// Loop through our units
		CvUnit* pLoopUnit;
		int iLoop;
		for(pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
		{
			// Never want immobile/dead units, explorers, ones that have already moved or automated human units
			if(pLoopUnit->isDelayedDeath() || pLoopUnit->AI_getUnitAIType() == UNITAI_UNKNOWN ||  pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE || !pLoopUnit->canMove() || pLoopUnit->isHuman())
			{
				continue;
			}

			// Now down to land and sea units ... in these groups our unit must have a base combat strength ... or be a great general/admiral
			else if(!pLoopUnit->IsCombatUnit() && !(pLoopUnit->IsGreatGeneral() || pLoopUnit->IsGreatAdmiral() || pLoopUnit->IsCityAttackSupport()))
			{
				continue;
			}
			// No units finishing up operations
			else if(pLoopUnit->IsRecentlyDeployedFromOperation())
			{
				continue;
			}

			int iArmyID = pLoopUnit->getArmyID();
			int iDist = plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), iCapitalX, iCapitalY);
			strTemp.Format("%s, Damage=%d, ArmyID=%d, X=%d, Y=%d, DistFromCap=%d", pLoopUnit->getName().GetCString(), pLoopUnit->getDamage(), iArmyID, pLoopUnit->getX(), pLoopUnit->getY(), iDist);
			strOutBuf = strBaseString + strTemp;

			pLog->Msg(strOutBuf);
		}
		*/
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
void CvMilitaryAI::LogScrapUnit(CvUnit* pUnit, bool bDeficit, bool bSupply)
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
		if(bSupply)
		{
			strOutBuf += "OVER SUPPLY, ";
		}
		else
		{
			strOutBuf += "Supply ok, ";
		}
		if(pUnit->getDomainType() == DOMAIN_LAND)
		{
			strTemp.Format("Num Land Units: %d, In Armies %d, Rec Size: %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecommendedMilitarySize);
		}
		else
		{
			strTemp.Format("Num Naval Units: %d, In Armies %d, Rec: %d ", m_iNumNavalUnits, m_iNumNavalUnitsInArmies, m_iRecNavySize);
		}
		strOutBuf += strTemp;
		pLog->Msg(strOutBuf);
	}
}
/// Log that a unit is being gifted
void CvMilitaryAI::LogGiftUnit(CvUnit* pUnit, bool bDeficit, bool bSupply)
{
	if (GC.getLogging() && GC.getAILogging())
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

		strTemp.Format("Gifting unit %s to City-State, X: %d, Y: %d, ", pUnit->getUnitInfo().GetDescription(), pUnit->getX(), pUnit->getY());
		strOutBuf += strTemp;
		if (bDeficit)
		{
			strOutBuf += "IN DEFICIT, ";
		}
		else
		{
			strOutBuf += "Finances ok, ";
		}
		if (bSupply)
		{
			strOutBuf += "OVER SUPPLY, ";
		}
		else
		{
			strOutBuf += "Supply ok, ";
		}
		if (pUnit->getDomainType() == DOMAIN_LAND)
		{
			strTemp.Format("Num Land Units: %d, In Armies %d, Rec Size: %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecommendedMilitarySize);
		}
		else
		{
			strTemp.Format("Num Naval Units: %d, In Armies %d, Rec: %d ", m_iNumNavalUnits, m_iNumNavalUnitsInArmies, m_iRecNavySize);
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

//Gets the type of war the player is, overall, facing (used to decide production). 1 is land, 2 is sea (thanks, Paul Revere).
WarTypes CvMilitaryAI::GetWarType(PlayerTypes ePlayer)
{
	int iLand = 0;
	int iSea = 0;
	//No player? Let's get majority.
	if (ePlayer == NO_PLAYER)
	{
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
		{
			PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
			if (eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive() && eLoopPlayer != m_pPlayer->GetID() && !GET_PLAYER(eLoopPlayer).isMinorCiv())
			{
				int iWar = GetWarType(eLoopPlayer);
				if (iWar == WARTYPE_LAND)
				{
					iLand++;
				}
				else if (iWar == WARTYPE_SEA)
				{
					iSea++;
				}
			}
		}
		
		return (iLand >= iSea) ? WARTYPE_LAND : WARTYPE_SEA;
	}

	return (WarTypes)m_aiWarFocus[ePlayer];
}

void CvMilitaryAI::UpdateWarType()
{
	int iEnemyWater = 0;
	int iEnemyLand = 0;
	int iEnemyWaterCities = 0;
	int iEnemyLandCities = 0;
	int iLoop;

	int iFriendlyLand = 0;
	int iFriendlySea = 0;
	int iFriendlyLandCities = 0;
	int iFriendlySeaCities = 0;

	for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if (pLoopUnit != NULL && pLoopUnit->IsCombatUnit())
		{
			if (pLoopUnit->getDomainType() == DOMAIN_SEA)
			{
				if (pLoopUnit->isRanged())
				{
					iFriendlySea += pLoopUnit->GetBaseRangedCombatStrength();
				}
				else
				{
					iFriendlySea += pLoopUnit->GetBaseCombatStrength();
				}
			}
			else if (pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				if (pLoopUnit->isRanged())
				{
					iFriendlyLand += pLoopUnit->GetBaseRangedCombatStrength();
				}
				else
				{
					iFriendlyLand += pLoopUnit->GetBaseCombatStrength();
				}
			}
		}
	}

	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity != NULL)
		{
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;
				if (eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive() && eLoopPlayer != m_pPlayer->GetID() && !GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					if (GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isAtWar(m_pPlayer->getTeam()) || m_pPlayer->GetDiplomacyAI()->IsWantsSneakAttack(eLoopPlayer))
					{
						if (pLoopCity->isCoastal())
						{
							iFriendlySeaCities += 50;
						}
						else
						{
							iFriendlyLandCities += 50;
						}
						if (pLoopCity->IsInDanger(eLoopPlayer))
						{
							if (pLoopCity->isCoastal())
							{
								iFriendlySeaCities += 50;
							}
							else
							{
								iFriendlyLandCities += 50;
							}
						}
					}
				}
			}
		}
	}

	//And now theirs...
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{	
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(eLoopPlayer != NO_PLAYER && GET_PLAYER(eLoopPlayer).isAlive() && eLoopPlayer != m_pPlayer->GetID() && !GET_PLAYER(eLoopPlayer).isMinorCiv())
		{
			if(GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).isAtWar(m_pPlayer->getTeam()) || m_pPlayer->GetDiplomacyAI()->IsWantsSneakAttack(eLoopPlayer))
			{
				for(CvUnit* pLoopUnit = GET_PLAYER(eLoopPlayer).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(eLoopPlayer).nextUnit(&iLoop))
				{
					if (pLoopUnit != NULL && pLoopUnit->IsCombatUnit())
					{
						if(pLoopUnit->getDomainType() == DOMAIN_SEA)
						{
							if (pLoopUnit->plot()->isVisible(m_pPlayer->getTeam()))
							{
								if (pLoopUnit->isRanged())
								{
									iEnemyWater += pLoopUnit->GetBaseRangedCombatStrength() * 2;
								}
								else
								{
									iEnemyWater += pLoopUnit->GetBaseCombatStrength() * 2;
								}
							}
							else
							{
								if (pLoopUnit->isRanged())
								{
									iEnemyWater += pLoopUnit->GetBaseRangedCombatStrength();
								}
								else
								{
									iEnemyWater += pLoopUnit->GetBaseCombatStrength();
								}
							}				
						}
						else if (pLoopUnit->getDomainType() == DOMAIN_LAND)
						{
							if (pLoopUnit->plot()->isVisible(m_pPlayer->getTeam()))
							{
								if (pLoopUnit->isRanged())
								{
									iEnemyLand += pLoopUnit->GetBaseRangedCombatStrength() * 2;
								}
								else
								{
									iEnemyLand += pLoopUnit->GetBaseCombatStrength() * 2;
								}
							}
							else
							{
								if (pLoopUnit->isRanged())
								{
									iEnemyLand += pLoopUnit->GetBaseRangedCombatStrength();
								}
								else
								{
									iEnemyLand += pLoopUnit->GetBaseCombatStrength();
								}
							}
						}
					}
				}
				CvCity* pLoopCity;
				int iLoopCity = 0;
				for(pLoopCity = GET_PLAYER(eLoopPlayer).firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GET_PLAYER(eLoopPlayer).nextCity(&iLoopCity))
				{
					if(pLoopCity != NULL)
					{
						if (pLoopCity->IsInDanger(m_pPlayer->GetID()))
						{
							if (pLoopCity->isCoastal())
							{
								iFriendlySeaCities += 50;
							}
							else
							{
								iFriendlyLandCities += 50;
							}
						}
						if(pLoopCity->isCoastal())
						{
							iEnemyWaterCities += 50;
						}
						else
						{
							iEnemyLandCities += 50;
						}
					}
				}
			}
			if (iEnemyWater > 0)
			{
				iEnemyWater += iEnemyWaterCities;
			}
			else
			{
				iEnemyWater += (iEnemyWaterCities / 10);
			}
			if (iEnemyLand > 0)
			{
				iEnemyLand += iEnemyLandCities;
			}
			else
			{
				iEnemyLand += (iEnemyLandCities / 10);
			}

			if (iFriendlySea > 0)
			{
				iFriendlySea += iFriendlySeaCities;
			}
			else
			{
				iFriendlySea += (iFriendlySeaCities / 10);
			}
			if (iFriendlyLand > 0)
			{
				iFriendlyLand += iFriendlyLandCities;
			}
			else
			{
				iFriendlyLand += (iFriendlyLandCities / 10);
			}

			if (iEnemyWater > iEnemyLand && iEnemyWater > iFriendlySea && m_aiWarFocus[eLoopPlayer] != WARTYPE_SEA)
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp;
					CvString strLogString;
					strLogString.Format("War Type versus %s now WATER. Enemy has: %d Water, %d Land, we have %d Water, %d Land", GET_PLAYER(eLoopPlayer).getCivilizationShortDescription(), iEnemyWater, iEnemyLand, iFriendlyLand, iFriendlySea);
					m_pPlayer->GetTacticalAI()->LogTacticalMessage(strLogString);
				}
				m_aiWarFocus[eLoopPlayer] = WARTYPE_SEA;
			}
			else if (iEnemyLand >= iEnemyWater && iEnemyLand > iFriendlyLand && (iEnemyLand != 0) && m_aiWarFocus[eLoopPlayer] != WARTYPE_LAND)
			{
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp;
					CvString strLogString;
					strLogString.Format("War Type versus %s now LAND. Enemy has: %d Water, %d Land, we have %d Water, %d Land", GET_PLAYER(eLoopPlayer).getCivilizationShortDescription(), iEnemyWater, iEnemyLand, iFriendlyLand, iFriendlySea);
					m_pPlayer->GetTacticalAI()->LogTacticalMessage(strLogString);
				}
				m_aiWarFocus[eLoopPlayer] = WARTYPE_LAND;
			}
			else
			{
				m_aiWarFocus[eLoopPlayer] = WARTYPE_UNDEFINED;
			}		
		}
	}
}
void CvMilitaryAI::SetNumFreeCargo(int iValue)
{
	if(iValue != m_iFreeCargo)
	{
		m_iFreeCargo = iValue;
	}
}
int CvMilitaryAI::GetNumFreeCargo() const
{
	return m_iFreeCargo;
}

void CvMilitaryAI::SetNumFreeCarrier(int iValue)
{
	if(iValue != m_iFreeCarrier)
	{
		m_iFreeCarrier = iValue;
	}
}
int CvMilitaryAI::GetNumFreeCarrier() const
{
	return m_iFreeCarrier;
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
#if defined(MOD_BALANCE_CORE)
	if(pPlayer->IsCramped())
	{
		iCurrentWeight += 5;
	}
#endif

	if (pPlayer->GetMilitaryAI()->GetArmyBeingBuilt() != NO_ARMY_TYPE)
	{
		iCurrentWeight += 100;
	}

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
		if(pkDiplomacyAI->GetWarGoal(eOtherPlayer) == WAR_GOAL_PREPARE)
		{
			iCurrentWeight += 100;
		}

		// Add in weight for each civ we're on really bad terms with
		else if(pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_WAR ||
		        pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_HOSTILE ||
		        pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_AFRAID ||
				pkDiplomacyAI->GetVisibleApproachTowardsUs(eOtherPlayer) == MAJOR_CIV_APPROACH_WAR ||
				pkDiplomacyAI->GetVisibleApproachTowardsUs(eOtherPlayer) == MAJOR_CIV_APPROACH_HOSTILE ||
				pkDiplomacyAI->IsCapitalCapturedBy(eOtherPlayer) || pkDiplomacyAI->IsHolyCityCapturedBy(eOtherPlayer) ||
				pkDiplomacyAI->GetNumCitiesCapturedBy(eOtherPlayer) > 0 || pkDiplomacyAI->GetNumWarsDeclaredOnUs(eOtherPlayer) > 2)
		{
			iCurrentWeight += 50;
		}

		// And some if on fairly bad terms
		else if(pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_GUARDED ||
		        pkDiplomacyAI->GetMajorCivApproach(eOtherPlayer, /*bHideTrueFeelings*/ false) == MAJOR_CIV_APPROACH_DECEPTIVE ||
				pkDiplomacyAI->GetVisibleApproachTowardsUs(eOtherPlayer) == MAJOR_CIV_APPROACH_GUARDED ||
				pkDiplomacyAI->GetNumWarsDeclaredOnUs(eOtherPlayer) > 0)
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
		iStrategyWeight = iBarbarianCampCount * 75 + iVisibleBarbarianCount * 25;   // Two visible camps or 3 roving Barbarians will trigger this
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
		iStrategyWeight = iBarbarianCampCount * 75 + iVisibleBarbarianCount * 25;
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

	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer != pPlayer->GetID() && pPlayer->IsAtWarWith(eLoopPlayer) && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			if (pPlayer->GetDiplomacyAI()->GetWarScore(eLoopPlayer) > 25)
				return true;
		}
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
	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer != pPlayer->GetID() && pPlayer->IsAtWarWith(eLoopPlayer) && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			if (pPlayer->GetDiplomacyAI()->GetWarScore(eLoopPlayer) < -10)
				return true;
		}
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
		if (pPlayer->GetDiversity(DOMAIN_LAND) == (int)UNITAI_RANGED || pPlayer->GetDiversity(DOMAIN_LAND) == (int)UNITAI_CITY_BOMBARD)
		{
			return true;
		}
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
	
#if defined(MOD_BALANCE_CORE)
	if (pPlayer->GetDiplomacyAI()->IsNuclearGandhiEnabled() && !pPlayer->isHuman() && pPlayer->GetPlayerTraits()->GetCityUnhappinessModifier() != 0)
	{
		return true;
	}
#endif

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
#if defined(MOD_BALANCE_CORE)
	int iNumLoadableAirUnits = 0;
	int iNumTotalCargoSpace = 0;
	int iNumEmptyCarriers = 0;
	int iCarrierCargo = 0;

	CvUnit* pLoopUnit;
	int iLoop;
	SpecialUnitTypes eSpecialUnitPlane = (SpecialUnitTypes)GC.getInfoTypeForString("SPECIALUNIT_FIGHTER");
	for (pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
	{
		if (pLoopUnit->getSpecialUnitType() == eSpecialUnitPlane)
		{
			iNumLoadableAirUnits += 1;
		}
		else if (pLoopUnit->isAircraftCarrier())
		{
			iNumTotalCargoSpace += pLoopUnit->cargoSpaceAvailable(eSpecialUnitPlane);
			if (pLoopUnit->getCargo() == 0)
				iNumEmptyCarriers++;

			iNumTotalCargoSpace += pLoopUnit->cargoSpace();
			iCarrierCargo += pLoopUnit->cargoSpace();
		}
	}

	CvCity* pLoopCity;
	int iLoopCity = 0;
	for (pLoopCity = pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoopCity))
	{
		if (pLoopCity != NULL)
		{
			iNumTotalCargoSpace += pLoopCity->GetMaxAirUnits();
		}
	}
	if (iNumLoadableAirUnits > 0)
	{
		pPlayer->GetMilitaryAI()->SetNumFreeCarrier(iCarrierCargo - iNumLoadableAirUnits);
	}
	else
	{
		pPlayer->GetMilitaryAI()->SetNumFreeCarrier(-1);
	}

	pPlayer->GetMilitaryAI()->SetNumFreeCargo(iNumTotalCargoSpace - iNumLoadableAirUnits);

	if (iNumEmptyCarriers==0 && iNumLoadableAirUnits>2*iCarrierCargo)
		return true;
	else
		return false;
#else
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
#endif
}

// MORE NON-MEMBER FUNCTIONS

///	How many military units should we have given current threats?
int MilitaryAIHelpers::ComputeRecommendedNavySize(CvPlayer* pPlayer, int iMinSize)
{
	int iNumUnitsWanted = 0;
	int iFlavorNaval = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
	int iFlavorNavalRecon = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_RECON"));
	// cap at 10?

	// Start with 1
#if defined(MOD_BALANCE_CORE_MILITARY)
		iNumUnitsWanted = /*3*/ GC.getBALANCE_NAVY_START_SIZE();
#else
		iNumUnitsWanted = 1;
#endif
	
	int iNumCoastalCities = 0;
	int iLoop;
	for(CvCity* pCity = pPlayer->firstCity(&iLoop); pCity != NULL; pCity = pPlayer->nextCity(&iLoop))
	{
		if(pCity->isCoastal())
		{
			iNumCoastalCities++;
		}
	}

	iNumUnitsWanted += iNumCoastalCities;
	// Scale up or down based on true threat level and a bit by flavors (multiplier should range from about 0.75 to 2.0)
	float fMultiplier = 0.25f + (pPlayer->GetMilitaryAI()->GetHighestThreat() + iFlavorNaval + iFlavorNavalRecon) / 100.0f;

	//Look at neighbors - if they're stronger than us, let's increase our amount.
	if (MOD_BALANCE_CORE_MILITARY && pPlayer->isMajorCiv())
	{
		for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			PlayerTypes eOtherPlayer = (PlayerTypes) iMajorLoop;
			if (eOtherPlayer != NO_PLAYER && GET_PLAYER(eOtherPlayer).isMajorCiv() && GET_PLAYER(eOtherPlayer).isAlive() && (eOtherPlayer != pPlayer->GetID()))
			{
				if (pPlayer->GetDiplomacyAI()->IsPotentialMilitaryTargetOrThreat(eOtherPlayer))
				{
					fMultiplier += 0.10f;
				}
			}
		}
	}

	int iCityLoop;
	for (CvCity* pLoopCity = pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
	{
		if (pLoopCity && pLoopCity->isCoastal() && pLoopCity->isUnderSiege())
		{
			fMultiplier += .25f;
		}
	}

	iNumUnitsWanted = (int)(iNumUnitsWanted * fMultiplier* /*0.67*/ GC.getAI_STRATEGY_NAVAL_UNITS_PER_CITY());

	iNumUnitsWanted = max(1,iNumUnitsWanted);

	//want more as game goes along, for all units.
	iNumUnitsWanted += pPlayer->GetCurrentEra();

	EconomicAIStrategyTypes eStrategyNavalMap = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NAVAL_MAP");
	EconomicAIStrategyTypes eExpandOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	if (pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyNavalMap) || pPlayer->GetEconomicAI()->IsUsingStrategy(eExpandOtherContinents))
	{
		iNumUnitsWanted *= 2;
	}

	if (pPlayer->getCivilizationInfo().isCoastalCiv())
	{
		iNumUnitsWanted *= 2;
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

	//Navy more than half supply?
	int iSupply = pPlayer->GetNumUnitsSupplied();
	if(iNumUnitsWanted > iSupply)
	{
		iNumUnitsWanted = iSupply;
	}

	if (iNumUnitsWanted <= iMinSize)
		iNumUnitsWanted = iMinSize;

	return iNumUnitsWanted;
}

//todo: use the step pathfinder here to get a plot which is on the correct side of the target? need a starting point then ...
CvPlot* MilitaryAIHelpers::GetCoastalPlotNearPlot(CvPlot *pTarget, bool bCheckTeam)
{
	if (!pTarget)
		return NULL;
	TeamTypes eTeam = pTarget->getTeam();

	//change iteration order, don't return the same plot every time
	//don't use the RNG here: too many calls in the log and diplo quirks can lead to desyncs
	int aiShuffle[3][RING2_PLOTS] = { 
		{ 0,5,6,3,2,4,1,14,13,17,16,15,11,8,9,18,12,7,10 },
		{ 0,4,1,5,2,3,6,14,8,15,12,18,16,9,7,11,10,13,17 },
		{ 0,6,3,5,2,1,4,18,15,16,14,12,17,8,7,10,9,13,11 } };
	int iShuffleType = GC.getGame().getSmallFakeRandNum(3, *pTarget);

	for(int iI = RING0_PLOTS; iI < RING2_PLOTS; iI++)
	{
		CvPlot* pAdjacentPlot = iterateRingPlots(pTarget->getX(), pTarget->getY(), aiShuffle[iShuffleType][iI]);
		if(pAdjacentPlot != NULL && 
			(!bCheckTeam || pAdjacentPlot->getTeam()==eTeam || pAdjacentPlot->getTeam()==NO_TEAM) && //ownership check
			pAdjacentPlot->isShallowWater() && //coastal
			pAdjacentPlot->getFeatureType()==NO_FEATURE && //no ice
			pAdjacentPlot->isLake()==false && //no lake
			pAdjacentPlot->countPassableNeighbors(DOMAIN_SEA)>2) //no bays
		{
			return pAdjacentPlot;
		}
	}

	return NULL;
}
MultiunitFormationTypes MilitaryAIHelpers::GetCurrentBestFormationTypeForNavalAttack()
{
	int iTurnMin = 100;
	iTurnMin *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iTurnMin /= 100;

	//Granular, for later variation (if needed)
	if(GC.getGame().getGameTurn() <= (iTurnMin * 4))
	{
		return MUFORMATION_NAVAL_INVASION;
	}
	else if(GC.getGame().getGameTurn() > (iTurnMin * 4))
	{
		return MUFORMATION_LATE_GAME_INVASION_FORCE;
	}
	return MUFORMATION_NAVAL_INVASION;
}
MultiunitFormationTypes MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack()
{
	int iTurnMin = 100;
	iTurnMin *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iTurnMin /= 100;

	
	//Granular, for later variation (if needed)
	if (GC.getGame().getGameTurn() <= iTurnMin * 2)
	{
		return MUFORMATION_NAVAL_BOMBARDMENT;
	}
	else if(GC.getGame().getGameTurn() <= (iTurnMin * 4))
	{
		return MUFORMATION_PURE_NAVAL_CITY_ATTACK;
	}
	else if(GC.getGame().getGameTurn() > (iTurnMin * 4))
	{
		return MUFORMATION_LATE_GAME_PURE_NAVAL_FORCE;
	}
	return MUFORMATION_NAVAL_INVASION;
}
MultiunitFormationTypes MilitaryAIHelpers::GetCurrentBestFormationTypeForCityAttack()
{
	int iTurnMin = 100;
	iTurnMin *= GC.getGame().getGameSpeedInfo().getTrainPercent();
	iTurnMin /= 100;

	//Granular, for later variation (if needed)
	if(GC.getGame().getGameTurn() <= iTurnMin)
	{
		return MUFORMATION_EARLY_RUSH;
	}
	else if(GC.getGame().getGameTurn() <= iTurnMin * 2)
	{
		return MUFORMATION_SMALL_CITY_ATTACK_FORCE;
	}
	else if(GC.getGame().getGameTurn() <= (iTurnMin * 3))
	{
		return MUFORMATION_BASIC_CITY_ATTACK_FORCE;
	}
	else if(GC.getGame().getGameTurn() > (iTurnMin * 3))
	{
		return MUFORMATION_BIGGER_CITY_ATTACK_FORCE;
	}
	return MUFORMATION_BASIC_CITY_ATTACK_FORCE;
}

int MilitaryAIHelpers::NumberOfFillableSlots(CvPlayer* pPlayer, PlayerTypes eEnemy, MultiunitFormationTypes formation, bool bRequiresNavalMoves, bool bMustBeDeepWaterNaval, CvPlot* pMuster, CvPlot* pTarget, int* piNumberSlotsRequired, int* piNumberLandReservesUsed)
{
	CvPlayerAI& ownerPlayer = GET_PLAYER(pPlayer->GetID());

	int iWillBeFilled = 0;
	int iLandReservesUsed = 0;
	int iRequiredSlots = 0;
	if (piNumberSlotsRequired) 
		*piNumberSlotsRequired = INT_MAX;
	if (piNumberLandReservesUsed) 
		*piNumberLandReservesUsed = 0;

	std::vector< CvFormationSlotEntry > slotsToFill;
	CvMultiUnitFormationInfo* thisFormation = GC.getMultiUnitFormationInfo(formation);
	for(int iThisSlotIndex = 0; iThisSlotIndex < thisFormation->getNumFormationSlotEntries(); iThisSlotIndex++)
	{
		const CvFormationSlotEntry& thisSlotEntry = thisFormation->getFormationSlotEntry(iThisSlotIndex);
		slotsToFill.push_back(thisSlotEntry);
		if (thisSlotEntry.m_requiredSlot)
			iRequiredSlots++;
	}

	if (pMuster == NULL || pTarget == NULL)
		return 0;

	// Now go back through remaining slots and see how many were required, we'll need that many more units
	if(piNumberSlotsRequired)
		(*piNumberSlotsRequired) = iRequiredSlots;

	if (bRequiresNavalMoves || bMustBeDeepWaterNaval)
	{
		CvPlot* pAdjacentPlot = NULL;
		if (pMuster->isCoastalLand())
		{
			pAdjacentPlot = GetCoastalPlotNearPlot(pMuster);
			if (pAdjacentPlot != NULL)
			{
				pMuster = pAdjacentPlot;
			}
			else
			{
				return 0;
			}
		}
		if (pTarget->isCoastalLand())
		{
			pAdjacentPlot = GetCoastalPlotNearPlot(pTarget);
			if (pAdjacentPlot != NULL)
			{
				pTarget = pAdjacentPlot;
			}
			else
			{
				return 0;
			}
		}
	}

	if(pPlayer && eEnemy != NO_PLAYER && pMuster != NULL && pTarget != NULL && (pTarget != pMuster))
	{
		if(bRequiresNavalMoves || bMustBeDeepWaterNaval)
		{
			//naval units are fast and terrain is easy. if there's a trade path we assume we can attack it
			//we enforce the same area for naval ops, everything else leads to problems later
			if (pMuster->getArea() != pTarget->getArea())
			{
				return 0;
			}
			else
			{
				//make sure that both plots are water
				SPathFinderUserData data(pPlayer->GetID(), PT_GENERIC_SAME_AREA, eEnemy);
				data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
				if (!GC.GetStepFinder().DoesPathExist(pMuster, pTarget, data))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strTemp;
						CvString strLogString;
						strLogString.Format("FAILED PATHFINDER - Tallying up land units for %s formation.", thisFormation->GetType());
						pPlayer->GetTacticalAI()->LogTacticalMessage(strLogString);
					}
					return 0;
				}
			}
		}
		else
		{
			//land based ops need a wide path so they don't get stuck - but they don't need to stay within their area
			SPathFinderUserData data( pPlayer->GetID(), iRequiredSlots>5 ? PT_GENERIC_ANY_AREA_WIDE : PT_GENERIC_ANY_AREA, eEnemy );
			data.iFlags = CvUnit::MOVEFLAG_APPROX_TARGET_RING1;
			if(!GC.GetStepFinder().DoesPathExist(pMuster, pTarget, data))
			{
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strTemp;
					CvString strLogString;
					strLogString.Format("FAILED PATHFINDER - Tallying up land units for %s formation.", thisFormation->GetType());
					pPlayer->GetTacticalAI()->LogTacticalMessage(strLogString);
				}
				return 0;
			}
		}
	}

	//fixme - the max number of turns actually depends on the operation we are trying to kick off!
	ReachablePlots turnsFromMuster;
	SPathFinderUserData data(pPlayer->GetID(),PT_GENERIC_REACHABLE_PLOTS,-1,GC.getAI_OPERATIONAL_MAX_RECRUIT_TURNS_DEFAULT());
	turnsFromMuster = GC.GetStepFinder().GetPlotsInReach(pMuster, data);

	int iLoop = 0;
	for(CvUnit* pLoopUnit = ownerPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = ownerPlayer.nextUnit(&iLoop))
	{
		if (OperationalAIHelpers::IsUnitSuitableForRecruitment(pLoopUnit,pMuster,turnsFromMuster,pTarget,bRequiresNavalMoves,bMustBeDeepWaterNaval,thisFormation))
		{
			// Is this unit one of the requested types?
			CvUnitEntry* unitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
			if(unitInfo == NULL)
			{
				continue;
			}

			//Do we fit in any slot?
			for(std::vector< CvFormationSlotEntry >::iterator it = slotsToFill.begin(); it != slotsToFill.end(); it++)
			{
				CvFormationSlotEntry thisSlotEntry = *it;
				if (unitInfo->GetUnitAIType(thisSlotEntry.m_primaryUnitType) || unitInfo->GetUnitAIType(thisSlotEntry.m_secondaryUnitType))
				{
					slotsToFill.erase(it);

					if (pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->IsCombatUnit())
						iLandReservesUsed++;

					iWillBeFilled++;
					break;
				}
			}

			//Run out of slots to test? Break!
			if (slotsToFill.empty())
				break;
		}
	}

	if(piNumberLandReservesUsed)
		*piNumberLandReservesUsed = iLandReservesUsed;

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strTemp;
		CvString strLogString;
		int iLandReserves = pPlayer->GetMilitaryAI()->GetLandReservesAvailable();
		strLogString.Format("Tallying up units for %s formation. Total Needed: %d. Found: %d. Land Reserves: %d, Used: %d", thisFormation->GetType(), iRequiredSlots, iWillBeFilled, iLandReserves, iLandReservesUsed);
		pPlayer->GetTacticalAI()->LogTacticalMessage(strLogString);
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

	bool bMustBeDeepWaterNaval = pPlayer->CanCrossOcean() && thisFormation->IsRequiresNavalUnitConsistency();

	for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
	{
		// Don't count scouts
		if(pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
		{
			// Don't count units that are damaged too heavily
			if(pLoopUnit->GetCurrHitPoints() >= pLoopUnit->GetMaxHitPoints() * GC.getAI_OPERATIONAL_PERCENT_HEALTH_FOR_OPERATION() / 100)
			{
				if(pLoopUnit->getArmyID() == -1 && pLoopUnit->canRecruitFromTacticalAI())
				{
					if(!pLoopUnit->IsRecentlyDeployedFromOperation())
					{
						if(!bRequiresNavalMoves || pLoopUnit->getDomainType() == DOMAIN_SEA || pLoopUnit->CanEverEmbark())
						{
							if (!bMustBeDeepWaterNaval || pLoopUnit->getDomainType() != DOMAIN_SEA || !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
							{
								for(it = slotsToFill.begin(); it != slotsToFill.end(); it++)
								{
									CvFormationSlotEntry slotEntry = *it;
									CvUnitEntry& kUnitInfo = pLoopUnit->getUnitInfo();
									if(kUnitInfo.GetUnitAIType(slotEntry.m_primaryUnitType) ||
										kUnitInfo.GetUnitAIType(slotEntry.m_secondaryUnitType))
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
				UnitAITypes eType = slotsToFill[iThisSlotIndex].m_primaryUnitType;
				if(eType == UNITAI_ASSAULT_SEA || eType == UNITAI_ATTACK_SEA || eType == UNITAI_RESERVE_SEA || eType == UNITAI_ESCORT_SEA)
				{
					if(!bSecondaryUnit)
					{
						return slotsToFill[iThisSlotIndex].m_primaryUnitType;
					}
					else
					{
						return slotsToFill[iThisSlotIndex].m_secondaryUnitType;
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
				return slotsToFill[iThisSlotIndex].m_primaryUnitType;
			}
			else
			{
				return slotsToFill[iThisSlotIndex].m_secondaryUnitType;
			}
		}
	}

	return NO_UNITAI;
}
