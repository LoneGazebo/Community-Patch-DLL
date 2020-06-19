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
#include "CvEnumSerialization.h"
#include "CvInfosSerializationHelper.h"
#include "cvStopWatch.h"
#if defined(MOD_BALANCE_CORE)
#include "CvTypes.h"
#include "CvWonderProductionAI.h"
#endif
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
#if defined(MOD_BALANCE_CORE)
	return (index!=NO_AICITYSTRATEGY) ? m_paAICityStrategyEntries[index] : NULL;
#else
	return m_paAICityStrategyEntries[index];
#endif
}

//=====================================
// CvCityStrategyAI
//=====================================

/// defining static

unsigned char  CvCityStrategyAI::m_acBestYields[NUM_YIELD_TYPES][MAX_CITY_PLOTS];
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

#if defined(MOD_BALANCE_CORE)
	for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
		m_adYieldAvg[iI] = 0;
#endif
}

/// Serialization read
void CvCityStrategyAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	CvAssertMsg(m_piLatestFlavorValues != NULL && GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");

	int iNumFlavors;
	kStream >> iNumFlavors;
	ArrayWrapper<int> wrapLatestFlavor(iNumFlavors, m_piLatestFlavorValues);
	kStream >> wrapLatestFlavor;

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_pabUsingCityStrategy, GC.getNumAICityStrategyInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_paiTurnCityStrategyAdopted, GC.getNumAICityStrategyInfos());

	kStream >> m_eSpecialization;
	kStream >> m_eDefaultSpecialization;
	m_eFocusYield = (YieldTypes)NO_YIELD;	//force yield to default since we don't serialize it.

	m_pBuildingProductionAI->Read(kStream);
	m_pUnitProductionAI->Read(kStream);
	m_pProjectProductionAI->Read(kStream);
	m_pProcessProductionAI->Read(kStream);
}

/// Serialization write
void CvCityStrategyAI::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	CvAssertMsg(GC.getNumFlavorTypes() > 0, "Number of flavor values to serialize is expected to greater than 0");
	int iNumFlavors = GC.getNumFlavorTypes();
	kStream << iNumFlavors;
	kStream << ArrayWrapper<int>(iNumFlavors, m_piLatestFlavorValues);

	int iNumStrategies = GC.getNumAICityStrategyInfos();

	CvInfosSerializationHelper::WriteHashedDataArray<AICityStrategyTypes, bool>(kStream, m_pabUsingCityStrategy, iNumStrategies);
	CvInfosSerializationHelper::WriteHashedDataArray<AICityStrategyTypes, int>(kStream, m_paiTurnCityStrategyAdopted, iNumStrategies);

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
		const char* pcFlavorName = GC.getFlavorTypes((FlavorTypes)iFlavor).c_str(); pcFlavorName; //for debugging

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
#if defined(MOD_BALANCE_CORE)
CvString CvCityStrategyAI::GetHurryLogFileName(CvString& playerName, CvString& cityName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "CityStrategyAIHurryLog_" + playerName + "_" + cityName + ".csv";
	}
	else
	{
		strLogName = "CityStrategyAIHurryLog.csv";
	}

	return strLogName;
}
CvString CvCityStrategyAI::GetProductionLogFileName(CvString& playerName, CvString& cityName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "CityStrategyAIProductionLog_" + playerName + "_" + cityName + ".csv";
	}
	else
	{
		strLogName = "CityStrategyAIProductionLog.csv";
	}

	return strLogName;
}
#endif

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
	int iYieldAverage = (int)citystrategyround(fYieldAverage);

	if(iYieldAverage < iDesiredYield)
	{
		return true;
	}
	else
	{
		return false;
	}
}
#if defined(MOD_BALANCE_CORE)
/// Determines if the yield is below a sustainable amount
YieldTypes CvCityStrategyAI::GetMostDeficientYield()
{
	int iBestDiff = 0;
	YieldTypes eBestYield = NO_YIELD;
	for(uint ui = 0; ui < YIELD_TOURISM; ui++)
	{
		YieldTypes yield = (YieldTypes)ui;
		double fDesiredYield = GetDeficientYieldValue(yield);
		double fYieldAverage = GetYieldAverage(yield);

		int iDesiredYield = (int)citystrategyround(fDesiredYield * 100);
		int iYieldAverage = (int)citystrategyround(fYieldAverage);
		
		int iDiff = iDesiredYield - iYieldAverage;

		if(iDiff > iBestDiff)
		{
			eBestYield = yield;
			iBestDiff = iDiff;
		}
	}
	return eBestYield;
}
/// Determines if the yield is the best
YieldTypes CvCityStrategyAI::GetHighestYield()
{
	int iBestDiff = 0;
	YieldTypes eBestYield = NO_YIELD;
	for(uint ui = 0; ui < YIELD_TOURISM; ui++)
	{
		YieldTypes yield = (YieldTypes)ui;
		double fYieldAverage = GetYieldAverage(yield);

		int iYieldAverage = (int)citystrategyround(fYieldAverage);
		
		if(iYieldAverage > iBestDiff)
		{
			eBestYield = yield;
			iBestDiff = iYieldAverage;
		}
	}
	return eBestYield;
}
#endif

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
		for(uint ui = 0; ui < YIELD_TOURISM; ui++)
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
void CvCityStrategyAI::PrecalcYieldAverages()
{
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes) iI;
#if defined(MOD_BALANCE_CORE)
		if(eYield > YIELD_FAITH)
		{
			return;
		}
		int iYield = m_pCity->getYieldRateTimes100(eYield, false);
		if(eYield == YIELD_FOOD)
		{
			iYield -= (m_pCity->foodConsumption() * 100);
		}
		
		iYield /= max(1, m_pCity->getPopulation());

		m_adYieldAvg[iI] = iYield;
#endif
	}
}

double CvCityStrategyAI::GetYieldAverage(YieldTypes eYieldType)
{
	return m_adYieldAvg[eYieldType];
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
#if defined(MOD_BALANCE_CORE)
		//No it isn't!
		fDesiredYield = GC.getAI_CITYSTRATEGY_YIELD_DEFICIENT_CULTURE();
#endif
		break;
	case YIELD_FAITH:
#if defined(MOD_BALANCE_CORE)
		//No it isn't!
		fDesiredYield = 0.5f;
#endif
		break;
#if defined(MOD_API_UNIFIED_YIELDS_TOURISM)
	case YIELD_TOURISM:
		break;
#endif
#if defined(MOD_API_UNIFIED_YIELDS_GOLDEN_AGE)
	case YIELD_GOLDEN_AGE_POINTS:
		break;
#endif
	default:
		FAssertMsg(false, "Yield type is not handled. What?");
		return false;
	}

	return fDesiredYield;
}

/// Pick the next build for a city (unit, building or wonder)
#if defined(MOD_BALANCE_CORE)
void CvCityStrategyAI::ChooseProduction(BuildingTypes eIgnoreBldg, UnitTypes eIgnoreUnit, bool bInterruptBuildings, bool bInterruptWonders)
#else
void CvCityStrategyAI::ChooseProduction(BuildingTypes eIgnoreBldg /* = NO_BUILDING */, UnitTypes eIgnoreUnit/*  = NO_UNIT */)
#endif
{
	//Let's make sure we're not getting into a weird loop.
	if (bInterruptBuildings)
	{
		//Are we already building a military unit? If so, ignore this call to choose a new production.
		UnitTypes eUnit = m_pCity->getProductionUnit();
		if (eUnit != NO_UNIT)
		{
			if (GC.getUnitInfo(eUnit)->GetCombat() > 0 || GC.getUnitInfo(eUnit)->GetRangedCombat() > 0)
				return;
		}
	}
	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());

	if (!bInterruptWonders && m_pCity->getProductionBuilding() != NO_BUILDING && kPlayer.GetWonderProductionAI()->IsWonder(*GC.getBuildingInfo((BuildingTypes)m_pCity->getProductionBuilding())))
		return;

	int iBldgLoop, iUnitLoop, iProjectLoop, iProcessLoop, iTempWeight;
	CvCityBuildable buildable;
	CvCityBuildable selection;
	UnitTypes eUnitForOperation;
	UnitTypes eUnitForArmy;

	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);

	// Reset vector holding items we can currently build
	m_Buildables.clear();
	m_BuildablesPrecheck.clear();
	
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
		iTempWeight += m_pUnitProductionAI->GetWeight(eUnitForOperation);
		if(iTempWeight > 0)
		{						
			m_BuildablesPrecheck.push_back(buildable, iTempWeight);
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
		iTempWeight += (GC.getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER() * iOffenseFlavor);
		if(iTempWeight > 0)
		{
			buildable.m_iValue = iTempWeight;
			m_BuildablesPrecheck.push_back(buildable, iTempWeight);
		}
	}

	// Loop through adding the available units
	for(iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
	{	
		// Make sure this unit can be built now
		if ((UnitTypes)iUnitLoop != eIgnoreUnit && m_pCity->canTrain((UnitTypes)iUnitLoop, (m_pCity->isProductionUnit() && (UnitTypes)iUnitLoop == m_pCity->getProductionUnit())))
		{
			buildable.m_eBuildableType = CITY_BUILDABLE_UNIT;
			buildable.m_iIndex = iUnitLoop;
			buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft((UnitTypes)iUnitLoop, 0);
			iTempWeight = m_pUnitProductionAI->GetWeight((UnitTypes)iUnitLoop);		
			if(iTempWeight > 0)
			{
				m_BuildablesPrecheck.push_back(buildable, iTempWeight);
				buildable.m_iValue = iTempWeight;
			}
		}
	}

	std::vector<int> vTotalBuildingCount( GC.getNumBuildingInfos(), 0);
	int iLoop;
	for(const CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
	{
		if(pLoopCity != NULL)
		{
			const std::vector<BuildingTypes>& vBuildings = pLoopCity->GetCityBuildings()->GetAllBuildingsHere();
			for (size_t i=0; i<vBuildings.size(); i++)
				vTotalBuildingCount[ vBuildings[i] ]++;
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
		if ((BuildingTypes)iBldgLoop != eIgnoreBldg && m_pCity->canConstruct(eLoopBuilding, vTotalBuildingCount, (m_pCity->isProductionBuilding() && (BuildingTypes)iBldgLoop == m_pCity->getProductionBuilding())))
		{
			buildable.m_eBuildableType = CITY_BUILDABLE_BUILDING;
			buildable.m_iIndex = iBldgLoop;
			buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eLoopBuilding, 0);
			iTempWeight = m_pBuildingProductionAI->GetWeight(eLoopBuilding);

			// Save it for later
			if(iTempWeight > 0)
			{
				buildable.m_iValue = iTempWeight;
				m_BuildablesPrecheck.push_back(buildable, iTempWeight);
			}
		}
	}

	// Loop through adding the available projects
	for(iProjectLoop = 0; iProjectLoop < GC.GetGameProjects()->GetNumProjects(); iProjectLoop++)
	{
		if (m_pCity->canCreate((ProjectTypes)iProjectLoop, (m_pCity->isProductionProject() && (ProjectTypes)iProjectLoop == m_pCity->getProductionProject())))
		{
			int iTempWeight = m_pProjectProductionAI->GetWeight((ProjectTypes)iProjectLoop);
			if(iTempWeight > 0)
			{
				buildable.m_eBuildableType = CITY_BUILDABLE_PROJECT;
				buildable.m_iIndex = iProjectLoop;
				buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft((ProjectTypes)iProjectLoop, 0);
				buildable.m_iValue = iTempWeight;
				m_BuildablesPrecheck.push_back(buildable, m_pProjectProductionAI->GetWeight((ProjectTypes)iProjectLoop));
			}
		}
	}

	// Loop through adding available processes
	//I cannot use the yield rate since it adds in set process yield, which is what I am trying to set...
	int iBaseYield = GetCity()->getBaseYieldRate(YIELD_PRODUCTION) * 100;
	iBaseYield += (GetCity()->GetYieldPerPopTimes100(YIELD_PRODUCTION) * GetCity()->getPopulation());
#if defined(MOD_BALANCE_CORE)
	iBaseYield += (GetCity()->GetYieldPerPopInEmpireTimes100(YIELD_PRODUCTION) * GET_PLAYER(GetCity()->getOwner()).getTotalPopulation());
#endif
	int iModifiedYield = iBaseYield * GetCity()->getBaseYieldRateModifier(YIELD_PRODUCTION);
	iModifiedYield /= 10000;

	if (iModifiedYield >= 5 || m_BuildablesPrecheck.size() <= 0)
	{
		for (iProcessLoop = 0; iProcessLoop < GC.getNumProcessInfos(); iProcessLoop++)
		{
			ProcessTypes eProcess = (ProcessTypes)iProcessLoop;
			
			if (m_pCity->canMaintain(eProcess, (m_pCity->isProductionProcess() && eProcess == m_pCity->getProductionProcess())))
			{		
				int iTempWeight = m_pProcessProductionAI->GetWeight((ProcessTypes)iProcessLoop);
				if (eProcess == GC.getInfoTypeForString("PROCESS_DEFENSE"))
				{
					iTempWeight = 100;
				}
				if (iTempWeight > 0)
				{
					buildable.m_eBuildableType = CITY_BUILDABLE_PROCESS;
					buildable.m_iIndex = iProcessLoop;
					buildable.m_iTurnsToConstruct = 1;
					buildable.m_iValue = iTempWeight;
					m_BuildablesPrecheck.push_back(buildable, iTempWeight);
				}
			}
		}
	}

	m_BuildablesPrecheck.SortItems();

	LogPossibleBuilds();

	if(m_BuildablesPrecheck.size() > 0)
	{
		int iGPT = kPlayer.GetTreasury()->CalculateBaseNetGold();

		//stats to decide whether to disband a unit
		int iWaterPriority = m_pCity->GetTradePrioritySea();
		int iLandPriority = m_pCity->GetTradePriorityLand();

		int iWaterRoutes = -1;
		int iLandRoutes = -1;

		if(iWaterPriority >= 0)
		{
			//0 is best, and 1+ = 100% less valuable than top. More routes from better cities, please!
			iWaterRoutes = 1000 - min(1000, (iWaterPriority * 50));
		}
		if(iLandPriority >= 0)
		{
			iLandRoutes = 1000 - min(1000, (iLandPriority * 50));
		}

		for(int iI = 0; iI < m_BuildablesPrecheck.size(); iI++)
		{
			selection = m_BuildablesPrecheck.GetElement(iI);
			switch(selection.m_eBuildableType)
			{
				case CITY_BUILDABLE_UNIT_FOR_OPERATION:
				{
					UnitTypes eUnitType = (UnitTypes) selection.m_iIndex;
					bool bCitySameAsMuster = false;
					OperationSlot thisOperationSlot = kPlayer.PeekAtNextUnitToBuildForOperationSlot(m_pCity, bCitySameAsMuster);
					if (thisOperationSlot.IsValid() && bCitySameAsMuster)
					{
						CvArmyAI* pThisArmy = kPlayer.getArmyAI(thisOperationSlot.m_iArmyID);

						if(pThisArmy)
						{
							int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, true, pThisArmy, m_BuildablesPrecheck.GetWeight(iI), iGPT, 0, 0, false, false, bInterruptBuildings);
							if (iNewWeight > 0)
							{
								selection.m_iValue = iNewWeight;
								m_Buildables.push_back(selection, iNewWeight);
							}
							else
								LogInvalidItem(selection, iNewWeight);
						}
						else
						{
							int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, true, NULL, m_BuildablesPrecheck.GetWeight(iI), iGPT, 0, 0, false, false, bInterruptBuildings);
							if(iNewWeight > 0)
							{
								selection.m_iValue = iNewWeight;
								m_Buildables.push_back(selection, iNewWeight);
							}
							else
								LogInvalidItem(selection, iNewWeight);
						}
					}
					else
					{
						int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, true, NULL, m_BuildablesPrecheck.GetWeight(iI), iGPT, 0, 0, false, false, bInterruptBuildings);
						if(iNewWeight > 0)
						{
							selection.m_iValue = iNewWeight;
							m_Buildables.push_back(selection, iNewWeight);
						}
						else
							LogInvalidItem(selection, iNewWeight);
					}
					break;
				}
				case CITY_BUILDABLE_UNIT_FOR_ARMY:
				{
					UnitTypes eUnitType = (UnitTypes) selection.m_iIndex;
					int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, true, NULL, m_BuildablesPrecheck.GetWeight(iI), iGPT, 0, 0, false, false, bInterruptBuildings);
					if(iNewWeight > 0)
					{
						selection.m_iValue = iNewWeight;
						m_Buildables.push_back(selection, iNewWeight);
					}
					else
						LogInvalidItem(selection, iNewWeight);
					break;
				}
				case CITY_BUILDABLE_UNIT:
				{
					UnitTypes eUnitType = (UnitTypes) selection.m_iIndex;
					int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, false, NULL, m_BuildablesPrecheck.GetWeight(iI), iGPT, iWaterRoutes, iLandRoutes, false, false, bInterruptBuildings);
					if(iNewWeight > 0)
					{
						selection.m_iValue = iNewWeight;
						m_Buildables.push_back(selection, iNewWeight);
					}
					else
						LogInvalidItem(selection, iNewWeight);
					break;
				}
				case CITY_BUILDABLE_BUILDING:
				{
					BuildingTypes eBuildingType = (BuildingTypes) selection.m_iIndex;
					int iNewWeight = GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuildingType, m_BuildablesPrecheck.GetWeight(iI), iLandRoutes, iWaterRoutes, iGPT, bInterruptBuildings);
					if(iNewWeight > 0)
					{
						selection.m_iValue = iNewWeight;
						m_Buildables.push_back(selection, iNewWeight);
					}
					else
						LogInvalidItem(selection, iNewWeight);
					break;
				}
				case CITY_BUILDABLE_PROCESS:
				{
					ProcessTypes eProcessType = (ProcessTypes)selection.m_iIndex;
					int iNewWeight = m_pProcessProductionAI->CheckProcessBuildSanity(eProcessType, m_BuildablesPrecheck.GetWeight(iI), m_BuildablesPrecheck.size(), iGPT);
					if(iNewWeight > 0)
					{
						selection.m_iValue = iNewWeight;
						m_Buildables.push_back(selection, iNewWeight);
					}
					else
						LogInvalidItem(selection, iNewWeight);
					break;
				}
				case CITY_BUILDABLE_PROJECT:
				{
					ProjectTypes eProjectType = (ProjectTypes) selection.m_iIndex;
					int iNewWeight = m_pProjectProductionAI->CheckProjectBuildSanity(eProjectType, m_BuildablesPrecheck.GetWeight(iI));
					if(iNewWeight > 0)
					{
						selection.m_iValue = iNewWeight;
						m_Buildables.push_back(selection, iNewWeight);
					}
					else
						LogInvalidItem(selection, iNewWeight);
					break;
				}
			}
		}
	}

	ReweightByCost();

	m_Buildables.SortItems();

	LogPossibleBuildsPostCheck();

	if (m_Buildables.size() <= 0)
		m_Buildables = m_BuildablesPrecheck;

	if(m_Buildables.size() > 0)
	{
		int iRushIfMoreThanXTurns = GC.getAI_ATTEMPT_RUSH_OVER_X_TURNS_TO_BUILD();
		iRushIfMoreThanXTurns *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iRushIfMoreThanXTurns /= 100;

		bool bContinueSelection = false;
		for (int i = 0; i < m_Buildables.size(); i++)
		{
			if (bContinueSelection)
				break;

			switch (m_Buildables.GetElement(i).m_eBuildableType)
			{
				case CITY_BUILDABLE_UNIT:
				case CITY_BUILDABLE_UNIT_FOR_ARMY:
				case CITY_BUILDABLE_UNIT_FOR_OPERATION:
				{
					UnitTypes eUnitType = (UnitTypes)m_Buildables.GetElement(i).m_iIndex;
					if (m_pCity->isProductionUnit() && m_pCity->getProductionUnit() == eUnitType)
					{
						selection = m_Buildables.GetElement(i);
						bContinueSelection = true;
					}
					break;
				}

				case CITY_BUILDABLE_BUILDING:
				{
					BuildingTypes eBuildingType = (BuildingTypes)m_Buildables.GetElement(i).m_iIndex;
					if (m_pCity->isProductionBuilding() && !bInterruptBuildings && m_pCity->getProductionBuilding() == eBuildingType)
					{
						selection = m_Buildables.GetElement(i);
						bContinueSelection = true;
					}
					break;
				}

				case CITY_BUILDABLE_PROJECT:
				{
					ProjectTypes eProjectType = (ProjectTypes)m_Buildables.GetElement(i).m_iIndex;
					if (m_pCity->isProductionProject() && m_pCity->getProductionProject() == eProjectType)
					{
						selection = m_Buildables.GetElement(i);
						bContinueSelection = true;
					}
					break;
				}
			}
		}

		if (!bContinueSelection)
		{
			int iNumChoices = GC.getGame().getHandicapInfo().GetCityProductionNumOptions();
			if (m_pCity->isBarbarian())
			{
				selection = m_Buildables.GetElement(0);
			}
			else
			{
				selection = m_Buildables.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing city build from Top 2 Choices");
			}
		}

		bool bRush = selection.m_iTurnsToConstruct > iRushIfMoreThanXTurns;

		LogCityProduction(selection, bRush);

		switch(selection.m_eBuildableType)
		{
			case CITY_BUILDABLE_UNIT:
			case CITY_BUILDABLE_UNIT_FOR_ARMY:
			case CITY_BUILDABLE_UNIT_FOR_OPERATION:
			{
				UnitTypes eUnitType = (UnitTypes)selection.m_iIndex;
				CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnitType);
				if (pkUnitInfo)
				{
					UnitAITypes eUnitAI = pkUnitInfo->GetDefaultUnitAIType();
					GetCity()->pushOrder(ORDER_TRAIN, eUnitType, eUnitAI, false, true, false, bRush);

					if (pkUnitInfo->IsFound())
						kPlayer.GetMilitaryAI()->ResetNumberOfTimesSettlerBuildSkippedOver();
				}
				if (selection.m_eBuildableType == CITY_BUILDABLE_UNIT_FOR_OPERATION)
				{
					kPlayer.GetMilitaryAI()->ResetNumberOfTimesOpsBuildSkippedOver();
				}
				break;
			}

			case CITY_BUILDABLE_BUILDING:
			{
				BuildingTypes eBuildingType = (BuildingTypes) selection.m_iIndex;
				GetCity()->pushOrder(ORDER_CONSTRUCT, eBuildingType, -1, false, true, false, bRush);
				break;
			}

			case CITY_BUILDABLE_PROJECT:
			{
				ProjectTypes eProjectType = (ProjectTypes) selection.m_iIndex;
				GetCity()->pushOrder(ORDER_CREATE, eProjectType, -1, false, true, false, bRush);
				break;
			}

			case CITY_BUILDABLE_PROCESS:
			{
				ProcessTypes eProcessType = (ProcessTypes)selection.m_iIndex;
				GetCity()->pushOrder(ORDER_MAINTAIN, eProcessType, -1, false, true, false, false); // ignoring rush because we can't rush a process
				break;
			}
		}

	}

	return;
}

