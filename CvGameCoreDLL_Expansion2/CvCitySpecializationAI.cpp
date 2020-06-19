/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvCitySpecializationAI.h"
#include "ICvDLLUserInterface.h"
#include "CvGameCoreUtils.h"
#include "CvGrandStrategyAI.h"
#include "CvEconomicAI.h"
#include "CvMilitaryAI.h"
#include "CvEnumSerialization.h"
#include "CvWonderProductionAI.h"
#include "cvStopWatch.h"

// must be included after all other headers
#include "LintFree.h"



//=====================================
// CvCitySpecializationXMLEntry
//=====================================
CvCitySpecializationXMLEntry::CvCitySpecializationXMLEntry(void):
	m_piFlavorValue(NULL),
	m_piYieldTargetTimes100(NULL),
	m_eYieldType(NO_YIELD),
	m_iSubtype(0),
	m_bWonder(false),
	m_bDefault(false),
	m_bMustBeCoastal(false),
	m_bOperationUnitProvider(false)
{
}
//------------------------------------------------------------------------------
CvCitySpecializationXMLEntry::~CvCitySpecializationXMLEntry(void)
{
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piYieldTargetTimes100);
}
//------------------------------------------------------------------------------
bool CvCitySpecializationXMLEntry::CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility)
{
	if(!CvBaseInfo::CacheResults(kResults, kUtility))
		return false;

	//Basic Properties
	m_iSubtype = kResults.GetInt("SubType");

	//References
	const char* szYieldType = kResults.GetText("YieldType");
	if(szYieldType != NULL)
	{
		m_eYieldType = (YieldTypes)GC.getInfoTypeForString(szYieldType);
	}

	//Arrays
	const char* szType = GetType();
	kUtility.SetFlavors(m_piFlavorValue, "CitySpecialization_Flavors", "CitySpecializationType", szType);
	kUtility.SetYields(m_piYieldTargetTimes100, "CitySpecialization_TargetYields", "CitySpecializationType", szType);

	//Booleans
	m_bWonder = kResults.GetBool("IsWonder");
	m_bDefault = kResults.GetBool("IsDefault");
	m_bMustBeCoastal = kResults.GetBool("MustBeCoastal");
	m_bOperationUnitProvider = kResults.GetBool("IsOperationUnitProvider");

	return true;
}

/// What Flavors will be added by adopting this Strategy?
int CvCitySpecializationXMLEntry::GetFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : -1;
}

/// Yield associated with this specialization
YieldTypes CvCitySpecializationXMLEntry::GetYieldType() const
{
	return m_eYieldType;
}

int CvCitySpecializationXMLEntry::GetYieldTargetTimes100(YieldTypes eYield) const
{
	return m_piYieldTargetTimes100[eYield];
}


/// Subtype of this city specialization if there are more than one specializations for this yield
int CvCitySpecializationXMLEntry::GetSubtype() const
{
	return m_iSubtype;
}

/// Is this the subtype for building wonders (a special case)
bool CvCitySpecializationXMLEntry::IsWonder() const
{
	return m_bWonder;
}

/// Is this the default subtype used to fill in gaps (a special case)
bool CvCitySpecializationXMLEntry::IsDefault() const
{
	return m_bDefault;
}

/// Is this a specialization reserved for coastal cities?
bool CvCitySpecializationXMLEntry::IsMustBeCoastal() const
{
	return m_bMustBeCoastal;
}

/// Is this a specialization used to provide units to operations?
bool CvCitySpecializationXMLEntry::IsOperationUnitProvider() const
{
	return m_bOperationUnitProvider;
}

//=====================================
// CvCitySpecializationXMLEntries
//=====================================
/// Constructor
CvCitySpecializationXMLEntries::CvCitySpecializationXMLEntries(void)
{

}

/// Destructor
CvCitySpecializationXMLEntries::~CvCitySpecializationXMLEntries(void)
{
	DeleteArray();
}

/// Returns vector of AIStrategy entries
std::vector<CvCitySpecializationXMLEntry*>& CvCitySpecializationXMLEntries::GetCitySpecializationEntries()
{
	return m_paCitySpecializationEntries;
}

/// Number of defined AIStrategies
int CvCitySpecializationXMLEntries::GetNumCitySpecializations()
{
	return m_paCitySpecializationEntries.size();
}

/// Get a specific entry
CvCitySpecializationXMLEntry* CvCitySpecializationXMLEntries::GetEntry(int index)
{
#if defined(MOD_BALANCE_CORE)
	return (index!=NO_CITY_SPECIALIZATION) ? m_paCitySpecializationEntries[index] : NULL;
#else
	return m_paCitySpecializationEntries[index];
#endif
}

/// Find the first specializations for a yield
CitySpecializationTypes CvCitySpecializationXMLEntries::GetFirstSpecializationForYield(YieldTypes eYield)
{
	m_CurrentYield = eYield;
	for(m_CurrentIndex = 0; m_CurrentIndex < (int)m_paCitySpecializationEntries.size(); m_CurrentIndex++)
	{
		if(m_paCitySpecializationEntries[m_CurrentIndex]->GetYieldType() == m_CurrentYield)
		{
			return (CitySpecializationTypes)m_CurrentIndex;
		}
	}
	return NO_CITY_SPECIALIZATION;
}

/// Find the next specialization for a yield
CitySpecializationTypes CvCitySpecializationXMLEntries::GetNextSpecializationForYield()
{
	for(m_CurrentIndex = m_CurrentIndex++; m_CurrentIndex < (int)m_paCitySpecializationEntries.size(); m_CurrentIndex++)
	{
		if(m_paCitySpecializationEntries[m_CurrentIndex]->GetYieldType() == m_CurrentYield)
		{
			return (CitySpecializationTypes)m_CurrentIndex;
		}
	}
	return NO_CITY_SPECIALIZATION;
}

// How many specializations are there for this yield?
int CvCitySpecializationXMLEntries::GetNumSpecializationsForYield(YieldTypes eYield)
{
	int iRtnValue = 0;
	for(int iI = 0; iI < (int)m_paCitySpecializationEntries.size(); iI++)
	{
		if(m_paCitySpecializationEntries[iI]->GetYieldType() == eYield)
		{
			iRtnValue++;
		}
	}

	return iRtnValue;
}

/// Clear AIStrategy entries
void CvCitySpecializationXMLEntries::DeleteArray()
{
	for(std::vector<CvCitySpecializationXMLEntry*>::iterator it = m_paCitySpecializationEntries.begin(); it != m_paCitySpecializationEntries.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_paCitySpecializationEntries.clear();
}

//=====================================
// CvCitySpecializationAI
//=====================================

/// Constructor
CvCitySpecializationAI::CvCitySpecializationAI():
	m_bSpecializationsDirty(false),
	m_bInterruptWonders(false),
#if defined(MOD_BALANCE_CORE)
	m_bInterruptBuildings(false),
	m_bChooseNewWonder(false),
#endif
	m_eNextSpecializationDesired(NO_CITY_SPECIALIZATION),
	m_eNextWonderDesired(NO_BUILDING),
	m_iWonderCityID(-1),
	m_iNextWonderWeight(0),
	m_iLastTurnEvaluated(0)
{
}

/// Destructor
CvCitySpecializationAI::~CvCitySpecializationAI(void)
{
	Uninit();
}

/// Initialize
void CvCitySpecializationAI::Init(CvCitySpecializationXMLEntries* pSpecializations, CvPlayer* pPlayer)
{
	m_pSpecializations = pSpecializations;
	m_pPlayer = pPlayer;

	Reset();
}

/// Deallocate memory created in initialize
void CvCitySpecializationAI::Uninit()
{
}

/// Reset AIStrategy status array to all false
void CvCitySpecializationAI::Reset()
{
	m_bSpecializationsDirty = false;
	m_bInterruptWonders = false;
#if defined(MOD_BALANCE_CORE)
	m_bInterruptBuildings = false;
	m_bChooseNewWonder = false;
#endif
	m_eNextSpecializationDesired = NO_CITY_SPECIALIZATION;
	m_eNextWonderDesired = NO_BUILDING;
	m_iWonderCityID = -1;
	m_iNextWonderWeight = 0;
	m_YieldWeights.clear();
	m_ProductionSubtypeWeights.clear();
	m_iLastTurnEvaluated = 0;
}

/// Serialization read
void CvCitySpecializationAI::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_bSpecializationsDirty;
	kStream >> m_bInterruptWonders;
#if defined(MOD_BALANCE_CORE)
	kStream >> m_bInterruptBuildings;
	kStream >> m_bChooseNewWonder;
#endif
	kStream >> m_eNextSpecializationDesired;
	kStream >> (int&)m_eNextWonderDesired;
	kStream >> m_iWonderCityID;
	kStream >> m_iNextWonderWeight;

	if (uiVersion >= 2)
	{
		kStream >> m_iLastTurnEvaluated;
	}
	else
	{
		m_iLastTurnEvaluated = 0;
		m_bSpecializationsDirty = true;
	}
}

/// Serialization write
void CvCitySpecializationAI::Write(FDataStream& kStream) const
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_bSpecializationsDirty;
	kStream << m_bInterruptWonders;
#if defined(MOD_BALANCE_CORE)
	kStream << m_bInterruptBuildings;
	kStream << m_bChooseNewWonder;
#endif
	kStream << m_eNextSpecializationDesired;
	kStream << m_eNextWonderDesired;
	kStream << m_iWonderCityID;
	kStream << m_iNextWonderWeight;
	kStream << m_iLastTurnEvaluated;
}

/// Returns the Player object the Strategies are associated with
CvPlayer* CvCitySpecializationAI::GetPlayer()
{
	return m_pPlayer;
}

/// Returns AIStrategies object stored in this class
CvCitySpecializationXMLEntries* CvCitySpecializationAI::GetCitySpecializations()
{
	return m_pSpecializations;
}

