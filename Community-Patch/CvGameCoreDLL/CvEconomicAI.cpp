/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvEconomicAI.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvMinorCivAI.h"
#include "CvGrandStrategyAI.h"
#include "CvMilitaryAI.h"
#include "Fireworks/FVariableSystem.h"
#include "CvImprovementClasses.h"
#include "CvAStar.h"
#include "CvCitySpecializationAI.h"

// must be included after all other headers
#include "LintFree.h"


//=====================================
// CvEconomicAIStrategyXMLEntry
//=====================================
CvEconomicAIStrategyXMLEntry::CvEconomicAIStrategyXMLEntry(void):
m_piFlavorValue(NULL),
m_iWeightThreshold(0),
m_piPersonalityFlavorThresholdMod(NULL),
m_bNoMinorCivs(false),
m_bDontUpdateCityFlavors(false),
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
CvEconomicAIStrategyXMLEntry::~CvEconomicAIStrategyXMLEntry(void)
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piPersonalityFlavorThresholdMod);
}
//------------------------------------------------------------------------------
bool CvEconomicAIStrategyXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if (!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_bDontUpdateCityFlavors		= kResults.GetBool("DontUpdateCityFlavors");
	m_bNoMinorCivs					= kResults.GetBool("NoMinorCivs");
	m_iCheckTriggerTurnCount		= kResults.GetInt("CheckTriggerTurnCount");
	m_iMinimumNumTurnsExecuted		= kResults.GetInt("MinimumNumTurnsExecuted");
	m_iWeightThreshold				= kResults.GetInt("WeightThreshold");
	m_iFirstTurnExecuted            = kResults.GetInt("FirstTurnExecuted");

	const char* strAdvisor = kResults.GetText("Advisor");
	m_eAdvisor = NO_ADVISOR_TYPE;
	if (strAdvisor)
	{
		if (strcmp(strAdvisor, "MILITARY") == 0)
		{
			m_eAdvisor = ADVISOR_MILITARY;
		}
		else if (strcmp(strAdvisor, "ECONOMIC") == 0)
		{
			m_eAdvisor = ADVISOR_ECONOMIC;
		}
		else if (strcmp(strAdvisor, "FOREIGN") == 0)
		{
			m_eAdvisor = ADVISOR_FOREIGN;
		}
		else if (strcmp(strAdvisor, "SCIENCE") == 0)
		{
			m_eAdvisor = ADVISOR_SCIENCE;
		}
	}

	m_strAdvisorCounselText = kResults.GetText("AdvisorCounsel");
	m_iAdvisorCounselImportance = kResults.GetInt("AdvisorCounselImportance");

	//References
	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("TechObsolete");
	m_iTechObsolete = GC.getInfoTypeForString(szTextVal, true);

	//Arrays
	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "AIEconomicStrategy_Flavors", "AIEconomicStrategyType", szType);
	kUtility.SetFlavors(m_piPersonalityFlavorThresholdMod, "AIEconomicStrategy_PersonalityFlavorThresholdMods", "AIEconomicStrategyType", szType);

	return true;
}

/// What Flavors will be added by adopting this Strategy?
int CvEconomicAIStrategyXMLEntry::GetFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : -1;
}

/// The amount of weight a Strategy must accumulate to be adopted (if applicable)
int CvEconomicAIStrategyXMLEntry::GetWeightThreshold() const
{
	return m_iWeightThreshold;
}

/// How do a player's Personality Flavors affect the Threshold for adopting a Strategy? (if applicable)
int CvEconomicAIStrategyXMLEntry::GetPersonalityFlavorThresholdMod(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPersonalityFlavorThresholdMod ? m_piPersonalityFlavorThresholdMod[i] : -1;
}

/// Is this a valid strategy for Minor Civs to use?
bool CvEconomicAIStrategyXMLEntry::IsNoMinorCivs() const
{
	return m_bNoMinorCivs;
}

/// Should this strategy update the City's flavors as well?  (There are cases when a PlayerStrategy's trigger involves CityStrategies being active, so we don't want to double-dip)
bool CvEconomicAIStrategyXMLEntry::IsDontUpdateCityFlavors() const
{
	return m_bDontUpdateCityFlavors;
}

/// Technology prerequisite an AI Player must have to enable this Strategy
int CvEconomicAIStrategyXMLEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}

/// Technology that obsoletes this Strategy for an AI Player
int CvEconomicAIStrategyXMLEntry::GetTechObsolete() const
{
	return m_iTechObsolete;
}

/// Minimum # of turns this AI Strategy must be executed for - don't want the AI starting then stopping Strategies every turn
int CvEconomicAIStrategyXMLEntry::GetMinimumNumTurnsExecuted() const
{
	return m_iMinimumNumTurnsExecuted;
}

/// Once this Strategy has been adopted, check the Trigger conditions every X turns to see if it's maintained
int CvEconomicAIStrategyXMLEntry::GetCheckTriggerTurnCount() const
{
	return m_iCheckTriggerTurnCount;
}

/// First turn to check for adoption
int CvEconomicAIStrategyXMLEntry::GetFirstTurnExecuted() const
{
	return m_iFirstTurnExecuted;
}

AdvisorTypes CvEconomicAIStrategyXMLEntry::GetAdvisor () const
{
	return m_eAdvisor;
}

const char* CvEconomicAIStrategyXMLEntry::GetAdvisorCounselText() const
{
	return m_strAdvisorCounselText.c_str();
}

int CvEconomicAIStrategyXMLEntry::GetAdvisorCounselImportance() const
{
	return m_iAdvisorCounselImportance;
}

//=====================================
// CvEconomicAIStrategyXMLEntries
//=====================================
/// Constructor
CvEconomicAIStrategyXMLEntries::CvEconomicAIStrategyXMLEntries(void)
{

}

/// Destructor
CvEconomicAIStrategyXMLEntries::~CvEconomicAIStrategyXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of AIStrategy entries
std::vector<CvEconomicAIStrategyXMLEntry*>& CvEconomicAIStrategyXMLEntries::GetEconomicAIStrategyEntries()
{
	return m_paAIStrategyEntries;
}

/// Number of defined AIStrategies
int CvEconomicAIStrategyXMLEntries::GetNumEconomicAIStrategies()
{
	return m_paAIStrategyEntries.size();
}

/// Clear AIStrategy entries
void CvEconomicAIStrategyXMLEntries::DeleteArray()
{
	for (std::vector<CvEconomicAIStrategyXMLEntry*>::iterator it = m_paAIStrategyEntries.begin(); it != m_paAIStrategyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAIStrategyEntries.clear();
}

/// Get a specific entry
CvEconomicAIStrategyXMLEntry *CvEconomicAIStrategyXMLEntries::GetEntry(int index)
{
	return m_paAIStrategyEntries[index];
}

//=====================================
// CvEconomicAI
//=====================================

/// Constructor
CvEconomicAI::CvEconomicAI ():
m_pabUsingStrategy(NULL),
m_paiTurnStrategyAdopted(NULL),
m_aiTempFlavors(NULL),
m_iEarlyCityNumberTarget(1)
{
}

/// Destructor
CvEconomicAI::~CvEconomicAI (void)
{
	Uninit();
}

/// Initialize
void CvEconomicAI::Init(CvEconomicAIStrategyXMLEntries *pAIStrategies, CvPlayer *pPlayer)
{
	// Store off the pointer to the AIStrategies active for this game
	m_pAIStrategies = pAIStrategies;

	m_pPlayer = pPlayer;

	// Initialize arrays
	FAssertMsg(m_pabUsingStrategy==NULL, "about to leak memory, CvStrategyAI::m_pabUsingStrategy");
	m_pabUsingStrategy = FNEW(bool[m_pAIStrategies->GetNumEconomicAIStrategies()], c_eCiv5GameplayDLL, 0);

	FAssertMsg(m_paiTurnStrategyAdopted==NULL, "about to leak memory, CvStrategyAI::m_paiTurnStrategyAdopted");
	m_paiTurnStrategyAdopted = FNEW(int[m_pAIStrategies->GetNumEconomicAIStrategies()], c_eCiv5GameplayDLL, 0);

	FAssertMsg(m_aiTempFlavors==NULL, "about to leak memory, CvStrategyAI::m_aiTempFlavors");
	m_aiTempFlavors = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);

	m_auiYields.clear();
	m_auiYields.push_back_copy(-1, NUM_YIELD_TYPES);
	m_RequestedSavings.clear();

	Reset();
}

/// Deallocate memory created in initialize
void CvEconomicAI::Uninit()
{
	SAFE_DELETE_ARRAY(m_pabUsingStrategy);
	SAFE_DELETE_ARRAY(m_paiTurnStrategyAdopted);
	SAFE_DELETE_ARRAY(m_aiTempFlavors);
}

/// Reset AIStrategy status array to all false
void CvEconomicAI::Reset()
{
	int iI;

	for (iI = 0; iI < m_pAIStrategies->GetNumEconomicAIStrategies(); iI++)
	{
		m_pabUsingStrategy[iI] = false;
		m_paiTurnStrategyAdopted[iI] = -1;
	}

	for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		m_auiYields[ui] = 0;
	}

	for (uint ui = 0; ui < m_aiExplorationPlots.size(); ui++)
	{
		m_aiExplorationPlots[ui] = -1;
		m_aiExplorationPlotRatings[ui] = -1;
	}

	for (uint ui = 0; ui < m_aiGoodyHutPlots.size(); ui++)
	{
		m_aiGoodyHutPlots[ui] = -1;
		m_aiGoodyHutUnitAssignments[ui].Clear();
	}

	m_bExplorationPlotsDirty = true;

	for (uint ui = 0; ui < NUM_PURCHASE_TYPES; ui++)
	{
		CvPurchaseRequest request;
		request.m_eType = (PurchaseType)ui;
		request.m_iAmount = 0;
		request.m_iPriority = 0;
		m_RequestedSavings.push_back(request);
	}

	m_iExplorersDisbanded = 0;
	m_eReconState = NO_RECON_STATE;
	m_eNavalReconState = NO_RECON_STATE;
	m_iLastTurnWorkerDisbanded = -1;

	// Cached AI defines
	m_iMinimumSettleFertility = GC.getAI_STRATEGY_MINIMUM_SETTLE_FERTILITY();

	// Basic number of cities desired for early growth is in XML (10)
	// Later will scale that up or down based on ratio of FLAVOR_EXPANSION to FLAVOR_GROWTH
	m_iEarlyCityNumberTarget = GC.getAI_STRATEGY_EARLY_EXPANSION_NUM_CITIES_LIMIT();
}

/// Serialization read
void CvEconomicAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	int iEntriesToRead;

	FAssertMsg(m_pAIStrategies != NULL && m_pAIStrategies->GetNumEconomicAIStrategies() > 0, "Number of AIStrategies to serialize is expected to greater than 0");

	if (uiVersion < 3)
	{
		iEntriesToRead = 24;
	}
	else
	{
		kStream >> iEntriesToRead;
	}

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 6011 ) // no clear solution or recovery if m_pAIStrategies is ever NULL
#endif//_MSC_VER
	ArrayWrapper<bool> wrapGetNumEconomicAIStrategies(iEntriesToRead, m_pabUsingStrategy);
#ifdef _MSC_VER
#pragma warning ( pop )
#endif//_MSC_VER

	kStream >> wrapGetNumEconomicAIStrategies;

	ArrayWrapper<int> wrapGetNumEconomicAIStrategies2(iEntriesToRead, m_paiTurnStrategyAdopted);
	kStream >> wrapGetNumEconomicAIStrategies2;

	for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		kStream >> m_auiYields[ui];
	}

	kStream >> m_bExplorationPlotsDirty;

	kStream >> iEntriesToRead;

	#define MAX_PLOT_ARRAY_SIZE	((152+1)*(96+1))
	int iMaxEntriesToRead = MIN(MAX_PLOT_ARRAY_SIZE, iEntriesToRead);

	m_aiExplorationPlots.resize(iMaxEntriesToRead);
	m_aiExplorationPlotRatings.resize(iMaxEntriesToRead);

	for (int i = 0; i < iMaxEntriesToRead; i++)
	{
		kStream >> m_aiExplorationPlots[i];
		kStream >> m_aiExplorationPlotRatings[i];
	}

	// Skip any ones that go over the limit
	for (int i = iMaxEntriesToRead; i < iEntriesToRead; i++)
	{
		int iDummy;
		kStream >> iDummy;
		kStream >> iDummy;
	}

	// goody hut plots
	kStream >> iEntriesToRead;
	iMaxEntriesToRead = MIN(MAX_PLOT_ARRAY_SIZE, iEntriesToRead);
	m_aiGoodyHutPlots.resize(iMaxEntriesToRead);
	m_aiGoodyHutUnitAssignments.resize(iMaxEntriesToRead);

	for (int i = 0; i < iMaxEntriesToRead; i++)
	{
		kStream >> m_aiGoodyHutPlots[i];
		m_aiGoodyHutUnitAssignments[i].Clear();
		kStream >> m_aiGoodyHutUnitAssignments[i].m_iUnitID;
	}

	// Skip any ones that go over the limit
	for (int i = iMaxEntriesToRead; i < iEntriesToRead; i++)
	{
		int iDummy;
		kStream >> iDummy;
		kStream >> iDummy;
	}

	int iTemp;
	kStream >> iTemp;
	m_eReconState = (ReconState)iTemp;
	kStream >> iTemp;
	m_eNavalReconState = (ReconState)iTemp;

	kStream >> m_iExplorersDisbanded;

	if (uiVersion >= 2)
	{
		kStream >> m_iLastTurnWorkerDisbanded;
	}
	else
	{
		m_iLastTurnWorkerDisbanded = -1;
	}

	kStream >> iEntriesToRead;
	for (int i = 0; i < iEntriesToRead; i++)
	{
		kStream >> m_RequestedSavings[i];
	}
}