#if defined(MOD_BALANCE_CORE)
/// Pick the next build for a city (unit, building)
CvCityBuildable CvCityStrategyAI::ChooseHurry(bool bUnitOnly, bool bFaithPurchase)
{
	int iBldgLoop, iUnitLoop, iTempWeight;
	CvCityBuildable buildable;
	CvCityBuildable selection;
	UnitTypes eUnitForOperation;
	UnitTypes eUnitForArmy;

	CvPlayerAI& kPlayer = GET_PLAYER(m_pCity->getOwner());
	iTempWeight = 0;

	RandomNumberDelegate fcn = MakeDelegate(&GC.getGame(), &CvGame::getJonRandNum);

	YieldTypes ePurchaseYield = YIELD_GOLD;
	if (bFaithPurchase)
		ePurchaseYield = YIELD_FAITH;

	// Reset vector holding items we can currently build
	m_Buildables.clear();
	m_BuildablesPrecheck.clear();
	
	if (!bFaithPurchase)
	{
		// Check units for operations first
		eUnitForOperation = m_pCity->GetUnitForOperation();
		if (eUnitForOperation != NO_UNIT)
		{
			CvUnitEntry* pUnitEntry = GC.getUnitInfo((UnitTypes)eUnitForOperation);
			if (pUnitEntry)
			{
				bool bOoS = (kPlayer.GetNumUnitsOutOfSupply() > 0 && pUnitEntry->IsMilitarySupport() && !pUnitEntry->IsNoSupply());
				if (!bOoS)
				{
					buildable.m_eBuildableType = CITY_BUILDABLE_UNIT_FOR_OPERATION;
					buildable.m_iIndex = (int)eUnitForOperation;
					buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eUnitForOperation, 0);
					iTempWeight = GC.getAI_CITYSTRATEGY_OPERATION_UNIT_BASE_WEIGHT();
					int iOffenseFlavor = kPlayer.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) + kPlayer.GetMilitaryAI()->GetNumberOfTimesOpsBuildSkippedOver();
					iTempWeight += (GC.getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER() * iOffenseFlavor);
					iTempWeight += m_pUnitProductionAI->GetWeight(eUnitForOperation);
					if (iTempWeight > 0)
					{
						buildable.m_iValue = iTempWeight;
						m_BuildablesPrecheck.push_back(buildable, iTempWeight);
						kPlayer.GetMilitaryAI()->BumpNumberOfTimesOpsBuildSkippedOver();
					}
				}
			}
		}
		// Next units for sneak attack armies
		eUnitForArmy = kPlayer.GetMilitaryAI()->GetUnitForArmy(GetCity());
		if (eUnitForArmy != NO_UNIT)
		{
			CvUnitEntry* pUnitEntry = GC.getUnitInfo((UnitTypes)eUnitForArmy);
			if(pUnitEntry)
			{
				bool bOoS = (kPlayer.GetNumUnitsOutOfSupply() > 0 && pUnitEntry->IsMilitarySupport() && !pUnitEntry->IsNoSupply());
				if (!bOoS)
				{
					buildable.m_eBuildableType = CITY_BUILDABLE_UNIT_FOR_ARMY;
					buildable.m_iIndex = (int)eUnitForArmy;
					buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eUnitForArmy, 0);
					iTempWeight = GC.getAI_CITYSTRATEGY_ARMY_UNIT_BASE_WEIGHT();
					int iOffenseFlavor = kPlayer.GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
					iTempWeight += (GC.getAI_CITYSTRATEGY_OPERATION_UNIT_FLAVOR_MULTIPLIER() * iOffenseFlavor);
					if (iTempWeight > 0)
					{
						buildable.m_iValue = iTempWeight;
						m_BuildablesPrecheck.push_back(buildable, iTempWeight);
					}
				}
			}
		}
	}

	// Loop through adding the available units
	for(iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
	{
		CvUnitEntry* pUnitEntry = GC.getUnitInfo((UnitTypes)iUnitLoop);
		if (bFaithPurchase)
		{
			if (pUnitEntry)
			{
				if (pUnitEntry->IsSpreadReligion() || pUnitEntry->IsRemoveHeresy())
					continue;

				if (pUnitEntry->GetFaithCost() <= 0 || pUnitEntry->GetSpecialUnitType() != NO_SPECIALUNIT)
					continue;
			}
		}
		if (pUnitEntry)
		{
			bool bOoS = (kPlayer.GetNumUnitsOutOfSupply() > 0 && pUnitEntry->IsMilitarySupport() && !pUnitEntry->IsNoSupply());
			if (!bOoS)
			{
				// Make sure this unit can be built now
				if (m_pCity->IsCanPurchase(true, true, (UnitTypes)iUnitLoop, NO_BUILDING, NO_PROJECT, ePurchaseYield))
				{
					buildable.m_eBuildableType = CITY_BUILDABLE_UNIT;
					buildable.m_iIndex = iUnitLoop;
					buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft((UnitTypes)iUnitLoop, 0);

					iTempWeight = m_pUnitProductionAI->GetWeight((UnitTypes)iUnitLoop);

					if (iTempWeight > 0)
					{
						buildable.m_iValue = iTempWeight;
						m_BuildablesPrecheck.push_back(buildable, iTempWeight);
					}
				}
			}
		}
	}
	
	// Loop through adding the available buildings
	if (!bUnitOnly)
	{
		std::vector<int> vTotalBuildingCount( GC.getNumBuildingInfos(), 0);
		int iLoop;
		for(const CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			const std::vector<BuildingTypes>& vBuildings = pLoopCity->GetCityBuildings()->GetAllBuildingsHere();
			for (size_t i=0; i<vBuildings.size(); i++)
				vTotalBuildingCount[ vBuildings[i] ]++;
		}

		for (iBldgLoop = 0; iBldgLoop < GC.GetGameBuildings()->GetNumBuildings(); iBldgLoop++)
		{
			const BuildingTypes eLoopBuilding = static_cast<BuildingTypes>(iBldgLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eLoopBuilding);

			//Skip if null
			if (pkBuildingInfo == NULL)
				continue;

			// Make sure this building can be built now
			if (m_pCity->IsCanPurchase(vTotalBuildingCount, true, true, NO_UNIT, eLoopBuilding, NO_PROJECT, ePurchaseYield))
			{
				buildable.m_eBuildableType = CITY_BUILDABLE_BUILDING;
				buildable.m_iIndex = iBldgLoop;
				buildable.m_iTurnsToConstruct = GetCity()->getProductionTurnsLeft(eLoopBuilding, 0);

				iTempWeight = m_pBuildingProductionAI->GetWeight(eLoopBuilding);

				// Save it for later
				if (iTempWeight > 0)
				{
					buildable.m_iValue = iTempWeight;
					m_BuildablesPrecheck.push_back(buildable, iTempWeight);
				}
			}
		}
	}
	m_BuildablesPrecheck.SortItems();

	ReweightPreCheckByCost();

	LogPossibleHurries();

	if(m_BuildablesPrecheck.size() > 0)
	{
		int iGPT = kPlayer.GetTreasury()->CalculateBaseNetGold();
		////Sanity and AI Optimization Check
	
		//stats to decide whether to disband a unit
		int iWaterPriority = m_pCity->GetTradePrioritySea();
		int iLandPriority = m_pCity->GetTradePriorityLand();

		int iWaterRoutes = -1;
		int iLandRoutes = -1;

		if (iWaterPriority >= 0)
		{
			//0 is best, and 1+ = 100% less valuable than top. More routes from better cities, please!
			iWaterRoutes = 1000 - min(1000, (iWaterPriority * 50));
		}
		if (iLandPriority >= 0)
		{
			iLandRoutes = 1000 - min(1000, (iLandPriority * 50));
		}

		for(int iI = 0; iI < m_BuildablesPrecheck.size(); iI++)
		{
			selection = m_BuildablesPrecheck.GetElement(iI);
			switch(selection.m_eBuildableType)
			{
				case CITY_BUILDABLE_UNIT_FOR_OPERATION:
				{
					UnitTypes eUnitType = (UnitTypes) selection.m_iIndex;
					bool bCitySameAsMuster = false;
					OperationSlot thisOperationSlot = kPlayer.PeekAtNextUnitToBuildForOperationSlot(m_pCity, bCitySameAsMuster);
					if (thisOperationSlot.IsValid() && bCitySameAsMuster)
					{
						CvArmyAI* pThisArmy = kPlayer.getArmyAI(thisOperationSlot.m_iArmyID);

						if(pThisArmy)
						{
							int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, true, pThisArmy,  m_BuildablesPrecheck.GetWeight(iI), iGPT, true);
							if(iNewWeight > 0)
							{
								selection.m_iValue = iNewWeight;
								m_Buildables.push_back(selection, iNewWeight);
							}
						}
						else
						{
							int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, true, NULL, m_BuildablesPrecheck.GetWeight(iI), iGPT, true);
							if(iNewWeight > 0)
							{
								selection.m_iValue = iNewWeight;
								m_Buildables.push_back(selection, iNewWeight);
							}
						}
					}
					else
					{
						int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, true, NULL, m_BuildablesPrecheck.GetWeight(iI), iGPT, true);
						if(iNewWeight > 0)
						{
							selection.m_iValue = iNewWeight;
							m_Buildables.push_back(selection, iNewWeight);
						}
					}
					break;
				}
				case CITY_BUILDABLE_UNIT_FOR_ARMY:
				{
					UnitTypes eUnitType = (UnitTypes) selection.m_iIndex;
					int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, true, NULL, m_BuildablesPrecheck.GetWeight(iI), iGPT, true);
					if(iNewWeight > 0)
					{
						selection.m_iValue = iNewWeight;
						m_Buildables.push_back(selection, iNewWeight);
					}
					break;
				}
				case CITY_BUILDABLE_UNIT:
				{
					UnitTypes eUnitType = (UnitTypes) selection.m_iIndex;
					int iNewWeight = GetUnitProductionAI()->CheckUnitBuildSanity(eUnitType, false, NULL, m_BuildablesPrecheck.GetWeight(iI), iGPT, iWaterRoutes, iLandRoutes, true);
					if(iNewWeight > 0)
					{
						selection.m_iValue = iNewWeight;
						m_Buildables.push_back(selection, iNewWeight);
					}
					break;
				}
				case CITY_BUILDABLE_BUILDING:
				{
					BuildingTypes eBuildingType = (BuildingTypes) selection.m_iIndex;
					int iNewWeight = GetBuildingProductionAI()->CheckBuildingBuildSanity(eBuildingType, m_BuildablesPrecheck.GetWeight(iI), iLandRoutes, iWaterRoutes, iGPT);
					int AmountComplete = GetCity()->GetCityBuildings()->GetBuildingProductionTimes100(eBuildingType);
					if (AmountComplete > 0)
					{
						int AmountNeeded = max(1, GetCity()->getProductionNeeded(eBuildingType));
						AmountComplete /= AmountNeeded;
						if (AmountComplete < 50)
						{
							iNewWeight *= (100 + AmountComplete);
							iNewWeight /= max(1, AmountComplete);
						}
					}
					if(iNewWeight > 0)
					{
						selection.m_iValue = iNewWeight;
						m_Buildables.push_back(selection, iNewWeight);
					}
					break;
				}
			}
		}
	}

	m_Buildables.SortItems();

	LogPossibleHurriesPostCheck();

	if(m_Buildables.GetTotalWeight() > 0)
	{
		// Choose from the best options (currently 2)
		int iNumChoices = GC.getGame().getHandicapInfo().GetCityProductionNumOptions();
		if (m_pCity->isBarbarian())
		{
			selection = m_Buildables.GetElement(0);
		}
		else
		{
			selection = m_Buildables.ChooseFromTopChoices(iNumChoices, &fcn, "Choosing city hurry from Top Choices");
		}
		return selection;
	}

	buildable.m_eBuildableType = NOT_A_CITY_BUILDABLE;
	return buildable;
}
#endif