/// Called every turn to see what Strategies this player should using (or not)
void CvCitySpecializationAI::DoTurn()
{
	AI_PERF_FORMAT("AI-perf.csv", ("CvCitySpecializationAI::DoTurn, Turn %03d, %s", GC.getGame().getElapsedGameTurns(), GetPlayer()->getCivilizationShortDescription()) );

	int iCityLoop = 0;

	// No city specializations for humans!
	if(m_pPlayer->isHuman())
	{
		return;
	}

	// No city specializations for minor civs
	if(m_pPlayer->isMinorCiv())
	{
		return;
	}

	// No city specializations early in the game
	if(GC.getGame().getGameTurn() < GC.getAI_CITY_SPECIALIZATION_EARLIEST_TURN())
	{
		return;
	}

	CvCity* pLoopCity = NULL;
	for (pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		if (pLoopCity->isInDangerOfFalling() || pLoopCity->isUnderSiege())
		{
			SetSpecializationsDirty(SPECIALIZATION_UPDATE_CITIES_UNDER_SIEGE);
			break;
		}
	}
	if (m_bChooseNewWonder)
	{
		BuildingTypes eOldWonder = m_eNextWonderDesired;
		m_eNextWonderDesired = m_pPlayer->GetWonderProductionAI()->ChooseWonder(true /*bAdjustForOtherPlayers*/, m_iNextWonderWeight);

		if (eOldWonder != m_eNextWonderDesired)
		{
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(m_eNextWonderDesired);
			if (pkBuildingInfo)
				LogMsg(CvString("Next desired wonder is ") + pkBuildingInfo->GetDescription());
		}

		m_bChooseNewWonder = false;
	}

	// No city specialization if we don't have enough cities
	if (m_pPlayer->getNumCities() < 2)
	{
		return;
	}

	// See if need to update assignments
	if(m_bSpecializationsDirty || ((m_iLastTurnEvaluated + GC.getAI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL()) <= GC.getGame().getGameTurn()))
	{
		WeightSpecializations();
		AssignSpecializations();
		m_bSpecializationsDirty = false;
		m_iLastTurnEvaluated = GC.getGame().getGameTurn();

#if defined(MOD_BALANCE_CORE)
		// Do we need to choose production again at all our cities?
		if(m_bInterruptWonders || m_bInterruptBuildings)
#else
		// Do we need to choose production again at all our cities?
		if(m_bInterruptWonders)
#endif
		{
			CvCity* pLoopCity = NULL;
			for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
			{
				if(pLoopCity->isProductionBuilding())
				{
#if defined(MOD_BALANCE_CORE)
					pLoopCity->AI_chooseProduction(m_bInterruptWonders, m_bInterruptBuildings);
#else
					pLoopCity->AI_chooseProduction(true /*bInterruptWonders*/);
#endif
				}
			}
		}
	}

	// Reset this flag -- need a new high priority event before we'll interrupt again
	m_bInterruptWonders = false;
	m_bInterruptBuildings = false;
}

/// Change specializations for cities since world state has changed
void CvCitySpecializationAI::SetSpecializationsDirty(CitySpecializationUpdateType eUpdateType)
{
	// No city specializations for minor civs
	if(!m_pPlayer->isMinorCiv())
	{
		m_bSpecializationsDirty = true;
		LogSpecializationUpdate(eUpdateType);

		switch(eUpdateType)
		{
		case SPECIALIZATION_UPDATE_NOW_AT_WAR:
			m_bInterruptBuildings = true;
			break;
		case SPECIALIZATION_UPDATE_MY_CITY_CAPTURED:
			m_bInterruptBuildings = true;
			m_bInterruptWonders = true;
			break;
		case SPECIALIZATION_UPDATE_CITIES_UNDER_SIEGE:
#if defined(MOD_BALANCE_CORE)
			m_bInterruptBuildings = true;
			m_bInterruptWonders = true;
#endif
			break;
		case SPECIALIZATION_UPDATE_WONDER_BUILT_BY_RIVAL:
			m_bChooseNewWonder = true;
			break;
		case SPECIALIZATION_UPDATE_CITY_FOUNDED:
			m_bChooseNewWonder = true;
			break;
		case SPECIALIZATION_UPDATE_WONDER_BUILT_BY_US:
			m_bChooseNewWonder = true;
			break;

		default:
			// Don't set it to false for these other cases!
			// We shouldn't set it to false until after the next time we've picked specializations.
			break;
		}
	}

	return;
}

/// Which city should build the next wonder?
CvCity* CvCitySpecializationAI::GetWonderBuildCity() const
{
	CvCity* pRtnValue = NULL;
	if(m_iWonderCityID != -1)
	{
		pRtnValue = m_pPlayer->getCity(m_iWonderCityID);
	}
	else
	{
		int iLoop;
		CvCity* pLoopCity;
		for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			BuildingTypes eCurrentBuilding = pLoopCity->getProductionBuilding();
			//alright, we found a city with the wonder we want.
			if (m_eNextWonderDesired != NO_BUILDING && eCurrentBuilding != NO_BUILDING && eCurrentBuilding == m_eNextWonderDesired)
			{
				return pLoopCity;
			}
		}
	}
	return pRtnValue;
}

// PRIVATE METHODS
#if defined(MOD_BALANCE_CORE)
//Helper functions to round
static double citystrategyround(double x)
{
	return (x >= 0) ? floor(x + .5) : ceil(x - .5);
};
#endif
/// Evaluate which specializations we need
void CvCitySpecializationAI::WeightSpecializations()
{
	int iFoodYieldWeight = 1;
	int iProductionYieldWeight = 1;
	int iGoldYieldWeight = 1;
	int iScienceYieldWeight = 1;
	int iGeneralEconomicWeight = 1;
#if defined(MOD_BALANCE_CORE)
	int iCultureYieldWeight = 1;
	int iFaithYieldWeight = 1;
#endif

	// Clear old weights
	m_YieldWeights.clear();
	m_ProductionSubtypeWeights.clear();

	// Must have a capital to do any specialization
	if(m_pPlayer->getCapitalCity() != NULL)
	{
		//int iFlavorExpansion = 0;
		int iFlavorWonder = 0;
		int iFlavorGold = 0;
		int iFlavorScience = 0;
		int iFlavorSpaceship = 0;

		//iFlavorExpansion = m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_EXPANSION"));
		//if(iFlavorExpansion < 0) iFlavorExpansion = 0;
		iFlavorWonder = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_WONDER"));
		if(iFlavorWonder < 0) iFlavorWonder = 0;
		iFlavorGold = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
		if(iFlavorGold < 0) iFlavorGold = 0;
		iFlavorScience = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
		if(iFlavorScience < 0) iFlavorScience = 0;
		iFlavorSpaceship = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SPACESHIP"));
		if(iFlavorSpaceship < 0) iFlavorSpaceship = 0;
#if defined(MOD_BALANCE_CORE)
		int iFlavorGrowth = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
		if(iFlavorGrowth < 0) iFlavorGrowth = 0;
		int iFlavorCulture = 10 * (m_pPlayer->GetCulture()->GetNumCivsInfluentialOn() + m_pPlayer->GetFlavorManager()->GetIndividualFlavor((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE")));
		if(iFlavorCulture < 0) iFlavorCulture = 0;
		int iFlavorFaith = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
		if(iFlavorFaith < 0) iFlavorFaith = 0;
#endif

		// COMPUTE NEW WEIGHTS
#if defined(MOD_BALANCE_CORE)
		//City Loop (to encourage specializations that capitalize on what we're good at).		
		CvCity* pLoopCity;
		int iLoop;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(pLoopCity == NULL || pLoopCity->IsPuppet())
			{
				continue;
			}
			
			//Highest First
			YieldTypes eHighestYield = pLoopCity->GetCityStrategyAI()->GetHighestYield();
			int iYieldAverage = (int)citystrategyround(pLoopCity->GetCityStrategyAI()->GetYieldAverage(eHighestYield));

			if(eHighestYield == YIELD_FOOD)
			{
				iFoodYieldWeight += iYieldAverage;
			}
			else if(eHighestYield == YIELD_PRODUCTION)
			{
				iProductionYieldWeight += iYieldAverage;
			}
			else if(eHighestYield == YIELD_GOLD)
			{
				iGoldYieldWeight += iYieldAverage;
			}
			else if(eHighestYield == YIELD_SCIENCE)
			{
				iScienceYieldWeight += iYieldAverage;
			}
			else if(eHighestYield == YIELD_CULTURE)
			{
				iCultureYieldWeight += iYieldAverage;
			}
			else if(eHighestYield == YIELD_FAITH)
			{
				iFaithYieldWeight += iYieldAverage;
			}

			iGeneralEconomicWeight += (iYieldAverage / 3);

			//Then Lowest
			YieldTypes eLowestYield = pLoopCity->GetCityStrategyAI()->GetMostDeficientYield();
			int iLowestYieldAverage = (int)citystrategyround(pLoopCity->GetCityStrategyAI()->GetYieldAverage(eLowestYield));

			int iDelta = iYieldAverage - iLowestYieldAverage;

			if(eLowestYield == YIELD_FOOD)
			{
				iFoodYieldWeight += iDelta;
			}
			else if(eLowestYield == YIELD_PRODUCTION)
			{
				iProductionYieldWeight += iDelta;
			}
			else if(eLowestYield == YIELD_GOLD)
			{
				iGoldYieldWeight += iDelta;
			}
			else if(eLowestYield == YIELD_SCIENCE)
			{
				iScienceYieldWeight += iDelta;
			}
			else if(eLowestYield == YIELD_CULTURE)
			{
				iCultureYieldWeight += iDelta;
			}
			else if(eLowestYield == YIELD_FAITH)
			{
				iFaithYieldWeight += iDelta;
			}

			iGeneralEconomicWeight += (iDelta / 3);
		}			

		//   Add in any contribution from the current grand strategy
		for(int iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
		{
			CvAIGrandStrategyXMLEntry* grandStrategy = GC.getAIGrandStrategyInfo((AIGrandStrategyTypes)iGrandStrategyLoop);
			if(grandStrategy)
			{
				if(iGrandStrategyLoop == m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy())
				{
					if(grandStrategy->GetSpecializationBoost(YIELD_FOOD) > 0)
					{
						iFoodYieldWeight *=	grandStrategy->GetSpecializationBoost(YIELD_FOOD);
						iFoodYieldWeight /= 100;
					}
					else if(grandStrategy->GetSpecializationBoost(YIELD_GOLD) > 0)
					{
						iGoldYieldWeight *=	grandStrategy->GetSpecializationBoost(YIELD_GOLD);
						iGoldYieldWeight /= 100;
					}
					else if(grandStrategy->GetSpecializationBoost(YIELD_SCIENCE) > 0)
					{
						iScienceYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_SCIENCE);
						iScienceYieldWeight /= 100;
					}
					else if(grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION) > 0)
					{
						iProductionYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION);
						iProductionYieldWeight /= 100;
					}
					else if(grandStrategy->GetSpecializationBoost(YIELD_CULTURE) > 0)
					{
						iCultureYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_CULTURE);
						iCultureYieldWeight /= 100;
					}
					else if(grandStrategy->GetSpecializationBoost(YIELD_FAITH) > 0)
					{
						iFaithYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_FAITH);
						iFaithYieldWeight /= 100;
					}
				}
			}
		}

		//Food
		iFoodYieldWeight *= (100 + iFlavorGrowth);
		iFoodYieldWeight /= 100;

		//Subtype considerations for Production (instead of flavor)
		iProductionYieldWeight *= (100 + (WeightProductionSubtypes(iFlavorWonder, iFlavorSpaceship) / 125));
		iProductionYieldWeight /= 100;

		//Gold
		iGoldYieldWeight *= (100 + iFlavorGold);
		iGoldYieldWeight /= 100;

		//Science
		iScienceYieldWeight *= (100 + iFlavorScience);
		iScienceYieldWeight /= 100;

		//Culture
		iCultureYieldWeight *= (100 + iFlavorCulture);
		iCultureYieldWeight /= 100;

		//Faith
		iFaithYieldWeight *= (100 + iFlavorFaith);
		iFaithYieldWeight /= 100;

		//   General Economics
		iGeneralEconomicWeight *= (100 + ((iFlavorGrowth + iFlavorGold + iFlavorCulture + iFlavorFaith + iFlavorScience) / 5));
		iGeneralEconomicWeight /= 100;

		// Add weights to our weighted vector
		m_YieldWeights.push_back(YIELD_FOOD, iFoodYieldWeight);
		m_YieldWeights.push_back(YIELD_PRODUCTION, iProductionYieldWeight);
		m_YieldWeights.push_back(YIELD_GOLD, iGoldYieldWeight);
		m_YieldWeights.push_back(YIELD_SCIENCE, iScienceYieldWeight);
		m_YieldWeights.push_back(YIELD_CULTURE, iCultureYieldWeight);
		m_YieldWeights.push_back(YIELD_FAITH, iFaithYieldWeight);
		m_YieldWeights.push_back(NO_YIELD, iGeneralEconomicWeight);

		// Log results
		LogSpecializationWeights();
