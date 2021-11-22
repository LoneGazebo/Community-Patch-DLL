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
#include "CvCitySpecializationAI.h"
#include "CvEconomicAI.h"
#include "CvGrandStrategyAI.h"
#include "CvDllInterfaces.h"
#include "CvInfosSerializationHelper.h"
#include "cvStopWatch.h"
#include "cvMilitaryAI.h"
#include "CvTypes.h"
#include "CvEnumMapSerialization.h"

// must be included after all other headers
#include "LintFree.h"

// global state ... it's a hack
SPrecomputedExpensiveNumbers gCachedNumbers;

//=====================================
// CvCityCitizens
//=====================================

/// Constructor
CvCityCitizens::CvCityCitizens()
{
}

/// Destructor
CvCityCitizens::~CvCityCitizens()
{
	Uninit();
}

/// Initialize
void CvCityCitizens::Init(CvCity* pCity)
{
	m_pCity = pCity;

	m_aiSpecialistCounts.init();
	m_aiSpecialistSlots.init();
	m_aiSpecialistGreatPersonProgressTimes100.init();
	m_aiNumSpecialistsInBuilding.init();
	m_aiNumForcedSpecialistsInBuilding.init();
	m_piBuildingGreatPeopleRateChanges.init();

	// Clear variables
	Reset();
}

/// Deallocate memory created in initialize
void CvCityCitizens::Uninit()
{
	m_aiSpecialistCounts.uninit();
	m_aiSpecialistSlots.uninit();
	m_aiSpecialistGreatPersonProgressTimes100.uninit();
	m_aiNumSpecialistsInBuilding.uninit();
	m_aiNumForcedSpecialistsInBuilding.uninit();
	m_piBuildingGreatPeopleRateChanges.uninit();
}

/// Reset member variables
void CvCityCitizens::Reset()
{
	m_bAutomated = false;
	m_bNoAutoAssignSpecialists = false;
	m_bIsDirty = false;

	m_iNumUnassignedCitizens = 0;
	m_iNumCitizensWorkingPlots = 0;
	m_iNumForcedWorkingPlots = 0;

	m_eCityAIFocusTypes = NO_CITY_AI_FOCUS_TYPE;

	int iI;

	m_vWorkedPlots.clear();
	CvAssertMsg((0 < MAX_CITY_PLOTS), "MAX_CITY_PLOTS is not greater than zero but an array is being allocated in CvCityCitizens::reset");
	for (iI = 0; iI < MAX_CITY_PLOTS; iI++)
	{
		m_pabWorkingPlot[iI] = false;
		m_pabForcedWorkingPlot[iI] = false;
	}

	m_vBlockadedPlots.clear();

	m_iNumDefaultSpecialists = 0;
	m_iNumForcedDefaultSpecialists = 0;

	m_aiSpecialistCounts.assign(0);
	m_aiSpecialistSlots.assign(0);
	m_aiSpecialistGreatPersonProgressTimes100.assign(0);
	m_aiNumSpecialistsInBuilding.assign(0);
	m_aiNumForcedSpecialistsInBuilding.assign(0);
	m_piBuildingGreatPeopleRateChanges.assign(0);

	m_bForceAvoidGrowth = false;
}

template<typename CityCitizens, typename Visitor>
void CvCityCitizens::Serialize(CityCitizens& cityCitizens, Visitor& visitor)
{
	visitor(cityCitizens.m_bAutomated);
	visitor(cityCitizens.m_bNoAutoAssignSpecialists);
	visitor(cityCitizens.m_bIsDirty);
	visitor(cityCitizens.m_iNumUnassignedCitizens);
	visitor(cityCitizens.m_iNumCitizensWorkingPlots);
	visitor(cityCitizens.m_iNumForcedWorkingPlots);

	visitor(cityCitizens.m_eCityAIFocusTypes);

	visitor(cityCitizens.m_bForceAvoidGrowth);

	visitor(cityCitizens.m_pabWorkingPlot);
	visitor(cityCitizens.m_pabForcedWorkingPlot);
	visitor(cityCitizens.m_vBlockadedPlots);

	visitor(cityCitizens.m_iNumDefaultSpecialists);
	visitor(cityCitizens.m_iNumForcedDefaultSpecialists);

	visitor(cityCitizens.m_aiSpecialistCounts);
	visitor(cityCitizens.m_aiSpecialistSlots);
	visitor(cityCitizens.m_aiSpecialistGreatPersonProgressTimes100);
	visitor(cityCitizens.m_aiNumSpecialistsInBuilding);
	visitor(cityCitizens.m_aiNumForcedSpecialistsInBuilding);
	visitor(cityCitizens.m_piBuildingGreatPeopleRateChanges);
}

/// Serialization read
void CvCityCitizens::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);

	m_vWorkedPlots.clear();
	for (int i = 0; i<MAX_CITY_PLOTS; i++)
	{
		if (m_pabWorkingPlot[i])
		{
			CvPlot* pPlot = GetCityPlotFromIndex(i);
			if (pPlot)
			{
				int iPlotNum = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
				m_vWorkedPlots.push_back(iPlotNum);
			}
		}
	}
}

/// Serialization write
void CvCityCitizens::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvCityCitizens& cityCitizens)
{
	cityCitizens.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvCityCitizens& cityCitizens)
{
	cityCitizens.Write(stream);
	return stream;
}

/// Returns the City object this set of Citizens is associated with
CvCity* CvCityCitizens::GetCity()
{
	return m_pCity;
}

/// Returns the Player object this City belongs to
CvPlayer* CvCityCitizens::GetPlayer()
{
	return &GET_PLAYER(GetOwner());
}

/// Helper function to return Player owner of our City
PlayerTypes CvCityCitizens::GetOwner() const
{
	return m_pCity->getOwner();
}

/// Helper function to return Team owner of our City
TeamTypes CvCityCitizens::GetTeam() const
{
	return m_pCity->getTeam();
}

/// What happens when a City is first founded?
void CvCityCitizens::DoFoundCity()
{
	// always work the home plot (center)
	CvPlot* pHomePlot = GetCityPlotFromIndex(CITY_HOME_PLOT);
	if (pHomePlot != NULL)
	{
		bool bWorkPlot = IsCanWork(pHomePlot);
		SetWorkingPlot(pHomePlot, bWorkPlot, CvCity::YIELD_UPDATE_GLOBAL);
	}
}

/// Processed every turn
void CvCityCitizens::DoTurn()
{
	CvPlayerAI& thisPlayer = GET_PLAYER(GetOwner());

	bool bForceCheck = false;
	if (CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity))
	{
		bForceCheck |= SetFocusType(NO_CITY_AI_FOCUS_TYPE);

		if (IsForcedAvoidGrowth())
			bForceCheck |= SetForcedAvoidGrowth(false);
	}

	if (!thisPlayer.isHuman())
	{
		// Are we running at a deficit?
		EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY", true);
		bool bInDeficit = thisPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);

		EconomicAIStrategyTypes eStrategyBuildingReligion = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_DEVELOPING_RELIGION", true);
		bool bBuildingReligion = thisPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyBuildingReligion);
		bool bNeedFood = m_pCity->GetCityStrategyAI()->GetMostDeficientYield() == YIELD_FOOD;

		//---------------
		// note that GetPlotValue already considers wonder/settler building so we don't do that here
		//---------------

		if (bInDeficit && !bNeedFood)
		{
			bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_GOLD);
		}
		else if (bInDeficit && bNeedFood)
		{
			bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_GOLD_GROWTH);
		}
		else if (m_pCity->getProductionProcess() != NO_PROCESS || m_pCity->getProductionProject() != NO_PROJECT)
		{
			bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_PRODUCTION);
		}
		else // no special cases? Alright, let's pick a function to follow...
		{
			AICityStrategyTypes eGoodGP = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_GOOD_GP_CITY");
			bool bGPCity = m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eGoodGP) || thisPlayer.GetDiplomacyAI()->IsGoingForCultureVictory();

			bool bCultureBlock = false;
			for (int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
			{
				PlayerTypes eLoopPlayer = (PlayerTypes)iPlayerLoop;

				if (eLoopPlayer != NO_PLAYER && eLoopPlayer != m_pCity->getOwner() && thisPlayer.GetDiplomacyAI()->IsPlayerValid(eLoopPlayer) && !GET_PLAYER(eLoopPlayer).isMinorCiv())
				{
					if (GET_PLAYER(eLoopPlayer).GetDiplomacyAI()->IsCloseToCultureVictory())
					{
						bCultureBlock = true;
						break;
					}
				}
			}

			if (bCultureBlock)
			{
				bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_CULTURE);
			}
			else
			{
				CitySpecializationTypes eSpecialization = m_pCity->GetCityStrategyAI()->GetSpecialization();
				if (eSpecialization != NO_CITY_SPECIALIZATION)
				{
					if (m_pCity->GetCityStrategyAI()->GetDefaultSpecialization() == eSpecialization)
					{
						bForceCheck |= SetFocusType(NO_CITY_AI_FOCUS_TYPE);
					}
					else
					{
						CvCitySpecializationXMLEntry* pCitySpecializationEntry = GC.getCitySpecializationInfo(eSpecialization);
						if (pCitySpecializationEntry)
						{
							YieldTypes eYield = pCitySpecializationEntry->GetYieldType();
							if (eYield == YIELD_FOOD && !IsAvoidGrowth()) //should really make sure the specialization is sane ...
							{
								bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_FOOD);
							}
							else if (eYield == YIELD_PRODUCTION && bNeedFood)
							{
								bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_PROD_GROWTH);
							}
							else if (eYield == YIELD_PRODUCTION && !bNeedFood)
							{
								bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_PRODUCTION);
							}
							else if (eYield == YIELD_GOLD && bNeedFood)
							{
								bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_GOLD_GROWTH);
							}
							else if (eYield == YIELD_GOLD && !bNeedFood)
							{
								bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_GOLD);
							}
							else if (eYield == YIELD_SCIENCE)
							{
								bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_SCIENCE);
							}
							else if (eYield == YIELD_FAITH)
							{
								if (bBuildingReligion)
								{
									bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_FAITH);
								}
								else
								{
									bForceCheck |= SetFocusType(NO_CITY_AI_FOCUS_TYPE);
								}
							}
							else if (eYield == YIELD_CULTURE && (!bGPCity || bCultureBlock))
							{
								bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_CULTURE);
							}
							else if (eYield == YIELD_CULTURE && bGPCity)
							{
								bForceCheck |= SetFocusType(CITY_AI_FOCUS_TYPE_GREAT_PEOPLE);
							}
							else
							{
								bForceCheck |= SetFocusType(NO_CITY_AI_FOCUS_TYPE);
							}
						}
						else
						{
							bForceCheck |= SetFocusType(NO_CITY_AI_FOCUS_TYPE);
						}
					}
				}
				else
				{
					bForceCheck |= SetFocusType(NO_CITY_AI_FOCUS_TYPE);
				}
			}

			EconomicAIStrategyTypes eEarlyExpand = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_EARLY_EXPANSION");
			bool bWantSettlers = thisPlayer.GetEconomicAI()->IsUsingStrategy(eEarlyExpand);

			int iPotentialUnhappiness = m_pCity->getPotentialUnhappinessWithGrowthVal() - m_pCity->getPotentialHappinessWithGrowthVal();
			if (iPotentialUnhappiness > 0 && thisPlayer.IsEmpireUnhappy())
			{
				//default value for vanilla happiness
				int iLockThreshold = -20;
				if (MOD_BALANCE_CORE_HAPPINESS)
				{
					if (bWantSettlers)
						//if we fall below this threshold the early expansion strategy will be disabled and we leave good city sites to our enemies
						iLockThreshold = /*50*/ GD_INT_GET(UNHAPPY_THRESHOLD);
					else
						//later on tolerate some more unhappiness
						iLockThreshold = /*35*/ GD_INT_GET(VERY_UNHAPPY_THRESHOLD);
				}

				//lock the city if it's net negative and would take us over the threshold
				int iExcessHappiness = thisPlayer.GetExcessHappiness();
				if (iExcessHappiness - iPotentialUnhappiness <= iLockThreshold && m_pCity->getHappinessDelta() < 1)
				{
					bForceCheck |= SetForcedAvoidGrowth(true);
				}
				//unlock only one city per turn, recheck next turn
				else if (IsForcedAvoidGrowth() && thisPlayer.unlockedGrowthAnywhereThisTurn())
				{
					thisPlayer.setUnlockedGrowthAnywhereThisTurn(true);
					bForceCheck |= SetForcedAvoidGrowth(false);
				}
			}
			else
			{
				bForceCheck |= SetForcedAvoidGrowth(false);
			}
		}

#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
		CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(true), "Gameplay: More workers than population in the city.");
#else
		CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(), "Gameplay: More workers than population in the city.");
#endif
		DoReallocateCitizens(bForceCheck);
	}

#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(true), "Gameplay: More workers than population in the city.");
#else
	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(), "Gameplay: More workers than population in the city.");
#endif
	DoSpecialists();

#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(true), "Gameplay: More workers than population in the city.");
#else
	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(), "Gameplay: More workers than population in the city.");
#endif
}