/// Serialization write
void CvEconomicAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 3;
	kStream << uiVersion;

	FAssertMsg(GC.getNumEconomicAIStrategyInfos() > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	kStream << m_pAIStrategies->GetNumEconomicAIStrategies();
	kStream << ArrayWrapper<bool>(m_pAIStrategies->GetNumEconomicAIStrategies(), m_pabUsingStrategy);
	kStream << ArrayWrapper<int>(m_pAIStrategies->GetNumEconomicAIStrategies(), m_paiTurnStrategyAdopted);
	for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		kStream << m_auiYields[ui];
	}

	kStream << m_bExplorationPlotsDirty;
	kStream << m_aiExplorationPlots.size();
	for (uint ui = 0; ui < m_aiExplorationPlots.size(); ui++)
	{
		kStream << m_aiExplorationPlots[ui];
		kStream << m_aiExplorationPlotRatings[ui];
	}

	kStream << m_aiGoodyHutPlots.size();
	for (uint ui = 0; ui < m_aiGoodyHutPlots.size(); ui++)
	{
		kStream << m_aiGoodyHutPlots[ui];
		kStream << m_aiGoodyHutUnitAssignments[ui].m_iUnitID;
	}

	kStream << (int)m_eReconState;
	kStream << (int)m_eNavalReconState;
	kStream << m_iExplorersDisbanded;
	kStream << m_iLastTurnWorkerDisbanded;

	kStream << (int)m_RequestedSavings.size();
	for (uint ui = 0; ui < m_RequestedSavings.size(); ui++)
	{
		kStream << m_RequestedSavings[ui];
	}
}

/// Returns the Player object the Strategies are associated with
CvPlayer* CvEconomicAI::GetPlayer()
{
	return m_pPlayer;
}

/// Returns AIStrategies object stored in this class
CvEconomicAIStrategyXMLEntries* CvEconomicAI::GetEconomicAIStrategies()
{
	return m_pAIStrategies;
}

/// Returns whether or not a player has adopted this Strategy
bool CvEconomicAI::IsUsingStrategy(EconomicAIStrategyTypes eStrategy)
{
	return m_pabUsingStrategy[(int) eStrategy];
}

