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
#include "CvInternalGameCoreUtils.h"
#include "CvDiplomacyAIEnums.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvGrandStrategyAI.h"
#include "CvCitySpecializationAI.h"
#include "CvEspionageClasses.h"
#include "Fireworks/FVariableSystem.h"
#include "CvEnumSerialization.h"

// must be included after all other headers
#include "LintFree.h"

//=====================================
// CvAICityStrategyEntry
//=====================================
CvAICityStrategyEntry::CvAICityStrategyEntry(void):
	m_piFlavorValue(NULL),
	m_iWeightThreshold(0),
	m_piPersonalityFlavorThresholdMod(NULL),
	m_iTechPrereq(NO_TECH),
	m_iTechObsolete(NO_TECH),
	m_iMinimumNumTurnsExecuted(0),
	m_iCheckTriggerTurnCount(0),
	m_bNoMinorCivs(false),
	m_bPermanent(false),
	m_eAdvisor(NO_ADVISOR_TYPE),
	m_iAdvisorCounselImportance(1)
{
}
//------------------------------------------------------------------------------
CvAICityStrategyEntry::~CvAICityStrategyEntry(void)
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piPersonalityFlavorThresholdMod);
}
//------------------------------------------------------------------------------
bool CvAICityStrategyEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_bNoMinorCivs				= kResults.GetBool("NoMinorCivs");
	m_bPermanent				= kResults.GetBool("Permanent");
	m_iCheckTriggerTurnCount	= kResults.GetInt("CheckTriggerTurnCount");
	m_iMinimumNumTurnsExecuted	= kResults.GetInt("MinimumNumTurnsExecuted");
	m_iWeightThreshold			= kResults.GetInt("WeightThreshold");

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
	const char* szTextVal;
	szTextVal = kResults.GetText("TechObsolete");
	m_iTechObsolete = GC.getInfoTypeForString(szTextVal, true);

	szTextVal = kResults.GetText("TechPrereq");
	m_iTechPrereq = GC.getInfoTypeForString(szTextVal, true);

	//Arrays
	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "AICityStrategy_Flavors",
	                    "AICityStrategyType", szType);

	kUtility.SetFlavors(m_piPersonalityFlavorThresholdMod,
	                    "AICityStrategy_PersonalityFlavorThresholdMods",
	                    "AICityStrategyType", szType);

	return true;
}

/// What Flavors will be added by adopting this Strategy?
int CvAICityStrategyEntry::GetFlavorValue(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : -1;
}

/// The amount of weight a Strategy must accumulate to be adopted (if applicable)
int CvAICityStrategyEntry::GetWeightThreshold() const
{
	return m_iWeightThreshold;
}

/// How do a player's Personality Flavors affect the Threshold for adopting a Strategy? (if applicable)
int CvAICityStrategyEntry::GetPersonalityFlavorThresholdMod(int i) const
{
	CvAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	CvAssertMsg(i > -1, "Index out of bounds");
	return m_piPersonalityFlavorThresholdMod ? m_piPersonalityFlavorThresholdMod[i] : -1;
}

/// Technology prerequisite an AI Player must have to enable this Strategy
int CvAICityStrategyEntry::GetTechPrereq() const
{
	return m_iTechPrereq;
}

/// Technology that obsoletes this Strategy for an AI Player
int CvAICityStrategyEntry::GetTechObsolete() const
{
	return m_iTechObsolete;
}

/// Minimum # of turns this AI Strategy must be executed for - don't want the AI starting then stopping Strategies every turn
int CvAICityStrategyEntry::GetMinimumNumTurnsExecuted() const
{
	return m_iMinimumNumTurnsExecuted;
}

/// Once this Strategy has been adopted, check the Trigger conditions every X turns to see if it's maintained
int CvAICityStrategyEntry::GetCheckTriggerTurnCount() const
{
	return m_iCheckTriggerTurnCount;
}

/// Is this a valid CityStrategy for Minor Civs to run?
bool CvAICityStrategyEntry::IsNoMinorCivs() const
{
	return m_bNoMinorCivs;
}

/// Will this CityStrategy be run forever after being adopted? [Overwrites whatever is present in the above 2 functions/members]
bool CvAICityStrategyEntry::IsPermanent() const
{
	return m_bPermanent;
}

AdvisorTypes CvAICityStrategyEntry::GetAdvisor() const
{
	return m_eAdvisor;
}

const char* CvAICityStrategyEntry::GetAdvisorCounselText() const
{
	return m_strAdvisorCounselText.c_str();
}

int CvAICityStrategyEntry::GetAdvisorCounselImportance() const
{
	return m_iAdvisorCounselImportance;
}


//=====================================
// CvAICityStrategies
//=====================================

/// Constructor
CvAICityStrategies::CvAICityStrategies(void)
{

}

/// Destructor
CvAICityStrategies::~CvAICityStrategies(void)
{
	DeleteArray();
}

/// Returns vector of AICityStrategy entries
std::vector<CvAICityStrategyEntry*>& CvAICityStrategies::GetAICityStrategyEntries()
{
	return m_paAICityStrategyEntries;
}

/// Number of defined AICityStrategies
int CvAICityStrategies::GetNumAICityStrategies()
{
	return m_paAICityStrategyEntries.size();
}

/// Clear AICityStrategy entries
void CvAICityStrategies::DeleteArray()
{
	for(std::vector<CvAICityStrategyEntry*>::iterator it = m_paAICityStrategyEntries.begin(); it != m_paAICityStrategyEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paAICityStrategyEntries.clear();
}

/// Get a specific entry
CvAICityStrategyEntry* CvAICityStrategies::GetEntry(int index)
{
	return m_paAICityStrategyEntries[index];
}

//=====================================
// CvCityStrategyAI
//=====================================

/// defining static
unsigned char  CvCityStrategyAI::m_acBestYields[NUM_YIELD_TYPES][NUM_CITY_PLOTS];

/// Constructor
CvCityStrategyAI::CvCityStrategyAI():
	m_pabUsingCityStrategy(NULL),
	m_paiTurnCityStrategyAdopted(NULL),
	m_aiTempFlavors(NULL),
	m_pBuildingProductionAI(NULL),
	m_pUnitProductionAI(NULL),
	m_pProjectProductionAI(NULL),
	m_pProcessProductionAI(NULL),
	m_eFocusYield((YieldTypes)NO_YIELD)
{
}

/// Destructor
CvCityStrategyAI::~CvCityStrategyAI()
{
	Uninit();
}

/// Initialize
void CvCityStrategyAI::Init(CvAICityStrategies* pAICityStrategies, CvCity* pCity, bool bIsCity)
{
	// Init base class
	CvFlavorRecipient::Init();

	// Store off the pointers passed in to this object
	m_bIsCity = bIsCity;
	m_pAICityStrategies = pAICityStrategies;
	m_pCity = pCity;

	// Initialize arrays
	CvAssertMsg(m_pabUsingCityStrategy==NULL, "about to leak memory, CvCityAIStrategies::m_pabUsingCityStrategy");
	m_pabUsingCityStrategy = FNEW(bool[m_pAICityStrategies->GetNumAICityStrategies()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_paiTurnCityStrategyAdopted==NULL, "about to leak memory, CvCityAIStrategies::m_paiTurnCityStrategyAdopted");
	m_paiTurnCityStrategyAdopted = FNEW(int[m_pAICityStrategies->GetNumAICityStrategies()], c_eCiv5GameplayDLL, 0);

	CvAssertMsg(m_aiTempFlavors==NULL, "about to leak memory, CvCityAIStrategies::m_aiTempFlavors");
	m_aiTempFlavors = FNEW(int[GC.getNumFlavorTypes()], c_eCiv5GameplayDLL, 0);

	// Create AI subobjects
	m_pBuildingProductionAI = FNEW(CvBuildingProductionAI(pCity, pCity->GetCityBuildings()), c_eCiv5GameplayDLL, 0);
	m_pUnitProductionAI = FNEW(CvUnitProductionAI(pCity, GC.GetGameUnits()), c_eCiv5GameplayDLL, 0);
	m_pProjectProductionAI = FNEW(CvProjectProductionAI(pCity), c_eCiv5GameplayDLL, 0);
	m_pProcessProductionAI = FNEW(CvProcessProductionAI(pCity), c_eCiv5GameplayDLL, 0);

	// Clear variables
	Reset();
}

/// Deallocate memory created in initialize
void CvCityStrategyAI::Uninit()
{
	// Uninit base class
	CvFlavorRecipient::Uninit();

	// Deallocate member variables
	SAFE_DELETE_ARRAY(m_pabUsingCityStrategy);
	SAFE_DELETE_ARRAY(m_paiTurnCityStrategyAdopted);
	SAFE_DELETE_ARRAY(m_aiTempFlavors);
	SAFE_DELETE(m_pBuildingProductionAI);
	SAFE_DELETE(m_pUnitProductionAI);
	SAFE_DELETE(m_pProjectProductionAI);
	SAFE_DELETE(m_pProcessProductionAI);
}

/// Reset member variables
void CvCityStrategyAI::Reset()
{
	int iI;

	for(iI = 0; iI < m_pAICityStrategies->GetNumAICityStrategies(); iI++)
	{
		m_pabUsingCityStrategy[iI] = false;
		m_paiTurnCityStrategyAdopted[iI] = -1;
	}

	m_eSpecialization = NO_CITY_SPECIALIZATION;
	m_eDefaultSpecialization = NO_CITY_SPECIALIZATION;
	m_eFocusYield = (YieldTypes)NO_YIELD;

	// Reset sub AI objects
	m_pBuildingProductionAI->Reset();
	m_pUnitProductionAI->Reset();
	m_pProjectProductionAI->Reset();
	m_pProcessProductionAI->Reset();
}

/// Serialization read
void CvCityStrategyAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	if(uiVersion >= 3)
	{
		int iNumFlavors;
		kStream >> iNumFlavors;
		ArrayWrapper<int> wrapLatestFlavor(iNumFlavors, m_piLatestFlavorValues);
		kStream >> wrapLatestFlavor;
	}
	else
	{
		const int iNumFlavorsVersion2 = 27;
		ArrayWrapper<int> wrapLatestFlavor(iNumFlavorsVersion2, m_piLatestFlavorValues);
		kStream >> wrapLatestFlavor;
	}

	if(uiVersion >= 2)
	{
		int iNumStrategies;
		kStream >> iNumStrategies;
		ArrayWrapper<bool> wrapUsingCity(iNumStrategies, m_pabUsingCityStrategy);
		kStream >> wrapUsingCity;
		ArrayWrapper<int> wrapTurnCity(iNumStrategies, m_paiTurnCityStrategyAdopted);
		kStream >> wrapTurnCity;
	}

	else
	{
		const int iNumCityStrategiesVersion1 = 17;
		ArrayWrapper<bool> wrapUsingCity(iNumCityStrategiesVersion1, m_pabUsingCityStrategy);
		kStream >> wrapUsingCity;
		ArrayWrapper<int> wrapTurnCity(iNumCityStrategiesVersion1, m_paiTurnCityStrategyAdopted);
		kStream >> wrapTurnCity;

		for(int iI = iNumCityStrategiesVersion1; iI < m_pAICityStrategies->GetNumAICityStrategies(); iI++)
		{
			m_pabUsingCityStrategy[iI] = false;
			m_paiTurnCityStrategyAdopted[iI] = -1;
		}
	}

	kStream >> m_eSpecialization;
	kStream >> m_eDefaultSpecialization;
	m_eFocusYield = (YieldTypes)NO_YIELD;	//force yield to default since we don't serialize it.

	m_pBuildingProductionAI->Read(kStream);
	m_pUnitProductionAI->Read(kStream);
	m_pProjectProductionAI->Read(kStream);
	if (uiVersion >= 4)
	{
		m_pProcessProductionAI->Read(kStream);
	}
}

/// Serialization write
void CvCityStrategyAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 4;
	kStream << uiVersion;

	CvAssertMsg(GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");
	int iNumFlavors = GC.getNumFlavorTypes();
	kStream << iNumFlavors;
	kStream << ArrayWrapper<int>(iNumFlavors, m_piLatestFlavorValues);

	int iNumStrategies = GC.getNumAICityStrategyInfos();
	CvAssertMsg(iNumStrategies > 0, "Number of AICityStrategies to serialize is expected to greater than 0");
	kStream << iNumStrategies;
	kStream << ArrayWrapper<bool>(iNumStrategies, m_pabUsingCityStrategy);
	kStream << ArrayWrapper<int>(iNumStrategies, m_paiTurnCityStrategyAdopted);

	kStream << m_eSpecialization;
	kStream << m_eDefaultSpecialization;

	m_pBuildingProductionAI->Write(kStream);
	m_pUnitProductionAI->Write(kStream);
	m_pProjectProductionAI->Write(kStream);
	m_pProcessProductionAI->Write(kStream);
}

/// Respond to a new set of flavor values
void CvCityStrategyAI::FlavorUpdate()
{
	// Reset our sub AI objects
	m_pBuildingProductionAI->Reset();
	m_pUnitProductionAI->Reset();
	m_pProjectProductionAI->Reset();
	m_pProcessProductionAI->Reset();

	// Broadcast to our sub AI objects
	for(int iFlavor = 0; iFlavor < GC.getNumFlavorTypes(); iFlavor++)
	{
		int iFlavorValue = GetLatestFlavorValue((FlavorTypes)iFlavor);// m_piLatestFlavorValues[iFlavor];

		m_pBuildingProductionAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
		m_pUnitProductionAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
		m_pProjectProductionAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
		m_pProcessProductionAI->AddFlavorWeights((FlavorTypes)iFlavor, iFlavorValue);
	}
}

/// Runs through all active player strategies and propagates Flavors down to this City
void CvCityStrategyAI::UpdateFlavorsForNewCity()
{
	int iFlavorLoop;

	// Clear out Temp array
	for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		m_aiTempFlavors[iFlavorLoop] = 0;
	}

	// Go through all Player strategies and for the active ones apply the Flavors
	for(int iStrategyLoop = 0; iStrategyLoop < GC.getNumEconomicAIStrategyInfos(); iStrategyLoop++)
	{
		EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) iStrategyLoop;
		CvEconomicAIStrategyXMLEntry* pStrategy = GC.getEconomicAIStrategyInfo(eStrategy);

		if(pStrategy)
		{
			// Active?
			if(GET_PLAYER(m_pCity->getOwner()).GetEconomicAI()->IsUsingStrategy(eStrategy))
			{
				for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
				{
					m_aiTempFlavors[iFlavorLoop] += pStrategy->GetCityFlavorValue(iFlavorLoop);
				}
			}
		}
	}
	for(int iStrategyLoop = 0; iStrategyLoop < GC.getNumMilitaryAIStrategyInfos(); iStrategyLoop++)
	{
		MilitaryAIStrategyTypes eStrategy = (MilitaryAIStrategyTypes) iStrategyLoop;
		CvMilitaryAIStrategyXMLEntry* pStrategy = GC.getMilitaryAIStrategyInfo(eStrategy);

		if(pStrategy)
		{
			// Active?
			if(GET_PLAYER(m_pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eStrategy))
			{
				for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
				{
					m_aiTempFlavors[iFlavorLoop] += pStrategy->GetCityFlavorValue(iFlavorLoop);
				}
			}
		}
	}

	ChangeFlavors(m_aiTempFlavors, true);

	LogFlavors();
}

