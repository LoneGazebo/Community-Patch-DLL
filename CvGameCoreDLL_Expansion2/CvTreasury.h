/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CV_TREASURY_H
#define CV_TREASURY_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS: CvTreasury
//!  \brief Tracks income and balance of gold for a single player
//
//!  Key Attributes:
//!  - This object is created inside the CvPlayer object and accessed through CvPlayer
//!  - Consolidates a number of individual gold-related functions that were in CvPlayer
//!  - Provides new historical tracking/trending of income and gold balances
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTreasury
{
public:
	CvTreasury();
	virtual ~CvTreasury();
	void Init(CvPlayer* pPlayer);
	void Uninit();

	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	void DoGold();

	// Methods to add or subtract gold
	int GetGold() const;
	void SetGold(int iNewValue);
	void ChangeGold(int iChange);
	int GetGoldTimes100() const;
	void SetGoldTimes100(int iNewValue);
	void ChangeGoldTimes100(int iChange);

	// Gold from Cities
	int GetGoldFromCities() const;
	int GetGoldFromCitiesTimes100(bool bExcludeTradeRoutes = false) const;

	// Diplomacy
	int GetGoldPerTurnFromDiplomacy() const;
	void SetGoldPerTurnFromDiplomacy(int iValue);
	void ChangeGoldPerTurnFromDiplomacy(int iChange);

	// City connections
	int GetCityConnectionRouteGoldTimes100(CvCity* pNonCapitalCity) const;
	int GetCityConnectionGold() const;
	int GetCityConnectionGoldTimes100() const;
	void DoUpdateCityConnectionGold();

	// City Connection Route Modifiers
#if defined(MOD_BALANCE_CORE_POLICIES)
	int GetInternalTradeRouteGoldBonus() const;
	void DoInternalTradeRouteGoldBonus();
#endif
	int GetCityConnectionTradeRouteGoldModifier() const;
	void ChangeCityConnectionTradeRouteGoldModifier(int iChange);
	int GetCityConnectionTradeRouteGoldChange() const;
	void ChangeCityConnectionTradeRouteGoldChange(int iChange);

	// Gold from international trade routes
	int GetGoldPerTurnFromTradeRoutes() const;
	int GetGoldPerTurnFromTradeRoutesTimes100() const;
	int GetGoldPerTurnFromTraits() const;

	// Religion
	int GetGoldPerTurnFromReligion() const;

	// Calculations ported over from CvPlayer
	int CalculateGrossGold();
	int CalculateGrossGoldTimes100();
	int GetLifetimeGrossGold();
	int CalculateBaseNetGold();
	int CalculateBaseNetGoldTimes100();
	int CalculateUnitCost(int& iFreeUnits, int& iPaidUnits, int& iBaseUnitCost, int& iExtraCost);
	int CalculateUnitSupply(int& iPaidUnits, int& iBaseSupplyCost);
	int CalculatePreInflatedCosts();
	int CalculateInflationRate();
	int CalculateInflatedCosts();
	int GetExpensePerTurnUnitMaintenance()
	{
		return m_iExpensePerTurnUnitMaintenance;
	}
	int GetExpensePerTurnUnitSupply()
	{
		return m_iExpensePerTurnUnitSupply;
	}

	// Other Maintenance
	int GetBuildingGoldMaintenance() const;
	int GetBaseBuildingGoldMaintenance() const;
	void SetBaseBuildingGoldMaintenance(int iValue);
	void ChangeBaseBuildingGoldMaintenance(int iChange);

	int GetImprovementGoldMaintenance() const;
	int GetBaseImprovementGoldMaintenance() const;
	void SetBaseImprovementGoldMaintenance(int iValue);
	void ChangeBaseImprovementGoldMaintenance(int iChange);

	// Methods to query financial history
	double AverageIncome(int iTurns);
	void CvTreasury::LogExpenditure(CvString strExpenditure, int iAmount, int iColumn);

#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	int GetVassalGoldMaintenance() const;

	int GetMyShareOfVassalTaxes() const;
	int GetVassalTaxContributionTimes100(PlayerTypes ePlayer) const;
	int GetVassalTaxContribution(PlayerTypes ePlayer) const;

	void CalculateExpensePerTurnFromVassalTaxes();
	
	int GetExpensePerTurnFromVassalTaxesTimes100() const;
	int GetExpensePerTurnFromVassalTaxes() const;
	void SetExpensePerTurnFromVassalTaxesTimes100(int iValue);
#endif
#if defined(MOD_BALANCE_CORE)
	int GetContractGoldMaintenance();
#endif

protected:
	CvPlayer* m_pPlayer;
	int m_iGold;
	int m_iGoldPerTurnFromDiplomacy;
	int m_iExpensePerTurnUnitMaintenance;
	int m_iExpensePerTurnUnitSupply;
	int m_iCityConnectionGoldTimes100;
#if defined(MOD_BALANCE_CORE)
	int m_iInternalTradeGoldBonus;
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	int m_iExpensePerTurnFromVassalTax;
#endif
	int m_iCityConnectionTradeRouteGoldModifier;
	int m_iCityConnectionTradeRouteGoldChange;

	int m_iBaseBuildingGoldMaintenance;
	int m_iBaseImprovementGoldMaintenance;

	int m_iLifetimeGrossGoldIncome;

	std::vector<int> m_GoldBalanceForTurnTimes100;
	std::vector<int> m_GoldChangeForTurnTimes100;
};

namespace TreasuryHelpers
{
	void AppendToLog (CvString& strHeader, CvString& strLog, CvString strHeaderValue, CvString strValue);
	void AppendToLog (CvString& strHeader, CvString& strLog, CvString strHeaderValue, int iValue);
	void AppendToLog (CvString& strHeader, CvString& strLog, CvString strHeaderValue, float fValue);
}

#endif // CV_TREASURY_H