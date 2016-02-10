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
	if(m_bInited)
	{
		SAFE_DELETE_ARRAY(m_aiSpecialistCounts);
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
	m_iNumUnassignedCitizens = 0;
	m_iNumCitizensWorkingPlots = 0;
	m_iNumForcedWorkingPlots = 0;

	m_eCityAIFocusTypes = NO_CITY_AI_FOCUS_TYPE;

	int iI;

	m_vWorkedPlots.clear();
	CvAssertMsg((0 < MAX_CITY_PLOTS),  "MAX_CITY_PLOTS is not greater than zero but an array is being allocated in CvCityCitizens::reset");
	for(iI = 0; iI < MAX_CITY_PLOTS; iI++)
	{
		m_pabWorkingPlot[iI] = false;
		m_pabForcedWorkingPlot[iI] = false;
	}

	m_iNumDefaultSpecialists = 0;
	m_iNumForcedDefaultSpecialists = 0;

	CvAssertMsg(m_aiSpecialistCounts==NULL, "about to leak memory, CvCityCitizens::m_aiSpecialistCounts");
	m_aiSpecialistCounts = FNEW(int[GC.getNumSpecialistInfos()], c_eCiv5GameplayDLL, 0);
	for(iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_aiSpecialistCounts[iI] = 0;
	}

	CvAssertMsg(m_aiSpecialistGreatPersonProgressTimes100==NULL, "about to leak memory, CvCityCitizens::m_aiSpecialistGreatPersonProgressTimes100");
	m_aiSpecialistGreatPersonProgressTimes100 = FNEW(int[GC.getNumSpecialistInfos()], c_eCiv5GameplayDLL, 0);
	for(iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_aiSpecialistGreatPersonProgressTimes100[iI] = 0;
	}

	CvAssertMsg(m_aiNumSpecialistsInBuilding==NULL, "about to leak memory, CvCityCitizens::m_aiNumSpecialistsInBuilding");
	m_aiNumSpecialistsInBuilding = FNEW(int[GC.getNumBuildingInfos()], c_eCiv5GameplayDLL, 0);
	for(iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_aiNumSpecialistsInBuilding[iI] = 0;
	}

	CvAssertMsg(m_aiNumForcedSpecialistsInBuilding==NULL, "about to leak memory, CvCityCitizens::m_aiNumForcedSpecialistsInBuilding");
	m_aiNumForcedSpecialistsInBuilding = FNEW(int[GC.getNumBuildingInfos()], c_eCiv5GameplayDLL, 0);
	for(iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_aiNumForcedSpecialistsInBuilding[iI] = 0;
	}

	CvAssertMsg(m_piBuildingGreatPeopleRateChanges==NULL, "about to leak memory, CvCityCitizens::m_piBuildingGreatPeopleRateChanges");
	m_piBuildingGreatPeopleRateChanges = FNEW(int[GC.getNumSpecialistInfos()], c_eCiv5GameplayDLL, 0);
	for(iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
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
	kStream >> m_iNumUnassignedCitizens;
	kStream >> m_iNumCitizensWorkingPlots;
	kStream >> m_iNumForcedWorkingPlots;

	kStream >> m_eCityAIFocusTypes;

	kStream >> 	m_bForceAvoidGrowth;

	kStream >> m_pabWorkingPlot;
	kStream >> m_pabForcedWorkingPlot;

	kStream >> m_iNumDefaultSpecialists;
	kStream >> m_iNumForcedDefaultSpecialists;

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiSpecialistCounts, GC.getNumSpecialistInfos());
	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_aiSpecialistGreatPersonProgressTimes100, GC.getNumSpecialistInfos());

	BuildingArrayHelpers::Read(kStream, m_aiNumSpecialistsInBuilding);
	BuildingArrayHelpers::Read(kStream, m_aiNumForcedSpecialistsInBuilding);

	CvInfosSerializationHelper::ReadHashedDataArray(kStream, m_piBuildingGreatPeopleRateChanges, GC.getNumSpecialistInfos());

	m_vWorkedPlots.clear();
	for (int i=0; i<MAX_CITY_PLOTS; i++)
	{
		if (m_pabWorkingPlot[i])
		{
			CvPlot* pPlot = GetCityPlotFromIndex(i);
			if (pPlot)
			{
				int iPlotNum = GC.getMap().plotNum(pPlot->getX(),pPlot->getY());
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
	kStream << m_iNumUnassignedCitizens;
	kStream << m_iNumCitizensWorkingPlots;
	kStream << m_iNumForcedWorkingPlots;

	kStream << m_eCityAIFocusTypes;

	kStream << 	m_bForceAvoidGrowth;

	kStream << m_pabWorkingPlot;
	kStream << m_pabForcedWorkingPlot;

	kStream << m_iNumDefaultSpecialists;
	kStream << m_iNumForcedDefaultSpecialists;

	CvInfosSerializationHelper::WriteHashedDataArray<SpecialistTypes, int>(kStream, m_aiSpecialistCounts, GC.getNumSpecialistInfos());
	CvInfosSerializationHelper::WriteHashedDataArray<SpecialistTypes, int>(kStream, m_aiSpecialistGreatPersonProgressTimes100, GC.getNumSpecialistInfos());

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
	if(pHomePlot != NULL)
	{
		bool bWorkPlot = IsCanWork(pHomePlot);
		SetWorkingPlot(pHomePlot, bWorkPlot, /*bUseUnassignedPool*/ false);
	}
}

/// Processed every turn
void CvCityCitizens::DoTurn()
{
	AI_PERF_FORMAT("City-AI-perf.csv", ("CvCityCitizens::DoTurn, Turn %03d, %s, %s", GC.getGame().getElapsedGameTurns(), m_pCity->GetPlayer()->getCivilizationShortDescription(), m_pCity->getName().c_str()) );
	DoVerifyWorkingPlots();

	CvPlayerAI& thisPlayer = GET_PLAYER(GetOwner());
#if defined(MOD_BALANCE_CORE)
	const OrderData* pOrderNode = m_pCity->headOrderQueueNode();
	CvUnitEntry* pkUnitInfo = NULL;
	CvBuildingClassInfo* pkBuildingClassInfo = NULL;
	bool bWonder = false;
	if(pOrderNode != NULL && pOrderNode->eOrderType == ORDER_TRAIN)
	{
		pkUnitInfo = GC.getUnitInfo((UnitTypes)pOrderNode->iData1);
	}
	else if (pOrderNode != NULL && pOrderNode->eOrderType == ORDER_CONSTRUCT)
	{
		CvBuildingEntry* pkOrderBuildingInfo = GC.getBuildingInfo((BuildingTypes)pOrderNode->iData1);

		if(pkOrderBuildingInfo)
		{
			const BuildingClassTypes eOrderBuildingClass = (BuildingClassTypes)pkOrderBuildingInfo->GetBuildingClassType();
			if(eOrderBuildingClass != NO_BUILDINGCLASS)
			{
				pkBuildingClassInfo = GC.getBuildingClassInfo(eOrderBuildingClass);
				if(pkBuildingClassInfo && pkBuildingClassInfo->getMaxGlobalInstances() == 1)
				{
					bWonder = true;
				}
			}
		}
	}
	if(m_pCity->IsPuppet())
	{
#if defined(MOD_UI_CITY_PRODUCTION)
		if(!(MOD_UI_CITY_PRODUCTION && thisPlayer.isHuman()))
		{
#endif
			SetFocusType(CITY_AI_FOCUS_TYPE_GOLD_GROWTH);
			SetNoAutoAssignSpecialists(false);
			SetForcedAvoidGrowth(false);
#if defined(MOD_UI_CITY_PRODUCTION)
		}
		if(!thisPlayer.isHuman())
		{
			SetFocusType(CITY_AI_FOCUS_TYPE_GOLD_GROWTH);
			SetNoAutoAssignSpecialists(false);
			SetForcedAvoidGrowth(false);
		}
#endif
	}
	else if(!thisPlayer.isHuman())
	{
		CitySpecializationTypes eWonderSpecializationType = thisPlayer.GetCitySpecializationAI()->GetWonderSpecialization();

		if((m_pCity->GetCityStrategyAI()->GetSpecialization() == eWonderSpecializationType) || bWonder)
		{
			SetFocusType(CITY_AI_FOCUS_TYPE_PRODUCTION);
			SetNoAutoAssignSpecialists(false);
			SetForcedAvoidGrowth(false);
		}
		else if(pkUnitInfo != NULL && pkUnitInfo->IsFound()) // we want production for settlers
		{
			SetFocusType(CITY_AI_FOCUS_TYPE_PRODUCTION);
			SetNoAutoAssignSpecialists(false);
			SetForcedAvoidGrowth(false);
		}
		else if(m_pCity->getPopulation() < 8 && !m_pCity->isCapital())  // we want a balanced growth
		{
			SetFocusType(NO_CITY_AI_FOCUS_TYPE);
			SetNoAutoAssignSpecialists(true);
			SetForcedAvoidGrowth(false);
		}
		else
		{
			// Are we running at a deficit?
			EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY", true);
			bool bInDeficit = false;
			if (eStrategyLosingMoney != NO_ECONOMICAISTRATEGY)
			{
				bInDeficit = thisPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);
			}

			EconomicAIStrategyTypes eStrategyBuildingReligion = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_DEVELOPING_RELIGION", true);
			bool bBuildingReligion = false;
			if (eStrategyBuildingReligion != NO_ECONOMICAISTRATEGY)
			{
				bBuildingReligion = thisPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyBuildingReligion);
				if(thisPlayer.GetReligions()->GetReligionCreatedByPlayer() == NO_RELIGION)
				{
					bBuildingReligion = false;
				}
			}
			
			ProcessTypes eProcess = m_pCity->getProductionProcess();
			ProjectTypes eProject = m_pCity->getProductionProject();
			if(bInDeficit)
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_GOLD);
				SetNoAutoAssignSpecialists(false);
				SetForcedAvoidGrowth(false);
			}
			else if((eProcess != NO_PROCESS) || (eProject != NO_PROJECT))
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_PRODUCTION);
				SetNoAutoAssignSpecialists(false);
				SetForcedAvoidGrowth(false);
			}
			else // no special cases? Alright, let's pick a function to follow...
			{
				bool bGPCity = false;
				AICityStrategyTypes eGoodGP = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_GOOD_GP_CITY");
				if(eGoodGP != NO_AICITYSTRATEGY)
				{
					bGPCity = m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eGoodGP);
					bGPCity = thisPlayer.GetDiplomacyAI()->IsGoingForCultureVictory();
				}
				SetNoAutoAssignSpecialists(false);
				SetForcedAvoidGrowth(false);
				CitySpecializationTypes eSpecialization = m_pCity->GetCityStrategyAI()->GetSpecialization();
				if(eSpecialization != -1)
				{
					CvCitySpecializationXMLEntry* pCitySpecializationEntry =  GC.getCitySpecializationInfo(eSpecialization);
					if(pCitySpecializationEntry)
					{
						YieldTypes eYield = pCitySpecializationEntry->GetYieldType();
						if(eYield == YIELD_FOOD)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_FOOD);
						}
						else if(eYield == YIELD_PRODUCTION)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_PRODUCTION);
						}
						else if(eYield == YIELD_GOLD)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_GOLD);
						}
						else if(eYield == YIELD_SCIENCE)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_SCIENCE);
						}
						else if(eYield == YIELD_FAITH && bBuildingReligion)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_FAITH);
						}
						else if(eYield == YIELD_CULTURE)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_CULTURE);
						}
						else if(bGPCity)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_GREAT_PEOPLE);
						}
						else
						{
							SetFocusType(NO_CITY_AI_FOCUS_TYPE);
						}
					}
					else
					{
						SetFocusType(NO_CITY_AI_FOCUS_TYPE);
					}
				}
				else
				{
					SetFocusType(NO_CITY_AI_FOCUS_TYPE);
				}
			}
		}
	}
	if(!thisPlayer.isHuman() && thisPlayer.IsEmpireVeryUnhappy())
	{
		int iMostUnhappy = 0;
		CvCity* pLoopCity;
		CvCity* pWorstCity = NULL;
		int iLoop = 0;
		for(pLoopCity = GET_PLAYER(thisPlayer.GetID()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(thisPlayer.GetID()).nextCity(&iLoop))
		{
			if(pLoopCity != NULL)
			{
				//mind the sign change
				int iUnhappiness = - pLoopCity->getHappinessDelta(); 

				if(iUnhappiness > iMostUnhappy)
				{
					pWorstCity = pLoopCity;
					iMostUnhappy = iUnhappiness;
				}
			}

		}
		if(pWorstCity != NULL && pWorstCity->GetID() == m_pCity->GetID())
		{
			SetForcedAvoidGrowth(true);
		}
	}
	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(), "Gameplay: More workers than population in the city.");

	DoReallocateCitizens();

	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(), "Gameplay: More workers than population in the city.");

	DoSpecialists();

	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(), "Gameplay: More workers than population in the city.");