/// Set special production emphasis for this city
bool CvCityStrategyAI::SetSpecialization(CitySpecializationTypes eSpecialization)
{
	if(eSpecialization != m_eSpecialization)
	{
		LogSpecializationChange(eSpecialization);

		// Turn off old specialization
		SpecializationFlavorChange(false /*Don't turn on */, m_eSpecialization);

		// Turn on new specialization
		SpecializationFlavorChange(true /* Do turn on */, eSpecialization);

		m_eSpecialization = eSpecialization;

		return true;
	}

	return false;
}

/// Set preferred production emphasis for this city
bool CvCityStrategyAI::SetDefaultSpecialization(CitySpecializationTypes eDefaultSpecialization)
{
	if(eDefaultSpecialization != m_eDefaultSpecialization)
	{
		m_eDefaultSpecialization = eDefaultSpecialization;
		return true;
	}

	return false;
}

/// Turn on or off the flavors for a city specialization
void CvCityStrategyAI::SpecializationFlavorChange(bool bTurnOn, CitySpecializationTypes eSpecialization)
{
	if(eSpecialization != NO_CITY_SPECIALIZATION)
	{
		CvCitySpecializationXMLEntry* pSpecialization = GC.getCitySpecializationInfo(eSpecialization);
		if(pSpecialization)
		{
			int iFlavorLoop;

			// Clear out Temp array
			for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
			{
				m_aiTempFlavors[iFlavorLoop] = 0;
			}

			for(iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
			{
				if(bTurnOn)
				{
					m_aiTempFlavors[iFlavorLoop] += pSpecialization->GetFlavorValue(iFlavorLoop);
				}
				else
				{
					m_aiTempFlavors[iFlavorLoop] -= pSpecialization->GetFlavorValue(iFlavorLoop);
				}
			}

			ChangeFlavors(m_aiTempFlavors, true);

			LogFlavors();
		}
	}
}

// ACCESSOR FUNCTIONS

/// Returns the City object the Strategies are associated with
CvCity* CvCityStrategyAI::GetCity()
{
	return m_pCity;
}

/// Returns AICityStrategies object stored in this class
CvAICityStrategies* CvCityStrategyAI::GetAICityStrategies()
{
	return m_pAICityStrategies;
}

/// Returns whether or not a player has adopted this CityStrategy
bool CvCityStrategyAI::IsUsingCityStrategy(AICityStrategyTypes eStrategy)
{
	return m_pabUsingCityStrategy[(int) eStrategy];
}

/// Signifies that eStrategy is now being utilized by this AI Player
void CvCityStrategyAI::SetUsingCityStrategy(AICityStrategyTypes eStrategy, bool bValue)
{
	if(m_pabUsingCityStrategy[eStrategy] != bValue)
	{
		m_pabUsingCityStrategy[eStrategy] = bValue;

		if(bValue)
		{
			SetTurnCityStrategyAdopted(eStrategy, GC.getGame().getGameTurn());
		}
		else
		{
			SetTurnCityStrategyAdopted(eStrategy, -1);
		}

		LogStrategy(eStrategy, bValue);
	}
}

/// Returns the turn on which a CityStrategy was adopted (-1 if it hasn't been)
int CvCityStrategyAI::GetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy)
{
	return m_paiTurnCityStrategyAdopted[(int) eStrategy];
}

/// Sets the turn number eStrategy was most recently adopted
void CvCityStrategyAI::SetTurnCityStrategyAdopted(AICityStrategyTypes eStrategy, int iValue)
{
	if(m_paiTurnCityStrategyAdopted[(int) eStrategy] != iValue)
	{
		m_paiTurnCityStrategyAdopted[(int) eStrategy] = iValue;
	}
}

/// Get the sub-object tracking building production
CvBuildingProductionAI* CvCityStrategyAI::GetBuildingProductionAI()
{
	return m_pBuildingProductionAI;
}

/// Get the sub-object tracking unit production
CvUnitProductionAI* CvCityStrategyAI::GetUnitProductionAI()
{
	return m_pUnitProductionAI;
}

/// Get the sub-object tracking project production
CvProjectProductionAI* CvCityStrategyAI::GetProjectProductionAI()
{
	return m_pProjectProductionAI;
}

CvProcessProductionAI* CvCityStrategyAI::GetProcessProductionAI()
{
	return m_pProcessProductionAI;
}

/// Build log filename
CvString CvCityStrategyAI::GetLogFileName(CvString& playerName, CvString& cityName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "CityStrategyAILog_" + playerName + "_" + cityName + ".csv";
	}
	else
	{
		strLogName = "CityStrategyAILog.csv";
	}

	return strLogName;
}

//Helper functions to round
static double citystrategyround(double x)
{
	return (x >= 0) ? floor(x + .5) : ceil(x - .5);
};

/// Determines if the yield is below a sustainable amount
bool CvCityStrategyAI::IsYieldDeficient(YieldTypes eYieldType)
{
	double fDesiredYield = GetDeficientYieldValue(eYieldType);
	double fYieldAverage = GetYieldAverage(eYieldType);

	int iDesiredYield = (int)citystrategyround(fDesiredYield * 100);
	int iYieldAverage = (int)citystrategyround(fYieldAverage * 100);

	if(iYieldAverage < iDesiredYield)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Determines what yield type is in a deficient state. If none, then NO_YIELD is returned
YieldTypes CvCityStrategyAI::GetDeficientYield(void)
{
	if(IsYieldDeficient(YIELD_FOOD))
	{
		return YIELD_FOOD;
	}
	else if(IsYieldDeficient(YIELD_PRODUCTION))
	{
		return YIELD_PRODUCTION;
	}
	else
	{
		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			YieldTypes yield = (YieldTypes)ui;
			if(IsYieldDeficient(yield))
			{
				return yield;
			}
		}
	}

	return NO_YIELD;
}

/// Get the average value of the yield for this city
double CvCityStrategyAI::GetYieldAverage(YieldTypes eYieldType)
{
	CvPlayer* pPlayer = &GET_PLAYER(m_pCity->getOwner());
	CvPlotsVector& aiPlots = pPlayer->GetPlots();

	int iTilesWorked = 0;
	int iYieldAmount = 0;
	for(uint ui = 0; ui < aiPlots.size(); ui++)
	{
		// at the end of the plot list
		if(aiPlots[ui] == -1)
		{
			break;
		}

		CvPlot* pPlot = GC.getMap().plotByIndex(aiPlots[ui]);
		if(!m_pCity->GetCityCitizens()->IsWorkingPlot(pPlot))
		{
			continue;
		}

		iTilesWorked++;
		iYieldAmount += pPlot->calculateYield(eYieldType);
	}

	double fRatio = 0.0;
	if(iTilesWorked > 0)
	{
		fRatio = iYieldAmount / (double)iTilesWorked;
	}

	return fRatio;
}