/// Signifies that eStrategy is now being utilized by this AI Player
void CvEconomicAI::SetUsingStrategy(EconomicAIStrategyTypes eStrategy, bool bValue)
{
	if (m_pabUsingStrategy[eStrategy] != bValue)
	{
		m_pabUsingStrategy[eStrategy] = bValue;

		if (bValue)
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
int CvEconomicAI::GetTurnStrategyAdopted(EconomicAIStrategyTypes eStrategy)
{
	return m_paiTurnStrategyAdopted[(int) eStrategy];
}

/// Sets the turn number eStrategy was most recently adopted
void CvEconomicAI::SetTurnStrategyAdopted(EconomicAIStrategyTypes eStrategy, int iValue)
{
	if (m_paiTurnStrategyAdopted[(int) eStrategy] != iValue)
	{
		m_paiTurnStrategyAdopted[(int) eStrategy] = iValue;
	}
}

// Do Turn & Strategy Trigger Functions beneath this line

/// Called every turn to see what Strategies this player should using (or not)
void CvEconomicAI::DoTurn()
{
	UpdatePlots();
	LogMonitor();
	LogCityMonitor();

	// Functions that need to run before we look at strategies
	DoReconState();

	int iStrategiesLoop = 0;

	// Loop through all strategies
	for (iStrategiesLoop = 0; iStrategiesLoop < GetEconomicAIStrategies()->GetNumEconomicAIStrategies(); iStrategiesLoop++)
	{
		EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) iStrategiesLoop;
		CvEconomicAIStrategyXMLEntry* pStrategy = GetEconomicAIStrategies()->GetEntry(iStrategiesLoop);

		// Minor Civs can't run some Strategies
		if (m_pPlayer->isMinorCiv() && pStrategy->IsNoMinorCivs())
		{
			continue;
		}

		bool bTestStrategyStart = true;

		// Do we already have this Strategy adopted?
		if (IsUsingStrategy(eStrategy))
		{
			bTestStrategyStart = false;
		}
		else
		{
			// Has the prereq Tech necessary?
			if (pStrategy->GetTechPrereq() != NO_TECH && !GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechPrereq()))
			{
				bTestStrategyStart = false;
			}

			// Has the Tech which obsoletes this Strategy?
			if (bTestStrategyStart && pStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechObsolete()))
			{
				bTestStrategyStart = false;
			}

			// Not time to check this yet?
			if (GC.getGame().getGameTurn() < pStrategy->GetFirstTurnExecuted())
			{
				bTestStrategyStart = false;
			}
		}

		bool bTestStrategyEnd = false;

		// Strategy is active, check to see if we should even try to disable it
		if (IsUsingStrategy(eStrategy))
		{
			if (pStrategy->GetCheckTriggerTurnCount() > 0)
			{
				// Is it a turn where we want to check to see if this Strategy is maintained?
				if ((GC.getGame().getGameTurn() - GetTurnStrategyAdopted(eStrategy)) % pStrategy->GetCheckTriggerTurnCount() == 0)
				{
					bTestStrategyEnd = true;
				}
			}

			if (bTestStrategyEnd && pStrategy->GetMinimumNumTurnsExecuted() > 0)
			{
				// Has the minimum # of turns passed for this Strategy?
				if (GC.getGame().getGameTurn() < GetTurnStrategyAdopted(eStrategy) + pStrategy->GetMinimumNumTurnsExecuted())
				{
					bTestStrategyEnd = false;
				}
			}
		}

		// Check Strategy Triggers
		// Functionality and existence of specific Strategies is hardcoded here, but data is stored in XML so it's easier to modify

		if (bTestStrategyStart || bTestStrategyEnd)
		{
			bool bStrategyShouldBeActive = false;

			// Has the Tech which obsoletes this Strategy? If so, Strategy should be deactivated regardless of other factors
			if (pStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pStrategy->GetTechObsolete()))
			{
				bStrategyShouldBeActive = false;
			}
			// Strategy isn't obsolete, so test triggers as normal
			else
			{
				CvString strStrategyName = (CvString) pStrategy->GetType();

				// Check all of the Strategy Triggers
				if (strStrategyName == "ECONOMICAISTRATEGY_NEED_RECON")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedRecon(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_RECON")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughRecon(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_REALLY_NEED_RECON_SEA")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ReallyNeedReconSea(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_NEED_RECON_SEA")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedReconSea(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_RECON_SEA")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughReconSea(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_EARLY_EXPANSION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EarlyExpansion(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_EXPANSION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughExpansion(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_NEED_HAPPINESS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedHappiness(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_NEED_HAPPINESS_CRITICAL")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedHappinessCritical(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_CITIES_NEED_NAVAL_GROWTH")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalGrowth(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_CITIES_NEED_NAVAL_TILE_IMPROVEMENT")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalTileImprovement(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_FOUND_CITY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_FoundCity(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_TRADE_WITH_CITY_STATE")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_TradeWithCityState(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_NEED_IMPROVEMENT_FOOD")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedImprovement(m_pPlayer, YIELD_FOOD);
				else if (strStrategyName == "ECONOMICAISTRATEGY_NEED_IMPROVEMENT_PRODUCTION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedImprovement(m_pPlayer, YIELD_PRODUCTION);
				else if (strStrategyName == "ECONOMICAISTRATEGY_ONE_OR_FEWER_COASTAL_CITIES")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_OneOrFewerCoastalCities(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_LOSING_MONEY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_LosingMoney(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_HALT_GROWTH_BUILDINGS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_HaltGrowthBuildings(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_TOO_MANY_UNITS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_TooManyUnits(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_ISLAND_START")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_IslandStart(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ExpandToOtherContinents(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_MOSTLY_ON_THE_COAST")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_MostlyOnTheCoast(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_EXPAND_LIKE_CRAZY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ExpandLikeCrazy(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_GROW_LIKE_CRAZY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GrowLikeCrazy(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_GS_CULTURE")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_Culture(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_GS_CONQUEST")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_Conquest(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_GS_DIPLOMACY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_Diplomacy(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_GS_SPACESHIP")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_Spaceship(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_GS_SPACESHIP_HOMESTRETCH")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_GS_SpaceshipHomestretch(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_NAVAL_MAP")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NavalMap(m_pPlayer);
			}

			// This variable keeps track of whether or not we should be doing something (i.e. Strategy is active now but should be turned off, OR Strategy is inactive and should be enabled)
			bool bAdoptOrEndStrategy = false;

			// Strategy should be on, and if it's not, turn it on
			if (bStrategyShouldBeActive)
			{
				if (bTestStrategyStart)
				{
					bAdoptOrEndStrategy = true;
				}
				else if (bTestStrategyEnd)
				{
					bAdoptOrEndStrategy = false;
				}
			}
			// Strategy should be off, and if it's not, turn it off
			else
			{
				if (bTestStrategyStart)
				{
					bAdoptOrEndStrategy = false;
				}
				else if (bTestStrategyEnd)
				{
					bAdoptOrEndStrategy = true;
				}
			}

			// Flavor propagation
			if (bAdoptOrEndStrategy)
			{
				int iFlavorLoop;
				bool bDontUpdateCityFlavors = pStrategy->IsDontUpdateCityFlavors();

				// We should adopt this Strategy
				if (bTestStrategyStart)
				{
					SetUsingStrategy(eStrategy, true);

					for (iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = pStrategy->GetFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, bDontUpdateCityFlavors);
				}
				// End the Strategy
				else if (bTestStrategyEnd)
				{
					SetUsingStrategy(eStrategy, false);

					for (iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = -pStrategy->GetFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeFlavors(m_aiTempFlavors, bDontUpdateCityFlavors);
				}
			}
		}
	}

	if (!m_pPlayer->isHuman())
	{
		DoHurry();
		DoPlotPurchases();
		DisbandExtraWorkers();
	}
}

void AppendToLog (CvString& strHeader, CvString& strLog, CvString strHeaderValue, CvString strValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	strLog += strValue;
	strLog += ",";
}

void AppendToLog (CvString& strHeader, CvString& strLog, CvString strHeaderValue, int iValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%d,", iValue);
	strLog += str;
}

void AppendToLog (CvString& strHeader, CvString& strLog, CvString strHeaderValue, float fValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%.2f,", fValue);
	strLog += str;
}

FFastVector<int>& CvEconomicAI::GetExplorationPlots ()
{
	if (m_bExplorationPlotsDirty)
	{
		UpdatePlots();
	}

	return m_aiExplorationPlots;
}

FFastVector<int>& CvEconomicAI::GetExplorationPlotRatings()
{
	if (m_bExplorationPlotsDirty)
	{
		UpdatePlots();
	}

	return m_aiExplorationPlotRatings;
}

FFastVector<int>& CvEconomicAI::GetGoodyHutPlots ()
{
	if (m_bExplorationPlotsDirty)
	{
		UpdatePlots();
	}

	return m_aiGoodyHutPlots;
}

CvPlot* CvEconomicAI::GetUnitTargetGoodyPlot(CvUnit* pUnit, CvPlot** ppkStepPlot /*= NULL */)
{
	if (m_bExplorationPlotsDirty)
	{
		UpdatePlots();
	}

	int iUnitID = pUnit->GetID();
	for (uint ui = 0; ui < m_aiGoodyHutUnitAssignments.size(); ui++)
	{
		if(iUnitID == m_aiGoodyHutUnitAssignments[ui].m_iUnitID)
		{
			if (ppkStepPlot)
			{
				int iStepPlotID = m_aiGoodyHutUnitAssignments[ui].m_iStepPlotID;
				*ppkStepPlot = (iStepPlotID != -1)?GC.getMap().plotByIndex(iStepPlotID):NULL;
			}
			return GC.getMap().plotByIndex(m_aiGoodyHutPlots[ui]);
		}
	}

	return NULL;
}

//	---------------------------------------------------------------------------
//	Clear the calculated step plot for the unit.  This should be called after
//	the unit had been given and order to move to the plot.
void CvEconomicAI::ClearUnitTargetGoodyStepPlot(CvUnit* pUnit)
{
	int iUnitID = pUnit->GetID();
	for(uint ui = 0; ui < m_aiGoodyHutUnitAssignments.size(); ui++)
	{
		if(iUnitID == m_aiGoodyHutUnitAssignments[ui].m_iUnitID)
		{
			m_aiGoodyHutUnitAssignments[ui].m_iStepPlotID = -1;
		}
	}
}

//	---------------------------------------------------------------------------
int CvEconomicAI::ScoreExplorePlot (CvPlot* pPlot, TeamTypes eTeam, int iRange, DomainTypes eDomainType)
{
	int iResultValue = 0;
	int iAdjacencyBonus = 1;
	int iBadScore = 10;
	int iGoodScore = 100;
	int iReallyGoodScore = 200;
	//int iGoodyHutScore = 100000;

	int iPlotX = pPlot->getX();
	int iPlotY = pPlot->getY();

	FAssertMsg(pPlot->isRevealed(eTeam), "Plot isn't revealed. This isn't good.");
	CvPlot* pEvalPlot = NULL;
	for (int iX = -iRange; iX <= iRange; iX++)
	{
		for (int iY = -iRange; iY <= iRange; iY++)
		{
			pEvalPlot = plotXYWithRangeCheck(iPlotX, iPlotY, iX, iY, iRange);
			if (!pEvalPlot)
			{
				continue;
			}

			if (pEvalPlot == pPlot)
			{
				continue;
			}

			if (pEvalPlot->isRevealed(eTeam))
			{
				continue;
			}

			if (pEvalPlot->isAdjacentRevealed(eTeam))
			{
				if (plotDistance(iPlotX, iPlotY, pEvalPlot->getX(), pEvalPlot->getY()) > 1)
				{
					CvPlot* pAdjacentPlot;
					bool bViewBlocked = true;
					for (int i = 0; i < NUM_DIRECTION_TYPES; ++i)
					{
						pAdjacentPlot = plotDirection(pEvalPlot->getX(), pEvalPlot->getY(), ((DirectionTypes)i));
						if (pAdjacentPlot != NULL)
						{
							if (pAdjacentPlot->isRevealed(eTeam))
							{
								int iDistance = plotDistance(iPlotX, iPlotY, pAdjacentPlot->getX(), pAdjacentPlot->getY());
								if (iDistance > iRange)
								{
									continue;
								}

								// this cheats, because we can't be sure that between the target and the viewer
								if (pPlot->canSeePlot(pEvalPlot, eTeam, iRange, NO_DIRECTION))
								{
									bViewBlocked = false;
								}

								if (!bViewBlocked)
								{
									break;
								}
							}
						}
					}

					if (bViewBlocked)
					{
						continue;
					}
				}

				// "cheating" to look to see what the next tile is.
				// a human should be able to do this by looking at the transition from the tile to the next
				switch (eDomainType)
				{
				case DOMAIN_SEA:
					{
						FeatureTypes eFeature = pEvalPlot->getFeatureType();
						if (pEvalPlot->isWater() || (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->isImpassable()))
						{
							iResultValue += iBadScore;
						}
						else if (pEvalPlot->isMountain() || pEvalPlot->isHills() || (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->getSeeThroughChange() > 0))
						{
							iResultValue += iGoodScore;
						}
						else
						{
							iResultValue += iReallyGoodScore;
						}
					}
					break;
				case DOMAIN_LAND:
					if (pEvalPlot->isMountain() || pEvalPlot->isWater())
					{
						iResultValue += iBadScore;
					}
					else if (pEvalPlot->isHills())
					{
						iResultValue += iReallyGoodScore;
					}
					else
					{
						iResultValue += iGoodScore;
					}
					break;
				}
			}
			else
			{
				iResultValue += iGoodScore;
			}

			int iDistance = plotDistance(iPlotX, iPlotY, pEvalPlot->getX(), pEvalPlot->getY());
			iResultValue += (iRange - iDistance) * iAdjacencyBonus;
		}
	}

	return iResultValue;
}

/// Request that the AI set aside this much money
void CvEconomicAI::StartSaveForPurchase(PurchaseType ePurchase, int iAmount, int iPriority)
{
	int iIndex = (int)ePurchase;
	CvPurchaseRequest request;
	request.m_eType = ePurchase;
	request.m_iAmount = iAmount;
	request.m_iPriority = iPriority;
	m_RequestedSavings[iIndex] = request;
}

/// Have we put in a request for this type of purchase?
bool CvEconomicAI::IsSavingForThisPurchase(PurchaseType ePurchase)
{
	return (m_RequestedSavings[(int)ePurchase].m_iAmount > 0);
}

/// Cancel savings request
void CvEconomicAI::CancelSaveForPurchase(PurchaseType ePurchase)
{
	int iIndex = (int)ePurchase;
	CvPurchaseRequest request;
	request.m_eType = ePurchase;
	request.m_iAmount = 0;
	request.m_iPriority = 0;
	m_RequestedSavings[iIndex] = request;
}

/// Returns true if have enough saved up for this purchase. May return false if have enough but higher priority requests have dibs on the gold.
//  (Priority of -1 (default parameter) means use existing priority
bool CvEconomicAI::CanWithdrawMoneyForPurchase(PurchaseType ePurchase, int iAmount, int iPriority)
{
	int iBalance = m_pPlayer->GetTreasury()->GetGold();

	// Update this item's priority
	if (iPriority != -1)
	{
		int iIndex = (int)ePurchase;
		m_RequestedSavings[iIndex].m_iPriority = iPriority;
	}

	// Copy into temp array and sort by priority
	m_TempRequestedSavings = m_RequestedSavings;
	std::stable_sort(m_TempRequestedSavings.begin(), m_TempRequestedSavings.end());

	for (int iI = 0; iI < (int)m_TempRequestedSavings.size(); iI++)
	{
		CvPurchaseRequest request = m_TempRequestedSavings[iI];

		// Is this higher priority than the request we care about?
		if (request.m_eType != ePurchase)
		{
			iBalance -= request.m_iAmount;

			// No money left?
			if (iBalance <= 0)
			{
				return false;
			}
		}

		// Is this the one, if so, check balance remaining
		else if (request.m_eType == ePurchase)
		{
			return (iBalance >=iAmount);
		}
	}

	CvAssert(false);
	return false;  // Should never reach here
}

/// Returns amount of gold economic AI is willing to release for this type of purchase. May not be full gold balance if higher priority requests are in. Does not actually spend the gold.
int CvEconomicAI::AmountAvailableForPurchase(PurchaseType ePurchase)
{
	int iBalance = m_pPlayer->GetTreasury()->GetGold();

	// Copy into temp array and sort by priority
	m_TempRequestedSavings = m_RequestedSavings;
	std::stable_sort(m_TempRequestedSavings.begin(), m_TempRequestedSavings.end());

	for (int iI = 0; iI < (int)m_TempRequestedSavings.size(); iI++)
	{
		CvPurchaseRequest request = m_TempRequestedSavings[iI];

		// Is this higher priority than the request we care about?
		if (request.m_eType != ePurchase)
		{
			iBalance -= request.m_iAmount;

			// No money left?
			if (iBalance <= 0)
			{
				return 0;
			}
		}

		// Is this the one, if so, check balance remaining
		else if (request.m_eType == ePurchase)
		{
			return (iBalance);
		}
	}

	CvAssert(false);
	return false;  // Should never reach here
}

/// Make a master log so that we can evaluate and compare changes to the economic AI
void CvEconomicAI::LogMonitor (void)
{
	if (!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	// don't log minor civs for now
	if (m_pPlayer->isMinorCiv())
	{
		return;
	}

	static bool bFirstRun = true;
	bool bBuildHeader = false;
	CvString strHeader;
	if (bFirstRun)
	{
		bFirstRun = false;
		bBuildHeader = true;
	}

	CvString strLog;

	// Find the name of this civ and city
	CvString strPlayerName;
	strPlayerName = m_pPlayer->getCivilizationShortDescription();
	CvString strLogName;

	// Open the log file
	if (GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "EconomicMonitorLog_" + strPlayerName + ".csv";
	}
	else
	{
		strLogName = "EconomicMonitorLog.csv";
	}

	FILogFile *pLog;
	pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

	CvString str;

	// civ name
	AppendToLog(strHeader, strLog, "Civ Name", strPlayerName);

	// turn
	AppendToLog(strHeader, strLog, "Turn", GC.getGame().getGameTurn());

	// # cities
	AppendToLog(strHeader, strLog, "# Cities", GetPlayer()->getNumCities());

	// total pop
	int iPop = 0;
	int iLoopCity = 0;
	CvCity* pLoopCity = NULL;
	for (pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoopCity))
	{
		iPop += pLoopCity->getPopulation();
	}
	AppendToLog(strHeader, strLog, "Pop", iPop);

	// total yields this turn
	for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		switch (ui)
		{
		case YIELD_FOOD:
			AppendToLog(strHeader, strLog, "Food", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_PRODUCTION:
			AppendToLog(strHeader, strLog, "Production", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_GOLD:
			AppendToLog(strHeader, strLog, "City Gold", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_SCIENCE:
			AppendToLog(strHeader, strLog, "Science", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		}
	}

	// Gold breakdown
	int iGoldPerTurn = m_pPlayer->calculateTotalYield((YieldTypes)YIELD_GOLD) + m_pPlayer->GetTreasury()->GetCityConnectionGold();
	iGoldPerTurn -= m_pPlayer->GetTreasury()->GetExpensePerTurnUnitMaintenance();
	iGoldPerTurn -= m_pPlayer->GetTreasury()->GetBuildingGoldMaintenance();
	iGoldPerTurn -= m_pPlayer->GetTreasury()->GetImprovementGoldMaintenance();
	AppendToLog(strHeader, strLog, "GPT", iGoldPerTurn);
	AppendToLog(strHeader, strLog, "Treasury", m_pPlayer->GetTreasury()->GetGold());
	AppendToLog(strHeader, strLog, "GPT - Connects", m_pPlayer->GetTreasury()->GetCityConnectionGold());
	AppendToLog(strHeader, strLog, "GPT - Diplo", m_pPlayer->GetTreasury()->GetGoldPerTurnFromDiplomacy());
	AppendToLog(strHeader, strLog, "Unit Maint", m_pPlayer->GetTreasury()->GetExpensePerTurnUnitMaintenance());
	AppendToLog(strHeader, strLog, "Build Maint", m_pPlayer->GetTreasury()->GetBuildingGoldMaintenance());
	AppendToLog(strHeader, strLog, "Improve Maint", m_pPlayer->GetTreasury()->GetImprovementGoldMaintenance());

	// Num Techs
	AppendToLog(strHeader, strLog, "Num Techs", GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->GetNumTechsKnown());

	// Culture
	AppendToLog(strHeader, strLog, "Policies", GetPlayer()->GetPlayerPolicies()->GetNumPoliciesOwned());
	AppendToLog(strHeader, strLog, "Culture (lifetime)", GetPlayer()->GetJONSCultureEverGenerated());

	// workers
	int iWorkerCount = 0;
	CvUnit* pLoopUnit;
	int iLoopUnit;
	for (pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		if (pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			iWorkerCount++;
		}
	}
	AppendToLog(strHeader, strLog, "# Workers", iWorkerCount);
	AppendToLog(strHeader, strLog, "Positive Happiness",  m_pPlayer->GetHappiness());
	AppendToLog(strHeader, strLog, "Negative Happiness", m_pPlayer->GetUnhappiness());
	AppendToLog(strHeader, strLog, "Net Happiness", m_pPlayer->GetExcessHappiness());

	// # of each improvement constructed
	FFastVector<int, true> m_aiNumImprovements;
	m_aiNumImprovements.push_back_copy(-1, GC.getNumImprovementInfos());

	// go through all the plots the player has under their control
	CvPlotsVector& aiPlots = m_pPlayer->GetPlots();

	// worked tiles
	int iTiles = 0;
	int iWorkedTiles = 0;
	int iImprovedTiles = 0;
	for (uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
	{
		// when we encounter the first plot that is invalid, the rest of the list will be invalid
		if (aiPlots[uiPlotIndex] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
		if (!pPlot)
		{
			continue;
		}

		iTiles++;

		// plot has city in it, don't count
		if (pPlot->getPlotCity())
		{
			continue;
		}

		if (pPlot->isBeingWorked())
		{
			iWorkedTiles++;
		}

		if (pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			iImprovedTiles++;
		}
	}

	AppendToLog(strHeader, strLog, "Tiles", iTiles);
	AppendToLog(strHeader, strLog, "Worked (non-city) tiles", iWorkedTiles);
	AppendToLog(strHeader, strLog, "Improved tiles", iImprovedTiles);

	if (bBuildHeader)
	{
		pLog->Msg(strHeader);
	}
	pLog->Msg(strLog);
}

void CvEconomicAI::LogCityMonitor()
{
	if (!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	// don't log minor civs for now
	if (m_pPlayer->isMinorCiv())
	{
		return;
	}

	static bool bFirstRun = true;
	bool bBuildHeader = false;
	CvString strHeader;
	if (bFirstRun)
	{
		bFirstRun = false;
		bBuildHeader = true;
	}

	// go through all the plots the player has under their control
	CvPlotsVector& aiPlots = m_pPlayer->GetPlots();

	CvString strLog;

	// Find the name of this civ and city
	CvString strPlayerName;
	strPlayerName = m_pPlayer->getCivilizationShortDescription();
	CvString strLogName;

	// Open the log file
	if (GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "EconomicCityMonitorLog_" + strPlayerName + ".csv";
	}
	else
	{
		strLogName = "EconomicCityMonitorLog.csv";
	}

	FILogFile *pLog;
	pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

	CvString str;

	float fRatio;

	// per city
	int iLoopCity = 0;
	CvCity* pLoopCity = NULL;

	FFastVector<int> aiCityYields;
	aiCityYields.push_back_copy(-1, NUM_YIELD_TYPES);

	FFastVector<int> aiSpecialistsYields;
	aiSpecialistsYields.push_back_copy(-1, NUM_YIELD_TYPES);

	for (pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoopCity))
	{
		for (uint ui = 0; ui < aiCityYields.size(); ui++)
		{
			aiCityYields[ui] = 0;
		}

		for (uint ui = 0; ui < aiSpecialistsYields.size(); ui++)
		{
			aiSpecialistsYields[ui] = 0;
		}

		strHeader = "";
		strLog = "";

		// civ name
		AppendToLog(strHeader, strLog, "Civ Name", strPlayerName);

		// turn
		AppendToLog(strHeader, strLog, "Turn", GC.getGame().getGameTurn());

		// city name
		str = pLoopCity->getName();
		AppendToLog(strHeader, strLog, "City Name", str);

		//	pop
		AppendToLog(strHeader, strLog, "Population", pLoopCity->getPopulation());

		//	total yields
		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			aiCityYields[ui] = pLoopCity->getYieldRate((YieldTypes)ui);
			switch (ui)
			{
			case YIELD_FOOD:
				AppendToLog(strHeader, strLog, "food", aiCityYields[ui]);
				break;
			case YIELD_PRODUCTION:
				AppendToLog(strHeader, strLog, "production", aiCityYields[ui]);
				break;
			case YIELD_SCIENCE:
				AppendToLog(strHeader, strLog, "science", aiCityYields[ui]);
				break;
			case YIELD_GOLD:
				AppendToLog(strHeader, strLog, "city gold", aiCityYields[ui]);
				break;
			}
		}

		//	yields / pop
		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			fRatio = 0.0f;
			if (pLoopCity->getPopulation() > 0)
			{
				fRatio = aiCityYields[ui] / (float)pLoopCity->getPopulation();
			}

			switch (ui)
			{
			case YIELD_FOOD:
				AppendToLog(strHeader, strLog, "food / pop", fRatio);
				break;
			case YIELD_PRODUCTION:
				AppendToLog(strHeader, strLog, "production / pop", fRatio);
				break;
			case YIELD_SCIENCE:
				AppendToLog(strHeader, strLog, "science / pop", fRatio);
				break;
			case YIELD_GOLD:
				AppendToLog(strHeader, strLog, "gold / pop", fRatio);
				break;
			}
		}

		//	yields from specialists
		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			aiSpecialistsYields[ui] = pLoopCity->getExtraSpecialistYield((YieldTypes)ui);
			switch (ui)
			{
			case YIELD_FOOD:
				AppendToLog(strHeader, strLog, "food specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_PRODUCTION:
				AppendToLog(strHeader, strLog, "production specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_SCIENCE:
				AppendToLog(strHeader, strLog, "science specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_GOLD:
				AppendToLog(strHeader, strLog, "gold specialist", aiSpecialistsYields[ui]);
				break;
			}
		}

		// ratio from specialists
		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			fRatio = 0.0f;
			if (aiCityYields[ui] > 0)
			{
				fRatio = aiSpecialistsYields[ui] / (float)aiCityYields[ui];
			}
			switch (ui)
			{
			case YIELD_FOOD:
				AppendToLog(strHeader, strLog, "food specialist / food", fRatio);
				break;
			case YIELD_PRODUCTION:
				AppendToLog(strHeader, strLog, "production specialist / production", fRatio);
				break;
			case YIELD_SCIENCE:
				AppendToLog(strHeader, strLog, "science specialist / science", fRatio);
				break;
			case YIELD_GOLD:
				AppendToLog(strHeader, strLog, "gold specialist / gold", fRatio);
				break;
			}
		}

		//	% of worked tiles that are improved
		// worked tiles
		int iTiles = 0;
		int iWorkedTiles = 0;
		int iImprovedTiles = 0;
		for (uint uiPlotIndex = 0; uiPlotIndex < aiPlots.size(); uiPlotIndex++)
		{
			// when we encounter the first plot that is invalid, the rest of the list will be invalid
			if (aiPlots[uiPlotIndex] == -1)
			{
				break;
			}

			CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[uiPlotIndex]);
			if (!pPlot)
			{
				continue;
			}

			// plot has city in it, don't count
			if (pPlot->getPlotCity())
			{
				continue;
			}

			if (!pLoopCity->GetCityCitizens()->IsCanWork(pPlot))
			{
				continue;
			}

			iTiles++;

			if (pPlot->getWorkingCity() == pLoopCity)
			{
				iWorkedTiles++;
			}

			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iImprovedTiles++;
			}
		}

		AppendToLog(strHeader, strLog, "Tiles", iTiles);
		AppendToLog(strHeader, strLog, "Worked (non-city) tiles", iWorkedTiles);
		AppendToLog(strHeader, strLog, "Improved tiles", iImprovedTiles);

		// % of worked tiles that are improved
		// improved / worked tiles
		fRatio = 0.0f;
		if (iWorkedTiles > 0)
		{
			fRatio = iImprovedTiles / (float)iWorkedTiles;
		}
		AppendToLog(strHeader, strLog, "improved / worked", fRatio);

		if (bBuildHeader)
		{
			bBuildHeader = false;
			pLog->Msg(strHeader);
		}
		pLog->Msg(strLog);
	}
}

// PRIVATE METHODS

/// See if we want to finish any of our builds by rushing
void CvEconomicAI::DoHurry()
{
	int iLoop = 0;
	OrderData *pOrder = 0;

	CvCity *pLoopCity = 0;
	int iTurnsSaved = 0;
	int iHurryAmount = 0;
	int iHurryAmountAvailable = 0;
	int iI = 0;

	CvCity *pBestHurryCity = NULL;
	int iBestHurryTurnsSaved = 0;
	int iBestHurryAmount = 0;
	int iBestHurryAmountAvailable = 0;
	HurryTypes eBestHurryType = NO_HURRY;

	// Look at each of our cities
	for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		// What are we currently working on?
		pOrder = pLoopCity->getOrderFromQueue(0);

		// Did we want to rush it?
		if (pOrder != NULL && pOrder->bRush)
		{
			for (iI = 0; iI < GC.getNumHurryInfos(); iI++)
			{
				iHurryAmount = 0;

				if (pLoopCity->canHurry((HurryTypes)iI) &&
					pLoopCity->hurryProduction((HurryTypes)iI) > 0)
				{
					iTurnsSaved = pLoopCity->getProductionTurnsLeft() - 1;
					if (iTurnsSaved > 0)
					{
						if (GC.getHurryInfo((HurryTypes)iI)->getGoldPerProduction() > 0)
						{
							// Don't gold rush at all anymore (save gold to buy tiles)

							// iHurryAmount = pLoopCity->hurryGold((HurryTypes)iI);
							// iHurryAmountAvailable = m_pPlayer->getGold();
						}
						else if (GC.getHurryInfo((HurryTypes)iI)->getProductionPerPopulation() > 0)
						{
							// Only pop rush if over our current Happiness limit
							if (m_pPlayer->IsEmpireUnhappy())
							{
								iHurryAmount = pLoopCity->hurryPopulation((HurryTypes)iI);
								iHurryAmountAvailable = pLoopCity->getPopulation();
							}
						}

						// Current hurry heuristic is to hurry whichever item has the most turns still to complete
						if (iHurryAmount > 0 && iTurnsSaved >= iBestHurryTurnsSaved)
						{
							iBestHurryTurnsSaved = iTurnsSaved;
							iBestHurryAmount = iHurryAmount;
							iBestHurryAmountAvailable = iHurryAmountAvailable;
							pBestHurryCity = pLoopCity;
							eBestHurryType = (HurryTypes)iI;
						}
					}
				}
			}
		}
	}

	// Now enact the best hurry we've found (only hurry one item per turn for now)
	if (pBestHurryCity != NULL)
	{
		pBestHurryCity->hurry(eBestHurryType);
		pBestHurryCity->GetCityStrategyAI()->LogHurry(eBestHurryType, iBestHurryAmount, iBestHurryAmountAvailable, iBestHurryTurnsSaved);
	}
}

/// Spend money buying plots
void CvEconomicAI::DoPlotPurchases()
{
	CvCity *pLoopCity = 0;
	CvCity *pBestCity = NULL;
	int iBestX = -1;
	int iBestY = -1;
	int iTempX = 0, iTempY = 0;

	int iScore = 0;
	int iLoop = 0;

	// No plot buying for minors
	if (m_pPlayer->isMinorCiv())
	{
		return;
	}

	// No plot buying when at war
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if (eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if (m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return;
		}
	}

	// Set up the parameters
	int iBestScore = /*150*/ GC.getAI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE();
	int iCurrentCost = m_pPlayer->GetBuyPlotCost();
	int iGoldForHalfCost = /*1000*/ GC.getAI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM();
	int iBalance = m_pPlayer->GetTreasury()->GetGold();

	// Let's always invest any money we have in plot purchases
	//  (LATER -- save up money to spend at newly settled cities)
	if (iCurrentCost < iBalance && iGoldForHalfCost > iCurrentCost)
	{
		// Lower our requirements if we're building up a sizable treasury
		int iDiscountPercent = 50 * (iBalance - iCurrentCost) / (iGoldForHalfCost - iCurrentCost);
		iBestScore = iBestScore - (iBestScore * iDiscountPercent / 100);

		// Find the best city to buy a plot
		for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if (pLoopCity->CanBuyAnyPlot())
			{
				iScore = pLoopCity->GetBuyPlotScore(iTempX, iTempY);

				if (iScore > iBestScore)
				{
					pBestCity = pLoopCity;
					iBestScore = iScore;
					iBestX = iTempX;
					iBestY = iTempY;
				}
			}
		}

		if (pBestCity != NULL)
		{
			if (iBestX != -1 && iBestY != -1)
			{
				int iCost = pBestCity->GetBuyPlotCost(iBestX, iBestY);

				if (CanWithdrawMoneyForPurchase(PURCHASE_TYPE_TILE, iCost, iBestScore))
				{
					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Buying plot, X: %d, Y: %d, Cost: %d, Balance (before buy): %d, Priority: %d", iBestX, iBestY,
							iCost, m_pPlayer->GetTreasury()->GetGold(), iBestScore);
						m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
					}
					pBestCity->BuyPlot(iBestX, iBestY);
				}
			}
		}
	}
}


/// Determine how our recon efforts are going
void CvEconomicAI::DoReconState()
{
	int iPlotLoop, iDirectionLoop, iUnitLoop;
	CvPlot* pPlot;
	CvPlot* pAdjacentPlot;
	CvUnit* pLoopUnit;

	if (GetPlayer()->isMinorCiv())
	{
		m_eReconState = RECON_STATE_ENOUGH;
		m_eNavalReconState = RECON_STATE_ENOUGH;
		return;
	}

	// Start at 1 so we don't get divide-by-0 errors
	//   Land recon counters
	int iNumLandPlotsRevealed = 1;
	int iNumLandPlotsWithAdjacentFog = 1;

	//   Naval recon counters
	int iNumCoastalTilesRevealed = 1;
	int iNumCoastalTilesWithAdjacentFog = 1;

	bool bNeedToLookAtDeepWaterAlso = GET_TEAM(m_pPlayer->getTeam()).canEmbarkAllWaterPassage();

	// Look at map size and gauge how much of it we know about
	for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);

		if (pPlot->isRevealed(m_pPlayer->getTeam()))
		{
			// Count Revealed Land Plots
			if (!pPlot->isWater())
			{
				iNumLandPlotsRevealed++;
			}
			else if (pPlot->isShallowWater() || bNeedToLookAtDeepWaterAlso)
			{
				iNumCoastalTilesRevealed++;
			}

			// Check adjacent Plots for THEIR visibility
			for (iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; ++iDirectionLoop)
			{
				pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iDirectionLoop));

				if (pAdjacentPlot != NULL)
				{
					// Check to see if adjacent Tile is land or water...
					if (pAdjacentPlot->isWater())
					{
						// This is a slight cheat (because the AI rules out water tiles) but helps prevents the AI from building too many Land explorers
						if ((bNeedToLookAtDeepWaterAlso || pAdjacentPlot->isShallowWater()) && !pAdjacentPlot->isRevealed(m_pPlayer->getTeam()))
						{
							iNumCoastalTilesWithAdjacentFog++;
							break;
						}
					}
					else
					{
						// This is a slight cheat (because the AI rules out water tiles) but helps prevents the AI from building too many Land explorers
						if (!pAdjacentPlot->isRevealed(m_pPlayer->getTeam()))
						{
							iNumLandPlotsWithAdjacentFog++;
							break;
						}
					}
				}
			}
		}
	}

	// RECON ON OUR HOME CONTINENT

	// How many Units do we have exploring or being trained to do this job? The more Units we have the less we want this Strategy
	int iNumExploringUnits = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_EXPLORE, true, false) + m_iExplorersDisbanded;
	int iStrategyWeight = /*100*/ GC.getAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT();
	int iWeightThreshold = 110;  // So result is a number from 10 to 100
	iWeightThreshold -= m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RECON")) *
		/*10*/ GC.getAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR();

	// Safety check even if personality flavor is higher than expected
	if (iWeightThreshold > 100)
	{
		iWeightThreshold = 100;
	}

	iStrategyWeight *= iNumLandPlotsWithAdjacentFog;
	int iNumExplorerDivisor = iNumExploringUnits + /*1*/ GC.getAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR();
	iStrategyWeight /= (iNumExplorerDivisor * iNumExplorerDivisor);
	iStrategyWeight /= (int)sqrt((double)iNumLandPlotsRevealed);

	if (iStrategyWeight > iWeightThreshold)
	{
		m_eReconState = RECON_STATE_NEEDED;
	}
	else
	{
		if (iStrategyWeight > (iWeightThreshold / 4))
		{
			m_eReconState = RECON_STATE_NEUTRAL;
		}
		else
		{
			m_eReconState = RECON_STATE_ENOUGH;

			// Return all/most warriors/spears to normal unit AI since have enough recon.  Keep at least 1 explorer through Turn 100.
			bool bSkipFirst = GC.getGame().getGameTurn() < 100;
			for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
			{
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE && pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_ATTACK))
				{
					if (bSkipFirst)
					{
						bSkipFirst = false;
					}
					else
					{
						pLoopUnit->AI_setUnitAIType(UNITAI_ATTACK);
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Assigning exploring %s back to attack AI, X: %d, Y: %d", pLoopUnit->getName().GetCString(), pLoopUnit->getX(), pLoopUnit->getY());
							m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
						}
					}
				}
			}
		}
	}

	// NAVAL RECON ACROSS THE ENTIRE MAP

	// No coastal cities?  Moot point...
	CvCity *pLoopCity;
	int iCityLoop;
	bool bFoundCoastalCity = false;
	for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL && !bFoundCoastalCity; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		if (pLoopCity->isCoastal())
		{
			bFoundCoastalCity = true;
		}
	}

	if (!bFoundCoastalCity)
	{
		m_eNavalReconState = RECON_STATE_ENOUGH;
	}

	else
	{
		// How many Units do we have exploring or being trained to do this job? The more Units we have the less we want this Strategy
		iNumExploringUnits = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_EXPLORE_SEA, true, true);
		iStrategyWeight = /*100*/ GC.getAI_STRATEGY_EARLY_EXPLORATION_STARTING_WEIGHT();
		iWeightThreshold = 110;  // So result is a number from 10 to 100
		iWeightThreshold -= m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_RECON")) *
			/*10*/ GC.getAI_STRATEGY_EARLY_EXPLORATION_WEIGHT_PER_FLAVOR();

		// Safety check even if personality flavor is higher than expected
		if (iWeightThreshold > 100)
		{
			iWeightThreshold = 100;
		}

		iStrategyWeight *= iNumCoastalTilesWithAdjacentFog;
		iNumExplorerDivisor = iNumExploringUnits + /*1*/ GC.getAI_STRATEGY_EARLY_EXPLORATION_EXPLORERS_WEIGHT_DIVISOR();
		iStrategyWeight /= (iNumExplorerDivisor * iNumExplorerDivisor);
		iStrategyWeight /= (int)sqrt((double)iNumCoastalTilesRevealed);

		if (iStrategyWeight > iWeightThreshold/* || iNumExploringUnits == 0 && iNumCoastalTilesWithAdjacentFog > 50*/)
		{
			m_eNavalReconState = RECON_STATE_NEEDED;
		}
		else
		{
			if (iStrategyWeight > (iWeightThreshold / 4))
			{
				m_eNavalReconState = RECON_STATE_NEUTRAL;
			}
			else
			{
				m_eNavalReconState = RECON_STATE_ENOUGH;

				// Return all/most boats to normal unit AI since have enough recon
				bool bSkipFirst = (m_eNavalReconState == RECON_STATE_NEUTRAL);
				for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
				{
					if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA && pLoopUnit->getUnitInfo().GetUnitAIType(UNITAI_ATTACK_SEA))
					{
						if (bSkipFirst)
						{
							bSkipFirst = false;
						}
						else
						{
							pLoopUnit->AI_setUnitAIType(UNITAI_ATTACK_SEA);
							if (GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("Assigning naval explorer back to attack sea AI to %s, X: %d, Y: %d", pLoopUnit->getName().GetCString(), pLoopUnit->getX(), pLoopUnit->getY());
								m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
							}
						}
					}
				}
			}
		}
	}
}

