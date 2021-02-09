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
#include "CvCitySpecializationAI.h"
#include "CvEconomicAI.h"
#include "CvGrandStrategyAI.h"
#include "CvDllInterfaces.h"
#include "CvInfosSerializationHelper.h"
#include "cvStopWatch.h"
#if defined(MOD_BALANCE_CORE)
#include "cvMilitaryAI.h"
#include "CvTypes.h"
#endif

// must be included after all other headers
#include "LintFree.h"


//=====================================
// CvCityCitizens
//=====================================

/// Constructor
CvCityCitizens::CvCityCitizens()
{
	m_aiSpecialistCounts = NULL;
#if defined(MOD_BALANCE_CORE)
	m_aiSpecialistSlots = NULL;
#endif
	m_aiSpecialistGreatPersonProgressTimes100 = NULL;
	m_aiNumSpecialistsInBuilding = NULL;
	m_aiNumForcedSpecialistsInBuilding = NULL;
	m_piBuildingGreatPeopleRateChanges = NULL;
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

	// Clear variables
	Reset();

	m_bInited = true;
}

/// Deallocate memory created in initialize
void CvCityCitizens::Uninit()
{
	if (m_bInited)
	{
		SAFE_DELETE_ARRAY(m_aiSpecialistCounts);
#if defined(MOD_BALANCE_CORE)
		SAFE_DELETE_ARRAY(m_aiSpecialistSlots);
#endif
		SAFE_DELETE_ARRAY(m_aiSpecialistGreatPersonProgressTimes100);
		SAFE_DELETE_ARRAY(m_aiNumSpecialistsInBuilding);
		SAFE_DELETE_ARRAY(m_aiNumForcedSpecialistsInBuilding);
		SAFE_DELETE_ARRAY(m_piBuildingGreatPeopleRateChanges);
	}
	m_bInited = false;
}