/// Called every turn to see what CityStrategies this City should using (or not)
void CvCityStrategyAI::DoTurn()
{
	AI_PERF_FORMAT("City-AI-perf.csv", ("CvCityStrategyAI::DoTurn, Turn %03d, %s, %s", GC.getGame().getElapsedGameTurns(), m_pCity->GetPlayer()->getCivilizationShortDescription(), m_pCity->getName().c_str()) );

	int iCityStrategiesLoop = 0;

#if defined(MOD_BALANCE_CORE)
	PrecalcYieldAverages();
#endif

	// Loop through all CityStrategies
	for(iCityStrategiesLoop = 0; iCityStrategiesLoop < GetAICityStrategies()->GetNumAICityStrategies(); iCityStrategiesLoop++)
	{
		AICityStrategyTypes eCityStrategy = (AICityStrategyTypes) iCityStrategiesLoop;
		CvAICityStrategyEntry* pCityStrategy = GetAICityStrategies()->GetEntry(iCityStrategiesLoop);
#if defined(MOD_BALANCE_CORE)
		if(pCityStrategy == NULL)	// Can have holes in the list
			continue;

		// Minor Civs can't run some Strategies
		if( (GET_PLAYER(GetCity()->getOwner()).isMinorCiv() || GET_PLAYER(GetCity()->getOwner()).isBarbarian() ) && pCityStrategy->IsNoMinorCivs())
		{
			continue;
		}
#else
		// Minor Civs can't run some Strategies
		if(GET_PLAYER(GetCity()->getOwner()).isMinorCiv() && pCityStrategy->IsNoMinorCivs())
		{
			continue;
		}
#endif

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
#if defined(MOD_BALANCE_CORE)
				else if(strStrategyName == "AICITYSTRATEGY_LAKEBOUND")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_Lakebound(GetCity());
#endif
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
#if defined(MOD_BALANCE_CORE)
				else if(strStrategyName == "AICITYSTRATEGY_ENOUGH_SETTLERS")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_EnoughSettlers(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEW_CONTINENT_FEEDER")
#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NewContinentFeeder(eCityStrategy, GetCity());
#else
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NewContinentFeeder(GetCity());
#endif
				else if(strStrategyName == "AICITYSTRATEGY_POCKET_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_PocketCity(GetCity());
#endif
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
				else if(strStrategyName == "AICITYSTRATEGY_NEED_INTERNATIONAL_LAND_TRADE_ROUTE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedInternationalTradeRoute(GetCity(), DOMAIN_LAND);
				else if(strStrategyName == "AICITYSTRATEGY_NO_NEED_INTERNATIONAL_LAND_TRADE_ROUTE")
					//duplication of pathfinding going on here, should not run the same check twice ... best fix in xml
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NoNeedInternationalTradeRoute(GetCity(), DOMAIN_LAND);
				else if(strStrategyName == "AICITYSTRATEGY_NEED_INTERNATIONAL_SEA_TRADE_ROUTE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedInternationalTradeRoute(GetCity(), DOMAIN_SEA);
				else if(strStrategyName == "AICITYSTRATEGY_NO_NEED_INTERNATIONAL_SEA_TRADE_ROUTE")
					//duplication of pathfinding going on here, should not run the same check twice ... best fix in xml
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NoNeedInternationalTradeRoute(GetCity(), DOMAIN_SEA);
				else if(strStrategyName == "AICITYSTRATEGY_INTERNATIONAL_TRADE_DESTINATION")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_IsInternationalTradeDestination(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_INTERNATIONAL_TRADE_ORIGIN")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_IsInternationalTradeOrigin(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_CULTURE_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedCultureBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_NEED_TOURISM_BUILDING")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedTourismBuilding(GetCity());
				else if(strStrategyName == "AICITYSTRATEGY_GOOD_AIRLIFT_CITY")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_GoodAirliftCity(GetCity());
#if defined(MOD_DIPLOMACY_CITYSTATES)
				else if(MOD_DIPLOMACY_CITYSTATES && strStrategyName == "AICITYSTRATEGY_NEED_DIPLOMATS")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedDiplomats(GetCity()); 
				else if(MOD_DIPLOMACY_CITYSTATES && strStrategyName == "AICITYSTRATEGY_NEED_DIPLOMATS_CRITICAL")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedDiplomatsCritical(GetCity()); 
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS)
				else if(MOD_BALANCE_CORE_HAPPINESS && strStrategyName == "AICITYSTRATEGY_NEED_HAPPINESS_CULTURE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessCulture(GetCity());
				else if(MOD_BALANCE_CORE_HAPPINESS && strStrategyName == "AICITYSTRATEGY_NEED_HAPPINESS_SCIENCE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessScience(GetCity()); 
				else if(MOD_BALANCE_CORE_HAPPINESS && strStrategyName == "AICITYSTRATEGY_NEED_HAPPINESS_DEFENSE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessDefense(GetCity()); 
				else if(MOD_BALANCE_CORE_HAPPINESS && strStrategyName == "AICITYSTRATEGY_NEED_HAPPINESS_GOLD")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessGold(GetCity());
				else if(MOD_BALANCE_CORE_HAPPINESS && strStrategyName == "AICITYSTRATEGY_NEED_HAPPINESS_CONNECTION")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessConnection(GetCity());
				else if(MOD_BALANCE_CORE_HAPPINESS && strStrategyName == "AICITYSTRATEGY_NEED_HAPPINESS_PILLAGE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessPillage(GetCity());
				else if(MOD_BALANCE_CORE_HAPPINESS && strStrategyName == "AICITYSTRATEGY_NEED_HAPPINESS_RELIGION")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessReligion(GetCity());
				else if(MOD_BALANCE_CORE_HAPPINESS && strStrategyName == "AICITYSTRATEGY_NEED_HAPPINESS_STARVE")
					bStrategyShouldBeActive = CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessStarve(GetCity());
#endif

				// Check Lua hook
				ICvEngineScriptSystem1* pkScriptSystem = gDLL->GetScriptSystem();
				if(pkScriptSystem && bStrategyShouldBeActive)
				{
					CvLuaArgsHandle args;
					args->Push(iCityStrategiesLoop);
					args->Push(GetCity()->getOwner());
					args->Push(GetCity()->GetID());

					// Attempt to execute the game events.
					// Will return false if there are no registered listeners.
					bool bResult = false;
					if(LuaSupport::CallTestAll(pkScriptSystem, "CityStrategyCanActivate", args.get(), bResult))
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

		for(uint uiPlots = 0; uiPlots < MAX_CITY_PLOTS - 1; uiPlots++)
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


	int iPopulationToEvaluate = min(m_pCity->getPopulation() + 2, m_pCity->GetNumWorkablePlots());
	CvPlot* pPlot = NULL;
	uint uiPlotsEvaluated = 0;

	struct ReverseSort
	{
		bool operator()(unsigned char ucA, unsigned char ucB)
		{
			return ucA > ucB;
		}
	};


	for(int iPlotLoop = 0; iPlotLoop < m_pCity->GetNumWorkablePlots(); iPlotLoop++)
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

		CvCity* pOwningCity = pPlot->getOwningCity();
		if(!pOwningCity || pOwningCity->GetID() != m_pCity->GetID())
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
#if defined(MOD_BUGFIX_MINOR)
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield) * pCityBuildings->GetNumBuilding(eBuilding);
#else
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield);
#endif
					}
				}
			}
			iYieldSum += iCityYieldSum * 100;
		}
#if defined(MOD_BALANCE_CORE)
		// add in additional gold from the city plot and the city buildings that provide food
		if(iYield == YIELD_GOLD)
		{
			int iCityYieldSum = 0;
			iCityYieldSum = m_pCity->plot()->getYield(YIELD_GOLD);
			CvYieldInfo* pYield = GC.getYieldInfo(YIELD_GOLD);
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
#if defined(MOD_BUGFIX_MINOR)
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield) * pCityBuildings->GetNumBuilding(eBuilding);
#else
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield);
#endif
					}
				}
			}
			iYieldSum += iCityYieldSum * 100;
		}
		// add in additional production from the city plot and the city buildings that provide food
		if(iYield == YIELD_PRODUCTION)
		{
			int iCityYieldSum = 0;
			iCityYieldSum = m_pCity->plot()->getYield(YIELD_PRODUCTION);
			CvYieldInfo* pYield = GC.getYieldInfo(YIELD_PRODUCTION);
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
#if defined(MOD_BUGFIX_MINOR)
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield) * pCityBuildings->GetNumBuilding(eBuilding);
#else
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield);
#endif
					}
				}
			}
			iYieldSum += iCityYieldSum * 100;
		}
		// add in additional science from the city plot and the city buildings that provide food
		if(iYield == YIELD_SCIENCE)
		{
			int iCityYieldSum = 0;
			iCityYieldSum = m_pCity->plot()->getYield(YIELD_SCIENCE);
			CvYieldInfo* pYield = GC.getYieldInfo(YIELD_SCIENCE);
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
#if defined(MOD_BUGFIX_MINOR)
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield) * pCityBuildings->GetNumBuilding(eBuilding);
#else
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield);
#endif
					}
				}
			}
			iYieldSum += iCityYieldSum * 100;
		}
		// add in additional science from the city plot and the city buildings that provide food
		if(iYield == YIELD_CULTURE)
		{
			int iCityYieldSum = 0;
			iCityYieldSum = m_pCity->plot()->getYield(YIELD_CULTURE);
			CvYieldInfo* pYield = GC.getYieldInfo(YIELD_CULTURE);
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
#if defined(MOD_BUGFIX_MINOR)
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield) * pCityBuildings->GetNumBuilding(eBuilding);
#else
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield);
#endif
					}
				}
			}
			iYieldSum += iCityYieldSum * 100;
		}
		// add in additional science from the city plot and the city buildings that provide food
		if(iYield == YIELD_FAITH)
		{
			int iCityYieldSum = 0;
			iCityYieldSum = m_pCity->plot()->getYield(YIELD_FAITH);
			CvYieldInfo* pYield = GC.getYieldInfo(YIELD_FAITH);
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
#if defined(MOD_BUGFIX_MINOR)
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield) * pCityBuildings->GetNumBuilding(eBuilding);
#else
						iCityYieldSum += pkBuildingInfo->GetYieldChange(iYield);
#endif
					}
				}
			}
			iYieldSum += iCityYieldSum * 100;
		}
#endif
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
#if defined(MOD_BALANCE_CORE)
			case CITY_AI_FOCUS_TYPE_PRODUCTION:
				strLookup = "CITYSPECIALIZATION_MILITARY_TRAINING";
				break;
			case CITY_AI_FOCUS_TYPE_SCIENCE:
				strLookup = "CITYSPECIALIZATION_SCIENCE";
				break;
			case CITY_AI_FOCUS_TYPE_CULTURE:
				strLookup = "CITYSPECIALIZATION_CULTURE";
				break;
			case CITY_AI_FOCUS_TYPE_GREAT_PEOPLE:
				strLookup = "CITYSPECIALIZATION_CULTURE";
				break;
			case CITY_AI_FOCUS_TYPE_FAITH:
				strLookup = "CITYSPECIALIZATION_FAITH";
				break;
			case NO_CITY_AI_FOCUS_TYPE:
				strLookup = "CITYSPECIALIZATION_GENERAL_ECONOMIC";
				break;
			case CITY_AI_FOCUS_TYPE_PROD_GROWTH:
				strLookup = "CITYSPECIALIZATION_GENERAL_ECONOMIC";
				break;
			case CITY_AI_FOCUS_TYPE_GOLD_GROWTH:
				strLookup = "CITYSPECIALIZATION_COMMERCE";
				break;
