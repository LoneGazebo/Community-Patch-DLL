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
#include "CvSpanSerialization.h"

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
	m_iNumberOfTimesOpsBuildSkippedOver(0),
	m_iNumberOfTimesSettlerBuildSkippedOver(0)
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

	m_aiTempFlavors.init();

	Reset();
}

/// Deallocate memory created in initialize
void CvMilitaryAI::Uninit()
{
	SAFE_DELETE_ARRAY(m_pabUsingStrategy);
	SAFE_DELETE_ARRAY(m_paiTurnStrategyAdopted);
	m_aiTempFlavors.uninit();
}

/// Reset AIStrategy status array to all false
void CvMilitaryAI::Reset()
{
	m_potentialAttackTargets.clear();
	m_exposedCities.clear();

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
	m_iRecDefensiveLandUnits = 0;
	m_iRecOffensiveLandUnits = 0;
	m_iRecOffensiveNavalUnits = 0;
	m_eLandDefenseState = NO_DEFENSE_STATE;
	m_eNavalDefenseState = NO_DEFENSE_STATE;
	m_iNumberOfTimesOpsBuildSkippedOver = 0;
	m_iNumberOfTimesSettlerBuildSkippedOver = 0;
#if defined(MOD_BALANCE_CORE)
	for (int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
		m_aiWarFocus[iI] = WARTYPE_UNDEFINED;
	m_iNumFreeCarriers = 0;
	//new unit counters
	m_iNumArcherLandUnits = 0;
	m_iNumSiegeLandUnits = 0;
	m_iNumSkirmisherLandUnits = 0;
	m_iNumReconLandUnits = 0;
	m_iNumBomberAirUnits = 0;
	m_iNumFighterAirUnits = 0;
	m_iNumMeleeNavalUnits = 0;
	m_iNumRangedNavalUnits = 0;
	m_iNumSubmarineNavalUnits = 0;
	m_iNumCarrierNavalUnits = 0;
	m_iNumMissileUnits = 0;
	m_iNumActiveUniqueUnits = 0;
#endif

	for(int iI = 0; iI < m_pAIStrategies->GetNumMilitaryAIStrategies(); iI++)
	{
		m_pabUsingStrategy[iI] = false;
		m_paiTurnStrategyAdopted[iI] = -1;
	}
}

///
template<typename MilitaryAI, typename Visitor>
void CvMilitaryAI::Serialize(MilitaryAI& militaryAI, Visitor& visitor)
{
	CvAssertMsg(militaryAI.m_pAIStrategies != NULL && militaryAI.m_pAIStrategies->GetNumMilitaryAIStrategies() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	visitor(militaryAI.m_iNumberOfTimesOpsBuildSkippedOver);
	visitor(militaryAI.m_iNumberOfTimesSettlerBuildSkippedOver);

	visitor(militaryAI.m_iNumAntiAirUnits);
	visitor(militaryAI.m_iNumLandUnits);
	visitor(militaryAI.m_iNumNavalUnits);
	visitor(militaryAI.m_iRecOffensiveLandUnits);
	visitor(militaryAI.m_iRecOffensiveNavalUnits);
	visitor(militaryAI.m_iNumFreeCarriers);
	visitor(militaryAI.m_potentialAttackTargets);
	visitor(militaryAI.m_exposedCities);

	const int iNumMilitaryAIStrategies = militaryAI.m_pAIStrategies->GetNumMilitaryAIStrategies();
	visitor(MakeConstSpan(militaryAI.m_pabUsingStrategy, iNumMilitaryAIStrategies));
	visitor(MakeConstSpan(militaryAI.m_paiTurnStrategyAdopted, iNumMilitaryAIStrategies));
	visitor(militaryAI.m_aiWarFocus);
}

/// Serialization read
void CvMilitaryAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvMilitaryAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvMilitaryAI& militaryAI)
{
	militaryAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvMilitaryAI& militaryAI)
{
	militaryAI.Write(stream);
	return stream;
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
	if (eStrategy == NO_MILITARYAISTRATEGY)
		return false;

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
	ScanForBarbarians();
	UpdateBaseData();
	UpdateDefenseState();
	UpdateMilitaryStrategies();
	UpdateWarType();

	//do this also for humans because AI relies on the data!
	UpdateAttackTargets();

	if(!m_pPlayer->isHuman())
	{
		UpdateOperations();
		MakeEmergencyPurchases();
		DisbandObsoleteUnits();
	}

	if(!m_pPlayer->isMinorCiv())
	{
		LogMilitaryStatus();

		if(IsBuildingArmy(ARMY_TYPE_ANY))
		{
			LogAvailableForces();
		}
	}
}

/// Ask to send a nuke at an enemy
bool CvMilitaryAI::RequestNukeAttack(PlayerTypes eEnemy)
{
	if(m_pPlayer->getNumNukeUnits() > 0)
		return m_pPlayer->addAIOperation(AI_OPERATION_NUKE_ATTACK, 0, eEnemy)!=NULL;

	return false;
}


/// harass the enemy
bool CvMilitaryAI::RequestPillageAttack(PlayerTypes eEnemy)
{
	if (!m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_PILLAGE_ENEMY))
		//don't try to build additional units, only do this if we have enough at hand
		return m_pPlayer->addAIOperation(AI_OPERATION_PILLAGE_ENEMY, 0, eEnemy) != NULL;

	return false;
}

/// Send an army to force concessions
bool CvMilitaryAI::RequestBullyingOperation(PlayerTypes eEnemy)
{
	if (GET_PLAYER(eEnemy).isBarbarian())
		return false;

	// If we already have an operation on the way, don't send another one
	if (m_pPlayer->HasAnyOffensiveOperationsAgainstPlayer(eEnemy))
		return false;

	CvCity* pTargetCity = GET_PLAYER(eEnemy).isMajorCiv() ? m_pPlayer->GetClosestCityToUsByPlots(eEnemy) : GET_PLAYER(eEnemy).getCapitalCity();
	if (!pTargetCity)
		return false;

	//don't venture too far
	int iDistanceTurns = m_pPlayer->GetCityDistancePathLength(pTargetCity->plot());
	if (iDistanceTurns > 23)
		return false;

	CvCity* pMusterCity = m_pPlayer->GetClosestCityByPathLength(pTargetCity->plot());
	if (!pMusterCity)
		return false;

	//if the target is very close assume we can embark or don't even need to
	AIOperationTypes opType = (pMusterCity->getArea() == pTargetCity->getArea()) ? AI_OPERATION_CITY_ATTACK_LAND : AI_OPERATION_CITY_ATTACK_NAVAL;

	//don't try to build additional units, only do this if we have enough at hand
	return m_pPlayer->addAIOperation(opType, 0, eEnemy, pTargetCity, pMusterCity) != NULL;
}