void CvEconomicAI::DisbandExtraWorkers()
{
	// Are we running at a deficit?
	EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
	bool bInDeficit = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);

	int iGoldSpentOnUnits = m_pPlayer->GetTreasury()->GetExpensePerTurnUnitMaintenance();
	int iAverageGoldPerUnit = iGoldSpentOnUnits / (max(1,m_pPlayer->getNumUnits()));

	if (!bInDeficit && iAverageGoldPerUnit <= 4)
	{
		return;
	}

	double fWorstCaseRatio = 0.25; // one worker for four cities
	int iNumWorkers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
	int iNumCities = m_pPlayer->getNumCities();

	double fCurrentRatio = iNumWorkers / (double)iNumCities;
	if (fCurrentRatio <= fWorstCaseRatio || iNumWorkers == 1)
	{
		return;
	}

	const CvPlotsVector & aiPlots = m_pPlayer->GetPlots();
	int iNumValidPlots = 0;
	int iNumImprovedPlots = 0;
	for (uint ui = 0; ui < aiPlots.size(); ui++)
	{
		if (aiPlots[ui] == -1)
		{
			continue;
		}

		const CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[ui]);
		if (!pPlot)
		{
			continue;
		}

		if (pPlot->isWater() || pPlot->isImpassable() || pPlot->isCity())
		{
			continue;
		}

		iNumValidPlots++;

		if (pPlot->getImprovementType() != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
		{
			iNumImprovedPlots++;
		}
	}

	// potential div by zero
	if (iNumValidPlots <= 0)
	{
		return;
	}

	int iNumUnimprovedPlots = iNumValidPlots - iNumImprovedPlots;

	// less than two thirds of the plots are improved, don't discard anybody
	double fRatio = iNumImprovedPlots / (double)iNumValidPlots;
	if (fRatio < 2/(double)3)
	{
		return;
	}

	int iWorkersPerUnimprovedPlot = 5;
	int iMinWorkers = iNumUnimprovedPlots / iWorkersPerUnimprovedPlot;
	if ((iNumUnimprovedPlots % iWorkersPerUnimprovedPlot) > 0)
	{
		iMinWorkers += 1;
	}

	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if (!pCapital)
	{
		return;
	}

	int iLoop;
	CvCity *pCity;
	for (pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
	{
		if (pCity == pCapital)
		{
			continue;
		}

		if (pCapital->area() == pCity->area() && !pCity->IsRouteToCapitalConnected())
		{
			iMinWorkers += 1;
		}
	}


	if (iNumWorkers <= iMinWorkers)
	{
		return;
	}

	m_iLastTurnWorkerDisbanded = GC.getGame().getGameTurn();

	CvUnit* pUnit = FindWorkerToScrap();
	if (!pUnit)
	{
		return;
	}

	pUnit->scrap();
	LogScrapUnit(pUnit, iNumWorkers, iNumCities, iNumImprovedPlots, iNumValidPlots);
}