int CvCityCitizens::GetBonusPlotValue(CvPlot* pPlot, YieldTypes eYield, SPrecomputedExpensiveNumbers& cache)
{
	int iBonus = 0;

	FeatureTypes eFeature = pPlot->getFeatureType();
	TerrainTypes eTerrain = pPlot->getTerrainType();

	if (eFeature != NO_FEATURE)
	{
		if (eYield < (int)cache.bonusForXFeature.size() && eFeature < (int)cache.bonusForXFeature[eYield].size())
		{
			int iEffect = cache.bonusForXFeature[eYield][eFeature];

			//update cache on demand only
			if (iEffect == INT_MAX)
			{
				const CvReligion* pReligion = m_pCity->GetCityReligions()->GetMajorityReligion();
				if (pReligion)
					iEffect = pReligion->m_Beliefs.GetYieldPerXFeatureTimes100(eFeature, eYield, m_pCity->getOwner(), m_pCity);
				else
					iEffect = 0;

				cache.bonusForXFeature[eYield][eFeature] = iEffect;
			}

			int iTempBonus = (iEffect * (m_pCity->GetNumFeatureWorked(eFeature))) / 100;
			int iTempBonusPlusOne = (iEffect * (m_pCity->GetNumFeatureWorked(eFeature) + 1)) / 100;
			if (iTempBonus != iTempBonusPlusOne)
				iBonus += iTempBonusPlusOne;
		}

		int iTempBonus = (m_pCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeature, eYield) * (m_pCity->GetNumFeatureWorked(eFeature))) / 100;
		int iTempBonusPlusOne = (m_pCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeature, eYield) * (m_pCity->GetNumFeatureWorked(eFeature) + 1)) / 100;
		if (iTempBonus != iTempBonusPlusOne)
			iBonus += iTempBonusPlusOne;
	}
	if (eTerrain != NO_TERRAIN)
	{
		if (eYield < (int)cache.bonusForXTerrain.size() && eTerrain < (int)cache.bonusForXTerrain[eYield].size())
		{
			int iEffect = cache.bonusForXTerrain[eYield][eTerrain];

			//update cache on demand only
			if (iEffect == INT_MAX)
			{
				const CvReligion* pReligion = m_pCity->GetCityReligions()->GetMajorityReligion();
				if (pReligion)
					iEffect = pReligion->m_Beliefs.GetYieldPerXTerrainTimes100(eTerrain, eYield, m_pCity->getOwner(), m_pCity);
				else
					iEffect = 0;

				cache.bonusForXTerrain[eYield][eTerrain] = iEffect;
			}

			int iTempBonus = (iEffect * (m_pCity->GetNumTerrainWorked(eTerrain))) / 100;
			int iTempBonusPlusOne = (iEffect * (m_pCity->GetNumTerrainWorked(eTerrain) + 1)) / 100;
			if (iTempBonus != iTempBonusPlusOne)
				iBonus += iTempBonusPlusOne;
		}

		int iTempBonus = (m_pCity->GetYieldPerXTerrainFromBuildingsTimes100(eTerrain, eYield) * (m_pCity->GetNumTerrainWorked(eTerrain))) / 100;
		int iTempBonusPlusOne = (m_pCity->GetYieldPerXTerrainFromBuildingsTimes100(eTerrain, eYield) * (m_pCity->GetNumTerrainWorked(eTerrain) + 1)) / 100;
		if (iTempBonus != iTempBonusPlusOne)
			iBonus += iTempBonusPlusOne;
	}

	return iBonus;
}
// What is the overall value of the current plot?
// This is a bit tricky because the value of a plot changes depending on which other plots are being worked!
int CvCityCitizens::GetPlotValue(CvPlot* pPlot, SPrecomputedExpensiveNumbers& cache)
{
	int iValue = 0;

	ProcessTypes eProcess = m_pCity->getProductionProcess();
	const CvProcessInfo* pkProcessInfo = GC.getProcessInfo(eProcess);
	CityAIFocusTypes eFocus = GetFocusType();
	bool bAvoidGrowth = IsAvoidGrowth();
	bool bIsWorking = IsWorkingPlot(pPlot);

	//we always want to be growing a little bit
	//we might fall under the threshold if we stop working a given plot, so take that into account
	bool bEmphasizeFood = CityShouldEmphasizeFood(bIsWorking ? cache.iExcessFoodTimes100 - pPlot->getYield(YIELD_FOOD)*100 : cache.iExcessFoodTimes100);
	bool bEmphasizeProduction = CityShouldEmphasizeProduction();

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;

		//Simplification - errata yields not worth considering.
		if (eYield > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			break;

		//base yield plus combo bonuses. 
		int iYield100 = pPlot->getYield(eYield) * 100;

		//FIXME: if we're trying to find the worst worked plot, we have to consider losing the combo bonus!
		if (!bIsWorking)
			iYield100 += GetBonusPlotValue(pPlot, eYield, cache) * 100;

		if (iYield100 > 0)
		{
			//processes can convert between yields
			if (pkProcessInfo && pkProcessInfo->getProductionToYieldModifier(eYield) > 0)
				iYield100 += (iYield100 * pkProcessInfo->getProductionToYieldModifier(eYield)) / 100;

			//how much do we value certain yields
			if (eYield == YIELD_FOOD)
			{
				if (cache.iExcessFoodTimes100>0)
					// if we have growth penalties, pretend the yield is lower
					iYield100 += min(0, (iYield100*m_pCity->getGrowthMods()) / 100);

				// even if we don't want to grow we care a little, extra food can help against unhappiness from distress!
				if (!bEmphasizeFood && bAvoidGrowth)
					iYield100 /= 20;
			}

			int iYieldMod = GetYieldModForFocus(eYield, eFocus, bEmphasizeFood, bEmphasizeProduction, cache);

			iValue += iYield100*max(1,iYieldMod);
		}
	}

	return iValue;
}

bool CvCityCitizens::CityShouldEmphasizeFood(int iAssumedExcessFood) const
{
	bool bCityFoodProduction = !GET_PLAYER(GetOwner()).isHuman() && m_pCity->getPopulation() > 3 && m_pCity->isFoodProduction(); //settler!
	bool bSmallCity = m_pCity->getPopulation() < /*2 in CP, 4 in CBO*/ GD_INT_GET(CITY_MIN_SIZE_FOR_SETTLERS);
	int iFoodThreshold = GetExcessFoodThreshold100();

	//do we have enough food? always want to grow a little bit a least, so don't use zero as threshold
	bool bNeedFood = (iAssumedExcessFood < iFoodThreshold);

	return (bNeedFood || bSmallCity || bCityFoodProduction);
}

bool CvCityCitizens::CityShouldEmphasizeProduction() const
{
	bool bCityFoodProduction = !GET_PLAYER(GetOwner()).isHuman() && m_pCity->getPopulation() > 3 && m_pCity->isFoodProduction(); //settler!
	bool bCityWonderProduction = !GET_PLAYER(GetOwner()).isHuman() && m_pCity->getPopulation() > 3 && m_pCity->IsBuildingWorldWonder();
	return (bCityFoodProduction || bCityWonderProduction);
}

/// Are this City's Citizens under automation?
bool CvCityCitizens::IsAutomated() const
{
	return m_bAutomated;
}

/// Sets this City's Citizens to be under automation
void CvCityCitizens::SetAutomated(bool bValue)
{
	m_bAutomated = bValue;
}

/// Are this City's Specialists under automation?
bool CvCityCitizens::IsNoAutoAssignSpecialists() const
{
	return m_bNoAutoAssignSpecialists;
}

/// Sets this City's Specialists to be under automation
void CvCityCitizens::SetNoAutoAssignSpecialists(bool bValue, bool bReallocate)
{
	if (m_bNoAutoAssignSpecialists != bValue)
	{
		m_bNoAutoAssignSpecialists = bValue;

		// If we're giving the AI control clear all manually assigned Specialists
		if (!bValue)
			DoClearForcedSpecialists();

		if (bReallocate)
			DoReallocateCitizens(true);
	}
}

/// Is this City avoiding growth?
bool CvCityCitizens::IsAvoidGrowth() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	//failsafe for AI
	const CvPlayer& kPlayer = GET_PLAYER(GetOwner());
	if (!kPlayer.isHuman() && kPlayer.IsEmpireVeryUnhappy())
	{
		return true;
	}

	//comfort function for all
	if (kPlayer.IsEmpireUnhappy() && m_pCity->getHappinessDelta()<-1)
	{
		return true;
	}

	return IsForcedAvoidGrowth();
}

bool CvCityCitizens::IsForcedAvoidGrowth() const
{
	return m_bForceAvoidGrowth;
}

bool CvCityCitizens::SetForcedAvoidGrowth(bool bAvoidGrowth, bool bReallocate)
{
	if (m_bForceAvoidGrowth != bAvoidGrowth)
	{
		m_bForceAvoidGrowth = bAvoidGrowth;
		if (bReallocate)
			DoReallocateCitizens(true);

		return true;
	}

	return false;
}

/// What is this city focusing on?
CityAIFocusTypes CvCityCitizens::GetFocusType() const
{
	return m_eCityAIFocusTypes;
}

/// What is this city focusing on?
bool CvCityCitizens::SetFocusType(CityAIFocusTypes eFocus, bool bReallocate)
{
	FAssert(eFocus >= NO_CITY_AI_FOCUS_TYPE);
	FAssert(eFocus < NUM_CITY_AI_FOCUS_TYPES);

	if (eFocus != m_eCityAIFocusTypes)
	{
		m_eCityAIFocusTypes = eFocus;
		// Reallocate with our new focus
		if (bReallocate)
			DoReallocateCitizens(true,true);
		return true;
	}

	return false;
}

int CvCityCitizens::GetYieldModForFocus(YieldTypes eYield, CityAIFocusTypes eFocus, bool bEmphasizeFood, bool bEmphasizeProduction, const SPrecomputedExpensiveNumbers& cache)
{
	int iDefaultValue = 6; //not too low to limit influence of moddable values
	int iYieldMod = iDefaultValue;

	if (eYield == YIELD_FOOD)
	{
		if (eFocus == CITY_AI_FOCUS_TYPE_FOOD || bEmphasizeFood)
			iYieldMod += /*12*/ GD_INT_GET(AI_CITIZEN_VALUE_FOOD);

		iYieldMod += cache.iUnhappinessFromDistress;
	}
	else if (eYield == YIELD_PRODUCTION)
	{
		if (eFocus == CITY_AI_FOCUS_TYPE_PRODUCTION || bEmphasizeProduction)
			iYieldMod += /*12*/ GD_INT_GET(AI_CITIZEN_VALUE_PRODUCTION);

		iYieldMod += cache.iUnhappinessFromDistress;
	}
	else if (eYield == YIELD_GOLD)
	{
		if (eFocus == CITY_AI_FOCUS_TYPE_GOLD)
			iYieldMod += /*12*/ GD_INT_GET(AI_CITIZEN_VALUE_GOLD);

		iYieldMod += cache.iUnhappinessFromGold;
	}
	else if (eYield == YIELD_SCIENCE)
	{
		if (eFocus == CITY_AI_FOCUS_TYPE_SCIENCE)
			iYieldMod += /*16*/ GD_INT_GET(AI_CITIZEN_VALUE_SCIENCE);

		iYieldMod += cache.iUnhappinessFromScience;
	}
	else if (eYield == YIELD_CULTURE || eYield == YIELD_TOURISM)
	{
		if (eFocus == CITY_AI_FOCUS_TYPE_CULTURE)
			iYieldMod += /*16*/ GD_INT_GET(AI_CITIZEN_VALUE_CULTURE);

		iYieldMod += cache.iUnhappinessFromCulture;
	}
	else if (eYield == YIELD_FAITH || eYield == YIELD_GOLDEN_AGE_POINTS)
	{
		if (eFocus == CITY_AI_FOCUS_TYPE_FAITH)
			iYieldMod += /*12*/ GD_INT_GET(AI_CITIZEN_VALUE_FAITH);

		iYieldMod += cache.iUnhappinessFromReligion;
	}

	//sanity check: do not focus too much on anything else while we need food
	//if we are too greedy initially we fix it in OptimizeWorkedPlots()
	if (bEmphasizeFood && eYield != YIELD_FOOD)
		return min(iDefaultValue + /*12*/ GD_INT_GET(AI_CITIZEN_VALUE_FOOD), iYieldMod);
	else
		return iYieldMod;
}

/// What is the Building Type the AI likes the Specialist of most right now?
BuildingTypes CvCityCitizens::GetAIBestSpecialistBuilding(int& iSpecialistValue, bool bLogging)
{
	if (m_pCity->GetResistanceTurns() > 0)
		return NO_BUILDING;

	gCachedNumbers.update(m_pCity);

	int iBestSpecialistValue = GetSpecialistValue((SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST),gCachedNumbers);
	BuildingTypes eBestBuilding = NO_BUILDING;
	CvBuildingEntry* pBestBuildingInfo = NULL;

	//many buildings have the same specialist yields ...
	std::map<SpecialistTypes, int> specialistValueCache;

	// Loop through all Buildings
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if (pkBuildingInfo)
		{
			// Have this Building in the City?
			if (GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if (IsCanAddSpecialistToBuilding(eBuilding))
				{
					SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
					int iValue = 0;

					std::map<SpecialistTypes, int>::iterator it = specialistValueCache.find(eSpecialist);
					if (it != specialistValueCache.end())
						iValue = specialistValueCache[eSpecialist];
					else
					{
						iValue = GetSpecialistValue(eSpecialist, gCachedNumbers);
						specialistValueCache[eSpecialist] = iValue;
					}

					// Add a bit more weight to a Building if it has more slots (10% per).
					// This will bias the AI to fill a single building over spreading Specialists out
					int iBonus = ((GetNumSpecialistsAllowedByBuilding(*pkBuildingInfo) - 1) * iValue * 10) / 100;

					if (iValue+iBonus > iBestSpecialistValue)
					{
						eBestBuilding = eBuilding;
						iBestSpecialistValue = iValue + iBonus;
						pBestBuildingInfo = pkBuildingInfo;

						//return the unbiased value
						iSpecialistValue = iValue;
					}
				}
			}
		}
	}

	if (bLogging && GC.getLogging() && pBestBuildingInfo)
	{
		CvString strBaseString;
		CvString strOutBuf;
		CvString strFileName = "CityTileScorer.csv";
		FILogFile* pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strOutBuf.Format("%s, focus %d, idle citizens %d, specialist building: %s, score %d", 
			m_pCity->getName().GetCString(), m_eCityAIFocusTypes, GetNumUnassignedCitizens(), pBestBuildingInfo->GetDescription(), iSpecialistValue);
		strBaseString += strOutBuf;
		pLog->Msg(strBaseString);
	}

	return eBestBuilding;
}

/// What is the Building Type the AI likes the Specialist of most right now?
BuildingTypes CvCityCitizens::GetAIBestSpecialistCurrentlyInBuilding(int& iSpecialistValue, bool bWantBest)
{
	BuildingTypes eBestBuilding = NO_BUILDING;
	int iBestSpecialistValue = bWantBest ? -INT_MAX : INT_MAX;

	gCachedNumbers.update(m_pCity);

	//many buildings have the same specialist yields ...
	vector<int> checked(GC.getNumSpecialistInfos(),0);

	const vector<BuildingTypes>& allBuildings = m_pCity->GetCityBuildings()->GetAllBuildingsHere();
	for (size_t i=0; i<allBuildings.size(); i++)
	{
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(allBuildings[i]);
		int iUnforcedSpecialist = GetNumSpecialistsInBuilding(allBuildings[i]) - GetNumForcedSpecialistsInBuilding(allBuildings[i]);
		if (pkBuildingInfo &&  iUnforcedSpecialist > 0)
		{
			SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
			if (checked[eSpecialist]>0)
				continue;

			int iValue = GetSpecialistValue(eSpecialist,gCachedNumbers);
			checked[eSpecialist] = iValue;

			if (bWantBest && iValue > iBestSpecialistValue)
			{
				eBestBuilding = allBuildings[i];
				iBestSpecialistValue = iValue;
			}
			else if (!bWantBest && iValue < iBestSpecialistValue)
			{
				eBestBuilding = allBuildings[i];
				iBestSpecialistValue = iValue;
			}
		}
	}

	iSpecialistValue = iBestSpecialistValue;
	return eBestBuilding;
}