#else
		//   Food
		CvArea* pArea = GC.getMap().getArea(m_pPlayer->getCapitalCity()->getArea());
		int iNumUnownedTiles = pArea->getNumUnownedTiles();
		int iNumCities = m_pPlayer->getNumCities();
		int iNumSettlers = m_pPlayer->GetNumUnitsWithUnitAI(UNITAI_SETTLE, true);
		EconomicAIStrategyTypes eStrategy = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
		if(eStrategy != NO_ECONOMICAISTRATEGY && m_pPlayer->GetEconomicAI()->IsUsingStrategy(eStrategy))
		{
			iFoodYieldWeight += GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_EARLY_EXPANSION() /* 500 */;
		}
		iFoodYieldWeight *= max(1, iFlavorGrowth);
		iFoodYieldWeight += iFlavorExpansion * GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_FLAVOR_EXPANSION() /* 5 */;
		iFoodYieldWeight += (iNumUnownedTiles * 100) / pArea->getNumTiles() * GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_PERCENT_CONTINENT_UNOWNED() /* 5 */;
		iFoodYieldWeight += iNumCities * GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_CITIES() /* -50 */;
		iFoodYieldWeight += iNumSettlers * GC.getAI_CITY_SPECIALIZATION_FOOD_WEIGHT_NUM_SETTLERS() /* -40 */;
		if((iNumCities + iNumSettlers) == 1)
		{
			iFoodYieldWeight *= 3;   // Really want to get up over 1 city
		}
		if(iFoodYieldWeight < 0) iFoodYieldWeight = 0;

		//   Production
		iProductionYieldWeight = WeightProductionSubtypes(iFlavorWonder, iFlavorSpaceship);

		//   Trade
		int iLandDisputeLevel = m_pPlayer->GetDiplomacyAI()->GetTotalLandDisputeLevel();
		iGoldYieldWeight += iFlavorGold * GC.getAI_CITY_SPECIALIZATION_GOLD_WEIGHT_FLAVOR_GOLD() /* 20 */;
		iGoldYieldWeight += iLandDisputeLevel * GC.getAI_CITY_SPECIALIZATION_GOLD_WEIGHT_LAND_DISPUTE() /* 10 */;

		//   Science
		iScienceYieldWeight += iFlavorScience * GC.getAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SCIENCE() /* 20 */;
		iScienceYieldWeight += iFlavorSpaceship * GC.getAI_CITY_SPECIALIZATION_SCIENCE_WEIGHT_FLAVOR_SPACESHIP() /* 10 */;

		//   General Economics
		iGeneralEconomicWeight = GC.getAI_CITY_SPECIALIZATION_GENERAL_ECONOMIC_WEIGHT() /* 200 */;

		//   Add in any contribution from the current grand strategy
		for(int iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
		{
			CvAIGrandStrategyXMLEntry* grandStrategy = GC.getAIGrandStrategyInfo((AIGrandStrategyTypes)iGrandStrategyLoop);
			if(grandStrategy)
			{
				if(iGrandStrategyLoop == m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy())
				{
					iFoodYieldWeight +=	grandStrategy->GetSpecializationBoost(YIELD_FOOD);
					iGoldYieldWeight += grandStrategy->GetSpecializationBoost(YIELD_GOLD);
					iScienceYieldWeight += grandStrategy->GetSpecializationBoost(YIELD_SCIENCE);
				}
			}
		}

		// Add weights to our weighted vector
		m_YieldWeights.push_back(YIELD_FOOD, iFoodYieldWeight);
		m_YieldWeights.push_back(YIELD_PRODUCTION, iProductionYieldWeight);
		m_YieldWeights.push_back(YIELD_GOLD, iGoldYieldWeight);
		m_YieldWeights.push_back(YIELD_SCIENCE, iScienceYieldWeight);
		m_YieldWeights.push_back(NO_YIELD, iGeneralEconomicWeight);

		// Log results
		LogSpecializationWeights();
#endif
	}

	return;
}

/// Compute the weight of each production subtype (return value is total of all these weights)
int CvCitySpecializationAI::WeightProductionSubtypes(int iFlavorWonder, int iFlavorSpaceship)
{
	bool bCriticalDefenseOn = false;

	int iMilitaryTrainingWeight = 0;
	int iEmergencyUnitWeight = 0;
	int iSeaWeight = 0;
	int iWonderWeight = 0;
	int iSpaceshipWeight = 0;

	int iFlavorOffense = 0;
	iFlavorOffense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));

	int iUnitsRequested = m_pPlayer->GetNumUnitsNeededToBeBuilt();

	// LONG-TERM MILITARY BUILD-UP
	iMilitaryTrainingWeight += (iFlavorOffense * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE()) /* 10 */;
	iMilitaryTrainingWeight += (m_pPlayer->GetDiplomacyAI()->GetPersonalityMajorCivApproachBias(MAJOR_CIV_APPROACH_WAR) * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY() /* 10 */);

	// EMERGENCY UNITS
	iEmergencyUnitWeight += iUnitsRequested * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED() /* 10 */;
	iEmergencyUnitWeight += m_pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith(false) * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH() /* 100 */;

	// Is our capital under threat?
	AICityStrategyTypes eCityStrategy = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_CAPITAL_UNDER_THREAT");
	CvCity* pCapital;
	pCapital = m_pPlayer->getCapitalCity();
	if(pCapital && eCityStrategy != NO_AICITYSTRATEGY && pCapital->GetCityStrategyAI()->IsUsingCityStrategy(eCityStrategy))
	{
		iEmergencyUnitWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT() /* 50 */;
	}

	// Add in weights depending on what the military AI is up to
	MilitaryAIStrategyTypes eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iMilitaryTrainingWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION() /* 150 */;
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iEmergencyUnitWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE() /* 150 */;
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		bCriticalDefenseOn = true;
		iEmergencyUnitWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL() /* 1000 */;
	}

	// Override all this if have too many units!
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_MILITARY_UNITS");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iMilitaryTrainingWeight = 0;
		iEmergencyUnitWeight = 0;
	}

	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_UNITS");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iSeaWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS() /* 50 */;
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_UNITS_CRITICAL");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iSeaWeight += GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL() /* 250 */;
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_NAVAL_UNITS");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iSeaWeight = 0;
	}

	// Wonder is MIN between weight of wonders available to build and value from flavors (but not less than zero)
	int iWonderFlavorWeight = iFlavorWonder * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER() /* 200 */;
	int iWeightOfWonders = (int)(m_iNextWonderWeight * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER()); /* 0.2 */
	iWonderWeight = min(iWonderFlavorWeight, iWeightOfWonders);
	iWonderWeight = max(iWonderWeight, 0);

	// One-half of normal weight if critical defense is on
	if(bCriticalDefenseOn)
	{
		iWonderWeight /= 2;
	}

	if(CanBuildSpaceshipParts())
	{
		iSpaceshipWeight += iFlavorSpaceship * GC.getAI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP() /* 5 */;
	}

	for(int iGrandStrategyLoop = 0; iGrandStrategyLoop < GC.getNumAIGrandStrategyInfos(); iGrandStrategyLoop++)
	{
		CvAIGrandStrategyXMLEntry* grandStrategy = GC.getAIGrandStrategyInfo((AIGrandStrategyTypes)iGrandStrategyLoop);
		if(grandStrategy)
		{
			if(iGrandStrategyLoop == m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy())
			{
				if(grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION) > 0)
				{
					if(grandStrategy->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE")) > 0)
					{
						iMilitaryTrainingWeight += grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION);
					}
					else if(grandStrategy->GetFlavorValue((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SPACESHIP")) > 0)
					{
						iSpaceshipWeight += grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION);
					}
				}
			}
		}
	}

	// Add weights to our weighted vector
	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_MILITARY_TRAINING, iMilitaryTrainingWeight);
	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_EMERGENCY_UNITS, iEmergencyUnitWeight);
	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_MILITARY_NAVAL, iSeaWeight);
	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_WONDER, iWonderWeight);
	m_ProductionSubtypeWeights.push_back(PRODUCTION_SPECIALIZATION_SPACESHIP, iSpaceshipWeight);

	return iMilitaryTrainingWeight + iEmergencyUnitWeight + iSeaWeight + iWonderWeight + iSpaceshipWeight;
}

