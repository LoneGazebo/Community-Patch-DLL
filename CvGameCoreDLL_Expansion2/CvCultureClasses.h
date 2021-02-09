/*	-------------------------------------------------------------------------------------------------------
	� 1991-2012 Take-Two Interactive Software and its subsidiaries.  Developed by Firaxis Games.  
	Sid Meier's Civilization V, Civ, Civilization, 2K Games, Firaxis Games, Take-Two Interactive Software 
	and their respective logos are all trademarks of Take-Two interactive Software, Inc.  
	All other marks and trademarks are the property of their respective owners.  
	All rights reserved. 
	------------------------------------------------------------------------------------------------------- */
#pragma once

#ifndef CIV5_CULTURE_CLASSES_H
#define CIV5_CULTURE_CLASSES_H


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS: CvGreatWork
//!  \brief All the information about a single Great Work
//
//!  Key Attributes:
//!  - Stores the work's name and type
//!  - Stores creation info (GP, year, era, player)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGreatWork
{
public:
	CvGreatWork();
	CvGreatWork(CvString szGreatPersonName, GreatWorkType eType, GreatWorkClass eClassType, int iTurn, EraTypes eEra, PlayerTypes ePlayer);

	// Public data
	CvString m_szGreatPersonName;
	GreatWorkType m_eType;
	GreatWorkClass m_eClassType;
	int m_iTurnFounded;
	EraTypes m_eEra;
	PlayerTypes m_ePlayer;
};

FDataStream& operator>>(FDataStream&, CvGreatWork&);
FDataStream& operator<<(FDataStream&, const CvGreatWork&);

typedef vector<CvGreatWork> GreatWorkList;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvGameCulture
//!  \brief		All the information about culture at the game level
//
//!  Key Attributes:
//!  - Core data in this class is a list of CvGreatWorks
//!  - This object is created inside the CvGame object and accessed through CvGame
//!  - Provides convenience functions to the other game subsystems to quickly summarize
//!    information on the Great Works (and other culture elements) in place
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameCulture
{
public:
	CvGameCulture(void);
	~CvGameCulture(void);

	void DoTurn();

	// Great Works
	int CreateGreatWork(GreatWorkType eType, GreatWorkClass eClassType, PlayerTypes ePlayer, EraTypes eEra, CvString szCreator);
	int GetNumGreatWorks() const
	{
		return m_CurrentGreatWorks.size();
	}

	GreatWorkType GetGreatWorkType(int iIndex) const;
	GreatWorkClass GetGreatWorkClass(int iIndex) const;
	CvString GetGreatWorkTooltip(int iIndex, PlayerTypes eOwner) const;
	CvString GetGreatWorkName(int iIndex) const;
	CvString GetGreatWorkArtist(int iIndex) const;
	CvString GetGreatWorkEra(int iIndex) const;
	CvString GetGreatWorkEraAbbreviation(int iIndex) const;
	CvString GetGreatWorkEraShort(int iIndex) const;
	PlayerTypes GetGreatWorkCreator (int iIndex) const;
	PlayerTypes GetGreatWorkController(int iIndex) const;
#if defined(MOD_API_EXTENSIONS)
	bool IsGreatWorkCreated(GreatWorkType eType) const;
	CvCity* GetGreatWorkCity(int iIndex) const;
#endif
	int GetGreatWorkCurrentThemingBonus (int iIndex) const;

	bool SwapGreatWorks (PlayerTypes ePlayer1, int iWork1, PlayerTypes ePlayer2, int iWork2);
	void MoveGreatWorks(PlayerTypes ePlayer, int iCity1, int iBuildingClass1, int iWorkIndex1, 
																					 int iCity2, int iBuildingClass2, int iWorkIndex2);
	GreatWorkList m_CurrentGreatWorks;

	// Culture Victory
	int GetNumCivsInfluentialForWin() const;
	bool GetReportedSomeoneInfluential() const
	{
		return m_bReportedSomeoneInfluential;
	};
	void SetReportedSomeoneInfluential(bool bValue)
	{
		m_bReportedSomeoneInfluential = bValue;
	};

private:
	bool m_bReportedSomeoneInfluential;
};

FDataStream& operator>>(FDataStream&, CvGameCulture&);
FDataStream& operator<<(FDataStream&, const CvGameCulture&);