/// Get the deficient value of the yield for this city
double CvCityStrategyAI::GetDeficientYieldValue(YieldTypes eYieldType)
{
	double fDesiredYield = -999.0;

	switch(eYieldType)
	{
	case YIELD_FOOD:
		fDesiredYield = GC.getAI_CITYSTRATEGY_YIELD_DEFICIENT_FOOD();
		break;
	case YIELD_PRODUCTION:
		fDesiredYield = GC.getAI_CITYSTRATEGY_YIELD_DEFICIENT_PRODUCTION();
		break;
	case YIELD_SCIENCE:
		fDesiredYield = GC.getAI_CITYSTRATEGY_YIELD_DEFICIENT_SCIENCE();
		break;
	case YIELD_GOLD:
		fDesiredYield = GC.getAI_CITYSTRATEGY_YIELD_DEFICIENT_GOLD();
		break;
		// OK if deficient in the newer (bonus) yields
	case YIELD_CULTURE:
		break;
	case YIELD_FAITH:
		break;
	default:
		FAssertMsg(false, "Yield type is not handled. What?");
		return false;
	}

	return fDesiredYield;
}

/// Pick the next build for a city (unit, building or wonder)
void CvCityStrategyAI::ChooseProduction(bool bUseAsyncRandom, BuildingTypes eIgnoreBldg /* = NO_BUILDING */, UnitTypes eIgnoreUnit/*  = NO_UNIT */)
{
	RandomNumberDelegate fcn;
	int iBldgLoop, iUnitLoop, iProjectLoop, iProcessLoop, iTempWeight;
	CvCityBuildable buildable;
	CvCityBuildable selection;
	UnitTypes eUnitForOperation;
	UnitTypes eUnitForArmy;

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());
	CvDiplomacyAI* pDiploAI = kPlayer.GetDiplomacyAI();

	//int iSettlersOnMap = kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);

	// Use the asynchronous random number generate if "no random" is set
	if(bUseAsyncRandom)
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getAsyncRandNum);
	}
	else
	{
		fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);
	}

	// Reset vector holding items we can currently build
	m_Buildables.clear();

	EconomicAIStrategyTypes eStrategyEnoughSettlers = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_ENOUGH_EXPANSION");
	bool bEnoughSettlers = kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyEnoughSettlers);

	// Check units for operations first
	eUnitForOperation = m_pCity->GetUnitForOperation();
	if(eUnitForOperation != NO_UNIT)
	{
		buildable.m_eBuildableType = CITY_BUILDABLE_UNIT_FOR_OPERATION;
		buildable.m_iIndex = (int)eUnitForOperation;
		buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eUnitForOperation, 0);
		iTempWeight = GC.getAI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT();
		int iOffenseFlavor = kPlayer.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) + kPlayer.GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver();
		iTempWeight += (GC.getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER() * iOffenseFlavor);

		if(GetSpecialization() != NO_CITY_SPECIALIZATION && GC.getCitySpecializationInfo(GetSpecialization())->IsOperationUnitProvider())
		{
			iTempWeight *= 5;
		}

		// add in the weight of this unit as if I were deciding to build it without having a reason
		iTempWeight += m_pUnitProductionAI->GetWeight(eUnitForOperation);

		CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnitForOperation);
		if(pkUnitEntry && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SETTLE)
		{
			if(bEnoughSettlers)
			{
				iTempWeight = 0;
			}
			else
			{
				int iBestArea, iSecondBestArea;
				int iNumGoodAreas = kPlayer.GetBestSettleAreas(kPlayer.GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
				if(iNumGoodAreas == 0)
				{
					iTempWeight = 0;
				}
			}
		}

		if (iTempWeight > 0)
		{
			m_Buildables.push_back(buildable, iTempWeight);
			kPlayer.GetMilitaryAI()->BumpNumberOfTimesOpsBuildSkippedOver();
		}

	}

	// Next units for sneak attack armies
	eUnitForArmy = kPlayer.GetMilitaryAI()->GetUnitForArmy(GetCity());
	if(eUnitForArmy != NO_UNIT)
	{
		buildable.m_eBuildableType = CITY_BUILDABLE_UNIT_FOR_ARMY;
		buildable.m_iIndex = (int)eUnitForArmy;
		buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eUnitForArmy, 0);
		iTempWeight = GC.getAI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT();
		int iOffenseFlavor = kPlayer.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
		int iBonusMultiplier = max(1,GC.getGame().getHandicapInfo().GetID() - 5); // more at the higher difficulties
		iTempWeight += (GC.getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER() * iOffenseFlavor * iBonusMultiplier);
		// add in the weight of this unit as if I were deciding to build it without having a reason
		iTempWeight += m_pUnitProductionAI->GetWeight(eUnitForArmy);

		if (iTempWeight > 0)
		{
			m_Buildables.push_back(buildable, iTempWeight);
		}
	}

	// Loop through adding the available buildings
	for(iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
	{
		const BuildingTypes eLoopBuilding = static_cast<BuildingTypes>(iBldgLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eLoopBuilding);

		//Skip if null
		if(pkBuildingInfo == NULL)
			continue;

		// Make sure this building can be built now
		if(iBldgLoop != eIgnoreBldg && m_pCity->canConstruct(eLoopBuilding))
		{
			buildable.m_eBuildableType = CITY_BUILDABLE_BUILDING;
			buildable.m_iIndex = iBldgLoop;
			buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eLoopBuilding, 0);

			iTempWeight = m_pBuildingProductionAI->GetWeight(eLoopBuilding);

			// Don't build the UN if you aren't going for the diplo victory
			if(pkBuildingInfo->IsDiplomaticVoting())
			{
				int iVotesNeededToWin = GC.getGame().GetVotesNeededForDiploVictory();
				int iSecuredVotes = 0;
				TeamTypes myTeamID = kPlayer.getTeam();
				PlayerTypes myPlayerID = kPlayer.GetID();

				// Loop through Players to see if they'll vote for this player
				PlayerTypes eLoopPlayer;
				TeamTypes eLoopTeam;
				for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
				{
					eLoopPlayer = (PlayerTypes) iPlayerLoop;

					if(GET_PLAYER(eLoopPlayer).isAlive())
					{
						eLoopTeam = GET_PLAYER(eLoopPlayer).getTeam();

						// Liberated?
						if(GET_TEAM(eLoopTeam).GetLiberatedByTeam() == myTeamID)
						{
							iSecuredVotes++;
						}

						// Minor civ?
						else if(GET_PLAYER(eLoopPlayer).isMinorCiv())
						{
							// Best Relations?
							if(GET_PLAYER(eLoopPlayer).GetMinorCivAI()->GetAlly() == myPlayerID)
							{
								iSecuredVotes++;
							}
						}
					}
				}

				int iNumberOfPlayersWeNeedToBuyOff = MAX(0, iVotesNeededToWin - iSecuredVotes);

				if(!pDiploAI  || !pDiploAI->IsGoingForDiploVictory() || kPlayer.GetTreasury()->GetGold() < iNumberOfPlayersWeNeedToBuyOff * 500)
				{
					iTempWeight = 0;
				}
			}

			// If the City is a puppet, it avoids Wonders (because the human can't change it if he wants to build it somewhere else!)
			if(GetCity()->IsPuppet())
			{
				const CvBuildingClassInfo& kBuildingClassInfo = pkBuildingInfo->GetBuildingClassInfo();

				if(isWorldWonderClass(kBuildingClassInfo) || isTeamWonderClass(kBuildingClassInfo) || isNationalWonderClass(kBuildingClassInfo) || isLimitedWonderClass(kBuildingClassInfo))
				{
					iTempWeight = 0;
				}
				// it also avoids military training buildings - since it can't build units
				if(pkBuildingInfo->GetDomainFreeExperience(DOMAIN_LAND))
				{
					iTempWeight = 0;
				}
				// they also like stuff that won't burden the empire with maintenance costs
				if(pkBuildingInfo->GetGoldMaintenance() == 0)
				{
					iTempWeight *= 2;
				}
				// and they avoid any buildings that require resources
				int iNumResources = GC.getNumResourceInfos();
				for(int iResourceLoop = 0; iResourceLoop < iNumResources; iResourceLoop++)
				{
					if(pkBuildingInfo->GetResourceQuantityRequirement(iResourceLoop) > 0)
					{
						iTempWeight = 0;
					}
				}
			}
			if(iTempWeight > 0)
				m_Buildables.push_back(buildable, iTempWeight);
		}
	}

	// If the City is a puppet, it avoids training Units and projects
	if(!GetCity()->IsPuppet())
	{
		// Loop through adding the available units
		for(iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
		{
			// Make sure this unit can be built now
			if(iUnitLoop != eIgnoreUnit &&
			        //GC.GetGameBuildings()->GetEntry(iUnitLoop)->GetAdvisorType() != eIgnoreAdvisor &&
			        m_pCity->canTrain((UnitTypes)iUnitLoop))
			{
				buildable.m_eBuildableType = CITY_BUILDABLE_UNIT;
				buildable.m_iIndex = iUnitLoop;
				buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft((UnitTypes)iUnitLoop, 0);

				iTempWeight = m_pUnitProductionAI->GetWeight((UnitTypes)iUnitLoop);

				CvUnitEntry* pkUnitEntry = GC.getUnitInfo((UnitTypes)iUnitLoop);
				if(pkUnitEntry && pkUnitEntry->GetDefaultUnitAIType() == UNITAI_SETTLE)
				{
					if(bEnoughSettlers)
					{
						iTempWeight = 0;
					}
					else
					{
						int iBestArea, iSecondBestArea;
						int iNumGoodAreas = kPlayer.GetBestSettleAreas(kPlayer.GetEconomicAI()->GetMinimumSettleFertility(), iBestArea, iSecondBestArea);
						if(iNumGoodAreas == 0)
						{
							iTempWeight = 0;
						}
					}
				}

				// sanity check for building ships on small inland seas (not lakes)
				if (pkUnitEntry)
				{
					DomainTypes eDomain = (DomainTypes) pkUnitEntry->GetDomainType();
					if (eDomain == DOMAIN_SEA && pkUnitEntry->GetDefaultUnitAIType() != UNITAI_WORKER_SEA) // if needed allow workboats...
					{
						CvArea* pBiggestNearbyBodyOfWater = m_pCity->waterArea();
						if (pBiggestNearbyBodyOfWater)
						{
							int iWaterTiles = pBiggestNearbyBodyOfWater->getNumTiles();
							int iNumUnitsofMine = pBiggestNearbyBodyOfWater->getUnitsPerPlayer(m_pCity->getOwner());
							if (iNumUnitsofMine * 5 > iWaterTiles)
							{
								iTempWeight = 0;
							}
						}
						else // this should never happen, but...
						{
							iTempWeight = 0;
						}
					}
				}


				if(iTempWeight > 0)
					m_Buildables.push_back(buildable, iTempWeight);
			}
		}

		// Loop through adding the available projects
		for(iProjectLoop = 0; iProjectLoop < GC.GetGameProjects()->GetNumProjects(); iProjectLoop++)
		{
			if(m_pCity->canCreate((ProjectTypes)iProjectLoop))
			{
				buildable.m_eBuildableType = CITY_BUILDABLE_PROJECT;
				buildable.m_iIndex = iProjectLoop;
				buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft((ProjectTypes)iProjectLoop, 0);
				m_Buildables.push_back(buildable, m_pProjectProductionAI->GetWeight((ProjectTypes)iProjectLoop));
			}
		}

		// Loop through adding available processes
		if (!GET_PLAYER(m_pCity->getOwner()).isMinorCiv())
		{
			//I cannot use the yield rate since it adds in set process yield, which is what I am trying to set...
			int iBaseYield = GetCity()->getBaseYieldRate(YIELD_PRODUCTION) * 100;
			iBaseYield += (GetCity()->GetYieldPerPopTimes100(YIELD_PRODUCTION) * GetCity()->getPopulation());
			int iModifiedYield = iBaseYield * GetCity()->getBaseYieldRateModifier(YIELD_PRODUCTION);
			iModifiedYield /= 100;

			if (iModifiedYield >= 8)
			{
				for (iProcessLoop = 0; iProcessLoop < GC.getNumProcessInfos(); iProcessLoop++)
				{
					ProcessTypes eProcess = (ProcessTypes)iProcessLoop;
					if (m_pCity->canMaintain(eProcess))
					{
						iTempWeight = m_pProcessProductionAI->GetWeight((ProcessTypes)iProcessLoop);
						buildable.m_eBuildableType = CITY_BUILDABLE_PROCESS;
						buildable.m_iIndex = iProcessLoop;
						buildable.m_iTurnsToConstruct = 1;
						m_Buildables.push_back(buildable, iTempWeight);
					}
				}
			}
		}
	}


	ReweightByCost();

	m_Buildables.SortItems();

	LogPossibleBuilds();

	if(m_Buildables.GetTotalWeight() > 0)
	{
		// Choose from the best options (currently 2)
		int iNumChoices = GC.getGame().getHandicapInfo().GetCityProductionNumOptions();
		selection = m_Buildables.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing city build from Top Choices");
		int iRushIfMoreThanXTurns = GC.getAI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD();
		if(GET_PLAYER(m_pCity->getOwner()).isMinorCiv())
		{
			iRushIfMoreThanXTurns *= GC.getMINOR_CIV_PRODUCTION_PERCENT();
			iRushIfMoreThanXTurns /= 100;
		}

		bool bRush = selection.m_iTurnsToConstruct > iRushIfMoreThanXTurns;
		LogCityProduction(selection, bRush);

		switch(selection.m_eBuildableType)
		{
		case CITY_BUILDABLE_UNIT:
		case CITY_BUILDABLE_UNIT_FOR_ARMY:
		{
			UnitTypes eUnitType = (UnitTypes) selection.m_iIndex;
			CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitType);
			if(pkUnitInfo)
			{
				UnitAITypes eUnitAI = (UnitAITypes) pkUnitInfo->GetDefaultUnitAIType();
				GetCity()->pushOrder(ORDER_TRAIN, eUnitType, eUnitAI, false, true, false, bRush);
			}
		}
		break;

		case CITY_BUILDABLE_BUILDING:
		{
			BuildingTypes eBuildingType = (BuildingTypes) selection.m_iIndex;
			GetCity()->pushOrder(ORDER_CONSTRUCT, eBuildingType, -1, false, true, false, bRush);
		}
		break;

		case CITY_BUILDABLE_PROJECT:
		{
			ProjectTypes eProjectType = (ProjectTypes) selection.m_iIndex;
			GetCity()->pushOrder(ORDER_CREATE, eProjectType, -1, false, true, false, bRush);
		}
		break;

		case CITY_BUILDABLE_PROCESS:
		{
			ProcessTypes eProcessType = (ProcessTypes)selection.m_iIndex;
			GetCity()->pushOrder(ORDER_MAINTAIN, eProcessType, -1, false, true, false, false); // ignoring rush because we can't rush a process
		}

		case CITY_BUILDABLE_UNIT_FOR_OPERATION:
		{
			GetCity()->CommitToBuildingUnitForOperation();
			kPlayer.GetMilitaryAI()->ResetNumberOfTimesOpsBuildSkippedOver();
		}
		break;
		}

	}

	return;
}