/// Assign specializations to cities
void CvCitySpecializationAI::AssignSpecializations()
{
	int iI;
	CitySpecializationTypes eSpecialization;
	CitySpecializationData cityData;
	list<CitySpecializationData> citiesWithoutSpecialization;
	list<CitySpecializationTypes>::iterator it;
	list<CitySpecializationTypes>::iterator iterEnd;
	list<CitySpecializationData>::iterator cityIter;
	list<CitySpecializationData>::iterator cityIterEnd;
	list<CitySpecializationData>::iterator bestCity;

	m_eNextSpecializationDesired = NO_CITY_SPECIALIZATION;
	citiesWithoutSpecialization.clear();

	CitySpecializationTypes eWonderSpecialiation = GetWonderSpecialization();

	// Find specializations needed (including for the next city we build)
	SelectSpecializations();

	// OBVIOUS ASSIGNMENTS: Loop through our cities making obvious assignments
	CvCity* pLoopCity;
	int iLoop;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->IsPuppet())
		{
			pLoopCity->GetCityStrategyAI()->SetSpecialization(GetEconomicDefaultSpecialization());
			LogSpecializationAssignment(pLoopCity, GetEconomicDefaultSpecialization(), true);
			continue;
		}

		// If this is the city to build our current wonder in, mark all that
		if(m_bWonderChosen && pLoopCity->GetID() == m_iWonderCityID)
		{
			it = find(m_SpecializationsNeeded.begin(), m_SpecializationsNeeded.end(), eWonderSpecialiation);
			if(it != m_SpecializationsNeeded.end())
			{
				m_SpecializationsNeeded.erase(it);
				pLoopCity->GetCityStrategyAI()->SetSpecialization(eWonderSpecialiation);
				LogSpecializationAssignment(pLoopCity, eWonderSpecialiation, true);
				continue;
			}
		}

		// If city default is equal to a needed type, go with that
		eSpecialization = pLoopCity->GetCityStrategyAI()->GetDefaultSpecialization();
		it = find(m_SpecializationsNeeded.begin(), m_SpecializationsNeeded.end(), eSpecialization);
		if(it != m_SpecializationsNeeded.end())
		{
			m_SpecializationsNeeded.erase(it);
			pLoopCity->GetCityStrategyAI()->SetSpecialization(eSpecialization);
			LogSpecializationAssignment(pLoopCity, eSpecialization);
		}
		else
		{
			// If cities' current specialization is needed, stick with that
			eSpecialization = pLoopCity->GetCityStrategyAI()->GetSpecialization();
			it = find(m_SpecializationsNeeded.begin(), m_SpecializationsNeeded.end(), eSpecialization);
			if(it != m_SpecializationsNeeded.end())
			{
				m_SpecializationsNeeded.erase(it);
				pLoopCity->GetCityStrategyAI()->SetSpecialization(eSpecialization);
				LogSpecializationAssignment(pLoopCity, eSpecialization);
			}

			// Save this city off (with detailed data about what it is good at) to assign later
			else
			{
				cityData.m_eID = pLoopCity->GetID();
#if defined(MOD_BALANCE_CORE)
				for(iI = 0; iI <= YIELD_FAITH; iI++)
#else
				for(iI = 0; iI <= YIELD_SCIENCE; iI++)
#endif
				{
					if(iI == YIELD_SCIENCE)
					{
						cityData.m_iWeight[iI] = PlotValueForScience(pLoopCity->plot()); // -- BKW, looks like PlotValueForScience is making some assumptions that are no longer true
					}
#if defined(MOD_BALANCE_CORE)
					else if (iI == YIELD_CULTURE)
					{
						cityData.m_iWeight[iI] = PlotValueForCulture(pLoopCity->plot()); 
					}
					else if (iI == YIELD_FAITH)
					{
						cityData.m_iWeight[iI] = PlotValueForFaith(pLoopCity->plot());
					}
#endif
					else
					{
						cityData.m_iWeight[iI] = PlotValueForSpecificYield(pLoopCity->plot(), (YieldTypes)iI);
					}
					cityData.m_iWeight[iI] = AdjustValueBasedOnBuildings(pLoopCity, (YieldTypes)iI, cityData.m_iWeight[iI]);
#if defined(MOD_BALANCE_CORE)
					if(MOD_BALANCE_CORE_HAPPINESS)
					{
						cityData.m_iWeight[iI] += AdjustValueBasedOnHappiness(pLoopCity, (YieldTypes)iI, cityData.m_iWeight[iI]);
					}
#endif
					if(cityData.m_iWeight[iI] < 0)
					{
						cityData.m_iWeight[iI] = 0;
					}
				}
				citiesWithoutSpecialization.push_back(cityData);

				LogCity(pLoopCity, cityData);

			}
		}
	}

	FAssert(citiesWithoutSpecialization.size() + 1 == m_SpecializationsNeeded.size());

	// NEXT SPECIALIZATION NEEDED: Now figure out what we want to assign as our "next specialization needed"

	// If only one specialization left, it's easy
	if(citiesWithoutSpecialization.empty())
	{
		it = m_SpecializationsNeeded.begin();
		m_eNextSpecializationDesired = *it;
		LogNextSpecialization(m_eNextSpecializationDesired);
		return;
	}

	// If all specializations left are "general economic", set that as next needed
	bool bAllGeneral = true;
	it = m_SpecializationsNeeded.begin();
	iterEnd = m_SpecializationsNeeded.end();
	for(; it != iterEnd; ++it)
	{
		CitySpecializationTypes eType = *it;
		if(eType != GetEconomicDefaultSpecialization())
		{
			bAllGeneral = false;
		}
	}

	if(bAllGeneral)
	{
		m_eNextSpecializationDesired = GetEconomicDefaultSpecialization();
		LogNextSpecialization(m_eNextSpecializationDesired);
		m_SpecializationsNeeded.erase(m_SpecializationsNeeded.begin());
	}

	else
	{
		// Find best possible sites for each of the yield types
		FindBestSites();

		// Compute the yield which we can improve the most with a new city
		int iCurrentDelta;
		int iBestDelta[YIELD_FAITH + 1];
		for(iI = 0; iI <= YIELD_FAITH; iI++)
			iBestDelta[iI] = MIN_INT;

		cityIter = citiesWithoutSpecialization.begin();
		cityIterEnd = citiesWithoutSpecialization.end();
		for(; cityIter != cityIterEnd; ++cityIter)
		{
			cityData = *cityIter;
#if defined(MOD_BALANCE_CORE)
			for(iI = 0; iI <= YIELD_FAITH; iI++)
#else
			for(iI = 0; iI <= YIELD_SCIENCE; iI++)
#endif
			{
				iCurrentDelta = cityData.m_iWeight[iI] - m_iBestValue[iI];
				if(iCurrentDelta > iBestDelta[iI])
				{
					iBestDelta[iI] = iCurrentDelta;
				}
			}
		}

		// Save yield improvements in a vector we can sort
#if defined(MOD_BALANCE_CORE)
		CvWeightedVector<int, YIELD_FAITH, true> yieldImprovements;
		for(iI = 0; iI <= YIELD_FAITH; iI++)
#else
		CvWeightedVector<int, YIELD_SCIENCE+1, true> yieldImprovements;
		for(iI = 0; iI <= YIELD_SCIENCE; iI++)
#endif
		{
			int iImprovementWithNewCity;
			if(iBestDelta[iI] > 0)
			{
				iImprovementWithNewCity = 0;
			}
			else
			{
				iImprovementWithNewCity = -iBestDelta[iI];
			}
			yieldImprovements.push_back(iI, iImprovementWithNewCity);
		}
		yieldImprovements.SortItems();

		// Take them out in order and see if we need this specialization
		bool bFoundIt = false;
		for(iI = 0; iI < yieldImprovements.size(); iI++)
		{
			YieldTypes eMostImprovedYield = (YieldTypes)yieldImprovements.GetElement(iI);

			// Loop through needed specializations until we find one that matches
			it = m_SpecializationsNeeded.begin();
			iterEnd = m_SpecializationsNeeded.end();
			for(; it != iterEnd; ++it)
			{
				CitySpecializationTypes eType = *it;
#if defined(MOD_BALANCE_CORE)
				CvCitySpecializationXMLEntry* pkCitySpecializationEntry = GC.getCitySpecializationInfo(eType);
				if(pkCitySpecializationEntry == NULL)
					continue;
#endif
				YieldTypes eYield = GC.getCitySpecializationInfo(eType)->GetYieldType();
				if(eYield == eMostImprovedYield)
				{
					m_eNextSpecializationDesired = eType;
					LogNextSpecialization(m_eNextSpecializationDesired);
					m_SpecializationsNeeded.erase(it);
					bFoundIt = true;
					break;
				}
			}
			if(bFoundIt)
			{
				break;
			}
		}
	}

	FAssert(citiesWithoutSpecialization.size() == m_SpecializationsNeeded.size());

	// REMAINING ASSIGNMENTS: Make the rest of the assignments
	it = m_SpecializationsNeeded.begin();
	iterEnd = m_SpecializationsNeeded.end();
	for(; it != iterEnd; ++it)
	{
		const CitySpecializationTypes eType = *it;
		CvCitySpecializationXMLEntry* pkCitySpecializationEntry = GC.getCitySpecializationInfo(eType);
		if(pkCitySpecializationEntry == NULL)
			continue;

		YieldTypes eYield = pkCitySpecializationEntry->GetYieldType();
		bool bCoastal = pkCitySpecializationEntry->IsMustBeCoastal();
		bestCity = citiesWithoutSpecialization.end();

		// Pick best existing city based on a better computation of existing city's value for a yield type
		int iBestValue = -1;
		cityIter = citiesWithoutSpecialization.begin();
		cityIterEnd = citiesWithoutSpecialization.end();
		for(; cityIter != cityIterEnd; ++cityIter)
		{
			cityData = *cityIter;

			if(bCoastal && !m_pPlayer->getCity(cityData.m_eID)->isCoastal())
			{
				continue;
			}

			if(eYield == NO_YIELD)
			{
				// General economic is all yields added together
				int iCityValue = 0;
#if defined(MOD_BALANCE_CORE)
				for(iI = 0; iI <= YIELD_FAITH; iI++)
#else
				for(iI = 0; iI <= YIELD_SCIENCE; iI++)
#endif
				{
					iCityValue += cityData.m_iWeight[iI];
				}
				if(iCityValue > iBestValue)
				{
					iBestValue = iCityValue;
					bestCity = cityIter;
				}
			}
			else
			{
				if(cityData.m_iWeight[(int)eYield] > iBestValue)
				{
					iBestValue = cityData.m_iWeight[(int)eYield];
					bestCity = cityIter;
				}
			}
		}

		// Found a city to set
		if(bestCity != citiesWithoutSpecialization.end())
		{
			CvCity* pCity = m_pPlayer->getCity(bestCity->m_eID);
			pCity->GetCityStrategyAI()->SetSpecialization(eType);
			LogSpecializationAssignment(pCity, eType);
			citiesWithoutSpecialization.erase(bestCity);
		}

		// No (coastal) city found, use default specialization as last resort
		else
		{
			CvCity* pCity = m_pPlayer->getCity(citiesWithoutSpecialization.begin()->m_eID);
			pCity->GetCityStrategyAI()->SetSpecialization(GetEconomicDefaultSpecialization());
			LogSpecializationAssignment(pCity, GetEconomicDefaultSpecialization());
			citiesWithoutSpecialization.erase(citiesWithoutSpecialization.begin());
		}
	}

	return;
}