/// How valuable is eSpecialist?
int CvCityCitizens::GetSpecialistValue(SpecialistTypes eSpecialist, const SPrecomputedExpensiveNumbers& cache)
{
	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if (pSpecialistInfo == NULL)
		return 0;

	bool bEmphasizeFood = CityShouldEmphasizeFood(cache.iExcessFoodTimes100);
	bool bEmphasizeProduction = CityShouldEmphasizeProduction();

	///////
	// Bonuses
	//////////
	ProcessTypes eProcess = m_pCity->getProductionProcess();
	const CvProcessInfo* pkProcessInfo = GC.getProcessInfo(eProcess);
	CvPlayer* pPlayer = m_pCity->GetPlayer();

	int iValue = 0;

	CityAIFocusTypes eFocus = GetFocusType();

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;

		//Simplification - errata yields not worth considering.
		if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			break;

		int iYield100 = pPlayer->specialistYield(eSpecialist, eYield) * 100;
		//Culture is treated differently, sadly.
		if (eYield == YIELD_CULTURE)
			iYield100 = m_pCity->GetCultureFromSpecialist(eSpecialist) * 100;

		//FIXME: should we consider the food consumed by the specialist ... foodConsumptionSpecialistTimes100()
		//or maybe not, since we only consider specialists when we have food to spare

		if (iYield100 > 0)
		{
			//conversion to other yields
			if (pkProcessInfo && pkProcessInfo->getProductionToYieldModifier(eYield) > 0)
				iYield100 += (iYield100 * pkProcessInfo->getProductionToYieldModifier(eYield))/100;

			//religion bonus
			ReligionTypes eMajority = m_pCity->GetCityReligions()->GetReligiousMajority();
			if (eMajority >= RELIGION_PANTHEON)
			{
				const CvReligion* pReligion = m_pCity->GetCityReligions()->GetMajorityReligion();
				if (pReligion)
				{
					iYield100 += pReligion->m_Beliefs.GetSpecialistYieldChange(eSpecialist, eYield, m_pCity->getOwner(), m_pCity)*100;
					int iYield1Specialist = pReligion->m_Beliefs.GetYieldChangeAnySpecialist(eYield, m_pCity->getOwner(), m_pCity);
					if (GetTotalSpecialistCount() <= 0 && iYield1Specialist > 0)
						iYield100 += iYield1Specialist*100;
				}
			}

			//specialists don't produce food so we can ignore any emphasis
			int iYieldMod = GetYieldModForFocus(eYield, eFocus, bEmphasizeFood, bEmphasizeProduction, cache);

			//prefer to even out yields
			if (m_pCity->GetCityStrategyAI()->GetMostDeficientYield() == eYield)
				iYieldMod += 3;

			iValue += iYield100 * max(1, iYieldMod);
		}
	}

	//nothing else for laborers ...
	if (eSpecialist == (SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST))
		return iValue;

	// GPP modifiers
	int iMod = m_pCity->getGreatPeopleRateModifier() + GetPlayer()->getGreatPeopleRateModifier() + m_pCity->GetSpecialistRateModifier(eSpecialist);

	bool bWantArt = false;
	bool bWantScience = false;
	bool bWantDiplo = false;
	if (!GetPlayer()->isHuman())
	{
		if (GetPlayer()->GetPlayerTraits()->IsTourism() || GetPlayer()->GetDiplomacyAI()->IsGoingForCultureVictory() || GetPlayer()->GetDiplomacyAI()->IsCloseToCultureVictory())
		{
			bWantArt = true;
		}
		if (GetPlayer()->GetPlayerTraits()->IsNerd() || GetPlayer()->GetDiplomacyAI()->IsGoingForSpaceshipVictory() || GetPlayer()->GetDiplomacyAI()->IsCloseToSSVictory())
		{
			bWantScience = true;
		}
		if (GetPlayer()->GetPlayerTraits()->IsDiplomat() || GetPlayer()->GetDiplomacyAI()->IsGoingForDiploVictory() || GetPlayer()->GetDiplomacyAI()->IsCloseToDiploVictory())
		{
			bWantDiplo = true;
		}
	}

	// Player and Golden Age mods to this specific class
	if ((UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
	{
		iMod += GetPlayer()->getGreatScientistRateModifier();
		if (bWantScience)
		{
			iMod += 20;
		}
	}
	else if ((UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_WRITER"))
	{
		if (GetPlayer()->isGoldenAge())
		{
			iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatWriterRateModifier();
		}
		iMod += GetPlayer()->getGreatWriterRateModifier();
		if (bWantArt)
		{
			iMod += 20;
		}
	}
	else if ((UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
	{
		if (GetPlayer()->isGoldenAge())
		{
			iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatArtistRateModifier();
		}
		iMod += GetPlayer()->getGreatArtistRateModifier();
		if (bWantArt)
		{
			iMod += 20;
		}
	}
	else if ((UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
	{
		if (GetPlayer()->isGoldenAge())
		{
			iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatMusicianRateModifier();
		}
		iMod += GetPlayer()->getGreatMusicianRateModifier();
		if (bWantArt)
		{
			iMod += 20;
		}
	}
	else if ((UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
	{
		iMod += GetPlayer()->getGreatMerchantRateModifier();
		if (bWantDiplo)
		{
			iMod += 20;
		}
	}
	else if ((UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
	{
		iMod += GetPlayer()->getGreatEngineerRateModifier();
		if (bWantScience || bWantArt)
		{
			iMod += 20;
		}
	}
#if defined(MOD_DIPLOMACY_CITYSTATES)
	else if (MOD_DIPLOMACY_CITYSTATES && (UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
	{
		iMod += GetPlayer()->getGreatDiplomatRateModifier();
		if (bWantDiplo)
		{
			iMod += 20;
		}
	}
#endif

	GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
	if (eGreatPerson != NO_GREATPERSON)
	{
		iMod += GetPlayer()->getSpecificGreatPersonRateModifierFromMonopoly(eGreatPerson);
		if (GetPlayer()->isGoldenAge())
		{
			iMod += GetPlayer()->getGoldenAgeGreatPersonRateModifier(eGreatPerson);
			iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);

			const CvReligion* pReligion = m_pCity->GetCityReligions()->GetMajorityReligion();
			BeliefTypes eSecondaryPantheon = NO_BELIEF;
			if (pReligion)
			{
				iMod += pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGreatPerson, m_pCity->getOwner(), m_pCity, true);
				eSecondaryPantheon = GetCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief();
				if (eSecondaryPantheon != NO_BELIEF)
				{
					iMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
				}
			}

#if defined(MOD_RELIGION_PERMANENT_PANTHEON)
			// Mod for civs keeping their pantheon belief forever
			if (MOD_RELIGION_PERMANENT_PANTHEON)
			{
				if (GC.getGame().GetGameReligions()->HasCreatedPantheon(GetOwner()))
				{
					const CvReligion* pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, GetOwner());
					BeliefTypes ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(GetOwner());
					if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != eSecondaryPantheon)
					{
						if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, pReligion->m_eReligion, GetOwner()))) // check that the our religion does not have our belief, to prevent double counting
						{
							iMod += GC.GetGameBeliefs()->GetEntry(ePantheonBelief)->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
						}
					}
				}
			}
#endif
		}
		int iNumPuppets = GetPlayer()->GetNumPuppetCities();
		if (iNumPuppets > 0)
		{
			iMod += (iNumPuppets * GetPlayer()->GetPlayerTraits()->GetPerPuppetGreatPersonRateModifier(eGreatPerson));
		}
	}

	if (GetCity()->isCapital() && GetPlayer()->GetPlayerTraits()->IsDiplomaticMarriage())
	{
		int iNumMarried = 0;
		// Loop through all minors and get the total number we've met.
		for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
		{
			PlayerTypes eMinor = (PlayerTypes)iPlayerLoop;

			if (eMinor != GetPlayer()->GetID() && GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
			{
				if (!GET_PLAYER(eMinor).IsAtWarWith(GetPlayer()->GetID()) && GET_PLAYER(eMinor).GetMinorCivAI()->IsMarried(GetPlayer()->GetID()))
				{
					iNumMarried++;
				}
			}
		}
		if (iNumMarried > 0)
		{
			iMod += (iNumMarried * /*15*/ GD_INT_GET(BALANCE_MARRIAGE_GP_RATE));
		}
	}

	//Bonus for art-producing specialists
	UnitClassTypes eUnitClass = (UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass();
	if (eUnitClass != NO_UNITCLASS)
	{
		if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
		{
			int iEmptySlots = GET_PLAYER(m_pCity->getOwner()).GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT());
			if (iEmptySlots == 0)
			{
				iMod /= 3;
			}
			else
			{
				iMod += (iEmptySlots * 2);
			}
		}
		else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER"))
		{
			int iEmptySlots = GET_PLAYER(m_pCity->getOwner()).GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_LITERATURE());
			if (iEmptySlots == 0)
			{
				iMod /= 3;
			}
			else
			{
				iMod += (iEmptySlots * 2);
			}
		}
		else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
		{
			int iEmptySlots = GET_PLAYER(m_pCity->getOwner()).GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_MUSIC());
			if (iEmptySlots == 0)
			{
				iMod /= 3;
			}
			else
			{
				iMod += (iEmptySlots * 2);
			}

			if (bWantArt)
			{
				iMod += 10;
			}
		}
	}

	iValue *= (100+iMod);
	iValue /= 100;

	///////
	//Penalties
	//////////
	int iCityUnhappiness = 0;

	if (MOD_BALANCE_CORE_HAPPINESS)
	{
		//if we're happy, reduce specialist value by 3% per positive delta so we don't eat all our excess happiness with specialists
		iCityUnhappiness = m_pCity->getHappinessDelta();
		if (iCityUnhappiness > 0)
		{
			iCityUnhappiness = max(0, 15 - iCityUnhappiness);
			iCityUnhappiness *= 3;
		}
	}
	else
	{
		iCityUnhappiness = m_pCity->GetUnhappinessFromCitySpecialists() / 100;
		int iTotalUnhappiness = GetPlayer()->GetHappiness() - GetPlayer()->GetUnhappiness();

		//What percent is this city's specialist count having on our overall unhappiness?
		if (iTotalUnhappiness < 0)
		{
			iCityUnhappiness *= 100;
			iCityUnhappiness /= abs(iTotalUnhappiness);
		}
	}

	//And reduce the specialist's value based on this.
	iValue *= max(1, (100 - iCityUnhappiness));
	iValue /= 100;

	if (eFocus == CITY_AI_FOCUS_TYPE_FOOD)
		iValue /= 2;
	else if (eFocus == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE)
		iValue *= 2;

	return iValue;
}

/// How many Citizens need to be given a job?
int CvCityCitizens::GetNumUnassignedCitizens() const
{
	return m_iNumUnassignedCitizens;
}

/// Changes how many Citizens need to be given a job
void CvCityCitizens::ChangeNumUnassignedCitizens(int iChange)
{
	m_iNumUnassignedCitizens += iChange;
	CvAssert(m_iNumUnassignedCitizens >= 0);
}

/// How many Citizens are working Plots?
int CvCityCitizens::GetNumCitizensWorkingPlots() const
{
	return m_iNumCitizensWorkingPlots;
}

/// Changes how many Citizens are working Plots
void CvCityCitizens::ChangeNumCitizensWorkingPlots(int iChange)
{
	m_iNumCitizensWorkingPlots += iChange;
}

/// Pick the best Plot to work from one of our unassigned pool
bool CvCityCitizens::DoAddBestCitizenFromUnassigned(CvCity::eUpdateMode updateMode, bool bLogging)
{
	// We only assign the unassigned here, folks
	if (GetNumUnassignedCitizens() == 0)
		return false;

	int iNetFood100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - m_pCity->foodConsumptionTimes100();
	bool bCanAffordSpecialist = (iNetFood100 >= m_pCity->foodConsumptionSpecialistTimes100());
	bool bSpecialistForbidden = GET_PLAYER(GetOwner()).isHuman() && IsNoAutoAssignSpecialists();
	FILogFile* pLog = bLogging && GC.getLogging() ? LOGFILEMGR.GetLog("CityTileScorer.csv", FILogFile::kDontTimeStamp) : NULL;

	int iSpecialistValue = -1;
	BuildingTypes eBestSpecialistBuilding = NO_BUILDING;

	if (bCanAffordSpecialist && !bSpecialistForbidden)
		eBestSpecialistBuilding = GetAIBestSpecialistBuilding(iSpecialistValue, bLogging);

	int iBestPlotValue = -1;
	CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, eBEST_UNWORKED_NO_OVERRIDE, bLogging);

	if (iBestPlotValue > iSpecialistValue)
	{
		// Plot is best
		SetWorkingPlot(pBestPlot, true, updateMode);

		if (pLog)
		{
			int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
			CvString strOutBuf;
			strOutBuf.Format("now working plot %d:%d, current net food %d", pBestPlot->getX(), pBestPlot->getY(), iExcessFoodTimes100);
			pLog->Msg(strOutBuf);
		}

		return true;
	}
	else if (iSpecialistValue >= iBestPlotValue && iSpecialistValue > 0)
	{
		// Specialist is best (can also be a default specialist aka laborer)
		DoAddSpecialistToBuilding(eBestSpecialistBuilding, /*bForced*/ false, updateMode);

		if (pLog)
		{
			int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
			CvString strOutBuf;
			strOutBuf.Format("now working building %d, current net food %d", eBestSpecialistBuilding, iExcessFoodTimes100);
			pLog->Msg(strOutBuf);
		}

		return true;
	}
	else
	{
		// Laborer if we can't do anything else
		ChangeNumDefaultSpecialists(1, updateMode);
		return false;
	}
}

/// Pick the worst Plot to stop working
bool CvCityCitizens::DoRemoveWorstCitizen(CvCity::eUpdateMode updateMode, bool bRemoveForcedStatus, SpecialistTypes eDontChangeSpecialist)
{
#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
	int iCurrentCityPopulation = GetCity()->getPopulation(true);
#else
	int iCurrentCityPopulation = GetCity()->getPopulation();
#endif

	// Are all of our guys already not working Plots?
#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
	if (GetNumUnassignedCitizens() == GetCity()->getPopulation(true))
#else
	if (GetNumUnassignedCitizens() == GetCity()->getPopulation())
#endif
	{
		return false;
	}

	// Find default Specialist to pull off, if there is one
	if (GetNumDefaultSpecialists() > 0)
	{
		// Do we either have unforced default specialists we can remove?
		if (GetNumDefaultSpecialists() > GetNumForcedDefaultSpecialists())
		{
			ChangeNumDefaultSpecialists(-1, updateMode);
			return true;
		}
		if (GetNumDefaultSpecialists() > iCurrentCityPopulation)
		{
			ChangeNumForcedDefaultSpecialists(-1);
			ChangeNumDefaultSpecialists(-1, updateMode);
			return true;
		}
	}

	// No Default Specialists, remove a working Pop, if there is one
	int iWorstPlotValue = 0;
	CvPlot* pWorstPlot = GetBestCityPlotWithValue(iWorstPlotValue, bRemoveForcedStatus ? eWORST_WORKED_ANY : eWORST_WORKED_UNFORCED);

	if (pWorstPlot != NULL)
	{
		// If we were force-working this Plot, turn it off
		if (bRemoveForcedStatus && IsForcedWorkingPlot(pWorstPlot))
		{
			SetWorkingPlot(pWorstPlot, false, updateMode); //this automatically removes forced status
			return true;
		}
		else if (!IsForcedWorkingPlot(pWorstPlot))
		{
			SetWorkingPlot(pWorstPlot, false, updateMode);
			return true;
		}
	}
	// Have to resort to pulling away a good Specialist if we're 
	else if (!IsNoAutoAssignSpecialists() || bRemoveForcedStatus)
	{
		if (DoRemoveWorstSpecialist(eDontChangeSpecialist, NO_BUILDING, updateMode))
		{
			return true;
		}
	}

	return false;
}

/// Find a Plot the City is either working or not, and the best/worst value for it - this function does "double duty" depending on what the user wants to find
CvPlot* CvCityCitizens::GetBestCityPlotWithValue(int& iChosenValue, ePlotSelectionMode eMode, bool bLogging)
{
	int iBestPlotValue = (eMode<eWORST_WORKED_UNFORCED) ? -INT_MAX : INT_MAX;
	bool bBestPlotIsForcedWork = false;
	CvPlot* pBestPlot = NULL;

	FILogFile* pLog = bLogging && GC.getLogging() ? LOGFILEMGR.GetLog("CityTileScorer.csv", FILogFile::kDontTimeStamp) : NULL;
	gCachedNumbers.update(m_pCity);

	// Look at all workable Plots
	for (int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
	{
		//never touch this
		if (iPlotLoop == CITY_HOME_PLOT)
			continue;

		CvPlot* pLoopPlot = GetCityPlotFromIndex(iPlotLoop);
		if (!pLoopPlot)
			continue;

		int iValue = -1;
		switch (eMode)
		{
		case eBEST_UNWORKED_NO_OVERRIDE:
			if (IsCanWork(pLoopPlot) && !IsWorkingPlot(iPlotLoop))
			{
				iValue = GetPlotValue(pLoopPlot, gCachedNumbers);
				if ( pBestPlot==NULL || iValue > iBestPlotValue )
				{
					iBestPlotValue = iValue;
					bBestPlotIsForcedWork = IsForcedWorkingPlot(iPlotLoop);
					pBestPlot = pLoopPlot;
				}
			}
			break;
		case eBEST_UNWORKED_ALLOW_OVERRIDE:
			if (IsCanWorkWithOverride(pLoopPlot) && !IsWorkingPlot(iPlotLoop))
			{
				iValue = GetPlotValue(pLoopPlot, gCachedNumbers);
				if ( pBestPlot==NULL || iValue > iBestPlotValue )
				{
					iBestPlotValue = iValue;
					bBestPlotIsForcedWork = IsForcedWorkingPlot(iPlotLoop);
					pBestPlot = pLoopPlot;
				}
			}
			break;
		case eWORST_WORKED_UNFORCED:
			if (IsWorkingPlot(iPlotLoop) && !IsForcedWorkingPlot(iPlotLoop))
			{
				iValue = GetPlotValue(pLoopPlot, gCachedNumbers);
				if ( pBestPlot==NULL || iValue < iBestPlotValue )
				{
					iBestPlotValue = iValue;
					bBestPlotIsForcedWork = false;
					pBestPlot = pLoopPlot;
				}
			}
			break;
		case eWORST_WORKED_FORCED:
			if (IsWorkingPlot(iPlotLoop) && IsForcedWorkingPlot(iPlotLoop))
			{
				iValue = GetPlotValue(pLoopPlot, gCachedNumbers);
				if ( pBestPlot==NULL || iValue < iBestPlotValue )
				{
					iBestPlotValue = iValue;
					bBestPlotIsForcedWork = true;
					pBestPlot = pLoopPlot;
				}
			}
			break;
		case eWORST_WORKED_ANY:
			if (IsWorkingPlot(iPlotLoop))
			{
				const bool bIsForcedWork = IsForcedWorkingPlot(iPlotLoop);

				// Select forced working plots but prioritize unforced working plots.
				const bool bDisallowedByUnforcedPriority = !bBestPlotIsForcedWork && bIsForcedWork;
				const bool bSupersedesByUnforcedPriority = bBestPlotIsForcedWork && !bIsForcedWork;
				iValue = GetPlotValue(pLoopPlot, gCachedNumbers);
				if ((pBestPlot == NULL || bSupersedesByUnforcedPriority) || (!bDisallowedByUnforcedPriority && iValue < iBestPlotValue))
				{
					iBestPlotValue = iValue;
					bBestPlotIsForcedWork = bIsForcedWork;
					pBestPlot = pLoopPlot;
				}
			}
			break;
		}

		if (0) // (pLog)
		{
			CvString strOutBuf;
			strOutBuf.Format("check index %d, plot %d:%d (%df%dp%dg%do), score %d with forced work status %d. current net food %d", 
				iPlotLoop, pLoopPlot->getX(), pLoopPlot->getY(), pLoopPlot->getYield(YIELD_FOOD), pLoopPlot->getYield(YIELD_PRODUCTION), 
				pLoopPlot->getYield(YIELD_GOLD), pBestPlot->getYield(YIELD_SCIENCE) + pBestPlot->getYield(YIELD_CULTURE) + pBestPlot->getYield(YIELD_FAITH), iValue, int(bBestPlotIsForcedWork), gCachedNumbers.iExcessFoodTimes100);
			pLog->Msg(strOutBuf);
		}
	}

	// Passed in by reference
	iChosenValue = iBestPlotValue;
	if (pBestPlot == NULL)
		return NULL;

	if (pLog)
	{
		CvString strBaseString;
		CvString strOutBuf;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strOutBuf.Format("%s, focus %d%s, idle citizens %d, plot %d:%d (%df%dp%dg%do), score %d", 
			m_pCity->getName().GetCString(), m_eCityAIFocusTypes, IsForcedAvoidGrowth()?" no growth":"", GetNumUnassignedCitizens(), pBestPlot->getX(), pBestPlot->getY(), 
			pBestPlot->getYield(YIELD_FOOD), pBestPlot->getYield(YIELD_PRODUCTION), pBestPlot->getYield(YIELD_GOLD), 
			pBestPlot->getYield(YIELD_SCIENCE) + pBestPlot->getYield(YIELD_CULTURE) + pBestPlot->getYield(YIELD_FAITH), iChosenValue);
		strBaseString += strOutBuf;
		pLog->Msg(strBaseString);
	}

	return pBestPlot;
}

//how much excess food should a city produce, so that it is growing at an adequate rate
int CvCityCitizens::GetExcessFoodThreshold100() const
{
	if (IsAvoidGrowth())
	{
		//slowly approach full stores
		if (m_pCity->getFood() == m_pCity->growthThreshold())
			return 0;
		else
			return 200;
	}
	else
	{
		CityAIFocusTypes eFocus = GetFocusType();
		if (eFocus == NO_CITY_AI_FOCUS_TYPE || eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH || eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
			return max(200, m_pCity->getPopulation() * 50);

		if (eFocus == CITY_AI_FOCUS_TYPE_FOOD)
			return m_pCity->getPopulation() * 150;

		//default (other specializations)
		return 200;
	}
}

//see if we can find a better assignment when we're not assigning plots greedily from scratch but from the final state where all citizens are already fed
void CvCityCitizens::OptimizeWorkedPlots(bool bLogging)
{
	int iCount = 0;
	FILogFile* pLog = bLogging && GC.getLogging() ? LOGFILEMGR.GetLog("CityTileScorer.csv", FILogFile::kDontTimeStamp) : NULL;
	bool bIsHuman = GET_PLAYER(GetOwner()).isHuman();
	bool bSpecialistForbidden = bIsHuman && IsNoAutoAssignSpecialists();

	//failsafe: if we have unassigned citizens get then assign them first
	while (GetNumUnassignedCitizens() > 0)
		DoAddBestCitizenFromUnassigned(CvCity::YIELD_UPDATE_LOCAL);

	//do not forget default specialists aka laborers
	gCachedNumbers.update(m_pCity);
	int iLaborerValue = GetSpecialistValue((SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST), gCachedNumbers);

	//failsafe against switching back and forth, don't try this too often
	while (iCount < m_pCity->getPopulation() / 2)
	{
		//now the real check
		int iWorstWorkedPlotValue = 0;
		int iWorstSpecialistValue = 0;
		int iBestFreePlotValue = 0;
		int iBestSpecialistValue = 0;

		//where do we have potential for improvement?
		CvPlot* pWorstWorkedPlot = GetBestCityPlotWithValue(iWorstWorkedPlotValue, eWORST_WORKED_UNFORCED);
		BuildingTypes eWorstSpecialistBuilding = GetAIBestSpecialistCurrentlyInBuilding(iWorstSpecialistValue, false);

		bool bReleaseLaborer = (GetNumDefaultSpecialists() > GetNumForcedDefaultSpecialists()) && (iWorstSpecialistValue > iLaborerValue);
		if (bReleaseLaborer)
			eWorstSpecialistBuilding = NO_BUILDING;

		//both options are valid
		if (pWorstWorkedPlot && eWorstSpecialistBuilding != NO_BUILDING)
		{
			if (iWorstWorkedPlotValue < iWorstSpecialistValue)
				//remove the citizen from the plot (at least temporarily) so that combo bonuses can be considered correctly
				SetWorkingPlot(pWorstWorkedPlot, false, CvCity::YIELD_UPDATE_LOCAL);
			else
			{
				DoRemoveSpecialistFromBuilding(eWorstSpecialistBuilding, false, CvCity::YIELD_UPDATE_LOCAL);
				pWorstWorkedPlot = NULL;
			}
		}
		else if (bReleaseLaborer)
		{
			if (iWorstWorkedPlotValue < iLaborerValue)
				//plot should be released
				SetWorkingPlot(pWorstWorkedPlot, false, CvCity::YIELD_UPDATE_LOCAL);
			else
			{
				//laborer should be released
				ChangeNumDefaultSpecialists(-1, CvCity::YIELD_UPDATE_LOCAL);
				pWorstWorkedPlot = NULL;
			}
		}
		else if (pWorstWorkedPlot)
		{
			//no alternative
			SetWorkingPlot(pWorstWorkedPlot, false, CvCity::YIELD_UPDATE_LOCAL);
		}
		else if (eWorstSpecialistBuilding != NO_BUILDING)
			//only specialist can be released
			DoRemoveSpecialistFromBuilding(eWorstSpecialistBuilding, false, CvCity::YIELD_UPDATE_LOCAL);
		else
			//cannot change anything
			break;

		//consider alternatives (no automatic plot overrides for humans!)
		CvPlot* pBestFreePlot = GetBestCityPlotWithValue(iBestFreePlotValue, bIsHuman ? eBEST_UNWORKED_NO_OVERRIDE : eBEST_UNWORKED_ALLOW_OVERRIDE);
		BuildingTypes eBestSpecialistBuilding = NO_BUILDING;

		int iNetFood100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - m_pCity->foodConsumptionTimes100();
		bool bCanAffordSpecialist = (iNetFood100 >= m_pCity->foodConsumptionSpecialistTimes100()+GetExcessFoodThreshold100());
		if (bCanAffordSpecialist && !bSpecialistForbidden) 
			eBestSpecialistBuilding = GetAIBestSpecialistBuilding(iBestSpecialistValue);

		//can a laborer be better than a specialist?
		if (iBestSpecialistValue < iLaborerValue)
		{
			eBestSpecialistBuilding = NO_BUILDING;
			iBestSpecialistValue = iLaborerValue;
		}

		//better work a plot than a specialist?
		if (iBestFreePlotValue > iBestSpecialistValue)
		{
			//are we taking an (unworked!) plot from another city?
			if (!pBestFreePlot->isEffectiveOwner(m_pCity))
				pBestFreePlot->setOwningCityOverride(m_pCity);

			SetWorkingPlot(pBestFreePlot, true, CvCity::YIELD_UPDATE_GLOBAL);

			//new plot is same as old plot, we're done
			if (pBestFreePlot == pWorstWorkedPlot)
			{
				if (pLog)
				{
					int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
					CvString strOutBuf;
					strOutBuf.Format("nothing to optimize, current net food %d", iExcessFoodTimes100);
					pLog->Msg(strOutBuf);
				}
				break;
			}

			if (pLog)
			{
				int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
				CvString strOutBuf;
				if (pWorstWorkedPlot)
				{
					strOutBuf.Format("switched plot %d:%d (score %d) to plot %d:%d (score %d), current net food %d",
						pWorstWorkedPlot->getX(), pWorstWorkedPlot->getY(), iWorstWorkedPlotValue, pBestFreePlot->getX(), pBestFreePlot->getY(), iBestFreePlotValue, iExcessFoodTimes100);
				}
				else
				{
					strOutBuf.Format("assigned citizen to plot %d:%d (score %d), current net food %d",
						pBestFreePlot->getX(), pBestFreePlot->getY(), iBestFreePlotValue, iExcessFoodTimes100);
				}
				pLog->Msg(strOutBuf);
			}
		}
		else
		{
			//is a specialist better than working a plot?
			if (iBestSpecialistValue > iWorstWorkedPlotValue && pWorstWorkedPlot)
			{
				//this method also handles laborers
				DoAddSpecialistToBuilding(eBestSpecialistBuilding, /*bForced*/ false, CvCity::YIELD_UPDATE_GLOBAL);

				if (pLog)
				{
					CvBuildingEntry* pBuilding = GC.getBuildingInfo(eBestSpecialistBuilding);
					int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
					CvString strOutBuf;
					if (pWorstWorkedPlot)
					{
						strOutBuf.Format("switched plot %d:%d (score %d) to specialist (%s, score %d), current net food %d",
							pWorstWorkedPlot->getX(), pWorstWorkedPlot->getY(), iWorstWorkedPlotValue, pBuilding ? pBuilding->GetType() : "default", iBestSpecialistValue, iExcessFoodTimes100);
					}
					else
					{
						strOutBuf.Format("assigned citizen to specialist (%s, score %d), current net food %d",
							pBuilding ? pBuilding->GetType() : "default", iBestSpecialistValue, iExcessFoodTimes100);
					}
					pLog->Msg(strOutBuf);
				}
			}
			else if (pWorstWorkedPlot)
			{
				//add the citizen back to the original plot
				SetWorkingPlot(pWorstWorkedPlot, true, CvCity::YIELD_UPDATE_GLOBAL);
				break;
			}
			else if (eWorstSpecialistBuilding != NULL)
			{
				//add the specialist back
				DoAddSpecialistToBuilding(eWorstSpecialistBuilding, /*bForced*/ false, CvCity::YIELD_UPDATE_GLOBAL);
			}
		}

		iCount++;
	}

	//failsafe: make sure we don't have anybody stuck in limbo (should not happen!)
	while (GetNumUnassignedCitizens() > 0)
		DoAddBestCitizenFromUnassigned(CvCity::YIELD_UPDATE_LOCAL);
}

bool CvCityCitizens::NeedReworkCitizens()
{
	if (IsDirty())
	{
		return true;
	}

	int iWorstWorkedPlotValue = 0;
	CvPlot* pWorstWorkedPlot = GetBestCityPlotWithValue(iWorstWorkedPlotValue, eWORST_WORKED_UNFORCED);

	int iBestUnworkedPlotValue = 0;
	CvPlot* pBestUnworkedPlot = GetBestCityPlotWithValue(iBestUnworkedPlotValue, eBEST_UNWORKED_NO_OVERRIDE);

	//First let's look at plots - if there is a better plot not being worked, we need to reallocate.
	if (pWorstWorkedPlot != NULL && pBestUnworkedPlot != NULL)
	{
		if (iBestUnworkedPlotValue > iWorstWorkedPlotValue)
		{
			return true;
		}
	}

	//Second let's look at specialists - if there is a better specialist not being worked, we need to reallocate.
	int iSpecialistValue = 0;
	BuildingTypes eBestSpecialistBuilding = NO_BUILDING;
	if (!GET_PLAYER(GetOwner()).isHuman() || !IsNoAutoAssignSpecialists())
	{
		eBestSpecialistBuilding = GetAIBestSpecialistBuilding(iSpecialistValue);
	}

	if (iSpecialistValue > iWorstWorkedPlotValue)
	{
		return true;
	}

	//for simplicity we do not check for laborers here
	int iSpecialistInCityValue = 0;
	BuildingTypes eBestSpecialistInCityBuilding = NO_BUILDING;
	if (!GET_PLAYER(GetOwner()).isHuman() || !IsNoAutoAssignSpecialists())
	{
		eBestSpecialistInCityBuilding = GetAIBestSpecialistCurrentlyInBuilding(iSpecialistInCityValue,true);
	}

	//check if new specialist is better than existing specialist
	return (iSpecialistValue > iSpecialistInCityValue);
}

/// Optimize our Citizen Placement
void CvCityCitizens::SetDirty(bool bValue)
{
	if (m_bIsDirty != bValue)
	{
		m_bIsDirty = bValue;
	}
}

bool CvCityCitizens::IsDirty()
{
	return m_bIsDirty;
}

void CvCityCitizens::DoReallocateCitizens(bool bForce, bool bLogging)
{
	DoVerifyWorkingPlots();

	DoValidateForcedWorkingPlots();

	//Let's check if we need to do this.
	if (!bForce && !NeedReworkCitizens())
		return;

	FILogFile* pLog = bLogging && GC.getLogging() ? LOGFILEMGR.GetLog("CityTileScorer.csv", FILogFile::kDontTimeStamp) : NULL;
	if (pLog)
		pLog->Msg("==== starting reallocation");

	// Remove all of the allocated guys (except forced)
	for (int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
	{
		if (iPlotLoop != CITY_HOME_PLOT && IsWorkingPlot(iPlotLoop) && !IsForcedWorkingPlot(iPlotLoop))
			SetWorkingPlot(GetCityPlotFromIndex(iPlotLoop), false, CvCity::YIELD_UPDATE_NONE);
	}

	// Remove Non-Forced Specialists in Buildings
	const vector<BuildingTypes>& allBuildings = m_pCity->GetCityBuildings()->GetAllBuildingsHere();
	for (size_t iBuildingLoop = 0; iBuildingLoop < allBuildings.size(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = allBuildings[iBuildingLoop];

		// Don't include Forced guys
		int iNumSpecialistsToRemove = GetNumSpecialistsInBuilding(eBuilding) - GetNumForcedSpecialistsInBuilding(eBuilding);

		// Loop through guys to remove (if there are any)
		for (int iSpecialistLoop = 0; iSpecialistLoop < iNumSpecialistsToRemove; iSpecialistLoop++)
			DoRemoveSpecialistFromBuilding(eBuilding, /*bForced*/ false, CvCity::YIELD_UPDATE_NONE);
	}

	// Remove Default Specialists (Laborers)
	int iNumDefaultsToRemove = GetNumDefaultSpecialists() - GetNumForcedDefaultSpecialists();
	for (int iSpecialistLoop = 0; iSpecialistLoop < iNumDefaultsToRemove; iSpecialistLoop++)
	{
		ChangeNumDefaultSpecialists(-1, CvCity::YIELD_UPDATE_NONE);
	}

	// do a single update, but treat player happiness as constant
	m_pCity->UpdateAllNonPlotYields(false);

	// Now put all of the unallocated guys back
	int iNumToAllocate = GetNumUnassignedCitizens();
	for (int iUnallocatedLoop = 0; iUnallocatedLoop < iNumToAllocate; iUnallocatedLoop++)
	{
		if (pLog)
			pLog->Msg("--- next allocation");

		//here we need to make sure to update after each assignment so we correctly track our needs
		DoAddBestCitizenFromUnassigned(CvCity::YIELD_UPDATE_LOCAL, bLogging);
	}

	if (pLog)
		pLog->Msg("==== starting optimization");

	//maybe we can reassign some citizens to work low food plots
	//(the default algorithm is greedy, so picks high food plots first without knowing where we land)
	OptimizeWorkedPlots(bLogging);

	//final update, this time include player happiness
	m_pCity->UpdateAllNonPlotYields(true);

	SetDirty(false);

	if (pLog)
		pLog->Msg("==== finished reallocation");
}



///////////////////////////////////////////////////
// Worked Plots
///////////////////////////////////////////////////

bool CvCityCitizens::IsWorkingPlot(int iIndex) const
{
	return iIndex>=0 ? m_pabWorkingPlot[iIndex] : false;
}

/// Is our City working a CvPlot?
bool CvCityCitizens::IsWorkingPlot(const CvPlot* pPlot) const
{
	int iIndex = GetCityIndexFromPlot(pPlot);

	if (iIndex != -1)
	{
		return m_pabWorkingPlot[iIndex];
	}

	return false;
}

/// Tell a City to start or stop working a Plot.  Citizens will go to/from the Unassigned Pool if the 3rd argument is true
void CvCityCitizens::SetWorkingPlot(CvPlot* pPlot, bool bNewValue, CvCity::eUpdateMode updateMode)
{
	if (!pPlot)
		return;

	int iIndex = GetCityIndexFromPlot(pPlot);

	CvAssertMsg(iIndex >= 0, "iIndex expected to be >= 0");

	int iPlotNum = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
	std::vector<int>::iterator it = std::find(m_vWorkedPlots.begin(), m_vWorkedPlots.end(), iPlotNum);
	if (bNewValue)
	{
		if (it == m_vWorkedPlots.end())
			m_vWorkedPlots.push_back(iPlotNum);
	}
	else
	{
		if (it != m_vWorkedPlots.end())
			m_vWorkedPlots.erase(it);

		//make sure we stay consistent
		SetForcedWorkingPlot(pPlot, false);
	}

	CvAssertMsg(iIndex < GetCity()->GetNumWorkablePlots(), "iIndex expected to be < NUM_CITY_PLOTS");

	if (IsWorkingPlot(pPlot) != bNewValue && iIndex >= 0 && iIndex < GetCity()->GetNumWorkablePlots())
	{
		m_pabWorkingPlot[iIndex] = bNewValue;

		// Don't look at the center Plot of a City, because we always work it for free
		if (iIndex != CITY_HOME_PLOT)
		{
			// Alter the count of Plots being worked by Citizens
			if (bNewValue)
			{
				ChangeNumCitizensWorkingPlots(1);
				ChangeNumUnassignedCitizens(-1);
			}
			else
			{
				ChangeNumCitizensWorkingPlots(-1);
				ChangeNumUnassignedCitizens(1);
			}
		}

		// Now working pPlot
		if (IsWorkingPlot(pPlot))
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				//Simplification - errata yields not worth considering.
				if ((YieldTypes)iI > YIELD_CULTURE_LOCAL && !MOD_BALANCE_CORE_JFD)
					break;

				GetCity()->ChangeBaseYieldRateFromTerrain(((YieldTypes)iI), pPlot->getYield((YieldTypes)iI));
			}

			if (iIndex != CITY_HOME_PLOT)
			{
				if (!pPlot->isEffectiveOwner(m_pCity))
					pPlot->setOwningCityOverride(m_pCity);

				if (pPlot->getTerrainType() != NO_TERRAIN)
				{
					GetCity()->ChangeNumTerrainWorked(pPlot->getTerrainType(), 1);
					if (pPlot->getFeatureType() == NO_FEATURE && !pPlot->isHills())
					{
						GetCity()->ChangeNumFeaturelessTerrainWorked(pPlot->getTerrainType(), 1);
					}
				}
				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					GetCity()->ChangeNumFeatureWorked(pPlot->getFeatureType(), 1);
				}
				if (pPlot->getResourceType(GetCity()->getTeam()) != NO_RESOURCE)
				{
					GetCity()->ChangeNumResourceWorked(pPlot->getResourceType(GetCity()->getTeam()), 1);
				}
				if (pPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					GetCity()->ChangeNumImprovementWorked(pPlot->getImprovementType(), 1);
				}
			}
		}
		// No longer working pPlot
		else
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				//Simplification - errata yields not worth considering.
				if ((YieldTypes)iI > YIELD_CULTURE_LOCAL && !MOD_BALANCE_CORE_JFD)
					break;

				GetCity()->ChangeBaseYieldRateFromTerrain(((YieldTypes)iI), -pPlot->getYield((YieldTypes)iI));
			}

			if (iIndex != CITY_HOME_PLOT)
			{
				if (pPlot->getTerrainType() != NO_TERRAIN)
				{
					GetCity()->ChangeNumTerrainWorked(pPlot->getTerrainType(), -1);
					if (pPlot->getFeatureType() == NO_FEATURE && !pPlot->isHills())
					{
						GetCity()->ChangeNumFeaturelessTerrainWorked(pPlot->getTerrainType(), -1);
					}
				}
				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					GetCity()->ChangeNumFeatureWorked(pPlot->getFeatureType(), -1);
				}
				if (pPlot->getResourceType(GetCity()->getTeam()) != NO_RESOURCE)
				{
					GetCity()->ChangeNumResourceWorked(pPlot->getResourceType(GetCity()->getTeam()), -1);
				}
				if (pPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					GetCity()->ChangeNumImprovementWorked(pPlot->getImprovementType(), -1);
				}
			}
		}

		//Let's only worry about yield changes if we need to update, or we're human (humans want instant feedback, whereas the AI doesn't always need that).
		if (updateMode == CvCity::YIELD_UPDATE_GLOBAL)
			m_pCity->UpdateAllNonPlotYields(true);
		else if (updateMode == CvCity::YIELD_UPDATE_LOCAL)
			m_pCity->UpdateAllNonPlotYields(false);

		if (GetCity()->isCitySelected())
		{
			GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
			//GC.GetEngineUserInterface()->setDirty(InfoPane_DIRTY_BIT, true );
			GC.GetEngineUserInterface()->setDirty(CityScreen_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(ColoredPlots_DIRTY_BIT, true);
		}

		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}

/// Tell City to work a Plot, pulling a Citizen from the worst location we can
void CvCityCitizens::DoAlterWorkingPlot(int iIndex)
{
	//cannot change anything if in resistance
	if (m_pCity->IsResistance() || m_pCity->IsIgnoreCityForHappiness())
		return;

	DoVerifyWorkingPlots();

	// Clicking ON the city "resets" it to default setup
	if (iIndex == CITY_HOME_PLOT)
	{
		// If we've forced any plots to be worked, reset them to the normal state
		for (int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
		{
			if (iPlotLoop != CITY_HOME_PLOT)
			{
				CvPlot* pLoopPlot = GetCityPlotFromIndex(iPlotLoop);
				if (pLoopPlot != NULL)
				{
					if (IsForcedWorkingPlot(pLoopPlot))
					{
						SetForcedWorkingPlot(pLoopPlot, false);
					}
				}
			}
		}

		// Reset Forced Default Specialists
		ChangeNumForcedDefaultSpecialists(-GetNumForcedDefaultSpecialists());
		DoReallocateCitizens(true, true);
	}
	else
	{
		//we have multiple types of citizens and need to do some bookkeeping when switching between categories
		// * working a plot
		// * specialists (in buildings)
		// * laborers (default specialists)
		// * unassigned (should be only temporary)

		CvPlot* pPlot = GetCityPlotFromIndex(iIndex);

		if (pPlot != NULL)
		{
			if (IsCanWork(pPlot))
			{
				// If we're already working the Plot, then take the guy off and turn him into a Default Specialist
				if (IsWorkingPlot(pPlot))
				{
					SetWorkingPlot(pPlot, false, CvCity::YIELD_UPDATE_LOCAL); //this automatically removes the forced status
					ChangeNumDefaultSpecialists(1, CvCity::YIELD_UPDATE_GLOBAL);
					ChangeNumForcedDefaultSpecialists(1);
				}
				// Player picked a new Plot to work
				else
				{
					// Pull from the Default Specialist pool, if possible
					if (GetNumDefaultSpecialists() > 0)
					{
						ChangeNumDefaultSpecialists(-1, CvCity::YIELD_UPDATE_LOCAL);
						// Player is forcibly telling city to work a plot, so reduce count of forced default specialists
						if (GetNumForcedDefaultSpecialists() > 0)
							ChangeNumForcedDefaultSpecialists(-1);

						SetWorkingPlot(pPlot, true, CvCity::YIELD_UPDATE_GLOBAL);
						SetForcedWorkingPlot(pPlot, true);
					
					}
					// No Default Specialists, so grab a better allocated guy
					else
					{
						// Shift a citizen to the new plot
						if (DoRemoveWorstCitizen(CvCity::YIELD_UPDATE_LOCAL))
						{
							SetWorkingPlot(pPlot, true, CvCity::YIELD_UPDATE_GLOBAL);
							SetForcedWorkingPlot(pPlot, true);
						}
					}
				}
			}
			//do not call isCanWorkWithOverride()
			else if (pPlot->getOwner() == GetOwner() && !pPlot->isBlockaded())
			{
				// Can't take away plots from puppet cities by force unless venice
				if (pPlot->getOwningCity()->IsPuppet() && !GET_PLAYER(GetOwner()).GetPlayerTraits()->IsNoAnnexing() )
					return;

				// Can't take away plots from cities which were just conquered
				if (pPlot->getOwningCity()->IsResistance() || pPlot->getOwningCity()->IsIgnoreCityForHappiness())
					return;

				pPlot->setOwningCityOverride(GetCity());
			}
		}
	}
}



bool CvCityCitizens::IsForcedWorkingPlot(int iRelativeIndex) const
{
	if (iRelativeIndex>=0 && iRelativeIndex<MAX_CITY_PLOTS)
		return m_pabForcedWorkingPlot[iRelativeIndex];

	return false;
}

/// Has our City been told it MUST a particular CvPlot?
bool CvCityCitizens::IsForcedWorkingPlot(const CvPlot* pPlot) const
{
	int iIndex = GetCityIndexFromPlot(pPlot);
	if (iIndex != -1)
	{
		return m_pabForcedWorkingPlot[iIndex];
	}

	return false;
}

/// Tell our City it MUST work a particular CvPlot
void CvCityCitizens::SetForcedWorkingPlot(CvPlot* pPlot, bool bNewValue)
{
	int iIndex = GetCityIndexFromPlot(pPlot);

	CvAssertMsg(iIndex >= 0, "iIndex expected to be >= 0");
	CvAssertMsg(iIndex < GetCity()->GetNumWorkablePlots(), "iIndex expected to be < NUM_CITY_PLOTS");

	if (IsForcedWorkingPlot(pPlot) != bNewValue && iIndex >= 0 && iIndex < GetCity()->GetNumWorkablePlots())
	{
		m_pabForcedWorkingPlot[iIndex] = bNewValue;

		// Change the count of how many are forced
		if (bNewValue)
		{
			ChangeNumForcedWorkingPlots(1);

			// More forced plots than we have citizens working?  If so, then pick someone to lose their forced status
			DoValidateForcedWorkingPlots();
		}
		else
		{
			ChangeNumForcedWorkingPlots(-1);
		}
	}
}

/// Make sure we don't have more forced working plots than we have citizens to work
void CvCityCitizens::DoValidateForcedWorkingPlots()
{
	int iNumForcedWorkingPlotsToDemote = GetNumForcedWorkingPlots() - GetNumCitizensWorkingPlots();
	if (iNumForcedWorkingPlotsToDemote > 0)
	{
		for (int iLoop = 0; iLoop < iNumForcedWorkingPlotsToDemote; iLoop++)
			DoDemoteWorstForcedWorkingPlot();
	}
}

/// Remove the Forced status from the worst ForcedWorking plot
bool CvCityCitizens::DoDemoteWorstForcedWorkingPlot()
{
	int iBestPlotValue = -1;
	int iBestPlotID = -1;

	gCachedNumbers.update(m_pCity);

	// Look at all workable Plots
	for (int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
	{
		if (iPlotLoop != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

			if (pLoopPlot != NULL)
			{
				if (IsForcedWorkingPlot(pLoopPlot))
				{
					int iValue = GetPlotValue(pLoopPlot, gCachedNumbers);

					// First, or worst yet?
					if (iBestPlotValue == -1 || iValue < iBestPlotValue)
					{
						iBestPlotValue = iValue;
						iBestPlotID = iPlotLoop;
					}
				}
			}
		}
	}

	if (iBestPlotID > -1)
	{
		CvPlot* pLoopPlot = GetCityPlotFromIndex(iBestPlotID);
		SetForcedWorkingPlot(pLoopPlot, false);
		return true;
	}

	return false;
}

/// How many plots have we forced to be worked?
int CvCityCitizens::GetNumForcedWorkingPlots() const
{
	return m_iNumForcedWorkingPlots;
}

/// Changes how many plots we have forced to be worked
void CvCityCitizens::ChangeNumForcedWorkingPlots(int iChange)
{
	if (iChange != 0)
	{
		m_iNumForcedWorkingPlots += iChange;
	}
}

/// Can our City work a particular CvPlot if we override ownership?
bool CvCityCitizens::IsCanWorkWithOverride(CvPlot* pPlot) const
{
	if (pPlot->getOwner() != m_pCity->getOwner())
	{
		return false;
	}

	if (!pPlot->hasYield())
	{
		return false;
	}

	if (pPlot->isWater())
	{
		if (!(GET_TEAM(GetTeam()).isWaterWork()))
		{
			return false;
		}
	}

	if (IsBlockaded(pPlot))
	{
		return false;
	}

	//looking up the effective owning city is expensive, so do a precheck
	if (!pPlot->isEffectiveOwner(m_pCity))
	{
		CvCity* pEffectiveOwner = pPlot->getEffectiveOwningCity();
		if (!pEffectiveOwner)
		{
			return false;
		}

		//we do not want to steal plots which are currently being worked or in the innermost ring
		//humans can still re-assign plots manually (see DoAlterWorkingPlot)
		if (pEffectiveOwner->GetCityCitizens()->IsWorkingPlot(pPlot) || plotDistance(pPlot->getX(),pPlot->getY(),pEffectiveOwner->getX(),pEffectiveOwner->getY())<2)
		{
			return false;
		}
	}

	return true;
}

/// Can our City work a particular CvPlot?
bool CvCityCitizens::IsCanWork(CvPlot* pPlot) const
{
	if (!pPlot->isEffectiveOwner(m_pCity))
	{
		return false;
	}

	return IsCanWorkWithOverride(pPlot);
}

bool CvCityCitizens::IsBlockaded(CvPlot * pPlot) const
{
	return std::find( m_vBlockadedPlots.begin(), m_vBlockadedPlots.end(), pPlot->GetPlotIndex()) != m_vBlockadedPlots.end();
}

void CvCityCitizens::SetBlockaded(CvPlot * pPlot)
{
	if (!IsBlockaded(pPlot))
		m_vBlockadedPlots.push_back(pPlot->GetPlotIndex());
}

void CvCityCitizens::ClearBlockades()
{
	m_vBlockadedPlots.clear();
}

/// Check all Plots by this City to see if we can actually be working them (if we are)
void CvCityCitizens::DoVerifyWorkingPlots()
{
	ClearBlockades();

	for (int iI = 0; iI < GetCity()->GetNumWorkablePlots(); iI++)
	{
		if (!IsWorkingPlot(iI))
			continue;

		CvPlot* pPlot = GetCityPlotFromIndex(iI);
		if (!pPlot)
			continue;

		//cache which plots are blockaded, the check can be expensive
		if (pPlot->isBlockaded())
			SetBlockaded(pPlot);

		//worked plot might be invalid now ... so move the citizen somewhere else
		if (!IsCanWork(pPlot))
		{
			SetWorkingPlot(pPlot, false, CvCity::YIELD_UPDATE_LOCAL);
			DoAddBestCitizenFromUnassigned(CvCity::YIELD_UPDATE_GLOBAL);
		}
	}
}




///////////////////////////////////////////////////
// Helpful Stuff
///////////////////////////////////////////////////




/// Returns the Plot Index from a CvPlot
int CvCityCitizens::GetCityIndexFromPlot(const CvPlot* pPlot) const
{
	return getRingIterationIndex(m_pCity->plot(), pPlot);
}


/// Returns the CvPlot from a Plot Index
CvPlot* CvCityCitizens::GetCityPlotFromIndex(int iIndex) const
{
	return iterateRingPlots(m_pCity->getX(), m_pCity->getY(), iIndex);
}




///////////////////////////////////////////////////
// Specialists
///////////////////////////////////////////////////
int CvCityCitizens::GetSpecialistRate(SpecialistTypes eSpecialist)
{
	if (CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity))
		return 0;

	int iGPPChange = 0;
	CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if (pkSpecialistInfo)
	{
		// Does this Specialist spawn a GP?
		if (pkSpecialistInfo->getGreatPeopleUnitClass() != NO_UNITCLASS)
		{
			int iCount = GetSpecialistCount(eSpecialist);

			// GPP from Specialists
			iGPPChange = pkSpecialistInfo->getGreatPeopleRateChange() * iCount * 100;

			// GPP from Buildings
			iGPPChange += GetBuildingGreatPeopleRateChanges(eSpecialist) * 100;

			const CvReligion* pReligion = m_pCity->GetCityReligions()->GetMajorityReligion();
			if (pReligion)
			{
				iGPPChange += pReligion->m_Beliefs.GetGreatPersonPoints(GetGreatPersonFromSpecialist(eSpecialist), m_pCity->getOwner(), m_pCity, true) * 100;
			}

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
			// GPP from resource monopolies
			GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
			if (eGreatPerson != NO_GREATPERSON)
			{
				iGPPChange += GetPlayer()->getSpecificGreatPersonRateChangeFromMonopoly(eGreatPerson) * 100;
			}
#endif

			if (iGPPChange > 0)
			{
				int iMod = 0;

				// City mod
				iMod += GetCity()->getGreatPeopleRateModifier();

				// Player mod
				iMod += GetPlayer()->getGreatPeopleRateModifier();

#if defined(MOD_BALANCE_CORE)
				iMod += GetCity()->GetSpecialistRateModifier(eSpecialist);
#endif

				// Player and Golden Age mods to this specific class
				if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
				{
					iMod += GetPlayer()->getGreatScientistRateModifier();
				}
				else if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_WRITER"))
				{
					if (GetPlayer()->isGoldenAge())
					{
						iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatWriterRateModifier();
					}
					iMod += GetPlayer()->getGreatWriterRateModifier();
				}
				else if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
				{
					if (GetPlayer()->isGoldenAge())
					{
						iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatArtistRateModifier();
					}
					iMod += GetPlayer()->getGreatArtistRateModifier();
				}
				else if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
				{
					if (GetPlayer()->isGoldenAge())
					{
						iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatMusicianRateModifier();
					}
					iMod += GetPlayer()->getGreatMusicianRateModifier();
				}
				else if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
				{
					iMod += GetPlayer()->getGreatMerchantRateModifier();
				}
				else if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
				{
					iMod += GetPlayer()->getGreatEngineerRateModifier();
				}
#if defined(MOD_DIPLOMACY_CITYSTATES)
				else if (MOD_DIPLOMACY_CITYSTATES && (UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
				{
					iMod += GetPlayer()->getGreatDiplomatRateModifier();
				}
#endif

#if defined(MOD_API_UNIFIED_YIELDS)
				GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
				if (eGreatPerson != NO_GREATPERSON)
				{
					iMod += GetPlayer()->getSpecificGreatPersonRateModifierFromMonopoly(eGreatPerson);
					if (GetPlayer()->isGoldenAge())
					{
						iMod += GetPlayer()->getGoldenAgeGreatPersonRateModifier(eGreatPerson);
						iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);

						const CvReligion* pReligion = m_pCity->GetCityReligions()->GetMajorityReligion();
						BeliefTypes eSecondaryPantheon = NO_BELIEF;
						if (pReligion)
						{
							iMod += pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGreatPerson, GetCity()->getOwner(), m_pCity);
							eSecondaryPantheon = GetCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief();
							if (eSecondaryPantheon != NO_BELIEF)
							{
								iMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
							}
						}

#if defined(MOD_RELIGION_PERMANENT_PANTHEON)
						// Mod for civs keeping their pantheon belief forever
						if (MOD_RELIGION_PERMANENT_PANTHEON)
						{
							if (GC.getGame().GetGameReligions()->HasCreatedPantheon(GetOwner()))
							{
								const CvReligion* pPantheon = GC.getGame().GetGameReligions()->GetReligion(RELIGION_PANTHEON, GetOwner());
								BeliefTypes ePantheonBelief = GC.getGame().GetGameReligions()->GetBeliefInPantheon(GetOwner());
								if (pPantheon != NULL && ePantheonBelief != NO_BELIEF && ePantheonBelief != eSecondaryPantheon)
								{
									if (pReligion == NULL || (pReligion != NULL && !pReligion->m_Beliefs.IsPantheonBeliefInReligion(ePantheonBelief, pReligion->m_eReligion, GetOwner()))) // check that the our religion does not have our belief, to prevent double counting
									{
										iMod += GC.GetGameBeliefs()->GetEntry(ePantheonBelief)->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
									}
								}
							}
						}
#endif
					}
				}
				int iNumPuppets = GetPlayer()->GetNumPuppetCities();
				if (iNumPuppets > 0)
				{
					GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
					if (eGreatPerson != NO_GREATPERSON)
					{
						iMod += (iNumPuppets * GetPlayer()->GetPlayerTraits()->GetPerPuppetGreatPersonRateModifier(eGreatPerson));
					}
				}
#endif
#if defined(MOD_BALANCE_CORE)
				if (GetCity()->isCapital() && GetPlayer()->GetPlayerTraits()->IsDiplomaticMarriage())
				{
					int iNumMarried = 0;
					// Loop through all minors and get the total number we've met.
					for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
					{
						PlayerTypes eMinor = (PlayerTypes)iPlayerLoop;

						if (eMinor != GetPlayer()->GetID() && GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
						{
							if (!GET_PLAYER(eMinor).IsAtWarWith(GetPlayer()->GetID()) && GET_PLAYER(eMinor).GetMinorCivAI()->IsMarried(GetPlayer()->GetID()))
							{
								iNumMarried++;
							}
						}
					}
					if (iNumMarried > 0)
					{
						iMod += (iNumMarried * /*15*/ GD_INT_GET(BALANCE_MARRIAGE_GP_RATE));
					}
				}
#endif
				// Apply mod
				iGPPChange *= (100 + iMod);
				iGPPChange /= 100;

			}
		}
	}
	return iGPPChange;
}


/// Called at the end of every turn: Looks at the specialists in this City and levels them up
void CvCityCitizens::DoSpecialists()
{
	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);

		int iGPPChange = GetSpecialistRate(eSpecialist);
		if (iGPPChange != 0)
		{
			CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
			int iGPThreshold = GetSpecialistUpgradeThreshold((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass());
			ChangeSpecialistGreatPersonProgressTimes100(eSpecialist, iGPPChange);

#if defined(MOD_EVENTS_CITY)
			if (MOD_EVENTS_CITY) {
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_CityPrepared, GetCity()->getOwner(), GetCity(), eSpecialist, iGPPChange, iGPThreshold);
			}
#endif

			// Enough to spawn a GP?
			if (GetSpecialistGreatPersonProgress(eSpecialist) >= iGPThreshold)
			{
				// No Minors
				if (!GET_PLAYER(GetCity()->getOwner()).isMinorCiv())
				{
					// Reset progress on this Specialist
					DoResetSpecialistGreatPersonProgressTimes100(eSpecialist, (iGPThreshold * 100));

					// Now... actually create the GP!
					const UnitClassTypes eUnitClass = (UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass();
					if (eUnitClass != NO_UNITCLASS)
					{
						UnitTypes eUnit = GET_PLAYER(GetCity()->getOwner()).GetSpecificUnitType(eUnitClass);

#if defined(MOD_GLOBAL_TRULY_FREE_GP)
						DoSpawnGreatPerson(eUnit, true, false, false);
#else
						DoSpawnGreatPerson(eUnit, true, false);
#endif
					}
				}
			}
		}
	}
}

/// How many Specialists are assigned to this Building Type?
int CvCityCitizens::GetNumSpecialistsAllowedByBuilding(const CvBuildingEntry& kBuilding)
{
	return kBuilding.GetSpecialistCount();
}

/// Are we in the position to add another Specialist to eBuilding?
bool CvCityCitizens::IsCanAddSpecialistToBuilding(BuildingTypes eBuilding)
{
	CvAssert(eBuilding > -1);
	CvAssert(eBuilding < GC.getNumBuildingInfos());

#if defined(MOD_BALANCE_CORE)
	if (m_pCity->IsResistance() || m_pCity->IsRazing())
	{
		return false;
	}
#endif

	int iNumSpecialistsAssigned = GetNumSpecialistsInBuilding(eBuilding);

#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
	if(iNumSpecialistsAssigned < GetCity()->getPopulation(true) &&	// Limit based on Pop of City
#else
	if(iNumSpecialistsAssigned < GetCity()->getPopulation() &&	// Limit based on Pop of City
#endif
		iNumSpecialistsAssigned < GC.getBuildingInfo(eBuilding)->GetSpecialistCount() &&				// Limit for this particular Building
		iNumSpecialistsAssigned < /*4*/ GD_INT_GET(MAX_SPECIALISTS_FROM_BUILDING))	// Overall Limit
	{
		return true;
	}

	return false;
}

/// Adds and initializes a Specialist for this building
void CvCityCitizens::DoAddSpecialistToBuilding(BuildingTypes eBuilding, bool bForced, CvCity::eUpdateMode updateMode)
{
	CvAssert(eBuilding > -1);
	CvAssert(eBuilding < GC.getNumBuildingInfos());

	//no building means laborer
	if (eBuilding == NO_BUILDING)
	{
		ChangeNumDefaultSpecialists(1, updateMode);
		return;
	}

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if (pkBuildingInfo == NULL)
	{
		return;
	}

	SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();

	// Can't add more than the max
	if (IsCanAddSpecialistToBuilding(eBuilding))
	{
		// If we're force-assigning a specialist, then we can reduce the count on forced default specialists
		if (bForced)
		{
			if (GetNumForcedDefaultSpecialists() > 0)
				ChangeNumForcedDefaultSpecialists(-1);
		}

		// If we don't already have an Unassigned Citizen to turn into a Specialist, find one from somewhere
		if (GetNumUnassignedCitizens() == 0)
		{
			DoRemoveWorstCitizen(updateMode, true, /*Don't remove this type*/ eSpecialist);

			if (GetNumUnassignedCitizens() == 0)
			{
				// Still nobody, all the citizens may be assigned to the eSpecialist we are looking for, try again
				if (!DoRemoveWorstSpecialist(NO_SPECIALIST, eBuilding, updateMode))
				{
					return; // For some reason we can't do this, we must exit, else we will be going over the population count
				}
			}
		}

		// Increase count for the whole city
		m_aiSpecialistCounts[eSpecialist]++;
		m_aiNumSpecialistsInBuilding[eBuilding]++;

		if (bForced)
			m_aiNumForcedSpecialistsInBuilding[eBuilding]++;

		GetCity()->processSpecialist(eSpecialist, 1, updateMode);
		ChangeNumUnassignedCitizens(-1);

		//we added the first specialist, this may have religious effects
		if (GetTotalSpecialistCount() == 1)
		{
			GetCity()->UpdateReligion(GetCity()->GetCityReligions()->GetReligiousMajority(), false);
			GetCity()->UpdateAllNonPlotYields(updateMode==CvCity::YIELD_UPDATE_GLOBAL);
		}

		ICvUserInterface2* pkIFace = GC.GetEngineUserInterface();
		pkIFace->setDirty(GameData_DIRTY_BIT, true);
		pkIFace->setDirty(CityInfo_DIRTY_BIT, true);
		//pkIFace->setDirty(InfoPane_DIRTY_BIT, true );
		pkIFace->setDirty(CityScreen_DIRTY_BIT, true);
		pkIFace->setDirty(ColoredPlots_DIRTY_BIT, true);

		CvCity* pkCity = GetCity();
		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(pkCity);

		pkIFace->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_SPECIALISTS);
	}
}

/// Removes and uninitializes a Specialist for this building
void CvCityCitizens::DoRemoveSpecialistFromBuilding(BuildingTypes eBuilding, bool bForced, CvCity::eUpdateMode updateMode)
{
	CvAssert(eBuilding > -1);
	CvAssert(eBuilding < GC.getNumBuildingInfos());

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if (pkBuildingInfo == NULL)
	{
		return;
	}

	SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();

	int iNumSpecialistsAssigned = GetNumSpecialistsInBuilding(eBuilding);

	// Need at least 1 assigned to remove
	if (iNumSpecialistsAssigned > 0)
	{
		// Decrease count for the whole city
		m_aiSpecialistCounts[eSpecialist]--;
		m_aiNumSpecialistsInBuilding[eBuilding]--;

		if (bForced)
		{
			m_aiNumForcedSpecialistsInBuilding[eBuilding]--;
		}

		//we removed the last specialist, this may have religious effects
		if (GetTotalSpecialistCount() == 0)
		{
			GetCity()->processSpecialist(eSpecialist, -1, updateMode);
			GetCity()->UpdateReligion(GetCity()->GetCityReligions()->GetReligiousMajority(), false);
		}
		else
		{
			GetCity()->processSpecialist(eSpecialist, -1, updateMode);
		}

		if (updateMode==CvCity::YIELD_UPDATE_GLOBAL)
		{
			GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();
			GetCity()->updateNetHappiness();
		}

		ChangeNumUnassignedCitizens(1);

		GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		//GC.GetEngineUserInterface()->setDirty(InfoPane_DIRTY_BIT, true );
		GC.GetEngineUserInterface()->setDirty(CityScreen_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(ColoredPlots_DIRTY_BIT, true);

		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(GetCity());

		GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_SPECIALISTS);
	}
}

//	----------------------------------------------------------------------------
/// Clear EVERYONE from this Building
/// Any one in the building will be put in the unassigned citizens list.
/// It is up to the caller to reassign population.
void CvCityCitizens::DoRemoveAllSpecialistsFromBuilding(BuildingTypes eBuilding, CvCity::eUpdateMode updateMode)
{
	CvAssert(eBuilding > -1);
	CvAssert(eBuilding < GC.getNumBuildingInfos());

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if (pkBuildingInfo == NULL)
	{
		return;
	}

	SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
	int iNumSpecialists = GetNumSpecialistsInBuilding(eBuilding);

	m_aiNumForcedSpecialistsInBuilding[eBuilding] = 0;

	// Pick the worst to remove
	for (int iAssignedLoop = 0; iAssignedLoop < iNumSpecialists; iAssignedLoop++)
	{
		// Decrease count for the whole city
		m_aiSpecialistCounts[eSpecialist]--;
		m_aiNumSpecialistsInBuilding[eBuilding]--;
		GetCity()->processSpecialist(eSpecialist, -1, updateMode);
		ChangeNumUnassignedCitizens(1);

		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		//GC.GetEngineUserInterface()->setDirty(InfoPane_DIRTY_BIT, true );
		GC.GetEngineUserInterface()->setDirty(CityScreen_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(ColoredPlots_DIRTY_BIT, true);

		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(GetCity());
		GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_SPECIALISTS);
	}

	GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();
	GetCity()->updateNetHappiness();
}


/// Find the worst Specialist and remove him from duty
bool CvCityCitizens::DoRemoveWorstSpecialist(SpecialistTypes eDontChangeSpecialist, const BuildingTypes eDontRemoveFromBuilding, CvCity::eUpdateMode updateMode)
{
	int iWorstValue = INT_MAX;
	BuildingTypes eWorstType = NO_BUILDING;

	gCachedNumbers.update(m_pCity);
	vector<int> checked(GC.getNumSpecialistInfos(),0);

	const vector<BuildingTypes>& allBuildings = m_pCity->GetCityBuildings()->GetAllBuildingsHere();
	for (size_t iBuildingLoop = 0; iBuildingLoop < allBuildings.size(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = allBuildings[iBuildingLoop];
		if (eBuilding == eDontRemoveFromBuilding)
		{
			continue;
		}

		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if (pkBuildingInfo == NULL)
		{
			continue;
		}

		// We might not be allowed to change this Building's Specialists
		SpecialistTypes specType = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
		if (eDontChangeSpecialist == specType)
		{
			continue;
		}

		//many buildings have the same specialist type
		if (checked[specType] > 0)
		{
			continue;
		}

		if (GetNumSpecialistsInBuilding(eBuilding) > 0)
		{
			int iValue = GetSpecialistValue((SpecialistTypes)pkBuildingInfo->GetSpecialistType(), gCachedNumbers);
			checked[specType] = iValue;

			if (iValue < iWorstValue)
			{
				iWorstValue = iValue;
				eWorstType = eBuilding;
			}
		}
	}

	if (eWorstType != NO_BUILDING)
	{
		DoRemoveSpecialistFromBuilding(eWorstType, true, updateMode);
		return true;
	}

	return false;
}

/// How many Default Specialists are assigned in this City?
int CvCityCitizens::GetNumDefaultSpecialists() const
{
	return m_iNumDefaultSpecialists;
}

/// Changes how many Default Specialists are assigned in this City
void CvCityCitizens::ChangeNumDefaultSpecialists(int iChange, CvCity::eUpdateMode updateMode)
{
	m_iNumDefaultSpecialists += iChange;

	SpecialistTypes eSpecialist = (SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST);
	m_aiSpecialistCounts[eSpecialist] += iChange;

	if (m_aiSpecialistCounts[eSpecialist] > m_pCity->getPopulation())
		OutputDebugString("warning: implausible number of specialists!\n");

	GetCity()->processSpecialist(eSpecialist, iChange, updateMode);

	ChangeNumUnassignedCitizens(-iChange);
}

/// How many Default Specialists have been forced assigned in this City?
int CvCityCitizens::GetNumForcedDefaultSpecialists() const
{
	return m_iNumForcedDefaultSpecialists;
}

/// How many Default Specialists have been forced assigned in this City?
void CvCityCitizens::ChangeNumForcedDefaultSpecialists(int iChange)
{
	m_iNumForcedDefaultSpecialists += iChange;
}

/// How many Specialists do we have assigned of this type in our City?
int CvCityCitizens::GetSpecialistCount(SpecialistTypes eIndex) const
{
	CvAssert(eIndex > -1);
	CvAssert(eIndex < GC.getNumSpecialistInfos());

	return m_aiSpecialistCounts[eIndex];
}

#if defined(MOD_BALANCE_CORE)
int CvCityCitizens::GetSpecialistSlots(SpecialistTypes eIndex) const
{
	CvAssert(eIndex > -1);
	CvAssert(eIndex < GC.getNumSpecialistInfos());

	return m_aiSpecialistSlots[eIndex];
}
void CvCityCitizens::ChangeNumSpecialistSlots(SpecialistTypes eIndex, int iValue)
{
	CvAssert(eIndex > -1);
	CvAssert(eIndex < GC.getNumSpecialistInfos());

	if (m_aiSpecialistSlots[eIndex] != iValue)
	{
		m_aiSpecialistSlots[eIndex] = iValue;
	}
}
int CvCityCitizens::GetSpecialistSlotsTotal() const
{
	int iNumSpecialists = 0;
	SpecialistTypes eSpecialist;

	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		eSpecialist = (SpecialistTypes)iSpecialistLoop;

		if (eSpecialist != (SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST))
		{
			iNumSpecialists += GetSpecialistSlots(eSpecialist);
		}
	}

	return iNumSpecialists;
}
#endif

/// Count up all the Specialists we have here
int CvCityCitizens::GetTotalSpecialistCount() const
{
	int iNumSpecialists = 0;
	SpecialistTypes eSpecialist;

	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		eSpecialist = (SpecialistTypes)iSpecialistLoop;

		if (eSpecialist != (SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST))
		{
			iNumSpecialists += GetSpecialistCount(eSpecialist);
		}
	}

	if (iNumSpecialists>m_pCity->getPopulation())
		OutputDebugString("warning: implausible number of specialists!\n");

	return iNumSpecialists;
}

/// GPP changes from Buildings
int CvCityCitizens::GetBuildingGreatPeopleRateChanges(SpecialistTypes eSpecialist) const
{
	CvAssert(eSpecialist > -1);
	CvAssert(eSpecialist < GC.getNumSpecialistInfos());

	return m_piBuildingGreatPeopleRateChanges[eSpecialist];
}

/// Change GPP from Buildings
void CvCityCitizens::ChangeBuildingGreatPeopleRateChanges(SpecialistTypes eSpecialist, int iChange)
{
	CvAssert(eSpecialist > -1);
	CvAssert(eSpecialist < GC.getNumSpecialistInfos());

	m_piBuildingGreatPeopleRateChanges[eSpecialist] += iChange;
}

/// How much progress does this City have towards a Great Person from eIndex?
int CvCityCitizens::GetSpecialistGreatPersonProgress(SpecialistTypes eIndex) const
{
	CvAssert(eIndex > -1);
	CvAssert(eIndex < GC.getNumSpecialistInfos());

	return GetSpecialistGreatPersonProgressTimes100(eIndex) / 100;
}

/// How much progress does this City have towards a Great Person from eIndex? (in hundreds)
int CvCityCitizens::GetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex) const
{
	CvAssert(eIndex > -1);
	CvAssert(eIndex < GC.getNumSpecialistInfos());

	return m_aiSpecialistGreatPersonProgressTimes100[eIndex];
}

/// How much progress does this City have towards a Great Person from eIndex?
void CvCityCitizens::ChangeSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex, int iChange, bool bCheckForSpawn)
{
	CvAssert(eIndex > -1);
	CvAssert(eIndex < GC.getNumSpecialistInfos());

	m_aiSpecialistGreatPersonProgressTimes100[eIndex] += iChange;

	if (bCheckForSpawn)
	{
		CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eIndex);
		if (pkSpecialistInfo)
		{
			int iGPThreshold = GetSpecialistUpgradeThreshold((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass());
			// Enough to spawn a GP?
			if (GetSpecialistGreatPersonProgress(eIndex) >= iGPThreshold)
			{
				// No Minors
				if (!GET_PLAYER(GetOwner()).isMinorCiv())
				{
					// Reset progress on this Specialist
#if defined(MOD_BALANCE_CORE)
					DoResetSpecialistGreatPersonProgressTimes100(eIndex, (iGPThreshold * 100));
#else
					DoResetSpecialistGreatPersonProgressTimes100(eSpecialist);
#endif

					// Now... actually create the GP!
					const UnitClassTypes eUnitClass = (UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass();
					if (eUnitClass != NO_UNITCLASS)
					{
						UnitTypes eUnit = GET_PLAYER(GetOwner()).GetSpecificUnitType(eUnitClass);
						if (eUnit != NO_UNIT)
						{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
							DoSpawnGreatPerson(eUnit, true, false, false);
#else
							DoSpawnGreatPerson(eUnit, true, false);
#endif
						}
					}
				}
			}
		}
	}
}

/// Reset Specialist progress
void CvCityCitizens::DoResetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex, int iAmountToRemove)
{
	CvAssert(eIndex > -1);
	CvAssert(eIndex < GC.getNumSpecialistInfos());
	m_aiSpecialistGreatPersonProgressTimes100[eIndex] -= iAmountToRemove;
}

/// How many Specialists are assigned to eBuilding?
int CvCityCitizens::GetNumSpecialistsInBuilding(BuildingTypes eBuilding) const
{
	CvAssert(eBuilding > -1);
	CvAssert(eBuilding < GC.getNumBuildingInfos());

	return m_aiNumSpecialistsInBuilding[eBuilding];
}

/// How many Forced Specialists are assigned to eBuilding?
int CvCityCitizens::GetNumForcedSpecialistsInBuilding(BuildingTypes eBuilding) const
{
	CvAssert(eBuilding > -1);
	CvAssert(eBuilding < GC.getNumBuildingInfos());

	return m_aiNumForcedSpecialistsInBuilding[eBuilding];
}

/// Remove forced status from all Specialists
void CvCityCitizens::DoClearForcedSpecialists()
{
	// Loop through all Buildings
	BuildingTypes eBuilding;
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		eBuilding = (BuildingTypes)iBuildingLoop;

		// Have this Building in the City?
		if (GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		{
			m_aiNumForcedSpecialistsInBuilding[eBuilding] = 0;
		}
	}
}

/// What upgrade progress does a Specialist need to level up?
int CvCityCitizens::GetSpecialistUpgradeThreshold(UnitClassTypes eUnitClass)
{
	int iThreshold = /*100 in CP, 150 in CBO*/ GD_INT_GET(GREAT_PERSON_THRESHOLD_BASE);
	int iNumCreated = 0;

	if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER", true))
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatWritersCreated(MOD_GLOBAL_TRULY_FREE_GP);
		iThreshold -= /*0*/ GD_INT_GET(GWAM_THRESHOLD_DECREASE);
#else
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatWritersCreated();
#endif
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST", true))
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatArtistsCreated(MOD_GLOBAL_TRULY_FREE_GP);
		iThreshold -= /*0*/ GD_INT_GET(GWAM_THRESHOLD_DECREASE);
#else
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatArtistsCreated();
#endif
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN", true))
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatMusiciansCreated(MOD_GLOBAL_TRULY_FREE_GP);
		iThreshold -= /*0*/ GD_INT_GET(GWAM_THRESHOLD_DECREASE);
#else
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatMusiciansCreated();
#endif
	}
#if defined(MOD_DIPLOMACY_CITYSTATES)
	else if (MOD_DIPLOMACY_CITYSTATES && eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT", true))
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatDiplomatsCreated(MOD_GLOBAL_TRULY_FREE_GP);
#else
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatDiplomatsCreated();
#endif
	}
#endif
	else
	{
#if defined(MOD_GLOBAL_SEPARATE_GP_COUNTERS)
		if (MOD_GLOBAL_SEPARATE_GP_COUNTERS)
		{
			if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MERCHANT", true)) {
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatMerchantsCreated(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatMerchantsCreated();
#endif
			}
			else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST", true)) {
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatScientistsCreated(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatScientistsCreated();
#endif
			}
			else
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatEngineersCreated(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatEngineersCreated();
#endif
			}
		}
		else
#endif
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatPeopleCreated(MOD_GLOBAL_TRULY_FREE_GP);
#else
			iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatPeopleCreated();
#endif
	}
#if defined(MOD_BALANCE_CORE)
	const UnitTypes eThisPlayersUnitType = GET_PLAYER(GetCity()->getOwner()).GetSpecificUnitType(eUnitClass);
	if (eThisPlayersUnitType != NO_UNIT)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eThisPlayersUnitType);
		if (pkUnitInfo != NULL)
		{
			if (pkUnitInfo->IsGPExtra() == 1)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra1Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra1Created();
#endif
			}
			else if (pkUnitInfo->IsGPExtra() == 2)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra2Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra2Created();
#endif
			}

			else if (pkUnitInfo->IsGPExtra() == 3)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra3Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra3Created();