/// Spend money to quickly add a unit to a city
CvUnit* CvMilitaryAI::BuyEmergencyUnit(UnitAITypes eUnitType, CvCity* pCity)
{
	// No units in puppet cities except for Venice!
	if (CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(pCity))
	{
		return NULL;
	}

	// Get best unit with this AI type
	UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(eUnitType, true);
	if(eType != NO_UNIT)
	{
		CvUnitEntry* pUnitInfo = GC.GetGameUnits()->GetEntry(eType);
		if(pUnitInfo && pUnitInfo->GetUnitAIType(eUnitType))
		{
			// Can we buy the primary unit type at the start city?
			if(pCity->IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, eType, NO_BUILDING, NO_PROJECT, YIELD_GOLD))
			{
				int iGoldCost = pCity->GetPurchaseCost(eType);
				int iPriority = /*500*/ GD_INT_GET(AI_GOLD_PRIORITY_UNIT);
				if(m_pPlayer->GetEconomicAI()->CanWithdrawMoneyForPurchase(PURCHASE_TYPE_UNIT, iGoldCost, iPriority))
				{
					if(pCity->getOwner() == m_pPlayer->GetID())		// Player must own the city or this will create a unit for another player
					{
						// This is an EXTRA build for the operation beyond any that are already assigned to this city, so pass in the right flag to CreateUnit()
						CvUnit* pUnit = pCity->CreateUnit(eType, NO_UNITAI, REASON_BUY, false /*bUseToSatisfyOperation*/);
						if (pUnit)
						{
							m_pPlayer->GetTreasury()->LogExpenditure((CvString)pUnit->getUnitInfo().GetText(), iGoldCost, 7);
							m_pPlayer->GetTreasury()->ChangeGold(-iGoldCost);

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
					CvUnit* pUnit = pCity->CreateUnit(eType, NO_UNITAI, REASON_FAITH_BUY, false /*bUseToSatisfyOperation*/);
					if (pUnit)
					{
						CvString szMsg;
						szMsg.Format("Emergency Faith Unit Purchase: %s, ", pUnit->getUnitInfo().GetDescription());
						szMsg += pCity->getName();
						m_pPlayer->GetTacticalAI()->LogTacticalMessage(szMsg);
					}
					return pUnit;
				}
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
					int iPriority = /*250*/ GD_INT_GET(AI_GOLD_PRIORITY_DEFENSIVE_BUILDING);
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

// check if the army path makes sense
bool MilitaryAIHelpers::ArmyPathIsGood(const SPath & path, PlayerTypes eAttacker, PlayerTypes eIntendedEnemy)
{
	//define short paths as safe (start and dest plot are included in count)
	if (path.vPlots.size() < 4)
		return true;

	bool bWaterPath = path.get(1)->isWater();
	int iThirdPartyPlots = 0;

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

		//passing another city of our enemy? then we should attack that one first
		if (pCity->getOwner() == eIntendedEnemy)
		{
			if (path.get(-1)->getOwningCityID() != pCity->GetID())
			{
				if (!bWaterPath || pCity->isCoastal())
					return false;
			}
		}
		//maybe we have a better muster plot?
		else if (pCity->getOwner() == eAttacker)
		{
			if (path.get(0)->getOwningCityID() != pCity->GetID())
			{
				if (!bWaterPath || pCity->isCoastal())
					return false;
			}
		}
		//passing through a different warzone? not good
		else if (GET_PLAYER(eAttacker).IsAtWarWith(pCity->getOwner()))
			return false;

		//the trade path may lead through third-party territory
		if (pPlot->isOwned() && pPlot->getOwner()!=eIntendedEnemy && !pPlot->IsFriendlyTerritory(eAttacker))
			iThirdPartyPlots++;
	}

	//the trade paths often lead through third party territory which an army may not be able to pass
	//if we need open borders and there is no easy detour, exclude this path
	return (iThirdPartyPlots<3);
}

bool CvMilitaryAI::IsPossibleAttackTarget(CvCity* pCity) const
{
	if (!pCity || pCity->getOwner()==m_pPlayer->GetID())
		return false;

	for (size_t i = 0; i < m_potentialAttackTargets.size(); i++)
		if (m_potentialAttackTargets[i].GetTargetPlot() == pCity->plot())
			return true;

	return false;
}

bool CvMilitaryAI::IsPreferredAttackTarget(CvCity* pCity) const
{
	if (!pCity || pCity->getOwner()==m_pPlayer->GetID())
		return false;

	for (size_t i = 0; i < m_potentialAttackTargets.size(); i++)
		if (m_potentialAttackTargets[i].GetTargetPlot() == pCity->plot() && m_potentialAttackTargets[i].IsPreferred())
			return true;

	return false;
}

bool CvMilitaryAI::IsExposedToEnemy(CvCity * pCity, PlayerTypes eOtherPlayer) const
{
	for (size_t i = 0; i < m_exposedCities.size(); i++)
		if (eOtherPlayer==NO_PLAYER || m_exposedCities[i].first == eOtherPlayer)
			if (pCity == NULL || m_exposedCities[i].second == pCity->GetID())
				return true;

	return false;
}

bool CvMilitaryAI::HavePreferredAttackTarget(PlayerTypes eEnemy) const
{
	for (size_t i = 0; i < m_potentialAttackTargets.size(); i++)
		if (m_potentialAttackTargets[i].GetTargetPlot()->getOwner() == eEnemy && m_potentialAttackTargets[i].IsPreferred())
			return true;

	return false;
}

bool CvMilitaryAI::HavePossibleAttackTarget(PlayerTypes eEnemy) const
{
	for (size_t i = 0; i < m_potentialAttackTargets.size(); i++)
		if (m_potentialAttackTargets[i].GetTargetPlot()->getOwner() == eEnemy)
			return true;

	return false;
}

//override the diplo AI method so that it returns a sensible result for barbarians
bool CvMilitaryAI::IsPlayerValid(PlayerTypes eOtherPlayer) const
{
	if (eOtherPlayer == BARBARIAN_PLAYER)
		return true;

	//ignore our vassals
	if (m_pPlayer->GetDiplomacyAI()->IsMaster(eOtherPlayer))
		return false;

	//minors don't do sneak attacks
	if (m_pPlayer->isMinorCiv() && m_pPlayer->IsAtPeaceWith(eOtherPlayer))
		return false;

	return m_pPlayer->GetDiplomacyAI()->IsPlayerValid(eOtherPlayer);
}

size_t CvMilitaryAI::UpdateAttackTargets()
{
	m_potentialAttackTargets.clear();
	m_exposedCities.clear();

	vector<OptionWithScore<CvAttackTarget>> vAttackOptions;
	vector<OptionWithScore<CvAttackTarget>> vDefenseOptions;

	int iCityLoop = 0;
	for (CvCity* pMusterCity = m_pPlayer->firstCity(&iCityLoop); pMusterCity != NULL; pMusterCity = m_pPlayer->nextCity(&iCityLoop))
	{
		map<int, SPath> landpaths = GC.getGame().GetGameTrade()->GetAllPotentialTradeRoutesFromCity(pMusterCity, false);
		map<int, SPath> waterpaths = GC.getGame().GetGameTrade()->GetAllPotentialTradeRoutesFromCity(pMusterCity, true);

		//first filter land targets
		for (map<int, SPath>::iterator it = landpaths.begin(); it != landpaths.end(); ++it)
		{
			PlayerTypes eOtherPlayer = it->second.get(-1)->getOwner();

			if (!IsPlayerValid(eOtherPlayer))
				continue;
			if (!MilitaryAIHelpers::ArmyPathIsGood(it->second,m_pPlayer->GetID(),eOtherPlayer))
				continue;

			CvAttackTarget target;
			target.SetWaypoints(it->second);
			MilitaryAIHelpers::SetBestTargetApproach(target, m_pPlayer->GetID());
			if (target.m_iApproachScore == 0)
				continue;

			vAttackOptions.push_back(OptionWithScore<CvAttackTarget>(target, ScoreAttackTarget(target)));

			//and now the other way around
			if (GET_PLAYER(eOtherPlayer).isMajorCiv())
			{
				it->second.invert();
				if (MilitaryAIHelpers::ArmyPathIsGood(it->second, eOtherPlayer, m_pPlayer->GetID()))
				{
					CvAttackTarget reverseTarget;
					reverseTarget.SetWaypoints(it->second);
					MilitaryAIHelpers::SetBestTargetApproach(reverseTarget, eOtherPlayer);
					if (target.m_iApproachScore > 0)
					{
						//do not try any advanced scoring, just use a basic approach/distance score
						int iScore = (reverseTarget.m_iApproachScore * 10) / sqrti(it->second.length());
						vDefenseOptions.push_back(OptionWithScore<CvAttackTarget>(reverseTarget, iScore));
					}
				}
			}
		}

		//then water targets
		for (map<int, SPath>::iterator it = waterpaths.begin(); it != waterpaths.end(); ++it)
		{
			PlayerTypes eOtherPlayer = it->second.get(-1)->getOwner();

			if (!IsPlayerValid(eOtherPlayer))
				continue;
			if (!MilitaryAIHelpers::ArmyPathIsGood(it->second,m_pPlayer->GetID(),eOtherPlayer))
				continue;

			CvAttackTarget target;
			target.SetWaypoints(it->second);
			MilitaryAIHelpers::SetBestTargetApproach(target, m_pPlayer->GetID());
			if (target.m_iApproachScore == 0)
				continue;

			vAttackOptions.push_back(OptionWithScore<CvAttackTarget>(target, ScoreAttackTarget(target)));

			//and now the other way around
			if (GET_PLAYER(eOtherPlayer).isMajorCiv())
			{
				it->second.invert();
				if (MilitaryAIHelpers::ArmyPathIsGood(it->second, eOtherPlayer, m_pPlayer->GetID()))
				{
					CvAttackTarget reverseTarget;
					reverseTarget.SetWaypoints(it->second);
					MilitaryAIHelpers::SetBestTargetApproach(reverseTarget, eOtherPlayer);
					if (target.m_iApproachScore > 0)
					{
						//do not try any advanced scoring, just use a basic approach/distance score
						int iScore = (reverseTarget.m_iApproachScore * 10) / sqrti(it->second.length());
						vDefenseOptions.push_back(OptionWithScore<CvAttackTarget>(reverseTarget, iScore));
					}
				}
			}
		}
	}

	if (!vAttackOptions.empty())
	{
		//some might be present twice but we sort that out below
		std::sort(vAttackOptions.begin(), vAttackOptions.end());

		int iBestScore = vAttackOptions.front().score;
		for (size_t i = 0; i < vAttackOptions.size(); i++)
		{
			CvAttackTarget target = vAttackOptions[i].option;
			int iScore = vAttackOptions[i].score;
			if (iScore == 0)
				continue;

			//mark the best targets
			target.m_bPreferred = (iScore > iBestScore / 3);

			//don't target a city twice with different army types, only keep the best approach
			if (IsPossibleAttackTarget(target.GetTargetPlot()->getPlotCity()))
				continue;

			m_potentialAttackTargets.push_back(target);

			if (GC.getLogging() && GC.getAILogging())
			{
				CvCity* pMuster = target.GetMusterPlot()->getPlotCity();
				CvCity* pTarget = target.GetTargetPlot()->getPlotCity();
				CvString msg = CvString::format("%03d, %s, %sattack target: %s, muster: %s, army type: %s, score: %d",
					GC.getGame().getElapsedGameTurns(), m_pPlayer->getName(), target.IsPreferred() ? "preferred " : "",
					pTarget->getNameNoSpace().c_str(), pMuster->getNameNoSpace().c_str(), ArmyTypeToString(target.m_armyType), iScore);
				CvString playerName = GetPlayer()->getCivilizationShortDescription();
				FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);
				if (pLog)
					pLog->Msg(msg.c_str());
			}
		}
	}

	if (!vDefenseOptions.empty())
	{
		std::sort(vDefenseOptions.begin(), vDefenseOptions.end());

		int iBestScore = vDefenseOptions.front().score;
		for (size_t i = 0; i < vDefenseOptions.size(); i++)
		{
			CvAttackTarget target = vDefenseOptions[i].option;
			int iScore = vDefenseOptions[i].score;

			if (iScore > iBestScore / 3)
			{
				CvCity* pMuster = target.GetMusterPlot()->getPlotCity();
				CvCity* pTarget = target.GetTargetPlot()->getPlotCity();

				//store it only once (for one army type)
				if (!IsExposedToEnemy(pTarget, pMuster->getOwner()))
				{
					m_exposedCities.push_back(make_pair(pMuster->getOwner(), pTarget->GetID()));

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString msg = CvString::format("%03d, %s, exposed city: %s, muster: %s, army type: %s, score: %d",
							GC.getGame().getElapsedGameTurns(), m_pPlayer->getName(),
							pTarget->getNameNoSpace().c_str(), pMuster->getNameNoSpace().c_str(), ArmyTypeToString(target.m_armyType), iScore);
						CvString playerName = GetPlayer()->getCivilizationShortDescription();
						FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);
						if (pLog)
							pLog->Msg(msg.c_str());
					}
				}
			}
		}
	}

	return m_potentialAttackTargets.size();
}

bool CvMilitaryAI::RequestCityAttack(PlayerTypes eIntendedTarget, int iNumUnitsWillingToBuild, bool bCareful)
{
	//note that a given target might be repeated with different muster points / army types
	for (size_t i = 0; i < m_potentialAttackTargets.size(); i++)
	{
		//ignore bad targets!
		if (bCareful && !m_potentialAttackTargets[i].IsPreferred())
			continue;

		CvPlot* pMusterPlot = m_potentialAttackTargets[i].GetMusterPlot();
		CvPlot* pTargetPlot = m_potentialAttackTargets[i].GetTargetPlot();
		PlayerTypes eTargetPlayer = pTargetPlot->getOwner();
		if (eTargetPlayer != eIntendedTarget)
			continue;

		AIOperationTypes opType = AI_OPERATION_TYPE_UNKNOWN;
		switch (m_potentialAttackTargets[i].m_armyType)
		{
		case ARMY_TYPE_LAND:
			opType = AI_OPERATION_CITY_ATTACK_LAND;
			break;
		case ARMY_TYPE_NAVAL:
			opType = AI_OPERATION_CITY_ATTACK_NAVAL;
			break;
		case ARMY_TYPE_COMBINED:
			opType = AI_OPERATION_CITY_ATTACK_COMBINED;
			break;
		default:
			continue;
		}

		//don't duplicate operations
		if (bCareful && m_pPlayer->getFirstAIOperationOfType(opType, eTargetPlayer, pTargetPlot) != NULL)
			continue;

		//if we're being careless, just use whatever units we have and do not wait for new ones
		if (m_pPlayer->addAIOperation(opType, bCareful ? iNumUnitsWillingToBuild : 0, eTargetPlayer, pTargetPlot->getPlotCity(), pMusterPlot->getPlotCity()) != NULL)
			return true;
	}

	return false;
}

void MilitaryAIHelpers::SetBestTargetApproach(CvAttackTarget& target, PlayerTypes ePlayer)
{
	//default
	target.m_iApproachScore = 0;
	target.m_armyType = ARMY_TYPE_ANY;

	//compare
	int iLandScore = EvaluateTargetApproach(target,ePlayer,ARMY_TYPE_LAND);
	int iNavalScore = EvaluateTargetApproach(target,ePlayer,ARMY_TYPE_NAVAL);
	int iCombinedScore = EvaluateTargetApproach(target,ePlayer,ARMY_TYPE_COMBINED);

	//we want a minimum score of 30!
	if (iLandScore >= iNavalScore && iLandScore >= iCombinedScore && iLandScore>30)
	{
		target.m_armyType = ARMY_TYPE_LAND;
		target.m_iApproachScore = iLandScore;
	}
	else if (iNavalScore >= iLandScore && iNavalScore >= iCombinedScore && iNavalScore>30)
	{
		target.m_armyType = ARMY_TYPE_NAVAL;
		target.m_iApproachScore = iNavalScore;
	}
	else if (iCombinedScore >= iNavalScore && iCombinedScore >= iLandScore && iCombinedScore>30)
	{
		target.m_armyType = ARMY_TYPE_COMBINED;
		target.m_iApproachScore = iCombinedScore;
	}
}

int CvMilitaryAI::ScoreAttackTarget(const CvAttackTarget& target)
{
	CvCity* pTargetCity = target.GetTargetPlot()->getPlotCity();

	float fDistWeightInterpolated = 1;
	if(target.m_armyType==ARMY_TYPE_LAND)
	{
		// interpolate linearly between a low and a high distance
		float fDistanceLow = 8, fWeightLow = 10;
		float fDistanceHigh = 24, fWeightHigh = 1;

		float fSlope = (fWeightHigh-fWeightLow) / (fDistanceHigh-fDistanceLow);
		fDistWeightInterpolated = (target.GetPathLength()-fDistanceLow) * fSlope + fWeightLow;
		fDistWeightInterpolated = min( fWeightLow, max( fWeightHigh, fDistWeightInterpolated ) );
	}
	else
	{
		// interpolate linearly between a low and a high distance
		float fDistanceLow = 8, fWeightLow = 6;
		float fDistanceHigh = 36, fWeightHigh = 1;

		float fSlope = (fWeightHigh-fWeightLow) / (fDistanceHigh-fDistanceLow);
		fDistWeightInterpolated = (target.GetPathLength()-fDistanceLow) * fSlope + fWeightLow;
		fDistWeightInterpolated = min( fWeightLow, max( fWeightHigh, fDistWeightInterpolated ) );
	}

	//scores for each target are estimated before calling this function
	float fDesirability = 1.f;
	if (pTargetCity->IsOriginalCapital() && (m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest() || m_pPlayer->GetDiplomacyAI()->IsCloseToWorldConquest()))
	{
		fDesirability *= /*250*/ GD_INT_GET(AI_MILITARY_CAPTURING_ORIGINAL_CAPITAL);
		fDesirability /= 100;
	}
	if (pTargetCity->getOriginalOwner() == m_pPlayer->GetID())
	{
		fDesirability *= /*150*/ GD_INT_GET(AI_MILITARY_RECAPTURING_OWN_CITY);
		fDesirability /= 100;
	}

	//Going after a City-State? Depends if it has allies
	if(GET_PLAYER(pTargetCity->getOwner()).isMinorCiv())
	{
		//in general prefer to target major players ...
		//todo: maybe also factor in traits? austria, venice, germany, greece? what about statecraft
		fDesirability /= 2;

		//unless they are allied to our enemy
		PlayerTypes eAlly = GET_PLAYER(pTargetCity->getOwner()).GetMinorCivAI()->GetAlly();
		if (eAlly != NO_PLAYER)
		{
			if (GET_TEAM(GET_PLAYER(eAlly).getTeam()).isAtWar(GetPlayer()->getTeam()))
			{
				fDesirability *= 100;
				fDesirability /= max(1, /*250*/ GD_INT_GET(AI_MILITARY_CAPTURING_ORIGINAL_CAPITAL));
			}
		}
		else if (m_pPlayer->IsAtWarAnyMajor() && !m_pPlayer->IsAtWarWith(pTargetCity->getOwner()))
		{
			//don't target minors at all while at war with an unrelated major
			return 0;
		}
	}

	//If we were given a quest to go to war with this player, that should influence our decision. Plus, it probably means he's a total jerk.
	for(int iMinorLoop = MAX_MAJOR_CIVS; iMinorLoop < MAX_CIV_PLAYERS; iMinorLoop++)
	{
		PlayerTypes eMinor = (PlayerTypes) iMinorLoop;
		CvPlayer* pMinor = &GET_PLAYER(eMinor);
		if (!pMinor->isAlive())
			continue;

		CvMinorCivAI* pMinorCivAI = pMinor->GetMinorCivAI();

		if (MOD_DIPLOMACY_CITYSTATES)
		{
			if(pMinorCivAI->IsActiveQuestForPlayer(m_pPlayer->GetID(), MINOR_CIV_QUEST_LIBERATION))
			{
				if(pTargetCity->getOriginalOwner() == pMinor->GetMinorCivAI()->GetQuestData1(m_pPlayer->GetID(), MINOR_CIV_QUEST_LIBERATION))
				{
					fDesirability *= /*200*/ GD_INT_GET(AI_MILITARY_RECAPTURING_CITY_STATE);
					fDesirability /= 100;
				}	
			}
		}

		if(pMinorCivAI->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_WAR))
		{
			if(pMinorCivAI->GetQuestData1(GetPlayer()->GetID(), MINOR_CIV_QUEST_WAR) == pTargetCity->getOwner())
			{
				fDesirability *= /*150*/ GD_INT_GET(AI_MILITARY_RECAPTURING_OWN_CITY);
				fDesirability /= 100;
			}
		}
		if(pMinorCivAI->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_LIBERATION))
		{
			if(pMinorCivAI->GetQuestData1(GetPlayer()->GetID(), MINOR_CIV_QUEST_LIBERATION) == eMinor)
			{
				TeamTypes eConquerorTeam = GET_TEAM(pMinor->getTeam()).GetKilledByTeam();	
				if(eConquerorTeam == pTargetCity->getTeam())
				{
					fDesirability *= /*150*/ GD_INT_GET(AI_MILITARY_RECAPTURING_OWN_CITY);
					fDesirability /= 100;
				}
			}
		}
		if(pMinorCivAI->IsActiveQuestForPlayer(GetPlayer()->GetID(), MINOR_CIV_QUEST_UNIT_GET_CITY))
		{
			int iX = pMinorCivAI->GetQuestData1(GetPlayer()->GetID(), MINOR_CIV_QUEST_UNIT_GET_CITY);
			int iY = pMinorCivAI->GetQuestData2(GetPlayer()->GetID(), MINOR_CIV_QUEST_UNIT_GET_CITY);

			CvPlot* pPlot = GC.getMap().plot(iX, iY);
			if(pPlot != NULL && pPlot->isCity())
			{
				if(pPlot->getOwner() == pTargetCity->getOwner())
				{
					fDesirability *= /*150*/ GD_INT_GET(AI_MILITARY_RECAPTURING_OWN_CITY);
					fDesirability /= 100;
				}
			}
		}
	}

	//If we get instant yields from conquering specific terrain types, look for cities with those
	for (int iYield = 0; iYield < YIELD_TOURISM; iYield++)
	{
		YieldTypes eYield = (YieldTypes)iYield;
		for (int iTerrain = 0; iTerrain < GC.getNumTerrainInfos(); iTerrain++)
		{
			TerrainTypes eTerrain = (TerrainTypes)iTerrain;
			if (m_pPlayer->GetPlayerTraits()->GetYieldChangeFromTileConquest(eTerrain, eYield) > 0)
			{
				int iNumTerrain = pTargetCity->CountTerrain(eTerrain); // not using CountAllOwnedTerrain to save performance
				if (iNumTerrain > 0)
				{
					fDesirability *= 100 + (3 * iNumTerrain);
					fDesirability /= 100;
				}
			}
		}
	}

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	if(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
	{
		for(int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes) iResourceLoop;
			if(pTargetCity->IsHasResourceLocal(eResource, true) && (GC.getMap().getNumResources(eResource) > 0))
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

	// Economic value / hardness of target
	float fEconomicValue =  sqrt( pTargetCity->getEconomicValue( GetPlayer()->GetID() ) / float(max(1,pTargetCity->GetMaxHitPoints()-pTargetCity->getDamage())) );

	//everything together now
	int iRtnValue = (int)(target.m_iApproachScore * fDistWeightInterpolated * fDesirability * fEconomicValue);

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

// score 100: optimal terrain, score 0 worst terrain 
int MilitaryAIHelpers::EvaluateTargetApproach(const CvAttackTarget& target, PlayerTypes ePlayer, ArmyType eArmyType)
{
	CvPlot* pMusterPlot = target.GetMusterPlot();
	CvPlot* pStagingPlot = target.GetStagingPlot();
	CvPlot* pTargetPlot = target.GetTargetPlot();

	//basic sanity check
	if (eArmyType == ARMY_TYPE_LAND)
	{
		if (pStagingPlot->isWater())
			return 0;
	}
	else
	{
		if (!pStagingPlot->isWater())
			return 0;
	}

	//Expanded to look at three hexes around each city - will give a better understanding of approach.
	int nGoodPlots = 0;
	int nUsablePlots = 0;

	//all else equal, we prefer a pure naval approach over combined
	int iScale = (eArmyType == ARMY_TYPE_COMBINED ? 54 : 45);

	CvCity* pTargetCity = pTargetPlot->getPlotCity();
	if (!pTargetCity)
		return 0;

	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	for (int i = RING0_PLOTS; i < RING4_PLOTS; i++)
	{
		bool bIsGood = false;
		CvPlot* pLoopPlot = iterateRingPlots(pTargetPlot,i);
		if (pLoopPlot == NULL)
			continue;

		//cannot go here? important, ignore territory checks (typically we are at peace without open borders)
		if(!pLoopPlot->isValidMovePlot(ePlayer,false) || pLoopPlot->isCity())
			continue;

		//ignore plots owned by third parties
		if (pLoopPlot->isOwned() && pLoopPlot->getTeam() != eTeam && pLoopPlot->getTeam() != pTargetPlot->getTeam())
			if (!GET_TEAM(eTeam).IsAllowsOpenBordersToTeam(pLoopPlot->getTeam()))
				continue;

		//ignore plots outside of the zone
		int iStagingDistance = plotDistance(*pStagingPlot, *pLoopPlot);
		int iTargetDistance = plotDistance(*pTargetPlot, *pLoopPlot);
		if (iTargetDistance > 3 || iStagingDistance > 3)
			continue;

		//correct area?
		if (!pTargetCity->isMatchingArea(pLoopPlot))
			continue;

		if (eArmyType==ARMY_TYPE_LAND)
			//siege weapons cannot set up here
			if (pLoopPlot->isWater())
				continue;

		if (eArmyType==ARMY_TYPE_NAVAL)
			//ships cannot go here
			if (!pLoopPlot->isWater())
				continue;

		//for naval invasions we want coastal plots
		//ignore inland plots, combined attacks tend to get higher scores than pure naval anyway)
		if (eArmyType == ARMY_TYPE_COMBINED)
			if (!pLoopPlot->isWater() && !pLoopPlot->isCoastalLand())
				continue;

		//enemy citadels are dangerous, pretend we cannot use those plots
		if (TacticalAIHelpers::IsOtherPlayerCitadel(pLoopPlot, ePlayer, false))
			continue;

		//makes us slow
		if(!pLoopPlot->isRoughGround())
			bIsGood = true;

		//we want to have plots for our siege units
		if (iTargetDistance == 2 && pLoopPlot->canSeePlot(pTargetPlot,NO_TEAM,2,NO_DIRECTION))
			bIsGood = true;

		if (bIsGood)
			nGoodPlots++;
		else
			nUsablePlots++;
	}

	//we have 17 eligible plots, so max score is 51
	int iScore = nGoodPlots * 3 + nUsablePlots;
	int iResult = (iScore * 100) / iScale;

	if (gDebugOutput)
	{
		CvCity* pMusterCity = pMusterPlot->getPlotCity();

		OutputDebugString(CvString::format("%s attack approach on %s from %s is %d. staging at (%d:%d), good %d, ok %d\n", 
			ArmyTypeToString(eArmyType), pTargetCity->getNameKey(), pMusterCity ? pMusterCity->getNameKey() : "unknown", iResult, pStagingPlot->getX(), pStagingPlot->getY(), nGoodPlots, nUsablePlots).c_str());
	}

	return iResult;
}

// PROVIDE MILITARY DATA TO OTHER SUBSYSTEMS

bool CvMilitaryAI::ShouldFightBarbarians() const
{
	return m_iBarbarianCampCount > 2 || m_iVisibleBarbarianCount > 4;
}

/// How many civs are we fighting?
int CvMilitaryAI::GetNumberCivsAtWarWith(bool bIncludeMinor) const
{
	vector<PlayerTypes> vEnemies = m_pPlayer->GetPlayersAtWarWith();

	int iRtnValue = 0;
	for(size_t i=0; i<vEnemies.size(); i++)
	{
		//ignore barbarians ...
		if(GET_PLAYER(vEnemies[i]).isBarbarian())
			continue;

		if(GET_PLAYER(vEnemies[i]).isMinorCiv() && !bIncludeMinor)
			continue;

		iRtnValue++;
	}

	return iRtnValue;
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
	}
}

/// Log that a unit is being scrapped because of the deficit checks
void CvMilitaryAI::LogDeficitScrapUnit(CvUnit* pUnit, bool bGifted)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strTemp;

		// Open the right file
		CvString playerName = m_pPlayer->getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		strOutBuf.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strOutBuf += playerName + ", ";
		if (bGifted)
			strTemp.Format("Gifting %s, X: %d, Y: %d, ", pUnit->getUnitInfo().GetDescription(), pUnit->getX(), pUnit->getY());
		else
			strTemp.Format("Scrapping %s, X: %d, Y: %d, ", pUnit->getUnitInfo().GetDescription(), pUnit->getX(), pUnit->getY());
		strOutBuf += strTemp;
		strOutBuf += "b/c bankrupt, , ";   //extra space so format is consistent with LogScrapUnit()

		if(pUnit->getDomainType() == DOMAIN_LAND)
		{
			strTemp.Format("Num Land Units: %d, In Armies %d, Rec Size: %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecOffensiveLandUnits + m_iRecDefensiveLandUnits);
		}
		else
		{
			strTemp.Format("Num Naval Units: %d, In Armies %d, Rec Size %d, ", m_iNumNavalUnits, m_iNumNavalUnitsInArmies, m_iRecOffensiveNavalUnits);
		}

		strOutBuf += strTemp;
		if (pLog)
			pLog->Msg(strOutBuf);
	}
}

