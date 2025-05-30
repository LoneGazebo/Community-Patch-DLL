﻿/*	-------------------------------------------------------------------------------------------------------
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
#include "CvImprovementClasses.h"
#include "CvAStar.h"
#include "CvCitySpecializationAI.h"
#include "CvTypes.h"
#include "cvStopWatch.h"
#include "CvSpanSerialization.h"
#include "CvGameCoreEnumSerialization.h"
#include "CvInternalGameCoreUtils.h"

// must be included after all other headers
#include "LintFree.h"

FDataStream& operator>>(FDataStream& loadFrom, PurchaseType& writeTo)
{
	return ReadBasicEnum<int>(loadFrom, writeTo);
}
FDataStream& operator<<(FDataStream& saveTo, const PurchaseType& readFrom)
{
	return WriteBasicEnum<int>(saveTo, readFrom);
}

FDataStream& operator>>(FDataStream& loadFrom, ReconState& writeTo)
{
	return ReadBasicEnum<int>(loadFrom, writeTo);
}
FDataStream& operator<<(FDataStream& saveTo, const ReconState& readFrom)
{
	return WriteBasicEnum<int>(saveTo, readFrom);
}

//=====================================
// CvEconomicAIStrategyXMLEntry
//=====================================
CvEconomicAIStrategyXMLEntry::CvEconomicAIStrategyXMLEntry(void):
	m_piPlayerFlavorValue(NULL),
	m_piCityFlavorValue(NULL),
	m_iWeightThreshold(0),
	m_piPersonalityFlavorThresholdMod(NULL),
	m_bNoMinorCivs(false),
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
	SAFE_DELETE_ARRAY(m_piPlayerFlavorValue);
	SAFE_DELETE_ARRAY(m_piCityFlavorValue);
	SAFE_DELETE_ARRAY(m_piPersonalityFlavorThresholdMod);
}
//------------------------------------------------------------------------------
bool CvEconomicAIStrategyXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_bNoMinorCivs					= kResults.GetBool("NoMinorCivs");
	m_iCheckTriggerTurnCount		= kResults.GetInt("CheckTriggerTurnCount");
	m_iMinimumNumTurnsExecuted		= kResults.GetInt("MinimumNumTurnsExecuted");
	m_iWeightThreshold				= kResults.GetInt("WeightThreshold");
	m_iFirstTurnExecuted            = kResults.GetInt("FirstTurnExecuted");

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
	m_iAdvisorCounselImportance = kResults.GetInt("AdvisorCounselImportance");

	//References
	const char* szTextVal = NULL;
	szTextVal = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("TechObsolete");
	m_iTechObsolete = GC.getInfoTypeForString(szTextVal, true);

	//Arrays
	//Arrays
	const char* szType = GetType();
	kUtility.SetFlavors(m_piPlayerFlavorValue, "AIEconomicStrategy_Player_Flavors",
	                    "AIEconomicStrategyType", szType);
	kUtility.SetFlavors(m_piCityFlavorValue, "AIEconomicStrategy_City_Flavors",
		"AIEconomicStrategyType", szType);


	kUtility.SetFlavors(m_piPersonalityFlavorThresholdMod,
	                    "AIEconomicStrategy_PersonalityFlavorThresholdMods",
	                    "AIEconomicStrategyType", szType);

	return true;
}

/// What player flavors will be added by adopting this Strategy?
int CvEconomicAIStrategyXMLEntry::GetPlayerFlavorValue(int i) const
{
	ASSERT_DEBUG(i < GC.getNumFlavorTypes(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	return m_piPlayerFlavorValue ? m_piPlayerFlavorValue[i] : -1;
}

/// What city flavors will be added by adopting this Strategy?
int CvEconomicAIStrategyXMLEntry::GetCityFlavorValue(int i) const
{
	ASSERT_DEBUG(i < GC.getNumFlavorTypes(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	return m_piCityFlavorValue ? m_piCityFlavorValue[i] : -1;
}

/// The amount of weight a Strategy must accumulate to be adopted (if applicable)
int CvEconomicAIStrategyXMLEntry::GetWeightThreshold() const
{
	return m_iWeightThreshold;
}

/// How do a player's Personality Flavors affect the Threshold for adopting a Strategy? (if applicable)
int CvEconomicAIStrategyXMLEntry::GetPersonalityFlavorThresholdMod(int i) const
{
	ASSERT_DEBUG(i < GC.getNumFlavorTypes(), "Index out of bounds");
	ASSERT_DEBUG(i > -1, "Index out of bounds");
	return m_piPersonalityFlavorThresholdMod ? m_piPersonalityFlavorThresholdMod[i] : -1;
}

/// Is this a valid strategy for Minor Civs to use?
bool CvEconomicAIStrategyXMLEntry::IsNoMinorCivs() const
{
	return m_bNoMinorCivs;
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

AdvisorTypes CvEconomicAIStrategyXMLEntry::GetAdvisor() const
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
	for(std::vector<CvEconomicAIStrategyXMLEntry*>::iterator it = m_paAIStrategyEntries.begin(); it != m_paAIStrategyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAIStrategyEntries.clear();
}

/// Get a specific entry
CvEconomicAIStrategyXMLEntry* CvEconomicAIStrategyXMLEntries::GetEntry(int index)
{
	return (index != NO_ECONOMICAISTRATEGY) ? m_paAIStrategyEntries[index] : NULL;
}

//=====================================
// CvEconomicAI
//=====================================

/// Constructor
CvEconomicAI::CvEconomicAI():
	m_pabUsingStrategy(NULL),
	m_paiTurnStrategyAdopted(NULL)
{
}

/// Destructor
CvEconomicAI::~CvEconomicAI(void)
{
	Uninit();
}

/// Initialize
void CvEconomicAI::Init(CvEconomicAIStrategyXMLEntries* pAIStrategies, CvPlayer* pPlayer)
{
	// Store off the pointer to the AIStrategies active for this game
	m_pAIStrategies = pAIStrategies;

	m_pPlayer = pPlayer;

	// Initialize arrays
	ASSERT_DEBUG(m_pabUsingStrategy==NULL, "about to leak memory, CvStrategyAI::m_pabUsingStrategy");
	m_pabUsingStrategy = FNEW(bool[m_pAIStrategies->GetNumEconomicAIStrategies()], c_eCiv5GameplayDLL, 0);

	ASSERT_DEBUG(m_paiTurnStrategyAdopted==NULL, "about to leak memory, CvStrategyAI::m_paiTurnStrategyAdopted");
	m_paiTurnStrategyAdopted = FNEW(int[m_pAIStrategies->GetNumEconomicAIStrategies()], c_eCiv5GameplayDLL, 0);

	m_aiTempFlavors.init();

	m_RequestedSavings.clear();

	Reset();
}

/// Deallocate memory created in initialize
void CvEconomicAI::Uninit()
{
	SAFE_DELETE_ARRAY(m_pabUsingStrategy);
	SAFE_DELETE_ARRAY(m_paiTurnStrategyAdopted);
	m_aiTempFlavors.uninit();
}

/// Reset AIStrategy status array to all false
void CvEconomicAI::Reset()
{
	int iI = 0;

	for(iI = 0; iI < m_pAIStrategies->GetNumEconomicAIStrategies(); iI++)
	{
		m_pabUsingStrategy[iI] = false;
		m_paiTurnStrategyAdopted[iI] = -1;
	}

	m_vPlotsToExploreLand.clear();
	m_vPlotsToExploreSea.clear();

	for(uint ui = 0; ui < NUM_PURCHASE_TYPES; ui++)
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
	m_iVisibleAntiquitySites = 0;
#if defined(MOD_BALANCE_CORE)
	m_iVisibleAntiquitySitesOwn = 0;
	m_iVisibleHiddenAntiquitySitesOwn = 0;
	m_iVisibleAntiquitySitesNeutral = 0;
	m_iExplorersNeeded = 0;
	m_iNavalExplorersNeeded = 0;
#endif
}

///
template<typename EconomicAI, typename Visitor>
void CvEconomicAI::Serialize(EconomicAI& economicAI, Visitor& visitor)
{
	ASSERT_DEBUG(economicAI.m_pAIStrategies != NULL);
	const int iNumStrategies = economicAI.m_pAIStrategies->GetNumEconomicAIStrategies();
	ASSERT_DEBUG(iNumStrategies > 0, "Number of AIStrategies to serialize is expected to greater than 0");
	visitor(MakeConstSpan(economicAI.m_pabUsingStrategy, iNumStrategies));
	visitor(MakeConstSpan(economicAI.m_paiTurnStrategyAdopted, iNumStrategies));

	visitor(economicAI.m_vPlotsToExploreLand);
	visitor(economicAI.m_vPlotsToExploreSea);

	visitor(economicAI.m_eReconState);
	visitor(economicAI.m_eNavalReconState);
	visitor(economicAI.m_iExplorersDisbanded);
	visitor(economicAI.m_iLastTurnWorkerDisbanded);
	visitor(economicAI.m_iVisibleAntiquitySites);
	visitor(economicAI.m_iVisibleAntiquitySitesOwn);
	visitor(economicAI.m_iVisibleHiddenAntiquitySitesOwn);
	visitor(economicAI.m_iVisibleAntiquitySitesNeutral);
	visitor(economicAI.m_iExplorersNeeded);
	visitor(economicAI.m_iNavalExplorersNeeded);

	visitor(economicAI.m_RequestedSavings);
}

/// Serialization read
void CvEconomicAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	CvEconomicAI::Serialize(*this, serialVisitor);
}

/// Serialization write
void CvEconomicAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	CvEconomicAI::Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvEconomicAI& economicAI)
{
	economicAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvEconomicAI& economicAI)
{
	economicAI.Write(stream);
	return stream;
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
	if (eStrategy == NO_ECONOMICAISTRATEGY)
		return false;

	return m_pabUsingStrategy[(int) eStrategy];
}

/// Signifies that eStrategy is now being utilized by this AI Player
void CvEconomicAI::SetUsingStrategy(EconomicAIStrategyTypes eStrategy, bool bValue)
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
int CvEconomicAI::GetTurnStrategyAdopted(EconomicAIStrategyTypes eStrategy)
{
	return m_paiTurnStrategyAdopted[(int) eStrategy];
}

/// Sets the turn number eStrategy was most recently adopted
void CvEconomicAI::SetTurnStrategyAdopted(EconomicAIStrategyTypes eStrategy, int iValue)
{
	if(m_paiTurnStrategyAdopted[(int) eStrategy] != iValue)
	{
		m_paiTurnStrategyAdopted[(int) eStrategy] = iValue;
	}
}

/// Build log filename
CvString CvEconomicAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "PlayerEconomyAILog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "PlayerEconomyAILog.csv";
	}

	return strLogName;
}

void CvEconomicAI::LogEconomyMessage(const CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString szTemp2;
		CvString strPlayerName;
		FILogFile* pLog = NULL;

		strPlayerName = m_pPlayer->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}

// Do Turn & Strategy Trigger Functions beneath this line

/// Called every turn to see what Strategies this player should using (or not)
void CvEconomicAI::DoTurn()
{
	LogMonitor();
	LogCityMonitor();

	// Functions that need to run before we look at strategies
	DoReconState();
	DoAntiquitySites();

	int iStrategiesLoop = 0;

	// Loop through all strategies
	for(iStrategiesLoop = 0; iStrategiesLoop < GetEconomicAIStrategies()->GetNumEconomicAIStrategies(); iStrategiesLoop++)
	{
		EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) iStrategiesLoop;
		CvEconomicAIStrategyXMLEntry* pStrategy = GetEconomicAIStrategies()->GetEntry(iStrategiesLoop);
		CvString strStrategyName = (CvString) pStrategy->GetType();

		// Minor Civs can't run some Strategies
		if(m_pPlayer->isMinorCiv() && pStrategy->IsNoMinorCivs())
		{
			continue;
		}

		bool bTestStrategyStart = true;

		// Do we already have this Strategy adopted?
		if(IsUsingStrategy(eStrategy))
		{
			bTestStrategyStart = false;
		}
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
				{
					bTestStrategyEnd = true;
				}
			}

			if(bTestStrategyEnd && pStrategy->GetMinimumNumTurnsExecuted() > 0)
			{
				// Has the minimum # of turns passed for this Strategy?
				if(GC.getGame().getGameTurn() < GetTurnStrategyAdopted(eStrategy) + pStrategy->GetMinimumNumTurnsExecuted())
				{
					bTestStrategyEnd = false;
				}
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
				// Check all of the Strategy Triggers
				if (strStrategyName == "ECONOMICAISTRATEGY_NEED_RECON")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedRecon(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_RECON")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughRecon(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_REALLY_NEED_RECON_SEA")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ReallyNeedReconSea(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_NEED_RECON_SEA")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedReconSea(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_RECON_SEA")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughReconSea(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_EARLY_EXPANSION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EarlyExpansion(eStrategy, m_pPlayer);
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
				else if (strStrategyName == "ECONOMICAISTRATEGY_INFLUENCE_CITY_STATE")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_InfluenceCityState(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_NEED_IMPROVEMENT_FOOD")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedImprovement(m_pPlayer, YIELD_FOOD);
				else if (strStrategyName == "ECONOMICAISTRATEGY_NEED_IMPROVEMENT_PRODUCTION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedImprovement(m_pPlayer, YIELD_PRODUCTION);
				else if (strStrategyName == "ECONOMICAISTRATEGY_ONE_OR_FEWER_COASTAL_CITIES")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_OneOrFewerCoastalCities(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_LOSING_MONEY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_LosingMoney(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_TOO_MANY_UNITS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_TooManyUnits(m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_ISLAND_START")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_IslandStart(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ExpandToOtherContinents(eStrategy, m_pPlayer);
				else if (strStrategyName == "ECONOMICAISTRATEGY_MOSTLY_ON_THE_COAST")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_MostlyOnTheCoast(m_pPlayer);
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
				else if(strStrategyName == "ECONOMICAISTRATEGY_DEVELOPING_RELIGION")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_DevelopingReligion(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_TECH_LEADER")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_TechLeader(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_ARCHAEOLOGISTS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedArchaeologists(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_ENOUGH_ARCHAEOLOGISTS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_EnoughArchaeologists(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_DIPLOMATS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedDiplomats(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedDiplomatsCritical(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_MUSEUMS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedMuseums(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_NEED_GUILDS")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_NeedGuilds(m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_CONCERT_TOUR")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_ConcertTour(eStrategy, m_pPlayer);
				else if(strStrategyName == "ECONOMICAISTRATEGY_STARTED_PIETY")
					bStrategyShouldBeActive = EconomicAIHelpers::IsTestStrategy_StartedPiety(m_pPlayer);

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
					if(LuaSupport::CallTestAll(pkScriptSystem, "EconomicStrategyCanActivate", args.get(), bResult))
					{
						// Check the result.
						if(!bResult)
						{
							bStrategyShouldBeActive = false;
						}
					}
				}
			}

			CvString shortName = strStrategyName.substr(19);
			if (IsUsingStrategy(eStrategy))
				LogEconomyMessage(CvString::format("%s active, check for %s, result is %s", shortName.c_str(), bTestStrategyEnd ? "end":"nothing", bStrategyShouldBeActive ? "true":"false" ));
			else
				LogEconomyMessage(CvString::format("%s inactive, check for %s, result is %s", shortName.c_str(), bTestStrategyStart ? "start":"nothing", bStrategyShouldBeActive ? "true":"false" ));

			// This variable keeps track of whether or not we should be doing something (i.e. Strategy is active now but should be turned off, OR Strategy is inactive and should be enabled)
			bool bAdoptOrEndStrategy = false;

			// Strategy should be on, and if it's not, turn it on
			if(bStrategyShouldBeActive)
			{
				if(bTestStrategyStart)
				{
					bAdoptOrEndStrategy = true;
				}
				else if(bTestStrategyEnd)
				{
					bAdoptOrEndStrategy = false;
				}
			}
			// Strategy should be off, and if it's not, turn it off
			else
			{
				if(bTestStrategyStart)
				{
					bAdoptOrEndStrategy = false;
				}
				else if(bTestStrategyEnd)
				{
					bAdoptOrEndStrategy = true;
				}
			}

			// Flavor propagation
			if(bAdoptOrEndStrategy)
			{
				int iFlavorLoop = 0;

				// We should adopt this Strategy
				if(bTestStrategyStart)
				{
					SetUsingStrategy(eStrategy, true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = pStrategy->GetPlayerFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeActivePersonalityFlavors(m_aiTempFlavors, pStrategy->GetType(), true);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = pStrategy->GetCityFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeCityFlavors(m_aiTempFlavors, pStrategy->GetType(), true);
				}
				// End the Strategy
				else if(bTestStrategyEnd)
				{
					SetUsingStrategy(eStrategy, false);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = -pStrategy->GetPlayerFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeActivePersonalityFlavors(m_aiTempFlavors, pStrategy->GetType(), false);

					for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
					{
						m_aiTempFlavors[iFlavorLoop] = -pStrategy->GetCityFlavorValue(iFlavorLoop);
					}

					GetPlayer()->GetFlavorManager()->ChangeCityFlavors(m_aiTempFlavors, pStrategy->GetType(), false);
				}
			}
		}
	}

	if (!m_pPlayer->isHuman())
	{
		// This needs to be called first
		m_pPlayer->DoUpdateCoreCitiesForSpaceshipProduction();

		DoHurry();
		DoPlotPurchases();
		DisbandExtraWorkers();
		DisbandExtraArchaeologists();
		DisbandLongObsoleteUnits();
		DisbandUselessSettlers();
		DisbandUselessDiplomats();
		DisbandExtraWorkboats();
		DisbandMiscUnits();
		DisbandUnitsToFreeSpaceshipResources();

		YieldTypes eFocusYield = NO_YIELD;
		if (EconomicAIHelpers::IsTestStrategy_GS_Spaceship(m_pPlayer)) {
			eFocusYield = YIELD_SCIENCE;
		} else if (EconomicAIHelpers::IsTestStrategy_DevelopingReligion(m_pPlayer)) {
			eFocusYield = YIELD_FAITH;
		} else if (EconomicAIHelpers::IsTestStrategy_LosingMoney((EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY", true), m_pPlayer)) {
			eFocusYield = YIELD_GOLD;
		} else {
			eFocusYield = YIELD_CULTURE;
		}
		
		m_pPlayer->GetCulture()->DoSwapGreatWorks(eFocusYield);
	}
}

/// Find the best city to create a Great Work in
CvCity* CvEconomicAI::GetBestGreatWorkCity(CvPlot *pStartPlot, GreatWorkType eGreatWork) const
{
	CvCity* pBestCity = NULL;
	int iBestDistance = MAX_INT;

	// Make sure there is an undamaged city with a Great Work slot
	GreatWorkSlotType eGreatWorkSlot = CultureHelpers::GetGreatWorkSlot(eGreatWork);
	if (m_pPlayer->GetCulture()->HasAvailableGreatWorkSlot(eGreatWorkSlot))
	{
		int iLoop = 0;
		for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if (pLoopCity->getDamage() <= (pLoopCity->GetMaxHitPoints() / 2) && m_pPlayer->GetPlotDanger(pLoopCity)==0 )
			{
				int iDistance = plotDistance(pStartPlot->getX(), pStartPlot->getY(), pLoopCity->getX(), pLoopCity->getY());
				if(iDistance < iBestDistance)
				{
					iBestDistance = iDistance;
					pBestCity = pLoopCity;
				}
			}
		}
	}

	return pBestCity;
}

void AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, const CvString& strValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	strLog += strValue;
	strLog += ",";
}

void AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, int iValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%d,", iValue);
	strLog += str;
}

void AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, float fValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%.2f,", fValue);
	strLog += str;
}

#if defined(MOD_BALANCE_CORE)
const std::vector<SPlotWithScore>& CvEconomicAI::GetExplorationPlots(DomainTypes domain)
{
	return (domain==DOMAIN_SEA) ? m_vPlotsToExploreSea : m_vPlotsToExploreLand;
}

void CvEconomicAI::SetExplorersNeeded(int iValue)
{
	if(iValue != m_iExplorersNeeded)
	{
		m_iExplorersNeeded = iValue;
	}
}

int CvEconomicAI::GetExplorersNeeded() const
{
	return m_iExplorersNeeded;
}

void CvEconomicAI::SetNavalExplorersNeeded(int iValue)
{
	if(iValue != m_iNavalExplorersNeeded)
	{
		m_iNavalExplorersNeeded = iValue;
	}
}

int CvEconomicAI::GetNavalExplorersNeeded() const
{
	return m_iNavalExplorersNeeded;
}

//	---------------------------------------------------------------------------
bool EconomicAIHelpers::IsPotentialNavalExplorer(UnitAITypes eType)
{
	return eType == UNITAI_ATTACK_SEA ||
		eType == UNITAI_RESERVE_SEA ||
		eType == UNITAI_ASSAULT_SEA ||
		(MOD_AI_UNIT_PRODUCTION && eType == UNITAI_SUBMARINE);
}

//compute score for yet-to-be revealed plots
int EconomicAIHelpers::ScoreExplorePlot(CvPlot* pPlot, CvPlayer* pPlayer, DomainTypes eDomainType, bool bEmbarked)
{
	int iResultValue = 0;
	int iSmallScore = 5;
	int iMediumScore = 30;
	int iLargeScore = 80;
	int iJackpot = 1000;

	//adjacent plots should be unrevealed, but the target itself needs to be revealed
	if(!pPlot->isRevealed(pPlayer->getTeam()))
		return 0;

	//No value if we can't go there
	if(!pPlot->isValidMovePlot(pPlayer->GetID()))
		return 0;

	//add goodies - they go away - do not add any permanent scores here - leads to loops
	if(pPlot->isRevealedGoody(pPlayer->getTeam()) && !pPlot->isVisibleEnemyUnit(pPlayer->GetID()))
		iResultValue += iJackpot;
	if(pPlot->HasBarbarianCamp() && pPlot->getNumDefenders(BARBARIAN_PLAYER) == 0)
		iResultValue += iJackpot;
	if (pPlot->isHills() || pPlot->isMountain()) //inca can enter mountains ...
		if (pPlot->isAdjacentNonrevealed(pPlayer->getTeam()))
			iResultValue += iLargeScore;

	CvPlot** aPlotsToCheck = GC.getMap().getNeighborsUnchecked(pPlot);
	for(int iCount=0; iCount<NUM_DIRECTION_TYPES; iCount++)
	{
		const CvPlot* pLoopPlot = aPlotsToCheck[iCount];

		if(pLoopPlot != NULL)
		{
			//no value if revealed already
			if(pLoopPlot->isRevealed(pPlayer->getTeam()))
				continue;
			//Not interested in very small areas we can see completely from the surrounding plots (like arctic lakes)
			if(pLoopPlot->isWater() && pLoopPlot->area()->getNumTiles()<5)
				continue;

			// "cheating" to look to see what the next tile is.
			// a human should be able to do this by looking at the transition from the tile to the next
			FeatureTypes eFeature = pLoopPlot->getFeatureType();
			if (eDomainType==DOMAIN_LAND && !bEmbarked)
			{
				if (pLoopPlot->isIce())
					//there is nothing interesting here
					continue;
				else if (pLoopPlot->isWater() || !pLoopPlot->isValidMovePlot(pPlayer->GetID()))
					//we're not very interested in these plots
					iResultValue += iSmallScore;
				else if(eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature)->getSeeThroughChange() > 0 && !pLoopPlot->isHills())
					//flatland forest/jungle is meh
					iResultValue += iMediumScore;
				else
					//the real deal - gives us a good view
					iResultValue += iLargeScore;
			}
			else
			{
				if (pLoopPlot->isIce())
					//there is nothing interesting here
					continue;
				else if (!pLoopPlot->isWater())
					//after all we're here to find new land!
					iResultValue += iLargeScore;
				else if (pLoopPlot->getNumAdjacentNonrevealed(pPlayer->getTeam()) > 3 || pLoopPlot->isShallowWater())
					//punching into the unknown. or land nearby
					iResultValue += iMediumScore;
				else
					iResultValue += iSmallScore;
			}

			//recon should gravitate towards enemy lands during war.
			if (pLoopPlot->getOwner() != NO_PLAYER && GET_TEAM(GET_PLAYER(pLoopPlot->getOwner()).getTeam()).isAtWar(pPlayer->getTeam()))
			{
				iResultValue *= 2;
			}
		}
	}

	return iResultValue;
}
#endif

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
	if(iPriority != -1)
	{
		int iIndex = (int)ePurchase;
		m_RequestedSavings[iIndex].m_iPriority = iPriority;
	}

	// Copy into temp array and sort by priority
	vector<CvPurchaseRequest> m_TempRequestedSavings = m_RequestedSavings;
	std::stable_sort(m_TempRequestedSavings.begin(), m_TempRequestedSavings.end());

	for(int iI = 0; iI < (int)m_TempRequestedSavings.size(); iI++)
	{
		CvPurchaseRequest request = m_TempRequestedSavings[iI];

		// Is this higher priority than the request we care about?
		if(request.m_eType != ePurchase)
		{
			iBalance -= request.m_iAmount;

			// No money left?
			if(iBalance <= 0)
			{
				return false;
			}
		}

		// Is this the one, if so, check balance remaining
		else if(request.m_eType == ePurchase)
		{
			return (iBalance >=iAmount);
		}
	}

	UNREACHABLE();
	return false;  // Should never reach here
}

/// Returns amount of gold economic AI is willing to release for this type of purchase. May not be full gold balance if higher priority requests are in. Does not actually spend the gold.
int CvEconomicAI::AmountAvailableForPurchase(PurchaseType ePurchase)
{
	int iBalance = m_pPlayer->GetTreasury()->GetGold();

	// Copy into temp array and sort by priority
	vector<CvPurchaseRequest> m_TempRequestedSavings = m_RequestedSavings;
	std::stable_sort(m_TempRequestedSavings.begin(), m_TempRequestedSavings.end());

	for(int iI = 0; iI < (int)m_TempRequestedSavings.size(); iI++)
	{
		CvPurchaseRequest request = m_TempRequestedSavings[iI];

		// Is this higher priority than the request we care about?
		if(request.m_eType != ePurchase)
		{
			iBalance -= request.m_iAmount;

			// No money left?
			if(iBalance <= 0)
			{
				return 0;
			}
		}

		// Is this the one, if so, check balance remaining
		else if(request.m_eType == ePurchase)
		{
			return (iBalance);
		}
	}

	UNREACHABLE();
	return false;  // Should never reach here
}

int CvEconomicAI::GetPurchaseSaveAmount(PurchaseType ePurchase)
{
	return (m_RequestedSavings[(int)ePurchase].m_iAmount);
}

/// What is the ratio of workers we have to the number of cities we have?
double CvEconomicAI::GetWorkersToCitiesRatio()
{
	int iNumWorkers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_WORKER, true); // includes workers currently being produced
	int iNumCities = m_pPlayer->getNumCities();
	double fCurrentRatio = iNumWorkers / (double)iNumCities;

	return fCurrentRatio;
}

/// What is the ratio of our improved plots to all the plots we are able to improve?
double CvEconomicAI::GetImprovedToImprovablePlotsRatio()
{
	int iNumValidPlots = 0;
	int iNumImprovedPlots = 0;
	const PlotIndexContainer& aiPlots = m_pPlayer->GetPlots();
	// go through all the plots the player has under their control
	for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if (!pPlot)
		{
			continue;
		}
		if(pPlot->isWater() || !pPlot->isValidMovePlot(GetPlayer()->GetID()) || pPlot->isCity())
		{
			continue;
		}

		iNumValidPlots++;

		if(pPlot->getImprovementType() != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
		{
			iNumImprovedPlots++;
		}
	}
	// Avoid potential division by 0
	if(iNumValidPlots <= 0)
	{
		return 1.0;
	}
	double fCurrentRatio = iNumImprovedPlots / (double)iNumValidPlots;

	return fCurrentRatio;
}


/// Make a master log so that we can evaluate and compare changes to the economic AI
void CvEconomicAI::LogMonitor(void)
{
	if(!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	// don't log minor civs for now
	if(m_pPlayer->isMinorCiv())
	{
		return;
	}

	static bool bFirstRun = true;
	bool bBuildHeader = false;
	CvString strHeader;
	if(bFirstRun)
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
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "EconomicMonitorLog_" + strPlayerName + ".csv";
	}
	else
	{
		strLogName = "EconomicMonitorLog.csv";
	}

	FILogFile* pLog = NULL;
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
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoopCity))
	{
		iPop += pLoopCity->getPopulation();
	}
	AppendToLog(strHeader, strLog, "Pop", iPop);

	// total yields this turn
	for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
	{
		switch(ui)
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
		case YIELD_CULTURE:
			AppendToLog(strHeader, strLog, "Culture", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_FAITH:
			AppendToLog(strHeader, strLog, "Faith", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_TOURISM:
			AppendToLog(strHeader, strLog, "Tourism", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		case YIELD_GOLDEN_AGE_POINTS:
			AppendToLog(strHeader, strLog, "Golden Age", m_pPlayer->calculateTotalYield((YieldTypes)ui));
			break;
		}
	}

	// Num Techs
	AppendToLog(strHeader, strLog, "Num Techs", GET_TEAM(GetPlayer()->getTeam()).GetTeamTechs()->GetNumTechsKnown());

	// Culture
	AppendToLog(strHeader, strLog, "Policies", GetPlayer()->GetPlayerPolicies()->GetNumPoliciesOwned());
	AppendToLog(strHeader, strLog, "Culture (lifetime)", (float)(GetPlayer()->GetJONSCultureEverGeneratedTimes100() / 100));

	// Faith
	AppendToLog(strHeader, strLog, "Faith Stored", (float)GetPlayer()->GetFaithTimes100() / 100);

	// workers
	int iWorkerCount = 0;
	CvUnit* pLoopUnit = NULL;
	int iLoopUnit = 0;
	for(pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		if(pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			iWorkerCount++;
		}
	}
	AppendToLog(strHeader, strLog, "# Workers", iWorkerCount);
	AppendToLog(strHeader, strLog, "Positive Happiness",  m_pPlayer->GetHappiness());
	AppendToLog(strHeader, strLog, "Negative Happiness", m_pPlayer->GetUnhappiness());
	AppendToLog(strHeader, strLog, "Net Happiness", m_pPlayer->GetExcessHappiness());

	// worked tiles
	int iTiles = 0;
	int iWorkedTiles = 0;
	int iImprovedTiles = 0;
	const PlotIndexContainer& aiPlots = m_pPlayer->GetPlots();
	// go through all the plots the player has under their control
	for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if (!pPlot)
		{
			continue;
		}

		iTiles++;

		// plot has city in it, don't count
		if(pPlot->getPlotCity())
		{
			continue;
		}

		if(pPlot->isBeingWorked())
		{
			iWorkedTiles++;
		}

		if(pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			iImprovedTiles++;
		}
	}

	AppendToLog(strHeader, strLog, "Tiles", iTiles);
	AppendToLog(strHeader, strLog, "Worked (non-city) tiles", iWorkedTiles);
	AppendToLog(strHeader, strLog, "Improved tiles", iImprovedTiles);

	AppendToLog(strHeader, strLog, "Trade units in use", m_pPlayer->GetTrade()->GetNumTradeUnits(true));
	AppendToLog(strHeader, strLog, "Trade Routes available", (int)m_pPlayer->GetTrade()->GetNumTradeRoutesPossible());

	CvTreasury *pTreasury = m_pPlayer->GetTreasury();

	int iInternationalTradeGPT = pTreasury->GetGoldFromCitiesTimes100(false) - pTreasury->GetGoldFromCitiesTimes100(true);
	AppendToLog(strHeader, strLog, "Gold From Cities", pTreasury->GetGoldFromCitiesTimes100(true) / 100);
	AppendToLog(strHeader, strLog, "Gold From Trade Routes", (iInternationalTradeGPT / 100));
	AppendToLog(strHeader, strLog, "Treasury", pTreasury->GetGold());
	AppendToLog(strHeader, strLog, "GPT - Connects", pTreasury->GetCityConnectionGold());
	AppendToLog(strHeader, strLog, "GPT - Diplo", pTreasury->GetGoldPerTurnFromDiplomacy());
	AppendToLog(strHeader, strLog, "GPT - Religion", pTreasury->GetGoldPerTurnFromReligion());
	AppendToLog(strHeader, strLog, "GPT - Traits", pTreasury->GetGoldPerTurnFromTraits());

	int iGoldFromCitiesMinusTR = pTreasury->GetGoldFromCitiesTimes100(true) / 100;
	int iGPTFromDiplomacy = pTreasury->GetGoldPerTurnFromDiplomacy();
	int iGoldFromCityConnect = pTreasury->GetCityConnectionGoldTimes100() / 100;
	int iGPTFromReligion = pTreasury->GetGoldPerTurnFromReligion();
	int iGPTFromTraits = pTreasury->GetGoldPerTurnFromTraits();
	int iTradeRouteGold = (iInternationalTradeGPT / 100);
	int iTotalIncome = iGoldFromCitiesMinusTR + iGPTFromDiplomacy + iGoldFromCityConnect + iGPTFromReligion + iGPTFromTraits + iTradeRouteGold;
	AppendToLog(strHeader, strLog, "Total Income", iTotalIncome);

	// Gold breakdown
	int iExpenses = pTreasury->GetExpensePerTurnUnitMaintenance() + pTreasury->GetBuildingGoldMaintenance() + pTreasury->GetImprovementGoldMaintenance();
	AppendToLog(strHeader, strLog, "Unit Maint", pTreasury->GetExpensePerTurnUnitMaintenance());
	AppendToLog(strHeader, strLog, "Build Maint", pTreasury->GetBuildingGoldMaintenance());
	AppendToLog(strHeader, strLog, "Improve Maint", pTreasury->GetImprovementGoldMaintenance());
	AppendToLog(strHeader, strLog, "Total Expenses", iExpenses);
	AppendToLog(strHeader, strLog, "Net GPT", iTotalIncome - iExpenses);

	if(bBuildHeader)
	{
		pLog->Msg(strHeader);
	}
	pLog->Msg(strLog);
}

void CvEconomicAI::LogCityMonitor()
{
	if(!(GC.getLogging() && GC.getAILogging()))
	{
		return;
	}

	// don't log minor civs for now
	if(m_pPlayer->isMinorCiv())
	{
		return;
	}

	static bool bFirstRun = true;
	bool bBuildHeader = false;
	if(bFirstRun)
	{
		bFirstRun = false;
		bBuildHeader = true;
	}

	// Find the name of this civ and city
	CvString strPlayerName = m_pPlayer->getCivilizationShortDescription();
	CvString strLogName = "EconomicCityMonitorLog.csv";

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
		strLogName = "EconomicCityMonitorLog_" + strPlayerName + ".csv";

	FILogFile* pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

	// per city
	int iLoopCity = 0;
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoopCity))
	{
		vector<float> afCityYields(NUM_YIELD_TYPES, 0);
		vector<int> aiSpecialistsYields(NUM_YIELD_TYPES, 0);

		CvString strHeader = "";
		CvString strLog = "";

		// civ name
		AppendToLog(strHeader, strLog, "Civ Name", strPlayerName);

		// turn
		AppendToLog(strHeader, strLog, "Turn", GC.getGame().getGameTurn());

		// city name
		CvString str = pLoopCity->getName();
		AppendToLog(strHeader, strLog, "City Name", str);

		//	pop
		AppendToLog(strHeader, strLog, "Population", pLoopCity->getPopulation());

		//	total yields
		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			afCityYields[ui] = (float)pLoopCity->getYieldRateTimes100((YieldTypes)ui) / 100;
			switch(ui)
			{
			case YIELD_FOOD:
				AppendToLog(strHeader, strLog, "food", afCityYields[ui]);
				break;
			case YIELD_PRODUCTION:
				AppendToLog(strHeader, strLog, "production", afCityYields[ui]);
				break;
			case YIELD_SCIENCE:
				AppendToLog(strHeader, strLog, "science", afCityYields[ui]);
				break;
			case YIELD_GOLD:
				AppendToLog(strHeader, strLog, "city gold", afCityYields[ui]);
				break;
			case YIELD_CULTURE:
				AppendToLog(strHeader, strLog, "culture", afCityYields[ui]);
				break;
			case YIELD_FAITH:
				AppendToLog(strHeader, strLog, "faith", afCityYields[ui]);
				break;
			case YIELD_TOURISM:
				AppendToLog(strHeader, strLog, "tourism", afCityYields[ui]);
				break;
			case YIELD_GOLDEN_AGE_POINTS:
				AppendToLog(strHeader, strLog, "goldenage", afCityYields[ui]);
				break;
			}
		}

		//	yields / pop
		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			float fRatio = 0.0f;
			if(pLoopCity->getPopulation() > 0)
			{
				fRatio = afCityYields[ui] / pLoopCity->getPopulation();
			}

			switch(ui)
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
			case YIELD_CULTURE:
				AppendToLog(strHeader, strLog, "culture / pop", fRatio);
				break;
			case YIELD_FAITH:
				AppendToLog(strHeader, strLog, "faith / pop", fRatio);
				break;
			case YIELD_TOURISM:
				AppendToLog(strHeader, strLog, "tourism / pop", fRatio);
				break;
			case YIELD_GOLDEN_AGE_POINTS:
				AppendToLog(strHeader, strLog, "goldenage / pop", fRatio);
				break;
			}
		}

		//	yields from specialists
		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			aiSpecialistsYields[ui] = pLoopCity->getExtraSpecialistYield((YieldTypes)ui);
			switch(ui)
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
			case YIELD_CULTURE:
				AppendToLog(strHeader, strLog, "culture specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_FAITH:
				AppendToLog(strHeader, strLog, "faith specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_TOURISM:
				AppendToLog(strHeader, strLog, "tourism specialist", aiSpecialistsYields[ui]);
				break;
			case YIELD_GOLDEN_AGE_POINTS:
				AppendToLog(strHeader, strLog, "goldenage specialist", aiSpecialistsYields[ui]);
				break;
			}
		}

		// ratio from specialists
		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			float fRatio = 0.0f;
			if(afCityYields[ui] > 0)
			{
				fRatio = aiSpecialistsYields[ui] / afCityYields[ui];
			}
			switch(ui)
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
			case YIELD_CULTURE:
				AppendToLog(strHeader, strLog, "culture specialist / culture", fRatio);
				break;
			case YIELD_FAITH:
				AppendToLog(strHeader, strLog, "faith specialist / faith", fRatio);
				break;
			case YIELD_TOURISM:
				AppendToLog(strHeader, strLog, "tourism specialist / tourism", fRatio);
				break;
			case YIELD_GOLDEN_AGE_POINTS:
				AppendToLog(strHeader, strLog, "goldenage specialist / tourism", fRatio);
				break;
			}
		}

		//	% of worked tiles that are improved
		// worked tiles
		int iTiles = 0;
		int iWorkedTiles = 0;
		int iImprovedTiles = 0;
		const PlotIndexContainer& aiPlots = m_pPlayer->GetPlots();
		// go through all the plots the player has under their control
		for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
		{
			CvPlot* pPlot = GC.getMap().plotByIndex(*it);
			if (!pPlot)
			{
				continue;
			}

			// plot has city in it, don't count
			if(pPlot->getPlotCity())
			{
				continue;
			}

			if(!pLoopCity->GetCityCitizens()->IsCanWork(pPlot))
			{
				continue;
			}

			iTiles++;

			if(pPlot->getEffectiveOwningCity() == pLoopCity)
			{
				iWorkedTiles++;
			}

			if(pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iImprovedTiles++;
			}
		}

		AppendToLog(strHeader, strLog, "Tiles", iTiles);
		AppendToLog(strHeader, strLog, "Worked (non-city) tiles", iWorkedTiles);
		AppendToLog(strHeader, strLog, "Improved tiles", iImprovedTiles);

		// % of worked tiles that are improved
		// improved / worked tiles
		float fRatio = 0.0f;
		if(iWorkedTiles > 0)
		{
			fRatio = iImprovedTiles / (float)iWorkedTiles;
		}
		AppendToLog(strHeader, strLog, "improved / worked", fRatio);

		if(bBuildHeader)
		{
			bBuildHeader = false;
			pLog->Msg(strHeader);
		}
		pLog->Msg(strLog);
	}
}

// PRIVATE METHODS

/// See if we want to finish any of our builds by rushing
void CvEconomicAI::LogPossibleHurries(CvWeightedVector<CvCityBuildable> const& m_Buildables)
{
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = m_pPlayer->getCivilizationShortDescription();

		string strLogName = "HurryCityPriorities.csv";
		if (GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "HurryCityPriorities_" + playerName + ".csv";
		}

		FILogFile* pLog = NULL;
		pLog = LOGFILEMGR.GetLog(strLogName.c_str(), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", BUY: ";

		// Dump out the weight of each buildable item
		for (int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvCityBuildable buildable = m_Buildables.GetElement(iI);

			switch (buildable.m_eBuildableType)
			{
			case NOT_A_CITY_BUILDABLE:
				UNREACHABLE(); // m_Buildables is not supposed to contain these items.
			case CITY_BUILDABLE_BUILDING:
			{
				CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(buildable.m_iIndex);
				if (pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Building, %s, %d, %d", strDesc.GetCString(), buildable.m_iValue, buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if (pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Unit, %s, %d, %d", strDesc.GetCString(), buildable.m_iValue, buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROJECT:
			{
				CvProjectEntry* pEntry = GC.GetGameProjects()->GetEntry(buildable.m_iIndex);
				if (pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Project, %s, %d, %d", strDesc.GetCString(), buildable.m_iValue, buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROCESS:
			{
				CvProcessInfo* pProcess = GC.getProcessInfo((ProcessTypes)buildable.m_iIndex);
				if (pProcess != NULL)
				{
					strDesc = pProcess->GetDescription();
					strTemp.Format("Process, %s, %d, %d", strDesc.GetCString(), buildable.m_iValue, buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_OPERATION:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if (pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Operation unit, %s, %d, %d", strDesc.GetCString(), buildable.m_iValue, buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_ARMY:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if (pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Army unit, %s, %d, %d", strDesc.GetCString(), buildable.m_iValue, buildable.m_iTurnsToConstruct);
				}
			}
			break;
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}
void CvEconomicAI::DoHurry()
{
	int iLoop = 0;

	//Let's give the AI a treasury cushion ...
	int iTreasuryBuffer = /*50*/ GD_INT_GET(AI_GOLD_TREASURY_BUFFER);
	// ... modified by gamespeed
	iTreasuryBuffer *= GC.getGame().getGameSpeedInfo().getGoldPercent();
	iTreasuryBuffer /= 100;
	// ... and era
	iTreasuryBuffer *= m_pPlayer->GetCurrentEra();

	CvTreasury* pTreasury = m_pPlayer->GetTreasury();
	int iOurGold = pTreasury->GetGold();

	int iGPT = pTreasury->CalculateBaseNetGold();
	// In debt? Be more careful
	if (iGPT < 0)
	{
		iTreasuryBuffer += 10 * iGPT;
	}
	else
	{
		iTreasuryBuffer = max(0, iTreasuryBuffer - iGPT);
	}


	if(pTreasury->GetGold() < iTreasuryBuffer)
		return;

	int iPurchaseBuffer = 0;
	for(int iPurchaseType = 0; iPurchaseType < NUM_PURCHASE_TYPES; iPurchaseType++)
	{
		PurchaseType ePurchase = (PurchaseType)iPurchaseType;
		if(ePurchase == NO_PURCHASE_TYPE)
			continue;

		if (ePurchase == PURCHASE_TYPE_BUILDING || ePurchase == PURCHASE_TYPE_UNIT)
			continue;

		if(IsSavingForThisPurchase(ePurchase))
		{
			iPurchaseBuffer += GetPurchaseSaveAmount(ePurchase) * 2;
		}
	}

	if(iOurGold <= iPurchaseBuffer)
		return;

	// Which city needs hurrying most?
	vector<CvCity*> threatCities = m_pPlayer->GetThreatenedCities(false);
	CvCity* pMostThreatenedCity = threatCities.empty() ? NULL : threatCities.front();

	CvWeightedVector<CvCityBuildable> m_Buildables;

	// Look at each of our cities
	m_Buildables.clear();
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if (CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(pLoopCity))
			continue;

		//Is the city threatened - don't invest there. Always be able to hurry things in the capital.
		if (pLoopCity == pMostThreatenedCity && !pLoopCity->isCapital())
			continue;

		CvCityBuildable selection = (pLoopCity->GetCityStrategyAI()->ChooseHurry());
		if (selection.m_eBuildableType == NOT_A_CITY_BUILDABLE)
			continue;

		selection.m_iCityID = pLoopCity->GetID();
		m_Buildables.push_back(selection, selection.m_iValue);
	}


	/* diplomats */
	if (MOD_BALANCE_VP && m_pPlayer->HasMetValidMinorCiv() && GC.getGame().isVictoryValid((VictoryTypes)GC.getInfoTypeForString("VICTORY_DIPLOMATIC", true)))
	{
		ResourceTypes ePaper = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_PAPER", true);
		if (m_pPlayer->getNumResourceAvailable(ePaper) > 0)
		{
			UnitTypes eDiplomatUnit = NO_UNIT;
			int iNumPaperPerDiplomat = 0;
			for (int iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
			{
				UnitTypes eUnit = (UnitTypes)iUnitLoop;
				CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
				if (pkUnitEntry->GetDefaultUnitAIType() != UNITAI_MESSENGER)
					continue;

				if (!m_pPlayer->canTrainUnit(eUnit))
					continue;

				// found a diplo unit we can train
				eDiplomatUnit = eUnit;
				iNumPaperPerDiplomat = pkUnitEntry->GetResourceQuantityRequirement(ePaper);
				break;
			}
			if (eDiplomatUnit != NO_UNIT)
			{
				int iNumDiplomatsWeCanBuy = iNumPaperPerDiplomat > 0 ? m_pPlayer->getNumResourceAvailable(ePaper) / iNumPaperPerDiplomat : 5;
				if (iNumDiplomatsWeCanBuy > 0)
				{
					// get lowest purchase cost
					int iLowestPurchaseCost = INT_MAX;
					for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
						iLowestPurchaseCost = min(iLowestPurchaseCost, pLoopCity->GetPurchaseCost(eDiplomatUnit));
					}

					for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
					{
						if (CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(pLoopCity))
							continue;

						//Is the city threatened - don't invest there. Always be able to hurry things in the capital.
						if (pLoopCity == pMostThreatenedCity && !pLoopCity->isCapital())
							continue;

						if (pLoopCity->canTrain(eDiplomatUnit, false, false, false, true))
						{
							CvCityBuildable selection;
							selection.m_eBuildableType = CITY_BUILDABLE_UNIT;
							selection.m_iIndex = (int)eDiplomatUnit;
							selection.m_iCityID = pLoopCity->GetID();
							selection.m_iValue = (m_pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory() ? 4 : 1) * m_pPlayer->GetCurrentEra() * 200 * (100 - 100 * (pLoopCity->GetPurchaseCost(eDiplomatUnit) - iLowestPurchaseCost) / iLowestPurchaseCost) / 100;
							if (selection.m_iValue > 0)
							{
								m_Buildables.push_back(selection, selection.m_iValue);
							}

							iNumDiplomatsWeCanBuy--;
							if (iNumDiplomatsWeCanBuy == 0)
								break;
						}
					}
				}
			}
		}
	}

	if (m_Buildables.size() <= 0)
		return;

	m_Buildables.StableSortItems();
	LogPossibleHurries(m_Buildables);

	int iSelection = 0;
	while (pTreasury->GetGold() > iTreasuryBuffer && iSelection < m_Buildables.size())
	{
		CvCityBuildable selection = m_Buildables.GetElement(iSelection);
		iSelection++;

		CvCity* pSelectedCity = m_pPlayer->getCity(selection.m_iCityID);
		if (pSelectedCity == NULL)
			continue;

		//Units
		switch (selection.m_eBuildableType)
		{
		case NOT_A_CITY_BUILDABLE:
		case CITY_BUILDABLE_PROJECT:
		case CITY_BUILDABLE_PROCESS:
			UNREACHABLE(); // selection is not supposed to be one of these items.
		case CITY_BUILDABLE_UNIT:
		case CITY_BUILDABLE_UNIT_FOR_ARMY:
		case CITY_BUILDABLE_UNIT_FOR_OPERATION:
		{
			UnitTypes eUnitType = (UnitTypes)selection.m_iIndex;
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitType);
			if (pkUnitInfo)
			{
				int iGoldCost = pSelectedCity->GetPurchaseCost(eUnitType);
				if (m_pPlayer->GetEconomicAI()->CanWithdrawMoneyForPurchase(PURCHASE_TYPE_UNIT, iGoldCost))
				{
					// Resource requirement
					if (!m_pPlayer->HasResourceForNewUnit(eUnitType, false, true))
						continue;

					if (MOD_BALANCE_CORE_UNIT_INVESTMENTS || (MOD_BALANCE_CORE && pkUnitInfo->GetSpaceshipProject() != NO_PROJECT))
					{
						const UnitClassTypes eUnitClass = (UnitClassTypes)(pkUnitInfo->GetUnitClassType());
						pSelectedCity->SetUnitInvestment(eUnitClass, true);
						m_pPlayer->GetTreasury()->ChangeGold(-iGoldCost);
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("MOD - Investing in unit: %s in %s. Cost: %d, Balance (before buy): %d",
								pkUnitInfo->GetDescription(), pSelectedCity->getName().c_str(), iGoldCost, m_pPlayer->GetTreasury()->GetGold());
							m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
						}
					}
					else
					{
						CvUnit* pUnit = pSelectedCity->PurchaseUnit(eUnitType, YIELD_GOLD);
						if (pUnit)
						{
							if (GC.getLogging() && GC.getAILogging())
							{
								CvString strLogString;
								strLogString.Format("MOD - Buying unit: %s in %s. Cost: %d, Balance (before buy): %d",
									pkUnitInfo->GetDescription(), pSelectedCity->getName().c_str(), iGoldCost, m_pPlayer->GetTreasury()->GetGold());
								m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
							}

							if (pkUnitInfo->IsFound())
								m_pPlayer->GetMilitaryAI()->ResetNumberOfTimesSettlerBuildSkippedOver();
							if (selection.m_eBuildableType == CITY_BUILDABLE_UNIT_FOR_OPERATION)
								m_pPlayer->GetMilitaryAI()->ResetNumberOfTimesOpsBuildSkippedOver();

							pSelectedCity->CleanUpQueue();
						}
					}
				}
			}
		}
		break;

		case CITY_BUILDABLE_BUILDING:
		{
			BuildingTypes eBuildingType = (BuildingTypes)selection.m_iIndex;
			CvBuildingEntry* pkBuildingInfo = GC.GetGameBuildings()->GetEntry(eBuildingType);
			if (pkBuildingInfo)
			{
				if (pSelectedCity->IsCanPurchase(/*bTestPurchaseCost*/ true, /*bTestTrainable*/ true, NO_UNIT, eBuildingType, NO_PROJECT, YIELD_GOLD))
				{
					int iGoldCost = pSelectedCity->GetPurchaseCost(eBuildingType);
					// New investment code, turns left to produce will influence AI decision
					int iExcessValuation = 0; 
					int iTurnsLeft = pSelectedCity->getProductionTurnsLeft(eBuildingType, 0);

					if (GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("MOD - Considering investing in building: %s in city %s. TurnsLeft: %d, TestValuation: %d",
						pkBuildingInfo->GetDescription(), pSelectedCity->getName().c_str(), iTurnsLeft, iExcessValuation);
						m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
					}

					if (iTurnsLeft == 1)
					{
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("MOD - Can't invest in building: %s in city %s. TurnsLeft: %d",
							pkBuildingInfo->GetDescription(), pSelectedCity->getName().c_str(), iTurnsLeft);
							m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
						}
						continue;
					}
					else if (iTurnsLeft == 2)
						iExcessValuation = (int)(iGoldCost * 1.0);
					else if (iTurnsLeft == 3)
						iExcessValuation = (int)(iGoldCost * 0.5);
					else if (iTurnsLeft == 4)
						iExcessValuation = (int)(iGoldCost * 0.2);

					const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();
					if (::isWorldWonderClass(kBuildingClassInfo)) // if wonder, investing late doesn't matter 
						iExcessValuation = 0;

					if (m_pPlayer->GetEconomicAI()->CanWithdrawMoneyForPurchase(PURCHASE_TYPE_BUILDING, iGoldCost + iExcessValuation)) // ExcessValuation is considered, but not actually deducted from treasury
					{
						//Log it
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							if (MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
								strLogString = "MOD - Investing in building:";
							else
								strLogString = "MOD - Buying building:";

							strLogString.Format("%s %s in %s. Cost: %d, Balance (before buy): %d, TurnsLeft: %d, ExcessValuation: %d",
								strLogString.c_str(), pkBuildingInfo->GetDescription(), pSelectedCity->getName().c_str(),
								iGoldCost, m_pPlayer->GetTreasury()->GetGold(), iTurnsLeft, iExcessValuation);

							m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
						}

						//take the money...
						m_pPlayer->GetTreasury()->ChangeGold(-iGoldCost);

						//and build it!
						if (MOD_BALANCE_CORE_BUILDING_INVESTMENTS)
						{
							const BuildingClassTypes eBuildingClass = pkBuildingInfo->GetBuildingClassType();
							pSelectedCity->SetBuildingInvestment(eBuildingClass, true);
						}
						else
						{
							pSelectedCity->CreateBuilding(eBuildingType);
							pSelectedCity->SetBuildingPurchaseCooldown(pkBuildingInfo->GetCooldown());
							if (MOD_EVENTS_CITY)
							{
								GAMEEVENTINVOKE_HOOK(GAMEEVENT_CityConstructed, pSelectedCity->getOwner(), pSelectedCity->GetID(), eBuildingType, true, false);
							}
							pSelectedCity->CleanUpQueue();
						}
					}
				}
			}
		}
		break;
		}
	}
}

