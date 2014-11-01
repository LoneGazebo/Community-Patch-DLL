/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */

#include "CvGameCoreDLLPCH.h"
#include "CvGameCoreUtils.h"
#include "ICvDLLUserInterface.h"

#include "LintFree.h"

//=====================================
// CvTreasury
//=====================================

/// Constructor
CvTreasury::CvTreasury():
	m_iGold(0),
	m_iGoldPerTurnFromDiplomacy(0),
	m_iExpensePerTurnUnitMaintenance(0),
	m_iExpensePerTurnUnitSupply(0),
	m_iCityConnectionGoldTimes100(0),
	m_iCityConnectionTradeRouteGoldModifier(0),
	m_iCityConnectionTradeRouteGoldChange(0),
	m_iBaseBuildingGoldMaintenance(0),
	m_iBaseImprovementGoldMaintenance(0),
	m_iLifetimeGrossGoldIncome(0),
	m_pPlayer(NULL)
{

}

/// Destructor
CvTreasury::~CvTreasury()
{
}

/// Initialize
void CvTreasury::Init(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	m_iGold = 0;
	m_iGoldPerTurnFromDiplomacy = 0;
	m_iExpensePerTurnUnitMaintenance = 0;
	m_iExpensePerTurnUnitSupply = 0;
	m_iCityConnectionGoldTimes100 = 0;
	m_iCityConnectionTradeRouteGoldModifier = 0;
	m_iCityConnectionTradeRouteGoldChange = 0;
	m_iBaseBuildingGoldMaintenance = 0;
	m_iBaseImprovementGoldMaintenance = 0;
	m_iLifetimeGrossGoldIncome = 0;

	m_GoldBalanceForTurnTimes100.clear();
	m_GoldBalanceForTurnTimes100.reserve(750);
	m_GoldChangeForTurnTimes100.clear();
	m_GoldChangeForTurnTimes100.reserve(750);
}

/// Deallocate memory created in initialize
void CvTreasury::Uninit()
{

}

/// Update treasury for a turn
void CvTreasury::DoGold()
{
	int iGoldChange;

	iGoldChange = m_pPlayer->calculateGoldRateTimes100();

	int iGoldAfterThisTurn = iGoldChange + GetGoldTimes100();
	if(iGoldAfterThisTurn < 0)
	{
		SetGold(0);

		if(iGoldAfterThisTurn <= /*-5*/ GC.getDEFICIT_UNIT_DISBANDING_THRESHOLD() * 100)
			m_pPlayer->DoDeficit();
	}
	else
	{
		ChangeGoldTimes100(iGoldChange);
	}

	// Update the amount of gold grossed across lifetime of game
	int iGrossGoldChange = CalculateGrossGold();
	if(iGrossGoldChange > 0)
	{
		m_iLifetimeGrossGoldIncome += iGrossGoldChange;
	}

	FAssertMsg(m_GoldBalanceForTurnTimes100.size() <= (unsigned int) GC.getGame().getGameTurn(), "History of Gold Balances corrupted");
	if(m_GoldBalanceForTurnTimes100.size() < (unsigned int) GC.getGame().getGameTurn())
	{
		m_GoldBalanceForTurnTimes100.push_back(GetGoldTimes100());
	}

	FAssertMsg(m_GoldChangeForTurnTimes100.size() <= (unsigned int) GC.getGame().getGameTurn(), "History of Gold Changes corrupted");
	if(m_GoldChangeForTurnTimes100.size() < (unsigned int) GC.getGame().getGameTurn())
	{
		m_GoldChangeForTurnTimes100.push_back(iGoldChange);
	}

#if !defined(NO_ACHIEVEMENTS)
	if (m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		int iGoldDelta = (GetGoldFromCitiesTimes100(false) - GetGoldFromCitiesTimes100(true)) / 100;
		if (iGoldDelta >= 200)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP2_32);
		}
	}
#endif
}

/// Returns current balance in treasury
int CvTreasury::GetGold() const
{
	return m_iGold / 100;
}

/// Sets current balance in treasury
void CvTreasury::SetGold(int iNewValue)
{
	SetGoldTimes100(iNewValue * 100);
}

/// Modifies current balance in treasury
void CvTreasury::ChangeGold(int iChange)
{
	ChangeGoldTimes100(iChange*100);
}

/// Returns current balance in treasury (in hundredths)
int CvTreasury::GetGoldTimes100() const
{
	return m_iGold;
}

