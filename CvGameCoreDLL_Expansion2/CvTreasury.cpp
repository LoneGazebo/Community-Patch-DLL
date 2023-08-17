/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
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
	m_iCityConnectionGoldTimes100(0),
	m_iCityConnectionTradeRouteGoldModifier(0),
	m_iCityConnectionTradeRouteGoldChange(0),
	m_iBaseBuildingGoldMaintenance(0),
	m_iBaseImprovementGoldMaintenance(0),
	m_iLifetimeGrossGoldIncome(0),
#if defined(MOD_BALANCE_CORE)
	m_iInternalTradeGoldBonus(0),
#endif
	m_iExpensePerTurnFromVassalTax(0),
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
	m_iCityConnectionGoldTimes100 = 0;
	m_iCityConnectionTradeRouteGoldModifier = 0;
	m_iCityConnectionTradeRouteGoldChange = 0;
	m_iBaseBuildingGoldMaintenance = 0;
	m_iBaseImprovementGoldMaintenance = 0;
	m_iLifetimeGrossGoldIncome = 0;
#if defined(MOD_BALANCE_CORE)
	m_iInternalTradeGoldBonus = 0;
#endif
	m_iExpensePerTurnFromVassalTax = 0;

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
	int iGoldChange = CalculateBaseNetGoldTimes100();
	int iGoldAfterThisTurn = iGoldChange + GetGoldTimes100();

	if (iGoldAfterThisTurn < 0 || m_pPlayer->isMinorCiv())
	{
		SetGold(0);

		//forced disbanding
		if (iGoldAfterThisTurn <= /*-5*/ GD_INT_GET(DEFICIT_UNIT_DISBANDING_THRESHOLD) * 100)
			m_pPlayer->DoBankruptcy();
	}
	else
	{
		ChangeGoldTimes100(iGoldChange);
	}

	//instant yields are tracked in ChangeGoldTimes100. we ignore expenses (negative values) there.
	//but here we have to consider a negative gold rate, so fix it after the fact
	if (iGoldChange < 0)
	{
		if (m_GoldChangeForTurnTimes100.size() < (size_t)GC.getGame().getGameTurn())
			m_GoldChangeForTurnTimes100.push_back(iGoldChange);
		else
			m_GoldChangeForTurnTimes100.back() += iGoldChange;
	}

	// Update the amount of gold grossed across lifetime of game
	int iGrossGoldChange = CalculateGrossGold();
	if(iGrossGoldChange > 0)
	{
		m_iLifetimeGrossGoldIncome += iGrossGoldChange;
	}

	if(m_GoldBalanceForTurnTimes100.size() < (unsigned int) GC.getGame().getGameTurn())
	{
		m_GoldBalanceForTurnTimes100.push_back(GetGoldTimes100());
	}

	if (MOD_API_ACHIEVEMENTS && m_pPlayer->isHuman() && !GC.getGame().isGameMultiPlayer())
	{
		int iGoldDelta = (GetGoldFromCitiesTimes100(false) - GetGoldFromCitiesTimes100(true)) / 100;
		if (iGoldDelta >= 200)
		{
			gDLL->UnlockAchievement(ACHIEVEMENT_XP2_32);
		}
	}
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

		m_iGold = max(0,iNewValue);

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

	//track the income for each turn (instant yields and regular)
	if (iChange > 0)
	{
		if (m_GoldChangeForTurnTimes100.size() < (size_t)GC.getGame().getGameTurn())
			m_GoldChangeForTurnTimes100.push_back(iChange);
		else
			m_GoldChangeForTurnTimes100.back() += iChange;
	}
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

	CvCity* pLoopCity = NULL;

	int iLoop = 0;
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
	if (!pNonCapitalCity || !pCapitalCity || pNonCapitalCity == pCapitalCity)
		return 0;

	int iGold = /*-100*/ GD_INT_GET(TRADE_ROUTE_BASE_GOLD);
	iGold += pCapitalCity->getPopulation() * /*15 in CP, 6 in VP*/ GD_INT_GET(TRADE_ROUTE_CAPITAL_POP_GOLD_MULTIPLIER);	// Capital Multiplier
	iGold += pNonCapitalCity->getPopulation() * /*110 in CP, 50 in VP*/ GD_INT_GET(TRADE_ROUTE_CITY_POP_GOLD_MULTIPLIER);	// City Multiplier
	iGold += GetCityConnectionTradeRouteGoldChange() * 100;

	int iMod = GetCityConnectionTradeRouteGoldModifier() + pNonCapitalCity->GetCityConnectionTradeRouteGoldModifier();
	if (iMod != 0)
	{
		iGold *= (100 + iMod);
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
		CvCity* pLoopCity = NULL;

		int iLoop = 0;
		for(pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if(pLoopCity != pCapitalCity)
			{
				if(pLoopCity->IsConnectedToCapital())
				{
					iNumGold += GetCityConnectionRouteGoldTimes100(pLoopCity);
				}
			}
		}
	}

	m_iCityConnectionGoldTimes100 = iNumGold;
}
#if defined(MOD_BALANCE_CORE_POLICIES)
/// How much of a percent bonus do we get for Trade Routes
int CvTreasury::GetInternalTradeRouteGoldBonus() const
{
	return m_iInternalTradeGoldBonus;
}
/// How much of a percent bonus do we get for Trade Routes
void CvTreasury::DoInternalTradeRouteGoldBonus()
{
	int iGold = 0;
	//Bonus for Internal Trade Routes Gold Policy added in here (for LUA simplicity)
	if(m_pPlayer->IsGoldInternalTrade())
	{
		int iInternalTradeRoutes = m_pPlayer->GetTrade()->GetNumberOfInternalTradeRoutes();
		if(iInternalTradeRoutes > 0)
		{
			iGold += (iInternalTradeRoutes * /*10*/ m_pPlayer->GetGoldInternalTrade());
		}
	}
	m_iInternalTradeGoldBonus = iGold;
}
#endif


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