/// Reset member variables
void CvCityCitizens::Reset()
{
	m_bAutomated = false;
	m_bNoAutoAssignSpecialists = false;
#if defined(MOD_BALANCE_CORE)
	m_bIsDirty = false;
	m_bIsBlockaded = false;
#endif
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

	m_iNumDefaultSpecialists = 0;
	m_iNumForcedDefaultSpecialists = 0;

	CvAssertMsg(m_aiSpecialistCounts == NULL, "about to leak memory, CvCityCitizens::m_aiSpecialistCounts");
	m_aiSpecialistCounts = FNEW(int[GC.getNumSpecialistInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_aiSpecialistCounts[iI] = 0;
	}
#if defined(MOD_BALANCE_CORE)
	CvAssertMsg(m_aiSpecialistSlots == NULL, "about to leak memory, CvCityCitizens::m_aiSpecialistSlots");
	m_aiSpecialistSlots = FNEW(int[GC.getNumSpecialistInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_aiSpecialistSlots[iI] = 0;
	}
#endif

	CvAssertMsg(m_aiSpecialistGreatPersonProgressTimes100 == NULL, "about to leak memory, CvCityCitizens::m_aiSpecialistGreatPersonProgressTimes100");
	m_aiSpecialistGreatPersonProgressTimes100 = FNEW(int[GC.getNumSpecialistInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_aiSpecialistGreatPersonProgressTimes100[iI] = 0;
	}

	CvAssertMsg(m_aiNumSpecialistsInBuilding == NULL, "about to leak memory, CvCityCitizens::m_aiNumSpecialistsInBuilding");
	m_aiNumSpecialistsInBuilding = FNEW(int[GC.getNumBuildingInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_aiNumSpecialistsInBuilding[iI] = 0;
	}

	CvAssertMsg(m_aiNumForcedSpecialistsInBuilding == NULL, "about to leak memory, CvCityCitizens::m_aiNumForcedSpecialistsInBuilding");
	m_aiNumForcedSpecialistsInBuilding = FNEW(int[GC.getNumBuildingInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_aiNumForcedSpecialistsInBuilding[iI] = 0;
	}

	CvAssertMsg(m_piBuildingGreatPeopleRateChanges == NULL, "about to leak memory, CvCityCitizens::m_piBuildingGreatPeopleRateChanges");
	m_piBuildingGreatPeopleRateChanges = FNEW(int[GC.getNumSpecialistInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_piBuildingGreatPeopleRateChanges[iI] = 0;
	}

	m_bForceAvoidGrowth = false;
}

/// Serialization read
void CvCityCitizens::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;
	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_bAutomated;
	kStream >> m_bNoAutoAssignSpecialists;
	kStream >> m_bIsDirty;
	kStream >> m_bIsBlockaded;
	kStream >> m_iNumUnassignedCitizens;
	kStream >> m_iNumCitizensWorkingPlots;
	kStream >> m_iNumForcedWorkingPlots;

	kStream >> m_eCityAIFocusTypes;

	kStream >> m_bForceAvoidGrowth;

	kStream >> m_pabWorkingPlot;
	kStream >> m_pabForcedWorkingPlot;

	kStream >> m_iNumDefaultSpecialists;
	kStream >> m_iNumForcedDefaultSpecialists;

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiSpecialistCounts, GC.getNumSpecialistInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiSpecialistGreatPersonProgressTimes100, GC.getNumSpecialistInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiSpecialistSlots, GC.getNumSpecialistInfos());

	BuildingArrayHelpers::Read(kStream, m_aiNumSpecialistsInBuilding);
	BuildingArrayHelpers::Read(kStream, m_aiNumForcedSpecialistsInBuilding);

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_piBuildingGreatPeopleRateChanges, GC.getNumSpecialistInfos());

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
void CvCityCitizens::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_bAutomated;
	kStream << m_bNoAutoAssignSpecialists;
	kStream << m_bIsDirty;
	kStream << m_bIsBlockaded;
	kStream << m_iNumUnassignedCitizens;
	kStream << m_iNumCitizensWorkingPlots;
	kStream << m_iNumForcedWorkingPlots;

	kStream << m_eCityAIFocusTypes;

	kStream << m_bForceAvoidGrowth;

	kStream << m_pabWorkingPlot;
	kStream << m_pabForcedWorkingPlot;

	kStream << m_iNumDefaultSpecialists;
	kStream << m_iNumForcedDefaultSpecialists;

	CvInfosSerializationHelper::WriteHashedDataArray<SpecialistTypes, int>(kStream, m_aiSpecialistCounts, GC.getNumSpecialistInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<SpecialistTypes, int>(kStream, m_aiSpecialistGreatPersonProgressTimes100, GC.getNumSpecialistInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<SpecialistTypes, int>(kStream, m_aiSpecialistSlots, GC.getNumSpecialistInfos());
	BuildingArrayHelpers::Write(kStream, m_aiNumSpecialistsInBuilding, GC.getNumBuildingInfos());
	BuildingArrayHelpers::Write(kStream, m_aiNumForcedSpecialistsInBuilding, GC.getNumBuildingInfos());

	CvInfosSerializationHelper::WriteHashedDataArray<SpecialistTypes, int>(kStream, m_piBuildingGreatPeopleRateChanges, GC.getNumSpecialistInfos());
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
		SetWorkingPlot(pHomePlot, bWorkPlot, /*bUseUnassignedPool*/ false, /*bUpdateNow */ false);
	}
}

/// Processed every turn
void CvCityCitizens::DoTurn()
{
	AI_PERF_FORMAT("City-AI-perf.csv", ("CvCityCitizens::DoTurn, Turn %03d, %s, %s", GC.getGame().getElapsedGameTurns(), m_pCity->GetPlayer()->getCivilizationShortDescription(), m_pCity->getName().c_str()));
	DoVerifyWorkingPlots();

	CvPlayerAI& thisPlayer = GET_PLAYER(GetOwner());

	bool bForceCheck = false;
	if (!IsBlockade())
	{
		if (m_pCity->isCoastal() && m_pCity->IsBlockaded(true))
		{
			SetBlockade(true);
		}
	}
	else if (IsBlockade())
	{
		if (m_pCity->isCoastal() && !m_pCity->IsBlockaded(true))
		{
			SetBlockade(false);
			bForceCheck = true;
		}
	}

	if (m_pCity->IsPuppet())
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
		bool bNeedFood = m_pCity->GetCityStrategyAI()->IsYieldDeficient(YIELD_FOOD);

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

			int iLoop = 0;
			int iUnhappyAverage = 0;
			for (CvCity* pLoopCity = thisPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = thisPlayer.nextCity(&iLoop))
			{
				int iDelta = pLoopCity->getHappinessDelta();

				//now crazyness - this is how we cache the value
				pLoopCity->setHappinessDelta(iDelta);

				//mind the sign change
				if (iDelta < 0)
					iUnhappyAverage -= iDelta;
			}

			if (thisPlayer.getNumCities() > 0)
				iUnhappyAverage /= thisPlayer.getNumCities();

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
						iLockThreshold = GC.getUNHAPPY_THRESHOLD();
					else
						//later on tolerate some more unhappiness
						iLockThreshold = GC.getVERY_UNHAPPY_THRESHOLD();
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

int CvCityCitizens::GetBonusPlotValue(CvPlot* pPlot, YieldTypes eYield)
{
	int iBonus = 0;

	FeatureTypes eFeature = pPlot->getFeatureType();
	TerrainTypes eTerrain = pPlot->getTerrainType();

	if (eFeature != NO_FEATURE)
	{
		ReligionTypes eReligionFounded = m_pCity->GetCityReligions()->GetReligiousMajority();
		if (eReligionFounded != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligionFounded, m_pCity->getOwner());
			if (pReligion)
			{
				int iTempBonus = (pReligion->m_Beliefs.GetYieldPerXFeatureTimes100(eFeature, eYield, m_pCity->getOwner(), m_pCity) * (m_pCity->GetNumFeatureWorked(eFeature))) / 100;
				int iTempBonusPlusOne = (pReligion->m_Beliefs.GetYieldPerXFeatureTimes100(eFeature, eYield, m_pCity->getOwner(), m_pCity) * (m_pCity->GetNumFeatureWorked(eFeature) + 1)) / 100;
				if (iTempBonus != iTempBonusPlusOne)
					iBonus += iTempBonusPlusOne;
			}
		}

		int iTempBonus = (m_pCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeature, eYield) * (m_pCity->GetNumFeatureWorked(eFeature))) / 100;
		int iTempBonusPlusOne = (m_pCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeature, eYield) * (m_pCity->GetNumFeatureWorked(eFeature) + 1)) / 100;
		if (iTempBonus != iTempBonusPlusOne)
			iBonus += iTempBonusPlusOne;
	}
	if (eTerrain != NO_TERRAIN)
	{
		ReligionTypes eReligionFounded = m_pCity->GetCityReligions()->GetReligiousMajority();
		if (eReligionFounded != NO_RELIGION)
		{
			const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eReligionFounded, m_pCity->getOwner());
			if (pReligion)
			{
				int iTempBonus = (pReligion->m_Beliefs.GetYieldPerXTerrainTimes100(eTerrain, eYield, m_pCity->getOwner(), m_pCity) * (m_pCity->GetNumTerrainWorked(eTerrain))) / 100;
				int iTempBonusPlusOne = (pReligion->m_Beliefs.GetYieldPerXTerrainTimes100(eTerrain, eYield, m_pCity->getOwner(), m_pCity) * (m_pCity->GetNumTerrainWorked(eTerrain) + 1)) / 100;
				if (iTempBonus != iTempBonusPlusOne)
					iBonus += iTempBonusPlusOne;
			}
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
int CvCityCitizens::GetPlotValue(CvPlot* pPlot, SPrecomputedExpensiveNumbers store)
{
	int iValue = 0;

	bool bCityFoodProduction = m_pCity->isFoodProduction(); //settler!
	ProcessTypes eProcess = m_pCity->getProductionProcess();
	const CvProcessInfo* pkProcessInfo = GC.getProcessInfo(eProcess);
	CityAIFocusTypes eFocus = GetFocusType();

	bool bAvoidGrowth = IsAvoidGrowth();
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;

		//Simplification - errata yields not worth considering.
		if (eYield > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			break;

		//base yield plus combo bonuses
		int iYield100 = (pPlot->getYield(eYield)+GetBonusPlotValue(pPlot, eYield)) * 100;
		
		int iYieldMod = 0; //how much do we want it right now?
		int iUnhappyMod = 0; //how much does it help for happiness?

		if (iYield100 > 0)
		{
			//processes can convert between yields
			if (pkProcessInfo && pkProcessInfo->getProductionToYieldModifier(eYield) > 0)
				iYield100 += (iYield100 * pkProcessInfo->getProductionToYieldModifier(eYield)) / 100;

			//how much do we value certain yields
			if (eYield == YIELD_FOOD)
			{
				// if we have enough food we care little about extra food
				if (store.iExcessFoodTimes100 > 0)
				{
					if (bAvoidGrowth)
						// even if we don't want to grow we care a little, extra food can help against unhappiness from distress!
						iYield100 /= 20;
					else
						// if we have growth penalties, pretend the yield is lower
						iYield100 += (iYield100*m_pCity->getGrowthMods()) / 100;
				}

				if (eFocus == CITY_AI_FOCUS_TYPE_FOOD || bCityFoodProduction)
					iYieldMod = GC.getAI_CITIZEN_VALUE_FOOD();
				else if (eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH || eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
					iYieldMod = GC.getAI_CITIZEN_VALUE_FOOD()/2;

				if (store.iUnhappinessFromDistress > 0)
					iUnhappyMod = store.iUnhappinessFromDistress;
			}
			else if (eYield == YIELD_PRODUCTION)
			{
				if (eFocus == CITY_AI_FOCUS_TYPE_PRODUCTION || bCityFoodProduction || m_pCity->IsBuildingWorldWonder())
					iYieldMod = GC.getAI_CITIZEN_VALUE_PRODUCTION();

				if (eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
					iYieldMod = GC.getAI_CITIZEN_VALUE_PRODUCTION()/2;

				if (store.iUnhappinessFromDistress > 0)
					iUnhappyMod = store.iUnhappinessFromDistress;
			}
			else if (eYield == YIELD_GOLD)
			{		
				if (eFocus == CITY_AI_FOCUS_TYPE_GOLD)
					iYieldMod = GC.getAI_CITIZEN_VALUE_GOLD();

				if (eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
					iYieldMod = GC.getAI_CITIZEN_VALUE_GOLD()/2;

				if (store.iUnhappinessFromGold > 0)
					iUnhappyMod = store.iUnhappinessFromGold;
			}
			else if (eYield == YIELD_SCIENCE)
			{
				if (eFocus == CITY_AI_FOCUS_TYPE_SCIENCE)
					iYieldMod = GC.getAI_CITIZEN_VALUE_SCIENCE();

				if (store.iUnhappinessFromScience > 0)
					iUnhappyMod = store.iUnhappinessFromScience;
			}
			else if (eYield == YIELD_CULTURE || eYield == YIELD_TOURISM)
			{			
				if (eFocus == CITY_AI_FOCUS_TYPE_CULTURE)
					iYieldMod = GC.getAI_CITIZEN_VALUE_CULTURE();

				if (store.iUnhappinessFromCulture> 0)
					iUnhappyMod = store.iUnhappinessFromCulture;
			}
			else if (eYield == YIELD_FAITH || eYield == YIELD_GOLDEN_AGE_POINTS)
			{			
				if (eFocus == CITY_AI_FOCUS_TYPE_FAITH)
					iYieldMod = GC.getAI_CITIZEN_VALUE_FAITH();

				if (store.iUnhappinessFromReligion > 0)
					iUnhappyMod = store.iUnhappinessFromReligion;
			}

			//now put it all together
			if (m_pCity->GetCityStrategyAI()->GetMostDeficientYield() == eYield)
				iYieldMod += 2;
			else if (m_pCity->GetCityStrategyAI()->IsYieldDeficient(eYield))
				iYieldMod += 2;

			//no yield mods until we're fed!
			if (eYield == YIELD_FOOD || store.iExcessFoodTimes100 > 0)
				iYield100 *= max(1, iYieldMod+iUnhappyMod);

			iValue += iYield100;
		}
	}

	return iValue;
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
bool CvCityCitizens::IsAvoidGrowth()
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	//failsafe for AI
	if (!GetPlayer()->isHuman() && GetPlayer()->IsEmpireVeryUnhappy())
	{
		return true;
	}

	return IsForcedAvoidGrowth();
}

bool CvCityCitizens::IsForcedAvoidGrowth()
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
			DoReallocateCitizens(true);
		return true;
	}

	return false;
}

/// Does the AI want a Specialist?
bool CvCityCitizens::IsAIWantSpecialistRightNow()
{
	if (!CanCreateSpecialist())
		return false;

	int iWeight = 100;

	// If the City is Size 1 or 2 then we probably don't want Specialists
	if (m_pCity->getPopulation() < 3)
	{
		iWeight /= 2;
	}

	int iFoodPerTurn = m_pCity->getYieldRate(YIELD_FOOD, false);
	int iFoodEatenPerTurn = m_pCity->foodConsumption();
	int iSurplusFood = iFoodPerTurn - iFoodEatenPerTurn;

	CityAIFocusTypes eFocusType = GetFocusType();
	// Don't want specialists until we've met our food needs
	if (iSurplusFood < 0)
	{
		return false;
	}
	else if (IsAvoidGrowth() && (eFocusType == NO_CITY_AI_FOCUS_TYPE || eFocusType == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE))
	{
		iWeight *= 2;
	}
	else if (iSurplusFood <= 2)
	{
		iWeight /= 2;
	}
	else if (iSurplusFood > 2)
	{
		if (eFocusType == NO_CITY_AI_FOCUS_TYPE || eFocusType == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE || eFocusType == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
		{
			iWeight *= 100 + (20 * (iSurplusFood - 4));
			iWeight /= 100;
		}
	}

	// If we're deficient in Food then we're less likely to want Specialists
	if (m_pCity->GetCityStrategyAI()->IsYieldDeficient(YIELD_FOOD))
	{
		iWeight *= 75;
		iWeight /= 100;
	}
	// if we've got some slackers in town (since they provide Production)
	else if (GetNumDefaultSpecialists() > 0 && eFocusType != CITY_AI_FOCUS_TYPE_PRODUCTION && eFocusType != CITY_AI_FOCUS_TYPE_PROD_GROWTH)
	{
		iWeight *= 150;
		iWeight /= 100;
	}

	// Someone told this AI it should be focused on something that is usually gotten from specialists
	if (eFocusType == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE)
	{
		// Loop through all Buildings
		BuildingTypes eBuilding;
		for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			eBuilding = (BuildingTypes)iBuildingLoop;

			// Have this Building in the City?
			if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if (IsCanAddSpecialistToBuilding(eBuilding))
				{
					iWeight *= 5;
					break;
				}
			}
		}
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_CULTURE)
	{
		// Loop through all Buildings
		for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (pkBuildingInfo)
			{
				// Have this Building in the City?
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if (IsCanAddSpecialistToBuilding(eBuilding))
					{
						const SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if (pSpecialistInfo && pSpecialistInfo->getCulturePerTurn() > 0)
						{
							iWeight *= 5;
							break;
						}
					}
				}
			}
		}
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_SCIENCE)
	{
		// Loop through all Buildings
		BuildingTypes eBuilding;
		for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			eBuilding = (BuildingTypes)iBuildingLoop;

			// Have this Building in the City?
			if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if (IsCanAddSpecialistToBuilding(eBuilding))
				{
					SpecialistTypes eSpecialist = (SpecialistTypes)GC.getBuildingInfo(eBuilding)->GetSpecialistType();
					CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
					if (pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_SCIENCE) > 0)
					{
						iWeight *= 3;
					}

					if (GetPlayer()->getSpecialistExtraYield(YIELD_SCIENCE) > 0)
					{
						iWeight *= 3;
					}

					if (GetPlayer()->GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, YIELD_SCIENCE) > 0)
					{
						iWeight *= 3;
					}
				}
			}
		}
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_PRODUCTION)
	{
		// Loop through all Buildings
		for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (pkBuildingInfo)
			{
				// Have this Building in the City?
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if (IsCanAddSpecialistToBuilding(eBuilding))
					{
						SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if (NULL != pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_PRODUCTION) > 0)
						{
							iWeight *= 150;
							iWeight /= 100;
						}

						if (GetPlayer()->getSpecialistExtraYield(YIELD_PRODUCTION) > 0)
						{
							iWeight *= 2;
						}

						if (GetPlayer()->GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, YIELD_PRODUCTION) > 0)
						{
							iWeight *= 2;
						}
					}
				}
			}
		}
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_GOLD)
	{
		// Loop through all Buildings
		BuildingTypes eBuilding;
		for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			eBuilding = (BuildingTypes)iBuildingLoop;

			// Have this Building in the City?
			if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if (IsCanAddSpecialistToBuilding(eBuilding))
				{
					SpecialistTypes eSpecialist = (SpecialistTypes)GC.getBuildingInfo(eBuilding)->GetSpecialistType();
					CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
					if (pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_GOLD) > 0)
					{
						iWeight *= 150;
						iWeight /= 100;
						break;
					}
				}
			}
		}
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_FOOD)
	{
		iWeight *= 50;
		iWeight /= 100;
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
	{
		// Loop through all Buildings
		for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (pkBuildingInfo)
			{
				// Have this Building in the City?
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if (IsCanAddSpecialistToBuilding(eBuilding))
					{
						SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if (pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_PRODUCTION) > 0)
						{
							iWeight *= 150;
							iWeight /= 100;
							break;
						}
					}
				}
			}
		}
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
	{
		// Loop through all Buildings
		for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (pkBuildingInfo)
			{
				// Have this Building in the City?
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if (IsCanAddSpecialistToBuilding(eBuilding))
					{
						SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if (pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_GOLD) > 0)
						{
							iWeight *= 150;
							iWeight /= 100;
						}

						if (GetPlayer()->getSpecialistExtraYield(YIELD_GOLD) > 0)
						{
							iWeight *= 2;
						}

						if (GetPlayer()->GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, YIELD_GOLD) > 0)
						{
							iWeight *= 2;
						}
					}
				}
			}
		}
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_FAITH)
	{
		// Loop through all Buildings
		for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = (BuildingTypes)iBuildingLoop;
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if (pkBuildingInfo)
			{
				// Have this Building in the City?
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if (IsCanAddSpecialistToBuilding(eBuilding))
					{
						const SpecialistTypes eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if (pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_FAITH) > 0)
						{
							iWeight *= 3;
							break;
						}
					}
				}
			}
		}
	}

	// specialists are cheaper somehow
	if (m_pCity->GetPlayer()->isHalfSpecialistUnhappiness() || m_pCity->GetPlayer()->isHalfSpecialistFood())
	{
		iWeight *= 150;
		iWeight /= 100;
	}
	else if (m_pCity->GetPlayer()->isHalfSpecialistFoodCapital() && m_pCity->isCapital())
	{
		iWeight *= 150;
		iWeight /= 100;
	}

	// Does the AI want it enough?
	if (iWeight >= 150)
	{
		return true;
	}

	return false;
}