#endif
			}
			else if (pkUnitInfo->IsGPExtra() == 4)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra4Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra4Created();
#endif
			}
			else if (pkUnitInfo->IsGPExtra() == 5)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra5Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra5Created();
#endif
			}
		}
	}

	int iMod = GET_PLAYER(GetCity()->getOwner()).GetPlayerTraits()->GetGreatPersonCostReduction(GetGreatPersonFromUnitClass(eUnitClass));
	if (iMod != 0)
	{
		iThreshold *= (100 + iMod);
		iThreshold /= 100;
	}
#endif
	// Increase threshold based on how many GP have already been spawned
	iThreshold += (/*100 in CP, 250 in CBO*/ GD_INT_GET(GREAT_PERSON_THRESHOLD_INCREASE) * iNumCreated);

	// Game Speed mod
	iThreshold *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	// Start era mod
	iThreshold *= GC.getGame().getStartEraInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	if (MOD_ALTERNATIVE_DIFFICULTY && !GET_PLAYER(GetCity()->getOwner()).isHuman())
	{
		iThreshold *= GC.getGame().getHandicapInfo().getAIGreatPersonPercent();
		iThreshold /= 100;
	}

	return iThreshold;
}

/// Create a GP!
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
void CvCityCitizens::DoSpawnGreatPerson(UnitTypes eUnit, bool bIncrementCount, bool bCountAsProphet, bool bIsFree)
#else
void CvCityCitizens::DoSpawnGreatPerson(UnitTypes eUnit, bool bIncrementCount, bool bCountAsProphet)
#endif
{
	CvAssert(eUnit != NO_UNIT);

	if (eUnit == NO_UNIT)
		return;	// Better than crashing.

	// If it's the active player then show the popup
	if (GetCity()->getOwner() == GC.getGame().getActivePlayer())
	{
		// Don't show in MP
		if (!GC.getGame().isReallyNetworkMultiPlayer())
		{
			CvPopupInfo kPopupInfo(BUTTONPOPUP_GREAT_PERSON_REWARD, eUnit, GetCity()->GetID());
			GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
		}
	}

	CvPlayer& kPlayer = GET_PLAYER(GetCity()->getOwner());
	CvUnit* newUnit = kPlayer.initUnit(eUnit, GetCity()->getX(), GetCity()->getY());

	// Bump up the count
	if (bIncrementCount && !bCountAsProphet)
	{
#if defined(MOD_BALANCE_CORE)
		if (kPlayer.GetPlayerTraits()->IsGPWLTKD())
		{
			int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
			iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iWLTKD /= 100;

			if (iWLTKD > 0)
			{
				GetCity()->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
				CvNotifications* pNotifications = kPlayer.GetNotifications();
				if (pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA");
					strText << newUnit->getNameKey() << GetCity()->getNameKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UA");
					strSummary << GetCity()->getNameKey();
					pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), GetCity()->getX(), GetCity()->getY(), -1);
				}
			}
		}
		if (newUnit->isWLKTKDOnBirth())
		{
			CvCity* pLoopCity;
			int iLoop;
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity != NULL)
				{
					int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
					iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
					iWLTKD /= 100;

					if (iWLTKD > 0)
					{
						pLoopCity->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
						CvNotifications* pNotifications = kPlayer.GetNotifications();
						if (pNotifications)
						{
							Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UNIT");
							strText << newUnit->getNameKey() << pLoopCity->getNameKey();
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UNIT");
							strSummary << pLoopCity->getNameKey();
							pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), pLoopCity->getX(), pLoopCity->getY(), -1);
						}
					}
				}
			}
		}
		if (newUnit->isGoldenAgeOnBirth())
		{
			int iGoldenAgeTurns = kPlayer.getGoldenAgeLength();
			int iValue = kPlayer.GetGoldenAgeProgressMeter();
			kPlayer.changeGoldenAgeTurns(iGoldenAgeTurns, iValue);
		}
		if (newUnit->isCultureBoost())
		{
			int iValue = kPlayer.GetTotalJONSCulturePerTurn() * 4;
			kPlayer.changeJONSCulture(iValue);
			if (kPlayer.getCapitalCity() != NULL)
			{
				kPlayer.getCapitalCity()->ChangeJONSCultureStored(iValue);
			}
			CvNotifications* pNotifications = kPlayer.GetNotifications();
			if (pNotifications)
			{
				Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CULTURE_UNIT");
				strText << newUnit->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CULTURE_UNIT");
				strSummary << newUnit->getNameKey();
				pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), newUnit->getX(), newUnit->getY(), -1);
			}
		}