/// Gold per turn from international trade routes
int CvTreasury::GetGoldPerTurnFromTradeRoutes() const
{
	return GetGoldPerTurnFromTradeRoutesTimes100() / 100;
}

/// Gold per turn from international trade routes times 100
int CvTreasury::GetGoldPerTurnFromTradeRoutesTimes100() const
{
	return m_pPlayer->GetTrade()->GetAllTradeValueTimes100(YIELD_GOLD);
}

/// Gold per turn from traits
int CvTreasury::GetGoldPerTurnFromTraits() const
{
#if defined(MOD_BALANCE_CORE)
	if(MOD_BALANCE_YIELD_SCALE_ERA)
	{
		int iEra = m_pPlayer->GetCurrentEra();
		if(iEra < 1)
		{
			iEra = 1;
		}
		return ((iEra * m_pPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner(YIELD_GOLD) * m_pPlayer->GetTrade()->GetNumDifferentTradingPartners()) + (m_pPlayer->GetYieldPerTurnFromResources(YIELD_GOLD, true, true)));
	}
	else
	{
#endif
#if defined(MOD_BALANCE_CORE)
	return ((m_pPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner(YIELD_GOLD) * m_pPlayer->GetTrade()->GetNumDifferentTradingPartners()) + (m_pPlayer->GetYieldPerTurnFromResources(YIELD_GOLD, true, true)));
#else
	return m_pPlayer->GetPlayerTraits()->GetYieldChangePerTradePartner(YIELD_GOLD) * m_pPlayer->GetTrade()->GetNumDifferentTradingPartners();
#endif
#if defined(MOD_BALANCE_CORE)
	}
#endif
}

/// Gold Per Turn from Religion
int CvTreasury::GetGoldPerTurnFromReligion() const
{
	return m_pPlayer->GetYieldPerTurnFromReligion(YIELD_GOLD);
}

/// Gross income for turn times 100
int CvTreasury::CalculateGrossGold()
{
	return CalculateGrossGoldTimes100() / 100;
}

/// Gross income for turn
int CvTreasury::CalculateGrossGoldTimes100()
{
	int iNetGold = 0;

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

#if defined(MOD_BALANCE_CORE)
	iNetGold += GetInternalTradeRouteGoldBonus() * 100;
#endif

	// We're a master of someone, we get gold from taxes
	iNetGold += GetMyShareOfVassalTaxes();

#if defined(MOD_BALANCE_CORE)
	if(MOD_BALANCE_CORE_MINOR_CIV_GIFT)
	{
		iNetGold += m_pPlayer->GetGoldPerTurnFromMinorCivs() * 100;
	}
	if (MOD_BALANCE_CORE_JFD)
	{
		iNetGold += m_pPlayer->GetYieldPerTurnFromMinors(YIELD_GOLD) * 100;
	}
#endif

	// Annexed City-States (Rome UA)
	iNetGold += m_pPlayer->GetGoldPerTurnFromAnnexedMinors() * 100;

	//Espionage Events
	iNetGold += m_pPlayer->GetYieldPerTurnFromEspionageEvents(YIELD_GOLD, true) * 100;

	return iNetGold;
}
/// Gross income across entire game
int CvTreasury::GetLifetimeGrossGold() const
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
	if (m_pPlayer->IsAnarchy())
		return 0;

	return CalculateGrossGoldTimes100() - CalculateTotalCosts() * 100;
}

