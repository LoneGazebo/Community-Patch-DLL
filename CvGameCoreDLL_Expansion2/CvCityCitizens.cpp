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
#include "CvMilitaryAI.h"
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

	int iI = 0;

	m_vWorkedPlots.clear();
	ASSERT_DEBUG((0 < MAX_CITY_PLOTS), "MAX_CITY_PLOTS is not greater than zero but an array is being allocated in CvCityCitizens::reset");
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
CvPlayer* CvCityCitizens::GetPlayer() const
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
		static EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY", true);
		bool bInDeficit = thisPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);

		static EconomicAIStrategyTypes eStrategyBuildingReligion = (EconomicAIStrategyTypes)GC.getInfoTypeForString("ECONOMICAISTRATEGY_DEVELOPING_RELIGION", true);
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
		else if (m_pCity->GetCityCitizens()->CityShouldEmphasizeProduction())
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

			int iPotentialUnhappiness = m_pCity->getPotentialUnhappinessWithGrowthVal() - m_pCity->GetPotentialHappinessWithGrowthVal();
			if (iPotentialUnhappiness > 0 && thisPlayer.IsEmpireUnhappy())
			{
				//default value for vanilla happiness
				int iLockThreshold = -20;
				if (MOD_BALANCE_VP)
				{
					if (thisPlayer.IsEarlyExpansionPhase())
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

		ASSERT_DEBUG((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(true), "Gameplay: More workers than population in the city.");
		DoReallocateCitizens(bForceCheck);
	}

	ASSERT_DEBUG((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(true), "Gameplay: More workers than population in the city.");
	DoSpecialists();

	ASSERT_DEBUG((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(true), "Gameplay: More workers than population in the city.");
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

			iBonus += iEffect;
		}
		iBonus = m_pCity->GetYieldPerXFeatureFromBuildingsTimes100(eFeature, eYield);
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

			if (iEffect > 0)
			{
				const CvReligion* pReligion = m_pCity->GetCityReligions()->GetMajorityReligion();
				if (pReligion && pReligion->m_Beliefs.RequiresNoFeature(m_pCity->getOwner()) && (eFeature != NO_FEATURE || pPlot->isHills()))
				{
					iEffect = 0;
				}
			}

			iBonus += iEffect;
		}
		iBonus += m_pCity->GetYieldPerXTerrainFromBuildingsTimes100(eTerrain, eYield);
	}

	return iBonus;
}

YieldAndGPPList CvCityCitizens::GetPlotYields(CvPlot* pPlot, SPrecomputedExpensiveNumbers& cache)
{
	YieldAndGPPList yieldList;
	
	if (!pPlot)
		return yieldList;

	for (int iI = 0; iI < (int)yieldList.yield.size(); iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;

		//base yield plus combo bonuses. 
		int iYield100 = pPlot->getYield(eYield) * 100;

		iYield100 += GET_PLAYER(GetOwner()).getYieldFromNonSpecialistCitizens(eYield);
		iYield100 += GetBonusPlotValue(pPlot, eYield, cache);

		// modifiers 
		iYield100 *= 100 + m_pCity->getYieldRateModifier(eYield);
		iYield100 /= 100;
		yieldList.yield[iI] = iYield100;
	}
	return yieldList;
}

bool CvCityCitizens::CityShouldEmphasizeFood(int iAssumedExcessFood) const
{
	bool bCityFoodProduction = !GET_PLAYER(GetOwner()).isHuman() && m_pCity->getPopulation() > 3 && m_pCity->isFoodProduction(); //settler!
	bool bSmallCity = m_pCity->getPopulation() < /*2 in CP, 4 in VP*/ GD_INT_GET(CITY_MIN_SIZE_FOR_SETTLERS);
	int iFoodThreshold = GetExcessFoodThreshold100();

	//do we have enough food? always want to grow a little bit a least, so don't use zero as threshold
	bool bNeedFood = (iAssumedExcessFood < iFoodThreshold);

	return (bNeedFood || bSmallCity || bCityFoodProduction);
}