/// Spend money buying plots
void CvEconomicAI::DoPlotPurchases()
{
	CvCity* pBestCity = NULL;
	int iBestX = INVALID_PLOT_COORD;
	int iBestY = INVALID_PLOT_COORD;

	// No plot buying for minors
	if(m_pPlayer->isMinorCiv())
	{
		return;
	}

	// No plot buying when at war and losing
	static MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if (m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar) && m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
	{
		return;
	}

	//Let's give the AI a treasury cushion ...
	int iTreasuryBuffer = /*50*/ GD_INT_GET(AI_GOLD_TREASURY_BUFFER);
	// ... modified by gamespeed
	iTreasuryBuffer *= GC.getGame().getGameSpeedInfo().getGoldPercent();
	iTreasuryBuffer /= 100;
	// ... and era
	iTreasuryBuffer *= m_pPlayer->GetCurrentEra();

	CvTreasury* pTreasury = m_pPlayer->GetTreasury();
	//Let's check our average income over five-turn periods
	int iGPT = pTreasury->CalculateBaseNetGold();
	// In debt? Be more careful
	if (iGPT < 0)
	{
		iTreasuryBuffer += 10 * iGPT;
	}
	else
	{
		iTreasuryBuffer = max(0, iTreasuryBuffer - iGPT);
	}

	iTreasuryBuffer += 200;

	if (pTreasury->GetGold() < iTreasuryBuffer)
		return;


	// Set up the parameters
	int iBestScore = /*150*/ GD_INT_GET(AI_GOLD_PRIORITY_MINIMUM_PLOT_BUY_VALUE);
	int iCurrentCost = m_pPlayer->GetBuyPlotCost();
	int iGoldForHalfCost = /*1000*/ GD_INT_GET(AI_GOLD_BALANCE_TO_HALVE_PLOT_BUY_MINIMUM);
	int iBalance = m_pPlayer->GetTreasury()->GetGold() - iTreasuryBuffer;
	int iBestCost = 0;
	int iMultiplier = max(2, (int)GC.getGame().getCurrentEra());

	// Let's not blow all our money on plot purchases
	if(iCurrentCost*iMultiplier < iBalance && iGoldForHalfCost > iCurrentCost)
	{
		// Lower our requirements if we're building up a sizable treasury
		int iDiscountPercent = 50 * (iBalance - iCurrentCost) / (iGoldForHalfCost - iCurrentCost);
		iBestScore = iBestScore - (iBestScore * iDiscountPercent) / 100;

		// Find the best city to buy a plot
		int iLoop = 0;
		for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(pLoopCity->CanBuyAnyPlot())
			{
				int iTempX = 0;
				int iTempY = 0;
				int iScore = pLoopCity->GetBuyPlotScore(iTempX, iTempY);
				if (iScore == -1)
					continue;

				int iCost = pLoopCity->GetBuyPlotCost(iTempX, iTempY);

				/*
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Looking at buying plot for %s, X: %d, Y: %d, Cost: %d, Balance (before buy): %d, Priority: %d", pLoopCity->getName().c_str(), iTempX, iTempY, iCost, iBalance, iScore);
					m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
				}
				*/

				if(CanWithdrawMoneyForPurchase(PURCHASE_TYPE_TILE, iCost, iScore))
				{
					if(iScore > iBestScore)
					{
						pBestCity = pLoopCity;
						iBestScore = iScore;
						iBestX = iTempX;
						iBestY = iTempY;
						iBestCost = iCost;
					}
				}
				else
				{
					StartSaveForPurchase(PURCHASE_TYPE_TILE, iCost, /*Priority*/ 2);
				}
			}
		}

		if(pBestCity != NULL)
		{
			if(iBestX != INVALID_PLOT_COORD && iBestY != INVALID_PLOT_COORD)
			{
				pBestCity->BuyPlot(iBestX, iBestY);
				if(GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Buying plot, X: %d, Y: %d, Cost: %d, Balance (before buy): %d, Priority: %d", iBestX, iBestY,
						iBestCost, iBalance, iBestScore);
					m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
				}
			}
		}
	}
}

