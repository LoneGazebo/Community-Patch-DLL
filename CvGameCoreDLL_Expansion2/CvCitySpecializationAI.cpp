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
	for(m_CurrentIndex = m_CurrentIndex+1; m_CurrentIndex < (int)m_paCitySpecializationEntries.size(); m_CurrentIndex++)
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
	m_eNextWonderDesired = NO_BUILDING;
	m_iWonderCityID = -1;
	m_iNextWonderWeight = 0;
	m_iLastTurnEvaluated = 0;
}

template<typename CitySpecializationAI, typename Visitor>
void CvCitySpecializationAI::Serialize(CitySpecializationAI& citySpecializationAI, Visitor& visitor)
{
	visitor(citySpecializationAI.m_bSpecializationsDirty);
	visitor(citySpecializationAI.m_bInterruptWonders);
	visitor(citySpecializationAI.m_bInterruptBuildings);
	visitor(citySpecializationAI.m_bChooseNewWonder);
	visitor(citySpecializationAI.m_eNextWonderDesired);
	visitor(citySpecializationAI.m_iWonderCityID);
	visitor(citySpecializationAI.m_iNextWonderWeight);
	visitor(citySpecializationAI.m_iLastTurnEvaluated);
}

/// Serialization read
void CvCitySpecializationAI::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvCitySpecializationAI::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvCitySpecializationAI& citySpecializationAI)
{
	citySpecializationAI.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvCitySpecializationAI& citySpecializationAI)
{
	citySpecializationAI.Write(stream);
	return stream;
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
	if(GC.getGame().getGameTurn() < /*25*/ GD_INT_GET(AI_CITY_SPECIALIZATION_EARLIEST_TURN))
	{
		return;
	}

	int iCityLoop = 0;
	for (CvCity* pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
	{
		if (pLoopCity->isUnderSiege())
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
	if(m_bSpecializationsDirty || ((m_iLastTurnEvaluated + /*35*/ GD_INT_GET(AI_CITY_SPECIALIZATION_REEVALUATION_INTERVAL)) <= GC.getGame().getGameTurn()))
	{
		AssignSpecializations();

		m_bSpecializationsDirty = false;
		m_iLastTurnEvaluated = GC.getGame().getGameTurn();

		// Do we need to choose production again at all our cities?
		if(m_bInterruptWonders || m_bInterruptBuildings)
		{
			CvCity* pLoopCity = NULL;
			for(pLoopCity = m_pPlayer->firstCity(&iCityLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iCityLoop))
			{
				if(pLoopCity->isProductionBuilding() && pLoopCity->AI_isChooseProductionDirty())
				{
					pLoopCity->AI_chooseProduction(m_bInterruptWonders, m_bInterruptBuildings);
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

/// Evaluate which specializations we need
CvWeightedVector<YieldTypes> CvCitySpecializationAI::WeightSpecializations()
{
	int iFoodYieldWeight = 100;
	int iProductionYieldWeight = 100;
	int iGoldYieldWeight = 100;
	int iScienceYieldWeight = 100;
	int iCultureYieldWeight = 100;
	int iFaithYieldWeight = 100;

	// Must have a capital to do any specialization
	 if (m_pPlayer->getCapitalCity() != NULL)
	 {
		 int iFlavorGold = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
		 if (iFlavorGold < 0) iFlavorGold = 0;

		 int iFlavorScience = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
		 if (iFlavorScience < 0) iFlavorScience = 0;

		 int iFlavorGrowth = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
		 if (iFlavorGrowth < 0) iFlavorGrowth = 0;

		 int iFlavorCulture = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
		 if (iFlavorCulture < 0) iFlavorCulture = 0;

		 int iFlavorFaith = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
		 if (iFlavorFaith < 0) iFlavorFaith = 0;

		 int iFlavorProduction = 10 * m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION"));
		 if (iFlavorProduction < 0) iFlavorProduction = 0;

		 //   Add in any contribution from the current grand strategy
		 CvAIGrandStrategyXMLEntry* grandStrategy = GC.getAIGrandStrategyInfo(m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy());
		 if (grandStrategy)
		 {
			 if (grandStrategy->GetSpecializationBoost(YIELD_FOOD) > 0)
			 {
				 iFoodYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_FOOD);
				 iFoodYieldWeight /= 100;
			 }
			 if (grandStrategy->GetSpecializationBoost(YIELD_GOLD) > 0)
			 {
				 iGoldYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_GOLD);
				 iGoldYieldWeight /= 100;
			 }
			 if (grandStrategy->GetSpecializationBoost(YIELD_SCIENCE) > 0)
			 {
				 iScienceYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_SCIENCE);
				 iScienceYieldWeight /= 100;
			 }
			 if (grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION) > 0)
			 {
				 iProductionYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_PRODUCTION);
				 iProductionYieldWeight /= 100;
			 }
			 if (grandStrategy->GetSpecializationBoost(YIELD_CULTURE) > 0)
			 {
				 iCultureYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_CULTURE);
				 iCultureYieldWeight /= 100;
			 }
			 if (grandStrategy->GetSpecializationBoost(YIELD_FAITH) > 0)
			 {
				 iFaithYieldWeight *= grandStrategy->GetSpecializationBoost(YIELD_FAITH);
				 iFaithYieldWeight /= 100;
			 }
		 }

		 iFoodYieldWeight *= (100 + iFlavorGrowth);
		 iProductionYieldWeight *= (100 + iFlavorProduction);
		 iGoldYieldWeight *= (100 + iFlavorGold);
		 iScienceYieldWeight *= (100 + iFlavorScience);
		 iCultureYieldWeight *= (100 + iFlavorCulture);
		 iFaithYieldWeight *= (100 + iFlavorFaith);
	 }

	// Add weights to our weighted vector
	CvWeightedVector<YieldTypes> yieldWeights;
	yieldWeights.push_back(YIELD_FOOD, iFoodYieldWeight);
	yieldWeights.push_back(YIELD_PRODUCTION, iProductionYieldWeight);
	yieldWeights.push_back(YIELD_GOLD, iGoldYieldWeight);
	yieldWeights.push_back(YIELD_SCIENCE, iScienceYieldWeight);
	yieldWeights.push_back(YIELD_CULTURE, iCultureYieldWeight);
	yieldWeights.push_back(YIELD_FAITH, iFaithYieldWeight);

	return yieldWeights;
}

/// Compute the weight of each production subtype (return value is total of all these weights)
CvWeightedVector<ProductionSpecializationSubtypes> CvCitySpecializationAI::WeightProductionSubtypes()
{
	CvWeightedVector<ProductionSpecializationSubtypes> result;
	bool bCriticalDefenseOn = false;

	int iMilitaryTrainingWeight = 0;
	int iEmergencyUnitWeight = 0;
	int iSeaWeight = 0;
	int iWonderWeight = 0;
	int iSpaceshipWeight = 0;

	int iFlavorWonder = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_WONDER"));
	if (iFlavorWonder < 0) iFlavorWonder = 0;
	int iFlavorSpaceship = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SPACESHIP"));
	if (iFlavorSpaceship < 0) iFlavorSpaceship = 0;
	int iFlavorOffense = m_pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_OFFENSE"));
	if (iFlavorOffense < 0) iFlavorOffense = 0;

	int iUnitsRequested = m_pPlayer->GetNumUnitsNeededToBeBuilt();

	// LONG-TERM MILITARY BUILD-UP
	iMilitaryTrainingWeight += iFlavorOffense * /*10*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_OFFENSE);
	iMilitaryTrainingWeight += m_pPlayer->GetDiplomacyAI()->GetMajorCivApproachBias(CIV_APPROACH_WAR) * /*10*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_TRAINING_PER_PERSONALITY);

	// EMERGENCY UNITS
	iEmergencyUnitWeight += iUnitsRequested * /*20*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_OPERATIONAL_UNITS_REQUESTED);
	iEmergencyUnitWeight += m_pPlayer->GetMilitaryAI()->GetNumberCivsAtWarWith(false) * /*150*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CIVS_AT_WAR_WITH);

	// Is our capital under threat?
	AICityStrategyTypes eCityStrategy = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_CAPITAL_UNDER_THREAT");
	CvCity* pCapital = m_pPlayer->getCapitalCity();
	if(pCapital && eCityStrategy != NO_AICITYSTRATEGY && pCapital->GetCityStrategyAI()->IsUsingCityStrategy(eCityStrategy))
	{
		iEmergencyUnitWeight += /*50*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_CAPITAL_THREAT);
	}

	// Add in weights depending on what the military AI is up to
	MilitaryAIStrategyTypes eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_WAR_MOBILIZATION");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iMilitaryTrainingWeight += /*250*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_WAR_MOBILIZATION);
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iEmergencyUnitWeight += /*250*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE);
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_EMPIRE_DEFENSE_CRITICAL");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		bCriticalDefenseOn = true;
		iEmergencyUnitWeight += /*1250*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_EMPIRE_DEFENSE_CRITICAL);
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
		iSeaWeight += /*50*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS);
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_NEED_NAVAL_UNITS_CRITICAL");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iSeaWeight += /*250*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEED_NAVAL_UNITS_CRITICAL);
	}
	eStrategy = (MilitaryAIStrategyTypes) GC.getInfoTypeForString("MILITARYAISTRATEGY_ENOUGH_NAVAL_UNITS");
	if(eStrategy != NO_MILITARYAISTRATEGY && m_pPlayer->GetMilitaryAI()->IsUsingStrategy(eStrategy))
	{
		iSeaWeight = 0;
	}

	// Wonder is MIN between weight of wonders available to build and value from flavors (but not less than zero)
	int iWonderFlavorWeight = iFlavorWonder * /*250*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_WONDER);
	int iWeightOfWonders = (int)(m_iNextWonderWeight * /*0.2f*/ GD_FLOAT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_NEXT_WONDER));
	iWonderWeight = min(iWonderFlavorWeight, iWeightOfWonders);
	iWonderWeight = max(iWonderWeight, 0);

	// One-half of normal weight if critical defense is on
	if (bCriticalDefenseOn)
	{
		iWonderWeight /= 2;
	}

	if (CanBuildSpaceshipParts())
	{
		iSpaceshipWeight += iFlavorSpaceship * /*10*/ GD_INT_GET(AI_CITY_SPECIALIZATION_PRODUCTION_WEIGHT_FLAVOR_SPACESHIP);
	}

	CvAIGrandStrategyXMLEntry* grandStrategy = GC.getAIGrandStrategyInfo(m_pPlayer->GetGrandStrategyAI()->GetActiveGrandStrategy());
	if(grandStrategy)
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

	// Add weights to our weighted vector
	result.push_back(PRODUCTION_SPECIALIZATION_MILITARY_TRAINING, iMilitaryTrainingWeight);
	result.push_back(PRODUCTION_SPECIALIZATION_EMERGENCY_UNITS, iEmergencyUnitWeight);
	result.push_back(PRODUCTION_SPECIALIZATION_MILITARY_NAVAL, iSeaWeight);
	result.push_back(PRODUCTION_SPECIALIZATION_WONDER, iWonderWeight);
	result.push_back(PRODUCTION_SPECIALIZATION_SPACESHIP, iSpaceshipWeight);

	return result;
}