/// Find specializations needed (including for the next city we build)
void CvCitySpecializationAI::SelectSpecializations()
{
	CitySpecializationTypes eSpecialization;
	int iSpecializationsToAssign = m_pPlayer->getNumCities() - m_pPlayer->GetNumPuppetCities() + 1;
	int iOldWeight;
	int iNewWeight;
	int iReductionAmount;

	m_SpecializationsNeeded.clear();
	m_bWonderChosen = false;

	// Clear info about what we've picked
	for(int iI = 0; iI < NUM_SPECIALIZATION_YIELDS; iI++)
	{
		m_iNumSpecializationsForThisYield[iI] = 0;
	}
	for(int iI = 0; iI < NUM_PRODUCTION_SPECIALIZATION_SUBTYPES; iI++)
	{
		m_iNumSpecializationsForThisSubtype[iI] = 0;
	}

	CvCity* pkWonderBuildCity = GetWonderBuildCity();
	CvBuildingEntry* pkProductionBuildingInfo = NULL;
	if(NULL != pkWonderBuildCity && pkWonderBuildCity->getProductionBuilding() != NO_BUILDING)
	{
		pkProductionBuildingInfo = GC.getBuildingInfo(pkWonderBuildCity->getProductionBuilding());
	}

	// Do we have a wonder build in progress that we can't interrupt?
	if(!m_bInterruptWonders && NULL != pkProductionBuildingInfo && m_pPlayer->GetWonderProductionAI()->IsWonder(*pkProductionBuildingInfo))
	{
		m_SpecializationsNeeded.push_back(GetWonderSpecialization());
		m_iNumSpecializationsForThisYield[1 + (int)YIELD_PRODUCTION]++;
		iOldWeight = m_YieldWeights.GetWeight(YIELD_PRODUCTION);
		iReductionAmount = m_ProductionSubtypeWeights.GetWeight(GetWonderSubtype());
		m_YieldWeights.SetWeight(YIELD_PRODUCTION, (iOldWeight - iReductionAmount));

		// Only one wonder at a time, so zero out the weight for this subtype entirely
		m_ProductionSubtypeWeights.SetWeight(GetWonderSubtype(), 0);
		m_bWonderChosen = true;
	}
	else
	{
		m_iWonderCityID = -1;
	}

	// LOOP as many times as we have cities PLUS ONE
	while(m_SpecializationsNeeded.size() < (size_t)iSpecializationsToAssign)
	{
		// Find highest weighted specialization
		m_YieldWeights.SortItems();

		// Mark that we need one city of this type
		YieldTypes eYield = m_YieldWeights.GetElement(0);

		if(GC.GetGameCitySpecializations()->GetNumSpecializationsForYield(eYield) > 1)
		{
			if(eYield == YIELD_PRODUCTION)
			{
				eSpecialization = SelectProductionSpecialization(iReductionAmount);

				iOldWeight = m_YieldWeights.GetWeight(0);
				m_iNumSpecializationsForThisYield[1 + (int)eYield]++;
				iNewWeight = iOldWeight - iReductionAmount;
				m_YieldWeights.SetWeight(0, iNewWeight);
			}
			else
			{
				FAssertMsg(false, "Code does not support > 1 specialization for yields other than production.");
			}
		}
		else
		{
			eSpecialization = GC.GetGameCitySpecializations()->GetFirstSpecializationForYield(eYield);

			// Reduce weight for this specialization based on dividing original weight by <num of this type + 1>
			iOldWeight = m_YieldWeights.GetWeight(0);
			m_iNumSpecializationsForThisYield[1 + (int)eYield]++;

			iNewWeight = iOldWeight * m_iNumSpecializationsForThisYield[1 + (int)eYield] / (m_iNumSpecializationsForThisYield[1 + (int)eYield] + 1);
			m_YieldWeights.SetWeight(0, iNewWeight);
		}
		m_SpecializationsNeeded.push_back(eSpecialization);
	}
}

/// Find production specializations needed
CitySpecializationTypes CvCitySpecializationAI::SelectProductionSpecialization(int& iReductionAmount)
{
	CitySpecializationTypes eSpecialization;

	// Find highest weighted subtype
	m_ProductionSubtypeWeights.SortItems();

	ProductionSpecializationSubtypes eSubtype = m_ProductionSubtypeWeights.GetElement(0);

	int iNumSubTypes = 0;
	eSpecialization = GC.GetGameCitySpecializations()->GetFirstSpecializationForYield(YIELD_PRODUCTION);
	while(iNumSubTypes != (int)eSubtype)
	{
		eSpecialization = GC.GetGameCitySpecializations()->GetNextSpecializationForYield();
		iNumSubTypes++;
		FAssertMsg(eSpecialization != NO_CITY_SPECIALIZATION, "Production specializations in XML doesn't match NUM_PRODUCTION_SPECIALIZATION_SUBTYPES");
	}

	// If this is the wonder type, make sure we have a city to build it
	if(eSubtype == GetWonderSubtype())
	{
		CvCity* pCity = FindBestWonderCity();
		if(pCity != NULL)
		{
			m_iWonderCityID = pCity->GetID();
			m_bWonderChosen = true;
		}

		// No wonder city, substitute default specialization instead
		else
		{
			eSpecialization = GetEconomicDefaultSpecialization();
		}
		iReductionAmount = m_ProductionSubtypeWeights.GetWeight(0);
		m_ProductionSubtypeWeights.SetWeight(0, 0);
	}

	else
	{
		// Reduce weight for this subtype based on dividing original weight by <num of this type + 1>
		int iOldWeight = m_ProductionSubtypeWeights.GetWeight(0);
		m_iNumSpecializationsForThisSubtype[(int)iNumSubTypes]++;
		int iNewWeight = iOldWeight * m_iNumSpecializationsForThisSubtype[(int)iNumSubTypes] / (m_iNumSpecializationsForThisSubtype[(int)iNumSubTypes] + 1);
		iReductionAmount = iOldWeight - iNewWeight;
		m_ProductionSubtypeWeights.SetWeight(0, iNewWeight);
	}

	return eSpecialization;
}