/// What is the Building Type the AI likes the Specialist of most right now?
BuildingTypes CvCityCitizens::GetAIBestSpecialistBuilding(int& iSpecialistValue, std::map<SpecialistTypes, int>& specialistValueCache, bool bLogging)
{
	BuildingTypes eBestBuilding = NO_BUILDING;
	int iBestSpecialistValue = -1;
	int iBestUnmodifiedSpecialistValue = -1;

	SpecialistTypes eSpecialist;
	int iValue;

	if (m_pCity->GetResistanceTurns() > 0)
		return NO_BUILDING;

	if (!CanCreateSpecialist())
		return NO_BUILDING;

	int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption(false, 1) * 100);

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
					eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();

					std::map<SpecialistTypes, int>::iterator it = specialistValueCache.find(eSpecialist);
					if (it != specialistValueCache.end())
						iValue = specialistValueCache[eSpecialist];
					else
					{
						iValue = GetSpecialistValue(eSpecialist, iExcessFoodTimes100);
						specialistValueCache[eSpecialist] = iValue;
					}

					// Add a bit more weight to a Building if it has more slots (10% per).  This will bias the AI to fill a single building over spreading Specialists out
					int iTemp = ((GetNumSpecialistsAllowedByBuilding(*pkBuildingInfo) - 1) * iValue * 15);
					iTemp /= 100;
					iValue += iTemp;

					if (bLogging && (GC.getLogging() && GC.getAILogging()))
					{
						CvString playerName;
						FILogFile* pLog;
						CvString strBaseString;
						CvString strOutBuf;
						CvString strFileName = "CityTileScorer.csv";
						playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
						pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
						strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
						strBaseString += playerName + ", ";
						strOutBuf.Format("%s: Specialist Building: %s, Score: %d", m_pCity->getName().GetCString(), pkBuildingInfo->GetDescription(), iValue);
						strBaseString += strOutBuf;
						pLog->Msg(strBaseString);
					}

					if (iValue > iBestSpecialistValue)
					{
						eBestBuilding = eBuilding;
						iBestSpecialistValue = iValue;
						iBestUnmodifiedSpecialistValue = iValue - iTemp;
					}
				}
			}
		}
	}

	iSpecialistValue = iBestUnmodifiedSpecialistValue;
	return eBestBuilding;
}

/// What is the Building Type the AI likes the Specialist of most right now?
BuildingTypes CvCityCitizens::GetAIBestSpecialistCurrentlyInBuilding(int& iSpecialistValue, std::map<SpecialistTypes, int>& specialistValueCache)
{
	BuildingTypes eBestBuilding = NO_BUILDING;
	int iBestSpecialistValue = -1;
	int iBestUnmodifiedSpecialistValue = -1;

	SpecialistTypes eSpecialist;
	int iValue;

	int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption(false, 1) * 100);

	// Loop through all Buildings
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if (pkBuildingInfo)
		{
			// Have this Building in the City, and there's a specialist in it?
			if (GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0 && GetCity()->GetCityCitizens()->GetNumSpecialistsInBuilding(eBuilding) > 0)
			{
				eSpecialist = (SpecialistTypes)pkBuildingInfo->GetSpecialistType();

				std::map<SpecialistTypes, int>::iterator it = specialistValueCache.find(eSpecialist);
				if (it != specialistValueCache.end())
					iValue = specialistValueCache[eSpecialist];
				else
				{
					iValue = GetSpecialistValue(eSpecialist, iExcessFoodTimes100);
					specialistValueCache[eSpecialist] = iValue;
				}

				// Add a bit more weight to a Building if it has more slots (10% per).  This will bias the AI to fill a single building over spreading Specialists out
				int iTemp = ((GetNumSpecialistsAllowedByBuilding(*pkBuildingInfo) - 1) * iValue * 15);
				iTemp /= 100;
				iValue += iTemp;

				if (iValue > iBestSpecialistValue)
				{
					eBestBuilding = eBuilding;
					iBestSpecialistValue = iValue;
					iBestUnmodifiedSpecialistValue = iValue - iTemp;
				}
			}
		}
	}

	iSpecialistValue = iBestUnmodifiedSpecialistValue;
	return eBestBuilding;
}