/// Assign specializations to cities
void CvCitySpecializationAI::AssignSpecializations()
{
	map<int, vector<int>> citiesWithoutSpecialization;
	CitySpecializationTypes eWonderSpecialization = GetWonderSpecialization();
	vector<CitySpecializationTypes> specializationsNeeded =	SelectSpecializations();
	vector<CitySpecializationTypes>::iterator it;

	// OBVIOUS ASSIGNMENTS: Loop through our cities making obvious assignments
	int iLoop;
	for(CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		if(pLoopCity->IsPuppet() && pLoopCity->getPopulation()<5)
		{
			pLoopCity->GetCityStrategyAI()->SetSpecialization(GetEconomicDefaultSpecialization());
			LogSpecializationAssignment(pLoopCity, GetEconomicDefaultSpecialization(), true);
			continue;
		}

		// If this city is building a wonder, set the specialization accordingly
		if(pLoopCity->IsBuildingWorldWonder())
		{
			pLoopCity->GetCityStrategyAI()->SetSpecialization(eWonderSpecialization);
			LogSpecializationAssignment(pLoopCity, eWonderSpecialization, true);
			continue;
		}

		// If city default is equal to a needed type, go with that
		CitySpecializationTypes eSpecialization = pLoopCity->GetCityStrategyAI()->GetDefaultSpecialization();
		it = find(specializationsNeeded.begin(), specializationsNeeded.end(), eSpecialization);
		if (it != specializationsNeeded.end())
		{
			specializationsNeeded.erase(it);
			pLoopCity->GetCityStrategyAI()->SetSpecialization(eSpecialization);
			LogSpecializationAssignment(pLoopCity, eSpecialization);
			continue;
		}

		// Store all other cities for later
		vector<int> cityData = CityValueForUnworkedTileYields(pLoopCity);
		for(int iI = 0; iI < YIELD_TOURISM; iI++)
		{
			cityData[iI] = AdjustValueBasedOnBuildings(pLoopCity, (YieldTypes)iI, cityData[iI]);
			if(MOD_BALANCE_CORE_HAPPINESS)
				cityData[iI] = AdjustValueBasedOnHappiness(pLoopCity, (YieldTypes)iI, cityData[iI]);

			if(cityData[iI] < 0)
				cityData[iI] = 0;
		}

		citiesWithoutSpecialization[pLoopCity->GetID()] = cityData;
		LogCity(pLoopCity, cityData);
	}

	// REMAINING ASSIGNMENTS: Make the rest of the assignments
	for (it = specializationsNeeded.begin(); it != specializationsNeeded.end(); ++it)
	{
		CitySpecializationTypes eType = *it;

		CvCitySpecializationXMLEntry* pkCitySpecializationEntry = GC.getCitySpecializationInfo(eType);
		if (pkCitySpecializationEntry == NULL)
			continue;

		YieldTypes eYield = pkCitySpecializationEntry->GetYieldType();
		bool bCoastal = pkCitySpecializationEntry->IsMustBeCoastal();

		// Pick best existing city based on a better computation of existing city's value for a yield type
		int iBestValue = 1;
		int iBestCityID = -1;
		for (map<int, vector<int>>::iterator itCity = citiesWithoutSpecialization.begin(); itCity != citiesWithoutSpecialization.end(); ++itCity)
		{
			CvCity* pCity = m_pPlayer->getCity(itCity->first);
			if (bCoastal && !pCity->isCoastal())
				continue;

			//food is the one yield than can turn bad - don't grow unhappy cities
			if (pCity->getHappinessDelta()<0 && eYield == YIELD_FOOD)
				continue;

			int iCityValue = 0;
			if (eYield == NO_YIELD)
			{
				for (int iI = 0; iI < YIELD_TOURISM; iI++)
					iCityValue += itCity->second[iI];
			}
			else
				iCityValue = itCity->second[eYield];

			if (iCityValue > iBestValue)
			{
				iBestValue = iCityValue;
				iBestCityID = itCity->first;
			}
		}

		// Found a city to set
		if (iBestCityID != -1)
		{
			CvCity* pCity = m_pPlayer->getCity(iBestCityID);

			pCity->GetCityStrategyAI()->SetSpecialization(eType);
			LogSpecializationAssignment(pCity, eType);

			citiesWithoutSpecialization.erase(iBestCityID);
		}
	}

	//set all remaining cities to default
	for(map<int,vector<int>>::iterator itCity = citiesWithoutSpecialization.begin(); itCity != citiesWithoutSpecialization.end(); ++itCity)
	{
		CvCity* pCity = m_pPlayer->getCity(itCity->first);
		pCity->GetCityStrategyAI()->SetSpecialization(GetEconomicDefaultSpecialization());
		LogSpecializationAssignment(pCity, GetEconomicDefaultSpecialization());
	}

	return;
}