//debug flag
bool g_bNoNewExplorers = false;

/// Determine how our recon efforts are going
void CvEconomicAI::DoReconState()
{
	//debug flag so combat units aren't randomly turned into explorers
	if (g_bNoNewExplorers)
		return;

	if (m_pPlayer->isMinorCiv())
		return;

	int iUnitLoop = 0;
	CvUnit* pLoopUnit = NULL;

	//important. do this exactly once per turn.
	UpdateExplorePlotsFromScratch();
	m_eReconState = RECON_STATE_NEUTRAL;
	m_eNavalReconState = RECON_STATE_NEUTRAL;

	//No scouting if we're cityless!
	if (m_pPlayer->getNumCities() <= 0)
		return;

	bool isCannotRecon = EconomicAIHelpers::CannotMinorCiv(m_pPlayer, (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON"));
	bool isCannotReconSea = EconomicAIHelpers::CannotMinorCiv(m_pPlayer, (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_RECON_SEA"));

	if (isCannotRecon && isCannotReconSea)
	{
		m_eReconState = RECON_STATE_ENOUGH;
		m_eNavalReconState = RECON_STATE_ENOUGH;
		return;
	}

	// Don't bother scouting if we're losing all wars
	if (m_pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
	{
		m_eReconState = RECON_STATE_ENOUGH;
		m_eNavalReconState = RECON_STATE_ENOUGH;
		SetExplorersNeeded(0);
		SetNavalExplorersNeeded(0);
		return;
	}

	// Less desperate for explorers if we are at war
	static MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	bool bWar = (eStrategyAtWar != NO_MILITARYAISTRATEGY) && GetPlayer()->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar);

	// RECON ON OUR HOME CONTINENT

	// How many Units do we have exploring or being trained to do this job?
	int iNumExploringUnits = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_EXPLORE, true);
	int iNumPlotsToExplore = (int)GetExplorationPlots(DOMAIN_LAND).size();

	// estimate one explorer per x open plots, depending on personality (these are only the border plots between known and unknown)
	int iPlotsPerExplorer = /*20 in CP, 27 in VP*/ GD_INT_GET(MAX_PLOTS_PER_EXPLORER) - m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RECON"));
	int iNumExplorersNeededTimes100 = 50 + (iNumPlotsToExplore*100) / iPlotsPerExplorer;
	if (bWar)
		iNumExplorersNeededTimes100 /= 2;

	SetExplorersNeeded(iNumExplorersNeededTimes100 / 100);

	//there is a slight hysteresis here to avoid unit AI flipping back and forth
	if(iNumExploringUnits*100 < iNumExplorersNeededTimes100-50)
	{
		m_eReconState = RECON_STATE_NEEDED;

		// Increase number of explorers
		vector< pair<int,int> > eligibleExplorers; //distance / id (don't store pointers for stable sorting!)
		for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
		{
			if(pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_EXPLORE) 
			{
				//note that new units are created only afterwards, so here we pick up the units without an important assignment from last turn
				if(pLoopUnit->canUseForAIOperation())
				{
					int iDistance = m_pPlayer->GetCityDistanceInPlots( pLoopUnit->plot() );
					eligibleExplorers.push_back( make_pair(iDistance,pLoopUnit->GetID()) );
				}
			}
		}

		//choose the one who is farthest out
		if (!eligibleExplorers.empty())
		{
			std::stable_sort( eligibleExplorers.begin(), eligibleExplorers.end() );
			CvUnit* pNewExplorer = m_pPlayer->getUnit( eligibleExplorers.back().second );
			pNewExplorer->AI_setUnitAIType(UNITAI_EXPLORE);
			if(GC.getLogging() && GC.getAILogging())
				LogEconomyMessage(CvString::format("Creating new land explorer (%s %d). Have %d, want %d, candidates %d",
					pNewExplorer->getName().GetCString(), pNewExplorer->GetID(), iNumExploringUnits, iNumExplorersNeededTimes100 / 100, eligibleExplorers.size()));
		}
	}
	else if(iNumExploringUnits*100 > iNumExplorersNeededTimes100+50)
	{
		m_eReconState = RECON_STATE_ENOUGH;

		// Reduce number of explorers by one
		vector< pair<int, int> > eligibleExplorers; //distance / id (don't store pointers for stable sorting!)
		for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
		{
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE && pLoopUnit->getUnitInfo().GetDefaultUnitAIType() != UNITAI_EXPLORE)
			{
				int iDistance = m_pPlayer->GetCityDistanceInPlots(pLoopUnit->plot());
				eligibleExplorers.push_back(make_pair(iDistance, pLoopUnit->GetID()));
			}
		}

		//choose the one who is closest
		if (!eligibleExplorers.empty())
		{
			std::stable_sort(eligibleExplorers.begin(), eligibleExplorers.end());
			CvUnit* pExplorer = m_pPlayer->getUnit(eligibleExplorers.front().second);

			pExplorer->AI_setUnitAIType(pExplorer->getUnitInfo().GetDefaultUnitAIType());
			if (GC.getLogging() && GC.getAILogging())
				LogEconomyMessage(CvString::format("Retiring land explorer (%s %d). Have %d, want %d, candidates %d",
					pExplorer->getName().GetCString(), pExplorer->GetID(), iNumExploringUnits, iNumExplorersNeededTimes100 / 100, eligibleExplorers.size()));
		}
	}
	else
	{
		m_eReconState = RECON_STATE_NEUTRAL;
	}

	// NAVAL RECON ACROSS THE ENTIRE MAP

	// No coastal cities?  Moot point...
	int iCityLoop = 0;
	bool bFoundCoastalCity = false;
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL && !bFoundCoastalCity; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
		if(pLoopCity->isCoastal())
			bFoundCoastalCity = true;

	if(!bFoundCoastalCity)
	{
		m_eNavalReconState = RECON_STATE_ENOUGH;
	}
	else
	{
		int iNumExploringUnits = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_EXPLORE_SEA, true);
		int iNumPlotsToExplore = (int)GetExplorationPlots(DOMAIN_SEA).size();

		// estimate one explorer per x open plots (these are only the border plots between known and unknown)
		int iPlotsPerExplorer = 100 - m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_NAVAL_RECON"));
		int iNumExplorersNeededTimes100 = 50 + (iNumPlotsToExplore * 100) / iPlotsPerExplorer;

		//always one?
		if (bWar || iNumExplorersNeededTimes100 < 100)
			iNumExplorersNeededTimes100 = 100;

		SetNavalExplorersNeeded(iNumExplorersNeededTimes100 / 100);

		//there is a slight hysteresis here to avoid unit AI flipping back and forth
		if(iNumExploringUnits*100 < iNumExplorersNeededTimes100-50)
		{
			m_eNavalReconState = RECON_STATE_NEEDED;

			// Send one additional boat out as a scout every round until we don't need recon anymore.
			vector< pair<int,int> > eligibleExplorersCoast; //distance / id (don't store pointers for stable sorting!)
			vector< pair<int, int> > eligibleExplorersDeepwater;
			PromotionTypes ePromotionOceanImpassable = (PromotionTypes)GD_INT_GET(PROMOTION_OCEAN_IMPASSABLE);
			for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
			{
				if (pLoopUnit->AI_getUnitAIType() != UNITAI_EXPLORE_SEA &&
					EconomicAIHelpers::IsPotentialNavalExplorer(pLoopUnit->getUnitInfo().GetDefaultUnitAIType()) &&
					pLoopUnit->canUseForAIOperation())
				{
					int iDistance = m_pPlayer->GetCityDistanceInPlots( pLoopUnit->plot() );

					if (pLoopUnit->isHasPromotion(ePromotionOceanImpassable))
						eligibleExplorersCoast.push_back(make_pair(iDistance, pLoopUnit->GetID()));
					else
						eligibleExplorersDeepwater.push_back(make_pair(iDistance, pLoopUnit->GetID()));
				}
			}

			//prefer oceangoing ships if we have any
			vector< pair<int,int> > eligibleExplorers = eligibleExplorersDeepwater.empty() ? eligibleExplorersCoast : eligibleExplorersDeepwater;

			//choose the one who is farthest out
			if (!eligibleExplorers.empty())
			{
				std::stable_sort( eligibleExplorers.begin(), eligibleExplorers.end() );
				CvUnit* pNewExplorer = m_pPlayer->getUnit( eligibleExplorers.back().second );
				pNewExplorer->AI_setUnitAIType(UNITAI_EXPLORE_SEA);
				if(GC.getLogging() && GC.getAILogging())
					LogEconomyMessage(CvString::format("Creating new naval explorer (%s %d). Have %d, want %d, candidates %d",
						pNewExplorer->getName().GetCString(), pNewExplorer->GetID(), iNumExploringUnits, iNumExplorersNeededTimes100 / 100, eligibleExplorers.size()));
			}
		}
		else if(iNumExploringUnits*100 > iNumExplorersNeededTimes100+50)
		{
			m_eNavalReconState = RECON_STATE_ENOUGH;

			// Return one boat to normal unit AI since have enough recon
			vector< pair<int, int> > eligibleExplorers; //distance / id (don't store pointers for stable sorting!)
			for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
			{
				if (pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA && pLoopUnit->getUnitInfo().GetDefaultUnitAIType() != UNITAI_EXPLORE_SEA)
				{
					int iDistance = m_pPlayer->GetCityDistanceInPlots(pLoopUnit->plot());
					eligibleExplorers.push_back(make_pair(iDistance, pLoopUnit->GetID()));
				}
			}

			//choose the one who is closest
			if (!eligibleExplorers.empty())
			{
				std::stable_sort(eligibleExplorers.begin(), eligibleExplorers.end());
				CvUnit* pExplorer = m_pPlayer->getUnit(eligibleExplorers.front().second);

				pExplorer->AI_setUnitAIType(pExplorer->getUnitInfo().GetDefaultUnitAIType());
				if (GC.getLogging() && GC.getAILogging())
					LogEconomyMessage(CvString::format("Retiring naval explorer (%s %d). Have %d, want %d, candidates %d",
						pExplorer->getName().GetCString(), pExplorer->GetID(), iNumExploringUnits, iNumExplorersNeededTimes100 / 100, eligibleExplorers.size()));
			}
		}
		else
		{
			m_eNavalReconState = RECON_STATE_NEUTRAL;
		}
	}

	if (isCannotRecon)
	{
		m_eReconState = RECON_STATE_ENOUGH;
	}
	if (isCannotReconSea)
	{
		m_eNavalReconState = RECON_STATE_ENOUGH;
	}
}

/// Determine how many sites we have for archaeologists
void CvEconomicAI::DoAntiquitySites()
{
	int iNumSites = 0;
	int iNumSitesOwn = 0;
	int iNumHiddenSitesOwn = 0;
	int iNumSitesNeutral = 0;

	ResourceTypes eArtifactResourceType = static_cast<ResourceTypes>(GD_INT_GET(ARTIFACT_RESOURCE));
	ResourceTypes eHiddenArtifactResourceType = static_cast<ResourceTypes>(GD_INT_GET(HIDDEN_ARTIFACT_RESOURCE));

	for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if (pPlot->isRevealed(m_pPlayer->getTeam()))
		{
			if (pPlot->getResourceType(m_pPlayer->getTeam()) == eArtifactResourceType ||
				pPlot->getResourceType(m_pPlayer->getTeam()) == eHiddenArtifactResourceType)
			{
				if (pPlot->getOwner() == m_pPlayer->GetID())
				{
					iNumSitesOwn++;
					if (pPlot->getResourceType(m_pPlayer->getTeam()) == eHiddenArtifactResourceType)
					{
						iNumHiddenSitesOwn++;
					}
				}
				else if (pPlot->getOwner() == NO_PLAYER)
				{
					iNumSitesNeutral++;
				}
				else
				{
					iNumSites++;
				}
			}
		}
	}

	m_iVisibleAntiquitySitesOwn =  iNumSitesOwn;
	m_iVisibleHiddenAntiquitySitesOwn = iNumHiddenSitesOwn;
	m_iVisibleAntiquitySitesNeutral = iNumSitesNeutral;
	m_iVisibleAntiquitySites = iNumSites;
}