// PRIVATE METHODS

/// Compute data we need later to make decisions during the turn processing
void CvMilitaryAI::UpdateBaseData()
{

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
	m_iRecOffensiveNavalUnits = 0;
	m_iNumFreeCarriers = 0;
	
	// new counters
	m_iNumArcherLandUnits = 0;
	m_iNumSiegeLandUnits = 0;
	m_iNumSkirmisherLandUnits = 0;
	m_iNumReconLandUnits = 0;
	m_iNumBomberAirUnits = 0;
	m_iNumFighterAirUnits = 0;
	m_iNumMeleeNavalUnits = 0;
	m_iNumRangedNavalUnits = 0;
	m_iNumSubmarineNavalUnits = 0;
	m_iNumCarrierNavalUnits = 0;
	m_iNumMissileUnits = 0;
	m_iNumActiveUniqueUnits = 0;

	int iLoop;
	for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		// Don't count civilians or exploration units
		if (!pLoopUnit->IsCanAttack()/* || pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE && pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA*/) //second conditional didn't worked anyway, it should've been or
			continue;

		if (!MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
		{
			if (pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE)
			{
				m_iNumLandUnits++;

				if (pLoopUnit->getArmyID() != -1)
					m_iNumLandUnitsInArmies++;

				if (pLoopUnit->IsCanAttackRanged())
					m_iNumRangedLandUnits++;
				else if (pLoopUnit->canIntercept())
					m_iNumAntiAirUnits++;
				else if (pLoopUnit->getUnitInfo().GetMoves() > 2)
					m_iNumMobileLandUnits++;
				else
					m_iNumMeleeLandUnits++;
			}
			else if (pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA)
			{
				if (pLoopUnit->getArmyID() != -1)
					m_iNumNavalUnitsInArmies++;

				m_iNumNavalUnits++;

				//a carrier is considered free if it is not in a strike group or empty
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_CARRIER_SEA && (pLoopUnit->getArmyID() == -1 || pLoopUnit->getCargo() == 0))
					m_iNumFreeCarriers++;
			}
			else if (pLoopUnit->getDomainType() == DOMAIN_AIR && !pLoopUnit->isSuicide())
			{
				m_iNumAirUnits++;
			}
		}
		else
		{
/*			if (m_pPlayer->getCivilizationInfo().isCivilizationUnitOverridden(pLoopUnit->getUnitInfo().GetUnitClassType())) //not currently working, hope for the next release
				m_iNumActiveUniqueUnits++;
*/
			if (pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				m_iNumLandUnits++;

				if (pLoopUnit->getArmyID() != -1)
					m_iNumLandUnitsInArmies++;

				if (pLoopUnit->IsCanAttackRanged()) // still counts all land ranged
					m_iNumRangedLandUnits++;
					if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER", true) && pLoopUnit->getUnitInfo().IsMounted() == false)
						m_iNumArcherLandUnits++;
					else if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER", true) && pLoopUnit->getUnitInfo().IsMounted() == true)
						m_iNumSkirmisherLandUnits++;
					else if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SIEGE", true))
						m_iNumSiegeLandUnits++;
				else if (pLoopUnit->canIntercept())
					m_iNumAntiAirUnits++;
				else if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MOUNTED", true) || pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARMOR", true))
					m_iNumMobileLandUnits++;
				else if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_RECON", true))
					m_iNumReconLandUnits++;
				else
					m_iNumMeleeLandUnits++;
			}
			else if (pLoopUnit->getDomainType() == DOMAIN_SEA)
			{
				m_iNumNavalUnits++;

				if (pLoopUnit->getArmyID() != -1)
					m_iNumNavalUnitsInArmies++;

				if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_NAVALMELEE", true))
					m_iNumMeleeNavalUnits++;
				else if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_NAVALRANGED", true))
					m_iNumRangedNavalUnits++;
				else if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SUBMARINE", true))
					m_iNumSubmarineNavalUnits++;

				if (pLoopUnit->AI_getUnitAIType() == UNITAI_CARRIER_SEA)
					m_iNumCarrierNavalUnits++;
					//a carrier is considered free if it is not in a strike group or empty
					if (pLoopUnit->getArmyID() == -1 || pLoopUnit->getCargo() == 0)
							m_iNumFreeCarriers++;
			}
			else if (pLoopUnit->getDomainType() == DOMAIN_AIR && !pLoopUnit->isSuicide())
			{
				m_iNumAirUnits++;

				if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_BOMBER", true))
					m_iNumBomberAirUnits++;
				else if (pLoopUnit->getUnitCombatType() == (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_FIGHTER", true))
					m_iNumFighterAirUnits++;
			}
			else if (pLoopUnit->getDomainType() == DOMAIN_AIR && pLoopUnit->isSuicide()) // missiles&bombs
			{
/*				if (pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_ICBM)
					m_iNumNukeUnits++; //both atomic bomb&nuclear missile, but already counted by CvPlayer */
				if (pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_MISSILE_AIR)
					m_iNumMissileUnits++;
			}
		}
	}

	// finally check how many units we want
	SetRecommendedArmyNavySize();
}