/// Evaluate strength of an existing city for providing a specific type of yield
vector<int> CvCitySpecializationAI::CityValueForUnworkedTileYields(CvCity* pCity)
{
	vector<int> result(YIELD_TOURISM, 0);
	CvPlot* pPlot = pCity->plot();

	// Evaluate potential from plots not currently being worked
	for(int iI = 1; iI < pCity->GetNumWorkablePlots(); iI++)
	{
		CvPlot* pLoopPlot = iterateRingPlots(pPlot->getX(), pPlot->getY(), iI);
		if(pLoopPlot != NULL && pCity->GetCityCitizens()->IsCanWork(pLoopPlot) && !pCity->GetCityCitizens()->IsWorkingPlot(pLoopPlot))
		{
			for (int iYield = 0; iYield < YIELD_TOURISM; iYield++)
			{
				int iPotentialYield = pLoopPlot->getYield((YieldTypes)iYield);
				if (iYield == YIELD_FOOD) //a plot needs to be worked by a citizen who needs food
					iPotentialYield = max(0, iPotentialYield - /*2*/ GD_INT_GET(FOOD_CONSUMPTION_PER_POPULATION));

				result[iYield] += iPotentialYield*100; //to make sure modifiers are not rounded away later on
			}
		}
	}

	return result;
}