/// Go through the plots for the exploration automation to evaluate
void CvEconomicAI::UpdatePlots()
{
	// reset all plots
	for (uint ui = 0; ui < m_aiExplorationPlots.size(); ui++)
	{
		m_aiExplorationPlots[ui] = -1;
		m_aiExplorationPlotRatings[ui] = -1;
	}

	for (uint ui = 0; ui < m_aiGoodyHutPlots.size(); ui++)
	{
		m_aiGoodyHutPlots[ui] = -1;
		m_aiGoodyHutUnitAssignments[ui].Clear();
	}

	// find the center of all the cities
	int iTotalX = 0;
	int iTotalY = 0;
	int iCityCount = 0;
	int iLoopCity = 0;
	CvCity* pLoopCity = NULL;
	for (pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		iTotalX += pLoopCity->getX();
		iTotalY += pLoopCity->getY();
		iCityCount++;
	}

	uint uiExplorationPlotIndex = 0;
	uint uiGoodyHutPlotIndex = 0;
	TeamTypes ePlayerTeam = m_pPlayer->getTeam();

	CvPlot* pPlot;
	for (int i = 0; i < GC.getMap().numPlots(); i++)
	{
		pPlot = GC.getMap().plotByIndexUnchecked(i);
		if (pPlot == NULL)
		{
			continue;
		}

		if (!pPlot->isRevealed(ePlayerTeam))
		{
			continue;
		}

		if (pPlot->isRevealedGoody(ePlayerTeam) && !pPlot->isVisibleEnemyUnit(m_pPlayer->GetID()))
		{
			if (m_aiGoodyHutPlots.size() <= uiGoodyHutPlotIndex)
			{
				m_aiGoodyHutPlots.push_back(-1);
				m_aiGoodyHutUnitAssignments.push_back(GoodyHutUnitAssignment(-1, -1));
			}
			m_aiGoodyHutPlots[uiGoodyHutPlotIndex] = i;
			m_aiGoodyHutUnitAssignments[uiGoodyHutPlotIndex].Clear();
			++uiGoodyHutPlotIndex;
		}
		if (pPlot->HasBarbarianCamp())
		{
			if (pPlot->getNumDefenders(BARBARIAN_PLAYER) == 0)
			{
				if (m_aiGoodyHutPlots.size() <= uiGoodyHutPlotIndex)
				{
					m_aiGoodyHutPlots.push_back(-1);
					m_aiGoodyHutUnitAssignments.push_back(GoodyHutUnitAssignment(-1, -1));
				}
				m_aiGoodyHutPlots[uiGoodyHutPlotIndex] = i;
				m_aiGoodyHutUnitAssignments[uiGoodyHutPlotIndex].Clear();
				++uiGoodyHutPlotIndex;
			}
		}

		DomainTypes eDomain = DOMAIN_LAND;
		if (pPlot->isWater())
		{
			eDomain = DOMAIN_SEA;
		}

		int iScore = ScoreExplorePlot(pPlot, ePlayerTeam, 1, eDomain);
		if (iScore <= 0)
		{
			continue;
		}

		// add an entry for this plot
		if (m_aiExplorationPlots.size() <= uiExplorationPlotIndex)
		{
			m_aiExplorationPlots.push_back(-1);
			m_aiExplorationPlotRatings.push_back(-1);
		}

		m_aiExplorationPlots[uiExplorationPlotIndex] = i;
		m_aiExplorationPlotRatings[uiExplorationPlotIndex] = iScore;
		uiExplorationPlotIndex++;
	}

	// assign explorers to goody huts

	// build explorer list
	CvUnit* pLoopUnit;
	int iLoopUnit;
	m_apExplorers.clear();
	for (pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		// non-automated human-controlled units should not be considered
		if (m_pPlayer->isHuman() && !pLoopUnit->IsAutomated())
		{
			continue;
		}

		if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
		{
			continue;
		}

		if (pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE && pLoopUnit->GetMissionAIType() != MISSIONAI_EXPLORE)
		{
			continue;
		}

		if (pLoopUnit->getArmyID() != FFreeList::INVALID_INDEX)
		{
			continue;
		}

		m_apExplorers.push_back(pLoopUnit);
	}

	if (m_apExplorers.size() >= m_aiGoodyHutPlots.size())
	{
		AssignExplorersToHuts();
	}
	else
	{
		AssignHutsToExplorers();
	}

	m_bExplorationPlotsDirty = false;
}