///	How many military units should we have given current threats?
void CvMilitaryAI::SetRecommendedArmyNavySize()
{
	int iNumUnitsWantedDefense = /*3*/ GD_INT_GET(AI_STRATEGY_DEFEND_MY_LANDS_BASE_UNITS);
	int	iMinNumUnits = /*3*/ GD_INT_GET(BALANCE_ARMY_NAVY_START_SIZE); //for each category

	//now check how many units we want for defense
	//these are only land units!
	//offensive units and navy are checked later
	int iFlavorDefense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DEFENSE"));
	int iModifier = 100 + iFlavorDefense*2;

	// 1 Unit per City & 1 per Settler
	iNumUnitsWantedDefense += (int)(m_pPlayer->getNumCities() * /*1.0f*/ GD_FLOAT_GET(AI_STRATEGY_DEFEND_MY_LANDS_UNITS_PER_CITY));
	iNumUnitsWantedDefense += m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);

	// tall players have few cities but many wonders
	iNumUnitsWantedDefense += min(5, (m_pPlayer->GetNumWonders() / 3));

	int iNumCoastalCities = 0;
	int iLoop;
	for(CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		//need this later
		if (pCity->isCoastal())
			iNumCoastalCities++;

		//additional units if the enemy is likely to attack here
		if (m_pPlayer->GetMilitaryAI()->IsExposedToEnemy(pCity,NO_PLAYER))
			iNumUnitsWantedDefense++;
	}

	// put it together
	m_iRecDefensiveLandUnits = max(iMinNumUnits,(iNumUnitsWantedDefense*iModifier)/100);

	// how many units can we afford?
	int iMaxOffensiveUnitsPossible = max(0, m_pPlayer->GetNumUnitsSupplied() - m_iRecDefensiveLandUnits);

	// offense is simple for minors
	if (m_pPlayer->isMinorCiv())
	{
		m_iRecOffensiveLandUnits = 0;
		m_iRecOffensiveNavalUnits = 0;
		if (m_pPlayer->getCapitalCity() && m_pPlayer->getCapitalCity()->isCoastal())
			m_iRecOffensiveNavalUnits = 2;
		return;
	}

	int iFlavorOffense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	// this is the total number for offense, we split between land and sea later
	// in practise some defensive land units can be used as offensive land units as well ...
	int iNumUnitsWantedOffense = m_pPlayer->GetCurrentEra() + m_pPlayer->GetDiplomacyAI()->GetBoldness()/2 + iFlavorOffense/2;

	//Look at our competitors ...
	if (m_pPlayer->isMajorCiv())
	{
		for (int iMajorLoop = 0; iMajorLoop < MAX_MAJOR_CIVS; iMajorLoop++)
		{
			PlayerTypes eOtherPlayer = (PlayerTypes) iMajorLoop;
			if (eOtherPlayer != m_pPlayer->GetID())
			{
				//one army per target
				if (GetPlayer()->GetMilitaryAI()->HavePreferredAttackTarget(eOtherPlayer))
					iNumUnitsWantedOffense += /*6*/ GD_INT_GET(BALANCE_BASIC_ATTACK_ARMY_SIZE);
			}
		}
	}

	// if we are going for conquest we want at least one more task force, more in later eras
	if (m_pPlayer->isMajorCiv() && m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest())
		iNumUnitsWantedOffense += /*6*/ GD_INT_GET(BALANCE_BASIC_ATTACK_ARMY_SIZE);

	// now how many should be naval units?
	EconomicAIStrategyTypes eStrategyNavalMap = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NAVAL_MAP");
	EconomicAIStrategyTypes eExpandOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	bool bNavalFocus = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyNavalMap) || m_pPlayer->GetEconomicAI()->IsUsingStrategy(eExpandOtherContinents); 

	int iCoastalPercent = (iNumCoastalCities * 100) / max(1,m_pPlayer->getNumCities());
	int iFlavorNaval = m_pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL"));
	int iNavalPercent = (iFlavorNaval > 7 ? 30 : 15) + (bNavalFocus ? 10 : 0) + iCoastalPercent/3;

	//you don't always get what you want ...
	iNumUnitsWantedOffense = min(iNumUnitsWantedOffense, iMaxOffensiveUnitsPossible);

	if (iNumCoastalCities > 0)
		m_iRecOffensiveNavalUnits = max(iMinNumUnits, (iNavalPercent*iMaxOffensiveUnitsPossible) / 100);
	else
		m_iRecOffensiveNavalUnits = 0;

	//the remainder is our offensive land army
	m_iRecOffensiveLandUnits = max(iMinNumUnits, iMaxOffensiveUnitsPossible - m_iRecOffensiveNavalUnits);
}


