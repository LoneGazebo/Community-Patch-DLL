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
void CvCityCitizens::Init(CvCity *pCity)
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

	CvAssertMsg((0 < NUM_CITY_PLOTS),  "NUM_CITY_PLOTS is not greater than zero but an array is being allocated in CvCityCitizens::reset");
	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_pabWorkingPlot[iI] = false;
		m_pabForcedWorkingPlot[iI] = false;
	}

	m_iNumDefaultSpecialists = 0;
	m_iNumForcedDefaultSpecialists = 0;

	CvAssertMsg(m_aiSpecialistCounts==NULL, "about to leak memory, CvCityCitizens::m_aiSpecialistCounts");
	m_aiSpecialistCounts = FNEW(int[GC.getNumSpecialistInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_aiSpecialistCounts[iI] = 0;
	}

	CvAssertMsg(m_aiSpecialistGreatPersonProgressTimes100==NULL, "about to leak memory, CvCityCitizens::m_aiSpecialistGreatPersonProgressTimes100");
	m_aiSpecialistGreatPersonProgressTimes100 = FNEW(int[GC.getNumSpecialistInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_aiSpecialistGreatPersonProgressTimes100[iI] = 0;
	}

	CvAssertMsg(m_aiNumSpecialistsInBuilding==NULL, "about to leak memory, CvCityCitizens::m_aiNumSpecialistsInBuilding");
	m_aiNumSpecialistsInBuilding = FNEW(int[GC.getNumBuildingInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_aiNumSpecialistsInBuilding[iI] = 0;
	}

	CvAssertMsg(m_aiNumForcedSpecialistsInBuilding==NULL, "about to leak memory, CvCityCitizens::m_aiNumForcedSpecialistsInBuilding");
	m_aiNumForcedSpecialistsInBuilding = FNEW(int[GC.getNumBuildingInfos()], c_eCiv5GameplayDLL, 0);
	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_aiNumForcedSpecialistsInBuilding[iI] = 0;
	}

	CvAssertMsg(m_piBuildingGreatPeopleRateChanges==NULL, "about to leak memory, CvCityCitizens::m_piBuildingGreatPeopleRateChanges");
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

	ArrayWrapper<int> wrapSpecialistCounts(GC.getNumSpecialistInfos(), m_aiSpecialistCounts);
	kStream >> wrapSpecialistCounts;

	ArrayWrapper<int> wrapGreatPersonProgress(GC.getNumSpecialistInfos(), m_aiSpecialistGreatPersonProgressTimes100);
	kStream >> wrapGreatPersonProgress;

	if (uiVersion >= 2)
	{
		BuildingArrayHelpers::Read(kStream, m_aiNumSpecialistsInBuilding);
		BuildingArrayHelpers::Read(kStream, m_aiNumForcedSpecialistsInBuilding);
	}
	else
	{
		ArrayWrapper<int> wrapSpecialistsInBuilding(89, m_aiNumSpecialistsInBuilding);
		kStream >> wrapSpecialistsInBuilding;
		ArrayWrapper<int> wrapForcedInBuilding(89, m_aiNumForcedSpecialistsInBuilding);
		kStream >> wrapForcedInBuilding;
	}

	ArrayWrapper<int> wrapGreatPeopleRateChanges(GC.getNumSpecialistInfos(), m_piBuildingGreatPeopleRateChanges);
	kStream >> wrapGreatPeopleRateChanges;
}

/// Serialization write
void CvCityCitizens::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 2;
	kStream << uiVersion;

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
	kStream << ArrayWrapper<int>(GC.getNumSpecialistInfos(), m_aiSpecialistCounts);
	kStream << ArrayWrapper<int>(GC.getNumSpecialistInfos(), m_aiSpecialistGreatPersonProgressTimes100);

	BuildingArrayHelpers::Write(kStream, m_aiNumSpecialistsInBuilding, GC.getNumBuildingInfos());
	BuildingArrayHelpers::Write(kStream, m_aiNumForcedSpecialistsInBuilding, GC.getNumBuildingInfos());

	kStream << ArrayWrapper<int>(GC.getNumSpecialistInfos(), m_piBuildingGreatPeopleRateChanges);
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
		SetWorkingPlot(pHomePlot, true, /*bUseUnassignedPool*/ false);
	}
}