#define PATH_PLAN_LAST
typedef CvWeightedVector<CvUnit*, 1, true> WeightedUnitVector;
void CvEconomicAI::AssignExplorersToHuts()
{
#if defined(PATH_PLAN_LAST)
	WeightedUnitVector aBestUnitList;
	aBestUnitList.reserve(m_apExplorers.size());
#endif
	CvTwoLayerPathFinder& kPathFinder = GC.getPathFinder();
	for (uint uiGoodyPlots = 0; uiGoodyPlots < m_aiGoodyHutPlots.size(); uiGoodyPlots++)
	{
		if (m_aiGoodyHutPlots[uiGoodyPlots] == -1)
		{
			continue;
		}

		CvPlot* pGoodyPlot = GC.getMap().plotByIndex(m_aiGoodyHutPlots[uiGoodyPlots]);
#if !defined(PATH_PLAN_LAST)
		int iClosestEstimateTurns = MAX_INT;
#endif
		int iUnitID = NO_UNIT;
		int iStepPlotID = -1;

		aBestUnitList.clear();

		for(uint uiExplorer = 0; uiExplorer < m_apExplorers.size(); uiExplorer++)
		{
			CvUnit* pUnit = m_apExplorers[uiExplorer];

			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY());

#if defined(PATH_PLAN_LAST)
			if(pUnit->maxMoves() >= 1)
			{
				int iEstimateTurns = iDistance / (pUnit->maxMoves() / GC.getMOVE_DENOMINATOR());
				aBestUnitList.push_back(pUnit, iEstimateTurns);
			}
#else
			int iEstimateTurns = MAX_INT;
			if(pUnit->maxMoves() >= 1)
			{
				iEstimateTurns = iDistance / (pUnit->maxMoves() / GC.getMOVE_DENOMINATOR());
			}

			if(iEstimateTurns < iClosestEstimateTurns)
			{
				// Now check path
				bool bCanFindPath = kPathFinder.GenerateUnitPath(pUnit, pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
				if(bCanFindPath)
				{
					iClosestEstimateTurns = iEstimateTurns;
					iUnitID = pUnit->GetID();
					CvPlot* pPlot = kPathFinder.GetPathEndTurnPlot();
					if(pPlot)
						iStepPlotID = pPlot->GetPlotIndex();
				}
			}
#endif
		}

#if defined(PATH_PLAN_LAST)
		uint uiListSize;
		if ((uiListSize = aBestUnitList.size()) > 0)
		{
			aBestUnitList.SortItems();			// highest score will be first.
			for (uint i = uiListSize; i--; )	// Go backward, we want the lowest score (distance)
			{
				CvUnit* pUnit = aBestUnitList.GetElement(i);
				bool bCanFindPath = kPathFinder.GenerateUnitPath(pUnit, pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
				if(bCanFindPath)
				{
					iUnitID = pUnit->GetID();
					// Since we've gone through the trouble of calculating a path, save where our turn will end so the Homeland AI doesn't need to re-do the path.
					CvPlot* pPlot = kPathFinder.GetPathEndTurnPlot();
					if(pPlot)
						iStepPlotID = pPlot->GetPlotIndex();
					break;
				}
			}
		}
#endif

		if(iUnitID != NO_UNIT)
		{
			m_aiGoodyHutUnitAssignments[uiGoodyPlots] = GoodyHutUnitAssignment(iUnitID, iStepPlotID);

			FFastVector<CvUnit*> tempExplorers = m_apExplorers;
			m_apExplorers.clear();
			for(uint uiExplorer = 0; uiExplorer < tempExplorers.size(); uiExplorer++)
			{
				if(tempExplorers[uiExplorer]->GetID() != iUnitID)
				{
					m_apExplorers.push_back(tempExplorers[uiExplorer]);
				}
			}
		}
	}
}

//	---------------------------------------------------------------------------
void CvEconomicAI::AssignHutsToExplorers()
{
	FFastVector<unsigned int> aiHutIndices;

	// Create temporary list of huts
	for (uint uiGoodyPlots = 0; uiGoodyPlots < m_aiGoodyHutPlots.size(); uiGoodyPlots++)
	{
		aiHutIndices.push_back(uiGoodyPlots);
	}

	for (uint uiExplorer = 0; uiExplorer < m_apExplorers.size(); uiExplorer++)
	{
		CvUnit* pUnit = m_apExplorers[uiExplorer];
		uint uiHutIndex = MAX_INT;
		int iClosestEstimateTurns = MAX_INT;

		for (uint uiGoody = 0; uiGoody < aiHutIndices.size(); uiGoody++)
		{
			if (m_aiGoodyHutPlots[aiHutIndices[uiGoody]] == -1)
			{
				continue;
			}

			CvPlot* pGoodyPlot = GC.getMap().plotByIndex(m_aiGoodyHutPlots[aiHutIndices[uiGoody]]);
			int iDistance = plotDistance(pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY());

			int iEstimateTurns = MAX_INT;
			if (pUnit->maxMoves() >= 1)
			{
				iEstimateTurns = iDistance / (pUnit->maxMoves() / GC.getMOVE_DENOMINATOR());
			}

			if (iEstimateTurns < iClosestEstimateTurns)
			{
				// Now check path
				bool bCanFindPath = GC.getPathFinder().GenerateUnitPath(pUnit, pUnit->getX(), pUnit->getY(), pGoodyPlot->getX(), pGoodyPlot->getY(), MOVE_TERRITORY_NO_ENEMY | MOVE_MAXIMIZE_EXPLORE | MOVE_UNITS_IGNORE_DANGER /*iFlags*/, true/*bReuse*/);
				if (bCanFindPath)
				{
					iClosestEstimateTurns = iEstimateTurns;
					uiHutIndex = aiHutIndices[uiGoody];
				}
			}
		}

		if (uiHutIndex != MAX_INT)
		{
			m_aiGoodyHutUnitAssignments[uiHutIndex] = GoodyHutUnitAssignment( pUnit->GetID(), -1);

			FFastVector<unsigned int> tempHutIndices = aiHutIndices;
			aiHutIndices.clear();
			for (uint uiHut = 0; uiHut < tempHutIndices.size(); uiHut++)
			{
				if (tempHutIndices[uiHut] != uiHutIndex)
				{
					aiHutIndices.push_back(tempHutIndices[uiHut]);
				}
			}
		}
	}
}

CvUnit* CvEconomicAI::FindWorkerToScrap ()
{
	CvUnit* pLoopUnit = NULL;
	int iUnitLoop = 0;

	// Look at map for loose settlers
	for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		if (!pLoopUnit)
		{
			continue;
		}

		if (pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->IsWork() && !pLoopUnit->IsCombatUnit() && pLoopUnit->getSpecialUnitType() == NO_SPECIALUNIT)
		{
			return pLoopUnit;
		}
	}

	return NULL;
}

/// Log that a strategy is being turned on or off
void CvEconomicAI::LogStrategy(EconomicAIStrategyTypes eStrategy, bool bValue)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString strDesc;
		CvString strLogName;

		// Find the name of this civ and city
		playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		if (GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "FlavorAILog_" + playerName + ".csv";
		}
		else
		{
			strLogName = "FlavorAILog.csv";
		}

		FILogFile *pLog;
		pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format ("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		// Strategy Info
		strTemp.Format ("%s, %d", GC.getEconomicAIStrategyInfo(eStrategy)->GetType(), bValue);
		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);

		// Also - write to Other Logs, so that we know what the player is doing in there as well

		if (GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "TechAILog_" + playerName + ".csv";
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);

			CvString cityName;

			CvCity* pLoopCity;
			int iLoop;

			for (pLoopCity = GetPlayer()->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoop))
			{
				cityName = pLoopCity->getName();
				strLogName = "CityStrategyAILog_" + playerName + "_" + cityName + ".csv";
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

/// Log that a strategy is being turned on or off
void CvEconomicAI::LogScrapUnit(UnitHandle pUnit, int iNumWorkers, int iNumCities, int iNumImprovedPlots, int iNumValidPlots)
{
	if (!GC.getLogging() || !GC.getAILogging())
	{
		return;
	}

	CvString strLogString;
	strLogString.Format("Disbanding worker. %s, X: %d, Y: %d, iNumWorkers: %d, iNumCities: %d, improved/valid plots: %d/%d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY(), iNumWorkers, iNumCities, iNumImprovedPlots, iNumValidPlots);
	m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
}

FDataStream & operator<<(FDataStream & saveTo, const CvPurchaseRequest & readFrom)
{
	saveTo << (int)readFrom.m_eType;
	saveTo << readFrom.m_iAmount;
	saveTo << readFrom.m_iPriority;
	return saveTo;
}

FDataStream & operator>>(FDataStream & loadFrom, CvPurchaseRequest & writeTo)
{
	int iTemp;
	loadFrom >> iTemp;
	writeTo.m_eType = (PurchaseType)iTemp;
	loadFrom >> writeTo.m_iAmount;
	loadFrom >> writeTo.m_iPriority;
	return loadFrom;
}

// NON-MEMBER FUNCTIONS
//
// These are functions that do not need access to the internals of the CvEconomicAI class.
// Keep them as non-member functions to:
// a) simplify the class (improving encapsulation, reducing coupling)
// b) allow their general use by other classes

// Figure out what the WeightThreshold Mod should be by looking at the Flavors for this player & the Strategy
int EconomicAIHelpers::GetWeightThresholdModifier(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	int iWeightThresholdModifier = 0;

	// Look at all Flavors for the Player & this Strategy
	for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		FlavorTypes eFlavor = (FlavorTypes) iFlavorLoop;
		int iPersonalityFlavor = pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
		int iStrategyFlavorMod = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy)->GetPersonalityFlavorThresholdMod(eFlavor);

		iWeightThresholdModifier += (iPersonalityFlavor * iStrategyFlavorMod);
	}

	return iWeightThresholdModifier;
}

/// Do we have an island clear of hostile units to settle on?
bool EconomicAIHelpers::IsAreaSafeForQuickColony(int iAreaID, CvPlayer *pPlayer)
{
	if (iAreaID == -1)
	{
		return false;
	}

	// Can't be capitals area
	CvCity *pCapitalCity = pPlayer->getCapitalCity();
	if (pCapitalCity != NULL)
	{
		if (iAreaID == pCapitalCity->getArea())
		{
			return false;
		}
	}

	CvArea *pArea = GC.getMap().getArea(iAreaID);
	int iBeginSearchX = pArea->getAreaBoundaries().m_iWestEdge;
	int iBeginSearchY = pArea->getAreaBoundaries().m_iSouthEdge;
	int iEndSearchX   = pArea->getAreaBoundaries().m_iEastEdge;
	int iEndSearchY   = pArea->getAreaBoundaries().m_iNorthEdge;

	for (int iPlotX = iBeginSearchX; iPlotX <= iEndSearchX; iPlotX++)
	{
		for (int iPlotY = iBeginSearchY; iPlotY <= iEndSearchY; iPlotY++)
		{
			CvPlot* pPlot = GC.getMap().plotCheckInvalid(iPlotX, iPlotY);
			if (!pPlot)
			{
				continue;
			}

			if (pPlot->getArea() != iAreaID)
			{
				continue;
			}

			if (pPlot->isVisibleEnemyUnit(pPlayer->GetID()))
			{
				return false;
			}
		}
	}
	return true;
}

/// "Need Recon" Player Strategy: chosen by the DoRecon() function
bool EconomicAIHelpers::IsTestStrategy_NeedRecon(CvPlayer *pPlayer)
{
	// Never desperate for explorers if we are at war
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if (eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if (pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return false;
		}
	}

	return (pPlayer->GetEconomicAI()->GetReconState() == RECON_STATE_NEEDED);
}

/// "Enough Recon" Player Strategy: chosen by the DoRecon() function
bool EconomicAIHelpers::IsTestStrategy_EnoughRecon(CvPlayer *pPlayer)
{
	return (pPlayer->GetEconomicAI()->GetReconState() == RECON_STATE_ENOUGH);
}

/// "Really Need Recon Sea" Player Strategy: If we could theoretically start exploring other continents but we don't have any appropriate ship...
bool EconomicAIHelpers::IsTestStrategy_ReallyNeedReconSea(CvPlayer *pPlayer)
{
	if (pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_NEEDED)
	{
		if (GET_TEAM(pPlayer->getTeam()).canEmbarkAllWaterPassage()) // get a caravel out there NOW!
		{
			CvUnit* pLoopUnit;
			CvCity* pLoopCity;
			int iLoop;

			// Current Units
			for (pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
			{
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA && !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
				{
					return false;
				}
			}

			// Figure out which Promotion is the one which makes a unit not cross oceans
			PromotionTypes eOceanImpassablePromotion = NO_PROMOTION;
			for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
				CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
				if(pkPromotionInfo)
				{
					if (pkPromotionInfo->GetTerrainImpassable(TERRAIN_OCEAN) && pkPromotionInfo->GetTerrainPassableTech(TERRAIN_OCEAN) == -1)
					{
						eOceanImpassablePromotion = eLoopPromotion;
						break;
					}
				}
			}

			// Units being trained now
			for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
			{
				if (pLoopCity->isProductionUnit())
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopCity->getProductionUnit());
					if (pkUnitEntry)
					{
						if (pkUnitEntry->GetDomainType() == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
						{
							if (!pkUnitEntry->GetFreePromotions(eOceanImpassablePromotion))
							{
								return false;
							}
						}
					}
				}
			}
			return true;
		}
		else if (GET_TEAM(pPlayer->getTeam()).canEmbark()) // get a trireme out there NOW!
		{
			CvUnit* pLoopUnit;
			CvCity* pLoopCity;
			int iLoop;

			// Current Units
			for (pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
			{
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
				{
					return false;
				}
			}

			// Units being trained now
			for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
			{
				if (pLoopCity->isProductionUnit())
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopCity->getProductionUnit());
					if (pkUnitEntry)
					{
						if (pkUnitEntry->GetDomainType() == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
						{
							return false;
						}
					}
				}
			}
			return true;
		}
	}

	return false;
}

/// "Need Recon Sea" Player Strategy: chosen by the DoRecon() function
bool EconomicAIHelpers::IsTestStrategy_NeedReconSea(CvPlayer *pPlayer)
{
	// Never desperate for explorers if we are at war
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	if (eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if (pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return false;
		}
	}

	return (pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_NEEDED);
}