void CvEconomicAI::DisbandMiscUnits()
{
	if (m_pPlayer->isMinorCiv())
	{
		int iUnitLoop = 0;
		for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
		{
			if (!pLoopUnit)
				continue;

			//disband missionaries, however we got them
			if (pLoopUnit->GetReligionData()->GetSpreadsLeft(pLoopUnit) > 0)
			{
				pLoopUnit->scrap();
				LogScrapUnit(pLoopUnit, 0, 0, -1, 0);
			}
		}
	}
}

/// Disband units that require aluminum if we want to build spaceship parts and don't have enough aluminum to do so
void CvEconomicAI::DisbandUnitsToFreeSpaceshipResources()
{
	if (!m_pPlayer->GetDiplomacyAI()->IsGoingForSpaceshipVictory())
		return;

	int iNumTotalAluminumNeededForSpaceship = m_pPlayer->GetNumAluminumStillNeededForSpaceship();
	int iNumTotalAluminumNeededForCoreCities = m_pPlayer->GetNumAluminumStillNeededForCoreCities();

	static ResourceTypes eAluminum = (ResourceTypes)GC.getInfoTypeForString("RESOURCE_ALUMINUM", true);
	int iNumAluminumWeNeed = max(0, iNumTotalAluminumNeededForSpaceship + iNumTotalAluminumNeededForCoreCities - m_pPlayer->getNumResourceAvailable(eAluminum, true));

	const vector<int>& vCoreCities = m_pPlayer->GetCoreCitiesForSpaceshipProduction();
	if (GC.getLogging() && GC.getAILogging())
	{
		CvString strLogString;
		strLogString.Format("");
		m_pPlayer->LogSpaceshipPlanMessage(strLogString);
		if (vCoreCities.size() > 0)
		{
			strLogString.Format("Core Cities: ");
			for (size_t i = 0; i < vCoreCities.size(); i++)
			{
				strLogString += m_pPlayer->getCity(vCoreCities[i])->getName();
				strLogString += ", ";
			}
		}
		m_pPlayer->LogSpaceshipPlanMessage(strLogString);
		strLogString.Format("Aluminum still needed for spaceship parts: %d. Aluminum still needed for core cities: %d. Currently available: %d", iNumTotalAluminumNeededForSpaceship, iNumTotalAluminumNeededForCoreCities, m_pPlayer->getNumResourceAvailable(eAluminum, true));
		m_pPlayer->LogSpaceshipPlanMessage(strLogString);
	}

	// todo: check for units currently in production and remove them from the queue before disbanding anything
	
	// create a weighted list of all units that require aluminum and that we could disband
	CvWeightedVector<int> vUnitsDisband;
	int iValue = 0;
	if (iNumAluminumWeNeed > 0)
	{
		int iUnitLoop = 0;
		for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
		{
			if (!pLoopUnit)
				continue;

			if (!pLoopUnit->canScrap())
				continue;

			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(pLoopUnit->getUnitType());
			if (!pkUnitInfo)
				continue;

			if (pkUnitInfo->GetSpaceshipProject() != NO_PROJECT)
				continue;

			int iNumResourceInUnit = pkUnitInfo->GetResourceQuantityRequirement(eAluminum);
			if (iNumResourceInUnit > 0)
			{
				// value based on unit strength
				iValue = pkUnitInfo->GetPower() / iNumResourceInUnit;
				// higher value for units currently in an operation
				if (pLoopUnit->getArmyID() != -1)
				{
					iValue *= 3;
					iValue /= 2;
				}
				// higher value for units with many promotions
				iValue *= (100 + 20 * pLoopUnit->getLevel());
				iValue /= 100;
				vUnitsDisband.push_back(pLoopUnit->GetID(), iValue);
			}
		}

		if (vUnitsDisband.size() > 0)
		{
			// go through the list of units and disband the ones with the lowest value until we have enough aluminum
			vUnitsDisband.StableSortItems();
			for (int i = vUnitsDisband.size() - 1; i >= 0; i--)
			{
				CvUnit* pDisbandUnit = m_pPlayer->getUnit(vUnitsDisband.GetElement(i));
				int iNumUnitResources = GC.getUnitInfo(pDisbandUnit->getUnitType())->GetResourceQuantityRequirement(eAluminum);
				pDisbandUnit->scrap(false);
				if (GC.getLogging() && GC.getAILogging())
				{
					CvString strLogString;
					strLogString.Format("Disbanding %s to get aluminum for spaceship. Aluminum available after disbanding: %d", pDisbandUnit->getName().GetCString(), m_pPlayer->getNumResourceAvailable(eAluminum, true));
					m_pPlayer->LogSpaceshipPlanMessage(strLogString);
				}

				iNumAluminumWeNeed -= iNumUnitResources;
				if (iNumAluminumWeNeed <= 0)
				{
					return;
				}
			}
		}

		// still need aluminum? try to sell buildings with resource requirements
		if (iNumAluminumWeNeed > 0)
		{
			// sort cities by economic value, cities in which we want to build spaceship parts are extra valuable
			CvWeightedVector<CvCity*> vCityEconomicWeights;
			const vector<int>& vCitiesForSpaceshipParts = m_pPlayer->GetCoreCitiesForSpaceshipProduction();
			int iLoopCity = 0;
			for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
			{
				// can't sell anything in this city?
				if (pLoopCity->IsPuppet() || pLoopCity->IsResistance() || pLoopCity->GetCityBuildings()->IsSoldBuildingThisTurn() || m_pPlayer->GetPlayerTraits()->IsNoAnnexing())
					continue;

				int iWeight = pLoopCity->getEconomicValue(m_pPlayer->GetID());
				if (pLoopCity->isProductionSpaceshipPart() || (vCitiesForSpaceshipParts.size() > 0 && std::find(vCitiesForSpaceshipParts.begin(), vCitiesForSpaceshipParts.end(), pLoopCity->GetID()) != vCitiesForSpaceshipParts.end()))
				{
					// make sure the core cities for spaceship production have the highest score
					iWeight *= 10;
				}
				vCityEconomicWeights.push_back(pLoopCity, iWeight);
			}
			if (vCityEconomicWeights.size() > 0)
			{
				vCityEconomicWeights.StableSortItems();
				// start selling buildings in the worst cities
				for (int i = vCityEconomicWeights.size() - 1; i >= 0; i--)
				{

					CvCity* pLoopCity = vCityEconomicWeights.GetElement(i);
					// if this is a core city, sell buildings only if we need them for spaceship parts
					if (pLoopCity->isProductionSpaceshipPart() || (vCitiesForSpaceshipParts.size() > 0 && std::find(vCitiesForSpaceshipParts.begin(), vCitiesForSpaceshipParts.end(), pLoopCity->GetID()) != vCitiesForSpaceshipParts.end()))
					{
						if (iNumAluminumWeNeed - iNumTotalAluminumNeededForCoreCities <= 0)
						{
							continue;
						}
					}
					BuildingTypes eBestBuilding = NO_BUILDING;
					int iNumAluminumInBuilding = 0;
					int iWorstRefund = INT_MAX;
					// sell the building with the lowest refund, which is probably the least important one
					for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
					{
						const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
						CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

						if (pkBuildingInfo && pkBuildingInfo->GetResourceQuantityRequirement(eAluminum) > 0)
						{
							// Building in queue? Cancel it.
							if (pLoopCity->isBuildingInQueue(eBuilding))
							{
								eBestBuilding = eBuilding;
								iNumAluminumInBuilding = pkBuildingInfo->GetResourceQuantityRequirement(eAluminum);
								// canceling a building in queue is always better than selling an existing one, so don't consider other options anymore
								break;
							}
							// Has this Building
							if (pLoopCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0 && pLoopCity->GetCityBuildings()->IsBuildingSellable(*pkBuildingInfo))
							{
								int iRefund = pLoopCity->GetCityBuildings()->GetSellBuildingRefund(eBuilding);
								if (iRefund < iWorstRefund)
								{
									iWorstRefund = iRefund;
									eBestBuilding = eBuilding;
									iNumAluminumInBuilding = pkBuildingInfo->GetResourceQuantityRequirement(eAluminum);
								}
							}
						}
					}
					if (eBestBuilding != NO_BUILDING)
					{
						pLoopCity->isBuildingInQueue(eBestBuilding) ? pLoopCity->clearOrderQueue() : pLoopCity->GetCityBuildings()->DoSellBuilding(eBestBuilding);
						if (GC.getLogging() && GC.getAILogging())
						{
							CvString strLogString;
							strLogString.Format("Selling or canceling production of building %s in %s to get aluminum for spaceship. Aluminum available now: %d", GC.getBuildingInfo(eBestBuilding)->GetText(), pLoopCity->getName().GetCString(), m_pPlayer->getNumResourceAvailable(eAluminum, true));
							m_pPlayer->LogSpaceshipPlanMessage(strLogString);
						}
						iNumAluminumWeNeed -= iNumAluminumInBuilding;
						if (iNumAluminumWeNeed <= 0)
						{
							return;
						}
					}
				}
			}
		}
	}
}

