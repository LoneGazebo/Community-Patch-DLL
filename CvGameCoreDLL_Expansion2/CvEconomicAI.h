/*	-------------------------------------------------------------------------------------------------------
	© 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_ECONOMIC_AI_H
#define CIV5_ECONOMIC_AI_H

enum PurchaseType
{
    NO_PURCHASE_TYPE = -1,
    PURCHASE_TYPE_TILE,
    PURCHASE_TYPE_UNIT_UPGRADE,
    PURCHASE_TYPE_MINOR_CIV_GIFT,
    PURCHASE_TYPE_MAJOR_CIV_TRADE,
    PURCHASE_TYPE_UNIT,
    PURCHASE_TYPE_BUILDING,
    NUM_PURCHASE_TYPES,
};

enum ReconState
{
    NO_RECON_STATE = -1,
    RECON_STATE_ENOUGH,
    RECON_STATE_NEUTRAL,
    RECON_STATE_NEEDED,
};

class CvPurchaseRequest
{
public:
	CvPurchaseRequest()
	{
		m_eType = NO_PURCHASE_TYPE;
		m_iAmount = 0;
		m_iPriority = 0;
	}

	bool operator<(const CvPurchaseRequest& rhs) const
	{
		return (m_iPriority > rhs.m_iPriority);
	}

	PurchaseType m_eType;
	int m_iAmount;
	int m_iPriority;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvEconomicAIStrategyXMLEntry
//!  \brief		A single entry in the AI strategy XML file
//
//!  Key Attributes:
//!  - Populated from XML\AI\CIV5EconomicAIStrategies.xml
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEconomicAIStrategyXMLEntry: public CvBaseInfo
{
public:
	CvEconomicAIStrategyXMLEntry();
	virtual ~CvEconomicAIStrategyXMLEntry();

	virtual bool CacheResults(Database::Results& kResults, CvDatabaseUtility& kUtility);

	// Accessor Functions
	int GetPlayerFlavorValue(int i) const;
	int GetCityFlavorValue(int i) const;

	int GetWeightThreshold() const;
	int GetPersonalityFlavorThresholdMod(int i) const;

	bool IsNoMinorCivs() const;
	bool IsDontUpdateCityFlavors() const;

	int GetTechPrereq() const;
	int GetTechObsolete() const;
	int GetMinimumNumTurnsExecuted() const;
	int GetCheckTriggerTurnCount() const;
	int GetFirstTurnExecuted() const;

	AdvisorTypes GetAdvisor() const;
	const char* GetAdvisorCounselText() const;
	int GetAdvisorCounselImportance() const;

private:
	int* m_piPlayerFlavorValue;
	int* m_piCityFlavorValue;

	int m_iWeightThreshold;
	int* m_piPersonalityFlavorThresholdMod;

	bool m_bNoMinorCivs;

	int m_iTechPrereq;
	int m_iTechObsolete;
	int m_iMinimumNumTurnsExecuted;
	int m_iCheckTriggerTurnCount;
	int m_iFirstTurnExecuted;

	AdvisorTypes m_eAdvisor;
	CvString m_strAdvisorCounselText;
	int m_iAdvisorCounselImportance;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvEconomicAIStrategyXMLEntries
//!  \brief		Game-wide information about possible AI strategies
//
//! Key Attributes:
//! - Populated from XML\AI\Civ5EconomicAIStrategyInfos.xml
//! - Contains an array of CvEconomicAIStrategyXMLEntry from the above XML file
//! - One instance for the entire game
//! - Accessed heavily by CvEconomicAIStrategy class (which stores the AI strategy state for 1 player)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEconomicAIStrategyXMLEntries
{
public:
	CvEconomicAIStrategyXMLEntries(void);
	~CvEconomicAIStrategyXMLEntries(void);

	// Accessor functions
	std::vector<CvEconomicAIStrategyXMLEntry*>& GetEconomicAIStrategyEntries();
	int GetNumEconomicAIStrategies();
	CvEconomicAIStrategyXMLEntry* GetEntry(int index);

	void DeleteArray();

private:
	std::vector<CvEconomicAIStrategyXMLEntry*> m_paAIStrategyEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvEconomicAI
//!  \brief		Information about the AI strategies of a single player
//
//!  Key Attributes:
//!  - Plan is it will be contained in CvPlayerState object within CvPlayer class
//!  - One instance for each civ (player or AI)
//!  - Accessed by any class that needs to check a civ's AI strategy state
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvEconomicAI
{
public:
	CvEconomicAI(void);
	~CvEconomicAI(void);
	void Init(CvEconomicAIStrategyXMLEntries* pAIStrategies, CvPlayer* pPlayer);
	void Uninit();
	void Reset();
	void Read(FDataStream& kStream);
	void Write(FDataStream& kStream);

	CvPlayer* GetPlayer();
	CvEconomicAIStrategyXMLEntries* GetEconomicAIStrategies();

	bool IsUsingStrategy(EconomicAIStrategyTypes eStrategy);
	void SetUsingStrategy(EconomicAIStrategyTypes eStrategy, bool bValue);
	int GetTurnStrategyAdopted(EconomicAIStrategyTypes eStrategy);
	void SetTurnStrategyAdopted(EconomicAIStrategyTypes eStrategy, int iValue);

	void DoTurn();

	CvCity* GetBestGreatWorkCity(CvPlot *pStartPlot, GreatWorkType eGreatWork) const;

#if defined(MOD_CORE_ALTERNATIVE_EXPLORE_SCORE)
	void SetExplorersNeeded(int iValue);
	int GetExplorersNeeded() const;

	void SetNavalExplorersNeeded(int iValue);
	int GetNavalExplorersNeeded() const;

	const std::vector<SPlotWithScore>& GetExplorationPlots(DomainTypes domain);
#else
	FFastVector<int>& GetExplorationPlots();
	FFastVector<int>& GetExplorationPlotRatings();
	FFastVector<int>& GetGoodyHutPlots();
	CvPlot* GetUnitTargetGoodyPlot(CvUnit* pUnit, CvPlot** ppkStepPlot = NULL);
	void ClearUnitTargetGoodyStepPlot(CvUnit* pUnit);

	static int ScoreExplorePlot(CvPlot* pPlot, TeamTypes eTeam, int iRange, DomainTypes eDomainType);
#endif

	void StartSaveForPurchase(PurchaseType ePurchase, int iAmount, int iPriority);
	bool IsSavingForThisPurchase(PurchaseType ePurchase);
	void CancelSaveForPurchase(PurchaseType ePurchase);
	bool CanWithdrawMoneyForPurchase(PurchaseType ePurchase, int iAmount, int iPriority = -1);
	int AmountAvailableForPurchase(PurchaseType ePurchase);
#if defined(MOD_BALANCE_CORE)
	int GetPurchaseSaveAmout(PurchaseType ePurchase);
#endif

	double GetWorkersToCitiesRatio();
	double GetImprovedToImprovablePlotsRatio();

	void LogMonitor();
	void LogCityMonitor();
	void LogPossibleHurries(CvWeightedVector<CvCityBuildable, (SAFE_ESTIMATE_NUM_BUILDINGS + SAFE_ESTIMATE_NUM_UNITS), true> m_Buildables);

	ReconState GetReconState() const
	{
		return m_eReconState;
	};
	ReconState GetNavalReconState() const
	{
		return m_eNavalReconState;
	};
	void IncrementExplorersDisbanded()
	{
		m_iExplorersDisbanded++;
	};
	int GetExplorersDisbanded()
	{
		return m_iExplorersDisbanded;
	};
	int GetLastTurnWorkerDisbanded() const
	{
		return m_iLastTurnWorkerDisbanded;
	};
	int GetVisibleAntiquitySites() const
	{
		return m_iVisibleAntiquitySites;
	};
#if defined(MOD_BALANCE_CORE)
	int GetVisibleAntiquitySitesOwnTerritory() const
	{
		return m_iVisibleAntiquitySitesOwn;
	};
	int GetVisibleHiddenAntiquitySitesOwnTerritory() const
	{
		return m_iVisibleHiddenAntiquitySitesOwn;
	};
	int GetVisibleAntiquitySitesUnownedTerritory() const
	{
		return m_iVisibleAntiquitySitesNeutral;
	};
#endif
	void UpdateExplorePlotsFromScratch();
	void UpdateExplorePlotsLocally(CvPlot* pRefPlot);

	void LogEconomyMessage(const CvString& strMsg);

private:

	CvString GetLogFileName(CvString& playerName) const;

	// DoTurn() sub-functions
	void DoHurry();
	void DoPlotPurchases();
	void DoReconState();
	void DoAntiquitySites();
	void DisbandExtraWorkers();
#if defined(MOD_BALANCE_CORE)
	void DisbandExtraWorkboats();
	void DisbandUselessSettlers();
	void DisbandMiscUnits();
	CvUnit* FindSettlerToScrap(bool bMayBeInOperation);
	CvUnit* FindSeaWorkerToScrap();
#endif
	void DisbandExtraArchaeologists();
#if defined(MOD_AI_SMART_DISBAND)
	void DisbandLongObsoleteUnits();
#endif

	// Low-level utility functions
	void AssignExplorersToHuts();
	void AssignHutsToExplorers();
	CvUnit* FindWorkerToScrap();
	CvUnit* FindArchaeologistToScrap();

	// Logging functions
	void LogStrategy(EconomicAIStrategyTypes eStrategy, bool bValue);
	void LogScrapUnit(CvUnit* pUnit, int iNumWorkers, int iNumCities, int iNumImprovedPlots, int iNumValidPlots);


	CvPlayer* m_pPlayer;
	CvEconomicAIStrategyXMLEntries* m_pAIStrategies;
	bool* m_pabUsingStrategy;
	int* m_paiTurnStrategyAdopted;
	int* m_aiTempFlavors;
	ReconState m_eReconState;
	ReconState m_eNavalReconState;
	int m_iExplorersDisbanded;
	int m_iLastTurnWorkerDisbanded;
	int m_iVisibleAntiquitySites;
#if defined(MOD_BALANCE_CORE)
	int m_iExplorersNeeded;
	int m_iNavalExplorersNeeded;
	int m_iVisibleAntiquitySitesOwn;
	int m_iVisibleHiddenAntiquitySitesOwn;
	int m_iVisibleAntiquitySitesNeutral;
#endif

	// used for the log monitor
	FFastVector<uint> m_auiYields;

	// for the exploration plots
	struct GoodyHutUnitAssignment
	{
		int m_iUnitID;				// The unit that is assigned.
		int m_iStepPlotID;			// The plot ID of the next step toward the destination.  -1 = no defined.

		GoodyHutUnitAssignment(int unitID, int plotID) : m_iUnitID(unitID), m_iStepPlotID(plotID) {}
		void Clear() { m_iUnitID = -1; m_iStepPlotID = -1; }
	};

#if defined(MOD_CORE_ALTERNATIVE_EXPLORE_SCORE)
	std::vector<SPlotWithScore> m_vPlotsToExploreLand;
	std::vector<SPlotWithScore> m_vPlotsToExploreSea;
#else
	FFastVector<int> m_aiExplorationPlots;
	FFastVector<int> m_aiExplorationPlotRatings;
	FFastVector<int> m_aiGoodyHutPlots;
	FFastVector<GoodyHutUnitAssignment> m_aiGoodyHutUnitAssignments;
#endif

	FStaticVector<CvPurchaseRequest, NUM_PURCHASE_TYPES, true, c_eCiv5GameplayDLL, 0> m_RequestedSavings;
	FStaticVector<CvPurchaseRequest, NUM_PURCHASE_TYPES, true, c_eCiv5GameplayDLL, 0> m_TempRequestedSavings;
	FFastVector<CvUnit*> m_apExplorers;
};

FDataStream& operator<<(FDataStream&, const CvPurchaseRequest&);
FDataStream& operator>>(FDataStream&, CvPurchaseRequest&);

namespace EconomicAIHelpers
{
int ScoreExplorePlot2(CvPlot* pPlot, CvPlayer* pPlayer, DomainTypes eDomainType, bool bEmbarked);
int GetWeightThresholdModifier(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);

// Functions that check triggers to see if a strategy should be adopted/continued
#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
bool IsTestStrategy_NeedRecon(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_NeedReconSea(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_EarlyExpansion(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_EnoughExpansion(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
#else
bool IsTestStrategy_NeedRecon(CvPlayer* pPlayer);
bool IsTestStrategy_NeedReconSea(CvPlayer* pPlayer);
bool IsTestStrategy_EarlyExpansion(CvPlayer* pPlayer);
#if defined(MOD_BALANCE_CORE)
bool IsTestStrategy_EnoughExpansion(CvPlayer* pPlayer);
#else
bool IsTestStrategy_EnoughExpansion(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
#endif
#endif
bool IsTestStrategy_EnoughRecon(CvPlayer* pPlayer);
bool IsTestStrategy_ReallyNeedReconSea(CvPlayer* pPlayer);
bool IsTestStrategy_EnoughReconSea(CvPlayer* pPlayer);
bool IsTestStrategy_NeedHappiness(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_NeedHappinessCritical(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_CitiesNeedNavalGrowth(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_CitiesNeedNavalTileImprovement(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_FoundCity(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_TradeWithCityState(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
#if defined(MOD_DIPLOMACY_CITYSTATES)
bool IsTestStrategy_InfluenceCityState(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
#endif
bool IsTestStrategy_ConcertTour(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_NeedImprovement(CvPlayer* pPlayer, YieldTypes eYield);
bool IsTestStrategy_OneOrFewerCoastalCities(CvPlayer* pPlayer);
bool IsTestStrategy_LosingMoney(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_HaltGrowthBuildings(/*CvPlayer* pPlayer*/);
bool IsTestStrategy_TooManyUnits(CvPlayer* pPlayer);
bool IsTestStrategy_IslandStart(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
bool IsTestStrategy_ExpandToOtherContinents(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
#else
bool IsTestStrategy_ExpandToOtherContinents(CvPlayer* pPlayer);
#endif
bool IsTestStrategy_MostlyOnTheCoast(CvPlayer* pPlayer);
bool IsTestStrategy_NavalMap(CvPlayer* pPlayer);
bool IsTestStrategy_OffshoreExpansionMap(CvPlayer* pPlayer);
bool IsTestStrategy_DevelopingReligion(CvPlayer* pPlayer);
bool IsTestStrategy_TechLeader(CvPlayer* pPlayer);
bool IsTestStrategy_NeedArchaeologists(CvPlayer* pPlayer);
bool IsTestStrategy_EnoughArchaeologists(CvPlayer* pPlayer);
#if defined(MOD_DIPLOMACY_CITYSTATES)
int IsTestStrategy_ScoreDiplomats(CvPlayer* pPlayer);
bool IsTestStrategy_NeedDiplomats(CvPlayer* pPlayer);
bool IsTestStrategy_NeedDiplomatsCritical(CvPlayer* pPlayer);
#endif
bool IsTestStrategy_NeedMuseums(CvPlayer* pPlayer);
bool IsTestStrategy_NeedGuilds(CvPlayer* pPlayer);
bool IsTestStrategy_StartedPiety(CvPlayer* pPlayer);

// these make a few players' behaviors a bit more extreme
bool IsTestStrategy_ExpandLikeCrazy(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);
bool IsTestStrategy_GrowLikeCrazy(EconomicAIStrategyTypes eStrategy, CvPlayer* pPlayer);

bool IsTestStrategy_GS_Culture(CvPlayer* pPlayer);
bool IsTestStrategy_GS_Conquest(CvPlayer* pPlayer);
bool IsTestStrategy_GS_Diplomacy(CvPlayer* pPlayer);
bool IsTestStrategy_GS_Spaceship(CvPlayer* pPlayer);

bool IsTestStrategy_GS_SpaceshipHomestretch(CvPlayer* pPlayer);
#if defined(MOD_BUGFIX_MINOR_CIV_STRATEGIES)
bool CannotMinorCiv(CvPlayer* pPlayer, EconomicAIStrategyTypes strategy);
#endif
}

#endif //CIV5_ECONOMIC_AI_H