/// Find specializations needed (including for the next city we build)
vector<CitySpecializationTypes> CvCitySpecializationAI::SelectSpecializations()
{
	vector<CitySpecializationTypes> specializationsNeeded;
	map<YieldTypes,int> numSpecializationsPerYield;
	map<ProductionSpecializationSubtypes, int> numSpecializationsPerSubtype;

	CvWeightedVector<ProductionSpecializationSubtypes> prodSubtypeWeights = WeightProductionSubtypes();
	CvWeightedVector<YieldTypes> yieldWeights = WeightSpecializations();
	LogSpecializationWeights(prodSubtypeWeights,yieldWeights);

	// LOOP we want to specialize half of our cities. the other half will use the default.
	size_t iSpecializationsToAssign = size_t(m_pPlayer->getNumCities() - m_pPlayer->GetNumPuppetCities()) / 2;
	while(specializationsNeeded.size() < iSpecializationsToAssign)
	{
		// Find highest weighted specialization
		yieldWeights.SortItems();

		// Mark that we need one city of this type
		YieldTypes eYield = yieldWeights.GetElement(0);

		CitySpecializationTypes eSpecialization = NO_CITY_SPECIALIZATION;
		if(GC.GetGameCitySpecializations()->GetNumSpecializationsForYield(eYield) > 1)
		{
			if(eYield == YIELD_PRODUCTION)
			{
				eSpecialization = SelectProductionSpecialization(prodSubtypeWeights,numSpecializationsPerSubtype);
			}
			else
			{
				FAssertMsg(false, "Code does not support > 1 specialization for yields other than production.");
			}
		}
		else
		{
			eSpecialization = GC.GetGameCitySpecializations()->GetFirstSpecializationForYield(eYield);
		}

		numSpecializationsPerYield[eYield]++;

		// Reduce weight for this specialization based on dividing original weight by <num of this type + 1>
		int iOldWeight = yieldWeights.GetWeight(0);
		int iNewWeight = iOldWeight * numSpecializationsPerYield[eYield] / (numSpecializationsPerYield[eYield] + 1);

		yieldWeights.SetWeight(0, iNewWeight);

		specializationsNeeded.push_back(eSpecialization);
	}

	return specializationsNeeded;
}