/// How valuable is eSpecialist?
int CvCityCitizens::GetSpecialistValue(SpecialistTypes eSpecialist, int iExcessFoodTimes100)
{
	if (iExcessFoodTimes100 <= 0)
		return 0;

	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if (pSpecialistInfo == NULL)
		return 0;

	CvPlayer* pPlayer = m_pCity->GetPlayer();

	//defaults
	int iFlavorGold = 7;
	int iFlavorScience = 7;
	int iFlavorCulture = 7;
	int iFlavorProduction = 7;
	int iFlavorFaith = 7;
	int iFlavorHappiness = 7;
	int iFlavorGrowth = 7;
	int iFlavorDiplomacy = 7;

	if (!pPlayer->isHuman())
	{
		iFlavorGold = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
		iFlavorScience = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
		iFlavorCulture = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
		iFlavorProduction = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION"));
		iFlavorFaith = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_RELIGION"));
		iFlavorHappiness = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_HAPPINESS"));
		iFlavorGrowth = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GROWTH"));
		iFlavorDiplomacy = pPlayer->GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
	}

	// factor in the fact that specialists may need less food
	int iFoodConsumptionBonus = (pPlayer->isHalfSpecialistFood()) ? 1 : 0;
	if (iFoodConsumptionBonus == 0 && m_pCity->isCapital())
		iFoodConsumptionBonus = (pPlayer->isHalfSpecialistFoodCapital()) ? 1 : 0;

	///////
	// Bonuses
	//////////
	ProcessTypes eProcess = m_pCity->getProductionProcess();
	const CvProcessInfo* pkProcessInfo = GC.getProcessInfo(eProcess);

	int iYieldValue = 0;
	CityAIFocusTypes eFocus = GetFocusType();
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;

		//Simplification - errata yields not worth considering.
		if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			break;

		int iYield100 = pPlayer->specialistYield(eSpecialist, eYield) * 100;
		int iYieldMod = 0;
		//Culture is treated differently, sadly.
		if (eYield == YIELD_CULTURE)
			iYield100 = m_pCity->GetCultureFromSpecialist(eSpecialist) * 100;

		if (iYield100 > 0)
		{
			//conversion to other yields
			if (pkProcessInfo && pkProcessInfo->getProductionToYieldModifier(eYield) > 0)
				iYield100 += (iYield100 * pkProcessInfo->getProductionToYieldModifier(eYield))/100;

			//religion bonus
			ReligionTypes eMajority = m_pCity->GetCityReligions()->GetReligiousMajority();
			if (eMajority >= RELIGION_PANTHEON)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, m_pCity->getOwner());
				if (pReligion)
				{
					iYield100 += pReligion->m_Beliefs.GetSpecialistYieldChange(eSpecialist, eYield, m_pCity->getOwner(), m_pCity)*100;
					int iYield1Specialist = pReligion->m_Beliefs.GetYieldChangeAnySpecialist(eYield, m_pCity->getOwner(), m_pCity);
					if (GetTotalSpecialistCount() <= 0 && iYield1Specialist > 0)
						iYield100 += iYield1Specialist*100;
				}
			}
			if (m_pCity->GetCityStrategyAI()->GetMostDeficientYield() == eYield)
			{
				iYieldMod += 5;
			}
			else if (m_pCity->GetCityStrategyAI()->IsYieldDeficient(eYield))
			{
				iYieldMod += 5;
			}

			if (eYield == YIELD_FOOD)
			{
				if (eFocus == CITY_AI_FOCUS_TYPE_FOOD)
				{
					iYieldMod += max(1, (GC.getAI_CITIZEN_VALUE_FOOD() - iFoodConsumptionBonus));
				}
				else if (eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
				{
					iYieldMod += max(1, (GC.getAI_CITIZEN_VALUE_FOOD() - iFoodConsumptionBonus));
				}
				else if (eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
				{
					iYieldMod += max(1, (GC.getAI_CITIZEN_VALUE_FOOD() - iFoodConsumptionBonus));
				}
			}
			else if (eYield == YIELD_PRODUCTION)
			{
				if (eFocus == CITY_AI_FOCUS_TYPE_PRODUCTION)
				{
					iYieldMod += GC.getAI_CITIZEN_VALUE_PRODUCTION();
				}
				else if (eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
				{
					iYieldMod += GC.getAI_CITIZEN_VALUE_PRODUCTION()/2;
				}
			}
			else if (eYield == YIELD_GOLD)
			{
				if (eFocus == CITY_AI_FOCUS_TYPE_GOLD)
				{
					iYieldMod += GC.getAI_CITIZEN_VALUE_GOLD();
				}
				if (eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
				{
					iYieldMod += GC.getAI_CITIZEN_VALUE_GOLD()/2;
				}
			}
			else if (eYield == YIELD_SCIENCE)
			{
				if (eFocus == CITY_AI_FOCUS_TYPE_SCIENCE)
				{
					iYieldMod += GC.getAI_CITIZEN_VALUE_SCIENCE();
				}
			}
			else if (eYield == YIELD_CULTURE || eYield == YIELD_TOURISM)
			{
				if (eFocus == CITY_AI_FOCUS_TYPE_CULTURE)
				{
					iYieldMod += GC.getAI_CITIZEN_VALUE_CULTURE();
				}
			}
			else if (eYield == YIELD_FAITH || eYield == YIELD_GOLDEN_AGE_POINTS)
			{
				if (eFocus == CITY_AI_FOCUS_TYPE_FAITH)
				{
					iYieldMod += GC.getAI_CITIZEN_VALUE_FAITH();
				}
			}

			if (iYield100 > 0)
			{
				switch (eYield)
				{
				case YIELD_GOLD:
					if (m_pCity->getUnhappinessFromGold() > 0)
					{
						iYieldMod += m_pCity->getUnhappinessFromGold();
					}
					break;
				case YIELD_SCIENCE:
					if (m_pCity->getUnhappinessFromScience() > 0)
					{
						iYieldMod += m_pCity->getUnhappinessFromScience();
					}
					break;
				case YIELD_CULTURE:
					if (m_pCity->getUnhappinessFromCulture() > 0)
					{
						iYieldMod += m_pCity->getUnhappinessFromCulture();
					}
					break;
				case YIELD_FAITH:
					if (m_pCity->getUnhappinessFromReligion() > 0)
					{
						iYieldMod += m_pCity->getUnhappinessFromReligion();
					}
					break;
				case YIELD_PRODUCTION:
				case YIELD_FOOD:
					if (m_pCity->getUnhappinessFromDefense() > 0)
					{
						iYieldMod += m_pCity->getUnhappinessFromDefense();
					}
					break;
				}
			}

			iYieldValue += iYield100*iYieldMod;
		}
	}

	int iGPPYieldValue = pSpecialistInfo->getGreatPeopleRateChange();
	for (int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		int iSpecialistFlavor = pSpecialistInfo->getFlavorValue((FlavorTypes)i);
		if (iSpecialistFlavor > 0)
		{
			if (GC.getFlavorTypes((FlavorTypes)i) == "FLAVOR_GOLD")
			{
				iGPPYieldValue += (iSpecialistFlavor * iFlavorGold);
			}
			else if (GC.getFlavorTypes((FlavorTypes)i) == "FLAVOR_PRODUCTION")
			{
				iGPPYieldValue += iSpecialistFlavor * iFlavorProduction;
			}
			else if (GC.getFlavorTypes((FlavorTypes)i) == "FLAVOR_RELIGION")
			{
				iGPPYieldValue += iSpecialistFlavor * iFlavorFaith;
			}
			else if (GC.getFlavorTypes((FlavorTypes)i) == "FLAVOR_CULTURE")
			{
				iGPPYieldValue += iSpecialistFlavor * iFlavorCulture;
			}
			else if (GC.getFlavorTypes((FlavorTypes)i) == "FLAVOR_GROWTH")
			{
				iGPPYieldValue += iSpecialistFlavor * iFlavorGrowth;
			}
			else if (GC.getFlavorTypes((FlavorTypes)i) == "FLAVOR_SCIENCE")
			{
				iGPPYieldValue += iSpecialistFlavor * iFlavorScience;
			}
			else if (GC.getFlavorTypes((FlavorTypes)i) == "FLAVOR_HAPPINESS")
			{
				iGPPYieldValue += iSpecialistFlavor * iFlavorHappiness;
			}
			else if (GC.getFlavorTypes((FlavorTypes)i) == "FLAVOR_DIPLOMACY")
			{
				iGPPYieldValue += iSpecialistFlavor * iFlavorDiplomacy;
			}
			else
			{
				iGPPYieldValue += iSpecialistFlavor /= 2;
			}
		}
	}

	AICityStrategyTypes eGoodGP = (AICityStrategyTypes)GC.getInfoTypeForString("AICITYSTRATEGY_GOOD_GP_CITY");
	if (eGoodGP != NO_AICITYSTRATEGY && m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eGoodGP))
		iGPPYieldValue *= 2;

	if (eFocus == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE)
		iGPPYieldValue /= 2;
	else
		iGPPYieldValue /= 10;

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
			iGPPYieldValue *= 2;
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
			iGPPYieldValue *= 2;
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
			iGPPYieldValue *= 2;
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
			iGPPYieldValue *= 2;
		}
	}
	else if ((UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
	{
		iMod += GetPlayer()->getGreatMerchantRateModifier();
		if (bWantDiplo)
		{
			iGPPYieldValue *= 2;
		}
	}
	else if ((UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
	{
		iMod += GetPlayer()->getGreatEngineerRateModifier();
		if (bWantScience || bWantArt)
		{
			iGPPYieldValue *= 2;
		}
	}
#if defined(MOD_DIPLOMACY_CITYSTATES)
	else if (MOD_DIPLOMACY_CITYSTATES && (UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
	{
		iMod += GetPlayer()->getGreatDiplomatRateModifier();
		if (bWantDiplo)
		{
			iGPPYieldValue *= 2;
		}
	}
#endif

#if defined(MOD_API_LUA_EXTENSIONS)
	GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
	if (eGreatPerson != NO_GREATPERSON)
	{
		iMod += GetPlayer()->getSpecificGreatPersonRateModifierFromMonopoly(eGreatPerson);
		if (GetPlayer()->isGoldenAge())
		{
			iMod += GetPlayer()->getGoldenAgeGreatPersonRateModifier(eGreatPerson);
			iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);

			ReligionTypes eMajority = GetCity()->GetCityReligions()->GetReligiousMajority();
			if (eMajority != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, m_pCity->getOwner());
				if (pReligion)
				{
					iMod += pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGreatPerson, m_pCity->getOwner(), m_pCity, true);
					BeliefTypes eSecondaryPantheon = GetCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief();
					if (eSecondaryPantheon != NO_BELIEF)
					{
						iMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
					}
				}
			}
		}
		int iNumPuppets = GetPlayer()->GetNumPuppetCities();
		if (iNumPuppets > 0)
		{
			iMod += (iNumPuppets * GetPlayer()->GetPlayerTraits()->GetPerPuppetGreatPersonRateModifier(eGreatPerson));
		}
	}
#endif
	if (GetCity()->isCapital() && GetPlayer()->IsDiplomaticMarriage())
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
			iMod += (iNumMarried * GC.getBALANCE_MARRIAGE_GP_RATE());
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
				iGPPYieldValue /= 3;
			}
			else
			{
				iGPPYieldValue += (iEmptySlots * 2);
			}
		}
		else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER"))
		{
			int iEmptySlots = GET_PLAYER(m_pCity->getOwner()).GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_LITERATURE());
			if (iEmptySlots == 0)
			{
				iGPPYieldValue /= 3;
			}
			else
			{
				iGPPYieldValue += (iEmptySlots * 2);
			}
		}
		else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
		{
			int iEmptySlots = GET_PLAYER(m_pCity->getOwner()).GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_MUSIC());
			if (iEmptySlots == 0)
			{
				iGPPYieldValue /= 3;
			}
			else
			{
				iGPPYieldValue += (iEmptySlots * 2);
			}
			if (bWantArt)
			{
				iGPPYieldValue *= 2;
			}
		}
	}

	int iValue = iYieldValue + iGPPYieldValue;

	//Let's see how close we are to a specialist. If close, emphasize.
	int iProximityToGPBonus = 0;
	int iGPWeHave = GetSpecialistGreatPersonProgress(eSpecialist);
	if (iGPWeHave != 0)
	{
		int iGPNeededForNextGP = GetSpecialistUpgradeThreshold((UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass());
		if (iGPNeededForNextGP != 0)
		{
			iGPWeHave *= 100;
			iProximityToGPBonus = iGPWeHave / iGPNeededForNextGP;
		}
	}

	iValue *= (100 + iMod + iProximityToGPBonus);
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
			iCityUnhappiness = 15 - iCityUnhappiness;
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
		iValue /= 10;
	else if (eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH || eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
		iValue /= 4;
	else if ((eFocus == NO_CITY_AI_FOCUS_TYPE))
		iValue /= 2;
	else if (eFocus == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE)
		iValue *= 2;

	return iValue;
}

/// Determine if eSpecialist is preferable to a default specialist, based on our focus
bool CvCityCitizens::IsBetterThanDefaultSpecialist(SpecialistTypes eSpecialist)
{
	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	CvAssertMsg(pSpecialistInfo, "Invalid specialist type when assigning citizens. Please send Anton your save file and version.");
	if (!pSpecialistInfo) return false; // Assumes that default specialist will work out

	SpecialistTypes eDefaultSpecialist = (SpecialistTypes)GC.getDEFAULT_SPECIALIST();
	CvSpecialistInfo* pDefaultSpecialistInfo = GC.getSpecialistInfo(eDefaultSpecialist);
	CvAssertMsg(pDefaultSpecialistInfo, "Invalid default specialist type when assigning citizens. Please send Anton your save file and version.");
	if (!pDefaultSpecialistInfo) return false;

	//antonjs: consider: deficient yield

	CityAIFocusTypes eFocus = GetFocusType();
	YieldTypes eYield = NO_YIELD;
	switch (eFocus)
	{
	case CITY_AI_FOCUS_TYPE_FOOD:
		eYield = YIELD_FOOD;
		break;
	case CITY_AI_FOCUS_TYPE_PRODUCTION:
		eYield = YIELD_PRODUCTION;
		break;
	case CITY_AI_FOCUS_TYPE_GOLD:
		eYield = YIELD_GOLD;
		break;
	case CITY_AI_FOCUS_TYPE_GREAT_PEOPLE:
		eYield = NO_YIELD;
		break;
	case CITY_AI_FOCUS_TYPE_SCIENCE:
		eYield = YIELD_SCIENCE;
		break;
	case CITY_AI_FOCUS_TYPE_CULTURE:
		eYield = YIELD_CULTURE;
		break;
	case CITY_AI_FOCUS_TYPE_PROD_GROWTH:
	case CITY_AI_FOCUS_TYPE_GOLD_GROWTH:
		eYield = YIELD_FOOD;
		break;
	case CITY_AI_FOCUS_TYPE_FAITH:
		eYield = YIELD_FAITH;
		break;
	default:
		eYield = NO_YIELD;
		break;
	}

	if (eYield == NO_YIELD)
		return true;

	int iSpecialistYield = pSpecialistInfo->getYieldChange(eYield);
	int iDefaultSpecialistYield = pDefaultSpecialistInfo->getYieldChange(eYield);

	if (m_pCity->GetPlayer()->isHalfSpecialistUnhappiness() || m_pCity->GetPlayer()->isHalfSpecialistFood())
	{
		iSpecialistYield *= 2;
	}
	else if (m_pCity->isCapital() && m_pCity->GetPlayer()->isHalfSpecialistFoodCapital())
	{
		iSpecialistYield *= 2;
	}

	return (iSpecialistYield >= iDefaultSpecialistYield); // Unless default Specialist has strictly more, this Specialist is better
}

bool CvCityCitizens::CanCreateSpecialist()
{
	return true;

	/*
	if (!MOD_BALANCE_CORE_HAPPINESS)
		return true;

	int iHappinessDelta = m_pCity->getHappinessDelta();
	if (iHappinessDelta > 0)
		return true;

	int iCapital = m_pCity->isCapital() ? GET_PLAYER(m_pCity->getOwner()).GetNoUnhappfromXSpecialistsCapital() : 0;
	int iFreeSpecialistsRemaining = (m_pCity->GetNoUnhappfromXSpecialists() + GET_PLAYER(m_pCity->getOwner()).GetNoUnhappfromXSpecialists() + iCapital) - GetTotalSpecialistCount();
	if (iFreeSpecialistsRemaining > 0)
		return true;

	return false;
	*/
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

bool CvCityCitizens::DoAddBestCitizenFromUnassigned()
{
	std::map<SpecialistTypes, int> specialistValueCache;
	return DoAddBestCitizenFromUnassignedEx(specialistValueCache, false, false, true);
}

/// Pick the best Plot to work from one of our unassigned pool
bool CvCityCitizens::DoAddBestCitizenFromUnassignedEx(std::map<SpecialistTypes, int>& specialistValueCache, bool bLogging, bool bNoSpecialists, bool bUpdateNow)
{
	// We only assign the unassigned here, folks
	if (GetNumUnassignedCitizens() == 0)
		return false;

	//FOUR STAGE SETUP:
	bool bAvoidGrowth = IsAvoidGrowth();
	//FIRST, WE FEED OURSELVES!
	int iPotentialExcessTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption(false, 1) * 100);
	if ((!bAvoidGrowth && iPotentialExcessTimes100 <= 300) || (bAvoidGrowth && iPotentialExcessTimes100 <= 0))
	{
		int iBestPlotValue = 0;
		CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, /*bBest*/ true, /*bWorked*/ false, false, bLogging);
		if (pBestPlot != NULL)
		{
			// Now assign the guy to the best possible Plot
			SetWorkingPlot(pBestPlot, true);
			return true;
		}
	}
	//SECOND, WE CHECK SPECIALISTS!
	else
	{
		int iSpecialistValue = 0;
		BuildingTypes eBestSpecialistBuilding = NO_BUILDING;
		if (!bNoSpecialists && (!GET_PLAYER(GetOwner()).isHuman() || !IsNoAutoAssignSpecialists()))
		{
			eBestSpecialistBuilding = GetAIBestSpecialistBuilding(iSpecialistValue, specialistValueCache, bLogging);
		}

		int iBestPlotValue = 0;
		//GOOD PLOT? BAD PLOT? NYEH!
		CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, /*bBest*/ true, /*bWorked*/ false, false, bLogging);
		if (pBestPlot != NULL)
		{
			bool bSpecialistBetterThanPlot = (eBestSpecialistBuilding != NO_BUILDING) && ((iSpecialistValue >= iBestPlotValue) && ((iPotentialExcessTimes100 > 0) || (bAvoidGrowth && iPotentialExcessTimes100 >= 0)));
			if (bSpecialistBetterThanPlot)
			{
				DoAddSpecialistToBuilding(eBestSpecialistBuilding, /*bForced*/ false, bUpdateNow);
				return true;
			}
			else
			{
				SetWorkingPlot(pBestPlot, true);
				return true;
			}
		}
		//THIRD, IF WE HAVE NO MORE GOOD PLOTS, WE DUMP EVERYTHING ELSE INTO SPECIALISTS UNLESS IT WOULD STARVE US
		else if (eBestSpecialistBuilding != NO_BUILDING && iPotentialExcessTimes100 >= 0)
		{
			DoAddSpecialistToBuilding(eBestSpecialistBuilding, /*bForced*/ false, bUpdateNow);
			return true;
		}
		//FOURTH, MANUAL LABOR FOREVER
		else
		{
			CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, /*bBest*/ true, /*bWorked*/ false, false, bLogging);
			if (pBestPlot != NULL)
			{
				SetWorkingPlot(pBestPlot, true);
				return true;
			}
			else
			{
				// Default Specialist if we can't do anything else
				ChangeNumDefaultSpecialists(1);
			}
		}
	}

	return false;
}