/// Called every turn to see what CityStrategies this City should using (or not)
void CvCityStrategyAI::DoTurn()
{
	int iCityStrategiesLoop = 0;

	// Loop through all CityStrategies
	for(iCityStrategiesLoop = 0; iCityStrategiesLoop < GetAICityStrategies()->GetNumAICityStrategies(); iCityStrategiesLoop++)
	{
		AICityStrategyTypes eCityStrategy = (AICityStrategyTypes) iCityStrategiesLoop;
		CvAICityStrategyEntry* pCityStrategy = GetAICityStrategies()->GetEntry(iCityStrategiesLoop);

		// Minor Civs can't run some Strategies
		if(GET_PLAYER(GetCity()->getOwner()).isMinorCiv() && pCityStrategy->IsNoMinorCivs())
		{
			continue;
		}

		bool bTestCityStrategyStart = true;

		// Do we already have this CityStrategy adopted?
		if(IsUsingCityStrategy(eCityStrategy))
		{
			bTestCityStrategyStart = false;
		}
		else
		{
			// Has the prereq Tech necessary?
			if(pCityStrategy->GetTechPrereq() != NO_TECH && !GET_TEAM(GetCity()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pCityStrategy->GetTechPrereq()))
			{
				bTestCityStrategyStart = false;
			}

			// Has the Tech which obsoletes this Strategy?
			if(bTestCityStrategyStart && pCityStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetCity()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pCityStrategy->GetTechObsolete()))
			{
				bTestCityStrategyStart = false;
			}
		}

		bool bTestCityStrategyEnd = false;

		// CityStrategy is active, check to see if we should even try to disable it
		if(IsUsingCityStrategy(eCityStrategy))
		{
			// If Strategy is Permanent we can't ever turn it off
			if(!pCityStrategy->IsPermanent())
			{
				if(pCityStrategy->GetCheckTriggerTurnCount() > 0)
				{
					// Is it a turn where we want to check to see if this Strategy is maintained?
					if((GC.getGame().getGameTurn() - GetTurnCityStrategyAdopted(eCityStrategy)) % pCityStrategy->GetCheckTriggerTurnCount() == 0)
					{
						bTestCityStrategyEnd = true;
					}
				}

				if(bTestCityStrategyEnd && pCityStrategy->GetMinimumNumTurnsExecuted() > 0)
				{
					// Has the minimum # of turns passed for this Strategy?
					if(GC.getGame().getGameTurn() < GetTurnCityStrategyAdopted(eCityStrategy) + pCityStrategy->GetMinimumNumTurnsExecuted())
					{
						bTestCityStrategyEnd = false;
					}
				}
			}
		}

		// Check CityStrategy Triggers
		// Functionality and existence of specific CityStrategies is hardcoded here, but data is stored in XML so it's easier to modify

		if(bTestCityStrategyStart || bTestCityStrategyEnd)
		{
			bool bStrategyShouldBeActive = false;

			// Has the Tech which obsoletes this Strategy? If so, Strategy should be deactivated regardless of other factors
			if(pCityStrategy->GetTechObsolete() != NO_TECH && GET_TEAM(GetCity()->getTeam()).GetTeamTechs()->HasTech((TechTypes) pCityStrategy->GetTechObsolete()))
			{
				bStrategyShouldBeActive = false;
			}
			// Strategy isn't obsolete, so test triggers as normal
			else
			{
				CvString strStrategyName = (CvString) pCityStrategy->GetType();

				// Check all of the CityStrategy Triggers
				if(strStrategyName == "AICITYSTRATEGY_TINY_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_TinyCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_SMALL_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_SmallCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_MEDIUM_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_MediumCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_LARGE_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_LargeCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_LANDLOCKED")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_Landlocked(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_TILE_IMPROVERS")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedTileImprovers(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_WANT_TILE_IMPROVERS")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_WantTileImprovers(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_ENOUGH_TILE_IMPROVERS")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_EnoughTileImprovers(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_NAVAL_GROWTH")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedNavalGrowth(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedNavalTileImprovement(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_ENOUGH_NAVAL_TILE_IMPROVEMENT")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_EnoughNavalTileImprovement(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_IMPROVEMENT_FOOD")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedImprovement(GetCity(), YIELD_FOOD);
				else if(strStrategyName == "AICITYSTRATEGY_NEED_IMPROVEMENT_PRODUCTION")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedImprovement(GetCity(), YIELD_PRODUCTION);
				else if(strStrategyName == "AICITYSTRATEGY_HAVE_TRAINING_FACILITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_HaveTrainingFacility(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_CAPITAL_NEED_SETTLER")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_CapitalNeedSettler(eCityStrategy, GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_CAPITAL_UNDER_THREAT")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_CapitalUnderThreat(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_CULTURE_BUILDING_EMERGENCY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstCultureBuildingEmergency(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_CULTURE_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstCultureBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_SCIENCE_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstScienceBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_GOLD_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstGoldBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_FAITH_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstFaithBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FIRST_PRODUCTION_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_FirstProductionBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_UNDER_BLOCKADE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_UnderBlockade(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_IS_PUPPET")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_IsPuppet(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_MEDIUM_CITY_HIGH_DIFFICULTY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_MediumCityHighDifficulty(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_ORIGINAL_CAPITAL")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_OriginalCapital(GetCity());

				else if(strStrategyName == "AICITYSTRATEGY_RIVER_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_RiverCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_MOUNTAIN_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_MountainCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_FOREST_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_ForestCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_HILL_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_HillCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_JUNGLE_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_JungleCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_COAST_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_CoastCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_MANY_TECHS_STOLEN")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_ManyTechsStolen(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_KEY_SCIENCE_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_KeyScienceCity(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_GOOD_GP_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_GoodGPCity(GetCity());
			}

			// This variable keeps track of whether or not we should be doing something (i.e. Strategy is active now but should be turned off, OR Strategy is inactive and should be enabled)
			bool bAdoptOrEndStrategy = false;

			// Strategy should be on, and if it's not, turn it on
			if(bStrategyShouldBeActive)
			{
				if(bTestCityStrategyStart)
				{
					bAdoptOrEndStrategy = true;
				}
				else if(bTestCityStrategyEnd)
				{
					bAdoptOrEndStrategy = false;
				}
			}
			// Strategy should be off, and if it's not, turn it off
			else
			{
				if(bTestCityStrategyStart)
				{
					bAdoptOrEndStrategy = false;
				}
				else if(bTestCityStrategyEnd)
				{
					bAdoptOrEndStrategy = true;
				}
			}

			// Flavor propagation
			if(bAdoptOrEndStrategy)
			{
				int iFlavorLoop;

				// We should adopt this CityStrategy
				if(bTestCityStrategyStart)
				{
					SetUsingCityStrategy(eCityStrategy, true);

					const int iFlavorMinValue = /*-1000*/ GC.getFLAVOR_MIN_VALUE();
					const int iFlavorMaxValue = /*1000*/ GC.getFLAVOR_MAX_VALUE();

					const int iNumFlavors = GC.getNumFlavorTypes();
					for(iFlavorLoop = 0; iFlavorLoop < iNumFlavors; iFlavorLoop++)
					{
						if(pCityStrategy->GetFlavorValue(iFlavorLoop) != 0)
						{
							m_piLatestFlavorValues[iFlavorLoop] += pCityStrategy->GetFlavorValue(iFlavorLoop);

							if(m_piLatestFlavorValues[iFlavorLoop] < iFlavorMinValue)
							{
								m_piLatestFlavorValues[iFlavorLoop] = iFlavorMinValue;
							}
							else if(m_piLatestFlavorValues[iFlavorLoop] > iFlavorMaxValue)
							{
								m_piLatestFlavorValues[iFlavorLoop] = iFlavorMaxValue;
							}

							LogFlavors((FlavorTypes) iFlavorLoop);
						}
					}

					FlavorUpdate();
				}
				// End the CityStrategy
				else if(bTestCityStrategyEnd)
				{
					SetUsingCityStrategy(eCityStrategy, false);

					const int iFlavorMinValue = /*-1000*/ GC.getFLAVOR_MIN_VALUE();
					const int iFlavorMaxValue = /*1000*/ GC.getFLAVOR_MAX_VALUE();

					const int iNumFlavors = GC.getNumFlavorTypes();
					for(iFlavorLoop = 0; iFlavorLoop < iNumFlavors; iFlavorLoop++)
					{
						if(pCityStrategy->GetFlavorValue(iFlavorLoop) != 0)
						{
							m_piLatestFlavorValues[iFlavorLoop] -= pCityStrategy->GetFlavorValue(iFlavorLoop);

							if(m_piLatestFlavorValues[iFlavorLoop] < iFlavorMinValue)
							{
								m_piLatestFlavorValues[iFlavorLoop] = iFlavorMinValue;
							}
							else if(m_piLatestFlavorValues[iFlavorLoop] > iFlavorMaxValue)
							{
								m_piLatestFlavorValues[iFlavorLoop] = iFlavorMaxValue;
							}

							LogFlavors((FlavorTypes) iFlavorLoop);
						}
					}

					FlavorUpdate();
				}
			}
		}
	}
}

void CvCityStrategyAI::ResetBestYields()
{
	for(uint uiYields = 0; uiYields < NUM_YIELD_TYPES; uiYields++)
	{
		for(uint uiPlots = 0; uiPlots < NUM_CITY_PLOTS - 1; uiPlots++)
		{
			m_acBestYields[uiYields][uiPlots] = MAX_UNSIGNED_CHAR;
		}

		m_asBestYieldAverageTimes100[uiYields] = MAX_SHORT;
	}
}

void CvCityStrategyAI::UpdateBestYields()
{
	m_eFocusYield = (YieldTypes)NO_YIELD;

	ResetBestYields();

	int iPopulationToEvaluate = min(m_pCity->getPopulation() + 2, NUM_CITY_PLOTS);
	CvPlot* pPlot = NULL;
	uint uiPlotsEvaluated = 0;

	struct ReverseSort
	{
		bool operator()(unsigned char ucA, unsigned char ucB)
		{
			return ucA > ucB;
		}
	};

	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		// we want to evaluate the city plot
		//if (iPlotLoop == CITY_HOME_PLOT)
		//{
		//	continue;
		//}

		pPlot = m_pCity->GetCityCitizens()->GetCityPlotFromIndex(iPlotLoop);
		if(!pPlot)
		{
			continue;
		}

		CvCity* pWorkingCity = pPlot->getWorkingCity();
		if(!pWorkingCity || pWorkingCity->GetID() != m_pCity->GetID())
		{
			continue;
		}

		for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			YieldTypes eYield = (YieldTypes)iYield;
			int iYieldAmount = pPlot->getYield(eYield);

			m_acBestYields[iYield][uiPlotsEvaluated] = iYieldAmount;
		}
		uiPlotsEvaluated++;
	}

	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		std::stable_sort(m_acBestYields[iYield], m_acBestYields[iYield] + uiPlotsEvaluated, ReverseSort());
	}

	int iSlotsToEvaluate = MIN((int)uiPlotsEvaluated, iPopulationToEvaluate);
	CvAssertMsg(iSlotsToEvaluate > 0, "iSlotsToEvaluate is zero, trying to div by 0");
	if(iSlotsToEvaluate <= 0)
	{
		return;
	}

	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		int iYieldSum = 0;
		for(int iSlot = 0; iSlot < iSlotsToEvaluate; iSlot++)
		{
			iYieldSum += (m_acBestYields[iYield][iSlot] * 100);
		}

		// add in additional food from the city plot and the city buildings that provide food
		if(iYield == YIELD_FOOD)
		{
			int iCityYieldSum = 0;
			iCityYieldSum = m_pCity->plot()->getYield(YIELD_FOOD);
			CvYieldInfo* pYield = GC.getYieldInfo(YIELD_FOOD);
			if(pYield)
			{
				iCityYieldSum -= pYield->getMinCity();
			}

			CvCityBuildings* pCityBuildings = m_pCity->GetCityBuildings();
			BuildingTypes eBuilding;
			for(int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
			{
				eBuilding = (BuildingTypes) iI;
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
				if(pkBuildingInfo)
				{
					if(pCityBuildings->GetNumBuilding(eBuilding) > 0)
					{
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield);
					}
				}
			}

			iYieldSum += iCityYieldSum * 100;
		}

		m_asBestYieldAverageTimes100[iYield] = iYieldSum / iSlotsToEvaluate;

		CvAssertMsg(m_asBestYieldAverageTimes100[iYield] < 750, "Crazy high yield");
	}

	CitySpecializationTypes eSpecialization = m_pCity->GetCityStrategyAI()->GetSpecialization();
	if(eSpecialization == NO_CITY_SPECIALIZATION)
	{
		if(m_pCity->GetPlayer()->isHuman())
		{
			// find a specialization type according to the citizen focus type
			CityAIFocusTypes eCityAIFocusTypes = m_pCity->GetCityCitizens()->GetFocusType();

			CvString strLookup;
			switch(eCityAIFocusTypes)
			{
			case CITY_AI_FOCUS_TYPE_FOOD:
				strLookup = "CITYSPECIALIZATION_SETTLER_PUMP";
				break;
			case CITY_AI_FOCUS_TYPE_GOLD:
				strLookup = "CITYSPECIALIZATION_COMMERCE";
				break;
			}

			if(!strLookup.IsEmpty())
			{
				std::vector<CvCitySpecializationXMLEntry*> aCitySpecializations = GC.getCitySpecializationInfo();
				for(uint ui = 0; ui < aCitySpecializations.size(); ui++)
				{
					CvCitySpecializationXMLEntry* pEntry = aCitySpecializations[ui];
					if(pEntry->GetType() == strLookup)
					{
						eSpecialization = (CitySpecializationTypes)ui;
						break;
					}
				}
			}
		}

		// if the human did not have a city ai specialization
		if(eSpecialization == NO_CITY_SPECIALIZATION)
		{
			std::vector<CvCitySpecializationXMLEntry*> m_aCitySpecializations;
			m_aCitySpecializations = GC.getCitySpecializationInfo();
			for(uint ui = 0; ui < m_aCitySpecializations.size(); ui++)
			{
				if(m_aCitySpecializations[ui]->IsDefault())
				{
					eSpecialization = (CitySpecializationTypes)ui;
					break;
				}
			}
		}
	}

	CvCitySpecializationXMLEntry* pEntry = NULL;
	if(eSpecialization != NO_CITY_SPECIALIZATION)
	{
		pEntry = GC.getCitySpecializationInfo(eSpecialization);
	}

	if(!pEntry)
	{
		return;
	}

	for(int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
	{
		m_asYieldDeltaTimes100[iYield] = m_asBestYieldAverageTimes100[iYield] - pEntry->GetYieldTargetTimes100((YieldTypes)iYield);
	}

	m_eFocusYield = pEntry->GetYieldType();
}

unsigned short CvCityStrategyAI::GetBestYieldAverageTimes100(YieldTypes eYield)
{
	return m_asBestYieldAverageTimes100[eYield];
}

short CvCityStrategyAI::GetYieldDeltaTimes100(YieldTypes eYield)
{
	return m_asYieldDeltaTimes100[eYield];
}

YieldTypes CvCityStrategyAI::GetFocusYield()
{
	return m_eFocusYield;
}

void CvCityStrategyAI::LogHurry(HurryTypes iHurryType, int iHurryAmount, int iHurryAmountAvailable, int iTurnsSaved)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		// Open the log file
		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		if(iHurryType == 0)
		{
			strTemp.Format("Pop rushed: %s, Spent: %d, Available: %d, Turns Saved: %d", m_pCity->getProductionName(), iHurryAmount, iHurryAmountAvailable, iTurnsSaved);
		}
		else
		{
			strTemp.Format("Gold rushed: %s, Spent: %d, Available: %d, Turns Saved: %d", m_pCity->getProductionName(), iHurryAmount, iHurryAmountAvailable, iTurnsSaved);
		}

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}