/// Find production specializations needed
CitySpecializationTypes CvCitySpecializationAI::SelectProductionSpecialization(
	CvWeightedVector<ProductionSpecializationSubtypes>& prodSubtypeWeights, 
	map<ProductionSpecializationSubtypes,int>& numSpecializationsPerSubtype)
{
	// Find current highest weighted subtype
	prodSubtypeWeights.SortItems();
	ProductionSpecializationSubtypes eSubtype = prodSubtypeWeights.GetElement(0);

	// find the matching specialization in the stupidest possible way
	CitySpecializationTypes eSpecialization = GC.GetGameCitySpecializations()->GetFirstSpecializationForYield(YIELD_PRODUCTION);
	for (int i=0; i<10; i++) //failsafe ...
	{
		if ((ProductionSpecializationSubtypes)i == eSubtype)
		{
			eSpecialization = GC.GetGameCitySpecializations()->GetNextSpecializationForYield();
			break;
		}
	}

	// If this is the wonder type, make sure we have a city to build it
	if(eSubtype == GetWonderSubtype())
	{
		CvCity* pCity = FindBestWonderCity();
		if(pCity != NULL)
		{
			m_iWonderCityID = pCity->GetID();
		}
		// No wonder city, substitute default specialization instead
		else
		{
			eSpecialization = GetEconomicDefaultSpecialization();
		}

		prodSubtypeWeights.SetWeight(0, 0);
	}
	else
	{
		numSpecializationsPerSubtype[eSubtype]++;

		// Reduce weight for this subtype based on dividing original weight by <num of this type + 1>
		int iOldWeight = prodSubtypeWeights.GetWeight(0);
		int iNewWeight = iOldWeight * numSpecializationsPerSubtype[eSubtype] / (numSpecializationsPerSubtype[eSubtype] + 1);

		prodSubtypeWeights.SetWeight(0, iNewWeight);
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

	return NO_CITY_SPECIALIZATION;
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

	return NO_CITY_SPECIALIZATION;
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

			if (pLoopCity->IsBestForWonder(pkBuildingInfo->GetBuildingClassType()))
				return pBestCity;
		}
	}

	return NULL;
}