#endif
		if (newUnit->IsGreatGeneral())
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGreatGeneralsCreated(bIsFree);
#else
			kPlayer.incrementGreatGeneralsCreated();
#endif
		}
		else if (newUnit->IsGreatAdmiral())
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGreatAdmiralsCreated(bIsFree);
#else
			kPlayer.incrementGreatAdmiralsCreated();
#endif
			CvPlot *pSpawnPlot = kPlayer.GetBestCoastalSpawnPlot(newUnit);
			if (newUnit->plot() != pSpawnPlot)
			{
				newUnit->setXY(pSpawnPlot->getX(), pSpawnPlot->getY());
			}
		}
		else if (newUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_WRITER"))
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGreatWritersCreated(bIsFree);
#else
			kPlayer.incrementGreatWritersCreated();
#endif
		}
		else if (newUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGreatArtistsCreated(bIsFree);
#else
			kPlayer.incrementGreatArtistsCreated();
#endif
		}
		else if (newUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGreatMusiciansCreated(bIsFree);
#else
			kPlayer.incrementGreatMusiciansCreated();
#endif
		}
#if defined(MOD_DIPLOMACY_CITYSTATES)
		else if (MOD_DIPLOMACY_CITYSTATES && newUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGreatDiplomatsCreated(bIsFree);
#else
			kPlayer.incrementGreatDiplomatsCreated();
#endif
		}