bool CvCityCitizens::CityShouldEmphasizeProduction() const
{
	if (GET_PLAYER(GetOwner()).isHuman() || m_pCity->getPopulation() <= 3)
		return false;

	//settler
	if (m_pCity->isFoodProduction())
		return true;

	// world wonder
	if (m_pCity->IsBuildingWorldWonder())
		return true;

	// league project
	ProcessTypes eProcess = m_pCity->getProductionProcess();
	if (eProcess != NO_PROCESS && m_pCity->IsProcessInternationalProject(eProcess))
		return true;

	// one-time project
	ProjectTypes eProject = m_pCity->getProductionProject();
	if (eProject != NO_PROJECT)
	{
		CvProjectEntry* pkProjectInfo = GC.getProjectInfo(eProject);
		if (!pkProjectInfo->IsRepeatable())
		{
			return true;
		}
	}

	// spaceship part
	UnitTypes eUnit = m_pCity->getProductionUnit();
	if (eUnit != NO_UNIT)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eUnit);
		if (pkUnitInfo && pkUnitInfo->GetSpaceshipProject() != -1)
		{
			return true;
		}
	}

	return false;
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
void CvCityCitizens::SetNoAutoAssignSpecialists(bool bValue, bool bReset)
{
	if (m_bNoAutoAssignSpecialists != bValue || bReset)
	{
		m_bNoAutoAssignSpecialists = bValue;

		// If we're giving the AI control clear all manually assigned Specialists
		if (!bValue)
			DoClearForcedSpecialists();

		// If we activate auto-assignment do it right now
		// No auto-assignment after activation of manual mode
		if (!bValue || bReset)
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
	ASSERT_DEBUG(eFocus >= NO_CITY_AI_FOCUS_TYPE);
	ASSERT_DEBUG(eFocus < NUM_CITY_AI_FOCUS_TYPES);

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

/// What is the Building Type the AI likes the Specialist of most right now?
BuildingTypes CvCityCitizens::GetAIBestSpecialistBuilding(int& iSpecialistValue, SPrecomputedExpensiveNumbers& cache, bool bLogging)
{
	if (m_pCity->GetResistanceTurns() > 0)
		return NO_BUILDING;

	int iBestSpecialistValue = -INT_MAX;
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
						iValue = GetSpecialistValue(eSpecialist, /*bAdd*/ true, cache);
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

/// What is the Building Type the AI would be the least unwilling to remove a Specialist from?
BuildingTypes CvCityCitizens::GetAIWorstSpecialistCurrentlyInBuilding(int& iSpecialistValue, SPrecomputedExpensiveNumbers& cache)
{
	BuildingTypes eWorstBuilding = NO_BUILDING;
	int iWorstSpecialistValue = -INT_MAX;

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

			int iValue = GetSpecialistValue(eSpecialist, /*bAdd*/ false, cache);
			checked[eSpecialist] = iValue;

			if (iValue > iWorstSpecialistValue)
			{
				eWorstBuilding = allBuildings[i];
				iWorstSpecialistValue = iValue;
			}
		}
	}

	iSpecialistValue = iWorstSpecialistValue;
	return eWorstBuilding;
}

/// the GPP rate of a eSpecialist
int CvCityCitizens::GetSpecialistGPPRate(SpecialistTypes eSpecialist, SPrecomputedExpensiveNumbers& cache)
{
	// update cached value if necessary
	if (cache.iSpecialistGPPRates.size() == 0)
		cache.update(m_pCity);

	if (cache.iSpecialistGPPRates[eSpecialist] == INT_MAX)
	{
		CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
		UnitClassTypes eUnitClass = (UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass();
		int iGPPRate = (pSpecialistInfo->getGreatPeopleRateChange() + m_pCity->GetEventGPPFromSpecialists()) * 100;
		int iGPPRateMod = m_pCity->getGreatPeopleRateModifier() + GetPlayer()->getGreatPeopleRateModifier() + m_pCity->GetSpecialistRateModifierFromBuildings(eSpecialist);

		// Player and Golden Age mods to this specific class
		if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
		{
			iGPPRateMod += GetPlayer()->getGreatScientistRateModifier();
		}
		else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER"))
		{
			if (GetPlayer()->isGoldenAge())
			{
				iGPPRateMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatWriterRateModifier();
			}
			iGPPRateMod += GetPlayer()->getGreatWriterRateModifier();
		}
		else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
		{
			if (GetPlayer()->isGoldenAge())
			{
				iGPPRateMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatArtistRateModifier();
			}
			iGPPRateMod += GetPlayer()->getGreatArtistRateModifier();
		}
		else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
		{
			if (GetPlayer()->isGoldenAge())
			{
				iGPPRateMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatMusicianRateModifier();
			}
			iGPPRateMod += GetPlayer()->getGreatMusicianRateModifier();
		}
		else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
		{
			iGPPRateMod += GetPlayer()->getGreatMerchantRateModifier();
		}
		else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
		{
			iGPPRateMod += GetPlayer()->getGreatEngineerRateModifier();
		}
		else if (MOD_BALANCE_VP && eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
		{
			iGPPRateMod += GetPlayer()->getGreatDiplomatRateModifier();
		}

		GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
		if (eGreatPerson != NO_GREATPERSON)
		{
			iGPPRateMod += GetPlayer()->getSpecificGreatPersonRateModifierFromMonopoly(eGreatPerson);
			iGPPRateMod += GetCity()->GetReligionGreatPersonRateModifier(eGreatPerson);
			int iNumPuppets = GetPlayer()->GetNumPuppetCities();
			if (iNumPuppets > 0)
			{
				iGPPRateMod += (iNumPuppets * GetPlayer()->GetPlayerTraits()->GetPerPuppetGreatPersonRateModifier(eGreatPerson));
			}
		}

		iGPPRate *= (100 + iGPPRateMod);
		iGPPRate /= 100;
		cache.iSpecialistGPPRates[eSpecialist] = iGPPRate;
	}
	return cache.iSpecialistGPPRates[eSpecialist];
}

///  Which yields does eSpecialist give?
YieldAndGPPList CvCityCitizens::GetSpecialistYields(SpecialistTypes eSpecialist)
{
	YieldAndGPPList yieldList;

	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if (pSpecialistInfo == NULL)
		return yieldList;


	CvPlayer* pPlayer = m_pCity->GetPlayer();
	for (int iI = 0; iI < (int)yieldList.yield.size(); iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;

		int iYield100 = pPlayer->specialistYield(eSpecialist, eYield) * 100;
		// laborers get non-specialist bonus yields:
		if (eSpecialist == GD_INT_GET(DEFAULT_SPECIALIST))
		{
			iYield100 += GET_PLAYER(GetOwner()).getYieldFromNonSpecialistCitizens(eYield);
		}
		//Culture is treated differently, sadly.
		if (eYield == YIELD_CULTURE)
			iYield100 = m_pCity->GetCultureFromSpecialist(eSpecialist) * 100;

		//religion bonus
		ReligionTypes eMajority = m_pCity->GetCityReligions()->GetReligiousMajority();
		if (eMajority >= RELIGION_PANTHEON)
		{
			const CvReligion* pReligion = m_pCity->GetCityReligions()->GetMajorityReligion();
			if (pReligion)
			{
				iYield100 += pReligion->m_Beliefs.GetSpecialistYieldChange(eSpecialist, eYield, m_pCity->getOwner(), m_pCity) * 100;
			}
		}

		// modifiers 
		iYield100 *= 100 + m_pCity->getYieldRateModifier(eYield);
		iYield100 /= 100;
		yieldList.yield[iI] = iYield100;
	}

	// don't count laborers as specialists ...
	if (eSpecialist != (SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST))
		yieldList.iNumSpecialists[eSpecialist] = 1;

	return yieldList;
}

int CvCityCitizens::GetPlotValueQuick(CvPlot* pPlot, bool bAdd, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving, bool bAssumeBelowGrowthThreshold, bool bAssumeInDebt)
{
	YieldAndGPPList yieldList = GetPlotYields(pPlot, cache);
	return ScoreYieldChangeQuick(bAdd ? yieldList : yieldList.negative(), cache, bAssumeStarving, bAssumeBelowGrowthThreshold, bAssumeInDebt);
}

// What is the overall value of the current plot?
// This is a bit tricky because the value of a plot changes depending on which other plots are being worked!
int CvCityCitizens::GetPlotValue(CvPlot* pPlot, bool bAdd, SPrecomputedExpensiveNumbers& cache)
{
	YieldAndGPPList yieldList = GetPlotYields(pPlot, cache);
	return ScoreYieldChange(bAdd ? yieldList : yieldList.negative(), cache);
}

int CvCityCitizens::GetSpecialistValueQuick(SpecialistTypes eSpecialist, bool bAdd, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving, bool bAssumeBelowGrowthThreshold, bool bAssumeInDebt)
{
	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if (pSpecialistInfo == NULL)
		return 0;

	YieldAndGPPList yieldList = GetSpecialistYields(eSpecialist);
	return ScoreYieldChangeQuick(bAdd ? yieldList : yieldList.negative(), cache, bAssumeStarving, bAssumeBelowGrowthThreshold, bAssumeInDebt);
}

/// How valuable is eSpecialist?
int CvCityCitizens::GetSpecialistValue(SpecialistTypes eSpecialist, bool bAdd, SPrecomputedExpensiveNumbers& cache)
{
	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if (pSpecialistInfo == NULL)
		return 0;

	YieldAndGPPList yieldList = GetSpecialistYields(eSpecialist);
	return ScoreYieldChange(bAdd ? yieldList : yieldList.negative(), cache);
}



int CvCityCitizens::GetBaseValuePerYield(YieldTypes eYield, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving, bool bAssumeBelowGrowthThreshold, bool bAssumeInDebt) const
{
	CityAIFocusTypes eFocus = GetFocusType();
	int iYieldMod = 0;

	// if we're working a process that converts production into eYield, always value eYield highly
	ProcessTypes eProcess = m_pCity->getProductionProcess();
	bool bFoodProcess = eProcess != NO_PROCESS && GC.getProcessInfo(eProcess)->getProductionToYieldModifier(YIELD_FOOD) > 0;

	switch (eYield)
	{
	case YIELD_FOOD:
		// in small cities, treat being under the growth threshold like starvation, unless we focus on a yield other than food
		if (bAssumeStarving || (bAssumeBelowGrowthThreshold && m_pCity->getPopulation() <= 3 && (eFocus == NO_CITY_AI_FOCUS_TYPE || eFocus == CITY_AI_FOCUS_TYPE_FOOD || eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH || eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH)))
			iYieldMod =  /*500*/ GD_INT_GET(AI_CITIZEN_VALUE_FOOD_STARVING);
		else if (bAssumeBelowGrowthThreshold || eFocus == CITY_AI_FOCUS_TYPE_FOOD || bFoodProcess)
			iYieldMod =  /*32*/ GD_INT_GET(AI_CITIZEN_VALUE_FOOD_NEED_GROWTH);
		else
			iYieldMod = /*8*/ GD_INT_GET(AI_CITIZEN_VALUE_FOOD);
		break;
	case YIELD_PRODUCTION:
		iYieldMod = /*12*/ GD_INT_GET(AI_CITIZEN_VALUE_PRODUCTION);
		// working a process?
		if (eProcess != NO_PROCESS)
		{
			const CvProcessInfo* pkProcessInfo = GC.getProcessInfo(eProcess);
			iYieldMod *= 3;
			// if the process converts production to some other yield, use the valuation of that yield instead
			for (int i = 0; i < YIELD_TOURISM; i++)
			{
				YieldTypes eLoopYield = (YieldTypes)i;
				if (eLoopYield != YIELD_PRODUCTION && pkProcessInfo->getProductionToYieldModifier(eLoopYield) > 0)
				{
					// converting production into eLoopYield? value production with the corresponding fraction of the value of eYield
					iYieldMod = GetBaseValuePerYield(eLoopYield, cache, bAssumeStarving, bAssumeBelowGrowthThreshold, bAssumeInDebt) * pkProcessInfo->getProductionToYieldModifier(eLoopYield);
					iYieldMod /= 100;
					break;
				}
			}
			break;
		}
		if (eFocus == CITY_AI_FOCUS_TYPE_PRODUCTION)
			iYieldMod *= 3;
		break;
	case YIELD_GOLD:
		iYieldMod = /*12 in CP, 8 in VP*/ GD_INT_GET(AI_CITIZEN_VALUE_GOLD);
		if (eFocus == CITY_AI_FOCUS_TYPE_GOLD)
			iYieldMod *= 3;
		else if (bAssumeInDebt)
			iYieldMod = /*24*/ GD_INT_GET(AI_CITIZEN_VALUE_GOLD_IN_DEBT);
		break;
	case YIELD_SCIENCE:
		iYieldMod = /*20*/ GD_INT_GET(AI_CITIZEN_VALUE_SCIENCE);
		if (eFocus == CITY_AI_FOCUS_TYPE_SCIENCE)
			iYieldMod *= 3;
		else if (cache.bWantScience)
			iYieldMod *= 2;
		break;
	case YIELD_CULTURE:
	case YIELD_TOURISM:
		iYieldMod = /*20*/ GD_INT_GET(AI_CITIZEN_VALUE_CULTURE);
		if (eFocus == CITY_AI_FOCUS_TYPE_CULTURE)
			iYieldMod *= 3;
		else if (cache.bWantArt)
			iYieldMod *= 2;
		break;
	case YIELD_FAITH:
		iYieldMod = /*20*/ GD_INT_GET(AI_CITIZEN_VALUE_FAITH);
		if (eFocus == CITY_AI_FOCUS_TYPE_FAITH)
			iYieldMod *= 3;
		break;
	default:
		iYieldMod = 10; //not too low to limit influence of moddable values
	}
	return iYieldMod;

}

int CvCityCitizens::GetBaseValuePerGPP(SpecialistTypes eSpecialist, SPrecomputedExpensiveNumbers& cache) const
{
	CityAIFocusTypes eFocus = GetFocusType();

	int iValue = /*8 in CP, 10 in VP*/ GD_INT_GET(AI_CITIZEN_VALUE_GPP);

	int iMod = 0;
	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	UnitClassTypes eUnitClass = (UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass();
	// bonus value if we don't have a specialist of this type in any other city
	if (!cache.bAnySpecialistInOtherCity[eSpecialist])
	{
		iMod += 50;
	}

	if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
	{
		if (cache.bWantScience)
		{
			iMod += 50;
		}
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER"))
	{
		//Bonus for art-producing specialists
		iMod += 200;
		if (cache.bWantArt)
		{
			iMod += 50;
		}
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
	{
		//Bonus for art-producing specialists
		iMod += 200;
		if (cache.bWantArt)
		{
			iMod += 50;
		}

	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
	{
		//Bonus for art-producing specialists
		iMod += 200;
		if (cache.bWantArt)
		{
			iMod += 50;
		}
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
	{
		if (cache.bWantDiplo)
		{
			iMod += 20;
		}
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
	{
		if (cache.bWantScience || cache.bWantArt)
		{
			iMod += 20;
		}
	}
	else if (MOD_BALANCE_VP && eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
	{
		if (cache.bWantDiplo)
		{
			iMod += 50;
		}
	}

	//Bonus for existing progress towards a Great Person of that type
	iMod += 200 * GetSpecialistGreatPersonProgress(eSpecialist) / GetSpecialistUpgradeThreshold(eUnitClass);

	// Bonus for total number of specialist slots: we like to have multiple specialists of the same type
	iMod += 10 * GetSpecialistSlots(eSpecialist);

	iValue *= (100 + iMod);
	iValue /= 100;

	if (eFocus == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE)
	{
		iValue *= 2;
	}

	return iValue;
}

// for a quick initial assigment of worked plots. result doesn't depend on the other worked tiles of the city
int CvCityCitizens::ScoreYieldChangeQuick(YieldAndGPPList yieldChanges, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving, bool bAssumeBelowGrowthThreshold, bool bAssumeInDebt)
{
	int iScore = 0;

	for (int iI = 0; iI < (int)yieldChanges.yield.size(); iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		int iYield100 = yieldChanges.yield[iI];

		// Inca exception: if we're already under the non-specialist food consumption threshold, we can safely remove even more food ...
		if (m_pCity->IsNoStarvationNonSpecialist() && eYield == YIELD_FOOD && iYield100 < 0 && cache.iFoodConsumptionTimes100 <= cache.iFoodConsumptionAssumeNoReductionNonSpecialistsTimes100)
		{
			// ... unless we're adding a specialist
			int iNumSpecialistAdded = 0;
			for (int iI = 0; iI < (int)yieldChanges.iNumSpecialists.size(); iI++)
			{
				iNumSpecialistAdded += yieldChanges.iNumSpecialists[iI];
			}
			if (iNumSpecialistAdded <= 0)
				continue;

		}

		if (iYield100 != 0)
		{
			int iYieldMod = GetBaseValuePerYield(eYield, cache, bAssumeStarving, bAssumeBelowGrowthThreshold, bAssumeInDebt);
			iScore += iYield100 * max(1, iYieldMod);
		}
	}

	for (int iI = 0; iI < (int)yieldChanges.iNumSpecialists.size(); iI++)
	{
		if (yieldChanges.iNumSpecialists[iI] != 0)
		{
			iScore += GetBaseValuePerGPP((SpecialistTypes)iI, cache) * yieldChanges.iNumSpecialists[iI];
		}
	}

	return iScore;
}
int CvCityCitizens::ScoreYieldChange(YieldAndGPPList yieldChanges, SPrecomputedExpensiveNumbers& cache)
{
	int iValue = 0;

	//////////
	// Unhappiness Calculation
	//////////

	int iUnhappinessChange = 0;
	int iNumSpecialistsAdded = 0;
	for (uint ui = 0; ui < yieldChanges.iNumSpecialists.size(); ui++)
	{
		iNumSpecialistsAdded += yieldChanges.iNumSpecialists[ui];
	}
	if (MOD_BALANCE_VP)
	{
		// unhappiness from needs:
		int iUnhappinessNeedsNow = cache.iDistress + cache.iPoverty + cache.iIlliteracy + cache.iBoredom + cache.iOtherUnhappiness;
		int iUnhappinessNeedsThen = iUnhappinessNeedsNow;

		// we have precalculated how yield rates must change in order to have an effect on the current unhappiness level from needs
		if (yieldChanges.yield[YIELD_FOOD] + yieldChanges.yield[YIELD_PRODUCTION] <= cache.iBasicNeedsRateChangeForIncreasedDistress ||
			yieldChanges.yield[YIELD_FOOD] + yieldChanges.yield[YIELD_PRODUCTION] >= cache.iBasicNeedsRateChangeForReducedDistress)
		{
			// the change in yield rate would cause change the distress value. calculate the difference between the new value and the current one
			iUnhappinessNeedsThen += m_pCity->GetDistress(false, yieldChanges.yield[YIELD_FOOD] + yieldChanges.yield[YIELD_PRODUCTION]) - cache.iDistress;
		}

		if (yieldChanges.yield[YIELD_GOLD] <= cache.iGoldRateChangeForIncreasedPoverty || yieldChanges.yield[YIELD_GOLD] >= cache.iGoldRateChangeForReducedPoverty)
		{
			iUnhappinessNeedsThen += m_pCity->GetPoverty(false, yieldChanges.yield[YIELD_GOLD]) - cache.iPoverty;
		}
		if (yieldChanges.yield[YIELD_SCIENCE] <= cache.iScienceRateChangeForIncreasedIlliteracy || yieldChanges.yield[YIELD_SCIENCE] >= cache.iScienceRateChangeForReducedIlliteracy)
		{
			iUnhappinessNeedsThen += m_pCity->GetIlliteracy(false, yieldChanges.yield[YIELD_SCIENCE]) - cache.iIlliteracy;
		}
		if (yieldChanges.yield[YIELD_CULTURE] <= cache.iCultureRateChangeForIncreasedBoredom || yieldChanges.yield[YIELD_CULTURE] >= cache.iCultureRateChangeForReducedBoredom)
		{
			iUnhappinessNeedsThen += m_pCity->GetBoredom(false, yieldChanges.yield[YIELD_CULTURE]) - cache.iBoredom;
		}


		iUnhappinessNeedsThen = min(iUnhappinessNeedsThen, m_pCity->getPopulation());
		iUnhappinessNeedsNow = min(iUnhappinessNeedsNow, m_pCity->getPopulation());
		iUnhappinessChange += iUnhappinessNeedsThen - iUnhappinessNeedsNow;

		// unhappiness from specialists:
		if (iNumSpecialistsAdded != 0)
		{
			iUnhappinessChange += m_pCity->getUnhappinessFromSpecialists(GetTotalSpecialistCount() + iNumSpecialistsAdded) - cache.iUrbanization;
		}

		if (iUnhappinessChange != 0)
		{
			int iLocalHappiness = cache.iLocalHappiness;
			int iLocalUnhappiness = cache.iLocalUnhappiness;
			int iGlobalHappiness = cache.iLocalHappiness + cache.iHappinessOtherCities;
			int iGlobalUnhappiness = cache.iLocalUnhappiness + cache.iUnhappinessOtherCities;
			int iHappinessPercent = ((iGlobalHappiness * 100) / max(1, iGlobalUnhappiness) / 2);

			// make sure the calculation is symmetrical: if iUnhappinessChange < 0, we calculate backwards from the final state and then take the negative of the resulting score
			int iMultiplier = 1;
			if (iUnhappinessChange < 0)
			{
				iMultiplier = -1;
				iLocalUnhappiness += iUnhappinessChange;
				iGlobalUnhappiness += iUnhappinessChange;
				iHappinessPercent = ((iGlobalHappiness * 100) / max(1, iGlobalUnhappiness) / 2);
			}
			iUnhappinessChange *= iMultiplier;

			int iUnhappinessScore = 0;
			for (int i = 0; i < iUnhappinessChange; i++)
			{
				// how does one extra unhappiness affect our happiness situation?
				iLocalUnhappiness++;
				iGlobalUnhappiness++;
				iHappinessPercent = ((iGlobalHappiness * 100) / max(1, iGlobalUnhappiness) / 2);

				// would our empire be very unhappy? then we really don't want it
				if (iHappinessPercent <= GD_INT_GET(VERY_UNHAPPY_THRESHOLD))
				{
					iUnhappinessScore += GD_INT_GET(AI_CITIZEN_UNHAPPINESS_VALUE_EMPIRE_VERY_UNHAPPY);
				}
				// empire unhappy or close to it?
				else if ((iHappinessPercent <= GD_INT_GET(UNHAPPY_THRESHOLD) + 5) || (iGlobalHappiness - iGlobalUnhappiness < 5))
				{
					iUnhappinessScore += GD_INT_GET(AI_CITIZEN_UNHAPPINESS_VALUE_EMPIRE_UNHAPPY);
				}
				else
				{
					// empire happy. what about the city?
					if (iLocalUnhappiness > iLocalHappiness)
					{
						iUnhappinessScore += GD_INT_GET(AI_CITIZEN_UNHAPPINESS_VALUE_CITY_UNHAPPY);
					}
					else
					{
						// happy city, happy empire. then we don't mind the additional unhappiness too much
						iUnhappinessScore += GD_INT_GET(AI_CITIZEN_UNHAPPINESS_VALUE_EMPIRE_HAPPY);
					}
				}
			}

			iValue += iUnhappinessScore * iMultiplier;
		}
	}

	//////////
	// Yields
	//////////

	for (int iI = 0; iI < (int)yieldChanges.yield.size(); iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;

		int iYield100 = yieldChanges.yield[iI];

		// extra yields for having at least one specialist in the city?
		if (cache.iYieldChangeAnySpecialist[iI] > 0)
		{
			int iTotalSpecialistCount = GetTotalSpecialistCount();
			if (iTotalSpecialistCount > 0 && iNumSpecialistsAdded == -iTotalSpecialistCount)
			{
				// remove extra yield
				iYield100 -= cache.iYieldChangeAnySpecialist[iI];
			}
			else if (iTotalSpecialistCount == 0 && iNumSpecialistsAdded > 0)
			{
				// add extra yield
				iYield100 += cache.iYieldChangeAnySpecialist[iI];
			}
		}
		
		if (eYield == YIELD_FOOD)
		{
			// special scoring for food: valuation depends on how much net food we produce now and after the yield change

			int iNetFoodNow = cache.iFoodRateTimes100 - cache.iFoodConsumptionTimes100;
			// food consumption also depends on how many specialists are worked
			int iNetFoodThen = iNetFoodNow + iYield100 - iNumSpecialistsAdded * (m_pCity->foodConsumptionSpecialistTimes100() - m_pCity->foodConsumptionNonSpecialistTimes100());
			if (m_pCity->IsNoStarvationNonSpecialist())
			{
				// if non-specialists can't cause starvation, the calculation is more complicated. we have to recalculate current and future food consumption by non-specialists manually
				// iNetFoodThen = iNetFoodNow + (Change in Food Rate) - (Change in Food Consumption)
				// with (Change in Food Consumption) = (Future Food Consumption Non-Specialists) - (Current Food Consumption Non-Specialists) + (Change in Food Consumption Specialists)
				iNetFoodThen = iNetFoodNow + iYield100 - (min(cache.iFoodRateTimes100 + iYield100, cache.iFoodConsumptionAssumeNoReductionNonSpecialistsTimes100 - iNumSpecialistsAdded * m_pCity->foodConsumptionNonSpecialistTimes100()) - min(cache.iFoodRateTimes100, cache.iFoodConsumptionAssumeNoReductionNonSpecialistsTimes100) + iNumSpecialistsAdded * m_pCity->foodConsumptionSpecialistTimes100());
			}

			// special case of building a settler: all excess food is converted into production
			if (m_pCity->isFoodProduction())
			{
				int iExcessFoodDiff = max(iNetFoodThen, 0) - max(iNetFoodNow, 0); // difference in excess food only
				yieldChanges.yield[YIELD_PRODUCTION] += iExcessFoodDiff;
				iNetFoodThen = min(iNetFoodThen, 0);
				iNetFoodNow = min(iNetFoodNow, 0);
			}


			// growth mods, apply them only if we're growing
			int iGrossFoodNow = iNetFoodNow;
			if (iGrossFoodNow > 0)
			{
				iGrossFoodNow *= 100 + cache.iGrowthMod;
				iGrossFoodNow /= 100;
			}
			int iGrossFoodThen = iNetFoodThen;
			if (iGrossFoodThen > 0)
			{
				// happiness changes would affect growth mod
				if (iUnhappinessChange != 0)
				{
					iGrossFoodThen *= 100 + m_pCity->getGrowthMods(NULL, -iUnhappinessChange);
				}
				else
				{
					iGrossFoodThen *= 100 + cache.iGrowthMod;
				}
				iGrossFoodThen /= 100;
			}

			if (iGrossFoodNow == iGrossFoodThen)
				// no effective change
				continue;

			// we want the scoring to be completely reversible: if applying a change has a score of x, reverting the change after it's been applied should have a score of -x
			// always calculate as if we were adding food. if we are actually subtracting it, we'll later multiply the result with -1
			bool bLessGrossFoodAfterChange = iGrossFoodThen < iGrossFoodNow;
			int iLower = min(iGrossFoodNow, iGrossFoodThen);
			int iHigher = max(iGrossFoodNow, iGrossFoodThen);

			int iGrowthThreshold = GetExcessFoodThreshold100(); // how much excess food would we like to have for adequate city growth?

			// now the scoring itself
			int iFoodValue = 0;

			// we want to avoid starving at all costs, and it would be nice to have enough food to grow a bit
			// food above the growth threshold is not so important
			int iValuePerAddedFoodStarving = GetBaseValuePerYield(eYield, cache, /*bAssumeStarving*/ true);
			int iValuePerAddedFoodBelowGrowthThreshold = GetBaseValuePerYield(eYield, cache, /*bAssumeStarving*/ false, /*bAssumeBelowGrowthThreshold*/ true);
			int iValuePerAddedFoodAboveGrowthThreshold = GetBaseValuePerYield(eYield, cache, /*bAssumeStarving*/ false, /*bAssumeBelowGrowthThreshold*/ false);

			// for the scoring, we split the difference in excess food in the different parts and value them accordingly
			// the code below is a more efficient calculation of the following:
			// for (i = iLower; i < iHigher; i++) { if (i < 0) {iFoodValue += iValuePerFoodStarving} ; if (0 <= i < GrowthThreshold) {iFoodValue += iValueBelowThreshold} ; if(i > iGrowthThreshold) {...} }

			// how much of the difference (iHigher - iLower) corresponds to food in the "starving" area?
			// case 1) if both iHigher and iLower < 0   -> iHigher - iLower
			// case 2) if iLower < 0 and iHigher > 0    -> 0 - iLower
			// case 3) if both > 0					    -> 0
			// so in all three cases: min(iHigher, 0) - min(iLower, 0)
			iFoodValue += (min(iHigher, 0) - min(iLower, 0)) * iValuePerAddedFoodStarving;

			// the same calculation for food above the starving threshold and below the growth threshold
			iFoodValue += (max(min(iHigher, iGrowthThreshold), 0) - max(min(iLower, iGrowthThreshold), 0)) * iValuePerAddedFoodBelowGrowthThreshold;
			// and finally for food above the growth threshold
			iFoodValue += (max(iHigher, iGrowthThreshold) - max(iLower, iGrowthThreshold)) * iValuePerAddedFoodAboveGrowthThreshold;

			if (bLessGrossFoodAfterChange)
			{
				iFoodValue *= -1;
			}
			iValue += iFoodValue;
		}
		else if (eYield == YIELD_GOLD)
		{
			if (iYield100 != 0)
			{
				// gold value much higher if we're losing gold
				int iNetGoldNow = cache.iNetGold * 100;
				int iNetGoldThen = iNetGoldNow + iYield100;
				int iYield100InDebt = min(iNetGoldThen, 0) - min(iNetGoldNow, 0);
				int iYield100NotInDebt = iYield100 - iYield100InDebt;
				int iYieldModInDebt = GetBaseValuePerYield(eYield, cache, false, false, true);
				int iYieldModNotInDebt = GetBaseValuePerYield(eYield, cache);
				iValue += iYield100InDebt * max(1, iYieldModInDebt) + iYield100NotInDebt * max(1, iYieldModNotInDebt);
			}
		}
		else
		{
			if (iYield100 != 0)
			{
				int iYieldMod = GetBaseValuePerYield(eYield, cache);
				iValue += iYield100 * max(1, iYieldMod);
			}
		}
	}

	//////////
	// Great Person Points
	//////////

	for (int iI = 0; iI < (int)yieldChanges.iNumSpecialists.size(); iI++)
	{
		if (yieldChanges.iNumSpecialists[iI] == 0)
			continue;

		SpecialistTypes eSpecialist = (SpecialistTypes)iI;
		CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
		if (!pSpecialistInfo)
			continue;

		// how many great person points are added/subtracted?
		int iGPPRate = yieldChanges.iNumSpecialists[iI] * GetSpecialistGPPRate(eSpecialist, cache);

		int iGPPValuation = GetBaseValuePerGPP(eSpecialist, cache);
		iValue += iGPPRate * iGPPValuation;

		// the valuation of an added specialist increases with the number of specialists we already have. we like to use slots of the same type
		int iSpecialistsNow = m_aiSpecialistCounts[eSpecialist];
		int iSpecialistsThen = iSpecialistsNow + yieldChanges.iNumSpecialists[eSpecialist];
		
		// symmetry. if we're removing specialists, calculate as if we added them and then multiply with -1
		int iMultiplier = 1;
		if (iSpecialistsNow > iSpecialistsThen)
		{
			iMultiplier = -1;
			iSpecialistsNow = iSpecialistsThen;
			iSpecialistsThen = m_aiSpecialistCounts[eSpecialist];
		}

		for (int iNumSpec = iSpecialistsNow; iNumSpec < iSpecialistsThen; iNumSpec++)
		{
			iValue += iNumSpec * GD_INT_GET(AI_CITIZEN_SPECIALIST_COMBO_BONUS) * iMultiplier; // the more specialists we already have, the higher is the bonus
		}
	}

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
	ASSERT_DEBUG(m_iNumUnassignedCitizens >= 0, "invalid number of unassigned citizens in CvCityCitizens::ChangeNumUnassignedCitizens!");
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

vector<TileChange> CvCityCitizens::GetBestOptionsQuick(int iNumOptions, bool bAdd, bool bAllowOverride, SPrecomputedExpensiveNumbers& cache, bool bAssumeStarving, bool bAssumeBelowGrowthThreshold, bool bAssumeInDebt, bool bIncludePlots, bool bIncludeSpecialists, bool bNoTwoOptionsWithSameYields)
{
	// score all possible options
	vector<OptionWithScore<TileChange>> vScoredOptions;

	if (bIncludePlots)
	{
		// loop through all unworked plots
		for (int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
		{
			//never touch this
			if (iPlotLoop == CITY_HOME_PLOT)
				continue;

			CvPlot* pLoopPlot = GetCityPlotFromIndex(iPlotLoop);
			if (!pLoopPlot)
				continue;

			if ((bAdd && (bAllowOverride ? IsCanWorkWithOverride(pLoopPlot) : IsCanWork(pLoopPlot)) && !IsWorkingPlot(iPlotLoop)) || (!bAdd && IsWorkingPlot(pLoopPlot) && !IsForcedWorkingPlot(iPlotLoop)))
			{
				vScoredOptions.push_back(OptionWithScore<TileChange>(TileChange(pLoopPlot), GetPlotValueQuick(pLoopPlot, bAdd, gCachedNumbers, bAssumeStarving, bAssumeBelowGrowthThreshold, bAssumeInDebt)));
			}
		}
	}

	if (bIncludeSpecialists && !m_pCity->IsResistance() && !m_pCity->IsRazing())
	{
		for (int iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
		{
			if (iSpecialist != GD_INT_GET(DEFAULT_SPECIALIST)) // no laborers here
			{
				SpecialistTypes eSpecialist = (SpecialistTypes)iSpecialist;
				int iNumSpecialists = 0;
				if (bAdd)
				{
					// how many specialists of this type can we assign?
					if (GET_PLAYER(GetOwner()).isHuman() && (IsNoAutoAssignSpecialists()))
					{
						iNumSpecialists = 0;
					}
					else
					{
						iNumSpecialists = GetSpecialistSlots(eSpecialist) - GetSpecialistCount(eSpecialist);
						if (bNoTwoOptionsWithSameYields)
						{
							iNumSpecialists = min(1, iNumSpecialists);
						}
					}
				}
				else
				{
					// how many specialists of this type can we remove?
					iNumSpecialists = GetSpecialistCount(eSpecialist);
					if (iNumSpecialists > 0)
					{
						// exclude forced specialists
						const vector<BuildingTypes>& allBuildings = m_pCity->GetCityBuildings()->GetAllBuildingsHere();
						for (size_t iBuildingLoop = 0; iBuildingLoop < allBuildings.size(); iBuildingLoop++)
						{
							const BuildingTypes eBuilding = allBuildings[iBuildingLoop];
							CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
							if (pkBuildingInfo->GetSpecialistType() == eSpecialist)
							{
								iNumSpecialists -= GetNumForcedSpecialistsInBuilding(eBuilding);
							}
						}
					}
				}
				if (iNumSpecialists > 0)
				{
					if (bNoTwoOptionsWithSameYields)
					{
						iNumSpecialists = min(1, iNumSpecialists);
					}
					int iValue = GetSpecialistValueQuick(eSpecialist, /*bAdd*/ bAdd, gCachedNumbers, bAssumeStarving, bAssumeBelowGrowthThreshold, bAssumeInDebt);
					for (int i = 0; i < iNumSpecialists; i++)
					{
						vScoredOptions.push_back(OptionWithScore<TileChange>(TileChange(eSpecialist), iValue));
					}
				}
			}
		}
	}

	if (bIncludePlots)
	{
		SpecialistTypes eLaborer = (SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST);
		int iValue = GetSpecialistValueQuick(eLaborer, /*bAdd*/ bAdd, gCachedNumbers, bAssumeStarving, bAssumeBelowGrowthThreshold, bAssumeInDebt);
		int iNumLaborers = 0;
		if (bAdd)
		{
			iNumLaborers = 1;
		}
		else
		{
			// how many laborers can we remove?
			iNumLaborers = max(0, GetNumDefaultSpecialists() - GetNumForcedDefaultSpecialists());
		}

		for (int i = 0; i < iNumLaborers; i++)
		{
			vScoredOptions.push_back(OptionWithScore<TileChange>(TileChange(eLaborer), iValue));
		}
	}

	// sort the options by score
	std::stable_sort(vScoredOptions.begin(), vScoredOptions.end());

	bool bYieldsAlreadyIncluded = false;
	vector<TileChange> res;
	for (size_t i = 0; i < vScoredOptions.size(); i++)
	{
		if (bNoTwoOptionsWithSameYields && vScoredOptions[i].option.plot)
		{
			// if two plots have the same yields, include only one of them in the list
			bYieldsAlreadyIncluded = false;
			for (size_t j = 0; j < (size_t)i; j++)
			{
				// two plots with the same yields must also have the same score..
				if (vScoredOptions[j].option.plot && vScoredOptions[i].score == vScoredOptions[j].score)
				{
					if (GetPlotYields(vScoredOptions[j].option.plot, cache) == GetPlotYields(vScoredOptions[i].option.plot, cache))
					{
						bYieldsAlreadyIncluded = true;
						break;
					}
				}
			}
			if (bYieldsAlreadyIncluded)
			{
				continue;
			}
		}

		res.push_back(vScoredOptions[i].option);
		if ((int)res.size() >= iNumOptions)
			break;

	}

	return res;
}

void CvCityCitizens::DoApplyTileChange(TileChange tileChange, bool bAdd, bool bLogging)
{
	FILogFile* pLog = bLogging && GC.getLogging() ? LOGFILEMGR.GetLog("CityTileScorer.csv", FILogFile::kDontTimeStamp) : NULL;
	if (tileChange.plot)
	{
		if (bAdd == IsWorkingPlot(tileChange.plot))
		{
			// trying to add a plot we're already working? or to remove one we're not working?
			UNREACHABLE();
		}

		SetWorkingPlot(tileChange.plot, bAdd, CvCity::YIELD_UPDATE_LOCAL);
		if (pLog)
		{
			const char* logStr = (bAdd ? "now working" : "no longer working");
			int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
			CvString strOutBuf;
			strOutBuf.Format("%s plot %d:%d, current net food %d", logStr, tileChange.plot->getX(), tileChange.plot->getY(), iExcessFoodTimes100);
			pLog->Msg(strOutBuf);
		}
	}
	else if (tileChange.specialist != NO_SPECIALIST)
	{
		if (tileChange.specialist == (SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST))
		{
			// laborer
			ChangeNumDefaultSpecialists(bAdd ? 1 : -1, CvCity::YIELD_UPDATE_LOCAL);
			if (pLog)
			{
				const char* logStr = (bAdd ? "now working" : "no longer working");
				int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
				CvString strOutBuf;
				strOutBuf.Format("%s laborer, current net food % d", logStr, iExcessFoodTimes100);
				pLog->Msg(strOutBuf);
			}
		}
		else
		{
			// specialist. loop through the buildings to see where we can add/remove him
			bool bFoundBuilding = false;
			for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
			{
				const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
				CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

				if (pkBuildingInfo)
				{
					// Have this Building in the City?
					if (GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
					{
						if ((SpecialistTypes)pkBuildingInfo->GetSpecialistType() == tileChange.specialist)
						{
							// Found a free specialist slot
							if (bAdd && IsCanAddSpecialistToBuilding(eBuilding))
							{
								DoAddSpecialistToBuilding(eBuilding, /*bForced*/ false, CvCity::YIELD_UPDATE_LOCAL);
								bFoundBuilding = true;
								break;
							}
							else if (!bAdd && GetNumSpecialistsInBuilding(eBuilding) - GetNumForcedSpecialistsInBuilding(eBuilding) > 0)
							{
								DoRemoveSpecialistFromBuilding(eBuilding, /*bForced*/ false, CvCity::YIELD_UPDATE_LOCAL);
								bFoundBuilding = true;
								break;
							}
						}
					}
				}
			}
			if (!bFoundBuilding)
				UNREACHABLE();

			if (pLog)
			{
				const char* logStr = (bAdd ? "now working" : "no longer working");
				int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
				CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(tileChange.specialist);
				CvString strOutBuf;
				strOutBuf.Format("%s %s, current net food %d", logStr, pSpecialistInfo->GetDescription(), iExcessFoodTimes100);
				pLog->Msg(strOutBuf);
			}
		}
	}
	else
	{
		UNREACHABLE();
	}
}

/// Pick the best Plot to work from one of our unassigned pool
void CvCityCitizens::DoInitialAssigment(bool bAssumeStarving, bool bAssumeBelowGrowthThreshold, bool bLogging)
{
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
	int iNumToAssign = GetNumUnassignedCitizens();
	if (iNumToAssign == 0)
		return;

	gCachedNumbers.update(m_pCity);
	
	// score all possible options
	vector<TileChange> vScoredOptions = GetBestOptionsQuick(iNumToAssign, /*bAdd*/ true, /*bAllowOverride*/ !GET_PLAYER(GetOwner()).isHuman(), gCachedNumbers, bAssumeStarving, bAssumeBelowGrowthThreshold, gCachedNumbers.iNetGold < 0);

	// assign the best ones
	for (int i = 0; i < (int)vScoredOptions.size(); i++)
	{
		DoApplyTileChange(vScoredOptions[i], /*bAdd*/ true, bLogging);
	}	
	// fill up with laborers
	if (iNumToAssign > (int)vScoredOptions.size())
	{
		ChangeNumDefaultSpecialists(iNumToAssign - (int)vScoredOptions.size(), CvCity::YIELD_UPDATE_NONE);
	}
}

/// Pick the best Plot to work from one of our unassigned pool
bool CvCityCitizens::DoAddBestCitizenFromUnassigned(CvCity::eUpdateMode updateMode, bool bLogging, bool NoSpecialists)
{
	// We only assign the unassigned here, folks
	if (GetNumUnassignedCitizens() == 0)
		return false;

	//inside a loop, don't drop the global happiness!
	gCachedNumbers.update(m_pCity, updateMode==CvCity::YIELD_UPDATE_LOCAL);

	int iLaborerValue = GetSpecialistValue((SpecialistTypes)GD_INT_GET(DEFAULT_SPECIALIST), /*bAdd*/ true, gCachedNumbers);

	int iBestPlotValue = -1;
	CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, eBEST_UNWORKED_NO_OVERRIDE, gCachedNumbers, bLogging);

	bool bSpecialistForbidden = GET_PLAYER(GetOwner()).isHuman() && ( IsNoAutoAssignSpecialists() || NoSpecialists );
	FILogFile* pLog = bLogging && GC.getLogging() ? LOGFILEMGR.GetLog("CityTileScorer.csv", FILogFile::kDontTimeStamp) : NULL;

	int iSpecialistValue = -1;
	BuildingTypes eBestSpecialistBuilding = NO_BUILDING;

	if (!bSpecialistForbidden)
		eBestSpecialistBuilding = GetAIBestSpecialistBuilding(iSpecialistValue, gCachedNumbers, bLogging);

	if (iBestPlotValue > iSpecialistValue && iBestPlotValue > iLaborerValue)
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
	else if (iSpecialistValue >= iBestPlotValue && iSpecialistValue > 0) // does not include laborers
	{
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
		// Laborer if we can't do anything else or if they're better than both plots and specialists
		ChangeNumDefaultSpecialists(1, updateMode);

		if (pLog)
		{
			int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumptionTimes100());
			CvString strOutBuf;
			strOutBuf.Format("now working laborer, current net food %d", iExcessFoodTimes100);
			pLog->Msg(strOutBuf);
		}

		return false;
	}
}

/// Pick the worst Plot to stop working
bool CvCityCitizens::DoRemoveWorstCitizen(CvCity::eUpdateMode updateMode, bool bRemoveForcedStatus, SpecialistTypes eDontChangeSpecialist)
{
	int iCurrentCityPopulation = GetCity()->getPopulation(true);

	// Are all of our guys already not working Plots?
	if (GetNumUnassignedCitizens() == GetCity()->getPopulation(true))
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
			if (bRemoveForcedStatus)
			{
				ChangeNumForcedDefaultSpecialists(-1);
				ChangeNumDefaultSpecialists(-1, updateMode);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	// No Default Specialists, remove a working Pop, if there is one
	gCachedNumbers.update(m_pCity);
	int iWorstPlotValue = 0;
	CvPlot* pWorstPlot = GetBestCityPlotWithValue(iWorstPlotValue, bRemoveForcedStatus ? eWORST_WORKED_ANY : eWORST_WORKED_UNFORCED, gCachedNumbers);

	if (pWorstPlot != NULL)
	{
		// If we were force-working this Plot, turn it off
		if (bRemoveForcedStatus && IsForcedWorkingPlot(pWorstPlot))
		{
			SetWorkingPlot(pWorstPlot, false, updateMode); 
			SetForcedWorkingPlot(pWorstPlot, false);
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
		if (DoRemoveWorstSpecialist(eDontChangeSpecialist, bRemoveForcedStatus, NO_BUILDING, updateMode))
		{
			return true;
		}
	}

	return false;
}

/// Find a Plot the City is either working or not (depending on eMode), and the best value for it - highest positive score if we want to find a plot to be worked, highest (= closest to zero) negative score if we want to find a plot no longer to be worked
CvPlot* CvCityCitizens::GetBestCityPlotWithValue(int& iChosenValue, ePlotSelectionMode eMode, SPrecomputedExpensiveNumbers& cache, bool bLogging)
{
	int iBestPlotValue = -INT_MAX;
	bool bBestPlotIsForcedWork = false;
	CvPlot* pBestPlot = NULL;

	FILogFile* pLog = bLogging && GC.getLogging() ? LOGFILEMGR.GetLog("CityTileScorer.csv", FILogFile::kDontTimeStamp) : NULL;

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
				iValue = GetPlotValue(pLoopPlot, /*bAdd*/ true, cache);

				//if it's forced, don't even look at the value
				if (IsForcedWorkingPlot(iPlotLoop))
				{
					iChosenValue = iValue;
					return pLoopPlot;
				}

				if ( pBestPlot==NULL || iValue > iBestPlotValue )
				{
					iBestPlotValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
			break;
		case eBEST_UNWORKED_ALLOW_OVERRIDE:
			if (IsCanWorkWithOverride(pLoopPlot) && !IsWorkingPlot(iPlotLoop))
			{
				iValue = GetPlotValue(pLoopPlot, /*bAdd*/ true, cache);

				//if it's forced, don't even look at the value
				if (IsForcedWorkingPlot(iPlotLoop))
				{
					iChosenValue = iValue;
					return pLoopPlot;
				}

				if ( pBestPlot==NULL || iValue > iBestPlotValue )
				{
					iBestPlotValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
			break;
		case eWORST_WORKED_UNFORCED:
			if (IsWorkingPlot(iPlotLoop) && !IsForcedWorkingPlot(iPlotLoop))
			{
				iValue = GetPlotValue(pLoopPlot, /*bAdd*/ false, cache);
				if ( pBestPlot==NULL || iValue > iBestPlotValue )
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
				iValue = GetPlotValue(pLoopPlot, /*bAdd*/ false, cache);
				if ( pBestPlot==NULL || iValue > iBestPlotValue )
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
				iValue = GetPlotValue(pLoopPlot,  /*bAdd*/ false, cache);
				if ((pBestPlot == NULL || bSupersedesByUnforcedPriority) || (!bDisallowedByUnforcedPriority && iValue > iBestPlotValue))
				{
					iBestPlotValue = iValue;
					bBestPlotIsForcedWork = bIsForcedWork;
					pBestPlot = pLoopPlot;
				}
			}
			break;
		}

		if (false) // (pLog)
		{
			CvString strOutBuf;
			strOutBuf.Format("check index %d, plot %d:%d (%df%dp%dg%do), score %d with forced work status %d. current net food %d", 
				iPlotLoop, pLoopPlot->getX(), pLoopPlot->getY(), pLoopPlot->getYield(YIELD_FOOD), pLoopPlot->getYield(YIELD_PRODUCTION), 
				pLoopPlot->getYield(YIELD_GOLD), pBestPlot->getYield(YIELD_SCIENCE) + pBestPlot->getYield(YIELD_CULTURE) + pBestPlot->getYield(YIELD_FAITH), iValue, int(bBestPlotIsForcedWork), cache.iFoodRateTimes100 - cache.iFoodConsumptionTimes100);
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
	if (IsAvoidGrowth() || /*settler*/ m_pCity->isFoodProduction())
	{
		return 0;
	}
	else
	{
		CityAIFocusTypes eFocus = GetFocusType();
		if (eFocus == NO_CITY_AI_FOCUS_TYPE)
			return max(1000, 500 + 2 * m_pCity->getPopulation() * max(0, 70 - m_pCity->getPopulation()));
		else if (eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH || eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
			return 1000 + max(1000, 500 + 2 * m_pCity->getPopulation() * max(0, 70 - m_pCity->getPopulation()));
		else if (eFocus == CITY_AI_FOCUS_TYPE_FOOD)
			return m_pCity->getPopulation() * 150;

		//default (other specializations)
		return 200;
	}
}

//see if we can find a better assignment when we're not assigning plots greedily from scratch but from the final state where all citizens are already fed
void CvCityCitizens::OptimizeWorkedPlots(bool bLogging)
{
	if (CityStrategyAIHelpers::IsTestCityStrategy_IsPuppetAndAnnexable(m_pCity) || m_pCity->IsResistance())
		// no need to do this for non-Venice puppets or cities being razed
		return;

	int iCount = 0;
	FILogFile* pLog = bLogging && GC.getLogging() ? LOGFILEMGR.GetLog("CityTileScorer.csv", FILogFile::kDontTimeStamp) : NULL;
	bool bIsHuman = GET_PLAYER(GetOwner()).isHuman();
	bool bSpecialistForbidden = bIsHuman && IsNoAutoAssignSpecialists();

	if (pLog)
	{
		pLog->Msg("==== starting optimization");
		CvString strBaseString;
		CvString strOutBuf;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strOutBuf.Format("%s, focus %d%s, idle citizens %d",
			m_pCity->getName().GetCString(), m_eCityAIFocusTypes, IsForcedAvoidGrowth() ? " no growth" : "", GetNumUnassignedCitizens());
		strBaseString += strOutBuf;
		pLog->Msg(strBaseString);
	}

	gCachedNumbers.update(m_pCity);

	//failsafe: if we have unassigned citizens get then assign them first
	while (GetNumUnassignedCitizens() > 0)
		DoAddBestCitizenFromUnassigned(CvCity::YIELD_UPDATE_LOCAL);

	const int iNumOptionsPlots = 3;
	const int iNumOptionsSpecialists = 5;

	//failsafe against switching back and forth, don't try this too often
	while (iCount < max(5, m_pCity->getPopulation() / 2))
	{
		//now the real check. unassigning a tile might cause the valuation of the other tiles to change, so we have to consider combinations
		int iNetFood100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - m_pCity->foodConsumptionTimes100();
		bool bStarving = iNetFood100 < 0;
		bool bBelowGrowthThreshold = iNetFood100 < GetExcessFoodThreshold100();
		bool bInDebt = gCachedNumbers.iNetGold < 0;
		
		bool bAllowOverride = !GET_PLAYER(GetOwner()).isHuman();
		vector<TileChange> vRemoveOptions = GetBestOptionsQuick(iNumOptionsPlots, /*bAdd*/ false, bAllowOverride, gCachedNumbers, bStarving, bBelowGrowthThreshold, bInDebt, /*bIncludePlots*/ true, /*bIncludeSpecialists*/ false, true);
		if (!bSpecialistForbidden)
		{
			vector<TileChange> tmp = GetBestOptionsQuick(iNumOptionsSpecialists, /*bAdd*/ false, bAllowOverride, gCachedNumbers, bStarving, bBelowGrowthThreshold, bInDebt, /*bIncludePlots*/ false, /*bIncludeSpecialists*/ true, true);
			vRemoveOptions.insert(vRemoveOptions.end(), tmp.begin(), tmp.end());
		}

		vector<TileChange> vAddOptions = GetBestOptionsQuick(iNumOptionsPlots, /*bAdd*/ true, bAllowOverride, gCachedNumbers, bStarving, bBelowGrowthThreshold, bInDebt, /*bIncludePlots*/ true, /*bIncludeSpecialists*/ false, true);
		if (!bSpecialistForbidden)
		{
			vector<TileChange> tmp = GetBestOptionsQuick(iNumOptionsSpecialists, /*bAdd*/ true, bAllowOverride, gCachedNumbers, bStarving, bBelowGrowthThreshold, bInDebt, /*bIncludePlots*/ false, /*bIncludeSpecialists*/ true, true);
			vAddOptions.insert(vAddOptions.end(), tmp.begin(), tmp.end());
		}
		int iBestScore = 0;
		int iBestOptionRemoveIndex = -1;
		int iBestOptionAddIndex = -1;

		vector<YieldAndGPPList> vRemoveYieldLists;
		vector<YieldAndGPPList> vAddYieldLists;
		for (int i = 0; i < (int)vRemoveOptions.size(); i++)
		{
			YieldAndGPPList list = (vRemoveOptions[i].plot) ? GetPlotYields(vRemoveOptions[i].plot, gCachedNumbers) : GetSpecialistYields(vRemoveOptions[i].specialist);
			vRemoveYieldLists.push_back(list.negative());
		}
		for (int i = 0; i < (int)vAddOptions.size(); i++)
		{
			YieldAndGPPList list = (vAddOptions[i].plot) ? GetPlotYields(vAddOptions[i].plot, gCachedNumbers) : GetSpecialistYields(vAddOptions[i].specialist);
			vAddYieldLists.push_back(list);
		}

		// now score all the combinations of yield changes
		for (int i = 0; i < (int)vRemoveOptions.size(); i++)
		{
			for (int j = 0; j < (int)vAddOptions.size(); j++)
			{
				YieldAndGPPList thisOptionYields = vRemoveYieldLists[i] + vAddYieldLists[j];
				if (thisOptionYields.isNoPositivePlotYield())
					// obviously a bad option
					continue;

				int iScore = ScoreYieldChange(thisOptionYields, gCachedNumbers);
				if (iScore > iBestScore)
				{
					// the initial value for iBestScore is 0: we accept only options with a positive score
					iBestOptionRemoveIndex = i;
					iBestOptionAddIndex = j;
					iBestScore = iScore;
				}
				/*if (bLogging)
				{
					CvString strRem;
					if (vRemoveOptions[i].plot)
						strRem.Format("remove plot %d:%d, ", vRemoveOptions[i].plot->getX(), vRemoveOptions[i].plot->getY());
					else
						strRem.Format("remove specialist %s, ", GC.getSpecialistInfo(vRemoveOptions[i].specialist)->GetDescription());

					CvString strAdd;
					if (vAddOptions[j].plot)
						strAdd.Format("add plot %d:%d, ", vAddOptions[j].plot->getX(), vAddOptions[j].plot->getY());
					else
						strAdd.Format("add specialist %s, ", GC.getSpecialistInfo(vAddOptions[j].specialist)->GetDescription());

					CvString strOutBuf;

					strOutBuf.Format("   score %d - option %s%s (%df %dp %dg %ds %do %dgpp). current net food %d", iScore,
						strRem.c_str(), strAdd.c_str(), thisOptionYields.yield[0], thisOptionYields.yield[1], thisOptionYields.yield[2], thisOptionYields.yield[3], thisOptionYields.yield[4] + thisOptionYields.yield[5],
						thisOptionYields.iNumSpecialists[0]+ thisOptionYields.iNumSpecialists[1] + thisOptionYields.iNumSpecialists[2] + thisOptionYields.iNumSpecialists[3] + thisOptionYields.iNumSpecialists[4] + thisOptionYields.iNumSpecialists[5] + thisOptionYields.iNumSpecialists[6] , gCachedNumbers.iFoodRateTimes100 - gCachedNumbers.iFoodConsumptionTimes100);
					pLog->Msg(strOutBuf);
				}*/
			}
		}

		// did we find anything?
		if (iBestOptionAddIndex >= 0)
		{
			DoApplyTileChange(vRemoveOptions[iBestOptionRemoveIndex], /*bRemove*/ false, true);
			DoApplyTileChange(vAddOptions[iBestOptionAddIndex], /*bRemove*/ true, true);
		}
		else
		{
			// no better option available
			break;
		}

		//inside a loop, update only the cached values affected by city tile selection
		gCachedNumbers.update(m_pCity, true);

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

	gCachedNumbers.update(m_pCity);

	int iRemoveWorstWorkedPlotValue = 0;
	GetBestCityPlotWithValue(iRemoveWorstWorkedPlotValue, eWORST_WORKED_UNFORCED, gCachedNumbers);

	//for simplicity we do not check for laborers here
	int iRemoveWorstSpecialistValue = 0;
	if (!GET_PLAYER(GetOwner()).isHuman() || !IsNoAutoAssignSpecialists())
	{
		GetAIWorstSpecialistCurrentlyInBuilding(iRemoveWorstSpecialistValue, gCachedNumbers);
	}

	int iRemoveWorstOptionValue = max(iRemoveWorstWorkedPlotValue, iRemoveWorstSpecialistValue);
	if (iRemoveWorstOptionValue == -INT_MAX)
	{
		// can't remove anything
		return false;
	}


	int iAddBestUnworkedPlotValue = 0;
	GetBestCityPlotWithValue(iAddBestUnworkedPlotValue, eBEST_UNWORKED_NO_OVERRIDE, gCachedNumbers);

	int iAddBestSpecialistValue = 0;
	if (!GET_PLAYER(GetOwner()).isHuman() || !IsNoAutoAssignSpecialists())
	{
		GetAIBestSpecialistBuilding(iAddBestSpecialistValue, gCachedNumbers);
	}

	int iAddBestOptionValue = max(iAddBestUnworkedPlotValue, iAddBestSpecialistValue);
	if (iAddBestOptionValue == -INT_MAX)
	{
		// can't add anything
		return false;
	}

	if (iAddBestOptionValue + iRemoveWorstOptionValue > 0)
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

bool CvCityCitizens::IsDirty() const
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
	{
		pLog->Msg("==== starting reallocation");
		CvString strBaseString;
		CvString strOutBuf;
		strBaseString.Format("%03d, ", GC.getGame().getElapsedGameTurns());
		strOutBuf.Format("%s, focus %d%s, idle citizens %d",
			m_pCity->getName().GetCString(), m_eCityAIFocusTypes, IsForcedAvoidGrowth() ? " no growth" : "", GetNumUnassignedCitizens());
		strBaseString += strOutBuf;
		pLog->Msg(strBaseString);
	}

	// initial assigment is quick and simple, we'll optimize it later
	DoInitialAssigment(false, false, bLogging);

	// are we below the growth threshold? retry the initial assigment with higher food valuation
	if (m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - m_pCity->foodConsumptionTimes100() < GetExcessFoodThreshold100())
	{
		if (pLog)
			pLog->Msg("==== repeating initial allocation with higher emphasis on food");
		DoInitialAssigment(false, true, bLogging);
	}
	// are we starving? 
	if (m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - m_pCity->foodConsumptionTimes100() < 0)
	{
		if (pLog)
			pLog->Msg("==== repeating initial allocation with even higher emphasis on food");
		DoInitialAssigment(true, true, bLogging);
	}
		

	// now we check if we can switch some of the tiles to optimize the result
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
	if (plotDistance(m_pCity->getX(), m_pCity->getY(), pPlot->getX(), pPlot->getY()) > MAX_CITY_RADIUS)
		return false;

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

	ASSERT_DEBUG(iIndex >= 0, "iIndex expected to be >= 0");

	int iPlotNum = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
	std::vector<int>::iterator it = std::find(m_vWorkedPlots.begin(), m_vWorkedPlots.end(), iPlotNum);
	if (bNewValue)
	{
		if (it == m_vWorkedPlots.end())
			m_vWorkedPlots.push_back(iPlotNum);
	}
	else
	{
		//do not reset the "forced working plot" status! we want to remember that
		if (it != m_vWorkedPlots.end())
			m_vWorkedPlots.erase(it);
	}

	ASSERT_DEBUG(iIndex < GetCity()->GetNumWorkablePlots(), "iIndex expected to be < NUM_CITY_PLOTS");

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
			// can't work the center tile of another city
			if (pPlot->isCity())
				return;

			if (IsCanWork(pPlot))
			{
				// If we're already working the Plot, then take the guy off and turn him into a Default Specialist
				if (IsWorkingPlot(pPlot))
				{
					SetWorkingPlot(pPlot, false, CvCity::YIELD_UPDATE_LOCAL);
					SetForcedWorkingPlot(pPlot, false);
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
			else if (pPlot->getOwner() == GetOwner() && !pPlot->isBlockaded(GetOwner()))
			{
				CvCity* pOC = pPlot->getOwningCity();

				// Can't take away plots from puppet cities by force unless venice (mainly because the AI doesn't have logic to do that)
				if (pOC->IsPuppet() && pOC->IsWithinWorkRange(pPlot) && !GET_PLAYER(GetOwner()).GetPlayerTraits()->IsNoAnnexing() )
					return;

				// Can't take away plots from cities which were just conquered
				if (pOC->IsResistance() || pOC->IsIgnoreCityForHappiness())
					return;

				pPlot->setOwningCityOverride(GetCity());
				// check if the city governor wants to work the new tile
				DoReallocateCitizens(true);
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

	ASSERT_DEBUG(iIndex >= 0, "iIndex expected to be >= 0");
	ASSERT_DEBUG(iIndex < GetCity()->GetNumWorkablePlots(), "iIndex expected to be < NUM_CITY_PLOTS");

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
	// when considering removing the worked status of a plot, GetPlotValue returns a negative score. find the plot with the best score (score closest to zero)
	int iBestPlotValue = -INT_MAX;
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
					int iValue = GetPlotValue(pLoopPlot, /*bAdd*/ false, gCachedNumbers);

					// First, or worst yet?
					if (iValue > iBestPlotValue)
					{
						iBestPlotValue = iValue;
						iBestPlotID = iPlotLoop;
					}
				}
			}
		}
	}

	if (iBestPlotID != -1)
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
	// Cannot work another city center
	if (pPlot->getPlotCity() != NULL && pPlot->getPlotCity()->GetID() != m_pCity->GetID())
	{
		return false;
	}

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

bool CvCityCitizens::AnyPlotBlockaded() const
{
	return !m_vBlockadedPlots.empty();
}

/// Check all Plots by this City to see if we can actually be working them (if we are)
bool CvCityCitizens::DoVerifyWorkingPlots()
{
	std::vector<int> oldBlocks = m_vBlockadedPlots;
	m_vBlockadedPlots.clear();
	
	for (int iI = 0; iI < GetCity()->GetNumWorkablePlots(); iI++)
	{
		CvPlot* pPlot = GetCityPlotFromIndex(iI);
		if (!pPlot || !pPlot->isEffectiveOwner(m_pCity))
			continue;

		//cache which plots are blockaded, the check can be expensive
		if (pPlot->isBlockaded(m_pCity->getOwner()))
			SetBlockaded(pPlot);
	}

	//re-assign all plots which are unavailable right now
	for (size_t iI = 0; iI < m_vBlockadedPlots.size(); iI++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndexUnchecked(m_vBlockadedPlots[iI]);
		if (IsWorkingPlot(pPlot) && !IsCanWork(pPlot))
		{
			SetWorkingPlot(pPlot, false, CvCity::YIELD_UPDATE_LOCAL);
			DoAddBestCitizenFromUnassigned(CvCity::YIELD_UPDATE_GLOBAL);
		}
	}

	//did anything change?
	return m_vBlockadedPlots != oldBlocks;
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
			iGPPChange = (pkSpecialistInfo->getGreatPeopleRateChange() + m_pCity->GetEventGPPFromSpecialists()) * iCount * 100;

			// GPP from Buildings
			iGPPChange += GetBuildingGreatPeopleRateChanges(eSpecialist) * 100;

			const CvReligion* pReligion = m_pCity->GetCityReligions()->GetMajorityReligion();
			if (pReligion)
			{
				iGPPChange += pReligion->m_Beliefs.GetGreatPersonPoints(GetGreatPersonFromSpecialist(eSpecialist), m_pCity->getOwner(), m_pCity, true) * 100;
			}

			if (MOD_BALANCE_CORE_RESOURCE_MONOPOLIES)
			{
				// GPP from resource monopolies
				GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);
				if (eGreatPerson != NO_GREATPERSON)
				{
					iGPPChange += GetPlayer()->getSpecificGreatPersonRateChangeFromMonopoly(eGreatPerson) * 100;
				}
			}

			if (iGPPChange > 0)
			{
				int iMod = 0;

				// City mod
				iMod += GetCity()->getGreatPeopleRateModifier();

				// Player mod
				iMod += GetPlayer()->getGreatPeopleRateModifier();

				iMod += GetCity()->GetSpecialistRateModifierFromBuildings(eSpecialist);

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
				else if (MOD_BALANCE_VP && (UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
				{
					iMod += GetPlayer()->getGreatDiplomatRateModifier();
				}

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

			if (MOD_EVENTS_CITY)
				GAMEEVENTINVOKE_HOOK(GAMEEVENT_CityPrepared, GetCity()->getOwner(), GetCity(), eSpecialist, iGPPChange, iGPThreshold);

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
					const UnitTypes eUnit = GET_PLAYER(GetCity()->getOwner()).GetSpecificUnitType(eUnitClass);
					if (eUnit != NO_UNIT)
					{
						DoSpawnGreatPerson(eUnit, true, false, false);
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
	ASSERT_DEBUG(eBuilding > -1);
	ASSERT_DEBUG(eBuilding < GC.getNumBuildingInfos());

	if (m_pCity->IsResistance() || m_pCity->IsRazing())
		return false;

	int iNumSpecialistsAssigned = GetNumSpecialistsInBuilding(eBuilding);

	if (iNumSpecialistsAssigned < GetCity()->getPopulation(true) && // Limit based on Pop of City
		iNumSpecialistsAssigned < GC.getBuildingInfo(eBuilding)->GetSpecialistCount() && // Limit for this particular Building
		iNumSpecialistsAssigned < /*4*/ GD_INT_GET(MAX_SPECIALISTS_FROM_BUILDING)) // Overall Limit
	{
		return true;
	}

	return false;
}

/// Adds and initializes a Specialist for this building
void CvCityCitizens::DoAddSpecialistToBuilding(BuildingTypes eBuilding, bool bForced, CvCity::eUpdateMode updateMode)
{
	ASSERT_DEBUG(eBuilding > -1);
	ASSERT_DEBUG(eBuilding < GC.getNumBuildingInfos());

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
				if (!DoRemoveWorstSpecialist(NO_SPECIALIST, bForced, eBuilding, updateMode))
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
			GetCity()->UpdateReligiousYieldFromSpecialist(true);
			GetCity()->UpdateAllNonPlotYields(updateMode==CvCity::YIELD_UPDATE_GLOBAL);
		}

		ICvUserInterface2* pkIFace = GC.GetEngineUserInterface();
		pkIFace->setDirty(GameData_DIRTY_BIT, true);
		pkIFace->setDirty(CityInfo_DIRTY_BIT, true);
		//pkIFace->setDirty(InfoPane_DIRTY_BIT, true );
		pkIFace->setDirty(CityScreen_DIRTY_BIT, true);
		pkIFace->setDirty(ColoredPlots_DIRTY_BIT, true);

		CvCity* pkCity = GetCity();
		CvInterfacePtr<ICvCity1> pCity = GC.WrapCityPointer(pkCity);

		pkIFace->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_SPECIALISTS);
	}
}

/// Removes and uninitializes a Specialist for this building
void CvCityCitizens::DoRemoveSpecialistFromBuilding(BuildingTypes eBuilding, bool bForced, CvCity::eUpdateMode updateMode)
{
	ASSERT_DEBUG(eBuilding > -1);
	ASSERT_DEBUG(eBuilding < GC.getNumBuildingInfos());

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
		if (m_aiNumSpecialistsInBuilding[eBuilding] < 0)
		{
			UNREACHABLE();
		}

		if (bForced)
		{
			m_aiNumForcedSpecialistsInBuilding[eBuilding]--;
			if (m_aiNumForcedSpecialistsInBuilding[eBuilding] < 0)
			{
				UNREACHABLE();
			}
		}

		GetCity()->processSpecialist(eSpecialist, -1, updateMode);

		//we removed the last specialist, this may have religious effects
		if (GetTotalSpecialistCount() == 0)
			GetCity()->UpdateReligiousYieldFromSpecialist(false);

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

		CvInterfacePtr<ICvCity1> pCity = GC.WrapCityPointer(GetCity());

		GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_SPECIALISTS);
	}
}

//	----------------------------------------------------------------------------
/// Clear EVERYONE from this Building
/// Any one in the building will be put in the unassigned citizens list.
/// It is up to the caller to reassign population.
void CvCityCitizens::DoRemoveAllSpecialistsFromBuilding(BuildingTypes eBuilding, CvCity::eUpdateMode updateMode)
{
	ASSERT_DEBUG(eBuilding > -1);
	ASSERT_DEBUG(eBuilding < GC.getNumBuildingInfos());

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

		CvInterfacePtr<ICvCity1> pCity = GC.WrapCityPointer(GetCity());
		GC.GetEngineUserInterface()->SetSpecificCityInfoDirty(pCity.get(), CITY_UPDATE_TYPE_SPECIALISTS);
	}

	//we removed the last specialist, this may have religious effects
	if (iNumSpecialists>0 && GetTotalSpecialistCount() == 0)
		GetCity()->UpdateReligiousYieldFromSpecialist(false);

	GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();
	GetCity()->updateNetHappiness();
}


/// Find the worst Specialist and remove him from duty
bool CvCityCitizens::DoRemoveWorstSpecialist(SpecialistTypes eDontChangeSpecialist, bool bForced, const BuildingTypes eDontRemoveFromBuilding, CvCity::eUpdateMode updateMode)
{
	int iWorstValue = -INT_MAX;
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
			if (bForced || GetNumSpecialistsInBuilding(eBuilding) - GetNumForcedSpecialistsInBuilding(eBuilding) > 0)
			{
				int iValue = GetSpecialistValue((SpecialistTypes)pkBuildingInfo->GetSpecialistType(), /*bAdd*/ false, gCachedNumbers);
				checked[specType] = iValue;

				if (iValue > iWorstValue)
				{
					iWorstValue = iValue;
					eWorstType = eBuilding;
				}
			}
		}
	}

	if (eWorstType != NO_BUILDING)
	{
		DoRemoveSpecialistFromBuilding(eWorstType, bForced, updateMode);
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
		CUSTOMLOG("warning: implausible number of specialists in %s!\n",m_pCity->getName().c_str());

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
	ASSERT_DEBUG(eIndex > -1);
	ASSERT_DEBUG(eIndex < GC.getNumSpecialistInfos());

	return m_aiSpecialistCounts[eIndex];
}

#if defined(MOD_BALANCE_CORE)
int CvCityCitizens::GetSpecialistSlots(SpecialistTypes eIndex) const
{
	ASSERT_DEBUG(eIndex > -1);
	ASSERT_DEBUG(eIndex < GC.getNumSpecialistInfos());

	return m_aiSpecialistSlots[eIndex];
}
void CvCityCitizens::ChangeNumSpecialistSlots(SpecialistTypes eIndex, int iValue)
{
	ASSERT_DEBUG(eIndex > -1);
	ASSERT_DEBUG(eIndex < GC.getNumSpecialistInfos());

	m_aiSpecialistSlots[eIndex] += iValue;
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
		CUSTOMLOG("warning: implausible number of specialists in %s!\n",m_pCity->getName().c_str());

	return iNumSpecialists;
}

/// GPP changes from Buildings
int CvCityCitizens::GetBuildingGreatPeopleRateChanges(SpecialistTypes eSpecialist) const
{
	ASSERT_DEBUG(eSpecialist > -1);
	ASSERT_DEBUG(eSpecialist < GC.getNumSpecialistInfos());

	return m_piBuildingGreatPeopleRateChanges[eSpecialist];
}

/// Change GPP from Buildings
void CvCityCitizens::ChangeBuildingGreatPeopleRateChanges(SpecialistTypes eSpecialist, int iChange)
{
	ASSERT_DEBUG(eSpecialist > -1);
	ASSERT_DEBUG(eSpecialist < GC.getNumSpecialistInfos());

	m_piBuildingGreatPeopleRateChanges[eSpecialist] += iChange;
}

/// How much progress does this City have towards a Great Person from eIndex?
int CvCityCitizens::GetSpecialistGreatPersonProgress(SpecialistTypes eIndex) const
{
	ASSERT_DEBUG(eIndex > -1);
	ASSERT_DEBUG(eIndex < GC.getNumSpecialistInfos());

	return GetSpecialistGreatPersonProgressTimes100(eIndex) / 100;
}

/// How much progress does this City have towards a Great Person from eIndex? (in hundreds)
int CvCityCitizens::GetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex) const
{
	ASSERT_DEBUG(eIndex > -1);
	ASSERT_DEBUG(eIndex < GC.getNumSpecialistInfos());

	return m_aiSpecialistGreatPersonProgressTimes100[eIndex];
}

/// Change progress for this City towards a Great Person
void CvCityCitizens::ChangeSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex, int iChange, bool bCheckForSpawn)
{
	ASSERT_DEBUG(eIndex > -1);
	ASSERT_DEBUG(eIndex < GC.getNumSpecialistInfos());

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
					DoResetSpecialistGreatPersonProgressTimes100(eIndex, (iGPThreshold * 100));

					// Now... actually create the GP!
					const UnitClassTypes eUnitClass = (UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass();
					const UnitTypes eUnit = GET_PLAYER(GetOwner()).GetSpecificUnitType(eUnitClass);
					if (eUnit != NO_UNIT)
					{
						DoSpawnGreatPerson(eUnit, true, false, false);
					}
				}
			}
		}
	}
}

/// Reset Specialist progress
void CvCityCitizens::DoResetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex, int iAmountToRemove)
{
	ASSERT_DEBUG(eIndex > -1);
	ASSERT_DEBUG(eIndex < GC.getNumSpecialistInfos());
	m_aiSpecialistGreatPersonProgressTimes100[eIndex] -= iAmountToRemove;
}

/// How many Specialists are assigned to eBuilding?
int CvCityCitizens::GetNumSpecialistsInBuilding(BuildingTypes eBuilding) const
{
	ASSERT_DEBUG(eBuilding > -1);
	ASSERT_DEBUG(eBuilding < GC.getNumBuildingInfos());

	return m_aiNumSpecialistsInBuilding[eBuilding];
}

/// How many Forced Specialists are assigned to eBuilding?
int CvCityCitizens::GetNumForcedSpecialistsInBuilding(BuildingTypes eBuilding) const
{
	ASSERT_DEBUG(eBuilding > -1);
	ASSERT_DEBUG(eBuilding < GC.getNumBuildingInfos());

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
int CvCityCitizens::GetSpecialistUpgradeThreshold(UnitClassTypes eUnitClass) const
{
	int iThreshold = /*100 in CP, 150 in VP*/ GD_INT_GET(GREAT_PERSON_THRESHOLD_BASE);
	int iNumCreated = 0;
	CvPlayer& kPlayer = GET_PLAYER(m_pCity->getOwner());

	if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER", true))
	{
		iNumCreated = kPlayer.getGreatWritersCreated(MOD_GLOBAL_TRULY_FREE_GP);
		iThreshold -= /*0*/ GD_INT_GET(GWAM_THRESHOLD_DECREASE);
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST", true))
	{
		iNumCreated = kPlayer.getGreatArtistsCreated(MOD_GLOBAL_TRULY_FREE_GP);
		iThreshold -= /*0*/ GD_INT_GET(GWAM_THRESHOLD_DECREASE);
	}
	else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN", true))
	{
		iNumCreated = kPlayer.getGreatMusiciansCreated(MOD_GLOBAL_TRULY_FREE_GP);
		iThreshold -= /*0*/ GD_INT_GET(GWAM_THRESHOLD_DECREASE);
	}
	else if (MOD_BALANCE_VP && eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
	{
		iNumCreated = kPlayer.getGreatDiplomatsCreated(MOD_GLOBAL_TRULY_FREE_GP);
	}
	else
	{
		if (MOD_GLOBAL_SEPARATE_GP_COUNTERS)
		{
			if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_MERCHANT", true))
			{
				iNumCreated = kPlayer.getGreatMerchantsCreated(MOD_GLOBAL_TRULY_FREE_GP);
			}
			else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST", true))
			{
				iNumCreated = kPlayer.getGreatScientistsCreated(MOD_GLOBAL_TRULY_FREE_GP);
			}
			else
			{
				iNumCreated = kPlayer.getGreatEngineersCreated(MOD_GLOBAL_TRULY_FREE_GP);
			}
		}
		else
		{
			iNumCreated = kPlayer.getGreatPeopleCreated(MOD_GLOBAL_TRULY_FREE_GP);
		}
	}

	const UnitTypes eThisPlayersUnitType = kPlayer.GetSpecificUnitType(eUnitClass);
	if (eThisPlayersUnitType != NO_UNIT)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eThisPlayersUnitType);
		if (pkUnitInfo != NULL)
		{
			if (pkUnitInfo->IsGPExtra() == 1)
			{
				iNumCreated = kPlayer.getGPExtra1Created(MOD_GLOBAL_TRULY_FREE_GP);
			}
			else if (pkUnitInfo->IsGPExtra() == 2)
			{
				iNumCreated = kPlayer.getGPExtra2Created(MOD_GLOBAL_TRULY_FREE_GP);
			}
			else if (pkUnitInfo->IsGPExtra() == 3)
			{
				iNumCreated = kPlayer.getGPExtra3Created(MOD_GLOBAL_TRULY_FREE_GP);
			}
			else if (pkUnitInfo->IsGPExtra() == 4)
			{
				iNumCreated = kPlayer.getGPExtra4Created(MOD_GLOBAL_TRULY_FREE_GP);
			}
			else if (pkUnitInfo->IsGPExtra() == 5)
			{
				iNumCreated = kPlayer.getGPExtra5Created(MOD_GLOBAL_TRULY_FREE_GP);
			}
		}
	}

	int iMod = kPlayer.GetPlayerTraits()->GetGreatPersonCostReduction(GetGreatPersonFromUnitClass(eUnitClass));
	if (iMod != 0)
	{
		iThreshold *= (100 + iMod);
		iThreshold /= 100;
	}

	// Increase threshold based on how many GP have already been spawned
	iThreshold += (/*100 in CP, 250 in VP*/ GD_INT_GET(GREAT_PERSON_THRESHOLD_INCREASE) * iNumCreated);

	// Game Speed mod
	iThreshold *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	// Start era mod
	iThreshold *= GC.getGame().getStartEraInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	// Difficulty level
	if (kPlayer.isMajorCiv())
	{
		iThreshold *= kPlayer.getHandicapInfo().getGreatPeoplePercent();
		iThreshold /= 100;

		if (!kPlayer.isHuman())
		{
			iThreshold *= GC.getGame().getHandicapInfo().getAIGreatPeoplePercent();
			iThreshold /= 100;
		}
	}

	return iThreshold;
}

/// Create a GP!
void CvCityCitizens::DoSpawnGreatPerson(UnitTypes eUnit, bool bIncrementCount, bool bCountAsProphet, bool bIsFree)
{
	ASSERT_DEBUG(eUnit != NO_UNIT);

	if (GC.getUnitInfo(eUnit)->IsFoundReligion() && GC.getGame().isOption(GAMEOPTION_NO_RELIGION))
		return;

	// Not a great person
	if (GC.getUnitInfo(eUnit)->GetSpecialUnitType() != static_cast<SpecialUnitTypes>(GC.getInfoTypeForString("SPECIALUNIT_PEOPLE")))
		return;

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

	newUnit->DoGreatPersonSpawnBonus(GetCity());

	if (bIncrementCount)
		kPlayer.incrementGreatPersonCount(newUnit->getUnitInfo(), bCountAsProphet, bIsFree);

	// Setup prophet properly
	if (newUnit->getUnitInfo().IsFoundReligion())
	{
		ReligionTypes eReligion = kPlayer.GetReligions()->GetOwnedReligion();
		newUnit->GetReligionDataMutable()->SetFullStrength(kPlayer.GetID(), newUnit->getUnitInfo(),eReligion);
	}

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
	case NO_CITY_AI_FOCUS_TYPE:
		eTargetYield = NO_YIELD;
		break;
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
	case CITY_AI_FOCUS_TYPE_GREAT_PEOPLE:
	case CITY_AI_FOCUS_TYPE_PROD_GROWTH:
	case CITY_AI_FOCUS_TYPE_GOLD_GROWTH:
		break; // No yield to map too.
	}

	return eTargetYield;
}

SPrecomputedExpensiveNumbers::SPrecomputedExpensiveNumbers() :
	bNeedUpdate(true),
	bonusForXFeature(YIELD_TOURISM, vector<int>(GC.getNumFeatureInfos(), INT_MAX)),
	bonusForXTerrain(YIELD_TOURISM, vector<int>(GC.getNumTerrainInfos(), INT_MAX)),
	iSpecialistGPPRates(vector<int>(GC.getNumSpecialistInfos(), INT_MAX)),
	bAnySpecialistInOtherCity(vector<bool>(GC.getNumSpecialistInfos(), false)),
	iYieldChangeAnySpecialist(YIELD_TOURISM, INT_MAX),
	iDistress(0),
	iPoverty(0),
	iIlliteracy(0),
	iBoredom(0),
	iUrbanization(0),
	iGrowthMod(0),
	iLocalHappiness(0),
	iLocalUnhappiness(0),
	iHappinessOtherCities(0),
	iUnhappinessOtherCities(0),
	iBasicNeedsRateChangeForReducedDistress(0),
	iGoldRateChangeForReducedPoverty(0),
	iScienceRateChangeForReducedIlliteracy(0),
	iCultureRateChangeForReducedBoredom(0),
	iBasicNeedsRateChangeForIncreasedDistress(0),
	iGoldRateChangeForIncreasedPoverty(0),
	iScienceRateChangeForIncreasedIlliteracy(0),
	iCultureRateChangeForIncreasedBoredom(0),
	iOtherUnhappiness(0),
	iFoodRateTimes100(0),
	iFoodConsumptionTimes100(0),
	iFoodConsumptionAssumeNoReductionNonSpecialistsTimes100(0),
	iFoodCorpMod(0),
	bWantArt(false),
	bWantScience(false),
	bWantDiplo(false),
	iNetGold(0)
{
}

void SPrecomputedExpensiveNumbers::update(CvCity* pCity, bool bInsideLoop)
{
	CvPlayer& kPlayer = GET_PLAYER(pCity->getOwner());

	iGrowthMod = pCity->getGrowthMods();
	iFoodRateTimes100 = pCity->getYieldRateTimes100(YIELD_FOOD, false);
	iFoodConsumptionTimes100 = pCity->foodConsumptionTimes100();
	iFoodConsumptionAssumeNoReductionNonSpecialistsTimes100 = pCity->foodConsumptionTimes100(false, 0, true);

	if (MOD_BALANCE_VP)
	{
		iFamine = pCity->GetUnhappinessFromFamine();
		iDistress = pCity->GetDistress(false);
		iPoverty = pCity->GetPoverty(false);
		iIlliteracy = pCity->GetIlliteracy(false);
		iBoredom = pCity->GetBoredom(false);
		iUrbanization = pCity->getUnhappinessFromSpecialists(pCity->GetCityCitizens()->GetTotalSpecialistCount());
	}
	iLocalUnhappiness = pCity->GetUnhappinessAggregated();
	iNetGold = kPlayer.GetTreasury()->CalculateBaseNetGold();

	// if we're inside a loop, we don't need to update all the values, as some of them don't change while in the loop
	if (!bInsideLoop || bNeedUpdate)
	{
		bNeedUpdate = false;
		iLocalHappiness = pCity->GetLocalHappiness();
		iHappinessOtherCities = kPlayer.GetHappinessFromCitizenNeeds() - iLocalHappiness;
		iUnhappinessOtherCities = kPlayer.GetUnhappinessFromCitizenNeeds() - iLocalUnhappiness;
		iOtherUnhappiness = MOD_BALANCE_VP ? pCity->GetUnhappinessFromIsolation() + pCity->GetUnhappinessFromOccupation() + pCity->GetUnhappinessFromPillagedTiles() + pCity->GetUnhappinessFromReligiousUnrest() : 0;
		iFoodCorpMod = pCity->GetTradeRouteCityMod(YIELD_FOOD);

		for (size_t i = 0; i < iYieldChangeAnySpecialist.size(); i++)
		{
			iYieldChangeAnySpecialist[i] = 0;
			ReligionTypes eMajority = pCity->GetCityReligions()->GetReligiousMajority();
			if (eMajority >= RELIGION_PANTHEON)
			{
				const CvReligion* pReligion = pCity->GetCityReligions()->GetMajorityReligion();
				if (pReligion)
				{
					iYieldChangeAnySpecialist[i] = pReligion->m_Beliefs.GetYieldChangeAnySpecialist((YieldTypes)i, pCity->getOwner(), pCity);
				}
			}
		}

		bAnySpecialistInOtherCity.resize(GC.getNumSpecialistInfos());
		for (size_t i = 0; i < bAnySpecialistInOtherCity.size(); i++)
		{
			bAnySpecialistInOtherCity[i] = false;
			int iLoop = 0;
			for (CvCity* pLoopCity = kPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kPlayer.nextCity(&iLoop))
			{
				if (pLoopCity == pCity)
					continue;

				if (pLoopCity->GetCityCitizens()->GetSpecialistCount((SpecialistTypes)i) > 0)
				{
					bAnySpecialistInOtherCity[i] = true;
					break;
				}
			}
		}

		//just reset this, we update it on demand
		iSpecialistGPPRates.resize(GC.getNumSpecialistInfos());
		for (size_t i = 0; i < iSpecialistGPPRates.size(); i++)
			iSpecialistGPPRates[i] = INT_MAX;

		bonusForXFeature.resize(YIELD_TOURISM);
		for (size_t i = 0; i < bonusForXFeature.size(); i++)
		{
			bonusForXFeature[i].resize(GC.getNumFeatureInfos());
			for (size_t j = 0; j < bonusForXFeature[i].size(); j++)
				bonusForXFeature[i][j] = INT_MAX;
		}

		bonusForXTerrain.resize(YIELD_TOURISM);
		for (size_t i = 0; i < bonusForXTerrain.size(); i++)
		{
			bonusForXTerrain[i].resize(GC.getNumTerrainInfos());
			for (size_t j = 0; j < bonusForXTerrain[i].size(); j++)
				bonusForXTerrain[i][j] = INT_MAX;
		}

		if (kPlayer.isHuman())
		{
			bWantArt = false;
			bWantScience = false;
			bWantDiplo = false;
		}
		else
		{
			bWantArt = kPlayer.GetPlayerTraits()->IsTourism() || kPlayer.GetDiplomacyAI()->IsGoingForCultureVictory() || kPlayer.GetDiplomacyAI()->IsCloseToCultureVictory();
			bWantScience = kPlayer.GetPlayerTraits()->IsNerd() || kPlayer.GetDiplomacyAI()->IsGoingForSpaceshipVictory() || kPlayer.GetDiplomacyAI()->IsCloseToSpaceshipVictory();
			bWantDiplo = kPlayer.GetPlayerTraits()->IsDiplomat() || kPlayer.GetDiplomacyAI()->IsGoingForDiploVictory() || kPlayer.GetDiplomacyAI()->IsCloseToDiploVictory();
		}
	}

	if (pCity->IsPuppet() || pCity->IsResistance() || pCity->IsRazing() || !MOD_BALANCE_VP)
	{
		iBasicNeedsRateChangeForReducedDistress = 0;
		iGoldRateChangeForReducedPoverty = 0;
		iScienceRateChangeForReducedIlliteracy = 0;
		iCultureRateChangeForReducedBoredom = 0;
		iBasicNeedsRateChangeForIncreasedDistress = 0;
		iGoldRateChangeForIncreasedPoverty = 0;
		iScienceRateChangeForIncreasedIlliteracy = 0;
		iCultureRateChangeForIncreasedBoredom = 0;
	}
	else
	{
		// the smallest possible increase in yield rate that would reduce unhappiness from the respective need by 1
		// (see the calculations in GetDistress etc.)
		iBasicNeedsRateChangeForReducedDistress = iDistress > 0 ? (int)(pCity->GetBasicNeedsMedian(false, 0) * (pCity->getPopulation() - pCity->GetDistressRaw(false) + 1) - (pCity->getYieldRateTimes100(YIELD_FOOD, false, false) + pCity->getYieldRateTimes100(YIELD_PRODUCTION, false, false))) : INT_MAX;
		iGoldRateChangeForReducedPoverty = iPoverty > 0 ? (int)(pCity->GetGoldMedian(false, 0) * (pCity->getPopulation() - pCity->GetPovertyRaw(false) + 1) - pCity->getYieldRateTimes100(YIELD_GOLD, false, false)) : INT_MAX;
		iScienceRateChangeForReducedIlliteracy = iIlliteracy > 0 ? (int)(pCity->GetScienceMedian(false, 0) * (pCity->getPopulation() - pCity->GetIlliteracyRaw(false) + 1) - pCity->getYieldRateTimes100(YIELD_SCIENCE, false, false)) : INT_MAX;
		iCultureRateChangeForReducedBoredom = iBoredom > 0 ? (int)(pCity->GetCultureMedian(false, 0) * (pCity->getPopulation() - pCity->GetBoredomRaw(false) + 1) - pCity->getJONSCulturePerTurn(false) * 100) : INT_MAX;

		// the smallest possible decrease in yield rate that would increase unhappiness from the respective need by 1
		iBasicNeedsRateChangeForIncreasedDistress = -INT_MAX;
		iBasicNeedsRateChangeForIncreasedDistress = -INT_MAX;
		iBasicNeedsRateChangeForIncreasedDistress = -INT_MAX;
		iBasicNeedsRateChangeForIncreasedDistress = -INT_MAX;
		int iLimit = MOD_BALANCE_CORE_UNCAPPED_UNHAPPINESS ? INT_MAX : pCity->getPopulation();
		if (iDistress < iLimit)
		{
			// for additional distress to count, it must get us over the flat reduction threshold
			int iNewDistressRaw = max(pCity->GetDistressRaw(false) + 1, pCity->GetDistressFlatReduction() + kPlayer.GetDistressFlatReductionGlobal() + 1);
			iBasicNeedsRateChangeForIncreasedDistress =(int)(pCity->GetBasicNeedsMedian(false, 0) * (pCity->getPopulation() - iNewDistressRaw + 1) - (pCity->getYieldRateTimes100(YIELD_FOOD, false, false) + pCity->getYieldRateTimes100(YIELD_PRODUCTION, false, false)));
		}
		if (iPoverty < iLimit)
		{
			int iNewPovertyRaw = max(pCity->GetPovertyRaw(false) + 1, pCity->GetPovertyFlatReduction() + kPlayer.GetPovertyFlatReductionGlobal() + 1);
			iGoldRateChangeForIncreasedPoverty = (int)(pCity->GetGoldMedian(false, 0) * (pCity->getPopulation() - iNewPovertyRaw + 1) - pCity->getYieldRateTimes100(YIELD_GOLD, false, false));
		}
		if (iIlliteracy < iLimit)
		{
			int iNewIlliteracyRaw = max(pCity->GetIlliteracyRaw(false) + 1, pCity->GetIlliteracyFlatReduction() + kPlayer.GetIlliteracyFlatReductionGlobal() + 1);
			iScienceRateChangeForIncreasedIlliteracy = (int)(pCity->GetScienceMedian(false, 0) * (pCity->getPopulation() - iNewIlliteracyRaw + 1) - pCity->getYieldRateTimes100(YIELD_SCIENCE, false, false));
		}
		if (iBoredom < iLimit)
		{
			int iNewBoredomRaw = max(pCity->GetBoredomRaw(false) + 1, pCity->GetBoredomFlatReduction() + kPlayer.GetBoredomFlatReductionGlobal() + 1);
			iCultureRateChangeForIncreasedBoredom = (int)(pCity->GetCultureMedian(false, 0) * (pCity->getPopulation() - iNewBoredomRaw + 1) - pCity->getJONSCulturePerTurn(false) * 100);
		}
	}
}



YieldAndGPPList::YieldAndGPPList() :
	yield(vector<int>(YIELD_TOURISM, 0)),
	iNumSpecialists(vector<int>(GC.getNumSpecialistInfos(), 0))
{
}

YieldAndGPPList YieldAndGPPList::negative()
{
	YieldAndGPPList tmp;
	for (int i = 0; i < YIELD_TOURISM; i++)
	{
		tmp.yield[i] = -yield[i];
	}
	for (int i = 0; i < GC.getNumSpecialistInfos(); i++)
	{
		tmp.iNumSpecialists[i] = -iNumSpecialists[i];
	}
	return tmp;
}

// changes for which all plot yields are negative can quickly be excluded
bool YieldAndGPPList::isNoPositivePlotYield()
{
	for (int i = 0; i < YIELD_TOURISM; i++)
	{
		if (yield[i] > 0)
			return false;
	}
	for (int i = 0; i < GC.getNumSpecialistInfos(); i++)
	{
		// only plot-only changes can return true 
		if (iNumSpecialists[i] != 0)
			return false;
	}
	return true;
}


YieldAndGPPList operator+(const YieldAndGPPList& lhs, const YieldAndGPPList& rhs)
{
	YieldAndGPPList tmp;
	for (int i = 0; i < YIELD_TOURISM; i++)
	{
		tmp.yield[i] = lhs.yield[i] + rhs.yield[i];
	}
	for (int i = 0; i < GC.getNumSpecialistInfos(); i++)
	{
		tmp.iNumSpecialists[i] = lhs.iNumSpecialists[i] + rhs.iNumSpecialists[i];
	}
	return tmp;
}


TileChange::TileChange() :
	plot(NULL),
	specialist(NO_SPECIALIST)
{
}

TileChange::TileChange(CvPlot* plot) :
	plot(plot),
	specialist(NO_SPECIALIST)
{
}

TileChange::TileChange(SpecialistTypes eSpecialist) :
	plot(NULL),
	specialist(eSpecialist)
{
}