enum InfluenceLevelTypes		
{
	NO_INFLUENCE_LEVEL = -1,
	INFLUENCE_LEVEL_UNKNOWN,
	INFLUENCE_LEVEL_EXOTIC,
	INFLUENCE_LEVEL_FAMILIAR,
	INFLUENCE_LEVEL_POPULAR,
	INFLUENCE_LEVEL_INFLUENTIAL,
	INFLUENCE_LEVEL_DOMINANT,
};

enum InfluenceLevelTrend		
{
	INFLUENCE_TREND_FALLING = -1,
	INFLUENCE_TREND_STATIC,
	INFLUENCE_TREND_RISING,
};

enum PublicOpinionTypes		
{
	NO_PUBLIC_OPINION = -1,
	PUBLIC_OPINION_CONTENT,
	PUBLIC_OPINION_DISSIDENTS,
	PUBLIC_OPINION_CIVIL_RESISTANCE,
	PUBLIC_OPINION_REVOLUTIONARY_WAVE,
};

class CvGreatWorkInMyEmpire
{
public:
	CvGreatWorkInMyEmpire();
	CvGreatWorkInMyEmpire(int iIndex, int iCityID, BuildingTypes eBuilding, int iSlot, PlayerTypes ePlayer, EraTypes eEra);

	// Public data
	int m_iGreatWorkIndex;
	int m_iCityID;
	BuildingTypes m_eBuilding;
	int m_iSlot;
	PlayerTypes m_ePlayer;
	EraTypes m_eEra;
};

class CvGreatWorkBuildingInMyEmpire
{
public:
	CvGreatWorkBuildingInMyEmpire();
	CvGreatWorkBuildingInMyEmpire(int iCityID, BuildingTypes eBuilding);