/// Compute unit maintenance cost for the turn
int CvTreasury::CalculateUnitCost()
{
	// If player has 0 Cities then no Unit cost
	if (m_pPlayer->getNumCities() == 0)
		return 0;

	const CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();
	int iCostPerUnit = m_pPlayer->getGoldPerUnitTimes100(); // 0.5 GPT per unit

	int iFreeUnits = playerHandicap.getMaintenanceFreeUnits() + m_pPlayer->GetNumMaintenanceFreeUnits() + m_pPlayer->getBaseFreeUnits();
	iFreeUnits += m_pPlayer->isHuman() ? 0 : GC.getGame().getHandicapInfo().getAIMaintenanceFreeUnits();

	int iPaidUnits = max(0, m_pPlayer->getNumUnits() - iFreeUnits);

	int iBaseUnitCost = iPaidUnits * iCostPerUnit;

	// Discount on land unit maintenance?
	int iLandUnitMod = m_pPlayer->GetPlayerTraits()->GetLandUnitMaintenanceModifier();
	if (iLandUnitMod != 0)
	{
		int iPaidLandUnits = max(0, m_pPlayer->GetNumUnitsWithDomain(DOMAIN_LAND, /*bMilitaryOnly*/ true) - m_pPlayer->GetNumMaintenanceFreeUnits(DOMAIN_LAND, true));
		int iCost = iPaidLandUnits * iCostPerUnit;
		int iModifiedCost = iCost * (100 + iLandUnitMod) / 100;

		// Reduce cost based on difference
		iBaseUnitCost += iModifiedCost - iCost;
	}

	// Discount on naval unit maintenance?
	int iNavalUnitMod = m_pPlayer->GetPlayerTraits()->GetNavalUnitMaintenanceModifier();
	if (iNavalUnitMod != 0)
	{
		int iPaidNavalUnits = max(0, m_pPlayer->GetNumUnitsWithDomain(DOMAIN_SEA, /*bMilitaryOnly*/ true) - m_pPlayer->GetNumMaintenanceFreeUnits(DOMAIN_SEA, true));
		int iCost = iPaidNavalUnits * iCostPerUnit;
		int iModifiedCost = iCost * (100 + iNavalUnitMod) / 100;

		// Reduce cost based on difference
		iBaseUnitCost += iModifiedCost - iCost;
	}

	// Discounts for units of certain UnitCombat classes
	// FIXME: No consideration for free units?
	for (int iI = 0; iI < GC.getNumUnitCombatClassInfos(); iI++)
	{
		const UnitCombatTypes eUnitCombatClass = static_cast<UnitCombatTypes>(iI);
		CvBaseInfo* pkUnitCombatClassInfo = GC.getUnitCombatClassInfo(eUnitCombatClass);
		if (pkUnitCombatClassInfo)
		{
			int iModifier = m_pPlayer->GetPlayerTraits()->GetMaintenanceModifierUnitCombat(eUnitCombatClass);
			if (iModifier != 0)
			{
				int iUnitsOfClass = m_pPlayer->GetNumUnitsWithUnitCombat(eUnitCombatClass);
				int iCost = iUnitsOfClass * iCostPerUnit; 
				int iModifiedCost = iCost * (100 + iModifier) / 100; 
				
				// Reduce cost based on difference
				iBaseUnitCost += iModifiedCost - iCost;
			}
		}
	}

	// Game progress factor ranges from 0 to 100 based on how far into the game we are
	int iGameProgressFactor = (GC.getGame().getElapsedGameTurns() * 100) / GC.getGame().getDefaultEstimateEndTurn();
	// in VP: game progress is calculated based on tech progress, not on elapsed game turns
	if (MOD_BALANCE_VP)
		iGameProgressFactor = (GET_TEAM(m_pPlayer->getTeam()).GetTeamTechs()->GetNumTechsKnown() * 100) / GC.getNumTechInfos();

	// Multiplicative increase - helps scale costs as game goes on - the HIGHER this number the more is paid
	double fMultiplier = 0.0f;
	fMultiplier += 1.0f + ((float)iGameProgressFactor * /*8 in CP, 7 in VP*/ GD_INT_GET(UNIT_MAINTENANCE_GAME_MULTIPLIER) / 100);

	// Exponential increase - this one really punishes those with a HUGE military - the LOWER this number the more is paid
	double fExponent = 0.0f;
	fExponent += 1.0f + ((float)iGameProgressFactor / /*7 in CP, 6 in VP*/ GD_INT_GET(UNIT_MAINTENANCE_GAME_EXPONENT_DIVISOR) / 100);

	double dTempCost = 0.00f;
	dTempCost += fMultiplier * iBaseUnitCost / 100;

	double dFinalCost = 0.00f;
	dFinalCost += pow(dTempCost, fExponent);

	// Inflation mod (difficulty)
	if (m_pPlayer->isMajorCiv())
	{
		double dDifference = 0.00f;
		dDifference += dFinalCost - dTempCost;

		dDifference *= playerHandicap.getInflationPercent();
		dDifference /= 100;
		if (!m_pPlayer->isHuman())
		{
			dDifference *= GC.getGame().getHandicapInfo().getAIInflationPercent();
			dDifference /= 100;
		}

		dFinalCost = 0.00f;
		dFinalCost += dTempCost + dDifference;
	}

	dFinalCost += m_pPlayer->getExtraUnitCost(); // Add extra flat maintenance cost from units

	// A mod at the player level? (Policies, etc.)
	if (m_pPlayer->GetUnitGoldMaintenanceMod() != 0)
	{
		dFinalCost *= 100 + m_pPlayer->GetUnitGoldMaintenanceMod();
		dFinalCost /= 100;
	}

	// Difficulty bonus for unit maintenance costs
	if (m_pPlayer->isMajorCiv())
	{
		dFinalCost *= playerHandicap.getUnitCostPercent();
		dFinalCost /= 100;
		if (!m_pPlayer->isHuman())
		{
			dFinalCost *= GC.getGame().getHandicapInfo().getAIUnitCostPercent();
			dFinalCost /= 100;
		}
	}

	// Vassal bonus to unit maintenance costs
	if (m_pPlayer->IsVassalOfSomeone()) 
	{
		dFinalCost *= 100 - /*10*/ GD_INT_GET(VASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT);
		dFinalCost /= 100;
	}

	return std::max(0, int(dFinalCost));
}