/// Update how we're doing on defensive units
void CvMilitaryAI::UpdateDefenseState()
{
	if(m_iNumLandUnits < m_iRecDefensiveLandUnits)
	{
		m_eLandDefenseState = DEFENSE_STATE_CRITICAL;
	}
	else if(m_iNumLandUnits < (m_iRecDefensiveLandUnits+m_iRecOffensiveLandUnits) * 3 / 4)
	{
		m_eLandDefenseState = DEFENSE_STATE_NEEDED;
	}
	else if(m_iNumLandUnits < (m_iRecDefensiveLandUnits+m_iRecOffensiveLandUnits) * 5 / 4)
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

	if(m_iNumNavalUnits <= (m_iRecOffensiveNavalUnits / 2))
	{
		m_eNavalDefenseState = DEFENSE_STATE_CRITICAL;
	}
	else if(m_iNumNavalUnits <= m_iRecOffensiveNavalUnits)
	{
		m_eNavalDefenseState = DEFENSE_STATE_NEEDED;
	}
	else if(m_iNumNavalUnits <= m_iRecOffensiveNavalUnits * 5 / 4)
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
			if(pPlot->getRevealedImprovementType(eTeam) == GD_INT_GET(BARBARIAN_CAMP_IMPROVEMENT))
			{
				//count only reasonably close camps
				if (m_pPlayer->GetCityDistancePathLength(pPlot)<17)
					m_iBarbarianCampCount++;
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

/// Start or stop military strategies to get flavors set properly
void CvMilitaryAI::UpdateMilitaryStrategies()
{

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
				else if(strStrategyName == "MILITARYAISTRATEGY_NEED_RANGED" && !MOD_AI_UNIT_PRODUCTION)
					bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedRangedUnits(m_pPlayer, m_iNumRangedLandUnits, m_iNumMeleeLandUnits);
				else if(strStrategyName == "MILITARYAISTRATEGY_ENOUGH_RANGED" && !MOD_AI_UNIT_PRODUCTION)
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

				// MOD_AI_UNIT_PRODUCTION : New strategies
				else if (MOD_AI_UNIT_PRODUCTION)
				{
					if (strStrategyName == "MILITARYAISTRATEGY_NEED_ARCHER")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedArcherUnits(m_pPlayer, m_iNumArcherLandUnits, m_iNumMeleeLandUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_ENOUGH_ARCHER")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughArcherUnits(m_pPlayer, m_iNumArcherLandUnits, m_iNumMeleeLandUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_NEED_SIEGE")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedSiegeUnits(m_pPlayer, m_iNumSiegeLandUnits, m_iNumMeleeLandUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_ENOUGH_SIEGE")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughSiegeUnits(m_pPlayer, m_iNumSiegeLandUnits, m_iNumMeleeLandUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_NEED_SKIRMISHER")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedSkirmisherUnits(m_pPlayer, m_iNumSkirmisherLandUnits, m_iNumMeleeLandUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_ENOUGH_SKIRMISHER")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughSkirmisherUnits(m_pPlayer, m_iNumSkirmisherLandUnits, m_iNumMeleeLandUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_NEED_NAVAL_MELEE")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedNavalMeleeUnits(m_pPlayer, m_iNumMeleeNavalUnits, m_iNumNavalUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_ENOUGH_NAVAL_MELEE")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughNavalMeleeUnits(m_pPlayer, m_iNumMeleeNavalUnits, m_iNumNavalUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_NEED_NAVAL_RANGED")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedNavalRangedUnits(m_pPlayer, m_iNumRangedNavalUnits, m_iNumNavalUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_ENOUGH_NAVAL_RANGED")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughNavalRangedUnits(m_pPlayer, m_iNumRangedNavalUnits, m_iNumNavalUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_NEED_SUBMARINE")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedSubmarineUnits(m_pPlayer, m_iNumSubmarineNavalUnits, m_iNumNavalUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_ENOUGH_SUBMARINE")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughSubmarineUnits(m_pPlayer, m_iNumSubmarineNavalUnits, m_iNumNavalUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_NEED_BOMBER")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedBomberUnits(m_pPlayer, m_iNumBomberAirUnits, m_iNumAirUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_ENOUGH_BOMBER")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughBomberUnits(m_pPlayer, m_iNumBomberAirUnits, m_iNumAirUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_NEED_FIGHTER")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_NeedFighterUnits(m_pPlayer, m_iNumFighterAirUnits, m_iNumAirUnits);
					else if (strStrategyName == "MILITARYAISTRATEGY_ENOUGH_FIGHTER")
						bStrategyShouldBeActive = MilitaryAIHelpers::IsTestStrategy_EnoughFighterUnits(m_pPlayer, m_iNumFighterAirUnits, m_iNumAirUnits);

				}
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
	StrengthTypes eMilitaryStrength = m_pPlayer->GetDiplomacyAI()->GetPlayerMilitaryStrengthComparedToUs(ePlayer);
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
		else if (eMilitaryStrength == STRENGTH_IMMENSE || eCurrentWarState == WAR_STATE_NEARLY_DEFEATED)
		{
			bLaunchNuke = true;
		}
		else 
		{
			bool bRollForNuke = false;
			if (GET_PLAYER(ePlayer).isMajorCiv())
			{
				CivOpinionTypes eCivOpinion = m_pPlayer->GetDiplomacyAI()->GetCivOpinion(ePlayer);
				if (eMilitaryStrength == STRENGTH_POWERFUL || eCurrentWarState == WAR_STATE_DEFENSIVE)
				{
					// roll every turn
					bRollForNuke = true;
				}
				else if (eCivOpinion <= CIV_OPINION_ENEMY)
				{
					bRollForNuke = true;
				}
				else if (m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest() || m_pPlayer->GetDiplomacyAI()->IsCloseToWorldConquest())
				{
					bRollForNuke = true;
				}
				else if (GET_PLAYER(ePlayer).GetDiplomacyAI()->IsCloseToAnyVictoryCondition())
				{
					bRollForNuke = true;
				}
				if (bRollForNuke)
				{
					int iFlavorNuke = m_pPlayer->GetFlavorManager()->GetPersonalityFlavorForDiplomacy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_USE_NUKE"));
					int iRoll = GC.getGame().getSmallFakeRandNum(10, GET_PLAYER(ePlayer).GetPseudoRandomSeed());
					int iRoll2 = GC.getGame().getSmallFakeRandNum(10, m_pPlayer->GetPseudoRandomSeed());
					if (iRoll <= iFlavorNuke && iRoll2 <= iFlavorNuke)
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

void CvMilitaryAI::SetupInstantDefenses(PlayerTypes ePlayer)
{
	if(ePlayer == NO_PLAYER)
		return;

	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;

		// Open the right file
		CvString strPlayerName = GetPlayer()->getCivilizationShortDescription();
		CvString strOpponentName = GET_PLAYER(ePlayer).getCivilizationShortDescription();
		FILogFile *pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";

		// Strategy Info
		strTemp.Format("War Declared! ***** %s ****** Setting up defenses.", strOpponentName.GetCString());
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}

	//land response
	vector<CvCity*> allCities = m_pPlayer->GetThreatenedCities(false);
	CvCity* pMostThreatenedCity = allCities.empty() ? NULL : allCities.front();
	if (pMostThreatenedCity != NULL && !m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, ePlayer))
		m_pPlayer->addAIOperation(AI_OPERATION_RAPID_RESPONSE, 0, ePlayer, pMostThreatenedCity);

	//naval response
	vector<CvCity*> coastCities = m_pPlayer->GetThreatenedCities(true);
	CvCity* pMostThreatenedCoastalCity = coastCities.empty() ? NULL : coastCities.front();
	if (pMostThreatenedCoastalCity != NULL && !m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_NAVAL_SUPERIORITY, ePlayer))
		m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, 0, ePlayer, pMostThreatenedCoastalCity);
}

/// Abort or start operations as appropriate given the current threats and war states
void CvMilitaryAI::CheckLandDefenses(PlayerTypes eEnemy, CvCity* pThreatenedCity)
{
	if(eEnemy == NO_PLAYER || !pThreatenedCity)
		return;
	
	WarStateTypes eWarState = GET_PLAYER(eEnemy).isMajorCiv() ? m_pPlayer->GetDiplomacyAI()->GetWarState(eEnemy) : WAR_STATE_OFFENSIVE;

	if (eWarState >= WAR_STATE_STALEMATE)
		// If we're winning, nothing to do
		return;
	else if (eWarState == WAR_STATE_DEFENSIVE)
		// If we are losing, concentrate on defense against this player
		m_pPlayer->StopAllLandOffensiveOperationsAgainstPlayer(eEnemy, AI_ABORT_WAR_STATE_CHANGE);
	else if (eWarState == WAR_STATE_NEARLY_DEFEATED)
		// If we are really losing, let's pull back everywhere.	
		m_pPlayer->StopAllLandOffensiveOperationsAgainstPlayer(NO_PLAYER, AI_ABORT_WAR_STATE_CHANGE);

	//first a quick one
	bool bHasOperationUnderway = m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_RAPID_RESPONSE, eEnemy) != NULL;
	CvPlot* pStartPlot = OperationalAIHelpers::FindEnemiesNearHomelandPlot(m_pPlayer->GetID(), eEnemy, DOMAIN_LAND, pThreatenedCity->plot());
	if (!bHasOperationUnderway && pStartPlot != NULL && pStartPlot->getOwningCity() != NULL)
		m_pPlayer->addAIOperation(AI_OPERATION_RAPID_RESPONSE, 1, eEnemy, pStartPlot->getOwningCity());

	//this may take a bit longer to arrange
	bool bIsEnemyZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pThreatenedCity->plot());
	if (bIsEnemyZone && m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_CITY_DEFENSE, eEnemy, pThreatenedCity->plot())==NULL)
		m_pPlayer->addAIOperation(AI_OPERATION_CITY_DEFENSE, 2, eEnemy, pThreatenedCity);
}

/// Abort or start operations as appropriate given the current threats and war states
void CvMilitaryAI::CheckSeaDefenses(PlayerTypes ePlayer, CvCity* pThreatenedCity)
{
	if(ePlayer == NO_PLAYER || !pThreatenedCity)
		return;

	WarStateTypes eWarState = GET_PLAYER(ePlayer).isMajorCiv() ? m_pPlayer->GetDiplomacyAI()->GetWarState(ePlayer) : WAR_STATE_OFFENSIVE;
	if (eWarState >= WAR_STATE_STALEMATE)
		return;
	else if (eWarState ==  WAR_STATE_DEFENSIVE)
		m_pPlayer->StopAllSeaOffensiveOperationsAgainstPlayer(ePlayer, AI_ABORT_WAR_STATE_CHANGE);
	else if (eWarState == WAR_STATE_NEARLY_DEFEATED)
		m_pPlayer->StopAllSeaOffensiveOperationsAgainstPlayer(NO_PLAYER, AI_ABORT_WAR_STATE_CHANGE);

	if (!pThreatenedCity->isCoastal())
		return;

	CvPlot* pCoastalPlot = MilitaryAIHelpers::GetCoastalWaterNearPlot(pThreatenedCity->plot());
	if(pCoastalPlot != NULL)
	{
		bool bIsEnemyZone = m_pPlayer->GetTacticalAI()->GetTacticalAnalysisMap()->IsInEnemyDominatedZone(pThreatenedCity->plot());
		bool bHasOperationUnderway = m_pPlayer->getFirstAIOperationOfType(AI_OPERATION_NAVAL_SUPERIORITY, ePlayer, pThreatenedCity->plot()) != NULL;
		if (!bHasOperationUnderway || bIsEnemyZone)
			m_pPlayer->addAIOperation(AI_OPERATION_NAVAL_SUPERIORITY, 1, ePlayer, pThreatenedCity);
	}
}

void CvMilitaryAI::DoCityAttacks(PlayerTypes ePlayer)
{
	//Not perfect, as some operations are mixed, but it will keep us from sending everyone to slaughter all at once.
	int iReservesTotal = ((m_iNumLandUnits + m_iNumNavalUnits) - (m_iNumNavalUnitsInArmies + m_iNumLandUnitsInArmies));
	if (iReservesTotal >= m_iRecDefensiveLandUnits || (m_pPlayer->GetNumOffensiveOperations(DOMAIN_LAND)+m_pPlayer->GetNumOffensiveOperations(DOMAIN_SEA)) <= 0)
	{
		WarStateTypes eWarState = GET_PLAYER(ePlayer).isMajorCiv() ? m_pPlayer->GetDiplomacyAI()->GetWarState(ePlayer) : WAR_STATE_OFFENSIVE;
		if(eWarState >= WAR_STATE_STALEMATE)
		{
			RequestCityAttack(ePlayer, 2);
			if (GET_PLAYER(ePlayer).isMajorCiv())
				RequestPillageAttack(ePlayer);
		}
	}
}

/// Abort or start operations as appropriate given the current threats and war states
void CvMilitaryAI::UpdateOperations()
{
	//only major players set up operations
	if(!m_pPlayer->isMajorCiv())
		return;

	vector<CvCity*> allCities = m_pPlayer->GetThreatenedCities(false);
	CvCity* pThreatenedCityA = allCities.size()<1 ? NULL : allCities[0];
	CvCity* pThreatenedCityB = allCities.size()<2 ? NULL : allCities[1];
	vector<CvCity*> coastCities = m_pPlayer->GetThreatenedCities(true);
	CvCity* pThreatenedCoastalCityA = coastCities.size()<1 ? NULL : coastCities[0];
	CvCity* pThreatenedCoastalCityB = coastCities.size()<2 ? NULL : coastCities[1];

	// Are any of our strategies inappropriate given the type of war we are fighting
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes) iPlayerLoop;

		// Is this a player we have relations with?
		if(eLoopPlayer != m_pPlayer->GetID() && IsPlayerValid(eLoopPlayer))
		{
			// If we've made peace, abort all operations
			if(GET_TEAM(m_pPlayer->getTeam()).isForcePeace(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				m_pPlayer->StopAllLandOffensiveOperationsAgainstPlayer(eLoopPlayer,AI_ABORT_WAR_STATE_CHANGE);
				m_pPlayer->StopAllSeaOffensiveOperationsAgainstPlayer(eLoopPlayer,AI_ABORT_WAR_STATE_CHANGE);
				m_pPlayer->StopAllLandDefensiveOperationsAgainstPlayer(eLoopPlayer,AI_ABORT_WAR_STATE_CHANGE);
				m_pPlayer->StopAllSeaDefensiveOperationsAgainstPlayer(eLoopPlayer,AI_ABORT_WAR_STATE_CHANGE);
			}

			// If we cannot declare war on this player, abort all offensive operations related to him
			if(!m_pPlayer->IsAtWarWith(eLoopPlayer) && !GET_TEAM(m_pPlayer->getTeam()).canDeclareWar(GET_PLAYER(eLoopPlayer).getTeam()))
			{
				m_pPlayer->StopAllLandOffensiveOperationsAgainstPlayer(eLoopPlayer,AI_ABORT_TARGET_NOT_VALID);
				m_pPlayer->StopAllSeaOffensiveOperationsAgainstPlayer(eLoopPlayer,AI_ABORT_TARGET_NOT_VALID);
			}

			// if we're at war with this player
			if (m_pPlayer->IsAtWarWith(eLoopPlayer))
			{
				//Defense check.
				if (pThreatenedCoastalCityA == NULL)
					m_pPlayer->StopAllSeaDefensiveOperationsAgainstPlayer(eLoopPlayer, AI_ABORT_WAR_STATE_CHANGE);

				if(pThreatenedCityA == NULL)
					m_pPlayer->StopAllLandDefensiveOperationsAgainstPlayer(eLoopPlayer, AI_ABORT_WAR_STATE_CHANGE);

				CheckLandDefenses(eLoopPlayer,pThreatenedCityA);
				CheckLandDefenses(eLoopPlayer,pThreatenedCityB);

				CheckSeaDefenses(eLoopPlayer, pThreatenedCoastalCityA);
				CheckSeaDefenses(eLoopPlayer, pThreatenedCoastalCityB);

				//finally offense
				DoNuke(eLoopPlayer);
				DoCityAttacks(eLoopPlayer);
			}
		}
	}

	//if we have an idle carrier, try to start a strike group. the carrier may be empty!
	int iLoop;
	for(CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if(pLoopUnit->AI_getUnitAIType() == UNITAI_CARRIER_SEA && pLoopUnit->getArmyID() == -1 && !pLoopUnit->shouldHeal())
		{
			//the operation will find it's own target and remain active until indefinitely.
			//the airplanes rebase independently, they are not part of the operation.
			m_pPlayer->addAIOperation(AI_OPERATION_CARRIER_GROUP, 0, NO_PLAYER, NULL, NULL);
			break;
		}
	}
}