// PRIVATE METHODS

/// Recompute weights taking into account Production cost
void CvCityStrategyAI::ReweightByCost()
{
	CvCityBuildable buildable;

	for(int iI = 0; iI < m_Buildables.size(); iI++)
	{
		buildable = m_Buildables.GetElement(iI);

		// Compute the new weight and change it
		int iNewWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
		m_Buildables.SetWeight(iI, iNewWeight);
	}
}

/// Log new flavor settings
void CvCityStrategyAI::LogFlavors(FlavorTypes eFlavor)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		// Open the log file
		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		// Dump out the setting for each flavor
		if(eFlavor == NO_FLAVOR)
		{
			for(int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
			{
				// Only dump if non-zero
				//		if (m_piLatestFlavorValues[iI] > 0)
				{
					strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes((FlavorTypes)iI).GetCString(), m_piLatestFlavorValues[iI]);
					strOutBuf = strBaseString + strTemp;
					pLog->Msg(strOutBuf);
				}
			}
		}
		else
		{
			strTemp.Format("Flavor, %s, %d", GC.getFlavorTypes(eFlavor).GetCString(), m_piLatestFlavorValues[eFlavor]);
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log a city strategy
void CvCityStrategyAI::LogStrategy(AICityStrategyTypes eStrategy, bool bValue)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;

		// Find the name of this civ and city
		CvString playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		CvString cityName = m_pCity->getName();

		// Open the log file
		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		strOutBuf = strBaseString;

		// Strategy Info
		CvAICityStrategyEntry* pStrategyEntry = GC.getAICityStrategyInfo(eStrategy);
		if(pStrategyEntry != NULL)
		{
			CvString strTemp;
			strTemp.Format("%s, %d", pStrategyEntry->GetType(), bValue);
			strOutBuf += strTemp;
		}

		pLog->Msg(strOutBuf);
	}
}