/// Calculate total maintenance costs
int CvTreasury::CalculateTotalCosts()
{
	m_iExpensePerTurnUnitMaintenance = CalculateUnitCost();

	int iTotalCosts = m_iExpensePerTurnUnitMaintenance;
	iTotalCosts += GetBuildingGoldMaintenance();
	iTotalCosts += GetImprovementGoldMaintenance();
	iTotalCosts += GetVassalGoldMaintenance();
	iTotalCosts += GetExpensePerTurnFromVassalTaxes();
	iTotalCosts += MOD_BALANCE_CORE_JFD ? GetContractGoldMaintenance() : 0;
	iTotalCosts += m_pPlayer->GetYieldPerTurnFromEspionageEvents(YIELD_GOLD, false);

	return iTotalCosts;
}

/// What are our gold maintenance costs because of Buildings?
int CvTreasury::GetBuildingGoldMaintenance() const
{
	int iMaintenance = GetBaseBuildingGoldMaintenance();

	if (MOD_BALANCE_CORE_BUILDING_RESOURCE_MAINTENANCE)
	{
		int iLoop = 0;
		for (CvCity* pLoopCity = m_pPlayer->firstCity(&iLoop); pLoopCity != NULL; pLoopCity = m_pPlayer->nextCity(&iLoop))
		{
			if (pLoopCity->GetExtraBuildingMaintenance() > 0)
			{
				iMaintenance *= 100 + pLoopCity->GetExtraBuildingMaintenance();
				iMaintenance /= 100;
			}
		}
	}

	// Player modifier
	iMaintenance *= 100 + m_pPlayer->GetBuildingGoldMaintenanceMod();
	iMaintenance /= 100;

	// Modifier for difficulty level
	if (m_pPlayer->isMajorCiv())
	{
		const CvHandicapInfo& playerHandicap = m_pPlayer->getHandicapInfo();
		iMaintenance *= playerHandicap.getBuildingCostPercent();
		iMaintenance /= 100;
		if (!m_pPlayer->isHuman())
		{
			iMaintenance *= GC.getGame().getHandicapInfo().getAIBuildingCostPercent();
			iMaintenance /= 100;
		}
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
	m_iBaseBuildingGoldMaintenance = max(iValue, 0);
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
	iMaintenance *= 100 + m_pPlayer->GetImprovementGoldMaintenanceMod();
	iMaintenance /= 100;

	// Handicap
	if (m_pPlayer->isMajorCiv())
	{
		iMaintenance *= m_pPlayer->getHandicapInfo().getImprovementCostPercent();
		iMaintenance /= 100;
		if (!m_pPlayer->isHuman())
		{
			iMaintenance *= GC.getGame().getHandicapInfo().getAIImprovementCostPercent();
			iMaintenance /= 100;
		}
	}

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
	if (GetBaseImprovementGoldMaintenance() != iValue)
	{
		m_iBaseImprovementGoldMaintenance = max(iValue, 0);

		if (m_pPlayer->GetID() == GC.getGame().getActivePlayer())
			GC.GetEngineUserInterface()->setDirty(GameData_DIRTY_BIT, true);
	}
}

/// What are our gold maintenance costs because of Improvements?
void CvTreasury::ChangeBaseImprovementGoldMaintenance(int iChange)
{
	SetBaseImprovementGoldMaintenance(GetBaseImprovementGoldMaintenance() + iChange);
}

/// Average change in gold balance over N turns
int CvTreasury::AverageIncome100(int iTurns)
{
	CvAssertMsg(iTurns > 0, "Invalid number of turns parameter");

	if(m_GoldChangeForTurnTimes100.size() > 0)
	{
		int iSamples = 0;
		int iIndex = m_GoldChangeForTurnTimes100.size() - 1;
		int iTotal = 0;

		while(iSamples < iTurns && iIndex > -1)
		{
			iTotal += m_GoldChangeForTurnTimes100[iIndex];
			iSamples++;
			iIndex--;
		}

		return (iTotal / iSamples);
	}

	return 0;
}

void CvTreasury::LogExpenditure(const CvString& strExpenditure, int iAmount, int iColumn)
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


	FILogFile* pLog = NULL;
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

template<typename Treasury, typename Visitor>
void CvTreasury::Serialize(Treasury& treasury, Visitor& visitor)
{
	visitor(treasury.m_iGold);
	visitor(treasury.m_iGoldPerTurnFromDiplomacy);
	visitor(treasury.m_iExpensePerTurnUnitMaintenance);
	visitor(treasury.m_iCityConnectionGoldTimes100);
	visitor(treasury.m_iCityConnectionTradeRouteGoldModifier);
	visitor(treasury.m_iCityConnectionTradeRouteGoldChange);
	visitor(treasury.m_iBaseBuildingGoldMaintenance);
	visitor(treasury.m_iBaseImprovementGoldMaintenance);
	visitor(treasury.m_GoldBalanceForTurnTimes100);
	visitor(treasury.m_GoldChangeForTurnTimes100);
	visitor(treasury.m_iLifetimeGrossGoldIncome);
	visitor(treasury.m_iInternalTradeGoldBonus);
	visitor(treasury.m_iExpensePerTurnFromVassalTax);
}

/// Serialization read
void CvTreasury::Read(FDataStream& kStream)
{
	CvStreamLoadVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

/// Serialization write
void CvTreasury::Write(FDataStream& kStream) const
{
	CvStreamSaveVisitor serialVisitor(kStream);
	Serialize(*this, serialVisitor);
}

FDataStream& operator>>(FDataStream& stream, CvTreasury& treasury)
{
	treasury.Read(stream);
	return stream;
}
FDataStream& operator<<(FDataStream& stream, const CvTreasury& treasury)
{
	treasury.Write(stream);
	return stream;
}

void TreasuryHelpers::AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, const CvString& strValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	strLog += strValue;
	strLog += ",";
}

void TreasuryHelpers::AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, int iValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%d,", iValue);
	strLog += str;
}