/// Pick the worst Plot to stop working
bool CvCityCitizens::DoRemoveWorstCitizen(bool bRemoveForcedStatus, SpecialistTypes eDontChangeSpecialist, int iCurrentCityPopulation, bool bUpdateNow)
{
	if (iCurrentCityPopulation == -1)
	{
#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
		iCurrentCityPopulation = GetCity()->getPopulation(true);
#else
		iCurrentCityPopulation = GetCity()->getPopulation();
#endif
	}

	// Are all of our guys already not working Plots?
#if defined(MOD_GLOBAL_CITY_AUTOMATON_WORKERS)
	if(GetNumUnassignedCitizens() == GetCity()->getPopulation(true))
#else
	if(GetNumUnassignedCitizens() == GetCity()->getPopulation())
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
			ChangeNumDefaultSpecialists(-1);
			return true;
		}
		if (GetNumDefaultSpecialists() > iCurrentCityPopulation)
		{
			ChangeNumForcedDefaultSpecialists(-1);
			ChangeNumDefaultSpecialists(-1);
			return true;
		}
	}

	// No Default Specialists, remove a working Pop, if there is one
	int iWorstPlotValue = 0;
	CvPlot* pWorstPlot = GetBestCityPlotWithValue(iWorstPlotValue, /*bBest*/ false, /*bWorked*/ true, bRemoveForcedStatus);

	if (pWorstPlot != NULL)
	{
		// If we were force-working this Plot, turn it off
		if (bRemoveForcedStatus && IsForcedWorkingPlot(pWorstPlot))
		{
			SetWorkingPlot(pWorstPlot, false, true, bUpdateNow);
			SetForcedWorkingPlot(pWorstPlot, false);

			return true;
		}
		else if (!IsForcedWorkingPlot(pWorstPlot))
		{
			SetWorkingPlot(pWorstPlot, false, true, bUpdateNow);
			return true;
		}
	}
	// Have to resort to pulling away a good Specialist
	else if (bRemoveForcedStatus)
	{
		if (DoRemoveWorstSpecialist(eDontChangeSpecialist, NO_BUILDING, bUpdateNow))
		{
			return true;
		}
	}

	return false;
}