/// Log all potential builds
void CvCityStrategyAI::LogPossibleBuilds()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		// Open the log file
		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		// Dump out the weight of each buildable item
		for(int iI = 0; iI < m_Buildables.size(); iI++)
		{
			CvCityBuildable buildable = m_Buildables.GetElement(iI);

			switch(buildable.m_eBuildableType)
			{
			case CITY_BUILDABLE_BUILDING:
			{
				CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Building, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Unit, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROJECT:
			{
				CvProjectEntry* pEntry = GC.GetGameProjects()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Project, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROCESS:
			{
				CvProcessInfo* pProcess = GC.getProcessInfo((ProcessTypes)buildable.m_iIndex);
				if (pProcess != NULL)
				{
					strDesc = pProcess->GetDescription();
					strTemp.Format("Process, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_OPERATION:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Operation unit, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_ARMY:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Army unit, %s, %d, %d", strDesc.GetCString(), m_Buildables.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log the chosen item to build
void CvCityStrategyAI::LogCityProduction(CvCityBuildable buildable, bool bRush)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp;
		CvString playerName;
		CvString cityName;
		CvString strDesc;

		// Find the name of this civ and city
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		switch(buildable.m_eBuildableType)
		{
		case CITY_BUILDABLE_BUILDING:
		{
			CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(buildable.m_iIndex);
			if(pEntry != NULL)
			{
				strDesc = pEntry->GetDescription();
			}
		}
		break;
		case CITY_BUILDABLE_UNIT:
		case CITY_BUILDABLE_UNIT_FOR_OPERATION:
		case CITY_BUILDABLE_UNIT_FOR_ARMY:
		{
			CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
			if(pEntry != NULL)
			{
				strDesc = pEntry->GetDescription();

			}
		}
		break;
		case CITY_BUILDABLE_PROJECT:
		{
			CvProjectEntry* pEntry = GC.GetGameProjects()->GetEntry(buildable.m_iIndex);
			if(pEntry != NULL)
			{
				strDesc = pEntry->GetDescription();

			}
		}
		break;
		case CITY_BUILDABLE_PROCESS:
		{
			CvProcessInfo* pEntry = GC.getProcessInfo((ProcessTypes)buildable.m_iIndex);
			if (pEntry != NULL)
			{
				strDesc = pEntry->GetDescription();
			}
		}
		break;
		}

		if(bRush)
		{
			strTemp.Format("SEED: %d, CHOSEN: %s, Rush if possible, TURNS: %d", GC.getGame().getJonRand().getSeed(), strDesc.c_str(), buildable.m_iTurnsToConstruct);
		}
		else
		{
			strTemp.Format("SEED: %d, CHOSEN: %s, Do not rush, TURNS: %d", GC.getGame().getJonRand().getSeed(), strDesc.c_str(), buildable.m_iTurnsToConstruct);
		}

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

void CvCityStrategyAI::LogSpecializationChange(CitySpecializationTypes eSpecialization)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		// Find the name of this civ and city
		CvString playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		CvString cityName = m_pCity->getName();

		FILogFile* pLog = LOGFILEMGR.GetLog(GetLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		CvString strBaseString;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		CvString strOutBuf = strBaseString;

		CvCitySpecializationXMLEntry* pCitySpecializationInfo = GC.getCitySpecializationInfo(eSpecialization);
		if(pCitySpecializationInfo != NULL)
		{
			const YieldTypes eYieldType = pCitySpecializationInfo->GetYieldType();

			CvString strYieldString = "Unknown";
			if(eYieldType == NO_YIELD)
			{
				strYieldString = "General Economy";
			}
			else
			{
				CvYieldInfo* pYieldInfo = GC.getYieldInfo(eYieldType);
				if(pYieldInfo != NULL)
				{
					strYieldString = pYieldInfo->GetDescription();
				}
			}

			CvString strTemp = "NEW SPECIALIZATION: Yield Type = ";
			strOutBuf += strTemp + strYieldString;
		}

		pLog->Msg(strOutBuf);
	}
}

// NON-MEMBER FUNCTIONS
//
// These are functions that do not need access to the internals of the CvCityStrategyAI class.
// Keep them as non-member functions to:
// a) simplify the class (improving encapsulation, reducing coupling)
// b) allow their general use by other classes

/// Routine to reweight a city buildable based on time to build
int CityStrategyAIHelpers::ReweightByTurnsLeft(int iOriginalWeight, int iTurnsLeft)
{
	// 10 turns will add 0.02; 80 turns will add 0.16
	double fAdditionalTurnCostFactor = GC.getAI_PRODUCTION_WEIGHT_MOD_PER_TURN_LEFT() * iTurnsLeft;	// 0.004
	double fTotalCostFactor = GC.getAI_PRODUCTION_WEIGHT_BASE_MOD() + fAdditionalTurnCostFactor;	// 0.15
	double fWeightDivisor = pow((double) iTurnsLeft, fTotalCostFactor);

	/* Commented out for now: useful debug code for tweaking the exact effect of this function

	iTurnsLeft = 10;
	fAdditionalTurnCostFactor = 0.004f * iTurnsLeft;
	fTotalCostFactor = 0.15f + fAdditionalTurnCostFactor;
	fWeightDivisor = pow((double) iTurnsLeft, fTotalCostFactor);

	iTurnsLeft = 20;
	fAdditionalTurnCostFactor = 0.004f * iTurnsLeft;
	fTotalCostFactor = 0.15f + fAdditionalTurnCostFactor;
	fWeightDivisor = pow((double) iTurnsLeft, fTotalCostFactor);

	iTurnsLeft = 40;
	fAdditionalTurnCostFactor = 0.004f * iTurnsLeft;
	fTotalCostFactor = 0.15f + fAdditionalTurnCostFactor;
	fWeightDivisor = pow((double) iTurnsLeft, fTotalCostFactor);

	iTurnsLeft = 80;
	fAdditionalTurnCostFactor = 0.004f * iTurnsLeft;
	fTotalCostFactor = 0.15f + fAdditionalTurnCostFactor;
	fWeightDivisor = pow((double) iTurnsLeft, fTotalCostFactor);*/

	return int(double(iOriginalWeight) / fWeightDivisor);
}

// Figure out what the WeightThreshold Mod should be by looking at the Flavors for this player & the Strategy
int CityStrategyAIHelpers::GetWeightThresholdModifier(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	int iWeightThresholdModifier = 0;

	// Look at all Flavors for the Player & this Strategy
	for(int iFlavorLoop = 0; iFlavorLoop < GC.getNumFlavorTypes(); iFlavorLoop++)
	{
		FlavorTypes eFlavor = (FlavorTypes) iFlavorLoop;
		int iPersonalityFlavor = GET_PLAYER(pCity->getOwner()).GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);
		int iStrategyFlavorMod = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy)->GetPersonalityFlavorThresholdMod(eFlavor);

		iWeightThresholdModifier += (iPersonalityFlavor * iStrategyFlavorMod);
	}

	return iWeightThresholdModifier;
}

/// "Tiny City" City Strategy: If a City is under 2 Population tweak a number of different Flavors
bool CityStrategyAIHelpers::IsTestCityStrategy_TinyCity(CvCity* pCity)
{
	// City Population is too low, don't build Settlers yet
	if(pCity->getPopulation() < GC.getAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD())	// Just size 1
	{
		return true;
	}

	return false;
}

/// "Small City" City Strategy: If a City is under 3 Population tweak a number of different Flavors
bool CityStrategyAIHelpers::IsTestCityStrategy_SmallCity(CvCity* pCity)
{
	// City Population is too low, don't build Settlers yet
	if(pCity->getPopulation() >= GC.getAI_CITYSTRATEGY_SMALL_CITY_POP_THRESHOLD() &&   // 2 to 4
	        pCity->getPopulation() < GC.getAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD())
	{
		return true;
	}

	return false;
}

/// "Medium City" City Strategy: If a City is 8 or above Population boost science
bool CityStrategyAIHelpers::IsTestCityStrategy_MediumCity(CvCity* pCity)
{
	// City Population is getting larger, increase science
	if(pCity->getPopulation() >= GC.getAI_CITYSTRATEGY_MEDIUM_CITY_POP_THRESHOLD() &&   // 5 to 11
	        pCity->getPopulation() < GC.getAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD())
	{
		return true;
	}

	return false;
}

/// "Small City" City Strategy: If a City is 15 or above, boost science a LOT
bool CityStrategyAIHelpers::IsTestCityStrategy_LargeCity(CvCity* pCity)
{
	// City Population is very large, really boost science
	if(pCity->getPopulation() >= GC.getAI_CITYSTRATEGY_LARGE_CITY_POP_THRESHOLD())	// 12+
	{
		return true;
	}

	return false;
}

/// "Landlocked" City Strategy: If a City has no access to the Ocean then nullify all water-based Flavors
bool CityStrategyAIHelpers::IsTestCityStrategy_Landlocked(CvCity* pCity)
{
	// If this City isn't adjacent to a body of water big enough to be "Ocean" then we consider it landlocked
	if(!pCity->isCoastal())
	{
		return true;
	}

	return false;
}

/// "Need Tile Improvers" City Strategy: Do we REALLY need to train some Workers?
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
	int iCurrentNumCities = kPlayer.getNumCities();

	int iLastTurnWorkerDisbanded = kPlayer.GetEconomicAI()->GetLastTurnWorkerDisbanded();
	if(iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 25)
	{
		return false;
	}

	int iNumWorkers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
	// If it's a minor with at least 1 worker per city, always return false
	if(kPlayer.isMinorCiv())
	{
		if(iNumWorkers >= iCurrentNumCities)
			return false;
	}
	else
	{
		int iNumCities = max(1, (iCurrentNumCities * 3) / 4);
		if(iNumWorkers >= iNumCities)
			return false;
		// If we're losing at war, return false
		if(kPlayer.GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
			return false;
	}

	// If we're under attack from Barbs and have 1 or fewer Cities and no credible defense then training more Workers will only hurt us
	if(iCurrentNumCities <= 1)
	{
		CvMilitaryAI* pMilitaryAI =kPlayer.GetMilitaryAI();
		MilitaryAIStrategyTypes eStrategyKillBarbs = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
		if(eStrategyKillBarbs != NO_MILITARYAISTRATEGY)
		{
			if(pMilitaryAI->IsUsingStrategy(eStrategyKillBarbs))
			{
				return false;
			}
		}
	}

	CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);

	int iModdedNumWorkers = iNumWorkers* /*67*/ pCityStrategy->GetWeightThreshold() / 100;
	int iModdedNumCities = iCurrentNumCities + kPlayer.countCitiesFeatureSurrounded();

	// We have fewer than we think we should, or we have none at all
	if(iModdedNumWorkers <= iModdedNumCities || iModdedNumWorkers == 0)
	{
		// If we don't have any Workers by turn 30 we really need to get moving
		int iDesperateTurn = /*30*/ GC.getAI_CITYSTRATEGY_NEED_TILE_IMPROVERS_DESPERATE_TURN();

		iDesperateTurn *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iDesperateTurn /= 100;

		if(GC.getGame().getElapsedGameTurns() > iDesperateTurn)
			return true;
	}

	return false;
}

/// "Want Tile Improvers" City Strategy: Looks at how many Builders are available empire-wide.  This is not a Player Strategy because it's only worried about training new Builders in Cities, and not Techs, Policies, etc.
bool CityStrategyAIHelpers::IsTestCityStrategy_WantTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
	int iLastTurnWorkerDisbanded = kPlayer.GetEconomicAI()->GetLastTurnWorkerDisbanded();
	if(iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 10)
	{
		return false;
	}

	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		// If we're losing at war, return false
		if(GET_PLAYER(pCity->getOwner()).GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
			return false;
	}

	int iNumWorkers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
	if(iNumWorkers >= ((kPlayer.getNumCities() *  3) / 2) + 1)
		return false;

	// If we're under attack from Barbs and have 1 or fewer cities then training more Workers will only hurt us
	//if (kPlayer.getNumCities() <= 1)
	//{
	//	MilitaryAIStrategyTypes eStrategyKillBarbs = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
	//	if (eStrategyKillBarbs != NO_MILITARYAISTRATEGY)
	//	{
	//		if (GET_PLAYER(pCity->getOwner()).GetMilitaryAI()->IsUsingStrategy(eStrategyKillBarbs))
	//			return false;
	//	}
	//}

	// Don't get desperate for training a Builder here unless the City is at least of a certain size
	if(pCity->getPopulation() >= /*2*/ GC.getAI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE())
	{
		int iNumBuilders = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);

		// If we don't even have 1 builder on map or in a queue, turn this on immediately
		if(iNumBuilders < 1)
		{
			return true;
		}

		CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);

		int iWeightThresholdModifier = CityStrategyAIHelpers::GetWeightThresholdModifier(eStrategy, pCity);	// 2 Extra Weight per TILE_IMPROVEMENT Flavor
		int iPerCityThreshold = pCityStrategy->GetWeightThreshold() + iWeightThresholdModifier;	// 40

		// Look at all Tiles this City could potentially work to see if there are any Water Resources that could be improved
		CvPlot* pLoopPlot;
		int iNumResources = 0;
		int iNumImprovedResources = 0;

		for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
		{
			pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iPlotLoop);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getOwner() == pCity->getOwner())
				{
					if(!pLoopPlot->isWater())
					{
						ResourceTypes eResource = pLoopPlot->getResourceType(kPlayer.getTeam());
						if(eResource == NO_RESOURCE)
						{
							continue;
						}

						// loop through the build types to find one that we can use
						ImprovementTypes eCorrectImprovement = NO_IMPROVEMENT;
						BuildTypes eCorrectBuild = NO_BUILD;
						int iBuildIndex;
						for(iBuildIndex = 0; iBuildIndex < GC.getNumBuildInfos(); iBuildIndex++)
						{
							const BuildTypes eBuild = static_cast<BuildTypes>(iBuildIndex);
							CvBuildInfo* pkBuildInfo = GC.getBuildInfo(eBuild);
							if(pkBuildInfo)
							{
								const ImprovementTypes eImprovement = (ImprovementTypes)pkBuildInfo->getImprovement();

								// if this is the improvement we're looking for
								if(eImprovement != NO_IMPROVEMENT)
								{
									CvImprovementEntry* pkImprovementInfo = GC.getImprovementInfo(eImprovement);
									if(pkImprovementInfo)
									{
										if(pkImprovementInfo->IsImprovementResourceTrade(eResource))
										{
											eCorrectBuild = eBuild;
											eCorrectImprovement = eImprovement;
											break;
										}
									}
								}
							}

						}

						// no valid build found
						if(eCorrectBuild == NO_BUILD || eCorrectImprovement == NO_IMPROVEMENT)
						{
							continue;
						}


						// if we can't build the improvement for the resource, continue!
						if(!kPlayer.canBuild(pLoopPlot, eCorrectBuild, false, false))
						{
							if(pLoopPlot->getImprovementType() == eCorrectImprovement)
							{
								iNumImprovedResources++;
								iNumResources++;
							}

							continue;
						}

						iNumResources++;
					}
				}
			}
		}

		bool bManyUnimproveResources = (2 * (iNumResources - iNumImprovedResources)) > iNumResources;
		int iMultiplier = kPlayer.getNumCities();
		iMultiplier += kPlayer.countCitiesFeatureSurrounded();
		if(bManyUnimproveResources)
			iMultiplier += 1;

		int iNumSettlers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE, true, false);
		iMultiplier += iNumSettlers;

		int iWeightThreshold = (iPerCityThreshold * iMultiplier);

		//Do we want more Builders?
		if((iNumBuilders * 100) < iWeightThreshold)
		{
			// slewis - need to check to see if training another worker will put us below the -10 threshold.
			// slewis - todo: move -10 to a xml value
			return (kPlayer.calculateGoldRate() > -10);

			// Also want to check and see if we have enough happiness to support another builder
			//int iBuilderUnhappiness = 0;
			//int iBuildersInGame = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, false, false);
			//if (iBuildersInGame > 0)
			//{
			//	CvUnit *pLoopUnit;
			//	int iLoop;
			//	for (pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
			//	{
			//		if (pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
			//		{
			//			break;
			//		}
			//	}
			//	if (pLoopUnit)
			//	{
			//		iBuilderUnhappiness = pLoopUnit->getUnitInfo().GetUnhappiness();
			//	}
			//}

			//return (kPlayer.GetExcessHappiness()) >= (iNumBuilders - iBuildersInGame + 1) * iBuilderUnhappiness;
		}
	}

	return false;
}