/// Processed every turn
void CvCityCitizens::DoTurn()
{
	DoVerifyWorkingPlots();

	CvPlayerAI& thisPlayer = GET_PLAYER(GetOwner());

	if (m_pCity->IsPuppet())
	{
		SetFocusType(CITY_AI_FOCUS_TYPE_GOLD);
		SetNoAutoAssignSpecialists(false);
		SetForcedAvoidGrowth(false);
		int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);
		if (iExcessFoodTimes100 < 0)
		{
			SetFocusType(NO_CITY_AI_FOCUS_TYPE);
			SetNoAutoAssignSpecialists(true);
			SetForcedAvoidGrowth(false);
		}
	}
	else if (!thisPlayer.isHuman())
	{
		CitySpecializationTypes eWonderSpecializationType = thisPlayer.GetCitySpecializationAI()->GetWonderSpecialization();

		if (GC.getGame().getGameTurn() % 8 == 0)
		{
			SetFocusType(CITY_AI_FOCUS_TYPE_GOLD_GROWTH);
			SetNoAutoAssignSpecialists(true);
			SetForcedAvoidGrowth(false);
			int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);
			if (iExcessFoodTimes100 < 2)
			{
				SetFocusType(NO_CITY_AI_FOCUS_TYPE);
				SetNoAutoAssignSpecialists(true);
			}
		}
		if (m_pCity->isCapital() && !thisPlayer.isMinorCiv() && m_pCity->GetCityStrategyAI()->GetSpecialization() != eWonderSpecializationType)
		{
			SetFocusType(NO_CITY_AI_FOCUS_TYPE);
			SetNoAutoAssignSpecialists(false);
			SetForcedAvoidGrowth(false);
			int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);
			if (iExcessFoodTimes100 < 4)
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_FOOD);
				SetNoAutoAssignSpecialists(true);
			}
		}
		else if (m_pCity->GetCityStrategyAI()->GetSpecialization() == eWonderSpecializationType)
		{
			SetFocusType(CITY_AI_FOCUS_TYPE_PRODUCTION);
			SetNoAutoAssignSpecialists(false);
			//SetForcedAvoidGrowth(true);
			int iExcessFoodTimes100;// = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);
			//if (iExcessFoodTimes100 < 2)
			//{
				SetForcedAvoidGrowth(false);
			//}
			iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);
			if (iExcessFoodTimes100 < 2)
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_PROD_GROWTH);
				SetNoAutoAssignSpecialists(true);
				SetForcedAvoidGrowth(false);
			}
			iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);
			if (iExcessFoodTimes100 < 2)
			{
				SetFocusType(NO_CITY_AI_FOCUS_TYPE);
				SetNoAutoAssignSpecialists(true);
				SetForcedAvoidGrowth(false);
			}
		}
		else if (m_pCity->getPopulation() < 5) // we want a balanced growth
		{
			SetFocusType(NO_CITY_AI_FOCUS_TYPE);
			SetNoAutoAssignSpecialists(true);
			SetForcedAvoidGrowth(false);
		}
		else
		{
			// Are we running at a deficit?
			EconomicAIStrategyTypes eStrategyLosingMoney = (EconomicAIStrategyTypes) GC.getInfoTypeForString("ECONOMICAISTRATEGY_LOSING_MONEY");
			bool bInDeficit = thisPlayer.GetEconomicAI()->IsUsingStrategy(eStrategyLosingMoney);
			if (bInDeficit)
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_GOLD_GROWTH);
				SetNoAutoAssignSpecialists(false);
				SetForcedAvoidGrowth(false);
				int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);
				if (iExcessFoodTimes100 < 2)
				{
					SetFocusType(NO_CITY_AI_FOCUS_TYPE);
					SetNoAutoAssignSpecialists(true);
				}
			}
			else if (GC.getGame().getGameTurn() % 3 == 0 && thisPlayer.GetGrandStrategyAI()->GetActiveGrandStrategy() == (AIGrandStrategyTypes) GC.getInfoTypeForString("AIGRANDSTRATEGY_CULTURE") )
			{
				SetFocusType(CITY_AI_FOCUS_TYPE_CULTURE);
				SetNoAutoAssignSpecialists(true);
				SetForcedAvoidGrowth(false);
				int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);
				if (iExcessFoodTimes100 < 2)
				{
					SetFocusType(NO_CITY_AI_FOCUS_TYPE);
					SetNoAutoAssignSpecialists(true);
				}
			}
			else // we aren't a small city, building a wonder, or going broke
			{
				SetNoAutoAssignSpecialists(false);
				SetForcedAvoidGrowth(false);
				CitySpecializationTypes eSpecialization = m_pCity->GetCityStrategyAI()->GetSpecialization();
				if (eSpecialization != -1)
				{
					CvCitySpecializationXMLEntry* pCitySpecializationEntry =  GC.getCitySpecializationInfo(eSpecialization);
					if (pCitySpecializationEntry)
					{
						YieldTypes eYield = pCitySpecializationEntry->GetYieldType();
						if (eYield == YIELD_FOOD)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_FOOD);
						}
						else if (eYield == YIELD_PRODUCTION)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_PROD_GROWTH);
						}
						else if (eYield == YIELD_GOLD)
						{
							SetFocusType(CITY_AI_FOCUS_TYPE_GOLD);
						}
						else if (eYield == YIELD_SCIENCE)
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
}