/// Find a Plot the City is either working or not, and the best/worst value for it - this function does "double duty" depending on what the user wants to find
CvPlot* CvCityCitizens::GetBestCityPlotWithValue(int& iValue, bool bWantBest, bool bWantWorked, bool bForced, bool bLogging)
{
	bool bPlotForceWorked;

	int iBestPlotValue = -1;
	int iBestPlotID = -1;

	SPrecomputedExpensiveNumbers store(m_pCity);

	// Look at all workable Plots
	for (int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
	{
		if (iPlotLoop != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

			if (pLoopPlot != NULL)
			{
				// Is this a Plot this City controls?
				if (pLoopPlot->getOwningCityID() == GetCity()->GetID())
				{
					// Working the Plot and wanting to work it, or Not working it and wanting to find one to work?
					if ((IsWorkingPlot(iPlotLoop) && bWantWorked) ||
						(!IsWorkingPlot(iPlotLoop) && !bWantWorked))
					{
						// Working the Plot or CAN work the Plot?
						if (bWantWorked || IsCanWork(pLoopPlot))
						{
							iValue = GetPlotValue(pLoopPlot, store);

							if (bLogging && (GC.getLogging() && GC.getAILogging()))
							{
								CvString playerName;
								FILogFile* pLog;
								CvString strBaseString;
								CvString strOutBuf;
								CvString strFileName = "CityTileScorer.csv";
								playerName = GET_PLAYER(m_pCity->getOwner()).getCivilizationShortDescription();
								pLog = LOGFILEMGR.GetLog(strFileName, FILogFile::kDontTimeStamp);
								strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
								strBaseString += playerName + ", ";
								strOutBuf.Format("%s: Plot %d, Score: %d", m_pCity->getName().GetCString(), pLoopPlot->GetPlotIndex(), iValue);
								strBaseString += strOutBuf;
								pLog->Msg(strBaseString);
							}

							bPlotForceWorked = IsForcedWorkingPlot(pLoopPlot);

							if (bPlotForceWorked)
							{
								// Looking for best, unworked Plot: Forced plots are FIRST to be picked
								if (bWantBest && !bWantWorked)
								{
									iValue += 1000*1000;
								}
								// Looking for worst, worked Plot: Forced plots are LAST to be picked, so make it's value incredibly high
								if (!bWantBest && bWantWorked)
								{
									if (!bForced)
									{
										continue;
									}
									else
									{
										iValue += 1000*1000;
									}
								}
							}

							if (iBestPlotID == -1 ||							// First Plot?
								(bWantBest && iValue > iBestPlotValue) ||		// Best Plot so far?
								(!bWantBest && iValue < iBestPlotValue))		// Worst Plot so far?
							{
								iBestPlotValue = iValue;
								iBestPlotID = iPlotLoop;
							}
						}
					}
				}
			}
		}
	}

	// Passed in by reference
	iValue = iBestPlotValue;

	if (iBestPlotID == -1)
	{
		return NULL;
	}

	return GetCityPlotFromIndex(iBestPlotID);
}

bool CvCityCitizens::NeedReworkCitizens()
{
	if (IsDirty())
	{
		SetDirty(false);
		return true;
	}

	int iWorstWorkedPlotValue = 0;
	CvPlot* pWorstWorkedPlot = GetBestCityPlotWithValue(iWorstWorkedPlotValue, /*bBest*/ false, /*bWorked*/ true);

	int iBestUnworkedPlotValue = 0;
	CvPlot* pBestUnworkedPlot = GetBestCityPlotWithValue(iBestUnworkedPlotValue, /*bBest*/ true, /*bWorked*/ false);

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

	// don't calculate specialist value over and over ...
	std::map<SpecialistTypes, int> specialistValueCache;

	if (!GET_PLAYER(GetOwner()).isHuman() || !IsNoAutoAssignSpecialists())
	{
		eBestSpecialistBuilding = GetAIBestSpecialistBuilding(iSpecialistValue, specialistValueCache);
	}
	if (eBestSpecialistBuilding == NO_BUILDING)
	{
		return false;
	}

	bool bSpecialistBetterThanPlot = (eBestSpecialistBuilding != NO_BUILDING && iSpecialistValue >= iWorstWorkedPlotValue);
	if (bSpecialistBetterThanPlot)
	{
		return true;
	}

	int iSpecialistInCityValue = 0;
	BuildingTypes eBestSpecialistInCityBuilding = NO_BUILDING;
	if (!GET_PLAYER(GetOwner()).isHuman() || !IsNoAutoAssignSpecialists())
	{
		eBestSpecialistInCityBuilding = GetAIBestSpecialistCurrentlyInBuilding(iSpecialistInCityValue, specialistValueCache);
	}

	bool bSpecialistBetterThanExistingSpecialist = (eBestSpecialistInCityBuilding != NO_BUILDING && eBestSpecialistBuilding != NO_BUILDING && iSpecialistValue >= iSpecialistInCityValue);
	if (bSpecialistBetterThanExistingSpecialist)
	{
		return true;
	}
	return false;
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
void CvCityCitizens::SetBlockade(bool bValue)
{
	if (m_bIsBlockaded != bValue)
	{
		m_bIsBlockaded = bValue;
	}
}
bool CvCityCitizens::IsBlockade()
{
	return m_bIsBlockaded;
}

void CvCityCitizens::DoReallocateCitizens(bool bForce, bool bLogging)
{
	//Let's check if we need to do this.
	if (!DoValidateForcedWorkingPlots() && !bForce && !NeedReworkCitizens())
	{
		return;
	}

	// Remove all of the allocated guys
	int iNumCitizensToRemove = GetNumCitizensWorkingPlots();
	for (int iWorkerLoop = 0; iWorkerLoop < iNumCitizensToRemove; iWorkerLoop++)
	{
		DoRemoveWorstCitizen(false, NO_SPECIALIST, -1, false);
	}

	// Remove Non-Forced Specialists in Buildings
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		BuildingTypes eBuilding = (BuildingTypes)iBuildingLoop;

		// Have this Building in the City?
		if (GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		{
			int iNumSpecialistsToRemove = GetNumSpecialistsInBuilding(eBuilding) - GetNumForcedSpecialistsInBuilding(eBuilding);	// Don't include Forced guys

			// Loop through guys to remove (if there are any)
			for (int iSpecialistLoop = 0; iSpecialistLoop < iNumSpecialistsToRemove; iSpecialistLoop++)
			{
				DoRemoveSpecialistFromBuilding(eBuilding, /*bForced*/ false, false);
			}
		}
	}

	// Remove Default Specialists (Laborers)
	int iNumDefaultsToRemove = GetNumDefaultSpecialists() - GetNumForcedDefaultSpecialists();
	for (int iSpecialistLoop = 0; iSpecialistLoop < iNumDefaultsToRemove; iSpecialistLoop++)
	{
		ChangeNumDefaultSpecialists(-1, false);
	}

	//We deferred all recalculations to this spot for optimization purposes (doesn't matter if we're just nuking the entire city's population above).
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		if (eYield == NO_YIELD)
			continue;

		//Simplification - errata yields not worth considering.
		if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			break;

		m_pCity->UpdateCityYields(eYield);
		m_pCity->UpdateSpecialReligionYields(eYield);
	}
	m_pCity->GetCityCulture()->CalculateBaseTourismBeforeModifiers();
	m_pCity->GetCityCulture()->CalculateBaseTourism();

	// don't calculate specialist value over and over ...
	std::map<SpecialistTypes, int> specialistValueCache;

	// Now put all of the unallocated guys back
	int iNumToAllocate = GetNumUnassignedCitizens();
	for (int iUnallocatedLoop = 0; iUnallocatedLoop < iNumToAllocate; iUnallocatedLoop++)
	{
		DoAddBestCitizenFromUnassignedEx(specialistValueCache, bLogging);
	}

	//And then we do it again after everyone is allocated.
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		if (eYield == NO_YIELD)
			continue;

		//Simplification - errata yields not worth considering.
		if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
			break;

		GetCity()->UpdateCityYields(eYield);
		GetCity()->UpdateSpecialReligionYields(eYield);
	}
	GetCity()->GetCityCulture()->CalculateBaseTourismBeforeModifiers();
	GetCity()->GetCityCulture()->CalculateBaseTourism();
	GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();
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
void CvCityCitizens::SetWorkingPlot(CvPlot* pPlot, bool bNewValue, bool bUseUnassignedPool, bool bUpdateNow)
{
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

				if (bUseUnassignedPool)
				{
					ChangeNumUnassignedCitizens(-1);
				}
			}
			else
			{
				ChangeNumCitizensWorkingPlots(-1);

				if (bUseUnassignedPool)
				{
					ChangeNumUnassignedCitizens(1);
				}
			}
		}

		if (pPlot != NULL)
		{
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
		}

		//Let's only worry about yield changes if we need to update, or we're human (humans want instant feedback, whereas the AI doesn't always need that).
		if (bUpdateNow)
		{
			if (iIndex != CITY_HOME_PLOT)
			{
				for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					//Simplification - errata yields not worth considering.
					if ((YieldTypes)iI > YIELD_GOLDEN_AGE_POINTS && !MOD_BALANCE_CORE_JFD)
						break;

					YieldTypes eYield = (YieldTypes)iI;
					if (eYield == NO_YIELD)
						continue;

					GetCity()->UpdateCityYields(eYield);
					GetCity()->UpdateSpecialReligionYields(eYield);
				}
				GetCity()->GetCityCulture()->CalculateBaseTourismBeforeModifiers();
				GetCity()->GetCityCulture()->CalculateBaseTourism();
			}
		}

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
	CvAssertMsg(iIndex >= 0, "iIndex expected to be >= 0");

	CvAssertMsg(iIndex < GetCity()->GetNumWorkablePlots(), "iIndex expected to be < NUM_CITY_PLOTS");
	// Clicking ON the city "resets" it to default setup
	if (iIndex == CITY_HOME_PLOT)
	{
		CvPlot* pLoopPlot;

		// If we've forced any plots to be worked, reset them to the normal state

		for (int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
		{
			if (iPlotLoop != CITY_HOME_PLOT)
			{
				pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

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
		CvPlot* pPlot = GetCityPlotFromIndex(iIndex);

		if (pPlot != NULL)
		{
			if (IsCanWork(pPlot))
			{
				// If we're already working the Plot, then take the guy off and turn him into a Default Specialist
				if (IsWorkingPlot(pPlot))
				{
					SetWorkingPlot(pPlot, false);
					SetForcedWorkingPlot(pPlot, false);
					ChangeNumDefaultSpecialists(1);
					ChangeNumForcedDefaultSpecialists(1);
				}
				// Player picked a new Plot to work
				else
				{
					// Pull from the Default Specialist pool, if possible
					if (GetNumDefaultSpecialists() > 0)
					{
						ChangeNumDefaultSpecialists(-1);
						// Player is forcibly telling city to work a plot, so reduce count of forced default specialists
						if (GetNumForcedDefaultSpecialists() > 0)
							ChangeNumForcedDefaultSpecialists(-1);

						SetWorkingPlot(pPlot, true);
						SetForcedWorkingPlot(pPlot, true);
						
					}
					// No Default Specialists, so grab a better allocated guy
					else
					{
						// Working Plot
						if (DoRemoveWorstCitizen(true))
						{
							SetWorkingPlot(pPlot, true);
							SetForcedWorkingPlot(pPlot, true);
							//ChangeNumUnassignedCitizens(-1);
						}

						// Good Specialist
						else
						{
							CvAssert(false);
						}
					}
				}
			}
			// JON: Need to update this block to work with new system
			else if (pPlot->getOwner() == GetOwner())
			{
				// Can't take away forced plots from puppet Cities
				if (pPlot->getOwningCityOverride() != NULL)
				{
					if (pPlot->getOwningCityOverride()->IsPuppet())
					{
						return;
					}
				}

				pPlot->setOwningCityOverride(GetCity());
			}
		}
	}
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
			if (GetNumForcedWorkingPlots() > GetNumCitizensWorkingPlots())
			{
				DoValidateForcedWorkingPlots();
			}
		}
		else
		{
			ChangeNumForcedWorkingPlots(-1);
		}
	}
}