#endif
#if defined(MOD_BALANCE_CORE)
		else if (newUnit->getUnitInfo().IsGPExtra() == 1)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGPExtra1Created(bIsFree);
#else
			kPlayer.incrementGPExtra1Created();
#endif
		}
		else if (newUnit->getUnitInfo().IsGPExtra() == 2)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGPExtra2Created(bIsFree);
#else
			kPlayer.incrementGPExtra2Created();
#endif
		}
		else if (newUnit->getUnitInfo().IsGPExtra() == 3)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGPExtra3Created(bIsFree);
#else
			kPlayer.incrementGPExtra3Created();
#endif
		}
		else if (newUnit->getUnitInfo().IsGPExtra() == 4)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGPExtra4Created(bIsFree);
#else
			kPlayer.incrementGPExtra4Created();
#endif
		}
		else if (newUnit->getUnitInfo().IsGPExtra() == 5)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGPExtra5Created(bIsFree);
#else
			kPlayer.incrementGPExtra5Created();
#endif
		}
#endif
		else
		{
#if defined(MOD_GLOBAL_SEPARATE_GP_COUNTERS)
			if (MOD_GLOBAL_SEPARATE_GP_COUNTERS)
			{
				if (newUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
				{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
					kPlayer.incrementGreatMerchantsCreated(bIsFree);
#else
					kPlayer.incrementGreatMerchantsCreated();
#endif
				}
				else if (newUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
				{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
					kPlayer.incrementGreatScientistsCreated(bIsFree);
#else
					kPlayer.incrementGreatScientistsCreated();
#endif
				}
				else
				{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
					kPlayer.incrementGreatEngineersCreated(bIsFree);
#else
					kPlayer.incrementGreatEngineersCreated();
#endif
				}
			}
			else
#endif
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				kPlayer.incrementGreatPeopleCreated(bIsFree);
#else
				kPlayer.incrementGreatPeopleCreated();
#endif
		}
	}
	if (bCountAsProphet || newUnit->getUnitInfo().IsFoundReligion())
	{
#if defined(MOD_BALANCE_CORE)
		if (kPlayer.GetPlayerTraits()->IsGPWLTKD())
		{
			int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
			iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iWLTKD /= 100;

			if (iWLTKD > 0)
			{
				GetCity()->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
				CvNotifications* pNotifications = kPlayer.GetNotifications();
				if (pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA");
					strText << newUnit->getNameKey() << GetCity()->getNameKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UA");
					strSummary << GetCity()->getNameKey();
					pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), GetCity()->getX(), GetCity()->getY(), -1);
				}
			}
		}
		if (newUnit->isWLKTKDOnBirth())
		{
			CvCity* pLoopCity;
			int iLoop;
			for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity != NULL)
				{
					int iWLTKD = /*6*/ GD_INT_GET(CITY_RESOURCE_WLTKD_TURNS) / 3;
					iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
					iWLTKD /= 100;

					if (iWLTKD > 0)
					{
						pLoopCity->ChangeWeLoveTheKingDayCounter(iWLTKD, true);
						CvNotifications* pNotifications = kPlayer.GetNotifications();
						if (pNotifications)
						{
							Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UNIT");
							strText << newUnit->getNameKey() << pLoopCity->getNameKey();
							Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UNIT");
							strSummary << pLoopCity->getNameKey();
							pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), pLoopCity->getX(), pLoopCity->getY(), -1);
						}
					}
				}
			}
		}
		if (newUnit->isGoldenAgeOnBirth())
		{
			int iGoldenAgeTurns = kPlayer.getGoldenAgeLength();
			int iValue = kPlayer.GetGoldenAgeProgressMeter();
			kPlayer.changeGoldenAgeTurns(iGoldenAgeTurns, iValue);
		}
		if (newUnit->isCultureBoost())
		{
			int iValue = kPlayer.GetTotalJONSCulturePerTurn() * 4;
			kPlayer.changeJONSCulture(iValue);
			if (kPlayer.getCapitalCity() != NULL)
			{
				kPlayer.getCapitalCity()->ChangeJONSCultureStored(iValue);
			}
			CvNotifications* pNotifications = kPlayer.GetNotifications();
			if (pNotifications)
			{
				Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CULTURE_UNIT");
				strText << newUnit->getNameKey();
				Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CULTURE_UNIT");
				strSummary << newUnit->getNameKey();
				pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), newUnit->getX(), newUnit->getY(), -1);
			}
		}