void TreasuryHelpers::AppendToLog(CvString& strHeader, CvString& strLog, const CvString& strHeaderValue, float fValue)
{
	strHeader += strHeaderValue;
	strHeader += ",";
	CvString str;
	str.Format("%.2f,", fValue);
	strLog += str;
}
#if defined(MOD_BALANCE_CORE)
int CvTreasury::GetContractGoldMaintenance()
{
	int iMaintenance = m_pPlayer->GetContracts()->GetContractGoldMaintenance();
	return iMaintenance;
}
#endif

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
			int iLoop = 0, iCityPop = 0;
			// This player is our vassal
			if(GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).IsVassal(m_pPlayer->getTeam()))
			{
				// Loop through our vassal's cities
				for(CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
				{
					iCityPop = pLoopCity->getPopulation();
					iRtnValue += std::max(0, (int)(pow((double)iCityPop, (double) /*0.8f*/ GD_FLOAT_GET(VASSALAGE_VASSAL_CITY_POP_EXPONENT))));
				}

				iRtnValue += std::max(0, (GET_PLAYER((PlayerTypes)iI).GetTreasury()->GetExpensePerTurnUnitMaintenance() * /*10*/ GD_INT_GET(VASSALAGE_VASSAL_UNIT_MAINT_COST_PERCENT) / 100));
			}
		}
	}

	// Modifier for vassal maintenance?
	iRtnValue *= (100 + m_pPlayer->GetVassalGoldMaintenanceMod());
	iRtnValue /= 100;

	return iRtnValue;
}