/// "Enough Recon Sea" Player Strategy: chosen by the DoRecon() function
bool EconomicAIHelpers::IsTestStrategy_EnoughReconSea(CvPlayer *pPlayer)
{
	return (pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_ENOUGH);
}

/// "Naval Map" Player Strategy: the map script will dictate this
bool EconomicAIHelpers::IsTestStrategy_NavalMap(CvPlayer *pPlayer)
{
	if (pPlayer->isHuman())
	{
		return false;
	}

	// Never run this if we are in serious need of defending the stuff we already have
	MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL");
	if (eBuildCriticalDefenses != NO_MILITARYAISTRATEGY)
	{
		if (pPlayer->GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
		{
			return false;
		}
	}

	return (GC.getMap().GetAIMapHint() & 1);
}

/// "Early Expansion" Player Strategy: An early Strategy simply designed to get player up to 3 Cities quickly.
bool EconomicAIHelpers::IsTestStrategy_EarlyExpansion(CvPlayer *pPlayer)
{
	int iDesiredCities;
	int iFlavorExpansion = 0;
	int iFlavorGrowth = 0;

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
	{
		return false;
	}

	iDesiredCities = pPlayer->GetEconomicAI()->GetEarlyCityNumberTarget();
	for (int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes() && (iFlavorExpansion == 0 || iFlavorGrowth == 0); iFlavorLoop++)
	{
		if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_EXPANSION")
		{
			iFlavorExpansion = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavorLoop);
		}
		else if (GC.getFlavorTypes((FlavorTypes)iFlavorLoop) == "FLAVOR_GROWTH")
		{
			iFlavorGrowth = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)iFlavorLoop);
		}
	}

	CvDiplomacyAI* pDiploAI = pPlayer->GetDiplomacyAI();

	int iMaxCultureCities = GC.getAI_GS_CULTURE_MAX_CITIES();

	iDesiredCities = (iDesiredCities * iFlavorExpansion) / max(iFlavorGrowth, 1);
	int iDifficulty = max(0,GC.getGame().getHandicapInfo().GetID() - 3);
	iDesiredCities += iDifficulty;

	if (pDiploAI->IsGoingForCultureVictory())
	{
		if (iDesiredCities > iMaxCultureCities)
		{
			iDesiredCities = iMaxCultureCities;
		}
	}

	iDesiredCities = iDesiredCities < iMaxCultureCities ? iMaxCultureCities : iDesiredCities;

	// scale this based on world size
	const int iDefaultNumTiles = 80*52;

	iDesiredCities = (iDesiredCities * GC.getMap().numPlots()) / iDefaultNumTiles;

	// See how many unowned Tiles there are on this player's landmass
	if (pPlayer->getCapitalCity() != NULL)
	{
		// Make sure city specialization has gotten one chance to specialize the capital before we adopt this
		if (GC.getGame().getGameTurn() > GC.getAI_CITY_SPECIALIZATION_EARLIEST_TURN())
		{
			CvArea* pArea = GC.getMap().getArea(pPlayer->getCapitalCity()->getArea());

			// Is this area still the best to settle?
			int iBestArea, iSecondBestArea;
			pPlayer->GetBestSettleAreas(pPlayer->GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
			if (iBestArea == pArea->GetID())
			{
				int iNumOwnedTiles = pArea->getNumOwnedTiles();
				int iNumUnownedTiles = pArea->getNumUnownedTiles();
				int iNumTiles = max(1,pArea->getNumTiles());

				int iOwnageRatio = iNumOwnedTiles * 100 / iNumTiles;

				int iNumCities = pPlayer->getNumCities() - pPlayer->GetNumPuppetCities();
				int iSettlersOnMap = pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);

				if ( iOwnageRatio < GC.getAI_STRATEGY_AREA_IS_FULL_PERCENT()
					&& (iNumCities + iSettlersOnMap) < iDesiredCities
					&& iNumUnownedTiles >= GC.getAI_STRATEGY_EARLY_EXPANSION_NUM_UNOWNED_TILES_REQUIRED())
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// "Enough Expansion" Player Strategy: Never want a lot of settlers hanging around
bool EconomicAIHelpers::IsTestStrategy_EnoughExpansion(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	int iBestArea;
	int iSecondBestArea;

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
	{
		return true;
	}

	int iSettlersOnMap = pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);
	int iNumSettleAreas = pPlayer->GetBestSettleAreas(pPlayer->GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);

	if (iSettlersOnMap > iNumSettleAreas)
	{
		return true;
	}

	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	int iMaxSettlers = /*2*/ pStrategy->GetWeightThreshold();

	if (iSettlersOnMap >= iMaxSettlers)
	{
		return true;
	}

	return false;
}

/// "Need Happiness" Player Strategy: Time for Happiness?
bool EconomicAIHelpers::IsTestStrategy_NeedHappiness(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if (pPlayer->getTotalPopulation() > 0 && pPlayer->GetUnhappiness() > 0)
	{
		int iExcessHappiness = pPlayer->GetExcessHappiness();

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);	// 1 Weight per HAPPINESS Flavor

		// This will range from 0 to 5. If Happiness is less than this we will activate the strategy
		int iDivisor = /*2*/ pStrategy->GetWeightThreshold();
		iWeightThresholdModifier /= iDivisor;

		if (iExcessHappiness <= iWeightThresholdModifier)
			return true;
	}

	return false;
}

/// "Need Happiness" Player Strategy: REALLY time for Happiness?
bool EconomicAIHelpers::IsTestStrategy_NeedHappinessCritical(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	// If we're losing at war, return false
	if (pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
		return false;

	if (pPlayer->getTotalPopulation() > 0 && pPlayer->GetUnhappiness() > 0)
	{
		int iExcessHappiness = pPlayer->GetExcessHappiness();

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iThreshold = /*-3*/ pStrategy->GetWeightThreshold();

		if (iExcessHappiness <= iThreshold)
			return true;
	}

	return false;
}

/// "Cities Need Naval Growth" Player Strategy: Looks at how many of this player's Cities need NAVAL_GROWTH, and depending on the intrinsic NAVAL_GROWTH Flavor decides whether or not it's worth prioritizing this Flavor on an empire-wide scale
bool EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalGrowth(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	int iNumCitiesNeedNavalGrowth = 0;

	CvCity* pLoopCity;
	int iLoop;

	AICityStrategyTypes eStrategyNeedNavalGrowth = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_GROWTH");

	// CityStrategy doesn't exist in XML, so abort
	if (eStrategyNeedNavalGrowth == NO_AICITYSTRATEGY)
	{
		return false;
	}

	for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyNeedNavalGrowth))
		{
			iNumCitiesNeedNavalGrowth++;
		}
	}

	if (iNumCitiesNeedNavalGrowth > 0)
	{
		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);	// 1 Weight per NAVAL_GROWTH Flavor
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;	// 25

		int iCurrentWeight = (pPlayer->getNumCities() - 1) * 10;
		iCurrentWeight /= iWeightThreshold;

		// See CvStrategyAI::IsTestStrategy_CitiesNeedBorders for a couple examples on how the math here works

		// Do enough of our Cities want NavalGrowth? [Average is 10/30; range is 10/25 to 10/35]
//		if (iNumCitiesNeedNavalGrowth > (GetPlayer()->getNumCities() - 1) / GC.getAI_STRATEGY_CITIES_NAVAL_GROWTH_DIVISOR())	// 3
		if (iNumCitiesNeedNavalGrowth > iCurrentWeight)
		{
			return true;
		}
	}

	return false;
}

/// "Cities Need Naval Tile Improvement" Player Strategy: Looks at how many of this player's Cities need NAVAL_TILE_IMPROVEMENT, and depending on the intrinsic NAVAL_TILE_IMPROVEMENT Flavor decides whether or not it's worth prioritizing this Flavor on an empire-wide scale
bool EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalTileImprovement(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	int iNumCitiesNeedNavalTileImprovement = 0;

	CvCity* pLoopCity;
	int iLoop;

	AICityStrategyTypes eStrategyNeedNavalTileImprovement = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT");

	// CityStrategy doesn't exist in XML, so abort
	if (eStrategyNeedNavalTileImprovement == NO_AICITYSTRATEGY)
	{
		return false;
	}

	for (pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
	{
		if (pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyNeedNavalTileImprovement))
		{
			iNumCitiesNeedNavalTileImprovement++;
		}
	}

	if (iNumCitiesNeedNavalTileImprovement > 0)
	{
		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);	// 1 Weight per NAVAL_TILE_IMPROVEMENT Flavor
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;	// 25

		int iCurrentWeight = (pPlayer->getNumCities() - 1) * 10;
		iCurrentWeight /= iWeightThreshold;

		// See CvStrategyAI::IsTestStrategy_CitiesNeedBorders for a couple examples on how the math here works

		// Do enough of our Cities want NavalTileImprovement? [Average is 10/30; range is 10/25 to 10/35]
		if (iNumCitiesNeedNavalTileImprovement > iCurrentWeight)
		{
			return true;
		}
	}

	return false;
}

/// "Found City" Player Strategy: If there is a settler who isn't in an operation?  If so, find him a city site
// Very dependent on the fact that the player probably won't have more than 2 settlers available at a time; needs an
//   upgrade if that assumption is no longer true
bool EconomicAIHelpers::IsTestStrategy_FoundCity(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	int iUnitLoop;
	CvUnit *pLoopUnit;
	CvUnit *pFirstSettler = 0;
	int iLooseSettler = 0;
	int iStrategyWeight = 0;
	int iFirstSettlerArea = -1;
	int iBestArea;
	int iSecondBestArea;
	int iNumAreas;
	//int iNumFoundCity;
	//int iNumColonize;

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
	{
		return false;
	}

	// Never run this strategy for a human player
	if (!pPlayer->isHuman())
	{
		// Look at map for loose settlers
		for (pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
		{
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_SETTLE)
				{
					if (pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
					{
						iLooseSettler++;
						if (iLooseSettler == 1)
						{
							iFirstSettlerArea = pLoopUnit->getArea();
							pFirstSettler = pLoopUnit;
						}
					}
				}
			}
		}

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		iStrategyWeight = iLooseSettler * 10;   // Just one settler will trigger this
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		if (iStrategyWeight >= iWeightThreshold)
		{
			iNumAreas = pPlayer->GetBestSettleAreas(pPlayer->GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
			if (iNumAreas == 0)
			{
				return false;
			}

			CvPlot* pBestSettlePlot = pPlayer->GetBestSettlePlot(pFirstSettler, true);
			if (!pBestSettlePlot)
			{
				return false;
			}
			int iArea = pBestSettlePlot->getArea();

			bool bCanEmbark = GET_TEAM(pPlayer->getTeam()).canEmbark();
			bool bIsAtWarWithSomeone = (GET_TEAM(pPlayer->getTeam()).getAtWarCount(false) > 0);

			// CASE 1: Need a city on this area
			if (iFirstSettlerArea == iArea)
			{
				pPlayer->addAIOperation(AI_OPERATION_FOUND_CITY, NO_PLAYER, iArea);
				return true;
			}

			// CASE 2: Need a city on a safe distant area
			else if (bCanEmbark && IsAreaSafeForQuickColony(iArea, pPlayer))  // Have an overseas we can get to safely
			{
				pPlayer->addAIOperation(AI_OPERATION_QUICK_COLONIZE, NO_PLAYER, iArea);
				return true;
			}

			// CASE 3: My embarked units can fight, I always do quick colonization overseas
			else if (bCanEmbark && pPlayer->GetPlayerTraits()->IsEmbarkedNotCivilian())
			{
				pPlayer->addAIOperation(AI_OPERATION_NOT_SO_QUICK_COLONIZE, NO_PLAYER, iArea);
				return true;
			}

			// CASE 3a: Need a city on a not so safe distant area
			else if (bCanEmbark && !bIsAtWarWithSomeone)  // not at war with anyone
			{
				pPlayer->addAIOperation(AI_OPERATION_NOT_SO_QUICK_COLONIZE, NO_PLAYER, iArea);
				return true;
			}

			// CASE 4: Need a city on distant area
			else if (bCanEmbark)
			{
				pPlayer->addAIOperation(AI_OPERATION_COLONIZE, NO_PLAYER, iArea);
				return true;
			}
		}
	}

	return false;
}


/// "Trade with City State" Player Strategy: If there is a settler who isn't in an operation?  If so, find him a city site
bool EconomicAIHelpers::IsTestStrategy_TradeWithCityState(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	int iUnitLoop;
	CvUnit *pLoopUnit;
	int iLooseMerchant = 0;
	int iStrategyWeight = 0;

	// Never run this strategy for a human player
	if (!pPlayer->isHuman())
	{
		// Look at map for loose merchants
		for (pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
		{
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_MERCHANT && pLoopUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
				{
					if (pLoopUnit->getArmyID() == FFreeList::INVALID_INDEX)
					{
						iLooseMerchant++;
					}
				}
			}
		}

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		iStrategyWeight = iLooseMerchant * 10;   // Just one merchant will trigger this
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		if (iStrategyWeight >= iWeightThreshold)
		{
			// Launch an operation.
			pPlayer->addAIOperation(AI_OPERATION_MERCHANT_DELEGATION);

			// Set this strategy active
			return true;
		}
	}

	return false;
}


/// "Need Improvement" Player Strategy: Do the cities need an improvement that increases a yield? Do we have access to an improvement that builds that yield?
bool EconomicAIHelpers::IsTestStrategy_NeedImprovement(CvPlayer* pPlayer, YieldTypes eYield)
{
	// find the city strategy associated with this issue
	AICityStrategyTypes eCityStrategy = NO_AICITYSTRATEGY;
	switch (eYield)
	{
	case YIELD_FOOD:
		eCityStrategy = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_IMPROVEMENT_FOOD");
		break;
	case YIELD_PRODUCTION:
		eCityStrategy = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_IMPROVEMENT_PRODUCTION");
		break;
	}

	FAssertMsg(eCityStrategy != NO_AICITYSTRATEGY, "No strategy found. What?");
	if (eCityStrategy == NO_AICITYSTRATEGY)
	{
		return false;
	}

	// if enough cities are worried about this problem
	int iNumCities = pPlayer->getNumCities();
	if (iNumCities == 0)
	{
		return false; // no cities, no problem!
	}

	int iCityLoop = 0;
	CvCity* pLoopCity = NULL;
	int iNumCitiesConcerned = 0;
	for (pLoopCity = pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
	{
		if (pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eCityStrategy))
		{
			iNumCitiesConcerned++;
		}
	}

	double fWarningRatio = GC.getAI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO();

	// if not enough cities are upset
	if ((iNumCitiesConcerned / (double)iNumCities) < fWarningRatio)
	{
		return false;
	}

	// see if there's a builder
	int iUnitLoop = 0;
	CvUnit* pLoopUnit = NULL;
	CvUnit* pBuilder = NULL;
	for (pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
	{
		if (pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			pBuilder = pLoopUnit;
			break;
		}
	}

	// if no builder, ignore
	// perhaps prompt a builder?
	if (!pBuilder)
	{
		return false;
	}

	// is there a build that I can create to improve the yield?

	// loop through the build types to find one that we can use
	BuildTypes eBuild;
	int iBuildIndex;
	for (iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if (pkBuildInfo)
		{
			if (pkBuildInfo->getTechPrereq() != NO_TECH)
			{
				if (!(GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechPrereq())))
				{
					// don't have the tech needed for this build, so look at the next one
					continue;
				}
			}

			ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild)->getImprovement();
			if (eImprovement == NO_IMPROVEMENT)
			{
				continue;
			}

			CvPlotsVector& aPlots = pPlayer->GetPlots();
			bool bCanBuild = false;
			for (uint ui = 0; ui < aPlots.size(); ui++)
			{
				if (aPlots[ui] < 0)
				{
					break;
				}

				CvPlot* pPlot = GC.getMap().plotByIndex(aPlots[ui]);
				if (pPlayer->canBuild(pPlot, eBuild, false /*test era*/, false /*test visible*/, false /*test gold*/))
				{
					bCanBuild = true;
					break;
				}
			}

			if (!bCanBuild)
			{
				continue;
			}

			// we can use an improvement that increases the yield
			if (GC.getImprovementInfo(eImprovement)->GetYieldChange(eYield) > 0)
			{
				return false;
			}
		}
	}

	return true;
}

