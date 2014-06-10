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
	int GetGoldFromCitiesTimes100() const;

	// Diplomacy
	int GetGoldPerTurnFromDiplomacy() const;
	void SetGoldPerTurnFromDiplomacy(int iValue);
	void ChangeGoldPerTurnFromDiplomacy(int iChange);

	// City connections
	int GetRouteGoldTimes100(CvCity* pNonCapitalCity) const;
	int GetCityConnectionGold() const;
	int GetCityConnectionGoldTimes100() const;
	void DoUpdateCityConnectionGold();

	// Trade routes
	int GetTradeRouteGoldModifier() const;
	void ChangeTradeRouteGoldModifier(int iChange);
	int GetTradeRouteGoldChange() const;
	void ChangeTradeRouteGoldChange(int iChange);
	bool HasRouteBetweenCities(CvCity* pFirstCity, CvCity* pSecondCity, bool bBestRoute = false) const;

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

protected:
	CvPlayer* m_pPlayer;
	int m_iGold;
	int m_iGoldPerTurnFromDiplomacy;
	int m_iExpensePerTurnUnitMaintenance;
	int m_iExpensePerTurnUnitSupply;
	int m_iCityConnectionGoldTimes100;
	int m_iTradeRouteGoldModifier;
	int m_iTradeRouteGoldChange;

	int m_iBaseBuildingGoldMaintenance;
	int m_iBaseImprovementGoldMaintenance;

	int m_iLifetimeGrossGoldIncome;

	std::vector<int> m_GoldBalanceForTurnTimes100;
	std::vector<int> m_GoldChangeForTurnTimes100;
};


#endif // CV_TREASURY_H