/// Find the specialization type for building wonders
CitySpecializationTypes CvCitySpecializationAI::GetWonderSpecialization() const
{
	CvCitySpecializationXMLEntry* pEntry;

	for(int iI = 0; iI < GC.getNumCitySpecializationInfos(); iI++)
	{
		pEntry = GC.GetGameCitySpecializations()->GetEntry(iI);
		if(pEntry->IsWonder())
		{
			return (CitySpecializationTypes)iI;
		}
	}

	return (CitySpecializationTypes)-1;
}

/// Find the specialization type for building wonders
CitySpecializationTypes CvCitySpecializationAI::GetEconomicDefaultSpecialization() const
{
	CvCitySpecializationXMLEntry* pEntry;

	for(int iI = 0; iI < GC.getNumCitySpecializationInfos(); iI++)
	{
		pEntry = GC.GetGameCitySpecializations()->GetEntry(iI);
		if(pEntry->IsDefault())
		{
			return (CitySpecializationTypes)iI;
		}
	}

	return (CitySpecializationTypes)-1;
}

/// Find the production subtype for wonders
int CvCitySpecializationAI::GetWonderSubtype() const
{
	CvCitySpecializationXMLEntry* pEntry;
	int iSubType = -1;

	for(int iI = 0; iI < GC.getNumCitySpecializationInfos(); iI++)
	{
		pEntry = GC.GetGameCitySpecializations()->GetEntry(iI);
		if(pEntry->GetYieldType() == YIELD_PRODUCTION)
		{
			iSubType++;
		}
		if(pEntry->IsWonder())
		{
			return iSubType;
		}
	}

	return iSubType;
}

/// Choose a city to build the next wonder
CvCity* CvCitySpecializationAI::FindBestWonderCity() const
{
	CvCity* pBestCity = NULL;
	CvCity* pLoopCity;
	int iLoop;

	if (m_eNextWonderDesired != NO_BUILDING)
	{
		for (pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(m_eNextWonderDesired);
			if (!pkBuildingInfo)
				continue;

			if (pLoopCity->IsBestForWonder((BuildingClassTypes)pkBuildingInfo->GetBuildingClassType()))
				return pBestCity;
		}
	}

	return NULL;
}

/// Find the best nearby city site for all yield types
void CvCitySpecializationAI::FindBestSites()
{
	// Clear output
	for(int iI = 0; iI <= YIELD_FAITH; iI++)
		m_iBestValue[iI] = 0;

	//we prefer settling close in the beginning
	int iTimeOffset = (24 * GC.getGame().getElapsedGameTurns()) / max(512, GC.getGame().getMaxTurns());

	//basic search area around existing cities
	int iEvalDistance = GC.getSETTLER_EVALUATION_DISTANCE() + iTimeOffset;

	for(int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(iPlotLoop);
		if (m_pPlayer->canFound( pPlot->getX(),pPlot->getY() ))
		{
			// Check if within range of any of our cities
			CvCity* pNearestCity = m_pPlayer->GetClosestCityByPlots(pPlot);
			if(pNearestCity != NULL)
			{
				if(plotDistance(pPlot->getX(), pPlot->getY(), pNearestCity->getX(), pNearestCity->getY()) <= iEvalDistance)
				{
					int iPlotValue = 0;
					for(int iI = 0; iI <= YIELD_FAITH; iI++)
					{
						if(iI == YIELD_SCIENCE)
						{
							iPlotValue = PlotValueForScience(pPlot);
						}
						else if(iI == YIELD_CULTURE)
						{
							iPlotValue = PlotValueForCulture(pPlot);
						}
						else if(iI == YIELD_FAITH)
						{
							iPlotValue = PlotValueForFaith(pPlot);
						}
						else
						{
							iPlotValue = PlotValueForSpecificYield(pPlot, (YieldTypes)iI);
						}

						if(iPlotValue > m_iBestValue[iI])
						{
							m_iBestValue[iI] = iPlotValue;
						}
					}
				}
			}
		}
	}

	LogBestSites();

	return;
}

/// Evaluate strength of an existing city for providing a specific type of yield (except Science!)
int CvCitySpecializationAI::PlotValueForSpecificYield(CvPlot* pPlot, YieldTypes eYield)
{
	int iTotalPotentialYield = 0;
	int iMultiplier = 0;
	int iPotentialYield = 0;
	int iFirstRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING();
	int iSecondRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING();
	int iThirdRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING();

	// Evaluate potential from plots not currently being worked
	for(int iI = 0; iI < GC.getAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED(); iI++)
	{
		if(iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pPlot->getX(), pPlot->getY(), iI);
			if(pLoopPlot != NULL)
			{
				iPotentialYield = pLoopPlot->getYield(eYield);

				// If owned by someone else, not worth anything
				if(pLoopPlot->isOwned() && pLoopPlot->getOwner() != m_pPlayer->GetID())
				{
					iMultiplier = 0;
				}
				else
				{
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pPlot->getX(), pPlot->getY());
					if(iDistance == 1)
					{
						iMultiplier = iFirstRingMultiplier;
					}
					else if(iDistance == 2)
					{
						iMultiplier = iSecondRingMultiplier;
					}
					else if(iDistance == 3)
					{
						iMultiplier = iThirdRingMultiplier;
					}
				}
				iTotalPotentialYield += iPotentialYield * iMultiplier;
			}
		}
	}

	return iTotalPotentialYield;
}

/// Evaluate strength of a city plot for providing science
int CvCitySpecializationAI::PlotValueForScience(CvPlot* pPlot)
{
	// Roughly half of weight comes from food yield
	// The other half will be are there open tiles we can easily build schools on
	int iTotalFoodYield = 0;
	int iTotalClearTileWeight = 0;
	int iMultiplier = 0;
	int iPotentialYield = 0;
	int iFirstRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING();
	int iSecondRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING();
	int iThirdRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING();
#if defined(MOD_BALANCE_CORE)
	// Evaluate potential from plots not currently being worked
	for(int iI = 0; iI < GC.getAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED(); iI++)
	{
		if(iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pPlot->getX(), pPlot->getY(), iI);
			if(pLoopPlot != NULL)
			{
				// If owned by someone else, not worth anything
				if(pLoopPlot->isOwned() && pLoopPlot->getOwner() != m_pPlayer->GetID())
				{
					iPotentialYield = 0;
					iTotalClearTileWeight = 0;
				}
				else if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				{
#if defined(MOD_BALANCE_CORE)
					if(GC.getSCIENCE_PER_POPULATION() > 0)
					{
						iPotentialYield = pLoopPlot->getYield(YIELD_FOOD) + pLoopPlot->getYield(YIELD_SCIENCE);
					}
					else
					{
						iPotentialYield = pLoopPlot->getYield(YIELD_SCIENCE);
					}
#else
					iPotentialYield = pLoopPlot->getYield(YIELD_FOOD) + pLoopPlot->getYield(YIELD_SCIENCE);
#endif
				}

				else if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
				{
#if defined(MOD_BALANCE_CORE)
					if(GC.getSCIENCE_PER_POPULATION() > 0)
					{
						iTotalClearTileWeight = pLoopPlot->getYield(YIELD_FOOD) + pLoopPlot->getYield(YIELD_SCIENCE);
					}
					else
					{
						iTotalClearTileWeight = pLoopPlot->getYield(YIELD_SCIENCE);
					}
#else
					iTotalClearTileWeight = pLoopPlot->getYield(YIELD_FOOD) + pLoopPlot->getYield(YIELD_SCIENCE);
#endif
					iTotalClearTileWeight *= 2;
				}
				if(iPotentialYield > 0 || iTotalClearTileWeight > 0)
				{
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pPlot->getX(), pPlot->getY());
					if(iDistance == 1)
					{
						iMultiplier = iFirstRingMultiplier;
					}
					else if(iDistance == 2)
					{
						iMultiplier = iSecondRingMultiplier;
					}
					else if(iDistance == 3)
					{
						iMultiplier = iThirdRingMultiplier;
					}

					iTotalFoodYield += iPotentialYield * iMultiplier;
				}
			}
		}
	}
#else
	// Evaluate potential from plots not currently being worked
	for(int iI = 0; iI < GC.getAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED(); iI++)
	{
		bool bIsClear = false;

		if(iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pPlot->getX(), pPlot->getY(), iI);
			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->getResourceType() == NO_RESOURCE)
				{
					if(pLoopPlot->getFeatureType() == NO_FEATURE)
					{
						if(!pLoopPlot->isHills())
						{
							if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
							{
								bIsClear = true;
							}
						}
					}
				}

				iPotentialYield = pLoopPlot->getYield(YIELD_FOOD) + pLoopPlot->getYield(YIELD_SCIENCE);

				// If owned by someone else, not worth anything
				if(pLoopPlot->isOwned() && pLoopPlot->getOwner() != m_pPlayer->GetID())
				{
					iMultiplier = 0;
				}
				else
				{
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pPlot->getX(), pPlot->getY());
					if(iDistance == 1)
					{
						iMultiplier = iFirstRingMultiplier;
					}
					else if(iDistance == 2)
					{
						iMultiplier = iSecondRingMultiplier;
					}
					else if(iDistance == 3)
					{
						iMultiplier = iThirdRingMultiplier;
					}
				}

				iTotalFoodYield += iPotentialYield * iMultiplier;
				if(bIsClear)
				{
					iTotalClearTileWeight += iMultiplier;
				}
			}
		}
	}