#else
	if(m_pCity->IsPuppet())
	{
		SetFocusType(NO_CITY_AI_FOCUS_TYPE);
		//SetNoAutoAssignSpecialists(true);
		SetForcedAvoidGrowth(false);
	}
	else if(!thisPlayer.isHuman())
	{
		CitySpecializationTypes eWonderSpecializationType = thisPlayer.GetCitySpecializationAI()->GetWonderSpecialization();

		if(GC.getGame().getGameTurn() % 8 == 0)
		{
			SetFocusType(CITY_AI_FOCUS_TYPE_GOLD_GROWTH);
			SetNoAutoAssignSpecialists(true);
			SetForcedAvoidGrowth(false);
			int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
			if(iExcessFoodTimes100 < 200)
			{
				SetFocusType(NO_CITY_AI_FOCUS_TYPE);
				//SetNoAutoAssignSpecialists(true);
			}
		}
		if(m_pCity->isCapital() && !thisPlayer.isMinorCiv() && m_pCity->GetCityStrategyAI()->GetSpecialization() != eWonderSpecializationType)
		{
			SetFocusType(NO_CITY_AI_FOCUS_TYPE);
			SetNoAutoAssignSpecialists(false);
			SetForcedAvoidGrowth(false);
			int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
			if(iExcessFoodTimes100 < 400)
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_FOOD);
				//SetNoAutoAssignSpecialists(true);
			}
		}
		else if(m_pCity->GetCityStrategyAI()->GetSpecialization() == eWonderSpecializationType)
		{
			SetFocusType(CITY_AI_FOCUS_TYPE_PRODUCTION);
			SetNoAutoAssignSpecialists(false);
			//SetForcedAvoidGrowth(true);
			int iExcessFoodTimes100;// = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);
			//if (iExcessFoodTimes100 < 200)
			//{
			SetForcedAvoidGrowth(false);
			//}
			iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
			if(iExcessFoodTimes100 < 200)
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_PROD_GROWTH);
				//SetNoAutoAssignSpecialists(true);
				SetForcedAvoidGrowth(false);
			}
			iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
			if(iExcessFoodTimes100 < 200)
			{
				SetFocusType(NO_CITY_AI_FOCUS_TYPE);
				//SetNoAutoAssignSpecialists(true);
				SetForcedAvoidGrowth(false);
			}
		}
		else if(m_pCity->getPopulation() < 5)  // we want a balanced growth
		{
			SetFocusType(NO_CITY_AI_FOCUS_TYPE);
			//SetNoAutoAssignSpecialists(true);
			SetForcedAvoidGrowth(false);
		}
		else
		{
			// Are we running at a deficit?
			EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY", true);
			bool bInDeficit = false;
			if (eStrategyLosingMoney != NO_ECONOMICAISTRATEGY)
			{
				bInDeficit = thisPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);
			}

			if(bInDeficit)
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_GOLD_GROWTH);
				SetNoAutoAssignSpecialists(false);
				SetForcedAvoidGrowth(false);
				int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
				if(iExcessFoodTimes100 < 200)
				{
					SetFocusType(NO_CITY_AI_FOCUS_TYPE);
					//SetNoAutoAssignSpecialists(true);
				}
			}
			else if(GC.getGame().getGameTurn() % 3 == 0 && thisPlayer.GetGrandStrategyAI()->GetActiveGrandStrategy() == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE"))
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_CULTURE);
				SetNoAutoAssignSpecialists(true);
				SetForcedAvoidGrowth(false);
				int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
				if(iExcessFoodTimes100 < 200)
				{
					SetFocusType(NO_CITY_AI_FOCUS_TYPE);
					//SetNoAutoAssignSpecialists(true);
				}
			}
			else // we aren't a small city, building a wonder, or going broke
			{
				SetNoAutoAssignSpecialists(false);
				SetForcedAvoidGrowth(false);
				CitySpecializationTypes eSpecialization = m_pCity->GetCityStrategyAI()->GetSpecialization();
				if(eSpecialization != -1)
				{
					CvCitySpecializationXMLEntry* pCitySpecializationEntry =  GC.getCitySpecializationInfo(eSpecialization);
					if(pCitySpecializationEntry)
					{
						YieldTypes eYield = pCitySpecializationEntry->GetYieldType();
						if(eYield == YIELD_FOOD)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_FOOD);
						}
						else if(eYield == YIELD_PRODUCTION)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_PROD_GROWTH);
						}
						else if(eYield == YIELD_GOLD)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_GOLD);
						}
						else if(eYield == YIELD_SCIENCE)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_SCIENCE);
						}
						else
						{
							SetFocusType(NO_CITY_AI_FOCUS_TYPE);
						}
					}
					else
					{
						SetFocusType(NO_CITY_AI_FOCUS_TYPE);
					}
				}
				else
				{
					SetFocusType(NO_CITY_AI_FOCUS_TYPE);
				}
			}
		}
	}
	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(), "Gameplay: More workers than population in the city.");
	DoReallocateCitizens();
	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(), "Gameplay: More workers than population in the city.");
	DoSpecialists();

	CvAssertMsg((GetNumCitizensWorkingPlots() + GetTotalSpecialistCount() + GetNumUnassignedCitizens()) <= GetCity()->getPopulation(), "Gameplay: More workers than population in the city.");
#endif
}