/// What is the overall value of the current Plot?
int CvCityCitizens::GetPlotValue(CvPlot* pPlot, bool bUseAllowGrowthFlag)
{
	int iValue = 0;
	//YieldTypes eDeficientYield = GetCity()->GetCityStrategyAI()->GetDeficientYield();

	// Yield Values
	int iFoodYieldValue = (/*12*/ GC.getAI_CITIZEN_VALUE_FOOD() * pPlot->getYield(YIELD_FOOD));
	int iProductionYieldValue = (/*8*/ GC.getAI_CITIZEN_VALUE_PRODUCTION() * pPlot->getYield(YIELD_PRODUCTION));
	int iGoldYieldValue = (/*10*/ GC.getAI_CITIZEN_VALUE_GOLD() * pPlot->getYield(YIELD_GOLD));
	int iScienceYieldValue = (/*6*/ GC.getAI_CITIZEN_VALUE_SCIENCE() * pPlot->getYield(YIELD_SCIENCE));
	int iCultureYieldValue = GC.getAI_CITIZEN_VALUE_CULTURE() * pPlot->GetCulture(); // this is different than Civ4

	// How much surplus food are we making?
	int iExcessFoodTimes100 = m_pCity->getYieldRateTimes100(YIELD_FOOD) - (m_pCity->foodConsumption() * 100);

	bool bAvoidGrowth = IsAvoidGrowth();

	// City Focus
	CityAIFocusTypes eFocus = GetFocusType();
	if (eFocus == CITY_AI_FOCUS_TYPE_FOOD)
		iFoodYieldValue *= 5;
	else if (eFocus == CITY_AI_FOCUS_TYPE_PRODUCTION)
		iProductionYieldValue *= 4;
	else if (eFocus == CITY_AI_FOCUS_TYPE_GOLD)
		iGoldYieldValue *= 4;
	else if (eFocus == CITY_AI_FOCUS_TYPE_SCIENCE)
		iScienceYieldValue *= 4;
	else if (eFocus == CITY_AI_FOCUS_TYPE_CULTURE)
		iCultureYieldValue *= 4;
	else if (eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
	{
		iFoodYieldValue *= 2;
		iGoldYieldValue *= 5;
	}
	else if (eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
	{
		iFoodYieldValue *= 2;
		iProductionYieldValue *= 5;
	}

	// Food can be worth less if we don't want to grow
	if (bUseAllowGrowthFlag && iExcessFoodTimes100 >= 0 && bAvoidGrowth)
	{
		// If we at least have enough Food to feed everyone, zero out the value of additional food
		iFoodYieldValue = 0;
	}
	// We want to grow here
	else
	{
		// If we have a non-default and non-food focus, only worry about getting to 0 food
		if ( eFocus != NO_CITY_AI_FOCUS_TYPE && eFocus != CITY_AI_FOCUS_TYPE_FOOD && eFocus != CITY_AI_FOCUS_TYPE_PROD_GROWTH&& eFocus != CITY_AI_FOCUS_TYPE_GOLD_GROWTH )
		{
			int iFoodT100NeededFor0 = -iExcessFoodTimes100;

			if (iFoodT100NeededFor0 > 0)
			{
				iFoodYieldValue *= 2;
			}
		}
		// If our surplus is not at least 2, really emphasize food plots
		else if (!bAvoidGrowth)
		{
			int iFoodT100NeededFor2 = 200 - iExcessFoodTimes100;

			if (iFoodT100NeededFor2 > 0)
			{
				iFoodYieldValue *= 2;
			}
		}
	}

	if ((eFocus == NO_CITY_AI_FOCUS_TYPE || eFocus == CITY_AI_FOCUS_TYPE_PROD_GROWTH || eFocus == CITY_AI_FOCUS_TYPE_GOLD_GROWTH) && !bAvoidGrowth && m_pCity->getPopulation() < 5)
	{
		iFoodYieldValue *= 3;
	}

	iValue += iFoodYieldValue;
	iValue += iProductionYieldValue;
	iValue += iGoldYieldValue;
	iValue += iScienceYieldValue;
	iValue += iCultureYieldValue;

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
	if (m_bNoAutoAssignSpecialists != bValue)
	{
		m_bNoAutoAssignSpecialists = bValue;

		// If we're giving the AI control clear all manually assigned Specialists
		if (!bValue)
		{
			DoClearForcedSpecialists();
		}

		DoReallocateCitizens();
	}
}

/// Is this City avoiding growth?
bool CvCityCitizens::IsAvoidGrowth()
{
	if (GC.getGame().isOption(GAMEOPTION_NO_HAPPINESS))
	{
		return false;
	}

	if (GetPlayer()->GetExcessHappiness() < 0)
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
	if (m_bForceAvoidGrowth != bAvoidGrowth)
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

	if (eFocus != m_eCityAIFocusTypes)
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
	if (m_pCity->getPopulation() < 3)
	{
		iWeight /= 2;
	}

	int iFoodPerTurn = m_pCity->getYieldRate(YIELD_FOOD);
	int iFoodEatenPerTurn = m_pCity->foodConsumption();
	int iSurplusFood = iFoodPerTurn - iFoodEatenPerTurn;

	CityAIFocusTypes eFocusType = GetFocusType();

	// If we don't yet have enough Food to feed our City, we don't want no Specialists!
	if (iSurplusFood <= 0)
	{
		iWeight /= 3;
	}
	else if (IsAvoidGrowth() && (eFocusType == NO_CITY_AI_FOCUS_TYPE || eFocusType == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE) )
	{
		iWeight *= 2;
	}
	else if (iSurplusFood <= 2)
	{
		iWeight /= 2;
	}
	else if (iSurplusFood > 2)
	{
		if ( eFocusType == NO_CITY_AI_FOCUS_TYPE || eFocusType == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE || eFocusType == CITY_AI_FOCUS_TYPE_PROD_GROWTH )
		{
			iWeight *= 100 + (20 * (iSurplusFood - 4));
			iWeight /= 100;
		}
	}

	// If we're deficient in Production then we're less likely to want Specialists
	if (m_pCity->GetCityStrategyAI()->IsYieldDeficient(YIELD_PRODUCTION))
	{
		iWeight *= 50;
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
			eBuilding = (BuildingTypes) iBuildingLoop;

			// Have this Building in the City?
			if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if (IsCanAddSpecialistToBuilding(eBuilding))
				{
					iWeight *= 3;
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
			if(pkBuildingInfo)
			{
				// Have this Building in the City?
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if (IsCanAddSpecialistToBuilding(eBuilding))
					{
						const SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
						CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
						if (pSpecialistInfo && pSpecialistInfo->getCulturePerTurn() > 0)
						{
							iWeight *= 3;
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
			eBuilding = (BuildingTypes) iBuildingLoop;

			// Have this Building in the City?
			if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if (IsCanAddSpecialistToBuilding(eBuilding))
				{
					SpecialistTypes eSpecialist = (SpecialistTypes) GC.getBuildingInfo(eBuilding)->GetSpecialistType();
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
			if(pkBuildingInfo)
			{
				// Have this Building in the City?
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if (IsCanAddSpecialistToBuilding(eBuilding))
					{
						SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
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
			eBuilding = (BuildingTypes) iBuildingLoop;

			// Have this Building in the City?
			if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if (IsCanAddSpecialistToBuilding(eBuilding))
				{
					SpecialistTypes eSpecialist = (SpecialistTypes) GC.getBuildingInfo(eBuilding)->GetSpecialistType();
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
			if(pkBuildingInfo)
			{
				// Have this Building in the City?
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if (IsCanAddSpecialistToBuilding(eBuilding))
					{
						SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
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
			if(pkBuildingInfo)
			{
				// Have this Building in the City?
				if (m_pCity->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
				{
					// Can't add more than the max
					if (IsCanAddSpecialistToBuilding(eBuilding))
					{
						SpecialistTypes eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();
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

	// Does the AI want it enough?
	if (iWeight >= 150)
	{
		return true;
	}

	return false;
}

/// What is the Building Type the AI likes the Specialist of most right now?
BuildingTypes CvCityCitizens::GetAIBestSpecialistBuilding()
{
	BuildingTypes eBestBuilding = NO_BUILDING;
	int iBestSpecialistValue = -1;

	SpecialistTypes eSpecialist;
	int iValue;

	// Loop through all Buildings
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);
		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);

		if(pkBuildingInfo)
		{
			// Have this Building in the City?
			if (GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
			{
				// Can't add more than the max
				if (IsCanAddSpecialistToBuilding(eBuilding))
				{
					eSpecialist = (SpecialistTypes) pkBuildingInfo->GetSpecialistType();

					iValue = GetSpecialistValue(eSpecialist);

					// Add a bit more weight to a Building if it has more slots (10% per).  This will bias the AI to fill a single building over spreading Specialists out
					int iTemp = ((GetNumSpecialistsAllowedByBuilding(*pkBuildingInfo) - 1) * iValue * 10);
					iTemp /= 100;
					iValue += iTemp;

					if (iValue > iBestSpecialistValue)
					{
						eBestBuilding = eBuilding;
						iBestSpecialistValue = iValue;
					}
				}
			}
		}
	}

	return eBestBuilding;
}

/// How valuable is eSpecialist?
int CvCityCitizens::GetSpecialistValue(SpecialistTypes eSpecialist)
{
	int iValue = 20;

	CvSpecialistInfo* pSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
	if(pSpecialistInfo == NULL)
	{
		//This function should NEVER be called with an invalid specialist info type.
		CvAssert(pSpecialistInfo);
		return 0;
	}

	YieldTypes eDeficientYield = GetCity()->GetCityStrategyAI()->GetDeficientYield();

	// Does this Specialist help us with a Deficient Yield?

	CityAIFocusTypes eFocusType = GetFocusType();
	if (eFocusType == CITY_AI_FOCUS_TYPE_SCIENCE)
	{
		iValue += (pSpecialistInfo->getYieldChange(YIELD_SCIENCE) * 3);
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_CULTURE)
	{
		iValue += (pSpecialistInfo->getCulturePerTurn() * 3);
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_GOLD)
	{
		iValue += (pSpecialistInfo->getYieldChange(YIELD_GOLD) * 3);
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_PRODUCTION)
	{
		if (eDeficientYield == YIELD_PRODUCTION)
		{
			iValue += (iValue * pSpecialistInfo->getYieldChange(eDeficientYield));
		}
		iValue += (pSpecialistInfo->getYieldChange(YIELD_PRODUCTION) * 2);
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_GREAT_PEOPLE)
	{
		iValue += (GetSpecialistGreatPersonProgress(eSpecialist) / 5);
		if (eDeficientYield != NO_YIELD)
		{
			iValue += (iValue * pSpecialistInfo->getYieldChange(eDeficientYield));
		}
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_FOOD)
	{
		iValue += (pSpecialistInfo->getYieldChange(YIELD_FOOD) * 3);
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_PROD_GROWTH)
	{
		if (eDeficientYield == YIELD_PRODUCTION)
		{
			iValue += (iValue * pSpecialistInfo->getYieldChange(eDeficientYield));
		}
		iValue += (pSpecialistInfo->getYieldChange(YIELD_PRODUCTION) * 2);
	}
	else if (eFocusType == CITY_AI_FOCUS_TYPE_GOLD_GROWTH)
	{
		iValue += (pSpecialistInfo->getYieldChange(YIELD_GOLD) * 2);
	}
	else
	{
		if (eDeficientYield != NO_YIELD)
		{
			iValue += (iValue * pSpecialistInfo->getYieldChange(eDeficientYield));
		}
		// if we are nearing completion of a GP
		iValue += (GetSpecialistGreatPersonProgress(eSpecialist) / 10);
	}

	// GPPs are always good
	iValue += pSpecialistInfo->getGreatPeopleRateChange();

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
bool CvCityCitizens::DoAddBestCitizenFromUnassigned()
{
	// We only assign the unassigned here, folks
	if (GetNumUnassignedCitizens() == 0)
	{
		return false;
	}

	// Maybe we want to add a Specialist?
	if (!IsNoAutoAssignSpecialists())
	{
		// Have to want it right now: look at Food situation, mainly
		if (IsAIWantSpecialistRightNow())
		{
			BuildingTypes eBestBuilding = GetAIBestSpecialistBuilding();

			// Is there a Specialist we can assign?
			if (eBestBuilding != NO_BUILDING)
			{
				DoAddSpecialistToBuilding(eBestBuilding, /*bForced*/ false);
				return true;
			}
		}
	}

	int iBestPlotValue = 0;
	CvPlot* pBestPlot = GetBestCityPlotWithValue(iBestPlotValue, /*bBest*/ true, /*bWorked*/ false);

	// Found a Valid Plot to place a guy?
	if (pBestPlot != NULL)
	{
		// Now assign the guy to the best possible Plot
		SetWorkingPlot(pBestPlot, true);
		//ChangeNumUnassignedCitizens(-1);

		return true;
	}
	// No valid Plot - change this guy into a default Specialist
	else
	{
		ChangeNumDefaultSpecialists(1);
		//ChangeNumUnassignedCitizens(-1);
	}

	return false;
}

/// Pick the worst Plot to stop working
bool CvCityCitizens::DoRemoveWorstCitizen(bool bRemoveForcedStatus, SpecialistTypes eDontChangeSpecialist)
{
	// Are all of our guys already not working Plots?
	if (GetNumUnassignedCitizens() == GetCity()->getPopulation())
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
		if (GetNumDefaultSpecialists() > GetCity()->getPopulation())
		{
			ChangeNumForcedDefaultSpecialists(-1);
			ChangeNumDefaultSpecialists(-1);
			return true;
		}
	}

	// No Default Specialists, remove a working Pop, if there is one
	int iWorstPlotValue = 0;
	CvPlot* pWorstPlot = GetBestCityPlotWithValue(iWorstPlotValue, /*bBest*/ false, /*bWorked*/ true);

	if (pWorstPlot != NULL)
	{
		SetWorkingPlot(pWorstPlot, false);

		// If we were force-working this Plot, turn it off
		if (bRemoveForcedStatus)
		{
			if (IsForcedWorkingPlot(pWorstPlot))
			{
				SetForcedWorkingPlot(pWorstPlot, false);
			}
		}

		return true;
	}
	// Have to resort to pulling away a good Specialist
	else
	{
		if (DoRemoveWorstSpecialist(eDontChangeSpecialist))
		{
			return true;
		}
	}

	return false;
}

/// Find a Plot the City is either working or not, and the best/worst value for it - this function does "double duty" depending on what the user wants to find
CvPlot* CvCityCitizens::GetBestCityPlotWithValue(int &iValue, bool bWantBest, bool bWantWorked)
{
	bool bPlotForceWorked;

	int iBestPlotValue = -1;
	int iBestPlotID = -1;

	CvPlot* pLoopPlot;

	// Look at all workable Plots
	for (int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		if (iPlotLoop != CITY_HOME_PLOT)
		{
			pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

			if (pLoopPlot != NULL)
			{
				// Is this a Plot this City controls?
				if (pLoopPlot->getWorkingCity() != NULL && pLoopPlot->getWorkingCity()->GetID() == GetCity()->GetID())
				{
					// Working the Plot and wanting to work it, or Not working it and wanting to find one to work?
					if ((IsWorkingPlot(pLoopPlot) && bWantWorked) ||
						(!IsWorkingPlot(pLoopPlot) && !bWantWorked))
					{
						// Working the Plot or CAN work the Plot?
						if (bWantWorked || IsCanWork(pLoopPlot))
						{
							iValue = GetPlotValue(pLoopPlot, bWantBest);

							bPlotForceWorked = IsForcedWorkingPlot(pLoopPlot);

							if (bPlotForceWorked)
							{
								// Looking for best, unworked Plot: Forced plots are FIRST to be picked
								if (bWantBest && !bWantWorked)
								{
									iValue += 10000;
								}
								// Looking for worst, worked Plot: Forced plots are LAST to be picked, so make it's value incredibly high
								if (!bWantBest && bWantWorked)
								{
									iValue += 10000;
								}
							}

							if (iBestPlotValue == -1 ||							// First Plot?
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

	if (iBestPlotID == -1)
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
	for (int iWorkerLoop = 0; iWorkerLoop < iNumCitizensToRemove; iWorkerLoop++)
	{
		DoRemoveWorstCitizen();
	}

	int iSpecialistLoop;

	// Remove Non-Forced Specialists in Buildings
	int iNumSpecialistsToRemove;
	BuildingTypes eBuilding;
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		eBuilding = (BuildingTypes) iBuildingLoop;

		// Have this Building in the City?
		if (GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		{
			iNumSpecialistsToRemove = GetNumSpecialistsInBuilding(eBuilding) - GetNumForcedSpecialistsInBuilding(eBuilding);	// Don't include Forced guys

			// Loop through guys to remove (if there are any)
			for (iSpecialistLoop = 0; iSpecialistLoop < iNumSpecialistsToRemove; iSpecialistLoop++)
			{
				DoRemoveSpecialistFromBuilding(eBuilding, /*bForced*/ false);
			}
		}
	}

	// Remove Default Specialists
	int iNumDefaultsToRemove = GetNumDefaultSpecialists() - GetNumForcedDefaultSpecialists();
	for (iSpecialistLoop = 0; iSpecialistLoop < iNumDefaultsToRemove; iSpecialistLoop++)
	{
		ChangeNumDefaultSpecialists(-1);
	}

	// Now put all of the unallocated guys back
	int iNumToAllocate = GetNumUnassignedCitizens();
	for (int iUnallocatedLoop = 0; iUnallocatedLoop < iNumToAllocate; iUnallocatedLoop++)
	{
		DoAddBestCitizenFromUnassigned();
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

	if (iIndex != -1)
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
	CvAssertMsg(iIndex < NUM_CITY_PLOTS, "iIndex expected to be < NUM_CITY_PLOTS");

	if (IsWorkingPlot(pPlot) != bNewValue && iIndex >= 0 && iIndex < NUM_CITY_PLOTS)
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
			// investigate later
			//CvAssertMsg(pPlot->getWorkingCity() == GetCity(), "WorkingCity is expected to be this");

			// Now working pPlot
			if (IsWorkingPlot(pPlot))
			{
				//if (iIndex != CITY_HOME_PLOT)
				//{
				//	GetCity()->changeWorkingPopulation(1);
				//}

				for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					GetCity()->ChangeBaseYieldRateFromTerrain(((YieldTypes)iI), pPlot->getYield((YieldTypes)iI));
				}

				GetCity()->ChangeJONSCulturePerTurnFromTerrain(pPlot->GetCulture());
			}
			// No longer working pPlot
			else
			{
				//if (iIndex != CITY_HOME_PLOT)
				//{
				//	GetCity()->changeWorkingPopulation(-1);
				//}

				for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
				{
					GetCity()->ChangeBaseYieldRateFromTerrain(((YieldTypes)iI), -(pPlot->getYield((YieldTypes)iI)));
				}

				GetCity()->ChangeJONSCulturePerTurnFromTerrain(-pPlot->GetCulture());
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
	CvAssertMsg(iIndex < NUM_CITY_PLOTS, "iIndex expected to be < NUM_CITY_PLOTS");

	// Clicking ON the city "resets" it to default setup
	if (iIndex == CITY_HOME_PLOT)
	{
		CvPlot* pLoopPlot;

		// If we've forced any plots to be worked, reset them to the normal state
		for (int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
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

		DoReallocateCitizens();
	}
	else
	{
		CvPlot* pPlot = GetCityPlotFromIndex(iIndex);

		if (pPlot != NULL)
		{
			if (IsCanWork(pPlot))
			{
//				GetCity()->setCitizensAutomated(false);

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
					//if ((GetCity()->extraSpecialists() > 0) || GetCity()->AI_removeWorstCitizen())
					//{
					//	SetWorkingPlot(pPlot, true);
					//}
				}
			}
			// JON: Need to update this block to work with new system
			else if (pPlot->getOwner() == GetOwner())
			{
				// Can't take away forced plots from puppet Cities
				if (pPlot->getWorkingCityOverride() != NULL)
				{
					if (pPlot->getWorkingCityOverride()->IsPuppet())
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
	CvAssertMsg(iIndex < NUM_CITY_PLOTS, "iIndex expected to be < NUM_CITY_PLOTS");

	if (IsForcedWorkingPlot(pPlot) != bNewValue && iIndex >= 0 && iIndex < NUM_CITY_PLOTS)
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
void CvCityCitizens::DoValidateForcedWorkingPlots()
{
	int iNumForcedWorkingPlotsToDemote = GetNumForcedWorkingPlots() - GetNumCitizensWorkingPlots();

	if (iNumForcedWorkingPlotsToDemote > 0)
	{
		for (int iLoop = 0; iLoop < iNumForcedWorkingPlotsToDemote; iLoop++)
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

	// Look at all workable Plots
	for (int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		if (iPlotLoop != CITY_HOME_PLOT)
		{
			pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

			if (pLoopPlot != NULL)
			{
				if (IsForcedWorkingPlot(pLoopPlot))
				{
					iValue = GetPlotValue(pLoopPlot, false);

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
	if (iChange != 0)
	{
		m_iNumForcedWorkingPlots += iChange;
	}
}

/// Can our City work a particular CvPlot?
bool CvCityCitizens::IsCanWork(CvPlot* pPlot) const
{
	if (pPlot->getWorkingCity() != m_pCity)
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

	if (!pPlot->hasYield() && pPlot->GetCulture() <= 0)
	{
		return false;
	}

	if (IsPlotBlockaded(pPlot))
	{
		return false;
	}

	return true;
}

// Is there a naval blockade on this water tile?
bool CvCityCitizens::IsPlotBlockaded(CvPlot *pPlot) const
{
	// See if there are any enemy boats near us that are blockading this plot
	int iBlockadeDistance = /*2*/ GC.getNAVAL_PLOT_BLOCKADE_RANGE();
	int iDX, iDY;
	CvPlot* pNearbyPlot;

	PlayerTypes ePlayer = m_pCity->getOwner();

	// Might be a better way to do this that'd be slightly less CPU-intensive
	for (iDX = -(iBlockadeDistance); iDX <= iBlockadeDistance; iDX++)
	{
		for (iDY = -(iBlockadeDistance); iDY <= iBlockadeDistance; iDY++)
		{
			pNearbyPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

			if (pNearbyPlot != NULL)
			{
				// Must be water in the same Area
				if (pNearbyPlot->isWater() && pNearbyPlot->getArea() == pPlot->getArea())
				{
					if (plotDistance(pNearbyPlot->getX(), pNearbyPlot->getY(), pPlot->getX(), pPlot->getY()) <= iBlockadeDistance)
					{
						// Enemy boat within range to blockade our plot?
						if (pNearbyPlot->IsActualEnemyUnit(ePlayer))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

// Is there a naval blockade on any of this city's water tiles?
bool CvCityCitizens::IsAnyPlotBlockaded() const
{
	CvPlot* pLoopPlot;

	// Look at all workable Plots
	for (int iPlotLoop = 0; iPlotLoop < NUM_CITY_PLOTS; iPlotLoop++)
	{
		if (iPlotLoop != CITY_HOME_PLOT)
		{
			pLoopPlot = GetCityPlotFromIndex(iPlotLoop);

			if (pLoopPlot != NULL)
			{
				if (IsPlotBlockaded(pLoopPlot))
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
	if (pPlot != NULL)
	{
		if (IsWorkingPlot(pPlot))
		{
			if (!IsCanWork(pPlot))
			{
				SetWorkingPlot(pPlot, false);
				DoAddBestCitizenFromUnassigned();
			}
		}
	}
}

/// Check all Plots by this City to see if we can actually be working them (if we are)
void CvCityCitizens::DoVerifyWorkingPlots()
{
	int iI;
	CvPlot* pPlot;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
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
	return plotCityXY(m_pCity, pPlot);
}


/// Returns the CvPlot from a Plot Index
CvPlot* CvCityCitizens::GetCityPlotFromIndex(int iIndex) const
{
	return plotCity(m_pCity->getX(), m_pCity->getY(), iIndex);
}




///////////////////////////////////////////////////
// Specialists
///////////////////////////////////////////////////



/// Called at the end of every turn: Looks at the specialists in this City and levels them up
void CvCityCitizens::DoSpecialists()
{
	int iGPThreshold = GetSpecialistUpgradeThreshold();

	int iGPPChange;
	int iCount;
	int iMod;
	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iSpecialistLoop);
		CvSpecialistInfo* pkSpecialistInfo = GC.getSpecialistInfo(eSpecialist);
		if(pkSpecialistInfo)
		{
			// Does this Specialist spawn a GP?
			if (pkSpecialistInfo->getGreatPeopleUnitClass() != NO_UNITCLASS)
			{
				iCount = GetSpecialistCount(eSpecialist);

				// GPP from Specialists
				iGPPChange = pkSpecialistInfo->getGreatPeopleRateChange() * iCount * 100;

				// GPP from Buildings
				iGPPChange += GetBuildingGreatPeopleRateChanges(eSpecialist) * 100;

				if (iGPPChange > 0)
				{
					iMod = 0;

					// City mod
					iMod += GetCity()->getGreatPeopleRateModifier();

					// Player mod
					iMod += GetPlayer()->getGreatPeopleRateModifier();

					// Trait mod to this specific class
					if ((UnitClassTypes)pkSpecialistInfo->getGreatPeopleUnitClass() == GC.getInfoTypeForString("UNITCLASS_SCIENTIST"))
					{
						iMod += GetPlayer()->GetPlayerTraits()->GetGreatScientistRateModifier();
					}

					// Apply mod
					iGPPChange *= (100 + iMod);
					iGPPChange /= 100;

					ChangeSpecialistGreatPersonProgressTimes100(eSpecialist, iGPPChange);
				}

				// Enough to spawn a GP?
				if (GetSpecialistGreatPersonProgress(eSpecialist) >= iGPThreshold)
				{
					// No Minors
					if (!GET_PLAYER(GetCity()->getOwner()).isMinorCiv())
					{
						// Reset progress on this Specialist
						DoResetSpecialistGreatPersonProgressTimes100(eSpecialist);

						// Now... actually create the GP!
						UnitClassTypes eUnitClass = (UnitClassTypes) pkSpecialistInfo->getGreatPeopleUnitClass();
						UnitTypes eUnit = (UnitTypes) GetCity()->getCivilizationInfo().getCivilizationUnits(eUnitClass);

						DoSpawnGreatPerson(eUnit);
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

	int iNumSpecialistsAssigned = GetNumSpecialistsInBuilding(eBuilding);

	if (iNumSpecialistsAssigned < GetCity()->getPopulation() &&	// Limit based on Pop of City
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
			DoRemoveWorstCitizen(true, /*Don't remove this type*/ eSpecialist);
			if (GetNumUnassignedCitizens() == 0)
			{
				// Still nobody, all the citizens may be assigned to the eSpecialist we are looking for, try again
				if (!DoRemoveWorstSpecialist(NO_SPECIALIST, eBuilding))
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

		GetCity()->processSpecialist(eSpecialist, 1);

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
	if (iNumSpecialistsAssigned > 0)
	{
		// Decrease count for the whole city
		m_aiSpecialistCounts[eSpecialist]--;
		m_aiNumSpecialistsInBuilding[eBuilding]--;

		if (bForced)
		{
			m_aiNumForcedSpecialistsInBuilding[eBuilding]--;
		}

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
}


/// Find the worst Specialist and remove him from duty
bool CvCityCitizens::DoRemoveWorstSpecialist(SpecialistTypes eDontChangeSpecialist, const BuildingTypes eDontRemoveFromBuilding /* = NO_BUILDING */)
{
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuildingLoop);

		if (eBuilding == eDontRemoveFromBuilding)
		{
			continue;
		}

		CvBuildingEntry* pkBuildingInfo = GC.getBuildingInfo(eBuilding);
		if(pkBuildingInfo == NULL)
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

	for (int iSpecialistLoop = 0; iSpecialistLoop < GC.getNumSpecialistInfos(); iSpecialistLoop++)
	{
		eSpecialist = (SpecialistTypes) iSpecialistLoop;

		iNumSpecialists += GetSpecialistCount(eSpecialist);
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
	for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
	{
		eBuilding = (BuildingTypes) iBuildingLoop;

		// Have this Building in the City?
		if (GetCity()->GetCityBuildings()->GetNumBuilding(eBuilding) > 0)
		{
			m_aiNumForcedSpecialistsInBuilding[eBuilding] = 0;
		}
	}
}

/// What upgrade progress does a Specialist need to level up?
int CvCityCitizens::GetSpecialistUpgradeThreshold()
{
	int iThreshold = /*100*/ GC.getGREAT_PERSON_THRESHOLD_BASE();

	// Increase threshold based on how many GP have already been spawned
	iThreshold += (/*50*/ GC.getGREAT_PERSON_THRESHOLD_INCREASE() * GET_PLAYER(GetCity()->getOwner()).getGreatPeopleCreated());

	// Game Speed mod
	iThreshold *= GC.getGame().getGameSpeedInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	// Start era mod
	iThreshold *= GC.getGame().getStartEraInfo().getGreatPeoplePercent();
	iThreshold /= 100;

	return iThreshold;
}

/// Create a GP!
void CvCityCitizens::DoSpawnGreatPerson(UnitTypes eUnit)
{
	CvAssert(eUnit != NO_UNIT);

	if (eUnit == NO_UNIT)
		return;	// Better than crashing.

	// If it's the active player then show the popup
	if (GetCity()->getOwner() == GC.getGame().getActivePlayer())
	{
		// Don't show in MP
		if(!GC.getGame().isNetworkMultiPlayer())	// KWG: Candidate for !GC.getGame().IsOption(GAMEOPTION_SIMULTANEOUS_TURNS)
		{
			CvPopupInfo kPopupInfo(BUTTONPOPUP_GREAT_PERSON_REWARD, eUnit, GetCity()->GetID());
			GC.GetEngineUserInterface()->AddPopup(kPopupInfo);
		}
	}

	// Bump up the count
	GET_PLAYER(GetCity()->getOwner()).incrementGreatPeopleCreated();

	GET_PLAYER(GetOwner()).initUnit(eUnit, GetCity()->getX(), GetCity()->getY());

	// Notification
	if (GET_PLAYER(GetOwner()).GetNotifications())
	{
		Localization::String strText = Localization::Lookup("TXT_KEY_NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER");
		Localization::String strSummary = Localization::Lookup("TXT_KEY_NOTIFICATION_SUMMARY_GREAT_PERSON");
		GET_PLAYER(GetOwner()).GetNotifications()->Add(NOTIFICATION_GREAT_PERSON_ACTIVE_PLAYER, strText.toUTF8(), strSummary.toUTF8(), GetCity()->getX(), GetCity()->getY(), eUnit);
	}
}