#endif
		if (bIncrementCount)
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.GetReligions()->ChangeNumProphetsSpawned(1, bIsFree);
#else
			kPlayer.GetReligions()->ChangeNumProphetsSpawned(1);
#endif
	}

	// Setup prophet properly
	if (newUnit->getUnitInfo().IsFoundReligion())
	{
		ReligionTypes eReligion = kPlayer.GetReligions()->GetReligionCreatedByPlayer();
		newUnit->GetReligionData()->SetFullStrength(kPlayer.GetID(),newUnit->getUnitInfo(),eReligion,m_pCity);
	}

	if (newUnit->getUnitInfo().GetOneShotTourism() > 0)
	{
		newUnit->SetTourismBlastStrength(kPlayer.GetCulture()->GetTourismBlastStrength(newUnit->getUnitInfo().GetOneShotTourism()));
	}
	if (newUnit->getUnitInfo().GetTourismBonusTurns() > 0)
	{
		int iNumTurns = newUnit->getUnitInfo().GetTourismBonusTurns();
		CvCity *pLoopCity;
		int iLoop;
		for (pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
		{
			iNumTurns += pLoopCity->GetCityBuildings()->GetNumGreatWorks(CvTypes::getGREAT_WORK_SLOT_MUSIC());
		}

		iNumTurns *= GC.getGame().getGameSpeedInfo().getTrainPercent();
		iNumTurns /= 100;

		newUnit->SetTourismBlastLength(iNumTurns);
	}
#if defined(MOD_BALANCE_CORE)
	if (newUnit->getUnitInfo().GetBaseBeakersTurnsToCount() > 0)
	{
		newUnit->SetScienceBlastStrength(newUnit->getDiscoverAmount());
	}
	if (newUnit->getUnitInfo().GetBaseHurry() > 0)
	{
		newUnit->SetHurryStrength(newUnit->getHurryProduction(newUnit->plot()));
	}
	if (newUnit->getUnitInfo().GetBaseCultureTurnsToCount() > 0)
	{
		newUnit->SetCultureBlastStrength(newUnit->getGivePoliciesCulture());
	}
#endif

	// Notification
	if (GET_PLAYER(GetOwner()).GetNotifications())
	{
		Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER");
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_PERSON");
		GET_PLAYER(GetOwner()).GetNotifications()->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), GetCity()->getX(), GetCity()->getY(), eUnit);
	}
}