	// Public data
	int m_iCityID;
	BuildingTypes m_eBuilding;
	bool m_bThemed;
	bool m_bEndangered;
#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES) || defined(MOD_API_UNIFIED_YIELDS)
	bool m_bPuppet;
	YieldTypes m_eYieldType;
#endif
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvPlayerCulture
//!  \brief		All the information about culture at the player level
//
//!  Key Attributes:
//!  - This object is created inside the CvPlayer object and accessed through CvPlayer
//!  - Provides convenience functions to the other game subsystems to quickly summarize
//!    information on available Great Work slots at the player level
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerCulture
{
public:
	CvPlayerCulture(void);
	~CvPlayerCulture(void);

	void Init(CvPlayer* pPlayer);

	// Great Work routines
	bool HasAvailableGreatWorkSlot(GreatWorkSlotType eGreatWorkSlot);
	int GetNumAvailableGreatWorkSlots(GreatWorkSlotType eGreatWorkSlot) const;
	CvCity *GetClosestAvailableGreatWorkSlot(int iX, int iY, GreatWorkSlotType eGreatWorkSlot, BuildingClassTypes *eBuildingClass, int *iSlot) const;
	int GetNumGreatWorks() const;
	int GetNumGreatWorkSlots() const;
	int GetNumGreatWorkSlots(GreatWorkSlotType eGreatWorkSlot) const;
	bool ControlsGreatWork (int iIndex);
	bool GetGreatWorkLocation(int iGreatWorkIndex, int &iCityID, BuildingTypes &eBuilding, int &iSlot);
#if defined(MOD_BALANCE_CORE)
	void DoSwapGreatWorksHuman(bool bSwap);
#endif
#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	void DoSwapGreatWorks(YieldTypes eFocusYield);
	void MoveWorks (GreatWorkSlotType eType, vector<CvGreatWorkBuildingInMyEmpire> &buildings, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, YieldTypes eFocusYield, bool bSwap);
#else
	void DoSwapGreatWorks();
	void MoveWorks (GreatWorkSlotType eType, vector<CvGreatWorkBuildingInMyEmpire> &buildings, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2);
#endif
	bool ThemeBuilding(vector<CvGreatWorkBuildingInMyEmpire>::const_iterator it, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, bool bConsiderOtherPlayers);
	bool ThemeEqualArtArtifact(CvGreatWorkBuildingInMyEmpire kBldg, int iThemingBonusIndex, int iNumSlots, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, bool bConsiderOtherPlayers, int iThemeID = -1);
#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	bool MoveSingleWorks(vector<CvGreatWorkBuildingInMyEmpire> &buildings, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2, YieldTypes eFocusYield, bool bPuppet);
#endif
	bool FillBuilding(vector<CvGreatWorkBuildingInMyEmpire>::const_iterator it, vector<CvGreatWorkInMyEmpire> &works1, vector<CvGreatWorkInMyEmpire> &works2);
#if defined(MOD_BALANCE_CORE)
	bool MoveWorkIntoSlot (CvGreatWorkInMyEmpire kWork, int iCityID, BuildingTypes eBuilding, int iSlot);
#else
	void MoveWorkIntoSlot (CvGreatWorkInMyEmpire kWork, int iCityID, BuildingTypes eBuilding, int iSlot);
#endif
	int GetSwappableWritingIndex() const;
	int GetSwappableArtIndex() const;
	int GetSwappableArtifactIndex() const;
	int GetSwappableMusicIndex() const;
	void SetSwappableWritingIndex (int iIndex);
	void SetSwappableArtIndex (int iIndex);
	void SetSwappableArtifactIndex (int iIndex);
	void SetSwappableMusicIndex (int iIndex);

	// Archaeology 
	void AddDigCompletePlot(CvPlot *pPlot);
	void RemoveDigCompletePlot(CvPlot *pPlot);
	void ResetDigCompletePlots();
	CvPlot *GetNextDigCompletePlot() const;
	CvUnit *GetNextDigCompleteArchaeologist(CvPlot **ppPlot) const;
	bool HasDigCompleteHere(CvPlot *pPlot) const;
	int GetWrittenArtifactCulture() const;

	void DoArchaeologyChoice (ArchaeologyChoiceType eChoice);
	//    AI support routine - move elsewhere later?
	ArchaeologyChoiceType GetArchaeologyChoice(CvPlot *pPlot);
 
	// Cultural Influence
	void DoTurn();
#if defined(MOD_BALANCE_CORE)
	int GetLastUpdate() const;
	void SetLastUpdate(int iValue);

	int GetLastThemUpdate() const;
	void SetLastThemUpdate(int iValue);

	void SetBoredomCache(int iValue);
	int GetBoredomCache() const;
#endif
	int GetLastTurnLifetimeCulture() const;
	void SetLastTurnLifetimeCulture(int iValue);
	int GetLastTurnCPT() const;
	void SetLastTurnCPT(int iValue);
	int GetInfluenceOn(PlayerTypes ePlayer) const;
	void ChangeInfluenceOn(PlayerTypes ePlayer, int iValue);
#if defined(MOD_API_EXTENSIONS)
	int ChangeInfluenceOn(PlayerTypes eOtherPlayer, int iBaseInfluence, bool bApplyModifiers, bool bModifyForGameSpeed);
#endif
	int GetLastTurnInfluenceOn(PlayerTypes ePlayer) const;
	int GetLastTurnInfluenceIPT(PlayerTypes ePlayer) const;
	int GetInfluencePerTurn(PlayerTypes ePlayer) const;
	InfluenceLevelTypes GetInfluenceLevel(PlayerTypes ePlayer) const;
	InfluenceLevelTrend GetInfluenceTrend(PlayerTypes ePlayer) const;
	int GetTurnsToInfluential(PlayerTypes ePlayer);
	int GetNumCivsInfluentialOn() const;
	int GetNumCivsToBeInfluentialOn() const;
	PlayerTypes GetCivLowestInfluence(bool bCheckOpenBorders) const;
#if defined(MOD_BALANCE_CORE)
	int GetOtherPlayerCulturePerTurnIncludingInstant(PlayerTypes ePlayer);
	int GetTourismPerTurnIncludingInstant(PlayerTypes ePlayer, bool bJustInstant = false);
	int GetInfluenceTradeRouteGoldBonus(PlayerTypes ePlayer) const;
	int GetInfluenceTradeRouteGrowthBonus(PlayerTypes ePlayer) const;
#endif
	int GetInfluenceTradeRouteScienceBonus(PlayerTypes ePlayer) const;
	int GetInfluenceCityConquestReduction(PlayerTypes ePlayer) const;
	int GetInfluenceSurveillanceTime(PlayerTypes ePlayer) const;
	int GetInfluenceCityStateSpyRankBonus(PlayerTypes eCityStatePlayer) const;
	int GetInfluenceMajorCivSpyRankBonus(PlayerTypes ePlayer) const;
	CvString GetInfluenceSpyRankTooltip (CvString szName, CvString iRank, PlayerTypes ePlayer, bool bNoBasicHelp = false, int iSpyID = -1);
	int GetTourism();
	int GetTourismModifierWith(PlayerTypes ePlayer) const;
	CvString GetTourismModifierWithTooltip(PlayerTypes ePlayer) const;
	int GetTourismModifierSharedReligion() const;
	int GetTourismModifierTradeRoute() const;
	int GetTourismModifierOpenBorders() const;
	int GetFranchiseModifier(PlayerTypes ePlayer, bool bJustCheckOne = false) const;
	PublicOpinionTypes GetPublicOpinionType() const;
	PolicyBranchTypes GetPublicOpinionPreferredIdeology() const;
	CvString GetPublicOpinionTooltip() const;
	int GetPublicOpinionUnhappiness() const;
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int GetWarWeariness() const;
	void SetWarWeariness(int iValue);
#endif
#if defined(MOD_DIPLOMACY_CIV4_FEATURES)
	int GetTourismModifierVassal() const;
#endif
	CvString GetPublicOpinionUnhappinessTooltip() const;
	PlayerTypes GetPublicOpinionBiggestInfluence() const;
	int GetTurnIdeologySwitch() const;
	void SetTurnIdeologySwitch(int iTurn);
	int GetTurnIdeologyAdopted() const;
	void SetTurnIdeologyAdopted(int iTurn);
	int GetTourismBlastStrength(int iMultiplier);
	void AddTourismAllKnownCivs(int iTourism);
	void AddTourismAllKnownCivsWithModifiers(int iTourism);
	void AddTourismAllKnownCivsOtherCivWithModifiers(PlayerTypes eOtherPlayer, int iTourism);
	void DoPublicOpinion();
	int ComputeHypotheticalPublicOpinionUnhappiness(PolicyBranchTypes eBranch);
	bool WantsDiplomatDoingPropaganda(PlayerTypes ePlayer) const;
	int GetMaxPropagandaDiplomatsWanted() const;
	int GetNumCivsFollowingIdeology(PolicyBranchTypes ePolicyBranch);
	int GetNumCivsFollowingAnyIdeology();

	// Bonus stats
	int GetTotalThemingBonuses() const;

	// Public data
	vector<CvPlot *> m_aDigCompletePlots;
	int m_iLastTurnLifetimeCulture;
	int m_iLastTurnCPT;
	int m_aiCulturalInfluence[MAX_MAJOR_CIVS];
	int m_aiLastTurnCulturalInfluence[MAX_MAJOR_CIVS];
	int m_aiLastTurnCulturalIPT[MAX_MAJOR_CIVS];
	bool m_bReportedTwoCivsAway;
	bool m_bReportedOneCivAway;
	PublicOpinionTypes m_eOpinion;
	PlayerTypes m_eOpinionBiggestInfluence;
	int m_iTurnIdeologySwitch;
	int m_iTurnIdeologyAdopted;
	PolicyBranchTypes m_ePreferredIdeology;
	int m_iOpinionUnhappiness;
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int m_iRawWarWeariness; //without modifier
	int m_iLastUpdate;
	int m_iLastThemUpdate;
	int m_iBoredomCache;
	int m_iBoredomCacheTurn;
#endif
	CvString m_strOpinionTooltip;
	CvString m_strOpinionUnhappinessTooltip;

	int m_iSwappableWritingIndex;
	int m_iSwappableArtIndex;
	int m_iSwappableArtifactIndex;
	int m_iSwappableMusicIndex;

private:
#if defined(MOD_BALANCE_CORE)
	int ComputePublicOpinionUnhappiness(int iDissatisfaction);
#else
	int ComputePublicOpinionUnhappiness(int iDissatisfaction, int &iPerCityUnhappy, int &iUnhappyPerXPop);
#endif
#if defined(MOD_BALANCE_CORE_HAPPINESS)
	int ComputeWarWeariness();
#endif
	// Logging functions
	void LogCultureData();
	void LogThemedBuilding(int iCityID, BuildingTypes eBuilding, int iBonus);
	void LogSwapWorks(PlayerTypes eOtherPlayer, int iWorkDiscarded, int iWorkAcquired);
#if defined(MOD_BALANCE_CORE)
	void LogSwapMultipleWorks(PlayerTypes eOtherPlayer, int iWorkDiscarded, int iWorkAcquired);
	void LogSwapMultipleArtifacts(PlayerTypes eOtherPlayer, int iWorkDiscarded, int iWorkAcquired);
#endif
	void AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, CvString strValue);
	void AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, int iValue);
	void AppendToLog(CvString& strHeader, CvString& strLog, CvString strHeaderValue, float fValue);
	CvString GetLogFileName(CvString& playerName) const;

	CvPlayer *m_pPlayer;
};