/// Sets current balance in treasury (in hundredths)
void CvTreasury::SetGoldTimes100(int iNewValue)
{
	// Minors don't get Gold!
	if(GetGoldTimes100() != iNewValue)
	{
		if(iNewValue < 0)
		{
			CvAssertMsg(false, "GAMEPLAY: Player is being set to a negative Gold value. Please send Jon this with your last 5 autosaves.");
		}

		m_iGold = iNewValue;

		if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
		{
			// Are we now able to buy a Plot when we weren't before?
			//if (GetGold() >= m_pPlayer->GetBuyPlotCost() && iOldGold < m_pPlayer->GetBuyPlotCost() && m_pPlayer->getNumCities() > 0)
			//{
			//	CvString strBuffer = GetLocalizedText("TXT_KEY_NOTIFICATION_ENOUGH_GOLD_TO_BUY_PLOT");
			//	CvString strSummary = GetLocalizedText("TXT_KEY_NOTIFICATION_SUMMARY_ENOUGH_GOLD_TO_BUY_PLOT");
			//	CvNotifications* pNotifications = m_pPlayer->GetNotifications();
			//	if (pNotifications)
			//	{
			//		pNotifications->Add(NOTIFICATION_BUY_TILE, strBuffer, strSummary, -1, -1, -1);
			//	}
			//}

			GC.GetEngineUserInterface()->setDirty(MiscButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(SelectionButtons_DIRTY_BIT, true);
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

/// Modifies current balance in treasury (in hundredths)
void CvTreasury::ChangeGoldTimes100(int iChange)
{
	SetGoldTimes100(GetGoldTimes100() + iChange);
}

// Gold from Cities
int CvTreasury::GetGoldFromCities() const
{
	return GetGoldFromCitiesTimes100() / 100;
}

// Gold from Cities times 100
int CvTreasury::GetGoldFromCitiesTimes100(bool bExcludeTradeRoutes) const
{
	int iGold = 0;

	CvCity* pLoopCity;

	int iLoop;
	for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
	{
		iGold += pLoopCity->getYieldRateTimes100(YIELD_GOLD, bExcludeTradeRoutes);
	}

	return iGold;
}

/// Gold Per Turn from Diplomatic Deals
int CvTreasury::GetGoldPerTurnFromDiplomacy() const
{
	return m_iGoldPerTurnFromDiplomacy;
}

/// Sets Gold Per Turn from Diplomatic Deals
void CvTreasury::SetGoldPerTurnFromDiplomacy(int iValue)
{
	m_iGoldPerTurnFromDiplomacy = iValue;
}

/// Changes Gold Per Turn from Diplomatic Deals
void CvTreasury::ChangeGoldPerTurnFromDiplomacy(int iChange)
{
	SetGoldPerTurnFromDiplomacy(GetGoldPerTurnFromDiplomacy() + iChange);
}

/// Get the amount of gold granted by connecting the city
int CvTreasury::GetCityConnectionRouteGoldTimes100(CvCity* pNonCapitalCity) const
{
	CvCity* pCapitalCity = m_pPlayer->getCapitalCity();
	if(!pNonCapitalCity || pNonCapitalCity == pCapitalCity || pCapitalCity == NULL)
	{
		return 0;
	}

	int iGold = 0;

	int iTradeRouteBaseGold = /*100*/ GC.getTRADE_ROUTE_BASE_GOLD();
	int iTradeRouteCapitalGoldMultiplier = /*0*/ GC.getTRADE_ROUTE_CAPITAL_POP_GOLD_MULTIPLIER();
	int iTradeRouteCityGoldMultiplier = /*125*/ GC.getTRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER();

	iGold += iTradeRouteBaseGold;	// Base Gold: 0
	iGold += (pCapitalCity->getPopulation() * iTradeRouteCapitalGoldMultiplier);	// Capital Multiplier
	iGold += (pNonCapitalCity->getPopulation() * iTradeRouteCityGoldMultiplier);	// City Multiplier
	iGold += GetCityConnectionTradeRouteGoldChange() * 100;

	if(GetCityConnectionTradeRouteGoldModifier() != 0)
	{
		iGold *= (100 + GetCityConnectionTradeRouteGoldModifier());
		iGold /= 100;
	}

	return iGold;
}


/// Returns cached amount of Gold being brought in for having Cities connected via a Route
int CvTreasury::GetCityConnectionGold() const
{
	return m_iCityConnectionGoldTimes100 / 100;
}

/// Returns cached amount of Gold being brought in for having Cities connected via a Route
int CvTreasury::GetCityConnectionGoldTimes100() const
{
	return m_iCityConnectionGoldTimes100;
}

/// How much Gold is being brought in for having Cities connected via a Route
void CvTreasury::DoUpdateCityConnectionGold()
{
	int iNumGold = 0;

	CvCity* pCapitalCity = m_pPlayer->getCapitalCity();

	// Must have a capital before we can check if other Cities are connected to it!
	if(pCapitalCity != NULL && m_pPlayer->getNumCities() > 1)
	{
		CvCity* pLoopCity;

		int iLoop;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(pLoopCity != pCapitalCity)
			{
				if(HasCityConnectionRouteBetweenCities(pCapitalCity, pLoopCity))
				{
					iNumGold += GetCityConnectionRouteGoldTimes100(pLoopCity);
				}
			}
		}
	}

	m_iCityConnectionGoldTimes100 = iNumGold;
}


/// How much of a percent bonus do we get for Trade Routes
int CvTreasury::GetCityConnectionTradeRouteGoldModifier() const
{
	return m_iCityConnectionTradeRouteGoldModifier;
}

/// Changes how much of a percent bonus do we get for Trade Routes
void CvTreasury::ChangeCityConnectionTradeRouteGoldModifier(int iChange)
{
	if(iChange != 0)
	{
		m_iCityConnectionTradeRouteGoldModifier += iChange;

		DoUpdateCityConnectionGold();
	}
}

/// How much of a bonus do we get for Trade Routes
int CvTreasury::GetCityConnectionTradeRouteGoldChange() const
{
	return m_iCityConnectionTradeRouteGoldChange;
}

/// Changes how much of a bonus we get for Trade Routes
void CvTreasury::ChangeCityConnectionTradeRouteGoldChange(int iChange)
{
	if(iChange != 0)
	{
		m_iCityConnectionTradeRouteGoldChange += iChange;

		DoUpdateCityConnectionGold();
	}
}

/// Returns the route-type between two cities
#if defined(MOD_EVENTS_CITY_CONNECTIONS)
bool CvTreasury::HasCityConnectionRouteBetweenCities(CvCity* pFirstCity, CvCity* pSecondCity) const
#else
bool CvTreasury::HasCityConnectionRouteBetweenCities(CvCity* pFirstCity, CvCity* pSecondCity, bool bBestRoute) const
#endif
{
	CvCityConnections* pCityConnections = m_pPlayer->GetCityConnections();
	FASSERT(pCityConnections, "m_pCityConnections is null");
	if(!pCityConnections)
	{
		// invalid value
		return FALSE;
	}

	FASSERT(pFirstCity, "pFirstCity is null. Invalid!");
	FASSERT(pSecondCity, "pSecondCity is null. Invalid!");
	if(!pFirstCity || !pSecondCity)
	{
		// null city passed in
		return FALSE;
	}

	FASSERT(pFirstCity != pSecondCity, "Cities are the same and are by default connected?");
	if(pFirstCity == pSecondCity)
	{
		// trying to go to the same city
		return FALSE;
	}

	int iFirstCityIndex = -1;
	int iSecondCityIndex = -1;
	for(uint ui = 0; ui < pCityConnections->m_aiCityPlotIDs.size(); ui++)
	{
		if(pFirstCity->plot()->GetPlotIndex() == pCityConnections->m_aiCityPlotIDs[ui])
		{
			iFirstCityIndex = ui;
		}

		if(pSecondCity->plot()->GetPlotIndex() == pCityConnections->m_aiCityPlotIDs[ui])
		{
			iSecondCityIndex = ui;
		}

		if(iFirstCityIndex >= 0 && iSecondCityIndex >= 0)
		{
			break;
		}
	}

	if(iFirstCityIndex < 0 || iSecondCityIndex < 0)
	{
		// did not find one or both of the cities
		return FALSE;
	}

	CvCityConnections::RouteInfo* pRouteInfo = pCityConnections->GetRouteInfo(iFirstCityIndex, iSecondCityIndex);
	if(pRouteInfo)
	{
#if !defined(MOD_EVENTS_CITY_CONNECTIONS)
		if(bBestRoute)
		{
			return pRouteInfo->m_cRouteState & CvCityConnections::HAS_BEST_ROUTE;
		}
		else
#endif
		{
			return pRouteInfo->m_cRouteState & CvCityConnections::HAS_ANY_ROUTE;
		}
	}

	return FALSE;
}

/// Gold per turn from international trade routes
int CvTreasury::GetGoldPerTurnFromTradeRoutes() const
{
	// needs fixin
	return 0;
	//return GetGoldPerTurnFromTradeRoutesTimes100() / 100;
}

/// Gold per turn from international trade routes times 100
int CvTreasury::GetGoldPerTurnFromTradeRoutesTimes100() const
{
	return 0;
	//return m_pPlayer->GetTrade()->GetAllTradeValueTimes100(YIELD_GOLD);
}

/// Gold per turn from traits
int CvTreasury::GetGoldPerTurnFromTraits() const
{
	return m_pPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner(YIELD_GOLD) * m_pPlayer->GetTrade()->GetNumDifferentTradingPartners();
}

/// Gold Per Turn from Religion
int CvTreasury::GetGoldPerTurnFromReligion() const
{
	int iGoldFromReligion = 0;

#if defined(MOD_API_UNIFIED_YIELDS)
	iGoldFromReligion += m_pPlayer->GetYieldPerTurnFromReligion(YIELD_GOLD);
#else
	CvGameReligions* pReligions = GC.getGame().GetGameReligions();

	// Founder beliefs
	ReligionTypes eFoundedReligion = pReligions->GetFounderBenefitsReligion(m_pPlayer->GetID());
	if(eFoundedReligion != NO_RELIGION)
	{
		const CvReligion* pReligion = pReligions->GetReligion(eFoundedReligion, NO_PLAYER);
		if(pReligion)
		{
			int iGoldPerFollowingCity = pReligion->m_Beliefs.GetGoldPerFollowingCity();
			iGoldFromReligion += (pReligions->GetNumCitiesFollowing(eFoundedReligion) * iGoldPerFollowingCity);

			int iGoldPerXFollowers = pReligion->m_Beliefs.GetGoldPerXFollowers();
			if(iGoldPerXFollowers > 0)
			{
				iGoldFromReligion += (pReligions->GetNumFollowers(eFoundedReligion) / iGoldPerXFollowers);
			}

#if defined(MOD_API_UNIFIED_YIELDS)
			iGoldFromReligion += m_pPlayer->GetYieldPerTurnFromReligion(YIELD_GOLD);
#endif
		}
	}
#endif

	return iGoldFromReligion;
}

/// Gross income for turn times 100
int CvTreasury::CalculateGrossGold()
{
	return CalculateGrossGoldTimes100() / 100;
}

/// Gross income for turn
int CvTreasury::CalculateGrossGoldTimes100()
{
	int iNetGold;

	// Gold from Cities
	iNetGold = GetGoldFromCitiesTimes100();

	// Gold per Turn from Diplomacy
	iNetGold += GetGoldPerTurnFromDiplomacy() * 100;

	// City connection bonuses
	iNetGold += GetCityConnectionGoldTimes100();

	// Religion
	iNetGold += GetGoldPerTurnFromReligion() * 100;

	// International trade
	iNetGold += GetGoldPerTurnFromTraits() * 100;

#if defined(MOD_BALANCE_CORE_HAPPINESS_NATIONAL)
	//Mod for national unhappiness
	if(MOD_BALANCE_CORE_HAPPINESS_NATIONAL)
	{
		//Mechanic to allow for growth malus from happiness/unhappiness.
		int iHappiness = 0;
		iHappiness += (m_pPlayer->GetHappiness() - m_pPlayer->GetSetUnhappiness());

		//If Happiness is greater than or over threshold, calculate city bonus mod.
		if(iHappiness >= GC.getBALANCE_HAPPINESS_THRESHOLD())
		{
			iHappiness = (iHappiness - GC.getBALANCE_HAPPINESS_THRESHOLD());
			//Are there minimums/maximums for the bonus? Restrict this value.
			if(iHappiness > GC.getBALANCE_HAPPINESS_BONUS_MAXIMUM())
			{
				iHappiness = GC.getBALANCE_HAPPINESS_BONUS_MAXIMUM();
			}
			else if(iHappiness < GC.getBALANCE_HAPPINESS_BONUS_MINIMUM())
			{
				iHappiness = GC.getBALANCE_HAPPINESS_BONUS_MINIMUM();
			}
			
		}
		//If happiness is less than the main threshold, calculate city penalty mod.
		else if(iHappiness < GC.getBALANCE_HAPPINESS_THRESHOLD_MAIN())
		{
			//Are there minimums/maximums for the penalty? Restrict this value.
			if(iHappiness > GC.getBALANCE_HAPPINESS_PENALTY_MINIMUM())
			{
				iHappiness = GC.getBALANCE_HAPPINESS_PENALTY_MINIMUM();
			}
			else if(iHappiness < GC.getBALANCE_HAPPINESS_PENALTY_MAXIMUM())
			{
				iHappiness = GC.getBALANCE_HAPPINESS_PENALTY_MAXIMUM();
			}
		}
		if(iHappiness != 0)
		{
			iNetGold += ((iNetGold * /*5*/ GC.getBALANCE_HAPPINESS_GOLD_MODIFIER() * iHappiness) / 100);
		}
	}
#endif

	return iNetGold;
}
#if defined(MOD_BALANCE_CORE_HAPPINESS_NATIONAL)
/// Gross income for turn
int CvTreasury::CalculateGrossGoldTimes100ForUI()
{
	int iNetGold;

	// Gold from Cities
	iNetGold = GetGoldFromCitiesTimes100();

	// Gold per Turn from Diplomacy
	iNetGold += GetGoldPerTurnFromDiplomacy() * 100;

	// City connection bonuses
	iNetGold += GetCityConnectionGoldTimes100();

	// Religion
	iNetGold += GetGoldPerTurnFromReligion() * 100;

	// International trade
	iNetGold += GetGoldPerTurnFromTraits() * 100;

	return iNetGold;
}
#endif
/// Gross income across entire game
int CvTreasury::GetLifetimeGrossGold()
{
	return m_iLifetimeGrossGoldIncome;
}

/// Net income for turn times 100
int CvTreasury::CalculateBaseNetGold()
{
	return CalculateBaseNetGoldTimes100() / 100;
}

/// Net income for turn
int CvTreasury::CalculateBaseNetGoldTimes100()
{
	int iNetGold = CalculateGrossGoldTimes100();

	// Remove costs
	iNetGold -= CalculateInflatedCosts() * 100;

	return iNetGold;
}


/// Compute unit maintenance cost for the turn (returns component info)
int CvTreasury::CalculateUnitCost(int& iFreeUnits, int& iPaidUnits, int& iBaseUnitCost, int& iExtraCost)
{
	// If player has 0 Cities then no Unit cost
	if(m_pPlayer->getNumCities() == 0)
	{
		return 0;
	}

	int iSupport = 0;

	CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();
	iFreeUnits = playerHandicap.getGoldFreeUnits();

	// Defined in XML by unit info type
	iFreeUnits += m_pPlayer->GetNumMaintenanceFreeUnits();
	iFreeUnits += m_pPlayer->getBaseFreeUnits();

	iPaidUnits = max(0, m_pPlayer->getNumUnits() - iFreeUnits);

	iBaseUnitCost = iPaidUnits * m_pPlayer->getGoldPerUnitTimes100();

	// Discount on land unit maintenance?
	int iLandUnitMod = m_pPlayer->GetPlayerTraits()->GetLandUnitMaintenanceModifier();
	if(iLandUnitMod != 0)
	{
		int iLandUnits = m_pPlayer->GetNumUnitsWithDomain(DOMAIN_LAND, true /*bMilitaryOnly*/);
		int iFreeLandUnits = m_pPlayer->GetNumMaintenanceFreeUnits(DOMAIN_LAND, true);
		int iPaidLandUnits = iLandUnits - iFreeLandUnits;
		iBaseUnitCost += (iLandUnitMod * iPaidLandUnits * m_pPlayer->getGoldPerUnitTimes100()) / 100;
	}

	// Discount on naval unit maintenance?
	int iNavalUnitMod = m_pPlayer->GetPlayerTraits()->GetNavalUnitMaintenanceModifier();
	if(iNavalUnitMod != 0)
	{
		int iNavalUnits = m_pPlayer->GetNumUnitsWithDomain(DOMAIN_SEA, true /*bMilitaryOnly*/);
		int iFreeNavalUnits = m_pPlayer->GetNumMaintenanceFreeUnits(DOMAIN_SEA, true);
		int iPaidNavalUnits = iNavalUnits - iFreeNavalUnits;
		iBaseUnitCost += (iNavalUnitMod * iPaidNavalUnits * m_pPlayer->getGoldPerUnitTimes100()) / 100;
	}

	// Discounts for units of certain UnitCombat classes
	for(int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		const UnitCombatTypes eUnitCombatClass = static_cast<UnitCombatTypes>(iI);
		CvBaseInfo* pkUnitCombatClassInfo = GC.getUnitCombatClassInfo(eUnitCombatClass);
		if(pkUnitCombatClassInfo)
		{
			int iModifier = m_pPlayer->GetPlayerTraits()->GetMaintenanceModifierUnitCombat(eUnitCombatClass);
			if (iModifier != 0)
			{
				int iNumUnits = m_pPlayer->GetNumUnitsWithUnitCombat(eUnitCombatClass);
				int iCost = iNumUnits * m_pPlayer->getGoldPerUnitTimes100(); 
				int iModifiedCost = iNumUnits * m_pPlayer->getGoldPerUnitTimes100() * (100 + iModifier) / 100; 
				
				// Reduce cost based on difference
				iBaseUnitCost += (iModifiedCost - iCost);
			}
		}
	}

	iExtraCost = m_pPlayer->getExtraUnitCost() * 100;	// In hundreds to avoid rounding errors

	iSupport = iBaseUnitCost + iExtraCost;

	// Game progress factor ranges from 0.0 to 1.0 based on how far into the game we are
	double fGameProgressFactor = double(GC.getGame().getElapsedGameTurns()) / GC.getGame().getDefaultEstimateEndTurn();

	// Multiplicative increase - helps scale costs as game goes on - the HIGHER this number the more is paid
	double fMultiplyFactor = 1.0 + (fGameProgressFactor* /*8*/ GC.getUNIT_MAINTENANCE_GAME_MULTIPLIER());
	// Exponential increase - this one really punishes those with a HUGE military - the LOWER this number the more is paid
	double fExponentialFactor = 1.0 + (fGameProgressFactor / /*7*/ GC.getUNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR());

	double fTempCost = fMultiplyFactor * iSupport;
	fTempCost /= 100;	// Take this out of hundreds now

	double dFinalCost = pow(fTempCost, fExponentialFactor);

	// A mod at the player level? (Policies, etc.)
	if(m_pPlayer->GetUnitGoldMaintenanceMod() != 0)
	{
		dFinalCost *= (100 + m_pPlayer->GetUnitGoldMaintenanceMod());
		dFinalCost /= 100;
	}

	// Human bonus for unit maintenance costs
	if(m_pPlayer->isHuman())
	{
		dFinalCost *= playerHandicap.getUnitCostPercent();
		dFinalCost /= 100;
	}
	// AI bonus for unit maintenance costs
	else if(!m_pPlayer->IsAITeammateOfHuman())
	{
		dFinalCost *= GC.getGame().getHandicapInfo().getAIUnitCostPercent();
		dFinalCost /= 100;
	}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES) {
		// Vassal bonus for unit maintenance costs
		for(int iI = 0; iI < MAX_TEAMS; iI++)
		{
			// Are we the vassal of team?
			if(GET_TEAM(m_pPlayer->getTeam()).IsVassal((TeamTypes)iI))
			{
				dFinalCost *= (100 - /*40*/GC.getVASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT());
				dFinalCost /= 100;
			}
		}
	}
#endif

	//iFinalCost /= 100;

	return std::max(0, int(dFinalCost));
}

/// Compute unit supply for the turn (returns component info)
int CvTreasury::CalculateUnitSupply(int& iPaidUnits, int& iBaseSupplyCost)
{
	int iSupply;

	iPaidUnits = std::max(0, (m_pPlayer->getNumOutsideUnits() - /*3*/ GC.getINITIAL_FREE_OUTSIDE_UNITS()));

	// JON: This is set to 0 right now, which pretty much means it's disabled
	iBaseSupplyCost = iPaidUnits* /*0*/ GC.getINITIAL_OUTSIDE_UNIT_GOLD_PERCENT();
	iBaseSupplyCost /= 100;

	iSupply = iBaseSupplyCost;

	CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();
	iSupply *= playerHandicap.getUnitCostPercent();
	iSupply /= 100;

	if(!m_pPlayer->isHuman() && !m_pPlayer->IsAITeammateOfHuman() && !m_pPlayer->isBarbarian())
	{
		//iSupply *= gameHandicap->getAIUnitSupplyPercent();	// This is no longer valid
		//iSupply /= 100;

		iSupply *= std::max(0, ((GC.getGame().getHandicapInfo().getAIPerEraModifier() * m_pPlayer->GetCurrentEra()) + 100));
		iSupply /= 100;
	}

	// Game progress factor ranges from 0.0 to 1.0 based on how far into the game we are
	double fGameProgressFactor = float(GC.getGame().getElapsedGameTurns()) / GC.getGame().getEstimateEndTurn();

	// Multiplicative increase - helps scale costs as game goes on - the HIGHER this number the more is paid
	double fMultiplyFactor = 1.0 + (fGameProgressFactor* /*8*/ GC.getUNIT_MAINTENANCE_GAME_MULTIPLIER());
	// Exponential increase - this one really punishes those with a HUGE military - the LOWER this number the more is paid
	double fExponentialFactor = 1.0 + (fGameProgressFactor / /*7*/ GC.getUNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR());

	double fTempCost = fMultiplyFactor * iSupply;
	int iFinalCost = (int) pow(fTempCost, fExponentialFactor);

	// A mod at the player level? (Policies, etc.)
	if(m_pPlayer->GetUnitSupplyMod() != 0)
	{
		iFinalCost *= (100 + m_pPlayer->GetUnitSupplyMod());
		iFinalCost /= 100;
	}

	CvAssert(iFinalCost >= 0);

	return iFinalCost;
}

/// Costs to the player (prior to applying inflation)
int CvTreasury::CalculatePreInflatedCosts()
{
	int iFreeUnits;
	int iPaidUnits;
	int iBaseUnitCost;
	int iExtraCost;
	int iBaseSupplyCost;

	m_iExpensePerTurnUnitMaintenance = CalculateUnitCost(iFreeUnits, iPaidUnits, iBaseUnitCost, iExtraCost);
	m_iExpensePerTurnUnitSupply = CalculateUnitSupply(iPaidUnits, iBaseSupplyCost);

	int iTotalCosts = 0;

	iTotalCosts += m_iExpensePerTurnUnitMaintenance;
	iTotalCosts += m_iExpensePerTurnUnitSupply;
	iTotalCosts += GetBuildingGoldMaintenance();
	iTotalCosts += GetImprovementGoldMaintenance();

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	if (MOD_DIPLOMACY_CIV4_FEATURES) {
		iTotalCosts += GetVassalGoldMaintenance();
	}
#endif

	return iTotalCosts;
}

/// Compute inflation for this part of the game
int CvTreasury::CalculateInflationRate()
{
	CvGame& kGame = GC.getGame();

	CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();
	CvHandicapInfo& gameHandicap = kGame.getHandicapInfo();
	CvGameSpeedInfo& gameSpeedInfo = kGame.getGameSpeedInfo();

	int iTurns = ((kGame.getGameTurn() + kGame.getElapsedGameTurns()) / 2);
	iTurns += gameSpeedInfo.getInflationOffset();

	if(iTurns <= 0)
	{
		return 0;
	}

	int iInflationPerTurnTimes10000 = gameSpeedInfo.getInflationPercent();
	iInflationPerTurnTimes10000 *= playerHandicap.getInflationPercent();
	iInflationPerTurnTimes10000 /= 100;

	int iModifier = 0;

	if(!m_pPlayer->isHuman() && !m_pPlayer->isBarbarian())
	{
		int iAIModifier = gameHandicap.getAIInflationPercent();
		iAIModifier *= std::max(0, ((gameHandicap.getAIPerEraModifier() * m_pPlayer->GetCurrentEra()) + 100));
		iAIModifier /= 100;

		iModifier += iAIModifier - 100;
	}

	iInflationPerTurnTimes10000 *= std::max(0, 100 + iModifier);
	iInflationPerTurnTimes10000 /= 100;

	// Keep up to second order terms in binomial series
	int iRatePercent = (iTurns * iInflationPerTurnTimes10000) / 100;
	iRatePercent += (iTurns * (iTurns - 1) * iInflationPerTurnTimes10000 * iInflationPerTurnTimes10000) / 2000000;

	CvAssert(iRatePercent >= 0);

	return iRatePercent;
}

/// Apply inflation - JON: DISABLED. Inflation bad. Money good.
int CvTreasury::CalculateInflatedCosts()
{
	int iCosts = CalculatePreInflatedCosts();

	//iCosts *= std::max(0, (CalculateInflationRate() + 100));
	//iCosts /= 100;

	return iCosts;
}

/// What are our gold maintenance costs because of Buildings?
int CvTreasury::GetBuildingGoldMaintenance() const
{
	int iMaintenance = GetBaseBuildingGoldMaintenance();

	// Player modifier
	iMaintenance *= (100 + m_pPlayer->GetBuildingGoldMaintenanceMod());
	iMaintenance /= 100;

	// Modifier for difficulty level
	CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();
	//iMaintenance *= playerHandicap->getBuildingCostPercent();
	//iMaintenance /= 100;

	// Human bonus for Building maintenance costs
	if(m_pPlayer->isHuman())
	{
		iMaintenance *= playerHandicap.getBuildingCostPercent();
		iMaintenance /= 100;
	}
	// AI bonus for Building maintenance costs
	else if(!m_pPlayer->IsAITeammateOfHuman())
	{
		iMaintenance *= GC.getGame().getHandicapInfo().getAIBuildingCostPercent();
		iMaintenance /= 100;
	}

	// Start Era mod
	iMaintenance *= GC.getGame().getStartEraInfo().getBuildingMaintenancePercent();
	iMaintenance /= 100;

	return iMaintenance;
}

/// What are our BASE gold maintenance costs because of Buildings?
int CvTreasury::GetBaseBuildingGoldMaintenance() const
{
	return m_iBaseBuildingGoldMaintenance;
}

/// What are our gold maintenance costs because of Buildings?
void CvTreasury::SetBaseBuildingGoldMaintenance(int iValue)
{
	m_iBaseBuildingGoldMaintenance = iValue;

	if(m_iBaseBuildingGoldMaintenance < 0)
		m_iBaseBuildingGoldMaintenance = 0;

	CvAssertMsg(m_iBaseBuildingGoldMaintenance >= 0, "Building Maintenance is negative somehow. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");
}

/// What are our gold maintenance costs because of Buildings?
void CvTreasury::ChangeBaseBuildingGoldMaintenance(int iChange)
{
	SetBaseBuildingGoldMaintenance(GetBaseBuildingGoldMaintenance() + iChange);
}

/// What are our gold maintenance costs because of Improvements?
int CvTreasury::GetImprovementGoldMaintenance() const
{
	int iMaintenance = m_iBaseImprovementGoldMaintenance;

	// Player modifier
	iMaintenance *= (100 + m_pPlayer->GetRouteGoldMaintenanceMod());
	iMaintenance /= 100;

	// Handicap
	iMaintenance *= m_pPlayer->getHandicapInfo().getRouteCostPercent();
	iMaintenance /= 100;

	return iMaintenance;
}

/// What are our gold maintenance costs because of Improvements?
int CvTreasury::GetBaseImprovementGoldMaintenance() const
{
	return m_iBaseImprovementGoldMaintenance;
}

/// What are our gold maintenance costs because of Improvements?
void CvTreasury::SetBaseImprovementGoldMaintenance(int iValue)
{
	if(GetBaseImprovementGoldMaintenance() != iValue)
	{
		m_iBaseImprovementGoldMaintenance = iValue;

		CvAssertMsg(m_iBaseImprovementGoldMaintenance >= 0, "Improvement Maintenance is negative somehow. Please send Jon this with your last 5 autosaves and what changelist # you're playing.");

		if(m_iBaseImprovementGoldMaintenance < 0)
			m_iBaseImprovementGoldMaintenance = 0;

		if(m_pPlayer->GetID() == GC.getGame().getActivePlayer())
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}

/// What are our gold maintenance costs because of Improvements?
void CvTreasury::ChangeBaseImprovementGoldMaintenance(int iChange)
{
	SetBaseImprovementGoldMaintenance(GetBaseImprovementGoldMaintenance() + iChange);
}

/// Average change in gold balance over N turns
double CvTreasury::AverageIncome(int iTurns)
{
	CvAssertMsg(iTurns > 0, "Invalid number of turns parameter");

	if(m_GoldChangeForTurnTimes100.size() > 0)
	{
		int iSamples = 0;
		int iIndex = m_GoldChangeForTurnTimes100.size() - 1;
		int iTotal = 0;

		while(iSamples < iTurns)
		{
			iTotal += m_GoldChangeForTurnTimes100[iIndex];
			iSamples++;
		}

		return ((double)iTotal / (double)iSamples / 100);
	}

	return 0;
}

void CvTreasury::LogExpenditure(CvString strExpenditure, int iAmount, int iColumn)
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
		strLogName = "ExpenditureLog_" + strPlayerName + ".csv";
	}
	else
	{
		strLogName = "ExpenditureLog.csv";
	}


	FILogFile* pLog;
	pLog = LOGFILEMGR.GetLog(strLogName, FILogFile::kDontTimeStamp);

	CvString str;

	// civ name
	TreasuryHelpers::AppendToLog(strHeader, strLog, "Civ Name", strPlayerName);

	// turn
	TreasuryHelpers::AppendToLog(strHeader, strLog, "Turn", GC.getGame().getGameTurn());

	// treasury at turn start
	TreasuryHelpers::AppendToLog(strHeader, strLog, "Treasury Before Purchase", m_pPlayer->GetTreasury()->GetGold());
	
	// Cost of Plot
	if (iColumn == 1){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Cost of Plot:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Cost of Plot:", 0);
	}
	
	// City Expenditure and Amount
	if (iColumn == 2){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City Bought:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City Spent:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City Bought:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City Spent:", 0);
	}
	
	// Unit Upgrade and cost
	if (iColumn == 3){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Unit Upgraded:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Spent on Upgrade:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Unit Upgrade:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Spent on Upgrade:", 0);
	}

	// City State Gifted and Amount
	if (iColumn == 4){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Gold Gifted:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Gold Gift Amount:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Gold Gifted:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Gold Gift Amount:", 0);
	}
	
	// City State Improved and Amount
	if (iColumn == 5){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Improved:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Gold for Improvement Amount:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Improved:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Gold for Improvement Amount:", 0);
	}

	// City State Bought and Amount
	if (iColumn == 6){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Bought:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Bought for:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "City State Bought:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Bought for:", 0);
	}
	
	// Emergency City Expenditure Unit and Amount
	if (iColumn == 7){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Unit:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Unit Cost:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Unit:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Unit Cost:", 0);
	}
	
	// Emergency City Expenditure Unit and Amount
	if (iColumn == 8){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Building:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Building Cost:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Building:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Emergency Building Cost:", 0);
	}

	// Research Agreement and Amount
	if (iColumn == 9){
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Research Agreement with:", strExpenditure);
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Research Agreement Cost:", iAmount);
	}
	else{
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Research Agreement with:", "");
		TreasuryHelpers::AppendToLog(strHeader, strLog, "Research Agreement Cost:", 0);
	}

	if(bBuildHeader)
	{
		pLog->Msg(strHeader);
	}
	pLog->Msg(strLog);
}