YieldTypes CvCityCitizens::GetFocusTypeYield(CityAIFocusTypes eFocus)
{
	YieldTypes eTargetYield = NO_YIELD;
	switch (eFocus)
	{
	case CITY_AI_FOCUS_TYPE_PRODUCTION:
		eTargetYield = YIELD_PRODUCTION;
		break;
	case CITY_AI_FOCUS_TYPE_CULTURE:
		eTargetYield = YIELD_CULTURE;
		break;
	case CITY_AI_FOCUS_TYPE_FAITH:
		eTargetYield = YIELD_FAITH;
		break;
	case CITY_AI_FOCUS_TYPE_FOOD:
		eTargetYield = YIELD_FOOD;
		break;
	case CITY_AI_FOCUS_TYPE_GOLD:
		eTargetYield = YIELD_GOLD;
		break;
	case CITY_AI_FOCUS_TYPE_SCIENCE:
		eTargetYield = YIELD_SCIENCE;
		break;
	}

	return eTargetYield;
}

SPrecomputedExpensiveNumbers::SPrecomputedExpensiveNumbers() :
	bonusForXFeature(YIELD_TOURISM, vector<int>(GC.getNumFeatureInfos(),INT_MAX)),
	bonusForXTerrain(YIELD_TOURISM, vector<int>(GC.getNumTerrainInfos(),INT_MAX)),
	iUnhappinessFromGold(0),
	iUnhappinessFromScience(0),
	iUnhappinessFromCulture(0),
	iUnhappinessFromReligion(0),
	iUnhappinessFromDistress(0),
	iExcessFoodTimes100(0),
	iFoodCorpMod(0)
{
}

void SPrecomputedExpensiveNumbers::update(CvCity * pCity)
{
	iUnhappinessFromGold = pCity->getUnhappinessFromGold();
	iUnhappinessFromScience = pCity->getUnhappinessFromScience();
	iUnhappinessFromCulture = pCity->getUnhappinessFromCulture();
	iUnhappinessFromReligion = pCity->getUnhappinessFromReligion();
	iUnhappinessFromDistress = max(pCity->getUnhappinessFromDefense(), pCity->getUnhappinessFromStarving());
	iExcessFoodTimes100 = pCity->getYieldRateTimes100(YIELD_FOOD, false) - (pCity->foodConsumptionTimes100());
	iFoodCorpMod = pCity->GetTradeRouteCityMod(YIELD_FOOD);

	//just reset this, we update it on demand
	for (size_t i = 0; i < bonusForXFeature.size(); i++)
		for (size_t j = 0; j < bonusForXFeature[i].size(); j++)
			bonusForXFeature[i][j] = INT_MAX;

	for (size_t i = 0; i < bonusForXTerrain.size(); i++)
		for (size_t j = 0; j < bonusForXTerrain[i].size(); j++)
			bonusForXTerrain[i][j] = INT_MAX;
}