/// "One or Fewer Coastal Cities" Player Strategy: If we don't have 2 coastal cities, this runs nullifying the WATER_CONNECTION Flavor
bool EconomicAIHelpers::IsTestStrategy_OneOrFewerCoastalCities(CvPlayer *pPlayer)
{
	int iCityLoop = 0;
	CvCity* pLoopCity = NULL;
	int iNumCoastalCities = 0;
	for (pLoopCity = pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
	{
		if (pLoopCity->isCoastal())
		{
			iNumCoastalCities++;
		}
	}
	return iNumCoastalCities <= 1;
}

/// "Losing Money" Player Strategy: Stop building military if in a financial downturn
bool EconomicAIHelpers::IsTestStrategy_LosingMoney(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	int iInterval = pStrategy->GetMinimumNumTurnsExecuted();

	// Need a certain number of turns of history before we can turn this on
	if (GC.getGame().getGameTurn() <= iInterval)
	{
		return false;
	}

	// Is average income below desired threshold over past X turns?
	return (pPlayer->GetTreasury()->AverageIncome(iInterval) < (double)pStrategy->GetWeightThreshold() /* 2 */);
}

/// "Halt Growth Buildings" Player Strategy: Stop building granaries if working on a wonder that provides them for free
bool EconomicAIHelpers::IsTestStrategy_HaltGrowthBuildings(CvPlayer *pPlayer)
{
	BuildingTypes eNextDesiredWonder = pPlayer->GetCitySpecializationAI()->GetNextWonderDesired();

	// Is average income below desired threshold over past X turns?
	if (eNextDesiredWonder != NO_BUILDING)
	{
		CvBuildingXMLEntries* pkGameBuildings = GC.GetGameBuildings();
		CvBuildingEntry* pkBuildingInfo = pkGameBuildings->GetEntry(eNextDesiredWonder);
		if(pkBuildingInfo)
		{
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)pkBuildingInfo->GetFreeBuildingClass();
			if (eBuildingClass != NO_BUILDINGCLASS)
			{
				CvBuildingClassInfo* pkBuildingClassInfo = GC.getBuildingClassInfo(eBuildingClass);
				{
					if(pkBuildingClassInfo)
					{
						BuildingTypes eBuilding = (BuildingTypes)pkBuildingClassInfo->getDefaultBuildingIndex();
						if (eBuilding != NO_BUILDING)
						{
							CvBuildingEntry* pkFreeBuildingInfo = pkGameBuildings->GetEntry(eBuilding);
							if(pkFreeBuildingInfo)
							{
								if (pkFreeBuildingInfo->GetYieldChange(YIELD_FOOD) > 0)
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	return false;
}

/// Are we paying more in unit maintenance than we are taking in from our cities?
bool EconomicAIHelpers::IsTestStrategy_TooManyUnits(CvPlayer *pPlayer)
{
	return (pPlayer->GetUnitProductionMaintenanceMod()) != 0;
}

/// Did we start the game on a small continent (50 tiles or less)?
bool EconomicAIHelpers::IsTestStrategy_IslandStart(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	int iStartArea;
	CvPlot *pLoopPlot;
	int iCoastalTiles = 0;
	int iRevealedCoastalTiles = 0;

	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);

	// Only kick off this strategy in the first 25 turns of the game (though it will last 50 turns once selected)
	if (GC.getGame().getGameTurn() < 25 && pPlayer->getStartingPlot())
	{
		if (GET_TEAM(pPlayer->getTeam()).getCanEmbarkCount() < 1)
		{
			iStartArea = pPlayer->getStartingPlot()->getArea();

			// Have we revealed a high enough percentage of the coast of our landmass?
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
				if (pLoopPlot->getArea() == iStartArea)
				{
					if (pLoopPlot->isCoastalLand())
					{
						iCoastalTiles++;
					}
					if (pLoopPlot->isRevealed(pPlayer->getTeam()))
					{
						iRevealedCoastalTiles++;
					}
				}
			}

			if ((iRevealedCoastalTiles * 100 / (iCoastalTiles + 1)) > GC.getAI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT() &&
				GC.getMap().getArea(iStartArea)->getNumTiles() < pStrategy->GetWeightThreshold())
			{
				return true;
			}
		}
	}

	return false;
}

/// Are we running out of room on our current landmass?
bool EconomicAIHelpers::IsTestStrategy_ExpandToOtherContinents(CvPlayer *pPlayer)
{
	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
	{
		return false;
	}

	// Never run this at the same time as island start
	EconomicAIStrategyTypes eStrategyIslandStart = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ISLAND_START");
	if (eStrategyIslandStart != NO_ECONOMICAISTRATEGY)
	{
		if (pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyIslandStart))
		{
			return false;
		}
	}

	// we should settle our island first
	EconomicAIStrategyTypes eEarlyExpansion = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
	if (eEarlyExpansion != NO_ECONOMICAISTRATEGY)
	{
		if (pPlayer->GetEconomicAI()->IsUsingStrategy(eEarlyExpansion))
		{
			return false;
		}
	}

	// Never desperate to settle distant lands if we are at war (unless we are doing okay at the war)
	MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	if (eStrategyAtWar != NO_MILITARYAISTRATEGY)
	{
		if (pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
		{
			return false;
		}
	}

	// Never run this if we are in serious need of defending the stuff we already have
	// ??? How about at extreme difficulties?
	//MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL");
	//if (eBuildCriticalDefenses != NO_MILITARYAISTRATEGY)
	//{
	//	if (pPlayer->GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
	//	{
	//		return false;
	//	}
	//}

	// Never run this if we are going for a cultural victory since it will likely derail that
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
	if (eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if (pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			// Does number of cities matter?
			if (GC.getMap().getWorldInfo().GetNumCitiesPolicyCostMod() > 0)
			{
				int iMaxCultureCities = GC.getAI_GS_CULTURE_MAX_CITIES();
				// scale this based on world size
				const int iDefaultNumTiles = 80*52;

				iMaxCultureCities = (iMaxCultureCities * GC.getMap().numPlots()) / iDefaultNumTiles;

				if (pPlayer->getNumCities() >= iMaxCultureCities)
				{
					return false;
				}
			}
		}
	}

	if (pPlayer->getCapitalCity() != NULL)
	{
		CvArea* pArea = GC.getMap().getArea(pPlayer->getCapitalCity()->getArea());

		// Do we have another area to settle (either first or second choice)?
		int iBestArea, iSecondBestArea;
		pPlayer->GetBestSettleAreas(pPlayer->GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);

		if ( (iBestArea != pArea->GetID() && iBestArea != -1) || (iSecondBestArea != pArea->GetID() && iSecondBestArea != -1))
		{
			return true;
		}
	}

	return false;
}

/// Are we mostly on the coast?
bool EconomicAIHelpers::IsTestStrategy_MostlyOnTheCoast(CvPlayer *pPlayer)
{
	// total pop
	int iInlandPop = 0;
	int iCoastalPop = 0;
	int iLoopCity = 0;
	CvCity* pLoopCity = NULL;
	for (pLoopCity = pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoopCity))
	{
		if (pLoopCity->isCoastal(100)) // don't run this for lakes or even small inland seas
		{
			iCoastalPop += pLoopCity->getPopulation();
		}
		else
		{
			iInlandPop += pLoopCity->getPopulation();
		}
	}
	return (iCoastalPop > 0 && iCoastalPop >= iInlandPop);
}

bool EconomicAIHelpers::IsTestStrategy_ExpandLikeCrazy(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
	{
		return false;
	}

	// Never run this if we are going for a cultural victory since it will derail that
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
	if (eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if (pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			// Does number of cities matter?
			if (GC.getMap().getWorldInfo().GetNumCitiesPolicyCostMod() > 0)
			{
				return false;
			}
		}
	}

	int iFlavorExpansion = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));
	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	if (iFlavorExpansion >= pStrategy->GetWeightThreshold())
	{
		return true;
	}

	return false;
}

bool EconomicAIHelpers::IsTestStrategy_GrowLikeCrazy(EconomicAIStrategyTypes eStrategy, CvPlayer *pPlayer)
{
	int iFlavorGrowth = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	if (iFlavorGrowth >= pStrategy->GetWeightThreshold())
	{
		return true;
	}

	return false;
}

// MORE NON-MEMBER FUNCTIONS
//    This set of non member functions are used to evaluate which operation to launch

/// "Grand Strategy Culture" Player Strategy: Run this if our Grand Strategy is to go for Culture
bool EconomicAIHelpers::IsTestStrategy_GS_Culture(CvPlayer *pPlayer)
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");

	if (eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if (pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}

/// "Grand Strategy Conquest" Player Strategy: Run this if our Grand Strategy is to go for Conquest
bool EconomicAIHelpers::IsTestStrategy_GS_Conquest(CvPlayer *pPlayer)
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");

	if (eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if (pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}

/// "Grand Strategy Diplomacy" Player Strategy: Run this if our Grand Strategy is to go for Diplomacy
bool EconomicAIHelpers::IsTestStrategy_GS_Diplomacy(CvPlayer *pPlayer)
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS");

	if (eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if (pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}

/// "Grand Strategy Spaceship" Player Strategy: Run this if our Grand Strategy is to go for the Spaceship
bool EconomicAIHelpers::IsTestStrategy_GS_Spaceship(CvPlayer *pPlayer)
{
	AIGrandStrategyTypes eGrandStrategy = (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP");

	if (eGrandStrategy != NO_AIGRANDSTRATEGY)
	{
		if (pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy() == eGrandStrategy)
		{
			return true;
		}
	}

	return false;
}

/// We are in the home stretch of the "Grand Strategy Spaceship" Player Strategy
bool EconomicAIHelpers::IsTestStrategy_GS_SpaceshipHomestretch(CvPlayer *pPlayer)
{
	// if I already built the Apollo Program I should follow through
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
	if (eApolloProgram != NO_PROJECT)
	{
		if (GET_TEAM(pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			return true;
		}
	}

	return false;
}