// Calculate how much we owe this turn due to taxes
void CvTreasury::CalculateExpensePerTurnFromVassalTaxes()
{
	TeamTypes eMaster = GET_TEAM(m_pPlayer->getTeam()).GetMaster();
	if(eMaster == NO_TEAM) {
		if(GetExpensePerTurnFromVassalTaxes() != 0)
			SetExpensePerTurnFromVassalTaxesTimes100(0);
		return;
	}
	int iNet = CalculateGrossGoldTimes100();
	int iTax = iNet * GET_TEAM(eMaster).GetVassalTax(m_pPlayer->GetID()) / 100;

	SetExpensePerTurnFromVassalTaxesTimes100(iTax);
}

// Set how much we owe this turn due to taxes
void CvTreasury::SetExpensePerTurnFromVassalTaxesTimes100(int iValue)
{
	m_iExpensePerTurnFromVassalTax = iValue;
}

// Get how much we owe this turn due to taxes
int CvTreasury::GetExpensePerTurnFromVassalTaxesTimes100() const
{
	return m_iExpensePerTurnFromVassalTax;
}

// Get how much we owe this turn due to taxes
int CvTreasury::GetExpensePerTurnFromVassalTaxes() const
{
	return GetExpensePerTurnFromVassalTaxesTimes100() / 100;
}

// What percent of vassal taxes am I owed?
int CvTreasury::GetMyShareOfVassalTaxes() const
{
	int iNumTeamMembers = GET_TEAM(m_pPlayer->getTeam()).getAliveCount();
	if(iNumTeamMembers == 0)
		return 0;

	int iTotalTaxes = 0;
	PlayerTypes eLoopPlayer;
	for(int iPlayerLoop = 0; iPlayerLoop < MAX_MAJOR_CIVS; iPlayerLoop++)
	{
		eLoopPlayer = (PlayerTypes) iPlayerLoop;
		if(GET_TEAM(GET_PLAYER(eLoopPlayer).getTeam()).GetMaster() == m_pPlayer->getTeam())
		{
			iTotalTaxes += GET_PLAYER(eLoopPlayer).GetTreasury()->GetExpensePerTurnFromVassalTaxesTimes100();
		}
	}

	// What is my share of these taxes?
	return (iTotalTaxes / iNumTeamMembers);
}

// How much is ePlayer contributing to my vassal tax revenue (note: this doesn't actually set anything, for pure UI purposes)
int CvTreasury::GetVassalTaxContributionTimes100(PlayerTypes ePlayer) const
{
	int iNumTeamMembers = GET_TEAM(m_pPlayer->getTeam()).getAliveCount();
	if(iNumTeamMembers == 0)
		return 0;

	int iAmount = 0;
	
	if(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).GetMaster() == m_pPlayer->getTeam())
	{
		iAmount += GET_PLAYER(ePlayer).GetTreasury()->GetExpensePerTurnFromVassalTaxesTimes100();
	}

	return iAmount  / iNumTeamMembers;
}

int CvTreasury::GetVassalTaxContribution(PlayerTypes ePlayer) const
{
	return GetVassalTaxContributionTimes100(ePlayer) / 100;
}