#endif
	return iTotalFoodYield + iTotalClearTileWeight;
}
#if defined(MOD_BALANCE_CORE)
/// Evaluate strength of a city plot for providing culture
int CvCitySpecializationAI::PlotValueForCulture(CvPlot* pPlot)
{
	// Roughly half of weight comes from food yield
	// The other half will be are there open tiles we can easily build schools on
	int iTotalFoodYield = 0;
	int iTotalClearTileWeight = 0;
	int iMultiplier = 0;
	int iPotentialYield = 0;
	int iFirstRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING();
	int iSecondRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING();
	int iThirdRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING();
	// Evaluate potential from plots not currently being worked
	for(int iI = 0; iI < GC.getAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED(); iI++)
	{
		if(iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pPlot->getX(), pPlot->getY(), iI);
			if(pLoopPlot != NULL)
			{
				// If owned by someone else, not worth anything
				if(pLoopPlot->isOwned() && pLoopPlot->getOwner() != m_pPlayer->GetID())
				{
					iPotentialYield = 0;
					iTotalClearTileWeight = 0;
				}
				else if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					iPotentialYield = pLoopPlot->getYield(YIELD_CULTURE) + pLoopPlot->getYield(YIELD_FOOD);
				}

				else if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
				{
					iTotalClearTileWeight = pLoopPlot->getYield(YIELD_CULTURE) + pLoopPlot->getYield(YIELD_FOOD);
					iTotalClearTileWeight *= 2;
				}
				if(iPotentialYield > 0 || iTotalClearTileWeight > 0)
				{
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pPlot->getX(), pPlot->getY());
					if(iDistance == 1)
					{
						iMultiplier = iFirstRingMultiplier;
					}
					else if(iDistance == 2)
					{
						iMultiplier = iSecondRingMultiplier;
					}
					else if(iDistance == 3)
					{
						iMultiplier = iThirdRingMultiplier;
					}

					iTotalFoodYield += iPotentialYield * iMultiplier;
				}
			}
		}
	}
	return iTotalFoodYield + iTotalClearTileWeight; 
}
/// Evaluate strength of a city plot for providing faith
int CvCitySpecializationAI::PlotValueForFaith(CvPlot* pPlot)
{
	// Roughly half of weight comes from food yield
	// The other half will be are there open tiles we can easily build schools on
	int iTotalFoodYield = 0;
	int iTotalClearTileWeight = 0;
	int iMultiplier = 0;
	int iPotentialYield = 0;
	int iFirstRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_FIRST_RING();
	int iSecondRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_SECOND_RING();
	int iThirdRingMultiplier = GC.getAI_CITY_SPECIALIZATION_YIELD_WEIGHT_THIRD_RING();
	// Evaluate potential from plots not currently being worked
	for(int iI = 0; iI < GC.getAI_CITY_SPECIALIZATION_YIELD_NUM_TILES_CONSIDERED(); iI++)
	{
		if(iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = iterateRingPlots(pPlot->getX(), pPlot->getY(), iI);
			if(pLoopPlot != NULL)
			{
				// If owned by someone else, not worth anything
				if(pLoopPlot->isOwned() && pLoopPlot->getOwner() != m_pPlayer->GetID())
				{
					iPotentialYield = 0;
					iTotalClearTileWeight = 0;
				}
				else if(pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					iPotentialYield = pLoopPlot->getYield(YIELD_FAITH) + pLoopPlot->getYield(YIELD_FOOD);
				}

				else if(pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
				{
					iTotalClearTileWeight = pLoopPlot->getYield(YIELD_FAITH) + pLoopPlot->getYield(YIELD_FOOD);
					iTotalClearTileWeight *= 2;
				}
				if(iPotentialYield > 0 || iTotalClearTileWeight > 0)
				{
					int iDistance = plotDistance(pLoopPlot->getX(), pLoopPlot->getY(), pPlot->getX(), pPlot->getY());
					if(iDistance == 1)
					{
						iMultiplier = iFirstRingMultiplier;
					}
					else if(iDistance == 2)
					{
						iMultiplier = iSecondRingMultiplier;
					}
					else if(iDistance == 3)
					{
						iMultiplier = iThirdRingMultiplier;
					}

					iTotalFoodYield += iPotentialYield * iMultiplier;
				}
			}
		}
	}
	return iTotalFoodYield + iTotalClearTileWeight;
}
#endif
#if defined(MOD_BALANCE_CORE)
int CvCitySpecializationAI::AdjustValueBasedOnHappiness(CvCity* pCity, YieldTypes eYield, int iInitialValue)
{
	int iRtnValue = 0;
	if(eYield == YIELD_GOLD)
	{
		int iValue = (pCity->getUnhappinessFromGold() * 10);
		if(iValue > 0)
		{
			iRtnValue = ((iInitialValue * (100 + iValue)) / 100);
		}
		iValue = (GET_PLAYER(pCity->getOwner()).GetTreasury()->CalculateBaseNetGoldTimes100());
		iValue /= 100;
		if(iValue < 0)
		{
			iRtnValue = iInitialValue * (100 + (iValue * -1)) / 100;
		}

	}
	else if(eYield == YIELD_FOOD)
	{
		int iValue = (pCity->getUnhappinessFromStarving() * 10);
		if(iValue > 0)
		{
			iRtnValue = iInitialValue * (100 + iValue) / 100;
		}
	}
	else if(eYield == YIELD_CULTURE)
	{
		int iValue = (pCity->getUnhappinessFromCulture() * 10);
		if(iValue > 0)
		{
			iRtnValue = iInitialValue * (100 + iValue) / 100;
		}
	}
	else if(eYield == YIELD_SCIENCE)
	{
		int iValue = (pCity->getUnhappinessFromScience() * 10);
		if(iValue > 0)
		{
			iRtnValue = iInitialValue * (100 + iValue) / 100;
		}
	}
	else if(eYield == YIELD_FAITH)
	{
		int iValue = (pCity->getUnhappinessFromReligion() * 10);
		if(iValue > 0)
		{
			iRtnValue = iInitialValue * (100 + iValue) / 100;
		}
	}
	return iRtnValue;
}
#endif
/// Multiply city value for a yield based on buildings present
int CvCitySpecializationAI::AdjustValueBasedOnBuildings(CvCity* pCity, YieldTypes eYield, int iInitialValue)
{
	int iRtnValue;

	// Everything looks at yield modifier
	iRtnValue = iInitialValue * (100 + pCity->getYieldRateModifier(eYield)) / 100;

	// ... and yield per pop
	int iYieldPerPop = pCity->GetYieldPerPopTimes100(eYield);
	if(iYieldPerPop > 0)
	{
		iRtnValue = iRtnValue * (100 + iYieldPerPop) / 100;
	}

	// ... and yield per pop
	int iYieldPerReligion = pCity->GetYieldPerReligionTimes100(eYield);
	if(iYieldPerPop > 0)
	{
		iRtnValue = iRtnValue * (100 + (iYieldPerReligion * pCity->GetCityReligions()->GetNumReligionsWithFollowers())) / 100;
	}

	// ... and yield changes
	int iYieldChanges = pCity->GetBaseYieldRateFromBuildings(eYield);
	if(iYieldChanges > 0)
	{
		// +20% per point of yield change
		iRtnValue = iRtnValue * (100 + (iYieldChanges * 20)) / 100;
	}
#if defined(MOD_BALANCE_CORE)
	iYieldChanges = pCity->GetYieldPerPopInEmpireTimes100(eYield);
	if (iYieldChanges > 0)
	{
		iRtnValue = iRtnValue * (100 + iYieldChanges * 5) / 100;
	}
	iYieldChanges = pCity->GetBaseYieldRateFromGreatWorks(eYield);
	if(iYieldChanges > 0)
	{
		// +10% per point of yield change
		iRtnValue = iRtnValue * (100 + (iYieldChanges * 10)) / 100;
	}
	iYieldChanges = pCity->GetGrowthExtraYield(eYield);
	if(iYieldChanges > 0)
	{
		// +15% per point of yield change
		iRtnValue = iRtnValue * (100 + (iYieldChanges * 15)) / 100;
	}
	iYieldChanges = pCity->GetYieldFromVictory(eYield);
	if(iYieldChanges > 0)
	{
		// +5% per point of yield change
		iRtnValue = iRtnValue * (100 + (iYieldChanges * 5)) / 100;
	}
	iYieldChanges = pCity->GetYieldFromPillage(eYield);
	if (iYieldChanges > 0)
	{
		// +15% per point of yield change
		iRtnValue = iRtnValue * (100 + (iYieldChanges * 5)) / 100;
	}

	iYieldChanges = pCity->GetBaseYieldRateFromCSAlliance(eYield);
	if(iYieldChanges > 0)
	{
		// +15% per point of yield change
		iRtnValue = iRtnValue * (100 + (iYieldChanges * 15)) / 100;
	}
	iYieldChanges = pCity->GetBaseYieldRateFromCSFriendship(eYield);
	if(iYieldChanges > 0)
	{
		// +15% per point of yield change
		iRtnValue = iRtnValue * (100 + (iYieldChanges * 15)) / 100;
	}
	iYieldChanges = pCity->GetBaseYieldRateFromSpecialists(eYield);
	if(iYieldChanges > 0)
	{
		// +15% per point of yield change
		iRtnValue = iRtnValue * (100 + (iYieldChanges * 15)) / 100;
	}
	iYieldChanges = pCity->GetBaseYieldRateFromMisc(eYield);
	if(iYieldChanges > 0)
	{
		// +10% per point of yield change
		iRtnValue = iRtnValue * (100 + (iYieldChanges * 10)) / 100;
	}
#endif

	// Other modifiers (unique by yield type)
	switch(eYield)
	{
	case YIELD_FOOD:
	{
		int iMaxFoodKeptPercent = pCity->getMaxFoodKeptPercent();
		if(iMaxFoodKeptPercent >= 100)
		{
			iMaxFoodKeptPercent = 99;
		}
		iRtnValue *= 100 / (100 - iMaxFoodKeptPercent);
	}
	break;

	case YIELD_PRODUCTION:
		// Double production if any military training facilities present
#if defined(MOD_BALANCE_CORE)
		if(pCity->getDomainFreeExperience(DOMAIN_LAND) > 0)
		{
			iRtnValue = iRtnValue * (100 + (pCity->getDomainFreeExperience(DOMAIN_LAND) * 10)) / 100;
		}
		if(pCity->getDomainFreeExperience(DOMAIN_AIR) > 0)
		{
			iRtnValue = iRtnValue * (100 + (pCity->getDomainFreeExperience(DOMAIN_AIR) * 10)) / 100;
		}
		if(pCity->getDomainFreeExperience(DOMAIN_SEA) > 0)
		{
			iRtnValue = iRtnValue * (100 + (pCity->getDomainFreeExperience(DOMAIN_SEA) * 10)) / 100;
		}
#else
		if(pCity->getDomainFreeExperience(DOMAIN_LAND) > 0)
		{
			iRtnValue *= 2;
		}
#endif
		break;

	case YIELD_GOLD:
		break;

	case YIELD_SCIENCE:
		break;
#if defined(MOD_BALANCE_CORE)
	case YIELD_CULTURE:
		iYieldChanges = pCity->getCultureRateModifier();
		if(iYieldChanges > 0)
		{
			// +15% per point of yield change
			iRtnValue = iRtnValue * (100 + (iYieldChanges * 15)) / 100;
		}
		break;

	case YIELD_FAITH:
		iYieldChanges = pCity->GetFaithPerTurnFromBuildings();
		if(iYieldChanges > 0)
		{
			// +15% per point of yield change
			iRtnValue = iRtnValue * (100 + (iYieldChanges * 15)) / 100;
		}
		break;
#endif
	}

	return iRtnValue;
}

/// Should this player worry about assigning cities to build spaceship parts?
bool CvCitySpecializationAI::CanBuildSpaceshipParts()
{
	int iLoop;

	// Find a city to test with
	CvCity* pCity = m_pPlayer->firstCity(&iLoop);

	// Loop through adding the available units
	for(int iUnitLoop = 0; iUnitLoop < GC.GetGameUnits()->GetNumUnits(); iUnitLoop++)
	{
		const UnitTypes eUnit = static_cast<UnitTypes>(iUnitLoop);
		CvUnitEntry* pkUnitEntry = GC.getUnitInfo(eUnit);
		if(pkUnitEntry)
		{
			if(pkUnitEntry->GetSpaceshipProject() != NO_PROJECT)
			{
				// See if this unit can be built now
				if(pCity->canTrain((UnitTypes)iUnitLoop))
				{
					return true;
				}
			}
		}
	}
	return false;
}

/// Log which specializations were needed
void CvCitySpecializationAI::LogSpecializationWeights()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strPlayerName;
		CvString strBaseString;
		CvString strYieldString;
		CvString strWeightString;
		CvString strOutBuf;
		FILogFile* pLog;

		// Find the name of this civ
		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strBaseString += strPlayerName + ", Empire-wide specialization weight, ";

		for(int iI = 0; iI < NUM_SPECIALIZATION_YIELDS; iI++)
		{
#if defined(MOD_BALANCE_CORE)
			if(iI > YIELD_FAITH)
#else
			if(iI > YIELD_SCIENCE)
#endif
			{
				strYieldString = "General Economy";
			}
			else
			{
				CvYieldInfo* pYieldInfo = GC.getYieldInfo((YieldTypes)m_YieldWeights.GetElement(iI));
				if(pYieldInfo != NULL)
				{
					strYieldString = pYieldInfo->GetDescription();
				}
			}
			strWeightString.Format("%d", m_YieldWeights.GetWeight(iI));
			strOutBuf = strBaseString + strYieldString + ", " + strWeightString;
			pLog->Msg(strOutBuf);
		}

		const char* aNames[] = { "MILITARY_TRAINING", "EMERGENCY_UNITS", "MILITARY_NAVAL", "WONDER", "SPACESHIP" };
		for(int iI = 0; iI < NUM_PRODUCTION_SPECIALIZATION_SUBTYPES; iI++)
		{
			strYieldString.Format("Production Specialization: %s", aNames[iI]);
			strWeightString.Format("%d", m_ProductionSubtypeWeights.GetWeight(iI));
			strOutBuf = strBaseString + strYieldString + ", " + strWeightString;
			pLog->Msg(strOutBuf);
		}
	}
}