/// Serialization read
void CvTreasury::Read(FDataStream& kStream)
{
	// Version number to maintain backwards compatibility
	uint uiVersion;

	kStream >> uiVersion;
	MOD_SERIALIZE_INIT_READ(kStream);

	kStream >> m_iGold;
	kStream >> m_iGoldPerTurnFromDiplomacy;
	kStream >> m_iExpensePerTurnUnitMaintenance;
	kStream >> m_iExpensePerTurnUnitSupply;
	kStream >> m_iCityConnectionGoldTimes100;
	kStream >> m_iCityConnectionTradeRouteGoldModifier;
	kStream >> m_iCityConnectionTradeRouteGoldChange;
	kStream >> m_iBaseBuildingGoldMaintenance;
	kStream >> m_iBaseImprovementGoldMaintenance;
	kStream >> m_GoldBalanceForTurnTimes100;
	kStream >> m_GoldChangeForTurnTimes100;
	kStream >> m_iLifetimeGrossGoldIncome;
}

/// Serialization write
void CvTreasury::Write(FDataStream& kStream)
{
	// Current version number
	uint uiVersion = 1;
	kStream << uiVersion;
	MOD_SERIALIZE_INIT_WRITE(kStream);

	kStream << m_iGold;
	kStream << m_iGoldPerTurnFromDiplomacy;
	kStream << m_iExpensePerTurnUnitMaintenance;
	kStream << m_iExpensePerTurnUnitSupply;
	kStream << m_iCityConnectionGoldTimes100;
	kStream << m_iCityConnectionTradeRouteGoldModifier;
	kStream << m_iCityConnectionTradeRouteGoldChange;
	kStream << m_iBaseBuildingGoldMaintenance;
	kStream << m_iBaseImprovementGoldMaintenance;
	kStream << m_GoldBalanceForTurnTimes100;
	kStream << m_GoldChangeForTurnTimes100;
	kStream << m_iLifetimeGrossGoldIncome;
}