void CvEconomicAI::DisbandUselessSettlers()
{
	//If we want settlers, don't disband.
	static EconomicAIStrategyTypes eNoMoreSettlers = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ENOUGH_EXPANSION");
	int iNumSettlers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);
	int iNumCities = m_pPlayer->GetNumCitiesFounded();

	bool bNoMoreSettlers = m_pPlayer->GetEconomicAI()->IsUsingStrategy(eNoMoreSettlers);
	if(!bNoMoreSettlers)
	{
		return;
	}

	//two settlers are ok
	if (iNumSettlers <= 2)
	{
		return;
	}

	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if(!pCapital)
	{
		return;
	}

	//Don't disband during the early game.
	if(iNumCities <= 4 && (GC.getGame().getGameTurn() <= 150))
	{
		return;
	}
	if (m_pPlayer->HaveGoodSettlePlot(-1) )
	{
		return;
	}

	CvUnit* pUnit = FindSettlerToScrap(false);
	if(!pUnit)
	{
		return;
	}

	pUnit->scrap();
	LogScrapUnit(pUnit, iNumSettlers, iNumCities, -1, 0);
}

CvUnit* CvEconomicAI::FindSettlerToScrap(bool bMayBeInOperation)
{
	// Look at map for loose workers
	int iUnitLoop = 0;
	for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		if (!pLoopUnit->canScrap())
			continue;

		if (pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->isFound() && !pLoopUnit->IsFoundAbroad() && !pLoopUnit->IsCombatUnit() && !pLoopUnit->IsGreatPerson())
		{
			if (bMayBeInOperation || pLoopUnit->getArmyID()!=-1)
				return pLoopUnit;
		}
	}

	return NULL;
}

void CvEconomicAI::DisbandUselessDiplomats()
{
	bool bIsMinor = m_pPlayer->isMinorCiv();
	bool bAnyCityStatesEver = GC.getGame().GetNumMinorCivsEver(false) > 0;
	bool bAnyCityStatesAlive = GC.getGame().GetNumMinorCivsAlive() > 0;
	if (!bIsMinor && bAnyCityStatesAlive)
		return;

	// Look at map for loose diplomats
	int iUnitLoop = 0;
	for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		if (!pLoopUnit->canScrap())
			continue;

		if (pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_MESSENGER && (!bAnyCityStatesAlive || bIsMinor))
			pLoopUnit->scrap();
		else if (pLoopUnit->getUnitInfo().GetDefaultUnitAIType() == UNITAI_DIPLOMAT && (!bAnyCityStatesEver || bIsMinor))
			pLoopUnit->scrap();
	}
}

void CvEconomicAI::DisbandExtraWorkboats()
{
	int iNumWorkers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_WORKER_SEA, true);
	int iNumCities = m_pPlayer->getNumCities();
	if(iNumWorkers <= 0)
		return;

	//If we want workers in any city, don't disband.
	static AICityStrategyTypes eWantWorkers = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT");
	int iLoopCity = 0;
	int iNumCitiesWithStrat = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = m_pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity != NULL)
		{
			if(eWantWorkers != NO_AICITYSTRATEGY)
			{
				if(pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eWantWorkers))
				{
					iNumCitiesWithStrat++;
				}
			}
		}
	}
	//# of cities needing workers greater than number of workers? don't disband.
	if(iNumCitiesWithStrat >= iNumWorkers)
	{
		return;
	}
	//Don't disband during the early game.
	if(m_pPlayer->GetNumCitiesFounded() < 4 && (GC.getGame().getGameTurn() <= 100))
	{
		return;
	}

	int iNumValidPlots = 0;
	int iNumUnimprovedPlots = 0;
	const PlotIndexContainer& aiPlots = m_pPlayer->GetPlots();
	// go through all the plots the player has under their control
	for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if (!pPlot)
		{
			continue;
		}
		if(!pPlot->isWater() || !pPlot->isValidMovePlot(GetPlayer()->GetID()))
		{
			continue;
		}

		iNumValidPlots++;

		if(pPlot->getImprovementType() == NO_IMPROVEMENT && pPlot->getResourceType(GetPlayer()->getTeam()) != NO_RESOURCE)
		{
			iNumUnimprovedPlots++;
		}
	}

	if(iNumUnimprovedPlots > iNumWorkers)
	{
		return;
	}
	else if(iNumWorkers > iNumUnimprovedPlots)
	{
		m_iLastTurnWorkerDisbanded = GC.getGame().getGameTurn();

		CvUnit* pUnit = FindSeaWorkerToScrap();
		if(!pUnit)
		{
			return;
		}

		pUnit->scrap();
		LogScrapUnit(pUnit, iNumWorkers, iNumCities, iNumUnimprovedPlots, iNumValidPlots);
	}
}

CvUnit* CvEconomicAI::FindSeaWorkerToScrap()
{
	CvUnit* pLoopUnit = NULL;
	int iUnitLoop = 0;

	// Look at map for loose workers
	for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		if(!pLoopUnit)
		{
			continue;
		}

		if (!pLoopUnit->canScrap())
			continue;

		UnitTypes eWorker = m_pPlayer->GetSpecificUnitType("UNITCLASS_WORKBOAT");
		if(pLoopUnit->getDomainType() == DOMAIN_SEA && pLoopUnit->getUnitType() == eWorker && !pLoopUnit->IsCombatUnit() && pLoopUnit->getSpecialUnitType() == NO_SPECIALUNIT)
		{
			return pLoopUnit;
		}
	}

	return NULL;
}

void CvEconomicAI::DisbandExtraArchaeologists(){
	int iNumSites = GC.getGame().GetNumArchaeologySites();
	double dMaxRatio = .5; //Ratio of archaeologists to sites
	int iNumArchaeologists = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true);
	if(iNumArchaeologists <= 0)
		return; 

	static PolicyTypes eExpFinisher = (PolicyTypes) GC.getInfoTypeForString("POLICY_EXPLORATION_FINISHER", true /*bHideAssert*/);
	if (eExpFinisher != NO_POLICY)	
	{
		if (m_pPlayer->GetPlayerPolicies()->HasPolicy(eExpFinisher))
		{
			iNumSites += GC.getGame().GetNumHiddenArchaeologySites();
		}
	}
	
	CvUnit* pUnit = NULL;
	UnitTypes eArch = m_pPlayer->GetSpecificUnitType("UNITCLASS_ARCHAEOLOGIST", true);

	if (eArch == NO_UNIT)
		return;

	if ((double)iNumSites * dMaxRatio + 1 < iNumArchaeologists ){
		pUnit = FindArchaeologistToScrap();
	
		if(!pUnit)
		{
			return;
		}
	
		pUnit->scrap();
		LogScrapUnit(pUnit, iNumArchaeologists, iNumSites, 0, 0);
	}
}

void CvEconomicAI::DisbandExtraWorkers()
{
	int iNumWorkers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_WORKER, true);
	if (iNumWorkers <= 1)
		return;

	//Don't disband during the early game.
	if (m_pPlayer->GetNumCitiesFounded() < 4 && GC.getGame().getGameTurn() <= 100)
		return;

	int iNumCities = m_pPlayer->getNumCities();

	int iWorstCaseWorkerPerCityRatio = 25; // one worker for four cities
	int iCurrentWorkerPerCityRatio = (100 * iNumWorkers) / max(1, iNumCities);
	if (iCurrentWorkerPerCityRatio <= iWorstCaseWorkerPerCityRatio)
		return;

	int iNumValidPlots = 0;
	int iNumImprovedPlots = 0;
	const PlotIndexContainer& aiPlots = m_pPlayer->GetPlots();
	// go through all the plots the player has under their control
	for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(*it);
		if (!pPlot)
			continue;

		if (pPlot->isWater() || !pPlot->isValidMovePlot(m_pPlayer->GetID()) || pPlot->isCity())
			continue;

		iNumValidPlots++;

		if (pPlot->getImprovementType() != NO_IMPROVEMENT && !pPlot->IsImprovementPillaged())
			iNumImprovedPlots++;
	}

	if (iNumValidPlots == 0)
		return;

	int iImprovedPlotsRatio = (100 * iNumImprovedPlots) / iNumValidPlots;
	if (iImprovedPlotsRatio <= 66)
		return;

	// How many idle workers do we have?
	int iIdleWorkers = 0;
	static const UnitTypes eWorker = m_pPlayer->GetSpecificUnitType("UNITCLASS_WORKER");

	int iLoopUnit = 0;
	for (CvUnit* pLoopUnit = m_pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iLoopUnit))
	{
		if (pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->getUnitType() == eWorker && !pLoopUnit->IsCombatUnit() && pLoopUnit->getSpecialUnitType() == NO_SPECIALUNIT)
		{
			CvPlot* pUnitPlot = pLoopUnit->plot();
			if (pUnitPlot && pUnitPlot->isCity())
				iIdleWorkers++;
		}
	}

	// If we have more than one idle worker per city, start disbanding
	if (iIdleWorkers <= iNumCities)
		return;

	CvUnit* pUnit = FindWorkerToScrap();
	if (!pUnit)
		return;

	m_iLastTurnWorkerDisbanded = GC.getGame().getGameTurn();

	pUnit->scrap();
	LogScrapUnit(pUnit, iNumWorkers, iNumCities, iNumImprovedPlots, iNumValidPlots);
}

// Check for very long obsolete units that didn't get an upgrade (usual suspects are triremes and warriors)
void CvEconomicAI::DisbandLongObsoleteUnits()
{
	int iLoop = 0;
	// Treat information era as atomic for this checking.
	int playerCurrentEra = min(6,(int)m_pPlayer->GetCurrentEra());

	// Loop through our units
	for(CvUnit* pUnit = m_pPlayer->firstUnit(&iLoop); pUnit != NULL; pUnit = m_pPlayer->nextUnit(&iLoop))
	{
		if (pUnit)
		{
			int ArmyId = pUnit->getArmyID();
			if (ArmyId != -1)
			{
				CvArmyAI* pThisArmy = m_pPlayer->getArmyAI(ArmyId);
				if (pThisArmy)
					if ((pThisArmy->GetArmyAIState() == ARMYAISTATE_MOVING_TO_DESTINATION) || (pThisArmy->GetArmyAIState() == ARMYAISTATE_AT_DESTINATION))
						continue;
			}

			// The unit must have an upgrade option, if not, then we don't care about this (includes workers, settlers, explorers)
			UnitTypes eUpgradeUnitType = pUnit->GetUpgradeUnitType();

			//Fixed for settlers for advanced start.
			if(eUpgradeUnitType != NO_UNIT && !pUnit->isFound())
			{
				// Check out unit era based on the prerequirement tech, defaults at ancient era.
				int unitEra = 0;
				UnitTypes currentUnitType = pUnit->getUnitType();
				TechTypes ePrereqTech = (TechTypes)GC.getUnitInfo(currentUnitType)->GetPrereqAndTech();

				if (ePrereqTech != NO_TECH)
				{
					CvTechEntry* pkTechInfo = GC.getTechInfo(ePrereqTech);
					if (pkTechInfo)
					{
						unitEra = pkTechInfo->GetEra();
					}
				}

				// Too much era difference for that unit, lets scrap it.
				if ((playerCurrentEra - unitEra) > 2)
				{
					if(GC.getLogging() && GC.getAILogging())
					{
						CvString strLogString;
						strLogString.Format("Disbanding long obsolete unit. %s, X: %d, Y: %d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY());
						m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
					}

					pUnit->scrap();
					// Only one unit scrap per turn.
					return;
				}
			}
		}
	}
}

void TestExplorationPlot(CvPlot* pPlot, CvPlayer* pPlayer, bool bAllowShallowWater, bool bAllowDeepWater, std::vector<SPlotWithScore>& landTargets, std::vector<SPlotWithScore>& seaTargets)
{
	if (pPlot == NULL)
		return;

	//allow AI to see the next tile, a human can guess as well
	if (!pPlot->isAdjacentRevealed(pPlayer->getTeam()))
		return;

	if (pPlot->isWater())
	{
		if (pPlot->isShallowWater() || bAllowDeepWater)
		{
			int iScore = EconomicAIHelpers::ScoreExplorePlot(pPlot, pPlayer, DOMAIN_SEA, false);
			if (iScore <= 0)
				return;

			// add an entry for this plot
			seaTargets.push_back(SPlotWithScore(pPlot, iScore));

			// close coast is also interesting for embarked scouting (performance: use the cheap distance check)
			if (pPlot->isShallowWater() && bAllowShallowWater && pPlayer->GetCityDistanceInPlots(pPlot)<12)
				landTargets.push_back(SPlotWithScore(pPlot, iScore));
		}
	}
	else
	{
		int iScore = EconomicAIHelpers::ScoreExplorePlot(pPlot, pPlayer, DOMAIN_LAND, false);
		if (iScore <= 0)
			return;

		if (!MOD_BALANCE_VP && !bAllowShallowWater)
		{
			CvCity* pCapital = pPlayer->getCapitalCity();
			if (pCapital && !pCapital->HasAccessToArea(pPlot->getArea()))
				return;
		}

		// add an entry for this plot
		landTargets.push_back(SPlotWithScore(pPlot, iScore));
	}
}

/// Go through the plots for the exploration automation to evaluate
void CvEconomicAI::UpdateExplorePlotsFromScratch()
{
	m_vPlotsToExploreLand.clear();
	m_vPlotsToExploreSea.clear();

	bool bNeedToLookAtDeepWaterAlso = m_pPlayer->CanCrossOcean();
	bool bCanEmbark = m_pPlayer->CanEmbark();

	for(int i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(i);
		TestExplorationPlot(pPlot, m_pPlayer, bCanEmbark, bNeedToLookAtDeepWaterAlso, m_vPlotsToExploreLand, m_vPlotsToExploreSea);
	}

#if defined(MOD_BALANCE_CORE_MILITARY_LOGGING)
	bool bLogging = GC.getLogging() && GC.getAILogging() && m_pPlayer->isMajorCiv() && MOD_BALANCE_CORE_MILITARY_LOGGING;
	CvString fname = CvString::format("ExplorePlots_%s_%03d.txt", m_pPlayer->getCivilizationAdjective(), GC.getGame().getGameTurn());
	FILogFile* pLog = bLogging ? LOGFILEMGR.GetLog(fname.c_str(), FILogFile::kDontTimeStamp) : NULL;
	if (bLogging && pLog)
	{
		for (size_t i = 0; i < m_vPlotsToExploreLand.size(); i++)
		{
			CvPlot* pPlot = m_vPlotsToExploreLand[i].pPlot;
			CvString dump = CvString::format("%d,%d,%d,%d,%d,%d\n",
				pPlot->getX(), pPlot->getY(), pPlot->isRevealed(m_pPlayer->getTeam()), pPlot->getTerrainType(), pPlot->getOwner(), m_vPlotsToExploreLand[i].score);
			pLog->Msg(dump.c_str());
		}
		for (size_t i = 0; i < m_vPlotsToExploreSea.size(); i++)
		{
			CvPlot* pPlot = m_vPlotsToExploreSea[i].pPlot;
			CvString dump = CvString::format("%d,%d,%d,%d,%d,%d\n",
				pPlot->getX(), pPlot->getY(), pPlot->isRevealed(m_pPlayer->getTeam()), pPlot->getTerrainType(), pPlot->getOwner(), m_vPlotsToExploreSea[i].score);
			pLog->Msg(dump.c_str());
		}
	}
	if (pLog)
		pLog->Close();
#endif

	//keep all of them - GetBestExplorePlot will only look at the n best candidates anyway
	std::stable_sort(m_vPlotsToExploreLand.begin(),m_vPlotsToExploreLand.end());
	std::stable_sort(m_vPlotsToExploreSea.begin(),m_vPlotsToExploreSea.end());

	LogEconomyMessage(CvString::format("Updating exploration plots, found %d land and %d sea targets",m_vPlotsToExploreLand.size(),m_vPlotsToExploreSea.size()));
}

void CvEconomicAI::UpdateExplorePlotsLocally(CvPlot* pPlot)
{
	if (!pPlot)
		return;

	int iRange = 3; //must be <=5

	struct PrPlotDistanceSmallerThan
	{
		int dist;
		CvPlot* ref;
		PrPlotDistanceSmallerThan(CvPlot* p, int d) : dist(d), ref(p) {}
		bool operator()(const SPlotWithScore& test) const {
			if (ref && test.pPlot)
				return plotDistance(*test.pPlot, *ref) < dist;
			return true;
		}
	};

	//first remove the plots in the neighborhood of our ref plot
	m_vPlotsToExploreSea.erase(std::remove_if(m_vPlotsToExploreSea.begin(), m_vPlotsToExploreSea.end(), PrPlotDistanceSmallerThan(pPlot, iRange)), m_vPlotsToExploreSea.end());
	m_vPlotsToExploreLand.erase(std::remove_if(m_vPlotsToExploreLand.begin(), m_vPlotsToExploreLand.end(), PrPlotDistanceSmallerThan(pPlot, iRange)), m_vPlotsToExploreLand.end());

	//then add them again with their new valuation
	bool bNeedToLookAtDeepWaterAlso = m_pPlayer->CanCrossOcean();
	bool bCanEmbark = m_pPlayer->CanEmbark(); 
	for (int i = 0; i < RING_PLOTS[iRange]; i++)
	{
		CvPlot* pTestPlot = iterateRingPlots(pPlot, i);
		TestExplorationPlot(pTestPlot, m_pPlayer, bCanEmbark, bNeedToLookAtDeepWaterAlso, m_vPlotsToExploreLand, m_vPlotsToExploreSea);
	}

	//sort again
	std::stable_sort(m_vPlotsToExploreLand.begin(), m_vPlotsToExploreLand.end());
	std::stable_sort(m_vPlotsToExploreSea.begin(), m_vPlotsToExploreSea.end());
}

CvUnit* CvEconomicAI::FindWorkerToScrap()
{
	CvUnit* pLoopUnit = NULL;
	int iUnitLoop = 0;
	int iBestUnitValue = 0;
	CvUnit* pBestUnit = NULL;
	// Look at map for loose workers
	for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		if(!pLoopUnit)
		{
			continue;
		}

		UnitTypes eWorker = m_pPlayer->GetSpecificUnitType("UNITCLASS_WORKER");

		if(pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->getUnitType() == eWorker && !pLoopUnit->IsCombatUnit() && pLoopUnit->getSpecialUnitType() == NO_SPECIALUNIT)
		{
			int WorkRateMod = 100 - pLoopUnit->GetWorkRateMod();
			if (WorkRateMod < 100 && WorkRateMod > 0)
			{
				if (WorkRateMod > iBestUnitValue)
				{
					iBestUnitValue = WorkRateMod;
					pBestUnit = pLoopUnit;
				}
			}
		}
	}

	if (pBestUnit == NULL)
	{
		for (pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
		{
			if (!pLoopUnit)
			{
				continue;
			}

			if (!pLoopUnit->canScrap())
				continue;

			UnitTypes eWorker = m_pPlayer->GetSpecificUnitType("UNITCLASS_WORKER");

			if (pLoopUnit->getDomainType() == DOMAIN_LAND && pLoopUnit->getUnitType() == eWorker && !pLoopUnit->IsCombatUnit() && pLoopUnit->getSpecialUnitType() == NO_SPECIALUNIT)
			{
				return pLoopUnit;
			}
		}
	}

	return pBestUnit;
}

CvUnit* CvEconomicAI::FindArchaeologistToScrap()
{
	CvUnit* pLoopUnit = NULL;
	int iUnitLoop = 0;

	// Look at map for loose archaeologists
	for(pLoopUnit = m_pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = m_pPlayer->nextUnit(&iUnitLoop))
	{
		if(!pLoopUnit)
		{
			continue;
		}

		if (!pLoopUnit->canScrap())
			continue;

		UnitTypes eArch = m_pPlayer->GetSpecificUnitType("UNITCLASS_ARCHAEOLOGIST");

		if(pLoopUnit->getUnitType() == eArch)
		{
			return pLoopUnit;
		}
	}

	return NULL;
}

/// Log that a strategy is being turned on or off
void CvEconomicAI::LogStrategy(EconomicAIStrategyTypes eStrategy, bool bValue)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		// Find the name of this civ and city
		CvString playerName = GetPlayer()->getCivilizationShortDescription();

		// Open the log file
		CvString strLogName = "FlavorAILog.csv";
		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "FlavorAILog_" + playerName + ".csv";
		}

		FILogFile* pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", ";

		CvString strOutBuf = strBaseString;

		// Strategy Info
		CvEconomicAIStrategyXMLEntry* pEconomicAIStrategyEntry = GC.getEconomicAIStrategyInfo(eStrategy);
		if(pEconomicAIStrategyEntry != NULL)
		{
			CvString strTemp;
			strTemp.Format("%s, %d", pEconomicAIStrategyEntry->GetType(), bValue);
			strOutBuf += strTemp;
		}

		pLog->Msg(strOutBuf);

		// Also - write to Other Logs, so that we know what the player is doing in there as well

		if(GC.getPlayerAndCityAILogSplit())
		{
			strLogName = "TechAILog_" + playerName + ".csv";
			pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);
			pLog->Msg(strOutBuf);

			CvString cityName;

			CvCity* pLoopCity = NULL;
			int iLoop = 0;

			for(pLoopCity = GetPlayer()->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GetPlayer()->nextCity(&iLoop))
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