/// Make sure we don't have more forced working plots than we have citizens to work
bool CvCityCitizens::DoValidateForcedWorkingPlots()
{
	bool bValue = false;
	int iNumForcedWorkingPlotsToDemote = GetNumForcedWorkingPlots() - GetNumCitizensWorkingPlots();

	if (iNumForcedWorkingPlotsToDemote > 0)
	{
		for (int iLoop = 0; iLoop < iNumForcedWorkingPlotsToDemote; iLoop++)
		{
			bValue = DoDemoteWorstForcedWorkingPlot();

		}
	}

	return bValue;
}

/// Remove the Forced status from the worst ForcedWorking plot
bool CvCityCitizens::DoDemoteWorstForcedWorkingPlot()
{
	int iBestPlotValue = -1;
	int iBestPlotID = -1;

	SPrecomputedExpensiveNumbers store(m_pCity);

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
					int iValue = GetPlotValue(pLoopPlot, store);

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

/// Can our City work a particular CvPlot?
bool CvCityCitizens::IsCanWork(CvPlot* pPlot) const
{
	if (pPlot->getOwningCityID() != m_pCity->GetID())
	{
		return false;
	}

	CvAssertMsg(GetCityIndexFromPlot(pPlot) != -1, "GetCityIndexFromPlot(pPlot) is expected to be assigned (not -1)");

	if (pPlot->plotCheck(PUF_canSiege, GetOwner()) != NULL)
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

	if (!pPlot->hasYield())
	{
		return false;
	}

	if (pPlot->isBlockaded(GetOwner()))
	{
		return false;
	}

	return true;
}

// Is there a naval blockade on any of this city's water tiles?
bool CvCityCitizens::IsAnyPlotBlockaded() const
{
	CvPlot* pLoopPlot;

	// Look at all workable Plots

	for (int iPlotLoop = 0; iPlotLoop < m_pCity->GetNumWorkablePlots(); iPlotLoop++)
	{
		if (iPlotLoop != CITY_HOME_PLOT)
		{
			pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isBlockaded(GetOwner()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// Check all Plots by this City to see if we can actually be working them (if we are)
void CvCityCitizens::DoVerifyWorkingPlots()
{
	for (int iI = 0; iI < GetCity()->GetNumWorkablePlots(); iI++)
	{
		CvPlot* pPlot = GetCityPlotFromIndex(iI);

		if (pPlot && IsWorkingPlot(iI))
		{
			if (!IsCanWork(pPlot))
			{
				SetWorkingPlot(pPlot, false, true, false);
				DoAddBestCitizenFromUnassigned();
			}
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
	if (m_pCity->IsPuppet() && MOD_BALANCE_CORE_PUPPET_CHANGES && !GET_PLAYER(m_pCity->getOwner()).GetPlayerTraits()->IsNoAnnexing())
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

			ReligionTypes eMajority = m_pCity->GetCityReligions()->GetReligiousMajority();
			if (eMajority != NO_RELIGION)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, m_pCity->getOwner());
				if (pReligion)
				{
					iGPPChange += pReligion->m_Beliefs.GetGreatPersonPoints(GetGreatPersonFromSpecialist(eSpecialist), m_pCity->getOwner(), m_pCity, true) * 100;
				}
			}

#if defined(MOD_BALANCE_CORE_RESOURCE_MONOPOLIES) && defined(MOD_API_LUA_EXTENSIONS)
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

						ReligionTypes eMajority = GetCity()->GetCityReligions()->GetReligiousMajority();
						if (eMajority != NO_RELIGION)
						{
							const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, GetCity()->getOwner());
							if (pReligion)
							{
								iMod += pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGreatPerson, GetCity()->getOwner(), m_pCity);
								BeliefTypes eSecondaryPantheon = GetCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief();
								if (eSecondaryPantheon != NO_BELIEF)
								{
									iMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
								}
							}
						}
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
				if (GetCity()->isCapital() && GetPlayer()->IsDiplomaticMarriage())
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
						iMod += (iNumMarried * GC.getBALANCE_MARRIAGE_GP_RATE());
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
					const CivilizationTypes eCivilization = GetCity()->getCivilizationType();
					CvCivilizationInfo* pCivilizationInfo = GC.getCivilizationInfo(eCivilization);
					if (pCivilizationInfo != NULL)
					{
						UnitTypes eUnit = (UnitTypes)pCivilizationInfo->getCivilizationUnits(eUnitClass);

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
	if (m_pCity->IsResistance() || m_pCity->IsRazing() || !CanCreateSpecialist())
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
		iNumSpecialistsAssigned < GC.getMAX_SPECIALISTS_FROM_BUILDING())	// Overall Limit
	{
		return true;
	}

	return false;
}

/// Adds and initializes a Specialist for this building
void CvCityCitizens::DoAddSpecialistToBuilding(BuildingTypes eBuilding, bool bForced, bool bUpdateNow)
{
	CvAssert(eBuilding > -1);
	CvAssert(eBuilding < GC.getNumBuildingInfos());

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
			DoRemoveWorstCitizen(true, /*Don't remove this type*/ eSpecialist, -1, false);

			if (GetNumUnassignedCitizens() == 0)
			{
				// Still nobody, all the citizens may be assigned to the eSpecialist we are looking for, try again
				if (!DoRemoveWorstSpecialist(NO_SPECIALIST, eBuilding, false))
				{
					return; // For some reason we can't do this, we must exit, else we will be going over the population count
				}
			}
		}

		// Increase count for the whole city
		m_aiSpecialistCounts[eSpecialist]++;
		m_aiNumSpecialistsInBuilding[eBuilding]++;

		if (bForced)
		{
			m_aiNumForcedSpecialistsInBuilding[eBuilding]++;
		}

		GetCity()->processSpecialist(eSpecialist, 1, bUpdateNow);
		ChangeNumUnassignedCitizens(-1);

		//we added the first specialist, this may have religious effects
		if (GetTotalSpecialistCount() == 1)
			GetCity()->UpdateReligion(GetCity()->GetCityReligions()->GetReligiousMajority(), false);

		if (bUpdateNow)
			GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();

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
void CvCityCitizens::DoRemoveSpecialistFromBuilding(BuildingTypes eBuilding, bool bForced, bool bEliminatePopulation, bool bUpdateNow)
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
			GetCity()->processSpecialist(eSpecialist, -1, true);
			GetCity()->UpdateReligion(GetCity()->GetCityReligions()->GetReligiousMajority(), false);
		}
		else
		{
			GetCity()->processSpecialist(eSpecialist, -1, bUpdateNow);
		}

		if (bUpdateNow)
			GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();

		// Do we kill this population or reassign him?
		if (bEliminatePopulation)
		{
			GetCity()->changePopulation(-1, /*bReassignPop*/ false);
		}
		else
		{
			ChangeNumUnassignedCitizens(1);
		}

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
void CvCityCitizens::DoRemoveAllSpecialistsFromBuilding(BuildingTypes eBuilding, bool bEliminatePopulation)
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
		GetCity()->processSpecialist(eSpecialist, -1);

		// Do we kill this population or reassign him?
		if (bEliminatePopulation)
		{
			GetCity()->changePopulation(-1, /*bReassignPop*/ false);
		}
		else
		{
			ChangeNumUnassignedCitizens(1);
		}

		GC.GetEngineUserInterface()->setDirty(CityInfo_DIRTY_BIT, true);
		//GC.GetEngineUserInterface()->setDirty(InfoPane_DIRTY_BIT, true );
		GC.GetEngineUserInterface()->setDirty(CityScreen_DIRTY_BIT, true);
		GC.GetEngineUserInterface()->setDirty(ColoredPlots_DIRTY_BIT, true);

		auto_ptr<ICvCity1> pCity = GC.WrapCityPointer(GetCity());
		GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_SPECIALISTS);
	}

	GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();
}