/// "Enough Tile Improvers" City Strategy: This is not a Player Strategy because we only want to prevent the training of new Builders, not nullify new Techs or Policies, which could still be very useful
bool CityStrategyAIHelpers::IsTestCityStrategy_EnoughTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
	int iLastTurnWorkerDisbanded = kPlayer.GetEconomicAI()->GetLastTurnWorkerDisbanded();
	if(iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 10)
	{
		return true;
	}

	AICityStrategyTypes eNeedImproversStrategy = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_TILE_IMPROVERS");

	if(eNeedImproversStrategy != NO_ECONOMICAISTRATEGY)
	{
		if(pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedImproversStrategy))
			return false;
	}

	int iNumBuilders = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);

	// If it's a minor with at least 1 worker per city, always return true
	if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		if(iNumBuilders >= kPlayer.getNumCities())
			return true;
	}

	CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);

	int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pCity);	// 10 Extra Weight per TILE_IMPROVEMENT Flavor
	int iPerCityThreshold = pCityStrategy->GetWeightThreshold() + iWeightThresholdModifier;	// 100

	int iModdedNumCities = kPlayer.getNumCities() + kPlayer.countCitiesFeatureSurrounded();
	int iWeightThreshold = (iPerCityThreshold * iModdedNumCities);

	// Average Player wants no more than 1.50 Builders per City [150 Weight is Average; range is 100 to 200]
	if((iNumBuilders * 100) >= iWeightThreshold)
	{
		return true;
	}

	return false;
}

/// "Need Naval Growth" City Strategy: Looks at the Tiles this City can work, and if there are a lot of Ocean tiles prioritizes NAVAL_GROWTH: should give us a Harbor eventually
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedNavalGrowth(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	int iNumOceanPlots = 0;
	int iNumTotalWorkablePlots = 0;

	CvPlot* pLoopPlot;

	// Look at all Tiles this City could potentially work
	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iPlotLoop);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getOwner() == pCity->getOwner())
			{
				if(!pLoopPlot->isCity())
				{
					iNumTotalWorkablePlots++;

					if(pLoopPlot->isWater() && !pLoopPlot->isLake())
					{
						iNumOceanPlots++;;
					}
				}
			}
		}
	}

	if(iNumTotalWorkablePlots > 0)
	{
		CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);
		int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pCity);	// -1 Weight per NAVAL_GROWTH Flavor
		int iWeightThreshold = pCityStrategy->GetWeightThreshold() + iWeightThresholdModifier;	// 40

		// If at least 35% (Average Player) of a City's workable Tiles are low-food Water then we really should be building a Harbor
		// [35 Weight is Average; range is 30 to 40]
		if((iNumOceanPlots * 100) / iNumTotalWorkablePlots >= iWeightThreshold)
		{
			return true;
		}
	}

	return false;
}

/// "Need Naval Tile Improvement" City Strategy: If there's an unimproved Resource in the water that we could be using, HIGHLY prioritize NAVAL_TILE_IMPROVEMENT in this City: should give us a Workboat in short order
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedNavalTileImprovement(CvCity* pCity)
{
	int iNumUnimprovedWaterResources = 0;

	CvPlot* pLoopPlot;

	// Look at all Tiles this City could potentially work to see if there are any Water Resources that could be improved
	for(int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iPlotLoop);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getOwner() == pCity->getOwner())
			{
				if(pLoopPlot->isWater())
				{
					// Only look at Tiles THIS City can use; Prevents issue where two Cities can look at the same tile the same turn and both want Workboats for it; By the time this Strategy is called for a City another City isn't guaranteed to have popped it's previous order and registered that it's now training a Workboat! :(
					if(pCity->GetCityCitizens()->IsCanWork(pLoopPlot))
					{
						// Does this Tile already have a Resource, and if so, is it already improved?
						if(pLoopPlot->getResourceType() != NO_RESOURCE && pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
						{
							iNumUnimprovedWaterResources++;
						}
					}
				}
			}
		}
	}

	int iNumWaterTileImprovers = GET_PLAYER(pCity->getOwner()).GetNumUnitsWithUnitAI(UNITAI_WORKER_SEA, true, true);

	// Are there more Water Resources we can build an Improvement on than we have Naval Tile Improvers?
	if(iNumUnimprovedWaterResources > iNumWaterTileImprovers)
	{
		return true;
	}

	return false;
}

/// "Enough Naval Tile Improvement" City Strategy: If we're not running "Need Naval Tile Improvement" then there's no need to worry about it at all
bool CityStrategyAIHelpers::IsTestCityStrategy_EnoughNavalTileImprovement(CvCity* pCity)
{
	AICityStrategyTypes eStrategyNeedNavalTileImprovement = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_NAVAL_TILE_IMPROVEMENT");

	if(eStrategyNeedNavalTileImprovement != NO_ECONOMICAISTRATEGY)
	{
		if(!pCity->GetCityStrategyAI()->IsUsingCityStrategy(eStrategyNeedNavalTileImprovement))
		{
			return true;
		}
	}

	return false;
}

/// "Need Improvement" City Strategy: if we need to get an improvement that increases a yield amount
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedImprovement(CvCity* pCity, YieldTypes yield)
{
	if(pCity->GetCityStrategyAI()->GetDeficientYield() == yield)
	{
		return true;
	}

	return false;
}

/// "Have Training Facility" City Strategy: this city should pound out military units
bool CityStrategyAIHelpers::IsTestCityStrategy_HaveTrainingFacility(CvCity* pCity)
{
	if(pCity->getDomainFreeExperience(DOMAIN_LAND) > 0)
	{
		return true;
	}

	return false;
}

/// "Capital Need Settler" City Strategy: have capital build a settler ASAP
bool CityStrategyAIHelpers::IsTestCityStrategy_CapitalNeedSettler(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	if(pCity->isCapital())
	{
		CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());

		if(!kPlayer.isMinorCiv())
		{
			int iNumCities = kPlayer.getNumCities();
			int iSettlersOnMapOrBuild = kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);
			int iCitiesPlusSettlers = iNumCities + iSettlersOnMapOrBuild;

			if((iCitiesPlusSettlers) < 3)
			{

				AICityStrategyTypes eUnderThreat = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_CAPITAL_UNDER_THREAT");
				if(eUnderThreat != NO_AICITYSTRATEGY)
				{
					if(GC.getGame().getGameTurn() > 50 && pCity->GetCityStrategyAI()->IsUsingCityStrategy(eUnderThreat))
					{
						return false;
					}
				}

				MilitaryAIStrategyTypes eMilStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
				if(eMilStrategy != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eMilStrategy))
				{
					// this is very risky, if this war fails, the civ lost the entire game as they have no backup plan
					return false;
				}

				CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);
				int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pCity);	// -10 per EXPANSION, +2 per DEFENSE
				int iWeightThreshold = pCityStrategy->GetWeightThreshold() + iWeightThresholdModifier;	// 130

				int iGameTurn = GC.getGame().getGameTurn();
				if((iCitiesPlusSettlers == 1 && (iGameTurn * 4) > iWeightThreshold) ||
					(iCitiesPlusSettlers == 2 && (iGameTurn * 2) > iWeightThreshold) || 
					(iCitiesPlusSettlers == 3 && iGameTurn > iWeightThreshold) )
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// "Capital Under Threat" City Strategy: need military units, don't build buildings!
bool CityStrategyAIHelpers::IsTestCityStrategy_CapitalUnderThreat(CvCity* pCity)
{
	if (pCity->isCapital())
	{
		CvPlayer &kPlayer = GET_PLAYER(pCity->getOwner());

		bool bAtPeace = GET_TEAM(kPlayer.getTeam()).getAtWarCount(false) == 0;

		if (!bAtPeace && !kPlayer.isMinorCiv())
		{
			CvCity *pMostThreatened = kPlayer.GetMilitaryAI()->GetMostThreatenedCity();
			if (pMostThreatened == pCity && pMostThreatened->getThreatValue() > 200)
			{
				return true;
			}
		}
	}

	return false;
}