/// Log that a unit is disbanded
void CvEconomicAI::LogScrapUnit(CvUnit* pUnit, int iNumWorkers, int iNumCities, int iNumImprovedPlots, int iNumValidPlots)
{
	if(!GC.getLogging() || !GC.getAILogging())
	{
		return;
	}

	CvString strLogString;
	strLogString.Format("Disbanding %s, X: %d, Y: %d, iNumWorkers: %d, iNumCities: %d, improved/valid plots: %d/%d", pUnit->getName().GetCString(), pUnit->getX(), pUnit->getY(), iNumWorkers, iNumCities, iNumImprovedPlots, iNumValidPlots);
	m_pPlayer->GetHomelandAI()->LogHomelandMessage(strLogString);
}

///
template<typename PurchaseRequest, typename Visitor>
void CvPurchaseRequest::Serialize(PurchaseRequest& purchaseRequest, Visitor& visitor)
{
	visitor(purchaseRequest.m_eType);
	visitor(purchaseRequest.m_iAmount);
	visitor(purchaseRequest.m_iPriority);
}

FDataStream& operator<<(FDataStream& saveTo, const CvPurchaseRequest& readFrom)
{
	CvStreamSaveVisitor serialVisitor(saveTo);
	CvPurchaseRequest::Serialize(readFrom, serialVisitor);
	return saveTo;
}

FDataStream& operator>>(FDataStream& loadFrom, CvPurchaseRequest& writeTo)
{
	CvStreamLoadVisitor serialVisitor(loadFrom);
	CvPurchaseRequest::Serialize(writeTo, serialVisitor);
	return loadFrom;
}

// NON-MEMBER FUNCTIONS
//
// These are functions that do not need access to the internals of the CvEconomicAI class.
// Keep them as non-member functions to:
// a) simplify the class (improving encapsulation, reducing coupling)
// b) allow their general use by other classes

// Figure out what the WeightThreshold Mod should be by looking at the Flavors for this player & the Strategy
int EconomicAIHelpers::GetWeightThresholdModifier(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iWeightThresholdModifier = 0;

	// Look at all Flavors for the Player & this Strategy
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		FlavorTypes eFlavor = (FlavorTypes) iFlavorLoop;
		int iPersonalityFlavor = pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
		int iStrategyFlavorMod = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy)->GetPersonalityFlavorThresholdMod(eFlavor);

		iWeightThresholdModifier += (iPersonalityFlavor * iStrategyFlavorMod);
	}

	return iWeightThresholdModifier;
}

/// "Need Recon" Player Strategy: chosen by the DoRecon() function
bool EconomicAIHelpers::IsTestStrategy_NeedRecon(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	if (CannotMinorCiv(pPlayer,eStrategy))
	{
		return false;
	}
	// Never desperate for explorers if we are at war
	static MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_AT_WAR");
	if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
	{
		return false;
	}

	return (pPlayer->GetEconomicAI()->GetReconState() == RECON_STATE_NEEDED);
}

/// "Enough Recon" Player Strategy: chosen by the DoRecon() function
bool EconomicAIHelpers::IsTestStrategy_EnoughRecon(CvPlayer* pPlayer)
{
	return (pPlayer->GetEconomicAI()->GetReconState() == RECON_STATE_ENOUGH);
}

/// "Really Need Recon Sea" Player Strategy: If we could theoretically start exploring other continents but we don't have any appropriate ship...
bool EconomicAIHelpers::IsTestStrategy_ReallyNeedReconSea(CvPlayer* pPlayer)
{
	if(pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_NEEDED)
	{
		if(pPlayer->CanCrossOcean())  // get a caravel out there NOW!
		{
			CvUnit* pLoopUnit = NULL;
			CvCity* pLoopCity = NULL;
			int iLoop = 0;

			// Current Units
			for(pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
			{
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA && !pLoopUnit->isTerrainImpassable(TERRAIN_OCEAN))
				{
					return false;
				}
			}

			// Figure out which Promotion is the one which makes a unit not cross oceans
			PromotionTypes eOceanImpassablePromotion = NO_PROMOTION;
			for(int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				const PromotionTypes eLoopPromotion = static_cast<PromotionTypes>(iI);
				CvPromotionEntry* pkPromotionInfo = GC.getPromotionInfo(eLoopPromotion);
				if(pkPromotionInfo)
				{
					if(pkPromotionInfo->GetTerrainImpassable(TERRAIN_OCEAN) && pkPromotionInfo->GetTerrainPassableTech(TERRAIN_OCEAN) == -1)
					{
						eOceanImpassablePromotion = eLoopPromotion;
						break;
					}
				}
			}

			// Units being trained now
			for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
			{
				if(pLoopCity->isProductionUnit())
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopCity->getProductionUnit());
					if(pkUnitEntry)
					{
						if(pkUnitEntry->GetDomainType() == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
						{
							if(!pkUnitEntry->GetFreePromotions(eOceanImpassablePromotion))
							{
								return false;
							}
						}
					}
				}
			}
			return true;
		}
		else if(pPlayer->CanEmbark())  // get a trireme out there NOW!
		{
			int iLoop = 0;

			// Current Units
			for(CvUnit* pLoopUnit = pPlayer->firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoop))
			{
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_EXPLORE_SEA)
				{
					return false;
				}
			}

			// Units being trained now
			for(CvCity* pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
			{
				if(pLoopCity->isProductionUnit())
				{
					CvUnitEntry* pkUnitEntry = GC.getUnitInfo(pLoopCity->getProductionUnit());
					if(pkUnitEntry)
					{
						if(pkUnitEntry->GetDomainType() == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_EXPLORE_SEA)
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
bool EconomicAIHelpers::IsTestStrategy_NeedReconSea(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	if (CannotMinorCiv(pPlayer, eStrategy))
	{
		return false;
	}
	// Never desperate for explorers if we are at war
	static MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
	{
		return false;
	}

	return (pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_NEEDED);
}

/// "Enough Recon Sea" Player Strategy: chosen by the DoRecon() function
bool EconomicAIHelpers::IsTestStrategy_EnoughReconSea(CvPlayer* pPlayer)
{
#if defined(MOD_BALANCE_CORE)
	// Never desperate for explorers if we are at war
	static MilitaryAIStrategyTypes eStrategyAtWar = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategyAtWar))
	{
		return true;
	}
#endif
	return (pPlayer->GetEconomicAI()->GetNavalReconState() == RECON_STATE_ENOUGH);
}

/// "Developing Religion" Player Strategy: planning to create and spread religion
bool EconomicAIHelpers::IsTestStrategy_DevelopingReligion(CvPlayer* pPlayer)
{
	if(pPlayer->isHuman())
	{
		return false;
	}

	// Always true if we've already created a religion
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();
	if(pPlayer->GetReligions()->OwnsReligion())
	{
		return true;
	}

	// Also true if created a pantheon and there are religions left to create
	if(pReligions->HasCreatedPantheon(pPlayer->GetID()))
	{
		if (pReligions->GetNumReligionsStillToFound() > 0 || pPlayer->GetPlayerTraits()->IsAlwaysReligion())
		{
			return true;
		}
	}

	return false;
}

/// "Tech Leader" player strategy: if the player is a leader technologically, they should build espionage buildings to slow stealing
bool EconomicAIHelpers::IsTestStrategy_TechLeader(CvPlayer* pPlayer)
{
	if (pPlayer->isHuman())
	{
		return false;
	}

	// don't evaluate if espionage hasn't started yet
	if (pPlayer->GetEspionageAI()->m_iTurnEspionageStarted == -1)
	{
		return false;
	}

	int iMyTechNum = GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown();

	int iNumOtherPlayers = 0;
	int iNumPlayersAheadInTech = 0;
	for(uint ui = 0; ui < MAX_MAJOR_CIVS; ui++)
	{
		PlayerTypes ePlayer = (PlayerTypes)ui;
		if(!GET_PLAYER(ePlayer).isAlive())
		{
			continue;
		}

		if (ePlayer == pPlayer->GetID())
		{
			continue;
		}

		iNumOtherPlayers++;
		int iNumTechs = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetTeamTechs()->GetNumTechsKnown();
		if (iNumTechs > iMyTechNum)
		{
			iNumPlayersAheadInTech++;
		}
	}

	if (iNumOtherPlayers > 0)
	{
		FlavorTypes eFlavorEspionage = NO_FLAVOR;
		for (int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
		{
			FlavorTypes eFlavor = (FlavorTypes)iFlavor;
			if (GC.getFlavorTypes(eFlavor) == "FLAVOR_ESPIONAGE")
			{
				eFlavorEspionage = eFlavor;
			}
		}
		ASSERT_DEBUG(eFlavorEspionage != NO_FLAVOR, "Could not find espionage flavor!");

		float fRatio = iNumPlayersAheadInTech / (float)iNumOtherPlayers;
		float fCutOff = (0.05f * pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavorEspionage));

		return fRatio < fCutOff;
	}
	else
	{
		return false;
	}
}

/// "Early Expansion" Player Strategy: An early Strategy simply designed to get player up to 3 Cities quickly.
/// This should be run every turn because it is used in garrison logic and we need to react quickly
bool EconomicAIHelpers::IsTestStrategy_EarlyExpansion(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	if (CannotMinorCiv(pPlayer, eStrategy))
		return false;

	if (pPlayer->isHuman() && GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE))
		return false;

	if(pPlayer->IsEmpireUnhappy())
		return false;

	//we do not want to lose time for building our settlers even if we haven't explored yet
	if (pPlayer->GetNumCitiesFounded() < 3)
		return true;

	//midgame depends on the map sitation ...
	PlayerProximityTypes closestNeighborProximity = NO_PLAYER_PROXIMITY;
	for (int i = 0; i < MAX_MAJOR_CIVS; i++)
	{
		CvPlayer& other = GET_PLAYER((PlayerTypes)i);
		if (pPlayer->getTeam() == other.getTeam())
			continue;
		//continue expanding if our military is good enough
		if (pPlayer->GetDiplomacyAI()->GetMilitaryStrengthComparedToUs(other.GetID()) <= STRENGTH_AVERAGE)
			continue;
		PlayerProximityTypes p = pPlayer->GetProximityToPlayer(other.GetID());
		if (p > closestNeighborProximity)
			closestNeighborProximity = p;
	}
	if (closestNeighborProximity >= PLAYER_PROXIMITY_NEIGHBORS)
		return false;

	//never in late game
	if (GC.getGame().getElapsedGameTurns() > 169)
		return false;

	//do this check as late as possible, it can be expensive
	CvPlot* pSettlePlot = pPlayer->GetBestSettlePlot(NULL);
	if (!pSettlePlot)
		return false;

	//as long as it's within our "sphere of influence"
	CvCity* pClosestCity = GC.getGame().GetClosestCityByPathLength(pSettlePlot, true);
	if (pClosestCity)
	{
		if (pClosestCity->getOwner() == pPlayer->GetID())
			return true;

		//check for near ties ...
		int iTheirDist = GC.getGame().GetClosestCityDistancePathLength(pSettlePlot, true);
		int iOurDist = pPlayer->GetCityDistancePathLength(pSettlePlot);
		if (iOurDist <= iTheirDist + 1)
			return true;
	}

	//if the neighbors are far away it's also good
	int iOffset = 0;
	if (pPlayer->GetPlayerTraits()->IsExpansionist())
		iOffset--; //smaller distance - keep expanding longer
	if (pPlayer->GetPlayerTraits()->IsSmaller() || pPlayer->GetPlayerTraits()->IsTourism())
		iOffset++; //larger distance - stop expanding earlier

	if (GC.getGame().GetClosestCityDistancePathLength(pSettlePlot, true) > 7 + iOffset)
		return true;

	return false;
}

/// "Enough Expansion" Player Strategy: Never want a lot of settlers hanging around
bool EconomicAIHelpers::IsTestStrategy_EnoughExpansion(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	bool bCannotExpand = pPlayer->isBarbarian() || CannotMinorCiv(pPlayer, eStrategy) || pPlayer->GetPlayerTraits()->IsNoAnnexing();

	if ((GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman()) || bCannotExpand)
	{
		return true;
	}
	
	if(pPlayer->getNumCities() <= 1)
	{
		return false;
	}

	if(pPlayer->IsEmpireVeryUnhappy())
	{
		return true;
	}

	static MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	if(pPlayer->GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
	{
		return true;
	}

	// If we are running "ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS"
	static EconomicAIStrategyTypes eExpandOther = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EXPAND_TO_OTHER_CONTINENTS");
	if (pPlayer->GetEconomicAI()->IsUsingStrategy(eExpandOther))
	{
		return false;
	}

	// If we are running "ECONOMICAISTRATEGY_EARLY_EXPANSION"
	static EconomicAIStrategyTypes eEarlyExpand = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
	if (pPlayer->GetEconomicAI()->IsUsingStrategy(eEarlyExpand))
	{
		return false;
	}
	
	//do this check last, it can be expensive
	if (!pPlayer->HaveGoodSettlePlot(-1) )
	{
		return true;
	}

	return false;
}

/// "Need Happiness" Player Strategy: Time for Happiness?
bool EconomicAIHelpers::IsTestStrategy_NeedHappiness(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if(CannotMinorCiv(pPlayer, eStrategy) || pPlayer->isBarbarian())
	{
		return false;
	}

	if(pPlayer->getTotalPopulation() > 0 && pPlayer->GetUnhappiness() > 0)
	{
		int iExcessHappiness = pPlayer->GetExcessHappiness();

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);	// 1 Weight per HAPPINESS Flavor

		// This will range from 0 to 5. If Happiness is less than this we will activate the strategy
		int iDivisor = /*2*/ pStrategy->GetWeightThreshold();
		iWeightThresholdModifier /= iDivisor;

		if(iExcessHappiness <= iWeightThresholdModifier)
			return true;
	}

	return false;
}

/// "Need Happiness" Player Strategy: REALLY time for Happiness?
bool EconomicAIHelpers::IsTestStrategy_NeedHappinessCritical(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	// If we're losing at war, return false
	if(pPlayer->GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
		return false;

	if(pPlayer->getTotalPopulation() > 0 && pPlayer->GetUnhappiness() > 0)
	{
		int iExcessHappiness = pPlayer->GetExcessHappiness();

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iThreshold = /*-3*/ pStrategy->GetWeightThreshold();

		if(iExcessHappiness <= iThreshold)
			return true;
	}

	return false;
}

/// "Cities Need Naval Growth" Player Strategy: Looks at how many of this player's Cities need NAVAL_GROWTH, and depending on the intrinsic NAVAL_GROWTH Flavor decides whether or not it's worth prioritizing this Flavor on an empire-wide scale
bool EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalGrowth(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iNumCitiesNeedNavalGrowth = 0;

	CvCity* pLoopCity = NULL;
	int iLoop = 0;

	AICityStrategyTypes eStrategyNeedNavalGrowth = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_GROWTH");

	// CityStrategy doesn't exist in XML, so abort
	if(eStrategyNeedNavalGrowth == NO_AICITYSTRATEGY)
	{
		return false;
	}

	for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyNeedNavalGrowth))
		{
			iNumCitiesNeedNavalGrowth++;
		}
	}

	if(iNumCitiesNeedNavalGrowth > 0)
	{
		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);	// 1 Weight per NAVAL_GROWTH Flavor
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;	// 25

		int iCurrentWeight = (pPlayer->getNumCities() - 1) * 10;
		iCurrentWeight /= iWeightThreshold;

		if(iNumCitiesNeedNavalGrowth > iCurrentWeight)
		{
			return true;
		}
	}

	return false;
}