void TreasuryHelpers::AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, CvString strValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	strLog += strValue;
	strLog += ",";
}

void TreasuryHelpers::AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, int iValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%d,", iValue);
	strLog += str;
}

void TreasuryHelpers::AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, float fValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%.2f,", fValue);
	strLog += str;
}

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
// What are our gold maintenance costs because of Vassals?
int CvTreasury::GetVassalGoldMaintenance() const
{
	int iRtnValue = 0;
	// We have a vassal
	for(int iI = 0; iI < MAX_MAJOR_CIVS; iI++)
	{
		if(!GET_PLAYER((PlayerTypes)iI).isMinorCiv()
			&& !GET_PLAYER((PlayerTypes)iI).isBarbarian()
			&& GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			int iLoop, iCityPop;
			// This player is our vassal
			if(GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).IsVassal(m_pPlayer->getTeam()))
			{
				// Loop through our vassal's cities
				for(CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
				{
					iCityPop = pLoopCity->getPopulation();
					iRtnValue += std::max(0, (int)(pow((double)iCityPop, (double)/*0.6*/ GC.getVASSALAGE_VASSAL_CITY_POP_EXPONENT())));
				}

				iRtnValue += std::max(0, (GET_PLAYER((PlayerTypes)iI).GetTreasury()->GetExpensePerTurnUnitMaintenance() * /*40*/GC.getVASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT() / 100));
			}
		}
	}

	// Modifier for vassal maintenance?
	iRtnValue *= (100 + m_pPlayer->GetVassalGoldMaintenanceMod());
	iRtnValue /= 100;

	return iRtnValue;
}
#endif