/// Spend money on units/buildings for military contingencies
//  NOTE: The defensive side of this is done in dominance zone processing in the Tactical AI; this is spending to speed operations
void CvMilitaryAI::MakeEmergencyPurchases()
{

	// Are we winning all the wars we are in?
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(!IsUsingStrategy(eStrategyAtWar) || m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		// Is there an operation waiting for one more unit?
		for (size_t i=0; i<m_pPlayer->getNumAIOperations(); i++)
		{
			CvAIOperation* pOp = m_pPlayer->getAIOperationByIndex(i);
			// Can we buy a unit to fill that slot?
			if(pOp->BuyFinalUnit())
				break;
		}
	}
}

/// Delete older units no longer needed by military AI
void CvMilitaryAI::DisbandObsoleteUnits()
{

	bool bInDeficit = false;
	bool bConquestGrandStrategy = false;

	if (m_pPlayer->isBarbarian())
		return;

	if (GC.getGame().getGameTurn() <= 25)
		return;

	// Don't do this if at war
	if(GetNumberCivsAtWarWith(false) > 0)
	{
		if (m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
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
		if (m_pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest())
		{
			bConquestGrandStrategy = true;
		}
	}

	// do we have way too many units?
	int iMaxExcessUnits = bConquestGrandStrategy ? 3 : 1;
	bool bOverSupplyCap = (m_pPlayer->GetNumUnitsOutOfSupply(false) > iMaxExcessUnits);

	CvUnit* pScrapUnit = FindUselessShip();

	//if we don't have an easy candidate but are forced to act
	if (!pScrapUnit && (bInDeficit||bOverSupplyCap))
	{
		//check our options
		int iLandScore = MAX_INT;
		int iNavalScore = MAX_INT;
		CvUnit* pNavalUnit = FindUnitToScrap(DOMAIN_SEA, true, iNavalScore, bOverSupplyCap, bInDeficit);
		CvUnit* pLandUnit = FindUnitToScrap(DOMAIN_LAND, true, iLandScore, bOverSupplyCap, bInDeficit);

		//keep the more useful one
		pScrapUnit = (iLandScore < iNavalScore) ? pLandUnit : pNavalUnit;
	}

	if (pScrapUnit)
	{
		// Don't do this if we're a minor civ
		if (!m_pPlayer->isMinorCiv())
		{
			PlayerTypes eMinor = m_pPlayer->GetBestGiftTarget(pScrapUnit->getDomainType());
			if (eMinor != NO_PLAYER)
			{
				LogGiftUnit(pScrapUnit, bInDeficit, bOverSupplyCap);
				GET_PLAYER(eMinor).AddIncomingUnit(m_pPlayer->GetID(), pScrapUnit);
				pScrapUnit = NULL;
			}
		}

		if (pScrapUnit)
		{
			LogScrapUnit(pScrapUnit, bInDeficit, bOverSupplyCap);
			if (pScrapUnit->canScrap())
				pScrapUnit->scrap();
			else
				pScrapUnit->kill(true);
		}
	}
}

bool NeedShipInArea(PlayerTypes ePlayer, CvArea* pWaterBody)
{
	if (pWaterBody)
	{
		int iForeignCities = pWaterBody->getNumCities() - pWaterBody->getCitiesPerPlayer(ePlayer);
		int iForeignUnits = pWaterBody->getNumUnits() - pWaterBody->getUnitsPerPlayer(ePlayer);
		bool bTooManyUnits = (pWaterBody->getNumTiles() < pWaterBody->getUnitsPerPlayer(ePlayer) * /*6*/ GD_INT_GET(AI_CONFIG_MILITARY_TILES_PER_SHIP));

		if (!bTooManyUnits)
			if (iForeignCities > 0 || iForeignUnits > 0)
				return true;
	}

	return false;
}



CvUnit* CvMilitaryAI::FindUselessShip()
{
	vector<CvUnit*> candidates;

	int iUnitLoop;
	for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		if (!pLoopUnit->IsCombatUnit())
			continue;

		if (!pLoopUnit->canScrap())
			continue;

		// Is this a ship on a water body without enemies and without exits?
		if (pLoopUnit->getDomainType() == DOMAIN_SEA)
		{
			//check the current area first
			if (NeedShipInArea(m_pPlayer->GetID(), pLoopUnit->plot()->area()))
				continue;

			candidates.push_back(pLoopUnit);
		}
	}

	//sort ships by experience: we want to scrap the veterans last ...
	struct PrSortByExperience
	{
		bool operator()(const CvUnit* lhs, const CvUnit* rhs) const { return lhs->getExperienceTimes100() < rhs->getExperienceTimes100(); }
	};
	std::sort( candidates.begin(), candidates.end(), PrSortByExperience() );

	//check other areas we can reach before deciding
	for (size_t i=0; i<candidates.size(); i++)
	{
		CvUnit* pLoopUnit = candidates[i];

		//assume it's useless until proven otherwise
		bool bIsUseless = true;

		//two turns is a good compromise between reliability and performance 
		SPathFinderUserData data(pLoopUnit, CvUnit::MOVEFLAG_IGNORE_STACKING | CvUnit::MOVEFLAG_IGNORE_ENEMIES | CvUnit::MOVEFLAG_IGNORE_RIGHT_OF_PASSAGE, 2);
		ReachablePlots plots = GC.GetPathFinder().GetPlotsInReach(pLoopUnit->getX(), pLoopUnit->getY(), data);
		set<int> areas;

		for (ReachablePlots::iterator it = plots.begin(); it != plots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(it->iPlotIndex);
			if (pPlot && pPlot->isWater())
				areas.insert(pPlot->getArea());
		}
		for (set<int>::iterator it = areas.begin(); it != areas.end(); ++it)
		{
			CvArea* pWaterBody = GC.getMap().getArea(*it);
			if (NeedShipInArea(m_pPlayer->GetID(), pWaterBody))
			{
				bIsUseless = false;
				break;
			}
		}

		if (bIsUseless)
			return pLoopUnit;
	}

	return NULL;
}

/// Score the strength of the units for a domain; best candidate to scrap (with lowest score) is returned. Only supports land and naval units
CvUnit* CvMilitaryAI::FindUnitToScrap(DomainTypes eDomain, bool bCheckObsolete, int& iReturnedScore, bool bOverSupplyCap, bool bInDeficit)
{
	int iUnitLoop;
	CvUnit* pBestUnit = NULL;
	int iBestScore = MAX_INT;

	for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		//needed later
		CvUnitEntry& pUnitInfo = pLoopUnit->getUnitInfo();

		if(!pLoopUnit->IsCombatUnit())
			continue;

		if (!pLoopUnit->canScrap())
			continue;

		// don't delete no maintenance units if we're scrapping due to deficit
		// don't delete no supply units if we're scrapping due to oversupply
		if (bInDeficit && bOverSupplyCap)
		{
			if (pLoopUnit->IsNoMaintenance() && pLoopUnit->isNoSupply())
				continue;
		}
		else if (bInDeficit && pLoopUnit->IsNoMaintenance())
			continue;
		else if (bOverSupplyCap && pLoopUnit->isNoSupply())
			continue;

		//Failsafe to keep AI from deleting advanced start settlers
		//Probably useless because of the combat unit check above
		if (m_pPlayer->GetNumCitiesFounded() < 3)
			if (pUnitInfo.IsFound() || pUnitInfo.IsFoundAbroad())
				continue;

		if(eDomain != NO_DOMAIN && pLoopUnit->getDomainType() != eDomain)
			continue;

		//don't kill our explorers if we need them
		if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA && m_pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_NEEDED)
			continue;

		//don't kill our explorers if we need them
		if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE && m_pPlayer->GetEconomicAI()->GetReconState() == RECON_STATE_NEEDED)
			continue;

		// Do we need it to fight?
		if (!pLoopUnit->canUseForAIOperation())
			continue;

		bool bIsObsolete = (TechTypes)pUnitInfo.GetObsoleteTech() != NO_TECH && GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)(pUnitInfo.GetObsoleteTech()));
		if (bIsObsolete && bCheckObsolete)
		{
			UnitTypes eUpgradeUnit = pLoopUnit->GetUpgradeUnitType();
			if (eUpgradeUnit != NO_UNIT)
			{
				//ok nice looks like we can upgrade
				bIsObsolete = false;

				//but does this unit's upgrade require additional resources we don't have?
				CvUnitEntry* pUpgradeUnitInfo = GC.GetGameUnits()->GetEntry(eUpgradeUnit);
				if (pUpgradeUnitInfo != NULL)
				{
					for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
					{
						ResourceTypes eResource = (ResourceTypes)iResourceLoop;
						int iNumResourceNeeded = pUpgradeUnitInfo->GetResourceQuantityRequirement(eResource);
						int iNumResourceInUse = pLoopUnit->getUnitInfo().GetResourceQuantityRequirement(eResource);
						if (iNumResourceNeeded - iNumResourceInUse > m_pPlayer->getNumResourceTotal(eResource))
							bIsObsolete = true;

						if (MOD_UNITS_RESOURCE_QUANTITY_TOTALS)
						{
							if (pUpgradeUnitInfo->GetResourceQuantityTotal(eResource) > m_pPlayer->getNumResourceTotal(eResource))
								bIsObsolete = true;
						}
					}
				}
			}
		}

		bool bResourceDeficit = false;
		for (int iResourceLoop = 0; iResourceLoop < GC.getNumResourceInfos(); iResourceLoop++)
		{
			ResourceTypes eResource = (ResourceTypes)iResourceLoop;
			int iNumResourceInUse = pLoopUnit->getUnitInfo().GetResourceQuantityRequirement(eResource);
			if (iNumResourceInUse > 0 && m_pPlayer->getNumResourceAvailable(eResource) < 0)
			{
				bResourceDeficit = true;
				break;
			}
		}

		// Can I scrap this unit?
		if (!bCheckObsolete || bIsObsolete || bResourceDeficit)
		{
			int iScore = pLoopUnit->GetPower()*pLoopUnit->getUnitInfo().GetProductionCost() + pLoopUnit->getLevel(); //tiebreaker
			if (bResourceDeficit)
				iScore /= 2;

			if (iScore < iBestScore)
			{
				iBestScore = iScore;
				iReturnedScore = iBestScore;
				pBestUnit = pLoopUnit;
			}
		}
	}

	return pBestUnit;
}

bool CvMilitaryAI::IsBuildingArmy(ArmyType eType) const
{
	int iLoop;
	for (CvArmyAI* pLoopArmyAI = m_pPlayer->firstArmyAI(&iLoop); pLoopArmyAI != NULL; pLoopArmyAI = m_pPlayer->nextArmyAI(&iLoop))
	{
		if (pLoopArmyAI->GetType() == eType || eType == ARMY_TYPE_ANY)
			if (!pLoopArmyAI->GetOpenSlots(true).empty())
				return true;
	}

	return false;
}

/// Which unit should be built next for our army
UnitTypes CvMilitaryAI::GetUnitTypeForArmy(CvCity* pCity) const
{
	int iLoop;
	for (CvArmyAI* pLoopArmyAI = m_pPlayer->firstArmyAI(&iLoop); pLoopArmyAI != NULL; pLoopArmyAI = m_pPlayer->nextArmyAI(&iLoop))
	{
		vector<size_t> vOpenSlots = pLoopArmyAI->GetOpenSlots(true);
		for (size_t i = 0; i < vOpenSlots.size(); i++)
		{
			CvFormationSlotEntry slot = pLoopArmyAI->GetSlotInfo(vOpenSlots[i]);
			UnitTypes eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(slot.m_primaryUnitType, true);

			if (eType == NO_UNIT)
				eType = pCity->GetCityStrategyAI()->GetUnitProductionAI()->RecommendUnit(slot.m_secondaryUnitType, true);

			if (eType != NO_UNIT)
				return eType;
		}
	}

	return NO_UNIT;
}