#if defined(MOD_BALANCE_CORE)
int CvCitySpecializationAI::AdjustValueBasedOnHappiness(CvCity* pCity, YieldTypes eYield, int iInitialValue)
{
	int iRtnValue = iInitialValue;

	if (eYield == YIELD_GOLD)
	{
		int iMod = pCity->GetPoverty(false) * 20;
		int iGPT = (GET_PLAYER(pCity->getOwner()).GetTreasury()->CalculateBaseNetGoldTimes100());
		if (iGPT < 0)
			iMod -= iGPT;

		iRtnValue = iInitialValue * (100 + iMod) / 100;
	}
	else if (eYield == YIELD_FOOD)
	{
		int iBonus = pCity->GetUnhappinessFromFamine() * 20;
		int iMalus = pCity->getGrowthMods(); //if we're unhappy don't grow further

		iRtnValue = iInitialValue * (100 + iBonus + iMalus) / 100;
	}
	else if (eYield == YIELD_CULTURE)
	{
		int iValue = pCity->GetBoredom(false) * 20;
		if (iValue > 0)
		{
			iRtnValue = iInitialValue * (100 + iValue) / 100;
		}
	}
	else if (eYield == YIELD_SCIENCE)
	{
		int iValue = pCity->GetIlliteracy(false) * 20;
		if (iValue > 0)
		{
			iRtnValue = iInitialValue * (100 + iValue) / 100;
		}
	}
	else if (eYield == YIELD_FAITH)
	{
		//cannot really heal religious unhappiness by faith
		//but maybe we can buy an inquisitor eventually
		int iValue = pCity->GetUnhappinessFromReligiousUnrest() * 10;
		if (iValue > 0)
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
	// Everything looks at yield modifier
	int iRtnValue = iInitialValue * (100 + pCity->getYieldRateModifier(eYield)) / 100;

	// ... and yield per pop
	int iYieldPerPop = pCity->GetYieldPerPopTimes100(eYield);
	if(iYieldPerPop > 0)
	{
		iRtnValue = iRtnValue * (100 + iYieldPerPop) / 100;
	}

	// ... and yield per pop
	int iYieldPerReligion = pCity->GetYieldPerReligionTimes100(eYield);
	if(iYieldPerReligion > 0)
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
	case NO_YIELD:
	case YIELD_GOLD:
	case YIELD_TOURISM:
	case YIELD_GOLDEN_AGE_POINTS:
	case YIELD_GREAT_GENERAL_POINTS:
	case YIELD_GREAT_ADMIRAL_POINTS:
	case YIELD_POPULATION:
	case YIELD_CULTURE_LOCAL:
	case YIELD_JFD_HEALTH:
	case YIELD_JFD_DISEASE:
	case YIELD_JFD_CRIME:
	case YIELD_JFD_LOYALTY:
	case YIELD_JFD_SOVEREIGNTY:
		break; // Yield unmodified.
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
void CvCitySpecializationAI::LogSpecializationWeights(CvWeightedVector<ProductionSpecializationSubtypes> prodSubtypeWeights, CvWeightedVector<YieldTypes> yieldWeights)
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

		for(int iI = 0; iI < YIELD_TOURISM; iI++)
		{
			CvYieldInfo* pYieldInfo = GC.getYieldInfo(yieldWeights.GetElement(iI));
			if(pYieldInfo != NULL)
			{
				strYieldString = pYieldInfo->GetDescription();
			}

			strWeightString.Format("%d", yieldWeights.GetWeight(iI));
			strOutBuf = strBaseString + strYieldString + ", " + strWeightString;
			pLog->Msg(strOutBuf);
		}

		const char* aNames[] = { "MILITARY_TRAINING", "EMERGENCY_UNITS", "MILITARY_NAVAL", "WONDER", "SPACESHIP" };
		for(int iI = 0; iI < NUM_PRODUCTION_SPECIALIZATION_SUBTYPES; iI++)
		{
			strYieldString.Format("Production Specialization: %s", aNames[iI]);
			strWeightString.Format("%d", prodSubtypeWeights.GetWeight(iI));
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
			strTypeString = "Update: Wonder unlocked or built by rival, WONDER";
			break;
		case SPECIALIZATION_UPDATE_WONDER_BUILT_BY_US:
			strTypeString = "Update: Wonder unlocked or built by us, WONDER";
			break;
		case SPECIALIZATION_UPDATE_CITIES_UNDER_SIEGE:
			strTypeString = "Update: Cities under siege";
			break;
		}
		LogMsg( strTypeString );
	}
}

void CvCitySpecializationAI::LogCity(CvCity* pCity, const vector<int>& data)
{
	if(GC.getLogging() && GC.getAILogging())
	{
		CvString strYieldString;
		CvString strWeightString;
		CvString strCityName = pCity->getName();

		// Loop through each yield type
		for(int iI = 0; iI <= YIELD_FAITH; iI++)
		{
			CvYieldInfo* pYieldInfo = GC.getYieldInfo((YieldTypes)iI);
			if(pYieldInfo != NULL)
			{
				strYieldString = pYieldInfo->GetDescription();
			}

			strWeightString.Format(", Value: %d", data[iI]);
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