/// "Cities Need Naval Tile Improvement" Player Strategy: Looks at how many of this player's Cities need NAVAL_TILE_IMPROVEMENT, and depending on the intrinsic NAVAL_TILE_IMPROVEMENT Flavor decides whether or not it's worth prioritizing this Flavor on an empire-wide scale
bool EconomicAIHelpers::IsTestStrategy_CitiesNeedNavalTileImprovement(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iNumCitiesNeedNavalTileImprovement = 0;

	CvCity* pLoopCity = NULL;
	int iLoop = 0;

	AICityStrategyTypes eStrategyNeedNavalTileImprovement = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT");

	// CityStrategy doesn't exist in XML, so abort
	if(eStrategyNeedNavalTileImprovement == NO_AICITYSTRATEGY)
	{
		return false;
	}

	for(pLoopCity = pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyNeedNavalTileImprovement))
		{
			iNumCitiesNeedNavalTileImprovement++;
		}
	}

	if(iNumCitiesNeedNavalTileImprovement > 0)
	{
		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);	// 1 Weight per NAVAL_TILE_IMPROVEMENT Flavor
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;	// 25

		int iCurrentWeight = (pPlayer->getNumCities() - 1) * 10;
		iCurrentWeight /= iWeightThreshold;

		// See CvStrategyAI::IsTestStrategy_CitiesNeedBorders for a couple examples on how the math here works

		// Do enough of our Cities want NavalTileImprovement? [Average is 10/30; range is 10/25 to 10/35]
		if(iNumCitiesNeedNavalTileImprovement > iCurrentWeight)
		{
			return true;
		}
	}

	return false;
}

/// "Found City" Player Strategy: If there is a settler who isn't in an operation?  If so, find him a city site
bool EconomicAIHelpers::IsTestStrategy_FoundCity(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	// Never run this strategy for OCC, barbarians or minor civs
	if ((GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman()) || pPlayer->isBarbarian() || CannotMinorCiv(pPlayer, eStrategy))
		return false;

	if (pPlayer->GetNumCitiesFounded() == 0) //in this case homeland (first settler moves) should apply
		return false;

	// Never run this strategy for a human player
	if(pPlayer->isHuman())
		return false;

	// Won't be allowed to settle ...
	if (pPlayer->IsEmpireVeryUnhappy())
		return false;

	// Look at map for loose settlers
	std::vector<CvUnit*> vSettlers;
	int iUnitLoop = 0;
	for(CvUnit* pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
	{
		if (pLoopUnit->getArmyID() != -1)
			continue;

		if(pLoopUnit->canFoundCity(NULL,true,true))
			vSettlers.push_back(pLoopUnit);
	}

	if (vSettlers.empty())
		return false;

	EconomicAIStrategyTypes eEarlyExpand = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
	bool bIsEarlyExpansion = (eEarlyExpand != NO_ECONOMICAISTRATEGY) && pPlayer->GetEconomicAI()->IsUsingStrategy(eEarlyExpand);

	for (size_t i=0; i<vSettlers.size(); i++)
	{
		CvUnit* pLoopUnit = vSettlers[i];
		//this ignores all existing operations' targets by default so we won't pick the same plot twice
		CvPlot* pBestSettle = pPlayer->GetBestSettlePlot(pLoopUnit);
		if (!pBestSettle)
			continue;

		//we need good plots for our core cities, so hold off for a while if the plot is not good enough
		//there's a chance we just haven't explored enough yet
		int iFoundValue = pBestSettle->getFoundValue(pPlayer->GetID());
		if (bIsEarlyExpansion && pPlayer->getCapitalCity())
		{
			int iCapitalFoundValue = pPlayer->getCapitalCity()->plot()->getFoundValue(pPlayer->GetID());
			if (iFoundValue < iCapitalFoundValue / 2)
			{
				CvString msg = CvString::format("Passing on settle plot for unit %d at %d,%d - value %d - because it is not good enough", 
					pLoopUnit->GetID(), pBestSettle->getX(), pBestSettle->getY(), iFoundValue);
				pPlayer->GetHomelandAI()->LogHomelandMessage(msg);
				continue;
			}
		}

		pPlayer->addAIOperation(AI_OPERATION_FOUND_CITY, 1, NO_PLAYER, NULL, NULL);
;	}

	//always return false! this strategy never starts, but we try each turn
	return false;
}


/// "Trade with City State" Player Strategy: If there is a merchant who isn't in an operation?  If so, find him a city state
bool EconomicAIHelpers::IsTestStrategy_TradeWithCityState(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	// Never run this strategy for a human player
	if (pPlayer->isHuman())
		return false;

	// Look at map for loose merchants
	int iLooseMerchant = 0;
	int iUnitLoop = 0;
	for(CvUnit* pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
	{
		if(pLoopUnit != NULL)
		{
			if(pLoopUnit->AI_getUnitAIType() == UNITAI_MERCHANT && pLoopUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER)
			{
				if(pLoopUnit->getArmyID() == -1)
				{
					iLooseMerchant++;
				}
			}
		}
	}

	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	int iStrategyWeight = iLooseMerchant * 10;   // Just one merchant will trigger this
	int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
	int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

	if(iStrategyWeight >= iWeightThreshold)
	{
		// Launch an operation.
		return pPlayer->addAIOperation(AI_OPERATION_MERCHANT_DELEGATION, 1) != NULL;
	}

	return false;
}

/// "Influence City State" Player Strategy: If there is a diplomat who isn't in an operation?  If so, find him a city state
bool EconomicAIHelpers::IsTestStrategy_InfluenceCityState(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iUnitLoop = 0;
	CvUnit* pLoopUnit = NULL;
	int iLooseDiplomat = 0;
	int iStrategyWeight = 0;

	// Never run this strategy for a human player
	if(!pPlayer->isHuman())
	{
		// Look at map for loose diplomats
		for(pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
		{
			if(pLoopUnit != NULL)
			{
				if((pLoopUnit->AI_getUnitAIType() == UNITAI_DIPLOMAT) && (pLoopUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_USE_POWER))
				{
					if(pLoopUnit->getArmyID() == -1)
					{
						iLooseDiplomat++;
					}
				}
			}
		}

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		iStrategyWeight = iLooseDiplomat * 10;   // Just one diplomat will trigger this
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		if(iStrategyWeight >= iWeightThreshold)
		{
			// Launch an operation.
			return pPlayer->addAIOperation(AI_OPERATION_DIPLOMAT_DELEGATION,1)!=NULL;
		}
	}

	return false;
}

/// "Concert Tour" Player Strategy: If there is a musician who isn't in an operation?  If so, find him a major civ to target
bool EconomicAIHelpers::IsTestStrategy_ConcertTour(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iUnitLoop = 0;
	CvUnit* pLoopUnit = NULL;
	int iLooseMusician = 0;
	int iStrategyWeight = 0;

	// Never run this strategy for a human player
	if(!pPlayer->isHuman())
	{
		// Look at map for loose merchants
		for(pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
		{
			if(pLoopUnit != NULL)
			{
				if(pLoopUnit->AI_getUnitAIType() == UNITAI_MUSICIAN && pLoopUnit->GetGreatPeopleDirective() == GREAT_PEOPLE_DIRECTIVE_TOURISM_BLAST)
				{
					if(pLoopUnit->getArmyID() == -1)
					{
						iLooseMusician++;
					}
				}
			}
		}

		CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
		iStrategyWeight = iLooseMusician * 10;   // Just one musician will trigger this
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pPlayer);
		int iWeightThreshold = pStrategy->GetWeightThreshold() + iWeightThresholdModifier;

		if(iStrategyWeight >= iWeightThreshold)
		{
			// Launch an operation.
			return pPlayer->addAIOperation(AI_OPERATION_MUSICIAN_CONCERT_TOUR,1)!=NULL;
		}
	}

	return false;
}

/// "Need Improvement" Player Strategy: Do the cities need an improvement that increases a yield? Do we have access to an improvement that builds that yield?
bool EconomicAIHelpers::IsTestStrategy_NeedImprovement(CvPlayer* pPlayer, YieldTypes eYield)
{
	// find the city strategy associated with this issue
	AICityStrategyTypes eCityStrategy = NO_AICITYSTRATEGY;
	switch(eYield)
	{
	case YIELD_FOOD:
		eCityStrategy = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_IMPROVEMENT_FOOD");
		break;
	case YIELD_PRODUCTION:
		eCityStrategy = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_NEED_IMPROVEMENT_PRODUCTION");
		break;
	default:
		UNREACHABLE(); // Only YIELD_FOOD & YIELD_PRODUCTION supported.
	}

	ASSERT_DEBUG(eCityStrategy != NO_AICITYSTRATEGY, "No strategy found. What?");
	if(eCityStrategy == NO_AICITYSTRATEGY)
	{
		return false;
	}

	// if enough cities are worried about this problem
	int iNumCities = pPlayer->getNumCities();
	if(iNumCities == 0)
	{
		return false; // no cities, no problem!
	}

	int iCityLoop = 0;
	CvCity* pLoopCity = NULL;
	int iNumCitiesConcerned = 0;
	for(pLoopCity = pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
	{
		if(pLoopCity->GetCityStrategyAI()->IsUsingCityStrategy(eCityStrategy))
		{
			iNumCitiesConcerned++;
		}
	}

	double fWarningRatio = /*0.34f*/ GD_FLOAT_GET(AI_STRATEGY_NEED_IMPROVEMENT_CITY_RATIO);

	// if not enough cities are upset
	if((iNumCitiesConcerned / (double)iNumCities) < fWarningRatio)
	{
		return false;
	}

	// see if there's a builder
	int iUnitLoop = 0;
	CvUnit* pLoopUnit = NULL;
	CvUnit* pBuilder = NULL;
	for(pLoopUnit = pPlayer->firstUnit(&iUnitLoop); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iUnitLoop))
	{
		if(pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
		{
			pBuilder = pLoopUnit;
			break;
		}
	}

	// if no builder, ignore
	// perhaps prompt a builder?
	if(!pBuilder)
	{
		return false;
	}

	// is there a build that I can create to improve the yield?

	// loop through the build types to find one that we can use
	BuildTypes eBuild;
	int iBuildIndex = 0;
	for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
	{
		eBuild = (BuildTypes)iBuildIndex;
		CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
		if(pkBuildInfo)
		{
			if(pkBuildInfo->getTechPrereq() != NO_TECH)
			{
				if(!(GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechPrereq())))
				{
					// don't have the tech needed for this build, so look at the next one
					continue;
				}
			}
#if defined(MOD_BALANCE_CORE)
			if(pkBuildInfo->getTechObsolete() != NO_TECH)
			{
				if((GET_TEAM(pPlayer->getTeam()).GetTeamTechs()->HasTech((TechTypes)GC.getBuildInfo(eBuild)->getTechObsolete())))
				{
					// don'have the tech that obsoletes this build, so look at the next one
					continue;
				}
			}
#endif

			ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild)->getImprovement();
			if(eImprovement == NO_IMPROVEMENT)
			{
				continue;
			}

			bool bCanBuild = false;
			const PlotIndexContainer& aiPlots = pPlayer->GetPlots();
			// go through all the plots the player has under their control
			for (PlotIndexContainer::const_iterator it = aiPlots.begin(); it != aiPlots.end(); ++it)
			{
				CvPlot* pPlot = GC.getMap().plotByIndex(*it);
				if (!pPlot)
				{
					continue;
				}
				if(pPlayer->canBuild(pPlot, eBuild, false /*test era*/, false /*test visible*/, false /*test gold*/))
				{
					bCanBuild = true;
					break;
				}
			}

			if(!bCanBuild)
			{
				continue;
			}

			// we can use an improvement that increases the yield
			if(GC.getImprovementInfo(eImprovement)->GetYieldChange(eYield) > 0)
			{
				return false;
			}
		}
	}

	return true;
}

/// "One or Fewer Coastal Cities" Player Strategy: If we don't have 2 coastal cities, this runs nullifying the WATER_CONNECTION Flavor
bool EconomicAIHelpers::IsTestStrategy_OneOrFewerCoastalCities(CvPlayer* pPlayer)
{
	int iCityLoop = 0;
	CvCity* pLoopCity = NULL;
	int iNumCoastalCities = 0;
	for(pLoopCity = pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iCityLoop))
	{
		if(pLoopCity->isCoastal())
		{
			iNumCoastalCities++;
		}
	}
	return iNumCoastalCities <= 1;
}

/// "Losing Money" Player Strategy: Stop building military if in a financial downturn
bool EconomicAIHelpers::IsTestStrategy_LosingMoney(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	
	// Need a certain number of turns of history before we can turn this on
	if(GC.getGame().getGameTurn() <= pStrategy->GetFirstTurnExecuted())
		return false;

	int iInterval = pStrategy->GetMinimumNumTurnsExecuted();
	int iIncome100 = pPlayer->GetTreasury()->AverageIncome100(iInterval);
	if (iIncome100 >= 0)
		return false;

	int iTurnsTillBroke = (pPlayer->GetTreasury()->GetGold() * 100) / abs(iIncome100);
	return iTurnsTillBroke < 23;
}

/// Are we paying more in unit maintenance than we are taking in from our cities?
bool EconomicAIHelpers::IsTestStrategy_TooManyUnits(CvPlayer* pPlayer)
{
	if(pPlayer->GetTreasury()->AverageIncome100(10) <= -500 && pPlayer->GetTreasury()->GetGold() <= 100)
	{
		return true;
	}
	if(pPlayer->GetTreasury()->GetGold() <= 0)
	{
		return true;
	}
	if(pPlayer->GetTreasury()->AverageIncome100(15) <= -1500)
	{
		if (pPlayer->GetTreasury()->CalculateUnitCost() > (pPlayer->GetTreasury()->GetBuildingGoldMaintenance() + pPlayer->GetTreasury()->GetImprovementGoldMaintenance()))
		{
			return true;
		}
	}

	return (pPlayer->GetUnitProductionMaintenanceMod()) != 0;
}

/// Did we start the game on a small continent (50 tiles or less)?
bool EconomicAIHelpers::IsTestStrategy_IslandStart(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	int iCoastalTiles = 0;
	int iRevealedCoastalTiles = 0;

	CvEconomicAIStrategyXMLEntry* pStrategy = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);

	// Only kick off this strategy in the first 25 turns of the game (though it will last 50 turns once selected)
	if(GC.getGame().getGameTurn() < 25 && pPlayer->getStartingPlot())
	{
		if(!pPlayer->CanEmbark())
		{
			int iStartLandmass = pPlayer->getStartingPlot()->getLandmass();

			// Have we revealed a high enough percentage of the coast of our landmass?
			for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndexUnchecked(iI);
				if(pLoopPlot->getLandmass() == iStartLandmass)
				{
					if(pLoopPlot->isCoastalLand())
					{
						iCoastalTiles++;
					}
					if(pLoopPlot->isRevealed(pPlayer->getTeam()))
					{
						iRevealedCoastalTiles++;
					}
				}
			}

			if((iRevealedCoastalTiles * 100 / (iCoastalTiles + 1)) > /*80*/ GD_INT_GET(AI_STRATEGY_ISLAND_START_COAST_REVEAL_PERCENT) &&
			        GC.getMap().getLandmassById(iStartLandmass)->getNumTiles() < pStrategy->GetWeightThreshold())
			{
				return true;
			}
		}
	}

	return false;
}

/// Are we running out of room on our current landmass?
bool EconomicAIHelpers::IsTestStrategy_ExpandToOtherContinents(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer)
{
	if(GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && pPlayer->isHuman())
		return false;

	if (pPlayer->IsEmpireUnhappy() || CannotMinorCiv(pPlayer, eStrategy) || !pPlayer->getCapitalCity())
		return false;

	if(!pPlayer->CanEmbark())
		return false;

	int iLoopCity = 0;
	bool bCoastal = false;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isCoastal())
		{
			bCoastal = true;
			break;
		}
	}
	if(!bCoastal)
		return false;

	static MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
	// scale based on flavor and world size
	if(eBuildCriticalDefenses != NO_MILITARYAISTRATEGY && pPlayer->GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses) && !pPlayer->IsCramped())
		return false;

	// Never run this at the same time as island start
	static EconomicAIStrategyTypes eStrategyIslandStart = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ISLAND_START");
	if(eStrategyIslandStart != NO_ECONOMICAISTRATEGY)
	{
		if(pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyIslandStart))
			return false;
	}

	// we should settle our island first
	static EconomicAIStrategyTypes eEarlyExpansion = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
	if(eEarlyExpansion != NO_ECONOMICAISTRATEGY)
	{
		if(pPlayer->GetEconomicAI()->IsUsingStrategy(eEarlyExpansion))
			return false;
	}

	//don't do it if we already have a lot of cities
	int iFlavorExpansion = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));
	if (pPlayer->GetNumCitiesFounded() < iFlavorExpansion * 2)
		return false;

	//check for sparseley settled areas first
	CvArea* pStart = pPlayer->getCapitalCity()->plot()->area();
	for (int i = 0; i < GC.getMap().getNumAreas(); i++)
	{
		CvArea* pArea = GC.getMap().getAreaByIndex(i);
		if (!pArea)
			continue;

		if (pArea->isWater())
			continue;

		//make up an arbitrary threshold for islands
		if (pArea->getNumTiles() < 23)
			continue;

		//have to know about it
		if (pArea->getNumRevealedTiles(pPlayer->getTeam()) < 3)
			continue;

		//assuming 10 tiles per city (some may be water) at most 1/3 of the good plots should be claimed
		if (pArea->getNumCities() * 10 > (pArea->getNumTiles()-pArea->GetNumBadPlots()) / 3)
			continue;

		//do this last, potentially expensive!
		// * note this doesn't guarantee that our settler will actually go there ...
		// * note this doesn't guarantee that our area is actually on another continent ...
		//but good enough, this strategy is called only in the mid-game, by then all empty areas are fair game
		if (pPlayer->HaveGoodSettlePlot(pArea->GetID()) && !pPlayer->HaveGoodSettlePlot(pStart->GetID()))
			return true;
	}

	return false;
}