/// Assess nearby enemy air assets
int CvMilitaryAI::GetNumEnemyAirUnitsInRange(CvPlot* pCenterPlot, int iRange, bool bCountFighters, bool bCountBombers) const
{
	int iRtnValue = 0;

	// Loop through all players' Units (that we're at war with) to see if they can intercept
	const std::vector<PlayerTypes>& vEnemies = m_pPlayer->GetPlayersAtWarWith();

	for (size_t iI = 0; iI < vEnemies.size(); iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER(vEnemies[iI]);

		// Loop through their units looking for bombers (this will allow us to find bombers on carriers also
		int iLoopUnit = 0;
		for(CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoopUnit))
		{
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				// Just to keep fighters closer to high range bombers (stealth bombers)
				int iAcceptableDistance = min(pLoopUnit->GetRange(), 12) + (iRange / 2);
				// distance was checked to a fixed 10 value
				if (plotDistance(pCenterPlot->getX(), pCenterPlot->getY(), pLoopUnit->getX(), pLoopUnit->getY()) <= iAcceptableDistance)
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
				if(m_pPlayer->canTrainUnit(eUnit, false /*bContinue*/, false /*bTestVisible*/, true /*bIgnoreCost*/))
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
		FILogFile* pLog;

		// Open the right file
		playerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName,true), FILogFile::kDontTimeStamp);

		// Very first update (to write header row?)
		if(GC.getGame().getElapsedGameTurns() == 0 && m_pPlayer->GetID() == 0)
		{
			if (!MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
			{
				strTemp.Format("Turn, Player, Era, Cities, Settlers, LandUnits, LandArmySize, RecLandOffensive, RecLandDefensive, NavalUnits, NavalArmySize, RecNavyOffensive, AirUnits, AntiAirUnits, RecTotal, MilitaryUnits, SupplyLimit, OutOfSupply, NoSupplyUnits, WarCount, MostEndangeredCity, Danger");
			}
			else
			{
				strTemp.Format("Turn, Player, Era, Cities, Settlers, LandUnits, LandArmySize, RecLandOffensive, RecLandDefensive, NavalUnits, NavalArmySize, RecNavyOffensive, MeleeUnits, MobileUnits, ReconUnits, ArcherUnits, SiegeUnits, SkirmisherUnits, AllLandRanged, AntiAirUnits, MeleeNavalUnits, RangedNavalUnits, Submarines, Carriers, TotalAirUnits, BomberUnits, FighterUnits, Nukes, Missiles, RecTotal, TotalMilitaryUnits, SupplyLimit, OutOfSupply, WarWearinessSupplyReduction, NoSupplyUnits, WarCount, MostEndangeredCity, Danger");
			}
			pLog->Msg(strTemp);
		}

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// City info
		strTemp.Format("%d, %d, %d, ", m_pPlayer->GetCurrentEra(), m_pPlayer->getNumCities(), m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true));
		strOutBuf = strBaseString + strTemp;

		// Military size Info
		if (!MOD_BALANCE_CORE_MILITARY_PROMOTION_ADVANCED)
		{
			strTemp.Format("%d, %d, %d, %d, %d, %d, %d, %d, %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecOffensiveLandUnits, m_iRecDefensiveLandUnits, m_iNumNavalUnits, m_iNumNavalUnitsInArmies, m_iRecOffensiveNavalUnits, m_iNumAirUnits, m_iNumAntiAirUnits);
		}
		else
		{
			strTemp.Format("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, m_iRecOffensiveLandUnits, m_iRecDefensiveLandUnits, m_iNumNavalUnits, m_iNumNavalUnitsInArmies, m_iRecOffensiveNavalUnits, m_iNumMeleeLandUnits, m_iNumMobileLandUnits, m_iNumReconLandUnits, m_iNumArcherLandUnits, m_iNumSiegeLandUnits, m_iNumSkirmisherLandUnits, m_iNumRangedLandUnits, m_iNumAntiAirUnits, m_iNumMeleeNavalUnits, m_iNumRangedNavalUnits, m_iNumSubmarineNavalUnits, m_iNumCarrierNavalUnits, m_iNumAirUnits, m_iNumBomberAirUnits, m_iNumFighterAirUnits, m_pPlayer->getNumNukeUnits(), m_iNumMissileUnits);
		}
		strOutBuf += strTemp;

		// Unit supply
		strTemp.Format("%d, %d, %d, %d, %d, %d, %d, ", 
			GetRecommendedMilitarySize(), m_pPlayer->getNumMilitaryUnits(), m_pPlayer->GetNumUnitsSupplied(), m_pPlayer->GetNumUnitsOutOfSupply(), m_pPlayer->GetNumUnitsOutOfSupply(false) - m_pPlayer->GetNumUnitsOutOfSupply(),
			m_pPlayer->getNumUnitsSupplyFree(), GetNumberCivsAtWarWith(false)); //adjusted for better readability
		strOutBuf += strTemp;

		// Most threatened city
		vector<CvCity*> threatCities = m_pPlayer->GetThreatenedCities(false);
		pCity = threatCities.empty() ? NULL : threatCities.front();
		if(pCity != NULL)
		{
			strOutBuf += pCity->getNameNoSpace();
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
		int iCapitalX = 0;
		int iCapitalY = 0;
		CvCity* pCapital = GetPlayer()->getCapitalCity();
		if(pCapital)
		{
			iCapitalX = pCapital->getX();
			iCapitalY = pCapital->getY();
		}

		// Open the right file
		CvString playerName = GetPlayer()->getCivilizationShortDescription();
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		if (IsBuildingArmy(ARMY_TYPE_LAND))
		{
			CvString strOutBuf = strBaseString + ArmyTypeToString(ARMY_TYPE_LAND) + " under construction";
			pLog->Msg(strOutBuf);
		}
		if (IsBuildingArmy(ARMY_TYPE_NAVAL))
		{
			CvString strOutBuf = strBaseString + ArmyTypeToString(ARMY_TYPE_NAVAL) + " under construction";
			pLog->Msg(strOutBuf);
		}
		if (IsBuildingArmy(ARMY_TYPE_COMBINED))
		{
			CvString strOutBuf = strBaseString + ArmyTypeToString(ARMY_TYPE_COMBINED) + " under construction";
			pLog->Msg(strOutBuf);
		}

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
			strTemp.Format("Num Land Units: %d, In Armies %d, Rec Size: %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, GetRecommendLandArmySize());
		}
		else
		{
			strTemp.Format("Num Naval Units: %d, In Armies %d, Rec Size: %d ", m_iNumNavalUnits, m_iNumNavalUnitsInArmies, m_iRecOffensiveNavalUnits);
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
			strTemp.Format("Num Land Units: %d, In Armies %d, Rec Size: %d, ", m_iNumLandUnits, m_iNumLandUnitsInArmies, GetRecommendLandArmySize());
		}
		else
		{
			strTemp.Format("Num Naval Units: %d, In Armies %d, Rec: %d ", m_iNumNavalUnits, m_iNumNavalUnitsInArmies, m_iRecOffensiveNavalUnits);
		}
		strOutBuf += strTemp;
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
				if (pLoopUnit->IsCanAttackRanged())
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
				if (pLoopUnit->IsCanAttackRanged())
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
								if (pLoopUnit->IsCanAttackRanged())
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
								if (pLoopUnit->IsCanAttackRanged())
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
								if (pLoopUnit->IsCanAttackRanged())
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
								if (pLoopUnit->IsCanAttackRanged())
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
	if (bInDeficit || !pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest())
	{
		if (pPlayer->GetMilitaryAI()->GetLandDefenseState() == DEFENSE_STATE_ENOUGH)
		{
			return true;
		}
	}
	else
	{
		return false;
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
	if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
	{
		return false;
	}

	// Are we running the Conquest Grand Strategy?
	if (pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest())
	{
		iCurrentWeight += 25;
	}
#if defined(MOD_BALANCE_CORE)
	if(pPlayer->IsCramped())
	{
		iCurrentWeight += 5;
	}
#endif

	if (pPlayer->GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_ANY))
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
		if(pkDiplomacyAI->IsWantsSneakAttack(eOtherPlayer))
		{
			iCurrentWeight += 100;
		}

		// Add in weight for each civ we're on really bad terms with
		else if(pkDiplomacyAI->GetCivApproach(eOtherPlayer) <= CIV_APPROACH_HOSTILE ||
		        pkDiplomacyAI->GetCivApproach(eOtherPlayer) == CIV_APPROACH_AFRAID ||
				pkDiplomacyAI->GetVisibleApproachTowardsUs(eOtherPlayer) <= CIV_APPROACH_HOSTILE ||
				pkDiplomacyAI->IsCapitalCapturedBy(eOtherPlayer) || pkDiplomacyAI->IsHolyCityCapturedBy(eOtherPlayer) ||
				pkDiplomacyAI->GetNumCitiesCapturedBy(eOtherPlayer) > 0 || pkDiplomacyAI->GetNumWarsDeclaredOnUs(eOtherPlayer) > 2)
		{
			iCurrentWeight += 50;
		}

		// And some if on fairly bad terms
		else if(pkDiplomacyAI->GetCivApproach(eOtherPlayer) <= CIV_APPROACH_GUARDED ||
				pkDiplomacyAI->GetVisibleApproachTowardsUs(eOtherPlayer) == CIV_APPROACH_GUARDED ||
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
bool MilitaryAIHelpers::IsTestStrategy_AtWar(CvPlayer* pPlayer, bool bMinor)
{
	if (pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith(bMinor) > 0)
	{
		return true;
	}

	return false;
}

/// "Minor Civ GeneralDefense" Player Strategy: Prioritize CITY_DEFENSE and DEFENSE
bool MilitaryAIHelpers::IsTestStrategy_MinorCivGeneralDefense()
{
	return true;
}

/// "Minor Civ Threat Elevated" Player Strategy: If a Minor Civ is in danger, turn CITY_DEFENSE and DEFENSE up
bool MilitaryAIHelpers::IsTestStrategy_MinorCivThreatElevated(CvPlayer* pPlayer)
{
	if (pPlayer->GetMinorCivAI()->GetStatus() == MINOR_CIV_STATUS_ELEVATED)
	{
		return true;
	}

	return false;
}

/// "Minor Civ Threat Critical" Player Strategy: If a Minor Civ is in danger, turn CITY_DEFENSE and DEFENSE up
bool MilitaryAIHelpers::IsTestStrategy_MinorCivThreatCritical(CvPlayer* pPlayer)
{
	if (pPlayer->GetMinorCivAI()->GetStatus() == MINOR_CIV_STATUS_CRITICAL)
	{
		return true;
	}

	return false;
}

/// "Eradicate Barbarians" Player Strategy: If there is a large group of barbarians units or camps near our civilization, increase OFFENSE
bool MilitaryAIHelpers::IsTestStrategy_EradicateBarbarians(MilitaryAIStrategyTypes eStrategy, CvPlayer* pPlayer, int iBarbarianCampCount, int iVisibleBarbarianCount)
{
	// If we're at war don't bother with this Strategy (unless it is clear we are already winning)
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
	{
		if(!pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
		{
			return false;
		}
	}

	// We ARE getting our guys back
	CvPlayerAI& BarbPlayer = GET_PLAYER(BARBARIAN_PLAYER);
	int iLoop;
	for (CvUnit* pLoopUnit = BarbPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = BarbPlayer.nextUnit(&iLoop))
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
		int iStrategyWeight = iBarbarianCampCount * 75 + iVisibleBarbarianCount * 25;   // Two visible camps or 3 roving Barbarians will trigger this
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
				PlayerTypes eOtherPlayer = (PlayerTypes) iMajorLoop;
				if(pkDiplomacyAI->IsWantsSneakAttack(eOtherPlayer))
				{
					return false;
				}
			}

			return true;
		}

		//Let's hunt barbs if we want to expand
		EconomicAIStrategyTypes eStrategyExpandToOtherContinents = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
		if(pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyExpandToOtherContinents))
		{
			return true;
		}
	}

	return false;
}

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

/// "Winning Wars" Strategy: boost OFFENSE over DEFENSE
bool MilitaryAIHelpers::IsTestStrategy_WinningWars(CvPlayer* pPlayer)
{
	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_WINNING)
	{
		return true;
	}

	int iSum = 0;
	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer != pPlayer->GetID() && pPlayer->IsAtWarWith(eLoopPlayer) && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			iSum += pPlayer->GetDiplomacyAI()->GetWarScore(eLoopPlayer);
		}
	}

	return iSum > 25;
}

/// "Losing Wars" Strategy: boost DEFENSE over OFFENSE
bool MilitaryAIHelpers::IsTestStrategy_LosingWars(CvPlayer* pPlayer)
{
	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
	{
		return true;
	}

	int iSum = 0;
	PlayerTypes eLoopPlayer;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes)iPlayerLoop;
		if (eLoopPlayer != pPlayer->GetID() && pPlayer->IsAtWarWith(eLoopPlayer) && pPlayer->GetDiplomacyAI()->IsPlayerValid(eLoopPlayer))
		{
			iSum += pPlayer->GetDiplomacyAI()->GetWarScore(eLoopPlayer);
		}
	}
	return iSum < -10;
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
	if(pPlayer->GetMilitaryAI()->IsBuildingArmy(ARMY_TYPE_LAND))
	{
		if (pPlayer->IsUnderrepresentedUnitType(UNITAI_RANGED) || pPlayer->IsUnderrepresentedUnitType(UNITAI_CITY_BOMBARD))
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
	if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyWarMob))
	{
		return true;
	}

	return false;
}