/// Log which specializations were assigned
void CvCitySpecializationAI::LogSpecializationAssignment(CvCity* pCity, CitySpecializationTypes eType, bool bWonderCity)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strCityName;
		CvString strSpecialization;

		strCityName = pCity->getName();
		strSpecialization.Format("New Specialization Type: %d (%s)", eType, eType!=NO_CITY_SPECIALIZATION ? GC.getCitySpecializationInfo(eType)->GetType() : "none");

		strOutBuf = strCityName + ", " + strSpecialization;
		if(bWonderCity)
		{
			strOutBuf += ", WonderCity";
		}

		LogMsg(strOutBuf);
	}
}

/// Report the reason for reevaluating specializations
void CvCitySpecializationAI::LogSpecializationUpdate(CitySpecializationUpdateType eUpdate)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strTypeString;

		switch(eUpdate)
		{
		case SPECIALIZATION_UPDATE_NEW_GRAND_STRATEGY:
		{
			CvAIGrandStrategyXMLEntry* pGrandStrategyInfo = GC.getAIGrandStrategyInfo(m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy());
			if(NULL != pGrandStrategyInfo)
			{
				strTypeString.Format("Update: New grand strategy: %s", pGrandStrategyInfo->GetType());
			}
		}
		break;
		case SPECIALIZATION_UPDATE_NOW_AT_WAR:
			strTypeString = "Update: New war";
			break;
		case SPECIALIZATION_UPDATE_CITY_FOUNDED:
			strTypeString = "Update: City founded";
			break;
		case SPECIALIZATION_UPDATE_MY_CITY_CAPTURED:
			strTypeString = "Update: My city captured";
			break;
		case SPECIALIZATION_UPDATE_ENEMY_CITY_CAPTURED:
			strTypeString = "Update: Enemy city captured";
			break;
		case SPECIALIZATION_UPDATE_STRATEGY_NOW_ON:
			strTypeString = "Update: War strategy turned on";
			break;
		case SPECIALIZATION_UPDATE_STRATEGY_NOW_OFF:
			strTypeString = "Update: War strategy turned off";
			break;
		case SPECIALIZATION_UPDATE_RESEARCH_COMPLETE:
			strTypeString = "Update: Tech research complete";
			break;
		case SPECIALIZATION_UPDATE_WONDER_BUILT_BY_RIVAL:
			strTypeString = "Update: wonder unlocked or built by rival, WONDER";
		}
		LogMsg( strTypeString );
	}
}

/// Record the specialization we want for our next city
void CvCitySpecializationAI::LogNextSpecialization(CitySpecializationTypes eType)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strTypeString;
		strTypeString.Format("Next Specialization: %d (%s)", eType, eType!=NO_CITY_SPECIALIZATION ? GC.getCitySpecializationInfo(eType)->GetType() : "none");
		LogMsg(strTypeString);
	}
}

void CvCitySpecializationAI::LogBestSites()
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strYieldString;
		CvString strWeightString;

		// Loop through each yield type
#if defined(MOD_BALANCE_CORE)
		for(int iI = 0; iI <= YIELD_FAITH; iI++)
#else
		for(int iI = 0; iI <= YIELD_SCIENCE; iI++)
#endif
		{
			CvYieldInfo* pYieldInfo = GC.getYieldInfo((YieldTypes)iI);
			if(pYieldInfo != NULL)
			{
				strYieldString = pYieldInfo->GetDescription();
			}

			strWeightString.Format(", Best site value: %d", m_iBestValue[iI]);
			LogMsg( strYieldString + strWeightString );
		}
	}
}

void CvCitySpecializationAI::LogCity(CvCity* pCity, CitySpecializationData data)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strCityName;
		CvString strYieldString;
		CvString strWeightString;

		strCityName = pCity->getName();

		// Loop through each yield type
#if defined(MOD_BALANCE_CORE)
		for(int iI = 0; iI <= YIELD_FAITH; iI++)
#else
		for(int iI = 0; iI <= YIELD_SCIENCE; iI++)
#endif
		{
			CvYieldInfo* pYieldInfo = GC.getYieldInfo((YieldTypes)iI);
			if(pYieldInfo != NULL)
			{
				strYieldString = pYieldInfo->GetDescription();
			}

			strWeightString.Format(", Value: %d", data.m_iWeight[iI]);
			LogMsg(strCityName + ", " + strYieldString + strWeightString);
		}
	}
}

void CvCitySpecializationAI::LogMsg(const CvString& msg)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strOutBuf;
		CvString strBaseString;
		CvString strPlayerName;
		FILogFile* pLog;

		// Find the name of this civ
		strPlayerName = GetPlayer()->getCivilizationShortDescription();
		pLog = LOGFILEMGR.GetLog(GetLogFileName(strPlayerName), FILogFile::kDontTimeStamp);

		// Get the leading info for this line
		strBaseString.Format("%03d, %s, ", GC.getGame().getElapsedGameTurns(), strPlayerName.c_str());

		strOutBuf = strBaseString + msg;
		pLog->Msg( strOutBuf.c_str() );
	}
}


/// Build log filename
CvString CvCitySpecializationAI::GetLogFileName(CvString& playerName) const
{
	CvString strLogName;

	// Open the log file
	if(GC.getPlayerAndCityAILogSplit())
	{
		strLogName = "CitySpecializationLog_" + playerName + ".csv";
	}
	else
	{
		strLogName = "CitySpecializationLog.csv";
	}

	return strLogName;
}