/// Are we mostly on the coast?
bool EconomicAIHelpers::IsTestStrategy_MostlyOnTheCoast(CvPlayer* pPlayer)
{
	// total pop
	int iInlandPop = 0;
	int iCoastalPop = 0;
	int iLoopCity = 0;
	CvCity* pLoopCity = NULL;
	for(pLoopCity = pPlayer->firstCity(&iLoopCity); pLoopCity != NULL; pLoopCity = pPlayer->nextCity(&iLoopCity))
	{
		if(pLoopCity->isCoastal())  // don't run this for lakes or even small inland seas
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

// MORE NON-MEMBER FUNCTIONS
//    This set of non member functions are used to evaluate which operation to launch

/// "Grand Strategy Culture" Player Strategy: Run this if our Grand Strategy is to go for Culture
bool EconomicAIHelpers::IsTestStrategy_GS_Culture(CvPlayer* pPlayer)
{
	return pPlayer->isMajorCiv() && pPlayer->GetDiplomacyAI()->IsGoingForCultureVictory();
}

/// "Grand Strategy Conquest" Player Strategy: Run this if our Grand Strategy is to go for Conquest
bool EconomicAIHelpers::IsTestStrategy_GS_Conquest(CvPlayer* pPlayer)
{
	return pPlayer->isMajorCiv() && pPlayer->GetDiplomacyAI()->IsGoingForWorldConquest();
}

/// "Grand Strategy Diplomacy" Player Strategy: Run this if our Grand Strategy is to go for Diplomacy
bool EconomicAIHelpers::IsTestStrategy_GS_Diplomacy(CvPlayer* pPlayer)
{
	return pPlayer->isMajorCiv() && pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory();
}

/// "Grand Strategy Spaceship" Player Strategy: Run this if our Grand Strategy is to go for the Spaceship
bool EconomicAIHelpers::IsTestStrategy_GS_Spaceship(CvPlayer* pPlayer)
{
	return pPlayer->isMajorCiv() && pPlayer->GetDiplomacyAI()->IsGoingForSpaceshipVictory();
}

/// We are in the home stretch of the "Grand Strategy Spaceship" Player Strategy
bool EconomicAIHelpers::IsTestStrategy_GS_SpaceshipHomestretch(CvPlayer* pPlayer)
{
	// if I already built the Apollo Program I should follow through
	ProjectTypes eApolloProgram = (ProjectTypes) GC.getInfoTypeForString("PROJECT_APOLLO_PROGRAM", true);
	if(eApolloProgram != NO_PROJECT)
	{
		if(GET_TEAM(pPlayer->getTeam()).getProjectCount(eApolloProgram) > 0)
		{
			return true;
		}
	}

	return false;
}

/// We see a lot of sites needing archaeologists
bool EconomicAIHelpers::IsTestStrategy_NeedArchaeologists(CvPlayer* pPlayer)
{
	int iNumSites = pPlayer->GetEconomicAI()->GetVisibleAntiquitySites();
	int iNumArchaeologists = pPlayer->GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true);
	int iNumSitesOwn = pPlayer->GetEconomicAI()->GetVisibleAntiquitySitesOwnTerritory();
	int iNumSitesNeutral = pPlayer->GetEconomicAI()->GetVisibleAntiquitySitesUnownedTerritory();

	if (iNumSitesOwn > iNumArchaeologists)
		return true;

	if ((iNumSitesOwn + iNumSitesNeutral > iNumArchaeologists * 2) && !pPlayer->IsAtWar())
		return true;

	if ((iNumSitesOwn + iNumSitesNeutral + iNumSites > iNumArchaeologists * 4) && !pPlayer->IsAtWar() && iNumArchaeologists < 3)
		return true;

	return false;
}

/// There are more archaeologists than sites
bool EconomicAIHelpers::IsTestStrategy_EnoughArchaeologists(CvPlayer* pPlayer)
{
	int iNumSites = GC.getGame().GetNumArchaeologySites();
	double iMaxRatio = .25; //Ratio of archaeologists to sites

	int iNumArchaeologists = pPlayer->GetNumUnitsWithUnitAI(UNITAI_ARCHAEOLOGIST, true);
	PolicyTypes eExpFinisher = (PolicyTypes) GC.getInfoTypeForString("POLICY_EXPLORATION_FINISHER", true /*bHideAssert*/);

	if (eExpFinisher != NO_POLICY)
	{
		if (pPlayer->GetPlayerPolicies()->HasPolicy(eExpFinisher))
		{
			iNumSites += GC.getGame().GetNumHiddenArchaeologySites();
		}
	}

	if ((double)iNumSites * iMaxRatio + 1 < iNumArchaeologists)
	{
		return true;
	}

	return false;
}

/// Do we need more Diplomatic Units? Let's score it and see.
bool EconomicAIHelpers::IsTestStrategy_NeedDiplomats(CvPlayer* pPlayer)
{
	static EconomicAIStrategyTypes eStrategyNeedDiplomatsCritical = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL");

	if(pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyNeedDiplomatsCritical))
	{
		return false;
	}

	int iScore = IsTestStrategy_ScoreDiplomats(pPlayer);

	return (iScore > 0) && (iScore <= 15);
}

bool EconomicAIHelpers::IsTestStrategy_NeedDiplomatsCritical(CvPlayer* pPlayer)
{
	static EconomicAIStrategyTypes eStrategyNeedDiplomats = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS");

	if(pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategyNeedDiplomats))
	{
		return false;
	}

	int iScore = IsTestStrategy_ScoreDiplomats(pPlayer);
	return iScore > 15;
}

int EconomicAIHelpers::IsTestStrategy_ScoreDiplomats(CvPlayer* pPlayer)
{

	PlayerTypes ePlayer = pPlayer->GetID();
	CvLeague* pLeague = GC.getGame().GetGameLeagues()->GetActiveLeague();

	//The Base INTs
	int iNumCities = 0;
	int iNumAllies = 0;
	int iNumFriends = 0;

	CvUnit* pLoopUnit = NULL;
	int iLoopUnit = 0;
	CvUnit* TestUnit = NULL;
	for (pLoopUnit = pPlayer->firstUnit(&iLoopUnit); pLoopUnit != NULL; pLoopUnit = pPlayer->nextUnit(&iLoopUnit))
	{
		if (pLoopUnit->AI_getUnitAIType() == UNITAI_MESSENGER)
		{
			TestUnit = pLoopUnit;
			break;
		}
	}

	if (TestUnit != NULL && GET_PLAYER(pPlayer->GetID()).ChooseMessengerTargetPlot(TestUnit) == NULL)
		return -1;

	// Loop through all minors and get the total number we've met.
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		PlayerTypes eMinor = (PlayerTypes) iPlayerLoop;

		if (eMinor != ePlayer && GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
		{
			if (GET_PLAYER(eMinor).GetMinorCivAI()->IsNoAlly())
				continue;

			if(GET_PLAYER(eMinor).GetMinorCivAI()->IsHasMetPlayer(ePlayer))
			{
				iNumCities++;
			}
			if (GET_PLAYER(eMinor).GetMinorCivAI()->IsAllies(ePlayer))
			{
				iNumAllies++;
			}
			if (GET_PLAYER(eMinor).GetMinorCivAI()->IsFriends(ePlayer))
			{
				iNumFriends++;
			}
		}
	}
	if(iNumCities > 0)
	{
		//are we allied with everyone? Well, we don't need any diplo units then!
		if (iNumAllies == iNumCities)
			return 0;

		int iNumApConq = 0;
		int iNumApBully = 0;
		int iNumCloseNotAllies = 0;
		int iNumInfluenceQuest = 0;
		int iMinorAllyEnemy = 0;
		int iMinorAllyCompetitor = 0;
		int iMinorAllyHostile = 0;
		int iMinorAllyDispute = 0;
		int iVictoryAllyDispute = 0;
		int iLotsOfVotes = 0;
		int iWinVotes = 0;
		//The rest of the INTs
	
		int iFlavorDiplo =  pPlayer->GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));

		int iCSDesire = (((iFlavorDiplo + /*2*/ GD_INT_GET(NEED_DIPLOMAT_DESIRE_MODIFIER)) * iNumCities) / 10); //Baseline Desire. Global modifier increases this.

		int iCSDistaste = /*6*/ GD_INT_GET(NEED_DIPLOMAT_DISTASTE_MODIFIER) - iFlavorDiplo; //Lack of desire. Lower is better for diplo. If negative, counts as zero.

		int	iThreshold = iNumCities * /*125*/ GD_INT_GET(NEED_DIPLOMAT_THRESHOLD_MODIFIER) / 100; //This is the baseline threshold: Number of city-states in-game increases baseline. Changing Global value decreases diplomatic competitiveness.

		//The Minor/Major Loop Tests
		// Loop through all minors and majors to get our relations with them.
		for(int iMinorCivLoop = MAX_MAJOR_CIVS; iMinorCivLoop < MAX_CIV_PLAYERS; iMinorCivLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes) iMinorCivLoop;
			PlayerTypes eMinorAlly = NO_PLAYER;
			eMinorAlly = GET_PLAYER(eMinor).GetMinorCivAI()->GetAlly();

			if(eMinor)
			{
				//Are we aggressive towards city-states? If so, discourage building diplo units.
				if (pPlayer->GetDiplomacyAI()->GetCivApproach(eMinor) == CIV_APPROACH_WAR)
				{
					iNumApConq++;
				}
				//Are we planning on bullying them?
				else if (pPlayer->GetDiplomacyAI()->GetCivApproach(eMinor) == CIV_APPROACH_HOSTILE)
				{
					iNumApBully++;
				}

				//About to lose an ally? Build units!
				if (GET_PLAYER(eMinor).GetMinorCivAI()->IsCloseToNotBeingAllies(ePlayer))
				{
					iNumCloseNotAllies++;
				}

				// Is there an active influence quest? Let's take advantage of that.
				if (GET_PLAYER(eMinor).GetMinorCivAI()->IsActiveQuestForPlayer(ePlayer, MINOR_CIV_QUEST_INFLUENCE))
				{
					iNumInfluenceQuest++;
				}

				//MINOR ALLY TESTS - If other minors are allied, what's our opinion of the ally? 
				if (eMinorAlly != NO_PLAYER)
				{
					CivOpinionTypes eOpinion = pPlayer->GetDiplomacyAI()->GetCivOpinion(eMinorAlly);
					CivApproachTypes eApproach = pPlayer->GetDiplomacyAI()->GetCivApproach(eMinorAlly);
					CivApproachTypes eApproachTowardsUs = pPlayer->GetDiplomacyAI()->GetVisibleApproachTowardsUs(eMinorAlly);

					if (eOpinion == CIV_OPINION_COMPETITOR)
					{
						iMinorAllyCompetitor++;
					}
					if (eOpinion <= CIV_OPINION_ENEMY)
					{
						iMinorAllyEnemy++;
					}
					if (eApproach <= CIV_APPROACH_HOSTILE || eApproachTowardsUs <= CIV_APPROACH_HOSTILE)
					{
						iMinorAllyHostile++;
					}
					if (pPlayer->GetDiplomacyAI()->GetMinorCivDisputeLevel(eMinorAlly) == DISPUTE_LEVEL_STRONG || pPlayer->GetDiplomacyAI()->GetMinorCivDisputeLevel(eMinorAlly) == DISPUTE_LEVEL_FIERCE)
					{					
						iMinorAllyDispute++;
					}
					if (pPlayer->GetDiplomacyAI()->GetVictoryDisputeLevel(eMinorAlly) == DISPUTE_LEVEL_STRONG || pPlayer->GetDiplomacyAI()->GetVictoryDisputeLevel(eMinorAlly) == DISPUTE_LEVEL_FIERCE)
					{					
						iVictoryAllyDispute++;
					}
				}
			}
		}

		for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eOtherMajor = (PlayerTypes) iPlayerLoop;

			if (eOtherMajor != ePlayer && GET_PLAYER(eOtherMajor).isAlive() && !GET_PLAYER(eOtherMajor).isMinorCiv())
			{	
				//Does someone have more than half of the votes needed to win a diplomatic victory? Let's not like them at all!
				if(pLeague != NULL)
				{
					if(pLeague->CalculateStartingVotesForMember(eOtherMajor) >= (GC.getGame().GetVotesNeededForDiploVictory() / 2))
					{
						iLotsOfVotes++;
					}
					//Does someone have more than 4/5? That's awful!
					if(pLeague->CalculateStartingVotesForMember(eOtherMajor) >= ((GC.getGame().GetVotesNeededForDiploVictory() * 400) / 500))
					{
						iWinVotes++;
					}
				}
			}
		}

//////////////////
/////////////
//Positive Tests
////////////
/////////////////

		int iScore = 0;

		//Do we have less alliances than we want?
		if(iNumAllies < iCSDesire)
		{
			iScore += iCSDesire + (iNumCities - iNumAllies);
		}
		
		// You have non-ally friends? Add points for this.
		if((iNumFriends - iNumAllies) > 0)
		{
			iScore += (iNumFriends - iNumAllies);
		}

		//About to lose an ally? Is there an active influence quest? Build units!
		if((iNumCloseNotAllies > 0) || (iNumInfluenceQuest > 0))
		{
			iScore += iCSDesire;
		}

		//Going for a Diplo Victory?
		if(pPlayer->GetDiplomacyAI()->IsGoingForDiploVictory())
		{
			iScore += (iCSDesire * 5);
		}

		//APPROACH WEIGHTS - These will make the AI more competitive if they are fighting for minors.
		//iCSDistaste used to show how sensitive a player is to alliances. The lower the distaste, the more sensitive they are (i.e. the higher their diplo flavor, the more they care).
		//In order of importance...

		//How many minors are allied to civs we are disputing over minors with?
		if(iCSDistaste >= 0)
		{
			if((iMinorAllyDispute - iCSDistaste) > 0)
			{
				iScore += (iMinorAllyDispute - iCSDistaste);
			}

			//How many minors are allied to competitors?
			if((iMinorAllyCompetitor - iCSDistaste) > 0)
			{
				iScore += (iMinorAllyCompetitor - iCSDistaste);
			}

			//How many minors are allied to enemies?
			if((iMinorAllyEnemy - iCSDistaste) > 0)
			{
				iScore += (iMinorAllyEnemy - iCSDistaste);
			}
			//How many minors are allied to players we are hostile towards?
			if((iMinorAllyHostile - iCSDistaste) > 0)
			{
				iScore += (iMinorAllyHostile - iCSDistaste);
			}

			//How many minors are allied to civs we are disputing over victory with?
			if((iVictoryAllyDispute - iCSDistaste) > 0)
			{
				iScore += (iVictoryAllyDispute - iCSDistaste);
			}
		}
		else if (iCSDistaste < 0)
		{
			if(iMinorAllyDispute > 0)
			{
				iScore += iMinorAllyDispute;
			}

			//How many minors are allied to competitors?
			if(iMinorAllyCompetitor > 0)
			{
				iScore += iMinorAllyCompetitor;
			}

			//How many minors are allied to enemies?
			if(iMinorAllyEnemy > 0)
			{
				iScore += iMinorAllyEnemy;
			}
			//How many minors are allied to players we are hostile towards?
			if(iMinorAllyHostile > 0)
			{
				iScore += iMinorAllyHostile;
			}

			//How many minors are allied to civs we are disputing over victory with?
			if(iVictoryAllyDispute > 0)
			{
				iScore += iVictoryAllyDispute;
			}
		}
		//Does someone have a lot of votes? That's not cool!
		if((iLotsOfVotes > 0))
		{
			iScore += (iCSDesire * 4);
		}

		//Is someone about to win? Oh no!
		if((iWinVotes > 0))
		{
			iScore += (iCSDesire * 6);
		}

/////////////////////////
///////////////////
//NEGATIVE TESTS
//////////////////
////////////////////////

		//Are we aggressive towards city-states? If so, discourage building diplo units.
		if(iNumApConq > 0)
		{
			if (iCSDistaste > 0)
			{
				iScore -= iNumApConq + iCSDistaste;
			}
			else
			{
				iScore -= iNumApConq;
			}
		}

		//Are we planning on bullying city-states?
		if(iNumApBully > 0)
		{
			if (iCSDistaste > 0)
			{
				iScore -= iNumApBully + iCSDistaste;
			}
			else
			{
				iScore -= iNumApBully;
			}
		}

		if(iScore > iThreshold)
		{
			iScore = (iScore - iThreshold);
			return iScore;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

/// We see sites but don't have any art/artifact slots
bool EconomicAIHelpers::IsTestStrategy_NeedMuseums(CvPlayer* pPlayer)
{
	int iNumSites = pPlayer->GetEconomicAI()->GetVisibleAntiquitySites();
	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
	int iNumGreatWorkSlots = pPlayer->GetCulture()->GetNumAvailableGreatWorkSlots(eArtArtifactSlot);

	return iNumSites > iNumGreatWorkSlots;
}

/// We have the tech for guilds but haven't built them yet
bool EconomicAIHelpers::IsTestStrategy_NeedGuilds(CvPlayer* pPlayer)
{
	const CvCivilizationInfo& playerCivilizationInfo = pPlayer->getCivilizationInfo();
	BuildingTypes eWritersGuild = static_cast<BuildingTypes>(playerCivilizationInfo.getCivilizationBuildings(GC.getInfoTypeForString("BUILDINGCLASS_WRITERS_GUILD")));
	BuildingTypes eArtistsGuild = static_cast<BuildingTypes>(playerCivilizationInfo.getCivilizationBuildings(GC.getInfoTypeForString("BUILDINGCLASS_ARTISTS_GUILD")));
	BuildingTypes eMusiciansGuild = static_cast<BuildingTypes>(playerCivilizationInfo.getCivilizationBuildings(GC.getInfoTypeForString("BUILDINGCLASS_MUSICIANS_GUILD")));

	if (eWritersGuild != NO_BUILDING)
	{
		CvBuildingEntry* pkWritersGuildInfo = GC.getBuildingInfo(eWritersGuild);
		if (pPlayer->HasTech(static_cast<TechTypes>(pkWritersGuildInfo->GetPrereqAndTech())))
		{
			if (!pPlayer->HasBuildingClass(pkWritersGuildInfo->GetBuildingClassType()))
			{
				return true;
			}
		}
	}

	if (eArtistsGuild != NO_BUILDING)
	{
		CvBuildingEntry* pkArtistsGuildInfo = GC.getBuildingInfo(eArtistsGuild);
		if (pPlayer->HasTech(static_cast<TechTypes>(pkArtistsGuildInfo->GetPrereqAndTech())))
		{
			if (!pPlayer->HasBuildingClass(pkArtistsGuildInfo->GetBuildingClassType()))
			{
				return true;
			}
		}
	}

	if (eMusiciansGuild != NO_BUILDING)
	{
		CvBuildingEntry* pkMusiciansGuildInfo = GC.getBuildingInfo(eMusiciansGuild);
		if (pPlayer->HasTech(static_cast<TechTypes>(pkMusiciansGuildInfo->GetPrereqAndTech())))
		{
			if (!pPlayer->HasBuildingClass(pkMusiciansGuildInfo->GetBuildingClassType()))
			{
				return true;
			}
		}
	}

	return false;
}

/// We have the tech for guilds but haven't built them yet
bool EconomicAIHelpers::IsTestStrategy_StartedPiety(CvPlayer* pPlayer)
{
	bool bRtnValue = false;

	PolicyBranchTypes eBranch = (PolicyBranchTypes)GC.getInfoTypeForString("POLICY_BRANCH_PIETY", true);
	if (eBranch != NO_POLICY_BRANCH_TYPE)
	{
		if (pPlayer->GetPlayerPolicies()->IsPolicyBranchUnlocked(eBranch))
		{
			return true;
		}
	}

	return bRtnValue;
}

bool EconomicAIHelpers::CannotMinorCiv(CvPlayer* pPlayer, EconomicAIStrategyTypes eStrategy)
{
	if (!pPlayer->isMinorCiv() || eStrategy == NO_ECONOMICAISTRATEGY)
		return false;

	CvEconomicAIStrategyXMLEntry* pkStrategyInfo = pPlayer->GetEconomicAI()->GetEconomicAIStrategies()->GetEntry(eStrategy);
	return pkStrategyInfo->IsNoMinorCivs();
}