#endif
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
		pLog = LOGFILEMGR.GetLog(GetProductionLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

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
#if defined(MOD_BALANCE_CORE)
/// Recompute weights taking into account Production cost
void CvCityStrategyAI::ReweightPreCheckByCost()
{
	CvCityBuildable buildable;

	for(int iI = 0; iI < m_BuildablesPrecheck.size(); iI++)
	{
		buildable = m_BuildablesPrecheck.GetElement(iI);

		// Compute the new weight and change it
		int iNewWeight = CityStrategyAIHelpers::ReweightByTurnsLeft(m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
		m_BuildablesPrecheck.SetWeight(iI, iNewWeight);
	}
}
#endif

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
#if defined(MOD_BALANCE_CORE)
void CvCityStrategyAI::LogPossibleHurries()
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
		pLog = LOGFILEMGR.GetLog(GetHurryLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", PRE: ";

		// Dump out the weight of each buildable item
		for(int iI = 0; iI < m_BuildablesPrecheck.size(); iI++)
		{
			CvCityBuildable buildable = m_BuildablesPrecheck.GetElement(iI);

			switch(buildable.m_eBuildableType)
			{
			case CITY_BUILDABLE_BUILDING:
			{
				CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Building, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Unit, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROJECT:
			{
				CvProjectEntry* pEntry = GC.GetGameProjects()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Project, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROCESS:
			{
				CvProcessInfo* pProcess = GC.getProcessInfo((ProcessTypes)buildable.m_iIndex);
				if (pProcess != NULL)
				{
					strDesc = pProcess->GetDescription();
					strTemp.Format("Process, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_OPERATION:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Operation unit, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_ARMY:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Army unit, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			}
			strOutBuf = strBaseString + strTemp;
			pLog->Msg(strOutBuf);
		}
	}
}
void CvCityStrategyAI::LogPossibleHurriesPostCheck()
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
		pLog = LOGFILEMGR.GetLog(GetHurryLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", POST: ";

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
void CvCityStrategyAI::LogPossibleBuildsPostCheck()
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
		pLog = LOGFILEMGR.GetLog(GetProductionLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", POST: ";

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
void CvCityStrategyAI::LogHurryMessage(CvString& strMsg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strTemp, szTemp2;
		CvString playerName;
		CvString cityName;

		// Find the name of this civ and city
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		// Open the log file
		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetHurryLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";
		strOutBuf = strBaseString + strMsg;
		pLog->Msg(strOutBuf);
	}
}
#endif
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
		pLog = LOGFILEMGR.GetLog(GetProductionLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", PRE: ";

		// Dump out the weight of each buildable item
		for(int iI = 0; iI < m_BuildablesPrecheck.size(); iI++)
		{
			CvCityBuildable buildable = m_BuildablesPrecheck.GetElement(iI);

			switch(buildable.m_eBuildableType)
			{
			case CITY_BUILDABLE_BUILDING:
			{
				CvBuildingEntry* pEntry = GC.GetGameBuildings()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Building, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Unit, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROJECT:
			{
				CvProjectEntry* pEntry = GC.GetGameProjects()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Project, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_PROCESS:
			{
				CvProcessInfo* pProcess = GC.getProcessInfo((ProcessTypes)buildable.m_iIndex);
				if (pProcess != NULL)
				{
					strDesc = pProcess->GetDescription();
					strTemp.Format("Process, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_OPERATION:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Operation unit, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
				}
			}
			break;
			case CITY_BUILDABLE_UNIT_FOR_ARMY:
			{
				CvUnitEntry* pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
				if(pEntry != NULL)
				{
					strDesc = pEntry->GetDescription();
					strTemp.Format("Army unit, %s, %d, %d", strDesc.GetCString(), m_BuildablesPrecheck.GetWeight(iI), buildable.m_iTurnsToConstruct);
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
		pLog = LOGFILEMGR.GetLog(GetProductionLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		CvBaseInfo* pEntry = NULL;
		CvString strType = "Unknown";
		int iEra = -1;
		switch(buildable.m_eBuildableType)
		{
		case CITY_BUILDABLE_BUILDING:
		{
			CvBuildingEntry* pInfo = GC.GetGameBuildings()->GetEntry(buildable.m_iIndex);
			strType = (pInfo->GetBuildingClassInfo().getMaxGlobalInstances() != -1 || pInfo->GetBuildingClassInfo().getMaxPlayerInstances() != -1) ? "Wonder" : "Building";

			TechTypes eTech = (TechTypes)pInfo->GetPrereqAndTech();
			if (eTech != NO_TECH)
			{
				CvTechEntry* pTech = GC.GetGameTechs()->GetEntry(eTech);
				if (pTech && pTech->GetEra() != NO_ERA)
					iEra = pTech->GetEra();
			}

			pEntry = pInfo;
			break;
		}
		case CITY_BUILDABLE_UNIT:
		case CITY_BUILDABLE_UNIT_FOR_OPERATION:
		case CITY_BUILDABLE_UNIT_FOR_ARMY:
		{
			CvUnitEntry* pInfo = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
			strType = (pInfo->GetCombat() > 0 || pInfo->GetRangedCombat() > 0) ? "Military" : "Civilian";

			TechTypes eTech = (TechTypes)pInfo->GetPrereqAndTech();
			if (eTech != NO_TECH)
			{
				CvTechEntry* pTech = GC.GetGameTechs()->GetEntry(eTech);
				if (pTech && pTech->GetEra() != NO_ERA)
					iEra = pTech->GetEra();
			}

			pEntry = pInfo;
			break;
		}
		case CITY_BUILDABLE_PROJECT:
			pEntry = GC.GetGameProjects()->GetEntry(buildable.m_iIndex);
			strType = "Project";
			iEra = GC.getGame().getCurrentEra();
			break;
		case CITY_BUILDABLE_PROCESS:
			pEntry = GC.getProcessInfo((ProcessTypes)buildable.m_iIndex);
			strType = "Process";
			iEra = GC.getGame().getCurrentEra();
			break;
		}

		if (pEntry != NULL)
			strDesc = pEntry->GetDescription();

		strTemp.Format("SEED: %I64u, CHOSEN: %s, %s, %s, ERA: %d, TURNS: %d", GC.getGame().getJonRand().getSeed(), 
			strType.c_str(), strDesc.c_str(), bRush?"Rush":"NoRush", iEra, buildable.m_iTurnsToConstruct);

		strOutBuf = strBaseString + strTemp;
		pLog->Msg(strOutBuf);
	}
}

void CvCityStrategyAI::LogInvalidItem(CvCityBuildable buildable, int iVal)
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
		playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
		cityName = m_pCity->getName();

		FILogFile* pLog;
		pLog = LOGFILEMGR.GetLog(GetProductionLogFileName(playerName, cityName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += playerName + ", " + cityName + ", ";

		CvBaseInfo* pEntry = NULL;
		switch (buildable.m_eBuildableType)
		{
		case CITY_BUILDABLE_BUILDING:
			pEntry = GC.GetGameBuildings()->GetEntry(buildable.m_iIndex);
			break;
		case CITY_BUILDABLE_UNIT:
		case CITY_BUILDABLE_UNIT_FOR_OPERATION:
		case CITY_BUILDABLE_UNIT_FOR_ARMY:
			pEntry = GC.GetGameUnits()->GetEntry(buildable.m_iIndex);
			break;
		case CITY_BUILDABLE_PROJECT:
			pEntry = GC.GetGameProjects()->GetEntry(buildable.m_iIndex);
			break;
		case CITY_BUILDABLE_PROCESS:
			pEntry = GC.getProcessInfo((ProcessTypes)buildable.m_iIndex);
			break;
		}

		if (pEntry != NULL)
			strDesc = pEntry->GetDescription();

		strTemp.Format("SKIPPED: %s, Val: %d, TURNS: %d",
			strDesc.c_str(), iVal, buildable.m_iTurnsToConstruct);

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
	// Never consider the capital to be a medium city (so with late game starts at least one city retains high flavors for SPACESHIP, etc.)
	if (pCity->isCapital())
	{
		return false;
	}

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
#if defined(MOD_BALANCE_CORE)
/// "Lakebound" City Strategy: If a City has no access to actual Ocean, reduce all water-based Flavors
bool CityStrategyAIHelpers::IsTestCityStrategy_Lakebound(CvCity* pCity)
{
	bool bHaveLake = false;
	bool bHaveOcean = false;

	std::vector<int> areas = pCity->plot()->getAllAdjacentAreas();
	for (std::vector<int>::iterator it=areas.begin(); it!=areas.end(); ++it)
	{
		CvArea* pkArea = GC.getMap().getArea(*it);
		if (pkArea->isWater())
		{
			if (pkArea->getNumTiles() <= GC.getLAKE_MAX_AREA_SIZE())
				bHaveLake = true;
			else
				bHaveOcean = true;
		}
	}

	return bHaveLake && !bHaveOcean;
}
#endif

/// "Need Tile Improvers" City Strategy: Do we REALLY need to train some Workers?
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
	int iCurrentNumCities = kPlayer.countCitiesNeedingTerrainImprovements();

	int iLastTurnWorkerDisbanded = kPlayer.GetEconomicAI()->GetLastTurnWorkerDisbanded();
	if(iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 40)
	{
		return false;
	}
	int iNumWorkers = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);

	// If it's a minor with at least 1 worker per city, always return false
	if(kPlayer.isMinorCiv())
	{
		if(iNumWorkers >= iCurrentNumCities)
			return false;
#if defined(MOD_MINOR_CIV_EXTENDED)
		// if we lost our worker, rebuild-it asap. (>50 turn in normal speed)
		if (iNumWorkers == 0 && GC.getGame().getElapsedGameTurns()
			> GC.getGame().getGameSpeedInfo().getTrainPercent() / 2)
			return true;
#endif
	}
	else
	{
		//Do we have more workers than cities already?
		if(iNumWorkers > iCurrentNumCities+1)
			return false;

		// If we're losing at war, return false
		if(kPlayer.GetDiplomacyAI()->GetStateAllWars() == STATE_ALL_WARS_LOSING)
			return false;
	}

	// If we're under attack from Barbs and have 1 or fewer Cities and no credible defense then training more Workers will only hurt us
	if(iCurrentNumCities <= 4)
	{
		CvMilitaryAI* pMilitaryAI =kPlayer.GetMilitaryAI();
		MilitaryAIStrategyTypes eStrategyKillBarbs = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ERADICATE_BARBARIANS");
		if(eStrategyKillBarbs != NO_MILITARYAISTRATEGY)
		{
			if(pMilitaryAI->IsUsingStrategy(eStrategyKillBarbs))
			{
				//Do we have enough military units to defend our land? No? Abort.
				int iNumMilitaryUnits = kPlayer.getNumMilitaryUnits();
				if((iNumWorkers * 6) >= iNumMilitaryUnits)
					return false;
			}
		}
	}

	CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);

	int iModdedNumWorkers = iNumWorkers * /*67*/ pCityStrategy->GetWeightThreshold() / 100;

	// We have fewer than we think we should, or we have none at all
	if(iModdedNumWorkers <= iCurrentNumCities || iModdedNumWorkers == 0)
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

	int iNumBuilders = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
	if(iNumBuilders <= 0)
	{
		return true;
	}

	int iCurrentNumCities = kPlayer.countCitiesNeedingTerrainImprovements();
	if(iNumBuilders >= iCurrentNumCities)
	{
		return false;
	}

	// Don't get desperate for training a Builder here unless the City is at least of a certain size
	if(pCity->getPopulation() >= /*2*/ GC.getAI_CITYSTRATEGY_WANT_TILE_IMPROVERS_MINIMUM_SIZE())
	{
		// If we don't even have 1 builder on map or in a queue, turn this on immediately
		if(iNumBuilders < 1)
			return true;

		CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);
		if(iNumBuilders < iCurrentNumCities * pCityStrategy->GetWeightThreshold()) // limit to x builders per city
			return true;
	}

	return false;
}

/// "Enough Tile Improvers" City Strategy: This is not a Player Strategy because we only want to prevent the training of new Builders, not nullify new Techs or Policies, which could still be very useful
bool CityStrategyAIHelpers::IsTestCityStrategy_EnoughTileImprovers(AICityStrategyTypes eStrategy, CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
	int iLastTurnWorkerDisbanded = kPlayer.GetEconomicAI()->GetLastTurnWorkerDisbanded();
	if(iLastTurnWorkerDisbanded >= 0 && GC.getGame().getGameTurn() - iLastTurnWorkerDisbanded <= 20)
	{
		return true;
	}
	
	int iNumBuilders = kPlayer.GetNumUnitsWithUnitAI(UNITAI_WORKER, true, false);
	if (iNumBuilders <= 0)
		return false;

	AICityStrategyTypes eNeedImproversStrategy = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_NEED_TILE_IMPROVERS");
	if(eNeedImproversStrategy != NO_ECONOMICAISTRATEGY)
	{
		if(pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedImproversStrategy))
			return false;
	}

	// If it's a minor with at least 1 worker per city, always return true
	if(GET_PLAYER(pCity->getOwner()).isMinorCiv())
	{
		if(iNumBuilders >= kPlayer.getNumCities())
			return true;
#if defined(MOD_MINOR_CIV_EXTENDED)
		// if we lost our worker, rebuild-it asap. (>50 turn in normal speed)
		if (iNumBuilders == 0 && GC.getGame().getElapsedGameTurns()
			> GC.getGame().getGameSpeedInfo().getTrainPercent() / 2)
			return false;
#endif
	}

	CvAICityStrategyEntry* pCityStrategy = pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy);

	int iWeightThresholdModifier = GetWeightThresholdModifier(eStrategy, pCity);	// 10 Extra Weight per TILE_IMPROVEMENT Flavor
	int iPerCityThreshold = pCityStrategy->GetWeightThreshold() + iWeightThresholdModifier;	// 100
	int iNumCities = kPlayer.countCitiesNeedingTerrainImprovements();

	// Average Player wants no more than 1.50 Builders per City [150 Weight is Average; range is 100 to 200]
	if((iNumBuilders * 100) >= iPerCityThreshold*iNumCities)
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

	for(int iPlotLoop = 0; iPlotLoop < pCity->GetNumWorkablePlots(); iPlotLoop++)
	{
		pLoopPlot = iterateRingPlots(pCity->getX(), pCity->getY(), iPlotLoop);

		if(pLoopPlot != NULL)
		{
			if(pLoopPlot->getOwner() == pCity->getOwner())
			{
				if(!pLoopPlot->isCity())
				{
					iNumTotalWorkablePlots++;

					if(pLoopPlot->isWater() && !pLoopPlot->isLake())
					{
						iNumOceanPlots++;
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

	for(int iPlotLoop = 0; iPlotLoop < pCity->GetNumWorkablePlots(); iPlotLoop++)
	{
		pLoopPlot = iterateRingPlots(pCity->getX(), pCity->getY(), iPlotLoop);

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
#if defined(MOD_BALANCE_CORE)
						if(pLoopPlot->getResourceType(pCity->getTeam()) != NO_RESOURCE && pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
#else
						if(pLoopPlot->getResourceType() != NO_RESOURCE && pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
#endif
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
#if defined(MOD_BALANCE_CORE)
	int iX = pCity->getX(); int iY = pCity->getY(); int iOwner = pCity->getOwner();

	int iNumWorkersHere = 0;
	int iCanImprove = 0;
	for (int iCityPlotLoop = 0; iCityPlotLoop < RING5_PLOTS; iCityPlotLoop++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(iX, iY, iCityPlotLoop);

		// Invalid plot or not owned by this player
		if (pLoopPlot == NULL || pLoopPlot->getOwner() != iOwner || pLoopPlot->getOwningCity() != pCity)
		{
			continue;
		}
		//No improved, no impassable, water only.
		if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT && !pLoopPlot->isImpassable() && pLoopPlot->isWater())
		{
			CvUnit* pLoopUnit;
			for(int iUnitLoop = 0; iUnitLoop < pLoopPlot->getNumUnits(); iUnitLoop++)
			{
				//Workers nearby?
				pLoopUnit = pLoopPlot->getUnitByIndex(iUnitLoop);
				if(pLoopUnit != NULL && pLoopUnit->getOwner() == pCity->GetID() && pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA)
				{
					iNumWorkersHere++;
				}
			}
			for(int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				CvBuildInfo* pkBuildInfo = GC.getBuildInfo((BuildTypes) iI);
				if(!pkBuildInfo)
				{
					continue;
				}
				ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo((BuildTypes) iI)->getImprovement();
				if(eImprovement != NO_IMPROVEMENT)
				{
					CvImprovementEntry* pkEntry = GC.getImprovementInfo(eImprovement);
					if(pkEntry->IsCreatedByGreatPerson())
						continue;
				}

				//Valid right now with any worker valid build?
				if(GET_PLAYER(pCity->getOwner()).canBuild(pLoopPlot, (BuildTypes)iI))
				{
					iCanImprove++;
					break;
				}
			}
		}
	}
	//No tiles to improve?
	if(iCanImprove <= 0)
	{
		return true;
	}
	//Enough workers already here? 1:1 ratio is good ratio.
	if(iNumWorkersHere > iCanImprove)
	{
		return true;
	}
#endif

	return false;
}
#if defined(MOD_BALANCE_CORE)
// Too many settlers!
bool CityStrategyAIHelpers::IsTestCityStrategy_EnoughSettlers(CvCity* pCity)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());

#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
	EconomicAIStrategyTypes eCanSettle = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_FOUND_CITY");
	if (!EconomicAIHelpers::CannotMinorCiv(&kPlayer, eCanSettle))
#else
	if(!kPlayer.isMinorCiv())
#endif
	{
		int iSettlersOnMapOrBuild = kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE, true, true);
		//Too many settlers? Stop building them!
		if(iSettlersOnMapOrBuild >= 2)
		{
			return true;
		}
		MilitaryAIStrategyTypes eBuildCriticalDefenses = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_LOSING_WARS");
		// scale based on flavor and world size
		if(eBuildCriticalDefenses != NO_MILITARYAISTRATEGY && kPlayer.GetMilitaryAI()->IsUsingStrategy(eBuildCriticalDefenses))
		{
			if(iSettlersOnMapOrBuild > 0)
			{
				return true;
			}
		}
	}
	return false;
}
// We a new city on a bigger continent? Let's spread our legs!
#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
bool CityStrategyAIHelpers::IsTestCityStrategy_NewContinentFeeder(AICityStrategyTypes eStrategy, CvCity* pCity)
#else
bool CityStrategyAIHelpers::IsTestCityStrategy_NewContinentFeeder(CvCity* pCity)
#endif
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());
	CvArea* pArea = GC.getMap().getArea(pCity->getArea());
	if(pCity->getPopulation() <= 6)
	{
		return false;
	}
	if(pArea == NULL || pArea->getNumTiles() <= 10)
	{
		return false;
	}
#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
	if (!( kPlayer.isMinorCiv() && pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy)->IsNoMinorCivs() )
		&& kPlayer.getCapitalCity() != NULL)
#else
	if(!kPlayer.isMinorCiv() && kPlayer.getCapitalCity() != NULL)
#endif
	{
		CvArea* pArea2 = GC.getMap().getArea(kPlayer.getCapitalCity()->getArea());
		if(pArea != NULL && pArea->GetID() != pArea2->GetID())
		{
			//Is there more room here to grow?
			if((pArea->getNumUnownedTiles() > pArea2->getNumUnownedTiles()) && (pArea->GetNumBadPlots() < pArea2->getNumUnownedTiles()))
			{
				if(kPlayer.HaveGoodSettlePlot(pArea->GetID()))
				{
					return true;
				}
			}
		}	
	}
	return false;
}
// Is this an isolated city with no land routes out? Maybe open border with neighbors could help
bool CityStrategyAIHelpers::IsTestCityStrategy_PocketCity(CvCity* pCity)
{
	if(!pCity)
		return false;

	if(pCity->isCapital())
		return false;

	CvCity* pCapitalCity = GET_PLAYER(pCity->getOwner()).getCapitalCity();
	if(!pCapitalCity)
		return false;

	CvArea* pArea = GC.getMap().getArea(pCity->getArea());
	if(pArea->GetID() != pCapitalCity->getArea())
		return false;

	//do we already have a connection to the capital?
	if (pCity->IsRouteToCapitalConnected())
		return false;

	//could we build a route?
	SPathFinderUserData data(pCity->getOwner(), PT_BUILD_ROUTE, ROUTE_ANY);
	return !GC.GetStepFinder().DoesPathExist(pCapitalCity->getX(), pCapitalCity->getY(), pCity->getX(), pCity->getY(), data);
}
#endif

/// "Need Improvement" City Strategy: if we need to get an improvement that increases a yield amount
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedImprovement(CvCity* pCity, YieldTypes yield)
{
#if defined(MOD_BALANCE_CORE)
	if(pCity->GetCityStrategyAI()->GetMostDeficientYield() == yield)
#else
	if(pCity->GetCityStrategyAI()->GetDeficientYield() == yield)
#endif
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

#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
		if (! (kPlayer.isMinorCiv() && pCity->GetCityStrategyAI()->GetAICityStrategies()->GetEntry(eStrategy)->IsNoMinorCivs()))
#else
		if(!kPlayer.isMinorCiv())
#endif
		{
			int iNumCities = kPlayer.getNumCities();
			int iSettlersOnMapOrBuild = kPlayer.GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);
			int iCitiesPlusSettlers = iNumCities + iSettlersOnMapOrBuild;

#if defined(MOD_BALANCE_CORE)
			bool bIsVenice = kPlayer.GetPlayerTraits()->IsNoAnnexing();
			//City #2 is essential.
			if(!bIsVenice && (iCitiesPlusSettlers <= 1))
			{
				return true;
			}
#endif
#if defined(MOD_BALANCE_CORE)
			if((iCitiesPlusSettlers > 0) && (iCitiesPlusSettlers < 6))
#else
			if((iCitiesPlusSettlers) < 3)
#endif
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
					(iCitiesPlusSettlers == 3 && iGameTurn > iWeightThreshold) 
#if defined(MOD_BALANCE_CORE)
					|| (iCitiesPlusSettlers == 4 && (iGameTurn / 2) > iWeightThreshold) 
					|| (iCitiesPlusSettlers == 5 && (iGameTurn / 4) > iWeightThreshold) 
#endif
					)
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
			CvCity *pMostThreatened = kPlayer.GetThreatenedCityByRank();
			//threat value is now calculated differently
			if (pMostThreatened == pCity && pMostThreatened->getThreatValue() > GC.getCITY_HIT_POINTS_HEALED_PER_TURN()*2)
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

	if (GC.getGame().GetGameReligions()->GetNumReligionsStillToFound() <= 0)
	{
		return false;
	}

	if (pCity->GetFaithPerTurnFromBuildings() > 0 && kPlayer.GetReligions()->HasCreatedPantheon())
	{
		return false;
	}

	return true;
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
#if defined(MOD_BALANCE_CORE)
	if(pCity->IsPuppet() && !GET_PLAYER(pCity->getOwner()).GetPlayerTraits()->IsNoAnnexing())
#else
	if(pCity->IsPuppet())
#endif
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
	return pCity->isCoastal();
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
	int iCityScienceOutput = pCity->getYieldRateTimes100(YIELD_SCIENCE, false);

	for(pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		iNumOtherCities++;
		// don't evaluate ourselves
		if (pLoopCity == pCity)
		{
			continue;
		}

		if (pLoopCity->getYieldRateTimes100(YIELD_SCIENCE, false) > iCityScienceOutput)
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

				ReligionTypes eMajority = pCity->GetCityReligions()->GetReligiousMajority();
				if (eMajority != NO_RELIGION)
				{
					const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pCity->getOwner());
					if (pReligion)
					{
						iGPPChange += pReligion->m_Beliefs.GetGreatPersonPoints(GetGreatPersonFromSpecialist(eSpecialist), pCity->getOwner(), pCity, true) * 100;
					}
				}

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES) && defined(MOD_API_LUA_EXTENSIONS)
				// GPP from resource monopolies
				GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
				if (eGreatPerson != NO_GREATPERSON)
				{
					iGPPChange += pCity->GetPlayer()->getSpecificGreatPersonRateChangeFromMonopoly(eGreatPerson) * 100;
				}
#endif

				if (iGPPChange > 0)
				{
					int iMod = 0;

					// City mod
					iMod += pCity->getGreatPeopleRateModifier();

					// Player mod
					iMod += pCity->GetPlayer()->getGreatPeopleRateModifier();

					iMod += pCity->GetPlayer()->GetPlayerTraits()->GetWLTKDGPImprovementModifier() * 10;

#if defined(MOD_API_LUA_EXTENSIONS)
					GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
					if(eGreatPerson != NO_GREATPERSON)
					{
						iMod += pCity->GetPlayer()->getSpecificGreatPersonRateModifierFromMonopoly(eGreatPerson);
						if(pCity->GetPlayer()->isGoldenAge())
						{
							GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
							if(eGreatPerson != NO_GREATPERSON)
							{
								iMod += pCity->GetPlayer()->getGoldenAgeGreatPersonRateModifier(eGreatPerson);
								iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);

								iMod += pCity->GetPlayer()->GetPlayerTraits()->GetWLTKDGPImprovementModifier() * 10;

								ReligionTypes eMajority = pCity->GetCityReligions()->GetReligiousMajority();
								if(eMajority != NO_RELIGION)
								{
									const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pCity->getOwner());
									if(pReligion)
									{
										iMod += pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGreatPerson, pCity->getOwner(), pCity);
										BeliefTypes eSecondaryPantheon = pCity->GetCityReligions()->GetSecondaryReligionPantheonBelief();
										if (eSecondaryPantheon != NO_BELIEF)
										{
											iMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
										}
									}
								}
							}
						}
						if (pCity->GetPlayer()->GetPlayerTraits()->GetGreatPersonGWAM(eGreatPerson) > 0)
						{
							iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGreatPersonGWAM(eGreatPerson);
						}
						if (pCity->GetPlayer()->GetPlayerTraits()->GetCityConquestGWAM() > 0)
						{
							iMod += pCity->GetPlayer()->GetPlayerTraits()->GetCityConquestGWAM();
						}
						int iNumPuppets = pCity->GetPlayer()->GetNumPuppetCities();
						if(iNumPuppets > 0)
						{
							iMod += (iNumPuppets * pCity->GetPlayer()->GetPlayerTraits()->GetPerPuppetGreatPersonRateModifier(eGreatPerson));			
						}
					}
#endif
					if (pCity->isCapital() && GET_PLAYER(pCity->getOwner()).IsDiplomaticMarriage())
					{
						int iNumMarried = 0;
						// Loop through all minors and get the total number we've met.
						for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
						{
							PlayerTypes eMinor = (PlayerTypes) iPlayerLoop;

							if (eMinor != pCity->GetPlayer()->GetID() && GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
							{
								if (!GET_PLAYER(eMinor).IsAtWarWith(pCity->GetPlayer()->GetID()) && GET_PLAYER(eMinor).GetMinorCivAI()->IsMarried(pCity->GetPlayer()->GetID()))
								{
									iNumMarried++;
								}
							}
						}
						if(iNumMarried > 0)
						{
							iMod += (iNumMarried * GC.getBALANCE_MARRIAGE_GP_RATE());
						}
					}

					// Trait mod to this specific class
					if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
					{
						iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGreatScientistRateModifier();
						iMod += pCity->GetPlayer()->getGreatScientistRateModifier();
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_WRITER"))
					{
						iMod += pCity->GetPlayer()->getGreatWriterRateModifier();
						if (pCity->GetPlayer()->isGoldenAge())
						{
							iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatWriterRateModifier();
						}
						if (pCity->GetPlayer()->GetPlayerTraits()->IsGreatWorkWLTKD())
						{
							iMod += 25;
						}
					}					
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
					{
						iMod += pCity->GetPlayer()->getGreatArtistRateModifier();
						if (pCity->GetPlayer()->isGoldenAge())
						{
							iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatArtistRateModifier();
						}
						if (pCity->GetPlayer()->GetPlayerTraits()->IsGreatWorkWLTKD())
						{
							iMod += 25;
						}
					}					
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
					{
						iMod += pCity->GetPlayer()->getGreatMusicianRateModifier();
						if (pCity->GetPlayer()->isGoldenAge())
						{
							iMod += pCity->GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatMusicianRateModifier();
						}
						if (pCity->GetPlayer()->GetPlayerTraits()->IsGreatWorkWLTKD())
						{
							iMod += 25;
						}
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
					{
						iMod += pCity->GetPlayer()->getGreatMerchantRateModifier();
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
					{
						iMod += pCity->GetPlayer()->getGreatEngineerRateModifier();
					}
#if defined(MOD_DIPLOMACY_CITYSTATES)
					else if(MOD_DIPLOMACY_CITYSTATES && (UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
					{
						iMod += pCity->GetPlayer()->getGreatDiplomatRateModifier();
					}
#endif

					iGPPChange *= (100 + iMod);
					iGPPChange /= 100;

					iTotalGPPChange += iGPPChange;
				}
			}
		}
	}
#if defined(MOD_BALANCE_CORE)
	if (iTotalGPPChange >= 2500)
#else
	if (iTotalGPPChange >= 800)
#endif
	{
		return true;
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_NeedInternationalTradeRoute (CvCity* pCity, DomainTypes eDomain)
{
	PlayerTypes ePlayer = pCity->getOwner();
	CvPlayerTrade* pTrade = GET_PLAYER(ePlayer).GetTrade();

	if (pTrade->GetNumTradeUnitsRemaining(true) <= 0)
	{
		return false;
	}

	if (pTrade->GetNumPotentialConnections(pCity, eDomain, true) <= 0)
	{
		return false;
	}

	return true;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_NoNeedInternationalTradeRoute (CvCity* pCity, DomainTypes eDomain)
{
	PlayerTypes ePlayer = pCity->getOwner();
	CvPlayerTrade* pTrade = GET_PLAYER(ePlayer).GetTrade();

	if (pTrade->GetNumTradeUnitsRemaining(true) <= 0)
	{
		return true;
	}

	if (pTrade->GetNumPotentialConnections(pCity, eDomain, true) <= 0)
	{
		return true;
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_IsInternationalTradeDestination(CvCity* pCity)
{
	int iNumTimesDestination = GC.getGame().GetGameTrade()->GetNumTimesDestinationCity(pCity, true);
	if (iNumTimesDestination >= 2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CityStrategyAIHelpers::IsTestCityStrategy_IsInternationalTradeOrigin(CvCity* pCity)
{
	int iNumTimesOrigin = GC.getGame().GetGameTrade()->GetNumTimesOriginCity(pCity, true);
	if (iNumTimesOrigin >= 2)
	{
		return true;
	}
	else
	{
		return false;
	}	
}

bool CityStrategyAIHelpers::IsTestCityStrategy_NeedCultureBuilding(CvCity *pCity)
{
	CvPlayer &kPlayer = GET_PLAYER(pCity->getOwner());

	GreatWorkSlotType eSlotType = pCity->GetCityCulture()->GetSlotTypeFirstAvailableCultureBuilding();

	if (eSlotType != NO_GREAT_WORK_SLOT)
	{
		int iSlotsOpen = kPlayer.GetCulture()->GetNumGreatWorkSlots(eSlotType);

		if (iSlotsOpen <= 2)
		{
			return true;
		}
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_NeedTourismBuilding(CvCity *pCity)
{
	int iTourismValue = 0;
	iTourismValue += pCity->getYieldRate(YIELD_CULTURE, false);
#if defined(MOD_BALANCE_CORE)
	iTourismValue += pCity->GetBaseTourism() / 100;
#else
	iTourismValue += pCity->GetCityCulture()->GetBaseTourism();
#endif

	if (iTourismValue > 10)
	{
		return true;
	}

	return false;
}

bool CityStrategyAIHelpers::IsTestCityStrategy_GoodAirliftCity(CvCity *pCity)
{
	if (pCity->isCapital())
	{
		return true;
	}

	CvPlayer &kPlayer = GET_PLAYER(pCity->getOwner());
	CvCity *pCapital = kPlayer.getCapitalCity();
#if defined(MOD_BALANCE_CORE)
	if (pCity && pCapital && pCity->getArea() != pCapital->getArea())
#else
	if (pCity && pCity->getArea() != pCapital->getArea())
#endif
	{
		return true;
	}

	if (pCapital && pCity && plotDistance(pCity->getX(), pCity->getY(), pCapital->getX(), pCapital->getY()) > 20)
	{
		return true;
	}

	return false;
}

#if defined(MOD_DIPLOMACY_CITYSTATES)
/// Do we need more Diplomatic Units? Check and see.
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedDiplomats(CvCity *pCity)
{
	PlayerTypes ePlayer = pCity->getOwner();
	EconomicAIStrategyTypes eStrategyNeedDiplomats = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS");

	bool bHasDiploBuilding = false;

	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if(pkBuildingInfo)
		{
			// Has this Building
			if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Does it grant a diplomatic production bonus?
				if (pkBuildingInfo->GetBuildingClassType() == (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_SCRIBE"))
				{
					bHasDiploBuilding = true;
				}

				if (bHasDiploBuilding)
				{
					//Let's make sure the city is robust before we start this.
					if(pCity->getPopulation() >= 6)
					{
						//Need diplomats?
						if(GET_PLAYER(ePlayer).GetEconomicAI()->IsUsingStrategy(eStrategyNeedDiplomats))
						{
							return true;
						}
					}
				}
			}
		}
	}
	if (pCity->isCapital() && pCity->getPopulation() >= 6)
	{
		//Need diplomats?
		if(GET_PLAYER(ePlayer).GetEconomicAI()->IsUsingStrategy(eStrategyNeedDiplomats))
		{
			return true;
		}
	}
	return false;
}

/// Do we REALLY need more Diplomatic Units? Check and see.
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedDiplomatsCritical(CvCity *pCity)
{
	PlayerTypes ePlayer = pCity->getOwner();
	EconomicAIStrategyTypes eStrategyNeedDiplomatsCritical = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_NEED_DIPLOMATS_CRITICAL");

	bool bHasDiploBuilding = false;

	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if(pkBuildingInfo)
		{
			// Has this Building
			if (pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Does it grant a diplomatic production bonus?
				if (pkBuildingInfo->GetBuildingClassType() == (BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_COURT_SCRIBE"))
				{
					bHasDiploBuilding = true;
				}

				if (bHasDiploBuilding)
				{
					//Let's make sure the city is robust before we start this.
					if(pCity->getPopulation() >= 5)
					{
						//Need diplomats?
						if(GET_PLAYER(ePlayer).GetEconomicAI()->IsUsingStrategy(eStrategyNeedDiplomatsCritical))
						{
							return true;
						}
					}
				}
			}
		}
	}
	if (pCity->isCapital() && pCity->getPopulation() >= 5)
	{
		//Need diplomats?
		if(GET_PLAYER(ePlayer).GetEconomicAI()->IsUsingStrategy(eStrategyNeedDiplomatsCritical))
		{
			return true;
		}
	}
	return false;
}
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS)
//Tests to help AI build buildings it needs.
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessCulture(CvCity *pCity)
{
	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv() && pCity->getUnhappinessFromCulture() > 0)
	{
		return true;
	}
	return false;
}
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessScience(CvCity *pCity)
{
	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv() && pCity->getUnhappinessFromScience() > 0)
	{
		return true;
	}
	return false;
}
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessDefense(CvCity *pCity)
{
	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv() && pCity->getUnhappinessFromDefense() > 0)
	{
		return true;
	}
	return false;
}
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessGold(CvCity *pCity)
{
	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv() && pCity->getUnhappinessFromGold() > 0)
	{
		return true;
	}
	return false;
}
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessConnection(CvCity *pCity)
{
	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv() && pCity->getUnhappinessFromConnection() > 0)
	{
		return true;
	}
	return false;
}
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessPillage(CvCity *pCity)
{
	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv() && pCity->getUnhappinessFromPillaged() > 0)
	{
		return true;
	}
	return false;
}
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessReligion(CvCity *pCity)
{
	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv() && pCity->getUnhappinessFromReligion() > 0)
	{
		return true;
	}
	return false;
}
bool CityStrategyAIHelpers::IsTestCityStrategy_NeedHappinessStarve(CvCity *pCity)
{
	if(!GET_PLAYER(pCity->getOwner()).isMinorCiv() && pCity->getUnhappinessFromStarving() > 0)
	{
		return true;
	}
	return false;
}
int CityStrategyAIHelpers::GetBuildingYieldValue(CvCity *pCity, BuildingTypes eBuilding, YieldTypes eYield)
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	int iYieldValue = 0;

	//Skip if null
	if (pkBuildingInfo == NULL)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(pCity->getOwner());

	int iEra = kPlayer.GetCurrentEra();

	int iYieldRate = max(1, pCity->getYieldRate(eYield, false));

	int iFlatYield = 0;
	int iModifier = 0;
	int iInstant = 0;

	//////////////
	// Flat Yields
	/////////////
	if (pkBuildingInfo->GetYieldChange(eYield) > 0)
	{
		iFlatYield += pkBuildingInfo->GetYieldChange(eYield);
	}
	if (pkBuildingInfo->GetYieldChangePerPop(eYield) > 0)
	{
		//Since this is going to grow, let's boost the pop by Era (earlier more: Anc x6, Cla x3, Med x2, Ren x1.5, Mod x1.2)
		int iValue = (pCity->getPopulation() * pkBuildingInfo->GetYieldChangePerPop(eYield) * 100) / (100 * (iEra + 1));

		if (iValue <= pkBuildingInfo->GetYieldChangePerPop(eYield))
			iValue = pkBuildingInfo->GetYieldChangePerPop(eYield);

		iFlatYield += iValue;
	}
#if defined(MOD_BALANCE_CORE)
	if (pkBuildingInfo->GetYieldChangePerPopInEmpire(eYield) > 0)
	{
		//Since this is going to grow, let's boost the pop by Era (earlier more: Anc x6, Cla x3, Med x2, Ren x1.5, Mod x1.2)
		int iValue = (GET_PLAYER(pCity->getOwner()).getTotalPopulation() * pkBuildingInfo->GetYieldChangePerPopInEmpire(eYield) * 100) / (100 * (iEra + 1));

		if (iValue <= pkBuildingInfo->GetYieldChangePerPopInEmpire(eYield))
			iValue = pkBuildingInfo->GetYieldChangePerPopInEmpire(eYield);

		iFlatYield += iValue;
	}
#endif
	if (pkBuildingInfo->GetYieldChangePerReligion(eYield) > 0)
	{
		int numReligions = pCity->GetCityReligions()->GetNumReligionsWithFollowers();
		int tempYield = (pkBuildingInfo->GetYieldChangePerReligion(eYield) * numReligions) / 100;
		iFlatYield += numReligions == 1 ? tempYield / 2 : tempYield;
	}

	if (pkBuildingInfo->GetThemingYieldBonus(eYield) > 0)
	{
		iFlatYield += (pkBuildingInfo->GetThemingYieldBonus(eYield) * 5);
	}

	if (pCity->GetEventBuildingClassCityYield((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield) > 0)
	{
		iFlatYield += (pCity->GetEventBuildingClassCityYield((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield) * 5);
	}

	if (pCity->plot()->isRiver() && pkBuildingInfo->GetRiverPlotYieldChange(eYield) > 0)
	{
		iFlatYield += (pkBuildingInfo->GetRiverPlotYieldChange(eYield) * pCity->countNumRiverPlots());
	}
	if (pkBuildingInfo->GetSeaPlotYieldChange(eYield) > 0)
	{
		iFlatYield += (pkBuildingInfo->GetSeaPlotYieldChange(eYield) * pCity->countNumWaterPlots());
	}
	for (int j = 0; j < NUM_YIELD_TYPES; j++)
	{
		if (pkBuildingInfo->GetYieldFromYield(eYield, (YieldTypes)j) > 0)
		{
			iFlatYield += iYieldRate / pkBuildingInfo->GetYieldFromYield(eYield, (YieldTypes)j);
		}
	}	
	if (pkBuildingInfo->GetGreatWorkYieldChange(eYield) > 0)
	{
		iFlatYield += pkBuildingInfo->GetGreatWorkYieldChange(eYield) * (kPlayer.GetCulture()->GetNumGreatWorkSlots() / 2);
	}
	if (pkBuildingInfo->GetGreatWorkYieldChangeLocal(eYield) > 0)
	{
		iFlatYield += pkBuildingInfo->GetGreatWorkYieldChange(eYield) * pCity->GetCityBuildings()->GetNumAvailableGreatWorkSlots();
	}

	const vector<BuildingTypes>& buildingInteractions = GC.getBuildingInteractions(eBuilding);
	for (size_t i = 0; i < buildingInteractions.size(); i++)
	{
		CvBuildingEntry* pkLoopBuilding = GC.getBuildingInfo(buildingInteractions[i]);
		if (pkLoopBuilding)
		{
			iFlatYield += (pkBuildingInfo->GetBuildingClassYieldChange(pkLoopBuilding->GetBuildingClassType(), eYield) * kPlayer.getNumCities());

			if (pCity->GetCityBuildings()->GetNumBuildingClass((BuildingClassTypes)pkLoopBuilding->GetBuildingClassType()) > 0)
			{
				iFlatYield += (pkBuildingInfo->GetBuildingClassLocalYieldChange(pkLoopBuilding->GetBuildingClassType(), eYield) * 5);
			}
			else if (pCity->canConstruct(buildingInteractions[i]))
			{
				iFlatYield += (pkBuildingInfo->GetBuildingClassLocalYieldChange(pkLoopBuilding->GetBuildingClassType(), eYield) * 2);
			}
			else
			{
				iFlatYield += (pkBuildingInfo->GetBuildingClassLocalYieldChange(pkLoopBuilding->GetBuildingClassType(), eYield));
			}

			iModifier += (pkBuildingInfo->GetBuildingClassYieldModifier(pkLoopBuilding->GetBuildingClassType(), eYield) * kPlayer.getNumCities() * 2);
		}
	}

	int iNumTerrainInfos = GC.getNumTerrainInfos();
	for (int iI = 0; iI < iNumTerrainInfos; iI++)
	{
		TerrainTypes eTerrain = (TerrainTypes)iI;
		if (eTerrain == NO_TERRAIN)
			continue;

		if (pkBuildingInfo->GetYieldPerXTerrain(eTerrain, eYield) == 0 && pkBuildingInfo->GetTerrainYieldChange(eTerrain, eYield) == 0)
			continue;

		int iNumTerrain = pCity->CountTerrain(eTerrain);

		if (pkBuildingInfo->GetYieldPerXTerrain(eTerrain, eYield) > 0)
		{
			if (eTerrain == TERRAIN_MOUNTAIN)
			{
				if (pCity->GetNearbyMountains() > 0)
				{
					iFlatYield += (pkBuildingInfo->GetYieldPerXTerrain(eTerrain, eYield) * pCity->GetNearbyMountains() / 100);
				}
			}
			else
			{
				if (iNumTerrain > 0)
				{
					iFlatYield += (pkBuildingInfo->GetYieldPerXTerrain(eTerrain, eYield) * iNumTerrain / 100);
				}
			}
		}
		if (pkBuildingInfo->GetTerrainYieldChange(eTerrain, eYield) > 0)
		{
			if (iNumTerrain > 0)
			{
				iFlatYield += (iNumTerrain * pkBuildingInfo->GetTerrainYieldChange(eTerrain, eYield));
			}
		}
	}
	int iNumFeatureInfos = GC.getNumFeatureInfos();
	for (int iI = 0; iI < iNumFeatureInfos; iI++)
	{
		FeatureTypes eFeature = (FeatureTypes)iI;
		if (eFeature == NO_FEATURE)
			continue;

		if (pkBuildingInfo->GetFeatureYieldChange(eFeature, eYield) == 0 && pkBuildingInfo->GetYieldPerXFeature(eFeature, eYield) == 0)
			continue;

		int iCount = pCity->CountFeature(eFeature);
		if (iCount <= 0)
			continue;
		
		if (pkBuildingInfo->GetFeatureYieldChange(eFeature, eYield) > 0)
		{	
			iFlatYield += (iCount * pkBuildingInfo->GetFeatureYieldChange(eFeature, eYield));
		}
		if (pkBuildingInfo->GetYieldPerXFeature(eFeature, eYield) > 0)
		{
			iFlatYield += (iCount * pkBuildingInfo->GetFeatureYieldChange(eFeature, eYield) * iCount / 100);
		}
	}
	int iNumResourceInfos = GC.getNumResourceInfos();
	for (int iI = 0; iI < iNumResourceInfos; iI++)
	{
		ResourceTypes eResource = (ResourceTypes)iI;
		if (eResource == NO_RESOURCE)
			continue;

		const CvResourceInfo* pkResourceInfo = GC.getResourceInfo(eResource);
		if (pkResourceInfo == NULL)
			continue;

		bool bWater = false;
		if ( (pkResourceInfo->isTerrain(TERRAIN_COAST) || pkResourceInfo->isTerrain(TERRAIN_OCEAN)) &&
			// Oil can be on sea tiles and land tiles, without the below part Oil is never valued in non-coastal cities
			 !(pkResourceInfo->isTerrain(TERRAIN_DESERT) || pkResourceInfo->isTerrain(TERRAIN_GRASS) ||
			  pkResourceInfo->isTerrain(TERRAIN_HILL) || pkResourceInfo->isTerrain(TERRAIN_PLAINS) ||
			  pkResourceInfo->isTerrain(TERRAIN_SNOW) || pkResourceInfo->isTerrain(TERRAIN_TUNDRA)) )
		{
			bWater = true;
			if (!pCity->isCoastal())
				continue;
		}
		
		if (!GET_TEAM(kPlayer.getTeam()).GetTeamTechs()->HasTech((TechTypes)pkResourceInfo->getTechReveal()))
			continue;

		if (eYield == YIELD_CULTURE && pkBuildingInfo->GetResourceCultureChange(eResource) == 0 && pkBuildingInfo->GetResourceYieldChange(eResource, eYield) == 0 && pkBuildingInfo->GetSeaResourceYieldChange(eYield) == 0)
			continue;
		else if (eYield == YIELD_FAITH && pkBuildingInfo->GetResourceFaithChange(eResource) == 0 && pkBuildingInfo->GetResourceYieldChange(eResource, eYield) == 0 && pkBuildingInfo->GetSeaResourceYieldChange(eYield) == 0)
			continue;
		else if (pkBuildingInfo->GetResourceYieldChange(eResource, eYield) == 0 && pkBuildingInfo->GetSeaResourceYieldChange(eYield) == 0)
			continue;
	
		int iNumResource = pCity->CountResource(eResource);

		if (eYield == YIELD_CULTURE && pkBuildingInfo->GetResourceCultureChange(eResource) > 0)
		{
			
			if (iNumResource > 0)
			{
				iFlatYield += (iNumResource * pkBuildingInfo->GetResourceCultureChange(eResource));
			}
		}
		else if (eYield == YIELD_FAITH && pkBuildingInfo->GetResourceFaithChange(eResource) > 0)
		{
			if (iNumResource > 0)
			{
				iFlatYield += (pkBuildingInfo->GetResourceFaithChange(eResource) * iNumResource);
			}
		}
		if (pkBuildingInfo->GetResourceYieldChange(eResource, eYield) > 0)
		{
			if (iNumResource > 0)
			{
				iFlatYield += (iNumResource * pkBuildingInfo->GetResourceYieldChange(eResource, eYield));
			}
		}

		if (bWater && pkBuildingInfo->GetSeaResourceYieldChange(eYield) > 0)
		{
			if (iNumResource > 0)
			{
				iFlatYield += (iNumResource * pkBuildingInfo->GetSeaResourceYieldChange(eYield));
			}
		}
	}
	int iNumImprovementInfos = GC.getNumImprovementInfos();
	for (int iI = 0; iI < iNumImprovementInfos; iI++)
	{
		ImprovementTypes eImprovement = (ImprovementTypes)iI;
		if (eImprovement == NO_IMPROVEMENT)
			continue;

		if (pkBuildingInfo->GetImprovementYieldChange(eImprovement, eYield) == 0 && pkBuildingInfo->GetImprovementYieldChangeGlobal(eImprovement, eYield) == 0)
			continue;

		int iNumImprovement = pCity->CountImprovement(eImprovement);
		if (pkBuildingInfo->GetImprovementYieldChange(eImprovement, eYield) > 0)
		{
			if (iNumImprovement > 0)
			{
				iFlatYield += (iNumImprovement * pkBuildingInfo->GetImprovementYieldChange(eImprovement, eYield));
			}
		}
		if (pkBuildingInfo->GetImprovementYieldChangeGlobal(eImprovement, eYield) > 0)
		{
			if (iNumImprovement > 0)
			{
				iFlatYield += (iNumImprovement * pkBuildingInfo->GetImprovementYieldChangeGlobal(eImprovement, eYield)) * kPlayer.getNumCities();
			}
		}
	}
	if (pkBuildingInfo->GetTradeRouteRecipientBonus() > 0 || pkBuildingInfo->GetTradeRouteTargetBonus() > 0 && eYield == YIELD_GOLD)
	{
		iFlatYield += ((kPlayer.GetTrade()->GetTradeValuesAtCityTimes100(pCity, YIELD_GOLD) / 100) * (pkBuildingInfo->GetTradeRouteRecipientBonus() + pkBuildingInfo->GetTradeRouteTargetBonus()));
	}

	int iYieldPolicyBonus = kPlayer.GetBuildingClassYieldChange((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield) + kPlayer.GetPlayerPolicies()->GetBuildingClassYieldChange((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield);
	if (iYieldPolicyBonus > 0)
	{
		iFlatYield += iYieldPolicyBonus;
	}

	SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
	if (eSpecialist != NO_SPECIALIST)
	{
		int iNumNewSpecialists = pkBuildingInfo->GetSpecialistCount();
		if (iNumNewSpecialists > 0)
		{
			int iExistingSpecialists = pCity->GetCityCitizens()->GetSpecialistCount(eSpecialist);
			//Total slots.
			int iSpecialistSlots = pCity->GetCityCitizens()->GetSpecialistSlots(eSpecialist);

			int iSpecialistYield = 0;

			CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
			if (pkSpecialistInfo)
			{
				iSpecialistYield = pkSpecialistInfo->getYieldChange(eYield);

				if (eYield == YIELD_CULTURE)
				{
					iSpecialistYield += pkSpecialistInfo->getCulturePerTurn();
					iSpecialistYield += kPlayer.GetSpecialistCultureChange();
				}

				iSpecialistYield += kPlayer.getSpecialistExtraYield(eSpecialist, eYield) + kPlayer.getSpecialistExtraYield(eYield) + kPlayer.GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, eYield);
				iSpecialistYield += pCity->GetEventSpecialistYield(eSpecialist, eYield);
				iSpecialistYield += pCity->getSpecialistExtraYield(eSpecialist, eYield);
				iSpecialistYield += kPlayer.getSpecialistYieldChange(eSpecialist, eYield);

				ReligionTypes eMajority = pCity->GetCityReligions()->GetReligiousMajority();
				if (eMajority >= RELIGION_PANTHEON)
				{
					const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, pCity->getOwner());
					if (pReligion)
					{
						iSpecialistYield += pReligion->m_Beliefs.GetSpecialistYieldChange(eSpecialist, eYield, pCity->getOwner(), pCity);
					}
				}
			}
			//Alright, we got the specialist yields.
			if (iSpecialistYield > 0)
			{
				//More than one? Multiply!
				iSpecialistYield *= iNumNewSpecialists;

				//If this is our first specialist, double the value.
				if (iSpecialistSlots == 0)
				{
					iSpecialistYield *= 2;

					iFlatYield += (iSpecialistYield * iNumNewSpecialists);
				}
				//We have slots already? If we have open slots we aren't using, reduce the value.
				else if ((iSpecialistSlots - iExistingSpecialists) != 0)
				{
					iSpecialistYield *= 2;
					iSpecialistYield /= 3;

					iFlatYield += (iSpecialistYield * max(1, iExistingSpecialists));
				}
				//Growing normally? Value should increase based on number of specialists already here (to encourage clumping)!
				else
				{
					iSpecialistYield *= 3;
					iSpecialistYield /= 2;
					iFlatYield += (max(1, iExistingSpecialists) * iSpecialistYield);
				}
			}
		}
	}
	if (pkBuildingInfo->GetYieldPerAlly(eYield) > 0)
	{
		iFlatYield += (pkBuildingInfo->GetYieldPerAlly(eYield) * max(GC.getGame().GetNumMinorCivsAlive() / 4, kPlayer.GetNumCSAllies()));
	}
	if (pkBuildingInfo->GetYieldPerFriend(eYield) > 0)
	{
		iFlatYield += (pkBuildingInfo->GetYieldPerFriend(eYield) * max(GC.getGame().GetNumMinorCivsAlive() / 4, kPlayer.GetNumCSFriends()));
	}
	if (pkBuildingInfo->GetYieldFromInternal(eYield) > 0)
	{
		iFlatYield += pkBuildingInfo->GetYieldFromInternal(eYield);
	}

	int iNumCities = kPlayer.getNumCities();
	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
		CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
		if (pkSpecialistInfo)
		{
			int iNumWorkers = pCity->GetCityCitizens()->GetSpecialistSlots(eSpecialist);
			if (iNumWorkers <= 0)
				continue;

			for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
			{
				YieldTypes yield = (YieldTypes)ui;

				if (yield == NO_YIELD)
					continue;

				iFlatYield += (2 * pkBuildingInfo->GetSpecialistYieldChangeLocal(eSpecialist, yield) * iNumWorkers);

				if (pkBuildingInfo->GetSpecialistYieldChange(eSpecialist, yield) > 0)
				{
					iFlatYield += (iNumWorkers * iNumCities * pkBuildingInfo->GetSpecialistYieldChange(eSpecialist, yield) * 5);
				}
			}
		}
	}

	///////////////
	// Instant Yields
	//////////////


	if (pkBuildingInfo->GetYieldFromInternalTREnd(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromInternalTREnd(eYield);
	}
	if (pkBuildingInfo->GetYieldFromConstruction(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromConstruction(eYield);
	}
	if (pkBuildingInfo->GetYieldFromDeath(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromDeath(eYield);
	}
	if (pkBuildingInfo->GetYieldFromGPExpend(eYield) > 0)
	{
		iInstant += (pkBuildingInfo->GetYieldFromGPExpend(eYield) * max(10, ((pCity->getGreatPeopleRateModifier() + kPlayer.getGreatPeopleRateModifier()) / 10)));
		iInstant += kPlayer.GetPlayerTraits()->GetWLTKDGPImprovementModifier() * 10;
	}
	if (pkBuildingInfo->GetYieldFromTech(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromTech(eYield);
	}
	if (pkBuildingInfo->GetYieldFromVictory(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromVictory(eYield);
	}
	if (pkBuildingInfo->GetYieldFromVictoryGlobal(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromVictoryGlobal(eYield) * 5;
	}
	if (pkBuildingInfo->GetYieldFromVictoryGlobalPlayer(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromVictoryGlobalPlayer(eYield) * 25;
	}
	
	if (pkBuildingInfo->GetYieldFromPillage(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromPillage(eYield);
	}
	if (pkBuildingInfo->GetYieldFromPillageGlobal(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromPillageGlobal(eYield) * 10;
	}
	if (pkBuildingInfo->GetYieldFromPillageGlobalPlayer(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromPillageGlobalPlayer(eYield) * 25;
	}
	if (pkBuildingInfo->GetInstantYield(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetInstantYield(eYield);
	}
	if (pkBuildingInfo->GetGrowthExtraYield(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetGrowthExtraYield(eYield);
	}
	if (pkBuildingInfo->GetYieldFromBorderGrowth(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromBorderGrowth(eYield) + (pCity->getPlotCultureCostModifier() * -1) + (kPlayer.GetPlotCultureCostModifier() * -1);
	}
	if (pkBuildingInfo->GetYieldFromPolicyUnlock(eYield) > 0)
	{
		iInstant += (kPlayer.getPolicyCostModifier() * -1) + pkBuildingInfo->GetYieldFromPolicyUnlock(eYield);
	}
	if (pkBuildingInfo->GetYieldFromSpyAttack(eYield) > 0)
	{
		iInstant += max(1, kPlayer.GetEspionage()->GetNumSpies()) * pkBuildingInfo->GetYieldFromSpyAttack(eYield) / 15;
	}
	if (pkBuildingInfo->GetYieldFromSpyDefense(eYield) > 0)
	{
		iInstant += max(1, kPlayer.GetEspionage()->GetNumSpies()) * pkBuildingInfo->GetYieldFromSpyDefense(eYield) / 15;
	}
	if (pkBuildingInfo->GetYieldFromPurchase(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromPurchase(eYield);

		if ((kPlayer.GetInvestmentModifier() * -1) > 0)
		{
			iInstant += kPlayer.GetInvestmentModifier() * -1;
		}
		if ((kPlayer.GetPlayerTraits()->GetInvestmentModifier() * -1) > 0)
		{
			iInstant += kPlayer.GetPlayerTraits()->GetInvestmentModifier() * -1;
		}
	}
	if (pkBuildingInfo->GetYieldFromFaithPurchase(eYield) > 0)
	{
		iInstant += pkBuildingInfo->GetYieldFromFaithPurchase(eYield);

		if (kPlayer.GetPlayerTraits()->IsReligious())
			iInstant += pkBuildingInfo->GetYieldFromFaithPurchase(eYield);
	}
	if (pkBuildingInfo->GetYieldFromUnitLevelUp(eYield) > 0)
	{
		if (kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, DOMAIN_LAND) == pCity)
		{
			iInstant += pkBuildingInfo->GetYieldFromUnitLevelUp(eYield);
		}
		if (kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, DOMAIN_SEA) == pCity)
		{
			iInstant += pkBuildingInfo->GetYieldFromUnitLevelUp(eYield);
		}
		if (kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, DOMAIN_AIR) == pCity)
		{
			iInstant += pkBuildingInfo->GetYieldFromUnitLevelUp(eYield);
		}
	}

	if (pkBuildingInfo->GetYieldFromUnitProduction(eYield) > 0)
	{
		if (kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, DOMAIN_LAND) == pCity)
		{
			iInstant += pkBuildingInfo->GetYieldFromUnitProduction(eYield);
		}
		if (kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, DOMAIN_SEA) == pCity)
		{
			iInstant += pkBuildingInfo->GetYieldFromUnitProduction(eYield);
		}
		if (kPlayer.GetBestMilitaryCity(NO_UNITCOMBAT, DOMAIN_AIR) == pCity)
		{
			iInstant += pkBuildingInfo->GetYieldFromUnitProduction(eYield);
		}
	}
	if (pkBuildingInfo->GetYieldFromBirth(eYield) > 0)
	{
		//we want these as early as possible!
		iInstant += max(1, (500 - (pCity->getPopulation() * 10)));

		iInstant += pkBuildingInfo->GetYieldFromBirth(eYield) + pCity->foodDifference() + pCity->GetGrowthExtraYield(eYield) + kPlayer.GetCityGrowthMod();
		if (pCity->isCapital())
		{
			iInstant += kPlayer.GetCapitalGrowthMod();
		}
		iInstant += kPlayer.GetPlayerTraits()->GetWLTKDGPImprovementModifier();
		iInstant += kPlayer.GetPlayerTraits()->GetGrowthBoon();
	}

	///////////////
	// Yield Modifiers
	//////////////

	if (pkBuildingInfo->GetGoldenAgeYieldMod(eYield) > 0)
	{
		iModifier += pkBuildingInfo->GetGoldenAgeYieldMod(eYield);
		if (kPlayer.getGoldenAgeModifier() != 0)
		{
			iModifier *= (100 + kPlayer.getGoldenAgeModifier());
			iModifier /= 100;
		}
		if (kPlayer.GetGoldenAgeTourism() > 0)
		{
			iModifier *= 125;
			iModifier /= 100;
		}
		if (kPlayer.GetPlayerTraits()->GetGoldenAgeDurationModifier() > 0)
		{
			iModifier *= (100 + kPlayer.GetPlayerTraits()->GetGoldenAgeDurationModifier());
			iModifier /= 100;
		}
		if (kPlayer.GetPlayerTraits()->GetWonderProductionModGA() > 0)
		{
			iModifier *= (100 + kPlayer.GetPlayerTraits()->GetWonderProductionModGA());
			iModifier /= 100;
		}

		ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(kPlayer.GetID());
		if (eReligion == NO_RELIGION)
		{
			eReligion = kPlayer.GetReligions()->GetReligionInMostCities();
		}
		if (eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, kPlayer.GetID());
			if (pReligion)
			{
				if (pReligion->m_Beliefs.GetYieldBonusGoldenAge(eYield, kPlayer.GetID(), pCity, true) > 0)
				{
					iModifier += (pReligion->m_Beliefs.GetYieldBonusGoldenAge(eYield, kPlayer.GetID(), pCity, true) * 2);
				}
			}
		}
	}

	if (pCity->GetEventBuildingClassCityYieldModifier((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield) > 0)
	{
		iModifier += (pCity->GetEventBuildingClassCityYieldModifier((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield) * 2);
	}

	if (pkBuildingInfo->GetYieldFromWLTKD(eYield) > 0)
	{
		iModifier += pkBuildingInfo->GetYieldFromWLTKD(eYield);

		ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(kPlayer.GetID());
		if (eReligion == NO_RELIGION)
		{
			eReligion = kPlayer.GetReligions()->GetReligionInMostCities();
		}
		if (eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, kPlayer.GetID());
			if (pReligion)
			{
				if (pReligion->m_Beliefs.GetYieldFromWLTKD(eYield, kPlayer.GetID(), pCity) > 0)
				{
					iModifier += (pReligion->m_Beliefs.GetYieldFromWLTKD(eYield, kPlayer.GetID(), pCity) * 10);
				}
			}
		}
	}

	if (pkBuildingInfo->GetAreaYieldModifier(eYield) > 0)
	{
		iModifier += (pkBuildingInfo->GetAreaYieldModifier(eYield) * 5);
	}

	if (pkBuildingInfo->GetYieldFromProcessModifier(eYield) > 0)
	{
		iModifier += (pkBuildingInfo->GetYieldFromProcessModifier(eYield) * 2);
	}

	if (pkBuildingInfo->GetYieldModifier(eYield) > 0)
	{
		iModifier += pkBuildingInfo->GetYieldModifier(eYield);
	}
	if (pkBuildingInfo->GetGlobalYieldModifier(eYield) > 0)
	{
		iModifier += pkBuildingInfo->GetGlobalYieldModifier(eYield);
	}

	int iYieldPolicyModBonus = kPlayer.GetPlayerPolicies()->GetBuildingClassYieldModifier((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield);
	if (iYieldPolicyModBonus > 0)
	{
		iModifier += iYieldPolicyModBonus;
	}
	if (pCity->GetCityReligions()->GetReligiousMajority() == kPlayer.GetReligions()->GetReligionInMostCities())
	{
		int iReligionPolicyBonus = pCity->getReligionBuildingYieldRateModifier((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield);
		if (iReligionPolicyBonus > 0)
		{
			iModifier += iReligionPolicyBonus;
		}
	}

	//Let's see how big these changes are for our city...
	//If we are deficient in this yield, increase the flat yield's value to compensate.
	if (pCity->GetCityStrategyAI()->GetFocusYield() == eYield || pCity->GetCityStrategyAI()->IsYieldDeficient(eYield))
	{
		iFlatYield *= 10;
		iModifier *= 10;
	}

	//Math time! Let's see how this affects our city.
	int iDelta = 0;
	if (iFlatYield > 0)
	{
		//let's see our % bump here.
		iDelta = (iFlatYield * 100) / max(1, iYieldRate);

		if (iYieldRate <= 0)
		{
			//Yield value here greater than our yield output in this city? We need this badly!
			iDelta *= 5;
		}

		//Instant Yields don't scale with era, but they do help for base infrastructure. Scale by city population.
		iDelta *= (100 + pCity->getPopulation() - iEra);
		iDelta /= 100;

		//And here's what the value represents.
		iYieldValue += iDelta;
	}

	if (iInstant > 0)
	{
		//Instant Yields almost always scale with era, so compensate.
		iInstant *= max(1, iEra);

		//Let's see how much this is compared to our actual rate.
		//We divide, since we are getting this sporadically, not all the time.
		iDelta = max((iInstant/2), (iInstant / max(1, iYieldRate)));

		iYieldValue += iDelta;

	}
	if (iModifier > 0)
	{
		//Modifiers are more important as the game goes on, exponentially so.
		iModifier *= (100 + (iEra * iEra * iEra * iEra));
		iModifier /= 100;
		//Let's see how much this is compared to our actual rate.
		//We multiply, as we want to see what the 'new' value will be with this modifier intact.
		//We don't need to do this again as this shows us the actual bonus earned here.
		int iActualIncrease = ((iModifier * max(1, iYieldRate)) / 100);

		iYieldValue += iActualIncrease;
	}
	
	AICityStrategyTypes eNeedCulture = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_FIRST_CULTURE_BUILDING");
	EconomicAIStrategyTypes eStrategyBuildingReligion = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_DEVELOPING_RELIGION", true);

	if (iYieldValue > 0)
	{
		switch (eYield)
		{
		case YIELD_CULTURE:
			if (eNeedCulture != NO_AICITYSTRATEGY && pCity->GetCityStrategyAI()->IsUsingCityStrategy(eNeedCulture))
			{
				iYieldValue *= 2;
			}
			if (kPlayer.GetDiplomacyAI()->IsCloseToCultureVictory())
			{
				iYieldValue *= 2;
			}
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

				if (eLoopPlayer != NO_PLAYER && eLoopPlayer != kPlayer.GetID() && GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && !GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsCloseToCultureVictory())
					{
						iYieldValue *= 3;
					}
					else if (GET_PLAYER(eLoopPlayer).GetCulture()->GetInfluenceTrend(kPlayer.GetID()) == INFLUENCE_TREND_RISING)
					{
						iYieldValue *= 2;
					}
					else if (GET_PLAYER(eLoopPlayer).GetCulture()->GetInfluenceLevel(kPlayer.GetID()) >= INFLUENCE_LEVEL_FAMILIAR)
					{
						iYieldValue *= 2;
					}
				}
			}
			break;
		case YIELD_SCIENCE:
			if (kPlayer.GetDiplomacyAI()->IsCloseToSSVictory())
			{
				iYieldValue *= 2;
			}
			break;
		case YIELD_PRODUCTION:
			if (kPlayer.GetDiplomacyAI()->IsCloseToDominationVictory())
			{
				iYieldValue *= 2;
			}
			break;
		case YIELD_TOURISM:
			if (kPlayer.GetDiplomacyAI()->IsCloseToCultureVictory())
			{
				iYieldValue *= 2;
			}
			break;
		case YIELD_FAITH:
			
			int iFlavorReligion = kPlayer.GetFlavorManager()->GetPersonalityIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
			
			if (kPlayer.GetPlayerTraits()->IsReligious())
			{
				iFlavorReligion *= 3;
				iFlavorReligion /= 2;
			}
			else
			{
				iFlavorReligion *= 2;
				iFlavorReligion /= 3;
			}

			if (pCity->GetFaithPerTurnFromBuildings() <= 0 && !kPlayer.GetReligions()->HasCreatedPantheon())
			{		
				iYieldValue += max(1, iFlavorReligion);
			}
			
			if (eStrategyBuildingReligion != NO_ECONOMICAISTRATEGY && kPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyBuildingReligion))
			{
				iYieldValue += max(1, iFlavorReligion);
			}
			break;
		}

		AIGrandStrategyTypes eGrandStrategy = kPlayer.GetGrandStrategyAI()->GetActiveGrandStrategy();
		bool bSeekingDiploVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_UNITED_NATIONS");
		bool bSeekingConquestVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CONQUEST");
		bool bSeekingCultureVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE");
		bool bSeekingScienceVictory = eGrandStrategy == GC.getInfoTypeForString("AIGRANDSTRATEGY_SPACESHIP");

		//GS Yield Valuation
		if (bSeekingDiploVictory && (eYield == YIELD_GOLD || eYield == YIELD_FAITH))
		{
			iYieldValue *= 2;
		}
		if (bSeekingConquestVictory && (eYield == YIELD_PRODUCTION || eYield == YIELD_GOLD))
		{
			iYieldValue *= 2;
		}
		if (bSeekingCultureVictory && (eYield == YIELD_CULTURE || eYield == YIELD_TOURISM))
		{
			iYieldValue *= 2;
		}
		if (bSeekingScienceVictory && (eYield == YIELD_SCIENCE || eYield == YIELD_FOOD))
		{
			iYieldValue *= 2;
		}

		//JFD CRIME NEGATIVE OVERRIDE
		if (MOD_BALANCE_CORE_JFD && (eYield == YIELD_JFD_CRIME || (eYield == YIELD_JFD_DISEASE)))
		{
			//Flip value if Crime or Disease
			iYieldValue *= -1;
		}
	}

	return iYieldValue;
}
int CityStrategyAIHelpers::GetBuildingGrandStrategyValue(CvCity *pCity, BuildingTypes eBuilding, PlayerTypes ePlayer)
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	//Skip if null
	if (pkBuildingInfo == NULL)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);

	//puppets don't care.
	if (pCity != NULL && pCity->IsPuppet() && !kPlayer.GetPlayerTraits()->IsNoAnnexing())
		return 0;


	//Start with 0 value for modifier.

	int iValue = 0;

	//We're going to use the current 'interest' value of the grand strategy to gauge how useful/useless a building is to a player.

	// == Grand Strategy ==
	int iDiploInterest = 0;
	int iConquestInterest = 0;
	int iScienceInterest = 0;
	int iCultureInterest = 0;

	int iDiploValue = 0;
	int iScienceValue = 0;
	int iConquestValue = 0;
	int iCultureValue = 0;

	int iGrandStrategiesLoop;
	AIGrandStrategyTypes eGrandStrategy;
	CvAIGrandStrategyXMLEntry* pGrandStrategy;
	CvString strGrandStrategyName;

	// Loop through all GrandStrategies and get priority. Since these are usually 100+, we will divide by 10 later
	for (iGrandStrategiesLoop = 0; iGrandStrategiesLoop < GC.GetGameAIGrandStrategies()->GetNumAIGrandStrategies(); iGrandStrategiesLoop++)
	{
		eGrandStrategy = (AIGrandStrategyTypes)iGrandStrategiesLoop;
		pGrandStrategy = GC.GetGameAIGrandStrategies()->GetEntry(iGrandStrategiesLoop);
		strGrandStrategyName = (CvString)pGrandStrategy->GetType();

		if (strGrandStrategyName == "AIGRANDSTRATEGY_CONQUEST")
		{
			iConquestInterest += kPlayer.GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_CULTURE")
		{
			iCultureInterest += kPlayer.GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_UNITED_NATIONS")
		{
			iDiploInterest += kPlayer.GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
		else if (strGrandStrategyName == "AIGRANDSTRATEGY_SPACESHIP")
		{
			iScienceInterest += kPlayer.GetGrandStrategyAI()->GetGrandStrategyPriority(eGrandStrategy);
		}
	}

	if (kPlayer.GetPlayerTraits()->IsExpansionist() || kPlayer.GetPlayerTraits()->IsWarmonger())
	{
		iConquestInterest *= 3;
		iDiploInterest *= 2;
	}
	if (kPlayer.GetPlayerTraits()->IsNerd())
	{
		iConquestInterest *= 2;
		iScienceInterest *= 3;
	}
	if (kPlayer.GetPlayerTraits()->IsDiplomat() || kPlayer.GetPlayerTraits()->IsSmaller())
	{
		iCultureInterest *= 2;
		iDiploInterest *= 3;
	}
	if (kPlayer.GetPlayerTraits()->IsTourism() || kPlayer.GetPlayerTraits()->IsReligious())
	{
		iCultureInterest *= 3;
		iScienceInterest *= 2;
	}

	//Let's look at building special traits.

	if(pkBuildingInfo->IsDiplomaticVoting())
	{
		iDiploValue += 100;
	}
	if(pkBuildingInfo->GetVotesPerGPT() > 0)
	{
		iDiploValue += 100;
	}
	if(pkBuildingInfo->GetSingleVotes() > 0)
	{
		iDiploValue += (pkBuildingInfo->GetSingleVotes() * 25);
	}
	if(pkBuildingInfo->GetExtraLeagueVotes() > 0)
	{
		iDiploValue += (pkBuildingInfo->GetExtraLeagueVotes() * 25);
	}
	if(pkBuildingInfo->GetMinorFriendshipChange() > 0)
	{
		iDiploValue += (pkBuildingInfo->GetMinorFriendshipChange() / 2);
	}
#if defined(MOD_DIPLOMACY_CITYSTATES)
	if(MOD_DIPLOMACY_CITYSTATES)
	{
		if(pkBuildingInfo->GetCapitalsToVotes() > 0)
		{
			iDiploValue += 25;
		}
		if(pkBuildingInfo->GetRAToVotes() > 0)
		{
			iDiploValue += 25;
		}
		if(pkBuildingInfo->GetDoFToVotes() > 0)
		{
			iDiploValue += 25;
		}
		if(pkBuildingInfo->GetFaithToVotes() > 0)
		{
			iDiploValue += 25;
		}
		if(pkBuildingInfo->GetDPToVotes() > 0)
		{
			iDiploValue += 25;
		}
		if(pkBuildingInfo->GetGPExpendInfluence() > 0)
		{
			iDiploValue += 25;
		}
		UnitCombatTypes eUnitCombat = (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_DIPLOMACY", true);
		if(eUnitCombat != NO_UNITCOMBAT)
		{
			if(pkBuildingInfo->GetUnitCombatProductionModifier((int)eUnitCombat) > 0)
			{
				iDiploValue += pkBuildingInfo->GetUnitCombatProductionModifier((int)eUnitCombat);
				if (pCity != NULL)
				{
					iDiploValue += pCity->getUnitCombatProductionModifier(eUnitCombat);
				}
			}
		}
	}
#endif

	if(pkBuildingInfo->GetAirModifier() > 0)
	{
		iConquestValue += (pkBuildingInfo->GetAirModifier() / 2);
	}

	if (pkBuildingInfo->GetAirModifierGlobal() > 0)
	{
		iConquestValue += pkBuildingInfo->GetAirModifierGlobal() * ((kPlayer.getNumCities()+1) * 10);
	}

	for (int ik = 0; ik < GC.getNumHurryInfos(); ik++)
	{
		if (pkBuildingInfo->GetHurryModifier((HurryTypes)ik) <= 0)
			iConquestValue += (pkBuildingInfo->GetHurryModifier((HurryTypes)ik) * -100);

		if (pkBuildingInfo->GetHurryModifierLocal((HurryTypes)ik) <= 0)
			iConquestValue += (pkBuildingInfo->GetHurryModifierLocal((HurryTypes)ik) * -50);
	}

	if(pkBuildingInfo->GetAlwaysHeal() > 0)
	{
		if (pCity != NULL)
		{
			if (pCity->isBorderCity())
			{
				iConquestValue += 200;
			}
			else
			{
				iConquestValue += 100;
			}
		}
		else
		{
			iConquestValue += 50;
		}
	}
	if(pkBuildingInfo->GetFreeExperience() > 0)
	{
		iConquestValue += pkBuildingInfo->GetFreeExperience();
	}
	PromotionTypes eFreePromotion = (PromotionTypes) pkBuildingInfo->GetFreePromotion();
	if(eFreePromotion != NO_PROMOTION)
	{
		iConquestValue += 50;
	}
	PromotionTypes eFreeTrainedPromotion = (PromotionTypes) pkBuildingInfo->GetTrainedFreePromotion();
	if(eFreeTrainedPromotion != NO_PROMOTION)
	{
		iConquestValue += 50;
	}
	if(pkBuildingInfo->GetGlobalFreeExperience() > 0)
	{
		iConquestValue += pkBuildingInfo->GetGlobalFreeExperience();
	}
	if(pkBuildingInfo->GetGreatGeneralRateModifier() > 0)
	{
		iConquestValue += pkBuildingInfo->GetGreatGeneralRateModifier();
	}
	if(pkBuildingInfo->GetInstantMilitaryIncrease() > 0)
	{
		iConquestValue += 500;
	}
	if(pkBuildingInfo->GetUnitUpgradeCostMod() != 0)
	{
		iConquestValue += (pkBuildingInfo->GetUnitUpgradeCostMod() * -1);
	}


	if(pkBuildingInfo->GetGlobalSpaceProductionModifier() > 0)
	{
		iScienceValue += pkBuildingInfo->GetGlobalSpaceProductionModifier();
	}
	if(pkBuildingInfo->GetGreatScientistBeakerModifier() > 0)
	{
		iScienceValue += pkBuildingInfo->GetGreatScientistBeakerModifier();
	}
	if(pkBuildingInfo->GetSpaceProductionModifier() > 0)
	{
		iScienceValue += pkBuildingInfo->GetSpaceProductionModifier();
	}
	if(pkBuildingInfo->GetMedianTechPercentChange() > 0)
	{
		iScienceValue += pkBuildingInfo->GetMedianTechPercentChange();
	}
	
	if(pkBuildingInfo->GetEventTourism() > 0)
	{
		iCultureValue += (pkBuildingInfo->GetEventTourism() * 10);
	}
	if(pkBuildingInfo->GetFreeGreatWork() > 0)
	{
		iCultureValue += 50;
		if (kPlayer.GetPlayerTraits()->IsGreatWorkWLTKD())
		{
			iCultureValue += 100;
		}
	}
	if(pkBuildingInfo->GetFreePolicies() > 0)
	{
		iCultureValue += kPlayer.GetNumPolicies() * kPlayer.getNumCities() * 5;
	}	
	if(pkBuildingInfo->GetGreatWorkCount() > 0)
	{
		iCultureValue += (pkBuildingInfo->GetGreatWorkCount() * 5);
		if (pCity != NULL && pCity->GetCityCulture()->GetNumGreatWorkSlots() <= pkBuildingInfo->GetGreatWorkCount())
			iCultureValue += (pkBuildingInfo->GetGreatWorkCount() * 5);
	}
	if(pkBuildingInfo->GetGreatWorksTourismModifier() > 0)
	{
		iCultureValue += pkBuildingInfo->GetGreatWorksTourismModifier();
	}
	if(pkBuildingInfo->GetLandmarksTourismPercent() > 0)
	{
		iCultureValue += pkBuildingInfo->GetLandmarksTourismPercent();
	}
	if (pkBuildingInfo->GetGreatWorksTourismModifierGlobal() > 0)
	{
		iCultureValue += pkBuildingInfo->GetGreatWorksTourismModifierGlobal() * kPlayer.getNumCities();
	}
	if (pkBuildingInfo->GetLandmarksTourismPercentGlobal() > 0)
	{
		iCultureValue += pkBuildingInfo->GetLandmarksTourismPercentGlobal() * kPlayer.getNumCities();
	}
	if(pkBuildingInfo->GetLandTourismEnd() > 0)
	{
		iCultureValue += (pkBuildingInfo->GetLandTourismEnd() * 10);
	}
	if(pkBuildingInfo->GetSeaTourismEnd() > 0)
	{
		iCultureValue += (pkBuildingInfo->GetSeaTourismEnd() * 10);
	}
	if(pkBuildingInfo->GetTechEnhancedTourism() > 0)
	{
		iCultureValue += (pkBuildingInfo->GetTechEnhancedTourism() * 10);
	}

	if(pCity != NULL && (pkBuildingInfo->GetLandmarksTourismPercent() > 0 || pkBuildingInfo->GetLandmarksTourismPercentGlobal() > 0))
	{
		int iTest = pCity->getYieldRate(YIELD_CULTURE, false);

		iCultureValue += (iTest / max(1, (pkBuildingInfo->GetLandmarksTourismPercent() + pkBuildingInfo->GetLandmarksTourismPercentGlobal())));
	}
	if(pCity != NULL && (pkBuildingInfo->GetGreatWorksTourismModifier() > 0 || pkBuildingInfo->GetGreatWorksTourismModifierGlobal() > 0))
	{
		int iWorks = max(3, pCity->GetCityCulture()->GetNumGreatWorkSlots());

		//Higher value the higher the number of works.
		iCultureValue += (iWorks * (pkBuildingInfo->GetGreatWorksTourismModifier() + pkBuildingInfo->GetGreatWorksTourismModifierGlobal()));
	}

	//Take the bonus from above and multiply it by the priority value / 10 (as most are 100+, so we're getting a % interest here).

	iConquestValue *= (100 + (iConquestInterest / 10));
	iConquestValue /= 100;

	iCultureValue *= (100 + (iCultureInterest / 10));
	iCultureValue /= 100;

	iDiploValue *= (100 + (iDiploInterest / 10));
	iDiploValue /= 100;

	iScienceValue *= (100 + (iScienceInterest / 10));
	iScienceValue /= 100;

	//And now add them in. Halve if not our main focus.
	if (kPlayer.GetDiplomacyAI()->IsGoingForCultureVictory() || kPlayer.GetDiplomacyAI()->IsCloseToCultureVictory())
	{
		iValue += iCultureValue;
	}
	else
	{
		iValue += (iCultureValue / 2);
	}
	if (kPlayer.GetDiplomacyAI()->IsGoingForDiploVictory() || kPlayer.GetDiplomacyAI()->IsCloseToDiploVictory())
	{
		iValue += iDiploValue;
	}
	else
	{
		iValue += (iDiploValue / 2);
	}
	if (kPlayer.GetDiplomacyAI()->IsGoingForSpaceshipVictory() || kPlayer.GetDiplomacyAI()->IsCloseToSSVictory())
	{
		iValue += iScienceValue;
	}
	else
	{
		iValue += (iScienceValue / 2);
	}

	if (kPlayer.GetDiplomacyAI()->IsGoingForWorldConquest() || kPlayer.GetDiplomacyAI()->IsGoingForWorldConquest())
	{
		iValue += iConquestValue;
	}
	else
	{
		iValue += (iConquestValue / 2);
	}
	
	return iValue * (kPlayer.GetCurrentEra()+1);
}
int CityStrategyAIHelpers::GetBuildingPolicyValue(CvCity *pCity, BuildingTypes eBuilding)
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	//Skip if null
	if(pkBuildingInfo == NULL)
		return 0;

	if(pCity == NULL)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(pCity->getOwner());

	int iValue = 0;

	//Bonuses below are compounding based on existing bonuses. The idea is to help the AI 'synergize' its bonuses.
	if(pkBuildingInfo->GetWorkerSpeedModifier() > 0)
	{
		iValue += kPlayer.getWorkerSpeedModifier() + pkBuildingInfo->GetWorkerSpeedModifier();
	}

	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
		CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
		if(pkSpecialistInfo)
		{
			int iNumWorkers = pCity->GetCityCitizens()->GetSpecialistSlots(eSpecialist);
			if (iNumWorkers <= 0)
				continue;

			iValue += (pkBuildingInfo->GetSpecificGreatPersonRateModifier(iSpecialistLoop) * iNumWorkers);

			iValue += pCity->GetPlayer()->GetPlayerTraits()->GetWLTKDGPImprovementModifier() * 5;
		}
	}

	if(pkBuildingInfo->GetPlotCultureCostModifier() < 0)
	{
		iValue += 2 * abs((kPlayer.GetPlotCultureCostModifier() + pkBuildingInfo->GetPlotCultureCostModifier()) * abs(kPlayer.GetPlotCultureCostModifier() + pkBuildingInfo->GetPlotCultureCostModifier()));
	}
	if(pkBuildingInfo->GetPlotBuyCostModifier() < 0)
	{
		iValue += 2 * abs((kPlayer.GetPlotGoldCostMod() + pkBuildingInfo->GetPlotBuyCostModifier())) * abs((kPlayer.GetPlotGoldCostMod() + pkBuildingInfo->GetPlotBuyCostModifier()));
	}
	if(pkBuildingInfo->GetNumTradeRouteBonus())
	{
		iValue += ((pkBuildingInfo->GetNumTradeRouteBonus() + kPlayer.GetTrade()->GetNumTradeRoutesPossible()) * 5);
	}
	if (pkBuildingInfo->GetResourceDiversityModifier())
	{
		iValue += ((pkBuildingInfo->GetResourceDiversityModifier() + kPlayer.GetTrade()->GetNumTradeRoutesPossible()) * 5);
	}
	if (pkBuildingInfo->GetNoUnhappfromXSpecialists())
	{
		iValue += (pkBuildingInfo->GetNoUnhappfromXSpecialists() + pCity->GetCityCitizens()->GetSpecialistSlotsTotal());
	}
	if (pkBuildingInfo->GetNoUnhappfromXSpecialistsGlobal())
	{
		iValue += (pkBuildingInfo->GetNoUnhappfromXSpecialistsGlobal() + (pCity->GetCityCitizens()->GetSpecialistSlotsTotal() * pCity->GetCityCitizens()->GetSpecialistSlotsTotal()));
	}
	
	if(pkBuildingInfo->GetPolicyCostModifier() <= 0)
	{
		iValue += (-10 * ((kPlayer.GetPolicyCostBuildingModifier() + pkBuildingInfo->GetPolicyCostModifier())));
	}
	if(pkBuildingInfo->GetGoldenAgeModifier() > 0 || pkBuildingInfo->IsGoldenAge())
	{
		iValue += (kPlayer.getGoldenAgeModifier() + kPlayer.GetGoldenAgeTourism() + kPlayer.GetPlayerTraits()->GetGoldenAgeDurationModifier() + pkBuildingInfo->GetGoldenAgeModifier());

		if (kPlayer.GetPlayerTraits()->IsGreatWorkWLTKD())
		{
			iValue += 25;
		}

		for(int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
		{
			GreatPersonTypes eGP = (GreatPersonTypes)iJ;
			if(eGP == -1 || eGP == NULL || !eGP)
				continue;

			if(kPlayer.GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGP) > 0)
			{
				iValue += kPlayer.GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGP);
			}
		}
		if(kPlayer.GetPlayerTraits()->GetGoldenAgeCombatModifier() > 0)
		{
			iValue += kPlayer.GetPlayerTraits()->GetGoldenAgeCombatModifier();
		}
		if(kPlayer.GetPlayerTraits()->GetGoldenAgeGreatArtistRateModifier() > 0)
		{
			iValue += kPlayer.GetPlayerTraits()->GetGoldenAgeGreatArtistRateModifier();
			if (kPlayer.GetPlayerTraits()->IsGreatWorkWLTKD())
			{
				iValue += 25;
			}
		}
		if(kPlayer.GetPlayerTraits()->GetGoldenAgeGreatWriterRateModifier() > 0)
		{
			iValue += kPlayer.GetPlayerTraits()->GetGoldenAgeGreatWriterRateModifier();
			if (kPlayer.GetPlayerTraits()->IsGreatWorkWLTKD())
			{
				iValue += 25;
			}
		}
		if(kPlayer.GetPlayerTraits()->GetGoldenAgeGreatMusicianRateModifier() > 0)
		{
			iValue += kPlayer.GetPlayerTraits()->GetGoldenAgeGreatMusicianRateModifier();
			if (kPlayer.GetPlayerTraits()->IsGreatWorkWLTKD())
			{
				iValue += 25;
			}
		}
		if(kPlayer.GetPlayerTraits()->GetGoldenAgeTourismModifier() > 0)
		{
			iValue += kPlayer.GetPlayerTraits()->GetGoldenAgeTourismModifier();
		}

		ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(kPlayer.GetID());
		if(eReligion == NO_RELIGION)
		{
			eReligion = kPlayer.GetReligions()->GetReligionInMostCities();
		}
		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, kPlayer.GetID());
			if(pReligion)
			{
				for(int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
				{
					GreatPersonTypes eGP = (GreatPersonTypes)iJ;
					if(eGP == -1 || eGP == NULL || !eGP)
						continue;

					if (pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGP, kPlayer.GetID(), pCity) > 0)
					{
						iValue += pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGP, kPlayer.GetID(), pCity);
						iValue += kPlayer.GetPlayerTraits()->GetWLTKDGPImprovementModifier() * 10;
					}
				}
				for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
				{
					YieldTypes yield = (YieldTypes)ui;
					 
					if(yield == NO_YIELD)
						continue;
							
					if (pReligion->m_Beliefs.GetYieldBonusGoldenAge(yield, kPlayer.GetID(), pCity, true) > 0)
					{
						iValue += pReligion->m_Beliefs.GetYieldBonusGoldenAge(yield, kPlayer.GetID(), pCity, true);
					}
				}
			}
		}
	}
	if(pkBuildingInfo->GetFreeGreatPeople() > 0 || pkBuildingInfo->GetGreatPeopleRateChange() > 0 || pkBuildingInfo->GetGlobalGreatPeopleRateModifier() > 0 || pkBuildingInfo->GetGreatPeopleRateModifier() > 0)
	{
		iValue += kPlayer.getGreatPeopleRateModifier() + pCity->getGreatPeopleRateModifier() + (kPlayer.GetGreatPersonExpendGold() / 10) + pkBuildingInfo->GetGlobalGreatPeopleRateModifier() + pkBuildingInfo->GetGreatPeopleRateModifier();

		iValue += pkBuildingInfo->GetGreatPeopleRateChange() * 50;

		if (kPlayer.GetPlayerTraits()->IsGPWLTKD() || kPlayer.GetPlayerTraits()->IsGreatWorkWLTKD())
		{
			iValue += 100;
			iValue += kPlayer.GetPlayerTraits()->GetWLTKDGPImprovementModifier() * 10;
		}
		for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			YieldTypes yield = (YieldTypes)ui;
					 
			if(yield == NO_YIELD)
				continue;
			
			for(int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
			{
				GreatPersonTypes eGP = (GreatPersonTypes)iJ;
				if(eGP == -1 || eGP == NULL || !eGP)
					continue;
				
				if(kPlayer.getGreatPersonExpendedYield(eGP, yield) > 0)
				{
					iValue += (kPlayer.getGreatPersonExpendedYield(eGP, yield) / 5);
				}
			}
			
		}
		ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(kPlayer.GetID());
		if(eReligion == NO_RELIGION)
		{
			eReligion = kPlayer.GetReligions()->GetReligionInMostCities();
		}
		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, kPlayer.GetID());
			if(pReligion)
			{
				if (pReligion->m_Beliefs.GetGreatPersonExpendedFaith(kPlayer.GetID(), pCity) > 0)
				{
					iValue += (pReligion->m_Beliefs.GetGreatPersonExpendedFaith(kPlayer.GetID(), pCity) / 5);
				}
				for(int iJ = 0; iJ < GC.getNumGreatPersonInfos(); iJ++)
				{
					GreatPersonTypes eGP = (GreatPersonTypes)iJ;
					if(eGP == -1 || eGP == NULL || !eGP)
						continue;

					for(uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
					{
						YieldTypes yield = (YieldTypes)ui;
					 
						if(yield == NO_YIELD)
							continue;
						
						if (pReligion->m_Beliefs.GetGreatPersonExpendedYield(eGP, yield, kPlayer.GetID(), pCity, true) > 0)
						{
							iValue += (pReligion->m_Beliefs.GetGreatPersonExpendedYield(eGP, yield, kPlayer.GetID(), pCity, true) / 5);
						}
					}
				}
			}
		}
	}
	int iProductionBonus = kPlayer.GetPlayerPolicies()->GetBuildingClassProductionModifier((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType());
	if(iProductionBonus > 0)
	{
		iValue += iProductionBonus;
	}
	int iHappinessBonus = kPlayer.GetPlayerPolicies()->GetBuildingClassHappinessModifier((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType());
	if(iHappinessBonus > 0)
	{
		iValue += 5 * kPlayer.getNumCities();
	}
	int iTourism = kPlayer.GetPlayerPolicies()->GetBuildingClassTourismModifier((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType());
	if(iTourism > 0)
	{
		iValue += 5 * kPlayer.getNumCities();
	}

	if(pkBuildingInfo->GetExtraSpies() > 0 || pkBuildingInfo->GetEspionageModifier() < 0 || pkBuildingInfo->GetGlobalEspionageModifier() < 0 || pkBuildingInfo->GetSpyRankChange() > 0 || pkBuildingInfo->GetInstantSpyRankChange() > 0)
	{
		iValue += ((kPlayer.GetEspionage()->GetNumSpies() + kPlayer.GetPlayerTraits()->GetExtraSpies() * 10) + (pkBuildingInfo->GetEspionageModifier() * -5) + (pkBuildingInfo->GetGlobalEspionageModifier() * -10) + (pkBuildingInfo->GetSpyRankChange() + pkBuildingInfo->GetInstantSpyRankChange() * 100));

		if(kPlayer.GetEspionageModifier() != 0)
		{
			iValue += kPlayer.GetEspionageModifier();
		}
		if (kPlayer.GetEspionageModifier() == 0)
		{
			iValue += 50;
		}
		if(kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER) != 0)
		{
			iValue += kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_STEAL_TECH_FASTER_MODIFIER);
		}
		if(kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER) != 0)
		{
			iValue += kPlayer.GetPlayerPolicies()->GetNumericModifier(POLICYMOD_RIGGING_ELECTION_MODIFIER);
		}
		ReligionTypes eReligion = GC.getGame().GetGameReligions()->GetFounderBenefitsReligion(kPlayer.GetID());
		if(eReligion == NO_RELIGION)
		{
			eReligion = kPlayer.GetReligions()->GetReligionInMostCities();
		}
		if(eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, kPlayer.GetID());
			if(pReligion)
			{
				if (pReligion->m_Beliefs.GetSpyPressure(kPlayer.GetID(), pCity) != 0)
				{
					iValue += pReligion->m_Beliefs.GetSpyPressure(kPlayer.GetID(), pCity);
				}
			}
		}
		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			YieldTypes yield = (YieldTypes)ui;

			if (yield == NO_YIELD)
				continue;

			if (kPlayer.getYieldModifierFromActiveSpies(yield) > 0)
			{
				iValue += kPlayer.getYieldModifierFromActiveSpies(yield);
			}
		}
	}
	return iValue * (kPlayer.GetCurrentEra()+1);
}
int CityStrategyAIHelpers::GetBuildingBasicValue(CvCity *pCity, BuildingTypes eBuilding)
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	//Skip if null
	if(pkBuildingInfo == NULL)
		return 0;

	if(pCity == NULL)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(pCity->getOwner());

	int iValue = 0;

	//GWS
	if (pkBuildingInfo->GetNumThemingBonuses() > 0 || pkBuildingInfo->GetGreatWorkCount() > 0)
	{
		int iNumWorks = max(1, pCity->GetCityBuildings()->GetNumGreatWorks());
		iValue += (pkBuildingInfo->GetNumThemingBonuses());
		iValue += (pkBuildingInfo->GetGreatWorkCount() * iNumWorks * 2);
		if (kPlayer.GetPlayerTraits()->GetCapitalThemingBonusModifier() > 0)
		{
			if (pCity != NULL && pCity->isCapital())
			{
				iValue += kPlayer.GetPlayerTraits()->GetCapitalThemingBonusModifier();
			}
			else
			{
				iValue -= kPlayer.GetPlayerTraits()->GetCapitalThemingBonusModifier();
			}
		}
		for (uint ui = 0; ui < NUM_YIELD_TYPES; ui++)
		{
			YieldTypes yield = (YieldTypes)ui;

			if (yield == NO_YIELD)
				continue;

			if (kPlayer.getYieldModifierFromGreatWorks(yield) > 0)
			{
				iValue += kPlayer.getYieldModifierFromGreatWorks(yield);
			}
		}
	}
	if(pkBuildingInfo->GetBuildingProductionModifier() > 0)
	{
		iValue += (pkBuildingInfo->GetBuildingProductionModifier() + pCity->getPopulation()) * 5;
	}

	if (pkBuildingInfo->GetPopulationChange() > 0)
	{
		iValue += (pkBuildingInfo->GetPopulationChange() + pCity->getPopulation()) * 10;
	}

	if (pkBuildingInfo->GetGlobalPopulationChange() > 0)
	{
		iValue += (pkBuildingInfo->GetPopulationChange() + kPlayer.getTotalPopulation()) * 5;
	}

	if (pkBuildingInfo->IsReformation() || pkBuildingInfo->GetReformationFollowerReduction() != 0)
	{
		ReligionTypes eReligion = kPlayer.GetReligions()->GetReligionCreatedByPlayer();
		if (eReligion != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligion, pCity->getOwner());
			if (pReligion && !pReligion->m_bReformed)
			{
				iValue += 100;
			}
		}
	}
	if(pkBuildingInfo->IsAllowsPuppetPurchase() && pCity->IsPuppet())
	{
		iValue += (25 + pCity->getPopulation()) * 5;
	}

	if (pkBuildingInfo->GetFoodKept() > 0)
	{
		iValue += pkBuildingInfo->GetFoodKept() * pCity->getPopulation();
	}

	if (pkBuildingInfo->AllowsFoodTradeRoutes())
	{
		iValue += 10 * pCity->getPopulation();
	}
	if (pkBuildingInfo->AllowsFoodTradeRoutesGlobal())
	{
		iValue += 25 * pCity->getPopulation();
	}
	if (pkBuildingInfo->AllowsProductionTradeRoutes())
	{
		iValue += 10 * pCity->getPopulation();
	}
	if (pkBuildingInfo->AllowsProductionTradeRoutesGlobal())
	{
		iValue += 25 * pCity->getPopulation();
	}

	if (pkBuildingInfo->GetFreeTechs() > 0)
	{
		//Earlier techs pretty useful...
		iValue += (100 + (GC.getNumTechInfos() - GET_TEAM(kPlayer.getTeam()).GetTeamTechs()->GetNumTechsKnown()));
	}
	for (int iUnitLoop = 0; iUnitLoop < GC.getNumUnitInfos(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if (pkUnitInfo)
		{
			int iNumUnits = pkBuildingInfo->GetNumFreeUnits(iUnitLoop);
			if (iNumUnits > 0)
			{
				if (pkUnitInfo->GetSpecialUnitType() != NO_SPECIALUNIT)
				{
					iValue += (100 * iNumUnits);
				}
				else if (pkUnitInfo->IsFound())
				{
					iValue += (50 * iNumUnits);
				}
				else
				{
					iValue += (10 * iNumUnits);
				}
			}
		}
	}
	if(kPlayer.GetPlayerTraits()->GetCapitalBuildingDiscount(eBuilding) > 0 && !pCity->isCapital() && kPlayer.getCapitalCity() != NULL)
	{
		if (kPlayer.getCapitalCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		{
			iValue += kPlayer.GetPlayerTraits()->GetCapitalBuildingDiscount(eBuilding);
		}
	}
	if(pCity->isCapital() && kPlayer.GetPlayerTraits()->GetCapitalBuildingModifier() > 0)
	{
		iValue += kPlayer.GetPlayerTraits()->GetCapitalBuildingModifier();
	}
	if(pkBuildingInfo->GetXBuiltTriggersIdeologyChoice())
	{
		if (kPlayer.getBuildingClassCount((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType()) < pkBuildingInfo->GetXBuiltTriggersIdeologyChoice())
		{
			iValue += 250 * pkBuildingInfo->GetXBuiltTriggersIdeologyChoice();
		}
	}
	if(pkBuildingInfo->GetCityConnectionTradeRouteModifier() != 0 && pCity->IsRouteToCapitalConnected())
	{
		iValue += pkBuildingInfo->GetCityConnectionTradeRouteModifier() * kPlayer.getNumCities();
	}
	if(pkBuildingInfo->GetCityCountUnhappinessMod() != 0)
	{
		iValue += (kPlayer.getNumCities() * 5);
	}

	if(pkBuildingInfo->GetFreeBuildingThisCity() != NO_BUILDINGCLASS)
	{
		const CvCivilizationInfo& thisCiv = pCity->getCivilizationInfo();
		BuildingTypes eFreeBuildingThisCity = (BuildingTypes)(thisCiv.getCivilizationBuildings(pkBuildingInfo->GetFreeBuildingThisCity()));

		if (eFreeBuildingThisCity != NO_BUILDING)
		{
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eFreeBuildingThisCity);
			if (pkBuildingInfo)
			{
				int iFreeValue = kPlayer.getCapitalCity()->GetCityStrategyAI()->GetBuildingProductionAI()->CheckBuildingBuildSanity(eFreeBuildingThisCity, 30, 10, 10, 10, false, false, true);
				if (iFreeValue > 0)
				{
					iValue += iFreeValue;
				}
			}
		}
	}
	if(pkBuildingInfo->GetCultureRateModifier() > 0)
	{
		iValue += (pkBuildingInfo->GetCultureRateModifier() + pCity->getCultureRateModifier()) * kPlayer.GetNumPolicies();
	}
	//this is super useful!
	if(pkBuildingInfo->GetGlobalCultureRateModifier() > 0)
	{
		iValue += pkBuildingInfo->GetGlobalCultureRateModifier() * (kPlayer.getNumCities() + kPlayer.GetNumPolicies());
	}
    if(kPlayer.GetPlayerTraits()->GetWonderProductionToBuildingDiscount(eBuilding) > 0)
    {
		iValue += pCity->getProductionModifier(eBuilding) + kPlayer.GetPlayerTraits()->GetWonderProductionToBuildingDiscount(eBuilding);
    }
	if (pkBuildingInfo->GetExtraMissionarySpreads() > 0)
	{
		int iNumNearbyCities = kPlayer.GetReligionAI()->GetNumCitiesWithReligionCalculator(kPlayer.GetReligions()->GetCurrentReligion());

		iValue += (iNumNearbyCities / 25);
	}

	if (pkBuildingInfo->GetExtraMissionarySpreadsGlobal() > 0)
	{
		int iNumNearbyCities = kPlayer.GetReligionAI()->GetNumCitiesWithReligionCalculator(kPlayer.GetReligions()->GetCurrentReligion());

		iValue += (iNumNearbyCities / 10);
	}

	if (pkBuildingInfo->GetExtraMissionaryStrength() > 0)
	{
		int iNumNearbyCities = kPlayer.GetReligionAI()->GetNumCitiesWithReligionCalculator(kPlayer.GetReligions()->GetCurrentReligion());

		iValue += (iNumNearbyCities / 25);
	}

	//Is this a prereq for another building we can build right now? 
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding2 = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo2 = GC.getBuildingInfo(eBuilding2);

		if (pkBuildingInfo2 && pkBuildingInfo2->GetPrereqAndTech() != NO_TECH && kPlayer.HasTech((TechTypes)pkBuildingInfo2->GetPrereqAndTech()))
		{
			if (pkBuildingInfo2->GetNeedBuildingThisCity() == eBuilding)
			{
				iValue += 100;
			}
		}
	}

	return iValue;
}
int  CityStrategyAIHelpers::GetBuildingTraitValue(CvCity *pCity, YieldTypes eYield, BuildingTypes eBuilding, int iValue)
{
	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

	//Skip if null
	if(pkBuildingInfo == NULL)
		return 0;

	if(pCity == NULL)
		return 0;

	CvPlayerAI& kPlayer = GET_PLAYER(pCity->getOwner());

	int iBonus = 0;

	//GWS
	GreatWorkSlotType eArtArtifactSlot = CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT();
	GreatWorkSlotType eWritingSlot = CvTypes::getGREAT_WORK_SLOT_LITERATURE();
	GreatWorkSlotType eMusicSlot = CvTypes::getGREAT_WORK_SLOT_MUSIC();
	
	if(pkBuildingInfo->GetGreatWorkSlotType() == eArtArtifactSlot)
	{
		iBonus += (pkBuildingInfo->GetGreatWorkCount() * kPlayer.GetPlayerTraits()->GetArtifactYieldChanges(eYield) * 2);
		iBonus += (pkBuildingInfo->GetGreatWorkCount() * kPlayer.GetPlayerTraits()->GetArtYieldChanges(eYield) * 2);
	}
	else if(pkBuildingInfo->GetGreatWorkSlotType() == eWritingSlot)
	{
		iBonus += (pkBuildingInfo->GetGreatWorkCount() * kPlayer.GetPlayerTraits()->GetLitYieldChanges(eYield) * 2);
	}
	else if(pkBuildingInfo->GetGreatWorkSlotType() == eMusicSlot)
	{
		iBonus += (pkBuildingInfo->GetGreatWorkCount() * kPlayer.GetPlayerTraits()->GetMusicYieldChanges(eYield) * 2);
	}
	
	//Strategy-specific yield bonuses (that lack a yield modifier)

	if(kPlayer.GetPlayerTraits()->GetBuildingClassYieldChange((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield) > 0)
	{
		iBonus += (kPlayer.GetPlayerTraits()->GetBuildingClassYieldChange((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType(), eYield) * 5);
	}

	if (isWorldWonderClass(pkBuildingInfo->GetBuildingClassInfo()))
	{
		if (kPlayer.GetPlayerTraits()->GetYieldChangeWorldWonder(eYield) > 0)
		{
			iBonus += (kPlayer.GetPlayerTraits()->GetYieldChangeWorldWonder(eYield) * 5);
		}
	}

	if(kPlayer.GetPlayerTraits()->GetGreatWorkYieldChanges(eYield) > 0 && pkBuildingInfo->GetGreatWorkCount() > 0)
	{
		iBonus += (kPlayer.GetPlayerTraits()->GetGreatWorkYieldChanges(eYield) * 5);
	}

	if(iValue > 0)
	{
		if (eYield == YIELD_GOLDEN_AGE_POINTS && kPlayer.GetPlayerTraits()->GetWLTKDGATimer() > 0)
		{
			iBonus += 50;
		}

		if(eYield == YIELD_SCIENCE)
		{
			if(kPlayer.GetPlayerTraits()->IsMayaCalendarBonuses())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->GetGreatScientistRateModifier() > 0)
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->IsTechBoostFromCapitalScienceBuildings())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->IsTechFromCityConquer())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->GetCombatBonusVsHigherTech() != 0)
			{
				iBonus -= 50;
			}
		}
		else if(eYield == YIELD_FAITH)
		{
			if(kPlayer.GetPlayerTraits()->IsUniqueBeliefsOnly())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->IsBonusReligiousBelief())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->IsReconquista())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->IsPopulationBoostReligion())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->GetFaithFromKills() > 0)
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->IsFaithFromUnimprovedForest())
			{
				iBonus += 50;
			}
		}
		else if(eYield == YIELD_GOLD)
		{
			if(kPlayer.GetPlayerTraits()->IsAbleToAnnexCityStates())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->IsDiplomaticMarriage())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->IsNoAnnexing())
			{
				iBonus += 50;
			}
			if(kPlayer.GetPlayerTraits()->GetLuxuryHappinessRetention())
			{
				iBonus += 50;
			}
		}

		if(MOD_BALANCE_CORE_JFD && (eYield == YIELD_JFD_CRIME || (eYield == YIELD_JFD_DISEASE)))
		{
			return iBonus *= -1;
		}
	}

	return iBonus;
}
#endif