/// Find the worst Specialist and remove him from duty
bool CvCityCitizens::DoRemoveWorstSpecialist(SpecialistTypes eDontChangeSpecialist, const BuildingTypes eDontRemoveFromBuilding /* = NO_BUILDING */, bool bUpdateNow)
{
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);

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
		if (eDontChangeSpecialist == pkBuildingInfo->GetSpecialistType())
		{
			continue;
		}

		if (GetNumSpecialistsInBuilding(eBuilding) > 0)
		{
			DoRemoveSpecialistFromBuilding(eBuilding, true, false, bUpdateNow);
			return true;
		}
	}

	return false;
}

/// How many Default Specialists are assigned in this City?
int CvCityCitizens::GetNumDefaultSpecialists() const
{
	return m_iNumDefaultSpecialists;
}

/// Changes how many Default Specialists are assigned in this City
void CvCityCitizens::ChangeNumDefaultSpecialists(int iChange, bool bUpdateNow)
{
	m_iNumDefaultSpecialists += iChange;

	SpecialistTypes eSpecialist = (SpecialistTypes)GC.getDEFAULT_SPECIALIST();
	m_aiSpecialistCounts[eSpecialist] += iChange;

	if (m_aiSpecialistCounts[eSpecialist] > m_pCity->getPopulation())
		OutputDebugString("warning: implausible number of specialists!\n");

	GetCity()->processSpecialist(eSpecialist, iChange, bUpdateNow);

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

		if (eSpecialist != (SpecialistTypes)GC.getDEFAULT_SPECIALIST())
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

		if (eSpecialist != (SpecialistTypes)GC.getDEFAULT_SPECIALIST())
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
					const CivilizationTypes eCivilization = GetCity()->getCivilizationType();
					CvCivilizationInfo* pCivilizationInfo = GC.getCivilizationInfo(eCivilization);
					if (pCivilizationInfo != NULL)
					{
						UnitTypes eUnit = (UnitTypes)pCivilizationInfo->getCivilizationUnits(eUnitClass);

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
	int iThreshold = /*100*/ GC.getGREAT_PERSON_THRESHOLD_BASE();
	int iNumCreated;

	if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER", true))
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatWritersCreated(MOD_GLOBAL_TRULY_FREE_GP);
		iThreshold -= GC.getGWAM_THRESHOLD_DECREASE();
#else
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatWritersCreated();
#endif
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST", true))
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatArtistsCreated(MOD_GLOBAL_TRULY_FREE_GP);
		iThreshold -= GC.getGWAM_THRESHOLD_DECREASE();
#else
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatArtistsCreated();
#endif
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN", true))
	{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGreatMusiciansCreated(MOD_GLOBAL_TRULY_FREE_GP);
		iThreshold -= GC.getGWAM_THRESHOLD_DECREASE();
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
	const UnitTypes eThisPlayersUnitType = (UnitTypes)GET_PLAYER(GetCity()->getOwner()).getCivilizationInfo().getCivilizationUnits(eUnitClass);
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
	iThreshold += (/*50*/ GC.getGREAT_PERSON_THRESHOLD_INCREASE() * iNumCreated);

	// Game Speed mod
	iThreshold *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	// Start era mod
	iThreshold *= GC.getGame().getStartEraInfo().getGreatPeoplePercent();
	iThreshold /= 100;

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
#if defined(MOD_API_EXTENSIONS)
		if (!GC.getGame().isReallyNetworkMultiPlayer())
#else
		if (!GC.getGame().isNetworkMultiPlayer())	// KWG: Candidate for !GC.getGame().IsOption(GAMEOPTION_SIMULTANEOUS_TURNS)
#endif
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
			int iWLTKD = (GC.getCITY_RESOURCE_WLTKD_TURNS() / 3);

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
					int iWLTKD = (GC.getCITY_RESOURCE_WLTKD_TURNS() / 3);

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
			CvPlot *pSpawnPlot = kPlayer.GetGreatAdmiralSpawnPlot(newUnit);
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
			int iWLTKD = (GC.getCITY_RESOURCE_WLTKD_TURNS() / 3);

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
					int iWLTKD = (GC.getCITY_RESOURCE_WLTKD_TURNS() / 3);

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

SPrecomputedExpensiveNumbers::SPrecomputedExpensiveNumbers(CvCity * pCity)
{
	iUnhappinessFromGold = pCity->getUnhappinessFromGold();
	iUnhappinessFromScience = pCity->getUnhappinessFromScience();
	iUnhappinessFromCulture = pCity->getUnhappinessFromCulture();
	iUnhappinessFromReligion = pCity->getUnhappinessFromReligion();
	iUnhappinessFromDistress = pCity->getUnhappinessFromDefense();

	iExcessFoodTimes100 = pCity->getYieldRateTimes100(YIELD_FOOD, false) - (pCity->foodConsumption() * 100);
	iFoodCorpMod = pCity->GetTradeRouteCityMod(YIELD_FOOD);
}