FDataStream& operator>>(FDataStream&, CvPlayerCulture&);
FDataStream& operator<<(FDataStream&, const CvPlayerCulture&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:		CvCityCulture
//!  \brief		Provides summary information on culture and tourism at the city level
//
//!  Key Attributes:
//!  - This object is created inside the CvCity object and accessed through CvCity
//!  - Provides convenience functions to the other game subsystems to quickly summarize
//!    information on culture and tourism at the city level
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCityCulture
{
public:
	CvCityCulture(void);
	~CvCityCulture(void);

	void Init(CvCity* m_pCity);

#if defined(MOD_GLOBAL_GREATWORK_YIELDTYPES)
	int GetNumGreatWorks(bool bIgnoreYield = true) const;
#else
	int GetNumGreatWorks() const;
#endif
	int GetNumGreatWorkSlots() const;
	int GetNumAvailableGreatWorkSlots(GreatWorkSlotType eSlotType) const;
	int GetNumFilledGreatWorkSlots(GreatWorkSlotType eSlotType) const;
	void ClearGreatWorks();
	GreatWorkSlotType GetSlotTypeFirstAvailableCultureBuilding() const;
#if defined(MOD_BALANCE_CORE)
	void CalculateBaseTourismBeforeModifiers();
	void CalculateBaseTourism();
#endif
	int GetBaseTourismBeforeModifiers();
	int GetBaseTourism();
	int GetTourismMultiplier(PlayerTypes ePlayer, bool bIgnoreReligion, bool bIgnoreOpenBorders, bool bIgnoreTrade, bool bIgnorePolicies, bool bIgnoreIdeologies) const;

	CvString GetTourismTooltip();
	CvString GetFilledSlotsTooltip();
	CvString GetTotalSlotsTooltip();

	bool IsThemingBonusPossible(BuildingClassTypes eBuildingClass) const;
	int GetThemingBonus(BuildingClassTypes eBuildingClass) const;
	CvString GetThemingTooltip(BuildingClassTypes eBuildingClass) const;

	int GetCultureFromWonders() const;
	int GetCultureFromNaturalWonders() const;

#if defined(MOD_BALANCE_CORE)
	void UpdateThemingBonusIndex(BuildingClassTypes eBuildingClass);
#endif
	void LogGreatWorks (FILogFile* pLog);

private:
	int GetThemingBonusIndex(BuildingClassTypes eBuildingClass) const;
	CvCity *m_pCity;
};

namespace CultureHelpers
{
	GreatWorkType GetArtifact(CvPlot *pPlot);
	GreatWorkClass GetGreatWorkClass(GreatWorkType eGreatWork);
	CvString GetGreatWorkName(GreatWorkType eGreatWorkType);
	GreatWorkSlotType GetGreatWorkSlot(GreatWorkType eType);
	CvString GetGreatWorkAudio(GreatWorkType eGreatWorkType);
	int GetThemingBonusIndex(PlayerTypes eOwner, CvBuildingEntry *pkEntry, vector<int> &aGreatWorkIndices);
	bool IsValidForThemingBonus(CvThemingBonusInfo *pBonusInfo, EraTypes eEra, vector<EraTypes> &aErasSeen, PlayerTypes ePlayer, vector<PlayerTypes> &aPlayersSeen, PlayerTypes eOwner);
#if defined(MOD_BALANCE_CORE)
	bool IsValidForForeignThemingBonus(CvThemingBonusInfo *pBonusInfo, EraTypes eEra, vector<EraTypes> &aForeignErasSeen, vector<EraTypes> &aErasSeen, PlayerTypes ePlayer, vector<PlayerTypes> &aForeignPlayersSeen, vector<PlayerTypes> &aPlayersSeen, PlayerTypes eOwner);
#endif
	int FindWorkNotChosen(vector<CvGreatWorkInMyEmpire> &aWorks, vector<int> &aWorksChosen);
	void SendArtSwapNotification(GreatWorkSlotType eType, bool bArt, PlayerTypes eOriginator, PlayerTypes eReceipient, int iWorkFromOriginator, int iWorkFromRecipient);
}

#endif //CIV5_CULTURE_CLASSES_H