/// What is the overall value of the current Plot?
#if defined(MOD_BALANCE_CORE)
int CvCityCitizens::GetPlotValue(CvPlot* pPlot, bool bUseAllowGrowthFlag, int iExcessFoodTimes100)
#else
int CvCityCitizens::GetPlotValue(CvPlot* pPlot, bool bUseAllowGrowthFlag)
#endif
{
	int iValue = 0;

	// Yield Values
	///////
	// Bonuses
	//////////
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;

		if(eYield == NO_YIELD)
			continue;

		int iYield = pPlot->getYield(eYield);
		if(iYield > 0)
		{
			if (m_pCity->GetCityStrategyAI()->GetDeficientYield() == eYield)
			{
				iValue *= 2;
			}
			CityAIFocusTypes eFocus = GetFocusType();
			if(eYield == YIELD_FOOD)
			{
				//Food is unique, so let's separate it out for now.
				bool bAvoidGrowth = IsAvoidGrowth();

				// Food can be worth less if we don't want to grow
				if(bUseAllowGrowthFlag && iExcessFoodTimes100 >= 0 && bAvoidGrowth)
				{
					// If we at least have enough Food to feed everyone, zero out the value of additional food
					iYield = 0;
				}
				// If our surplus is not at least 2, really emphasize food plots
				else if(!bAvoidGrowth)
				{
					if(iExcessFoodTimes100 <= 200)
					{
						iYield *= 10;
					}
					else
					{
						int iFoodNeeded = (m_pCity->growthThreshold() * 100);
						int iRemainder = 0;
						iRemainder = (max(iFoodNeeded, 1) / max((iExcessFoodTimes100 * 8), 1));
						if(eFocus == CITY_AI_FOCUS_TYPE_FOOD || eFocus == NO_CITY_AI_FOCUS_TYPE || eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH || eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
						{
							iYield *= max(1, iRemainder);
						}
						else
						{
							iYield *= max(1, (iRemainder / 4));
						}
					}
				}
				iYield *= GC.getAI_CITIZEN_VALUE_FOOD();
				if(eFocus == CITY_AI_FOCUS_TYPE_FOOD)
				{
					iYield *= 5;
				}
				else if(eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
				{
					iYield *= 4;
				}
				else if(eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
				{
					iYield *= 4;
				}			
			}
			else if(eYield == YIELD_PRODUCTION)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_PRODUCTION();
				if(eFocus == CITY_AI_FOCUS_TYPE_PRODUCTION)
				{
					iYield *= 5;
				}
				if(eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
				{
					iYield *= 4;
				}
			}
			else if(eYield == YIELD_GOLD)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_GOLD();
				if(eFocus == CITY_AI_FOCUS_TYPE_GOLD)
				{
					iYield *= 5;
				}
				if(eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
				{
					iYield *= 3;
				}
			}
			else if(eYield == YIELD_SCIENCE)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_SCIENCE();
				if(eFocus == CITY_AI_FOCUS_TYPE_SCIENCE)
				{
					iYield *= 5;
				}			
			}
			else if(eYield == YIELD_CULTURE || eYield == YIELD_TOURISM)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_CULTURE();
				if(eFocus == CITY_AI_FOCUS_TYPE_CULTURE)
				{
					iYield *= 5;
				}
			}
			else if(eYield == YIELD_FAITH || eYield == YIELD_GOLDEN_AGE_POINTS)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_FAITH();
				if(eFocus == CITY_AI_FOCUS_TYPE_FAITH)
				{
					iYield *= 5;
				}
			}
			else
			{
				iYield *= GC.getAI_CITIZEN_VALUE_FAITH();
			}
			iValue += iYield;
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
void CvCityCitizens::SetNoAutoAssignSpecialists(bool bValue)
{
	if(m_bNoAutoAssignSpecialists != bValue)
	{
		m_bNoAutoAssignSpecialists = bValue;

		// If we're giving the AI control clear all manually assigned Specialists
		if(!bValue)
		{
			DoClearForcedSpecialists();
		}

		DoReallocateCitizens();
	}
}

/// Is this City avoiding growth?
bool CvCityCitizens::IsAvoidGrowth()
{
	if(GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	if(GetPlayer()->IsEmpireSuperUnhappy())
#else
	if(GetPlayer()->GetExcessHappiness() < 0)
#endif
	{
		return true;
	}

	return IsForcedAvoidGrowth();
}

bool CvCityCitizens::IsForcedAvoidGrowth()
{
	return m_bForceAvoidGrowth;
}

void CvCityCitizens::SetForcedAvoidGrowth(bool bAvoidGrowth)
{
	if(m_bForceAvoidGrowth != bAvoidGrowth)
	{
		m_bForceAvoidGrowth = bAvoidGrowth;
		DoReallocateCitizens();
	}
}

/// What is this city focusing on?
CityAIFocusTypes CvCityCitizens::GetFocusType() const
{
	return m_eCityAIFocusTypes;
}

/// What is this city focusing on?
void CvCityCitizens::SetFocusType(CityAIFocusTypes eFocus)
{
	FAssert(eFocus >= NO_CITY_AI_FOCUS_TYPE);
	FAssert(eFocus < NUM_CITY_AI_FOCUS_TYPES);

	if(eFocus != m_eCityAIFocusTypes)
	{
		m_eCityAIFocusTypes = eFocus;
		// Reallocate with our new focus
		DoReallocateCitizens();
	}
}

/// Does the AI want a Specialist?
bool CvCityCitizens::IsAIWantSpecialistRightNow()
{
	int iWeight = 100;

	// If the City is Size 1 or 2 then we probably don't want Specialists
	if(m_pCity->getPopulation() < 3)
	{
		iWeight /= 2;
	}

	int iFoodPerTurn = m_pCity->getYieldRate(YIELD_FOOD, false);
	int iFoodEatenPerTurn = m_pCity->foodConsumption();
	int iSurplusFood = iFoodPerTurn - iFoodEatenPerTurn;

	CityAIFocusTypes eFocusType = GetFocusType();
	// Don't want specialists until we've met our food needs
	if(iSurplusFood < 0)
	{
		return false;
	}
	else if(IsAvoidGrowth() && (eFocusType == NO_CITY_AI_FOCUS_TYPE || eFocusType == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE))
	{
		iWeight *= 2;
	}
	else if(iSurplusFood <= 2)
	{
		iWeight /= 2;
	}
	else if(iSurplusFood > 2)
	{
		if(eFocusType == NO_CITY_AI_FOCUS_TYPE || eFocusType == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE || eFocusType == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
		{
			iWeight *= 100 + (20 * (iSurplusFood - 4));
			iWeight /= 100;
		}
	}

	// If we're deficient in Production then we're less likely to want Specialists
	if(m_pCity->GetCityStrategyAI()->IsYieldDeficient(YIELD_PRODUCTION))
	{
		iWeight *= 50;
		iWeight /= 100;
	}
	// if we've got some slackers in town (since they provide Production)
	else if(GetNumDefaultSpecialists() > 0 && eFocusType != CITY_AI_FOCUS_TYPE_PRODUCTION && eFocusType != CITY_AI_FOCUS_TYPE_PROD_GROWTH)
	{
		iWeight *= 150;
		iWeight /= 100;
	}
	// Someone told this AI it should be focused on something that is usually gotten from specialists
	if(eFocusType == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE)
	{
		// Loop through all Buildings
		BuildingTypes eBuilding;
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			eBuilding = (BuildingTypes) iBuildingLoop;

			// Have this Building in the City?
			if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if(IsCanAddSpecialistToBuilding(eBuilding))
				{
#if defined(MOD_BALANCE_CORE)
					iWeight *= 5;
#else
					iWeight *= 3;
#endif
					break;
				}
			}
		}
	}
	else if(eFocusType == CITY_AI_FOCUS_TYPE_CULTURE)
	{
		// Loop through all Buildings
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				// Have this Building in the City?
				if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if(IsCanAddSpecialistToBuilding(eBuilding))
					{
						const SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if(pSpecialistInfo && pSpecialistInfo->getCulturePerTurn() > 0)
						{
#if defined(MOD_BALANCE_CORE)
							iWeight *= 5;
#else
							iWeight *= 3;
#endif
							break;
						}
					}
				}
			}
		}
	}
	else if(eFocusType == CITY_AI_FOCUS_TYPE_SCIENCE)
	{
		// Loop through all Buildings
		BuildingTypes eBuilding;
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			eBuilding = (BuildingTypes) iBuildingLoop;

			// Have this Building in the City?
			if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if(IsCanAddSpecialistToBuilding(eBuilding))
				{
					SpecialistTypes eSpecialist = (SpecialistTypes) GC.getBuildingInfo(eBuilding)->GetSpecialistType();
					CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
					if(pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_SCIENCE) > 0)
					{
						iWeight *= 3;
					}

					if(GetPlayer()->getSpecialistExtraYield(YIELD_SCIENCE) > 0)
					{
						iWeight *= 3;
					}

					if(GetPlayer()->GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, YIELD_SCIENCE) > 0)
					{
						iWeight *= 3;
					}
				}
			}
		}
	}
	else if(eFocusType == CITY_AI_FOCUS_TYPE_PRODUCTION)
	{
		// Loop through all Buildings
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				// Have this Building in the City?
				if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if(IsCanAddSpecialistToBuilding(eBuilding))
					{
						SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if(NULL != pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_PRODUCTION) > 0)
						{
							iWeight *= 150;
							iWeight /= 100;
						}

						if(GetPlayer()->getSpecialistExtraYield(YIELD_PRODUCTION) > 0)
						{
							iWeight *= 2;
						}

						if(GetPlayer()->GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, YIELD_PRODUCTION) > 0)
						{
							iWeight *= 2;
						}
					}
				}
			}
		}
	}
	else if(eFocusType == CITY_AI_FOCUS_TYPE_GOLD)
	{
		// Loop through all Buildings
		BuildingTypes eBuilding;
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			eBuilding = (BuildingTypes) iBuildingLoop;

			// Have this Building in the City?
			if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if(IsCanAddSpecialistToBuilding(eBuilding))
				{
					SpecialistTypes eSpecialist = (SpecialistTypes) GC.getBuildingInfo(eBuilding)->GetSpecialistType();
					CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
					if(pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_GOLD) > 0)
					{
						iWeight *= 150;
						iWeight /= 100;
						break;
					}
				}
			}
		}
	}
	else if(eFocusType == CITY_AI_FOCUS_TYPE_FOOD)
	{
		iWeight *= 50;
		iWeight /= 100;
	}
	else if(eFocusType == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
	{
		// Loop through all Buildings
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				// Have this Building in the City?
				if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if(IsCanAddSpecialistToBuilding(eBuilding))
					{
						SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if(pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_PRODUCTION) > 0)
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
	else if(eFocusType == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
	{
		// Loop through all Buildings
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				// Have this Building in the City?
				if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if(IsCanAddSpecialistToBuilding(eBuilding))
					{
						SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if(pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_GOLD) > 0)
						{
							iWeight *= 150;
							iWeight /= 100;
						}

						if(GetPlayer()->getSpecialistExtraYield(YIELD_GOLD) > 0)
						{
							iWeight *= 2;
						}

						if(GetPlayer()->GetPlayerTraits()->GetSpecialistYieldChange(eSpecialist, YIELD_GOLD) > 0)
						{
							iWeight *= 2;
						}
					}
				}
			}
		}
	}
	else if(eFocusType == CITY_AI_FOCUS_TYPE_FAITH)
	{
		// Loop through all Buildings
		for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
		{
			const BuildingTypes eBuilding = (BuildingTypes) iBuildingLoop;
			CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
			if(pkBuildingInfo)
			{
				// Have this Building in the City?
				if(m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if(IsCanAddSpecialistToBuilding(eBuilding))
					{
						const SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if(pSpecialistInfo && pSpecialistInfo->getYieldChange(YIELD_FAITH) > 0)
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
#if defined(MOD_BALANCE_CORE)
	else if(m_pCity->GetPlayer()->isHalfSpecialistFoodCapital() && m_pCity->isCapital())
	{
		iWeight *= 150;
		iWeight /= 100;
	}
#endif

	// Does the AI want it enough?
	if(iWeight >= 150)
	{
		return true;
	}

	return false;
}

/// What is the Building Type the AI likes the Specialist of most right now?
BuildingTypes CvCityCitizens::GetAIBestSpecialistBuilding(int& iSpecialistValue, std::map<SpecialistTypes, int>& specialistValueCache)
{
	BuildingTypes eBestBuilding = NO_BUILDING;
	int iBestSpecialistValue = -1;
	int iBestUnmodifiedSpecialistValue = -1;

	SpecialistTypes eSpecialist;
	int iValue;

	// Loop through all Buildings
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if(pkBuildingInfo)
		{
			// Have this Building in the City?
			if(GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if(IsCanAddSpecialistToBuilding(eBuilding))
				{
					eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();

					std::map<SpecialistTypes, int>::iterator it = specialistValueCache.find(eSpecialist);
					if (it != specialistValueCache.end())
						iValue = specialistValueCache[eSpecialist];
					else
					{
						iValue = GetSpecialistValue(eSpecialist);
						specialistValueCache[eSpecialist] = iValue;
					}

					// Add a bit more weight to a Building if it has more slots (10% per).  This will bias the AI to fill a single building over spreading Specialists out
#if defined(MOD_BALANCE_CORE)
					int iTemp = ((GetNumSpecialistsAllowedByBuilding(*pkBuildingInfo) - 1) * iValue * 15);
#else
					int iTemp = ((GetNumSpecialistsAllowedByBuilding(*pkBuildingInfo) - 1) * iValue * 10);
#endif
					iTemp /= 100;
					iValue += iTemp;

					if(iValue > iBestSpecialistValue)
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

/// How valuable is eSpecialist?
int CvCityCitizens::GetSpecialistValue(SpecialistTypes eSpecialist)
{

	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if (pSpecialistInfo == NULL)
	{
		//This function should NEVER be called with an invalid specialist info type.
		CvAssert(pSpecialistInfo);
		return 0;
	}

	int iValue = 0;

	CvPlayer* pPlayer = m_pCity->GetPlayer();

	// factor in the fact that specialists may need less food
	int iFoodConsumptionBonus = (pPlayer->isHalfSpecialistFood()) ? 1 : 0;
	if(iFoodConsumptionBonus == 0 && m_pCity->isCapital())
	{
		iFoodConsumptionBonus = (pPlayer->isHalfSpecialistFoodCapital ()) ? 1 : 0;
	}
	CityAIFocusTypes eFocus = GetFocusType();
	//Calc food first (as it might end the function early)
	int iPenalty = 0;
	int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
	bool bAvoidGrowth = IsAvoidGrowth();
	if(iExcessFoodTimes100 < 0 && bAvoidGrowth)
	{
		return 0;
	}
	else if(iExcessFoodTimes100 <= 0 && !bAvoidGrowth)
	{
		return 0;
	}
	else if(iExcessFoodTimes100 > 0)
	{
		//Increase penalty based on function of excess food value and growth thresholds. 
		int iFoodNeeded = (m_pCity->growthThreshold() * 100);
		int iRemainder = 0;
		iRemainder = (max(iFoodNeeded, 1) / max((iExcessFoodTimes100 * 6), 1));
		if((eFocus == CITY_AI_FOCUS_TYPE_FOOD || eFocus == NO_CITY_AI_FOCUS_TYPE || eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH || eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH) && !bAvoidGrowth)
		{
			iPenalty += iRemainder;
		}
		else
		{
			iPenalty += (iRemainder / 4);
		}
		if(iExcessFoodTimes100 < 200 && !bAvoidGrowth)
		{
			iPenalty *= 10;
		}
	}
	if(iExcessFoodTimes100 >= 0 && bAvoidGrowth)
	{
		// If we at least have enough Food to feed everyone, zero out the value of additional food
		iPenalty = 0;
	}

	///////
	// Bonuses
	//////////
	for(int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;

		if(eYield == NO_YIELD)
			continue;

		int iYield = pPlayer->specialistYield(eSpecialist, eYield);
		//Culture is treated differently, sadly.
		if(eYield == YIELD_CULTURE)
		{
			iYield = m_pCity->GetCultureFromSpecialist(eSpecialist);
		}
		if(iYield > 0)
		{
			ReligionTypes eMajority = m_pCity->GetCityReligions()->GetReligiousMajority();
			if(eMajority >= RELIGION_PANTHEON)
			{
				const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, m_pCity->getOwner());
				if(pReligion)
				{
					iYield += pReligion->m_Beliefs.GetSpecialistYieldChange(eSpecialist, eYield);
					if(GetTotalSpecialistCount() <= 0 && pReligion->m_Beliefs.GetYieldChangeAnySpecialist(eYield) > 0)
					{
						iYield += (pReligion->m_Beliefs.GetYieldChangeAnySpecialist(eYield) * 2);
					}
					else if(pReligion->m_Beliefs.GetYieldFromGPUse(eYield) > 0)
					{
						iYield += (pReligion->m_Beliefs.GetYieldFromGPUse(eYield) / 2);
					}
				}
			}
			if (m_pCity->GetCityStrategyAI()->GetDeficientYield() == eYield)
			{
				iValue *= 2;
			}
			if(eYield == YIELD_FOOD)
			{
				iYield *= (GC.getAI_CITIZEN_VALUE_FOOD() + iFoodConsumptionBonus);
				if(eFocus == CITY_AI_FOCUS_TYPE_FOOD)
				{
					iYield *= 5;
				}
			}
			else if(eYield == YIELD_PRODUCTION)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_PRODUCTION();
				if(eFocus == CITY_AI_FOCUS_TYPE_PRODUCTION)
				{
					iYield *= 5;
				}
				if(eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
				{
					iYield *= 3;
				}
			}
			else if(eYield == YIELD_GOLD)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_GOLD();
				if(eFocus == CITY_AI_FOCUS_TYPE_GOLD)
				{
					iYield *= 5;
				}
				if(eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
				{
					iYield *= 3;
				}
			}
			else if(eYield == YIELD_SCIENCE)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_SCIENCE();
				if(eFocus == CITY_AI_FOCUS_TYPE_SCIENCE)
				{
					iYield *= 5;
				}			
			}
			else if(eYield == YIELD_CULTURE || eYield == YIELD_TOURISM)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_CULTURE();
				if(eFocus == CITY_AI_FOCUS_TYPE_CULTURE)
				{
					iYield *= 5;
				}
			}
			else if(eYield == YIELD_FAITH || eYield == YIELD_GOLDEN_AGE_POINTS)
			{
				iYield *= GC.getAI_CITIZEN_VALUE_FAITH();
				if(eFocus == CITY_AI_FOCUS_TYPE_FAITH)
				{
					iYield *= 5;
				}
			}
			else
			{
				iYield *= GC.getAI_CITIZEN_VALUE_FAITH();
			}
			iValue += iYield;
		}
	}
	int iGPPYieldValue = pSpecialistInfo->getGreatPeopleRateChange() * 3; // TODO: un-hardcode this
	if(eFocus == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE)
	{
		iGPPYieldValue *= 5;
	}
	bool bGPCity = false;
	AICityStrategyTypes eGoodGP = (AICityStrategyTypes) GC.getInfoTypeForString("AICITYSTRATEGY_GOOD_GP_CITY");
	if(eGoodGP != NO_AICITYSTRATEGY)
	{
		bGPCity = m_pCity->GetCityStrategyAI()->IsUsingCityStrategy(eGoodGP);
	}
	if(bGPCity)
	{
		iGPPYieldValue *= 3;
	}
	iValue += iGPPYieldValue;

	///////
	//Penalties
	//////////
	int iHappinessYieldValue = 0;
	if(GET_PLAYER(m_pCity->getOwner()).IsEmpireUnhappy())
	{
		CvCity* pAssumeCityAnnexed = NULL;
		CvCity* pAssumeCityPuppeted = NULL;
		if(m_pCity->IsPuppet())
		{
			pAssumeCityPuppeted = m_pCity;
		}
		if(m_pCity->IsOccupied() && !m_pCity->IsNoOccupiedUnhappiness())
		{
			pAssumeCityAnnexed = m_pCity;
		}

		iHappinessYieldValue = (GET_PLAYER(m_pCity->getOwner()).GetUnhappinessFromCitySpecialists(pAssumeCityAnnexed, pAssumeCityPuppeted) * 4);
	}

	iValue -= iHappinessYieldValue;
	iValue -= iPenalty;

	pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if(pSpecialistInfo)
	{
		int iFlavorGold = 0;
		int iFlavorScience = 0;
		int iFlavorCulture = 0;
		int iFlavorProduction = 0;
		int iFlavorDiplomacy = 0;
		if(GET_PLAYER(m_pCity->getOwner()).isHuman())
		{
			iFlavorGold = 8;
			iFlavorScience = 8;
			iFlavorCulture = 8;
			iFlavorProduction = 8;
			iFlavorDiplomacy = 8;
		}
		else
		{
			iFlavorGold = GET_PLAYER(m_pCity->getOwner()).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_GOLD"));
			iFlavorScience = GET_PLAYER(m_pCity->getOwner()).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_SCIENCE"));
			iFlavorCulture = GET_PLAYER(m_pCity->getOwner()).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_CULTURE"));
			iFlavorProduction = GET_PLAYER(m_pCity->getOwner()).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_PRODUCTION"));
			iFlavorDiplomacy = GET_PLAYER(m_pCity->getOwner()).GetGrandStrategyAI()->GetPersonalityAndGrandStrategy((FlavorTypes)GC.getInfoTypeForString("FLAVOR_DIPLOMACY"));
		}
		UnitClassTypes eUnitClass = (UnitClassTypes)pSpecialistInfo->getGreatPeopleUnitClass();
		if(eUnitClass != NO_UNITCLASS)
		{
			if(eUnitClass == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
			{
				int iEmptySlots = GET_PLAYER(m_pCity->getOwner()).GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_ART_ARTIFACT());
				if(iEmptySlots == 0)
				{
					iValue /= 2;
				}
				else
				{
					iValue += (iFlavorCulture * iEmptySlots * 2);
				}
			}
			else if(eUnitClass == GC.getInfoTypeForString("UNITCLASS_WRITER"))
			{
				int iEmptySlots = GET_PLAYER(m_pCity->getOwner()).GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_LITERATURE());
				if(iEmptySlots == 0)
				{
					iValue /= 2;
				}
				else
				{
					iValue += (iFlavorCulture * iEmptySlots * 2);
				}
			}
			else if(eUnitClass == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
			{
				int iEmptySlots = GET_PLAYER(m_pCity->getOwner()).GetCulture()->GetNumAvailableGreatWorkSlots(CvTypes::getGREAT_WORK_SLOT_MUSIC());
				if(iEmptySlots == 0)
				{
					iValue /= 2;
				}
				else
				{
					iValue += (iFlavorCulture * iEmptySlots * 2);
				}
			}
			else if(eUnitClass == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
			{
				iValue += (iFlavorProduction * 3);
			}
			else if(eUnitClass == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
			{
				iValue += (iFlavorGold * 3);
			}
			else if(eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
			{
				iValue += (iFlavorScience * 3);
			}
#if defined(MOD_DIPLOMACY_CITYSTATES)
			else if(MOD_DIPLOMACY_CITYSTATES && eUnitClass == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
			{
				iValue += (iFlavorDiplomacy * 3);
			}
#endif
		}
	}
	//Every 4 citizens should increase our desire for more specialists slightly.
	int iPop = m_pCity->getPopulation();
	iPop /= 4;

	iValue += iPop;
	return iValue;
}

/// Determine if eSpecialist is preferable to a default specialist, based on our focus
bool CvCityCitizens::IsBetterThanDefaultSpecialist(SpecialistTypes eSpecialist)
{
	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	CvAssertMsg(pSpecialistInfo, "Invalid specialist type when assigning citizens. Please send Anton your save file and version.");
	if(!pSpecialistInfo) return false; // Assumes that default specialist will work out

	SpecialistTypes eDefaultSpecialist = (SpecialistTypes) GC.getDEFAULT_SPECIALIST();
	CvSpecialistInfo* pDefaultSpecialistInfo = GC.getSpecialistInfo(eDefaultSpecialist);
	CvAssertMsg(pDefaultSpecialistInfo, "Invalid default specialist type when assigning citizens. Please send Anton your save file and version.");
	if(!pDefaultSpecialistInfo) return false;

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
#if defined(MOD_BALANCE_CORE)
	else if (m_pCity->isCapital() && m_pCity->GetPlayer()->isHalfSpecialistFoodCapital())
	{
		iSpecialistYield *= 2;
	}
#endif

	return (iSpecialistYield >= iDefaultSpecialistYield); // Unless default Specialist has strictly more, this Specialist is better
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
bool CvCityCitizens::DoAddBestCitizenFromUnassigned(std::map<SpecialistTypes, int>& specialistValueCache)
{
	// We only assign the unassigned here, folks
	if (GetNumUnassignedCitizens() == 0)
	{
		return false;
	}
#if defined(MOD_BALANCE_CORE)
	//FOUR STAGE SETUP:

	//FIRST, WE FEED OURSELVES!
	int iPotentialExcessTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption(false, 1) * 100);
	if(!IsAvoidGrowth() && iPotentialExcessTimes100 <= 200 || (IsAvoidGrowth() && iPotentialExcessTimes100 < 0))
	{
		int iBestPlotValue = 0;
		CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, /*bBest*/ true, /*bWorked*/ false);
		if(pBestPlot != NULL)
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
		if (!GET_PLAYER(GetOwner()).isHuman() || !IsNoAutoAssignSpecialists())
		{
			eBestSpecialistBuilding = GetAIBestSpecialistBuilding(iSpecialistValue,specialistValueCache);
		}

		int iBestPlotValue = 0;
		//GOOD PLOT? BAD PLOT? NYEH!
		CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, /*bBest*/ true, /*bWorked*/ false);
		if(pBestPlot != NULL)
		{
			bool bSpecialistBetterThanPlot = (eBestSpecialistBuilding != NO_BUILDING && iSpecialistValue >= iBestPlotValue && ((iPotentialExcessTimes100 > 0) || (IsAvoidGrowth() && iPotentialExcessTimes100 >= 0)));
			if (bSpecialistBetterThanPlot)
			{
				DoAddSpecialistToBuilding(eBestSpecialistBuilding, /*bForced*/ false);
				return true;
			}
			else
			{
				SetWorkingPlot(pBestPlot, true);
				return true;
			}
		}
		//THIRD, IF WE HAVE NO MORE GOOD PLOTS, WE DUMP EVERYTHING ELSE INTO SPECIALISTS UNLESS IT WOULD STARVE US
		else if(eBestSpecialistBuilding != NO_BUILDING && iPotentialExcessTimes100 >= 0)
		{
			DoAddSpecialistToBuilding(eBestSpecialistBuilding, /*bForced*/ false);
			return true;
		}
		//FOURTH, MANUAL LABOR FOREVER
		else
		{
			CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, /*bBest*/ true, /*bWorked*/ false);
			if(pBestPlot != NULL)
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
#else
	// First Specialist Pass
	int iSpecialistValue = 0;
	BuildingTypes eBestSpecialistBuilding = NO_BUILDING;
	if (!IsNoAutoAssignSpecialists())
	{
		eBestSpecialistBuilding = GetAIBestSpecialistBuilding(iSpecialistValue);
	}

	bool bBetterThanSlacker = false;
	if (eBestSpecialistBuilding != NO_BUILDING)
	{
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBestSpecialistBuilding);
		if(pkBuildingInfo)
		{
			SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
			// Must not be worse than a default Specialist for our focus!
			if (IsBetterThanDefaultSpecialist(eSpecialist))
			{
				bBetterThanSlacker = true;
			}
		}
	}

	int iBestPlotValue = 0;
	CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, /*bBest*/ true, /*bWorked*/ false);

	bool bSpecialistBetterThanPlot = (eBestSpecialistBuilding != NO_BUILDING && iSpecialistValue >= iBestPlotValue);

	// Is there a Specialist we can assign?
	if (bSpecialistBetterThanPlot && bBetterThanSlacker)
	{
		DoAddSpecialistToBuilding(eBestSpecialistBuilding, /*bForced*/ false);
		return true;
	}
	// Found a Valid Plot to place a guy?
	else if (!bSpecialistBetterThanPlot && pBestPlot != NULL)
	{
		// Now assign the guy to the best possible Plot
		SetWorkingPlot(pBestPlot, true);
		return true;
	}
	// No Valid Plots left - and no good specialists
	else
	{
		CvPlayer* pOwner = &GET_PLAYER(GetOwner());
		CvAssertMsg(pOwner, "Could not find owner of city when assigning citizens. Please send Anton your save file and version.");

		// Assign a cool Specialist! Only do this for AI players, or humans who do not have manual specialist control set
		if (pOwner)
		{
			if (!GET_PLAYER(GetOwner()).isHuman() || !IsNoAutoAssignSpecialists())
			{
				BuildingTypes eBestBuilding = GetAIBestSpecialistBuilding(iSpecialistValue);
				if(eBestBuilding != NO_BUILDING)
				{
					CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBestBuilding);
					if(pkBuildingInfo)
					{
						SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
						// Must not be worse than a default Specialist for our focus!
						if (IsBetterThanDefaultSpecialist(eSpecialist))
						{
							DoAddSpecialistToBuilding(eBestBuilding, false);
							return true;

						}
					}
				}
			}
		}

		// Default Specialist if we can't do anything else
		ChangeNumDefaultSpecialists(1);
	}
#endif
	return false;
}

/// Pick the worst Plot to stop working
bool CvCityCitizens::DoRemoveWorstCitizen(bool bRemoveForcedStatus, SpecialistTypes eDontChangeSpecialist, int iCurrentCityPopulation)
{
	if (iCurrentCityPopulation == -1)
	{
		iCurrentCityPopulation = GetCity()->getPopulation();
	}

	// Are all of our guys already not working Plots?
	if(GetNumUnassignedCitizens() == GetCity()->getPopulation())
	{
		return false;
	}

	// Find default Specialist to pull off, if there is one
	if(GetNumDefaultSpecialists() > 0)
	{
		// Do we either have unforced default specialists we can remove?
		if(GetNumDefaultSpecialists() > GetNumForcedDefaultSpecialists())
		{
			ChangeNumDefaultSpecialists(-1);
			return true;
		}
		if(GetNumDefaultSpecialists() > iCurrentCityPopulation)
		{
			ChangeNumForcedDefaultSpecialists(-1);
			ChangeNumDefaultSpecialists(-1);
			return true;
		}
	}

	// No Default Specialists, remove a working Pop, if there is one
	int iWorstPlotValue = 0;
	CvPlot* pWorstPlot = GetBestCityPlotWithValue(iWorstPlotValue, /*bBest*/ false, /*bWorked*/ true);

	if(pWorstPlot != NULL)
	{
#if defined(MOD_BALANCE_CORE)
		// If we were force-working this Plot, turn it off
		if(bRemoveForcedStatus && IsForcedWorkingPlot(pWorstPlot))
		{
			SetWorkingPlot(pWorstPlot, false);
			SetForcedWorkingPlot(pWorstPlot, false);

			return true;
		}
		else if(!IsForcedWorkingPlot(pWorstPlot))
		{
			SetWorkingPlot(pWorstPlot, false);
			return true;
		}
#else
		SetWorkingPlot(pWorstPlot, false);

		// If we were force-working this Plot, turn it off
		if(bRemoveForcedStatus)
		{
			if(IsForcedWorkingPlot(pWorstPlot))
			{
				SetForcedWorkingPlot(pWorstPlot, false);
			}
		}

		return true;
#endif
	}
	// Have to resort to pulling away a good Specialist
#if defined(MOD_BALANCE_CORE)
	else if(bRemoveForcedStatus)
#else
	else
#endif
	{
		if(DoRemoveWorstSpecialist(eDontChangeSpecialist))
		{
			return true;
		}
	}

	return false;
}

/// Find a Plot the City is either working or not, and the best/worst value for it - this function does "double duty" depending on what the user wants to find
CvPlot* CvCityCitizens::GetBestCityPlotWithValue(int& iValue, bool bWantBest, bool bWantWorked)
{
	bool bPlotForceWorked;

	int iBestPlotValue = -1;
	int iBestPlotID = -1;

	CvPlot* pLoopPlot;

#if defined(MOD_BALANCE_CORE)
	int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
#endif

	// Look at all workable Plots

	for(int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
	{
		if(iPlotLoop != CITY_HOME_PLOT)
		{
			pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

			if(pLoopPlot != NULL)
			{
				// Is this a Plot this City controls?
				if(pLoopPlot->getWorkingCity() != NULL && pLoopPlot->getWorkingCity()->GetID() == GetCity()->GetID())
				{
					// Working the Plot and wanting to work it, or Not working it and wanting to find one to work?
					if((IsWorkingPlot(pLoopPlot) && bWantWorked) ||
					        (!IsWorkingPlot(pLoopPlot) && !bWantWorked))
					{
						// Working the Plot or CAN work the Plot?
						if(bWantWorked || IsCanWork(pLoopPlot))
						{
#if defined(MOD_BALANCE_CORE)
							iValue = GetPlotValue(pLoopPlot, bWantBest, iExcessFoodTimes100);
#else
							iValue = GetPlotValue(pLoopPlot, bWantBest);
#endif

							bPlotForceWorked = IsForcedWorkingPlot(pLoopPlot);

							if(bPlotForceWorked)
							{
								// Looking for best, unworked Plot: Forced plots are FIRST to be picked
								if(bWantBest && !bWantWorked)
								{
									iValue += 10000;
								}
								// Looking for worst, worked Plot: Forced plots are LAST to be picked, so make it's value incredibly high
								if(!bWantBest && bWantWorked)
								{
#if defined(MOD_BALANCE_CORE)
									continue;
#else
									iValue += 10000;
#endif
								}
							}

							if(iBestPlotValue == -1 ||							// First Plot?
							        (bWantBest && iValue > iBestPlotValue) ||		// Best Plot so far?
							        (!bWantBest && iValue < iBestPlotValue))			// Worst Plot so far?
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

	if(iBestPlotID == -1)
	{
		return NULL;
	}

	return GetCityPlotFromIndex(iBestPlotID);
}

/// Optimize our Citizen Placement
void CvCityCitizens::DoReallocateCitizens()
{
	// Make sure we don't have more forced working plots than we have citizens working.  If so, clean it up before reallocating
	DoValidateForcedWorkingPlots();

	// Remove all of the allocated guys
	int iNumCitizensToRemove = GetNumCitizensWorkingPlots();
	for(int iWorkerLoop = 0; iWorkerLoop < iNumCitizensToRemove; iWorkerLoop++)
	{
		DoRemoveWorstCitizen();
	}

	int iSpecialistLoop;

	// Remove Non-Forced Specialists in Buildings
	int iNumSpecialistsToRemove;
	BuildingTypes eBuilding;
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		eBuilding = (BuildingTypes) iBuildingLoop;

		// Have this Building in the City?
		if(GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		{
			iNumSpecialistsToRemove = GetNumSpecialistsInBuilding(eBuilding) - GetNumForcedSpecialistsInBuilding(eBuilding);	// Don't include Forced guys

			// Loop through guys to remove (if there are any)
			for(iSpecialistLoop = 0; iSpecialistLoop < iNumSpecialistsToRemove; iSpecialistLoop++)
			{
				DoRemoveSpecialistFromBuilding(eBuilding, /*bForced*/ false);
			}
		}
	}

	// Remove Default Specialists
	int iNumDefaultsToRemove = GetNumDefaultSpecialists() - GetNumForcedDefaultSpecialists();
	for(iSpecialistLoop = 0; iSpecialistLoop < iNumDefaultsToRemove; iSpecialistLoop++)
	{
		ChangeNumDefaultSpecialists(-1);
	}

	// don't calculate specialist value over and over ...
	std::map<SpecialistTypes, int> specialistValueCache;

	// Now put all of the unallocated guys back
	int iNumToAllocate = GetNumUnassignedCitizens();
	for(int iUnallocatedLoop = 0; iUnallocatedLoop < iNumToAllocate; iUnallocatedLoop++)
	{
		DoAddBestCitizenFromUnassigned(specialistValueCache);
	}

	if(GET_PLAYER(GetCity()->getOwner()).isHuman() && GetCity()->getOwner() == GC.getGame().getActivePlayer())
	{
		GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();
	}
}



///////////////////////////////////////////////////
// Worked Plots
///////////////////////////////////////////////////



/// Is our City working a CvPlot?
bool CvCityCitizens::IsWorkingPlot(const CvPlot* pPlot) const
{
	int iIndex;

	iIndex = GetCityIndexFromPlot(pPlot);

	if(iIndex != -1)
	{
		return m_pabWorkingPlot[iIndex];
	}

	return false;
}

/// Tell a City to start or stop working a Plot.  Citizens will go to/from the Unassigned Pool if the 3rd argument is true
void CvCityCitizens::SetWorkingPlot(CvPlot* pPlot, bool bNewValue, bool bUseUnassignedPool)
{
	int iI;

	int iIndex = GetCityIndexFromPlot(pPlot);

	CvAssertMsg(iIndex >= 0, "iIndex expected to be >= 0");

	int iPlotNum = GC.getMap().plotNum(pPlot->getX(), pPlot->getY());
	std::vector<int>::iterator it = std::find( m_vWorkedPlots.begin(), m_vWorkedPlots.end(), iPlotNum );
	if (bNewValue)
	{
		if (it==m_vWorkedPlots.end())
			m_vWorkedPlots.push_back(iPlotNum);
	}
	else
	{
		if (it!=m_vWorkedPlots.end())
			m_vWorkedPlots.erase(it);
	}


	CvAssertMsg(iIndex < GetCity()->GetNumWorkablePlots(), "iIndex expected to be < NUM_CITY_PLOTS");

	if(IsWorkingPlot(pPlot) != bNewValue && iIndex >= 0 && iIndex < GetCity()->GetNumWorkablePlots())
	{
		m_pabWorkingPlot[iIndex] = bNewValue;

		// Don't look at the center Plot of a City, because we always work it for free
		if(iIndex != CITY_HOME_PLOT)
		{
			// Alter the count of Plots being worked by Citizens
			if(bNewValue)
			{
				ChangeNumCitizensWorkingPlots(1);

				if(bUseUnassignedPool)
				{
					ChangeNumUnassignedCitizens(-1);
				}
			}
			else
			{
				ChangeNumCitizensWorkingPlots(-1);

				if(bUseUnassignedPool)
				{
					ChangeNumUnassignedCitizens(1);
				}
			}
		}

		if(pPlot != NULL)
		{
			// investigate later
			//CvAssertMsg(pPlot->getWorkingCity() == GetCity(), "WorkingCity is expected to be this");

			// Now working pPlot
			if(IsWorkingPlot(pPlot))
			{
				//if (iIndex != CITY_HOME_PLOT)
				//{
				//	GetCity()->changeWorkingPopulation(1);
				//}
				for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					GetCity()->ChangeBaseYieldRateFromTerrain(((YieldTypes)iI), pPlot->getYield((YieldTypes)iI));
				}
#if defined(MOD_BALANCE_CORE)
				if(pPlot->getTerrainType() != NO_TERRAIN)
				{
					GetCity()->ChangeNumTerrainWorked(pPlot->getTerrainType(), 1);
				}
				if(pPlot->getFeatureType() != NO_FEATURE)
				{
					GetCity()->ChangeNumFeatureWorked(pPlot->getFeatureType(), 1);
				}
				if(pPlot->getResourceType(GetCity()->getTeam()) != NO_RESOURCE)
				{
					GetCity()->ChangeNumResourceWorked(pPlot->getResourceType(GetCity()->getTeam()), 1);
				}
				if(pPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					GetCity()->ChangeNumImprovementWorked(pPlot->getImprovementType(), 1);
				}
#endif
			}
			// No longer working pPlot
			else
			{
				//if (iIndex != CITY_HOME_PLOT)
				//{
				//	GetCity()->changeWorkingPopulation(-1);
				//}

				for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					GetCity()->ChangeBaseYieldRateFromTerrain(((YieldTypes)iI), -pPlot->getYield((YieldTypes)iI));
				}
#if defined(MOD_BALANCE_CORE)
				if(pPlot->getTerrainType() != NO_TERRAIN)
				{
					GetCity()->ChangeNumTerrainWorked(pPlot->getTerrainType(), -1);
				}
				if(pPlot->getFeatureType() != NO_FEATURE)
				{
					GetCity()->ChangeNumFeatureWorked(pPlot->getFeatureType(), -1);
				}
				if(pPlot->getResourceType(GetCity()->getTeam()) != NO_RESOURCE)
				{
					GetCity()->ChangeNumResourceWorked(pPlot->getResourceType(GetCity()->getTeam()), -1);
				}
				if(pPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					GetCity()->ChangeNumImprovementWorked(pPlot->getImprovementType(), -1);
				}
#endif
			}
		}
#if defined(MOD_BALANCE_CORE)
		for(iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			GetCity()->UpdateYieldPerXTerrain(((YieldTypes)iI));
		}
#endif

		if(GetCity()->isCitySelected())
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
	if(iIndex == CITY_HOME_PLOT)
	{
		CvPlot* pLoopPlot;

		// If we've forced any plots to be worked, reset them to the normal state

		for(int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
		{
			if(iPlotLoop != CITY_HOME_PLOT)
			{
				pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

				if(pLoopPlot != NULL)
				{
					if(IsForcedWorkingPlot(pLoopPlot))
					{
						SetForcedWorkingPlot(pLoopPlot, false);
					}
				}
			}
		}

		// Reset Forced Default Specialists
		ChangeNumForcedDefaultSpecialists(-GetNumForcedDefaultSpecialists());

		DoReallocateCitizens();
	}
	else
	{
		CvPlot* pPlot = GetCityPlotFromIndex(iIndex);

		if(pPlot != NULL)
		{
			if(IsCanWork(pPlot))
			{
//				GetCity()->setCitizensAutomated(false);

				// If we're already working the Plot, then take the guy off and turn him into a Default Specialist
				if(IsWorkingPlot(pPlot))
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
					if(GetNumDefaultSpecialists() > 0)
					{
						ChangeNumDefaultSpecialists(-1);
						// Player is forcibly telling city to work a plot, so reduce count of forced default specialists
						if(GetNumForcedDefaultSpecialists() > 0)
							ChangeNumForcedDefaultSpecialists(-1);

						SetWorkingPlot(pPlot, true);
						SetForcedWorkingPlot(pPlot, true);
					}
					// No Default Specialists, so grab a better allocated guy
					else
					{
						// Working Plot
						if(DoRemoveWorstCitizen(true))
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
					//if ((GetCity()->extraSpecialists() > 0) || GetCity()->AI_removeWorstCitizen())
					//{
					//	SetWorkingPlot(pPlot, true);
					//}
				}
			}
			// JON: Need to update this block to work with new system
			else if(pPlot->getOwner() == GetOwner())
			{
				// Can't take away forced plots from puppet Cities
				if(pPlot->getWorkingCityOverride() != NULL)
				{
					if(pPlot->getWorkingCityOverride()->IsPuppet())
					{
						return;
					}
				}

				pPlot->setWorkingCityOverride(GetCity());
			}
		}
	}
}



/// Has our City been told it MUST a particular CvPlot?
bool CvCityCitizens::IsForcedWorkingPlot(const CvPlot* pPlot) const
{
	int iIndex;

	iIndex = GetCityIndexFromPlot(pPlot);

	if(iIndex != -1)
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

	if(IsForcedWorkingPlot(pPlot) != bNewValue && iIndex >= 0 && iIndex < GetCity()->GetNumWorkablePlots())
	{
		m_pabForcedWorkingPlot[iIndex] = bNewValue;

		// Change the count of how many are forced
		if(bNewValue)
		{
			ChangeNumForcedWorkingPlots(1);

			// More forced plots than we have citizens working?  If so, then pick someone to lose their forced status
			if(GetNumForcedWorkingPlots() > GetNumCitizensWorkingPlots())
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
void CvCityCitizens::DoValidateForcedWorkingPlots()
{
	int iNumForcedWorkingPlotsToDemote = GetNumForcedWorkingPlots() - GetNumCitizensWorkingPlots();

	if(iNumForcedWorkingPlotsToDemote > 0)
	{
		for(int iLoop = 0; iLoop < iNumForcedWorkingPlotsToDemote; iLoop++)
		{
			DoDemoteWorstForcedWorkingPlot();
		}
	}
}

/// Remove the Forced status from the worst ForcedWorking plot
void CvCityCitizens::DoDemoteWorstForcedWorkingPlot()
{
	int iValue;

	int iBestPlotValue = -1;
	int iBestPlotID = -1;

	CvPlot* pLoopPlot;

#if defined(MOD_BALANCE_CORE)
	int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD, false) - (m_pCity->foodConsumption() * 100);
#endif

	// Look at all workable Plots

	for(int iPlotLoop = 0; iPlotLoop < GetCity()->GetNumWorkablePlots(); iPlotLoop++)
	{
		if(iPlotLoop != CITY_HOME_PLOT)
		{
			pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

			if(pLoopPlot != NULL)
			{
				if(IsForcedWorkingPlot(pLoopPlot))
				{
#if defined(MOD_BALANCE_CORE)
					iValue = GetPlotValue(pLoopPlot, false, iExcessFoodTimes100);
#else
					iValue = GetPlotValue(pLoopPlot, false);
#endif

					// First, or worst yet?
					if(iBestPlotValue == -1 || iValue < iBestPlotValue)
					{
						iBestPlotValue = iValue;
						iBestPlotID = iPlotLoop;
					}
				}
			}
		}
	}

	if(iBestPlotID > -1)
	{
		pLoopPlot = GetCityPlotFromIndex(iBestPlotID);
		SetForcedWorkingPlot(pLoopPlot, false);
	}
}

/// How many plots have we forced to be worked?
int CvCityCitizens::GetNumForcedWorkingPlots() const
{
	return m_iNumForcedWorkingPlots;
}

/// Changes how many plots we have forced to be worked
void CvCityCitizens::ChangeNumForcedWorkingPlots(int iChange)
{
	if(iChange != 0)
	{
		m_iNumForcedWorkingPlots += iChange;
	}
}

/// Can our City work a particular CvPlot?
bool CvCityCitizens::IsCanWork(CvPlot* pPlot) const
{
	if(pPlot->getWorkingCity() != m_pCity)
	{
		return false;
	}

	CvAssertMsg(GetCityIndexFromPlot(pPlot) != -1, "GetCityIndexFromPlot(pPlot) is expected to be assigned (not -1)");

	if(pPlot->plotCheck(PUF_canSiege, GetOwner()) != NULL)
	{
		return false;
	}

	if(pPlot->isWater())
	{
		if(!(GET_TEAM(GetTeam()).isWaterWork()))
		{
			return false;
		}

	}

	if(!pPlot->hasYield())
	{
		return false;
	}

	if(pPlot->isBlockaded(GetOwner()))
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

	for(int iPlotLoop = 0; iPlotLoop < m_pCity->GetNumWorkablePlots(); iPlotLoop++)
	{
		if(iPlotLoop != CITY_HOME_PLOT)
		{
			pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

			if(pLoopPlot != NULL)
			{
				if(pLoopPlot->isBlockaded(GetOwner()))
				{
					return true;
				}
			}
		}
	}

	return false;
}

/// If we're working this plot make sure we're allowed, and if we're not then correct the situation
void CvCityCitizens::DoVerifyWorkingPlot(CvPlot* pPlot)
{
	if(pPlot != NULL)
	{
		if(IsWorkingPlot(pPlot))
		{
			if(!IsCanWork(pPlot))
			{
				SetWorkingPlot(pPlot, false);
				std::map<SpecialistTypes, int> specialistValueCache;
				DoAddBestCitizenFromUnassigned(specialistValueCache);
			}
		}
	}
}

/// Check all Plots by this City to see if we can actually be working them (if we are)
void CvCityCitizens::DoVerifyWorkingPlots()
{
	int iI;
	CvPlot* pPlot;


	for(iI = 0; iI < GetCity()->GetNumWorkablePlots(); iI++)
	{
		pPlot = GetCityPlotFromIndex(iI);

		DoVerifyWorkingPlot(pPlot);
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



/// Called at the end of every turn: Looks at the specialists in this City and levels them up
void CvCityCitizens::DoSpecialists()
{
	int iGPPChange;
	int iCount;
	int iMod;
	for(int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
		CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
		if(pkSpecialistInfo)
		{
			int iGPThreshold = GetSpecialistUpgradeThreshold((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass());

			// Does this Specialist spawn a GP?
			if(pkSpecialistInfo->getGreatPeopleUnitClass() != NO_UNITCLASS)
			{
				iCount = GetSpecialistCount(eSpecialist);

				// GPP from Specialists
				iGPPChange = pkSpecialistInfo->getGreatPeopleRateChange() * iCount * 100;

				// GPP from Buildings
				iGPPChange += GetBuildingGreatPeopleRateChanges(eSpecialist) * 100;

				if(iGPPChange > 0)
				{
					iMod = 0;

					// City mod
					iMod += GetCity()->getGreatPeopleRateModifier();

					// Player mod
					iMod += GetPlayer()->getGreatPeopleRateModifier();

					// Player and Golden Age mods to this specific class
					if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
					{
						iMod += GetPlayer()->getGreatScientistRateModifier();
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_WRITER"))
					{ 
						if (GetPlayer()->isGoldenAge())
						{
							iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatWriterRateModifier();
						}
						iMod += GetPlayer()->getGreatWriterRateModifier();
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ARTIST"))
					{
						if (GetPlayer()->isGoldenAge())
						{
							iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatArtistRateModifier();
						}
						iMod += GetPlayer()->getGreatArtistRateModifier();
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MUSICIAN"))
					{
						if (GetPlayer()->isGoldenAge())
						{
							iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatMusicianRateModifier();
						}
						iMod += GetPlayer()->getGreatMusicianRateModifier();
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_MERCHANT"))
					{
						iMod += GetPlayer()->getGreatMerchantRateModifier();
					}
					else if((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_ENGINEER"))
					{
						iMod += GetPlayer()->getGreatEngineerRateModifier();
					}
#if defined(MOD_DIPLOMACY_CITYSTATES)
					else if(MOD_DIPLOMACY_CITYSTATES && (UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_GREAT_DIPLOMAT"))
					{
						iMod += GetPlayer()->getGreatDiplomatRateModifier();
					}
#endif

#if defined(MOD_API_UNIFIED_YIELDS)
					if (GetPlayer()->isGoldenAge())
					{
						GreatPersonTypes eGreatPerson = GetGreatPersonFromSpecialist(eSpecialist);

						iMod += GetPlayer()->getGoldenAgeGreatPersonRateModifier(eGreatPerson);
						iMod += GetPlayer()->GetPlayerTraits()->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);

						ReligionTypes eMajority = GetCity()->GetCityReligions()->GetReligiousMajority();
						if(eMajority != NO_RELIGION)
						{
							const CvReligion* pReligion = GC.getGame().GetGameReligions()->GetReligion(eMajority, GetCity()->getOwner());
							if(pReligion)
							{
								iMod += pReligion->m_Beliefs.GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
								BeliefTypes eSecondaryPantheon = GetCity()->GetCityReligions()->GetSecondaryReligionPantheonBelief();
								if (eSecondaryPantheon != NO_BELIEF)
								{
									iMod += GC.GetGameBeliefs()->GetEntry(eSecondaryPantheon)->GetGoldenAgeGreatPersonRateModifier(eGreatPerson);
								}
							}
						}
					}
#endif
#if defined(MOD_BALANCE_CORE)
					if(GetCity()->isCapital())
					{
						int iNumMarried = 0;
						// Loop through all minors and get the total number we've met.
						for(int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
						{
							PlayerTypes eMinor = (PlayerTypes) iPlayerLoop;

							if (eMinor != GetPlayer()->GetID() && GET_PLAYER(eMinor).isAlive() && GET_PLAYER(eMinor).isMinorCiv())
							{
								if (GetPlayer()->IsDiplomaticMarriage() && !GET_TEAM(GET_PLAYER(eMinor).getTeam()).isAtWar(GetPlayer()->getTeam()) && GET_PLAYER(eMinor).GetMinorCivAI()->IsMarried(GetPlayer()->GetID()))
								{
									iNumMarried++;
								}
							}
						}
						if(GetPlayer()->IsDiplomaticMarriage() && iNumMarried > 0)
						{
							iMod += (iNumMarried * GC.getBALANCE_MARRIAGE_GP_RATE());
						}
					}
#endif
					// Apply mod
					iGPPChange *= (100 + iMod);
					iGPPChange /= 100;

					ChangeSpecialistGreatPersonProgressTimes100(eSpecialist, iGPPChange);
				}

				// Enough to spawn a GP?
				if(GetSpecialistGreatPersonProgress(eSpecialist) >= iGPThreshold)
				{
					// No Minors
					if(!GET_PLAYER(GetCity()->getOwner()).isMinorCiv())
					{
						// Reset progress on this Specialist
						DoResetSpecialistGreatPersonProgressTimes100(eSpecialist);

						// Now... actually create the GP!
						const UnitClassTypes eUnitClass = (UnitClassTypes) pkSpecialistInfo->getGreatPeopleUnitClass();
						const CivilizationTypes eCivilization = GetCity()->getCivilizationType();
						CvCivilizationInfo* pCivilizationInfo = GC.getCivilizationInfo(eCivilization);
						if(pCivilizationInfo != NULL)
						{
							UnitTypes eUnit = (UnitTypes) pCivilizationInfo->getCivilizationUnits(eUnitClass);

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
	if(m_pCity->IsResistance() || m_pCity->IsRazing())
	{
		return false;
	}
#endif

	int iNumSpecialistsAssigned = GetNumSpecialistsInBuilding(eBuilding);

	if(iNumSpecialistsAssigned < GetCity()->getPopulation() &&	// Limit based on Pop of City
	        iNumSpecialistsAssigned < GC.getBuildingInfo(eBuilding)->GetSpecialistCount() &&				// Limit for this particular Building
	        iNumSpecialistsAssigned < GC.getMAX_SPECIALISTS_FROM_BUILDING())	// Overall Limit
	{
		return true;
	}

	return false;
}

/// Adds and initializes a Specialist for this building
void CvCityCitizens::DoAddSpecialistToBuilding(BuildingTypes eBuilding, bool bForced)
{
	CvAssert(eBuilding > -1);
	CvAssert(eBuilding < GC.getNumBuildingInfos());

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{
		return;
	}

	SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();

	// Can't add more than the max
	if(IsCanAddSpecialistToBuilding(eBuilding))
	{
		// If we're force-assigning a specialist, then we can reduce the count on forced default specialists
		if(bForced)
		{
			if(GetNumForcedDefaultSpecialists() > 0)
				ChangeNumForcedDefaultSpecialists(-1);
		}

		// If we don't already have an Unassigned Citizen to turn into a Specialist, find one from somewhere
		if(GetNumUnassignedCitizens() == 0)
		{
			DoRemoveWorstCitizen(true, /*Don't remove this type*/ eSpecialist);
			if(GetNumUnassignedCitizens() == 0)
			{
				// Still nobody, all the citizens may be assigned to the eSpecialist we are looking for, try again
				if(!DoRemoveWorstSpecialist(NO_SPECIALIST, eBuilding))
				{
					return; // For some reason we can't do this, we must exit, else we will be going over the population count
				}
			}
		}

		// Increase count for the whole city
		m_aiSpecialistCounts[eSpecialist]++;
		m_aiNumSpecialistsInBuilding[eBuilding]++;

		if(bForced)
		{
			m_aiNumForcedSpecialistsInBuilding[eBuilding]++;
		}

		if(GET_PLAYER(GetCity()->getOwner()).isHuman() && GetCity()->getOwner() == GC.getGame().getActivePlayer())
		{
			GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();
		}

#if defined(MOD_BALANCE_CORE)
		GetCity()->processSpecialist(eSpecialist, 1);

		//we added the first specialist, this may have religious effects
		if (GetTotalSpecialistCount() == 1)
			GetCity()->UpdateReligion(GetCity()->GetCityReligions()->GetReligiousMajority(),false);
#else
		GetCity()->processSpecialist(eSpecialist, 1);
		GetCity()->UpdateReligion(GetCity()->GetCityReligions()->GetReligiousMajority());
#endif

		ChangeNumUnassignedCitizens(-1);

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
void CvCityCitizens::DoRemoveSpecialistFromBuilding(BuildingTypes eBuilding, bool bForced, bool bEliminatePopulation)
{
	CvAssert(eBuilding > -1);
	CvAssert(eBuilding < GC.getNumBuildingInfos());

	CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
	if(pkBuildingInfo == NULL)
	{
		return;
	}

	SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();

	int iNumSpecialistsAssigned = GetNumSpecialistsInBuilding(eBuilding);

	// Need at least 1 assigned to remove
	if(iNumSpecialistsAssigned > 0)
	{
		// Decrease count for the whole city
		m_aiSpecialistCounts[eSpecialist]--;
		m_aiNumSpecialistsInBuilding[eBuilding]--;

		if(bForced)
		{
			m_aiNumForcedSpecialistsInBuilding[eBuilding]--;
		}

		if(GET_PLAYER(GetCity()->getOwner()).isHuman() && GetCity()->getOwner() == GC.getGame().getActivePlayer())
		{
			GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();
		}

#if defined(MOD_BALANCE_CORE)
		GetCity()->processSpecialist(eSpecialist, -1);

		//we removed the last specialist, this may have religious effects
		if (GetTotalSpecialistCount()==0)
			GetCity()->UpdateReligion(GetCity()->GetCityReligions()->GetReligiousMajority(),false);
#else
		GetCity()->processSpecialist(eSpecialist, -1);
		GetCity()->UpdateReligion(GetCity()->GetCityReligions()->GetReligiousMajority());
#endif

		// Do we kill this population or reassign him?
		if(bEliminatePopulation)
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
	if(pkBuildingInfo == NULL)
	{
		return;
	}

	SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
	int iNumSpecialists = GetNumSpecialistsInBuilding(eBuilding);

	m_aiNumForcedSpecialistsInBuilding[eBuilding] = 0;

	// Pick the worst to remove
	for(int iAssignedLoop = 0; iAssignedLoop < iNumSpecialists; iAssignedLoop++)
	{
		// Decrease count for the whole city
		m_aiSpecialistCounts[eSpecialist]--;
		m_aiNumSpecialistsInBuilding[eBuilding]--;
		GetCity()->processSpecialist(eSpecialist, -1);

		if(GET_PLAYER(GetCity()->getOwner()).isHuman() && GetCity()->getOwner() == GC.getGame().getActivePlayer())
		{
			GET_PLAYER(GetCity()->getOwner()).CalculateNetHappiness();
		}

		// Do we kill this population or reassign him?
		if(bEliminatePopulation)
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
}


/// Find the worst Specialist and remove him from duty
bool CvCityCitizens::DoRemoveWorstSpecialist(SpecialistTypes eDontChangeSpecialist, const BuildingTypes eDontRemoveFromBuilding /* = NO_BUILDING */)
{
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);

		if(eBuilding == eDontRemoveFromBuilding)
		{
			continue;
		}

		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo == NULL)
		{
			continue;
		}

		// We might not be allowed to change this Building's Specialists
		if(eDontChangeSpecialist == pkBuildingInfo->GetSpecialistType())
		{
			continue;
		}

		if(GetNumSpecialistsInBuilding(eBuilding) > 0)
		{
			DoRemoveSpecialistFromBuilding(eBuilding, true);

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
void CvCityCitizens::ChangeNumDefaultSpecialists(int iChange)
{
	m_iNumDefaultSpecialists += iChange;

	SpecialistTypes eSpecialist = (SpecialistTypes) GC.getDEFAULT_SPECIALIST();
	m_aiSpecialistCounts[eSpecialist] += iChange;

	GetCity()->processSpecialist(eSpecialist, iChange);

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

/// Count up all the Specialists we have here
int CvCityCitizens::GetTotalSpecialistCount() const
{
	int iNumSpecialists = 0;
	SpecialistTypes eSpecialist;

	for(int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		eSpecialist = (SpecialistTypes) iSpecialistLoop;

		if (eSpecialist != (SpecialistTypes) GC.getDEFAULT_SPECIALIST())
		{
			iNumSpecialists += GetSpecialistCount(eSpecialist);
		}
	}

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
void CvCityCitizens::ChangeSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex, int iChange)
{
	CvAssert(eIndex > -1);
	CvAssert(eIndex < GC.getNumSpecialistInfos());

	m_aiSpecialistGreatPersonProgressTimes100[eIndex] += iChange;
}

/// Reset Specialist progress
void CvCityCitizens::DoResetSpecialistGreatPersonProgressTimes100(SpecialistTypes eIndex)
{
	CvAssert(eIndex > -1);
	CvAssert(eIndex < GC.getNumSpecialistInfos());

	m_aiSpecialistGreatPersonProgressTimes100[eIndex] = 0;
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
	for(int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		eBuilding = (BuildingTypes) iBuildingLoop;

		// Have this Building in the City?
		if(GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
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
			} else if (eUnitClass == GC.getInfoTypeForString("UNITCLASS_SCIENTIST", true)) {
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
	if(eThisPlayersUnitType != NO_UNIT)
	{
		CvUnitEntry* pkUnitInfo = GC.getUnitInfo(eThisPlayersUnitType);
		if(pkUnitInfo != NULL)
		{
			if(pkUnitInfo->IsGPExtra() == 1)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra1Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
			iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra1Created();
#endif
			}
			else if(pkUnitInfo->IsGPExtra() == 2)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra2Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra2Created();
#endif
			}

			else if(pkUnitInfo->IsGPExtra() == 3)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra3Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra3Created();
#endif
			}
			else if(pkUnitInfo->IsGPExtra() == 4)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra4Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra4Created();
#endif
			}
			else if(pkUnitInfo->IsGPExtra() == 5)
			{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra5Created(MOD_GLOBAL_TRULY_FREE_GP);
#else
				iNumCreated = GET_PLAYER(GetCity()->getOwner()).getGPExtra5Created();
#endif
			}
		}
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
	if(GetCity()->getOwner() == GC.getGame().getActivePlayer())
	{
		// Don't show in MP
#if defined(MOD_API_EXTENSIONS)
		if(!GC.getGame().isReallyNetworkMultiPlayer())
#else
		if(!GC.getGame().isNetworkMultiPlayer())	// KWG: Candidate for !GC.getGame().IsOption(GAMEOPTION_SIMULTANEOUS_TURNS)
#endif
		{
			CvPopupInfo kPopupInfo(BUTTONPOPUP_GREAT_PERSON_REWARD, eUnit, GetCity()->GetID());
			GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
		}
	}

	CvPlayer& kPlayer = GET_PLAYER(GetCity()->getOwner());
	CvUnit* newUnit = kPlayer.initUnit(eUnit, GetCity()->getX(), GetCity()->getY());

#if defined(MOD_BALANCE_CORE)
	if(!bIsFree)
	{
		if(kPlayer.GetPlayerTraits()->IsGPWLTKD())
		{
			int iWLTKD = (GC.getCITY_RESOURCE_WLTKD_TURNS() / 2);
			iWLTKD *= GC.getGame().getGameSpeedInfo().getTrainPercent();
			iWLTKD /= 100;
			if(iWLTKD > 0)
			{
				GetCity()->ChangeWeLoveTheKingDayCounter(iWLTKD);
				CvNotifications* pNotifications = kPlayer.GetNotifications();
				if(pNotifications)
				{
					Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_CITY_WLTKD_UA");
					strText <<  newUnit->getNameKey() << GetCity()->getNameKey();
					Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_CITY_WLTKD_UA");
					strSummary << GetCity()->getNameKey();
					pNotifications->Add(NOTIFICATION_GENERIC, strText.toUTF8(), strSummary.toUTF8(), GetCity()->getX(), GetCity()->getY(), -1);
				}
			}
		}
		if(newUnit != NULL)
		{
			float fDelay = 0.0f; 
			GreatPersonTypes eGreatPerson = GetGreatPersonFromUnitClass(newUnit->getUnitClassType());
			if (eGreatPerson != NO_GREATPERSON)
			{
				int iEra = kPlayer.GetCurrentEra();
				if(iEra < 1)
				{
					iEra = 1;
				}
				int iGoldYield = kPlayer.GetPlayerTraits()->GetGreatPersonBornYield(eGreatPerson, YIELD_GOLD);
				if(iGoldYield > 0)
				{
					kPlayer.GetTreasury()->ChangeGold(iGoldYield * iEra);
					if(kPlayer.GetID() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_YELLOW]+%d[ENDCOLOR][ICON_GOLD]", iGoldYield * iEra);
						DLLUI->AddPopupText(newUnit->getX(),newUnit->getY(), text, fDelay);
					}
				}
				int iFaithYield = kPlayer.GetPlayerTraits()->GetGreatPersonBornYield(eGreatPerson, YIELD_FAITH);
				if(iFaithYield > 0)
				{
					kPlayer.ChangeFaith(iFaithYield * iEra);
					if(kPlayer.GetID() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_PEACE]", iFaithYield * iEra);
						DLLUI->AddPopupText(newUnit->getX(),newUnit->getY(), text, fDelay);
					}
				}
				int iCultureYield = kPlayer.GetPlayerTraits()->GetGreatPersonBornYield(eGreatPerson, YIELD_CULTURE);
				if(iCultureYield > 0)
				{
					kPlayer.changeJONSCulture(iCultureYield * iEra);
					GetCity()->ChangeJONSCultureStored(iCultureYield * iEra);
					if(kPlayer.GetID() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_MAGENTA]+%d[ENDCOLOR][ICON_CULTURE]", iCultureYield * iEra);
						DLLUI->AddPopupText(newUnit->getX(),newUnit->getY(), text, fDelay);
					}
				}
				int iGAPYield = kPlayer.GetPlayerTraits()->GetGreatPersonBornYield(eGreatPerson, YIELD_GOLDEN_AGE_POINTS);
				if(iGAPYield > 0)
				{
					kPlayer.ChangeGoldenAgeProgressMeter(iGAPYield * iEra);
					if(kPlayer.GetID() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_GOLDEN_AGE]", iGAPYield * iEra);
						DLLUI->AddPopupText(newUnit->getX(),newUnit->getY(), text, fDelay);
					}
				}
				int iScienceYield = kPlayer.GetPlayerTraits()->GetGreatPersonBornYield(eGreatPerson, YIELD_SCIENCE);
				if(iScienceYield > 0)
				{
					TechTypes eCurrentTech = kPlayer.GetPlayerTechs()->GetCurrentResearch();
					if(eCurrentTech == NO_TECH)
					{
						kPlayer.changeOverflowResearch(iScienceYield * iEra);
					}
					else
					{
						GET_TEAM(kPlayer.getTeam()).GetTeamTechs()->ChangeResearchProgress(eCurrentTech, (iScienceYield * iEra), kPlayer.GetID());
					}
					if(kPlayer.GetID() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_BLUE]+%d[ENDCOLOR][ICON_RESEARCH]", iScienceYield * iEra);
						DLLUI->AddPopupText(newUnit->getX(),newUnit->getY(), text, fDelay);
					}
				}
				int iTourismYield = kPlayer.GetPlayerTraits()->GetGreatPersonBornYield(eGreatPerson, YIELD_TOURISM);
				if(iTourismYield > 0)
				{
					kPlayer.GetCulture()->AddTourismAllKnownCivs(iTourismYield * iEra);
					if(kPlayer.GetID() == GC.getGame().getActivePlayer())
					{
						char text[256] = {0};
						fDelay += 0.5f;
						sprintf_s(text, "[COLOR_WHITE]+%d[ENDCOLOR][ICON_TOURISM]", iTourismYield * iEra);
						DLLUI->AddPopupText(newUnit->getX(),newUnit->getY(), text, fDelay);
					}
				}
			}
		}
	}
#endif
	// Bump up the count
	if(bIncrementCount && !bCountAsProphet)
	{
		if(newUnit->IsGreatGeneral())
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGreatGeneralsCreated(bIsFree);
#else
			kPlayer.incrementGreatGeneralsCreated();
#endif
		}
		else if(newUnit->IsGreatAdmiral())
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
		else if(newUnit->getUnitInfo().IsGPExtra() == 1)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGPExtra1Created(bIsFree);
#else
			kPlayer.incrementGPExtra1Created();
#endif
		}
		else if(newUnit->getUnitInfo().IsGPExtra() == 2)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGPExtra2Created(bIsFree);
#else
			kPlayer.incrementGPExtra2Created();
#endif
		}
		else if(newUnit->getUnitInfo().IsGPExtra() == 3)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGPExtra3Created(bIsFree);
#else
			kPlayer.incrementGPExtra3Created();
#endif
		}
		else if(newUnit->getUnitInfo().IsGPExtra() == 4)
		{
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
			kPlayer.incrementGPExtra4Created(bIsFree);
#else
			kPlayer.incrementGPExtra4Created();
#endif
		}
		else if(newUnit->getUnitInfo().IsGPExtra() == 5)
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
				if (newUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_MERCHANT")) {
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
					kPlayer.incrementGreatMerchantsCreated(bIsFree);
#else
					kPlayer.incrementGreatMerchantsCreated();
#endif
				} else if (newUnit->getUnitInfo().GetUnitClassType() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST")) {
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
					kPlayer.incrementGreatScientistsCreated(bIsFree);
#else
					kPlayer.incrementGreatScientistsCreated();
#endif
				} else {
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
	if(bCountAsProphet || newUnit->getUnitInfo().IsFoundReligion())
	{
#if defined(MOD_BUGFIX_MINOR)
		if (bIncrementCount)
#endif
#if defined(MOD_GLOBAL_TRULY_FREE_GP)
		kPlayer.GetReligions()->ChangeNumProphetsSpawned(1, bIsFree);
#else
		kPlayer.GetReligions()->ChangeNumProphetsSpawned(1);
#endif
	}

	// Setup prophet properly
	if(newUnit->getUnitInfo().IsFoundReligion())
	{
		ReligionTypes eReligion = kPlayer.GetReligions()->GetReligionCreatedByPlayer();
		int iReligionSpreads = newUnit->getUnitInfo().GetReligionSpreads();
		int iReligiousStrength = newUnit->getUnitInfo().GetReligiousStrength();
		if(iReligionSpreads > 0 && eReligion > RELIGION_PANTHEON)
		{
			newUnit->GetReligionData()->SetSpreadsLeft(iReligionSpreads);
			newUnit->GetReligionData()->SetReligiousStrength(iReligiousStrength);
			newUnit->GetReligionData()->SetReligion(eReligion);
		}
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
	if (newUnit->getUnitInfo().GetBaseCultureTurnsToCount() > 0)
	{
		newUnit->SetCultureBlastStrength(newUnit->getGivePoliciesCulture());
	}
#endif

	// Notification
	if(GET_PLAYER(GetOwner()).GetNotifications())
	{
		Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER");
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_PERSON");
		GET_PLAYER(GetOwner()).GetNotifications()->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), GetCity()->getX(), GetCity()->getY(), eUnit);
	}
}