/// "First Culture Building" City Strategy: construct a building to get some culture going in this city
bool CityStrategyAIHelpers::IsTestCityStrategy_FirstCultureBuilding(CvCity* pCity)
{
	if(pCity->GetJONSCulturePerTurnFromBuildings() <= 0)
	{
		return true;
	}

	if(pCity->isCapital() && pCity->GetJONSCulturePerTurnFromBuildings() <= 1)
	{
		return true;
	}

	return false;
}

/// "First Culture Building Emergency" City Strategy: construct a building to get some culture going in this city BECAUSE WE ARE RUNNING OUT OF ROOM!!!
bool CityStrategyAIHelpers::IsTestCityStrategy_FirstCultureBuildingEmergency(CvCity* pCity)
{
	if(pCity->getPopulation() >= 5 && pCity->GetJONSCulturePerTurnFromBuildings() <= 0)
	{
		return true;
	}

	return false;
}

/// "First Science Building" City Strategy: construct a building to get some science going in this city
bool CityStrategyAIHelpers::IsTestCityStrategy_FirstScienceBuilding(CvCity* pCity)
{
	if(pCity->getPopulation() >= 4 && pCity->GetYieldPerPopTimes100(YIELD_SCIENCE) == 0)
	{
		return true;
	}

	return false;
}

/// "First Gold Building" City Strategy: construct a building to get some more gold going in this city
bool CityStrategyAIHelpers::IsTestCityStrategy_FirstGoldBuilding(CvCity* pCity)
{
	if(pCity->getPopulation() >= 4 && pCity->getYieldRateModifier(YIELD_GOLD) == 0)
	{
		return true;
	}

	return false;
}

/// "First Production Building" City Strategy: construct a building to get some more hammers going in this city
bool CityStrategyAIHelpers::IsTestCityStrategy_FirstProductionBuilding(CvCity* pCity)
{
	if(pCity->getPopulation() >= 4 && pCity->GetBaseYieldRateFromBuildings(YIELD_PRODUCTION) == 0 && pCity->getYieldRateModifier(YIELD_PRODUCTION) == 0)
	{
		return true;
	}

	return false;
}

/// "First Faith Building" City Strategy: construct a building to get some faith going in this city
bool CityStrategyAIHelpers::IsTestCityStrategy_FirstFaithBuilding(CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
	FlavorTypes eFlavor = (FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION");

	int iReligionFlavor = kPlayer.GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavor);

	if(pCity->GetFaithPerTurnFromBuildings() > 0)
	{
		return false;
	}

	// Need population of 2 before worrying about this
	if(pCity->getPopulation() < 2)
	{
		return false;
	}

	// Turn on if high religion flavor (doesn't need to be as high if already has a pantheon)
	if((iReligionFlavor > 4 && kPlayer.GetReligions()->HasCreatedPantheon()) || iReligionFlavor > 6)
	{
		return true;
	}

	return false;
}

/// "Under Blockade" City Strategy: build walls or archers
bool CityStrategyAIHelpers::IsTestCityStrategy_UnderBlockade(CvCity* pCity)
{
	if(pCity->GetCityCitizens()->IsAnyPlotBlockaded()/* && !pCity->IsHasBuildingThatAllowsRangeStrike()*/)
	{
		return true;
	}

	return false;
}

/// "Is Puppet" City Strategy: build gold buildings and not military training buildings
bool CityStrategyAIHelpers::IsTestCityStrategy_IsPuppet(CvCity* pCity)
{
	if(pCity->IsPuppet())
	{
		return true;
	}

	return false;
}

/// "Medium City" City Strategy: If a City is 5 or above and we are playing at a high difficulty level
bool CityStrategyAIHelpers::IsTestCityStrategy_MediumCityHighDifficulty(CvCity* pCity)
{
	if(pCity->getPopulation() >= 5 && GC.getGame().getHandicapInfo().GetID() > 4)
	{
		return true;
	}

	return false;
}

/// "Original Capital" City Strategy: If a City was the original capital for any team (or is our original capital)
bool CityStrategyAIHelpers::IsTestCityStrategy_OriginalCapital(CvCity* pCity)
{
	if(pCity->IsOriginalCapital())
	{
		return true;
	}

	return false;
}


/// "River City" City Strategy: give a little flavor to this city
bool CityStrategyAIHelpers::IsTestCityStrategy_RiverCity(CvCity* pCity)
{
	if(pCity->plot()->isRiver())
	{
		return true;
	}

	return false;
}

/// "Hill City" City Strategy: give a little flavor to this city
bool CityStrategyAIHelpers::IsTestCityStrategy_HillCity(CvCity* pCity)
{
	// scan the nearby tiles to see if there are at least two hills in the vicinity
	const int iRange = 2;
	int iNumHills = 0;
	CvPlot* pPlot = pCity->plot();

	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iRange);
			if(pLoopPlot)
			{
				if(pLoopPlot->isHills() && pLoopPlot->getOwner() == pPlot->getOwner())
				{
					iNumHills++;
					if(iNumHills > 1)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

/// "Mountain City" City Strategy: give a little flavor to this city
bool CityStrategyAIHelpers::IsTestCityStrategy_MountainCity(CvCity* pCity)
{
	// scan the nearby tiles to see if there is a mountain close enough to build an observatory
	CvPlot* pPlot = pCity->plot();
	for(int iDirectionLoop = 0; iDirectionLoop < NUM_DIRECTION_TYPES; iDirectionLoop++)
	{
		CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), ((DirectionTypes)iDirectionLoop));
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isMountain())
			{
				return true;
			}
		}
	}


	return false;
}

/// "Forest City" City Strategy: give a little flavor to this city
bool CityStrategyAIHelpers::IsTestCityStrategy_ForestCity(CvCity* pCity)
{
	// scan the nearby tiles to see if there are at least two forests in the vicinity
	const int iRange = 2;
	int iNumForests = 0;
	CvPlot* pPlot = pCity->plot();

	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iRange);
			if(pLoopPlot)
			{
				// FEATURE_FOREST seems dubious to me...
				if(pLoopPlot->getFeatureType() == FEATURE_FOREST && pLoopPlot->getOwner() == pPlot->getOwner())
				{
					iNumForests++;
					if(iNumForests > 1)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

/// "Jungle City" City Strategy: give a little flavor to this city
bool CityStrategyAIHelpers::IsTestCityStrategy_JungleCity(CvCity* pCity)
{
	// scan the nearby tiles to see if there are at least two jungles in the vicinity
	const int iRange = 2;
	int iNumJungles = 0;
	CvPlot* pPlot = pCity->plot();

	for(int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for(int iDY = -iRange; iDY <= iRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXYWithRangeCheck(pPlot->getX(), pPlot->getY(), iDX, iDY, iRange);
			if(pLoopPlot)
			{
				// FEATURE_JUNGLE seems dubious to me...
				if(pLoopPlot->getFeatureType() == FEATURE_JUNGLE && pLoopPlot->getOwner() == pPlot->getOwner())
				{
					iNumJungles++;
					if(iNumJungles > 1)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


/// "Coast City" City Strategy: give a little flavor to this city
bool CityStrategyAIHelpers::IsTestCityStrategy_CoastCity(CvCity* pCity)
{
	if(pCity->plot()->isCoastalLand())
	{
		return true;
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_ManyTechsStolen(CvCity* pCity)
{
	PlayerTypes ePlayer = pCity->getOwner();
	CvEspionageAI* pEspionageAI = GET_PLAYER(ePlayer).GetEspionageAI();
	CvCityEspionage* pCityEspionage = pCity->GetCityEspionage();
	float fRatio = 0.0;
	int iTurnsOfEspionage = GC.getGame().getGameTurn() - pEspionageAI->m_iTurnEspionageStarted;
	if (pEspionageAI->m_iTurnEspionageStarted != 0)
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
		CvAssertMsg(eFlavorEspionage != NO_FLAVOR, "Could not find espionage flavor!");
		
		fRatio = pCityEspionage->m_aiNumTimesCityRobbed[ePlayer] / (float)(iTurnsOfEspionage);
	}

	if (fRatio > 0.0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CityStrategyAIHelpers::IsTestCityStrategy_KeyScienceCity(CvCity* pCity)
{
	if (GET_PLAYER(pCity->getOwner()).GetEspionageAI()->m_iTurnEspionageStarted == -1)
	{
		return false;
	}

	PlayerTypes ePlayer = pCity->getOwner();
	CvCity* pLoopCity = NULL;
	int iLoop = 0;
	int iNumBetterScienceCities = 0;
	int iNumOtherCities = 0;
	int iCityScienceOutput = pCity->getYieldRateTimes100(YIELD_SCIENCE);

	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		iNumOtherCities++;
		// don't evaluate ourselves
		if (pLoopCity == pCity)
		{
			continue;
		}

		if (pLoopCity->getYieldRateTimes100(YIELD_SCIENCE) > iCityScienceOutput)
		{
			iNumBetterScienceCities++;
		}
	}

	if (iNumOtherCities > 0)
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
		CvAssertMsg(eFlavorEspionage != NO_FLAVOR, "Could not find espionage flavor!");

		float fRatio = iNumBetterScienceCities / (float)iNumOtherCities;
		float fCutOff = (0.05f * GET_PLAYER(ePlayer).GetFlavorManager()->GetPersonalityIndividualFlavor(eFlavorEspionage));

		if (fRatio < fCutOff)
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
		return false;
	}
}


bool CityStrategyAIHelpers::IsTestCityStrategy_GoodGPCity(CvCity* pCity)
{
	// if this city is producing at least 8 GP points a turn (since most GP mod buildings increase by 25% this will yield 2 extra GP points a turn)

	int iTotalGPPChange = 0;

	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
		CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
		if(pkSpecialistInfo)
		{
			// Does this Specialist spawn a GP?
			if (pkSpecialistInfo->getGreatPeopleUnitClass() != NO_UNITCLASS)
			{
				int iCount = pCity->GetCityCitizens()->GetSpecialistCount(eSpecialist);

				// GPP from Specialists
				int iGPPChange = pkSpecialistInfo->getGreatPeopleRateChange() * iCount * 100;

				// GPP from Buildings
				iGPPChange += pCity->GetCityCitizens()->GetBuildingGreatPeopleRateChanges(eSpecialist) * 100;

				if (iGPPChange > 0)
				{
					int iMod = 0;

					// City mod
					iMod += pCity->getGreatPeopleRateModifier();

					// Player mod
					iMod += pCity->GetPlayer()->getGreatPeopleRateModifier();

					// Trait mod to this specific class
					if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
					{
						iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGreatScientistRateModifier();
					}

					iGPPChange *= (100 + iMod);
					iGPPChange /= 100;

					iTotalGPPChange += iGPPChange;
				}
			}
		}
	}

	if (iTotalGPPChange >= 800)
	{
		return true;
	}

	return false;
}