/// "Enough Mobile" Player Strategy: If a player has too many mobile units
bool MilitaryAIHelpers::IsTestStrategy_EnoughMobileUnits(CvPlayer* pPlayer, int iNumMobile, int iNumMelee)
{
	int iFlavorMobile = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_MOBILE"));
	if (!MOD_AI_UNIT_PRODUCTION)
	{
		int iRatio = iNumMobile * 10 / max(1,iNumMelee+iNumMobile);
		return (iRatio >= iFlavorMobile);
	}
	else
	{
		int iRatio = iNumMobile * 10 / max(1, iNumMelee);
		return (iRatio >= iFlavorMobile);
	}
}

/// "Need Mobile" Player Strategy: If a player has too many slow units
bool MilitaryAIHelpers::IsTestStrategy_NeedMobileUnits(CvPlayer* pPlayer, int iNumMobile, int iNumMelee)
{
	int iFlavorMobile = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_MOBILE"));
	if (!MOD_AI_UNIT_PRODUCTION)
	{
		int iRatio = iNumMobile * 10 / max(1, iNumMelee + iNumMobile);
		return (iRatio <= iFlavorMobile / 2);
	}
	else
	{
		int iRatio = iNumMobile * 10 / max(1, iNumMelee);
		return (iRatio <= iFlavorMobile / 2);
	}
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

	if (GC.getGame().IsNuclearGandhiEnabled() && !pPlayer->isHuman() && pPlayer->GetPlayerTraits()->IsPopulationBoostReligion())
	{
		return true;
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
		// The original code simplifies to 4*iNumAA > iNumMelee
		return (iNumAA * /*2*/ GD_INT_GET(AI_CONFIG_MILITARY_MELEE_PER_AA) > iNumMelee);
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
		// The original code simplifies to 4*iNumAA <= iNumMelee
		return (iNumAA * /*2*/ GD_INT_GET(AI_CONFIG_MILITARY_MELEE_PER_AA) <= iNumMelee);
	}
	else
	{
		return false;
	}
}

/// "Need Air Carrier" Player Strategy: If a player needs some aircraft carriers
bool MilitaryAIHelpers::IsTestStrategy_NeedAirCarriers(CvPlayer* pPlayer)
{
	return pPlayer->GetMilitaryAI()->HasAirforce() && pPlayer->GetMilitaryAI()->GetNumFreeCarrier() == 0;
}


// MOD_AI_UNIT_PRODUCTION: New strategies
/// "Enough Archer" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_EnoughArcherUnits(CvPlayer* pPlayer, int iNumArcher, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_ARCHER"));
	int iRatio = iNumArcher * 10 / max(1, iNumMelee);
	return (iRatio >= iFlavorRange);
}

/// "Need Archer" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_NeedArcherUnits(CvPlayer* pPlayer, int iNumArcher, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_ARCHER"));
	int iRatio = iNumArcher * 10 / max(1, iNumMelee);
	return (iRatio <= iFlavorRange / 2);
}

/// "Enough Siege" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_EnoughSiegeUnits(CvPlayer* pPlayer, int iNumSiege, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SIEGE"));
	int iRatio = iNumSiege * 10 / max(1, iNumMelee);
	return (iRatio >= iFlavorRange);
}

/// "Need Siege" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_NeedSiegeUnits(CvPlayer* pPlayer, int iNumSiege, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SIEGE"));
	int iRatio = iNumSiege * 10 / max(1, iNumMelee);
	return (iRatio <= iFlavorRange / 4);
}

/// "Enough Skirmisher" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_EnoughSkirmisherUnits(CvPlayer* pPlayer, int iNumSkirmisher, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SKIRMISHER"));
	int iRatio = iNumSkirmisher * 10 / max(1, iNumMelee);
	return (iRatio >= iFlavorRange);
}

/// "Need Skirmisher" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_NeedSkirmisherUnits(CvPlayer* pPlayer, int iNumSkirmisher, int iNumMelee)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SKIRMISHER"));
	int iRatio = iNumSkirmisher * 10 / max(1, iNumMelee);
	return (iRatio <= iFlavorRange / 4);
}

/// "Enough NavalMelee" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_EnoughNavalMeleeUnits(CvPlayer* pPlayer, int iNumNavalMelee, int iNumNaval)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_MELEE"));
	int iRatio = iNumNavalMelee * 10 / max(1, iNumNaval);
	return (iRatio >= (7 * iFlavorRange / 5));
}

/// "Need NavalMelee" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_NeedNavalMeleeUnits(CvPlayer* pPlayer, int iNumNavalMelee, int iNumNaval)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_MELEE"));
	int iRatio = iNumNavalMelee * 10 / max(1, iNumNaval);
	return (iRatio <= (3 * iFlavorRange / 5));
}

/// "Enough NavalRanged" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_EnoughNavalRangedUnits(CvPlayer* pPlayer, int iNumNavalRanged, int iNumNaval)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_RANGED"));
	int iRatio = iNumNavalRanged * 10 / max(1, iNumNaval);
	return (iRatio >= (7 * iFlavorRange / 5));
}

/// "Need NavalRanged" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_NeedNavalRangedUnits(CvPlayer* pPlayer, int iNumNavalRanged, int iNumNaval)
{	
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_RANGED"));
	int iRatio = iNumNavalRanged * 10 / max(1, iNumNaval);
	return (iRatio <= (3 * iFlavorRange / 5));
}

/// "Enough Submarine" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_EnoughSubmarineUnits(CvPlayer* pPlayer, int iNumSubmarine, int iNumNaval)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SUBMARINE"));
	int iRatio = iNumSubmarine * 10 / max(1, iNumNaval);
	return (iRatio >= (3 * iFlavorRange / 5));
}

/// "Need Submarine" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_NeedSubmarineUnits(CvPlayer* pPlayer, int iNumSubmarine, int iNumNaval)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SUBMARINE"));
	int iRatio = iNumSubmarine * 10 / max(1, iNumNaval);
	return (iRatio <= (iFlavorRange / 5));
}

/// "Enough Bomber" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_EnoughBomberUnits(CvPlayer* pPlayer, int iNumBomber, int iNumAir)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_BOMBER"));
	int iRatio = iNumBomber * 10 / max(1, iNumAir);
	return (iRatio >= (7 * iFlavorRange / 5));
}

/// "Need Bomber" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_NeedBomberUnits(CvPlayer* pPlayer, int iNumBomber, int iNumAir)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_BOMBER"));
	int iRatio = iNumBomber * 10 / max(1, iNumAir);
	return (iRatio <= (3 * iFlavorRange / 5));
}

/// "Enough Fighter" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_EnoughFighterUnits(CvPlayer* pPlayer, int iNumFighter, int iNumAir)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_FIGHTER"));
	int iRatio = iNumFighter * 10 / max(1, iNumAir);
	return (iRatio >= (7 * iFlavorRange / 5));
}

/// "Need Fighter" Player Strategy
bool MilitaryAIHelpers::IsTestStrategy_NeedFighterUnits(CvPlayer* pPlayer, int iNumFighter, int iNumAir)
{
	int iFlavorRange = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_FIGHTER"));
	int iRatio = iNumFighter * 10 / max(1, iNumAir);
	return (iRatio <= (3 * iFlavorRange / 5));
}


// MORE NON-MEMBER FUNCTIONS

//todo: use the step pathfinder here to get a plot which is on the correct side of the target? need a starting point then ...
CvPlot* MilitaryAIHelpers::GetCoastalWaterNearPlot(CvPlot *pTarget, bool bCheckTeam)
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

MultiunitFormationTypes MilitaryAIHelpers::GetCurrentBestFormationTypeForCombinedAttack()
{
	static EraTypes eThreshold = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	return GC.getGame().getCurrentEra() > eThreshold ? MUFORMATION_LATE_GAME_INVASION_FORCE : MUFORMATION_NAVAL_INVASION;
}

MultiunitFormationTypes MilitaryAIHelpers::GetCurrentBestFormationTypeForPureNavalAttack()
{
	static EraTypes eThreshold = (EraTypes) GC.getInfoTypeForString("ERA_MEDIEVAL", true);
	return GC.getGame().getCurrentEra() > eThreshold ? MUFORMATION_LATE_GAME_PURE_NAVAL_FORCE : MUFORMATION_PURE_NAVAL_CITY_ATTACK;
}

MultiunitFormationTypes MilitaryAIHelpers::GetCurrentBestFormationTypeForLandAttack()
{
	switch (GC.getGame().getCurrentEra())
	{
	case 0: //ancient
		return MUFORMATION_EARLY_RUSH;
	case 1: //classic
	case 2: //medieval
		return MUFORMATION_SMALL_CITY_ATTACK_FORCE;
	case 3: //renaissance
	case 4: //industrial
		return MUFORMATION_BASIC_CITY_ATTACK_FORCE;
	case 5: //modern
	case 6: //atomic
	case 7: //information
		return MUFORMATION_BIGGER_CITY_ATTACK_FORCE;
	}

	return MUFORMATION_BASIC_CITY_ATTACK_FORCE;
}

const char * ArmyTypeToString(ArmyType eType)
{
	switch(eType)
	{
	case ARMY_TYPE_ANY:
		return "Undefined Army";
		break;
	case ARMY_TYPE_LAND:
		return "Land Army";
		break;
	case ARMY_TYPE_NAVAL:
		return "Naval Army";
		break;
	case ARMY_TYPE_COMBINED:
		return "Combined Army";
		break;
	case ARMY_TYPE_AIR:
		return "Air Army";
		break;
	case ARMY_TYPE_ESCORT_LAND:
		return "Land Escort";
		break;
	case ARMY_TYPE_ESCORT_SEA:
		return "Naval Escort";
		break;
	default:
		return "No Army";
	}
}

FDataStream& operator<<(FDataStream& saveTo, const CvAttackTarget& readFrom)
{
	saveTo << readFrom.m_armyType;
	saveTo << readFrom.m_iMusterPlotIndex;
	saveTo << readFrom.m_iStagingPlotIndex;
	saveTo << readFrom.m_iTargetPlotIndex;
	saveTo << readFrom.m_iPathLength;
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvAttackTarget& writeTo)
{
	loadFrom >> writeTo.m_armyType;
	loadFrom >> writeTo.m_iMusterPlotIndex;
	loadFrom >> writeTo.m_iStagingPlotIndex;
	loadFrom >> writeTo.m_iTargetPlotIndex;
	loadFrom >> writeTo.m_iPathLength;
	return loadFrom;
}

void CvAttackTarget::SetWaypoints(const SPath& path)
{
	m_iPathLength = path.length();
	if (path.length() > 4)
	{
		m_iMusterPlotIndex = path.get(0)->GetPlotIndex();
		m_iStagingPlotIndex = path.get(-4)->GetPlotIndex();
		m_iTargetPlotIndex = path.get(-1)->GetPlotIndex();
	}
	else if (path.length() > 1)
	{
		m_iMusterPlotIndex = path.get(0)->GetPlotIndex();
		m_iStagingPlotIndex = path.get(0)->GetPlotIndex();
		m_iTargetPlotIndex = path.get(-1)->GetPlotIndex();
	}
	else
	{
		m_iMusterPlotIndex = -1;
		m_iStagingPlotIndex = -1;
		m_iTargetPlotIndex = -1;
	}
}

CvPlot* CvAttackTarget::GetMusterPlot() const
{
	return GC.getMap().plotByIndex(m_iMusterPlotIndex);
}

CvPlot* CvAttackTarget::GetStagingPlot() const
{
	return GC.getMap().plotByIndex(m_iStagingPlotIndex);
}

CvPlot* CvAttackTarget::GetTargetPlot() const
{
	return GC.getMap().plotByIndex(m_iTargetPlotIndex);
}

int CvAttackTarget::GetPathLength() const
{
	return m_iPathLength;
}

bool CvAttackTarget::IsValid() const
{
	return m_iMusterPlotIndex >= 0 &&
		m_iStagingPlotIndex >= 0 &&
		m_iTargetPlotIndex >= 0;
}

bool CvAttackTarget::IsPreferred() const
{
	return m_bPreferred;
}
